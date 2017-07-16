/*
  CodeVisionAVR C Compiler
  (C) 2008 Taras Drozdovsky, My.
*/

#ifndef _BOARD_INCLUDED_
#define _BOARD_INCLUDED_

#include <mega1280.h>

#define CRCEH       4095
#define CRCEL       4094
#define HND         4081

#define LED_ON      PORTG.2=0;
#define LED_OFF     PORTG.2=1;
#define LED_TOD     PORTG.2=~PORTG.2;

#define LED1_ON     PORTA.0=0;
#define LED1_OFF    PORTA.0=1;
#define LED1_TOD    PORTA.0=~PORTA.0;

#define LED2_ON     PORTA.1=0;
#define LED2_OFF    PORTA.1=1;
#define LED2_TOD    PORTA.1=~PORTA.1;

#define LED3_ON     PORTA.2=0;
#define LED3_OFF    PORTA.2=1;
#define LED3_TOD    PORTA.2=~PORTA.2;

#define LED4_ON     PORTA.3=0;
#define LED4_OFF    PORTA.3=1;
#define LED4_TOD    PORTA.3=~PORTA.3;

#define LED5_ON     PORTA.4=0;
#define LED5_OFF    PORTA.4=1;
#define LED5_TOD    PORTA.4=~PORTA.4;

#define LED6_ON     PORTA.5=0;
#define LED6_OFF    PORTA.5=1;
#define LED6_TOD    PORTA.5=~PORTA.5;

#define LED7_ON     PORTA.6=0;
#define LED7_OFF    PORTA.6=1;
#define LED7_TOD    PORTA.6=~PORTA.6;

#define LED8_ON     PORTA.7=0;
#define LED8_OFF    PORTA.7=1;
#define LED8_TOD    PORTA.7=~PORTA.7;

#define BUZ_ON      PORTC.4=1;  
#define BUZ_OFF     PORTC.4=0;      
#define BUZ_TOG     PORTC.4=~PORTC.4;  

#define H_SIM_ON    PORTC.5=1;
#define H_SIM_OFF   PORTC.5=0;

#define LED_RD_OFF  PORTH&=~0x02;
#define LED_RD_ON   PORTH|=0x02;

#define BEEP_RD_OFF  PORTH&=~0x04;
#define BEEP_RD_ON PORTH|=0x04;

// Declare SoftWare Interrupt

#define SI_GEN      PORTE.4=1;
#define SI_RST      PORTE.4=0;

#define SI_RFID     (1<<0)
#define SI_UPBW     (1<<1) 
#define SI_MENU     (1<<2)
#define SI_PLAY_LIST (1<<3)

#define BUTT_TYPE   0x08
#define RFID_TYPE   0x09
#define DTMF_TYPE   0x0A

// Declare PIN POWER SIM

#define S_RING      (1<<0)
#define S_SMS       (1<<1)
#define S_CRDY      (1<<2)
#define S_POWERDOWN (1<<3)
#define S_ERROR     (1<<4) 
#define S_CREG      (1<<5)
#define S_CREGNOT   (1<<6)

#define SIM_ON    PORTD.5
#define VDD_EXT   PIND.4
      
#define BUT       (PINK&0x01)
#define NUMA_NUMBER 5

#define _CS PORTB.0

#define DPORT_CS DDRB.0=1;
#define SCK 1
#define MOSI 2
#define MISO 3

//  define PWM
#define OCREG           OCR5AL
#define NUMMELODY       128          

#define RELAY1_ON       PORTH|=0x40;                      // relay-1 on
#define RELAY1_OFF      PORTH&=~0x40;                      // relay-1 off
#define RELAY2_ON       PORTH|=0x20;                      // relay-2 on
#define RELAY2_OFF      PORTH&=~0x20;                      // relay-2 off
#define OPENDRAIN1_OFF  PORTH&=~0x10;
#define OPENDRAIN1_ON   PORTH|=0x10;
#define OPENDRAIN2_OFF  PORTH&=~0x08;
#define OPENDRAIN2_ON   PORTH|=0x08;

#define CS PORTB._CS                                      
      

#define rx_buffer_overflowGSM rx_buffer_overflow1
#define rx_counterGSM  rx_counter1
#define tx_counterGSM  tx_counter1
#define putcharGSM     putchar1
#define getcharGSM     getchar1

#define rx_buffer_overflowPC rx_buffer_overflow0
#define rx_counterPC  rx_counter0
#define tx_counterPC  tx_counter0
#define putcharPC     putchar
#define getcharPC     getchar

#define rx_buffer_overflowM rx_buffer_overflow2
#define rx_counterM  rx_counter2
#define getcharM     getchar2

// Possible respons after executant command
#define C_OK              0             
#define C_ERROR           2
#define C_REG_NET         8
#define C_NOT_REG_NET     9
    
#endif         