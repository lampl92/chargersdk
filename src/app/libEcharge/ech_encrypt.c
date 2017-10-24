#include "mbedtls/aes.h"

/** @brief aes cbc 加密
 *
 * @param input char*       明文
 * @param inputSize int     明文长度
 * @param key const char*   16位Key
 * @param output char*      密文
 * @return int              密文长度
 *
 */
int ech_aes_encrypt(char *input, int inputSize, const char *key, char *output)
{
    unsigned char iv[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; 
    char temp[1024] = { 0 };
    int pendLen;
    mbedtls_aes_context aes_ctx;
    
    memcpy(temp, input, inputSize);
    if (inputSize % 16 == 0)
    {
        pendLen = 0;
    }
    else
    {
        pendLen = 16 - inputSize % 16;
        memset(temp + inputSize, pendLen, pendLen);
    }
    
    mbedtls_aes_setkey_enc(&aes_ctx, key, 128);
    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, inputSize + pendLen, iv, temp, output);

    return inputSize + pendLen;
}
/** @brief aes cbc 解密
 *
 * @param input char*       密文
 * @param inputSize int    密文长度
 * @param key const char*   16位Key
 * @param output char*      明文
 * @return void
 *
 */
void ech_aes_decrypt(char *input, int inputSize, const char *key, char *output)
{
    unsigned char iv[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; 
    mbedtls_aes_context aes_ctx;
    
    mbedtls_aes_setkey_dec(&aes_ctx, key, 128);
    mbedtls_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, inputSize, iv, input, output);
}