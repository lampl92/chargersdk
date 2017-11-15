/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2011 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * This is an interface module for handling NOR in yaffs1 mode.
 */

/* First set up for M18 with 1k chunks and 16-byte spares.
 *
 * NB We're using the oddball M18 modes of operation here
 * The chip is 64MB based at 0x0000, but YAFFS only going to use the top half
 * ie. YAFFS will be from 32MB to 64MB.
 *
 * The M18 has two ways of writing data. Every Programming Region (1kbytes)
 * can be programmed in two modes:
 * * Object Mode 1024 bytes of write once data.
 * * Control Mode: 512bytes of bit-writeable data.
 *    This is arranged as 32 * (16 bytes of bit-writable followed by 16 bytes of "dont touch")
 *
 * The block size is 256kB, making 128 blocks in the 32MB YAFFS area.
 * Each block comprises:
 *   Offset   0k: 248 x 1k data pages
 *   Offset 248k: 248 x 32-byte spare areas implemented as 16 bytes of spare followed by 16 bytes untouched)
 *   Offset 248k + (248 * 32): Format marker
 *
 */

#include "yaffs_mt29_drv.h"
#include "yaffs_nand_drv.h"

#include "yportenv.h"
#include "yaffs_trace.h"

#include "yaffs_flashif.h"
#include "yaffs_guts.h"


struct yaffs_dev *yaffs_mt29_install_drv(const char *name)
{

	struct yaffs_dev *dev = malloc(sizeof(struct yaffs_dev));
	char *name_copy = strdup(name);
	struct yaffs_param *param;
	struct yaffs_driver *drv;


	if(!dev || !name_copy) {
		free(name_copy);
		free(dev);
		return NULL;
	}

	param = &dev->param;
	drv = &dev->drv;

	memset(dev, 0, sizeof(*dev));

	param->name = name_copy;

	param->total_bytes_per_chunk = 2048;
	param->chunks_per_block =64;
	param->n_reserved_blocks = 2;
	param->start_block = 0; // Can use block 0
	param->end_block = 4095; // Last block
	param->use_nand_ecc = 1; // use YAFFS's ECC
	param->n_caches = 10;
	param->disable_soft_del = 1;
    
    chip_mt29f4g08_init(&chip_mt29f4g08);
    yaffs_nand_install_drv(dev, &chip_mt29f4g08);

	yaffs_add_device(dev);

	return NULL;
}
