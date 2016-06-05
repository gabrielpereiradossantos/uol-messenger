/*
  Copyright (C) 2005,2006,2007  Wengo SA
  
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

#if defined(WIN32)
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <windows.h>
	#include <winbase.h>
	#define FCNTL_BLOCK(fd)		{u_long arg1 = 0;\
								ioctlsocket(fd, FIONBIO, &arg1);}
	#define FCNTL_NOBLOCK(fd)	{u_long arg2 = 1;\
								ioctlsocket(fd, FIONBIO, &arg2);}

	#define ERR_SOCK(err)	(err == WSAENETDOWN ||\
							 err == WSAEHOSTUNREACH ||\
							 err == WSAENETRESET ||\
							 err == WSAENOTCONN ||\
							 err == WSAESHUTDOWN ||\
							 err == WSAECONNABORTED ||\
							 err == WSAECONNRESET ||\
							 err == WSAETIMEDOUT)

	#ifndef snprintf
	#define snprintf _snprintf
	#endif

#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <errno.h>
	#include <pthread.h>
	#define FCNTL_BLOCK(fd)		{int flags1 = fcntl(fd, F_GETFL, 0);\
								fcntl(fd, F_SETFL, flags1 & ~O_NONBLOCK);}

	#define FCNTL_NOBLOCK(fd)	{int flags2 = fcntl(fd, F_GETFL, 0);\
								fcntl(fd, F_SETFL, flags2 | O_NONBLOCK);}


	#define WSAEWOULDBLOCK 	EAGAIN
	#define ERR_SOCK(err)	(ECONNREFUSED || ENOTCONN)

	inline int closesocket(int sock) {return close(sock);}
	inline int WSAGetLastError() {return errno;}
	inline int GetLastError() {return errno;}
	#define SOCKET int

#endif

#ifndef MSG_NOSIGNAL
	#define MSG_NOSIGNAL 0
#endif


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <curl/curl.h>
#include "httptunnel.h"

/* <UOLFONE> */
/* Enable HTTP tunnel sending/receiving debugging info */
/*#define _DEBUG_HTTP_TUNNEL 1*/
/* </UOLFONE> */


#ifdef __cplusplus
extern "C" {
#endif

char	*httpServerIP;
int		httpServerPort;
char	*proxyServerIP;
int		proxyServerPort;
long	proxyAuthType;

static char	*proxyUsername;
static char	*proxyPassword;

int	UseProxy = 0;

int UseSSL = 0;
int sslIsInit = 0;

#ifdef HT_USE_SSL

SSL_METHOD	*sslMethod;
SSL_CTX		*ctx;

#ifdef WIN32
static HANDLE *lock_cs;
#else
static pthread_mutex_t *lock_cs;
#endif

#endif


static HttpTunnelLogCbk writeLogCbk = 0;

void http_tunnel_set_write_log_callback(HttpTunnelLogCbk cbk)
{
	writeLogCbk = cbk;
}

#define HTTPTUN_LOG_ERROR(buff) {if (writeLogCbk) writeLogCbk(HTTPTUN_LOG_ERR, buff); }

int _curloutputcbk(CURL *mycurl, curl_infotype type, char *buffer, size_t size, void * arg)
{
	if (writeLogCbk) {
		HTTPTUNNEL_LOG_TYPE httpt_type = HTTPTUN_LOG_UNK;

		switch (type) {
			case CURLINFO_HEADER_IN:
				httpt_type = HTTPTUN_LOG_IN;
				break;

			case CURLINFO_HEADER_OUT:
				httpt_type = HTTPTUN_LOG_OUT;
				break;

			case CURLINFO_TEXT:
				httpt_type = HTTPTUN_LOG_INFO;
				break;

			default:
				return 0;
		}

		writeLogCbk(httpt_type, buffer);
	}

	return 0;
}

char *make_error_string(const char *str)
{
	static char buffer[256];
	char *ptr = 0;

	memset(buffer, 0, sizeof(buffer));
#ifdef WIN32
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &ptr,
		0,
		NULL);
	_snprintf(buffer, sizeof(buffer), "%s: (%d) %s", str, GetLastError(), ptr);
	LocalFree(ptr);
#else
	snprintf(buffer, sizeof(buffer), "%s: (%d) %s\n", str, errno, strerror(errno));
#endif

	return buffer;
}

int get_ip_addr(char * outbuf, int size, const char * hostname)
{
	struct addrinfo aiHints;
	struct addrinfo *res;
	int ret;

	if (size < 16) {
		return -1;
	}
	// *** Get IP of host ***
	memset(&aiHints, 0, sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	/* //// resolution DNS server SIP //// */

	if ((ret = getaddrinfo(hostname,NULL,&aiHints,&res)) != 0)
	{
		char errbuff[256];

		memset(errbuff, 0, sizeof(errbuff));
		snprintf(errbuff, sizeof(errbuff),
			"get_ip_addr : getaddrinfo : (%d) %s\n",
			ret, gai_strerror(ret));
		HTTPTUN_LOG_ERROR(errbuff);
		return -1;
	}
	if (!res) {
		HTTPTUN_LOG_ERROR("get_ip_addr : getaddrinfo : empty struct addrinfo\n");
		return -1;
	}

	strncpy(outbuf, inet_ntoa( ((struct sockaddr_in *)res->ai_addr)->sin_addr), size);
	freeaddrinfo(res);
	return 0;
}

/* <UOLFONE> */
#ifdef HT_USE_SSL
/* </UOLFONE> */
void locking_callback(int mode, int type, const char *file, int line)
{
#ifdef WIN32
	if (mode & CRYPTO_LOCK)
	{
		WaitForSingleObject(lock_cs[type],INFINITE);
	}
	else
	{
		ReleaseMutex(lock_cs[type]);
	}

#else
	if (mode & CRYPTO_LOCK)
	{
		pthread_mutex_lock(&(lock_cs[type]));
	}
	else
	{
		pthread_mutex_unlock(&(lock_cs[type]));
	}

#endif
}
/* <UOLFONE> */
#endif
/* </UOLFONE> */

#ifndef WIN32
unsigned long pthreads_thread_id(void)
{
	unsigned long ret;

	ret = (unsigned long)pthread_self();
	return ret;
}
#endif

void http_tunnel_init_ssl()
{
#if HT_USE_SSL
	if (UseSSL && !sslIsInit)
	{
		int i;

		SSL_load_error_strings();
		SSLeay_add_ssl_algorithms();
		
		sslMethod = SSLv23_client_method();
		ctx = SSL_CTX_new(sslMethod);
		SSL_CTX_set_options(ctx, SSL_OP_ALL);
		SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);

#ifdef WIN32
		lock_cs = OPENSSL_malloc(CRYPTO_num_locks() * sizeof(HANDLE));
		for (i = 0; i < CRYPTO_num_locks(); i++)
		{
			lock_cs[i] = CreateMutex(NULL, FALSE, NULL);
		}
#else
		lock_cs=OPENSSL_malloc(CRYPTO_num_locks() * sizeof(pthread_mutex_t));
		for (i = 0; i < CRYPTO_num_locks(); i++)
		{
			pthread_mutex_init(&(lock_cs[i]), NULL);
		}
		CRYPTO_set_id_callback((unsigned long (*)())pthreads_thread_id);
#endif

		CRYPTO_set_locking_callback(locking_callback);
		sslIsInit = 1;
	}
#endif
}

