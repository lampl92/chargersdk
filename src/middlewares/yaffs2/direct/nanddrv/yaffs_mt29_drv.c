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
	param->chunks_per_block = 64;
	param->n_reserved_blocks = 2;
	param->start_block = 0; // Can use block 0
	param->end_block = 4095; // Last block
	param->use_nand_ecc = 1; // use YAFFS's ECC
    param->is_yaffs2 = 1;
	param->n_caches = 10;
	param->disable_soft_del = 1;
    
    chip_mt29f4g08_init(&chip_mt29f4g08);
    yaffs_nand_install_drv(dev, &chip_mt29f4g08);

	yaffs_add_device(dev);

	return NULL;
}
