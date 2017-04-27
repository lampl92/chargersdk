#ifndef __QRDECODE_H
#define __QRDECODE_H
#include <stdint.h>
#include "includes.h"
//ATK_QR_UTF82OEM_SUPPORT:�����Ƿ�֧�ֽ�UTF8����ת��ΪOEM����(����һ����ָGBK����),����Ϊ:
//1,��������ΪGBK���뷽ʽ���ַ���;����Ϊ0,���������ԭʼ�ַ���(δ������ת��,������UTF8,Ҳ
//������GBK,ȡ���ڶ�ά��ı��뷽ʽ).
#define ATK_QR_UTF82OEM_SUPPORT		1

//ATK_QR_GBK_SUPPORT:������ATK_QR_UTF82OEM_SUPPORT==1��ʱ��,�Ƿ�֧��ʶ��GBK����Ķ�ά��,��
//������Ϊ1,���������ж���UTF8����GBK,����ʶ����,����ת��.�������Ϊ0,��ֻʶ��UTF8���뷽
//ʽ�Ķ�ά��.��ʶ��GBK����Ķ�ά��.
//ע��:
//1,��ATK_QR_UTF82OEM_SUPPORT==0��ʱ��,�ú궨�岻������.
//2,��������ʶ�����(UTF8ʶ���GBK��)��ʱ��,������ATK_QR_GBK_SUPPORT==0,�Ը��õ�֧��UTF8
//3,�����GBK����������GBK��֧��,��֧��GB2312�����ʶ��.
#define ATK_QR_GBK_SUPPORT			1


//����ֵ����
#define ATK_QR_OK					0					//����
#define ATK_QR_MEM_ERR				1					//�ڴ����
#define ATK_QR_OTHER_ERR			2					//��������
#define ATK_QR_RECO_ERR				3					//�޷�ʶ��


uint8_t atk_qr_init(void);									//��ʼ��ʶ���
uint8_t atk_qr_decode(uint16_t bmp_width,uint16_t bmp_heigh,uint8_t *bmp,uint8_t btype,uint8_t* result);//QR���뺯��
void atk_qr_destroy(void);								//����ʶ��
void atk_qr_memset(void *p,uint8_t c,uint32_t len);				//�ڴ����ú���
void *atk_qr_malloc(uint32_t size);							//��̬�����ڴ�
void *atk_qr_realloc(void *ptr,uint32_t size);				//�ڴ������뺯��
void atk_qr_free(void *ptr);							//��̬�ͷ��ڴ�
void atk_qr_memcpy(void *des,void *src,uint32_t n);			//�ڴ渴�ƺ���
uint16_t atk_qr_convert(uint16_t unicode);						//unicodeתoem
#endif



