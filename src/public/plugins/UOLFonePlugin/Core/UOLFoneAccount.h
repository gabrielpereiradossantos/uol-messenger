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

#include <interfaces/IUOLMessengerAccount.h>
#include "PhoneCall.h"
#include "InscriptionParser.h"


class CUOLFoneAccount
{
public:
	enum ConnectionState
	{
		ConnectionStateUnknown = 0,
		ConnectionStateOnline,
		ConnectionStateOffline,
		ConnectionStateConnectionInProgress,
		ConnectionStateDisconnectionInProgress,
		ConnectionStateErrorUnauthorized,
		ConnectionStateErrorServerUnavailable,
		ConnectionStateErrorTimeout
	};

	static double UNKNOWN_BALANCE;
	static LONG UNKNOWN_PHONE_LINE_ID;

public:
	CUOLFoneAccount(IUOLMessengerAccountPtr pAccount);
	virtual ~CUOLFoneAccount();

	IUOLMessengerAccountPtr GetIMAccount();

	LONG GetPhoneLineId() const;
	void SetPhoneLineId(LONG lPhoneLineId);

	ConnectionState GetConnectionState() const;
	void SetConnectionState(ConnectionState state);

	BOOL HasInscription() const;
	BOOL HasInscription(CString strInscriptionID) const;
	BOOL HasAnyInscription(CString strInscriptionList) const;
	void SetInscriptions(CString strInscriptionList);

	BOOL HasVoipOutPlan() const;

	CPhoneCallPtr GetCurrentPhoneCall();
	void SetCurrentPhoneCall(CPhoneCallPtr pPhoneCall);

	double GetBalance() const;
	void SetBalance(double dBalance);

	CErrorInfoPtr GetErrorInfo() const;
	void SetErrorInfo(CErrorInfoPtr pErrorInfo);

	CString GetPlanName() const;
	void SetPlanName(CString strName);

	int GetPlanSeconds() const;
	void SetPlanSeconds(int nSeconds);

	int GetPlanUsedSeconds() const;
	void SetPlanUsedSeconds(int nSeconds);
	int GetPlanRemainingSeconds();

private:
	IUOLMessengerAccountPtr		m_pAccount;
	LONG						m_lPhoneLineId;
	ConnectionState				m_connectionState;
	CAtlList<CString>			m_listInscriptions;
	CPhoneCallPtr				m_pCurrentPhoneCall;
	double						m_dBalance;
	CErrorInfoPtr				m_pErrorInfo;
	CString						m_strPlanName;
	int							m_nPlanSeconds;
	int							m_nPlanUsedSeconds;
};

MAKEAUTOPTR(CUOLFoneAccount);