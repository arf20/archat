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

#include "net.h"
#include "db.h"

int
main()
{
    printf("arfchat  Copyright (C) 2024  Angel Ruiz Fernandez <arf20>\n"
        "This program comes with ABSOLUTELY NO WARRANTY\n"
        "This is free software, and you are welcome to redistribute it\n"
        "under certain conditions.\n\n");

    srand(time(NULL));

    /* User information */
    uint32_t uid = rand();
    char *nick = getlogin();
    char *nickbuff = NULL;
    size_t nicklen = 0;

    /* Room information */
    uint16_t rid = 0;
    char *rname = NULL;

    /* DB */
    user_node_t *user_list = malloc(sizeof(user_node_t));
    user_list->next = NULL;
    room_node_t *room_list = malloc(sizeof(room_node_t));
    room_list->next = NULL;

    printf("Nickname [%s]> ", nick);
    getline(&nickbuff, &nicklen, stdin);
    nickbuff[strlen(nickbuff) - 1] = '\0';

    if (strlen(nickbuff) > 0) nick = nickbuff;

    /* Init */
    if (create_sockets() < 0) {
        printf("create_sockets: %s\n", strerror(errno));
        return 1;
    }

    /* Begin autodiscovery process */
    printf("Autodiscovery...\n");
    if (send_ping(uid) < 0) {
        printf("send_ping: %s\n", strerror(errno));
        return 1;
    }

    time_t t_begin = time(NULL);

    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

    while (1) {
        const header_t *header;
        const char *data;
        if (recv_message(&header, &data) < 0) {
            if (errno != EAGAIN) {
                printf("recv_message: %s\n", strerror(errno));
                break;
            }
        } else {
            //printf("<<TYPE: %d\n", header->type);

            switch (header->type) {
                case TYPE_PING: {
                    printf("<<PING\n");

                    printf(">>PONG %d %d %s %s\n", uid, rid, nick, rname);
                    send_pong(uid, rid, nick, rname);
                } break;
                case TYPE_PONG: {
                    printf("<<PONG %d ", header->s_uid);

                    uint16_t s_rid = *(uint16_t*)data;
                    const char *s_nick = data + 4;
                    const char *s_rname = data + 4 + strlen(s_nick) + 1;

                    printf("%d %s %s\n", s_rid, s_nick, s_rname);

                    user_list_push(user_list, header->s_uid, strdup(s_nick), s_rid);
                    room_list_push(room_list, s_rid, strdup(s_rname));
                }
            }
        }

        char cmd;
        if (read(0, &cmd, 1) < 0) {
            if (errno != EAGAIN) {
                printf("read: %s\n", strerror(errno));
                break;
            }
        } else  {
            switch (cmd) {
                case 'w': {
                    for (user_node_t *i = user_list->next; i != NULL; i = i->next)
                        printf("%s ", i->nick);
                    puts("\n");
                } break;
                case 'l': {
                    printf(" room name    #\n");
                    printf("-------------------------------\n");
                    for (room_node_t *i = room_list->next; i != NULL; i = i->next)
                        printf("%s %d\n", i->rid);
                    printf("-------------------------------\n");
                } break;
            }
        }

        usleep(1000);
    }

    return 0;
}
