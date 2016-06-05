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

#include "IUOLMessengerSettingsEventSource.h"
#include "IUOLMessengerSoundEvent.h"


class  __declspec(novtable) IUOLMessengerSoundSettings : public IUOLMessengerSettingsEventSource
{
public:
	virtual ~IUOLMessengerSoundSettings() {}

	virtual CString GetNewMessageSound() const = 0;
	virtual void SetNewMessageSound(const CString& strNewMessageSound) = 0;

	virtual CString GetOnlineSound() const = 0;
	virtual void SetOnlineSound(const CString& strOnlineSound) = 0;
};

MAKEAUTOPTR(IUOLMessengerSoundSettings);



class __declspec(novtable) IUOLMessengerSoundSettings2 : public IUOLMessengerSoundSettings
{
public:
	virtual ~IUOLMessengerSoundSettings2() {}

	virtual BOOL LoadSettings() = 0;
	virtual BOOL SaveSettings() = 0;

	virtual CString GetDefaultDirPath() = 0;
	virtual CString GetUserConfigFilePath(const CString& strUserProfile) = 0;
	virtual CString GetUserSoundsDirPath(const CString& strUserProfile) = 0;

	virtual void GetSoundEventList(CAtlList<IUOLMessengerSoundEventPtr>& listSoundEvents) = 0;
	
	virtual IUOLMessengerSoundEventPtr GetSoundEvent(const CString& strEventType) = 0;
	virtual IUOLMessengerSoundEventPtr AddSoundEvent(const CString& strEventType, const CString& strDescription, const CString& strFilePath, BOOL bEnabled) = 0;
	virtual IUOLMessengerSoundEventPtr RemoveSoundEvent(const CString& strEventType) = 0;

	virtual BOOL ChangeSoundEventFile(const CString& strEventType, const CString& strFilePath) = 0;
	virtual BOOL EnableSoundEvent(const CString& strEventType, BOOL bEnable) = 0;	
	virtual BOOL GetSoundEventFilePath(const CString& strEventType, CString& strFilePath, BOOL bFullPath) = 0;

public:
	static const TCHAR NEW_MESSAGE_SOUND_EVENT[];
	static const TCHAR CONTACT_ONLINE_SOUND_EVENT[];
};

MAKEAUTOPTR(IUOLMessengerSoundSettings2);


class CUOLMessengerSoundSettingsObserver;
class  __declspec(novtable) IUOLMessengerSoundSettings3 : public IUOLMessengerSoundSettings2
{
public:
	virtual ~IUOLMessengerSoundSettings3() {}
	
	virtual BOOL LoadDefaultSettings() = 0;
	
	virtual CString GetNudgeReceivedSound() const = 0;
	virtual void SetNudgeReceivedSound(const CString& strNudgeReceivedSound) = 0;

	virtual CString GetFileTransferAlertSound() const = 0;
	virtual void SetFileTransferAlertSound(const CString& strFileTransferAlertSound) = 0;
	virtual CString GetFileTransferFinishedSound() const = 0;
	virtual void SetFileTransferFinishedSound(const CString& strFileTransferFinishedSound) = 0;

	virtual void RegisterObserver(CUOLMessengerSoundSettingsObserver* pObserver) = 0;
	virtual void UnregisterObserver(CUOLMessengerSoundSettingsObserver* pObserver) = 0;
	
public:
	static const TCHAR NUDGE_RECEIVED_SOUND_EVENT[];
	static const TCHAR FILE_TRANSFER_ALERT_SOUND_EVENT[];
	static const TCHAR FILE_TRANSFER_FINISHED_SOUND_EVENT[];
};

MAKEAUTOPTR(IUOLMessengerSoundSettings3);



__declspec(selectany)
const TCHAR IUOLMessengerSoundSettings2::NEW_MESSAGE_SOUND_EVENT[] = _T("NewMessage");

__declspec(selectany)
const TCHAR IUOLMessengerSoundSettings2::CONTACT_ONLINE_SOUND_EVENT[] = _T("ContactOnline");

__declspec(selectany)
const TCHAR IUOLMessengerSoundSettings3::NUDGE_RECEIVED_SOUND_EVENT[] = _T("NudgeReceived");

__declspec(selectany)
const TCHAR IUOLMessengerSoundSettings3::FILE_TRANSFER_ALERT_SOUND_EVENT[] = _T("FileTransferAlert");

__declspec(selectany)
const TCHAR IUOLMessengerSoundSettings3::FILE_TRANSFER_FINISHED_SOUND_EVENT[] = _T("FileTransferFinished");



class CUOLMessengerSoundSettingsObserver
{
public:
	virtual ~CUOLMessengerSoundSettingsObserver() {}

	virtual void OnSoundSettingsChanged(IUOLMessengerSoundSettings3Ptr pSoundSettings) = 0;
};