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


#include "rtptimer.h"
#include <stdio.h>
//#include <time.h>

int main(int argc, char *argv[])
{
	RtpTimer *timer=&posix_timer;
	int i;
	struct timeval interval;
	struct timeval stv,etv;
	long dsec, dusec;
	int limit = 1000;

	interval.tv_sec=0;
	interval.tv_usec=10000;

	ortp_set_debug_file("oRTP", stdout);
	
	rtp_timer_set_interval(timer,&interval);
	
	for(i = 1; i < argc; i++)
	  {
	    if (!strcasecmp("-i", argv[i])) 
	      interval.tv_usec=atoi(argv[i++ + 1]);
	    else if (!strcasecmp("-l", argv[i])) 
	        limit = atoi(argv[i++ + 1]);
	  }

	printf("timer test %s %s\n", __DATE__, __TIME__);
	timer->timer_init();
	gettimeofday(&stv, 0);
	for (i=0;i<limit;i++)
	{
	  //		printf("doing something... %d\n", i);
		timer->timer_do();
	}
	gettimeofday(&etv, 0);
	

	timer->timer_uninit();

	dsec = etv.tv_sec - stv.tv_sec;
	dusec = etv.tv_usec - stv.tv_usec;
	if (dusec < 0)
	  {
	    dsec -= 1;
	    dusec += 1000000;
	  }

	printf("stv==%u.%u etv=%u.%u diff=%d.%d\n", stv.tv_sec,stv.tv_usec, etv.tv_sec,etv.tv_usec, dsec,dusec);
	return 0;
}
