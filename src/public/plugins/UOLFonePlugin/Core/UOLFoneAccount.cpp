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
#include "UOLFoneAccount.h"
#include "InscriptionParser.h"
#include "../FacadeWrapper.h"


double CUOLFoneAccount::UNKNOWN_BALANCE = -1.0;
LONG CUOLFoneAccount::UNKNOWN_PHONE_LINE_ID = -1;


CUOLFoneAccount::CUOLFoneAccount(IUOLMessengerAccountPtr pAccount)
{
	m_pAccount = pAccount;
	m_dBalance = CUOLFoneAccount::UNKNOWN_BALANCE;
	m_lPhoneLineId = CUOLFoneAccount::UNKNOWN_PHONE_LINE_ID;
	m_strPlanName = "";
	m_nPlanSeconds = 0;
	m_nPlanUsedSeconds = 0;
	
	m_connectionState = ConnectionStateUnknown;
}


CUOLFoneAccount::~CUOLFoneAccount()
{
	m_listInscriptions.RemoveAll();
}


IUOLMessengerAccountPtr CUOLFoneAccount::GetIMAccount()
{
	return m_pAccount;
}


LONG CUOLFoneAccount::GetPhoneLineId() const
{
	return m_lPhoneLineId;
}


void CUOLFoneAccount::SetPhoneLineId(LONG lPhoneLineId)
{
	m_lPhoneLineId = lPhoneLineId;
}


CUOLFoneAccount::ConnectionState CUOLFoneAccount::GetConnectionState() const
{
	return m_connectionState;
}


void CUOLFoneAccount::SetConnectionState(CUOLFoneAccount::ConnectionState state)
{
	m_connectionState = state;
}


BOOL CUOLFoneAccount::HasInscription() const
{
	return (m_listInscriptions.GetCount() > 0);
}


BOOL CUOLFoneAccount::HasInscription(CString strInscriptionID) const
{
	POSITION pos = m_listInscriptions.Find(strInscriptionID);
	
	return (pos != NULL);
}


BOOL CUOLFoneAccount::HasAnyInscription(CString strInscriptionList) const
{
	CAtlList<CString> inscriptionList;

	CInscriptionParser parser;
	parser.ParseInscriptions(strInscriptionList, _T(";"), inscriptionList);

	for (POSITION listPos = inscriptionList.GetHeadPosition(); listPos;)
	{
		CString strInscriptionID = inscriptionList.GetNext(listPos);
		
		POSITION pos = m_listInscriptions.Find(strInscriptionID);
		if (pos != NULL)
		{
			return TRUE;
		}
	}
	
	return FALSE;
}


void CUOLFoneAccount::SetInscriptions(CString strInscriptionList)
{
	m_listInscriptions.RemoveAll();
	
	CInscriptionParser parser;
	parser.ParseInscriptions(strInscriptionList, _T(";"), m_listInscriptions);
}


BOOL CUOLFoneAccount::HasVoipOutPlan() const
{
	CString strVoipOutPlanIDs;

	strVoipOutPlanIDs = CFacadeWrapper::GetInstance()->GetVoipOutPlanIDs();
	return HasAnyInscription(strVoipOutPlanIDs);
}


CPhoneCallPtr CUOLFoneAccount::GetCurrentPhoneCall()
{
	return m_pCurrentPhoneCall;
}


void CUOLFoneAccount::SetCurrentPhoneCall(CPhoneCallPtr pPhoneCall)
{
	m_pCurrentPhoneCall = pPhoneCall;
}


double CUOLFoneAccount::GetBalance() const
{
	return m_dBalance;
}


void CUOLFoneAccount::SetBalance(double dBalance)
{
	m_dBalance = dBalance;
}


CErrorInfoPtr CUOLFoneAccount::GetErrorInfo() const
{
	return m_pErrorInfo;
}


void CUOLFoneAccount::SetErrorInfo(CErrorInfoPtr pErrorInfo)
{
	m_pErrorInfo = pErrorInfo;
}


CString CUOLFoneAccount::GetPlanName() const
{
	return m_strPlanName;
}


void CUOLFoneAccount::SetPlanName(CString strName)
{
	m_strPlanName = strName;
}


int CUOLFoneAccount::GetPlanSeconds() const
{
	return m_nPlanSeconds;
}


void CUOLFoneAccount::SetPlanSeconds(int nSeconds)
{
	m_nPlanSeconds = nSeconds;
}


int CUOLFoneAccount::GetPlanUsedSeconds() const
{
	return m_nPlanUsedSeconds;
}


void CUOLFoneAccount::SetPlanUsedSeconds(int nSeconds)
{
	m_nPlanUsedSeconds = nSeconds;
}


int CUOLFoneAccount::GetPlanRemainingSeconds()
{
	int nRemainingSeconds = 0;
	if (m_nPlanSeconds >= m_nPlanUsedSeconds)
	{
		nRemainingSeconds = m_nPlanSeconds - m_nPlanUsedSeconds;
	}

	return nRemainingSeconds;
}