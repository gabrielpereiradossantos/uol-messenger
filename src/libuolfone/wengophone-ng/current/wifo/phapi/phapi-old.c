/*
* phapi   phone api
*
* Copyright (C) 2006        WENGO SAS
* Copyright (C) 2004        Vadim Lebedev <vadim@mbdsys.com>
*
* This is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as
* published by the Free Software F undation; either version 2,
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

/**
* @file phapi.c
* @brief softphone  API
*
* phapi is a library providing simplified api to create VOIP sessions
* using eXosip library oSIP stack and oRTP stcak
* <P>
*/
#ifdef WIN32
#include <winsock2.h>
#endif /* WIN32 */
#include <phapi-old.h>
#include <phglobal.h>
#include <phvline.h>
#include <phevents.h>
#include <phrpc.h>
#include <phmedia.h>
#include <stun/stun.h>
#include <phcall.h>

#include <phlog.h>

#include <owpl_plugin.h>
#include <svoip_phapi.h>

/* <UOLFONE> */
#include <../src/eXosip2.h>
/* </UOLFONE> */

#include <phapi-util/util.h>
#include <eXosip/eXosip.h>
#include "phcodec.h"

#ifdef USE_HTTP_TUNNEL
#include "httptunnel.h"
#endif
/*
#define G711_ONLY
*/

#define SKIP(x)

#ifdef PHAPI_VIDEO_SUPPORT
#include <avcodec.h>
#include <webcam/webcam.h>
#endif

#include <osip2/osip_mt.h>
#include <rtpport.h> // only for GMutex
#include <eXosip/eXosip.h>
#include <eXosip/eXosip_cfg.h>


#ifdef OS_WINDOWS
#include <winsock2.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>

#ifndef OS_WINDOWS
//#include "config.h"
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#ifndef OS_MACOSX
#include <sys/soundcard.h>
#endif
#include <sys/ioctl.h>
#include <fcntl.h>
#else  /* WIN32 */
#define snprintf _snprintf
#define strncasecmp strnicmp
#define strcasecmp stricmp
#define usleep(usecs) Sleep((usecs)/1000)
#endif


void ph_wegot_dtmf(void  *ctx, int dtmfEvent);
static int ph_event_get();
static char *ph_get_call_contact(phcall_t *ca);

/**
 * Processes an event from eXosip as an SDP event
 *
 * @param	je	an eXosip event
 * @return	0 on success; negative value on failure
 */
static int ph_process_call_event_default(eXosip_event_t * je);

#define MEDIA_SUSPEND

#ifndef PH_STREAM_AUDIO
#define PH_STREAM_AUDIO (1 << 0)
#define PH_STREAM_VIDEO_RX (1 << 1)
#define PH_STREAM_VIDEO_TX (1 << 2)
#endif

#define PH_STREAM_CNG (1 << 30)

static int ph_call_retrieve_payloads(phcall_t *ca, eXosip_event_t *je, int flags);
static int ph_call_media_start(phcall_t *ca, eXosip_event_t *je, int resumeflag);
static void ph_call_media_stop(phcall_t * ca);
static int ph_call_media_suspend(phcall_t *ca, int localhold);
static int ph_tunnel_init2(char *sip_proxy);
void ph_release_call(phcall_t *ca);
int ph_call_hasaudio(phcall_t *ca);

int phWaitTimeout = 500;

#define PORT_STRING_LEN 16
static char ph_nat_router_addr[32];
static char ph_nat_type_str[32];
static char ph_nat_port_str[PORT_STRING_LEN];
static char ph_nat_video_port_str[16];

static char ph_nat_sip_port_str[16];

static char vcontact[256] = "\0";

static NatType ph_nat_type = StunTypeOpen;

static char * _get_local_sip_port();
static void  _get_local_video_sdp_port(char buf[]);
static void  _get_local_audio_sdp_port(char buf[]);

static char*  _get_public_sip_port();

void ph_message_progress(eXosip_event_t *je);
static void ph_keep_refreshing();
/*static*/ void  ph_call_requestfailure(eXosip_event_t *je);

static void ph_frame_display_cbk(void *ctx, void *event);

/* <UOLFONE> */
int ph_uolfone_mic_mute = 0;

MY_DLLEXPORT void phSetUOLFoneMicMute(int nVal)
{
	ph_uolfone_mic_mute = nVal;
}


MY_DLLEXPORT int phGetUOLFoneMicMute()
{
	return ph_uolfone_mic_mute;
}
/* </UOLFONE> */

#define nonempty(x)  ((x) && (x)[0])

static void
ph_build_cname(char *buf, int n, phVLine *vl);


//int phvlRegister(int vlid);
//static phVLine *vline_alloc();
//static void vline_free(phVLine *vl);
//#define PHM_IGNORE_PORT 1
//#define PHM_IGNORE_HOST 2
//static phVLine *ph_find_matching_vline(const char *userid, int ignore);
//static phVLine *ph_find_matching_vline2(const char *username, const char* host, int ignore);
//static phVLine *ph_find_matching_vline3(const char *username, const char* host, int port, int ignore);
//static phVLine *ph_find_vline_by_rid(int rid);

static char *ph_get_proxy(const char *from);
static int   ph_get_vline_id(const char *userid, const char *altid);

// mutex for ph_release_call
GMutex * ph_media_stop_mutex;
#define PH_MSESSION_STOP_LOCK() g_mutex_lock(ph_media_stop_mutex)
#define PH_MSESSION_STOP_UNLOCK() g_mutex_unlock(ph_media_stop_mutex)
////

int getPublicPort(char *local_voice_port, char *local_video_port, char *public_voice_port, char *public_video_port);

#define clear(x) memset(&x, 0, sizeof(x))

/* <UOLFONE> */
MY_DLLEXPORT void phFreeVlines()
{
	phVLine *vl = NULL;
	int i = 1;
	for (i = 1; i <= PH_MAX_VLINES; i++)
	{
		vl = ph_vlid2vline(i);
		if (vl->used == 1)
		{
			vline_free(vl);
		}
	}
}

/* forward declaration */
void ph_payloads_init();
/* </UOLFONE> */

phCallbacks_t *phcb;
int phIsInitialized;
int phDebugLevel = 0;
char *phLogFileName = 0;

static char ph_follow_me_addr[256];

unsigned short phCallBackPort = PH_CALLBACK_PORT;
 unsigned short phServerPort = PH_SERVER_PORT;

static int ph_busyFlag;

static FILE *ph_log_file;

#define ph_printf printf
void * ph_api_thread(void *arg);

/* <UOLFONE> */
static struct osip_thread* ph_ayncmode_thread = NULL;

static void ph_asyncmode_thread_start()
{
	ph_ayncmode_thread = osip_thread_create(20000, ph_api_thread, 0);
}

static void ph_asyncmode_thread_stop()
{
	if (ph_ayncmode_thread != NULL)
	{
		osip_thread_join(ph_ayncmode_thread);
		osip_free(ph_ayncmode_thread);
		ph_ayncmode_thread = NULL;
	}
}


static phCustomHeaderInfo_t* ph_custom_header_alloc()
{
	phCustomHeaderInfo_t * ph_custom_header = (phCustomHeaderInfo_t *) osip_malloc(sizeof(phCustomHeaderInfo_t));

	if (ph_custom_header)
	{
		memset(ph_custom_header, 0, sizeof(phCustomHeaderInfo_t));
	}

	return ph_custom_header;
}


static void ph_custom_header_free(phCustomHeaderInfo_t *ph_custom_header)
{
	osip_free(ph_custom_header->name);
	osip_free(ph_custom_header->value);
	osip_free(ph_custom_header);
}


static void ph_add_custom_reply_headers(phRegStateInfo_t* regInfo, eXosip_event_t *je)
{
	struct osip_header *header = NULL;
	phCustomHeaderInfo_t *ph_custom_header = NULL;
	int pos, size;

	if (!regInfo || !je)
	{
		return;
	}

	size = osip_list_size(je->custom_reply_headers);
	for (pos = 0; pos < size; pos++)
	{
		header = (struct osip_header *) osip_list_get(je->custom_reply_headers, pos);

		if (header)
		{
			ph_custom_header = ph_custom_header_alloc();

			if (ph_custom_header)
			{
				ph_custom_header->name = osip_strdup(header->hname);
				ph_custom_header->value = osip_strdup(header->hvalue);

				ADD_ELEMENT(regInfo->custom_headers, ph_custom_header);
			}
		}
	}
}

static void ph_clear_custom_reply_headers(phRegStateInfo_t* regInfo)
{
	phCustomHeaderInfo_t *ph_custom_header = NULL;

	for (ph_custom_header = regInfo->custom_headers; ph_custom_header != NULL; ph_custom_header = regInfo->custom_headers)
	{
		REMOVE_ELEMENT(regInfo->custom_headers, ph_custom_header);
		ph_custom_header_free(ph_custom_header);
	}
}

/* </UOLFONE> */


/* <UOLFONE <mode adjustment> > */
static void ph_update_media_payloads_packetization_info(
		osip_list_t *local_format_modifiers, struct ph_mstream_params_s* msp, 
		int in_call_ptime, int bIsCallAccepted)
{
	int payloadIndex;
	int fmtpModifIndex;
	char* format_modifier = NULL;
	char payload_id[10];
	
	if (!msp)
	{
		return;
	}
	
	for (payloadIndex = 0; payloadIndex < PH_MSTREAM_PARAMS_MAX_PAYLOADS; payloadIndex++)
	{
		if (msp->opayloads[payloadIndex].string[0] != '\0')
		{
			memset(payload_id, 0, 10);
			itoa(msp->opayloads[payloadIndex].number, payload_id, 10);
			
			if (local_format_modifiers)
			{
				for (fmtpModifIndex = 0; 
						!osip_list_eol(local_format_modifiers, fmtpModifIndex); fmtpModifIndex++)
				{
					format_modifier = (char*)osip_list_get(local_format_modifiers, fmtpModifIndex);
					
					if ( (strstr(format_modifier, payload_id) == format_modifier) && 
							(format_modifier[strlen(payload_id)] == ' ') )
					{
						msp->opayloads[payloadIndex].fmtp = osip_strdup(format_modifier);
						msp->ipayloads[payloadIndex].fmtp = osip_strdup(format_modifier);
						
						break;
					}
				}
			}
			
			if (msp->ptime != 0)
			{
				msp->opayloads[payloadIndex].ptime = msp->ptime;
				
				if (bIsCallAccepted)
				{
					msp->ipayloads[payloadIndex].ptime = msp->ptime;
				}
				else
				{
					msp->ipayloads[payloadIndex].ptime = in_call_ptime;
				}
			}
		}
	}
}
/* </UOLFONE> */



#define name(x) #x

static char *evtnames[] =
{
	name(EXOSIP_REGISTRATION_NEW),              /* announce new registration.       */
	name(EXOSIP_REGISTRATION_SUCCESS),          /* user is successfully registred.  */
	name(EXOSIP_REGISTRATION_FAILURE),          /* user is not registred.           */
	name(EXOSIP_REGISTRATION_REFRESHED),        /* registration has been refreshed. */
	name(EXOSIP_REGISTRATION_TERMINATED),       /* UA is not registred any more.    */

	/* for UAC events */
	name(EXOSIP_CALL_NOANSWER),        /* announce no answer within the phWaitTimeout */
	name(EXOSIP_CALL_PROCEEDING),      /* announce processing by a remote app   */
	name(EXOSIP_CALL_RINGING),         /* announce ringback                     */
	name(EXOSIP_CALL_ANSWERED),        /* announce start of call                */
	name(EXOSIP_CALL_REDIRECTED),      /* announce a redirection                */
	name(EXOSIP_CALL_REQUESTFAILURE),  /* announce a request failure            */
	name(EXOSIP_CALL_SERVERFAILURE),   /* announce a server failure             */
	name(EXOSIP_CALL_GLOBALFAILURE),   /* announce a global failure             */

	/* for UAS events */
	name(EXOSIP_CALL_NEW),             /* announce a new call                   */
	name(EXOSIP_CALL_ACK),             /* ACK received for 200ok to INVITE      */
	name(EXOSIP_CALL_CANCELLED),       /* announce that call has been cancelled */
	name(EXOSIP_CALL_TIMEOUT),         /* announce that call has failed         */
	name(EXOSIP_CALL_HOLD),            /* audio must be stopped                 */
	name(EXOSIP_CALL_OFFHOLD),         /* audio must be restarted               */
	name(EXOSIP_CALL_CLOSED),          /* a BYE was received for this call      */

	/* for both UAS & UAC events */
	name(EXOSIP_CALL_STARTAUDIO),         /* audio must be established           */
	name(EXOSIP_CALL_RELEASED),           /* call context is cleared.            */

	/* for UAC events */
	name(EXOSIP_OPTIONS_NOANSWER),        /* announce no answer within the phWaitTimeout */
	name(EXOSIP_OPTIONS_PROCEEDING),      /* announce processing by a remote app   */
	name(EXOSIP_OPTIONS_ANSWERED),        /* announce a 200ok                      */
	name(EXOSIP_OPTIONS_REDIRECTED),      /* announce a redirection                */
	name(EXOSIP_OPTIONS_REQUESTFAILURE),  /* announce a request failure            */
	name(EXOSIP_OPTIONS_SERVERFAILURE),   /* announce a server failure             */
	name(EXOSIP_OPTIONS_GLOBALFAILURE),   /* announce a global failure             */

	name(EXOSIP_INFO_NOANSWER),        /* announce no answer within the phWaitTimeout */
	name(EXOSIP_INFO_PROCEEDING),      /* announce processing by a remote app   */
	name(EXOSIP_INFO_ANSWERED),        /* announce a 200ok                      */
	name(EXOSIP_INFO_REDIRECTED),      /* announce a redirection                */
	name(EXOSIP_INFO_REQUESTFAILURE),  /* announce a request failure            */
	name(EXOSIP_INFO_SERVERFAILURE),   /* announce a server failure             */
	name(EXOSIP_INFO_GLOBALFAILURE),   /* announce a global failure             */

	/* for UAS events */
	name(EXOSIP_OPTIONS_NEW),             /* announce a new options method         */
	name(EXOSIP_INFO_NEW),               /* new info request received           */

	name(EXOSIP_MESSAGE_NEW),            /* announce new incoming MESSAGE. */
	name(EXOSIP_MESSAGE_SUCCESS),        /* announce a 200ok to a previous sent */
	name(EXOSIP_MESSAGE_FAILURE),        /* announce a failure. */


	/* Presence and Instant Messaging */
	name(EXOSIP_SUBSCRIPTION_NEW),          /* announce new incoming SUBSCRIBE.  */
	name(EXOSIP_SUBSCRIPTION_UPDATE),       /* announce incoming SUBSCRIBE.      */
	name(EXOSIP_SUBSCRIPTION_CLOSED),       /* announce end of subscription.     */

	name(EXOSIP_SUBSCRIPTION_NOANSWER),        /* announce no answer              */
	name(EXOSIP_SUBSCRIPTION_PROCEEDING),      /* announce a 1xx                  */
	name(EXOSIP_SUBSCRIPTION_ANSWERED),        /* announce a 200ok                */
	name(EXOSIP_SUBSCRIPTION_REDIRECTED),      /* announce a redirection          */
	name(EXOSIP_SUBSCRIPTION_REQUESTFAILURE),  /* announce a request failure      */
	name(EXOSIP_SUBSCRIPTION_SERVERFAILURE),   /* announce a server failure       */
	name(EXOSIP_SUBSCRIPTION_GLOBALFAILURE),   /* announce a global failure       */
	name(EXOSIP_SUBSCRIPTION_NOTIFY),          /* announce new NOTIFY request     */

	name(EXOSIP_SUBSCRIPTION_RELEASED),        /* call context is cleared.        */

	name(EXOSIP_IN_SUBSCRIPTION_NEW),          /* announce new incoming SUBSCRIBE.*/
	name(EXOSIP_IN_SUBSCRIPTION_RELEASED),     /* announce end of subscription.   */

	name(EXOSIP_CALL_REFERED),              /* announce incoming REFER           */
	name(EXOSIP_CALL_REFER_STATUS),         /* announce incoming NOTIFY          */
	name(EXOSIP_CALL_REFER_FAILURE),         /* announce error during refer */

	name(EXOSIP_CALL_REPLACES)         /* announce incoming INVITE with Rpelaces    */

};

#undef name



phcall_t ph_calls[PH_MAX_CALLS];


/*
//#define FORCE_VAD   1
#define FORCE_CNG   1
*/




phConfig_t *phGetConfig()
{
	return &phcfg;
}






#ifdef EMBED
phConfig_t phcfg = {"10600", "",
/* sipport  */ "5060",
/* identity */ "",
/*  proxy */    "" ,
/* nattype */  "" ,
/* codecs */   "" ,
0 ,0 ,
/* audio_dev */   "" ,
/* softboost */ 0,0,0,
/* vad */ 0,0,0,
0,0,0,
/* stunserver */ "80.118.132.74",
0
};
#else
phConfig_t phcfg = {"10600", "", "10700", "",
/* sipport  */ "5060",
/* identity */ "",
/*  proxy */    "" ,
/* nattype */  "" ,
/* codecs */   "" ,"",
0 ,0 ,
/* audio_dev */   "" ,
/* softboost */ 0,0,0,
/* vad */ 0,0,0, 0, 0,
0,0,0,
/* stunserver */  "80.118.132.74",
0
};

#endif

static int _is_video_enabled(int streams)
{
	return (streams & (PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_TX));
}


static int _is_audio_enabled(int streams)
{
	return (streams & PH_STREAM_AUDIO);
}


static int
ph_port_inuse(int port)
{
	int i;
	phcall_t *ca;

	/* scan all active calls and check that the given port is inuse */
	for(i=0; i<PH_MAX_CALLS;  i++)
	{
		ca = &ph_calls[i];
		if (ca->cid != -1)
		{
			/* active call */
			if(ca->local_sdp_audio_port == port || port == ca->local_sdp_video_port)
			{
				return 1;
			}
		}
	}

	return 0;
}

static void
_get_local_audio_sdp_port(char buf[])
{

	/* <MINHPQ>
	* To get arround the problem of connect in winsock ( connect returns
	* an error: 10048: Address already in use connect is called an the local address is in used
	* within the last 2 or 4 minutes), we should not bind the rtp socket to a specific local port. Hence,
	* we should return "0" here to let the system choose a random port number.
	*/

	/* base port number*/
	int port = atoi(phcfg.local_rtp_port);

	buf[0] = 0;

	while(1)
	{
		if (!ph_port_inuse(port))
		{
			sprintf(buf, "%d", port);
			return;
		}
		else
		{
			port += 2;    /* try next pair */
		}
	}
}

static void
_get_local_video_sdp_port(char buf[])
{
	int port = atoi(phcfg.local_video_rtp_port) + 2;

	buf[0] = 0;

	while(1)
	{
		if (!ph_port_inuse(port))
		{
			sprintf(buf, "%d", port);
			return;
		}
		else
		{
			port += 2;    /* try next pair */
		}
	}
}


static char *
_get_local_sip_port()
{
	return phcfg.sipport;
}

static char *
_get_public_sip_port()
{
	if (ph_nat_sip_port_str[0])
	{
		return ph_nat_sip_port_str;
	}

	return _get_local_sip_port();

}



phcall_t *
ph_locate_call_by_cid(int cid)
{
	phcall_t *ca;


	for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
	{
		if (ca->cid == cid)
		{
			return ca;
		}
	}

	return 0;
}

#if 0
void phReleaseTerminatedCalls()
{
	phcall_t *ca;
	for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
	{
		if ((ca->cid != -1) && (ph_media_is_stream_stopped(ca) == 1))
		{
			ph_release_call(ca);
		}
	}
}
#endif

phcall_t *
ph_locate_call_by_rcid(int cid)
{
	phcall_t *ca;


	for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
	{
		if (ca->rcid == cid)
		{
			return ca;
		}
	}

	return 0;
}

phcall_t *
ph_locate_call_by_rdid(int did)
{
	phcall_t *ca;


	for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
	{
		if (ca->rdid == did)
		{
			return ca;
		}
	}

	return 0;
}


phcall_t *
ph_allocate_call(int cid)
{
	phcall_t *ca = ph_locate_call_by_cid(-1);

	if (!ca)
	{
		return 0;
	}

	ca->redirs = 0;
	ca->cid = cid;
	return ca;
}



phcall_t *
ph_locate_call(eXosip_event_t *je, int creatit)
{
	phcall_t *ca, *found = 0, *newca = 0;

	/* lookup matching call descriptor */
	for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
	{
		// locate first non-used pre-allocated phcall_t
		if (ca->cid == -1 && !newca)
		{
			newca = ca;
		}

		if (ca->extern_cid == je->cid)
		{
			found  = ca;
			break;
		}
	}

	ca = found;

	if (!ca)   /* we didn't find a matching call descriptor */
	{
		if (creatit)
		{
			/* allocate a new one */
			if (!newca)
			{
				return 0; /* !!! BUG !!! */
			}
			ca = newca;
			memset(ca, 0, sizeof(*ca));
			ca->cid = getNextCallId();
			ca->extern_cid = je->cid;
			ca->did = je->did;
			ca->vlid = ph_vline2vlid(ph_find_vline_by_rid(je->rid));
		}
	}

	if (!ca)
	{
		return 0;
	}

	if (je->remote_sdp_audio_ip[0])
	{
		strncpy(ca->remote_sdp_audio_ip, je->remote_sdp_audio_ip, sizeof(ca->remote_sdp_audio_ip));
		ca->remote_sdp_audio_port = je->remote_sdp_audio_port;
		strncpy(ca->audio_payload_name, je->payload_name, sizeof(ca->audio_payload_name));
		ca->audio_payload = je->payload;
	}

	if (je->remote_sdp_video_ip[0])
	{
		strncpy(ca->remote_sdp_video_ip, je->remote_sdp_video_ip, sizeof(ca->remote_sdp_video_ip));
		ca->remote_sdp_video_port = je->remote_sdp_video_port;
		strncpy(ca->video_payload_name, je->video_payload_name, sizeof(ca->video_payload_name));
		ca->video_payload = je->video_payload;
	}

	return ca;
}

void ph_release_call(phcall_t *ca)
{
	DBG_SIP_NEGO("SIP_NEGO: ph_release_call start\n");
	PH_MSESSION_STOP_LOCK();

	ph_call_media_stop(ca);

	/* <UOLFONE> */
	if (ca->mses)
	{
		ph_msession_free(ca->mses);
		ca->mses = 0;
	}
	/* </UOLFONE> */

	memset(ca, 0, sizeof(phcall_t));
	ca->cid = -1;
	ca->extern_cid = -1;
	PH_MSESSION_STOP_UNLOCK();
}

