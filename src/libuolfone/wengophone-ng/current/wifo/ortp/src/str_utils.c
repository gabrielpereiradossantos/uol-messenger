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

#include <stdio.h>
#include <string.h>

#include "rtpport.h"

#ifndef TARGET_IS_HPUXKERNEL

#include "rtp.h"
#include "str_utils.h"


#ifdef MBDB_LOCAL_POOL
volatile mblk_t *free_mblks;
volatile dblk_t *free_dblks;
#define N_MBLKS 64 
#define N_DBLKS (N_MBLKS*2)

GMutex *mblk_mutex;
GMutex *dblk_mutex;


#ifndef NO_MBLK_LOCK
#define mblk_lock() g_mutex_lock(mblk_mutex)
#define mblk_unlock() g_mutex_unlock(mblk_mutex)

#define dblk_lock() g_mutex_lock(dblk_mutex)
#define dblk_unlock() g_mutex_unlock(dblk_mutex)
#else
#define mblk_lock()
#define mblk_unlock()

#define dblk_lock()
#define dblk_unlock()
#endif

void mbdb_mutex_init()
{
	mblk_mutex = g_mutex_new();
	dblk_mutex = g_mutex_new();
}

void mbdb_mutex_cleanup()
{
	g_mutex_free(dblk_mutex);
	dblk_mutex = 0;
	g_mutex_free(mblk_mutex);
	mblk_mutex = 0;

}


mblk_t  *mblk_alloc()
{
	mblk_t *mp;

	mblk_lock();
	
	mp = free_mblks;
	if (0 == mp)
		mp = (mblk_t*)g_malloc(sizeof(mblk_t));
	else
		free_mblks = mp->b_next;

	mblk_unlock();
	
	return mp;
}

void mblk_free(mblk_t* mp)
{
	mblk_lock();

	mp->b_next = free_mblks;
	free_mblks = mp;

	mblk_unlock();
	
}

dblk_t *dblk_alloc()
{
	dblk_t *d;

	dblk_lock();
	
	d = free_dblks;

	
	if (0 == d)
		d = (dblk_t*) g_malloc(sizeof(dblk_t));
	else
		free_dblks = (dblk_t*) (d->db_base);

	dblk_unlock();
	
	return d;
}
	
void dblk_free(dblk_t* d)
{
	dblk_lock();
	
	d->db_base = free_dblks;
	free_dblks = d;

	dblk_unlock();
	
}

/*
 * preallocate mblk and dblk pools
 * 
 */ 
void mbdb_init()
{
	int i;
	mblk_t *mp;
	dblk_t *db;
	
	for(i = N_MBLKS; i--; )
	  {
	    mp = g_malloc(sizeof(mblk_t));
	    if(!mp)
	      {
		printf("malloc failed for %d mblk !!!\n\r",i);
		break;
	      }else
		mblk_free(mp);
	  }

	for(i = N_DBLKS; i--; )
	  {
	    db = g_malloc(sizeof(dblk_t));
	    if(!db)
	      {
		printf("malloc failed for %d dblk !!!\n\r",i);
		break;
	      }else
		dblk_free(db);
	  }
}

/*
 * free mblk and dblk pools
 *
 */
void mbdb_clean()
{
	while(free_mblks)
		g_free(mblk_alloc());
		
	while(free_dblks)
		g_free(dblk_alloc());
}


#endif /* MBDB_LOCAL_POOL */

void mblk_init(mblk_t *mp)
{
	mp->b_cont=mp->b_prev=mp->b_next=NULL;
	mp->b_rptr=mp->b_wptr=NULL;
}

mblk_t *allocb(int size, int pri)
{
	mblk_t *mp;
	dblk_t *datab;
	gchar *buf;
	
#ifdef MBDB_LOCAL_POOL
	mp=mblk_alloc(sizeof(mblk_t));
#else
	mp=g_malloc(sizeof(mblk_t));
#endif
	mblk_init(mp);
#ifdef MBDB_LOCAL_POOL
	datab=dblk_alloc();
#else
	datab=g_malloc(sizeof(dblk_t));
#endif	
	buf=g_malloc(size);

	datab->db_base=buf;
	datab->db_lim=buf+size;
	datab->ref_count=1;
	
	mp->b_datap=datab;
	mp->b_rptr=mp->b_wptr=buf;
	mp->b_next=mp->b_prev=mp->b_cont=NULL;
	return mp;
}
	
void freeb(mblk_t *mp)
{
	g_return_if_fail(mp->b_datap!=NULL);
	g_return_if_fail(mp->b_datap->db_base!=NULL);
	
	mp->b_datap->ref_count--;
	if (mp->b_datap->ref_count==0)
	{
		g_free(mp->b_datap->db_base);
#ifdef MBDB_LOCAL_POOL
		dblk_free(mp->b_datap);
#else
		g_free(mp->b_datap);
#endif
	}
#ifdef MBDB_LOCAL_POOL
	mblk_free(mp);
#else
       	g_free(mp);
#endif
}	

