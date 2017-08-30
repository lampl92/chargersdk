/**
* @file bsp_uart_queue.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-02
*/
#include <stdlib.h>
#include <string.h>
#include "gdsl_queue.h"

gdsl_queue_t queGPRS;
gdsl_queue_t queCLI;

static gdsl_element_t alloc_char (void *ch)
{
    uint8_t *n = (uint8_t *) ch;
    uint8_t *value = (uint8_t *) malloc (sizeof (uint8_t));

    memcpy (value, n, sizeof (uint8_t));

    return (gdsl_element_t) value;
}

static void free_char (gdsl_element_t e)
{
    free (e);
}

void uart_queue_init(void)
{
    queGPRS = gdsl_queue_alloc ("queGPRS", alloc_char, free_char);
    queCLI = gdsl_queue_alloc ("queCLI", alloc_char, free_char);

}
