/*
 * File:   org.mpris.MediaPlayer2.c
 * Author: Mario Aichinger <aichingm@gmail.com>
 *
 * Created on October 23, 2017, 1:21 PM
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
#include <dbus/dbus.h>

#include "logging.h"
#include "mpris.h"


struct SPlay *splay;

void mediaplayer2_set_splay(struct SPlay *sp) {
    splay = sp;
}

DBusHandlerResult mediaplayer2_property_handler(const char *property, DBusConnection *conn, DBusMessage *reply) {
    SPLOGF("%s", property);
    if (PROPERTY_IS("CanQuit") || PROPERTY_IS("CanRaise") || PROPERTY_IS("HasTrackList")) {
        /* send false*/
        dbus_bool_t val = FALSE;
        dbus_message_append_args(reply, DBUS_TYPE_BOOLEAN, &val, DBUS_TYPE_INVALID);
    } else if (PROPERTY_IS("Identity")) {
        OPEN_ITER(reply, iter);
        ADD_VARIANT_OPEN(iter, variant, "s")
                const char * val = MPRIS2_IDENTITY;
        ADD_STRING(variant, val)
        ADD_VARIANT_CLOSE(iter, variant)
    } else if (PROPERTY_IS("SupportedUriSchemes") || PROPERTY_IS("SupportedMimeTypes")) {
        /* Send empty list*/
        DBusMessageIter iter, array;
        dbus_message_iter_init_append(reply, &iter);
        dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "s", &array);
        dbus_message_iter_close_container(&iter, &array);
    } else {
        /* not a property */
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }
    if (dbus_connection_send(conn, reply, NULL)) {
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    return DBUS_HANDLER_RESULT_NEED_MEMORY;
}

DBusHandlerResult mediaplayer2_all_properties_handler(DBusConnection *conn, DBusMessage *reply) {
    OPEN_ITER(reply, iter)
    ADD_ARRAY_OPEN(iter, array, "{sv}")
    ADD_DICT_OF_STRING_BOOLEAN(array, "CanQuit", FALSE)
    ADD_DICT_OF_STRING_BOOLEAN(array, "CanRaise", FALSE)
    ADD_DICT_OF_STRING_BOOLEAN(array, "HasTrackList", FALSE)
    ADD_DICT_OF_STRING_STRING(array, "Identity", MPRIS2_IDENTITY)
    ADD_DICT_OF_STRING_ARRAY_OF_STRING_OPEN(array, "SupportedUriSchemes", SupportedUriSchemes)
    ADD_DICT_OF_ARRAY_OF_STRING_VARIANT_CLOSE(array, SupportedUriSchemes)
    ADD_DICT_OF_STRING_ARRAY_OF_STRING_OPEN(array, "SupportedMimeTypes", SupportedMimeTypes)
    ADD_DICT_OF_ARRAY_OF_STRING_VARIANT_CLOSE(array, SupportedMimeTypes)
    ADD_ARRAY_CLOSE(iter, array)
    if (dbus_connection_send(conn, reply, NULL)) {
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    return DBUS_HANDLER_RESULT_NEED_MEMORY;
}
