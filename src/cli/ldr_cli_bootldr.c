#include "includes.h"
#include "task.h"
#include "interface.h"

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

