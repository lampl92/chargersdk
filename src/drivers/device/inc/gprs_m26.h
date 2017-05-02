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

/*at result*/
#define DR_AT_OK    1
#define DR_AT_ERR   0

/*poll state*/
#define DS_GPRS_POLL_AT         1
#define DS_GPRS_POLL_ATE0V1     2
#define DS_GPRS_POLL_CPIN       3
#define DS_GPRS_POLL_CREG       4
#define DS_GPRS_POLL_CGREG      5
#define DS_GPRS_POLL_APN        6
#define DS_GPRS_POLL_ATD        7
#define DS_GPRS_POLL_PPP        8
#define DS_GPRS_POLL_PPPDego    9
#define DS_GPRS_POLL_ERR        0xff
typedef struct
{
    uint8_t state;
    uint8_t pollstate;
}dev_gprs_t;


#endif
