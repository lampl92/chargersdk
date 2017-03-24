/**
* @file D:\Documents\Projects\chargersdk\src\cli\cli_aes.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-17
*/
#include "includes.h"
#include "aes.h"

//same as function rfc3686_inc
void ctr_inc(unsigned char ctr_buf[16])
{
    if(!(++(ctr_buf[15])))
        if(!(++(ctr_buf[14])))
            if(!(++(ctr_buf[13])))
            {
                ++(ctr_buf[12]);
            }
}

//same as function rfc3686_init
//4Bytes nounce+8Bytes iv+4Bytes counter
void ctr_init( unsigned char nonce[4], unsigned char iv[8], unsigned char ctr_buf[16])
{
    memcpy(ctr_buf, nonce, 4);
    memcpy(ctr_buf +  4, iv, 8);
    memset(ctr_buf + 12, 0, 4);
    ctr_inc(ctr_buf);
}

void print_hex(uint8_t *buf, uint32_t len)
{
    //Print results:
    for(int i = 0; i < len; i++)
    {
        printf_safe("%02X", buf[i]);
        if(15 == i % 16)
        {
            printf_safe("\n");
        }
    }
    printf_safe("\n");
}

void aesmain()
{

    uint8_t key[]   = {0x10, 0xa5, 0x88, 0x69, 0xd7, 0x4b, 0xe5, 0xa3, 0x74, 0xcf,   0x86, 0x7c, 0xfb, 0x47, 0x38, 0x59}; //AES::DEFAULT_KEYLENGTH
    uint8_t buf[16];//tmp buffer

    uint8_t msg[]   = "HelloWorld!23456asdfasdfasdfasdfasdfÖÐ¹ú";
    uint32_t    fsize = strlen((char *)msg); //message size
    uint8_t    *DataBuf;//Data Buffer
    DataBuf = (uint8_t *)malloc(1024);

    //AES with CBC
    printf_safe("AES with CBC\n");

    //Copy data
    memset(DataBuf, 0, 1024);
    strcpy((char *)DataBuf, (char *)msg);
    uint8_t *pDataBuf = DataBuf;//tmp pointer
    uint8_t iv1[]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    uint32_t    iEncryptTimes = fsize / 16 + 1;
    uint8_t iPaddings     = 16 - fsize % 16; //Padding size
    uint32_t    newlen = fsize + iPaddings; //New length

    uint32_t i;
    //memcpy(DataBuf,iv,16);//Save iv
    memset(pDataBuf + fsize, iPaddings, iPaddings); //Padding
    printf_safe("input	=\n");
    print_hex(DataBuf, newlen);
    printf_safe("%s\n", DataBuf);
    aes_encrypt_ctx en_ctx[1];//Init encrypt

    //Encrypt
    for(i = 0; i < iEncryptTimes; i++)
    {
        aes_encrypt_key128(key, en_ctx);
        aes_cbc_encrypt(pDataBuf, buf, 16, iv1, en_ctx); //iv has been changed, ctx has been changed!!!
        memcpy(pDataBuf, buf, 16);
        pDataBuf += 16;
    }

    printf_safe("encrypt	=\n");
    print_hex(DataBuf, newlen);

    //Decrypt
    pDataBuf = DataBuf;
    uint8_t iv2[]       = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t buf3[256] = {'\0'};
    aes_decrypt_ctx de_ctx[1];
    aes_decrypt_key128(key, de_ctx);
    aes_cbc_decrypt(pDataBuf, buf3, newlen, iv2, de_ctx);

    printf_safe("decrypt	=\n");
    print_hex(buf3, newlen);
    printf_safe("%s\n", buf3);

    //================================

    printf_safe("AES with CTR\n");
    //Copy data
    memset(DataBuf, 0, 1024);
    strcpy((char *)DataBuf, (char *)msg);
    pDataBuf = DataBuf;//tmp pointer
    uint8_t iv3[]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    iEncryptTimes = fsize / 16;
    uint8_t iRemain       = fsize % 16;
    uint8_t ctr_buf[AES_BLOCK_SIZE];

    //Save iv(as ctrl buffer)
    //memcpy(DataBuf,iv,16);
    printf_safe("input	=\n");
    print_hex(DataBuf, fsize);
    printf_safe("%s\n", DataBuf);
    //Init encrypt
    //aes_encrypt_ctx en_ctx[1];

    //Encrypt
    for(i = 0; i < iEncryptTimes; i++)
    {
        aes_encrypt_key128(key, en_ctx);
        ctr_init(iv3, iv3 + 4, ctr_buf); //we set iv as the nouce
        aes_ctr_encrypt(pDataBuf, buf, 16, ctr_buf, ctr_inc, en_ctx); //iv has been changed, ctx has been changed!!!
        memcpy(pDataBuf, buf, 16);
        pDataBuf += 16;
    }

    if(iRemain != 0) //last times
    {
        pDataBuf += i * 16;
        aes_encrypt_key128(key, en_ctx);
        ctr_init(iv3, iv3 + 4, ctr_buf); //we set iv as the nouce
        aes_ctr_encrypt(pDataBuf, buf, iRemain, ctr_buf, ctr_inc, en_ctx); //iv has been changed, ctx has been changed!!!
        memcpy(pDataBuf, buf, iRemain);
    }

    printf_safe("encrypt	=\n");
    print_hex(DataBuf, fsize);
    //Decrypt
    pDataBuf = DataBuf;
    uint8_t iv4[]       = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    //uint8_t   buf3[256]={'\0'};
    //aes_decrypt_ctx de_ctx[1];
    //aes_decrypt_key128(key,de_ctx);
    aes_encrypt_key128(key, en_ctx);
    ctr_init(iv4, iv4 + 4, ctr_buf); //we set iv as the nouce
    aes_ctr_decrypt(pDataBuf, buf3, 16, ctr_buf, ctr_inc, en_ctx);
    printf_safe("decrypt	=\n");
    print_hex(buf3, fsize);
    printf_safe("%s\n", buf3);
    printf_safe("\n");
}

static void cli_aestest_fnt(int argc, char **argv)
{
    aesmain();

}

tinysh_cmd_t cli_aestest_cmd = {0, "aestest", "aes²âÊÔ", "[args]",
                                cli_aestest_fnt, 0, 0, 0
                               };
