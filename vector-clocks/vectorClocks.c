 /*
Vector Clocks Program
Authors : Hemanth Srinivasan
Year: 2007
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/nameser.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>
#include <pthread.h>

#define SERVER_PORT 6061

int *vectorClock;

struct vcMessage {
   char **msgTimestamp;
   char data[5];

};



int numProcess; //number of processes in the distributed system
int dValue; // incremental d value in vector clocks
int tMin, tMax; //timeout range for msg generation
float pValue; // probability of local and send events
int EValue; // total event count

char **peerAddrList; 
char logfileName[25];
char inputfileName[25];

int selfID; //ID of this process
int *connected_socks;  /* Array of connected sockets to know who we are talking to */

//pthread variables
pthread_mutex_t safe_mutex, vector_mutex, file_mutex, connect_mutex;

FILE *logfile;
char title[] = "Event Type     To/From         Vector Clock\n\n";


//function declarations
void parse_Input (void);
int is_Connected (int);
void set_Connected (int, int);
void *serverFunctionThread (void *arg);
void *clientFunctionThread (void *arg);
int getProcessID (struct in_addr);
void updateLogfile (char [], int *, int);
int readVectorClock (int);
void writeVectorClock (int, int);
void vectorClockSimulation (void);
int randomTimeout (void);
int eventType (void);
int *randomPeerSelect (void);
char *buildsendMsg(void);
void itoa(int, char []);
int updateVectorTimeStamp(char *);
void printVectorClock ();



int main (int argc, char *argv[]) {

  int i;
  pthread_t clientThread_id;
  pthread_t serverThread_id;
  void *cexitStatus,*sexitStatus;
  

  if (argc != 3) {

   printf("\nUsage: vectorClocks <ProcessID> <Config File>\n");
   exit(0);

  }
  
  strcpy (inputfileName, argv[2]);

  strcpy (logfileName, argv[1]);
  strcat (logfileName, "_");
  strcat (logfileName, "vectorClocks.log");

  selfID = atoi(argv[1]);  
  
  printf("\nReading Input File...\n");
  //function call to parse the input file and fill up the parameters
  parse_Input();

  //Allocate space and Initialize vector clock structure
  vectorClock = (int *)malloc(numProcess * sizeof(int));
  for (i = 0; i < numProcess; i++) 
    vectorClock[i] = 0;
  
  //create log file
  if ((logfile = fopen(logfileName, "w")) == NULL) {
    printf("\nError in creating log file\n");
  }

  if (fputs(title, logfile) < 0) {
    printf("\nError in writing to log file\n");
  }
  fclose(logfile);

  //initialize the mutexes 
  pthread_mutex_init(&safe_mutex,NULL);
  pthread_mutex_init(&vector_mutex,NULL);
  pthread_mutex_init(&file_mutex,NULL);
  pthread_mutex_init(&connect_mutex,NULL);

  //list of connected sockets
  connected_socks = (int *)malloc(numProcess * sizeof(int));
  for (i = 0; i < numProcess; i++ )
	 connected_socks[i] = -1; 

  
  // Creating worker threads
  if(pthread_create(&clientThread_id,NULL,clientFunctionThread,NULL) < 0) {
	 printf("\nProblem in client worker thread creation \n");
  }

  if(pthread_create(&serverThread_id,NULL,serverFunctionThread,NULL) < 0) {
	 printf("\nproblem in server worker thread creation \n");
  } 

  pthread_join(clientThread_id,&cexitStatus);
  pthread_join(serverThread_id,&sexitStatus);

  return 0;

}

