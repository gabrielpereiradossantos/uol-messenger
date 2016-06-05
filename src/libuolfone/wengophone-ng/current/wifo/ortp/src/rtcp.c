/*
  The oRTP library is an RTP (Realtime Transport Protocol - rfc1889) stack.
  Copyright (C) 2001  Simon MORLAT simon.morlat@linphone.org
  Copyright (C) 2003  Vadim Lebedev vadim@7chips.com
  Copyright (C) 2005  wengo sas
					  written by ThaiHuynh Nghia nghia.th@wengo.fr

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

#if defined (WIN32) || defined(_WIN32_WCE)
#include <winsock2.h>
#include <windows.h>
#endif

#include "ortp.h"
#include "rtpmod.h"
#include "rtcp.h"
#include "math.h"


#define USING_TUNNEL(s) ((s)->flags & RTP_SESSION_USING_TUNNELS)
#define TUNNEL_SEND(t, b, l) ((t) ? (t)->tunnel_send((t)->h_tunnel, b, l) : 0)
#define TUNNEL_RECV(t, b, l) ((t) ? (t)->tunnel_recv((t)->h_tunnel, b, l) : 0)
#define TUNNEL_CLOSE(t)  ((t) ? (t)->tunnel_close((t)->h_tunnel) : 0)

#ifdef QOS_DEBUG_ENABLE
/*<JT>*/
double rtcp_rfactor(const double d, const double e)
{
/* declaration the params */
/* common:
 * anpha = 94.2, beta1 = 0.024, beta2 = 0.11, beta3 = 177.3
 * case using codec G.729a
 * teta1 = 11, teta2 = 40, teta3 = 10
 * case using codec G.711
 * teta1 = 0, teta2 = 30, teta3 = 15
 */

/* at this moment i calculate for case G.711 */
struct r_factor_params rf;

	rf.anpha = 94.2;
	rf.beta1 = 0.024;
	rf.beta2 = 0.11;
	rf.beta3 = 177.3;
	rf.teta1 = 0;
	rf.teta2 = 30;
	rf.teta3 = 15;

	if ((d-rf.beta3)<0)	{
		return (rf.anpha - rf.beta1*d - rf.teta1 - rf.teta2*log(1+rf.teta3*e));
	}
	else	{
		return (rf.anpha - rf.beta1*d - rf.beta2*(d-rf.beta3) - rf.teta1 - rf.teta2*log(1+rf.teta3*e));
	}
#if 0
	switch (Rfactor){
	case (R>90) && ( R<100):
		printf("Best");
		break;
	case (R>80) && ( R<90):
		printf("High");
		break;
	case (R>70) && ( R<80):
		printf("Medium");
		break;
	case (R>60) && ( R<70):
		printf("Low");
		break;
	case (R>50) && ( R<60):
		printf("Poor");
		break;
	}
#endif
}

double rf_to_mos (const double rf)
{
/*
 * R<0    :   MOS = 1
 * R>100  :   MOS = 4.5
 * 0<R<100:   MOS = 1+0.035*R + R*(R-60)*(100-R)*7/1000000;
 *           R-factor, quality ratings and the associated MOS
 * |-------------|--------------------------------|------------------------|
 * |R-factor     |      Quality of voice rating   |           MOS          |
 * |-------------|--------------------------------|------------------------|
 * |90<R<100     |              Best              |      4.34  -   4.5     |
 * |80<R<90 	 |			    High              |     4.03  -   4.34     |
 * |70<R<80      |              Medium            |      3.06  -   4.03    | 
 * |60<R<70      |              Low               |      3.10  -   3.60    |
 * |50<R<60      |              Poor              |      2.58  -   3.10    |
 * |-------------|--------------------------------|------------------------|
 */
	if (rf <0) {
		return 1.0;
	}
	else {
		if (rf >100 ){
			return 4.5;
		}
		else {
			return (1+0.035*rf + rf*(rf-60)*(100-rf)*7/1000000);
		}
	}
#if 0
	switch (MOS)
	{
	case (MOS>4.34) && ( MOS<4.5):
		printf("Best");
		break;
	case (MOS>4.03) && ( MOS<4.34):
		printf("High");
		break;
	case (MOS>3.06) && ( MOS<4.03):
		printf("Medium");
		break;
	case (MOS>3.01) && ( MOS<3.06):
		printf("Low");
		break;
	case (MOS>2.58) && ( MOS<3.01):
		printf("Poor");
		break;
	}
#endif

}

/* Linked List */
struct rtcp_ll* new_rtcp_llist(struct rtcp_ll* tmpll)
{
  	tmpll = (struct rtcp_ll*) malloc(sizeof(struct rtcp_ll));
	tmpll->headOfll = (struct rtcp_llnode*) malloc(sizeof(struct rtcp_llnode));
	tmpll->endOfll =  (struct rtcp_llnode*) malloc(sizeof(struct rtcp_llnode));
	tmpll->headOfll=tmpll->endOfll = NULL; 
	
