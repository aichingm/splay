/*
 * File:   mpris.c
 * Author: Mario Aichinger <aichingm@gmail.com>
 *
 * Created on October 23, 2017, 1:19 PM
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

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <vlc/vlc.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h> /* GMainLoop */

#include "mpris.h"
#include "splay.h"
#include "logging.h"

#include "org.mpris.MediaPlayer2.c"
#include "org.mpris.MediaPlayer2.Player.c"


void mpris_libvlc_event(const struct libvlc_event_t* event, void* sp);
DBusHandlerResult mpris_message_handler(DBusConnection *conn, DBusMessage *message, void *data);

const char *version = "1";
DBusConnection *conn;
GMainLoop *mainloop;
struct SPlay *splay;

const DBusObjectPathVTable server_vtable = {
    .message_function = mpris_message_handler
};

void mpris_spawn() {
    int * retval;
    retval = (int *) malloc(4);
    pthread_t tid;
    pthread_create(&tid, NULL, mpris_main, 0);
}

void mpris_shutdown() {
    g_main_loop_quit(mainloop);
}


const char *server_introspection_xml =
        DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE
        "<node>\n"

        "  <interface name='org.freedesktop.DBus.Introspectable'>\n"
        "    <method name='Introspect'>\n"
        "      <arg name='data' type='s' direction='out' />\n"
        "    </method>\n"
        "  </interface>\n"

        "  <interface name='org.freedesktop.DBus.Properties'>\n"
        "    <method name='Get'>\n"
        "      <arg name='interface' type='s' direction='in' />\n"
        "      <arg name='property'  type='s' direction='in' />\n"
        "      <arg name='value'     type='s' direction='out' />\n"
        "    </method>\n"
        "    <method name='GetAll'>\n"
        "      <arg name='interface'  type='s'     direction='in'/>\n"
        "      <arg name='properties' type='a{sv}' direction='out'/>\n"
        "    </method>\n"
        "    <method name='Set'>\n"
        "      <arg type='s' name='interface_name' direction='in'/>\n"
        "      <arg type='s' name='property_name' direction='in'/>\n"
        "      <arg type='v' name='value' direction='in'/>\n"
        "    </method>\n"
        "    <signal name='PropertiesChanged'>\n"
        "      <arg type='s' name='interface_name'/>\n"
        "      <arg type='a{sv}' name='changed_properties'/>\n"
        "      <arg type='as' name='invalidated_properties'/>\n"
        "    </signal>\n"
        "  </interface>\n"

        "  <interface name='org.mpris.MediaPlayer2'>\n"
        "    <property name='CanQuit' type='b' access='read' />\n"
        "    <property name='CanRaise' type='b' access='read' />\n"
        "    <property name='HasTrackList' type='b' access='read' />\n"
        "    <property name='Identity' type='s' access='read' />\n"
        "    <property name='SupportedUriSchemes' type='as' access='read' />\n"
        "    <property name='SupportedMimeTypes' type='as' access='read' />\n"
        "    <method name='Raise' >\n"
        "    </method>\n"
        "    <method name='Quit' >\n"
        "    </method>\n"
        "  </interface>\n"

        "  <interface name='org.mpris.MediaPlayer2.Player'>\n"
        "    <property name='SupportedMimeTypes' type='as' access='read' />\n"
        "    <method name='Next' >\n"
        "    </method>\n"
        "    <method name='Previous' >\n"
        "    </method>\n"
        "    <method name='Pause' >\n"
        "    </method>\n"
        "    <method name='PlayPause' >\n"
        "    </method>\n"
        "    <method name='Stop' >\n"
        "    </method>\n"
        "    <method name='Play' >\n"
        "    </method>\n"
        "    <method name='Seek' >\n"
        "      <arg name='Offset'  type='x' direction='in'/>\n"
        "    </method>\n"
        "    <method name='SetPosition' >\n"
        "      <arg name='TrackId'  type='o' direction='in'/>\n"
        "      <arg name='Position'  type='x' direction='in'/>\n"
        "    </method>\n"
        "    <method name='OpenUri' >\n"
        "      <arg name='Uri'  type='s' direction='in'/>\n"
        "    </method>\n"
        "    <signal name='Seeked'>\n"
        "      <arg name='Position' type='x' />"
        "    </signal>"
        "    <property name='PlaybackStatus' type='s' access='read' />\n"
        "    <property name='LoopStatus' type='s' access='readwrite' />\n"
        "    <property name='Rate' type='d' access='readwrite' />\n"
        "    <property name='Metadata' type='a{sv}' access='read' />\n"
        "    <property name='Volume' type='d' access='readwrite' />\n"
        "    <property name='Position' type='x' access='read' />\n"
        "    <property name='MinimumRate' type='d' access='read' />\n"
        "    <property name='MaximumRate' type='d' access='read' />\n"
        "    <property name='CanGoNext' type='b' access='read' />\n"
        "    <property name='CanGoPrevious' type='b' access='read' />\n"
        "    <property name='CanPlay' type='b' access='read' />\n"
        "    <property name='CanPause' type='b' access='read' />\n"
        "    <property name='CanSeek' type='b' access='read' />\n"
        "    <property name='CanControl' type='b' access='read' />\n"
        "  </interface>\n"
        "  <!--<interface name='org.mpris.MediaPlayer2.TrackList'>\n"
        "  </interface>-->\n"
        "</node>\n";

