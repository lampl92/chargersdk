#include "tinysh.h"
#include "interface_ftpserver.h"
#include "evse_globals.h"
void cli_ftpserver_fnt(int argc, char **argv)
{
    net_ftp_server_start(&eth_dev);
}

tinysh_cmd_t cli_ftpserver_cmd =
{
    0,
    "ftpserver",
    "ftp server start",
    0,
    cli_ftpserver_fnt,
    "<cr>",
    0,
    0
};

