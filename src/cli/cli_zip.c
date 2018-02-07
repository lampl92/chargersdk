#include "tinysh.h"

void cli_unzip_fnt(int argc, char **argv)
{
    miniunz_main(argc, argv);
}

tinysh_cmd_t cli_unzip_cmd =
{
    0,
    "miniunz",
    "miniunz",
    0,
    cli_unzip_fnt,
    "<cr>",
    0,
    0
};

