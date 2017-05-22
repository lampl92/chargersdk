#include <string.h>
#include "aes.h"

/** @brief
 *
 * @param input char*
 * @param inputSize int
 * @param key const char*
            buffer of key
 * @param output char*
            buffer of output
 * @return int
                Return output length
 *
 */
int aes_encrypt(char *input, int inputSize, const char *key, char *output)
{
    //assert(NULL != input && NULL != output);
    //assert(inputSize <=1024 && inputSize > 0);
    int pendLen = 16 - inputSize % 16;
    unsigned char iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    char temp[1024] = {0};
    memcpy(temp, input, inputSize);
    memset(temp + inputSize, pendLen, pendLen);
    AES128_CBC_encrypt_buffer((uint8_t *)output, (uint8_t *)temp, inputSize + pendLen, (uint8_t *)key, iv);
    return inputSize + pendLen;
}

/** @brief
 *
 * @param input char*
 * @param key const char*
 * @param output char*
 * @param outputSize int
 * @return void
 *
 */
void aes_decrypt(char *input, const char *key, char *output, int outputSize)
{
    //assert(NULL != input && NULL != output);
    unsigned char iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int pendLen = 16 - outputSize % 16;
    AES128_CBC_decrypt_buffer((uint8_t *)output, (uint8_t *)input, outputSize + pendLen, (uint8_t *)key, iv);
    memset(output + outputSize, 0, pendLen);
}
