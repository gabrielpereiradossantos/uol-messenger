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

#include "../eXosip2.h"
#include "ow_osip.h"

#if defined(WIN32) || defined(_WIN32_WCE)
#define snprintf _snprintf
#endif

int osip_list_get_first_index (osip_list_t * li, void *el)
{
	__node_t *ntmp;
	int i = 0;
	
	if (!li) {
		return -1;
	}
	ntmp = li->node;
	while ((i < li->nb_elt) && ntmp) {
		if (ntmp->element == el) {
			return i;
		}
		i++;
		ntmp = (__node_t *) ntmp->next;
	}
	return -1;
}

int osip_list_add_nodup(osip_list_t * li, void *el, int pos)
{
	if (osip_list_get_first_index(li,el) >= 0)
		return -1;
	return osip_list_add(li,el,pos);
}

/* return -1 if failed */
int
osip_list_remove_element (osip_list_t * li, void *el)
{
	__node_t *ntmp;
	int i = 0;
	ntmp = li->node;
	while ((i < li->nb_elt) && ntmp)
	{
		if (ntmp->element == el)
		{
			ntmp = (__node_t *) ntmp->next;
			osip_list_remove(li,i);
			continue;
		}
		i++;
		ntmp = (__node_t *) ntmp->next;
	}
	return li->nb_elt;
}

/**
 * Get the mime payload string of a media in a SDP message
 * that corresponds to the specified payload number
 * 
 * @param sdp			The sdp message.
 * @param pos_media		The index of media in the SDP message
 * @param payloadnumber	The payload number
 * @param result(out)	The output buffer to contain result
 * @param result		The output buffer size 
 * @return				0 on success. -1 on failure
 */
int sdp_message_m_payload_mime_get(sdp_message_t * sdp, int pos_media, int payloadnumber, char *result, const size_t result_size)
{
	sdp_media_t *med = osip_list_get (&sdp->m_medias, pos_media);
	sdp_attribute_t *attr;
	int i;
	
	if (med == NULL)
		return -1;
	
	for( i = 0; ; i++)
    {
		attr = sdp_message_attribute_get (sdp, pos_media, i);
		
		if (!attr) 
			return -1;
		
		if (!strcmp(attr->a_att_field, "rtpmap") && (atoi(attr->a_att_value) == payloadnumber))
		{
			char *p = attr->a_att_value;
			
			p = strchr(p, ' ');
			if (!p)
				return NULL;
			
			p += strspn(p, " ");
			
			if (*p) {
				if (strlen(p) >= result_size) {
						return -1;
				}
				strcpy(result, p);
				return 0;
			}
												
			return -1;
		}
    }
	return -1;
}

int osip_dialog_build_replaces_value(osip_dialog_t * dialog, char **str)
{
	char *to, *from;
	char* val;
	
	if (dialog->type == CALLER)
    {
		to = dialog->remote_tag;
		from = dialog->local_tag;
    }
	else
    {
		to = dialog->local_tag;
		from = dialog->remote_tag;
    }
	
	val = osip_malloc(256);
	snprintf(val, 255, "%s;to-tag=%s;from-tag=%s", dialog->call_id, to, from);
	*str = val;
	return 0;
}

/* removes all conntacts from contact list */
/* INPUT : osip_message_t *sip | sip message.                       */
/* OUTPUT: osip_contact_t *contact | structure to save results. */
/* returns -1 on error. */
int osip_message_clean_contacts(osip_message_t * sip)
{
	osip_contact_t *ct;
	
	if (sip == NULL)
		return -1;
	
	while(0 != (ct = (osip_contact_t *)osip_list_get (&sip->contacts, 0)))
    {
		osip_list_remove(&sip->contacts, 0);
		sip->message_property = 2;
		osip_contact_free(ct);
    }
	
	return 0;
}
