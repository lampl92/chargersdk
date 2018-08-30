#ifndef __BSP_NAND_H
#define __BSP_NAND_H

#include "bsp_define.h"

#define DATA_AREA                  ((uint32_t)0x00000000)

#define NAND_CMD_RESET             ((uint8_t)0xFF)

#define NAND_CMD_STATUS				((uint8_t)0x70)		/* 读NAND Flash的状态字 */

#define NAND_OK   0
#define NAND_FAIL 1

uint8_t NAND_Init(void);

#endif /* __FSMC_NAND_H */
