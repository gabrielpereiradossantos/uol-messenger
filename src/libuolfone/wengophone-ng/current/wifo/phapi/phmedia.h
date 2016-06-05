/*
 * phmedia -  Phone Api media streamer
 *
 * Copyright (C) 2005-2006 WENGO SAS
 * Copyright (C) 2004 Vadim Lebedev <vadim@mbdsys.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef __PHMEDIA_H__
#define __PHMEDIA_H__

#include <rtpport.h> // only for GMutex


#define PH_MEDIA_DTMF_PAYLOAD 101
#define PH_MEDIA_SPEEXNB_PAYLOAD 105
#define PH_MEDIA_G726WB_PAYLOAD 102
#define PH_MEDIA_SPEEXWB_PAYLOAD 103
#define PH_MEDIA_ILBC_PAYLOAD 111
#define PH_MEDIA_AMR_PAYLOAD 123
#define PH_MEDIA_AMR_WB_PAYLOAD 124
#define PH_MEDIA_G72632_PAYLOAD 2
/* <UOLFONE> */
#define PH_MEDIA_G723_PAYLOAD 4
/* </UOLFONE> */
#define PH_MEDIA_G729_PAYLOAD 18
#define PH_MEDIA_H263_PAYLOAD 34
#define PH_MEDIA_H263FLV1_PAYLOAD 96
#define PH_MEDIA_H263P_PAYLOAD 97
#define PH_MEDIA_MPEG4_PAYLOAD 98
#define PH_MEDIA_H264_PAYLOAD 40
#define PH_MEDIA_CN_16000_PAYLOAD 125
#define PH_MEDIA_G72632_PAYLOAD 2
#define PH_MEDIA_CN_PAYLOAD 13

#define PH_MEDIA_CN_PT_STR "13"

/* <UOLFONE <mode adjustment> > */
#define PH_MSTREAM_PARAMS_MAX_PAYLOADS	16
/* </UOLFONE> */


struct ph_media_payload_s
{
  int  number;       /* payload code for rtp packets*/
  char string[32];   /* mime string (ex: ILBC/8000/1) */
  int  rate;         /* sampling rate */
  int  chans;        /* number of channels */
  int  ptime;        /* media duration in one packet in msecs */
  int  psize;        /* (optional) size of payload in one packet */
  int  mode;         /* (optional) codec specific mode parameter */
/* <UOLFONE <mode adjustment> > */
  char* fmtp;        /* value part of a fmtp attribute (ex: 111 mode=30) */
/* </UOLFONE> */
};



enum ph_media_types {
  PH_MEDIA_TYPE_AUDIO = 1,
  PH_MEDIA_TYPE_VIDEO,
  PH_MEDIA_TYPE_OTHER
};

/* traffic type of a streamer */
enum ph_mstream_traffic_type {
  PH_MSTREAM_TRAFFIC_NONE = 0,
  PH_MSTREAM_TRAFFIC_IN = 1, // 1<<0 = RX
  PH_MSTREAM_TRAFFIC_OUT = 2, // 1<<1 = TX
  PH_MSTREAM_TRAFFIC_IO = 3 // RX | TX
};


enum ph_mstream_flags {
  PH_MSTREAM_FLAG_VAD = 1,
  PH_MSTREAM_FLAG_CNG = 2,
  PH_MSTREAM_FLAG_AEC = 4,
  // SPIKE_HDX
  PH_MSTREAM_FLAG_MICHDX = 8,  /* half duplex mode */
  PH_MSTREAM_FLAG_SPKHDX = 16,  /* half duplex mode */
  PH_MSTREAM_FLAG_RUNNING = 32,
  PH_MSTREAM_FLAG_TUNNEL = 64,

};

/**
 * @struct description of 1 media stream
 */
struct ph_mstream_params_s
{
    /* local IP port on which the stream will be received */
	int  localport;
	int  mediatype;
	int  traffictype;
	int  flags;
	int  jitter;
/* <UOLFONE <pTime adjustment> > */
	int  ptime;
/* </UOLFONE> */
    /* IP of the remote peer */
	char remoteaddr[16];
	int  remoteport;
	int  vadthreshold;
	int  videoconfig;
/* <UOLFONE <mode adjustment> > */
	struct ph_media_payload_s ipayloads[PH_MSTREAM_PARAMS_MAX_PAYLOADS];
	struct ph_media_payload_s opayloads[PH_MSTREAM_PARAMS_MAX_PAYLOADS];
/* </UOLFONE> */
	/* structure dedicated to the underlying streamer engine (audio, video..)
		cf phastream, phvstream
	*/
	void *streamerData;
	
