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

#include "ortp.h"
#include "rtpmod.h"
#include "str_utils.h"


#ifdef TARGET_IS_HPUXKERNEL
#else
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#ifdef INET6
#include <netdb.h>
#endif

#else

#include <winsock.h>
#include "errno-win32.h"
#endif
#include <stdlib.h>

#include "telephonyevents.h"
#include "scheduler.h"
#endif

#include "port_fct.h"

#ifndef RTP_MAX_SIZE
#define RTP_MAX_SIZE 1024
#endif


#ifdef INET6
#define NETADDR_SIZE(a) (sizeof(a))
#else
#define NETADDR_SIZE(a) (sizeof(struct sockaddr_in))
#endif


#define SYNC_OPT 1


#define USING_TUNNEL(s) ((s)->flags & RTP_SESSION_USING_TUNNELS)
#define TUNNEL_SEND(t, b, l) ((t)->tunnel_send((t)->h_tunnel, b, l))
#define TUNNEL_RECV(t, b, l) ((t)->tunnel_recv((t)->h_tunnel, b, l))
#define TUNNEL_CLOSE(t) { (t)->tunnel_close((t)->h_tunnel); (t)->h_tunnel = 0; }  
#define TUNNEL_SOCKET(t) ((t)->tunnel_get_socket((t)->h_tunnel))


static int rtp_ses_count;
int ortp_session_opbject_size = sizeof(RtpSession);

void
rtp_session_set_cng_pts (RtpSession * session, int spt, int rpt)
{
  session->snd.cng_pt = spt;
  session->rcv.cng_pt = rpt;
}

void
rtp_session_set_markbit (RtpSession * session, int mark)
{
  session->markbit = mark;
}

static void
tvsub (register struct timeval *out, register struct timeval *in)
{
  out->tv_usec -= in->tv_usec;

  while (out->tv_usec < 0)
    {
      --out->tv_sec;
      out->tv_usec += 1000000;
    }

  out->tv_sec -= in->tv_sec;
}

static gint32 biggestdiff = 0;
static gint32 bigcnt = 0;
static gint32 lowcnt = 0;
void
check_timings (mblk_t * m)
{
#if DBG_DELAY
  struct timeval now;
  gint32 diff;
  gettimeofday (&now, NULL);
  tvsub (&now, &m->bts);
  diff = now.tv_sec * 1000 + (now.tv_usec) / 1000;
  if (diff < 50)
    {
      lowcnt++;
    }
  if (diff > 100)
    {
      bigcnt++;
      //      printf("diff %d\n", diff);
    }
  if (diff > biggestdiff)
    {
      biggestdiff = diff;
    }
#endif
}


void
rtp_session_init (RtpSession * session, gint mode, const char *cname)
{
        int i;
	memset (session, 0, sizeof (RtpSession));
	session->rtp.max_rq_size = RTP_MAX_RQ_SIZE;
	session->rtp.jitt_comp_time = RTP_DEFAULT_JITTER;
	session->mode = mode;
	if ((mode == RTP_SESSION_RECVONLY) || (mode == RTP_SESSION_SENDRECV))
	{
		rtp_session_set_flag (session, RTP_SESSION_RECV_SYNC);
		rtp_session_set_flag (session, RTP_SESSION_RECV_NOT_STARTED);
	}
	if ((mode == RTP_SESSION_SENDONLY) || (mode == RTP_SESSION_SENDRECV))
	{
		rtp_session_set_flag (session, RTP_SESSION_SEND_NOT_STARTED);
		rtp_session_set_flag (session, RTP_SESSION_SEND_SYNC);
	}
	session->snd.telephone_events_pt= 	/* not defined a priori */
	  session->rcv.telephone_events_pt=-1;	
	session->snd.cng_pt=
	  session->rcv.cng_pt=-1;	/* not defined a priori */
	rtp_session_set_profiles (session, &av_profile, &av_profile);

#ifndef TARGET_IS_HPUXKERNEL
  session->rtp.rq = &session->rtp._rq;
  session->rtp.wq = &session->rtp._wq;
#endif
  session->lock = g_mutex_new ();
  /* init signal tables */
  rtp_signal_table_init (&session->on_ssrc_changed, session);
  rtp_signal_table_init (&session->on_payload_type_changed, session);
  rtp_signal_table_init (&session->on_telephone_event, session);
  rtp_signal_table_init (&session->on_telephone_event_packet, session);
  rtp_signal_table_init (&session->on_timestamp_jump, session);
  rtp_signal_table_init (&session->on_cng_packet, session);
#ifdef BUILD_SCHEDULER
  session->rtp.wait_for_packet_to_be_sent_mutex = g_mutex_new ();
  session->rtp.wait_for_packet_to_be_sent_cond = g_cond_new ();
  session->rtp.wait_for_packet_to_be_recv_mutex = g_mutex_new ();
  session->rtp.wait_for_packet_to_be_recv_cond = g_cond_new ();
#endif
  session->max_buf_size = RTP_MAX_SIZE;
#ifdef MBDB_LOCAL_POOL
  if (rtp_ses_count++ == 0)
    mbdb_init ();

  for (i = 0; i < RTP_SESSION_RECV_POOL_SIZE; i++)
    session->recvbuffer[i] = allocb (session->max_buf_size, 0);

  for (i = 0; i < RTP_SESSION_SEND_POOL_SIZE; i++)
    session->sendbuffer[i] = allocb (session->max_buf_size, 0);
#endif

	session->adapt_rx_rate = 0;      /* throw max 5% of packets (each 20th) */ 
	session->rtp_rq_throw_cnt = 0; 
	session->rtp_rq_threshold = session->rtp.max_rq_size/2; 

	gettimeofday (&session->start_time, 0);
	session->base_timestamp = random ();
	session->local.ssrc = random ();

	strncpy(session->local.cname, ((cname != 0) && cname[0]) ? cname : "test@mbdsys.com", sizeof(session->local.cname));
}



void
rtp_session_set_cname(RtpSession *session, const char *cname)
{
  strncpy(session->local.cname, cname, sizeof(session->local.cname));
}

/**
 *rtp_session_new:
 *@mode: One of the #RtpSessionMode flags.
 *
 *	Creates a new rtp session.
 *
 *Returns: the newly created rtp session.
**/


RtpSession *
rtp_session_new (gint mode)
{
  RtpSession *session;
  
  
#ifdef WORDS_BIGENDIAN
  printf("rtp_session_new: BIGENDIAN\n");
#else
  printf("rtp_session_new: LITTLEENDIAN\n");
#endif
 
#ifdef TARGET_IS_HPUXKERNEL
  MALLOC (session, RtpSession *, sizeof (RtpSession), M_IOSYS, M_WAITOK);
#else
  session = g_malloc (sizeof (RtpSession));
#endif
	rtp_session_init (session, mode, 0);
	return session;
}


/**
 *rtp_session_set_scheduling_mode:
 *@session: a rtp session.
 *@yesno:	a boolean to indicate the scheduling mode.
 *
 *	Sets the scheduling mode of the rtp session. If @yesno is 1, the rtp session is in
 *	the scheduled mode: this means that packet input/output for that session
 *	is done by a thread that is in charge of getting and sending packet at regular time
 *	interval. This is very usefull for outgoing packets, that have to be sent at a time that
 *	matches their timestamp.
 *	If @yesno is zero, then the session is not scheduled. All recv and send operation will
 *	occur when calling respectively rtp_session_recv_with_ts() and rtp_session_send_with_ts().
 *
**/

void
rtp_session_set_scheduling_mode (RtpSession * session, gint yesno)
{
  if (yesno)
    {
#ifdef BUILD_SCHEDULER
      RtpScheduler *sched;
      sched = ortp_get_scheduler ();
      if (sched != NULL)
	{
	  rtp_session_set_flag (session, RTP_SESSION_SCHEDULED);
	  session->sched = sched;
	  rtp_scheduler_add_session (sched, session);
	}
      else
	g_warning
	  ("rtp_session_set_scheduling_mode: Cannot use scheduled mode because the "
	   "scheduler is not started. Use ortp_scheduler_init() before.");
#else
      g_warning
	("rtp_session_set_scheduling_mode: Cannot use scheduled mode because the "
	 "scheduler is not compiled within this oRTP stack.");
#endif
    }
  else
    rtp_session_unset_flag (session, RTP_SESSION_SCHEDULED);
}


/**
 *rtp_session_set_blocking_mode:
 *@session: a rtp session
 *@yesno: a boolean
 *
 *	This function defines the behaviour of the rtp_session_recv_with_ts() and 
 *	rtp_session_send_with_ts() functions. If @yesno is 1, rtp_session_recv_with_ts()
 *	will block until it is time for the packet to be received, according to the timestamp
 *	passed to the function. After this time, the function returns.
 *	For rtp_session_send_with_ts(), it will block until it is time for the packet to be sent.
 *	If @yesno is 0, then the two functions will return immediately.
 *
**/
void
rtp_session_set_blocking_mode (RtpSession * session, gint yesno)
{
  if (yesno)
    rtp_session_set_flag (session, RTP_SESSION_BLOCKING_MODE);
  else
    rtp_session_unset_flag (session, RTP_SESSION_BLOCKING_MODE);
}

/**
 *rtp_session_set_profile:
 *@session: a rtp session
 *@profile: a rtp profile
 *
 *	Set the RTP profile to be used for the session. By default, all session are created by
 *	rtp_session_new() are initialized with the AV profile, as defined in RFC 1890. The application
 *	can set any other profile instead using that function.
 *
 *
**/
void
rtp_session_set_profile (RtpSession * session, RtpProfile * profile)
{
  session->snd.profile = profile;
  session->rcv.profile = profile;
  rtp_session_telephone_events_supported (session);
}


void
rtp_session_set_profiles (RtpSession * session, RtpProfile * sprofile, RtpProfile * rprofile)
{
  session->snd.profile = sprofile;
  session->rcv.profile = rprofile;
  rtp_session_telephone_events_supported (session);
}

