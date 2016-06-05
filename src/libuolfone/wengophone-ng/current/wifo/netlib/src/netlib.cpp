/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>

#include <cutil/global.h>

#ifdef OS_WIN32
	#include <winsock2.h>
	#include <windows.h>
	#include <wininet.h>

	#ifndef CC_MINGW
		#include <urlmon.h>
		#include <stdio.h>
		#include <ws2tcpip.h>
		#include <time.h>

		#ifndef snprintf
			#define snprintf _snprintf
		#endif

		#ifndef CC_MSVC8
			#include "bugfix.h"
		#endif

		inline int strncasecmp(const char *str1, const char *str2, int size) {return strnicmp(str1, str2, size);}

	#endif // CC_MINGW
#else
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <errno.h>
	#include <net/if.h>
	#include <sys/ioctl.h>
	#include <netdb.h>

	/*
	typedef int Socket;
	inline int closesocket(Socket fd) {return close(fd);}
	*/
#endif	//WIN32

#include "udp_func.h"
#include "stun_func.h"

#include <curl/curl.h>
#include "httptunnel.h"
#include <netlib.h>
#include "netlib_util.h"

using namespace std;

typedef struct	HttpProxy_s
{
	int			port;
	char		*address;
	long		auth_type;
	long		proxy_auth_type;
	char		**proxy_exceptions;
}				HttpProxy_t;

HttpProxy_t	_LocalProxy = {0, NULL, 0, 0, NULL};


static NetlibLogCbk writeLogCbk = 0;

void netlib_set_write_log_callback(NetlibLogCbk cbk)
{
	writeLogCbk = cbk;
	http_tunnel_set_write_log_callback((HttpTunnelLogCbk)cbk);
}

#define NETLIB_LOG_ERROR(buff) {if (writeLogCbk) writeLogCbk(NETLIB_LOG_ERR, buff); }

int _curloutputcbk(CURL *mycurl, curl_infotype type, char *buffer, size_t size, void * arg)
{
	if (writeLogCbk) {
		NETLIB_LOG_TYPE netlib_type = NETLIB_LOG_UNK;

		switch (type) {
			case CURLINFO_HEADER_IN:
				netlib_type = NETLIB_LOG_IN;
				break;

			case CURLINFO_HEADER_OUT:
				netlib_type = NETLIB_LOG_OUT;
				break;

			case CURLINFO_TEXT:
				netlib_type = NETLIB_LOG_INFO;
				break;

			default:
				return 0;
		}

		writeLogCbk(netlib_type, buffer);
	}

	return 0;
}

/**
 * Empty callback to silence libcurl
 * follows curl_write_callback prototype
 */
static size_t _curlsilentwritecbk(char* /*ptr*/, size_t size, size_t nitems, void* /*outstream*/) {
	return size * nitems;
}

char *_cleanStr(char *str)
{
	int i;

	if (str && *str) {
		for (i = 0; str[i]; i++) {
			if (str[i] == '=') {
				return (&str[i+1]);
			}
		}

		return str;
	}

	return 0;
}

int _parseProxyUrl(char *url)
{
	char * tmp;

	for (tmp = _cleanStr(url), url = tmp; *tmp && *tmp != ':'; tmp++);

	if (tmp && *tmp) {
		_LocalProxy.address = (char *) malloc(tmp - url + 1);
		memcpy(_LocalProxy.address, url, tmp - url);
		_LocalProxy.address[tmp - url] = 0;

		for (url = ++tmp; *tmp && *tmp >= '0' && * tmp <= '9'; tmp++);
		*tmp = 0;
		_LocalProxy.port = atoi(url);
		return 0;
	}

	return -1;
}

