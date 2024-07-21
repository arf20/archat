/*
    arfchat: Local and LAN chat application
    Copyright (C) 2024 Angel Ruiz Fernandez <arf20>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _NET_H
#define _NET_H

#include "config.h"

#include <stdint.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>

typedef enum {
    TYPE_NOP,
    TYPE_PING,
    TYPE_PONG,
    TYPE_JOIN,
    TYPE_RMSG
} type_t;

typedef struct {
    uint32_t _magic;
    type_t type;
    uint8_t flags;
    uint16_t len;
    uint32_t s_uid;
} header_t;

int create_sockets();
void destroy_sockets();
int recv_message(const header_t **header, const char **data, struct sockaddr_in *addr);
int relay_packet(const void *buff, size_t size, struct sockaddr_in *addr);

#endif /* _NET_H */
