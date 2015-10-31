/*  Copyright (c) 2015 Andrey Chilikin https://github.com/achilikin

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
#include <string.h>

#include "cio.h"
#include "cmd.h"

int cmd_arg(char *cmd, const char *str, char **arg)
{
	if (!cmd)
		return 0;
	while(*cmd && *cmd <= ' ') cmd++;
	char *end;
	size_t len = strlen(str);
	if (strncmp(cmd, str, len) == 0) {
		end = cmd + len;
		if (*end > ' ')
			return 0;
		if (arg) {
			for(; *end <= ' ' && *end != '\0'; end ++);
			*arg = end;
		}
		return 1;
	}
	return 0;
}

int cmd_is(char *str, const char *is)
{
	if (!str || !is)
		return 0;
	return cmd_arg(str, is, NULL);
}

int stdio_cmd_handler(console_io_t *cli, void *ptr)
{
	char *arg;
	command_t *cmd = (command_t *)ptr;
	cmd_line_t *cl = (cmd_line_t *)cli->cbuf;

	for(unsigned i = 0; cmd[i].proc != NULL; i++) {
		if (cmd_arg(cl->cmd, cmd[i].cmd, &arg))
			return cmd[i].proc(cli, arg);
	}

	return CLI_ENOTSUP;
}
