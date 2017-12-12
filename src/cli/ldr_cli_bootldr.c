#include "includes.h"
#include "task.h"
#include "interface.h"
#include "utils.h"
#include "sysinit.h"
#include "stringName.h"
#include "cfg_parse.h"

void cli_set_crc32_fn(int argc, char **argv)
{
    ul2uc crc32;    
    uint8_t bin = 1;
    double dcrc32;
    if (argc == 1)
    {
        printf_safe("\n\nupgrade_flag = %d\n", xSysconf.xUpFlag.chargesdk_bin);
        printf_safe(    "bin_crc32    = %x\n", (uint32_t)xSysconf.xUpFlag.chargesdk_bin_crc32);
    }
    if (argc == 2)
    {
        if (argv[1][0] == '0' && argv[1][1] == '0')
        {
            bin = 0;
            xSysconf.SetSysCfg(jnSysChargersdk_bin, &bin, ParamTypeU8);
            xSysconf.GetSysCfg(&xSysconf, NULL);
            return ;
        }
        if (argv[1][0] == '0' && argv[1][1] == '1' && argv[1][2] == '\0')
        {
            bin = 1;
            xSysconf.SetSysCfg(jnSysChargersdk_bin, &bin, ParamTypeU8);
            xSysconf.GetSysCfg(&xSysconf, NULL);
            return ;
        }
        StrToHex(argv[1] + 2, crc32.ucVal, strlen(argv[1] + 2));
        crc32.ulVal = utils_ntohl(crc32.ulVal);
        dcrc32 = (double)crc32.ulVal;
        xSysconf.SetSysCfg(jnSysChargersdk_bin_crc32, (void *)&dcrc32, ParamTypeDouble);
        xSysconf.SetSysCfg(jnSysChargersdk_bin, &bin, ParamTypeU8);
        xSysconf.GetSysCfg(&xSysconf, NULL);
    }
}
tinysh_cmd_t cli_set_crc32_cmd =
{
    0,
    "sc",
    "set crc32 for chargesdk.bin",
    0,
    cli_set_crc32_fn,
    "<cr>",
    0,
    0
};

extern uint32_t initstart;
void cli_exit_fn(int argc, char **argv)
{
    initstart = 1;
}
tinysh_cmd_t cli_exit_cmd =
{
    0,
    "exit",
    "exit cli",
    0,
    cli_exit_fn,
    "<cr>",
    0,
    0
};

