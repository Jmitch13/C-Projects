compiled on ubuntu 18.04 with GCC ver. 7.5.0

command to compile 

gcc -o test main.c filesystem.c disk.c

command to run

./test 

Comes with a main.c that tests basic functionality of the program and prints results of tests. 



------Function Descriptions-------

Making a new file:
    Starts with a single block allocated. Found by linear search to first unused block.

File close/open:
    Program finds file by searching list contained in the metablock, then gets pointed to the first block of the file
    Closing file makes the file descriptor NULL in the open file descriptor table. 

File read/write:
    File descriptor used to find cursor. For reads the data is read and keeps going through the pointers until all the requested bytes are read.
    Writes will write until the block is full and will keep writing to a new block if it needs more space and will point the first block to it.

lseek:
    Changes position of the cursor in open file descriptor table

fs_delete, fs_truncate:
    When delete, removes entry from table in metablock and sets the bytes back to "-999".
    Truncating will delete a key-value pair from the metablock.
