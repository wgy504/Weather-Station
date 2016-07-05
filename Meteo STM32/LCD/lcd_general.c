
#include "lcd_general.h"
#include "includes.h"

void vLcdTask (void *pvParameters)
{
  char strMsgLcd[16*2];
  char strMsgTemp[16];

  THumidity_Data stHumidityData;
  TPressure_Data stPressureData;

  
  LCD_Init();
  LCD_Send_CMD(DISP_ON);
  LCD_Send_CMD(CLR_DISP);
  _delay_ms(250);
  LCD_Send_STR("-=RUN APPLICAT=-");
  _delay_ms(1000);
  
  while(1)
  {    

    memset(strMsgLcd, 0x20, sizeof(strMsgLcd));
    //Проверяем создана ли очередь и читаем из нее.
    if(xQueueHumidityForLcd != 0) 
    { 
       if(xQueueReceive(xQueueHumidityForLcd,  &stHumidityData, (portTickType) 10)) {
       }
       else {
         //В очереде пока ничего нет.
       }   

      if(stHumidityData.bDataValid){
        if(stHumidityData.sRealTemperatur < 0) {
          sprintf(strMsgTemp, "Ext %.01fC, %i%H", ((double)stHumidityData.sRealTemperatur / 10), (stHumidityData.usRealHumidity / 10) );          
        }
        else {
          sprintf(strMsgTemp, "Ext %.01fC,  %i%H", ((double)stHumidityData.sRealTemperatur / 10), (stHumidityData.usRealHumidity / 10) );     
        }
      }
      else{
        sprintf(strMsgTemp, "Ext  ERROR  DATA");
      }
    }
    memcpy(&strMsgLcd[0], strMsgTemp, strlen(strMsgTemp));
    
    //Проверяем создана ли очередь и читаем из нее.
    if(xQueuePressureForLcd != 0) 
    { 
       if(xQueueReceive(xQueuePressureForLcd,  &stPressureData, (portTickType) 10)) {
       }
       else {
         //В очереде пока ничего нет.
       }   
      
      if(stPressureData.bDataValid){
        sprintf(strMsgTemp, "Int %iC, %immHg", (stPressureData.iRealTemperatur / 10), stPressureData.iRealPressureHg);          
      }
      else{
        sprintf(strMsgTemp, "Int  ERROR  DATA");
      }
    }
    memcpy(&strMsgLcd[sizeof(strMsgLcd)/2], strMsgTemp, strlen(strMsgTemp));
    
    //LCD_Send_CMD(CLR_DISP);
    LCD_Send_CMD(DD_RAM_ADDR1);
    //LCD_Send_STR(strMsgLcd);
    for(uint8_t i=0; i<sizeof(strMsgLcd); i++) {
       _delay_ms(1);
       if(i == sizeof(strMsgLcd)/2) {
          LCD_Send_CMD(DD_RAM_ADDR2);
       }
       if(strMsgLcd[i] == 0) {
          LCD_Send_CHAR(0x20);
       }
       else {
          LCD_Send_CHAR(strMsgLcd[i]);
       }
    }
   
    _delay_ms(1000);
  }
  
  vTaskDelete(NULL);
}

