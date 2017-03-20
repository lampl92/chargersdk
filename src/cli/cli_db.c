/**
* @file D:\Documents\Projects\chargersdk\src\cli\cli_db.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-16
*/
#include "includes.h"
#include "dbparser/dbparser.h"

#define BYTES_LEN 400

int testdb(void)
{
    char memseg[BYTES_LEN];
    db_query_mm_t mm;
    db_op_base_t *root;
    db_tuple_t    tuple;
    db_int errcode;

    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("CREATE TABLE sensors3 (id int, temp int);", &mm);

    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("INSERT INTO sensors3 VALUES (1, 221);", &mm);

    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("INSERT INTO sensors3 VALUES (2, 89884);", &mm);
    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("INSERT INTO sensors3 VALUES (3, 112);", &mm);
    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("INSERT INTO sensors3 VALUES (4, 455);", &mm);
    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("INSERT INTO sensors3 VALUES (5, 3313);", &mm);
    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("INSERT INTO sensors3 VALUES (6, 11);", &mm);
    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("INSERT INTO sensors3 VALUES (7, 99996);", &mm);
    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("INSERT INTO sensors3 VALUES (8, 6565);", &mm);
    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("INSERT INTO sensors3 VALUES (9, 6565);", &mm);

    init_query_mm(&mm, memseg, BYTES_LEN);
    root = parse("SELECT * FROM sensors3;", &mm);
    if (root == NULL)
    {
        printf_safe("NULL root\n");
    }
    else
    {
        init_tuple(&tuple, root->header->tuple_size, root->header->num_attr, &mm);

        while (next(root, &tuple, &mm) == 1)
        {
            int id = getintbyname(&tuple, "id", root->header);
            int sensor_val = getintbyname(&tuple, "temp", root->header);;
            printf_safe("sensor val: %i (%i)\n", sensor_val, id);
        }
    }

    return 0;
}


static void cli_db_fnt(int argc, char **argv)
{
    testdb();
}


tinysh_cmd_t cli_db_cmd = {0, "dbtest", "db≤‚ ‘", "[args]",
                           cli_db_fnt, 0, 0, 0
                          };
