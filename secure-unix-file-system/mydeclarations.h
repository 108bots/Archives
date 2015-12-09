/*
Secure File System - Declarations File
 * Author: Hemanth Srinivasan
 * Year: 2006
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


//#ifndef DISK_H
//#define DISK_H

/*#define SECTOR_SIZE 1024*/
#define SECTOR_SIZE 256
#define SECTORS_PER_TRACK 16
#define TRACKS_PER_CYLINDER 10
#define N_CYLINDERS 4
#define ROOT "DISK"
#define MAX_INODES 276 //changed
#define MAX_DATA 566
#define INODE_START 1 //changed - usable inode number , inode 0 reserved for root dir
#define INODE_END 275
#define DATA_START 73 //block 72 reserved for root dir data
#define DATA_END 638 //last block 639 intended to store login-pwd file
#define INODE_SIZE 64
#define STD_IN -2
#define STD_OUT -3
#define STD_ERR -4
#define LOGIN_BLK 639

//int initialize_disk(int);

/*read buffer from block# (0-639) */
//int disk_read(int, char*);

/* write buffer at block number# */
//int disk_write(int, char*);

//void disk_stats(int*, int*);

//#endif // DISK_H

/*****changing the block structure and variable types of inode and super block****************************/

/*The file system is organized as follows
Block size = 256 B
Total number of blocks = 640
Block 0 = boot block
Block 1,2 = super block
Block 3 - 71 = inode blocks
  total inode blocks = 69 (69*256), inode size = 64, inodes per block = 256/64 = 4, max no: of inodes = 4*69 = 276
Block 72 - 639 = Data blocks, total number of data blocks = 568
Max: no: of files in FS = 568 (min: file size = 1 blk)
*/

struct inode {
   char owner[6]; /*file owner-max 5 chars*/
   char type;     /* r-regular file d-directory file u-unsed*/
   char perm[7];   /*user/others permissions rwxrwx*/
   char mtime[15]; /*last file modification time*/
   char key[5];    /*password for the file*/
   char fsize[5];     /*file size in bytes*/
   char blk_addr[4][4]; /*disk block addresses- max file size is 4 blocks (ie 4*256 B), each blk addr is max 3 digit long*/
   
}; /*total size is 55-2(2 int vars) will be padded to 64B*/

struct boot_blk {
  /*block 0 is the boot block
    This is where the bootstrap 
	goes*/
	char *boot_msg;
} *boot_block;

struct super_blk {
/* block 1 & 2 is the super block*/
   char fs_size[7];
   char nfree_inodes[4];
   char nfree_dblocks[4];
   /*free inode numbers not inode block numbers*/
   char inodes_free_cache[24][4];   //sorted list, if value = -1, means used, once all values in the list become -1, we reload free block numbers from the disk
   /*free data block numbers*/
   char dblocks_free_cache[100][4]; //sorted list, if value = -1, means used, once all values in the list become -1, we reload free block numbers from the disk
} *super_block; /*total size = 360 B */

/* file descriptor table*/
struct fd_table {
   int of_index;

} fdesc_table[MAX_INODES];  // max number of files in the file system = max number of inodes ( however some
                            // inodes will be used for directories) 


/*open file table*/
struct open_fd{
   int offset;
   int inode_index;

} open_fdesc[64]; /* max number of open files = 64 */


/*inode table*/
struct cur_inode {
   short inum;
   struct inode *in_cur;

};

/*list of commands*/
enum cmd_list {
   CLEAR = 1,
   OPEN,
   READ,
   WRITE,
   LSEEK,
   CLOSE,
   CAT,
   TRUNC, /*delete*/
   MKDIR,
   CD,
   PWD,
   LS,
   CHMOD,
   FSYS,
   FILEDESC,
   LOGOUT,
   HELP
   
 };


