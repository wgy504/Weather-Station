
#ifndef _SRAM_H
#define _SRAM_H

#include "includes.h"
#include "sram.h"

typedef enum TYPE_GSM {
   GSM_ERROR_TYPE       = 0,
   CINTERION            = 1,
   SIMCOM               = 2,
} TYPE_GSM;

enum {
    SIZE_MOBILE_COUNTRY_CODE = 6,
    SIZE_PIN_CODE =             6,
    SIZE_PASW =                 12,
    SIZE_FTP_USER =             16,
    SIZE_MODEM_IDENT =          18,
    SIZE_TEL =                  20,
    SIZE_TIME =                 20,
    SIZE_IMEI =                 20,
    SIZE_NAME_GSM_SOFT =        23,
    SIZE_SCID =                 24,
    SIZE_TNUM =                 24,
    SIZE_TOKEN =                25,
    LOGIN_PASS_SIZE =           30,
    SIZE_SERV =                 32,
    SIZE_SERV_FTP =             40,     
    SIZE_USER_REG_INFO =        100,
    SIZE_GPS_POSITION  =        100,
};

typedef enum SIM_STATUS {
   SIM_WAIT =           0,
   HOME_NET =           1,
   ROAMING_NET =        2,
   FIND_NET =           3,
   SIM_ERROR =          10,
   SIM_NO_READY =       11,
   SIM_PIN =            12,
   SIM_PUK =            13,
   SIM_PIN_GUESSING =   14,   
   SIM_PIN_NO_SET =     15,
}SIM_STATUS;

//������ � ���������
typedef enum TYPE_SERVER {
   FIRST_SERVER =       0,      //���������  
   SECOND_SERVER =      1,      //���������
   OTHER_SERVER =       2,      //�������� ������
   NO_SERVER =          3,
} TYPE_SERVER;

/*������ ����������������� */
typedef enum {
   POWER_RUN_MODE =             0,      //������� ����� ������ ^_^
   POWER_LOW_PWR1_MODE =        1,      //������ -_-
   POWER_LOW_PWR2_MODE =        2,      //����      -_-oO
   POWER_SLEEP_MODE =           3,      //�������� ���  -_-zZ
}PWR_STATUS;

//������� ������� �������������� ����������.
typedef enum LED_Status {
   LOAD_DEVICE =        0,      //�������� ����������
   FIND_SIM =           1,      //����� ��� �����
   REG_GSM =            2,      //�������� ����������� GSM
   SERVER_CONNECT =     3,      //����������� � ���� GSM ��������, �������� ����������� � �������
   NORMAL_RUN =         4,      //����������� � ���� GSM ��������, ���������� ���������� � �������
   LOW_PWR1 =           5,      //���������� ��������� � ������� ������ LOW_PWR1
   LOW_PWR2 =           6,      //���������� ��������� � ������� ������ LOW_PWR2     
}LED_Status;

/* ������ ������������ � �������������� */
typedef enum RESET_STATUS_DEVICE {
   POWER_ON =                   0,
   NO_RESET =                   1,      //����� ������������ �� ����.
   
/* ���������� ������������ */
   BUTTON_RESET =               2,      //������������ �� �������/������ RESET (�� �����������).
   CMD_RESET =                  3,      //������������ �� ������� �������.
   WAKE_UP_LOW_PWR1 =           4,      //����� �� ������� ������ � ������ ����������������� LOW PWR1.
   WAKE_UP_LOW_PWR2 =           5,      //����� �� ������� ������ � ������ ����������������� LOW PWR2.
   WAKE_UP_ALARM =              6,      //����� �� ������� ������ �� ���������� ������� SLEEP.
   WAKE_UP_ACCEL =              7,
   UPDATE_FIRM_DEVICE =         8,      //������ ������� ��������.
   UPDATE_BASE_DEVICE =         9,      //������ ��������� �� ������� ��������.    
   SLEEP_DEVICE =               10,
   
/* �������������� ������� ����� ����� ������� ������������ */
   WARNING_RTOS_HEAP_SIZE_FAIL =        11,     //�������������� � ����� ������� ���� FreeRTOS.
   WARNING_RTOS_TASK_NUM_FAIL =         12,     //��������� ���������� ����� ��������� FreeRTOS.
   WARNING_PWR_RESET =                  13,     //��������� ������ ������� ����������.
   WARNING_PWR_ONLY_USB =               14,     //���� ������� ������ �� USB.
   WARNING_USB_CONNECT =                15,     //����������� ������� � USB.
   WARNING_GPS_DMA_FAIL =               16,     //
   WARNING_HSE_FAIL =                   17,     //������ ������� ��������� �������� ���������� ����������.
   WARNING_ACCEL_FAIL =                 18,     //����� �������������.
   WARNING_EEPROM_FAIL =                19,     //���� EEPROM
   
/* ��������� ������ */
   ERR_HARD_FAULT =             20,     //������ � Hard Fault.
   ERR_GSM_FAIL =               21,     //��������� GSM ������.
   ERR_WATCHDOG_RESET =         22,     //�������� Watchdog �� ���������.
   ERR_FLASH_TASK_FAIL =        23,     //��������� ��������� FreeRTOS.
   ERR_GSM_TASK_FAIL =          24,     //��������� ��������� FreeRTOS.
   ERR_GPS_TASK_FAIL =          25,     //��������� ��������� FreeRTOS.
   ERR_PERIPH_TASK_FAIL =       26,     //��������� ��������� FreeRTOS. 
   ERR_SENSOR_TASK_FAIL =       27,     //��������� ��������� FreeRTOS. 
   ERR_USB_TASK_FAIL =          28,     //��������� ��������� FreeRTOS.

} RESET_STATUS_DEVICE;

                                                    //��������� ������.

typedef __packed struct {
  uint64_t ucIMEI;                                              // IMEI GSM ������  
  char strStrIMEI[SIZE_IMEI];                                   // IMEI GSM ������ � ������
  uint8_t aucGsmCsq[2];                                         // ������� CSQ
  char strGsmModemIdentification[SIZE_MODEM_IDENT];             // ������������� gsm ������ (GSM900 / GSM1800 / UMTS2100 /LTE1800 / LTE2600).
  char strGsmModemSoftware[SIZE_NAME_GSM_SOFT];                 // ������ �������� gsm ������.
  TYPE_GSM eGsmType;                                            // ��� GSM ������.
  int8_t cTemperatur;
  uint32_t usTimePwrGsmNotSim;
  uint32_t usTimePwrGsmSim;
} TGsm;


typedef __packed struct {
  char strUSER_SCID[SIZE_SCID];                                 // SCID ��� ����� 
  char strOTHER_SCID[SIZE_SCID];                                 // SCID ��� ����� 
  SIM_STATUS eRegStatus;                                        // �������/�� �������    
  char acMobCountCode[SIZE_MOBILE_COUNTRY_CODE];                // ��� ��������� ��������� �����.
  _Bool bGprsTrue;
} TSim;

typedef __packed struct {
  TYPE_SERVER eConnectNumSer;                           //����� ������� � �������� �� ���������(0 - FIRST, 1 - SECOND, 3 - AUTOFON, 4 - NO SER).
  _Bool bGprsProfActivate;
  _Bool bWaitGpsModeFm;
}TConnect;



typedef __packed struct {
   TGsm stGsm;
   TSim stSim;
   TConnect stConnect;
}TRam;
GLOBAL TRam g_stRam;  

GLOBAL int g_iFramePaketNumber    _EQU(0);


#endif