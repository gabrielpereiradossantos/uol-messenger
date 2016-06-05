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

#include <interfaces/IUOLMessengerGeneralSettings.h>

#include "ISettings.h"
#include "../controller/SettingsEventSource.h"

#include <MinDependencySpecialFolder.h>



class CGeneralSettings : public IUOLMessengerGeneralSettings,
						 public ISettings
{
public:
	CGeneralSettings();
	virtual ~CGeneralSettings(void);

protected:
	virtual BOOL IsEnableSound();
	virtual BOOL IsMuteWhileAwayEnabled();
	virtual void EnableSound(BOOL bEnableSound);
	virtual void EnableMuteWhileAway(BOOL bEnableMuteWhileAway);

	virtual void InitTagProcessMap();
	virtual void Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);
	virtual void Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);

	virtual BOOL IsAutoStartupEnabled();
	virtual void EnableAutoStartup(BOOL bAutoStartup);
	
	BOOL IsOpenBrowserEnabled();
	void EnableOpenBrowser(BOOL bEnable);
	void LoadAutoStartUp();
	BOOL SaveAutoStartUp();
	HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc, 
			LPCSTR lpszWorkingDir, LPCSTR lpszIconLocation = NULL, int nIconIndex = 0); 

	virtual DWORD GetCordX();
	virtual void SetCordX(DWORD nCordX);
	virtual DWORD GetCordY();
	virtual void SetCordY(DWORD nCordY);
	virtual DWORD GetSizeX();
	virtual void SetSizeX(DWORD nSizeX);
	virtual DWORD GetSizeY();
	virtual void SetSizeY(DWORD nSizeY);
	
	__declspec(deprecated) virtual DWORD GetUIMVersion();
	__declspec(deprecated) virtual void SetUIMVersion(DWORD nVersion);

	virtual DWORD GetListSplitterPosition();
	virtual void SetListSplitterPosition(DWORD nPosition);

	virtual void SetSelectedTheme(const CString& strSelectedTheme);
	virtual CString GetSelectedTheme() const;
	virtual CString GetDefaultTheme() const;

	virtual void SetLastAvatarFolder(const CString& strLastAvatarFolder);
	virtual CString GetLastAvatarFolder() const;

	virtual void SetDontShowHideAppWarning(); // Allways set
	virtual BOOL  GetDontShowHideAppWarning() const; 

	__declspec(deprecated) virtual BOOL IsAutoReconnectEnabled();
	__declspec(deprecated) virtual void EnableAutoReconnect(BOOL bEnable);

	virtual void SetLastSendFileFolder(const CString& strFolder);
	virtual CString GetLastSendFileFolder() const;

	virtual void SetLastReceiveFileFolder(const CString& strFolder);
	virtual CString GetLastReceiveFileFolder() const;

	virtual CString GetPluginLibFolder() const;

	void AddEventListener(IUOLMessengerSettingsEventListener* pListener);
	void RemoveEventListener(IUOLMessengerSettingsEventListener* pListener);
	void NotifyEventListeners(UINT nSettingId);	

protected:
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, OpenBrowser);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, Sound);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, MuteWhileAway);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, SelectedTheme);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, CordX);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, CordY);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, SizeX);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, SizeY);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, UIMVersion);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, ListSplitterPosition);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, LastAvatarFolder);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, DontShowHideAppWarning);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, AutoReconnect);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, LastSendFileFolder);
	DECLARE_SETTINGS_CALLBACK(CGeneralSettings, LastReceiveFileFolder);

private:
	BOOL					m_bOpenBrowser;
	BOOL					m_bSound;
	BOOL					m_bMuteWhileAway;

	CProcessMap				ms_tagProcess;
	CSettingsEventSource	m_settingEventSource;

	BOOL					m_bAutoStartup;
	CString					m_strSelectedTheme;
	CString					m_strDefaultTheme;

	CString					m_strLastAvatarFolder;

	DWORD					m_nCordX;
	DWORD					m_nCordY;
	DWORD					m_nSizeX;	
	DWORD					m_nSizeY;
	DWORD					m_nUIMVersion;

	DWORD					m_nListSplitterPosition;

	BOOL					m_bDontShowHideAppWarning;
	BOOL					m_bAutoReconnect;

	CString					m_strLastSendFileFolder;
	CString					m_strLastReceiveFileFolder;
};

MAKEAUTOPTR(CGeneralSettings);