void parse_Input () {

  
  FILE *input_file;
  char cN[4], cD[4], cTmin[4], cTmax[4], cP[4], cE[4];  
  int c, i, j;


  if ((input_file = fopen(inputfileName, "r")) == NULL) {
    printf("\nError in opening input file\n");
	exit(0);
  }


  while(1) {

    
	j = 0;
	while ((c = getc(input_file)) != '/') {  //read the total number of nodes
     cN [j++] = c;
	}
    cN [j] = '\0';
   // printf("\ncN is %s\n", cN );
    numProcess = atoi(cN);
    printf("\nnumProcess is %d\n", numProcess );

    while ((c = getc(input_file)) != '\n'); //goto end of line

	j = 0;
	while ((c = getc(input_file)) != '/') {  //read value of d
     cD [j++] = c;
   	}
    cD [j] = '\0';

	//printf("\ncD is %s\n", cD );
	dValue = atoi(cD);
    printf("\ndValue is %d\n", dValue );


    while ((c = getc(input_file)) != '\n'); //goto end of line

	//allocate space for storing peer addresses
	peerAddrList = (char **)calloc(numProcess, sizeof(char *));
    for (i = 0; i < numProcess; i++)
      peerAddrList[i] = (char *)calloc(6, sizeof(char));

	//fill the addresses
	for (i = 0; i < numProcess; i++) { 
     
      j = 0;
      while ((c = getc(input_file)) != '.') {  //read the netid
        peerAddrList[i][j++] = c;
	  }
      peerAddrList[i][j] = '\0';

      printf("\npeerAddrList[%d] is %s\n", i, peerAddrList[i]);
  
      while ((c = getc(input_file)) != '\n'); //goto end of line


	}

    j = 0;
	while ((c = getc(input_file)) != '/') {  //read value of tmin
     cTmin [j++] = c;
   	}
    cTmin [j] = '\0';

	//printf("\ncTmin is %s\n", cTmin );
	tMin = atoi(cTmin);
    printf("\ntMin is %d\n", tMin );
	
	while ((c = getc(input_file)) != '\n'); //goto end of line
    
	j = 0;
	while ((c = getc(input_file)) != '/') {  //read value of tmax
     cTmax [j++] = c;
   	}
    cTmax [j] = '\0';

	//printf("\ncTmax is %s\n", cTmax );
	tMax = atoi(cTmax);
    printf("\ntMax is %d\n", tMax );
	
	while ((c = getc(input_file)) != '\n'); //goto end of line

	j = 0;
	while ((c = getc(input_file)) != '/') {  //read the probability value
     cP [j++] = c;
	
	}
    cP [j] = '\0';

	//printf("\ncP is %s\n", cP);
	pValue = atof(cP);
    printf("\npValue is %f\n", pValue);

    while ((c = getc(input_file)) != '\n'); //goto end of line

    j = 0;
	while ((c = getc(input_file)) != '/') {  //read value of E
     cE [j++] = c;
   	}
    cE [j] = '\0';

	//printf("\ncE is %s\n", cE);
	EValue = atoi(cE);
    printf("\nEValue is %d\n", EValue );

	break;

  } //end while(1)

  fclose(input_file);


}

int is_Connected (int seekvalue) {

	pthread_mutex_lock(&safe_mutex);
	if( connected_socks[seekvalue] > -1 )
	{
		pthread_mutex_unlock(&safe_mutex);
		return 1;
	}
	else
	{
		pthread_mutex_unlock(&safe_mutex);
		return 0;
	}


}

void set_Connected (int seekvalue, int pfd) {

	pthread_mutex_lock(&safe_mutex);
	connected_socks[seekvalue] = pfd;
	pthread_mutex_unlock(&safe_mutex);

}

