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
 

#include "PurpleProtocolAsyncExec.h"
#include "PurpleService.h"


using namespace std;


// Private structures to handle operations...
//
struct STR_OPERATION
{
	int		iParamType;
	string	strParam1;
	CIMProtocol* pProtocol;
	IMProtocolError (CIMProtocol::*pfnHandler)(const string&);
};

struct STR_STR_OPERATION
{
	int		iParamType;
	string	strParam1;
	string	strParam2;
	CIMProtocol* pProtocol;
	IMProtocolError (CIMProtocol::*pfnHandler)(const string&, const string&);
};

struct STR_INT_OPERATION
{
	int		iParamType;
	string	strParam1;
	int		intParam1;
	CIMProtocol* pProtocol;
	IMProtocolError (CIMProtocol::*pfnHandler)(const string&, int);
};

struct STR_STR_STR_OPERATION
{
	int		iParamType;
	string	strParam1;
	string	strParam2;
	string	strParam3;
	CIMProtocol* pProtocol;
	IMProtocolError (CIMProtocol::*pfnHandler)(const string&, const string&, const string&);
};

struct STR_INT_STR_OPERATION
{
	int		iParamType;
	string	strParam1;
	int		intParam1;
	string	strParam2;
	CIMProtocol6* pProtocol;
	IMProtocolError (CIMProtocol6::*pfnHandler)(const string&, int, const string&);
};

struct STR_EXTENDEDSETTINGS_OPERATION
{
	int		iParamType;
	string	strParam1;
	CProtocolSettings2* pExtendedCapabilitiesParam1;
	CIMProtocol7* pProtocol;
	IMProtocolError (CIMProtocol7::*pfnHandler)(
			const std::string& strUser, CProtocolSettings2* pExtendedCapabilities);
};



#define			TERMINATE_ASYNC_OPERATIONS_SLEEP			500


// Public methods...
//

CPurpleProtocolAsyncExec::CPurpleProtocolAsyncExec() : 
		m_bInitialized(FALSE), 
		m_bIsTerminating(FALSE)
{
	if (m_terminateEvent.Create(NULL, TRUE, FALSE, NULL))
	{
		m_pTerminatingMutex = g_mutex_new();
		
		if (m_pTerminatingMutex)
		{
			m_pOperationListMutex = g_mutex_new();
		}
	}
	
	m_bInitialized = (m_pOperationListMutex != NULL);
}


CPurpleProtocolAsyncExec::~CPurpleProtocolAsyncExec()
{
	if (m_terminateEvent)
	{
		m_terminateEvent.Close();
	}
	
	if (m_pTerminatingMutex)
	{
		g_mutex_free(m_pTerminatingMutex);
		m_pTerminatingMutex = NULL;
	}
	
	if (m_pOperationListMutex)
	{
		g_mutex_free(m_pOperationListMutex);
		m_pOperationListMutex = NULL;
	}
}


