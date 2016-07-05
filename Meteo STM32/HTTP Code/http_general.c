#include "http_general.h"
#include "includes.h"

#define ESP_RX_BUFFER_SIZE 2048
#define ESP_TX_BUFFER_SIZE 2048

char aucRxBufferESP[ESP_RX_BUFFER_SIZE];
char aucTxBufferESP[ESP_RX_BUFFER_SIZE];

#define TIME_SEND_DATA_SERVER   5000


#define MAX_IP_ADDRESS          20 
#define MAX_MAC_ADDRESS         20
#define MAX_NAME_SSID_LEN       20
#define MAX_PASS_LEN            30

typedef enum {
   OPEN           = 0,
   WEP            = 1,
   WPA_PSK        = 2,
   WPA2_PSK       = 3,
   WPA_WPA2_PSK   = 4
} TYPE_SECURITY_NETWORK;



#define IP_ADDRESS              "narodmon.ru"
#define MAC_ADDRESS             "18-FE-34-D4-3A-BF"     //18:fe:34:d4:3a:bf   
#define SSID_NETWORK            "HomeInternet"          //"RF_Guest"
#define PASSWORD_NETWORK        "OmMaNiPadmeHum"        //"8123181819"

#define CHANNEL                 10
#define TCP_PORT                8283//80

typedef struct {
  char strIpAddressServer[MAX_IP_ADDRESS];
  char strMacAddress[MAX_MAC_ADDRESS];
  char srtSsid[MAX_NAME_SSID_LEN];
  char srtPasswd[MAX_PASS_LEN];
  uint8_t ucTypeSecurity;
  uint8_t ucChannel;
  uint16_t usTcpPort;
} TEspNetConfig;

int InitEspSTA(char *ptEspRet, uint16_t Len, TEspNetConfig *pEspNetConfig);
int ReadDataWiFi(char *ptEspRet, uint16_t Len);
int SendDataWiFi(int iProf, char *ptEspRet, uint16_t Len);
int SocketClose(int iProf);
int SendDataServer(int iProf, char *ptEspRet, uint16_t Len, TEspNetConfig *pEspNetConfig, TServer_Data *pServerData);
void Delay(portTickType eDelayMS);

void vWifiEspTask (void *pvParameters)
{ 
  int iNumConnect = 0;
  int iLen;
  uint32_t uiTimeSendDataServer = 0;
  
  portTickType xLastWakeTimerDelay;

  TServer_Data stServerData;
  
  TEspNetConfig stEspNetConfig;
  strcpy(stEspNetConfig.strIpAddressServer, IP_ADDRESS);
  strcpy(stEspNetConfig.strMacAddress, MAC_ADDRESS);
  strcpy(stEspNetConfig.srtSsid, SSID_NETWORK);
  strcpy(stEspNetConfig.srtPasswd, PASSWORD_NETWORK);
  stEspNetConfig.ucTypeSecurity = OPEN;
  stEspNetConfig.ucChannel = CHANNEL;
  stEspNetConfig.usTcpPort = TCP_PORT;
  
  xQueueServerData = xQueueCreate(sizeof(uint8_t), sizeof(TServer_Data));
  vQueueAddToRegistry(xQueueServerData, "xQueueServerData");
  
  osDelay(SLEEP_MS_1000);
  InitUSART(UART_ESP, ESP_BAUDRATE);
  InitDMA(UART_ESP);
  
  xSemaphoreGive(mINIT_WIFI_ESP);               //Первоначальная инициализация драйвера ESP.
  
  while(1)
  {
    if(osMutexWait(mINIT_WIFI_ESP, SLEEP_MS_100) == osOK) {             //Инициализация драйвера ESP(происходит по Мьютексу.
       if(InitEspSTA(aucTxBufferESP, sizeof(aucTxBufferESP), &stEspNetConfig)) {
         xLastWakeTimerDelay = xTaskGetTickCount();
         vTaskDelayUntil(&xLastWakeTimerDelay, (3000 / portTICK_RATE_MS));  
         xSemaphoreGive(mINIT_WIFI_ESP);
       }
      else
      {
        xSemaphoreTake(mINIT_WIFI_ESP, 0);
      }
    }

    
    if( (xQueueReceive(xQueueServerData,  &stServerData, (portTickType) 0)) && (xQueueServerData !=0) ) 
    { 
      iNumConnect = 2;
      SendDataServer(iNumConnect, aucTxBufferESP, iLen, &stEspNetConfig, &stServerData);
      xLastWakeTimerDelay = xTaskGetTickCount();
      vTaskDelayUntil(&xLastWakeTimerDelay, (5000 / portTICK_RATE_MS));  
      SocketClose(iNumConnect);
      xLastWakeTimerDelay = xTaskGetTickCount();
      vTaskDelayUntil(&xLastWakeTimerDelay, (5000 / portTICK_RATE_MS));  
      xSemaphoreGive(mINIT_WIFI_ESP);
    }
    
    //Ставим мьютекс готовить данных для сервера.
    uiTimeSendDataServer++;
    if(uiTimeSendDataServer > TIME_SEND_DATA_SERVER) {
        uiTimeSendDataServer = 0;
        xSemaphoreGive(mSEND_DATA_SERVER);
    }
    
  }
}

  
int ReadDataWiFi(char *ptEspRet, uint16_t Len)
{
  ESP_INFO stEspInfo;
  int iLenAns;
  
  iLenAns = esp_read(ptEspRet, Len);
  if((iLenAns <= 0) || (iLenAns > Len) ) return 0;

  esp_parser(&stEspInfo, "", ptEspRet, Len);
  
  if((stEspInfo.eResType == RET_IPD) || (stEspInfo.eResType == RET_CONNECT)) { 
     if(stEspInfo.eResType == RET_CONNECT) {
       iLenAns = esp_read(ptEspRet, Len);
       if(iLenAns>0) {
        esp_parser(&stEspInfo, "", ptEspRet, Len);
       }
     }
     memcpy(ptEspRet, ptEspRet, sizeof(ptEspRet));
     return (stEspInfo.iValueParamRes[0] + 1);             //Возвращает номер открытого коннекта.
  }
  
  if(stEspInfo.eResType == RET_CLOSE) {
    return (stEspInfo.iValueParamRes[0] + 1) * -1;         //Возвращает номер закрытого коннекта.
  }
  
  if(stEspInfo.eResType == RET_OK) {
    return 0;
  }
  
  return ERR_UNK_ANS;
}


