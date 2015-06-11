/*
 * Raman Power Micro Protocol Driver
 *
 * Copyright (C) 2015 Russell Robinson <rrobinson@phytec.com>
 *
 * Based on libs_pwr.c:
 * Copyright (C) 2014 Russell Robinsion <rrobinson@phytec.com>
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/* Allows sysfs access for PIC registers as specified
 * in the Raman Power Micro Protocol REV 0.1 (June 9, 2015)
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/i2c.h>
#include <linux/types.h>

static struct i2c_client *raman_client;

struct raman_pwr_data {
	struct	i2c_client	*client;

	bool			nvram_unlocked;
};

typedef struct
{
	const char	*command;
	u8		buf[3];
} raman_cmds;

typedef struct
{
	const char	*name;
	u8		reg;
	const bool	writable;
	const bool	nvram;
} raman_regs;

static raman_cmds raman_known_cmds[] = {
	{ "reset_micro",	{0xA0, 0x7A, 0xA7} },
	{ "enter_bl",		{0xA1, 0x3C, 0xC3} },
	{ "unlock_nvram",	{0xA2, 0x55, 0xAA} },
};

static raman_regs raman_known_regs[] = {
	/* read-only constants*/
	{ "protocol", 		0x00,	0,	0 },
	{ "fw_code_lw",		0x01,	0,	0 },
	{ "fw_code_hw",		0x02,	0,	0 },
	{ "version_lw",		0x03,	0,	0 },
	{ "version_hw",		0x04,	0,	0 },

	/* read-only */
	{ "status_1",		0x40,	0,	0 },
	{ "status_2",		0x41,	0,	0 },
	{ "adc1_in6",		0x42,	0,	0 },
	{ "adc1_in7",		0x43,	0,	0 },
	{ "adc1_in8",		0x44,	0,	0 },
	{ "adc1_in9",		0x45,	0,	0 },
	{ "adc1_in14",		0x46,	0,	0 },
	{ "adc1_in15",		0x47,	0,	0 },

	/* read-write */
	{ "control",		0x80,	1,	0 },
	{ "spi_bus_sel",	0x81,	1,	0 },
	{ "analog_mux_sel",	0x82,	1,	0 },

	/* read-write if nvram_unlocked, else read-only */
	{ "hw_rev",		0x84, 	1,	1 },
	{ "temp_cal",		0x85,	1,	1 },
	{ "laser_type",		0x86,	1,	1 },
	{ "serial_num_lw",	0x87,	1,	1 },
	{ "serial_num_hw",	0x88,	1,	1 },
};

static const int raman_cmd_num = sizeof(raman_known_cmds)/sizeof(raman_cmds);
static const int raman_reg_num = sizeof(raman_known_regs)/sizeof(raman_regs);

static int raman_lookup(bool cmd, char *buf)
{
	int i;

	if (cmd) {
		for (i = 0; i < raman_cmd_num; i++) {
			if (!strcmp(raman_known_cmds[i].command, buf)) {
				return i;
			}
		}
	} else {
		for (i = 0; i < raman_reg_num; i++) {
			if (!strcmp(raman_known_regs[i].name, buf)) {
				return i;
			}
		}
	}

	/* didn't find an index that matched */
	return -EINVAL;
}

static int raman_pwr_i2c_write(struct i2c_client *client,
			u8 *buf, int count)
{
	int ret;

	struct i2c_msg msg[] = {
		{
			.addr	= client->addr,
			.flags	= 0,
			.len	= count,
			.buf	= buf,
		},
	};
	
	ret = i2c_transfer(client->adapter, msg, 1);
	if (ret < 0)
		dev_err(&client->dev, "%s write error %d\n", __func__, ret);
	return ret;
}

static int raman_pwr_i2c_read(struct i2c_client *client,
			u8 *reg, int count,
			u8 *buf)
{
	int ret;

	struct i2c_msg msgs[] = {
		{
			.addr	= client->addr,
			.flags	= 0,
			.len	= 1,
			.buf	= reg,
		},
		{
			.addr	= client->addr,
			.flags	= I2C_M_RD,
			.len	= count,
			.buf	= buf,
		},
	};

	ret = i2c_transfer(client->adapter, msgs, 2);
	if (ret < 0)
		dev_err(&client->dev, "%s read error %d\n", __func__, ret);
	return ret;
}

