#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdlib.h>
#include "filesystem.h"
#include "disk.h"


#define METABLKID 0 // Block ID for the meta block
#define SMALLBUFF 256
#define FREETOKEN "-999" //token to signal block is free
#define RESERVED_CHARS 5 //reserved chars at start of each block
#define MAX_KV_SIZE 15 + RESERVED_CHARS + 2 //15 characters for file name, RESERVED_CHARS characters for meta, 2 characters for delimiters

#define isfree(buff) strcmp(buff, FREETOKEN) == 0

struct fildes_table open_fildes;//lists open file descriptors, global

//helper functions
off_t get_free_bl();
struct fildes* get_file(int fildes);
void build_block(int block_id);
int get_head(char* fname);
int get_next_blk(int init_blk);
int isOpen(char* filename);


int isOpen(char* filename) { //takes filename param and checks if it is open
	int i = 0;
	for(i = 0; i < MAX_DESC; i++) {
		if(open_fildes.fds[i] != NULL && strncmp(filename, open_fildes.fds[i]->fname, FILENAME_SIZE) == 0)		
			return 1;
	}
	return 0;
}

int make_fs(char* disk_name) {//take disk_name argument and sets the beginning of the block to be marked as free
	char buff[BLOCK_SIZE] = FREETOKEN;
	make_disk(disk_name);
	open_disk(disk_name);
	int i = 1;
	for(; i < DISK_BLOCKS; i++) {//reserve everything before 4096
		block_write(i, buff);
	}
	close_disk();
	return 0;
}

int mount_fs(char* disk_name) {//takes argument of disk name and opens it for use
	open_disk(disk_name);
	return 0;
}

int umount_fs(char* disk_name) {//unmounts disk
	close_disk();
	return -1;
}

int fs_open(char* name) {//takes name param and opens the file
	if(isOpen(name) == 1) {
        fprintf(stderr, "fs_open file is already open.\n");
		return -1;
	}
	int fd = 0;
	if(open_fildes.num_open >= MAX_DESC) {//check if there are too many descriptors open
        fprintf(stderr,"fs_open too many file descriptors open.\n");
        return -1;
	}
	for(fd = 0; fd < MAX_DESC; fd++) {
		if(open_fildes.fds[fd] == NULL)
			break;
	}
	struct fildes* new = malloc(sizeof(struct fildes));
	strncpy(new->fname, name, FILENAME_SIZE);

	new->blk_num = get_head(name);//make descriptor 
	new->blk_off = 0;
	open_fildes.fds[fd] = new;
	open_fildes.num_open++;
	return fd;
}

int fs_close(int fildes) { //takes fildes param and closes it
	if(fildes < 0 || fildes > MAX_DESC){
        fprintf(stderr,"fs_close invalid file description requested.\n");
		return -1;
    }
	free(open_fildes.fds[fildes]);
	open_fildes.fds[fildes] = NULL;
	open_fildes.num_open--;
	return -1;
}

int fs_create(char* name) {//takes name param and creates a new file
	char buff[BLOCK_SIZE];

	if(strlen(name) > FILENAME_SIZE)
    {
        fprintf(stderr,"fs_create name too big.\n");
		return -1;
    }
	off_t block = 0;//init block
	if((block = get_free_bl()) < 0) //return block if failed
		return block;

	if(get_head(name) != -1) {//check if exists
        fprintf(stderr,"fs_create file already exists.\n");
		return -1;
	}
	block_read(0, buff);//insert file record at end of the file list
	if(BLOCK_SIZE - strlen(buff) <= MAX_KV_SIZE){
        fprintf(stderr,"bock_read in fs_create cannot create file too many files already.\n");
		return -1;
    }

	sprintf(buff, "%s%s:%ld;", buff, name, block);
	block_write(0, buff);
	build_block(block);

	return 0;
}

