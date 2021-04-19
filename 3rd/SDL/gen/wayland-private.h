/*
 * Copyright © 2008-2011 Kristian Høgsberg
 * Copyright © 2011 Intel Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef WAYLAND_PRIVATE_H
#define WAYLAND_PRIVATE_H

#include <stdarg.h>
#include <ffi.h>
#include "wayland-util.h"

#define WL_ZOMBIE_OBJECT ((void *) 2)

#define WL_MAP_SERVER_SIDE 0
#define WL_MAP_CLIENT_SIDE 1
#define WL_SERVER_ID_START 0xff000000

struct wl_map {
	struct wl_array client_entries;
	struct wl_array server_entries;
	uint32_t free_list;
};

typedef void (*wl_iterator_func_t)(void *element, void *data);

void wl_map_init(struct wl_map *map);
void wl_map_release(struct wl_map *map);
uint32_t wl_map_insert_new(struct wl_map *map, uint32_t side, void *data);
int wl_map_insert_at(struct wl_map *map, uint32_t i, void *data);
int wl_map_reserve_new(struct wl_map *map, uint32_t i);
void wl_map_remove(struct wl_map *map, uint32_t i);
void *wl_map_lookup(struct wl_map *map, uint32_t i);
void wl_map_for_each(struct wl_map *map, wl_iterator_func_t func, void *data);

struct wl_connection;
struct wl_closure;

#define WL_CONNECTION_READABLE 0x01
#define WL_CONNECTION_WRITABLE 0x02

typedef int (*wl_connection_update_func_t)(struct wl_connection *connection,
					   uint32_t mask, void *data);

struct wl_connection *wl_connection_create(int fd,
					   wl_connection_update_func_t update,
					   void *data);
void wl_connection_destroy(struct wl_connection *connection);
void wl_connection_copy(struct wl_connection *connection, void *data, size_t size);
void wl_connection_consume(struct wl_connection *connection, size_t size);
int wl_connection_data(struct wl_connection *connection, uint32_t mask);
int wl_connection_write(struct wl_connection *connection, const void *data, size_t count);
int wl_connection_queue(struct wl_connection *connection,
			const void *data, size_t count);

struct wl_closure {
	int count;
	const struct wl_message *message;
	ffi_type *types[20];
	ffi_cif cif;
	void *args[20];
	uint32_t *start;
	uint32_t buffer[0];
};

struct argument_details {
	char type;
	int nullable;
};

const char *
get_next_argument(const char *signature, struct argument_details *details);

int
arg_count_for_signature(const char *signature);

struct wl_closure *
wl_closure_vmarshal(struct wl_object *sender,
		    uint32_t opcode, va_list ap,
		    const struct wl_message *message);

struct wl_closure *
wl_connection_demarshal(struct wl_connection *connection,
			uint32_t size,
			struct wl_map *objects,
			const struct wl_message *message);

void
wl_closure_invoke(struct wl_closure *closure,
		  struct wl_object *target, void (*func)(void), void *data);
int
wl_closure_send(struct wl_closure *closure, struct wl_connection *connection);
int
wl_closure_queue(struct wl_closure *closure, struct wl_connection *connection);
void
wl_closure_print(struct wl_closure *closure, struct wl_object *target, int send);
void
wl_closure_destroy(struct wl_closure *closure);

extern wl_log_func_t wl_log_handler;

void wl_log(const char *fmt, ...);

#endif
