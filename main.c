#include <cc65.h>
#include <cbm.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>

#include "ultimate_lib.h"
#include "timer.h"
#include "port.h"
#include <dottedquad.h>
#include <errno.h>

// private data/const **************************************************


//#define _DEBUG64
//#define _NO_TCP

/* Home IP and Port */
#define HOME_SRV_DEFAULT  "192.168.188.55"
#define HOME_PORT 64
#define HOME_CLIENT_DEFAULT "ULTIMATE64"

/* IP file name */
#define LOAD_HOME_IP_FILE "0:home_ip,s"
#define SAVE_HOME_IP_FILE "@0:home_ip,s"

/* Name file */
#define LOAD_NAME_FILE "0:name,s"
#define SAVE_NAME_FILE "@0:name,s"

/* Error Exit defines */
#define ERROR_OTHER 0u
#define ERROR_OS    1u
#define ERROR_HOME  2u
#define ERROR_UII   3u
#define ERROR_DEFAULT 4u
#define NO_ERROR     5u

/* rbuf and sbuf for Tcp/Ip. use rbuf also for reading IP file at start and getting IP over keyboard */
static unsigned char rbuf[128];
static unsigned char *bufptr1;
static unsigned char *bufptr2;
#define ipfile  rbuf
#define namefile rbuf
#define keybuf  rbuf
static unsigned char socketID;

#define FlushKeyboardInputBuffer() keybuf[0]=0
#define uii_flush()  rbuf[0]=0 ; /*uii_tcp_emptybuffer()*/ uii_reset_uiidata()


static unsigned char home_srv_config[16] = HOME_SRV_DEFAULT; /* maximum 15 characters + 1 termination */
static unsigned char home_client_config[32] = HOME_CLIENT_DEFAULT;   /* maximum 31 characters + 1 termination */
static uint32_t home_srv;


static uint8_t TempChar;

static uint8_t port_state, port_mask, stop_com;

// public data/const ***************************************************

//private functions
static void load_config(void);
static void uii_connect(void);
static void wait(uint8_t time);
static void change_ip(void);
static void change_name(void);
static void setup_screen(uint8_t screen);
static void setup_network(void);
#define START_SCREEN   0
#define MENU_SCREEN    1


static void error_exit(uint8_t from);
static int uii_tcp_receive(unsigned char socketid, char *result);

void main(void)
{
   //_heapadd ((void *) 0x7000, 0x1000); /* adding 0x1000 heap */

   setup_screen(START_SCREEN);

   load_config();
  
   setup_network();

   start_timer_freerunning_sysclock_32bit(TIM2);

   set_port(PORT2,B, 0xFF); /* All high */
   set_port_direction(PORT2,B,OUTALL);
    
   while(1)
   {
         static uint8_t port = 0xFF;
         static uint8_t pos,i, green, error_flag;
 
         uii_connect();
         
         wait(10);         
         if(stop_com == 1) 
         {
               stop_com = 0;
               continue;
         }
         uii_tcpsocketwrite_ascii(socketID, home_client_config);
         if(!uii_success()){
            error_flag = 1;
         }
         else {
              wait(5);
              if(stop_com == 1)
              {
                error_flag = 1; 
                stop_com = 0;               
              }
              else {
                 while((uii_tcp_nextline_ascii(socketID, rbuf) == 0) && (*rbuf == 0)) {
                 if(!uii_success())
                 {
                    error_flag = 1;
                    break;
                 }
              }
           }

         }
         
         if(!error_flag){
           /* find delimiter ',' and terminate */
           bufptr1 = strchr(rbuf, ',');
           *bufptr1 = 0;
           bufptr1++;

           /* find end \n and terminate */
           bufptr2 = strchr(bufptr1, '\n');
           *bufptr2 = 0;

           port_mask = (uint8_t) atoi(rbuf);
           port_state = (uint8_t) atoi(bufptr1);


           port = port & ~port_mask;
           port |= port_state;
         }
         else {
             error_flag = 0;
         }

         uii_tcpclose(socketID);

         uii_flush();

         set_port(PORT2,B, port);

         /* online */
         gotoxy(0,7);
         (void)revers (1);
         if(green)
         {
            (void)textcolor(COLOR_BLUE);
            green = 0;

         }
         else
         {
            (void)textcolor(COLOR_BLACK);
            green = 1;
         }
         cprintf(" ");
         (void)textcolor(COLOR_GRAY1);
         (void)revers (0);
         
/* print Output States */
         gotoxy(0,8);
         cprintf("Last cmd: mask %3d, state %3d",port_mask, port_state);

         pos = 3;
         for(i=0;i<8;i++)
         {
            gotoxy(pos,24);
            if((port>>i) & 1)
            {
                cprintf("HI");
            }
            else
            {
                cprintf("LO");
            }
            pos+=5;
         }
   }



   clrscr();
   cprintf("Quit and Bye\r\n");
#ifndef _NO_TCP
   uii_tcpclose(socketID);
#endif
}




