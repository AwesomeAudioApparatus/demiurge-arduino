
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "driver/spi_master.h"
#include "mcp48x2/MCP48x2.h"

#include "Demiurge.h"

portMUX_TYPE demiurgeTimerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer(void *parameter) {
   portENTER_CRITICAL_ISR(&demiurgeTimerMux);
   Demiurge::runtime()->tick();
   portEXIT_CRITICAL_ISR(&demiurgeTimerMux);
}

bool Demiurge::_initialized = 0;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

Demiurge::Demiurge() {
   initializeSpi();
   initializeDac();
   initializeTimer();
   initializeSinks();
}

void Demiurge::initializeSpi() {
   memset(&spiBusConfig, 0, sizeof(spiBusConfig));
   spiBusConfig.miso_io_num = -1;
   spiBusConfig.mosi_io_num = PIN_NUM_MOSI;
   spiBusConfig.sclk_io_num = PIN_NUM_CLK;
   spiBusConfig.quadwp_io_num = -1;
   spiBusConfig.quadhd_io_num = -1;
   spiBusConfig.max_transfer_sz = 16;
   //spiBusConfig.flags = SPICOMMON_BUSFLAG_MASTER;
//spiBusConfig.intr_flags = ESP_INTR_FLAG_IRAM;

   memset(&spiDeviceIntfConfig, 0, sizeof(spiDeviceIntfConfig));
   spiDeviceIntfConfig.clock_speed_hz = 80000000;           //Clock out at 1 MHz
   spiDeviceIntfConfig.mode = 0;                                //SPI mode 0
   spiDeviceIntfConfig.spics_io_num = PIN_NUM_CS;               //CS pin
   spiDeviceIntfConfig.queue_size = 8;
//   spiDeviceIntfConfig.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_NO_DUMMY;
//   spiDeviceIntfConfig.command_bits = 0;
//   spiDeviceIntfConfig.address_bits = 0;

   //Initialize the SPI bus
   esp_err_t ret = spi_bus_initialize(HSPI_HOST, &spiBusConfig, 1);
}

void Demiurge::initializeDac() {
   _dac = new MCP48x2(MCP4822, spi);
   _config = static_cast<esp_timer_create_args_t *>(malloc(sizeof(esp_timer_create_args_t)));
   _config->callback = onTimer;
   _config->name = "Sampler";
   _config->dispatch_method = ESP_TIMER_TASK;
}

void Demiurge::initializeTimer() {

   // TODO: What if there is an error?
   esp_err_t timerError = esp_timer_create(_config, &_timer);

   // TODO: What if there is an error?
   // TODO: We can not run faster than 50 microseconds on soft-ish timers. Use hardware timers??
   timerError = esp_timer_start_periodic(_timer, 50);
}

void Demiurge::initializeSinks() {
   for (auto &_sink : _sinks) {
      _sink = nullptr;
   }
}

#pragma clang diagnostic pop

Demiurge::~Demiurge() {
   esp_timer_stop(_timer);
   esp_timer_delete(_timer);
   free(_config);
}

void Demiurge::registerSink(Sink *processor) {
   for (auto &_sink : _sinks) {
      if (_sink == nullptr) {
         _sink = processor;
         break;
      }
   }
}

void Demiurge::unregisterSink(Sink *processor) {
   for (auto &_sink : _sinks) {
      if (_sink == processor) {
         _sink = nullptr;
         break;
      }
   }
}


void IRAM_ATTR Demiurge::tick() {
   readADC();
   for (auto &_sink : _sinks) {
      _sink->tick(timerCounter);
   }
   timerCounter = timerCounter + 50;
}

void IRAM_ATTR Demiurge::readADC() {

}

void Demiurge::setDAC(int channel, double voltage) {

   // Convert to 12 bit DAC levels. -10V -> 0, 0 -> 2048, +10V -> 4095
   auto output = (unsigned int) (voltage * 204.8 + 204.8);

   if( channel == 1 )
   {
      _dac->send(CHANNEL_A | GAIN_1 | MODE_ACTIVE, output);
   }
   if( channel == 2 )
   {
      _dac->send(CHANNEL_B | GAIN_1 | MODE_ACTIVE, output);
   }
}

analog_in_t *Demiurge::inputs() {
   return _inputs;
}

analog_in_t *Demiurge::outputs() {
   return _outputs;
}

bool Demiurge::gpio(int pin) {
   return 0;
}
