/*****************************************************
This program was produced by the
CodeWizardAVR V1.25.3 Professional
Automatic Program Generator
© Copyright 1998-2007 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : GSM Ringer Pro
Version : v1.0.1
Date    : 04.03.2010
Author  : T.Drozdovskiy                           
Company : Smart Logic                              
Comments: 


Chip type           : ATmega1280
Program type        : Application
Clock frequency     : 11,059200 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 2048
*****************************************************/

#include <mega1280.h>

register unsigned char TimeGuard=0;

#include <board.h>
#include <sim300.h>
#include <delay.h>                     
#include <string.h>
#include <stdlib.h>
#include <snd.h>  
#include <at45db161.h>
#include <ctype.h>                      

         
#define DELAY_C         60                              // время сек. длительность исходящего звонка
#define DELAY_CC        150                             // время сек. длительности входящего звонка
#define DELAYMC240      5                               // количество стробов для обработки зон

#define NUMCALL         5                               // количество абонентов системы        
#define AMOUNT_ATTEMPT_SMS  10                          // количество попыток отправить SMS
#define NUMCARD         20                              // количество RFID карточек 

#define MIN_VOLTAGE     0x45                            // минимальное напряжение окна в зонах
#define MAX_VOLTAGE     0x75                            // максимальное напряжение окна в зонах

#define C_DTMF          0                               // возможность активировать/деактивировать по DTMF
#define C_RFID          2                               // возможность активировать/деактивировать по RFID
#define C_BUTT          4                               // возможность активировать/деактивировать по кнопке
#define C_SET           0                               // активировать
#define C_GET           1                               // деактивировать

#define F_SETACTIVEZONE     "SYSAZ OK"                  // активные зоны установлены
#define F_SETUSERNUMBER     "SYSUN OK"                  // номер абонента установлен
#define F_SETTIMEACTIVE     "SYSTA OK"                  // время отсрочки активации установлено
#define F_SETTIMEDISACTIVE  "SYSTD OK"                  // время отсрочки деактвации установлено
#define F_SETCONFIGURATION  "SYSCS OK"                  // значения п умолчанию установлены                  
#define F_SETIMMING_SRC_EV  "SYSTE OK"                  // отсрочка и активация исполнительных устройств установлена 
#define F_SETCONDITIONSG    "SYSON OK"                  // параметры активации системы установлены
#define F_SETCARD           "SYSCD OK"                  // карточка установлена
#define F_COMMANDNOTSUPPORT "SYCOMMAND NOT SUPPORT"     // команда не поддерживается
#define F_VERSIA            "SYVER GSM Ringer Pro v1.0.1" // версия программного обеспечения

#define MELODY          0x00005D7C                      // мелодия при совпадении карточки

#define M_SYSTEM_ACT      0x00                          // контроллер активирован
#define M_SYSTEM_DACT     0x01                          // контроллер в режиме ожыдания
#define M_MAIN_MENU       0x02                          // главное меню
#define M_ACT_DACT        0x03                          // активация деактивация контроллера
#define M_LISTEN_AGAIN    0x04                          // прослушать еще раз; выйти вглавное меню
#define M_A_ACT           0x05                          // по истечению времени акт. проверить сост. контроллера   
#define M_A_DACT          0x06                          // контроллер деативирован  
#define M_RELAY1          0x07                          // меню реле 1
#define M_A_RELAY1_ON     0x08                          // ответ реле 1 включено  
#define M_A_RELAY1_OFF    0x09                          // ответ реле 1 выключено  
#define M_RELAY2          0x0A                          // меню реле 2
#define M_A_RELAY2_ON     0x0B                          // ответ реле 2 включено  
#define M_A_RELAY2_OFF    0x0C                          // ответ реле 2 выключено  
#define M_OPENDRAIN1      0x0D                          // меню откр. колл. 1
#define M_A_OPENDRAIN1_ON 0x0E                          // ответ откр. колл. 1 включен  
#define M_A_OPENDRAIN1_OFF  0x0F                        // ответ откр. колл. 1 выключен
#define M_OPENDRAIN2      0x10                          // меню откр. колл. 2
#define M_A_OPENDRAIN2_ON 0x11                          // ответ откр. колл. 2 включен  
#define M_A_OPENDRAIN2_OFF  0x12                        // ответ откр. колл. 2 выключен
#define M_A_CONTROL_OUT_ON  0x13                        // все выходы октивированы
#define M_A_OUT_OFF       0x14                          // все выходы выключены
#define M_OUT_ZONE        0x15                          // введите номер исключаемой зоны
#define M_CURRENT_TIME    0x16                          // на данный момент зоны  
#define M_IN_ZONE         0x17                          // введите номер включаемой зоны
#define M_A_ZONA          0x18                          // зона
#define M_A_ON            0x19                          // включена
#define M_A_OFF           0x1A                          // выключена
#define M_A_ZONAS         0x1B                          // зоны
#define M_A_ONS           0x1C                          // включены
#define M_A_OFFS          0x1D                          // выключены
#define M_A_1             0x1E                          // 1
#define M_A_2             0x1F                          // 2
#define M_A_3             0x20                          // 3
#define M_A_4             0x21                          // 4
#define M_A_5             0x22                          // 5
#define M_A_6             0x23                          // 6
#define M_A_7             0x24                          // 7
#define M_A_8             0x25                          // 8
#define M_A_9             0x26                          // 9
#define M_A_0             0x27                          // 0
#define M_A_ALARMS        0x28                          // внимание тревога в зонах
#define M_A_ALARM         0x29                          // внимание тревога в зоне  

#define MAX_ON_EXECUTE_DEVICE   32400                   // максимальное время работы исполнительных устройств

flash char tascii[]={"0123456789ABCDEF"};               // табица ASCII символов
flash char str_zone[8][7]={" Zone1"," Zone2"," Zone3"," Zone4"," Zone5"," Zone6"," Zone7"," Zone8"};
            
// Declare variable setting in EEPROM and initial value             
eeprom unsigned char *pEEPROM;                          // укзательна на EEPROM
eeprom unsigned char SETSE=0x0A;                        // значение отсрочки для активации системы
eeprom unsigned char SMSE=0x00;                         // необходимость отправки SMS абонентам
eeprom unsigned char GETSE=0x0A;                        // значение отсрочки для деактивации системы
eeprom unsigned char CALE=0x00;                         // необходимость дозвона абонентам
eeprom unsigned char ZoneMaskE=0xFF;                    // 
eeprom unsigned char ConditionSGE=0x00;                  // условия активации и деактивации 
eeprom unsigned char NUMAE[NUMA_NUMBER][11]={"0000000000",  // номера абонентов
                                             "0000000000",
                                             "0000000000",
                                             "0000000000",
                                             "0000000000"};              
              
eeprom unsigned char CardE[NUMCARD][3]={                // начальное значение масива карточек
                                        0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, // 1 - 2  
                                        0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, // 3 - 4
                                        0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, // 5 - 6
                                        0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, // 7 - 8
                                        0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, // 9 - 10
                                        0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, // 11 - 12
                                        0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, // 13 - 14
                                        0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, // 15 - 16
                                        0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF, // 17 - 18
                                        0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF}; // 19 - 20


eeprom unsigned char PWDE[]="000000";                   // начальный пароль   
eeprom unsigned char sign_E=0;
eeprom struct {                                         // структура источников событий
    unsigned int CounterRelay1_on;
    unsigned int CounterRelay2_on;
    unsigned int CounterOpenDrain1_on;
    unsigned int CounterOpenDrain2_on;
    unsigned int CounterRelay1_off;
    unsigned int CounterRelay2_off;
    unsigned int CounterOpenDrain1_off;
    unsigned int CounterOpenDrain2_off;
    unsigned char ExecuteDevice; 
}SourceEvE[11]={
                         {0,0,0,0,0,0,0,0,0},       // Zona1
                         {0,0,0,0,0,0,0,0,0},       // Zona2
                         {0,0,0,0,0,0,0,0,0},       // Zona3
                         {0,0,0,0,0,0,0,0,0},       // Zona4
                         {0,0,0,0,0,0,0,0,0},       // Zona5
                         {0,0,0,0,0,0,0,0,0},       // Zona6
                         {0,0,0,0,0,0,0,0,0},       // Zona7
                         {0,0,0,0,0,0,0,0,0},       // Zona8
                         {0,0,0,0,0,0,0,0,0},       // Button
                         {0,0,0,0,0,0,0,0,0},       // RFID
                         {0,0,0,0,0,0,0,0,0}        // DTMF
                         };             

//  eeprom struct {
//     unsigned int CounterRelay1_on;
//     unsigned char Number[4][11];
//     unsigned char ExecuteDevice; 
// }ZonasE[2]={
//             {0,{"0683453549","0683453549","0683453549","0683453549"},0},
//             {0,{"0683453549","0683453549","0683453549","0683453549"},0}
//            };

// Declare your variables in RAM here
register unsigned int  CounterGSM=0;                    // счетчик тиков для АТ команд
register unsigned char ZonaS=0;                         // статус зон

unsigned char Card[NUMCARD][3];                         // масив карточек
unsigned char counter_PC=0;                             // количество байт в пакете 
unsigned char buffer_PC[255];                           // приемный буфер 
unsigned char SETSR=0;                                  // счетчик количества сек для активации системы   
unsigned char GETSR=0;                                  // счетчик количества сек для деактивации системы      
unsigned char NUMAR[NUMA_NUMBER][11];                   // номера абонентов
unsigned char rdy_counter=25;                           // счетчик сек до опроса о готовности работы SIM300
unsigned char work_modem=120;                           // счетчик сек по опросу модема на АТ команду
unsigned char ZoneMask;                                 // маска зон
unsigned int  gsm_counter_AC=0;                         // счетчик времени соединения
unsigned int CounterRelay1_on=0;                        // счетчик (сек.) до включения реле-1
unsigned int CounterRelay2_on=0;                        // счетчик (сек.) до включения реле-2
unsigned int CounterOpenDrain1_on=0;                    // счетчик (сек.) до включения открытого коллектора-1
unsigned int CounterOpenDrain2_on=0;                    // счетчик (сек.) до включения открытого коллектора-2
unsigned int CounterRelay1_off=0;                       // счетчик (сек.) включения реле-1
unsigned int CounterRelay2_off=0;                       // счетчик (сек.) включения реле-2
unsigned int CounterOpenDrain1_off=0;                   // счетчик (сек.) включения открытого коллектора-1
unsigned int CounterOpenDrain2_off=0;                   // счетчик (сек.) включения открытого коллектора-2
unsigned char Zona=0;                                   // состояние зоны при активной системе            
eeprom unsigned char ZonaE=0;                                   // состояние зоны при активной системе            
unsigned char SoftInterrupt=0;                          // статус программного прерывания и причина
unsigned char DTMF_menu[5];                             // память на 5 символов по DTMF
unsigned char menu_index=0;                             // количество вложенности в меню DTMF
unsigned char ConditionSG;                              // условия активации и деактивации
unsigned char StatusCall=0;                             // статус соединения
unsigned char StatusSMS=0;                             // статус соединения
unsigned char AttemptSMS=0;                                // количество попыток
unsigned char AttemptCall=0;                                // количество попыток
unsigned char FHandContr[12];                           // идентификатор устройства
unsigned char counter1=0;                               // количество принятых байт по wiegund-26
unsigned char mas1[5]={0x0,0x0,0x0,0x0,0x0};            // масив байт принятых по wiegund-26
unsigned char IdenCard1[3];                             // идентификатор карточки
unsigned long MusicRD1=0;                               // мелодия для ридера   
unsigned char MusicCounterRD1=0;                        // делитель для мелодии
unsigned char ChangeRegNetwork=240;
typedef struct {                                        // структура источников событий
    unsigned int CounterRelay1_on;
    unsigned int CounterRelay2_on;
    unsigned int CounterOpenDrain1_on;
    unsigned int CounterOpenDrain2_on;
    unsigned int CounterRelay1_off;
    unsigned int CounterRelay2_off;
    unsigned int CounterOpenDrain1_off;
    unsigned int CounterOpenDrain2_off;
    unsigned char ExecuteDevice; 
}EventVariable;

EventVariable SourceEv[11]={
                         {0,0,0,0,0,0,0,0,0},       // Zona1
                         {0,0,0,0,0,0,0,0,0},       // Zona2
                         {0,0,0,0,0,0,0,0,0},       // Zona3
                         {0,0,0,0,0,0,0,0,0},       // Zona4
                         {0,0,0,0,0,0,0,0,0},       // Zona5
                         {0,0,0,0,0,0,0,0,0},       // Zona6
                         {0,0,0,0,0,0,0,0,0},       // Zona7
                         {0,0,0,0,0,0,0,0,0},       // Zona8
                         {0,0,0,0,0,0,0,0,0},       // Button
                         {0,0,0,0,0,0,0,0,0},       // RFID
                         {0,0,0,0,0,0,0,0,0}        // DTMF
                         };             


bit RCallRDY=0;                                         // начальная инициализация параметров модема
bit StartFlagPC=0;                                      // флаг старта приема пакета PC
bit sign=0;                                             // статус сиганализации
bit sign_on=0;                                          // активировать систему
bit SInfo=0;                                            // флаг необходимости оповещения абонентов (Call)
bit CurrentNetwork=0;                           

bit VoiceChannel_0=0;                                   // проключение 0 голосового канала 
bit VoiceChannel_1=0;                                   // проключение 1 голосового канала
// Параметры уровней для АЦП
unsigned char counter_adc0_h=0;                         // счетчик длительности высокого уровня 
unsigned char counter_adc0_m=0;                         // счетчик длительности среднего уровня
unsigned char counter_adc0_l=0;                         // счетчик длительности низкого уровня
bit level_adc0_l=0;                                     // флаг низкого уровня 
bit level_adc0_m=0;                                     // флаг среднего уровня
bit level_adc0_h=0;                                     // флаг высокого уровня

unsigned char counter_adc1_h=0;
unsigned char counter_adc1_m=0;
unsigned char counter_adc1_l=0;
bit level_adc1_l=0;
bit level_adc1_m=0;
bit level_adc1_h=0;

unsigned char counter_adc2_h=0;
unsigned char counter_adc2_m=0;
unsigned char counter_adc2_l=0;
bit level_adc2_l=0;
bit level_adc2_m=0;
bit level_adc2_h=0;

