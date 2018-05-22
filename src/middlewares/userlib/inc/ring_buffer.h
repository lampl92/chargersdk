#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "stm32f4xx.h"
#include "cmsis_os.h"

typedef struct __ring_buffer
{
    void *buffer;          //缓冲区
    uint32_t size;         //大小
    uint32_t in;           //入口位置
    uint32_t out;          //出口位置
    osMutexId lock;        //互斥锁
}ring_buffer_s;

//uint32_t __ring_buffer_get(ring_buffer_s *rb, void *buffer, uint32_t len);
//uint32_t __ring_buffer_put(ring_buffer_s *rb, void *buffer, uint32_t len);
//uint32_t __ring_buffer_len(const ring_buffer_s *rb);

uint32_t ring_buffer_get(ring_buffer_s *rb, void *buffer, uint32_t len);
uint32_t ring_buffer_put(ring_buffer_s *rb, void *buffer, uint32_t len);
uint32_t ring_buffer_len(const ring_buffer_s *rb);
void ring_buffer_free(ring_buffer_s *rb);
ring_buffer_s *ring_uint8_init(uint32_t size);

#endif