/**
 *rtp_session_signal_connect:
 *@session: 	a rtp session
 *@signal:		the name of a signal
 *@cb:			a #RtpCallback
 *@user_data:	a pointer to any data to be passed when invoking the callback.
 *
 *	This function provides the way for an application to be informed of various events that
 *	may occur during a rtp session. @signal is a string identifying the event, and @cb is 
 *	a user supplied function in charge of processing it. The application can register
 *	several callbacks for the same signal, in the limit of #RTP_CALLBACK_TABLE_MAX_ENTRIES.
 *	Here are name and meaning of supported signals types:
 *
 *	"ssrc_changed" : the SSRC of the incoming stream has changed.
 *
 *	"payload_type_changed" : the payload type of the incoming stream has changed.
 *
 *	"telephone-event_packet" : a telephone-event rtp packet (RFC1833) is received.
 *
 *	"telephone-event" : a telephone event has occured. This is a shortcut for "telephone-event_packet".
 *
 *	Returns: 0 on success, -EOPNOTSUPP if the signal does not exists, -1 if no more callbacks
 *	can be assigned to the signal type.
**/
int
rtp_session_signal_connect (RtpSession * session, char *signal,
			    RtpCallback cb, gpointer user_data)
{
  if (strcmp (signal, "ssrc_changed") == 0)
    {
      return rtp_signal_table_add (&session->on_ssrc_changed, cb, user_data);
    }
  else if (strcmp (signal, "payload_type_changed") == 0)
    {
      return rtp_signal_table_add (&session->
				   on_payload_type_changed, cb, user_data);
    }
  else if (strcmp (signal, "telephone-event") == 0)
    {
      return rtp_signal_table_add (&session->on_telephone_event, cb,
				   user_data);
    }
  else if (strcmp (signal, "telephone-event_packet") == 0)
    {
      return rtp_signal_table_add (&session->on_telephone_event_packet, cb,
				   user_data);
    }
  else if (strcmp (signal, "timestamp_jump") == 0)
    {
      return rtp_signal_table_add (&session->on_timestamp_jump, cb,
				   user_data);
    }
  else if (strcmp (signal, "bye_packet") == 0)
    {
      return rtp_signal_table_add (&session->on_bye_packet, cb, user_data);
    }
  else if (strcmp (signal, "cng_packet") == 0)
    {
      return rtp_signal_table_add (&session->on_cng_packet, cb, user_data);
    }
  g_warning ("rtp_session_signal_connect: inexistant signal.");
  return -EOPNOTSUPP;
}


/**
 *rtp_session_signal_disconnect_by_callback:
 *@session: a rtp session
 *@signal:	a signal name
 *@cb:		a callback function.
 *
 *	Removes callback function @cb to the list of callbacks for signal @signal.
 *
 *Returns: 0 on success, -ENOENT if the callbacks was not found.
**/

int
rtp_session_signal_disconnect_by_callback (RtpSession * session, char *signal,
					   RtpCallback cb)
{
  if (strcmp (signal, "ssrc_changed") == 0)
    {
      return rtp_signal_table_remove_by_callback (&session->
						  on_ssrc_changed, cb);
    }
  else if (strcmp (signal, "payload_type_changed") == 0)
    {
      return rtp_signal_table_remove_by_callback (&session->
						  on_payload_type_changed,
						  cb);
    }
  else if (strcmp (signal, "telephone-event") == 0)
    {
      return rtp_signal_table_remove_by_callback (&session->
						  on_telephone_event, cb);
    }
  else if (strcmp (signal, "telephone-event_packet") == 0)
    {
      return rtp_signal_table_remove_by_callback (&session->
						  on_telephone_event_packet,
						  cb);
    }
  g_warning
    ("rtp_session_signal_disconnect_by_callback: callback not found.");
  return -ENOENT;
}

/**
 *rtp_session_set_callbacks:
 *@session:		a rtp session.
 *@pre_recv_cb:		a callback for pre-recv action
 *@post_recv_cb:	a callback for post-recv action
 *@pre_send_cb:		a callback for pre-send action
 *@post_send_cb:	a callback for post-send action
 *
 *	Set callbacks for pre/post actions on each RTP packet that has been sent or received 
 *	Returns: 0 on success.
**/
int
rtp_session_set_callbacks (RtpSession *session, 
			   RtpCallback pre_recv_cb, RtpCallback post_recv_cb,
			   RtpCallback pre_send_cb, RtpCallback post_send_cb
			   )
{
  session->rtp_pre_recv_cb = pre_recv_cb;
  session->rtp_post_recv_cb = post_recv_cb;
  session->rtp_pre_send_cb = pre_send_cb;
  session->rtp_post_send_cb = post_send_cb;
  return 0;
}


/**
 *rtp_session_set_local_addr:
 *@session:		a rtp session freshly created.
 *@addr:		a local IP address in the xxx.xxx.xxx.xxx form.
 *@port:		a local port.
 *
 *	Specify the local addr to be use to listen for rtp packets or to send rtp packet from.
 *	In case where the rtp session is send-only, then it is not required to call this function:
 *	when calling rtp_session_set_remote_addr(), if no local address has been set, then the 
 *	default INADRR_ANY (0.0.0.0) IP address with a random port will be used. Calling 
 *	rtp_sesession_set_local_addr() is mandatory when the session is send-only or duplex.
 *
 *	Returns: 0 on success.
**/
#ifdef TARGET_IS_HPUXKERNEL
gint
rtp_session_set_local_addr (RtpSession * session, gchar * addr, gint port)
{
  return EOPNOTSUPP;
}
#else
gint
rtp_session_set_local_addr (RtpSession * session, gchar * addr, gint port)
{
  gint err;
  gint optval = 1;
#ifdef INET6
  char num[8];
  struct addrinfo hints, *res0, *res;

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  snprintf (num, sizeof (num), "%d", port);
  err = getaddrinfo (addr, num, &hints, &res0);
  if (err != 0)
    {
      g_warning ("Error: %s", gai_strerror (err));
      return err;
    }

  for (res = res0; res; res = res->ai_next)
    {
      session->rtp.socket = socket (res->ai_family, res->ai_socktype, 0);
      if (session->rtp.socket < 0)
	continue;

      /* set non blocking mode */
      set_non_blocking_socket (session);
      memcpy (&session->rtp.loc_addr, res->ai_addr, res->ai_addrlen);

      err = bind (session->rtp.socket, res->ai_addr, res->ai_addrlen);
      if (err != 0)
	{
	  g_warning ("Fail to bind rtp socket to port %i: %s.", port,
		     getSocketError ());
	  close_socket (session->rtp.socket);
	  freeaddrinfo (res0);
	  return -1;
	}
      else
	{
	  /* set the address reusable */

	  err = setsockopt (session->rtp.socket, SOL_SOCKET, SO_REUSEADDR,
			    (void *) &optval, sizeof (optval));
	  if (err < 0)
	    g_warning ("Fail to set rtp address reusable: %s.",
		       getSocketError ());

	  break;
	}

    }
  freeaddrinfo (res0);
  if (session->rtp.socket < 0)
    {
      if (session->mode == RTP_SESSION_RECVONLY)
	g_warning ("Could not create rtp socket with address %s: %s", addr,
		   getSocketError ());
      return -1;
    }

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  snprintf (num, sizeof (num), "%d", (port + 1));

  err = getaddrinfo (addr, num, &hints, &res0);
  for (res = res0; res; res = res->ai_next)
    {
      session->rtcp.socket = socket (res->ai_family, res->ai_socktype, 0);

      if (session->rtcp.socket < 0)
	continue;

      memcpy (&session->rtcp.loc_addr, res->ai_addr, res->ai_addrlen);
      err = bind (session->rtcp.socket, res->ai_addr, res->ai_addrlen);
      if (err != 0)
	{
	  g_warning ("Fail to bind rtp socket to port %i: %s.", port,
		     getSocketError ());
	  close_socket (session->rtp.socket);
	  close_socket (session->rtcp.socket);
	  return -1;
	}
      optval = 1;

      err =
	setsockopt (session->rtcp.socket, SOL_SOCKET, SO_REUSEADDR,
		    (void *) &optval, sizeof (optval));
      if (err < 0)
	g_warning ("Fail to set rtcp address reusable: %s.",
		   getSocketError ());


      break;
    }
  freeaddrinfo (res0);
  if (session->rtp.socket < 0)
    {
      g_warning ("Could not create rtcp socket with address %s: %s", addr,
		 getSocketError ());
      return -1;
    }
#ifdef RTCP
  ORTP_FD_ZERO (&session->scanfd);
  ORTP_FD_SET (session->rtp.socket, &session->scanfd);
  ORTP_FD_SET (session->rtcp.socket, &session->scanfd);
  if (session->rtcp.socket > session->rtp.socket)
    session->highest_fd = session->rtcp.socket + 1;
  else
    session->highest_fd = session->rtp.socket + 1;

#endif
  return 0;
#else // if IPv4:
  session->rtp.loc_addr.sin_family = AF_INET;

  err = inet_aton (addr, &session->rtp.loc_addr.sin_addr);

  if (err < 0)
    {
      g_warning ("Error in socket address:%s.", getSocketError ());
      return err;
    }
  session->rtp.loc_addr.sin_port = htons (port);


  if (!USING_TUNNEL (session))
    {
      session->rtp.socket = socket (PF_INET, SOCK_DGRAM, 0);
      g_return_val_if_fail (session->rtp.socket > 0, -1);

      /* set non blocking mode */
      set_non_blocking_socket (session);



      err = bind (session->rtp.socket,
		  (struct sockaddr *) &session->rtp.loc_addr,
		  sizeof (struct sockaddr_in));

      if (err != 0)
	{
	  g_warning ("Fail to bind rtp socket to port %i: %s.", port,
		     getSocketError ());
	  close_socket (session->rtp.socket);
	  return -1;
	}
      /* set the address reusable */

      err = setsockopt (session->rtp.socket, SOL_SOCKET, SO_REUSEADDR,
			(void *) &optval, sizeof (optval));
      if (err < 0)
	{
	  g_warning ("Fail to set rtp address reusable: %s.",
		     getSocketError ());
	}
    }

  /* RTCP socket */
  memcpy (&session->rtcp.loc_addr, &session->rtp.loc_addr,
	  sizeof (struct sockaddr_in));
  session->rtcp.loc_addr.sin_port = htons (port + 1);

  if (!USING_TUNNEL (session))
    {
      session->rtcp.socket = socket (PF_INET, SOCK_DGRAM, 0);
      g_return_val_if_fail (session->rtcp.socket > 0, -1);
      err = bind (session->rtcp.socket,
		  (struct sockaddr *) &session->rtcp.loc_addr,
		  sizeof (struct sockaddr_in));
      if (err != 0)
	{
	  g_warning ("Fail to bind rtcp socket to port %i: %s.", port + 1,
		     getSocketError ());
	  close_socket (session->rtp.socket);
	  close_socket (session->rtcp.socket);
	  return -1;
	}
      optval = 1;

      err = setsockopt (session->rtcp.socket, SOL_SOCKET, SO_REUSEADDR,
			(void *) &optval, sizeof (optval));
      if (err < 0)
	{
	  g_warning ("Fail to set rtcp address reusable: %s.",
		     getSocketError ());
	}

#ifdef RTCP
//<JT>
#if defined( WIN32 ) || defined (_WIN32_WCE)
      FD_ZERO (&session->scanfd);
      FD_SET  (session->rtp.socket, &session->scanfd);
      FD_SET  (session->rtcp.socket, &session->scanfd);
#else 
	  ORTP_FD_ZERO (&session->scanfd);
      ORTP_FD_SET (session->rtp.socket, &session->scanfd);
      ORTP_FD_SET (session->rtcp.socket, &session->scanfd);
#endif
//<JT/>
      if (session->rtcp.socket > session->rtp.socket)
	session->highest_fd = session->rtcp.socket + 1;
      else
	session->highest_fd = session->rtp.socket + 1;
#endif
	}
  return 0;
#endif
}
#endif


