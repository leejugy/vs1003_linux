#include "gpio.h"

sem_t gpio_sem;
struct gpio_v2_line_request gpio_req[GPIO_INDEX_MAX_NUMBER] = {
    0,
};
int req_fd[GPIO_INDEX_MAX_NUMBER] = {
    0,
};

static int open_gpio_driver(struct gpio_v2_line_request *gpio_req, char *gpio_route)
{
    int gpio_fd = 0;
    int ret = 0;

    gpio_fd = open(gpio_route, O_RDWR);
    if (gpio_fd < 0)
    {
        perror("fail to open gpio driver");
        return -1;
    }

    ret = ioctl(gpio_fd, GPIO_V2_GET_LINE_IOCTL, gpio_req);
    if (ret < 0)
    {
        perror("fail to get gpio line ioctl");
        close(gpio_fd);
        return -1;
    }

    close(gpio_fd); // gpiochip의 fd는 닫아도 상관없다. gpio 제어는 오로지 gpio_v2_line_request의 fd 맴버를 통해서 이뤄진다.
    return gpio_req->fd;
}

static int open_gpio(GPIO_INDEX gpio_name)
{
    char *gpio_route = NULL;
    switch (gpio_name)
    {

    case GPIO_INDEX_IN_DREQ:
        gpio_route = GPIO_CHIP0_ROUTE;                              // gpiochip 0 선택
        gpio_req[gpio_name].offsets[0] = GPIO_IN_DREQ_OFFSET;       // gpiochip 0의 gpioline 12번을 설정할 것임
        gpio_req[gpio_name].num_lines = 1;                          // gpio 설정할 라인의 수는 1개
        gpio_req[gpio_name].config.flags = GPIO_V2_LINE_FLAG_INPUT; // gpioline의 속성은 input 즉, gpiochip0의 12번라인을 input으로
        gpio_req[gpio_name].config.num_attrs = 0;                   // 속성을 단체로 설정하지는 않을거임
        sprintf(gpio_req[gpio_name].consumer, GPIO_IN_DREQ_NAME);   // 사용자는 GPIO_IN_DREQ_NAME ("READ_VALUE")
        break;

    case GPIO_INDEX_OUT_XCS:
        gpio_route = GPIO_CHIP0_ROUTE;                               // gpiochip 0 선택
        gpio_req[gpio_name].offsets[0] = GPIO_OUT_XCS_OFFSET;        // gpiochip 0의 gpioline 13번을 설정할 것임
        gpio_req[gpio_name].num_lines = 1;                           // gpio 설정할 라인의 수는 1개
        gpio_req[gpio_name].config.flags = GPIO_V2_LINE_FLAG_OUTPUT; // gpioline의 속성은 output 즉, gpiochip0의 13번라인을 output으로
        gpio_req[gpio_name].config.num_attrs = 0;                    // 속성을 단체로 설정하지는 않을거임
        sprintf(gpio_req[gpio_name].consumer, GPIO_OUT_XCS_NAME);    // 사용자는 GPIO_OUT_XCS_NAME ("XCS")
        break;

    case GPIO_INDEX_OUT_XDCS:
        gpio_route = GPIO_CHIP0_ROUTE;                               // gpiochip 0 선택
        gpio_req[gpio_name].offsets[0] = GPIO_OUT_XDCS_OFFSET;        // gpiochip 0의 gpioline 14번을 설정할 것임
        gpio_req[gpio_name].num_lines = 1;                           // gpio 설정할 라인의 수는 1개
        gpio_req[gpio_name].config.flags = GPIO_V2_LINE_FLAG_OUTPUT; // gpioline의 속성은 output 즉, gpiochip0의 14번라인을 output으로
        gpio_req[gpio_name].config.num_attrs = 0;                    // 속성을 단체로 설정하지는 않을거임
        sprintf(gpio_req[gpio_name].consumer, GPIO_OUT_XDCS_NAME);   // 사용자는 GPIO_OUT_XDCS_NAME ("XDCS")
        break;

    case GPIO_INDEX_OUT_XRST:
        gpio_route = GPIO_CHIP0_ROUTE;                               // gpiochip 0 선택
        gpio_req[gpio_name].offsets[0] = GPIO_OUT_XRST_OFFSET;        // gpiochip 0의 gpioline 15번을 설정할 것임
        gpio_req[gpio_name].num_lines = 1;                           // gpio 설정할 라인의 수는 1개
        gpio_req[gpio_name].config.flags = GPIO_V2_LINE_FLAG_OUTPUT; // gpioline의 속성은 output 즉, gpiochip0의 15번라인을 output으로
        gpio_req[gpio_name].config.num_attrs = 0;                    // 속성을 단체로 설정하지는 않을거임
        sprintf(gpio_req[gpio_name].consumer, GPIO_OUT_XRST_NAME);   // 사용자는 GPIO_OUT_XRST_NAME ("XRST")
        break;

    default:
        break;
    }

    req_fd[gpio_name] = open_gpio_driver(&gpio_req[gpio_name], gpio_route);
    if (req_fd[gpio_name] < 0)
    {
        err("fail to set gpio");
        return req_fd[gpio_name];
    }

    return 1;
}

static int init_gpio()
{
    int ret = 0;
    int i = 0;

    ret = sem_init(&gpio_sem, 0, 1);
    if (ret < 0)
    {
        perror("fail to init semaphore");
        return ret;
    }

    for (i = 0; i < GPIO_INDEX_MAX_NUMBER; i++)
    {
        ret = open_gpio(i);
        if (ret < 0)
        {
            return ret;
        }
    }

    return 1;
}

static int set_gpio_value(GPIO_INDEX gpio_name, GPIO_VALUES gpio_set)
{
    int ret = 0;
    struct gpio_v2_line_values gpio_val = {
        0,
    };

    gpio_val.bits = gpio_set; // 1bit만 설정
    gpio_val.mask = 1;        // 1비트 마스크, 최하위 비트 하나만 씀을 의미

    sem_wait(&gpio_sem);
    ret = ioctl(req_fd[gpio_name], GPIO_V2_LINE_SET_VALUES_IOCTL, &gpio_val);
    sem_post(&gpio_sem);

    if (ret < 0)
    {
        perror("fail to get ioctl");
        return ret;
    }

    return 1;
}

static int get_gpio_value(GPIO_INDEX gpio_name, GPIO_VALUES *gpio_get)
{
    int ret = 0;
    struct gpio_v2_line_values gpio_val = {
        0,
    };

    gpio_val.mask = 0x01; // 1비트 마스크, 최하위 비트 하나만 씀을 의미

    sem_wait(&gpio_sem);
    ret = ioctl(req_fd[gpio_name], GPIO_V2_LINE_GET_VALUES_IOCTL, &gpio_val);
    sem_post(&gpio_sem);
    if (ret < 0)
    {
        perror("fail to get ioctl");
        return ret;
    }

    *gpio_get = gpio_val.bits & gpio_val.mask; // 최하위 비트 마스킹

    return 1;
}