#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/of.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/power/libs_battery.h>
#include <linux/platform_data/android_battery.h>

static enum PROCESS {
	VOLTAGE,
	CAPACITY,
} target;

struct libs_battery_data {
	struct libs_bat_platform_data	*pdata;
	struct device			*dev;
};

static struct libs_battery_data *libs_bat;

/* begin external battery functions */
int libs_bat_get_voltage_now(void)
{
	return libs_bat->pdata->voltage;
}

int libs_bat_get_capacity(void)
{
	return (int) libs_bat->pdata->capacity;
}

int libs_bat_poll_charge_source(void)
{
	if (libs_bat->pdata->voltage >= 0x4E20) {
		return CHARGE_SOURCE_AC;
	} else {
		return CHARGE_SOURCE_NONE;
	}
}
/* end external battery functions */

static void process_store(struct device *dev,
			const char *buf, int select)
{
	char * temp = NULL;
        char end;
        int res, ret, size;

        /* handle integer input */
        size = strlen(buf);
        temp = kzalloc(sizeof(char) * size, GFP_KERNEL);
        if (NULL == temp) {
                dev_err(dev, "Could not allocate memory for target\n");
                return;
        }

        res = sscanf(buf, "%s%c", temp, &end);
        if (res != 2) {
                dev_err(dev, "Invalid input. "
                "Expected [integer]\n");
		return;
        }

        /* convert the input string into uint */
	switch (select)
	{
		case VOLTAGE:
			ret = kstrtouint(temp, 0, &(libs_bat->pdata->voltage));
			break;
		case CAPACITY:
			ret = kstrtouint(temp, 0, &(libs_bat->pdata->capacity));
			break;
		default:
			ret = -1;
	}

        if (ret < 0) {
                dev_err(libs_bat->dev, "Input is not a number\n");
                return;
        }
}

static ssize_t libs_show_voltage(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE,
		"%d\n", libs_bat->pdata->voltage);
}

static ssize_t libs_store_voltage(struct device *dev,
                        struct device_attribute *attr,
                        const char *buf, size_t count)
{
	process_store(dev, buf, VOLTAGE);

	return count;
}

static ssize_t libs_show_capacity(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	return scnprintf(buf, PAGE_SIZE,
		"%d\n", libs_bat->pdata->capacity);
}

static ssize_t libs_store_capacity(struct device *dev,
                        struct device_attribute *attr,
                        const char *buf, size_t count)
{
	process_store(dev, buf, CAPACITY);

	return count;
}

static DEVICE_ATTR(voltage, 0666, libs_show_voltage, libs_store_voltage);
static DEVICE_ATTR(capacity, 0666, libs_show_capacity, libs_store_capacity);

static int libs_setup_sysfs(void)
{
	device_create_file(libs_bat->dev, &dev_attr_voltage);
	device_create_file(libs_bat->dev, &dev_attr_capacity);

	return 0;
}

static int libs_delete_sysfs(void)
{
	device_remove_file(libs_bat->dev, &dev_attr_voltage);
	device_remove_file(libs_bat->dev, &dev_attr_capacity);

	return 0;
}

static int libs_bat_probe(struct platform_device *pdev)
{
	struct libs_bat_platform_data *pdata = dev_get_platdata(&pdev->dev);
	int ret = 0;

	libs_bat = kzalloc(sizeof(*libs_bat), GFP_KERNEL);
	if (!libs_bat) {
		return -ENOMEM;
	}

	libs_bat->pdata = pdata;
	if (!libs_bat->pdata) {
		dev_err(libs_bat->dev, "%s : No platform data\n", __func__);
		ret = -EINVAL;
		goto err_pdata;
	}

	libs_bat->dev = &pdev->dev;
	platform_set_drvdata(pdev, libs_bat);
	
	libs_setup_sysfs();
	dev_info(libs_bat->dev, "device probed\n");

	return 0;

err_pdata:
	kfree(libs_bat);
	
	return ret;
}
static int libs_bat_remove(struct platform_device *pdev)
{
	libs_bat = platform_get_drvdata(pdev);

	libs_delete_sysfs();
	kfree(libs_bat);

	return 0;
}

static struct platform_driver libs_bat_driver = {
	.driver = {
		.name   = "libs-battery",
		.owner = THIS_MODULE
	},
	.probe      = libs_bat_probe,
	.remove     = libs_bat_remove,
};

static int __init libs_bat_init(void)
{
	return platform_driver_register(&libs_bat_driver);
}

static void __exit libs_bat_exit(void)
{
	platform_driver_unregister(&libs_bat_driver);
}
module_init(libs_bat_init);
module_exit(libs_bat_exit);

MODULE_AUTHOR("Russell Robinson <rrobinson@phytec.com>");
MODULE_DESCRIPTION("Sciaps LIBs Battery Workaround Driver");
MODULE_LICENSE("GPL");