/**
 *rtp_session_set_remote_addr:
 *@session:		a rtp session freshly created.
 *@addr:		a local IP address in the xxx.xxx.xxx.xxx form.
 *@port:		a local port.
 *
 *	Sets the remote address of the rtp session, ie the destination address where rtp packet
 *	are sent. If the session is recv-only or duplex, it sets also the origin of incoming RTP 
 *	packets. Rtp packets that don't come from addr:port are discarded.
 *
 *	Returns: 0 on success.
**/
#ifdef TARGET_IS_HPUXKERNEL

gint
rtp_session_set_remote_addr (RtpSession * session, struct sockaddr_in * dest)
{
  mblk_t *mproto;
  struct T_unitdata_req *req;
  /* make a M_PROTO message to be linked with every outgoing rtp packet */
  mproto =
    allocb (sizeof (struct T_unitdata_req) + sizeof (struct sockaddr_in),
	    BPRI_MED);
  if (mproto == NULL)
    return -1;
  mproto->b_datap->db_type = M_PROTO;
  req = (struct T_unitdata_req *) mproto->b_wptr;
  req->PRIM_type = T_UNITDATA_REQ;
  req->DEST_length = sizeof (struct sockaddr_in);
  req->DEST_offset = sizeof (struct T_unitdata_req);
  req->OPT_length = 0;
  req->OPT_offset = 0;
  mproto->b_wptr += sizeof (struct T_unitdata_req);
  memcpy ((void *) mproto->b_wptr, (void *) dest,
	  sizeof (struct sockaddr_in));
  mproto->b_wptr += sizeof (struct sockaddr_in);
  rtp_session_lock (session);
  if (session->dest_mproto != NULL)
    {
      freemsg (session->dest_mproto);
    }
  session->dest_mproto = mproto;
  rtp_session_unlock (session);
  return 0;
}

