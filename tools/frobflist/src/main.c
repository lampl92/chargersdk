#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "enc_dec.h"

void *utils_memfrob(void *s, size_t n)
{
    char *p = (char *) s;

    while (n-- > 0)
        *p++ ^= 42;

    return s;
}
int main(int argc, char *argv[])
{
    FILE *fp = NULL;
    FILE *fp_o = NULL;
    char *rbuff;
    char *rbuff_o;
    uint32_t fsize;
    uint32_t fsize_w;

    fp = fopen(argv[1], "rb+");
    if(fp == NULL)
    {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    rbuff = (char *)malloc(fsize * sizeof(char));
    rbuff_o = (char *)malloc(fsize * sizeof(char) + 16);
    memset(rbuff_o, 0, fsize + 16);
    fread(rbuff, 1, fsize, fp);
    if(rbuff[0] == '{' || rbuff[0] == '[')
    {
        fsize_w = aes_encrypt(rbuff, fsize, rbuff_o);
    }
    else
    {
        fsize_w = fsize;
        aes_decrypt(rbuff, rbuff_o, fsize_w);
    }

    char *path_o_ext;
    if(strstr(argv[1], "json")!=NULL)
    {
        path_o_ext = strstr(argv[1], "json");
        strcpy(path_o_ext, "nosj");
        fp_o = fopen(argv[1], "wb+");
        fwrite(rbuff_o, 1, fsize_w, fp_o);
    }
    else if(strstr(argv[1], "nosj")!=NULL)
    {
        path_o_ext = strstr(argv[1], "nosj");
        strcpy(path_o_ext, "json");
        fp_o = fopen(argv[1], "wb+");
        fsize_w = strlen(rbuff_o);
        fwrite(rbuff_o, 1, fsize_w, fp_o);
    }

    fclose(fp_o);
    fclose(fp);
    free(rbuff);
    free(rbuff_o);
    return 0;
}
