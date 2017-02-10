#define __ALLOCATE_NOW__

// User functions //
#include "includes.h"


// ID Task
xTaskHandle xHandleDebugTask;


int main()
{
  InitGPIO();
  //InitTIM3();
  //InitTIM4();

  //InitIWDG();    // Init Watch Dog 
  InitBKP();
  InitI2C();
  
  rtc_init();

  // Start Task //
  xTaskCreate(vDebugTask, "vDebugTask", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, &xHandleDebugTask);
  
  // Start scheduler //
  osKernelStart(NULL, NULL);
}


void vApplicationMallocFailedHook( void ) { for( ;; ); }
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ) { for( ;; ); }

void vApplicationIdleHook( void ) 
{
    unsigned long  ulFreeSizeStackTask; //��������� ����� � ��������� RTOS.
    unsigned long ulFreeHeapSize;       //��������� ����� � "����" RTOS
    uint8_t ucCPU_load;                 //�������� RTOS
    portTickType WakeTick = 0;
    uint32_t count = 0;                  
    uint32_t max_count = 0;             //������������ �������� ��������, ����������� ��� ���������� � ������������� 100% CPU idle
    
    WakeTick = xTaskGetTickCount() + configTICK_RATE_HZ;
    while(1) 
    {  
      if (xTaskGetTickCount() >= WakeTick)    
      {

#if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )
        ulFreeSizeStackTask = uxTaskGetStackHighWaterMark(xHandleDebugTask) << 2;
        ulFreeHeapSize = (unsigned long) xPortGetFreeHeapSize(); //in Byte
#endif

        /* ���������� ������������� OS */
        WakeTick += configTICK_RATE_HZ;
        if(count > max_count) {
           max_count = count;          //����������
        }
        ucCPU_load = (uint8_t)(100.0 - 100.0 * (float)count / (float)max_count);       //��������� ������� ��������
        count = 0;                                        //�������� �������
      }    
      if(WakeTick > xTaskGetTickCount() + configTICK_RATE_HZ<<1) {
          WakeTick = configTICK_RATE_HZ + xTaskGetTickCount();
      }
      count++;    //���������� ��������
      IWDG_ReloadCounter();     // Reload IWDG counter
    }
}