/*
Secure File System using Linux Diskfile
 * Author: Hemanth Srinivasan
 * Year: 2006
*/

//all initialization sections same as secFS, changes in input command line and further


#include "mydeclarations.h"

int inititalize_filesystem(void); 
void itoa(int, char []);
int write_super_block(void);
int read_super_block(void);
int write_inode(int, struct inode);
int read_inode(int, struct inode*);
int init_inode_blocks(void);
int initialize_rootdir(void);
char *get_cur_time(void);
int qsort_compfunc(const void*, const void*);
void qsort_superblock_cache(int, int);
void reload_supercache(int, int, int);
int read_input(void);
int open_file(void);
int search_cur_dir(void);
void print_inode(struct inode*);
int fsys_func(void);
int write_file (void);
void clear_filesystem(void);
void print_working_dir(void);
void logout_user (void);
void help_user(void);
int read_file(void);
int seek_file(void);
int cat_file (void);
int close_file(void);
void filedesc(void);
unsigned char* decrypt_file(char*, char*);
unsigned char* encrypt_file(char*, char*);
int chmod_file(void);
int list_dir(void);
int make_dir(void);
int change_dir(void);




/*disk functions*/
int initialize_disk(int);
static int n_to_cts(int, int*, int*, int*);
int disk_read(int, int, char*); //modified to include offset within a block
int disk_write(int, int, char*); //modified to include offset within a block
void disk_stats(int*, int*);

int itable_index = -1, oftable_index = -1, fdtable_index = 3; //fds 0,1,2 reserved
int used_dsmax = -1, used_inmax = -1; // used to search the super cache, highest value of the data block and inode block that has been used
                            // points to the starting point of the next free blocks to be loaded into
                            // the super cache

struct cur_inode cur_dir, inode_table[64];

int dir_size = 0, file_size = 0, parent_inum;
int inodes_max = MAX_INODES, datanodes_max = MAX_DATA;
int cur_fd;
char filename[12] = {'\0'}, mode[3] = {'\0'}, key[5] = {'\0'}; //open parameters


char root_key[] = "root"; //key to encrypt root (presently all) dir's key
char user_key[] = "root"; //key to encrypt the file key stored in the inode
char logpwd_key[] = "shb123"; //key to encrypt login/password file

char uname[6];
char upwd[6];
char path[256] = {'\0'};

enum cmd_list choice = 0;

static int NSECTORS = SECTORS_PER_TRACK *
                      TRACKS_PER_CYLINDER *
		              N_CYLINDERS;

static FILE *FD = NULL;
static int n_seeks = 0;
static int n_transfers = 0;
static int current_cylinder = 0;
static int current_track = -1;

char *cmdstr, full_cmdstr[1050];

int main () {
 
 char un[16], up[16], c;
 int i, x = 0, j, k, nb, rb, c_off, cat, cls, chm, l_s, mk, cd;
 int cnt = 0, pflg = 0, uflg = 0, uflg1 = 0, uexist = 1, usrflg =0;
 char *loginbuf, e_data[256], d_data[256], t_data[256];
 int ret;
 char ut;
 char tpath[256] = {'\0'};



 printf("\nWelcome to the Secure File System!!!");
 printf("\nInitializing the file system.... Please wait...\n");
if ( inititalize_filesystem() != -1)
  printf("\nFile System Initialized Successfully!!!\n");
 //exit(0);

 while (1) {
  //resetting the flags
  choice = 0; 
  cnt = 0;
  pflg = 0;
  uflg = 0;
  uflg1 = 0;
  uexist = 1;
  usrflg =0;
  cur_fd =0;
  fflush(stdin);

  do { 
   printf("\nTo coninue... \nExisting Users press 1\tNew Users press 2:");
   scanf("%c", &ut);
   if ( ut == 49 || ut == 50 )
	   break;
  printf("Invalid Choice\n"); 
   
  } while ( ut != 49 || ut != 50 );

  /*Read the loginpwd block*/
  loginbuf = (char *)malloc(256*sizeof(char));

  if(disk_read(LOGIN_BLK, 0, loginbuf) == -1)
         printf("\nError reading loginpwd block");

  strcpy(t_data, loginbuf);
  strcpy (d_data, (const char *)decrypt_file(logpwd_key, t_data));
  strcpy(loginbuf, d_data);

  switch (ut) {  /*login verification*/

		 case 49: /*Existing User*/
	             if(strlen(loginbuf) == 0)
				 {
				   printf("\nNo users registered. Use option 2!!!");
				   continue;
				 }
				 while (cnt < 5) {
				   printf("login:");
				   scanf("%s", uname);
				   printf("password:");
				   scanf("%s", upwd);
                /*   i = 0;
				   fflush(stdin);
				   while ((c = getch()) != '\n') {
					  upwd[i++] = c;
					   if (c == '\b') i--;
				   }
				   printf("\npwd %s", upwd);*/
                   j = 0;
				   while (j < strlen(loginbuf)) {
                     k = 0;
					 while (loginbuf[j] != ' ')
				       un[k++] = loginbuf[j++];
					 un[k] = '\0';
					 j++;
					 k = 0;
                     while (loginbuf[j] != '\n') 
                        up[k++] = loginbuf[j++];
					 up[k] = '\0';  
					 
					 if (strcmp(un, uname) == 0) {
	           		     uflg = 1;
			             if (strcmp(up, upwd) == 0) {
					       printf("login Successful\n");
                           pflg = 1;
			    	       break;
						 }
				         else
						 { 
					       printf("password incorrect!!\n");
					       break;
						 }

					  }
					  j++;
               
				   }
                   if (pflg) break;
			
                   if (!uflg && cnt == 4) {
	        		 printf("\nFailed to Authenticate User... Exiting!!!\n");
		             exit(0);
				   }

			       cnt++;
				 }
		         break;
 
         case 50: /*new user */
	     
	             while (uexist) {
				   printf("Enter user name:");
		           scanf("%s", uname);
		           uflg1 = 0;
				   j = 0;
                   while (j < strlen(loginbuf)) {
					 k= 0;
					 while (loginbuf[j] != ' ')
				       un[k++] = loginbuf[j++];
					 un[k] = '\0';
                     while (loginbuf[j] != '\n') j++;
					 j++;
					 if (strcmp(un, uname) == 0) {
				       printf("Username exists!!!\n");
					   uflg1 = 1;
				       break;
					 }
                     
				   }
				   if (uflg1) continue;

				   uexist = 0;
				 }
		         printf("Enter new password:");
				 scanf("%s", upwd);
				 
				 strcat(loginbuf, uname);
				 strcat(loginbuf, " ");
				 strcat(loginbuf, upwd);
				 strcat(loginbuf, "\n");
                 
				 strcpy(t_data, loginbuf);
                 strcpy (e_data, (const char *)encrypt_file(logpwd_key, t_data));
                 
                 if (disk_write(LOGIN_BLK, 0, e_data) != -1)
		           printf("\nSucessfully wrote to loginpwd block!!!");

				 printf("\nType <HELP> to get started...\n");
				 break;

		 default: 
			      break;

  } //end user switch

  free(loginbuf);

 /*setting up the path variable*/
  strcat(path, uname);
  strcat(path, " /"); // "/" indicating root dir
  
 //printf("path is : %s", path);

  while (choice != LOGOUT) {  
   fflush(stdin);
   free(cmdstr);
  // free(full_cmdstr);
   strcpy(tpath, path);

   printf ("%s", tpath);
   printf(">");
   choice = read_input();

  //printf("\npath after read is %s and %s", path, tpath);  
  
  if (choice == 0) continue;
   else if ( choice == -1 ) {
	 printf("\nInvalid Command\n");
	 continue;
   }
  
   switch (choice) {

   case CLEAR:     
		        clear_filesystem();
	            break;

    case OPEN: 
		        cur_fd = open_file();
				printf("\nFile Descriptor is %d\n", cur_fd);
				if(cur_fd == -2) {
				  printf("Usage: Open [filename] [mode] [key] <return>\n" );
				  printf("Use HELP command for more details...\n" );
				  
	              break;
				}
				else if(cur_fd == -1) {
				  printf("\nError in opening the file!!!\n" );
				  
	              break;
				}
		        
	            break;

    case READ:  
		        rb = read_file();
				printf("\nnbytes read %d\n", rb);
				if(rb == -2) {
				  printf("Usage: Read [fd] [nbytes] <return>\n" );
				  printf("Use HELP command for more details...\n" );
				  
	              break;
				}
				else if(rb == -1) {
				  printf("\nError in reading the file!!!\n" );
				  
	              break;
				}
	            break;

    case WRITE: 
		        nb = write_file();
				printf("\nnbytes written %d\n", nb);
				if(nb == -2) {
				  printf("Usage: Write [fd] [nbytes] [bytes] <return>\n" );
				  printf("Use HELP command for more details...\n" );
				  
	              break;
				}
				else if(nb == -1) {
				  printf("\nError in writing to the file!!!\n" );
				  
	              break;
				}
	            break;

    case LSEEK: 
		        c_off = seek_file();
				printf("\nCurrent Offset %d\n", c_off);
				if(c_off == -2) {
				printf("Usage: Lseek [fd] [offset] [whence] <return>\n" );
				  printf("Use HELP command for more details...\n" );
				  
	              break;
				}
				else if(c_off == -1) {
				  printf("\nError in Lseek to the file!!!\n" );
				  
	              break;
				}
	            break;

    case CLOSE: 
		        cls = close_file();
				printf("\nClose %d\n", cls);
				if(cls == -2) {
				  printf("Usage: Close [fd] <return>\n" );
				  printf("Use HELP command for more details...\n" );
				  
	              break;
				}
				else if(cls == -1) {
				  printf("\nError in closing file descriptor !!!\n");
				  
	              break;
				}

	            break;

    case CAT:   
		        cat = cat_file();
				printf("\nCat %d\n", cat);
				if(cat == -2) {
				  printf("Usage: Cat [filename] [key] <return>\n" );
				  printf("Use HELP command for more details...\n" );
				  
	              break;
				}
				else if(cat == -1) {
				  printf("\nError in cat to the file!!!\n" );
				  
	              break;
				}
	            break;

    case TRUNC: 
		        printf("Usage: Trunc [filename] <return>\n" );
		        
	            break;

    case MKDIR: 
		        mk = make_dir();
				printf("\nmkdir %d\n", mk);
				if(mk == -2) {
				  printf("Usage: Mkdir [dirname] <return>\n" );
				  printf("Use HELP command for more details...\n" );
				  
	              break;
				}
				else if(mk == -1) {
				  printf("\nError in creating the dir!!!\n" );
				  
	              break;
				}
	            break;

    case CD:    
		       
		        cd = change_dir();
				printf("\nchdir %d\n", cd);
				if(mk == -2) {
				  printf("Usage: Cd [dirpath] <return>\n" );
				  printf("Use HELP command for more details...\n" );
				  
	              break;
				}
				else if(cd == -1) {
				  printf("\nError in changing the dir!!!\n" );
				  
	              break;
				}
	            break;

    case PWD:  
		        print_working_dir();
	            break;

    case LS:   
		        l_s = list_dir();
				printf("\nLs %d\n", l_s);
				if(l_s == -2) {
				  printf("Usage: Ls <return>\n" );
				  printf("Use HELP command for more details...\n" );
				  
	              break;
				}
				else if(l_s == -1) {
				  printf("\nError in ls to the dir!!!\n" );
				  break;
				}
		      
	            break;

    case CHMOD: 
		        chm = chmod_file();
				printf("\nChmod %d\n", chm);
				if(chm == -2) {
			      printf("Usage: Chmod [filename] [perms] <return>\n" );
				  printf("Use HELP command for more details...\n" );
				  
	              break;
				}
				else if(chm == -1) {
				  printf("\nError in chmod to the file!!!\n" );
				  
	              break;
				}
	            break;

    case FSYS: 
		       if(strcmp(uname, "root") != 0) {
		          printf("\nOnly ROOT can execute this command!!!\n");
				 
				  break;
				}
                ret = fsys_func();
			//	printf("ret = %d", ret);
				if(ret == -2) {
				  printf("Usage: Fsys <return>\n" );
				  printf("Use HELP command for more details...\n" );
				 
				  break;
				}
				else if(ret == -1) {
				  printf("\nError in getting the file system information!!!\n" );
				  
	              break;
				}
			
				break;
    
	case FILEDESC: 
		           filedesc();
	               break;

    case LOGOUT: 
		         logout_user();
	             break;

    case HELP:  
		        help_user();				
	            break;

    default: printf("No such command\n" );
		     
	         break;


   } //end command switch


  } //end choice while
 
 } //end main usr while

 
}//end main01




