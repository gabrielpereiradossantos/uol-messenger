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

#include "../Controller/UIMApplication.h"
#include "ProfileElement.h"
#include "SettingsTraits.h"
#include "../Cryptography.h"
#include <interfaces/IUOLMessengerUserIconCacheManager.h>



CProfileElement::CProfileElement() :
m_strName(""),
m_strPath(""),
m_bRememberPass(FALSE),
m_bAuthentication(FALSE)
{
	InitTagProcessMap();
}


CProfileElement::CProfileElement(CString strName, CString strPath, BOOL bRememberPass, BOOL bAuthentication) : 
m_strName(strName),
m_strPath(strPath),
m_bRememberPass(bRememberPass),
m_bAuthentication(bAuthentication)
{
	InitTagProcessMap();
}

CProfileElement::~CProfileElement(void)
{
}

void CProfileElement::InitTagProcessMap()
{
	init_helper ih[] = 
	{	
		{"Name", NEW_SETTINGS_CALLBACK(Name, this)},
		{"Path", NEW_SETTINGS_CALLBACK(Path, this)},
		{"Password", NEW_SETTINGS_CALLBACK(Password, this)},
		{"RememberPass", NEW_SETTINGS_CALLBACK(RememberPass, this)},
		{"Authentication", NEW_SETTINGS_CALLBACK(Authentication, this)},
		{"IconPath", NEW_SETTINGS_CALLBACK(IconPath, this)},
		{"Nickname", NEW_SETTINGS_CALLBACK(Nickname, this)}
	};

	int nSize = sizeof(ih) / sizeof(ih[0]);

	for (int nInd = 0; nInd < nSize; nInd++)
	{
		ms_tagProcess[ih[nInd].str]= ih[nInd].pCallback;			 
	}	
}

void CProfileElement::Load(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Load(pNode, ms_tagProcess, this);
}

void CProfileElement::Save(MSXML::IXMLDOMNodePtr pNode, const CString& strKey)
{
	CSettingsTraits::Save(pNode, ms_tagProcess, this);
}


CString CProfileElement::GetProfileName()
{
	return m_strName;
}

void CProfileElement::SetProfileName(const CString& strName)
{
	m_strName = strName;
}

CString CProfileElement::GetProfileNickName()
{
	return m_strNickname;
}

void CProfileElement::SetProfileNickname(const CString& strNickname)
{
	m_strNickname = strNickname;
}

IUOLMessengerUserIconPtr CProfileElement::GetUserIcon()
{
	return m_pUserIcon;
}

void CProfileElement::SetUserIcon(IUOLMessengerUserIconPtr pUserIcon)
{
	m_pUserIcon = pUserIcon;
}

void CProfileElement::SetProfilePassword(const CString& strPassword)
{
    m_strPassword = strPassword;
}

void CProfileElement::SetEnableAuthentication(BOOL bAuthentication)
{
    m_bAuthentication = bAuthentication;
}

void CProfileElement::SetEnableRememberPass(BOOL bRememberPass)
{
    m_bRememberPass = bRememberPass;
}

void CProfileElement::SetProfilePath(const CString& strPath)
{
	m_strPath = strPath;
}

BOOL CProfileElement::IsEnableAuthentication()
{
	return m_bAuthentication;
}

BOOL CProfileElement::IsEnableRememberPass()
{
	return m_bRememberPass;
}

CString CProfileElement::GetProfilePassword()
{
	return m_strPassword;
}

CString CProfileElement::GetProfilePath()
{
	return m_strPath;
}

IMPLEMENT_SETTINGS_CALLBACK(CProfileElement, Name)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, (CString)m_pThis->m_strName);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, (CString&)m_pThis->m_strName);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CProfileElement, Nickname)
{
	if (bSave)
	{
		CSettingsTraits::SetStringValue(pNode, strKey, (CString)m_pThis->m_strNickname);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, (CString&)m_pThis->m_strNickname);
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CProfileElement, IconPath)
{
	CString strIconPath;

	if (bSave)
	{
		if (m_pThis->m_pUserIcon != NULL)
		{
			strIconPath = m_pThis->m_pUserIcon->GetImagePath();
		}

		CSettingsTraits::SetStringValue(pNode, strKey, strIconPath);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, strIconPath);

		IUOLMessengerUserIconCacheManagerPtr pUserIconCacheManager = CUIMApplication::GetApplication()->GetUserIconCacheManager();

		if ((pUserIconCacheManager != NULL) && (!strIconPath.IsEmpty()))
		{
			IUOLMessengerUserIconPtr pUserIcon = pUserIconCacheManager->RegisterCachedImage(m_pThis, strIconPath);

			if (pUserIcon != NULL)
			{
				m_pThis->m_pUserIcon = pUserIcon;
			}
		}
	}
}

IMPLEMENT_SETTINGS_CALLBACK(CProfileElement, Path)
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

IMPLEMENT_SETTINGS_CALLBACK(CProfileElement, Password)
{
	CString strPass;
	CCryptography cryptography;
	int nStrLength;

	if (bSave)
	{
		strPass = m_pThis->m_strPassword;
		nStrLength = strPass.GetLength();
		cryptography.Encrypt(strPass, nStrLength);
		cryptography.EncodeBase64(strPass, nStrLength);

		CSettingsTraits::SetStringValue(pNode, strKey, strPass);
	}
	else
	{
		CSettingsTraits::GetStringValue(pNode, strKey, strPass);
		nStrLength = strPass.GetLength();
		cryptography.DecodeBase64(strPass, nStrLength);

		if(cryptography.Decrypt(strPass, nStrLength))
		{
			m_pThis->m_strPassword = strPass;
		}
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CProfileElement, RememberPass)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bRememberPass);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bRememberPass);
	}
}


IMPLEMENT_SETTINGS_CALLBACK(CProfileElement, Authentication)
{
	if (bSave)
	{
		CSettingsTraits::SetBOOLValue(pNode, strKey, m_pThis->m_bAuthentication);
	}
	else
	{
		CSettingsTraits::GetBOOLValue(pNode, strKey, m_pThis->m_bAuthentication);
	}
}
