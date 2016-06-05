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
#include "SoundSettings.h"

#include "../resource.h"
#include "SettingsTraits.h"
#include <MinDependencySpecialFolder.h>
#include "../controller/UIMApplication.h"
#include "../controller/SettingsManager.h"


DWORD CSoundSettings::VERSION_ATTRIBUTE_DEFAULT_VALUE = 1;


CSoundSettings::CSoundSettings() :
m_strContactOnlineSound(_T("")), 
m_strNewMessageSound(_T("")),
m_userSettingsHandler(_T(""))
{
	m_settingEventSource.SetEventSource(this);

	// Initialize sounds settings default directory path
	CMinDependencySpecialFolder		minDSF;
	CString	strCompany;
	CString	strProduct;

	strCompany.LoadString(IDS_STRING_COMPANY_NAME);
	strProduct.LoadString(IDS_STRING_PRODUCT_NAME);

	m_strDefaultDirPath.Format(_T("%s%s\\%s"), 
			minDSF.GetCommonAppData(), 
			strCompany, 
			strProduct);

	// Initialize settings processing map
	InitTagProcessMap();

	m_userSettingsHandler.AddSetting("Sounds", this);
}


CSoundSettings::~CSoundSettings()
{
}


void CSoundSettings::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Sound", NEW_SETTINGS_CALLBACK(Sound, this)},
		{"SoundDefaultDirPath", NEW_SETTINGS_CALLBACK(SoundDefaultDirPath, this)},
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}


void CSoundSettings::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	m_mapSoundEvents.RemoveAll();

	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}


void CSoundSettings::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}


CString CSoundSettings::GetNewMessageSound() const
{
	return GetFilePath(IUOLMessengerSoundSettings2::NEW_MESSAGE_SOUND_EVENT);
}


void CSoundSettings::SetNewMessageSound(const CString& strNewMessageSound)
{
	ChangeSoundEventFile(IUOLMessengerSoundSettings2::NEW_MESSAGE_SOUND_EVENT, strNewMessageSound);
}


CString CSoundSettings::GetOnlineSound() const
{
	return GetFilePath(IUOLMessengerSoundSettings2::CONTACT_ONLINE_SOUND_EVENT);
}


void CSoundSettings::SetOnlineSound(const CString& strOnlineSound)
{
	ChangeSoundEventFile(IUOLMessengerSoundSettings2::CONTACT_ONLINE_SOUND_EVENT, strOnlineSound);
}


void CSoundSettings::AddEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.AddEventListener(pListener);
}


void CSoundSettings::RemoveEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.RemoveEventListener(pListener);
}


void CSoundSettings::NotifyEventListeners(UINT nSettingId)
{
	m_settingEventSource.NotifyEventListeners(nSettingId);
}


BOOL CSoundSettings::LoadSettings()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bLoaded = FALSE;

	BuildUserSettingsPaths();

	m_userSettingsHandler.Init(m_strSoundsFilePath, FALSE, FALSE);

	// Try loading user settings
	if (ATLPath::FileExists(m_strSoundsFilePath))
	{
		bLoaded = m_userSettingsHandler.LoadSettings(FALSE);
	}

	// If user settings loading failed, load default settings
	if (FALSE == bLoaded)
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, "First loading try failed.");
		
		// Create directories if necessary
		BOOL bPathExists = TRUE;

		CString strDir;
		int nFind = m_strSoundsFilePath.Find('\\');
		while (-1 != nFind)
		{
			strDir = m_strSoundsFilePath.Left(nFind);
			bPathExists = ::PathFileExists(strDir);
			
			if (!bPathExists)
			{
				bPathExists = ::CreateDirectory(strDir, NULL);
			}

			nFind = m_strSoundsFilePath.Find('\\', nFind+1);
		}

		::CopyFile(GetDefaultSettingsFilePath(), m_strSoundsFilePath, TRUE);

		bLoaded = m_userSettingsHandler.LoadSettings(TRUE);
		
		if (!bLoaded)
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogDebug(__FUNCTION__, "Second loading try failed.");
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, "Sound settings loaded on first try.");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bLoaded = [%d].", bLoaded);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bLoaded;
}


