
#include "http_menu.h"
#include "http_general.h"
#include "includes.h"


const char strMSG_heading[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n";                     // Heading
const char strMSG_html_enter[] = "<html>\r\n\r\n";
const char strMSG_html_exit[] = "</html>";
const char strMSG_head_enter[] = "<head>";
const char strMSG_meta[] = "<meta charset=\"utf-8\">";
const char strMSG_head_exit[] = "</head>";
const char strMSG_body_enter[] = "<body style=\"margin: 0px;\">";
const char strMSG_body_exit[] = "</body>";
const char strMSG_style_enter[] = "<style>";
const char strMSG_style_exit[] = "</style>";
const char strMSG_caption_enter[] = "<caption>";
const char strMSG_caption_exit[] = "</caption>";
const char strMSG_title_enter[] = "<title>";
const char strMSG_title_exit[] = "</title>\r\n";

const char strMSG_table_enter[] = "<table border=\"1\" align=\"center\" width=\"100%\">";
const char strMSG_table_exit[] = "</table>";

const char strMSG_caption_css[] = "caption{font-size: 50px;font-weight: bold;text-align: left;color: #0E1F9D;\r\ntext-shadow: 2px 2px 0 #ccc;}";         // CSS //em{color: #064A2C;}
const char strMSG_td_css[] = "td{font-size: 45px;font-weight: bold;text-align: center;}";

//const char strMSG_body_div[] = "<div class=\"top\"></div><div class=\"left\"></div>";

const char strMSG_h1_css_first[] = "h1{font-size: 55px;font-weight: bold;text-align: center;color: #0E1F9D;\r\ntext-shadow: 2px 2px 0 #ccc;}";         // CSS //em{color: #064A2C;}
const char strMSG_h2_css_first[] = "h2{font-size: 45px;font-weight: bold;text-align: center;}";

const char strMSG_body_css_first[] = "body{background: #00FFFA;}";


const char strMSG_title_first[] = "METEOCLOCK";            //title

const char strMSG_title_archive[] = "ARCHIVE";             //title


const char strMSG_welcome[] = "<h1>METEOCLOCK <em>Ver:16.05</em><hr></h1>\r\n";                 // Welcome Msg

#ifdef WIFI_MODE_AP
  const char strMSG_update[] = "<br><hr><h2><a href=\"http://192.168.4.1\">UPDATE</a></h2><hr>";      // End Msg (update)
  const char strMSG_back[] = "<br><hr><h2><a href=\"http://192.168.4.1\">BACK</a></h2><hr>";          // End Msg (back)
  const char strMSG_next_temperatur[] = "<br><hr><h2><a href=\"http://192.168.4.1/TEMPERATURE_EXT\">NEXT</a></h2><hr>";          // End Msg (next)
#endif
#ifdef WIFI_MODE_STA  
  const char strMSG_update[] = "<br><hr><h2><a href=\"http://192.168.0.101\">UPDATE</a></h2><hr>";      // End Msg (update)
  const char strMSG_back[] = "<br><hr><h2><a href=\"http://192.168.0.101\">BACK</a></h2><hr>";          // End Msg (back)
  const char strMSG_next_temperatur[] = "<br><hr><h2><a href=\"http://192.168.0.101/TEMPERATURE_EXT\">NEXT</a></h2><hr>";          // End Msg (next)
#endif

const char strMSG_Second_Title[5][25] = {
    "BAROMETR",
    "TEMPERATURE INT",
    "HUMIDITY",
    "CHARGER",
    "TEMPERATURE EXT"
};

char strTempMSG[256];
int FromSecondWebGuiInterface(char *p_strMsgWeb, E_HTTP_GET cRet)
{
  TArchive_Data stArchiveData;
  memset(strTempMSG, 0, strlen(strTempMSG));
  
  xSemaphoreGive(mARCHIVE_WIFI);
  if( (xQueueReceive(xQueueWiFiDataArchive,  &stArchiveData, (portTickType) SLEEP_MS_1000)) && (xQueueWiFiDataArchive !=0) ) {
    sprintf(strTempMSG, strMSG_Second_Title[cRet]);
      /* Heading *****************************/
      strcpy(p_strMsgWeb, strMSG_heading);
      strcat(p_strMsgWeb, strMSG_html_enter);
      /***************************************/
         /* Head Enter */
         strcat(p_strMsgWeb, strMSG_head_enter);
            //strcat(p_strMsgWeb, strMSG_meta);
            /* CSS **********************************/
            strcat(p_strMsgWeb, strMSG_style_enter);
            strcat(p_strMsgWeb, strMSG_caption_css);
            strcat(p_strMsgWeb, strMSG_h2_css_first);
            strcat(p_strMsgWeb, strMSG_td_css);
            strcat(p_strMsgWeb, strMSG_style_exit);  
            /***************************************/ 
            /* Title ******************************/
            strcat(p_strMsgWeb, strMSG_title_enter);  
            strcat(p_strMsgWeb, strTempMSG);
            strcat(p_strMsgWeb, strMSG_title_exit);
             /**************************************/
         /* Head Exit */
         strcat(p_strMsgWeb, strMSG_head_exit);
         
         /* Body Enter */
         strcat(p_strMsgWeb, strMSG_body_enter);
         
          /* Table Enter */
          strcat(p_strMsgWeb, strMSG_table_enter);
          
            /* Caption ******************************/
            strcat(p_strMsgWeb, strMSG_caption_enter); 
            strcat(p_strMsgWeb, strTempMSG);
            strcat(p_strMsgWeb, "<hr>");
            strcat(p_strMsgWeb, strMSG_caption_exit);
            /***************************************/
            
            for(uint8_t i=0; i<24; i++)
            {
              //Получаем  дату измерений
              sprintf(strTempMSG, "<tr><td>%.02i/%.02i/%.02i  %02i:%.02i</td>", 
                      stArchiveData.stTimeRecords[i].mday, 
                      stArchiveData.stTimeRecords[i].month,
                      stArchiveData.stTimeRecords[i].year,                     
                      stArchiveData.stTimeRecords[i].hour,
                      stArchiveData.stTimeRecords[i].min
                      );
              strcat(p_strMsgWeb, strTempMSG);
              
              //Определяем что будем передавать
              switch(cRet) {
                case BAROMETR:
                  sprintf(strTempMSG, "<td> %i mmHg </td></tr>\r\n", stArchiveData.iPressureHg[i]);
                  break;    
                case HUMIDITY:
                  sprintf(strTempMSG, "<td>  %i%  </td></tr>\r\n", stArchiveData.usHumidity[i]);
                  break;  
                case TEMPERATURE_INT:
                 sprintf(strTempMSG, "<td>  %.01f°C</td></tr>\r\n", stArchiveData.fIntTemperatur[i]);
                 break;  
                case CHARGER:
                 sprintf(strTempMSG, "<td> %.02f A</td></tr>\r\n", stArchiveData.fCharger[i]);
                 break;  
                case TEMPERATURE_EXT:
                 sprintf(strTempMSG, "<td>  %.01f°C</td></tr>\r\n", stArchiveData.fExtTemperatur[i]);
                 break; 
              }
              strcat(p_strMsgWeb, strTempMSG);
            }
            
            /* Table Exit */
            strcat(p_strMsgWeb, strMSG_table_exit);
          
            if(cRet==TEMPERATURE_INT) {
              /* NEXT */
              strcat(p_strMsgWeb, strMSG_next_temperatur);
            }
        
          /* BACK */
          strcat(p_strMsgWeb, strMSG_back);
        
     /* Body Exit */
     strcat(p_strMsgWeb, strMSG_body_exit);
         
    strcat(p_strMsgWeb, strMSG_html_exit);
    return strlen(p_strMsgWeb);
  }
  return 0;
}

int FromFirstWebGuiInterface(char *p_strMsgWeb)
{
  TIIC_WiFi_Or_Archive_Data stIicWiFiData;  
  RTC_t stDateRTC;
  THumidity_Data stHumidityData;
  float fOwTemper = 0;
  float fCharger = 0;
  memset(strTempMSG, 0, strlen(strTempMSG));
  
  float latitude = 0;
  float longitude = 0;
  int ucDeg_lt; 
  int ucDeg_lg;
  int uiDeg_lt_fr;
  int uiDeg_lg_fr;
  
  _Bool bValidQueueIIC;
  _Bool bValidQueueDHT;
  _Bool bValidQueueOW;
  _Bool bValidQueueCRG;
  
  xSemaphoreGive(mCRG_WIFI);
  xSemaphoreGive(mIIC_WIFI);                                              
  xSemaphoreGive(mDHT_WIFI); 
  xSemaphoreGive(mOWT_WIFI); 
  xSemaphoreGive(mTEMPER_OW);


  strcpy(p_strMsgWeb, strMSG_heading);
  strcat(p_strMsgWeb, strMSG_html_enter);
  /* Head Enter */
  strcat(p_strMsgWeb, strMSG_head_enter);
    //strcat(p_strMsgWeb, strMSG_meta);
    strcat(p_strMsgWeb, strMSG_style_enter);
      strcat(p_strMsgWeb, strMSG_h1_css_first);
      strcat(p_strMsgWeb, strMSG_h2_css_first);
      //strcat(p_strMsgWeb, strMSG_body_css_first);
    strcat(p_strMsgWeb, strMSG_style_exit);  
    strcat(p_strMsgWeb, strMSG_title_enter);  
      strcat(p_strMsgWeb, strMSG_title_first);
    strcat(p_strMsgWeb, strMSG_title_exit); 
  strcat(p_strMsgWeb, strMSG_head_exit);
  strcat(p_strMsgWeb, strMSG_body_enter);
    strcat(p_strMsgWeb, strMSG_welcome);
    
  if( (xQueueReceive(xQueueWiFiDataIIC,  &stIicWiFiData, (portTickType) SLEEP_MS_1000)) && (xQueueWiFiDataIIC !=0) ) {
    bValidQueueIIC = 1;
  }
  else {
     bValidQueueIIC = 0;
     memset(&stIicWiFiData, 0, sizeof(stIicWiFiData));
  }
 
  //Проверяем создана ли очередь и читаем из нее.  
  if( (xQueueReceive(xQueueWiFiDataCRG,  &fCharger, (portTickType) SLEEP_MS_1000)) && (xQueueWiFiDataCRG !=0) ) {
    bValidQueueCRG = 1;
  }
  else {
     bValidQueueCRG = 0;
     fCharger = 0;
  }
  
  if( (xQueueReceive(xQueueDhtDataToWifi,  &stHumidityData, (portTickType) SLEEP_MS_1000)) && (xQueueDhtDataToWifi !=0) ) {
      bValidQueueDHT = 1;
  }
  else {
     bValidQueueDHT = 0;
     memset(&stHumidityData, 0, sizeof(stHumidityData));
  }
  
  if( (xQueueReceive(xQueueWiFiDataOWT,  &fOwTemper, (portTickType) SLEEP_MS_1000)) && (xQueueWiFiDataOWT !=0) ) {
    bValidQueueOW = 1;
  }
  else {
     bValidQueueOW = 0;
     fOwTemper = 0;
  }
  
  /* Real Time */
  if(bValidQueueIIC) {
    sprintf(strTempMSG, "<hr><h2>Time: <em>%i:%.02i:%.02i,</em>   ", stIicWiFiData.stDS.hour, stIicWiFiData.stDS.min, stIicWiFiData.stDS.sec);
    strcat(p_strMsgWeb, strTempMSG);
    sprintf(strTempMSG, "   Date: <em>%.02i/%.02i/%i</em><hr></h2>", stIicWiFiData.stDS.mday, stIicWiFiData.stDS.month, stIicWiFiData.stDS.year);
    strcat(p_strMsgWeb, strTempMSG);
  }
  
  /* GPS Data */
  /*
  g_stGpsWiFiData.status = 1;
  g_stGpsWiFiData.latitude = 6000.19331;
  g_stGpsWiFiData.longitude = 3024.09331;*/
  if(g_stGpsWiFiData.status) 
  {
    /* Преобразование координат к форме выхода из gps модуля */
    latitude = ConvertLatitudeGpsFindMe(&g_stGpsWiFiData);
    longitude = ConvertLongitudeGpsFindMe(&g_stGpsWiFiData);

    /* Преобразование координат к форме google maps */
    ucDeg_lt = (int)latitude / 100;
    uiDeg_lt_fr = (int)((latitude - ucDeg_lt * 100)*1000*1000) / 60;
    ucDeg_lg = (int)longitude / 100;
    uiDeg_lg_fr = (int)((longitude - ucDeg_lg * 100)*1000*1000) / 60;
    /**************************************************/
    
    //https://www.google.ru/maps?q=60.031948+30.403660&t=m&z=16
    /* Форма выпуска ссылки на google maps */
    sprintf((char*)(strTempMSG), "<hr><h2><a href=\"https://www.google.com/maps?q=%02d.", ucDeg_lt);
    sprintf((char*)(strTempMSG+strlen(strTempMSG)), "%06d+", uiDeg_lt_fr);
    sprintf((char*)(strTempMSG+strlen(strTempMSG)), "%03d.", ucDeg_lg);
    sprintf((char*)(strTempMSG+strlen(strTempMSG)), "%06d&z=16\n\">", uiDeg_lg_fr);
    sprintf(strTempMSG+strlen(strTempMSG), "GPS Valid, Lat: <em>%i.%i</em>, Long: <em>%i.%i</em></a></h2><hr>", ucDeg_lt, uiDeg_lt_fr, ucDeg_lg, uiDeg_lg_fr);
    strcat(p_strMsgWeb, strTempMSG);
  }
  else
  {    
    g_stGpsWiFiData.time = BKP_ReadBackupRegister(BKP_DR2);
    g_stGpsWiFiData.time = g_stGpsWiFiData.time<<16;
    g_stGpsWiFiData.time |= BKP_ReadBackupRegister(BKP_DR1);
    
    strcat(p_strMsgWeb, "<hr><h2>GPS Not valid,  Data valid: ");
    Sec2Date(&stDateRTC, g_stGpsWiFiData.time);  
    sprintf(strTempMSG, "<em>%.02i/%.02i/%i</em></h2><hr>", stDateRTC.mday, stDateRTC.month, stDateRTC.year);
    strcat(p_strMsgWeb, strTempMSG);
  }

  
  /* Pressure and Temperature */
  if(bValidQueueIIC) {
    /* Pressure */
    float fPressKPa = stIicWiFiData.stBMP.iRealPressurePa;
    fPressKPa /= 1000;
    stIicWiFiData.stBMP.iRealPressureHg = BMP180_kpa_to_mmhg(stIicWiFiData.stBMP.iRealPressurePa); 
#ifdef WIFI_MODE_AP
    sprintf(strTempMSG, "<hr><h2><a href=\"http://192.168.4.1/BAROMETR\">Barometer: <em>%i</em>mmHg (<em>%.03f</em>kPa)</a></h2><hr>", stIicWiFiData.stBMP.iRealPressureHg, fPressKPa);
#endif
#ifdef WIFI_MODE_STA
    sprintf(strTempMSG, "<hr><h2><a href=\"http://192.168.0.101/BAROMETR\">Barometer: <em>%i</em>mmHg (<em>%.03f</em>kPa)</a></h2><hr>", stIicWiFiData.stBMP.iRealPressureHg, fPressKPa);
#endif
    strcat(p_strMsgWeb, strTempMSG);
    
    /* Temperature */
    float fTemperInt = stIicWiFiData.stBMP.iRealTemperatur;
    fTemperInt /= 10;
    float fTemperExt;
    if(bValidQueueOW) {
      fTemperExt = fOwTemper;
    }
    else {
      fTemperExt = 0;
    }
#ifdef WIFI_MODE_AP
    sprintf(strTempMSG, "<hr><h2><a href=\"http://192.168.4.1/TEMPERATURE_INT\">Temperature Int: <em>%.02f</em> °C, Ext: <em>%.02f</em> °C</a></h2><hr>", fTemperInt, fTemperExt);
#endif
#ifdef WIFI_MODE_STA
    sprintf(strTempMSG, "<hr><h2><a href=\"http://192.168.0.101/TEMPERATURE_INT\">Temperature Int: <em>%.02f</em> °C, Ext: <em>%.02f</em> °C</a></h2><hr>", fTemperInt, fTemperExt);
#endif
    strcat(p_strMsgWeb, strTempMSG);
  }
  
  /* Humidity */
  if(bValidQueueDHT) {
    if(stHumidityData.bDataValid == NULL) {
      stHumidityData.usRealHumidity = 0;
    }
#ifdef WIFI_MODE_AP
    sprintf(strTempMSG, "<hr><h2><a href=\"http://192.168.4.1/HUMIDITY\">Humidity: <em>%i</em>%</a></h2><hr>", stHumidityData.usRealHumidity);
#endif
#ifdef WIFI_MODE_STA   
   sprintf(strTempMSG, "<hr><h2><a href=\"http://192.168.0.101/HUMIDITY\">Humidity: <em>%i</em>%</a></h2><hr>", stHumidityData.usRealHumidity);
#endif
    strcat(p_strMsgWeb, strTempMSG);
  }
  
  /* Charger */
  bValidQueueCRG = 1;
  if(bValidQueueCRG) {
#ifdef WIFI_MODE_AP    
    sprintf(strTempMSG, "<hr><h2><a href=\"http://192.168.4.1/CHARGER\">Charger: <em>%.03f</em>A</a></h2><hr>", fCharger);
#endif
#ifdef WIFI_MODE_STA     
    sprintf(strTempMSG, "<hr><h2><a href=\"http://192.168.0.101/CHARGER\">Charger: <em>%.03f</em>A</a></h2><hr>", fCharger);
#endif    
    strcat(p_strMsgWeb, strTempMSG);
  }
  
  /* UPDATA */
  strcat(p_strMsgWeb, strMSG_update);
  strcat(p_strMsgWeb, strMSG_body_exit);
  strcat(p_strMsgWeb, strMSG_html_exit);

  return strlen(p_strMsgWeb);
}

E_HTTP_GET HttpGetParser(char *ptEspRet)
{
  char *pFind;
  pFind = strstr(ptEspRet, "GET /BAROMETR");
  if(pFind>0) {
    return BAROMETR;
  }
  
  pFind = strstr(ptEspRet, "GET /TEMPERATURE_INT");
  if(pFind>0) {
    return TEMPERATURE_INT;
  }
  
  pFind = strstr(ptEspRet, "GET /HUMIDITY");
  if(pFind>0) {
    return HUMIDITY;
  }
  
  pFind = strstr(ptEspRet, "GET /CHARGER");
  if(pFind>0) {
    return CHARGER;
  } 
  
  pFind = strstr(ptEspRet, "GET /TEMPERATURE_EXT");
  if(pFind>0) {
    return TEMPERATURE_EXT;
  } 
  
  pFind = strstr(ptEspRet, "GET / HTTP/1.1");
  if(pFind>0) {
    return FIRST_MENU;
  } 
  
  return NO;
}