unsigned char counter_adc3_h=0;
unsigned char counter_adc3_m=0;
unsigned char counter_adc3_l=0;
bit level_adc3_l=0;
bit level_adc3_m=0;
bit level_adc3_h=0;

unsigned char counter_adc4_h=0;
unsigned char counter_adc4_m=0;
unsigned char counter_adc4_l=0;
bit level_adc4_l=0;
bit level_adc4_m=0;
bit level_adc4_h=0;
                                         
unsigned char counter_adc5_h=0;
unsigned char counter_adc5_m=0;
unsigned char counter_adc5_l=0;
bit level_adc5_l=0;
bit level_adc5_m=0;
bit level_adc5_h=0;

unsigned char counter_adc6_h=0;
unsigned char counter_adc6_m=0;
unsigned char counter_adc6_l=0;
bit level_adc6_l=0;
bit level_adc6_m=0;
bit level_adc6_h=0;

unsigned char counter_adc7_h=0;
unsigned char counter_adc7_m=0;
unsigned char counter_adc7_l=0;
bit level_adc7_l=0;
bit level_adc7_m=0;
bit level_adc7_h=0;                                             

// Параметры уровня для кнопки
unsigned char counter_but1_l=0;                         // счетчик длительности низкого уровня
unsigned char counter_but1_h=0;                         // счетчик длительности высокого уровня
bit level_but1_l=1;                                     // флаг низкого уровня
bit level_but1_h=0;                                     // флаг высокого уровня
            
// PlayList buffer                                      // 
#define PLAY_LIST_PWM 16                                // количество фрагментов в листе
unsigned char rx_play_list[PLAY_LIST_PWM];              // лист очереди проигрывания звуковых фрагментов

unsigned char rx_wr_index_list,rx_rd_index_list,rx_counter_list;  // индексы очереди 
// Get a character from the play list buffer
#pragma used+
unsigned char get_play_list(void)                                // вытащить с плей-листа
{
unsigned char data;
while (rx_counter_list==0);
data=rx_play_list[rx_rd_index_list];                      
if (++rx_rd_index_list == PLAY_LIST_PWM) rx_rd_index_list=0;
--rx_counter_list;
return data;
}
#pragma used-
 
// This flag is set on play list buffer overflow
bit rx_buffer_overflow_list;

void put_play_list(unsigned char data)                           // добавить в плей-лист
{
    if(FSND)
    {
        rx_play_list[rx_wr_index_list]=data;
        if (++rx_wr_index_list == PLAY_LIST_PWM) rx_wr_index_list=0;
        if (++rx_counter_list == PLAY_LIST_PWM)
        {
            rx_counter_list=0;
            rx_buffer_overflow_list=1;
        };
    }
    else
        snd_play(data);
}                    

#define clear_list()   FSND=0; while(rx_counter_list) get_play_list()   // очистить плей-лист
 
// прототипы функций
void Receive_PC(void);                                  // прием данных от PC и формирование пакета
void SendAnswer(flash unsigned char *);                          // посылает строку с Flash
void SendAnswerF(flash unsigned char *);                         // посылает строку с Flash
void SendAnswerR(unsigned char *);                               // посылает строку с RAM
void SetFactorySetting(void);                           // устанавливет установки по умолчанию
void StatusS(void);                                     // 
void eeprom2ram(void);                                  // копирование данных с EEPROM в RAM
void SendInfo(void);                                    // отправка информации абоненту
void SendCall(void);                                    // отправка информации абоненту
unsigned char SMSF(void);                                        // отправка SMS
void SetUserNumber(unsigned char *);                    // запись номера абонента
void GetUserNumber(unsigned char *);                    // считывание номера абонента
void SetTimeActive(unsigned char *);                    // установка времени отстрочки активации
void GetTimeActive(void);                               // считывание времени отстрочки активации
void SetTimeDisactive(unsigned char *);                 // установка времени отсрочки оповещения
void GetTimeDisactive(void);                            // считывание времени отсрочки оповещения
void SetConfiguration(unsigned char *);                 // установка конфигурации
void GetConfiguration(void);                            // считывание конфигурации
void SetTimmingSourceEv(unsigned char *);               // установка временных параметров при возникновении события
void GetTimmingSourceEv(unsigned char *);               // считывание временных параметров при возникновении события
void SetOptionSG(unsigned char *);                      // установка опции активации
void GetOptionSG();                                     // считывание опции деактивации
void SetActiveZone(unsigned char *);                    // установка параметров активных зон
void GetActiveZone(void);                               // считывание параметров активных зон
void SetCard(unsigned char *);                          // установка карт пользователей
void GetCard(unsigned char *);                          // считывание карт пользователей
void Eventsheduler(unsigned char);                      // обработчик событий при возникновении событий 
unsigned char cpi_ram(unsigned char *);                 // проверка карточки пользователя
unsigned char  Char2Hexh(unsigned char);                // перевод ASCII в HEX старший байт
unsigned char  Char2Hexl(unsigned char);                // перевод ASCII в HEX младший байт
unsigned char cmp_digit(unsigned char *,unsigned char); // проверку масива на пренадлежность цифрам 
void SimHardReset(void);                                // аппаратно-програмный сброс модема

unsigned char F_Ring(void);                             // прием входящего звонка
unsigned char F_SMS(void);                              // прием входящей SMS
unsigned char F_Crdy(void);                             // инициализация GSM модема

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

// USART0 Receiver buffer
#define RX_BUFFER_SIZE0 255
unsigned char rx_buffer0[RX_BUFFER_SIZE0];

#if RX_BUFFER_SIZE0<256
unsigned char rx_wr_index0,rx_rd_index0,rx_counter0;
#else
unsigned int rx_wr_index0,rx_rd_index0,rx_counter0;
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
#define TX_BUFFER_SIZE0 255
char tx_buffer0[TX_BUFFER_SIZE0];

#if TX_BUFFER_SIZE0<256
unsigned char tx_wr_index0,tx_rd_index0,tx_counter0;
#else
unsigned int tx_wr_index0,tx_rd_index0,tx_counter0;
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

// USART1 Receiver buffer
#define RX_BUFFER_SIZE1 255
char rx_buffer1[RX_BUFFER_SIZE1];

#if RX_BUFFER_SIZE1<256
unsigned char rx_wr_index1,rx_rd_index1,rx_counter1;
#else
unsigned int rx_wr_index1,rx_rd_index1,rx_counter1;
#endif

// This flag is set on USART1 Receiver buffer overflow
bit rx_buffer_overflow1;

// USART1 Receiver interrupt service routine
interrupt [USART1_RXC] void usart1_rx_isr(void)
{
char status,data;
status=UCSR1A;
data=UDR1;    
       #ifdef DEBUG                                                                        
       putcharPC(data);
       #endif

if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {                                     
   TimeGuard=3;
   rx_buffer1[rx_wr_index1]=data;
   if (++rx_wr_index1 == RX_BUFFER_SIZE1) rx_wr_index1=0;
   if (++rx_counter1 == RX_BUFFER_SIZE1)
      {
      rx_counter1=0;
      rx_buffer_overflow1=1;
      };
   };
}

// Get a character from the USART1 Receiver buffer
#pragma used+
char getchar1(void)
{
char data;
while (rx_counter1==0);
data=rx_buffer1[rx_rd_index1];
if (++rx_rd_index1 == RX_BUFFER_SIZE1) rx_rd_index1=0;
#asm("cli")
--rx_counter1;
#asm("sei")
return data;
}
#pragma used-

// USART1 Transmitter buffer
#define TX_BUFFER_SIZE1 255
char tx_buffer1[TX_BUFFER_SIZE1];

#if TX_BUFFER_SIZE1<256
unsigned char tx_wr_index1,tx_rd_index1,tx_counter1;
#else
unsigned int tx_wr_index1,tx_rd_index1,tx_counter1;
#endif

// USART1 Transmitter interrupt service routine
interrupt [USART1_TXC] void usart1_tx_isr(void)
{
if (tx_counter1)
   {
   --tx_counter1;
   UDR1=tx_buffer1[tx_rd_index1];
   if (++tx_rd_index1 == TX_BUFFER_SIZE1) tx_rd_index1=0;
   };
}

// Write a character to the USART1 Transmitter buffer
#pragma used+
void putchar1(char c)
{
while (tx_counter1 == TX_BUFFER_SIZE1);
#asm("cli")
if (tx_counter1 || ((UCSR1A & DATA_REGISTER_EMPTY)==0))
   {
   tx_buffer1[tx_wr_index1]=c;
   if (++tx_wr_index1 == TX_BUFFER_SIZE1) tx_wr_index1=0;
   ++tx_counter1;
   }
else
   UDR1=c;
#asm("sei")
}
#pragma used-

// USART2 Receiver buffer
#define RX_BUFFER_SIZE2 255
char rx_buffer2[RX_BUFFER_SIZE2];

#if RX_BUFFER_SIZE2<256
unsigned char rx_wr_index2,rx_rd_index2,rx_counter2;
#else
unsigned int rx_wr_index2,rx_rd_index2,rx_counter2;
#endif

// This flag is set on USART2 Receiver buffer overflow
bit rx_buffer_overflow2;

// USART2 Receiver interrupt service routine
interrupt [USART2_RXC] void usart2_rx_isr(void)
{
char status,data;
status=UCSR2A;
data=UDR2;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer2[rx_wr_index2]=data;
   if (++rx_wr_index2 == RX_BUFFER_SIZE2) rx_wr_index2=0;
   if (++rx_counter2 == RX_BUFFER_SIZE2)
      {
      rx_counter2=0;
      rx_buffer_overflow2=1;
      };
   };
}

// Get a character from the USART2 Receiver buffer
#pragma used+
char getchar2(void)
{
char data;
while (rx_counter2==0);
data=rx_buffer2[rx_rd_index2];
if (++rx_rd_index2 == RX_BUFFER_SIZE2) rx_rd_index2=0;
#asm("cli")
--rx_counter2;
#asm("sei")
return data;
}
#pragma used-


// USART3 Receiver buffer
#define RX_BUFFER_SIZE3 32
char rx_buffer3[RX_BUFFER_SIZE3];

#if RX_BUFFER_SIZE3<256
unsigned char rx_wr_index3,rx_rd_index3,rx_counter3;
#else
unsigned int rx_wr_index3,rx_rd_index3,rx_counter3;
#endif

// This flag is set on USART3 Receiver buffer overflow
bit rx_buffer_overflow3;

// USART3 Receiver interrupt service routine
interrupt [USART3_RXC] void usart3_rx_isr(void)
{
char status,data;
status=UCSR3A;
data=UDR3;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
   {
   rx_buffer3[rx_wr_index3]=data;
   if (++rx_wr_index3 == RX_BUFFER_SIZE3) rx_wr_index3=0;
   if (++rx_counter3 == RX_BUFFER_SIZE3)
      {
      rx_counter3=0;
      rx_buffer_overflow3=1;
      };
   };
}

// Get a character from the USART3 Receiver buffer
#pragma used+
char getchar3(void)
{
char data;
while (rx_counter3==0);
data=rx_buffer3[rx_rd_index3];
if (++rx_rd_index3 == RX_BUFFER_SIZE3) rx_rd_index3=0;
#asm("cli")
--rx_counter3;
#asm("sei")
return data;
}
#pragma used-

// Standard Input/Output functions
#include <stdio.h>

// SPI functions
#include <spi.h>

// External Interrupt 0 service routine
interrupt [INT0] void ext_int0_isr(void)
{
// Place your code here   
 DTMF_menu[menu_index]=PINC&0x0F;
 menu_index++;                  
 SoftInterrupt|=SI_MENU;
 SI_GEN    
}


