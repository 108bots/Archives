/* Utility functions headers
 * Author: Hemanth Srinivasan
 * Year: 2002
*/
#include <stdio.h>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netinet/ip_icmp.h>
#include <netinet/igmp.h>
#include <netinet/ether.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netdb.h>
#include <stdarg.h>
#include <getopt.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <regex.h>
#include <crypt.h>

#include <regex.h>
#include <crypt.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>
#include <sys/timeb.h>


#define THREAT 1
#define NOTHREAT 0

char main_string[200];
 FILE *fp,*fp1,*fpcomp1,*fpcomp2;

//  char command[300];
char logfile[20],logfile1[20];
//char *filename,*filename1;
static int count=0;
static int count1=0;
//int status;
char sample[4],sample1[4],sample2[4],sample3[4];
int checks=6;
static int num=0;
static int num1=0;
static int num2=0;
static int num3=0;
pcap_t *handle,*handle1;
char candidate[6][20]=
      {
        "anthrax",
        "kill",
        "attack",
        "bomb",
        "missile",
        "nuclear"
       };
int FLAG=0;
