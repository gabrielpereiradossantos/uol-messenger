/*
  The oRTP library is an RTP (Realtime Transport Protocol - rfc1889) stack.
  Copyright (C) 2001  Simon MORLAT simon.morlat@linphone.org
  Copyright (C) 2003  Vadim Lebedev vadim@7chips.com
  Copyright (C) 2005  Wengo sas
					  Written by:ThaiHuynh Nghia nghia.th@wengo.fr

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


#ifndef RTCP_H
#define RTCP_H


#include "rtpport.h"


#define RTP_MAX_SDES 255

typedef enum {
	RTCP_SR   = 200,
	RTCP_RR   = 201,
	RTCP_SDES = 202,
	RTCP_BYE  = 203,
	RTCP_APP  = 204
} rtcp_type_t;



typedef enum {
	RTCP_SDES_END   = 0,
	RTCP_SDES_CNAME = 1,
	RTCP_SDES_NAME  = 2,
	RTCP_SDES_EMAIL = 3,
	RTCP_SDES_PHONE = 4,
	RTCP_SDES_LOC   = 5,
	RTCP_SDES_TOOL  = 6,
	RTCP_SDES_NOTE  = 7,
	RTCP_SDES_PRIV  = 8
} rtcp_sdes_type_t;


typedef struct rtcp_common_s{
#ifdef WORDS_BIGENDIAN
	guint32 version:2;   /* protocol version */
	guint32 p:1;         /* padding flag */
	guint32 count:5;     /* varies by packet type */
#else
	guint32 count:5;     /* varies by packet type */
	guint32 p:1;         /* padding flag */
	guint32 version:2;   /* protocol version */
#endif
	guint32  pt:8;        /* RTCP packet type */
	guint32 length:16;   /* pkt len in words, w/o this word */
} rtcp_common_t;

/*
 * Big-endian mask for version, padding bit and packet type pair
 * XXX?
 */
#if 0
#define RTCP_VALID_MASK (0xc000 | 0x2000 | 0xfe)
#define RTCP_VALID_VALUE ((RTP_VERSION << 14) | RTCP_SR)
#else
/* only version checked */
#define RTCP_VALID_MASK (0xc000)
#define RTCP_VALID_VALUE (RTP_VERSION << 14)
#endif
/*
 * Reception report block
 */
typedef struct {
	guint32 ssrc;             /* data source being reported */
	guint32 fraction:8;  /* fraction lost since last SR/RR */
	gint32  lost:24;              /* cumul. no. pkts lost (signed!) */
	guint32 last_seq;         /* extended last seq. no. received */
	guint32 jitter;           /* interarrival jitter */
	guint32 lsr;              /* last SR packet from this source */
	guint32 dlsr;             /* delay since last SR packet */
} rtcp_rr_t;


/*
 * SDES item
 */
typedef struct rtcp_sdes_item {
	guint8 type;              /* type of item (rtcp_sdes_type_t) */
	guint8 length;            /* length of item (in octets) */
	char data[1];             /* text, not null-terminated */
} rtcp_sdes_item_t;

/* source description (SDES) */
typedef struct rtcp_sdes {
	guint32 src;      /* first SSRC/CSRC */
	rtcp_sdes_item_t item[1]; /* list of SDES items */
} rtcp_sdes_t;

/*
 * One RTCP packet
 */
typedef struct {
	rtcp_common_t common;     /* common header */
	union {
		/* sender report (SR) */
		struct {
			guint32 ssrc;     /* sender generating this report */
			guint32 ntp_sec;  /* NTP timestamp */
			guint32 ntp_frac;
			guint32 rtp_ts;   /* RTP timestamp */
			guint32 psent;    /* packets sent */
			guint32 osent;    /* octets sent */ 
			rtcp_rr_t rr[1];  /* variable-length list */
		} sr;

		/* reception report (RR) */
		struct {
			guint32 ssrc;     /* receiver generating this report */
			rtcp_rr_t rr[1];  /* variable-length list */
		} rr;

		/* source description (SDES) */
		rtcp_sdes_t sdes;


		/* BYE */
		struct {
			guint32 src[1];   /* list of sources */
			/* can't express trailing text for reason */
		} bye;
	} r;
} rtcp_t;

