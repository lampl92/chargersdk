#ifndef __USERLIB_QUEUE_H
#define __USERLIB_QUEUE_H

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

    QUERESULT (*isFull)(struct _Queue *q);
    QUERESULT (*EnElem)(struct _Queue *q, QUEUETYPE elem);
    QUERESULT (*isEmpty)(struct _Queue *q);
    QUERESULT (*DeElem)(struct _Queue *q, QUEUETYPE *pelem);
    void (*Delete)(struct _Queue *q);
} Queue;

Queue *QueueCreate(int len);
#endif
