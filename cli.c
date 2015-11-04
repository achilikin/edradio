/*  Simple cli parser
    Copyright (c) 2015 Andrey Chilikin https://github.com/achilikin

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    
	* Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
    
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
	BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
	OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>

#include "main.h"
#include "rfm12bs.h"

static int help_proc(console_io_t *cli, char *arg);
static int cls_proc(console_io_t *cli, char *arg);
static int exit_proc(console_io_t *cli, char *arg);
static int echo_proc(console_io_t *cli, char *arg);
static int sync_proc(console_io_t *cli, char *arg);
static int vbat_proc(console_io_t *cli, char *arg);
static int clock_proc(console_io_t *cli, char *arg);

command_t cli_cmd[] = {
	{ "help", "", help_proc },
	{ "cls", "clear screen", cls_proc },
	{ "exit", "exit application", exit_proc },
	{ "quit", "exit application", exit_proc },
	{ "q", "", exit_proc },
	{ "echo", "echo rx|dan on|off", echo_proc },
	{ "vbat", "check battery level", vbat_proc },
	{ "sync", "send sync request to active base station", sync_proc },
	{ "clock", "off|1|1.25|1.66|2|2.5|5|3.33|5|10, set clock output frequency, MHz", clock_proc },
	{ NULL }
};

int cls_proc(console_io_t *cli, UNUSED(char *arg))
{
	cli->puts(cli, clrs);
	cli->puts(cli, gotop);

	return CLI_EOK;
}

int exit_proc(console_io_t *cli, UNUSED(char *arg))
{
	cli->stop = 1;
	return CLI_EOK;
}

int help_proc(console_io_t *cli, UNUSED(char *arg))
{
	uint32_t len = 0;
	for(int i = 0; cli_cmd[i].cmd != NULL; i++) {
		if (strlen(cli_cmd[i].cmd) > len)
			len = strlen(cli_cmd[i].cmd);
	}

	for(int i = 0; cli_cmd[i].cmd != NULL; i++) {
		if (cli_cmd[i].help && *cli_cmd[i].help)
			dprintf(cli->ofd, "%-*s: %s\n", len, cli_cmd[i].cmd, cli_cmd[i].help);
	}
	return CLI_EOK;
}

int echo_proc(console_io_t *cli, char *arg)
{
	app_cfg_t *cfg = cli->data;

	if (*arg == '\0') {
		dprintf(cli->ofd, "rx   %s\n", (cfg->flags & RFM_RX_DEBUG) ? "on" : "off");
		dprintf(cli->ofd, "node %s\n", (cfg->flags & APPF_ECHO_DAN) ? "on" : "off");
		return CLI_EOK;
	}

	uint32_t flag = 0;

	if (cmd_arg(arg, "rx", &arg))
		flag = RFM_RX_DEBUG;
	if (cmd_arg(arg, "dan", &arg))
		flag = APPF_ECHO_DAN;

	if (flag == 0)
		return CLI_EARG;

	if (cmd_is(arg, "on"))
		cfg->flags |= flag;
	else if (cmd_is(arg, "off"))
		cfg->flags &= ~flag;
	else
		dprintf(cli->ofd, "%s\n", (cfg->flags & flag) ? "on" : "off");

	return CLI_EOK;
}

int sync_proc(console_io_t *cli, UNUSED(char *arg))
{
	app_cfg_t *cfg = cli->data;

	dnode_t node;
	memset(&node, 0, sizeof(node));
	node.nid = NODE_TSYNC | NODE_LBS;

	rfm12_send(cfg->rfm, &node, sizeof(node));
	rfm12_set_mode(cfg->rfm, RFM_MODE_RX);
	return 0;
}

int vbat_proc(console_io_t *cli, UNUSED(char *arg))
{
	app_cfg_t *cfg = cli->data;

	uint16_t vbat = rfm12_battery(cfg->rfm, RFM_MODE_RX, 14);
	vbat = vbat * 10 + 230;
	dprintf(cli->ofd, "Battery: ~%u.%uV\n", vbat/100, vbat % 100);

	return 0;
}

int clock_proc(console_io_t *cli, char *arg)
{
	app_cfg_t *cfg = cli->data;

	if (cmd_is(arg, "off")) {
		cfg->rfm->mode &= ~ RFM_CLOCK_ENABLE;
		rfm12_set_mode(cfg->rfm, RFM_MODE_RX);
		return 0;
	}
	uint16_t cmd_clock = RFM12CMD_LBDCLK;

	static struct clock {
		const char *name;
		uint16_t   clock;
	} valid_clock[] = {
		{"1",    RFM12_CLOCK_1_00MHZ },
		{"1.25", RFM12_CLOCK_1_25MHZ },
		{"1.66", RFM12_CLOCK_1_66MHZ },
		{"2",    RFM12_CLOCK_2_00MHZ },
		{"2.5",  RFM12_CLOCK_2_50MHZ },
		{"3.33", RFM12_CLOCK_3_33MHZ },
		{"5",    RFM12_CLOCK_5_00MHZ },
		{"10",   RFM12_CLOCK_10_0MHZ },
	};

	for(unsigned i = 0; i < sizeof(valid_clock)/sizeof(valid_clock[0]); i++) {
		if (cmd_is(arg, valid_clock[i].name)) {
			cmd_clock |= valid_clock[i].clock;
			break;
		}
	}

	rfm12_cmdw(cfg->rfm, cmd_clock);
	cfg->rfm->mode |= RFM_CLOCK_ENABLE;
	rfm12_set_mode(cfg->rfm, RFM_MODE_RX);

	return 0;
}

