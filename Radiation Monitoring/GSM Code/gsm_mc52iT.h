#ifndef _GSM_MC52IT_H_
#define _GSM_MC52IT_H_

#include "includes.h"
#include "gsm_parser.h"
#include "sram.h"

/* ��������: ������� ������ ������ ������ ���������� ������� */
#ifdef GSM_MODULE_SIM800
  #define ATZ                           "atz"
  #define AT_CONFIG_LED                 "at+csgs=1"                             //������� GPIO LED ��� ��������� GSM ���� � �������
  #define AT_SCID                       "at+ccid"                               //������ ���������������� ������ ��� �����
  #define AT_SMGL                       "at+cmgl=4"                             //������ ��� ���
  #define AT_SPIC                       "at+spic"                               //��������� ������� ��� �������� ������� ������

  #define AT_CGATT                      "at+cgatt?"                             //��������� ������ GPRS (SIM800)
  #define AT_CIPMUX                     "at+cipmux=1"                           //�������� ����������� � ����� ���������� ���� ��������
  #define AT_SEND                       "at+cipsend="                           //��������� �� ������� ������
  #define AT_SISR_FTP                   "at+ftpget="                            //��������� ������ ftp

  #define ADD_SEND_PAR                  ""
  #define AT_SMSO                       "at+cpowd=1"                            //��������� ������� ������(������).
  #define AT_CIPCLOSE                   "at+cipclose="                          //������ �����
  #define AT_CIPSTATUS                  "at+cipstatus="
  #define AT_CNETSCAN1                  "at+cnetscan=1"                         //�������� ����� ������ ���� � gsm ��������
  #define AT_SMOND                      "at+cnetscan"                           //����� ������� �������

  #define AT_CIPHEAD1                   "at+ciphead=1"                          //�������� ��������� � ���������� ���� �������� �� GPRS ����� ������ �������

  #define AT_CMGD                       "at+cmgda="

  /* ������� ����������� ������ ��� SIM800 */
  #define AT_BT_ON                      "at+btpower=1"                          //��������� Bluetooth
  #define AT_BT_OFF                     "at+btpower=0"                          //���������� Bluetooth

  #define AT_JD_ON                      "at+sjdr=1,1"                           //��������� ������ �������������� �������� GSM ������� � ����� ������ � UART.
  #define AT_JD_OFF                     "at+sjdr=0"                             //���������� ������ �������������� �������� GSM �������

#else

  #define ATZ                          "atz"
  #define AT_CONFIG_LED                 "at^ssync=1"                            //������� GPIO LED ��� ��������� GSM ���� � �������
  #define AT_SCID                       "at^scid"                               //������ ���������������� ������ ��� �����
  #define AT_SMGL                       "at^smgl=0"                             //������ ��� ���
  #define AT_SPIC                       "at^spic"                               //
  #define AT_SEND                       "at^sisw="                              //��������� �� ������� ������

  #define ADD_SEND_PAR                  ",0,0"
  #define AT_CIPCLOSE                   "at^sisc="                              //������ �����
  #define AT_CIPSTATUS                  "at^sisi="
  #define AT_CNETSCAN1                  ""
  #define AT_SMOND                      "at^smond"                              //����� ������� �������
  #define AT_SISR_FTP                   "at^sisr="                              //��������� ������ ftp

  #define AT_CMGD                       "at+cmgd="

  #define AT_SMSO                       "at^smso"                               //������� � ����������.

  #define AT_CIPHEAD1                   ""                                      //�������� ��������� � ���������� ���� �������� �� GPRS ����� ������ �������

  #define AT_BT_ON                      ""                          
  #define AT_BT_OFF                     ""                          

  #define AT_JD_ON                      ""                           
  #define AT_JD_OFF                     ""         

#endif  


#define SMS_PDU
//#define SMS_TEXT

//#define CLI_ENABLE //��� �������
#define MULTI_IP_CONNECTION_ENABLE      //�������� ����������� � ����� ���������� ���� ��������

#define MC_COUNT 3

typedef enum RET_INFO{
    RET_OK = 0,
    ERR_CMD,
    ERR_POWER,  //2
    ERR_RESTART,
    ERR_TIMEOUT,
    ERR_SIMCART,
    ERR_SIM_NO_READY,
    ERR_GSMNET,
    ERR_APN,
    ERR_GPRS_ACTIVATE,
    ERR_GPRS_TIMEOUT,   //10
    ERR_GPRS_SEND,      //11
    ERR_SERVER_CONNECT,
    ERR_SERVER_NO_ANSWER,
    ERR_EE_DATA,
    ERR_TEMPERATURE,
    ERR_ABORTED,
    IT_IS_OK,
    IT_IS_BREAK,
    HACK_TRANSMIT_DATA,
    ERR_SIM_LITTLE_PIN,
    RET_GPRS_OK,
    RET_GPRS_SEND_OK,
    RET_GPRS_RECEIVE_OK,
    ERR_WRITE_FLASH,    //25
    ERR_DOWNLOAD_FIRMWARE,
    ERR_DMA,
    RET_FTP_OK,
    RET_JD_OK,
    
    ERR_BT_SEND,        //������ �������� ������ �� BT
    RET_BT_WAIT_DATA,   //������ �� BT ������
    RET_BT_TIMEOUT,     //����� ������� ���������� �� BT
    RET_BT_DISCONN,     //���������� BT ���������

    RET_CLOSE0,
    RET_CLOSE1,
    
    RET_HTTP_OK,
    RET_HOME_NET = 0x31,  
    RET_ROAMING_NET = 0x35,
    RET_HTTP_SERV,
}RET_INFO;


typedef enum GSM_COMMAND {
    GSM_PWRON  = 0,
    GSM_PWROFF = 1,
    GSM_SLEEP  = 2,
    GSM_WAKEUP = 3,
    GSM_LOCK   = 4
} GSM_COMMAND;


int mc_get(const char *pCmd, u8 m_type, GSM_INFO *pOut, u8 count, u32 second);
void mc_send(const char *pCmd, char *pPrm, uint32_t pause);
RET_INFO modem_on(void);
void modem_off(void);
RET_INFO isOK(const char *pCmd, uint32_t wait);
RET_INFO modem_setting_start(void);
RET_INFO modem_precmd(void);
RET_INFO modem_second_precmd(void);
RET_INFO mc(const char *pCmd, u32 second, int count);   // 1 - ��� �������, 2 - ������� �������, 3 - ���������� ������� ����� �������
uint32_t get_gsm_time(void);
void rtc_gsm_settime(const RTC_t *date);
void TimeSynchronizationGSM(void);

M_INFO SimCardPin(void);
M_INFO SimCardInit(void); 
    
uint64_t GetGsmIMEI(char * pIMEI);
int GetSimSCID(char *pSCID);
_Bool GsmModemConnectGprsService(void);         // 0-Attach, 1-Detach from GPRS Service.
RET_INFO WakeupGSM(_Bool State);
void SetupInternetConnectionProfile(const char *pUser, const char *pPasswd, const char *pApn, _Bool bTypeProf);
int ActivationConnectToGprs(u8 usProf);
int ServerConnect(char *pNameServ, u8 usProf);
RET_INFO modem_jamming_detected(u8 ucTimeFindJammingDetect);
void gsm_write(const char *pBuf, int size, uint32_t pause);
int gsm_gets(char *pBuf, int size, uint32_t wait);
int gsm_read(char *pBuf, int size, uint32_t wait);


#endif