	return tmpll;
}	

int Push_To_Rtcpll(struct rtcp_ll** llist_ref,void* data)
{
	struct rtcp_llnode* newNode = (struct rtcp_llnode*)malloc(sizeof(struct rtcp_llnode));
	if (newNode == NULL){
		fprintf(stderr,"Push_To_Rtcpll: malloc() error ");
		return -1;
	}
	
	if ((*llist_ref)->headOfll == NULL ) {
		newNode->data = data;
		newNode->next = NULL;

		(*llist_ref)->headOfll = newNode;
		(*llist_ref)->endOfll = newNode;
		(*llist_ref)->headOfll->next = (*llist_ref)->endOfll;
	}
	else {
		newNode->data = data;
		newNode->next = NULL;

		(*llist_ref)->endOfll->next = newNode;
		(*llist_ref)->endOfll = newNode;
	}

	return 0;
}

void* Pop_From_Rtcpll (struct rtcp_ll** llist_ref)
{
	struct rtcp_llnode* newNode = (struct rtcp_llnode*)malloc(sizeof(struct rtcp_llnode));
	if (newNode == NULL){
		fprintf(stderr,"Pop_From_Rtcpll: malloc() error ");
		return NULL;
	}

	if ((*llist_ref)->headOfll == NULL )
		return NULL;
	else {
	newNode = (*llist_ref)->headOfll ;
	(*llist_ref)->headOfll = (*llist_ref)->headOfll->next; 

		if ((*llist_ref)->headOfll == NULL )
				(*llist_ref)->endOfll = NULL;
		
		return newNode->data;
	}

}

int Delete_rtcp_list(struct rtcp_ll* llist_ref)
{
	struct rtcp_llnode* tmp;
	
	while (llist_ref->headOfll!=NULL){
		free(llist_ref->headOfll->data);
			llist_ref->headOfll->data = NULL;

		tmp=llist_ref->headOfll->next;

		free(llist_ref->headOfll);
			llist_ref->headOfll = NULL;

		llist_ref->headOfll = tmp; 	
	}

	free(llist_ref);
	return 0;
}

/* Linked List. */

rtcp_report_params* new_rtcp_report_params(double myfl,guint32 myJitter,double myRtt,double myEjb,double myR_factor,double myMos_factor)
{
	rtcp_report_params* newReportParams;
	newReportParams = (rtcp_report_params*) malloc (sizeof(rtcp_report_params));

	if (newReportParams == NULL ){
	  printf("new_rtcp_report_params:malloc() error");
	  return NULL;
	}
    newReportParams->Fraction  = myfl;
	newReportParams->Jitter    = myJitter;
	newReportParams->Rtt	   = myRtt;
	newReportParams->Ejb       = myEjb;
	newReportParams->R_factor  = myR_factor;
	newReportParams->Mos_factor= myMos_factor;
	

	return newReportParams;
}

void ortcp_enable_rtcp_report(int ToF)
{
	if (ToF == 1)
		QoS_Enable_Rtcp_Report = 1;
	else
		QoS_Enable_Rtcp_Report = 0;
}

void Save_Rr_To_Llist (const rtcp_t* rp,const double myrtt,const double myEjb,const double myFraction,int RrOrSr)
{
	double r_factor = 0;
	double mos_factor = 0;
	rtcp_report_params* report_params = NULL;

	r_factor = rtcp_rfactor((40+60+(myrtt)/(2*1000) ),(myFraction));
	mos_factor = rf_to_mos (r_factor);
	
	report_params = new_rtcp_report_params(myFraction,(ntohl(rp->r.rr.rr->jitter)),myrtt,myEjb, r_factor, mos_factor);
	if (RrOrSr == 0)
		Push_To_Rtcpll(&rtcp_ll_rr,(void*)report_params);
	else 
		Push_To_Rtcpll(&rtcp_ll_sr,(void*)report_params);

}

int rtcp_get_all_data_from_report(struct rtcp_ll* myRtcp_ll_sr,struct rtcp_ll* myRtcp_ll_rr)
{
rtcp_report_params* tmp_params;

if (myRtcp_ll_sr->headOfll!=NULL || myRtcp_ll_rr->headOfll != NULL){

	/* get data from llist_sr */
	while (myRtcp_ll_sr->headOfll!=NULL){

				tmp_params = (rtcp_report_params*)myRtcp_ll_sr->headOfll->data; 
				jt_rtcp_reportcb->_rtcp_reportcb(tmp_params->Fraction,tmp_params->Jitter,tmp_params->Rtt,tmp_params->Ejb,tmp_params->R_factor,tmp_params->Mos_factor,1);
				myRtcp_ll_sr->headOfll = myRtcp_ll_sr->headOfll->next ; 
				
				}
	/* get data from llist_rr */	
	while (myRtcp_ll_rr->headOfll!=NULL){

				tmp_params = (rtcp_report_params*)myRtcp_ll_rr->headOfll->data; 
				jt_rtcp_reportcb->_rtcp_reportcb(tmp_params->Fraction,tmp_params->Jitter,tmp_params->Rtt,tmp_params->Ejb,tmp_params->R_factor,tmp_params->Mos_factor,0);
				myRtcp_ll_rr->headOfll = myRtcp_ll_rr->headOfll->next ; 
				}	
}
return 0;
}

