#ifndef _HLK_LD2450_
#define _HLK_LD2450_

#define HLK_LD2450_BAUD 256000

#define ld2450_begin_data_len 4
#define ld2450_end_data_len 2

#define ld2450_begin_control_len 4
#define ld2450_end_control_len 4

#define ld2450_target_len 8

#define ld2450_all_target_len 30 // 4+8*3+2

#include <Arduino.h>

class HLK_LD2450
{

public:
  HLK_LD2450(uint8_t rx, uint8_t tx, HardwareSerial *hsUart);
  void beginGetData();

  void processTarget();
  bool enableCFG();
  bool disableCFG();

  int16_t getTargetX(int index);
  int16_t getTargetY(int index);
  int16_t getSpeed(int index);
  uint16_t getDistanceResolution(int index);
  bool getFirmVersion();
  int findFirstDifference(const void *ptr1, const void *ptr2, size_t size);
private:
  bool checkFrameData(byte *frameData);
  int16_t decodeCoordinate(byte lowByte, byte highByte);
  int16_t decodeSpeed(byte lowByte, byte highByte);
  byte *readFrameData();
  HardwareSerial *HS;

  uint16_t rx_pin;
  uint16_t tx_pin;

  const byte begin_bytes_len = 4;
  const byte end_bytes_len = 2;
  byte begin_data_bytes[ld2450_begin_data_len] = {0xAA, 0xFF, 0x03, 0x00};
  byte end_data_bytes[ld2450_end_data_len] = {0x55, 0xCC};

  byte begin_control_bytes[ld2450_begin_control_len] = {0xFD, 0xFC, 0xFB, 0xFA};
  byte end_control_bytes[ld2450_end_control_len] = {0x04, 0X03, 0X02, 0X01};

  byte enable_cmd = 0xFF;
  byte enable_cmd_value = 0x01;
  byte enable_cmd_buffer = 0x40;


  struct ld2450_elem {
    int16_t target_x;
    int16_t target_y;
    int16_t speed;
    uint16_t distance_resolution;
  };

  struct ld2450_datas {
    struct ld2450_elem data[3];
  } ld2450_data;
};

#endif
