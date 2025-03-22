#include "vs1003.h"
#include "gpio.h"
#include "spi_ctl.h"
#include "audio.h"

static int wait_dreq_high()
{
    int i = 0;
    GPIO_VALUES gpio_val = GPIO_NONE;

    for (i = 0; i < 10000; i++)
    {
        GET_DREQ(&gpio_val);
        if (gpio_val == GPIO_HIGH)
        {
            return 1;
        }
    }

    FATAL("fail to get gpio value");
    return -1;
}

static void get_command_vs1003_half_duplex(uint8_t reg, uint8_t *reg_data, size_t data_size)
{
    SET_GPIO_XCS_LOW;
    SET_GPIO_XDCS_HIGH;

    int tx_index = 0;
    uint8_t tx_buffer[GET_COMMAND_BUFFER_SIZE] = {
        0,
    };

    tx_buffer[tx_index++] = VS1003_READ_BIT;
    tx_buffer[tx_index++] = reg;

    transmit_spi(SPI_DEV_VS1003, tx_buffer, sizeof(tx_buffer));
    receive_spi(SPI_DEV_VS1003, reg_data, data_size);
    wait_dreq_high();

    SET_GPIO_XCS_HIGH;
}

static void set_command_vs1003_half_duplex(uint8_t reg, uint8_t *reg_data, size_t data_size)
{
    SET_GPIO_XCS_LOW;
    SET_GPIO_XDCS_HIGH;

    int tx_index = 0;
    uint8_t tx_buffer[SET_COMMAND_BUFFER_SIZE] = {
        0,
    };

    tx_buffer[tx_index++] = VS1003_WRITE_BIT;
    tx_buffer[tx_index++] = reg;
    memcpy(&tx_buffer[tx_index], reg_data, data_size);

    transmit_spi(SPI_DEV_VS1003, tx_buffer, sizeof(tx_index));
    wait_dreq_high();

    SET_GPIO_XCS_HIGH;
}

static void send_data_vs1003_half_duplex(uint8_t *data, size_t data_size)
{
    SET_GPIO_XDCS_LOW;
    SET_GPIO_XCS_HIGH;

    transmit_spi(SPI_DEV_VS1003, data, data_size);
    wait_dreq_high();

    SET_GPIO_XDCS_HIGH;
}

static void print_vs1003_reg_half_duplex(uint8_t reg)
{
    uint8_t reg_data[GET_COMMAND_BUFFER_SIZE] = {
        0,
    };
    get_command_vs1003_half_duplex(reg, reg_data, sizeof(reg_data));
    INFO("0x%02x : 0x%02x%02x", reg, reg_data[0], reg_data[1]);
}

static void set_vs1003_audio_config(char *music_route)
{
    mp3_header_t mp3_header = {
        0,
    };
    mp3_id_tag_t mp3_id = {
        0,
    };

    MP3_SAMPLE_RATE sample_rate = 0;
    MP3_TYPE type = 0;

    uint16_t vs1003_audio_data = 0;
    uint8_t send_data[SET_COMMAND_DATA_SIZE] = {
        0,
    };

    int spi_freq = 0;
    int send_index = 0;

    if (strstr(music_route, ".mp3") == NULL)
    {
        FATAL("not a mp3 file!");
        return;
    }

    read_mp3_header(music_route, &mp3_id, &mp3_header);

    sample_rate = get_sample_rate(&mp3_header);
    if (sample_rate < 0)
    {
        FATAL("fail to get sample rate");
        return;
    }

    type = get_auido_mode(&mp3_header);
    if (type < 0)
    {
        FATAL("fail to get audio mode");
        return;
    }

    spi_freq = get_bitrate(&mp3_header) * 1000;
    if (spi_freq < 0)
    {
        FATAL("fail to get bitrate");
        return;
    }

    vs1003_audio_data = (((sample_rate / 2) % 2) == 0) ? ((sample_rate / 2) << 1) + type : ((sample_rate / 2) << 1) + ((type + 1) % 2);

    send_data[send_index++] = (vs1003_audio_data >> 8) & 0xff;
    send_data[send_index++] = vs1003_audio_data & 0xff;

    set_command_vs1003_half_duplex(VS1003_SCI_AUDIO_DATA, send_data, send_index);

    reset_spi_frequency(SPI_DEV_VS1003, spi_freq);
}

