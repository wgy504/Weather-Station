
#include "onewire_general.h"
#include "DS18B20.h"
#include "includes.h"

int32_t ConvertsTemperatur(uint8_t *pIn);

void vOnewireTask (void *pvParameters)
{
  OW_Init();
  
  uint8_t ucTempBufTemper[2];
  TOnewire_Data stTemperaturData;
  char * pSt = (char*)&stTemperaturData;
  char toSend;
    
  memset(&stTemperaturData, 0, sizeof(stTemperaturData));
  /*
  // ������ �������
  xQueueOnewireDataToLcd = xQueueCreate(sizeof(stTemperaturData), sizeof(uint32_t));
  vQueueAddToRegistry(xQueueOnewireDataToLcd, "xQueueOnewireDataToLcd" );
  
   // ������ �������
  xQueueOnewireDataToSdcard = xQueueCreate(sizeof(stTemperaturData), sizeof(uint32_t));
  vQueueAddToRegistry(xQueueOnewireDataToSdcard, "xQueueOnewireDataToSdcard" );
  */
  // ������ �������
  xQueueOnewireDataToUsart = xQueueCreate(sizeof(stTemperaturData), sizeof(uint32_t));
  vQueueAddToRegistry(xQueueOnewireDataToUsart, "xQueueOnewireDataToUsart" );


  while(1)
  {
    // ���������� ��� ���������� ������������ �������� ��������� �����������    
    OW_Send(OW_SEND_RESET, "\xcc\x44", 2, NULL, NULL, OW_NO_READ);
    
    // ��������� ������� ������������ ������ - ������ "�������" �� ����
    OW_out_set_as_Power_pin();
    
    // ����������� ����� ��������� (�������� 750 �� ��� 12-������� ���������)
    _delay_ms(750);

    // ��������������� ������� ����������� UART
    OW_out_set_as_TX_pin();

    // ������ ��������� ������� ����������      
    if(OW_Send(OW_SEND_RESET, "\xcc\xbe\xff\xff", 4, ucTempBufTemper, 2, 2) == OW_OK) {
      stTemperaturData.iRealTemperatur = ConvertsTemperatur(ucTempBufTemper);
      stTemperaturData.bDataValid = 1;
    }
      else
      {
        stTemperaturData.bDataValid = 0;
      }
    /*
    if(xQueueOnewireDataToLcd !=0) {
      for(uint8_t i=0; i<sizeof(stTemperaturData); i++) {
        toSend = pSt[i];
        xQueueSendToFront(xQueueOnewireDataToLcd, ( void * ) &toSend, (portTickType) 0);
      }
    }
    
    if(xQueueOnewireDataToSdcard !=0) {
      for(uint8_t i=0; i<sizeof(stTemperaturData); i++) {
        toSend = pSt[i];
        xQueueSendToFront(xQueueOnewireDataToSdcard, ( void * ) &toSend, (portTickType) 0);
      }
    }
    */
    if(xQueueOnewireDataToUsart !=0) {
      for(uint8_t i=0; i<sizeof(stTemperaturData); i++) {
        toSend = pSt[i];
        xQueueSendToFront(xQueueOnewireDataToUsart, ( void * ) &toSend, (portTickType) 0);
      }
    }
    
    //_delay_ms(200);
  }
  
  vTaskDelete(NULL);
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
  
