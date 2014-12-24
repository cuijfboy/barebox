/*
 * pfla03 - phyFLEX-AM335x Board Initalization Code
 *
 * Copyright (C) 2014 Stefan Müller-Klieser, Phytec Messtechnik GmbH
 *
 * Based on arch/arm/boards/omap/board-beagle.c
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <bootsource.h>
#include <common.h>
#include <nand.h>
#include <init.h>
#include <io.h>
#include <sizes.h>
#include <envfs.h>
#include <asm/armlinux.h>
#include <generated/mach-types.h>
#include <linux/phy.h>
#include <linux/micrel_phy.h>
#include <mach/am33xx-generic.h>
#include <mach/am33xx-silicon.h>
#include <mach/bbu.h>

static int ksz9031rn_phy_fixup(struct phy_device *dev)
{
	phy_write_mmd_indirect(dev, 6, 2, 0);
	phy_write_mmd_indirect(dev, 8, 2, 0x003ff);

	return 0;
}

static int pfla03_coredevice_init(void)
{
	if (!of_machine_is_compatible("phytec,phyflex-am335x-som"))
		return 0;

	phy_register_fixup_for_uid(PHY_ID_KSZ9031, MICREL_PHY_ID_MASK,
					ksz9031rn_phy_fixup);
	am33xx_register_ethaddr(0, 0);
	am33xx_register_ethaddr(1, 1);

	return 0;
}
coredevice_initcall(pfla03_coredevice_init);

static struct omap_barebox_part pfla03_barebox_part = {
	.nand_offset = SZ_512K,
	.nand_size = SZ_512K,
	.nor_offset = SZ_128K,
	.nor_size = SZ_512K,
};

static char *xloadslots[] = {
	"/dev/nand0.xload.bb",
	"/dev/nand0.xload_backup1.bb",
	"/dev/nand0.xload_backup2.bb",
	"/dev/nand0.xload_backup3.bb"
};

static int pfla03_devices_init(void)
{
	if (!of_machine_is_compatible("phytec,phyflex-am335x-som"))
		return 0;

	switch (bootsource_get()) {
	case BOOTSOURCE_SPI:
		of_device_enable_path("/chosen/environment-spi");
		break;
	case BOOTSOURCE_MMC:
		omap_set_bootmmc_devname("mmc0");
		break;
	default:
		of_device_enable_path("/chosen/environment-nand");
		break;
	}

	omap_set_barebox_part(&pfla03_barebox_part);
	armlinux_set_architecture(MACH_TYPE_PFLA03);
	defaultenv_append_directory(defaultenv_phyflex_am335x);

	am33xx_select_rmii2_crs_dv();

	am33xx_bbu_spi_nor_mlo_register_handler("MLO.spi", "/dev/m25p0.xload");
	am33xx_bbu_spi_nor_register_handler("spi", "/dev/m25p0.barebox");
	am33xx_bbu_nand_xloadslots_register_handler("MLO.nand",
		xloadslots, ARRAY_SIZE(xloadslots));
	am33xx_bbu_nand_register_handler("nand", "/dev/nand0.barebox.bb");

	return 0;
}
device_initcall(pfla03_devices_init);
