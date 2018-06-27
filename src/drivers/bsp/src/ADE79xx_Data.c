#include "ADE79xx_Data.h"

const ADE7953_Reg	Reg_SAGCYC = { SAGCYC, 1, 1 };  				 		//R/W 8 0x00 U Sag Line Cycle Register
const ADE7953_Reg 	Reg_LCYCMODE = { LCYCMODE, 1, 1 }; 			 		//R/W 8 0x40 U Line cycle Accumulation Mode Configuration
const ADE7953_Reg	Reg_PGA_V = { PGA_V, 1, 1 }; 					 		//R/W 8 0x00 U Voltage Channel Gain Configuration
const ADE7953_Reg	Reg_PGA_IA = { PGA_IA, 1, 1 }; 				 		//R/W 8 0x00 U Current Channel A Gain Configuration
const ADE7953_Reg	Reg_PGA_IB = { PGA_IB, 1, 1 }; 				 		//R/W 8 0x00 U Current Channel B Gain Configuration
const ADE7953_Reg	Reg_Write_protect = { Write_protect, 1, 1 }; 	 		//R/W 8 0x00 U Write protection bits [2:0]
const ADE7953_Reg	Reg_Last_Op = { Last_Op, 1, 1 }; 						//R 8 0x00 U Contains the
const ADE7953_Reg	Reg_Last_rwdata8 = { Last_rwdata8, 1, 1 }; 				//R 8 0x00 U Contains the data from the last successful 8 bit register communication
const ADE7953_Reg	Reg_EX_REF = { EX_REF, 1, 1 }; 				 		//R/W 8 0x00 U Reference input configuration. 0 for internal, set to 1 for external
                                             
                                            
const ADE7953_Reg	Reg_ZXTOUT = { ZXTOUT, 2, 2 }; 				 		//W/R 16 0xFFFF U Zero Crossing Timeout
const ADE7953_Reg	Reg_LINECYC = { LINECYC, 2, 2 }; 				 		//R/W 16 0x00 U Line Cycle Energy Accumulation Mode Line-Cycle Register
const ADE7953_Reg	Reg_CONFIG = { CONFIG, 2, 2 }; 				 		//W/R 16 0x8004 U Configuration Register
const ADE7953_Reg	Reg_CF1DEN = { CF1DEN, 2, 2 }; 				 		//R/W 16 0x3F U CF1 Frequency Divider Denominator Register
const ADE7953_Reg	Reg_CF2DEN = { CF2DEN, 2, 2 }; 				 		//R/W 16 0x3F U CF2 Frequency Divider Denominator Register
const ADE7953_Reg	Reg_CFMODE = { CFMODE, 2, 2 }; 				 		//R/W 16 0x300 U CF output Selection
const ADE7953_Reg	Reg_PHCALA = { PHCALA, 2, 2 }; 				 		//R/W 16 0x00 S Phase Calibration Register (channel A)
const ADE7953_Reg	Reg_PHCALB = { PHCALB, 2, 2 }; 				 		//R/W 16 0x00 S Phase Calibration Register (channel B)
const ADE7953_Reg	Reg_PFA = { PFA, 2, 2 }; 						 		//R 16 0x00 S Power Factor channel A
const ADE7953_Reg	Reg_PFB = { PFB, 2, 2 }; 						 		//R 16 0x00 S Power Factor Channel B
const ADE7953_Reg	Reg_Angle_A = { Angle_A, 2, 2 }; 				 		//R 16 0x00 S Angle between voltage and Current A
const ADE7953_Reg	Reg_Angle_B = { Angle_B, 2, 2 }; 				 		//R 16 0x00 S Angle between voltage and Current B
const ADE7953_Reg	Reg_PERIOD = { PERIOD, 2, 2 }; 				 		//R 16 0x00 U Period Register
const ADE7953_Reg	Reg_ALT_Output = { ALT_Output, 2, 2 }; 		 		//R/W 16 0x00 U Alternative Output Functions
const ADE7953_Reg	Reg_Last_Add = { Last_Add, 2, 2 }; 			 		//R 16 0x00 U Contains the address of the last successful communication
const ADE7953_Reg	Reg_Last_rwdata16 = { Last_rwdata16, 2, 2 }; 		 		//R 16 0x00 U Contains the data from the last successive 16 bit register communication
                                            
                                            
                                            