// External Interrupt 4 service routine
interrupt [INT4] void ext_int4_isr(void)
{
// Place your code here                  
 unsigned char i;
 unsigned char mas[13];   
 while(SoftInterrupt)
 {
  #asm("wdr")       
    if(SoftInterrupt&SI_RFID) 
    {
        mas[0]=0;
        strcatf(&mas[0],"SYCRD ");
        mas[6]=tascii[(IdenCard1[0]&0xF0)>>4];
        mas[7]=tascii[(IdenCard1[0]&0x0F)];
        mas[8]=tascii[(IdenCard1[1]&0xF0)>>4];
        mas[9]=tascii[(IdenCard1[1]&0x0F)];
        mas[10]=tascii[(IdenCard1[2]&0xF0)>>4];
        mas[11]=tascii[(IdenCard1[2]&0x0F)];
        mas[12]=0;
        SendAnswerR(mas);
        #asm("cli")
        if (cpi_ram(&IdenCard1[0]))
        {   
            Eventsheduler(RFID_TYPE);
            MusicRD1=MELODY;
            if((ConditionSG&((1<<C_RFID)<<C_GET))&&(sign))
            {
                sign_on=0; //get
                SETSR=0;
                BUZ_OFF
                if(sign)
                {
                    Zona=0;
                    ZonaE=0;
                }
            }
            if((ConditionSG&((1<<C_RFID)<<C_SET))&&(!sign))
            {
                sign_on=1; //set
                GETSR=0;
                if(!SInfo)
                {
                    Zona=0;
                    ZonaE=0;                
                }
            }
        } 
        SoftInterrupt&=~SI_RFID;    
    }
    if(SoftInterrupt&SI_MENU)
    {                       
        if(menu_index==1)
        {   
            #ifdef DEBUG
            SendAnswer("\r\nMain menu:");
            #asm("cli")
            #endif
        }                                
        switch (DTMF_menu[0]) {
        case 1 :{
                    #ifdef DEBUG
                    SendAnswer("\r\nRelay1 mode:");
                    #asm("cli")
                    #endif
                    if(menu_index==2)
                    {
                        switch (DTMF_menu[1]) {
                        case 1 :{                                  
                                    #ifdef DEBUG                                    
                                    SendAnswer("\r\nRelay1 ON:");
                                    #asm("cli") 
                                    #endif
                                    menu_index--;
                                    RELAY1_ON
                                    CounterRelay1_off=MAX_ON_EXECUTE_DEVICE;
                                    clear_list();                                    
                                    put_play_list(M_A_RELAY1_ON);
                                    put_play_list(M_RELAY1);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 2 :{         
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nRelay1 OFF");
                                    #asm("cli")                                         
                                    #endif
                                    menu_index--;
                                    RELAY1_OFF
                                    CounterRelay1_off=0;
                                    clear_list();                                    
                                    put_play_list(M_A_RELAY1_OFF);
                                    put_play_list(M_RELAY1);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 0x0B :{                                                
                                        #ifdef DEBUG                                                        
                                        SendAnswer("\r\nAgain listen menu");
                                        #asm("cli")
                                        #endif
                                        menu_index--;
                                        clear_list();
                                        put_play_list(M_RELAY1);
                                        put_play_list(M_LISTEN_AGAIN);
                                   }
                        break;
                        case 0x0C :{
                                        #ifdef DEBUG                                    
                                        SendAnswer("\r\nOut in main menu");
                                        #asm("cli")
                                        #endif
                                        menu_index=0;
                                        clear_list();
                                        put_play_list(M_MAIN_MENU);
                                   }
                        break;
                        default: menu_index--;
                        }; 
                    }
                    else
                    {
                        clear_list();
                        put_play_list(M_RELAY1);
                        put_play_list(M_LISTEN_AGAIN);
                    }
                }
        break;
        case 2 :{
                    #ifdef DEBUG                                    
                    SendAnswer("\r\nRelay2 mode:");
                    #asm("cli")
                    #endif
                    if(menu_index==2)
                    {
                        switch (DTMF_menu[1]) {
                        case 1 :{                                  
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nRelay2 ON:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    RELAY2_ON
                                    CounterRelay2_off=MAX_ON_EXECUTE_DEVICE;
                                    clear_list();                                    
                                    put_play_list(M_A_RELAY2_ON);
                                    put_play_list(M_RELAY2);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 2 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nRelay2 OFF");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    RELAY2_OFF
                                    CounterRelay2_off=0;
                                    clear_list();                                    
                                    put_play_list(M_A_RELAY2_OFF);
                                    put_play_list(M_RELAY2);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 0x0B :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nAgain listen menu");
                                        #asm("cli")
                                        #endif
                                        menu_index--;
                                        clear_list();
                                        put_play_list(M_RELAY2);
                                        put_play_list(M_LISTEN_AGAIN);
                                   }
                        break;
                        case 0x0C :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nOut in main menu");
                                        #asm("cli")
                                        #endif
                                        menu_index=0;
                                        clear_list();
                                        put_play_list(M_MAIN_MENU);
                                   }
                        break;
                        default: menu_index--;
                        }; 
                    }
                    else
                    {
                        clear_list();
                        put_play_list(M_RELAY2);
                        put_play_list(M_LISTEN_AGAIN);
                    }
                }
        break;
        case 3 :{
                    #ifdef DEBUG                                                                        
                    SendAnswer("\r\nOpenDrain1 mode:");
                    #asm("cli")
                    #endif
                    if(menu_index==2)
                    {
                        switch (DTMF_menu[1]) {
                        case 1 :{                                                                      
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nOpenDrain1 ON:");
                                    #asm("cli")                                                                     
                                    #endif
                                    menu_index--;
                                    OPENDRAIN1_ON
                                    CounterOpenDrain1_off=MAX_ON_EXECUTE_DEVICE;
                                    clear_list();                                    
                                    put_play_list(M_A_OPENDRAIN1_ON);
                                    put_play_list(M_OPENDRAIN1);
                                    put_play_list(M_LISTEN_AGAIN);                                    
                                }
                        break;
                        case 2 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nOpenDrain1 OFF");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    OPENDRAIN1_OFF
                                    CounterOpenDrain1_off=0;
                                    clear_list();                                    
                                    put_play_list(M_A_OPENDRAIN1_OFF);
                                    put_play_list(M_OPENDRAIN1);
                                    put_play_list(M_LISTEN_AGAIN);                                                                        
                                }
                        break;
                        case 0x0B :{                                                                  
                                        #ifdef DEBUG                                                                                                                
                                        SendAnswer("\r\nAgain listen menu");
                                        #asm("cli")
                                        #endif
                                        menu_index--;
                                        clear_list();
                                        put_play_list(M_OPENDRAIN1);
                                        put_play_list(M_LISTEN_AGAIN);                                                                           
                                   }
                        break;
                        case 0x0C :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nOut in main menu");
                                        #asm("cli")
                                        #endif
                                        menu_index=0;
                                        clear_list();
                                        put_play_list(M_MAIN_MENU);
                                   }
                        break;
                        default: menu_index--;
                        }; 
                    }
                    else
                    {
                        clear_list();
                        put_play_list(M_OPENDRAIN1);
                        put_play_list(M_LISTEN_AGAIN);                                                            
                    }                    
                }
        break;
        case 4 :{
                    #ifdef DEBUG                                                                        
                    SendAnswer("\r\nOpenDrain2 mode:");
                    #asm("cli")
                    #endif
                    if(menu_index==2)
                    {
                        switch (DTMF_menu[1]) {
                        case 1 :{                                                                      
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nOpenDrain2 ON:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    OPENDRAIN2_ON
                                    CounterOpenDrain2_off=MAX_ON_EXECUTE_DEVICE;
                                    clear_list();                                    
                                    put_play_list(M_A_OPENDRAIN2_ON);
                                    put_play_list(M_OPENDRAIN2);
                                    put_play_list(M_LISTEN_AGAIN);                                                                        
                                }
                        break;
                        case 2 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nOpenDrain2 OFF");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    OPENDRAIN2_OFF
                                    CounterOpenDrain2_off=0;
                                    clear_list();                                    
                                    put_play_list(M_A_OPENDRAIN2_OFF);
                                    put_play_list(M_OPENDRAIN2);
                                    put_play_list(M_LISTEN_AGAIN);                                                                        
                                }
                        break;
                        case 0x0B :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nAgain listen menu");
                                        #asm("cli")
                                        #endif
                                        menu_index--;
                                        clear_list();
                                        put_play_list(M_OPENDRAIN2);
                                        put_play_list(M_LISTEN_AGAIN);                                                                        
                                   }
                        break;
                        case 0x0C :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nOut in main menu");
                                        #asm("cli")                                        
                                        #endif
                                        menu_index=0;
                                        clear_list();
                                        put_play_list(M_MAIN_MENU);
                                   }
                        break;
                        default: menu_index--;
                        }; 
                    }
                    else
                    {
                        clear_list();
                        put_play_list(M_OPENDRAIN2);
                        put_play_list(M_LISTEN_AGAIN);                                                                        
                    }                    
                }
        break;
        case 5 :{
                    VoiceChannel_0=1;  
                    if(menu_index==2)
                    {
                        switch (DTMF_menu[1]) {
                        case 0x0C :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nOut in main menu");
                                        #asm("cli")
                                        #endif
                                        menu_index=0;
                                        VoiceChannel_1=1;
                                        clear_list();
                                        put_play_list(M_MAIN_MENU);
                                   }
                        break;
                        default: menu_index--;
                        }; 
                    }
                }
        break;                      
        case 6 :{                       
                    #ifdef DEBUG                                                                                            
                    SendAnswer("\r\nDTMF mode");
                    #asm("cli")
                    #endif
                    Eventsheduler(DTMF_TYPE);
                    menu_index=0;
                    clear_list();                     
                    put_play_list(M_A_CONTROL_OUT_ON);
                    put_play_list(M_MAIN_MENU);
                }
        break;
        case 7 :{
                    #ifdef DEBUG                                                                        
                    SendAnswer("\r\nExecute device off");
                    #asm("cli")
                    #endif
                    RELAY1_OFF
                    RELAY2_OFF
                    OPENDRAIN1_OFF
                    OPENDRAIN2_OFF
                    CounterRelay1_off=0;
                    CounterRelay2_off=0;
                    CounterOpenDrain1_off=0;
                    CounterOpenDrain2_off=0;
                    menu_index=0;
                    clear_list();
                    put_play_list(M_A_OUT_OFF);
                    put_play_list(M_MAIN_MENU);
                }
        break;                      
        case 8 :{
                    if(menu_index==2)
                    {
                        switch (DTMF_menu[1]) {
                        case 1 :{                                                                      
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nOut Zone1:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask&=~(1<<0);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_1);
                                    put_play_list(M_A_OFF);
                                    put_play_list(M_OUT_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 2 :{
                                    #ifdef DEBUG                                                                                                            
                                    SendAnswer("\r\nOut Zone2:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask&=~(1<<1);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;    
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_2);
                                    put_play_list(M_A_OFF);
                                    put_play_list(M_OUT_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);                                        
                                }
                        break;
                        case 3 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nOut Zone3:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask&=~(1<<2);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_3);
                                    put_play_list(M_A_OFF);
                                    put_play_list(M_OUT_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 4 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nOut Zone4:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask&=~(1<<3);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_4);
                                    put_play_list(M_A_OFF);
                                    put_play_list(M_OUT_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 5 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nOut Zone5:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask&=~(1<<4);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_5);
                                    put_play_list(M_A_OFF);
                                    put_play_list(M_OUT_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 6 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nOut Zone6:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask&=~(1<<5);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_6);
                                    put_play_list(M_A_OFF);
                                    put_play_list(M_OUT_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 7 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nOut Zone7:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask&=~(1<<6);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_7);
                                    put_play_list(M_A_OFF);
                                    put_play_list(M_OUT_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 8 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nOut Zone8:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask&=~(1<<7);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_8);
                                    put_play_list(M_A_OFF);
                                    put_play_list(M_OUT_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 0x0B :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nAgain listen menu");
                                        #asm("cli")
                                        #endif
                                        menu_index--;
                                        clear_list();              
                                        put_play_list(M_CURRENT_TIME);
                                        if(ZoneMask)
                                        {
                                            for(i=0;i!=8;i++)
                                                if(ZoneMask&(0x01<<i)) put_play_list(M_A_1+i);
                                            put_play_list(M_A_ONS);
                                        }
                                        else
                                            put_play_list(M_A_OFFS);                                        
                                        put_play_list(M_OUT_ZONE);
                                        put_play_list(M_LISTEN_AGAIN);
                                   }
                        break;
                        case 0x0C :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nOut in main menu");
                                        #asm("cli")
                                        #endif
                                        menu_index=0;
                                        clear_list();
                                        put_play_list(M_MAIN_MENU);
                                   }
                        break;
                        default: menu_index--;
                        }; 
                    }
                    else
                    {
                        clear_list();              
                        put_play_list(M_CURRENT_TIME);
                        if(ZoneMask)
                        {
                            for(i=0;i!=8;i++)
                                if(ZoneMask&(0x01<<i)) put_play_list(M_A_1+i);
                            put_play_list(M_A_ONS);
                        }
                        else
                            put_play_list(M_A_OFFS);                                        
                        put_play_list(M_OUT_ZONE);
                        put_play_list(M_LISTEN_AGAIN);
                    }
        
                }
        break;                      
        case 9 :{
                    if(menu_index==2)
                    {
                        switch (DTMF_menu[1]) {
                        case 1 :{                                                                      
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nIn Zone1:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask|=(1<<0);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_1);
                                    put_play_list(M_A_ON);
                                    put_play_list(M_IN_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 2 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nIn Zone2:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask|=(1<<1);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;    
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_2);
                                    put_play_list(M_A_ON);
                                    put_play_list(M_IN_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);                                        
                                }
                        break;
                        case 3 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nIn Zone3:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask|=(1<<2);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_3);
                                    put_play_list(M_A_ON);
                                    put_play_list(M_IN_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 4 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nIn Zone4:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask|=(1<<3);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_4);
                                    put_play_list(M_A_ON);
                                    put_play_list(M_IN_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 5 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nIn Zone5:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask|=(1<<4);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_5);
                                    put_play_list(M_A_ON);
                                    put_play_list(M_IN_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 6 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nIn Zone6:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask|=(1<<5);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_6);
                                    put_play_list(M_A_ON);
                                    put_play_list(M_IN_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 7 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nIn Zone7:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask|=(1<<6);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_7);
                                    put_play_list(M_A_ON);
                                    put_play_list(M_IN_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 8 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nIn Zone8:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    ZoneMask|=(1<<7);
                                    ZoneMaskE=ZoneMask;
                                    sign=0;        
                                    sign_E=0;
                                    clear_list();              
                                    put_play_list(M_A_ZONA);
                                    put_play_list(M_A_8);
                                    put_play_list(M_A_ON);
                                    put_play_list(M_IN_ZONE);
                                    put_play_list(M_LISTEN_AGAIN);
                                }
                        break;
                        case 0x0B :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nAgain listen menu");
                                        #asm("cli")
                                        #endif
                                        menu_index--;
                                        clear_list();              
                                        put_play_list(M_CURRENT_TIME);
                                        if(ZoneMask!=0xFF)
                                        {
                                            for(i=0;i!=8;i++)
                                                if((~ZoneMask)&(0x01<<i)) put_play_list(M_A_1+i);
                                            put_play_list(M_A_OFFS);
                                        }
                                        else
                                            put_play_list(M_A_ONS);                                        
                                        put_play_list(M_IN_ZONE);
                                        put_play_list(M_LISTEN_AGAIN);
                                   }
                        break;
                        case 0x0C :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nOut in main menu");
                                        #asm("cli")
                                        #endif
                                        menu_index=0;
                                        clear_list();
                                        put_play_list(M_MAIN_MENU);

                                   }
                        break;
                        default: menu_index--;
                        }; 
                    }
                    else
                    {
                        clear_list();              
                        put_play_list(M_CURRENT_TIME);
                        if(ZoneMask!=0xFF)
                        {
                            for(i=0;i!=8;i++)
                                if((~ZoneMask)&(0x01<<i)) put_play_list(M_A_1+i);
                            put_play_list(M_A_OFFS);
                        }
                        else
                            put_play_list(M_A_ONS);                                        
                        put_play_list(M_IN_ZONE);
                        put_play_list(M_LISTEN_AGAIN);
                    }
        
                }
        break;                      
        case 0x0A:{
                    if(menu_index==2)
                    {
                        switch (DTMF_menu[1]) {
                        case 1 :{                                                                      
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nNotification ON:");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    if(ConditionSG&((1<<C_DTMF)<<C_SET))
                                    {
                                        if(!SInfo)
                                        {
                                            Zona=0;
                                            ZonaE=0;
                                        }
                                        sign_on=1; //set
                                        GETSR=0;
                                        clear_list();
                                        put_play_list(M_A_ACT);
                                        put_play_list(M_ACT_DACT);
                                        put_play_list(M_LISTEN_AGAIN);
                                    }
                                }
                        break;
                        case 2 :{
                                    #ifdef DEBUG                                                                        
                                    SendAnswer("\r\nNotification OFF");
                                    #asm("cli")
                                    #endif
                                    menu_index--;
                                    if(ConditionSG&((1<<C_DTMF)<<C_GET))
                                    {
                                        sign_on=0; //get
                                        SETSR=0;
                                        BUZ_OFF
                                        if(!SInfo)
                                        {
                                            Zona=0;                                                        ZonaE=0;
                                            ZonaE=0;
                                        }
                                        clear_list();
                                        put_play_list(M_A_DACT);
                                        put_play_list(M_ACT_DACT);
                                        put_play_list(M_LISTEN_AGAIN);
                                    }
                                }
                        break;
                        case 0x0B :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nAgain listen menu");
                                        #asm("cli")
                                        #endif
                                        menu_index--;
                                        clear_list();
                                        put_play_list(M_ACT_DACT);
                                        put_play_list(M_LISTEN_AGAIN);
                                   }
                        break;
                        case 0x0C :{
                                        #ifdef DEBUG                                                                        
                                        SendAnswer("\r\nOut in main menu");
                                        #asm("cli")
                                        #endif
                                        menu_index=0;
                                        clear_list();
                                        put_play_list(M_MAIN_MENU);
                                   }
                        break;
                        default: menu_index--;
                        }; 
                    }
                    else
                    {
                        clear_list();
                        put_play_list(M_ACT_DACT);
                        put_play_list(M_LISTEN_AGAIN);
                    }
                }
        break;
        case 0x0B:{
                        menu_index=0;
                        clear_list();
                        put_play_list(M_MAIN_MENU);
                }
        break;                
        case 0x0C:{
                        menu_index=0;
                        clear_list();
                        put_play_list(M_MAIN_MENU);
                }
        break;                
        default: menu_index=0;        
        };
        SoftInterrupt&=~SI_MENU;
    }   
    if(SoftInterrupt&SI_PLAY_LIST)
    {
        if(rx_counter_list) snd_play(get_play_list());
        SoftInterrupt&=~SI_PLAY_LIST;
    }
    if(SoftInterrupt&SI_UPBW) 
    {
        #asm("sei")
        update_snd_buffer();
        #asm("cli")
        SoftInterrupt&=~SI_UPBW;          
    }
 }          
 SI_RST
 EIFR|=0x10;             
}
// External Interrupt 6 service routine
interrupt [INT6] void ext_int6_isr(void)
{
// Place your code here 
  if(!PINE.7)
  {
   TIFR2|=0x02;    
   TIMSK2|=0x02;       
   OCR2A=TCNT2+0xE0;
   mas1[4]<<=1;
   if(mas1[3]&0x80) mas1[4]|=0x01;
   mas1[3]<<=1;
   if(mas1[2]&0x80) mas1[3]|=0x01;
   mas1[2]<<=1;
   if(mas1[1]&0x80) mas1[2]|=0x01;
   mas1[1]<<=1;
   if(mas1[0]&0x80) mas1[1]|=0x01;
   mas1[0]<<=1;
   counter1++;
  }
 EIFR|=0x80;
}

