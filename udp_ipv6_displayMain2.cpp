/*
* Author: Manisha
* 
* file:-Display figure locally and msg using udp_ipv6
*
*/

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>


/* Network socket*/

#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
 
#define BUFLEN 512  //Max length of buffer
#define PORT 8881   //The port on which to listen for incoming data

#define VIB				"A"	//Vibration
#define BLOW			"B"	//Blow
#define	TEMPHIGH		"C"	//TEMP high
#define BATALERT		"D"	//Battery alert
#define DISPON			"E"	//Display ON
#define DISPOFF			"F"	//Display OFF
#define	MOVELEFT		"G"	//Move figure to left
#define	MOVERIGHT		"H"	//Move figure to right
#define	COMEFROMLEFT	"I"	//Come from left
#define	COMEFROMRIGHT	"J"	//Come from right
#define	ACK				"K"	//Acknowledgement
#define FALL			"L"	//Fall

#define CENTRAL_IP   "bbbb::fec2:deff:fe30:ad8e"
#define PORT_SEND	8880


#include "./imu_edison.h"
#include "./display_edison.h"
#include "./ldc_edison.h"
#include "./batgauge_edison.h"

/*************************************/
char straddr[INET6_ADDRSTRLEN];
tContext g_sContext;
/***************************************/


display_edison* m_dsp;



// default config
int m_i2c_bus = 1;
uint8_t m_mpu_address = 0x68;
uint8_t m_dsp_hands   = 3;
int m_log_level = 1;
int m_alert_threshold = 4;


bool m_start_dsp = true;

bool m_colors = true;
bool m_idonly = false;

int m_imu_ID = 0x71;
int m_mag_ID = 0x48;
int m_env_ID = 0x60;
std::string m_ldc_ID = "41:9|8|1";
int m_bat_ID = 3;

/******************************/
void die(char *s)
{
    perror(s);
    exit(1);
}

/***********************/

// definition of subroutines

// display size
Graphics_Display info_display;

//int display_figure (tContext g_sContext, int x_shift);

int display_empty ();

int display_figure ( int x_shift, int h);

int display_right (int x_shift, int h);

int display_vib (int x_shift, int h);

int display_vib2 (int x_shift, int h);

int comefromright(int delay,int xshift_init,int xshift_final,int h);

int comefromleft(int delay,int xshift_init,int xshift_final,int h);

void clearDisplay();

void initialize();

void udpClient6(char *serverIPv6, char *payload, int port);

//_______________________________________________________________________________________________________

