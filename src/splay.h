/*
 * File:   splay.h
 * Author: Mario Aichinger <aichingm@gmail.com>
 *
 * Created on October 23, 2017, 1:22 PM
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

#include <stdlib.h>
#include <curses.h>
#include "colors.h"


#ifndef SPLAY_H
#define SPLAY_H
#define SEEK_DURATION 10000

#define STDIN_FILE "/dev/tty"

/*
 * Title Modes
 */
#define TITLE_MODE_TITLE 0
#define TITLE_MODE_TITLE_ARTIST 1
#define TITLE_MODE_PATH 2
#define TITLE_MODE_FILE 3
#define TITLE_MODES_LENGTH 4



/*
 * Reading from stdin
 */

#define LREAD_OK 0
#define LREAD_NO_INPUT 1
#define LREAD_TOO_LONG 2
#define LREAD_EOF 3

static int lread(char *buff, size_t len);
/*
 * File Utils
 */
int isfile(const char *file);
/*
 * Time Utils
 */

struct tm * milstotime(int mills);

/*
 * Structs
 */
struct Player {
    libvlc_instance_t *inst;
    libvlc_media_player_t *mp;
    libvlc_media_list_t *mpl;
    libvlc_media_list_player_t *mplp;
    int curr_playing_index;
    int is_playing;
    int events_blocked;
};

struct List_Box {
    int offset;
    int selected;
} List_Box;

struct SPlay {
    struct Player *plyr;
    struct List_Box *fl;
    int title_mode;
    WINDOW *win;
    pthread_t inf_thrd;
    int prep_shutdown;
};

void events_block(struct Player *p);

void events_unblock(struct Player *p);

int events_blocked(struct Player *p);

/*
 * File List functions
 */

int fl_visible_lines();

int fl_selected_offscreen(struct SPlay *sp);

int fl_mvd(struct SPlay *sp);

int fl_mvu(struct SPlay *sp);

int fl_can_mvu(struct SPlay *sp);

int fl_can_mvd(struct SPlay *sp);

int fl_has_osu(struct SPlay *sp);

int fl_has_osd(struct SPlay *sp);

/*
 * Printing
 */

char * trktitle(struct SPlay *sp, int index);

void printtrkln(struct SPlay *sp, int index, int line);

int printlist(struct SPlay * sp);

void printinfln(struct SPlay * sp);

void printwin(struct SPlay * sp);

/*
 * SPlay Functions
 */
struct SPlay * splay_new();

void splay_delete(struct SPlay * sp);
/*
 * Player Functions
 */
int ply_hasn(struct SPlay *sp);
int ply_hasp(struct SPlay *sp);
void ply_idx(struct SPlay *sp, int idx);

void ply_nxt(struct SPlay * sp);

void ply_prv(struct SPlay * sp);

void ply_nxt_no_ev_drw(struct SPlay * sp);

void ply_prv_no_ev_drw(struct SPlay * sp);

void ply_pp(struct SPlay * sp);
/*
 * Printing the Info Line
 */
void* inf_thrd(void *ptr);

/*
 * Shutdown
 */

void splay_shutdown(struct SPlay * sp);
/*
 * VLc Event Listeners
 */
void libvlc_event(const struct libvlc_event_t* event, void* sp);


#endif
