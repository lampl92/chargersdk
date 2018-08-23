#ifndef CFG_ORDER_H
#define CFG_ORDER_H

ErrorCode_t  AddOrderCfg(char *path, OrderData_t *pOrder, echProtocol_t *pProto);
ErrorCode_t  AddOrderTmp(char *path, OrderData_t *pOrder, echProtocol_t *pProto);
void RemoveOrderTmp(char *path);
ErrorCode_t GetOrderBySN(char *path, uint64_t ullOrderSN, OrderData_t *pOrder);
ErrorCode_t GetOrderTmp(char *path, OrderData_t *pOrder);

#endif