jt_rtcpCallbacks_t   *jt_rtcp_reportcb;
void ortcp_report_set_cb(jt_rtcpCallbacks_t *cbk)
{
jt_rtcp_reportcb = cbk;
}

int ortcp_report_begin()
{
//rtcp_mutex = g_mutex_new ();
rtcp_ll_rr =  new_rtcp_llist(rtcp_ll_rr);
rtcp_ll_sr =  new_rtcp_llist(rtcp_ll_sr);	

return 0;
}
int ortcp_report_end()
{
rtcp_get_all_data_from_report(rtcp_ll_sr,rtcp_ll_rr);

Delete_rtcp_list(rtcp_ll_rr);
Delete_rtcp_list(rtcp_ll_sr);
//g_mutex_free(rtcp_mutex);

return 0;
}
/*<JT/>*/
#endif /* end of  QOS_DEBUG_ENABLE */
#ifdef RTCP

static char *write_sdes(char *b, guint32 src, int argc,
					  rtcp_sdes_type_t type[], char *value[],
					  int length[])
{
	rtcp_sdes_t *s = (rtcp_sdes_t *)b;
	rtcp_sdes_item_t *rsp;
	int i;
	int len;
	int pad;

	   /* SSRC header */
	s->src = htonl(src);
	rsp = &s->item[0];

	   /* SDES items */
	for (i = 0; i < argc; i++)
	{
		rsp->type = type[i];
		len = length[i];
		if (len > RTP_MAX_SDES)
		{
			   /* invalid length, may want to take other action */
			len = RTP_MAX_SDES;
		}
		rsp->length = len;
		memcpy(rsp->data, value[i], len);
		rsp = (rtcp_sdes_item_t *)&rsp->data[len];
	}

	   /* terminate with end marker and pad to next 4-octet boundary */
	len = ((char *) rsp) - b;
	pad = 4 - (len & 0x3);
	b = (char *) rsp;
	while (pad--)
		*b++ = RTCP_SDES_END;

	return b;
}


static guint32 tv_to_ntp32(struct timeval *tv)
{
	/*
	 * 1) tvusec/1000000 =  nttpval/2^31
	 * 2) ntpval = 2^31*tvusec/1000000
	 * 3) ntpval = 2^31/1000000 * tvusec
	 * 4) ntpval32 = ntpval/2^16
	 */

	return (tv->tv_sec << 16) | ((((2u << 30 ) / 500000) * tv->tv_usec) >> 16);
	
}

static guint32 tv_to_ntp_frac(struct timeval *tv)
{
	/*
	 * 1) tvusec/1000000 =  nttpval/2^31
	 * 2) ntpval = 2^31*tvusec/1000000
	 * 3) ntpval = 2^31/1000000 * tvusec
	 */

	return  (((2u<<30)/500000) * tv->tv_usec);

}

static void tvsub(register struct timeval *out, register struct timeval *in)
{
	out->tv_usec -= in->tv_usec;

	while(out->tv_usec < 0) {
		--out->tv_sec;
		out->tv_usec += 1000000;
	}

	out->tv_sec -= in->tv_sec;
}