const ADE7953_Reg Reg_SAGLVL = { SAGLVL, 4, 3 }; 				 		//R/W 24/32 0x00 U SAG Voltage Level
const ADE7953_Reg Reg_ACCMODE = { ACCMODE, 4, 3 }; 				 		//R/W 24/32 0x00 U Accumulation Mode
const ADE7953_Reg Reg_AP_NOLOAD = { AP_NOLOAD, 4, 3 }; 			 		//R/W 24/32 0x00 U Active Power No Load Level
const ADE7953_Reg Reg_VAR_NOLOAD = { VAR_NOLOAD, 4, 3 }; 		 		//R/W 24/32 0xE419 U Reactive Power No Load Level
const ADE7953_Reg Reg_VA_NLOAD = { VA_NLOAD, 4, 3 }; 			 		//R/W 24/32 0xE419 U Apparent Power No Load Level
const ADE7953_Reg Reg_AVA = { AVA, 4, 3 }; 						 		//R 24/32 0x00 S Instantaneous Apparent Power A
const ADE7953_Reg Reg_BVA = { BVA, 4, 3 }; 						 		//R 24/32 0x00 S Instantaneous Apparent Power B
const ADE7953_Reg Reg_AWATT = { AWATT, 4, 3 }; 					 		//R 24/32 0x00 S Instantaneous Active Power A
const ADE7953_Reg Reg_BWATT = { BWATT, 4, 3 }; 					 		//R 24/32 0x00 S Instantaneous Active Power B
const ADE7953_Reg Reg_AVAR = { AVAR, 4, 3 }; 					 		//R 24/32 0x00 S Instantaneous Reactive Power A
const ADE7953_Reg Reg_BVAR = { BVAR, 4, 3 }; 					 		//R 24/32 0x00 S Instantaneous Reactive Power B
const ADE7953_Reg Reg_IA = { IA, 4, 3 }; 						 		//R 24/32 0x00 S Instantaneous Current Channel A
const ADE7953_Reg Reg_IB = { IB, 4, 3 }; 						 		//R 24/32 0x00 S Instantaneous Current Channel B
const ADE7953_Reg Reg_VA = { VA, 4, 3 }; 						 		//R 24/32 0x00 S Instantaneous Voltage Channel A
const ADE7953_Reg Reg_VB = { VB, 4, 3 }; 						 		//R 24/32 0x00 S Instantaneous Voltage Channel B
const ADE7953_Reg Reg_IRMSA = { IRMSA, 4, 3 }; 					 		//R 24/32 0x00 U IRMS Register A (channel A)
const ADE7953_Reg Reg_IRMSB = { IRMSB, 4, 3 }; 					 		//R 24/32 0x00 U IRMS Register B (channel B)
const ADE7953_Reg Reg_VRMS = { VRMS, 4, 3 }; 					 		//R 24/32 0x00 U VRMS Register
const ADE7953_Reg Reg_AENERGYA = { AENERGYA, 4, 3 }; 			 		//R 24/32 0x00 S Active Energy Register (channel A)
const ADE7953_Reg Reg_AENERGYB = { AENERGYB, 4, 3 }; 			 		//R 24/32 0x00 S Active Energy Register (channel B)
const ADE7953_Reg Reg_RENERGYA = { RENERGYA, 4, 3 }; 			 		//R 24/32 0x00 S Reactive Energy Register (channel A)
const ADE7953_Reg Reg_RENERGYB = { RENERGYB, 4, 3 }; 			 		//R 24/32 0x00 S Reactive Energy Register (channel B)
const ADE7953_Reg Reg_APENERGYA = { APENERGYA, 4, 3 }; 			 		//R 24/32 0x00 S Apparent Energy Register (channel A)
const ADE7953_Reg Reg_APENERGYB = { APENERGYB, 4, 3 }; 		 		//R 24/32 0x00 S Apparent Energy Register (channel B)
const ADE7953_Reg Reg_OVLVL = { OVLVL, 4, 3 }; 					 		//R/W 24/32 0xFFFFFF U Over Voltage Level
const ADE7953_Reg Reg_OILVL = { OILVL, 4, 3 }; 					 		//R/W 24/32 0xFFFFFF U Over Current Level
const ADE7953_Reg Reg_VPEAK = { VPEAK, 4, 3 }; 					 		//R 24/32 0x00 U Voltage Channel Peak Register
const ADE7953_Reg Reg_RSTVPEAK = { RSTVPEAK, 4, 3 }; 			 		//R 24/32 0x00 U Read Voltage Peak with Reset
const ADE7953_Reg Reg_IAPEAK = { IAPEAK, 4, 3 }; 				 		//R 24/32 0x00 U Current Channel A Peak Register
const ADE7953_Reg Reg_RSTIAPEAK = { RSTIAPEAK, 4, 3 }; 			 		//R 24/32 0x00 U Read Current Channel A Peak with Reset
const ADE7953_Reg Reg_IBPEAK = { IBPEAK, 4, 3 }; 				 		//R 24/32 0x00 U Current Channel B Peak Register
const ADE7953_Reg Reg_RSTIBPEAK = { RSTIBPEAK, 4, 3 }; 			 		//R 24/32 0x00 U Read Current Channel B Peak with Reset
const ADE7953_Reg Reg_IRQEN = { IRQEN, 4, 3 }; 					 		//R/W 24/32 0x100000 U Interrupt Enable Register
const ADE7953_Reg Reg_IRQSTAT = { IRQSTAT, 4, 3 }; 				 		//R 24/32 0x00 U Interrupt Status Register
const ADE7953_Reg Reg_RSTIRQSTAT = { RSTIRQSTAT, 4, 3 }; 		 		//R 24/32 0x00 U Reset Interrupt Status register
const ADE7953_Reg Reg_IRQENB = { IRQENB, 4, 3 }; 				 		//R/W 24/32 0x00 U Interrupt B Enable Register
const ADE7953_Reg Reg_IRQSTATB = { IRQSTATB, 4, 3 }; 			 		//R 24/32 0x00 U Interrupt B Status Register
const ADE7953_Reg Reg_RSTIRQSTATB = { RSTIRQSTATB, 4, 3 }; 		 		//R 24/32 0x00 U Reset Interrupt B Status register
const ADE7953_Reg Reg_CRC = { ADE_CRC, 4, 4 }; 						 		//R 32 0xC02F1AD4 U Check Sum
const ADE7953_Reg Reg_AIGAIN = { AIGAIN, 4, 3 }; 				 		//R/W 24/32 0x400000 U Current Channel Gain (channel A) ¨C 23 bit
const ADE7953_Reg Reg_AVGAIN = { AVGAIN, 4, 3 }; 					 		//R/W 24/32 0x400000 U Voltage Channel Gain ¨C 23 bit
const ADE7953_Reg Reg_AWGAIN = { AWGAIN, 4, 3 }; 				 		//R/W 24/32 0x400000 U Active Power Gain (channel A) ¨C 23 bit
const ADE7953_Reg Reg_AVARGAIN = { AVARGAIN, 4, 3 }; 			 		//R/W 24/32 0x400000 U Reactive Power Gain (channel A) ¨C 23 bit
const ADE7953_Reg Reg_AVAGAIN = { AVAGAIN, 4, 3 }; 				 		//R/W 24/32 0x400000 U Apparent Power Gain (channel A) ¨C 23 bit
const ADE7953_Reg Reg_AIOS = { AIOS, 4, 3 };  							//R/W 24/32 0x00 S Current Channel Offset (channel A)
const ADE7953_Reg Reg_AIRMSOS = { AIRMSOS, 4, 3 }; 				 		//R/W 24/32 0x00 S IRMS Offset (channel A)
const ADE7953_Reg Reg_AVOS = { AVOS, 4, 3 }; 						 		//R/W 24/32 0x00 S Voltage Channel Offset
const ADE7953_Reg Reg_AVRMSOS = { AVRMSOS, 4, 3 }; 				 		//R/W 24/32 0x00 S VRMS Offset
const ADE7953_Reg Reg_AWATTOS = { AWATTOS, 4, 3 }; 				 		//R/W 24/32 0x00 S Active Power Offset Correction (channel A)
const ADE7953_Reg Reg_AVAROS = { AVAROS, 4, 3 }; 				 		//R/W 24/32 0x00 S Reactive Power Offset Correction (channel A)
const ADE7953_Reg Reg_AVAOS = { AVAOS, 4, 3 }; 					 		//R/W 24/32 0x00 S Apparent Power Offset Correction (channel A)
const ADE7953_Reg Reg_BIGAIN = { BIGAIN, 4, 3 }; 				 		//R/W 24/32 0x400000 S Current Channel Gain (channel B)
const ADE7953_Reg Reg_BVGAIN = { BVGAIN, 4, 3 }; 					 		//R/W 24/32 0x400000 S Voltage Channel Gain
const ADE7953_Reg Reg_BWGAIN = { BWGAIN, 4, 3 }; 				 		//R/W 24/32 0x400000 S Active Power Gain (channel B)
const ADE7953_Reg Reg_BVARGAIN = { BVARGAIN, 4, 3 }; 			 		//R/W 24/32 0x400000 S Reactive Power Gain (channel B)
const ADE7953_Reg Reg_BVAGAIN = { BVAGAIN, 4, 3 }; 				 		//R/W 24/32 0x400000 S Apparent Power Gain (channel B)
const ADE7953_Reg Reg_BIOS = { BIOS, 4, 3 }; 					 		//R/W 24/32 0x00 S Current Channel Offset (channel B)
const ADE7953_Reg Reg_BIRMSOS = { BIRMSOS, 4, 3 }; 				 		//R/W 24/32 0x00 S IRMS Offset (channel B)
const ADE7953_Reg Reg_BVOS = { BVOS, 4, 3 }; 						 		//R/W 24/32 0x00 S Voltage Channel Offset
const ADE7953_Reg Reg_BVRMSOS = { BVRMSOS, 4, 3 }; 				 		//R/W 24/32 0x00 S VRMS Offset
const ADE7953_Reg Reg_BWATTOS = { BWATTOS, 4, 3 }; 				 		//R/W 24/32 0x00 S Active Power Offset Correction (channel B)
const ADE7953_Reg Reg_BVAROS = { BVAROS, 4, 3 }; 				 		//R/W 24/32 0x00 S Reactive Power Offset Correction (channel B)
const ADE7953_Reg Reg_BVAOS = { BVAOS, 4, 3 }; 					 		//R/W 24/32 0x00 S Apparent Power Offset Correction (channel B)
const ADE7953_Reg Reg_Last_rwdata32 = { Last_rwdata32, 4, 3 }; 		 		//R 24/32 0x00 U Contains the data from the last successive 24/32 bit register communication





