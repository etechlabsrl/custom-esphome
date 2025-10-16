#pragma once

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include <vector>
#include <Arduino.h>

namespace esphome {
namespace knx {

// Priority Flag bits (B7 B6)
enum KnxPriorityType : uint8_t {
  KNX_PRIORITY_SYSTEM = 0b00,      // System
  KNX_PRIORITY_ALARM = 0b10,       // Alarm
  KNX_PRIORITY_HIGH = 0b01,        // High
  KNX_PRIORITY_NORMAL = 0b11       // Normal
};

// Command Flag bits (B3 B2 B1 B0)
enum KnxCommandType : uint8_t {
  KNX_COMMAND_READ = 0b0000,
  KNX_COMMAND_WRITE = 0b0010,
  KNX_COMMAND_ANSWER = 0b0001,
  KNX_COMMAND_INDIVIDUAL_ADDR_WRITE = 0b0011,
  KNX_COMMAND_INDIVIDUAL_ADDR_REQUEST = 0b0100,
  KNX_COMMAND_INDIVIDUAL_ADDR_RESPONSE = 0b0101,
  KNX_COMMAND_MASK_VERSION_READ = 0b1100,
  KNX_COMMAND_MASK_VERSION_RESPONSE = 0b1101,
  KNX_COMMAND_RESTART = 0b1110,
  KNX_COMMAND_ESCAPE = 0b1111
};

// Extension Command Flag bits (B5 B4 B3 B2 B1 B0)
enum KnxExtCommandType : uint8_t {
  KNX_EXT_COMMAND_AUTH_REQUEST = 0b010001,
  KNX_EXT_COMMAND_AUTH_RESPONSE = 0b010010
};

// Communication Type Flag bits (B7 B6)
enum KnxCommunicationType : uint8_t {
  KNX_COMM_UDP = 0b00, // Unnumbered Data Packet
  KNX_COMM_NDP = 0b01, // Numbered Data Packet
  KNX_COMM_UCD = 0b10, // Unnumbered Control Data
  KNX_COMM_NCD = 0b11  // Numbered Control Data
};

// Control Data Flag bits (B1 B0)
enum KnxControlDataType : uint8_t {
  KNX_CONTROLDATA_CONNECT = 0b00,      // UCD
  KNX_CONTROLDATA_DISCONNECT = 0b01,   // UCD
  KNX_CONTROLDATA_POS_CONFIRM = 0b10,  // NCD
  KNX_CONTROLDATA_NEG_CONFIRM = 0b11   // NCD
};

class KnxTelegram {
 public:
  KnxTelegram();
  void clear();
  int calculate_checksum();
  void set_buffer(uint8_t *new_buffer);
  uint8_t *get_buffer() { return buffer; }
  void set_priority(KnxPriorityType type);
  KnxPriorityType get_priority();
  bool is_repeated();
  void set_repeated(bool repeated);
  void set_source_individual_address(int area, int line, int member);
  void set_target_group_address(int main, int middle, int sub);
  void set_target_individual_address(int area, int line, int member);
  bool is_target_group();
  int get_source_area();
  int get_source_line();
  int get_source_member();
  int get_target_area();
  int get_target_line();
  int get_target_member();
  int get_target_main_group();
  int get_target_middle_group();
  int get_target_sub_group();
  void set_routing_counter(int counter);
  int get_routing_counter();
  void set_payload_length(int length);
  int get_payload_length();
  void set_command(KnxCommandType type);
  KnxCommandType get_command();
  void set_communication_type(KnxCommunicationType type);
  KnxCommunicationType get_communication_type();
  void set_control_data(KnxControlDataType type);
  KnxControlDataType get_control_data();
  void set_sequence_number(int sequence_number);
  int get_sequence_number();
  void set_first_data_byte(int byte);
  int get_first_data_byte();
  void set_bool(bool value);
  bool get_bool();
  void set_4bit_int_value(int value);
  int get_4bit_int_value();
  void set_4bit_direction_value(bool direction);
  bool get_4bit_direction_value();
  void set_4bit_steps_value(uint8_t steps);
  uint8_t get_4bit_steps_value();
  void set_1byte_uchar_value(uint8_t value);
  uint8_t get_1byte_uchar_value();
  void set_2byte_uchar_value(uint16_t value);
  uint16_t get_2byte_uchar_value();
  void set_2byte_int_value(int16_t value);
  int16_t get_2byte_int_value();
  void set_2byte_float_value(float value);
  float get_2byte_float_value();
  void set_3byte_time(int weekday, int hour, int minute, int second);
  int get_3byte_weekday_value();
  int get_3byte_hour_value();
  int get_3byte_minute_value();
  int get_3byte_second_value();
  void set_3byte_date(int year, int month, int day);
  int get_3byte_date_year_value();
  int get_3byte_date_month_value();
  int get_3byte_date_day_value();
  void set_4byte_float_value(float value);
  float get_4byte_float_value();
  String get_14byte_value();

 protected:
  uint8_t buffer[24];
  uint8_t buffer_size = 0;
};

}  // namespace knx
}  // namespace esphome
