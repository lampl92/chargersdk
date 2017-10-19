/**
* @file D:\Documents\Projects\chargersdk\src\cli\cli_aes.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-17
*/
#include "includes.h"
#include "aes.h"
#include "libEcharge/ech_encrypt.h"
#include "enc_dec.h"

    //密钥数值
static unsigned char key[16] = "0123456789abcdeg"; 
    //明文空间
static unsigned char plain[] = {
    0x59,
    0xe5,
    0x67,
    0xef
};
static unsigned char iv[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//解密后明文的空间
static unsigned char dec_plain[200] = { 0 };
unsigned char enc_plain[200] = { 0 };
//密文空间
unsigned char cipher[] = { 0x60, 0x62, 0x53, 0xCD, 0xF5, 0x7C, 0x2A, 0xBC, 0x51, 0x34, 0xBC, 0x53, 0xBF, 0x52, 0x39, 0xC4 };


int tinyaestest()
{
    int i;
    uint32_t en_len;

    printf_safe("***tinyaes***\n");
    en_len = aes_encrypt(plain, sizeof(plain), key, enc_plain);
  
    printf_safe("\n*********** 加密后：\n");
    for (i = 0; i < en_len; i++)
    {
        printf_safe("%02X ", enc_plain[i]);
    }
    printf_safe("\n");
    //设置解密密钥


    aes_decrypt(cipher, key, dec_plain, sizeof(cipher));
    printf_safe("\n*********** 解密后：");
    for (i = 0; i < sizeof(cipher); i++)
    {
        printf_safe("%02X ", dec_plain[i]);
    }
    printf_safe("\n");
}

int mbedtlsaesmain()
{
    int i;
    uint32_t en_len;

    printf_safe("***mbedtls***\n");
    en_len = ech_aes_encrypt(plain, sizeof(plain), key, enc_plain);
  
    printf_safe("\n*********** 加密后：\n");
    for (i = 0; i < en_len; i++)
    {
        printf_safe("%02X ", enc_plain[i]);
    }
    printf_safe("\n");
    //设置解密密钥


    ech_aes_decrypt(cipher, sizeof(cipher), key, dec_plain);
    printf_safe("\n*********** 解密后：");
    for (i = 0; i < sizeof(cipher); i++)
    {
        printf_safe("%02X ", dec_plain[i]);
    }
    printf_safe("\n");
}


static void cli_aestest_fnt(int argc, char **argv)
{
    int i;
    clock_t start;
    start = clock();
    for (i = 0; i < 10000; i++)
    {
//        mbedtlsaesmain();
    tinyaestest();
        
    }
    printf_safe("mbed cost %d\n", clock() - start);
//    start = clock();
//    printf_safe("tiny cost %d\n", clock() - start);
    

}

tinysh_cmd_t cli_aestest_cmd = {0, "aestest", "aes测试", "[args]",
                                cli_aestest_fnt, 0, 0, 0
                               };
