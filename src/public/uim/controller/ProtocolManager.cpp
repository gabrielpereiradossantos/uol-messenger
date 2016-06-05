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
#include "../resource.h"
#include ".\protocolmanager.h"
#include ".\UIMApplication.h"

#include <MinDependencySpecialFolder.h>


using namespace std;


CProtocolManager::CProtocolManager(void) :
	m_pProtocolFactory(NULL),
	m_pProtocolStack(NULL)
{
}

CProtocolManager::~CProtocolManager(void)
{
}


void CProtocolManager::GetProtocolList(CConstAtlListIterator< CIMProtocol* >& itProtocols)
{
	itProtocols = CConstAtlListIterator< CIMProtocol* >(&m_listProtocols);
}


BOOL CProtocolManager::Init()
{
	BOOL bRet = FALSE;

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_pProtocolFactory = CIMProtocolFactory::GetInstance();
    
	if (m_pProtocolFactory != NULL)
	{
		// Get plugins serach path
		char* ppszPluginsSearchPath[1];

		ppszPluginsSearchPath[0] = new char [MAX_PATH + 1];

		char* pszModulePath;
		pszModulePath = new char [MAX_PATH + 1];

		HMODULE hModule = _Module.GetModuleInstance();
		::GetModuleFileName(hModule, pszModulePath, MAX_PATH);

		char* pszAux;
		char* pszFileName = NULL;
		for (pszAux = pszModulePath; (*pszAux) != '\0'; pszAux++)
		{
			if ((*pszAux) == '\\')
			{
				pszFileName = pszAux + 1;
			}
		}
#ifdef _DEBUG
		int nSize = (int) strlen("plugins\\debug");
		strncpy(pszFileName, "plugins\\debug", nSize);
#else
		int nSize = (int) strlen("plugins");
		strncpy(pszFileName, "plugins", nSize);
#endif
		pszModulePath[pszFileName - pszModulePath + nSize] = '\0';

		strcpy(ppszPluginsSearchPath[0], pszModulePath);
		delete pszModulePath;

		CString strCurrentProfileFullPath = GetCurrentProfileFullPath();

		// Init factory
		bRet = m_pProtocolFactory->Init(ppszPluginsSearchPath, 
										sizeof(ppszPluginsSearchPath) / sizeof(ppszPluginsSearchPath[0]), 
										&m_pProtocolStack, 
										NULL, strCurrentProfileFullPath.GetBuffer(), 
										CUIMApplication::GetApplication()->GetApplicationThreadId());

		delete ppszPluginsSearchPath[0];

		// Fill protocols list
		if (bRet)
		{
			CIMProtocolList listProtocols;
			CIMProtocolList::iterator itProtocols;

			m_pProtocolFactory->GetProtocols(listProtocols);

			string strId;
			CIMProtocol* pProtocol;

			for (itProtocols = listProtocols.begin(); 
				 itProtocols != listProtocols.end(); 
				 itProtocols++)
			{
				pProtocol = *itProtocols;

				m_listProtocols.AddTail(pProtocol);
			}
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "m_pProtocolFactory == NULL");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bRet = [%d].", bRet);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}


void CProtocolManager::Finalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pProtocolFactory = [%x].", m_pProtocolFactory);
	
	if (m_pProtocolFactory != NULL)
	{
		m_pProtocolFactory->Finalize();
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


void CProtocolManager::PreFinalize()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pProtocolFactory = [%x].", m_pProtocolFactory);
	
    if (m_pProtocolFactory != NULL)
	{
		m_pProtocolFactory->PreFinalize();
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}


CIMProtocol* CProtocolManager::GetProtocol(const CString& strProtocolId)
{
	CIMProtocol* pProtocol;

	for (POSITION pos = m_listProtocols.GetHeadPosition(); pos;)
	{
		pProtocol = m_listProtocols.GetNext(pos);

		if (0 == strProtocolId.Compare(pProtocol->GetId().c_str()))
		{
			return pProtocol;
		}
	}

	return NULL;
}


BOOL CProtocolManager::AddProtocolToStack(const CString& strProtocolId, CIMProtocol* pProtocol, CIMProtocolCallback* pCallback)
{
	m_pProtocolStack->AddProtocolToStack(strProtocolId, pProtocol, pCallback);

	return TRUE;
}

BOOL CProtocolManager::RemoveProtocolFromStack(const CString& strProtocolId, CIMProtocol* pProtocol, CIMProtocolCallback* pCallback)
{
	m_pProtocolStack->RemoveProtocolFromStack(strProtocolId, pProtocol, pCallback);

	return TRUE;
}


CIMProtocol* CProtocolManager::GetProtocol(const CString& strProtocolId, const CString& strUserId)
{
	CString strProtocolIdToFind = strProtocolId;
	
	if (strProtocolId.CompareNoCase("prpl-oscar") == 0)
	{
		if (isdigit(strUserId[0]))
		{
			strProtocolIdToFind = "prpl-icq";
		}
		else
		{
			strProtocolIdToFind = "prpl-aim";
		}
	}
	
	return GetProtocol(strProtocolIdToFind);
}


CString CProtocolManager::GetProtocolStrId(ProtocolId nProtocolId)
{
	if (nProtocolId == IUOLMessengerProtocolManager3::PROTOCOL_ID_ICQ)
	{
		return _T("prpl-icq");
	}
	else if (nProtocolId == IUOLMessengerProtocolManager3::PROTOCOL_ID_JABBER)
	{
		return _T("prpl-jabber");
	}
	else if (nProtocolId == IUOLMessengerProtocolManager3::PROTOCOL_ID_MSN)
	{
		return _T("prpl-msn");
	}
	else if (nProtocolId == IUOLMessengerProtocolManager3::PROTOCOL_ID_UOL)
	{
		return _T("prpl-uol");
	}
	else if (nProtocolId == IUOLMessengerProtocolManager3::PROTOCOL_ID_TODAOFERTA)
	{
		return _T("prpl-uolto");
	}
	else if (nProtocolId == IUOLMessengerProtocolManager3::PROTOCOL_ID_YAHOO)
	{
		return _T("prpl-yahoo");
	}
	
	return "";
}


void CProtocolManager::StopProtocolService()
{
	m_pProtocolFactory->StopProtocolService();
}


CString CProtocolManager::GetCurrentProfileFullPath()
{
	CString strCurrentProfile;
	CString strCurrentProfileFullPath;
	
	IUOLMessengerProfileManagerPtr pProfileManager = 
			CUIMApplication::GetApplication()->GetProfileManager();
	
	if (pProfileManager)
	{
		strCurrentProfile = pProfileManager->GetCurrentProfile();
	}
	
	if (!strCurrentProfile.IsEmpty())
	{
		CMinDependencySpecialFolder minDSF;
		CString	strCompany;
		CString	strProduct;
		CString	strProfiles;
		
		strCompany.LoadString(IDS_STRING_COMPANY_NAME);
		strProduct.LoadString(IDS_STRING_PRODUCT_NAME);
		strProfiles.LoadString(IDS_STRING_PROFILE_DIR);
		
		strCurrentProfileFullPath.Format(_T("%s%s\\%s\\%s\\%s"), 
				minDSF.GetAppData(), 
				strCompany, 
				strProduct,
				strProfiles,
				strCurrentProfile);
		
		return strCurrentProfileFullPath;
	}
	
	return "";
}

