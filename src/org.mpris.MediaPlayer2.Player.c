/*
 * File:   org.mpris.MediaPlayer2.Player.c
 * Author: Mario Aichinger <aichingm@gmail.com>
 *
 * Created on October 23, 2017, 1:16 PM
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

void mediaplayer2_player_set_splay(struct SPlay *sp) {
    splay = sp;
}

DBusHandlerResult mediaplayer2_player_property_handler(const char *property, DBusConnection *conn, DBusMessage *reply) {
    SPLOGF("Player call: %s", property);
    char * title = NULL;
    char * album = NULL;
    char * art_url = NULL;
    char * artist = NULL;

    if (PROPERTY_IS("CanGoNext")) {
        OPEN_ITER(reply, iter);
        ADD_VARIANT_OPEN(iter, variant, "b")
        dbus_bool_t val = ply_hasn(splay) ? TRUE : FALSE;
        ADD_BOOLEAN(variant, val)
        ADD_VARIANT_CLOSE(iter, variant)
    } else if (PROPERTY_IS("CanGoPrevious")) {
        OPEN_ITER(reply, iter);
        ADD_VARIANT_OPEN(iter, variant, "b")
        dbus_bool_t val = ply_hasp(splay) ? TRUE : FALSE;
        ADD_BOOLEAN(variant, val)
        ADD_VARIANT_CLOSE(iter, variant)
    } else if (PROPERTY_IS("CanPlay") || PROPERTY_IS("CanPause")) {
        OPEN_ITER(reply, iter);
        ADD_VARIANT_OPEN(iter, variant, "b")
        dbus_bool_t val = TRUE;
        ADD_BOOLEAN(variant, val)
        ADD_VARIANT_CLOSE(iter, variant)
    } else if (PROPERTY_IS("CanSeek")) {
        OPEN_ITER(reply, iter);
        ADD_VARIANT_OPEN(iter, variant, "b")
        dbus_bool_t val = TRUE;
        ADD_BOOLEAN(variant, val)
        ADD_VARIANT_CLOSE(iter, variant)
    } else if (PROPERTY_IS("CanControl")) {
        dbus_bool_t val = TRUE;
        dbus_message_append_args(reply, DBUS_TYPE_BOOLEAN, &val, DBUS_TYPE_INVALID);
    } else if (PROPERTY_IS("MinimumRate") || PROPERTY_IS("MaximumRate") || PROPERTY_IS("Rate") || PROPERTY_IS("Volume")) {
        double val = 1;
        dbus_message_append_args(reply, DBUS_TYPE_DOUBLE, &val, DBUS_TYPE_INVALID);
    } else if (PROPERTY_IS("LoopStatus")) {
        const char * val = "None";
        dbus_message_append_args(reply, DBUS_TYPE_STRING, &val, DBUS_TYPE_INVALID);
    } else if (PROPERTY_IS("PlaybackStatus")) {
        OPEN_ITER(reply, iter);
        ADD_VARIANT_OPEN(iter, variant, "s")
        const char * val = libvlc_media_player_is_playing(splay->plyr->mp) ? "Playing" : "Paused";
        ADD_STRING(variant, val)
        ADD_VARIANT_CLOSE(iter, variant)
    } else if (PROPERTY_IS("Position")) {
        OPEN_ITER(reply, iter);
        ADD_VARIANT_OPEN(iter, variant, "x")
                long long int val = libvlc_media_player_get_time(splay->plyr->mp)*1000;
        ADD_SIGNED_INT(variant, val)
        ADD_VARIANT_CLOSE(iter, variant)

    } else if (PROPERTY_IS("Metadata")) {
        OPEN_ITER(reply, iter);
        ADD_ARRAY_OPEN(iter, array, "{sv}")
        //ADD_DICT_OF_STRING_STRING_FORM_X(array, "mpris:trackid", splay->plyr->curr_playing_index, "%d", 64)
            ADD_DICT_OF_STRING_OBJECT(array, "mpris:trackid", "/org/mpris/MediaPlayer2/Player/track_xxx")

        ADD_DICT_OF_STRING_SIGNED_INT(array, "mpris:length", get_duration(splay))

        title = GET_META(libvlc_meta_Title);
        if (title) {
            ADD_DICT_OF_STRING_STRING(array, "mpris:title", title)
        }
        artist = GET_META(libvlc_meta_Artist);
        if (artist) {
            ADD_DICT_OF_STRING_ARRAY_OF_STRING_OPEN(array, "mpris:artist", Artist)
            ADD_STRING(Artist, artist)
            ADD_DICT_OF_STRING_ARRAY_OF_STRING_CLOSE(array, Artist)
        }
        album = GET_META(libvlc_meta_Album);
        if (album) {
            ADD_DICT_OF_STRING_STRING(array, "mpris:album", album)
        }

        art_url = GET_META(libvlc_meta_ArtworkURL);
        if (art_url) {
            ADD_DICT_OF_STRING_STRING(array, "mpris:artUrl", art_url)
        }

        ADD_ARRAY_CLOSE(iter, array)

    } else {
        /* not a property */
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    if (dbus_connection_send(conn, reply, NULL)) {
        return DBUS_HANDLER_RESULT_HANDLED;
    }
    free(title);
    free(artist);
    free(art_url);
    free(album);
    return DBUS_HANDLER_RESULT_NEED_MEMORY;
}

