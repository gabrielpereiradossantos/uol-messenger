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

#include <string.h>

#include "payloadtype.h"

char offset127=127; 
char offset0xD5=0xD5; 
char offset0=0;

/* IMPORTANT */
/* some compiler don't support the "field:" syntax. Those macros are there to trap the problem
This means that if you want to keep portability, payload types must be defined with their fields
in the right order. */

#if !defined(HOST_IS_HPUX) && !defined(WIN32) 

#define TYPE(val) type: (val)
#define CLOCK_RATE(val) clock_rate: (val)
#define BYTES_PER_SAMPLE(val) bytes_per_sample: (val)
#define ZERO_PATTERN(val)   zero_pattern: (val)
#define PATTERN_LENGTH(val) pattern_length: (val)
#define NORMAL_BITRATE(val) normal_bitrate: (val)
#define MIME_TYPE(val) mime_type: (val)

#else

#define TYPE(val) (val)
#define CLOCK_RATE(val) (val)
#define BYTES_PER_SAMPLE(val) (val)
#define ZERO_PATTERN(val) (val)
#define PATTERN_LENGTH(val) (val)
#define NORMAL_BITRATE(val) (val)
#define MIME_TYPE(val) (val)

#endif

PayloadType pcmu8000={
	TYPE( PAYLOAD_AUDIO_CONTINUOUS),
	CLOCK_RATE( 8000),
	BYTES_PER_SAMPLE( 1.0),
	ZERO_PATTERN( &offset127),
	PATTERN_LENGTH( 1),
	NORMAL_BITRATE( 64000),
	MIME_TYPE ("PCMU")
};

PayloadType pcma8000={
	TYPE( PAYLOAD_AUDIO_CONTINUOUS),
	CLOCK_RATE(8000),
	BYTES_PER_SAMPLE(1.0),
	ZERO_PATTERN( &offset0xD5),
	PATTERN_LENGTH( 1),
	NORMAL_BITRATE( 64000),
	MIME_TYPE ("PCMA")
};

PayloadType pcm8000={
	TYPE( PAYLOAD_AUDIO_CONTINUOUS),
	CLOCK_RATE(8000),
	BYTES_PER_SAMPLE(2.0),
	ZERO_PATTERN( &offset0),
	PATTERN_LENGTH(1),
	NORMAL_BITRATE( 128000),
	MIME_TYPE ("PCM")
};

PayloadType lpc1016={
	TYPE( PAYLOAD_AUDIO_PACKETIZED),
	CLOCK_RATE(8000),
	BYTES_PER_SAMPLE( 0),
	ZERO_PATTERN( NULL),
	PATTERN_LENGTH( 0),
	NORMAL_BITRATE( 2400),
	MIME_TYPE ("1016")
};


PayloadType gsm=
{
	TYPE( PAYLOAD_AUDIO_PACKETIZED),
	CLOCK_RATE(8000),
	BYTES_PER_SAMPLE( 0),
	ZERO_PATTERN(NULL),
	PATTERN_LENGTH( 0),
	NORMAL_BITRATE( 13500),
	MIME_TYPE ("GSM")
};


PayloadType mpv=
{
	TYPE( PAYLOAD_VIDEO),
	CLOCK_RATE(90000),
	BYTES_PER_SAMPLE(0),
	ZERO_PATTERN(NULL),
	PATTERN_LENGTH(0),
	NORMAL_BITRATE( 0),
	MIME_TYPE ("MPV")
};


PayloadType h261={
	TYPE( PAYLOAD_VIDEO),
	CLOCK_RATE(90000),
	BYTES_PER_SAMPLE(0),
	ZERO_PATTERN(NULL),
	PATTERN_LENGTH(0),
	NORMAL_BITRATE(0),
	MIME_TYPE ("H261")
};

PayloadType truespeech=
{
	TYPE( PAYLOAD_AUDIO_PACKETIZED),
	CLOCK_RATE(8000),
	BYTES_PER_SAMPLE(0),
	ZERO_PATTERN(NULL),
	PATTERN_LENGTH( 0),
	NORMAL_BITRATE(8536),
	MIME_TYPE ("TSP0")
};

RtpProfile av_profile;


void av_profile_init(RtpProfile *profile)
{
	rtp_profile_clear_all(profile);
	rtp_profile_set_name(profile,"AV profile");
	rtp_profile_set_payload(profile,0,&pcmu8000);
	rtp_profile_set_payload(profile,1,&lpc1016);
	rtp_profile_set_payload(profile,3,&gsm);
	rtp_profile_set_payload(profile,8,&pcma8000);
	rtp_profile_set_payload(profile,31,&h261);
	rtp_profile_set_payload(profile,32,&mpv);
}
	
/* these are extra payload types that can be used dynamically */
PayloadType lpc1015={
    TYPE( PAYLOAD_AUDIO_PACKETIZED),
    CLOCK_RATE(8000),
    BYTES_PER_SAMPLE(0),
    ZERO_PATTERN(NULL),
    PATTERN_LENGTH(0),
    NORMAL_BITRATE(2400),
    MIME_TYPE ("1015")
};

PayloadType speex_nb={
    TYPE( PAYLOAD_AUDIO_PACKETIZED),
    CLOCK_RATE(8000),
    BYTES_PER_SAMPLE(0),
    ZERO_PATTERN(NULL),
    PATTERN_LENGTH(0),
    NORMAL_BITRATE(8000),   /*not true: 8000 is the minimum*/
    MIME_TYPE ("speex")
};

PayloadType speex_wb={
    TYPE( PAYLOAD_AUDIO_PACKETIZED),
    CLOCK_RATE(16000),
    BYTES_PER_SAMPLE(0),
    ZERO_PATTERN(NULL),
    PATTERN_LENGTH(0),
    NORMAL_BITRATE(28000),
    MIME_TYPE ("speex")
};