// External Interrupt 7 service routine
interrupt [INT7] void ext_int7_isr(void)
{
// Place your code here
  if(!PINE.6)
  {
    TIFR2|=0x02;    
    TIMSK2|=0x02;       
    OCR2A=TCNT2+0xE0;
    mas1[4]<<=1;
    if(mas1[3]&0x80) mas1[4]|=0x01;
    mas1[3]<<=1;
    if(mas1[2]&0x80) mas1[3]|=0x01;
    mas1[2]<<=1;
    if(mas1[1]&0x80) mas1[2]|=0x01;
    mas1[1]<<=1;
    if(mas1[0]&0x80) mas1[1]|=0x01;
    mas1[0]<<=1;
    mas1[0]|=0x01;
    counter1++;
  }
 EIFR|=0x40;
}

// Timer 1 output compare A interrupt service routine
interrupt [TIM1_COMPA] void timer1_compa_isr(void)
{
// Place your code here
 LED_ON      
 if(CounterRelay1_on)
 {
    CounterRelay1_on--;
    if(!CounterRelay1_on) RELAY1_ON
 }
 if(CounterRelay2_on)
 {
    CounterRelay2_on--;
    if(!CounterRelay2_on) RELAY2_ON
 }
 if(CounterOpenDrain1_on)
 {
    CounterOpenDrain1_on--;
    if(!CounterOpenDrain1_on) OPENDRAIN1_ON
 }
 if(CounterOpenDrain2_on)
 {
    CounterOpenDrain2_on--;
    if(!CounterOpenDrain2_on) OPENDRAIN2_ON
 }
 if(CounterRelay1_off)
 {
    CounterRelay1_off--;
    if(!CounterRelay1_off) RELAY1_OFF 
 }
 if(CounterRelay2_off)
 {
    CounterRelay2_off--;
    if(!CounterRelay2_off) RELAY2_OFF
 }
 if(CounterOpenDrain1_off)
 {
    CounterOpenDrain1_off--;
    if(!CounterOpenDrain1_off) OPENDRAIN1_OFF
 }
 if(CounterOpenDrain2_off)
 {
    CounterOpenDrain2_off--;
    if(!CounterOpenDrain2_off) OPENDRAIN2_OFF
 }
                       
 if(gsm_counter_AC) gsm_counter_AC--; 
 if(!sign)
 {
    if(sign_on)
    {
        if(!SETSR)
        {
            if(((ZonaS&ZoneMask)==ZoneMask)&&(!(ZoneMask&Zona)))
               SETSR=SETSE; 
        }
        else
        {           
            BUZ_TOG
            SETSR--;
            if(!SETSR)
            {         
                if(((ZonaS&ZoneMask)==ZoneMask)&&(!(ZoneMask&Zona))) 
                {
                    sign=1;
                    sign_E=1;
                }
                BUZ_OFF                
            }     
        }
    }
 }
 else
 {
    if(Zona&ZoneMask)
    {
        if(sign_on)
        {
            if(!GETSR)
            {
                GETSR=GETSE; 
            }
            else
            {
                GETSR--;
                if(!GETSR)
                {
                    SInfo=1;
                    StatusSMS=SMSE;
                    StatusCall=CALE;
                    AttemptCall=NUMCALL;   
                    AttemptSMS=AMOUNT_ATTEMPT_SMS;
                    sign=0;
                    sign_E=0;
                }   
            }
        }
    }
    else
    {
        if(!sign_on)
        {
            sign=0;
            sign_E=0;
        }
    }   
 }   
 if(!RCallRDY) 
    if(rdy_counter) rdy_counter--;
 if(work_modem) work_modem--;
 if(ChangeRegNetwork) ChangeRegNetwork--;
}
// Timer 1 output compare B interrupt service routine
interrupt [TIM1_COMPB] void timer1_compb_isr(void)
{
// Place your code here
 if(!sign) LED_OFF
}
// Timer 0 output compare A interrupt service routine
interrupt [TIM0_COMPA] void timer0_compa_isr(void)      // каждые 5мс
{
// Place your code here
 if(CounterGSM) CounterGSM--;
 if(TimeGuard) TimeGuard--;
 if(BUT)
 {
    counter_but1_h=0;
    if(!level_but1_l)
    {
        if(counter_but1_l>=DELAYMC240) 
        {
            level_but1_l=1;
            level_but1_h=0;
            Eventsheduler(BUTT_TYPE);
            if(ConditionSG&((1<<C_BUTT)<<C_SET))
            {
                sign_on=1; //set
                GETSR=0;
                if(!SInfo)
                {
                    Zona=0;
                    ZonaE=0;                
                }
            }
        }
        else
        {
            counter_but1_l++;    
        }    
    }
 }            
 else
 {           
    counter_but1_l=0;            
    if(!level_but1_h)
    {
        if(counter_but1_h>=DELAYMC240) 
        {
            level_but1_h=1;
            level_but1_l=0;            
            if(ConditionSG&((1<<C_BUTT)<<C_GET))
            {
                sign_on=0; //get
                SETSR=0;
                BUZ_OFF
                if(sign)
                {
                    Zona=0;
                    ZonaE=0;    
                }
            }  
        }
        else
        {
            counter_but1_h++;    
        }    
   }
 }            
  if(MusicCounterRD1) MusicCounterRD1--;
  else
  {
    if(MusicRD1) 
    {   
        MusicCounterRD1=6;
        if(MusicRD1&0x01){ LED_RD_ON BEEP_RD_ON}  
        else { LED_RD_OFF BEEP_RD_OFF}
        MusicRD1>>=1;
        if(!MusicRD1){ LED_RD_OFF BEEP_RD_OFF } 
    }
  } 
}   

// Timer 2 output compare interrupt service routine
interrupt [TIM2_COMPA] void timer2_compa_isr(void)
{
// Place your code here
 unsigned char i=0;
 TIMSK2&=~0x02;  
 if(mas1[0]&0x01)i++;
 mas1[0]>>=1;
 if(mas1[1]&0x01) mas1[0]|=0x80;
 mas1[1]>>=1;
 if(mas1[2]&0x01) mas1[1]|=0x80;
 mas1[2]>>=1;
 if(mas1[3]&0x01) mas1[2]|=0x80;
 mas1[3]>>=1;
 if(mas1[4]&0x01) mas1[3]|=0x80;
 mas1[4]>>=1;
 switch (counter1) {
    case 26:    {   
                     if(mas1[0]&0x01) i++; 
                     if(mas1[0]&0x02) i++;
                     if(mas1[0]&0x04) i++;
                     if(mas1[0]&0x08) i++;
                     if(mas1[0]&0x10) i++;
                     if(mas1[0]&0x20) i++;
                     if(mas1[0]&0x40) i++;
                     if(mas1[0]&0x80) i++;
                     if(mas1[1]&0x01) i++;
                     if(mas1[1]&0x02) i++;
                     if(mas1[1]&0x04) i++;
                     if(mas1[1]&0x08) i++;
                     if(i&0x01)
                     {                 
                        i=0;
                        if(mas1[1]&0x10) i++;
                        if(mas1[1]&0x20) i++;
                        if(mas1[1]&0x40) i++;
                        if(mas1[1]&0x80) i++;
                        if(mas1[2]&0x01) i++;
                        if(mas1[2]&0x02) i++;
                        if(mas1[2]&0x04) i++;
                        if(mas1[2]&0x08) i++;
                        if(mas1[2]&0x10) i++;
                        if(mas1[2]&0x20) i++;
                        if(mas1[2]&0x40) i++;
                        if(mas1[2]&0x80) i++;
                        if(mas1[3]&0x01) i++;
                        if(i&0x01)
                        {
                            counter1=0;
                            mas1[0]=mas1[1]=mas1[2]=mas1[3]=mas1[4]=0; 
                            return ;
                        }    
                     }
                mas1[3]=0;
                } 
    break;
    case 34: mas1[4]=0;
    break;
    case 37: mas1[4]&=0x07;
    break;       
    default:
    };                    
 IdenCard1[0]=mas1[2];
 IdenCard1[1]=mas1[1];
 IdenCard1[2]=mas1[0];
 SoftInterrupt|=SI_RFID;
 SI_GEN    
 counter1=0;              
 mas1[0]=mas1[1]=mas1[2]=mas1[3]=mas1[4]=0; 
}

// Timer 5 overflow interrupt service routine
interrupt [TIM5_OVF] void timer5_ovf_isr(void)
{
// Place your code here
    Sound_ISR();
}

#define FIRST_ADC_INPUT 0
#define LAST_ADC_INPUT 7
unsigned char adc_data[LAST_ADC_INPUT-FIRST_ADC_INPUT+1];
#define ADC_VREF_TYPE 0x60