void mpris_set_splay(struct SPlay *sp) {
    splay = sp;
    mediaplayer2_set_splay(sp);
    mediaplayer2_player_set_splay(sp);
    libvlc_event_manager_t* emmlp = libvlc_media_list_player_event_manager(sp->plyr->mplp);
    libvlc_event_attach(emmlp, libvlc_MediaListPlayerNextItemSet, mpris_libvlc_event, sp);
    libvlc_event_manager_t* emmp = libvlc_media_player_event_manager(sp->plyr->mp);
    libvlc_event_attach(emmp, libvlc_MediaPlayerPaused, mpris_libvlc_event, sp);
    libvlc_event_attach(emmp, libvlc_MediaPlayerPlaying, mpris_libvlc_event, sp);
    libvlc_event_attach(emmp, libvlc_MediaPlayerEndReached, mpris_libvlc_event, sp);
}

/*
 * VLc Event Listeners
 */
void mpris_libvlc_event(const struct libvlc_event_t* event, void* sp) {

    SPLOGF("%d", event->type)
    if (libvlc_MediaPlayerPlaying == event->type || libvlc_MediaPlayerPaused == event->type) {
        SIGNAL_OPEN(reply, iter, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "PropertiesChanged")
        ADD_STRING(iter, "org.mpris.MediaPlayer2.Player")
        ADD_ARRAY_OPEN(iter, array, "{sv}")
        ADD_DICT_ENTRY_OPEN(array, dict)
        ADD_STRING(dict, "PlaybackStatus")
        ADD_VARIANT_OPEN(dict, variant, "s")
        ADD_STRING(variant, libvlc_media_player_is_playing(splay->plyr->mp) ? "Playing" : "Paused")
        ADD_VARIANT_CLOSE(dict, variant)
        ADD_DICT_ENTRY_CLOSE(array, dict)
        ADD_ARRAY_CLOSE(iter, array)
        ADD_ARRAY_OPEN(iter, array2, "s")
        ADD_ARRAY_CLOSE(iter, array2)
        SIGNAL_CLOSE(conn, reply)
    }

    if (libvlc_MediaListPlayerNextItemSet == event->type) {
        SIGNAL_OPEN(reply, iter, "/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties", "PropertiesChanged")
        ADD_STRING(iter, "org.mpris.MediaPlayer2.Player")
        ADD_ARRAY_OPEN(iter, array, "{sv}")
        ADD_DICT_OF_ARRAY_OF_STRING_VARIANT_OPEN(array, "Metadata", Metadata)
        ADD_DICT_OF_STRING_OBJECT(Metadata, "mpris:trackid", "/org/mpris/MediaPlayer2/Player/track_xxx")

        //ADD_DICT_OF_STRING_STRING_FORM_X(Metadata, "mpris:trackid", splay->plyr->curr_playing_index, "%d", 64)
        ADD_DICT_OF_STRING_SIGNED_INT(Metadata, "mpris:length", libvlc_media_get_duration(libvlc_media_list_item_at_index(splay->plyr->mpl, splay->plyr->curr_playing_index))*1000)
                void * title = GET_META(libvlc_meta_Title);
        if (title) {
            ADD_DICT_OF_STRING_STRING(Metadata, "xesam:title", title)
        }
        void * artist = GET_META(libvlc_meta_Artist);
        if (artist) {
            ADD_DICT_OF_STRING_ARRAY_OF_STRING_OPEN(Metadata, "xesam:artist", Artist)
            ADD_STRING(Artist, artist)
            ADD_DICT_OF_STRING_ARRAY_OF_STRING_CLOSE(Metadata, Artist)
        }
        void * album = GET_META(libvlc_meta_Album);
        if (album) {
            ADD_DICT_OF_STRING_STRING(Metadata, "xesam:album", album)
        }
        void * art_url = GET_META(libvlc_meta_ArtworkURL);
        if (art_url) {
            ADD_DICT_OF_STRING_STRING(Metadata, "mpris:artUrl", art_url)
        }
        ADD_DICT_OF_ARRAY_OF_STRING_VARIANT_CLOSE(array, Metadata)
        ADD_DICT_OF_STRING_BOOLEAN(array, "CanGoNext", ply_hasn(splay))
        ADD_DICT_OF_STRING_BOOLEAN(array, "CanGoPrevious", ply_hasp(splay))
        ADD_ARRAY_CLOSE(iter, array)
        ADD_ARRAY_OPEN(iter, array2, "s")
        ADD_ARRAY_CLOSE(iter, array2)
        SIGNAL_CLOSE(conn, reply)
    }


}