void http_tunnel_uninit_ssl()
{
#if HT_USE_SSL
	if (sslIsInit)
	{
		int i;
		CRYPTO_set_locking_callback(NULL);
		for (i = 0; i < CRYPTO_num_locks(); i++)
		{
#ifdef WIN32
			CloseHandle(lock_cs[i]);
#else
			pthread_mutex_destroy(&(lock_cs[i]));
#endif
		}

		OPENSSL_free(lock_cs);
	}
#endif
}

/* <UOLFONE> */
/*
void http_tunnel_init_host(const char *hostname, int port, int ssl)
{
	char hostIP[20];

	httpServerIP = 0;
	httpServerPort = 0;

	UseSSL = ssl;
	http_tunnel_init_ssl();

	memset(hostIP, 0, sizeof(hostIP));

	// TODO: Put a default IP on the SSO in the case of the hostname can't be resolved
	if (get_ip_addr(hostIP, sizeof(hostIP), hostname) == -1 || !hostIP || hostIP[0] == '\0')
		httpServerIP = strdup("80.118.99.31");
	else
		httpServerIP = strdup(hostIP);
	httpServerPort = port;
}

void http_tunnel_init_proxy(const char *hostname, int port, const char *username, const char *password)
{
	char hostIP[20];

	proxyServerIP = 0;
	proxyServerPort = 0;
	proxyUsername = 0;
	proxyPassword = 0;
	proxyAuthType = 0;
	UseProxy = 0;

	memset(hostIP, 0, sizeof(hostIP));

	if (hostname && *hostname != 0) 
	{
		if (get_ip_addr(hostIP, sizeof(hostIP), hostname) < 0)
		{
			return;
		}
		proxyServerIP = strdup(hostIP);
		proxyServerPort = port;
		UseProxy = 1;
	}

	if (username && *username != 0)
		proxyUsername = strdup(username);

	if (password && *password != 0)
		proxyPassword = strdup(password);
}
*/
/* </UOLFONE> */

void http_tunnel_clean_up()
{
	http_tunnel_uninit_ssl();

	if (httpServerIP)
		free(httpServerIP);
	if (proxyServerIP)
		free(proxyServerIP);
	if (proxyUsername)
		free(proxyUsername);
	if (proxyPassword)
		free(proxyPassword);
}

int http_tunnel_get_socket(void *h_tunnel)
{
	struct http_sock *hs;

	if (!h_tunnel)
	{
		HTTPTUN_LOG_ERROR("http_tunnel_get_socket : struct http_sock not initialized\n");
		return -1;
	}
	hs = (struct http_sock *) h_tunnel;
	return hs->fd;
}

static SOCKET easy_get_sock(CURL *curl)
{
	SOCKET fd = (SOCKET) -1;

	curl_easy_getinfo(curl, CURLINFO_LASTSOCKET, &fd);

	if (fd > 0)
		FCNTL_BLOCK(fd);

	return fd;
}

int get_https_response(http_sock_t *hs, char *buff, int buffsize)
{
#ifdef HT_USE_SSL
	struct timeval	timeout;
	fd_set	rfds;
	int ret;
	int nbytes = 0;

	while (1)
	{
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_SET(hs->fd, &rfds);

		ret = select(hs->fd + 1, &rfds, 0, 0, &timeout);

		if (ret <= 0)
		{
			if (ret == 0) {
				HTTPTUN_LOG_ERROR("get_https_response : select : timeout reached\n");
			} else {
				HTTPTUN_LOG_ERROR(make_error_string("get_https_response : select"));
			}

			return -1;
		}

		if (FD_ISSET(hs->fd, &rfds))
		{
			do
			{
				ret = SSL_read(hs->s_ssl, buff + nbytes, 1);

				if (ret < 0)
				{
					HTTPTUN_LOG_ERROR(make_error_string("get_https_response : SSL_read"));
					return -1;
				}
				else if (ret == 0)
					return nbytes;
				else
					nbytes += ret;

				if (nbytes == buffsize)
					return nbytes;

				if (nbytes > 3 && strncmp("\r\n\r\n", buff + nbytes - 4, 4) == 0)
					return nbytes;
			}
			while (SSL_pending(hs->s_ssl));
		}
	}

	return nbytes;
#else
	return 0;
#endif
}

