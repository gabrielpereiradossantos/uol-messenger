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
#include "EmoticonSetElement.h"

#include "SettingsTraits.h"
#include "EmoticonElement.h"
#include "../controller/UIMApplication.h"
#include "../resource.h"
#include "../controller/SettingsManager.h"
#include "EmoticonSettings.h"


CEmoticonSetElement::CEmoticonSetElement() : 
		m_userSettingsHandler("")
{
	// Initialize settings processing map
	InitTagProcessMap();
	
	m_userSettingsHandler.AddSetting("EmoticonSet", this);
	
	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = 
			ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(
					CUIMApplication::GetApplication()->GetSettingsManager());
	
	IUOLMessengerEmoticonSettings3Ptr pEmoticonSettings3;
	
	ATLASSERT(pSettingsManager2);
	if (pSettingsManager2)
	{
		pEmoticonSettings3 = ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>
				(pSettingsManager2->GetEmoticonSettings());
	}
	
	if (pEmoticonSettings3)
	{
		m_strCustomEmoticonSetName.Format("%s", pEmoticonSettings3->GetEmoticonSetNameById(
				IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_CUSTOM));
	}
}


CEmoticonSetElement::~CEmoticonSetElement()
{
}


void CEmoticonSetElement::SetName(const CString& strEmoticonSetName)
{
	m_strEmoticonSetName = strEmoticonSetName;
}


CString CEmoticonSetElement::GetName() const
{
	return m_strEmoticonSetName;
}


void CEmoticonSetElement::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Name", NEW_SETTINGS_CALLBACK(Name, this)},
		{"Emoticon", NEW_SETTINGS_CALLBACK(Emoticon, this)},
	};
	
	int nSize = sizeof(ih) / sizeof(ih[0]);
	
	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}


void CEmoticonSetElement::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	m_mapEmoticon.RemoveAll();
	
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}


void CEmoticonSetElement::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}


int CEmoticonSetElement::GetEmoticonCount()
{
	return m_mapEmoticon.GetCount();
}


void CEmoticonSetElement::GetEmoticonList(CEmoticonList& listEmoticon)
{
	for (POSITION pos = m_mapEmoticon.GetHeadPosition(); pos;)
	{
		CEmoticonElementMap::CPair* pPair = m_mapEmoticon.GetNext(pos);
		
		pPair->m_value->GetShortcutList(listEmoticon);
	}
}


void CEmoticonSetElement::GetEmoticonList(CAtlList<IUOLMessengerEmoticonPtr>& listEmoticons)
{
	for (POSITION pos = m_mapEmoticon.GetHeadPosition(); pos;)
	{
		CEmoticonElementMap::CPair* pPair = m_mapEmoticon.GetNext(pos);
		
		listEmoticons.AddTail(pPair->m_value);
	}
}


BOOL CEmoticonSetElement::GetEmoticonPath(const CString& strShortcut, CString& strPath)
{
	IUOLMessengerEmoticonPtr pEmoticon = GetEmoticon(strShortcut);
	
	if (pEmoticon)
	{
		strPath = pEmoticon->GetFilePath();
		return TRUE;
	}
	
	return FALSE;
}


IUOLMessengerEmoticonPtr CEmoticonSetElement::GetEmoticon(const CString& strShortcut)
{
	for (POSITION pos = m_mapEmoticon.GetHeadPosition(); pos;)
	{
		CEmoticonElementMap::CPair* pPair = m_mapEmoticon.GetNext(pos);
		
		if (pPair->m_value->IsShortcut(strShortcut))
		{
			return pPair->m_value;
		}
	}
	
	return IUOLMessengerEmoticonPtr();
}