static void set_vs1003_clock_frequency()
{
    uint8_t send_data[SET_COMMAND_DATA_SIZE] = {
        0,
    };
    uint16_t clock_freq = 0x9000;
    int send_index = 0;

    send_data[send_index++] = (clock_freq >> 8) & 0xff;
    send_data[send_index++] = clock_freq & 0xff;

    set_command_vs1003_half_duplex(VS1003_SCI_CLOCK_FREQUNECY, send_data, send_index);
}

static void get_vs1003_volume(uint8_t *reg_data, size_t data_size)
{
    get_command_vs1003_half_duplex(VS1003_SCI_VOLUME, reg_data, data_size);
}

static void set_vs1003_volume(uint8_t *reg_data, size_t data_size)
{
    set_command_vs1003_half_duplex(VS1003_SCI_VOLUME, reg_data, data_size);
}

static void print_vs1003_volume()
{
    uint8_t reg_data[SET_COMMAND_DATA_SIZE] = {
        0,
    };
    get_vs1003_volume(reg_data, sizeof(reg_data));
    INFO("left_volume : %d[%%]", GET_VOLUME_PERCENT(reg_data[0]));
    INFO("right_volume : %d[%%]", GET_VOLUME_PERCENT(reg_data[1]));
}

static void up_vs1003_volume()
{
    uint8_t reg_data[SET_COMMAND_DATA_SIZE] = {
        0,
    };
    int return_flag = 0;
    get_vs1003_volume(reg_data, sizeof(reg_data));

    if (reg_data[0] - 1 < 0)
    {
        FATAL("volume max left");
        return_flag = 1;
    }
    else
    {
        reg_data[0]--;
        INFO("left_volume : %d[%%]", GET_VOLUME_PERCENT(reg_data[0]));
    }

    if (reg_data[1] - 1 < 0)
    {
        FATAL("volume max right");
        return_flag = 1;
    }
    else
    {
        reg_data[1]--;
        INFO("right_volume : %d[%%]", GET_VOLUME_PERCENT(reg_data[1]));
    }

    if (return_flag == 1)
    {
        return;
    }
    else
    {
        set_vs1003_volume(reg_data, sizeof(reg_data));
    }
}

static void down_vs1003_volume()
{
    uint8_t reg_data[SET_COMMAND_DATA_SIZE] = {
        0,
    };
    int return_flag = 0;
    get_vs1003_volume(reg_data, sizeof(reg_data));

    if (reg_data[0] + 1 > MAX_VOLUME_DECREASE)
    {
        FATAL("volume max left");
        return_flag = 1;
    }
    else
    {
        reg_data[0]++;
        INFO("left_volume : %d[%%]", GET_VOLUME_PERCENT(reg_data[0]));
    }

    if (reg_data[1] + 1 > MAX_VOLUME_DECREASE)
    {
        FATAL("volume max right");
        return_flag = 1;
    }
    else
    {
        reg_data[1]++;
        INFO("right_volume : %d[%%]", GET_VOLUME_PERCENT(reg_data[1]));
    }

    if (return_flag == 1)
    {
        return;
    }
    else
    {
        set_vs1003_volume(reg_data, sizeof(reg_data));
    }
}

static int play_vs1003_mp3_music(char *music_route)
{
    int ret = 0;
    int fd = 0;
    uint8_t send_buffer[1024] = {
        0,
    };

    set_vs1003_audio_config(music_route);

    ret = open(music_route, O_RDONLY);
    if (ret < 0)
    {
        perror("fail to open music");
        return ret;
    }

    fd = ret;

    while (1)
    {
        ret = read(fd, send_buffer, sizeof(send_buffer));
        if (ret < 0)
        {
            perror("fail to read");
            reset_spi_frequency(SPI_DEV_VS1003, SPI_CLOCK_VS1003);
            return ret;
        }

        send_data_vs1003_half_duplex(send_buffer, ret);
    }

    return -1;
}

