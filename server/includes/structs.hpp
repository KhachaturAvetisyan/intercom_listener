#ifndef STRUCTS_HEADER
#define STRUCTS_HEADER

#include "serv_include.hpp"

// structs
typedef struct
{
  uint8_t  startbyte;
  uint8_t  working_mode;
  uint16_t firmware_version;
  uint8_t  SIM_info;
  uint8_t  SIM1_connection_quality;
  uint8_t  SIM2_connection_quality;
  uint16_t battery_voltage;
  uint32_t NFC_list_update_time;
  uint32_t PIN_list_update_time;
  uint16_t checksum;
}ping_struct;

typedef struct
{
  uint32_t time;
  uint8_t type; // 0x00 - NFC & 0x01 - PIN
  uint64_t value;
  uint16_t crc;
}history_struct;

typedef struct
{
  uint8_t  startbyte; //  0XB1
  uint8_t  datatype;  //  NFC[0X00] | PIN[0X01]
  uint32_t data_time; //  1672040924
  uint64_t data_count;//  15
  uint16_t checksum;  //  sizeof(struct X) - sizeof(uint16_t) | 
}upd_request; 

#endif // STRUCTS_HEADER