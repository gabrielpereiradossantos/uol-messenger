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


#ifndef ORTP_H
#define ORTP_H

#ifdef WIN32
#include <winsock2.h>
#endif

#include "rtpsession.h"

#ifdef BUILD_SCHEDULER
#include "scheduler.h"
#include "sessionset.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif
void ortp_init();
void ortp_scheduler_init();
void ortp_exit();


#ifndef TARGET_IS_HPUXKERNEL
void ortp_set_debug_file(gchar *domain, FILE *file);
/* domain is ignored when not compiling with glib support */
void ortp_set_log_handler(gchar *domain, GLogFunc func, gpointer ud);
#endif

#ifdef BUILD_SCHEDULER
RtpScheduler * ortp_get_scheduler();
#else
#define ortp_get_scheduler() 	(NULL)
#endif

extern rtp_stats_t ortp_global_stats;

void ortp_global_stats_reset();
rtp_stats_t *ortp_get_global_stats();

void ortp_global_stats_display();
void rtp_stats_display(rtp_stats_t *stats, char *header);
void ortp_session_stats_display(RtpSession *session);

#ifdef __cplusplus
}
#endif

#endif
