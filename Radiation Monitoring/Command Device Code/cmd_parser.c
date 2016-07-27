#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "includes.h"
#include "cmd_parser.h"
#include "cmd_func.h"

#include "cmd_commands.h"


//����������� ���������� ������ (��������� �������� ��� ��� � �������)
const char cmd_delimiter[4][3] = {"\r\n", "\r\n", "\r\n", ";"};
//����������� �������� ������� �� �������� (���� �� ��������)
const char cmd_separator[] = {'='};//, '?'};
#define SET_VAL_SEPARATOR       0
#define GET_HELP_SEPARATOR      1
#define GET_HELP_SYMBOL         '?'

//��������� ������ ��� ������ �������� ����������
uint8_t val_buff[30];
uint8_t curr_cmd_pass = 0;
_Bool bUsbSessionPassOk = FALSE;
#define COMMANDS_NUM    (sizeof(cmd_array)/sizeof(CMD))

enum {
    SIZE_IMSI      = 24,
    SIZE_PORT      = 6,
    SIZE_SFTP      = 300,
    SIZE_TELNUMBER = 20,
    SIZE_PINCODE   =  9,
    SIZE_OW_ROM    =  8,     // ������ ��������� � ��������������� ������������
    SIZE_PASSWORD  = 16
};

typedef enum {
  CMD_NONE = 0,
  CMD_GET_HELP,         // ������ ������� �� �������
  CMD_HELP_PRM,         // ������ ������� �� ��������� �������
  CMD_HELP_ELEMENT,     // ������ ������� �� ��������� �������
  CMD_GET_PRM,          // ������ �������� ���������
  //CMD_GET_PRM_EXTENDED, // ������ �������� ��������� �� ������������� ��������� ����������
  CMD_GET_CONFIG,       // ������ ��� ��������� �������
  CMD_SET_PRM,          // ���������� �������� ���������
  CMD_SET_SOME_PRM,     // ���������� �������� ���������� ����������
  CMD_DEL_PRM,          // ������� ��������� ��������
  CMD_DEL_SOME_PRM      // ������� ������ ����������
}CMD_CODE;

typedef enum {
  PRM_NONE = 0,
  PRM_PORT,
  PRM_TYPE,
  PRM_PERIOD,
  PRM_FILTER,
  PRM_ADDRESS,
  PRM_CHANNEL,
  
  PRM_WHO_IS_HERE,
  PRM_RD_DATA_FROM_SENSOR,
  
  PRM_BITS,
  PRM_SPEED,
  PRM_PARITY,
  PRM_STOPBITS,
  
  PRM_SRV_ADDR,
  PRM_SRV_PORT,
  PRM_SRV_KEY,
  PRM_SET_GROUP,
  
  PRM_OFF,
  PRM_SIM,
  PRM_LOCK,
  PRM_INFO,
  
  PRM_MASK,
  PRM_OUTPUT,
  PRM_NUMBER,
  PRM_WRITE,
  PRM_READ,
  PRM_COMMAND_MODE = PRM_CHANNEL,
  PRM_PULL,
  
  PRM_FORMAT = PRM_FILTER,
  PRM_FREQUENCE = PRM_WHO_IS_HERE,
  PRM_STATUS = PRM_RD_DATA_FROM_SENSOR,
  
  IBUT_NUMBER_PIN = PRM_OUTPUT,
  IBUT_NUMBER_OF_PULSES = PRM_NUMBER,
  IBUT_DURATION = PRM_RD_DATA_FROM_SENSOR,
}PRM_CODE;

typedef struct {
  CMD_CODE code;
  PRM_CODE type;
  uint8_t size;
  uint8_t *data;
  int arg;
}USER_CMD;


uint8_t UpCaseChar(uint8_t ch)
{
  if ('a' <= ch && ch <= 'z')
    return ch-0x20;
  else if ('�' <= ch /*&& pstr[i] <= '�'*/)
    return ch-0x20;
  else
    return ch;
}

