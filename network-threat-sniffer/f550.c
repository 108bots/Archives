/* Threat Sniffer for network file transfers using raw Pcap Library
 * Author: Hemanth Srinivasan
 * Year: 2002
*/
#include <pcap.h>
 #include <stdio.h>
 #include <ctype.h>
#include <unistd.h>//getpass()

 #include "finalnext.h"
 #include "finalfunctions.c"
// int count1=0;


void my_callback(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char*
        packet)
{


    int i,j,k,b=0,a,status,t;
   char login[5],command[300],cmd[200];
  FILE *fpp;
 struct ether_header *ethernet; /* The ethernet header */
 struct iphdr *ip; /* The IP header */
 struct tcphdr *tcp; /* The TCP header */
const char *payload; /* Packet payload */

/* For readability, we'll make variables for the sizes of each of the structures
 */

int size_ethernet = sizeof(struct ether_header);
int size_ip = sizeof(struct iphdr);
int size_tcp = sizeof(struct tcphdr);
int payload_len;
char *str1;
 count++;

ethernet = (struct ether_header*)(packet);
ip = (struct iphdr*)(packet + size_ethernet);
tcp = (struct tcphdr*)(packet + size_ethernet + size_ip);
payload = (u_char *)(packet + size_ethernet + size_ip + size_tcp);
payload_len=(pkthdr->len)-(size_ethernet+size_ip+size_tcp);


 if(count==1)
 {
   printf("\nSource IP address:%s",longaddy(ip->saddr));
   printf("\nDestination IP address:%s",longaddy(ip->daddr));
   printf("\nSource ETHERNET(NIC) address:%s",pr_ether(ethernet->ether_shost));

  printf("\nDestination ETHERNET(NIC) address:%s",pr_ether(ethernet->ether_dhost));
   printf("\nTime of Transfer:%s",get_time(pkthdr->ts));
 }




 if((pkthdr->len)<=(size_ethernet+size_ip+size_tcp))

     return; /* rejecting packets which contain no data */

  //printf("\nopening");
  fp=fopen(logfile,"a");
  //printf("\nopened");
  for(i=0,j=0;i<payload_len-1;i++,j++)
 {
   if( isspace(payload[i]) || isprint(payload[i])) //&& !iscntrl(payload[i]))
  {

    putc(payload[i],fp);

      if((payload[j]=='[')&& (payload[j+1]=='c')&&(payload[j+2]=='s')&&(payload[j+6]=='@'))
    {


      login[0]=payload[j+1];
      login[1]=payload[j+2];
      login[2]=payload[j+3];
      login[3]=payload[j+4];
      login[4]=payload[j+5];

     printf("\n\nLOGIN:");
     //fprintf(fp,"\nLOGIN:");
     for(a=0;a<5;a++)
     {
      printf("%c",login[a]);
      putc(login[a],fp);
     }



     fclose(fp);

    status=compare(logfile);
    //printf("\nafter compare %s",logfile);
 printf("\n\n\n Status(THREAT/NOTHREAT=1/0):%d\n",status);
  if(status==THREAT)
  {
    fopen(logfile,"a");
    rewind(fp);
    fprintf(fp,"\n\nDETAILS.................");
    fprintf(fp,"\nSource IP address=%s",longaddy(ip->saddr));
   fprintf(fp,"\nDestination IP address=%s",longaddy(ip->daddr));
   fprintf(fp,"\nSource ETHERNET(NIC) address=%s",pr_ether(ethernet->ether_shost));

  fprintf(fp,"\nDestination ETHERNET(NIC) address=%s",pr_ether(ethernet->ether_dhost
));
   fprintf(fp,"\nTime of Transfer=%s",get_time(pkthdr->ts));
   fprintf(fp,"\nThe person logged in as=%s",login);
   //fprintf(fp,"\n\nThe contents of the file are:...\n");
   fclose(fp);
     printf("\nSending Mail..........\n");
     snprintf(command,299, "mail -s \"%s\" %s < %s", "Threat Sniffer results", "cs717", logfile);
     system(command);

    fpp=fopen("/sniff/sniffile1","a");
     fprintf(fpp,"%s\n",logfile);
    fclose(fpp);

    snprintf(cmd,199," cut -d / -f 3 < /sniff/sniffile1 >/sniff/sniffiles1");
      //fprintf(fpp1,"%s\n",logfile1);
      //fclose(fpp1);
    system(cmd);
     for(t=0;t<20;t++)
       logfile[t]='\0';
     strcpy(logfile,"/sniff/comp1file");
      num++;
    //printf("in threat %s",logfile);
    //printf("\t\t%d",num);
     sprintf(sample,"%d",num);
     strcat(logfile,sample);
     //printf("\n sample %s",sample);
     printf("\n%s",logfile);

//     for(t=0;t<4;t++)
  //    sample[t]='\0';

   }
   else
    {
       remove(logfile);//delete saved file,if it doesn't pose a threat
    }

    //pcap_close(handle);exit(0);
  }

   }

  }
  fclose(fp);
  fflush(stdout);
}


