#define 	SAGCYC 			0x000  		//R/W 8 0x00 U Sag Line Cycle Register
#define 	LCYCMODE		0x004  		//R/W 8 0x40 U Line cycle Accumulation Mode Configuration
#define 	PGA_V			0x007  		//R/W 8 0x00 U Voltage Channel Gain Configuration
#define 	PGA_IA			0x008  		//R/W 8 0x00 U Current Channel A Gain Configuration
#define	PGA_IB			0x009  		//R/W 8 0x00 U Current Channel B Gain Configuration
#define	Write_protect	0x040  		//R/W 8 0x00 U Write protection bits [2:0]
#define	Last_Op			0xFD  		//R 8 0x00 U Contains the
#define	Last_rwdata8	0xFF  		//R 8 0x00 U Contains the data from the last successful 8 bit register communication
#define	EX_REF			0x800  		//R/W 8 0x00 U Reference input configuration. 0 for internal, set to 1 for external


#define	ZXTOUT			0x100  		//W/R 16 0xFFFF U Zero Crossing Timeout
#define	LINECYC			0x101  		//R/W 16 0x00 U Line Cycle Energy Accumulation Mode Line-Cycle Register
#define	CONFIG			0x102  		//W/R 16 0x8004 U Configuration Register
#define	CF1DEN			0x103  		//R/W 16 0x3F U CF1 Frequency Divider Denominator Register
#define	CF2DEN			0x104  		//R/W 16 0x3F U CF2 Frequency Divider Denominator Register
#define	CFMODE			0x107  		//R/W 16 0x300 U CF output Selection
#define	PHCALA			0x108  		//R/W 16 0x00 S Phase Calibration Register (channel A)
#define	PHCALB			0x109  		//R/W 16 0x00 S Phase Calibration Register (channel B)
#define	PFA				0x10A  		//R 16 0x00 S Power Factor channel A
#define	PFB				0x10B  		//R 16 0x00 S Power Factor Channel B
#define	Angle_A			0x10C  		//R 16 0x00 S Angle between voltage and Current A
#define	Angle_B			0x10D  		//R 16 0x00 S Angle between voltage and Current B
#define	PERIOD			0x10E  		//R 16 0x00 U Period Register
#define	ALT_Output		0x110  		//R/W 16 0x00 U Alternative Output Functions
#define	Last_Add		0x1FE  		//R 16 0x00 U Contains the address of the last successful communication
#define	Last_rwdata16	0x1FF  		//R 16 0x00 U Contains the data from the last successive 16 bit register communication



