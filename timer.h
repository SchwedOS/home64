#ifndef __timer_h
#define __timer_h

#include <stdint.h>

struct __CIA6526_TIMER {
    uint16_t  TA;             /* Timer A */
    uint16_t  TB;             /* Timer B */
    uint8_t   TOD_10;         /* TOD, 1/10 sec. */
    uint8_t   TOD_SEC;        /* TOD, seconds */
    uint8_t   TOD_MIN;        /* TOD, minutes */
    uint8_t   TOD_HOUR;       /* TOD, hours */
    uint16_t  dummy;
    uint8_t   CRA;            /* Control register A */
    uint8_t   CRB;            /* Control register B */
};
#define TIM1    (*(struct __CIA6526_TIMER*)0xDC04) /* CIA1 */
#define TIM2    (*(struct __CIA6526_TIMER*)0xDD04) /* CIA2 */

/*
CRA Register
------------------

0 START 1   =  STARTTIMER A, 0=STOP TIMER A. This bit is automatically reset when underflow occurs
               during one-shot mode.
1 PBON 1    =  TIMER A output appears on PB6, 0=PB6 normal operation.
2 OUTMODE 1 =  TOGGLE, 0=PULSE
3 RUNMODE 1 =  ON E-SHOT, 0=CONTINUOUS
4 LOAD 1    =  FORCE LOAD (this is a STROBE input, there is no data storage, bit 4 will always read back a
               zero and writing a zero has no effect).
5 INMODE 1  =  TIMER A counts positive CNT transitions, 0=TIMER A counts p2 pulses.
6 SPMODE 1  =  SERIAL PORT output (CNT sources shift clock), 0=SERIAL PORT input (external shift clock
               required).
7 TODIN 1   =  50 Hz clock required on TOD pin for accurate time, 0=60 Hz clock required on TOD pin for
               accurate time.

CRB Register special
------------------------

5,6        =   INMODE Bits CRB5 and CRB6 select one of four input modes for TIMER B as:
               CRB6  CRB5
               0     0    TIMER B count phi pulses.
               0     1    TIMER B counts positive CNT transistions.
               1     0    TIMER B counts TIMER A underflow pulses.
               1     1    TIMER B counts TIMER A underflow pulses while CNT is high.
*/

#define START   1u
#define RUNMODE 8u
#define INMODE  32u

#define INMODE5 32u
#define INMODE6 64u

#define  get_timer_16bit(tim,ch) ((volatile uint16_t)##tim.T##ch)
#define  get_timer_8bit(tim,ch) ((volatile uint8_t)##tim.T##ch)

#define  start_timer_freerunning_sysclock(tim,ch) ##tim.CR##ch &= ~(START | RUNMODE | INMODE); ##tim.CR##ch |= (START)

/* Timer B counts Timer A sysclock underflow (32-bit): timer A loword, timer B hiword */
#define  start_timer_freerunning_sysclock_32bit(tim) ##tim.CRA &= ~(START | RUNMODE | INMODE);##tim.CRB &= ~(START | RUNMODE | INMODE5 | INMODE6); \
                                                     ##tim.CRB |= (START | INMODE6); ##tim.CRA |= (START)

/* configuartion of sysclock source cia and timer */

#define set_sysclock_timestamp_1us_16bit(time)  (time) = get_timer_16bit(TIM2,A)
#define get_sysclock_timedelta_1us_16bit(time)  ((uint16_t)((time)-get_timer_16bit(TIM2,A)))

#define set_sysclock_timestamp_65536us_16bit(time)  (time) = get_timer_16bit(TIM2,B)
#define get_sysclock_timedelta_65536us_16bit(time)  ((uint16_t)((time)-get_timer_16bit(TIM2,B)))

#define set_sysclock_timestamp_65536us_8bit(time)  (time) = get_timer_8bit(TIM2,B)
#define get_sysclock_timedelta_65536us_8bit(time)  ((uint8_t)((time)-get_timer_8bit(TIM2,B)))

#endif