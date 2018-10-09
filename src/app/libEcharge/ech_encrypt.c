#include "cipher_mode/cbc.h"
#include "cipher/aes.h"

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
    AesContext aes_ctx;
    
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
    
    aesInit(&aes_ctx, key, 16);
    cbcEncrypt(&aesCipherAlgo, &aes_ctx, iv, temp, output, inputSize + pendLen);

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
    AesContext aes_ctx;
    
    aesInit(&aes_ctx, key, 16);
    cbcDecrypt(&aesCipherAlgo, &aes_ctx, iv, input, output, inputSize);
}