int findchar(char *buf, char bayt, int size)
{
    int n=0;
    
    while(n < size) {   
        if(buf[n] == bayt) return n;
        n++;
    }
    
    return -1;
}

int get_uint(uint32_t *out, uint8_t *buf, uint16_t len)
{
  int n=0;
  int res=0;
  
  if(buf > 0 && len > 0) {
    for(n=0; n<len; n++) {
      if(buf[n] < '0' || buf[n] > '9') break;
      res *= 10;
      res += (buf[n] - '0');
    }
    if(n > 0) *out = res;
  }
  
  return n;
}

int cmp_string(const char *str1, const char *str2, int len)
{
  int n;
  
  for(n=0; ;n++) {
    if(str1[n] == 0 || str2[n] == 0) return n;
    if(n >= len || (str1[n] != str2[n] && str1[n] != (str2[n] + 0x20))) break;
  }
  
  return 0;
}


int findstr2(const char *str, const char *buf, int size)
{
  int n=0,m=0;
  char b,s,f;
  
  f = *str;
  if(*str >= 'a' && *str <= 'z') f -= 0x20;
  s = f;
  
  while(n < size) {
    b = buf[n];
    if(buf[n] >= 'a' && buf[n] <= 'z') b -= 0x20;
    n++;
    
    if(b == s) {
      m++;
      s = str[m];
      if(str[m] >= 'a' && str[m] <= 'z') s -= 0x20;
      if(s == 0) return (n - m);
    } else {
      m=0;
      s=f;
    }
  }
  
  return -1;
}

int findstr(const char *str, char *buf, int size)
{
    int n=0,m=0;

    while(n < size) {
        if(buf[n] == str[m]) {
            m++;
        } else {
            m=0;
        }
       n++;

       if(str[m] == 0) {
            return n-m;
       }
    }
    
    return -1;
}

float fGetVal(uint8_t *ptr, uint8_t ucInd, uint8_t ucType)
{
  float ft;
  
  switch (ucType)
  {
    case TYPE_U8:
        ft = (float)(*((uint8_t*)ptr + ucInd));
        break;
    case TYPE_S8:
        ft = (float)(*((int8_t*)ptr + ucInd));
        break;
    case TYPE_U16:
        ft = (float)(*((uint16_t*)ptr + ucInd));
        break;
    case TYPE_S16:
        ft = (float)(*((int16_t*)ptr + ucInd));
        break;
    case TYPE_U32:
        ft = (float)(*((uint32_t*)ptr + ucInd));
        break;
    case TYPE_S32:
        ft = (float)(*((int32_t*)ptr + ucInd));
        break;
    case TYPE_FLOAT:
        ft = (float)(*((float*)ptr + ucInd));
        break;
  }
  
  return ft;
}

void SetVal(uint8_t *pdest, uint8_t ucInd, uint8_t ucType, float val)
{
  uint8_t len=0;
  uint8_t uc[4];
  uint8_t *pval = &uc[0];
  
  //_mutex_lock(&cfg_device.mutex);  
  switch (ucType)
  {
    case TYPE_U8:
      *(uint8_t*)pval = (uint8_t)val;
      len = 1;
      break;
    case TYPE_S8:
      *(int8_t*)pval = (int8_t)val;
      len = 1;
      break;
    case TYPE_U16:
      *(uint16_t*)pval = (uint16_t)val;
      len = 2;
      break;
    case TYPE_S16:
      *(int16_t*)pval = (int16_t)val;
      len = 2;
      break;
    case TYPE_U32:
      *(uint32_t*)pval = (uint32_t)val;
      len = 4;
      break;
    case TYPE_S32:
      *(int32_t*)pval = (int32_t)val;
      len = 4;
      break;
    case TYPE_FLOAT:
      *(float*)pval = (float)val;
      len = 4;
      break;
  }
  memcpy(pdest + ucInd*len,pval,len);
}

