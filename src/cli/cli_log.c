#include "tinysh.h"
#include "stm32f4xx.h"

#include <string.h>

uint8_t protolog;
uint8_t dePrintTime;

void cli_printlog_fnt(int argc, char **argv)
{
    int onoff;
    char item[16];
    if (argc == 2)
    {
        sscanf(argv[1], "%[a-z]%d", item, &onoff);
     
        if (strcasecmp(item, "p") == 0)
            protolog = onoff;
        if (strcasecmp(item, "t") == 0)
            dePrintTime = onoff;
    }
}

tinysh_cmd_t cli_printlog_cmd =
{
    0,
    "print",
    "p0 -> protolog off\n\
     p1 -> protolog on\n\
     t0 -> printtime off\n\
     t1 -> printtime on\n",
    0,
    cli_printlog_fnt,
    "<cr>",
    0,
    0
};