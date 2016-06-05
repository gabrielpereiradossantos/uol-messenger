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

#include "IUOLMessengerEmoticon.h"


typedef CAtlList	<CString>			CEmoticonList;
typedef CAtlMap		<CString, CString>	CEmoticonMap;

class __declspec(novtable) IUOLMessengerEmoticonSettings
{
public:
	virtual ~IUOLMessengerEmoticonSettings() {}

	__declspec(deprecated) virtual void GetEmoticonList(CEmoticonList& listEmoticon) = 0;
	__declspec(deprecated) virtual BOOL GetEmoticonPath(const CString& strShortcut, CString& strPath) = 0;
	virtual BOOL AddEmoticon(const CString& strShortcut, const CString& strPath) = 0;
	virtual BOOL RemoveEmoticon(const CString& strShortcut) = 0;
	__declspec(deprecated) virtual BOOL IsEmoticonShortcut(const CString& strShortcut) = 0;

};

MAKEAUTOPTR(IUOLMessengerEmoticonSettings);



class __declspec(novtable) IUOLMessengerEmoticonSettings2 : public IUOLMessengerEmoticonSettings
{
public:
	virtual ~IUOLMessengerEmoticonSettings2() {}

	virtual BOOL LoadSettings() = 0;
	virtual BOOL SaveSettings() = 0;

	virtual CString GetDefaultDirPath() = 0;

	__declspec(deprecated) virtual void GetEmoticonList2(CAtlList<IUOLMessengerEmoticonPtr>& listEmoticons) = 0;
	
	__declspec(deprecated) virtual IUOLMessengerEmoticonPtr GetEmoticon2(const CString& strShortcut) = 0;
	virtual IUOLMessengerEmoticonPtr AddEmoticon2(const CString& strShortcut, const CString& strPath) = 0;
	virtual IUOLMessengerEmoticonPtr RemoveEmoticon2(const CString& strShortcut) = 0;
};

MAKEAUTOPTR(IUOLMessengerEmoticonSettings2);



class __declspec(novtable) IUOLMessengerEmoticonSettings3 : public IUOLMessengerEmoticonSettings2
{
public:
	
	enum EmoticonSetId
	{
		EMOTICON_SET_ID_INVALID		= 0,
		EMOTICON_SET_ID_ICQ			= 1,
		EMOTICON_SET_ID_JABBER		= 2,
		EMOTICON_SET_ID_MSN			= 4,
		EMOTICON_SET_ID_TODAOFERTA	= 8,
		EMOTICON_SET_ID_UOL			= 16,
		EMOTICON_SET_ID_YAHOO		= 32,
		EMOTICON_SET_ID_CUSTOM		= 64,	// Added by user.
		EMOTICON_SET_ID_ALL			= 127	// Must be the sum of all others.
	};
	
public:
	virtual ~IUOLMessengerEmoticonSettings3() {}
	
	virtual CString GetFileVersion() = 0;
	virtual CString GetUserSettingsFolderPath() = 0;
	
	virtual BOOL LoadDefaultSettings() = 0;
	
	virtual CString GetEmoticonSetNameById(EmoticonSetId emoticonSetId) = 0;
	virtual IUOLMessengerEmoticonSettings3::EmoticonSetId GetEmoticonSetIdByName(CString strEmoticonSetName) = 0;
	virtual void GetEmoticonSetNamesList(CAtlList<CString>& listEmoticonSetNames) = 0;
	virtual CString GetProtocolIdByEmoticonSetName(const CString& strEmoticonSetName) = 0;
	virtual int GetEmoticonSetIdsByProtocol(const CString& strProtocolId, BOOL bCombineCustomEmoticons) = 0;
	
	virtual void GetEmoticonList(int comboEmoticonSetIds, CEmoticonList& listEmoticon) = 0;
	virtual void GetEmoticonList(int comboEmoticonSetIds, CAtlList<IUOLMessengerEmoticonPtr>& listEmoticons) = 0;
	virtual void GetEmoticonList(const CString& strProtocolId, CAtlList<IUOLMessengerEmoticonPtr>& listEmoticons) = 0;
	virtual IUOLMessengerEmoticonPtr AddEmoticon(int nEmoticonSetId, const CString& strShortcut, const CString& strPath) = 0;
	virtual IUOLMessengerEmoticonPtr RemoveEmoticon(int nEmoticonSetId, const CString& strShortcut) = 0;
	virtual IUOLMessengerEmoticonPtr ChangeEmoticonShortcut(int nEmoticonSetId, const CString& strOldShortcut, const CString& strNewShortcut) = 0;
	virtual IUOLMessengerEmoticonPtr ChangeEmoticonFilePath(int nEmoticonSetId, const CString& strShortcut, const CString& strNewFilePath) = 0;
	virtual EmoticonSetId GetEmoticonOwnerId(int comboEmoticonSetIds, const CString& strShortcut) = 0;
	virtual IUOLMessengerEmoticonPtr GetEmoticon(int comboEmoticonSetIds, const CString& strShortcut) = 0;
	virtual BOOL GetEmoticonPath(int comboEmoticonSetIds, const CString& strShortcut, CString& strPath) = 0;
	virtual BOOL IsEmoticonShortcut(int comboEmoticonSetIds, const CString& strShortcut) = 0;
};

MAKEAUTOPTR(IUOLMessengerEmoticonSettings3);