static make_rr(RtpSource* s,  rtcp_rr_t* rr, struct timeval *ctv)
{
	guint32 extended_max = s->cycles + s->max_seq;
	guint32  expected = extended_max - s->base_seq;
	int lost = expected - s->received;
	int  expected_interval = expected - s->expected_prior;
	int  received_interval;
	int lost_interval;
	int fraction;
	guint32 *p;
	
	
	s->expected_prior = expected;
	received_interval = s->received - s->received_prior;
	s->received_prior = s->received;

	lost_interval = expected_interval - received_interval;


	if (expected_interval == 0 || lost_interval <= 0)
	{
		fraction = 0;
		#ifdef QOS_DEBUG_ENABLE
	    jt_fraction = 0; /* jt_ use this param to calcul R-factor */
		#endif /* end of QOS_DEBUG_ENABLE */
	}
	else
	{   fraction = (lost_interval << 8) / expected_interval;
		#ifdef QOS_DEBUG_ENABLE
	    jt_fraction = (double)lost_interval/(double)expected_interval; /* jt_ use this param to calcul R-factor */
		#endif /* end of  QOS_DEBUG_ENABLE */
	}

	printf("xmax = %d, Jitter = %d /16 = %d  frac=%d  lost=%d lsr=%d\n",  extended_max, s->jitter, s->jitter >> 4, fraction, lost, s->lastsrstamp );

	rr->ssrc = htonl(s->ssrc);
	rr->fraction = fraction;
//	lost = (ntohl(lost))>>8;              /* cumul. no. pkts lost (signed!) */   /* cumul. no. pkts lost (signed!) */ /* JT I think we have to use htonl */
	lost = (htonl(lost))>>8;              /* cumul. no. pkts lost (signed!) */
	rr->lost = lost;              /* cumul. no. pkts lost (signed!) */

	p = (guint32* ) &rr;


	rr->last_seq = htonl(extended_max);      /* extended last seq. no. received */
	rr->jitter = s->jitter >> 4;           /* interarrival jitter */
	rr->jitter = htonl(rr->jitter);
	
	rr->lsr = s->lastsrstamp;
	
	if (rr->lsr) {
		struct timeval now;
		
		rr->lsr = htonl(rr->lsr);
		now = *ctv;
		tvsub(&now, &s->lastsrtime);
		rr->dlsr = tv_to_ntp32(&now);           /* delay since last SR packet */
		rr->dlsr = htonl(rr->dlsr);
	}
	else {
		rr->dlsr = 0;
	}
#if 0	
	if (fraction < gLossLimit)
	{
		m_cbk->goodQuality();
	}
	else if (fraction < gLossLimit2)
	{
		m_cbk->mediumQuality();
	}
	else if (fraction > 20*256/100)
	{
		m_cbk->awfulQuality();
	}
	else
		m_cbk->badQuality();

	return fraction;
#endif
	
}

static gint
   rtcp_send (RtpSession * session, mblk_t * m)
{
	gint error;

//	g_message("rctp_send: calling sendto\n");
	if (USING_TUNNEL(session))
	  {
	    error = TUNNEL_SEND(session->rtcp.tunnel, m->b_rptr,
					(m->b_wptr - m->b_rptr));
	    if (error < 0)
	      session->terminated = 1;

	    return;
	  }

	    
	error = sendto (session->rtcp.socket, m->b_rptr,
					(m->b_wptr - m->b_rptr), 0,
					(struct sockaddr *) &session->rtcp.rem_addr,
					sizeof (struct sockaddr_in));
	if (error < 0)
		g_warning ("Error sending rtcp packet: %s.", getSocketError());

	return error;
}

static void make_sr(RtpSession* session, rtcp_t *r)
{

//	g_message("getting time of day\n");
//	gettimeofday(&session->local.lastsrtime, NULL);
	r->r.sr.ntp_sec = htonl(session->local.lastsrtime.tv_sec);

//	g_message("calling tv_to_ntp_frac\n");
	r->r.sr.ntp_frac = tv_to_ntp_frac(&session->local.lastsrtime);
	r->r.sr.ntp_frac = htonl(r->r.sr.ntp_frac);

//	g_message("calling rtp_session_t_to_ts_frac\n");
	r->r.sr.rtp_ts = rtp_session_t_to_ts(session, &session->local.lastsrtime);
	r->r.sr.rtp_ts = htonl(r->r.sr.rtp_ts);
	r->r.sr.ssrc = htonl(session->local.ssrc);



	r->r.sr.psent = (guint32) (session->stats.packet_sent);
	r->r.sr.osent = (guint32) (session->stats.sent);
	r->r.sr.psent = htonl(r->r.sr.psent);
	r->r.sr.osent = htonl(r->r.sr.osent);

	if (session->rtcp_sr_sent_cb) {
		session->rtcp_sr_sent_cb(session, r);
	}
}	

mblk_t *rtp_session_alloc_recv_buffer(RtpSession* session);
#define RTCP_BYE_HEADER_SIZE 8 
void rtcp_send_bye_packet(RtpSession *session, gchar *reason)
{	
	mblk_t *mp;
	rtcp_t * r;
	unsigned char rlen=0;
	int i;
	g_message("Sending BYE\n");

#ifdef MBDB_LOCAL_POOL

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
	   mp = rtp_session_alloc_recv_buffer(session);
	//g_message("got mp=%p\n", mp);
	r = (rtcp_t *)mp->b_rptr;

	r->common.version = 2;
	r->common.length = 1;     /* no reason yet */
	if(reason){
	  rlen = strlen(reason);
	  if(rlen)
	    r->common.length += (rlen+1)/sizeof(guint32); /* (reason length + length byte)/4 */
	}
	r->common.length = htons(r->common.length);
	r->common.pt = RTCP_BYE;
	r->common.count = 1;
	r->r.bye.src[0] = htonl(session->local.ssrc);
	if(rlen){
	  char *p = (char *)&r->r.bye.src[1];
	  *p++ =  rlen;
	  strcpy(p, reason);
	}
	mp->b_wptr += RTCP_BYE_HEADER_SIZE;
	if(rlen)
	  mp->b_wptr += rlen+1;

	rtcp_send(session, mp);
	//g_message("freeng buffer\n");
	freeb(mp);
}

