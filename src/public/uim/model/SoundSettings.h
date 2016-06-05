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
 
#pragma once

#include <interfaces/IUOLMessengerSoundSettings.h>
#include <interfaces/IUOLMessengerSoundEvent.h>

#include "ISettings.h"
#include "SettingsHandler.h"
#include "SoundEvent.h"
#include "../controller/SettingsEventSource.h"


class CSoundSettings : public IUOLMessengerSoundSettings3,
					   public ISettings
{
public:
	CSoundSettings();
	virtual ~CSoundSettings();

	virtual void InitTagProcessMap();
	virtual void Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);
	virtual void Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);

	// IUOLMessengerSoundSettings interface
	virtual CString GetNewMessageSound() const;
	virtual void SetNewMessageSound(const CString& strNewMessageSound);

	virtual CString GetOnlineSound() const;
	virtual void SetOnlineSound(const CString& strOnlineSound);

	void AddEventListener(IUOLMessengerSettingsEventListener* pListener);
	void RemoveEventListener(IUOLMessengerSettingsEventListener* pListener);
	void NotifyEventListeners(UINT nSettingId);

	// IUOLMessengerSoundSettings2 interface
	virtual BOOL LoadSettings();
	virtual BOOL SaveSettings();

	virtual CString GetDefaultDirPath();
	virtual CString GetUserConfigFilePath(const CString& strUserProfile);
	virtual CString GetUserSoundsDirPath(const CString& strUserProfile);

	virtual void GetSoundEventList(CAtlList<IUOLMessengerSoundEventPtr>& listSoundEvents);
	
	virtual IUOLMessengerSoundEventPtr GetSoundEvent(const CString& strEventType);
	virtual IUOLMessengerSoundEventPtr AddSoundEvent(const CString& strEventType, const CString& strDescription, const CString& strFilePath, BOOL bEnabled);
	virtual IUOLMessengerSoundEventPtr RemoveSoundEvent(const CString& strEventType);

	virtual BOOL ChangeSoundEventFile(const CString& strEventType, const CString& strFilePath);
	virtual BOOL EnableSoundEvent(const CString& strEventType, BOOL bEnable);
	
	virtual BOOL GetSoundEventFilePath(const CString& strEventType, CString& strFilePath, BOOL bFullPath);

	// IUOLMessengerSoundSettings3 interface
	virtual BOOL LoadDefaultSettings();
	
	virtual CString GetNudgeReceivedSound() const;
	virtual void SetNudgeReceivedSound(const CString& strNudgeReceivedSound);

	virtual CString GetFileTransferAlertSound() const;
	virtual void SetFileTransferAlertSound(const CString& strFileTransferAlertSound);
	virtual CString GetFileTransferFinishedSound() const;
	virtual void SetFileTransferFinishedSound(const CString& strFileTransferFinishedSound);

	virtual void RegisterObserver(CUOLMessengerSoundSettingsObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerSoundSettingsObserver* pObserver);
	virtual void NotifyObservers(void (CUOLMessengerSoundSettingsObserver::*pfnCallback)(IUOLMessengerSoundSettings3Ptr), IUOLMessengerSoundSettings3Ptr pSoundSettings);

protected:
	DECLARE_SETTINGS_CALLBACK(CSoundSettings, Version);
	DECLARE_SETTINGS_CALLBACK(CSoundSettings, Sound);
	DECLARE_SETTINGS_CALLBACK(CSoundSettings, SoundDefaultDirPath);

	static DWORD VERSION_ATTRIBUTE_DEFAULT_VALUE;

private:
	void BuildUserSettingsPaths();
	CString GetUserPath(const CString& strUserProfile, const CString& strRelativePath);
	CString GetDefaultSettingsFilePath();
	
	CString GetFilePath(const CString& strEventType) const;

private:
	typedef CRBMap<CString, CSoundEventPtr> CSoundEventMap;

private:
	CSettingsEventSource	m_settingEventSource;

	CSettingsHandler	m_userSettingsHandler;
	CProcessMap			ms_tagProcess;

	DWORD				m_dwVersion;
	CString				m_strNewMessageSound;
	CString				m_strContactOnlineSound;
	CSoundEventMap		m_mapSoundEvents;

	CString				m_strSoundsFilePath;
	CString				m_strSoundsFolderPath;
	CString				m_strDefaultDirPath;

	CAtlList<CUOLMessengerSoundSettingsObserver*> m_listObservers;
	CComAutoCriticalSection	m_criticalSection;
};

MAKEAUTOPTR(CSoundSettings);
