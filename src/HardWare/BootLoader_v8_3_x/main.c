/*****************************************************
This program was produced by the
CodeWizardAVR V1.25.3 Standard
Automatic Program Generator
© Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : BootLoader ASCII
Version : v8.3.x
Date    : 21.08.2008
Author  : T.Drozdovsky                            
Company : My                            
Comments: 


Chip type           : ATmega1280
Program type        : Application
Clock frequency     : 14,745600 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 2048
*****************************************************/
                                                                          
#include <mega1280.h>              
#include <string.h>    
#include <delay.h>
#include <g28147.h>
#include <board.h>

#define RXB8 1
#define TXB8 0
#define UPE 2
#define OVR 3
#define FE 4
#define UDRE 5
#define RXC 7

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<OVR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

#define SPMEN  0
#define PGERS  1
#define PGWRT  2
#define BLBSET 3
#define RWWSRE 4
#define SIRGD  5
#define RWWSB  6
#define SPMIE  7
#define EEWE   1 
#define EEPE   1                          

flash char FHandContr[]={"SYHND 00016"};                    // Get Handle Controller
#define VERSIA_BOOT_LOADER  "\r\nBootLoader v8.3.x"

/*****************************************************************************/
/*                G L O B A L    V A R I A B L E S                           */
/*****************************************************************************/
#pragma warn-

register unsigned int  Buffer   @0;
register unsigned int  reg_temp @2;

flash unsigned char *mas;

union {                                                 // union for conversion type long,int in char
  unsigned char byte[4];
  unsigned int word[2];
  unsigned long dword;      
 } number;

union {                                                 // union for check sum (interrupt)
 unsigned char crcb[2];
 unsigned int crcw;      
      } crci;      

/*****************************************************************************/
/*                 G O S T   2 8 1 4 7 - 8 9                                 */
/*****************************************************************************/

unsigned long S[2];

unsigned long KZU[8]={0xF416D687,0x47E38B0E,0xCB7FAB72,0xC442F690,  // KEY
                      0xB928436A,0x7F951ABB,0x1569CE34,0xCD8ED736};   
                      
       
unsigned char K[128]={       // Gam_c
   0x0F, 0x04, 0x03, 0x06, 0x0D, 0x02, 0x09, 0x08, 0x00, 0x0C, 0x01, 0x0E, 0x07, 0x05, 0x0A, 0x0B,
   0xE0, 0x30, 0xC0, 0x20, 0xD0, 0x90, 0x10, 0xB0, 0x50, 0x40, 0x60, 0xA0, 0x00, 0xF0, 0x80, 0x70,
   0x04, 0x08, 0x0C, 0x01, 0x0E, 0x07, 0x00, 0x0F, 0x0B, 0x02, 0x09, 0x06, 0x05, 0x03, 0x0A, 0x0D,
   0x10, 0x90, 0x80, 0x50, 0xE0, 0xF0, 0xA0, 0xB0, 0x00, 0xC0, 0x70, 0x30, 0xD0, 0x60, 0x20, 0x40,
   0x09, 0x03, 0x04, 0x0C, 0x01, 0x00, 0x0B, 0x05, 0x06, 0x0F, 0x0D, 0x0E, 0x07, 0x02, 0x0A, 0x08,
   0xE0, 0x40, 0x90, 0xD0, 0xA0, 0x30, 0xF0, 0x10, 0xB0, 0x70, 0x80, 0x50, 0x60, 0xC0, 0x20, 0x00,
   0x02, 0x0A, 0x06, 0x0E, 0x05, 0x0D, 0x0C, 0x03, 0x07, 0x08, 0x0F, 0x00, 0x09, 0x0B, 0x01, 0x04,
   0xB0, 0x90, 0x80, 0x10, 0xA0, 0x60, 0xD0, 0x40, 0x00, 0x50, 0x20, 0xC0, 0xE0, 0xF0, 0x30, 0x70};
