  /*
  The oRTP LinPhone RTP library intends to provide basics for a RTP stack.
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
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#endif

int runcond=1;

#define PCM  0
#define MULAW 1
#define ALAW 2

void stophandler(int signum)
{
	runcond=0;
}

static char *help="usage: rtpsend	filename dest_ip4addr dest_port [-s] [-r] [-b] [--format]\n";

char buffer[160];

int main(int argc, char *argv[])
{
	RtpSession *session;
	int i;
	FILE *infile;
	char *ssrc;
	gint sched=0;
	gint blocking=0;
	gint noreason=0;
	guint32 user_ts=0;
	gint format=PCM;
	guint32 samplet = 20000;    // 20ms in us
	struct timeval prev,new;
	gint32 diff;

	if (argc<4){
		printf(help);
		exit(-1);
	}
	for (i=4;i<argc;i++)
	{
		if (strcmp(argv[i],"-s")==0){
			sched=1;
		}
		else if (strcmp(argv[i],"-r")==0){
			noreason=1;
		}
		else if (strcmp(argv[i],"-b")==0){
			blocking=1;
		}
		else if (strcmp(argv[i],"--format")==0){
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

	}
	ortp_init();
	if(sched)
	  ortp_scheduler_init();
	//ortp_set_debug_file("oRTP",stdout);
	session=rtp_session_new(RTP_SESSION_SENDONLY);	
	
	rtp_session_set_scheduling_mode(session,sched);
	rtp_session_set_blocking_mode(session,blocking);
	rtp_session_set_remote_addr(session,argv[2],atoi(argv[3]));
	rtp_session_set_payload_type(session,0);
	
	ssrc=getenv("SSRC");
	if (ssrc!=NULL) {
		printf("using SSRC=%i.\n",atoi(ssrc));
		rtp_session_set_ssrc(session,atoi(ssrc));
	}
		
	#ifndef _WIN32
	infile=fopen(argv[1],"r");
	#else
	infile=fopen(argv[1],"rb");
	#endif

	if (infile==NULL) {
		perror("Cannot open file");
		exit(-1);
	}

	signal(SIGINT,stophandler);

	while( ((i=fread(buffer,1,160,infile))>0) && (runcond) )
	{
	if(!sched)
	  {
	    gettimeofday(&prev, NULL);
	  }
		rtp_session_send_with_ts(session,buffer,i,user_ts);
		user_ts+=160;
		if(!sched){
		  {
		    gettimeofday(&new,NULL);
		    diff=(new.tv_sec-prev.tv_sec)*1000000;
		    if(new.tv_usec >= prev.tv_usec)
		      diff +=new.tv_usec-prev.tv_usec;
		    else
		      diff += (1000000-prev.tv_usec + new.tv_usec); 
		  }
		  //		  usleep(samplet - diff);
		}
	}
#ifdef RTCP
	printf("\nSending BYE packet\n");
	if(noreason)
	  rtp_session_send_bye(session, NULL);
	else
	  rtp_session_send_bye(session, "END");
#endif
	fclose(infile);
	rtp_session_destroy(session);
	ortp_exit();
	ortp_global_stats_display();
	exit(0);
}
	