int initialize_disk(int clr)
{
  char fn[100];
  char *un;
  long bs, n, i;
  struct stat statbuf;
  int ret;
  if (FD != NULL && !clr) {
    fprintf(stderr, "Initialize_disk has already been called\n");
    return -1;
  } else if (FD != NULL) {
    fclose(FD);
  }

  n_seeks = 0;
  n_transfers = 0;
  current_cylinder = 0;
  current_track = -1;

  bs = SECTOR_SIZE * SECTORS_PER_TRACK * TRACKS_PER_CYLINDER;

  un = getenv("USER");
  if (un == NULL) {
    fprintf(stderr, "Error: USER environment variable is not set\n");
    return -1;
  }
  printf("un= %s\n",un);
  sprintf(fn, "%s", ROOT );
  printf("fn= %s\n", fn);
  if (clr ) {
    FD = fopen(fn, "w+");

  }
  else {
    ret = stat(fn, &statbuf);
  
    if (errno == ENOENT || errno == EFAULT) {
      printf("Not found exisitng DISK, creating one for you..\n");
      clr = 1;
      FD = fopen(fn,"w+");
    }
    else if (ret != 0) {
      perror("stat");
    }
    else {
      FD = fopen(fn, "r+"); /* open existing DISK */
    }
  }
  if (FD == NULL) {
    fprintf(stderr, "Couldn't open disk file: %s\n", fn);
    return -1;
  }

  if (clr) {
    fprintf(stderr, "Clearing disk ... ");
    fflush(stderr);
    un = (char *) calloc(bs, 1);
    fseek(FD, 0L, 0);
    if (ftell(FD) != 0) {
      fprintf(stderr, "Error -- couldn't rewind fp for clearing.\n");
      return -1;
    }
    for (i = 0; i < N_CYLINDERS; i++) {
      n = fwrite(un, 1, bs, FD);
      if (n != bs) {
        fprintf(stderr, "Error -- fwrite failed\n");
        return -1;
      }
    }
    fprintf(stderr, "done\n");
    fflush(stderr);
  }
  if (fseek(FD, 0L, 2) == -1) {
    perror("fseek");
    
  }
  
  n = ftell(FD);
  if (n != bs * N_CYLINDERS) {
    fprintf(stderr, "Bad disk file size.  Should be %d.  Is %d\n",
            bs * N_CYLINDERS, n);
    return -1;
  }
  return 0;
}

static int n_to_cts(int n, int *c,int *t,int *s)
{
  if (n >= NSECTORS || n < 0) return -1;
  *s = n % SECTORS_PER_TRACK;
  n /= SECTORS_PER_TRACK;
  *t = n % TRACKS_PER_CYLINDER;
  *c = n / TRACKS_PER_CYLINDER;
  return 0;
}

/*read buffer from block# (0-639) */
int disk_read(int number, int off, char *buf) //off holds the number of bytes into the block
{
  int c, t, s, n;
  int seeks;

  if (n_to_cts(number, &c, &t, &s) < 0) return -1;

  if (FD == NULL) {
    fprintf(stderr, "Disk not initialized\n");
    return -1;
  }

  seeks = c - current_cylinder;
  if (seeks < 0) seeks = -seeks;
  n_seeks += seeks;
  current_cylinder = c;

  if (current_track != t) n_transfers++;
  current_track = t;
  
  fseek(FD, (long) (number * SECTOR_SIZE + off), 0);
  n = fread(buf, 1, SECTOR_SIZE, FD);
 //  n = fread(buf, 1, strlen(buf), FD);
  //printf("\nDisk_read data: %s\n", buf);

 // if (n != strlen(buf)) {
  if (n != SECTOR_SIZE) {
    fprintf(stderr, "Error, couldn't read %d bytes starting at %d\n",
            SECTOR_SIZE, number*SECTOR_SIZE);
    return -1;
  }
  //return 0;
  return n;
}

/* write buffer at block number# */
int disk_write(int number, int off, char* buf) //off holds the number of bytes into the block
{
  int c, t, s, n, seeks;

  if (n_to_cts(number, &c, &t, &s) < 0) return -1;

  if (FD == NULL) {
    fprintf(stderr, "Disk not initialized\n");
    return -1;
  }

  seeks = c - current_cylinder;
  if (seeks < 0) seeks = -seeks;
  n_seeks += seeks;
  current_cylinder = c;

  if (current_track != t) n_transfers++;
  current_track = t;

  fseek(FD, (long) (number * SECTOR_SIZE + off), 0);

 // printf("\nWriting in block %d at offset %d at file system's %d th byte.", number, off, (number * SECTOR_SIZE + off));

  n = fwrite(buf, 1, strlen(buf), FD);
  if (n != strlen(buf)) {
    fprintf(stderr, "Error, couldn't write %d bytes starting at %d\n",
            SECTOR_SIZE, number*SECTOR_SIZE);
    return -1;
  }
  fflush(FD);
 
  return n;
}

void disk_stats(int *ns,int *nt)
{
  *ns = n_seeks;
  *nt = n_transfers;
}

/*intialize the file system*/
int inititalize_filesystem() {

  int i, istart, dstart, j;
  char *super_data, *buf;
  buf = (char *)malloc(256 * sizeof(char));  
  struct inode dnode;
  
  super_data = (char *)malloc(256 * sizeof(char));
  super_block = (struct super_blk *)malloc(sizeof(struct super_blk));
  boot_block = (struct boot_blk *)malloc(sizeof(struct boot_blk));
  boot_block->boot_msg = (char *)calloc(256, sizeof(char));


  printf("\nInitializing DISK....");
  if (initialize_disk(1) < 0) {
     printf("\nCould not initialize the DISK!!!");
	 return -1;

  }
  else
   printf("\nDISK Initialied!!!");
  
  printf("\nCreating Boot Sector...");
  strcpy (boot_block->boot_msg,"This is the Boot Sector.");
  for(i = 24; i < 248; i++) 
	boot_block->boot_msg[i] = '.';
  strcat (boot_block->boot_msg,"BOOT END");
  printf("\nSize of boot sector = %d B", strlen(boot_block->boot_msg));
  disk_write(0,0,boot_block->boot_msg); //block 0 is boot
  printf("\nDone!!!");
  
  printf("\nInitializing the super sector....\n");

  itoa((int)(NSECTORS * 256), super_block->fs_size);
  itoa(MAX_INODES, super_block->nfree_inodes);
  itoa(MAX_DATA, super_block->nfree_dblocks);
  
  istart = INODE_START;
  dstart = DATA_START;

  for (i = 0; i < 24; i++) {
     itoa(istart, super_block->inodes_free_cache[i]);
     istart++;
  }

  for (i = 0; i < 100; i++) {
      itoa(dstart, super_block->dblocks_free_cache[i]); 
	  dstart++;
  }

 /* printf("\nsize %d\n", atoi(super_block->fs_size));
  printf("\nfree inode %d\n", atoi(super_block->nfree_inodes));
  printf("\nfree data sectors %d\n", atoi(super_block->nfree_dblocks));
 

  printf("\nfree inode cache");
  for (i = 0; i < 24; i++) {
   printf("%d ",atoi(super_block->inodes_free_cache[i]));
  }

  printf("\nfree data cache");
  for (i = 0; i < 100; i++) {
   printf("%d ",atoi(super_block->dblocks_free_cache[i]));
  }*/

  /* write to super block*/
  if( write_super_block() != -1 ) 
   printf("\nSuper sector....Done!!!\n");


  /*initialize the Inode blocks*/
  printf("\nInitializing the inode blocks....Please wait!!!\n");
  if( init_inode_blocks() != -1 ) 
   printf("\nInode blocks....Done!!!\n");
  

  printf("\nInitializing the Inode table...");
 for (i = 0; i < 64; i++) {
      inode_table[i].inum = -1;
  }
 printf("\nInode table...Done!!!"); 

 printf("\nInitializing the Open file table...");
 for (i = 0; i < 64; i++) {
      open_fdesc[i].offset = -1;
	  open_fdesc[i].inode_index = -1;
  }
 printf("\nOpen file table...Done!!!"); 

 printf("\nInitializing the File descriptor table...");
 for (i = 0; i < MAX_INODES; i++) {
      fdesc_table[i].of_index = -1;
  }
 fdesc_table[0].of_index = STD_IN;
 fdesc_table[1].of_index = STD_OUT;
 fdesc_table[2].of_index = STD_ERR;
 //user file fd range starts from 3.
 printf("\nFile descriptor table...Done!!!"); 

 /*initialize the Inode blocks*/
  printf("\nInitializing the root directory....\n");
  if( initialize_rootdir() != -1 ) 
   printf("\nRoot directory....Done!!!\n");
 
  return 0;
}


void itoa(int n, char str[]) {
 
  int i, j, sign;
  char *temp;

  if ((sign = n) < 0) 
     n = -n;
  i = 0;
  do {  // generate string digits in reverse order
   str[i++] = n % 10 + '0';

  }while((n /= 10) > 0);
  if (sign < 0)
    str[i++] = '-';
  str[i] = '\0';

  //reverse the string

  temp = (char *)malloc (strlen(str) * sizeof(char));

  for (j = strlen(str) - 1, i = 0;j >= 0; j--, i++) 
	  temp[i] = str[j];
  temp[i] = '\0';
  strcpy(str,temp);
  
  free(temp);
  
}

int write_super_block() {

  FILE *sfd;
  char tmp_buf[20], buf[512] = {'\0'}; //2 blocks for super
  int i,j,k,n;

  if((sfd = fopen("DISK", "r+")) == NULL) {
	  printf("\nCould not open DISK to write Super sector\n");
	  return -1;
  }

   
  strcat(buf,super_block->fs_size);
  strcat(buf,"$");
  strcat(buf,super_block->nfree_inodes);
  strcat(buf,"$");
  strcat(buf,super_block->nfree_dblocks);
  strcat(buf,"$");
  
  for (i = 0; i < 24; i++) {
    strcat(buf,super_block->inodes_free_cache[i]);
	strcat(buf,"$");
  }

  for (i = 0; i < 100; i++) {
    strcat(buf,super_block->dblocks_free_cache[i]);
	strcat(buf,"$");
  }

//  printf("\nsize of buf is %d sector size is %d", strlen(buf), SECTOR_SIZE);
//  printf("\nbuf contains %s", buf);

  for(i = strlen(buf); i < 503; i++) 
	buf[i] = '.';
  strcat (buf,"SUPER END");
 // printf("\nSize of Super sector = %d B", strlen(buf));

  fseek(sfd, (long) 1 * SECTOR_SIZE, 0);

  n = fwrite(buf, 1, strlen(buf), sfd);
  if (n != strlen(buf)) {
    fprintf(stderr, "Error in writing to super block");
    return -1;
  }
  else
	printf("\nSuccessfully wrote the Super Sector!!\n");

  fclose(sfd);

  return 0;
}

int read_super_block() {

  FILE *sfd;
  char tmp_buf[20], buf[512] = {'\0'}; 
  int i,j,k,n;

  if((sfd = fopen("DISK", "r")) == NULL) {
	  printf("\nCould not open DISK to read Super sector\n");
	  return -1;
  }

  fseek(sfd, (long) 1 * SECTOR_SIZE, 0);
  n = fread(buf, 1, 512, sfd);
  if (n != 512) {
    fprintf(stderr, "Error in reading super sector\n");
    return -1;
  }
  
  /////////reading from buffer and loading the structure - read_super_block
  i = 0;
  j = 0;
  while (i < strlen(buf)) {
	while (buf[i] != '$') {
	  super_block->fs_size[j++] = buf[i++];
	}
	j=0;
    while (buf[i] != '$') {
	  super_block->nfree_inodes[j++] = buf[i++];
	} 
	j=0;
    while (buf[i] != '$') {
	  super_block->nfree_dblocks[j++] = buf[i++];
	}
    j=0;
	for( k = 0; k < 24; k++ ) {
      while (buf[i] != '$') {
	    super_block->inodes_free_cache[k][j++] = buf[i++];
	  }
	}
	j=0;
	for( k = 0; k < 100; k++ ) {
      while (buf[i] != '$') {
	    super_block->dblocks_free_cache[k][j++] = buf[i++];
	  }
	}
    break; 
  }

  fclose(sfd);

  return 0;
 
}


int write_inode(int i_number, struct inode in) {

  int b_num, b_offset;
  FILE *ifd; 
  char ibuf[64] = {'\0'}; //fixed inode size
  int i,j,k,n;
  char ke_data[5];
  
  b_num = (i_number / 4) + 3; //jumping the 1st three blocks
  b_offset = i_number % 4;

  if((ifd = fopen("DISK", "r+")) == NULL) {
	  printf("\nCould not open DISK to write inode %d\n", i_number);
	  return -1;
  }


  strcat(ibuf,in.owner);
  strcat(ibuf,"$");
  ibuf[strlen(ibuf)] = in.type;
  strcat(ibuf,"$");
  strcat(ibuf,in.perm);
  strcat(ibuf,"$");
  strcat(ibuf,in.mtime);
  strcat(ibuf,"$");

  //encrypting key 
  strcpy (ke_data, (const char *)encrypt_file(user_key, in.key));

  strcat(ibuf,ke_data);
  strcat(ibuf,"$");
  strcat(ibuf,in.fsize);
  strcat(ibuf,"$");

  for (i = 0; i < 4; i++) {
    strcat(ibuf,in.blk_addr[i]);
	strcat(ibuf,"$");
  }

 // printf("\nsize of ibuf is %d sector size is %d", strlen(ibuf), SECTOR_SIZE);
 // printf("\nibuf contains %s", ibuf);

  for(i = strlen(ibuf); i < 64; i++) 
	ibuf[i] = '.';
  
  //printf("\nSize of inode %d is %d",i_number, strlen(ibuf));

  fseek(ifd, (long)(b_num * SECTOR_SIZE + b_offset * INODE_SIZE), 0);
  //printf("\nwrite inode seek ptr = %d", (b_num * SECTOR_SIZE + b_offset * INODE_SIZE));
  n = fwrite(ibuf, 1, strlen(ibuf), ifd);
  if (n != strlen(ibuf)) {
    fprintf(stderr, "Error in writing inode %d\n", i_number);
    return -1;
  }
 // else
//	printf("\nSuccessfully wrote inode %d!!\n", i_number);

  fclose(ifd);
  return 0;

}

