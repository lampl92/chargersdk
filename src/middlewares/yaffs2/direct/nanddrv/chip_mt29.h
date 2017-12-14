#ifndef __CHIP_MT29_H__
#define __CHIP_MT29_H__

#include "stm32f4xx.h"

typedef struct
{
    __IO uint8_t *nand_bus_area;
    uint32_t nand_bank_addr;
    int (*nand_init)(void);
}nand_priv_t;

extern struct nand_chip chip_mt29f4g08;

void chip_mt29f4g08_init(struct nand_chip *this);

#endif
