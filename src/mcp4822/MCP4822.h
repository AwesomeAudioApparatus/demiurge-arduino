/*
  Copyright 2019, Awesome Audio Apparatus.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

      https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
      limitations under the License.
*/

#ifndef _DEMIURGE_MCP4822_h_
#define _DEMIURGE_MCP4822_h_

#include "driver/spi_master.h"

#define MCP4822_CHANNEL_A 0x00
#define MCP4822_CHANNEL_B 0x80
#define MCP4822_GAIN 0x20
#define MCP4822_ACTIVE 0x10

typedef struct {
   spi_device_handle_t _spi;
   spi_transaction_t _tx;
   uint8_t *_rxdata;
   uint8_t *_txdata;
} mcp4822;

void mcp4822_init( mcp4822 *handle, spi_device_handle_t spi );

void mcp4822_dacOutput(mcp4822 *handle, uint16_t outputA, uint16_t outputB);

#endif