#define __ALLOCATE_NOW__

// User functions //
#include "includes.h"


// ID Task
xTaskHandle xHandleLcdTask;
xTaskHandle xHandlevPressureTask;
xTaskHandle xHandleHumidityTask;
xTaskHandle xHandleWifiTask;
xTaskHandle xHandleDebugTask;
xTaskHandle xHandleWifiEspTask;
xTaskHandle xHandleGpsTask;

int main()
{
  InitGPIO();
  InitTIM3();

  InitIWDG();    // Init Watch Dog 
  InitBKP();
  rtc_init();
  
#ifdef DEBUG_OUTPUT_USB
  Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
#endif
  
  mINIT_WIFI_ESP  = osMutexCreate(NULL);
  mSEND_DATA_SERVER = osMutexCreate(NULL);
  mINIT_GPS_MODULE      = osMutexCreate(NULL);
  mGPS_DATA_ARRIVAL     = osMutexCreate(NULL);
  xSemaphoreTake(mINIT_WIFI_ESP, SLEEP_MS_100);
  xSemaphoreTake(mSEND_DATA_SERVER, SLEEP_MS_100);
  xSemaphoreTake(mINIT_GPS_MODULE, SLEEP_MS_100);
  xSemaphoreTake(mGPS_DATA_ARRIVAL, SLEEP_MS_100);
  
  // Start Task //
  xTaskCreate(vLcdTask, "vLcdTask", configMINIMAL_STACK_SIZE * 3, NULL, tskIDLE_PRIORITY + 1, &xHandleLcdTask);
  xTaskCreate(vPressureTask, "vPressureTask", configMINIMAL_STACK_SIZE * 1, NULL, tskIDLE_PRIORITY + 1, &xHandlevPressureTask);
  xTaskCreate(vHumidityTask, "vHumidityTask", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, &xHandleHumidityTask);
  xTaskCreate(vWifiEspTask, "vWifiEspTask", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1, &xHandleWifiTask);
  xTaskCreate(vDebugTask, "vDebugTask", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, &xHandleDebugTask);
  xTaskCreate(vGpsHandler, "vGpsHandler", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, &xHandleGpsTask);
  
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