BOOL CSoundSettings::LoadDefaultSettings()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bLoaded = FALSE;

	m_strSoundsFilePath = GetDefaultSettingsFilePath();

	m_userSettingsHandler.Init(m_strSoundsFilePath, FALSE, FALSE);

	bLoaded = m_userSettingsHandler.LoadSettings(FALSE);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bLoaded = [%d].", bLoaded);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bLoaded;
}


BOOL CSoundSettings::SaveSettings()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bSaveOk = FALSE;
	
	if (m_mapSoundEvents.IsEmpty())
	{
		::DeleteFile(m_strSoundsFilePath);
		
		bSaveOk = TRUE;
	}
	else
	{
		bSaveOk = m_userSettingsHandler.SaveSettings();
	}
	
	if (bSaveOk)
	{
		CSettingsManagerPtr pSettingsManager = ap_dynamic_cast<CSettingsManagerPtr>(CUIMApplication::GetApplication()->GetSettingsManager());
		
		if (pSettingsManager)
		{
			pSettingsManager->NotifySoundSettingsChanged();
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"pSettingsManager == NULL.");
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bSaveOk = [%d].", bSaveOk);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return bSaveOk;
}


CString CSoundSettings::GetDefaultDirPath()
{
	return m_strDefaultDirPath;
}


CString CSoundSettings::GetUserConfigFilePath(const CString& strUserProfile)
{
	CString strSoundsFile;
	strSoundsFile.LoadString(IDS_FILE_SOUNDS_SETTINGS);

	CString strSoundsFilePath = GetUserPath(strUserProfile, strSoundsFile);

	return strSoundsFilePath;
}


CString CSoundSettings::GetUserSoundsDirPath(const CString& strUserProfile)
{
	CString strSoundsFolder;
	strSoundsFolder.LoadString(IDS_DIR_SOUNDS);

	CString strSoundsFolderPath = GetUserPath(strUserProfile, strSoundsFolder);

	return strSoundsFolderPath;
}


void CSoundSettings::GetSoundEventList(CAtlList<IUOLMessengerSoundEventPtr>& listSoundEvents)
{
	for (POSITION pos = m_mapSoundEvents.GetHeadPosition(); pos;)
	{
		CSoundEventMap::CPair* pPair = m_mapSoundEvents.GetNext(pos);

		listSoundEvents.AddTail(pPair->m_value);
	}
}


IUOLMessengerSoundEventPtr CSoundSettings::GetSoundEvent(const CString& strEventType)
{
	CSoundEventMap::CPair* pPair = m_mapSoundEvents.Lookup(strEventType);

	if (pPair != NULL)
	{
		return pPair->m_value;
	}

	return IUOLMessengerSoundEventPtr();
}


IUOLMessengerSoundEventPtr CSoundSettings::AddSoundEvent(const CString& strEventType, const CString& strDescription, const CString& strFilePath, BOOL bEnabled)
{
	CSoundEventPtr pSoundEvent;
	
	if ((FALSE == strEventType.IsEmpty()) &&
		(FALSE == strFilePath.IsEmpty()))
	{
		// Remove old sound event
		CSoundEventMap::CPair* pPair = m_mapSoundEvents.Lookup(strEventType);

		if (pPair)
		{
			m_mapSoundEvents.RemoveKey(strEventType);
		}

		CString strNewFilePath;

		// Use relative path if sound file path is under default dir path
		CString strDefaultDirPath = GetDefaultDirPath();
		
		int nFind = strFilePath.Find(strDefaultDirPath);

		if (0 == nFind)
		{
			strNewFilePath = strFilePath.Mid(strDefaultDirPath.GetLength() + 1);
		}
		else
		{
			CPath path(strFilePath);
			path.StripPath();

			if (FALSE == ATLPath::FileExists(m_strSoundsFolderPath))
			{
				::CreateDirectory(m_strSoundsFolderPath, NULL);
			}

			CString strCopyFilePath = m_strSoundsFolderPath + _T("\\") + path.m_strPath;

			::CopyFile(strFilePath, strCopyFilePath, FALSE);

			strNewFilePath = strCopyFilePath;
		}

		pSoundEvent = new CSoundEvent(strEventType, strDescription, strNewFilePath, bEnabled);

		// Add new sound event
		m_mapSoundEvents.SetAt(pSoundEvent->GetEventType(), pSoundEvent);
	}

	return pSoundEvent;
}


