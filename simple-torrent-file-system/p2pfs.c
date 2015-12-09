/*
Peer to Peer File Sharing System Program
 * Author: Hemanth Srinivasan
 * Year: 2007
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
#include <time.h>
#include <termios.h>
#include <sys/stat.h>


#define SERVER_PORT 6061
#define FILE_TRANSFER_PORT 7071
#define MAX_PEERS 15
#define HOP_TIMER 2  //t_hop_count = hop_count * HOP_TIMER
#define REQUEST_CLEANUP_TIMEOUT 120 //after every REQUEST_CLEANUP_TIMEOUT seconds, one (last) entry from recvRequest_Q will be removed
#define REQUEST_CLEANUP_THRESHOLD 3
#define THREAD_POOL_SIZE 5 //number of worker threads
#define MAX_DUPLICATE 10 //upto 10 multiple matches for a search
#define MAX_RESPONSE_COUNT 5 //wait for atleast these many responses


struct Message {

	char type; //1-search request 2-search response 3-withdraw 4-withdraw_ok
	char hop_count[4]; //max value = 15, only for type 1 and 2
	char record_route[3*MAX_PEERS]; //node id's appended with ',' delimiter; only for type 1 and 2
	char sequence_num[5]; //only for type 1, incremented only new user requests
	char *data; //type  contains
};	            //  1   filename/keyword
	            //  2   filename keyword/nodeID
	            //  3   selfid, <tojoin_id,>
	            //  4   selfid


struct inMessage_Q {      //in coming message queue

    char *in_data;
	struct inMessage_Q *next;
};

struct recvRequest_Q {  //received request message queue

	char org_source[3];  //originating source node id
	char org_seqnum[5];  //correspodning sequence number, unique for every request
	char org_hopcnt[4]; //current hop_count
	struct recvRequest_Q *next;

};

struct recvResponse_Q {
    char *identifier;
	int sqnum;
	char searchData[512]; //conatins strings of the form "filename keyword/ID$"
	struct recvResponse_Q *next;

};

struct fileService_Q {
	int sock;
	char filename[25];
	struct fileService_Q *next;

};

struct download_threadData {

	int index;
	char file[25];
	int node;
	int count;
};


//pointers to the various Queues
struct inMessage_Q *inMsgHead = NULL, *inMsgTail = NULL;
struct recvRequest_Q *recvReqHead = NULL, *recvReqTail = NULL;
struct recvResponse_Q *recvRespHead = NULL, *recvRespTail = NULL;
struct fileService_Q *fileServHead = NULL, *fileServTail = NULL;

int peer_list[MAX_PEERS];
int self_ID; //self node ID

int withdrawOK_count = 0;
int withdraw_flag = 0;
int fixPoint = -1;

char peerAddrList[MAX_PEERS][6] = {  "net22",
                                     "net23",
									 "net24",
									 "net25",
									 "net26",
									 "net27",
									 "net28",
									 "net29",
									 "net30",
									 "net31",
									 "net32",
									 "net33",
									 "net34",
									 "net35",
									 "net36"
							       };


//thread variables
pthread_mutex_t recvReq_mutex, recvResp_mutex, connect_mutex, shared_fp_mutex, logfile_mutex;
pthread_mutex_t inMsg_mutex, fileService_mutex;// = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
pthread_mutex_t withdraw_mutex;



//pthread_mutexattr_t mattr;

//condition variable on which the worker threads operating on the inMsg_Q will wait
pthread_cond_t  got_inMsg_cond   = PTHREAD_COND_INITIALIZER;
pthread_cond_t  got_fileService_cond   = PTHREAD_COND_INITIALIZER;

int num_inMsg = 0;	//number of inMsgs pending to be served
int num_recvReq = 0; //number of queued received search requests
int req_seqnum = 0; //incremented for every new user request
int file_servNum = 0; //number of file transfer requests pending


char logfileName[25];
char peerInfofileName[25];
char sharedfileName[25];
char sharedDirName[25];

FILE *shared_fp, *log_fp;

void *listen_recvThread (void *);
void *file_transferThread (void *);
void *connect_user_interfaceThread (void *);
void *req_cleanupThread (void *);
void *search_requestThread (void *);
void *handle_inComing_Msg (void *);
void *handle_file_service (void *);
void *concurrentDownload_thread (void *);

int getNodeID (struct in_addr);
int is_Connected (int);
void set_Connected (int, int);
int select_random_peer (void);
void itoa(int, char []);
void print_peer_list (void);
int connect_toPeer (int, int, int);
char *extract_fileID (char *);
int peer_count (void);
void handle_display_search_history(void);
void handle_clear_search_history(void);
void handle_node_withdraw(void);
char **search_localSharedfiles (char *);
void addto_respQ (char *);
void build_send_request_message(char *, int, int);
void download_file (char *, int, int, int);
void addto_inMsgQ (char *);
char *getMsg_inMsgQ (void);
void handle_request_message(char *);
void handle_response_message(char *);
void handle_withdraw_message(char *);
void handle_withdrawOK_message(char *);
void addto_requestQ (char *);
struct Message *decode_Msg(char *);
int search_requestQ (struct Message);
void forward_request_message(struct Message, int);
void build_send_response_message(char **, char *, char *, int, int);
void handle_download(char *);
void consume_response(struct Message);
void forward_response_message(struct Message, int);
void addto_fileServiceQ (char *, int);
char *getReq_fileServiceQ (void);
void send_file(char *, int);
void build_send_withdraw_message (int);
void upate_ActiveNodesFile(void);
int response_count (char *);
void process_recvData (char *);
void handle_concurrentDownloads(char *, char *);
int extract_nodeIDs(int *, char *, char *);




int main (int argc, char *argv[]) {

  int i;
  pthread_t listen_recv_thread_id, file_transfer_thread_id, connect_user_interface_thread_id, req_cleanup_thread_id;
  void *listen_recv_exitStatus, *file_transfer_exitStatus, *connect_user_interface_exitStatus, *req_cleanup_exitStatus;

  if (argc != 4) {
    printf("\nUsage: p2pfs <node ID> <Active P-2-P nodes> <Shared Files Data>\n");
    exit(0);
  }
  
  printf("\nInitializing the System...");
  self_ID = atoi(argv[1]);

  strcpy (peerInfofileName, argv[2]);
  strcpy (sharedfileName, argv[3]);
  strcpy (logfileName, argv[1]);
  strcat (logfileName, "_");
  strcat (logfileName, "p2pfs.log");

  //get shared dir name
  i = 0;
  while (sharedfileName[i] != '.') {
	  sharedDirName[i] = sharedfileName[i];
	  i++;
  }
  sharedDirName[i] = '\0';
  printf("\nShared directory name is %s", sharedDirName);

    
  //pthread_mutexattr_init(&mattr);
  //pthread_mutex_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);

  //initialize the mutexes 
  pthread_mutex_init(&connect_mutex,NULL);
  pthread_mutex_init(&recvReq_mutex,NULL);
  pthread_mutex_init(&recvResp_mutex,NULL);
  //pthread_mutex_init(&inMsg_mutex,&mattr);
  pthread_mutex_init(&inMsg_mutex,NULL);
  pthread_mutex_init(&fileService_mutex,NULL);
  pthread_mutex_init(&shared_fp_mutex,NULL);
  pthread_mutex_init(&logfile_mutex,NULL);
  pthread_mutex_init(&withdraw_mutex,NULL);
  

    
  // Creating main worker threads
  if(pthread_create(&listen_recv_thread_id,NULL,listen_recvThread,NULL) < 0) {
	 printf("\nProblem in listen_recvThread creation \n");
  }
  if(pthread_create(&connect_user_interface_thread_id,NULL,connect_user_interfaceThread,NULL) < 0) {
	 printf("\nProblem in connect_user_interfaceThread creation \n");
  }
  if(pthread_create(&file_transfer_thread_id,NULL,file_transferThread,NULL) < 0) {
	 printf("\nProblem in file_transferThread creation \n");
  }
  if(pthread_create(&req_cleanup_thread_id,NULL,req_cleanupThread,NULL) < 0) {
	 printf("\nProblem in req_cleanupThread creation \n");
  }
   

  pthread_join(listen_recv_thread_id,&listen_recv_exitStatus);
  pthread_join(file_transfer_thread_id,&file_transfer_exitStatus);
  pthread_join(connect_user_interface_thread_id,&connect_user_interface_exitStatus);
  pthread_join(req_cleanup_thread_id,&req_cleanup_exitStatus);
  

  return 0;


}

void *listen_recvThread (void *arg) {
   
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
   int recv_num;
   int service_thr_id[THREAD_POOL_SIZE];      // thread IDs for the inMsgQ service threads
   pthread_t  service_threads[THREAD_POOL_SIZE];// thread's structures for the inMsgQ service threads
   char *msgData;

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
    if (listen(listenfd, MAX_PEERS) < 0) {
      fprintf(stderr, "\nError in listening on the port\n");
	  perror("listen");
      exit(1);
	}

	//initialize the peer list 
    for (i = 0; i < MAX_PEERS; i++ ) 
	 peer_list[i] = -1; 
		
    // create the service handling threads 
    for (i = 0; i < THREAD_POOL_SIZE; i++) {
	   service_thr_id[i] = i;
	   printf("\ncreating thread %d\n", i);
	   pthread_create(&service_threads[i], NULL, handle_inComing_Msg, (void*)&service_thr_id[i]);
    }


    maxsock = listenfd;

	while(1) {

	    FD_ZERO(&mon_socks);
	    FD_SET(listenfd,&mon_socks);

        pthread_mutex_lock(&connect_mutex);
        for (listnum = 0; listnum < MAX_PEERS; listnum++) {
			if (peer_list[listnum] > -1) {
				FD_SET(peer_list[listnum],&mon_socks);
				if (peer_list[listnum] > maxsock)
					maxsock = peer_list[listnum];
			}
		}
		pthread_mutex_unlock(&connect_mutex);

        /*call select with 10 sec timeout interval*/
		servTimeout.tv_sec = 5;
		servTimeout.tv_usec = 0;
        
		

		socks_read = select(maxsock+1, &mon_socks, NULL, NULL, &servTimeout);
       // printf ("\nsocks_read = %d", socks_read);

		/*if (socks_read < 0) {
			fprintf(stderr, "\nError in selecting connections\n");
			perror("select");
			exit(1);
		}*/

		if (socks_read == 0) {
			/* print "S" to show server is alive */
			//printf(".");
			fflush(stdout);
		}
		else if (socks_read > 0) { //activity on some socket

			if (FD_ISSET(listenfd,&mon_socks)) { /*new connection*/

				peer_fd = accept(listenfd,(struct sockaddr *)&peerAddress,&sin_size);
		        
                printf("\nAccepted a connection on socket %d\n", peer_fd);
				
				//get processID based on peer's IP address
                if( (peer_id = getNodeID(peerAddress.sin_addr)) == -1 ) {
                   printf("\nPeer not recognized\n");
				   continue;
				}
                printf("Peer_ID is :%d\n", peer_id);
                
                //safely store peer_fd to peer_list
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
             
				//print_peer_list ();
			}
			else {  //data recieved on some socket
              //  pthread_mutex_lock(&connect_mutex);
				for (listnum = 0; listnum < MAX_PEERS; listnum++) {
					if (peer_list[listnum] < 0) continue;
					if (FD_ISSET(peer_list[listnum],&mon_socks)) {
                        
						//printf("\nData Received on connected_sock[%d] = %d", listnum, peer_list[listnum]);

						//recv msg
						mrecv_data = (char *)calloc(512,sizeof(char));
				
				        recv_num = recv(peer_list[listnum], mrecv_data, (512*sizeof(char)), 0);
                        if (recv_num == 0) {
                          // fprintf(stderr, "\nError in recieving message\n");
			         	 //  perror("recv");
				           continue;
						}
						else if (recv_num > 0) { 
                         
							  printf("\nReceived a Message from Node [%d] on socket %d", listnum, peer_list[listnum]);
                              printf("\nmessage contains:\n%s", mrecv_data);
                              
							  //if responses sent quickly at the other end
							  //receive buffers the data, so
							  //extract single response messages and addto_inMsgQ
							  if(mrecv_data[0] == '2')
							     process_recvData (mrecv_data); 
							  else  
							  //add received message to Q
							    addto_inMsgQ (mrecv_data);

						}
					 
						free(mrecv_data);
						
					}


				} //end listnum
                
				
              //  pthread_mutex_unlock(&connect_mutex);
			} //end data recv
		
		} //end socket activity

	} //end while


}