void rtcp_send_report(RtpSession *session)
{
	mblk_t *mp;
	rtcp_t * r;
	int i;

	g_message("Sending SR\n");

#ifdef MBDB_LOCAL_POOL

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
	     mp = rtp_session_alloc_recv_buffer(session);

	
	g_message("got mp=%p\n", mp);


	r = (rtcp_t *)mp->b_rptr;
	

	// Make sender report
	r->common.version = 2;
	r->common.p = 0;

       gettimeofday(&session->local.lastsrtime, NULL);

	if (session->stats.packet_sent)
	{
		
		r->common.length = (((char*)&r->r.sr.rr[1]) - mp->b_rptr) / sizeof(guint32) - 1;
		r->common.length = htons(r->common.length);

		r->common.pt = RTCP_SR;
		r->common.count = 1;


		make_sr(session, r);

	g_message("Building RR\n");
#ifndef MULTI_SSRC
		make_rr(&session->peer, &r->r.sr.rr[0], &session->local.lastsrtime);
#else
		if(session->flags & RTP_SESSION_MULTI_PEER)
		  make_rr(session->peers.q_first, &r->r.sr.rr[0], &session->local.lastsrtime);
		else
		  make_rr(&session->peer, &r->r.sr.rr[0], &session->local.lastsrtime);
#endif
		/* JT */
		#ifdef QOS_DEBUG_ENABLE
		//g_mutex_lock(rtcp_mutex); /* not ckeck yet */
		if (QoS_Enable_Rtcp_Report !=0)
			Save_Rr_To_Llist (r,jt_rtcp_rtt,jt_rtcp_e_jitter_buffer,jt_fraction,1);
		//g_mutex_unlock(rtcp_mutex);
		#endif /* end of  QOS_DEBUG_ENABLE */
		/* JT. */

		r = (rtcp_t*) &r->r.sr.rr[1];
	}
	else
	{
		r->common.length = (((char*)&r->r.rr.rr[1]) - mp->b_rptr) / sizeof(guint32) - 1;
		r->common.length = htons(r->common.length);

		r->common.pt = RTCP_RR;
		r->common.count = 1;
		r->r.rr.ssrc = htonl(session->local.ssrc);

		make_rr(&session->peer, &r->r.rr.rr[0], &session->local.lastsrtime);
		
		/* JT */
		#ifdef QOS_DEBUG_ENABLE
		//g_mutex_lock(rtcp_mutex);
		if (QoS_Enable_Rtcp_Report !=0)
			Save_Rr_To_Llist (r,jt_rtcp_rtt,jt_rtcp_e_jitter_buffer,jt_fraction,0);
		//g_mutex_unlock(rtcp_mutex);
		#endif /* end of QOS_DEBUG_ENABLE */
		/* JT. */
		
		r = &r->r.rr.rr[1];

	}
	/*
	 * write an SDES CNAME iitem
	 *
	 */
	{
		char *val[1];
		int   len = strlen(session->local.cname) + 1;
		rtcp_sdes_type_t   type = RTCP_SDES_CNAME;

		val[0] =  session->local.cname;
	

		g_message("Builing SDES\n");
		mp->b_wptr = write_sdes((char*) &r->r.sdes, session->local.ssrc, 1, &type, val, &len);

		r->common.version = RTP_VERSION;
		r->common.count = 1;
		r->common.p = 0;
		r->common.pt = RTCP_SDES;

		r->common.length = (mp->b_wptr - (char*)r)/sizeof(guint32) - 1;
		r->common.length = htons(r->common.length);
	}
   
	rtcp_send(session, mp);
	if (session->rtcp_rr_sent_cb) {
		session->rtcp_rr_sent_cb(session, r);
	}

	g_message("freeng buffer\n");
	freeb(mp);

};	


static void rtcp_parse_sdes_item(RtpSource* s, int type, char* data, int len)
{
	static const char end[] = "END";
	static const char cname[] = "CNAME";
	static const char name[] = "NAME";
	static const char email[] = "EMAIL";
	static const char phone[] = "PHONE";
	static const char loc[] = "LOC";
	static const char tool[] = "TOOL";
	static const char note[] = "NOTE";
	static const char priv[] = "PRIV";
	static const char * const sdes[] =
	{
		end, cname,  name,  email,  phone,  loc,
		tool, note,  priv
	};						

	// store new SDES info for that source
	if (RTCP_SDES_END <= type && type <= RTCP_SDES_PRIV)
	{

		const char* nm = sdes[type];
		char* val;

		if (type == RTCP_SDES_END || type == RTCP_SDES_PRIV)
		{
			val = "";
		}
		else
		{
			val = data;
		}
#ifdef MULTI_SSRC
		rtp_update_peer_sdes(s, type, data, len);
#endif

	}
	else
	{
		/*
		 * unknown sdes item
		 */
	}

	return;
}



