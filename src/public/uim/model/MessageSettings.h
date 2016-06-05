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

#include <interfaces/IUOLMessengerMessageSettings.h>

#include "ISettings.h"
#include "../controller/SettingsEventSource.h"


class CMessageSettings : public IUOLMessengerMessageSettings4,
						 public ISettings
{
public:
	CMessageSettings();
	virtual ~CMessageSettings(void);

protected:
	virtual void InitTagProcessMap();
	virtual void Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);
	virtual void Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);

	CString GetFontFace() const;
	void SetFontFace(const CString& strFontFace);

	INT GetFontSize();
	void SetFontSize(INT nFontSize);

	COLORREF GetFontColor();
	void SetFontColor(COLORREF colorRef);

	virtual DWORD GetCordX();
	virtual void SetCordX(DWORD nCordX);
	virtual DWORD GetCordY();
	virtual void SetCordY(DWORD nCordY);
	virtual DWORD GetSizeX();
	virtual void SetSizeX(DWORD nSizeX);
	virtual DWORD GetSizeY();
	virtual void SetSizeY(DWORD nSizeY);

    __declspec(deprecated) BOOL IsTimeStampEnabled();
	__declspec(deprecated) void EnableTimeStamp(BOOL bTimeStamp);

	__declspec(deprecated) BOOL IsContactIconEnabled();
	__declspec(deprecated) void EnableContactIcon(BOOL bContactIcon);

	BOOL IsLogMessagesEnabled();
	void EnableLogMessages(BOOL bLogMessages);

	BOOL IsTrayNotificationEnabled();
	void EnableTrayNotification(BOOL bTrayNotification );

	BOOL IsOnNewMsgWindowEnabled();
	void EnableOnNewMsgWindow(BOOL bNewMessage) ;

	BOOL IsFontBoldEnabled();
	void EnableFontBold(BOOL );

	BOOL IsFontStrikeoutEnabled();
	void EnableFontStrikeout(BOOL );

	BOOL IsFontItalicEnabled();
	void EnableFontItalic(BOOL );

	BOOL IsFontUnderlineEnabled();
	void EnableFontUnderline(BOOL );

	__declspec(deprecated) BOOL IsWindowNotificationEnabled();
	__declspec(deprecated) void EnableWindowNotification(BOOL bWindowNotification);

	BOOL IsShowUnreadMessagesAlertEnabled();
	void EnableShowUnreadMessagesAlert(BOOL bEnable);

	void AddEventListener(IUOLMessengerSettingsEventListener* pListener);
	void RemoveEventListener(IUOLMessengerSettingsEventListener* pListener);
	void NotifyEventListeners(UINT nSettingId);

    // IUOLMessengerMessageSettings3 methods
	virtual DWORD GetConversationWindowVerticalSplitterPosition();
    virtual DWORD GetConversationWindowHorizontalSplitterPosition();
    virtual void SetConversationWindowVerticalSplitterPosition(DWORD nPosition);
    virtual void SetConversationWindowHorizontalSplitterPosition(DWORD nPosition);

	virtual BOOL GetAllowNudges() const;
	virtual void SetAllowNudges(BOOL bAllowNudges);

	virtual BOOL GetAcceptCustomEmoticons() const;
	virtual void SetAcceptCustomEmoticons(BOOL bAcceptCustomEmoticons);

	virtual void SetFileTransferSaveFolderMode(DWORD dwMode);
	virtual DWORD GetFileTransferSaveFolderMode();
	virtual void SetDefaultFileTransferFolder(const CString& strPath);
	virtual CString GetDefaultFileTransferFolder() const;
	
	virtual CString GetDefaultMessageStyle() const;
	virtual void SetDefaultMessageStyle(const CString& strDefaultMessageStyle);

	virtual BOOL IsShowRecentHistoryEnabled() const;
	virtual void EnableShowRecentHistory(BOOL bEnable);

	// IUOLMessengerMessageSettings4 methods
	virtual DWORD GetHistoryWindowCordX();
	virtual void SetHistoryWindowCordX(DWORD nCordX);
	virtual DWORD GetHistoryWindowCordY();
	virtual void SetHistoryWindowCordY(DWORD nCordY);
	virtual DWORD GetHistoryWindowSizeX();
	virtual void SetHistoryWindowSizeX(DWORD nSizeX);
	virtual DWORD GetHistoryWindowSizeY();
	virtual void SetHistoryWindowSizeY(DWORD nSizeY);
	
protected:
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, LogMessages);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, TimeStamp);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, ContactIcon);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, TrayNotification);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, NewMsgWindow);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, WindowNotification);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, ShowUnreadMessagesAlert);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, FontBold);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, FontItalic);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, FontUnderline);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, FontStrikeout);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, FontFace);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, FontColor);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, FontSize);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, CordX);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, CordY);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, SizeX);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, SizeY);
    DECLARE_SETTINGS_CALLBACK(CMessageSettings, ConversationWindowVerticalSplitterPosition);
    DECLARE_SETTINGS_CALLBACK(CMessageSettings, ConversationWindowHorizontalSplitterPosition);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, AllowNudges);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, AcceptCustomEmoticons);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, DefaultMessageStyle);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, FileTransferSaveFolderMode);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, DefaultFileTransferFolder);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, ShowRecentHistory);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, HistoryWindowCordX);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, HistoryWindowCordY);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, HistoryWindowSizeX);
	DECLARE_SETTINGS_CALLBACK(CMessageSettings, HistoryWindowSizeY);

private:
	CProcessMap				ms_tagProcess;

	CSettingsEventSource    m_settingEventSource;

	BOOL					m_bTimeStamp;
	BOOL					m_bContactIcon;
	BOOL					m_bLogMessages;
	BOOL					m_bTrayNotification;
	BOOL					m_bWindowNotification;
	BOOL					m_bNewMsgWindow;
	BOOL					m_bShowUnreadMessagesAlert;

	// Font Settings
	BOOL					m_bFontBold;
	BOOL					m_bFontItalic;
	BOOL					m_bFontStrikeout;
	BOOL					m_bFontUnderline;
	CString					m_strFontFace;
	INT						m_nFontSize;
	COLORREF				m_colorRef;

	DWORD					m_nCordX;
	DWORD					m_nCordY;
	DWORD					m_nSizeX;	
	DWORD					m_nSizeY;
    DWORD					m_nConversationWindowVerticalSplitterPosition;
    DWORD					m_nConversationWindowHorizontalSplitterPosition;
	
	BOOL					m_bAllowNudges;
	BOOL					m_bAcceptCustomEmoticons;
	
	CString					m_strDefaultMessageStyle;

	DWORD					m_nFileTransferSaveFolderMode;
	CString					m_strDefaultFileTransferFolder;

	BOOL					m_bShowRecentHistory;

	DWORD					m_nHistoryWindowCordX;
	DWORD					m_nHistoryWindowCordY;
	DWORD					m_nHistoryWindowSizeX;	
	DWORD					m_nHistoryWindowSizeY;
};

MAKEAUTOPTR(CMessageSettings);