static void setup_screen(uint8_t screen)
{
   clrscr(); /* clear whole screen */
   (void)textcolor(COLOR_GRAY1);
   (void)bgcolor(COLOR_BLACK);
   (void)bordercolor(COLOR_BLACK);
   (void)revers (1);
   if(screen == START_SCREEN) {
       cprintf("Home64 V1.0          (c) 2020 J.Schwedes");
       (void)revers (0);
       chline(40); /* line horizontal */
       gotoxy(0,23);
       cprintf("UserPort");
       (void)revers (1);
       gotoxy(0,24);
       cprintf("o0:  o1:  o2:  o3:  o4:  o5:  o6:  o7:  ");
       gotoxy(0,19);
       cprintf("Press q to quit                         ");
       gotoxy(0,20);
       cprintf("Press c for configuration menu          ");
       (void)revers (0);

   }
   else if(screen == MENU_SCREEN) {
       cprintf("q=Quit    i=ServerIp    n=My Name       ");
       (void)revers (0);
       chline(40); /* line horizontal */
       gotoxy(0,5);
       TempChar = 0;
       do
       {
            TempChar = cgetc();
           
            switch (TempChar)
            {
               case 'q':
               stop_com = 1;
               TempChar = 0;
               return;
               break;
           
               case 'i':
                 TempChar = 0;
                 stop_com = 1;
                 change_ip();
                 
               break;
           
               case 'n':
                 TempChar = 0;
                 stop_com = 1;
                 change_name();
                 
               break;
               
               default:
                  TempChar = 0;
               break;
            }
           
       } while(TempChar == 0);
       
       
   }
   else {

   }

}


static void error_exit(uint8_t from)
{
  switch (from)
  {
      case ERROR_OS:
        printf("Error: OS $%X\n\r", _oserror);
        break;

      case ERROR_HOME:
        printf("Error: Home\n\r");
        break;

      case ERROR_UII:
        printf("Error: UII %s\n\r", uii_status);
        break;
      
      case NO_ERROR:
         clrscr();
         cprintf("Quit, and good byte");
         break;
      default:
        printf("Error: Unknown\n\r");
        break;
  }

  if(socketID > 0) uii_tcpclose(socketID);

  if (doesclrscrafterexit())
  {
    printf("Press any key\n");
    cgetc();
  }
  exit(1);
}

static void change_ip(void)
{
   printf("Current Home IP: %s \r\n", home_srv_config);
   printf("Change Home IP ? (y/n) \r\n");

   do
   {
       TempChar = cgetc();

       if(TempChar == 'y')
       {
           printf("Enter IP:");
           FlushKeyboardInputBuffer();
           gets(keybuf);
           printf("\r\n");

           memcpy(home_srv_config, keybuf, sizeof(home_srv_config));

           printf("Save new Home IP to file ...\r\n");

           if(cbm_save (SAVE_HOME_IP_FILE, 8, home_srv_config , sizeof(home_srv_config)) > 0)
           {
               /* any os error */
               error_exit(ERROR_OS);
           }

       }
       else
       {
           if(TempChar != 'n')
           {
               TempChar = 0;
           }
       }
   }
   while(TempChar == 0);


   if(!(home_srv = parse_dotted_quad(home_srv_config)))
   {
     printf("Home IP is not valid, please change \r\n", home_srv_config);
     error_exit(ERROR_DEFAULT);
   }
}

