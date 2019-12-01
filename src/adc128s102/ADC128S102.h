#ifndef DEMIURGE_ADC128S102_H
#define DEMIURGE_ADC128S102_H

#include "driver/spi_master.h"

class ADC128S102 {

public:
   explicit ADC128S102(spi_device_handle_t spi);

   ~ADC128S102();

   void queue();

   void read();

   uint16_t *channels();

private :
   spi_device_handle_t _spi;
   spi_transaction_t _tx;
   spi_transaction_t *_rx;

   uint8_t _txdata[16];
   uint8_t _rxdata[16];

   uint16_t _channels[8];

};


#endif
