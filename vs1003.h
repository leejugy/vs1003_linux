#ifndef __VS1003__
#define __VS1003__

#define VS1003_READ_BIT 0x03
#define VS1003_WRITE_BIT 0x02

#define FULL_DUPLEX_BUFFER_SIZE 4
#define GET_COMMAND_BUFFER_SIZE 2
#define SET_COMMAND_BUFFER_SIZE 4
#define SET_COMMAND_DATA_SIZE 2
#define MAX_VOLUME_DECREASE 254

#define GET_VOLUME_PERCENT(volume) ((MAX_VOLUME_DECREASE - volume)*100)/MAX_VOLUME_DECREASE

#define VS1003_THREAD_NUM 2

#include "define.h"

typedef enum
{
    VS1003_SCI_MODE,
    VS1003_SCI_STATUS,
    VS1003_SCI_BASS,
    VS1003_SCI_CLOCK_FREQUNECY,
    VS1003_SCI_DECODE_TIME,
    VS1003_SCI_AUDIO_DATA,
    VS1003_SCI_RAM,
    VS1003_SCI_RAM_ADDR,
    VS1003_SCI_HEADER_0,
    VS1003_SCI_HEADER_1,
    VS1003_SCI_APP_START_ADDR,
    VS1003_SCI_VOLUME,
    VS1003_SCI_APP_CONTROL_REG0,
    VS1003_SCI_APP_CONTROL_REG1,
    VS1003_SCI_APP_CONTROL_REG2,
    VS1003_SCI_APP_CONTROL_REG3,
}VS1003_SCI_REG;


typedef enum
{
    VS1003_MUSIC_NONE,
    VS1003_MUSIC_LIST1,
    VS1003_MUSIC_LIST2,
    VS1003_MUSIC_LIST3,
    VS1003_MUSIC_PAUSE,
    VS1003_MUSIC_RESUME,
    VS1003_MUSIC_RESET,
    VS1003_MUSIC_VOLUME_INFO,
    VS1003_MUSIC_VOLUME_UP,
    VS1003_MUSIC_VOLUME_DOWN,
}VS1003_MUSIC_COMMAND;

typedef enum
{
    VS1003_MP3_ERR = -1,
    VS1003_MP3_END_MUSIC = 0,
    VS1003_MP3_CONTINOUS_PLAYING = 1,
}VS1003_MP3_STATUS;

typedef enum
{
    VS1003_MUSIC_RESET_DO,
    VS1003_MUSIC_RESET_NONE,
}VS1003_RESET_FLAG;

void start_vs1003_thread();

#endif