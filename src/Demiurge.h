#ifndef _DEMIURGE_DEMIURGE_H_
#define _DEMIURGE_DEMIURGE_H_

#ifndef DEMIURGE_MAX_SINKS
#define DEMIURGE_MAX_SINKS 2
#endif

#ifndef PIN_NUM_MOSI
#define PIN_NUM_MOSI 13
#endif

#ifndef PIN_NUM_CLK
#define PIN_NUM_CLK 14
#endif

#ifndef PIN_NUM_CS
#define PIN_NUM_CS 15
#endif

#include "Adsr.h"
#include "AudioInPort.h"
#include "AudioOutPort.h"
#include "ControlPair.h"
#include "CvInPort.h"
#include "FixedSignal.h"
#include "GatePort.h"
#include "Inverter.h"
#include "Mixer.h"
#include "Pan.h"
#include "Passthru.h"
#include "Potentiometer.h"
#include "PushButton.h"
#include "Signal.h"
#include "Sink.h"
#include "SoundProcessor.h"
#include "Threshold.h"
#include "VCO.h"
#include "mcp48x2/MCP48x2.h"

typedef struct {
   double value[8];
} analog_in_t;

typedef struct {
   double value[2];
} analog_out_t;

class Demiurge {

   void registerSink(Sink *processor);

   void unregisterSink(Sink *processor);

public:

   static void initialize() {
      if (_initialized)
         return;
      _runtime = new Demiurge();
      _initialized = true;
   }

   static void shutdown() {
      delete _runtime;
      _initialized = false;
   }

   static Demiurge *runtime() {
      return _runtime;
   }

   static double clip(double value) {
      if (value > 10.0) return 10.0;
      if (value < -10.0) return -10.0;
      return value;
   };

   void tick();

   analog_in_t *inputs();

   analog_out_t *outputs();

   void setDAC(int channel, double voltage);

   bool gpio(int i);

private:
   static bool _initialized;

   static Demiurge *_runtime;

   Demiurge();

   ~Demiurge();

   void initializeSpi();

   void initializeDac();

   void initializeTimer();

   void initializeSinks();

   Sink *_sinks[DEMIURGE_MAX_SINKS] = {nullptr, nullptr};

   void readADC();

   analog_in_t _inputs;
   analog_out_t _outputs;

   double timerCounter = 0;         // in microseconds, increments 50 at a time.
   esp_timer_create_args_t *_config;

   esp_timer_handle_t _timer;
   spi_device_handle_t spi;
   spi_bus_config_t spiBusConfig;
   spi_device_interface_config_t spiDeviceIntfConfig;

   MCP48x2 *_dac;
};

#endif