void freemsg(mblk_t *mp)
{
	mblk_t *tmp1,*tmp2;
	tmp1=mp;
	while(tmp1!=NULL)
	{
		tmp2=tmp1->b_cont;
		freeb(tmp1);
		tmp1=tmp2;
	}
}

mblk_t *dupb(mblk_t *mp)
{
	mblk_t *newm;
	g_return_val_if_fail(mp->b_datap!=NULL,NULL);
	g_return_val_if_fail(mp->b_datap->db_base!=NULL,NULL);
	
	mp->b_datap->ref_count++;
#ifdef MBDB_LOCAL_POOL
	newm=mblk_alloc();
#else
	newm=g_malloc(sizeof(mblk_t));
#endif
	mblk_init(newm);
	newm->b_datap=mp->b_datap;
	newm->b_rptr=mp->b_rptr;
	newm->b_wptr=mp->b_wptr;
	return newm;
}

/* duplicates a complex mblk_t */
mblk_t	*dupmsg(mblk_t* m)
{
	mblk_t *newm=NULL,*mp,*prev;
	prev=newm=dupb(m);
	m=m->b_cont;
	while (m!=NULL){
		mp=dupb(m);
		prev->b_cont=mp;
		prev=mp;
		m=m->b_cont;
	}
	return newm;
}

void putq(queue_t *q,mblk_t *mp)
{
	mblk_t *oldlast=q->q_last;
	g_return_if_fail(mp!=NULL);
	q->q_last=mp;
	mp->b_prev=oldlast;
	mp->b_next=NULL;
	if (oldlast!=NULL) oldlast->b_next=mp;
	else q->q_first=mp;
	q->q_mcount++;
}

mblk_t *getq(queue_t *q)
{
	mblk_t *oldfirst;
	oldfirst=q->q_first;
	if (oldfirst==NULL) return NULL;  /* empty queue */
	
	q->q_first=oldfirst->b_next;
	if (oldfirst->b_next!=NULL)
	{
		oldfirst->b_next->b_prev=NULL;
	}
	else
	{	/* it was the only element of the q */
		q->q_last=NULL;
	}
	oldfirst->b_prev=oldfirst->b_next=NULL;
	q->q_mcount--;
	return oldfirst;
}

/* insert mp in q just before emp */
void insq(queue_t *q,mblk_t *emp, mblk_t *mp)
{
	mblk_t *m;
	
	g_return_if_fail(mp!=NULL);
	q->q_mcount++;
	if (q->q_first==NULL) {
		q->q_first=q->q_last=mp;
	    return;
	}
	if (emp==NULL){ /* insert it at the bottom of the q */
		m=q->q_last;
		q->q_last=mp;
		mp->b_prev=m;
		m->b_next=mp;
	}
	else
	{
		m=emp->b_prev;
		mp->b_prev=m;
		mp->b_next=emp;
		emp->b_prev=mp;
		if (m!=NULL){
			m->b_next=mp;
		} else q->q_first=mp;
	}
}

/* remove and free all messages in the q */
void flushq(queue_t *q, int how)
{
	mblk_t *mp;
	
	while ((mp=getq(q))!=NULL)
	{
		freemsg(mp);
	}
}

gint msgdsize(mblk_t *mp)
{
	gint msgsize=0;
	while(mp!=NULL){
		msgsize+=mp->b_wptr-mp->b_rptr;
		mp=mp->b_cont;
	}
	return msgsize;
}

mblk_t * msgpullup(mblk_t *mp,int len)
{
	mblk_t *newm;
	gint msgsize=msgdsize(mp);
	gint rlen;
	gint mlen;
	
	
	if ((len==-1) || (len>msgsize)) len=msgsize;
	rlen=len;
	newm=allocb(len,BPRI_MED);

	while(mp!=NULL){
		mlen=mp->b_wptr-mp->b_rptr;
		if (rlen>=mlen)
		{
			memcpy(newm->b_wptr,mp->b_rptr,mlen);
			rlen-=mlen;
			newm->b_wptr+=mlen;
		}
		else /* rlen < mlen */
		{
			memcpy(newm->b_wptr,mp->b_rptr,rlen);
			newm->b_wptr+=rlen;
			
			/* put the end of the original message at the end of the new */
			newm->b_cont=dupmsg(mp);
			newm->b_cont->b_rptr+=rlen;
			return newm;
		}
		mp=mp->b_cont;
	}
	return newm;
}


mblk_t *copyb(mblk_t *mp)
{
	mblk_t *newm;
	gint len=mp->b_wptr-mp->b_rptr;
	newm=allocb(len,BPRI_MED);
	memcpy(newm->b_wptr,mp->b_rptr,len);
	newm->b_wptr+=len;
	return newm;
}

mblk_t *copymsg(mblk_t *mp)
{
	mblk_t *newm=0,*m;
	m=newm=copyb(mp);
	mp=mp->b_cont;
	while(mp!=NULL){
		m->b_cont=copyb(mp);
		m=m->b_cont;
		mp=mp->b_cont;
	}
	return newm;
}

#endif
