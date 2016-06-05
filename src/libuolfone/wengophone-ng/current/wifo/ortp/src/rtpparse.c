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
#include "rtpsession.h"

#define TIMESTAMP_JUMP	32000
static void init_seq(RtpSource *s, guint16 seq)
{
	s->base_seq = seq - 1;
	s->max_seq = seq;
	s->bad_seq = RTP_SEQ_MOD + 1;
	s->cycles = 0;
	s->received = 0;
	s->received_prior = 0;
	s->expected_prior = 0;
	s->statsChanged = 1;
}

static int update_seq(RtpSource *s, guint16 seq)
{
	guint16 udelta = seq - s->max_seq;
	const int MAX_DROPOUT = 3000;
	const int MAX_MISORDER = 100;
	const int MIN_SEQUENTIAL = 2;

	   /*
        * Source is not valid until MIN_SEQUENTIAL packets with
        * sequential sequence numbers have been received.
        */
	if (s->probation) {
		   /* packet is in sequence */
		if (seq == s->max_seq + 1) {
			s->probation--;
			s->max_seq = seq;
			if (s->probation == 0) {
				init_seq(s, seq);
				s->received++;
				return 1;
			}
		} else {
			s->probation = MIN_SEQUENTIAL - 1;
			s->max_seq = seq;
		}
		return 0;
	} else if (!s->received) {
		s->max_seq = seq;
		init_seq(s, seq);
		s->received++;
		return 1;
	} else  if (udelta < MAX_DROPOUT) {
		   /* in order, with permissible gap */
		if (seq < s->max_seq) {
			   /*
                * Sequence number wrapped - count another 64K cycle.
                */
			s->cycles += RTP_SEQ_MOD;
		}
		s->max_seq = seq;
	} else if (udelta <= RTP_SEQ_MOD - MAX_MISORDER) {
		   /* the sequence number made a very large jump */
		if (seq == s->bad_seq) {
			   /*
                * Two sequential packets -- assume that the other side
                * restarted without telling us so just re-sync
                * (i.e., pretend this was the first packet).
                */
			init_seq(s, seq);
		}
		else {
			s->bad_seq = (seq + 1) & (RTP_SEQ_MOD-1);
			return 0;
		}
	} else {
		   /* duplicate or reordered packet */
	}
	s->received++;
	return 1;
}

static void update_jitter_stats(RtpSource* s, guint32 current_time, guint32 packet_ts)
{

	int transit = current_time - packet_ts;
	int d = transit - s->transit;
	guint32 ad;
	int jitinc;
	
	ad = (guint32) (d > 0 ? d : -d);
	jitinc = ad - ((s->jitter + 8) >> 4);

#if 0
	printf("ct=%u pt=%u transit=%d d=%d s->transit=%d s->jitter=%u jitinc=%d newjit=%u\n",
	       current_time, packet_ts, transit, d, s->transit, s->jitter,  jitinc, s->jitter + jitinc);
#endif

	s->transit = transit;


	s->jitter += jitinc;
#if 0 /* still working */
	int transit = current_time - packet_ts;
	int d = transit - s->transit;
	s->transit = transit;
	if (d<0) d=-d;
	//s->jitter +=d-((s->jitter + 8) >> 4); //jt1
	s->jitter +=(1./16)*((double)d -s->jitter);
#endif


}	


