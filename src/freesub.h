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

#ifndef __FREESUB_H__
#define __FREESUB_H__

#define PROJECT_NAME "freesub"

#include <syslog.h>

#define LOG(fmt, ...) do { \
		fprintf(stderr, "%s: "fmt"\n", PROJECT_NAME, ## __VA_ARGS__); \
	} while (0)

#ifndef typeof
#define typeof __typeof
#endif

#ifndef container_of
#define container_of(ptr, type, member) ( \
	(type *)( (char *)ptr - offsetof(type,member) ))
#endif

#ifndef __unused
#define __unused __attribute__((unused))
#endif

#endif /* __FREESUB_H__ */
