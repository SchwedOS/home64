#ifndef __port_h
#define __port_h

#include <stdint.h>

struct __CIA6526_PORT {
    unsigned char       PA;            /* Port register A */
    unsigned char       PB;            /* Port register B */
    unsigned char       DDRA;           /* Data direction register A */
    unsigned char       DDRB;           /* Data direction register B */
};
#define PORT1    (*(struct __CIA6526_PORT*)0xDC00) /* CIA1 */
#define PORT2    (*(struct __CIA6526_PORT*)0xDD00) /* CIA2 */


#define OUT0 1u
#define OUT1 2u
#define OUT2 4u
#define OUT3 8u
#define OUT4 16u
#define OUT5 32u
#define OUT6 64u
#define OUT7 128u

#define OUTALL 255u

#define IN0 0u
#define IN1 0u
#define IN2 0u
#define IN3 0u
#define IN4 0u
#define IN5 0u
#define IN6 0u
#define IN7 0u

#define INALL 0u


#define set_port_direction(port,ch,dir) ##port.DDR##ch = dir
#define get_port(port,ch) ##port.P##ch
#define set_port(port,ch,out) ##port.P##ch = out

#endif