// <ncouturier>
/**
* Releases a SIP call (notion of audio is here inexistant)
*/
void ph_release_call2(phcall_t *ca){
	DBG_SIP_NEGO("SIP_NEGO: ph_release_call2 start\n");

	memset(ca, 0, sizeof(phcall_t));
	ca->cid = -1;
	ca->extern_cid = -1;
}
// </ncouturier>

int ph_has_active_calls()
{
	phcall_t *ca;
	int count = 0;

	for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS];  ca++)
	{
		if (ca->cid != -1 && ca->extern_cid != -1 && ph_call_hasaudio(ca))
		{
			if (!ca->remotehold && !ca->localhold)
			{
				count++;
			}
		}
	}
	return count;
}

/**
* @brief does the call have an active audio session stream ?
*/
int ph_call_hasaudio(phcall_t *ca)
{
	if (ca->mses && (ca->mses->activestreams & (1 << PH_MSTREAM_AUDIO1)))
	{
		return 1;
	}
	return 0;
}

void ph_stream_ended(void *ctx, int event)
{
	phcall_t *ca = (phcall_t *)ctx;

	ca->closereq = 1;
}

void ph_wegot_dtmf(void *ctx, int dtmfEvent)
{
	phCallStateInfo_t info;
	phcall_t *ca = (phcall_t *)ctx;

	clear(info);

	info.event = phDTMF;
	info.u.dtmfDigit = dtmfEvent;
	info.vlid = ca->vlid;
	if (phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = NULL;
		info.textinfo = NULL;
		info.status_code = 0;
		info.error_source = 0;
/* </UOLFONE> */
		phcb->callProgress(ca->cid, &info);
	}

	owplFireCallEvent2(ca->cid,
		CALLSTATE_AUDIO_EVENT,
		CALLSTATE_AUDIO_DTMF,
		0, /* No remote address */
		dtmfEvent,
		0, /* No additional data */
		0  /* no associated call*/
		);


}

/* <UOLFONE> */
void ph_stream_receive_timeout(void *ctx, int error)
{
	phCallStreamErrorInfo_t info;
	phcall_t *ca = (phcall_t *) ctx;

	clear(info);

	info.error = error;
	info.vlid = ca->vlid;

	//phcb->callStreamError(ca->cid, &info);
}
/* </UOLFONE> */

void ph_frame_display_cbk(void *ctx, void *event)
{
	phcall_t *ca = (phcall_t *) ctx;

	if (phcb->onFrameReady) {
		phcb->onFrameReady(ca->cid, (phVideoFrameReceivedEvent_t*)event);
	}
	owplFireCallEvent2(ca->cid,
		CALLSTATE_VIDEO_EVENT,
		CALLSTATE_VIDEO_FRAME_RCV,
		0,
		0,
		event,
		0
		);
}





int ph_same_str(const char *str1, const char *str2)
{
	if (str1 == 0)
	{
		return str2 == 0;
	}

	if (str2 == 0)
	{
		return str1 == 0;
	}

	return (0 == strcasecmp(str1, str2));
}

int ph_same_uri(const char *uristr1, const char *uristr2)
{
	osip_contact_t *uri1, *uri2;
	int ret;

	osip_contact_init(&uri1);
	osip_contact_init(&uri2);

	osip_contact_parse(uri1, uristr1);
	osip_contact_parse(uri2, uristr2);

	/* if we've got and invalid URI return TRUE */
	if (!uri1 || !uri2 || !uri1->url || !uri2->url)
	{
		ret = 1;
	}
	else
	{
		ret = ph_same_str(uri1->url->username, uri2->url->username) &&
			ph_same_str(uri1->url->host, uri2->url->host) &&
			ph_same_str(uri1->url->port, uri2->url->port);
	}

	osip_contact_free(uri1);
	osip_contact_free(uri2);

	return ret;
}



int
phGetVersion()
{
	static char version[] = PHAPI_VERSION;
	char *subv = strstr(version, ".");
	int v,s,r;

	v = atoi(version);
	s = atoi(subv+1);
	r = atoi(strstr(subv+1, ".")+1);

	return (v << 16) | (s << 8) | r;
}
#undef stringize


int
phGetAudioVersion()
{
	int fd, ret=-1;
#if !defined(WIN32) && !defined(__APPLE__) && !defined(__FreeBSD__)
#ifndef EMBED
	fd = open("/dev/dsp", O_RDWR, O_NONBLOCK);
#else
	fd = open("/dev/sound/mixer0", O_RDWR, O_NONBLOCK);
#endif
	if (fd>=0)
	{
#if defined(EMBED)
		if(0>ioctl(fd, SOUND_MIXER_PRIVATE5, &ret))
#else
		if(0>ioctl(fd, OSS_GETVERSION, &ret))
#endif
		{
			ret = -1;
		}
		close(fd);
	}
	return ret;
#else
	return 0;
#endif
}

int
phGetNatInfo(char *ntstr, int ntlen, char *fwip, int fwiplen)
{
	assert(ntstr);
	assert(fwip);

	strncpy(ntstr, ph_nat_type_str, ntlen);
	strncpy(fwip, ph_nat_router_addr, fwiplen);
	return 0;
}

static void
ph_build_cname(char *buf, int n, phVLine *vl)
{
	char *un, *s;
	assert(buf);
	assert(vl);

	un = nonempty(vl->username) ? vl->username : "unknown";
	s = nonempty(vl->server) ? vl->server : "localhost";

	snprintf(buf, n, "%s@%s", un, s);
}

#define optional(x) (x[0] ? x : 0)

int
phLinePlaceCall_withCa(int vlid, const char *uri, void *userdata, int rcid, int streams, phcall_t *ca0)
{
	int i;
	osip_message_t *invite;
	phcall_t *ca = 0;
	char *proxy = phcfg.proxy;
	phVLine *vl;
	char from[512];
	char  local_voice_port[16];
	char  local_video_port[16];
	/* <UOLFONE> */
	char  audio_rtp_tunnel_out_host[HTTP_TUNNEL_HOST_MAX_LEN + 1];
	int   audio_rtp_tunnel_out_host_len = HTTP_TUNNEL_HOST_MAX_LEN + 1;
	int   audio_rtp_tunnel_out_port = 0;
	char  audio_rtcp_tunnel_out_host[HTTP_TUNNEL_HOST_MAX_LEN + 1];
	int   audio_rtcp_tunnel_out_host_len = HTTP_TUNNEL_HOST_MAX_LEN + 1;
	int   audio_rtcp_tunnel_out_port = 0;
	int rtp_use_tunnel = phcfg.rtp_use_tunnel;
	void* audio_rtp_tunnel = NULL;
	void* audio_rtcp_tunnel = NULL;
	int port_len;
	/* </UOLFONE> */
#ifdef STUN_ENABLE
	char  public_voice_port[16];
	char  public_video_port[16];

	public_video_port[0] = 0;
	public_voice_port[0] = 0;
#endif

	//streams = PH_STREAM_AUDIO;
	DBG_SIP_NEGO("phLinePlaceCall_withCa start\n");

	local_video_port[0] = 0;
	local_voice_port[0] = 0;

	if (!nonempty(uri))
	{
		return -PH_BADARG;
	}

	vl = ph_valid_vlid(vlid);

	if (!vl)
	{
		return -PH_BADVLID;
	}

	if (rcid) {
		ca = ph_locate_call_by_cid(rcid);
		if (!ca)
		{
			return -PH_BADCID;
		}
	}

	ph_vline_get_from(from, sizeof(from), vl);

	proxy = vl->proxy;

	i = eXosip_build_initial_invite(&invite,
		uri,
		from,
		proxy,
		"");
	if (i!=0)
	{
		return -1;
	}

	if (_is_video_enabled(streams))
	{
		_get_local_video_sdp_port(local_video_port);
	}

	_get_local_audio_sdp_port(local_voice_port);

	/* <UOLFONE> */
	if (rtp_use_tunnel)
	{
		audio_rtp_tunnel_out_host[0] = '\0';
		audio_rtp_tunnel = ph_media_tunnel_open(audio_rtp_tunnel_out_host, &audio_rtp_tunnel_out_host_len, &audio_rtp_tunnel_out_port);

		if (audio_rtp_tunnel == NULL)
		{
			return -3;
		}

		audio_rtp_tunnel_out_host[HTTP_TUNNEL_HOST_MAX_LEN] = '\0';

		audio_rtcp_tunnel_out_host[0] = '\0';
		audio_rtcp_tunnel = ph_media_tunnel_open(audio_rtcp_tunnel_out_host, &audio_rtcp_tunnel_out_host_len, &audio_rtcp_tunnel_out_port);

		if (audio_rtcp_tunnel == NULL)
		{
			ph_media_tunnel_close(audio_rtp_tunnel);
			ph_media_tunnel_free(audio_rtp_tunnel);

			return -3;
		}

		audio_rtcp_tunnel_out_host[HTTP_TUNNEL_HOST_MAX_LEN] = '\0';

		// TODO: Handle video stream
		port_len = snprintf(local_voice_port, 16, "%d", audio_rtp_tunnel_out_port);
		if (port_len > 0)
		{
			local_voice_port[port_len] = '\0';
		}

		eXosip_set_mediaip(audio_rtp_tunnel_out_host);
		eXosip_set_media_use_tunnel(TRUE);
	}
	else
	{
		eXosip_set_mediaip(phcfg.local_media_ip);
		eXosip_set_media_use_tunnel(FALSE);
	}
	/* </UOLFONE> */

#ifdef STUN_ENABLE

	if (0!=strncasecmp(eXosip_get_nattype(), "none", 4) && (0!=strncasecmp(eXosip_get_nattype(), "sym", 3) ))
	{
		int ret = getPublicPort(local_voice_port, local_video_port, public_voice_port, public_video_port );

		if (ret <1)
		{
			DBG_SIP_NEGO("unable to alocate public port ...\n");
			return;
		}

		eXosip_lock();
		DBG_SIP_NEGO("STUN ports (a.local=%s, a.public=%s) (v.local=%s, v.public=%s)\n",local_voice_port,public_voice_port,local_video_port,public_video_port);
		i = eXosip_initiate_call(invite, userdata, NULL,  local_voice_port,  optional(local_video_port), optional(public_voice_port),
			optional(public_video_port));
	}
	else
	{
#endif
	eXosip_lock();
	DBG_SIP_NEGO("NO STUN ports (a.local=%s, a.public=%s) (v.local=%s, v.public=%s)\n",local_voice_port,0,local_video_port,0);
	i = eXosip_initiate_call(invite, userdata, NULL, local_voice_port, optional(local_video_port),  0, 0);

#ifdef STUN_ENABLE
	}
#endif

	if (!ca0) {
		int newCallId = getNextCallId();
		ca0 = ph_allocate_call(newCallId);
		ca0->extern_cid = i;
	}
	else {
		if (ca0->cid < 0) {
			ca0->cid = getNextCallId();
		}
		ca0->extern_cid = i;
	}


	/* <UOLFONE> */
	if (rtp_use_tunnel)
	{
		ca0->mses = ph_msession_alloc();
		
		if (ca0->mses)
		{
			(ca0->mses->streams)[PH_MSTREAM_AUDIO1].tunRtp = audio_rtp_tunnel;
			(ca0->mses->streams)[PH_MSTREAM_AUDIO1].tunRtcp = audio_rtcp_tunnel;
		}
	}
	/* </UOLFONE> */

	ca0->local_sdp_audio_port = atoi(local_voice_port);
	if (_is_video_enabled(streams))
	{
		ca0->local_sdp_video_port = atoi(local_video_port);
	}


	ca0->user_mflags = streams;
	ca0->nego_mflags = ca0->user_mflags;

	if (rcid)
	{
		ca0->rcid = rcid;
	}

	ca0->vlid = ph_vline2vlid(vl);

	eXosip_unlock();

	owplFireCallEvent(ca0->cid,
		CALLSTATE_REMOTE_OFFERING,
		CALLSTATE_REMOTE_OFFERING_NORMAL,
		uri,
		0);

	return ca0->cid;
}


int
phLinePlaceCall(int vlid, const char *uri, void *userdata, int rcid)
{
	return phLinePlaceCall_withCa(vlid, uri, userdata, rcid, PH_STREAM_AUDIO, 0);
}

int
phLinePlaceCall2(int vlid, const char *uri, void *userdata, int rcid, int streams)
{
	return phLinePlaceCall_withCa(vlid, uri, userdata, rcid, streams, 0);
}


int
phLineSendMessage(int vlid, const char *uri, const char *buff, const char *mime)
{
	int i;
	phVLine *vl;
	char from[512];

	vl = ph_valid_vlid(vlid);
	if (!vl)
	{
		return -PH_BADVLID;
	}

	ph_vline_get_from(from, sizeof(from), vl);

	if ( !nonempty(uri))
	{
		return -PH_BADARG;
	}

	eXosip_lock();
	i = eXosip_message((char *)uri, from, vl->proxy, buff, mime);
	eXosip_unlock();
	return i;
}


int
phSendMessage(const char *from, const char *uri,
			  const char *buff, const char *mime)
{
	int i;

	if (!nonempty(from) || !nonempty(uri))
	{
		return -PH_BADARG;
	}

	eXosip_lock();
	i = eXosip_message((char *)uri, (char*) from, ph_get_proxy(from),
		buff, mime);
	eXosip_unlock();
	return i;
}


int
phLineSubscribe(int vlid, const char *uri, const int winfo)
{
	int i;
	phVLine *vl;
	char from[512];


	vl = ph_valid_vlid(vlid);

	if (!vl)
	{
		return -PH_BADVLID;
	}

	ph_vline_get_from(from, sizeof(from), vl);


	if ( !nonempty(uri))
	{
		return -PH_BADARG;
	}


	eXosip_lock();
	i = eXosip_subscribe(uri, from, vl->proxy, winfo);
	eXosip_unlock();
	if (i < 0) {
		return -1;
	}
	return 0;
}

int
phSubscribe(const char *from, const char *to, const int winfo)
{
	int i;

	if (!nonempty(to) || !nonempty(from))
	{
		return -PH_BADARG;
	}

	eXosip_lock();
	i = eXosip_subscribe(to, from, ph_get_proxy(from), winfo);
	eXosip_unlock();
	if (i < 0) {
		return -1;
	}
	return 0;
}



int
phLinePublish(int vlid, const char *uri, const int winfo, const char * content_type, const char * content)
{
	int i;
	phVLine *vl;
	char from[512];


	vl = ph_valid_vlid(vlid);

	if (!vl)
	{
		return -PH_BADVLID;
	}

	ph_vline_get_from(from, sizeof(from), vl);


	if ( !nonempty(uri))
	{
		return -PH_BADARG;
	}


	eXosip_lock();
	i = eXosip_publish(uri, from, vl->proxy, winfo, content_type,content);
	eXosip_unlock();
	return i;
}



int
phPublish(const char *from, const char *to, const int winfo, const char * content_type, const char * content)
{
	int i;

	if (!nonempty(to) || !nonempty(from) || !nonempty(content_type) || !nonempty(content))
	{
		return -PH_BADARG;
	}


	i = eXosip_publish(to, from, ph_get_proxy(from), winfo, content_type,content);
	eXosip_unlock();
	return i;
}

void
phRefresh()
{
	if (!phIsInitialized)
	{
		return;
	}
	eXosip_lock();
	eXosip_update();
	eXosip_unlock();
}


int
phLineSendOptions(int vlid, const char *to)
{
	int i;
	phVLine *vl;
	char from[512];

	vl = ph_valid_vlid(vlid);

	if (!vl)
	{
		return -PH_BADVLID;
	}

	ph_vline_get_from(from, sizeof(from), vl);

	if (!nonempty(to))
	{
		return -PH_BADARG;
	}


	eXosip_lock();
	i = eXosip_options(to, from, vl->proxy);
	eXosip_unlock();
	return i;

}



int
phSendOptions(const char *from, const char *uri)
{
	int i;

	if (!nonempty(from) || !nonempty(uri))
	{
		return -PH_BADARG;
	}

	eXosip_lock();
	i = eXosip_options((char *)uri, (char*) from, ph_get_proxy(from));
	eXosip_unlock();
	return i;
}


int
phAcceptCall2(int cid, void *userData)
{
	return phAcceptCall3(cid, userData, PH_STREAM_AUDIO);
}


int
phAcceptCall3(int cid, void *userData, int streams)
{
	int i;
	phcall_t *ca = ph_locate_call_by_cid(cid);
	phCallStateInfo_t info;
	char *remoteUri = 0;
	char  local_video_port[16];
	char  local_voice_port[16];
	/* <UOLFONE> */
	char  audio_rtp_tunnel_out_host[HTTP_TUNNEL_HOST_MAX_LEN + 1];
	int   audio_rtp_tunnel_out_host_len = HTTP_TUNNEL_HOST_MAX_LEN + 1;
	int   audio_rtp_tunnel_out_port = 0;
	char  audio_rtcp_tunnel_out_host[HTTP_TUNNEL_HOST_MAX_LEN + 1];
	int   audio_rtcp_tunnel_out_host_len = HTTP_TUNNEL_HOST_MAX_LEN + 1;
	int   audio_rtcp_tunnel_out_port = 0;
	int rtp_use_tunnel = phcfg.rtp_use_tunnel;
	void* audio_rtp_tunnel = NULL;
	void* audio_rtcp_tunnel = NULL;
	int port_len;
	char* remote_audio_host = NULL;
	int   remote_audio_port = 0;
	/* </UOLFONE> */
#ifdef STUN_ENABLE
	char  public_voice_port[16];
	char  public_video_port[16];

	public_voice_port[0] = 0;
	public_video_port[0] = 0;
#endif

	local_video_port[0] = 0;
	local_voice_port[0] = 0;

	//streams = PH_STREAM_AUDIO;
	DBG_SIP_NEGO("SIP NEGO: phAcceptCall3\n");
	if (!ca) {
		return -PH_BADCID;
	}

	ca->user_mflags = streams; // trace of what the user decided
	ca->nego_mflags = ca->user_mflags; // current negociated media flags

	if (_is_video_enabled(streams))
	{
		_get_local_video_sdp_port(local_video_port);
	}

	_get_local_audio_sdp_port(local_voice_port);

	/* <UOLFONE> */
	if ((rtp_use_tunnel) &&
		(ca->remote_sdp_audio_ip != NULL))
	{
		audio_rtp_tunnel_out_host[0] = '\0';
		audio_rtp_tunnel = ph_media_tunnel_open_connected(ca->remote_sdp_audio_ip, ca->remote_sdp_audio_port,
				audio_rtp_tunnel_out_host, &audio_rtp_tunnel_out_host_len, &audio_rtp_tunnel_out_port);

		if (audio_rtp_tunnel == NULL)
		{
			return -3;
		}

		audio_rtp_tunnel_out_host[HTTP_TUNNEL_HOST_MAX_LEN] = '\0';

		audio_rtcp_tunnel_out_host[0] = '\0';
		audio_rtcp_tunnel = ph_media_tunnel_open_connected(ca->remote_sdp_audio_ip, ca->remote_sdp_audio_port + 1,
				audio_rtcp_tunnel_out_host, &audio_rtcp_tunnel_out_host_len, &audio_rtcp_tunnel_out_port);

		if (audio_rtcp_tunnel == NULL)
		{
			ph_media_tunnel_close(audio_rtp_tunnel);
			ph_media_tunnel_free(audio_rtp_tunnel);

			return -3;
		}

		audio_rtcp_tunnel_out_host[HTTP_TUNNEL_HOST_MAX_LEN] = '\0';

		// TODO: Handle video stream
		port_len = snprintf(local_voice_port, 16, "%d", audio_rtp_tunnel_out_port);
		if (port_len > 0)
		{
			local_voice_port[port_len] = '\0';
		}

		eXosip_set_mediaip(audio_rtp_tunnel_out_host);
		eXosip_set_media_use_tunnel(TRUE);
	}
	else
	{
		eXosip_set_mediaip(phcfg.local_media_ip);
		eXosip_set_media_use_tunnel(FALSE);
	}
	/* </UOLFONE> */

#ifdef STUN_ENABLE
	DBG_SIP_NEGO(" phAcceptCall3 nattype: %s \n", eXosip_get_nattype());

	if (0!=strncasecmp(eXosip_get_nattype(), "none", 4) && (0!=strncasecmp(eXosip_get_nattype(), "sym", 3) ))
	{
		int ret = getPublicPort(local_voice_port, local_video_port, public_voice_port , public_video_port );
		if (ret <1)
		{
			DBG_SIP_NEGO("unable to alocate public port ...");
			return;
		}

		eXosip_lock();
		DBG_SIP_NEGO("STUN ports (a.local=%s, a.public=%s) (v.local=%s, v.public=%s)\n",local_voice_port,public_voice_port,local_video_port,public_video_port);
		i = eXosip_answer_call(ca->did, 200, local_voice_port, ph_get_call_contact(ca), optional(local_video_port) , optional(public_voice_port) , optional(public_video_port));
	}
	else
	{ // start ifdef'ed else clause
#endif


    eXosip_lock();
    DBG_SIP_NEGO("NO STUN ports (a.local=%s, a.public=%s) (v.local=%s, v.public=%s)\n",local_voice_port,0,local_video_port,0);
    i = eXosip_answer_call(ca->did, 200, local_voice_port, ph_get_call_contact(ca), optional(local_video_port), 0, 0);


#ifdef STUN_ENABLE
	} // end ifdef'ed else clause
#endif

	if (i == 0)
	{
		i = ph_call_retrieve_payloads(ca, NULL, streams | PH_STREAM_CNG );

		ca->local_sdp_audio_port = atoi(local_voice_port);
		if (_is_video_enabled(streams))
		{
			ca->local_sdp_video_port = atoi(local_video_port);
		}

	}

	eXosip_unlock();

    /* <UOLFONE> */
    if (rtp_use_tunnel)
    {
	  if (!ca->mses)
	  {
		ca->mses = ph_msession_alloc();
	  }
	  
	  if (ca->mses)
	  {
	    (ca->mses->streams)[PH_MSTREAM_AUDIO1].tunRtp = audio_rtp_tunnel;
		(ca->mses->streams)[PH_MSTREAM_AUDIO1].tunRtcp = audio_rtcp_tunnel;
	  }
    }
    /* </UOLFONE> */

	if (!i)
	{
		i = ph_call_media_start(ca, NULL, 0);
	}

	if (i)
	{
		return i;
	}



	eXosip_lock();
	eXosip_retrieve_from(ca->did, &remoteUri);
	eXosip_unlock();

	clear(info);

	info.u.remoteUri = remoteUri;
	info.event = phCALLOK;

	if (phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = NULL;
		info.textinfo = NULL;
		info.status_code = 0;
		info.error_source = 0;
/* </UOLFONE> */
		phcb->callProgress(cid, &info);
	}

	// Fire the call connected event
	owplFireCallEvent(cid,
		CALLSTATE_CONNECTED,
		CALLSTATE_CONNECTED_ACTIVE,
		remoteUri,
		0);

	if (remoteUri)
	{
		osip_free(remoteUri);
	}

	return 0;
}


