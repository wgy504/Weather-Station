
#define __ALLOCATE_NOW__

// User functions //
#include "includes.h"

// ID Task
xTaskHandle xHandleDebugTask;
xTaskHandle xHandleGSM;


int main()
{
  InitGPIO();
  InitTIM3();

  //InitIWDG();    // Init Watch Dog 
  InitBKP();
  rtc_init();
  
  InitUSART(UART_DBG, DBG_BAUDRATE);
  InitDMA(UART_DBG);
  
  DPS("\r\n-=D_RUN APPL=-\r\n"); 
  
  // Start Task //
  xTaskCreate(vDebugTask, "vDebugTask", configMINIMAL_STACK_SIZE * 1, NULL, tskIDLE_PRIORITY + 1, &xHandleDebugTask);
  xTaskCreate(vGsmTask, "vGsmTask", configMINIMAL_STACK_SIZE * 8, NULL, tskIDLE_PRIORITY + 1, &xHandleGSM);
  
  // Start scheduler //
  osKernelStart(NULL, NULL);
}

void vApplicationMallocFailedHook( void ) { for( ;; ); }
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) { for( ;; ); }

void vApplicationIdleHook( void ) {             //это и есть поток Idle с минимальным приоритетом.
  
  while(1){
     IWDG_ReloadCounter();          // Reload IWDG counter.
  }
}