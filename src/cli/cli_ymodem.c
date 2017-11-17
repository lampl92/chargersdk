#include "ymodem.h"
#include "tinysh.h"

extern rym_err_t rym_write_to_file(void);
void cli_ymodem_fnt(int argc, char **argv)
{
    rym_write_to_file();
}

tinysh_cmd_t cli_xmodem_cmd =
{
    0,
    "ry",
    "ymodem trans",
    0,
    cli_ymodem_fnt,
    "-r<cr>",
    0,
    0
};



