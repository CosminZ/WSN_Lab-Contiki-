/*
  Firmware of the central node in the WSN
  Jorge Saez
  WSN Laboratory
  Uni Freiburg
  Freiburg, 25.01.2015
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "localizationList.h"
#include "udpClient6.h"


#define PORT_REC 8880
#define PORT_SEND 8881
#define MESSAGE "hello"
#define MESSAGE2 "TO YOUR RIGHT"
//define number of nodes in the network

#define NODES_NUMBER 2


int main(void)
{
  int sock;
  socklen_t clilen;
  struct sockaddr_in6 server_addr, client_addr;
  char buffer[1024];
  char addrbuf[INET6_ADDRSTRLEN];
  int local_done = 0; //flag to be set when the localization process is done
/* -------- Initializing localization arraylist ----------------- */
	struct arraylist list;
	arraylist_initial(&list);

  /* create a DGRAM (UDP) socket in the INET6 (IPv6) protocol */
  sock = socket(PF_INET6, SOCK_DGRAM, 0);

  if (sock < 0) {
    perror("creating socket");
    exit(1);
  }

#ifdef V6ONLY
  // setting this means the socket only accepts connections from v6;
  // unset, it accepts v6 and v4 (mapped address) connections
  { int opt = 1;
    if (setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)) < 0) {
      perror("setting option IPV6_V6ONLY");
      exit(1);
    }
  }
