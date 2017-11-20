#include "ymodem.h"
#include "yaffsfs.h"
#include "stringName.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include <stdlib.h>

struct custom_ctx
{
    struct rym_ctx parent;
    int fd;
    int flen;
    char fpath[256];
};

static enum rym_code _rym_bg(
        struct rym_ctx *ctx,
        uint8_t *buf,
        uint32_t len)
{
    struct custom_ctx *cctx = (struct custom_ctx*)ctx;
    strcpy(cctx->fpath, pathSystemDir);
    /* the buf should be the file name */
    strcat(cctx->fpath, (const char*)buf);
    cctx->fd = yaffs_open(cctx->fpath, O_CREAT | O_RDWR | O_TRUNC, S_IWRITE | S_IREAD);
    if (cctx->fd < 0)
    {
        rym_err_t err = yaffsfs_GetLastError();
        printf_safe("error creating file: %d\n", err);
        printf_safe("abort transmission\n");
        return RYM_CODE_CAN;
    }

    cctx->flen = atoi((const char*)buf+strlen((const char*)buf)+1);
    if (cctx->flen == 0)
        cctx->flen = -1;
    return RYM_CODE_ACK;
}

static enum rym_code _rym_tof(
        struct rym_ctx *ctx,
        uint8_t *buf,
        uint32_t len)
{
    struct custom_ctx *cctx = (struct custom_ctx*)ctx;
    if (cctx->flen == -1)
    {
        yaffs_write(cctx->fd, buf, len);
    }
    else
    {
        int wlen = len > cctx->flen ? cctx->flen : len;
        yaffs_write(cctx->fd, buf, wlen);
        cctx->flen -= wlen;
    }
    return RYM_CODE_ACK;
}

static enum rym_code _rym_end(
        struct rym_ctx *ctx,
        uint8_t *buf,
        uint32_t len)
{
    struct custom_ctx *cctx = (struct custom_ctx*)ctx;

    yaffs_close(cctx->fd);
    cctx->fd = -1;

    return RYM_CODE_ACK;
}

rym_err_t rym_write_to_file(void)
{
    rym_err_t res;
    struct custom_ctx *ctx = malloc(sizeof(*ctx));
    ctx->parent.buf = malloc(_RYM_STX_PKG_SZ);
    if (ctx->parent.buf == NULL)
        return -RT_ENOMEM;
    
    
    printf_safe("enter ymodem recv file mode\n");
    
    res = rym_recv_on_device(&ctx->parent, _rym_bg, _rym_tof, _rym_end, 1000);

    /* there is no Ymodem traffic on the line so print out info. */
    printf_safe("ymodem exit, code: %d\n", res);
//    if (res == 0)
//    {
//        printf_safe("file %s created\n", ctx->fpath);
//    }

    free(ctx->parent.buf);
    free(ctx);

    return res;
}