#define SAGLVL			0x300  		//R/W 24/32 0x00 U SAG Voltage Level
#define ACCMODE			0x301  		//R/W 24/32 0x00 U Accumulation Mode
#define AP_NOLOAD		0x303  		//R/W 24/32 0x00 U Active Power No Load Level
#define VAR_NOLOAD		0x304  		//R/W 24/32 0xE419 U Reactive Power No Load Level
#define VA_NLOAD		0x305  		//R/W 24/32 0xE419 U Apparent Power No Load Level
#define AVA				0x310  		//R 24/32 0x00 S Instantaneous Apparent Power A
#define BVA				0x311  		//R 24/32 0x00 S Instantaneous Apparent Power B
#define AWATT			0x312  		//R 24/32 0x00 S Instantaneous Active Power A
#define BWATT			0x313  		//R 24/32 0x00 S Instantaneous Active Power B
#define AVAR				0x314  		//R 24/32 0x00 S Instantaneous Reactive Power A
#define BVAR				0x315  		//R 24/32 0x00 S Instantaneous Reactive Power B
#define IA				0x316  		//R 24/32 0x00 S Instantaneous Current Channel A
#define IB				0x317  		//R 24/32 0x00 S Instantaneous Current Channel B
#define VA				0x318  		//R 24/32 0x00 S Instantaneous Voltage Channel A
#define VB				0x319  		//R 24/32 0x00 S Instantaneous Voltage Channel B
#define IRMSA			0x31A  		//R 24/32 0x00 U IRMS Register A (channel A)
#define IRMSB			0x31B  		//R 24/32 0x00 U IRMS Register B (channel B)
#define VRMS			0x31C  		//R 24/32 0x00 U VRMS Register
#define AENERGYA		0x31E  		//R 24/32 0x00 S Active Energy Register (channel A)
#define AENERGYB		0x31F  		//R 24/32 0x00 S Active Energy Register (channel B)
#define RENERGYA		0x320  		//R 24/32 0x00 S Reactive Energy Register (channel A)
#define RENERGYB		0x321  		//R 24/32 0x00 S Reactive Energy Register (channel B)
#define APENERGYA		0x322  		//R 24/32 0x00 S Apparent Energy Register (channel A)
#define APENERGYB		0x323  		//R 24/32 0x00 S Apparent Energy Register (channel B)
#define OVLVL			0x324  		//R/W 24/32 0xFFFFFF U Over Voltage Level
#define OILVL			0x325  		//R/W 24/32 0xFFFFFF U Over Current Level
#define VPEAK			0x326  		//R 24/32 0x00 U Voltage Channel Peak Register
#define RSTVPEAK		0x327  		//R 24/32 0x00 U Read Voltage Peak with Reset
#define IAPEAK			0x328  		//R 24/32 0x00 U Current Channel A Peak Register
#define RSTIAPEAK		0x329  		//R 24/32 0x00 U Read Current Channel A Peak with Reset
#define IBPEAK			0x32A  		//R 24/32 0x00 U Current Channel B Peak Register
#define RSTIBPEAK		0x32B  		//R 24/32 0x00 U Read Current Channel B Peak with Reset
#define IRQEN			0x32C  		//R/W 24/32 0x100000 U Interrupt Enable Register
#define IRQSTAT			0x32D  		//R 24/32 0x00 U Interrupt Status Register
#define RSTIRQSTAT		0x32E  		//R 24/32 0x00 U Reset Interrupt Status register
#define IRQENB			0x32F  		//R/W 24/32 0x00 U Interrupt B Enable Register
#define IRQSTATB		0x330  		//R 24/32 0x00 U Interrupt B Status Register
#define RSTIRQSTATB	0x331  		//R 24/32 0x00 U Reset Interrupt B Status register
#define ADE_CRC				0x37F  		//R 32 0xC02F1AD4 U Check Sum
#define AIGAIN			0x380  		//R/W 24/32 0x400000 U Current Channel Gain (channel A) 每 23 bit
#define AVGAIN			0x381  		//R/W 24/32 0x400000 U Voltage Channel Gain 每 23 bit
#define AWGAIN			0x382  		//R/W 24/32 0x400000 U Active Power Gain (channel A) 每 23 bit
#define AVARGAIN		0x383  		//R/W 24/32 0x400000 U Reactive Power Gain (channel A) 每 23 bit
#define AVAGAIN			0x384  		//R/W 24/32 0x400000 U Apparent Power Gain (channel A) 每 23 bit
#define AIOS 			0x385 		//R/W 24/32 0x00 S Current Channel Offset (channel A)
#define AIRMSOS			0x386  		//R/W 24/32 0x00 S IRMS Offset (channel A)
#define AVOS				0x387  		//R/W 24/32 0x00 S Voltage Channel Offset
#define AVRMSOS			0x388  		//R/W 24/32 0x00 S VRMS Offset
#define AWATTOS		0x389  		//R/W 24/32 0x00 S Active Power Offset Correction (channel A)
#define AVAROS			0x38A  		//R/W 24/32 0x00 S Reactive Power Offset Correction (channel A)
#define AVAOS			0x38B  		//R/W 24/32 0x00 S Apparent Power Offset Correction (channel A)
#define BIGAIN			0x38C  		//R/W 24/32 0x400000 S Current Channel Gain (channel B)
#define BVGAIN			0x38D  		//R/W 24/32 0x400000 S Voltage Channel Gain
#define BWGAIN			0x38E  		//R/W 24/32 0x400000 S Active Power Gain (channel B)
#define BVARGAIN		0x38F  		//R/W 24/32 0x400000 S Reactive Power Gain (channel B)
#define BVAGAIN			0x390  		//R/W 24/32 0x400000 S Apparent Power Gain (channel B)
#define BIOS				0x391  		//R/W 24/32 0x00 S Current Channel Offset (channel B)
#define BIRMSOS			0x392  		//R/W 24/32 0x00 S IRMS Offset (channel B)
#define BVOS				0x393  		//R/W 24/32 0x00 S Voltage Channel Offset
#define BVRMSOS			0x394  		//R/W 24/32 0x00 S VRMS Offset
#define BWATTOS		0x395  		//R/W 24/32 0x00 S Active Power Offset Correction (channel B)
#define BVAROS			0x396  		//R/W 24/32 0x00 S Reactive Power Offset Correction (channel B)
#define BVAOS			0x397  		//R/W 24/32 0x00 S Apparent Power Offset Correction (channel B)
#define Last_rwdata32	0x3FF  		//R 24/32 0x00 U Contains the data from the last successive 24/32 bit register communication


