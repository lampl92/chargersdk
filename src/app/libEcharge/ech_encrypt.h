#ifndef ECH_ENCRYPT_H
#define ECH_ENCRYPT_H

int ech_aes_encrypt(char *input, int inputSize, const char *key, char *output);
void ech_aes_decrypt(char *input, int inputSize, const char *key, char *output);

#endif