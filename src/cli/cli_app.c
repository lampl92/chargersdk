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

u16 testsram[250000] __attribute__((at(0xC0000000)));//测试用数组

//SDRAM内存测试	    
void fsmc_sdram_test()
{  
	u32 i=0;  	  
	u32 temp=0;	   
	u32 sval=0;	//在地址0读到的数据	  				   
	//每隔16K字节,写入一个数据,总共写入2048个数据,刚好是32M字节
	for(i=0;i<32*1024*1024;i+=16*1024)
	{
		*(vu32*)(Bank5_SDRAM_ADDR+i)=temp; 
		temp++;
        printf("每隔16K字节,写入temp = %d, i = %d\n", temp, i);
	}
	//依次读出之前写入的数据,进行校验		  
 	for(i=0;i<32*1024*1024;i+=16*1024) 
	{	
  		temp=*(vu32*)(Bank5_SDRAM_ADDR+i);
        printf("每隔16K字节,读出temp = %d, i = %d\n", temp, i);
		if(i==0)
            sval=temp;
 		else if(temp<=sval)
            break;//后面读出的数据一定要比第一次读到的数据大.              
 	}
    printf("SDRAM 容量:%dKB\r\n",(u16)(temp-sval+1)*16);//打印SDRAM容量
}

void cli_testsdram_fnt(int argc, char **argv)
{
    u32 ts=0;
    u32 tmp;

    taskENTER_CRITICAL();
    printf("准备测试数据,ts = %d\n", ts);
    for(ts=0;ts<250000;ts++)
	{
		testsram[ts]=ts;//预存测试数据
	}
    printf("测试数据准备完毕,ts = %d\n", ts);
    printf("开始对SDRAM进行容量测试\n");
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

