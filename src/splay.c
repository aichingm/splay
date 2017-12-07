/*
 * File:   splay.c
 * Author: Mario Aichinger <aichingm@gmail.com>
 *
 * Created on July 4, 2017, 4:22 PM
 *
 *  This file is part of splay.
 *
 *  splay is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  splay is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with splay.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <vlc/vlc.h>

#include <vlc/plugins/vlc_common.h>
#include <vlc/plugins/vlc_url.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>

#include "splay.h"
#include "logging.h"

/*
 * Reading from stdin
 */


static int lread(char *buff, size_t len) {
    int ch, extra;
    if (feof(stdin)) {
        return LREAD_EOF;
    }

    if (fgets(buff, len, stdin) == NULL) {
        return LREAD_NO_INPUT;
    }

    if (buff[strlen(buff) - 1] != '\n') {
        extra = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF))
            extra = 1;
        return (extra == 1) ? LREAD_TOO_LONG : OK;
    }
    buff[strlen(buff) - 1] = '\0';
    return LREAD_OK;
}

/*
 * File Utils
 */
int isfile(const char *file) {
    struct stat s;
    stat(file, &s);
    return S_ISREG(s.st_mode);
}

/*
 * Time Utils
 */

struct tm * milstotime(int mills) { /* milliseconds to time_t */
    time_t s = (time_t) (mills / 1000);
    return localtime(&s);
}

void events_block(struct Player *p) {
    p->events_blocked = 1;
}

void events_unblock(struct Player *p) {
    p->events_blocked = 0;
}

int events_blocked(struct Player *p) {
    return p->events_blocked == 1;
}

/*
 * File List functions
 */

int fl_visible_lines() {
    return LINES - 5;
}

int fl_selected_offscreen(struct SPlay *sp) {
    if (sp->fl->offset + fl_visible_lines() <= sp->fl->selected
            || sp->fl->offset > sp->fl->selected
            ) {
        return 1;
    } else {
        return 0;
    }
}

int fl_mvd(struct SPlay *sp) {
    if (sp->fl->selected == libvlc_media_list_count(sp->plyr->mpl) - 1) {
        return 0;
    }
    sp->fl->selected++;
    if (fl_selected_offscreen(sp)) {
        sp->fl->offset++;
    }
    return 1;
}

int fl_mvu(struct SPlay *sp) {
    if (sp->fl->selected == 0) {
        return 0;
    }
    sp->fl->selected--;
    if (fl_selected_offscreen(sp)) {
        sp->fl->offset--;
    }
    return 1;
}

int fl_can_mvu(struct SPlay *sp) {
    if (sp->fl->selected == 0) {
        return 0;
    }
    return 1;
}

int fl_can_mvd(struct SPlay *sp) {
    if (sp->fl->selected == libvlc_media_list_count(sp->plyr->mpl) - 1) {
        return 0;
    }
    return 1;
}

int fl_has_osu(struct SPlay *sp) {
    if (sp->fl->offset > 0) {
        return 1;
    }
    return 0;
}

int fl_has_osd(struct SPlay *sp) {
    if (sp->fl->offset + fl_visible_lines() < libvlc_media_list_count(sp->plyr->mpl)) {
        return 1;
    }
    return 0;
}

/*
 * Printing
 */

char * trktitle(struct SPlay *sp, int index) {
    char * title = NULL;
    if (sp->title_mode == TITLE_MODE_TITLE_ARTIST) {
        char * t;
        char * artist;
        t = libvlc_media_get_meta(libvlc_media_list_item_at_index(sp->plyr->mpl, index), libvlc_meta_Title);
        artist = libvlc_media_get_meta(libvlc_media_list_item_at_index(sp->plyr->mpl, index), libvlc_meta_Artist);
        if(t != NULL && artist != NULL) {
            title = malloc(strlen(t)+3+strlen(artist)+1);
            title[0] = '\0';
            strcat(title, t);
            strcat(title, " - ");
            strcat(title, artist);
            free(artist);
            free(t);
        }
    } else if (sp->title_mode == TITLE_MODE_TITLE) {
        char * t = libvlc_media_get_meta(libvlc_media_list_item_at_index(sp->plyr->mpl, index), libvlc_meta_Title);
        if(t != NULL){
            title = malloc(strlen(t)+1);
            title[0] = '\0';
            strcat(title, t);
            free(t);
        }
    }else if(sp->title_mode == TITLE_MODE_PATH){
        char * t = libvlc_media_get_mrl(libvlc_media_list_item_at_index(sp->plyr->mpl, index));
        if(t != NULL){
            char * dup_t = strdup(t);
            title = malloc(strlen(t)+1);
            title[0] = '\0';
            strcpy(title, dirname(t));
            strcpy(title, basename(title));
            strcat(title, "/");
            strcat(title, basename(dup_t));
            decode_URI(title);
            free(dup_t);
            free(t);
        }
    }

    if(title == NULL || sp->title_mode == TITLE_MODE_FILE){
        char * t = libvlc_media_get_mrl(libvlc_media_list_item_at_index(sp->plyr->mpl, index));
        title = malloc(strlen(t)+1);
        title[0] = '\0';
        strcat(title, basename(t));
        decode_URI(title);
        free(t);
    }

    return title;
}

