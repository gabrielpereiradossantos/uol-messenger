/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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
 */

/**
 * @file ow_osip.h
 * 
 * This should help to get less differences between
 * the Wengo version and the original one, in order to
 * submit a patch to the project maintainer.
 */

#ifndef OW_OSIP_H
#define OW_OSIP_H

#include "osip_negotiation.h"
#include "../eXosip2.h"

int __osip_transaction_set_state(osip_transaction_t * transaction, state_t state);

int osip_list_get_first_index (osip_list_t * li, void *el);
int osip_list_add_nodup(osip_list_t * li, void *el, int pos);

/**
 * Remove all elements with a value from a list.
 * @param li The element to work on.
 * @param el: the value of the element to remove.
 */
int osip_list_remove_element(osip_list_t * li, void *el);

int sdp_message_m_payload_mime_get(sdp_message_t * sdp, int pos_media, int payloadnumber, char *result, const size_t result_size);

int osip_dialog_build_replaces_value(osip_dialog_t * dialog, char **str);

/**
 * Clean the Contact header.
 * @param sip The element to work on.
 * @param hvalue The string describing the element.
 */
int osip_message_clean_contacts (osip_message_t * sip);

#endif	//OW_OSIP_H