void *serverFunctionThread (void *arg) {
   //Server variables
   struct sockaddr_in servAddress, peerAddress;
   struct timeval servTimeout;
   char opt= '1';
   int listenfd, peer_fd, socks_read;
   int maxsock, i;
   fd_set mon_socks;
   int listnum, peer_id;
   int sin_size = sizeof(struct sockaddr_in);
   char *mrecv_data;
   int recv_num, rcvID;

   /* creating a socket */
   if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
     fprintf(stderr, "\nError in creating listening socket");
	 perror("socket");
     exit(1);
   }

   /*Prevent the address already in use error*/
   if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == 1) {
     fprintf(stderr, "\nError in setting socket options of reuse address.\n");
	 perror("setsockopt");
     exit(1);
   }

   /* configuring server address structure */
   servAddress.sin_family = AF_INET;
   servAddress.sin_addr.s_addr = htonl(INADDR_ANY);
   servAddress.sin_port = htons(SERVER_PORT);

   memset(&(servAddress.sin_zero), '\0', 8);

   /* binding the socket to the service port */
   if (bind(listenfd, (struct sockaddr*) &servAddress, sizeof(servAddress)) < 0) {
     fprintf(stderr, "\nError in binding the socket to the port\n");
	 perror("bind");
     exit(1);
   }


   /* listen on the port*/
    if (listen(listenfd, numProcess) < 0) {
      fprintf(stderr, "\nError in listening on the port\n");
	  perror("listen");
      exit(1);
	}

    maxsock = listenfd;

	while(1) {

	    FD_ZERO(&mon_socks);
	    FD_SET(listenfd,&mon_socks);

        pthread_mutex_lock(&connect_mutex);
        for (listnum = 0; listnum < numProcess; listnum++) {
			if (connected_socks[listnum] > -1) {
				FD_SET(connected_socks[listnum],&mon_socks);
				if (connected_socks[listnum] > maxsock)
					maxsock = connected_socks[listnum];
			}
		}
		pthread_mutex_unlock(&connect_mutex);

        /*call select with 10 sec timeout interval*/
		servTimeout.tv_sec = 10;
		servTimeout.tv_usec = 0;
        
		

		socks_read = select(maxsock+1, &mon_socks, NULL, NULL, &servTimeout);
       // printf ("\nsocks_read = %d", socks_read);

		if (socks_read < 0) {
			fprintf(stderr, "\nError in selecting connections\n");
			perror("select");
			exit(1);
		}

		if (socks_read == 0) {
			/* print "S" to show server is alive */
			printf(".");
			fflush(stdout);
		}
		else { //activity on some socket

			if (FD_ISSET(listenfd,&mon_socks)) { /*new connection*/

				peer_fd = accept(listenfd,(struct sockaddr *)&peerAddress,&sin_size);
		        
                printf("\nAccepted a connection on socket %d\n", peer_fd);
				//get processID based on peer's IP address
                if( (peer_id = getProcessID(peerAddress.sin_addr)) == -1 ) {
                   printf("\nPeer not recognized\n");
				   continue;
				}
                printf("Peer_ID is :%d\n", peer_id);
                
                //safely store peer_fd to connected_socks
                
				if(!is_Connected(peer_id)) {
					set_Connected (peer_id, peer_fd);
				 
					if (send(peer_fd, "A", 10, 0) < 0) {
	                   fprintf(stderr, "\nError in sending Accept Message\n");
                       perror("send");
					}
				}
				else { //connection already established, close the socket
                    printf("\nClosing already connected socket with peer[%d] %s\n", peer_id, peerAddrList[peer_id] );  
					if (send(peer_fd, "D", 10, 0) < 0) {
	                   fprintf(stderr, "\nError in sending Deny Message\n");
                       perror("send");
					}
					close(peer_fd);
				}



			}
			else {  //data recieved on some socket
                pthread_mutex_lock(&connect_mutex);
				for (listnum = 0; listnum < numProcess; listnum++) {
					if (connected_socks[listnum] < 0) continue;
					if (FD_ISSET(connected_socks[listnum],&mon_socks)) {
                        
						//printf("\nData Received on connected_sock[%d] = %d", listnum, connected_socks[listnum]);

						//recv msg
						mrecv_data = (char *)calloc((numProcess*6+10),sizeof(char));
				
				        recv_num = recv(connected_socks[listnum], mrecv_data, ((numProcess*6+10)*sizeof(char)), 0);
                        if (recv_num == 0) {
                           fprintf(stderr, "\nError in recieving timestamped message\n");
			         	   perror("recv");
				           continue;
						}
						else if (recv_num > 0) { 
                            printf("\nReceived a Message!!!");
                           // printf("\nmessage contains:\n %s", mrecv_data);
                           //termination notification message
							if (strcmp(mrecv_data, "T") == 0) {
			                  printf("\nMessage is a Termination Notification Message from Process[%d] (%s)", listnum, peerAddrList[listnum]);
                              continue;
							}

                           //update vector timestamp based on recieved timestamp
                          
                           printf("\nMessage is a Timestamp Message!!!");
                           rcvID = updateVectorTimeStamp(mrecv_data);
     			           
						   printf("\nSender of the Message was Process[%d] (%s)", rcvID, peerAddrList[rcvID]);
						  
						   //update logfile
                           updateLogfile ("Receive", &rcvID, 3);
                          
						   printVectorClock ();


						}
					 
						free(mrecv_data);
					}


				} //end listnum
                
				
                pthread_mutex_unlock(&connect_mutex);
			} //end data recv
		
		} //end socket activity

	} //end while


}