int _getProxyAddress()
{
#if defined OS_WIN32 && defined CC_MSVC
	long ret;
	HKEY result;
	char url[1024];
	DWORD size = 1024;
	char TempPath[MAX_PATH];
	char TempFile[MAX_PATH];
	HMODULE hModJS;
	DWORD enable;
	DWORD enablesize = sizeof(DWORD);

	/* MSDN EXAMPLE */

	char url1[1025] = "http://www.google.fr/about.html";
	char host[256] = "http://www.google.fr";
	char proxyBuffer[1024];
	char * proxy = proxyBuffer;
	ZeroMemory(proxy, 1024);
	DWORD dwProxyHostNameLength = 1024;
	DWORD returnVal;
	// Declare function pointers for the three autoproxy functions
	pfnInternetInitializeAutoProxyDll pInternetInitializeAutoProxyDll;
	pfnInternetDeInitializeAutoProxyDll pInternetDeInitializeAutoProxyDll;
	pfnInternetGetProxyInfo pInternetGetProxyInfo;

	/* ************ */

	ret = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_QUERY_VALUE, & result);
	ret = RegQueryValueExA(result, "ProxyEnable", 0, 0, (LPBYTE) & enable, & enablesize);
	ret = RegQueryValueExA(result, "ProxyServer", 0, 0, (LPBYTE)url, & size);

	if (ret == ERROR_SUCCESS && enable == 1) {
		if (_parseProxyUrl(url) == 0) {
			RegCloseKey(result);
			return 0;
		}
	}

	ZeroMemory(url, 1024);
	size = 1024;
	ret = RegQueryValueExA(result, "AutoConfigURL", 0, 0, (LPBYTE)url, & size);
	RegCloseKey(result);

	if (ret != ERROR_SUCCESS) {
		if (DetectAutoProxyUrl(url, size, PROXY_AUTO_DETECT_TYPE_DHCP | PROXY_AUTO_DETECT_TYPE_DNS_A) == false) {
			NETLIB_LOG_ERROR(make_error_string("_getProxyAddress : DetectAutoProxyUrl"));
			return -1;
		}
	}

	GetTempPathA(sizeof(TempPath), TempPath);
	GetTempFileNameA(TempPath, NULL, 0, TempFile);
	if (URLDownloadToFileA(NULL, url, TempFile, NULL, NULL) != S_OK) {
		NETLIB_LOG_ERROR(make_error_string("_getProxyAddress : URLDownloadToFileA"));
		return -1;
	}

	if (!(hModJS = LoadLibraryA("jsproxy.dll"))) {
		NETLIB_LOG_ERROR(make_error_string("_getProxyAddress : LoadLibrary"));
		return -1;
	}

	if (!(pInternetInitializeAutoProxyDll = (pfnInternetInitializeAutoProxyDll)
		GetProcAddress(hModJS, "InternetInitializeAutoProxyDll")) ||
		!(pInternetDeInitializeAutoProxyDll = (pfnInternetDeInitializeAutoProxyDll)
		GetProcAddress(hModJS, "InternetDeInitializeAutoProxyDll")) ||
		!(pInternetGetProxyInfo = (pfnInternetGetProxyInfo)
		GetProcAddress(hModJS, "InternetGetProxyInfo"))) {
		NETLIB_LOG_ERROR(make_error_string("_getProxyAddress : GetProcAddress"));
		return -1;
	}

	if (!(returnVal = pInternetInitializeAutoProxyDll(0, TempFile, NULL, 0, NULL))) {
		NETLIB_LOG_ERROR(make_error_string("_getProxyAddress : pInternetInitializeAutoProxyDll"));
		return -1;
	}

	DeleteFileA(TempFile);

	if (!pInternetGetProxyInfo((LPSTR)url1, sizeof(url1),
								(LPSTR)host, sizeof(host),
								&proxy, &dwProxyHostNameLength)) {
		NETLIB_LOG_ERROR(make_error_string("_getProxyAddress : pInternetGetProxyInfo"));
		return -1;
	}

	if (strncmp("PROXY ", proxy, 6) == 0) {
		if (_parseProxyUrl(proxy + 6) != 0) {
			NETLIB_LOG_ERROR(make_error_string("_getProxyAddress : _parseProxyUrl"));
			return -1;
		}
	}
	else {
		NETLIB_LOG_ERROR("_getProxyAddress : strncmp : PROXY doesn't match\n");
		return -1;
	}

	if (!pInternetDeInitializeAutoProxyDll(NULL, 0)) {
		NETLIB_LOG_ERROR(make_error_string("_getProxyAddress : pInternetDeInitializeAutoProxyDll"));
		return -1;
	}
#endif

	return 0;
}

EnumAuthType get_proxy_auth_type(const char *proxy_addr, int proxy_port, int timeout)
{
	EnumAuthType authType = proxyAuthUnknown;

	is_proxy_auth_needed(proxy_addr, proxy_port, timeout);

	if ((_LocalProxy.proxy_auth_type & CURLAUTH_BASIC) == CURLAUTH_BASIC) {
		authType = proxyAuthBasic;
	}
	else if ((_LocalProxy.proxy_auth_type & CURLAUTH_DIGEST) == CURLAUTH_DIGEST) {
		authType = proxyAuthDigest;
	}
	else if ((_LocalProxy.proxy_auth_type & CURLAUTH_NTLM) == CURLAUTH_NTLM) {
		authType = proxyAuthNTLM;
	}

	return authType;
}

