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
#include "ISettings.h"
#include "SettingsHandler.h"
#include "EmoticonElement.h"


class CEmoticonSetElement : public ISettings
{
public:
	CEmoticonSetElement();
	virtual ~CEmoticonSetElement();
	
	virtual void SetName(const CString& strEmoticonSetName);
	virtual CString GetName() const;
	
	virtual int GetEmoticonCount();
	virtual void GetEmoticonList(CEmoticonList& listEmoticon);
	virtual void GetEmoticonList(CAtlList<IUOLMessengerEmoticonPtr>& listEmoticons);
	virtual BOOL GetEmoticonPath(const CString& strShortcut, CString& strPath);
	virtual IUOLMessengerEmoticonPtr GetEmoticon(const CString& strShortcut);
	virtual IUOLMessengerEmoticonPtr AddEmoticon(const CString& strShortcut, const CString& strPath);
	
	// This version of AddEmoticon method is intended to be used used only 
	// internally for merging of new and old versions of "Emoticon.xml" 
	// file. See "CEmoticonSettingsUpdater".
	// This way we don't need to worry about the Set it belongs to and about 
	// the physical file.
	//
	virtual void AddEmoticon(IUOLMessengerEmoticonPtr pEmoticon);
	
	virtual IUOLMessengerEmoticonPtr RemoveEmoticon(const CString& strShortcut);
	virtual IUOLMessengerEmoticonPtr ChangeEmoticonShortcut(const CString& strOldShortcut, const CString& strNewShortcut);
	virtual IUOLMessengerEmoticonPtr ChangeEmoticonFilePath(const CString& strShortcut, const CString& strNewFilePath);
	virtual BOOL IsEmoticonShortcut(const CString& strShortcut);
	
	virtual void Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);
	virtual void Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);
	
protected:
	// ISettings
	virtual void InitTagProcessMap();
	
	BOOL ImportFileToProfile(CString& strFinalFilePath, const CString& strOriginalFilePath);
	
protected:
	DECLARE_SETTINGS_CALLBACK(CEmoticonSetElement, Name);
	DECLARE_SETTINGS_CALLBACK(CEmoticonSetElement, Emoticon);
	
private:
	typedef CRBMap<CString, CEmoticonElementPtr> CEmoticonElementMap;
	
	CString					m_strEmoticonSetName;
	CString					m_strCustomEmoticonSetName;		// Comparation purpouses.
	
	CSettingsHandler		m_userSettingsHandler;
	
	CProcessMap				ms_tagProcess;
	
	CEmoticonElementMap		m_mapEmoticon;
};

MAKEAUTOPTR(CEmoticonSetElement);

