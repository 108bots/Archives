/* Sample file transfer server
 * Author: Hemanth Srinivasan
 * Year: 2002
*/
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_HOST_ADDR "127.0.0.1"

main()
{
 int socfd,newsocfd,childid,clilen,n=0,ffd;
 struct sockaddr_in seradr,cliadr;
 char buf[64];
 //static char name[30]={'\0'};
 static char fname[30]={'\0'};

 if((socfd=socket(AF_INET,SOCK_STREAM,0))<0)
   perror("\nERROR:server cannot open the socket");
 bzero((char *) &seradr,sizeof(seradr));
 seradr.sin_family=AF_INET;
 seradr.sin_addr.s_addr=htonl(INADDR_ANY);
 seradr.sin_port=5001;
 
 if(bind(socfd,(struct sockaddr *) &seradr,sizeof(seradr))<0)  {
   printf("\nERROR:server can't bind the local address");
   exit(0);
  }

 printf("\nSocket binded and named");

 //listen(socfd,5);

 printf("\nServer waiting....connection awaited");

 listen(socfd,5);

 for(;;)
 {
  clilen=sizeof(cliadr);
  newsocfd=accept(socfd,(struct sockaddr *) &cliadr,&clilen);
 
  if(newsocfd<0)
  {
   perror("\nAccept error in server");
   exit(0);
  }

  printf("\nConnection established from server\n");
 
  if((childid=fork())<0)
   perror("\nServer fork error");
  if(childid==0)
  {
   close(socfd);
   printf("\nIn the child process,old socfd closed\n");
  
   strcpy(fname,"sent.c");
  // printf("Received the filename %s %d\n",name,n);
   
   printf("Sending file %s\n",fname);
   
   ffd=open(fname,O_RDONLY);
    while((n=read(ffd,buf,64))>0)
     write(newsocfd,buf,n,0);
    exit();
  }
 if(childid)
  {
   signal(SIGCLD,SIG_IGN);

   printf("\nClosing new socfd\n");
   close(newsocfd);
  }
 }
}
  
 
