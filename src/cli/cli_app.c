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



//SDRAM内存测试	    
void fsmc_sdram_test()
{  
	u32 i=0;  	  
	u32 temp=0;	   
	u32 sval=0;	//在地址0读到的数据	  				   
	printf("每隔16K字节,写入一个数据,总共写入2048个数据,刚好是32M字节\n");
	for(i=0;i<32*1024*1024;i+=16*1024)
	{
		*(vu32*)(Bank5_SDRAM_ADDR+i)=temp; 
		temp++;
	}
    temp = 0;
	//依次读出之前写入的数据,进行校验		  
 	for(i=0;i<32*1024*1024;i+=16*1024) 
	{	
  		sval=*(vu32*)(Bank5_SDRAM_ADDR+i);
		if(temp==sval)
            printf("[@0x%x]=OK!\n",Bank5_SDRAM_ADDR | i);
        else{
            printf("[@0x%x]=ERR!\n",Bank5_SDRAM_ADDR | i);
            break;
      }
        temp++;
 	}
    printf("SDRAM 容量:%dMB\r\n",i/1024/1024);//打印SDRAM容量
}


void cli_testsdram_fnt(int argc, char **argv)
{
    taskENTER_CRITICAL();
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