static char* rtcp_parse_sdes(RtpSession *session, rtcp_t *r)
{
#ifdef MULTI_SSRC
  asking_ssrc = session->flags & RTP_SESSION_ACCEPT_ALL;
#endif
	int count = r->common.count;
	rtcp_sdes_t *sd = &r->r.sdes;
	rtcp_sdes_item_t *rsp, *rspn;
	rtcp_sdes_item_t *end = (rtcp_sdes_item_t *)
							((guint32 *)r + ntohs(r->common.length) + 1);
	RtpSource *s;

#ifdef MULTI_SSRC
	s = rtp_session_find_peer(sd->src);
	if(s == NULL)
	  {
	    rtp_signal_emit2(&session->on_new_ssrc, &asking_ssrc);
	    if(session->flags & RTP_SESSION_ACCEPT_ALL || asking_ssrc){
	      /* accept new peer */
	      s = rtp_source_new(session);
	      rtp_source_copy_addr_from_session(session, s);
	    }
	  }
#endif

	while (--count >= 0)
	{
		rsp = &sd->item[0];
		if (rsp >= end)
			break;

		for (; rsp->type; rsp = rspn )
		{
			rspn = (rtcp_sdes_item_t *)((char*)rsp+rsp->length+2);
			if (rspn >= end)
			{
				rsp = rspn;
				break;
			}
#ifdef MULTI_SSRC
			if(asking_ssrc)
			  rtp_update_peer_sdes(s, rsp->type, rsp->data, rsp->length);
#else
			rtcp_parse_sdes_item(s, rsp->type, rsp->data, rsp->length);
#endif
		}
		sd = (rtcp_sdes_t *)
			 ((guint32 *)sd + (((char *)rsp - (char *)sd) >> 2)+1);
	}

	if (count >= 0)
	{
		   /* invalid packet format */
	}
	return (char*) end;
}


void rtcp_parse_rr(RtpSession *session, rtcp_t* r)
{
	if (!session->peer.ssrc)
	{
		session->peer.ssrc = ntohl(r->r.rr.ssrc);
		g_message("got new rtcp peer: %u\n", session->peer.ssrc);
	}
	else if (session->peer.ssrc != ntohl(r->r.rr.ssrc))
	{
		g_message("got BAD rtcp peer: %u %u\n", session->peer.ssrc, ntohl(r->r.sr.ssrc));
		return;
	}
	gettimeofday(&session->peer.lastsrtime, NULL);
	r->r.sr.ntp_sec = ntohl(r->r.sr.ntp_sec);
	r->r.sr.ntp_frac = ntohl(r->r.sr.ntp_frac);
	session->peer.lastsrstamp = (r->r.sr.ntp_sec << 16) | ( r->r.sr.ntp_frac >> 16);

	if (session->rtcp_rr_received_cb) {
		session->rtcp_rr_received_cb(session, r);
	}
}

void rtcp_parse_bye(RtpSession *session, rtcp_t* r)
{
  int len = ntohs(r->common.length);
#ifdef MULTI_SSRC
    RtpSource *s = rtp_session_find_peer(session, r->r.bye.src[0]);
    if(s){
      rtp_signal_table_emit2 (&session->on_bye_packet, s);
    }
#else
    if(len > 1)
      rtp_signal_table_emit2 (&session->on_bye_packet, &r->r.bye.src[1]);
    else
      rtp_signal_table_emit (&session->on_bye_packet);
#endif
}


void rtcp_parse_sr(RtpSession* session, rtcp_t* r)
{
	if (!session->peer.ssrc)
	{
		session->peer.ssrc = ntohl(r->r.sr.ssrc);
		g_message("got new rtcp peer: %u\n", session->peer.ssrc);
	}
	else if (session->peer.ssrc != ntohl(r->r.sr.ssrc))
	{
		g_message("got BAD rtcp peer: %u %u\n", session->peer.ssrc, ntohl(r->r.sr.ssrc));
		return;
	}

	gettimeofday(&session->peer.lastsrtime, NULL);
	r->r.sr.ntp_sec = ntohl(r->r.sr.ntp_sec);
	r->r.sr.ntp_frac = ntohl(r->r.sr.ntp_frac);
	session->peer.lastsrstamp = (r->r.sr.ntp_sec << 16) | ( r->r.sr.ntp_frac >> 16);
}






