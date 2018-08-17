#ifndef _ADE79xx_Data_H
#define _ADE79xx_Data_H
#include "ade7953.h"
#include "ADE79xx.h"

extern const ADE7953_Reg Reg_SAGCYC;        //R/W 8 0x00 U Sag Line Cycle Register
extern const ADE7953_Reg Reg_LCYCMODE;      //R/W 8 0x40 U Line cycle Accumulation Mode Configuration
extern const ADE7953_Reg Reg_PGA_V;         //R/W 8 0x00 U Voltage Channel Gain Configuration
extern const ADE7953_Reg Reg_PGA_IA;        //R/W 8 0x00 U Current Channel A Gain Configuration
extern const ADE7953_Reg Reg_PGA_IB;        //R/W 8 0x00 U Current Channel B Gain Configuration
extern const ADE7953_Reg Reg_Write_protect; //R/W 8 0x00 U Write protection bits [2:0]
extern const ADE7953_Reg Reg_Last_Op;       //R 8 0x00 U Contains the
extern const ADE7953_Reg Reg_Last_rwdata8;  //R 8 0x00 U Contains the data from the last successful 8 bit register communication
extern const ADE7953_Reg Reg_EX_REF;        //R/W 8 0x00 U Reference input configuration. 0 for internal, set to 1 for external

extern const ADE7953_Reg Reg_ZXTOUT;        //W/R 16 0xFFFF U Zero Crossing Timeout
extern const ADE7953_Reg Reg_LINECYC;       //R/W 16 0x00 U Line Cycle Energy Accumulation Mode Line-Cycle Register
extern const ADE7953_Reg Reg_CONFIG;        //W/R 16 0x8004 U Configuration Register
extern const ADE7953_Reg Reg_CF1DEN;        //R/W 16 0x3F U CF1 Frequency Divider Denominator Register
extern const ADE7953_Reg Reg_CF2DEN;        //R/W 16 0x3F U CF2 Frequency Divider Denominator Register
extern const ADE7953_Reg Reg_CFMODE;        //R/W 16 0x300 U CF output Selection
extern const ADE7953_Reg Reg_PHCALA;        //R/W 16 0x00 S Phase Calibration Register (channel A)
extern const ADE7953_Reg Reg_PHCALB;        //R/W 16 0x00 S Phase Calibration Register (channel B)
extern const ADE7953_Reg Reg_PFA;           //R 16 0x00 S Power Factor channel A
extern const ADE7953_Reg Reg_PFB;           //R 16 0x00 S Power Factor Channel B
extern const ADE7953_Reg Reg_Angle_A;       //R 16 0x00 S Angle between voltage and Current A
extern const ADE7953_Reg Reg_Angle_B;       //R 16 0x00 S Angle between voltage and Current B
extern const ADE7953_Reg Reg_PERIOD;        //R 16 0x00 U Period Register
extern const ADE7953_Reg Reg_ALT_Output;    //R/W 16 0x00 U Alternative Output Functions
extern const ADE7953_Reg Reg_Last_Add;      //R 16 0x00 U Contains the address of the last successful communication
extern const ADE7953_Reg Reg_Last_rwdata16; //R 16 0x00 U Contains the data from the last successive 16 bit register communication

