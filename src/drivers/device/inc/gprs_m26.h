/**
* @file gprs_m26.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-27
*/
#ifndef  __GPRS_M26_H
#define  __GPRS_M26_H

/*device state*/
#define DS_GPRS_ON  1
#define DS_GPRS_OFF 0
#define DS_GPRS_ERR 0xff

/*device result*/
#define DR_GPRS_OK   1
#define DR_GPRS_ERR  0

/*device action*/
#define DA_GPRS_OFF  0
#define DA_GPRS_ON 1
#define DA_GPRS_RESET    2

typedef struct
{
    uint8_t state;
}dev_gprs_t;


#endif
