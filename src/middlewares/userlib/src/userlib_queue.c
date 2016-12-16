#include "../inc/userlib_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static QUERESULT isFull(Queue *q)
{
    if(q->front == (q->rear + 1) % q->length) //����
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
    q->rear = (q->rear + 1) % q->length; //����
    return QUE_OK;
}
static QUERESULT isEmpty(Queue *q)
{
    if(q->front == q->rear) //�п�
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
    pQueue->isEmpty = isEmpty;
    pQueue->isFull = isEmpty;
    pQueue->EnElem = EnElem;
    pQueue->DeElem = DeElem;
    pQueue->Delete = DeleteQue;
    return pQueue;
}

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
        printf("%d\n", ch);
    }
    q->Delete(q);
}