ADE_REGS_DATA	DATA_SAGCYC;  				//R/W 8 0x00 U Sag Line Cycle Register                                                                     
ADE_REGS_DATA DATA_LCYCMODE; 			  //R/W 8 0x40 U Line cycle Accumulation Mode Configuration                                                  
ADE_REGS_DATA	DATA_PGA_V; 					//R/W 8 0x00 U Voltage Channel Gain Configuration                                                          
ADE_REGS_DATA	DATA_PGA_IA; 				  //R/W 8 0x00 U Current Channel A Gain Configuration                                                        
ADE_REGS_DATA	DATA_PGA_IB; 				  //R/W 8 0x00 U Current Channel B Gain Configuration                                                        
ADE_REGS_DATA	DATA_Write_protect; 	//R/W 8 0x00 U Write protection bits [2:0]                                                                 
ADE_REGS_DATA	DATA_Last_Op; 				//R 8 0x00 U Contains the                                                                                  
ADE_REGS_DATA	DATA_Last_rwdata8; 		//R 8 0x00 U Contains the data from the last successful 8 bit register communication                       
ADE_REGS_DATA	DATA_EX_REF; 				  //R/W 8 0x00 U Reference input configuration. 0 for internal, set to 1 for external                        
                                                                                                                                             
                                                                                                                                             
ADE_REGS_DATA	DATA_ZXTOUT; 				  //W/R 16 0xFFFF U Zero Crossing Timeout                                                                    
ADE_REGS_DATA	DATA_LINECYC; 				//R/W 16 0x00 U Line Cycle Energy Accumulation Mode Line-Cycle Register                                    
ADE_REGS_DATA	DATA_CONFIG; 			  //W/R 16 0x8004 U Configuration Register                                                                   
ADE_REGS_DATA	DATA_CF1DEN; 			  //R/W 16 0x3F U CF1 Frequency Divider Denominator Register                                                 
ADE_REGS_DATA	DATA_CF2DEN; 			  //R/W 16 0x3F U CF2 Frequency Divider Denominator Register                                                 
ADE_REGS_DATA	DATA_CFMODE; 				  //R/W 16 0x300 U CF output Selection                                                                       
ADE_REGS_DATA	DATA_PHCALA; 			  //R/W 16 0x00 S Phase Calibration Register (channel A)                                                     
ADE_REGS_DATA	DATA_PHCALB; 			  //R/W 16 0x00 S Phase Calibration Register (channel B)                                                     
ADE_REGS_DATA	DATA_PFA; 						//R 16 0x00 S Power Factor channel A                                                                       
ADE_REGS_DATA	DATA_PFB; 						//R 16 0x00 S Power Factor Channel B                                                                       
ADE_REGS_DATA	DATA_Angle_A; 				//R 16 0x00 S Angle between voltage and Current A                                                          
ADE_REGS_DATA	DATA_Angle_B; 				//R 16 0x00 S Angle between voltage and Current B                                                          
ADE_REGS_DATA	DATA_PERIOD; 			  //R 16 0x00 U Period Register                                                                              
ADE_REGS_DATA	DATA_ALT_Output; 		  //R/W 16 0x00 U Alternative Output Functions                                                               
ADE_REGS_DATA	DATA_Last_Add; 			  //R 16 0x00 U Contains the address of the last successful communication                                    
ADE_REGS_DATA	DATA_Last_rwdata16; 		//R 16 0x00 U Contains the data from the last successive 16 bit register communication                     
                                                                                                                                             
                                                                                                                                             
                                                                                                                                             
