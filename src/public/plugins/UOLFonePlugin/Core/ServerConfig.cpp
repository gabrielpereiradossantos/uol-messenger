/* UOL Messenger
 * Copyright (c) 2005 Universo Online S/A
 *
 * Direitos Autorais Reservados
 * All rights reserved
 *
 * Este programa é software livre; você pode redistribuí-lo e/ou modificá-lo
 * sob os termos da Licença Pública Geral GNU conforme publicada pela Free
 * Software Foundation; tanto a versão 2 da Licença, como (a seu critério)
 * qualquer versão posterior.
 * Este programa é distribuído na expectativa de que seja útil, porém,
 * SEM NENHUMA GARANTIA; nem mesmo a garantia implícita de COMERCIABILIDADE
 * OU ADEQUAÇÃO A UMA FINALIDADE ESPECÍFICA. Consulte a Licença Pública Geral
 * do GNU para mais detalhes. 
 * Você deve ter recebido uma cópia da Licença Pública Geral do GNU junto
 * com este programa; se não, escreva para a Free Software Foundation, Inc.,
 * no endereço 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Universo Online S/A - A/C: UOL Messenger 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */
 

#include "stdafx.h"
#include "ServerConfig.h"

#include <ws2tcpip.h>
#include "com/ComFacade.h"
#include "../FacadeWrapper.h"


#define SIP_SERVER_HOST _T("sip.uol.com.br")
#define SIP_SERVER_PORT 5060

#define SIP_HTTP_TUNNEL_HOST _T("tunnel.sip.uol.com.br")
#define SIP_HTTP_TUNNEL_PORT 80

#define HTTP_PROXY_HOST		_T("")
#define HTTP_PROXY_PORT		80
#define HTTP_PROXY_USERNAME _T("")
#define HTTP_PROXY_PASSWORD _T("")

#define MEDIA_HTTP_TUNNEL_HOST _T("mediatunnel.sip.uol.com.br")
#define MEDIA_HTTP_TUNNEL_PORT 80

#define SIP_DOMAIN _T("sip.uol.com.br")



CServerConfig::CServerConfig()
{
}


CServerConfig::~CServerConfig()
{
}


BOOL CServerConfig::AllocConfig()
{
	unsigned uTime = (unsigned) time(NULL);

	srand(uTime);

	// Create SIP server and SIP proxy config
	CString strSipServerAddress;

	if (GetHostAddress(SIP_SERVER_HOST, strSipServerAddress))
	{
		m_pServerInfo = CreateServerInfoObject(strSipServerAddress, SIP_SERVER_PORT, NULL, NULL);
		m_pProxyInfo = CreateServerInfoObject(strSipServerAddress, SIP_SERVER_PORT, NULL, NULL);
	}
	else
	{
		m_pServerInfo = CreateServerInfoObject(SIP_SERVER_HOST, SIP_SERVER_PORT, NULL, NULL);
		m_pProxyInfo = CreateServerInfoObject(SIP_SERVER_HOST, SIP_SERVER_PORT, NULL, NULL);
	}

	ATLASSERT(m_pServerInfo != NULL);
	ATLASSERT(m_pProxyInfo != NULL);

	// Create HTTP tunnel server
	CString strTunnelAddress;

	if (GetHostAddress(SIP_HTTP_TUNNEL_HOST, strTunnelAddress))
	{
		m_pHttpTunnelInfo = CreateServerInfoObject(strTunnelAddress, SIP_HTTP_TUNNEL_PORT, NULL, NULL);
	}
	else
	{
		m_pHttpTunnelInfo = CreateServerInfoObject(SIP_HTTP_TUNNEL_HOST, SIP_HTTP_TUNNEL_PORT, NULL, NULL);
	}

	ATLASSERT(m_pHttpTunnelInfo != NULL);

	m_pHttpProxyInfo = CreateServerInfoObject(HTTP_PROXY_HOST, HTTP_PROXY_PORT, HTTP_PROXY_USERNAME, HTTP_PROXY_PASSWORD);
	ATLASSERT(m_pHttpProxyInfo != NULL);

	// Create HTTP media tunnel server
	CString strMediaTunnelAddress;

	if (GetHostAddress(MEDIA_HTTP_TUNNEL_HOST, strMediaTunnelAddress))
	{
		m_pMediaHttpTunnelInfo = CreateServerInfoObject(strMediaTunnelAddress, MEDIA_HTTP_TUNNEL_PORT, NULL, NULL);
	}
	else
	{
		m_pMediaHttpTunnelInfo = CreateServerInfoObject(MEDIA_HTTP_TUNNEL_HOST, MEDIA_HTTP_TUNNEL_PORT, NULL, NULL);
	}

	ATLASSERT(m_pMediaHttpTunnelInfo != NULL);

	return (m_pServerInfo != NULL) && (m_pProxyInfo != NULL);
}