void rtp_parse(RtpSession *session,mblk_t *mp)
{
	gint i,header_size;
	mblk_t *mdata;
	rtp_header_t *rtp;
	gint msgsize;
	struct timeval tv;

	g_return_if_fail(mp!=NULL);
#ifdef TARGET_IS_HPUXKERNEL
	if (mp->b_cont!=NULL){
		mblk_t *newm;
		/* the message is fragmented, we need to reassemble it*/
		newm=msgpullup(mp,-1);
		freemsg(mp);
		mp=newm;
	}
#endif
	
	msgsize=msgdsize(mp);
	ortp_global_stats.hw_recv+=msgsize;
	session->stats.hw_recv+=msgsize;
	ortp_global_stats.packet_recv++;
	session->stats.packet_recv++;
		
	rtp=(rtp_header_t*)mp->b_rptr;
	if (rtp->version!=2)
	{
		ortp_debug("Receiving rtp packet with version number !=2...discarded");
		session->stats.bad++;
		ortp_global_stats.bad++;
		freemsg(mp);
		return;
	}
	
	/* convert all header data from network order to host order */
	rtp->seq_number=ntohs(rtp->seq_number);
	rtp->timestamp=ntohl(rtp->timestamp);
	rtp->ssrc=ntohl(rtp->ssrc);

	gettimeofday(&tv, NULL);
	update_jitter_stats(&session->peer, rtp_session_t_to_ts(session, &tv), rtp->timestamp);

	if (!update_seq(&session->peer, rtp->seq_number))
	{
		ortp_debug("sequence number problem...discarded\n");
		session->stats.bad++;
		ortp_global_stats.bad++;
		freemsg(mp);
		return;
	}
	

	/* convert csrc if necessary */
	for (i=0;i<rtp->cc;i++)
		rtp->csrc[i]=ntohl(rtp->csrc[i]);
	if (session->peer.ssrc!=0)
	{
		/*the ssrc is set, so we must check it */
		if (session->peer.ssrc!=rtp->ssrc){
			//ortp_debug("rtp_parse: bad ssrc - %i",rtp->ssrc);
			session->peer.ssrc=rtp->ssrc;
			rtp_signal_table_emit(&session->on_ssrc_changed);
		}
	}else session->peer.ssrc=rtp->ssrc;
	if(rtp->paytype != session->rcv.cng_pt)
	  {
	    if (!(session->flags & RTP_SESSION_RECV_SYNC)){
	      /* detect timestamp important jumps in the future, to workaround stupid rtp senders */
	      if (RTP_TIMESTAMP_IS_NEWER_THAN(rtp->timestamp,session->rtp.rcv_last_ts+TIMESTAMP_JUMP)){
		ortp_debug("rtp_parse: timestamp jump ?");
		rtp_signal_table_emit2(&session->on_timestamp_jump,&rtp->timestamp);
	      }

	      else if (session->rtp.jitt_comp != 0 && RTP_TIMESTAMP_IS_STRICTLY_NEWER_THAN(session->rtp.rcv_last_ts,rtp->timestamp)){
		/* avoid very old packet to enqueued, because the user is no more supposed to get them */
		ortp_debug("rtp_parse: silently discarding very old packet (ts=%i)",rtp->timestamp);
		freemsg(mp);
		session->stats.outoftime+=msgsize;
		ortp_global_stats.outoftime+=msgsize;
		return;
	      }
	    }
	  }
	else
	  {
	    rtp_signal_table_emit2(&session->on_cng_packet,(gpointer)mp);
	    freemsg(mp);
	    return;
	  }
	/* Adapt to peer packets rate to prevent the queue becoming full: discard the packet from time to time */
	/* Let pass the telephony event packets */

	if (rtp->paytype!=session->rcv.telephone_events_pt && session->adapt_rx_rate)
	  {
	    if(session->rtp.rq->q_mcount > session->rtp_rq_threshold)
	      {
		if(++session->rtp_rq_throw_cnt > session->adapt_rx_rate)
		  {
		    freemsg(mp);
		    session->stats.thrown++;
		    ortp_global_stats.thrown++;
		    session->rtp_rq_throw_cnt = 0;
		    return;
		  }
	      }
	    else
	      {
		session->rtp_rq_throw_cnt = 0;
	      }
	  }
 	/* creates a new mblk_t to be linked with the rtp header*/
	mdata=dupb(mp);
	header_size=RTP_FIXED_HEADER_SIZE+ (4*rtp->cc);
	mp->b_wptr=mp->b_rptr+header_size;
	mdata->b_rptr+=header_size;
	/* link proto with data */
	mp->b_cont=mdata;
	/* and then add the packet to the queue */
	
	rtp_putq(session->rtp.rq,mp);
	/* make some checks: q size must not exceed RtpStream::max_rq_size */
	while (session->rtp.rq->q_mcount > session->rtp.max_rq_size)
	{
		/* remove the oldest mblk_t */
		mp=getq(session->rtp.rq);
		if (mp!=NULL)
		{
			msgsize=msgdsize(mp);
			ortp_debug("rtp_putq: Queue is full. Discarding message with ts=%i",((rtp_header_t*)mp->b_rptr)->timestamp);
			freemsg(mp);
			session->stats.discarded+=msgsize;
			ortp_global_stats.discarded+=msgsize;
		}
	}
}
