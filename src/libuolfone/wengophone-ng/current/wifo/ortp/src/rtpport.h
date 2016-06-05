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


/* this file is responsible of the portability of the stack */

#ifndef RTPPORT_H
#define RTPPORT_H



#ifndef _WIN32	/* do not include ortp-config.h when we are on win32 */
#include <ortp-config.h>
#else
#include "ortp-config-win32.h"
#include <windows.h>
#include <process.h>
#ifdef __MINGW32__
#include <stdint.h>
#else
typedef unsigned __int64 uint64_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
#endif
#ifdef _MSC_VER
#define inline _inline
#endif
#endif


#ifndef WORDS_BIGENDIAN
#if defined(__powerpc__) || defined(__PPC__) || defined(__ppc__) || defined(__POWERPC__)
#define WORDS_BIGENDIAN 1
#endif
#endif


/* avoid using glib types if we are compiling for HPUX kernel */
#ifdef TARGET_IS_HPUXKERNEL
#undef HAVE_GLIB
#endif


/* defines things that should be defined when we have not glib */
#if defined(HAVE_GLIB)
#include <glib.h>

#else


#include <errno.h>
#include <sys/types.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

/* integer types */
typedef  uint64_t guint64;
typedef  uint16_t guint16;
typedef  uint32_t guint32;
typedef  signed short gint16;
typedef  int32_t gint32;
typedef  unsigned int guint;
typedef  int gint;
typedef  char gchar;
typedef  unsigned char guchar;
typedef  unsigned char guint8;
typedef  void* gpointer;
typedef int gboolean;
typedef double gdouble;

#define TRUE 1
#define FALSE 0

/*misc*/
#define g_return_if_fail(expr) if (!(expr)) {printf("%s:%i- assertion #expr failed\n",__FILE__,__LINE__); return;}
#define g_return_val_if_fail(expr,ret) if (!(expr)) {printf("%s:%i- assertion #expr failed\n",__FILE__,__LINE__); return (ret);}

#ifndef _WIN32
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdarg.h>


typedef enum {
	/* GLib log levels */
  G_LOG_LEVEL_ERROR             = 1 << 2,       /* always fatal */
  G_LOG_LEVEL_CRITICAL          = 1 << 3,
  G_LOG_LEVEL_WARNING           = 1 << 4,
  G_LOG_LEVEL_MESSAGE           = 1 << 5,
  G_LOG_LEVEL_MASK              = ~0

} GLogLevelFlags;

#ifndef G_LOG_DOMAIN
#define G_LOG_DOMAIN ""
#endif


void g_log(const gchar *log_domain,GLogLevelFlags log_level,const gchar *format,...);
void g_logv(const gchar *log_domain,GLogLevelFlags log_level,const gchar *format,va_list args);
typedef void (*GLogFunc)  (const gchar *log_domain,
                                          GLogLevelFlags log_level,
                                           const gchar *message,
                                           gpointer user_data);
static inline void g_warning(const gchar *fmt,...)
{
  va_list args;
  va_start (args, fmt);
  g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_WARNING, fmt, args);
  va_end (args);
}
static inline void g_error(const gchar *fmt,...)
{
  va_list args;
  va_start (args, fmt);
  g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_ERROR, fmt, args);
  va_end (args);
}
static inline void g_message(const gchar *fmt,...)
{
  va_list args;
  va_start (args, fmt);
  g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_MESSAGE, fmt, args);
  va_end (args);
}
/* in order to simplify, domain is ignored when using the folowing function, ie all logs will go through your handler
whatever the domain is */
void g_log_set_handler(const gchar *domain, GLogLevelFlags levels, GLogFunc func, gpointer ud);



	
/* specific things for HPUX kernel (without glib of course) */	
#ifdef TARGET_IS_HPUXKERNEL

/* REVISIT: are all HP-UX kernel big endian ? */
#define htons(x)	(x)
#define htonl(x)	(x)
#define ntohs(x)	(x)
#define ntohl(x)	(x)

#define memcpy(dest,src,size)	bcopy((src),(dest),(size))
#define memset(p,v,n)		do { int _i; for (_i=0;_i<(n);_i++) ((char*)(p))[_i]=(v);}while (0)

