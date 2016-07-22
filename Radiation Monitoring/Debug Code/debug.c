
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



void vDebugTask (void *pvParameters)
{

  while(1)
  {
    osDelay(SLEEP_MS_1000);
  }
  
}