extern const ADE7953_Reg Reg_SAGLVL;        //R/W 24/32 0x00 U SAG Voltage Level
extern const ADE7953_Reg Reg_ACCMODE;       //R/W 24/32 0x00 U Accumulation Mode
extern const ADE7953_Reg Reg_AP_NOLOAD;     //R/W 24/32 0x00 U Active Power No Load Level
extern const ADE7953_Reg Reg_VAR_NOLOAD;    //R/W 24/32 0xE419 U Reactive Power No Load Level
extern const ADE7953_Reg Reg_VA_NLOAD;      //R/W 24/32 0xE419 U Apparent Power No Load Level
extern const ADE7953_Reg Reg_AVA;           //R 24/32 0x00 S Instantaneous Apparent Power A
extern const ADE7953_Reg Reg_BVA;           //R 24/32 0x00 S Instantaneous Apparent Power B
extern const ADE7953_Reg Reg_AWATT;         //R 24/32 0x00 S Instantaneous Active Power A
extern const ADE7953_Reg Reg_BWATT;         //R 24/32 0x00 S Instantaneous Active Power B
extern const ADE7953_Reg Reg_AVAR;          //R 24/32 0x00 S Instantaneous Reactive Power A
extern const ADE7953_Reg Reg_BVAR;          //R 24/32 0x00 S Instantaneous Reactive Power B
extern const ADE7953_Reg Reg_IA;            //R 24/32 0x00 S Instantaneous Current Channel A
extern const ADE7953_Reg Reg_IB;            //R 24/32 0x00 S Instantaneous Current Channel B
extern const ADE7953_Reg Reg_VA;            //R 24/32 0x00 S Instantaneous Voltage Channel A
extern const ADE7953_Reg Reg_VB;            //R 24/32 0x00 S Instantaneous Voltage Channel B
extern const ADE7953_Reg Reg_IRMSA;         //R 24/32 0x00 U IRMS Register A (channel A)
extern const ADE7953_Reg Reg_IRMSB;         //R 24/32 0x00 U IRMS Register B (channel B)
extern const ADE7953_Reg Reg_VRMS;          //R 24/32 0x00 U VRMS Register
extern const ADE7953_Reg Reg_AENERGYA;      //R 24/32 0x00 S Active Energy Register (channel A)
extern const ADE7953_Reg Reg_AENERGYB;      //R 24/32 0x00 S Active Energy Register (channel B)
extern const ADE7953_Reg Reg_RENERGYA;      //R 24/32 0x00 S Reactive Energy Register (channel A)
extern const ADE7953_Reg Reg_RENERGYB;      //R 24/32 0x00 S Reactive Energy Register (channel B)
extern const ADE7953_Reg Reg_APENERGYA;     //R 24/32 0x00 S Apparent Energy Register (channel A)
extern const ADE7953_Reg Reg_APENERGYB;     //R 24/32 0x00 S Apparent Energy Register (channel B)
extern const ADE7953_Reg Reg_OVLVL;         //R/W 24/32 0xFFFFFF U Over Voltage Level
extern const ADE7953_Reg Reg_OILVL;         //R/W 24/32 0xFFFFFF U Over Current Level
extern const ADE7953_Reg Reg_VPEAK;         //R 24/32 0x00 U Voltage Channel Peak Register
extern const ADE7953_Reg Reg_RSTVPEAK;      //R 24/32 0x00 U Read Voltage Peak with Reset
extern const ADE7953_Reg Reg_IAPEAK;        //R 24/32 0x00 U Current Channel A Peak Register
extern const ADE7953_Reg Reg_RSTIAPEAK;     //R 24/32 0x00 U Read Current Channel A Peak with Reset
extern const ADE7953_Reg Reg_IBPEAK;        //R 24/32 0x00 U Current Channel B Peak Register
extern const ADE7953_Reg Reg_RSTIBPEAK;     //R 24/32 0x00 U Read Current Channel B Peak with Reset
extern const ADE7953_Reg Reg_IRQEN;         //R/W 24/32 0x100000 U Interrupt Enable Register
extern const ADE7953_Reg Reg_IRQSTAT;       //R 24/32 0x00 U Interrupt Status Register
extern const ADE7953_Reg Reg_RSTIRQSTAT;    //R 24/32 0x00 U Reset Interrupt Status register
extern const ADE7953_Reg Reg_IRQENB;        //R/W 24/32 0x00 U Interrupt B Enable Register
extern const ADE7953_Reg Reg_IRQSTATB;      //R 24/32 0x00 U Interrupt B Status Register
extern const ADE7953_Reg Reg_RSTIRQSTATB;   //R 24/32 0x00 U Reset Interrupt B Status register
extern const ADE7953_Reg Reg_CRC;           //R 32 0xC02F1AD4 U Check Sum
extern const ADE7953_Reg Reg_AIGAIN;        //R/W 24/32 0x400000 U Current Channel Gain (channel A) ��C 23 bit
extern const ADE7953_Reg Reg_AVGAIN;        //R/W 24/32 0x400000 U Voltage Channel Gain ��C 23 bit
extern const ADE7953_Reg Reg_AWGAIN;        //R/W 24/32 0x400000 U Active Power Gain (channel A) ��C 23 bit
extern const ADE7953_Reg Reg_AVARGAIN;      //R/W 24/32 0x400000 U Reactive Power Gain (channel A) ��C 23 bit
extern const ADE7953_Reg Reg_AVAGAIN;       //R/W 24/32 0x400000 U Apparent Power Gain (channel A) ��C 23 bit
extern const ADE7953_Reg Reg_AIOS;          //R/W 24/32 0x00 S Current Channel Offset (channel A)
extern const ADE7953_Reg Reg_AIRMSOS;       //R/W 24/32 0x00 S IRMS Offset (channel A)
extern const ADE7953_Reg Reg_AVOS;          //R/W 24/32 0x00 S Voltage Channel Offset
extern const ADE7953_Reg Reg_AVRMSOS;       //R/W 24/32 0x00 S VRMS Offset
extern const ADE7953_Reg Reg_AWATTOS;       //R/W 24/32 0x00 S Active Power Offset Correction (channel A)
extern const ADE7953_Reg Reg_AVAROS;        //R/W 24/32 0x00 S Reactive Power Offset Correction (channel A)
extern const ADE7953_Reg Reg_AVAOS;         //R/W 24/32 0x00 S Apparent Power Offset Correction (channel A)
extern const ADE7953_Reg Reg_BIGAIN;        //R/W 24/32 0x400000 S Current Channel Gain (channel B)
extern const ADE7953_Reg Reg_BVGAIN;        //R/W 24/32 0x400000 S Voltage Channel Gain
extern const ADE7953_Reg Reg_BWGAIN;        //R/W 24/32 0x400000 S Active Power Gain (channel B)
extern const ADE7953_Reg Reg_BVARGAIN;      //R/W 24/32 0x400000 S Reactive Power Gain (channel B)
extern const ADE7953_Reg Reg_BVAGAIN;       //R/W 24/32 0x400000 S Apparent Power Gain (channel B)
extern const ADE7953_Reg Reg_BIOS;          //R/W 24/32 0x00 S Current Channel Offset (channel B)
extern const ADE7953_Reg Reg_BIRMSOS;       //R/W 24/32 0x00 S IRMS Offset (channel B)
extern const ADE7953_Reg Reg_BVOS;          //R/W 24/32 0x00 S Voltage Channel Offset
extern const ADE7953_Reg Reg_BVRMSOS;       //R/W 24/32 0x00 S VRMS Offset
extern const ADE7953_Reg Reg_BWATTOS;       //R/W 24/32 0x00 S Active Power Offset Correction (channel B)
extern const ADE7953_Reg Reg_BVAROS;        //R/W 24/32 0x00 S Reactive Power Offset Correction (channel B)
extern const ADE7953_Reg Reg_BVAOS;         //R/W 24/32 0x00 S Apparent Power Offset Correction (channel B)
extern const ADE7953_Reg Reg_Last_rwdata32; //R 24/32 0x00 U Contains the data from the last successive 24/32 bit register communication

