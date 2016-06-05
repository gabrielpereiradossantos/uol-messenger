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

class __declspec(novtable) IUOLMessengerMessageSettings : public IUOLMessengerSettingsEventSource
{
public:
	virtual ~IUOLMessengerMessageSettings() {}

	virtual CString GetFontFace() const = 0;
	virtual void SetFontFace(const CString& strFontFace) = 0;

	virtual INT GetFontSize() = 0;
	virtual void SetFontSize(INT nFontSize) = 0;

	virtual COLORREF GetFontColor() = 0;
	virtual void SetFontColor(COLORREF colorRef) = 0;

	__declspec(deprecated) virtual BOOL IsTimeStampEnabled() = 0;
	__declspec(deprecated) virtual void EnableTimeStamp(BOOL bTimeStamp) = 0;

	__declspec(deprecated) virtual BOOL IsContactIconEnabled() = 0;
	__declspec(deprecated) virtual void EnableContactIcon(BOOL bContactIcon) = 0;

	virtual BOOL IsLogMessagesEnabled() = 0;
	virtual void EnableLogMessages(BOOL bLogMessages) = 0;

	virtual BOOL IsOnNewMsgWindowEnabled() = 0;
	virtual void EnableOnNewMsgWindow(BOOL bNewMessage) = 0;

	virtual BOOL IsTrayNotificationEnabled() = 0;
	virtual void EnableTrayNotification(BOOL bTrayNotification) = 0;

	virtual BOOL IsFontBoldEnabled() = 0;
	virtual void EnableFontBold(BOOL ) = 0;

	virtual BOOL IsFontItalicEnabled() = 0;
	virtual void EnableFontItalic(BOOL ) = 0;

	virtual BOOL IsFontStrikeoutEnabled() = 0;
	virtual void EnableFontStrikeout(BOOL ) = 0;

	virtual BOOL IsFontUnderlineEnabled() = 0;
	virtual void EnableFontUnderline(BOOL ) =0;

	__declspec(deprecated) virtual BOOL IsWindowNotificationEnabled() = 0;
	__declspec(deprecated) virtual void EnableWindowNotification(BOOL bWindowNotification) = 0;

	virtual BOOL IsShowUnreadMessagesAlertEnabled() = 0;
	virtual void EnableShowUnreadMessagesAlert(BOOL bEnable) = 0;

public:
	static enum SettingId
	{
		MSG_FONT_FACE = 0,
		MSG_FONT_SIZE,
		MSG_FONT_COLOR,
		MSG_TIMESTAMP,		/*DEPRECATED*/
		MSG_CONTACT_ICON,	/*DEPRECATED*/
		MSG_LOG_MESSAGES,
		MSG_TRAY_NOTIFICATION,
		MSG_WINDOW_NOTIFICATION,
		MSG_NEW_MSG_WINDOW_NOTIFICATION,
		MSG_SHOW_UNREAD_MESSAGES_ALERT,
		MSG_ALLOW_NUDGES,
		MSG_ACCEPT_CUSTOM_EMOTICONS,
		MSG_FILE_TRANSFER_SAVE_FOLDER_MODE,
		MSG_SHOW_RECENT_HISTORY
	};
};

class IUOLMessengerMessageSettings2 : public IUOLMessengerMessageSettings
{
public:
	virtual DWORD GetCordX() = 0;
	virtual void SetCordX(DWORD nCordX) = 0;
	virtual DWORD GetCordY() = 0;
	virtual void SetCordY(DWORD nCordY) = 0;
	virtual DWORD GetSizeX() = 0;
	virtual void SetSizeX(DWORD nSizeX) = 0;
	virtual DWORD GetSizeY() = 0;
	virtual void SetSizeY(DWORD nSizeY) = 0;
};

class IUOLMessengerMessageSettings3 : public IUOLMessengerMessageSettings2
{
public:
    static enum FileTransferSaveFolderModes
	{
		FILE_TRANSFER_SAVE_FOLDER_AWAYS_ASK = 0,
		FILE_TRANSFER_SAVE_FOLDER_USE_DEFAULT
	};

	virtual DWORD GetConversationWindowVerticalSplitterPosition() = 0;
    virtual DWORD GetConversationWindowHorizontalSplitterPosition() = 0;
    virtual void SetConversationWindowVerticalSplitterPosition(DWORD nPosition) = 0;
    virtual void SetConversationWindowHorizontalSplitterPosition(DWORD nPosition) = 0;
	
	virtual BOOL GetAllowNudges() const = 0;
	virtual void SetAllowNudges(BOOL bAllowNudges) = 0;
    
	virtual BOOL GetAcceptCustomEmoticons() const = 0;
	virtual void SetAcceptCustomEmoticons(BOOL bAcceptCustomEmoticons) = 0;
	
	virtual CString GetDefaultMessageStyle() const = 0;
	virtual void SetDefaultMessageStyle(const CString& strDefaultMessageStyle) = 0;
	
	virtual void SetFileTransferSaveFolderMode(DWORD dwMode) = 0;
	virtual DWORD GetFileTransferSaveFolderMode() = 0;
	virtual void SetDefaultFileTransferFolder(const CString& strPath) = 0;
	virtual CString GetDefaultFileTransferFolder() const = 0;

	virtual BOOL IsShowRecentHistoryEnabled() const = 0;
	virtual void EnableShowRecentHistory(BOOL bEnable) = 0;	
};


class IUOLMessengerMessageSettings4 : public IUOLMessengerMessageSettings3
{
public:
	virtual DWORD GetHistoryWindowCordX() = 0;
	virtual void SetHistoryWindowCordX(DWORD nCordX) = 0;
	virtual DWORD GetHistoryWindowCordY() = 0;
	virtual void SetHistoryWindowCordY(DWORD nCordY) = 0;
	virtual DWORD GetHistoryWindowSizeX() = 0;
	virtual void SetHistoryWindowSizeX(DWORD nSizeX) = 0;
	virtual DWORD GetHistoryWindowSizeY() = 0;
	virtual void SetHistoryWindowSizeY(DWORD nSizeY) = 0;
};


MAKEAUTOPTR(IUOLMessengerMessageSettings);
MAKEAUTOPTR(IUOLMessengerMessageSettings2);
MAKEAUTOPTR(IUOLMessengerMessageSettings3);
MAKEAUTOPTR(IUOLMessengerMessageSettings4);