uint8_t GetTypeSize(uint8_t uctype)
{
  switch (uctype)
  {
    case TYPE_U8:
      return 1;
    case TYPE_S8:
      return 1;
    case TYPE_U16:
      return 2;
    case TYPE_S16:
      return 2;
    case TYPE_U32:
      return 4;
    case TYPE_S32:
      return 4;
    case TYPE_FLOAT:
      return 4;
    default:
      return 0;
  }
}

int FindCommand(uint8_t *pstr, uint8_t cmd_len)
{
  uint16_t i;
  uint16_t len;
  
  if (!cmd_len) 
    return FAIL;
  
  for (i = 0; i < COMMANDS_NUM; i++)
  {
    len = strlen(cmd_array[i].ptrName);
    if (cmd_len != len) {
      if(cmd_len > len && (cmd_array[i].flags & USER_PROCESSING) != 0) {
        if(memcmp(cmd_array[i].ptrName, pstr, len) == 0) {
          return i;
        }
      }
      continue;
    }
      len = 0;
      while (cmd_array[i].ptrName[len] == pstr[len])
        len++;
            
      //����� ���� �������    
      if (len == cmd_len)
        return i;
  }
  
  return FAIL;
}


int parse_cmd(uint8_t *pcmd, uint8_t *pansw, uint8_t uc_interf, uint16_t cmd_sz, uint16_t answ_sz)
{
  uint8_t stop_converting=0;
  uint8_t *buff_start = pansw;
  uint16_t i,n;
  uint8_t ucPassSt=0;
  uint16_t buf_sz;
  uint16_t answ_len;
  uint8_t uc_delim_len = strlen(cmd_delimiter[uc_interf]);
  int ret;
  
  s16 cmd_start;     //������ ������� � ������
  s16 cmd_stop;      //�����. ����� �������
  s16 cmd_indx;      //����� �������
  s16 cmd_val;       //�����. ������ ���������� ���������� � �������
  s8  cmd_type;      //��� ������� ������ ���� ������
#define GET_VAL_CMD     0
#define SET_VAL_CMD     1
#define USER_FUNC_CMD   2
#define GET_HELP_CMD    3
  
//��������� ��� �������� ������ ���������� ������
#define CMD_MAX_ANSW_LEN            20
#define CMD_MAX_ANSW_LEN_ECHO       30
  
    //���� �� ������ ������� ��� �������, ��������� �� � ��������
  if (!cmd_sz) {
    cmd_sz = 512;
  }
  if (!answ_sz) {
    answ_sz = ANS_BUFF_SIZE;
  }
  
  //Delete spaces.
  i = 0; n = 0;
  while(pcmd[i] != '\0')
  {
     if(pcmd[i] == 0x20) 
     {
       n = i;
       while(pcmd[n] != '\0'){
          pcmd[n] = pcmd[n+1];
          n++;
          if(pcmd[n+1] == '\0') break;
       }
     }
       
    i++; 
  }
  
  //����������� ��� � ��������� ��������
  for (i=0,n=0; i < cmd_sz && pcmd[i] != '\0'; i++)
  {
    if (i >= cmd_sz)
      return FAIL;
    
    if(pcmd[i] == '\r') {
       pcmd[i+1] = '\n';
    }
    
    if(pcmd[i] == '\r' || pcmd[i] == '\n' || pcmd[i] == ';') {
      if(uc_interf == 0) {
        if(pcmd[i] == '\n') {
          stop_converting = 0;
        }
        pcmd[n++] = pcmd[i];
      } else if(pcmd[i] == cmd_delimiter[uc_interf][0]) {
        stop_converting = 0;
        pcmd[n++] = pcmd[i];
      }
    } else if(pcmd[i] == 8) {
      if(n > 0) n--;
    } else if (stop_converting == 0) {
      if('a' <= pcmd[i] && pcmd[i] <= 'z') {
        pcmd[n++] = pcmd[i] - 0x20;
      } else {
        if('=' == pcmd[i])
          stop_converting = 1;
        pcmd[n++] = pcmd[i];
      }
    } else {
      pcmd[n++] = pcmd[i];
    }
  }
  //���������� ������ �������
  buf_sz = n+1;
  pcmd[n+1] = 0;
  cmd_stop = 0;
  cmd_start = 0;
  
  //�������� ������
  
  if(1) {
    if(uc_interf == INTERF_SMS) {
      // �������� ������� ����� ���, ������ �� �����, ������� ��� ������
      return 0;
    }
    ucPassSt = 1;
  } else if(uc_interf == INTERF_COM_USB) {
    ucPassSt = bUsbSessionPassOk;
  } 
  else if(uc_interf == INTERF_SMS) {
    ret = findstr("PASS=", (char*)pcmd, buf_sz);
    if(ret < 0) {
      // �������� ������� ����� ���, ������ �����������, ������� ��� ������
      return 0;
    }
    cmd_stop = ret;
    cmd_start = ret;
  } 
  else {
      ucPassSt = 0;
  }
  
   if(uc_interf == INTERF_SRV) {
      ucPassSt = 1;
   }
  
  //������� ������������ �������
  for (; cmd_start < buf_sz; cmd_start += cmd_stop + uc_delim_len)
  {
    //����� ����� �������, ���� ������ ������ ��� � �������
    if ((cmd_stop = findstr(cmd_delimiter[uc_interf], (char*)&pcmd[cmd_start], buf_sz - cmd_start)) == -1) // ����� cmd_start �� ������, ��������� �� ������ ���������
      break;
    //���� �������� ���������� � �������
    for (i = 0; i < sizeof(cmd_separator); i++) {
      if ((cmd_val = findchar((char*)&pcmd[cmd_start], cmd_separator[i], cmd_stop)) != -1)
      { //�����, ��������� �� ��������
        cmd_val += 1;
        break;
      }
      cmd_val = cmd_stop + 1;
    }
    
    //������� ������ �������
    cmd_indx = FindCommand(&pcmd[cmd_start], cmd_val - 1);
    
    //���������� ��� ������� - ������, ������, ����������������, �������� ������
    if (cmd_array[cmd_indx].pUserParse != NULL) 
      cmd_type = USER_FUNC_CMD;
    else if (i == SET_VAL_SEPARATOR && (pcmd[cmd_start + cmd_val] != GET_HELP_SYMBOL || (cmd_stop - cmd_val) != 1))
      cmd_type = SET_VAL_CMD;
    else if (i == GET_HELP_SEPARATOR)
      cmd_type = GET_HELP_CMD;
    else
      cmd_type = GET_VAL_CMD;
    
    answ_len = pansw - buff_start;
    //�������� ���� �� ����� � ������ ��� ������
    if ((cmd_array[cmd_indx].flags & ECHO) && (answ_sz < answ_len + CMD_MAX_ANSW_LEN_ECHO))
    {
      return answ_len;
    }
    else if (answ_sz < answ_len + CMD_MAX_ANSW_LEN)
    {
      return answ_len;
    }  
    //���� ����� �������, ����� ���� �� ���������
    if (cmd_indx != FAIL)
    {
      uint32_t flags = cmd_array[cmd_indx].flags;
      uint16_t num_type = flags & NUM_MASK;
      uint8_t  val_sz;
      double val;
      s8 error = 0;
      uint8_t *psrc;
      uint8_t *pdest;
      
      //���� ��� ���� �������
      if (flags & ECHO)
      {
        memcpy(pansw ,&pcmd[cmd_start], cmd_stop);
        pansw += cmd_stop;
        pansw += sprintf((char*)pansw,"\r\n");
      }
      
      if (uc_interf == INTERF_SMS && (flags & SMS_ACCESS_DIS) || (flags & NOT_SUPPORTED_CMD))
      { //��� ������� �� ��� ��� ������ ��������� ���������
        error = CMD_ERR_ACCESS;
      }
      else
      {
        curr_cmd_pass = 1;
        uint8_t ucAccessLevel = 0;
        switch(ucAccessLevel) {
          case 0:
            // ������ ������ ��� ������ ������� �������
            if (ucPassSt == 0 && cmd_type == SET_VAL_CMD && (flags & PASS_LVL_0) != 0)
              curr_cmd_pass = 0;
            break;
          case 1:
            // ������ ������ ��� ������ IP ����� �������, ������� ������� � ������
            if (ucPassSt == 0 && (cmd_type == SET_VAL_CMD || cmd_type == USER_FUNC_CMD) && (flags & PASS_LVL_1) != 0)
              curr_cmd_pass = 0;
            break;
          case 2:
            // ������ ������ ����������, ��� ��������� ����� ������
            if(ucPassSt == 0 && (cmd_type == SET_VAL_CMD || cmd_type == USER_FUNC_CMD))
              curr_cmd_pass = 0;
            break;
        }
        
        if( (curr_cmd_pass) && (uc_interf != INTERF_COM_USB) && (uc_interf != INTERF_SMS) ) {
           bUsbSessionPassOk = TRUE;
        }
        
        //�������� ���������������� ������� ������������ ������
        if (cmd_type == USER_FUNC_CMD)
        {
          if((flags & USER_PROCESSING) != 0) {
            cmd_val = strlen(cmd_array[cmd_indx].ptrName);
          }
          
          int len = cmd_stop - cmd_val > 0 ? cmd_stop - cmd_val : 0; 
          int rez = cmd_array[cmd_indx].pUserParse(pansw, &pcmd[cmd_start + cmd_val], len);
          
          char *pFindNULL = strstr((char *)pansw,"=\r\n");
          if(pFindNULL>0) {
            strcpy(pFindNULL, "=NULL\r\n");
            rez = strlen((char *)pansw);
          }
         
          if (rez >= 0) {
            pansw += rez;
          } else {
            error = rez;
            if(error == CMD_ERR_WRNG) {
              pansw = buff_start;
            }
          }
        } else {
          switch(cmd_type) {
            case GET_HELP_CMD:
              //������� ������ �� �������
              pansw += sprintf((char*)pansw, "Usage: %s\r\n", cmd_array[cmd_indx].ptrHelp);
              break;
              
            case SET_VAL_CMD:
              if ((flags & PASS_EN) && curr_cmd_pass == 0)
              { //������ �������, �� ����� ������
                error = CMD_ERR_PASS;
              }
              else 
              {
                if (flags & NUM_MASK)
                { //�������� � �������
                  uint8_t *str_end;
                  
                  val_sz = GetTypeSize(num_type);
                  val = strtold((const char*)&pcmd[cmd_start + cmd_val], (char**)&str_end);
                  if (flags & LIM)
                  {
                    if (&pcmd[cmd_start + cmd_stop] == str_end && cmd_array[cmd_indx].limits[0] <= val && val <= cmd_array[cmd_indx].limits[1])
                      SetVal(val_buff, 0, num_type, val);
                    else
                      error = CMD_ERR_LIM;
                  }
                  else
                    SetVal(val_buff, 0, num_type, val); 
                }
                else if (flags & TYPE_STRING)
                {
                  //�������� �� ��������
                  val_sz = cmd_stop - cmd_val;
                  psrc = &pcmd[cmd_start + cmd_val];
                  
                  if(findstr2("NULL", (char*)psrc, val_sz) != 0) {
                    if (val_sz > 0 && (val_sz + 1) <= cmd_array[cmd_indx].uclen)
                    {
                      pdest = val_buff;
                      //������ ��� ������� ��������, ������ ����� ������
                      for (i = 0; i < val_sz; i++)
                        *pdest++ = *psrc++;
                      *pdest = 0;
                    }
                    else
                    {
                      error = CMD_ERR_STR;
                    }
                  } else {
                    *val_buff = 0;
                  }
                }
              
                //��������� �������� � ������ ��� ���-�� ��� ��� :)
                if (error == 0)
                {
                  if (flags & CFG_VAL && (flags & RAM_VAL) == 0)
                  {
                    memcpy((void*)cmd_array[cmd_indx].adr, val_buff, cmd_array[cmd_indx].uclen);
                    //SaveConfigCMD();
                  }
                  else
                  {
                    memcpy((void*)cmd_array[cmd_indx].adr, val_buff, cmd_array[cmd_indx].uclen);
                  }
                }
              }
              break;
              
            case GET_VAL_CMD:
              psrc = (uint8_t*)cmd_array[cmd_indx].adr;
              
              if ((flags & READ_PASS) && curr_cmd_pass == 0)
              { //������ �������, �� ����� ������
                error = CMD_ERR_PASS;
              }
              else if (flags & READ_DIS && ((flags & TYPE_STRING) == 0 || *psrc != 0))
              { //��� ������� �� ��� ��� ������ ��������� ���������
                error = CMD_ERR_ACCESS;
              }
              else
              {
                //������� �������� ��������� � �����
                memcpy(pansw, &pcmd[cmd_start], cmd_val - 1);
                pansw += cmd_val - 1;
                
                *pansw++ = '=';
                if (flags & NUM_MASK)
                {
                  val = fGetVal(psrc, 0, num_type);
                  if (num_type & TYPE_FLOAT)
                  {
                    if(flags & OUT_U32)
                      pansw += sprintf((char*)pansw, "%d", (uint32_t)val);
                    else
                      pansw += sprintf((char*)pansw, "%.2f", val);
                  }
                  else 
                  {
                    if (num_type & (TYPE_S8|TYPE_S16|TYPE_S32))
                      pansw += sprintf((char*)pansw, "%d", (s32)val);
                    else
                      pansw += sprintf((char*)pansw, "%u", (uint32_t)val);
                  }
                }
                else if (flags & TYPE_STRING)
                {
                  int len;
                  
                  for(len = 0; *psrc != '\0' && len < cmd_array[cmd_indx].uclen; len++)
                  {
                    *pansw++ = *psrc++;
                  }
                  
                  if(len == 0) pansw += sprintf((char*)pansw, "NULL");
                }
                pansw += sprintf((char*)pansw, "\r\n");
              }          
              break;
          }
        }
      }
      
      //����� ������� ������������
      if (flags & FLAG_OK)
      {
        switch(error) {
          case CMD_PASS_OK:     pansw += sprintf((char*)pansw, "PASS OK\r\n"); ucPassSt = 1; bUsbSessionPassOk = TRUE; break;
          case 0:           pansw += sprintf((char*)pansw, "OK\r\n"); break;
          case CMD_ERR_LIM:     pansw += sprintf((char*)pansw, "ERR_LIM\r\n"); break;
          case CMD_ERR_STR:     pansw += sprintf((char*)pansw, "ERR_LEN\r\n"); break;
          case CMD_ERR_PASS:    if(uc_interf == INTERF_SMS && ucPassSt == 0) {
                              // �������� ������� ����� ���, ������ �� �����, ������� ��� ������
                              return 0;
                            }
                            pansw += sprintf((char*)pansw, "ERR_PASS\r\n"); break;
          case CMD_ERR_ACCESS:  pansw += sprintf((char*)pansw, "ERR_ACCESS\r\n"); break;
          case CMD_ERR_BUSY:    pansw += sprintf((char*)pansw, "ERR_BUSY\r\n"); break;
          case CMD_ERR_TIMEOUT: pansw += sprintf((char*)pansw, "ERR_TIMEOUT\r\n"); break;
          case CMD_ERR_EXISTS:  pansw += sprintf((char*)pansw, "ERR_NOT_EXISTS\r\n"); break;
          case CMD_ERR_UNKNOW:  pansw += sprintf((char*)pansw, "ERR_UNKNOW\r\n"); break;
          case CMD_ERR_MEM:     pansw += sprintf((char*)pansw, "ERR_MEM\r\n"); break;
          case CMD_ERR_CONTINUE: break;
          case CMD_ERR_WRNG:
            //���� ��� ���� �������
            if (flags & ECHO && uc_interf != INTERF_SMS) {
              memcpy(pansw ,&pcmd[cmd_start], cmd_stop);
              pansw += cmd_stop;
              pansw += sprintf((char*)pansw,"\r\n");
              pansw += sprintf((char*)pansw, "WRNG CMD\r\n");
            }
            break;
          case CMD_NO_ANS:              //������ ���� ���
            break;
          default:         pansw += sprintf((char*)pansw, "ERROR\r\n"); break;
        }
      }
      
    }//if (cmd_indx != FAIL)
    else
    {
      //�� ����� ����� �������
      if (cmd_val > 1) {
        //���� ��� ���� �������
        if (uc_interf != INTERF_SMS) {
          memcpy(pansw ,&pcmd[cmd_start], cmd_stop);
          pansw += cmd_stop;
          pansw += sprintf((char*)pansw,"\r\n");
        }
        pansw += sprintf((char*)pansw,"WRNG CMD\r\n");
      } else {
        pansw += sprintf((char*)pansw,"NULL CMD\r\n");
      }
    }
    
    if((pansw - buff_start) >= ANS_BUFF_SIZE) {
      pansw += sprintf((char*)pansw,"ERR_BUFOVERFLOW\r\n");
      break;
    }
  }//for (cmd_start = 0, cmd_stop = 0; cmd_stop < buf_sz; cmd_start = cmd_stop + CMD_DELIMITER_SZ)
  
  if((pansw - buff_start) == 0) {
    pansw += sprintf((char*)pansw,"WRNG CMD\r\n");
  }
  			  
  return pansw - buff_start;
}