int read_inode(int i_num, struct inode *rnode) {

  FILE *ifd;
  char ibuf[64] = {'\0'}; 
  int i,j,k,n;
  int b_num, b_offset;
  char kd_data[5];

  b_num = (i_num / 4) + 3; //jumping the 1st three blocks
  b_offset = i_num % 4;

  if((ifd = fopen("DISK", "r")) == NULL) {
	  printf("\nCould not read inode %d\n", i_num);
	  return -1;
  }

  fseek(ifd, (long)(b_num * SECTOR_SIZE + b_offset * INODE_SIZE), 0);
 // printf("\nread inode seek ptr = %d", (b_num * SECTOR_SIZE + b_offset * INODE_SIZE));

  n = fread(ibuf, 1, INODE_SIZE, ifd);
  //printf("\ninode_read data: %s\n", ibuf);

  if (n != INODE_SIZE) {
    fprintf(stderr, "Error in reading inode %d\n", i_num);
    return -1;
  }
  
  /////////reading from buffer and loading the structure - read_inode
  i = 0;
  
  while (i < strlen(ibuf)) {
 	j = 0;
	while (ibuf[i] != '$') {
	  rnode->owner[j++] = ibuf[i++];
	}
	rnode->owner[j] = '\0';
	i++;
   
	while (ibuf[i] != '$') {
	  rnode->type = ibuf[i++];
	} 
    j=0;
	i++;

	while (ibuf[i] != '$') {
	  rnode->perm[j++] = ibuf[i++];
	}
	rnode->perm[j] = '\0';

    j=0;
	i++;
	while (ibuf[i] != '$') {
	  rnode->mtime[j++] = ibuf[i++];
	}
	rnode->mtime[j] = '\0';

    j=0;
	i++;
	while (ibuf[i] != '$') {
	  rnode->key[j++] = ibuf[i++];
	}
	rnode->key[j] = '\0';

	strcpy (kd_data, (const char *)decrypt_file(user_key, rnode->key));
    strcpy(rnode->key, kd_data);

    j=0;
	i++;
	while (ibuf[i] != '$') {
	  rnode->fsize[j++] = ibuf[i++];
	}
	rnode->fsize[j] = '\0';

	i++;
	for( k = 0; k < 4; k++ ) {
	  j = 0;
      while (ibuf[i] != '$') {
	    rnode->blk_addr[k][j++] = ibuf[i++];
	  }
	  rnode->blk_addr[k][j] = '\0';

	  i++;
	}
	break; 
  }

  fclose(ifd);
  
//  printf("\nDecrytped key is %s", rnode->key);
  
  return 0;

}

int init_inode_blocks() {

  struct inode init_node;
  int i;

  //initializing the inode structure to store dots(denoting empty),
  //only type feild is set to "u", to indicate that inode is not used

  strcpy(init_node.owner,".....");
  init_node.type = 'u'; //unused
  strcpy(init_node.perm,"......");
  strcpy(init_node.mtime,".............");
  strcpy(init_node.key,"....");
  strcpy(init_node.fsize,"....");
  for (i = 0; i < 4; i++) {
    strcpy(init_node.blk_addr[i], "...");
  }

  for (i = 0; i < MAX_INODES; i++) {
	 
	  if( write_inode(i, init_node) < 0 ) {
	    printf("\nCould not initialize inode %d\n", i);
		return -1;
	  }
	  //printf(".");
  }

  return 0;
}


int initialize_rootdir() {

/*inode 0 is the root inode, 1st data sector (block 72) is the 1st inode data block.
 to start with block 72 will have one entry whose filename is "." and inode number is 0.
 each dir file entry is of fixed length 16B. Each dir data block will contain max of 256/16
 = 16 entries. dir entry structure is (xxxxxxxxxxxx:000\n) 11B name, a delimiter (:), a 3B inode
 number and another delimiter(\n)
 */

	//to do --> create root inode (inode 0), write inode to disk, write root dir's data block.
	//calls to disk_read, disk_write should be preceded by a call to init_disk with clr = 0;

   // initialize cur dir, and fill the inode information
    int i;
    char dir_data[16], tdata[16], edata[16];

	cur_dir.inum = 0; // inode 0 is root.
	parent_inum = cur_dir.inum;

	cur_dir.in_cur = (struct inode*)malloc(sizeof(struct inode));

	/*fill the data sector for the root dir, only one entry now . is the root/cur dir and 0 is the inode*/
    strcpy(dir_data, ".          :0  #");
    
	dir_size = strlen(dir_data);
    
	strcpy(cur_dir.in_cur->owner, "root");
    cur_dir.in_cur->type = 'd';
    strcpy(cur_dir.in_cur->perm,"111111");
    strcpy(cur_dir.in_cur->mtime,get_cur_time());
    strcpy(cur_dir.in_cur->key,root_key);
    itoa(dir_size, cur_dir.in_cur->fsize);
    
	strcpy(cur_dir.in_cur->blk_addr[0], "72"); //first data block
	strcpy(cur_dir.in_cur->blk_addr[1], "0");
	strcpy(cur_dir.in_cur->blk_addr[2], "0");
	strcpy(cur_dir.in_cur->blk_addr[3], "0");

    /*write the root dir inode to disk*/
    if (write_inode(cur_dir.inum, *cur_dir.in_cur) != -1)
	  printf("\nSucessfully wrote the root inode!!!");

     strcpy(tdata, dir_data);
     strcpy (edata, (const char *)encrypt_file(root_key, tdata));
    
	/*write dir data to root dir data sector*/
    if (disk_write(72, 0, edata) != -1)
		printf("\nSucessfully wrote the root dir data block!!!");
    
	

	used_inmax = 0; //current max used inode in the inode cache of super block
	used_dsmax = 72; //current max used data sector in the data cache of super block
    inodes_max--;
	datanodes_max--;
    itoa(inodes_max, super_block->nfree_inodes); //update total number of free inodes
    itoa(datanodes_max, super_block->nfree_dblocks); //update total number of free data sectors

    return 0;
	
}

char *get_cur_time() {

  char time_str[15];
  struct tm tim;
  time_t now;
  now = time(NULL);
  tim = *(localtime(&now));
  strftime(time_str, 16, "%m-%d-%y %H:%M", &tim);
 // printf("Curent time is %s\n length %d", time_str, strlen(time_str));
  return time_str;
}


void qsort_superblock_cache(int type, int size) {
//keeps the inodes_free_cache and dblocks_free_cache sorted
 //call it after a block has been freed and has to be added to the cache

  int array_int[size], i;
  
  //int super_array[nrow][ncol];
  
  //convert string to int
  if( type == 1) {
    for( i = 0; i < size; i++) {
      array_int[i] = atoi(super_block->inodes_free_cache[i]);
      //printf("int %d", array_int[i]);
	}
  }
  else if( type == 2) {
    for( i = 0; i < size; i++) {
      array_int[i] = atoi(super_block->dblocks_free_cache[i]);
     // printf("int %d", array_int[i]);
	}
  }
   
  //sort ints
  qsort(array_int, size, 4, qsort_compfunc);

  //reconvert int to str
  if( type == 1) {
    for ( i = 0; i < size; i++) {
      itoa(array_int[i], super_block->inodes_free_cache[i]);

	}
  } else if( type == 1) {
    for ( i = 0; i < size; i++) {
      itoa(array_int[i], super_block->dblocks_free_cache[i]);

	}
  }
  
}


void reload_supercache(int type, int size, int start_block) {
/*once all the entries in the cache become used (value = -1 ) reload the cache(super_array)
with array_size blocks, with block number starting from start_block*/

   
  int i;

  if(type == 1) {
    for(i = 0; i < size; i++) {
      
	  if (start_block > INODE_END) {
		 printf("\n End of Inode blocks reached!!!");
		 itoa(-1, super_block->inodes_free_cache[i]);
		 //break;
	  }
	  else
        itoa(start_block, super_block->inodes_free_cache[i]); 
	  start_block++;
	}
  } else if (type == 2) {
    for(i = 0; i < size; i++) {
      if (start_block > DATA_END) {
		 printf("\n End of data blocks reached!!!");
		 itoa(-1, super_block->dblocks_free_cache[i]); 
		 //break;
	  }
	  else
		itoa(start_block, super_block->dblocks_free_cache[i]); 
	  start_block++;
	}
  }

}

int qsort_compfunc(const void *x, const void *y)
{
   int pp,qq;
   int t;

   pp = (int)(*(int *)x);
   qq = (int)(*(int *)y);

   if (pp < qq) t = -1;
   else
   if (pp == qq) t = 0;
   else  t = 1;
   return t;
}

int read_input() {

 char *tmpstr, *tok1;
 int ch;

 fflush(stdout);

 if (gets(tmpstr) == NULL) {
   printf("\nError in gets()");
   return -1;
 }

 cmdstr = (char *)malloc(strlen(tmpstr) * sizeof(char));
// full_cmdstr = (char *)malloc(strlen(tmpstr) * sizeof(char));

 strcpy(cmdstr,tmpstr);
 strcpy(full_cmdstr,tmpstr);
// strcat(full_cmdstr, "\n");
 
// printf("\nfull cmdstr is %s length is %d\n", full_cmdstr, strlen(full_cmdstr));

 if(strlen(cmdstr) == 0) return 0; //input is just '\n'
 
 tok1 = strtok(cmdstr, " ");
 if(strcasecmp(tok1, "clear") == 0)
 {ch = CLEAR; return ch;}
 if(strcasecmp(tok1, "open") == 0)
  {ch = OPEN; return ch;}
 if(strcasecmp(tok1, "read") == 0)
  {ch = READ; return ch;}
 if(strcasecmp(tok1, "write") == 0)
  {ch = WRITE; return ch;}
 if(strcasecmp(tok1, "lseek") == 0)
  {ch = LSEEK; return ch;}
 if(strcasecmp(tok1, "close") == 0)
  {ch = CLOSE; return ch;}
 if(strcasecmp(tok1, "cat") == 0)
  {ch = CAT; return ch;}
 if(strcasecmp(tok1, "trunc") == 0)
  {ch = TRUNC; return ch;}
 if(strcasecmp(tok1, "mkdir") == 0)
  {ch = MKDIR; return ch;}
 if(strcasecmp(tok1, "cd") == 0)
  {ch = CD; return ch;}
 if(strcasecmp(tok1, "pwd") == 0)
  {ch = PWD; return ch;}
 if(strcasecmp(tok1, "ls") == 0)
 {ch = LS; return ch;}
 if(strcasecmp(tok1, "chmod") == 0)
  {ch = CHMOD; return ch;}
 if(strcasecmp(tok1, "fsys") == 0)
  {ch = FSYS; return ch;}
 if(strcasecmp(tok1, "filedesc") == 0)
  {ch = FILEDESC; return ch;}
 if(strcasecmp(tok1, "logout") == 0)
  {ch = LOGOUT; return ch;}
 if(strcasecmp(tok1, "help") == 0)
  {ch = HELP; return ch;}
 else
  return -1;
 
}




