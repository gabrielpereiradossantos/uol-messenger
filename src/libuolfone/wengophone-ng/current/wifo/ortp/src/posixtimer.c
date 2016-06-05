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

#include "rtpport.h"
#include "rtptimer.h"

#ifdef BUILD_SCHEDULER

#ifndef _WIN32

#include <signal.h>
#include <math.h>
#include <string.h>


static guint32 late_ticks;
static struct timeval prev,new;
static int alarm_received=0;

static guint32 posix_timer_time=0;		/*in milisecond */
static gint32 posix_timer_interval = POSIXTIMER_INTERVAL/1000;		/*in milisecond */

void posix_timer_init()
{
	struct itimerval timer;
	sigset_t set;
	/* block the SIGALRM signal */
	sigemptyset(&set);
	sigaddset(&set,SIGALRM);
	sigprocmask(SIG_BLOCK,&set,NULL);
	timer.it_value.tv_sec=posix_timer.interval.tv_sec;
	timer.it_value.tv_usec=posix_timer.interval.tv_usec;
	timer.it_interval.tv_sec=posix_timer.interval.tv_sec;
	timer.it_interval.tv_usec=posix_timer.interval.tv_usec;
	posix_timer_interval = posix_timer.interval.tv_sec * 1000 + posix_timer.interval.tv_usec / 1000;
	//	g_warning("posix_interval = %d\n", posix_timer_interval);
	setitimer(ITIMER_REAL,&timer,NULL);
	posix_timer.state=RTP_TIMER_RUNNING;
	gettimeofday(&prev,NULL);
	late_ticks=0;
	posix_timer_time=0;
}



void dummy_handler(int signum)
{
	posix_timer_time+=posix_timer_interval;
	alarm_received++;
}

void posix_timer_do()
{
	sigset_t set;
	gint32 diff;
	gint32 time;

	if (late_ticks>0){
		late_ticks--;
		posix_timer_time += posix_timer_interval;
		return;
	}


	gettimeofday(&new,NULL);
	time=((new.tv_usec-prev.tv_usec)/1000 ) + ((new.tv_sec-prev.tv_sec)*1000 );
	diff=time-posix_timer_time;
	if (diff > posix_timer_interval){
		late_ticks=diff/posix_timer_interval - 1;
		if (late_ticks > 4) g_warning("we must catchup %i ticks.\n",late_ticks);
		posix_timer_time += posix_timer_interval;
		return;
	}

	
	sigfillset(&set);
	sigdelset(&set,SIGALRM);
	alarm_received=0;
	signal(SIGALRM,dummy_handler);
	while (1) 
	{
		sigsuspend(&set);
		if (alarm_received>1){
			g_message("alarm received=%i",alarm_received);
			return;
		}
		if (alarm_received==1) return;
		g_warning("posix_timer_do: we received an unknow signal !!!!\n");
	}
	/* restore the old mask */
	//sigprocmask(SIG_SETMASK,&oldset,NULL);
}

void posix_timer_uninit()
{
	struct itimerval timer;
	/* unset the timer */
	memset(&timer,0,sizeof(struct itimerval));
	setitimer(ITIMER_REAL,&timer,NULL);
	posix_timer.state=RTP_TIMER_STOPPED;
}

RtpTimer posix_timer={	0,
						posix_timer_init,
						posix_timer_do,
						posix_timer_uninit,
						{0,POSIXTIMER_INTERVAL}};
							
							
#else //WIN32

#include <windows.h>
#include <mmsystem.h>


MMRESULT timerId;
HANDLE   TimeEvent;
int      late_ticks;


static DWORD posix_timer_time;
static DWORD offset_time;


#define TIME_INTERVAL           10
#define TIME_RESOLUTION         10
#define TIME_TIMEOUT            100



void CALLBACK timerCb(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
        // Check timerId
        if (timerId == uID)
        {
                posix_timer_time += TIME_INTERVAL;
                SetEvent(TimeEvent);
        }
}


void win_timer_init(void)
{
        posix_timer_time=0;

        timerId = timeSetEvent(TIME_INTERVAL,10,timerCb,0,TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
        TimeEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

        late_ticks = 0;

        offset_time = GetTickCount();

}

void win_timer_do(void)
{
        DWORD diff;

        // If timer have expired while we where out of this method
        // Try to run after lost time.
        if (late_ticks > 0)
        {
                late_ticks--;
                InterlockedExchangeAdd(&posix_timer_time,TIME_INTERVAL);
                return;
        }


        diff = GetTickCount() - posix_timer_time - offset_time;
        if( diff>TIME_INTERVAL && (diff<(1<<31)))
        {
                late_ticks = diff/TIME_INTERVAL - 1;
                g_warning("we must catchup %i ticks.",late_ticks);
                return;
        }

        WaitForSingleObject(TimeEvent,TIME_TIMEOUT);
        return;
}


void win_timer_close(void)
{
	timeKillEvent(timerId); 
}

RtpTimer toto;

RtpTimer posix_timer={	0,
						win_timer_init,
						win_timer_do,
						win_timer_close,
						{0,TIME_INTERVAL * 1000}};
							

#endif // _WIN32
#endif // BUILD_SCHEDULER
