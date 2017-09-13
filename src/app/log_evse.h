#ifndef LOG_EVSE_H
#define LOG_EVSE_H

 
#define defLogLevelState            0
#define defLogLevelWarning          1
#define defLogLevelCritical         2
#define defLogLevelFault            3

#define defLogStateOn               1
#define defLogStateOff              0



ErrorCode_t  AddEVSELog(uint8_t *path, uint8_t device, uint8_t level, uint8_t state, uint8_t *msg);

#endif