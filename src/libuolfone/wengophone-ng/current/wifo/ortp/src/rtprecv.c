 /*
  The lprtplib LinPhone RTP library intends to provide basics for a RTP stack.
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
#include <signal.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#endif

gint cond=1;

void stop_handler(int signum)
{
	cond=0;
}
gint get_sockets(gchar *addr, gint port, gint *rtp_s, gint *rtcp_s)
{
	struct sockaddr_in rtp_addr, rtcp_addr;
	gint err, rtpsocket, rtcpsocket;
	gint optval=1;

	rtp_addr.sin_family = AF_INET;

	err = inet_aton (addr, &rtp_addr.sin_addr);

	if (err < 0)
	{
		g_warning ("Error in socket address:%s.", getSocketError());
		return err;
	}
	rtp_addr.sin_port = htons (port);

	rtpsocket = socket (PF_INET, SOCK_DGRAM, 0);
	g_return_val_if_fail (rtpsocket > 0, -1);

	/* set non blocking mode */
	set_socket_non_blocking (rtpsocket);

	err = bind (rtpsocket, (struct sockaddr *) &rtp_addr, sizeof (struct sockaddr_in));

	if (err != 0)
	{
		g_warning ("Fail to bind rtp socket to port %i: %s.", port, getSocketError());
		close_socket (rtpsocket);
		return -1;
	}
	/* set the address reusable */
	err = setsockopt (rtpsocket, SOL_SOCKET, SO_REUSEADDR,
			  (void*)&optval, sizeof (optval));
	if (err < 0)
	{
		g_warning ("Fail to set rtp address reusable: %s.", getSocketError());
	}
	/* RTCP socket */
	memcpy (&rtcp_addr, &rtp_addr,
		sizeof (struct sockaddr_in));
	rtcp_addr.sin_port = htons (port + 1);
	rtcpsocket = socket (PF_INET, SOCK_DGRAM, 0);
	g_return_val_if_fail (rtcpsocket > 0, -1);
	/* set non blocking mode */
	set_socket_non_blocking (rtcpsocket);
	err = bind (rtcpsocket, (struct sockaddr *) &rtcp_addr,
		    sizeof (struct sockaddr_in));
	if (err != 0)
	{
		g_warning ("Fail to bind rtcp socket to port %i: %s.", port + 1, getSocketError());
		close_socket (rtpsocket);
		close_socket (rtcpsocket);
		return -1;
	}
	optval = 1;
	err = setsockopt (rtcpsocket, SOL_SOCKET, SO_REUSEADDR,
			  (void*)&optval, sizeof (optval));
	if (err < 0)
	{
		g_warning ("Fail to set rtcp address reusable: %s.",getSocketError());
	}
	*rtp_s = rtpsocket; 
	*rtcp_s = rtcpsocket; 
}

void on_bye(RtpSession *session, gpointer bye)
{
  printf("Got BYE packet, exiting!\n");
  if(bye != NULL)
    {
      unsigned char reason[16];
      unsigned char len = *(unsigned char *)bye;
      if(len > 15)
	len = 15;
      strncpy(reason, (unsigned char *)bye+1, len); 
      reason[len]=0;
      printf("Reason: %s\n", reason);
    }
  cond = 0;
}
void ssrc_cb(RtpSession *session)
{
	printf("hey, the ssrc has changed !\n");
}

static char *help="usage: rtprecv  filename loc_port [--format format] [--soundcard] [-s] [-e]\n";

#define PCM  0
#define MULAW 1
#define ALAW 2

#if HOST_IS_HPUX && HAVE_SYS_AUDIO_H

#include <sys/audio.h>

gint sound_init(gint format)
{
	int fd;
	fd=open("/dev/audio",O_WRONLY);
	if (fd<0){
		perror("Can't open /dev/audio");
		return -1;
	}
	ioctl(fd,AUDIO_RESET,0);
	ioctl(fd,AUDIO_SET_SAMPLE_RATE,8000);
	ioctl(fd,AUDIO_SET_CHANNELS,1);
	if (format==MULAW)
		ioctl(fd,AUDIO_SET_DATA_FORMAT,AUDIO_FORMAT_ULAW);
	else ioctl(fd,AUDIO_SET_DATA_FORMAT,AUDIO_FORMAT_ALAW);
	return fd;	
}
#else
gint sound_init(gint format)
{
	return -1;
}
#endif
char buffer[160];

