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

#include "order.h"
#include "interface.h"
#include "utils.h"

#define BYTES_LEN 1024

int littledtest()
{
    char memseg[BYTES_LEN];
    db_query_mm_t mm;
    db_op_base_t* root;
    db_tuple_t    tuple;
    int i;
    char sn[32];
    char *p;
    int id;
    int balance;
    p = sn;
    printf_safe("FILE REMOVE!!\n");
    db_fileremove("OrderLDBTest");
    vTaskDelay(10);
    init_query_mm(&mm, memseg, BYTES_LEN);
    printf_safe("CREATE TABLE!!\n");
//    parse("CREATE TABLE OrderLDBTest (id INT, temp INT, SN STRING(32), power INT,a2 INT, b2 INT, c2 INT, d2 INT, e2 INT, f2 INT, g2 INT, h2 INT, i2 INT, j2 INT);", &mm);
    parse("CREATE TABLE OrderLDBTest (OrderSN       STRING(32), \
                                  CardID        STRING(32), \
                                  Balance       INT, \
                                  CONID         INT, \
                                  StartTime     STRING(32), \
                                  StartType     INT, \
                                  LimitFee      INT, \
                                  StartPower    INT, \
                                  ServType      INT, \
                                  TotalPower    INT, \
                                  TotalPowerFee INT, \
                                  TotalServFee  INT, \
                                  TotalFee      INT, \
                                  TotalSeg      INT, \
                                  DefSegPower   INT, \
                                  DefSegFee     INT, \
                                  PayType       INT, \
                                  StopType      INT, \
                                  StopTime      STRING(32));", &mm);


    init_query_mm(&mm, memseg, BYTES_LEN);
    printf_safe("INSERT INTO OrderLDBTest!!\n");
    parse("INSERT INTO OrderLDBTest VALUES ('SN123123', 'id12323',12,1,'2017',1,2,3,4,5,6,7,8,9,1,2,3,4,'2018');", &mm); //设置全部值
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO OrderLDBTest VALUES (2, 89884,'2222222222' , 4);", &mm);
    init_query_mm(&mm, memseg, BYTES_LEN);
    parse("INSERT INTO OrderLDBTest (Balance, CONID) VALUES (2233, 3);", &mm); //设置某项值
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO OrderLDBTest (id , temp) VALUES (4, 455);", &mm);
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO OrderLDBTest (id , temp) VALUES (5, 3313);", &mm);
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO OrderLDBTest (id , temp) VALUES (6, 11);", &mm);
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO OrderLDBTest (id , temp) VALUES (7, 99996);", &mm);
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO OrderLDBTest (id , temp) VALUES (8, 6565);", &mm);
//    init_query_mm(&mm, memseg, BYTES_LEN);
//    parse("INSERT INTO OrderLDBTest (id , temp) VALUES (9, 6565);", &mm);


    init_query_mm(&mm, memseg, BYTES_LEN);
    printf_safe("SELECT * FROM OrderLDBTest!!\n");
    root = parse("SELECT * FROM OrderLDBTest;", &mm);
    //printQuery(root, &mm);
    if (root == NULL)
    {
        printf_safe("NULL root\n");
    }
    else
    {
        init_tuple(&tuple, root->header->tuple_size, root->header->num_attr, &mm);

        while (next(root, &tuple, &mm) == 1)
        {
            id = getintbyname(&tuple, "CONID", root->header);
            balance = getintbyname(&tuple, "Balance", root->header);
            p = getstringbyname(&tuple, "OrderSN", root->header);
            printf_safe("balance: %d (%d)\n", balance, id);
            printf_safe("SN %s\n", p);
        }
    }
    close_tuple(&tuple, &mm);
    closeexecutiontree(root, &mm);

//==============================================================================
    init_query_mm(&mm, memseg, BYTES_LEN);
    printf_safe("SELECT * FROM OrderLDBTest WHERE Balance > 0;!!\n");
    root = parse("SELECT * FROM OrderLDBTest WHERE Balance > 0;", &mm);
    if (root == NULL)
    {
        printf_safe("NULL root\n");
    }
    else
    {
        init_tuple(&tuple, root->header->tuple_size, root->header->num_attr, &mm);

        while (next(root, &tuple, &mm) == 1)
        {
            id = getintbyname(&tuple, "CONID", root->header);
            balance = getintbyname(&tuple, "Balance", root->header);
            p = getstringbyname(&tuple, "OrderSN", root->header);
            printf_safe("balance: %d (%d)\n", balance, id);
            printf_safe("SN %s\n", p);
        }
    }

    close_tuple(&tuple, &mm);
    closeexecutiontree(root, &mm);

    //==============================================================================
    init_query_mm(&mm, memseg, BYTES_LEN);
    printf_safe("SELECT * FROM OrderLDBTest WHERE Balance < 5000 AND CONID < 5;!!\n");

    root = parse("SELECT * FROM OrderLDBTest WHERE Balance < 5000 AND CONID < 5;", &mm);

    if (root == NULL)
    {
        printf_safe("NULL root\n");
    }
    else
    {
        init_tuple(&tuple, root->header->tuple_size, root->header->num_attr, &mm);

        while (next(root, &tuple, &mm) == 1)
        {
            id = getintbyname(&tuple, "CONID", root->header);
            balance = getintbyname(&tuple, "Balance", root->header);
            p = getstringbyname(&tuple, "SN", root->header);
            printf_safe("balance: %i (%i)\n", balance, id);
            printf_safe("SN %s\n", p);
        }
    }

    close_tuple(&tuple, &mm);
    closeexecutiontree(root, &mm);
    return 0;
}