void *clientFunctionThread (void *arg) {

	int connect_flag = 0, id = -1, sockfd;
    struct sockaddr_in cliAddress;
    struct timeval cliTimeout;
    struct hostent *peer_info;
    int i; 
    int maxin_socks, insocks_read;
    fd_set in_socks;
    char in_buf[8];
	char perm[10];
	int recv_perm;

	while (!connect_flag) {

	   for (i = 0; i < numProcess; i++) {
		   if(i == selfID) continue;
		   if(is_Connected(i)) 
			   connect_flag = 1;
		   else {
			 connect_flag = 0;
			 id = i;
			 break;
		   }
	   }

       if ((!connect_flag) && (id > -1) && (id != selfID)) {
	 	  cliAddress.sin_family = AF_INET;
		  cliAddress.sin_port = htons((unsigned)SERVER_PORT);
		  memset(&(cliAddress.sin_zero), '\0', 8);

		  if ((peer_info=gethostbyname(peerAddrList[id])) == NULL) { 
            fprintf(stderr, "\nCould not resolve the Hostname\n");
		    perror("gethostbyname");
            continue;
		  }
	      cliAddress.sin_addr = *((struct in_addr *)peer_info->h_addr);
 
		  //printf("\nIP address of - %s - is %s\n", peerAddrList[id], inet_ntoa(cliAddress.sin_addr));
         
          
		  if (inet_pton(AF_INET, inet_ntoa(cliAddress.sin_addr), &cliAddress.sin_addr) <= 0) {
            fprintf(stderr, "\nError in connecting using hostname\n");
			perror("inet_pton");
		    continue;
		  }

		  
		  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		     fprintf(stderr, "\nClient:Error in creating socket\n");
		     perror("socket");
		     continue;
		  }
		  

		 /* connecting to the server */
		 if (connect(sockfd, (struct sockaddr *)&cliAddress, sizeof(cliAddress)) < 0) {
		   fprintf(stderr, "\nError in connecting to the  server\n");
		   perror("connect");
		   continue;
		 }
          
		 

		 recv_perm = recv(sockfd, perm, 10, 0);
         if (recv_perm == 0) {
             fprintf(stderr, "\nError in recieving connection status message\n");
		     perror("recv");
		     continue;
		 }
		 else if (recv_perm > 0) { 
			 if(strcmp(perm, "A") == 0) {
				 set_Connected (id, sockfd);
			 }
			 else if(strcmp(perm, "D") == 0) {
                    
                  close(sockfd);
			 }
         
		 }
		 
							
	   } //end if !connect

	} //end while !connect

	printf("\nConnections among all processes established successfully!!!\n");
    printf("\nPress 'S' to start VectorClock Simulation!!!\n");
    
	pthread_mutex_lock(&connect_mutex);
	for (i = 0; i < numProcess; i++) {
      printf("\nFinal ConnectedState[%d] is %d", i, connected_socks[i]);
	
	}
    pthread_mutex_unlock(&connect_mutex);

	cliTimeout.tv_sec = 10;
	cliTimeout.tv_usec = 0;
	maxin_socks = fileno(stdin);
    
	while (1) {
		
		FD_ZERO(&in_socks);
		FD_SET(fileno(stdin),&in_socks);


		/*call select with the timeout interval*/
        
    	insocks_read = select(maxin_socks+1, &in_socks, NULL, NULL, &cliTimeout);

		if (insocks_read < 0) {
		   fprintf(stderr, "\nError in reading user input\n");
		   perror("select");
		   continue;
		}

		if (insocks_read == 0) {
		 /* print "." to show client  is waiting for user input */
		 printf(".");
		 fflush(stdout);
		}
		else if (FD_ISSET(fileno(stdin),&in_socks)) { //if some activity on socket

            if (read(1, in_buf, 8) < 0)
     		  printf("\nError in select/reading user input!!");
	    	printf("\nKey Pressed!!!!" );
							
			if (strstr(in_buf,"S") != NULL) { //S to start the vector clock compuation
			  	printf("\nNow Starting Vector Clock Recording!!!\n");
			    vectorClockSimulation();  				
			}
			else if(strstr(in_buf,"T") != NULL) { //Termination notification

               pthread_mutex_lock(&connect_mutex);
	  		   printf("\nNow Sending Finished signal to all Processes!!!\n");  
			   for (i = 0; i < numProcess; i++) {
                 if (i == selfID) continue;
			     if (send(connected_socks[i], "T", 10, 0) < 0) {
	                   fprintf(stderr, "\nError in sending Termination Notification Message\n");
                       perror("send");
				 }
			   }
               pthread_mutex_unlock(&connect_mutex);              
 
			   //reseting vectorClocks 
			   printf("\nNow Resetting the Vector Clocks!!!\n"); 
               pthread_mutex_lock(&vector_mutex);
			   for (i = 0; i < numProcess; i++) 
                  vectorClock[i] = 0;
               pthread_mutex_unlock(&vector_mutex);

               //Reseting Logfiles
			   printf("\nNow Resetting the Logfiles!!!\n"); 
               pthread_mutex_lock(&file_mutex);
			   //removing log file
               if (remove(logfileName) != 0) {
                  printf("\nError in removing log file\n");
			   }
			   //create log file
               if ((logfile = fopen(logfileName, "w")) == NULL) {
                  printf("\nError in re-creating log file\n");
			   }

               if (fputs(title, logfile) < 0) {
                  printf("\nError in re-writing to log file\n");
			   }
               fclose(logfile);
               pthread_mutex_unlock(&file_mutex);
			}

			fflush(stdin);
		}

	}//end while
    

}