int open_file() {

  char *arg2, *arg3, *arg4, *str1, inum_str[4], d_data[16];
  int arg_cnt = 1, exists, i, new_inum, new_dnum, tblk_num, j;
  char *tbuf = (char *)malloc(256 * sizeof(char));
  char ot_data[16], oe_data[16], t_tbuf[256], d_tbuf[256];

  /*first token (tok1) extracted in read_input()*/
  /* extract from second token in string sequence */
   /* loop until finishied */
  while ( (str1=strtok(NULL, " ")) != NULL)
  {
    if(arg_cnt == 1)
      arg2 = str1;
	if(arg_cnt == 2)
      arg3 = str1;
	if(arg_cnt == 3)
      arg4 = str1;
	arg_cnt++;
  }

  if(arg_cnt != 4) return -2; //argument not matching
  
  //copying and restricting argument lengths
  strcpy(filename, arg2);
  if(strlen(arg2) > 11)
   filename[11] = '\0';
  strcpy(mode, arg3);
  mode[2] = '\0';
  strcpy(key, arg4);
  key[4] = '\0';

 // printf("\nfilename: %s mode: %s key: %s", arg2, arg3, arg4);
 // printf("\nfilename: %s mode: %s key: %s", filename, mode, key);

  if((strcmp(filename, ".") == 0) || (strcmp(filename, "..") == 0)) {
	printf("\nUser cannot open . and .. special files");
	return -1;
  }

  if((strcmp(mode, "r") != 0) && (strcmp(mode, "w") != 0) && (strcmp(mode, "a") != 0) && (strcmp(mode, "rw") != 0)) {
	printf("\nMode argument incorrect!!!");
	return -2;
  }

  if(strlen(key) != 4) {
    printf("\nKey length should be 4!!!");
	return -2;
  }


  /*file open operations begin here*/

  /*first check current dir to see if file exists*/
  exists = search_cur_dir();
//  printf("\nExists = %d", exists);
 
  if ( exists < 0) { /*file doesn't exist*/
  //look for free inode from super inode cache, load this to inode table
  //change super blocks values to indicate this
 //update cur dir inode
  
  //looks for free inodes in inode cache of super block
  for (i = 0; i < 24; i++) {
    if(atoi(super_block->inodes_free_cache[i]) >= INODE_START) { //found free node
       new_inum = atoi(super_block->inodes_free_cache[i]); 
	   used_inmax = i;
	 //  printf("\nfree inode is %d used_max is %d\n", new_inum, used_inmax);
	   itoa(-1, super_block->inodes_free_cache[i]); //mark entry used
       break;
	}
   
  }
  
  //looks for free data blocks in dblocks cache of super block
  for (i = 0; i < 100; i++) {
    if(atoi(super_block->dblocks_free_cache[i]) >= DATA_START) { //found free node
       new_dnum = atoi(super_block->dblocks_free_cache[i]); 
	   used_dsmax = i;
	 //  printf("\nfree data sector is %d used_dsmax is %d\n", new_dnum, used_dsmax);
	   itoa(-1, super_block->dblocks_free_cache[i]); //mark entry used
       break;
	}
   
  }


  //load inode table
 // printf("\nOriginal itable index is %d", itable_index);

  for(i = 0; i < 64; i++) { //linear search
	 if( inode_table[i].inum == -1) {
        itable_index = i;
		break;
	 }
   }


  inode_table[itable_index].inum = new_inum; //increment index and load
  inode_table[itable_index].in_cur = (struct inode*)malloc(sizeof(struct inode));
  
 // printf("\nitable index before reading = %d", itable_index);
  if( read_inode(new_inum, inode_table[itable_index].in_cur) == -1) {
    printf("\nError in loading new inode number %d to inode table", new_inum);
	return -1;
  }
  
  //fill up the inode

  strcpy(inode_table[itable_index].in_cur->owner, uname);
  inode_table[itable_index].in_cur->type = 'r'; 
  strcpy(inode_table[itable_index].in_cur->perm,"111100");
  strcpy(inode_table[itable_index].in_cur->mtime, get_cur_time());
  strcpy(inode_table[itable_index].in_cur->key, key);
  strcpy(inode_table[itable_index].in_cur->fsize,"0");
  
  itoa(new_dnum, inode_table[itable_index].in_cur->blk_addr[0]);
  strcpy(inode_table[itable_index].in_cur->blk_addr[1], "0");
  strcpy(inode_table[itable_index].in_cur->blk_addr[2], "0");
  strcpy(inode_table[itable_index].in_cur->blk_addr[3], "0");

 // printf("\ninode table data");
 // printf("\nitable index is %d", itable_index);
 // printf("\ninode number is %d", inode_table[itable_index].inum);
 // print_inode(inode_table[itable_index].in_cur);


  //update the super block

  inodes_max--;
  datanodes_max--;
  itoa(inodes_max, super_block->nfree_inodes); //update total number of free inodes
  itoa(datanodes_max, super_block->nfree_dblocks); //update total number of free data sectors

  //update current dir inode, mtime, blk_addr and fszie change, when new entry is made
  
   for (i = 0; i < 4; i++) {
   tblk_num = atoi(cur_dir.in_cur->blk_addr[i]);  
 //  printf("\n current dir data block number is %d", tblk_num);

     if( tblk_num != 0 ) {
	   if(disk_read(tblk_num, 0, tbuf) == -1)
         printf("\nError reading current dir data block %d", tblk_num);
         
         strcpy(t_tbuf, tbuf);
         strcpy (d_tbuf, (const char *)decrypt_file(root_key, t_tbuf));
         strcpy(tbuf, d_tbuf);

	//   printf("\ntbuf data is %s", tbuf);
    //   printf("\nfilename: %s mode: %s key: %s", filename, mode, key);
       
	   //create the dir file entry
       strcpy(d_data, filename);  
	   for(j = strlen(filename); j < 11; j++)
        strcat(d_data, " ");
	   strcat(d_data, ":");
         
       itoa(new_inum, inum_str);
	   strcat(d_data, inum_str);
       for(j = strlen(inum_str); j < 3; j++)
        strcat(d_data, " ");
	   strcat(d_data, "#");
       strcat(d_data, "\0");
	//   printf("\nDir data length is %d", strlen(d_data));
       
	//   printf("\nDir data in open is %s", d_data);
       if((strlen(tbuf) + strlen(d_data)) > SECTOR_SIZE ) {
		 tblk_num++; //go to next block
         itoa(tblk_num, cur_dir.in_cur->blk_addr[i+1]); // write next blk_addr in cur dir inode  
	   }
	//   printf("\nWriting to block %d offset %d", tblk_num, strlen(tbuf));

	   strcpy(ot_data, d_data);
       strcpy (oe_data, (const char *)encrypt_file(root_key, ot_data));
       
       if (disk_write(tblk_num, strlen(tbuf), oe_data) < 0 ) {
		printf("\nCould not write cur dir data block %d", tblk_num);
        return -1;
	   }

	 }

   }
   dir_size = strlen(tbuf) + strlen(d_data);
   strcpy(cur_dir.in_cur->mtime,get_cur_time());
   itoa(dir_size, cur_dir.in_cur->fsize);
 
 //  printf("\ncur dir data");
//   print_inode(cur_dir.in_cur);

   //write the cur_dir inode to disk
  // printf("\nWriting cur dir inode to disk!!!");
   if( write_inode(cur_dir.inum, *cur_dir.in_cur) < 0 ) {
	    printf("\nCouldn't write cur dir inode %d to disk\n", cur_dir.inum);
		return -1;
	  }

   
  }
  else if ( exists > 0) { /*file exists*/
  
	//check if inode number already in the inode table
	for (i = 0; i < 64; i++) {
      if ( inode_table[i].inum == exists) {
         printf("\nFile %s is already open", filename);
		 return -1;
	  }
	}
  // inode number of file = exists, get this inode and load the inode table
    itable_index++;

	if(itable_index > 63) { 	  //if reach end, then circle back and look for free entry
     for(i = 0; i < 64; i++) {
	   if( inode_table[i].inum == -1) {
        itable_index = i;
		break;
	   }
	 }
	}
	inode_table[itable_index].inum = exists;
    inode_table[itable_index].in_cur = (struct inode*)malloc(sizeof(struct inode));

//	printf("\nitable_index before read %d", itable_index);

    if( read_inode(exists, inode_table[itable_index].in_cur) == -1) {
		printf("\nError in loading inode number %d to inode table", exists);
		return -1;
	}

 //   printf("\nRead Inode contains:\n");
 //  	print_inode(inode_table[itable_index].in_cur);

	if(inode_table[itable_index].in_cur->type == 'd') { /*directory file return error*/
        printf("\n %s is a directory, not a file", filename);
        inode_table[itable_index--].inum = -1; //mark that inode table entry as free and decrement index
		return -1;
   	}
   
	if(strcmp(inode_table[itable_index].in_cur->key, key) != 0) { /*wrong key while openining*/
        printf("\n %s was not the key used to encrypt the file", key);
        inode_table[itable_index--].inum = -1; //mark that inode table entry as free and decrement index
		return -1;
   	}
    
//	printf("\ninode table data");
 //   printf("\nitable index is %d", itable_index);
 //   printf("\ninode number is %d", inode_table[itable_index].inum);
  //  print_inode(inode_table[itable_index].in_cur);

  }
  
 //need to update open file table
// printf("\nOriginal oftable index is %d", oftable_index);

   for (i = 0; i < 64; i++) { //linear search
	 if (open_fdesc[i].inode_index == -1) {
        open_fdesc[i].inode_index = itable_index; // store index of inode table in open file table
	//	printf("\nofd inode_index is %d", open_fdesc[i].inode_index);

	   if(strcmp(mode, "a") == 0) 
		 open_fdesc[i].offset = atoi(inode_table[itable_index].in_cur->fsize);
		else
         open_fdesc[i].offset = 0;

	//	printf("\nofd inode offset is %d", open_fdesc[i].offset);
		oftable_index = i;
        break;
	 } 
  }
  
// printf("\nNew oftable index is %d", oftable_index);

 //need to update file descriptor table
 // printf("\nOriginal fd table index is %d", fdtable_index);
  
  for (i = 0; i < MAX_INODES; i++) { //linear search
      if ( fdesc_table[i].of_index == -1) {
         fdesc_table[i].of_index = oftable_index; // store index of openfile table in file desc table
//		 printf("\nfd table of index is %d", fdesc_table[i].of_index);
         fdtable_index = i;
		 return fdtable_index;
	  }
  }



}


int search_cur_dir() {

 int i, blk_num, j, k, m;
 char name[13] = {'\0'}, inode_n[5] = {'\0'}, dbuf[257];
 char d_dbuf[257], t_dbuf[257];

 //dbuf = (char *)malloc(SECTOR_SIZE * sizeof(char));
 
 for (i = 0; i < 4; i++) {
   blk_num = atoi(cur_dir.in_cur->blk_addr[i]);   
   if( blk_num != 0 ) {
	   if(disk_read(blk_num, 0, dbuf) == -1)
         printf("\nError reading data block %d", blk_num);
	
     strcpy(t_dbuf, dbuf);
     strcpy (d_dbuf, (const char *)decrypt_file(root_key, t_dbuf));
     strcpy(dbuf, d_dbuf);
      
		 j = 0;
//	 printf("\ndecrypt dbuf data in search is %s\nlength is %d", dbuf, strlen(dbuf));
     while(j < strlen(dbuf)) {
		 k = 0;
		 while(dbuf[j] != ':') {
     		 name[k++] = dbuf[j++];
             if( dbuf[j] == ' ') break;
		 }
		 name[k] = '\0';
    //     printf("\nfile name in search is %s length is %d", name, strlen(name));
         
		 while(dbuf[j++] != ':');
		 m = 0;
		 while(dbuf[j] != '#') 
		   inode_n[m++] = dbuf[j++];
		 
		 inode_n[m] = '\0';
      //   printf("\ninode_n in search is %s length is %d", inode_n, strlen(inode_n));

		 
		// printf("\nUser filename is %s and search filename is %s", filename, name);
		
		 if( strcmp(filename, name) == 0) {
			 // free(dbuf);
	
			  return atoi(inode_n);
		 } 
         
		 j++;
	
	 }

   }

 }
 //printf("\ndbuf data in search is %s", dbuf);
 //free(dbuf);
 //printf("\nleaving search");
 return -1;
  
}



void print_inode(struct inode *dnode) {
  
  int i;

  printf("\nstructure contains:\n");
  printf("%s\n", dnode->owner);
  printf("%c\n", dnode->type);
  printf("%s\n", dnode->perm);
  printf("%s\n", dnode->mtime);
  printf("%s\n", dnode->key);
  printf("%s\n", dnode->fsize);
  for (i = 0; i < 4; i++) {
    printf("%s\n",dnode->blk_addr[i]);
  }

}

int fsys_func() {

  char *str;
  int arg_cnt = 1, i = 0;
  
  if(strlen(full_cmdstr) > 4) return -2;
 
  if ( read_super_block() == -1 ) {
	  printf("\nError reading super block!!! ");
	  return -1;
  }
  printf("\nFile system structure variables");
  printf("\n*******************************************");
  printf("\nSECTOR_SIZE: %d",SECTOR_SIZE); 
  printf("\nTRACKS_PER_CYLINDER: %d",TRACKS_PER_CYLINDER); 
  printf("\nN_CYLINDERS: %d",N_CYLINDERS); 
  printf("\nMAX_INODES: %d",MAX_INODES); 
  printf("\nMAX_DATA: %d",MAX_DATA); 
  printf("\nINODE_START: %d",INODE_START); 
  printf("\nINODE_END: %d",INODE_END); 
  printf("\nDATA_START: %d",DATA_START); 
  printf("\nDATA_END: %d",DATA_END); 
  printf("\nINODE_SIZE: %d",INODE_SIZE); 
  printf("\nLOGIN_BLK: %d",LOGIN_BLK); 
  printf("\nSECTOR_SIZE: %d",SECTOR_SIZE); 
  printf("\n*******************************************");
  printf("\nFile system size: %d\n", atoi(super_block->fs_size));
  printf("\nNumber of free inodes: %d\n", atoi(super_block->nfree_inodes));
  printf("\nNumber of free data sectors: %d\n", atoi(super_block->nfree_dblocks));
 
  printf("\nFree inode cache contains:\n");
  for (i = 0; i < 24; i++) {
   printf("%d ",atoi(super_block->inodes_free_cache[i]));
  }

  printf("\n\nFree data cache contains:\n");
  for (i = 0; i < 100; i++) {
   printf("%d ",atoi(super_block->dblocks_free_cache[i]));
  }
  printf("\n*******************************************");
  printf("\nCurrent values of: itable_index is %d oftable_index is %d fdtable_index is %d", itable_index, oftable_index, fdtable_index);
  printf("\n\nCurrent snapshot of File descriptor table is:");
  for (i = 0; i < MAX_INODES; i++) {
      printf("%d ", fdesc_table[i].of_index );
  }

  printf("\n\nCurrent snapshot of Open File table is:");
  for (i = 0; i < 64; i++) {
      printf("%d ", open_fdesc[i].inode_index);
	  
  }

  printf("\n\nCurrent snapshot of Inode table is:");

  for (i = 0; i < 64; i++) {
      printf("%d ", inode_table[i].inum);
  }
  printf("\n");
  return 0;
	
}


