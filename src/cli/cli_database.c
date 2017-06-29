/**
* @file D:\Documents\Projects\chargersdk\src\cli\cli_database.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-06-21
*/
#include "includes.h"
#include "dbparser.h"
#include "debug.h"

#define BYTES_LEN 1024

int littledtest()
{
    char          memseg[BYTES_LEN];
    db_query_mm_t mm;
    db_op_base_t* root;
    db_tuple_t    tuple;
    int i;
    char sn[8];
    char *p;
    int id;
    int sensor_val;
    p = sn;
    DB_PRINTF_DEBUG("FILE REMOVE!!\n");
    db_fileremove("sensors");
    vTaskDelay(10);
    init_query_mm(&mm, memseg, BYTES_LEN);
    DB_PRINTF_DEBUG("CREATE TABLE!!\n");
    parse("CREATE TABLE sensors (id INT, temp INT, SN STRING(32), power INT);", &mm);
//
    init_query_mm(&mm, memseg, BYTES_LEN);
    DB_PRINTF_DEBUG("INSERT INTO sensors!!\n");
    parse("INSERT INTO sensors VALUES (1, 111322, '1111111' , 3);", &mm); //����ȫ��ֵ
    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("INSERT INTO sensors VALUES (2, 89884,'2222222222' , 4);", &mm);
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO sensors (id , temp) VALUES (3, 112);", &mm); //����ĳ��ֵ
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO sensors (id , temp) VALUES (4, 455);", &mm);
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO sensors (id , temp) VALUES (5, 3313);", &mm);
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO sensors (id , temp) VALUES (6, 11);", &mm);
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO sensors (id , temp) VALUES (7, 99996);", &mm);
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO sensors (id , temp) VALUES (8, 6565);", &mm);
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO sensors (id , temp) VALUES (9, 6565);", &mm);


    init_query_mm(&mm, memseg, BYTES_LEN);
    DB_PRINTF_DEBUG("SELECT * FROM sensors!!\n");
    root = parse("SELECT * FROM sensors;", &mm);
    if (root == NULL)
    {
        printf("NULL root\n");
    }
    else
    {
        init_tuple(&tuple, root->header->tuple_size, root->header->num_attr, &mm);

        while (next(root, &tuple, &mm) == 1)
        {
            id = getintbyname(&tuple, "id", root->header);
            sensor_val = getintbyname(&tuple, "temp", root->header);
            p = getstringbyname(&tuple, "SN", root->header);
            printf("sensor val: %i (%i)\n", sensor_val, id);
            printf("SN %s\n", p);
        }
    }
    close_tuple(&tuple, &mm);
    closeexecutiontree(root, &mm);

//==============================================================================
    init_query_mm(&mm, memseg, BYTES_LEN);
    DB_PRINTF_DEBUG("SELECT * FROM sensors WHERE temp > 5000;!!\n");
    root = parse("SELECT * FROM sensors WHERE temp > 5000;", &mm);
    if (root == NULL)
    {
        printf("NULL root\n");
    }
    else
    {
        init_tuple(&tuple, root->header->tuple_size, root->header->num_attr, &mm);

        while (next(root, &tuple, &mm) == 1)
        {
            id = getintbyname(&tuple, "id", root->header);
            sensor_val = getintbyname(&tuple, "temp", root->header);
            p = getstringbyname(&tuple, "SN", root->header);
            printf("sensor val: %i (%i)\n", sensor_val, id);
            printf("SN %s\n", p);
        }
    }

    close_tuple(&tuple, &mm);
    closeexecutiontree(root, &mm);

    //==============================================================================
    init_query_mm(&mm, memseg, BYTES_LEN);
    DB_PRINTF_DEBUG("SELECT * FROM sensors WHERE temp > 5000 AND id < 5;!!\n");

    root = parse("SELECT * FROM sensors WHERE temp > 5000 AND id < 5;", &mm);

if (root == NULL)
    {
        printf("NULL root\n");
    }
    else
    {
        init_tuple(&tuple, root->header->tuple_size, root->header->num_attr, &mm);

        while (next(root, &tuple, &mm) == 1)
        {
            id = getintbyname(&tuple, "id", root->header);
            sensor_val = getintbyname(&tuple, "temp", root->header);
            p = getstringbyname(&tuple, "SN", root->header);
            printf("sensor val: %i (%i)\n", sensor_val, id);
            printf("SN %s\n", p);
        }
    }

    close_tuple(&tuple, &mm);
    closeexecutiontree(root, &mm);
    return 0;
}


static void cli_testdb_fnt(int argc, char **argv)
{
    int i;
    for(i = 0; i < 1; i++)
    {
        littledtest();
    }
}

tinysh_cmd_t cli_testdb_cmd = {0, "testdb", "database test", "[args]",
                               cli_testdb_fnt, 0, 0, 0
                              };