void mpris_on_seek() {
    SIGNAL_OPEN(reply, iter, "/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", "Seeked")
            long long int pos = libvlc_media_player_get_time(splay->plyr->mp)*1000;
    ADD_SIGNED_INT(iter, pos)
    SIGNAL_CLOSE(conn, reply)
}

DBusHandlerResult mpris_message_handler(DBusConnection *conn, DBusMessage *message, void *data) {
    DBusHandlerResult result;
    DBusMessage *reply = NULL;
    DBusError err;

    SPLOGF("MPRIS: Request: %s->%s on %s",
            dbus_message_get_interface(message),
            dbus_message_get_member(message),
            dbus_message_get_path(message));

    dbus_error_init(&err);
    if (METHOD_CALL_II("Introspect")) {
        if (reply = dbus_message_new_method_return(message)) {
            dbus_message_append_args(reply,
                    DBUS_TYPE_STRING, &server_introspection_xml,
                    DBUS_TYPE_INVALID);

        }
    } else if (METHOD_CALL_P("Get")) {
        const char *interface, *property;
        if (dbus_message_get_args(message, &err, DBUS_TYPE_STRING, &interface, DBUS_TYPE_STRING, &property, DBUS_TYPE_INVALID) &&
                (reply = dbus_message_new_method_return(message))) {
            if (STR_EQUALS(interface, "org.mpris.MediaPlayer2")) {
                result = mediaplayer2_property_handler(property, conn, reply);
            } else if (STR_EQUALS(interface, "org.mpris.MediaPlayer2.Player")) {
                result = mediaplayer2_player_property_handler(property, conn, reply);
            } else {
                result = DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
            }

            dbus_message_unref(reply);
            return result;
        }
    } else if (METHOD_CALL_P("Set")) {
        reply = dbus_message_new_method_return(message);
    } else if (METHOD_CALL_P("GetAll")) {
        const char *interface;
        ;
        if (dbus_message_get_args(message, &err, DBUS_TYPE_STRING, &interface, DBUS_TYPE_INVALID) && (reply = dbus_message_new_method_return(message))) {
            if (STR_EQUALS(interface, "org.mpris.MediaPlayer2")) {
                result = mediaplayer2_all_properties_handler(conn, reply);
                dbus_message_unref(reply);
                return result;
            } else if (STR_EQUALS(interface, "org.mpris.MediaPlayer2.Player")) {
                result = mediaplayer2_player_all_properties_handler_(conn, reply);
                dbus_message_unref(reply);
                return result;
            } else {
                return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
            }
        }
    } else if (METHOD_CALL("Raise") || METHOD_CALL("Quit") || METHOD_CALL_PLAYER("Stop")) {
        // Not Implemented
        reply = dbus_message_new_method_return(message);
    } else if (METHOD_CALL_PLAYER("Next")) {
        ply_nxt_no_ev_drw(splay);
        reply = dbus_message_new_method_return(message);
    } else if (METHOD_CALL_PLAYER("Previous")) {
        ply_prv_no_ev_drw(splay);
        reply = dbus_message_new_method_return(message);
    } else if (METHOD_CALL_PLAYER("PlayPause")) {
        ply_pp(splay);
        reply = dbus_message_new_method_return(message);
    } else if (METHOD_CALL_PLAYER("Play")) {
        libvlc_media_list_player_play(splay->plyr->mplp);
        reply = dbus_message_new_method_return(message);
    } else if (METHOD_CALL_PLAYER("Pause")) {
        libvlc_media_list_player_pause(splay->plyr->mplp);
        reply = dbus_message_new_method_return(message);
    } else if (METHOD_CALL_PLAYER("Seek")) {
        long long int seek;
        dbus_message_get_args(message, &err, DBUS_TYPE_INT64, &seek, DBUS_TYPE_INVALID);
        long long int time = libvlc_media_player_get_time(splay->plyr->mp);
        libvlc_media_player_set_time(splay->plyr->mp, time + (int) seek / 1000);
        mpris_on_seek();
        reply = dbus_message_new_method_return(message);
    } else if (METHOD_CALL_PLAYER("SetPosition")) {
        long long int position;
        char *track_id;
        dbus_message_get_args(message, &err, DBUS_TYPE_OBJECT_PATH, &track_id, DBUS_TYPE_INT64, &position, DBUS_TYPE_INVALID);
        libvlc_media_player_set_time(splay->plyr->mp, position / 1000);
        mpris_on_seek();
        reply = dbus_message_new_method_return(message);
    } else {
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    if (dbus_error_is_set(&err)) {
        if (reply) {
            dbus_message_unref(reply);
        }
        reply = dbus_message_new_error(message, err.name, err.message);
        SPLOGF("MPRIS: Error: %s, Message", err.name, err.message);
        dbus_error_free(&err);
    }

    if (reply) {
        result = DBUS_HANDLER_RESULT_HANDLED;
        if (!dbus_connection_send(conn, reply, NULL)) {
            result = DBUS_HANDLER_RESULT_NEED_MEMORY;
        }
        dbus_message_unref(reply);
        return result;
    }
    return DBUS_HANDLER_RESULT_NEED_MEMORY;
}

void * mpris_main() {
    DBusError err;

    dbus_error_init(&err);

    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (!conn) {
        SPLOGF("MPRIS: Failed to the the session bus, Error: %s", err.message);
        FAIL_DBUS_STARTUP_ERROR();
    }


    int retval;
    char domain[50];
    snprintf(domain, 50, "org.mpris.MediaPlayer2.splay_%d", getpid());
    retval = dbus_bus_request_name(conn, domain, DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (retval != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        SPLOGF("MPRIS: Requesting the name %s failed: %s", domain, err.message);
        FAIL_DBUS_STARTUP_ERROR();
    }

    if (!dbus_connection_register_object_path(conn, MPRIS2_PATH, &server_vtable, NULL)) {
        SPLOGF("Failed to register the path: %s", MPRIS2_PATH);
        FAIL_DBUS_STARTUP_ERROR();
    }

    SPLOGF("MPRIS: started version: %s", version);

    mainloop = g_main_loop_new(NULL, FALSE);
    dbus_connection_setup_with_g_main(conn, NULL);
    g_main_loop_run(mainloop);

    return NULL;
}