int SocketClose(int iProf)
{
  ESP_INFO stEspInfo; 
  memset(&stEspInfo, 0, sizeof(stEspInfo));
  char strTempCmd[64];
  int iNumSocket;
  if(iProf <=0 ){
    return ERR_SOCKET;
  }
  iNumSocket = iProf - 1;
  
  sprintf(strTempCmd, "%s%i", AT_CIPCLOSE, iNumSocket);
  cmd_send(strTempCmd, &stEspInfo, strTempCmd, strlen(strTempCmd));  
  if(stEspInfo.eResType != RET_OK) {
    return ERR_NO_PROMPT;
  }
  return RET_OK;
}

int SendDataWiFi(int iProf, char *ptEspRet, uint16_t Len)
{
  ESP_INFO stEspInfo; 
  memset(&stEspInfo, 0, sizeof(stEspInfo));
  char strTempCmd[128];
  int iLenAns;
  int iNumSocket;
  
  if(iProf <=0 ){
    return ERR_SOCKET;
  }
  
  iNumSocket = iProf - 1;
  
  sprintf(strTempCmd, "%s%i,%i", AT_CIPSEND, iNumSocket, Len);
  cmd_send(strTempCmd, &stEspInfo, strTempCmd, strlen(strTempCmd));  
  
  for(uint8_t i=0; i<10; i++) {
    iLenAns = esp_read(strTempCmd, sizeof(strTempCmd));
    if(iLenAns>0) break;
    osDelay(SLEEP_MS_10);
  }
  esp_parser(&stEspInfo, "", strTempCmd, sizeof(strTempCmd));
  
  if(stEspInfo.eResType != RET_PROMPT) {
    return ERR_SEND_DATA;
  }
  
  /* Передаем данные */
  for(uint16_t i=0; i<Len; i++) {
      esp_write(&ptEspRet[i], 1);
  }
  
  for(uint8_t i=0; i<50; i++) {
    iLenAns = esp_read(strTempCmd, sizeof(strTempCmd));
    if(iLenAns>0) break;
    osDelay(SLEEP_MS_10);
  }
  
  if(iLenAns <= 0) {
  //   return ERR_TIMEOUT;
  }
  
  esp_parser(&stEspInfo, "", strTempCmd, sizeof(strTempCmd));

  if(stEspInfo.eResType == RET_SEND_OK) {
    return RET_SEND_OK;
  }
  else {
    memset(strTempCmd, 0, sizeof(strTempCmd));
    osDelay(SLEEP_MS_50);       
    for(uint8_t i=0; i<50; i++) {
        iLenAns = esp_read(strTempCmd, sizeof(strTempCmd));
        if(iLenAns>0) break;
        osDelay(SLEEP_MS_10);
      }
      esp_parser(&stEspInfo, "", strTempCmd, sizeof(strTempCmd));
      if(stEspInfo.eResType == RET_SEND_OK) {
        return RET_SEND_OK;
      }
  }
  return ERR_SEND_DATA;
}