int
ph_answer_request(int did, int reason, char *ctct)
{
	int i;

	eXosip_lock();
	i = eXosip_answer_call(did, reason, 0, ctct, 0, 0, 0);
	eXosip_unlock();

	return i;
}


int
phRejectCall(int cid, int reason)
{
	int i;
	phcall_t *ca = ph_locate_call_by_cid(cid);
	phCallStateInfo_t info;

	if (!ca)
	{
		return -PH_BADCID;
	}

	i = ph_answer_request(ca->did, reason, ph_get_call_contact(ca));

	clear(info);
	info.event = phCALLCLOSED;
	if (phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = NULL;
		info.textinfo = NULL;
		info.status_code = 0;
		info.error_source = 0;
/* </UOLFONE> */
		phcb->callProgress(cid, &info);
	}
	owplFireCallEvent(cid,
		CALLSTATE_DISCONNECTED,
		CALLSTATE_DISCONNECTED_NORMAL,
		0,
		0);

	DBG_SIP_NEGO("release calls");
	ph_release_call(ca);

	return i;

}


int
phRingingCall(int cid)
{
	int i;
	phcall_t *ca = ph_locate_call_by_cid(cid);

	if (!ca)
	{
		return -PH_BADCID;
	}

	i = ph_answer_request(ca->did, 180, ph_get_call_contact(ca));

	owplFireCallEvent(cid,
		CALLSTATE_ALERTING,
		CALLSTATE_ALERTING_NORMAL,
		0,
		0);

	return i;

}



int
phCloseCall(int cid)
{
	int i;
	phcall_t *ca = ph_locate_call_by_cid(cid);
	phCallStateInfo_t info;
	int did;
	int extern_cid = -1;

	DBG_SIP_NEGO("phCloseCall %d\n", cid);
	clear(info);
	info.event = phCALLCLOSED;

	if (!ca)
	{
		return -PH_BADCID;
	}

	if (ca->isringing)
	{
		info.event = phCALLCLOSEDandSTOPRING;
		ca->isringing = 0;
	}


	info.vlid = ca->vlid;
	did = ca->did;
	extern_cid = ca->extern_cid;

	DBG_SIP_NEGO("release calls");
	ph_release_call(ca);

	eXosip_lock();
	i = eXosip_terminate_call(extern_cid, did);
	eXosip_unlock();



	if (i)
	{
		return i;
	}

	info.userData = 0;

	info.u.errorCode = 0;
	if (phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = NULL;
		info.textinfo = NULL;
		info.status_code = 0;
		info.error_source = 0;
/* </UOLFONE> */
		phcb->callProgress(cid, &info);
	}

	owplFireCallEvent(cid,
		CALLSTATE_DISCONNECTED,
		CALLSTATE_DISCONNECTED_NORMAL,
		0,
		0);

	return i;
}



int
phBlindTransferCall(int cid, const char *uri)
{
	int i;
	phcall_t *ca = ph_locate_call_by_cid(cid);

	if (!ca)
	{
		return -PH_BADCID;
	}

	if (!nonempty(uri))
	{
		return -PH_BADARG;
	}

	if (ph_find_matching_vline(uri, PHM_IGNORE_PORT))
	{
		return -PH_REDIRLOOP;
	}


	ca->localrefer = 1;
	phHoldCall(cid);

	eXosip_lock();
	i = eXosip_transfer_call(ca->did, (char *)uri);
	eXosip_unlock();

	return i;

}


int
phTransferCall(int cid, int tcid)
{
	phcall_t *ca = ph_locate_call_by_cid(cid);
	phcall_t *txca = ph_locate_call_by_cid(tcid);
	int i;


	if (!ca || !txca)
	{
		return -PH_BADCID;
	}

	ca->localrefer = 1;
	ca->txcid = tcid;

	eXosip_lock();
	i = eXosip_transfer_call_to(ca->did, txca->did);
	eXosip_unlock();


	return i;
}

 int
phSetContact(int vlid, const char *uri)
{
	phVLine *vl;


	if (!vlid)
	{
		/* set contact for all VL */
		strncpy(vcontact, uri, sizeof(vcontact));
		return 0;
	}


	vl = ph_valid_vlid(vlid);

	if (!vl)
	{
		return -PH_BADVLID;
	}

	if (vl->contact)
	{
		osip_free(vl->contact);
	}

	{
		char contact[256];
		eXosip_guess_contact_uri(uri, contact, sizeof(contact), 1);
		vl->contact = osip_strdup(contact);
	}
	return 0;
}



int
phConf(int cid1, int cid2)
{
	phcall_t *ca1 = ph_locate_call_by_cid(cid1);
	phcall_t *ca2 = ph_locate_call_by_cid(cid2);

	if(!ca1 || !ca2)
	{
		return -PH_BADCFID;
	}

	if (0 > ph_msession_conf_start(ca1->mses, ca2->mses, phcfg.audio_dev))
	{
		return PH_NORESOURCES;
	}
	else
	{
		return 0;
	}
}

int
phStopConf(int cid1, int cid2)
{
	phcall_t *ca1 = ph_locate_call_by_cid(cid1);
	phcall_t *ca2 = ph_locate_call_by_cid(cid2);

	if(!ca1 || !ca2)
	{
		return -PH_BADCFID;
	}

	if( 0 > ph_msession_conf_stop(ca1->mses, ca2->mses))
	{
		return PH_NORESOURCES;
	}
	else
	{
		return 0;
	}
}

#define CONF_MODE 1


int
phResumeCall(int cid)
{
	phcall_t *ca = ph_locate_call_by_cid(cid);
	int i;

	DBG_SIP_NEGO("phResumeCall:begin for callId: %d\n", cid);
    
	if (!ca)
	{
		return -PH_BADCID;
	}

	// a call that is not held locally cannot be resumed locally
	if (!ca->localhold)
	{
		return -PH_HOLDERR;
	}

#ifndef CONF_MODE
	if (ph_has_active_calls())
	{
		return -PH_NORESOURCES;
	}
#endif

	ca->localhold = 0;
    // the local peer starts a resume process on the call
	ca->localresume = 1;

	eXosip_lock();
	i = eXosip_off_hold_call(ca->did, 0, 0);
	eXosip_unlock();

	// if eXosip failed to put the call off hold, revert the changes on the call
	if(i != 0) {
		ca->localhold = 1;
		ca->localresume = 0;
	}

	DBG_SIP_NEGO("phResumeCall:end for callId: %d, local.(hold,resume,refer)=(%d, %d, %d)\n", cid, ca->localhold, ca->localresume, ca->localrefer);
	return i;
}


int
phHoldCall(int cid)
{
	phcall_t *ca = ph_locate_call_by_cid(cid);
	int i;

	DBG_SIP_NEGO("SIP_NEGO: phHoldCall\n");

	// call does not exist
	if (!ca)
	{
		return -PH_BADCID;
	}

	// call hold is already in progress
	if (ca->localhold)
	{
		return -PH_HOLDERR;
	}

	// the local peer starts a call hold process on the call
	ca->localhold = 1;

	// SPIKE_EXOSIP: hold a call
	eXosip_lock();
	i = eXosip_on_hold_call(ca->did);
	eXosip_unlock();

	// if eXosip failed to put the call on hold, revert the change on the call
	if(i != 0) {
		ca->localhold = 0;
	}

	return i;

}


/* <UOLFONE> */
MY_DLLEXPORT int 
phRenegotiateCallMedia(int cid)
{
	phcall_t *ca = ph_locate_call_by_cid(cid);
	int i;

	if (!ca)
	{
		return -PH_BADCID;
	}

	ca->localhold = 1;

	eXosip_lock();
	//if ((ca->cfg) && (ca->cfg->rtp_use_tunnel))
	//{
	//	i = eXosip_off_hold_call();
	//}
	//else
	{
		i = eXosip_off_hold_call(ca->did, 0, 0);
	}
	eXosip_unlock();

	return i;
}
/* </UOLFONE> */


int
phSetFollowMe(const char *uri)
{
	if (!uri)
	{
		ph_follow_me_addr[0] = 0;
	}

	if (ph_find_matching_vline(uri, PHM_IGNORE_PORT))
	{
		return -PH_REDIRLOOP;
	}

	strncpy(ph_follow_me_addr, uri, sizeof(ph_follow_me_addr));
	return 0;


}




int
phSetBusy(int busyFlag)
{
	ph_busyFlag = busyFlag;
	return 0;
}


int
phLineSetFollowMe(int vlid, const char *uri)
{
	phVLine *vl = ph_valid_vlid(vlid);

	if (!vl)
	{
		return -PH_BADVLID;
	}

	if (ph_find_matching_vline(uri, PHM_IGNORE_PORT))
	{
		return -PH_REDIRLOOP;
	}

	if (vl->followme)
	{
		osip_free(vl->followme);
	}

	vl->followme = osip_strdup(uri);

	return 0;

}






int
phLineSetBusy(int vlid, int busyFlag)
{
	phVLine *vl = ph_valid_vlid(vlid);


	if (!vl)
	{
		return -PH_BADVLID;
	}

	vl->busy = busyFlag;
	return 0;
}


int
phAddAuthInfo(const char *username, const char *userid,
			  const char *passwd, const char *ha1,
			  const char *realm)
{
	int ret;

	if (!username)
	{
		return -PH_BADARG;
	}

	if (!userid)
	{
		return -PH_BADARG;
	}

	if (!passwd)
	{
		return -PH_BADARG;
	}

	if (!realm)
	{
		return -PH_BADARG;
	}

	/* <UOLFONE> */
	/* Save authentication info */
	memset(&phcfg.auth_info.username[0], 0, 128);
	memset(&phcfg.auth_info.userid[0], 0, 128);
	memset(&phcfg.auth_info.passwd[0], 0, 128);
	memset(&phcfg.auth_info.ha1[0], 0, 128);
	memset(&phcfg.auth_info.realm[0], 0, 128);
	strncpy(phcfg.auth_info.username, username, 127);
	strncpy(phcfg.auth_info.userid, userid, 127);
	strncpy(phcfg.auth_info.passwd, passwd, 127);
	strncpy(phcfg.auth_info.ha1, ha1, 127);
	strncpy(phcfg.auth_info.realm, realm, 127);
	/* </UOLFONE> */

	eXosip_lock();

	ret = eXosip_add_authentication_info(username, userid, passwd, ha1, realm);

	eXosip_unlock();

	return ret;
}


int
phSendDtmf(int cid, int dtmfEvent, int mode)
{
	phcall_t *ca = ph_locate_call_by_cid(cid);

	if (!ca)
	{
		return -PH_BADCID;
	}

	if (!ph_call_hasaudio(ca))
	{
		return -PH_NOMEDIA;
	}


	return ph_msession_send_dtmf(ca->mses, dtmfEvent, mode);

}





int
phPlaySoundFile(const char *fileName , int loop)
{
	return -1;
}



int
phSendSoundFile(int cid, const char *fileName)
{
	phcall_t *ca = ph_locate_call_by_cid(cid);

	if (!ca)
	{
		return -PH_BADCID;
	}

	if (!ph_call_hasaudio(ca))
	{
		return -PH_NOMEDIA;
	}


	return ph_msession_send_sound_file(ca->mses, fileName);


}




int
phStopSoundFile()
{
	return -1;
}


int
phSetSpeakerVolume(int cid,  int volume)
{
#if 0
	phcall_t *ca = ph_locate_call_by_cid(cid);

	if (!ca)
	{
		return -PH_BADCID;
	}

	return(ph_media_set_spkvol(ca, volume));
#else
	return 0;
#endif
}


int
phSetRecLevel(int cid,  int level)
{
#if 0
	phcall_t *ca = ph_locate_call_by_cid(cid);

	if (!ca)
	{
		return -PH_BADCID;
	}

	return(ph_media_set_recvol(ca, level));
#else
	return 0;
#endif
}

int
phAddVline(const char* username, const char *server, const char*  proxy,  int regTimeout)
{
	return phAddVline2(NULL, username, server, proxy,  regTimeout);
}

/*
scrap the :port part from the host uri
*/
static char *
ph_scrap_port(char *buf, int bufsize, const char *host, int *port)
{

	assert(buf != 0);
	assert(port != 0);

	*port = 0;

	if (!host)
	{
		return 0;
	}

	if (!strchr(host, ':'))
	{
		return host;
	}

	strncpy(buf, host, bufsize);
	host = strchr(buf, ':');
	if (host)
	{
		*( char *) host = 0;
		*port = atoi(host+1);
	}

	return buf;

}

static const char emptystr[] = { 0 };
#define nonull(x) ((x) ? (x) : emptystr)

/**
* Initialize the phoneapi module
*/
eXosip_tunnel_t *phTunnel;

/* <UOLFONE> */
static BOOL eXosip_tunnel_activated = FALSE;

static BOOL eXosip_reset_tunnel = FALSE;
/* </UOLFONE> */


int
phAddVline2(const char *displayname, const char* username, const char *server, const char*  proxy,  int regTimeout)
{
	phVLine *vl;
	int oldTimeout = 0;
	char srvbuf[256];
	char *srv2;
	int port;
	int i;

#ifdef USE_HTTP_TUNNEL
	/* <UOLFONE> */
	/*
	if (phcfg.use_tunnel && phTunnel && !phTunnel->h_tunnel)
	{
		if (ph_tunnel_init2(proxy))
		{
			return -1;
		}

		i = eXosip_init(0, 0, atoi(phcfg.sipport), phTunnel);
		if (i)
		{
			return -1;
		}

		if (phcfg.asyncmode)
		{
			osip_thread_create(20000, ph_api_thread, 0);
		}
	}
	*/
	if ( ( (phcfg.use_tunnel) && (!eXosip_tunnel_activated) ) || 
		( (phcfg.use_tunnel) && (eXosip_reset_tunnel) ) )
	{
		eXosip_tunnel_activated = TRUE;
		
		phIsInitialized = 0;

		eXosip_quit();

		if (eXosip_reset_tunnel)
		{
			eXosip_reset_tunnel = FALSE;
			
			if (phTunnel)
			{
				http_tunnel_close(phTunnel->h_tunnel);
				http_tunnel_clean_up();
				free(phTunnel);
				phTunnel = 0;
			}
		}

		if ( (!phTunnel) || (phTunnel && !phTunnel->h_tunnel) )
		{
			if (ph_tunnel_init2(proxy))
			{
				// Reset Tunnel if it could not be initialized
				eXosip_reset_tunnel = TRUE;
				return -1;
			}
		}

		i = eXosip_init(0, 0, atoi(phcfg.sipport), phTunnel);
		if (i)
		{
			return -1;
		}

		if (phcfg.force_proxy)
		    eXosip_force_proxy(phcfg.proxy);

		{
			char contact[512];
   
			eXosip_guess_contact_uri(phcfg.identity, contact, sizeof(contact), 1);
			eXosip_set_answer_contact(contact);
		}
  
		// register callbacks?
		eXosip_set_mode(EVENT_MODE);

		// Set user agent 
		if (phcfg.user_agent[0])
		{
			eXosip_set_user_agent(phcfg.user_agent);
		}
		else
		{
			const char * ua  = "phapi/eXosip/" PHAPI_VERSION;
			eXosip_set_user_agent(ua);
		}

		// Reset local media IP
		memset(phcfg.local_media_ip, 0, sizeof(phcfg.local_media_ip));
		strncpy(phcfg.local_media_ip, eXosip_get_mediaip(), sizeof(phcfg.local_media_ip));

		ph_payloads_init();

		if (phcfg.asyncmode) 
		{
			ph_asyncmode_thread_stop();
			ph_asyncmode_thread_start();
		}

		phIsInitialized = 1;

		eXosip_lock();

		eXosip_add_authentication_info(phcfg.auth_info.username, phcfg.auth_info.userid, phcfg.auth_info.passwd, phcfg.auth_info.ha1, phcfg.auth_info.realm);

		eXosip_unlock();
	}
	else if ( (!phcfg.use_tunnel) && (eXosip_tunnel_activated) )
	{
		eXosip_tunnel_activated = FALSE;
		
		phIsInitialized = 0;

		if (phTunnel)
		{
			http_tunnel_close(phTunnel->h_tunnel);
			http_tunnel_clean_up();
			free(phTunnel);
			phTunnel = 0;
		}
		
		eXosip_quit();
		
		i = eXosip_init(0, 0, atoi(phcfg.sipport), NULL);
		if (i)
			return -1;
		
		if (phcfg.force_proxy)
		{
		    eXosip_force_proxy(phcfg.proxy);
		}
		
		{
			char contact[512];
			
			eXosip_guess_contact_uri(phcfg.identity, contact, sizeof(contact), 1);
			eXosip_set_answer_contact(contact);
		}
		
		// register callbacks?
		eXosip_set_mode(EVENT_MODE);
		
		// Set user agent
		if (phcfg.user_agent[0])
		{
			eXosip_set_user_agent(phcfg.user_agent);
		}
		else
		{
			const char * ua  = "phapi/eXosip/" PHAPI_VERSION;
			eXosip_set_user_agent(ua);
		}
		
		// Reset local media IP
		memset(phcfg.local_media_ip, 0, sizeof(phcfg.local_media_ip));
		strncpy(phcfg.local_media_ip, eXosip_get_mediaip(), sizeof(phcfg.local_media_ip));
		
		ph_payloads_init();
		
		if (phcfg.asyncmode) 
		{
			ph_asyncmode_thread_stop();
			ph_asyncmode_thread_start();
		}
		
		phIsInitialized = 1;
		
		eXosip_lock();
		
		eXosip_add_authentication_info(phcfg.auth_info.username, phcfg.auth_info.userid, phcfg.auth_info.passwd, phcfg.auth_info.ha1, phcfg.auth_info.realm);
		
		eXosip_unlock();
	}
	/* </UOLFONE> */
#endif

	DBG_SIP_NEGO("AddVline2(dn = %s, un=%s, srv=%s pxy=%s regT=%d)\n", nonull(displayname),
		nonull(username), nonull(server), nonull(proxy), regTimeout);

	srv2 = ph_split_host_port(srvbuf, sizeof(srvbuf), server, &port);

	if (!port)
	{
		port = 5060;
	}

	if (!username)
	{
		username = "";
	}


	vl  = ph_find_matching_vline3(username, srv2, port, 0);

	if (0 < regTimeout && regTimeout < 200)
	{
		regTimeout = 200;
	}

	if (!vl)
	{
		vl = vline_alloc();
		if (!vl)
		{
			return -PH_NORESOURCES;
		}

		vl->username = osip_strdup(username);
	}
	else
	{
		if (vl->proxy)
		{
			osip_free(vl->proxy);
			vl->proxy = 0;
		}

		if (vl->displayname)
		{
			osip_free(vl->displayname);
			vl->displayname = 0;
		}

		if (vl->contact)
		{
			osip_free(vl->contact);
			vl->contact = 0;
		}
		oldTimeout = vl->regTimeout;
	}

	vl->port = port;

	if (nonempty(proxy))
	{
		if (0 == strstr(proxy, "sip:"))
		{
			int l = 15 + strlen(proxy);
			vl->proxy = osip_malloc(l);
			snprintf(vl->proxy, l, "<sip:%s;lr>", proxy);
		}
		else
		{
			vl->proxy = osip_strdup(proxy);
		}
	}

	if (nonempty(srv2) && !vl->server)
	{
		vl->server = osip_strdup(srv2);
	}

	if (nonempty(displayname))
	{
		vl->displayname = osip_strdup(displayname);
	}

	vl->regTimeout = regTimeout;

	if(vcontact[0])
	{
		vl->contact = osip_strdup(vcontact);
	}
	else
	{
		char contact[512];
		char from[512];

		ph_vline_get_from(from, sizeof(from), vl);
		eXosip_guess_contact_uri(from, contact, sizeof(contact), 1);
		vl->contact = osip_strdup(contact);
	}

	vl->LineState = LINESTATE_PROVISIONED;

	if (nonempty(srv2) && (oldTimeout > 0 || regTimeout > 0))
	{
		phvlRegister(ph_vline2vlid(vl));
	}

	return ph_vline2vlid(vl);
}


int
phDelVline(int vlid, int regTimeout)
{
	phVLine *vl;
	phcall_t *ca;

	if (!(vl = ph_valid_vlid(vlid)))
	{
		return -PH_BADVLID;
	}

	if (vl->LineState == LINESTATE_DELETING) {
		return 0;
	}


	/* forbid deletion of the lines which have pending calls */
	for(ca = ph_calls; ca < &ph_calls[PH_MAX_CALLS]; ca++)
	{
		if (ca->vlid == vlid && ca->cid > 0)
		{
			return -PH_VLBUSY;
		}
	}

	if (regTimeout >= 0)
	{
		vl->regTimeout = regTimeout;
	}

	if (vl->LineState == LINESTATE_REGISTERED)
	{
		phvlUnregister(ph_vline2vlid(vl));
	}
	else
	{
		vline_free(vl);
	}

	vl->LineState = LINESTATE_DELETING;

	return 0;
}


int phChangeAudioDevices(const char *devstr)
{
	char *forcedDeviceId;
	/*
	Audio device selection:
	if we have PH_FORCE_AUDIO_DEVICE env var it overrides everything else
	otherwise we try to use the device specified by the UI....
	if UI didn't specify anything we try to use content of PH_AUDIO_DEVICE env var (if it is nonempty)
	and in the last resort we use PortAudio default device
	*/
	forcedDeviceId = getenv("PH_FORCE_AUDIO_DEVICE");

	if (forcedDeviceId && forcedDeviceId[0])
	{
		strncpy(phcfg.audio_dev, forcedDeviceId, sizeof(phcfg.audio_dev));
	}
	else if (devstr && devstr[0])
	{
		strncpy(phcfg.audio_dev, devstr, sizeof(phcfg.audio_dev));
	}
	else
	{
		forcedDeviceId = getenv("PH_AUDIO_DEVICE");
		if (forcedDeviceId && forcedDeviceId[0])
		{
			strncpy(phcfg.audio_dev, forcedDeviceId, sizeof(phcfg.audio_dev));
		}
		else
		{
#if defined(OS_MACOSX)
			strncpy(phcfg.audio_dev, "ca:", sizeof(phcfg.audio_dev));
#else
			strncpy(phcfg.audio_dev, "pa:", sizeof(phcfg.audio_dev));
#endif
		}
	}

	return 0;
}