void process_recvData (char *data) {

	char *singleMsg;
    int i, k;
    int len = strlen(data)+1;

	singleMsg = (char *)calloc(len, sizeof(char));

	bzero (singleMsg, len);
    
	i = 0;
	while (data[i] != '\0') {  //extract single msg
   
	  k = 0;
	  while (data[i] != '/') {
		  singleMsg[k++] = data[i++];
	  }
	  
	  while (data[i] != '#') {
		  singleMsg[k++] = data[i++];
	  }
	  
	  singleMsg[k] = data[i];
	  i++;
	  
	  strcat(singleMsg, "\0");
      
	  printf("\nSingleMsg: %s", singleMsg);

	  //add received message to Q
      addto_inMsgQ (singleMsg);

      bzero (singleMsg, len);
	}

	free(singleMsg);

}



void *handle_inComing_Msg (void *arg) {

  int thread_id = *((int*)arg);
  char *msgData;

  //printf("\nEntering inMsg Handler....");

  pthread_mutex_lock(&inMsg_mutex); //lock mutex before cond_wait

  while (1) {
      
      if (num_inMsg > 0) {

         msgData = getMsg_inMsgQ ();

		 if (msgData == NULL)
			 continue;
     	 else
           printf("\nService Thread %d servicing the following msg from in_MsgQ:\n%s\n", thread_id, msgData);
		 
         //check the msg type
		switch (msgData[0]) {

		     case '1': printf("\nReceived a Search Request Message");
				       handle_request_message(msgData);
					   break;

			 case '2': printf("\nReceived a Search Response Message");
                       handle_response_message(msgData);
				       break;

			 case '3': printf("\nReceived a Withdraw Message");
				       handle_withdraw_message(msgData);
				       break;

			 case '4': printf("\nReceived a Withdraw OK Message");
			           handle_withdrawOK_message(msgData);
			           break;

			default: printf("\nUnknown Message Type");
		}
		free(msgData); 
		 
		 
	  } 
	  else {   //wait for msgs to arrive
		        // printf("\ncond_waiting");
				 //pthread_mutex_lock(&inMsg_mutex);
                 pthread_cond_wait(&got_inMsg_cond, &inMsg_mutex);
            
	  }



  }

}

void handle_request_message(char *msg) {

  struct Message *reqMessage;
  int dup = 0;
  char *req_buf;
  int i = 0, k = 0, j = 0, len = 0;
  char src[3] = {'\0'};
  int hpcnt = 0;
  char **found, fwd_src[3] = {'\0'};
  int source;
  

  reqMessage = decode_Msg(msg);
  
  /*printf("\nReqMessage Structure contains:");
  printf("\nreqMsg.type: %c", reqMessage->type);
  printf("\nreqMsg.hop_count: %s", reqMessage->hop_count);
  printf("\nreqMsg.record_route: %s", reqMessage->record_route);
  printf("\nreqMsg.sequence_num: %s", reqMessage->sequence_num);
  printf("\nreqMsg.data: %s", reqMessage->data);*/

  dup = search_requestQ (*reqMessage);
  
  if (dup) {
	printf("\nDiscarding Duplicate Request!!!");
	return;
  }

  //prepare requestQ entry of the form "src-seq_num-hop_count-"
  while (reqMessage->record_route[i] != ',')
		src[k++] = reqMessage->record_route[i++];
  src[k] = '\0';

  len += strlen(src);
  len += strlen(reqMessage->hop_count);
  len += strlen(reqMessage->sequence_num);

  

  req_buf = (char *)calloc(len+5, sizeof(char));
  
  strcpy(req_buf, src);
  strcat(req_buf, "-");
  strcat(req_buf, reqMessage->sequence_num);
  strcat(req_buf, "-");
  strcat(req_buf, reqMessage->hop_count);
  strcat(req_buf, "-");

  //add to your received requestQ
  addto_requestQ (req_buf);

  hpcnt = atoi(reqMessage->hop_count);
  hpcnt--;
  itoa (hpcnt, reqMessage->hop_count); //copy decremented hop count to the message structure

  //extract peer from whom you received the message (last entry)
 	
	i = strlen(reqMessage->record_route) - 2;
	while (reqMessage->record_route[i] != ',') {
		i--;
		if(i == -1) break;
	}
	j = 0;
	i++;
	while (reqMessage->record_route[i] != ',')
		fwd_src[j++] = reqMessage->record_route[i++];
    fwd_src[j] = '\0';
   // printf("\nForwarding node is %s", fwd_src);
	
	source = atoi(fwd_src);
	printf("\nForwarding node is %d", source);
	


  if (hpcnt > 0) { //foward the request to your peers

	  forward_request_message(*reqMessage, source);

  }

  //search for file locally and send response if present
  found = search_localSharedfiles (reqMessage->data);
  i = 0;
  if (strcmp(found[i],"\0") != 0) { 
      printf("\nCalling build_send_response_message");
      build_send_response_message(found, reqMessage->record_route, reqMessage->sequence_num, source, hpcnt+1);
  }

  free (reqMessage);

}

void build_send_response_message(char **data, char *route, char *seq, int src, int hop) {

	char *resp_buf;
    int length = 0;
	char hpstr[4] = {'\0'};
    char self_str[3] = {'\0'};
	int i = 0;
	 
	itoa(self_ID, self_str);
    itoa(hop, hpstr);

	length++;
	length+=strlen(hpstr);
	length+=strlen(seq);
	length+=strlen(route);
	
	while (strcmp(data[i], "\0") != 0) {
       length+=strlen(data[i]);
	   i++;
	}

	resp_buf = (char *)calloc(length+(10*MAX_DUPLICATE), sizeof(char));

	resp_buf[0] = '2';

	strcat(resp_buf, "#");
	strcat(resp_buf, hpstr);
	strcat(resp_buf, "#");
	strcat(resp_buf, route);
	strcat(resp_buf, "#");
	strcat(resp_buf, seq);
	strcat(resp_buf, "#");
	i = 0;
	while (strcmp(data[i], "\0") != 0) {
	   strcat(resp_buf, data[i]);
	   strcat(resp_buf, "/");
	   strcat(resp_buf, self_str);
	   strcat(resp_buf, "$");
	   i++;
	}
	
	strcat(resp_buf, "#");
	length = strlen(resp_buf);
	printf("\nResponse buffer contains: %s", resp_buf);

	printf("\nSending Response Message to Peer...");

    pthread_mutex_lock(&connect_mutex);
	     if (send(peer_list[src], resp_buf, length, 0) < 0) {
	         fprintf(stderr, "\nError in sending Response Message\n");
             perror("send");
		 }
		 else
            printf("\nSent Response to Peer[%d]\n", src);
	
	pthread_mutex_unlock(&connect_mutex);

    free(resp_buf);


}


void forward_request_message(struct Message fwdMsg, int src) {
 

	char *req_buffer;
	int length = 0, i, j;
   
    char self_str[3] = {'\0'};
	
	
 
	itoa(self_ID, self_str);

	
	//append your id to record route
	strcat(fwdMsg.record_route, self_str);
	strcat(fwdMsg.record_route, ",");

	//populate the buffer
    length++;
	length+=strlen(fwdMsg.hop_count);
	length+=strlen(fwdMsg.record_route);
    length+=strlen(fwdMsg.sequence_num);
	length+=strlen(fwdMsg.data);

	req_buffer = (char *)calloc(length+10, sizeof(char));
 
	req_buffer[0] = fwdMsg.type;
	strcat(req_buffer, "#");
	strcat(req_buffer, fwdMsg.hop_count);
	strcat(req_buffer, "#");
	strcat(req_buffer, fwdMsg.record_route);
	strcat(req_buffer, "#");
	strcat(req_buffer, fwdMsg.sequence_num);
	strcat(req_buffer, "#");
	strcat(req_buffer, fwdMsg.data);
	strcat(req_buffer, "#");
	
	length = strlen(req_buffer);
	printf("\nRequest buffer contains: %s", req_buffer);

	printf("\nFowarding Request Message to Peers...\n");

    pthread_mutex_lock(&connect_mutex);
	for (i = 0; i < MAX_PEERS; i++) {
      if(i == src) continue; //dont send msg back to the node you received from
	  if (peer_list[i] > -1) {  
	     if (send(peer_list[i], req_buffer, length, 0) < 0) {
	         fprintf(stderr, "\nError in fowarding Request Message\n");
             perror("send");
		 }
		 else
            printf("Forwarded Request to Peer[%d]\n", i);
	  }

	}
	pthread_mutex_unlock(&connect_mutex);


	free(req_buffer);

}


struct Message *decode_Msg(char *mdata) {

	struct Message *reqMsg;
    int i = 0, k, j, count = 0;

	reqMsg = (struct Message*)malloc(sizeof(struct Message));

	printf("\nDecode data contains: %s", mdata);
	while (mdata[i] != '\0') {
     reqMsg->type = mdata[i];  //extract message type
	 //printf("\nreqMsg.type: %c", reqMsg->type);
	 
     k = 0;
	 i+=2;
	 while (mdata[i] != '#') {   //extract hop_count
		reqMsg->hop_count[k] = mdata[i];
		k++; i++;
	 }
     reqMsg->hop_count[k] = '\0';
	 //printf("\nreqMsg.hop_count: %s", reqMsg->hop_count);
	      
	 k = 0;i++;
	 while (mdata[i] != '#') { //extract record route
		reqMsg->record_route[k] = mdata[i];
		k++; i++;
	 }
	 reqMsg->record_route[k] = '\0';
     //printf("\nreqMsg.record_route: %s", reqMsg->record_route);
	 
	 k = 0;i++;
	 while (mdata[i] != '#') { //extract seq num
		reqMsg->sequence_num[k] = mdata[i];
		k++; i++;
	 }
     reqMsg->sequence_num[k] = '\0';
     //printf("\nreqMsg.sequence_num: %s", reqMsg->sequence_num);
	 
	 k = 0;i++;j = i;
	 while (mdata[i++] != '#')count++; //get length of data field
     reqMsg->data = (char *)calloc(count+1, sizeof(char));

	 while (mdata[j] != '#') { //extract data
		reqMsg->data[k] = mdata[j];
		k++; j++;
	 }
     reqMsg->data[k] = '\0';
	 //printf("\nreqMsg.data: %s", reqMsg->data);
	 
	 break;
	}

  
  
	return reqMsg;


}

int search_requestQ (struct Message req) {
    
	struct recvRequest_Q *temp;
	char src[3] = {'\0'};
	int i = 0, k = 0;

	while (req.record_route[i] != ',')
		src[k++] = req.record_route[i++];
	src[k] = '\0';
    printf("\nExtracted source node is %s", src);

    pthread_mutex_lock(&recvReq_mutex);
	temp = recvReqHead;
	if (temp == NULL) {
		pthread_mutex_unlock(&recvReq_mutex);
		return 0;
	}

    while ( temp != NULL) {

		if((strcasecmp(src, temp->org_source)==0) && (strcasecmp(req.sequence_num, temp->org_seqnum)==0) && (strcasecmp(req.hop_count, temp->org_hopcnt)==0)) {
			pthread_mutex_unlock(&recvReq_mutex);
			return 1;
		}
	  temp = temp->next;
	}
	pthread_mutex_unlock(&recvReq_mutex);
    return 0;

}



