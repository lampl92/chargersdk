#ifndef YAFFS2MISC_H
#define YAFFS2MISC_H
int dump_file_data(char *fn);
void dump_file(const char *fn);
void dump_directory_tree(const char *dname);
Y_LOFF_T yaffs_ftell(int fd);
int find_file(const char *dir, const char *key_name, char *find_name);
int copy_in_a_file(const char *yaffsName, const char *inName);
#endif