static ssize_t raman_show_all_reg(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE,
			"Dumping all registers is currently unsupported.\n"
			"Write an individual register name to this sysfs\n"
			"node to output its contents\n");
}


static ssize_t raman_print_reg(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
	char *blank;
	char *reg_name = NULL;
	u8 data[2];
	int index;
	int buf_size, err;
	ssize_t ret = count;

	blank = strchr(buf, ' ');
	if (NULL != blank) {
		dev_err(dev, "Too many parameters. Input register name only.");
		return -EINVAL;
	}

	buf_size = sizeof(char)*strlen(buf);
	reg_name = kzalloc(buf_size, GFP_KERNEL);
	if (NULL == reg_name) {
		dev_err(dev, "Could not allocate memory for reg_name\n");
		return -ENOMEM;
	}
	strncpy(reg_name, buf, buf_size - 1);

	index = raman_lookup(0, reg_name);
	
	if (index < 0) {
		dev_err(dev, "%s is an invalid register name\n", reg_name);
		ret = -EINVAL;
		goto exit;
	} else {
		err = raman_pwr_i2c_read(raman_client,
				&(raman_known_regs[index].reg), 2,
				data);
		if (err < 0) {
			ret = -EINVAL;
			goto exit;
		}

		dev_info(dev, "%s value: 0x%.2x%.2x\n",
				reg_name, data[1], data[0]);
	}

exit:
	kfree(reg_name);

	return ret;
}

static ssize_t raman_store_reg(struct device *dev,
			struct device_attribute *attr, const char *buf,
			size_t count)
{
	struct raman_pwr_data *raman_pwr = i2c_get_clientdata(raman_client);
	char *blank, end;
	char *reg_name = NULL, *temp1 = NULL, *temp2 = NULL;
	u8 data[3];
	int size, index, err;
	ssize_t ret = count;

	/* extract the register name from the input */
	blank = strchr(buf, ' ');
	size = blank - buf;
	reg_name = kzalloc(sizeof(char)*(size + 1), GFP_KERNEL);
	if (NULL == reg_name) {
		dev_err(dev, "Could not allocate memory for reg_name\n");
		return -ENOMEM;
	}
	strncpy(reg_name, buf, size);

	/* store the data_low and data_high strings */
	temp1 = kzalloc(sizeof(char)*5, GFP_KERNEL);
	if (NULL == temp1) {
		dev_err(dev, "Could not allocate memory for data\n");
		ret = -ENOMEM;
		goto reg_name;
	}
	temp2 = kzalloc(sizeof(char)*5, GFP_KERNEL);
	if (NULL == temp2) {
		dev_err(dev, "Could not allocate memory for data\n");
		ret = -ENOMEM;
		goto temp1;
	}

	err = sscanf(++blank, "%s %s%c", temp1, temp2, &end);
	if (err != 3) {
		dev_err(dev, "Invalid inputs\n"
			"Expected [reg_name] [data_low] [data_high]\n");
		ret = -EINVAL;
		goto exit;
	}

	/* convert the data strings into u8 values */
	err = kstrtou8(temp1, 0, &data[1]);
	if (err < 0) {
		dev_err(dev, "[data_low] is not in the proper 0x## format\n");
		ret = -EINVAL;
		goto exit;
	}

	err = kstrtou8(temp2, 0, &data[2]);
	if (err < 0) {
		dev_err(dev, "[data_low] is not in the proper 0x## format\n");
		ret = -EINVAL;
		goto exit;
	}

	/* find register name in the lookup table */
	index = raman_lookup(0, reg_name);
	
	/* write selectively based on writable and nvram flags */
	if (index < 0) {
		dev_err(dev, "%s is an invalid register name\n", reg_name);
		ret = -EINVAL;
		goto exit;
	} else if (!raman_known_regs[index].writable) {
		dev_err(dev, "%s is read-only\n", reg_name);
		ret = -EINVAL;
		goto exit;
	} else if (raman_known_regs[index].nvram &&
			!raman_pwr->nvram_unlocked) {
		dev_err(dev, "%s requires the NVRAM to be unlocked "
				"before writing is possible\n", reg_name);
		ret = -EINVAL;
		goto exit;
	} else {
		data[0] = raman_known_regs[index].reg;
		raman_pwr_i2c_write(raman_client, data, 3);
	
		goto exit;
	}

exit:
	kfree(temp2);
temp1:
	kfree(temp1);
reg_name:
	kfree(reg_name);

	return ret;
}

