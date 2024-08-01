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

    main.c: Application entry point
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "net.h"
#include "db.h"


struct termios saved_tattr = { };

void
prompt_input(char *buff, size_t n)
{
    char c;
    int i = 0;
    while ((c = getchar()) != '\n' && i < n - 2) {
        if (c == -1) continue;
        if (c == 127) {
            i--;
            printf("\b \b");
            continue;
        }
        putchar(c);
        buff[i] = c;
        i++;
    }
    buff[i] = '\0';
    putchar('\n');
}

int
main(int argc, char **argv)
{
    printf("arfchatd  Copyright (C) 2024  Angel Ruiz Fernandez <arf20>\n"
        "This program comes with ABSOLUTELY NO WARRANTY\n"
        "This is free software, and you are welcome to redistribute it\n"
        "under certain conditions.\n\n");

    int debug = 0;
    for (int i = 1; i < argc; i++)
        if (strcmp(argv[i], "-d") == 0)
            debug = 1;

    /* DB */
    user_node_t *user_list = malloc(sizeof(user_node_t));
    user_list->next = NULL;
    room_node_t *room_list = malloc(sizeof(room_node_t));
    room_list->next = NULL;

    /* Init */
    if (create_sockets() < 0) {
        printf("create_sockets: %s\n", strerror(errno));
        return 1;
    }

    time_t t_begin = time(NULL);

    int run = 1;
    const header_t *header;
    const char *data;
    struct sockaddr_in s_addr;
    int size = 0;
    while (run) {
        if ((size = recv_message(&header, &data, &s_addr)) < 0) {
            if (errno != EAGAIN) {
                printf("recv_message: %s\n", strerror(errno));
                break;
            }
        } else {
            /* Handle incoming packets */
            switch (header->type) {
                case TYPE_PING: {
                    if (debug)
                        printf("<<PING %d\n", header->s_uid);
                    
                } break;
                case TYPE_PONG: {
                    if (debug)
                        printf("<<PONG %d ", header->s_uid);

                    uint16_t s_rid = *(uint16_t*)data;
                    const char *s_nick = data + 4;
                    const char *s_hname = s_nick + strlen(s_nick) + 1;
                    const char *s_rname = s_hname + strlen(s_hname) + 1;
                    
                    if (debug)
                        printf("%d %s %s %s\n", s_rid, s_nick, s_hname, s_rname);

                    user_list_push(user_list, header->s_uid, s_nick,
                        s_rid, s_addr, s_hname);
                    if (s_rid != 0)
                        room_list_push(room_list, s_rid, s_rname);
                } break;
                case TYPE_JOIN: {
                    if (debug)
                        printf("<<JOIN %d ", header->s_uid);

                    uint16_t s_rid = *(uint16_t*)data;
                    const char *s_rname = data + 4;

                    if (debug)
                        printf("%d %s\n", s_rid, s_rname);

                    uint16_t prev_rid = user_list_get_rid(user_list,+
                        header->s_uid);
                    
                    if (prev_rid != s_rid)
                        printf("%s has left %s\n", user_list_get_nick(user_list,
                            header->s_uid), room_list_get_rname(room_list,
                            prev_rid));

                    if (s_rid == 0) {
                        
                        break;
                    }

                    user_list_set_rid(user_list, header->s_uid, s_rid);
                    room_list_push(room_list, s_rid, s_rname);

                    printf("%s has joined %s\n", user_list_get_nick(user_list,
                        header->s_uid), s_rname);
                } break;
                case TYPE_RMSG: {
                    if (debug)
                        printf("<<RMSG %d ", header->s_uid);

                    uint16_t s_rid = *(uint16_t*)data;
                    const char *s_msg = data + 4;

                    if (debug)
                        printf("%d %s\n", s_rid, s_msg);

                    printf("[%s/%s] %s\n",
                        room_list_get_rname(room_list, s_rid),
                        user_list_get_nick(user_list, header->s_uid),
                        s_msg);
                } break;
            }

            /* Relay packet */
            for (user_node_t *i = user_list->next; i != NULL; i = i->next)
                relay_packet(header, size, &i->addr);
        }
    }

    /* Deinit */
    destroy_sockets();

    for (user_node_t *i = user_list->next; i != NULL;) {
        user_node_t *t = i->next;
        free(i->nick);
        free(i->hname);
        free(i);
        i = t;
    }
    free(user_list);

    for (room_node_t *i = room_list->next; i != NULL;) {
        room_node_t *t = i->next;
        free(i->rname);
        free(i);
        i = t;
    }
    free(room_list);

    return 0;
}
