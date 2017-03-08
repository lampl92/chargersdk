/**
* @file cli_json.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-08
*/
#include "includes.h"
#include "cJSON.h"
static void cli_testjson_create_fnt(int argc, char **argv)
{

}
static void cli_testjson_fnt(int argc, char **argv)
{

}
tinysh_cmd_t cli_testjson_cmd = {0, "testjson", "jsonΩ‚Œˆ≤‚ ‘", "[args]",
                                cli_testjson_fnt, 0, 0, 0
                               };

tinysh_cmd_t cli_testjson_create_cmd = {0, "testjsoncreate", "json¥¥Ω®≤‚ ‘", "[args]",
                                       cli_testjson_create_fnt, 0, 0, 0
                                      };