#ifdef FULL_DUPLEX
static void get_command_vs1003_full_duplex(uint8_t reg, uint8_t *reg_data, size_t data_size)
{
    SET_GPIO_XCS_LOW;
    SET_GPIO_XDCS_HIGH;

    int tx_index = 0;
    uint8_t tx_buffer[FULL_DUPLEX_BUFFER_SIZE] = {
        0,
    };

    tx_buffer[tx_index++] = VS1003_READ_BIT;
    tx_buffer[tx_index++] = reg;

    send_recv_spi_full_duplex(SPI_DEV_VS1003, reg_data, tx_buffer, data_size);
    wait_dreq_high();

    SET_GPIO_XCS_HIGH;
}

static void set_command_vs1003_full_duplex(uint8_t reg, uint8_t *reg_data, size_t data_size)
{
    SET_GPIO_XCS_LOW;
    SET_GPIO_XDCS_HIGH;

    int tx_index = 0;
    uint8_t tx_buffer[SET_COMMAND_BUFFER_SIZE] = {
        0,
    };

    tx_buffer[tx_index++] = VS1003_WRITE_BIT;
    tx_buffer[tx_index++] = reg;
    memcpy(&tx_buffer[tx_index], reg_data, data_size);

    send_recv_spi_full_duplex(SPI_DEV_VS1003, tx_buffer, tx_buffer, sizeof(tx_index));
    wait_dreq_high();

    SET_GPIO_XCS_HIGH;
}

static void send_data_vs1003_full_duplex(uint8_t *data, size_t data_size)
{
    SET_GPIO_XDCS_LOW;
    SET_GPIO_XCS_HIGH;

    send_recv_spi_full_duplex(SPI_DEV_VS1003, data, data, data_size);
    wait_dreq_high();

    SET_GPIO_XDCS_HIGH;
}

static void print_vs1003_reg_full_duplex(uint8_t reg)
{
    uint8_t reg_data[FULL_DUPLEX_BUFFER_SIZE] = {
        0,
    };
    get_command_vs1003_full_duplex(reg, reg_data, sizeof(reg_data));
    INFO("0x%02x : 0x%02x%02x", reg, reg_data[2], reg_data[3]);
}
#endif

static void init_vs1003()
{
    SET_GPIO_XCS_HIGH;
    SET_GPIO_XDCS_HIGH;
    SET_GPIO_XRST_HIGH;

    // 리셋
    usleep(100 * 1000);
    SET_GPIO_XRST_LOW;
    usleep(100 * 1000);
    SET_GPIO_XRST_HIGH;
    wait_dreq_high();

    init_spi();
}

static int select_music()
{
    int ret = 0;
    printf("===================\n");
    printf("1. Mesmerize.mp3\n");
    printf("2. Places_Like_That.mp3\n");
    printf("3. Sunburst.mp3\n");
    printf("4. volume info\n");
    printf("5. up volume\n");
    printf("6. down volume\n");
    printf("===================\n");
    printf("select music:");
    scanf("%d", &ret);
    return ret;
}

static void thread_vs1003()
{
    set_vs1003_clock_frequency();
    print_vs1003_reg_half_duplex(VS1003_SCI_AUDIO_DATA);
    print_vs1003_reg_half_duplex(VS1003_SCI_VOLUME);
    print_vs1003_reg_half_duplex(VS1003_SCI_CLOCK_FREQUNECY);

    while (1)
    {
        switch (select_music())
        {
        case VS1003_MUSIC_LIST1:
            play_vs1003_mp3_music("./Mesmerize.mp3");
            break;
        case VS1003_MUSIC_LIST2:
            play_vs1003_mp3_music("./Places_Like_That.mp3");
            break;
        case VS1003_MUSIC_LIST3:
            play_vs1003_mp3_music("./Sunburst.mp3");
            break;

        case VS1003_MUSIC_VOLUME_INFO:
            print_vs1003_volume();
            break;

        case VS1003_MUSIC_VOLUME_UP:
            up_vs1003_volume();
            break;

        case VS1003_MUSIC_VOLUME_DOWN:
            down_vs1003_volume();
            break;

        default:
            break;
        }
    }
}

void start_vs1003_thread()
{
    pthread_t tid = 0;
    int ret = 0;

    init_gpio();
    init_vs1003();

    ret = pthread_create(&tid, NULL, (void *)&thread_vs1003, NULL);
    if (ret < 0)
    {
        perror("fail to create vs1003 thread");
        return;
    }

    ret = pthread_detach(tid);
    if (ret < 0)
    {
        perror("fail to detach vs1003");
    }
}