extern ADE_REGS_DATA DATA_SAGCYC;        //R/W 8 0x00 U Sag Line Cycle Register
extern ADE_REGS_DATA DATA_LCYCMODE;      //R/W 8 0x40 U Line cycle Accumulation Mode Configuration
extern ADE_REGS_DATA DATA_PGA_V;         //R/W 8 0x00 U Voltage Channel Gain Configuration
extern ADE_REGS_DATA DATA_PGA_IA;        //R/W 8 0x00 U Current Channel A Gain Configuration
extern ADE_REGS_DATA DATA_PGA_IB;        //R/W 8 0x00 U Current Channel B Gain Configuration
extern ADE_REGS_DATA DATA_Write_protect; //R/W 8 0x00 U Write protection bits [2:0]
extern ADE_REGS_DATA DATA_Last_Op;       //R 8 0x00 U Contains the
extern ADE_REGS_DATA DATA_Last_rwdata8;  //R 8 0x00 U Contains the data from the last successful 8 bit register communication
extern ADE_REGS_DATA DATA_EX_REF;        //R/W 8 0x00 U Reference input configuration. 0 for internal, set to 1 for external

extern ADE_REGS_DATA DATA_ZXTOUT;        //W/R 16 0xFFFF U Zero Crossing Timeout
extern ADE_REGS_DATA DATA_LINECYC;       //R/W 16 0x00 U Line Cycle Energy Accumulation Mode Line-Cycle Register
extern ADE_REGS_DATA DATA_CONFIG;        //W/R 16 0x8004 U Configuration Register
extern ADE_REGS_DATA DATA_CF1DEN;        //R/W 16 0x3F U CF1 Frequency Divider Denominator Register
extern ADE_REGS_DATA DATA_CF2DEN;        //R/W 16 0x3F U CF2 Frequency Divider Denominator Register
extern ADE_REGS_DATA DATA_CFMODE;        //R/W 16 0x300 U CF output Selection
extern ADE_REGS_DATA DATA_PHCALA;        //R/W 16 0x00 S Phase Calibration Register (channel A)
extern ADE_REGS_DATA DATA_PHCALB;        //R/W 16 0x00 S Phase Calibration Register (channel B)
extern ADE_REGS_DATA DATA_PFA;           //R 16 0x00 S Power Factor channel A
extern ADE_REGS_DATA DATA_PFB;           //R 16 0x00 S Power Factor Channel B
extern ADE_REGS_DATA DATA_Angle_A;       //R 16 0x00 S Angle between voltage and Current A
extern ADE_REGS_DATA DATA_Angle_B;       //R 16 0x00 S Angle between voltage and Current B
extern ADE_REGS_DATA DATA_PERIOD;        //R 16 0x00 U Period Register
extern ADE_REGS_DATA DATA_ALT_Output;    //R/W 16 0x00 U Alternative Output Functions
extern ADE_REGS_DATA DATA_Last_Add;      //R 16 0x00 U Contains the address of the last successful communication
extern ADE_REGS_DATA DATA_Last_rwdata16; //R 16 0x00 U Contains the data from the last successive 16 bit register communication