// ADC interrupt service routine
// with auto input scanning
interrupt [ADC_INT] void adc_isr(void)
{
register static unsigned char input_index=0;
// Read the 8 most significant bits
// of the AD conversion result
adc_data[input_index]=ADCH;
// Select next ADC input
if (++input_index > (LAST_ADC_INPUT-FIRST_ADC_INPUT))
   input_index=0;
ADMUX=(ADC_VREF_TYPE & 0xff) | ((FIRST_ADC_INPUT+input_index) & 0x07);
if ((FIRST_ADC_INPUT+input_index) & 0x08) ADCSRB |= 0x08;
else ADCSRB &= 0xf7;            
switch (input_index) {
    case 0 : {                 
                if(MIN_VOLTAGE>adc_data[input_index])
                {
                 counter_adc0_m=0;                       
                 counter_adc0_h=0;            
                 if(!level_adc0_l)
                 {
                    if(counter_adc0_l>=DELAYMC240) 
                        {
                            level_adc0_l=1;
                            level_adc0_m=0;
                            level_adc0_h=0;  
                            LED1_OFF
                            ZonaS&=~(1<<0);
                            if(sign&&(ZoneMask&(1<<0)))
                            {
                                Zona|=(1<<0);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }    
                        }
                    else
                        {
                            counter_adc0_l++;    
                        }    
                 }
                }
                else
                {            
                 if((adc_data[input_index]>MIN_VOLTAGE)&&(adc_data[input_index]<MAX_VOLTAGE))
                 {           
                  counter_adc0_l=0;            
                  counter_adc0_h=0;            
                  if(!level_adc0_m)
                  {
                     if(counter_adc0_m>=DELAYMC240) 
                         {
                             level_adc0_m=1;
                             level_adc0_l=0;            
                             level_adc0_h=0;
                             LED1_ON
                             ZonaS|=(1<<0);
                         }
                     else
                         {
                             counter_adc0_m++;    
                        }    
                  }
                 }
                 else
                 {            
                  if(MAX_VOLTAGE<adc_data[input_index])
                  {           
                   counter_adc0_l=0;            
                   counter_adc0_m=0;            
                   if(!level_adc0_h)
                   {
                      if(counter_adc0_h>=DELAYMC240) 
                        {
                            level_adc0_h=1;
                            level_adc0_m=0;
                            level_adc0_l=0;
                            LED1_OFF
                            ZonaS&=~(1<<0);
                            if(sign&&(ZoneMask&(1<<0)))
                            {
                                Zona|=(1<<0);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                      else
                        {
                            counter_adc0_h++;    
                        }
                   }             
                  }
                 }
                }            
              }
    break;
    case 1 : 
    {           
                if(MIN_VOLTAGE>adc_data[input_index])
                {
                 counter_adc1_m=0;                       
                 counter_adc1_h=0;            
                 if(!level_adc1_l)
                 {
                    if(counter_adc1_l>=DELAYMC240) 
                        {
                            level_adc1_l=1;
                            level_adc1_m=0;
                            level_adc1_h=0;  
                            LED2_OFF
                            ZonaS&=~(1<<1);
                            if(sign&&(ZoneMask&(1<<1)))
                            {
                                Zona|=(1<<1);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                    else
                        {
                            counter_adc1_l++;    
                        }    
                 }
                }
                else
                {            
                 if((adc_data[input_index]>MIN_VOLTAGE)&&(adc_data[input_index]<MAX_VOLTAGE))
                 {           
                  counter_adc1_l=0;            
                  counter_adc1_h=0;            
                  if(!level_adc1_m)
                  {
                     if(counter_adc1_m>=DELAYMC240) 
                         {
                             level_adc1_m=1;
                             level_adc1_l=0;            
                             level_adc1_h=0;
                             LED2_ON
                             ZonaS|=(1<<1);
                         }                
                     else
                         {
                             counter_adc1_m++;    
                        }    
                  }
                 }
                 else
                 {            
                  if(MAX_VOLTAGE<adc_data[input_index])
                  {           
                   counter_adc1_l=0;            
                   counter_adc1_m=0;            
                   if(!level_adc1_h)
                   {
                      if(counter_adc1_h>=DELAYMC240) 
                        {
                            level_adc1_h=1;
                            level_adc1_m=0;
                            level_adc1_l=0;
                            LED2_OFF
                            ZonaS&=~(1<<1);
                            if(sign&&(ZoneMask&(1<<1)))
                            {
                                Zona|=(1<<1);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                      else
                        {
                            counter_adc1_h++;    
                        }
                   }             
                  }
                 }
                }                
    }
    break;         
    case 2 : 
    {
                if(MIN_VOLTAGE>adc_data[input_index])
                {
                 counter_adc2_m=0;                       
                 counter_adc2_h=0;            
                 if(!level_adc2_l)
                 {
                    if(counter_adc2_l>=DELAYMC240) 
                        {
                            level_adc2_l=1;
                            level_adc2_m=0;
                            level_adc2_h=0;  
                            LED3_OFF
                            ZonaS&=~(1<<2);
                            if(sign&&(ZoneMask&(1<<2)))
                            {
                                Zona|=(1<<2);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }    
                        }
                    else
                        {
                            counter_adc2_l++;    
                        }    
                 }
                }
                else
                {            
                 if((adc_data[input_index]>MIN_VOLTAGE)&&(adc_data[input_index]<MAX_VOLTAGE))
                 {           
                  counter_adc2_l=0;            
                  counter_adc2_h=0;            
                  if(!level_adc2_m)
                  {
                     if(counter_adc2_m>=DELAYMC240) 
                         {
                             level_adc2_m=1;
                             level_adc2_l=0;            
                             level_adc2_h=0;
                             LED3_ON
                             ZonaS|=(1<<2);
                         }
                     else
                         {
                             counter_adc2_m++;    
                        }    
                  }
                 }
                 else
                 {            
                  if(MAX_VOLTAGE<adc_data[input_index])
                  {           
                   counter_adc2_l=0;            
                   counter_adc2_m=0;            
                   if(!level_adc2_h)
                   {
                      if(counter_adc2_h>=DELAYMC240) 
                        {
                            level_adc2_h=1;
                            level_adc2_m=0;
                            level_adc2_l=0;
                            LED3_OFF
                            ZonaS&=~(1<<2);
                            if(sign&&(ZoneMask&(1<<2)))
                            {
                                Zona|=(1<<2);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                      else
                        {
                            counter_adc2_h++;    
                        }
                   }             
                  }
                 }
                }            
    }
    break;
    case 3 :
    {                                  
                if(MIN_VOLTAGE>adc_data[input_index])
                {
                 counter_adc3_m=0;                       
                 counter_adc3_h=0;            
                 if(!level_adc3_l)
                 {  

                    if(counter_adc3_l>=DELAYMC240) 
                        {
                            level_adc3_l=1;
                            level_adc3_m=0;
                            level_adc3_h=0;  
                            LED4_OFF
                            ZonaS&=~(1<<3);
                            if(sign&&(ZoneMask&(1<<3)))
                            {
                                Zona|=(1<<3);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                    else
                        {
                            counter_adc3_l++;    
                        }    
                 }
                }
                else
                {            
                 if((adc_data[input_index]>MIN_VOLTAGE)&&(adc_data[input_index]<MAX_VOLTAGE))
                 {           
                  counter_adc3_l=0;            
                  counter_adc3_h=0;            
                  if(!level_adc3_m)
                  {
                     if(counter_adc3_m>=DELAYMC240) 
                         {
                             level_adc3_m=1;
                             level_adc3_l=0;            
                             level_adc3_h=0;
                             LED4_ON
                            ZonaS|=(1<<3);
                         }
                     else
                         {
                             counter_adc3_m++;    
                        }    
                  }
                 }
                 else
                 {            
                  if(MAX_VOLTAGE<adc_data[input_index])
                  {           
                   counter_adc3_l=0;            
                   counter_adc3_m=0;            
                   if(!level_adc3_h)
                   {
                      if(counter_adc3_h>=DELAYMC240) 
                        {
                            level_adc3_h=1;
                            level_adc3_m=0;
                            level_adc3_l=0;
                            LED4_OFF
                            ZonaS&=~(1<<3);
                            if(sign&&(ZoneMask&(1<<3)))
                            {
                                Zona|=(1<<3);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                      else
                        {
                            counter_adc3_h++;    
                        }
                   }             
                  }
                 }
                }            
    } ;
    break;
    case 4 : {                 
                if(MIN_VOLTAGE>adc_data[input_index])
                {
                 counter_adc4_m=0;                       
                 counter_adc4_h=0;            
                 if(!level_adc4_l)
                 {
                    if(counter_adc4_l>=DELAYMC240) 
                        {
                            level_adc4_l=1;
                            level_adc4_m=0;
                            level_adc4_h=0;  
                            LED5_OFF
                            ZonaS&=~(1<<4);
                            if(sign&&(ZoneMask&(1<<4)))
                            {
                                Zona|=(1<<4);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                    else
                        {
                            counter_adc4_l++;    
                        }    
                 }
                }
                else
                {            
                 if((adc_data[input_index]>MIN_VOLTAGE)&&(adc_data[input_index]<MAX_VOLTAGE))
                 {           
                  counter_adc4_l=0;            
                  counter_adc4_h=0;            
                  if(!level_adc4_m)
                  {
                     if(counter_adc4_m>=DELAYMC240) 
                         {
                             level_adc4_m=1;
                             level_adc4_l=0;            
                             level_adc4_h=0;
                             LED5_ON
                             ZonaS|=(1<<4);
                         }
                     else
                         {
                             counter_adc4_m++;    
                        }    
                  }
                 }
                 else
                 {            
                  if(MAX_VOLTAGE<adc_data[input_index])
                  {           
                   counter_adc4_l=0;            
                   counter_adc4_m=0;            
                   if(!level_adc4_h)
                   {
                      if(counter_adc4_h>=DELAYMC240) 
                        {
                            level_adc4_h=1;
                            level_adc4_m=0;
                            level_adc4_l=0;
                            LED5_OFF
                            ZonaS&=~(1<<4);
                            if(sign&&(ZoneMask&(1<<4)))
                            {
                                Zona|=(1<<4);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                      else
                        {
                            counter_adc4_h++;    
                        }
                   }             
                  }
                 }
                }            
              }
    break;
    case 5 : {                 
                if(MIN_VOLTAGE>adc_data[input_index])
                {
                 counter_adc5_m=0;                       
                 counter_adc5_h=0;            
                 if(!level_adc5_l)
                 {
                    if(counter_adc5_l>=DELAYMC240) 
                        {
                            level_adc5_l=1;
                            level_adc5_m=0;
                            level_adc5_h=0;  
                            LED6_OFF
                            ZonaS&=~(1<<5);
                            if(sign&&(ZoneMask&(1<<5)))
                            {
                                Zona|=(1<<5);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                    else
                        {
                            counter_adc5_l++;    
                        }    
                 }
                }
                else
                {            
                 if((adc_data[input_index]>MIN_VOLTAGE)&&(adc_data[input_index]<MAX_VOLTAGE))
                 {           
                  counter_adc5_l=0;            
                  counter_adc5_h=0;            
                  if(!level_adc5_m)
                  {
                     if(counter_adc5_m>=DELAYMC240) 
                         {
                             level_adc5_m=1;
                             level_adc5_l=0;            
                             level_adc5_h=0;
                             LED6_ON
                             ZonaS|=(1<<5);
                         }
                     else
                         {
                             counter_adc5_m++;    
                        }    
                  }
                 }
                 else
                 {            
                  if(MAX_VOLTAGE<adc_data[input_index])
                  {           
                   counter_adc5_l=0;            
                   counter_adc5_m=0;            
                   if(!level_adc5_h)
                   {
                      if(counter_adc5_h>=DELAYMC240) 
                        {
                            level_adc5_h=1;
                            level_adc5_m=0;
                            level_adc5_l=0;
                            LED6_OFF
                            ZonaS&=~(1<<5);
                            if(sign&&(ZoneMask&(1<<5)))
                            {
                                Zona|=(1<<5);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                      else
                        {
                            counter_adc5_h++;    
                        }
                   }             
                  }
                 }
                }            
              }
    break;
    case 6 : {                 
                if(MIN_VOLTAGE>adc_data[input_index])
                {
                 counter_adc6_m=0;                       
                 counter_adc6_h=0;            
                 if(!level_adc6_l)
                 {
                    if(counter_adc6_l>=DELAYMC240) 
                        {
                            level_adc6_l=1;
                            level_adc6_m=0;
                            level_adc6_h=0;  
                            LED7_OFF
                            ZonaS&=~(1<<6);
                            if(sign&&(ZoneMask&(1<<6)))
                            {
                                Zona|=(1<<6);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                    else
                        {
                            counter_adc6_l++;    
                        }    
                 }
                }
                else
                {            
                 if((adc_data[input_index]>MIN_VOLTAGE)&&(adc_data[input_index]<MAX_VOLTAGE))
                 {           
                  counter_adc6_l=0;            
                  counter_adc6_h=0;            
                  if(!level_adc6_m)
                  {
                     if(counter_adc6_m>=DELAYMC240) 
                         {
                             level_adc6_m=1;
                             level_adc6_l=0;            
                             level_adc6_h=0;
                             LED7_ON
                             ZonaS|=(1<<6);
                         }
                     else
                         {
                             counter_adc6_m++;    
                        }    
                  }
                 }
                 else
                 {            
                  if(MAX_VOLTAGE<adc_data[input_index])
                  {           
                   counter_adc6_l=0;            
                   counter_adc6_m=0;            
                   if(!level_adc6_h)
                   {
                      if(counter_adc6_h>=DELAYMC240) 
                        {
                            level_adc6_h=1;
                            level_adc6_m=0;
                            level_adc6_l=0;
                            LED7_OFF
                            ZonaS&=~(1<<6);
                            if(sign&&(ZoneMask&(1<<6)))
                            {
                                Zona|=(1<<6);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }    
                        }
                      else
                        {
                            counter_adc6_h++;    
                        }
                   }             
                  }
                 }
                }            
              }
    break;
    case 7 : {                 
                if(MIN_VOLTAGE>adc_data[input_index])
                {
                 counter_adc7_m=0;                       
                 counter_adc7_h=0;            
                 if(!level_adc7_l)
                 {
                    if(counter_adc7_l>=DELAYMC240) 
                        {
                            level_adc7_l=1;
                            level_adc7_m=0;
                            level_adc7_h=0;  
                            LED8_OFF
                            ZonaS&=~(1<<7);
                            if(sign&&(ZoneMask&(1<<7)))
                            {
                                Zona|=(1<<7);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                    else
                        {
                            counter_adc7_l++;    
                        }    
                 }
                }
                else
                {            
                 if((adc_data[input_index]>MIN_VOLTAGE)&&(adc_data[input_index]<MAX_VOLTAGE))
                 {           
                  counter_adc7_l=0;            
                  counter_adc7_h=0;            
                  if(!level_adc7_m)
                  {
                     if(counter_adc7_m>=DELAYMC240) 
                         {
                             level_adc7_m=1;
                             level_adc7_l=0;            
                             level_adc7_h=0;
                             LED8_ON
                             ZonaS|=(1<<7);
                         }
                     else
                         {
                             counter_adc7_m++;    
                        }    
                  }
                 }
                 else
                 {            
                  if(MAX_VOLTAGE<adc_data[input_index])
                  {           
                   counter_adc7_l=0;            
                   counter_adc7_m=0;            
                   if(!level_adc7_h)
                   {
                      if(counter_adc7_h>=DELAYMC240) 
                        {
                            level_adc7_h=1;
                            level_adc7_m=0;
                            level_adc7_l=0;
                            LED8_OFF
                            ZonaS&=~(1<<7);
                            if(sign&&(ZoneMask&(1<<7)))
                            {
                                Zona|=(1<<7);
                                ZonaE=Zona;
                                Eventsheduler(input_index);
                            }
                        }
                      else
                        {
                            counter_adc7_h++;    
                        }
                   }             
                  }
                 }
                }            
              }
    break;
    }; 
TIFR0=0x02;   
}

// Declare your global variables here

void main(void)
{
// Declare your local variables here
unsigned char i;
// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif
i=MCUSR&0x1F;
// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0xFF;
DDRA=0xFF;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=Out Func1=Out Func0=Out 
// State7=T State6=T State5=T State4=T State3=T State2=0 State1=0 State0=0 
PORTB=0xF0;
DDRB=0xF7;

// Port C initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x0F;
DDRC=0x30;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x20;

// Port E initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTE=0x00;
DDRE=0x10;

// Port F initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTF=0x00;
DDRF=0x00;

// Port G initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTG=0x0C;
DDRG=0x0C;

// Port H initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTH=0x00;
DDRH=0x7E;

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
// Func7=In Func6=In Func5=In Func4=In Func3=Out Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=0 State2=T State1=T State0=T 
PORTL=0x00;
DDRL=0x08;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 10,800 kHz
// Mode: CTC top=OCR0A
// OC0A output: Disconnected
// OC0B output: Disconnected
TCCR0A=0x02;
TCCR0B=0x05;
TCNT0=0x00;
OCR0A=0x36;
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
OCR1AH=0x2A;
OCR1AL=0x30;
OCR1BH=0x04;
OCR1BL=0x38;
OCR1CH=0x00;
OCR1CL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: 10,800 kHz
// Mode: Normal top=FFh
// OC2A output: Disconnected
// OC2B output: Disconnected
ASSR=0x00;
TCCR2A=0x00;
TCCR2B=0x07;
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
// Clock value: 11059,200 kHz
// Mode: Fast PWM top=00FFh
// OC5A output: Non-Inv.
// OC5B output: Discon.
// OC5C output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 5 Overflow Interrupt: On
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
// Compare C Match Interrupt: Off
TCCR5A=0x83;//81;
TCCR5B=0x09;
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
// INT0: On
// INT0 Mode: Rising Edge
// INT1: Off
// INT2: Off
// INT3: Off
// INT4: On
// INT4 Mode: Rising Edge
// INT5: Off
// INT6: On
// INT6 Mode: Rising Edge
// INT7: On
// INT7 Mode: Rising Edge
EICRA=0x03;
EICRB=0xF3;
EIMSK=0xD1;
EIFR=0xD1;

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
TIMSK0=0x02;
// Timer/Counter 1 Interrupt(s) initialization
TIMSK1=0x06;
// Timer/Counter 2 Interrupt(s) initialization
TIMSK2=0x00;
// Timer/Counter 3 Interrupt(s) initialization
TIMSK3=0x00;
// Timer/Counter 4 Interrupt(s) initialization
TIMSK4=0x00;
// Timer/Counter 5 Interrupt(s) initialization
TIMSK5=0x01;

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

// USART1 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART1 Receiver: On
// USART1 Transmitter: On
// USART1 Mode: Asynchronous
// USART1 Baud rate: 115200
UCSR1A=0x00;
UCSR1B=0xD8;
UCSR1C=0x06;
UBRR1H=0x00;
UBRR1L=0x05;

// USART2 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART2 Receiver: On
// USART2 Transmitter: Off
// USART2 Mode: Asynchronous
// USART2 Baud rate: 115200
UCSR2A=0x00;
UCSR2B=0x90;
UCSR2C=0x06;
UBRR2H=0x00;
UBRR2L=0x05;
// USART3 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART3 Receiver: On
// USART3 Transmitter: Off
// USART3 Mode: Asynchronous
// USART3 Baud rate: 9600
UCSR3A=0x00;
UCSR3B=0x90;
UCSR3C=0x06;
UBRR3H=0x00;
UBRR3L=0x47;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
ADCSRB=0x00;

// ADC initialization
// ADC Clock frequency: 691,200 kHz
// ADC Voltage Reference: AVCC pin
// ADC Auto Trigger Source: Timer0 Compare Match
// Only the 8 most significant bits of
// the AD conversion result are used
// Digital input buffers on ADC0: Off, ADC1: Off, ADC2: Off, ADC3: Off
// ADC4: Off, ADC5: Off, ADC6: Off, ADC7: Off
DIDR0=0xFF;
// Digital input buffers on ADC8: On, ADC9: On, ADC10: On, ADC11: On
// ADC12: On, ADC13: On, ADC14: On, ADC15: On
DIDR2=0x00;
ADMUX=(FIRST_ADC_INPUT & 0x07) | (ADC_VREF_TYPE & 0xff);
ADCSRA=0xAC;
ADCSRB&=0xF8;
ADCSRB |=0x03 | (FIRST_ADC_INPUT & 0x08);

// SPI initialization
// SPI Type: Master
// SPI Clock Rate: 2*2764,800 kHz
// SPI Clock Phase: Cycle Start
// SPI Clock Polarity: High
// SPI Data Order: MSB First
SPCR=0x5C;
SPSR=0x01;                      

// Watchdog Timer initialization
// Watchdog Timer Prescaler: OSC/256k
// Watchdog Timer interrupt: Off
#pragma optsize-
#asm("wdr")
WDTCSR=0x1F;
WDTCSR=0x0F;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

pEEPROM=0;
for(i=0;i!=11;i++)
 FHandContr[i]=(*(pEEPROM+HND+i));
FHandContr[11]=0;

eeprom2ram();

H_SIM_ON
delay_ms(10);
if(sign_E)
{
    sign=sign_on=1;
    GETSR=1;
}
if(Zona)
{
    SInfo=1;
    StatusSMS=SMSE;
    StatusCall=CALE;
    AttemptCall=NUMCALL;
    AttemptSMS=AMOUNT_ATTEMPT_SMS;
}                          

// Global enable interrupts
#asm("sei")
SimPowerOff();
SendAnswer(&F_VERSIA[6]);
LED_ON 
SimPowerOn();  

SendCommand("AT");        
delay_ms(1000);
SendCommand("AT");        

AT45DB161_init();

snd_stop();
snd_load_info();

while (1)
      {
      // Place your code here                           
      #asm("wdr")
      // Check receive byte with GSM
      if(GSM_PACK)              HandlerEventGSM(); 
      else
      {
        if(rx_buffer_overflowGSM)   {gsm_rx_counter=0;rx_buffer_overflowGSM=0;}
        // Check receive byte with GSM
        if(rx_counterGSM)         Receive_gsm();
        // Check receive packet with GSM
        if(GSM_PACK) continue;
      }          
      if(rx_counterGSM) continue;
      if(status_sim&&(!TimeGuard)&&(!CounterGSM))
      {
        if(status_sim&S_RING)   if(F_Ring())status_sim&=~S_RING;
        if(status_sim&S_SMS)    if(F_SMS()) status_sim&=~S_SMS;
        if(status_sim&S_CRDY)   if(F_Crdy())status_sim&=~S_CRDY;
        if(status_sim&S_ERROR)  {SimHardReset();status_sim&=~S_ERROR;}
        if(status_sim&S_CREG)   {CurrentNetwork=1;status_sim&=~S_CREG;}        
        if(status_sim&S_CREGNOT){CurrentNetwork=0;status_sim&=~S_CREGNOT;}        
      }                
      if((!work_modem)&&(!TimeGuard)&&(!CounterGSM))
      {
        if(C_AT()) SimHardReset();
        work_modem=120;
      }
      if((!ChangeRegNetwork)&&(!TimeGuard)&&(!CounterGSM))
      {
        switch (C_CREGG()) {
            case C_REG_NET : CurrentNetwork=1;
            break;
            case C_NOT_REG_NET :CurrentNetwork=0;
            break;       
        };               
        ChangeRegNetwork=240;
      }  
      if(!RCallRDY) 
        if(!rdy_counter) SimHardReset();
      if(SInfo&&(!TimeGuard)&&(!CounterGSM)) SendInfo();
      // Check buffer overflow with PC
      if(rx_buffer_overflowPC)  {StartFlagPC=0;rx_buffer_overflowPC=0;}
      // Check receive byte with PC
      if(rx_counterPC)          Receive_PC();  
      if(rx_counterM) pc_comm();
};
}                                       
void SimHardReset(void)
{
    #asm("wdr")
    SimPowerOff();
    H_SIM_OFF
    #asm("cli")
    gsm_rx_counter=0;
    GSM_PACK=0;
    RCallRDY=0;                                                 
    status_sim=0;
    rdy_counter=30;
    work_modem=120;
    while(rx_counterGSM) getcharGSM();
    #asm("sei")
    delay_ms(5);
    H_SIM_ON
    SimPowerOn();  
    SendCommand("AT");        
    delay_ms(1000);
    SendCommand("AT");            
}

/**
 * Прием и обработка пакета от PC  
 **/
void Receive_PC(void)
{
 unsigned char data;
 #asm("wdr")
 data=getcharPC(); 
  switch (data) {
    case '$': { StartFlagPC=1; counter_PC=0; }
    break;
    case '\n': 
        if(StartFlagPC)
        {  
         LED_ON
         StartFlagPC=0;
         buffer_PC[counter_PC]=0;
         counter_PC=0;
         data=0;
         while((buffer_PC[counter_PC] != '*')&&(buffer_PC[counter_PC] != '\n')) data^=buffer_PC[counter_PC++];
       	 if((tascii[(data&0xF0)>>4]==buffer_PC[++counter_PC])&&(tascii[(data&0x0F)]==buffer_PC[++counter_PC]))
         {
            if(!strncmpf(buffer_PC, "PCHND", 5))
	        { SendAnswerR(FHandContr); return; }
    		if(!strncmpf(buffer_PC, "PCVER", 5))
            { SendAnswerF(F_VERSIA); return; }
		    if(!strncmpf(buffer_PC, "PCSUN", 5))
		    { SetUserNumber(&buffer_PC[6]);	return;	}
		    if(!strncmpf(buffer_PC, "PCGUN", 5))
		    { GetUserNumber(&buffer_PC[6]);	return;	}
		    if(!strncmpf(buffer_PC, "PCSCD", 5))
		    { SetCard(&buffer_PC[6]);	return;	}
		    if(!strncmpf(buffer_PC, "PCGCD", 5))
		    { GetCard(&buffer_PC[6]);	return;	}
   		    if(!strncmpf(buffer_PC, "PCSTA", 5))
		    { SetTimeActive(&buffer_PC[6]);	return;	}
   		    if(!strncmpf(buffer_PC, "PCGTA", 5))
		    { GetTimeActive();	return;	}
   		    if(!strncmpf(buffer_PC, "PCSTD", 5))
		    { SetTimeDisactive(&buffer_PC[6]);	return;	}
   		    if(!strncmpf(buffer_PC, "PCGTD", 5))
		    { GetTimeDisactive();	return;	}
   		    if(!strncmpf(buffer_PC, "PCSON", 5))
		    { SetOptionSG(&buffer_PC[6]);	return;	}
   		    if(!strncmpf(buffer_PC, "PCGON", 5))
		    { GetOptionSG();	return;	}
   		    if(!strncmpf(buffer_PC, "PCSCS", 5))
		    { SetConfiguration(&buffer_PC[6]);	return;	}
   		    if(!strncmpf(buffer_PC, "PCGCS", 5))
		    { GetConfiguration();	return;	}
   		    if(!strncmpf(buffer_PC, "PCSAZ", 5))
		    { SetActiveZone(&buffer_PC[6]);	return;	}
   		    if(!strncmpf(buffer_PC, "PCGAZ", 5))
		    { GetActiveZone();	return;	}
   		    if(!strncmpf(buffer_PC, "PCSFS", 5))
		    { SetFactorySetting(); return;	}   	    
   			if(!strncmpf(buffer_PC, "PCSTE", 5))
		    { SetTimmingSourceEv(&buffer_PC[6]);	return;	}
		    if(!strncmpf(buffer_PC, "PCGTE", 5))
		    { GetTimmingSourceEv(&buffer_PC[6]);	return;	}
   			if(!strncmpf(buffer_PC, "PCUSD", 5))
		    { C_USSD(&buffer_PC[6]);	return;	}
    		if(!strncmpf(buffer_PC, "PBLFL", 5))
		    {        
		        #asm("cli")
                pEEPROM=0;
                *(pEEPROM+CRCEH)=0xFF;
                *(pEEPROM+CRCEL)=0xFF;
 	            while(1);
            }
	        SendAnswerF(F_COMMANDNOTSUPPORT);
	        return; 
		 }   
	}
    break;  
    default: 
        if(StartFlagPC)
        {
         buffer_PC[counter_PC++]=data;
        }  
    };
}   
                
/**
 * посылка строки на PC  
 *
 * @param	*data       указатель на строку во Flash памяти
 **/
void SendAnswer(flash char *data)
{
    unsigned char i=0;
    
    do
    {     
        #asm("wdr")
        putcharPC(data[i]);
    }while(data[++i]); 
}                               

/**
 * посылка пакета в виде строки на PC
 *
 * @param	*data	указатель на строку во Flash памяти  
 **/
void SendAnswerF(flash char *data){
 unsigned char i=0;
 unsigned char checkbyte=0;
  putcharPC('$'); 
  do {     
   #asm("wdr")
   checkbyte^=data[i];
   putcharPC(data[i]);
  }while(data[++i]); 
   putcharPC('*');
   putcharPC(tascii[(checkbyte&0xF0)>>4]);
   putcharPC(tascii[checkbyte&0x0F]);
   putcharPC('\r');
   putcharPC('\n');     
}

/**
 * посылка пакета в виде строки на PC
 *
 * @param	*data	указатель на строку в RAM памяти  
 **/
void SendAnswerR(char *data){
 unsigned char i=0;
 unsigned char checkbyte=0;
  putcharPC('$'); 
  do {     
   #asm("wdr")
   checkbyte^=data[i];
   putcharPC(data[i]);
  }while(data[++i]);
  putcharPC('*');
   putcharPC(tascii[(checkbyte&0xF0)>>4]);
   putcharPC(tascii[checkbyte&0x0F]);
   putcharPC('\r');
   putcharPC('\n');     
}

/**
 * установка номера абонента
 *
 * @param	*mas	указатель на параметры абонента
 *                  1 байт порядковый номер абонента (от 0 до 4)
 *                  2..11 байт номер абонента (0683332211)
 **/
void SetUserNumber(unsigned char *mas){
 unsigned char i;
 #asm("wdr")          
 if(0<=(mas[0]-0x30)<5)
 {
    for(i=0;i!=10;i++)
        NUMAR[mas[0]-0x30][i]=NUMAE[mas[0]-0x30][i]=mas[i+1];
    NUMAE[mas[0]-0x30][10]=0;        
    NUMAR[mas[0]-0x30][10]=0;        
 }                      
 else
 {
  SendAnswerF("SYSUN ERROR");
  return;
 }
 SendAnswerF(F_SETUSERNUMBER);
}

/**
 * считывание номера абонента
 *
 * @param	*a  	указатель на параметры абонента
 *                  1 байт порядковый номер абонента (от 0 до 4)
 **/
void GetUserNumber(unsigned char *a){
 unsigned char i;
 unsigned char mas[18]; 
 #asm("wdr")          
 if(0<=(a[0]-0x30)<5)
 {      
    mas[0]='S';
    mas[1]='Y';
    mas[2]='G';
    mas[3]='U';
    mas[4]='N';
    mas[5]=' ';
    mas[6]=a[0];
    for(i=0;i!=10;i++)
        mas[i+7]=NUMAE[a[0]-0x30][i];
    mas[17]=0;        
    SendAnswerR(&mas[0]);    
 }
 else
 {
    SendAnswerF("SYGUN ERROR");   
 }
}

/**
 * Установка карточек   
 **/      
void SetCard(unsigned char *mas){
 unsigned char temp;
 #asm("wdr")
 temp=Char2Hexh(mas[0])|Char2Hexl(mas[1]);   
 CardE[temp][0]=Card[temp][0]=(Char2Hexh(mas[2])|Char2Hexl(mas[3]));
 CardE[temp][1]=Card[temp][1]=(Char2Hexh(mas[4])|Char2Hexl(mas[5]));
 CardE[temp][2]=Card[temp][2]=(Char2Hexh(mas[6])|Char2Hexl(mas[7]));
 SendAnswerF(F_SETCARD);
}
/**
 * Считывание карточек   
 **/      
void GetCard(unsigned char *mas){
 unsigned char temp;
 unsigned char mas1[13]; 
 #asm("wdr")
 temp=Char2Hexh(mas[0])|Char2Hexl(mas[1]);   
 mas1[0]='S';
 mas1[1]='Y';
 mas1[2]='G';
 mas1[3]='C';
 mas1[4]='D';
 mas1[5]=' ';
 mas1[6]=tascii[(Card[temp][0]&0xF0)>>4];
 mas1[7]=tascii[(Card[temp][0]&0x0F)];
 mas1[8]=tascii[(Card[temp][1]&0xF0)>>4];
 mas1[9]=tascii[(Card[temp][1]&0x0F)];
 mas1[10]=tascii[(Card[temp][2]&0xF0)>>4];
 mas1[11]=tascii[(Card[temp][2]&0x0F)];
 mas1[12]=0;
 SendAnswerR(mas1);
}

/**
 * Set delta move write coordinate event   
 **/      
void SetTimeActive(unsigned char *mas){
 #asm("wdr")  
 SETSE=Char2Hexh(mas[0])|Char2Hexl(mas[1]);
SendAnswerF(F_SETTIMEACTIVE);
}
/**
 * Get delta move write coordinate event   
 **/      
void GetTimeActive(void){
 unsigned char mas[9]; 
 #asm("wdr")           
 mas[0]='S';
 mas[1]='Y';
 mas[2]='G';
 mas[3]='T';
 mas[4]='A';
 mas[5]=' ';
 mas[8]=SETSE;
 mas[6]=tascii[(mas[8]&0xF0)>>4];
 mas[7]=tascii[(mas[8]&0x0F)];
 mas[8]=0;
 SendAnswerR(mas);
}
/**
 * Set delta move write coordinate event   
 **/      
void SetTimeDisactive(unsigned char *mas){
 #asm("wdr")          
 GETSE=Char2Hexh(mas[0])|Char2Hexl(mas[1]);
 SendAnswerF(F_SETTIMEDISACTIVE);
}
/**
 * Get delta move write coordinate event   
 **/      
void GetTimeDisactive(){
 unsigned char mas[9];
 #asm("wdr")          
 mas[0]='S';
 mas[1]='Y';
 mas[2]='G';
 mas[3]='T';
 mas[4]='D';
 mas[5]=' ';
 mas[8]=GETSE;
 mas[6]=tascii[(mas[8]&0xF0)>>4];
 mas[7]=tascii[(mas[8]&0x0F)];
 mas[8]=0;
 SendAnswerR(mas);
}                                   

/**
 * Установка   
 **/      
void SetTimmingSourceEv(unsigned char *mas){
 unsigned char temp,i=0;            
 #asm("wdr")
 #asm("cli")
 temp = Char2Hexh(mas[i++])|Char2Hexl(mas[i++]);          
 SourceEvE[temp].ExecuteDevice=SourceEv[temp].ExecuteDevice=Char2Hexh(mas[i++])|Char2Hexl(mas[i++]); 
 SourceEvE[temp].CounterRelay1_on=SourceEv[temp].CounterRelay1_on=((unsigned int)(Char2Hexh(mas[i++])|Char2Hexl(mas[i++]))<<8)|(Char2Hexh(mas[i++])|Char2Hexl(mas[i++]));
 SourceEvE[temp].CounterRelay2_on=SourceEv[temp].CounterRelay2_on=((unsigned int)(Char2Hexh(mas[i++])|Char2Hexl(mas[i++]))<<8)|(Char2Hexh(mas[i++])|Char2Hexl(mas[i++])); ; 
 SourceEvE[temp].CounterOpenDrain1_on=SourceEv[temp].CounterOpenDrain1_on=((unsigned int)(Char2Hexh(mas[i++])|Char2Hexl(mas[i++]))<<8)|(Char2Hexh(mas[i++])|Char2Hexl(mas[i++]));
 SourceEvE[temp].CounterOpenDrain2_on=SourceEv[temp].CounterOpenDrain2_on=((unsigned int)(Char2Hexh(mas[i++])|Char2Hexl(mas[i++]))<<8)|(Char2Hexh(mas[i++])|Char2Hexl(mas[i++])); 
 SourceEvE[temp].CounterRelay1_off=SourceEv[temp].CounterRelay1_off=((unsigned int)(Char2Hexh(mas[i++])|Char2Hexl(mas[i++]))<<8)|(Char2Hexh(mas[i++])|Char2Hexl(mas[i++])); ; 
 SourceEvE[temp].CounterRelay2_off=SourceEv[temp].CounterRelay2_off=((unsigned int)(Char2Hexh(mas[i++])|Char2Hexl(mas[i++]))<<8)|(Char2Hexh(mas[i++])|Char2Hexl(mas[i++])); ; 
 SourceEvE[temp].CounterOpenDrain1_off=SourceEv[temp].CounterOpenDrain1_off=((unsigned int)(Char2Hexh(mas[i++])|Char2Hexl(mas[i++]))<<8)|(Char2Hexh(mas[i++])|Char2Hexl(mas[i++])); 
 SourceEvE[temp].CounterOpenDrain2_off=SourceEv[temp].CounterOpenDrain2_off=((unsigned int)(Char2Hexh(mas[i++])|Char2Hexl(mas[i++]))<<8)|(Char2Hexh(mas[i++])|Char2Hexl(mas[i++])); 
 #asm("sei")
 SendAnswerF(F_SETIMMING_SRC_EV);
}            
/**
 * Считывание    
 **/      
void GetTimmingSourceEv(unsigned char *mas){
 unsigned char i=0,temp;
 unsigned char mas1[41];
 #asm("wdr")          
 temp = Char2Hexh(mas[0])|Char2Hexl(mas[1]);          
 mas1[i++]='S';
 mas1[i++]='Y';
 mas1[i++]='G';
 mas1[i++]='T';
 mas1[i++]='E';
 mas1[i++]=' '; 
 mas1[i++]=tascii[(SourceEv[temp].ExecuteDevice&0xF0)>>4];
 mas1[i++]=tascii[(SourceEv[temp].ExecuteDevice&0x0F)];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay1_on&0xF000)>>12];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay1_on&0x0F00)>>8];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay1_on&0x00F0)>>4];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay1_on&0x000F)];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay2_on&0xF000)>>12];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay2_on&0x0F00)>>8];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay2_on&0x00F0)>>4];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay2_on&0x000F)];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain1_on&0xF000)>>12];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain1_on&0x0F00)>>8];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain1_on&0x00F0)>>4];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain1_on&0x000F)];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain2_on&0xF000)>>12];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain2_on&0x0F00)>>8];  
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain2_on&0x00F0)>>4];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain2_on&0x000F)];  
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay1_off&0xF000)>>12];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay1_off&0x0F00)>>8];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay1_off&0x00F0)>>4];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay1_off&0x000F)];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay2_off&0xF000)>>12];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay2_off&0x0F00)>>8];   
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay2_off&0x00F0)>>4];
 mas1[i++]=tascii[(SourceEv[temp].CounterRelay2_off&0x000F)];   
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain1_off&0xF000)>>12];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain1_off&0x0F00)>>8];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain1_off&0x00F0)>>4];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain1_off&0x000F)];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain2_off&0xF000)>>12];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain2_off&0x0F00)>>8];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain2_off&0x00F0)>>4];
 mas1[i++]=tascii[(SourceEv[temp].CounterOpenDrain2_off&0x000F)];
 mas1[i]=0;
 SendAnswerR(mas1);
}