void _get_proxy_auth_type2(const char *url, const char *proxy_addr, int proxy_port, int timeout)
{
	CURL *curl_tmp;
	char url_buf[1024];
	char proxy_buf[1024];
	int ret;

	ret = 0;
	curl_tmp = curl_easy_init();

	curl_easy_setopt(curl_tmp, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl_tmp, CURLOPT_WRITEFUNCTION, _curlsilentwritecbk);
	curl_easy_setopt(curl_tmp, CURLOPT_DEBUGFUNCTION, _curloutputcbk);

	snprintf(url_buf, sizeof(url_buf), "http://%s", url);
	curl_easy_setopt(curl_tmp, CURLOPT_URL, url_buf);

	snprintf(proxy_buf, sizeof(proxy_buf), "%s:%d", proxy_addr, proxy_port);
	curl_easy_setopt(curl_tmp, CURLOPT_PROXY, proxy_buf);

	if (timeout > 0) {
		curl_easy_setopt(curl_tmp, CURLOPT_TIMEOUT, timeout);
	}

	curl_easy_setopt(curl_tmp, CURLOPT_HTTPPROXYTUNNEL, 1);
	ret = curl_easy_perform(curl_tmp);

	curl_easy_getinfo(curl_tmp, CURLINFO_PROXYAUTH_AVAIL, &(_LocalProxy.proxy_auth_type));

	curl_easy_cleanup(curl_tmp);
}

void _get_auth_type(const char *url, int timeout)
{
	CURL *curl_tmp;
	char url_buf[1024];
	int ret;

	ret = 0;
	curl_tmp = curl_easy_init();

	curl_easy_setopt(curl_tmp, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl_tmp, CURLOPT_WRITEFUNCTION, _curlsilentwritecbk);
	curl_easy_setopt(curl_tmp, CURLOPT_DEBUGFUNCTION, _curloutputcbk);

	snprintf(url_buf, sizeof(url_buf), "http://%s", url);
	curl_easy_setopt(curl_tmp, CURLOPT_URL, url_buf);

	if (timeout > 0) {
		curl_easy_setopt(curl_tmp, CURLOPT_TIMEOUT, timeout);
	}

	ret = curl_easy_perform(curl_tmp);

	curl_easy_getinfo(curl_tmp, CURLINFO_HTTPAUTH_AVAIL, &(_LocalProxy.auth_type));

	curl_easy_cleanup(curl_tmp);
}

char **internet_explorer_proxyless_exception_list()
{
#ifdef OS_WIN32
	char **list;
	long ret;
	HKEY result;
	char buff[1024];
	DWORD buff_size = sizeof(buff);

	ret = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_QUERY_VALUE, &result);
	ret = RegQueryValueExA(result, "ProxyOverride", 0, 0, (LPBYTE)buff, &buff_size);
	RegCloseKey(result);

	list = my_split(buff, ';');
	return list;
#else
	return 0;
#endif
}

NETLIB_BOOLEAN is_url_proxyless_exception(const char *url)
{
	int i;

	if (!url) {
		return NETLIB_FALSE;
	}

	if (!_LocalProxy.proxy_exceptions) {
		_LocalProxy.proxy_exceptions = internet_explorer_proxyless_exception_list();
	}

	if (!_LocalProxy.proxy_exceptions) {
		return NETLIB_FALSE;
	}

	for (i = 0; _LocalProxy.proxy_exceptions[i]; i++) {
		if (domain_url_cmp((char *)url, _LocalProxy.proxy_exceptions[i]) == false) {
			return NETLIB_TRUE;
		}
	}

	return NETLIB_FALSE;
}

NETLIB_BOOLEAN is_udp_port_opened(const char *stun_server, int port, NatType *nType)
{
	*nType = get_nat_type(stun_server);
	return (*nType > StunTypeUnknown && *nType < StunTypeBlocked ?
		NETLIB_TRUE : NETLIB_FALSE);
}