#ifdef QOS_DEBUG_ENABLE
/* <JT> */
/**
 * I use R-factor to evaluate Quality of Voice.At this moment, it only supportes G.711
 * R = anpha - beta1*d - beta2*(d-beta3)*H(d-beta3) - teta1 - teta2*ln(1+teta3*e)
 * To know how to find params, see article :" Voice over IP performance monitoring "
 */
struct r_factor_params{
	double anpha;
	double beta1;
	double beta2;
	double beta3;
	double teta1;
	double teta2;
	double teta3;
};

/* to calculate R-factor */
double rtcp_rfactor(const double d, const double e);
/* calculate MOS from R-facotr */
double rf_to_mos (const double rf);

/* Begin of declaration for linked list */
struct rtcp_llnode {
	struct rtcp_llnode* next;
	void* data;
};

struct rtcp_ll{
	struct rtcp_llnode* headOfll;
	struct rtcp_llnode* endOfll;
};

struct rtcp_ll* new_rtcp_llist(struct rtcp_ll* tmpll);
int Push_To_Rtcpll(struct rtcp_ll** llist_ref,void* data);
void* Pop_From_Rtcpll (struct rtcp_ll** llist_ref);
int Delete_rtcp_llist(struct rtcp_ll* llist_ref);
/* End of declaration for linked list */

typedef struct _rtcp_report_params{
 double Fraction;
 guint32 Jitter;
 double  Rtt;
 double  Ejb;
 double R_factor;
 double Mos_factor;
} rtcp_report_params;

GMutex* rtcp_mutex;
int QoS_Enable_Rtcp_Report = 0;
struct rtcp_ll* rtcp_ll_rr;		/* Initiated by new_rtcp_llist */
struct rtcp_ll* rtcp_ll_sr;

rtcp_report_params* new_rtcp_report_params(double myfl,guint32 myJitter,double myRtt,double myEjb,double myR_factor,double myMos_factor);
void Save_Rr_To_Llist (const rtcp_t* report,const double myrtt,const double myEjb,const double myFraction,int RrOrSr);

/** 
 *the type of callback function from rtcp will be used in phapi to save rtcp params
 *@param myrtt: RTT from jt_rtcp_rtt
 *@param myEjb: lost probability from jt_rtcp_e_jitter_buffer
 *@param status: O for llist_rr and 1 for llist_sr
 */
typedef struct jt_rtcpCallbacks
{
//void (*_rtcp_reportcb) (const rtcp_t* report,const double myrtt,const double myEjb,const double myFraction);
void (*_rtcp_reportcb) (const double myFraction,guint32 myJitter,const double myrtt,const double myEjb,double myR_factor,double MyMos_factor,int status);

} jt_rtcpCallbacks_t;

extern  jt_rtcpCallbacks_t   *jt_rtcp_reportcb;

/**
 *	the purpose is to get all data stocked in rtcp_ll_rr and rtcp_ll_sr
 */
int rtcp_get_all_data_from_report(struct rtcp_ll* myRtcp_ll_sr,struct rtcp_ll* myRtcp_ll_rr);

/* Declaration of functions is given to phAPI */
void ortcp_enable_rtcp_report(int ToF);
void ortcp_report_set_cb(jt_rtcpCallbacks_t *cbk);
int ortcp_report_begin();
int ortcp_report_end();

//guint32  jt_ta;  /* time A when the report is received */
double jt_rtcp_rtt = 0;    /* Round trip time of RTCP = tA - LSR - DLSR */
double jt_rtcp_e_jitter_buffer = 0; /* e_jitter_buffer is loss probability due to the arriving packet stream underflowing or overflowing the decoder's de_jitter_buffers */
double jt_fraction = 0; /* Cos i need calcul exactly fraction */

/* <JT/> */
#endif /* end of QOS_DEBUG_ENABLE */
#endif
