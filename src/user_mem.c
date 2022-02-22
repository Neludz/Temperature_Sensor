
//********************************** user mem ***********************************
#include <zephyr.h>
#include <sys/reboot.h>
#include <device.h>
#include <string.h>
#include <drivers/flash.h>
#include <storage/flash_map.h>
#include <fs/nvs.h>
#include <logging/log.h>
#include "user_mem.h"



//**************************************************************************
static struct nvs_fs fs;

#define STORAGE_NODE_LABEL storage

LOG_MODULE_REGISTER(nvs_mem, LOG_LEVEL_INF);
//**************************************************************************

void user_mem_init (void)
{
	/* define the nvs file system by settings with:
	 *	sector_size equal to the pagesize,
	 *	3 sectors
	 *	starting at FLASH_AREA_OFFSET(storage)
	 */
	struct flash_pages_info info;
	const struct device *flash_dev;
    int rc = 0;
	flash_dev = FLASH_AREA_DEVICE(STORAGE_NODE_LABEL);
	if (!device_is_ready(flash_dev)) {
		LOG_INF("Flash device %s is not ready\n", flash_dev->name);
		return;
	}
	fs.offset = FLASH_AREA_OFFSET(storage);
	rc = flash_get_page_info_by_offs(flash_dev, fs.offset, &info);
	if (rc) {
		LOG_INF("Unable to get page info\n");
		return;
	}
	fs.sector_size = info.size;
	fs.sector_count = 3U;

	rc = nvs_init(&fs, flash_dev->name);
	if (rc) {
		LOG_INF("Flash Init failed\n");
		return;
	}    
}
//**************************************************************************
uint32_t user_mem_read_reg16 (uint16_t addr_id, uint16_t *reg16)
{
uint16_t buf;
int rc =0;
rc = nvs_read(&fs, addr_id, &buf, sizeof(buf));
	if (rc > 0) { /* item was found, show it */
        *reg16=buf;
		//LOG_INF("Id: %d, Address: %d\n", addr_id, buf);
        return RET_MEM_OK;
	} else   {/* item was not found*/	
		LOG_INF("No address found!\n");
        return RET_MEM_NOT_FOUND;		
	}
}
//**************************************************************************
uint32_t user_mem_update_reg16 (uint16_t addr_id, uint16_t *data)
{
int ret;
ret=nvs_write(&fs, addr_id, data, sizeof(data));
return ret;
}

////*********************************** user mem *******************************