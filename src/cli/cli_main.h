/**
* @file cli_main.h
* @brief
* @author rgw
* @version v1.0
* @date 2016-11-04
*/

#ifndef CLI_MAIN_H
#define CLI_MAIN_H

#include "tinysh.h"

void cli_main(void);
extern tinysh_cmd_t cli_parseJson_cmd;
extern tinysh_cmd_t cli_makeJson_cmd;
extern tinysh_cmd_t cli_aestest_cmd;
extern tinysh_cmd_t cli_evseinfo_cmd;
extern tinysh_cmd_t cli_evsestatus_cmd;
extern tinysh_cmd_t cli_evseorder_cmd;
extern tinysh_cmd_t cli_gprs_cmd;
extern tinysh_cmd_t cli_modeminfo_cmd;
extern tinysh_cmd_t cli_testdb_cmd;
extern tinysh_cmd_t cli_protoinfo_cmd;
extern tinysh_cmd_t cli_crctest_cmd;
extern tinysh_cmd_t cli_evselog_cmd;
extern tinysh_cmd_t cli_ymodem_cmd;
extern tinysh_cmd_t cli_yaffs2test_cmd;
extern tinysh_cmd_t cli_ls_cmd;
extern tinysh_cmd_t cli_file_cmd;
extern tinysh_cmd_t cli_cat_cmd;
extern tinysh_cmd_t cli_rm_cmd;
extern tinysh_cmd_t cli_reboot_cmd;
extern tinysh_cmd_t cli_format_cmd;
extern tinysh_cmd_t cli_set_crc32_cmd;
extern tinysh_cmd_t cli_networkinfo_cmd;
extern tinysh_cmd_t cli_ftpserver_cmd;
extern tinysh_cmd_t cli_unzip_cmd;
extern tinysh_cmd_t cli_zip_cmd;



#endif
