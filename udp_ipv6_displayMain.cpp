/*
* Author: Manisha
* 
* Display figure and msg using udp_ipv6
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
#define PORT 8888   //The port on which to listen for incoming data


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

int display_figure ( int x_shift);

//_______________________________________________________________________________________________________

int main(int argc, char** argv) {
  
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
	// display hight and width of the used disply
	uint16_t h =Graphics_getDisplayHeight(&g_sContext);
	uint16_t w =Graphics_getDisplayWidth(&g_sContext);
	printf("height= %i width=%i",h,w);
	printf("\n");
	int response_figure;

	response_figure = display_figure(-30);
	usleep(50000); 
	response_figure = display_figure(-15);
	usleep(50000); 

	response_figure = display_figure(15);
	usleep(50000);
	response_figure = display_figure(30);
	usleep(50000);
	response_figure = display_figure(45);
	usleep(50000);
	response_figure = display_figure(60);
	usleep(50000);
	response_figure = display_figure(75);
	usleep(50000);
	response_figure = display_figure(90);
	usleep(50000);
	response_figure = display_figure(105);
	usleep(50000);

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
    int s, i, slen = sizeof(si_other) , recv_len;
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
                                                                                                         
          // Set up display                                                                              
        if (m_start_dsp)                                                                                 
           m_dsp = new display_edison(m_dsp_hands);                                                      
                                                                                                         
          printf("\n");                                                                             
                                                            
/*        //print details of the client/peer and the data received                                 
        printf("Received packet from %s:%d\n", inet_ntop(AF_INET6, &si_other.sin6_addr, straddr,INET6_ADDRSTRLEN), ntohs(si_other.sin6_port));
        printf("Data: %s\n" , buf);                         
*/
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

int display_figure ( int x_shift){


Graphics_clearDisplay(&g_sContext);
Graphics_drawCircle(&g_sContext,60+x_shift,22,12); // example for  circle
Graphics_drawLine(&g_sContext,48+x_shift,35,72+x_shift,35); // example for a solder
Graphics_drawLine(&g_sContext,66+x_shift,18,67+x_shift,18); // example for a right eye
Graphics_drawLine(&g_sContext,54+x_shift,18,55+x_shift,18); // example for a left eye
Graphics_drawLine(&g_sContext,59+x_shift,25,61+x_shift,25); // example for a mouth 
Graphics_drawLine(&g_sContext,60+x_shift,35,60+x_shift,55); // example for a body
Graphics_drawLine(&g_sContext,60+x_shift,55,58+x_shift,75); // example for a leftleg
Graphics_drawLine(&g_sContext,60+x_shift,55,75+x_shift,75); // example for a right leg  
Graphics_drawLine(&g_sContext,58+x_shift,75,48+x_shift,95); // example for a leftfeet                             
Graphics_drawLine(&g_sContext,75+x_shift,75,70+x_shift,95);//example for a right feet
Graphics_drawLine(&g_sContext,38+x_shift,55,28+x_shift,59); // example for a left hand
Graphics_drawLine(&g_sContext,87+x_shift,20,100+x_shift,20); // example for a right hand
Graphics_drawLine(&g_sContext,48+x_shift,35,38+x_shift,55); // example for a left arm
Graphics_drawLine(&g_sContext,72+x_shift,35,87+x_shift,20); // example for a right arm
Graphics_drawLine(&g_sContext,38+x_shift,95,87+x_shift,95); // example for skateboard
Graphics_drawCircle(&g_sContext,48+x_shift,103,8); // example for first circle
Graphics_drawCircle(&g_sContext,75+x_shift,103,8); // skateboard second circle
Graphics_drawLine(&g_sContext,48+x_shift,5,88+x_shift,5); // example for a solder
Graphics_drawLine(&g_sContext,88+x_shift,5,67+x_shift,3); // example for a solder
Graphics_drawLine(&g_sContext,88+x_shift,5,67+x_shift,7); // example for a solder
Graphics_flushBuffer(&g_sContext);
//usleep(50000);
//Graphics_clearDisplay(&g_sContext);

return(0);
}
