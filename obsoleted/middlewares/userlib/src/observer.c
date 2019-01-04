/**
* @file observer.c
* @brief 订阅发布设计模式
* @author rgw
* @version v1.0
* @date 2016-12-14
*/
#include <stdlib.h>
#include <string.h>
#include "observer.h"

/** @brief 订阅者析构函数
 *
 * @param pObserver void* 订阅者实例指针
 * @return void
 *
 */
static void ObserverDelete(void *pObserver)
{
    free(pObserver);
}

/** @brief 订阅者构造函数
 *
 * @param Name char*
 * @param pSub void*
 * @param Size size_t
 * @return Observer_t*
 *
 */
Observer_t *ObserverCreate(char *Name, void *pSub, size_t Size)
{
    Observer_t *pObserver = (Observer_t *)malloc(Size);
    pObserver->Name = Name;
    pObserver->pSub = pSub;
    pObserver->Delete = ObserverDelete;
    return pObserver;
}


/** @brief 发布者析构函数
 *
 * @param pSubject void* 发布者实例指针
 * @return void
 *
 */
static void SubjectDelete(void *pSubject)
{
    free(pSubject);
}

/** @brief 发布者构造函数
 *
 * @param Size size_t
 * @return Subject_t*
 *
 */
Subject_t *SubjectCreate(size_t Size)
{
    Subject_t *pSubject = (Subject_t *)malloc(Size);
    pSubject->Delete = SubjectDelete;
    return pSubject;
}
