#include "tinysh.h"
#include <stdio.h>
#include <sqlite3.h>
#include "retarget.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        printf_safe("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf_safe("\n");
    return 0;
}

int cli_sqlite3_main(int argc, char **argv) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    if (argc != 3) {
        printf_safe("Usage: %s DATABASE SQL-STATEMENT\n", argv[0]);
        return (1);
    }
    //rc = sqlite3_open(argv[1], &db);
    rc = sqlite3_open("/nand/system/test.db", &db);
    if (rc) {
        printf_safe("Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return (1);
    }
//    rc = sqlite3_exec(db, argv[2], callback, 0, &zErrMsg);
    rc = sqlite3_exec(db, "create table MyTable_1( ID integer primary key autoincrement, name nvarchar(32) ) ", callback, 0, &zErrMsg);
    for (int i = 0; i < 1000; i++)
    {
        printf_safe("%d\r",i);
    rc = sqlite3_exec(db, "insert into MyTable_1( name ) values ( 'walk' )", callback, 0, &zErrMsg);
    rc = sqlite3_exec(db, "insert into MyTable_1( name ) values ( 'bike')  ", callback, 0, &zErrMsg);
    rc = sqlite3_exec(db, "insert into MyTable_1( name ) values ( ' bus' ) ", callback, 0, &zErrMsg);
    rc = sqlite3_exec(db, "insert into MyTable_1( name ) values ( 'subway' ) ", callback, 0, &zErrMsg);
        
    }
    rc = sqlite3_exec(db, "select * from MyTable_1", callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        printf_safe("SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    sqlite3_close(db);
    return 0;
}

tinysh_cmd_t cli_sqlite3_cmd =
{
    0,
    "testsqlite",
    "test sqlite",
    0,
    cli_sqlite3_main,
    "<cr>",
    0,
    0
};