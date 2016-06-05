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

#include <string.h>

#include "ortp.h"

rtp_stats_t ortp_global_stats;

#ifdef ENABLE_MEMCHECK
gint ortp_allocations=0;
#endif

#ifdef BUILD_SCHEDULER
static RtpScheduler *ortp_scheduler;
#endif


void dummy_log(const gchar *log_domain,
                                             GLogLevelFlags log_level,
                                             const gchar *message,
                                             gpointer user_data)
{
	return;
}

extern void av_profile_init(RtpProfile *profile);

/**
 *ortp_init:
 *
 *	Initialize the oRTP library. You should call this function first before using
 *	oRTP API.
**/
 
void ortp_init()
{
#ifdef _WIN32
#define WORD unsigned short	
	WORD wVersionRequested;
	WSADATA wsaData;
#endif /* !_WIN32 */
	static gboolean initialized=FALSE;

/* <UOLFONE> */
#ifdef MBDB_LOCAL_POOL
	mbdb_mutex_init();
#endif
	ortp_global_stats_reset();
/* </UOLFONE> */

	if (initialized) return;
	initialized=TRUE;
#ifndef _WIN32
	if (sysconf(_SC_OPEN_MAX)>FD_SETSIZE){
		g_warning("Your kernel can handle much more file descriptors (sockets+files) than ortp does.");
		g_warning("This can result in bugs and crash when using many file descriptors.");
		g_warning("If you want to get rid of this problem, consider recompiling oRTP on your target machine.");
	}
#endif
#ifdef _WIN32
	wVersionRequested = MAKEWORD( 1, 0 );

	if (WSAStartup(wVersionRequested,&wsaData)!=0) 
	{
		g_error("Fail to initialise socket api");
	}
#endif

#ifdef HAVE_GLIB
	g_log_set_handler ("oRTP", G_LOG_LEVEL_MESSAGE, dummy_log, NULL);
	if (!g_thread_supported()) g_thread_init (NULL);
#endif
	av_profile_init(&av_profile);

/* <UOLFONE> */
/*
#ifdef MBDB_LOCAL_POOL
	mbdb_mutex_init();
#endif
	ortp_global_stats_reset();
*/
/* </UOLFONE> */
}


/**
 *ortp_scheduler_init:
 *
 *	Initialize the oRTP scheduler. You only have to do that if you intend to use the
 *	scheduled mode of the #RtpSession in your application.
 *	
**/
void ortp_scheduler_init()
{
	static gboolean initialized=FALSE;
	if (initialized) return;
	initialized=TRUE;
#ifdef BUILD_SCHEDULER
#ifdef HOST_IS_HPUX
	/* on hpux, we must block sigalrm on the main process, because signal delivery
	is ?random?, well, sometimes the SIGALRM goes to both the main thread and the 
	scheduler thread */
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set,SIGALRM);
	sigprocmask(SIG_BLOCK,&set,NULL);
#endif /*HOST_IS_HPUX*/
	if (!g_thread_supported()) g_thread_init(NULL);
	ortp_scheduler=rtp_scheduler_new();
	rtp_scheduler_start(ortp_scheduler);
	//sleep(1);
#else
	g_warning("ortp_init_scheduler: ERROR: scheduler is not compiled !");
#endif
}


/**
 *ortp_exit:
 *
 * Gracefully uninitialize the library, including shutdowning the scheduler if it was started.
 *	
**/
void ortp_exit()
{
#ifdef BUILD_SCHEDULER
	if (ortp_scheduler!=NULL)
	{
		rtp_scheduler_destroy(ortp_scheduler);
		ortp_scheduler=NULL;
	}
#endif

/* <UOLFONE> */
#ifdef MBDB_LOCAL_POOL
	mbdb_mutex_cleanup();
#endif
/* </UOLFONE> */
}

