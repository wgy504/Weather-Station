
#define __ALLOCATE_NOW__

// User functions //
#include "includes.h"

// ID Task
xTaskHandle xHandleDebugTask;
xTaskHandle xHandleLcdTask;
xTaskHandle xHandleGSM;
xTaskHandle xHandleWifiTask;

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
  xTaskCreate(vLcdTask, "vLcdTask", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, &xHandleLcdTask);
  //xTaskCreate(vGsmTask, "vGsmTask", configMINIMAL_STACK_SIZE * 8, NULL, tskIDLE_PRIORITY + 1, &xHandleGSM);
  xTaskCreate(vWifiEspTask, "vWifiEspTask", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1, &xHandleWifiTask);
  
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