void printtrkln(struct SPlay *sp, int index, int line) { /* Print Track Line */

    char * name;
    char * title = trktitle(sp, index);
    if (sp->plyr->curr_playing_index == index) {
        wattron(sp->win, A_BOLD);
        if (sp->plyr->curr_playing_index > -1 && sp->plyr->is_playing == 1) {
            if ((name = malloc(strlen("[playing] ") + strlen(title) + 1)) != NULL) {
                name[0] = '\0';
                strcat(name, "[playing] ");
                strcat(name, title);
            }
        } else {
            if ((name = malloc(strlen("[pause] ") + strlen(title) + 1)) != NULL) {
                name[0] = '\0';
                strcat(name, "[pause] ");
                strcat(name, title);
            }
        }
    } else {
        name = malloc(strlen(title)+1);
        strcpy(name, title);
    }
    int x = getmaxx(sp->win);
    if(x-8 < strlen(name)){
        name[x - 8] = '.';
        name[x - 7] = '.';
        name[x - 6] = '.';
        name[x - 5] = '\0';
    }
    mvwaddstr(sp->win, line, 3, name);
    wattroff(sp->win, A_BOLD);
    free(name);
    free(title);

}

int printlist(struct SPlay * sp) { /* Print List */
    for (int i = 0; i < fl_visible_lines()
            && i < libvlc_media_list_count(sp->plyr->mpl) + sp->fl->offset; i++) {

        if (i + sp->fl->offset == sp->fl->selected) {
            wattron(sp->win, COLOR_PAIR(3));
            wattron(sp->win, A_BOLD);
        }
        printtrkln(sp, i + sp->fl->offset, i + 1);
        wattroff(sp->win, COLOR_PAIR(2));
        wattroff(sp->win, A_BOLD);
    }

    if (fl_has_osu(sp)) {
        mvwaddstr(sp->win, 1, 1, "^");
    }
    if (fl_has_osd(sp)) {
        mvwaddstr(sp->win, fl_visible_lines(), 1, "v");
    }
    return 0;
}

void printinfln(struct SPlay * sp) {
    if (sp->plyr->curr_playing_index == -1) {
        return;
    }
    libvlc_media_t * media = libvlc_media_list_item_at_index(sp->plyr->mpl, sp->plyr->curr_playing_index);
    int dur = libvlc_media_get_duration(media);
    libvlc_media_release(media);
    int now = libvlc_media_player_get_time(sp->plyr->mp);
    if (dur != -1 && now != -1) {
        char time [13];
        struct tm *tm = milstotime(dur);
        int dur_min = tm->tm_min;
        int dur_sec = tm->tm_sec;
        tm = milstotime(now);
        int now_min = tm->tm_min;
        int now_sec = tm->tm_sec;
        sprintf(time, "%02d:%02d/%02d:%02d", now_min, now_sec, dur_min, dur_sec);
        time[12] = '\0';
        char *title = trktitle(sp, sp->plyr->curr_playing_index);
        int x = getmaxx(sp->win);
        if(x - 15 < strlen(title)){
            title[x - 15] = '.';
            title[x - 14] = '.';
            title[x - 13] = '.';
            title[x - 12] = '\0';
        }
        move(LINES - 1, 1);
        clrtoeol();
        mvaddstr(LINES - 1, 1, title);
        mvaddstr(LINES - 1, COLS - 12, time);
        refresh();
        free(title);
    }
}

void printwin(struct SPlay * sp) {
    wclear(sp->win);
    wattrset(sp->win, A_UNDERLINE);
    wbkgd(sp->win, COLOR_PAIR(2));
    box(sp->win, 0, 0);
    printlist(sp);
    refresh();
    wrefresh(sp->win);
}

/*
 * SPlay Functions
 */
struct SPlay * splay_new() {
    struct SPlay *sp;
    sp = (struct SPlay*) malloc(sizeof (struct SPlay));
    sp->prep_shutdown = 0;
    sp->inf_thrd = -1;
    sp->fl = (struct List_Box*) malloc(sizeof (List_Box));
    sp->fl->offset = 0;
    sp->fl->selected = 0;
    sp->plyr = (struct Player*) malloc(sizeof (struct Player));
    sp->plyr->is_playing = 0;
    sp->plyr->curr_playing_index = -1;
    sp->plyr->events_blocked = 0;

    return sp;
}