IUOLMessengerSoundEventPtr CSoundSettings::RemoveSoundEvent(const CString& strEventType)
{
	CSoundEventPtr pSoundEvent;

	CSoundEventMap::CPair* pPair = m_mapSoundEvents.Lookup(strEventType);

	if (pPair)
	{
		pSoundEvent = pPair->m_value;
		
		m_mapSoundEvents.RemoveKey(strEventType);
	}

	return pSoundEvent;
}


BOOL CSoundSettings::ChangeSoundEventFile(const CString& strEventType, const CString& strFilePath)
{
	IUOLMessengerSoundEventPtr pSoundEvent = GetSoundEvent(strEventType);

	if (pSoundEvent)
	{
		CString strNewFilePath;

		CString strDefaultDirPath = GetDefaultDirPath();
			
		int nFind = strFilePath.Find(strDefaultDirPath);

		if (0 == nFind)
		{
			// File is under default sound dir store relative path
			strNewFilePath = strFilePath.Mid(strDefaultDirPath.GetLength() + 1);
		}
		else
		{
			// File is not under default sound dir, store absolute path
			CPath path(strFilePath);
			path.StripPath();

			if (FALSE == ATLPath::FileExists(m_strSoundsFolderPath))
			{
				::CreateDirectory(m_strSoundsFolderPath, NULL);
			}

			CString strCopyFilePath = m_strSoundsFolderPath + _T("\\") + path.m_strPath;

			::CopyFile(strFilePath, strCopyFilePath, FALSE);

			strNewFilePath = strCopyFilePath;
		}

		pSoundEvent->SetFilePath(strNewFilePath);

		return TRUE;
	}

	return FALSE;
}


BOOL CSoundSettings::EnableSoundEvent(const CString& strEventType, BOOL bEnable)
{
	IUOLMessengerSoundEventPtr pSoundEvent = GetSoundEvent(strEventType);

	if (pSoundEvent)
	{
		pSoundEvent->SetEnabled(bEnable);

		return TRUE;
	}

	return FALSE;
}


BOOL CSoundSettings::GetSoundEventFilePath(const CString& strEventType, CString& strFilePath, BOOL bFullPath)
{
	IUOLMessengerSoundEventPtr pSoundEvent = GetSoundEvent(strEventType);

	if (pSoundEvent)
	{
		strFilePath = pSoundEvent->GetFilePath();

		if (ATLPath::IsRelative(strFilePath) &&
			bFullPath)
		{
			strFilePath = GetDefaultDirPath() + _T("\\") + strFilePath;
		}

		return TRUE;
	}

	return FALSE;
}


CString CSoundSettings::GetNudgeReceivedSound() const
{
	return GetFilePath(IUOLMessengerSoundSettings3::NUDGE_RECEIVED_SOUND_EVENT);
}


void CSoundSettings::SetNudgeReceivedSound(const CString& strNudgeReceivedSound)
{
	ChangeSoundEventFile(IUOLMessengerSoundSettings3::NUDGE_RECEIVED_SOUND_EVENT, strNudgeReceivedSound);
}


CString CSoundSettings::GetFileTransferAlertSound() const
{
	return GetFilePath(IUOLMessengerSoundSettings3::FILE_TRANSFER_ALERT_SOUND_EVENT);
}


void CSoundSettings::SetFileTransferAlertSound(const CString& strFileTransferAlertSound)
{
	ChangeSoundEventFile(IUOLMessengerSoundSettings3::FILE_TRANSFER_ALERT_SOUND_EVENT, strFileTransferAlertSound);
}


CString CSoundSettings::GetFileTransferFinishedSound() const
{
	return GetFilePath(IUOLMessengerSoundSettings3::FILE_TRANSFER_FINISHED_SOUND_EVENT);
}


void CSoundSettings::SetFileTransferFinishedSound(const CString& strFileTransferFinishedSound)
{
	ChangeSoundEventFile(IUOLMessengerSoundSettings3::FILE_TRANSFER_FINISHED_SOUND_EVENT, strFileTransferFinishedSound);
}


