#ifndef __DEFINE__
#define __DEFINE__

#include <stdio.h>

#define FATAL(fmt, ...) printf("[\x1b[31m%s\x1b[0m]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define DEBUG(fmt, ...) printf("[\x1b[35m%s\x1b[0m]" fmt "\n", __FUNCTION__, ##__VA_ARGS__)
#define INFO(fmt, ...) printf("[\x1b[32minfo\x1b[0m]" fmt "\n", ##__VA_ARGS__)

#endif