#define   CFDEN6400    		125       	
#define   AIGAIN_C		0
#define   AVGAIN_C		0
#define   AIRMSOS_C		0
#define   AVRMSOS_C		0
#define   BIGAIN_C		0
#define   BVGAIN_C		0
#define   BIRMSOS_C		0
#define   BVRMSOS_C		0
#define   CIGAIN_C		0
#define   CVGAIN_C		0
#define   CIRMSOS_C		0
#define   CVRMSOS_C		0
#define   APHCAL_C		0
#define   BPHCAL_C		0
#define   CPHCAL_C		0
/* 	#define   VANOLOAD_C	2503
 	#define   APNOLOAD_C	2503
 	#define   VARNOLOAD_C	2503	*/
#define   VANOLOAD_C	25
#define   APNOLOAD_C	25
#define   VARNOLOAD_C	25
#define   VATHR1_C		0x01
#define   VATHR0_C		0xF3E709
#define   WTHR1_C		0x01
#define   WTHR0_C		0xF3E709
#define   VARTHR1_C		0x01
#define   VARTHR0_C		0xF3E709
#define   VLEVEL_C		402885


#define   EXTERREFEN            0x01
#define   INTERREFEN            0xFE
#define   PORT_LOCK             0x02
#define   WATT2CF             	0xFFF8
#define   VAR2CF             	0xFFF9
#define   FWATT2CF             	0xFFFB
#define   CF1EN             	0xFDFF
#define   CF2EN             	0xFBFF
#define   CF3EN             	0xF7FF
#define   Angle_Phase           0xF3FF
#define   Angle_Voltage         0xF7FF
#define   Angle_Current         0xFBFF
#define   Period_A           	0xFC
#define   Period_B           	0xFD
#define   Period_C           	0xFE

typedef struct
{
    unsigned int ADD;
    unsigned char COMM_Bytes;
    unsigned char REG_Bytes;
}ADE7953_Reg;



/*******************************************************************************
                Definition of Union ADE_REGS_DATA
********************************************************************************
//Data of this type are used for data exchange between ADE module and Core.
//It can be accessed in different methods:by byte,by word and by double word.
//So it provides an easy way to access the ADE registers.
*/
typedef union
{
    long int F_DATA;
    struct
    {
        unsigned short low;
        unsigned char high;
    }wordL;
    struct
    {
        unsigned char low;
        unsigned short  high;
    }wordH;
    struct
    {
        unsigned char DataL;
        unsigned char DataMl;
        unsigned char DataMH;
        unsigned char DataH;
    }bytes;
    unsigned char Data_Array[4];
}ADE_REGS_DATA;



