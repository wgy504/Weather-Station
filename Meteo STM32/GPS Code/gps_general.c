

#include "includes.h"
#include "gps_general.h"
#include "USART.h"

//$GNRMC,165835.000,A,6001.8859,N,03024.1946,E,0.02,26.13,151013,,,D*4E\r\n
//char msg_rmc[] = "$GNRMC,165835.000,A,6001.8859,N,03024.1946,E,0.02,26.13,151013,,,D*4E\r\n";
//char msg_gga[] = "$GPGGA,111812.00,6001.89706,N,03024.19850,E,1,05,5.67,70.1,M,15.8,M,,*6A\r\n";


const char ublox_msg_led_gps_settings[] = {0xB5, 0x62, 0x06, 0x31, 0x20, 0x00, 0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00,
    0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x57, 0x00, 0x00, 0x00, 0x94, 0x56, 0xB5, 0x62, 0x06, 0x31, 0x01, 0x00, 0x00, 0x38, 0xE5};     //led settings

const char ublox_msg_navi_auto_2d[] = {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x10,
                      0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x3C,
                      0x00, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xA2};      //navi-auto+2d

const char ublox_msg_gll_disabled[] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x01, 0x00, 0xFB, 0x11};      //gll-disabled
const char ublox_msg_vtg_disabled[] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x05, 0x00, 0xFF, 0x19};      //vtg-disabled
const char ublox_msg_gsv_disabled[] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x03, 0x00, 0xFD, 0x15};      //gsv-disalbed
const char ublox_msg_gsa_disabled[] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x02, 0x00, 0xFC, 0x13};      //gsa-disalbed

const char ublox_msg_config_filt_stop[] = {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x00, 0x03, 0x00, 0x00, 0x00,
                                    0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00,  0xFA, 0x00, 0x64, 0x00, 
                                    0x2C, 0x01, 0x68, 0x3C, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x7C, 0x0C};

const char sim_msg_disable[] = "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0*34\r\n";


uint32_t GpsTimeBack = 0;
uint32_t GpsRealTimeBack = 0;



#if UART_GPS == 1
  char *pRxBufferGPS = (char*)(g_aucRxBufferUSART1);
#endif

#if UART_GPS == 2
  char *pRxBufferGPS = (char*)(g_aucRxBufferUSART2);
#endif
  
#if UART_GPS == 3
  char *pRxBufferGPS = (char*)(g_aucRxBufferUSART3);
#endif
  
  
void InitGPS(void)
{
   // Init UART 3
   InitUSART(UART_GPS, GPS_BAUDRATE);
   InitDMA(UART_GPS);

   osDelay(1000);
  
#ifdef GPS_MODULE_SIMCOM    
     USART_Write(UART_GPS, sim_msg_disable, (sizeof(sim_msg_disable)-1));  
#endif
     
     
#ifdef GPS_MODULE_UBLOX
       //gll-disabled
  #ifdef GPS_GLL_DISABLE   
       osDelay(100);
       USART_Write(UART_GPS, ublox_msg_gll_disabled, sizeof(ublox_msg_gll_disabled));  
  #endif
       //vtg-disabled
  #ifdef GPS_VTG_DISABLE
       osDelay(100);  
       USART_Write(UART_GPS, ublox_msg_vtg_disabled, sizeof(ublox_msg_vtg_disabled));  
  #endif 
       //gsv-disalbed
  #ifdef GPS_GSV_DISABLE
       osDelay(100);     
       USART_Write(UART_GPS, ublox_msg_gsv_disabled, sizeof(ublox_msg_gsv_disabled));
  #endif    
       //gsa-disalbed
  #ifdef GPS_GSA_DISABLE
       osDelay(100);
       USART_Write(UART_GPS, ublox_msg_gsa_disabled, sizeof(ublox_msg_gsa_disabled));
  #endif
       //led config
  #ifdef LED_CONFIG_ENABLE
       osDelay(100);
       USART_Write(UART_GPS, ublox_msg_led_gps_settings, sizeof(ublox_msg_led_gps_settings));
  #endif
       //gps mode config
  #ifdef NAVI_AVTO_2D_ENABLE
      osDelay(100);
       USART_Write(UART_GPS, ublox_msg_navi_auto_2d, sizeof(ublox_msg_navi_auto_2d));
  #endif
       // config filter enable
  #ifdef GPS_FILTER_STOP_ENABLE     
       osDelay(100);
       USART_Write(UART_GPS, ublox_msg_config_filt_stop, sizeof(ublox_msg_config_filt_stop));
  #endif 
#endif//GPS_MODULE_UBLOX 
}
  

void vGpsHandler (void *pvParameters)
{
  GPS_INFO stGpsData;          //Структура GPS.
  memset(&stGpsData, 0, sizeof(stGpsData));
  uint32_t uiMaxTimeGpsReboot;
  uint8_t ucTimeoutMutexUartGps = 0;
  
  // Создаём очередь
  xQueueGpsForDebug = xQueueCreate(sizeof(uint8_t), sizeof(stGpsData));
  vQueueAddToRegistry(xQueueGpsForDebug, "xQueueGpsForDebug");
  /*
  xQueueGpsDateToRtc = xQueueCreate(sizeof(uiTime), sizeof(uiTime));
  vQueueAddToRegistry(xQueueGpsDateToRtc, "xQueueGpsDateToRtc");
  */
  xSemaphoreGive(mINIT_GPS_MODULE);
  
  while(1)
  {  
   if(osMutexWait(mINIT_GPS_MODULE, 0) == osOK) {
       uiMaxTimeGpsReboot = 0;
       InitGPS();
       xSemaphoreTake(mINIT_GPS_MODULE, 0);
   }
   
   if(osMutexWait(mGPS_DATA_ARRIVAL, SLEEP_MS_1000) == osOK)
   {
      if(!(GpsBufNumber())) {
        gps_parser_sting(&stGpsData, pRxBufferGPS, GPS_RX_BUFFER_SIZE/2);
      }
      else {
        gps_parser_sting(&stGpsData, &pRxBufferGPS[GPS_RX_BUFFER_SIZE/2], GPS_RX_BUFFER_SIZE/2);
      }

      if((stGpsData.time) && (stGpsData.status == (MASK_RMC)) )
      {
        /*
        uint32_t uiTime = stGpsData.time;
        if(xQueueGpsDateToRtc != 0) {
           xQueueSendToFront(xQueueGpsDateToRtc, &uiTime, (portTickType) 0);
        }
        */
        uiMaxTimeGpsReboot = 0;      
      }
      else
      {
        uiMaxTimeGpsReboot++;
        memset(&stGpsData, 0, sizeof(stGpsData));
      }  
      
      if(xQueueGpsForDebug != 0) {
           xQueueSendToFront(xQueueGpsForDebug, &stGpsData, (portTickType) 0);
      }
      memset(&stGpsData, 0, sizeof(stGpsData));
      
      if(uiMaxTimeGpsReboot > MAX_TIME_NO_VALID_GPS) {
        xSemaphoreGive(mINIT_GPS_MODULE);
      }
      ucTimeoutMutexUartGps = 0;
    }
    else
    {
      ucTimeoutMutexUartGps++;
      if(ucTimeoutMutexUartGps > 5) {
        ucTimeoutMutexUartGps = 6;
        memset(&stGpsData, 0, sizeof(stGpsData));
        if(xQueueGpsForDebug != 0) {
           xQueueSendToFront(xQueueGpsForDebug, &stGpsData, (portTickType) 0);
        }
      }
    }
  }
 
}
