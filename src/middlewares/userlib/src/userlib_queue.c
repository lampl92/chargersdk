#include "../inc/userlib_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static QUERESULT isFull(Queue *q)
{
    if(q->front == (q->rear + 1) % q->length) //ÅÐÂú
    {
        return QUE_TRUE;
    }
    else
    {
        return QUE_FALSE;
    }
}
static QUERESULT EnElem(Queue *q, QUEUETYPE elem)
{
    if(isFull(q) == QUE_TRUE)
    {
        return QUE_FAIL;
    }
    q->elem[q->rear] = elem;
    q->rear = (q->rear + 1) % q->length; //²åÈë
    return QUE_OK;
}
static QUERESULT isEmpty(Queue *q)
{
    if(q->front == q->rear) //ÅÐ¿Õ
    {
        return QUE_TRUE;
    }
    else
    {
        return QUE_FALSE;
    }
}
static QUERESULT DeElem(Queue *q, QUEUETYPE *pelem)
{
    if(isEmpty(q) == QUE_TRUE)
    {
        return QUE_FAIL;
    }
    *pelem = q->elem[q->front];
    q->front = (q->front + 1) % q->length;
    return QUE_OK;
}
static QUERESULT FlushQueue(Queue *q)
{
    QUEUETYPE ch;
    QUERESULT res;
    do
    {
        res = DeElem(q, &ch);
    }
    while(res == QUE_OK);
    memset(q->elem, 0, q->length);
    q->front = 0;
    q->rear = 0;
}
static void DeleteQue(Queue *q)
{
    free(q->elem);
    free(q);
}
Queue *QueueCreate(int len)
{
    Queue *pQueue = (Queue *)malloc(sizeof(Queue));
    pQueue->length = len;
    pQueue->elem = (QUEUETYPE *)malloc(sizeof(QUEUETYPE) * pQueue->length);
    memset(pQueue->elem, 0, pQueue->length);
    pQueue->front = 0;
    pQueue->rear = 0;
    pQueue->xHandleMutexQue = xSemaphoreCreateMutex();
    pQueue->isEmpty = isEmpty;
    pQueue->isFull = isEmpty;
    pQueue->EnElem = EnElem;
    pQueue->DeElem = DeElem;
    pQueue->Flush = FlushQueue;
    pQueue->Delete = DeleteQue;
    return pQueue;
}
#if 0
void testqueue()
{
    Queue *q;
    unsigned char ch;
    int i;
    q = QueueCreate(200);
    for(i = 0; i < 128; i++)
    {
        q->EnElem(q, i);
    }
    for(i = 0; i < 128; i++)
    {
        q->DeElem(q, &ch);
        printf_safe("%d\n", ch);
    }
    q->Delete(q);
}
#endif