void CPurpleProtocolAsyncExec::StartAsyncOperations()
{
	if (!m_bInitialized)
	{
		return;
	}
	
	while(true)
	{
		DUMMY_OPERATION* pDummyOperation = GetAsyncOperation();
		
		if (pDummyOperation)
		{
			if (!CPurpleService::GetInstance()->FinalizeInProgress())
			{
				if (pDummyOperation->iParamType == AOT_DISCONNECT)
				{
					STR_OPERATION* pAsyncOperation = 
							(STR_OPERATION*)pDummyOperation;
					
					if (pAsyncOperation)
					{
						((pAsyncOperation->pProtocol)->*(pAsyncOperation->pfnHandler))(
								pAsyncOperation->strParam1);
					}
				}
				else if (pDummyOperation->iParamType == AOT_SET_ALIAS)
				{
					STR_STR_OPERATION* pAsyncOperation = 
							(STR_STR_OPERATION*)pDummyOperation;
					
					if (pAsyncOperation)
					{
						((pAsyncOperation->pProtocol)->*(pAsyncOperation->pfnHandler))(
								pAsyncOperation->strParam1, pAsyncOperation->strParam2);
					}
				}
				else if (pDummyOperation->iParamType == AOT_SET_USER_DISPLAY_ICON)
				{
					STR_STR_OPERATION* pAsyncOperation = 
							(STR_STR_OPERATION*)pDummyOperation;
					
					if (pAsyncOperation)
					{
						((pAsyncOperation->pProtocol)->*(pAsyncOperation->pfnHandler))(
								pAsyncOperation->strParam1, pAsyncOperation->strParam2);
					}
				}
				else if (pDummyOperation->iParamType == AOT_SET_USER_STATUS)
				{
					STR_INT_STR_OPERATION* pAsyncOperation = 
							(STR_INT_STR_OPERATION*)pDummyOperation;
					
					if (pAsyncOperation)
					{
						((pAsyncOperation->pProtocol)->*(pAsyncOperation->pfnHandler))(
								pAsyncOperation->strParam1, pAsyncOperation->intParam1, pAsyncOperation->strParam2);
					}
				}
				else if (pDummyOperation->iParamType == AOT_ALIAS_CONTACT)
				{
					STR_STR_STR_OPERATION* pAsyncOperation = 
							(STR_STR_STR_OPERATION*)pDummyOperation;
					
					if (pAsyncOperation)
					{
						((pAsyncOperation->pProtocol)->*(pAsyncOperation->pfnHandler))(
								pAsyncOperation->strParam1, pAsyncOperation->strParam2, 
								pAsyncOperation->strParam3);
					}
				}
				else if (pDummyOperation->iParamType == AOT_SET_EXTENDED_CAPABILITIES)
				{
					STR_EXTENDEDSETTINGS_OPERATION* pAsyncOperation = 
							(STR_EXTENDEDSETTINGS_OPERATION*)pDummyOperation;
					
					if (pAsyncOperation)
					{
						((pAsyncOperation->pProtocol)->*(pAsyncOperation->pfnHandler))(
								pAsyncOperation->strParam1, 
								pAsyncOperation->pExtendedCapabilitiesParam1);
						
						delete pAsyncOperation->pExtendedCapabilitiesParam1;
					}
				}
			}
			
			delete pDummyOperation;
		}
		/*
		else if (IsFinishingAsyncOperations()) // Leave as requested and whether is nothing else to do...
		{
			break;
		}
		*/
		else
		{
			::WaitForSingleObject(m_terminateEvent, TERMINATE_ASYNC_OPERATIONS_SLEEP);
		}
		
		if (IsFinishingAsyncOperations()) // Leave anyway...
		{
			break;
		}
	}
	
	// We don't need to block access to the list because we are finishing, 
	// and when we are finishing nothing is added to it.
	//
	while (m_asyncExecList.size() > 0)
	{
		DUMMY_OPERATION* pDummyOperation = m_asyncExecList.front();
		
		if (pDummyOperation)
		{
			m_asyncExecList.pop_front();
			
			delete pDummyOperation;
		}
	}
	
	// Is this redundant ?
	m_asyncExecList.clear();
}


void CPurpleProtocolAsyncExec::FinishAsyncOperations()
{
	g_mutex_lock(m_pTerminatingMutex);
	
	m_bIsTerminating = TRUE;
	m_terminateEvent.Set();
	
	g_mutex_unlock(m_pTerminatingMutex);
}


BOOL CPurpleProtocolAsyncExec::IsFinishingAsyncOperations()
{
	BOOL bIsFinishing = FALSE;
	
	if (m_pTerminatingMutex)
	{
		g_mutex_lock(m_pTerminatingMutex);
	}
	
	bIsFinishing = m_bIsTerminating;
	
	if (m_pTerminatingMutex)
	{
		g_mutex_unlock(m_pTerminatingMutex);
	}
	
	return bIsFinishing;
}