/**
 * Установка активных зон   
 **/      
void SetActiveZone(unsigned char *mas){
 #asm("wdr")          
 ZoneMaskE = ZoneMask = Char2Hexh(mas[0])|Char2Hexl(mas[1]);
 sign=0;        
 sign_E=0;
 SendAnswerF(F_SETACTIVEZONE);
}
/**
 * Считывание активных зон   
 **/      
void GetActiveZone(){
 unsigned char mas[9];
 #asm("wdr")          
 mas[0]='S';
 mas[1]='Y';
 mas[2]='G';
 mas[3]='A';
 mas[4]='Z';
 mas[5]=' '; 
 mas[6]=tascii[(ZoneMask&0xF0)>>4];
 mas[7]=tascii[(ZoneMask&0x0F)];
 mas[8]=0;
 SendAnswerR(mas);
}

/**
 * Установка параметров постановки и снятия с сигнализации   
 **/      
void SetOptionSG(unsigned char *mas){
 #asm("wdr")          
 #asm("cli")
 ConditionSGE = ConditionSG = Char2Hexh(mas[0])|Char2Hexl(mas[1]);
 #asm("sei")
 SendAnswerF(F_SETCONDITIONSG);
}
/**
 * Считывание параметров постановки и снятия с сигнализации      
 **/      