void addto_requestQ (char *val) { //insert front Qs

	struct recvRequest_Q *q_element, *temp;
	int i = 0, k = 0;

	q_element = (struct recvRequest_Q*)malloc(sizeof(struct recvRequest_Q));
    
	printf("\nCreating a Request_Q element..");
	pthread_mutex_lock(&recvReq_mutex);

    while (val[i] != '\0') {
		
		while (val[i] != '-') {
           q_element->org_source[k] = val[i];
		   i++;k++;
		}
	    q_element->org_source[k] = '\0';
        k = 0; i++;
        
		while (val[i] != '-') {
           q_element->org_seqnum[k] = val[i];
		   i++;k++;
		}
	    q_element->org_seqnum[k] = '\0';
        k = 0; i++;

		while (val[i] != '-') {
           q_element->org_hopcnt[k] = val[i];
		   i++;k++;
		}
	    q_element->org_hopcnt[k] = '\0';
      
		break;
	  
	}

	if (recvReqHead == NULL) {
       recvReqHead = q_element;
       recvReqTail = q_element; 
	   q_element->next = NULL;
	}
	else {
          
		 q_element->next = recvReqHead;
		 recvReqHead = q_element;
	}
 
	num_recvReq++;

    //display the Q
  /*  printf("\nRequestQ contains:\n");
	for (temp = recvReqHead; temp != NULL; temp = temp->next ) {
		printf("q_element->org_source %s ", temp->org_source);
		printf("q_element->org_seqnum %s ", temp->org_seqnum);
		printf("q_element->org_hopcnt %s ", temp->org_hopcnt);
		printf("\n");
	}*/
 
	free(val);
	pthread_mutex_unlock(&recvReq_mutex);

	

}

void handle_response_message(char *msg) {

  struct Message *respMessage = NULL;
  int route_list[MAX_PEERS];
  int i, j, k;
  char id[3] = {'\0'};
  int src;
  int hp;

  //initialize
  for (i = 0; i < MAX_PEERS; i++)
	  route_list[i] = -1;

  //decode the message
  respMessage = decode_Msg(msg);
  
  
  /*printf("\nRespMessage Structure contains:");
  printf("\nrespMessage.type: %c", respMessage->type);
  printf("\nrespMessage.hop_count: %s", respMessage->hop_count);
  printf("\nrespMessage.record_route: %s", respMessage->record_route);
  printf("\nrespMessage.sequence_num: %s", respMessage->sequence_num);
  printf("\nrespMessage.data: %s", respMessage->data);*/
  
  

  
  //fill up the route list
  i = 0; j = 0; k = 0;
  while (respMessage->record_route[i] != '\0') {
	  k = 0;
	  while (respMessage->record_route[i] != ',')
           id[k++] = respMessage->record_route[i++];
	  id[k] = '\0';
	  i++;
	  //printf("\nid is %s", id);
	  route_list[j] = atoi(id);
	  j++;

  }

  //print route list
  for (i = 0; i < MAX_PEERS; i++) {
	  if(route_list[i] == -1)break;
	  printf("\nRoute_list[%d]: %d", i, route_list[i]);
  }

  if (route_list[0] == self_ID) {  //response to your request

	  printf("\nReceived a response for my request!!");
	  consume_response(*respMessage);

  } 
  else {  //foward the response towards the source

	  //increment hop count before forwarding
      hp = atoi(respMessage->hop_count);
      hp++;
      itoa(hp, respMessage->hop_count);

	  i = 0;
	  while (route_list[i] > -1) i++;

	  //delete your entry from list
	  route_list[i-1] = -1;
	  src = route_list[i-2];

	  //fill up the new record route
      //strcpy(respMessage->record_route, "\0");
	  bzero(respMessage->record_route, 3*MAX_PEERS);
	  bzero(id, 3);

	  
      k = 0;
	  while (route_list[k] > -1) {
		  itoa(route_list[k], id);
		  strcat(respMessage->record_route, id);
		  strcat(respMessage->record_route, ",");
		  k++;
	  }
      printf("\nNew Record Route is: %s", respMessage->record_route);
      
      forward_response_message(*respMessage, src);

  }


  free(respMessage);

}


void consume_response(struct Message resp) {

	char *cur_time;
    time_t timer;
    struct recvResponse_Q *temp;
	int snum;

	timer = time(NULL);
    cur_time = asctime(localtime(&timer));
    
    //update log file
	printf("\nReceived a response at time: %s", cur_time);
	pthread_mutex_lock(&logfile_mutex);
	if ((log_fp = fopen(logfileName, "a+")) == NULL) {
       printf("\nError in opening log file\n");
	   return;
	}
	fprintf(log_fp, "\nReceived a response\nDetails:\n");
	fprintf(log_fp, "Search Result: %s\n", resp.data);
	fprintf(log_fp, "Hop Count Used: %s\n", resp.hop_count);
    fprintf(log_fp, "Time of Receipt: %s\n", cur_time);
	fclose(log_fp);
	pthread_mutex_unlock(&logfile_mutex);

    //update response_Q
	snum = atoi(resp.sequence_num);
   
	pthread_mutex_lock(&recvResp_mutex);

	
    for (temp = recvRespHead; temp != NULL; temp = temp->next) {
	
		if (snum == temp->sqnum) break; 
	
	}
	
	//printf("\nRespData = %s", resp.data);
	if (strstr(temp->searchData, resp.data) == NULL)
	   strcat(temp->searchData, resp.data); 

	//printf("\nConsumed Data is: %s", temp->searchData);
    pthread_mutex_unlock(&recvResp_mutex);


}



void forward_response_message(struct Message fwdMsg, int src) {
 

	char *resp_buffer;
	int length = 0;
   	
	//populate the buffer
    length++;
	length+=strlen(fwdMsg.hop_count);
	length+=strlen(fwdMsg.record_route);
    length+=strlen(fwdMsg.sequence_num);
	length+=strlen(fwdMsg.data);

	resp_buffer = (char *)calloc(length+10, sizeof(char));
 
	resp_buffer[0] = fwdMsg.type;
	strcat(resp_buffer, "#");
	strcat(resp_buffer, fwdMsg.hop_count);
	strcat(resp_buffer, "#");
	strcat(resp_buffer, fwdMsg.record_route);
	strcat(resp_buffer, "#");
	strcat(resp_buffer, fwdMsg.sequence_num);
	strcat(resp_buffer, "#");
	strcat(resp_buffer, fwdMsg.data);
	strcat(resp_buffer, "#");
	
	length = strlen(resp_buffer);
	printf("\nResponse buffer contains: %s", resp_buffer);

	printf("\nFowarding Response Message towards the source...");

    pthread_mutex_lock(&connect_mutex);
	
	
	     if (send(peer_list[src], resp_buffer, length, 0) < 0) {
	         fprintf(stderr, "\nError in fowarding Response Message\n");
             perror("send");
		 }
		 else
            printf("\nForwarded Response Message to Peer[%d]", src);
	
	pthread_mutex_unlock(&connect_mutex);

    free(resp_buffer);
	

}


char *getMsg_inMsgQ () {

	struct inMessage_Q *temp;
    char *qdata;
	
	if (inMsgHead == NULL) {
        qdata = NULL;
		return qdata;
	}
	temp = inMsgHead;
    inMsgHead = inMsgHead->next;
    qdata = (char *)calloc(strlen(temp->in_data)+1, sizeof(char));
	strcpy(qdata, temp->in_data);
	//printf("\nqdata: %s\n", qdata);
	free(temp);
    num_inMsg--; //decrement count of inMsgs
	pthread_mutex_unlock(&inMsg_mutex); //need to unlock the mutex automatically locked when the signal is received
	return qdata;
   

}


void addto_inMsgQ (char *data) {  //insert rear Qs

	struct inMessage_Q *q_element, *temp;
	q_element = (struct inMessage_Q*)malloc(sizeof(struct inMessage_Q));
    
	printf("\nAdding received message to in_MsgQueue..");
	pthread_mutex_lock(&inMsg_mutex);

    q_element->in_data = (char *)calloc(strlen(data)+1, sizeof(char));
	strcpy(q_element->in_data, data);
    		
	if (inMsgHead == NULL) {
       inMsgHead = q_element;
       inMsgTail = q_element; 
	   q_element->next = NULL;
	}
	else {
         inMsgTail->next = q_element; 
		 q_element->next = NULL;
         inMsgTail = q_element; 
		 
	}
 
	num_inMsg++; //increment count of inMsgs
    
    
	//display the Q
    printf("\nIn_msgQ contains:");
	for (temp = inMsgHead; temp != NULL; temp = temp->next ) {
		printf("\n%s", temp->in_data);
	}
	
	

	//signal the worker threads that a new msg has arrived
	pthread_cond_signal(&got_inMsg_cond);

	pthread_mutex_unlock(&inMsg_mutex);

	

}

int getNodeID (struct in_addr peerAddr) {

 struct hostent *peer_info;
 int i;

 
 if ((peer_info = gethostbyaddr((const char *)&peerAddr, sizeof(peerAddr), AF_INET)) == NULL) { 
    fprintf(stderr, "\nError in getting IP address for peer\n");
    perror("gethostbyaddr");
 }

 printf("Peer name is: %s\n", peer_info->h_name);

 for (i = 0; i < MAX_PEERS; i++) {

 if ( strstr(peer_info->h_name, peerAddrList[i] ) != NULL) 
   return i;
 else
  continue;
 
 }

 return -1;

}

int is_Connected (int seekvalue) {

	pthread_mutex_lock(&connect_mutex);
	if( peer_list[seekvalue] > -1 )
	{
		pthread_mutex_unlock(&connect_mutex);
		return 1;
	}
	else
	{
		pthread_mutex_unlock(&connect_mutex);
		return 0;
	}


}

void set_Connected (int seekvalue, int pfd) {

	pthread_mutex_lock(&connect_mutex);
	peer_list[seekvalue] = pfd;
	pthread_mutex_unlock(&connect_mutex);

}