int write_file () {

  char arg1[6], arg2[4], arg3[5], arg4[1024];
  int i, j, k, found = 0, bufsize, m, flag =0;
  int w_fd, w_ofd, w_idx, nbytes, w_inum, w_off, w_dsnum, write_nbytes;
  char r;
  char blockdata[4][256], t_blockdata[256], en_blockdata[256];

  int org_off, org_dsnum, org_size, org_size_rep;
 
  k =  strlen(full_cmdstr);
  while ((r=getchar()) != '#') {
    full_cmdstr[k++] = r;
    //printf("\nfull cmd_str after cat is: %s", full_cmdstr);
  }
  full_cmdstr[k] = '#';
 // printf("\nfull cmd_str outside is: %s length is %d", full_cmdstr, strlen(full_cmdstr));  
  
  i = 0;
  while ( i < strlen(full_cmdstr)) {
	 j = 0;
	 while (full_cmdstr[i] != ' ')
		arg1[j++] = full_cmdstr[i++];
     arg1[j] = '\0';
	// printf("\ninner arg1 is %s", arg1);
	 i++;
	 j = 0;
	 while (full_cmdstr[i] != ' ')
		arg2[j++] = full_cmdstr[i++];
     arg2[j] = '\0';
	// printf("\ninner arg1 is %s", arg1);
	 i++;
	 j = 0;
	 while (full_cmdstr[i] != ' ')
		arg3[j++] = full_cmdstr[i++];
     arg3[j] = '\0';
 //	 printf("\ninner arg2 is %s", arg2);
	 i++;
	// i++; //jump the beginning #
	 j = 0;
     while (full_cmdstr[i] != '#')
		arg4[j++] = full_cmdstr[i++];
     arg4[j] = '\0';
	 //	 printf("\ninner arg3 is %s", arg3);
     break;
  }

  w_fd = atoi(arg2);

  if(w_fd < 3 || w_fd > 275) {
	  printf("\nInvalid file descriptor!!!");
	  return -1;
  }

  if(strlen(arg3) > 4)
  {
     printf("\nArgument 3 too long!!!");
	 return -1;
  }
  nbytes = atoi(arg3);

 // printf("\narg1 is %s", arg1);
 // printf("\narg2 is %s and file fd is %d", arg2, w_fd);
 // printf("\narg3 is %s and nbytes is %d", arg3, nbytes);
  //printf("\narg4 length is %d", strlen(arg4));

  if (nbytes < 0 || nbytes > 1024) {
	printf("\n Number of bytes to write should be in the range 0 - 1024!!!");
	return -2;
  }
  if (nbytes < strlen(arg4)) {
 	printf("\n Input Text Larger than no: of Bytes to be written!!!");
  	return -2;
  }
  
 
  //check dir permissions 
  if (strcmp(cur_dir.in_cur->owner, uname) == 0) {
	  if(cur_dir.in_cur->perm[1] != '1') {
		  printf("\nNo write permission for this directory for owner %s", uname);
		  return -1;
	  }
  }
  else 
    if(cur_dir.in_cur->perm[4] != '1') {
		  printf("\nNo write permission for this directory for user %s", uname);
		  return -1;
	
	}
  
  //scan fd table 
	if( fdesc_table[w_fd].of_index == -1 ) {
	  printf("\nFile Descriptor %d not allocated", w_fd);
	  return -1;
	}
  
  //traverse through fdtable, ofdtable, and inodetable to get the inode pointed to by w_fd
  w_ofd = fdesc_table[w_fd].of_index; 
  w_idx = open_fdesc[w_ofd].inode_index;
  w_inum = inode_table[w_idx].inum;

 // printf("\nw_ofd = %d w_idx = %d w_inum = %d", w_ofd, w_idx, w_inum);

  //check file permissions 
  if (strcmp(inode_table[w_idx].in_cur->owner, uname) == 0) {
	  if(inode_table[w_idx].in_cur->perm[1] != '1') {
		  printf("\nNo write permission for this file for owner %s!!", uname);
		  return -1;
	  }
  }
  else 
    if(inode_table[w_idx].in_cur->perm[4] != '1') {
		  printf("\nNo write permission for this file for user %s!!", uname);
		  return -1;
	}
  

  //check current open mode   
  if(strcmp(mode, "r") == 0) {
      printf("\nCannot write to file opened in read mode!!");
	  return -1;

  }
   
   // get offset from open file table
    w_off = open_fdesc[w_ofd].offset;

  //get disk block number
	for (i = 0; i < 4; i++) { //get the end of the file if spanning multiple blocks;
	  w_dsnum = atoi(inode_table[w_idx].in_cur->blk_addr[i]);
	  if( w_dsnum == 0 ) break;
	}
    w_dsnum = atoi(inode_table[w_idx].in_cur->blk_addr[i-1]);

//	printf("\nw_off = %d w_dsnum = %d", w_off, w_dsnum);

    //write file to disk involves breaking of data in 32 chars and writing each, getting new data block if required
	//and then updating the inode to reflect the changes
	bufsize = strlen(arg4);

//	printf("\nBufsize = %d", bufsize);

  //break data into 256B (32 char) size
	k = 0;
	if (bufsize > 256) {
      i = 0;
	  flag = 0;
      while (i < bufsize) {
       j = 0;
	   while(j < 256) {
        if(i == bufsize) 
		{ flag =1;	break;}
        blockdata[0][j++] = arg4[i++];
	   }
	   blockdata[0][j] = '\0'; 
	 //   printf("\ninner blockdata[0] = %s", blockdata[0]);
	   if(flag) break;
	   k++;
	   j = 0;
	   while(j < 256) {
		if(i == bufsize) 
			{ flag =1;	break;}
        blockdata[1][j++] = arg4[i++];
	
	   }
	   blockdata[1][j] = '\0';
	//    printf("\ninner blockdata[1] = %s", blockdata[1]);
	    if(flag) break;
	   k++;
	   j = 0;
	   while(j < 256) {
        if(i == bufsize) 
			{ flag =1;	break;}
	    blockdata[2][j++] = arg4[i++];
		
	   }
	   blockdata[2][j] = '\0';
//	    printf("\ninner blockdata[2] = %s", blockdata[2]);
	    if(flag) break;
	   k++;
	   j = 0;
	   while(j < 256) {
        if(i == bufsize) 
			{ flag =1;	break;}
		blockdata[3][j++] = arg4[i++];
		
	   }
	   blockdata[3][j] = '\0';
	//    printf("\ninner blockdata[3] = %s", blockdata[3]);
	  if(flag) break;
	   k++;
	  break;
	  }

	  
	}
	else
	{

		strcpy(blockdata[0],arg4);
		strcpy(blockdata[1],"\0");
		strcpy(blockdata[2],"\0");
		strcpy(blockdata[3],"\0");
		k = 1;
	}
   
//	for (i = 0; i < 4; i++)
//	  printf("\nblockdata[%d] = %s", i, blockdata[i]);

    org_off = w_off;
	org_dsnum = w_dsnum;
	org_size = atoi(inode_table[w_idx].in_cur->fsize);
	org_size_rep = org_size;

   // printf("\norg_size = %d and K value = %d", org_size, k);

     
    for (i = 0; i < k; i++) {
  
		  
     strcpy(t_blockdata, blockdata[i]);
     strcpy (en_blockdata, (const char *)encrypt_file(key, t_blockdata));

     write_nbytes = disk_write(org_dsnum, org_off, en_blockdata);

  //   printf("\nwrite_nbytes soon after write = %d", write_nbytes);


	 if ( write_nbytes == -1) {
       printf("\nError in writing blockdata[%d] of fd %d to block %d at offset %d", i, w_fd, org_dsnum, org_off);
	   return -1;
	 }
    // else
      // printf("\nSuccessfully wrote blockdata[%d] of fd %d to block %d at offset %d", i, w_fd, org_dsnum, org_off);
     
	// printf("\nInput number of bytes is %d. Wrote %d bytes in iteration %d", nbytes, write_nbytes, i);
     
	 org_size_rep += write_nbytes;
   //  printf("\norg_size_rep = %d", org_size_rep);

     itoa(org_size_rep, inode_table[w_idx].in_cur->fsize);

	 if((org_size + strlen(blockdata[i])) > 256) { //data going above one block
     
    //looks for free data blocks in dblocks cache of super block
        for (i = 0; i < 100; i++) {
          if(atoi(super_block->dblocks_free_cache[i]) >= DATA_START) { //found free node
           org_dsnum = atoi(super_block->dblocks_free_cache[i]); 
	       used_dsmax = i;
	   //    printf("\nFor write free data sector is %d used_dsmax is %d\n", org_dsnum, used_dsmax);
	       itoa(-1, super_block->dblocks_free_cache[i]); //mark entry used
           break;
		  }
   
		}

		org_off = (org_size + strlen(blockdata[i])) - 256; 
	
       //load new datablock num to the inode  
	   for (m = 0; m < 4; m++) {
		   if(atoi(inode_table[w_idx].in_cur->blk_addr[m]) == 0) {
			itoa(org_dsnum, inode_table[w_idx].in_cur->blk_addr[m]);
			break;
		   }
	   }
   
	 }
	 else {
   
		 org_off = org_size_rep; 

	 }

	}

    //update the new offset value in open file table
    open_fdesc[w_ofd].offset = org_off;
   
	//update file modification time
    strcpy(inode_table[w_idx].in_cur->mtime, get_cur_time());
	
	//printf("\nInput number of bytes is %d. Wrote %d bytes", nbytes, write_nbytes);

  //  printf("\nInode Data after Write....");
//	print_inode(inode_table[w_idx].in_cur);
   
    return (org_size_rep - org_size);
  
}

void clear_filesystem() {
	char c1;
	if(strlen(full_cmdstr) > 5) {
       printf("Usage: Clear <return>\n" );
       printf("Use HELP command for more details...\n" );
	   return;
	  }
	  if(strcmp(uname, "root") != 0) {
	     printf("\nOnly ROOT can execute this command!!!\n");
	     return;
	  }
	  printf("\nEntire File system will be Erased!!!\nDo you want to continue:");
	  c1=getchar();
	  if( (c1 == 'y') || (c1 == 'Y')) {
		printf("\nInitializing the file system.... Please wait...\n");
        if ( inititalize_filesystem() != -1)
          printf("\nFile System Initialized Successfully!!!\n");
          choice = LOGOUT;
		  strcpy(path, "\0");
	  }

}

void print_working_dir() {

	if(strlen(full_cmdstr) > 3) {
	   printf("Usage: Pwd <return>\n" );
	   printf("Use HELP command for more details...\n" );
	   return;
	}
	printf("\nYour present working directory is: %s\n", path);
}

void logout_user () {
  
  int i;
  if(strlen(full_cmdstr) > 6) {
	printf("Usage: Logout <return>\n" );
	printf("Use HELP command for more details...\n" );
	choice = 0;          
 	return;
  }
  /* close all fds, write superblock, cur_dir inode, inodes from inode table to disk, reinitialize fd table
  open file table and inode table, set cur_dir to root*/

  //scan fd table 
  for (i = 3; i < MAX_INODES; i++) {
	if( fdesc_table[i].of_index != -1 ) {
	  printf("\nOpen File Descriptors need to be closed!!");
	  printf("\nUse <filedesc> command to see all the open file desciptors.!!!\n");
	  choice = 0;          
	  return;
	  
	}
  }

 //re-initialializing all tables 
 printf("\nInitializing the Inode table...");
 for (i = 0; i < 64; i++) {
      inode_table[i].inum = -1;
  }
 printf("\nInode table...Done!!!"); 

 printf("\nInitializing the Open file table...");
 for (i = 0; i < 64; i++) {
      open_fdesc[i].offset = -1;
	  open_fdesc[i].inode_index = -1;
  }
 printf("\nOpen file table...Done!!!"); 

 printf("\nInitializing the File descriptor table...");
 for (i = 3; i < MAX_INODES; i++) {
      fdesc_table[i].of_index = -1;
  }
 //user file fd range starts from 3.
 printf("\nFile descriptor table...Done!!!"); 

 
 /* write to super block*/
  if( write_super_block() != -1 ) 
   printf("\nWrote back Super sector....!!!\n");


  /*writing cur dir to disk*/
  if (write_inode(cur_dir.inum, *cur_dir.in_cur) != -1)
	  printf("\nSucessfully wrote the cur dir inode!!!");

  printf ("Logging out %s .. Good bye!!!\n", uname);
  strcpy(path, "\0");

}

void help_user() {
  
  FILE *hlpfp;
  char hlp_buf[100] = {'\0'};

  if(strlen(full_cmdstr) > 6) {
	printf("Usage: Help <return>\n" );
	printf("Use HELP command for more details...\n" );
	return;
  }
  hlpfp = fopen("help.txt", "r");
  while(!feof(hlpfp)) {
	fgets(hlp_buf,100,hlpfp);
	printf("%s\n", hlp_buf);
  }
  fclose(hlpfp);

}

