#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_
#include <unistd.h>
#include "disk.h"
#define MAX_DESC 32 //max concurrent file descriptors 
#define FILENAME_SIZE 15 //max file name length 

struct fildes {//single file descriptor 
	char fname[FILENAME_SIZE];
	int blk_num; //current block cursor is in
	off_t blk_off; //offset from beginning of block
};

struct fildes_table {//maps integers to file descriptors, max of 32 concurrent
	struct fildes* fds[MAX_DESC]; //all open file descriptors
	int num_open; //number of open file descriptors
};

int make_fs(char *disk_name);
int mount_fs(char *disk_name);
int umount_fs(char *disk_name);
int fs_open(char *name);
int fs_close(int fildes);
int fs_create(char *name);
int fs_delete(char *name);
int fs_read(int fildes, void *buffer, size_t nbyte);
int fs_write(int fildes, void *buffer, size_t nbyte);
int fs_get_filesize(int fildes);
int fs_lseek(int fildes, off_t offset);
int fs_truncate(int fildes, off_t length);

#endif


