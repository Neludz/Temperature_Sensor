/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <zephyr/types.h>
#include <sys/__assert.h>
#include <drivers/disk.h>
#include <errno.h>
#include <init.h>
#include <device.h>
#include "emfat.h"

#define EMFATDISK_SECTOR_SIZE 512
#define EMFATDISK_VOLUME_SIZE (2048 * 512)


extern emfat_t emfat;


static int disk_emfat_access_status(struct disk_info *disk)
{
	return DISK_STATUS_OK;
}

static int disk_emfat_access_init(struct disk_info *disk)
{
	return 0;
}

static int disk_emfat_access_read(struct disk_info *disk, uint8_t *buff,
				uint32_t sector, uint32_t count)
{
	 emfat_read(&emfat, buff, sector, count);

	return 0;
}

static int disk_emfat_access_write(struct disk_info *disk, const uint8_t *buff,
				 uint32_t sector, uint32_t count)
{
		return 0;
}

static int disk_emfat_access_ioctl(struct disk_info *disk, uint8_t cmd, void *buff)
{
	switch (cmd) {
	case DISK_IOCTL_CTRL_SYNC:
		break;
	case DISK_IOCTL_GET_SECTOR_COUNT:
		*(uint32_t *)buff = EMFATDISK_VOLUME_SIZE / EMFATDISK_SECTOR_SIZE;
		break;
	case DISK_IOCTL_GET_SECTOR_SIZE:
		*(uint32_t *)buff = EMFATDISK_SECTOR_SIZE;
		break;
	case DISK_IOCTL_GET_ERASE_BLOCK_SZ:
		*(uint32_t *)buff  = 1U;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static const struct disk_operations emfat_disk_ops = {
	.init = disk_emfat_access_init,
	.status = disk_emfat_access_status,
	.read = disk_emfat_access_read,
	.write = disk_emfat_access_write,
	.ioctl = disk_emfat_access_ioctl,
};

static struct disk_info emfat_disk = {
	.name = "EMFAT",
	.ops = &emfat_disk_ops,
};

static int disk_emfat_init(const struct device *dev)
{
	ARG_UNUSED(dev);

	return disk_access_register(&emfat_disk);
}

SYS_INIT(disk_emfat_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
