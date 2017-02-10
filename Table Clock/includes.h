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

#include "I2C.h"

#include "LCD.h"

// buffer
GLOBAL uint8_t g_aucRxBufferUSART1[RX_BUFFER_SIZE1]; 
GLOBAL uint8_t g_aucRxBufferUSART2[RX_BUFFER_SIZE2]; 
GLOBAL uint8_t g_aucRxBufferUSART3[RX_BUFFER_SIZE3]; 
GLOBAL uint8_t g_aucTxBufferUSART1[TX_BUFFER_SIZE1]; 
GLOBAL uint8_t g_aucTxBufferUSART2[TX_BUFFER_SIZE2]; 
GLOBAL uint8_t g_aucTxBufferUSART3[TX_BUFFER_SIZE3]; 

//**********************//

// SIZE BUFFER I2C //
#define I2C_TX_BUFFER_SIZE1    64      
#define I2C_RX_BUFFER_SIZE1    64

/* Buffer of data to be received by I2C1 */
GLOBAL uint8_t BufferI2C_Rx1[I2C_RX_BUFFER_SIZE1];
/* Buffer of data to be transmitted by I2C1 */
GLOBAL uint8_t BufferI2C_Tx1[I2C_TX_BUFFER_SIZE1];


GLOBAL  xTaskHandle CurrentTaskHandle;  //ID �������� ��������(Debug)
GLOBAL  char *pNameCurrentTask;          //��� �������� ��������(Debug)

#endif