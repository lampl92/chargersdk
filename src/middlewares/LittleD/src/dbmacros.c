/**
* @file D:\Documents\Projects\chargersdk\src\middlewares\LittleD\src\dbmacros.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-06-23
*/

#include "dblogic\eet.h"

void *move_pointer_n_bytes(unsigned char *s, int n)
{
    return s + n;
}
void *pointer_at_n_bytes(unsigned char *s, int n)
{
    return s +n;
}
int pointer_byte_dist(unsigned char *p1, unsigned char *p2)
{
    return (int)(p1 - p2);
}
//MOVEPOINTERNUNITS(d, s, n, td, ts) (d) = (td)(((ts)((s)))+(n))
//MOVEPOINTERNUNITS(*npp, *npp, 1, db_eetnode_t*,db_eetnode_dbint_t*);
void *move_pointer_n_units(void *s, int n)
{
    if (((db_eetnode_t *)s)->type == DB_EETNODE_ATTR)
    {
        return (db_eetnode_attr_t*)s + n;
    }
    else if (((db_eetnode_t *)s)->type == DB_EETNODE_AGGR_ATTR)
    {
        return (db_eetnode_aggr_attr_t*)s + n;
    }
    else if (((db_eetnode_t *)s)->type == DB_EETNODE_AGGR_TEMP)
    {
        return (db_eetnode_aggr_temp_t*)s + n;
    }
    else if (((db_eetnode_t *)s)->type == DB_EETNODE_ALIAS)
    {
        return (db_eetnode_alias_t*)s + n;
    }
    else if (((db_eetnode_t *)s)->type == DB_EETNODE_CONST_DBINT)
    {
        return (db_eetnode_dbint_t*)s + n;
    }
    else if (((db_eetnode_t *)s)->type == DB_EETNODE_CONST_DBSTRING)
    {
        return (db_eetnode_dbstring_t*)s + n;
    }
}
