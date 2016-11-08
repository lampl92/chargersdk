#include "includes.h"

void cli_hello_fnt(int argc, char **argv)
{
    printf("\nhello world\n");
}

tinysh_cmd_t cli_hello_cmd={
    0,
    "hello",
    "display hello",
    0,cli_hello_fnt,
    "<cr>",0,0
    };