/*
scan call marked for close and close them while delivering phCALLCLOSED to phApi client
*/
static void
ph_scan_calls()
{
	int i;
	phcall_t *ca;

	ca = ph_calls;
	for (i = PH_MAX_CALLS; i; i--, ca++)
	{
		if (ca->cid > 0 && ca->closereq)
		{
			phCallStateInfo_t info;
			int cid = ca->cid;

			memset(&info, 0, sizeof(info));
			info.vlid = ca->vlid;
			info.event = phCALLCLOSED;

			DBG_SIP_NEGO("release calls");
			ph_release_call(ca);

			if (phcb->callProgress) {
/* <UOLFONE> */
				info.reason_phrase = NULL;
				info.textinfo = NULL;
				info.status_code = 0;
				info.error_source = 0;
/* </UOLFONE> */
				phcb->callProgress(cid, &info);
			}
			owplFireCallEvent(cid,
				CALLSTATE_DISCONNECTED,
				CALLSTATE_DISCONNECTED_NORMAL,
				0,
				0);
		}
	}
}

static char *
ph_get_proxy(const char *userid)
{
	phVLine *vl = ph_find_matching_vline(userid, PHM_IGNORE_PORT);

	if (!vl)
	{
		return "";
	}

	return vl->proxy ? vl->proxy : "";
}


static char *
ph_get_call_contact(phcall_t *ca)
{
	phVLine *vl;
	assert(ca);

	vl = ph_vlid2vline(ca->vlid);
	if (!vl)
	{
		return 0;
	}

	return vl->contact;
}


static int
ph_get_vline_id(const char *userid, const char *altid)
{

	phVLine *vl = ph_find_matching_vline(userid, PHM_IGNORE_PORT);

	if (vl)
	{
		return ph_vline2vlid(vl);
	}

	if (nonempty(altid))
	{
		vl = ph_find_matching_vline(altid, PHM_IGNORE_PORT);
		if (vl)
		{
			return ph_vline2vlid(vl);
		}
	}

	vl = ph_find_matching_vline(userid, PHM_IGNORE_HOST|PHM_IGNORE_PORT);
	if (vl)
	{
		return ph_vline2vlid(vl);
	}


	if (nonempty(altid))
	{
		vl = ph_find_matching_vline(altid, PHM_IGNORE_HOST|PHM_IGNORE_PORT);
		if (vl)
		{
			return ph_vline2vlid(vl);
		}
	}

	return 0;

}

static void setup_payload(const char *ptstring)
{
	char  tmp[64];
	char  num[8];
	ph_media_payload_t  pt;

	DBG_CODEC_LOOKUP("trying to setup codec in eXosip: %s\n", ptstring);
	if (ph_media_supported_payload(&pt, ptstring))
	{
		DBG_CODEC_LOOKUP("...setup accepted : %d - %s/%d\n", pt.number, pt.string, pt.rate);
		snprintf(num, sizeof(num), "%d", pt.number);
		snprintf(tmp, sizeof(tmp), "%d %s/%d/1", pt.number, pt.string, pt.rate);

		eXosip_sdp_negotiation_add_codec(
			osip_strdup(num),
			NULL,
			osip_strdup("RTP/AVP"),
			NULL, NULL, NULL,
			NULL,NULL,
			osip_strdup(tmp));
		return;
	}
	DBG_CODEC_LOOKUP("...setup refused - not found in ortp profile");

}



static void setup_video_payload(const char *ptsring)
{
	char  tmp[64];
	char  num[8];
	ph_media_payload_t  pt;

	if (ph_media_supported_payload(&pt, ptsring))
	{
		snprintf(num, sizeof(num), "%d", pt.number);
		snprintf(tmp, sizeof(tmp), "%d %s/%d/1", pt.number, pt.string, pt.rate);

		eXosip_sdp_negotiation_add_videocodec(osip_strdup(num),
			NULL,
			osip_strdup("RTP/AVP"),
			NULL, NULL, NULL,
			NULL,NULL,
			osip_strdup(tmp));
	}
	else
	{
		DBG_CODEC_LOOKUP("unsupported payload");
	}
}

#ifdef OS_WIN32

static int
wsock_init()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int i;

	wVersionRequested = MAKEWORD(1,1);
	if(i = WSAStartup(wVersionRequested,  &wsaData))
	{
		return -1;
	}
	return 0;
}
#else
#define wsock_init() 0
#endif


static int
ph_debug_init()
{
	const char *dbgstr;

	dbgstr = getenv("PH_DEBUG_LEVEL");
	if (dbgstr)
	{
		phDebugLevel = atoi(dbgstr);
	}

	if (phDebugLevel > 0)
	{
		if (!phLogFileName)
		{
			phLogFileName = getenv("PH_LOG_FILENAME");
		}

		ph_log_file = phLogFileName ? fopen (phLogFileName, "w+") : stdout;

		if (!ph_log_file)
		{
			perror ("phapi: log file");
			return -1;
		}
		TRACE_INITIALIZE (phDebugLevel, ph_log_file);
	}

	return 0;
}


void
ph_avcodec_init()
{
#ifdef PHAPI_VIDEO_SUPPORT
	avcodec_init();
	avcodec_register_all();
	//phcfg.video_config.video_line_configuration = PHAPI_VIDEO_LINE_128KBPS;
#endif
}


void
ph_calls_init()
{
	int i;

	for( i = 0; i < PH_MAX_CALLS; i++)
	{
		ph_calls[i].cid = -1;
		ph_calls[i].extern_cid = -1;
	}
}

int
ph_tunnel_init2(char *sip_proxy)
{
#ifdef USE_HTTP_TUNNEL
	eXosip_tunnel_t *tunnel = NULL;
	int port;
	char *c;
	char buf[256];
	int tunerr;
	/* <UOLFONE> */
	http_tunnel_info_t httpTunnelInfo;
	/* </UOLFONE> */

	if (!(phcfg.use_tunnel & PH_TUNNEL_USE))
	{
		return 0;
	}

	/* <UOLFONE> */
	/*
	http_tunnel_init_host(phcfg.httpt_server, phcfg.httpt_server_port, phcfg.use_tunnel & PH_TUNNEL_SSL);
	http_tunnel_init_proxy(phcfg.http_proxy,phcfg.http_proxy_port,
		phcfg.http_proxy_user, phcfg.http_proxy_passwd);
	*/
	memset(&httpTunnelInfo, 0, sizeof(http_tunnel_info_t));

	strncpy(httpTunnelInfo.http_proxy_host, phcfg.http_proxy, sizeof(httpTunnelInfo.http_proxy_host));
	httpTunnelInfo.http_proxy_port = phcfg.http_proxy_port;
	strncpy(httpTunnelInfo.http_proxy_username, phcfg.http_proxy_user, sizeof(httpTunnelInfo.http_proxy_username));
	strncpy(httpTunnelInfo.http_proxy_password, phcfg.http_proxy_passwd, sizeof(httpTunnelInfo.http_proxy_password));

	strncpy(httpTunnelInfo.tunnel_host, phcfg.httpt_server, sizeof(httpTunnelInfo.tunnel_host));
	httpTunnelInfo.tunnel_port = phcfg.httpt_server_port;
	/* </UOLFONE> */

	tunnel = malloc(sizeof(eXosip_tunnel_t));
	if (tunnel)
	{
		strncpy(buf, sip_proxy, sizeof(buf));
		c = strstr(buf, ":");

		port = 5060;
		if (c)
		{
			*c++ = 0;
			port = atoi(c);
		}
		/* <UOLFONE> */
		strncpy(httpTunnelInfo.dest_host, buf, sizeof(httpTunnelInfo.dest_host));
		httpTunnelInfo.dest_port = port;
		/* </UOLFONE> */

		/* <UOLFONE> */
		/*
		tunnel->h_tunnel = http_tunnel_open(buf, port, HTTP_TUNNEL_VAR_MODE, &tunerr, -1);
		*/
		tunnel->h_tunnel = http_tunnel_open(&httpTunnelInfo, HTTP_TUNNEL_CONNECT_MODE, &tunerr, -1, phcfg.use_tunnel & PH_TUNNEL_SSL, phcfg.tunnel_keep_alive_timeout, -1);
		/* </UOLFONE> */
		if (!tunnel->h_tunnel)
		{
			if (!tunnel->h_tunnel)
			{
				free(tunnel);
				return -PH_NOTUNNEL;
			}
		}

		tunnel->tunnel_recv = http_tunnel_recv;
		tunnel->tunnel_send = http_tunnel_send;
		tunnel->get_tunnel_socket = http_tunnel_get_socket;
		phTunnel = tunnel;
		return 0;
	}

	return -PH_NORESOURCES;
#else
	return 0;
#endif
}

int
ph_tunnel_init()
{
#ifdef USE_HTTP_TUNNEL
	eXosip_tunnel_t *tunnel = NULL;
	int port;
	char *c;
	char buf[256];
	int tunerr;
	/* <UOLFONE> */
	http_tunnel_info_t httpTunnelInfo;
	/* </UOLFONE> */

	if (!(phcfg.use_tunnel & PH_TUNNEL_USE))
	{
		return 0;
	}

	if(!phcfg.httpt_server_port) {
		if(!phcfg.http_proxy) {
			phcfg.httpt_server_port = 80;
		} else {
			phcfg.httpt_server_port = 443;
		}
	}

	/* <UOLFONE> */
	/*
	http_tunnel_init_host(phcfg.httpt_server, phcfg.httpt_server_port, phcfg.use_tunnel & PH_TUNNEL_SSL);
	http_tunnel_init_proxy(phcfg.http_proxy,phcfg.http_proxy_port,
		phcfg.http_proxy_user, phcfg.http_proxy_passwd);
	*/
	memset(&httpTunnelInfo, 0, sizeof(http_tunnel_info_t));

	strncpy(httpTunnelInfo.http_proxy_host, phcfg.http_proxy, sizeof(httpTunnelInfo.http_proxy_host));
	httpTunnelInfo.http_proxy_port = phcfg.http_proxy_port;
	strncpy(httpTunnelInfo.http_proxy_username, phcfg.http_proxy_user, sizeof(httpTunnelInfo.http_proxy_username));
	strncpy(httpTunnelInfo.http_proxy_password, phcfg.http_proxy_passwd, sizeof(httpTunnelInfo.http_proxy_password));

	strncpy(httpTunnelInfo.tunnel_host, phcfg.httpt_server, sizeof(httpTunnelInfo.tunnel_host));
	httpTunnelInfo.tunnel_port = phcfg.httpt_server_port;
	/* </UOLFONE> */

	tunnel = malloc(sizeof(eXosip_tunnel_t));
	if (tunnel)
	{
		strncpy(buf, phcfg.proxy, sizeof(buf));
		c = strstr(buf, ":");

		port = 5060;
		if (c)
		{
			*c++ = 0;
			port = atoi(c);
		}
		/* <UOLFONE> */
		strncpy(httpTunnelInfo.dest_host, buf, sizeof(httpTunnelInfo.dest_host));
		httpTunnelInfo.dest_port = port;
		/* </UOLFONE> */

		/* <UOLFONE> */
		/*
		tunnel->h_tunnel = http_tunnel_open(buf, port, HTTP_TUNNEL_VAR_MODE, &tunerr, -1);
		*/
		tunnel->h_tunnel = http_tunnel_open(&httpTunnelInfo, HTTP_TUNNEL_CONNECT_MODE, &tunerr, -1, phcfg.use_tunnel & PH_TUNNEL_SSL, phcfg.tunnel_keep_alive_timeout, -1);
		/* </UOLFONE> */
		if (!tunnel->h_tunnel)
		{
			if (!tunnel->h_tunnel)
			{
				free(tunnel);
				return -PH_NOTUNNEL;
			}
		}

		tunnel->tunnel_recv = http_tunnel_recv;
		tunnel->tunnel_send = http_tunnel_send;
		tunnel->get_tunnel_socket = http_tunnel_get_socket;
		phTunnel = tunnel;
		return 0;
	}

	return -PH_NORESOURCES;
#else
	return 0;
#endif
}

void
ph_nat_init()
{
	char *ntstr = 0;

	ph_nat_router_addr[0] = 0;
	ph_nat_port_str[0] = 0;
	ph_nat_type_str[0] = 0;

	if (!phcfg.use_tunnel && phcfg.nattype[0])
	{
		if (!strncasecmp(phcfg.nattype, "auto", 4))
		{
			/* <UOLFONE> */
			if (!eXosip_is_public_address(eXosip_get_mediaip()))
			{
				ntstr = "sym";
			}
			/* </UOLFONE> */

			/* <UOLFONE> */
			/*
			NatType ntype;
			int resPort = 0, hairpin = 0;
			int needMappedAddress = 0;
			StunAddress4 stunServerAddr;

			ntstr = "sym";

			stunParseServerName(phcfg.stunserver, &stunServerAddr);
			ph_nat_type = ntype = stunNatType( &stunServerAddr,  phDebugLevel > 1, &resPort, &hairpin,
				0, 0);

			switch (ntype)
			{
			case StunTypeOpen:
				ntstr = 0;
				break;

			case StunTypeConeNat:
				ntstr = "fcone";
				needMappedAddress = 1;
				break;

			case StunTypeRestrictedNat:
				ntstr = "rcone";
				needMappedAddress = 1;
				break;

			case StunTypePortRestrictedNat:
				ntstr = "prcone";
				needMappedAddress = 1;
				break;

			case StunTypeSymFirewall:
			case StunTypeSymNat:
				needMappedAddress = 1;
				break;

			case StunTypeFailure:
			case StunTypeUnknown:
			case StunTypeBlocked:
			default:
				break;
			}

			if (needMappedAddress)
			{
				StunAddress4 mappedAddr;
				Socket  sock;

				sock = stunOpenSocket(&stunServerAddr, &mappedAddr, atoi(_get_local_sip_port()), NULL, 0);

				if (-1 !=  (int) sock)
				{
					ipv4tostr(ph_nat_router_addr, mappedAddr);
					snprintf(ph_nat_sip_port_str, sizeof(ph_nat_sip_port_str), "%d", mappedAddr.port);
					stunCloseSocket(sock);
				}
			}
			*/
			/* </UOLFONE> */

		}
		else if (!strncasecmp(phcfg.nattype, "fcone", 5) ||
			!strncasecmp(phcfg.nattype, "rcone", 5) ||
			!strncasecmp(phcfg.nattype, "prcone", 6) ||
			!strncasecmp(phcfg.nattype, "sym", 3))
		{
			ntstr = phcfg.nattype;
		}
	}
	else if (phcfg.use_tunnel)
	{
		ntstr = "open";
	}

	if (ntstr)
	{
		eXosip_set_nattype(ntstr);
		if (!phcfg.nat_refresh_time)
		{
			phcfg.nat_refresh_time = 15;
		}

		strncpy(ph_nat_type_str, ntstr, sizeof(ph_nat_type_str));
		if (ph_nat_router_addr[0] && strcmp(ntstr, "sym"))
		{
			eXosip_set_mediaip(ph_nat_router_addr);
			eXosip_set_firewallip(ph_nat_router_addr);
			eXosip_set_firewallport(ph_nat_sip_port_str);
		}
	}
	else
	{
		phcfg.nat_refresh_time = 0;
		strcpy(ph_nat_type_str, "open");
	}

}

/**
* @brief initialize the payload/codecs that are allowed to be handled by the SIP stack
*/
void
ph_payloads_init()
{
	/* reset all payload to fit application capabilities */
	eXosip_sdp_negotiation_remove_audio_payloads();
	eXosip_sdp_negotiation_remove_video_payloads();

	// init payload/codecs for VIDEO

#ifdef PHAPI_VIDEO_SUPPORT
	//setup_video_payload(CODEC_H263P_MIME_STRING"/90000");
	setup_video_payload(CODEC_H263_MIME_STRING"/90000");
	setup_video_payload(CODEC_FLV1_MIME_STRING"/90000");
#if 0
	if (!phcfg.video_codecs[0])
	{
		setup_video_payload(CODEC_H263_MIME_STRING"/90000");
		setup_video_payload(CODEC_H264_MIME_STRING"/90000");
	}
	else
	{
		char tmp[32];
		char *tok = strtok(phcfg.video_codecs, ",");

		while(tok) {
			snprintf(tmp, sizeof(tmp), "%s/90000", tok);
			setup_video_payload(tmp);
			DBG_CODEC_LOOKUP("phapi: added video codec: %s\n",tmp);
			tok = strtok(0, ",");
		}
	}
#endif
#endif

	// init payload/codecs for AUDIO

	// add codecs out of an ENV var
	{
		char *aclist = getenv("PH_AUDIO_CODECS");
		if (aclist)
		{
			strncpy(phcfg.audio_codecs, aclist, sizeof(phcfg.audio_codecs));
		}

	}


	// limit codecs to G711 codecs according to compile time DEFINE
#ifdef G711_ONLY
	strcpy(phcfg.audio_codecs, "PCMU/8000,PCMA/8000");
#endif

	// if at this stage, no codecs are required, fix a default list
	if (!phcfg.audio_codecs[0])
	{
		setup_payload("PCMU/8000");
		setup_payload("PCMA/8000");
		setup_payload("GSM/8000");
#ifdef ENABLE_ILBC
		setup_payload("ILBC/8000");
#endif
		setup_payload("SPEEX/16000");
		setup_payload("SPEEX/8000");
		setup_payload("AMR/8000");
		setup_payload("AMR-WB/16000");
		/* <UOLFONE> */
		setup_payload("G723");
		/* </UOLFONE> */

	}
	// phapi.h client has required a specific list of codecs
	else
	{
		/*
		The list is "," separated
		some hacks are present to allow for :
		- payload usurpation
		- default /8000 clockrate when not specified
		- ... look at the code
		*/

		char tmp[32];
		char *acodecs_bak;
		char *tok;
		
		acodecs_bak = strdup(phcfg.audio_codecs);
		tok = strtok(phcfg.audio_codecs, ",");

		while(tok)
		{
			if(!strcmp(tok, "AMR-WB"))
			{
#ifdef AMR_OVER_G729_HACK
				snprintf(tmp, sizeof(tmp), "G729/8000");
#else
				snprintf(tmp, sizeof(tmp), "%s/16000", tok);
#endif
			}
#ifdef SPEEX_OVER_G729_HACK
			else if(!strcmp(tok, "SPEEX/16000"))
			{
				snprintf(tmp, sizeof(tmp), "G729/8000");
			}
#endif
			else if (strchr(tok, '/'))
			{
				strncpy(tmp, tok, sizeof(tmp));
			}
			else
			{
				snprintf(tmp, sizeof(tmp), "%s/8000", tok);
			}

			if (ph_media_can_handle_payload(tmp))
			{
				setup_payload(tmp);
			}

			tok = strtok(0, ",");
		}

		if (acodecs_bak)
		{
			strncpy(phcfg.audio_codecs, acodecs_bak, sizeof(phcfg.audio_codecs));
			free(acodecs_bak);
		}
	}

	// set codec in sip stack for CNG (confort noise generator=
	if(phcfg.cng)
	{
		setup_payload("CN/8000");
	}

	// set codec in sip stack for DTMF
	setup_payload("telephone-event/8000");
}

static int ph_tunnel_init2(char *sip_proxy);

void DEBUGTRACE(const char * mess)
{
	if(phcb && phcb->debugTrace)
	{
		phcb->debugTrace(mess);
	}
	owplReportLogMessage(mess);
}

// SPIKE_SRTP: enable / disable cipher mode using environnment variable
/**
 * @brief enable / disable cipher mode using environnment variable
 */
static void
ph_cipher_init()
{
  char *cipherMode_str = getenv("SVOIP_PHAPI_CIPHERMODE");

  fprintf(stdout,"sVoIP cipherMode_str = %s\n", cipherMode_str);
  if (cipherMode_str == NULL)
    {
      sVoIP_phapi_setCipherMode(0);
    }
  else
    {
      if (!strcmp(cipherMode_str, "NULL"))
	{
	  fprintf(stdout,"sVoIP will not ciphered\n");
	  sVoIP_phapi_setCipherMode(0);
	}
      if (!strcmp(cipherMode_str, "SRTP"))
	{
	  fprintf(stdout,"sVoIP uses SRTP\n");
	  sVoIP_phapi_setCipherMode(1);
	}
    }
}

