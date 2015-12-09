/* Utility functions
 * Author: Hemanth Srinivasan
 * Year: 2002
*/
#include <pcap.h>
#include <stdio.h>
#include <ctype.h>

char *longaddy(unsigned long ip)
{
unsigned long addr=ip;
unsigned char *p;

static char i[15];

 p=(unsigned char *)&ip;
 sprintf(i,"%u.%u.%u.%u",p[0],p[1],p[2],p[3]);

 return i;
}


char *pr_ether(unsigned char *ptr)
{
    static char buff[64];

    snprintf(buff, sizeof(buff), "%02X:%02X:%02X:%02X:%02X:%02X",
     (ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
     (ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377)
);
    return (buff);
}


#ifdef HAVE_BPF_TIMEVAL
char* get_time(struct bpf_timeval tm)
#else
char* get_time(struct timeval tm)
#endif
{
char day[4];
char mon[4];
int number;

char time1[10];
char year[5];
time_t czas;
struct tm* t;

    czas=time(NULL);
    t=localtime(&czas);
    sscanf(ctime(&tm.tv_sec), "%s %s %d %s %s", day, mon,&number, time1, year);
    snprintf(main_string,199, "%d/%d/%d %s:%ld",t->tm_mday, ++t->tm_mon,(t->tm_year+1900), time1, tm.tv_usec);
    return(main_string);
}



int  compare(char *fname)
{

  int x,i,flag,cnt=0,j;
  FILE *fp;
  char *buffer,c;
  fp=fopen(fname,"r");

  while(getc(fp)!=EOF){cnt++;}//find filesize
  rewind(fp);//reset position to begining of file

  buffer=(char *)malloc(cnt*sizeof(char));
  x=0;
   while((c=getc(fp))!=EOF)
   {
     buffer[x]=tolower(c);
     x++;

   }

   for(i=0;i<checks;i++)
   {

     if(strstr(buffer,candidate[i])==NULL)
      flag=NOTHREAT;
     else
      {
       flag=THREAT;
       break;
      }
   
       }
  fclose(fp);

  return(flag);
}

char *int2str(int i)
{

  char *str,*str1,k;
  int ii,m;
  int j=0;
  str=(char *)malloc(15);
  str1=(char *)malloc(15);
  while(i!=0)
  {
   
   k=i%10;

   i=i/10;

   k+='0';//0x30 or 48d
   
   str[j]=k;
   j++;

  }
  str[j]='\0';
  //m=j;
// for(ii=0,m=j-1;m>0;ii++,m--)
  //  str1[ii]=str[m];
//str1[ii]='\0';
 return str;

}
   