extern const ADE7953_Reg	Reg_SAGCYC;				//R/W 8 0x00 U Sag Line Cycle Register                                                          
extern const ADE7953_Reg 	Reg_LCYCMODE;			//R/W 8 0x40 U Line cycle Accumulation Mode Configuration                                     
extern const ADE7953_Reg	Reg_PGA_V;					//R/W 8 0x00 U Voltage Channel Gain Configuration                                               
extern const ADE7953_Reg	Reg_PGA_IA;				//R/W 8 0x00 U Current Channel A Gain Configuration                                             
extern const ADE7953_Reg	Reg_PGA_IB;				//R/W 8 0x00 U Current Channel B Gain Configuration                                             
extern const ADE7953_Reg	Reg_Write_protect;	//R/W 8 0x00 U Write protection bits [2:0]                                                      
extern const ADE7953_Reg	Reg_Last_Op;			//R 8 0x00 U Contains the                                                                       
extern const ADE7953_Reg	Reg_Last_rwdata8;		//R 8 0x00 U Contains the data from the last successful 8 bit register communication          
extern const ADE7953_Reg	Reg_EX_REF;				//R/W 8 0x00 U Reference input configuration. 0 for internal, set to 1 for external             
                                                                                                                                   
                                                                                                                                   
extern const ADE7953_Reg	Reg_ZXTOUT;			//W/R 16 0xFFFF U Zero Crossing Timeout                                                         
extern const ADE7953_Reg	Reg_LINECYC;				//R/W 16 0x00 U Line Cycle Energy Accumulation Mode Line-Cycle Register                         
extern const ADE7953_Reg	Reg_CONFIG;				//W/R 16 0x8004 U Configuration Register                                                        
extern const ADE7953_Reg	Reg_CF1DEN;				//R/W 16 0x3F U CF1 Frequency Divider Denominator Register                                      
extern const ADE7953_Reg	Reg_CF2DEN;			//R/W 16 0x3F U CF2 Frequency Divider Denominator Register                                      
extern const ADE7953_Reg	Reg_CFMODE;				//R/W 16 0x300 U CF output Selection                                                            
extern const ADE7953_Reg	Reg_PHCALA;				//R/W 16 0x00 S Phase Calibration Register (channel A)                                          
extern const ADE7953_Reg	Reg_PHCALB;				//R/W 16 0x00 S Phase Calibration Register (channel B)                                          
extern const ADE7953_Reg	Reg_PFA;						//R 16 0x00 S Power Factor channel A                                                            
extern const ADE7953_Reg	Reg_PFB;						//R 16 0x00 S Power Factor Channel B                                                            
extern const ADE7953_Reg	Reg_Angle_A;				//R 16 0x00 S Angle between voltage and Current A                                               
extern const ADE7953_Reg	Reg_Angle_B;				//R 16 0x00 S Angle between voltage and Current B                                               
extern const ADE7953_Reg	Reg_PERIOD;				//R 16 0x00 U Period Register                                                                   
extern const ADE7953_Reg	Reg_ALT_Output;		//R/W 16 0x00 U Alternative Output Functions                                                    
extern const ADE7953_Reg	Reg_Last_Add;			//R 16 0x00 U Contains the address of the last successful communication                         
extern const ADE7953_Reg	Reg_Last_rwdata16;	 		//R 16 0x00 U Contains the data from the last successive 16 bit register communication      
                                                                                                                                   
                                                                                                                                   
                                                                                                                                   