/*****************************************************************************/

unsigned char kkkk[]="Not correct CRC check sum";

#pragma warn+ 
#define  PageByte       256                              
#define  EEPROMByte     4096                             
eeprom unsigned char *pEEPROM;

unsigned int counter_PC=0;
unsigned char buffer_PC[1024];

bit StartFlagPC=0;        

unsigned char BootHere[]="SBLFL";   


/*****************************************************************************/
/*                  F U N C T I O N  P R O T O T Y P E                       */
/*****************************************************************************/
void Receive_PC(void);
void ProgramFlashPage(unsigned char *);
void ProgramEEPROM(unsigned char *);
void InitSign(unsigned char *);

char hextocharh(unsigned char);        
char hextocharl(unsigned char);        
unsigned char chartohexh(unsigned char);
unsigned char chartohexl(unsigned char);
void SendAnswerR(char *);
        
void crc2(unsigned char *,unsigned int *,unsigned int);
unsigned char check_flash_crc(void);
void WriteFlash(unsigned int P_address,unsigned char *pData);  
void ExecCode(void);

/*****************************************************************************/  

// USART0 Receiver buffer
#define RX_BUFFER_SIZE0 2048
char rx_buffer0[RX_BUFFER_SIZE0];

#if RX_BUFFER_SIZE0<256
unsigned char rx_wr_index0=0,rx_rd_index0=0,rx_counter0=0;
#else
unsigned int rx_wr_index0=0,rx_rd_index0=0,rx_counter0=0;
#endif

// This flag is set on USART0 Receiver buffer overflow
bit rx_buffer_overflow0;

// USART0 Receiver interrupt service routine
interrupt [USART0_RXC] void usart0_rx_isr(void)
{
char status,data;
status=UCSR0A;
data=UDR0;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer0[rx_wr_index0]=data;
   if (++rx_wr_index0 == RX_BUFFER_SIZE0) rx_wr_index0=0;
   if (++rx_counter0 == RX_BUFFER_SIZE0)
      {
      rx_counter0=0;
      rx_buffer_overflow0=1;
      };
   };
}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART0 Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+
char getchar(void)
{
char data;
while (rx_counter0==0);
data=rx_buffer0[rx_rd_index0];
if (++rx_rd_index0 == RX_BUFFER_SIZE0) rx_rd_index0=0;
#asm("cli")
--rx_counter0;
#asm("sei")
return data;
}
#pragma used-
#endif

// USART0 Transmitter buffer
#define TX_BUFFER_SIZE0 512
char tx_buffer0[TX_BUFFER_SIZE0];

#if TX_BUFFER_SIZE0<256
unsigned char tx_wr_index0=0,tx_rd_index0=0,tx_counter0=0;
#else
unsigned int tx_wr_index0=0,tx_rd_index0=0,tx_counter0=0;
#endif

