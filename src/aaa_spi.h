//
// Created by niclas on 2019-12-31.
//

#ifndef _DEMIURGE_MY_SPI_H_
#define _DEMIURGE_MY_SPI_H_

#include <driver/spi_common.h>
#include <driver/spi_master.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MY_ESP_ERR(base, x)      ((esp_err_t) ((base) + (x)))
#define MY_ESP_ERR_USER_BASE    (0x40000000)
#define MY_ESP_ERR_SPI_BASE     (MY_ESP_ERR_USER_BASE + 0x1000 * 1)
#define MY_ESP_ERR_SPI(x)                       MY_ESP_ERR(MY_ESP_ERR_SPI_BASE, (x))
#define MY_ESP_ERR_SPI_SPI1_IS_NOT_SUPPORTED    MY_ESP_ERR_SPI(1)
#define MY_ESP_ERR_SPI_INVALID_HOST_NUMBER      MY_ESP_ERR_SPI(2)
#define MY_ESP_ERR_SPI_INVALID_DMA_CHANNEL      MY_ESP_ERR_SPI(3)
#define DEMIURGE_ESP_ERR_SPI_HOST_ALREADY_IN_USE      MY_ESP_ERR_SPI(4)
#define DEMIURGE_ESP_ERR_SPI_DMA_ALREADY_IN_USE       MY_ESP_ERR_SPI(5)

esp_err_t
aaa_spi_prepare_circular(const spi_host_device_t spiHostDevice, const int dma_chan,
                         const lldesc_t *lldescs_out, const lldesc_t *lldescs_in,
                         const long dmaClockSpeedInHz, const gpio_num_t mosi_gpio_num,
                         const gpio_num_t miso_gpio_num, const gpio_num_t sclk_gpio_num, const int waitCycle);

esp_err_t aaa_spi_release_circular_buffer(spi_host_device_t host, int dma_chan);

spi_dev_t *aaa_spi_get_hw_for_host(spi_host_device_t host);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
