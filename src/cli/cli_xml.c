#include "includes.h"
#include "ezxml.h"

static void cli_testxml_create_fnt(int argc, char **argv)
{
    char *xml;
    FIL xmlfile;
    uint32_t len = 0;
    ezxml_t xmlroot,xmlchild;
    xmlroot = ezxml_new("充电记录");
    xmlchild = ezxml_add_child(xmlroot, "充电时间",4);
    ezxml_set_txt(xmlchild, "2016.11.29");
    xml = ezxml_toxml(xmlroot);
    f_open(&xmlfile, "chargelog.xml", FA_CREATE_ALWAYS | FA_WRITE);
    f_write (&xmlfile, xml, strlen(xml), &len);
    f_close(&xmlfile);
    free(xml);
    
}


static void cli_testxml_fnt(int argc, char **argv)
{
    ezxml_t f1 = ezxml_parse_file("formula1.xml"), team, driver;
    const char *teamname;

    for (team = ezxml_child(f1, "team"); team; team = team->next)
    {
        teamname = ezxml_attr(team, "name");
        for (driver = ezxml_child(team, "driver"); driver; driver = driver->next)
        {
            printf("%s, %s: %s\n", ezxml_child(driver, "name")->txt, teamname,
                   ezxml_child(driver, "points")->txt);
        }
    }
    ezxml_free(f1);
}

tinysh_cmd_t cli_testxml_cmd = {0, "testxml", "xml解析测试", "[args]",
                              cli_testxml_fnt, 0, 0, 0
                             };

tinysh_cmd_t cli_testxml_create_cmd = {0, "testxmlcreate", "xml创建测试", "[args]",
                              cli_testxml_create_fnt, 0, 0, 0
                             };
