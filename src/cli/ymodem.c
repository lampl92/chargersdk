/*
 * File      : ymodem.c
 * COPYRIGHT (C) 2012, Shanghai Real-Thread Technology Co., Ltd
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-04-14     Grissiom     initial implementation
 */

#include "ymodem.h"
#include "bsp.h"
#include "cli_main.h"
#include <stdlib.h>

#if USE_FreeRTOS
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#endif

static const uint16_t ccitt_table[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};
uint16_t CRC16(unsigned char *q, int len)
{
    uint16_t crc = 0;

    while (len-- > 0)
        crc = (crc << 8) ^ ccitt_table[((crc >> 8) ^ *q++) & 0xff];
    return crc;
}
static uint32_t ymod_read(uint8_t *pbuff, uint32_t rlen, uint32_t timeout_ms)
{
    uint32_t len;
    len = uart_read_wait(cli_huart, pbuff, rlen, timeout_ms);
    return len;
}
static uint32_t ymod_write(uint8_t *pbuff, uint32_t wlen)
{
    uint32_t len;
    taskENTER_CRITICAL();
    len = uart_write_fast(cli_huart, pbuff, wlen);
    taskEXIT_CRITICAL();
    return len;

}
// we could only use global varible because we could not use
// rt_device_t->user_data(it is used by the serial driver)...
static struct ymod_ctx *_ymod_the_ctx;



static enum ymod_code _ymod_read_code(
    struct ymod_ctx *ctx,
    uint32_t timeout)
{
    if (ymod_read(ctx->buf, 1, timeout) == 1)
    {
        return *ctx->buf;
    }
    else
    {
        return YMOD_CODE_NONE;
    }
}

/* the caller should at least alloc _YMOD_STX_PKG_SZ buffer */
static uint32_t _ymod_read_data(
    struct ymod_ctx *ctx,
    uint32_t len)
{
    /* we should already have had the code */
    uint8_t *buf = ctx->buf + 1;
    uint32_t readlen = 0;
    uint32_t currlen = 0;

    do
    {
        readlen = ymod_read(buf + currlen, len - currlen, YMOD_WAIT_CHR_TICK);
        currlen += readlen;
        if (currlen >= len)
            return currlen;
    }
    while (readlen > 0);

    return currlen;
}

static uint32_t _ymod_putchar(uint8_t code)
{
    ymod_write(&code, sizeof(code));
    return 1;
}

static ymod_err_t _ymod_do_handshake(
    struct ymod_ctx *ctx,
    int tm_sec)
{
    enum ymod_code code;
    uint32_t i;
    uint16_t recv_crc, cal_crc;
    uint32_t pkg_sz;

    ctx->stage = YMOD_STAGE_ESTABLISHING;
    /* send C every second, so the sender could know we are waiting for it. */
    for (i = 0; i < tm_sec; i++)
    {
        _ymod_putchar(YMOD_CODE_C);
        code = _ymod_read_code(ctx, YMOD_CHD_INTV_TICK);
        if (code == YMOD_CODE_SOH)
        {
            pkg_sz = _YMOD_SOH_PKG_SZ;
            break;
        }
        if (code == YMOD_CODE_STX)
        {
            pkg_sz = _YMOD_STX_PKG_SZ;
            break;
        }
    }
    if (i == tm_sec)
        return -YMOD_ERR_TMO;

    i = _ymod_read_data(ctx, pkg_sz - 1);
    if (i != (pkg_sz - 1))
        return -YMOD_ERR_DSZ;

    /* sanity check */
    if (ctx->buf[1] != 0 || ctx->buf[2] != 0xFF)
        return -YMOD_ERR_SEQ;

    recv_crc = (uint16_t)(*(ctx->buf + pkg_sz - 2) << 8) | *(ctx->buf + pkg_sz - 1);
    cal_crc = CRC16(ctx->buf + 3, pkg_sz - 5);
    if (recv_crc != cal_crc)
        return -YMOD_ERR_CRC;

    if (ctx->buf[3] == 0)
    {
        _ymod_putchar(YMOD_CODE_ACK);
        ctx->stage = YMOD_STAGE_FINISHED;
        return RT_EEMPTY;
    }
    else
    {
        /* congratulations, check passed. */
        if (ctx->on_begin && ctx->on_begin(ctx, ctx->buf + 3, 1024) != YMOD_CODE_ACK)
            return -YMOD_ERR_CAN;
    }
    return RT_EOK;
}

static ymod_err_t _ymod_trans_data(
    struct ymod_ctx *ctx,
    uint32_t data_sz,
    enum ymod_code *code)
{
    const uint32_t tsz = 2 + data_sz + 2;
    uint16_t recv_crc;

    /* seq + data + crc */
    uint32_t i = _ymod_read_data(ctx, tsz);
    if (i != tsz)
        return -YMOD_ERR_DSZ;

