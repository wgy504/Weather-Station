
#include "debug.h"
#include "includes.h"

int HandlerHumidity(char *pOut);


void vDebugTask (void *pvParameters)
{
  int Res;
  char strMsgDebug[64];
  TServer_Data stServerData;
  THumidity_Data stHumidityData;
  TPressure_Data stPressureData;
  GPS_INFO stGpsData;          //Структура GPS.
  memset(&stGpsData, 0, sizeof(stGpsData));
  
#ifdef  DEBUG_OUTPUT_USART
  InitUSART(UART_DBG, DBG_BAUDRATE);
  InitDMA(UART_DBG);
#endif
  

  while(1)
  {
    memset(strMsgDebug, 0, sizeof(strMsgDebug));
    
    //Проверяем создана ли очередь и читаем из нее.
    if(xQueueHumidityForDebug != 0) 
    { 
       if(xQueueReceive(xQueueHumidityForDebug, &stHumidityData, (portTickType) 10)) {
       }
       else {
         //В очереде пока ничего нет.
         stHumidityData.bDataValid = 0;
       }   
    }
    else {
       stHumidityData.bDataValid = 0;
    }

    //Проверяем создана ли очередь и читаем из нее.
    if(xQueuePressureForDebug != 0) 
    { 
       if(xQueueReceive(xQueuePressureForDebug, &stPressureData, (portTickType) 10)) {
       }
       else {
         //В очереде пока ничего нет.
         stPressureData.bDataValid = 0;
       }   
    }
    else {
       stPressureData.bDataValid = 0;
    }
    
    //Проверяем создана ли очередь и читаем из нее.
    if(xQueueGpsForDebug != 0) 
    { 
       if(xQueueReceive(xQueueGpsForDebug,  &stGpsData, (portTickType) 10)) {
       }
       else {
         //В очереде пока ничего нет.
       }   
    }
    else {
       stGpsData.status = 0;
    }
    
    if(stGpsData.status) {    
      LED_ON;
    }
    else {
      LED_TOGGLE;
    }
    
    if(osMutexWait(mSEND_DATA_SERVER, SLEEP_MS_1000) == osOK) {
       if( (stHumidityData.bDataValid) && (stPressureData.bDataValid) ) {
          stServerData.fIntTemperatur = (float)stPressureData.iRealTemperatur / 10;
          stServerData.iPressureHg = stPressureData.iRealPressureHg;
          stServerData.fExtTemperatur = (float)stHumidityData.sRealTemperatur / 10;
          stServerData.usExtHumidity = stHumidityData.usRealHumidity / 10;
          if(stGpsData.status) {
            stServerData.bGpsDataValid = TRUE;
            stServerData.fLatitude = stGpsData.latitude;
            stServerData.fLongitude = stGpsData.longitude;
          }
          else {
            stServerData.bGpsDataValid = FALSE;
          }
          xSemaphoreTake(mSEND_DATA_SERVER, 100);
          if(xQueueServerData != 0) {
             xQueueSendToFront(xQueueServerData, &stServerData, (portTickType) 1000);
          }
       }
       else {
          xSemaphoreGive(mSEND_DATA_SERVER);
          _delay_ms(1000);
       }
    }
    
    
    if(!(Res)) {
#ifdef  DEBUG_OUTPUT_USART     
      USART_Write(UART_DBG, strMsgDebug, strlen(strMsgDebug));
#endif
      
#ifdef DEBUG_OUTPUT_USB
      if(bDeviceState == CONFIGURED) {
          CDC_Send_DATA ((unsigned char *)strMsgDebug, strlen(strMsgDebug));
          NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
      }
#endif
    }
  }
}

