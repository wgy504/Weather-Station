
#ifndef HTTP_MENU_H_
#define HTTP_MENU_H_

#include "includes.h"

typedef enum {
  BAROMETR              = 0,
  TEMPERATURE_INT       = 1,
  HUMIDITY              = 2,
  CHARGER               = 3,
  TEMPERATURE_EXT       = 4,
  FIRST_MENU            = 5,
  NO                    = 0xFF,
}E_HTTP_GET;

int FromFirstWebGuiInterface(char *p_strMsgWeb);
int FromSecondWebGuiInterface(char *p_strMsgWeb, E_HTTP_GET cRet);
E_HTTP_GET HttpGetParser(char *ptEspRet);

#endif