    if ((ctx->buf[1] + ctx->buf[2]) != 0xFF)
    {
        return -YMOD_ERR_SEQ;
    }

    /* As we are sending C continuously, there is a chance that the
     * sender(remote) receive an C after sending the first handshake package.
     * So the sender will interpret it as NAK and re-send the package. So we
     * just ignore it and proceed. */
    if (ctx->stage == YMOD_STAGE_ESTABLISHED && ctx->buf[1] == 0x00)
    {
        *code = YMOD_CODE_NONE;
        return RT_EOK;
    }

    ctx->stage = YMOD_STAGE_TRANSMITTING;

    /* sanity check */
    recv_crc = (uint16_t)(*(ctx->buf + tsz - 1) << 8) | *(ctx->buf + tsz);
    if (recv_crc != CRC16(ctx->buf + 3, data_sz))
        return -YMOD_ERR_CRC;

    /* congratulations, check passed. */
    if (ctx->on_data)
        *code = ctx->on_data(ctx, ctx->buf + 3, data_sz);
    else
        *code = YMOD_CODE_ACK;
    return RT_EOK;
}

static ymod_err_t _ymod_do_trans(struct ymod_ctx *ctx)
{
    _ymod_putchar(YMOD_CODE_ACK);
    _ymod_putchar(YMOD_CODE_C);

    ctx->stage = YMOD_STAGE_ESTABLISHED;

    while (1)
    {
        ymod_err_t err;
        enum ymod_code code;
        uint32_t data_sz, i;

        code = _ymod_read_code(ctx, YMOD_WAIT_PKG_TICK);
        switch (code)
        {
        case YMOD_CODE_SOH:
            data_sz = 128;
            break;
        case YMOD_CODE_STX:
            data_sz = 1024;
            break;
        case YMOD_CODE_EOT:
            return RT_EOK;
        case YMOD_CODE_CAN:
            return -YMOD_ERR_CAN;
        default:
            while (ymod_read(ctx->buf, _YMOD_STX_PKG_SZ, YMOD_WAIT_PKG_TICK) != 0)
                ;
            memset(ctx->buf, 0, _YMOD_STX_PKG_SZ);
            _ymod_putchar(YMOD_CODE_NAK);
            continue;
//            return -YMOD_ERR_CODE;
        };

        err = _ymod_trans_data(ctx, data_sz, &code);
        if (err != RT_EOK)
        {
            while (ymod_read(ctx->buf, _YMOD_STX_PKG_SZ, YMOD_WAIT_PKG_TICK) != 0)
                ;
            memset(ctx->buf, 0, _YMOD_STX_PKG_SZ);
            _ymod_putchar(YMOD_CODE_NAK);
            continue;
            //return err;
        }
        switch (code)
        {
        case YMOD_CODE_CAN:
            /* the spec require multiple CAN */
            for (i = 0; i < YMOD_END_SESSION_SEND_CAN_NUM; i++)
            {
                _ymod_putchar(YMOD_CODE_CAN);
            }
            return -YMOD_ERR_CAN;
        case YMOD_CODE_ACK:
            _ymod_putchar(YMOD_CODE_ACK);
            break;
        default:
            // wrong code
            break;
        };
    }
}

static ymod_err_t _ymod_do_fin(struct ymod_ctx *ctx)
{
    enum ymod_code code;
    uint16_t recv_crc;
    uint32_t i;

    ctx->stage = YMOD_STAGE_FINISHING;

    /* we already got one EOT in the caller. invoke the callback if there is
     * one. */
    if (ctx->on_end)
        ctx->on_end(ctx, ctx->buf + 3, 128);

    _ymod_putchar(YMOD_CODE_NAK);
    code = _ymod_read_code(ctx, YMOD_WAIT_PKG_TICK);
    if (code != YMOD_CODE_EOT)
        return -YMOD_ERR_CODE;

    _ymod_putchar(YMOD_CODE_ACK);

    return RT_EOK;
}

static ymod_err_t _ymod_do_recv(
    struct ymod_ctx *ctx,
    int handshake_timeout)
{
    ymod_err_t err;

    ctx->stage = YMOD_STAGE_NONE;

    err = _ymod_do_handshake(ctx, handshake_timeout);
    if (err == RT_EEMPTY)
        return RT_EEMPTY;
    if (err != RT_EOK)
        return err;

    err = _ymod_do_trans(ctx);
    if (err != RT_EOK)
        return err;

    return _ymod_do_fin(ctx);
}

ymod_err_t ymod_recv_on_device( struct ymod_ctx *ctx, ymod_callback on_begin, ymod_callback on_data, ymod_callback on_end, int handshake_timeout)
{
    ymod_err_t res;

    ctx->on_begin = on_begin;
    ctx->on_data  = on_data;
    ctx->on_end   = on_end;

    res = _ymod_do_recv(ctx, handshake_timeout);
    while (res == RT_EOK)
    {
        res = _ymod_do_recv(ctx, 1);
    }

    return res;
}
