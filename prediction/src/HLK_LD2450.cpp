#include "HLK_LD2450.h"

HLK_LD2450::HLK_LD2450(uint8_t rx, uint8_t tx, HardwareSerial *hsUart)
{
  rx_pin = rx;
  tx_pin = tx;
  HS = hsUart;
  struct ld2450_datas ld2450_data;
  memset(&ld2450_data, 0, sizeof(ld2450_data));
  HS->begin(HLK_LD2450_BAUD, SERIAL_8N1, rx_pin, tx_pin);
 
}

byte *HLK_LD2450::readFrameData()
{
  uint8_t data = 0;
  byte *buffer = new byte[ld2450_all_target_len];

  while (data != begin_data_bytes[0])
  {
    if (HS->available() > 0)
      data = HS->read();
  }

  buffer[0] = data;
  HS->readBytes(buffer + 1, ld2450_all_target_len - 1);
  // Serial.write(buffer, ld2450_all_target_len);
  return buffer;
}

void HLK_LD2450::processTarget()
{
  byte *targetFrame = new byte[ld2450_all_target_len];

  byte *frameData = readFrameData();
  if (frameData)
  {
    memccpy(targetFrame, frameData, 0, ld2450_all_target_len);
    delete[] frameData;
  }
  // Serial.write(targetFrame, ld2450_all_target_len);
  if (checkFrameData(targetFrame))
  {
    uint8_t *newTargetFrame = &targetFrame[4];
    // Serial.write(targetFrame, ld2450_all_target_len);

    for (int i = 0; i < 3; i++)
    { // for each target
      ld2450_data.data[i].target_x = decodeCoordinate(newTargetFrame[i * 8], newTargetFrame[i * 8 + 1]);
      ld2450_data.data[i].target_y = decodeCoordinate(newTargetFrame[i * 8 + 2], newTargetFrame[i * 8 + 3]);
      ld2450_data.data[i].speed = decodeSpeed(newTargetFrame[i * 8 + 4], newTargetFrame[i * 8 + 5]);
      ld2450_data.data[i].distance_resolution = (newTargetFrame[i * 8 + 7] << 8) + newTargetFrame[i * 8 + 6];
    }
  }
}

int16_t HLK_LD2450::decodeCoordinate(byte lowByte, byte highByte)
{
  int16_t coordinate = ((highByte & 0x7F) << 8) + lowByte;
  if ((highByte & 0x80) == 0)
  {
    coordinate = -coordinate;
  }
  return coordinate;
}

int16_t HLK_LD2450::decodeSpeed(byte lowByte, byte highByte)
{
  int16_t speed = ((highByte & 0x7F) << 8) + lowByte;
  if ((highByte & 0x80) == 0)
  {
    speed = -speed;
  }
  return speed;
}

bool HLK_LD2450::checkFrameData(byte *frameData)
{

  if (memcmp(frameData, begin_data_bytes, ld2450_begin_data_len) != 0)
  {
    return false;
  }
  if (memcmp(frameData + ld2450_begin_data_len + ld2450_target_len * 3, end_data_bytes, ld2450_end_data_len) != 0)
  {
    return false;
  }
  return true;
}

bool HLK_LD2450::enableCFG()
{
  byte enable_frame[ld2450_begin_control_len + 6 + ld2450_end_control_len] = {0xFD, 0xFC, 0xFB, 0xFA, 0x04, 0x00, 0xFF, 0x00, 0x01, 0x00, 0x04, 0X03, 0X02, 0X01};
  // if (HS->available() > 0)
  // {
  //   HS->read();
  // }
  while (HS->availableForWrite() < (ld2450_begin_control_len + 6 + ld2450_end_control_len))
  {
    Serial.println("wait for enableCFG");
    delay(100);
  }
  HS->write(enable_frame, ld2450_begin_control_len + 6 + ld2450_end_control_len);
  // HS->write(begin_control_bytes, ld2450_begin_control_len);
  // HS->write(4 << 8);
  // HS->write(enable_cmd << 8);
  // HS->write(enable_cmd_value << 8);
  // HS->write(end_control_bytes, ld2450_end_control_len);

  // receive data
  uint8_t data = 0;
  byte *buffer = new byte[18];
  Serial.println("Available Bytes:");
  int availableBytes = HS->available();
  Serial.println(availableBytes);

  if (availableBytes > 0)
    data = HS->read();
  if (data != begin_control_bytes[0])
  {
    return false;
  }
  else
  {
    buffer[0] = data;
    HS->readBytes(buffer + 1, 18 - 1);
    Serial.write(buffer, 18);

    byte returnEnableACK[18] = {
        0xFD, 0xFC, 0xFB, 0xFA,
        0x08, 0x00, 0xFF, 0x01,
        0x00, 0x00, 0X01, 0x00,
        0x40, 0x00,
        0x04, 0X03, 0X02, 0X01};

    if (memcmp(buffer, returnEnableACK, 18) == 0)
    {
      Serial.println("enableCFG success");
      delete[] buffer;
      return true;
    }
    Serial.println("enableCFG failed");
    delete[] buffer;
    return false;
  }
}

