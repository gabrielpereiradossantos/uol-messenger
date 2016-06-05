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

#include "EmoticonElement.h"
#include "SettingsTraits.h"

CEmoticonElement::CEmoticonElement(BOOL bIsCustomEmoticon) :
m_strPath(""),
m_bIsCustomEmoticon(bIsCustomEmoticon)
{
	InitTagProcessMap();
}


CEmoticonElement::CEmoticonElement(const CString& strShortcut, const CString& strPath, BOOL bIsCustomEmoticon) : 
m_strPath(strPath),
m_bIsCustomEmoticon(bIsCustomEmoticon)
{
	InitTagProcessMap();
	
	m_listShortcut.AddHead(strShortcut);
}

CEmoticonElement::~CEmoticonElement(void)
{
}

void CEmoticonElement::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Shortcut", NEW_SETTINGS_CALLBACK(Shortcut, this)},
		{"Path", NEW_SETTINGS_CALLBACK(Path, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}

void CEmoticonElement::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}

void CEmoticonElement::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}


CString CEmoticonElement::GetShortcut() const
{
	return GetDefaultShortcut();
}

CString CEmoticonElement::GetFilePath() const
{
	return m_strPath;
}

void CEmoticonElement::SetFilePath(const CString& strPath)
{
	m_strPath = strPath;
}

BOOL CEmoticonElement::IsCustomEmoticon() const
{
	return m_bIsCustomEmoticon;
}

BOOL CEmoticonElement::IsShortcut(const CString& strShortcut)
{
	return (m_listShortcut.Find(strShortcut) != NULL);
}

CString CEmoticonElement::GetDefaultShortcut() const
{
	if (m_listShortcut.GetCount())
	{
		return m_listShortcut.GetAt(m_listShortcut.GetHeadPosition());
	}
	
	return "";
}

void CEmoticonElement::GetShortcutList(CAtlList<CString>& listShortcuts)
{
	for (POSITION pos = m_listShortcut.GetHeadPosition(); pos;)
	{
		listShortcuts.AddTail(m_listShortcut.GetNext(pos));
	}
}

void CEmoticonElement::ChangeShortcut(const CString& strOldShortcut, const CString& strNewShortcut)
{
	m_listShortcut.RemoveAt(m_listShortcut.Find(strOldShortcut));
	m_listShortcut.AddTail(strNewShortcut);
}

IMPLEMENT_SETTINGS_CALLBACK(CEmoticonElement, Shortcut)
{
	if (bSave)
	{
		MSXML::IXMLDOMNodePtr pPriorNode;
		
		for (POSITION pos = m_pThis->m_listShortcut.GetHeadPosition(); pos;)
		{
			CSettingsTraits::SetStringValue(pNode, strKey, 
					(CString)m_pThis->m_listShortcut.GetNext(pos));
		}
	}
	else
	{
		CString strShortcut;
		
		CSettingsTraits::GetStringValue(pNode, strKey, (CString&)strShortcut);
		
		m_pThis->m_listShortcut.AddTail(strShortcut);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CEmoticonElement, Path)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, (CString)m_pThis->m_strPath);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, (CString&)m_pThis->m_strPath);
	}
}
