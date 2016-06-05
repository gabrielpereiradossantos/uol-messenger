/*
  The oRTP library is an RTP (Realtime Transport Protocol - rfc1889) stack.
  Copyright (C) 2001  Simon MORLAT simon.morlat@linphone.org

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/



#include "rtpsession.h"



void rtp_signal_table_init(RtpSignalTable *table,RtpSession *session)
{
	memset(table,0,sizeof(RtpSignalTable));
	table->session=session;
}

int rtp_signal_table_add(RtpSignalTable *table,RtpCallback cb, gpointer user_data)
{
	gint i;
	
	for (i=0;i<RTP_CALLBACK_TABLE_MAX_ENTRIES;i++){
		if (table->callback[i]==NULL){
			table->callback[i]=cb;
			table->user_data[i]=user_data;
			table->count++;
			return 0;
		}
	}
	return -1;
}

void rtp_signal_table_emit(RtpSignalTable *table)
{
	gint i;
	
	for (i=0;i<RTP_CALLBACK_TABLE_MAX_ENTRIES;i++){
		if (table->callback[i]!=NULL){
			table->callback[i](table->session,table->user_data[i]);
		}
	}
}

void rtp_signal_table_emit2(RtpSignalTable *table, gpointer arg)
{
	gint i;
	
	for (i=0;i<RTP_CALLBACK_TABLE_MAX_ENTRIES;i++){
		if (table->callback[i]!=NULL){
			table->callback[i](table->session, arg, table->user_data[i]);
		}
	}
}

int rtp_signal_table_remove_by_callback(RtpSignalTable *table,RtpCallback cb)
{
	gint i;
	
	for (i=0;i<RTP_CALLBACK_TABLE_MAX_ENTRIES;i++){
		if (table->callback[i]==cb){
			table->callback[i]=NULL;
			table->user_data[i]=NULL;
			table->count--;
			return 0;
		}
	}
	return -1;
}


