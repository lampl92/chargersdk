#include "stm32f4xx.h"
#include "yaffsfs.h"
#include "FreeRTOS.h"
#include "task.h"

uint8_t set_upgrade_tmp(char *path, char *flg)
{
    int fd;
    int bw;
    fd = yaffs_open(path, O_CREAT | O_TRUNC | O_RDWR, S_IWRITE | S_IREAD);
    if (fd < 0)
    {
        ThrowFSCode(yaffs_get_error(), path, "set_upgrade_tmp()-open");
        return 0;
    }
    taskENTER_CRITICAL();
    bw = yaffs_write(fd, flg, 1);
    taskEXIT_CRITICAL();
    if (1 != bw)
    {
        ThrowFSCode(yaffs_get_error(), path, "set_upgrade_tmp()-write");
        yaffs_close(fd);
        return 0;
    }
    yaffs_close(fd);
    return 1;
}

uint8_t get_upgrade_tmp(char *path, char *flg)
{
    int fd;
    int rb;
    fd = yaffs_open(path, O_RDWR, S_IWRITE | S_IREAD);
    if (fd < 0)
    {
        ThrowFSCode(yaffs_get_error(), path, "get_upgrade_tmp()-open");
        return 0;
    }
    taskENTER_CRITICAL();
    rb = yaffs_read(fd, flg, 1);
    taskEXIT_CRITICAL();
    if (1 != rb)
    {
        ThrowFSCode(yaffs_get_error(), path, "set_upgrade_tmp()-read");
        yaffs_close(fd);
        return 0;
    }
    yaffs_close(fd);
    return 1;
}