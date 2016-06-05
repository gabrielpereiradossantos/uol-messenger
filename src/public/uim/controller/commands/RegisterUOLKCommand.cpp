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
 
#include "StdAfx.h"
#include <commands/registeruolkcommand.h>

#include "../../resource.h"
#include "../../Authentication/InternetAuthentication.h"
#include "../utils.h"
#include "../UIMApplication.h"
#include <InternetHandle.h>

CRegisterUOLKCommand::CRegisterUOLKCommand(IUOLMessengerAccountPtr pAccount, BOOL bSilent) : 
m_pAccount(pAccount), 
m_bSilent(bSilent)
{
	m_authResult = CInternetAuthentication::AUTH_FALSE;
}

CRegisterUOLKCommand::~CRegisterUOLKCommand(void)
{
}

BOOL CRegisterUOLKCommand::RegisterSuccessfull()
{
	return m_bRegistred;
}

void CRegisterUOLKCommand::Execute()
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	CAuthentication *authentication;

	authentication = new CAuthentication;

	if (NULL == authentication)
	{
		ATLASSERT(FALSE);
	}

	CString strCookie;

	m_authResult = authentication->DoAuthentication(m_pAccount->GetUserId(), m_pAccount->GetPassword(), strCookie);	

	delete authentication;

	m_bRegistred = FALSE;

	if (CInternetAuthentication::AUTH_OK == m_authResult)
	{
		CString strUserAgent = GetIEUserAgent();
		ATLASSERT(strUserAgent.IsEmpty() == FALSE);

		CInternetHandle hInet = ::InternetOpen(strUserAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

		if (hInet)
		{
			const TCHAR* pszAccept[] = {"text/html", NULL};

			CString strURL;
			strURL.LoadString(IDS_STRING_UOLK_REGISTRATION_URL);

			CUrl url;

			if (TRUE == url.CrackUrl(strURL))
			{
				CString strCookieName(strCookie.Mid(0, strCookie.Find("=")));
				CString strCookieValue(strCookie.Mid(strCookie.Find("=")+1));

				::InternetSetCookie((LPCTSTR)strURL, (LPCSTR)strCookieName, (LPCSTR)strCookieValue);

				DWORD_PTR pContext = NULL;

				CInternetHandle hConnect = ::InternetConnect(hInet, url.GetHostName(), url.GetPortNumber(), NULL, NULL, INTERNET_SERVICE_HTTP, 0, pContext);

				if (hConnect)
				{
					DWORD dwFlags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_AUTO_REDIRECT;

					CInternetHandle hRequest = ::HttpOpenRequest(hConnect, "GET", url.GetUrlPath(), NULL, NULL, pszAccept, dwFlags, pContext);

					if (hRequest)
					{
						if (::HttpSendRequest(hRequest, NULL, NULL, NULL, NULL))
						{	
							TCHAR szStatusCode[20] = {0};
							DWORD dwStatusCodeSize = sizeof(szStatusCode) / sizeof(TCHAR);

							DWORD dwStatusCode = 0;
							if (::HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwStatusCodeSize, NULL))
							{
								dwStatusCode = (DWORD) _ttol(szStatusCode);
							}

							if (dwStatusCode == 200)
							{
								DWORD dwBytesRead;
								DWORD dwRead;
								DWORD dwBytesInBuffer;
								TCHAR*	pBuffer;

								dwBytesRead = 0;
								pBuffer = NULL;

								dwBytesInBuffer = 0;
								::InternetQueryDataAvailable(hRequest, &dwBytesInBuffer, 0, 0);

								CString strResponse;

								while (dwBytesInBuffer > 0)
								{
									pBuffer = new TCHAR[dwBytesInBuffer + 1];
									::ZeroMemory((PVOID) pBuffer, (SIZE_T) (dwBytesInBuffer + 1));

									if (::InternetReadFile(hRequest, pBuffer, dwBytesInBuffer, &dwRead))
									{
										pBuffer[dwRead] = '\0';
										strResponse += pBuffer;

										delete[] pBuffer;
										pBuffer = NULL;

										dwBytesInBuffer = 0;
										if (!::InternetQueryDataAvailable(hRequest, &dwBytesInBuffer, 0, 0))
										{
											break;
										}
									}
								}

								if (pBuffer)
								{
									delete[] pBuffer;
									pBuffer = NULL;
								}

								CString strSuccessfullyResponse;
								strSuccessfullyResponse.LoadString(IDS_STRING_UOLK_SUCCESSFULLY_RESPONSE);

								if (-1 != strResponse.Find(strSuccessfullyResponse))
								{
									m_bRegistred = TRUE;
								}
							}
						}
					}
				}
			}
		}
	}

	if (!m_bRegistred)
	{
		if (!m_bSilent)
		{
			CString strMessage;

			UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			strMessage = pStringLoader->GetValue("uol.resource.IDS_STRING_UOLK_REGISTRATION_FAILED_MESSAGE");

			UINT ret = CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_YESNO | MB_ICONERROR);
            BOOL bAutoConnect = (ret == IDYES);

			m_pAccount->EnableAutoConnect(bAutoConnect);
		}
	}
}