	/* <UOLFONE> */
	void *tunRtp;
	void *tunRtcp;
	/* </UOLFONE> */
};

typedef struct ph_mstream_params_s ph_mstream_params_t;


#define PH_MSESSION_MAX_STREAMS  4
#define PH_MSTREAM_AUDIO1 0
#define PH_MSTREAM_VIDEO1 1
#define PH_MSTREAM_AUDIO2 2
#define PH_MSTREAM_VIDEO2 3

enum ph_mession_conf_flags
  {
    PH_MSESSION_CONF_NOCONF = 0,
    PH_MSESSION_CONF_MEMBER = 1,
    PH_MSESSION_CONF_MASTER = 2
  };


/**
 * @brief session englobes up to 2 audio and 2 video streams
 * allocation sequence :
 *   - fill with 0
 *   - g_mutex_new on the critsec_mstream_init
 */
struct ph_msession_s
{
	/* bit mask of active streams */
	int    activestreams;

	/* bit mask of new streams to be activated */
	int    newstreams;

	/* potential role of the session in a conference. cf ph_mession_conf_flags enum */
	int    confflags;

    /* partner session for a conference (the other session)*/
	struct ph_msession_s *confsession;

    /* array of potential streams */
	struct ph_mstream_params_s streams[PH_MSESSION_MAX_STREAMS];

	/* mutex used to avoid threading issues when operating on the streams */
	GMutex *critsec_mstream_init;

	void (*dtmfCallback)(void *info, int event);
	void (*endCallback)(void  *info, int event);
	void (*frameDisplayCbk)(void *info, void *event);

	/* <UOLFONE> */
	void (*streamReceiveTimeoutCbk)(void *info, int event);
	/* </UOLFONE> */
    
	/*
        holder for the userdata "info" element of the callbacks prototypes
    */
	void *cbkInfo;
};


typedef struct ph_msession_s ph_msession_t;

typedef struct ph_media_payload_s  ph_media_payload_t;

int ph_media_init(int usetunnel, const char *pluginpath);
int ph_media_cleanup(void);



int ph_media_supported_payload(ph_media_payload_t *pt, const char *ptstring);
int ph_media_can_handle_payload(const char *mime);


/**
 * @brief start or (modify and restart) the necessary device/streaming engines of the session
 */
int ph_msession_start(struct ph_msession_s *s, const char *adeviceId);
/**
 * @brief stop the necessary device/streaming engines of the session
 */
void ph_msession_stop(struct ph_msession_s *s, const char *adeviceId);
int ph_msession_stopped(struct ph_msession_s *s);
#define ph_msession_stream_active(s, n)  (s->activestreams & (1 << n))
int ph_msession_set_recvol(struct ph_msession_s *s,  int level);
int ph_msession_set_playvol(struct ph_msession_s *s,  int level);
int ph_msession_send_sound_file(struct ph_msession_s *s, const char *filename);
int ph_msession_send_dtmf(struct ph_msession_s *s, int dtmf, int mode);
int ph_msession_conf_start(struct ph_msession_s *s1, struct ph_msession_s *s2, const char *adevice);
int ph_msession_conf_stop(struct ph_msession_s *s1, struct ph_msession_s *s2);
int ph_msession_suspend(struct ph_msession_s *s,  int traffictype, const char *adevice);
int ph_msession_resume(struct ph_msession_s *s, int traffictype, const char *adevice);


struct timeval;
void ph_tvdiff(struct timeval *diff,  struct timeval *out,  struct timeval *in);
void ph_tvsub( struct timeval *out,  struct timeval *in);
int ph_timeval_substract (struct timeval *result, struct timeval *x, struct timeval *y);

/* <UOLFONE> */
struct ph_msession_s * ph_msession_alloc();
void ph_msession_free(struct ph_msession_s *s);
void * ph_media_tunnel_open(char *out_host, int* out_host_len, int* out_port);
void * ph_media_tunnel_open_connected(const char* dst_host, int dst_port, char *out_host, int* out_host_len, int* out_port);
int ph_media_tunnel_close(void *media_tunnel);
void ph_media_tunnel_free(void *media_tunnel);
/* </UOLFONE> */
/* <UOLFONE <mode adjustment> > */
void ph_clear_msession_streams_fmtps(struct ph_msession_s *s);
/* </UOLFONE> */

/* */
#endif
