#ifndef __AUDIO__
#define __AUDIO__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "define.h"

#define GET1_BIT(byte, shift) ((byte >> shift) & 0x01)
#define GET2_BIT(byte, shift) ((byte >> shift) & 0x03)
#define GET3_BIT(byte, shift) ((byte >> shift) & 0x07)
#define GET4_BIT(byte, shift) ((byte >> shift) & 0x0f)

typedef enum
{
    MP3_STEREO,
    MP3_JOINT_STEREO,
    MP3_2_MONO, //dual channel
    MP3_1_MONO,
} MP3_MODE;

typedef enum
{
    MP3_TYPE_MONO,
    MP3_TYPE_STEREO
} MP3_TYPE;

typedef enum
{
    MPG_VERSION_2_5,
    MPG_RESERVED,
    MPG_VERSION_2_0,
    MPG_VERSION_1_0,
} MP3_MPG_VERSION;

typedef enum
{
    SAMPLE_BIT0,
    SAMPLE_BIT1,
    SAMPLE_BIT2,
    SAMPLE_RESERVED,
} MP3_SAMPLE_RATE_BIT;

typedef enum
{
    BITRATE_BIT0,
    BITRATE_BIT1,
    BITRATE_BIT2,
    BITRATE_BIT3,
    BITRATE_BIT4,
    BITRATE_BIT5,
    BITRATE_BIT6,
    BITRATE_BIT7,
    BITRATE_BIT8,
    BITRATE_BIT9,
    BITRATE_BIT10,
    BITRATE_BIT11,
    BITRATE_BIT12,
    BITRATE_BIT13,
    BITRATE_BIT14,
    BITRATE_BIT15,
} MP3_BITRATE_BIT;

typedef enum
{
    SAMPLE_RATE_ID3_BIT_0 = 44100,
    SAMPLE_RATE_ID3_BIT_1 = 48000,
    SAMPLE_RATE_ID3_BIT_2 = 32000,

    SAMPLE_RATE_ID2_BIT_0 = 16000,
    SAMPLE_RATE_ID2_BIT_1 = 24000,
    SAMPLE_RATE_ID2_BIT_2 = 12000,

    SAMPLE_RATE_ID1_BIT_0 = 8000,
    SAMPLE_RATE_ID1_BIT_1 = 12000,
    SAMPLE_RATE_ID1_BIT_2 = 11025,
} MP3_SAMPLE_RATE;

typedef struct
{
    uint8_t reserved : 4;
    uint8_t footer_present : 1;
    uint8_t experimental_indicator : 1;
    uint8_t extended_header : 1;
    uint8_t unsynchronization : 1;
} mp3_id_attr_t;

typedef struct
{
    uint8_t id_version[3];
    uint8_t major_version;
    uint8_t minor_version;
    mp3_id_attr_t attr;
    uint8_t size[4];
} mp3_id_tag_t;

typedef struct
{
    uint16_t syncword;
    uint8_t id;
    uint8_t layer;
    uint8_t protect_bit;
    uint8_t bitrate;
    uint8_t sample_rate;
    uint8_t pad_bit;
    uint8_t private_bit;
    uint8_t mode;
    uint8_t extension;
    uint8_t copyright;
    uint8_t original;
    uint8_t emphasis;
} mp3_header_t;

int read_mp3_header(char *mp3_file_route, mp3_id_tag_t *mp3_id, mp3_header_t *mp3_header);
int get_sample_rate(mp3_header_t *mp3_header);
int get_auido_mode(mp3_header_t *mp3_header);
int get_bitrate(mp3_header_t *mp3_header);
#endif
