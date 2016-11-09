#include "includes.h"

void cli_hello_fnt(int argc, char **argv)
{
    printf("\nhello world\n");
}

tinysh_cmd_t cli_hello_cmd={
    0,
    "hello",
    "display hello",
    0,cli_hello_fnt,
    "<cr>",0,0
    };

u16 testsram[250000] __attribute__((at(0xC0000000)));//����������

//SDRAM�ڴ����	    
void fsmc_sdram_test()
{  
	u32 i=0;  	  
	u32 temp=0;	   
	u32 sval=0;	//�ڵ�ַ0����������	  				   
	//ÿ��16K�ֽ�,д��һ������,�ܹ�д��2048������,�պ���32M�ֽ�
	for(i=0;i<32*1024*1024;i+=16*1024)
	{
		*(vu32*)(Bank5_SDRAM_ADDR+i)=temp; 
		temp++;
        printf("ÿ��16K�ֽ�,д��temp = %d, i = %d\n", temp, i);
	}
	//���ζ���֮ǰд�������,����У��		  
 	for(i=0;i<32*1024*1024;i+=16*1024) 
	{	
  		temp=*(vu32*)(Bank5_SDRAM_ADDR+i);
        printf("ÿ��16K�ֽ�,����temp = %d, i = %d\n", temp, i);
		if(i==0)
            sval=temp;
 		else if(temp<=sval)
            break;//�������������һ��Ҫ�ȵ�һ�ζ��������ݴ�.              
 	}
    printf("SDRAM ����:%dKB\r\n",(u16)(temp-sval+1)*16);//��ӡSDRAM����
}

void cli_testsdram_fnt(int argc, char **argv)
{
    u32 ts=0;
    u32 tmp;

    taskENTER_CRITICAL();
    printf("׼����������,ts = %d\n", ts);
    for(ts=0;ts<250000;ts++)
	{
		testsram[ts]=ts;//Ԥ���������
	}
    printf("��������׼�����,ts = %d\n", ts);
    printf("��ʼ��SDRAM������������\n");
    fsmc_sdram_test();
    taskEXIT_CRITICAL();
}

tinysh_cmd_t cli_testsdram_cmd={
    0,
    "testsdram",
    "test sdram",
    0,cli_testsdram_fnt,
    "<cr>",0,0
    };

