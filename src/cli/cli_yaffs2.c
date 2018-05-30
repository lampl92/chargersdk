#include "yaffsfs.h"
#include "bsp.h"
#include "tinysh.h"
#include "yaffs2misc.h"
#include "stringName.h"

#define YAFFS_MOUNT_POINT "/nand/"
#define FILE_PATH "/nand/system/foo.txt"

int random_seed;
int simulate_power_failure = 0;


void cli_ls_fn(int argc, char **argv)
{
    dump_directory_tree(YAFFS_MOUNT_POINT);
}
void cli_file_fn(int argc, char **argv)
{
    char path[64] = "/nand/system/";
    if (argc == 2)
    {
        strcat(path, argv[1]);
        dump_file(path);
    }
}
void cli_cat_fn(int argc, char **argv)
{
    char path[64] = "/nand/system/";
    if (argc == 2)
    {
        strcat(path, argv[1]);
        dump_file_data(path);
    }
}
void cli_rm_fn(int argc, char **argv)
{
    char path[64] = "/nand/system/";
    if (argc == 2)
    {
        strcat(path, argv[1]);
        yaffs_unlink(path);
    }
}
void cli_format_fn(int argc, char **argv)
{
    yaffs_format(YAFFS_MOUNT_POINT, 1, 0, 1);
}
tinysh_cmd_t cli_ls_cmd =
{
    0,
    "ls",
    "dump directory tree",
    0,
    cli_ls_fn,
    "<cr>",
    0,
    0
};

tinysh_cmd_t cli_file_cmd =
{
    0,
    "file",
    "dump file info",
    0,
    cli_file_fn,
    "<cr>",
    0,
    0
};
tinysh_cmd_t cli_cat_cmd =
{
    0,
    "cat",
    "dump file data",
    0,
    cli_cat_fn,
    "<cr>",
    0,
    0
};
tinysh_cmd_t cli_rm_cmd =
{
    0,
    "rm",
    "remove file",
    0,
    cli_rm_fn,
    "<cr>",
    0,
    0
};
tinysh_cmd_t cli_format_cmd =
{
    0,
    "format",
    "format and remount",
    0,
    cli_format_fn,
    "<cr>",
    0,
    0
};
extern void create_dir(char *dir);
int yaffs2_main()
{
    int output = 0;
    int output2 = 0;
    u8 str[] = "Yaffs test OK!\n";
    u8 rbuff[64] = { 0};
    u32 bw, br;
    yaffs_start_up();

    printf_safe("\n\n starting test\n");
    yaffs_set_trace(0);
    output = yaffs_mount(YAFFS_MOUNT_POINT);
    create_dir(pathSystemDir);
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
    bw = yaffs_write(output, str, strlen(str));
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
    output = yaffs_open(FILE_PATH, O_RDWR, 0);
    if (output >= 0) {  
        printf_safe("file created: %s\n", FILE_PATH); 
    }
    else {
        printf_safe("error\n yaffs failed to create the file: %s\nerror\n", FILE_PATH);
        return (0);
    }
 
    br = yaffs_read(output, rbuff, 64);
    printf_safe("bw = %d, br = %d, str=%s\n", bw, br, rbuff);
    
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