DBusHandlerResult mediaplayer2_player_all_properties_handler(DBusConnection *conn, DBusMessage *reply) {
    char * title = NULL;
    char * album = NULL;
    char * art_url = NULL;
    char * artist = NULL;
    DBusHandlerResult result;
    DBusMessageIter array, iter;
    result = DBUS_HANDLER_RESULT_NEED_MEMORY;
    dbus_message_iter_init_append(reply, &iter);
    dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &array);

    ADD_DICT_OF_STRING_BOOLEAN(array, "CanGoNext", ply_hasn(splay))
    ADD_DICT_OF_STRING_BOOLEAN(array, "CanGoPrevious", ply_hasp(splay))
    ADD_DICT_OF_STRING_BOOLEAN(array, "CanPlay", TRUE)
    ADD_DICT_OF_STRING_BOOLEAN(array, "CanPause", TRUE)
    ADD_DICT_OF_STRING_BOOLEAN(array, "CanSeek", TRUE)
    ADD_DICT_OF_STRING_BOOLEAN(array, "CanControl", TRUE)
    ADD_DICT_OF_STRING_DOUBLE(array, "MinimumRate", 1)
    ADD_DICT_OF_STRING_DOUBLE(array, "MaximumRate", 1)
    ADD_DICT_OF_STRING_DOUBLE(array, "Rate", 1)
    ADD_DICT_OF_STRING_DOUBLE(array, "Volume", 1)
    ADD_DICT_OF_STRING_STRING(array, "LoopStatus", "None")
    ADD_DICT_OF_STRING_STRING(array, "PlaybackStatus", libvlc_media_player_is_playing(splay->plyr->mp) ? "Playing" : "Paused")
    ADD_DICT_OF_STRING_SIGNED_INT(array, "Position", libvlc_media_player_get_time(splay->plyr->mp)*1000)

    ADD_DICT_OF_ARRAY_OF_STRING_VARIANT_OPEN(array, "Metadata", Metadata)
    ADD_DICT_OF_STRING_OBJECT(Metadata, "mpris:trackid", "/org/mpris/MediaPlayer2/Player/track_xxx")
    ADD_DICT_OF_STRING_SIGNED_INT(Metadata, "mpris:length", get_duration(splay))

    title = GET_META(libvlc_meta_Title);
    if (title) {
        ADD_DICT_OF_STRING_STRING(Metadata, "xesam:title", title)
    }
    artist = GET_META(libvlc_meta_Artist);
    if (artist) {
        ADD_DICT_OF_STRING_ARRAY_OF_STRING_OPEN(Metadata, "xesam:artist", Artist)
        ADD_STRING(Artist, artist)
        ADD_DICT_OF_STRING_ARRAY_OF_STRING_CLOSE(Metadata, Artist)
    }
    album = GET_META(libvlc_meta_Album);
    if (album) {
        ADD_DICT_OF_STRING_STRING(Metadata, "xesam:album", album)
    }

    art_url = GET_META(libvlc_meta_ArtworkURL);
    if (art_url) {
        ADD_DICT_OF_STRING_STRING(Metadata, "mpris:artUrl", art_url)
    }

    ADD_DICT_OF_ARRAY_OF_STRING_VARIANT_CLOSE(array, Metadata)

    dbus_message_iter_close_container(&iter, &array);

    if (dbus_connection_send(conn, reply, NULL)) {
        result = DBUS_HANDLER_RESULT_HANDLED;
    }
    free(title);
    free(artist);
    free(art_url);
    free(album);
    return result;
}
