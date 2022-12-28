# Device-Listener communication

## Table of contents
- [Device-Listener communication](#device-listener-communication)
  - [Table of contents](#table-of-contents)
  - [1. Requests from device to listener](#1-requests-from-device-to-listener)
    - [1.1. Handshake](#11-handshake)
    - [1.2. Ping](#12-ping)
    - [1.3. Event (history)](#13-event-history)
  - [2. Requests from listener to device](#2-requests-from-listener-to-device)
    - [2.1. Request for update](#21-request-for-update)
    - [2.2. Data packet](#22-data-packet)
  - [3. Startbytes list](#3-startbytes-list)
  - [4. Code examples](#4-code-examples)
    - [4.1. Data sending example](#41-data-sending-example)
    - [4.2. Data receiving example](#42-data-receiving-example)


---
## 1. Requests from device to listener

### 1.1. Handshake

**Request:** 16 Bytes
```
Byte 1 - Startbyte
    Fixed value 0xFE

Bytes 2:16 - Device IMEI
```

**Response:** 1 Byte
```
Byte 1 - Status
    0x01 when OK
    0x00 when error
```

**Response Timeout:** 30s


### 1.2. Ping

**Request:** 19 Bytes
```
Byte 1 - Startbyte
    Fixed value 0xA1

Byte 2 - Working mode
    Reserved, value doesn't matter

Bytes 3:4 - Firmware version

Byte 5 - SIM info
    fixme

Byte 6 - SIM 1 connection quality
    Connection quality in DBM for SIM 1

Byte 7 - SIM 2 connection quality
    Connection quality in DBM for SIM 2

Bytes 8:9 - Battery voltage
    Battery voltage in 0.1V

Bytes 10:13 - NFC list update time
    NFC list update time (UNIX time)

Bytes 14:17 - PIN list update time
    PIN list update time (UNIX time)

Bytes 18:19 - Checksum
    2 bytes sum of 17 previous bytes
```

**Response:** 1 Byte
```
Byte 1 - Status
    Fixed value 0x01 (OK status)
```

**Response Timeout:** 30s


### 1.3. Event (history)

**Request:** 16 Bytes
```
Byte 1 - Startbyte
    Fixed value 0xA2
    
Bytes 2:5 - Event time
    Event time (UNIX time)

Byte 6 - Event type
    0x00 for NFC code
    0x01 for PIN code

Bytes 7:14 - Value
    NFC of PIN code

Bytes 15:16 - Checksum
    2 bytes sum of 14 previous bytes
```

**Response:** 1 Byte
```
Byte 1 - Status
    0x01 when OK
    0x00 when error
```

**Response Timeout:** 30s


---
## 2. Requests from listener to device

### 2.1. Request for update

**Request:** 10 Bytes
```
Byte 1 - Startbyte
    Fixed value 0xB1

Byte 2 - Data type
    0x00 for NFC code
    0x01 for PIN code

Bytes 3:6 - Data time
    Data time (UNIX time)

Bytes 7:8 - Data count
    Count of data (In one packet, the amount of data is 20 (80 bytes))

Bytes 9:10 - Checksum
    2 bytes sum of 8 previous bytes
```

**Response:** 1 Byte
```
Byte 1 - Status
    0x01 when OK and ready to receive data
    0x00 when error
```

**Response Timeout:** 30s


### 2.2. Data packet

**Request:** 83 Bytes
```
Byte 1 - Startbyte
    Fixed value 0xB2

Bytes 2:81 - Data
    20x8 bytes per data unit

Bytes 82:83 - Checksum
    2 bytes sum of 81 previous bytes
```

**Response:** 1 Byte
```
Byte 1 - Status
    0x01 when OK
    0x00 when error
```

**Response Timeout:** 30s


---
## 3. Startbytes list

| Value | Meaning            |
| ----- | ------------------ |
| 0xFE  | Handshake          |
| 0xA1  | Ping               |
| 0xA2  | Event (history)    |
| 0xB1  | Request for update |
| 0xB2  | Data packet        |


---
## 4. Code examples

### 4.1. Data sending example

```C
typedef struct {
uint8_t  startbyte;
uint8_t  working_mode;
uint8_t  firmware_version;
uint8_t  SIM_info;
uint8_t  SIM1_connection_quality;
uint8_t  SIM2_connection_quality;
uint16_t battery_voltage;
uint32_t NFC_list_update_time;
uint32_t PIN_list_update_time;
uint16_t checksum;
} ping_data_typedef;


uint16_t GetChecksum(uint8_t *array, uint16_t array_length)
{
    if (array_length < 0) return 0;
    if (array_length < 2) return array[0];

    uint16_t ret_val = 0;
    uint16_t *arr_ptr = (uint16_t *)array;

    for (int i = 0; i < array_length / 2; i++)
    {
        retval += arr_ptr[i];
    }

    return = ret_val;
}


void PingDataStructToArray(ping_data_typedef *ping_data, uint8_t *array)
{
    array[0] = ping_data->startbyte;
    array[1] = ping_data->working_mode;

    array[2] = ping_data->firmware_version >> 8;
    array[3] = ping_data->firmware_version;
    
    array[4] = ping_data->SIM_info;
    array[5] = ping_data->SIM1_connection_quality;
    array[6] = ping_data->SIM2_connection_quality;

    array[7] = ping_data->battery_voltage >> 8;
    array[8] = ping_data->battery_voltage;

    array[9]  = ping_data->NFC_list_update_time >> 24;
    array[10] = ping_data->NFC_list_update_time >> 16;
    array[11] = ping_data->NFC_list_update_time >>  8;
    array[12] = ping_data->NFC_list_update_time;

    array[13] = ping_data->PIN_list_update_time >> 24;
    array[14] = ping_data->PIN_list_update_time >> 16;
    array[15] = ping_data->PIN_list_update_time >>  8;
    array[16] = ping_data->PIN_list_update_time;

    array[17] = ping_data->checksum >> 8;
    array[18] = ping_data->checksum;
}


int SendPingData(int socket, ping_data_typedef *ping_data)
{
    ping_data->startbyte = 0xA1;
    ping_data->working_mode = 0x01;
    ping_data->firmware_version = 0xA101;
    ping_data->SIM_info = 0x01;
    ping_data->SIM1_connection_quality = 120;
    ping_data->SIM2_connection_quality = 65;
    ping_data->battery_voltage = 1206;
    ping_data->NFC_list_update_time = 0xFA122112;
    ping_data->PIN_list_update_time = 0xFA122112;
    ping_data->checksum = GetChecksum((uint8_t *)ping_data, 17);

    return send(socket, (char *)ping_data, 19, NULL);
}
```


### 4.2. Data receiving example

```C
typedef struct {
uint8_t  startbyte;
uint8_t  working_mode;
uint8_t  firmware_version;
uint8_t  SIM_info;
uint8_t  SIM1_connection_quality;
uint8_t  SIM2_connection_quality;
uint16_t battery_voltage;
uint32_t NFC_list_update_time;
uint32_t PIN_list_update_time;
uint16_t checksum;
} ping_data_typedef;


uint16_t GetChecksum(uint8_t *array, uint16_t array_length)
{
    if (array_length < 0) return 0;
    if (array_length < 2) return array[0];

    uint16_t ret_val = 0;
    uint16_t *arr_ptr = (uint16_t *)array;

    for (int i = 0; i < array_length / 2; i++)
    {
        retval += arr_ptr[i];
    }

    return = ret_val;
}


void ArrayToPingDataStruct(uint8_t *array, ping_data_typedef *ping_data)
{
    ping_data->startbyte = array[0];
    ping_data->working_mode = array[1];
    ping_data->firmware_version = (array[2] << 8) | array[3];
    
    ping_data->SIM_info = array[4];
    ping_data->SIM1_connection_quality = array[5];
    ping_data->SIM2_connection_quality = array[6];

    ping_data->battery_voltage = (array[7] << 8) | array[8];

    ping_data->NFC_list_update_time  = (array[9]  << 24) 
                                     | (array[10] << 16)
                                     | (array[11] << 8)
                                     | (array[12]);

    ping_data->PIN_list_update_time  = (array[13] << 24) 
                                     | (array[14] << 16)
                                     | (array[15] << 8)
                                     | (array[16]);

    ping_data->checksum = (array[17] << 8) | array[18];
}


int ReadPingData(int socket, ping_data_typedef *ping_data)
{
    read(socket, (char *)ping_data, 19);

    uint16_t temp_checksum = GetChecksum((uint8_t *)ping_data, 17);

    return ping_data->checksum == temp_checksum;
}
```