void splay_delete(struct SPlay * sp) {
    free(sp->fl);
    libvlc_media_player_release(sp->plyr->mp);
    libvlc_media_list_release(sp->plyr->mpl);
    libvlc_release(sp->plyr->inst);
    free(sp->plyr);
    free(sp);
}

/*
 * Player Functions
 */

int ply_hasn(struct SPlay *sp) {
    return sp->plyr->curr_playing_index + 1 < libvlc_media_list_count(sp->plyr->mpl);
}

int ply_hasp(struct SPlay *sp) {
    return (sp->plyr->curr_playing_index - 1) >= 0;
}

void ply_idx(struct SPlay *sp, int idx) {
    if (idx < 0 || idx >= libvlc_media_list_count(sp->plyr->mpl)) {
        return;
    }
    sp->plyr->curr_playing_index = idx;
    libvlc_media_list_player_play_item_at_index(sp->plyr->mplp, idx);
}

void ply_nxt_no_ev_drw(struct SPlay * sp) {
    events_block(sp->plyr);
    ply_nxt(sp);
    printwin(sp);
    events_unblock(sp->plyr);

}

void ply_prv_no_ev_drw(struct SPlay * sp) {
    events_block(sp->plyr);
    ply_prv(sp);
    printwin(sp);
    events_unblock(sp->plyr);
}

void ply_nxt(struct SPlay * sp) {
    if (sp->plyr->curr_playing_index + 1 < libvlc_media_list_count(sp->plyr->mpl)) {
        sp->plyr->curr_playing_index++;
        libvlc_media_list_player_next(sp->plyr->mplp);
    }
}

void ply_prv(struct SPlay * sp) {
    if (sp->plyr->curr_playing_index - 1 >= 0) {
        sp->plyr->curr_playing_index--;
        libvlc_media_list_player_previous(sp->plyr->mplp);
    }
}

void ply_pp(struct SPlay * sp) {
    if (sp->plyr->is_playing == 1) {
        libvlc_media_list_player_pause(sp->plyr->mplp);
        sp->plyr->is_playing = 0;
    } else {
        libvlc_media_list_player_play(sp->plyr->mplp);
        sp->plyr->is_playing = 1;
    }
}

/*
 * Printing the Info Line
 */
void* inf_thrd(void *ptr) {
    struct SPlay *sp = (struct SPlay*) ptr;
    while (sp->prep_shutdown == 0) {
        printinfln(sp);
        sleep(1);
    }
    pthread_exit(NULL);
    return NULL;
}

/*
 * Shutdown
 */
void splay_shutdown(struct SPlay *sp) {
    sp->prep_shutdown = 1;

    //pthread_join(sp->inf_thrd, NULL);
    delwin(sp->win);
    endwin();
    if(libvlc_media_player_is_playing(sp->plyr->mp)){
        libvlc_media_player_stop(sp->plyr->mp);
    }
    if(libvlc_media_list_player_is_playing(sp->plyr->mplp)){
        libvlc_media_list_player_stop(sp->plyr->mplp);
    }
    splay_delete(sp);
    exit(0);
}

/*
 * VLC Event Listeners
 */
void libvlc_event(const struct libvlc_event_t* event, void* sp) {
    if (libvlc_MediaPlayerEndReached == event->type &&
            ((struct SPlay*) sp)->plyr->curr_playing_index == libvlc_media_list_count(((struct SPlay*) sp)->plyr->mpl) - 1) {
        splay_shutdown((struct SPlay*) sp);
    }
    if (libvlc_MediaListPlayerNextItemSet == event->type && !events_blocked(((struct SPlay*) sp)->plyr)) {
        ((struct SPlay*) sp)->plyr->curr_playing_index++;
        printinfln((struct SPlay*) sp);
        printwin((struct SPlay*) sp);
    }
}

#ifdef MPRIS

void setup_mpris(struct SPlay *splay) {
    mpris_set_splay(splay);
    mpris_spawn();
}

void shutdown_mpris(struct SPlay *splay) {
    mpris_shutdown();
}
#endif

/*
 * Main
 */
