#include "knx_telegram.h"
#include <Arduino.h>

namespace esphome {
namespace knx {

KnxTelegram::KnxTelegram() {
  this->clear();
}

void KnxTelegram::clear() {
  for (int i = 0; i < 24; i++) {
    buffer[i] = 0;
  }

  // PCI (Physical Control Information)
  // Repeat + Priority + System Priority (B10111100)
  buffer[0] = 0b10111100;

  // Source Address (0.0.0)
  buffer[1] = 0x00;
  buffer[2] = 0x00;

  // Target Address (0/0/0)
  buffer[3] = 0x00;
  buffer[4] = 0x00;

  // APCI (Application Protocol Control Information) + Data Length (225 + 0 + 1)
  // Target Addr Type (Group) + Hop Count (7) + Extended Frame (0) + Payload Length (0+1) (B11100001)
  buffer[5] = 0b11100001;

  // TPCI + Command (APCI)
  // Comm Type (UDP) + Sequence (0) + Control Data (0) (B00000000)
  buffer[6] = 0x00;

  // Data
  buffer[7] = 0x00;

  buffer_size = 0;
}

void KnxTelegram::set_buffer(uint8_t *new_buffer) {
  for (int i = 0; i < 24; i++) {
    buffer[i] = new_buffer[i];
  }
}

bool KnxTelegram::is_repeated() {
  if (buffer[0] & 0b00100000) {
    return true;
  } else {
    return false;
  }
}

void KnxTelegram::set_repeated(bool repeated) {
  if (repeated) {
    // Repeated (B11011111)
    buffer[0] = buffer[0] & 0b11011111;
  } else {
    // Not Repeated (B00100000)
    buffer[0] = buffer[0] | 0b00100000;
  }
}

void KnxTelegram::set_priority(KnxPriorityType type) {
  // Clear priority bits (B11110011)
  buffer[0] = buffer[0] & 0b11110011;
  // Set priority bits
  buffer[0] = buffer[0] | (type << 2);
}

KnxPriorityType KnxTelegram::get_priority() {
  // Extract priority bits (B00001100)
  return (KnxPriorityType) ((buffer[0] & 0b00001100) >> 2);
}

void KnxTelegram::set_source_individual_address(int area, int line, int member) {
  buffer[1] = (area << 4) | (line);
  buffer[2] = (member);
}

int KnxTelegram::get_source_area() {
  return (buffer[1] >> 4);
}

int KnxTelegram::get_source_line() {
  // Extract line bits (B00001111)
  return (buffer[1] & 0b00001111);
}

int KnxTelegram::get_source_member() {
  return (buffer[2]);
}

void KnxTelegram::set_target_group_address(int main, int middle, int sub) {
  // Target is group (B10000000)
  buffer[5] = buffer[5] | 0b10000000;

  // Set Address
  buffer[3] = (main << 3) | (middle);
  buffer[4] = (sub);
}

void KnxTelegram::set_target_individual_address(int area, int line, int member) {
  // Target is individual (B01111111)
  buffer[5] = buffer[5] & 0b01111111;

  // Set Address
  buffer[3] = (area << 4) | (line);
  buffer[4] = (member);
}

bool KnxTelegram::is_target_group() {
  // Check Target Address Type (B10000000)
  return buffer[5] & 0b10000000;
}

int KnxTelegram::get_target_area() {
  return (buffer[3] >> 4);
}

int KnxTelegram::get_target_line() {
  // Extract line bits (B00001111)
  return (buffer[3] & 0b00001111);
}

int KnxTelegram::get_target_member() {
  return (buffer[4]);
}

int KnxTelegram::get_target_main_group() {
  // Extract main group bits (B01111000)
  return ((buffer[3] & 0b01111000) >> 3);
}

int KnxTelegram::get_target_middle_group() {
  // Extract middle group bits (B00000111)
  return (buffer[3] & 0b00000111);
}

int KnxTelegram::get_target_sub_group() {
  return (buffer[4]);
}

void KnxTelegram::set_routing_counter(int counter) {
  // Clear Routing Counter (B10000000)
  buffer[5] = buffer[5] & 0b10000000;
  // Set Routing Counter
  buffer[5] = buffer[5] | ((counter & 0b0111) << 4);
}

int KnxTelegram::get_routing_counter() {
  // Extract Routing Counter (B01110000)
  return ((buffer[5] & 0b01110000) >> 4);
}

void KnxTelegram::set_payload_length(int length) {
  // Clear payload length (B11110000)
  buffer[5] = buffer[5] & 0b11110000;
  // Set payload length
  buffer[5] = buffer[5] | ((length - 1) & 0b00001111);
}

int KnxTelegram::get_payload_length() {
  // Extract payload length (B00001111)
  int length = (buffer[5] & 0b00001111) + 1;
  if (length > 16)
    length = 16;
  return length;
}

void KnxTelegram::set_command(KnxCommandType type) {
  // Clear command bits (B11111100)
  buffer[6] = buffer[6] & 0b11111100;
  // Clear command bits (B00111111)
  buffer[7] = buffer[7] & 0b00111111;

  // Set command bits
  buffer[6] = buffer[6] | ((type >> 2) & 0b00000011);
  buffer[7] = buffer[7] | ((type & 0b00000011) << 6);
}

KnxCommandType KnxTelegram::get_command() {
  // Extract command bits (B00000011) + (B11000000)
  return (KnxCommandType) (((buffer[6] & 0b00000011) << 2) | ((buffer[7] & 0b11000000) >> 6));
}

void KnxTelegram::set_control_data(KnxControlDataType type) {
  // Clear control data bits (B11111100)
  buffer[6] = buffer[6] & 0b11111100;
  // Set control data bits
  buffer[6] = buffer[6] | (type & 0b00000011);
}

KnxControlDataType KnxTelegram::get_control_data() {
  // Extract control data bits (B00000011)
  return (KnxControlDataType) (buffer[6] & 0b00000011);
}

KnxCommunicationType KnxTelegram::get_communication_type() {
  // Extract communication type bits (B11000000)
  return (KnxCommunicationType) ((buffer[6] & 0b11000000) >> 6);
}

void KnxTelegram::set_communication_type(KnxCommunicationType type) {
  // Clear communication type bits (B00111111)
  buffer[6] = buffer[6] & 0b00111111;
  // Set communication type bits
  buffer[6] = buffer[6] | (type << 6);
}

int KnxTelegram::get_sequence_number() {
  // Extract sequence number bits (B00111100)
  return (buffer[6] & 0b00111100) >> 2;
}

void KnxTelegram::set_sequence_number(int sequence_number) {
  // Clear sequence number bits (B11000011)
  buffer[6] = buffer[6] & 0b11000011;
  // Set sequence number bits
  buffer[6] = buffer[6] | ((sequence_number & 0b00001111) << 2);
}

int KnxTelegram::calculate_checksum() {
  uint8_t bcc = 0;
  // bcc is 8-bit XOR sum of 7-th bit of 1st byte to 7-th bit of 7th byte
  for (int i = 0; i < 7; i++) {
    bcc ^= buffer[i];
  }
  return bcc;
}

void KnxTelegram::set_first_data_byte(int byte) {
  // Clear first data byte (B11000000)
  buffer[7] = buffer[7] & 0b11000000;
  // Set first data byte
  buffer[7] = buffer[7] | (byte & 0b00111111);
}

int KnxTelegram::get_first_data_byte() {
  // Extract first data byte (B00111111)
  return (buffer[7] & 0b00111111);
}

void KnxTelegram::set_bool(bool value) {
  this->set_first_data_byte(value ? 0b00000001 : 0b00000000);
}

bool KnxTelegram::get_bool() {
  // Extract boolean (B00000001)
  return (get_first_data_byte() & 0b00000001);
}

void KnxTelegram::set_4bit_int_value(int value) {
  this->set_first_data_byte(value);
}

int KnxTelegram::get_4bit_int_value() {
  // Extract 4-bit integer (B00001111)
  return (get_first_data_byte() & 0b00001111);
}

void KnxTelegram::set_4bit_direction_value(bool direction) {
  this->set_first_data_byte(direction ? 0b00001000 : 0b00000000);
}

bool KnxTelegram::get_4bit_direction_value() {
  // Extract direction (B00001000)
  return ((get_first_data_byte() & 0b00001000)) >> 3;
}

void KnxTelegram::set_4bit_steps_value(uint8_t steps) {
  this->set_first_data_byte(steps);
}

uint8_t KnxTelegram::get_4bit_steps_value() {
  // Extract steps (B00000111)
  return (get_first_data_byte() & 0b00000111);
}

void KnxTelegram::set_1byte_uchar_value(uint8_t value) {
  buffer[8] = value;
  this->set_payload_length(2);
}

uint8_t KnxTelegram::get_1byte_uchar_value() {
  return buffer[8];
}

void KnxTelegram::set_2byte_uchar_value(uint16_t value) {
  buffer[8] = (value >> 8) & 0xFF;
  buffer[9] = value & 0xFF;
  this->set_payload_length(3);
}

uint16_t KnxTelegram::get_2byte_uchar_value() {
  uint16_t value = (buffer[8] << 8) | (buffer[9]);
  return value;
}

void KnxTelegram::set_2byte_int_value(int16_t value) {
  buffer[8] = (value >> 8) & 0xFF;
  buffer[9] = value & 0xFF;
  this->set_payload_length(3);
}

int16_t KnxTelegram::get_2byte_int_value() {
  int16_t value = (buffer[8] << 8) | (buffer[9]);
  return value;
}

void KnxTelegram::set_2byte_float_value(float value) {
  int sign = value < 0;
  if (sign) {
    value = -value;
  }

  // Find the largest exponent k (0..7) such that 2^k <= value
  int exponent = 0;
  if (value > 0) {
    exponent = floor(log2(value));
  }
  if (exponent > 7) {
    exponent = 7;
  }

  // Mantissa = value / 2^exponent
  int mantissa = round(value / pow(2, exponent) * 2048);

  buffer[8] = (sign << 7) | ((exponent & 0b0111) << 3) | ((mantissa >> 8) & 0b00000111);
  buffer[9] = mantissa & 0xFF;
  this->set_payload_length(3);
}

float KnxTelegram::get_2byte_float_value() {
  // Extract exponent bits (B01111000)
  int exponent = (buffer[8] & 0b01111000) >> 3;
  // Extract mantissa bits (B00000111)
  int mantissa = ((buffer[8] & 0b00000111) << 8) | (buffer[9]);

  float value = (float) mantissa * pow(2, exponent) / 2048;

  // Extract sign bit (B10000000)
  if (buffer[8] & 0b10000000) {
    value = -value;
  }

  return value;
}

void KnxTelegram::set_3byte_time(int weekday, int hour, int minute, int second) {
  // Weekday (B11100000) + Hour (B00011111)
  buffer[8] = (weekday & 0b11100000) + (hour & 0b00011111);
  // Minute (B00111111)
  buffer[9] =  minute & 0b00111111;
  // Second (B00111111)
  buffer[10] = second & 0b00111111;
  this->set_payload_length(4);
}

int KnxTelegram::get_3byte_weekday_value() {
  // Extract weekday bits (B11100000)
  return (buffer[8] & 0b11100000) >> 5;
}

int KnxTelegram::get_3byte_hour_value() {
  // Extract hour bits (B00011111)
  return (buffer[8] & 0b00011111);
}

int KnxTelegram::get_3byte_minute_value() {
  // Extract minute bits (B00111111)
  return (buffer[9] & 0b00111111);
}

int KnxTelegram::get_3byte_second_value() {
  // Extract second bits (B00111111)
  return (buffer[10] & 0b00111111);
}

void KnxTelegram::set_3byte_date(int year, int month, int day) {
  // Year + Month + Day
  buffer[8] = (year << 4) | (month);
  buffer[9] = day;
  this->set_payload_length(3);
}

int KnxTelegram::get_3byte_date_year_value() {
  return (buffer[8] >> 4);
}

int KnxTelegram::get_3byte_date_month_value() {
  // Extract month bits (B00001111)
  return (buffer[8] & 0b00001111);
}

int KnxTelegram::get_3byte_date_day_value() {
  return (buffer[9]);
}

void KnxTelegram::set_4byte_float_value(float value) {
  uint8_t *b = (uint8_t *) &value;

  buffer[8 + 3] = b[0];
  buffer[8 + 2] = b[1];
  buffer[8 + 1] = b[2];
  buffer[8 + 0] = b[3];
  this->set_payload_length(5);
}

float KnxTelegram::get_4byte_float_value() {
  float value;
  uint8_t *b = (uint8_t *) &value;

  b[0] = buffer[8 + 3];
  b[1] = buffer[8 + 2];
  b[2] = buffer[8 + 1];
  b[3] = buffer[8 + 0];
  return value;
}

String KnxTelegram::get_14byte_value() {
  uint8_t _load[14];
  _load[0] = buffer[8 + 0];
  _load[1] = buffer[8 + 1];
  _load[2] = buffer[8 + 2];
  _load[3] = buffer[8 + 3];
  _load[4] = buffer[8 + 4];
  _load[5] = buffer[8 + 5];
  _load[6] = buffer[8 + 6];
  _load[7] = buffer[8 + 7];
  _load[8] = buffer[8 + 8];
  _load[9] = buffer[8 + 9];
  _load[10] = buffer[8 + 10];
  _load[11] = buffer[8 + 11];
  _load[12] = buffer[8 + 12];
  _load[13] = buffer[8 + 13];

  String ret = String((char *) _load);
  return ret;
}


}  // namespace knx
}  // namespace esphome
