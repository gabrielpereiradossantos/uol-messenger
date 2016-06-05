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

#include <interfaces/IUOLMessengerEmoticon.h>
#include "ISettings.h"


class CEmoticonElement : 
	public IUOLMessengerEmoticon2,
	public ISettings
{
public:
	CEmoticonElement(BOOL bIsCustomEmoticon);
	CEmoticonElement(const CString& strShortcut, const CString& strPath, BOOL bIsCustomEmoticon);
	virtual ~CEmoticonElement(void);

	virtual void InitTagProcessMap();
	virtual void Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);
	virtual void Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey);

	// IUOLMessengerEmoticon
	virtual CString GetShortcut() const;
	virtual CString GetFilePath() const;
	virtual void SetFilePath(const CString& strFilePath);

	// IUOLMessengerEmoticon2
	virtual BOOL IsCustomEmoticon() const;
	virtual BOOL IsShortcut(const CString& strShortcut);
	virtual CString GetDefaultShortcut() const;
	virtual void GetShortcutList(CAtlList<CString>& listShortcuts);
	
	// Internal (not on public interface)
	virtual void ChangeShortcut(const CString& strOldShortcut, const CString& strNewShortcut);

protected:
	DECLARE_SETTINGS_CALLBACK(CEmoticonElement, Shortcut);
	DECLARE_SETTINGS_CALLBACK(CEmoticonElement, Path);

private:
	CAtlList<CString>	m_listShortcut;
	CString				m_strPath;
	CProcessMap			ms_tagProcess;
	BOOL				m_bIsCustomEmoticon;
};

MAKEAUTOPTR(CEmoticonElement);
