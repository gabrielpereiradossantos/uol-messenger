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
#include "Account.h"

#include "SettingsTraits.h"
#include "../Controller/UIMApplication.h"
#include "../Cryptography.h"


using namespace std;



CAccount::CAccount() :
	m_strUserId(_T("")),
	m_strPassword(_T("")),
	m_strNickName(_T("")),
	m_strStatusMessage(_T("")),
	m_bAutoConnect(TRUE),
	m_pProtocol(NULL),
	m_bActive(TRUE),
	m_bSavePassword(TRUE)
{
	m_pProtocolSettings = new CAccountProtocolSettings();

	InitTagProcessMap();
}


CAccount::~CAccount(void)
{
	ATLTRACE(_T("%s - %s\n"), __FUNCTION__, m_strUserId);
}


void CAccount::SetProtocol(CIMProtocol* pProtocol)
{
	m_pProtocol = pProtocol;
}


void CAccount::SetUserId(const CString& strUserId)
{
	m_strUserId = strUserId;
}


void CAccount::Finalize()
{
	SetActive(FALSE);

	m_pProtocol = NULL;
}


void CAccount::InitTagProcessMap()
{	 
	init_helper ih[] = 
	{	
		{"Protocol", NEW_SETTINGS_CALLBACK(Protocol, this)},
		{"UserId", NEW_SETTINGS_CALLBACK(UserId, this)},
		{"Password", NEW_SETTINGS_CALLBACK(Password, this)},
		{"NickName", NEW_SETTINGS_CALLBACK(NickName, this)},
		{"IconPath", NEW_SETTINGS_CALLBACK(IconPath, this)},
		{"AutoConnect", NEW_SETTINGS_CALLBACK(AutoConnect, this)},
		{"SavePassword", NEW_SETTINGS_CALLBACK(SavePassword, this)},
		{"Advanced", NEW_SETTINGS_CALLBACK(Advanced, this)},
		{"StatusMessage", NEW_SETTINGS_CALLBACK(StatusMessage, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		m_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}


void CAccount::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, m_tagProcess, this);
}


void CAccount::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, m_tagProcess, this);
}


CIMProtocol* CAccount::GetProtocol() const
{
	return m_pProtocol;
}


CString CAccount::GetUserId() const
{
	return m_strUserId;
}


CString CAccount::GetPassword() const
{
	return m_strPassword;
}


void CAccount::SetPassword(const CString& strPassword)
{
	if (0 != strPassword.Compare(m_strPassword))
	{
		m_strPassword = strPassword;

		NotifyObservers(&CUOLMessengerAccountObserver::OnPasswordChanged);
	}
}


CString CAccount::GetNickName() const
{
	if (m_strNickName.IsEmpty())
	{
		return m_strUserId;
	}
	
	return m_strNickName;
}


void CAccount::SetNickName(const CString& strNickName)
{
	if (0 != strNickName.Compare(m_strNickName))
	{
		m_strNickName = strNickName;

		NotifyObservers(&CUOLMessengerAccountObserver::OnNickNameChanged);
	}
}


CString CAccount::GetStatusMessage() const
{
	return m_strStatusMessage;
}


void CAccount::SetStatusMessage(const CString& strStatusMessage)
{
	if (0 != strStatusMessage.Compare(m_strStatusMessage))
	{
		m_strStatusMessage = strStatusMessage;

		NotifyObservers(&CUOLMessengerAccountObserver::OnStatusMessageChanged);
	}
}


BOOL CAccount::IsAutoConnectEnabled() const
{
	return m_bAutoConnect;
}


void CAccount::EnableAutoConnect(BOOL bAutoConnect)
{
	if (m_bAutoConnect != bAutoConnect)
	{
		m_bAutoConnect = bAutoConnect;

		NotifyObservers(&CUOLMessengerAccountObserver::OnAutoConnectChanged);
	}
}

BOOL CAccount::IsSavePasswordEnabled() const
{
	return m_bSavePassword;
}


void CAccount::EnableSavePassword(BOOL bSavePassword)
{
	m_bSavePassword = bSavePassword;

	NotifyObservers(&CUOLMessengerAccountObserver::OnSavePasswordChanged);
}

IUOLMessengerUserIconPtr CAccount::GetUserIcon() const
{
	return m_pUserIcon;
}


void CAccount::SetUserIcon(IUOLMessengerUserIconPtr pUserIcon)
{
	if (pUserIcon.get() != m_pUserIcon.get())
	{
		m_pUserIcon = pUserIcon;

		NotifyObservers(&CUOLMessengerAccountObserver::OnUserIconChanged);
	}
}


CString CAccount::GetProtocolId() const
{
	if (m_pProtocol != NULL)
	{
		return m_pProtocol->GetId().c_str();
	}

	return m_strProtocolId;
}


CString CAccount::GetProtocolName() const
{
	if (m_pProtocol != NULL)
	{
		return m_pProtocol->GetName().c_str();
	}

	return _T("");
}


CProtocolSettingsPtr CAccount::GetProtocolSettings() const
{
	return m_pProtocolSettings;
}


CProtocolSettings2Ptr CAccount::GetExtendedCapabilities() const
{
	return m_pExtendedCapabilities;
}


void CAccount::SetExtendedCapabilities(CProtocolSettings2Ptr pExtendedCapabilities)
{
	m_pExtendedCapabilities = pExtendedCapabilities;
}



void CAccount::RegisterAccountObserver(CUOLMessengerAccountObserver* pObserver)
{
	if (pObserver != NULL)
	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

		POSITION pos = m_listAccountObservers.Find(pObserver);

		if (!pos)
		{
			m_listAccountObservers.AddTail(pObserver);
		}
	}
}