IMProtocolError CPurpleProtocolAsyncExec::DisconnectAsync(CIMProtocol* pProtocol, 
		const std::string& strUser)
{
	IMProtocolError imProtocolError = IM_PROTOCOL_ERROR_FAILED;
	
	if (!IsFinishingAsyncOperations())
	{
		g_mutex_lock(m_pOperationListMutex);
		
		STR_OPERATION* pAsyncOperation = new STR_OPERATION;
		
		if (pAsyncOperation)
		{
			pAsyncOperation->iParamType = AOT_DISCONNECT;
			pAsyncOperation->strParam1 = strUser;
			pAsyncOperation->pProtocol = pProtocol;
			pAsyncOperation->pfnHandler = &CIMProtocol::Disconnect;
			
			m_asyncExecList.push_back((DUMMY_OPERATION*)pAsyncOperation);
			
			imProtocolError = IM_PROTOCOL_ERROR_SUCCESS;
		}
		
		g_mutex_unlock(m_pOperationListMutex);
	}
	else
	{
		imProtocolError = IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
	}
	
	return imProtocolError;
}


IMProtocolError CPurpleProtocolAsyncExec::SetAliasAsync(CIMProtocol* pProtocol, 
		const std::string& strUser, const std::string& strAlias)
{
	IMProtocolError imProtocolError = IM_PROTOCOL_ERROR_FAILED;
	
	if (!IsFinishingAsyncOperations())
	{
		g_mutex_lock(m_pOperationListMutex);
		
		STR_STR_OPERATION* pAsyncOperation = new STR_STR_OPERATION;
		
		if (pAsyncOperation)
		{
			pAsyncOperation->iParamType = AOT_SET_ALIAS;
			pAsyncOperation->strParam1 = strUser;
			pAsyncOperation->strParam2 = strAlias;
			pAsyncOperation->pProtocol = pProtocol;
			pAsyncOperation->pfnHandler = &CIMProtocol::SetAlias;
			
			m_asyncExecList.push_back((DUMMY_OPERATION*)pAsyncOperation);
			
			imProtocolError = IM_PROTOCOL_ERROR_SUCCESS;
		}
		
		g_mutex_unlock(m_pOperationListMutex);
	}
	else
	{
		imProtocolError = IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
	}
	
	return imProtocolError;
}


IMProtocolError CPurpleProtocolAsyncExec::SetUserDisplayIconAsync(CIMProtocol* pProtocol, 
		const std::string& strUser, const std::string& strFile)
{
	IMProtocolError imProtocolError = IM_PROTOCOL_ERROR_FAILED;
	
	if (!IsFinishingAsyncOperations())
	{
		g_mutex_lock(m_pOperationListMutex);
		
		STR_STR_OPERATION* pAsyncOperation = new STR_STR_OPERATION;
		
		if (pAsyncOperation)
		{
			pAsyncOperation->iParamType = AOT_SET_USER_DISPLAY_ICON;
			pAsyncOperation->strParam1 = strUser;
			pAsyncOperation->strParam2 = strFile;
			pAsyncOperation->pProtocol = pProtocol;
			pAsyncOperation->pfnHandler = &CIMProtocol::SetUserDisplayIcon;
			
			m_asyncExecList.push_back((DUMMY_OPERATION*)pAsyncOperation);
			
			imProtocolError = IM_PROTOCOL_ERROR_SUCCESS;
		}
		
		g_mutex_unlock(m_pOperationListMutex);
	}
	else
	{
		imProtocolError = IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
	}
	
	return imProtocolError;
}


IMProtocolError CPurpleProtocolAsyncExec::SetUserStatusAsync(CIMProtocol* pProtocol, 
		const std::string& strUser, int nStatus, const std::string& strStatusMessage)
{
	IMProtocolError imProtocolError = IM_PROTOCOL_ERROR_FAILED;
	
	if (!IsFinishingAsyncOperations())
	{
		g_mutex_lock(m_pOperationListMutex);
		
		STR_INT_STR_OPERATION* pAsyncOperation = new STR_INT_STR_OPERATION;
		
		if (pAsyncOperation)
		{
			CIMProtocol6* pProtocol6 = dynamic_cast<CIMProtocol6*>(pProtocol);
			ATLASSERT(pProtocol6);

			pAsyncOperation->iParamType = AOT_SET_USER_STATUS;
			pAsyncOperation->strParam1 = strUser;
			pAsyncOperation->intParam1 = nStatus;
			pAsyncOperation->strParam2 = strStatusMessage;
			pAsyncOperation->pProtocol = pProtocol6;
			pAsyncOperation->pfnHandler = &CIMProtocol6::SetUserStatus;
			
			m_asyncExecList.push_back((DUMMY_OPERATION*)pAsyncOperation);
			
			imProtocolError = IM_PROTOCOL_ERROR_SUCCESS;
		}
		
		g_mutex_unlock(m_pOperationListMutex);
	}
	else
	{
		imProtocolError = IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
	}
	
	return imProtocolError;
}


