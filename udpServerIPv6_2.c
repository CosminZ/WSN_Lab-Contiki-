/*
  Example IPv6 UDP server.
  Copyright (C) 2010 Russell Bradford

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, 
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (http://www.gnu.org/copyleft/gpl.html)
  for more details. 


  Modified by Jorge Saez
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

#define PORT 8881 //port used for receiving something from the server
#define MESSAGE "hello"


int main(void)
{
  int sock;
  socklen_t clilen;
  struct sockaddr_in6 server_addr, client_addr;
  char buffer[1024];
  char addrbuf[INET6_ADDRSTRLEN];

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
     int right_prefix = 0;
    
     char bbb[20];
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
                                right_prefix = 1;

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
      //if (port_con >= 1000) {
        port_con = (port_con/1000);
      //}
      
      printf("Connected to Port: %d\n", port_con);


  //************************************************************************************************
  /* the port we are going to listen on, in network byte order */
  //server_addr.sin6_port = htons(port_con + 64000);
      server_addr.sin6_port = htons(PORT);

  /* associate the socket with the address and port */
  // Open port only if the prefix was correct
  if (right_prefix == 1) {
    if (bind(sock, (struct sockaddr *)&server_addr,
	     sizeof(server_addr)) < 0) {
      perror("bind failed");
      exit(2);
    }
}

  while (1) {

    /* now wait until we get a datagram */
    printf("waiting for a datagram...\n");
    clilen = sizeof(client_addr);
    if (recvfrom(sock, buffer, 1024, 0,
		 (struct sockaddr *)&client_addr,
		 &clilen) < 0) {
      perror("recvfrom failed");
      exit(4);
    }

    /* now client_addr contains the address of the client */
    printf("got '%s' from %s\n", buffer,
	   inet_ntop(AF_INET6, &client_addr.sin6_addr, addrbuf,
		     INET6_ADDRSTRLEN));

    printf("sending message back\n");

    if (sendto(sock, MESSAGE, sizeof(MESSAGE), 0,
               (struct sockaddr *)&client_addr,
	       sizeof(client_addr)) < 0) {
      perror("sendto failed");
      exit(5);
    }

  }
  
  return 0;
}