#define ESP_CMD_DELAY         500

int InitEspSTA(char *ptEspRet, uint16_t Len, TEspNetConfig *pEspNetConfig)
{
  uint8_t ucTimeout = 0;
  
  Delay(SLEEP_MS_3000);
  
  int iLenAns;
  ESP_INFO stEspInfo;    
  memset(&stEspInfo, 0, sizeof(stEspInfo));
  char strCmd[64];
  
  cmd_send(AT_RST, &stEspInfo, ptEspRet, Len);
  Delay(SLEEP_MS_3000);
  
  cmd_send(AT, &stEspInfo, ptEspRet, Len);  
  if(stEspInfo.bEcho == FALSE) {
    cmd_send(ATE1, &stEspInfo, ptEspRet, Len);
    osDelay(ESP_CMD_DELAY);
  }
  
  //Set mode Wifi STA.
  cmd_send(ESP_MODE_STA, &stEspInfo, ptEspRet, Len);
  Delay(SLEEP_MS_5000);
  
  // Set config WiFi.
  sprintf(strCmd, "%s\"%s\",\"%s\"", AT_CWJAP, pEspNetConfig->srtSsid, pEspNetConfig->srtPasswd);
  memset(ptEspRet, 0, Len);
  cmd_send(strCmd, &stEspInfo, ptEspRet, Len);
  Delay(SLEEP_MS_5000);
  ucTimeout = 0;
  while(ucTimeout < 10)  
  {
    iLenAns = esp_read(ptEspRet, Len);
    if(iLenAns>0) {
        esp_parser(&stEspInfo, "", ptEspRet, Len);
    }
    if( (stEspInfo.eResType == RET_OK) || (stEspInfo.eResType == RET_busy) || (stEspInfo.eResType == RET_CONNECT) ) {
      break;
    }
    ucTimeout++;
    Delay(SLEEP_MS_1000);
  }
  if(stEspInfo.eResType == RET_busy) {
    return -1;
  }
    
    //Set mode Wifi STA.
  cmd_send(AT_CIFSR, &stEspInfo, ptEspRet, Len);
  if(stEspInfo.eResType != RET_OK) {
    return -1;
  }
  Delay(ESP_CMD_DELAY);
  
  cmd_send(ESP_MULTIPE_CONNECTION, &stEspInfo, ptEspRet, Len);
  if(stEspInfo.eResType != RET_OK) {
    return -1;
  }
  Delay(ESP_CMD_DELAY);
  
  //Start web server
  sprintf(strCmd, "%s,%i", ESP_START_SERVER, pEspNetConfig->usTcpPort);
  cmd_send(strCmd, &stEspInfo, ptEspRet, Len);
  if(stEspInfo.eResType != RET_OK) {
    return -1;
  }  
  return 0;
}



