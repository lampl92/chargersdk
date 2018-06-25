#include "Metering.h"

//////////////////////////////////////////////////////////////////////////////////	 

////////////////////////////////////////////////////////////////////////////////// 	
unsigned long ActiveEnergy;       //�й�����   
unsigned long ReactiveEnergy;     //�޹�����
unsigned long ActivePowerA;       //�й�����A
unsigned long ActivePowerB;       //�й�����B
unsigned long ReactivePowerA;     //�޹�����A
unsigned long ReactivePowerB;     //�޹�����B
unsigned long IrmsA;              //����ͨ��A
unsigned long IrmsB;              //����ͨ��B
unsigned long Vrms;               //��ѹͨ��F
unsigned short int FrequencyA;    //Ƶ��
unsigned short int PowerFactorA;  //��������
unsigned short int PowerFactorB;  //��������
double TEST_YUYE_VA;
int hzxiede[4]={0};
extern void Write_ADE7953_SPI(unsigned int ADE_Addr, unsigned char Nr_Bytes,unsigned char *pReg_Data);
extern void Read_ADE7953_SPI(unsigned int ADE_Addr, unsigned char Nr_Bytes,unsigned char *pReg_Data);
unsigned char gain_1=1,gain_22=5;
void setgain(void)
{
Write_ADE7953_SPI(PGA_V,1,&gain_1);
Write_ADE7953_SPI(PGA_IA,1,&gain_22);	
}
	
void ReadAde7953DataRegister(void)
{
    long YUYE_REG;
  Read_ADE7953_SPI(Reg_PGA_IA.ADD,Reg_PGA_IA.COMM_Bytes,DATA_PGA_IA.Data_Array);
	
	Read_ADE7953_SPI(Reg_PGA_V.ADD,Reg_PGA_V.COMM_Bytes,DATA_PGA_V.Data_Array);
	
	Read_ADE7953_SPI(Reg_AP_NOLOAD.ADD,Reg_AWATT.COMM_Bytes,DATA_AWATT.Data_Array);
	
	Read_ADE7953_SPI(Reg_BWATT.ADD,Reg_BWATT.COMM_Bytes,DATA_BWATT.Data_Array);
	
	Read_ADE7953_SPI(Reg_AVAR.ADD,Reg_AVAR.COMM_Bytes,DATA_AVAR.Data_Array);
	
	Read_ADE7953_SPI(Reg_BVAR.ADD,Reg_BVAR.COMM_Bytes,DATA_BVAR.Data_Array);
	
	Read_ADE7953_SPI(Reg_IRMSA.ADD,Reg_IRMSA.COMM_Bytes,DATA_IRMSA.Data_Array);
	
	Read_ADE7953_SPI(Reg_IRMSB.ADD,Reg_IRMSB.COMM_Bytes,DATA_IRMSB.Data_Array);
	
	Read_ADE7953_SPI(Reg_VRMS.ADD,Reg_VRMS.COMM_Bytes,DATA_VRMS.Data_Array);
	
		Read_ADE7953_SPI(0x31C ,4,(unsigned char *)(hzxiede));

	
	Read_ADE7953_SPI(Reg_PERIOD.ADD,Reg_PERIOD.COMM_Bytes,DATA_PERIOD.Data_Array);
	
	Read_ADE7953_SPI(Reg_PFB.ADD,Reg_PFB.COMM_Bytes,DATA_PFB.Data_Array);
	
	Read_ADE7953_SPI(Reg_PFA.ADD,Reg_PFA.COMM_Bytes,DATA_PFA.Data_Array);
    YUYE_REG = DATA_VRMS.Data_Array[0] + DATA_VRMS.Data_Array[1] * 256 + DATA_VRMS.Data_Array[2] * 256 * 256;
    TEST_YUYE_VA = YUYE_REG / 17191;   
}

