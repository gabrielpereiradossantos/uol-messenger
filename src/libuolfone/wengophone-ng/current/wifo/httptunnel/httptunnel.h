/*
  Copyright (C) 2005,2006  Wengo SA
  
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

#ifndef	_HTTPTUNNEL_H_
#define	_HTTPTUNNEL_H_

/* <UOLFONE> */
#ifdef HT_USE_SSL
/* </UOLFONE> */
/* ***** SSL ***** */
#include <openssl/ssl.h>
#include <openssl/pem.h>
#include <openssl/err.h>
/* *************** */
/* <UOLFONE> */
#endif
/* </UOLFONE> */

#ifdef __cplusplus
extern "C" {
#endif

/* <UOLFONE> */
/*
extern char	*httpServerIP;
extern int	httpServerPort;
extern char	*proxyServerIP;
extern int	proxyServerPort;

extern int	UseProxy;
*/
/* </UOLFONE> */

/* <UOLFONE> */
/*
#define	HTTP_TUNNEL_FIXE_MODE	1
#define HTTP_TUNNEL_VAR_MODE	0
*/
#define HTTP_TUNNEL_CONNECT_MODE 0
#define HTTP_TUNNEL_ACCEPT_MODE 1
/* </UOLFONE> */

#define MAX_SIZE	10000
#define HTTP_TUNNEL_ERR_UNKNOW	-1
#define HTTP_TUNNEL_ERR_DECONN	-2
#define HTTP_TUNNEL_ERR_BUF2SMA	-3

/* <UOLFONE> */
#define HTTP_TUNNEL_HOST_MAX_LEN 128
#define HTTP_TUNNEL_PROXY_USENAME_MAX_LENGTH 64
#define HTTP_TUNNEL_PROXY_PASSWORD_MAX_LENGTH 64
#define HTTP_TUNNEL_DEFAULT_KEEP_ALIVE_TIMEOUT 15
#define HTTP_TUNNEL_DEFAULT_CONNECTION_ACCEPT_TIMEOUT 30
/* </UOLFONE> */

typedef struct	http_sock
{
	int		fd;
	int		mode;
	int		send_size;
	int		recv_size;
	void	*curl;
/* <UOLFONE> */
#ifdef HT_USE_SSL
/* </UOLFONE> */
	SSL		*s_ssl;
/* <UOLFONE> */
#endif
/* </UOLFONE> */
}				http_sock_t;

typedef struct	http_distant_host
{
	const char *host;
	int	port;
}				http_distant_host_t;



//<MINHPQ>
typedef enum {
	SOCK_MODE_UDP_NO_TUNNEL,
	SOCK_MODE_HTTP_TUNNEL
} tunnel_sock_mode_t;
//</MINHPQ>


/* <UOLFONE> */
typedef struct http_tunnel_info
{
	char tunnel_host[HTTP_TUNNEL_HOST_MAX_LEN + 1];
	int tunnel_port;
	char http_proxy_host[HTTP_TUNNEL_HOST_MAX_LEN + 1];
	int http_proxy_port;
	char http_proxy_username[HTTP_TUNNEL_PROXY_USENAME_MAX_LENGTH + 1];
	char http_proxy_password[HTTP_TUNNEL_PROXY_PASSWORD_MAX_LENGTH + 1];
	char dest_host[HTTP_TUNNEL_HOST_MAX_LEN + 1];
	int dest_port;
	char output_host[HTTP_TUNNEL_HOST_MAX_LEN + 1];
	int output_port;
} http_tunnel_info_t;
/* </UOLFONE> */


typedef enum {
	HTTPTUN_LOG_UNK = 0,
	HTTPTUN_LOG_IN,
	HTTPTUN_LOG_OUT,
	HTTPTUN_LOG_INFO,
	HTTPTUN_LOG_ERR
} HTTPTUNNEL_LOG_TYPE;

typedef void (*HttpTunnelLogCbk)(HTTPTUNNEL_LOG_TYPE type, const char * buffer);

void http_tunnel_set_write_log_callback(HttpTunnelLogCbk cbk);

int http_tunnel_get_socket(void *h_tunnel);
int http_tunnel_recv(void *h_tunnel, void *buffer, int size);
int	http_tunnel_send(void *h_tunnel, const void *buffer, int size);
/* <UOLFONE> */
//void* http_tunnel_open(const char *host, int port, int mode, int *http_code, int timeout);
void* http_tunnel_open(http_tunnel_info_t* tunnel_info, int mode, int *http_code, int timeout, int useSSL, int keep_alive_timeout, int connection_accept_timeout);
/* </UOLFONE> */
int http_tunnel_close(void *h_tunnel);
/* <UOLFONE> */
/*
void http_tunnel_init_host(const char *hostname, int port, int ssl);
void http_tunnel_init_proxy(const char *hostname, int port, const char *username, const char *password);
*/
/* </UOLFONE> */
void http_tunnel_clean_up();
#ifdef __cplusplus
}
#endif

#endif
