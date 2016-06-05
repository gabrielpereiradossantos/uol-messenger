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

#include "MessageSettings.h"
#include "SettingsTraits.h"

CMessageSettings::CMessageSettings() :
	m_strFontFace(_T("")), 
	m_nFontSize(0),
	m_colorRef(0),
	m_bTimeStamp(TRUE),
	m_bContactIcon(TRUE),
	m_bLogMessages(TRUE),
	m_bTrayNotification(FALSE),
	m_bFontBold(FALSE),
	m_bFontItalic(FALSE),
	m_bFontUnderline(FALSE),
	m_bFontStrikeout(FALSE),
	m_bNewMsgWindow(TRUE),
	m_bWindowNotification(TRUE),
	m_bShowUnreadMessagesAlert(TRUE),
	m_nCordX(200),
	m_nCordY(200),
	m_nSizeX(400),
	m_nSizeY(380),
	m_nConversationWindowVerticalSplitterPosition(-1),
	m_nConversationWindowHorizontalSplitterPosition(-1),
	m_bAllowNudges(TRUE),
	m_bAcceptCustomEmoticons(TRUE),
	m_strDefaultMessageStyle(_T("UOLMessenger")),
	m_nFileTransferSaveFolderMode(0),
	m_bShowRecentHistory(TRUE),
	m_nHistoryWindowCordX(100),
	m_nHistoryWindowCordY(100),
	m_nHistoryWindowSizeX(630),
	m_nHistoryWindowSizeY(580)
{
	m_settingEventSource.SetEventSource(this);

	InitTagProcessMap();
}

CMessageSettings::~CMessageSettings(void)
{
}

void CMessageSettings::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"LogMessages", NEW_SETTINGS_CALLBACK(LogMessages, this)},
		{"TimeStamp", NEW_SETTINGS_CALLBACK(TimeStamp, this)},
		{"ContactIcon", NEW_SETTINGS_CALLBACK(ContactIcon, this)},		
		{"TrayNotification", NEW_SETTINGS_CALLBACK(TrayNotification, this)},
		{"NewMsgWindow", NEW_SETTINGS_CALLBACK(NewMsgWindow, this)},
		{"ShowUnreadMessagesAlert", NEW_SETTINGS_CALLBACK(ShowUnreadMessagesAlert, this)},
		{"FontBold", NEW_SETTINGS_CALLBACK(FontBold, this)},
		{"FontItalic", NEW_SETTINGS_CALLBACK(FontItalic, this)},
		{"FontUnderline", NEW_SETTINGS_CALLBACK(FontUnderline, this)},
		{"FontStrikeout", NEW_SETTINGS_CALLBACK(FontStrikeout, this)},
		{"FontFace", NEW_SETTINGS_CALLBACK(FontFace, this)},
		{"FontSize", NEW_SETTINGS_CALLBACK(FontSize, this)},
		{"FontColor", NEW_SETTINGS_CALLBACK(FontColor, this)},
		{"WindowNotification", NEW_SETTINGS_CALLBACK(WindowNotification, this)},
		{"CordX", NEW_SETTINGS_CALLBACK(CordX, this)},
		{"CordY", NEW_SETTINGS_CALLBACK(CordY, this)},
		{"SizeX", NEW_SETTINGS_CALLBACK(SizeX, this)},
		{"SizeY", NEW_SETTINGS_CALLBACK(SizeY, this)},
        {"ConversationWindowVerticalSplitterPosition", NEW_SETTINGS_CALLBACK(ConversationWindowVerticalSplitterPosition, this)},
        {"ConversationWindowHorizontalSplitterPosition", NEW_SETTINGS_CALLBACK(ConversationWindowHorizontalSplitterPosition, this)},
		{"AllowNudges", NEW_SETTINGS_CALLBACK(AllowNudges, this)},
		{"AcceptCustomEmoticons", NEW_SETTINGS_CALLBACK(AcceptCustomEmoticons, this)},
		{"DefaultMessageStyle", NEW_SETTINGS_CALLBACK(DefaultMessageStyle, this)},
		{"AcceptCustomEmoticons", NEW_SETTINGS_CALLBACK(AcceptCustomEmoticons, this)},
		{"FileTransferSaveFolderMode", NEW_SETTINGS_CALLBACK(FileTransferSaveFolderMode, this)},
		{"DefaultFileTransferFolder", NEW_SETTINGS_CALLBACK(DefaultFileTransferFolder, this)},
		{"ShowRecentHistory", NEW_SETTINGS_CALLBACK(ShowRecentHistory, this)},
		{"HistoryWindowCordX", NEW_SETTINGS_CALLBACK(HistoryWindowCordX, this)},
		{"HistoryWindowCordY", NEW_SETTINGS_CALLBACK(HistoryWindowCordY, this)},
		{"HistoryWindowSizeX", NEW_SETTINGS_CALLBACK(HistoryWindowSizeX, this)},
		{"HistoryWindowSizeY", NEW_SETTINGS_CALLBACK(HistoryWindowSizeY, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}

void CMessageSettings::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}

