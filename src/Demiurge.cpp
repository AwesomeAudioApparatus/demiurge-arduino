
#include "Arduino.h"

#include "esp_types.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "driver/spi_master.h"
#include "Demiurge.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

static const char* TAG = "Demiurge";

portMUX_TYPE demiurgeTimerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer(void *parameter) {
   portENTER_CRITICAL_ISR(&demiurgeTimerMux);
   Demiurge::runtime().tick();
   portEXIT_CRITICAL_ISR(&demiurgeTimerMux);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

Demiurge::Demiurge() = default;

void Demiurge::begin() {
   ESP_LOGI(TAG, "Starting Demiurge");
   Serial.println("Starting Demiurge");

   initializeDacSpi();
   initializeAdcSpi();
   _dac = new MCP4822(_hspi);
   _adc = new ADC128S102(_hspi);
   _adc->queue();
//   initializeTimer();
   initializeSinks();
}

void Demiurge::initializeDacSpi() {
   ESP_LOGI(TAG, "Initializing DAC SPI");
   Serial.println("Initializing DAC SPI");
   memset(&_hspiBusConfig, 0, sizeof(_hspiBusConfig));
   _hspiBusConfig.miso_io_num = -1;
   _hspiBusConfig.mosi_io_num = DEMIURGE_HMISO_PIN;
   _hspiBusConfig.sclk_io_num = DEMIURGE_HCLK_PIN;
   _hspiBusConfig.quadwp_io_num = -1;
   _hspiBusConfig.quadhd_io_num = -1;
   _hspiBusConfig.max_transfer_sz = 16;
   //_hspiBusConfig.flags = SPICOMMON_BUSFLAG_MASTER;
   //_hspiBusConfig.intr_flags = ESP_INTR_FLAG_IRAM;

   memset(&_hspiDeviceIntfConfig, 0, sizeof(_hspiDeviceIntfConfig));
   _hspiDeviceIntfConfig.clock_speed_hz = 16000000;   //80 MHz
   _hspiDeviceIntfConfig.mode = 0;                    //SPI mode 0
   _hspiDeviceIntfConfig.spics_io_num = DEMIURGE_HCS_PIN;   //CS pin
   _hspiDeviceIntfConfig.queue_size = 8;
//   _hspiDeviceIntfConfig.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_NO_DUMMY;
//   _hspiDeviceIntfConfig.command_bits = 0;
//   _hspiDeviceIntfConfig.address_bits = 0;

   esp_err_t ret = spi_bus_initialize(HSPI_HOST, &_hspiBusConfig, 1);
   ESP_ERROR_CHECK(ret);

   ret = spi_bus_add_device(HSPI_HOST, &_hspiDeviceIntfConfig, &_hspi);
   ESP_ERROR_CHECK(ret);
}

void Demiurge::initializeAdcSpi() {
   ESP_LOGI(TAG, "Initializing ADC SPI");
   Serial.println("Initializing ADC SPI");
   memset(&_vspiBusConfig, 0, sizeof(_vspiBusConfig));
   _vspiBusConfig.miso_io_num = DEMIURGE_VMISO_PIN;
   _vspiBusConfig.mosi_io_num = DEMIURGE_VMOSI_PIN;
   _vspiBusConfig.sclk_io_num = DEMIURGE_VCLK_PIN;
   _vspiBusConfig.quadwp_io_num = -1;
   _vspiBusConfig.quadhd_io_num = -1;
   _vspiBusConfig.max_transfer_sz = 32;
//   _vspiBusConfig.flags = SPICOMMON_BUSFLAG_MASTER;
//   _vspiBusConfig.intr_flags = ESP_INTR_FLAG_IRAM;

   memset(&_vspiDeviceIntfConfig, 0, sizeof(_vspiDeviceIntfConfig));
   _vspiDeviceIntfConfig.clock_speed_hz = 16000000;   //16 MHz
   _vspiDeviceIntfConfig.mode = 0;                    //SPI mode 0
   _vspiDeviceIntfConfig.spics_io_num = DEMIURGE_VCS_PIN;   //CS pin
   _vspiDeviceIntfConfig.queue_size = 8;
//   _vspiDeviceIntfConfig.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_NO_DUMMY;
//   _vspiDeviceIntfConfig.command_bits = 0;
//   _vspiDeviceIntfConfig.address_bits = 0;

   esp_err_t ret = spi_bus_initialize(VSPI_HOST, &_vspiBusConfig, 2);
   ESP_ERROR_CHECK(ret);

   ret = spi_bus_add_device(HSPI_HOST, &_vspiDeviceIntfConfig, &_vspi);
   ESP_ERROR_CHECK(ret);
}

void Demiurge::initializeTimer() {
   ESP_LOGI(TAG, "Initializing Timer");
   Serial.println( "Initializing Timer");
   _config = static_cast<esp_timer_create_args_t *>(malloc(sizeof(esp_timer_create_args_t)));
   _config->callback = onTimer;
   _config->name = "Sampler";
   _config->dispatch_method = ESP_TIMER_TASK;

   esp_err_t timerError = esp_timer_create(_config, &_timer);
   ESP_ERROR_CHECK(timerError)

   // TODO: We can not run faster than 50 microseconds on soft-ish timers. Use hardware timers??
   timerError = esp_timer_start_periodic(_timer, 50);
   ESP_ERROR_CHECK(timerError)
}

void Demiurge::initializeSinks() {
   ESP_LOGI(TAG, "Initializing Sinks");
   Serial.println( "Initializing Sinks");
   for (auto &_sink : _sinks) {
      _sink = nullptr;
   }
}

#pragma clang diagnostic pop

Demiurge::~Demiurge() {
   ESP_LOGI(TAG, "Destructor");
   Serial.println( "Destructor");

   spi_bus_remove_device(_vspi);
   spi_bus_remove_device(_hspi);

   esp_timer_stop(_timer);
   esp_timer_delete(_timer);
   free(_config);
}

void Demiurge::registerSink(Sink *processor) {
   ESP_LOGI(TAG, "Registering sink");
   Serial.println( "Registering sink");
   for (auto &_sink : _sinks) {
      if (_sink == nullptr) {
         _sink = processor;
         break;
      }
   }
}


void Demiurge::unregisterSink(Sink *processor) {
   ESP_LOGI(TAG, "Unregistering sink");
   Serial.println( "Unregistering sink");
   for (auto &_sink : _sinks) {
      if (_sink == processor) {
         _sink = nullptr;
         break;
      }
   }
}

void IRAM_ATTR Demiurge::tick() {
   ESP_LOGD(TAG, "tick()");
   Serial.println( "tick()");
   readADC();
   for (auto &_sink : _sinks) {
      _sink->tick(timerCounter);
   }
   timerCounter = timerCounter + 50;
}


void IRAM_ATTR Demiurge::readADC() {
   ESP_LOGI(TAG, "Reading ADC");
   Serial.println( "Reading ADC");
   _adc->read(); // get the previous cycle's data.
   _adc->queue();  // start new conversion.
   uint16_t *channels = _adc->channels();
   double k = 20 / 4095;
   for (int i = 0; i < 8; i++) {
      // k = (y1-y2) / (x1-x2)
      // k = (10- -10) / (4095 - 0)
      // k = 20 / 4095

      // m = y1 - k * x1;
      // m = 10 - k * 4095;
      _inputs[i] = k * channels[i] - 10;
   }
}

void Demiurge::setDAC(int channel, double voltage) {
   ESP_LOGI(TAG, "Writing to DAC");
   Serial.println( "Writing to DAC");

   // Convert to 12 bit DAC levels. -10V -> 0, 0 -> 2048, +10V -> 4095
   auto output = (unsigned int) (voltage * 204.8 + 204.8);

   esp_err_t dacError = ESP_OK;
   if (channel == 1) {
      dacError = _dac->send(MCP4822_CHANNEL_A | MCP4822_GAIN | MCP4822_ACTIVE, output);
   }
   if (channel == 2) {
      dacError = _dac->send(MCP4822_CHANNEL_B | MCP4822_GAIN | MCP4822_ACTIVE, output);
   }
   ESP_ERROR_CHECK(dacError)

   _outputs[channel - 1] = voltage;
}

double *Demiurge::inputs() {
   return _inputs;
}

double *Demiurge::outputs() {
   return _outputs;
}

bool Demiurge::gpio(int pin) {
   return 0;
}
