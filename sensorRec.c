/*
  Firmware of a sensor node (Working as a receiver or server)
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
#include "udpServer6.h"
#include "udpClient6.h"



#define PORT_REC 8881
#define PORT_SEND 8880
#define ACK "Thanks"
#define MESSAGE2 "Processed"
#define CENTRAL_NODE_IP "bbbb::fec2:deff:fe30:ad8e" 

int main(void) {

	//the sensor node is constantly listening, waiting for information coming from the central node
	//probably this information is related with the display, i.e., what it gets should be displayed.
	//It is also a possibility to receive a request for information????
	while (1) {
		char *received_data;
		char *processed_data;
		printf("before receive data\n");
		received_data = udpServer6(ACK, PORT_REC);
		printf("after receive data\n");
		//then, process this info!!!()
		//or if it is a request for measured data, send it
		//send info through the port 8880
		udpClient6(CENTRAL_NODE_IP, MESSAGE2,PORT_SEND);




	}



}