// USART0 Transmitter interrupt service routine
interrupt [USART0_TXC] void usart0_tx_isr(void)
{
if (tx_counter0)
   {
   --tx_counter0;
   UDR0=tx_buffer0[tx_rd_index0];
   if (++tx_rd_index0 == TX_BUFFER_SIZE0) tx_rd_index0=0;
   };
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART0 Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
while (tx_counter0 == TX_BUFFER_SIZE0);
#asm("cli")
if (tx_counter0 || ((UCSR0A & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer0[tx_wr_index0]=c;
   if (++tx_wr_index0 == TX_BUFFER_SIZE0) tx_wr_index0=0;
   ++tx_counter0;
   }
else
   UDR0=c;
#asm("sei")
}
#pragma used-
#endif

// Standard Input/Output functions
#include <stdio.h>
unsigned char Led_R=0xE0;
 
// Timer 1 output compare A interrupt service routine
interrupt [TIM1_COMPA] void timer1_compa_isr(void)
{
// Place your code here
    //LEDS_TOG
    if(Led_R&0x01){Led_R>>=1; Led_R|=0x80;}
    else Led_R>>=1;   
    PORTA=~Led_R;
    LED_TOD    
}

// Declare your global variables here

void main(void)
{
// Declare your local variables here
 unsigned char i;
 pEEPROM=0;
// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif
// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0xFF;
DDRA=0xFF;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTB=0x00;
DDRB=0x00;

// Port C initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Port E initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTE=0x00;
DDRE=0x00;

// Port F initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTF=0x00;
DDRF=0x00;

// Port G initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTG=0x00;
DDRG=0x04;

// Port H initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTH=0x00;
DDRH=0x00;

// Port J initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTJ=0x00;
DDRJ=0x00;

// Port K initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTK=0x00;
DDRK=0x00;

// Port L initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTL=0x00;
DDRL=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=FFh
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x00;
TCCR0B=0x00;
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 10,800 kHz
// Mode: CTC top=OCR1A
// OC1A output: Discon.
// OC1B output: Discon.
// OC1C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: On
// Compare B Match Interrupt: On
// Compare C Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x0D;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x06;
OCR1AL=0x50;
OCR1BH=0x00;
OCR1BL=0x00;
OCR1CH=0x00;
OCR1CL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer 2 Stopped
// Mode: Normal top=FFh
// OC2A output: Disconnected
// OC2B output: Disconnected
ASSR=0x00;
TCCR2A=0x00;
TCCR2B=0x00;
TCNT2=0x00;
OCR2A=0x00;
OCR2B=0x00;

// Timer/Counter 3 initialization
// Clock source: System Clock
// Clock value: Timer 3 Stopped
// Mode: Normal top=FFFFh
// Noise Canceler: Off
// Input Capture on Falling Edge
// OC3A output: Discon.
// OC3B output: Discon.
// OC3C output: Discon.
// Timer 3 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR3A=0x00;
TCCR3B=0x00;
TCNT3H=0x00;
TCNT3L=0x00;
ICR3H=0x00;
ICR3L=0x00;
OCR3AH=0x00;
OCR3AL=0x00;
OCR3BH=0x00;
OCR3BL=0x00;
OCR3CH=0x00;
OCR3CL=0x00;

// Timer/Counter 4 initialization
// Clock source: System Clock
// Clock value: Timer 4 Stopped
// Mode: Normal top=FFFFh
// OC4A output: Discon.
// OC4B output: Discon.
// OC4C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 4 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR4A=0x00;
TCCR4B=0x00;
TCNT4H=0x00;
TCNT4L=0x00;
ICR4H=0x00;
ICR4L=0x00;
OCR4AH=0x00;
OCR4AL=0x00;
OCR4BH=0x00;
OCR4BL=0x00;
OCR4CH=0x00;
OCR4CL=0x00;

// Timer/Counter 5 initialization
// Clock source: System Clock
// Clock value: Timer 5 Stopped
// Mode: Normal top=FFFFh
// OC5A output: Discon.
// OC5B output: Discon.
// OC5C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 5 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR5A=0x00;
TCCR5B=0x00;
TCNT5H=0x00;
TCNT5L=0x00;
ICR5H=0x00;
ICR5L=0x00;
OCR5AH=0x00;
OCR5AL=0x00;
OCR5BH=0x00;
OCR5BL=0x00;
OCR5CH=0x00;
OCR5CL=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
// INT3: Off
// INT4: Off
// INT5: Off
// INT6: Off
// INT7: Off
EICRA=0x00;
EICRB=0x00;
EIMSK=0x00;
// PCINT0 interrupt: Off
// PCINT1 interrupt: Off
// PCINT2 interrupt: Off
// PCINT3 interrupt: Off
// PCINT4 interrupt: Off
// PCINT5 interrupt: Off
// PCINT6 interrupt: Off
// PCINT7 interrupt: Off
// PCINT8 interrupt: Off
// PCINT9 interrupt: Off
// PCINT10 interrupt: Off
// PCINT11 interrupt: Off
// PCINT12 interrupt: Off
// PCINT13 interrupt: Off
// PCINT14 interrupt: Off
// PCINT15 interrupt: Off
// PCINT16 interrupt: Off
// PCINT17 interrupt: Off
// PCINT18 interrupt: Off
// PCINT19 interrupt: Off
// PCINT20 interrupt: Off
// PCINT21 interrupt: Off
// PCINT22 interrupt: Off
// PCINT23 interrupt: Off
PCMSK0=0x00;
PCMSK1=0x00;
PCMSK2=0x00;
PCICR=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=0x00;
// Timer/Counter 1 Interrupt(s) initialization
TIMSK1=0x02;
// Timer/Counter 2 Interrupt(s) initialization
TIMSK2=0x00;
// Timer/Counter 3 Interrupt(s) initialization
TIMSK3=0x00;
// Timer/Counter 4 Interrupt(s) initialization
TIMSK4=0x00;
// Timer/Counter 5 Interrupt(s) initialization
TIMSK5=0x00;

// USART0 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART0 Receiver: On
// USART0 Transmitter: On
// USART0 Mode: Asynchronous
// USART0 Baud rate: 115200
UCSR0A=0x00;
UCSR0B=0xD8;
UCSR0C=0x06;
UBRR0H=0x00;
UBRR0L=0x05;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
ADCSRB=0x00;

#asm("sei")       // Global enable interrupts

printf(VERSIA_BOOT_LOADER);

for(i=0;i!=11;i++)
{
 if(FHandContr[i]!=(*(pEEPROM+4081+i)))
  *(pEEPROM+4081+i)=FHandContr[i];
} 
printf("\r\nS/N ");
printf(FHandContr);

if(check_flash_crc())
{
 printf("\r\nCRC OK");
 while(tx_counter0);
 delay_ms(12);
 ExecCode();
}
else 
{
 printf("\r\nCRC ERROR\r\n"); 
}
SendAnswerR(BootHere);

while (1)                                                          
      {
      // Place your code here
      if(rx_counter0) Receive_PC();
      };
}
unsigned char check_flash_crc(void)
{
 unsigned char mas1[256];
 unsigned int i;  
 unsigned int j;
 unsigned int k; 
 unsigned char a;
 crci.crcw=0;
 a=RAMPZ;
 RAMPZ=0x00;     
 while(EECR & (1<<EEPE));
 if((*(pEEPROM+4092))>7) {   
        RAMPZ=a;
        return 0;                        }
 k=(*(pEEPROM+4092));
 k<<=8; 
 k|=(*(pEEPROM+4093));
 for(j=0;j!=k;j++)
 {
  for(i=0;i!=256;i++)
   mas1[i]=*(mas+i+j*256);
  crc2(&mas1[0],&crci.crcw,256);
 }              
 while(EECR & (1<<EEPE));
 if((crci.crcb[1]==(*(pEEPROM+4094)))&&(crci.crcb[0]==(*(pEEPROM+4095))))
 {
  RAMPZ=a;
  return 1;
 }
 RAMPZ=a;  
 return 0;
}

/**
 * Send answer with Flash memory string.  
 *
 * @param	*data	a pointer to the string command 
 **/
void SendAnswerR(char *data){
 unsigned char i=0;
 unsigned char checkbyte=0;
 putchar('$');
 do {     
   #asm("wdr")
   checkbyte^=data[i];
   putchar(data[i]);
  }while(data[++i]); 
   putchar('*');
   putchar(hextocharh(checkbyte));
   putchar(hextocharl(checkbyte));
   putchar('\n');     
}

/**
 * Receive data from PC and shaping packet  
 **/
void Receive_PC(void)
{
 unsigned char data;     
 #asm("wdr")
 data=getchar();
  switch (data) {
    case '$': { StartFlagPC=1; counter_PC=0;}
    break;
    case '\n': 
        if(StartFlagPC)
        {
         StartFlagPC=0;
         data=0;                  
         counter_PC=0;
         while((buffer_PC[counter_PC] != '*')&&(buffer_PC[counter_PC] != '\n') )	  
         data^=buffer_PC[counter_PC++];                      
         if(buffer_PC[counter_PC]=='*')
         {      
          if((hextocharh(data)==buffer_PC[++counter_PC])&&(hextocharl(data)==buffer_PC[++counter_PC]))
          {
           if(!strncmpf(buffer_PC, "PSIGN", 5))
	   {
	    InitSign(&buffer_PC[5]); 
	   }
	   if(!strncmpf(buffer_PC, "PINFO", 5))
	   {                           
	    ;
	   }
	   if(!strncmpf(buffer_PC, "PFLSH", 5))
	   {         
	    ProgramFlashPage(&buffer_PC[5]);
	   }
 	   if(!strncmpf(buffer_PC, "PBLFL", 5))
 	   {         
            Gost_init(&S[0]);
            SendAnswerR(BootHere);	    
 	   }
           if(!strncmpf(buffer_PC, "PEEPR", 5))
	   {
	    ProgramEEPROM(&buffer_PC[5]);
           } 
          }
          else
          {
           SendAnswerR(kkkk);
          }
         } 
         else
         {
          ;//SendAnswerR(kkkk);
         }
    }
    break;
    default: 
        if(StartFlagPC) buffer_PC[counter_PC++]=data;  
    };
}                                   
void InitSign(unsigned char *mas)
{
 unsigned char answerC[]="SSIGNOK";
 number.byte[3]=(chartohexh(mas[0]))|(chartohexl(mas[1]));
 number.byte[2]=(chartohexh(mas[2]))|(chartohexl(mas[3]));
 number.byte[1]=(chartohexh(mas[4]))|(chartohexl(mas[5]));
 number.byte[0]=(chartohexh(mas[6]))|(chartohexl(mas[7]));
 S[0]=number.dword;
 number.byte[3]=(chartohexh(mas[8]))|(chartohexl(mas[9]));
 number.byte[2]=(chartohexh(mas[10]))|(chartohexl(mas[11]));
 number.byte[1]=(chartohexh(mas[12]))|(chartohexl(mas[13]));
 number.byte[0]=(chartohexh(mas[14]))|(chartohexl(mas[15]));
 S[1]=number.dword;
 Gost_init(&S[0]);
 SendAnswerR(answerC);

}
void ProgramFlashPage(unsigned char *mas)
{           
 unsigned int addr;
 unsigned char kkk[]="SFLSH0000OK";
 addr=chartohexh(mas[0]);
 kkk[5]=mas[0];
 addr|=chartohexl(mas[1]);
 kkk[6]=mas[1];
 addr<<=8;
 addr|=chartohexh(mas[2]);
 kkk[7]=mas[2];
 addr|=chartohexl(mas[3]);                         
 kkk[8]=mas[3];                 
 WriteFlash(addr,&buffer_PC[9]);
 SendAnswerR(kkk);
}
void ProgramEEPROM(unsigned char *mas)
{	    
 unsigned int addr;
 unsigned int lenght;
 unsigned int i;  
 unsigned char s;
 unsigned char kkk[]="SEEPR0000OK";
 PORTF=0xC0;
 addr=chartohexh(mas[0]);
 kkk[5]=mas[0];
 addr|=chartohexl(mas[1]);
 kkk[6]=mas[1];
 addr<<=8;
 addr|=chartohexh(mas[2]);
 kkk[7]=mas[2];
 addr|=chartohexl(mas[3]);                         
 kkk[8]=mas[3];
 lenght=chartohexh(mas[4]);
 lenght|=chartohexl(mas[5]);
 lenght<<=8;
 lenght|=chartohexh(mas[6]);
 lenght|=chartohexl(mas[7]);               
 for(i=0;i!=lenght;i++)
 {                
  s=chartohexh(mas[8+(i<<1)]);
  s|=chartohexl(mas[9+(i<<1)]);
  *(pEEPROM+addr+i)=s;
 }
 if(addr==0x0FFC)
 {
  SendAnswerR(kkk);
  while(tx_counter0);
  delay_ms(12);
  #asm("cli") 
 MCUCR=0x01;                         //interrupts vectors are in the boot sector
 MCUCR=0x02;
 #asm("jmp 0xF000");                 // Run application code
 }                                         
  SendAnswerR(kkk);
}

void WriteFlash(unsigned int P_address,unsigned char *pData)
{ 
 unsigned int TempInt;
 unsigned int i,j;
 unsigned char a;
 
 for(i=0;i!=256;i++)
 {
  a=chartohexh(pData[(i<<1)]);
  a|=chartohexl(pData[(i<<1)+1]);
  pData[i]=a;
 }
 for(i=0;i!=64;i++)
 {           
  a=pData[i*4];
  pData[i*4]=pData[i*4+3];
  pData[i*4+3]=a;

  a=pData[i*4+1];
  pData[i*4+1]=pData[i*4+2];
  pData[i*4+2]=a;  
 }  
 Gam_cD(&pData[0],&KZU[0],&K[0],32);
 for(i=0;i!=64;i++)
 {           
  a=pData[i*4];
  pData[i*4]=pData[i*4+3];
  pData[i*4+3]=a;

  a=pData[i*4+1];
  pData[i*4+1]=pData[i*4+2];
  pData[i*4+2]=a;  
 }  
 
 #asm("cli");            
 while(EECR & (1<<EEPE));
 a=RAMPZ;
 if(P_address<256)RAMPZ=0x00; 
 else RAMPZ=0x01;
 reg_temp = P_address<<8;
 SPMCSR|=(1<<PGERS) | (1<<SPMEN);
 #asm  
     mov r31,r3
     mov r30,r2
     spm
 #endasm
 while(SPMCSR&(1<<SPMEN));        // Polled to find out when the CPU is ready for further page updates
 SPMCSR|=(1<<RWWSRE)|(1<<SPMEN);  // RWWSB Flag cleared by software when the SPM operation is completed.
 #asm("spm");
 RAMPZ=a;
 #asm("sei");
 for (i=0,j=0;j!=256;i+=2,j+=2)
 { 
  #pragma warn-         
  TempInt=*(pData+i+1);
  TempInt<<=8;
  TempInt|=*(pData+i);
  Buffer=TempInt;
  reg_temp = j;
  SPMCSR =(1<<SPMEN);
  #asm  
     mov r31,r3
     mov r30,r2
     spm
  #endasm
  #pragma warn+
 }
 #asm("cli");
 a=RAMPZ;
 if(P_address<256)RAMPZ=0x00; 
 else RAMPZ=0x01;
 reg_temp = P_address<<8;;
 SPMCSR|=(1<<PGWRT) | (1<<SPMEN);
 #asm  
    mov r31,r3
    mov r30,r2
    spm
 #endasm
 while(SPMCSR&(1<<SPMEN));        // Polled to find out when the CPU is ready for further page updates
 SPMCSR|=(1<<RWWSRE)|(1<<SPMEN);  // RWWSB Flag cleared by software when the SPM operation is completed.
 #asm("spm");
 RAMPZ=a;
 #asm("sei");
}

inline void ExecCode(void)
{
 MCUCR=0x01;                         //interrupts vectors are in the boot sector
 MCUCR=0x00;
 #asm("jmp 0x0000");                 // Run application code
}

/*****************************************************************************/

/**
 * Calculated CRC checksum 
 *
 * @param	*ZAdr	a pointer to the data 
 * @param	*DoAdr	a pointer to the CRC checksum returned
 * @param	lle	amount byte need for calculate CRC checksum
 **/
void crc2(unsigned char *ZAdr,unsigned int *DoAdr,unsigned int lle){
 unsigned char i;
 unsigned int C,NrBat;
 #asm("wdr")
 for(NrBat=0;NrBat!=lle;NrBat++,ZAdr++)
 {          
  C=((*DoAdr>>8)^*ZAdr)<<8;
  for(i=0;i!=8;i++)
   if (C&0x8000)
    C=(C<<1)^0x1021;
   else C=C<<1;
    *DoAdr=C^(*DoAdr<<8);
 }        
}   
char hextocharh(unsigned char byte){        
  #asm("wdr")
  switch (byte&0xF0) 
  {
   case 0x00: return ('0'); 
   case 0x10: return ('1'); 
   case 0x20: return ('2'); 
   case 0x30: return ('3'); 
   case 0x40: return ('4'); 
   case 0x50: return ('5'); 
   case 0x60: return ('6'); 
   case 0x70: return ('7'); 
   case 0x80: return ('8'); 
   case 0x90: return ('9'); 
   case 0xA0: return ('A'); 
   case 0xB0: return ('B'); 
   case 0xC0: return ('C'); 
   case 0xD0: return ('D'); 
   case 0xE0: return ('E'); 
   case 0xF0: return ('F'); 
  }
}  
 
char hextocharl(unsigned char byte){        
  #asm("wdr")
  switch (byte&0x0F)
  {
   case 0x00: return ('0'); 
   case 0x01: return ('1'); 
   case 0x02: return ('2'); 
   case 0x03: return ('3'); 
   case 0x04: return ('4'); 
   case 0x05: return ('5'); 
   case 0x06: return ('6'); 
   case 0x07: return ('7'); 
   case 0x08: return ('8'); 
   case 0x09: return ('9'); 
   case 0x0A: return ('A'); 
   case 0x0B: return ('B'); 
   case 0x0C: return ('C'); 
   case 0x0D: return ('D'); 
   case 0x0E: return ('E'); 
   case 0x0F: return ('F'); 
  }     
 }
unsigned char chartohexh(unsigned char byte){        
  #asm("wdr")
  switch (byte) 
  {
   case '0': return (0x00); 
   case '1': return (0x10); 
   case '2': return (0x20); 
   case '3': return (0x30); 
   case '4': return (0x40); 
   case '5': return (0x50); 
   case '6': return (0x60); 
   case '7': return (0x70); 
   case '8': return (0x80); 
   case '9': return (0x90); 
   case 'A': return (0xA0); 
   case 'B': return (0xB0); 
   case 'C': return (0xC0); 
   case 'D': return (0xD0); 
   case 'E': return (0xE0); 
   case 'F': return (0xF0);
   case 'a': return (0xA0); 
   case 'b': return (0xB0); 
   case 'c': return (0xC0); 
   case 'd': return (0xD0); 
   case 'e': return (0xE0); 
   case 'f': return (0xF0);
  }
 }  

unsigned char chartohexl(unsigned char byte){        
  #asm("wdr")
  switch (byte) 
  {
   case '0': return (0x00); 
   case '1': return (0x01); 
   case '2': return (0x02); 
   case '3': return (0x03); 
   case '4': return (0x04); 
   case '5': return (0x05); 
   case '6': return (0x06); 
   case '7': return (0x07); 
   case '8': return (0x08); 
   case '9': return (0x09); 
   case 'A': return (0x0A); 
   case 'B': return (0x0B); 
   case 'C': return (0x0C); 
   case 'D': return (0x0D); 
   case 'E': return (0x0E); 
   case 'F': return (0x0F);
   case 'a': return (0x0A); 
   case 'b': return (0x0B); 
   case 'c': return (0x0C); 
   case 'd': return (0x0D); 
   case 'e': return (0x0E); 
   case 'f': return (0x0F);
  } 
}