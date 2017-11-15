#include "yaffsfs.h"
#include "bsp.h"
#include "tinysh.h"

#define YAFFS_MOUNT_POINT "/yaffs2/"
#define FILE_PATH "/yaffs2/foo.txt"

int random_seed;
int simulate_power_failure = 0;


int yaffs2_main()
{
    int output = 0;
    int output2 = 0;
    yaffs_start_up();

    printf_safe("\n\n starting test\n");
    yaffs_set_trace(0);
    output = yaffs_mount(YAFFS_MOUNT_POINT);

    if (output >= 0) {  
        printf_safe("yaffs mounted: %s\n", YAFFS_MOUNT_POINT); 
    }
    else {
        printf_safe("error\n yaffs failed to mount: %s\nerror\n", YAFFS_MOUNT_POINT);
        return (0);
    }
    //now create a file.
    output = yaffs_open(FILE_PATH, O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
    if (output >= 0) {  
        printf_safe("file created: %s\n", FILE_PATH); 
    }
    else {
        printf_safe("error\n yaffs failed to create the file: %s\nerror\n", FILE_PATH);
        return (0);
    }
    output2 = yaffs_close(output);
    if (output2 >= 0) {  
        printf_safe("file closed: %s\n", FILE_PATH); 
    }
    else {
        printf_safe("error\n yaffs failed to close the file: %s\nerror\n", FILE_PATH);
        return (0);
    }
    //unmount and remount the mount point.
    output = yaffs_unmount(YAFFS_MOUNT_POINT);
    if (output >= 0) {  
        printf_safe("yaffs unmounted: %s\n", YAFFS_MOUNT_POINT); 
    }
    else {
        printf_safe("error\n yaffs failed to unmount: %s\nerror\n", YAFFS_MOUNT_POINT);
        return (0);
    }
    output = yaffs_mount(YAFFS_MOUNT_POINT);
    if (output >= 0) {  
        printf_safe("yaffs mounted: %s\n", YAFFS_MOUNT_POINT); 
    }
    else {
        printf_safe("error\n yaffs failed to mount: %s\nerror\n", YAFFS_MOUNT_POINT);
        return (0);
    }
    //now open the existing file.
    output = yaffs_open(FILE_PATH, O_RDWR, S_IREAD | S_IWRITE);
    if (output >= 0) {  
        printf_safe("file created: %s\n", FILE_PATH); 
    }
    else {
        printf_safe("error\n yaffs failed to create the file: %s\nerror\n", FILE_PATH);
        return (0);
    }
    //close the file.
    output2 = yaffs_close(output);
    if (output2 >= 0) {  
        printf_safe("file closed: %s\n", FILE_PATH); 
    }
    else {
        printf_safe("error\n yaffs failed to close the file: %s\nerror\n", FILE_PATH);
        return (0);
    }

    	//unmount the mount point.
    output = yaffs_unmount(YAFFS_MOUNT_POINT);
    if (output >= 0) {  
        printf_safe("yaffs unmounted: %s\n", YAFFS_MOUNT_POINT); 
    }
    else {
        printf_safe("error\n yaffs failed to unmount: %s\nerror\n", YAFFS_MOUNT_POINT);
        return (0);
    }

    printf_safe("test passed. yay!\n");
	
}
void cli_yaffs2test_fnt(int argc, char **argv)
{
    yaffs2_main();
}
tinysh_cmd_t cli_yaffs2test_cmd =
{
    0,
    "yaffs2test",
    "yaffs2 test",
    0,
    cli_yaffs2test_fnt,
    "<cr>",
    0,
    0
};