IMPLEMENT_SETTINGS_CALLBACK(CSoundSettings, Version)
{
	if (bSave)
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_dwVersion);
	}
	else
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_dwVersion);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CSoundSettings, Sound)
{
	CSoundEventPtr pSoundEvent;

	if (bSave)
	{
		for (POSITION pos = m_pThis->m_mapSoundEvents.GetHeadPosition(); pos;)
		{
			CSoundEventMap::CPair* pPair = m_pThis->m_mapSoundEvents.GetNext(pos);

			pSoundEvent = pPair->m_value;

			MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, strKey);
			pSoundEvent->Save(pChild, strKey);
		}
	}
	else
	{
		pSoundEvent = new CSoundEvent();

		if (pSoundEvent)
		{
			pSoundEvent->Load(pNode, strKey);

			m_pThis->m_mapSoundEvents.SetAt(pSoundEvent->GetEventType(), pSoundEvent);
		}
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CSoundSettings, SoundDefaultDirPath)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strDefaultDirPath);
	}
	else
	{
		CString strDefaultDirPath;

		CSettingsTraits::GetStringValue(pNode, strKey, strDefaultDirPath);

		if (FALSE == strDefaultDirPath.IsEmpty())
		{
			m_pThis->m_strDefaultDirPath = strDefaultDirPath;
		}
	}
}



void CSoundSettings::BuildUserSettingsPaths()
{
	m_strSoundsFolderPath.Empty();
	m_strSoundsFilePath.Empty();

	IUOLMessengerProfileManagerPtr pProfileManager = CUIMApplication::GetApplication()->GetProfileManager();

	if (pProfileManager != NULL)
	{
		CString strCurrentProfile = pProfileManager->GetCurrentProfile();
	
		CString strSoundsFolder;
		strSoundsFolder.LoadString(IDS_DIR_SOUNDS);

		m_strSoundsFolderPath = GetUserPath(strCurrentProfile, strSoundsFolder);

		CString strSoundsFile;
		strSoundsFile.LoadString(IDS_FILE_SOUNDS_SETTINGS);

		m_strSoundsFilePath = GetUserPath(strCurrentProfile, strSoundsFile);
	}
}


CString CSoundSettings::GetUserPath(const CString& strUserProfile, const CString& strRelativePath)
{
	CString strFilePath;

	IUOLMessengerProfileManagerPtr pProfileManager = CUIMApplication::GetApplication()->GetProfileManager();

	if (pProfileManager != NULL)
	{
		IUOLMessengerProfileElementPtr pProfileElement;

		pProfileManager->GetProfileElement(strUserProfile, pProfileElement);
	
		CString strProfilePathWithBackSlash = pProfileElement->GetProfilePath();

		if (strProfilePathWithBackSlash.Right(1) != '\\')
		{
			strProfilePathWithBackSlash += _T("\\");
		}

		strFilePath = strProfilePathWithBackSlash + strRelativePath;
	}

	return strFilePath;
}


CString CSoundSettings::GetDefaultSettingsFilePath()
{
	CString strSoundsFile;
	strSoundsFile.LoadString(IDS_FILE_SOUNDS_SETTINGS);

	CString strDefaultSettingsFilePath = GetDefaultDirPath() + _T("\\") + strSoundsFile;

	return strDefaultSettingsFilePath;
}


CString CSoundSettings::GetFilePath(const CString& strEventType) const
{
	IUOLMessengerSoundEventPtr pSoundEvent = ((CSoundSettings*) this)->GetSoundEvent(strEventType);

	if (pSoundEvent)
	{
		return pSoundEvent->GetFilePath();
	}

	return _T("");
}


void CSoundSettings::RegisterObserver(CUOLMessengerSoundSettingsObserver* pObserver)
{
	if (pObserver != NULL)
	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

		POSITION pos = m_listObservers.Find(pObserver);

		if (!pos)
		{
			m_listObservers.AddTail(pObserver);
		}
	}
}


void CSoundSettings::UnregisterObserver(CUOLMessengerSoundSettingsObserver* pObserver)
{
	if (pObserver != NULL)
	{	
		CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

		POSITION pos = m_listObservers.Find(pObserver);

		if (pos)
		{
			m_listObservers.RemoveAt(pos);
		}
	}
}


void CSoundSettings::NotifyObservers(void (CUOLMessengerSoundSettingsObserver::*pfnCallback)(IUOLMessengerSoundSettings3Ptr), IUOLMessengerSoundSettings3Ptr pSoundSettings)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos; )
	{
		CUOLMessengerSoundSettingsObserver* pObserver = m_listObservers.GetNext(pos);

		(pObserver->*pfnCallback)(pSoundSettings);
	}

}