#include "spi_ctl.h"

spi_ctl_t spi[SPI_DEV_MAX] = {0, };

static int set_spi_ioctl(SPI_DEVICE_INDEX spi_dev, spi_init_t *spi_init_val)
{
    int ret = 0;

    ret = ioctl(spi[spi_dev].fd, SPI_IOC_WR_BITS_PER_WORD, &spi_init_val->bit);
    if(ret < 0)
    {
        perror("fail to set spi bit");
        return ret;
    }

    ret = ioctl(spi[spi_dev].fd, SPI_IOC_WR_LSB_FIRST, &spi_init_val->lsb);
    if(ret < 0)
    {
        perror("fail to set spi bit");
        return ret;
    }

    ret = ioctl(spi[spi_dev].fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_init_val->speed);
    if(ret < 0)
    {
        perror("fail to set spi speed");
        return ret;
    }

    ret = ioctl(spi[spi_dev].fd, SPI_IOC_WR_MODE, &spi_init_val->mode);
    if(ret < 0)
    {
        perror("fail to set spi mode");
        return ret;
    }

    return 1;
}

static int open_spi_driver(SPI_INDEX spi_index, SPI_DEVICE_INDEX spi_dev)
{
    char *spi_route = NULL;
    int ret = 0;

    switch (spi_index)
    {
    case SPI_INDEX_0_0:
        spi_route = SPI_ROUTE_0_0;
        break;

    case SPI_INDEX_0_1:
        spi_route = SPI_ROUTE_0_1;
        break;

    case SPI_INDEX_1_0:
        spi_route = SPI_ROUTE_1_0;
        break;

    case SPI_INDEX_1_1:
        spi_route = SPI_ROUTE_1_1;
        break;
        
    case SPI_INDEX_2_0:
        spi_route = SPI_ROUTE_2_0;
        break;

    case SPI_INDEX_2_1:
        spi_route = SPI_ROUTE_2_1;
        break;

    default:
        break;
    }

    ret = open(spi_route, O_RDWR);
    if(ret < 0)
    {
        perror("fail to open spi");
        return ret;
    }

    spi[spi_dev].fd = ret;

    ret = sem_init(&spi[spi_dev].sem, 0, 1);
    if(ret < 0)
    {
        perror("fail to init semaphore");
        return ret;
    }

    return 1;
}

int reset_spi_frequency(SPI_DEVICE_INDEX spi_dev, int freq)
{
    int ret = 0;

    ret = ioctl(spi[spi_dev].fd, SPI_IOC_WR_MAX_SPEED_HZ, &freq);
    if(ret < 0)
    {
        perror("fail to set spi clock");
    }

    return ret;
}

int init_spi()
{
    spi_init_t spi_init_val = {0, };

    spi_init_val.bit = SPI_IOCTL_8BIT;
    spi_init_val.speed = SPI_CLOCK_VS1003;
    spi_init_val.mode = SPI_MODE_0;
    spi_init_val.lsb = SPI_IOCTL_MSB_FIRST;

    open_spi_driver(SPI_INDEX_0_0, SPI_DEV_VS1003);
    set_spi_ioctl(SPI_DEV_VS1003, &spi_init_val);
}

int send_recv_spi_full_duplex(SPI_DEVICE_INDEX spi_dev, uint8_t *recv, uint8_t *send, int max_buffer_size)
{
    int ret = 0;

    struct spi_ioc_transfer spi_msg = {0, };

    spi_msg.rx_buf = recv;
    spi_msg.tx_buf = send;
    spi_msg.len = max_buffer_size;
    
    sem_wait(&spi[spi_dev].sem);
    ret = ioctl(spi[spi_dev].fd, SPI_IOC_MESSAGE(1), &spi_msg);
    sem_post(&spi[spi_dev].sem);
    if(ret < 0)
    {
        perror("fail to ioctl message");
        return -1;
    }

    return 1;
}

int transmit_spi(SPI_DEVICE_INDEX spi_dev, uint8_t *tx, size_t tx_size)
{
    int ret = 0;

    sem_wait(&spi[spi_dev].sem);
    ret = write(spi[spi_dev].fd, tx, tx_size);
    sem_post(&spi[spi_dev].sem);
    if(ret < 0)
    {
        perror("fail to write spi");
        return -1;
    }
}

int receive_spi(SPI_DEVICE_INDEX spi_dev, uint8_t *rx, size_t rx_size)
{
    int ret = 0;

    sem_wait(&spi[spi_dev].sem);
    ret = read(spi[spi_dev].fd, rx, rx_size);
    sem_post(&spi[spi_dev].sem);
    if(ret < 0)
    {
        perror("fail to read spi");
        return -1;
    }
}