int
phInit(phCallbacks_t *cbk, char * server, int asyncmode)
{
	int i;
	char buf[200];

  ph_cipher_init();

  memset(vcontact, 0, sizeof(vcontact));

	i = ph_debug_init();
	if (i)
	{
		return i;
	}

	ph_avcodec_init();
	ph_calls_init();

	/* <UOLFONE> - initialize tunnel in phAddVline2() */
	/*
	if (phcfg.use_tunnel)
	{
		i = ph_tunnel_init();
		if (i)
		{
			return i;
		}
	}
	*/
	/* </UOLFONE> */

	if (phcfg.proxy[0])
	{
		char tmp[256];
		if (!strchr(phcfg.proxy, '<'))
		{
			snprintf(tmp, sizeof(tmp), "<sip:%s;lr>", phcfg.proxy);
			strncpy(phcfg.proxy, tmp, sizeof(phcfg.proxy));
		}
	}

#ifdef FORCE_VAD
	/* HACK for test */
#ifdef EMBED
	phcfg.vad = VAD_VALID_MASK | (500 & VAD_THRESHOLD_MASK);
#else
	phcfg.vad = VAD_VALID_MASK | (1000 & VAD_THRESHOLD_MASK);
#endif
#endif

#ifdef FORCE_CNG
	/* HACK for test */
	phcfg.cng = 1;
#endif

	/* <UOLFONE> */
	/*
	ph_media_init(phcfg.use_tunnel, phcfg.plugin_path);
	*/
	ph_media_init(phcfg.rtp_use_tunnel, phcfg.plugin_path);

	if (phcfg.uol_library_search_path[0] != '\0')
	{
		eXosip_set_uol_library_search_path(phcfg.uol_library_search_path);
	}

	if (phcfg.tunnel_keep_alive_timeout <= 0)
	{
		phcfg.tunnel_keep_alive_timeout = max(phcfg.nat_refresh_time * 2, 60);
	}

	if (phcfg.rtp_tunnel_connect_keep_alive_timeout <= 0)
	{
		phcfg.rtp_tunnel_connect_keep_alive_timeout = 30;
	}

	if (phcfg.rtp_tunnel_accept_keep_alive_timeout <= 0)
	{
		phcfg.rtp_tunnel_accept_keep_alive_timeout = 5 * 60;
	}

	if (phcfg.rtp_tunnel_connection_accept_timeout <= 0)
	{
		phcfg.rtp_tunnel_connection_accept_timeout = 5 * 60;
	}
	/* </UOLFONE> */

	i = eXosip_init(0, 0, atoi(phcfg.sipport), phTunnel);

	if (i)
	{
		return -1;
	}

	{
		/* <UOLFONE> */
		/*
		const char * ua  = WENGOPHONE_UA;
		eXosip_set_user_agent(ua);
		*/
		if (phcfg.user_agent[0])
		{
			eXosip_set_user_agent(phcfg.user_agent);
		}
		else
		{
			const char * ua  = UOLFONECLIENT_UA;
			eXosip_set_user_agent(ua);
		}
		/* </UOLFONE> */
	}

	ph_nat_init();

	DBG_SIP_NEGO("NAT type: %s fw=%s \n", ph_nat_type_str, ph_nat_router_addr);

	if (phcfg.force_proxy)
	{
		eXosip_force_proxy(phcfg.proxy);
	}

	{
		char contact[512];

		eXosip_guess_contact_uri(phcfg.identity, contact, sizeof(contact), 1);
		eXosip_set_answer_contact(contact);
	}

	/* <UOLFONE> */
	/* Save local media IP */
	memset(phcfg.local_media_ip, 0, sizeof(phcfg.local_media_ip));
	strncpy(phcfg.local_media_ip, eXosip_get_mediaip(), sizeof(phcfg.local_media_ip));
	/* </UOLFONE> */

	ph_vlines_init();

	ph_payloads_init();

	if (!phcfg.audio_dev || phcfg.audio_dev[0] == '\0')
	{
		// Set default audio device if no one has been set before
		phChangeAudioDevices("");
	}

	/* register callbacks? */
	eXosip_set_mode(EVENT_MODE);
	phcb = cbk;
	phcfg.asyncmode = asyncmode;
	if (!asyncmode)
	{
		phWaitTimeout = 1;
	}
	else
	{
		phWaitTimeout = 500;
	}

	if (asyncmode)
	{
		/* <UOLFONE> */
        //osip_thread_create(20000, ph_api_thread, 0);
		ph_asyncmode_thread_start();
		/* </UOLFONE> */
	}

	ph_media_stop_mutex = g_mutex_new();

	phIsInitialized = 1;

	DEBUGTRACE("PhInit finished\n");

	return 0;
}


int phTunnelConfig(const char* http_proxy, const int http_proxy_port,
				   const char* httpt_server, const int httpt_server_port,
				   const char *proxy_user, const char* proxy_passwd,
				   int use_ssl, int autoconf)
{
	phcfg.httpt_server[0] = 0;
	phcfg.http_proxy[0] = 0;
	phcfg.use_tunnel = 0;

	if (!httpt_server)
	{
		return -1;
	}

	phcfg.http_proxy_port = http_proxy_port;
	if (!httpt_server_port)
	{
		if (!http_proxy)
		{
			phcfg.httpt_server_port = 80;
		}
		else
		{
			phcfg.httpt_server_port = 443;
		}
	}
	else
	{
		phcfg.httpt_server_port = httpt_server_port;
	}

	if (httpt_server)
	{
		strncpy(phcfg.httpt_server, httpt_server, sizeof(phcfg.httpt_server));
	}
	if (http_proxy)
	{
		strncpy(phcfg.http_proxy, http_proxy, sizeof(phcfg.http_proxy));
	}

	if (proxy_user)
	{
		strncpy(phcfg.http_proxy_user, proxy_user, sizeof(phcfg.http_proxy_user));
	}
	if (proxy_passwd)
	{
		strncpy(phcfg.http_proxy_passwd, proxy_passwd, sizeof(phcfg.http_proxy_passwd));
	}

	phcfg.use_tunnel = PH_TUNNEL_USE;
	if (use_ssl)
	{
		phcfg.use_tunnel |= PH_TUNNEL_SSL;
	}
	if (autoconf)
	{
		phcfg.use_tunnel |= PH_TUNNEL_AUTOCONF;
	}
	return 0;
}


/* <UOLFONE> */
int phTunnelProxyConfig(const char* http_proxy, const int http_proxy_port,
                  const char *proxy_user, const char* proxy_passwd)
{
	phcfg.http_proxy[0] = 0;
	phcfg.rtp_use_tunnel = 0;

	if (!http_proxy)
	{
		return -1;
	}

	phcfg.http_proxy_port = http_proxy_port;

	if (http_proxy)
	{
		strncpy(phcfg.http_proxy, http_proxy, sizeof(phcfg.http_proxy));
	}
	if (proxy_user) 
	{
		strncpy(phcfg.http_proxy_user, proxy_user, sizeof(phcfg.http_proxy_user));
	}
	if (proxy_passwd)
	{
		strncpy(phcfg.http_proxy_passwd, proxy_passwd, sizeof(phcfg.http_proxy_passwd));
	}

	return 0;
}

int phMediaTunnelConfig(const char* httpt_server, const int httpt_server_port,
                  int use_ssl, int autoconf)
{
	phcfg.rtp_httpt_server[0] = 0;
	phcfg.rtp_use_tunnel = 0;

	if (!httpt_server)
	{
		return -1;
	}

	if (0 == httpt_server_port)
	{
		phcfg.rtp_httpt_server_port = 80;
	}
	else
	{
		phcfg.rtp_httpt_server_port = httpt_server_port;
	}

	if (httpt_server)
	{
		strncpy(phcfg.rtp_httpt_server, httpt_server, sizeof(phcfg.rtp_httpt_server));
	}

	phcfg.rtp_use_tunnel = PH_TUNNEL_USE;
	if (use_ssl)
	{
		phcfg.rtp_use_tunnel |= PH_TUNNEL_SSL;
	}
	if (autoconf)
	{
		phcfg.rtp_use_tunnel |= PH_TUNNEL_AUTOCONF;
	}
	return 0;
}
/* </UOLFONE> */

/**
* terminate ph api
*/
void
phTerminate()
{
	int i;

	DBG_SIP_NEGO("SIP NEGO: phTerminate\n");
	if (!phIsInitialized)
	{
		return;
	}


	for(i = 0; i < PH_MAX_CALLS; i++)
		if (ph_calls[i].cid != -1 && ph_calls[i].extern_cid != -1)
		{
			DBG_SIP_NEGO("release calls");
			ph_release_call(&ph_calls[i]);
		}



		for(i = 0; i < PH_MAX_VLINES; i++)
		{
			phDelVline(i+1, -1);
		}



		usleep(200000);

		phPoll();

		phIsInitialized = 0;

		eXosip_quit();

		/* <UOLFONE> */
		ph_asyncmode_thread_stop();
		/* </UOLFONE> */

#ifdef USE_HTTP_TUNNEL
		if (phTunnel)
		{
			http_tunnel_close(phTunnel->h_tunnel);
			http_tunnel_clean_up();
			free(phTunnel);
			phTunnel = 0;
		}
#endif

		ph_media_cleanup();




		if (phLogFileName && phDebugLevel > 0)
		{
			fclose(ph_log_file);
		}
		if (phDebugLevel > 0)
		{
			for (i = 0; i <= phDebugLevel && i < END_TRACE_LEVEL; ++i)
			{
				TRACE_DISABLE_LEVEL(i);
			}
		}

  /* <UOLFONE> - Fix memory leak */
  g_mutex_free(ph_media_stop_mutex);
  /* </UOLFONE> */



}



/**
* poll for phApi events.c
*/
int
phPoll()
{
	if (!phIsInitialized)
	{
		return -1;
	}

	if (!phcfg.asyncmode)
	{
		if (ph_event_get() == -2)
		{
			return -2;
		}

		ph_keep_refreshing();
	}
	return 0;
}

void ph_refer_notify(int did, int status, const char* msg, int final)
{
	char  statusmsg[128];

	snprintf(statusmsg, sizeof(statusmsg), "SIP/2.0 %d %s", status, msg);

	eXosip_lock();

	eXosip_transfer_send_notify(did, final ? EXOSIP_SUBCRSTATE_TERMINATED : EXOSIP_SUBCRSTATE_ACTIVE,
		statusmsg);

	eXosip_unlock();

}

static int
ph_call_retrieve_payloads(phcall_t *ca, eXosip_event_t *je, int flags)
{
	int  i = 0;

	DBG_SIP_NEGO("looking for payloads...\n");
	DBG_SIP_NEGO("audio...\n");
	if (_is_audio_enabled(flags))
	{
		i = eXosip_retrieve_negotiated_audio_payload(ca->did, &ca->audio_payload, ca->audio_payload_name, sizeof(ca->audio_payload_name));
		DBG_SIP_NEGO("remote_audio=%s payload=%s(%d)\n", ca->remote_sdp_audio_ip, ca->audio_payload_name, ca->audio_payload);
	}

	DBG_SIP_NEGO("video...\n");
	ca->video_payload = 0;
	if (ca->remote_sdp_video_ip[0] && (_is_video_enabled(flags)))
	{
		i = eXosip_retrieve_negotiated_video_payload(ca->did, &ca->video_payload, ca->video_payload_name, sizeof(ca->video_payload_name));
		DBG_SIP_NEGO("remote_video=%s payload=%s(%d)\n", ca->remote_sdp_video_ip, ca->video_payload_name, ca->video_payload);
	}

	DBG_SIP_NEGO("cng...\n");
	if(!i && phcfg.cng && (flags & PH_STREAM_CNG)) {
		ca->cng = !eXosip_retrieve_negotiated_specific_payload(ca->did, PH_MEDIA_CN_PT_STR, strlen(PH_MEDIA_CN_PT_STR));
		DBG_SIP_NEGO("cng: %d", ca->cng);
	}

	return i;
}


int phCallGetCodecs(int cid, char *audioCodecBuf, int aBufLen, char *videoCodecBuf, int vBufLen)
{
	phcall_t *ca = ph_locate_call_by_cid(cid);


	if (!ca)
	{
		return -PH_BADCID;
	}


	if (audioCodecBuf)
	{
		strncpy(audioCodecBuf, ca->audio_payload_name, aBufLen);
	}


	if (videoCodecBuf)
	{
		strncpy(videoCodecBuf, ca->video_payload_name, vBufLen);
	}


	return 0;


}

static void
ph_parse_payload_mime(struct ph_media_payload_s *pt, const char *mime, int rate, int chans)
{
	const char *rp;
	const char *cp;
	int len;

	rp = strchr(mime, '/');
	strncpy(pt->string, mime, sizeof(pt->string));
	pt->rate = rate;
	pt->chans = chans;

	if (!rp)
	{
		return;
	}

	rp++;
	if (!*rp)
	{
		return;
	}

	pt->rate = atol(rp);

	cp = strchr(rp, '/');
	if (!cp)
	{
		return;
	}

	cp++;
	if (!*cp)
	{
		return;
	}

	pt->chans = atol(cp);
}



static void
ph_call_media_stop(phcall_t *ca)
{
	DBG_SIP_NEGO("ph_call_media_stop\n");

	if (ca->mses)
	{
		ph_msession_stop(ca->mses, phcfg.audio_dev);

		// cf allocation sequence for the ph_msession_s in phmedia.h
		/* <UOLFONE> */
		/*
		g_mutex_free(ca->mses->critsec_mstream_init);
		free(ca->mses);
		*/
		ph_msession_free(ca->mses);
		/* </UOLFONE> */

		ca->mses = 0;
	}

	/* <UOLFONE> */
	return 0;
	/* </UOLFONE> */
}

static int
ph_call_media_suspend(phcall_t *ca, int localhold)
{
	if (ca->mses)
	{
		ph_msession_suspend(ca->mses, PH_MSTREAM_TRAFFIC_IO, phcfg.audio_dev);
	}
	return 0;
}


static int
ph_call_media_resume(phcall_t *ca, int localhold)
{
	if (ca->mses)
	{
		ph_msession_resume(ca->mses, PH_MSTREAM_TRAFFIC_IO, phcfg.audio_dev);
	}

	return 0;


}


/* <UOLFONE> */
void ph_copy_media_payload(ph_media_payload_t* orig_payload, ph_media_payload_t* dest_payload, int is_full_copy)
{
	if ( (orig_payload == NULL) || (dest_payload == NULL) )
	{
		return;
	}
	
	dest_payload->number = orig_payload->number;
	strcpy(dest_payload->string, orig_payload->string);
	dest_payload->rate = orig_payload->rate;
	dest_payload->chans = orig_payload->chans;
	dest_payload->ptime = orig_payload->ptime;
	dest_payload->psize = orig_payload->psize;
	dest_payload->mode = orig_payload->mode;
	
	if (!is_full_copy)
	{
		return;
	}
	
	if (orig_payload->fmtp != NULL)
	{
		dest_payload->fmtp = osip_strdup(orig_payload->fmtp);
	}
}
/* </UOLFONE> */


/**
* @brief used to start/restart the media engine
* this function is used everytime it is necessary to start media sessions
* @param ca phcall_t call that is concerned. will express the needed sessions
* @param je last exosip event that justified the invocation of the function
* @param resumeflag 0/1 is this a resume operation ?
*
* @return 0 - OK / PH_NOMEDIA
*/
static int
ph_call_media_start(phcall_t *ca, eXosip_event_t *je, int resumeflag)
{
	int i = 0;
	struct ph_msession_s *s = NULL, *olds = NULL;
	/* <UOLFONE> */
	int bIsCallAccepted = 0;
	/* </UOLFONE> */

	// cases when the invocation is ignored
	if (phcfg.nomedia || ca->localhold || ca->remotehold)
	{
		return 0;
	}

	// we will work on the media sessions of the given phcall_t
	s = ca->mses;

	// init the ph_msession_s for the call if the call doesn't have one yet
	/* <UOLFONE> */
	/*
	if (!s)
	{
		s = ca->mses = (struct ph_msession_s *)calloc(sizeof(struct ph_msession_s), 1);
		if (!s)
		{
			return -PH_NORESOURCES;
		}
		s->critsec_mstream_init = g_mutex_new();
	}
	*/
	if (!s)
	{
		s = ca->mses = ph_msession_alloc();
		if (!s)
		{
			return -PH_NORESOURCES;
		}
	}
	/* </UOLFONE> */

	s->cbkInfo = ca;


	// we need to understand what is required from the function call
	// by default, nothing to do
	s->newstreams = 0;

	if ( // user accepts video and network accepts video
		(_is_video_enabled(ca->user_mflags))
		&& ca->video_payload
		&& ca->remote_sdp_video_ip[0]
	)
	{
		// negociated flags (user+SDP) need to be IO
		ca->nego_mflags = ca->nego_mflags | PH_STREAM_VIDEO_RX;
		ca->nego_mflags = ca->nego_mflags | PH_STREAM_VIDEO_TX;

		DBG_SIP_NEGO("will have video stream ip: %s payload=%d\n", ca->remote_sdp_video_ip, ca->video_payload);
		DBG_SIP_NEGO("media flags may have changed: user= %d nego=%d\n", ca->user_mflags, ca->nego_mflags);
	}
	else
	{
		// video is not negociated
		// it is necessary to remove VIDEO IO flags
		ca->nego_mflags = ca->nego_mflags & ~PH_STREAM_VIDEO_RX;
		ca->nego_mflags = ca->nego_mflags & ~PH_STREAM_VIDEO_TX;

		DBG_SIP_NEGO("media flags may have changed: user= %d nego=%d\n", ca->user_mflags, ca->nego_mflags);
	}


	if ( _is_video_enabled(ca->nego_mflags) )
	{
		struct ph_mstream_params_s *msp = &s->streams[PH_MSTREAM_VIDEO1];
		int ttype;

		// program activation of VIDEO1
		s->newstreams |= (1 << PH_MSTREAM_VIDEO1);

		// define the traffic type of the stream
		ttype = _is_video_enabled(ca->user_mflags);
		if (ttype == (PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_TX))
		{
			msp->traffictype = PH_MSTREAM_TRAFFIC_IO;
		}
		else if (ttype == PH_STREAM_VIDEO_RX)
		{
			msp->traffictype = PH_MSTREAM_TRAFFIC_IN;
		}
		else if (ttype == PH_STREAM_VIDEO_TX)
		{
			msp->traffictype = PH_MSTREAM_TRAFFIC_OUT;
		}

		/* <UOLFONE> */
		/*
		if (phcfg.use_tunnel)
		{
			msp->flags |= PH_MSTREAM_FLAG_TUNNEL;
		}
		*/
		if (phcfg.rtp_use_tunnel)
		{
			msp->flags |= PH_MSTREAM_FLAG_TUNNEL;
		}
		/* </UOLFONE> */

		msp->localport = ca->local_sdp_video_port;
		msp->remoteport = ca->remote_sdp_video_port;
		if (je)
		{
			strncpy(msp->remoteaddr,
				je->remote_sdp_video_ip,
				sizeof(msp->remoteaddr));
		}
		else
		{
			strncpy(msp->remoteaddr,
				ca->remote_sdp_video_ip,
				sizeof(msp->remoteaddr));
		}

		// define the negociated codec payload on the stream
		msp->opayloads[0].number = ca->video_payload;
		ph_parse_payload_mime(&msp->opayloads[0], ca->video_payload_name, 90000, 1);
		/* <UOLFONE> */
		ph_copy_media_payload(&msp->opayloads[0], &msp->ipayloads[0], FALSE);
		/* </UOLFONE> */

		// define the video callback
		s->frameDisplayCbk =  ph_frame_display_cbk;

		// additional configuration for video
		msp->videoconfig = phcfg.video_config.video_line_configuration;
	} 	//if ( _is_video_enabled(ca->nego_mflags) )


	if ( // audio is enabled
		_is_audio_enabled(ca->nego_mflags)
		&& (!je || je->remote_sdp_audio_ip[0])
		)
	{
		struct ph_mstream_params_s *msp = &s->streams[PH_MSTREAM_AUDIO1];

/* <UOLFONE <pTime adjustment> > */
		int in_call_ptime = 0;
		int out_call_ptime = 0;
		
		// Set to this stream the correct ptime negociated in the SDP
		// If there is no ptime negociated, use the default value (20 ms)
		if ( (!je) && (ca->did > 0) )
		{
			// We are accepting a call. On this case "je == NULL", so 
			// we need to retrive the ptime value if any from the original 
			// SDP package.
			in_call_ptime = eXosip_get_audio_ptime_from_call(je, ca->did, 0, 1);
			out_call_ptime = eXosip_get_audio_ptime_from_call(je, ca->did, 1, 1);
			
			msp->ptime = (out_call_ptime > 0 ? out_call_ptime : atoi(DEFAULT_PTIME_VALUE));
			
			bIsCallAccepted = 1;
		}
		else
		{
			in_call_ptime = eXosip_get_audio_ptime_from_call(je, ca->did, 0, 0);
			out_call_ptime = eXosip_get_audio_ptime_from_call(je, ca->did, 1, 0);
			
			msp->ptime = (out_call_ptime > 0 ? out_call_ptime : atoi(DEFAULT_PTIME_VALUE));
		}
/* </UOLFONE> */

		// program activation of AUDIO1
		s->newstreams |= (1 << PH_MSTREAM_AUDIO1);

		s->dtmfCallback = ph_wegot_dtmf;
		s->endCallback = ph_stream_ended;

		/* <UOLFONE> */
		s->streamReceiveTimeoutCbk = ph_stream_receive_timeout;
		/* </UOLFONE> */

		if (phcfg.vad & 0x80000000)
		{
			msp->flags |= PH_MSTREAM_FLAG_VAD;
			msp->vadthreshold = phcfg.vad & 0x7fff;
		}

		if (phcfg.cng)
		{
			msp->flags |= PH_MSTREAM_FLAG_CNG;
		}

		msp->jitter = phcfg.jitterdepth;

		if (!phcfg.noaec)
		{
			msp->flags |= PH_MSTREAM_FLAG_AEC;
		}

		msp->traffictype = PH_MSTREAM_TRAFFIC_IO;
		/* <UOLFONE> */
		/*
		if (phcfg.use_tunnel)
		{
			msp->flags |= PH_MSTREAM_FLAG_TUNNEL;
		}
		*/
		if (phcfg.rtp_use_tunnel)
		{
			msp->flags |= PH_MSTREAM_FLAG_TUNNEL;
		}
		/* </UOLFONE> */

		msp->localport = ca->local_sdp_audio_port;
		msp->remoteport = ca->remote_sdp_audio_port;
		if (je)
		{
			strncpy(msp->remoteaddr,
				je->remote_sdp_audio_ip,
				sizeof(msp->remoteaddr));
		}
		else
		{
			strncpy(msp->remoteaddr,
				ca->remote_sdp_audio_ip,
				sizeof(msp->remoteaddr));
		}

		// SPIKE_HDX
		if (phcfg.hdxmode == PH_HDX_MODE_MIC)
		{
			msp->flags |= PH_MSTREAM_FLAG_MICHDX;
			msp->vadthreshold = phcfg.vad & 0x7fff;
		}

		if (phcfg.hdxmode == PH_HDX_MODE_SPK)
		{
			msp->flags |= PH_MSTREAM_FLAG_SPKHDX;
			msp->vadthreshold = phcfg.vad & 0x7fff;
		}

/* <UOLFONE <mode adjustment> > */
		ph_clear_msession_streams_fmtps(s);
/* </UOLFONE> */

		msp->opayloads[0].number = ca->audio_payload;
		ph_parse_payload_mime(&msp->opayloads[0], ca->audio_payload_name, 8000, 1);
		/* <UOLFONE <mode adjustment> > */
		ph_copy_media_payload(&msp->opayloads[0], &msp->ipayloads[0], FALSE);
		/* </UOLFONE> */

/* <UOLFONE <mode adjustment> > */
		{
			osip_list_t local_format_modifiers;
			int pos = 0;
			char *format_modifier = NULL;
			
			osip_list_init(&local_format_modifiers);
			
			eXosip_get_media_formats_from_call(je, 
					ca->did, 0, bIsCallAccepted, &local_format_modifiers);
			
			ph_update_media_payloads_packetization_info(
					&local_format_modifiers, msp, in_call_ptime, bIsCallAccepted);
			
			while (!osip_list_eol(&local_format_modifiers, pos))
			{
			  format_modifier = (char*)osip_list_get(&local_format_modifiers, pos);
			  osip_list_remove(&local_format_modifiers, pos);
			}
		}
/* </UOLFONE> */

	} // end 	if ( // audio is enabled

	// take action depending on the streaming configuration
	if (s->newstreams | s->activestreams)
	{
		if (resumeflag)
		{
			if (ph_msession_resume(s, PH_MSTREAM_TRAFFIC_IO, phcfg.audio_dev))
			{
				DBG_SIP_NEGO("SIP_NEGO:ph_call_media_start: just called ph_msession_resume\n");
				i = -PH_NOMEDIA;
			}
		}
		else if (ph_msession_start(s, phcfg.audio_dev))
		{
			DBG_SIP_NEGO("SIP_NEGO:ph_call_media_start: just called ph_msession_start\n");
			i = -PH_NOMEDIA;
		}
	}
	else
	{
		DBG_SIP_NEGO("SIP_NEGO:ph_call_media_start: nothing to start\n");
		i = -PH_NOMEDIA;
	}

	return i;
}


