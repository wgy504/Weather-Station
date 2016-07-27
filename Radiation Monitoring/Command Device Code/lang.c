#include "includes.h"
#include "lang.h"

const char * const l_mems[LANG_NUM][1] = {
    "Sensor",
    "������",
};

const char * const l_mems_tp[LANG_NUM][7] = {
  {
    "(off)",
    "(move)",
    "(keep)",
    "(move+k)",
    "(turn)",
    "(shock)",
    "(crash)" 
  },
  {
    "(��������)",
    "(��������)",
    "(���������)",
    "(����+�����)",
    "(���������)",
    "(����)",
    "(������)"
  }
};

const char * const l_w_mode[LANG_NUM][2] = {
  {
    "Mode: sleep",
    "Mode: online"
  },
  {
    "�����: ������",
    "�����: ������"
  }
};

const char * const l_ext_power[LANG_NUM][2] = {
  {
    "External power: off",
    "External power: on"
  },
  {
    "�������: ����.",
    "�������: ���."
  }
};

const char * const l_gps[LANG_NUM][7] = {
  {
    "Sat:",
    "at",
    "s.",
    "Speed: ",
    "km/h",
    "Altitude:",
    "m" 
  },
  {
    "�����.:",
    "��",
    "�.",
    "��������:",
    "��/�",
    "������:",
    "�"
  }
};

const char * const l_vbat[LANG_NUM][4] = {
  {
    "Bat.:",
    "V",
    "s.",
    "Warning: low power bat!"
  },
  {
    "���.:",
    "�",
    "�.",
    "��������! ������ ����� �������!"
  }
};

const char * const l_misc[LANG_NUM][13] = {
  {
    "sms#",
    "LBS search error\n",
    "empty",
    "Command fail. GPS search time 0 min. Set GPS search time",
    "Command OK. Wait.",
    "No GPS signal",
    "Lang.: eng",
    "Name",
    "Statistics reset",
    "Network busy",
    "Attention!Somebody breaking password.\n",
    "Incorrect SMS-command.\n",
    "Incorrect SMS-command.\n"
  },
  {
    "���#",
    "������ ������ LBS",
    "�� �����",
    "������� �� ���������, ������ �������� ����� ������ GPS.",
    "������� �������. �����",
    "��� ������� GPS",
    "����: �������",
    "��� ����.",
    "���������� ��������",
    "���� ������",
    "��������! ������� ������� ������.\n",
    "������������ ������ ���-�������.\n",
    "������������ ������ ���-�������.\n"
  }
};

const char * const l_event[LANG_NUM][2] = {
  {
    "Alarm! Sensor: move",
    "Alarm! Button press"
  },
  {
    "�������! ������:��������",
    "�������! ������ ������"
  }
};

const char * const l_bbox[LANG_NUM][4] = {
{
       "�Black box�:\ndisable.\nFLASH: ",
       
       "�Black box�:\nenable when move.\nFLASH: ",
         
       "�Black box�:\nenable.\nFLASH: ",
       
       "Recording period in a �black box�(s.): ",
},
{
       "������� ����:\n��������.\nFLASH: ",
       
       "������� ����:\n������� ��� ��������.\nFLASH: ",
       
       "������� ����:\n�������.\nFLASH: ",
       
       "������ ������ �\n������� ����(c.): ",
    }
 };

  