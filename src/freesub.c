/*
 * Copyright (C) 2014 Cisco Systems, Inc.
 *
 * Author: Luka Perkov <luka.perkov@sartura.hr>
 *
 * freesub is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * You should have received a copy of the GNU General Public License along with
 * freesub. If not, see <http://www.gnu.org/licenses/>.
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libubox/uloop.h>
#include <libubox/usock.h>
#include <libubox/ustream.h>

#include "freesub.h"

struct ustream_fd us_std;
struct ustream_fd us_tcp;

static void notify_state()
{
	ustream_free(&us_std.stream);
	close(us_std.fd.fd);

	ustream_free(&us_tcp.stream);
	close(us_tcp.fd.fd);

	uloop_end();
}

static void std_notify_state(struct ustream *s)
{
	LOG("connection closed (std)");
	notify_state();
}

static void std_notify_read(struct ustream *s, int bytes)
{
	char *data;
	int len;

	do {
		data = ustream_get_read_buf(s, &len);
		if (!data)
			break;

		ustream_write(&us_tcp.stream, data, len, false);

		ustream_consume(s, len);
	} while (1);
}

static void tcp_notify_state(struct ustream *s)
{
	LOG("connection closed (tcp)");
	notify_state();
}

static void tcp_notify_read(struct ustream *s, int bytes)
{
	char *data;
	int len;

	do {
		data = ustream_get_read_buf(s, &len);
		if (!data)
			break;

		printf("%.*s", len, data);

		ustream_consume(s, len);
	} while (1);
}

static struct option long_options[] = {
	{"host", required_argument, NULL, 'H'},
	{"port", required_argument, NULL, 'P'},
	{"help", required_argument, NULL, 'h'},
	{NULL, 0, NULL, 0}
};

static void usage(void)
{
	printf("%s is a stdin/stdout to/from tcp forwarding tool. Options:\n", PROJECT_NAME);
	printf(" -H host\n");
	printf(" -P port\n");
	printf(" -h help\n");
	printf("Tool is useful for using with SSH subsystems. Some subsystems, such as NETCONF, have predefined values.\n");
}

int main(int argc, char **argv)
{
	int fd_std = STDIN_FILENO, fd_tcp;
	char *host = NULL, *port = NULL;
	char *cmdname;
	int c, rc = 0;

	cmdname = strrchr(*argv, '/');
	if (cmdname)
		cmdname = cmdname + 1;
	else
		cmdname = *argv;

	if (strcmp(cmdname, "netconf") == 0) {
		host = "127.0.0.1";
		port = "830";
	}

	while ((c = getopt_long(argc, argv, "H:P:h", long_options, NULL)) != EOF) {
		switch (c) {
			case 'H':
				host = optarg;
				break;
			case 'P':
				port = optarg;
				break;
			case 'h':
				usage();
				goto exit;
			default:
				break;
		}
	}

	if (!host) {
		usage(); rc = -1;
		goto exit;
	}

	if (!port) {
		usage(); rc = -1;
		goto exit;
	}

	rc = uloop_init();
	if (rc) {
		LOG("uloop init failed");
		goto exit;
	}

	LOG("connecting to %s:%s", host, port);

	fd_tcp = usock(USOCK_TCP | USOCK_NONBLOCK, host, port);
	if (fd_tcp < 0) {
		LOG("failed connection to %s:%s", host, port);
		rc = -1;
		goto exit;
	}

	us_tcp.stream.notify_state = tcp_notify_state;
	us_tcp.stream.notify_read = tcp_notify_read;
	ustream_fd_init(&us_tcp, fd_tcp);

	us_std.stream.notify_state = std_notify_state;
	us_std.stream.notify_read = std_notify_read;
	ustream_fd_init(&us_std, fd_std);

	LOG("connection established, entering main loop");

	/* enter main loop */
	uloop_run();
exit:
	uloop_done();

	return rc;
}