static ssize_t raman_trig_cmd(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count)
{
	struct raman_pwr_data *raman_pwr = i2c_get_clientdata(raman_client);
	char *blank;
	char *cmd = NULL;
	int index;
	int buf_size, err;
	ssize_t ret = count;

	blank = strchr(buf, ' ');
	if (NULL != blank) {
		dev_err(dev, "Too many parameters. Input register name only.");
		return -EINVAL;
	}

	buf_size = sizeof(char)*strlen(buf);
	cmd = kzalloc(buf_size, GFP_KERNEL);
	if (NULL == cmd) {
		dev_err(dev, "Could not allocate memory for cmd\n");
		return -ENOMEM;
	}
	strncpy(cmd, buf, buf_size - 1);

	index = raman_lookup(1, cmd);

	if (index < 0) {
		dev_err(dev, "%s is an invalid command\n", cmd);
		ret = -EINVAL;
		goto exit;
	} else {
		err = raman_pwr_i2c_write(raman_client,
				raman_known_cmds[index].buf, 3);
		if (err < 0) {
			ret = err;
			goto exit;
		}

		if (!strcmp("unlock_nvram", cmd)) {
			raman_pwr->nvram_unlocked = 1;
			i2c_set_clientdata(raman_client, raman_pwr);
		}
	}

exit:
	kfree(cmd);

	return ret;
}

static DEVICE_ATTR(read_reg, 0666, raman_show_all_reg, raman_print_reg);
static DEVICE_ATTR(write_reg, 0222, NULL, raman_store_reg);
static DEVICE_ATTR(cmd, 0222, NULL, raman_trig_cmd);

static int raman_setup_sysfs(struct i2c_client *client)
{
	device_create_file(&client->dev, &dev_attr_read_reg);
	device_create_file(&client->dev, &dev_attr_write_reg);
	device_create_file(&client->dev, &dev_attr_cmd);

	return 0;
}

static int raman_delete_sysfs(struct i2c_client *client)
{
	device_remove_file(&client->dev, &dev_attr_read_reg);
	device_remove_file(&client->dev, &dev_attr_write_reg);
	device_remove_file(&client->dev, &dev_attr_cmd);

	return 0;
}

static int raman_pwr_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct raman_pwr_data *raman_pwr;

	raman_pwr = kzalloc(sizeof(struct raman_pwr_data), GFP_KERNEL);
	if (!raman_pwr) {
		dev_err(&client->dev, "cannot allocate memory\n");
		return -ENOMEM;
	}

	raman_pwr->nvram_unlocked = 0;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "i2c not supported\n");
		return -EPFNOSUPPORT;
	}

	i2c_set_clientdata(client, raman_pwr);
	raman_setup_sysfs(client);

	raman_client = client;
	dev_info(&client->dev, "device probed\n");
	
	return 0;
}
static int raman_pwr_remove(struct i2c_client *client)
{
	struct raman_pwr_data *raman_pwr;

	raman_pwr = i2c_get_clientdata(client);

	raman_delete_sysfs(client);
	i2c_unregister_device(client);

	kfree(raman_pwr);

	return 0;
}

static void raman_pwr_shutdown(struct i2c_client *client)
{
	int err;

	err = raman_pwr_i2c_write(client,
                                raman_known_cmds[0].buf, 3);
        if (err < 0)
		dev_err(&client->dev, "%s: could not shutdown PIC\n", __func__);
}

static struct i2c_device_id raman_pwr_idtable[] = {
	{ "raman_pwr", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, raman_pwr_idtable);

static struct i2c_driver raman_pwr_driver = {
	.driver = {
		.name   = "raman_pwr",
	},

	.id_table   = raman_pwr_idtable,
	.probe      = raman_pwr_probe,
	.remove     = raman_pwr_remove,
	.shutdown   = raman_pwr_shutdown,
#if 0
	.suspend    = raman_pwr_suspend,
	.resume     = raman_pwr_resume,
#endif
};
module_i2c_driver(raman_pwr_driver);

MODULE_AUTHOR("Russell Robinson <rrobinson@phytec.com>");
MODULE_DESCRIPTION("Sciaps Raman Power Board Micro I2C client driver");
MODULE_LICENSE("GPL");