void
ph_call_new(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca;
	phVLine *vl;


	clear(info);
	if (ph_busyFlag)
	{
		ph_answer_request(je->did, 486, 0);
		return;
	}


	if (ph_follow_me_addr[0])
	{
		ph_answer_request(je->did, 302, ph_follow_me_addr);
		return;
	}


	info.vlid = ph_get_vline_id(je->local_uri, je->req_uri);

	if (!info.vlid)
	{
		ph_answer_request(je->did, 404, 0);
		return;
	}

	vl = ph_vlid2vline(info.vlid);

	assert(vl);

	if (vl->busy)
	{
		ph_answer_request(je->did, 486, vl->contact);
		return;
	}


	if (vl->followme && vl->followme[0])
	{
		ph_answer_request(je->did, 302, vl->followme);
		return;
	}


	ca = ph_locate_call(je, 1);

	if (ca)
	{
		ca->vlid = info.vlid;
		ph_build_cname(ca->cname, sizeof(ca->cname), ph_vlid2vline(ca->vlid));

		info.userData = je->external_reference;
		info.event = phINCALL;
		info.u.remoteUri = je->remote_uri;
		info.localUri = je->local_uri;
		info.streams = PH_STREAM_AUDIO;
		if (ca->video_payload)
		{
			info.streams |= PH_STREAM_VIDEO_RX;
		}

		if (phcb->callProgress) {
/* <UOLFONE> */
			info.reason_phrase = NULL;
			info.textinfo = NULL;
			info.status_code = 0;
			info.error_source = 0;
/* </UOLFONE> */
			phcb->callProgress(ca->cid, &info);
		}
		owplFireCallEvent(ca->cid,
			CALLSTATE_OFFERING,
			CALLSTATE_OFFERING_ACTIVE,
			je->remote_uri,
			0);
	}
	else
	{
		ph_answer_request(je->did, 500, vl->contact);
	}

}

void
ph_call_replaces(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca, *oldca;

	DBG_SIP_NEGO("SIP_NEGO: ph_call_replaces\n");

	clear(info);

	ca = ph_locate_call(je, 1);

	if (ca)
	{
		oldca = ph_locate_call_by_cid(je->replacedcid);

		if (oldca)
		{
			info.userData = je->external_reference;
			info.event = phCALLREPLACED;
			info.u.remoteUri = je->remote_uri;
			info.localUri = je->local_uri;
			info.newcid = ca->cid;

			info.vlid = oldca->vlid;

			if (!ca->vlid)
			{
				ca->vlid = info.vlid;
			}

			if (phcb->callProgress) {
/* <UOLFONE> */
				info.reason_phrase = NULL;
				info.textinfo = NULL;
				info.status_code = 0;
				info.error_source = 0;
/* </UOLFONE> */
				phcb->callProgress(oldca->cid, &info);
			}

			//TODO: Find an appropriate event to raise here for the new owpl API

			ph_call_media_stop(oldca);

			phAcceptCall(ca->cid);
		}
	}
}


/**
 * @brief function called upon reception of a 200 OK to a previously sent INVITATION
 *
 * This function needs to handle several cases depending on why the INVITATION was locally sent in the first place
 */
void
ph_call_answered(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca, *rca=0;

	clear(info);

	ca = ph_locate_call(je, 1);
	DBG_SIP_NEGO("ph_call_answered remotely cid=%d, local.(hold, resume, refer) = (%d, %d, %d)\n", ca->cid, ca->localhold, ca->localresume, ca->localrefer);
    
	if (ca)
	{
		rca = ph_locate_call_by_cid(ca->rcid);
	}

	ca->did = je->did;

	if (!ca->localhold)
	{
		ph_call_retrieve_payloads(ca, je, -1);
		ph_call_media_start(ca, je, ca->localresume);
	}

	if (ca->localhold && ph_call_hasaudio(ca))
	{
#ifndef MEDIA_SUSPEND
		ph_call_media_stop(ca);
#else
		ph_call_media_suspend(ca, 1);
#endif
	}

	info.localUri = je->local_uri;
	info.userData = je->external_reference;
	info.u.remoteUri = je->remote_uri;
	info.vlid = ca->vlid;
	info.streams = PH_STREAM_AUDIO;
	if (ca->video_payload)
	{
		info.streams |= PH_STREAM_VIDEO_RX;
	}
	if (ca->localhold)
	{
		info.event = phHOLDOK;
		DBG_SIP_NEGO("phHOLDOK, cid=%d\n", ca->cid);
		if (!ca->localrefer)
		{
			owplFireCallEvent(ca->cid,
				CALLSTATE_HOLD,
				CALLSTATE_HOLD_STARTED,
				je->remote_uri,
				0);
		}
	}
	else if (ca->localresume)
	{
		info.event = phRESUMEOK;
		DBG_SIP_NEGO("phRESUMEOK, cid=%d\n", ca->cid);
		ca->localresume = 0;
		if (!ca->localrefer)
		{
			owplFireCallEvent(ca->cid,
				CALLSTATE_HOLD,
				CALLSTATE_HOLD_RESUMED,
				je->remote_uri,
				0);
		}
	}
	else {
		info.event = phCALLOK;
		DBG_SIP_NEGO("phCALLOK, cid=%d\n", ca->cid);
		if (!ca->localrefer)
		{
			owplFireCallEvent(ca->cid,
				CALLSTATE_CONNECTED,
				CALLSTATE_CONNECTED_ACTIVE,
				je->remote_uri,
				0);
		}
	}

	if (!ca->localrefer)
	{
		if (phcb->callProgress) {
/* <UOLFONE> */
			info.reason_phrase = NULL;
			info.textinfo = NULL;
			info.status_code = 0;
			info.error_source = 0;
/* </UOLFONE> */
			phcb->callProgress(ca->cid, &info);
		}
	}

	if (rca)
	{
		ph_refer_notify(rca->rdid, je->status_code, "Answered", 1);
	}
}


void
ph_call_proceeding(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca, *rca=0;
	int cng = 0;

	DBG_SIP_NEGO("SIP NEGO: ph_call_proceeding\n");
	clear(info);

	ca = ph_locate_call(je, 1);
	if (ca)
	{
		rca = ph_locate_call_by_cid(ca->rcid);
	}

	if ((!ca->localrefer)&&(!ca->localresume))
	{
		ph_call_retrieve_payloads(ca, je, PH_STREAM_CNG);
		ph_call_media_start(ca, je, 0);

		info.userData = je->external_reference;
		info.event = phDIALING;
		info.u.remoteUri = je->remote_uri;
		info.vlid = ca->vlid;

		info.streams = ca->nego_mflags;

		if (phcb->callProgress) {
/* <UOLFONE> */
			info.reason_phrase = NULL;
			info.textinfo = NULL;
			info.status_code = 0;
			info.error_source = 0;
/* </UOLFONE> */
			phcb->callProgress(ca->cid, &info);
		}

		owplFireCallEvent(ca->cid,
			CALLSTATE_REMOTE_OFFERING,
			CALLSTATE_REMOTE_OFFERING_NORMAL,
			je->remote_uri,
			0);
	}

	if (rca)
	{
		ph_refer_notify(rca->rdid, je->status_code, "Proceeding", 0);
	}
}

void
ph_call_redirected(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca;
	int validUris;


	ca = ph_locate_call(je, 1);

	if (!ca)
	{
		return;
	}

	validUris = nonempty(je->remote_contact) && nonempty(je->remote_uri);

	if ((ca->redirs > 8) || !validUris || ph_find_matching_vline(je->remote_contact, PHM_IGNORE_PORT) || ph_same_uri(je->remote_contact, je->remote_uri))
	{
		ph_call_requestfailure(je);
		return;
	}


	clear(info);


	info.localUri = je->local_uri;
	info.userData = je->external_reference;
	info.event = phCALLREDIRECTED;
	info.u.remoteUri = je->remote_contact;
	info.vlid = ca->vlid;
	info.newcid = 0;

	if (phcfg.autoredir)
	{
		phcall_t *newca;

		newca = ph_allocate_call(-2);
		newca->redirs = ca->redirs + 1;
		info.newcid = phLinePlaceCall_withCa(info.vlid, je->remote_contact, 0, 0, ca->user_mflags, newca);
	}

	if(phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = NULL;
		info.textinfo = NULL;
		info.status_code = 0;
		info.error_source = 0;
/* </UOLFONE> */
		phcb->callProgress(ca->cid, &info);
	}

	owplFireCallEvent(ca->cid,
		CALLSTATE_REDIRECTED,
		CALLSTATE_REDIRECTED_NORMAL,
		je->remote_contact,
		0);

	DBG_SIP_NEGO("release calls");
	ph_release_call(ca);
}


void ph_callStopRinging(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca;

	clear(info);

	ca = ph_locate_call(je, 1);
	if (ca->isringing)
	{
		ca->isringing = 0;
		info.event = phRINGandSTOP;

		info.localUri = je->local_uri;
		info.userData = je->external_reference;

		info.u.remoteUri = je->remote_uri;
		info.vlid = ca->vlid;

		if(phcb->callProgress) {
/* <UOLFONE> */
			info.reason_phrase = NULL;
			info.textinfo = NULL;
			info.status_code = 0;
			info.error_source = 0;
/* </UOLFONE> */
			phcb->callProgress(ca->cid, &info);
		}

		//TODO: Find an appropriate event to raise here for the new owpl API
	}
}

void
ph_call_ringing(eXosip_event_t *je)
{
	int ret = 0;
	phCallStateInfo_t info;
	phcall_t *ca, *rca=0;
	int cng=0;

	DBG_SIP_NEGO("SIP NEGO: ph_call_ringing\n");

	clear(info);

	ca = ph_locate_call(je, 1);
	if (ca)
	{
		rca = ph_locate_call_by_cid(ca->rcid);
	}

	ph_call_retrieve_payloads(ca, je, PH_STREAM_CNG);

	ret = ph_call_media_start(ca, je, 0);

	info.event = phRINGING;

	if (ret == -PH_NOMEDIA && !ph_call_hasaudio(ca) && !ca->isringing) /*  no audio and softPhone is now not ringing and has no sound */
	{
		ca->isringing = 1;
		info.event = phRINGandSTART;
	}
	else if (ca->isringing )
	{
		ca->isringing = 0;
		info.event = phRINGandSTOP;
	}


	info.localUri = je->local_uri;
	info.userData = je->external_reference;

	info.u.remoteUri = je->remote_uri;
	info.vlid = ca->vlid;

	info.streams = ca->nego_mflags;

	if(phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = NULL;
		info.textinfo = NULL;
		info.status_code = 0;
		info.error_source = 0;
/* </UOLFONE> */
		phcb->callProgress(ca->cid, &info);
	}

	owplFireCallEvent(ca->cid, CALLSTATE_REMOTE_ALERTING,
									CALLSTATE_REMOTE_ALERTING_NORMAL,
									je->remote_contact, 0);

	if (rca)
	{
		ph_refer_notify(rca->rdid, 180, "Ringing", 0);
	}
}


/*static*/ void
ph_call_requestfailure(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca, *rca=0;
	int i;

	clear(info);

	ca = ph_locate_call(je, 0);
	if (!ca)
	{
		return;
	}

	rca = ph_locate_call_by_cid(ca->rcid);
	info.vlid = ca->vlid;

	info.localUri = je->local_uri;
	info.userData = je->external_reference;
	if (je->status_code == 486)
	{
		info.event = phCALLBUSY;
		info.u.remoteUri = je->remote_uri;

		owplFireCallEvent(ca->cid, CALLSTATE_DISCONNECTED, CALLSTATE_DISCONNECTED_BUSY, je->remote_uri, 0);
	}
	else
	{
		info.event = phCALLERROR;
		info.u.errorCode = je->status_code;

		owplFireCallEvent(ca->cid, CALLSTATE_DISCONNECTED, CALLSTATE_DISCONNECTED_UNKNOWN, je->remote_uri, 0);
	}

	if (phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = je->reason_phrase;
		info.textinfo = je->textinfo;
		info.status_code = je->status_code;
		info.error_source = 1;
/* </UOLFONE> */
		phcb->callProgress(ca->cid, &info);
	}

	if (rca)
	{
		ph_refer_notify(rca->rdid, je->status_code, je->status_code == 486 ? "Busy" : "Request failure", 1);
	}
	DBG_SIP_NEGO("release calls");
	ph_release_call(ca);
}


void
ph_call_serverfailure(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca, *rca=0;

	clear(info);

	ca = ph_locate_call(je, 0);

	if (!ca) {
		return;
	}

	rca = ph_locate_call_by_cid(ca->rcid);
	info.vlid = ca->vlid;
	info.localUri = je->local_uri;
	info.userData = je->external_reference;
	info.event = phCALLERROR;
	info.u.errorCode = je->status_code;


	if (phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = je->reason_phrase;
		info.textinfo = je->textinfo;
		info.status_code = je->status_code;
		info.error_source = 2;
/* </UOLFONE> */
		phcb->callProgress(ca->cid, &info);
	}

	owplFireCallEvent(ca->cid, CALLSTATE_DISCONNECTED, CALLSTATE_DISCONNECTED_NETWORK, je->remote_uri, 0);

	if (rca)
	{
		ph_refer_notify(rca->rdid, je->status_code, "Server failure", 1);
	}

	DBG_SIP_NEGO("release calls");
	ph_release_call(ca);
}

void
ph_call_globalfailure(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca, *rca=0;

	clear(info);

	ca = ph_locate_call(je, 0);
	if (!ca) {
		return;
	}
	
	rca = ph_locate_call_by_cid(ca->rcid);
	info.vlid = ca->vlid;
	info.userData = je->external_reference;
	info.localUri = je->local_uri;

	if (je->status_code == 600)
	{
		info.event = phCALLBUSY;
		info.u.remoteUri = je->remote_uri;

		owplFireCallEvent(ca->cid, CALLSTATE_DISCONNECTED, CALLSTATE_DISCONNECTED_BUSY, je->remote_uri, 0);
	}
	else
	{
		info.event = phCALLERROR;
		info.u.errorCode = je->status_code;

		owplFireCallEvent(ca->cid, CALLSTATE_DISCONNECTED, CALLSTATE_DISCONNECTED_NETWORK, je->remote_uri, 0);
	}

	if (phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = je->reason_phrase;
		info.textinfo = je->textinfo;
		info.status_code = je->status_code;
		info.error_source = 3;
/* </UOLFONE> */
		phcb->callProgress(ca->cid, &info);
	}

	if (rca)
	{
		ph_refer_notify(rca->rdid, je->status_code, "Global failure", 1);
	}

	DBG_SIP_NEGO("release calls");
	ph_release_call(ca);
}

void
ph_call_noanswer(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca, *rca=0;

	clear(info);

	ca = ph_locate_call(je, 1);

	if (!ca)
	{
		return;
	}
	rca = ph_locate_call_by_cid(ca->rcid);
	info.vlid = ca->vlid;
	info.userData = je->external_reference;
	info.event = phNOANSWER;
	info.u.remoteUri = je->remote_uri;
	info.localUri = je->local_uri;

	if (phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = NULL;
		info.textinfo = NULL;
		info.status_code = 0;
		info.error_source = 0;
/* </UOLFONE> */
		phcb->callProgress(ca->cid, &info);
	}

	owplFireCallEvent(ca->cid, CALLSTATE_DISCONNECTED, CALLSTATE_DISCONNECTED_NO_RESPONSE, je->remote_uri, 0);

	if (rca)
	{
		ph_refer_notify(rca->rdid, je->status_code, "No answer", 1);
	}

	DBG_SIP_NEGO("release calls");
	ph_release_call(ca);
}



void
ph_call_closed(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca, *rca=0;

	clear(info);

	ca = ph_locate_call(je, 0);
	if (!ca)
	{
		return;
	}

	rca = ph_locate_call_by_cid(ca->rcid);
	info.vlid = ca->vlid;
	DBG_SIP_NEGO("release calls");
	
	info.userData = je->external_reference;
	info.event = phCALLCLOSED;
	info.u.errorCode = 0;
	if (phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = NULL;
		info.textinfo = NULL;
		info.status_code = 0;
		info.error_source = 0;
/* </UOLFONE> */
		phcb->callProgress(ca->cid, &info);
	}
	owplFireCallEvent(ca->cid, CALLSTATE_DISCONNECTED, CALLSTATE_DISCONNECTED_NORMAL, je->remote_uri, 0);

	if (rca)
	{
		ph_refer_notify(rca->rdid, je->status_code, "Closed", 1);
	}
	ph_release_call(ca);
}

/**
* @brief callback called by the exosip layer when a call is on hold
*/
void
ph_call_onhold(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca;

	DBG_SIP_NEGO("SIP_NEGO: ph_call_onhold\n");
	clear(info);

	// locate the corresponding call for this event
	ca = ph_locate_call(je, 0);
	if (!ca)
	{
		return;
	}

	info.vlid = ca->vlid;
	if (ph_call_hasaudio(ca))
	{
#ifndef MEDIA_SUSPEND
		ph_call_media_stop(ca);
#else
		ph_call_media_suspend(ca, 0);
#endif
	}

	ca->remotehold = 1;
	info.userData = je->external_reference;
	info.event = phCALLHELD;

	if (phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = NULL;
		info.textinfo = NULL;
		info.status_code = 0;
		info.error_source = 0;
/* </UOLFONE> */
		phcb->callProgress(ca->cid, &info);
	}

	owplFireCallEvent(ca->cid, CALLSTATE_HOLD, CALLSTATE_HOLD_STARTED, je->remote_uri, 0);
}

void
ph_call_offhold(eXosip_event_t *je)
{
	phCallStateInfo_t info;
	phcall_t *ca;
	int cng=0;
	int remhold;

	DBG_SIP_NEGO("SIP NEGO: ph_call_offhold\n");

	ca = ph_locate_call(je, 0);
	if (!ca)
	{
		return;
	}

	clear(info);

	info.vlid = ca->vlid;
	// ph_media_resume(ca);

	//  if (ph_call_hasaudio(ca))
	//      ph_media_stop(ca);

	remhold = ca->remotehold;
	ca->remotehold = 0;

	ph_call_retrieve_payloads(ca, je, -1);
	ph_call_media_start(ca, je, remhold);

	if (remhold)
	{
		info.userData = je->external_reference;
		info.event = phCALLRESUMED;
		info.streams = ca->nego_mflags;

		if (phcb->callProgress) {
/* <UOLFONE> */
			info.reason_phrase = NULL;
			info.textinfo = NULL;
			info.status_code = 0;
			info.error_source = 0;
/* </UOLFONE> */
			phcb->callProgress(ca->cid, &info);
		}
		owplFireCallEvent(ca->cid, CALLSTATE_HOLD, CALLSTATE_HOLD_RESUMED, je->remote_uri, 0);
	}

	ca->remotehold = 0;
}


void ph_reg_progress(eXosip_event_t *je)
{
	int i;
	phVLine *vl = 0;
	int vlid;
	int mask = 0;
	/* <UOLFONE> */
	phRegStateInfo_t regInfo;
	/* </UOLFONE> */

	vl =  ph_find_vline_by_rid(je->rid);

	if (!vl)
	{
		return;
	}

	/* <UOLFONE> */
	clear(regInfo);

	regInfo.regid = 0;
	regInfo.regStatus = -1;
	regInfo.custom_headers = NULL;
	/* </UOLFONE> */

	if (vl->LineState == LINESTATE_UNREGISTERING || 
			vl->LineState == LINESTATE_DELETING)
	{
		mask = PH_UNREG_MASK;
	}
	vlid = ph_vline2vlid(vl);

	DBG_SIP_NEGO("REGPROGRESS reg=%d for vlid=%d\n", je->rid, vlid);

	if (je->type == EXOSIP_REGISTRATION_SUCCESS)
	{
		if (vl->LineState == LINESTATE_UNREGISTERING
				 || vl->LineState == LINESTATE_UNREGISTER_FAILED
				 || vl->LineState == LINESTATE_DELETING)
		{
			_owplLineSetState(vlid, LINESTATE_UNREGISTERED, LINESTATE_CAUSE_NORMAL);
		}
		else {
			if (je->expires) {
				vl->regTimeout = je->expires;
			}
			_owplLineSetState(vlid, LINESTATE_REGISTERED, LINESTATE_CAUSE_NORMAL);
		}

		if (phcb->regProgress) {
			/* <UOLFONE> */
			/*
			phcb->regProgress(vlid, 0 | mask);
			*/
			regInfo.regid = vlid;
			regInfo.regStatus = 0 | mask;
			regInfo.custom_headers = NULL;

			ph_add_custom_reply_headers(&regInfo, je);

			phcb->regProgress(vlid, &regInfo);
			
			ph_clear_custom_reply_headers(&regInfo);
			/* </UOLFONE> */
		}

		//if (vl->used == VL_DELETING)
		if (vl->LineState == LINESTATE_DELETING)
		{
			{
				vline_free(vl);
				
				/* <UOLFONE> */
#ifdef USE_HTTP_TUNNEL
				if (phTunnel)
				{
					eXosip_reset_tunnel = TRUE;
				}
#endif
				/* </UOLFONE> */
			}
		}
	}
	else if (je->type == EXOSIP_REGISTRATION_FAILURE)
	{
		int newtimeout = -1;
		OWPL_LINESTATE_EVENT reg_event = LINESTATE_REGISTER_FAILED;

		if (mask & PH_UNREG_MASK) {
			reg_event = LINESTATE_UNREGISTER_FAILED;
		}

		if (je->status_code == 423)  /* Interval to brief */
		{
			if (je->minexpires)
			{
				vl->regTimeout = newtimeout = je->minexpires;
			}
		}

		if (je->status_code == 401 || je->status_code == 407 || (newtimeout > 0))
		{
			eXosip_lock();
			i = eXosip_register(je->rid, newtimeout);
			eXosip_unlock();

			SKIP(printf("Retrying reg=%d for vlid=%d i=%d t=%d\n", je->rid, vlid, i, newtimeout))
				if (i == 0)
				{
					return;
				}
		}

		if (phcb->regProgress) {
			/* <UOLFONE> */
			/*
			phcb->regProgress(vlid, mask | (je->status_code ? je->status_code : 500) );
			*/

			ph_add_custom_reply_headers(&regInfo, je);

			regInfo.regid = vlid;
			regInfo.regStatus = mask | (je->status_code ? je->status_code : 500);
			regInfo.custom_headers = NULL;

			phcb->regProgress(vlid, &regInfo);

			ph_clear_custom_reply_headers(&regInfo);
			/* </UOLFONE> */
		}

		switch(je->status_code) {
			case 403:
				_owplLineSetState(vlid, reg_event , LINESTATE_CAUSE_NOT_AUTHORIZED);
				break;
			case 404:
				_owplLineSetState(vlid, reg_event , LINESTATE_CAUSE_NOT_FOUND );
				break;
			case 408:
				_owplLineSetState(vlid, reg_event , LINESTATE_CAUSE_TIMEOUT);
				break;
			default:
				_owplLineSetState(vlid, reg_event , LINESTATE_CAUSE_COULD_NOT_CONNECT);
				break;
		}

		if (vl->LineState == LINESTATE_DELETING)
		{
			vline_free(vl);
		}
	}
}

