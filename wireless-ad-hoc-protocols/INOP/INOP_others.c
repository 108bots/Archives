/*
INOP node Program
 * Author: Hemanth Srinivasan
 * Year: 2006
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
//To prevent problems in global variable updation and access - using semaphore
#include <pthread.h>

#define SERVER_PORT 6868
#define MONITOR_SERVER_PORT 7071



struct Node {

	char name[3]; //name of this node
	char num_Neighbor[4]; //no: of neighbors for this node
	struct nodeList *neighbor_List; //neighbor names and their distances
};

struct nodeList {

	char node_Name[3]; //neighbor name
	char distance[6]; //distance

};

struct Message {

		char message_Type; //H- Hello ,B - Broadcast, U - unused
		char source[3];
		char retransmit_source[3];
		char seq_Num[10];
		char data[256];


};

struct source_sequence {

		char name;
		int sequencenum;
		int sent;

};

struct StatMessage {

		
		char source[3];
		char broadcastCnt[3];
		char receivedCnt[10];
		char consumedPower[10];


};

/*char *ipaddr_list[] = {
	                     "10.110.92.150", //A
					     "10.110.92.151", //B
						 "10.110.92.152", //C
						 "10.110.92.153", //D
						 "10.110.92.154", //E
						 "10.110.92.155", //F
						 "10.110.92.156", //G
						 "10.110.92.157"  //H
                       };
*/

pthread_mutex_t safe_mutex;


char **ipaddr_list;

//char *F_list;
char *R_list;
float *PI_prob;
float *F_List;
int sorted_flag = 0;

float *connect_sortedNDist;
char *connect_sortedNName;

float *inop_sortedNDist;
char *inop_sortedNName;


//measurement variables
int MaxNB, CNB;  //RBT variables
unsigned int seed;
int max_Delay;
int recv_count = 0;
int broadcast_count = 0;
float total_power_consumed = 0;



// Global Variables
int total_Nodes;
float f_Prob; // failure probability
int attenuation;
float max_Tx_Range;
float tx_Power; // r(u) for this node
int count = 0; //number of neighbors

int *hello_status;
struct Node *peer_node;
struct Node cur_node;
int hello_cnt = 0; //total number of hellos recevied
int con_cnt = 0; //total number of connected peers
int peer_num; //number of neighbors


int RBT_Flag = 0;



int *send_list;


//Server variables
struct sockaddr_in serv_addr;
struct timeval timeout;
char opt_val= '1';
int listenfd, peer_fd;
int *connected_list;  /* Array of connected sockets to know who we are talking to */
int maxsock;
fd_set mon_socks;
int loop_flag = 0;
int socks_read, listnum;
struct sockaddr_in PeerAddress;
int sin_size = sizeof(struct sockaddr_in);
char peer_addr[17];
int valid_peer = 0;
struct sigaction sigs;
int recv_num;
char *recv_data;
struct Message recv_message;
struct Message broadcast_queue[5];

float rebroadcast_dist;
 
//Monitor client variables



//Client variables
struct sockaddr_in client_addr;
int *sockfd;
int seek;
int numconnects;
struct sigaction sigc;
int numconnects;
struct Message send_message;
int maxin_socks	; 
fd_set in_socks;
int	insocks_read;
struct timeval intimeout;
float src_broadcast_dist;
int seqCnt=0;





//function declarations
//INOP
float min(float, float);

void parse_input(char []);
void sigchild_handler(int a) {while(wait(NULL) > 0);}
void process_data(void);
void print_message(struct Message);
void hello_packet_processing(int);
void broadcast_packet_processing(void);
void print_node(struct Node);
void sendHello(void);
void get_node_address(int);


float find_v_w_dist(char ,char );
void minDistIndexFunction(void); //sorted ascending
int commonNeighborCheck(char,char);

//Broadcast functionality //sands
void sendBroadcast();
struct Message send_message;


void *serverFunctionThread(void *arg);
void *clientFunctionThread(void *arg);

//general functionalities
void itoa(int , char []);
void find_common_neighbors(float);
void rebroadcast (void);

void send_rebroadcast(void);
int calculate_probability(float);
void compute_consumed_power(float);

void inop_alarm_handler(int);
void inop_algorithm(void);

int main (int argc, char *argv[]) {

 int i, j, k, con_id;
 pid_t pid;
 char self_id[3], in_buf[257];

 pthread_t clientThread_id;
 pthread_t serverThread_id;

 
 void *cexit_status,*sexit_status;
 int clientvalue,servervalue;

 clientvalue = 42;
 servervalue = 54;


 if (argc != 4) {

   printf("\nUsage: rngProtocol <seed> <max_delay> <self ID> \n");
   exit(0);

 }

  strcpy(self_id, argv[3]);
  strcpy(cur_node.name, self_id);

  seed = atoi(argv[1]);
  max_Delay = atoi(argv[2]);
 
 printf("\nReading Input File...\n");
 //function call to parse the input file and load the data structures
 parse_input(self_id);


 printf("\nThe Current Node Data is:");
 printf("\nNode name is %s", cur_node.name);
 printf("\nNumber of peers is %s", cur_node.num_Neighbor);

 for (i = 0; i < count; i++ ) {
   printf("\nNeighbor %s is at a distance of %s", cur_node.neighbor_List[i].node_Name, cur_node.neighbor_List[i].distance);

 }

// get IP address of the nodes
 get_node_address(total_Nodes);

 

 peer_num = atoi(cur_node.num_Neighbor);

 
//allocate memory for current node's neighbors
   peer_node = (struct Node *) malloc (peer_num * sizeof(struct Node));


 // list of connected peers
 connected_list = (int *)malloc(peer_num * sizeof(int));
 for (i = 0; i < peer_num; i++ )
	 connected_list[i] = 0;

//set hello status to 0 for potential neighbors
 hello_status= (int *)malloc(total_Nodes * sizeof(int));
 for (i = 0; i < total_Nodes; i++ )
	 hello_status[i] = 0;

//peer sockets
 sockfd = (int *)malloc(peer_num * sizeof(int));
 for (i = 0; i < peer_num; i++ )
	 sockfd[i] = -1;

//sorted neighbor distance

 connect_sortedNDist = (float *)malloc(peer_num * sizeof(float));
 inop_sortedNDist = (float *)malloc(peer_num * sizeof(float));

 for (i = 0; i < peer_num; i++ ) {

	 connect_sortedNDist[i] = 0;
	 inop_sortedNDist[i] = 0;
 }

 

 send_list= (int *)malloc(peer_num * sizeof(int));

//initialize broadcast_queue
for (i = 0; i < 5; i++)
  broadcast_queue[i].message_Type = 'U'; //un used


//sorted neighbor distance

 connect_sortedNName = (char *)calloc(peer_num , sizeof(char));
 inop_sortedNName = (char *)calloc(peer_num , sizeof(char));

 //F_list = (char *)calloc(peer_num , sizeof(char));

 R_list = (char *)calloc(peer_num , sizeof(char));


 PI_prob = (float *)malloc(peer_num * sizeof(float));
 for (i = 0; i < peer_num; i++ )
	 PI_prob[i] = 0;

 F_List = (float *)malloc(peer_num * sizeof(float));
 for (i = 0; i < peer_num; i++ )
	 F_List[i] = 0;





 pthread_mutex_init(&safe_mutex,NULL);

  // Create the thread , passing &value for the argument.
 if(pthread_create(&clientThread_id,NULL,clientFunctionThread,&clientvalue)<0)
 {
	 printf("\nproblem in thread creation \n");
 }

 if(pthread_create(&serverThread_id,NULL,serverFunctionThread,&servervalue)<0)
 {
	 printf("\nproblem in thread creation \n");
 }

// The main program continues while the thread executes.


 

 pthread_join(clientThread_id,&cexit_status);
 pthread_join(serverThread_id,&sexit_status);

 
 
 return 0;

}//end main




