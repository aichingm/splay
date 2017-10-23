/* 
 * File:   mpris.h
 * Author: Mario Aichinger <aichingm@gmail.com>
 *
 * Created on October 21, 2017, 10:07 AM
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

#ifndef MPRIS2_H
#define MPRIS2_H

#include <stdio.h>
#include <dbus-1.0/dbus/dbus-protocol.h>

#include "logging.h"


void* mpris_main();



#define MPRIS2_PATH "/org/mpris/MediaPlayer2"
#define MPRIS2_IDENTITY "splay"

#define PROPERTY_IS(PROPERTY) !strcmp(property, PROPERTY)

#define GET_META(TAG) libvlc_media_get_meta(libvlc_media_list_item_at_index(splay->plyr->mpl, splay->plyr->curr_playing_index), TAG)

#define METHOD_CALL_II(NAME) dbus_message_is_method_call(message, DBUS_INTERFACE_INTROSPECTABLE, NAME)
#define METHOD_CALL_P(NAME) dbus_message_is_method_call(message, DBUS_INTERFACE_PROPERTIES, NAME)
#define METHOD_CALL(NAME) dbus_message_is_method_call(message, "org.mpris.MediaPlayer2", NAME)
#define METHOD_CALL_PLAYER(NAME) dbus_message_is_method_call(message, "org.mpris.MediaPlayer2.Player", NAME)
#define METHOD_CALL_TRACKLIST(NAME) dbus_message_is_method_call(message, "org.mpris.MediaPlayer2.TrackList", NAME)
#define STR_EQUALS(X, Y) !strcmp(X,Y)

#define FAIL_DBUS_STARTUP_ERROR() \
    dbus_error_free(&err); \
    return NULL;

#define ADD_STRING(CONTAINER, VAL) \
    {\
        const char *  value = VAL;\
        dbus_message_iter_append_basic(&CONTAINER, DBUS_TYPE_STRING, &value);\
    } \
    
#define ADD_SIGNED_INT(CONTAINER, VAL) \
    {\
        signed long long int value = VAL;\
        dbus_message_iter_append_basic(&CONTAINER, DBUS_TYPE_INT64, &value);\
    } \

#define ADD_BOOLEAN(CONTAINER, VAL) \
    {\
        signed long long int value = VAL;\
        dbus_message_iter_append_basic(&CONTAINER, DBUS_TYPE_BOOLEAN, &value);\
    } \

#define ADD_DICT_OF_STRING_BOOLEAN(CONTAINER, KEY, VAL) \
    {\
        const char * key = KEY;\
        dbus_bool_t value = VAL;\
        DBusMessageIter dict, variant;\
        dbus_message_iter_open_container(&CONTAINER, DBUS_TYPE_DICT_ENTRY, NULL, &dict);\
        dbus_message_iter_append_basic(&dict, DBUS_TYPE_STRING, &key);\
        dbus_message_iter_open_container(&dict, DBUS_TYPE_VARIANT, "b", &variant);\
        dbus_message_iter_append_basic(&variant, DBUS_TYPE_BOOLEAN, &value);\
        dbus_message_iter_close_container(&dict, &variant);\
        dbus_message_iter_close_container(&CONTAINER, &dict);\
    } \

#define ADD_DICT_OF_STRING_STRING(CONTAINER, KEY, VAL) \
    {\
        const char * key = KEY;\
        const char *  value = VAL;\
        DBusMessageIter dict, variant;\
        dbus_message_iter_open_container(&CONTAINER, DBUS_TYPE_DICT_ENTRY, NULL, &dict);\
        dbus_message_iter_append_basic(&dict, DBUS_TYPE_STRING, &key);\
        dbus_message_iter_open_container(&dict, DBUS_TYPE_VARIANT, "s", &variant);\
        dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &value);\
        dbus_message_iter_close_container(&dict, &variant);\
        dbus_message_iter_close_container(&CONTAINER, &dict);\
    } \
    
#define ADD_DICT_OF_STRING_OBJECT(CONTAINER, KEY, VAL) \
    {\
        const char * key = KEY;\
        const char *  value = VAL;\
        DBusMessageIter dict, variant;\
        dbus_message_iter_open_container(&CONTAINER, DBUS_TYPE_DICT_ENTRY, NULL, &dict);\
        dbus_message_iter_append_basic(&dict, DBUS_TYPE_STRING, &key);\
        dbus_message_iter_open_container(&dict, DBUS_TYPE_VARIANT, "o", &variant);\
        dbus_message_iter_append_basic(&variant, DBUS_TYPE_OBJECT_PATH, &value);\
        dbus_message_iter_close_container(&dict, &variant);\
        dbus_message_iter_close_container(&CONTAINER, &dict);\
    } \
    
#define ADD_DICT_OF_STRING_STRING_FORM_X(CONTAINER, KEY, VAL, FORMAT, LENGTH) \
    {\
        const char * key = KEY;\
        char  * string_to_fill = malloc(sizeof(char)*LENGTH); \
        snprintf(string_to_fill, LENGTH, FORMAT, VAL); \
        const char *  value = string_to_fill;\
        DBusMessageIter dict, variant;\
        dbus_message_iter_open_container(&CONTAINER, DBUS_TYPE_DICT_ENTRY, NULL, &dict);\
        dbus_message_iter_append_basic(&dict, DBUS_TYPE_STRING, &key);\
        dbus_message_iter_open_container(&dict, DBUS_TYPE_VARIANT, "s", &variant);\
        dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &value);\
        dbus_message_iter_close_container(&dict, &variant);\
        dbus_message_iter_close_container(&CONTAINER, &dict);\
    } \

#define ADD_DICT_OF_STRING_DOUBLE(CONTAINER, KEY, VAL) \
    {\
        const char * key = KEY;\
        double value = VAL;\
        DBusMessageIter dict, variant;\
        dbus_message_iter_open_container(&CONTAINER, DBUS_TYPE_DICT_ENTRY, NULL, &dict);\
        dbus_message_iter_append_basic(&dict, DBUS_TYPE_STRING, &key);\
        dbus_message_iter_open_container(&dict, DBUS_TYPE_VARIANT, "d", &variant);\
        dbus_message_iter_append_basic(&variant, DBUS_TYPE_DOUBLE, &value);\
        dbus_message_iter_close_container(&dict, &variant);\
        dbus_message_iter_close_container(&CONTAINER, &dict);\
    } \
    
#define ADD_DICT_OF_STRING_SIGNED_INT(CONTAINER, KEY, VAL) \
    {\
        const char * key = KEY;\
        signed long long int value = VAL;\
        DBusMessageIter dict, variant;\
        dbus_message_iter_open_container(&CONTAINER, DBUS_TYPE_DICT_ENTRY, NULL, &dict);\
        dbus_message_iter_append_basic(&dict, DBUS_TYPE_STRING, &key);\
        dbus_message_iter_open_container(&dict, DBUS_TYPE_VARIANT, "x", &variant);\
        dbus_message_iter_append_basic(&variant, DBUS_TYPE_INT64, &value);\
        dbus_message_iter_close_container(&dict, &variant);\
        dbus_message_iter_close_container(&CONTAINER, &dict);\
    } \

#define ADD_DICT_OF_ARRAY_OF_STRING_VARIANT_OPEN(CONTAINER, KEY, ARRAY) \
    { \
        const char * key = KEY; \
        DBusMessageIter dict, variant, ARRAY; \
        dbus_message_iter_open_container(&CONTAINER, DBUS_TYPE_DICT_ENTRY, NULL, &dict); \
        dbus_message_iter_append_basic(&dict, DBUS_TYPE_STRING, &key); \
        dbus_message_iter_open_container(&dict, DBUS_TYPE_VARIANT, "a{sv}", &variant); \
        dbus_message_iter_open_container(&variant, DBUS_TYPE_ARRAY, "{sv}", &ARRAY);

#define ADD_DICT_OF_ARRAY_OF_STRING_VARIANT_CLOSE(CONTAINER, ARRAY) \
        dbus_message_iter_close_container(&variant, &ARRAY); \
        dbus_message_iter_close_container(&dict, &variant); \
        dbus_message_iter_close_container(&CONTAINER, &dict); \
    } \


#define ADD_DICT_OF_STRING_ARRAY_OF_STRING_OPEN(CONTAINER, KEY, ARRAY) \
    { \
        const char * key = KEY; \
        DBusMessageIter dict, variant, ARRAY; \
        dbus_message_iter_open_container(&CONTAINER, DBUS_TYPE_DICT_ENTRY, NULL, &dict); \
        dbus_message_iter_append_basic(&dict, DBUS_TYPE_STRING, &key); \
        dbus_message_iter_open_container(&dict, DBUS_TYPE_VARIANT, "as", &variant); \
        dbus_message_iter_open_container(&variant, DBUS_TYPE_ARRAY, "s", &ARRAY);

#define ADD_DICT_OF_STRING_ARRAY_OF_STRING_CLOSE(CONTAINER, ARRAY) \
        dbus_message_iter_close_container(&variant, &ARRAY); \
        dbus_message_iter_close_container(&dict, &variant); \
        dbus_message_iter_close_container(&CONTAINER, &dict); \
    } \
    


#define ADD_ARRAY_OPEN(CONTAINER, ARRAY, DBUS_TYPE) \
    { \
        DBusMessageIter ARRAY; \
        dbus_message_iter_open_container(&CONTAINER, DBUS_TYPE_ARRAY, DBUS_TYPE, &ARRAY);

#define ADD_ARRAY_CLOSE(CONTAINER, ARRAY) \
        dbus_message_iter_close_container(&CONTAINER, &ARRAY); \
    } \

#define ADD_DICT_ENTRY_OPEN(CONTAINER, DICT) \
    { \
        DBusMessageIter DICT; \
        dbus_message_iter_open_container(&CONTAINER, DBUS_TYPE_DICT_ENTRY, NULL, &DICT);

#define ADD_DICT_ENTRY_CLOSE(CONTAINER, DICT) \
        dbus_message_iter_close_container(&CONTAINER, &DICT); \
    } \
    
#define ADD_VARIANT_OPEN(CONTAINER, VARIANT, DBUS_TYPE) \
    { \
        DBusMessageIter VARIANT; \
        dbus_message_iter_open_container(&CONTAINER, DBUS_TYPE_VARIANT, DBUS_TYPE, &VARIANT);\

#define ADD_VARIANT_CLOSE(CONTAINER, VARIANT) \
        dbus_message_iter_close_container(&CONTAINER, &VARIANT); \
    } \


#define OPEN_ITER(REPLY, ITER) \
        DBusMessageIter ITER;\
        dbus_message_iter_init_append(REPLY, &ITER);\
    


#define SIGNAL_OPEN(REPLY, ITERATOR, PATH, DOMAIN, SIG_NAME)\
    { \
        dbus_uint32_t serial = 0;\
        DBusMessage* REPLY;\
        DBusMessageIter ITERATOR;\
        if (REPLY = dbus_message_new_signal(PATH, DOMAIN, SIG_NAME)){ \
        dbus_message_iter_init_append(REPLY, &ITERATOR);\
        


#define SIGNAL_CLOSE(CONNECTION, REPLY) \
            if(dbus_connection_send(CONNECTION, REPLY, &serial)){ \
                dbus_connection_flush(CONNECTION); \
            } \
            dbus_message_unref(REPLY); \
        } \
   } \


#endif 

