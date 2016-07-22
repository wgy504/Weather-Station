
#ifndef HTTP_GENERAL_H_
#define HTTP_GENERAL_H_
#include "includes.h"

#define MAX_TIME_ESP_REBOOT     36000   //1���

#define TIME_OPEN_SOCKET        5000    //SEC

#define MAX_ERR_SEND_FIRST_SERVER       2
#define MAX_ERR_SEND_SECOND_SERVER      3

#define INT_TEMPERATUR_TRANSMIT_SERVER
#define PRESSURE_TRANSMIT_SERVER
#define HUMIDITY_TRANSMIT_SERVER
#define CHARGER_TRANSMIT_SERVER
#define EXT_TEMPERATUR_TRANSMIT_SERVER


void vWifiEspTask (void *pvParameters);

#endif