int SendDataServer(int iProf, char *ptEspRet, uint16_t Len, TEspNetConfig *pEspNetConfig, TServer_Data *pServerData)
{
  ESP_INFO stEspInfo; 
  memset(&stEspInfo, 0, sizeof(stEspInfo));
  char strTempCmd[128];
  int iNumSocket;
  int iLenAns;
  GPS_INFO stGpsData;          //Структура GPS.
  float latitude = 0;
  float longitude = 0;
  int ucDeg_lt; 
  int ucDeg_lg;
  int uiDeg_lt_fr;
  int uiDeg_lg_fr;
  
  if(iProf <=0 ){
    return ERR_SOCKET;
  }
  
  iNumSocket = iProf - 1;
  
  sprintf(strTempCmd, "%s%i,\"TCP\",\"%s\",%i", AT_CIPSTART, iNumSocket, pEspNetConfig->strIpAddressServer, pEspNetConfig->usTcpPort);
  cmd_send(strTempCmd, &stEspInfo, strTempCmd, strlen(strTempCmd));  
  osDelay(SLEEP_MS_2000);
  
  
  sprintf(ptEspRet, "#%s#Meteo STM32",  pEspNetConfig->strMacAddress);
  
  if(pServerData->bGpsDataValid) {                 //Если есть координаты, то передаем их
      stGpsData.latitude = pServerData->fLatitude;
      stGpsData.longitude = pServerData->fLongitude;
      
      /* Преобразование координат к форме выхода из gps модуля */
      latitude = ConvertLatitudeGpsFindMe(&stGpsData);
      longitude = ConvertLongitudeGpsFindMe(&stGpsData);
      
      /* Преобразование координат к форме google форме */
      ucDeg_lt = (int)latitude / 100;
      uiDeg_lt_fr = (int)((latitude - ucDeg_lt * 100)*1000*1000) / 60;
      ucDeg_lg = (int)longitude / 100;
      uiDeg_lg_fr = (int)((longitude - ucDeg_lg * 100)*1000*1000) / 60;
      /**************************************************/
      
      sprintf(strTempCmd, "#%02d.%06d", ucDeg_lt, uiDeg_lt_fr);
      strcat(ptEspRet, strTempCmd);
      sprintf(strTempCmd, "#%03d.%06d", ucDeg_lg, uiDeg_lg_fr);
      strcat(ptEspRet, strTempCmd);
  }
  strcat(ptEspRet, "\n");
  

  if( (pServerData->fExtTemperatur > -40) && (pServerData->fExtTemperatur < 80) ) {
    sprintf(strTempCmd, "#T1#%.02f#Ext Temperatur\n", pServerData->fExtTemperatur);
    strcat(ptEspRet, strTempCmd);
  }
  
  if( (pServerData->fIntTemperatur > -40) && (pServerData->fIntTemperatur < 80) ){
    sprintf(strTempCmd, "#T2#%.02f#Int Temperatur\n", pServerData->fIntTemperatur);
    strcat(ptEspRet, strTempCmd);
  }
  
  if( (pServerData->usExtHumidity > 0) && (pServerData->usExtHumidity < 100) ){
    sprintf(strTempCmd, "#H1#%i#Ext Humidity\n", pServerData->usExtHumidity);
    strcat(ptEspRet, strTempCmd);
  }
    
  if( (pServerData->iPressureHg > 600) && (pServerData->iPressureHg < 820) ) {
    sprintf(strTempCmd, "#P1#%i#Pressure\n", pServerData->iPressureHg);
    strcat(ptEspRet, strTempCmd);
  }

  strcat(ptEspRet, "##");
    
  Len = strlen(ptEspRet);
  memset(strTempCmd, 0, sizeof(strTempCmd));
  sprintf(strTempCmd, "%s%i,%i", AT_CIPSEND, iNumSocket, Len);
  cmd_send(strTempCmd, &stEspInfo, strTempCmd, strlen(strTempCmd));  
 
  osDelay(SLEEP_MS_10);
  /* Передаем данные */
  for(uint16_t i=0; i<Len; i++) {
      esp_write(&ptEspRet[i], 1);
  }
  
   for(uint8_t i=0; i<50; i++) {
    iLenAns = esp_read(strTempCmd, sizeof(strTempCmd));
    if(iLenAns>0) break;
    osDelay(SLEEP_MS_10);
  }
  
  if(iLenAns <= 0) return RET_OK; //ERR_TIMEOUT;
  
  esp_parser(&stEspInfo, "", strTempCmd, sizeof(strTempCmd));

  if(stEspInfo.eResType == RET_SEND_OK) {
    return RET_SEND_OK;
  }
  else {
    memset(strTempCmd, 0, sizeof(strTempCmd));
    osDelay(SLEEP_MS_50);       
    for(uint8_t i=0; i<50; i++) {
        iLenAns = esp_read(strTempCmd, sizeof(strTempCmd));
        if(iLenAns>0) break;
        osDelay(SLEEP_MS_10);
      }
      esp_parser(&stEspInfo, "", strTempCmd, sizeof(strTempCmd));
      if(stEspInfo.eResType == RET_SEND_OK) {
        return RET_SEND_OK;
      }
  }
  return ERR_SEND_DATA;
}

void Delay(portTickType eDelayMS) 
{
  portTickType xLastWakeTimerDelay;
  xLastWakeTimerDelay = xTaskGetTickCount();
  vTaskDelayUntil(&xLastWakeTimerDelay, (eDelayMS / portTICK_RATE_MS));
}