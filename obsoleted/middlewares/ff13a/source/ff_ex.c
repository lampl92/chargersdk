/**
* @file ff_ex.c
* @brief fatfs extensional function.
* @author rgw
* @version 1.0
* @date 2016-12-06
*/
#include "ff.h"

int f_getc (
    FIL *fp         /* Pointer to the file object */
)
{
    BYTE c;
    UINT rc;
    f_read(fp, &c, 1, &rc);
    if (rc != 1 || rc < 1)
    {
        return EOF;
    }
    return c;
}