int read_file() {

  char arg2[4], arg3[5];
  char *tmp2, *tmp3, *str1;
  int i, j, arg_cnt = 1, k;
  int r_fd, r_ofd, r_idx, nbytes, r_inum, r_off, r_dsnum, read_nbytes, x, b_cnt =0;
  char blockdata[4][256] = {"\0"};
  char temp[256];
  char nbyte_data[256];
  int org_off, org_dsnum, org_size, org_size_rep;
  int div, mod;
  char t_blockdata[256], de_blockdata[256];

 while ( (str1=strtok(NULL, " ")) != NULL)
  {
    if(arg_cnt == 1)
      tmp2 = str1;
	if(arg_cnt == 2)
      tmp3 = str1;

	arg_cnt++;
  }

  if(arg_cnt != 3) return -2; //argument not matching

  strcpy(arg2, tmp2);
  strcpy(arg3, tmp3);

  r_fd = atoi(arg2);

  if(r_fd < 3 || r_fd > 275) {
	  printf("\nInvalid file descriptor!!!");
	  return -1;
  }


  nbytes = atoi(arg3);

   
  // printf("\narg2 is %s and file fd is %d", arg2, r_fd);
  // printf("\narg3 is %s and nbytes is %d", arg3, nbytes);
  

  if (nbytes < 0 || nbytes > 1024) {
	printf("\n Number of bytes to read should be in the range <1-1024>!!!");
	return -2;
  }
  
 
  //check dir permissions 
  if (strcmp(cur_dir.in_cur->owner, uname) == 0) {
	  if(cur_dir.in_cur->perm[0] != '1') {
		  printf("\nNo read permission for this directory for owner %s", uname);
		  return -1;
	  }
  }
  else 
    if(cur_dir.in_cur->perm[3] != '1') {
		  printf("\nNo read permission for this directory for user %s", uname);
		  return -1;
	
	}
  
  //scan fd table 
	if( fdesc_table[r_fd].of_index == -1 ) {
	  printf("\nFile Descriptor %d not allocated", r_fd);
	  return -1;
	}
  
  //traverse through fdtable, ofdtable, and inodetable to get the inode pointed to by r_fd
  r_ofd = fdesc_table[r_fd].of_index; 
  r_idx = open_fdesc[r_ofd].inode_index;
  r_inum = inode_table[r_idx].inum;

 // printf("\nr_ofd = %d r_idx = %d r_inum = %d", r_ofd, r_idx, r_inum);

  //check file permissions 
  if (strcmp(inode_table[r_idx].in_cur->owner, uname) == 0) {
	  if(inode_table[r_idx].in_cur->perm[0] != '1') {
		  printf("\nNo read permission for this file for owner %s!!", uname);
		  return -1;
	  }
  }
  else 
    if(inode_table[r_idx].in_cur->perm[3] != '1') {
		  printf("\nNo read permission for this file for user %s!!", uname);
		  return -1;
	}
  

  //check current open mode   
  if(strcmp(mode, "w") == 0) {
      printf("\nCannot read to file opened in write mode!!");
	  return -1;

  }
  if(strcmp(mode, "a") == 0) {
      printf("\nCannot read to file opened in append mode!!");
	  return -1;

  }
    
   // get offset from open file table
    r_off = open_fdesc[r_ofd].offset;


  //get disk block number
	for (i = 0; i < 4; i++) { //get the end of the file if spanning multiple blocks;
	  r_dsnum = atoi(inode_table[r_idx].in_cur->blk_addr[i]);
	  if( r_dsnum != 0 ) break;
	}
   
//	printf("\nr_off = %d r_dsnum = %d r_d = %d", r_off, r_dsnum, r_fd);

    
	//reading data from file.. if nbytes is > block size, then read block_size at a time
	org_off = r_off;
	org_dsnum = r_dsnum;
	org_size_rep = 0;

	if (nbytes > 256) {
    
	  div = nbytes / 256;
	  mod = nbytes % 256;
      printf("\ndiv is %d and mod is %d", div, mod);

      for (i = 0; i < div; i++) {
        
		org_dsnum = atoi(inode_table[r_idx].in_cur->blk_addr[i]); //get data block number

        read_nbytes = disk_read(org_dsnum, org_off, blockdata[i]);
        
		strcpy(t_blockdata, blockdata[i]);
        strcpy (de_blockdata, (const char *)decrypt_file(key, t_blockdata));
        strcpy(blockdata[i], de_blockdata);


       // printf("\nread_nbytes soon after read = %d", read_nbytes);

	    if ( read_nbytes == -1) {
          printf("\nError in reading blockdata[%d] of fd %d from block %d at offset %d", i, r_fd, org_dsnum, org_off);
	      return -1;
		}
     //   else
      //    printf("\nSuccessfully read blockdata[%d] of fd %d to block %d at offset %d", i, r_fd, org_dsnum, org_off);
     
        //  printf("\nInput number of bytes is %d. Read %d bytes in iteration %d", nbytes, read_nbytes, i);
     
	      org_size_rep += read_nbytes;

       //   printf("\norg_size_rep = %d", org_size_rep);

	  }
      if(mod != 0 ) { 
	  //read remaining data from next block
		x = i;
	    org_off = 0;
		strcpy(temp, blockdata[x]);
	    read_nbytes = disk_read(org_dsnum+1, org_off, temp);

		strcpy(t_blockdata, temp);
        strcpy (de_blockdata, (const char *)decrypt_file(key, t_blockdata));
        strcpy(blockdata[x], de_blockdata);
		strcpy(temp, de_blockdata);

      	printf("\ntemp data is: %s", temp);
        if ( read_nbytes == -1) {
          printf("\nError in reading blockdata[%d] of fd %d from block %d at offset %d", x, r_fd, org_dsnum+1, org_off);
	      return -1;
		}
      //  else
       //   printf("\nSuccessfully read blockdata[%d] of fd %d from block %d at offset %d", x, r_fd, org_dsnum+1, org_off);
       
        org_size_rep += strlen(temp);

       // printf("\norg_size_rep = %d", org_size_rep);

	//	printf("\nbytes = %d", nbytes);
        k =0;
		while (k < mod) {
              nbyte_data[k] = temp[k];
			  k++;
		//	  printf("\n K= %d", k);
		}
		nbyte_data[k] = '\0';

     //   printf("\nbyte_data contains %s", nbyte_data);
		
		strcpy(blockdata[x], nbyte_data);
     
	  }

	  
	}
	else //nbytes less than 256
	{
		
      //read remaining data from next block
		strcpy(temp, blockdata[0]);
	    read_nbytes = disk_read(org_dsnum, org_off, temp);

	
        strcpy(t_blockdata, temp);

        strcpy (de_blockdata, (const char *)decrypt_file(key, t_blockdata));

//		printf("\ndecryp data is: %s", de_blockdata);
			
		strcpy(blockdata[0], de_blockdata);
		strcpy(temp, de_blockdata);

	//	printf("\nread_nbytes soon after read = %d", read_nbytes);
      //  printf("\nBlock 0 contains %s of length %d", temp, strlen(temp));
     //    
        if ( read_nbytes == -1) {
          printf("\nError in reading blockdata[%d] of fd %d from block %d at offset %d", i, r_fd, org_dsnum, org_off);
	      return -1;
		}
      //  else
      //    printf("\nSuccessfully read blockdata[%d] of fd %d from block %d at offset %d", i, r_fd, org_dsnum, org_off);
       
		   
   	//	printf("\nbytes = %d", nbytes);
	//	printf("\norg_size_rep = %d", org_size_rep);

        i =0;
		while (i < nbytes) {
              nbyte_data[i] = temp[i];
			  i++;
		}
		nbyte_data[i] = '\0';
       
		org_size_rep += strlen(nbyte_data);

     //   printf("\nbyte_data contains %s of length", nbyte_data, org_size_rep);
		
		strcpy(blockdata[0], nbyte_data);
	//	printf("\nNew Block 0 contains %s", blockdata[0]);
		
		strcpy(blockdata[1], "\0");
		strcpy(blockdata[2], "\0");
		strcpy(blockdata[3], "\0");
		
	}
    b_cnt =0;
    printf("\nThe Read Data is: ");
 	for (i = 0; i < 4; i++)
		for(j = 0; j < 256; j++) {
			if (b_cnt > nbytes) {i=5;break;}
	      printf("%c", blockdata[i][j]);
		  b_cnt++;
		}

    //update the new offset value in open file table
    open_fdesc[r_ofd].offset = org_size_rep;
   
//	printf("\nInode Data after read....");
	//print_inode(inode_table[r_idx].in_cur);
  
	return org_size_rep;


}


int seek_file() {

  char arg2[4], arg3[5], arg4[9];
  char *tmp2, *tmp3, *tmp4, *str1;
  int arg_cnt = 1;
  int l_fd, l_ofd, l_idx, l_inum, l_off, offset, whence;
  int org_off, org_end;
  int t_when;
 
 while ( (str1=strtok(NULL, " ")) != NULL)
  {
    if(arg_cnt == 1)
      tmp2 = str1;
	if(arg_cnt == 2)
      tmp3 = str1;
    if(arg_cnt == 3)
      tmp4 = str1;

	arg_cnt++;
  }

  if(arg_cnt != 4) return -2; //argument not matching

  strcpy(arg2, tmp2);
  strcpy(arg3, tmp3);
  strcpy(arg4, tmp4);

  l_fd = atoi(arg2);
  if(l_fd < 3 || l_fd > 275) {
	  printf("\nInvalid file descriptor!!!");
	  return -1;
  }


  offset = atoi(arg3);

   
//   printf("\narg2 is %s and file fd is %d", arg2, l_fd);
//   printf("\narg3 is %s and nbytes is %d", arg3, offset);
//   printf("\narg4 is %s length is %d", arg4, strlen(arg4));
  

  if (offset < 0 || offset > 1024) {
	printf("\n Offset should be in the range <1-1024>!!!");
	return -2;
  }
  
  //set the whence value based on the number or string input

  if(strlen(arg4) > 1) {

   if(strcmp(arg4, "SEEK_SET") == 0)
	  whence = 0;
   else if(strcmp(arg4, "SEEK_CUR") == 0)
	  whence = 1;
   else if(strcmp(arg4, "SEEK_END") == 0)
	  whence = 2;
   else {

		printf("\nInvalid whence value!!!");
		return -2;
   }

  }
  else {
 
     t_when = atoi(arg4);
    // printf("\nt_when is %d", t_when);

    if(t_when > 2){
         printf("\nInvalid whence value!!!");
		return -2;
    }

    if(t_when == 0) 
      whence = 0;
    else if (t_when == 1)
	  whence = 1;
    else if (t_when == 2)
	  whence = 2;
  
  }
  
 //  printf("\nWhence value is: %d", whence);
  

  //check dir permissions - should have read or write permissions
  if (strcmp(cur_dir.in_cur->owner, uname) == 0) {
	  if((cur_dir.in_cur->perm[0] != '1') && (cur_dir.in_cur->perm[1] != '1')) {
		  printf("\nNo permission to seek in this directory for owner %s", uname);
		  return -1;
	  }
  }
  else 
    if((cur_dir.in_cur->perm[3] != '1') && (cur_dir.in_cur->perm[4] != '1')) {
		  printf("\nNo permission to seek in this directory for user %s", uname);
		  return -1;
	
	}
  
  //scan fd table 
	if( fdesc_table[l_fd].of_index == -1 ) {
	  printf("\nFile Descriptor %d not allocated", l_fd);
	  return -1;
	}
  
  //traverse through fdtable, ofdtable, and inodetable to get the inode pointed to by l_fd
  l_ofd = fdesc_table[l_fd].of_index; 
  l_idx = open_fdesc[l_ofd].inode_index;
  l_inum = inode_table[l_idx].inum;

//  printf("\nl_ofd = %d l_idx = %d l_inum = %d", l_ofd, l_idx, l_inum);

  //check file permissions 
  if (strcmp(inode_table[l_idx].in_cur->owner, uname) == 0) {
	  if((inode_table[l_idx].in_cur->perm[0] != '1') && (inode_table[l_idx].in_cur->perm[1] != '1')) {
		  printf("\nNo permission to seek this file for owner %s!!", uname);
		  return -1;
	  }
  }
  else 
    if((inode_table[l_idx].in_cur->perm[3] != '1') && (inode_table[l_idx].in_cur->perm[4] != '1')) {
		  printf("\nNo permission to seek this file for user %s!!", uname);
		  return -1;
	}
  

  
   // get current offset from open file table
    org_off = open_fdesc[l_ofd].offset;
	//l_off = 0;
   
	//get end of file
	org_end = atoi(inode_table[l_idx].in_cur->fsize);
	
   //adjust the offset value according to whence 
	if (whence == 0) {

	   open_fdesc[l_ofd].offset = offset;

	}
	if (whence == 1) {

		if ((org_off + offset) > 1024)
			offset = 1024;
        open_fdesc[l_ofd].offset = offset;
	}
    if (whence == 2) {

		offset = org_end + offset;
		if(offset > 1024)
			offset = 0;
        open_fdesc[l_ofd].offset = offset;
	}
    
	if(open_fdesc[l_ofd].offset < 0)
	  open_fdesc[l_ofd].offset = 0;

	return open_fdesc[l_ofd].offset;

	//one can read past the eof and write something there. there will be a gap in that case.
	//the DISK will not physically show that gap. But it can be proved that there is a gap
	// reading from the gap(return value 0) or writing (data will be inserted).

}

int cat_file () {

 char arg2[12], arg3[5], cat_data[257], de_blockdata[257], t_blockdata[257];
  char *tmp2, *tmp3, *str1;
  int i, arg_cnt = 1, exists;
  int c_inode;
  static int cat_nbytes, cat_dsnum;
  struct inode cat_inode;
 
 while ( (str1=strtok(NULL, " ")) != NULL)
  {
    if(arg_cnt == 1)
      tmp2 = str1;
	if(arg_cnt == 2)
      tmp3 = str1;

	arg_cnt++;
  }

  if(arg_cnt != 3) return -2; //argument not matching

  strcpy(arg2, tmp2);
  strcpy(arg3, tmp3);

   
 //  printf("\narg2 filename is %s ", arg2);
  // printf("\narg3 key is %s ", arg3);
  

  /*first check current dir to see if file exists*/
  strcpy(filename, arg2);
 // printf("\nfilename is %s", filename);

  exists = search_cur_dir();
 // printf("\nExists = %d", exists);
  
  if (exists < 0) {
	  printf("\nFilename %s not found in current directory!!!", arg2);
	  return -1;
  }
  
  c_inode = exists;

  //check dir permissions 
  if (strcmp(cur_dir.in_cur->owner, uname) == 0) {
	  if(cur_dir.in_cur->perm[0] != '1') {
		  printf("\nNo read permission for this directory for owner %s", uname);
		  return -1;
	  }
  }
  else 
    if(cur_dir.in_cur->perm[3] != '1') {
		  printf("\nNo read permission for this directory for user %s", uname);
		  return -1;
	
	}
  
  
  //read the inode from the disk

  if( read_inode(c_inode, &cat_inode) == -1) {
		printf("\nError in reading inode number %d for cat!!!", exists);
		return -1;
  }

 // printf("\nCat inode data is:");
 // print_inode(&cat_inode);

  //check file not written to disk yet
  if (cat_inode.type == 'u') {
	      printf("\nFile not written to disk yet!! Call CLOSE and then try!!!");
		  return -1;
	  
  }
  
  //check file permissions 
  if (strcmp(cat_inode.owner, uname) == 0) {
	  if(cat_inode.perm[0] != '1') {
		  printf("\nNo read permission for this file for owner %s!!", uname);
		  return -1;
	  }
  }
  else 
    if(cat_inode.perm[3] != '1') {
		  printf("\nNo read permission for this file for user %s!!", uname);
		  return -1;
	}


 //check the key

	if(strcmp(cat_inode.key, arg3) !=0) {
		printf("Incorrect key!!!!");
		return -1;
	}

 //scan the datablocks and print it

 for (i = 0; i < 4; i++) {
	 cat_dsnum = atoi(cat_inode.blk_addr[i]);
    if(cat_dsnum == 0) break;

  	   cat_nbytes = disk_read(cat_dsnum, 0, cat_data);    

        strcpy(t_blockdata, cat_data);
        strcpy (de_blockdata, (const char *)decrypt_file(key, t_blockdata));
        strcpy(cat_data, de_blockdata);

//		printf("\ndecryp data is: %s", de_blockdata);
			
		   

    //printf("\ncat_nbytes soon after read = %d\n", cat_nbytes);

	if ( cat_nbytes == -1) {
          printf("\nError in reading cat data from block %d", cat_dsnum);
	      return -1;
	}
    printf("\nFile Data is:\n");
	printf("%s", cat_data);

  } 
 
  return 0;


}


