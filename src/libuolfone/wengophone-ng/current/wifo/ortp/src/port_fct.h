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

/* fct-win32.h.  define unix methods that are not defined in win32 env  */

#ifdef _WIN32
#include <winsock.h>
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define random rand
#define gettimeofday rtp_gettimeofday


#ifdef __cplusplus
extern "C" {
#endif

int rtp_gettimeofday (struct timeval *tp, void *tz);

#ifdef __cplusplus
}
#endif

#else
#include <fcntl.h>
#endif

#include "rtpsession.h"

#ifndef _WIN32
/* use unix pthread_t... */
	extern int close_socket(gint sock);
#else
	#define pthread_t	HANDLE
	extern int pthread_create(pthread_t *thread,const void *attr,void *(__cdecl *start)(void *),void* arg);
	extern int pthread_join(pthread_t thread,void **);

	extern int close_socket(SOCKET sock);
	extern int inet_aton (const char * cp, struct in_addr * addr);
#endif

extern int set_non_blocking_socket (RtpSession *session);
extern int set_socket_non_blocking (gint socket);
extern int set_thread_priority();




