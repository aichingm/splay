#!/bin/bash

# File:   build.sh
# Author: Mario Aichinger <aichingm@gmail.com>
#
# Created on July 12, 2017, 10:03 PM
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

gcc splay.c -o splay -lncurses -lvlc -pthread
chmod +x splay