int close_file() {

char arg2[4];
  char *tmp2, *str1;
  int i, j, arg_cnt = 1, k;
  int c_fd, c_ofd, c_idx, c_inum;
  
   
 while ( (str1=strtok(NULL, " ")) != NULL)
  {
    if(arg_cnt == 1)
      tmp2 = str1;

	arg_cnt++;
  }

  if(arg_cnt != 2) return -2; //argument not matching

  strcpy(arg2, tmp2);
  
  c_fd = atoi(arg2);
  
  if(c_fd < 3 || c_fd > 275) {
	  printf("\nInvalid file descriptor!!!");
	  return -1;
  }
	  

 //  printf("\narg2 is %s and file fd is %d", arg2, c_fd);
  
  
  //scan fd table 
	if( fdesc_table[c_fd].of_index == -1 ) {
	  printf("\nFile Descriptor %d not allocated", c_fd);
	  return -1;
	}
  
  //traverse through fdtable, ofdtable, and inodetable to get the inode pointed to by c_fd
  c_ofd = fdesc_table[c_fd].of_index; 
  c_idx = open_fdesc[c_ofd].inode_index;
  c_inum = inode_table[c_idx].inum;

//  printf("\nc_ofd = %d c_idx = %d c_inum = %d", c_ofd, c_idx, c_inum);

  //mark the corresponding entries in all tables to be free (-1)

  fdesc_table[c_fd].of_index = -1;
  open_fdesc[c_ofd].inode_index = -1;
  inode_table[c_idx].inum = -1;
 // printf("\nInode data when closing:\n");
//  print_inode(inode_table[c_idx].in_cur);

  //write inode to disk
  if( write_inode(c_inum, *inode_table[c_idx].in_cur) < 0 ) {
	    printf("\nFor fd %d , Couldn't write inode %d to disk\n", c_fd, c_inum);
		return -1;
	  }

  //free memory used by the inode
  free(inode_table[c_idx].in_cur);

  return 0;

}

void filedesc() {

  int i;

  if(strlen(full_cmdstr) > 8) {
	printf("Usage: Filedesc <return>\n" );
	printf("Use HELP command for more details...\n" );
	return;
  }

 // printf("\nCurrent File Descriptor: %d", cur_fd);
  printf("\nAll open file descriptors: ");
  
  for (i = 3; i < MAX_INODES; i++) {
    if (fdesc_table[i].of_index != -1) 
      printf("%d ",i);
	
  }
  printf("\n");
}


/* 
 * encrypt_file takes plain text and returns encrypted 
 * text in an unsigned char buffer
 */

unsigned char* encrypt_file(char *input_key, char *text)
{
	int ksz, i, j, k =0;
    int *key;
	int buffer_size;
	int c, m;
	unsigned char *in_buf;
	unsigned char *encryp_buf;
	
	ksz = strlen(input_key);
	
	buffer_size = strlen(text);
//	printf("\n to encrypt data is: %s", text);

	in_buf = (unsigned char *)malloc(buffer_size*sizeof(unsigned char));
	encryp_buf = (unsigned char *)malloc(buffer_size*sizeof(unsigned char));
	
	key=(int *)malloc(ksz*sizeof(int));
	
	if( key == NULL ){
		printf("Error: malloc key\n");
		exit(-1);
	}
	/*
   for(i=0; i < ksz; i++)
		{		
		key[i]=input_key[i];
		}
		*/
	
	int skey;
	for(i=0; i < ksz; i++)
	{		
		skey =+input_key[i];
	}
	
	for (i = 0; i < buffer_size; i++)
	{
	 in_buf[i] = text[i];
	}
	
    i = 0;
    j = 0;
	 while (j < buffer_size)
		{		
		int x;

					// 	x = in_buf[j++] + key [i];
			x = in_buf[j++] + skey;
		    c = (x)%126;		
		  
			if(x < 0)
			c = c + 126;

	    /*dont allow the delimited char $ in the encrytped text*/
	     if (c == 36) c = 126;			
		 
			 encryp_buf[k++] = c;
		
		if(i < 3)
			i++; 
		else
			i=0;								
		
		}
	    
	 /*pad the end of the buffer with null*/    
	for(m = 1; m <= (k - buffer_size); m++)
		  encryp_buf[k-m] = '\0';
				
//	printf("Finished encryption\n");
	
	/* return the encrypted file buffer */

	free(key);
	free(in_buf);


//	printf("\n encrypted data is %s length is %d", encryp_buf, strlen((const char*)encryp_buf));
	return encryp_buf;
} /*end encrypt*/


/* 
 * decrypt_file takes cipher text and returns plain 
 * text in an unsigned char buffer
 */

unsigned char* decrypt_file(char *input_key, char *text)
{
	int ksz, i, j, k=0;
	int *key1;
	int buffer_size; 
	int c, m;
	unsigned char *in_buf;	
	unsigned char *decryp_buf;
	
	//	printf("\ndecrypt recieved data is %s", text);

 ksz = strlen(input_key);
	
	buffer_size = strlen(text);
	//printf("\nBuf_size in decrypt: %d", buffer_size);

	in_buf = (unsigned char*)malloc(buffer_size*sizeof(unsigned char));
	decryp_buf = (unsigned char *)malloc(buffer_size*sizeof(unsigned char));
	key1=(int *)malloc(ksz*sizeof(int));
	
	if( key1 == NULL ){
		printf("Error: malloc key\n");
		exit(-1);
	}
	
	/*
		for(i=0; i < ksz; i++)
		{		
			key[i]=input_key[i];
		}
		*/
	
	int skey;
	for(i=0; i < ksz; i++)
	{		
		skey =+input_key[i];
	}	
	
	for (i = 0; i < buffer_size; i++)
		{
		  if(text[i] == 126) 
		    in_buf[i] = '$'; //put back the original value
		  else
	        in_buf[i] = text[i];
		  
		}
	

	i = 0;
	
 for( j = 0; j < buffer_size; j++)
		{
		
		int x;

		 // x = in_buf[j] - key1[i];
		x = in_buf[j] - skey;
		  c = (x) % 126;
					
		  if(c < 0)
			c = c + 126;
	
		 
		decryp_buf[k++] = c;
		
		if(i < 3)
			i++;
		else
			i = 0;
		
		}
	
 /*pad the end of the buffer with null*/    
	for(m = 1; m <= (k - buffer_size); m++)
		  decryp_buf[k-m] = '\0';

//	printf("Finished decryption\n");
	
	free(key1);
	free(in_buf);

    return decryp_buf;

} /*end decrypt*/


int chmod_file () {

 char arg2[12], arg3[10];
  char *tmp2, *tmp3, *str1;
  int i, arg_cnt = 1, exists, perm_len;
  struct inode chm_inode;
 
 while ( (str1=strtok(NULL, " ")) != NULL)
  {
    if(arg_cnt == 1)
      tmp2 = str1;
	if(arg_cnt == 2)
      tmp3 = str1;

	arg_cnt++;
  }

  if(arg_cnt != 3) return -2; //argument not matching

  strcpy(arg2, tmp2);
  strcpy(arg3, tmp3);

   
 //  printf("\narg2 filename is %s ", arg2);
  // printf("\narg3 key is %s ", arg3);
  

  /*first check current dir to see if file exists*/
  strcpy(filename, arg2);

  // printf("\nfilename is %s", filename);

  exists = search_cur_dir();
 // printf("\nExists = %d", exists);
  
 
  if (exists < 0) {
	  printf("\nFilename %s not found in current directory!!!", arg2);
	  return -1;
  }
  
   //check dir permissions 
  if (strcmp(cur_dir.in_cur->owner, uname) == 0) {
	  if(cur_dir.in_cur->perm[0] != '1') {
		  printf("\nNo read permission for this directory for owner %s", uname);
		  return -1;
	  }
  }
  else 
    if(cur_dir.in_cur->perm[3] != '1') {
		  printf("\nNo read permission for this directory for user %s", uname);
		  return -1;
	
	}
  
  
  //read the inode from the disk

  if( read_inode(exists, &chm_inode) == -1) {
		printf("\nError in reading inode number %d for cat!!!", exists);
		return -1;
  }

 //check file not written to disk yet
  if (chm_inode.type == 'u') {
	      printf("\nFile not written to disk yet!! Call CLOSE and then try!!!");
		  return -1;
	  
  }
  
  //check file permissions, user and root are allowed
  // printf("\nfile owner is %s", chm_inode.owner);
  if ( (strcmp(chm_inode.owner, uname) != 0) && ((strcmp(uname, "root") != 0))) {
	  
		  printf("\nOnly file owner or root can change file permissions!!!");
		  return -1;
	  
  }
  

  perm_len = strlen(arg3);

 //set the new permissions and write inode to disk
  if( strlen(arg3) > 6 ) {
     printf("Permission argument too long!!!");
     return -2;
  }


  i = 0;
  while (i < perm_len) {
	if((arg3[i] != '1') && (arg3[i] != '0')) {
      printf("Invalid Permission values!!");
	  return -2;
	}
   
    i++;
  }

//  printf("\npermission string is %s", arg3);

  strcpy(chm_inode.perm, arg3);

  //write inode to disk
   if( write_inode(exists, chm_inode) < 0 ) {
	    printf("\nCould not write inode %d in chmod\n", exists);
		return -1;
   }
  
 // print_inode(&chm_inode);
 
  return 0;


}


int list_dir() {

  int blk_num, i, j, k, m, n;
  char name[13] = {'\0'}, inode_n[5] = {'\0'}, dbuf[257];
  char d_dbuf[257], t_dbuf[257];
  struct inode ls_inode;
  int cur_drsize;

  if(strlen(full_cmdstr) > 2) {
    printf("Usage: Ls <return>\n" );
    printf("Use HELP command for more details...\n" );
    return -2;
  }
  
 // print_inode(cur_dir.in_cur);

  //check dir permissions 
  if (strcmp(cur_dir.in_cur->owner, uname) == 0) {
	  if(cur_dir.in_cur->perm[0] != '1') {
		  printf("\nNo read permission for this directory for owner %s", uname);
		  return -1;
	  }
  }
  else 
    if(cur_dir.in_cur->perm[3] != '1') {
		  printf("\nNo read permission for this directory for user %s", uname);
		  return -1;
	
	}

 //print the header..
  printf("\n");
  printf("File    Owner         Type   Size    Permission    Modfication_Time");
  printf("\n");
  for(i = 0; i< 80; i++)
    printf("*");
  

  for (i = 0; i < 4; i++) {
   blk_num = atoi(cur_dir.in_cur->blk_addr[i]);   
   if( blk_num != 0 ) {
	 if(disk_read(blk_num, 0, dbuf) == -1)
       printf("\nError reading data block %d", blk_num);
	
     strcpy(t_dbuf, dbuf);
     strcpy (d_dbuf, (const char *)decrypt_file(root_key, t_dbuf));
     strcpy(dbuf, d_dbuf);
     
	 cur_drsize = atoi(cur_dir.in_cur->fsize); 
	 j = 0;
 //  	 printf("\ndecrypt dbuf data in search is %s\nlength is %d", dbuf, strlen(dbuf));
     while(j < cur_drsize) {
		 k = 0;
		 while(dbuf[j] != ':') {
     		 name[k++] = dbuf[j++];
             if( dbuf[j] == ' ') break;
		 }
		 name[k] = '\0';
         printf("\n%s", name);
         for(n = strlen(name); n < 8; n++)
		  printf(" ");
		 while(dbuf[j++] != ':');
		 m = 0;
		 while(dbuf[j] != '#') 
		   inode_n[m++] = dbuf[j++];
		 
		 inode_n[m] = '\0';
      //  printf("\ninode_n in search is %s length is %d", inode_n, strlen(inode_n));
	
		
		 //read the inode and print the information
            
		 if( read_inode(atoi(inode_n), &ls_inode) == -1) {
           printf("\nError in reading inode during ls");
	       return -1;
		 }
         
		 //check file not written to disk yet
        // if (ls_inode.type == 'u') 
	      // printf("\nSome files not written to disk yet!! Call CLOSE to get updated information!!!");
		 

		 printf("%s", ls_inode.owner);
		 for(n = strlen(ls_inode.owner); n < 15; n++)
		  printf(" ");

         printf("%c      ", ls_inode.type);
         printf("%s", ls_inode.fsize);
		 for(n = strlen(ls_inode.fsize); n < 8; n++)
		  printf(" ");

		 printf("%s", ls_inode.perm);
		 for(n = strlen(ls_inode.perm); n < 14; n++)
    	  printf(" ");

		 printf("%s", ls_inode.mtime);
 		 
		 j++;
	
	 }

   }

 }


  
return 0;

}


