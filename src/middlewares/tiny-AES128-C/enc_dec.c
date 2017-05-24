#include <string.h>
#include "aes.h"

/** @brief aes cbc ����
 *
 * @param input char*       ����
 * @param inputSize int     ���ĳ���
 * @param key const char*   16λKey
 * @param output char*      ����
 * @return int              ���ĳ���
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

/** @brief aes cbc ����
 *
 * @param input char*       ����
 * @param key const char*   16λKey
 * @param output char*      ����
 * @param outputSize int    ���ĳ���
 * @return void
 *
 */
void aes_decrypt(char *input, const char *key, char *output, int outputSize)
{
    unsigned char iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    AES128_CBC_decrypt_buffer((uint8_t *)output, (uint8_t *)input, outputSize, (uint8_t *)key, iv);
}