/* <UOLFONE> */
/*
int	get_http_response(http_sock_t *hs, char *buff, int buffsize)
*/
int	get_http_response(http_sock_t *hs, char *buff, int buffsize, int waittimeout)
/* </UOLFONE> */
{
	struct timeval	timeout;
	fd_set	rfds;
	int ret;
	int nbytes = 0;

	while (1)
	{
		/* <UOLFONE> */
		/*
		timeout.tv_sec = 2;
		*/
		timeout.tv_sec = (waittimeout > 0) ? waittimeout : 2;
		/* </UOLFONE> */
		timeout.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_SET(hs->fd, &rfds);

		ret = select(hs->fd + 1, &rfds, 0, 0, &timeout);
		
		if (ret <= 0)
		{
			if (ret == 0) {
				HTTPTUN_LOG_ERROR("get_http_response : select : timeout reached\n");
			} else {
				HTTPTUN_LOG_ERROR(make_error_string("get_http_response : select"));
			}

			return -1;
		}

		if (FD_ISSET(hs->fd, &rfds))
		{
			ret = recv(hs->fd, buff + nbytes, 1, 0);
			
			if (ret < 0)
				return -1;
			else if (ret == 0)
				return nbytes;
			else
				nbytes += ret;

			if (nbytes == buffsize)
				return nbytes;
			
			if (nbytes > 3 && strncmp("\r\n\r\n", buff + nbytes - 4, 4) == 0)
				break;
		}
	}

	return nbytes;
}

/* <UOLFONE> */
/*
void get_proxy_auth_type_h()
*/
long get_proxy_auth_type_h(const char* http_server_ip, int http_server_port, const char* proxy_server_ip, int proxy_server_port)
/* </UOLFONE> */
{
	CURL *curl_tmp;
	char url_buff[1024];
	char proxy_buff[1024];
	int ret;
	/* <UOLFONE> */
	long proxy_auth_type = 0;
	/* </UOLFONE> */

	ret = 0;
	curl_tmp = curl_easy_init();

	/* <UOLFONE> */
	/*
	curl_easy_setopt(curl_tmp, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl_tmp, CURLOPT_DEBUGFUNCTION, _curloutputcbk);

	snprintf(url_buff, sizeof(url_buff), "http://%s:%d", httpServerIP, httpServerPort);
	curl_easy_setopt(curl_tmp, CURLOPT_URL, url_buff);

	snprintf(proxy_buff, sizeof(proxy_buff), "%s:%d", proxyServerIP, proxyServerPort);
	curl_easy_setopt(curl_tmp, CURLOPT_PROXY, proxy_buff);

	curl_easy_setopt(curl_tmp, CURLOPT_HTTPPROXYTUNNEL, 1);
	ret = curl_easy_perform(curl_tmp);

	curl_easy_getinfo(curl_tmp, CURLINFO_PROXYAUTH_AVAIL, &proxyAuthType);
	*/
	curl_easy_setopt(curl_tmp, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl_tmp, CURLOPT_DEBUGFUNCTION, _curloutputcbk);

	snprintf(url_buff, sizeof(url_buff), "http://%s:%d", http_server_ip, http_server_port);
	curl_easy_setopt(curl_tmp, CURLOPT_URL, url_buff);

	snprintf(proxy_buff, sizeof(proxy_buff), "%s:%d", proxy_server_ip, proxy_server_port);
	curl_easy_setopt(curl_tmp, CURLOPT_PROXY, proxy_buff);

	curl_easy_setopt(curl_tmp, CURLOPT_HTTPPROXYTUNNEL, 1);
	ret = curl_easy_perform(curl_tmp);

	curl_easy_getinfo(curl_tmp, CURLINFO_PROXYAUTH_AVAIL, &proxy_auth_type);
	/* </UOLFONE> */
	
	// free the Curl tmp handle
	curl_easy_cleanup(curl_tmp);

	/* <UOLFONE> */
	return proxy_auth_type;
	/* </UOLFONE> */
}

/* <UOLFONE> */
int parse_output_address(const char* buffer, char* output_host, int* output_port)
{
	const char* tmp_buffer = buffer;
	char host[HTTP_TUNNEL_HOST_MAX_LEN + 1];
	char ch;
	int host_len = 0;
	int port = 0;

	if ((buffer == NULL) || 
		(output_host == NULL) ||
		(output_port == NULL))
	{
		return -1;
	}

	*output_host = '\0';
	*output_port = 0;
	memset(host, 0, sizeof(char) * (HTTP_TUNNEL_HOST_MAX_LEN + 1));

	while (tmp_buffer)
	{
		ch = *tmp_buffer;

		if (ch == ':')
		{
			/* read output port */
			tmp_buffer++;

			port = atoi(tmp_buffer);
			break;
		}
		else if (isdigit(ch) || ch == '.')
		{
			host[host_len] = ch;
			host_len++;
		}
		else
		{
			break;
		}

		tmp_buffer++;
	}

	host[host_len] = '\0';

	if ((host[0] == '\0') || (port == 0))
	{
		return -2;
	}

	strncpy(output_host, host, host_len);
	*output_port = port;

	return 0;
}
/* </UOLFONE> */

