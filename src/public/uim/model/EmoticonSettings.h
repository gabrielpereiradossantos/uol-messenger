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

#include <interfaces/IUOLMessengerEmoticonSettings.h>
#include <interfaces/IUOLMessengerAccount.h>
#include "ISettings.h"
#include "SettingsHandler.h"
#include "EmoticonElement.h"
#include "EmoticonSetElement.h"


class CEmoticonSettings : public IUOLMessengerEmoticonSettings3,
					      public ISettings
{
public:
	CEmoticonSettings();
	virtual ~CEmoticonSettings();
	
	void SetFileVersion(CString strFileVersion);
	
	CEmoticonSetElementPtr GetEmoticonSet(const CString& strEmoticonSetName);
	void AddEmoticonSet(CEmoticonSetElementPtr pEmoticonSetElement);
	void ReplaceEmoticonSet(CEmoticonSetElementPtr pOldEmoticonSetElement, 
			CEmoticonSetElementPtr pNewEmoticonSetElement);
	void AddCustomEmoticonSet();
	
protected:
	// IUOLMessengerEmoticonSettings
	virtual void GetEmoticonList(CEmoticonList& listEmoticon);
	virtual BOOL GetEmoticonPath(const CString& strShortcut, CString& strPath);
	virtual BOOL AddEmoticon(const CString& strShortcut, const CString& strPath);
	virtual BOOL RemoveEmoticon(const CString& strShortcut);
	virtual BOOL IsEmoticonShortcut(const CString& strShortcut);

	// IUOLMessengerEmoticonSettings2
	virtual BOOL LoadSettings();
	virtual BOOL SaveSettings();

	virtual CString GetDefaultDirPath();

	virtual void GetEmoticonList2(CAtlList<IUOLMessengerEmoticonPtr>& listEmoticons);
	
	virtual IUOLMessengerEmoticonPtr GetEmoticon2(const CString& strShortcut);
	virtual IUOLMessengerEmoticonPtr AddEmoticon2(const CString& strShortcut, const CString& strPath);
	virtual IUOLMessengerEmoticonPtr RemoveEmoticon2(const CString& strShortcut);

	// ISettings
	virtual void InitTagProcessMap();
	virtual void Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);
	virtual void Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);

	// IUOLMessengerEmoticonSettings3
	virtual CString GetFileVersion();
	
	virtual BOOL LoadDefaultSettings();
	
	virtual CString GetEmoticonSetNameById(EmoticonSetId emoticonSetId);
	virtual IUOLMessengerEmoticonSettings3::EmoticonSetId GetEmoticonSetIdByName(CString strEmoticonSetName);
	virtual void GetEmoticonSetNamesList(CAtlList<CString>& listEmoticonSetNames);
	virtual CString GetProtocolIdByEmoticonSetName(const CString& strEmoticonSetName);
	virtual int GetEmoticonSetIdsByProtocol(const CString& strProtocolId, BOOL bCombineCustomEmoticons);
	
	virtual void GetEmoticonList(int comboEmoticonSetIds, CEmoticonList& listEmoticon);
	virtual void GetEmoticonList(int comboEmoticonSetIds, CAtlList<IUOLMessengerEmoticonPtr>& listEmoticons);
	virtual void GetEmoticonList(const CString& strProtocolId, CAtlList<IUOLMessengerEmoticonPtr>& listEmoticons);
	virtual IUOLMessengerEmoticonPtr AddEmoticon(int nEmoticonSetId, const CString& strShortcut, const CString& strPath);
	virtual IUOLMessengerEmoticonPtr RemoveEmoticon(int nEmoticonSetId, const CString& strShortcut);
	virtual IUOLMessengerEmoticonPtr ChangeEmoticonShortcut(int nEmoticonSetId, const CString& strOldShortcut, const CString& strNewShortcut);
	virtual IUOLMessengerEmoticonPtr ChangeEmoticonFilePath(int nEmoticonSetId, const CString& strShortcut, const CString& strNewFilePath);
	virtual IUOLMessengerEmoticonSettings3::EmoticonSetId GetEmoticonOwnerId(int comboEmoticonSetIds, const CString& strShortcut);
	virtual IUOLMessengerEmoticonPtr GetEmoticon(int comboEmoticonSetIds, const CString& strShortcut);
	virtual BOOL GetEmoticonPath(int comboEmoticonSetIds, const CString& strShortcut, CString& strPath);
	virtual BOOL IsEmoticonShortcut(int comboEmoticonSetIds, const CString& strShortcut);

	CEmoticonSetElementPtr GetEmoticonSetImpl(CString emoticonSetName);
	IUOLMessengerEmoticonPtr SearchEmoticonSetsForEmoticon(int comboEmoticonSetIds, const CString& strShortcut);

protected:
	DECLARE_SETTINGS_CALLBACK(CEmoticonSettings, Version);
	DECLARE_SETTINGS_CALLBACK(CEmoticonSettings, EmoticonSet);
	DECLARE_SETTINGS_CALLBACK(CEmoticonSettings, EmoticonDefaultDirPath);

	static DWORD VERSION_ATTRIBUTE_DEFAULT_VALUE;

private:
	void BuildUserSettingsPaths();
	CString GetUserSettingsFilePath();
	CString GetUserSettingsFolderPath();
	CString GetDefaultSettingsFilePath();
	int GetEmoticonSetIdByProtocolStrId(const CString& strProtocolId);
	
	IUOLMessengerAccountPtr GetAccount();
	
private:
	typedef CRBMap<CString, CEmoticonSetElementPtr> CEmoticonSetElementMap;

	CString				m_strFileVersion;

	CString				m_strEmoticonsFilePath;
	CString				m_strEmoticonsFolderPath;
	CString				m_strDefaultDirPath;
	CSettingsHandler	m_userSettingsHandler;

	CProcessMap		ms_tagProcess;

	CEmoticonSetElementMap		m_mapEmoticonSet;
	
	BOOL				m_bHasUnsavedData;
};

MAKEAUTOPTR(CEmoticonSettings);