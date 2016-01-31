/*
  Example IPv6 UDP client.
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

  28.01.2015 12:00 Client do not need to wait for a reply

*/



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


#define MESSAGE "VIB"
#define PORT 8880


int main(int arc, char **argv)
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

  /* create server address: where we want to send to */

  /* clear it out */
  memset(&server_addr, 0, sizeof(server_addr));

  /* it is an INET address */
  server_addr.sin6_family = AF_INET6;

  /* the server IP address, in network byte order */
  inet_pton(AF_INET6, argv[1], &server_addr.sin6_addr);

  /* the port we are going to send to, in network byte order */
  //get the port number from the IP address
  char *sep_ip; 
  char last_segment[20];
  //get the last digits
  printf("IPv6 address of the server: %s\n", argv[1]);
  sep_ip = strtok(argv[1], "::");
  while (sep_ip != NULL) {                        
    //copy the last segment of the IP address to an array
    strcpy(last_segment, sep_ip);
    sep_ip = strtok(NULL,":");
   }

  //convert it to an integer
  int port_con;

  unsigned char byte;
  sscanf(last_segment,"%x",&port_con);
  byte = port_con & 0xFF;

  
  //if (port_con >= 1000) {
    port_con = (port_con/1000);
  //}

  printf("Connecting through port: %d\n", port_con);
  //server_addr.sin6_port = htons(port_con + 64000);//***********************************************************
  server_addr.sin6_port = htons(PORT);


  /* now send a datagram */
  if (sendto(sock, MESSAGE, sizeof(MESSAGE), 0,
             (struct sockaddr *)&server_addr,
	     sizeof(server_addr)) < 0) {
      perror("sendto failed");
      exit(4);
  }
/*
  printf("waiting for a reply...\n");
  clilen = sizeof(client_addr);
  if (recvfrom(sock, buffer, 1024, 0,
	       (struct sockaddr *)&client_addr,
               &clilen) < 0) {
      perror("recvfrom failed");
      exit(4);
  }

  printf("got '%s' from %s\n", buffer,
	 inet_ntop(AF_INET6, &client_addr.sin6_addr, addrbuf,
		   INET6_ADDRSTRLEN));
*/
  /* close socket */
  close(sock);

  return 0;
}