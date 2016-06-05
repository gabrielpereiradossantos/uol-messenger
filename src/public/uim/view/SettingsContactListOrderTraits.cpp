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
#include "SettingsContactListOrderTraits.h"

#include "../controller/UIMApplication.h"


CSettingsContactListOrderTraits::CSettingsContactListOrderTraits() : 
m_dwSortType(IUOLMessengerContactListSettings::STATUS)
{
}


CSettingsContactListOrderTraits::~CSettingsContactListOrderTraits()
{
}


void CSettingsContactListOrderTraits::SetSortType(DWORD dwSortType)
{
	m_dwSortType = dwSortType;
}


DWORD CSettingsContactListOrderTraits::GetSortType() const
{
	return m_dwSortType;
}


int CSettingsContactListOrderTraits::CompareOrder(const IUOLMessengerGroup* pGroup1, const IUOLMessengerGroup* pGroup2) const
{
	if ((pGroup1 != NULL) && (pGroup2 != NULL))
	{
		//if (m_dwSortType & IUOLMessengerContactListSettings::NAME)
		//{
			int nNameResult = (pGroup1->GetName().CompareNoCase(pGroup2->GetName())); 

			if (m_dwSortType & IUOLMessengerContactListSettings::DESCENDING)
			{
				return (- nNameResult);
			}

			return nNameResult;
		//}
	}

	return 0;
}


int CSettingsContactListOrderTraits::CompareOrder(const IUOLMessengerContact* pContact1, const IUOLMessengerContact* pContact2) const
{
	if ((pContact1 != NULL) && (pContact2 != NULL))
	{
		if (m_dwSortType & IUOLMessengerContactListSettings::NAME)
		{
			int nNameResult = CompareContactAlias(pContact1, pContact2);

			if (m_dwSortType & IUOLMessengerContactListSettings::DESCENDING)
			{
				return (- nNameResult);
			}

			return nNameResult;
		}
		else if (m_dwSortType & IUOLMessengerContactListSettings::STATUS)
		{
			int nStatusResult = pContact1->GetStatus() - pContact2->GetStatus();

			if (0 == nStatusResult)
			{
				return CompareContactAlias(pContact1, pContact2);
			}

			if (m_dwSortType & IUOLMessengerContactListSettings::DESCENDING)
			{
				return (- nStatusResult);
			}

			return nStatusResult;
		}

		return pContact1->GetName().CompareNoCase(pContact2->GetName());
	}

	return 0;
}


int CSettingsContactListOrderTraits::CompareOrder(const IUOLMessengerAccount* pAccount1, const IUOLMessengerAccount* pAccount2) const
{
	int nNameResult = 0;
	
	if ( (pAccount1 != NULL) && (pAccount2 != NULL) )
	{
		if (m_dwSortType & IUOLMessengerContactListSettings::STATUS)
		{
			IUOLMessengerAccountPtr spAccount1 = 
					CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(
							pAccount1->GetUserId(), pAccount1->GetProtocolId());
			IUOLMessengerAccountPtr spAccount2 = 
					CUIMApplication::GetApplication()->GetAccountManager()->FindAccount(
							pAccount2->GetUserId(), pAccount2->GetProtocolId());
			
			DWORD dwStatusAccount1 = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetStatus(spAccount1);
			DWORD dwStatusAccount2 = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetStatus(spAccount2);
			
			nNameResult = dwStatusAccount1 - dwStatusAccount2;
		}
		
		if (!nNameResult)
		{
			nNameResult = (pAccount1->GetUserId().CompareNoCase(pAccount2->GetUserId()));
		}
		
		if (m_dwSortType & IUOLMessengerContactListSettings::DESCENDING)
		{
			return (- nNameResult);
		}
		
		return nNameResult;
	}
	
	return 0;
}


int CSettingsContactListOrderTraits::CompareContactAlias(const IUOLMessengerContact* pContact1, const IUOLMessengerContact* pContact2) const
{
	int nResult = pContact1->GetAlias().CompareNoCase(pContact2->GetAlias()); 

	if (0 == nResult)
	{
		nResult = pContact1->GetAccount()->GetProtocolId().Compare(pContact2->GetAccount()->GetProtocolId());

		if (0 == nResult)
		{
			nResult = pContact1->GetName().Compare(pContact2->GetName());
		}
	}

	return nResult;
}
