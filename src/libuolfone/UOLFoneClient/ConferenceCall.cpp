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
#include "ConferenceCall.h"


CConferenceCall::CConferenceCall()
{
}


CConferenceCall::~CConferenceCall()
{
}


STDMETHODIMP CConferenceCall::GetConferenceId(LONG* plConferenceCallId)
{
	ATLASSERT(plConferenceCallId != NULL);
	
	if (plConferenceCallId != NULL)
	{
		*plConferenceCallId = GetConferenceId();
		
		return S_OK;
	}
	
	return E_FAIL;
}


STDMETHODIMP CConferenceCall::GetNumberOfParticipants(LONG* plNumberOfParticipants)
{
	ATLASSERT(plNumberOfParticipants != NULL);
	
	if (plNumberOfParticipants != NULL)
	{
		*plNumberOfParticipants = GetNumberOfParticipants();
		
		return S_OK;
	}
	
	return E_FAIL;
}


STDMETHODIMP CConferenceCall::GetParticipantCallId(LONG lParticipantIndex, LONG* lpParticipantCallId)
{
	if (lpParticipantCallId != NULL)
	{
		*lpParticipantCallId = GetParticipantCallId(lParticipantIndex);
		
		return S_OK;
	}
	
	return E_FAIL;
}


void CConferenceCall::SetConferenceId(LONG lConferenceId)
{
	m_lConferenceId = lConferenceId;
}


LONG CConferenceCall::AddParticipant(LONG lParticipantCallId)
{
	m_mapParticipantCallIds[++m_lNumberOfParticipants] = lParticipantCallId;
	
	return m_lNumberOfParticipants;
}


LONG CConferenceCall::RemoveParticipant(LONG lParticipantCallId)
{
	LONG lParticipantIndex  = InternalGetParticipantIndex(lParticipantCallId);
	
	if (lParticipantIndex != UFC_CONF_CONST_INVALID_PARTICIPANT_INDEX)
	{
		BOOL bRet = m_mapParticipantCallIds.RemoveKey(lParticipantIndex);
		ATLASSERT(bRet);
		
		m_lNumberOfParticipants--;
	}
	
	return lParticipantIndex;
}


LONG CConferenceCall::FindParticipant(LONG lParticipantCallId)
{
	return InternalGetParticipantIndex(lParticipantCallId);
}


LONG CConferenceCall::GetConferenceId()
{
	return m_lConferenceId;
}


LONG CConferenceCall::GetNumberOfParticipants()
{
	return m_lNumberOfParticipants;
}


LONG CConferenceCall::GetParticipantCallId(LONG lParticipantIndex)
{
	return InternalGetParticipantCallId(lParticipantIndex);
}


LONG CConferenceCall::InternalGetParticipantCallId(LONG lParticipantIndex)
{
	CParticipantCallIdMap::CPair* pPair = m_mapParticipantCallIds.Lookup(lParticipantIndex);
	
	if (pPair)
	{
		return pPair->m_value;
	}
	
	return UFC_CONF_CONST_INVALID_PHONE_CALL_ID;
}


LONG CConferenceCall::InternalGetParticipantIndex(LONG lParticipantCallId)
{
	CParticipantCallIdMap::CPair* pPair = NULL;
	LONG lParticipantIndex = UFC_CONF_CONST_INVALID_PARTICIPANT_INDEX;
	
	for (POSITION pos = m_mapParticipantCallIds.GetStartPosition(); pos; )
	{
		pPair = m_mapParticipantCallIds.GetNext(pos);
		
		if (pPair->m_value == lParticipantCallId)
		{
			lParticipantIndex = pPair->m_key;
		}
	}
	
	return lParticipantIndex;
}

