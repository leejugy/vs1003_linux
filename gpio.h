#ifndef __GPIO__
#define __GPIO__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>

#define err(fmt, ...) printf("[\x1b[31m%s\x1b[0m]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define debug(fmt, ...) printf("[\x1b[33m%s\x1b[0m]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)

#define GPIO_CHAR_DEVICE_ROUTE "/dev/gpiochip"

#define GPIO_CHIP0_ROUTE GPIO_CHAR_DEVICE_ROUTE "0"
#define GPIO_CHIP1_ROUTE GPIO_CHAR_DEVICE_ROUTE "1"
#define GPIO_CHIP2_ROUTE GPIO_CHAR_DEVICE_ROUTE "2"
#define GPIO_CHIP3_ROUTE GPIO_CHAR_DEVICE_ROUTE "3"
#define GPIO_CHIP4_ROUTE GPIO_CHAR_DEVICE_ROUTE "4"

#define GPIO_IN_DREQ_NAME "READ_VALUE"
#define GPIO_OUT_XCS_NAME "XCS"
#define GPIO_OUT_XDCS_NAME "XDCS"
#define GPIO_OUT_XRST_NAME "XRST"

typedef enum
{
    GPIO_CHIP0,
    GPIO_CHIP1,
    GPIO_CHIP2,
    GPIO_CHIP3,
    GPIO_CHIP4
} GPIO_CHIP_NUMBER;

#define GPIO_IN_DREQ_OFFSET 12
#define GPIO_OUT_XCS_OFFSET 13
#define GPIO_OUT_XDCS_OFFSET 14
#define GPIO_OUT_XRST_OFFSET 15

typedef enum
{
    GPIO_INDEX_IN_DREQ,
    GPIO_INDEX_OUT_XCS,
    GPIO_INDEX_OUT_XDCS,
    GPIO_INDEX_OUT_XRST,
    GPIO_INDEX_MAX_NUMBER
} GPIO_INDEX;

typedef enum
{
    GPIO_LOW,
    GPIO_HIGH,
} GPIO_VALUES;

#define SET_GPIO_XCS_HIGH set_gpio_value(GPIO_INDEX_OUT_XCS, GPIO_HIGH)
#define SET_GPIO_XCS_LOW set_gpio_value(GPIO_INDEX_OUT_XCS, GPIO_LOW)

#define SET_GPIO_XDCS_HIGH set_gpio_value(GPIO_INDEX_OUT_XDCS, GPIO_HIGH)
#define SET_GPIO_XDCS_LOW set_gpio_value(GPIO_INDEX_OUT_XDCS, GPIO_LOW)

#define SET_GPIO_XRST_HIGH set_gpio_value(GPIO_INDEX_OUT_XRST, GPIO_HIGH)
#define SET_GPIO_XRST_LOW set_gpio_value(GPIO_INDEX_OUT_XRST, GPIO_LOW)

#define GET_DREQ get_gpio_value(GPIO_INDEX_IN_DREQ)

#endif