void CAccount::UnregisterAccountObserver(CUOLMessengerAccountObserver* pObserver)
{
	if (pObserver != NULL)
	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

		POSITION pos = m_listAccountObservers.Find(pObserver);

		if (pos)
		{
			m_listAccountObservers.RemoveAt(pos);
		}
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CAccount, Protocol)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->GetProtocolId());
	}
	else
	{
		CString strProtcolId;
		CSettingsTraits::GetStringValue(pNode, strKey, strProtcolId);

		/// <TODAOFERTA>
		// Replace old protocol id 
		if (0 == strProtcolId.Compare(_T("prpl-lagoinha")))
		{
			strProtcolId = _T("prpl-uolto");
		}
		/// </TODAOFERTA>

		//CIMProtocol* pProtocol = CUIMApplication::GetApplication()->GetProtocolManager()->GetProtocol(strProtcolId);
		IUOLMessengerProtocolManager2Ptr pProtocolManager2 = 
				ap_static_cast<IUOLMessengerProtocolManager2Ptr>(CUIMApplication::GetApplication()->GetProtocolManager());
		
		if (pProtocolManager2)
		{
			CIMProtocol* pProtocol = pProtocolManager2->GetProtocol(strProtcolId, m_pThis->GetUserId());
			m_pThis->SetProtocol(pProtocol);
			
			if (NULL == pProtocol)
			{
				m_pThis->m_strProtocolId = strProtcolId;
			}
		}
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CAccount, UserId)
{
	CString strUserId;
	CCryptography cryptography;
	int nStrLength;

	if (bSave)
	{
		strUserId = m_pThis->m_strUserId;
		nStrLength = m_pThis->m_strUserId.GetLength();
		cryptography.Encrypt(strUserId, nStrLength);
		cryptography.EncodeBase64(strUserId, nStrLength);
		CSettingsTraits::SetStringValue(pNode, strKey, strUserId);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, strUserId);
		nStrLength = strUserId.GetLength();
		cryptography.DecodeBase64(strUserId, nStrLength);
		
		if (cryptography.Decrypt(strUserId, nStrLength))
		{
			m_pThis->m_strUserId = strUserId;
		}
		else
		{
			//TODO: Tratar o erro de decodificação.
			m_pThis->m_strUserId = _T("");
		}

	}
}


IMPLEMENT_SETTINGS_CALLBACK(CAccount, Password)
{
	CString strPass;
	CCryptography cryptography;
	int nStrLength;

 	if (bSave)
	{
		if (m_pThis->m_bSavePassword)
		{
			strPass = m_pThis->m_strPassword;
		}
		else
		{
			strPass = _T("");
		}
		
		nStrLength = strPass.GetLength();
		cryptography.Encrypt(strPass, nStrLength);
		cryptography.EncodeBase64(strPass, nStrLength);
		CSettingsTraits::SetStringValue(pNode, strKey, strPass);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, strPass);
		nStrLength = strPass.GetLength();
		cryptography.DecodeBase64(strPass, nStrLength);

		if (cryptography.Decrypt(strPass, nStrLength))
		{
			m_pThis->m_strPassword = strPass;
		}
		else
		{
			//TODO: Tratar o erro de decodificação.
			m_pThis->m_strPassword = _T("");
		}
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CAccount, NickName)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strNickName);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strNickName);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CAccount, StatusMessage)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strStatusMessage);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strStatusMessage);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CAccount, IconPath)
{
	CString strIconPath;

	if (bSave)
	{
		if (m_pThis->m_pUserIcon != NULL)
		{
			strIconPath = m_pThis->m_pUserIcon->GetImagePath();
		}

		CSettingsTraits::SetStringValue(pNode, strKey, strIconPath);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, strIconPath);

		IUOLMessengerUserIconCacheManagerPtr pUserIconCacheManager = CUIMApplication::GetApplication()->GetUserIconCacheManager();

		if ((pUserIconCacheManager != NULL) && (!strIconPath.IsEmpty()))
		{
			IUOLMessengerUserIconPtr pUserIcon = pUserIconCacheManager->RegisterCachedImage(m_pThis, strIconPath);

			if (pUserIcon != NULL)
			{
				m_pThis->m_pUserIcon = pUserIcon;
			}
		}
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CAccount, AutoConnect)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bAutoConnect);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bAutoConnect);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CAccount, SavePassword)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bSavePassword);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bSavePassword);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CAccount, Advanced)
{
	CAccountProtocolSettingsPtr pProtocolSettings;

	if (bSave)
	{
		MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, strKey);
		
		CAccountProtocolSettingsPtr pProtocolSettings = m_pThis->m_pProtocolSettings;

		if (pProtocolSettings)
		{
			pProtocolSettings->Save(pChild, strKey);
		}
	}
	else
	{
		(m_pThis->m_pProtocolSettings)->Load(pNode, strKey);
	}
}


void CAccount::SetActive(BOOL bActive)
{	
	::InterlockedExchange(&m_bActive, bActive);
}


BOOL CAccount::IsActive() const
{
	return m_bActive;
}


void CAccount::NotifyObservers(void (CUOLMessengerAccountObserver::*pfnCallback)(IUOLMessengerAccount*))
{
	if (IsActive())
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

		for (POSITION pos = m_listAccountObservers.GetHeadPosition(); pos; )
		{
			CUOLMessengerAccountObserver* pObserver = m_listAccountObservers.GetNext(pos);

			(pObserver->*pfnCallback)(this);
		}
	}
}
