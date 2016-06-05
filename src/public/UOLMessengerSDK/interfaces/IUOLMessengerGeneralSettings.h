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

#include <interfaces/IUOLMessengerSettingsEventSource.h>



class  __declspec(novtable) IUOLMessengerGeneralSettings : public IUOLMessengerSettingsEventSource
{
public:
	static enum SettingId
	{
		CL_AUTO_STARTUP = 0,
		CL_SELECTEDTHEME,
		SD_MUTE,
		SD_MUTE_WHILE_AWAY,
		SD_CHANGED_CONTACT_ONLINE,
		SD_CHANGED_NEW_MESSAGE,
		GS_AUTORECONNECT,
		GS_LAST_AVATAR_FOLDER,
		GS_LAST_SEND_FILE_FOLDER,
		GS_LAST_RECEIVE_FILE_FOLDER
	};


public:
	virtual ~IUOLMessengerGeneralSettings() {}

	virtual BOOL IsEnableSound() = 0;
	virtual BOOL IsMuteWhileAwayEnabled() = 0;
	virtual void EnableSound(BOOL bEnableSound) = 0;
	virtual void EnableMuteWhileAway(BOOL bEnableMuteWhileAway) = 0;

	virtual BOOL IsAutoStartupEnabled() = 0;
	virtual void EnableAutoStartup(BOOL bAutoStartup) = 0;

	virtual BOOL IsOpenBrowserEnabled() = 0;
	virtual void EnableOpenBrowser(BOOL bEnable) = 0;

	virtual DWORD GetCordX() = 0;
	virtual void SetCordX(DWORD nCordX) = 0;
	virtual DWORD GetCordY() = 0;
	virtual void SetCordY(DWORD nCordY) = 0;
	virtual DWORD GetSizeX() = 0;
	virtual void SetSizeX(DWORD nSizeX) = 0;
	virtual DWORD GetSizeY() = 0;
	virtual void SetSizeY(DWORD nSizeY) = 0;
	
	virtual DWORD GetUIMVersion() = 0;
	virtual void SetUIMVersion(DWORD nVersion) = 0;

	virtual DWORD GetListSplitterPosition() = 0;
	virtual void SetListSplitterPosition(DWORD nPosition) = 0;

	virtual void SetSelectedTheme(const CString& strSelectedTheme) = 0;
	virtual CString GetSelectedTheme() const = 0;
	virtual CString GetDefaultTheme() const = 0;
	
	virtual void SetLastAvatarFolder(const CString& strLastAvatarFolder) = 0;
	virtual CString GetLastAvatarFolder() const = 0;
	virtual CString GetPluginLibFolder() const = 0;

	virtual void SetDontShowHideAppWarning() = 0;
	virtual BOOL GetDontShowHideAppWarning() const = 0;

	__declspec(deprecated) virtual BOOL IsAutoReconnectEnabled() = 0;
	__declspec(deprecated) virtual void EnableAutoReconnect(BOOL bEnable) = 0;

	virtual void SetLastSendFileFolder(const CString& strFolder) = 0;
	virtual CString GetLastSendFileFolder() const = 0;

	virtual void SetLastReceiveFileFolder(const CString& strFolder) = 0;
	virtual CString GetLastReceiveFileFolder() const = 0;
};

MAKEAUTOPTR(IUOLMessengerGeneralSettings);