void GetOptionSG(){
 unsigned char mas[9];
 #asm("wdr")          
 mas[0]='S';
 mas[1]='Y';
 mas[2]='G';
 mas[3]='O';
 mas[4]='N';
 mas[5]=' '; 
 mas[6]=tascii[(ConditionSG&0xF0)>>4];
 mas[7]=tascii[(ConditionSG&0x0F)];
 mas[8]=0;
 SendAnswerR(mas);
}

/**
 * Set delta move write coordinate event   
 **/      
void SetConfiguration(unsigned char *mas){
 #asm("wdr")     
 SMSE=Char2Hexh(mas[0])|Char2Hexl(mas[1]);
 CALE=Char2Hexh(mas[2])|Char2Hexl(mas[3]);
 SendAnswerF(F_SETCONFIGURATION);
}
/**
 * Get delta move write coordinate event   
 **/      
void GetConfiguration(){
 unsigned char mas[11];
 #asm("wdr")          
 mas[0]='S';
 mas[1]='Y';
 mas[2]='G';
 mas[3]='C';
 mas[4]='S';
 mas[5]=' ';
 mas[10]=SMSE;
 mas[6]=tascii[(mas[10]&0xF0)>>4];
 mas[7]=tascii[(mas[10]&0x0F)];
 mas[10]=CALE;
 mas[8]=tascii[(mas[10]&0xF0)>>4];
 mas[9]=tascii[(mas[10]&0x0F)];
 mas[10]=0;
 SendAnswerR(mas);
}

/**
 * Load with EEPROM to RAM setting.  
 **/             
void eeprom2ram(void)
{
 unsigned char i,j;
 unsigned char *pRAM;
 #asm("wdr")         
 for(j=0;j!=NUMA_NUMBER;j++)
  for(i=0;i!=11;i++)           
   NUMAR[j][i]=NUMAE[j][i];
 ZoneMask=ZoneMaskE;              
 for(j=0;j!=NUMCARD;j++)
  for(i=0;i!=3;i++)           
   Card[j][i]=CardE[j][i];
 pEEPROM=&SourceEvE;
 pRAM=&SourceEv;
 for(i=0;i!=sizeof(SourceEv);i++)
    *(pRAM+i)=*(pEEPROM+i);
 ConditionSG=ConditionSGE;
 Zona=ZonaE;
}             

void SetFactorySetting()
{
 unsigned char i,j;
 unsigned char *pRAM;
 #asm("wdr")         
 for(j=0;j!=NUMA_NUMBER;j++)
 {
    for(i=0;i!=10;i++)           
        NUMAR[j][i]=NUMAE[j][i]='0';
    NUMAR[j][10]=NUMAE[j][10]=0;
 }      
 ZoneMaskE=ZoneMask=0xFF;
 sign=0;        
 sign_E=0;
 SMSE=0;  
 CALE=0;
 for(i=0;i!=6;i++)           
  PWDE[i]='0';
 #asm("cli")      
 SETSE=10;
 Zona=ZonaE=0;
 GETSE=10;     
 for(j=0;j!=NUMCARD;j++)
  for(i=0;i!=3;i++)           
   Card[j][i]=CardE[j][i]=0xFF;
 pEEPROM=&SourceEvE;
 pRAM=&SourceEv;
 for(i=0;i!=sizeof(SourceEv);i++)
    *(pRAM+i)=*(pEEPROM+i)=0;
 ConditionSGE=ConditionSG=0;
 #asm("sei")                               
 SendAnswerF("SYSFS OK");
}

/**
 * Check status signaliz. if active, then 3 tones sending, disactive 1 tone sending  
 **/

void StatusS(void)
{
 #asm("wdr")  
 C_CHFA1();
 #asm("cli")
 menu_index=0;          
 clear_list();
 if(sign) put_play_list(M_SYSTEM_ACT);
 else put_play_list(M_SYSTEM_DACT);
 put_play_list(M_MAIN_MENU);
 gsm_counter_AC=DELAY_CC;
 GSM_PACK=0;
 #asm("sei")
 while(gsm_counter_AC)
 {
  #asm("wdr")
    while((!GSM_PACK)&&(gsm_counter_AC))
    {                
        Receive_gsm();
        #asm("wdr");
        if(VoiceChannel_0){C_CHFA0();VoiceChannel_0=0;}
        if(VoiceChannel_1){C_CHFA1();VoiceChannel_1=0;}
        if(Zona)gsm_counter_AC=0;
    }                             
    if(GSM_PACK)   
    {
        if ((strstrf(&gsm_rx_buffer[0],"NO CARRIER")!=NULL))      // No Carrier  
        { 
            GSM_PACK=0; return;
        }
    }
    else
    {  
        if(C_ATH())return;            
    }
    GSM_PACK=0;
 } 
}       
unsigned char SMSF(void)
{                                                                  
 unsigned char i,a;
 unsigned char str1[62]; 
 str1[0]=0;
 strcatf(str1,"Alarm:");
 for(i=0;i!=NUMA_NUMBER;i++)
 {
  #asm("wdr")                          
    if((NUMAR[i][1]!=0x30)&&(StatusSMS&(1<<i)))
    {                          
        switch (C_CREGG()) {
            case C_REG_NET :
            {                    
                CurrentNetwork=1;
                str1[6]=0;
                for(a=0;a!=8;a++)
                    if((Zona&ZoneMask)&(1<<a)) strcatf(str1,&str_zone[a][0]);     
                if(SendSMS(str1,&NUMAR[i][0])) {AttemptSMS--;return 0;}
                else StatusSMS&=~(1<<i);
            }
            break;
            case C_NOT_REG_NET :
            {
                CurrentNetwork=0;
                return 0;   
            }
            break;
            default: return 0;       
         }; 
    }    
 }
 if(SMSE) C_CMGDA();  
 return 1;
}         