NETLIB_BOOLEAN is_local_udp_port_used(const char *itf, int port)
{
	struct sockaddr_in  raddr;
	Socket localsock;

	if (!itf) {
		raddr.sin_addr.s_addr = htons(INADDR_ANY);
	} else {
		raddr.sin_addr.s_addr = inet_addr(itf);
	}
	raddr.sin_port = htons((short)port);
	raddr.sin_family = AF_INET;

	if ((localsock = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		NETLIB_LOG_ERROR(make_error_string("is_local_udp_port_used : socket"));
		return NETLIB_TRUE;
	}

	if (bind(localsock, (struct sockaddr *)&raddr, sizeof (raddr)) < 0) {
		NETLIB_LOG_ERROR(make_error_string("is_local_udp_port_used : bind"));
		closesocket(localsock);
		return NETLIB_TRUE;
	}

	closesocket(localsock);
	return NETLIB_FALSE;
}

int get_local_free_udp_port(const char *itf)
{
	struct sockaddr_in raddr;
	struct sockaddr_in name;
	int name_size = sizeof (struct sockaddr_in);
	Socket localsock;

	if (!itf) {
		raddr.sin_addr.s_addr = htons(INADDR_ANY);
	} else {
		raddr.sin_addr.s_addr = inet_addr(itf);
	}
	raddr.sin_port = htons(0);
	raddr.sin_family = AF_INET;

	if ((localsock = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		NETLIB_LOG_ERROR(make_error_string("get_local_free_udp_port : socket"));
		return -1;
	}

	if (bind(localsock, (struct sockaddr *)&raddr, sizeof (raddr)) < 0) {
		NETLIB_LOG_ERROR(make_error_string("get_local_free_udp_port : bind"));
		closesocket(localsock);
		return -1;
	}

	if (getsockname(localsock, (struct sockaddr *) &name, (socklen_t *)&name_size) < 0) {
		NETLIB_LOG_ERROR(make_error_string("get_local_free_udp_port : getsockname"));
		closesocket(localsock);
		return -1;
	}

	closesocket(localsock);
	return ntohs(name.sin_port);
}

char	*create_opt_req(const char *sip_login, const char *sip_realm, Socket sock)
{
	char *opt_req;
	struct sockaddr_in name;
	int name_size = sizeof (struct sockaddr_in);
	int local_port = 0;
	char *local_ip = 0;

	if (getsockname(sock, (struct sockaddr *) &name, (socklen_t *)&name_size) < 0) {
		NETLIB_LOG_ERROR(make_error_string("create_opt_req : getsockname"));
		return NULL;
	}

	local_port = ntohs(name.sin_port);
	local_ip = inet_ntoa(name.sin_addr);

	/* <UOLFONE> */
	/*
	// 220 is the size of the static text
	opt_req = (char *)malloc(220 + strlen(sip_login) * 3 + strlen(sip_realm) * 4 + strlen(local_ip) * 2 + 1);

	sprintf(opt_req,
		"REGISTER sip:%s SIP/2.0\r\n\
Via: SIP/2.0/UDP %s:%d;rport;branch=123456789\r\n\
Route: <sip:%s;lr>\r\n\
From: nobody <sip:%s@%s>;tag=123456789\r\n\
To: <sip:%s@%s>\r\n\
Call-ID: 000001@ping\r\n\
Contact: <sip:%s@%s>\r\n\
CSeq: 2 REGISTER\r\n\
Content-Length: 0\r\n\r\n",
		sip_realm,
		local_ip,
		local_port,
		sip_realm,
		sip_login,
		sip_realm,
		sip_login,
		sip_realm,
		sip_login,
		local_ip
		);
	*/
	char* final_sip_realm = NULL;
	
	if ( (!sip_realm) || (*sip_realm == '\0') )	{
		final_sip_realm = strdup("192.168.1.1");
	}
	else {
		final_sip_realm = strdup(sip_realm);
	}
	
	// 219 is the size of the static text
	opt_req = (char *)malloc(219 + strlen(sip_login) * 3 + strlen(final_sip_realm) * 4 + strlen(local_ip) * 2 + 1);
	
	sprintf(opt_req,
		"OPTIONS sip:%s SIP/2.0\r\n\
Via: SIP/2.0/UDP %s:%d;rport;branch=123456789\r\n\
Route: <sip:%s;lr>\r\n\
From: nobody <sip:%s@%s>;tag=123456789\r\n\
To: <sip:%s@%s>\r\n\
Call-ID: 000001@ping\r\n\
Contact: <sip:%s@%s>\r\n\
CSeq: 20 OPTIONS\r\n\
Content-Length: 0\r\n\r\n",
		final_sip_realm,
		local_ip,
		local_port,
		final_sip_realm,
		sip_login,
		final_sip_realm,
		sip_login,
		final_sip_realm,
		sip_login,
		local_ip
		);
	
	free(final_sip_realm);
	/* </UOLFONE> */

	return opt_req;
}

NETLIB_BOOLEAN sip_ping(Socket sock, int ping_timeout, const char *sip_login, const char *sip_realm)
{
	fd_set rfds;
	int ret;
	char buff[1024];
	struct timeval	timeout;
	char *opt_req;

	opt_req = create_opt_req(sip_login, sip_realm, sock);

	send(sock, opt_req, strlen(opt_req), 0);
	free(opt_req);

	timeout.tv_sec = ping_timeout;
	timeout.tv_usec = 0;
	FD_ZERO(&rfds);
	FD_SET(sock, &rfds);

	ret = select(sock + 1, &rfds, 0, 0, &timeout);

	if (ret < 1 || !FD_ISSET(sock, &rfds)) {
		if (ret == 0) {
			NETLIB_LOG_ERROR("sip_ping : select : timeout reached\n");
		} else {
			NETLIB_LOG_ERROR(make_error_string("sip_ping : select"));
		}

		return NETLIB_FALSE;
	}

	ret = recv(sock, buff, sizeof (buff), 0);
	if (ret < 1) {
		if (ret == 0) {
			NETLIB_LOG_ERROR("sip_ping : recv : returns 0\n");
		} else {
			NETLIB_LOG_ERROR(make_error_string("sip_ping : recv"));
		}
		return NETLIB_FALSE;
	}
	else {
		return NETLIB_TRUE;
	}
}

NETLIB_BOOLEAN sip_ping2(http_sock_t *hs, int ping_timeout, const char *sip_login, const char *sip_realm)
{
	fd_set rfds;
	int ret;
	char buff[1024];
	struct timeval	timeout;
	char *opt_req;

	opt_req = create_opt_req(sip_login, sip_realm, hs->fd);
	http_tunnel_send(hs, opt_req, strlen(opt_req));
	free(opt_req);

	timeout.tv_sec = ping_timeout;
	timeout.tv_usec = 0;
	FD_ZERO(&rfds);
	FD_SET(hs->fd, &rfds);

	ret = select(hs->fd + 1, &rfds, 0, 0, &timeout);

	if (ret < 1 || !FD_ISSET(hs->fd, &rfds)) {
		if (ret == 0) {
			NETLIB_LOG_ERROR("sip_ping2 : select : timeout\n");
		} else {
			NETLIB_LOG_ERROR(make_error_string("sip_ping2 : select"));
		}
		return NETLIB_FALSE;
	}

	ret = http_tunnel_recv(hs, buff, sizeof (buff));
	if (ret < 1) {
		if (ret == 0) {
			NETLIB_LOG_ERROR("sip_ping2 : http_tunnel_recv : returns 0\n");
		} else {
			NETLIB_LOG_ERROR(make_error_string("sip_ping2 : http_tunnel_recv"));
		}
		return NETLIB_FALSE;
	}
	else {
		/* <UOLFONE> */
#ifdef _DEBUG
		if (ret < 1024)
		{
			buff[ret] = '\0';
			OutputDebugStr(buff);
		}
#endif
		/* </UOLFONE> */
		return NETLIB_TRUE;
	}
}

NETLIB_BOOLEAN udp_sip_ping(const char *sip_server, int sip_port, int local_port, int ping_timeout, const char *sip_address, const char *sip_realm)
{
	Socket sock;
	struct sockaddr_in addr;
	struct sockaddr_in laddr;
	NETLIB_BOOLEAN ret;
	int i;
	hostent* sip_host;
	char* sip_ip;

	laddr.sin_addr.s_addr = htons(INADDR_ANY);
	laddr.sin_port = htons(local_port);
	laddr.sin_family = AF_INET;

	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock <= 0) {
		NETLIB_LOG_ERROR(make_error_string("udp_sip_ping : socket"));
		return -1;
	}

	if (bind(sock, (struct sockaddr *)&laddr, sizeof (laddr)) < 0) {
		NETLIB_LOG_ERROR(make_error_string("udp_sip_ping : bind"));
		closesocket(sock);
		return -1;
	}

	// Get the host information
	sip_host = gethostbyname(sip_server);
	if (sip_host != NULL)
		sip_ip = inet_ntoa(*(struct in_addr *)*sip_host->h_addr_list);
	else
		sip_ip = (char *)sip_server;
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = inet_addr(sip_ip);
	// ensure conversion worked
	if (addr.sin_addr.s_addr == INADDR_NONE) { // neither lookup worked
		closesocket(sock);
		return NETLIB_FALSE;
	}

	addr.sin_port = htons(sip_port);

	i = connect(sock,(struct sockaddr *)&addr, sizeof(struct sockaddr));
	if (i < 0) {
		NETLIB_LOG_ERROR(make_error_string("udp_sip_ping : connect"));
		closesocket(sock);
		return NETLIB_FALSE;
	}

	ret = sip_ping(sock, ping_timeout, sip_address, sip_realm);
	closesocket(sock);

	return (ret ? NETLIB_TRUE : NETLIB_FALSE);
}

NETLIB_BOOLEAN is_https(const char *url)
{
	if (strncasecmp(url, "https", 5) == 0) {
		return NETLIB_TRUE;
	} else {
		return NETLIB_FALSE;
	}
}

HttpRet is_http_conn_allowed(const char *url,
							  const char *proxy_addr, int proxy_port,
							  const char *proxy_login, const char *proxy_passwd,
							  NETLIB_BOOLEAN ssl, int timeout)
{
	char url_buf[1024];
	char auth_buf[1024];
	char proxy_buf[1024];
	CURL *mcurl;
	int ret;
	long http_resp_code;

	mcurl = curl_easy_init();

	curl_easy_setopt(mcurl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(mcurl, CURLOPT_WRITEFUNCTION, _curlsilentwritecbk);
	curl_easy_setopt(mcurl, CURLOPT_DEBUGFUNCTION, _curloutputcbk);

	if (ssl) {
		snprintf(url_buf, sizeof(url_buf), "https://%s", url);
		curl_easy_setopt(mcurl, CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv3);
		curl_easy_setopt(mcurl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(mcurl, CURLOPT_SSL_VERIFYHOST, 0);
	}
	else {
		snprintf(url_buf, sizeof(url_buf), "http://%s", url);
	}

	curl_easy_setopt(mcurl, CURLOPT_URL, url_buf);

	if (timeout > 0) {
		curl_easy_setopt(mcurl, CURLOPT_TIMEOUT, timeout);
	}

	/* FOLLOW REDIRECTION */
	curl_easy_setopt(mcurl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(mcurl, CURLOPT_UNRESTRICTED_AUTH, 1);
	/* ****************** */

	if (proxy_addr && *proxy_addr != 0) {
		if (proxy_login && *proxy_login != 0) {
			if (!_LocalProxy.proxy_auth_type)
				_get_proxy_auth_type2(url, proxy_addr, proxy_port, timeout);

			snprintf(auth_buf, sizeof(auth_buf), "%s:%s", proxy_login, proxy_passwd);
			curl_easy_setopt(mcurl, CURLOPT_PROXYUSERPWD, auth_buf);

			if ((_LocalProxy.proxy_auth_type & CURLAUTH_BASIC) == CURLAUTH_BASIC) {
				curl_easy_setopt(mcurl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC);
			}
			else if ((_LocalProxy.proxy_auth_type & CURLAUTH_DIGEST) == CURLAUTH_DIGEST) {
				curl_easy_setopt(mcurl, CURLOPT_PROXYAUTH, CURLAUTH_DIGEST);
			}
			else if ((_LocalProxy.proxy_auth_type & CURLAUTH_NTLM) == CURLAUTH_NTLM) {
				curl_easy_setopt(mcurl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM);
			}
		}
		snprintf(proxy_buf, sizeof(proxy_buf), "%s:%d", proxy_addr, proxy_port);
		curl_easy_setopt(mcurl, CURLOPT_PROXY, proxy_buf);
	}
	else {
		if (proxy_login && *proxy_login != 0) {
			if (!_LocalProxy.auth_type) {
				_get_auth_type(url, timeout);
			}

			snprintf(auth_buf, sizeof(auth_buf), "%s:%s", proxy_login, proxy_passwd);
			curl_easy_setopt(mcurl, CURLOPT_USERPWD, auth_buf);

			if ((_LocalProxy.proxy_auth_type & CURLAUTH_BASIC) == CURLAUTH_BASIC) {
				curl_easy_setopt(mcurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
			}
			else if ((_LocalProxy.proxy_auth_type & CURLAUTH_DIGEST) == CURLAUTH_DIGEST) {
				curl_easy_setopt(mcurl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
			}
			else if ((_LocalProxy.proxy_auth_type & CURLAUTH_NTLM) == CURLAUTH_NTLM) {
				curl_easy_setopt(mcurl, CURLOPT_HTTPAUTH, CURLAUTH_NTLM);
			}
		}

	}

	ret = curl_easy_perform(mcurl);
	curl_easy_getinfo(mcurl, CURLINFO_RESPONSE_CODE, &http_resp_code);
	if (!_LocalProxy.proxy_auth_type) {
		curl_easy_getinfo(mcurl, CURLINFO_PROXYAUTH_AVAIL, &(_LocalProxy.proxy_auth_type));
	}

	HttpRet http_ret;
	/* If this is a redirection */
	if ((http_resp_code / 100) == 3) {
		NETLIB_BOOLEAN is_ssl;
		char *redir_tmp = 0;
		char *redir_url = 0;
		char *tmp;

		curl_easy_getinfo(mcurl, CURLINFO_EFFECTIVE_URL, &redir_tmp);
		if (redir_tmp) {
			redir_url = strdup(redir_tmp);
			curl_easy_cleanup(mcurl);

			if ((is_ssl = is_https(redir_url)) == NETLIB_TRUE) {
				tmp = redir_url + 8;
			}
			else {
				tmp = redir_url + 7;
			}

			if (is_url_proxyless_exception(tmp)) {
				http_ret = is_http_conn_allowed(tmp, NULL, 0, proxy_login, proxy_passwd, is_ssl, timeout);
			}
			else {
				http_ret = is_http_conn_allowed(tmp, proxy_addr, proxy_port, proxy_login, proxy_passwd, is_ssl, timeout);
			}
			free(redir_url);
		}
		else {
			curl_easy_cleanup(mcurl);
			http_ret = HTTP_NOK;
		}
		return http_ret;
	}

	if (http_resp_code == 200) {
		http_ret = HTTP_OK;
	}
	else if (http_resp_code != 404 && http_resp_code != 200 && http_resp_code != 0) {
		http_ret = HTTP_AUTH;
	}
	else {
		http_ret = HTTP_NOK;
	}

	curl_easy_cleanup(mcurl);
	return http_ret;
}

/* <UOLFONE> */
/*
HttpRet is_tunnel_conn_allowed(const char *http_gate_addr, int http_gate_port,
								const char *sip_addr, int sip_port,
								const char *proxy_addr, int proxy_port,
								const char *proxy_login, const char *proxy_passwd,
								NETLIB_BOOLEAN ssl, int timeout, const char *sip_realm,
								NETLIB_BOOLEAN ping, int ping_timeout)
*/
HttpRet is_tunnel_conn_allowed(const char *http_gate_addr, int http_gate_port,
								const char *sip_addr, int sip_port,
								const char *proxy_addr, int proxy_port,
								const char *proxy_login, const char *proxy_passwd,
								NETLIB_BOOLEAN ssl, int timeout, const char *sip_realm,
								NETLIB_BOOLEAN ping, int ping_timeout,
								int tunnel_mode)
/* </UOLFONE> */
{

	http_sock_t *hs;
	int http_code;

	/* <UOLFONE> */
	/*
	if (proxy_addr) {
		http_tunnel_init_proxy(proxy_addr, proxy_port, proxy_login, proxy_passwd);
	}

	http_tunnel_init_host(http_gate_addr, http_gate_port, ssl);
	hs = (http_sock_t *)http_tunnel_open(sip_addr, sip_port, HTTP_TUNNEL_VAR_MODE, &http_code, timeout);

	if (hs == NULL) {
		if (http_code == 404) {
			return HTTP_NOK;
		} else {
			return HTTP_AUTH;
		}
	}

	if (ping) {
		if (sip_ping2(hs, ping_timeout, "nobody", sip_realm) == NETLIB_FALSE) {
			return HTTP_NOK;
		}
	}

	return HTTP_OK;
	*/

	HttpRet ret = HTTP_NOK;
	http_tunnel_info_t httpTunnelInfo;
	
	memset(&httpTunnelInfo, 0, sizeof(http_tunnel_info_t));

	if (proxy_addr)
	{
		strncpy(httpTunnelInfo.http_proxy_host, proxy_addr, sizeof(httpTunnelInfo.http_proxy_host));
		httpTunnelInfo.http_proxy_port = proxy_port;
		strncpy(httpTunnelInfo.http_proxy_username, proxy_login, sizeof(httpTunnelInfo.http_proxy_username));
		strncpy(httpTunnelInfo.http_proxy_password, proxy_passwd, sizeof(httpTunnelInfo.http_proxy_password));
	}

	strncpy(httpTunnelInfo.tunnel_host, http_gate_addr, sizeof(httpTunnelInfo.tunnel_host));
	httpTunnelInfo.tunnel_port = http_gate_port;

	strncpy(httpTunnelInfo.dest_host, sip_addr, sizeof(httpTunnelInfo.dest_host));
	httpTunnelInfo.dest_port = sip_port;

	hs = (http_sock_t *) http_tunnel_open(&httpTunnelInfo, tunnel_mode, &http_code, timeout, ssl, -1, -1);

	if (hs == NULL)
	{
		if (http_code == 404)
			return HTTP_NOK;
		else
			return HTTP_AUTH;
	}

	ret = HTTP_OK;

	if (ping)
	{
		if (sip_ping2(hs, ping_timeout, "nobody", sip_realm) == NETLIB_FALSE)
		{
			ret = HTTP_NOK;
		}
	}

	http_tunnel_close(hs);

	return ret;
	/* </UOLFONE> */
}


NETLIB_BOOLEAN is_proxy_auth_needed(const char *proxy_addr, int proxy_port, int timeout)
{
	HttpRet ret;

	if (_LocalProxy.auth_type) {
		return NETLIB_TRUE;
	}

	ret = is_http_conn_allowed("www.google.com:80", proxy_addr, proxy_port,
								NULL, NULL, NETLIB_FALSE, timeout);

	return (ret == HTTP_AUTH ? NETLIB_TRUE : NETLIB_FALSE);
}

NETLIB_BOOLEAN is_proxy_auth_ok(const char *proxy_addr, int proxy_port,
								const char *proxy_login, const char *proxy_passwd,
								int timeout)
{
	HttpRet ret;

	ret = is_http_conn_allowed("www.google.com:80", proxy_addr, proxy_port,
								proxy_login, proxy_passwd,
								NETLIB_FALSE, timeout);

	return (ret == HTTP_OK ? NETLIB_TRUE : NETLIB_FALSE);
}



char *get_local_http_proxy_address()
{
	if (_LocalProxy.address == NULL) {
		_getProxyAddress();
	}

	return _LocalProxy.address;
}

int get_local_http_proxy_port()
{
	return _LocalProxy.port;
}

NatType get_nat_type(const char *stun_server)
{
	static const int CTRL_PORT = 3478;

	initNetwork();

	StunAddress4 stunServerAddr;
	stunServerAddr.addr = 0;

	StunAddress4 sAddr[1];
	sAddr[0].addr = 0;
	sAddr[0].port = 0;

	bool ret = stunParseServerName((char *) stun_server, stunServerAddr, CTRL_PORT);
	if (!ret) {
		return StunTypeFailure;
	}

	bool presPort = false;
	bool hairpin = false;
	return stunNatType(stunServerAddr, true, & presPort, & hairpin, 0, & sAddr[0]);
}

NETLIB_BOOLEAN is_connection_available()
{
#if defined(OS_WIN32)
	DWORD flags;

	return InternetGetConnectedState(&flags, 0);

#elif defined(OS_MACOSX)

	// TODO: to be implemented
	return NETLIB_TRUE;

#else
	NETLIB_BOOLEAN res = NETLIB_FALSE;
	int sock;
	char buf[BUFSIZ];
	struct ifconf ifc;

	memset(&ifc, 0, sizeof ifc);

	ifc.ifc_len = sizeof buf;
	ifc.ifc_buf = (char *)buf;

	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		NETLIB_LOG_ERROR(make_error_string("is_connection_available : socket"));
		return NETLIB_FALSE;
	}

	if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
		closesocket(sock);
		NETLIB_LOG_ERROR(make_error_string("is_connection_available : ioctl"));
		return NETLIB_FALSE;
	}

	for (unsigned int i = 0; i < (ifc.ifc_len / sizeof(struct ifreq)); i++) {
		if (ioctl(sock, SIOCGIFFLAGS, &ifc.ifc_req[i]) == 0) {
			if ((ifc.ifc_req[i]).ifr_ifru.ifru_flags & IFF_LOOPBACK) {
				continue;
			}

			if ((ifc.ifc_req[i]).ifr_ifru.ifru_flags & IFF_RUNNING) {
				res = NETLIB_TRUE;
			}
		}
	}

	closesocket(sock);
	return res;

#endif
}