static void change_name(void)
{
   printf("Current Name: %s \r\n", home_client_config);
   printf("Change Name ? (y/n) \r\n");

   do
   {
       TempChar = cgetc();

       if(TempChar == 'y')
       {
           printf("Enter Name:");
           FlushKeyboardInputBuffer();
           gets(keybuf);
           printf("\r\n");

           memcpy(home_client_config, keybuf, sizeof(home_client_config));

           printf("Save new Name to file ...\r\n");

           if(cbm_save (SAVE_NAME_FILE, 8, home_client_config , sizeof(home_client_config)) > 0)
           {
               /* any os error */
               error_exit(ERROR_OS);
           }

       }
       else
       {
           if(TempChar != 'n')
           {
               TempChar = 0;
           }
       }
   }
   while(TempChar == 0);
}

static void wait(uint8_t time)
{
    uint8_t stamp;
    set_sysclock_timestamp_65536us_8bit(stamp);
    while(get_sysclock_timedelta_65536us_8bit(stamp) < time)
    {
       if(kbhit() == 1)
       {
           TempChar = cgetc();
           if(TempChar == 'c')
           {
              /* start configuration menu */
              setup_screen(MENU_SCREEN);
              setup_screen(START_SCREEN);
              if(socketID) uii_tcpclose(socketID);
              setup_network();
           } else if(TempChar == 'q')
           {
               error_exit(NO_ERROR);
           }

       }
       TempChar = 0;
    }
}

static void uii_connect(void)
{
    static uint8_t ctr;

    ctr = 0;

    gotoxy(0,6);
    cprintf("                                        ");
    gotoxy(0,6);
    do
    {
       socketID = uii_tcpconnect(home_srv_config, HOME_PORT);
       if(uii_success() && socketID) break;
        
       gotoxy(0,7);
       cprintf(" ");
       wait(5); 
       if(stop_com) break;
       
       if(socketID){
           uii_tcpclose(socketID);
       }
       
       uii_flush();
       
       if(ctr == 40)
       {
          gotoxy(0,6);
          cprintf("                                        ");
          ctr = 0;
       }
       gotoxy(ctr,6);
       cprintf(".");
       ctr++;

    }while(1);
}

static void load_config(void)
{
#ifndef _DEBUG64
   /* Open file with Home IP content */
   if(cbm_load (LOAD_HOME_IP_FILE, 8, ipfile) == 0)
   {
       if(_oserror == 4) /* load: file not found */
       {
           /* save default IP and create file */
           if(cbm_save (SAVE_HOME_IP_FILE, 8, home_srv_config , sizeof(home_srv_config)) > 0)
           {
               /* any os error */
               error_exit(ERROR_OS);
           }
       }
       else
       {
           /* any os error */
           error_exit(ERROR_OS);
       }
   }
   else
   {  /* copy ip to home_srv_config */
      memcpy(home_srv_config, ipfile, sizeof(home_srv_config));
   }

   /* Open file with Name content */
   if(cbm_load (LOAD_NAME_FILE, 8, namefile) == 0)
   {
       if(_oserror == 4) /* load: file not found */
       {
           /* save default IP and create file */
           if(cbm_save (SAVE_NAME_FILE, 8, home_client_config , sizeof(home_client_config)) > 0)
           {
               /* any os error */
               error_exit(ERROR_OS);
           }
       }
       else
       {
           /* any os error */
           error_exit(ERROR_OS);
       }
   }
   else
   {  /* copy ip to home_srv_config */
      memcpy(home_client_config, namefile, sizeof(home_client_config));
   }



#endif
    home_srv = parse_dotted_quad(home_srv_config);
}

static void setup_network(void)
{
   uii_flush();

   gotoxy(0,3);
   
   /* Connect and get socket ID */

   uii_settarget(TARGET_NETWORK);
   uii_identify();
   uii_getipaddress();

   //cprintf("Netmask:    %d.%d.%d.%d\n\r", uii_data[4], uii_data[5], uii_data[6], uii_data[7]);
   //cprintf("Gateway:    %d.%d.%d.%d\n\n\r", uii_data[8], uii_data[9], uii_data[10], uii_data[11]);

   if((uii_data[0] == 0) || !uii_success())
   {
      error_exit(ERROR_UII);
   }

   cprintf("Server IP: %s\n\r", dotted_quad(home_srv));
   cprintf("My IP:     %d.%d.%d.%d\n\r", uii_data[0], uii_data[1], uii_data[2], uii_data[3]);
   cprintf("My Name:   %s\n\r", home_client_config);
}