void rtcp_parse(RtpSession* session, char* pkt,  int size)
{
	guint32 len = size/sizeof(guint32);        /* length of compound RTCP packet in words */
	rtcp_t *r = (rtcp_t*) pkt;          /* RTCP header */
	rtcp_t *end;        /* end of compound RTCP packet */

	/* JT: still working */
	#ifdef QOS_DEBUG_ENABLE
	struct timeval rcv_time_tv;
	guint32 rcv_time_msw;
	guint32 rcv_time_lsw;
	guint32 rcv_time;
	double sum_sr =0;
	double sum_rr =0;
	long   nb_sr  =0;
    long   nb_rr  =0;
	jt_rtcp_rtt   =0;
		
	gettimeofday(&rcv_time_tv,NULL);
	rcv_time_msw = rcv_time_tv.tv_sec;
	rcv_time_lsw = (guint32) ((double)rcv_time_tv.tv_usec*(double)(1LL<<32)*1.0e-6);
    rcv_time = (rcv_time_msw<<16) | (rcv_time_lsw >> 16);

	#endif /* end of  QOS_DEBUG_ENABLE */
	//jt_rtcp_e_jitter_buffer
    /* JT. */

	if ((ntohs(*(guint16 *)r) & RTCP_VALID_MASK) != RTCP_VALID_VALUE)
	{
		/* something wrong with packet format */
		g_message("got bad RTCP packet %04x\n", ntohs(*(guint16 *)r));
		return;
	}

	end = (rtcp_t *)((guint32 *)r + len);

	do 
	{
		switch(r->common.pt)
		{
			case RTCP_SDES:
				rtcp_parse_sdes(session, r); 
				break;
			case RTCP_SR:
				rtcp_parse_sr(session, r);
				/* JT */
				#ifdef QOS_DEBUG_ENABLE
				//jt_rtcp_rtt = (double) (rcv_time -r->r.sr.rr->dlsr - r->r.sr.rr->lsr);
				r->r.sr.rr->dlsr = ntohl(r->r.sr.rr->dlsr);
				r->r.sr.rr->lsr  = ntohl(r->r.sr.rr->lsr);
				jt_rtcp_rtt = (double) (rcv_time -  r->r.sr.rr->dlsr  - r->r.sr.rr->lsr);
				jt_rtcp_rtt = jt_rtcp_rtt/65536;	 
				sum_sr = sum_sr + jt_rtcp_rtt;
				nb_sr = nb_sr+1;
				#endif /* end of  QOS_DEBUG_ENABLE */
				/* JT. */
				break;
			case RTCP_RR:
				rtcp_parse_rr(session, r);
				/* JT */
				#ifdef QOS_DEBUG_ENABLE
                r->r.rr.rr->dlsr = ntohl(r->r.rr.rr->dlsr);
				r->r.rr.rr->lsr  = ntohl(r->r.rr.rr->lsr);
				jt_rtcp_rtt = (double) (rcv_time - r->r.rr.rr->dlsr - r->r.rr.rr->lsr);
				jt_rtcp_rtt = jt_rtcp_rtt/65536;
				sum_rr=sum_rr + jt_rtcp_rtt;
				nb_rr = nb_rr + 1;
				#endif /* end of QOS_DEBUG_ENABLE */
				/* JT. */
				break;
			case RTCP_BYE:
				rtcp_parse_bye(session, r);
				break;
			default:
			  //printf("RTP: Invalid RTCP Payload: %d\n", r->common.pt);
				break;
		}

	//	session->rtcp.stats.incount++;   

		r = (rtcp_t *)((guint32 *)r + ntohs(r->common.length) + 1);

	} while (r < end && r->common.version == 2);
	/* JT  Here i think that unit of RTT is ms? What do you think */
	#ifdef QOS_DEBUG_ENABLE
	if (sum_sr !=0 && nb_sr !=0)
		jt_rtcp_rtt = sum_sr/(double)nb_sr;   
	else if(sum_rr !=0 && nb_rr !=0)
		jt_rtcp_rtt = sum_rr/(double)nb_rr; 
	else
		jt_rtcp_rtt = 0;               
	#endif /* end of QOS_DEBUG_ENABLE */
    /* JT. */

}

#if 0

static chunk_item_t *chunk_item_new()
{    
	chunk_item_t *chunk=g_new0(chunk_item_t, 1);    
	chunk->sdes_items=g_byte_array_new();    
	return chunk;
}

static void chunk_item_free(chunk_item_t *chunk)
{    
	g_byte_array_free(chunk->sdes_items, TRUE);    
	g_free(chunk);
}

static void rtcp_add_sdes_item(chunk_item_t *chunk, rtcp_sdes_type_t sdes_type, gchar *content)
{	
	if ( content )
	{
		guint8 stype = sdes_type;
		guint8 content_len = g_utf8_strlen(content, RTCP_SDES_MAX_STRING_SIZE);
		g_byte_array_append(chunk->sdes_items, &stype, 1);
		g_byte_array_append(chunk->sdes_items, &content_len, 1);
		g_byte_array_append(chunk->sdes_items, (guint8*)content, content_len);
	}
}