IUOLMessengerEmoticonPtr CEmoticonSetElement::AddEmoticon(const CString& strShortcut, const CString& strPath)
{
	CEmoticonElementPtr pNewEmoticon;
	
	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = 
			ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
	
	IUOLMessengerEmoticonSettings3Ptr pEmoticonSettings3;
	
	if (pSettingsManager2)
	{
		pEmoticonSettings3 = ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>(
					pSettingsManager2->GetEmoticonSettings());
	}
	
	if ((FALSE == strShortcut.IsEmpty()) &&
		(FALSE == strPath.IsEmpty()) &&
		(pEmoticonSettings3))
	{
		/*
		// Remove old emoticon reference if any
		IUOLMessengerEmoticon2Ptr pEmoticon2 = 
				ap_dynamic_cast<IUOLMessengerEmoticon2Ptr>GetEmoticon(strShortcut);
		
		if (pEmoticon2 != NULL)
		{
			m_mapEmoticon.RemoveKey(pEmoticon2->GetDefaultShortcut());
		}
		*/
		
		// If this shortcut is already being used, we can't add it.
		if (!pEmoticonSettings3->IsEmoticonShortcut(
				IUOLMessengerEmoticonSettings3::EMOTICON_SET_ID_CUSTOM, strShortcut))
		{
			CString strFilePath;
			
			ImportFileToProfile(strFilePath, strPath);
			
			if (m_strEmoticonSetName.CompareNoCase(m_strCustomEmoticonSetName) == 0)
			{
				pNewEmoticon = new CEmoticonElement(strShortcut, strFilePath, TRUE);
			}
			else
			{
				pNewEmoticon = new CEmoticonElement(strShortcut, strFilePath, FALSE);
			}
			
			// Add new emoticon.
			m_mapEmoticon.SetAt(strShortcut, pNewEmoticon);
		}
	}
	
	return pNewEmoticon;
}


void CEmoticonSetElement::AddEmoticon(IUOLMessengerEmoticonPtr pEmoticon)
{
	if (pEmoticon)
	{
		// Add the new emoticon.
		CEmoticonElementPtr pEmoticonElement = 
				ap_dynamic_cast<CEmoticonElementPtr>(pEmoticon);
		
		if (pEmoticonElement)
		{
			m_mapEmoticon.SetAt(pEmoticon->GetShortcut(), pEmoticonElement);
		}
	}
}


IUOLMessengerEmoticonPtr CEmoticonSetElement::RemoveEmoticon(const CString& strShortcut)
{
	IUOLMessengerEmoticonPtr pRemovedEmoticon;
	
	CEmoticonElementMap::CPair* pPair = m_mapEmoticon.Lookup(strShortcut);
	
	if (pPair != NULL)
	{
		pRemovedEmoticon = pPair->m_value;
		
		if (m_strEmoticonSetName.CompareNoCase(m_strCustomEmoticonSetName) == 0)
		{
			//::DeleteFile(pRemovedEmoticon->GetFilePath());
		}
		
		m_mapEmoticon.RemoveKey(strShortcut);
	}
	
	return pRemovedEmoticon;
}


IUOLMessengerEmoticonPtr CEmoticonSetElement::ChangeEmoticonShortcut(const CString& strOldShortcut, const CString& strNewShortcut)
{
	CEmoticonElementPtr pChangedEmoticon;
	
	CEmoticonElementMap::CPair* pPair = m_mapEmoticon.Lookup(strOldShortcut);
	
	if (pPair != NULL)
	{
		pChangedEmoticon = pPair->m_value;
		
		m_mapEmoticon.RemoveKey(strOldShortcut);
		m_mapEmoticon.SetAt(strNewShortcut, pChangedEmoticon);
		
		pChangedEmoticon->ChangeShortcut(strOldShortcut, strNewShortcut);
	}
	
	return pChangedEmoticon;
}


IUOLMessengerEmoticonPtr CEmoticonSetElement::ChangeEmoticonFilePath(const CString& strShortcut, const CString& strNewFilePath)
{
	CEmoticonElementPtr pChangedEmoticon;
	CString strFinalFilePath;
	
	CEmoticonElementMap::CPair* pPair = m_mapEmoticon.Lookup(strShortcut);
	
	if (pPair != NULL)
	{
		pChangedEmoticon = pPair->m_value;
		
		if (m_strEmoticonSetName.CompareNoCase(m_strCustomEmoticonSetName) == 0)
		{
			//::DeleteFile(pChangedEmoticon->GetFilePath());
		}
		
		ImportFileToProfile(strFinalFilePath, strNewFilePath);
		
		pChangedEmoticon->SetFilePath(strFinalFilePath);
	}
	
	return pChangedEmoticon;
}