int main(void) {

#ifdef DEBUG
    printf("Splay will now wait for 10 seconds in which you can connect a debugger with run 'gdb ./splay %d' \n", getpid());
    sleep(10);
#endif

    setlocale(LC_ALL, "");

    struct SPlay *sp = splay_new();
    sp->title_mode = TITLE_MODE_TITLE;
    sp->plyr->inst = libvlc_new(0, NULL);
    sp->plyr->mp = libvlc_media_player_new(sp->plyr->inst);
    sp->plyr->mpl = libvlc_media_list_new(sp->plyr->inst);
    sp->plyr->mplp = libvlc_media_list_player_new(sp->plyr->inst);
    libvlc_media_list_player_set_media_list(sp->plyr->mplp, sp->plyr->mpl);
    libvlc_media_list_player_set_media_player(sp->plyr->mplp, sp->plyr->mp);

    libvlc_event_manager_t* emmlp = libvlc_media_list_player_event_manager(sp->plyr->mplp);
    libvlc_event_attach(emmlp, libvlc_MediaListPlayerNextItemSet, libvlc_event, sp);
    libvlc_event_manager_t* emmp = libvlc_media_player_event_manager(sp->plyr->mp);
    libvlc_event_attach(emmp, libvlc_MediaPlayerEndReached, libvlc_event, sp);

    char buffer [PATH_MAX];
    memset(buffer, 0, PATH_MAX);
    int status = 0;
    while ((status = lread(buffer, PATH_MAX)) == 0) {
        if (isfile(buffer)) {
            libvlc_media_t *m = libvlc_media_new_path(sp->plyr->inst, buffer);
            libvlc_media_parse(m);
            libvlc_media_list_add_media(sp->plyr->mpl, m);
            libvlc_media_release(m);
        }
    }

    // check if files are loaded
    libvlc_media_list_lock(sp->plyr->mpl);
    int loaded_files = libvlc_media_list_count(sp->plyr->mpl);
    libvlc_media_list_unlock(sp->plyr->mpl);
    if (loaded_files > 0) {

#ifdef MPRIS
        /* MPRIS setup */
        setup_mpris(sp);
#endif

        freopen(STDIN_FILE, "r", stdin);


        initscr();
        clear();
        noecho();
        curs_set(0);
        cbreak();
        keypad(stdscr, 1);
        start_color();
        init_pair(1, FG_SCR, BG_SCR);
        init_pair(2, FG_WIN, BG_WIN);
        init_pair(3, FG_ACT, BG_ACT);
        bkgd(COLOR_PAIR(1));

        sp->win = newwin(LINES - 3, COLS - 2, 1, 1);
        printwin(sp);


        /*
         * Info Line setup
         */
        pthread_create(&sp->inf_thrd, NULL, inf_thrd, sp);



        if (1) {
            events_block(sp->plyr);
            sp->plyr->is_playing = 1;
            ply_idx(sp, 0);
            printwin(sp);
            printinfln(sp);
            events_unblock(sp->plyr);
        }

        int ch = 0;
        int time;
        while ((ch = getch()) != KEY_END && ch != 101) {
            switch (ch) {
                case KEY_DOWN:
                    fl_mvd(sp);
                    printwin(sp);
                    break;
                case KEY_UP:
                    fl_mvu(sp);
                    printwin(sp);
                    break;
                case KEY_RIGHT:
                    time = libvlc_media_player_get_time(sp->plyr->mp);
                    libvlc_media_t * media = libvlc_media_list_item_at_index(sp->plyr->mpl, sp->plyr->curr_playing_index);
                    int dur = libvlc_media_get_duration(media);
                    libvlc_media_release(media);
                    if (time + SEEK_DURATION < dur) {
                        libvlc_media_player_set_time(sp->plyr->mp, time + SEEK_DURATION);
                    }
                    printinfln(sp);
#ifdef MPRIS
                    /* MPRIS setup */
                    mpris_on_seek();
#endif

                    break;
                case KEY_LEFT:
                    time = libvlc_media_player_get_time(sp->plyr->mp);
                    if (time - SEEK_DURATION > 0) {
                        libvlc_media_player_set_time(sp->plyr->mp, time - SEEK_DURATION);
                    } else {
                        libvlc_media_player_set_time(sp->plyr->mp, 0);
                    }
                    printinfln(sp);
#ifdef MPRIS
                    /* MPRIS setup */
                    mpris_on_seek();
#endif
                    break;
                case 32:
                    if (sp->plyr->is_playing == 1) {
                        libvlc_media_list_player_pause(sp->plyr->mplp);
                        sp->plyr->is_playing = 0;
                    } else {
                        libvlc_media_list_player_play(sp->plyr->mplp);
                        sp->plyr->is_playing = 1;
                    }
                    printwin(sp);
                    break;
                case 10:
                    events_block(sp->plyr);
                    sp->plyr->is_playing = 1;
                    ply_idx(sp, sp->fl->selected);
                    printwin(sp);
                    printinfln(sp);
                    events_unblock(sp->plyr);
                    break;
                case 110:
                    ply_nxt_no_ev_drw(sp);
                    break;
                case 112:
                    ply_prv_no_ev_drw(sp);
                    break;
                case (int)'t':
                    sp->title_mode++;
                    if(sp->title_mode == TITLE_MODES_LENGTH){
                        sp->title_mode = TITLE_MODE_TITLE;
                    }
                    printwin(sp);
                    printinfln(sp);
                    break;
            }
        }
    }
    splay_shutdown(sp);
    return 0;
}
