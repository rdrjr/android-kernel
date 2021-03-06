/*
 * OMAP54XX Power domains framework
 *
 * Copyright (C) 2009-2011 Texas Instruments, Inc.
 * Copyright (C) 2009-2011 Nokia Corporation
 *
 * Abhijit Pagare (abhijitpagare@ti.com)
 * Benoit Cousson (b-cousson@ti.com)
 * Paul Walmsley (paul@pwsan.com)
 *
 * This file is automatically generated from the OMAP hardware databases.
 * We respectfully ask that any modifications to this file be coordinated
 * with the public linux-omap@vger.kernel.org mailing list and the
 * authors above to ensure that the autogeneration scripts are kept
 * up-to-date with the file contents.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>

#include "powerdomain.h"
#include "powerdomain-private.h"

#include "prcm-common.h"
#include "prcm44xx.h"
#include "prm-regbits-54xx_es1.h"
#include "prm54xx_es1.h"
#include "prcm_mpu54xx.h"

/* core_54xx_pwrdm: CORE power domain */
static struct powerdomain core_54xx_pwrdm = {
	.name		  = "core_pwrdm",
	.voltdm		  = { .name = "core" },
	.prcm_offs	  = OMAP54XX_PRM_CORE_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.context_offs	  = OMAP54XX_RM_L3MAIN1_L3_MAIN_1_CONTEXT_OFFSET,
	.pwrsts		  = PWRSTS_RET_INA_ON,
	.pwrsts_logic_ret = PWRSTS_OFF_RET,
	.banks		  = 5,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* core_nret_bank */
		[1] = PWRSTS_OFF_RET,	/* core_ocmram */
		[2] = PWRSTS_OFF_RET,	/* core_other_bank */
		[3] = PWRSTS_OFF_RET,	/* ipu_l2ram */
		[4] = PWRSTS_OFF_RET,	/* ipu_unicache */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_OFF_RET,	/* core_nret_bank */
		[1] = PWRSTS_OFF_RET,	/* core_ocmram */
		[2] = PWRSTS_OFF_RET,	/* core_other_bank */
		[3] = PWRSTS_OFF_RET,	/* ipu_l2ram */
		[4] = PWRSTS_OFF_RET,	/* ipu_unicache */
	},
	.flags		  = PWRDM_HAS_LOWPOWERSTATECHANGE,
	.wakeup_lat = {
		/* CPUIdle and suspend controls this explicitly */
		[PWRDM_POWER_OFF] = UNSUP_STATE,
		[PWRDM_POWER_OSWR] = UNSUP_STATE,
		[PWRDM_POWER_CSWR] = UNSUP_STATE,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = UNSUP_STATE,
		[PWRDM_POWER_ON] = UNSUP_STATE,
	},
};

/* abe_54xx_pwrdm: Audio back end power domain */
static struct powerdomain abe_54xx_pwrdm = {
	.name		  = "abe_pwrdm",
	.voltdm		  = { .name = "core" },
	.prcm_offs	  = OMAP54XX_PRM_ABE_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_OFF_RET_INA_ON,
	.pwrsts_logic_ret = PWRSTS_OFF,
	.banks		  = 2,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* aessmem */
		[1] = PWRSTS_OFF_RET,	/* periphmem */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_OFF_RET,	/* aessmem */
		[1] = PWRSTS_OFF_RET,	/* periphmem */
	},
	.flags		  = PWRDM_HAS_LOWPOWERSTATECHANGE,
	.wakeup_lat = {
		[PWRDM_POWER_OFF] = 1000,
		[PWRDM_POWER_OSWR] = 600,
		[PWRDM_POWER_CSWR] = UNSUP_STATE,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = 20,
		[PWRDM_POWER_ON] = 0,
	},
};

/* coreaon_54xx_pwrdm: Always ON logic that sits in VDD_CORE voltage domain */
static struct powerdomain coreaon_54xx_pwrdm = {
	.name		  = "coreaon_pwrdm",
	.voltdm		  = { .name = "core" },
	.prcm_offs	  = OMAP54XX_PRM_COREAON_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_ON,
};

/* dss_54xx_pwrdm: Display subsystem power domain */
static struct powerdomain dss_54xx_pwrdm = {
	.name		  = "dss_pwrdm",
	.voltdm		  = { .name = "core" },
	.prcm_offs	  = OMAP54XX_PRM_DSS_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_OFF_RET_INA_ON,
	.pwrsts_logic_ret = PWRSTS_OFF,
	.banks		  = 1,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* dss_mem */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_OFF_RET,	/* dss_mem */
	},
	.flags		  = PWRDM_HAS_LOWPOWERSTATECHANGE,
	.wakeup_lat = {
		[PWRDM_POWER_OFF] = 1000,
		[PWRDM_POWER_OSWR] = 500,
		[PWRDM_POWER_CSWR] = UNSUP_STATE,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = 20,
		[PWRDM_POWER_ON] = 0,
	},
};