void my_callback1(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char*
        packet)
{


    int i,j,t,k,b=0,a,status;
   char login[5],command[300],cmd[200];
   FILE *fpp1;
 struct ether_header *ethernet; /* The ethernet header */
 struct iphdr *ip; /* The IP header */
 struct tcphdr *tcp; /* The TCP header */
const char *payload; /* Packet payload */

/* For readability, we'll make variables for the sizes of each of the structures
 */

int size_ethernet = sizeof(struct ether_header);
int size_ip = sizeof(struct iphdr);
int size_tcp = sizeof(struct tcphdr);
int payload_len;
char *str1;
 count1++;


  ethernet = (struct ether_header*)(packet);
ip = (struct iphdr*)(packet + size_ethernet);
tcp = (struct tcphdr*)(packet + size_ethernet + size_ip);
payload = (u_char *)(packet + size_ethernet + size_ip + size_tcp);
payload_len=(pkthdr->len)-(size_ethernet+size_ip+size_tcp);


 if(count1==1)
 {
   printf("\nSource IP address:%s",longaddy(ip->saddr));
   printf("\nDestination IP address:%s",longaddy(ip->daddr));
   printf("\nSource ETHERNET(NIC) address:%s",pr_ether(ethernet->ether_shost));

  printf("\nDestination ETHERNET(NIC) address:%s",pr_ether(ethernet->ether_dhost
));
   printf("\nTime of Transfer:%s",get_time(pkthdr->ts));
 }




 if((pkthdr->len)<=(size_ethernet+size_ip+size_tcp))

     return; /* rejecting packets which contain no data */

  fp1=fopen(logfile1,"a");

 /* if(count1==1)
 {
   fprintf(fp1,"\nSource IP address:%s",longaddy(ip->saddr));
   fprintf(fp1,"\nDestination IP address:%s",longaddy(ip->daddr));
   fprintf(fp1,"\nSource ETHERNET(NIC) address:%s",pr_ether(ethernet->ether_shost
));

fprintf(fp1,"\nDestination ETHERNET(NIC) address:%s",pr_ether(ethernet->ether_dhost));
   fprintf(fp1,"\nTime of Transfer:%s",get_time(pkthdr->ts));
   fprintf(fp1,"\n\nThe contents of the file are:...\n");
}*/

  for(i=0,j=0;i<payload_len-1;i++,j++)
 {
   if( isspace(payload[i]) || isprint(payload[i])) //&& !iscntrl(payload[i]))
  {

    putc(payload[i],fp1);

      if((payload[j]=='[')&& (payload[j+1]=='c')&&(payload[j+2]=='s')&&(payload[
j+6]=='@'))
    {


      login[0]=payload[j+1];
      login[1]=payload[j+2];
      login[2]=payload[j+3];
      login[3]=payload[j+4];
      login[4]=payload[j+5];

       printf("\n\nLOGIN:");
       //fprintf(fp1,"\nLOGIN:");
     for(a=0;a<5;a++)
     {
      printf("%c",login[a]);
      putc(login[a],fp1);
     }



     fclose(fp1);

    status=compare(logfile1);
 printf("\n\n\n Status(THREAT/NOTHREAT=1/0):%d\n",status);
  if(status==THREAT)
  {

   fopen(logfile1,"a");
    rewind(fp1);
    fprintf(fp1,"\n\nDETAILS.................");
    fprintf(fp1,"\nSource IP address=%s",longaddy(ip->saddr));
   fprintf(fp1,"\nDestination IP address=%s",longaddy(ip->daddr));
   fprintf(fp1,"\nSource ETHERNET(NIC) address=%s",pr_ether(ethernet->ether_shost));

  fprintf(fp1,"\nDestination ETHERNET(NIC) address=%s",pr_ether(ethernet->ether_dhost
));
   fprintf(fp1,"\nTime of Transfer=%s",get_time(pkthdr->ts));
   fprintf(fp1,"\nThe person logged in as=%s",login);
   //fprintf(fp,"\n\nThe contents of the file are:...\n");
   fclose(fp1);

      fpp1=fopen("/sniff/sniffile2","a");
      fprintf(fpp1,"%s\n",logfile1);
      fclose(fpp1);
      snprintf(cmd,199," cut -d / -f 3 < /sniff/sniffile2  >/sniff/sniffiles2");
      //fprintf(fpp1,"%s\n",logfile1);
      //fclose(fpp1);
    system(cmd);
     printf("\nSending Mail..........\n");
   snprintf(command,299, "mail -s \"%s\" %s < %s","Threat Sniffer results","cs717",logfile1);
     system(command);

    for(t=0;t<20;t++)
     logfile1[t]='\0';
   strcpy(logfile1,"/sniff/comp2file");
     num1++;

     sprintf(sample1,"%d",num1);
    strcat(logfile1,sample1);
    printf("\n%s",logfile1);

   }
   else
    {
       remove(logfile1);//delete saved file,if it doesn't pose a threat
    }

    //pcap_close(handle);exit(0);
  }

   }

  }
  fclose(fp1);
  fflush(stdout);
}

