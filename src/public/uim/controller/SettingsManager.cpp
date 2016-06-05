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
#include "SettingsManager.h"

#include "../model/EmoticonSettings.h"
#include "../model/SoundSettings.h"
#include "../model/SoundSettingsMerge.h"
#include "../model/EmoticonSettingsUpdater.h"
#include "./UIMApplication.h"



CSettingsManager::CSettingsManager(IUOLMessengerProfileManagerPtr pProfileManager)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogInfo(__FUNCTION__, "Merging sound settings...");
	
	//TODO: achar um melhor local para isto
	CSoundSettingsMerge* mergeSoundSettings;
	mergeSoundSettings = new CSoundSettingsMerge();
	mergeSoundSettings->Merge();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "Sound settings merged...");
	
	CString strPath;
	IUOLMessengerProfileElementPtr pProfileElement;

	pProfileManager->GetProfileElement(pProfileManager->GetCurrentProfile(), pProfileElement);
	strPath = pProfileElement->GetProfilePath();
	
	m_pAppSettings = new CUIMSettings(strPath);
	m_pSoundSettings = new CSoundSettings();
	m_pEmoticonSettings = new CEmoticonSettings();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"m_pAppSettings = [%x], m_pSoundSettings = [%x], m_pEmoticonSettings = [%x] and strPath = [%s].", 
			m_pAppSettings.get(), m_pSoundSettings.get(), m_pEmoticonSettings.get(), strPath);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

CSettingsManager::~CSettingsManager()
{
}

void CSettingsManager::NotifyContactListSettingsChanged()
{
	m_observerNotifier.NotifyAll(&CUOLMessengerSettingsManagerObserver::OnContactListSettingsChanged);
}

void CSettingsManager::NotifyGeneralSettingsChanged()
{
	m_observerNotifier.NotifyAll(&CUOLMessengerSettingsManagerObserver::OnGeneralSettingsChanged);
}

void CSettingsManager::NotifyMessageSettingsChanged()
{
	m_observerNotifier.NotifyAll(&CUOLMessengerSettingsManagerObserver::OnMessageSettingsChanged);
}

void CSettingsManager::NotifyStatusSettingsChanged()
{
	m_observerNotifier.NotifyAll(&CUOLMessengerSettingsManagerObserver::OnStatusSettingsChanged);
}

void CSettingsManager::NotifyNetworkSettingsChanged()
{
	m_observerNotifier.NotifyAll(&CUOLMessengerSettingsManagerObserver::OnNetworkSettingsChanged);
}

void CSettingsManager::NotifySoundSettingsChanged()
{
	m_observerNotifier.NotifyAll(&CUOLMessengerSettingsManagerObserver::OnSoundSettingsChanged);
}

void CSettingsManager::NotifyEmoticonSettingsChanged()
{
	m_observerNotifier.NotifyAll(&CUOLMessengerSettingsManagerObserver::OnEmoticonSettingsChanged);
}

BOOL CSettingsManager::LoadSettings()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogInfo(__FUNCTION__, "Updating emoticon settings...");
	
	// Before loading, let's update it.
	//
	CEmoticonSettingsUpdaterPtr pEmoticonsUpdater = new CEmoticonSettingsUpdater();
	pEmoticonsUpdater->UpdatePersonalSettings();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "Emoticon settings updated...");
	
	BOOL bLoad = m_pAppSettings->Load();

	if (bLoad)
	{
		bLoad = m_pEmoticonSettings->LoadSettings();

		if (bLoad)
		{
			bLoad = m_pSoundSettings->LoadSettings();
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bLoad = [%d].", bLoad);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bLoad;
}

BOOL CSettingsManager::SaveSettings()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bSave = m_pAppSettings->Save();

	if (bSave)
	{
		bSave = m_pEmoticonSettings->SaveSettings();

		if (bSave)
		{
			bSave = m_pSoundSettings->SaveSettings();
		}
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bSave = [%d].", bSave);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bSave;
}

IUOLMessengerContactListSettingsPtr CSettingsManager::GetContactListSettings() const
{
	return m_pAppSettings->GetContactListSettings();
}

IUOLMessengerGeneralSettingsPtr CSettingsManager::GetGeneralSettings() const
{
	return m_pAppSettings->GetGeneralSettings();
}

IUOLMessengerMessageSettingsPtr CSettingsManager::GetMessageSettings() const
{
	return m_pAppSettings->GetMessageSettings();
}

IUOLMessengerStatusSettingsPtr CSettingsManager::GetStatusSettings() const
{
	return m_pAppSettings->GetStatusSettings();
}

IUOLMessengerNetworkSettingsPtr CSettingsManager::GetNetworkSettings() const
{
	return m_pAppSettings->GetNetworkSettings();
}

BOOL CSettingsManager::SetValue(const CString& strKey, const CString& strValue, BOOL bCanReplace)
{
	return m_pAppSettings->SetValue(strKey, strValue, bCanReplace);
}

BOOL CSettingsManager::GetValue(const CString& strKey, CString& strValue) const
{
	return m_pAppSettings->GetValue(strKey, strValue);
}

IUOLMessengerSoundSettings2Ptr CSettingsManager::GetSoundSettings() const
{
	return m_pSoundSettings;
}

IUOLMessengerEmoticonSettings2Ptr CSettingsManager::GetEmoticonSettings() const
{
	return m_pEmoticonSettings;
}

void CSettingsManager::RegisterObserver(CUOLMessengerSettingsManagerObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CSettingsManager::UnregisterObserver(CUOLMessengerSettingsManagerObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


