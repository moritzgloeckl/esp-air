#include "esphome.h"

static const char *TAG = "sm300d2";
static const uint8_t RESPONSE_LENGTH = 17;

using namespace esphome;

class SM300D2Sensor : public PollingComponent,  public UARTDevice, public sensor::Sensor{
 public:
  SM300D2Sensor(UARTComponent *parent) : PollingComponent(1000), UARTDevice(parent) {}

  Sensor *co2_sensor = new Sensor();
  Sensor *formaldehyde_sensor = new Sensor();
  Sensor *tvoc_sensor = new Sensor();
  Sensor *pm2_5_sensor = new Sensor();
  Sensor *pm10_sensor = new Sensor();
  Sensor *temperature_sensor = new Sensor();
  Sensor *humidity_sensor = new Sensor();

  void setup() override {
      ESP_LOGCONFIG(TAG, "Setting up SM300D2...");
  }

  void loop() override {}

  void update() override {
    uint8_t response[RESPONSE_LENGTH];
    
    flush();
    bool read_success = read_array(response, RESPONSE_LENGTH);
    flush();
    
    if (!read_success) {
      ESP_LOGW(TAG, "Reading data from SM300D2 failed!");
      status_set_warning();
      return;
    }

    if (response[0] != 0x3C || response[1] != 0x02) {
      ESP_LOGW(TAG, "Invalid preamble!");
      this->status_set_warning();
      return;
    }

    uint16_t calculated_checksum = this->calculate_checksum(response);
    if (calculated_checksum != response[RESPONSE_LENGTH-1]) {
      ESP_LOGW(TAG, "Checksum doesn't match: 0x%02X!=0x%02X", response[RESPONSE_LENGTH-1], calculated_checksum);
      this->status_set_warning();
      return;
    }

    this->status_clear_warning();

    ESP_LOGW(TAG, "Successfully read SM300D2 data");

    const uint16_t co2 = (response[2] * 256) + response[3];
    const uint16_t formaldehyde = (response[4] * 256) + response[5];
    const uint16_t tvoc = (response[6] * 256) + response[7];
    const uint16_t pm2_5 = (response[8] * 256) + response[9];
    const uint16_t pm10 = (response[10] * 256) + response[11];
    const float temperature = response[12] + (response[13] * 0.1);
    const float humidity = response[14] + (response[15] * 0.1);

    ESP_LOGD(TAG, "Received CO₂: %u ppm", co2);
    this->co2_sensor->publish_state(co2);

    ESP_LOGD(TAG, "Received Formaldehyde: %u µg/m³", formaldehyde);
    this->formaldehyde_sensor->publish_state(formaldehyde);

    ESP_LOGD(TAG, "Received TVOC: %u µg/m³", tvoc);
    this->tvoc_sensor->publish_state(tvoc);

    ESP_LOGD(TAG, "Received PM2.5: %u µg/m³", pm2_5);
    this->pm2_5_sensor->publish_state(pm2_5);

    ESP_LOGD(TAG, "Received PM10: %u µg/m³", pm10);
    this->pm10_sensor->publish_state(pm10);

    ESP_LOGD(TAG, "Received Temperature: %.2f °C", temperature);
    this->temperature_sensor->publish_state(temperature);

    ESP_LOGD(TAG, "Received Humidity: %.2f percent", humidity);
    this->humidity_sensor->publish_state(humidity);
  }

  uint16_t calculate_checksum(uint8_t arr[RESPONSE_LENGTH]) {
    uint8_t sum = 0;
    for (int i = 0; i < (RESPONSE_LENGTH-1); i++) {
      sum += arr[i];
    }
    return sum;
  }
};
