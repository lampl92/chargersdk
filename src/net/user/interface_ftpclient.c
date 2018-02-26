#include "stm32f4xx.h"
#include "yaffsfs.h"
#include "debug.h"
#include "stringName.h"
#include "evse_globals.h"
#include "interface_ftpclient.h"

#include "ftp/ftp_client.h"

struct _ftp_ctx_save_file
{
    struct _ftp_ctx parent;
    int fd;
    char fpath[256];
};

/**
 * @fn static int _ftp_bg_cb(struct _ftp_ctx *ftp_ctx, char *data_in, uint32_t len)
 *
 * @brief FTP begin callback
 *
 * @param [in] ftp_ctx If non-null, context for the FTP.
 * @param [in] data_in If non-null, the data input.
 * @param          len     The length.
 *
 * @return An int.
 */

static int _ftp_bg_cb(struct _ftp_ctx *ftp_ctx, char *data_in, uint32_t len)
{
    struct _ftp_ctx_save_file *cctx = (struct _ftp_ctx_save_file*)ftp_ctx;
    sprintf(cctx->fpath, "%s%s", pathSystemDir, ftp_ctx->fname);
    cctx->fd = yaffs_open(cctx->fpath, O_CREAT | O_RDWR | O_TRUNC, S_IWRITE | S_IREAD);
    pechProto->info.ftp.ftp_proc.ulFTPReGetCnt = 1;
    pechProto->info.ftp.ftp_proc.ulFTPReOpenCnt = 1;
    pechProto->info.ftp.ftp_proc.ulRecvFileSize = 0;
    
    return 0;
}

/**
 * @fn static int _ftp_tof_cb(struct _ftp_ctx *ftp_ctx, char *data_in, uint32_t len)
 *
 * @brief FTP save to file callback
 *
 * @param [in] ftp_ctx If non-null, context for the FTP.
 * @param [in] data_in If non-null, the data input.
 * @param          len     The length.
 *
 * @return An int.
 */

static int _ftp_tof_cb(struct _ftp_ctx *ftp_ctx, char *data_in, uint32_t len)
{
    int n;
    int res;
    struct _ftp_ctx_save_file *cctx = (struct _ftp_ctx_save_file*)ftp_ctx;
    n = yaffs_write(cctx->fd, data_in, len);
    if (n != len)
    {
        res = yaffs_get_error();
        TRACE_ERROR("FTP SAVE FILE ERROR, code = %d", res);
        return res;
    }
    pechProto->info.ftp.ftp_proc.ulRecvFileSize += len;
    TRACE_DEBUG("filesize = %d（recv %d）\n", pechProto->info.ftp.ftp_proc.ulRecvFileSize, len);
    vTaskDelay(1);//给其他任务喘气的机会
    return 0;
}

/**
 * @fn static int _ftp_end_cb(struct _ftp_ctx *ftp_ctx, char *data_in, uint32_t len)
 *
 * @brief FTP end callback
 *
 * @param [in] ftp_ctx If non-null, context for the FTP.
 * @param [in] data_in If non-null, the data input.
 * @param          len     The length.
 *
 * @return An int.
 */

static int _ftp_end_cb(struct _ftp_ctx *ftp_ctx, char *data_in, uint32_t len)
{
    struct _ftp_ctx_save_file *cctx = (struct _ftp_ctx_save_file*)ftp_ctx;

    yaffs_close(cctx->fd);
    cctx->fd = -1;
    
    return 0;
}

/**
 * @fn  static error_t ftp_recv_data(struct _ftp_ctx *ctx, net_device_t *net_dev)
 *
 * @brief   FTP receive data
 *
 * @param [in]  ctx     If non-null, the context.
 * @param [in]  net_dev If non-null, the net development.
 *
 * @return  An error_t.
 */

static error_t ftp_recv_data(struct _ftp_ctx *ctx, net_device_t *net_dev)
{
    error_t error;
    size_t length;
    IpAddr ipAddr;
    FtpClientContext ftpContext;
    char_t buffer[256];

    TRACE_INFO("\r\n\r\n解析FTP服务器域名...\r\n");
    error = getHostByName(net_dev->interface, ctx->ftp_server, &ipAddr, 0);
    if (error)
    {
        TRACE_INFO("解析FTP服务器域名失败!\r\n");
        return error;
    }

    TRACE_INFO("连接FTP服务器 %s\r\n", ipAddrToString(&ipAddr, NULL));
    error = ftpConnect(&ftpContext, NULL, &ipAddr, ctx->port, FTP_NO_SECURITY | FTP_PASSIVE_MODE);

    if (error)
    {
        TRACE_INFO("连接FTP服务器失败!\r\n");
        return error;
    }

    TRACE_INFO("连接成功\r\n");

    do
    {
        //error = ftpLogin(&ftpContext, "anonymous", "password", "");
        error = ftpLogin(&ftpContext, ctx->user, ctx->pass, "");
        if (error)
            break;

        error = ftpChangeWorkingDir(&ftpContext, ctx->fpath);
        if (error)
            break;
        error = ftpOpenFile(&ftpContext, ctx->fname, FTP_FOR_READING | FTP_BINARY_TYPE);
        if (error)
            break;
        if (ctx->on_bg != NULL)
        {
            ctx->on_bg(ctx, NULL, 0);
        }

        while (1)
        {
            error = ftpReadFile(&ftpContext, buffer, sizeof(buffer), &length, 0);
            if (ctx->on_data != NULL)
            {
                ctx->on_data(ctx, buffer, length);
            }
            //是否到文件结尾?
            if (error)
            {
                if (ctx->on_end != NULL)
                {
                    ctx->on_end(ctx, NULL, 0);
                }
                break;
            }
        }
        error = ftpCloseFile(&ftpContext);
    } while (0);
    
    ftpClose(&ftpContext);
    TRACE_INFO("关闭FTP连接...\r\n");

    return error;
}

/**
 * @fn  int ftp_download_file(EchFtpCfg_t *pechFtp)
 *
 * @brief   FTP download file
 *
 * @param [in]  pechFtp If non-null, the pech FTP.
 *
 * @return  An int.
 */

int ftp_download_file(EchFtpCfg_t *pechFtp, net_device_t *net_dev)
{
    struct _ftp_ctx_save_file ctx;
    error_t error;
    
    strcpy(ctx.parent.ftp_server, pechFtp->strServer);
    ctx.parent.port = pechFtp->usPort;
    sprintf(ctx.parent.fpath, "/%s/", pechFtp->strNewVersion);
    strcpy(ctx.parent.fname, pechFtp->strNewFileName);
    strcpy(ctx.parent.user, pechFtp->strUser);
    strcpy(ctx.parent.pass, pechFtp->strPassword);
    ctx.parent.on_bg = _ftp_bg_cb;
    ctx.parent.on_data = _ftp_tof_cb;
    ctx.parent.on_end = _ftp_end_cb;
    
    error = ftp_recv_data(&ctx.parent, net_dev);
    if (error != NO_ERROR)
    {
        return 0;
    }
    else 
    { 
        return 1;
    }
}
