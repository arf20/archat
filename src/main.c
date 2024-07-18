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

#include "net.h"
#include "db.h"


char *
prompt_input()
{
    int c;
    while (getchar() != '\n');
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_NONBLOCK);
    char *buff = NULL;
    size_t len = 0;
    int r = getline(&buff, &len, stdin);
    buff[strlen(buff) - 1] = '\0';
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
    return buff;
}

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
    const char *rname = NULL;

    /* DB */
    user_node_t *user_list = malloc(sizeof(user_node_t));
    user_list->next = NULL;
    room_node_t *room_list = malloc(sizeof(room_node_t));
    room_list->next = NULL;

    printf("Nickname [%s]> ", nick);
    getline(&nickbuff, &nicklen, stdin);
    nickbuff[strlen(nickbuff) - 1] = '\0';

    if (strlen(nickbuff) > 0) nick = nickbuff;

    printf("==uid: %d\n==nick: %s\n", uid, nick);

    /* Init */
    if (create_sockets() < 0) {
        printf("create_sockets: %s\n", strerror(errno));
        return 1;
    }

    /* Begin autodiscovery process */
    printf(">>PING %d\n", uid);
    if (send_ping(uid) < 0) {
        printf("send_ping: %s\n", strerror(errno));
        return 1;
    }

    time_t t_begin = time(NULL);

    /* Non-canonical console */
    struct termios tattr = { }, saved_tattr = { };
    tcgetattr(STDIN_FILENO, &tattr);
    saved_tattr = tattr;
    tattr.c_lflag &= ~(ICANON|ECHO);
    tattr.c_oflag |= ONLCR;
    tattr.c_cc[VMIN] = 1;
    tattr.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);

    /* Non-blocking console */
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

    int run = 1;
    while (run) {
        const header_t *header;
        const char *data;
        if (recv_message(&header, &data) < 0) {
            if (errno != EAGAIN) {
                printf("recv_message: %s\n", strerror(errno));
                break;
            }
        } else {
            /* Handle incoming packets */
            switch (header->type) {
                case TYPE_PING: {
                    printf("<<PING %d\n", header->s_uid);

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
                    if (s_rid != 0)
                        room_list_push(room_list, s_rid, strdup(s_rname));
                } break;
                case TYPE_JOIN: {
                    printf("<<JOIN %d ", header->s_uid);

                    uint16_t s_rid = *(uint16_t*)data;
                    const char *s_rname = data + 4;

                    printf("%d %s\n", s_rid, s_rname);

                    room_list_push(room_list, s_rid, s_rname);
                } break;
            }
        }

        /* Handle commands */
        char cmd;
        if (read(0, &cmd, 1) < 0) {
            if (errno != EAGAIN) {
                printf("read: %s\n", strerror(errno));
                break;
            }
        } else  {
            switch (cmd) {
                case 'q': {
                    run = 0;
                };
                case 'w': {
                    for (user_node_t *i = user_list->next; i != NULL; i = i->next)
                        printf("%s ", i->nick);
                    puts("\n");
                } break;
                case 'l': {
                    int maxw = 0;
                    for (room_node_t *i = room_list->next; i != NULL; i = i->next) {
                        int w = strlen(i->rname);
                        if (w > maxw) maxw = w;
                    }

                    printf(" room name   #\n");
                    printf("---------------\n");
                    for (room_node_t *i = room_list->next; i != NULL; i = i->next)
                        printf("%10s %3d\n", i->rname, i->rid);
                    printf("---------------\n");
                } break;
                case 'j': {
                    printf(":join> ");
                    char *joinrname = prompt_input();

                    room_node_t *found = NULL;
                    for (room_node_t *i = room_list->next; i != NULL; i = i->next) {
                        if (strcmp(i->rname, joinrname) == 0) {
                            found = i;
                            break;
                        }
                    }

                    if (found) {
                        /* Join room */
                        printf(">>JOIN %d %d %s\n", uid, found->rid, found->rname);
                        send_join(uid, found->rid, found->rname);

                        rid = found->rid;
                        rname = found->rname;
                    } else {
                        /* Create room */
                        uint16_t new_rid = 1 + (rand() % 65534);

                        printf(">>JOIN* %d %d %s\n", uid, new_rid, joinrname);
                        send_join(uid, new_rid, joinrname);

                        rid = new_rid;
                        rname = joinrname;
                    }
                }
            }
        }

        usleep(1000);
    }

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved_tattr);

    return 0;
}