#include <h/malloc.h>
#include <h/spinlock.h>
typedef lock_t GMutex;
static GMutex * g_mutex_new()
{
	lock_t *lock;
	MALLOC(lock,lock_t*,sizeof(lock_t),M_SPINLOCK, M_WAITOK|M_ALIGN);
	initlock(lock,0,0,"RtpSession lock");
	return lock;
}
#define g_mutex_lock(mutex)	spinlock((mutex))
#define g_mutex_unlock(mutex) spinunlock((mutex))
#define g_mutex_free(mutex)	FREE((void*)(mutex),M_SPINLOCK)

/*#define g_malloc(sz)  kmem_alloc(sz) */
/*#define g_free(p)     kmem_free(p) */

/*#define g_malloc(size)	("you cannot use g_malloc in hpux kernel but the MALLOC() macro instead") */
#define g_free(p)	FREE(p,M_IOSYS)

/* include the HPUX streams header*/
#include <h/stream.h>
#include <h/socket.h>
#include <netinet/in.h> 
#include <h/tihdr.h>

#else /*TARGET_IS_HPUXKERNEL */

/* things defined if we are not in HPUX kernel */


/* things that in glib, but should only be defined when we are not in the HPUX kernel. */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef _WIN32
#include <pthread.h>
#include <sched.h>
#endif



#ifdef ENABLE_MEMCHECK
extern gint ortp_allocations;
#endif

/* memory allocation functions */
static inline void * g_malloc(int sz) 
{
	void *p=malloc(sz);
	if (p==NULL) {
		printf("g_malloc: Failed to allocate %i bytes: %s.\n",sz,strerror(errno));
		abort();
	}
#ifdef ENABLE_MEMCHECK
	ortp_allocations++;
#endif
	return p;
}

static inline void * g_malloc0(int sz) 
{
	void *p=malloc(sz);
	if (p==NULL) {
		printf("g_malloc: Failed to allocate %i bytes: %s.\n",sz,strerror(errno));
		abort();
	}
	memset(p,0,sz);
#ifdef ENABLE_MEMCHECK
	ortp_allocations++;
#endif
	return p;
}

#define g_new(type,count)   (type *)g_malloc(sizeof(type)*(count))
#define g_new0(type, count)	(type *)g_malloc0(sizeof(type)*(count))
#define g_realloc(p,sz) realloc((p),(sz))
static inline void g_free(void *p)
{
#ifdef ENABLE_MEMCHECK
	ortp_allocations--;
#endif
	free(p);
}

#define g_strdup(machin)	strdup(machin)

typedef enum
{
  G_THREAD_PRIORITY_LOW,
  G_THREAD_PRIORITY_NORMAL,
  G_THREAD_PRIORITY_HIGH,
  G_THREAD_PRIORITY_URGENT
} GThreadPriority;

typedef gpointer (*GThreadFunc)(gpointer data);


#ifndef _WIN32

typedef pthread_t GThread;
typedef pthread_mutex_t GMutex;



static inline GThread *g_thread_create(GThreadFunc func, gpointer data, gboolean joinable, void **error){
	GThread *thread=g_new(GThread,1);
	pthread_create(thread,NULL,func,data);
	return thread;
}

static inline void g_thread_join(GThread *thread){
	pthread_join(*thread,NULL);
	g_free(thread);
}

static inline void g_thread_set_priority(GThread *thread,GThreadPriority prio){
	if (prio>G_THREAD_PRIORITY_NORMAL){
		/* this is unsupported on HPUX */
		/*
		struct sched_param param;
		param.sched_priority=1;
		sched_setscheduler(*thread,SCHED_RR,&param);
		*/
	}
}


static inline GMutex * g_mutex_new()
{
	pthread_mutex_t *mutex=g_new(pthread_mutex_t,1);
	pthread_mutex_init(mutex,NULL);
	return mutex;
}

#define g_mutex_lock(mutex)	  pthread_mutex_lock((mutex))
#define g_mutex_unlock(mutex)  pthread_mutex_unlock((mutex))
#define g_mutex_free(mutex)		pthread_mutex_destroy((mutex));g_free((mutex))

typedef pthread_cond_t GCond;
static inline GCond * g_cond_new()
{
	pthread_cond_t *cond=g_new(pthread_cond_t,1);
	pthread_cond_init(cond,NULL);
	return cond;
}

#define g_cond_wait(cond,mutex)	pthread_cond_wait((cond),(mutex))
#define g_cond_signal(cond)		pthread_cond_signal((cond))
#define g_cond_broadcast(cond)	pthread_cond_broadcast((cond))
#define g_cond_free(cond)		pthread_cond_destroy((cond)); g_free((cond))

#else

/* WIN32 version */
typedef struct _gthread
{
  HANDLE h;
  DWORD  id;
} GThread;


