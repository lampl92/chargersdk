#include "tinysh.h"
#include "interface_ftpserver.h"
#include "evse_globals.h"
void cli_ftpserver_fnt(int argc, char **argv)
{
    extern void netChangeState(net_device_t *net_dev, net_state new_state);
    if (argc == 1)
    {
        netChangeState(net_dev, NET_STATE_FTPServer);
    }
    else if (argv[1][0] == '0')
    {
        netChangeState(net_dev, NET_STATE_DISCONNECT);
    }
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