int main(int argc, char** argv) {
  
  printf("\n");
  fflush(stdout);

	//edited to test weather the drivers are working

	int delay,xshift_init,xshift_final, centerpos=0;
	
	initialize();		//Function to initialize display

        uint16_t h = Graphics_getDisplayHeight(&g_sContext);
        uint16_t w = Graphics_getDisplayWidth(&g_sContext);
        printf("height= %i width=%i",h,w);
        printf("\n");

	if(h == 128)
	{
		delay=60000;
		xshift_init=-105;
		xshift_final=120;
		centerpos = 0;
	}
	else
	{
		delay=80000;
		xshift_init=-120;
		xshift_final=90;
		centerpos = -15;
	}

	int response_figure;
	
	response_figure = display_figure (centerpos, h);
	clearDisplay();

	//comefromleft(60000, xshift_init, centerpos+15, h);


	/*response_figure = display_figure(g_sContext,-30);
	response_figure = display_figure(g_sContext,-15);
	response_figure = display_figure(g_sContext,0);
	response_figure = display_figure(g_sContext,15);
	*/

//_____________________________________________
    bool success = true;
/***************************************************
                    networking
***************************************************/

    struct sockaddr_in6 si_me, si_other;
    int  NEW = 20;
    int s;
    socklen_t slen = sizeof(si_other);
    int recv_len;
    char buf[BUFLEN];
    char dsp_1[NEW];
    char dsp_2[NEW];   

    //create a UDP socket
    if ((s=socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
  
                                                            
    // zero out the structure                               
    memset((char *) &si_me, 0, sizeof(si_me));              
                                                            
    si_me.sin6_family = AF_INET6;                           
    si_me.sin6_port = htons(PORT);                          
    //si_me.sin6_addr.s6_addr = htonl(INADDR_ANY);          
    //si_me.sin6_addr.s6_addr = htonl({IN6ADDR_ANY_INIT});  
    si_me.sin6_addr = in6addr_any;                           
                                                            
    //bind socket to port                                   
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {                                                            
        die("bind");                                             
    }               
 
    //keep listening for data
    while(1)
     {                                                              
        printf("Waiting for data...");                             
        fflush(stdout);                                            
		memset(buf,'\0',BUFLEN);
                                                            
		memset(dsp_1,'\0',NEW);
		memset(dsp_2,'\0',NEW);
        //try to receive some data, this is a blocking call 
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {                                                        
            die("recvfrom()");                              
        }

	//print the data                                                                         
                                                                                                         
        printf("Data Received: %s\n",  buf);                                                             
                                                                                                 
        printf("Length of buf: %i\n",sizeof(buf));                                                             
        //Graphics_clearDisplay(&g_sContext);                                                              
        //Graphics_flushBuffer(&g_sContext);                                                               
	
	//Display ON subroutine
        if(strcmp(buf, DISPON) == 0)
        {
			int count = 0;
			do{
				//initialize();
				response_figure = display_figure(centerpos,h);
			}
			while(count == 1);
			//sending ACK
			printf("sending ack\n");
			udpClient6(CENTRAL_IP, ACK, PORT_SEND);
			printf("ack sent\n");
        }
        
    //Display OFF subroutine
        else if(strcmp(buf, DISPOFF) == 0)
        {
        	Graphics_clearDisplay(&g_sContext);
        	Graphics_flushBuffer(&g_sContext);
        }

	//Subroutine to make figure come from left
        else if(strcmp(buf, COMEFROMLEFT) == 0)
        {
			int count = 0;
			do{
				comefromleft(delay, xshift_init, centerpos, h);
				count++;
			}
			while(count == 0);
			//sending ACK
			printf("sending ack\n");
			udpClient6(CENTRAL_IP, ACK, PORT_SEND);
			printf("ack sent\n");
		}
        
	//Subroutine to make figure come from right
        else if(strcmp(buf, COMEFROMRIGHT) == 0)
        {
			int count=0;
			do{
				comefromright(delay, centerpos, xshift_final, h);
				count++;
			}
			while(count == 0);
			//sending ACK
			printf("sending ack\n");
			udpClient6(CENTRAL_IP, ACK, PORT_SEND);
			printf("ack sent\n");
		}         
		
	//Subroutine to make figure fall
		else if(strcmp(buf, FALL) == 0)
		{
			int count=0;
			do{
				response_figure = display_figure(centerpos,h);
				usleep(delay*15);
				response_figure = display_vib(centerpos,h);
				usleep(delay*15);
				response_figure = display_vib2(centerpos,h);
				count++;
			}
			while(count == 0);
			//sending ACK
			printf("sending ack\n");
			udpClient6(CENTRAL_IP, ACK, PORT_SEND);
			printf("ack sent\n");
		}
	
	//Subroutine to make figure move left
		else if(strcmp(buf, MOVELEFT) == 0)
		{
			int count =0;
			do{
				comefromright(delay, centerpos-15, xshift_final, h);
				count++;
			}
			while(count == 0);
			printf("sending ack\n");
			udpClient6(CENTRAL_IP, ACK, PORT_SEND);
			printf("ack sent\n");
		}
	
	//Subroutine to make figure move right
		else if(strcmp(buf, MOVERIGHT) == 0)
		{
			int count =0;
			do{
				comefromleft(delay, centerpos+15, xshift_final, h);
				count++;
			}
			while(count == 0);
			printf("sending ack\n");
			udpClient6(CENTRAL_IP, ACK, PORT_SEND);
			printf("ack sent\n");
		}

                                                                                                
/*          // Set up display                                                                              
        if (m_start_dsp)                                                                                 
           m_dsp = new display_edison(m_dsp_hands);                                                      
                                                                                                         
          printf("\n");                                                                             
                                                            
        //print details of the client/peer and the data received                                 
        printf("Received packet from %s:%d\n", inet_ntop(AF_INET6, &si_other.sin6_addr, straddr,INET6_ADDRSTRLEN), ntohs(si_other.sin6_port));
        printf("Data: %s\n" , buf);                         

  // start testing
        bool success = true;
        bool centered = false;
        bool opaque = true;
		
  // test display
  	if (m_start_dsp) {
   	// printY("[TEST] Drawing clock on display...\n");
   	 printf("\n");


// display line by line
           int j=0;
	   m_dsp->clear();
	   for(int i=0;i<=20;i++)
	      dsp_1[i]=buf[i];
           for(int i=21;buf[i]!='\0';i++)
            { dsp_2[j]=buf[i];
	      j++;}
           printf("dsp_1:%s \n",dsp_1);
  	   m_dsp->print(dsp_1,2,10,centered,opaque);
           printf("dsp_2:%s \n",dsp_2);
  	   m_dsp->print(dsp_2,2,20,centered,opaque);
   	   m_dsp->flush();
           strcpy(buf,""); 
           strcpy(dsp_1,""); 
           strcpy(dsp_2,""); 
	   usleep(1000000);
	   m_dsp->clear();	
          // printf("dsp_1:%s \n",dsp_1);
  	   printf("\n");


  	}
*/
}
  close(s);

  if (m_start_dsp)
   { usleep(10);
    delete m_dsp;
   }	
  if (success)
    return 0;
  else
    return 1;
}

//Subroutine for the display of figure


//int display_figure (tContext g_sContext, int x_shift){

int display_empty ()
{
	
}

int display_figure ( int x_shift, int h)
{

if(h == 128)
{
	
Graphics_clearDisplay(&g_sContext);
Graphics_drawCircle(&g_sContext,60+x_shift,30,12); // example for  circle
Graphics_drawLine(&g_sContext,48+x_shift,43,72+x_shift,43); // example for a shoulder
Graphics_drawLine(&g_sContext,66+x_shift,26,67+x_shift,26); // example for a right eye
Graphics_drawLine(&g_sContext,54+x_shift,26,55+x_shift,26); // example for a left eye
Graphics_drawLine(&g_sContext,59+x_shift,33,61+x_shift,33); // example for a mouth 
Graphics_drawLine(&g_sContext,60+x_shift,43,60+x_shift,67); // example for a body
Graphics_drawLine(&g_sContext,60+x_shift,67,58+x_shift,85); // example for a leftleg
Graphics_drawLine(&g_sContext,60+x_shift,67,75+x_shift,85); // example for a right leg  
Graphics_drawLine(&g_sContext,58+x_shift,85,48+x_shift,111); // example for a leftfoot                             
Graphics_drawLine(&g_sContext,75+x_shift,85,70+x_shift,111);//example for a right foot
Graphics_drawLine(&g_sContext,38+x_shift,63,28+x_shift,67); // example for a left hand
Graphics_drawLine(&g_sContext,87+x_shift,28,100+x_shift,28); // example for a right hand
Graphics_drawLine(&g_sContext,48+x_shift,43,38+x_shift,63); // example for a left arm
Graphics_drawLine(&g_sContext,72+x_shift,43,87+x_shift,28); // example for a right arm
Graphics_drawLine(&g_sContext,36+x_shift,111,89+x_shift,111); // example for skateboard
Graphics_drawCircle(&g_sContext,48+x_shift,119,8); // example for first circle
Graphics_drawCircle(&g_sContext,75+x_shift,119,8); // skateboard second circle
Graphics_drawLine(&g_sContext,44+x_shift,7,84+x_shift,7); // example for a solder
Graphics_drawLine(&g_sContext,84+x_shift,7,63+x_shift,5); // example for a solder
Graphics_drawLine(&g_sContext,84+x_shift,7,63+x_shift,9); // example for a solder
Graphics_flushBuffer(&g_sContext);
//usleep(50000);
//Graphics_clearDisplay(&g_sContext);

}

else
{

Graphics_clearDisplay(&g_sContext);
Graphics_drawCircle(&g_sContext,60+x_shift,20,9); // example for  circle
Graphics_drawLine(&g_sContext,48+x_shift,30,72+x_shift,30); // example for a shoulder
Graphics_drawLine(&g_sContext,64+x_shift,18,65+x_shift,18); // example for a right eye
Graphics_drawLine(&g_sContext,55+x_shift,18,56+x_shift,18); // example for a left eye
Graphics_drawLine(&g_sContext,59+x_shift,24,61+x_shift,24); // example for a mouth 
Graphics_drawLine(&g_sContext,60+x_shift,30,60+x_shift,48); // example for a body
Graphics_drawLine(&g_sContext,60+x_shift,48,58+x_shift,66); // example for a leftleg
Graphics_drawLine(&g_sContext,60+x_shift,48,75+x_shift,66); // example for a right leg  
Graphics_drawLine(&g_sContext,58+x_shift,66,48+x_shift,84); // example for a leftfeet                             
Graphics_drawLine(&g_sContext,75+x_shift,66,70+x_shift,84);//example for a right feet
Graphics_drawLine(&g_sContext,38+x_shift,48,30+x_shift,51); // example for a left hand
Graphics_drawLine(&g_sContext,87+x_shift,17,95+x_shift,17); // example for a right hand
Graphics_drawLine(&g_sContext,48+x_shift,30,38+x_shift,48); // example for a left arm
Graphics_drawLine(&g_sContext,72+x_shift,30,86+x_shift,17); // example for a right arm
Graphics_drawLine(&g_sContext,38+x_shift,84,87+x_shift,84); // example for skateboard
Graphics_drawCircle(&g_sContext,48+x_shift,90,5); // example for first circle
Graphics_drawCircle(&g_sContext,75+x_shift,90,5); // skateboard second circle
Graphics_drawLine(&g_sContext,40+x_shift,5,80+x_shift,5); // example for a solder
Graphics_drawLine(&g_sContext,80+x_shift,5,59+x_shift,3); // example for a solder
Graphics_drawLine(&g_sContext,80+x_shift,5,59+x_shift,7); // example for a solder
Graphics_flushBuffer(&g_sContext);
//usleep(50000);
//Graphics_clearDisplay(&g_sContext);

}
return(0);
}

int display_right ( int x_shift, int h)
{

if(h == 128)
{
	
Graphics_clearDisplay(&g_sContext);
Graphics_drawCircle(&g_sContext,60+x_shift,30,12); // example for  circle
Graphics_drawLine(&g_sContext,48+x_shift,43,72+x_shift,43); // example for a shoulder
Graphics_drawLine(&g_sContext,66+x_shift,26,67+x_shift,26); // example for a right eye
Graphics_drawLine(&g_sContext,54+x_shift,26,55+x_shift,26); // example for a left eye
Graphics_drawLine(&g_sContext,59+x_shift,33,61+x_shift,33); // example for a mouth 
Graphics_drawLine(&g_sContext,60+x_shift,43,60+x_shift,67); // example for a body
Graphics_drawLine(&g_sContext,60+x_shift,67,45+x_shift,85); // example for a leftleg
Graphics_drawLine(&g_sContext,60+x_shift,67,62+x_shift,85); // example for a right leg  
Graphics_drawLine(&g_sContext,45+x_shift,85,50+x_shift,111); // example for a left foot                             
Graphics_drawLine(&g_sContext,62+x_shift,85,72+x_shift,111); //example for a right foot
Graphics_drawLine(&g_sContext,26+x_shift,28,34+x_shift,28); // example for a left hand
Graphics_drawLine(&g_sContext,72+x_shift,43,82+x_shift,63); // example for a right hand
Graphics_drawLine(&g_sContext,34+x_shift,28,48+x_shift,43); // example for a left arm
Graphics_drawLine(&g_sContext,82+x_shift,63,92+x_shift,66); // example for a right arm
Graphics_drawLine(&g_sContext,36+x_shift,111,89+x_shift,111); // example for skateboard
Graphics_drawCircle(&g_sContext,48+x_shift,119,8); // example for first circle
Graphics_drawCircle(&g_sContext,75+x_shift,119,8); // skateboard second circle
Graphics_drawLine(&g_sContext,40+x_shift,5,80+x_shift,5); // example for a line of arrow
Graphics_drawLine(&g_sContext,40+x_shift,5,61+x_shift,3); // example for a solder
Graphics_drawLine(&g_sContext,40+x_shift,5,61+x_shift,7); // example for a solder
Graphics_flushBuffer(&g_sContext);
//usleep(50000);
//Graphics_clearDisplay(&g_sContext);
}

else{

Graphics_clearDisplay(&g_sContext);
Graphics_drawCircle(&g_sContext,60+x_shift,20,9); // example for  circle
Graphics_drawLine(&g_sContext,48+x_shift,30,72+x_shift,30); // example for a shoulder
Graphics_drawLine(&g_sContext,64+x_shift,18,65+x_shift,18); // example for a right eye
Graphics_drawLine(&g_sContext,55+x_shift,18,56+x_shift,18); // example for a left eye
Graphics_drawLine(&g_sContext,59+x_shift,24,61+x_shift,24); // example for a mouth 
Graphics_drawLine(&g_sContext,60+x_shift,30,60+x_shift,48); // example for a body
Graphics_drawLine(&g_sContext,60+x_shift,48,45+x_shift,66); // example for a leftleg
Graphics_drawLine(&g_sContext,60+x_shift,48,62+x_shift,66); // example for a right leg  
Graphics_drawLine(&g_sContext,45+x_shift,66,50+x_shift,84); // example for a leftfeet                             
Graphics_drawLine(&g_sContext,62+x_shift,66,72+x_shift,84);//example for a right feet
Graphics_drawLine(&g_sContext,26+x_shift,17,34+x_shift,17); // example for a left hand
Graphics_drawLine(&g_sContext,86+x_shift,48,94+x_shift,51); // example for a right hand
Graphics_drawLine(&g_sContext,48+x_shift,30,34+x_shift,17); // example for a left arm
Graphics_drawLine(&g_sContext,72+x_shift,30,86+x_shift,48); // example for a right arm
Graphics_drawLine(&g_sContext,38+x_shift,84,87+x_shift,84); // example for skateboard
Graphics_drawCircle(&g_sContext,48+x_shift,90,5); // example for first circle
Graphics_drawCircle(&g_sContext,75+x_shift,90,5); // skateboard second circle
Graphics_drawLine(&g_sContext,40+x_shift,5,80+x_shift,5); // example for a line of arrow
Graphics_drawLine(&g_sContext,40+x_shift,5,61+x_shift,3); // example for a solder
Graphics_drawLine(&g_sContext,40+x_shift,5,61+x_shift,7); // example for a solder
Graphics_flushBuffer(&g_sContext);
//usleep(50000);
//Graphics_clearDisplay(&g_sContext);

}
return(0);
}

int display_vib2 ( int x_shift, int h)
{

if(h == 128)
{
	
Graphics_clearDisplay(&g_sContext);
Graphics_drawCircle(&g_sContext,39+x_shift,70,12); // example for  circle
Graphics_drawLine(&g_sContext,39+x_shift,68,39+x_shift,68); // example for a left eye
Graphics_drawLine(&g_sContext,39+x_shift,75,41+x_shift,75); // example for a mouth 
Graphics_drawLine(&g_sContext,39+x_shift,82,39+x_shift,127); // example for a body
Graphics_drawLine(&g_sContext,39+x_shift,127,73+x_shift,108); // example for a leftleg
Graphics_drawLine(&g_sContext,39+x_shift,127,73+x_shift,118); // example for a right leg  
Graphics_drawLine(&g_sContext,73+x_shift,108,101+x_shift,120); // example for a leftfeet  
Graphics_drawLine(&g_sContext,101+x_shift,120,110+x_shift,113);//example for a left feet2                           
Graphics_drawLine(&g_sContext,73+x_shift,118,101+x_shift,127);//example for a right feet
Graphics_drawLine(&g_sContext,101+x_shift,127,110+x_shift,120);//example for a right feet2
Graphics_drawLine(&g_sContext,39+x_shift,92,14+x_shift,119); // example for a left hand
Graphics_drawLine(&g_sContext,39+x_shift,100,14+x_shift,127); // example for a right hand
Graphics_drawLine(&g_sContext,7+x_shift,119,14+x_shift,119); // example for a left arm
Graphics_drawLine(&g_sContext,7+x_shift,127,14+x_shift,127); // example for a right arm
Graphics_flushBuffer(&g_sContext);
//usleep(50000);
//Graphics_clearDisplay(&g_sContext);
}

else
{
	
Graphics_clearDisplay(&g_sContext);
Graphics_drawCircle(&g_sContext,45+x_shift,53,9); // example for  circle
Graphics_drawLine(&g_sContext,45+x_shift,51,45+x_shift,51); // example for a left eye
Graphics_drawLine(&g_sContext,45+x_shift,56,47+x_shift,56); // example for a mouth 
Graphics_drawLine(&g_sContext,45+x_shift,62,45+x_shift,94); // example for a body
Graphics_drawLine(&g_sContext,45+x_shift,94,69+x_shift,80); // example for a leftleg
Graphics_drawLine(&g_sContext,45+x_shift,94,69+x_shift,88); // example for a right leg  
Graphics_drawLine(&g_sContext,69+x_shift,80,93+x_shift,88); // example for a leftfeet  
Graphics_drawLine(&g_sContext,93+x_shift,88,100+x_shift,81);//example for a left feet2                           
Graphics_drawLine(&g_sContext,69+x_shift,88,93+x_shift,94);//example for a right feet
Graphics_drawLine(&g_sContext,93+x_shift,94,100+x_shift,87);//example for a right feet2
Graphics_drawLine(&g_sContext,45+x_shift,70,30+x_shift,88); // example for a left hand
Graphics_drawLine(&g_sContext,45+x_shift,76,30+x_shift,94); // example for a right hand
Graphics_drawLine(&g_sContext,30+x_shift,88,23+x_shift,88); // example for a left arm
Graphics_drawLine(&g_sContext,30+x_shift,94,23+x_shift,94); // example for a right arm
Graphics_flushBuffer(&g_sContext);
//usleep(50000);
//Graphics_clearDisplay(&g_sContext);

}
return(0);

}

int display_vib ( int x_shift, int h)
{

if(h == 128)
{
	
Graphics_clearDisplay(&g_sContext);
Graphics_drawCircle(&g_sContext,45+x_shift,48,12); // example for  circle
//Graphics_drawLine(&g_sContext,48+x_shift,30,72+x_shift,30); // example for a shoulder
Graphics_drawLine(&g_sContext,44+x_shift,44,46+x_shift,44); // example for a right eye
//Graphics_drawLine(&g_sContext,55+x_shift,18,56+x_shift,18); // example for a left eye
Graphics_drawCircle(&g_sContext,49+x_shift,50,2); // example for a mouth 
Graphics_drawLine(&g_sContext,51+x_shift,59,66+x_shift,74); // example for a body
Graphics_drawLine(&g_sContext,66+x_shift,73,88+x_shift,76); // example for a leftleg
Graphics_drawLine(&g_sContext,66+x_shift,73,79+x_shift,92); // example for a right leg  
Graphics_drawLine(&g_sContext,88+x_shift,76,96+x_shift,92); // example for a leftfeet                             
Graphics_drawLine(&g_sContext,79+x_shift,92,77+x_shift,106);//example for a right feet
Graphics_drawLine(&g_sContext,58+x_shift,66,71+x_shift,53); // example for a left hand
Graphics_drawLine(&g_sContext,59+x_shift,66,76+x_shift,61); // example for a right hand
Graphics_drawLine(&g_sContext,94+x_shift,111,127+x_shift,111); // example for skateboard
Graphics_drawCircle(&g_sContext,108+x_shift,119,8); // example for first circle
Graphics_drawCircle(&g_sContext,135+x_shift,119,8); // skateboard second circle
Graphics_flushBuffer(&g_sContext);
//usleep(50000);
//Graphics_clearDisplay(&g_sContext);
}

else{
	
Graphics_clearDisplay(&g_sContext);
Graphics_drawCircle(&g_sContext,50+x_shift,34,9); // example for  circle
//Graphics_drawLine(&g_sContext,48+x_shift,30,72+x_shift,30); // example for a shoulder
Graphics_drawLine(&g_sContext,49+x_shift,30,50+x_shift,30); // example for a right eye
//Graphics_drawLine(&g_sContext,55+x_shift,18,56+x_shift,18); // example for a left eye
Graphics_drawCircle(&g_sContext,54+x_shift,35,2); // example for a mouth 
Graphics_drawLine(&g_sContext,54+x_shift,42,67+x_shift,58); // example for a body
Graphics_drawLine(&g_sContext,67+x_shift,58,85+x_shift,63); // example for a leftleg
Graphics_drawLine(&g_sContext,67+x_shift,58,76+x_shift,73); // example for a right leg  
Graphics_drawLine(&g_sContext,85+x_shift,63,90+x_shift,73); // example for a leftfeet                             
Graphics_drawLine(&g_sContext,76+x_shift,73,74+x_shift,83);//example for a right feet
Graphics_drawLine(&g_sContext,60+x_shift,50,72+x_shift,41); // example for a left hand
Graphics_drawLine(&g_sContext,62+x_shift,52,76+x_shift,48); // example for a right hand
Graphics_drawLine(&g_sContext,82+x_shift,84,120+x_shift,84); // example for skateboard
Graphics_drawCircle(&g_sContext,92+x_shift,90,5); // example for first circle
Graphics_drawCircle(&g_sContext,119+x_shift,90,5); // skateboard second circle
Graphics_flushBuffer(&g_sContext);
//usleep(50000);
//Graphics_clearDisplay(&g_sContext);

}
	return(0);

}

//Function to make the figure shift left
int comefromright(int delay,int xshift_init,int xshift_final,int h)
{
	int response_figure,i = xshift_final;
		while(i != xshift_init)
		{
			response_figure = display_right(i,h);
			usleep(delay);
			i=i-15;
		}
		

	return(0);
}

//Function to make the figure shift right
int comefromleft(int delay,int xshift_init,int xshift_final,int h)
{
	int response_figure, i = xshift_init;

		while(i != xshift_final)
		{
			response_figure = display_figure(i,h);
			usleep(delay);
			i=i+15;
		}
 
	return (0);
}

void clearDisplay()
{
	Graphics_clearDisplay(&g_sContext);
	//Graphics_flushBuffer(&g_sContext);
}

//Function to initialize the display
void initialize()
{
		printf("\n");
        		fflush(stdout);

        		//edited to test weather the drivers are working

        		HAL_LCD_initDisplay();
        		//tContext g_sContext;
        		Graphics_initContext(&g_sContext, &g_sharp96x96LCD);
        		Graphics_setForegroundColor(&g_sContext, ClrWhite);
        		Graphics_setBackgroundColor(&g_sContext, ClrBlack);
        		Graphics_setFont(&g_sContext, &g_sFontFixed6x8);
        		Graphics_clearDisplay(&g_sContext);
        		//Graphics_flushBuffer(&g_sContext);
}

//Client function to send ACK message to central node
void udpClient6(char *serverIPv6, char *payload, int port)
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
  inet_pton(AF_INET6, serverIPv6, &server_addr.sin6_addr);

  /* the port we are going to send to, in network byte order */
  //get the port number from the IP address
  char *sep_ip; 
  char last_segment[20];
  //get the last digits
  printf("IPv6 address of the server: %s\n", serverIPv6);
  /*
  sep_ip = strtok(serverIPv6, "::");
  while (sep_ip != NULL) {                        
    //copy the last segment of the IP address to an array
    strcpy(last_segment, sep_ip);
    sep_ip = strtok(NULL,":");
   }

  //convert it to an integer
  int port_con;

  //unsigned char byte;
  sscanf(last_segment,"%x",&port_con);
  //byte = port_con & 0xFF;

  
  //if (port_con >= 1000) {
    port_con = (port_con/1000) + 64000 + port;
  //}

  printf("Connecting through port: %d\n", port_con);
  */
  server_addr.sin6_port = htons(port);
  //printf("sending back to: %s - using port %d\n", serverIPv6, port);

  /* now send a datagram */
  if (sendto(sock, payload, sizeof(payload), 0,
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
		   INET6_ADDRSTRLEN));*/

  /* close socket */
  close(sock);

  
}
