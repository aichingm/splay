/*
 * File:   colors.h
 * Author: Mario Aichinger <aichingm@gmail.com>
 *
 * Created on November 10, 2017, 7:20 PM
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
/*
 * Available colors:
 *
 *  COLOR_BLACK
 *  COLOR_RED
 *  COLOR_GREEN
 *  COLOR_YELLOW
 *  COLOR_BLUE
 *  COLOR_MAGENTA
 *  COLOR_CYAN
 *  COLOR_WHITE
 *
 */

#define BG_SCR COLOR_BLACK
#define FG_SCR COLOR_WHITE
#define BG_WIN COLOR_BLACK
#define FG_WIN COLOR_WHITE
#define BG_ACT COLOR_WHITE
#define FG_ACT COLOR_BLACK

