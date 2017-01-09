#ifndef DHT11_H_
#define DHT11_H_

/* Includes ------------------------------------------------------------------*/
#include "includes.h"

#define  MAX_TICS       200     //��������� ������ � ����������� �� ������� ��.
#define  MAX_LOG_VAL    50      //��������� ���� �������� �� ������������� ��(����� ������ �����, ���������� �������).

/* Exported constants --------------------------------------------------------*/
#define DHT11_PORT_CLOCK        RCC_APB2Periph_GPIOA
#define DHT11_PORT              GPIOB
#define DHT11_PIN               GPIO_Pin_0

#define DHT11_OK 0
#define DHT11_NO_CONN 1
#define DHT11_CS_ERROR 2



/* Exported functions ------------------------------------------------------- */
int Read_DHT11(uint8_t* pBuf);

#endif
