#ifndef _INCLUDES_H_
#define _INCLUDES_H_

// �������� ������� ��� ��������� ��������� � ����
#define PUBLIC  __IO
#define PRIVATE static
#define FORWARD static
#define _EXTERN extern

// ������� ������ �����������
#ifdef __ALLOCATE_NOW__
  #define GLOBAL    PUBLIC
  #define _EQU(x)	=(x)
#else
  #define GLOBAL    _EXTERN
  #define _EQU(x)
#endif

// � �������� ������� ����� ���� ���������� �������������� ���� ������ (#define)
#define OK              0
#define FAIL            -1

// ����������� �������:
#define BIT(bit)          (1UL << (bit))

#define SETBIT(Val,bit)   ((Val) |= BIT(bit))
#define CLRBIT(Val,bit)   ((Val) &= ~BIT(bit))
#define XORBIT(Val,bit)   ((Val) ^= BIT(bit))
#define TSTBIT(Val,bit)   ((Val) & BIT(bit))

#define _delay_ms(delay)       osDelay(delay)

#define SLEEP_MS_60000  60000
#define SLEEP_MS_10000  10000
#define SLEEP_MS_5000   5000
#define SLEEP_MS_4000   4000
#define SLEEP_MS_3000   3000
#define SLEEP_MS_2500   2500
#define SLEEP_MS_2000   2000
#define SLEEP_MS_1000   1000
#define SLEEP_MS_750    750
#define SLEEP_MS_500    500
#define SLEEP_MS_250    250
#define SLEEP_MS_200    200
#define SLEEP_MS_150    150
#define SLEEP_MS_100    100
#define SLEEP_MS_50     50
#define SLEEP_MS_10     10
#define SLEEP_MS_5      5
#define SLEEP_MS_1      1

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <intrinsics.h>
#include <assert.h>

// Device functions //
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "arm_comm.h"
#include "stm32f10x_it.h"
//**************//

#include "platform_config.h"

#include "sram.h"

// FreeRTOS includes //
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os.h"
//**************//

// Drivers //
#include "USART.h"
//**************//

/* DEBUG */
#include "debug.h"
//**************//

/* RTC */
#include "rtc.h"
#include "calendar.h"
/**************/

/* LCD */
//#include "LCD.h"
//#include "lcd_general.h"
/***********/

#define RX_BUFFER_SIZE              600         //300
#define SIZE_IN_DATA_BUF            500         //200
#define SIZE_OUT_DATA_BUF           600         //300

GLOBAL char g_asRxBuf[RX_BUFFER_SIZE];                         //����� ��� ������ ������� GSM.
GLOBAL char g_asCmdBuf[SIZE_IN_DATA_BUF];                      //����� ��� ������ ������� GSM.      

#define MAX_GSM_CONNECT_FAIL 3

/* ��������� GSM & GPRS ���������� */
#define DEF_GPRS_RECEIVE_COUNT                  5       //ED
#define DEF_GSM_REG_COUNT                       30       //ED
#define DEF_GSM_TIMEOUT                         30      //SEC
#define DEF_WAIT_SEND_OK                        60      //ED                    //�������� �������� ������
#define DEF_GPRS_TIMEOUT                        30      //SEC
#define DEF_MAX_TIME_FIND_STA                   5       //SEC
#define DEF_MIN_COUNT_STA                       3       //ED
#define TIMEOUT_FTP_CONNECT                     30      //���� ����������� � FTP


/* GSM */
#include "gsm_hw.h"
#include "gsm_parser.h"
#include "gsm_mc52iT.h"
#include "gsm_check.h"
#include "gsm_info.h"
//#include "gsm_gprs.h"
//#include "sms_cmd.h"
//#include "gsm_sms.h"

/**************/

#define DBG_RX_BUFFER_SIZE 1
#define DBG_TX_BUFFER_SIZE 128
#define GSM_RX_BUFFER_SIZE 1800  //675
#define GSM_TX_BUFFER_SIZE 350

#define GPS_RX_BUFFER_SIZE 1  //150
#define GPS_TX_BUFFER_SIZE 1   //64

// buffer
GLOBAL uint8_t g_aucRxBufferUSART1[RX_BUFFER_SIZE1]; 
GLOBAL uint8_t g_aucRxBufferUSART2[RX_BUFFER_SIZE2]; 
GLOBAL uint8_t g_aucRxBufferUSART3[RX_BUFFER_SIZE3]; 
GLOBAL uint8_t g_aucTxBufferUSART1[TX_BUFFER_SIZE1]; 
GLOBAL uint8_t g_aucTxBufferUSART2[TX_BUFFER_SIZE2]; 
GLOBAL uint8_t g_aucTxBufferUSART3[TX_BUFFER_SIZE3]; 

//GLOBAL xQueueHandle xQueueServerData                    _EQU(NULL);             // ������� ������ 
//**********************//

/* Mutex Initialization */
GLOBAL xSemaphoreHandle mGPS_DATA_ARRIVAL       _EQU(NULL);

GLOBAL  xTaskHandle CurrentTaskHandle;  //ID �������� ��������(Debug)
GLOBAL  char *pNameCurrentTask;          //��� �������� ��������(Debug)

typedef enum PROF_CONNECT_SERVER {
   PROF_FIRST_SERVER         = 0,
   PROF_SECOND_SERVER        = 1,
   PROF_FTP_SERVER           = 2,
   PROF_AGPS_SERVER          = 3,
   PROF_HTTP_GET_SERVER      = 4,
   PROF_HTTP_POST_SERVER     = 5,
} PROF_CONNECT_SERVER;


#endif