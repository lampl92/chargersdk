#include "tinysh.h"
#include "minmea.h"

void cli_gps_fnt(int argc, char **argv)
{
    minmea_main(argc, argv);
}

tinysh_cmd_t cli_gps_cmd =
{
    0,
    "gps",
    "gps",
    0,
    cli_gps_fnt,
    "<cr>",
    0,
    0
};