/* cpu0_54xx_pwrdm: MPU0 processor and Neon coprocessor power domain */
static struct powerdomain cpu0_54xx_pwrdm = {
	.name		  = "cpu0_pwrdm",
	.voltdm		  = { .name = "mpu" },
	.prcm_offs	  = OMAP54XX_PRCM_MPU_PRM_C0_INST,
	.prcm_partition	  = OMAP54XX_PRCM_MPU_PARTITION,
	.pwrsts		  = PWRSTS_OFF_RET_INA_ON,
	.pwrsts_logic_ret = PWRSTS_OFF_RET,
	.banks		  = 1,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* cpu0_l1 */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_ON,	/* cpu0_l1 */
	},
	.wakeup_lat = {
		/* CPUIdle and suspend controls this explicitly */
		[PWRDM_POWER_OFF] = UNSUP_STATE,
		[PWRDM_POWER_OSWR] = UNSUP_STATE,
		[PWRDM_POWER_CSWR] = UNSUP_STATE,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = UNSUP_STATE,
		[PWRDM_POWER_ON] = UNSUP_STATE,
	}
};

/* cpu1_54xx_pwrdm: MPU1 processor and Neon coprocessor power domain */
static struct powerdomain cpu1_54xx_pwrdm = {
	.name		  = "cpu1_pwrdm",
	.voltdm		  = { .name = "mpu" },
	.prcm_offs	  = OMAP54XX_PRCM_MPU_PRM_C1_INST,
	.prcm_partition	  = OMAP54XX_PRCM_MPU_PARTITION,
	.pwrsts		  = PWRSTS_OFF_RET_INA_ON,
	.pwrsts_logic_ret = PWRSTS_OFF_RET,
	.banks		  = 1,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* cpu1_l1 */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_ON,	/* cpu1_l1 */
	},
	.flags		  = PWRDM_HAS_FORCE_OFF,
	.wakeup_lat = {
		/* CPUIdle and suspend controls this explicitly */
		[PWRDM_POWER_OFF] = UNSUP_STATE,
		[PWRDM_POWER_OSWR] = UNSUP_STATE,
		[PWRDM_POWER_CSWR] = UNSUP_STATE,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = UNSUP_STATE,
		[PWRDM_POWER_ON] = UNSUP_STATE,
	},
};

/* emu_54xx_pwrdm: Emulation power domain */
static struct powerdomain emu_54xx_pwrdm = {
	.name		  = "emu_pwrdm",
	.voltdm		  = { .name = "wkup" },
	.prcm_offs	  = OMAP54XX_PRM_EMU_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_OFF_ON,
	.banks		  = 1,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* emu_bank */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_OFF_RET,	/* emu_bank */
	},
	.wakeup_lat = {
		[PWRDM_POWER_OFF] = UNSUP_STATE,
		[PWRDM_POWER_OSWR] = UNSUP_STATE,
		[PWRDM_POWER_CSWR] = UNSUP_STATE,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = UNSUP_STATE,
		[PWRDM_POWER_ON] = 0,
	},
};

/* mpu_54xx_pwrdm: Modena processor and the Neon coprocessor power domain */
static struct powerdomain mpu_54xx_pwrdm = {
	.name		  = "mpu_pwrdm",
	.voltdm		  = { .name = "mpu" },
	.prcm_offs	  = OMAP54XX_PRM_MPU_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.context_offs	  = OMAP54XX_RM_MPU_MPU_CONTEXT_OFFSET,
	.pwrsts		  = PWRSTS_RET_INA_ON,
	.pwrsts_logic_ret = PWRSTS_OFF_RET,
	.banks		  = 2,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* mpu_l2 */
		[1] = PWRSTS_RET,	/* mpu_ram */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_OFF_RET,	/* mpu_l2 */
		[1] = PWRSTS_OFF_RET,	/* mpu_ram */
	},
	.wakeup_lat = {
		/* CPUIdle and suspend controls this explicitly */
		[PWRDM_POWER_OFF] = UNSUP_STATE,
		[PWRDM_POWER_OSWR] = UNSUP_STATE,
		[PWRDM_POWER_CSWR] = UNSUP_STATE,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = UNSUP_STATE,
		[PWRDM_POWER_ON] = UNSUP_STATE,
	},
};

