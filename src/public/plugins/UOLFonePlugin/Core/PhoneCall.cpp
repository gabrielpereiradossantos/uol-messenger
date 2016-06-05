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
#include "PhoneCall.h"


CPhoneCall::CPhoneCall()
{
    m_lPhoneCallId = CPhoneCall::UNKNOWN_PHONE_CALL_ID;
	m_phoneCallState = CPhoneCall::PhoneCallStateUnknown;
	m_dCostPerMinute = 0.0;
	m_dTotalCost = 0.0;
	m_dConnectionFee = 0;
	m_bSessionUsingCredits = TRUE;
	//m_lDuration = 0;
}


CPhoneCall::~CPhoneCall()
{
}


LONG CPhoneCall::GetPhoneCallId() const
{
	return m_lPhoneCallId;
}


void CPhoneCall::SetPhoneCallId(LONG lPhoneCallId)
{
	m_lPhoneCallId = lPhoneCallId;
}


CPhoneCall::PhoneCallState CPhoneCall::GetPhoneCallState() const
{
	return m_phoneCallState;
}


void CPhoneCall::SetPhoneCallState(CPhoneCall::PhoneCallState phoneCallState)
{
	m_phoneCallState = phoneCallState;
}


double CPhoneCall::GetCostPerMinute() const
{
	return m_dCostPerMinute;
}


void CPhoneCall::SetCostPerMinute(double dCostPerMinute)
{
	m_dCostPerMinute = dCostPerMinute;
}


double CPhoneCall::GetTotalCost() const
{
	return m_dTotalCost;
}


void CPhoneCall::SetTotalCost(double dTotalCost)
{
	m_dTotalCost = dTotalCost;
}


//ULONG CPhoneCall::GetDuration() const
//{
//	return m_lDuration;
//}
//
//
//void CPhoneCall::SetDuration(ULONG lDuration)
//{
//	m_lDuration = lDuration;
//}


CErrorInfoPtr CPhoneCall::GetErrorInfo() const
{
	return m_pErrorInfo;
}


void CPhoneCall::SetErrorInfo(CErrorInfoPtr pErrorInfo)
{
	m_pErrorInfo = pErrorInfo;
}


double CPhoneCall::GetConnectionFee() const
{
	return m_dConnectionFee;
}


void CPhoneCall::SetConnectionFee(double dConnectionFee)
{
	m_dConnectionFee = dConnectionFee;
}


BOOL CPhoneCall::IsSessionUsingCredits() const
{
	return m_bSessionUsingCredits;
}


void CPhoneCall::SetSessionUsingCredits(BOOL bUsing)
{
	m_bSessionUsingCredits = bUsing;
}