#ifndef __SPI_CTL__
#define __SPI_CTL__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <semaphore.h>

#define SPI_ROUTE_0_0 "/dev/spidev0.0"
#define SPI_ROUTE_0_1 "/dev/spidev0.1"
#define SPI_ROUTE_1_0 "/dev/spidev1.0"
#define SPI_ROUTE_1_1 "/dev/spidev1.1"
#define SPI_ROUTE_2_0 "/dev/spidev2.0"
#define SPI_ROUTE_2_1 "/dev/spidev2.1"

#define SPI_CLOCK_VS1003 200 * 1000

typedef enum
{
    SPI_IOCTL_MSB_FIRST,
    SPI_IOCTL_LSB_FIRST,
}SPI_IOCTL_LSB;

typedef enum
{
    SPI_IOCTL_8BIT = 8,
    SPI_IOCTL_16BIT = 16,
}SPI_IOCTL_BIT;

typedef enum
{
    SPI_INDEX_0_0,
    SPI_INDEX_0_1,
    SPI_INDEX_1_0,
    SPI_INDEX_1_1,
    SPI_INDEX_2_0,
    SPI_INDEX_2_1
}SPI_INDEX;

typedef enum
{
    SPI_DEV_VS1003,
    SPI_DEV_MAX
}SPI_DEVICE_INDEX;

typedef struct
{
    int bit;
    int mode;
    int lsb;
    int speed;
}spi_init_t;

typedef struct
{
    int fd;
    sem_t sem;
}spi_ctl_t;

int init_spi();
int send_recv_spi_full_duplex(SPI_DEVICE_INDEX spi_dev, uint8_t *recv, uint8_t *send, int max_buffer_size);
int transmit_spi(SPI_DEVICE_INDEX spi_dev, uint8_t *tx, size_t tx_size);
int receive_spi(SPI_DEVICE_INDEX spi_dev, uint8_t *rx, size_t rx_size);
int reset_spi_frequency(SPI_DEVICE_INDEX spi_dev, int freq);

#endif
