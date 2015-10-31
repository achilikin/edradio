#ifndef APP_MAIN_HEADER
#define APP_MAIN_HEADER

#include <stdint.h>

#include "cio.h"
#include "cmd.h"
#include "dnode.h"
#include "rfm12bs.h"

#define APPF_ECHO_DAN 0x0100

typedef struct app_config_s {
	uint32_t flags; // lower 8 bits are RFM12BS flags
	rfm12_t  *rfm;  // rfm12 configuration

	uint8_t rtc_hour; // realtime clock from active base station
	uint8_t rtc_min;
	uint8_t rtc_sec;
} app_cfg_t;


extern command_t cli_cmd[];
#endif