int make_dir() {

  char *arg2, *str1, inum_str[4], d_data[16], parent_str[4];
  int arg_cnt = 1, exists, i, new_inum, new_dnum, tblk_num, j;
  char *tbuf = (char *)malloc(256 * sizeof(char));
  struct inode mk_inode;
  char tdata[34], edata[34];
  char ot_data[16], oe_data[16], t_tbuf[256], d_tbuf[256];
  char new_dsize;

  /*first token (tok1) extracted in read_input()*/
  /* extract from second token in string sequence */
   /* loop until finishied */

  while ( (str1=strtok(NULL, " ")) != NULL)
  {
    if(arg_cnt == 1)
      arg2 = str1;
	
	arg_cnt++;
  }

  if(arg_cnt != 2) return -2; //argument not matching
  
  //copying and restricting argument lengths
  strcpy(filename, arg2);
  if(strlen(arg2) > 11)
   filename[11] = '\0';
  
 
  if((strcmp(filename, ".") == 0) || (strcmp(filename, "..") == 0)) {
	printf("\nUser cannot create . and .. special files");
	return -1;
  }

  
  /*mkdir operations begin here*/

  /*first check current dir to see if file exists*/
  exists = search_cur_dir();

 // printf("\nExists = %d", exists);
 

  if(exists > 0) {
	 printf("File/Dir of this name exists in current directory!!!");
	 return -1;
  }

   /*file doesn't exist*/
  //look for free inode from super inode cache, load this to inode table
  //change super blocks values to indicate this
 //update cur dir inode
  
  //looks for free inodes in inode cache of super block
  for (i = 0; i < 24; i++) {
    if(atoi(super_block->inodes_free_cache[i]) >= INODE_START) { //found free node
       new_inum = atoi(super_block->inodes_free_cache[i]); 
	   used_inmax = i;
	 //  printf("\nfree inode is %d used_max is %d\n", new_inum, used_inmax);
	   itoa(-1, super_block->inodes_free_cache[i]); //mark entry used
       break;
	}
   
  }
  
  //looks for free data blocks in dblocks cache of super block
  for (i = 0; i < 100; i++) {
    if(atoi(super_block->dblocks_free_cache[i]) >= DATA_START) { //found free node
       new_dnum = atoi(super_block->dblocks_free_cache[i]); 
	   used_dsmax = i;
	 //  printf("\nfree data sector is %d used_dsmax is %d\n", new_dnum, used_dsmax);
	   itoa(-1, super_block->dblocks_free_cache[i]); //mark entry used
       break;
	}
   
  }

  
 
  if( read_inode(new_inum, &mk_inode) == -1) {
    printf("\nError in reading new inode in mkdir", new_inum);
	return -1;
  }
  
  //fill up the inode

  strcpy(mk_inode.owner, uname);
  mk_inode.type = 'd'; 
  strcpy(mk_inode.perm,"111101");
  strcpy(mk_inode.mtime, get_cur_time());
  strcpy(mk_inode.key, root_key );
  strcpy(mk_inode.fsize,"0");
  
  itoa(new_dnum, mk_inode.blk_addr[0]);
  strcpy(mk_inode.blk_addr[1], "0");
  strcpy(mk_inode.blk_addr[2], "0");
  strcpy(mk_inode.blk_addr[3], "0");

 
  //update the super block

  inodes_max--;
  datanodes_max--;
  itoa(inodes_max, super_block->nfree_inodes); //update total number of free inodes
  itoa(datanodes_max, super_block->nfree_dblocks); //update total number of free data sectors

  //update current dir inode, mtime, blk_addr and fszie change, when new entry is made
  
   for (i = 0; i < 4; i++) {
   tblk_num = atoi(cur_dir.in_cur->blk_addr[i]);  
 //  printf("\n current dir data block number is %d", tblk_num);

     if( tblk_num != 0 ) {
	   if(disk_read(tblk_num, 0, tbuf) == -1)
         printf("\nError reading current dir data block %d", tblk_num);
         
         strcpy(t_tbuf, tbuf);
         strcpy (d_tbuf, (const char *)decrypt_file(root_key, t_tbuf));
         strcpy(tbuf, d_tbuf);

     
	   //create the dir file entry
       strcpy(d_data, filename);  
	   for(j = strlen(filename); j < 11; j++)
        strcat(d_data, " ");
	   strcat(d_data, ":");
         
       itoa(new_inum, inum_str);
	   strcat(d_data, inum_str);
       for(j = strlen(inum_str); j < 3; j++)
        strcat(d_data, " ");
	   strcat(d_data, "#");
       strcat(d_data, "\0");

	   if((strlen(tbuf) + strlen(d_data)) > SECTOR_SIZE ) {
		 tblk_num++; //go to next block
         itoa(tblk_num, cur_dir.in_cur->blk_addr[i+1]); // write next blk_addr in cur dir inode  
	   }
	//   printf("\nWriting to block %d offset %d", tblk_num, strlen(tbuf));

	   strcpy(ot_data, d_data);
       strcpy (oe_data, (const char *)encrypt_file(root_key, ot_data));
       
       if (disk_write(tblk_num, strlen(tbuf), oe_data) < 0) {
		printf("\nCouldnt write cur dir data block %d", tblk_num);
	    return -1;
	   }

	 }

   }
   dir_size = strlen(tbuf) + strlen(d_data);
   strcpy(cur_dir.in_cur->mtime,get_cur_time());
   itoa(dir_size, cur_dir.in_cur->fsize);
 
   //write the cur_dir inode to disk
   //printf("\nWriting cur dir inode to disk!!!");
   if( write_inode(cur_dir.inum, *cur_dir.in_cur) < 0 ) {
	    printf("\nCouldn't write cur dir inode %d to disk\n", cur_dir.inum);
		return -1;
	  }

  
 // print_inode(&mk_inode);

   
  //write the new dir data sectors

  //create . entry
    strcpy(tdata, ".");  
	for(j = 1; j < 11; j++)
      strcat(tdata, " ");
	strcat(tdata, ":");
        
    itoa(new_inum, inum_str);
	strcat(tdata, inum_str);
    
	for(j = strlen(inum_str); j < 3; j++)
    strcat(tdata, " ");

	strcat(tdata, "#");

	//create .. entry
    strcat(tdata, "..");  
	for(j = 2; j < 11; j++)
      strcat(tdata, " ");
	strcat(tdata, ":");
        
    itoa(cur_dir.inum, parent_str);
	strcat(tdata, parent_str);
    
	for(j = strlen(parent_str); j < 3; j++)
    strcat(tdata, " ");

	strcat(tdata, "#");
    strcat(tdata, "\0");
//	printf("\n New dir entry contains %s", tdata);


	new_dsize = strlen(tdata);
    itoa(new_dsize, mk_inode.fsize);

   //write the new dir inode to disk
   if( write_inode(new_inum, mk_inode) < 0 ) {
	    printf("\nCouldn't new dir inode %d to disk\n", new_inum);
		return -1;
	  }

    strcpy (edata, (const char *)encrypt_file(root_key, tdata));
  //  printf("\n New dnum %d", new_dnum);   
	
    if (disk_write(new_dnum, 0, edata) < 0) {
		printf("\nCouldnt write new dir to disk!!!");
		return -1;
	}
    
 //  print_inode(&mk_inode);

   return 0;


} 


int change_dir() {

  char *arg2, *str1, tempname[13];
  int arg_cnt = 1, exists;
  struct inode mk_inode;
  char *dirpath;
  int pathlen;
  int i, blk_num, j, m, k, n, p;
  char inode_n[5] = {'\0'}, dbuf[257];
  char d_dbuf[257], t_dbuf[257];
  int sz;
  char tin[5], instr[4], tname[13];

  /*first token (tok1) extracted in read_input()*/
  /* extract from second token in string sequence */
   /* loop until finishied */

  while ( (str1=strtok(NULL, " ")) != NULL)
  {
    if(arg_cnt == 1)
      arg2 = str1;
	
	arg_cnt++;
  }

  if(arg_cnt != 2) return -2; //argument not matching
  
  dirpath = (char *)malloc(strlen(arg2) * sizeof(char));
   
  strcpy(dirpath, arg2);
  pathlen = strlen(dirpath);

  printf("\ndirpath: %s length is %d", dirpath, pathlen);

  //print_inode(cur_dir.in_cur);
  
  if( (dirpath[0] == '/') && (pathlen == 1) ) { //go to root;
   
	  cur_dir.inum = 0;
	  parent_inum = 0;

	  if( read_inode(cur_dir.inum, cur_dir.in_cur) < 0) {
		printf("\nError in reading current directory search");
		return -1;
	  }


	  strcpy(filename, "/");
	  strcpy(path, uname);
	  strcat(path, " /");

  }
  else 
	if( (dirpath[0] != '/') && (pathlen > 1) ) {
	  if(strlen(arg2) > 11) {
	    printf("\nFilename too long!!");
		return -2;
	  } 
	  strcpy(filename, arg2);
      printf("\nfilename: %s", filename);

	  if(strcmp(filename, ".") == 0) 
	    return 0;  // current directory itself, do nothing return success
	  
      //search the cur dir
	  exists = search_cur_dir();
     // printf("\nexists: %d", exists);

	  if (exists < 0) {
       printf("\n%s not found in specified location!!!", filename);
	   return -1;
	  }
      
	 
	  cur_dir.inum = exists;
      itoa(cur_dir.inum, instr);

	  if( read_inode(cur_dir.inum, cur_dir.in_cur) < 0) {
		printf("\nError in reading current directory search");
		return -1;
	  }

	  if(strcmp(filename, "..") == 0) { //copy the parent dir's filename
      
	    sz = atoi(cur_dir.in_cur->fsize);

	    for (i = 0; i < 4; i++) {
          blk_num = atoi(cur_dir.in_cur->blk_addr[i]);   
          if( blk_num != 0 ) {
	        if(disk_read(blk_num, 0, dbuf) == -1)
                printf("\nError reading data block %d", blk_num);
	
          strcpy(t_dbuf, dbuf);
          strcpy (d_dbuf, (const char *)decrypt_file(root_key, t_dbuf));
          strcpy(dbuf, d_dbuf);
          
	      j = 0;
	// printf("\ndecrypt dbuf data in search is %s\nlength is %d sz size is %d", dbuf, strlen(dbuf), sz);
          while(j < sz) {
	         while(dbuf[j++] != ':'); 
    //    printf("\nfile name in search is %s length is %d", name, strlen(name));
		     m = 0;
		   //  j++;
             while(dbuf[j] != '#') {
				if(dbuf[j] == ' ') break;
	            tin[m++] = dbuf[j++];
			 }
	         tin[m] = '\0';
             printf("\ntin is %s and instr is %s", tin, instr);

		     if(strcmp(tin, instr) == 0) break;

		     j++;
        
	
		  }
 
		  }

		}
        printf("\ntin is %s and instr is %s", tin, instr);
		
		
	    k = j - m - 12;
        
	    m = 0;
        while(dbuf[k] != ':') {
		  if(dbuf[k] == ' ') break;
	       tname[m++] = dbuf[k++];
		}
	    tname[m] = '\0';

		printf("\ntname is %s", tname);

		if(strcmp(tname, ".") == 0) {
      
		 n = strlen(path);
		 printf("pathn is %s", path);

         while (path[n-1] != '/') n--;

		 path[n-1] = '\0';

         printf("pathn is %s", path);

		 }

		 
	  }
	  else {
		 strcat(path, "/");
		 strcat(path, filename); //copy new path
	  }
	  
	   parent_inum = cur_dir.inum;
  }
  else { //parse the full pathname

    i = 1;
   
    cur_dir.inum = 0; //start from the root dir
    
    while ( i < pathlen) {
	 j = 0;
	  while (dirpath[i] != '/') 
        tempname[j++] = dirpath[i++];
	  tempname[j] = '\0';
      
	  printf("tempname is %s", tempname);

	   //store temp name into file to search
	  strcpy(filename, tempname);
	//read inode
      if( read_inode(cur_dir.inum, cur_dir.in_cur) < 0) {
		printf("\nError in reading current directory search");
		return -1;
	  }
    
	  //search the cur dir
	  exists = search_cur_dir();

	  if (exists < 0) {
       printf("\n%s not found in specified location!!!", filename);
	   return -1;
	  }

	  cur_dir.inum = exists;
      parent_inum = cur_dir.inum;
	  i++;
           
	}

	cur_dir.inum = exists;
	if( read_inode(cur_dir.inum, cur_dir.in_cur) < 0) {
		printf("\nError in reading current directory search");
		return -1;
	  }
    
	printf("parent_inum = %d", parent_inum);

    strcpy(path, uname);
	strcat(path, " ");
	strcat(path, dirpath); //copy new path
	strcat(path, "\0");
    
  }

  
  //store the correct value in the parent_inum field

  sz = atoi(cur_dir.in_cur->fsize);

  for (i = 0; i < 4; i++) {
   blk_num = atoi(cur_dir.in_cur->blk_addr[i]);   
   if( blk_num != 0 ) {
	   if(disk_read(blk_num, 0, dbuf) == -1)
         printf("\nError reading data block %d", blk_num);
	
     strcpy(t_dbuf, dbuf);
     strcpy (d_dbuf, (const char *)decrypt_file(root_key, t_dbuf));
     strcpy(dbuf, d_dbuf);
     printf("data from block %d is %s", blk_num, dbuf); 
	 j = 0;
	 printf("\ndecrypt dbuf data in search is %s\nlength is %d", dbuf, strlen(dbuf));
     while(j < sz) {
	   while(dbuf[j++] != '#'); 
    //    printf("\nfile name in search is %s length is %d", name, strlen(name));
       while(dbuf[j++] != ':');
	   m = 0;
	  
	   while(dbuf[j] != '#') 
	    inode_n[m++] = dbuf[j++];
	   inode_n[m] = '\0';
       break;
	
	 }

   }

 }



return 0;

}