void CMessageSettings::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}

CString CMessageSettings::GetFontFace() const
{
	return m_strFontFace;
}

void CMessageSettings::SetFontFace(const CString& strFontFace)
{
	m_strFontFace = strFontFace;
}

INT CMessageSettings::GetFontSize()
{
	return m_nFontSize;
}

void CMessageSettings::SetFontSize(INT nFontSize)
{
	m_nFontSize = nFontSize;
}

COLORREF CMessageSettings::GetFontColor()
{
	return m_colorRef;
}

void CMessageSettings::SetFontColor(COLORREF colorRef)
{
	m_colorRef = colorRef;
}

__declspec(deprecated) BOOL CMessageSettings::IsTimeStampEnabled()
{
	return m_bTimeStamp;
}

__declspec(deprecated) void CMessageSettings::EnableTimeStamp(BOOL bTimeStamp)
{
	m_bTimeStamp = bTimeStamp;
}

__declspec(deprecated) BOOL CMessageSettings::IsContactIconEnabled()
{
	return m_bContactIcon;
}

__declspec(deprecated) void CMessageSettings::EnableContactIcon(BOOL bContactIcon)
{
	m_bContactIcon = bContactIcon;
}

BOOL CMessageSettings::IsLogMessagesEnabled()
{
	return m_bLogMessages;
}

void CMessageSettings::EnableLogMessages(BOOL bLogMessages)
{
	m_bLogMessages = bLogMessages;
}

BOOL CMessageSettings::IsTrayNotificationEnabled()
{
	return m_bTrayNotification;
}

void CMessageSettings::EnableTrayNotification(BOOL bTrayNotification)
{
	m_bTrayNotification = bTrayNotification;
}

BOOL CMessageSettings::IsOnNewMsgWindowEnabled()
{
	return m_bNewMsgWindow;
}

void CMessageSettings::EnableOnNewMsgWindow(BOOL bNewMessage)
{
	m_bNewMsgWindow = bNewMessage;
}


BOOL CMessageSettings::IsFontBoldEnabled()
{
	return m_bFontBold;
}

void CMessageSettings::EnableFontBold(BOOL bFontBold)
{
	m_bFontBold = bFontBold;
}

BOOL CMessageSettings::IsFontItalicEnabled()
{
	return m_bFontItalic;
}

void CMessageSettings::EnableFontItalic(BOOL bFontItalic)
{
	m_bFontItalic = bFontItalic;
}


BOOL CMessageSettings::IsFontStrikeoutEnabled()
{
	return m_bFontStrikeout;
}

void CMessageSettings::EnableFontStrikeout(BOOL bStrikeout)
{
	m_bFontStrikeout = bStrikeout;
}

BOOL CMessageSettings::IsFontUnderlineEnabled()
{
	return m_bFontUnderline;
}

void CMessageSettings::EnableFontUnderline(BOOL bFontUnderline)
{
	m_bFontUnderline = bFontUnderline;
}

__declspec(deprecated) BOOL CMessageSettings::IsWindowNotificationEnabled()
{
	ATLASSERT(FALSE);

	return m_bWindowNotification;
}


__declspec(deprecated) void CMessageSettings::EnableWindowNotification(BOOL bWindowNotification)
{
	m_bWindowNotification = bWindowNotification;
}


