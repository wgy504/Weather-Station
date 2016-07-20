
#define __ALLOCATE_NOW__

// User functions //
#include "includes.h"

// ID Task
xTaskHandle xHandleDebugTask;

int main()
{
  InitGPIO();
  InitTIM3();

  InitIWDG();    // Init Watch Dog 
  InitBKP();
  rtc_init();
  
  // Start Task //
  xTaskCreate(vDebugTask, "vDebugTask", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, &xHandleDebugTask);
  
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