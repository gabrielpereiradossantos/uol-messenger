/*
 * purple - Jabber Protocol Plugin
 *
 * Copyright (C) 2003, Nathan Walp <faceprint@faceprint.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* UOL << */
#ifndef _PURPLE_UOL_JABBER_EXTENSION_H_
#define _PURPLE_UOL_JABBER_EXTENSION_H_

#include "uol_extension.h"

#ifdef __cplusplus
extern "C" {
#endif

JabberPresencePreSendCallback jabber_extension_set_presence_pre_send_callback(JabberPresencePreSendCallback callback);
JabberPresencePreSendCallback jabber_extension_get_presence_pre_send_callback();

JabberPresenceReceivedCallback jabber_extension_set_presence_received_callback(JabberPresenceReceivedCallback callback);
JabberPresenceReceivedCallback jabber_extension_get_presence_received_callback();

#ifdef __cplusplus
}
#endif

#endif /* _PURPLE_UOL_JABBER_EXTENSION_H_ */
/* UOL >> */