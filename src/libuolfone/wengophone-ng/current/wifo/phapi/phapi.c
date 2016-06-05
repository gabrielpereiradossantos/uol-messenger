#include "phglobal.h"
#include <eXosip/eXosip.h>
#include "phapi.h"
#include "phcall.h"
#include "phmedia.h"
#include "phvline.h"
#include "owpl_plugin.h"
#include "svoip_phapi.h"
#include <rtpport.h> // only for GMutex

#if !defined(OS_WINDOWS)
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

/*
 * Declaration of some variables that were defined inside phapi-old.c
 */
// [
extern eXosip_tunnel_t * phTunnel;
extern int phWaitTimeout;
extern GMutex * ph_media_stop_mutex;

int ph_tunnel_init2(char *sip_proxy);
int ph_tunnel_init();
void * ph_api_thread(void *arg);
void ph_nat_init();
void ph_calls_init();
void ph_payloads_init();
void ph_avcodec_init();
// ]


OWPL_LOG_REPORT_PROC owplLogReportProc = NULL;

MY_DLLEXPORT OWPL_RESULT 
owplLogListenerSet(OWPL_LOG_REPORT_PROC pLogReportProc)
{
	owplLogReportProc = pLogReportProc;
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplInit(	const int asyncCallbackMode,
			const int udpPort, 
			const int tcpPort, 
			const int tlsPort, 
			const char* szBindToAddr,
			const int bUserSequentialPorts)
{
	int i;

	phcb = (phCallbacks_t * ) malloc(sizeof(phCallbacks_t));
	memset(phcb, 0, sizeof(phCallbacks_t));

	phcfg.asyncmode = asyncCallbackMode;

	ph_avcodec_init();
	ph_calls_init();

	if (udpPort > 0) {
		snprintf(phcfg.sipport, sizeof(phcfg.sipport), "%d", udpPort);
	}

	if (phcfg.use_tunnel)
	{
		i = ph_tunnel_init();
		if (i)
		{
			return i;
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

	ph_media_init(phcfg.use_tunnel, phcfg.plugin_path);

	i = eXosip_init(0, 0, atoi(phcfg.sipport), phTunnel);

	if (i)
	{
		return OWPL_RESULT_FAILURE;
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
		/* <UOLFONE> */
	}

	ph_nat_init();

/* This should be done in the setter of phcfg.proxy
	if (phcfg.force_proxy)
	{
		eXosip_force_proxy(phcfg.proxy);
	}
*/

	{
		char contact[512];

		eXosip_guess_contact_uri(phcfg.identity, contact, sizeof(contact), 1);
		eXosip_set_answer_contact(contact);
	}

	ph_vlines_init();

	ph_payloads_init();

	if (!phcfg.audio_dev || phcfg.audio_dev[0] == '\0')
	{
		// Set default audio device if no one has been set before
		owplAudioSetConfigString(0);
	}

	/* register callbacks? */
	eXosip_set_mode(EVENT_MODE);

	if (!phcfg.asyncmode)
	{
		phWaitTimeout = 1;
	}
	else 
	{
		phWaitTimeout = 500;
	}

	if (phcfg.asyncmode)
	{
		osip_thread_create(20000, ph_api_thread, 0);
	}

	ph_media_stop_mutex = g_mutex_new();

	phIsInitialized = 1;

	DEBUGTRACE("PhInit finished\n");

	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplShutdown() {
	phTerminate();
	owplEventListenerRemoveAll();
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplUseHttpTunnel(int Use)
{
	if (Use)
	{
//		ph_tunnel_init();
	}

	return OWPL_RESULT_SUCCESS;
}

/********************************************************************************************
 *								Phone CONFIG related functions								*
 ********************************************************************************************/

MY_DLLEXPORT OWPL_RESULT 
owplConfigSetLocalHttpProxy(const char* szLocalProxyAddr, 
					 const unsigned int LocalProxyPort,
					 const char* szLocalProxyUserName,
					 const char* szLocalProxyPasswd)
{
	if(szLocalProxyAddr != NULL) {
		strncpy(phcfg.http_proxy, szLocalProxyAddr, sizeof (phcfg.http_proxy));
	} else {
		phcfg.http_proxy[0] = 0;
	}
	phcfg.http_proxy_port = LocalProxyPort;
	if(szLocalProxyUserName != NULL) {
		strncpy(phcfg.http_proxy_user, szLocalProxyUserName, sizeof (phcfg.http_proxy_user));
	} else {
		phcfg.http_proxy_user[0] = 0;
	}
	if(szLocalProxyPasswd != NULL) {
		strncpy(phcfg.http_proxy_passwd, szLocalProxyPasswd, sizeof (phcfg.http_proxy_passwd));
	} else {
		phcfg.http_proxy_passwd[0] = 0;
	}

	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT 
owplConfigSetTunnel(const char* szTunnelSeverAddr, 
					 const unsigned int TunnelServerPort,
					 const unsigned int TunnelMode)
{
	if(szTunnelSeverAddr != NULL) {
		strncpy(phcfg.httpt_server, szTunnelSeverAddr, sizeof (phcfg.httpt_server));
	} else {
		phcfg.httpt_server[0] = 0;
	}
	phcfg.httpt_server_port = TunnelServerPort;
	phcfg.use_tunnel = TunnelMode;

	return OWPL_RESULT_SUCCESS;
}
/* Deprecated, use owplLineSetAutoKeepAlive instead */
MY_DLLEXPORT OWPL_RESULT
owplConfigSetNat(const OWPL_NAT_TYPE eNatType,
				 const unsigned int natRefreshTime)
{
	switch(eNatType) {
		case OWPL_NAT_TYPE_NONE :
			strncpy(phcfg.nattype, "none", sizeof(phcfg.nattype));
			break;
		case OWPL_NAT_TYPE_AUTO:
			strncpy(phcfg.nattype, "auto", sizeof(phcfg.nattype));
			break;
		case OWPL_NAT_TYPE_FCONE :
			strncpy(phcfg.nattype, "fcone", sizeof(phcfg.nattype));
			break;
		case OWPL_NAT_TYPE_RCONE :
			strncpy(phcfg.nattype, "rcone", sizeof(phcfg.nattype));
			break;
		case OWPL_NAT_TYPE_PRCONE :
			strncpy(phcfg.nattype, "prcone", sizeof(phcfg.nattype));
			break;
		case OWPL_NAT_TYPE_SYMETRIC :
			strncpy(phcfg.nattype, "sym", sizeof(phcfg.nattype));
			break;
		default :
			break;
	}
	phcfg.nat_refresh_time = natRefreshTime;
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplConfigSetOutboundProxy(const char * szProxyAddr)
{
	if(szProxyAddr != NULL) {
		strncpy(phcfg.proxy, szProxyAddr, sizeof(phcfg.proxy));
	} else {
		phcfg.proxy[0] = 0;
	}
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT 
owplConfigAddAudioCodecByName(const char* szCodecName)
{
	/* TODO */
	/* Verify that the code name is supported before adding to the list */
	if(szCodecName == NULL || strlen(szCodecName) == 0) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	if (strlen(phcfg.audio_codecs) <= 0)
	{
		strcpy(phcfg.audio_codecs, szCodecName);
	}
	else if (strstr(phcfg.audio_codecs,szCodecName) == 0)
	{
		strcat(phcfg.audio_codecs, ",");
		strcat(phcfg.audio_codecs, szCodecName);
	}
	return OWPL_RESULT_SUCCESS;
}


MY_DLLEXPORT OWPL_RESULT
owplConfigSetAudioCodecs(const char * szCodecs) {
	if(szCodecs == NULL || strlen(szCodecs) == 0) {
		phcfg.audio_codecs[0] = 0;
		return OWPL_RESULT_SUCCESS;
	}
	if(strlen(szCodecs) >= sizeof(phcfg.audio_codecs)) {
		return OWPL_RESULT_INVALID_ARGS;
	}
	memset(phcfg.audio_codecs, 0, sizeof(phcfg.audio_codecs));
	if(strlen(strncpy(phcfg.audio_codecs, szCodecs, sizeof(phcfg.audio_codecs))) == 0) {
		return OWPL_RESULT_FAILURE;
	}

	if (phIsInitialized) {
		ph_payloads_init();
	}

	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplConfigGetAudioCodecs(const char * szCodecs, size_t size) {
	if(szCodecs == NULL) {
		return OWPL_RESULT_INVALID_ARGS;
	}
	memset(szCodecs, 0, size);
	if(size < sizeof(phcfg.audio_codecs)) {
		return OWPL_RESULT_INSUFFICIENT_BUFFER;
	}
	if(strlen(strncpy(szCodecs, phcfg.audio_codecs, size)) == 0) {
		return OWPL_RESULT_FAILURE;
	}
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT 
owplConfigAddVideoCodecByName(const char* szCodecName)
{
	/* TODO */
	/* Verify that the code name is supported before adding to the list */
	if(szCodecName == NULL || strlen(szCodecName) == 0) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	if (strlen(phcfg.video_codecs) <= 0)
	{
		strcpy(phcfg.video_codecs, szCodecName);
	}
	else if (strstr(phcfg.video_codecs,szCodecName) < 0)
	{
		strcat(phcfg.video_codecs, ",");
		strcat(phcfg.video_codecs, szCodecName);
	}
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplConfigSetVideoCodecs(const char * szCodecs) {
	if(szCodecs == NULL || strlen(szCodecs) == 0) {
		phcfg.video_codecs[0] = 0;
		return OWPL_RESULT_SUCCESS;
	}
	if(strlen(szCodecs) >= sizeof(phcfg.video_codecs)) {
		return OWPL_RESULT_INVALID_ARGS;
	}
	memset(phcfg.video_codecs, 0, sizeof(phcfg.video_codecs));
	if(strlen(strncpy(phcfg.video_codecs, szCodecs, sizeof(phcfg.video_codecs))) == 0) {
		return OWPL_RESULT_FAILURE;
	}
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplConfigGetVideoCodecs(const char * szCodecs, size_t size) {
	if(szCodecs == NULL) {
		return OWPL_RESULT_INVALID_ARGS;
	}
	memset(szCodecs, 0, size);
	if(size < sizeof(phcfg.video_codecs)) {
		return OWPL_RESULT_INSUFFICIENT_BUFFER;
	}
	if(strlen(strncpy(szCodecs, phcfg.video_codecs, size)) == 0) {
		return OWPL_RESULT_FAILURE;
	}
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplConfigGetBoundLocalAddr(char * szLocalAddr, size_t size) {
	char ip[256]; // put a big buffer to prevent buffer overflow...

	if(szLocalAddr == NULL) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	memset(szLocalAddr, 0, size);

	if(size <= 0) {
		return OWPL_RESULT_INSUFFICIENT_BUFFER;
	}
	memset(ip, 0, sizeof(ip));
	// HACK : eXosip_get_localip should get the size of the buffer to prevent buffer overflow
	eXosip_get_localip(ip);
	ip[sizeof(ip)-1] = '\0'; // be sure to have a zero terminated string
	if(strlen(ip) > size-1) {
		return OWPL_RESULT_INSUFFICIENT_BUFFER;
	}
	strncpy(szLocalAddr, ip, size-1);
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplConfigLocalHttpProxyGetAddr(char * szLocalProxyAddr, size_t size) {
	size_t sourceSize = strlen(phcfg.http_proxy);

	if(szLocalProxyAddr == NULL) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	memset(szLocalProxyAddr, 0, size);

	if(size <= 0 || sourceSize > size-1) {
		return OWPL_RESULT_INSUFFICIENT_BUFFER;
	}
	if(sourceSize <= 0 ) {
		return OWPL_RESULT_FAILURE;
	}
	strncpy(szLocalProxyAddr, phcfg.http_proxy, size-1);
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplConfigLocalHttpProxyGetPasswd(char * szLocalProxyPasswd, size_t size) {
	size_t sourceSize = strlen(phcfg.http_proxy_passwd);

	if(szLocalProxyPasswd == NULL) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	memset(szLocalProxyPasswd, 0, size);

	if(size <= 0 || sourceSize > size-1) {
		return OWPL_RESULT_INSUFFICIENT_BUFFER;
	}
	if(sourceSize <= 0 ) {
		return OWPL_RESULT_FAILURE;
	}
	strncpy(szLocalProxyPasswd, phcfg.http_proxy_passwd, size-1);
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplConfigLocalHttpProxyGetPort(int * LocalProxyPort) {
	if(LocalProxyPort == NULL) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	*LocalProxyPort = 0;

	if(phcfg.http_proxy_port <= 0) {
		return OWPL_RESULT_FAILURE;
	}
	*LocalProxyPort = phcfg.http_proxy_port;
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplConfigLocalHttpProxyGetUserName(char * szLocalProxyUserName, size_t size) {
	size_t sourceSize = strlen(phcfg.http_proxy_user);

	if(szLocalProxyUserName == NULL) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	memset(szLocalProxyUserName, 0, size);

	if(size <= 0 || sourceSize > size-1) {
		return OWPL_RESULT_INSUFFICIENT_BUFFER;
	}
	if(sourceSize <= 0) {
		return OWPL_RESULT_FAILURE;
	}
	strncpy(szLocalProxyUserName, phcfg.http_proxy_user, size-1);
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT owplConfigSetEncryptionMode(int EncryptionMode)
{
	sVoIP_phapi_setCipherMode(EncryptionMode ? 1:0);
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT owplConfigGetEncryptionMode(int *EncryptionMode)
{
	*EncryptionMode = sVoIP_phapi_getCipherMode();
	return OWPL_RESULT_SUCCESS;
}

// SPIKE_SPIKE_SIP_SIMPLE
MY_DLLEXPORT OWPL_RESULT owplConfigEnablePIM(int PimEnabled)
{
	phcfg.pim_disabled = !PimEnabled;
	return OWPL_RESULT_SUCCESS;
}

/********************************************************************************************
 *								Phone AUDIO CONFIG related functions						*
 ********************************************************************************************/

/**
 * Set the audio config string
 * @param szAudioConfig		The audio configuration string
 */
MY_DLLEXPORT OWPL_RESULT owplAudioSetConfigString(const char* szAudioConfig)
{
	char *szEnvValue;
	char *szFinalAudioConfig;
	/*
	Audio device selection:
	if we have PH_FORCE_AUDIO_DEVICE env var it overrides everything else
	otherwise we try to use the device specified by the UI....
	if UI didn't specify anything we try to use content of PH_AUDIO_DEVICE env var (if it is nonempty)
	and in the last resort we use PortAudio default device
	*/
	szEnvValue = getenv("PH_FORCE_AUDIO_DEVICE");

	if (szEnvValue && szEnvValue[0])
	{
		szFinalAudioConfig = szEnvValue;
	}
	else if (szAudioConfig && szAudioConfig[0])
	{
		szFinalAudioConfig = szAudioConfig;
	}
	else
	{
		szEnvValue = getenv("PH_AUDIO_DEVICE");
		if (szEnvValue && szEnvValue[0])
		{
			szFinalAudioConfig = szEnvValue;
		}
		else
		{
#if defined(OS_MACOSX)
			strncpy(phcfg.audio_dev, "ca:", sizeof(phcfg.audio_dev));
#else
			strncpy(phcfg.audio_dev, "pa:", sizeof(phcfg.audio_dev));
#endif
			return OWPL_RESULT_SUCCESS;
		}
	}

	if (strlen(szFinalAudioConfig) >= sizeof(phcfg.audio_dev)) {
		return OWPL_RESULT_INVALID_ARGS;
	}
	strncpy(phcfg.audio_dev, szFinalAudioConfig, sizeof(phcfg.audio_dev));
	return OWPL_RESULT_SUCCESS;
}

/********************************************************************************************
 *								Phone LINE related functions								*
 ********************************************************************************************/

/**
 * owplLineAdd
 *
 * add a virtual line 
 * 
 * @param  displayname
 * @param  username
 * @param  server
 * @param  proxy
 * @param  regTimeout	The value that will be put in Expire header of register message
 * @param(out)  phLine  The newly created line handle
 * @return				OWPL_RESULT_SUCCESS in case of success
 */
MY_DLLEXPORT OWPL_RESULT
owplLineAdd(const char* displayname,
			 const char* username, 
			 const char* server, 
			 const char*  proxy,  
			 int regTimeout,
			 OWPL_LINE * phLine)
{	
	int ret;
	phVLine *vl;

	vl = ph_find_matching_vline2(username, server, 0);

	if (vl) {
		vl->regTimeout = regTimeout;
		*phLine = ph_vline2vlid(vl);
	}
	else {
		/* Call phAddVline2 with timeout = 0 to avoid sending right away the REGISTER message.
		 * (this function send a REGISTER message if timeout > 0 and we have shouldn't change it
		 * for backward compatibility
		 */
		ret = phAddVline2(displayname, username, server, proxy, 0); 
		if (ret < 0) {
			return OWPL_RESULT_FAILURE;
		}
		vl = ph_vlid2vline(ret);
		if (vl)	{
			vl->regTimeout = regTimeout;
		}
		*phLine = ret;
	}
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplLineDelete(OWPL_LINE hLine, unsigned int skipUnregister) {
	if(skipUnregister) {
		if(phDelVline(hLine, 0) == 0) {
			return OWPL_RESULT_SUCCESS;
		} else {
			return OWPL_RESULT_FAILURE;
		}
	}

	if(phDelVline(hLine, -1) == 0) {
		return OWPL_RESULT_SUCCESS;
	} else {
		return OWPL_RESULT_FAILURE;
	}
}

/**
 * owplLineRegister
 *
 * register a virtual line 
 * 
 * @param  hLine	   handle to the virtual line
 * @param  bRegister   1: Register the line; 0: Unregister the line
 * @return             OWPL_RESULT_SUCCESS in case of success
 */
MY_DLLEXPORT OWPL_RESULT
owplLineRegister(const OWPL_LINE hLine, const int bRegister)
{
	int ret;
	if (bRegister) {
		ret = phvlRegister(hLine);
	}
	else {
		ret = phvlUnregister(hLine);
	}
	
	if (ret < 0) {
		return OWPL_RESULT_FAILURE;
	}
	return OWPL_RESULT_SUCCESS;
}


MY_DLLEXPORT OWPL_RESULT
owplLineSetOpts(const OWPL_LINE hLine, LineOptEnum Opt, const void *Data)
{
	int *n;
	phVLine *vl = 0;
	char *s;

	vl = ph_valid_vlid(hLine);
	if (!vl) {
		return OWPL_RESULT_FAILURE;
	}

	switch (Opt)
	{
	case OWPL_LINE_OPT_REG_TIMEOUT:
		n = (int*) Data;
		vl->regTimeout = *n;
		break;
	case OWPL_LINE_OPT_PROXY:
		s = (char*) Data;
		if (strcmp(vl->proxy, s) != 0)
		{
			if (vl->LineState == LINESTATE_REGISTERED) {
				// Unregister it first
				owplLineRegister(hLine, 0);
			}
			strcpy(vl->proxy, s);
		}
		break;
	case OWPL_LINE_OPT_DOMAIN:
		s = (char*) Data;
		if (strcmp(vl->server, s) != 0)
		{
			if (vl->LineState == LINESTATE_REGISTERED) {
				// Unregister it first
				owplLineRegister(hLine, 0);
			}
			strcpy(vl->server, s);
		}
		break;
	}

	return OWPL_RESULT_SUCCESS;
}

/**
 * owplLineGetProxy
 *
 * Get the proxy address of the line
 * 
 * @param  hLine	   handle to the virtual line
 * @param  szBuffer    The buffer to contain the result. 
 * @param  nBuffer	   The size of the buffer. On return, nBuffer will contain the actual by written to the buffer.
 * @return             OWPL_RESULT_SUCCESS in case of success. On failure, nBuffer will contain
 *					   the size needed to store the result.
 */
MY_DLLEXPORT OWPL_RESULT 
owplLineGetProxy(const OWPL_LINE hLine,
                        char*  szBuffer,
                        int * nBuffer)
{
	int n = 0;
	phVLine *vl = 0;

	vl = ph_valid_vlid(hLine);
	if (!vl) {
		return OWPL_RESULT_FAILURE;
	}
		
	if (vl->proxy && strlen(vl->proxy)) {
		n += strlen(vl->proxy); // +1 for \0 at the end of the string
	}
	
	if (n >= *nBuffer) {
		*nBuffer = n;
		return OWPL_RESULT_INSUFFICIENT_BUFFER;
	}
	if (szBuffer) {		
		strncpy(szBuffer, vl->proxy, n);
		szBuffer[n] = '\0';
	}
	return OWPL_RESULT_SUCCESS;
}

/**
 * owplLineGetUserName
 *
 * Get username of the line
 * 
 * @param  hLine	   handle to the virtual line
 * @param  szBuffer    The buffer to contain the result. 
 * @param  nBuffer	   The size of the buffer. On return, nBuffer will contain the actual by written to the buffer.
 * @return             OWPL_RESULT_SUCCESS in case of success. On failure, nBuffer will contain
 *					   the size needed to store the result.
 */
MY_DLLEXPORT OWPL_RESULT 
owplLineGetLocalUserName(const OWPL_LINE hLine,
                        char*  szLocalUserName,
                        int * nBuffer)
{
	int n = 0;
	phVLine *vl = 0;

	vl = ph_valid_vlid(hLine);
	if (!vl) {
		return OWPL_RESULT_FAILURE;
	}
		
	if (vl->username && strlen(vl->username)) {
		n += strlen(vl->username); // +1 for \0 at the end of the string
	}
	
	if (n >= *nBuffer) {
		*nBuffer = n;
		return OWPL_RESULT_INSUFFICIENT_BUFFER;
	}
	if (szLocalUserName) {		
		strncpy(szLocalUserName, vl->username, n);
		szLocalUserName[n] = '\0';
	}
	return OWPL_RESULT_SUCCESS;
}

/**
 * owplLineGetUri
 *
 * Get uri of the line
 * 
 * @param  hLine	   handle to the virtual line
 * @param  szBuffer    The buffer to contain the result. 
 * @param  nBuffer	   The size of the buffer. If 
 * @return             OWPL_RESULT_SUCCESS in case of success. On failure, nBuffer will contain
 *					   the size needed to store the result.
 */
MY_DLLEXPORT OWPL_RESULT 
owplLineGetUri(const OWPL_LINE hLine,
                        char*  szBuffer,
                        int * nBuffer)
{
	char tmp[10];
	int n = 0;
	phVLine *vl = 0;

	vl = ph_valid_vlid(hLine);
	if (!vl) {
		return OWPL_RESULT_FAILURE;
	}
	
	if (vl->server && strlen(vl->server)) {
		n += strlen(vl->server);
	}
	if (vl->username && strlen(vl->username)) {
		n += strlen(vl->username);
	}
	if (vl->port != 5060) {
		snprintf(tmp, sizeof(tmp), "%d", vl->port);
		n += strlen(tmp) + 1;
	}
	n += 5;	
	if (n > *nBuffer) {
		*nBuffer = n;
		return OWPL_RESULT_INSUFFICIENT_BUFFER;
	}
	if (szBuffer) {
		if (vl->port != 5060) {
			snprintf(szBuffer, *nBuffer, "sip:%s@%s:%d", vl->username, vl->server, vl->port);
		}
		else {
			snprintf(szBuffer, *nBuffer, "sip:%s@%s", vl->username, vl->server);
		}
	}
	return OWPL_RESULT_SUCCESS;
}

/**
 * owplLineAddCredential
 *
 * Add credential information to a line
 */
MY_DLLEXPORT OWPL_RESULT
owplLineAddCredential(const OWPL_LINE hLine,                                                 
						const char* szUserID,
						const char* szPasswd,
						const char* szRealm)
{
	int ret;

	if (!szUserID
		|| !szPasswd
		|| !szRealm)
	{
		return OWPL_RESULT_INVALID_ARGS;
	}

	eXosip_lock();
	ret = eXosip_add_authentication_info(szUserID, szUserID, szPasswd, "", szRealm);
	eXosip_unlock();

	if(ret != 0) {
		return OWPL_RESULT_FAILURE;
	}
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplLineSetBusy(OWPL_LINE hLine,
				unsigned int bBusy) {
	if(phLineSetBusy(hLine, bBusy) != 0) {
		return OWPL_RESULT_FAILURE;
	}
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplLineIsBusy(OWPL_LINE hLine,
			   unsigned int * bBusy) {
	phVLine *vl = ph_valid_vlid(hLine);

	if(!vl) {
		return OWPL_RESULT_FAILURE;
	}

	*bBusy = vl->busy;
	return OWPL_RESULT_SUCCESS;
}

/**
 * owplLineSetAutoKeepAlive
 *
 * Get the proxy address of the line
 * 
 * @param  hLine		handle to the virtual line
 * @param  Enabled		1: Enabled, 0: Disabled
 * @param  Period		The timeout period between 2 keep alive messages (in seconds)
 * @return				OWPL_RESULT
 */
MY_DLLEXPORT OWPL_RESULT
owplLineSetAutoKeepAlive(OWPL_LINE hLine,
			   unsigned int Enabled,
			   unsigned int Period)
{
	phVLine *vl = ph_valid_vlid(hLine);

	if(!vl) {
		return OWPL_RESULT_FAILURE;
	}
	
	vl->keepAliveInfo.enabled = Enabled;
	vl->keepAliveInfo.period = Period;
	vl->keepAliveInfo.lastSentTime = time(0);
	return OWPL_RESULT_SUCCESS;
}

/********************************************************************************************
 *								Phone CALL related functions								*
 ********************************************************************************************/

int owStaticCallId = 0;

int getNextCallId()
{
	return ++owStaticCallId;
}

/**
 * Create a call object
 *
 * @param hLine			Handle to the line that is used for the call
 * @param hCall			The newly created call handle will be stored in this param
 */

MY_DLLEXPORT OWPL_RESULT
owplCallCreate( const OWPL_LINE hLine,
				 OWPL_CALL * hCall)
{
	phVLine *vl;
	int newCallId;
	phcall_t *ca;

	//Check hLine validity
	vl = ph_valid_vlid(hLine);
	if (!vl) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	newCallId = getNextCallId();
	ca = ph_allocate_call(newCallId);
	if ( ca == 0)
	{
		return OWPL_RESULT_FAILURE;
	}

	ca->vlid = hLine;
	*hCall = newCallId;
	return OWPL_RESULT_SUCCESS;
}

/**
 * Connect an already created call
 *
 * @param hCall				The handle to the created call.
 * @param szAddress			The SIP URI to call
 * @param mediaStreams		The flag to say which kind of media will be available.
 *							The value can be composed of the following bit :
 *							OWPL_STREAM_AUDIO, OWPL_STREAM_VIDEO_RX, OWPL_STREAM_VIDEO_TX
 */
MY_DLLEXPORT OWPL_RESULT
owplCallConnect(const OWPL_CALL hCall,
				const char* szAddress,
				int mediaStreams)
{
	phcall_t *ca = ph_locate_call_by_cid(hCall);

	if (phLinePlaceCall_withCa(ca->vlid, szAddress, 0, 0, mediaStreams, ca) <= 0)
	{
		return OWPL_RESULT_FAILURE;
	}

	return OWPL_RESULT_SUCCESS;
}

/**
 * Connect an already created call with a custom body
 *
 * @param hCall				The handle to the created call.
 * @param szAddress			The SIP URI to call
 * @param szContentType		The content type of the message
 * @param body				The body of the message 
 * @param BodySize			The size of the body of the message 
 *
 * ATTENTION: BodySize param is not yet taken into account by implementation. For now, the size is strlen(body).
 * So, body should be a zero terminated string
 *							
 */

MY_DLLEXPORT OWPL_RESULT
owplCallConnectWithBody(const OWPL_CALL hCall,
				const char* szAddress,
				const char* szContentType,
				const char* szBody,
				int BodySize)
{
	int i;
	osip_message_t *invite;
	char *proxy = phcfg.proxy;
	phVLine *vl;
	char from[512];

	phcall_t *ca = ph_locate_call_by_cid(hCall);

	// TODO verif des arguments
	if (!szAddress){
		return OWPL_RESULT_INVALID_ARGS;
	}

	vl = ph_valid_vlid(ca->vlid);
	if (!vl) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	ph_vline_get_from(from, sizeof(from), vl);

	proxy = vl->proxy;

	if((i = eXosip_build_initial_invite(&invite, szAddress, from, proxy, "")) != 0){
		return -1;
	}

	eXosip_lock();

	i = eXosip_initiate_call_with_body(invite, szContentType, szBody, 0);
	if (i <= 0) {
		return OWPL_RESULT_FAILURE;
	}

	ca->extern_cid = i;
	ca->vlid = ph_vline2vlid(vl);

	eXosip_unlock(); 

	owplAssociateCall2PluginByContentType(ca->cid, szContentType);

	return OWPL_RESULT_SUCCESS;
}

/**
 * Accept an incoming call. The message 180 RINGING is sent *ONLY* after the call to this function
 *
 * @param hCall				The handle to the incoming call.
 * @param mediaStreams		The flag to say which kind of media will be available.
 *							The value can be composed of the following bit :
 *							OWPL_STREAM_AUDIO, OWPL_STREAM_VIDEO_RX, OWPL_STREAM_VIDEO_TX
 */
MY_DLLEXPORT OWPL_RESULT
owplCallAccept(const OWPL_CALL hCall, int mediaStreams)
{
	int ret = phRingingCall(hCall);

	if (ret != 0) {
		return OWPL_RESULT_FAILURE;
	}
	return OWPL_RESULT_SUCCESS;
}

/**
 * Answer an incoming call
 *
 * @param hCall				The handle to the incoming call. 
 * @param mediaStreams		The flag to say which kind of media will be available.
 *							The value can be composed of the following bit :
 *							OWPL_STREAM_AUDIO, OWPL_STREAM_VIDEO_RX, OWPL_STREAM_VIDEO_TX
 */
MY_DLLEXPORT OWPL_RESULT
owplCallAnswer(const OWPL_CALL hCall, int mediaStreams)
{
	int ret = phAcceptCall3(hCall, 0, mediaStreams);

	if (ret != 0) {
		return OWPL_RESULT_FAILURE;
	}
	return OWPL_RESULT_SUCCESS;
}

/**
 * Answer an incoming call with the specified body
 *
 * @param hCall				The handle to the incoming call.
 * @param szContentType		The content type of the message
 * @param body				The body of the message 
 * @param BodySize			The size of the body of the message
 */
MY_DLLEXPORT OWPL_RESULT
owplCallAnswerWithBody (const OWPL_CALL hCall,
			const char* szContentType,
			const char* szBody,
			int BodySize) {
	int i;
	phcall_t *ca = ph_locate_call_by_cid(hCall);

	if(!ca) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	eXosip_lock();
	i = eXosip_answer_call_with_body(ca->did, 200, szContentType, szBody); // returns 0 on success; -1 else
	eXosip_unlock();

	if(i != 0) {
		return OWPL_RESULT_FAILURE;
	}

	return OWPL_RESULT_SUCCESS;
}

/**
 * Reject an incoming call
 *
 * @param hCall				The handle to the incoming call.
 * @param errorCode			The SIP error code (eg. 486 - Busy Here, 488 - Not acceptable here)
 * @param szErrorText		The text that will be sent in SIP response with the error code
 */
MY_DLLEXPORT OWPL_RESULT
owplCallReject(const OWPL_CALL hCall,
				const int errorCode,
				const char* szErrorText)
{
	int ret = phRejectCall(hCall, errorCode);
	if (ret != 0) {
		return OWPL_RESULT_FAILURE;
	}
	return OWPL_RESULT_SUCCESS; 
}

/**
 * Close an active call
 *
 * @param hCall				The handle to the active call. 
 */

MY_DLLEXPORT OWPL_RESULT owplCallDisconnect(OWPL_CALL hCall)
{
	int ret = phCloseCall(hCall);
	if (ret != 0) {
		return OWPL_RESULT_FAILURE;
	}
	return OWPL_RESULT_SUCCESS; 
}

/**
 * Reject an incoming call with a predefined reason
 *
 * @param hCall				The handle to the incoming call.
 * @param Reason			The predefined reason
 */
MY_DLLEXPORT OWPL_RESULT
owplCallRejectWithPredefinedReason(const OWPL_CALL hCall,
				   OWPL_CALL_REFUSED_REASON Reason)
{
	switch(Reason) {
		case OWPL_CALL_REFUSED_BUSY :
			if(phReject(hCall) != 0) {
				return OWPL_RESULT_FAILURE;
			}
			break;
		default :
			return OWPL_RESULT_INVALID_ARGS;
			break;
	}

	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT 
owplCallHold(const OWPL_CALL hCall) {
	if(phHoldCall(hCall) == 0) {
		return OWPL_RESULT_SUCCESS;
	}
	return OWPL_RESULT_FAILURE;
}

MY_DLLEXPORT OWPL_RESULT 
owplCallHoldWithBody(const OWPL_CALL hCall, 
		     const char * szContentType, 
		     const char * szBody, 
		     int BodySize) {
	phcall_t *ca = ph_locate_call_by_cid(hCall);
	int i;

	if(!ca) {
		return OWPL_RESULT_FAILURE;
	}

	if(ca->localhold) {
		return OWPL_RESULT_FAILURE;
	}

	ca->localhold = 1;

	eXosip_lock();
	i = eXosip_on_hold_call_with_body(ca->did, szContentType, szBody);
	eXosip_unlock();

	if(i==0) {
		return OWPL_RESULT_SUCCESS;
	}
	return OWPL_RESULT_FAILURE;
}

MY_DLLEXPORT OWPL_RESULT
owplCallUnhold(const OWPL_CALL hCall) {
	if(phResumeCall(hCall) == 0) {
		return OWPL_RESULT_SUCCESS;
	}
	return OWPL_RESULT_FAILURE;
}

MY_DLLEXPORT OWPL_RESULT
owplCallUnholdWithBody(const OWPL_CALL hCall, 
					   const char * szContentType, 
					   const char * szBody, 
					   int BodySize) 
{
	phcall_t *ca = ph_locate_call_by_cid(hCall);
	int i;

	if(!ca) {
		return OWPL_RESULT_FAILURE;
	}

	if(ca->localhold != 1) {
		return OWPL_RESULT_FAILURE;
	}

	ca->localhold = 0;

	eXosip_lock();
	i = eXosip_off_hold_call_with_body(ca->did, szContentType, szBody);
	eXosip_unlock();

	if(i==0) {
		return OWPL_RESULT_SUCCESS;
	}
	return OWPL_RESULT_FAILURE;
}

MY_DLLEXPORT OWPL_RESULT owplCallGetEncryptionMode(const OWPL_CALL hCall,
													int * EncryptionMode)
{
	phcall_t *ca = ph_locate_call_by_cid(hCall);

	if(!ca) {
		return OWPL_RESULT_FAILURE;
	}
	*EncryptionMode = sVoIP_phapi_isCrypted(ca->extern_cid);

	return OWPL_RESULT_SUCCESS;
}

/********************************************************************************************
 *								Presence related functions								*
 ********************************************************************************************/

/**
 * Sends a SUBSCRIBE message to a sip URI.
 * 
 * @param hLine		The handle to the line.
 * @param szUri		Sip address of the person to subscribe to
 * @param winfo		0: Subscribe for presence
					1: Subscribe for watcher info
 * @param hSub		The handle to this subscription

 */
MY_DLLEXPORT OWPL_RESULT owplPresenceSubscribe(OWPL_LINE  hLine,
                                          const char* szUri,
                                          const int winfo,
										  OWPL_SUB *hSub)
{
	char UriBuf[100];
	char MsgBodyBuf[500];
	char ProxyBuf[100];
	int n = sizeof(UriBuf);
	int i;
	
	// SPIKE_SPIKE_SIP_SIMPLE
	if (phcfg.pim_disabled) {
		return OWPL_RESULT_SUCCESS;
	}

	owplLineGetUri(hLine, UriBuf, &n);

	n = sizeof(ProxyBuf);
	owplLineGetProxy(hLine, ProxyBuf, &n);

	eXosip_lock();
	i = eXosip_subscribe(szUri, UriBuf, ProxyBuf, winfo);
	eXosip_unlock();
	if (i < 0) {
		return OWPL_RESULT_FAILURE;
	}
	if (hSub) {
		*hSub = i;
	}
	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplPresenceUnsubscribe(const char * szRemoteUri) {
	OWPL_SUB hSub;
	int i = 0;

	// SPIKE_SPIKE_SIP_SIMPLE
	if (phcfg.pim_disabled) {
		return OWPL_RESULT_SUCCESS;
	}

	if(szRemoteUri == NULL || strlen(szRemoteUri) <= 0) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	eXosip_lock();
	i = eXosip_get_subscribe_id(szRemoteUri, &hSub);
	if(i == 0) {
		i = eXosip_subscribe_close(hSub);
	}
	eXosip_unlock();
	if(i != 0) {
		owplFireSubscriptionEvent(hSub,
			OWPL_SUBSCRIPTION_CLOSE_FAILED,
			SUBSCRIPTION_CAUSE_UNKNOWN,
			szRemoteUri);
	} else {
		owplFireSubscriptionEvent(hSub,
			OWPL_SUBSCRIPTION_CLOSED,
			SUBSCRIPTION_CAUSE_NORMAL,
			szRemoteUri);
	}
	return OWPL_RESULT_SUCCESS;
}

/**
 * Sends a PUBLISH message to change the presence of the current user.
 * 
 * @param hLine			The handle to the line.
 * @param Online		0: Offline
						1: Online
 * @param szStatus		The status message if Online is 1
 * @param hPub			The handle to this publish (Unused now)
 */

#define PUBLISH_MSG_TEMPLATE "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<presence entity=\"%s\">\n\
<tuple id=\"azersdqre\">\n\
<status><basic>%s</basic>\
<note>%s</note></status>\n\
<contact priority=\"1\">%s\
</contact>\n\
</tuple>\n\
</presence>\n"

#define PRESENCE_CONTENT_TYPE "application/pidf+xml"


MY_DLLEXPORT OWPL_RESULT owplPresencePublish(OWPL_LINE  hLine,
                                          const int Online,
                                          const char * szStatus,
										  OWPL_PUB *hPub)
{
	char UriBuf[100];
	char MsgBodyBuf[500];
	char ProxyBuf[100];
	int n = sizeof(UriBuf);
	int i;
	phVLine * vl = NULL;

	// SPIKE_SPIKE_SIP_SIMPLE
	if (phcfg.pim_disabled) {
		return OWPL_RESULT_SUCCESS;
	}

	// save infos for later user from a timer event
	if((vl = ph_valid_vlid(hLine)) != NULL) {
		vl->publishInfo.onlineState = Online;
		if(szStatus != NULL) {
			vl->publishInfo.szStatus = strdup(szStatus);
		}
		vl->publishInfo.hPub = hPub;
		// nine minutes timeout i.e. 540s
		vl->publishInfo.publishTimeout = 540;
		vl->publishInfo.lastPublishTime = time(0);
	} else {
		return OWPL_RESULT_FAILURE;
	}

	owplLineGetUri(hLine, UriBuf, &n);
	snprintf(MsgBodyBuf, sizeof(MsgBodyBuf), PUBLISH_MSG_TEMPLATE, UriBuf, Online ? "open" : "close", szStatus, UriBuf);

	n = sizeof(ProxyBuf);
	owplLineGetProxy(hLine, ProxyBuf, &n);

	eXosip_lock();
	i = eXosip_publish(UriBuf, UriBuf, ProxyBuf, 0, PRESENCE_CONTENT_TYPE, MsgBodyBuf);
	eXosip_unlock();
	
	if (i != 0) {
		return OWPL_RESULT_FAILURE;
	}
	return OWPL_RESULT_SUCCESS;
}

/********************************************************************************************
 *								Message related functions								*
 ********************************************************************************************/

MY_DLLEXPORT OWPL_RESULT
owplMessageSend(OWPL_LINE hLine,
				const char * szRemoteUri,
				const char * szContent,
				const char * szMIME,
				int * messageId)
{
	int i;
	phVLine *vl;
	char from[512];

	// SPIKE_SPIKE_SIP_SIMPLE
	if (phcfg.pim_disabled) {
		return OWPL_RESULT_SUCCESS;
	}

	if(hLine <= 0 
		|| szRemoteUri == NULL
		|| strlen(szRemoteUri) <= 0
		|| szMIME == NULL
		|| strlen(szMIME) <= 0
		|| szContent == NULL
		|| strlen(szContent) <= 0
		|| messageId == NULL)
	{
		return OWPL_RESULT_INVALID_ARGS;
	}

	vl = ph_valid_vlid(hLine);
	if(!vl) {
		return OWPL_RESULT_INVALID_ARGS;
	}

	ph_vline_get_from(from, sizeof(from), vl);

	eXosip_lock();
	*messageId = eXosip_message((char *)szRemoteUri, from, vl->proxy, szContent, szMIME);
	eXosip_unlock();

	return OWPL_RESULT_SUCCESS;
}

MY_DLLEXPORT OWPL_RESULT
owplMessageSendPlainText(OWPL_LINE hLine,
				const char * szRemoteUri,
				const char * szContent,
				int * messageId)
{
	return owplMessageSend(hLine, szRemoteUri, szContent, "text/plain", messageId);
}



MY_DLLEXPORT OWPL_RESULT
owplMessageSendTypingState(OWPL_LINE hLine,
				const char * szRemoteUri,
				OWPL_TYPING_STATE state,
				int * messageId)
{
	switch(state) {
		case OWPL_TYPING_STATE_TYPING :
			return owplMessageSend(hLine, szRemoteUri, "is typing", "typingstate/typing", messageId);

		case OWPL_TYPING_STATE_STOP_TYPING :
			return owplMessageSend(hLine, szRemoteUri, "stops typing", "typingstate/stoptyping", messageId);

		case OWPL_TYPING_STATE_NOT_TYPING :
			return owplMessageSend(hLine, szRemoteUri, "is not typing", "typingstate/nottyping", messageId);

		default :
			break;
	}
	return OWPL_RESULT_FAILURE;
}

MY_DLLEXPORT OWPL_RESULT
owplMessageSendIcon(OWPL_LINE hLine,
				const char * szRemoteUri,
				const char * szIconFileName, 
				int * messageId)
{
	char szMime[1024] = "buddyicon/";

	strncat(szMime, szIconFileName, sizeof(szMime));

	return owplMessageSend(hLine, szRemoteUri, "has changed his icon", szMime, messageId);
}