/* custefuse_54xx_pwrdm: Customer efuse controller power domain */
static struct powerdomain custefuse_54xx_pwrdm = {
	.name		  = "custefuse_pwrdm",
	.voltdm		  = { .name = "core" },
	.prcm_offs	  = OMAP54XX_PRM_CUSTEFUSE_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_OFF_ON,
	.flags		  = PWRDM_HAS_LOWPOWERSTATECHANGE,
	.wakeup_lat = {
		[PWRDM_POWER_OFF] = UNSUP_STATE,
		[PWRDM_POWER_OSWR] = UNSUP_STATE,
		[PWRDM_POWER_CSWR] = UNSUP_STATE,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = UNSUP_STATE,
		[PWRDM_POWER_ON] = 0,
	},
};

/* dsp_54xx_pwrdm: Tesla processor power domain */
static struct powerdomain dsp_54xx_pwrdm = {
	.name		  = "dsp_pwrdm",
	.voltdm		  = { .name = "mm" },
	.prcm_offs	  = OMAP54XX_PRM_DSP_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_OFF_RET_ON,
	.pwrsts_logic_ret = PWRSTS_OFF_RET,
	.banks		  = 3,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* dsp_edma */
		[1] = PWRSTS_OFF_RET,	/* dsp_l1 */
		[2] = PWRSTS_OFF_RET,	/* dsp_l2 */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_OFF_RET,	/* dsp_edma */
		[1] = PWRSTS_OFF_RET,	/* dsp_l1 */
		[2] = PWRSTS_OFF_RET,	/* dsp_l2 */
	},
	.flags		  = PWRDM_HAS_LOWPOWERSTATECHANGE,
	.wakeup_lat = {
		[PWRDM_POWER_OFF] = 1000,
		[PWRDM_POWER_OSWR] = 600,
		[PWRDM_POWER_CSWR] = 300,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = 20,
		[PWRDM_POWER_ON] = 0,
	},
};

/* cam_54xx_pwrdm: Camera subsystem power domain */
static struct powerdomain cam_54xx_pwrdm = {
	.name		  = "cam_pwrdm",
	.voltdm		  = { .name = "core" },
	.prcm_offs	  = OMAP54XX_PRM_CAM_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_OFF_INA_ON,
	.banks		  = 1,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* cam_mem */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_OFF_RET,	/* cam_mem */
	},
	.flags		  = PWRDM_HAS_LOWPOWERSTATECHANGE,
	.wakeup_lat = {
		[PWRDM_POWER_OFF] = 1000,
		[PWRDM_POWER_OSWR] = UNSUP_STATE,
		[PWRDM_POWER_CSWR] = UNSUP_STATE,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = 20,
		[PWRDM_POWER_ON] = 0,
	},
};

/* l3init_54xx_pwrdm: L3 initators pheripherals power domain  */
static struct powerdomain l3init_54xx_pwrdm = {
	.name		  = "l3init_pwrdm",
	.voltdm		  = { .name = "core" },
	.prcm_offs	  = OMAP54XX_PRM_L3INIT_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_RET_ON,
	.pwrsts_logic_ret = PWRSTS_OFF_RET,
	.banks		  = 2,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* l3init_bank1 */
		[1] = PWRSTS_OFF_RET,	/* l3init_bank2 */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_OFF_RET,	/* l3init_bank1 */
		[1] = PWRSTS_OFF_RET,	/* l3init_bank2 */
	},
	.flags		  = PWRDM_HAS_LOWPOWERSTATECHANGE | PWRDM_HAS_HDWR_SAR,
	.wakeup_lat = {
		[PWRDM_POWER_OFF] = 1000,
		[PWRDM_POWER_OSWR] = 600,
		[PWRDM_POWER_CSWR] = 300,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = 20,
		[PWRDM_POWER_ON] = 0,
	},
};

/* l4per_54xx_pwrdm: Target peripherals power domain */
static struct powerdomain l4per_54xx_pwrdm = {
	.name		  = "l4per_pwrdm",
	.voltdm		  = { .name = "core" },
	.prcm_offs	  = OMAP54XX_PRM_L4PER_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_RET_ON,
	.pwrsts_logic_ret = PWRSTS_OFF_RET,
	.banks		  = 2,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* nonretained_bank */
		[1] = PWRSTS_OFF_RET,	/* retained_bank */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_OFF_RET,	/* nonretained_bank */
		[1] = PWRSTS_OFF_RET,	/* retained_bank */
	},
	.flags		  = PWRDM_HAS_LOWPOWERSTATECHANGE,
	.wakeup_lat = {
		[PWRDM_POWER_OFF] = 1000,
		[PWRDM_POWER_OSWR] = 600,
		[PWRDM_POWER_CSWR] = 300,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = 20,
		[PWRDM_POWER_ON] = 0,
	},
};

