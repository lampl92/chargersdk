/**
* @file userlib_queue.h
* @brief
* @author rgw
* @version v1.0
* @date 2016-11-01
*/
#ifndef __USERLIB_QUEUE_H
#define __USERLIB_QUEUE_H

#include "FreeRTOS.h"
#include "semphr.h"

#define QUEUETYPE   unsigned char

typedef enum
{
    QUE_TRUE,
    QUE_FALSE
} QUERESULT;

#define QUE_OK      QUE_TRUE
#define QUE_FAIL    QUE_FALSE

typedef struct _Queue
{
    QUEUETYPE *elem;
    int length;
    int front;
    int rear;
    SemaphoreHandle_t xHandleMutexQue;
    QUERESULT (*isFull)(struct _Queue *q);
    QUERESULT (*EnElem)(struct _Queue *q, QUEUETYPE elem);
    QUERESULT (*isEmpty)(struct _Queue *q);
    QUERESULT (*DeElem)(struct _Queue *q, QUEUETYPE *pelem);
    QUERESULT (*Flush)(struct _Queue *q);
    void (*Delete)(struct _Queue *q);
} Queue;

Queue *QueueCreate(int len);
#endif