BOOL CMessageSettings::IsShowUnreadMessagesAlertEnabled()
{
	return m_bShowUnreadMessagesAlert;
}


void CMessageSettings::EnableShowUnreadMessagesAlert(BOOL bEnable)
{
	m_bShowUnreadMessagesAlert = bEnable;
}


void CMessageSettings::AddEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.AddEventListener(pListener);
}


void CMessageSettings::RemoveEventListener(IUOLMessengerSettingsEventListener* pListener)
{
	m_settingEventSource.RemoveEventListener(pListener);
}


void CMessageSettings::NotifyEventListeners(UINT nSettingId)
{
	m_settingEventSource.NotifyEventListeners(nSettingId);
}

DWORD CMessageSettings::GetConversationWindowVerticalSplitterPosition()
{
	return m_nConversationWindowVerticalSplitterPosition;
}

DWORD CMessageSettings::GetConversationWindowHorizontalSplitterPosition()
{
	return m_nConversationWindowHorizontalSplitterPosition;
}

void CMessageSettings::SetConversationWindowVerticalSplitterPosition(DWORD nPosition)
{
	m_nConversationWindowVerticalSplitterPosition = nPosition;
}

void CMessageSettings::SetConversationWindowHorizontalSplitterPosition(DWORD nPosition)
{
	m_nConversationWindowHorizontalSplitterPosition = nPosition;
}

BOOL CMessageSettings::GetAllowNudges() const
{
	return m_bAllowNudges;
}

void CMessageSettings::SetAllowNudges(BOOL bAllowNudges)
{
	m_bAllowNudges = bAllowNudges;
}

BOOL CMessageSettings::GetAcceptCustomEmoticons() const
{
	return m_bAcceptCustomEmoticons;
}

void CMessageSettings::SetAcceptCustomEmoticons(BOOL bAcceptCustomEmoticons)
{
	m_bAcceptCustomEmoticons = bAcceptCustomEmoticons;
}


CString CMessageSettings::GetDefaultMessageStyle() const
{
	return m_strDefaultMessageStyle;
}


void CMessageSettings::SetDefaultMessageStyle(const CString& strDefaultMessageStyle)
{
	m_strDefaultMessageStyle = strDefaultMessageStyle;
}


BOOL CMessageSettings::IsShowRecentHistoryEnabled() const
{
	return m_bShowRecentHistory;
}


void CMessageSettings::EnableShowRecentHistory(BOOL bEnable)
{
	m_bShowRecentHistory = bEnable;
}


DWORD CMessageSettings::GetCordX()
{
	return m_nCordX;
}

void CMessageSettings::SetCordX(DWORD nCordX)
{
	m_nCordX = nCordX;
}

DWORD CMessageSettings::GetCordY()
{
	return m_nCordY;
}

void CMessageSettings::SetCordY(DWORD nCordY)
{
	m_nCordY = nCordY;
}

DWORD CMessageSettings::GetSizeX()
{
	return m_nSizeX;
}

void CMessageSettings::SetSizeX(DWORD nSizeX)
{
	m_nSizeX = nSizeX;
}

DWORD CMessageSettings::GetSizeY()
{
	return m_nSizeY;
}

void CMessageSettings::SetSizeY(DWORD nSizeY)
{
	m_nSizeY = nSizeY;
}

void CMessageSettings::SetFileTransferSaveFolderMode(DWORD dwMode)
{
	m_nFileTransferSaveFolderMode = dwMode;
}


DWORD CMessageSettings::GetFileTransferSaveFolderMode()
{
	return m_nFileTransferSaveFolderMode;
}


void CMessageSettings::SetDefaultFileTransferFolder(const CString& strPath)
{
	m_strDefaultFileTransferFolder = strPath;
}


CString CMessageSettings::GetDefaultFileTransferFolder() const
{
	return m_strDefaultFileTransferFolder;
}


DWORD CMessageSettings::GetHistoryWindowCordX()
{
	return m_nHistoryWindowCordX;
}


void CMessageSettings::SetHistoryWindowCordX(DWORD nCordX)
{
	m_nHistoryWindowCordX = nCordX;
}


DWORD CMessageSettings::GetHistoryWindowCordY()
{
	return m_nHistoryWindowCordY;
}