int parsing_command_line(USER_CMD *cmd, uint8_t *parg, uint16_t len)
{
  int size;
  int ret=1;
  int j,n=0,step=0;
  
  do {
    switch(step) {
      case 0:
        switch(parg[n]) {
          case '?':
            if(parg[n] == '\r' || parg[n] == '\n') {
              cmd->code = CMD_GET_HELP;
            } else {
              ret = CMD_ERR_CMD;
              break;
            }
          case '#':
            n++;
            step++;
            break;
          case '=':
            n++;
            step=5;
            break;
          default:
            ret = CMD_ERR_WRNG;
            break;
        }
        break;
      
      case 1:
        j = get_uint((uint32_t*)&cmd->arg, parg+n, 3);
        if(j > 0) {
          n += j;
          step++;
        } else {
          step = 4;
        }
        break;
      
      case 2:
        switch(parg[n]) {
          case '?':
            if(parg[n] == '\r' || parg[n] == '\n') {
              cmd->code = CMD_HELP_ELEMENT;
            } else {
              ret = CMD_ERR_CMD;
              break;
            }
          case '#':
            step++;
          case '=':
            n++;
            step++;
            break;
          default:
            ret = CMD_ERR_CMD;
            break;
        }
        break;
      
      case 3:
        if(parg[n] == '?') {
          n++;
          if(parg[n] == '\r' || parg[n] == '\n') {
            cmd->code = CMD_GET_CONFIG;
          } else {
            ret = CMD_ERR_CMD;
          }
        } else {
          size = len - n;
          if(size != 4 || cmp_string((char*)(parg+n), "NULL", 4) != 4) {
            cmd->size = size;
            cmd->data = parg+n;
            cmd->code = CMD_SET_SOME_PRM;
            cmd->type = PRM_SET_GROUP;
            n=len;
          } else {
            n += 4;
            cmd->code = CMD_DEL_SOME_PRM;
          }
        }
        break;
        
      case 4:
        for(j=n+1,size=0; j<len; j++) {
          if(parg[j] == '=') {
            size = j-n;
            break;
          }
        }
        if(size > 0) {
          switch(size) {
            case 1:
              j = 1;
              switch(parg[n]) {
                case 'D': cmd->type = PRM_RD_DATA_FROM_SENSOR; break;
                case 'P': cmd->type = PRM_WHO_IS_HERE; break;
                case 'F': cmd->type = PRM_FILTER; break;
                case 'A': cmd->type = PRM_ADDRESS; break;
                case 'I': cmd->type = PRM_PORT; break;
                case 'T': cmd->type = PRM_PERIOD; break;
                case 'M': cmd->type = PRM_MASK; break;
                case 'O': cmd->type = PRM_OUTPUT; break;
                case 'N': cmd->type = PRM_NUMBER; break;
                case 'W': cmd->type = PRM_WRITE; break;
                case 'R': cmd->type = PRM_READ; break;
                case 'C': cmd->type = PRM_COMMAND_MODE; break;
                default: j = 0;
              }
              break;
            case 2:
              j = cmp_string((char*)(parg+n), "TP", 2);
              cmd->type = PRM_TYPE;
              break;
            case 3:
              j = cmp_string((char*)(parg+n), "SIM", 3);
              cmd->type = PRM_SIM;
              if(j > 0) break;
              j = cmp_string((char*)(parg+n), "KEY", 3);
              cmd->type = PRM_SRV_KEY;
              if(j > 0) break;
              j = cmp_string((char*)(parg+n), "OFF", 3);
              cmd->type = PRM_OFF;
              break;
            case 4:
              j = cmp_string((char*)(parg+n), "ADDR", 4);
              cmd->type = PRM_SRV_ADDR;
              if(j > 0) break;
              j = cmp_string((char*)(parg+n), "PORT", 4);
              cmd->type = PRM_SRV_PORT;
              if(j > 0) break;
              j = cmp_string((char*)(parg+n), "BITS", 4);
              cmd->type = PRM_BITS;
              if(j > 0) break;
              j = cmp_string((char*)(parg+n), "LOCK", 4);
              cmd->type = PRM_LOCK;
              if(j > 0) break;
              j = cmp_string((char*)(parg+n), "INFO", 4);
              cmd->type = PRM_INFO;
              if(j > 0) break;
              j = cmp_string((char*)(parg+n), "PULL", 4);
              cmd->type = PRM_PULL;
              break;
            case 5:
              j = cmp_string((char*)(parg+n), "SPEED", 5);
              cmd->type = PRM_SPEED;
              break;
            case 6:
              j = cmp_string((char*)(parg+n), "PARITY", 6);
              cmd->type = PRM_PARITY;
              break;
            case 8:
              j = cmp_string((char*)(parg+n), "STOPBITS", 8);
              cmd->type = PRM_STOPBITS;
              break;
            default:
              j = 0;
          }
          
          if(j == size) {
            n += size+1;
            step++;
          } else {
            ret = CMD_ERR_CMD;
            cmd->type = PRM_NONE;
          }
        } else {
          ret = CMD_ERR_CMD;
        }
        break;
        
      case 5:
        if(parg[n] == '?') {
          n++;
          if(parg[n] == '\r' || parg[n] == '\n') {
            cmd->code = CMD_GET_PRM;
          } else {
            ret = CMD_ERR_CMD;
          }
        } else {
          if(cmp_string((char*)(parg+n), "NULL", 4) == 4 && (n+4) == len) {
            n += 4;
            cmd->code = CMD_DEL_PRM;
          } else {
            cmd->size = len - n;
            cmd->data = parg + n;
            if(cmd->type == PRM_RD_DATA_FROM_SENSOR) {
              //cmd->code = CMD_GET_PRM_EXTENDED;
              ret = CMD_ERR_CMD;
            } else {
              cmd->code = CMD_SET_PRM;
            }
            n = len;
          }
        }
        break;
    }
  } while(cmd->code == CMD_NONE && n < len && ret > 0);
  
  return ret;
}
