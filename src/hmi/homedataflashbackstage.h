#ifndef HOMEDATAFLASH_H
#define HOMEDATAFLASH_H

typedef enum gunenum
{
    GunfreeState       = 1,
    GunchargingState,
    GunchargedoneState,
    Gunerror,
}GUNState_E;

extern GUNState_E gunstate[2];

#endif