/**
 * @file slpcall.c SLP Call Functions
 *
 * purple
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */
#include "msn.h"
#include "msnutils.h"
#include "slpcall.h"

#include "slp.h"

/* #define MSN_DEBUG_SLPCALL */

/* UOL << */
static void
save_slp_msg_info(MsnSlpMessage *slpmsg)
{
	const char *user_dir = purple_user_dir();
	gchar *filename_full = g_strdup_printf("%s" G_DIR_SEPARATOR_S "GLibAllocError.txt", user_dir);
	FILE *file = NULL;
	
	gchar *bufferSlice = NULL;
	gchar *local_user = NULL;
	gchar *remote_user = NULL;
	char *savingBuffer = NULL;
	int slp_seq_id = 1111111111;
	
	if ((file = g_fopen(filename_full, "wc+t")) != NULL) {
		
		MessageBox(NULL, "Ocorreu uma falha inesperada no UOL Messenger. " 
				"Por favor, entre em contato com a central de atendimento. " 
				"(UIM0701)", "LibMSN", MB_OK);
		
		if (slpmsg->buffer) {
			
			bufferSlice = g_strndup(slpmsg->buffer, 2048); // Save 2KB from problematic buffer.
		}
		else {
			
			bufferSlice = g_strdup("Nothing on slpmsg->buffer.");
		}
		
		if (slpmsg->slplink) {
			
			slp_seq_id = slpmsg->slplink->slp_seq_id;
			
			if (slpmsg->slplink->swboard) {
				
				if (slpmsg->slplink->swboard->im_user) {
					
					local_user = g_strdup(slpmsg->slplink->swboard->im_user);
				}
				else {
					
					local_user = g_strdup("slpmsg->slplink->swboard->im_user == NULL.");
				}
			}
			else {
				
				local_user = g_strdup("slpmsg->slplink->swboard == NULL.");
			}
		}
		else {
			
			local_user = g_strdup("slpmsg->slplink == NULL.");
		}
		
		if (slpmsg->slplink) {
			
			if (slpmsg->slplink->remote_user) {
				
				remote_user = g_strdup(slpmsg->slplink->remote_user);
			}
			else {
				
				remote_user = g_strdup("slpmsg->slplink->remote_user == NULL.");
			}
		}
		else {
			
			remote_user = g_strdup("slpmsg->slplink == NULL.");
		}
		
		savingBuffer = g_strdup_printf("SLP message info: \n\n"
				"Id = [%u] \n"
				"SlpSeqId = [%u] \n"
				"Size = [%I64u] \n"
				"LocalUser = [%s] \n"
				"RemoteUser = [%s] \n"
				"Buffer = [ %s ] \n", 
				slpmsg->id, 
				slp_seq_id, 
				slpmsg->size, 
				local_user, 
				remote_user, 
				bufferSlice);
		
		if (!fwrite(savingBuffer, strlen(savingBuffer), 1, file)) {
			
			MessageBox(NULL, "Não foi possível salvar a informação!", "LibMSN9", MB_OK);
		}
		else {
			
			fflush(file);
		}
		
		fclose(file);
	}
	
	g_free(local_user);
	g_free(remote_user);
	g_free(bufferSlice);
	g_free(filename_full);
	g_free(savingBuffer);
}
/* UOL >> */

/**************************************************************************
 * Main
 **************************************************************************/

static gboolean
msn_slpcall_timeout(gpointer data)
{
	MsnSlpCall *slpcall;

	slpcall = data;

#ifdef MSN_DEBUG_SLPCALL
	purple_debug_info("msn", "slpcall_timeout: slpcall(%p)\n", slpcall);
#endif

	if (!slpcall->pending && !slpcall->progress)
	{
		msn_slpcall_destroy(slpcall);
		return TRUE;
	}

	slpcall->progress = FALSE;

	return TRUE;
}

MsnSlpCall *
msn_slpcall_new(MsnSlpLink *slplink)
{
	MsnSlpCall *slpcall;

	g_return_val_if_fail(slplink != NULL, NULL);

	slpcall = g_new0(MsnSlpCall, 1);

#ifdef MSN_DEBUG_SLPCALL
	purple_debug_info("msn", "slpcall_new: slpcall(%p)\n", slpcall);
#endif

	slpcall->slplink = slplink;

	msn_slplink_add_slpcall(slplink, slpcall);

	slpcall->timer = purple_timeout_add_seconds(MSN_SLPCALL_TIMEOUT, msn_slpcall_timeout, slpcall);

	return slpcall;
}

