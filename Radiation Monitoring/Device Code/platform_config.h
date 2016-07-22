

#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

// User functions //
#include "includes.h"


#define GPIO_HIGH(a,b) 		a->BSRR = b
#define GPIO_LOW(a,b)		a->BRR = b
#define GPIO_TOGGLE(a,b) 	a->ODR ^= b 
#define ON              TRUE
#define OFF             FALSE

// LIGHT LED
#define LED               GPIO_Pin_13
#define PORT_LED          GPIOC
#define PORT_LED_CLK      RCC_APB2Periph_GPIOC

/*
// GSM REFERENCE
#define GSM_REF_PIN                     GPIO_Pin_0
#define GSM_REF_PORT                    GPIOA
#define GSM_STATUS_ON           1   // � ������� SIM800 �� �����������.

// START BGS
#define GSM_PWR_KEY_PIN               GPIO_Pin_1
#define GSM_PWR_KEY_PORT              GPIOA
    
#define PWR_KEY_PULL_DOWN               GPIO_HIGH(GSM_PWR_KEY_PORT, GSM_PWR_KEY_PIN)
#define PWR_KEY_PULL_UP                 GPIO_LOW(GSM_PWR_KEY_PORT, GSM_PWR_KEY_PIN)      

#define GSM_DTR_PIN                  GPIO_Pin_4 
#define GSM_DTR_PORT                 GPIOA 
*/

#define ESP_RST_PIN                  GPIO_Pin_1
#define ESP_RST_PORT                 GPIOA

#define ESP_CP_PD_PIN                GPIO_Pin_4
#define ESP_CP_PD_PORT               GPIOA

#define LED_ON            GPIO_LOW(PORT_LED, LED);
#define LED_OFF           GPIO_HIGH(PORT_LED, LED);
#define LED_TOGGLE        GPIO_TOGGLE(PORT_LED, LED);

/* ---- GSM ---- */
#define GSM_MODULE_SIM800

/* ---- GPS ---- */
//#define GPS_MODULE_IT520
#define GPS_MODULE_UBLOX

#ifdef GPS_MODULE_IT520
  #define GPS_PACKET_LEN  1
#endif

#ifdef GPS_MODULE_UBLOX
  #define GPS_PACKET_LEN  0
#endif

//// ----- GPS BAUDRATE ---- ////
#ifdef GPS_MODULE_IT520
  #define GPS_BAUDRATE    115200
#endif

#ifdef GPS_MODULE_UBLOX
  #define GPS_BAUDRATE    9600
#endif

//#define GSM_BAUDRATE    115200
#define ESP_BAUDRATE    115200
#define DBG_BAUDRATE    115200
//**********************//

/// ----- NUM UART ---- ///   
//#define UART_GSM           2 
#define UART_ESP           2
#define UART_DBG           1  
#define UART_GPS           3   
//**********************//

#define TIM3_PERIOD     1000
GLOBAL uint16_t usTIMET1_CCR1_Val _EQU(500);

void InitGPIO(void);
void InitBKP(void);
void InitTIM3(void);
void InitIWDG(void);

#endif