/* gpu_54xx_pwrdm: 3D accelerator power domain */
static struct powerdomain gpu_54xx_pwrdm = {
	.name		  = "gpu_pwrdm",
	.voltdm		  = { .name = "mm" },
	.prcm_offs	  = OMAP54XX_PRM_GPU_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_OFF_INA_ON,
	.banks		  = 1,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* gpu_mem */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_OFF_RET,	/* gpu_mem */
	},
	.flags		  = PWRDM_HAS_LOWPOWERSTATECHANGE,
	.wakeup_lat = {
		[PWRDM_POWER_OFF] = 1000,
		[PWRDM_POWER_OSWR] = UNSUP_STATE,
		[PWRDM_POWER_CSWR] = UNSUP_STATE,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = 20,
		[PWRDM_POWER_ON] = 0,
	},
};

/* wkupaon_54xx_pwrdm: Wake-up power domain */
static struct powerdomain wkupaon_54xx_pwrdm = {
	.name		  = "wkupaon_pwrdm",
	.voltdm		  = { .name = "wkup" },
	.prcm_offs	  = OMAP54XX_PRM_WKUPAON_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_ON,
	.banks		  = 1,
	.pwrsts_mem_ret	= {
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_ON,	/* wkup_bank */
	},
};

/* iva_54xx_pwrdm: IVA-HD power domain */
static struct powerdomain iva_54xx_pwrdm = {
	.name		  = "iva_pwrdm",
	.voltdm		  = { .name = "mm" },
	.prcm_offs	  = OMAP54XX_PRM_IVA_INST,
	.prcm_partition	  = OMAP54XX_PRM_PARTITION,
	.pwrsts		  = PWRSTS_OFF_RET_INA_ON,
	.pwrsts_logic_ret = PWRSTS_OFF,
	.banks		  = 4,
	.pwrsts_mem_ret	= {
		[0] = PWRSTS_OFF_RET,	/* hwa_mem */
		[1] = PWRSTS_OFF_RET,	/* sl2_mem */
		[2] = PWRSTS_OFF_RET,	/* tcm1_mem */
		[3] = PWRSTS_OFF_RET,	/* tcm2_mem */
	},
	.pwrsts_mem_on	= {
		[0] = PWRSTS_OFF_RET,	/* hwa_mem */
		[1] = PWRSTS_OFF_RET,	/* sl2_mem */
		[2] = PWRSTS_OFF_RET,	/* tcm1_mem */
		[3] = PWRSTS_OFF_RET,	/* tcm2_mem */
	},
	.flags		  = PWRDM_HAS_LOWPOWERSTATECHANGE,
	.wakeup_lat = {
		[PWRDM_POWER_OFF] = 1000,
		[PWRDM_POWER_OSWR] = 500,
		[PWRDM_POWER_CSWR] = UNSUP_STATE,
		[PWRDM_POWER_RET] = UNSUP_STATE,
		[PWRDM_POWER_INACTIVE] = 20,
		[PWRDM_POWER_ON] = 0,
	},
};

/*
 * The following power domains are not under SW control
 *
 * mpuaon
 * mmaon
 */

/* As powerdomains are added or removed above, this list must also be changed */
static struct powerdomain *powerdomains_omap54xx[] __initdata = {
	&core_54xx_pwrdm,
	&abe_54xx_pwrdm,
	&coreaon_54xx_pwrdm,
	&dss_54xx_pwrdm,
	&cpu0_54xx_pwrdm,
	&cpu1_54xx_pwrdm,
	&emu_54xx_pwrdm,
	&mpu_54xx_pwrdm,
	&custefuse_54xx_pwrdm,
	&dsp_54xx_pwrdm,
	&cam_54xx_pwrdm,
	&l3init_54xx_pwrdm,
	&l4per_54xx_pwrdm,
	&gpu_54xx_pwrdm,
	&wkupaon_54xx_pwrdm,
	&iva_54xx_pwrdm,
	NULL
};

void __init omap54xx_powerdomains_init(void)
{
	pwrdm_register_platform_funcs(&omap5_pwrdm_operations);
	pwrdm_register_pwrdms(powerdomains_omap54xx);
	pwrdm_complete_init();
}
