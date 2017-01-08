

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

#define BUZ                      GPIO_Pin_9
#define PORT_BUZ                 GPIOB
#define PORT_BUZ_CLK             RCC_APB2Periph_GPIOB


#define PORT_ESP_PWR_CLK        RCC_APB2Periph_GPIOB
#define ESP_PWR_PIN             GPIO_Pin_3
#define ESP_PWR_PORT            GPIOB

#define ESP_PWR_ON              GPIO_HIGH(ESP_PWR_PORT, ESP_PWR_PIN)
#define ESP_PWR_OFF             GPIO_LOW(ESP_PWR_PORT, ESP_PWR_PIN)

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

#define ESP_CP_PD_ON              GPIO_HIGH(ESP_CP_PD_PORT, ESP_CP_PD_PIN)
#define ESP_CP_PD_OFF             GPIO_LOW(ESP_CP_PD_PORT, ESP_CP_PD_PIN)


#define DRF_EN_PIN                GPIO_Pin_11
#define DRF_EN_PORT               GPIOA

#define DRF_SET_PIN               GPIO_Pin_12
#define DRF_SET_PORT              GPIOA

#define DRF_ENABLE              GPIO_SetBits(DRF_EN_PORT, DRF_EN_PIN);          
#define DRF_DISABLE             GPIO_ResetBits(DRF_EN_PORT, DRF_EN_PIN);       

#define DRF_SET_ENABLE          GPIO_ResetBits(DRF_SET_PORT, DRF_SET_PIN);     
#define DRF_SET_DISABLE         GPIO_SetBits(DRF_SET_PORT, DRF_SET_PIN);        


#define LED_ON            GPIO_LOW(PORT_LED, LED);
#define LED_OFF           GPIO_HIGH(PORT_LED, LED);
#define LED_TOGGLE        GPIO_TOGGLE(PORT_LED, LED);

#define BUZ_ON               GPIO_HIGH(PORT_BUZ, BUZ);
#define BUZ_OFF              GPIO_LOW(PORT_BUZ, BUZ);

#define GEIGER_COUNTER_SBM20
//#define GEIGER_COUNTER_J305

#ifdef  GEIGER_COUNTER_SBM20
  #define CONVERSION_FACTOR     (double) 0.0057 * 98
#endif
#ifdef GEIGER_COUNTER_J305
 #define CONVERSION_FACTOR      (double) 0.00812 * 98
#endif


#define ESP_BAUDRATE    115200
#define DBG_BAUDRATE    9600
//**********************//

/// ----- NUM UART ---- ///   
#define UART_ESP             2
#define UART_DBG             1  

#define TIM3_PERIOD     1000
GLOBAL uint16_t usTIMET1_CCR1_Val _EQU(500);

void InitGPIO(void);
void InitBKP(void);
void InitTIM3(void);
void InitIWDG(void);
void EXTI0_Config(void);

#endif