int getProcessID (struct in_addr peerAddr) {

 struct hostent *peer_info;
 int i;

 
 if ((peer_info = gethostbyaddr((const char *)&peerAddr, sizeof(peerAddr), AF_INET)) == NULL) { 
    fprintf(stderr, "\nError in getting IP address for peer\n");
    perror("gethostbyaddr");
 }

 printf("Peer name is: %s\n", peer_info->h_name);

 for (i = 0; i < numProcess; i++) {

 if ( strstr(peer_info->h_name, peerAddrList[i] ) != NULL) 
   return i;
 else
  continue;
 
 }

 return -1;

}

void updateLogfile (char buffer[], int *prcID, int type) { //type 1-local, 2-send, 3-Receive

  FILE *log_file;
  char *fileBuffer;
  int i;
  char tmpStmp[6];
  char tmpID[5];
  int count=0;;

  //printf("\nEntering Update log file\n");
  
  fileBuffer = (char *)malloc((numProcess*6+10+50)*sizeof(char));

  switch (type) {

     case 1: strcpy(fileBuffer, buffer);
		     count = strlen(fileBuffer);
			 for (i = 0; i < (19 - strlen(fileBuffer)); i++ ) {
               strcat(fileBuffer, " ");
			   count++;
			 }
			 strcat(fileBuffer, "-");
			 count++;
             for (i = 0; i < (((numProcess-1)*3+17) - count); i++ ) {
               strcat(fileBuffer, " ");
			 }
			
			
             for (i = 0; i < numProcess; i++ ) {
               itoa(readVectorClock (i), tmpStmp);
               strcat(fileBuffer, tmpStmp);
               strcat(fileBuffer, ",");
			 }
			 strcat(fileBuffer, "\n");
			
		     break;

     case 2: strcpy(fileBuffer, buffer);
		     count = strlen(fileBuffer);
		     for (i = 0; i < (19 - strlen(fileBuffer)); i++ ) {
               strcat(fileBuffer, " ");
			   count++;
			 }
                
			 for (i = 1; i <= prcID[0]; i++ ) {
               strcat(fileBuffer, "P");
			   itoa(prcID[i], tmpID);
               strcat(fileBuffer, tmpID);
			   strcat(fileBuffer, ",");
			   count+=3;
			 }

             for (i = 0; i < (((numProcess-1)*3+17) - count); i++ ) {
               strcat(fileBuffer, " ");
			 }
             for (i = 0; i < numProcess; i++ ) {
               itoa(readVectorClock (i), tmpStmp);
               strcat(fileBuffer, tmpStmp);
               strcat(fileBuffer, ",");
			 }
			 strcat(fileBuffer, "\n");
		     break;
		 
		     
	 case 3: strcpy(fileBuffer, buffer);
		     count = strlen(fileBuffer);
		     for (i = 0; i < (19 - strlen(fileBuffer)); i++ ) {
               strcat(fileBuffer, " ");
			   count++;
			 }
			 strcat(fileBuffer, "P");
			 count++;
             itoa(*prcID, tmpID);
			 strcat(fileBuffer, tmpID);
			 count++;
             for (i = 0; i < (((numProcess-1)*3+17) - count); i++ ) {
               strcat(fileBuffer, " ");
			 }
             for (i = 0; i < numProcess; i++ ) {
               itoa(readVectorClock (i), tmpStmp);
               strcat(fileBuffer, tmpStmp);
               strcat(fileBuffer, ",");
			 }
			 strcat(fileBuffer, "\n");
		     break;


	 default: printf("\nInvalid Event Type\n");
  }


  pthread_mutex_lock(&file_mutex);

  if ((log_file = fopen(logfileName, "a")) == NULL) {
    printf("\nError in opening log file\n");
  }

  if (fputs(fileBuffer, log_file) < 0) {
    printf("\nError in writing to log file\n");
  }
  fclose(log_file);
  pthread_mutex_unlock(&file_mutex);

  free(fileBuffer);
  //printf("\nLeaving Update log file\n");
}