void CMessageSettings::SetHistoryWindowCordY(DWORD nCordY)
{
	m_nHistoryWindowCordY = nCordY;
}


DWORD CMessageSettings::GetHistoryWindowSizeX()
{
	return m_nHistoryWindowSizeX;
}


void CMessageSettings::SetHistoryWindowSizeX(DWORD nSizeX)
{
	m_nHistoryWindowSizeX = nSizeX;
}


DWORD CMessageSettings::GetHistoryWindowSizeY()
{
	return m_nHistoryWindowSizeY;
}


void CMessageSettings::SetHistoryWindowSizeY(DWORD nSizeY)
{
	m_nHistoryWindowSizeY = nSizeY;
}



///////////////////////////////////////////////////////////////////////////////
// Callbacks
IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, TimeStamp)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bTimeStamp);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bTimeStamp);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, ContactIcon)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bContactIcon);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bContactIcon);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, TrayNotification)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bTrayNotification);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bTrayNotification);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, NewMsgWindow)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bNewMsgWindow);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bNewMsgWindow);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, WindowNotification)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bWindowNotification);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bWindowNotification);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, ShowUnreadMessagesAlert)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bShowUnreadMessagesAlert);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bShowUnreadMessagesAlert);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, LogMessages)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bLogMessages);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bLogMessages);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, FontBold)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bFontBold);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bFontBold);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, FontItalic)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bFontItalic);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bFontItalic);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, FontStrikeout)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bFontStrikeout);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bFontStrikeout);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, FontUnderline)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bFontUnderline);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bFontUnderline);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, FontSize)
{
	if (bSave)
	{
		CSettingsTraits::SetIntValue(pNode, strKey, m_pThis->m_nFontSize);
	}
	else
	{
		CSettingsTraits::GetIntValue(pNode, strKey, m_pThis->m_nFontSize);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, FontFace)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strFontFace);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strFontFace);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, FontColor)
{
	if (bSave)
	{
		CSettingsTraits::SetDWORDValue(pNode,strKey,  m_pThis->m_colorRef);
	}
	else
	{
		CSettingsTraits::GetDWORDValue(pNode,strKey,  m_pThis->m_colorRef);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, ConversationWindowVerticalSplitterPosition)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nConversationWindowVerticalSplitterPosition);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nConversationWindowVerticalSplitterPosition);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, ConversationWindowHorizontalSplitterPosition)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nConversationWindowHorizontalSplitterPosition);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nConversationWindowHorizontalSplitterPosition);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, AllowNudges)
{
	if (!bSave)
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bAllowNudges);
	}
	else
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bAllowNudges);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, AcceptCustomEmoticons)
{
	if (!bSave)
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bAcceptCustomEmoticons);
	}
	else
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bAcceptCustomEmoticons);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, DefaultMessageStyle)
{
	if (!bSave)
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strDefaultMessageStyle);
	}
	else
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strDefaultMessageStyle);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, CordX)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nCordX);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nCordX);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, CordY)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nCordY);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nCordY);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, SizeX)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nSizeX);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nSizeX);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, SizeY)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nSizeY);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nSizeY);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, FileTransferSaveFolderMode)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nFileTransferSaveFolderMode);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nFileTransferSaveFolderMode);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, DefaultFileTransferFolder)
{
	if (!bSave)
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strDefaultFileTransferFolder);
	}
	else
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strDefaultFileTransferFolder);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, ShowRecentHistory)
{
	if (!bSave)
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bShowRecentHistory);
	}
	else
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bShowRecentHistory);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, HistoryWindowCordX)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nHistoryWindowCordX);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nHistoryWindowCordX);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, HistoryWindowCordY)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nHistoryWindowCordY);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nHistoryWindowCordY);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, HistoryWindowSizeX)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nHistoryWindowSizeX);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nHistoryWindowSizeX);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CMessageSettings, HistoryWindowSizeY)
{
	if (!bSave)
	{
		CSettingsTraits::GetDWORDValue(pNode, strKey, m_pThis->m_nHistoryWindowSizeY);
	}
	else
	{
		CSettingsTraits::SetDWORDValue(pNode, strKey, m_pThis->m_nHistoryWindowSizeY);
	}
}