#endif

  /* create server address: this will say where we will be willing to
     accept datagrams from */

  /* clear it out */
  memset(&server_addr, 0, sizeof(server_addr));

  /* it is an INET6 address */
  server_addr.sin6_family = AF_INET6;

  /* the client IP address, in network byte order */
  /* in this example we accept datagrams from ANYwhere */
  server_addr.sin6_addr = in6addr_any;


  //Get the connection port based on the IP address of the node.
  //*******************//
     FILE * fp = popen("ifconfig", "r");
    
     char *sep = NULL, *sep2, *sep3 =NULL, *sep4= NULL, *sep5 = NULL;
     char *aaaa[4];
    
     char bbb[20];
     //char ccc[20];
        if (fp) {
                char *p=NULL, *e, *x = NULL; size_t n;
                while ((getline(&p, &n, fp) > 0) && p) {

                   if (p = strstr(p, "inet6 ")) {
                        
                        p+=5;
                        
                        if (p = strchr(p, ':')) {
                            
                            ++p;
                            //the whole ip address
                            sep2 = strtok(p, " ");
                            

                            if(sep3 = strstr(sep2, "bbbb")) {
                              sep3 += 2;
                              sep4 = strtok(sep3, ":");
                            

                            while (sep4 != NULL) {
                            
                              //copy the last segment of the IP address to an array
                              strcpy(bbb, sep4);
                              sep4 = strtok(NULL,":");

                            }
                            sep5 = strtok(sep4, "/");
                            while (sep5 != NULL){
                              strcpy(bbb, sep5);
                              sep5 = strtok(NULL,"/");
                            }
                          }

                            if (e = strchr(p, ' ')) {
                                  *e='\0';
                            }
                        }
                   }
              }
        }
        
        pclose(fp);
        
      
      //Convert the array of chars into an int
      int port_con;
      unsigned char byte;
      sscanf(bbb,"%x",&port_con);
      byte = port_con & 0xFF;
      port_con = (port_con/1000) + 64000;
      printf("Connected to Port: %d\n", PORT_REC);


  //************************************************************************************************
  /* the port we are going to listen on, in network byte order */
  server_addr.sin6_port = htons(PORT_REC);

  /* associate the socket with the address and port */
  if (bind(sock, (struct sockaddr *)&server_addr,
	   sizeof(server_addr)) < 0) {
    perror("bind failed");
    exit(2);
  }

  while (1) {


	char *incomingIp = NULL;
	int index_list1 = 0;
	
  /* now wait until we get a datagram */
  // we use the same port for every possible connection
    printf("waiting for a datagram...\n");
    clilen = sizeof(client_addr);
    if (recvfrom(sock, buffer, 1024, 0,
		 (struct sockaddr *)&client_addr,
		 &clilen) < 0) {
      perror("recvfrom failed");
      exit(4);
    }
/********************************************************LOCALIZATION***********************************************************************/
//the localization algorithm should be carried out until the max number of nodes in the network is reached
//only vibration packets can trigger the localization algorithm (TODO)
//define vocabulary of the frame(TODO)
if (local_done == 0) {

    /* now client_addr contains the address of the client */
	
	char * bufferPtr = NULL;

	//store the incoming IP of the node
	incomingIp = inet_ntop(AF_INET6, &client_addr.sin6_addr, addrbuf, INET6_ADDRSTRLEN);
	
	bufferPtr = (char *)malloc(25*sizeof(char));
	strcpy(bufferPtr, incomingIp);	
  printf("got '%s' from %s\n", buffer, incomingIp);
 
		
	printf("Now going for check and add -------------------\n");
	addToListWithCheck(&list, bufferPtr);
	printf("------------------------ Printing list after check and add ------------- \n");
	index_list1 = 0;
  	for(index_list1 = 0; index_list1 != 6; index_list1++) {
    		printf("CHECK: %s\n", arraylist_get(list, index_list1));
  	}
    	printf("Size of list = %d \n",arraylist_get_size(list));
		
		
	printf ("Left Ip of - %s - is - %s -\n", incomingIp, getLeftOf (list, incomingIp));
	printf ("Right Ip of - %s - is - %s -\n", incomingIp, getRightOf (list, incomingIp));
	
	int index_list = 0;
  	for(index_list = 0; index_list != 6; index_list++) {
    		printf("CHECK: %s\n", arraylist_get(list, index_list));
  	}
    	printf("Size of list = %d \n",arraylist_get_size(list));
      //check if the list contains already all the nodes
      if (arraylist_get_size(list) == NODES_NUMBER) {
        local_done = 1;
        printf("Localization is complete\n");
      }
      //return ack message
      printf("sending message back\n");

    if (sendto(sock, MESSAGE, sizeof(MESSAGE), 0,
               (struct sockaddr *)&client_addr,
         sizeof(client_addr)) < 0) {
      perror("sendto failed");
      exit(5);
    }
  }
  //*******************************************************END OF LOCALIZATION*************************************************************************/
  //************In this stage the central node needs to determine what to do based on the information received ******************************************/
  //************If an event is triggered in a node, it notices automatically the central node and it will decide****************************************/


  else {
      //Here we will decide what to do with the incoming packets
      //First approach: send a message to the right of the incoming IP
      char *incomingIp2 = NULL;
      char *rightIP = NULL;
      incomingIp2 = inet_ntop(AF_INET6, &client_addr.sin6_addr, addrbuf, INET6_ADDRSTRLEN);
      printf("got '%s' from %s\n", buffer, incomingIp2);
      //return ack message
      
      //calculate the IP which is on the right
      rightIP = getRightOf (list, incomingIp2);
      printf("sending to the right of %s - which is - %s - \n", incomingIp2, rightIP);
	    char* bufferPtr1= (char*) malloc(25*sizeof(char));
	    strcpy(bufferPtr1, rightIP);
      //send message creating a new socket
      //all the information from the central node will be sent through port 8881
	    udpClient6(bufferPtr1, MESSAGE2,PORT_SEND);
	    for(index_list1 = 0; index_list1 != 6; index_list1++) {
    		printf("CHECK: %s\n", arraylist_get(list, index_list1));
  	  }
    	printf("Size of list = %d \n",arraylist_get_size(list));
	
      /*if (sendto(sock, MESSAGE2, sizeof(MESSAGE), 0,
               (struct sockaddr *)&client_addr,
         sizeof(client_addr)) < 0) {
      perror("sendto failed");
      exit(5);
      }*/
  }
    //********************************************************
  }
  
  return 0;
}