void SendInfo(void)
{
    if(CurrentNetwork)
    {
        if(StatusSMS&&AttemptSMS)
        {        
            if(SMSF()) StatusSMS=0; 
            else  return;
        }
        if(StatusCall&&AttemptCall)
        {
            SendCall();
        }
    }
    if((!(StatusCall&&AttemptCall))&&(!(StatusSMS&&AttemptSMS)))
    {             
        #asm("cli")
        Zona=0;
        ZonaE=0;   
        SInfo=0;
        #asm("sei")
        sign=0;
        SETSR=0;
        sign_on=1;
    }
}                    

/**
 * Send abonents information SMS and Call abonents.  
 **/             
void SendCall(void)
{             
 unsigned char i,a;           
 while(StatusCall&&AttemptCall)
 {
 #asm("wdr")
 for(i=0;i!=NUMA_NUMBER;i++)
 {
  if((NUMAR[i][1]!=0x30)&&(StatusCall&(1<<i)))
  {          
   #asm("cli")
   menu_index=0;   
   clear_list();
   #asm("sei")
   delay_ms(1000);
   #asm("wdr");
   switch (C_CREGG()) {
        case C_REG_NET :
        {                    
            CurrentNetwork=1;
            gsm_counter_AC=10;
            while(1)
            {               
                #asm("wdr")
                if(gsm_counter_AC)
                {
                    if(!C_CLCC())
                        if(!(S_CLCC.stat))    
                           break;
                }
                else
                    { AttemptCall--; return ;}
                delay_ms(1000);         
            }
            C_CHFA1();
            if(!(C_ATDD(&NUMAR[i][0])))
            {
                gsm_counter_AC=DELAY_C;
                GSM_PACK=0;        
                while((!GSM_PACK)&&(gsm_counter_AC))
                {          
                    #asm("wdr") 
                    if(CounterGSM)
                        Receive_gsm();
                    else
                    {
                        if(C_CLCC())return;
                        if(S_CLCC.stat=='0')
                        {
                            StatusCall&=~(1<<i);
                            #asm("cli")
                            clear_list();
                            put_play_list(M_A_ALARM);    
                            for(a=0;a!=8;a++)
                                if((Zona&ZoneMask)&(1<<a))  put_play_list(M_A_1+a);    
                            put_play_list(M_MAIN_MENU);    
                            CounterGSM=0;
                            #asm("sei")
                            break;    
                        }
                        if(S_CLCC.stat=='4') return;
                        CounterGSM=200;
                    }
                }                             
                while((!GSM_PACK)&&(gsm_counter_AC))
                {          
                    #asm("wdr") 
                    Receive_gsm();
                    if(VoiceChannel_0){C_CHFA0();VoiceChannel_0=0;}
                    if(VoiceChannel_1){C_CHFA1();VoiceChannel_1=0;}
                }                                     
                if(GSM_PACK)   
                {
                    if ((strstrf(&gsm_rx_buffer[0],"BUSY")!=NULL))           // Busy  
                    { 
                        if(S_CLCC.stat=='3')
                            StatusCall&=~(1<<i);
                    }
                    if ((strstrf(&gsm_rx_buffer[0],"NO ANSWER")!=NULL))           // Busy  
                    { 
                        StatusCall&=~(1<<i);
                    }
                }
                else
                {  
                    if(C_ATH())return;            
                }
                GSM_PACK=0;
                delay_ms(1000);
            }
            else return;
        }
        break;
        case C_NOT_REG_NET :
        {
            CurrentNetwork=0;
            return ;   
        }
        break;
        default: return ;       
   }; 
  }
 }             
 AttemptCall--;
 }          
}

unsigned char Char2Hexh(unsigned char byte){        
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
  }
}  

unsigned char Char2Hexl(unsigned char byte){        
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
  } 
}
void Eventsheduler(unsigned char type)
{            
    #asm("wdr")
    if(SourceEv[type].ExecuteDevice&0x01)
    {
        CounterRelay1_on=SourceEv[type].CounterRelay1_on;
        CounterRelay1_off=CounterRelay1_on+SourceEv[type].CounterRelay1_off;
        if((!CounterRelay1_on)&&(CounterRelay1_off)) RELAY1_ON
    }
    if(SourceEv[type].ExecuteDevice&0x02)
    {
        CounterRelay2_on=SourceEv[type].CounterRelay2_on;
        CounterRelay2_off=CounterRelay2_on+SourceEv[type].CounterRelay2_off;
        if((!CounterRelay2_on)&&(CounterRelay2_off)) RELAY2_ON
    }
    if(SourceEv[type].ExecuteDevice&0x04)
    {
        CounterOpenDrain1_on=SourceEv[type].CounterOpenDrain1_on;
        CounterOpenDrain1_off=CounterOpenDrain1_on+SourceEv[type].CounterOpenDrain1_off;
        if((!CounterOpenDrain1_on)&&(CounterOpenDrain1_off)) OPENDRAIN1_ON 
    }
    if(SourceEv[type].ExecuteDevice&0x08)
    {
        CounterOpenDrain2_on=SourceEv[type].CounterOpenDrain2_on;
        CounterOpenDrain2_off=CounterOpenDrain2_on+SourceEv[type].CounterOpenDrain2_off;
        if((!CounterOpenDrain2_on)&&(CounterOpenDrain2_off)) OPENDRAIN2_ON 
    }                          
}
                                      
/**
 * Check identifier card user  
 *
 * @param	*mas_iden       a pointer to the data: user identifier 
 *
 * @return	0	        not card register 
 * @return	1	        card register 
 **/
unsigned char cpi_ram(unsigned char *mas){
 unsigned char num;
 unsigned char i;
  #asm("wdr")       
  for(num=0;num!=NUMCARD;num++)
   {
    #asm("wdr") 
    i=0;         
      if(Card[num][i++]==mas[0])
       if(Card[num][i++]==mas[1])
        if(Card[num][i]==mas[2])     
         return 1;
   }
 return 0;
}

/**
 * Handler event as "RING".  
 *
 * @return	OK	if number register in base 
 **/
unsigned char F_Ring(void)
{      
    unsigned char i;
    #asm("wdr") 
    if(C_CLCC())return;
    for(i=0;i!=NUMA_NUMBER;i++)
    { 
        if(strstr(&S_CLCC.number[0],&NUMAR[i][0])!=NULL)
        {
            GSM_PACK=0;
            C_ATA();  
            StatusS();
            gsm_counter_AC=0;
            return 1;                              
        }
    }
    C_ATH();
    return 1;                              
}

/**
 * Handler event as "Call ready".  
 *
 * @return	OK	if number register in base 
 **/
unsigned char F_Crdy(void)
{  
    #asm("wdr") 
    if(C_ATE0()) ;//printf("\r\nC_ATE0");     
    if(C_CNMI()) ;//printf("\r\nC_CNMI");          
    delay_ms(1000);
    if(C_CFUN()) ;//printf("\r\nC_CFUN");     
    if(C_CMGF()) ;//printf("\r\nC_CMGF");                  
    if(C_CSMP()) ;//printf("\r\nC_CSMP");
    if(C_CREG()) ;//printf("\r\nC_CREG");
    if(C_CMIC1()) ;//printf("\r\nC_CMIC1");
    if(C_CMIC0()) ;//printf("\r\nC_CMIC0");
    if(C_CHFA1()) ;//printf("\r\nC_CHFA1");
    if(C_CLVL())  ;//printf("\r\nC_CLVL");    
    if(C_ATE0V1()) ;//printf("\r\nC_ATE0V1");
    if(C_CUSD()) ;//printf("\r\nC_CUSD");
    RCallRDY=1;
    return 1;                              
}   

/**
 * Handler event as "Receive SMS".  
 *
 * @return	OK	if number register in base 
 **/ 
unsigned char F_SMS(void)
{                     
    unsigned char i;           
    unsigned char mas[240];
    unsigned char *pointer;
    #asm("wdr") 
    C_CMGLU(&mas[0]);    
    
    pointer=strstrf(&mas[0],"CPWD");
    if(pointer!=NULL)
    { 
     if(cmp_digit((pointer+4),6))
     {   
     for(i=0;i!=6;i++)
     if(PWDE[i]!=(*(pointer+4+i)))break;
     if(i==6)    
     {     
         for(i=0;i!=6;i++)
            if(PWDE[i]!='0') break;
         if(i==6)
         {
            pointer=strstrf(&mas[0],"NUM1");
            if(pointer!=NULL)
            {
                if(cmp_digit((pointer+4),10))
                {
                    for(i=0;i!=10;i++)
                        NUMAR[0][i]=NUMAE[0][i]=*(pointer+4+i);   
                }
            }               
         }
         pointer=strstrf(&mas[0],"NPWD");
         if(pointer!=NULL)
         {
            if(cmp_digit((pointer+4),6))
            {
                for(i=0;i!=6;i++)
                 PWDE[i]=*(pointer+4+i);   
            }
         }
         pointer=strstrf(&mas[0],"NUM2");
         if(pointer!=NULL)
         {
            if(cmp_digit((pointer+4),10))
            {
                for(i=0;i!=10;i++)
                 NUMAR[1][i]=NUMAE[1][i]=*(pointer+4+i);   
            }
         }        
         #asm("wdr")         
         pointer=strstrf(&mas[0],"NUM3");
         if(pointer!=NULL)
         {
            if(cmp_digit((pointer+4),10))
            {
                for(i=0;i!=10;i++)
                 NUMAR[2][i]=NUMAE[2][i]=*(pointer+4+i);   
            }
         }
         pointer=strstrf(&mas[0],"NUM4");
         if(pointer!=NULL)
         {
            if(cmp_digit((pointer+4),10))
            {
                for(i=0;i!=10;i++)
                 NUMAR[3][i]=NUMAE[3][i]=*(pointer+4+i);   
            }
         }
         pointer=strstrf(&mas[0],"NUM5");
         if(pointer!=NULL)
         {
            if(cmp_digit((pointer+4),10))
            {
                for(i=0;i!=10;i++)
                 NUMAR[4][i]=NUMAE[4][i]=*(pointer+4+i);   
            }
         }
         pointer=strstrf(&mas[0],"ACTO"); // ?????
         if(pointer!=NULL)
         {
            SETSE=(unsigned char)atoi(pointer+4);
         }
         pointer=strstrf(&mas[0],"DACT"); // ?????
         if(pointer!=NULL)
         {
            GETSE=(unsigned char)atoi(pointer+4);
         }
         pointer=strstrf(&mas[0],"OPTC");      
         if(pointer!=NULL)
         {
            if(cmp_digit((pointer+4),5))
            {
                CALE=0x0;
                for(i=0;i!=5;i++)
                {                           
                    if(*(pointer+4+i)=='1') CALE|=(1<<i);   
                }
            }
         }                      
         pointer=strstrf(&mas[0],"OPTS");
         if(pointer!=NULL)
         {
            if(cmp_digit((pointer+4),5))
            {
                SMSE=0x0;
                for(i=0;i!=5;i++)
                {                           
                    if(*(pointer+4+i)=='1') SMSE|=(1<<i);   
                }
            }
         }
         pointer=strstrf(&mas[0],"ACTZ");
         if(pointer!=NULL)
         {
            if(cmp_digit((pointer+4),8))
            {
                ZoneMask=0x0;
                for(i=0;i!=8;i++)
                {                           
                    if(*(pointer+4+i)=='1') ZoneMask|=(1<<i);
                }
                ZoneMaskE=ZoneMask;
                sign=0;
                sign_E=0;
            }
         }
         mas[0]=mas[1]=0;
         strcatf(&mas[0]," NUM2");
         strcat(&mas[0],&NUMAR[1][0]);
         strcatf(&mas[0]," NUM3");
         strcat(&mas[0],&NUMAR[2][0]);
         strcatf(&mas[0]," NUM4");
         strcat(&mas[0],&NUMAR[3][0]);
         strcatf(&mas[0]," NUM5");
         strcat(&mas[0],&NUMAR[4][0]);
         strcatf(&mas[0]," OPTC");
         for(i=0;i!=5;i++)
         {
            if(CALE&(1<<i))  strcatf(&mas[0],"1");
            else   strcatf(&mas[0],"0");
         }
         strcatf(&mas[0]," OPTS");
         for(i=0;i!=5;i++)
         {
            if(SMSE&(1<<i))  strcatf(&mas[0],"1");
            else   strcatf(&mas[0],"0");
         }
         strcatf(&mas[0]," ACTZ");
         for(i=0;i!=8;i++)
         {
            if(ZoneMask&(1<<i))  strcatf(&mas[0],"1");
            else   strcatf(&mas[0],"0");
         }
         strcatf(&mas[0]," ACTO");
         i=SETSE;
         itoa(i,&mas[150]);
         strcat(&mas[0],&mas[150]);
         strcatf(&mas[0]," DACT");
         i=GETSE;
         itoa(i,&mas[150]);
         strcat(&mas[0],&mas[150]);
         strcatf(&mas[0]," CPWD");
         for(i=0;i!=6;i++)
            mas[150+i]=PWDE[i];
         mas[156]=0;
         strcat(&mas[0],&mas[150]);  
         SendSMS(&mas[0],&NUMAR[0][0]);
         delay_ms(2000);   
     }
     }
   }       
    C_CMGDA();
    return 1;                              
}
unsigned char cmp_digit(unsigned char *mas,unsigned char number)
{
    unsigned char i;         
    for(i=0;i!=number;i++)
        if(!(isdigit(mas[i]))) return 0;
    return 1;    
}                     