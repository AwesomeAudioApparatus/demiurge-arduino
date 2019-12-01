
#include <esp_system.h>
#include <string.h>
#include "driver/spi_master.h"

#include "MCP4822.h"

MCP4822::MCP4822(spi_device_handle_t spi) {
   _spi = spi;
}

esp_err_t MCP4822::send(unsigned char config, unsigned int data) {
   uint16_t payload = data;
   payload = SPI_SWAP_DATA_TX((payload & 0x0FFF) | (config << 8), 16);

   spi_transaction_t t;
   memset(&t, 0, sizeof(t));       //Zero out the transaction
   t.length = 16;
   t.tx_buffer = &payload;
   return spi_device_polling_transmit(_spi, &t);
}
