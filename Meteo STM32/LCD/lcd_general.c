
#include "lcd_general.h"
#include "includes.h"

void vLcdTask (void *pvParameters)
{
  char strMsgLcd[16*2];
  char strMsgTemp[16];
  uint8_t eStepLcd = 0;

  THumidity_Data stHumidityData;
  TPressure_Data stPressureData;
  RTC_t stDateTime;
  
  LCD_Init();
  LCD_Send_CMD(DISP_ON);
  LCD_Send_CMD(CLR_DISP);
  _delay_ms(250);
  LCD_Send_STR("-=RUN APPLICAT=-");
  _delay_ms(1000);
  
  while(1)
  {    
    memset(strMsgLcd, 0x20, sizeof(strMsgLcd));
    
    while(xQueueReceive(xQueuePressureForLcd,  &stPressureData, (portTickType) 0) == pdFALSE);
    while(xQueueReceive(xQueueExtHumidityForLcd,  &stHumidityData, (portTickType) 0) == pdFALSE);
    if(!(stHumidityData.bDataValid)) {
      stHumidityData.sRealTemperatur = -850;
      stHumidityData.usRealHumidity = 1000;
    }
    
    rtc_gettime(&stDateTime);
    sprintf(strMsgTemp, "%02d:%02d:%02d %dmmHg", 
            stDateTime.hour, 
            stDateTime.min, 
            stDateTime.sec, 
            stPressureData.iRealPressureHg);
    
    memcpy(&strMsgLcd[0], strMsgTemp, strlen(strMsgTemp));
    
    if(eStepLcd < 4) {
        sprintf(strMsgTemp, "Ext %dC Int %dC", stHumidityData.sRealTemperatur / 10, stPressureData.iRealTemperatur / 10);
    }
    if(eStepLcd >= 4 && eStepLcd < 7 ) {   
      sprintf(strMsgTemp, "Ext %d%% ", stHumidityData.usRealHumidity / 10);
    }
    
    eStepLcd++;
    if(eStepLcd >= 7) {
      eStepLcd = 0;
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

