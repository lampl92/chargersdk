#ifndef CFG_ORDER_H
#define CFG_ORDER_H

ErrorCode_t  AddOrderCfg(char *path, CON_t *pCON, echProtocol_t *pProto);
ErrorCode_t  AddOrderTmp(char *path, CON_t *pCON, echProtocol_t *pProto);
void RemoveOrderTmp(char *path);
ErrorCode_t GetNoPayOrder(char *path, OrderData_t *pOrder);

#endif