/* <UOLFONE> */
/*
http_sock_t *http_make_connection(const char *host, int port, int mode, int timeout)
{
	http_sock_t *hs;

	hs = (http_sock_t *) malloc(sizeof(http_sock_t));
	if (!hs)
	{
		HTTPTUN_LOG_ERROR(make_error_string("http_make_connection : malloc"));
		return NULL;
	}
	
	memset(hs, 0, sizeof(http_sock_t));
	hs->mode = mode;
	hs->send_size = 0;
	hs->recv_size = 0;

	if (UseProxy)
	{
		int ret = 0;
		char url_buff[1024];
		char proxy_buff[1024];
		char login_buff[1024];

		if (proxyAuthType == 0)
			get_proxy_auth_type_h();

		hs->curl = curl_easy_init();

		if(hs->curl)
		{
			curl_easy_setopt(hs->curl, CURLOPT_VERBOSE, 1);
			curl_easy_setopt(hs->curl, CURLOPT_DEBUGFUNCTION, _curloutputcbk);

			curl_easy_setopt(hs->curl, CURLOPT_CONNECT_ONLY, 1);

			snprintf(url_buff, sizeof(url_buff), "http://%s:%d", httpServerIP, httpServerPort);
			curl_easy_setopt(hs->curl, CURLOPT_URL, url_buff);

			snprintf(proxy_buff, sizeof(proxy_buff), "%s:%d", proxyServerIP, proxyServerPort);
			curl_easy_setopt(hs->curl, CURLOPT_PROXY, proxy_buff);

			if (timeout > 0)
				curl_easy_setopt(hs->curl, CURLOPT_CONNECTTIMEOUT, timeout);

			if (proxyAuthType)
			{
				snprintf(login_buff, sizeof(login_buff), "%s:%s", proxyUsername, proxyPassword);
				curl_easy_setopt(hs->curl, CURLOPT_PROXYUSERPWD, login_buff);
				
				if ((proxyAuthType & CURLAUTH_BASIC) == CURLAUTH_BASIC)
					curl_easy_setopt(hs->curl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC);
				else if ((proxyAuthType & CURLAUTH_DIGEST) == CURLAUTH_DIGEST)
					curl_easy_setopt(hs->curl, CURLOPT_PROXYAUTH, CURLAUTH_DIGEST);
				else if ((proxyAuthType & CURLAUTH_NTLM) == CURLAUTH_NTLM)
					curl_easy_setopt(hs->curl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM);
			}

			curl_easy_setopt(hs->curl, CURLOPT_HTTPPROXYTUNNEL, 1);

			ret = curl_easy_perform(hs->curl);
			
			//curl_easy_getinfo(hs->curl, CURLINFO_RESPONSE_CODE, http_code);

			if (ret != 0)
			{
				char errbuff[256];

				memset(errbuff, 0, sizeof(errbuff));
				snprintf(errbuff, sizeof(errbuff),
					"http_make_connection : curl_easy_perform returns error code = %d\n", ret);
				HTTPTUN_LOG_ERROR(errbuff);
				http_tunnel_close(hs);
				return NULL;
			}

			hs->fd = easy_get_sock(hs->curl);
		}
		else
		{
			HTTPTUN_LOG_ERROR("http_make_connection : curl_easy_init() : failed\n");
			free(hs);
			return NULL;
		}
	}
	else
	{
		int i = 1;
		struct sockaddr_in addr;
		SOCKET sock = 0;

		if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		{
			HTTPTUN_LOG_ERROR(make_error_string("http_make_connection : socket"));
			free(hs);
			return NULL;
		}

#ifdef SO_NOSIGPIPE
		setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &i, sizeof(i));
#endif

		hs->fd = sock;
		addr.sin_port = (unsigned short) htons(httpServerPort);
		addr.sin_addr.s_addr = inet_addr(httpServerIP);
		addr.sin_family = PF_INET;

		if (connect(hs->fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
		{
			HTTPTUN_LOG_ERROR(make_error_string("http_make_connection : connect"));
			http_tunnel_close(hs);
			return NULL;
		}
	}

	return hs;

}
*/
http_sock_t *http_make_connection(http_tunnel_info_t* tunnel_info, char *host, int mode, int *http_code, int timeout, int useSSL, int keep_alive_timeout, int connection_accept_timeout, char* tunnel_host_ip_ret)
{
	http_sock_t *hs;
	/* <UOLFONE> */
	char ipaddr[20];
	int use_proxy;
	char tunnel_host_ip[20];
	char proxy_host_ip[20];
	long proxy_auth_type = 0;
	char keep_alive_timeout_buff[1024];
	char connection_accept_timeout_buff[1024];
	/* </UOLFONE> */

	if (!tunnel_info)
	{
		return NULL;
	}

	// 404 (NOT FOUND) default value
	*http_code = 404;

	tunnel_info->tunnel_host[HTTP_TUNNEL_HOST_MAX_LEN] = '\0';
	tunnel_info->dest_host[HTTP_TUNNEL_HOST_MAX_LEN] = '\0';
	tunnel_info->http_proxy_host[HTTP_TUNNEL_HOST_MAX_LEN] = '\0';
	tunnel_info->http_proxy_username[HTTP_TUNNEL_PROXY_USENAME_MAX_LENGTH] = '\0';
	tunnel_info->http_proxy_password[HTTP_TUNNEL_PROXY_PASSWORD_MAX_LENGTH] = '\0';

	if ((tunnel_info->tunnel_host[0] == '\0') ||
		(tunnel_info->tunnel_port <= 0))
	{
		return NULL;
	}

	if ((HTTP_TUNNEL_CONNECT_MODE == mode) &&
		((tunnel_info->dest_host[0] == '\0') || (tunnel_info->dest_port <= 0)))
	{
		return NULL;
	}

	use_proxy = (tunnel_info->http_proxy_host[0] != '\0');

	if ((use_proxy) &&
		(tunnel_info->http_proxy_port <= 0))
	{
		return NULL;
	}

	memset(tunnel_host_ip, 0, sizeof(tunnel_host_ip));
	if (get_ip_addr(tunnel_host_ip, sizeof(tunnel_host_ip), tunnel_info->tunnel_host) < 0)
	{
		return NULL;
	}
	strncpy(tunnel_host_ip_ret, tunnel_host_ip, sizeof(tunnel_host_ip));

	hs = (http_sock_t *) malloc(sizeof(http_sock_t));
	if (!hs) 
	{
		HTTPTUN_LOG_ERROR(make_error_string("http_make_connection : malloc"));
		return NULL;
	}

	memset(hs, 0, sizeof(http_sock_t));
	memset(ipaddr, 0, sizeof(ipaddr));

	// *** Get IP of host ***
	if (HTTP_TUNNEL_CONNECT_MODE == mode)
	{
		if (get_ip_addr(ipaddr, sizeof(ipaddr), tunnel_info->dest_host))
		{
			free(hs);
			return NULL;
		}
		else
		{
			strncpy(host, ipaddr, sizeof(ipaddr));
		}
	}
	
	hs->mode = mode;
	hs->send_size = 0;
	hs->recv_size = 0;

	if (use_proxy)
	{
		int ret = 0;
		char url_buff[1024];
		char proxy_buff[1024];
		char login_buff[1024];

		memset(proxy_host_ip, 0, sizeof(proxy_host_ip));

		if (get_ip_addr(proxy_host_ip, sizeof(proxy_host_ip), tunnel_info->http_proxy_host) > 0)
		{
			free(hs);
			return NULL;
		}

		proxy_auth_type = get_proxy_auth_type_h(tunnel_host_ip, tunnel_info->tunnel_port, proxy_host_ip, tunnel_info->http_proxy_port);

		hs->curl = curl_easy_init();

		if(hs->curl)
		{
			curl_easy_setopt(hs->curl, CURLOPT_VERBOSE, 1);
			curl_easy_setopt(hs->curl, CURLOPT_DEBUGFUNCTION, _curloutputcbk);

			curl_easy_setopt(hs->curl, CURLOPT_CONNECT_ONLY, 1);

			snprintf(url_buff, sizeof(url_buff), "http://%s:%d", tunnel_host_ip, tunnel_info->tunnel_port);
			curl_easy_setopt(hs->curl, CURLOPT_URL, url_buff);

			snprintf(proxy_buff, sizeof(proxy_buff), "%s:%d", proxy_host_ip, tunnel_info->http_proxy_port);
			curl_easy_setopt(hs->curl, CURLOPT_PROXY, proxy_buff);

			if (timeout > 0)
				curl_easy_setopt(hs->curl, CURLOPT_CONNECTTIMEOUT, timeout);

			if (proxy_auth_type)
			{
				snprintf(login_buff, sizeof(login_buff), "%s:%s", tunnel_info->http_proxy_username, tunnel_info->http_proxy_password);
				curl_easy_setopt(hs->curl, CURLOPT_PROXYUSERPWD, login_buff);
				
				if ((proxy_auth_type & CURLAUTH_BASIC) == CURLAUTH_BASIC)
					curl_easy_setopt(hs->curl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC);
				else if ((proxy_auth_type & CURLAUTH_DIGEST) == CURLAUTH_DIGEST)
					curl_easy_setopt(hs->curl, CURLOPT_PROXYAUTH, CURLAUTH_DIGEST);
				else if ((proxy_auth_type & CURLAUTH_NTLM) == CURLAUTH_NTLM)
					curl_easy_setopt(hs->curl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM);
			}

			curl_easy_setopt(hs->curl, CURLOPT_HTTPPROXYTUNNEL, 1);

			/* <UOLFONE> */
			/*
			if (HTTP_TUNNEL_CONNECT_MODE == mode)
			{
				snprintf(header_buff, sizeof(header_buff), "Connect: %s:%d", ipaddr, tunnel_info->dest_port);
				slist = curl_slist_append(slist, header_buff);
			}
			else
			{
				snprintf(header_buff, sizeof(header_buff), "Accept: ");
				slist = curl_slist_append(slist, header_buff);

				snprintf(connection_accept_timeout_buff, sizeof(connection_accept_timeout_buff), "Accept-Timeout: %d", 
					(connection_accept_timeout > 0) ? connection_accept_timeout : HTTP_TUNNEL_DEFAULT_CONNECTION_ACCEPT_TIMEOUT);
				slist = curl_slist_append(slist, connection_accept_timeout_buff);
			}

			slist = curl_slist_append(slist, "Connection: Keep-Alive"); 
			
			snprintf(keep_alive_timeout_buff, sizeof(keep_alive_timeout_buff), "Keep-Alive: timeout=%d", 
					(keep_alive_timeout > 0) ? keep_alive_timeout : HTTP_TUNNEL_DEFAULT_KEEP_ALIVE_TIMEOUT);
			slist = curl_slist_append(slist, keep_alive_timeout_buff);			
			
			slist = curl_slist_append(slist, "Pragma: no-cache");
			slist = curl_slist_append(slist, "Cache-Control: no-cache");
			curl_easy_setopt(hs->curl, CURLOPT_HTTPHEADER, slist);
			*/
			/* </UOLFONE> */
			
			ret = curl_easy_perform(hs->curl);
			
			//curl_easy_getinfo(hs->curl, CURLINFO_RESPONSE_CODE, http_code);

			if (ret != 0)
			{
				char errbuff[256];

				memset(errbuff, 0, sizeof(errbuff));
				snprintf(errbuff, sizeof(errbuff),
					"http_make_connection : curl_easy_perform returns error code = %d\n", ret);
				HTTPTUN_LOG_ERROR(errbuff);
				http_tunnel_close(hs);
				return NULL;
			}

			/* <UOLFONE> */
			//curl_slist_free_all(slist);
			/* </UOLFONE> */
			
			hs->fd = easy_get_sock(hs->curl);
		}
		else
		{
			HTTPTUN_LOG_ERROR("http_make_connection : curl_easy_init() : failed\n");
			free(hs);
			return NULL;
		}
	}
	else
	{
		int i = 1;
		struct sockaddr_in addr;
		SOCKET sock = 0;

		if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		{
			HTTPTUN_LOG_ERROR(make_error_string("http_make_connection : socket"));
			free(hs);
			return NULL;
		}

#ifdef SO_NOSIGPIPE
		setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &i, sizeof(i));
#endif

		hs->fd = sock;
		addr.sin_port = (unsigned short) htons(tunnel_info->tunnel_port);
		addr.sin_addr.s_addr = inet_addr(tunnel_host_ip);
		addr.sin_family = PF_INET;

		if (connect(hs->fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
		{
			HTTPTUN_LOG_ERROR(make_error_string("http_make_connection : connect"));
			http_tunnel_close(hs);
			return NULL;
		}
	}

	return hs;

}
/* <UOLFONE> */

/* <UOLFONE> */
/*
void *http_tunnel_open(const char *host, int port, int mode, int *http_code, int timeout)
*/
void* http_tunnel_open(http_tunnel_info_t* tunnel_info, int mode, int *http_code, int timeout, int useSSL, int keep_alive_timeout, int connection_accept_timeout)
{
	char query[512];
	char buff[2048];
	char ipaddr[20];
	http_sock_t *hs;
	int nbytes;
	/* <UOLFONE> */
	char tunnel_host_ip_ret[20];
	char *tmp_buff;
	const int HTTP_OK_RESPONSE_LEN = 17;
	const int OUTPUT_ADDR_HEADER_LEN = 13;
	/* </UOLFONE> */

	/* <UOLFONE> */
	/*
	if (!host || !host[0])
	{
		return NULL;
	}

	// 404 (NOT FOUND) default value
	*http_code = 404;

		// *** Get IP of host ***
	memset(ipaddr, 0, sizeof(ipaddr));
	get_ip_addr(ipaddr, sizeof(ipaddr), host);

	if (!(hs = http_make_connection(ipaddr, port, mode, timeout)))
	*/
	memset(ipaddr, 0, sizeof(ipaddr));
	memset(tunnel_host_ip_ret, 0, sizeof(tunnel_host_ip_ret));
	if (!(hs = http_make_connection(tunnel_info, ipaddr, mode, http_code, timeout, useSSL, keep_alive_timeout, connection_accept_timeout, tunnel_host_ip_ret)))
	/* </UOLFONE> */
	{
		HTTPTUN_LOG_ERROR("http_tunnel_open : http_make_connection : failed\n");
		return NULL;
	}

#ifdef HT_USE_SSL
	if (UseSSL)
	{
		hs->s_ssl = SSL_new(ctx);
		//SSL_set_connect_state(hs->s_ssl);
		SSL_set_fd(hs->s_ssl, hs->fd);

		if (SSL_connect(hs->s_ssl) <= 0)
		{
			HTTPTUN_LOG_ERROR(make_error_string("http_tunnel_open : SSL_connect"));
			http_tunnel_close(hs);
			return NULL;
		}
	}
#endif

/* <UOLFONE> */
/*
	if (mode == HTTP_TUNNEL_FIXE_MODE)
	{
		snprintf(query, sizeof(query), "GET / HTTP/1.1\r\n\
UdpHost: %s:%d\r\n\
Mode: 1\r\n\
Connection: Keep-Alive\r\n\
Pragma: no-cache\r\n\
Cache-Control: no-cache\r\n\
Content-lenght: 0\r\n\r\n", ipaddr, port);
	}
	else
	{
		snprintf(query, sizeof(query), "GET / HTTP/1.1\r\n\
UdpHost: %s:%d\r\n\
Connection: Keep-Alive\r\n\
Pragma: no-cache\r\n\
Cache-Control: no-cache\r\n\
Content-lenght: 0\r\n\r\n", ipaddr, port);
	}
*/
	if (mode == HTTP_TUNNEL_CONNECT_MODE)
	{
			snprintf(query, sizeof(query), "GET / HTTP/1.1\r\nHost: %s\r\nConnect: %s:%d\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=%d\r\n\r\n", 
				tunnel_host_ip_ret, ipaddr, tunnel_info->dest_port, 
				(keep_alive_timeout > 0) ? keep_alive_timeout : HTTP_TUNNEL_DEFAULT_KEEP_ALIVE_TIMEOUT);
	}
	else
	{
			snprintf(query, sizeof(query), "GET / HTTP/1.1\r\nHost: %s\r\nAccept: \r\nAccept-Timeout: %d\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=%d\r\n\r\n",
				tunnel_host_ip_ret, (connection_accept_timeout > 0) ? connection_accept_timeout : HTTP_TUNNEL_DEFAULT_CONNECTION_ACCEPT_TIMEOUT,
				(keep_alive_timeout > 0) ? keep_alive_timeout : HTTP_TUNNEL_DEFAULT_KEEP_ALIVE_TIMEOUT);
	}
/* </UOLFONE> */

#ifdef HT_USE_SSL
	if (UseSSL)
		nbytes = SSL_write(hs->s_ssl, query, (int) strlen(query));
	else
#endif
		nbytes = send(hs->fd, query, (int) strlen(query), MSG_NOSIGNAL);

	if (nbytes < 0)
	{
		HTTPTUN_LOG_ERROR(make_error_string("http_tunnel_open : send/SSL_write"));
		http_tunnel_close(hs);
		return NULL;
	}

#ifdef HT_USE_SSL
	if (UseSSL)
		nbytes = get_https_response(hs, buff, sizeof(buff) - 1);
	else
#endif
		/* <UOLFONE> -- do not ignore timeout parameter */
		/*
		nbytes = get_http_response(hs, buff, sizeof(buff) - 1);
		*/
		nbytes = get_http_response(hs, buff, sizeof(buff) - 1, timeout);
		/* <UOLFONE> */

	if (nbytes > 0)
	{
		buff[nbytes] = 0;

		/* <UOLFONE> */
		/*
		if (strncmp(buff, "HTTP/1.0 200 OK\r\n", 17) == 0 || strncmp(buff, "HTTP/1.1 200 OK\r\n", 17) == 0)
		*/
		if (strncmp(buff, "HTTP/1.0 200 OK\r\n", HTTP_OK_RESPONSE_LEN) == 0 || 
			strncmp(buff, "HTTP/1.1 200 OK\r\n", HTTP_OK_RESPONSE_LEN) == 0)
		/* </UOLFONE> */
		{
			*http_code = 200;
			/* <UOLFONE> */
			/*
			return hs;
			*/
			
			/* Get tunnel Output Address */
			tmp_buff = buff + HTTP_OK_RESPONSE_LEN;
			if (strncmp(tmp_buff, "Output_Addr: ", OUTPUT_ADDR_HEADER_LEN) == 0)
			{
				tmp_buff += OUTPUT_ADDR_HEADER_LEN;
				if (0 == parse_output_address(tmp_buff, tunnel_info->output_host, &(tunnel_info->output_port)))
				{
					return hs;
				}
			}
			
			http_tunnel_close(hs);
			return NULL;
			/* </UOLFONE> */
		}
		else
		{
			HTTPTUN_LOG_ERROR("http_tunnel_open : strncmp : http code 200 doesn't match\n");
			http_tunnel_close(hs);
		}
	}
	else
	{
		HTTPTUN_LOG_ERROR("http_tunnel_open : get_http(s)_response : failed\n");
		http_tunnel_close(hs);
	}

	return NULL;
}

int http_tunnel_close(void *h_tunnel)
{
	http_sock_t *hs;

	if (!h_tunnel)
	{
		HTTPTUN_LOG_ERROR("http_tunnel_close : struct http_sock not initialized\n");
		return -1;
	}
	hs = (http_sock_t *) h_tunnel;

	if (hs->curl) {
		curl_easy_cleanup((CURL *)hs->curl);
	}
	else {
		closesocket(hs->fd);
#ifdef HT_USE_SSL
		/* <UOLFONE> */
		/*
		if (UseSSL)
			SSL_free(hs->s_ssl);
		*/
		/* </UOLFONE> */
		if (hs->s_ssl)
			SSL_free(hs->s_ssl);
#endif
	}
	free(hs);
	return 0;
}

int	http_tunnel_send(void *h_tunnel, const void *buffer, int size)
{
	int		send_bytes = 0;
	int		total = 0;
	char	ptr[10000];
	void	*ptr2 = 0;
	int		size2send = 0;
	int		err = 0;
	int		size_unknown = 0;
	fd_set	ofds;
	struct http_sock *hs;
	/* <UOLFONE> */
	long	sizeData = 0;
	/* </UOLFONE> */

	hs = (http_sock_t *) h_tunnel;

	if (!h_tunnel)
	{
		HTTPTUN_LOG_ERROR("http_tunnel_send : struct http_sock not initialized\n");
		return -1;
	}

	if (size > MAX_SIZE)
	{
		HTTPTUN_LOG_ERROR("http_tunnel_send : parameter 'size' is higher than MAX_SIZE\n");
		return HTTP_TUNNEL_ERR_DECONN;
	}

	ptr2 = (void *)buffer;
	size2send = hs->send_size;

	FD_ZERO(&ofds);
	FD_SET(hs->fd, &ofds);

	do
	{
		if (FD_ISSET(hs->fd, &ofds))
		{
			if (hs->send_size == 0)
			{
				size_unknown = 1;
				hs->send_size = size;
				//ptr = malloc(size + sizeof(size));
				/* <UOLFONE> */
				/*
				memcpy(ptr, &size, sizeof(size));
				memcpy(ptr + sizeof(size), buffer, size);
				ptr2 = ptr;
				size2send = size + sizeof(size);
				*/
				sizeData = size;
				sizeData = htonl(sizeData);
				memcpy(ptr, &sizeData, sizeof(sizeData));
				memcpy(ptr + sizeof(sizeData), buffer, size);
				ptr2 = ptr;
				size2send = size + sizeof(sizeData);
				/* </UOLFONE> */
			}
#ifdef HT_USE_SSL
			if (UseSSL)
				send_bytes = SSL_write(hs->s_ssl, (char *) ptr2, size2send);
			else
#endif
			{
				int i = 1;
				int ret = setsockopt(hs->fd, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i));
				send_bytes = send(hs->fd, (char *) ptr2, size2send, MSG_NOSIGNAL);
			}

			if (send_bytes < 0)
			{
				/* <UOLFONE> */
				/* Test */
#if ((defined _WIN32) && (defined _DEBUG_HTTP_TUNNEL))
				OutputDebugString("http_tunnel_send() - send() failed\n");
#endif
				/* Test */
				/* </UOLFONE> */

				HTTPTUN_LOG_ERROR(make_error_string("http_tunnel_send : send/SSL_write"));
				err = WSAGetLastError();
				if (ERR_SOCK(err))
				{
					return HTTP_TUNNEL_ERR_DECONN;
				}
				else
				{
					if (err != WSAEWOULDBLOCK)
						return HTTP_TUNNEL_ERR_UNKNOW;
				}
			}

			if (send_bytes > 0)
				total += send_bytes;
			else
				send_bytes = 0;
			
			if (send_bytes < size2send) 
			{
				size2send -= send_bytes;
			}
			else
			{
				if (size_unknown == 1)
					total -= sizeof(int);
				break;
			}
		}

		FD_ZERO(&ofds);
		FD_SET(hs->fd, &ofds);

		err = select(hs->fd + 1, 0, &ofds, 0, 0);

		if (err <= 0)
		{
			if (err == 0) {
				HTTPTUN_LOG_ERROR("http_tunnel_send : select : timeout reached\n");
			} else { 
				HTTPTUN_LOG_ERROR(make_error_string("http_tunnel_send : select"));
			}

			return -1;
		}

	} while (1);

	/* <UOLFONE> */
	/* Test */
#if ((defined _WIN32) && (defined _DEBUG_HTTP_TUNNEL))
	if (hs && hs->send_size > 0)
	{
		char debug_out[1024];

		memset(debug_out, 0, 1024);
		snprintf(debug_out, 1024, "http_tunnel_send() - len == %d, size2send = %d\n", hs->send_size, size2send);

		OutputDebugString(debug_out);
	}
#endif
	/* Test */
	/*
	if (hs->mode == HTTP_TUNNEL_VAR_MODE)
		hs->send_size = 0;
	*/
	hs->send_size = 0;
	/* </UOLFONE> */

	return total;
}

