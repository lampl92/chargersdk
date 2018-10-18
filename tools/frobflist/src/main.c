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
            printf("frob1 \n");
    for(int i = 0; i< fsize_w; i++)
        printf("%c", rbuff_o[i]);

    printf("\n");
    }
    else
    {
        fsize_w = fsize;
        aes_decrypt(rbuff, rbuff_o, fsize_w);
    }

    //utils_memfrob(rbuff, fsize);
    /*
    printf("frob1 \n");
    for(int i = 0; i< fsize; i++)
        printf("%c", rbuff[i]);

    printf("\n");
    aes_decrypt(rbuff, fsize);
    printf("frob2 \n");
    for(int i = 0; i< fsize; i++)
        printf("%c", rbuff[i]);
        */

    fseek(fp, 0, SEEK_SET);
    fwrite(rbuff_o, 1, fsize_w, fp);

    fclose(fp);
    free(rbuff);
    system("pause");
    return 0;
}
