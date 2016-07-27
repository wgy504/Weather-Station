
#include "debug.h"
#include "includes.h"


#include <stdarg.h>

#include "includes.h"
#include "debug.h"

char d_buf[DBG_TX_BUFFER_SIZE];
const char * const gsm_rw[]={
    "OK",                       // 00
    "CONNECT",                  // 01
    "RING",                     // 02
    "NO CARRIER",               // 03
    "ERROR",                    // 04
    "NO DIALTONE",              // 05
    "BUSY",                     // 06
    "NO ANSWER",                // 07
    "PROCEEDING",               // 08
    "connect <text>",           // 09
    "TIMEOUT"                   // 0A
};

int32_t ConvertsTemperatur(uint8_t *pIn);
uint16_t GetCPM(void);
void ClearCPM(void);

#define TIME_SEND_DATA_SERVER   600
#define TIME_WTDG_REBOOT        599

uint16_t g_usCPM = 0;
uint16_t ga_usDoseValue[256];
uint16_t ga_usCpmValue[256];
uint16_t ga_usDoseValueDay[1440];

void vDebugTask (void *pvParameters)
{
  BUZ_ON;
  uint16_t usBackCMP = 0;
  uint32_t uiCurSec = 0;  
  uint32_t uiTimeMeas = 0;
  uint8_t ucMeanCounter = 0;
  uint16_t usMeanCounterDay = 0;
  double dCPM_Day = 0;
  double dDose_Day = 0;
  
  RTC_t eDate;
  memset(&eDate, 0, sizeof(&eDate));
  portTickType xLastWakeTimerDelay;
  uint32_t uiTimeSendDataServer = 0;
  TServer_Data stServerData;
  stServerData.fDose = 0;
  stServerData.fIntTemperatur = 0;
  stServerData.iCPM_Day = 0;
  stServerData.fDoseDay = 0;
  float fIndTempValue;
  int iCountMeasDebug = TIME_WTDG_REBOOT;
  
  osDelay(SLEEP_MS_300);
  BUZ_OFF;
  ClearCPM();
  
  DPS("D_WAITING FOR FIRST CHANGE\r\n");
  
  for(int i=59; i > 0; i--) {
    IWDG_ReloadCounter();
    xLastWakeTimerDelay = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTimerDelay, (SLEEP_MS_1000 / portTICK_RATE_MS));
    DP_GSM(" %i\r\n", i);
  }
  if(xQueueServDataOW != NULL) {
     xQueueReceive(xQueueServDataOW,  &fIndTempValue, (portTickType) 0);
     //stServerData.fIntTemperatur = fIndTempValue;
  }
  
  while(1)
  {
    if(xQueueServDataOW != NULL) {
        xQueueReceive(xQueueServDataOW,  &fIndTempValue, (portTickType) 0);
        stServerData.fIntTemperatur = fIndTempValue;
    }
    
    rtc_gettime(&eDate);
    uiCurSec = Date2Sec(&eDate);
    
    if(uiTimeMeas <= uiCurSec) {
      uiTimeMeas = uiCurSec + TIME_MEAS_RADIATION;
      uint16_t usCPM = GetCPM();
      stServerData.fDose = (float)usCPM;
      ClearCPM();
      stServerData.fDose *= CONVERSION_FACTOR;
      ga_usDoseValue[ucMeanCounter] = (uint16_t) stServerData.fDose;
      ga_usCpmValue[ucMeanCounter] = usCPM;
      
      ga_usDoseValueDay[usMeanCounterDay] = (uint16_t) stServerData.fDose;
      dCPM_Day += usCPM;
      ucMeanCounter++;
      usMeanCounterDay++;
      iCountMeasDebug = TIME_WTDG_REBOOT;
      if(usMeanCounterDay > sizeof(ga_usDoseValueDay)) {
         for(uint16_t i=0; i<usMeanCounterDay; i++) {
            dDose_Day += ga_usDoseValueDay[i];
         }
         dDose_Day /= (double)usMeanCounterDay;
         stServerData.fDoseDay = dDose_Day;
         stServerData.iCPM_Day = (int)dCPM_Day;
         dCPM_Day = 0;
         dDose_Day = 0;
         usMeanCounterDay = 0;
      }
      
      DP_GSM("\r\nD_CUR DATA:\r\nCurCPM %i\r\nCurDose %.00fmR\r\nCurTemperature %.01fC\r\n", usCPM, stServerData.fDose, stServerData.fIntTemperatur);      
      DP_GSM("D_MEAN COUNTER: %i\r\n\r\n", ucMeanCounter);
      DPS("D_WAITING FOR SECOND CHANGE\r\n");
      if(stServerData.fDose > 75) {
        BUZ_ON;
        osDelay(SLEEP_MS_10000);
      }
      else {
        BUZ_OFF;
      }
    }
 
    /* Send Data To Queue */
    if( (uiTimeSendDataServer <= uiCurSec) && (stServerData.fDose && stServerData.fIntTemperatur) ) {
        uiTimeSendDataServer = uiCurSec + TIME_SEND_DATA_SERVER;
        if(xQueueServerData != 0) {
          uint32_t uiMeanValueDose =  0;
          uint32_t uiMeanValueCPM =  0;
          for(uint8_t i=0; i<ucMeanCounter; i++) {
            uiMeanValueDose += (uint32_t)ga_usDoseValue[i];
            uiMeanValueCPM += (uint32_t)ga_usCpmValue[i];
          }
          uiMeanValueDose /= (uint32_t)ucMeanCounter;
          uiMeanValueCPM /= (uint32_t)ucMeanCounter;
            
          stServerData.fDose = (float)uiMeanValueDose;
          stServerData.iCPM = (int)uiMeanValueCPM;
          DP_GSM("\r\nD_MEAN DATA:\r\nMeanCPM %i\r\nMeanDose %.00fmR\r\nCurTemperature %.01fC\r\n\r\n", uiMeanValueCPM, stServerData.fDose, stServerData.fIntTemperatur);
          xQueueSendToFront(xQueueServerData, &stServerData, (portTickType) 1000);
          if((stServerData.iCPM_Day) && (stServerData.fDoseDay)) {
             DP_GSM("\r\nD_DAY DATA:\r\nDayCPM %i\r\nDayDose %.00fmR\r\n\r\n", stServerData.iCPM_Day, stServerData.fDoseDay);
             stServerData.iCPM_Day = 0;
             stServerData.fDoseDay = 0;
          }
          uiMeanValueDose = 0;
          memset(ga_usDoseValue, 0, sizeof(ga_usDoseValue));
          ucMeanCounter = 0;
        }
        else {
          uiTimeSendDataServer = TIME_SEND_DATA_SERVER;
        }
    }
    
    if(usBackCMP != GetCPM()) {
      usBackCMP = GetCPM();
      LED_ON;
    }
    xLastWakeTimerDelay = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTimerDelay, (SLEEP_MS_100 / portTICK_RATE_MS));
    LED_OFF;
    iCountMeasDebug--;
    //DP_GSM(" %i\r\n", iCountMeasDebug);
    if(iCountMeasDebug < -100) { //������� ��� ����� ������� ����������.
      DPS("\r\n-D_DEVICE REBOOT-\r\n");
      IWDG_ReloadCounter();
      vTaskSuspendAll();           //Stop All Task
      vTaskEndScheduler();         //Stop OS
      __disable_interrupt();
      while(1);
    }
    
    IWDG_ReloadCounter();          // Reload IWDG counter.
  }
  
}

