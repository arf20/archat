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

    db.c: Local tracking of users and rooms
*/

#include "db.h"

#include <stdlib.h>
#include <string.h>

void
user_list_push(user_node_t *l, uint32_t uid, const char *nick, uint16_t rid)
{
    for (user_node_t *i = l->next; i != NULL; i = i->next)
        if (i->uid == uid) {
            i->nick = nick;
            i->rid = rid;
            return;
        }
    
    /* Last node */
    user_node_t *i = l;
    while (i->next) { i = i->next; };

    /* Create node */
    i->next = malloc(sizeof(user_node_t));
    i->next->uid = uid;
    i->next->nick = nick;
    i->next->rid = rid;
    i->next->next = NULL;
}

void
room_list_push(room_node_t *l, uint16_t rid, const char *rname)
{
    for (room_node_t *i = l->next; i != NULL; i = i->next)
        if (i->rid == rid) {
            free(i->rname);
            i->rname = strdup(rname);
            return;
        }
    
    /* Last node */
    room_node_t *i = l;
    while (i->next) { i = i->next; };

    /* Create node */
    i->next = malloc(sizeof(room_node_t));
        i->next->rid = rid;
    i->next->rname = strdup(rname);
    i->next->next = NULL;
}