IMProtocolError CPurpleProtocolAsyncExec::AliasContactAsync(CIMProtocol* pProtocol, 
		const std::string& strUser, const std::string& strContact, const std::string& strAlias)
{
	IMProtocolError imProtocolError = IM_PROTOCOL_ERROR_FAILED;
	
	if (!IsFinishingAsyncOperations())
	{
		g_mutex_lock(m_pOperationListMutex);
		
		STR_STR_STR_OPERATION* pAsyncOperation = new STR_STR_STR_OPERATION;
		
		if (pAsyncOperation)
		{
			pAsyncOperation->iParamType = AOT_ALIAS_CONTACT;
			pAsyncOperation->strParam1 = strUser;
			pAsyncOperation->strParam2 = strContact;
			pAsyncOperation->strParam3 = strAlias;
			pAsyncOperation->pProtocol = pProtocol;
			pAsyncOperation->pfnHandler = &CIMProtocol::AliasContact;
			
			m_asyncExecList.push_back((DUMMY_OPERATION*)pAsyncOperation);
			
			imProtocolError = IM_PROTOCOL_ERROR_SUCCESS;
		}
		
		g_mutex_unlock(m_pOperationListMutex);
	}
	else
	{
		imProtocolError = IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
	}
	
	return imProtocolError;
}


IMProtocolError CPurpleProtocolAsyncExec::SetExtendedCapabilitiesAsync(
		CIMProtocol* pProtocol, const std::string& strUser, 
		CProtocolSettings2* pExtendedCapabilities)
{
	IMProtocolError imProtocolError = IM_PROTOCOL_ERROR_FAILED;
	
	if (!IsFinishingAsyncOperations())
	{
		g_mutex_lock(m_pOperationListMutex);
		
		STR_EXTENDEDSETTINGS_OPERATION* pAsyncOperation = new STR_EXTENDEDSETTINGS_OPERATION;
		
		if (pAsyncOperation)
		{
			CIMProtocol7* pProtocol7 = dynamic_cast<CIMProtocol7*>(pProtocol);
			ATLASSERT(pProtocol7);
			
			pAsyncOperation->iParamType = AOT_SET_EXTENDED_CAPABILITIES;
			pAsyncOperation->strParam1 = strUser;
			
			if (pExtendedCapabilities != NULL)
			{
				pAsyncOperation->pExtendedCapabilitiesParam1 = pExtendedCapabilities->Clone();
			}
			else
			{
				pAsyncOperation->pExtendedCapabilitiesParam1 = NULL;
			}
			
			pAsyncOperation->pProtocol = pProtocol7;
			pAsyncOperation->pfnHandler = &CIMProtocol7::SetExtendedCapabilities;
			
			m_asyncExecList.push_back((DUMMY_OPERATION*)pAsyncOperation);
			
			imProtocolError = IM_PROTOCOL_ERROR_SUCCESS;
		}
		
		g_mutex_unlock(m_pOperationListMutex);
	}
	else
	{
		imProtocolError = IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
	}
	
	return imProtocolError;
}


DUMMY_OPERATION* CPurpleProtocolAsyncExec::GetAsyncOperation()
{
	DUMMY_OPERATION* pDummyOperation = NULL;
	
	g_mutex_lock(m_pOperationListMutex);
	
	if (m_asyncExecList.size())
	{
		pDummyOperation = m_asyncExecList.front();
		
		m_asyncExecList.pop_front();
	}
	
	g_mutex_unlock(m_pOperationListMutex);
	
	return pDummyOperation;
}