int http_tunnel_recv(void *h_tunnel, void *buffer, int size)
{
	int total = 0;
	int received = 0;
	int size_tmp = 0;
	int limit = 0;
	int	err = 0;
	http_sock_t *hs;
	/* <UOLFONE> */
	long sizeData = 0;
	/* </UOLFONE> */

	fd_set	rfds;
	struct timeval	to;

	if (!h_tunnel)
	{
		HTTPTUN_LOG_ERROR("http_tunnel_recv : struct http_sock not initialized\n");
		return -1;
	}

	hs = (http_sock_t *) h_tunnel;

/*
	if (nonblock)
		FCNTL_NOBLOCK(hs->fd);
*/

	if (hs->recv_size == 0)
	{
		for (total = 0; total < 4; total += received)
		{
			/* *** INIT TIMEOUT *** */
			to.tv_sec = 4;
			to.tv_usec = 0;
			/* ******************** */

			FD_ZERO(&rfds);
			FD_SET(hs->fd, &rfds);

			err = 0;
#ifdef HT_USE_SSL
			if (UseSSL)
				err = SSL_pending(hs->s_ssl);
#endif

			if (err == 0)
				err = select(hs->fd + 1, &rfds, 0, 0, &to);

			if (err && FD_ISSET(hs->fd, &rfds))
			{
#ifdef HT_USE_SSL
				if (UseSSL)
					received = SSL_read(hs->s_ssl, (char *) &(hs->recv_size) + total, 4 - total);
				else
#endif
					/* <UOLFONE> */
					/*
					received = recv(hs->fd, ((char *) &(hs->recv_size)) + total, 4 - total, 0);
				    */
					received = recv(hs->fd, ((char *) &(sizeData)) + total, 4 - total, 0);
				    /* </UOLFONE> */

				if (received <= 0)
				{
					if (received < 0)
						HTTPTUN_LOG_ERROR(make_error_string("http_tunnel_recv : recv/SSL_read"));
					
					err = WSAGetLastError();

					if (ERR_SOCK(err))
						return HTTP_TUNNEL_ERR_DECONN;
					else if (err != WSAEWOULDBLOCK)
						return HTTP_TUNNEL_ERR_UNKNOW;
					else
						received = 0;
				}
			}
			else
			{
				if (err == 0) {
					HTTPTUN_LOG_ERROR("http_tunnel_recv : select1 : timeout reached\n");
				} else {
					HTTPTUN_LOG_ERROR(make_error_string("http_tunnel_recv : select1"));
				}

				return HTTP_TUNNEL_ERR_UNKNOW;
			}
		}
	}

	/* <UOLFONE> */
	/* Convert from network byte order to host byte order */
	sizeData = ntohl(sizeData);
	hs->recv_size = sizeData;
	/* </UOLFONE> */

	if (hs->recv_size > MAX_SIZE)
		return HTTP_TUNNEL_ERR_DECONN;
	else if (hs->recv_size > size)
		return HTTP_TUNNEL_ERR_BUF2SMA;
	else if (hs->recv_size == 0)
		return 0;

	total = 0;
	limit = (size < hs->recv_size) ? size : hs->recv_size;
	
	for (size_tmp = limit; total < limit; size_tmp = limit - total)
	{
		/* *** INIT TIMEOUT *** */
		to.tv_sec = 4;
		to.tv_usec = 0;
		/* ******************** */

		FD_ZERO(&rfds);
		FD_SET(hs->fd, &rfds);

		err = 0;
#ifdef HT_USE_SSL
		if (UseSSL)
			err = SSL_pending(hs->s_ssl);
#endif
		if (err == 0)
			err = select(hs->fd + 1, &rfds, 0, 0, &to);

		received = 0;

		if (err && FD_ISSET(hs->fd, &rfds))
		{
#ifdef HT_USE_SSL
			if (UseSSL)
				received = SSL_read(hs->s_ssl, (char *) buffer + total, size_tmp);
			else
#endif
				received = recv(hs->fd, (char *) buffer + total, size_tmp, 0);

				if (received <= 0)
				{
					if (received < 0)
						HTTPTUN_LOG_ERROR(make_error_string("http_tunnel_recv : recv/SSL_read"));

					err = WSAGetLastError();

					if (ERR_SOCK(err))
						return HTTP_TUNNEL_ERR_DECONN;
					else if (err != WSAEWOULDBLOCK)
						return HTTP_TUNNEL_ERR_UNKNOW;
					else
						received = 0;
				}
		}
		else
		{
			if (err == 0) {
				HTTPTUN_LOG_ERROR("http_tunnel_recv : select2 : timeout reached\n");
			} else {
				HTTPTUN_LOG_ERROR(make_error_string("http_tunnel_recv : select2"));
			}

			return HTTP_TUNNEL_ERR_UNKNOW;
		}

		if (received > 0)
			total += received;
	}

	/* <UOLFONE> */
	/* Test */
#if ((defined _WIN32) && (defined _DEBUG_HTTP_TUNNEL))
	if (total > 200)
	{
		char debug_out[1024];

		memset(debug_out, 0, 1024);
		snprintf(debug_out, 1024, "http_tunnel_recv() - len == %d\n", total);

		OutputDebugString(debug_out);
	}
#endif
	/* Test */
	/*
	if (hs->mode == HTTP_TUNNEL_VAR_MODE)
		hs->recv_size = 0;
	*/
	hs->recv_size = 0;
	/* </UOLFONE> */
/*
	if (nonblock)
		FCNTL_BLOCK(hs->fd);
*/

	return total;
}

#ifdef __cplusplus
}
#endif