void ph_notify_handler(eXosip_event_t *je)
{
	char status[16];

	// TODO REFACTOR REMOVE
	if (phcb->onNotify)
	{
		phcb->onNotify(je->sip_event, je->remote_uri, je->msg_body);
	}

	if(strcmp(je->sip_event, "presence") == 0) {
		if(owplNotificationPresenceGetStatus(je->msg_body, status, sizeof(status)) == OWPL_RESULT_SUCCESS) {
			if(strcmp(status, "open") == 0) {
				owplFireNotificationEvent(NOTIFICATION_PRESENCE, NOTIFICATION_PRESENCE_ONLINE, je->msg_body, je->remote_uri);
			} else if(strcmp(status, "closed") == 0){
				owplFireNotificationEvent(NOTIFICATION_PRESENCE, NOTIFICATION_PRESENCE_OFFLINE, je->msg_body, je->remote_uri);
			} else {
				owplFireNotificationEvent(NOTIFICATION_UNKNOWN, NOTIFICATION_PARSE_ERROR, je->msg_body, je->remote_uri);
			}
		}
	} else if (strcmp(je->sip_event, "presence.winfo") == 0) {
		owplFireNotificationEvent(NOTIFICATION_PRESENCE, NOTIFICATION_PRESENCE_WATCHER, je->msg_body, je->remote_uri);
	} else if (strcmp(je->sip_event, "message-summary") == 0) {
		owplFireNotificationEvent(NOTIFICATION_MWI, NOTIFICATION_CAUSE_UNKNOWN, je->msg_body, je->remote_uri);
	} else {
		owplFireNotificationEvent(NOTIFICATION_UNKNOWN, NOTIFICATION_CAUSE_UNKNOWN, je->msg_body, je->remote_uri);
	}
}


void ph_call_refered(eXosip_event_t *je)
{
	phcall_t *ca;
	phCallStateInfo_t info;
	int nCid;
	phVLine *vl = 0;

	DBG_SIP_NEGO("SIP_NEGO: ph_call_refered\n");

	//ca = ph_locate_call_by_cid(je->cid);
	ca = ph_locate_call(je, 0);

	if (ca)
	{
		vl = ph_valid_vlid(ca->vlid);
	}

	/*
	we're rejecting the requests refering unxisting calls, vlines and
	if we detect URI loop
	*/
	if (!ca || !vl || ph_find_matching_vline(je->refer_to, 0))
	{
		eXosip_lock();
		eXosip_answer_refer(je->did, (!ca) ? 481 : 488);
		eXosip_unlock();
		return;
	}

	eXosip_lock();
	eXosip_answer_refer(je->did, 202);
	eXosip_unlock();

	clear(info);
	ca->rdid = je->did;

	ph_call_media_stop(ca);

	info.newcid = phLinePlaceCall2(ca->vlid, je->refer_to,  0, ca->cid, ca->user_mflags);

	info.event = phXFERREQ;
	info.u.remoteUri = je->refer_to;
	info.vlid = ca->vlid;

	if (phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = NULL;
		info.textinfo = NULL;
		info.status_code = 0;
		info.error_source = 0;
/* </UOLFONE> */
		phcb->callProgress(ca->cid, &info);
	}

	owplFireCallEvent(ca->cid,
		CALLSTATE_TRANSFER,
		CALLSTATE_TRANSFER_INITIATED,
		je->remote_contact,
		0);
}


void ph_call_refer_status(eXosip_event_t *je)
{
	phcall_t *ca;
	phCallStateInfo_t info;
	int cheat = 0;
	const char *resultstr;
	int status = 0;
	int txcid;

	//ca = ph_locate_call_by_cid(je->cid);
	ca = ph_locate_call(je, 0);

	if (!ca)
	{
		return;
	}

	clear(info);

	if (je->type == EXOSIP_CALL_REFER_STATUS)
	{
		DBG_SIP_NEGO("refer_status sdp=%s\n", je->msg_body);

		if (je->ss_status == EXOSIP_SUBCRSTATE_TERMINATED)
		{
			/* cheat and suppose that the transfer succeeded */
			cheat  = 200;
		}

		resultstr = strchr(je->msg_body, ' ');

		if (resultstr)
		{
			status = atoi(resultstr);
		}
	}
	else
	{
		status = je->status_code;
	}

	if (!status)
	{
		if (!cheat)
		{
			return;
		}

		status = cheat;
	}

	if ((status < 200) && cheat)
	{
		status = cheat;
	}

	info.u.errorCode = status;
	info.vlid = ca->vlid;

	if (!ca->txcid && status == 180)
	{
		/* blind transfer:  RINGING is good enough for us */
		info.event = phXFEROK;

		owplFireCallEvent(ca->cid,
		CALLSTATE_TRANSFER,
		CALLSTATE_TRANSFER_ACCEPTED,
		je->remote_contact,
		0);
	}
	else if (status < 200 && status >= 100)
	{
		info.event = phXFERPROGRESS;

		owplFireCallEvent(ca->cid,
		CALLSTATE_TRANSFER,
		CALLSTATE_TRANSFER_TRYING,
		je->remote_contact,
		0);
	}
	else if (status >= 200 && status < 300)
	{
		info.event = phXFEROK;

		owplFireCallEvent(ca->cid,
		CALLSTATE_TRANSFER,
		CALLSTATE_TRANSFER_ACCEPTED,
		je->remote_contact,
		0);
	}
	else
	{
		info.event = phXFERFAIL;

		owplFireCallEvent(ca->cid,
		CALLSTATE_TRANSFER,
		CALLSTATE_TRANSFER_FAILURE,
		je->remote_contact,
		0);
	}

	txcid = ca->txcid;

	if(phcb->callProgress) {
/* <UOLFONE> */
		info.reason_phrase = NULL;
		info.textinfo = NULL;
		info.status_code = 0;
		info.error_source = 0;
/* </UOLFONE> */
		phcb->callProgress(ca->cid, &info);
	}

	if (info.event == phXFEROK || info.event == phXFERFAIL)
	{
		if (txcid > 0)		/* assisted transfer, close both calls  */
		{
			phCloseCall(txcid);
		}
		if (ca->cid > 0)	/* assisted or blind transfer, close call  */
		{
			phCloseCall(ca->cid);
		}
	}
}

void ph_message_progress(eXosip_event_t *je)
{
	phMsgStateInfo_t info;

	if(je != NULL) {
		memset(&info, 0, sizeof(info));

		if (je->type == EXOSIP_MESSAGE_NEW)
		{
			info.event = phMsgNew;
			info.content = je->msg_body;
			if(je->i_ctt) 
            {
                info.ctype = je->i_ctt->type; 
                info.subtype = je->i_ctt->subtype; 
            } 
            else 
            { 
                info.ctype = NULL; 
                info.subtype = NULL; 
            }
			info.to = je->local_uri;
			info.from = je->remote_uri;
			if (phcb->msgProgress != NULL)
			{
				phcb->msgProgress(0, &info);
			}
			owplFireMessageEvent(MESSAGE_NEW,
				MESSAGE_NEW_NORMAL,
				je->mid,
				je->msg_body,
				je->local_uri,
				je->remote_uri,
				(je->i_ctt != NULL) ? je->i_ctt->type : NULL,
				(je->i_ctt != NULL) ? je->i_ctt->subtype : NULL);
		}
		else if (je->type == EXOSIP_MESSAGE_SUCCESS)
		{
			info.event = phMsgOk;
			info.to = je->local_uri;
			info.from = je->remote_uri;
			if (phcb->msgProgress != NULL)
			{
				phcb->msgProgress(je->mid, &info);
			}
			owplFireMessageEvent(MESSAGE_SUCCESS,
				MESSAGE_SUCCESS_NORMAL,
				je->mid,
				je->msg_body,
				je->local_uri,
				je->remote_uri,
				(je->i_ctt != NULL) ? je->i_ctt->type : NULL,
				(je->i_ctt != NULL) ? je->i_ctt->subtype : NULL);
		}
		else if (je->type == EXOSIP_MESSAGE_FAILURE)
		{
			info.to = je->local_uri;
			info.from = je->remote_uri;
			info.event = phMsgError;
			if (phcb->msgProgress != NULL)
			{
				phcb->msgProgress(je->mid, &info);
			}
			owplFireMessageEvent(MESSAGE_FAILURE,
				MESSAGE_FAILURE_UNKNOWN,
				je->mid,
				je->msg_body,
				je->local_uri,
				je->remote_uri,
				(je->i_ctt != NULL) ? je->i_ctt->type : NULL,
				(je->i_ctt != NULL) ? je->i_ctt->subtype : NULL);
		}
	}
}

/*
TODO for future use ?
void ph_message_progress(eXosip_event_t *je)
{
	phMsgStateInfo_t info;

	if(je != NULL) {
		memset(&info, 0, sizeof(info));

		if (je->type == EXOSIP_MESSAGE_NEW)
		{
			info.event = phMsgNew;
			info.content = je->msg_body;
			info.ctype = je->i_ctt->type;
			info.subtype = je->i_ctt->subtype;
			info.to = je->local_uri;
			info.from = je->remote_uri;
			if (phcb->msgProgress != NULL)
			{
				phcb->msgProgress(0, &info);
			}

			if(je->i_ctt != NULL && strcmp(je->i_ctt->type, "typingstate") == 0) {
				if(strcmp(je->i_ctt->subtype, "typing") == 0) {
					owplFireMessageEvent(MESSAGE_NEW,
						MESSAGE_NEW_TYPING,
						je->mid,
						je->msg_body,
						je->local_uri,
						je->remote_uri,
						(je->i_ctt != NULL) ? je->i_ctt->type : NULL,
						(je->i_ctt != NULL) ? je->i_ctt->subtype : NULL);
				} else if(strcmp(je->i_ctt->subtype, "stoptyping") == 0) {
					owplFireMessageEvent(MESSAGE_NEW,
						MESSAGE_NEW_STOP_TYPING,
						je->mid,
						je->msg_body,
						je->local_uri,
						je->remote_uri,
						(je->i_ctt != NULL) ? je->i_ctt->type : NULL,
						(je->i_ctt != NULL) ? je->i_ctt->subtype : NULL);
				} else {
					owplFireMessageEvent(MESSAGE_NEW,
						MESSAGE_NEW_NOT_TYPING,
						je->mid,
						je->msg_body,
						je->local_uri,
						je->remote_uri,
						(je->i_ctt != NULL) ? je->i_ctt->type : NULL,
						(je->i_ctt != NULL) ? je->i_ctt->subtype : NULL);
				}
			} else if(je->i_ctt != NULL && strcmp(je->i_ctt->type, "buddyicon") == 0) {
				owplFireMessageEvent(MESSAGE_NEW,
					MESSAGE_NEW_BUDDY_ICON,
					je->mid,
					je->msg_body,
					je->local_uri,
					je->remote_uri,
					(je->i_ctt != NULL) ? je->i_ctt->type : NULL,
					(je->i_ctt != NULL) ? je->i_ctt->subtype : NULL);
			} else {
				owplFireMessageEvent(MESSAGE_NEW,
					MESSAGE_NEW_NORMAL,
					je->mid,
					je->msg_body,
					je->local_uri,
					je->remote_uri,
					(je->i_ctt != NULL) ? je->i_ctt->type : NULL,
					(je->i_ctt != NULL) ? je->i_ctt->subtype : NULL);
			}
		}
		else if (je->type == EXOSIP_MESSAGE_SUCCESS)
		{
			info.event = phMsgOk;
			info.to = je->local_uri;
			info.from = je->remote_uri;
			if (phcb->msgProgress != NULL)
			{
				phcb->msgProgress(je->mid, &info);
			}

			owplFireMessageEvent(MESSAGE_SUCCESS,
				MESSAGE_SUCCESS_NORMAL,
				NULL,
				je->mid,
				je->local_uri,
				je->remote_uri,
				(je->i_ctt != NULL) ? je->i_ctt->type : NULL,
				(je->i_ctt != NULL) ? je->i_ctt->subtype : NULL);
		}
		else if (je->type == EXOSIP_MESSAGE_FAILURE)
		{
			info.to = je->local_uri;
			info.from = je->remote_uri;
			info.event = phMsgError;
			if (phcb->msgProgress != NULL)
			{
				phcb->msgProgress(je->mid, &info);
			}

			owplFireMessageEvent(MESSAGE_FAILURE,
				MESSAGE_FAILURE_COULD_NOT_SEND,
				NULL,
				je->mid,
				je->local_uri,
				je->remote_uri,
				(je->i_ctt != NULL) ? je->i_ctt->type : NULL,
				(je->i_ctt != NULL) ? je->i_ctt->subtype : NULL);
		}
	}
}
*/


void ph_subscription_progress(eXosip_event_t *je)
{
	phSubscriptionStateInfo_t info;

	memset(&info, 0, sizeof(info));

	if (je->type == EXOSIP_SUBSCRIPTION_ANSWERED)
	{
		info.event = phSubscriptionOk;
		info.from = je->local_uri;
		info.to = je->remote_uri;

		if (phcb->subscriptionProgress != NULL)
		{
			phcb->subscriptionProgress (je->sid, &info);
		}

		owplFireSubscriptionEvent(je->sid, OWPL_SUBSCRIPTION_ACTIVE, SUBSCRIPTION_CAUSE_NORMAL, je->remote_uri);
	}
	else if (je->type == EXOSIP_SUBSCRIPTION_REQUESTFAILURE)
	{
		info.event = phSubscriptionError;
		if (je->status_code == 404) {
			info.event = phSubscriptionErrNotFound;
		}

		info.from = je->local_uri;
		info.to = je->remote_uri;
		if (phcb->subscriptionProgress != NULL)
		{
			phcb->subscriptionProgress (je->sid, &info);
		}

		owplFireSubscriptionEvent(je->sid, OWPL_SUBSCRIPTION_FAILED, SUBSCRIPTION_CAUSE_UNKNOWN, je->remote_uri);
	}
}


static int ph_process_call_event_default(eXosip_event_t * je) {

	switch(je->type) {
		case EXOSIP_CALL_NEW:
			ph_call_new(je);
			break;

		case EXOSIP_CALL_ANSWERED:
			ph_callStopRinging(je);
			ph_call_answered(je);
			break;

		case EXOSIP_CALL_PROCEEDING:
			ph_call_proceeding(je);
			break;

		case EXOSIP_CALL_RINGING:
			ph_call_ringing(je);
			break;

		case EXOSIP_CALL_REDIRECTED:
			ph_call_redirected(je);
			break;

		case EXOSIP_CALL_REPLACES:
			ph_call_replaces(je);
			break;

		case EXOSIP_CALL_REQUESTFAILURE:
			ph_call_requestfailure(je);
			break;

		case EXOSIP_CALL_SERVERFAILURE:
			ph_call_serverfailure(je);
			break;

		case EXOSIP_CALL_GLOBALFAILURE:
			ph_call_globalfailure(je);
			break;

		case EXOSIP_CALL_NOANSWER:
			ph_call_noanswer(je);
			break;

		case EXOSIP_CALL_CLOSED:
			ph_call_closed(je);
			break;

		case EXOSIP_CALL_HOLD:
			ph_call_onhold(je);
			break;

		case EXOSIP_CALL_OFFHOLD:
			ph_call_offhold(je);
			break;

		case EXOSIP_REGISTRATION_SUCCESS:
			ph_reg_progress(je);
			break;

		case EXOSIP_REGISTRATION_FAILURE:
			ph_reg_progress(je);
			break;

		case EXOSIP_CALL_REFERED:
			ph_call_refered(je);
			break;

		case EXOSIP_CALL_REFER_STATUS:
		case EXOSIP_CALL_REFER_FAILURE:
			ph_call_refer_status(je);
			break;


		case EXOSIP_MESSAGE_NEW:
		case EXOSIP_MESSAGE_SUCCESS:		/* announce a 200ok to a previous sent */
		case EXOSIP_MESSAGE_FAILURE:
			ph_message_progress(je);
			break;

		case EXOSIP_SUBSCRIPTION_REQUESTFAILURE:
		case EXOSIP_SUBSCRIPTION_ANSWERED:
			ph_subscription_progress(je);
			break;

		case EXOSIP_SUBSCRIPTION_NOTIFY:
			ph_notify_handler(je);
			break;

		case EXOSIP_OPTIONS_NOANSWER:
			return -2;

		case EXOSIP_ENGINE_STOPPED:
#ifdef USE_HTTP_TUNNEL
			if(phcfg.use_tunnel && phTunnel && phTunnel->h_tunnel) {
				http_tunnel_close(phTunnel->h_tunnel);
				phTunnel->h_tunnel = NULL;
				/* <UOLFONE> */
				/*
				eXosip_quit();
				*/
				/* <UOLFONE> */
			}
#endif
			return -2;

		default:
			if(phDebugLevel > 0) {
				DBG_SIP_NEGO("event(%i %i %i %i) text=%s\n", je->cid, je->sid, je->nid, je->did, je->textinfo);
			}
			break;
	}

	return 0;
}


#define MAX_SRV_IDLE_TIME	75 /* The maximum idle time since we receive the last message from server */

/**
* Tests if an event is a call event.
*
* @param	[in]	je : an eXosip event
* @return	TRUE if it is a call event; FALSE else
*/
static unsigned int isCallEvent(eXosip_event_t * je){
	if(je->type == EXOSIP_CALL_NEW ||
		je->type == EXOSIP_CALL_ANSWERED ||
		je->type == EXOSIP_CALL_PROCEEDING ||
		je->type == EXOSIP_CALL_RINGING ||
		je->type == EXOSIP_CALL_REDIRECTED ||
		je->type == EXOSIP_CALL_REPLACES ||
		je->type == EXOSIP_CALL_REQUESTFAILURE ||
		je->type == EXOSIP_CALL_SERVERFAILURE ||
		je->type == EXOSIP_CALL_GLOBALFAILURE ||
		je->type == EXOSIP_CALL_NOANSWER ||
		je->type == EXOSIP_CALL_CLOSED ||
		je->type == EXOSIP_CALL_HOLD ||
		je->type == EXOSIP_CALL_OFFHOLD ||
		je->type == EXOSIP_CALL_REFERED ||
		je->type == EXOSIP_CALL_REFER_STATUS ||
		je->type == EXOSIP_CALL_REFER_FAILURE)
	{
		return TRUE;
	}
	return FALSE;
}

static void ph_update_callstate_by_event(eXosip_event_t * je)
{
	phcall_t * rca;
	phcall_t * ca = ph_locate_call(je, 0);
	if (!ca) {
		return;
	}
	rca = ph_locate_call_by_rcid(ca->rcid);

	switch(je->type){
			case EXOSIP_CALL_NEW:
				break;

			case EXOSIP_CALL_ANSWERED:
				ca->did = je->did;
				if(rca){
					ph_refer_notify(rca->rdid, je->status_code, "Answered", 1);
				}
				break;

			case EXOSIP_CALL_PROCEEDING:
				if(rca){
					ph_refer_notify(rca->rdid, je->status_code, "Proceeding", 0);
				}
				break;

			case EXOSIP_CALL_RINGING:
				if (rca){
					ph_refer_notify(rca->rdid, 180, "Ringing", 0);
				}
				break;

			case EXOSIP_CALL_REDIRECTED:
				break;

			case EXOSIP_CALL_REPLACES:
				break;

			case EXOSIP_CALL_REQUESTFAILURE:
				if(rca) {
					ph_refer_notify(rca->rdid, je->status_code, je->status_code == 486 ? "Busy" : "Request failure", 1);
				}
				DBG_SIP_NEGO("release calls");
				ph_release_call(ca);
				break;

			case EXOSIP_CALL_SERVERFAILURE:
				if(rca) {
					ph_refer_notify(rca->rdid, je->status_code, "Server failure", 1);
				}
				DBG_SIP_NEGO("release calls");
				ph_release_call(ca);
				break;

			case EXOSIP_CALL_GLOBALFAILURE:				
				if(rca) {
					ph_refer_notify(rca->rdid, je->status_code, "Global failure", 1);
				}
				DBG_SIP_NEGO("release calls");
				ph_release_call(ca);
				break;

			case EXOSIP_CALL_NOANSWER:
				if(rca) {
					ph_refer_notify(rca->rdid, je->status_code, "No answer", 1);
				}
				DBG_SIP_NEGO("release calls");
				ph_release_call(ca);
				break;

			case EXOSIP_CALL_CLOSED:
				if(rca) {
					ph_refer_notify(rca->rdid, je->status_code, "Closed", 1);
				}
				DBG_SIP_NEGO("release calls");
				ph_release_call(ca);
				break;

			case EXOSIP_CALL_HOLD:
				break;

			case EXOSIP_CALL_OFFHOLD:
				break;

			case EXOSIP_CALL_REFERED:
				break;

			case EXOSIP_CALL_REFER_STATUS:
				break;

			case EXOSIP_CALL_REFER_FAILURE:
				break;

			default:
				break;
	}
}

