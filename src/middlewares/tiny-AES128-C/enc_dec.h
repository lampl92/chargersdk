///加密函数
int aes_encrypt(char* input,int inputSize,const char* key,char* output);
///解密函数
void aes_decrypt(char* input,const char* key,char* output,int outputSize);