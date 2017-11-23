#include "nand_chip.h"
#include "stm32f4xx.h"
#include "bsp.h"

struct nand_chip chip_mt29f4g08;
static nand_priv_t g_m29_priv;

static void set_ale(struct nand_chip * this, int high);
static void set_cle(struct nand_chip * this, int high);

static unsigned read_cycle(struct nand_chip * this);
static void write_cycle(struct nand_chip * this, unsigned b);

static int check_busy(struct nand_chip * this);
static void idle_fn(struct nand_chip *this);
static int power_check(struct nand_chip *this);

///
int nand_hal_init(void)
{
    NAND_Init();
}
static void set_ale(struct nand_chip * this, int high)
{
    nand_priv_t *m29_priv;
    m29_priv = (nand_priv_t*)(this->private_data);
    if (high == 1)
    {
        m29_priv->nand_bus_area = (__IO uint8_t *)(m29_priv->nand_bank_addr | ADDR_AREA);
    }
    else if (high == 0)
    {
        m29_priv->nand_bus_area = (__IO uint8_t *)(m29_priv->nand_bank_addr);
    }
}
static void set_cle(struct nand_chip * this, int high)
{
    nand_priv_t *m29_priv;
    m29_priv = (nand_priv_t*)(this->private_data);
    if (high == 1)
    {
        m29_priv->nand_bus_area = (__IO uint8_t *)(m29_priv->nand_bank_addr | CMD_AREA);
    }
    else if (high == 0)
    {
        m29_priv->nand_bus_area = (__IO uint8_t *)(m29_priv->nand_bank_addr);
    }
}

static unsigned read_cycle(struct nand_chip * this)
{
    nand_priv_t *m29_priv;
    m29_priv = (nand_priv_t*)(this->private_data);
    
    return *(m29_priv->nand_bus_area);
}
static void write_cycle(struct nand_chip * this, unsigned b)
{
    nand_priv_t *m29_priv;
    m29_priv = (nand_priv_t*)(this->private_data);
    
    *(m29_priv->nand_bus_area) = (uint8_t)b;
}

static int check_busy(struct nand_chip * this)
{
    return 0;
}
static void idle_fn(struct nand_chip *this)
{
    ;
}
static int power_check(struct nand_chip *this)
{
    return 1;
}

void chip_mt29f4g08_init(struct nand_chip *this)
{
    g_m29_priv.nand_bank_addr = 0x80000000;
    g_m29_priv.nand_init = nand_hal_init;
    this->private_data = &g_m29_priv;
    
    this->set_ale = set_ale;
    this->set_cle = set_cle;
    this->read_cycle = read_cycle;
    this->write_cycle = write_cycle;
//    this->check_busy = check_busy;
//    this->idle_fn = idle_fn;
//    this->power_check = power_check;
    
    this->blocks = 4096;
    this->pages_per_block = 64;
    this->data_bytes_per_page = 2048;
    this->spare_bytes_per_page = 64;
    this->bus_width_shift = 0; //?
}