void CServerConfig::ReleaseConfig()
{
	if (m_pServerInfo)
	{
		m_pServerInfo->Release();
	}

	if (m_pProxyInfo)
	{
		m_pProxyInfo->Release();
	}

	if (m_pHttpTunnelInfo)
	{
		m_pHttpTunnelInfo->Release();
	}

	if (m_pHttpProxyInfo)
	{
		m_pHttpProxyInfo->Release();
	}

	if (m_pMediaHttpTunnelInfo)
	{
		m_pMediaHttpTunnelInfo->Release();
	}
}


CString CServerConfig::GetAuthenticationRealm()
{
	return SIP_DOMAIN;
}


HRESULT CServerConfig::GetServerInfo(IUOLFoneClientServerInfo** ppServerInfo)
{
	HRESULT hr = E_POINTER;

	if (ppServerInfo)
	{
		*ppServerInfo = m_pServerInfo;
		if (*ppServerInfo)
		{
			(*ppServerInfo)->AddRef();
			hr = S_OK;
		}
	}

	return hr;
}


HRESULT CServerConfig::GetProxyInfo(IUOLFoneClientServerInfo** ppServerInfo)
{
	HRESULT hr = E_POINTER;

	if (ppServerInfo)
	{
		*ppServerInfo = m_pProxyInfo;
		if (*ppServerInfo)
		{
			(*ppServerInfo)->AddRef();
			hr = S_OK;
		}
	}

	return hr;
}


HRESULT CServerConfig::GetHttpTunnelInfo(IUOLFoneClientServerInfo** ppServerInfo)
{
	HRESULT hr = E_POINTER;

	if (ppServerInfo)
	{
		*ppServerInfo = m_pHttpTunnelInfo;
		if (*ppServerInfo)
		{
			(*ppServerInfo)->AddRef();
			hr = S_OK;
		}
	}

	return hr;
}


HRESULT CServerConfig::GetHttpProxyInfo(IUOLFoneClientServerInfo** ppServerInfo)
{
	HRESULT hr = E_POINTER;

	if (ppServerInfo)
	{
		// Retrieve UOL Messenger proxy network configuration
		IUOLMessengerFacade* pFacade = CFacadeWrapper::GetInstance()->GetFacadeInstance();

		if (pFacade != NULL)
		{
			IUOLMessengerSettingsManagerPtr pSettingsManager = pFacade->GetSettingsManager();

			if (pSettingsManager)
			{
				IUOLMessengerNetworkSettingsPtr pNetworkSettings = pSettingsManager->GetNetworkSettings();

				if ((pNetworkSettings) && (pNetworkSettings->GetProxyType() == IUOLMessengerNetworkSettings::PT_PROXY_HTTP))
				{
					m_pHttpProxyInfo->SetHost(pNetworkSettings->GetProxyHost());
					m_pHttpProxyInfo->SetPort(pNetworkSettings->GetProxyPort());
					m_pHttpProxyInfo->SetUsername(pNetworkSettings->GetProxyUsername());
					m_pHttpProxyInfo->SetPassword(pNetworkSettings->GetProxyPassword());
				}
			}
		}

		*ppServerInfo = m_pHttpProxyInfo;

		if (*ppServerInfo)
		{
			(*ppServerInfo)->AddRef();
			hr = S_OK;
		}
	}

	return hr;
}


