/*
 * phmedia -  Phone Api media streamer
 *
 * Copyright (C) 2006 WENGO SAS
 * Copyright (C) 2005 WENGO SAS
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




#define PH_MEDIA_DTMF_PAYLOAD 101
#define PH_MEDIA_SPEEXNB_PAYLOAD 105
#define PH_MEDIA_G726WB_PAYLOAD 102
#define PH_MEDIA_SPEEXWB_PAYLOAD 103
#define PH_MEDIA_ILBC_PAYLOAD 111
#define PH_MEDIA_AMR_PAYLOAD 123
#define PH_MEDIA_AMR_WB_PAYLOAD 124
#define PH_MEDIA_G72632_PAYLOAD 2
#define PH_MEDIA_G729_PAYLOAD 18
#define PH_MEDIA_H263_PAYLOAD 34
#define PH_MEDIA_MPEG4_PAYLOAD 98
#define PH_MEDIA_H264_PAYLOAD 40
#define PH_MEDIA_CN_16000_PAYLOAD 125
#define PH_MEDIA_G72632_PAYLOAD 2
#define PH_MEDIA_CN_PAYLOAD 13
#define PH_MEDIA_G729_PAYLOAD 18

#define PH_MEDIA_CN_PT_STR "13"



struct ph_media_payload_s
{
  int  number;       /* payload code for rtp packets*/
  char string[32];   /* mime string (ex: ILBC/8000/1) */ 
  int  rate;         /* sampling rate */
  int  chans;        /* number of channels */
  int  ptime;        /* media duration in one packet in msecs */
  int  psize;        /* (optional) size of payload in one packet */
  int  mode;         /* (optional) codec specific mode parameter */
};



enum ph_media_types {
  PH_MEDIA_TYPE_AUDIO = 1,
  PH_MEDIA_TYPE_VIDEO,
  PH_MEDIA_TYPE_OTHER
};

enum ph_mstream_traffic_type { 
  PH_MSTREAM_TRAFFIC_NONE = 0,
  PH_MSTREAM_TRAFFIC_IN = 1,
  PH_MSTREAM_TRAFFIC_OUT = 2,
  PH_MSTREAM_TRAFFIC_IO = 3


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

/* media stream creation parameters */
struct ph_mstream_params_s
{
  int  localport;
  int  medatype;
  int  traffictype;
  int  flags;
  int  jitter;
  char remoteaddr[16];
  int  remoteport;
  int  vadthreshold;
  int  videoconfig; 
  struct ph_media_payload_s ipayloads[16];
  struct ph_media_payload_s opayloads[16];
  
  
  void *streamerData;


};

typedef struct ph_mstream_params_s ph_mstream_params_t;


#define PH_MSESSION_MAX_STREAMS  4
#define PH_MSTREAM_AUDIO1 0
#define PH_MSTREAM_VIDEO1 1
#define PH_MSTREAM_AUDIO2 2
#define PH_MSTREAM_VIDEO2 3

enum ph_mession_conf_flags
  {
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
  int    activestreams;   /* bit mask of active streams */
  int    newstreams;      /* bit mask of new streams to be activated */
  int    confflags;       /* when nonzero this session make part of a conference */
  struct ph_msession_s *confsession;
  struct ph_mstream_params_s streams[PH_MSESSION_MAX_STREAMS];

  void *critsec_mstream_init; /* mutex used to avoid threading issue when an INVITE
                                        is received for a session/stream that is currently being started */

  void (*dtmfCallback)(void *info, int event);
  void (*endCallback)(void  *info, int event);
  void (*frameDisplayCbk)(void *info, void *event);

  void *cbkInfo;

};


typedef struct ph_msession_s ph_msession_t; 

typedef struct ph_media_payload_s  ph_media_payload_t;



/* */
#endif