void *file_transferThread (void *arg) {

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
   int recv_num;
   int fileServ_thr_id;
   pthread_t  fileServ_thread;
   char *msgData;
   int file_peer_list[MAX_PEERS];

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
   servAddress.sin_port = htons(FILE_TRANSFER_PORT);

   memset(&(servAddress.sin_zero), '\0', 8);

   /* binding the socket to the service port */
   if (bind(listenfd, (struct sockaddr*) &servAddress, sizeof(servAddress)) < 0) {
     fprintf(stderr, "\nError in binding the socket to the port\n");
	 perror("bind");
     exit(1);
   }


   /* listen on the port*/
    if (listen(listenfd, MAX_PEERS) < 0) {
      fprintf(stderr, "\nError in listening on the port\n");
	  perror("listen");
      exit(1);
	}

	//initialize the peer list 
    for (i = 0; i < MAX_PEERS; i++ ) 
	 file_peer_list[i] = -1; 
		
    printf("\nCreating file transfer service thread...");
	pthread_create(&fileServ_thread, NULL, handle_file_service, NULL);

    maxsock = listenfd;

	while(1) {

	    FD_ZERO(&mon_socks);
	    FD_SET(listenfd,&mon_socks);

        
        for (listnum = 0; listnum < MAX_PEERS; listnum++) {
			if (file_peer_list[listnum] > -1) {
				FD_SET(file_peer_list[listnum],&mon_socks);
				if (file_peer_list[listnum] > maxsock)
					maxsock = file_peer_list[listnum];
			}
		}
		

        /*call select with 10 sec timeout interval*/
		servTimeout.tv_sec = 5;
		servTimeout.tv_usec = 0;
        
		

		socks_read = select(maxsock+1, &mon_socks, NULL, NULL, &servTimeout);
       // printf ("\nsocks_read = %d", socks_read);

		/*if (socks_read < 0) {
			fprintf(stderr, "\nError in selecting connections\n");
			perror("select");
			exit(1);
		}*/

		if (socks_read == 0) {
			/* print "S" to show server is alive */
			//printf(".");
			fflush(stdout);
		}
		else if (socks_read > 0) { //activity on some socket

			if (FD_ISSET(listenfd,&mon_socks)) { /*new connection*/

				peer_fd = accept(listenfd,(struct sockaddr *)&peerAddress,&sin_size);
		        
                printf("\nAccepted file transfer connection on socket %d\n", peer_fd);
				
				//get processID based on peer's IP address
                if( (peer_id = getNodeID(peerAddress.sin_addr)) == -1 ) {
                   printf("\nPeer not recognized\n");
				   continue;
				}
                printf("Peer_ID is :%d\n", peer_id);
                
				file_peer_list[peer_id] = peer_fd;

				if (send(peer_fd, "A", 10, 0) < 0) {
	                   fprintf(stderr, "\nError in sending Accept Message\n");
                       perror("send");
				}
				                
			}
			else {  //data recieved on some socket
              
				for (listnum = 0; listnum < MAX_PEERS; listnum++) {
					if (file_peer_list[listnum] < 0) continue;
					if (FD_ISSET(file_peer_list[listnum],&mon_socks)) {
                        
						//printf("\nData Received on connected_sock[%d] = %d", listnum, file_peer_list[listnum]);

						//recv msg
						mrecv_data = (char *)calloc(64,sizeof(char));
				
				        recv_num = recv(file_peer_list[listnum], mrecv_data, (64*sizeof(char)), 0);

                        if (recv_num == 0) {
                          // fprintf(stderr, "\nError in recieving message\n");
			         	  // perror("recv");
				           continue;
						}
						else if (recv_num > 0) { 
                         
							  printf("\nReceived a Message from Node [%d] on socket %d", listnum, file_peer_list[listnum]);
                              printf("\nmessage contains:\n%s", mrecv_data);
                              
							  //add received message to Q
							  addto_fileServiceQ (mrecv_data, file_peer_list[listnum]);

							  //deactivating the socket
							  file_peer_list[listnum] = -1;
                          

						}
					 
						free(mrecv_data);
					}


				} //end listnum
                
				
              
			} //end data recv
		
		} //end socket activity

	} //end while

    


}

char *getReq_fileServiceQ (void) {

	struct fileService_Q *temp;
    char *qdata;
	char sockstr[5] = {'\0'};
	
	if (fileServHead == NULL) {
        qdata = NULL;
		return qdata;
	}
	temp = fileServHead;
    fileServHead = fileServHead->next;
    qdata = (char *)calloc(35, sizeof(char));
	itoa(temp->sock, sockstr);
	strcpy(qdata, sockstr);
	strcat(qdata, "-");
	strcat(qdata, temp->filename);
	//printf("\nfile qdata: %s\n", qdata);
	free(temp);
    file_servNum--; //decrement count of fileServMsgs
	pthread_mutex_unlock(&fileService_mutex); //need to unlock the mutex automatically locked when the signal is received
	return qdata;


}



void addto_fileServiceQ (char *fname, int sockfd) {

	struct fileService_Q *q_element, *temp;
	q_element = (struct fileService_Q*)malloc(sizeof(struct fileService_Q));
    
	printf("\nAdding received file transfer request to fileService_Q..");
	pthread_mutex_lock(&fileService_mutex);

    q_element->sock = sockfd;
	strcpy(q_element->filename, fname);
	
    		
	if (fileServHead == NULL) {
       fileServHead = q_element;
       fileServTail = q_element; 
	   q_element->next = NULL;
	}
	else {
         fileServTail->next = q_element; 
		 q_element->next = NULL;
         fileServTail = q_element; 
		 
	}
 
	file_servNum++; //increment count of fileServMsgs
    
    
	//display the Q
    printf("\nFileService_Q contains:");
	for (temp = fileServHead; temp != NULL; temp = temp->next ) {
		printf("\n Filename: %s Scoket: %d", temp->filename, temp->sock);
	}
	
	

	//signal the worker threads that a new msg has arrived
	pthread_cond_signal(&got_fileService_cond);

	pthread_mutex_unlock(&fileService_mutex);


}

void *handle_file_service (void *arg) {

  char *msgData, i, k;
  int sock;
  char fname[25] = {'\0'}, sockstr[5] = {'\0'}; 

  //printf("\nEntering file service Handler....");

  pthread_mutex_lock(&fileService_mutex); //lock mutex before cond_wait

  while (1) {
      
      if (file_servNum > 0) {

         msgData = getReq_fileServiceQ ();

		 if (msgData == NULL)
			 continue;
     	 else
           printf("\nService Thread servicing the following file trasfer request: %s", msgData);

		i = 0; k= 0;
		while (msgData[i] != '-') 
			sockstr[k++] = msgData[i++];
		sockstr[k] = '\0';
		i++; k =0;
		while (msgData[i] != '\0')
			fname[k++] = msgData[i++];
		fname[k] = '\0';

		sock = atoi(sockstr);

        send_file(fname, sock);
	    
		free(msgData); 
       		 
		 
	  } 
	  else {   //wait for msgs to arrive
		        // printf("\ncond_waiting");
				 //pthread_mutex_lock(&fileService_mutex);
                 pthread_cond_wait(&got_fileService_cond, &fileService_mutex);
            
	  }



  }



}


void *connect_user_interfaceThread (void *arg) {

	struct timeval cliTimeout;
    int random_peer;
    int maxin_socks, insocks_read;
    fd_set in_socks;
    char in_buf[50] = {'\0'};
	pthread_t search_reuest_thread_id;
	char *file_idnty;
	char **found;
	char selfnodestr[3] = {'\0'};
	int ret;
	int i, space;


	random_peer = select_random_peer();
	printf("\nRandom Peer = %d", random_peer); 
	
	if (random_peer > -1) {
       ret = connect_toPeer (random_peer, SERVER_PORT, 1);
	   if (ret == -1)
		   printf("\nConnection request rejeted!!");
	}

	//print_peer_list ();
    printf("\n=======================================================\n");
    printf("Operations Available on this Peer-to-Peer System are:\n");
	printf("*********All commands should be in lower case**********\n");
	printf("=======================================================\n");
	printf("Search <filename/keyword>: for a file using filename or a keyword\n");
	printf("History: display search history\n");
    printf("Clear: search history\n");
	printf("Withdraw: the node from the peer to peer system\n");
	printf("Download <filename> <Node ID>: Download file named <filename> from node <Node ID>\n");
	printf("Peers: Show your peers in the P-2-P system\n");
    printf("Help: Reprints this menu\n");
	printf("=======================================================\n");
	
	cliTimeout.tv_sec = 5;
	cliTimeout.tv_usec = 0;
	maxin_socks = fileno(stdin);

	while (1) {

		space = 0;
		i = 0;

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
		 //printf(".");
		 fflush(stdout);
		}
		else if (FD_ISSET(fileno(stdin),&in_socks)) { //if some activity on socket/console
            
			

            if (read(1, in_buf, 50) < 0)
     		  printf("\nError in select/reading user input!!");
	    	
							
			if (strstr(in_buf,"search") != NULL) { 

				//command integrity check
				i = 0;
                while (in_buf[i] != '\n') {
					if (in_buf[i] == ' ')
						space++;
                i++;
				}
				if (space < 1) {
					printf("\nCommand in not proper format!!!Type 'help' for more info!!!\n");
					continue;

				}

				printf ("\nStarting Search for the file...\n");
				req_seqnum++; 
			    file_idnty = extract_fileID(in_buf); //get filename or keyword
				printf ("\nFile name/keyword is: %s", file_idnty);
		        found = search_localSharedfiles (file_idnty); //search locally
				
                addto_respQ (file_idnty);  //create resposeQ node for this request
                
				
				pthread_mutex_lock(&recvResp_mutex);
                i = 0;
                while (strcmp(found[i], "\0") != 0) { //file exists locally, populate the responseQ element
					//printf("\nfound %s", found[i]);
					itoa(self_ID, selfnodestr);
	                strcat(recvRespHead->searchData, found[i]);
					strcat(recvRespHead->searchData, "/");
                    strcat(recvRespHead->searchData, selfnodestr);
					strcat(recvRespHead->searchData, "$");
				    i++;	
				}
				pthread_mutex_unlock(&recvResp_mutex);
			   //file doesnt exist, send out search request
			    printf("\nCreating Send Search Request Thread\n");
			    if (pthread_create(&search_reuest_thread_id, NULL, search_requestThread, (void *)file_idnty) < 0) {
	                      printf("\nProblem in search_reuestThread creation \n");
				}
			
			  	
			}
			else if (strstr(in_buf,"history") != NULL) {
				//command integrity check
				i = 0;
                while (in_buf[i] != '\n') {
					if (in_buf[i] == ' ')
						space++;
                i++;
				}
				if (space > 0) {
					printf("\nCommand in not proper format!!!Type 'help' for more info!!!\n");
					continue;

				}

				 printf ("\nThe Search history on this system is...\n");
				 
				 handle_display_search_history();

			}
			else if (strstr(in_buf,"clear") != NULL) { 
				//command integrity check
				i = 0;
                while (in_buf[i] != '\n') {
					if (in_buf[i] == ' ')
						space++;
                i++;
				}
				if (space > 0) {
					printf("\nCommand in not proper format!!!Type 'help' for more info!!!\n");
					continue;

				}
								
				handle_clear_search_history();
				printf ("\nSearch history Cleared!!!\n");

			}
			else if (strstr(in_buf,"withdraw") != NULL) { 
				//command integrity check
				i = 0;
                while (in_buf[i] != '\n') {
					if (in_buf[i] == ' ')
						space++;
                i++;
				}
				if (space > 0) {
					printf("\nCommand in not proper format!!!Type 'help' for more info!!!\n");
					continue;

				}
				printf ("\nStarting withdrawal of the  node...\n");
				
				handle_node_withdraw();

			}
			else if (strstr(in_buf,"download") != NULL) { 
				//command integrity check
				i = 0;
                while (in_buf[i] != '\n') {
					if (in_buf[i] == ' ')
						space++;
                i++;
				}
				if (space != 2) {
					printf("\nCommand in not proper format!!!Type 'help' for more info!!!\n");
					continue;

				}

				printf ("\nEntering file download...\n");
				
				handle_download(in_buf);

			}
			else if (strstr(in_buf,"help") != NULL) { 
                printf("\n=======================================================\n");
				printf("Operations Available on this Peer-to-Peer System are:\n");
				printf("*********All commands should be in lower case**********\n");
				printf("=======================================================\n");
				printf("Search <filename/keyword>: for a file using filename or a keyword\n");
				printf("History: display search history\n");
				printf("Clear: search history\n");
				printf("Withdraw: the node from the peer to peer system\n");
				printf("Download <filename> <Node ID>: Download file named <filename> from node <Node ID>\n");
				printf("Peers: Show your peers in the P-2-P system\n");
				printf("Help: Reprints this menu\n");
				printf("=======================================================\n");

			}
			else if (strstr(in_buf,"peers") != NULL) { 

				printf("\nNode[%d]'s peers in the P-2-P system are:\n", self_ID);
				printf("==========================================\n");
                pthread_mutex_lock(&connect_mutex);
				for (i = 0; i < MAX_PEERS; i++) {
				  if (peer_list[i] > -1) {  
	                printf("\n Node [%d] (%s)", i, peerAddrList[i]);
				  }
				}
                printf("\n");
			    pthread_mutex_unlock(&connect_mutex);
				
				
			}
			else {

				//printf("\nInvalid Command!!!\n");
				printf("\nKey Pressed!!!!\n" );
			}
            
			//strcpy(in_buf, "\0");
			bzero(in_buf, 50);
		 	fflush(stdin);
		}
	}

}

