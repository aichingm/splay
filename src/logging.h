/* 
 * File:   logging.h
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

#ifndef LOGGING_H
#define LOGGING_H

#define LOGGING 1
#define LOGGING_FILE "/tmp/splay.log"

#define SPLOG_I_INIT(TAG) int logging_splog_i = 0; SPLOGF(TAG": %d", logging_splog_i++);
#define SPLOG_I(TAG) SPLOGF(TAG": %d", logging_splog_i++);

#define SPLOGF(format, ...) \
        { \
            char *splogf_msg; \
            splogf_msg = (char*) malloc(sizeof(char) * 1000);\
            snprintf(splogf_msg, 1000, format, __VA_ARGS__);\
            splog(splogf_msg);\
            free(splogf_msg); \
        }

void splog(char *s) {
    FILE *f = fopen(LOGGING_FILE, "a+");
    fwrite(s, 1, strlen(s), f);
    fwrite("\n", 1, 1, f);
    fclose(f);
}

void splogi(int i) {
    FILE *f = fopen(LOGGING_FILE, "a+");
    char s [18];
    s[17] = '\0';
    sprintf(s, "%016d\n", i);
    fwrite(s, 17, 1, f);
    fclose(f);
}

#endif

