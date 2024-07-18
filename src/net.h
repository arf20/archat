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

#include <stdint.h>

#include <arpa/inet.h>

#define PORT    42069
#define GROUP   "239.255.42.69"

#define MAGIC   0x42069cac

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
int recv_message(const header_t **header, const char **data, struct sockaddr_in *addr);
int send_ping(uint32_t uid);
int send_pong(uint32_t uid, int16_t rid, const char *nick, const char *hname,
    const char *rname);
int send_join(uint32_t uid, uint16_t rid, const char *rname);
int send_rmsg(uint32_t uid, uint16_t rid, const char *msg);


#endif /* _NET_H */