static guint rtcp_calculate_sdes_padding(guint chunk_size)
{
        chunk_size = chunk_size%4;
        /* if  no rest,  return 4 */
        if (chunk_size == 0)
        {
            chunk_size = 4;
        }
        return chunk_size;
}

static void rtcp_add_sdes_padding(chunk_item_t *chunk)
{
	guint8 pad[] = {0,0,0,0};
	guint8 pad_size = rtcp_calculate_sdes_padding(chunk->sdes_items->len);
    g_byte_array_append(chunk->sdes_items, pad, pad_size);
}


void
rtp_session_add_contributing_source(RtpSession *session, guint32 csrc, 
    gchar *cname, gchar *name, gchar *email, gchar *phone, 
    gchar *loc, gchar *tool, gchar *note)
{
	chunk_item_t *chunk = chunk_item_new(); 
	
	if (!cname  || !chunk)
	{
		g_error("Error");
	}
	chunk->csrc = csrc;
	rtcp_add_sdes_item(chunk, RTCP_SDES_CNAME, cname);
	rtcp_add_sdes_item(chunk, RTCP_SDES_NAME, name);
	rtcp_add_sdes_item(chunk, RTCP_SDES_EMAIL, email);
	rtcp_add_sdes_item(chunk, RTCP_SDES_PHONE, phone);
	rtcp_add_sdes_item(chunk, RTCP_SDES_LOC, loc);
	rtcp_add_sdes_item(chunk, RTCP_SDES_TOOL, tool);
	rtcp_add_sdes_item(chunk, RTCP_SDES_NOTE, note);
	rtcp_add_sdes_padding(chunk);
	
	g_list_append(session->contributing_sources, chunk);
}

static void rtcp_calculate_sdes_size(gpointer chunk, gpointer size)
{
        chunk_item_t* c = chunk;
        guint16 *s = size;
        *s += (c->sdes_items->len + sizeof(c->csrc));
}

static void rtcp_concatenate_sdes_item(gpointer chunk, gpointer mp)
{
        chunk_item_t *c = chunk;
        mblk_t *m = mp;
        memcpy(m->b_wptr, &c->csrc, sizeof(c->csrc));
        m->b_wptr += sizeof(c->csrc);
        memcpy(m->b_wptr, c->sdes_items->data, c->sdes_items->len);
        m->b_wptr += c->sdes_items->len;
}

mblk_t* rtp_session_create_rtcp_sdes_packet(RtpSession *session)
{
    mblk_t *mp;
	rtcp_common_header_t *rtcp;
    guint16 sdes_size = 0;
        
    g_list_foreach(session->contributing_sources, rtcp_calculate_sdes_size, &sdes_size);
	
	sdes_size += RTCP_COMMON_HEADER_SIZE;
	mp = allocb(sdes_size, 0);
    rtcp = (rtcp_common_header_t*)mp->b_rptr;
    rtcp->version = 2;
    rtcp->padbit = 0;
    rtcp->packet_type = RTCP_SDES;
    /*maybe need a cast to guint16 FIXME*/
	/* As in rfc1889 length is in 32-bit words minus 1*/
    rtcp->length = (sdes_size/4)-1;
    /*FIXME need to add rc
	rtcp->rc=*/
    mp->b_wptr += RTCP_COMMON_HEADER_SIZE;
    
    g_list_foreach(session->contributing_sources, rtcp_concatenate_sdes_item, mp);
	mp->b_wptr += sdes_size;
    return mp;
}
static gint cmp_ssrc (gconstpointer a, gconstpointer b)
{
	const chunk_item_t *const c = a;
	const guint *const s = b;
	return (c->csrc - *s);
}       
static mblk_t *rtcp_create_bye_packet(guint ssrc, gchar *reason)
{	
    guint packet_size = sizeof(rtcp_common_header_t);
	mblk_t *mp = allocb(packet_size, 0);
    rtcp_common_header_t *rtcp = (rtcp_common_header_t*)mp->b_rptr;

    rtcp->version = 2;
    rtcp->padbit = 0;
    rtcp->packet_type = RTCP_BYE;
    /*maybe need a cast to guint16 FIXME*/
    rtcp->length = 1;
	rtcp->rc = 1;
	rtcp->ssrc = ssrc;
    mp->b_wptr += packet_size;
	return mp;
}

mblk_t *rtp_session_remove_contributing_sources(RtpSession *session, guint32 ssrc)
{
	GList *deleting = g_list_find_custom(session->contributing_sources, &ssrc, cmp_ssrc);
	
	chunk_item_free((chunk_item_t*)deleting->data);
    session->contributing_sources = g_list_delete_link(session->contributing_sources, deleting);        
	return rtcp_create_bye_packet(ssrc, NULL);
}

#endif

#endif
