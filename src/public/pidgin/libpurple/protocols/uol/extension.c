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
#include "extension.h"


static JabberPresencePreSendCallback presence_pre_send_callback = NULL;
static JabberPresenceReceivedCallback presence_received_callback = NULL;


JabberPresencePreSendCallback jabber_extension_set_presence_pre_send_callback(JabberPresencePreSendCallback callback)
{
	JabberPresencePreSendCallback old_callback = presence_pre_send_callback;

	presence_pre_send_callback = callback;

	return old_callback;
}

JabberPresencePreSendCallback jabber_extension_get_presence_pre_send_callback()
{
	return presence_pre_send_callback;
}

JabberPresenceReceivedCallback jabber_extension_set_presence_received_callback(JabberPresenceReceivedCallback callback)
{
	JabberPresenceReceivedCallback old_callback = presence_received_callback;

	presence_received_callback = callback;

	return old_callback;
}

JabberPresenceReceivedCallback jabber_extension_get_presence_received_callback()
{
	return presence_received_callback;
}


/* UOL >> */