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

#include <interfaces/IUOLMessengerImage.h>
#include "ErrorInfo.h"


class CPhoneCall
{
public:
	enum
	{
		UNKNOWN_PHONE_CALL_ID = -1
	};

	enum PhoneCallState
	{
		PhoneCallStateUnknown = 0,
		PhoneCallStateError,
		PhoneCallStateResumed,
		PhoneCallStateTalking,
		PhoneCallStateDialing,
		PhoneCallStateRinging,
		PhoneCallStateClosed,
		PhoneCallStateIncoming,
		PhoneCallStateHold,
		PhoneCallStateBusy,
		PhoneCallStateNoAnswer,
		PhoneCallStateRedirected
	};
		
public:
	CPhoneCall();
	virtual ~CPhoneCall();

	LONG GetPhoneCallId() const;
	void SetPhoneCallId(LONG lPhoneCallId);

	PhoneCallState GetPhoneCallState() const;
	void SetPhoneCallState(PhoneCallState phoneCallState);

	double GetCostPerMinute() const;
	void SetCostPerMinute(double dCostPerMinute);

	double GetTotalCost() const;
	void SetTotalCost(double dTotalCost);

	// Duration in seconds
	//ULONG GetDuration() const;
	//void SetDuration(ULONG lDuration);

	CErrorInfoPtr GetErrorInfo() const;
	void SetErrorInfo(CErrorInfoPtr pErrorInfo);

	double GetConnectionFee() const;
	void SetConnectionFee(double dConnectionFee);

	BOOL IsSessionUsingCredits() const;
	void SetSessionUsingCredits(BOOL bUsing);

private:
	LONG		m_lPhoneCallId;
	PhoneCallState	m_phoneCallState;
	double			m_dCostPerMinute;
	double			m_dTotalCost;
	ULONG			m_lDuration;
	CErrorInfoPtr	m_pErrorInfo;
	double			m_dConnectionFee;
	BOOL			m_bSessionUsingCredits;
};

MAKEAUTOPTR(CPhoneCall);