int fs_delete(char* name) {//takes name param and deletes file, returns -1 if failed
	char buff[BLOCK_SIZE];
	if(isOpen(name) == 1){
        fprintf(stderr,"fs_delete file is already open.\n");
		return -1;
    }
	int curr_blk = get_head(name);
	if(curr_blk < 0) 
		return curr_blk;

	int fildes = fs_open(name); //delete blocks
	fs_truncate(fildes, 0);

	int name_end = 0; //holds offset in METABLKIDock
	char* kv = NULL;
	block_read(0, buff);
	for(kv = strtok(buff, ";"); kv != NULL; kv = strtok(NULL, ";")) {
		name_end = 0;
		while(kv[name_end] != ':') 
			name_end++;
		kv[name_end] = '\0';

		if(strncmp(kv, name, BLOCK_SIZE) == 0) {
			kv[name_end] = 'x'; 
			break;
		}

		kv[name_end] = ':';
		kv[strlen(kv)] = ';';
	}

	kv[0] = '\0';
	for(kv++; *kv != '\0'; kv++); 

	sprintf(buff, "%s%s", buff, kv + 1);
	block_write(0, buff);

	return 0;
}

int fs_read(int fildes, void* buf, size_t nbyte) {//reads from fildes, writes to buf, nbyte #bytes to read
	struct fildes* file = open_fildes.fds[fildes];
	char readBuff[BLOCK_SIZE];
	char* offset = NULL;
	int read = nbyte;
	
	if(file == NULL){
        fprintf(stderr,"fs_read invalid file descriptor.\n");
		return -1;
    }
	block_read(file->blk_num, readBuff);
	offset = readBuff + file->blk_off + RESERVED_CHARS - 1;//set first byte to offset value
	
	if(strlen(offset) >= nbyte){//if read stays within block, return it
		strncpy(buf, offset, nbyte);
		return read;
	}
	else{//else find next block
		strncpy(buf, offset, strlen(offset));
		nbyte -= strlen(offset);
		
		while((file->blk_num = get_next_blk(file->blk_num)) > 0){
			block_read(file->blk_num, readBuff);
			offset = readBuff + file->blk_off + RESERVED_CHARS - 1;

			if(strlen(offset) >= nbyte){//if end is in the block, return it
				strncat(buf, offset, nbyte);
				return read;
			}else{ //else add to string and repeat
				strncat(buf, offset, strlen(offset));
				nbyte -= strlen(offset);
			}
		}
		if(nbyte > 0)
			return read-nbyte;
	}

	return -1;
}

int fs_write(int fildes, void* buf, size_t nbyte) {//write to given fildes, buf contains data to write, nbyte is number of bytes to write
    char contents[BLOCK_SIZE]; //holds block contents
	char new_contents[BLOCK_SIZE]; //holds chunk to be written
	int nwrote = 0; //counter for number of bytes written
	struct fildes* file = get_file(fildes);//retrieve file descriptor 
	if(file == NULL){
        fprintf(stderr,"fs_write invalid file descriptor.\n");
		return -1;
    }
	if(nbyte > strlen(buf))
		nbyte = strlen(buf);//write to file

	while(nwrote < nbyte) {
		block_read(file->blk_num, contents);//write new block
		contents[file->blk_off + RESERVED_CHARS - 1] = '\0';
		int block_space = BLOCK_SIZE - RESERVED_CHARS - file->blk_off;
		strncpy(new_contents, buf, block_space);
		new_contents[block_space] = '\0';
		sprintf(contents, "%s%s", contents, new_contents);
		block_write(file->blk_num, contents);

		int nwrite = (block_space > strlen(buf)) ? strlen(buf) : block_space;
		file->blk_off += nwrite;
		nwrote += nwrite;
		buf += nwrite;

		if(file->blk_off >= BLOCK_SIZE - RESERVED_CHARS) {//new block allocation 
			off_t nblock = get_free_bl();
			if(nblock < 0)
				return nwrote;

			build_block(nblock);
			block_read(file->blk_num, contents);//link block to file
			sprintf(contents, "%04ld%s", nblock, contents + RESERVED_CHARS);
			block_write(file->blk_num, contents);

			file->blk_num = nblock;
			file->blk_off = 0;
		}
	}

	return nwrote;
}

