#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "crc32.h"

typedef struct _flist_t
{
    char filename[64];
    int size_byte;
    uint32_t crc32;
    char localpath[256];
    char ftpdir[256];
    int status;
}flist_t;
cJSON *GetCfgObj(char *path)
{
    FILE *fd = NULL;
    char *rbuff;
    uint32_t fsize;

    cJSON *jsCfgObj = NULL;
    fd = fopen(path, "r");
    if(fd == NULL)
    {
        return NULL;
    }
    fseek(fd, 0, SEEK_END);
    fsize = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    rbuff = (char *)malloc(fsize * sizeof(char));
    fread(rbuff, 1, fsize, fd);
    jsCfgObj = cJSON_Parse(rbuff);
    fclose(fd);
    free(rbuff);

    return jsCfgObj;
}
void get_ver_num(char *vernum)
{
    int size_ver;
    char *buff_ver;
    char *ver_pos;
    int maj,min,patch;
    FILE *fp_ver = NULL;

    fp_ver = fopen("../../../src/app/evse_version.h", "r");
    fseek(fp_ver, 0, SEEK_END);
    size_ver = ftell(fp_ver);
    fseek(fp_ver, 0, SEEK_SET);
    buff_ver = (char *)malloc(size_ver);
    fread(buff_ver, 1, size_ver, fp_ver);
    ver_pos = strstr(buff_ver, "02");
    sscanf(ver_pos, "%d_%d_%d",&maj, &min, &patch);
    sprintf(vernum, "%02d.%02d.%04d", maj, min, patch);
    free(buff_ver);
}
void gen_flist_item(char *path, flist_t *pflist)
{
    char drive[64] = {0};
    char dir[512] = {0};
    char fname[512] = {0};
    char ext[64] = {0};
    FILE *fp = NULL;

    //filename": "homeRfidStateN.dta",
    _splitpath(path, drive, dir, fname, ext);
    sprintf(pflist->filename, "%s.%s",fname, ext);
    //size_byte": 2976,
    fp = fopen(path, "r");
    fseek(fp, 0, SEEK_END);
    pflist->size_byte = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    //crc32": "1dec0d51",
    GetFileCrc32(fp, &pflist->crc32);
    //localpath": "/nand/resource/",
    //ftpdir": "02.04.0821/resource",
    //status": 1
    pflist->status = 1;
    fclose(fp);
}
void dump_flist(flist_t *pflist)
{
    printf("filename:%s\n", pflist->filename);
    printf("size:%d\n",pflist->size_byte);
    printf("crc32:%08x\n",pflist->crc32);
    printf("localpath:%s\n", pflist->localpath);
    printf("ftpdir:%s\n", pflist->ftpdir);
    printf("status:%d\n", pflist->status);
}
void addFlist(cJSON *jsFlist)
{

}
//genflist -c <binpath>
int main(int argc, char* argv[])
{
    FILE *fp_json = NULL;


    cJSON *jsFlist;
    cJSON *jsFlistItem;
    int isNewFw = 0;
    char ver_num[16] = {0};
    flist_t flist;

    for(int i = 1; i < argc; i++)
    {
        if(strstr(argv[i], "new_fw") != NULL)
        {
            isNewFw = 1;
        }
    }
    if(isNewFw == 1)
    {
        jsFlist = GetCfgObj("./flist.json");
        if(jsFlist != NULL)
        {

        }
        else
        {
            jsFlist = GetCfgObj("./flist_templete.json");
        }
    }
    else//isNewFw != 1
    {
        unlink("./flist.json");
        jsFlist = GetCfgObj("./flist_templete.json");
    }

    if(isNewFw == 1)
    {
        get_ver_num(ver_num);
        //printf("ver:%s\n", ver_num);
        gen_flist_item(argv[1], &flist);
        sprintf(flist.localpath, "/nand/upgrade/");
        sprintf(flist.ftpdir, "%s/upgrade", ver_num);
        dump_flist(&flist);
    }
    else
    {
        for(int i = 1; i < argc; i++)
        {
            gen_flist_item(argv[i], &flist);
            sprintf(flist.localpath, "/nand/resource/");
            sprintf(flist.ftpdir, "resource");
            dump_flist(&flist);
        }
    }

    printf("Hello world!\n");
    system("pause");
    return 0;
}
