#include "stringName.h" 
#include "net_device.h"

#include "ftp/ftp_server.h"
#include "path.h"
#include "debug.h"

uint_t ftpServerCheckUserCallback(FtpClientConnection *connection,
    const char_t *user);

uint_t ftpServerCheckPasswordCallback(FtpClientConnection *connection,
    const char_t *user,
    const char_t *password);

uint_t ftpServerGetFilePermCallback(FtpClientConnection *connection,
    const char_t *user,
    const char_t *path);

FtpServerSettings ftpServerSettings;
FtpServerContext ftpServerContext;

void net_ftp_server_start(net_device_t *net_dev)
{
    error_t error;
    
    //ftp初始化
    fsInit();
    //获取FTP默认设置
    ftpServerGetDefaultSettings(&ftpServerSettings);
    //绑定FTP到对应的接口
    ftpServerSettings.interface = net_dev->interface;
    //监听端口
    ftpServerSettings.port = FTP_PORT;
    //设置根目录
    strcpy(ftpServerSettings.rootDir, YAFFS_MOUNT_POINT);
#if 0
    //用户验证回调函数
    ftpServerSettings.checkUserCallback = ftpServerCheckUserCallback;
    //密码验证回调函数
    ftpServerSettings.checkPasswordCallback = ftpServerCheckPasswordCallback;
    //检索文件权限
    ftpServerSettings.getFilePermCallback = ftpServerGetFilePermCallback;
#endif

    //使用配置参数初始化 FTP 服务器
    error = ftpServerInit(&ftpServerContext, &ftpServerSettings);
    if (error)
    {
        TRACE_ERROR("初始化FTP服务器失败!\r\n");
    }
    //启动FTP服务器
    error = ftpServerStart(&ftpServerContext);
    if (error)
    {
        TRACE_ERROR("启动FTP服务器失败!\r\n");
    }
}

/**
 * @brief 用户验证回调函数
 * @param[in] FTP客户端连接句柄
 * @param[in] 以NULL结尾的用户名
 * @return Access status (FTP_ACCESS_ALLOWED, FTP_ACCESS_DENIED or FTP_PASSWORD_REQUIRED)
 **/

uint_t ftpServerCheckUserCallback(FtpClientConnection *connection,
    const char_t *user)
{
   //Debug message
    TRACE_INFO("FTP server: User verification\r\n");

       //Manage authentication policy
    if (!strcmp(user, "anonymous"))
    {
        return FTP_ACCESS_ALLOWED;
    }
    else if (!strcmp(user, "admin"))
    {
        return FTP_PASSWORD_REQUIRED;
    }
    else
    {
        return FTP_ACCESS_DENIED;
    }
}


/**
 * @brief 密码验证回调函数
 * @param[in] FTP客户端连接句柄
 * @param[in] 以NULL结尾的用户名
 * @param[in] 以NULL结尾的密码
 * @return Access status (FTP_ACCESS_ALLOWED or FTP_ACCESS_DENIED)
 **/

uint_t ftpServerCheckPasswordCallback(FtpClientConnection *connection,
    const char_t *user,
    const char_t *password)
{
    TRACE_INFO("FTP server: Password verification\r\n");
    //验证密码
    if (!strcmp(user, "admin") && !strcmp(password, "admin"))
    {
        return FTP_ACCESS_ALLOWED;
    }
    else
    {
        return FTP_ACCESS_DENIED;
    }
}


/**
 * @brief 检索文件权限
 * @param[in] FTP客户端连接句柄
 * @param[in] 以NULL结尾的用户名
 * @param[in] 文件路径
 * @return 指定文件的权限
 **/

uint_t ftpServerGetFilePermCallback(FtpClientConnection *connection,
    const char_t *user,
    const char_t *path)
{
    uint_t perm;

    //TRACE_INFO("FTP server: Checking access rights for %s\r\n", path);

    //管理权限
    if (!strcmp(user, "anonymous"))
    {
       //Allow read/write access to temp directory
        if (pathMatch(path, "/temp/*"))
            perm = FTP_FILE_PERM_LIST | FTP_FILE_PERM_READ | FTP_FILE_PERM_WRITE;
      //Allow read access only to other directories
        else
            perm = FTP_FILE_PERM_LIST | FTP_FILE_PERM_READ;
    }
    else if (!strcmp(user, "admin"))
    {
       //Allow read/write access
        perm = FTP_FILE_PERM_LIST | FTP_FILE_PERM_READ | FTP_FILE_PERM_WRITE;
    }
    else
    {
       //Deny access
        perm = 0;
    }

    //Return the relevant permissions
    return perm;
}
