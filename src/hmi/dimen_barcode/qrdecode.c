#include "qrdecode.h"
#include "malloc.h"

//内存设置函数
void qr_memset(void *p,uint8_t c,uint32_t len)
{
	mymemset(p,c,len);
}
//内存申请函数
void *qr_malloc(uint32_t size)
{
	return mymalloc(SRAMIN,size);
}
//内存重申请函数
void *qr_realloc(void *ptr,uint32_t size)
{
	return myrealloc(SRAMIN,ptr,size);
}
//内存释放函数
void qr_free(void *ptr)
{
	myfree(SRAMIN,ptr);
}
//内存复制函数
void qr_memcpy(void *des,void *src,uint32_t n)
{
	mymemcpy((uint8_t*)des,(uint8_t*)src,n);
}
//如果使能了UTF2OEM支持,则需要实现将UTF8转出的UNICODE转码成OEM
#if ATK_QR_UTF82OEM_SUPPORT
#include "ff.h"
//将UNICODE编码转换成OEM编码
//unicode:UNICODE编码的字符内码
//返回值:OEM编码方式的字符内码
uint16_t qr_convert(uint16_t unicode)
{
	return ff_convert(unicode,0);
}
#endif