static inline GThread *
g_thread_create(GThreadFunc func, gpointer data, gboolean joinable, void **error)
{
  GThread *thread=g_new(GThread,1);

  if (thread == NULL)
    return NULL;
/**<JT> 
 * fixed to use oRTP lib in win32 with visual studio.
 */
#if 0
     thread->h = (HANDLE) CreateThread (NULL,	/* default security attr */
				       0,	/* use default one */
				       (LPTHREAD_START_ROUTINE)(void *) func, data, 0, &(thread->id));   
#else
     thread->h = (HANDLE) _beginthreadex (NULL,	/* default security attr */
				       0,	/* use default one */
				       (unsigned ( __stdcall * )( void * ))func, data, 0, (unsigned int *) &(thread->id));
#endif  
/*<JT/>*/
  if (thread->h == 0)
    {
      g_free (thread);
      return NULL;
    }

  return thread;
}

static inline void 
g_thread_join(GThread *thread)
{
  if (thread == NULL)
    return;
  WaitForSingleObject (thread->h, INFINITE);
  CloseHandle (thread->h);
  g_free(thread);
}

static inline void 
g_thread_set_priority(GThread *thread,GThreadPriority prio)
{
	DWORD winPrio = THREAD_PRIORITY_NORMAL;

	switch(prio)
	{
	case  G_THREAD_PRIORITY_LOW:
		winPrio = THREAD_PRIORITY_BELOW_NORMAL;
		break;

	case   G_THREAD_PRIORITY_NORMAL:
		winPrio = THREAD_PRIORITY_NORMAL;
		break;


	case	G_THREAD_PRIORITY_HIGH:
		winPrio = THREAD_PRIORITY_HIGHEST;
		break;

	case    G_THREAD_PRIORITY_URGENT:
		winPrio = THREAD_PRIORITY_TIME_CRITICAL;
		break;
	}

	SetThreadPriority(GetCurrentThread(), winPrio );


}


static inline void
g_thread_exit ()
{
  /* ExitThread(0); */
  _endthreadex (0);
}

typedef struct _gmutex 
{
  HANDLE h;
} GMutex;


static inline GMutex *
g_mutex_new()
{
  GMutex* mutex=g_new(GMutex,1);
  
  if (!mutex)
    return 0;
  
  if ((mutex->h = CreateMutex (NULL, FALSE, NULL)) != NULL)
    return mutex;
  g_free (mutex);
  return (NULL);

}



static inline void 
g_mutex_free(GMutex *mut)
{
  if (mut == NULL)
    return;
  CloseHandle (mut->h);
  g_free (mut);
}

static inline void
g_mutex_lock (GMutex *mut)
{
   WaitForSingleObject (mut->h, INFINITE);
}

static inline void
g_mutex_unlock (GMutex *mut)
{
  ReleaseMutex (mut->h);
}



typedef struct _gcond
{
  HANDLE sleepevt;
  int    sleepers;
} GCond;


static inline GCond   *g_cond_new()
{
	
  GCond  *cond = g_new(GCond, 1);
   if ((cond->sleepevt = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL)
    {
      cond->sleepers = 0;
      return cond;
    }
  g_free (cond);

  return NULL;

}


static inline void     
g_cond_signal(GCond  *cond)
{
    SetEvent(cond->sleepevt);
}

static inline  void     
g_cond_broadcast(GCond *cond)
{
  int cnt = cond->sleepers;
  while(cnt--)
    SetEvent(cond->sleepevt);
}

static inline void     
g_cond_wait(GCond *cond, GMutex *mut)
{
  DWORD err;

  InterlockedIncrement((volatile LONG *)&cond->sleepers);
  g_mutex_unlock(mut);

  err = WaitForSingleObject(cond->sleepevt, INFINITE);

  g_mutex_lock(mut);

  InterlockedDecrement((volatile LONG *)&cond->sleepers);



}

static inline  void  
g_cond_free(GCond  *cond)
{
  CloseHandle(cond->sleepevt);
  g_free(cond);
}


#endif /* win32 */

#define g_thread_init(vtable)
#define g_thread_supported()	(1)

#endif /* HAVE_GLIB */


#endif /*TARGET_IS_HPUXKERNEL */



#ifndef RTP_DEBUG
#define ortp_debug
#else
#define ortp_debug	g_message
#endif

#ifdef _WIN32
extern char *getSocketError();
#else
#define getSocketError() strerror(errno)
#endif


#endif /*RTPPORT_H*/
