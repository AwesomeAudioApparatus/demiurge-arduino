
#ifndef _DEMIURGE_MCP4822_h_
#define _DEMIURGE_MCP4822_h_

#include "driver/spi_master.h"

#define MCP4822_CHANNEL_A 0x00
#define MCP4822_CHANNEL_B 0x80
#define MCP4822_GAIN 0x20
#define MCP4822_ACTIVE 0x10

#define MCP4822_CHANNEL_SELECT 7
#define MCP4822_GAIN_SELECT 5
#define MCP4822_SHUTDOWN_SELECT 4

class MCP4822 {
  public:
   MCP4822(spi_device_handle_t spi);

   esp_err_t send(unsigned char config, unsigned int data);

  private:
    spi_device_handle_t _spi;
};
#endif