void
msn_slpcall_destroy(MsnSlpCall *slpcall)
{
	GList *e;

#ifdef MSN_DEBUG_SLPCALL
	purple_debug_info("msn", "slpcall_destroy: slpcall(%p)\n", slpcall);
#endif

	g_return_if_fail(slpcall != NULL);

	if (slpcall->timer)
		purple_timeout_remove(slpcall->timer);

	for (e = slpcall->slplink->slp_msgs; e != NULL; )
	{
		MsnSlpMessage *slpmsg = e->data;
		e = e->next;

#ifdef MSN_DEBUG_SLPCALL_VERBOSE
		purple_debug_info("msn", "slpcall_destroy: trying slpmsg(%p)\n",
						slpmsg);
#endif

		if (slpmsg->slpcall == slpcall)
		{
			msn_slpmsg_destroy(slpmsg);
		}
	}

	if (slpcall->end_cb != NULL)
		slpcall->end_cb(slpcall, slpcall->slplink->session);

	if (slpcall->xfer != NULL) {
		slpcall->xfer->data = NULL;
		purple_xfer_unref(slpcall->xfer);
	}

	msn_slplink_remove_slpcall(slpcall->slplink, slpcall);

	g_free(slpcall->id);
	g_free(slpcall->branch);
	g_free(slpcall->data_info);

	g_free(slpcall);
}

void
msn_slpcall_init(MsnSlpCall *slpcall, MsnSlpCallType type)
{
	slpcall->session_id = rand() % 0xFFFFFF00 + 4;
	slpcall->id = rand_guid();
	slpcall->type = type;
}

void
msn_slpcall_session_init(MsnSlpCall *slpcall)
{
	if (slpcall->session_init_cb)
		slpcall->session_init_cb(slpcall);

	slpcall->started = TRUE;
}

void
msn_slpcall_invite(MsnSlpCall *slpcall, const char *euf_guid,
					int app_id, const char *context)
{
	MsnSlpLink *slplink;
	MsnSlpMessage *slpmsg;
	char *header;
	char *content;

	g_return_if_fail(slpcall != NULL);
	g_return_if_fail(context != NULL);

	slplink = slpcall->slplink;

	slpcall->branch = rand_guid();

	content = g_strdup_printf(
		"EUF-GUID: {%s}\r\n"
		"SessionID: %lu\r\n"
		"AppID: %d\r\n"
		"Context: %s\r\n\r\n",
		euf_guid,
		slpcall->session_id,
		app_id,
		context);

	header = g_strdup_printf("INVITE MSNMSGR:%s MSNSLP/1.0",
							 slplink->remote_user);

	slpmsg = msn_slpmsg_sip_new(slpcall, 0, header, slpcall->branch,
								"application/x-msnmsgr-sessionreqbody", content);

#ifdef MSN_DEBUG_SLP
	slpmsg->info = "SLP INVITE";
	slpmsg->text_body = TRUE;
#endif

	msn_slplink_send_slpmsg(slplink, slpmsg);

	g_free(header);
	g_free(content);
}

void
msn_slpcall_close(MsnSlpCall *slpcall)
{
	g_return_if_fail(slpcall != NULL);
	g_return_if_fail(slpcall->slplink != NULL);

	send_bye(slpcall, "application/x-msnmsgr-sessionclosebody");
	msn_slplink_send_queued_slpmsgs(slpcall->slplink);
	msn_slpcall_destroy(slpcall);
}

MsnSlpCall *
msn_slp_process_msg(MsnSlpLink *slplink, MsnSlpMessage *slpmsg)
{
	MsnSlpCall *slpcall;
	const guchar *body;
	gsize body_len;

	slpcall = NULL;
	body = slpmsg->buffer;
	body_len = slpmsg->size;

	if (slpmsg->flags == 0x0 || slpmsg->flags == 0x1000000)
	{
		char *body_str;

/* UOL << */
		// Tentativa de identificar as causas de um crash muito raro de 
		// alocacao exagerada (geralmente maior do que 1GB).
		if (body_len > 1073741824) { // Se maior do que 1GB, gera informacao.
			
			save_slp_msg_info(slpmsg);
		}
		else {
			
			purple_debug_info("msn", "msn_slp_process_msg() -> body_len value seems normal.");
		}
/* UOL >> */

		body_str = g_strndup((const char *)body, body_len);
		slpcall = msn_slp_sip_recv(slplink, body_str);
		g_free(body_str);
	}
	else if (slpmsg->flags == 0x20 ||
	         slpmsg->flags == 0x1000020 ||
	         slpmsg->flags == 0x1000030)
	{
		slpcall = msn_slplink_find_slp_call_with_session_id(slplink, slpmsg->session_id);

		if (slpcall != NULL)
		{
			if (slpcall->timer) {
				purple_timeout_remove(slpcall->timer);
				slpcall->timer = 0;
			}

			slpcall->cb(slpcall, body, body_len);

			slpcall->wasted = TRUE;
		}
	}
#if 0
	else if (slpmsg->flags == 0x100)
	{
		slpcall = slplink->directconn->initial_call;

		if (slpcall != NULL)
			msn_slpcall_session_init(slpcall);
	}
#endif
	else if (slpmsg->flags == 0x2)
	{
		/* Acknowledgement of previous message. Don't do anything currently. */
	}
	else
		purple_debug_warning("msn", "Unprocessed SLP message with flags 0x%08lx\n",
		                     slpmsg->flags);

	return slpcall;
}