int readVectorClock (int index) {
  
  int value; 
  pthread_mutex_lock(&vector_mutex);
  value = vectorClock[index];
  pthread_mutex_unlock(&vector_mutex);
  return value;

}

void writeVectorClock (int index, int value) {

  pthread_mutex_lock(&vector_mutex);
  vectorClock[index] = value;
  pthread_mutex_unlock(&vector_mutex);
 
}

void printVectorClock () {
  
  int i;
  
  pthread_mutex_lock(&vector_mutex);
  printf("\nMy Current Vector Clock Value is:\n");
  for (i = 0; i < numProcess; i++)
     printf("P[%d] = %d, ", i, vectorClock[i]);
  printf("\n");
  pthread_mutex_unlock(&vector_mutex);
 
}

void vectorClockSimulation () {

   int eventCount = 0;
   int waitTime, i, count;
   int localEvent;
   int selfvectorValue;
   int *sendList;
   char *buffer;

   while (eventCount < EValue) {
      
	  waitTime = randomTimeout();
	  sleep(waitTime);
      localEvent = eventType();
    
      if (localEvent) {
        printf("\nGenerating Local Event\n");
        //update self vector clock
        selfvectorValue = readVectorClock (selfID);
		selfvectorValue += dValue;
        writeVectorClock (selfID, selfvectorValue);
        eventCount++;

		printf ("\nEvent count = %d", eventCount);
		
		//update log file
        updateLogfile ("Local", NULL, 1);

		printVectorClock ();
	  }
	  else {  //send event
         printf("\nGenerating Send Event\n");
		 //update self vector clock
         selfvectorValue = readVectorClock (selfID);
		 selfvectorValue += dValue;
         writeVectorClock (selfID, selfvectorValue);
         eventCount++;

         sendList = randomPeerSelect ();
         //build send packet
         buffer = buildsendMsg();

		 //send packet
		 count = sendList[0];
         
		 pthread_mutex_lock(&connect_mutex);
         for (i = 1; i <= count; i++) {
		    if (send(connected_socks[sendList[i]], (const void*) buffer, strlen(buffer), 0) < 0) {
	           fprintf(stderr, "\nError in sending Message\n");
               perror("send");
			}
			else
              printf("\nSent TimeStamped Message to Process[%d] (%s) on socket %d", sendList[i], peerAddrList[sendList[i]], connected_socks[sendList[i]]);
		 }
		 pthread_mutex_unlock(&connect_mutex);

	

        printf ("\nEvent count = %d", eventCount);
		
		//update log file
		updateLogfile ("Send", sendList, 2);    

		printVectorClock ();
	  }

	  free (sendList);
	  free (buffer);

   }
   printf("\nGenerated %d Events. Stopping Vector Clock Simulation!!!", EValue);
   printf("\nPress 'T' to send Termination notification to all Processes and Reset the Vector clocks.");  
 

}

