

#include "includes.h"
#include "humidity_general.h"


void vHumidityTask (void *pvParameters)
{
   THumidity_Data stHumidityData;
   uint8_t aucHumidityData[5];
   
   // ������ �������
   xQueueHumidityForDebug = xQueueCreate(sizeof(uint8_t), sizeof(THumidity_Data));
   vQueueAddToRegistry(xQueueHumidityForDebug, "xQueueHumidityForDebug");
   
   // ������ �������
   xQueueHumidityForLcd = xQueueCreate(sizeof(uint8_t), sizeof(THumidity_Data));
   vQueueAddToRegistry(xQueueHumidityForLcd, "xQueueHumidityForLcd" );
          
   while(1)
   {      
      if(Read_DHT11(aucHumidityData) == DHT11_OK) {

        if( (aucHumidityData[1] == 0) && (aucHumidityData[3] == 0) ) {          //dht11
          stHumidityData.usRealHumidity = aucHumidityData[1];
          stHumidityData.usRealHumidity = stHumidityData.usRealHumidity<<8;
          stHumidityData.usRealHumidity |= aucHumidityData[0];
          stHumidityData.usRealHumidity *= 10;
      
          stHumidityData.sRealTemperatur = aucHumidityData[3];
          stHumidityData.sRealTemperatur = stHumidityData.sRealTemperatur<<8;
          stHumidityData.sRealTemperatur |= aucHumidityData[2];
          stHumidityData.sRealTemperatur *= 10;
         }
         else {                                                                 //dht21-dht22                                       
            stHumidityData.usRealHumidity = aucHumidityData[0] * 256 + aucHumidityData[1];
            stHumidityData.sRealTemperatur = (aucHumidityData[2] & 0x7F)* 256 + aucHumidityData[3];
            if (aucHumidityData[2] & 0x80) {
               stHumidityData.sRealTemperatur *= -1;
            }
         }
        stHumidityData.bDataValid = 1;
      }
      else {
        stHumidityData.bDataValid = 0;
      }

      if(xQueueHumidityForDebug) {
        xQueueSendToFront(xQueueHumidityForDebug, &stHumidityData, (portTickType) 0);
      }
      
      if(xQueueHumidityForLcd) {
          xQueueSendToFront(xQueueHumidityForLcd, &stHumidityData, (portTickType) 0);
      }
      _delay_ms(500);
   }
   
}