void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    g_usCPM++;
    /* Clear the  EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

void ClearCPM(void)
{
  g_usCPM = 0;
}

uint16_t GetCPM(void)
{
  return g_usCPM;
}

void vOnewireTask (void *pvParameters)
{
  uint8_t ucTempBufTemper[2];
  TOnewire_Data stTemperaturData;
  float fDataToServ = 0;
  
  OW_Init();
  memset(&stTemperaturData, 0, sizeof(stTemperaturData));
  
   // ������ ������� struct ������ 
  xQueueServDataOW = xQueueCreate(sizeof(uint8_t), sizeof(float));
  vQueueAddToRegistry(xQueueServDataOW, "xQueueServDataOW");
  
  //xQueueSendToFront(xQueueServDataOW, &fDataToServ, SLEEP_MS_100);
  
  while(1)
  {
    // ���������� ������ ��������� �������� ��������� �����������    
    OW_Send(OW_SEND_RESET, "\xcc\x44", 2, NULL, NULL, OW_NO_READ);
    
    // ��������� ������� ������������ ������ - ������ "�������" �� ����
    OW_out_set_as_Power_pin();
    
    // ����������� ����� ��������� (�������� 750 �� ��� 12-������� ���������)
    osDelay(SLEEP_MS_750);

    // ��������������� ������� ����������� UART
    OW_out_set_as_TX_pin();

    // ������ ��������� ������� ����������      
    if(OW_Send(OW_SEND_RESET, "\xcc\xbe\xff\xff", 4, ucTempBufTemper, 2, 2) == OW_OK) {
      stTemperaturData.iRealTemperatur = ConvertsTemperatur(ucTempBufTemper);
      stTemperaturData.bDataValid = 1;
    }
    else  {
       stTemperaturData.iRealTemperatur = -850;
       stTemperaturData.bDataValid = 0;
    }
    
    fDataToServ = (float)stTemperaturData.iRealTemperatur/10;
    xQueueSendToFront(xQueueServDataOW, &fDataToServ, SLEEP_MS_100);

  }
}


int32_t ConvertsTemperatur(uint8_t *pIn)
{
    unsigned char Temp_H = 0;
    unsigned char Temp_L = 0;
    
    char strTemp[4];
    _Bool temp_flag;
    uint8_t tempint = 0,tempint1,tempint2,tempint3; // ���������� ��� ������ �������� �����������
    uint16_t temppoint = 0,temppoint1; // ���������� ��� �������� �������� �����������

    Temp_L = pIn[0]; // ������ ������ 2 ����� ��������
    Temp_H = pIn[1];

    temp_flag = 1;         // ���� ����� ����������� ����� 1(����)

    if(Temp_H &(1 << 3))   // ��������� ��� ����� ����������� �� ��������� ������� 
    {			
            signed int tmp;
            temp_flag = 0;      // ���� ����� ����� 0(�����)
            tmp = (Temp_H << 8) | Temp_L;
            tmp = -tmp;
            Temp_L = tmp;
            Temp_H = tmp >> 8; 
    }		

    tempint = ((Temp_H << 4) & 0x70)|(Temp_L >> 4); // ��������� ����� �������� �����������
    tempint1 = tempint % 1000 / 100;  
    tempint2 = (tempint % 100 / 10) + 0x30;  
    tempint3 = (tempint % 10) + 0x30; 
                   
    temppoint = Temp_L & 0x0F; // ��������� ������� �������� �����������
    temppoint = temppoint * 625;       // �������� ����������� 
    temppoint1 = (temppoint / 1000) + 0x30;        
            
            
    if(temp_flag==0) { // ���� ���� ����� ����������� ����� 0, � ������ ������� ������ �����
            tempint1 = '-';
    }
    else{
            tempint1 = '+';
    }


    strTemp[0]=tempint1;
    strTemp[1]=tempint2;
    strTemp[2]=tempint3;
    strTemp[3]=temppoint1;
    
    return atoi(strTemp);
}


void GSM_DPD(char *pData_Usart, uint16_t Len)
{
  if(Len > DBG_TX_BUFFER_SIZE) return;
  
  DPD((char *)pData_Usart, Len);
}


void DS_GSM(const char *msg, const char *pData_Usart)
{  
   uint16_t Len = 0;

   d_buf[0] = 0;
   strcpy(d_buf, msg);
   strcat(d_buf, pData_Usart);
   
   while(d_buf[Len] != 0)
   {
     if((d_buf[Len] == '\r') || (d_buf[Len] == '\n'))
      d_buf[Len] = 0x20;
         
      Len++;
   }
   
   strcat(d_buf, "\r\n");
   
   DPS(d_buf);
}



void DPD(const char *pData_Usart, uint16_t Len)
{
   if(Len > DBG_TX_BUFFER_SIZE) return;
   USART_Write(UART_DBG, pData_Usart, Len);  
  
}

void DPC(char Data_Usart)
{
   DPD(&Data_Usart, 1);
}

void DPS(const char *pData_Usart)
{
   uint16_t Len;
 
   Len = strlen(pData_Usart);
   if(Len > DBG_TX_BUFFER_SIZE) { return; }
   
   USART_Write(UART_DBG, pData_Usart, Len);      
}


void debug_printf(const char *fmt_ptr, ...)
{    
    va_list ap;
   
    va_start(ap, fmt_ptr);
    vsprintf(d_buf, fmt_ptr, ap);
    va_end(ap);
    
    DPS(d_buf);
}


void GSM_DC(const char *pCmd, char res)
{ 
  GSM_WP(pCmd, 0, res);
}


void GSM_WP(const char *pCmd, const char *pPmd, char res)
{
    char n;
    char ch=0;

    n = ((res<'0' || res>'9') ? 10 : res-'0');
    
    switch(n) {
      case  0: ch = '+'; break;
      case  4: ch = '-'; break;
      case 10: ch = 't'; break;
    }
    
    if(ch != 0) {
      debug_printf("[%c] GSM: %s", ch, pCmd);
      if(pPmd != 0) DPS(pPmd);
    } else {
      DPS("GSM: ");
      DPS(pCmd);
      if(pPmd != 0) {
          DPS(pPmd);
      }
      DPC(' ');
      DPS(gsm_rw[n]);
    }
    
    DPC('\r');
}