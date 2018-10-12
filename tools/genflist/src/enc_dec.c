#include <string.h>
#include "aes.h"

/** @brief aes cbc 加密
 *
 * @param input char*       明文
 * @param inputSize int     明文长度
 * @param key const char*   16位Key
 * @param output char*      密文
 * @return int              密文长度
 *
 */
int aes_encrypt(char *input, int inputSize, const char *key, char *output)
{
    unsigned char iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char temp[1024] = {0};
    int pendLen;

    memcpy(temp, input, inputSize);
    if(inputSize % 16 == 0)
    {
        pendLen = 0;
    }
    else
    {
        pendLen = 16 - inputSize % 16;
        memset(temp + inputSize, pendLen, pendLen);
    }

    AES128_CBC_encrypt_buffer((uint8_t *)output, (uint8_t *)temp, inputSize + pendLen, (uint8_t *)key, iv);

    return inputSize + pendLen;
}

/** @brief aes cbc 解密
 *
 * @param input char*       密文
 * @param key const char*   16位Key
 * @param output char*      明文
 * @param outputSize int    密文长度
 * @return void
 *
 */
void aes_decrypt(char *input, const char *key, char *output, int outputSize)
{
    unsigned char iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    AES128_CBC_decrypt_buffer((uint8_t *)output, (uint8_t *)input, outputSize, (uint8_t *)key, iv);
}
