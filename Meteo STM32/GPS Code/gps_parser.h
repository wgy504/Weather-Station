#ifndef _GPS_PARSER
#define _GPS_PARSER

#include "includes.h"
#include "stdint.h"

enum {
    GPS_RMC   = 0,
    GPS_VTG   = 1,
    GPS_GGA   = 2,
    GPS_GLL   = 3,
    GPS_GSA   = 4,
    GPS_GSV   = 5,
    GPS_TOTAL = 6,
    GPS_START = 7
};

#define MASK_RMC    (1 << GPS_RMC)
#define MASK_GGA    (1 << GPS_GGA)


typedef __packed struct {
    // GPRMC
    uint32_t time;             // ���� & �����
    double latitude;           // ������
    double longitude;          // �������
    float    course;           // ����
    float    speed;            // ��������
        
    // GPGGA
    float hdop;                // hdop
    uint8_t  sat;              // satelites
    
    uint8_t  status;
    uint32_t cs_rmc;
    uint32_t cs_gga;
} GPS_INFO;

typedef __packed struct {
   double latitude;           // ������
   double longitude;          // �������
   float    course;           // ����
   float    speed;            // ��������
    
   float hdop;                // hdop
   uint8_t  sat;              // satelites
} GPS_INFO_NOT_VALID;


int gps_parser(GPS_INFO * const inf, unsigned char *buf, int size);
void gps_parser_sting(GPS_INFO * const inf, char *buf, int size);

#endif