void orderaddtest(OrderData_t *pOrder)
{
    OrderDBInsertItem(pOrder);
}
void formatdb(void)
{
    char memseg[BYTES_LEN];
    db_query_mm_t mm;
    db_op_base_t* root;
    db_tuple_t    tuple;
    int i;
    char sn[32];
    char *p;
    int id;
    int balance;
    p = sn;
    
    init_query_mm(&mm, memseg, BYTES_LEN);
    printf_safe("SELECT * FROM OrderDB!!\n");
    root = parse("SELECT * FROM OrderDB;", &mm);
//    root = parse("SELECT * FROM OrderDB WHERE CONID > 150;", &mm);
     //printQuery(root, &mm);
    if (root == NULL)
    {
        printf_safe("NULL root\n");
    }
    else
    {
        init_tuple(&tuple, root->header->tuple_size, root->header->num_attr, &mm);

        while (next(root, &tuple, &mm) == 1)
        {
            id = getintbyname(&tuple, "CONID", root->header);
            balance = getintbyname(&tuple, "Balance", root->header);
            p = getstringbyname(&tuple, "OrderSN", root->header);
            printf_safe("balance: %d (%d)\n", balance, id);
            printf_safe("SN %s\n", p);
        }
        close_tuple(&tuple, &mm);
        closeexecutiontree(root, &mm);
    }

}

static void cli_testdb_fnt(int argc, char **argv)
{
    int i;
    int start;
    int diff;
    char ucOrderSN[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x2D, 0x80};
    OrderData_t order;
    OrderCreate(&order);
    OrderInit(&order);
    db_fileremove("OrderDB");
    OrderDBCreate();
    start = time(NULL);
    for(i = 0; i < 100; i++)
    {
        order.dBalance = 1222.33+i;
        order.ucCONID = 1;
        HexToStr(ucOrderSN, order.strOrderSN, 8);
        order.tStartTime = time(NULL)+i;
        order.tStopTime = order.tStartTime +i;
        order.dTotalFee = 100.22+i;
        order.dTotalPower = 123.32+i;
        orderaddtest(&order);
    }
    diff = time(NULL) - start;
    printf_safe("一共耗时 ：%d\n", diff);
    formatdb();

//        littledtest();

}

tinysh_cmd_t cli_testdb_cmd = {0, "testdb", "database test", "[args]",
                               cli_testdb_fnt, 0, 0, 0
                              };
