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

#include "stdafx.h"
#include "PrivacyInfo.h"


CPrivacyInfo::CPrivacyInfo(IUOLMessengerAccountPtr pAccount) :
	m_pAccount(pAccount)
{
}


CPrivacyInfo::~CPrivacyInfo()
{
}


IUOLMessengerAccountPtr CPrivacyInfo::GetAccount()
{
	return m_pAccount;
}


void CPrivacyInfo::AddDenied(const CString& strContact)
{
	POSITION pos = m_listDenied.Find(strContact);

	if (!pos)
	{
		m_listDenied.AddTail(strContact);
	}
}


void CPrivacyInfo::AddPermited(const CString& strContact)
{
	POSITION pos = m_listPermited.Find(strContact);

	if (!pos)
	{
		m_listPermited.AddTail(strContact);
	}
}


void CPrivacyInfo::RemoveDenied(const CString& strContact)
{
	POSITION pos = m_listDenied.Find(strContact);

	if (pos)
	{
		m_listDenied.RemoveAt(pos);
	}
}


void CPrivacyInfo::RemovePermited(const CString& strContact)
{
	POSITION pos = m_listPermited.Find(strContact);

	if (pos)
	{
		m_listPermited.RemoveAt(pos);
	}
}


void CPrivacyInfo::GetDeniedList(CAtlList<CString>& listContacts)
{
	listContacts.AddTailList(&m_listDenied);
}


void CPrivacyInfo::GetPermitedList(CAtlList<CString>& listContacts)
{
	listContacts.AddTailList(&m_listPermited);
}


void CPrivacyInfo::RemoveAllDenied()
{
	m_listDenied.RemoveAll();
}


void CPrivacyInfo::RemoveAllPermited()
{
	m_listPermited.RemoveAll();
}


BOOL CPrivacyInfo::IsDenied(const CString& strContact)
{
	POSITION pos = m_listDenied.Find(strContact);

	return (pos != NULL);
}


BOOL CPrivacyInfo::IsPermited(const CString& strContact)
{
	POSITION pos = m_listPermited.Find(strContact);

	return (pos != NULL);
}