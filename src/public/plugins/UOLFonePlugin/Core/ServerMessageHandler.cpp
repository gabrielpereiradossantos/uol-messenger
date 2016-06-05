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
#include "ServerMessageHandler.h"



CServerMessageHandler::CServerMessageHandler() : 
		m_hrInitialize(S_OK)
{
}


CServerMessageHandler::~CServerMessageHandler()
{
}


BOOL CServerMessageHandler::Initialize(IUOLFoneServerConfigProvider* pServerConfigProvider)
{
	if (pServerConfigProvider != NULL)
	{
		m_hrInitialize = pServerConfigProvider->GetMessageParser(&m_pMessageParser);
		ATLASSERT(SUCCEEDED(m_hrInitialize));
		
		return SUCCEEDED(m_hrInitialize);
	}

	return FALSE;
}


void CServerMessageHandler::Finalize()
{
	if (m_pMessageParser)
	{
		m_pMessageParser.Release();
	}
}


CServerMessageHandler::MessageType CServerMessageHandler::HandleMessage(CUOLFoneAccountPtr pAccount, BSTR bstrMessageText)
{
	if ((!pAccount) || (!bstrMessageText) || (!m_pMessageParser))
	{
		return CServerMessageHandler::MessageTypeUnknown;
	}

	DWORD messageType = CServerMessageHandler::MessageTypeUnknown;

	CPhoneCallPtr pCurrentPhoneCall = pAccount->GetCurrentPhoneCall();

	CComPtr<IUOLFoneMessageInfoEnum> pMessageInfoEnum;

	HRESULT hr = m_pMessageParser->ParseMessage(bstrMessageText, &pMessageInfoEnum);

	if (SUCCEEDED(hr))
	{
		ULONG nCount = 0;
		
		CErrorInfoPtr pErrorInfo;

		do
		{
			CComPtr<IUOLFoneMessageInfo> pMessageInfo;
			ULONG lFetched = 0;

			hr = pMessageInfoEnum->Next(1, &pMessageInfo, &lFetched);

			if (FAILED(hr))
			{
				break;
			}

			if (pMessageInfo)
			{
				LONG lMessageType;
				CComVariant varValue;

				hr = pMessageInfo->GetType(&lMessageType);
				ATLASSERT(SUCCEEDED(hr));

				hr = pMessageInfo->GetValue(&varValue);
				ATLASSERT(SUCCEEDED(hr));

				nCount++;

				if ((lMessageType == UFS_MESSAGE_TYPE_CREDITS) &&
					(varValue.vt == VT_R8))
				{
					pAccount->SetBalance(varValue.dblVal);
					messageType |= CServerMessageHandler::MessageTypeAccountInfo;
				}

				else if ((lMessageType == UFS_MESSAGE_TYPE_RATE) &&
						 (varValue.vt == VT_R8))
				{
					if (pCurrentPhoneCall)
					{
						pCurrentPhoneCall->SetCostPerMinute(varValue.dblVal);
						messageType |= CServerMessageHandler::MessageTypePhoneCallInfo;
					}
				}

				else if	((lMessageType == UFS_MESSAGE_TYPE_CALL_COST) &&
						 (varValue.vt == VT_R8))
				{
					if (pCurrentPhoneCall)
					{
						pCurrentPhoneCall->SetTotalCost(varValue.dblVal);
						messageType |= CServerMessageHandler::MessageTypePhoneCallInfo;
					}
				}

				else if ((lMessageType == UFS_MESSAGE_TYPE_ERROR_MESSAGE) && 
						 (varValue.vt == VT_BSTR))
				{
					if (!pErrorInfo)
					{
						pErrorInfo = new CErrorInfo();
					}

					pErrorInfo->SetMessage(varValue.bstrVal);
					messageType = CServerMessageHandler::MessageTypeError;
				}

				else if ((lMessageType == UFS_MESSAGE_TYPE_ERROR_URL) &&
						 (varValue.vt == VT_BSTR))
				{
					if (!pErrorInfo)
					{
						pErrorInfo = new CErrorInfo();
					}

					pErrorInfo->SetUrl(varValue.bstrVal);
					messageType = CServerMessageHandler::MessageTypeError;
				}

				else if ((lMessageType == UFS_MESSAGE_TYPE_DISPLAY_URL) &&
						 (varValue.vt == VT_BSTR))
				{
					if (!pErrorInfo)
					{
						pErrorInfo = new CErrorInfo();
					}

					pErrorInfo->SetDisplayUrl(varValue.bstrVal);
				}

				else if ((lMessageType == UFS_MESSAGE_TYPE_STATUS_CODE) &&
						 (varValue.vt == VT_I4))							
				{
					if (!pErrorInfo)
					{
						pErrorInfo = new CErrorInfo();
					}

					pErrorInfo->SetStatusCode(varValue.intVal);

					if (messageType == CServerMessageHandler::MessageTypeUnknown)
					{
						messageType |= CServerMessageHandler::MessageTypeAccountInfo;
					}
				}

				else if ((lMessageType == UFS_MESSAGE_TYPE_PLAN_NAME) &&
						 (varValue.vt == VT_BSTR))
				{
					pAccount->SetPlanName(varValue.bstrVal);
					messageType |= CServerMessageHandler::MessageTypeAccountInfo;
				}

				else if ((lMessageType == UFS_MESSAGE_TYPE_PLAN_AVAIL_SECS) &&
						 (varValue.vt == VT_I4))
				{
					pAccount->SetPlanSeconds(varValue.intVal);
					messageType |= CServerMessageHandler::MessageTypeAccountInfo;
				}

				else if	((lMessageType == UFS_MESSAGE_TYPE_PLAN_USED_SECS) &&
						 (varValue.vt == VT_I4))
				{
					pAccount->SetPlanUsedSeconds(varValue.intVal);
					messageType |= CServerMessageHandler::MessageTypeAccountInfo;
				}

				else if	((lMessageType == UFS_MESSAGE_TYPE_CONNECTION_FEE) &&
						 (varValue.vt == VT_R8))
				{
					if (pCurrentPhoneCall)
					{
						pCurrentPhoneCall->SetConnectionFee(varValue.dblVal);
						messageType |= CServerMessageHandler::MessageTypePhoneCallInfo;
					}
				}

				else if	((lMessageType == UFS_MESSAGE_TYPE_SESSION_USE_CREDITS) &&
						 (varValue.vt == VT_BSTR))
				{
					if (pCurrentPhoneCall)
					{
						CString strValue(varValue.bstrVal);
						BOOL bUsing = (0 == strValue.Compare("true") ? TRUE : FALSE);
						pCurrentPhoneCall->SetSessionUsingCredits(bUsing);
						messageType |= CServerMessageHandler::MessageTypePhoneCallInfo;
					}
				}

				else if	((lMessageType == UFS_MESSAGE_TYPE_AVAILABLE_PRODUCTS) &&
						 (varValue.vt == VT_BSTR))
				{
					pAccount->SetInscriptions(varValue.bstrVal);
					messageType |= CServerMessageHandler::MessageTypeAccountInfo;
				}

				else if (lMessageType == UFS_MESSAGE_TYPE_UNKNOWN)
				{
					ATLTRACE(_T("%s\r\nUnknown type"), CString(bstrMessageText));
				}
				else
				{
					ATLTRACE(_T("%s\r\nCount = %d\r\nType = %d \r\nInvalid value"), CString(bstrMessageText), nCount, lMessageType);
				}

				pMessageInfo.Release();
			}
		}
		while (hr != S_FALSE);

		if (pErrorInfo)
		{
			if (pCurrentPhoneCall)
			{
				pCurrentPhoneCall->SetErrorInfo(pErrorInfo);
			}
			else
			{
				pAccount->SetErrorInfo(pErrorInfo);
			}
		}

		pMessageInfoEnum.Release();
	}

	return (CServerMessageHandler::MessageType)messageType;
}


HRESULT CServerMessageHandler::GetInitializeError()
{
	return m_hrInitialize;
}