BOOL CEmoticonSetElement::IsEmoticonShortcut(const CString& strShortcut)
{
	IUOLMessengerEmoticonPtr pEmoticon = GetEmoticon(strShortcut);
	
	if (pEmoticon)
	{
		return TRUE;
	}
	
	return FALSE;
}


IMPLEMENT_SETTINGS_CALLBACK(CEmoticonSetElement, Name)
{
	if (bSave)
	{
		CSettingsTraits::SetStringAttributeValue(pNode, strKey, m_pThis->m_strEmoticonSetName);
	}
	else
	{
		CString strEmoticonSetName;
		
		CSettingsTraits::GetStringAttibuteValue(pNode, strKey, strEmoticonSetName);
		
		if (FALSE == strEmoticonSetName.IsEmpty())
		{
			m_pThis->m_strEmoticonSetName = strEmoticonSetName;
		}
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CEmoticonSetElement, Emoticon)
{
	CEmoticonElementPtr pEmoticonElement;
	
	if (bSave)
	{
		for (POSITION pos = m_pThis->m_mapEmoticon.GetHeadPosition(); pos;)
		{
			CEmoticonElementMap::CPair* pPair = m_pThis->m_mapEmoticon.GetNext(pos);
			
			pEmoticonElement = pPair->m_value;
			
			MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, strKey);
			pEmoticonElement->Save(pChild, strKey);
		}
	}
	else
	{
		if (m_pThis->GetName().CompareNoCase(m_pThis->m_strCustomEmoticonSetName) == 0)
		{
			pEmoticonElement = new CEmoticonElement(TRUE);
		}
		else
		{
			pEmoticonElement = new CEmoticonElement(FALSE);
		}
		
		if (pEmoticonElement)
		{
			pEmoticonElement->Load(pNode, strKey);
			m_pThis->m_mapEmoticon.SetAt(pEmoticonElement->GetDefaultShortcut(), pEmoticonElement);
		}
	}
}


BOOL CEmoticonSetElement::ImportFileToProfile(
		CString& strFinalFilePath, const CString& strOriginalFilePath)
{
	BOOL bImported = FALSE;
	
	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = 
			ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
	
	IUOLMessengerEmoticonSettings3Ptr pEmoticonSettings3;
	
	if (pSettingsManager2)
	{
		pEmoticonSettings3 = ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>(
					pSettingsManager2->GetEmoticonSettings());
	}
	
	if ( (pEmoticonSettings3) && (!strOriginalFilePath.IsEmpty()) )
	{
		// Use relative path if shortcut file path is under default dir path
		CString strDefaultDirPath = pEmoticonSettings3->GetDefaultDirPath();
		
		int nFind = strOriginalFilePath.Find(strDefaultDirPath);
		
		if (0 == nFind)
		{
			strFinalFilePath = strOriginalFilePath.Mid(strDefaultDirPath.GetLength() + 1);
			
			bImported = TRUE;
		}
		else
		{
			CPath path(strOriginalFilePath);
			path.StripPath();
			
			CString strUserSettingsFolder = pEmoticonSettings3->GetUserSettingsFolderPath();
			
			if (FALSE == ATLPath::FileExists(strUserSettingsFolder))
			{
				::CreateDirectory(strUserSettingsFolder, NULL);
			}
			
			CString strCopyFilePath = strUserSettingsFolder + _T("\\") + path.m_strPath;
			
			bImported = ::CopyFile(strOriginalFilePath, strCopyFilePath, FALSE);
			
			strFinalFilePath = strCopyFilePath;
		}
	}
	
	return bImported;
}

