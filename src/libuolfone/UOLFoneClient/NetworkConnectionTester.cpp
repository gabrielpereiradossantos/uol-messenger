/* UOL Fone
 * Copyright (c) 2006 Universo Online S/A
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
 * Universo Online S/A - A/C: UOL Fone 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */


#include "stdafx.h"
#include "NetworkConnectionTester.h"

#include <netlib.h>
#include <httptunnel.h>
#include "ConnectionTestResult.h"


CNetworkConnectionTester::CNetworkConnectionTester() :
	m_lPort(0)
{
}


CNetworkConnectionTester::~CNetworkConnectionTester()
{
}


void CNetworkConnectionTester::SetLocalPort(LONG lPort)
{
	m_lPort = lPort;
}


HRESULT CNetworkConnectionTester::TestConnection(IUOLFoneClientConnectionTestParam* pConnectionTestParam, IUOLFoneClientConnectionTestResult** ppConnectionTestResult)
{
	HRESULT hr = E_FAIL;

	if ((pConnectionTestParam != NULL) &&
		(ppConnectionTestResult != NULL))
	{
		LONG lResult = UFC_CONNECTION_TEST_RESULT_NO_CONNECTION;

		*ppConnectionTestResult = NULL;

		// Create/initialize result COM object...
		CComObject<CConnectionTestResult>* pConnectionTestResultObj = NULL;

		hr = CComObject<CConnectionTestResult>::CreateInstance(&pConnectionTestResultObj);

		if (FAILED(hr))
		{
			return hr;
		}

		pConnectionTestResultObj->SetAvailablePort(m_lPort);
		pConnectionTestResultObj->SetResult(lResult);

		IUOLFoneClientConnectionTestResult* pConnectionTestResult = pConnectionTestResultObj;

		pConnectionTestResult->AddRef();
		*ppConnectionTestResult = pConnectionTestResult;

		// Test connections...
		CComPtr<IUOLFoneClientServerInfo> spSipServerInfo;

		hr = pConnectionTestParam->GetServerInfo(&spSipServerInfo);

		if ((SUCCEEDED(hr)) && (m_lPort > 0))
		{
			CComBSTR bstrHost;
			
			hr = spSipServerInfo->GetServerHost(&bstrHost);

			if (SUCCEEDED(hr))
			{
				LONG lServerPort = 0;
				hr = spSipServerInfo->GetServerPort(&lServerPort);

				if ((FAILED(hr)) ||	(lServerPort <= 0))
				{
					return hr;
				}

				CComBSTR bstrSipRealm;
				
				hr = spSipServerInfo->GetSipRealm(&bstrSipRealm);

				if (SUCCEEDED(hr))
				{
					LONG lServerResponseTimeout = 0;
					hr = pConnectionTestParam->GetServerResponseTimeout(&lServerResponseTimeout);
					
					if ((FAILED(hr)) || (lServerResponseTimeout <= 0))
					{
						lServerResponseTimeout = UDP_TIMEOUT;
					}

					// Test UDP connection
					const int MAX_RETRIES = 3;

					CString strHostName(bstrHost);
					CString strHostAddress = strHostName;
					CString strSipRealm(bstrSipRealm);

					if (GetHostAddress(strHostName, lServerPort, strHostAddress))
					{
						for (int nRetries = 0; nRetries < MAX_RETRIES; nRetries++)
						{
							if (udp_sip_ping(strHostAddress, lServerPort, m_lPort, lServerResponseTimeout, "nobody", strSipRealm))
							{
								lResult |= UFC_CONNECTION_TEST_RESULT_ALLOW_UDP;
								break;
							}
						}
					}

					if (lResult & UFC_CONNECTION_TEST_RESULT_ALLOW_UDP)
					{
						hr = S_OK;
						pConnectionTestResultObj->SetResult(lResult);
						return hr;
					}

					// Test HTTP tunnel connection...
					BOOL bUseHttpTunnel = FALSE;
					CComBSTR bstrHttpTunnelHost;
					LONG lHttpTunnelPort = 0;

					CComPtr<IUOLFoneClientServerInfo> pHttpTunnelInfo;

					hr = pConnectionTestParam->GetHttpTunnelInfo(&pHttpTunnelInfo);

					if (SUCCEEDED(hr) && pHttpTunnelInfo)
					{
						pHttpTunnelInfo->GetServerHost(&bstrHttpTunnelHost);
						pHttpTunnelInfo->GetServerPort(&lHttpTunnelPort);

						bUseHttpTunnel = ((bstrHttpTunnelHost.Length() > 0) && (lHttpTunnelPort > 0));
					}

					BOOL bUseHttpProxy = FALSE;
					CComBSTR bstrHttpProxyHost;
					LONG lHttpProxyPort = 0;
					CComBSTR bstrHttpProxyUsername;
					CComBSTR bstrHttpProxyPassword;

					if (bUseHttpTunnel)
					{
						LONG lHttpTunnelResponseTimeout = 0;
						hr = pConnectionTestParam->GetHttpTunnelResponseTimeout(&lHttpTunnelResponseTimeout);
						
						if ((FAILED(hr)) || (lHttpTunnelResponseTimeout <= 0))
						{
							lHttpTunnelResponseTimeout = HTTP_TUNNEL_TIMEOUT;
						}

						CComPtr<IUOLFoneClientServerInfo> pHttpProxyInfo;

						hr = pConnectionTestParam->GetHttpProxyInfo(&pHttpProxyInfo);

						if (SUCCEEDED(hr) && pHttpProxyInfo)
						{
							pHttpProxyInfo->GetServerHost(&bstrHttpProxyHost);
							pHttpProxyInfo->GetServerPort(&lHttpProxyPort);

							pHttpProxyInfo->GetUsername(&bstrHttpProxyUsername);
							pHttpProxyInfo->GetPassword(&bstrHttpProxyPassword);

							bUseHttpProxy = ((bstrHttpProxyHost.Length() > 0) && (lHttpProxyPort > 0));
						}

						LONG lHttpTunnelMode = HTTP_TUNNEL_CONNECT_MODE;

						hr = pConnectionTestParam->GetHttpTunnelMode(&lHttpTunnelMode);

						HttpRet ret = is_tunnel_conn_allowed(CString(bstrHttpTunnelHost), lHttpTunnelPort,
															strHostAddress, lServerPort,
															CString(bstrHttpProxyHost), lHttpProxyPort,
															CString(bstrHttpProxyUsername), CString(bstrHttpProxyPassword),
															NETLIB_FALSE, lHttpTunnelResponseTimeout, strSipRealm, 
															NETLIB_TRUE, lServerResponseTimeout,
															lHttpTunnelMode);
						if (ret == HTTP_OK)
						{
							lResult |= UFC_CONNECTION_TEST_RESULT_ALLOW_HTTP_TUNNEL;
						}
					}

					pConnectionTestResultObj->SetResult(lResult);
				}
			}
		}
	}

	return hr;
}


BOOL CNetworkConnectionTester::GetHostAddress(const CString& strHostName, LONG lPort, CString& strAddress)
{
	int nRetVal;
	
	CString strPort;
	strPort.Format(_T("%d"), lPort);

	addrinfo aiHints;
	::memset(&aiHints, 0, sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;

	addrinfo* aiList = NULL;

	if ((nRetVal = ::getaddrinfo((LPCTSTR) strHostName, (LPCTSTR) strPort, &aiHints, &aiList)) == 0)
	{
		strAddress = ::inet_ntoa( ((struct sockaddr_in *)aiList->ai_addr)->sin_addr);
		::freeaddrinfo(aiList);
	}

	return (0 == nRetVal);
}