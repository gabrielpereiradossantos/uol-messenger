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

#include "ProfileList.h"
#include "SettingsTraits.h"
#include "../resource.h"
#include "../Cryptography.h"

CProfileList::CProfileList()
{
	InitTagProcessMap();
}

CProfileList::~CProfileList()
{
}

void CProfileList::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Profile", NEW_SETTINGS_CALLBACK(Profile, this)},
		{"LastSelected", NEW_SETTINGS_CALLBACK(LastSelected, this)},
		{"DefautProfile", NEW_SETTINGS_CALLBACK(DefaultProfile, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;
	}
}

void CProfileList::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}

void CProfileList::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}

void CProfileList::GetProfileList(CAtlList <CString>& listProfile)
{
	for (POSITION p = m_mapProfiles.GetStartPosition(); p;)
	{
		listProfile.AddHead(m_mapProfiles.GetKeyAt(p));
		m_mapProfiles.GetNext(p);
	}
}

CString CProfileList::GetLastSelected()
{
	return this->m_strLastSelected;
}

void CProfileList::SetLastSelected(const CString& strLastSelected)
{
	this->m_strLastSelected = strLastSelected;
}

CString CProfileList::GetDefaultProfile()
{
	return this->m_strDefaultProfile;
}

void CProfileList::SetDefaultProfile(const CString& strDefaultProfile)
{
	this->m_strDefaultProfile = strDefaultProfile;
}

BOOL CProfileList::GetProfileElement(const CString& strName, IUOLMessengerProfileElementPtr& pProfileElement)
{
	CProfileMap::CPair* pPair = m_mapProfiles.Lookup(strName);

	if (pPair != NULL)
	{
		pProfileElement = pPair->m_value;
		return TRUE;
	}

	return FALSE;
}

BOOL CProfileList::AddProfile(const CString& strName, IUOLMessengerProfileElementPtr& pProfileElement)
{
	CProfileMap::CPair* pPair = m_mapProfiles.Lookup(strName);

	if (pPair == NULL)
	{
		this->m_mapProfiles.SetAt(strName, pProfileElement);

		return TRUE;
	}

	return FALSE;
}

BOOL CProfileList::IsProfile(const CString& strName)
{
	CProfileMap::CPair* pPair = m_mapProfiles.Lookup(strName);

	if (pPair == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CProfileList::RemoveProfile(const CString& strName)
{
	CProfileMap::CPair* pPair = m_mapProfiles.Lookup(strName);

	if (pPair != NULL)
	{
		this->m_mapProfiles.RemoveKey(strName);
		if (strName.Compare(m_strLastSelected) == 0)
		{
			m_strLastSelected = "";
		}

		return TRUE;
	}

	return FALSE;
}

IMPLEMENT_SETTINGS_CALLBACK(CProfileList, Profile)
{
	CProfileElementPtr pProfileElement;

	if (bSave)
	{
		for (POSITION pos = m_pThis->m_mapProfiles.GetStartPosition(); pos;)
		{
			CProfileMap::CPair* pPair = m_pThis->m_mapProfiles.GetNext(pos);

			if (pPair != NULL)
			{
				CProfileElementPtr pProfileElement = 
					ap_dynamic_cast<CProfileElementPtr>(pPair->m_value);

				if (pProfileElement)
				{
					MSXML::IXMLDOMNodePtr pChild = CSettingsTraits::OpenGroup(pNode, strKey);
					pProfileElement->Save(pChild, strKey);
				}
			}
		}
	}
	else
	{
		pProfileElement = new CProfileElement();

		if (pProfileElement)
		{
			pProfileElement->Load(pNode, strKey);
			m_pThis->m_mapProfiles.SetAt(pProfileElement->GetProfileName(), pProfileElement);
		}
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CProfileList, LastSelected)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, m_pThis->m_strLastSelected);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, m_pThis->m_strLastSelected);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CProfileList, DefaultProfile)
{
	CString strDefaultProfile;
	CCryptography cryptography(_T("#w%@!-$(*"));
	int nStrLength;

	if (bSave)
	{
		strDefaultProfile = m_pThis->m_strDefaultProfile;
		nStrLength = strDefaultProfile.GetLength();
		cryptography.Encrypt(strDefaultProfile, nStrLength);
		cryptography.EncodeBase64(strDefaultProfile, nStrLength);

		CSettingsTraits::SetStringValue(pNode, strKey, strDefaultProfile);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, strDefaultProfile);
		nStrLength = strDefaultProfile.GetLength();
		cryptography.DecodeBase64(strDefaultProfile, nStrLength);

		if(cryptography.Decrypt(strDefaultProfile, nStrLength))
		{
             m_pThis->m_strDefaultProfile = strDefaultProfile;
		}
	}
}