HRESULT CServerConfig::GetMediaHttpTunnelInfo(IUOLFoneClientServerInfo** ppServerInfo)
{
	HRESULT hr = E_POINTER;

	if (ppServerInfo)
	{
		*ppServerInfo = m_pMediaHttpTunnelInfo;
		if (*ppServerInfo)
		{
			(*ppServerInfo)->AddRef();
			hr = S_OK;
		}
	}

	return hr;
}


BOOL CServerConfig::IsHttpProxyEnabled()
{
	// Retrieve UOL Messenger proxy network configuration
	IUOLMessengerFacade* pFacade = CFacadeWrapper::GetInstance()->GetFacadeInstance();

	if (pFacade != NULL)
	{
		IUOLMessengerSettingsManagerPtr pSettingsManager = pFacade->GetSettingsManager();

		if (pSettingsManager)
		{
			IUOLMessengerNetworkSettingsPtr pNetworkSettings = pSettingsManager->GetNetworkSettings();

			return ((pNetworkSettings) && (pNetworkSettings->GetProxyType() == IUOLMessengerNetworkSettings::PT_PROXY_HTTP));
		}
	}

	return FALSE;
}


CComObject<CServerInfo>* CServerConfig::CreateServerInfoObject(LPCTSTR lpszHost, LONG lPort, LPCTSTR lpszUsername, LPCTSTR lpszPassword)
{
	CComObject<CServerInfo>* pServerInfoObject = NULL;

	HRESULT hr = CComObject<CServerInfo>::CreateInstance(&pServerInfoObject);

	if (SUCCEEDED(hr))
	{
		pServerInfoObject->AddRef();

		pServerInfoObject->SetHost(lpszHost);
		pServerInfoObject->SetPort(lPort);

		if (lpszUsername)
		{
			pServerInfoObject->SetUsername(lpszUsername);
		}

		if (lpszPassword)
		{
			pServerInfoObject->SetPassword(lpszPassword);
		}
	}

	return pServerInfoObject;
}


BOOL CServerConfig::GetHostAddress(const CString& strHostName, CString& strAddress)
{
	/// <TEST>
	// Do not resolve host address
	return FALSE;
	/// </TEST>

	int nRetVal = -1;
	
	if (FALSE == strHostName.IsEmpty())
	{
		addrinfo aiHints;
		::memset(&aiHints, 0, sizeof(aiHints));
		aiHints.ai_family = AF_INET;

		addrinfo* aiList = NULL;

		if ((nRetVal = ::getaddrinfo((LPCTSTR) strHostName, NULL, &aiHints, &aiList)) == 0)
		{
			addrinfo* aiCurrent = aiList;

			int nMaxWeight = -1;

			while (aiCurrent != NULL)
			{
				CString strCurAddress = ::inet_ntoa( ((struct sockaddr_in *)aiCurrent->ai_addr)->sin_addr);

				int nWeight = rand();

				if (nWeight > nMaxWeight)
				{
					nMaxWeight = nWeight;
					strAddress = strCurAddress;
				}

				ATLTRACE(_T("%s - Host: %s, IP: %s, nWeight = %d\n"), __FUNCTION__, strHostName, strCurAddress, nWeight);

				aiCurrent = aiCurrent->ai_next;
			}

			::freeaddrinfo(aiList);

			ATLTRACE(_T("%s - Host: %s, Return Address: %s\n"), __FUNCTION__, strHostName, strAddress);
		}
	}

	return (0 == nRetVal);
}
