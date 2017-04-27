#ifndef __QRDECODE_H
#define __QRDECODE_H
#include <stdint.h>
#include "includes.h"
//ATK_QR_UTF82OEM_SUPPORT:定义是否支持将UTF8编码转换为OEM编码(中文一般是指GBK编码),定义为:
//1,则输出结果为GBK编码方式的字符串;定义为0,则输出的是原始字符串(未做编码转换,可能是UTF8,也
//可能是GBK,取决于二维码的编码方式).
#define ATK_QR_UTF82OEM_SUPPORT		1

//ATK_QR_GBK_SUPPORT:定义在ATK_QR_UTF82OEM_SUPPORT==1的时候,是否支持识别GBK编码的二维码,如
//果定义为1,则程序会先判断是UTF8还是GBK,根据识别结果,再做转换.如果定义为0,则只识别UTF8编码方
//式的二维码.不识别GBK编码的二维码.
//注意:
//1,当ATK_QR_UTF82OEM_SUPPORT==0的时候,该宏定义不起作用.
//2,当发现有识别错误(UTF8识别成GBK了)的时候,可设置ATK_QR_GBK_SUPPORT==0,以更好的支持UTF8
//3,这里的GBK并不是所有GBK都支持,仅支持GB2312编码的识别.
#define ATK_QR_GBK_SUPPORT			1


//返回值定义
#define ATK_QR_OK					0					//正常
#define ATK_QR_MEM_ERR				1					//内存错误
#define ATK_QR_OTHER_ERR			2					//其他错误
#define ATK_QR_RECO_ERR				3					//无法识别


uint8_t atk_qr_init(void);									//初始化识别库
uint8_t atk_qr_decode(uint16_t bmp_width,uint16_t bmp_heigh,uint8_t *bmp,uint8_t btype,uint8_t* result);//QR解码函数
void atk_qr_destroy(void);								//结束识别
void atk_qr_memset(void *p,uint8_t c,uint32_t len);				//内存设置函数
void *atk_qr_malloc(uint32_t size);							//动态申请内存
void *atk_qr_realloc(void *ptr,uint32_t size);				//内存重申请函数
void atk_qr_free(void *ptr);							//动态释放内存
void atk_qr_memcpy(void *des,void *src,uint32_t n);			//内存复制函数
uint16_t atk_qr_convert(uint16_t unicode);						//unicode转oem
#endif