char* byte2char(byte* bytes)
{
  int len = sizeof(bytes);
  char* str = new char[len * 2 + 1];
  for (int i = 0; i < len; i++)
  {
    sprintf(str + i * 2, "%02X", bytes[i]);
  }
  str[len * 2] = '\0';
  return str;
}

int HLK_LD2450::findFirstDifference(const void *ptr1, const void *ptr2, size_t size)
{
  const unsigned char *p1 = (const unsigned char *)ptr1;
  const unsigned char *p2 = (const unsigned char *)ptr2;

  for (size_t i = 0; i < size; i++)
  {
    if (p1[i] != p2[i])
    {
      return i;
    }
  }

  return -1;
}
bool HLK_LD2450::disableCFG()
{
  byte disable_frame[ld2450_begin_control_len + 4 + ld2450_end_control_len] = {0xFD, 0xFC, 0xFB, 0xFA, 0x02, 0x00, 0xFE, 0x00, 0x04, 0X03, 0X02, 0X01};
  HS->write(disable_frame, ld2450_begin_control_len + 4 + ld2450_end_control_len);
  // HS->write(begin_control_bytes, ld2450_begin_control_len);
  // HS->write(4 << 8);
  // HS->write(enable_cmd << 8);
  // HS->write(enable_cmd_value << 8);
  // HS->write(end_control_bytes, ld2450_end_control_len);

  // receive data
  uint8_t data = 0;
  byte *buffer = new byte[ld2450_all_target_len];

  while (data != begin_control_bytes[0])
  {
    if (HS->available() > 0)
      data = HS->read();
  }

  buffer[0] = data;
  HS->readBytes(buffer + 1, 12 - 1);
  Serial.write(buffer, 17);

  byte returnEnableACK[14] = {
      0xFD, 0xFC, 0xFB, 0xFA,
      0x04, 0x00, 0xFE, 0x01,
      0x00, 0x00,
      0x04, 0X03, 0X02, 0X01};
  if (memcmp(buffer, returnEnableACK, sizeof(returnEnableACK)) == 0)
  {
    Serial.println("disableCFG success");
    return true;
  }
  Serial.println("disableCFG failed");
  return false;
}
bool HLK_LD2450::getFirmVersion()
{
  byte getFirmVersion[12] = {
      0xFD, 0xFC, 0xFB, 0xFA,
      0x02, 0x00,
      0xA0, 0x00,
      0x04, 0X03, 0X02, 0X01};

  while (HS->availableForWrite() < 12)
  {
    Serial.println("wait for getFirmVersion");
    delay(100);
  }
  HS->write(getFirmVersion, 12);
  Serial.println("getFirmVersion sent");

  uint8_t data = 0;
  byte *buffer = new byte[22];

  // while (data != begin_control_bytes[0] || data != begin_control_bytes[1])
  // {
  //   if (HS->available() > 0)
  //     data = HS->read();
  // }
  Serial.println("Available Bytes:");
  int availableBytes = HS->available();
  Serial.println(availableBytes);

  while (data != begin_control_bytes[0])
  {
    Serial.println("wait for getFirmVersion");
    delay(100);
    if (HS->available() > 0)
      data = HS->read();
  }
  // if (availableBytes > 0)
  //   data = HS->read();
  // if (data != begin_control_bytes[0])
  // {
  //   return false;
  // }
  // else
  {
    buffer[0] = data;
    HS->readBytes(buffer + 1, 22 - 1);
    // Serial.write(buffer, 22);
    Serial.write(buffer, 22);

    byte check[12] = {
        0xFD,
        0xFC,
        0xFB,
        0xFA,
        0x0C,
        0x00,
        0xA0,
        0x01,
        0x00,
        0x00,
        0X01,
        0x00,
    };

    if (memcmp(buffer, check, 12) == 0)
    {
      Serial.println("getVersion success");
      // Serial.write(buffer, 30);
      delete[] buffer;
      return true;
    }
    Serial.println("getVersion failed");
    delete[] buffer;
    return false;
  }
}

int16_t HLK_LD2450::getCoordinateX(int i)
{
  return ld2450_data.data[i].target_x;
}

int16_t HLK_LD2450::getCoordinateY(int i)
{
  return ld2450_data.data[i].target_y;
}

int16_t HLK_LD2450::getVelocity(int i)
{
  return ld2450_data.data[i].speed;
}

uint16_t HLK_LD2450::getDistanceResolution(int i)
{
  return ld2450_data.data[i].distance_resolution;
}
