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
#ifndef _PURPLE_UOL_JABBER_UOL_EXTENSION_H_
#define _PURPLE_UOL_JABBER_UOL_EXTENSION_H_

#include <connection.h>
#include <blist.h>
#include <xmlnode.h>

typedef void (*JabberPresencePreSendCallback)(xmlnode* root_node, PurpleConnection* gc);
typedef void (*JabberPresenceReceivedCallback)(xmlnode* root_node, PurpleBuddy* buddy);

struct _JabberExtensions
{
	unsigned int size;
	JabberPresencePreSendCallback (*jabber_set_presence_pre_send_callback)(JabberPresencePreSendCallback callback);
	JabberPresenceReceivedCallback (*jabber_set_presence_received_callback)(JabberPresenceReceivedCallback callback);
};

typedef struct _JabberExtensions JabberExtensions;

#endif /* _PURPLE_UOL_JABBER_UOL_EXTENSION_H_ */
/* UOL >> */