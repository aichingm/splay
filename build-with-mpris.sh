#!/bin/bash

# File:   build-with-mpris.sh
# Author: Mario Aichinger <aichingm@gmail.com>
#
# Created on July 12, 2017, 1:36 PM
#
#  splay is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  splay is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with splay.  If not, see <http://www.gnu.org/licenses/>.

gcc src/splay-mpris.c -o splay \
    -I/usr/include/dbus-1.0 \
    -I/usr/lib/dbus-1.0/include  \
    -I/usr/include/glib-2.0  \
    -I/usr/lib/glib-2.0/include \
    -ldbus-1 \
    -ldbus-glib-1 \
    -lglib-2.0 \
    -lncurses \
    -lvlc \
    -pthread

chmod +x splay