ADE_REGS_DATA DATA_SAGLVL; 				  //R/W 24/32 0x00 U SAG Voltage Level                                                                       
ADE_REGS_DATA DATA_ACCMODE; 				//R/W 24/32 0x00 U Accumulation Mode                                                                       
ADE_REGS_DATA DATA_AP_NOLOAD; 			//R/W 24/32 0x00 U Active Power No Load Level                                                              
ADE_REGS_DATA DATA_VAR_NOLOAD; 		  //R/W 24/32 0xE419 U Reactive Power No Load Level                                                          
ADE_REGS_DATA DATA_VA_NLOAD; 			  //R/W 24/32 0xE419 U Apparent Power No Load Level                                                          
ADE_REGS_DATA DATA_AVA; 						//R 24/32 0x00 S Instantaneous Apparent Power A                                                            
ADE_REGS_DATA DATA_BVA; 						//R 24/32 0x00 S Instantaneous Apparent Power B                                                            
ADE_REGS_DATA DATA_AWATT; 					//R 24/32 0x00 S Instantaneous Active Power A                                                              
ADE_REGS_DATA DATA_BWATT; 					//R 24/32 0x00 S Instantaneous Active Power B                                                              
ADE_REGS_DATA DATA_AVAR; 					  //R 24/32 0x00 S Instantaneous Reactive Power A                                                            
ADE_REGS_DATA DATA_BVAR; 					  //R 24/32 0x00 S Instantaneous Reactive Power B                                                            
ADE_REGS_DATA DATA_IA; 						  //R 24/32 0x00 S Instantaneous Current Channel A                                                           
ADE_REGS_DATA DATA_IB; 						  //R 24/32 0x00 S Instantaneous Current Channel B                                                           
ADE_REGS_DATA DATA_VA; 						  //R 24/32 0x00 S Instantaneous Voltage Channel A                                                           
ADE_REGS_DATA DATA_VB; 						  //R 24/32 0x00 S Instantaneous Voltage Channel B                                                           
ADE_REGS_DATA DATA_IRMSA; 					//R 24/32 0x00 U IRMS Register A (channel A)                                                               
ADE_REGS_DATA DATA_IRMSB; 					//R 24/32 0x00 U IRMS Register B (channel B)                                                               
ADE_REGS_DATA DATA_VRMS; 					  //R 24/32 0x00 U VRMS Register                                                                             
ADE_REGS_DATA DATA_AENERGYA; 			  //R 24/32 0x00 S Active Energy Register (channel A)                                                        
ADE_REGS_DATA DATA_AENERGYB; 			  //R 24/32 0x00 S Active Energy Register (channel B)                                                        
ADE_REGS_DATA DATA_RENERGYA; 			  //R 24/32 0x00 S Reactive Energy Register (channel A)                                                      
ADE_REGS_DATA DATA_RENERGYB; 			  //R 24/32 0x00 S Reactive Energy Register (channel B)                                                      
ADE_REGS_DATA DATA_APENERGYA; 			//R 24/32 0x00 S Apparent Energy Register (channel A)                                                      
ADE_REGS_DATA DATA_APENERGYB; 			//R 24/32 0x00 S Apparent Energy Register (channel B)                                                      
ADE_REGS_DATA DATA_OVLVL; 					//R/W 24/32 0xFFFFFF U Over Voltage Level                                                                  
ADE_REGS_DATA DATA_OILVL; 					//R/W 24/32 0xFFFFFF U Over Current Level                                                                  
ADE_REGS_DATA DATA_VPEAK; 					//R 24/32 0x00 U Voltage Channel Peak Register                                                             
ADE_REGS_DATA DATA_RSTVPEAK; 			  //R 24/32 0x00 U Read Voltage Peak with Reset                                                              
ADE_REGS_DATA DATA_IAPEAK; 				  //R 24/32 0x00 U Current Channel A Peak Register                                                           
ADE_REGS_DATA DATA_RSTIAPEAK; 			//R 24/32 0x00 U Read Current Channel A Peak with Reset                                                    
ADE_REGS_DATA DATA_IBPEAK; 				  //R 24/32 0x00 U Current Channel B Peak Register                                                           
ADE_REGS_DATA DATA_RSTIBPEAK; 			//R 24/32 0x00 U Read Current Channel B Peak with Reset                                                    
ADE_REGS_DATA DATA_IRQEN; 					//R/W 24/32 0x100000 U Interrupt Enable Register                                                           
ADE_REGS_DATA DATA_IRQSTAT; 			//R 24/32 0x00 U Interrupt Status Register                                                                 
ADE_REGS_DATA DATA_RSTIRQSTAT; 		  //R 24/32 0x00 U Reset Interrupt Status register                                                           
ADE_REGS_DATA DATA_IRQENB; 				  //R/W 24/32 0x00 U Interrupt B Enable Register                                                             
ADE_REGS_DATA DATA_IRQSTATB; 			  //R 24/32 0x00 U Interrupt B Status Register                                                               
ADE_REGS_DATA DATA_RSTIRQSTATB; 	//R 24/32 0x00 U Reset Interrupt B Status register                                                         
ADE_REGS_DATA DATA_CRC; 						//R 32 0xC02F1AD4 U Check Sum                                                                              
ADE_REGS_DATA DATA_AIGAIN; 				  //R/W 24/32 0x400000 U Current Channel Gain (channel A) ?§C 23 bit                                        
ADE_REGS_DATA DATA_AVGAIN; 					//R/W 24/32 0x400000 U Voltage Channel Gain ?§C 23 bit                                                    
ADE_REGS_DATA DATA_AWGAIN; 				  //R/W 24/32 0x400000 U Active Power Gain (channel A) ?§C 23 bit                                           
ADE_REGS_DATA DATA_AVARGAIN; 			  //R/W 24/32 0x400000 U Reactive Power Gain (channel A) ?§C 23 bit                                         
ADE_REGS_DATA DATA_AVAGAIN; 				//R/W 24/32 0x400000 U Apparent Power Gain (channel A) ?§C 23 bit                                         
ADE_REGS_DATA DATA_AIOS;  					//R/W 24/32 0x00 S Current Channel Offset (channel A)                                                      
ADE_REGS_DATA DATA_AIRMSOS; 				//R/W 24/32 0x00 S IRMS Offset (channel A)                                                                 
ADE_REGS_DATA DATA_VOS; 						//R/W 24/32 0x00 S Voltage Channel Offset                                                                  
ADE_REGS_DATA DATA_VRMSOS; 				  //R/W 24/32 0x00 S VRMS Offset                                                                             
ADE_REGS_DATA DATA_AWATTOS; 				//R/W 24/32 0x00 S Active Power Offset Correction (channel A)                                              
ADE_REGS_DATA DATA_AVAROS; 				  //R/W 24/32 0x00 S Reactive Power Offset Correction (channel A)                                            
ADE_REGS_DATA DATA_AVAOS; 					//R/W 24/32 0x00 S Apparent Power Offset Correction (channel A)                                            
ADE_REGS_DATA DATA_BIGAIN; 				  //R/W 24/32 0x400000 S Current Channel Gain (channel B)                                                    
ADE_REGS_DATA DATA_BVGAIN; 					//R/W 24/32 0x400000 S Voltage Channel Gain                                                                
ADE_REGS_DATA DATA_BWGAIN; 				  //R/W 24/32 0x400000 S Active Power Gain (channel B)                                                       
ADE_REGS_DATA DATA_BVARGAIN; 			  //R/W 24/32 0x400000 S Reactive Power Gain (channel B)                                                     
ADE_REGS_DATA DATA_BVAGAIN; 				//R/W 24/32 0x400000 S Apparent Power Gain (channel B)                                                     
ADE_REGS_DATA DATA_BIOS; 					  //R/W 24/32 0x00 S Current Channel Offset (channel B)                                                      
ADE_REGS_DATA DATA_BIRMSOS; 				//R/W 24/32 0x00 S IRMS Offset (channel B)                                                                 
ADE_REGS_DATA DATA_VOS; 						//R/W 24/32 0x00 S Voltage Channel Offset                                                                  
ADE_REGS_DATA DATA_VRMSOS; 				  //R/W 24/32 0x00 S VRMS Offset                                                                             
ADE_REGS_DATA DATA_BWATTOS; 				//R/W 24/32 0x00 S Active Power Offset Correction (channel B)                                              
ADE_REGS_DATA DATA_BVAROS; 				  //R/W 24/32 0x00 S Reactive Power Offset Correction (channel B)                                            
ADE_REGS_DATA DATA_BVAOS; 					//R/W 24/32 0x00 S Apparent Power Offset Correction (channel B)                                            
ADE_REGS_DATA DATA_Last_rwdata32; 		//R 24/32 0x00 U Contains the data from the last successive 24/32 bit register communication
