/* Sample file transfer client
 * Author: Hemanth Srinivasan
 * Year: 2002
*/
#include <stdio.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

main(int argc,char *argv[])
{
  int socfd,ffd;
  char buf[64];
  struct sockaddr_in seradr;
  int fp,n;

  bzero((char *) &seradr,sizeof(seradr));
  seradr.sin_family=AF_INET;
  seradr.sin_addr.s_addr=inet_addr("127.0.0.1");
  seradr.sin_port=5501;

  if((socfd=socket(AF_INET,SOCK_STREAM,0))<0)
  {
   printf("\nError:Client cannot open the socket");
   exit(0);
  }

  if(connect(socfd,(struct sockaddr *) &seradr,sizeof(seradr))<0)
 {
  perror("Client cannot connect to the server");
  exit();
 }

 while((n=recv(socfd,buf,64,0))>0)
  write(1,buf,n);
 close(socfd);
 exit();
}

