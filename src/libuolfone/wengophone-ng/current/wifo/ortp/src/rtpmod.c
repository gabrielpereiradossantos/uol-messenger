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




#include "rtp.h"

#include "rtpmod.h"


/* put an rtp packet in queue. It is called by rtp_parse()*/
void rtp_putq(queue_t *q, mblk_t *mp)
{
	mblk_t *tmp;
	rtp_header_t *rtp=(rtp_header_t*)mp->b_rptr,*tmprtp;
	/* insert message block by increasing time stamp order : the last (at the bottom)
		message of the queue is the newest*/
	ortp_debug("rtp_putq(): Enqueuing packet with ts=%i",rtp->timestamp);
	tmp=q->q_last;
	
	if (tmp==NULL) {
		putq(q,mp);
		return;
	}
	/* we look at the queue from bottom to top, because enqueued packets have a better chance
	to be enqueued at the bottom, since there are surely newer */
	while (tmp!=NULL)
	{
		tmprtp=(rtp_header_t*)tmp->b_rptr;
		ortp_debug("rtp_putq(): Seeing packet with ts=%i",tmprtp->timestamp);
		
 		if (rtp->timestamp == tmprtp->timestamp)
 		{
 			if (rtp->seq_number== tmprtp->seq_number)
 			{
 				/* this is a duplicated packet. Don't queue it */
 				ortp_debug("rtp_putq: duplicated message.");
 				freemsg(mp);
 				return;
 			}
			// Check seq_number to put it in the right order
			else if (rtp->seq_number > tmprtp->seq_number)
			{
				/* This is the right place for it, enqueue it! */
				insq(q,tmp->b_next,mp);
				return;
			}
 		}
		else if RTP_TIMESTAMP_IS_STRICTLY_NEWER_THAN(rtp->timestamp,tmprtp->timestamp)
		{
			/* insert mp just before tmp */
			insq(q,tmp->b_next,mp);
			ortp_debug("mp=%x inserted before %x. q->q_first=%i, q->q_last=%i",
						mp,
						tmp,
						((rtp_header_t*)q->q_first->b_rptr)->timestamp,
						((rtp_header_t*)q->q_last->b_rptr)->timestamp);
			return;
		}
		tmp=tmp->b_prev;
	}
	/* this packet is the oldest, it has to be 
	placed on top of the queue */
	insq(q,q->q_first,mp);
	
}



mblk_t *rtp_getq(queue_t *q,guint32 timestamp)
{
	mblk_t *tmp,*ret=NULL;
	rtp_header_t *tmprtp;
	guint32 oldest;

	ortp_debug("rtp_getq(): Timestamp %i wanted.",timestamp);

	if (q->q_last==NULL)
	{
		//ortp_debug("rtp_getq: q is empty.");
		return NULL;
	}
	/* prevent somebody to ask for a timestamp that is older than the oldest of the queue */
	oldest=((rtp_header_t*) q->q_first->b_rptr)->timestamp;
	if (RTP_TIMESTAMP_IS_STRICTLY_NEWER_THAN(oldest,timestamp))
	{
		ortp_debug("rtp_getq(): asking for too old packet ! oldest=%i",oldest);
		return NULL;
	}
	/* compose a multi data block mblk_t that represents all data that is oldest than timestamp, excepted
	the first data mblk_t of the chain that can be  == timestamp.*/

	while ((tmp=q->q_first)!=NULL)
	{
		tmp=q->q_first;
		tmprtp=(rtp_header_t*)tmp->b_rptr;
		ortp_debug("rtp_getq: Seeing packet with ts=%i",tmprtp->timestamp);
		if ( RTP_TIMESTAMP_IS_NEWER_THAN(timestamp,tmprtp->timestamp)  )
		{
			tmp=getq(q); /* dequeue the packet, since it has an interesting timestamp*/
			ortp_debug("rtp_getq: Found packet with ts=%i",tmprtp->timestamp);
			return(tmp);
			
#if 0
			/* make a big mblk_t with all unread rtp data linked together*/
			if (ret==NULL) {
				ret=tmp; /* tmp is proto , tmp->b_cont is data...*/
				end_data=ret->b_cont; /* the M_DATA */
				if (end_data==NULL){
					ortp_debug("rtp_getq:end_data=NULL !");
					return NULL;
				}
				while(end_data->b_cont!=NULL) end_data=end_data->b_cont;
			}
			else{
				/* link the M_DATA of tmp to the end of the M_DATA of rest*/
				end_data->b_cont=tmp->b_cont;
				while(end_data->b_cont!=NULL) end_data=end_data->b_cont;
				freeb(tmp);
			}
#endif
		}
		else
		{
			/* no more interesting packets, so return */
			return(ret);
		}
	}
	return ret;
}