char *buildsendMsg() {

  struct vcMessage sendMsg; 
  int i, val, tmp, maxLen = 0;
  char *msgBuf;

  sendMsg.msgTimestamp = (char **)calloc(numProcess, sizeof(char *));
  for (i = 0; i < numProcess; i++)
      sendMsg.msgTimestamp[i] = (char *)calloc(6, sizeof(char));
  
  //copy timestamp into structure
  for (i = 0; i < numProcess; i++) {
	  val = readVectorClock (i);
	  itoa (val, sendMsg.msgTimestamp[i]);
  }

  //find maxLength of timestamp string
  for (i = 0; i < numProcess; i++) {
      tmp = strlen(sendMsg.msgTimestamp[i]);  
	  if(tmp > maxLen)
        maxLen = tmp;
  }
  
  itoa (selfID, sendMsg.data);

  //allocate space for the msg buffer
  msgBuf = (char *)calloc((numProcess*maxLen+10),sizeof(char));

  strcpy(msgBuf, sendMsg.msgTimestamp[0]);
  strcat(msgBuf, ",");
  for (i = 1; i < numProcess; i++) {
    strcat(msgBuf, sendMsg.msgTimestamp[i]);
	strcat(msgBuf, ",");
  }
  strcat(msgBuf, sendMsg.data);
  strcat(msgBuf, "\0");
  
  printf("\nSend Message contains: %s\n", msgBuf);
  return msgBuf;

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

int randomTimeout () {
  
  int ran_val = 0;
  while ((ran_val <= tMin) || (ran_val >= tMax)) {
   	ran_val = rand() % 10;
  }
  printf("\nTimeout Value = %d", ran_val);
  return ran_val;

}

int eventType () {

 float myrand = 0;
 
 while (myrand == 0)
    myrand=(double)rand()/RAND_MAX;

 printf("\n Random Event = %f", myrand);

 if(myrand <= pValue) { 
		return 1;  //local event
 }
 else
   return 0;   //send event

}

int *randomPeerSelect () { 
  
  int *ran_list; //1st element of this list the number of processes, remaining elements are the id's of those
  int i, ranPeerVal = numProcess + 1;
  int ranPeerID = numProcess + 1;
  int j, dupFlag = 0;
  int temp[numProcess];

  //generating the number of peers
  while (ranPeerVal >= numProcess) {
   	ranPeerVal = rand() % 10; 
    if (ranPeerVal == 0)
	 ranPeerVal = numProcess + 1; 	
  }

  printf ("\nranPeerVal = %d", ranPeerVal);
  ran_list = (int *)malloc((ranPeerVal+1) * sizeof(int));
  
  for (i = 0; i < ranPeerVal+1; i++) 
	ran_list[i] = 0;

  ran_list[0] = ranPeerVal; //number of processes

  if (ranPeerVal == numProcess - 1) {
      for (i = 0; i < numProcess; i++) 
	    temp[i] = i;
      j = 0;
	  for (i = 1; i < ranPeerVal+1 ; i++) {
		  
		  if (temp[j] == selfID) {
		    i--;
            j++;
			continue;
		  }
		  ran_list[i] = temp[j++];
		  printf ("\nran_list[%d] = %d", i, ran_list[i]);
	  }
 
  }
  else {
    for (i = 1; i < ranPeerVal+1; i++) {
 
      while ((ranPeerID >= numProcess) || (ranPeerID == selfID)) {
    	   ranPeerID = rand() % 10;  
	     if (i > 1) {
		  for (j = 1; j < i; j++) {
            if (ranPeerID == ran_list[j])
               dupFlag = 1;  
		   }
		  if (dupFlag) {
			  ranPeerID = numProcess + 1; //reset
			  dupFlag = 0;
			  continue;  
		  }
				  
	   }
	   
	  }
      ran_list[i] = ranPeerID; //list of peer IDs
      printf ("\nran_list[%d] = %d", i, ran_list[i]);
      ranPeerID = numProcess + 1; //reset
	  dupFlag = 0;
	}
  }

  return ran_list;

}

int updateVectorTimeStamp(char *recv_msg) {

   int i, j, k;
   char msgTm[6];
   int recvTmVal, myTmVal, newTmVal;
   char recvID[5];
   int selfvectorValue;
   //printf("\nReceived message contains:\n %s", recv_msg);

   //update self vector clock
   selfvectorValue = readVectorClock (selfID);
   selfvectorValue += dValue;
   writeVectorClock (selfID, selfvectorValue);

   //update based on received time stamp
   for (i = 0, j = 0; i < numProcess; i++) {
       k=0;
	   while (recv_msg[j] != ',') {
         msgTm[k++] = recv_msg[j++];
	   }
	   msgTm[k] = '\0';
	   j++;
       
	   recvTmVal = atoi(msgTm);
       myTmVal = readVectorClock (i);
       
	   if(myTmVal > recvTmVal)
	     newTmVal = myTmVal;
	   else
		 newTmVal = recvTmVal;

       writeVectorClock (i, newTmVal);

	   printf("\nMy TimeStamp for Process[%d] is %d. Recieved TimeStamp is %d. Updated TimeStamp is %d", i, myTmVal, recvTmVal, newTmVal);
  
   }

   k = 0;
   while (recv_msg[j] != '\0')
        recvID[k++] = recv_msg[j++] ;
   recvID[k] = '\0';

   free (recv_msg);
   return atoi(recvID); //return the recieved process ID
  

}
