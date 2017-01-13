/**
* @file observer.c
* @brief ���ķ������ģʽ
* @author rgw
* @version v1.0
* @date 2016-12-14
*/
#include <stdlib.h>
#include <string.h>
#include "observer.h"

/** @brief ��������������
 *
 * @param pObserver void* ������ʵ��ָ��
 * @return void
 *
 */
static void ObserverDelete(void *pObserver)
{
    free(pObserver);
}

/** @brief �����߹��캯��
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


/** @brief ��������������
 *
 * @param pSubject void* ������ʵ��ָ��
 * @return void
 *
 */
static void SubjectDelete(void *pSubject)
{
    free(pSubject);
}

/** @brief �����߹��캯��
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
