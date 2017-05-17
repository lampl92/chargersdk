/**
* @file D:\Documents\Projects\chargersdk\src\cli\cli_aes.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-17
*/
#include "includes.h"
#include "aes.h"
#include "mbedtls/aes.h"

int tinyaestest()
{
    int i;
    unsigned char plain[32] = {0xEC, 0x5B, 0xBD, 0x91, 0xB3, 0x72, 0xBE, 0x99, 0x38, 0x31,
                               0xAD, 0x2E, 0xF4, 0xD4, 0xBB, 0x26, 0xCA, 0x63, 0xB1, 0x41,
                               0xDF, 0x7D, 0xCD, 0x47, 0xE6, 0x81, 0x21, 0x7B, 0x23, 0x54,
                               0x8D, 0x5C
                              };
    unsigned char key[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'g'};
    unsigned char dec_plain[200] = {0};

    aes_decrypt(plain, key, dec_plain, 32);
    printf_safe("\n*********** 解密后：\n");
    for(i = 0; i < 32; i++)
    {
        printf_safe("%02X ", dec_plain[i]);
    }
    printf_safe("\n");
}

int mbedtlsaesmain()
{
    int i;
    mbedtls_aes_context aes_ctx;
    //密钥数值
    unsigned char key[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'g'};

    //明文空间
    unsigned char plain[32] = {0xEC, 0x5B, 0xBD, 0x91, 0xB3, 0x72, 0xBE, 0x99, 0x38, 0x31,
                               0xAD, 0x2E, 0xF4, 0xD4, 0xBB, 0x26, 0xCA, 0x63, 0xB1, 0x41,
                               0xDF, 0x7D, 0xCD, 0x47, 0xE6, 0x81, 0x21, 0x7B, 0x23, 0x54,
                               0x8D, 0x5C
                              };
    unsigned char iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    //解密后明文的空间
    unsigned char dec_plain[200] = {0};
    //密文空间
    unsigned char cipher[200] = {0};

    mbedtls_aes_init( &aes_ctx );


    //设置加密密钥
//    mbedtls_aes_setkey_enc( &aes_ctx, key, 128);

    //printf_safe("\n*********** 加密前：%s\n", plain);
//    mbedtls_aes_crypt_cbc( &aes_ctx, MBEDTLS_AES_ENCRYPT, 32, plain, cipher );
//    printf_safe("\n*********** 加密后：%s\n", cipher);
    //设置解密密钥
    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);

    mbedtls_aes_crypt_cbc( &aes_ctx, MBEDTLS_AES_DECRYPT, 32, iv, cipher, dec_plain );
    printf_safe("\n*********** 解密后：");
    for(i = 0; i < 32; i++)
    {
        printf_safe("%02X ", dec_plain[i]);
    }
    printf_safe("\n");

    mbedtls_aes_free( &aes_ctx );

}
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


}

static void cli_aestest_fnt(int argc, char **argv)
{
    //aesmain();
    //mbedtlsaesmain();
    tinyaestest();

}

tinysh_cmd_t cli_aestest_cmd = {0, "aestest", "aes测试", "[args]",
                                cli_aestest_fnt, 0, 0, 0
                               };
