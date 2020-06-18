#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "filesystem.h"
#include "disk.h"
#define SIZE 4096

void stringDump(char* buffer, int nbytes);

int main(int argc, char** argv){
    char tempBuffer[SIZE];//temp buffer so we can generate garbage and send it to be written
    printf("--Making file system--\n");
    make_fs("myfilesystem");
    printf("--Done--\n");
    printf("--Mounting fs--\n");
    mount_fs("myfilesystem");
    printf("--Mounted--\n");
    printf("--Writing--\n");
    fs_create("foo");
    fs_create("bar");
    int file = fs_open("foo");
    int file2 = fs_open("bar");
    stringDump(tempBuffer, SIZE);
    printf("Attempting to write buffer contents to file.\n");
    fs_write(file, tempBuffer, SIZE);
    fs_write(file2, tempBuffer, SIZE);
    printf("Closing files...\n");
    fs_close(file2);
    fs_close(file);
    printf("--Closed--\n");
    printf("Attempting to write to a closed file...\n");
    if(fs_write(file, tempBuffer, SIZE) < 0){//test to see if we can write to a closed file or not, success if it doesn't
        printf("Successfuly failed to write to a closed file.\n");
    }else
    {
        printf("Failed to unsucessfuly write to a closed file.\n");
    }
    printf("Opening \"bar\" file and attempting to write data.\n");
    file2 = fs_open("bar");
    char temp[10] = "++++++++";//reopen file and write some more to it
    fs_write(file2, temp, SIZE);
    printf("File reopened.\n");

    printf("--Check file size--\n");//check file size of a file
    printf("Size of \"bar\" is: %d\n---------\n", fs_get_filesize(file2));

    printf("--Truncating file--\n");//test truncate then check file size again
    fs_truncate(file2, 4000);
    printf("Size is now: %d\n-------\n", fs_get_filesize(file2));

    printf("--checking delete--\n");
    fs_close(file2);
    fs_delete("bar");
    if(fs_open("bar") < 0 ){//delete file and then try to open it again to check it was actually deleted
        printf("file could not be opened, function works.\n");
    }
    else
    {
        printf("file could be written to after deletion, error.\n");
    }
    
    printf("--checking unmount--\n");//unmount the file system
    umount_fs("myfilesystem");
    printf("--unmounting complete--\n");
    return 0;
 

    


}


void stringDump(char* buffer, int nbytes){//takes parameter buf and fills it with 'a' upto nbytes
    for(int i=0; i <= nbytes -1 ; i++){
        buffer[i] = 'a';
    }
}