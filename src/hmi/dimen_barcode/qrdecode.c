#include "qrdecode.h"
#include "malloc.h"

//�ڴ����ú���
void qr_memset(void *p,uint8_t c,uint32_t len)
{
	mymemset(p,c,len);
}
//�ڴ����뺯��
void *qr_malloc(uint32_t size)
{
	return mymalloc(SRAMIN,size);
}
//�ڴ������뺯��
void *qr_realloc(void *ptr,uint32_t size)
{
	return myrealloc(SRAMIN,ptr,size);
}
//�ڴ��ͷź���
void qr_free(void *ptr)
{
	myfree(SRAMIN,ptr);
}
//�ڴ渴�ƺ���
void qr_memcpy(void *des,void *src,uint32_t n)
{
	mymemcpy((uint8_t*)des,(uint8_t*)src,n);
}
//���ʹ����UTF2OEM֧��,����Ҫʵ�ֽ�UTF8ת����UNICODEת���OEM
#if ATK_QR_UTF82OEM_SUPPORT
#include "ff.h"
//��UNICODE����ת����OEM����
//unicode:UNICODE������ַ�����
//����ֵ:OEM���뷽ʽ���ַ�����
uint16_t qr_convert(uint16_t unicode)
{
	return ff_convert(unicode,0);
}
#endif