/**
* Retrieves eXosip events
*
* @return	0 if there has been events; -1 if there were no events
*/
static int ph_event_get(){
	int counter =0;
	/* use events to print some info */
	eXosip_event_t *je;
	/*variables to handle OWPL plugin framework*/
	OWPL_PLUGIN *owplPlugin;
	char buf[101];
	phcall_t * ca = NULL;

	for (;;) {
		je = eXosip_event_wait(0,phWaitTimeout);
		if(je==NULL) {
			break;
		}
		counter++;

		if(phDebugLevel > 0) {
			ph_printf("\n<- %s (%i %i) [%i %s] %s requri=%s\n",
				evtnames[je->type], je->cid, je->did,
				je->status_code,
				je->reason_phrase,
				je->remote_uri,
				je->req_uri);
		}

		// get the plugin corresponding to that content-type
		if(isCallEvent(je)) {
			if((ca = ph_locate_call(je, je->type == EXOSIP_CALL_NEW ? 1 : 0)) == NULL) {
				return -1;
			}

			if( ca && (ca->owplPlugin == 0)) {
				if(je->i_ctt && je->i_ctt->type) {
					strncpy(buf, je->i_ctt->type, sizeof(buf)-1);
					if (je->i_ctt->subtype) {
						strncat(buf, "/", sizeof(buf) - strlen(buf) -1);
						strncat(buf,je->i_ctt->subtype, sizeof(buf) - strlen(buf) - 1);
					}
					owplPlugin = owplGetPlugin4ContentType(buf);
					if (owplPlugin) {
						ca->owplPlugin = owplPlugin;
					}
				}
			}

			if(ca->owplPlugin == NULL) {
				ph_process_call_event_default(je);
			} else {
				owplFireExosipCallEvent(je);
			}

            // beware here to the fact that events have already been fired to the top-level-program.
            // If the TLP is highly "stimulus-response" reactive (fast to react), this may lead to problems
            // The problem was identified once for the 3-way conference mechanism when the phResume was called
            // as soon as the phHoldOK had been sent. cf REV 10978 for a bugfix example (jwa)
			ph_update_callstate_by_event(je);
            
            
		}else{
			switch(je->type){
				case EXOSIP_REGISTRATION_SUCCESS:
					ph_reg_progress(je);
					break;

				case EXOSIP_REGISTRATION_FAILURE:
					ph_reg_progress(je);
					break;

				case EXOSIP_MESSAGE_NEW:
				case EXOSIP_MESSAGE_SUCCESS:        /* announce a 200ok to a previous sent */
				case EXOSIP_MESSAGE_FAILURE:
					ph_message_progress(je);
					break;

				case EXOSIP_SUBSCRIPTION_REQUESTFAILURE:
				case EXOSIP_SUBSCRIPTION_ANSWERED:
					ph_subscription_progress(je);
					break;

				case EXOSIP_SUBSCRIPTION_NOTIFY:
					ph_notify_handler(je);
					break;

				case EXOSIP_OPTIONS_NOANSWER:
					return -2;

				case EXOSIP_ENGINE_STOPPED:
#ifdef USE_HTTP_TUNNEL
					if(phcfg.use_tunnel && phTunnel && phTunnel->h_tunnel) {
						http_tunnel_close(phTunnel->h_tunnel);
						phTunnel->h_tunnel = NULL;
						/* <UOLFONE> */
						/*
						eXosip_quit();
						*/
						/* </UOLFONE> */
					}
#endif
					return -2;

				default:
					if(phDebugLevel > 0) {
						ph_printf("event(%i %i %i %i) text=%s\n", je->cid, je->sid, je->nid, je->did, je->textinfo);
					}
					break;
			}
		}

		eXosip_event_free(je);
	}

	ph_refresh_vlines();
	ph_scan_calls();

	if (counter>0) {
		return 0;
	}
	return -1;
}

static void
ph_keep_refreshing()
{
	time_t now;
	static time_t last_refresh;

	time(&now);
	if (now - last_refresh > PH_REFRESH_INTERVAL)
	{
		phRefresh();
		last_refresh = now;
	}

	if (eXosip_get_srv_idle_time() > MAX_SRV_IDLE_TIME)
	{
		/* <UOLFONE> */
		phRegStateInfo_t regInfo;

		clear(regInfo);

		regInfo.regid = 0;
		regInfo.regStatus = -5;
		regInfo.custom_headers = NULL;
		/* </UOLFONE> */

		// We treat this as register failure.
		eXosip_reset_idle_time();
		if (phcb && phcb->regProgress) {
			/* <UOLFONE> */
			/*
			phcb->regProgress(0, -1);
			*/
			phcb->regProgress(0, &regInfo);

			/* Free used phone lines */
			phFreeVlines();
			/* </UOLFONE> */
		}
		owplFireLineEvent(0, LINESTATE_REGISTER_FAILED, LINESTATE_CAUSE_COULD_NOT_CONNECT, 0);
	}
}



/**
* eXosip event reader thread
*/
void *
ph_api_thread(void *arg)
{
	time_t t1,t2;
	/* <UOLFONE> */
	phRegStateInfo_t regInfo;
	/* </UOLFONE> */
	t1 = 0;
	phIsInitialized = 1;

	/* <UOLFONE> */
	clear(regInfo);

	regInfo.regid = 0;
	regInfo.regStatus = -1;
	regInfo.custom_headers = NULL;
	/* </UOLFONE> */

#ifdef PHAPI_VIDEO_SUPPORT
	webcam_api_initialize();
#endif

	time(&t1);
	while(1)
	{
#ifdef WIN32
		Sleep(100);
#endif
		if (!phIsInitialized)
		{
			return 0;
		}

		ph_keep_refreshing();

		if (ph_event_get() == -2)
		{
			if (phcb->regProgress) {
				/* <UOLFONE> */
				/*
				phcb->regProgress(0, -1);
				*/
				phcb->regProgress(0, &regInfo);
				
				eXosip_reset_tunnel = TRUE;
				/* </UOLFONE> */
			}
			owplFireLineEvent(0, LINESTATE_REGISTER_FAILED, LINESTATE_CAUSE_COULD_NOT_CONNECT, 0);
			//phTerminate();
			break;
		}
	}

#ifdef PHAPI_VIDEO_SUPPORT
	webcam_api_uninitialize();
#endif

	return 0;
}

#ifdef QOS_DEBUG_ENABLE
void phrtcp_QoS_enable_rtcp_report(int ToF)
{
	ortcp_enable_rtcp_report(ToF);
}
void phrtcp_report_set_cb(jt_rtcpCallbacks_t *cbk)
{
	ortcp_report_set_cb(cbk);
}
int phrtcp_report_begin()
{
	phrtcp_QoS_enable_rtcp_report(1);
	ortcp_report_begin();
}
int phrtcp_report_end()
{
	ortcp_report_end();
}
#endif /* end of QOS_DEBUG_ENABLE */


int getPublicPort(char * local_voice_port, char * local_video_port, char * public_voice_port, char * public_video_port)
{
	Socket  sock;
	Socket  sock1;
	Socket  sock2;
	Socket  sock3;
	StunAddress4 mappedAddr;
	StunAddress4 stunServerAddr;
	int res;
	int res2;
	int loop = 0;

	if (phcfg.use_tunnel)
	{
		return 1;
	}

	stunParseServerName(phcfg.stunserver, &stunServerAddr);

	do {
		printf("get pub video: %s", local_voice_port);
		res = stunOpenSocketPair(&stunServerAddr, &mappedAddr,&sock, &sock1,atoi(local_voice_port), NULL, 1);

		if (-1 !=  res)
		{
			snprintf(local_voice_port, 9, "%i", res);
			snprintf(public_voice_port, 9, "%d", mappedAddr.port);
			stunCloseSocket(sock1);
			stunCloseSocket(sock);
		}
		else
		{
			stunCloseSocket(sock);
			stunCloseSocket(sock1);
			snprintf(local_voice_port, 9, "%i",atoi(local_voice_port)+5);
			loop++;
		}
	} while (res <0 && loop < 4 );

	if (res <0)
	{
		return -1;
	}

	loop = 0;

	do {
		printf("get pub video: %s", local_video_port);

		res2 = stunOpenSocketPair(&stunServerAddr, &mappedAddr,&sock2, &sock3,atoi(local_video_port), NULL, 1);
		//sock2 = stunOpenSocket(&stunServerAddr, &mappedAddr, atoi(&local_video_port), NULL, 0);

		if (-1 !=  res2)
		{
			snprintf(local_video_port, 9, "%i", res2);
			snprintf(public_video_port, 9, "%d", mappedAddr.port);
			stunCloseSocket(sock2);
			stunCloseSocket(sock3);
		}
		else
		{
			stunCloseSocket(sock2);
			stunCloseSocket(sock3);
			snprintf(local_video_port, 9, "%i",atoi(local_video_port)+5);
			loop++;
		}
	} while(res2 <0 && loop < 4 );

	if (res2 <0)
	{
		return -1;
	}

	printf("get public port public voice_port: %s video: %s  ------ \n \n ",public_voice_port, public_video_port);
	return 1;
}

int phLineGetSipAddress(int vlid, char buf[], int bufsize)
{
	int i;
	phVLine *vl;

	vl = ph_valid_vlid(vlid);

	if (!vl)
	{
		return PH_BADVLID;
	}

	ph_vline_get_from(buf, bufsize, vl);
	return 0;
}

int phCrash()
{
	*(int *)0 = 0;
	return 1;
}

void phSetDebugLevel(int level)
{
	phDebugLevel = level;
}

// <ncouturier>
// ----- GENERIC WRAPPERS FOR PLUGINS -----

/**
* Gets the local IP
*
* @param	[out]	ip : the buffer that will receive the local ip
*/
void ph_get_local_ip(char * ip){
	eXosip_get_localip(ip);
}

/**
* Gets the local username
*
* @param	[in]	vlid : a virtual line id
* @return	the local username
*/
char * ph_get_username(int vlid){
	phVLine *vl;

	if (!(vl = ph_valid_vlid(vlid)))
	{
		return NULL;
	}

	return vl->username;
}


/**
* Gets the virtual line id associated to a user_id
*
* @param	[in]	user_id : ?
* @param	[in]	alt_id : ?
* @return	the virtual line id
*/
int ph_get_vlid(const char * user_id, const char * alt_id){
	return ph_get_vline_id(user_id, alt_id);
}

/**
* Generic PhApi service. Sends an invite with a custom body
*
* @param	[in]	vlid : a virtual line id
* @param	[in]	userdata : ?
* @param	[in]	uri : the destination uri (ex : "<sip:user@domain>")
* @param	[in]	bodytype : the type of body message (ex : "type/subtype")
* @param	[in]	body : the custom message body
* @param	[in]	call_id : the call id
* @return	TRUE if succeeds; FALSE else
*/
int phInvite(int vlid, void *userdata, char * uri, const char * bodytype, const char * body, int * call_id){
	int i;
	osip_message_t *invite;
	char *proxy = phcfg.proxy;
	phVLine *vl;
	char from[512];
	phcall_t *ca = NULL; // forced to use it, even though it mixes the notion of SIP and SDP call

	DBG_SIP_NEGO("phLineSendFile: a new file transfer is being processed\n");


	// TODO verif des arguments
	if (!nonempty(uri))
	{
		return -PH_BADARG;
	}

	if (!(vl = ph_valid_vlid(vlid)))
	{
		return -PH_BADVLID;
	}

	ph_vline_get_from(from, sizeof(from), vl);

	proxy = vl->proxy;

	if((i = eXosip_build_initial_invite(&invite, uri, from, proxy, "")) != 0)
	{
		return -1;
	}

	eXosip_lock();
	i = eXosip_initiate_call_with_body(invite, bodytype, body, userdata);

	ca = ph_allocate_call(i);
	ca->vlid = ph_vline2vlid(vl);

	eXosip_unlock();

	if(ca != NULL && call_id != NULL)
	{
		*call_id = ca->cid;

	}

	owplFireCallEvent(ca->cid,
						CALLSTATE_REMOTE_OFFERING,
						CALLSTATE_REMOTE_OFFERING_NORMAL,
						uri,
						0);

	return i;
}

/** DEPRECATED, use owplCallAnswerWithBody
* Generic PhApi service. Sends a 200OK with a custom body
*
* @param	[in]	cid : the call id
* @param	[in]	bodytype : the type of body message (ex : "type/subtype")
* @param	[in]	body : the custom message body
* @return	TRUE if succeeds; FALSE else
*/
int phAccept(int cid, const char * bodytype, const char * body){
	int i;
	phcall_t *ca = ph_locate_call_by_cid(cid);

	DBG_SIP_NEGO("SIP NEGO: phAccept\n");
	if (!ca)
	{
		return -PH_BADCID;
	}

	eXosip_lock();
	i = eXosip_answer_call_with_body(ca->did, 200, bodytype, body); // returns 0 on succes
	eXosip_unlock();

	if(i != 0)
	{
		return -1;
	}

	return 0;
}

int phReject(int cid) {
	int i;
	phcall_t *ca = ph_locate_call_by_cid(cid);

	DBG_SIP_NEGO("SIP NEGO: phReject\n");
	if (!ca)
	{
		return -PH_BADCID;
	}

	eXosip_lock();
	i = eXosip_answer_call(ca->did, 486, 0, ph_get_call_contact(ca), 0, 0, 0);
	eXosip_unlock();

	DBG_SIP_NEGO("release calls");
	ph_release_call(ca);

	return i;

}

/** DEPRECATED, no replacement
* Generic PhApi service. Creates a new call in PhApi.
*
* @param	[in]	cid : a call id
* @param	[in]	did : a dialog id
* @param	[in]	local_uri : ?
* @param	[in]	req_uri : ?
* @return	TRUE if succeeds; FALSE else
*/
int phNewCall(int cid, int did, const char * local_uri, const char * req_uri){
	phcall_t *ca;
	phVLine *vl;
	int vlid;

	vlid = ph_get_vline_id(local_uri, req_uri);

	if (!vlid)
	{
		ph_answer_request(did, 404, 0);
		return FALSE;
	}

	vl = ph_vlid2vline(vlid);

	assert(vl);

	if (vl->busy)
	{
		ph_answer_request(did, 486, vl->contact);
		return FALSE;
	}


	if (vl->followme && vl->followme[0])
	{
		ph_answer_request(did, 302, vl->followme);
		return FALSE;
	}

	// ca = ph_locate_call(je, 1);
	if((ca = ph_locate_call_by_cid(cid)) == NULL)
	{
		if((ca = ph_allocate_call(cid)) == NULL)
		{
			return FALSE;
		}
		ca->did = did;
	}

	if (ca)
	{
		ca->vlid = vlid;
		ph_build_cname(ca->cname, sizeof(ca->cname), ph_vlid2vline(ca->vlid));
	}
	else
	{
		ph_answer_request(did, 500, vl->contact);
		return FALSE;
	}

	return ca->cid;
}

/** DEPRECATED, this function will be deleted and there is no replacement
* Generic PhApi service. Stops the state ringing of a call.
*
* @param	[in]	call_id : a call id
* @return	TRUE if succeeds; FALSE else
*/
int phStopRinging(int call_id){
	phcall_t *ca = NULL;

	ca = ph_locate_call_by_cid(call_id);
	if(ca && ca->isringing)
	{
		ca->isringing = 0;
		return TRUE;
	}
	return FALSE;
}

/** *DEPRECATED* , use owplCallDisconnect instead!

* Generic PhApi service. Sends a CANCEL, DECLINE or a BYE that must be sent
*
* @param	[in]	call_id : the call id
* @return	TRUE if succeeds; FALSE else
*/
int phBye(int call_id){
	int i;
	phcall_t *ca = ph_locate_call_by_cid(call_id);
	int did;
	int extern_cid = -1;

	DBG_SIP_NEGO("phCloseSipCall %d\n", call_id);

	if(!ca)
	{
		return -PH_BADCID;
	}

	if(ca->isringing)
	{
		ca->isringing = 0;
	}

	did = ca->did;
	extern_cid = ca->extern_cid;

	ph_release_call2(ca);

	eXosip_lock();
	i = eXosip_terminate_call(extern_cid, did);
	eXosip_unlock();

	if(i)
	{
		return i;
	}

	return i;
}

/**
* Generic PhApi service. Is the same as a phBye, because the function eXosip_terminate_call() used inside decides
* wether it is a CANCEL, DECLINE or a BYE that must be sent
*
* @param	[in]	call_id : the call id
* @return	TRUE if succeeds; FALSE else
*/
int phCancel(int call_id){
	int i;
	phcall_t *ca = ph_locate_call_by_cid(call_id);
	int did;
	int extern_cid = -1;

	DBG_SIP_NEGO("phCloseSipCall %d\n", call_id);

	if(!ca)
	{
		return -PH_BADCID;
	}

	if(ca->isringing)
	{
		ca->isringing = 0;
	}

	did = ca->did;
	extern_cid = ca->extern_cid;

	ph_release_call2(ca);

	eXosip_lock();
	i = eXosip_terminate_call(extern_cid, did);
	eXosip_unlock();

	if(i)
	{
		return i;
	}

	return i;
}

/**
* Generic PhApi service.
*
* @param	[in]
* @return	TRUE if succeeds; FALSE else
*/
int phEndCall(int call_id, int status_code){
	phcall_t *ca, *rca=0;

	ca = ph_locate_call_by_cid(call_id);
	if(ca)
	{
		rca = ph_locate_call_by_cid(ca->rcid);
		DBG_SIP_NEGO("release calls");
		ph_release_call(ca);
	}
	else
	{
		return FALSE;
	}

	if (rca)
	{
		ph_refer_notify(rca->rdid, status_code, "Closed", 1);
	}

	return TRUE;
}
/**
* Generic PhApi service.
*
* @param	[in]	call_id : the call id
* @param	[in]	status_code : the status code of the originating eXosip event
* @return	TRUE if succeeds; FALSE else
*/
int phRequestFailure(int call_id, int status_code){
	phcall_t *ca, *rca=0;

	ca = ph_locate_call_by_cid(call_id);
	if(!ca)
	{
		return FALSE;
	}

	rca = ph_locate_call_by_cid(ca->rcid);	

	if (rca)
	{
		ph_refer_notify(rca->rdid, status_code, status_code == 486 ? "Busy" : "Request failure", 1);
	}

	DBG_SIP_NEGO("release calls");
	ph_release_call(ca);

	return TRUE;
}

/**
* Generic PhApi service.
*
* @param	[in]	call_id : the call id
* @param	[in]	status_code : the status code of the originating eXosip event
* @return	TRUE if succeeds; FALSE else
*/
int phServerFailure(int call_id, int status_code){
	phcall_t *ca, *rca=0;

	ca = ph_locate_call_by_cid(call_id);
	if(ca)
	{
		rca = ph_locate_call_by_cid(ca->rcid);
		DBG_SIP_NEGO("release calls");
		ph_release_call(ca);
	}
	else
	{
		return FALSE;
	}

	if(rca)
	{
		ph_refer_notify(rca->rdid, status_code, "Server failure", 1);
	}

	return TRUE;
}

/**
* Generic PhApi service.
*
* @param	[in]	call_id : the call id
* @param	[in]	status_code : the status code of the originating eXosip event
* @return	TRUE if succeeds; FALSE else
*/
int phGlobalFailure(int call_id, int status_code){
	phcall_t *ca, *rca=0;

	ca = ph_locate_call_by_cid(call_id);
	if(!ca)
	{
		return FALSE;
	}
		
	rca = ph_locate_call_by_cid(ca->rcid);
	if(rca)
	{
		ph_refer_notify(rca->rdid, status_code, "Global failure", 1);
	}

	DBG_SIP_NEGO("release calls");
	ph_release_call(ca);

	return TRUE;
}

/**
* Generic PhApi service.
*
* @param	[in]	call_id : the call id
* @param	[in]	status_code : the status code of the originating eXosip event
* @return	TRUE if succeeds; FALSE else
*/
int phNoAnswer(int call_id, int status_code){
	phcall_t *ca, *rca=0;

	ca = ph_locate_call_by_cid(call_id);
	if (!ca)
	{
		return FALSE;
	}
	rca = ph_locate_call_by_cid(ca->rcid);
	if (rca)
	{
		ph_refer_notify(rca->rdid, status_code, "No answer", 1);
	}

	DBG_SIP_NEGO("release calls");
	ph_release_call(ca);
	//ca->owplPlugin = 0;

	return TRUE;
}

int phHoldOn(int call_id, const char * bodytype){
	phcall_t *ca = ph_locate_call_by_cid(call_id);
	int i;

	DBG_SIP_NEGO("SIP_NEGO: phHoldOn\n");

	if (!ca)
	{
		return -PH_BADCID;
	}

	eXosip_lock();
	i = eXosip_on_hold_call_with_body(ca->did, bodytype, "holdon");
	eXosip_unlock();

	if(i==0)
	{
		if (ca->localhold)
		{
			return -PH_HOLDERR;
		}

		ca->localhold = 1;
		return TRUE;
	}
	return FALSE;
}

int phHoldOff(int call_id, const char * bodytype){
	phcall_t *ca = ph_locate_call_by_cid(call_id);
	int i;

	DBG_SIP_NEGO("SIP_NEGO: phHoldOff\n");

	if (!ca)
	{
		return -PH_BADCID;
	}

	eXosip_lock();
	i = eXosip_off_hold_call_with_body(ca->did, bodytype, "holdoff");
	eXosip_unlock();

	if(i==0)
	{
		if (ca->localhold != 1)
		{
			return -PH_HOLDERR;
		}

		ca->localhold = 0;
		return TRUE;
	}
	return FALSE;
}

void getProxyInfo(phProxy_t * proxy_info) {
	if(proxy_info != NULL)
	{
		strncpy(proxy_info->http_proxy, phcfg.http_proxy, sizeof(proxy_info->http_proxy));
		proxy_info->http_proxy_port = phcfg.http_proxy_port;
		strncpy(proxy_info->http_proxy_user, phcfg.http_proxy_user, sizeof(proxy_info->http_proxy_user));
		strncpy(proxy_info->http_proxy_passwd, phcfg.http_proxy_passwd, sizeof(proxy_info->http_proxy_passwd));
	}
}

// -----
// </ncouturier>