extern const ADE7953_Reg Reg_SAGLVL;				//R/W 24/32 0x00 U SAG Voltage Level                                                            
extern const ADE7953_Reg Reg_ACCMODE;				//R/W 24/32 0x00 U Accumulation Mode                                                            
extern const ADE7953_Reg Reg_AP_NOLOAD;			//R/W 24/32 0x00 U Active Power No Load Level                                                   
extern const ADE7953_Reg Reg_VAR_NOLOAD;		//R/W 24/32 0xE419 U Reactive Power No Load Level                                               
extern const ADE7953_Reg Reg_VA_NLOAD;			//R/W 24/32 0xE419 U Apparent Power No Load Level                                               
extern const ADE7953_Reg Reg_AVA;						//R 24/32 0x00 S Instantaneous Apparent Power A                                                 
extern const ADE7953_Reg Reg_BVA;						//R 24/32 0x00 S Instantaneous Apparent Power B                                                 
extern const ADE7953_Reg Reg_AWATT;					//R 24/32 0x00 S Instantaneous Active Power A                                                   
extern const ADE7953_Reg Reg_BWATT;					//R 24/32 0x00 S Instantaneous Active Power B                                                   
extern const ADE7953_Reg Reg_AVAR;					//R 24/32 0x00 S Instantaneous Reactive Power A                                                 
extern const ADE7953_Reg Reg_BVAR;					//R 24/32 0x00 S Instantaneous Reactive Power B                                                 
extern const ADE7953_Reg Reg_IA;						//R 24/32 0x00 S Instantaneous Current Channel A                                                
extern const ADE7953_Reg Reg_IB;						//R 24/32 0x00 S Instantaneous Current Channel B                                                
extern const ADE7953_Reg Reg_VA;						//R 24/32 0x00 S Instantaneous Voltage Channel A                                                
extern const ADE7953_Reg Reg_VB;						//R 24/32 0x00 S Instantaneous Voltage Channel B                                                
extern const ADE7953_Reg Reg_IRMSA;					//R 24/32 0x00 U IRMS Register A (channel A)                                                    
extern const ADE7953_Reg Reg_IRMSB;					//R 24/32 0x00 U IRMS Register B (channel B)                                                    
extern const ADE7953_Reg Reg_VRMS;					//R 24/32 0x00 U VRMS Register                                                                  
extern const ADE7953_Reg Reg_AENERGYA;			//R 24/32 0x00 S Active Energy Register (channel A)                                             
extern const ADE7953_Reg Reg_AENERGYB;			//R 24/32 0x00 S Active Energy Register (channel B)                                             
extern const ADE7953_Reg Reg_RENERGYA;			//R 24/32 0x00 S Reactive Energy Register (channel A)                                           
extern const ADE7953_Reg Reg_RENERGYB;			//R 24/32 0x00 S Reactive Energy Register (channel B)                                           
extern const ADE7953_Reg Reg_APENERGYA;			//R 24/32 0x00 S Apparent Energy Register (channel A)                                           
extern const ADE7953_Reg Reg_APENERGYB;			//R 24/32 0x00 S Apparent Energy Register (channel B)                                           
extern const ADE7953_Reg Reg_OVLVL;					//R/W 24/32 0xFFFFFF U Over Voltage Level                                                       
extern const ADE7953_Reg Reg_OILVL;					//R/W 24/32 0xFFFFFF U Over Current Level                                                       
extern const ADE7953_Reg Reg_VPEAK;					//R 24/32 0x00 U Voltage Channel Peak Register                                                  
extern const ADE7953_Reg Reg_RSTVPEAK;			//R 24/32 0x00 U Read Voltage Peak with Reset                                                   
extern const ADE7953_Reg Reg_IAPEAK;				//R 24/32 0x00 U Current Channel A Peak Register                                                
extern const ADE7953_Reg Reg_RSTIAPEAK;			//R 24/32 0x00 U Read Current Channel A Peak with Reset                                         
extern const ADE7953_Reg Reg_IBPEAK;				//R 24/32 0x00 U Current Channel B Peak Register                                                
extern const ADE7953_Reg Reg_RSTIBPEAK;			//R 24/32 0x00 U Read Current Channel B Peak with Reset                                         
extern const ADE7953_Reg Reg_IRQEN;					//R/W 24/32 0x100000 U Interrupt Enable Register                                                
extern const ADE7953_Reg Reg_IRQSTAT;				//R 24/32 0x00 U Interrupt Status Register                                                      
extern const ADE7953_Reg Reg_RSTIRQSTAT;		//R 24/32 0x00 U Reset Interrupt Status register                                                
extern const ADE7953_Reg Reg_IRQENB;				//R/W 24/32 0x00 U Interrupt B Enable Register                                                  
extern const ADE7953_Reg Reg_IRQSTATB;			//R 24/32 0x00 U Interrupt B Status Register                                                    
extern const ADE7953_Reg Reg_RSTIRQSTATB;		//R 24/32 0x00 U Reset Interrupt B Status register                                              
extern const ADE7953_Reg Reg_CRC;						//R 32 0xC02F1AD4 U Check Sum                                                                   
extern const ADE7953_Reg Reg_AIGAIN;				//R/W 24/32 0x400000 U Current Channel Gain (channel A) ?∫C 23 bit                             
extern const ADE7953_Reg Reg_AVGAIN;				 		//R/W 24/32 0x400000 U Voltage Channel Gain ?∫C 23 bit                                     
extern const ADE7953_Reg Reg_AWGAIN;				//R/W 24/32 0x400000 U Active Power Gain (channel A) ?∫C 23 bit                                
extern const ADE7953_Reg Reg_AVARGAIN;			//R/W 24/32 0x400000 U Reactive Power Gain (channel A) ?∫C 23 bit                              
extern const ADE7953_Reg Reg_AVAGAIN;				//R/W 24/32 0x400000 U Apparent Power Gain (channel A) ?∫C 23 bit                              
extern const ADE7953_Reg Reg_AIOS; 					//R/W 24/32 0x00 S Current Channel Offset (channel A)                                           
extern const ADE7953_Reg Reg_AIRMSOS;			//R/W 24/32 0x00 S IRMS Offset (channel A)                                                      
extern const ADE7953_Reg Reg_AVOS;					 		//R/W 24/32 0x00 S Voltage Channel Offset                                              #ifndef _ADE79xx_H
#define _ADE79xx_H
#include "ade7953.h"
     
