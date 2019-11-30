/*
     MCP48x2.cpp - Library for Microship DAC MCP4802/4812/4822.
     Created by Jonas Forsberg, 18/1, 2014.
     Licensed under GPL3, no warrenty what so ever and the code is "as-is".
     version 0.1

*/

#include <esp_system.h>
#include "driver/spi_common.h"

#include "MCP48x2.h"

#define LOG_TAG "MCP48x2"

MCP48x2::MCP48x2(unsigned char mcpModel, spi_device_handle_t spi)
{
    //if latch pin isn't used be sure to tie the latch pin on MCP chip to GND
    //the both channels will be latched on the rising edge of CS (chip select)
    this->_useLDAC = false;
    this->_init(mcpModel, spi, 0);
}

MCP48x2::MCP48x2(unsigned char mcpModel, spi_device_handle_t spi, unsigned int ldacPin)
{
    this->_useLDAC = true;
    this->_init(mcpModel, spi, ldacPin);
}

void MCP48x2::_init(unsigned char mcpModel, spi_device_handle_t spi, unsigned int ldacPin)
{
    this->_mcpModel = mcpModel;
    this->_spi = spi;
    this->_ldacPin = ldacPin;
    
}

void MCP48x2::send(unsigned char config, unsigned int data)
{
    //move the LSB on the data to corispond to the model of chip
    uint16_t payload = data;
    if (this->_mcpModel)
        payload = payload << this->_mcpModel;

    //apply the config to the config bits
    payload = SPI_SWAP_DATA_TX( (config << 8) | (payload & 0x0FFF), 16 );

    // ESP_LOGD(LOG_TAG, "sending: 0x%04x", payload);

    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = 16;
    t.tx_buffer = &payload;
    //t.cmd = payload;
    ret=spi_device_polling_transmit(this->_spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.

}

void MCP48x2::latch()
{
    if (this->_useLDAC)
    {
        //Latch pin is defined and the latch pin will be set to LOW to latch both registers
        //digitalWrite(this->_ldacPin, LOW);
        //raise the latch pin.
        //digitalWrite(this->_ldacPin, HIGH);
    }
}