#else
gint
rtp_session_set_remote_addr (RtpSession * session, gchar * addr, gint port)
{
  gint err;
  int should_connect;
  char remote_ip[20];
  unsigned short remote_port;
#ifdef INET6
  struct addrinfo hints, *res0, *res;
  char num[8];
#endif

  if (session->rtp.socket == 0)
    {
      int retry;
      /* the session has not its socket bound, do it */
      g_message ("Setting random local addresses.");
      for (retry = 0; retry < 10; retry++)
	{
	  int localport;
	  do
	    {
	      localport = (rand () + 5000) & 0xfffe;
	    }
	  while ((localport < 5000) || (localport > 0xffff));
#ifdef INET6
	  /* first try an ipv6 address, then an ipv4 */
	  err = rtp_session_set_local_addr (session, "::", localport);
	  if (err != 0)
	    err = rtp_session_set_local_addr (session, "0.0.0.0", localport);
#else
	  err = rtp_session_set_local_addr (session, "0.0.0.0", localport);
#endif

	  if (err == 0)
	    break;
	}
      if (retry == 10)
	{
	  g_warning
	    ("rtp_session_set_remote_addr: Could not find a random local address for socket !");
	  return -1;
	}
    }

#ifdef INET6
  memset (&hints, 0, sizeof (hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  snprintf (num, sizeof (num), "%d", port);
  err = getaddrinfo (addr, num, &hints, &res0);
  if (err)
    {
      g_warning ("Error in socket address: %s", gai_strerror (err));
      return err;
    }

  for (res = res0; res; res = res->ai_next)
    {
      /*err = connect (session->rtp.socket, res->ai_addr, res->ai_addrlen);
       */
      /*don't connect: after you are no more able to use recvfrom() and sendto() */
      err = 0;
      if (err == 0)
	{
	  memcpy (&session->rtp.rem_addr, res->ai_addr, res->ai_addrlen);
	  break;
	}
    }
  freeaddrinfo (res0);
  if (err != 0)
    {
      g_message ("Can't connect rtp socket: %s.", getSocketError ());
      return err;
    }

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  snprintf (num, sizeof (num), "%d", (port + 1));
  err = getaddrinfo (addr, num, &hints, &res0);
  if (err)
    {
      g_warning ("Error: %s", gai_strerror (err));
      return err;
    }
  for (res = res0; res; res = res->ai_next)
    {
      /*err = connect (session->rtcp.socket, res->ai_addr, res->ai_addrlen); */
      /*don't connect: after you are no more able to use recvfrom() and sendto() */
      err = 0;
      if (err == 0)
	{
	  memcpy (&session->rtcp.rem_addr, res->ai_addr, res->ai_addrlen);
	  break;
	}
    }
  freeaddrinfo (res0);
#else
  session->rtp.rem_addr.sin_family = AF_INET;

  err = inet_aton (addr, &session->rtp.rem_addr.sin_addr);
  if (err < 0)
    {
      g_warning ("Error in socket address:%s.", getSocketError ());
      return err;
    }
  session->rtp.rem_addr.sin_port = htons (port);

  memcpy (&session->rtcp.rem_addr, &session->rtp.rem_addr,
	  sizeof (struct sockaddr_in));
  session->rtcp.rem_addr.sin_port = htons (port + 1);
#endif
#ifndef NOCONNECT

  if (!USING_TUNNEL (session) && (session->mode == RTP_SESSION_SENDONLY))
    {
      err = connect (session->rtp.socket,
		     (struct sockaddr *) &session->rtp.rem_addr,
		     NETADDR_SIZE (session->rtp.rem_addr));

      if (err != 0)
	{
	  g_message ("Can't connect rtp socket: %s.", getSocketError ());
	  return err;
	}
      err = connect (session->rtcp.socket,
		     (struct sockaddr *) &session->rtcp.rem_addr,
		     NETADDR_SIZE (session->rtcp.rem_addr));

      if (err != 0)
	{
	  g_message ("Can't connect rtp socket: %s.", getSocketError ());
	  return err;
	}
    }

#endif // NOCONNECT

  return 0;
}
#endif


void
rtp_session_set_tunnels (RtpSession * session, RtpTunnel * tunrtp,
			 RtpTunnel * tunrtcp)
{
  session->rtp.tunnel = tunrtp;
  session->rtcp.tunnel = tunrtcp;
  rtp_session_set_sockets (session, TUNNEL_SOCKET (tunrtp),
			   tunrtcp ? TUNNEL_SOCKET (tunrtcp) : 0);
  session->flags |= RTP_SESSION_USING_TUNNELS;
  set_non_blocking_socket (session);
}


void
rtp_session_set_sockets (RtpSession * session, gint rtpfd, gint rtcpfd)
{
  session->rtp.socket = rtpfd;
  session->rtcp.socket = rtcpfd;
  session->flags |= RTP_SESSION_USING_EXT_SOCKETS;
#ifdef RTCP
//<JT>
#if defined (WIN32) ||defined(_WIN32_WCE)
  FD_ZERO (&session->scanfd);
  FD_SET (session->rtp.socket, &session->scanfd);
  FD_SET (session->rtcp.socket, &session->scanfd);
#else
   ORTP_FD_ZERO (&session->scanfd);
   ORTP_FD_SET (session->rtp.socket, &session->scanfd);
   ORTP_FD_SET (session->rtcp.socket, &session->scanfd);
#endif
  if (session->rtcp.socket > session->rtp.socket)
    session->highest_fd = session->rtcp.socket + 1;
  else
    session->highest_fd = session->rtp.socket + 1;

#endif
}

/**
 *rtp_session_set_seq_number
 *@session:		a rtp session freshly created.
 *@addr:			a 16 bit unsigned number.
 *
 * sets the initial sequence number of a sending session.
 *
**/
void
rtp_session_set_seq_number (RtpSession * session, guint16 seq)
{
  session->rtp.snd_seq = seq;
}


guint16
rtp_session_get_seq_number (RtpSession * session)
{
  return session->rtp.snd_seq;
}

#ifdef TARGET_IS_HPUXKERNEL
#ifdef WORDS_BIGENDIAN

#if 0
#define rtp_send(_session,_m) \
	do{\
		mblk_t *_destmp;\
		if ((_session)->dest_mproto!=NULL){\
			_destmp=dupb((_session)->dest_mproto);\
			_destmp->b_cont=(_m);\
			streams_put(putnext,(_session)->rtp.wq,(_destmp),(void*)(_session)->rtp.wq);\
		} else {\
			g_warning("rtp_send: ERROR - there is no destination addreess !");\
			freemsg(_m);\
		}\
	}while (0);

#endif

#define rtp_send(_session,_m) \
	do{\
		mblk_t *_destmp;\
		if ((_session)->dest_mproto!=NULL){\
			_destmp=dupb((_session)->dest_mproto);\
			_destmp->b_cont=(_m);\
			streams_put(putnext,(_session)->rtp.wq,(_destmp),(void*)(_session)->rtp.wq);\
		} else {\
			streams_put(putnext,(_session)->rtp.wq,(_m),(void*)(_session)->rtp.wq);\
		}\
	}while (0);

#endif
#else
static gint
rtp_send (RtpSession * session, mblk_t * m)
{
  gint error;
  int i;
  rtp_header_t *hdr;

  if (m->b_cont != NULL)
    {
      mblk_t *newm;
      newm = msgpullup (m, -1);
      freemsg (m);
      m = newm;
    }
  hdr = (rtp_header_t *) m->b_rptr;
  hdr->ssrc = htonl (hdr->ssrc);
  hdr->timestamp = htonl (hdr->timestamp);
  hdr->seq_number = htons (hdr->seq_number);


  for (i = 0; i < hdr->cc; i++)
    hdr->csrc[i] = htonl (hdr->csrc[i]);

  /* Callback for pre-send action */
  if (session->rtp_pre_send_cb != NULL)
      session->rtp_pre_send_cb(session, &error, m);

  if (USING_TUNNEL (session))
    {
      error =
	TUNNEL_SEND (session->rtp.tunnel, m->b_rptr, (m->b_wptr - m->b_rptr));
      if (error < 0)
	{
	  session->terminated = 1;
	  freemsg (m);
	  return -1;
	}
    }
  else if (session->flags & RTP_SESSION_USING_EXT_SOCKETS)
    error = send (session->rtp.socket, m->b_rptr, (m->b_wptr - m->b_rptr), 0);
  else
    error = sendto (session->rtp.socket, m->b_rptr,
		    (m->b_wptr - m->b_rptr), 0,
		    (struct sockaddr *) &session->rtp.rem_addr,
		    sizeof (session->rtp.rem_addr));

  /* Callback for post-send action */
  if (session->rtp_post_send_cb != NULL)
    session->rtp_post_send_cb(session, &error, m);

  if (error < 0)
    g_warning ("Error sending rtp packet: %s.", getSocketError ());
  freemsg (m);
  return error;
}

#endif

/**
 *rtp_session_set_jitter_compensation:
 *@session: a RtpSession
 *@milisec: the time interval in milisec to be jitter compensed.
 *
 * Sets the time interval for which packet are buffered instead of being delivered to the 
 * application.
 **/
void
rtp_session_set_jitter_compensation (RtpSession * session, gint milisec)
{
  PayloadType *payload = rtp_profile_get_payload (session->rcv.profile,
						  session->rcv.payload_type);
  if (payload == NULL)
    {
      g_warning
	("rtp_session_set_jitter_compensation: cannot set because the payload type is unknown");
      return;
    }
  /* REVISIT: need to take in account the payload description */
  session->rtp.jitt_comp = milisec;
  /* convert in timestamp unit: */
  session->rtp.jitt_comp_time =
    (gint) (((double) milisec / 1000.0) * (payload->clock_rate));
  printf ("Jitter in ts units %d:\n", session->rtp.jitt_comp_time);
}


/**
 *rtp_session_set_ssrc:
 *@session: a rtp session.
 *@ssrc: an unsigned 32bit integer representing the synchronisation source identifier (SSRC).
 *
 *	Sets the SSRC of the session.
 *
**/
void
rtp_session_set_ssrc (RtpSession * session, guint32 ssrc)
{
  session->local.ssrc = ssrc;
}

/**
 *rtp_session_set_payload_type:
 *@session: a rtp session
 *@paytype: the payload type
 *
 *	Sets the payload type of the rtp session. It decides of the payload types written in the
 *	of the rtp header for the outgoing stream, if the session is SENDRECV or SENDONLY.
 *	For the incoming stream, it sets the waited payload type. If that value does not match
 *	at any time this waited value, then the application can be informed by registering
 *	for the "payload_type_changed" signal, so that it can make the necessary changes
 *	on the downstream decoder that deals with the payload of the packets.
 *
 *Returns: 0 on success, -1 if the payload is not defined.
**/

int
rtp_session_set_payload_types (RtpSession * session, int spaytype, int rpaytype)
{
  session->snd.payload_type = spaytype;
  session->rcv.payload_type = rpaytype;
  return 0;
}

int
_rtp_session_get_payload_type (RtpSession * session)
{
  return session->rcv.payload_type;
}

int
rtp_session_get_recv_payload_type (RtpSession * session)
{
  return session->rcv.payload_type;
}

int
rtp_session_get_send_payload_type (RtpSession * session)
{
  return session->snd.payload_type;
}




/**
 *rtp_session_set_payload_type_with_string:
 *@session: a rtp session
 *@paytype: the payload type
 *
 *	Sets the payload type of the rtp session. It decides of the payload types written in the
 *	of the rtp header for the outgoing stream, if the session is SENDRECV or SENDONLY.
 * 	Unlike #rtp_session_set_payload_type(), it takes as argument a string referencing the
 *	payload type (mime type).
 *	For the incoming stream, it sets the waited payload type. If that value does not match
 *	at any time this waited value, then the application can be informed by registering
 *	for the "payload_type_changed" signal, so that it can make the necessary changes
 *	on the downstream decoder that deals with the payload of the packets.
 *
 *Returns: 0 on success, -1 if the payload is not defined.
**/

int
rtp_session_set_payload_types_with_string (RtpSession * session,
					  const char *mime)
{
  int pt;
  pt = rtp_profile_get_payload_number_from_mime (session->rcv.profile, mime);
  if (pt < 0)
    {
      g_warning
	("%s is not a know mime string within the rtpsession's recv profile.",
	 mime);
      return -1;
    }
  session->rcv.payload_type = pt;
  if (session->rcv.profile == session->snd.profile)
    {
        session->snd.payload_type = pt;
	return 0;
    }

  pt = rtp_profile_get_payload_number_from_mime (session->snd.profile, mime);
  if (pt < 0)
    {
      g_warning
	("%s is not a know mime string within the rtpsession's send profile.",
	 mime);
      return -1;
    }
  session->snd.payload_type = pt;


  return 0;
}


/**
 *rtp_session_create_packet:
 *@session:		a rtp session.
 *@header_size:	the rtp header size. For standart size (without extensions), it is #RTP_FIXED_HEADER_SIZE
 *@payload		:data to be copied into the rtp packet.
 *@payload_size	: size of data carried by the rtp packet.
 *
 *	Allocates a new rtp packet. In the header, ssrc and payload_type according to the session's
 *	context. Timestamp and seq number are not set, there will be set when the packet is going to be
 *	sent with rtp_session_sendm_with_ts().
 *
 *Returns: a rtp packet in a mblk_t (message block) structure.
**/
mblk_t *
rtp_session_create_packet (RtpSession * session, gint header_size,
			   char *payload, gint payload_size)
{
  mblk_t *mp;
  gint msglen = header_size + payload_size;
  rtp_header_t *rtp;

#ifdef MBDB_LOCAL_POOL
  int i;

  for (i = 0; i < RTP_SESSION_SEND_POOL_SIZE; i++)
    {
      mp = session->sendbuffer[i];
      if (mp->b_datap->ref_count == 1)
	{
	  mp = dupb (mp);
	  break;
	}
    }

  if (i >= RTP_SESSION_SEND_POOL_SIZE)
#endif
    mp = allocb (msglen, BPRI_MED);
#ifdef _KERNEL
  if (mp == NULL)
    return NULL;
#endif
	rtp=(rtp_header_t*)mp->b_rptr;
	rtp->version = 2;
	rtp->padbit = 0;
	rtp->extbit = 0;
	rtp->markbit= session->markbit;
	rtp->cc = 0;
	//rtp_session_lock(session);
	rtp->paytype = session->snd.payload_type;
	rtp->ssrc = session->local.ssrc;
	//rtp_session_unlock(session);
	rtp->timestamp = 0;	/* set later, when packet is sended */
	rtp->seq_number = 0; /*set later, when packet is sended */
	/*copy the payload */
	mp->b_wptr+=header_size;
	memcpy(mp->b_wptr, payload, payload_size);
	mp->b_wptr+=payload_size;
	return mp;
}
mblk_t	*rtp_session_create_specific_payload_packet(RtpSession *session, gint header_size,  gint payload_type, char *payload, gint payload_size )
{
	mblk_t *mp;
	rtp_header_t *rtp;
	
#ifdef MBDB_LOCAL_POOL
	int i;

	for(i = 0; i < RTP_SESSION_SEND_POOL_SIZE; i++)
	{
		mp = session->sendbuffer[i];
		if (mp->b_datap->ref_count == 1)
		{
			mp = dupb(mp);
			break;
		}
	}

	if (i >= RTP_SESSION_SEND_POOL_SIZE)
#endif
	mp=allocb(header_size+payload_size, BPRI_MED);
	if (mp==NULL) return NULL;
	rtp=(rtp_header_t*)mp->b_rptr;
	rtp->version = 2;
	rtp->markbit=session->markbit;
	rtp->padbit = 0;
	rtp->extbit = 0;
	rtp->cc = 0;
	rtp->ssrc = session->local.ssrc;
	/* timestamp set later, when packet is sended */
	/*seq number set later, when packet is sended */
	
	/*set the payload type */
	rtp->paytype=payload_type;
	
	/*copy the payload */
	mp->b_wptr+=header_size;
	memcpy(mp->b_wptr, payload, payload_size);
	mp->b_wptr+=payload_size;
	return mp;
}

/**
 *rtp_session_sendm_with_ts:
 *@session	: a rtp session.
 *@mp		:	a rtp packet presented as a mblk_t.
 *@timestamp:	the timestamp of the data to be sent. Refer to the rfc to know what it is.
 *
 *	Send the rtp datagram @mp to the destination set by rtp_session_set_remote_addr() 
 *	with timestamp @timestamp. For audio data, the timestamp is the number
 *	of the first sample resulting of the data transmitted. See rfc1889 for details.
 *
 *Returns: the number of bytes sent over the network.
**/
gint
rtp_session_sendm_with_ts (RtpSession * session, mblk_t * mp,
			   guint32 timestamp)
{
  rtp_header_t *rtp;
  guint32 packet_time;
  gint error = 0;
  gint msgsize;
  guint32 sched_time;
#ifdef BUILD_SCHEDULER
  RtpScheduler *sched;
#endif

  if (session->flags & RTP_SESSION_SEND_NOT_STARTED)
    {
      session->rtp.snd_ts_offset = timestamp;
#ifdef BUILD_SCHEDULER
      if (session->flags & RTP_SESSION_SCHEDULED)
	{
	  sched = ortp_get_scheduler ();

	  session->rtp.snd_time_offset = sched->time_;
	  //g_message("setting snd_time_offset=%i",session->rtp.snd_time_offset);
	}
#endif
#ifdef RTCP
      rtcp_send_report (session);
#endif
      rtp_session_unset_flag (session, RTP_SESSION_SEND_NOT_STARTED);
    }

  rtp = (rtp_header_t *) mp->b_rptr;

  msgsize = msgdsize (mp);
  rtp_session_lock (session);

  /* set a seq number */
  rtp->seq_number = session->rtp.snd_seq;
  rtp->timestamp = timestamp;
  session->rtp.snd_seq++;
  session->rtp.snd_last_ts = timestamp;


  ortp_global_stats.sent += msgsize;
  session->stats.sent += msgsize;
  ortp_global_stats.packet_sent++;
  session->stats.packet_sent++;

#ifdef TARGET_IS_HPUXKERNEL
  /* send directly the message through the stream */
  rtp_send (session, mp);

#else
  if (!(session->flags & RTP_SESSION_SCHEDULED))
    {
      error = rtp_send (session, mp);
    }
  else
    {
      putq (session->rtp.wq, mp);
    }
#endif

#ifdef RTCP
  {
    struct timeval now;
    gettimeofday (&now, NULL);
    tvsub (&now, &session->local.lastsrtime);
    if (now.tv_sec >= 5)
      rtcp_send_report (session);
  }
#endif
  rtp_session_unlock (session);
  /* if we are in blocking mode, then suspend the process until the scheduler sends the
   * packet */
  /* if the timestamp of the packet queued is older than current time, then you we must
   * not block */
#ifdef BUILD_SCHEDULER
  if (session->flags & RTP_SESSION_SCHEDULED)
    {
      sched = ortp_get_scheduler ();
      packet_time =
	rtp_session_ts_to_t (session,
			     timestamp -
			     session->rtp.snd_ts_offset) +
	session->rtp.snd_time_offset;
      //g_message("rtp_session_send_with_ts: packet_time=%i time=%i",packet_time,sched->time_);
      sched_time = sched->time_;
      if (TIME_IS_STRICTLY_NEWER_THAN (packet_time, sched_time))
	{
	  if (session->flags & RTP_SESSION_BLOCKING_MODE)
	    {
	      //g_message("waiting packet to be sent");
	      g_mutex_lock (session->rtp.wait_for_packet_to_be_sent_mutex);
	      g_cond_wait (session->rtp.
			   wait_for_packet_to_be_sent_cond,
			   session->rtp.wait_for_packet_to_be_sent_mutex);
	      g_mutex_unlock (session->rtp.wait_for_packet_to_be_sent_mutex);
	    }
	  session_set_clr (&sched->w_sessions, session);	/* the session has written */

	}
      else
	session_set_set (&sched->w_sessions, session);	/*to indicate select to return immediately */
    }
#endif
  return error;
}



/**
 *rtp_session_send_with_ts:
 *@session: a rtp session.
 *@buffer:	a buffer containing the data to be sent in a rtp packet.
 *@len:		the length of the data buffer, in bytes.
 *@userts:	the timestamp of the data to be sent. Refer to the rfc to know what it is.
 *
 *	Send a rtp datagram to the destination set by rtp_session_set_remote_addr() containing
 *	the data from @buffer with timestamp @userts. This is a high level function that uses
 *	rtp_session_create_packet() and rtp_session_sendm_with_ts() to send the data.
 *
 *
 *Returns: the number of bytes sent over the network.
**/
gint
rtp_session_send_with_ts (RtpSession * session, gchar * buffer, gint len,
			  guint32 userts)
{
  mblk_t *m;
  gint msgsize;

  /* allocate a mblk_t, set the haeder. Perhaps if len>MTU, we should allocate a new
   * mlkt_t to split the packet FIXME */
  msgsize = len + RTP_FIXED_HEADER_SIZE;
  m = rtp_session_create_packet (session, RTP_FIXED_HEADER_SIZE, buffer, len);
  if (m == NULL)
    {
      g_warning ("Could not allocate message block for sending packet.");
      return -1;
    }

  return rtp_session_sendm_with_ts (session, m, userts);
}

#ifdef MBDB_LOCAL_POOL
mblk_t *
rtp_session_alloc_recv_buffer (RtpSession * session)
{
  int i;
  mblk_t *mp;


  for (i = 0; i < RTP_SESSION_RECV_POOL_SIZE; i++)
    {
      mp = session->recvbuffer[i];
      if (mp->b_datap->ref_count == 1)
	return dupb (mp);
    }

  return allocb (session->max_buf_size, 0);
}
#endif


#ifdef TARGET_IS_HPUXKERNEL
static gint
rtp_recv (RtpSession * session)
{
  return EOPNOTSUPP;
}
#else
static gint
rtp_recv (RtpSession * session)
{
  int count = 2;
  gint error;
  struct sockaddr_in remaddr;
  int addrlen = sizeof (struct sockaddr_in);
  char *p;
  mblk_t *mp = 0;
  fd_set fdset;
  struct timeval timeout = { 0, 0 };

  if (session->rtp.socket < 1)
    return -1;			/*session has no sockets for the moment */
  if (!session)
    printf ("Session null");
#ifndef RTCP
  FD_ZERO (&fdset);
  FD_SET (session->rtp.socket, &fdset);

  error = 1;
  while (error > 0 && count--)
    {
      timeout.tv_sec = timeout.tv_usec = 0;
      error = select (session->rtp.socket + 1, &fdset, NULL, NULL, &timeout);

      if ((error == 1) && (FD_ISSET (session->rtp.socket, &fdset)))	/* something to read */
	{
#ifdef MBDB_LOCAL_POOL
	  mp = rtp_session_alloc_recv_buffer (session);
#else
	  mp = allocb (session->max_buf_size, 0);
#endif
	  /* Callback for pre-recv action */
	  if (session->rtp_pre_recv_cb != NULL)
	    session->rtp_pre_recv_cb(session, &error, mp);

	  //<MINHPQ>

	  if (USING_TUNNEL (session))
	    {
	      error =
		TUNNEL_RECV (session->rtp.tunnel, mp->b_wptr,
			     session->max_buf_size);
	      if (error < 0)
		{
		  session->terminated = 1;
		}
	    }
	  else
	    {
	      if (session->flags & RTP_SESSION_USING_EXT_SOCKETS)
		{
		  error =
		    recv (session->rtp.socket, mp->b_wptr,
			  session->max_buf_size, 0);
		}
	      else
		error = recvfrom (session->rtp.socket, mp->b_wptr,
				  session->max_buf_size, 0,
				  (struct sockaddr *) &remaddr, &addrlen);
	    }


	  if (error > 0)
	    {
#ifdef MBDB_LOCAL_POOL
	      mp->b_wptr += error;
#else
	      /* resize the memory allocated to fit the udp message */

	      p = g_realloc (mp->b_wptr, error);
	      if (p != mp->b_wptr)
		ortp_debug ("The recv area has moved during reallocation.");
	      mp->b_datap->db_base = mp->b_rptr = mp->b_wptr = p;
	      mp->b_wptr += error;
	      mp->b_datap->db_lim = mp->b_wptr;
#endif
	      /* Callback for post-recv action */
	      if (session->rtp_post_recv_cb != NULL)
		session->rtp_post_recv_cb(session, &error, mp);

	      /* then put the new message on queue */
	      rtp_parse (session, mp);
	    }
	  else
	    {
	      if (error == 0)
		{
		  g_warning
		    ("rtp_stack_recv: strange... recv() returned zero.");
		}

	      freemsg (mp);
	      return -1;	/* avoids an infinite loop ! */
	    }
	}
      else
	return 0;
    }
  return error;
#else /* RTCP */
//<JT>
#if defined (WIN32) || defined(_WIN32_WCE)
  FD_ZERO (&fdset);
  FD_SET (session->rtp.socket, &fdset);
  FD_SET (session->rtcp.socket, &fdset);
  if (session->rtcp.socket > session->rtp.socket)
  session->highest_fd = session->rtcp.socket + 1;
      else
  session->highest_fd = session->rtp.socket + 1;
//<JT/>
  #else
  memcpy (&fdset, &session->scanfd, sizeof (&fdset));
  #endif
  error = 1;
  while (error > 0)
    {
      timeout.tv_sec = timeout.tv_usec = 0;
      error = select (session->highest_fd + 1, &fdset, NULL, NULL, &timeout);
      if ((error >= 1) && (FD_ISSET (session->rtp.socket, &fdset)))
	{			/* something to read */
	  int err2;
#ifdef MBDB_LOCAL_POOL
	  mp = rtp_session_alloc_recv_buffer (session);
#else
	  mp = allocb (session->max_buf_size, 0);
#endif
	  if (mp != NULL)
	    {
	      /* Callback for pre-recv action */
	      if (session->rtp_pre_recv_cb != NULL)
		session->rtp_pre_recv_cb(session, &error, mp);

	      if (USING_TUNNEL (session))
		{
		  err2 =
		    TUNNEL_RECV (session->rtp.tunnel, mp->b_wptr,
				 session->max_buf_size);
		  if (err2 < 0)
		    session->terminated = 1;
		}
	      else
		err2 =
		  recvfrom (session->rtp.socket, mp->b_wptr,
			    session->max_buf_size, 0,
			    (struct sockaddr *) &remaddr, &addrlen);
		}
	  else
	    {
	      g_warning ("rtp_stack_recv: allocating mblk failed!");
	      return -1;
	    }

	  if (session->flags & RTP_SESSION_RECV_NOT_STARTED)
	    {
	      freemsg (mp);
	      goto read_rtcp;
	    }

	  if (err2 > 0)
	    {

#ifdef MBDB_LOCAL_POOL
	      mp->b_wptr += err2;
#else
	      /* resize the memory allocated to fit the udp message */

	      p = g_realloc (mp->b_wptr, err2);
	      if (p != mp->b_wptr)
		ortp_debug ("The recv area has moved during reallocation.");
	      mp->b_datap->db_base = mp->b_rptr = mp->b_wptr = p;
	      mp->b_wptr += err2;
	      mp->b_datap->db_lim = mp->b_wptr;
#endif
	      /* Callback for post-recv action */
	      if (session->rtp_post_recv_cb != NULL)
		session->rtp_post_recv_cb(session, &error, mp);

	      /* then put the new message on queue */
	      gettimeofday (&mp->bts, NULL);

	      rtp_parse (session, mp);
	    }
	  else			/* recvfrom failed */
	    {
	      if (err2 == 0)
		g_warning
		  ("rtp_stack_recv: strange... recv() returned zero.");
	      else if (errno != EWOULDBLOCK)
		g_warning ("Error receiving udp packet: %s.",
			   getSocketError ());
	      freemsg (mp);
	      return -1;	/* avoids an infinite loop ! */
	    }
	}
    read_rtcp:
      if (error >= 1 && (FD_ISSET (session->rtcp.socket, &fdset)))
	{			/* something to read */
	  int err2;
#ifdef MBDB_LOCAL_POOL
	  mp = rtp_session_alloc_recv_buffer (session);
#else
	  mp = allocb (session->max_buf_size, 0);
#endif

	  if (mp != NULL)
	    {
	      /* Callback for pre-recv action */
	      if (session->rtp_pre_recv_cb != NULL)
		session->rtp_pre_recv_cb(session, &error, mp);

	      if (USING_TUNNEL (session))
		{
		  err2 =
		    TUNNEL_RECV (session->rtcp.tunnel, mp->b_wptr,
				 session->max_buf_size);
		  if (err2 < 0)
		    session->terminated = 1;

		}
	      else
		err2 =
		  recvfrom (session->rtcp.socket, mp->b_wptr,
			    session->max_buf_size, 0,
			    (struct sockaddr *) &remaddr, &addrlen);
	    }
	  else
	    {
	      g_warning ("rtp_stack_recv: allocating mblk failed!");
	      return -1;
	    }

	  if (err2 > 0)
	    {
#ifdef MBDB_LOCAL_POOL
	      mp->b_wptr += err2;
#else
	      /* resize the memory allocated to fit the udp message */

	      p = g_realloc (mp->b_wptr, err2);
	      if (p != mp->b_wptr)
		ortp_debug ("The recv area has moved during reallocation.");
	      mp->b_datap->db_base = mp->b_rptr = mp->b_wptr = p;
	      mp->b_wptr += err2;
	      mp->b_datap->db_lim = mp->b_wptr;
	      /* then put the new message on queue */
#endif
	      /* Callback for post-recv action */
	      if (session->rtp_post_recv_cb != NULL)
		session->rtp_post_recv_cb(session, &error, mp);

	      rtcp_parse (session, mp->b_rptr, mp->b_wptr - mp->b_rptr);
	      freemsg (mp);
	    }
	  else
	    {
	      if (err2 == 0)
		g_warning
		  ("rtp_stack_recv: strange... recv() returned zero.");
	      else if (errno != EWOULDBLOCK)
		g_warning ("Error receiving udp packet: %s.",
			   getSocketError ());
	      freemsg (mp);
	      return -1;
	    }
	}
      {
	struct timeval now;

	gettimeofday (&now, NULL);
	tvsub (&now, &session->local.lastsrtime);
	if (now.tv_sec >= 5)
	  rtcp_send_report (session);
      }
    }
  return error;
#endif
}
#endif

#ifdef SYNC_OPT
int
rtp_session_resync (RtpSession * session, guint32 user_ts)
{
#if 0
  if (!(session->flags & RTP_SESSION_RECV_SYNC))
    {
      rtp_header_t *oldest, *newest;
      queue_t *q = session->rtp.rq;
      if (q->q_last == NULL)
	return 0;

      oldest = (rtp_header_t *) q->q_first->b_rptr;
      user_ts += session->rtp.rcv_diff_ts;
      if (RTP_TIMESTAMP_IS_NEWER_THAN (oldest->timestamp, user_ts))
	{
	  rtp_session_set_flag (session, RTP_SESSION_RECV_SYNC);
	  return 1;
	}
    }

  return 0;
#else
  rtp_session_set_flag (session, RTP_SESSION_RECV_SYNC);
  return session->rtp.rq->q_last != NULL;
#endif


}
#endif
/**
 *rtp_session_recvm_with_ts:
 *@session: a rtp session.
 *@user_ts:	a timestamp.
 *
 *	Try to get a rtp packet presented as a mblk_t structure from the rtp session.
 *	The @user_ts parameter is relative to the first timestamp of the incoming stream. In other
 *	words, the application does not have to know the first timestamp of the stream, it can
 *	simply call for the first time this function with @user_ts=0, and then incrementing it
 *	as it want. The RtpSession takes care of synchronisation between the stream timestamp
 *	and the user timestamp given here.
 *
 *Returns: a rtp packet presented as a mblk_t.
**/

mblk_t *
rtp_session_recvm_with_ts (RtpSession * session, guint32 user_ts)
{
  mblk_t *mp = NULL;
  rtp_header_t *rtp;
  guint32 ts;
  guint32 packet_time;
  guint32 sched_time;
  int noresync = 0;
#ifdef BUILD_SCHEDULER
  RtpScheduler *sched;
#endif
  /* if we are scheduled, remember the scheduler time at which the application has
   * asked for its first timestamp */

  if (session->flags & RTP_SESSION_RECV_NOT_STARTED)
    {

      session->rtp.rcv_query_ts_offset = user_ts;
#ifdef BUILD_SCHEDULER
      if (session->flags & RTP_SESSION_SCHEDULED)
	{
	  sched = ortp_get_scheduler ();
	  session->rtp.rcv_time_offset = sched->time_;
	  //g_message("setting snd_time_offset=%i",session->rtp.snd_time_offset);
	}
#endif
      rtp_session_unset_flag (session, RTP_SESSION_RECV_NOT_STARTED);
    }
  session->rtp.rcv_last_app_ts = user_ts;
#ifdef TARGET_IS_HPUXKERNEL
  /* nothing to do: packets are enqueued on interrupt ! */
#else
  if (!(session->flags & RTP_SESSION_SCHEDULED))	/* if not scheduled */
    {
      rtp_recv (session);
    }
#endif
  /* then now try to return a packet, if possible */
  /* first condition: if the session is starting, don't return anything
   * until the queue size reaches jitt_comp */
  /* first lock the session */
  rtp_session_lock (session);
  if (session->flags & RTP_SESSION_RECV_SYNC)
    {
      rtp_header_t *oldest, *newest;
      queue_t *q = session->rtp.rq;
      if (q && q->q_last == NULL)
	{
	  ortp_debug ("Queue is empty.");
	  goto end;
	}
      oldest = (rtp_header_t *) q->q_first->b_rptr;
      newest = (rtp_header_t *) q->q_last->b_rptr;
      if (session->rtp.jitt_comp_time != 0 && (guint32) (newest->timestamp - oldest->timestamp) <
	  session->rtp.jitt_comp_time)
	{
	  ortp_debug ("Not enough packet bufferised.");
	  noresync = 1;
	  goto end;
	}
    resync:
      /* if enough packet queued continue but delete the starting flag */
      rtp_session_unset_flag (session, RTP_SESSION_RECV_SYNC);

      mp = getq (session->rtp.rq);
      rtp = (rtp_header_t *) mp->b_rptr;
      session->rtp.rcv_ts_offset = rtp->timestamp;
      session->rtp.rcv_app_ts_offset = user_ts;
      /* and also remember the timestamp offset between the stream timestamp (random)
       * and the user timestamp, that very often starts at zero */
      session->rtp.rcv_diff_ts = rtp->timestamp - user_ts;
      session->rtp.rcv_last_ret_ts = user_ts;	/* just to have an init value */
      session->peer.ssrc = rtp->ssrc;
#ifdef SYNC_OPT
      ts = rtp->timestamp;
#endif
      ortp_debug ("Returning FIRST mp with ts=%i", rtp->timestamp);

      goto end;
    }
  /* else this the normal case */
  /*calculate the stream timestamp from the user timestamp */
  ts = user_ts + session->rtp.rcv_diff_ts;
#ifndef SYNC_OPT
  session->rtp.rcv_last_ts = ts;
#endif
  mp = rtp_getq (session->rtp.rq, ts);

  /* perhaps we can now make some checks to see if a resynchronization is needed */
  /* TODO */
  goto end;

end:
  if (mp != NULL)
    {
      int msgsize = msgdsize (mp);	/* evaluate how much bytes (including header) is received by app */
      check_timings (mp);

      ortp_global_stats.recv += msgsize;
      session->stats.recv += msgsize;
      rtp = (rtp_header_t *) mp->b_rptr;
#ifdef SYNC_OPT
      ts = rtp->timestamp;
#endif
      ortp_debug ("Returning mp with ts=%i", rtp->timestamp);
      /* check for payload type changes */
      if (session->rcv.payload_type != rtp->paytype)
	{
	  /* this is perhaps a telephony event */
	  if (rtp->paytype == session->rcv.telephone_events_pt)
	    {
	      rtp_signal_table_emit2 (&session->on_telephone_event_packet,
				      (gpointer) mp);
	      if (session->on_telephone_event.count > 0)
		{
		  if (mp == NULL)
		    {
		      g_warning ("mp is null!");
		    }
		  else
		    rtp_session_check_telephone_events (session, mp);
		}
				/************ warning**********/
	      /* we free the telephony event packet and the function will return NULL */
	      /* is this good ? */
	      freemsg (mp);
	      mp = NULL;
	    }
	  else
	    {
	      /* check if we support this payload type */
	      PayloadType *pt =
		rtp_profile_get_payload (session->rcv.profile, rtp->paytype);
	      if (pt != 0)
		{
		  g_message ("rtp_parse: payload type changed to %i !",
			     rtp->paytype);
		  session->rcv.payload_type = rtp->paytype;
		  rtp_signal_table_emit (&session->on_payload_type_changed);
		}
	      else
		{
		  g_warning ("Receiving packet with unknown payload type %i.",
			     rtp->paytype);
		}
	    }
	}

#ifdef SYNC_OPT
      session->rtp.rcv_last_ts = ts;
      session->rtp.rcv_last_ret_ts = user_ts;
      session->rtp.rcv_diff_ts = ts - user_ts;
#endif
    }
  else
    {
      rtp_header_t *oldest, *newest;
      queue_t *q = session->rtp.rq;

#ifdef SYNC_OPT

	if (!noresync && rtp_session_resync (session, user_ts)) {
		goto resync;
	}


      if (q->q_last != NULL)
	{

	  oldest = (rtp_header_t *) q->q_first->b_rptr;
	  newest = (rtp_header_t *) q->q_last->b_rptr;
	  ortp_debug
	    ("No mp for timestamp queried %i(%i) oldest=%i newest=%i discarded=%i\n",
	     user_ts, user_ts + session->rtp.rcv_diff_ts, oldest->timestamp,
	     newest->timestamp, ortp_global_stats.discarded);
	}
      else
	ortp_debug ("No mp for timestamp queried %i(%i)\n", user_ts,
		   user_ts + session->rtp.rcv_diff_ts);
      session->stats.unavaillable++;
      ortp_global_stats.unavaillable++;
#else
      ortp_debug ("No mp for timestamp queried");
      session->stats.unavaillable++;
      ortp_global_stats.unavaillable++;

#endif
    }
  rtp_session_unlock (session);
#ifdef BUILD_SCHEDULER
  if (session->flags & RTP_SESSION_SCHEDULED)
    {
      /* if we are in blocking mode, then suspend the calling process until timestamp
       * wanted expires */
      /* but we must not block the process if the timestamp wanted by the application is older
       * than current time */
      sched = ortp_get_scheduler ();
      packet_time =
	rtp_session_ts_to_t (session,
			     user_ts -
			     session->rtp.rcv_query_ts_offset) +
	session->rtp.rcv_time_offset;
      //ortp_debug ("rtp_session_recvm_with_ts: packet_time=%i, time=%i",packet_time, sched->time_);
      sched_time = sched->time_;
      if (TIME_IS_STRICTLY_NEWER_THAN (packet_time, sched_time))
	{
	  if (session->flags & RTP_SESSION_BLOCKING_MODE)
	    {
	      g_mutex_lock (session->rtp.wait_for_packet_to_be_recv_mutex);
	      g_cond_wait (session->rtp.
			   wait_for_packet_to_be_recv_cond,
			   session->rtp.wait_for_packet_to_be_recv_mutex);
	      g_mutex_unlock (session->rtp.wait_for_packet_to_be_recv_mutex);
	    }
	  session_set_clr (&sched->r_sessions, session);
	}
      else
	session_set_set (&sched->r_sessions, session);	/*to unblock _select() immediately */
    }
#endif  
	
  return mp;
}


gint
msg_to_buf (mblk_t * mp, char *buffer, gint len)
{
  gint rlen = len;
  mblk_t *m, *mprev;
  gint mlen;
  m = mp->b_cont;
  mprev = mp;
  while (m != NULL)
    {
      mlen = m->b_wptr - m->b_rptr;
      if (mlen <= rlen)
	{
	  mblk_t *consumed = m;
	  memcpy (buffer, m->b_rptr, mlen);
	  /* go to next mblk_t */
	  mprev->b_cont = m->b_cont;
	  m = m->b_cont;
	  consumed->b_cont = NULL;
	  freeb (consumed);
	  buffer += mlen;
	  rlen -= mlen;
	}
      else
	{			/*if mlen>rlen */
	  memcpy (buffer, m->b_rptr, rlen);
	  m->b_rptr += rlen;
	  return len;
	}
    }
  return len - rlen;
}

/**
 *rtp_session_recv_with_ts:
 *@session: a rtp session.
 *@buffer:	a user supplied buffer to write the data.
 *@len:		the length in bytes of the user supplied buffer.
 *@time:	the timestamp wanted.
 *@have_more: the address of an integer to indicate if more data is availlable for the given timestamp.
 *
 *	Tries to read the bytes of the incoming rtp stream related to timestamp @time. In case 
 *	where the user supplied buffer @buffer is not large enough to get all the data 
 *	related to timestamp @time, then *( @have_more) is set to 1 to indicate that the application
 *	should recall the function with the same timestamp to get more data.
 *	
 *  When the rtp session is scheduled (see rtp_session_set_scheduling_mode() ), and the 
 *	blocking mode is on (see rtp_session_set_blocking_mode() ), then the calling thread
 *	is suspended until the timestamp given as argument expires, whatever a received packet 
 *	fits the query or not.
 *
 *	Important note: it is clear that the application cannot know the timestamp of the first
 *	packet of the incoming stream, because it can be random. The @time timestamp given to the
 *	function is used relatively to first timestamp of the stream. In simple words, 0 is a good
 *	value to start calling this function.
 *
 *	This function internally calls rtp_session_recvm_with_ts() to get a rtp packet. The content
 *	of this packet is then copied into the user supplied buffer in an intelligent manner:
 *	the function takes care of the size of the supplied buffer and the timestamp given in  
 *	argument. Using this function it is possible to read continous audio data (e.g. pcma,pcmu...)
 *	with for example a standart buffer of size of 160 with timestamp incrementing by 160 while the incoming
 *	stream has a different packet size.
 *
 *Returns: if a packet was availlable with the corresponding timestamp supplied in argument 
 *	then the number of bytes written in the user supplied buffer is returned. If no packets
 *	are availlable, either because the sender has not started to send the stream, or either
 *	because silence packet are not transmitted, or either because the packet was lost during
 *	network transport, then the function returns zero.
**/
gint
rtp_session_recv_with_ts (RtpSession * session, gchar * buffer,
			  gint len, guint32 time, gint * have_more)
{
  mblk_t *mp;
  gint rlen = len;
  gint wlen, mlen;
  guint32 ts_int = 0;		/*the length of the data returned in the user supplied buffer, in TIMESTAMP UNIT */
  PayloadType *payload;

  *have_more = 0;

  mp = rtp_session_recvm_with_ts (session, time);
  payload = rtp_profile_get_payload (session->rcv.profile, session->rcv.payload_type);
  if (payload == NULL)
    {
      g_warning
	("rtp_session_recv_with_ts: unable to recv an unsupported payload.");
      if (mp != NULL)
	freemsg (mp);
      return -1;
    }
  if (!(session->flags & RTP_SESSION_RECV_SYNC))
    {
      //ortp_debug("time=%i   rcv_last_ret_ts=%i",time,session->rtp.rcv_last_ret_ts);
      if (RTP_TIMESTAMP_IS_STRICTLY_NEWER_THAN
	  (time, session->rtp.rcv_last_ret_ts))
	{
	  /* the user has missed some data previously, so we are going to give him now. */
	  /* we must tell him to call the function once again with the same timestamp
	   * by setting *have_more=1 */
	  *have_more = 1;
	}
      if (payload->type == PAYLOAD_AUDIO_CONTINUOUS)
	{

	  ts_int = (guint32) (((double) len) / payload->bytes_per_sample);
	  session->rtp.rcv_last_ret_ts += ts_int;
	  //ortp_debug("ts_int=%i",ts_int);
	}
      else
	ts_int = 0;
    }
  else
    return 0;

  /* try to fill the user buffer */
  while (1)
    {

      if (mp != NULL)
	{
	  mlen = msgdsize (mp->b_cont);
	  wlen = msg_to_buf (mp, buffer, rlen);
	  buffer += wlen;
	  rlen -= wlen;
	  ortp_debug ("mlen=%i wlen=%i rlen=%i", mlen, wlen, rlen);
	  /* do we fill all the buffer ? */
	  if (rlen > 0)
	    {
	      /* we did not fill all the buffer */
	      freemsg (mp);
	      /* if we have continuous audio, try to get other packets to fill the buffer,
	       * ie continue the loop */
	      //ortp_debug("User buffer not filled entirely");
	      if (ts_int > 0)
		{
		  time = session->rtp.rcv_last_ret_ts;
		  ortp_debug ("Need more: will ask for %i.", time);
		}
	      else
		return len - rlen;
	    }
	  else if (mlen > wlen)
	    {
	      int unread = mlen - wlen + (mp->b_wptr - mp->b_rptr);
	      /* not enough space in the user supplied buffer */
	      /* we re-enqueue the msg with its updated read pointers for next time */
	      ortp_debug ("Re-enqueuing packet.");
	      rtp_session_lock (session);
	      rtp_putq (session->rtp.rq, mp);
	      rtp_session_unlock (session);
	      /* quite ugly: I change the stats ... */
	      ortp_global_stats.recv -= unread;
	      session->stats.recv -= unread;
	      return len;
	    }
	  else
	    {
	      /* the entire packet was written to the user buffer */
	      freemsg (mp);
	      return len;
	    }
	}
      else
	{
	  /* fill with a zero pattern (silence) */
	  int i = 0, j = 0;
	  if (payload->pattern_length != 0)
	    {
	      while (i < rlen)
		{
		  buffer[i] = payload->zero_pattern[j];
		  i++;
		  j++;
		  if (j <= payload->pattern_length)
		    j = 0;
		}
	    }
	  return len;
	}
      mp = rtp_session_recvm_with_ts (session, time);
      payload = rtp_profile_get_payload (session->rcv.profile,
					 session->rcv.payload_type);
      if (payload == NULL)
	{
	  g_warning
	    ("rtp_session_recv_with_ts: unable to recv an unsupported payload.");
	  if (mp != NULL)
	    freemsg (mp);
	  return -1;
	}
    }
  return -1;
}

/**
 *rtp_session_get_current_send_ts:
 *@session: a rtp session.
 *
 *	When the rtp session is scheduled and has started to send packets, this function
 *	computes the timestamp that matches to the present time. Using this function can be 
 *	usefull when sending discontinuous streams. Some time can be elapsed between the end
 *	of a stream burst and the begin of a new stream burst, and the application may be not
 *	not aware of this elapsed time. In order to get a valid (current) timestamp to pass to 
 *	#rtp_session_send_with_ts() or #rtp_session_sendm_with_ts(), the application may
 *	use rtp_session_get_current_send_ts().
 *
 *Returns: the actual send timestamp for the rtp session.
**/
guint32
rtp_session_get_current_send_ts (RtpSession * session)
{
  guint32 userts;
  guint32 session_time;
  RtpScheduler *sched = ortp_get_scheduler ();
  PayloadType *payload;
  g_return_val_if_fail (session->snd.payload_type < 128, 0);
  payload = rtp_profile_get_payload (session->snd.profile, session->snd.payload_type);
  g_return_val_if_fail (payload != NULL, 0);
  if ((session->flags & RTP_SESSION_SCHEDULED) == 0)
    {
      g_warning
	("can't guess current timestamp because session is not scheduled.");
      return 0;
    }
  session_time = sched->time_ - session->rtp.snd_time_offset;
  userts =
    (guint32) (((gdouble) (session_time) * (gdouble) payload->clock_rate) /
	       1000.0) + session->rtp.snd_ts_offset;
  return userts;
}

guint32
rtp_session_get_current_recv_ts (RtpSession * session)
{
  guint32 userts;
  guint32 session_time;
  RtpScheduler *sched = ortp_get_scheduler ();
  PayloadType *payload;
  g_return_val_if_fail (session->rcv.payload_type < 128, 0);
  payload = rtp_profile_get_payload (session->rcv.profile, session->rcv.payload_type);
  g_return_val_if_fail (payload != NULL, 0);
  if ((session->flags & RTP_SESSION_SCHEDULED) == 0)
    {
      g_warning
	("can't guess current timestamp because session is not scheduled.");
      return 0;
    }
  session_time = sched->time_ - session->rtp.rcv_time_offset;
  userts =
    (guint32) (((gdouble) (session_time) * (gdouble) payload->clock_rate) /
	       1000.0) + session->rtp.rcv_ts_offset;
  return userts;
}


#ifdef TARGET_IS_HPUXKERNEL
void
rtp_session_set_timeout (RtpSession * session, guint milisec)
{
  return;
}
#else
void
rtp_session_set_timeout (RtpSession * session, guint milisec)
{
  if (milisec == 0)
    {
      session->rtp.timeout = NULL;
      return;
    }
  session->rtp._timeout.tv_sec = milisec / 1000;
  session->rtp._timeout.tv_usec = (milisec % 1000) * 1000000;
  session->rtp.timeout = &session->rtp._timeout;
}
#endif

void
rtp_session_uninit (RtpSession * session)
{
#ifdef MBDB_LOCAL_POOL
  int i;
#endif

  /* first of all remove the session from the scheduler */
#ifdef BUILD_SCHEDULER
  if (session->flags & RTP_SESSION_SCHEDULED)
    {
      rtp_scheduler_remove_session (session->sched, session);
    }
#endif
  /*flush all queues */
  flushq (session->rtp.rq, FLUSHALL);
  flushq (session->rtp.wq, FLUSHALL);
#ifndef TARGET_IS_HPUXKERNEL
  /* close sockets */
  if (USING_TUNNEL (session))
    {
      TUNNEL_CLOSE (session->rtp.tunnel);
      if (session->rtcp.tunnel)
	TUNNEL_CLOSE (session->rtcp.tunnel);
    }
  else
    {
      close_socket (session->rtp.socket);
      close_socket (session->rtcp.socket);
    }
#else
  if (session->dest_mproto != NULL)
    freeb (session->dest_mproto);
#endif
#ifdef BUILD_SCHEDULER
  g_cond_free (session->rtp.wait_for_packet_to_be_sent_cond);
  g_mutex_free (session->rtp.wait_for_packet_to_be_sent_mutex);
  g_cond_free (session->rtp.wait_for_packet_to_be_recv_cond);
  g_mutex_free (session->rtp.wait_for_packet_to_be_recv_mutex);
#endif
#ifdef MBDB_LOCAL_POOL
  for (i = 0; i < RTP_SESSION_SEND_POOL_SIZE; i++)
    {
      freemsg (session->sendbuffer[i]);
    }
  for (i = 0; i < RTP_SESSION_RECV_POOL_SIZE; i++)
    {
      freemsg (session->recvbuffer[i]);
    }


#endif

  g_mutex_free (session->lock);
  session->lock = NULL;
  if (session->current_tev != NULL)
    freemsg (session->current_tev);


}

/**
 *rtp_session_reset:
 *@session: a rtp session.
 *
 *	Reset the session: local and remote addresses are kept unchanged but the internal
 *	queue for ordering and buffering packets is flushed, the session is ready to be
 *	re-synchronised to another incoming stream.
 *
**/
void
rtp_session_reset (RtpSession * session)
{
#ifdef BUILD_SCHEDULER
  if (session->flags & RTP_SESSION_SCHEDULED)
    rtp_session_lock (session);
#endif

  flushq (session->rtp.rq, FLUSHALL);
  flushq (session->rtp.wq, FLUSHALL);
  rtp_session_set_flag (session, RTP_SESSION_RECV_SYNC);
  rtp_session_set_flag (session, RTP_SESSION_SEND_SYNC);
  rtp_session_set_flag (session, RTP_SESSION_RECV_NOT_STARTED);
  rtp_session_set_flag (session, RTP_SESSION_SEND_NOT_STARTED);
  session->peer.ssrc = 0;
  session->rtp.snd_time_offset = 0;
  session->rtp.snd_ts_offset = 0;
  session->rtp.snd_rand_offset = 0;
  session->rtp.snd_last_ts = 0;
  session->rtp.rcv_time_offset = 0;
  session->rtp.rcv_ts_offset = 0;
  session->rtp.rcv_query_ts_offset = 0;
  session->rtp.rcv_app_ts_offset = 0;
  session->rtp.rcv_diff_ts = 0;
  session->rtp.rcv_ts = 0;
  session->rtp.rcv_last_ts = 0;
  session->rtp.rcv_last_app_ts = 0;
  session->rtp.rcv_seq = 0;
  session->rtp.snd_seq = 0;
#ifdef BUILD_SCHEDULER
  if (session->flags & RTP_SESSION_SCHEDULED)
    rtp_session_unlock (session);
#endif
}

/**
 *rtp_session_destroy:
 *@session: a rtp session.
 *
 *	Destroys a rtp session.
 *
**/
void
rtp_session_destroy (RtpSession * session)
{
#if DBG_DELAY
  printf ("Biggest diff: %d\n", biggestdiff);
  printf ("Biggest cnt: %d\n", bigcnt);
  printf ("Low cnt: %d\n", lowcnt);
#endif
  rtp_session_uninit (session);
  g_free (session);
}


guint32
rtp_session_t_to_ts (RtpSession * session, struct timeval *tv)
{
	guint32 millisecs;
	PayloadType *payload;
	struct timeval ntv;
	
	g_return_val_if_fail (session->snd.payload_type < 127, 0);
	payload = rtp_profile_get_payload (session->snd.profile, session->snd.payload_type);
	if (payload == NULL)
	{
		g_warning
				("rtp_session_t_to_ts: use of unsupported payload type.");
		return 0;
	}

	/* the return value is in timestamp units */


  ntv = *tv;

  tvsub (&ntv, &session->start_time);
  millisecs = ntv.tv_sec * 1000 + ntv.tv_usec / 1000;

  return (guint32) session->base_timestamp +
    (millisecs * payload->clock_rate) / 1000;


}


/* function used by the scheduler only:*/
guint32
rtp_session_ts_to_t (RtpSession * session, guint32 timestamp)
{
  PayloadType *payload;
  g_return_val_if_fail (session->snd.payload_type < 127, 0);
  payload = rtp_profile_get_payload (session->snd.profile, session->snd.payload_type);
  if (payload == NULL)
    {
      g_warning ("rtp_session_ts_to_t: use of unsupported payload type.");
      return 0;
    }
  /* the return value is in milisecond */
#ifdef SYNC_OPT
  return (guint32) (100 * timestamp) / (payload->clock_rate / 10);
#else
  return (guint32) (1000.0 *
		    ((double) timestamp / (double) payload->clock_rate));
#endif
}


#ifdef BUILD_SCHEDULER
/* time is the number of miliseconds elapsed since the start of the scheduler */
void
rtp_session_process (RtpSession * session, guint32 time, RtpScheduler * sched)
{
  queue_t *wq = session->rtp.wq;
  rtp_header_t *hdr;
  gint cond = 1;
  guint32 packet_time;
  gint packet_sent = 0;
  guint32 last_recv_time;

  rtp_session_lock (session);

  if (wq->q_first == NULL)
    cond = 0;
  /* send all packets that need to be sent */
  while (cond)
    {
      //g_message("GRMGIMIM");
      if (wq->q_first != NULL)
	{
	  hdr = (rtp_header_t *) wq->q_first->b_rptr;
	  packet_time =
	    rtp_session_ts_to_t (session,
				 hdr->timestamp -
				 session->rtp.
				 snd_ts_offset) +
	    session->rtp.snd_time_offset;
	  /*ortp_debug("session->rtp.snd_time_offset= %i, time= %i, packet_time= %i", 
	     session->rtp.snd_time_offset, time, packet_time); 
	     ortp_debug("seeing packet seq=%i ts=%i",hdr->seq_number,hdr->timestamp); */
	  if (TIME_IS_NEWER_THAN (time, packet_time))
	    {
	      mblk_t *mp;
	      mp = getq (wq);
	      rtp_send (session, mp);
	      packet_sent++;
	    }
	  else
	    cond = 0;
	}
      else
	{
	  cond = 0;

	}
    }
  /* and then try to recv packets */
  rtp_recv (session);

  //ortp_debug("after recv");

  /*if we are in blocking mode or in _select(), we must wake up (or at least notify)
   * the application process, if its last
   * packet has been sent, if it can recv a new packet */

  if (packet_sent > 0)
    {
      /* the session has finished to send: notify it for _select() */
      session_set_set (&sched->w_sessions, session);
      if (session->flags & RTP_SESSION_BLOCKING_MODE)
	{
	  g_mutex_lock (session->rtp.wait_for_packet_to_be_sent_mutex);
	  g_cond_signal (session->rtp.wait_for_packet_to_be_sent_cond);
	  g_mutex_unlock (session->rtp.wait_for_packet_to_be_sent_mutex);
	}
    }

  if (!(session->flags & RTP_SESSION_RECV_NOT_STARTED))
    {
      //ortp_debug("unblocking..");
      /* and also wake up the application if the timestamp it asked has expired */
      last_recv_time =
	rtp_session_ts_to_t (session,
			     session->rtp.rcv_last_app_ts -
			     session->rtp.
			     rcv_query_ts_offset) +
	session->rtp.rcv_time_offset;
      //ortp_debug("time=%i, last_recv_time=%i",time,last_recv_time);
      if TIME_IS_NEWER_THAN
	(time, last_recv_time)
	{
	  /* notify it in the w_sessions mask */
	  session_set_set (&sched->r_sessions, session);
	  if (session->flags & RTP_SESSION_BLOCKING_MODE)
	    {
	      //ortp_debug("rtp_session_process: Unlocking.");
	      g_mutex_lock (session->rtp.wait_for_packet_to_be_recv_mutex);
	      g_cond_signal (session->rtp.wait_for_packet_to_be_recv_cond);
	      g_mutex_unlock (session->rtp.wait_for_packet_to_be_recv_mutex);
	    }
	}
    }
  rtp_session_unlock (session);
}

#endif

/* packet api */

void
rtp_add_csrc (mblk_t * mp, guint32 csrc)
{
  rtp_header_t *hdr = (rtp_header_t *) mp->b_rptr;
  hdr->csrc[hdr->cc] = csrc;
  hdr->cc++;
}