void *clientFunctionThread(void *arg) {
       int j;
       char in_buf[257];
	   int cpeer_num=peer_num;
       	  
	   int i = 0,k ;
	   char sendStat[80] = {'\0'};
	   char recv_cnt[20];
	   char broadcast_cnt[30];
       char total_pwr_cnsd[30];
	   int msockfd;
	   struct sockaddr_in mclient_addr;

	/* child is the CLIENT*/
					sigc.sa_handler = sigchild_handler;
					sigemptyset(&sigc.sa_mask);
					sigc.sa_flags = SA_RESTART;
					 if (sigaction(SIGCHLD, &sigc, NULL) == -1) {
						  fprintf(stderr, "\nError in reaping zombie processes\n");
							perror("sigaction");
							exit(1);
					 }
				//Check for global state for each connections established .
	            //If connection is not established then establish a connection
                    sleep(10);
					while(numconnects!=cpeer_num){

				      for(j=0;j < cpeer_num;j++){

						minDistIndexFunction();

						            

				    	seek = connect_sortedNName[j]-65;

						//printf("\nSeek is %d \n",seek);


						//if(IS_CONNECTED(seek)){
							//printf("coming in after state check\n");
							printf("\nipaddress is : %s\n",ipaddr_list[seek]);

							client_addr.sin_family = AF_INET;
							client_addr.sin_port = htons((unsigned)SERVER_PORT);
							memset(&(client_addr.sin_zero), '\0', 8);

							if (inet_pton(AF_INET, ipaddr_list[seek], &client_addr.sin_addr) <= 0) {
							   fprintf(stderr, "\nError in configuring server address structure IP address\n");
							   perror("inet_pton");
						       continue;
							}

							if(sockfd[j] == -1) {
							  if ((sockfd[j] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
							   fprintf(stderr, "\nClient:Error in creating socket\n");
							   perror("socket");
							   continue;
							  }
							}

						//	printf("created socket %d ",sockfd[j]);

							/* connecting to the server */
				  		    if (connect(sockfd[j], (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
							  fprintf(stderr, "\nError in connecting to the  server\n");
							  perror("connect");
							 continue;
							}

							//SET_CONNECTED(seek);

				//			printf("\nConnected successfully to the server.\n");
							numconnects++;
							//}

					  } //end for


					} //end while
                    
printf("\n\nBuilding the Hello Packet...");
					sendHello();
					intimeout.tv_sec = 10;
					intimeout.tv_usec = 0;
					maxin_socks = fileno(stdin);

					while (1)
					{


						FD_ZERO(&in_socks);
						FD_SET(fileno(stdin),&in_socks);


						/*call select with 10 sec timeout interval*/


						insocks_read = select(maxin_socks+1, &in_socks, NULL, NULL, &intimeout);

						if (insocks_read < 0) {
						fprintf(stderr, "\nError in reading user input\n");
						perror("select");
						exit(1);
						}

						if (insocks_read == 0) {
							/* print "." to show client  is waiting for user input */
							//printf("C");
							fflush(stdout);
						}
						else if (FD_ISSET(fileno(stdin),&in_socks)) {

							//trigger broad cast
                            if (read(1, in_buf, 256) < 0)
							  printf("\nError in select/reading user input!!");
							printf("\nPressed keyboard" );
							//sands
							if(!(strstr(in_buf,"B")==NULL))
							{
								//Sands came here
							   
								//Resetting paramters
								RBT_Flag = 0;
								for(k=0;k<peer_num;k++){
									inop_sortedNName[k]=connect_sortedNName[k];
									inop_sortedNDist[k]=connect_sortedNDist[k];
									send_list[k] = 0;
								}



								seqCnt++;
								sendBroadcast();
								//broadcast_count++; //keep track of no: of broadcasts including rebroadcasts if any
							}
							else if(!(strstr(in_buf,"S")==NULL))	
							{
							 	 
								 //for reference to code
								 //int recv_count = 0;
								 //int broadcast_count = 0;
								 //float total_power_consumed = 0;

								 //Build stat message
								 k=0;
								 itoa(	broadcast_count,broadcast_cnt);
								 itoa(  recv_count,recv_cnt);
								 gcvt (total_power_consumed,5,total_pwr_cnsd);

								 sendStat[k++] = cur_node.name[0];
								 sendStat[k++] = '$';
								 							 
								 strcat(sendStat,broadcast_cnt);
								 strcat(sendStat,"$");
								 
								 strcat(sendStat,recv_cnt);
								 strcat(sendStat,"$");

								 strcat(sendStat,total_pwr_cnsd);
								 strcat(sendStat,"$");
								 
 
								 strcat(sendStat,"\0");							 
								 
								 
								// printf("Send stat message Structure is %s size of %d \n", sendStat, strlen(sendStat));
									
								 mclient_addr.sin_family = AF_INET;
								 mclient_addr.sin_port = htons((unsigned)MONITOR_SERVER_PORT);
								 memset(&(mclient_addr.sin_zero), '\0', 8);

								 if (inet_pton(AF_INET, "10.110.92.140", &mclient_addr.sin_addr) <= 0) {
								   fprintf(stderr, "\nError in configuring server address structure IP address\n");
								   perror("inet_pton");
								   continue;
								 }

								// if(msockfd == -1) {
								  if ((msockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
								   fprintf(stderr, "\nClient:Error in creating socket\n");
								   perror("socket");
								   continue;
								  }
								// }

							//	printf("created socket %d ",sockfd[j]);

								 /* connecting to the server */
				  				 if (connect(msockfd, (struct sockaddr *)&mclient_addr, sizeof(mclient_addr)) < 0) {
								  fprintf(stderr, "\nError in connecting to the  server\n");
								  perror("connect");
								  continue;
								 }
								
								 if (send(msockfd, (const void*) sendStat, strlen(sendStat), 0) < 0) {
								  fprintf(stderr, "\nError in stat send request\n");
								  perror("send");
	
								 }
								
								 bzero(sendStat,80);
								 close(msockfd);

								//resetting counters
								recv_count = 0;
								broadcast_count = 0;
								total_power_consumed = 0;
									
							}

							fflush(stdin);

						}

					}//end while


}



void *serverFunctionThread(void *arg) {
   int i,k,con_id;
   int speer_num= peer_num;
        /* Parent is the server*/
   /*
   sigs.sa_handler = sigchild_handler;
   sigemptyset(&sigs.sa_mask);
   sigs.sa_flags = SA_RESTART;
   if (sigaction(SIGCHLD, &sigs, NULL) == -1) {
    fprintf(stderr, "\nError in reaping zombie processes\n");
	perror("sigaction");
	exit(1);
   }
   */

   /* creating a socket */
   if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
     fprintf(stderr, "\nError in creating socket");
	 perror("socket");
     exit(1);
   }

   /*Prevent the address already in use error*/
   if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(int)) == 1) {
     fprintf(stderr, "\nError in setting socket options of reuse address.\n");
	 perror("setsockopt");
     exit(1);
   }

   /* configuring server address structure */
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
   serv_addr.sin_port = htons(SERVER_PORT);

   memset(&(serv_addr.sin_zero), '\0', 8);

   /* binding the socket to the service port */
   if (bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
     fprintf(stderr, "\nError in binding the socket to the port\n");
	 perror("bind");
     exit(1);
   }


   /* listen on the port*/
    if (listen(listenfd, speer_num) < 0) {
      fprintf(stderr, "\nError in listening on the port\n");
	  perror("listen");
      exit(1);
	}

    maxsock = listenfd;

	while(1) {


	    FD_ZERO(&mon_socks);
	    FD_SET(listenfd,&mon_socks);

        for (listnum = 0; listnum < speer_num; listnum++) {
			if (connected_list[listnum] != 0) {
				FD_SET(connected_list[listnum],&mon_socks);
				if (connected_list[listnum] > maxsock)
					maxsock = connected_list[listnum];
			}
		}

        /*call select with 10 sec timeout interval*/
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;

		socks_read = select(maxsock+1, &mon_socks, NULL, NULL, &timeout);
       // printf("sock_read is %d\n", socks_read);

		if (socks_read < 0) {
			fprintf(stderr, "\nError in selecting connections\n");
			perror("select");
			exit(1);
		}

		if (socks_read == 0) {
			/* print "." to show server is alive */
			//printf("S");
			fflush(stdout);
		}
		else {
			   if (FD_ISSET(listenfd,&mon_socks)) { /*new connection*/

      	         peer_fd = accept(listenfd,(struct sockaddr *)&PeerAddress,&sin_size);
		         //extract ip address of peer
				 strcpy(peer_addr, inet_ntoa(PeerAddress.sin_addr));

			//	 printf("\nPeer IP address on socket %d is %s\n", peer_fd, peer_addr);

				 //find out peer name based on IP address
				 for(i = 0; i < total_Nodes; i++) {
				   if(strcmp(peer_addr, ipaddr_list[i]) == 0) {
          //           printf("\nConnected peer's name is %c\n", i+65);
					 //if(IS_CONNECTED(i)){

					//	SET_CONNECTED(i);

						valid_peer = 1;
					 //}
					 //else
					 //{
					 //	 printf("\nAlready connected with peer %c\n", i+65);
					 //	 close(peer_fd);
					 //	 loop_flag=1;
					 //	 break;
					 //}
				   }
				 }
				 if(loop_flag)
					continue;

				 if(!valid_peer) {
                   printf("\nPeer not recognized!!!Closing the connection!!\n");
				   close(peer_fd);
				   continue;
				 }


				 if (peer_fd < 0) {
				   fprintf(stderr, "\nError in accepting new client\n");
				   perror("accept");
				   exit(1);
				 }

                 for (listnum = 0; listnum < speer_num; listnum++) {
					 if (connected_list[listnum] == 0) {
				        connected_list[listnum] = peer_fd;
						break;
					 }
				 }
              //   	printf("\nConnected to a peer on socket %d\n", connected_list[listnum]);


			   }
			   else {

				   for (listnum = 0; listnum < speer_num; listnum++) {
					  //	printf("\nConnected_list[%d] is %d\n", listnum, connected_list[listnum]);

					 if (connected_list[listnum] != 0) {
				        if (FD_ISSET(connected_list[listnum],&mon_socks)) {
							//check if hello packet or broadcast packet
							//Make sure all hello packets are received and then trigger processing of RNG peers.
                             //check hello_status
							//finish RNG computation wait for user input to initiate broadcast
							//start RBOP alogrithm

						    //check connectionstate and count the number of peers connected


							for( k = 0; k < total_Nodes; k++) {
							 if (con_cnt == speer_num) break;
							 con_id = cur_node.neighbor_List[k].node_Name[0]-65;
							 if(connected_list[con_id] > 0 ) {
									con_cnt++;
							  }
                            }
                           // printf("\nCon_cnt = %d\n", con_cnt);



                            //if all peers are connected
							if(con_cnt == speer_num) {
				//			    printf("\nComing into receive!!!!\n");
							    recv_data = (char *)malloc(sizeof(struct Message));
								/* recieve a message from the server and display it on the terminal*/
								recv_num = recv(connected_list[listnum], recv_data, sizeof(struct Message), 0);

                                if(recv_num == 0) {
							      fprintf(stderr, "\nConnection to Peer %c was closed!!\n", listnum+65 );
								  perror("recv");
								  continue;
								}
								else if (recv_num > 0) {
printf("\nData received from peer....");
								 printf("\nData received from peer on socket %d\n", connected_list[listnum]);
							//	 printf("\nData contains:\n %s", recv_data);
								 //process the recvd data and load the structure
                                 process_data();

								}

								free(recv_data);

								if(recv_message.message_Type == 'H') {
								  hello_status[listnum] = 1;
								  hello_cnt++;
								  printf("Function %s ,Line %d : hello_cnt is :%d\n",__FUNCTION__,__LINE__,hello_cnt);

								  //call hello processing function
								  hello_packet_processing(hello_cnt-1);

								}
								else if (recv_message.message_Type == 'B') {
								 //call broadcast processing function
									
										broadcast_packet_processing();

								} 
								else {
								  printf("\nMessage Type is Unknown!!!.Discarding the message.\n");
								  continue;
								}



							} //end all peer connect

						} //end if connected_list socket is set

					 } //end if connected list is not empty

				   } //end for - scanning the connected list for active data

			   }//end not a new connection data


		}//end data on sock_read


	} //end while(1)- server loop


  }




void parse_input (char self[]) {

  int i, c, j, k, m, n, found = 0, cur_node_peers;
  FILE *input_file;
  char c_total_nodes[4], c_fprob[6], c_attn[3], c_maxtx_rg[6], c_num_peers[4];
  char node_id[8] = "node ";
  char temp[256];

  strcat(node_id, self);

//  printf("\nthe node_id is %s\n",node_id);

  if ((input_file = fopen("input.txt", "r")) == NULL) {
    printf("\nError in opening input file\n");
	exit(0);
  }


  while(1) {

    while ((c = getc(input_file)) != '\n'); //goto end of first line

	j = 0;
	while ((c = getc(input_file)) != '\n') {  //read the total number of nodes
     c_total_nodes [j++] = c;
	}
    c_total_nodes [j] = '\0';

   total_Nodes = atoi(c_total_nodes);

   while ((c = getc(input_file)) != '\n'); //goto end of third line

	j = 0;
	while ((c = getc(input_file)) != '\n') {  //read the failure probability
     c_fprob [j++] = c;
//	  printf("%c ",c);
	}
    c_fprob [j] = '\0';

//	printf("\nc_fprob is %s\n", c_fprob );
	f_Prob = atof(c_fprob);
  //  printf("\nfprob is %f\n", f_Prob );


    while ((c = getc(input_file)) != '\n'); //goto end of 5th line

	j = 0;
	while ((c = getc(input_file)) != '\n') {  //read the attenuation
     c_attn [j++] = c;
//	  printf("%c ",c);
	}
    c_attn [j] = '\0';

//	printf("\nc_attn is %s\n", c_attn);
	attenuation = atoi(c_attn);
	//printf("\nattenuation is %d\n", attenuation);

    while ((c = getc(input_file)) != '\n'); //goto end of 7th line

	j = 0;
	while ((c = getc(input_file)) != '\n') {  //read the attenuation
     c_maxtx_rg [j++] = c;
//	 printf("%c ",c);
	}
    c_maxtx_rg [j] = '\0';

//	printf("\nc_maxtx_rg is %s\n", c_maxtx_rg);
	max_Tx_Range = atof(c_maxtx_rg);
 //   printf("\nmax tx range is %f\n", max_Tx_Range);


    while ((c = getc(input_file)) != '\n'); //goto end of 9th line
    while ((c = getc(input_file)) != '\n'); //goto end of 10th line

	rewind(input_file);

	while (!feof(input_file)) {
 //       printf("\ntemp contains %s", temp);

		if ((fgets(temp, 255, input_file)) == NULL) {
          printf("\nEnd of Input file Reached!!!\n");
		  found = 0;
		}


      if(strncmp(temp, node_id, 6) == 0) {
		found = 1;
        k = 0;
 		while ( k < strlen(temp)) {
          while (temp[k++] != ' ');
		  k++;
		  while (temp[k++] != ' ');
		  m =0;
		  while(temp[k] != '\0')
             c_num_peers[m++] = temp[k++];
          c_num_peers[m-1] = '\0';

		}

	   strcpy(cur_node.num_Neighbor, c_num_peers);

	//   printf("\nc_num_peers is %s\n", c_num_peers);
	   cur_node_peers = atoi(c_num_peers);
	//   printf("\ncur_node_peers is %d\n", cur_node_peers);

	   cur_node.neighbor_List = (struct nodeList *)malloc(cur_node_peers * sizeof(struct nodeList));

	   while (count < cur_node_peers) {
		    i = 0;
          	while ((c = getc(input_file)) != ' ') {  //read the neighbor name
               cur_node.neighbor_List[count].node_Name[i++] = c;
			}
            cur_node.neighbor_List[count].node_Name[i] = '\0';
  //          printf("\npeer name is %s", cur_node.neighbor_List[count].node_Name);

			i = 0;
            while ((c = getc(input_file)) != '\n') {  //read the neighbor distance
               cur_node.neighbor_List[count].distance[i++] = c;
			}
			cur_node.neighbor_List[count].distance[i] = '\0';
     //       printf("\npeer distance is %s", cur_node.neighbor_List[count].distance);

			count++;

	   }


	  }

	  if(found) break;

	}

	if(found) break;

	if(!found) {
     printf("Not a Valid Node ID!!! Exiting the Program!!!\n");
	 exit(0);

	}


  } //end while(1)

  fclose(input_file);


}


void process_data() {

   int i = 0, j, len;
   len = strlen(recv_data);

   while (i < len) {

	 recv_message.message_Type = recv_data[i];
	 i+=2;
	 j =0;
	 while (recv_data[i] != '$')
	   recv_message.source[j++] = recv_data[i++];
     recv_message.source[j] = '\0';
     i++;
     
	 j =0;
	 while (recv_data[i] != '$')
	   recv_message.retransmit_source[j++] = recv_data[i++];
     recv_message.retransmit_source[j] = '\0';
     i++;

     j =0;
	 while (recv_data[i] != '$')
	   recv_message.seq_Num[j++] = recv_data[i++];
     recv_message.seq_Num[j] = '\0';
     i++;

     j =0;
	 while (recv_data[i] != '$')
	   recv_message.data[j++] = recv_data[i++];
     recv_message.data[j] = '\0';

	 break;


   }

//  printf("Recieved message details...\n");
 // print_message(recv_message);


  //if msg type is broadcast, copy recvd message to brdcst msg q

  if(recv_message.message_Type == 'B') {
    
	  recv_count++;//keep track total number of broadcast's received


	for(i = 0; i < 5; i++) {
	  if(broadcast_queue[i].message_Type == 'U') {
   
         broadcast_queue[i].message_Type = recv_message.message_Type;
		 strcpy(broadcast_queue[i].source,recv_message.source);
         strcpy(broadcast_queue[i].retransmit_source,recv_message.retransmit_source);
         strcpy(broadcast_queue[i].seq_Num,recv_message.seq_Num);
		 strcpy(broadcast_queue[i].data,recv_message.data);
		 break;
	  }
	}

/*	for(i = 0; i < 5; i++) {
      if(broadcast_queue[i].message_Type != 'U') {  
        printf("\nQ msg type is %c ", broadcast_queue[i].message_Type);
		 printf("\nQ msg source is %s ", broadcast_queue[i].source);
         printf("\nQ msg retrans source is %s ", broadcast_queue[i].retransmit_source);
         printf("\nQ msg seq num is %s ", broadcast_queue[i].seq_Num);
		 printf("\nQ msg data is %s ", broadcast_queue[i].data);


	  }
	}*/

  }




}


void print_message(struct Message msg) {

  if (msg.message_Type == 'H')
    printf("\nMessage Type is Hello");
  else if (msg.message_Type == 'B')
    printf("\nMessage Type is Broadcast");
  else
	printf("\nMessage Type is Unknown");

  printf("\nMessage Source is %s", msg.source);
  printf("\nMessage Retranmsmitting Source is %s", msg.retransmit_source);
  printf("\nMessage Sequence number string is %s integer is %d", msg.seq_Num, atoi(msg.seq_Num));
  printf("\nMessage Data is:\n %s", msg.data);

}


void hello_packet_processing(int hello_num) {

  int id, i, j, k, length, n,aslindex;


  //load the neighbor node structures

   strcpy(peer_node[hello_num].name, recv_message.source);

   printf("\nReceived Hello from Neighbor %c.", peer_node[hello_num].name[0]);

   length = strlen(recv_message.data);

 //  printf("\n Hello message length is %d and it contains:\n", length);
  // printf("%s", recv_message.data);

   i = 0;
   while (i < length) {

	 j =0;
	 while (recv_message.data[i] != '#')
	 {
	 //  printf("Function %s , Line %d : hello_num is %d \n ",__FUNCTION__,__LINE__,hello_num);
	   //printf("Function %s , Line %d : recv_message.data is %d \n",__FUNCTION__,__LINE__,recv_message.data[i++]);
	   peer_node[hello_num].num_Neighbor[j++] = recv_message.data[i++];
	 }
     peer_node[hello_num].num_Neighbor[j] = '\0';
     i++;

     n = atoi(peer_node[hello_num].num_Neighbor);
    // printf("\n n is %d", n);

	 peer_node[hello_num].neighbor_List = (struct nodeList *)malloc(n * sizeof(struct nodeList));

     k = 0;
     while(k < n) {
      j =0;
	  while (recv_message.data[i] != '-')
	   peer_node[hello_num].neighbor_List[k].node_Name[j++] = recv_message.data[i++];
      peer_node[hello_num].neighbor_List[k].node_Name[j] = '\0';

      j = 0;
	  i++;
	  while (recv_message.data[i] != '#')
	   peer_node[hello_num].neighbor_List[k].distance[j++] = recv_message.data[i++];
      peer_node[hello_num].neighbor_List[k].distance[j] = '\0';
	  i++;

//	  printf("\n Peer %d name is %s distance is %s", k, peer_node[hello_num].neighbor_List[k].node_Name, peer_node[hello_num].neighbor_List[k].distance);
      k++;
	 }

	 break;

   }

 //  printf("\nHello Message details:\n");
 //  print_node(peer_node[hello_num]);


 //  printf("%s : %d : hello_cnt is %d \n",__FUNCTION__,__LINE__,hello_cnt);
  // printf("%s : %d : peer_num is %d \n",__FUNCTION__,__LINE__,peer_num);


   if(hello_cnt == peer_num) {

  //   printf("\nHello status for node %s is:\n", cur_node.name);
    /* for(i = 0; i < total_Nodes; i++) {
	   if(hello_status[i] == 1)
         printf("\nHello Received from neighbor %c", i+65);

	 }*/

    /* for(i=0;i<peer_num;i++) {
		printf("Function: %s  Line: %d  Peer name is :%s\n",__FUNCTION__,__LINE__, peer_node[i].name);
		printf("Function: %s  Line: %d  No of Neighbors is :%s\n",__FUNCTION__,__LINE__, peer_node[i].num_Neighbor);
     }*/



     printf("\n*****************************************\n");
     printf("*Service open for Broadcast Transmission*\n");
	 printf("*   Enter B to start broadcast    *\n");
printf("*   Enter S to display statistics    *\n");
	 printf("*****************************************\n");

   }


}


void inop_algorithm() {
	
   //globals referred
	/*
	char *F_list;
	char *R_list;
	float *PI_prob;
	*/

	int i = 0,q = 0,reset=0, k;
	int r,rq;
	int qi_exists;
	float qi_dist = 0;
    int r_index = 0;
	float ri_dist = 0;
    float  max =0;
	int f_index = 0;


	//To fill the sorted N list 
	minDistIndexFunction();
	
	

	for(i=0; i < peer_num; i++) {
		
		
		if(inop_sortedNName[i] == 'Z') continue;

		bzero(R_list, sizeof(R_list));

		r_index=0;
		
		
		//finding R list 
		for(q=0; q < i; q++) {
            
			if(inop_sortedNName[q] == 'Z') continue;

	//		printf("\n inop_sortedNName[%d] is %c,inop_sortedNName[%d] is %c \n",q,inop_sortedNName[q],i,inop_sortedNName[i]);
			
			qi_exists = commonNeighborCheck(inop_sortedNName[q],inop_sortedNName[i]);
	
			if(qi_exists){
		//	   printf("\n qi exists \n");
			   
			   qi_dist = find_v_w_dist(inop_sortedNName[q],inop_sortedNName[i]);
			   
	//		   printf("\ninop_sortedNName[%d] is %c,inop_sortedNName[%d] is %c,qi_dist is %f\n",q,inop_sortedNName[q],i,inop_sortedNName[i],qi_dist);

			}
			else
		     continue;
			
			if((qi_dist < inop_sortedNDist[i])){
				
				R_list[r_index] = inop_sortedNName[q]; //q not i;
			    r_index++;
			
			}
		
		}

        printf("\nRlist for I = %c", inop_sortedNName[i]);
        for(k = 0; k < r_index; k++) {
		  printf("\nAdded to RLIST[%d] -  %c \n",k,R_list[k]);
		}


		if(r_index == 0){

			PI_prob[i] = inop_sortedNDist[i];
	//		printf("\nr-index==0 - Added to PI_prob[%d] %f \n",i,PI_prob[i]);
		
		}
		else { 

			  for(r=0; r < r_index; r++){

                if(R_list[r] == inop_sortedNName[i]) continue;
			    
				ri_dist = find_v_w_dist(R_list[r],inop_sortedNName[i]);
				
				for(rq = 0 ; rq < r_index; rq++) {	
				  
					if(rq==r) continue;
				
		//			printf("\n R_list[%d] is %c,inop_sortedNName[%d] is %c \n",rq,R_list[rq],i,inop_sortedNName[i]);
					qi_dist= find_v_w_dist(R_list[rq],inop_sortedNName[i]);					
		//			printf("\nqi_dist in R is %f", qi_dist);

					if(qi_dist < ri_dist){
                      ri_dist = qi_dist;
					  //R_list[r]='0';
                      
					}
				
				  
				}
			
			  }
			 
			    
			//PI_Prob min	
			 if(i == 0){
				PI_prob[i]=min(ri_dist,inop_sortedNDist[i]);		
				printf("\nAdded to PI_prob[%d] %f \n",i,PI_prob[i]);
		
			 }
			 else {
				PI_prob[i]=min((PI_prob[i-1]+ri_dist),inop_sortedNDist[i]);
				printf("\nAdded to PI_prob[%d] %f \n",i,PI_prob[i]);
			 }

		}

		if(PI_prob[i] == inop_sortedNDist[i]) {
			
			F_List[f_index] = inop_sortedNDist[i];
			printf("\nAdded to F_List[%d]  node %c dist %f \n",f_index, inop_sortedNName[i], F_List[f_index]);
			f_index++;
		}

		
	}

	for(i=0; i < f_index; i++) {

		if(F_List[i]>max){
			max = F_List[i];
		}

	}

	rebroadcast_dist=max;
printf("\nINOP computation completed!!!\n");
	printf("\nRebroadcast distance after INOP is %f", rebroadcast_dist);

    for(reset = 0; reset < f_index ;reset++)
			F_List[reset]='0';

}

float min(float a,float b) {
	
	if(a<b)
		return a;
	else 
		return b;
}


//Sands came here
void broadcast_packet_processing() {

 //use probability to receive the packet

 //use seed for RBT before performing the relay

 //strcpy(peer_node[hello_num].name, recv_message.source);

 //printf("\nReceived Hello from Neighbor %c!!", peer_node[hello_num].name[0]);

    int i,j,k,acceptPacket;
    
       
    int alreadyReceived=0;
	int tempseq;
    
    char source;	
    float maxdistance, temp;
    
	   
    
	//printf("\nCalling calculate_probability!!!\n");
    acceptPacket = calculate_probability(f_Prob);

   if (!acceptPacket){
      printf("\n Probabilistically Dropping Packet!!!!\n");
	     for(j = 0; j < 5; j++) { //scan through broadcast msg queue mark that entry unused
		    tempseq=atoi(broadcast_queue[j].seq_Num);
	        if ((broadcast_queue[j].source[0] == recv_message.source[0]) && (tempseq == atoi(recv_message.seq_Num))) {
		       broadcast_queue[j].message_Type = 'U';
			}

		  } //end for j
	 
	
	  recv_count--;
      return;
    }
     

   //check if you are receiving you own packet. if so, discard
   if (recv_message.source[0] == cur_node.name[0]) {
    printf("\nReceiving Own Packet!!!Discarding it!!!");
    for (i = 0; i < 5; i++) {
		if(broadcast_queue[i].source[0] == recv_message.source[0]) {
         broadcast_queue[i].message_Type = 'U';
		}

	}
	return;
   }

    for(i=0;i<5;i++) {  //find if new broadcast from the source or if old packet
     tempseq=atoi(broadcast_queue[j].seq_Num);
	 if ((broadcast_queue[i].source[0] == recv_message.source[0]) && (tempseq == atoi(recv_message.seq_Num))) {
           printf("\nRecevied an already received packet!!!!");
           alreadyReceived = 1; 
		   break;
       }
    
	}
	
	source = recv_message.retransmit_source[0];
	maxdistance=atof(recv_message.data);

	//eliminating source node from the sortedNName
	printf("\nEliminating source node from the inop_sortedNName which now contains\n ");
	for(i=0;i<peer_num;i++){
		if(inop_sortedNName[i]==source){
			inop_sortedNName[i]='Z';
		}
	printf("\ninop_sortedNName[%d] is %c", i, inop_sortedNName[i]);
	}

	
    if(!RBT_Flag) {

	  //eliminate common neighbors
      printf("\nEliminating common neighbors between source node and self\n ");
      find_common_neighbors(maxdistance);
      printf("\ninop_sortedNName contains\n");
	  for(i=0;i<peer_num;i++){
		   printf("\ninop_sortedNName[%d] is %c", i, inop_sortedNName[i]);
	  }

      if ((acceptPacket) && (!alreadyReceived)) {  //new broadcast msg
		printf("\nAccepted packet is New");
		//Calling inop algorithm
		inop_algorithm();
		rebroadcast();
		
		RBT_Flag=1;
	
	  }
      /*else if ((acceptPacket) && (alreadyReceived)) { //old broadcast msg
	  printf("\nAccepted packet is Old");
		j = 0;
		for(i=1;i<5;i++) {    
            
		    if ((broadcast_queue[j].source[0] == broadcast_queue[i].source[0]) && (atoi(broadcast_queue[j].seq_Num) == atoi(broadcast_queue[i].seq_Num))) {
              printf("\nDiscarding an already received packet!!!!");  
   		      printf("Resetting broadcast queue for already received message\n");
			  printf("Rebroadcast source is %c \n ",broadcast_queue[j].retransmit_source[0]);
			 broadcast_queue[i].message_Type = 'U';
			}

				   
		}

          return;
	  } */

	  

	} 
	else {
       
	  printf("\nReceived packet while RBT is running!!!\n");
	  //eliminate common neighbors
      printf("\nDuring RBT, Eliminating common neighbors between source node and self\n ");
      find_common_neighbors(maxdistance);
      printf("\nDuring RBT inop_sortedNName contains\n");
	  for(i=0;i<peer_num;i++){
		   printf("\ninop_sortedNName[%d] is %c", i, inop_sortedNName[i]);
	  }

	  if ((acceptPacket) && (alreadyReceived)) { //old broadcast msg
	    printf("\nOld accepted packet");
		j = 0;
		tempseq = atoi(broadcast_queue[j].seq_Num);

		for(i=1;i<5;i++) {    
            
		    if ((broadcast_queue[j].source[0] == broadcast_queue[i].source[0]) && ( tempseq == atoi(broadcast_queue[i].seq_Num))) {
              printf("\nDiscarding an already received packet!!!!");  
   		//      printf("Resetting broadcast queue for already received message\n");
		//	  printf("Rebroadcast source is %c \n ",broadcast_queue[j].retransmit_source[0]);
			 broadcast_queue[i].message_Type = 'U';
			}

				   
		}

          
	  }
	
	}


	
  
  
}

int calculate_probability(float probability) {
    float myrand;
	myrand=(double)rand()/RAND_MAX;
	if(myrand > probability){
		return 1;
	}
	else
		return 0;
}
//Sands came here


void print_node(struct Node N) {

  int m, i;

  m = atoi(N.num_Neighbor);
 // N.neighbor_List = (struct nodeList *)malloc(m * sizeof(struct nodeList));

  printf("\nNode name is %c", N.name[0]);
  printf("\nNode %c has %d neighbors", N.name[0], m);
  printf("\nNode %c 's neighbor details are:", N.name[0]);
  for(i = 0; i < m; i++)
    printf("\nNeighbor %c is at a distance of %s", N.neighbor_List[i].node_Name[0], N.neighbor_List[i].distance);
 printf("\n");

}


void sendHello() {

 int i = 0, j, k ;
 char helloMsg[300] = {'\0'};

 //Build hello packet
 helloMsg[i++] = 'H';
 helloMsg[i++] = '$';

 helloMsg[i++] = cur_node.name[0];

 helloMsg[i++] = '$';
 helloMsg[i++] = '0';
 helloMsg[i++] = '$';
 helloMsg[i++] = '0';
 helloMsg[i++] = '$';

 j = 0;
 while (j < strlen(cur_node.num_Neighbor) - 1)
   helloMsg[i++] = cur_node.num_Neighbor[j++];

 helloMsg[i++] = '#';

 for (k = 0;k < peer_num; k++) {

   helloMsg[i++] = cur_node.neighbor_List[k].node_Name[0];

   helloMsg[i++] = '-';

   j = 0;

   while (j < strlen(cur_node.neighbor_List[k].distance) - 1)
     helloMsg[i++] = cur_node.neighbor_List[k].distance[j++];

   helloMsg[i++] = '#';


 }

 helloMsg[i++] = '$';
 helloMsg[i] = '\0';

// printf("\nHelloMsg Structure is \n%s size of %d", helloMsg, strlen(helloMsg));

 printf("\nCurrent node data is:");
 print_node(cur_node);




 for(j=0;j<peer_num;j++) {
   if(sockfd[j] != 0) {
	if (send(sockfd[j], (const void*) helloMsg, strlen(helloMsg), 0) < 0) {
	  fprintf(stderr, "\nError in service request\n");
      perror("send");

	}

printf("\nSent hello to Peer : %c ",connect_sortedNName[j]);
	printf("\nSent hello to Peer : %c on Socket : %d \n",connect_sortedNName[j],sockfd[j]);
   }


 }



}


//sands came here
void sendBroadcast() {

 int i = 0, j, k ;
 char broadcastMsg[300] = {'\0'};
 float maxAslDist = 0, temp;
 char seq;
 char tempseq[10];
 char broadcast_data[10];
 int temp_seqCnt = seqCnt;

// printf("\nENTERED FUNCTION TRACED : %s \n",__FUNCTION__);
 itoa(temp_seqCnt,tempseq);
// printf("seqCnter value is %d \n",temp_seqCnt);
// printf("tempseq value is %s \n",tempseq);

 //Calling inop algorithm
 inop_algorithm();

 //Build broadcast packet
 broadcastMsg[i++] = 'B';
 broadcastMsg[i++] = '$';

 broadcastMsg[i++] = cur_node.name[0];

 broadcastMsg[i++] = '$';
 broadcastMsg[i++] = cur_node.name[0]; //retransmit source
 broadcastMsg[i++] = '$';
 strcat(broadcastMsg , tempseq);
 strcat(broadcastMsg , "$");
 
 

  maxAslDist = rebroadcast_dist; //store the max distance used for the source broadcast
  
  gcvt (maxAslDist,5,broadcast_data);
 
  strcat(broadcastMsg, broadcast_data);
  strcat(broadcastMsg,"$");
  strcat(broadcastMsg,"\0");
 
  printf("\nBroadcast Message Structure is: \n%s", broadcastMsg);
//  printf("FUNCTION %s : maxAslDist is %f ",__FUNCTION__,maxAslDist);
 



 //peer num iteration gives the information for FD

 for(j=0;j<peer_num;j++) {
  //temp = atof(cur_node.neighbor_List[j].distance);
	 temp = inop_sortedNDist[j];
  if( temp <= maxAslDist){
   if(sockfd[j] != 0) {
	if (send(sockfd[j], (const void*) broadcastMsg, strlen(broadcastMsg), 0) < 0) {
	  fprintf(stderr, "\nError in broadcast service request\n");
      perror("send broadcast");

	}
	printf("\nSent Broadcast  to Peer : %c on Socket : %d \n",connect_sortedNName[j], sockfd[j]);
   }//if sockfd
  }//if maxDist check
  }//end for

  recv_count++;
  broadcast_count++;
  compute_consumed_power(maxAslDist); //calculate consumed power

  //printf("\nLEFT FUNCTION TRACED : %s \n",__FUNCTION__);
}//end sendBroadcast




float find_v_w_dist(char vnm,char w) {

  int i, j;
  int numofNeigh;
 // printf("FUNCTION: %s vnm passed is %c, w passed is  %c \n",__FUNCTION__,vnm,w);


  for (i = 0; i < peer_num; i++) {
    if(peer_node[i].name[0] != vnm)
		continue;
	numofNeigh  = atoi(peer_node[i].num_Neighbor);
    for (j = 0; j < numofNeigh; j++) {
      if(peer_node[i].neighbor_List[j].node_Name[0] == w)
	  {
	//	  printf("FUNCTION: %s vw PEERNODE NAME is %c \n",__FUNCTION__,peer_node[i].neighbor_List[j].node_Name[0]);
	//	  printf("FUNCTION: %s vw distance is %f \n",__FUNCTION__,atof(peer_node[i].neighbor_List[j].distance));
		  return atof(peer_node[i].neighbor_List[j].distance);
	  }
	}
  }

 // printf("FUNCTION: %s vw PEERNODE NAME is %c \n",__FUNCTION__,peer_node[i].neighbor_List[j].node_Name[0]);

  return 0;
}



void minDistIndexFunction() {

  int i, j = 0, k = 0;
  float min = 9999, temp;

  struct nodeList temp_node[peer_num];


  for (i = 0; i < peer_num; i++) {
   strcpy(temp_node[i].distance, cur_node.neighbor_List[i].distance);
   strcpy(temp_node[i].node_Name, cur_node.neighbor_List[i].node_Name);
  }

  //sort the temp_node of nieghbors and fill the sortedNDist and sortedNName
  while (j < peer_num) {
   for (i = 0; i < peer_num; i++) {
	temp = atof(temp_node[i].distance);
	if(temp < min) {
      min = temp;
	  k = i;
	}

   }
   
   if(!sorted_flag) {
     connect_sortedNDist[j] = min;
     connect_sortedNName[j] = cur_node.neighbor_List[k].node_Name[0];
   }
   
   if(inop_sortedNName[j] != 'Z') {
     inop_sortedNDist[j] = min;
     inop_sortedNName[j] = cur_node.neighbor_List[k].node_Name[0];
   }

   strcpy (temp_node[k].distance, "9999");
  // printf("\nconnect_sortedNDist %f, connect_sortedNName is %c k is %d", connect_sortedNDist[j], connect_sortedNName[j], k);
  // printf("\ninop_sortedNDist %f, inop_sortedNName is %c k is %d", inop_sortedNDist[j], inop_sortedNName[j], k);
   min  = 9999;
   j++;

  }

  sorted_flag = 1;
}

int commonNeighborCheck(char vTempName, char wTempName) {

  int i,j, k;
  int numNeighbor;

  for(i = 0; i < peer_num;i++) {

	if(peer_node[i].name[0] == vTempName) {

     numNeighbor= atoi(peer_node[i].num_Neighbor);

     for (j = 0; j < numNeighbor ; j++) {
      if(peer_node[i].neighbor_List[j].node_Name[0] == wTempName)
	    return 1;
	 }

	}


  }



  return 0;

}



void get_node_address(int n) {

  FILE *fp1;
  char temp[20];
  int i;

 // n =25;

  ipaddr_list = (char **)calloc(n, sizeof(char *));
  for(i = 0; i < n; i++)
    ipaddr_list[i] = (char *)calloc(16, sizeof(char));


  if (n > 25) {
	printf("\n Only 25 IP addresses defined!!");
	exit(0);
  }
  if ((fp1 = fopen("ipaddrs.txt", "r")) == NULL) {
    printf("\nError in opening ip address file\n");
	exit(0);
  }

  i = 0;

//  printf("\nno: of nodes = %d", n);

  while (i < n) {

       if ((fgets(temp, 16, fp1)) == NULL) {
          printf("\nError reading IP addresses!!!\n");
	   }

       strncpy(ipaddr_list[i], temp, 13);
	//   printf("\nIP address is %s", ipaddr_list[i]);
	   i++;
  }


  fclose(fp1);

}

void itoa(int n, char str[]) { 
  

  int i, j, sign; 
  char *temp; 

//  printf("\nENTERED FUNCTION TRACED : %s \n",__FUNCTION__);
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
//  printf("\nLEFT FUNCTION TRACED : %s \n",__FUNCTION__);
   
} 


void find_common_neighbors(float dist) {

  int i, j, k, ind, ngh_num;

//  printf("\nENTERED FUNCTION TRACED : %s \n",__FUNCTION__);
   for (i = 0; i < peer_num; i++)  { //extract peer node number

	if(recv_message.retransmit_source[0] == peer_node[i].name[0]) {
	   ind = i;
	   break;
	}

   }

  ngh_num =  atoi(peer_node[ind].num_Neighbor);
  
  for (i = 0; i < peer_num; i++) { //extract common physical neighbors
   for (j = 0; j < ngh_num; j++)	{
       if(cur_node.neighbor_List[i].node_Name[0] ==  peer_node[ind].neighbor_List[j].node_Name[0] ) {
           for(k=0;k<peer_num;k++) {
			   if(inop_sortedNName[k]==cur_node.neighbor_List[i].node_Name[0]){
				   inop_sortedNName[k]='Z';
			   }

		   }
			  
	   }	  
		  
   }

  }

     
//  printf("\nLEFT FUNCTION TRACED : %s \n",__FUNCTION__);
	
}

void rebroadcast () {
 
   
   double RBT, rand_value;
   int i;
   float temp;
   
 //  printf("\nENTERED FUNCTION TRACED : %s \n",__FUNCTION__);

   //Compute MaxNB and CNB, for RBT computation
   MaxNB = 0;
   CNB = 0;

   for (i = 0; i < peer_num; i++) {
	   temp = atof(cur_node.neighbor_List[i].distance);
     if( (temp <= max_Tx_Range) && (temp != 0))
		MaxNB++;
   }

   for (i = 0; i < total_Nodes; i++) {
	   temp = atof(cur_node.neighbor_List[i].distance);
	 if( (temp <= rebroadcast_dist) && (temp != 0))
       CNB++;
	 
   }
     
   
   //calculate RBT
   srand(seed);
   rand_value = (double)rand()/RAND_MAX;
   printf("\n Random_value is %lf", rand_value);

   RBT = ceil((CNB/MaxNB) * max_Delay * rand_value);
   
   printf("\nCNB is %d MaxNB is %d", CNB, MaxNB);
   printf("\n RBT value is %lf", RBT);

   
   
	/* Establish a handler for SIGALRM signals. */
    signal (SIGALRM, inop_alarm_handler);

	/*set an alarm to wait for Client B to respond*/
	printf("\nStarting RBT timer\n");
	alarm(RBT+3);
      // send_rebroadcast();
	
   
   
//   printf("\nLEFT FUNCTION TRACED : %s \n",__FUNCTION__);
}
 
void inop_alarm_handler(int sig){

	int i;
	float temp;

//	printf("\nENTERED FUNCTION TRACED : %s \n",__FUNCTION__);

	RBT_Flag=0;
	inop_algorithm();

	for(i = 0 ;i < peer_num;i++){
	   send_list[i]=0;
    }

    for(i = 0 ;i < peer_num;i++) { //calculate the actual neighbors(both rng and non-rng), who are going to receive the broadcast
        
	  if(inop_sortedNName[i] == 'Z') continue;
      temp = inop_sortedNDist[i];
      if((temp <= rebroadcast_dist) && (rebroadcast_dist != 0))
	     send_list[i] = 1;
    }
 
/*	printf("\nSend List contains\n");
    for(i = 0 ;i < peer_num;i++){
	   printf("\nsend_list[%d] is %d", i, send_list[i]);
    }*/

	printf("\nRBT timer expired!!! Retransmitting packet if required!!\n");
	send_rebroadcast();
	
	//printf("\nLEFT FUNCTION TRACED : %s \n",__FUNCTION__);
}

void send_rebroadcast() {
   
  int i, j, k =0;
  int cnt =0;
  char rebroadcast_data[300] = {'\0'};
  float maxRebDist;
   char broadcast_data[20];
  printf("\nENTERED FUNCTION TRACED : %s \n",__FUNCTION__);

  maxRebDist=rebroadcast_dist;
  
  for(i = 0; i < 5; i++) { //scan through broadcast msg queue and send 
	  k = 0;
	  printf("broadcast_queue[%d].message_Type is %c \n",i,broadcast_queue[i].message_Type );
	  if(broadcast_queue[i].message_Type != 'U') {
		  rebroadcast_data[k++] = broadcast_queue[i].message_Type;
		  strcat(rebroadcast_data, "$");
		  strcat(rebroadcast_data, broadcast_queue[i].source);
		  strcat(rebroadcast_data, "$");
		  strcat(rebroadcast_data, cur_node.name); //copying our (relay node) name
		  strcat(rebroadcast_data, "$");
		  strcat(rebroadcast_data, broadcast_queue[i].seq_Num);
		  strcat(rebroadcast_data, "$");

		  gcvt (maxRebDist,5,broadcast_data);
 
		  strcat(rebroadcast_data, broadcast_data);
		  strcat(rebroadcast_data,"$");
		  strcat(rebroadcast_data,"\0");
		  //strcat(rebroadcast_data, broadcast_queue[i].data);
		  //strcat(rebroadcast_data, "$");

		  //strcat(rebroadcast_data, "\0");

		  printf("\n Rebroadcast Message contains: %s\n", rebroadcast_data); 

		  for(j=0;j<peer_num;j++) {
		
			  if(send_list[j] == 1) {
		        cnt=1;
			

				if (send(sockfd[j], (const void*) rebroadcast_data, strlen(rebroadcast_data), 0) < 0) {
				  fprintf(stderr, "\nError in re-broadcast service request\n");
				  perror("send rebroadcast");

				}
				printf("\nSent re-broadcast  to Peer : %c on Socket : %d \n",connect_sortedNName[j],sockfd[j]);
		
			}//if maxDist check
		  }//end for
		  	

		  //set the sequence_number data to 'sent'
		  if(cnt) {
			 broadcast_count++;//keep track total number of rebroadcast's sent
             compute_consumed_power(maxRebDist); //calculate consumed power
		  }

		  //mark que entry as un used. 
		  broadcast_queue[i].message_Type = 'U';
		  strcpy(broadcast_queue[i].data, "\0");
          strcpy(broadcast_queue[i].retransmit_source, "\0"); 
		  strcpy(broadcast_queue[i].seq_Num, "\0");
		  strcpy(broadcast_queue[i].source, "\0");
          strcpy (rebroadcast_data, "\0"); 
		  
	  }
 
  }

//  printf("\nLEFT FUNCTION TRACED : %s \n",__FUNCTION__);
  

}

void compute_consumed_power(float tx_dist) {


	float power = 0;

	power = pow(tx_dist, attenuation);

	total_power_consumed += power;


}




