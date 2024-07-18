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

#ifndef _DB_H
#define _DB_H

#include <stdint.h>

typedef struct user_node_s {
    uint32_t uid;
    const char *nick;
    uint16_t rid;

    struct user_node_s *next;
} user_node_t;

typedef struct room_node_s {
    uint16_t rid;
    const char *rname;
    
    struct room_node_s *next;
} room_node_t;

void user_list_push(user_node_t *l, uint32_t uid, const char *nick,
    uint16_t rid);

void room_list_push(room_node_t *l, uint16_t rid, const char *rname);

#endif /* _DB_H */