extern const ADE7953_Reg Reg_AVRMSOS;				//R/W 24/32 0x00 S VRMS Offset                                                                  
extern const ADE7953_Reg Reg_AWATTOS;				//R/W 24/32 0x00 S Active Power Offset Correction (channel A)                                   
extern const ADE7953_Reg Reg_AVAROS;			//R/W 24/32 0x00 S Reactive Power Offset Correction (channel A)                                 
extern const ADE7953_Reg Reg_AVAOS;					//R/W 24/32 0x00 S Apparent Power Offset Correction (channel A)                                 
extern const ADE7953_Reg Reg_BIGAIN;				//R/W 24/32 0x400000 S Current Channel Gain (channel B)                                         
extern const ADE7953_Reg Reg_BVGAIN;				 		//R/W 24/32 0x400000 S Voltage Channel Gain                                                 
extern const ADE7953_Reg Reg_BWGAIN;				//R/W 24/32 0x400000 S Active Power Gain (channel B)                                            
extern const ADE7953_Reg Reg_BVARGAIN;			//R/W 24/32 0x400000 S Reactive Power Gain (channel B)                                          
extern const ADE7953_Reg Reg_BVAGAIN;			//R/W 24/32 0x400000 S Apparent Power Gain (channel B)                                          
extern const ADE7953_Reg Reg_BIOS;				//R/W 24/32 0x00 S Current Channel Offset (channel B)                                           
extern const ADE7953_Reg Reg_BIRMSOS;				//R/W 24/32 0x00 S IRMS Offset (channel B)                                                      
extern const ADE7953_Reg Reg_BVOS;					 		//R/W 24/32 0x00 S Voltage Channel Offset                                                   
extern const ADE7953_Reg Reg_BVRMSOS;				//R/W 24/32 0x00 S VRMS Offset                                                                  
extern const ADE7953_Reg Reg_BWATTOS;				//R/W 24/32 0x00 S Active Power Offset Correction (channel B)                                   
extern const ADE7953_Reg Reg_BVAROS;				//R/W 24/32 0x00 S Reactive Power Offset Correction (channel B)                                 
extern const ADE7953_Reg Reg_BVAOS;					//R/W 24/32 0x00 S Apparent Power Offset Correction (channel B)                                 
extern const ADE7953_Reg Reg_Last_rwdata32;	 		//R 24/32 0x00 U Contains the data from the last successive 24/32 bit register communication

