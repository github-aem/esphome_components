#include "modbus_tcp_client.h"

namespace esphome {
namespace modbus_tcp {

static const char *const TAG = "modbus_tcp";

void ModbusTcpClient::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Modbus TCP Client...");
  connect_();
}

void ModbusTcpClient::loop() {
  if (millis() - last_update_ >= update_interval_) {
    if (!client_.connected()) {
      ESP_LOGW(TAG, "Connection lost, attempting to reconnect...");
      connect_();
    }
    last_update_ = millis();
  }
}

void ModbusTcpClient::dump_config() {
  ESP_LOGCONFIG(TAG, "Modbus TCP Client:");
  ESP_LOGCONFIG(TAG, "  Host: %s", host_.c_str());
  ESP_LOGCONFIG(TAG, "  Port: %u", port_);
  ESP_LOGCONFIG(TAG, "  Update Interval: %ums", update_interval_);
}

bool ModbusTcpClient::connect_() {
  if (client_.connected())
    return true;

  if (!client_.connect(host_.c_str(), port_)) {
    ESP_LOGE(TAG, "Connection to %s:%u failed!", host_.c_str(), port_);
    return false;
  }

  ESP_LOGI(TAG, "Connected to %s:%u", host_.c_str(), port_);
  return true;
}

void ModbusTcpClient::disconnect_() {
  if (client_.connected())
    client_.stop();
}

bool ModbusTcpClient::send_request_(uint8_t function_code, uint16_t address, uint16_t quantity) {
  if (!connect_())
    return false;

  // Modbus TCP header (7 bytes)
  uint8_t header[7];
  header[0] = transaction_id_ >> 8;    // Transaction ID Hi
  header[1] = transaction_id_ & 0xFF;   // Transaction ID Lo
  header[2] = 0x00;                     // Protocol ID Hi
  header[3] = 0x00;                     // Protocol ID Lo
  header[4] = 0x00;                     // Length Hi
  header[5] = 0x06;                     // Length Lo (6 bytes follow)
  header[6] = 0x01;                     // Unit ID

  // Modbus request (6 bytes)
  uint8_t request[6];
  request[0] = function_code;           // Function code
  request[1] = address >> 8;            // Starting Address Hi
  request[2] = address & 0xFF;          // Starting Address Lo
  request[3] = quantity >> 8;           // Quantity Hi
  request[4] = quantity & 0xFF;         // Quantity Lo

  // Send request
  if (client_.write(header, 7) != 7) {
    ESP_LOGE(TAG, "Failed to send header");
    return false;
  }
  if (client_.write(request, 5) != 5) {
    ESP_LOGE(TAG, "Failed to send request");
    return false;
  }

  transaction_id_++;
  return true;
}

bool ModbusTcpClient::process_response_() {
  // Wait for response
  uint32_t start = millis();
  while (client_.available() < 7) {
    if (millis() - start > 1000) {
      ESP_LOGE(TAG, "Response timeout");
      return false;
    }
    delay(1);
  }

  // Read Modbus TCP header
  uint8_t header[7];
  client_.readBytes(header, 7);

  // Process header
  uint16_t length = (header[4] << 8) | header[5];
  uint8_t unit_id = header[6];

  // Read response data
  std::vector<uint8_t> response(length);
  uint32_t bytes_read = client_.readBytes(response.data(), length);
  
  if (bytes_read != length) {
    ESP_LOGE(TAG, "Incomplete response");
    return false;
  }

  // Process response based on function code
  uint8_t function_code = response[0];
  if (function_code & 0x80) {
    ESP_LOGE(TAG, "Modbus exception received: 0x%02X", response[1]);
    return false;
  }

  return true;
}

void ModbusTcpClient::read_coil(uint16_t address, uint16_t quantity) {
  if (send_request_(0x01, address, quantity)) {
    process_response_();
  }
}

void ModbusTcpClient::read_discrete_input(uint16_t address, uint16_t quantity) {
  if (send_request_(0x02, address, quantity)) {
    process_response_();
  }
}

void ModbusTcpClient::read_holding_register(uint16_t address, uint16_t quantity) {
  if (send_request_(0x03, address, quantity)) {
    process_response_();
  }
}

void ModbusTcpClient::read_input_register(uint16_t address, uint16_t quantity) {
  if (send_request_(0x04, address, quantity)) {
    process_response_();
  }
}

void ModbusTcpClient::write_single_coil(uint16_t address, bool state) {
  uint16_t value = state ? 0xFF00 : 0x0000;
  if (send_request_(0x05, address, value)) {
    process_response_();
  }
}

void ModbusTcpClient::write_single_register(uint16_t address, uint16_t value) {
  if (send_request_(0x06, address, value)) {
    process_response_();
  }
}

}  // namespace modbus_tcp
}  // namespace esphome