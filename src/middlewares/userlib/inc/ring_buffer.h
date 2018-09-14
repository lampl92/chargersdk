#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "stm32f4xx.h"
#include "cmsis_os.h"

typedef struct __ring_buffer
{
    void *buffer;          //������
    uint32_t size;         //��С
    uint32_t in;           //���λ��
    uint32_t out;          //����λ��
    osMutexId lock;        //������
}ring_buffer_s;

//uint32_t __ring_buffer_get(ring_buffer_s *rb, void *buffer, uint32_t len);
//uint32_t __ring_buffer_put(ring_buffer_s *rb, void *buffer, uint32_t len);
//uint32_t __ring_buffer_len(const ring_buffer_s *rb);

uint32_t ring_buffer_get(ring_buffer_s *rb, void *buffer, uint32_t len);
uint32_t ring_buffer_put(ring_buffer_s *rb, void *buffer, uint32_t len);
uint32_t ring_buffer_len(const ring_buffer_s *rb);
void ring_buffer_free(ring_buffer_s *rb);
ring_buffer_s *ring_uint8_init(uint32_t size);
ring_buffer_s *ring_double_init(uint32_t size);
uint32_t ring_double_put(ring_buffer_s *rb, void *buffer, uint32_t len);
uint32_t ring_double_get(ring_buffer_s *rb, void *buffer, uint32_t len);

#endif