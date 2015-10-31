/*
  Example of using RFM12BS RF module on Intel Edison with software SPI as an
  Listening Base Station for shDAN project https://github.com/achilikin/shDAN
  
  Software SPI implemented using memory mapped io with mraa library
  https://github.com/intel-iot-devkit/mraa
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <unistd.h>

#include "main.h"
#include "edtsc.h"
#include "swspi.h"
#include "rfm12bs.h"
#include "mraa_pin.h"

app_cfg_t cfg;

void print_node(console_io_t *cli, dnode_t *node);

int	main(UNUSED(int argc), UNUSED(char** argv))
{
	uint32_t tso = rdtsc32();

	stdio_mode(STDIO_MODE_CANON);

	// initialize mraa and software SPI
	mraa_init();
	printf("MRAA paltform %s, version %s\n", mraa_get_platform_name(), mraa_get_version());
	mraa_spi_sw_context spi = sw_spi_init(MRAA_SCK, MRAA_MOSI, MRAA_MISO);
	
	// initialize RFM12BS
	rfm12_t rfm;
	memset(&rfm, 0,  sizeof(rfm));
	rfm12_init_spi(&rfm, spi, MRAA_CS1, MRAA_GP047);
	rfm12_init(&rfm, 0xD4, RFM12_BAND_868, 868.0, RFM12_BPS_9600);
	rfm12_set_mode(&rfm, RFM_MODE_RX);

	printf("started up in %u msec\n", millis(tso));

	// default application configuration
	cfg.flags |= APPF_ECHO_DAN;
	cfg.rfm = &rfm;

	// initialize command line interface
	console_io_t cli;
	memset(&cli, 0,  sizeof(cli));
	cli.prompt = '>';
	cli.data = &cfg;

	stdio_init(&cli, stdio_cmd_handler);
	stdio_mode(STDIO_MODE_RAW);

	dnode_t node;

	while(!cli.stop) {
		cli.interact(&cli, cli_cmd);

		if (rfm12_receive_data(&rfm, &node, sizeof(node), cfg.flags & RFM_RX_DEBUG) == sizeof(node)) {
			rfm12_set_mode(&rfm, RFM_MODE_RX);
			if (node.nid == NODE_TSYNC) {
				tso = rdtsc32();
				ts_unpack(&node);
				cfg.rtc_hour = node.hour;
				cfg.rtc_min = node.min;
				cfg.rtc_sec = node.sec;
			}
			if (cfg.flags & APPF_ECHO_DAN)
				print_node(&cli, &node);
		}

		if (millis(tso) >= 1000) {
			tso = rdtsc32();
			if (++cfg.rtc_sec == 60) {
				cfg.rtc_sec = 0;
				if (++cfg.rtc_min == 60) {
					cfg.rtc_min = 0;
					if (++cfg.rtc_hour == 24)
						cfg.rtc_hour = 0;
				}
			}
		}
		usleep(1); // be nice 
	}

	stdio_mode(STDIO_MODE_CANON);
	rfm12_close_spi(&rfm);
	sw_spi_close(spi);
	mraa_deinit();
}

void print_node(console_io_t *cli, dnode_t *node)
{
	if (node->nid == 0)
		return;

	app_cfg_t *cfg = cli->data;

	dprintf(cli->ofd, " %02u:%02u:%02u | ", cfg->rtc_hour, cfg->rtc_min, cfg->rtc_sec);

	if (node->nid == NODE_TSYNC) {
		dprintf(cli->ofd, "TSYNC: %02u:%02u:%02u\n", node->hour, node->min, node->sec);
		return;
	}

	for(uint8_t i = 0; i < 4; i++) {
		uint8_t *pu8 = (uint8_t *)node;
		dprintf(cli->ofd, "%02X ", pu8[i]);
	}

	dprintf(cli->ofd, "| NID %u SID %u ", node->nid & NID_MASK, (node->nid & SENS_MASK) >> 4);

	if ((node->nid & SENS_MASK) == SENS_LIST) {
		for(uint8_t i = 1; i <= MAX_SENSORS; i++)
			dprintf(cli->ofd, "%02u ", get_sens_type(node, i));
	}
	else {
		unsigned vbat = (node->stat & STAT_VBAT) * 10 + 230;
		dprintf(cli->ofd, "S%u L%u A%u E%u V %u.%u T%+3d.%02d",
			!!(node->stat & STAT_SLEEP), !!(node->stat & STAT_LED),
			!!(node->stat & STAT_ACK), !!(node->stat & STAT_EOS),
			vbat/100, vbat%100, node->val, node->dec);
	}
	dprintf(cli->ofd, "\n");
}