int fs_get_filesize(int fildes) {//takes param fildes and returns number of blocks
	struct fildes* file = get_file(fildes);
	if(file == NULL){
        fprintf(stderr,"fs_get_filesize invalid file descriptor.\n");
		return -1;
    }
	int blk_count = 0;
	int curr_blk = 0;
	for(curr_blk = get_head(file->fname); curr_blk > 0; curr_blk = get_next_blk(curr_blk)) {
		blk_count++;
	}
	return blk_count * BLOCK_SIZE;
}

int fs_lseek(int fildes, off_t offset) {//seeks to offset, takes fildes
	struct fildes* file = get_file(fildes);
	if(file == NULL)
		return -1;

	file->blk_num = get_head(file->fname);

	int blk_stop = offset / BLOCK_SIZE; //number of blocks from the start of the file
	int off_stop = offset % BLOCK_SIZE; //get block offset
	int i = 0;
	int curr_blk = file->blk_num;
	for(i = 0; i < blk_stop; i++) {
		curr_blk = get_next_blk(file->blk_num);
		if(curr_blk == 0) //break if last block
			break;
		else if(curr_blk < 0){
            fprintf(stderr,"fs_lseek error.\n");
			return curr_blk;
        }
	}

	if(i != blk_stop) {//if the seek offset is out of the file return an error
        fprintf(stderr,"fs_lseek seek past EOF.\n");
		return -1;
	}

	file->blk_num = curr_blk;
	file->blk_off = off_stop;
	return 0;
}

int fs_truncate(int fildes, off_t length) { //Takes a fildes and truncates it to the length parameter
	struct fildes* file = get_file(fildes);
	if(file == NULL){ //if file dne return an error
        fprintf(stderr,"fs_truncate file descriptor not found.\n");
		return -1;
    }

	int flen = fs_get_filesize(fildes);
	if(length > flen)
		return 0;

	char buff[BLOCK_SIZE];
	fs_lseek(fildes, length);
	int curr_blk = file->blk_num;
	int last_blk = curr_blk;
	while(curr_blk > 0) {
		curr_blk = get_next_blk(curr_blk);
		block_read(last_blk, buff);//deallocate block
		sprintf(buff, "%s", FREETOKEN);
		block_write(last_blk, buff);
		last_blk = curr_blk;
	}

	return 0;
}

/**
 * Print out the contents of a specific block
 */
void print_block(int block) {
	char buff[BLOCK_SIZE];
	block_read(block, buff);
	printf("Block %d: %s\n", block, buff);
}

off_t get_free_bl() {//search disk for first available block
	char buff[BLOCK_SIZE];
	int i = 1;
	for(; i < DISK_BLOCKS; i++) {
		block_read(i, buff);
		if(isfree(buff))
			return i;
	}
    fprintf(stderr,"get_free_bl no blocks on disk.\n");
	return -1;
}

struct fildes* get_file(int fildes) {//returns file given a file descriptor
	if(fildes < 0 || fildes > MAX_DESC)
		return NULL;//return null if fildescriptor doesnt exist

	return open_fildes.fds[fildes];
}

void build_block(int block_id) {//makes a new block, initialises to all 0's
	char buff[5] = "0000";
	block_write(block_id, buff);
}

int get_head(char* fname) {//return block that is the first in the list for a file
	char buff[BLOCK_SIZE];
	char *kv = NULL;
	int i = 0;
	int file_found = 0; 
	block_read(0, buff);
	for(kv = strtok(buff, ";"); kv != NULL; kv = strtok(NULL, ";")) {
		i = 0;
		while(kv[i] != ':')
			i++;
		kv[i] = '\0';

		if(strncmp(fname, kv, FILENAME_SIZE) == 0) {//if file found, break
			file_found = 1;
			break;
		}
	}

	if(!file_found)
		return -1;

	return atoi(kv + i + 1);
}

int get_next_blk(int init_blk) {//takes param init_blk,  gets the next block
	char buff[BLOCK_SIZE];
	block_read(init_blk, buff);
	buff[RESERVED_CHARS - 1] = '\0';
	return atoi(buff);
}