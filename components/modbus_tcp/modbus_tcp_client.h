#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "esphome/components/ethernet/ethernet_component.h"
#include <Client.h>
#include <vector>

namespace esphome {

namespace ethernet {
class EthernetComponent;
}
extern ethernet::EthernetComponent *global_eth_component;


namespace modbus_tcp {

class ModbusTcpClient : public Component {
 public:
  ModbusTcpClient() = default;

  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_host(const std::string &host) { host_ = host; }
  void set_port(uint16_t port) { port_ = port; }
  void set_update_interval(uint32_t update_interval) { update_interval_ = update_interval; }

  float get_setup_priority() const override { return setup_priority::DATA; }

  // Methods for reading/writing Modbus registers
  void read_coil(uint16_t address, uint16_t quantity);
  void read_discrete_input(uint16_t address, uint16_t quantity);
  void read_holding_register(uint16_t address, uint16_t quantity);
  void read_input_register(uint16_t address, uint16_t quantity);
  void write_single_coil(uint16_t address, bool state);
  void write_single_register(uint16_t address, uint16_t value);

 protected:
  std::string host_;
  uint16_t port_{502};  // Default Modbus TCP port
  uint32_t update_interval_{5000};  // Default update interval: 5 seconds
  uint32_t last_update_{0};
  Client *client_{nullptr};
  uint16_t transaction_id_{0};

  bool connect_();
  void disconnect_();
  bool send_request_(uint8_t function_code, uint16_t address, uint16_t quantity);
  bool process_response_();
  uint16_t calculate_crc_(const uint8_t *data, size_t length);
};

}  // namespace modbus_tcp
}  // namespace esphome