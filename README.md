# Communication Protocol

## 1. Device to Listener

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

**Response Timeout:** 30000 ms


&nbsp;
### 1.2. Ping

**Request:** 19 Bytes
```
Byte 1 - Startbyte
    Fixed value 0xA1

Byte 2 - Working mode
    Reserved, value doesn't matter

Byte 3:4 - Firmware version

Byte 5 - SIM info
    fixme

Byte 6 - SIM 1 connection quality
    Connection quality in DBM for SIM 1

Byte 7 - SIM 2 connection quality
    Connection quality in DBM for SIM 2

Byte 8:9 - Battery voltage
    Battery voltage in 0.1V

Byte 10:13 - NFC list update time
    NFC list update time (UNIX time)

Byte 14:17 - PIN list update time
    PIN list update time (UNIX time)

Byte 18:19 - Checksum
    2 bytes sum of 17 previous bytes
```

**Response:** 1 Byte
```
Byte 1 - Status
    Fixed value 0x01 (OK status)
```

**Response Timeout:** 30000 ms


&nbsp;
### 1.3. Event (history)

**Request:** 16 Bytes
```
Byte 1 - Startbyte
    Fixed value 0xA2
    
Byte 2:5 - Event time
    Event time (UNIX time)

Byte 6 - Event type
    0x00 for NFC code
    0x01 for PIN code

Byte 7:14 - Value
    NFC of PIN code

Byte 15:16 - Checksum
    2 bytes sum of 14 previous bytes
```

**Response:** 1 Byte
```
Byte 1 - Status
    0x01 when OK
    0x00 when error
```

**Response Timeout:** 30000 ms


&nbsp;
## 2. Listener to Device

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

Byte 9:10 - Checksum
    2 bytes sum of 8 previous bytes
```

**Response:** 1 Byte
```
Byte 1 - Status
    0x01 when OK and ready to receive data
    0x00 when error
```

**Response Timeout:** 30000 ms

&nbsp;
### 2.2. Data packet

**Request:** 83 Bytes
```
Byte 1 - Startbyte
    Fixed value 0xB2

Byte 2:81 - Data
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

**Response Timeout:** 30000 ms


&nbsp;
## Startbytes list

| Value | Meaning            |
| ----- | ------------------ |
| 0xFE  | Handshake          |
| 0xA1  | Ping               |
| 0xA2  | Event (history)    |
| 0xB1  | Request for update |
| 0xB2  | Data packet        |