extern ADE_REGS_DATA DATA_SAGLVL;        //R/W 24/32 0x00 U SAG Voltage Level
extern ADE_REGS_DATA DATA_ACCMODE;       //R/W 24/32 0x00 U Accumulation Mode
extern ADE_REGS_DATA DATA_AP_NOLOAD;     //R/W 24/32 0x00 U Active Power No Load Level
extern ADE_REGS_DATA DATA_VAR_NOLOAD;    //R/W 24/32 0xE419 U Reactive Power No Load Level
extern ADE_REGS_DATA DATA_VA_NLOAD;      //R/W 24/32 0xE419 U Apparent Power No Load Level
extern ADE_REGS_DATA DATA_AVA;           //R 24/32 0x00 S Instantaneous Apparent Power A
extern ADE_REGS_DATA DATA_BVA;           //R 24/32 0x00 S Instantaneous Apparent Power B
extern ADE_REGS_DATA DATA_AWATT;         //R 24/32 0x00 S Instantaneous Active Power A
extern ADE_REGS_DATA DATA_BWATT;         //R 24/32 0x00 S Instantaneous Active Power B
extern ADE_REGS_DATA DATA_AVAR;          //R 24/32 0x00 S Instantaneous Reactive Power A
extern ADE_REGS_DATA DATA_BVAR;          //R 24/32 0x00 S Instantaneous Reactive Power B
extern ADE_REGS_DATA DATA_IA;            //R 24/32 0x00 S Instantaneous Current Channel A
extern ADE_REGS_DATA DATA_IB;            //R 24/32 0x00 S Instantaneous Current Channel B
extern ADE_REGS_DATA DATA_VA;            //R 24/32 0x00 S Instantaneous Voltage Channel A
extern ADE_REGS_DATA DATA_VB;            //R 24/32 0x00 S Instantaneous Voltage Channel B
extern ADE_REGS_DATA DATA_IRMSA;         //R 24/32 0x00 U IRMS Register A (channel A)
extern ADE_REGS_DATA DATA_IRMSB;         //R 24/32 0x00 U IRMS Register B (channel B)
extern ADE_REGS_DATA DATA_VRMS;          //R 24/32 0x00 U VRMS Register
extern ADE_REGS_DATA DATA_AENERGYA;      //R 24/32 0x00 S Active Energy Register (channel A)
extern ADE_REGS_DATA DATA_AENERGYB;      //R 24/32 0x00 S Active Energy Register (channel B)
extern ADE_REGS_DATA DATA_RENERGYA;      //R 24/32 0x00 S Reactive Energy Register (channel A)
extern ADE_REGS_DATA DATA_RENERGYB;      //R 24/32 0x00 S Reactive Energy Register (channel B)
extern ADE_REGS_DATA DATA_APENERGYA;     //R 24/32 0x00 S Apparent Energy Register (channel A)
extern ADE_REGS_DATA DATA_APENERGYB;     //R 24/32 0x00 S Apparent Energy Register (channel B)
extern ADE_REGS_DATA DATA_OVLVL;         //R/W 24/32 0xFFFFFF U Over Voltage Level
extern ADE_REGS_DATA DATA_OILVL;         //R/W 24/32 0xFFFFFF U Over Current Level
extern ADE_REGS_DATA DATA_VPEAK;         //R 24/32 0x00 U Voltage Channel Peak Register
extern ADE_REGS_DATA DATA_RSTVPEAK;      //R 24/32 0x00 U Read Voltage Peak with Reset
extern ADE_REGS_DATA DATA_IAPEAK;        //R 24/32 0x00 U Current Channel A Peak Register
extern ADE_REGS_DATA DATA_RSTIAPEAK;     //R 24/32 0x00 U Read Current Channel A Peak with Reset
extern ADE_REGS_DATA DATA_IBPEAK;        //R 24/32 0x00 U Current Channel B Peak Register
extern ADE_REGS_DATA DATA_RSTIBPEAK;     //R 24/32 0x00 U Read Current Channel B Peak with Reset
extern ADE_REGS_DATA DATA_IRQEN;         //R/W 24/32 0x100000 U Interrupt Enable Register
extern ADE_REGS_DATA DATA_IRQSTAT;       //R 24/32 0x00 U Interrupt Status Register
extern ADE_REGS_DATA DATA_RSTIRQSTAT;    //R 24/32 0x00 U Reset Interrupt Status register
extern ADE_REGS_DATA DATA_IRQENB;        //R/W 24/32 0x00 U Interrupt B Enable Register
extern ADE_REGS_DATA DATA_IRQSTATB;      //R 24/32 0x00 U Interrupt B Status Register
extern ADE_REGS_DATA DATA_RSTIRQSTATB;   //R 24/32 0x00 U Reset Interrupt B Status register
extern ADE_REGS_DATA DATA_CRC;           //R 32 0xC02F1AD4 U Check Sum
extern ADE_REGS_DATA DATA_AIGAIN;        //R/W 24/32 0x400000 U Current Channel Gain (channel A) ?��C 23 bit
extern ADE_REGS_DATA DATA_AVGAIN;        //R/W 24/32 0x400000 U Voltage Channel Gain ?��C 23 bit
extern ADE_REGS_DATA DATA_AWGAIN;        //R/W 24/32 0x400000 U Active Power Gain (channel A) ?��C 23 bit
extern ADE_REGS_DATA DATA_AVARGAIN;      //R/W 24/32 0x400000 U Reactive Power Gain (channel A) ?��C 23 bit
extern ADE_REGS_DATA DATA_AVAGAIN;       //R/W 24/32 0x400000 U Apparent Power Gain (channel A) ?��C 23 bit
extern ADE_REGS_DATA DATA_AIOS;          //R/W 24/32 0x00 S Current Channel Offset (channel A)
extern ADE_REGS_DATA DATA_AIRMSOS;       //R/W 24/32 0x00 S IRMS Offset (channel A)
extern ADE_REGS_DATA DATA_VOS;           //R/W 24/32 0x00 S Voltage Channel Offset
extern ADE_REGS_DATA DATA_VRMSOS;        //R/W 24/32 0x00 S VRMS Offset
extern ADE_REGS_DATA DATA_AWATTOS;       //R/W 24/32 0x00 S Active Power Offset Correction (channel A)
extern ADE_REGS_DATA DATA_AVAROS;        //R/W 24/32 0x00 S Reactive Power Offset Correction (channel A)
extern ADE_REGS_DATA DATA_AVAOS;         //R/W 24/32 0x00 S Apparent Power Offset Correction (channel A)
extern ADE_REGS_DATA DATA_BIGAIN;        //R/W 24/32 0x400000 S Current Channel Gain (channel B)
extern ADE_REGS_DATA DATA_BVGAIN;        //R/W 24/32 0x400000 S Voltage Channel Gain
extern ADE_REGS_DATA DATA_BWGAIN;        //R/W 24/32 0x400000 S Active Power Gain (channel B)
extern ADE_REGS_DATA DATA_BVARGAIN;      //R/W 24/32 0x400000 S Reactive Power Gain (channel B)
extern ADE_REGS_DATA DATA_BVAGAIN;       //R/W 24/32 0x400000 S Apparent Power Gain (channel B)
extern ADE_REGS_DATA DATA_BIOS;          //R/W 24/32 0x00 S Current Channel Offset (channel B)
extern ADE_REGS_DATA DATA_BIRMSOS;       //R/W 24/32 0x00 S IRMS Offset (channel B)
extern ADE_REGS_DATA DATA_VOS;           //R/W 24/32 0x00 S Voltage Channel Offset
extern ADE_REGS_DATA DATA_VRMSOS;        //R/W 24/32 0x00 S VRMS Offset
extern ADE_REGS_DATA DATA_BWATTOS;       //R/W 24/32 0x00 S Active Power Offset Correction (channel B)
extern ADE_REGS_DATA DATA_BVAROS;        //R/W 24/32 0x00 S Reactive Power Offset Correction (channel B)
extern ADE_REGS_DATA DATA_BVAOS;         //R/W 24/32 0x00 S Apparent Power Offset Correction (channel B)
extern ADE_REGS_DATA DATA_Last_rwdata32; //R 24/32 0x00 U Contains the data from the last successive 24/32 bit register communication

#endif