int main(int argc, char*argv[])
{
	RtpSession *session;
	int err;
	guint32 ts=0;
	gint stream_received=0;
	FILE *outfile;
	gint local_port;
	gint have_more;
	gint i;
	gint format=PCM;
	gint samplet = 20000;    /* in usec, 20ms */
	gint soundcard=0;
	gint sched=0;
	gint blocking=0;
	gint use_ext_sockets=0;
	gint sound_fd=0;
	struct timeval prev,new;
	gint32 diff;
	FILE *logf = NULL;
	char* logfn = getenv("RTPLOG");

	/* init the lib */
	if (argc<3){
		printf(help);
		exit(-1);
	}
	local_port=atoi(argv[2]);
	if (local_port<=0) {
		printf(help);
		exit(-1);
	}
	for (i=3;i<argc;i++)
	{
		if (strcmp(argv[i],"--format")==0){
			i++;
			if (i<argc){
				if (strcmp(argv[i],"mulaw")==0){
					format=MULAW;
				}else
				if (strcmp(argv[i],"alaw")==0){
					format=ALAW;
				}else{
					printf("Unsupported format %s\n",argv[i]);
					exit(-1);
				}
			}
		}
		else if (strcmp(argv[i],"--soundcard")==0){
			soundcard=1;
		}
		else if (strcmp(argv[i],"-s")==0){
			sched=1;
		}
		else if (strcmp(argv[i],"-b")==0){
			blocking=1;
		}
		else if (strcmp(argv[i],"-e")==0){
			use_ext_sockets=1;
		}
	}
	
	outfile=fopen(argv[1],"wb");
	if (outfile==NULL) {
		perror("Cannot open file for writing");
		exit(-1);
	}
	
	
	if (soundcard){
		sound_fd=sound_init(format);
	}
	
	ortp_init();
	if(sched)
	  ortp_scheduler_init();
	
	signal(SIGINT,stop_handler);

	if (logfn != NULL)
	  logf = fopen(logfn, "w");
	
	ortp_set_debug_file("oRTP", logf);
	session=rtp_session_new(RTP_SESSION_RECVONLY);	
	rtp_session_set_scheduling_mode(session,sched);
	rtp_session_set_blocking_mode(session,blocking);
	rtp_session_set_jitter_compensation(session,60);
	if(use_ext_sockets){
	  gint rtp, rtcp;
	  gint ret = get_sockets("0.0.0.0", atoi(argv[2]), &rtp, &rtcp);
	  if(ret == -1){
	    printf("Getting sockets failed\n");
	    goto end;
	  }
	  printf("Using external sockets %d %d\n", rtp, rtcp);
	  rtp_session_set_sockets(session, rtp, rtcp);
	}else{
	  rtp_session_set_local_addr(session,"0.0.0.0",atoi(argv[2]));
	}
	rtp_session_set_payload_type(session,0);
	rtp_session_signal_connect(session,"ssrc_changed",(RtpCallback)ssrc_cb,NULL);
	rtp_session_signal_connect(session,"ssrc_changed",(RtpCallback)rtp_session_reset,NULL);
	rtp_session_signal_connect(session,"bye_packet",(RtpCallback)on_bye,NULL);
	
	while(cond)
	{
	if(!sched)
	  {
	    gettimeofday(&prev, NULL);
	  }
		have_more=1;
		while (have_more){
			err=rtp_session_recv_with_ts(session,buffer,160,ts,&have_more);
			//			if (err>0) stream_received=1;
			/* this is to avoid to write to disk some silence before the first RTP packet is returned*/	
			if (err>0) {
				fwrite(buffer,1,err,outfile);
				if (sound_fd>0) write(sound_fd,buffer,err);
			}
		}
		if(!sched){
		    gettimeofday(&new,NULL);
		    diff=(new.tv_sec-prev.tv_sec)*1000000;
		    if(new.tv_usec >= prev.tv_usec)
		      diff += new.tv_usec-prev.tv_usec;
		    else
		      diff += (1000000-prev.tv_usec + new.tv_usec); 
		    
		    usleep(samplet - diff); 
		}
		ts+=160;
		g_message("Receiving packet.");
	}
 end:	
	rtp_session_destroy(session);
	ortp_exit();
	
	ortp_global_stats_display();
	
	exit(0);
}
