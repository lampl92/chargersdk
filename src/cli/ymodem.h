/* The word "YMOD" is stand for "YModem". */
#ifndef __YMODEM_H__
#define __YMODEM_H__

#include "stm32f4xx.h"

#define YMOD_TICK_PER_SECOND 1000
typedef long                            ymod_err_t;      /**< Nbit CPU related date type */

/* SOH/STX + seq + payload + crc */
#define _YMOD_SOH_PKG_SZ (1+2+128+2)
#define _YMOD_STX_PKG_SZ (1+2+1024+2)

enum ymod_code {
    YMOD_CODE_NONE = 0x00,
    YMOD_CODE_SOH  = 0x01,
    YMOD_CODE_STX  = 0x02,
    YMOD_CODE_EOT  = 0x04,
    YMOD_CODE_ACK  = 0x06,
    YMOD_CODE_NAK  = 0x15,
    YMOD_CODE_CAN  = 0x18,
    YMOD_CODE_C    = 0x43,
};

/* YM error code
 *
 * We use the ym_err_t to return error values. We take use of current error
 * codes available in RTT and append ourselves.
 */
/* timeout on handshake */
#define YMOD_ERR_TMO  0x70
/* wrong code, wrong SOH, STX etc. */
#define YMOD_ERR_CODE 0x71
/* wrong sequence number */
#define YMOD_ERR_SEQ  0x72
/* wrong CRC checksum */
#define YMOD_ERR_CRC  0x73
/* not enough data received */
#define YMOD_ERR_DSZ  0x74
/* the transmission is aborted by user */
#define YMOD_ERR_CAN  0x75
#define RT_EOK                          0               /**< There is no error */
#define RT_ERROR                        1               /**< A generic error happens */
#define RT_ETIMEOUT                     2               /**< Timed out */
#define RT_EFULL                        3               /**< The resource is full */
#define RT_EEMPTY                       4               /**< The resource is empty */
#define RT_ENOMEM                       5               /**< No memory */
#define RT_ENOSYS                       6               /**< No system */
#define RT_EBUSY                        7               /**< Busy */
#define RT_EIO                          8               /**< IO error */
#define RT_EINTR                        9               /**< Interrupted system call */
#define RT_EINVAL                       10              /**< Invalid argument */


/* how many ticks wait for chars between packet. */
#ifndef YMOD_WAIT_CHR_TICK
#define YMOD_WAIT_CHR_TICK (YMOD_TICK_PER_SECOND * 1)
#endif
/* how many ticks wait for between packet. */
#ifndef YMOD_WAIT_PKG_TICK
#define YMOD_WAIT_PKG_TICK (YMOD_TICK_PER_SECOND * 1)
#endif
/* how many ticks between two handshake code. */
#ifndef YMOD_CHD_INTV_TICK
#define YMOD_CHD_INTV_TICK (YMOD_TICK_PER_SECOND * 1)
#endif

/* how many CAN be sent when user active end the session. */
#ifndef YMOD_END_SESSION_SEND_CAN_NUM
#define YMOD_END_SESSION_SEND_CAN_NUM  0x07
#endif

enum ymod_stage {
    YMOD_STAGE_NONE,
    /* set when C is send */
    YMOD_STAGE_ESTABLISHING,
    /* set when we've got the packet 0 and sent ACK and second C */
    YMOD_STAGE_ESTABLISHED,
    /* set when the sender respond to our second C and recviever got a real
     * data packet. */
    YMOD_STAGE_TRANSMITTING,
    /* set when the sender send a EOT */
    YMOD_STAGE_FINISHING,
    /* set when transmission is really finished, i.e., after the NAK, C, final
     * NULL packet stuff. */
    YMOD_STAGE_FINISHED,
};

struct ymod_ctx;
/* when receiving files, the buf will be the data received from ymodem protocol
 * and the len is the data size.
 *
 * TODO:
 * When sending files, the len is the buf size in YMOD. The callback need to
 * fill the buf with data to send. Returning YMOD_CODE_EOT will terminate the
 * transfer and the buf will be discarded. Any other return values will cause
 * the transfer continue.
 */
typedef enum ymod_code (*ymod_callback)(struct ymod_ctx *ctx, uint8_t *buf, uint32_t len);

/* Currently YMOD only support one transfer session(ctx) for simplicity.
 *
 * In case we could support multiple sessions in The future, the first argument
 * of APIs are (struct ym_ctx*).
 */
struct ymod_ctx
{
    ymod_callback on_data;
    ymod_callback on_begin;
    ymod_callback on_end;
    /* When error happened, user need to check this to get when the error has
     * happened. */
    enum ymod_stage stage;
    /* user could get the error content through this */
    uint8_t *buf;
};

/** recv a file on device dev with ymodem session ctx.
 *
 * If an error happens, you can get where it is failed from ctx->stage.
 *
 * @param on_begin The callback will be invoked when the first packet arrived.
 * This packet often contain file names and the size of the file, if the sender
 * support it. So if you want to save the data to a file, you may need to
 * create the file on need. It is the on_begin's responsibility to parse the
 * data content. The on_begin can be NULL, in which case the transmission will
 * continue without any side-effects.
 *
 * @param on_data The callback will be invoked on the packets received.  The
 * callback should save the data to the destination. The return value will be
 * sent to the sender and in turn, only YMOD_{ACK,CAN} is valid. When on_data is
 * NULL, YMOD will barely send ACK on every packet and have no side-effects.
 *
 * @param on_end The callback will be invoked when one transmission is
 * finished. The data should be 128 bytes of NULL. You can do some cleaning job
 * in this callback such as closing the file. The return value of this callback
 * is ignored. As above, this parameter can be NULL if you don't need such
 * function.
 *
 * @param handshake_timeout the timeout when hand shaking. The unit is in
 * second.
 */
ymod_err_t ymod_recv_on_device(struct ymod_ctx *ctx, ymod_callback on_begin, ymod_callback on_data, ymod_callback on_end,
        int handshake_timeout);

#endif
