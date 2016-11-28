#include "includes.h"
#include "ezxml.h"

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

tinysh_cmd_t cli_testxml_cmd = {0, "testxml", "xmlΩ‚Œˆ≤‚ ‘", "[args]",
                              cli_testxml_fnt, 0, 0, 0
                             };