void handle_download (char *buf) {

	char file[25] = {'\0'}, idstr[3] = {'\0'};
	int id;
	int i, k;
    char dec;
    struct recvResponse_Q *temp, *found;
    char *foundData;

	//extract filename and node id from buf
	i = 0;
    while (buf[i] != '\0') { 
     
	   while (buf[i++] != ' ');
       
	   k = 0;
	   while (buf[i] != ' ')
          file[k++] = buf[i++];
	   file[k] = '\0';
	   
	   i++;
	   k = 0;
	   while (buf[i] != '\n')
          idstr[k++] = buf[i++];
	   idstr[k] = '\0';
	   
	   break;

	}
	
	id = atoi(idstr);

    printf("\nDo you want to download the file concurrently from all available locations(y/n):");
	scanf("%c", &dec);
   
	if (tolower(dec) == 'y') {
       
	 pthread_mutex_lock(&recvResp_mutex);
     temp = recvRespHead;
     while(temp != NULL) {
       if (strstr(temp->searchData, file) != NULL) {
        found = temp; 
	    break;
	   }
	         
      temp = temp->next;
	 }
     

     foundData = (char *)calloc(strlen(found->searchData)+1, sizeof(char));
     strcpy(foundData, found->searchData);
	 
	 pthread_mutex_unlock(&recvResp_mutex);

	 handle_concurrentDownloads(file, foundData);
	}
	else
	  download_file (file, id, -1, -1); //3rd arg -1 to show non-concurrent call
	                                    //4th arg -1 to show to download from single node

    free(foundData);
}


char **search_localSharedfiles (char *value) {

  char **str, line[255], kwd[25];
  int i, k, cnt = 0;
  char pathname[50] = {'\0'};
  char filestr[55] = {'\0'};

  str = (char **)calloc(MAX_DUPLICATE, sizeof(char *));
  for (i = 0; i < MAX_DUPLICATE; i++) {
      str[i] = (char *)calloc(55, sizeof(char));
	  bzero(str[i],55);
  }


  

  pthread_mutex_lock(&shared_fp_mutex);
  
  strcpy(pathname, sharedDirName);
  strcat(pathname, "/");
  strcat(pathname, sharedfileName);

  //printf("\nPathname is %s", pathname);

  //open the shared files list
  if ((shared_fp = fopen(pathname, "a+")) == NULL) {
     printf("\nError in opening shared files list\n");
	 fclose(shared_fp);
	 pthread_mutex_unlock(&shared_fp_mutex);
	 for (i = 0; i < MAX_DUPLICATE; i++)
       free(str[i]);
	 free(str);
	 return NULL;
   }
   
  if (fgets(line, 255, shared_fp) == NULL) {  //shared files list empty
     printf("\nShared files list is empty\n");  
  	 fclose(shared_fp);
	 pthread_mutex_unlock(&shared_fp_mutex);
	 for (i = 0; i < MAX_DUPLICATE; i++)
       free(str[i]);
	 free(str);
	 return NULL;
  }
  else {
        rewind(shared_fp);
		cnt = 0;
	    while (!feof(shared_fp)) {

		   if (cnt == MAX_DUPLICATE) break;
		   
           if (fgets(line, 255, shared_fp) == NULL) {
			  // printf("gotNULL");
			   break;
		   }
		   
		   if (strcmp(line, "\n") == 0) continue; 
		   

		   i = 0;
		  // printf("\nline %s", line);	     
		   while (line[i] != '-') {
			     filestr[i++] = line[i++];
		   }
		   filestr[i] = '\0';
		 //  printf("filestr %s length is %d value %s length is %d", filestr, strlen(filestr), value, strlen(value));	
		   if (strcasecmp(filestr, value) == 0) {
			  strcpy(str[cnt], filestr);
			  fclose(shared_fp);
			  pthread_mutex_unlock(&shared_fp_mutex);
    		  return str;
			  
			}
            else {
               i++;
			   while (line[i] != '\n') {
				   k = 0;
				   while (line[i] != ',')  
			           kwd[k++] = line[i++];
	   
		           kwd[k] = '\0';
				   i++;
                 //  printf("\nkwd %s\n", kwd);
				   
				   if (strcasecmp(kwd, value) == 0) {
					 strcpy(str[cnt], filestr);
                     strcat(str[cnt], "-");
					 strcat(str[cnt], kwd);
				//	 printf("\nstr[%d] = %s", cnt, str[cnt]);
					 cnt++;
					 bzero(line, 255);
		             bzero(filestr, 55);
		             bzero(kwd, 25);
					 break;
				   }

			   }

		   }
		  
		    
		}
		fclose(shared_fp);
	    pthread_mutex_unlock(&shared_fp_mutex);
		/*printf("\nSTR contains");
		i = 0;
		while(strcmp(str[i], "\0") != 0) {
          printf("\n%s", str[i]);
		  i++;
		}*/
	    return str;
  }
		 
  


}

char *extract_fileID (char *buf) {
    
	char *fnameID;
	int i = 0, k;

    fnameID = (char *)calloc(25, sizeof(char));
    
	while (buf[i++] != ' ');
	k=0;
	while (buf[i] != '\n') 
		fnameID[k++] = buf[i++];
	
	fnameID[k] = '\0';
     
	//printf("\nfnameID contains: %s", fnameID);
	return fnameID;
	
}

void addto_respQ (char *val) { //insert front Qs

	struct recvResponse_Q *q_element;
	q_element = (struct recvResponse_Q*)malloc(sizeof(struct recvResponse_Q));
    
	printf("\nCreating a responseQ element..");
	pthread_mutex_lock(&recvResp_mutex);

    q_element->identifier = (char *)calloc(strlen(val)+1, sizeof(char));
	strcpy(q_element->identifier, val);
    q_element->sqnum = req_seqnum;
	//strcpy(q_element->searchData, "\0");
    bzero(q_element->searchData, 512);
		
	if (recvRespHead == NULL) {
       recvRespHead = q_element;
       recvRespTail = q_element; 
	   q_element->next = NULL;
	}
	else {
          
		 q_element->next = recvRespHead;
		 recvRespHead = q_element;
	}
 
	//printf("\nq_element->identifier: %s", recvRespTail->identifier);
    //printf("\nq_element->sqnum: %d", recvRespTail->sqnum);

	pthread_mutex_unlock(&recvResp_mutex);

	

}


int connect_toPeer (int peer, int port, int first) {

	int connect_flag = 0, sockfd;
    struct sockaddr_in cliAddress;
    struct hostent *peer_info;
    int i; 
    char perm[10];
	int recv_perm;

	

	 	  cliAddress.sin_family = AF_INET;
		  cliAddress.sin_port = htons((unsigned)port);
		  memset(&(cliAddress.sin_zero), '\0', 8);

		  if ((peer_info=gethostbyname(peerAddrList[peer])) == NULL) { 
            fprintf(stderr, "\nCould not resolve the Hostname\n");
		    perror("gethostbyname");
          }
	      cliAddress.sin_addr = *((struct in_addr *)peer_info->h_addr);
 
		  //printf("\nIP address of - %s - is %s\n", peerAddrList[peer], inet_ntoa(cliAddress.sin_addr));
         
          
		  if (inet_pton(AF_INET, inet_ntoa(cliAddress.sin_addr), &cliAddress.sin_addr) <= 0) {
            fprintf(stderr, "\nError in connecting using hostname\n");
			perror("inet_pton");
		  
		  }

		  
		  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		     fprintf(stderr, "\nClient:Error in creating socket\n");
		     perror("socket");
		  
		  }
		  

		 /* connecting to the server */
		 if (connect(sockfd, (struct sockaddr *)&cliAddress, sizeof(cliAddress)) < 0) {
		   fprintf(stderr, "\nError in connecting to the  server\n");
		   perror("connect");
		  
		 }
         
		 recv_perm = recv(sockfd, perm, 10, 0);
         if (recv_perm == 0) {
             fprintf(stderr, "\nError in recieving connection status message\n");
		     perror("recv");
		  
		 }
		 else if (recv_perm > 0) { 

			 if(strcmp(perm, "A") == 0) {
                if (first) //new connection
		          set_Connected (peer, sockfd);
			 printf ("\nConnected Successfully to Node [%d] on socket %d", peer, sockfd);
			 return sockfd;

			 }
			 else if(strcmp(perm, "D") == 0) {
                    close(sockfd);
					return -1;
			 }
         
		 }
		 
	


}

