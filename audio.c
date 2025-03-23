#include "audio.h"

static int convert_header(uint8_t *buffer, mp3_header_t *header)
{
    header->syncword = (buffer[0] << 3) + GET3_BIT(buffer[1], 5);
    header->id = GET2_BIT(buffer[1], 3);
    header->layer = GET2_BIT(buffer[1], 1);
    header->protect_bit = GET1_BIT(buffer[1], 0);

    header->bitrate = GET4_BIT(buffer[2], 4);
    header->sample_rate = GET2_BIT(buffer[2], 2);
    header->pad_bit = GET1_BIT(buffer[2], 1);
    header->private_bit = GET1_BIT(buffer[2], 0);

    header->mode = GET2_BIT(buffer[3], 6);
    header->extension = GET2_BIT(buffer[3], 4);
    header->copyright = GET1_BIT(buffer[3], 3);
    header->original = GET1_BIT(buffer[3], 2);
    header->emphasis = GET2_BIT(buffer[3], 0);
}

int read_mp3_header(char *mp3_file_route, mp3_id_tag_t *mp3_id, mp3_header_t *mp3_header)
{
    int ret = 0;
    int fd = 0;
    uint32_t tag_size = 0;
    uint8_t temp[4] = {
        0,
    };

    ret = open(mp3_file_route, O_RDONLY);
    if (ret < 0)
    {
        perror("fail to open mp3");
    }

    fd = ret;
    ret = read(fd, mp3_id, sizeof(mp3_id_tag_t));
    if (ret < 0)
    {
        perror("fail to read mp3");
        return ret;
    }

    tag_size = (mp3_id->size[0] << 21) + (mp3_id->size[1] << 14) + (mp3_id->size[2] << 7) + mp3_id->size[3];
    lseek(fd, tag_size + sizeof(mp3_id_tag_t), SEEK_SET);

    ret = read(fd, temp, sizeof(temp));
    if (ret < 0)
    {
        perror("fail to read mp3");
        return ret;
    }

    convert_header(temp, mp3_header);
    if (mp3_header->syncword != 0x7ff)
    {
        FATAL("invalid header\n");
        return ret;
    }

    close(fd);
    return 1;
}

int get_sample_rate(mp3_header_t *mp3_header)
{
    int ret = 0;

    switch (mp3_header->id)
    {
    case MPG_VERSION_1_0:
        switch (mp3_header->sample_rate)
        {
        case SAMPLE_BIT0:
            ret = SAMPLE_RATE_ID3_BIT_0;
            break;

        case SAMPLE_BIT1:
            ret = SAMPLE_RATE_ID3_BIT_1;
            break;

        case SAMPLE_BIT2:
            ret = SAMPLE_RATE_ID3_BIT_2;
            break;

        default:
            ret = -1;
            break;
        }
        break;

    case MPG_VERSION_2_0:
        switch (mp3_header->sample_rate)
        {
        case SAMPLE_BIT0:
            ret = SAMPLE_RATE_ID2_BIT_0;
            break;

        case SAMPLE_BIT1:
            ret = SAMPLE_RATE_ID2_BIT_1;
            break;

        case SAMPLE_BIT2:
            ret = SAMPLE_RATE_ID2_BIT_2;
            break;

        default:
            ret = -1;
            break;
        }
        break;

    case MPG_VERSION_2_5:
        switch (mp3_header->sample_rate)
        {
        case SAMPLE_BIT0:
            ret = SAMPLE_RATE_ID1_BIT_0;
            break;

        case SAMPLE_BIT1:
            ret = SAMPLE_RATE_ID1_BIT_1;
            break;

        case SAMPLE_BIT2:
            ret = SAMPLE_RATE_ID1_BIT_2;
            break;

        default:
            ret = -1;
            break;
        }
        break;

    default:
        ret = -1;
        break;
    }

    return ret;
}

int get_auido_mode(mp3_header_t *mp3_header)
{
    int ret = 0;

    switch (mp3_header->mode)
    {
    case MP3_2_MONO:
    case MP3_1_MONO:
        ret = MP3_TYPE_MONO;
        break;

    case MP3_JOINT_STEREO:
    case MP3_STEREO:
        ret = MP3_TYPE_STEREO;
        break;

    default:
        ret = -1;
        break;
    }

    return ret;
}

int get_bitrate(mp3_header_t *mp3_header)
{
    int ret = 0;

    switch (mp3_header->id)
    {
    case MPG_VERSION_1_0:
        switch (mp3_header->bitrate)
        {
        case BITRATE_BIT1:
            ret = 32;
            break;

        case BITRATE_BIT2:
            ret = 40;
            break;

        case BITRATE_BIT3:
            ret = 48;
            break;

        case BITRATE_BIT4:
            ret = 56;
            break;

        case BITRATE_BIT5:
            ret = 64;
            break;

        case BITRATE_BIT6:
            ret = 80;
            break;

        case BITRATE_BIT7:
            ret = 96;
            break;

        case BITRATE_BIT8:
            ret = 112;
            break;

        case BITRATE_BIT9:
            ret = 128;
            break;

        case BITRATE_BIT10:
            ret = 160;
            break;

        case BITRATE_BIT11:
            ret = 192;
            break;

        case BITRATE_BIT12:
            ret = 224;
            break;

        case BITRATE_BIT13:
            ret = 256;
            break;
        case BITRATE_BIT14:
            ret = 320;
            break;

        case BITRATE_BIT0:
        case BITRATE_BIT15:
        default:
            return -1;
        }
        break;

    case MPG_VERSION_2_0:
    case MPG_VERSION_2_5:
        switch (mp3_header->bitrate)
        {
        case BITRATE_BIT1:
            ret = 8;
            break;

        case BITRATE_BIT2:
            ret = 16;
            break;

        case BITRATE_BIT3:
            ret = 24;
            break;

        case BITRATE_BIT4:
            ret = 32;
            break;

        case BITRATE_BIT5:
            ret = 40;
            break;

        case BITRATE_BIT6:
            ret = 48;
            break;

        case BITRATE_BIT7:
            ret = 56;
            break;

        case BITRATE_BIT8:
            ret = 64;
            break;

        case BITRATE_BIT9:
            ret = 80;
            break;

        case BITRATE_BIT10:
            ret = 96;
            break;

        case BITRATE_BIT11:
            ret = 112;
            break;

        case BITRATE_BIT12:
            ret = 128;
            break;

        case BITRATE_BIT13:
            ret = 144;
            break;

        case BITRATE_BIT14:
            ret = 160;
            break;

        case BITRATE_BIT0:
        case BITRATE_BIT15:
        default:
            return -1;
        }
        break;

    default:
        ret = -1;
        break;
    }

    return ret;
}