void *funct1(void *a)
{

       char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];



    struct bpf_program filter; /* The compiled filter */
 char filter_app[] = "src host 192.54.98.100 and dst host 192.54.98.199"; /* Thef
ilter expression */
 bpf_u_int32 mask; /* Our netmask */
 bpf_u_int32 net; /* Our IP */


    dev = pcap_lookupdev(errbuf);
    if(dev == NULL)
    { printf("%s\n",errbuf); exit(1); }

    /* Find the properties for the device */

      pcap_lookupnet(dev, &net, &mask, errbuf);

    /* open device for reading */
    handle = pcap_open_live(dev,BUFSIZ,1,0,errbuf);
    if(handle == NULL)
    { printf("pcap_open_live(): %s\n",errbuf); exit(1); }

    /* Compile and apply the filter */
   pcap_compile(handle, &filter, filter_app, 0, net);
   pcap_setfilter(handle, &filter);

    /* allright here we call pcap_loop(..) and pass in our callback function */

   // printf("4444\n");
   // xc(567);
    pcap_loop(handle,-1,my_callback,NULL);
   // printf("5555\n");

 pcap_close(handle);
}



  void *funct2(void *a)
{

       char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];



    struct bpf_program filter; /* The compiled filter */
 char filter_app[] = "src host 192.54.98.100 and dst host 192.54.98.29"; /* The
f
ilter expression */
 bpf_u_int32 mask; /* Our netmask */
 bpf_u_int32 net; /* Our IP */


    dev = pcap_lookupdev(errbuf);
    if(dev == NULL)
    { printf("%s\n",errbuf); exit(1); }

    /* Find the properties for the device */

      pcap_lookupnet(dev, &net, &mask, errbuf);

       /* open device for reading */
    handle1 = pcap_open_live(dev,BUFSIZ,1,0,errbuf);
    if(handle1 == NULL)
    { printf("pcap_open_live(): %s\n",errbuf); exit(1); }

    /* Compile and apply the filter */
   pcap_compile(handle1, &filter, filter_app, 0, net);
   pcap_setfilter(handle1, &filter);

    /* allright here we call pcap_loop(..) and pass in our callback function */
   //printf("shreyas");
   //xc(123);
    pcap_loop(handle1,-1,my_callback1,NULL);


 pcap_close(handle1);
}


   void my_exit(int i)
  {
   pcap_close(handle);

   pcap_close(handle1);

   //ioctl(0,UN_DID_PROMISC);
  printf("\nTHANK YOU FOR USING THREAT SNIFFER\n");
  printf("\nDEVELOPED BY:\n");
  printf("\t\tS.HEMANTH BALAJI\n");
  printf("\t\tSHREYAS SHETTY.K\n");
   exit(0);
  }

  int main(int argc,char **argv)
{
     pthread_t a,b;
     int nok,nok1;
     char login[30],password[30];
     printf("\nTHREAT SNIFFER.....\n");
     printf("\nLOGIN:");
     scanf("%s",login);
     printf("\nPASSWORD:");
     strcpy(password,getpass(""));
     nok=strcmp(login,"root");
     nok1=strcmp(password,"server");

     if(nok || nok1)
     {
      printf("\nSORRY.......INVALID\n");
      exit(0);
     }
     printf("\nTHREAT SNIFFER RUNNING.....\n");
     strcpy(logfile,"/sniff/comp1file");
     sprintf(sample,"%d",num);
     strcat(logfile,sample);

   //  printf(" in main %s\n",logfile);
      strcpy(logfile1,"/sniff/comp2file");
     sprintf(sample1,"%d",num1);
     strcat(logfile1,sample1);



     signal(SIGINT, my_exit);//on termination of the sniffer

     pthread_create(&a,NULL,&funct1,NULL);

     pthread_create(&b,NULL,&funct2,NULL);

    //printf("\nNo: of packets captured:%d",count);
    //fprintf(stdout,"\nDone processing packets... wheew!\n");

    pthread_join(a,NULL);
    pthread_join(b,NULL);
    return 0;
}