/**
 *ortp_get_scheduler:
 *
 *	Returns a pointer to the scheduler, NULL if it was not running.
 *	The application developer should have to call this function.
 *
 *Returns: a pointer to the scheduler.
**/
#ifdef BUILD_SCHEDULER
RtpScheduler * ortp_get_scheduler()
{
	if (ortp_scheduler==NULL) g_error("Cannot use the scheduled mode: the scheduler is not "
									"started. Call ortp_scheduler_init() at the begginning of the application.");
	return ortp_scheduler;
}
#endif


void ortp_log(const gchar *log_domain,GLogLevelFlags log_level,
                                           const gchar *message,
                                           gpointer user_data)
{
	gchar *lev;
	switch(log_level){
		case G_LOG_LEVEL_MESSAGE:
			lev="message";
			break;
		case G_LOG_LEVEL_WARNING:
			lev="warning";
			break;
		case G_LOG_LEVEL_ERROR:
			lev="error";
		default:
			lev="(unknown log type)";
	}
	if (user_data==NULL)
		printf("%s-%s:%s\n",log_domain,lev,message);
	else
		fprintf((FILE*)user_data,"%s-%s:%s\n",log_domain,lev,message);
}


#ifndef TARGET_IS_HPUXKERNEL
/**
 *ortp_set_debug_file:
 *@domain:	one of "oRTP" or "oRTP-stats" logging domain.
 *@file: a FILE pointer where to output the messages from the domain.
 *
 * Warning: domain is ignored when not compiling with glib support.
**/
void ortp_set_debug_file(gchar *domain,FILE *file)
{
	if (file!=NULL)
		g_log_set_handler (domain, G_LOG_LEVEL_MASK, ortp_log, (gpointer)file);
	else g_log_set_handler (domain, G_LOG_LEVEL_MASK, dummy_log, NULL);
}
/**
 *ortp_set_log_handler:
 *@domain: one of the "oRTP" or "oRTP-stats" logging domain.
 *@func: your logging function, compatible with the GLogFunc prototype.
 *
 * Warning: domain is ignored when not compiling with glib support.
**/
void ortp_set_log_handler(gchar *domain, GLogFunc func, gpointer userdata){
	g_log_set_handler(domain,G_LOG_LEVEL_MASK,func,userdata);
}
#endif



void ortp_global_stats_display()
{
	rtp_stats_display(&ortp_global_stats,"Global statistics");
#ifdef ENABLE_MEMCHECK	
	printf("Unfreed allocations: %i\n",ortp_allocations);
#endif
}

void ortp_session_stats_display(RtpSession *ses)
{
	printf("Statictics for payload type %d\n", ses->rcv.payload_type);
	rtp_stats_display( rtp_session_get_stats(ses), "Session statistics");
}

void rtp_stats_display(rtp_stats_t *stats, char *header)
{
#ifndef WIN32
	g_log("oRTP-stats",G_LOG_LEVEL_MESSAGE,
			"\n   %s :\n packet_sent=%lld\n sent=%lld bytes\n packet_recv=%lld\n hw_recv=%lld bytes\n recv=%lld bytes\n unavailable=%lld packets\n outoftime=%lld bytes\n bad=%lld\n discarded=%lld\n thrown=%lld\n",
			header,stats->packet_sent,stats->sent,stats->packet_recv,stats->hw_recv, stats->recv,stats->unavaillable,
			stats->outoftime,stats->bad,stats->discarded, stats->thrown);
#else
	g_log("oRTP-stats",G_LOG_LEVEL_MESSAGE,
			"\n   %s :\n packet_sent=%I64d\n sent=%I64d bytes\n packet_recv=%I64d\n hw_recv=%I64d bytes\n recv=%I64d bytes\n unavailable=%I64d packets\n outoftime=%I64d bytes\n bad=%I64d\n discarded=%I64d\n thrown=%I64d\n",
			header,stats->packet_sent,stats->sent,stats->packet_recv,stats->hw_recv, stats->recv,stats->unavaillable,
			stats->outoftime,stats->bad,stats->discarded, stats->thrown);
#endif
}

void ortp_global_stats_reset(){
	memset(&ortp_global_stats,0,sizeof(rtp_stats_t));
}

rtp_stats_t *ortp_get_global_stats(){
	return &ortp_global_stats;
}