int select_random_peer () {

  FILE *peer_fp;
  char nodestr[3] = {'\0'}, selfnodestr[3] = {'\0'};
  int line = 0;
  int ran_val = 0;
  int cnt = 0;

  if ((peer_fp = fopen(peerInfofileName, "r")) == NULL) {
    printf("\nError in opening active nodes file\n");
	return -1;
  }
  
  if (fgets(nodestr, 3, peer_fp) == NULL) {  //peer nodes empty
     printf("\nPeer nodes empty\n");  
     if ((peer_fp = freopen(peerInfofileName, "a", peer_fp)) == NULL) {
        printf("\nError in reopening peer nodes file\n");
	 } 
     
	 //write your ID to peer nodes file
	 itoa(self_ID, selfnodestr);
	 strcat(selfnodestr, "\n");
     if (fputs(selfnodestr, peer_fp) < 0) {
         printf("\nError in writing to peer node file\n");
	 }
     fclose(peer_fp);
     return -1;
  }
  else {
        rewind(peer_fp);  
	    while (!feof(peer_fp)) {
           bzero(nodestr, 3);
		   if (fgets(nodestr, 3, peer_fp) == NULL)break;
		  // printf("\nNodestr is %s", nodestr);
		   if (strcmp(nodestr, "\n")== 0) 
			   continue;
		   else
		     line++;
		   		   
		}
		 
  }
 // printf("\nLine count = %d", line);
  
  if (line > 1) {
    while (ran_val == 0)
      ran_val = rand() % line;
  }
  else
    ran_val = line;

 // printf("\nRan_val = %d", ran_val);
  rewind(peer_fp);  
  while (cnt < ran_val ) {
	 bzero(nodestr, 3);
	
	 if (fgets(nodestr, 3, peer_fp) == NULL) {
      printf("\nError in reading peer nodes file\n");
	 }
	 if (strcmp(nodestr, "\n")== 0) 
		 continue;
	 else
	    cnt++;
	

  }
  fclose(peer_fp);
  
  //write your ID to peer nodes file
  if ((peer_fp = fopen(peerInfofileName, "a")) == NULL) {
    printf("\nError in opening peer nodes file\n");
	return -1;
  }
  else {
	     itoa(self_ID, selfnodestr);
		 strcat(selfnodestr, "\n");
         if (fputs(selfnodestr, peer_fp) < 0) {
               printf("\nError in writing to peer node file\n");
		 }
         fclose(peer_fp);
  }
  //printf("\nNodestr contains %s", nodestr);

  return atoi(nodestr);
 

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

void *req_cleanupThread (void *arg) {

    struct recvRequest_Q *temp;
	while (1) {

		sleep(REQUEST_CLEANUP_TIMEOUT);
        printf("\nREQUEST_CLEANUP_TIMER expired!!!!\n");

		pthread_mutex_lock(&recvReq_mutex);
		printf("\nReceived Requests Count: %d", num_recvReq);		        
        
		printf("\nBefore Cleanup RequestQ contains:\n");
		for (temp = recvReqHead; temp != NULL; temp = temp->next ) {
		    printf("temp->org_source %s ", temp->org_source);
		    printf("temp->org_seqnum %s ", temp->org_seqnum);
		    printf("temp->org_hopcnt %s ", temp->org_hopcnt);
		    printf("\n");
		}

		if (num_recvReq > REQUEST_CLEANUP_THRESHOLD) { //remove one node at the tail

		   /*printf("\nTail element contains:\n");
		   temp = recvReqTail;
		   printf("temp->org_source %s ", temp->org_source);
		   printf("temp->org_seqnum %s ", temp->org_seqnum);
		   printf("temp->org_hopcnt %s ", temp->org_hopcnt);*/

		   for (temp = recvReqHead; temp->next != recvReqTail; temp = temp->next);
           temp->next = NULL;
		   free(recvReqTail);
		   recvReqTail = temp;
           num_recvReq--;
		   printf("\nPurged an Old Request!!!!");
		   
		   printf("\nAfter Cleanup RequestQ contains:\n");
		   for (temp = recvReqHead; temp != NULL; temp = temp->next ) {
			  printf("temp->org_source %s ", temp->org_source);
		 	  printf("temp->org_seqnum %s ", temp->org_seqnum);
			  printf("temp->org_hopcnt %s ", temp->org_hopcnt);
			  printf("\n");
		   }
		   
           
		}
		else
           printf("\nNumber of Requests is within the REQUEST_CLEANUP_THRESHOLD!!!\n");

		pthread_mutex_unlock(&recvReq_mutex);
	}

}

void print_peer_list () {
  
  int i;
  
  pthread_mutex_lock(&connect_mutex);
  printf("\nPeer list for node[%d] contains:\n", self_ID);
  for (i = 0; i < MAX_PEERS; i++)
     printf("Peer[%d] = %d, ", i, peer_list[i]);
  printf("\n");
  pthread_mutex_unlock(&connect_mutex);
 
}

int peer_count () {
  
  int i, count = 0;
  
  pthread_mutex_lock(&connect_mutex);
  
  for (i = 0; i < MAX_PEERS; i++)
     if (peer_list[i] > -1)
		 count++;
  pthread_mutex_unlock(&connect_mutex);
  return count;
 
}

void *search_requestThread (void *arg)  {
  
   int hop_count = 1, timeout, cur_sqnum, notempty_flag = 0; 
   char *fname = (char *)arg;
   struct recvResponse_Q *temp, *found;
   int dest[MAX_PEERS], i, k, j;
   char id_str[3] = {'\0'};
   int choice, valid_choice = -1;
   char *cur_time, *foundData;
   time_t timer;
   char file_name[25] = {'\0'};
   int responses = 0;

   cur_sqnum = req_seqnum;
   printf("\nItem to search for is %s", fname);

   //initialize to -1
   for (i = 0; i < MAX_PEERS; i++)
	   dest[i] = -1;

   //find the responseQ element
   pthread_mutex_lock(&recvResp_mutex);
   temp = recvRespHead;
   while(temp != NULL) {
     if ((strcasecmp(temp->identifier, fname)==0) && (temp->sqnum == cur_sqnum)) {
        found = temp; 
		//printf("\nfound->identifier: %s found->sqnum: %d", found->identifier, found->sqnum);
	    break;
	 }
	         
   temp = temp->next;
   }
   pthread_mutex_unlock(&recvResp_mutex);

   while (hop_count <= MAX_PEERS) { //send/retry request

      timeout = hop_count * HOP_TIMER;
	  build_send_request_message(fname, hop_count, cur_sqnum);
	  
	  if (hop_count == 1) { //write system time to logfile
     
		timer = time(NULL);
        cur_time = asctime(localtime(&timer));
        printf("\nSent Request Packet at time: %s", cur_time);
		pthread_mutex_lock(&logfile_mutex);
		if ((log_fp = fopen(logfileName, "a+")) == NULL) {
          printf("\nError in opening log file\n");
		  pthread_mutex_unlock(&logfile_mutex);
		  return;
		}
		fprintf(log_fp, "\nSent search request for item '%s' at time: %s\n", fname, cur_time);
		fclose(log_fp);
		pthread_mutex_unlock(&logfile_mutex);


	  }

      
	  printf("\nWaiting...");
	  printf("\nTimeout Value is %d secs", timeout);
      sleep(timeout); //wait
	  
      pthread_mutex_lock(&recvResp_mutex);
	  printf("\nCurrent Hop count is: %d", hop_count);
	  if (strcasecmp(found->searchData, "\0") != 0) {    //response is present
		  printf("\nfound->searchData contains: %s", found->searchData);
		  notempty_flag = 1;
          responses = response_count (found->searchData);
		  printf("\nResponse Count is: %d", responses);
		  pthread_mutex_unlock(&recvResp_mutex);
		  if (responses == MAX_RESPONSE_COUNT)
		      break;
	  }
	  
      hop_count++;
	  pthread_mutex_unlock(&recvResp_mutex);
		  
	
   }

   if (!notempty_flag) {  //no response after max hop_count
	   printf("\nMaximum Retry limit Reached. Item '%s' not found in the P-2-P system!!!\n", fname);
	     
	   pthread_mutex_lock(&logfile_mutex);
	   if ((log_fp = fopen(logfileName, "a+")) == NULL) {
          printf("\nError in opening log file\n");
 	      return;
	   }
	   fprintf(log_fp, "Maximum Retry limit Reached. Item '%s' not found in the P-2-P system!!!\n", fname);
	   fclose(log_fp);
	   pthread_mutex_unlock(&logfile_mutex);

	   pthread_exit(NULL);
   }
   else { //response is present

	   printf("\nSearch for item '%s' found the following matches:", fname);
	   printf("\nMatched file<-keyword>/Node ID");
	   printf("\n=============================\n");
	   i = 0; j = 0;
	   pthread_mutex_lock(&recvResp_mutex);
       
	   foundData = (char *)calloc(strlen(found->searchData)+1, sizeof(char));
       strcpy(foundData, found->searchData);

	   while (found->searchData[i] != '\0') {
		   while (found->searchData[i] != '/') {
                printf("%c", found->searchData[i]);
				i++;
		   }
		   printf("%c", found->searchData[i]);
           k = 0;
		   i++;
           while (found->searchData[i] != '$') {
			   id_str[k] = found->searchData[i];
			   printf("%c", found->searchData[i]);
			   i++;k++;
		   }
		   id_str[k] = '\0';
		  // printf("id_str is %s", id_str);
		   dest[j] = atoi(id_str);
		   j++;i++;
		   printf("\n");
	   }
	   //extract file name from (filename keyword) string
       i = j = k = 0;
	   while (found->searchData[i] != '/') {
		   if (found->searchData[i] == ' ') break;
		   file_name[k] = found->searchData[i];
		   k++;i++;
	   }
       file_name[k] = '\0';

	   pthread_mutex_unlock(&recvResp_mutex);
	   
	   /*while (valid_choice == -1) {
	      printf("\nEnter the node ID from which you wish to download (-1 to cancel download):");
		  scanf("%d", &choice);
		  fflush(stdin);
		  if (choice == -1) break;
		  for (i = 0; i < MAX_PEERS; i++) {
			  if (dest[i] == choice) {
				  valid_choice = choice;
				  break;
			  }
		  }
			  
	   }*/
       bzero(file_name, 25);
	   printf("\nEnter the name of the file you wish to download ('x' to cancel , 'c' for concurrent downloads):");
	   scanf("%s", file_name);
	   if (strcasecmp(file_name, "c") == 0) 
          choice = -2;
	   else if (strcasecmp(file_name, "x") == 0) 
               choice = -1;
	   else {
	       printf("\nEnter the node id you wish to download from :");
	       scanf("%d", &choice);
	   }

	   if(choice == -1) {
           printf("\nCancelling the Download!!!\n");
		   printf("\nUse 'History' and 'Download' command to download file later!!!\n");
		   pthread_exit(NULL);
	   }
	   else if (choice > -1) {  //download the file from Peer[valid_choice]
		   //printf("\nDownloading file from Node [%d]", valid_choice);
		   download_file (file_name, choice, -1, -1); //3rd arg -1 to show non-concurrent call
                                                     //4th arg -1 to show to download from single node

	   } //end download
	   else if (choice == -2) {
		   //concurrent downloads
		   handle_concurrentDownloads(NULL, foundData);
           free(foundData);
	   } 


   }//end response present


}

void handle_concurrentDownloads (char *fname, char *foundData) {


    char name[25] = {'\0'};
    int nodeIDs[MAX_PEERS];
    int i = 0, ret;
    pthread_t  *download_threads;// thread's structures for download threads
    char nodestr[4] = {'\0'};
    char n_str[4] = {'\0'};
	void *exitStatus;
	struct download_threadData *thread_data;
    char cmd[256]  = {'\0'}, idx_str[4] = {'\0'};
	char dirname[25] = {'\0'};

	for (i = 0; i < MAX_PEERS; i++)
		nodeIDs[i] = -1;
 
	if (fname == NULL) {
	  printf("\nEnter the file Name:");
	  scanf("%s", name);
	}
	else
	 strcpy(name, fname);

	ret = extract_nodeIDs(nodeIDs, name, foundData);
    if (ret == 0) {

	  printf("\nFile not found in search results!!!Use 'History' to view search results!!!");
	  return;
	}
  
	printf("\nFile '%s' is found in the following locations:", name);
	
    for (i = 0; i < MAX_PEERS; i++) {
		if(nodeIDs[i] > -1)
		  printf("\nNode [%d]", nodeIDs[i]);
	}

	//create threads for concurrent downloads
    download_threads = (pthread_t *)calloc(ret,sizeof(pthread_t));
	thread_data = (struct download_threadData *)calloc(ret,sizeof(struct download_threadData));
	
	for (i = 0; i < ret; i++) {
	   thread_data[i].index = i;
	   thread_data[i].node = nodeIDs[i];
	   thread_data[i].count = ret;
	   strcpy(thread_data[i].file, name);
	   printf("\nThread_data contains:");
	   printf("\nFile: %s", thread_data[i].file);
	   printf("\nIndex: %d", thread_data[i].index);
	   printf("\nNode: %d", thread_data[i].node);
	   printf("\nCreating Download thread %d\n", i);
	   pthread_create(&download_threads[i], NULL, concurrentDownload_thread, (void*)&thread_data[i]);
	   pthread_join(download_threads[i],exitStatus);
    }

	
    //join peices of temp files into singe file
	//cat name_temp_0 name_temp_1 >name
	//rm name_temp_0 , rm name_temp_1

	pthread_mutex_lock(&shared_fp_mutex);
    strcpy(dirname, sharedDirName);
    strcat(dirname, "/");
	pthread_mutex_unlock(&shared_fp_mutex);

	strcpy(cmd, "cat ");
	for (i = 0; i < ret; i++) {
      strcat(cmd,dirname);
	  strcat(cmd, name);
	  strcat(cmd, "_temp_");
	  itoa(i, idx_str);
      strcat(cmd, idx_str);
	  strcat(cmd, " ");

	}
	strcat(cmd, ">");
	strcat(cmd,dirname);
	strcat(cmd, name);
	//printf("\nCmd is: %s", cmd);
	system(cmd);

	//remove temp files
	bzero(cmd, 256);
	strcpy(cmd, "rm ");
	strcat(cmd,dirname);
	strcat(cmd, name);
	strcat(cmd, "_temp_*");
	system(cmd);
	
	free(thread_data);


}

void *concurrentDownload_thread (void *arg) {

	struct download_threadData *local_data;
	
	local_data = (struct download_threadData *)arg;

    printf("\nIndex is %d Node is %d File is %s Node Count is %d", local_data->index, local_data->node, local_data->file, local_data->count);
	
	download_file (local_data->file, local_data->node, local_data->index, local_data->count);

   
}




/*int extract_nodeIDs(int *fnodes, char *fname, char *fdata) {


	int i = 0, j, k;
    char key[50] = {'\0'}, node[4] = {'\0'};
    int cnt = 0;

	while (fdata[i] != '\0') {
   
	   k = 0;
	   while (fdata[i] != '/') {
		 key[k++] = fdata[i++];
	   }
	   key[k] = '\0';
     
       if (strstr(key, fname) != NULL) {
          i++;
		  j = 0;
		  while (fdata[i] != '$') {
             node[j++] = fdata[i++];
		  }
		  node[j] = '\0';
		  fnodes[cnt] = atoi(node);
          i++;
		  cnt++;
	   }
	   else {
	 
		  while (fdata[i++] != '$');
       	  i++;

	   }
		 

	}

   return cnt;

}*/

int extract_nodeIDs(int *fnodes, char *fname, char *fdata) {


	int i = 0, j, k;
    char key[50] = {'\0'}, node[4] = {'\0'};
    int cnt = 0, hyphen;

	while (fdata[i] != '\0') {
   
	   k = 0;
	   while ((fdata[i] != '/') && (fdata[i] != '-')) {
		 key[k++] = fdata[i++];
	   }
	   key[k] = '\0';

	   printf("\nkey is %s", key);
	   

       if (fdata[i] == '-') 
		  hyphen = 1;
	   else
          hyphen = 0;

	   printf("\nhyphen = %d", hyphen);
	   if (strcasecmp(key, fname) == 0) {
          
		  if (hyphen)
            while (fdata[i++] != '/'); 
          else
			i++;
		  
		  j = 0;

		  while (fdata[i] != '$') {
             node[j++] = fdata[i++];
		  }
		  node[j] = '\0';
		  printf("\nNode is %s", node);
		  fnodes[cnt] = atoi(node);
          i++;
		  cnt++;
	   }
	   else {
	 
		  while (fdata[i++] != '$');
       	  i++;

	   }
		 
       bzero (key, 50);
	}

   return cnt;

}

int response_count (char *resp_data) {

   int count = 0, i = 0;
   
   while (resp_data[i++] != '\0') {
	   if(resp_data[i] == '/')
		   count++;
   }
   
   return count;
}

void send_file(char *fname, int fd) {

  char pathname[50] = {'\0'}, line[255] = {'\0'};
  FILE *fp;
  struct stat file_stats;
  char fstr[10] = {'\0'};
  int size, i;
  int size_flag =0;
  int recv_perm;
  int start = 0, bytes = 0;
  int buf_size = 64, loop =0;
  int rem = 0;
  
  //sending the shared_file entry
  pthread_mutex_lock(&shared_fp_mutex);
  strcpy(pathname, sharedDirName);
  strcat(pathname, "/");
  strcat(pathname, sharedfileName);  

  //open the shared files list
  if ((shared_fp = fopen(pathname, "r")) == NULL) {
     printf("\nError in opening shared files list\n");
	 return;
    
  }

  printf("\nShared file Pathname is %s", pathname);

  while (!feof(shared_fp)) {

	  bzero(line, 255);
	  fgets(line, 255, shared_fp);

	  if(strstr(line, fname)!= NULL)
		  break;

  }

  fclose(shared_fp);
  pthread_mutex_unlock(&shared_fp_mutex);

 //send shared file entry
  if (send(fd, line, strlen(line), 0) < 0) {
	 fprintf(stderr, "\nError in sending Shared file Entry!!!\n");
     perror("send");
  }
  else
    printf("\nSent shared file entry : %s", line);

  //receive size query
  bzero(line, 255);
  
  recv_perm = recv(fd, line, 255, 0);
  if (strcasecmp(line, "size")==0) {
	  size_flag = 1;
	  printf("\nReceived size request!!");
  }

  bzero(pathname, 50);

  pthread_mutex_lock(&shared_fp_mutex);
  strcpy(pathname, sharedDirName);
  strcat(pathname, "/");
  strcat(pathname, fname);
  pthread_mutex_unlock(&shared_fp_mutex);

  printf("\nSending file Pathname is %s", pathname);

  //get file size
  if(stat(pathname, &file_stats) != 0){
    perror("stat");
  }

  size = file_stats.st_size;
  printf("\nFile size is: %d\n", size);
  
  itoa(size, fstr);
  

  //send file size
  if (send(fd, fstr, strlen(fstr), 0) < 0) {
	 fprintf(stderr, "\nError in sending file segment!!!\n");
     perror("send");
  }
   else
     printf("Sent: %s\n", fstr);

 

  //recv start postion
  bzero(line, 255);
  recv_perm = recv(fd, line, 6, 0);
  start = atoi(line);
  
  printf("\nStart Postion is %d ", start);

  //send ok
  if (send(fd, "ok", 2, 0) < 0) {
	 fprintf(stderr, "\nError in sending OK!!!\n");
     perror("send");
  }

  //recv byte count
  bzero(line, 255);
  recv_perm = recv(fd, line, 6, 0);
  bytes = atoi(line);

  printf("\nByte Count is %d ", bytes);
  
  bzero(line, 255);

  
  //start sending the file
  if ((fp = fopen(pathname, "r")) == NULL) {
     printf("\nError in opening send file\n");
	 return;
    
  }
  
  

  //seek to start position
  fseek(fp, start, SEEK_CUR);

  if (size_flag) {
   
    loop = bytes / buf_size;
	rem = bytes % buf_size;
    i = 0;
    while (i < loop) {
	  
	  fread(line, buf_size, 1, fp);
      
	  if (send(fd, line, strlen(line), 0) < 0) {
	      fprintf(stderr, "\nError in sending file segment!!!\n");
          perror("send");
	  }
	  else
	    printf("Sent: %s", line);
       
	  bzero(line, 255);
	  i++;
	}
   
	
	if (rem > 0) {
      //read any remaining bytes
      fread(line, rem, 1, fp);
      
      if (send(fd, line, strlen(line), 0) < 0) {
         fprintf(stderr, "\nError in sending file segment!!!\n");
         perror("send");
	  }
      else
        printf("Sent: %s", line);
      
     bzero(line, 255);
   
	}

    fclose(fp);

  }
  
  
  printf("\nFile Transfer Completed!!Closing the connection!!!\n");
  close(fd); //close the connection

	
}


void download_file (char *fname, int node, int index, int node_count) {

	
	int sockfd;
    char pathname[100] = {'\0'}, line[255] = {'\0'};
    FILE *fp; 
    int recv_perm;
    int i, k;
	char *temp;
	char *str;
	int len = 0, fsize;
	int end = 0;
	int count, ok = 0;
	char temp_file[50] = {'\0'}, idx_str[4] = {'\0'};
    int numbyte, start_pos;
    char numbyte_str[6] = {'\0'}, start_pos_str[6] = {'\0'};

	printf ("\n Beginning to download file '%s' from Node [%d]", fname, node);
		
	//connect to destination node and download file
	sockfd = connect_toPeer (node, FILE_TRANSFER_PORT, 0);
	if (sockfd == -1)
		  printf("\nConnection request rejeted!!");
    
	//send file name
    if (send(sockfd, fname, strlen(fname), 0) < 0) {
	   fprintf(stderr, "\nError in sending file name\n");
       perror("send");
	}
	else 
	  printf("\nSent file name %s on socket %d", fname, sockfd);

	//receive shared file entry
	recv_perm = recv(sockfd, line, 255, 0);
    
	//write the shared file only once, -1 for single download and thread with 0th index
	if (index == -1 || index == 0) {
		pthread_mutex_lock(&shared_fp_mutex);
		strcpy(pathname, sharedDirName);
		strcat(pathname, "/");
		strcat(pathname, sharedfileName);  
    
		//open the shared files list
		if ((shared_fp = fopen(pathname, "a")) == NULL) {
		  printf("\nError in opening shared files list\n");
		  pthread_mutex_unlock(&shared_fp_mutex);
	      return;
		}
		if (fputs(line, shared_fp) < 0) {
          printf("\nError in writing to shared file!!\n");
		}
		fclose(shared_fp);
		pthread_mutex_unlock(&shared_fp_mutex);
	}

   //get file size
   if (send(sockfd, "size", 4, 0) < 0) {
	   fprintf(stderr, "\nError in asking file size\n");
       perror("send");
	}
    bzero(line, 255);
    //receive file size
    recv_perm = recv(sockfd, line, 255, 0);
	fsize = atoi(line);
	
    pthread_mutex_lock(&shared_fp_mutex);
	if (node_count == -1) {  //download from single node

      strcpy(pathname, sharedDirName);
      strcat(pathname, "/");
      strcat(pathname, fname);
	}
	else {  //multinode concurrent download   filename = "name_temp_index"

	  strcpy(pathname, sharedDirName);
      strcat(pathname, "/");
      strcat(pathname, fname);
	  strcat(pathname, "_temp_");
	  itoa(index, idx_str);
      strcat(pathname, idx_str);

	}
    pthread_mutex_unlock(&shared_fp_mutex);

    printf("\nReceiving file Pathname is %s and file size is %d", pathname, fsize);

    if ((fp = fopen(pathname, "a+")) == NULL) {
     printf("\nError in opening receive file\n");
     return;
	}
   
	//calculate offset and start_position
    
	if (node_count == -1) {

	  start_pos = 0;
	  itoa(start_pos, start_pos_str);
	  numbyte = fsize;
	  itoa(numbyte, numbyte_str);
	  printf("\nStart_pos is %d - %s, Numbyte is %d - %s", start_pos, start_pos_str, numbyte, numbyte_str);

	}
	else {

		numbyte = fsize / node_count;
		start_pos = numbyte * index;
		if (index == node_count -1) { //last portion, include remainder

			numbyte += fsize % node_count;
		}
		itoa(start_pos, start_pos_str);
        itoa(numbyte, numbyte_str);
       printf("\nStart_pos is %d - %s, Numbyte is %d - %s", start_pos, start_pos_str, numbyte, numbyte_str);
	}

	//send start_postion
	if (send(sockfd, start_pos_str, 6, 0) < 0) {
	   fprintf(stderr, "\nError in sending start position\n");
       perror("send");
	}
    
    //recv ack - ok
	bzero(line, 255);
    recv_perm = recv(sockfd, line, 2, 0);
    if (strcasecmp(line, "ok") == 0)
		ok = 1;

	//send byte count
    if (ok) {
	  if (send(sockfd, numbyte_str, 6, 0) < 0) {
	    fprintf(stderr, "\nError in sending byte count\n");
        perror("send");
	  }
	}

	count = 0;

	while (count < numbyte) {
        
		bzero(line, 255);
		
		recv_perm = recv(sockfd, line, 255, 0);
        
        if (recv_perm == 0) {
             //fprintf(stderr, "\nError in recieving file\n");
		     //perror("recv");
		  
		 }
		 else if (recv_perm > 0) { 
             
		       if (fwrite(line, recv_perm, 1, fp) < 1) {
                 printf("\nError in writing file segment!!\n");
			   }
			   else
			    count += recv_perm;
		  
		 }

	}
	fclose(fp);
	printf("\nSuccessfully received the file!!!");
	printf("\nClosing the connection!!!\n");
	close(sockfd); //close the connection


}



void build_send_request_message(char *fid, int hpcnt, int cur_seq) {
 
	struct Message curReqMsg;
	char hpcnt_str[4] = {'\0'}, seq_str[5] = {'\0'}, self_str[3] = {'\0'};
    char *req_buffer;
	int length = 0, i;

	curReqMsg.data = (char *)calloc(strlen(fid)+1, sizeof(char));
	
	//populate the request message structure
    curReqMsg.type = '1';
	length++;
	itoa(hpcnt, hpcnt_str);
	strcpy(curReqMsg.hop_count, hpcnt_str);
	length+=strlen(curReqMsg.hop_count);
	itoa(self_ID, self_str);
    strcpy(curReqMsg.record_route, self_str);
	strcat(curReqMsg.record_route, ",");
	length+=strlen(curReqMsg.record_route);
    itoa(cur_seq, seq_str);
	strcpy(curReqMsg.sequence_num, seq_str);
	length+=strlen(curReqMsg.sequence_num);
	strcpy(curReqMsg.data, fid);
	length+=strlen(curReqMsg.data);

	req_buffer = (char *)calloc(length+10, sizeof(char));
 
	req_buffer[0] = curReqMsg.type;
	strcat(req_buffer, "#");
	strcat(req_buffer, curReqMsg.hop_count);
	strcat(req_buffer, "#");
	strcat(req_buffer, curReqMsg.record_route);
	strcat(req_buffer, "#");
	strcat(req_buffer, curReqMsg.sequence_num);
	strcat(req_buffer, "#");
	strcat(req_buffer, curReqMsg.data);
	strcat(req_buffer, "#");
	
	length = strlen(req_buffer);
	printf("\nRequest buffer contains: %s", req_buffer);

	printf("\nSending Request Message to Peers...\n");

    pthread_mutex_lock(&connect_mutex);
	for (i = 0; i < MAX_PEERS; i++) {
	  if (peer_list[i] > -1) {  
	     if (send(peer_list[i], req_buffer, length, 0) < 0) {
	         fprintf(stderr, "\nError in sending Sending Request Message\n");
             perror("send");
		 }
		 else
            printf("Sent Request to Peer[%d]\n", i);
	  }

	}
	pthread_mutex_unlock(&connect_mutex);

    free(curReqMsg.data);
	free(req_buffer);

}

void handle_display_search_history () {

    struct recvResponse_Q *temp;
    char *result;
    int i, k;

	pthread_mutex_lock(&recvResp_mutex);
    temp = recvRespHead;
	if (recvRespTail == NULL) {
       printf("\nSearch History is empty!!!\n");
	   pthread_mutex_unlock(&recvResp_mutex);
	   return;
	 
	}
	
	printf("\nSearch item           Matched file<-keyword>/Node ID");
	printf("\n===================================================");
	
    do {
       printf("\n %s", temp->identifier);
       result = (char *)calloc(strlen(temp->searchData), sizeof(char));
       i = 0;
	   while (temp->searchData[i] != '\0') {
         k = 0;
         while (temp->searchData[i] != '$') {
		   result[k] = temp->searchData[i];
		   i++;k++;
		 }
	     result[k] = '\0';
	     printf("\n                      %s", result);
		 //strcpy(result, "\0");
         bzero(result,strlen(temp->searchData));
		 i++;
	   }
	   temp = temp->next;
	   free(result);

	} while (temp != NULL);

	pthread_mutex_unlock(&recvResp_mutex);
    printf("\n");
}

void handle_clear_search_history () {
	
	struct recvResponse_Q *temp;
	pthread_mutex_lock(&recvResp_mutex);
	for (temp = recvRespHead;temp != NULL; temp = temp->next ) {
		free(temp);
	}
    recvRespHead = NULL;
	recvRespTail = NULL;
	pthread_mutex_unlock(&recvResp_mutex);

}

void handle_node_withdraw ()  {

   int nbr_count = 0;
   
   
   printf("\nFixing Network Disruptions if any.... ");
   nbr_count = peer_count();

   pthread_mutex_lock(&withdraw_mutex);
   withdraw_flag = 1;
   withdrawOK_count = nbr_count;
   pthread_mutex_unlock(&withdraw_mutex);
   

   if (nbr_count == 0) {
	  upate_ActiveNodesFile();
	  printf("\nNode withdrawing from the P-2-P system!!!!");
	  printf("\nGoodBye!!!\n");
	  exit(0);

   }
   else if (nbr_count == 1) { //leaf node, withdraw freely, no fixing required
   
	   build_send_withdraw_message (-1); // -1 to indicate, withdraw msg from leaf node, no need to fix the n/w 
     
   }
   else if (nbr_count > 1) {

	   //pick random neighbor to which others will connect to
	   pthread_mutex_lock(&connect_mutex);
	   while (fixPoint == -1) {
         fixPoint = rand() % MAX_PEERS;
		 if (peer_list[fixPoint] < 0)
           fixPoint = -1;
	   }
	   pthread_mutex_unlock(&connect_mutex);
	   printf("\nChosen Fix point is Node [%d]", fixPoint);
	   build_send_withdraw_message (fixPoint);
 
   }
      
   
 
	
}

void build_send_withdraw_message (int tojoin) {

	struct Message wMsg;
	char self_str[3] = {'\0'}, join_str[3] = {'\0'};
    char *wbuffer;
	int length = 0, i;

	
	itoa(self_ID, self_str);
	
	if (tojoin > -1)
	 itoa(tojoin, join_str);

	length = strlen(self_str)+strlen(join_str);

	wMsg.data = (char *)calloc(length+4, sizeof(char));
	
	//populate withdraw message structure
    wMsg.type = '3';
	length++;
	
	strcpy(wMsg.data, self_str);
	strcat(wMsg.data, ",");
	
	if (tojoin > -1) {
      strcat(wMsg.data, join_str);
	  strcat(wMsg.data, ",");
	}
	

	wbuffer = (char *)calloc(length+10, sizeof(char));
 
	wbuffer[0] = wMsg.type;
	strcat(wbuffer, "#");
	strcat(wbuffer, wMsg.data);
	strcat(wbuffer, "#");
	
	length = strlen(wbuffer);
	printf("\nWithdraw buffer contains: %s", wbuffer);

	printf("\nSending Withdraw Message to Peers...");

    pthread_mutex_lock(&connect_mutex);
	for (i = 0; i < MAX_PEERS; i++) {
	  if (peer_list[i] > -1) { 
		  if (i == self_ID) continue;
	     if (send(peer_list[i], wbuffer, length, 0) < 0) {
	         fprintf(stderr, "\nError in sending withdraw Message\n");
             perror("send");
		 }
		 else
            printf("\nSent Withdraw Message to Peer[%d]", i);
	  }

	}
	pthread_mutex_unlock(&connect_mutex);

    free(wMsg.data);
    free(wbuffer);
	
}



void handle_withdraw_message(char *msg) {

	int ids[2], i;
    int k = 0, cnt = 0;
	char id_str[4] = {'\0'}, self_str[4] = {'\0'};
    int flag, ret;
	char buf[8] = {'\0'};

	ids[0] = ids[1] = -1; //src and node to connect to
	
	//process the message
    i = 2; //read past msg type
	while (msg[i] != '#') {
		k = 0;
		while (msg[i] != ',') {
           id_str[k] = msg[i];
		   k++;i++;
		}
		id_str[k] = '\0';
		ids[cnt] = atoi(id_str);
		cnt++;i++;
	}

	//printf("\nids[0] = %d ids[1] = %d", ids[0], ids[1]);

	/******3 cases *******/

	//1 if you are withdrawing as well, send withdraw OK to ids[0] and fwd the received withdraw msg to your neighbors
    //Rare case: if your withdrawing neighbor has chosen you to be the fix point and u have chosen him to be the same, 
	//then choose a new fix point and send new withdraw message

	pthread_mutex_lock(&withdraw_mutex);
    flag = withdraw_flag;
	pthread_mutex_unlock(&withdraw_mutex);
    
	if ( flag == 1) {

     pthread_mutex_lock(&connect_mutex); 
	      
	 if ((ids[0] == fixPoint) && (ids[1] == self_ID)) { //Rare case: both you and your neighbor are withdrawing concurrently
	   printf("\nChoosing New Fix Point....");          //and both of you have chosen each other as the fixpoint
       //chose new fix point
       fixPoint = -1;
       while (fixPoint == -1) {
         fixPoint = rand() % MAX_PEERS;
		 if ((peer_list[fixPoint] < 0) || (fixPoint == ids[0]))
           fixPoint = -1;
	   }
	   //send withdraw message
       build_send_withdraw_message(fixPoint);

	 }

     //fwd received msg to your neighbors
       
	   for (i = 0; i < MAX_PEERS; i++) {
	     if (peer_list[i] > -1) { 
		   if (i == ids[0]) continue;
	       if (send(peer_list[i], msg, strlen(msg), 0) < 0) {
	         fprintf(stderr, "\nError in forwarding withdraw Message\n");
             perror("send");
		   }
		   else
             printf("\nForwarded Withdraw Message to Peer[%d]", i);
		 }

	   }
	   pthread_mutex_unlock(&connect_mutex);
	
	
	}

	//2 if you are not withdrawing and received withdraw msg from non leaf node, connect to peer ids[1]
	//if not already connected, send withdraw OK to ids[0]

	if (ids[1] > -1) {
	
	  if (!is_Connected(ids[1])) {

         if (ids[1] != self_ID)
		    ret = connect_toPeer (ids[1], SERVER_PORT, 1);
	     
		 if (ret == -1)
		   printf("\nConnection request rejeted!!");

		}

	}
     
	//finally send the withdraw OK message
	//3 if you are not withdrawing and received withdraw from leaf node (ids[1] = -1), just send withdraw OK to ids[0]

    itoa(self_ID, self_str);
	strcpy(buf, "4");
	strcat(buf, "#");
	strcat(buf, self_str);
	strcat(buf, "#");

	if (send(peer_list[ids[0]], buf, strlen(buf), 0) < 0) {   //type to indicate withOK msg
	   fprintf(stderr, "\nError in sending withdraw OK Message\n");
       perror("send");
	}
	else
      printf("\nSent Withdraw OK Message to Peer[%d]", ids[0]);

	//deactivating the socket on which the withdrawing node is connected
	printf("\nClosing connection with Peer[%d]\n", ids[0]);
    pthread_mutex_lock(&connect_mutex);
	close(peer_list[ids[0]]);
	peer_list[ids[0]] = -1;
    pthread_mutex_unlock(&connect_mutex);


}


void handle_withdrawOK_message(char *msg) {

    int i, k;
	char src[4] = {'\0'};
    int s;
    
    //process the message
    i = 2; //read past msg type
	k = 0;
	
	while (msg[i] != '#') {
	    src[k] = msg[i];
	    k++;i++;
	}
	src[k] = '\0';
    s = atoi(src);
    

	printf("\nReceived Withdraw OK message from Peer [%d]", s);
    
	//deactivate the socket
	set_Connected (peer_list[s], -1);
    printf("\nClosing Connection to Peer[%d]", s);
	close(peer_list[s]);
    
	pthread_mutex_lock(&withdraw_mutex);
    withdrawOK_count--;
	if (withdrawOK_count == 0) { //withdraw after receving OKs from all neighbors
       printf("\nNetwork Connectivity Confirmed!!!");
       upate_ActiveNodesFile();
       printf("\nNode withdrawing from the P-2-P system!!!!");
	   printf("\nGoodBye!!!\n");
	   exit(0);
	}
	
    pthread_mutex_unlock(&withdraw_mutex);



}

void upate_ActiveNodesFile() {

	FILE *peer_fp;
    char nodestr[3] = {'\0'};
	int line = 0;
    char linestr[5] = {'\0'};
	int x;
    char cmd[50]  = {'\0'};

	if ((peer_fp = fopen(peerInfofileName, "r")) == NULL) {
           printf("\nError in opening active nodes file\n");
	       return;
	}

	while (!feof(peer_fp)) {
       fgets(nodestr, 3, peer_fp);
	   if (strcmp(nodestr, "\n") == 0 )
          continue;
	   else
	      line++;
       x = strlen(nodestr);
	   nodestr[x] = '\0';
	   //printf("\n nodestr %s line is %d", nodestr, line);
	   if (atoi(nodestr) == self_ID) {
          strcpy(cmd, "sed ");
		  itoa(line, linestr);
		  strcat(cmd, linestr);
		  strcat(cmd, "d ");
		  strcat(cmd, peerInfofileName);
		  strcat(cmd, " >temp");
		  //printf("\n cmd is: %s", cmd);
		  system(cmd);
		  bzero(cmd, 50);
		  strcpy(cmd, "mv ");
		  strcat(cmd, "temp ");
		  strcat(cmd, peerInfofileName);
		  //printf("\n cmd is: %s", cmd);
		  system(cmd);
		  fclose(peer_fp);
		  return;
	   }

	}


}
