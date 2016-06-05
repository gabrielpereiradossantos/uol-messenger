/* UOL Fone
 * Copyright (c) 2006 Universo Online S/A
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
 * Universo Online S/A - A/C: UOL Fone 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */


#include "stdafx.h"
#include "PhoneCall.h"


CPhoneCall::CPhoneCall()
{
}


CPhoneCall::~CPhoneCall()
{
}


STDMETHODIMP CPhoneCall::GetPhoneLine(IUOLFoneClientPhoneLine** ppPhoneLine)
{
	ATLASSERT(ppPhoneLine != NULL);
	if (ppPhoneLine != NULL)
	{
		*ppPhoneLine = m_spPhoneLine;
		(*ppPhoneLine)->AddRef();

		return S_OK;
	}

	return E_FAIL;
}


STDMETHODIMP CPhoneCall::GetPhoneURI(BSTR* pbstrPhoneURI)
{
	ATLASSERT(pbstrPhoneURI != NULL);
	if (pbstrPhoneURI != NULL)
	{
		*pbstrPhoneURI = GetPhoneURI().AllocSysString();

		return S_OK;
	}

	return E_FAIL;
}


STDMETHODIMP CPhoneCall::IsVideoEnabled(BOOL* pbEnabled)
{
	ATLASSERT(pbEnabled != NULL);
	if (pbEnabled != NULL)
	{
		*pbEnabled = IsVideoEnabled();

		return S_OK;
	}

	return E_FAIL;
}


STDMETHODIMP CPhoneCall::GetPhoneCallId(LONG* plCallId)
{
	ATLASSERT(plCallId != NULL);
	if (plCallId != NULL)
	{
		*plCallId = GetPhoneCallId();

		return S_OK;
	}

	return E_FAIL;
}


STDMETHODIMP CPhoneCall::GetPhoneCallState(LONG* plCallState)
{
	ATLASSERT(plCallState != NULL);
	if (plCallState != NULL)
	{
		*plCallState = GetPhoneCallState();

		return S_OK;
	}

	return E_FAIL;
}


STDMETHODIMP CPhoneCall::SetPhoneLine(IUOLFoneClientPhoneLine* pPhoneLine)
{
	ATLASSERT(pPhoneLine != NULL);
	if (pPhoneLine != NULL)
	{
		m_spPhoneLine = pPhoneLine;

		return S_OK;
	}

	return E_FAIL;
}


void CPhoneCall::SetPhoneURI(const CString& strPhoneURI)
{
	m_strPhoneURI = strPhoneURI;
}


void CPhoneCall::EnableVideo(BOOL bEnable)
{
	m_bEnableVideo = bEnable;
}


void CPhoneCall::SetPhoneCallId(LONG lCallId)
{
	m_lCallId = lCallId;
}


void CPhoneCall::SetPhoneCallState(LONG lCallState)
{
	m_lCallState = lCallState;
}


CString CPhoneCall::GetPhoneURI()
{
	return m_strPhoneURI;
}


BOOL CPhoneCall::IsVideoEnabled()
{
	return m_bEnableVideo;
}


LONG CPhoneCall::GetPhoneCallId()
{
	return m_lCallId;
}


LONG CPhoneCall::GetPhoneCallState()
{
	return m_lCallState;
}