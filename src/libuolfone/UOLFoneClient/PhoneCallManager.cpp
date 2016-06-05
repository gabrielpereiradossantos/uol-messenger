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
#include "PhoneCallManager.h"


CPhoneCallManager::CPhoneCallManager()
{
}


CPhoneCallManager::~CPhoneCallManager()
{
}


IUOLFoneClientPhoneCall* CPhoneCallManager::AddPhoneCall(IUOLFoneClientPhoneLine* pPhoneLine, const CString& strPhoneURI, BOOL bEnableVideo, LONG lPhoneCallId)
{
	IUOLFoneClientPhoneCall* pPhoneCall = NULL;

	CComObject<CPhoneCall>* pFindPhoneCallObj = InternalGetPhoneCall(lPhoneCallId);

	if (!pFindPhoneCallObj)
	{
		CComObject<CPhoneCall>* pPhoneCallObj = NULL;

		HRESULT hr = CComObject<CPhoneCall>::CreateInstance(&pPhoneCallObj);

		if (SUCCEEDED(hr))
		{
			pPhoneCallObj->SetPhoneLine(pPhoneLine);
			pPhoneCallObj->SetPhoneURI(strPhoneURI);
			pPhoneCallObj->EnableVideo(bEnableVideo);
			pPhoneCallObj->SetPhoneCallId(lPhoneCallId);
			pPhoneCallObj->SetPhoneCallState(UFC_PHONE_CALL_STATE_CLOSED);

			pPhoneCallObj->AddRef();
			m_mapPhoneCalls.SetAt(lPhoneCallId, pPhoneCallObj);

			pPhoneCall = pPhoneCallObj;
			pPhoneCall->AddRef();
		}
	}

	return pPhoneCall;
}


IUOLFoneClientPhoneCall* CPhoneCallManager::GetPhoneCall(LONG lPhoneCallId)
{
	IUOLFoneClientPhoneCall* pPhoneCall = NULL;

	CComObject<CPhoneCall>* pPhoneCallObj = InternalGetPhoneCall(lPhoneCallId);

	if (pPhoneCallObj)
	{
		pPhoneCall = pPhoneCallObj;
		pPhoneCall->AddRef();
	}

	return pPhoneCall;
}


IUOLFoneClientPhoneCall* CPhoneCallManager::RemovePhoneCall(LONG lPhoneCallId)
{
	IUOLFoneClientPhoneCall* pPhoneCall = NULL;
		
	CComObject<CPhoneCall>* pPhoneCallObj = InternalGetPhoneCall(lPhoneCallId);

	if (pPhoneCallObj != NULL)
	{
		BOOL bRet = m_mapPhoneCalls.RemoveKey(lPhoneCallId);
		ATLASSERT(bRet);

		pPhoneCall = pPhoneCallObj;
		pPhoneCall->AddRef();

		pPhoneCallObj->Release();
	}

	return pPhoneCall;
}


void CPhoneCallManager::RemoveAllPhoneCalls()
{
	CPhoneCallMap::CPair* pPair;

	for (POSITION pos = m_mapPhoneCalls.GetStartPosition(); pos; )
	{
		pPair = m_mapPhoneCalls.GetNext(pos);

		CComObject<CPhoneCall>* pPhoneCallObj = pPair->m_value;
		pPhoneCallObj->Release();
	}

	m_mapPhoneCalls.RemoveAll();
}


UINT CPhoneCallManager::GetPhoneCallCount()
{
	return (UINT) m_mapPhoneCalls.GetCount();
}


UINT CPhoneCallManager::GetPhoneCallCount(EnumPhoneCallState state)
{
	UINT nCount = 0;

	CPhoneCallMap::CPair* pPair;

	for (POSITION pos = m_mapPhoneCalls.GetStartPosition(); pos; )
	{
		pPair = m_mapPhoneCalls.GetNext(pos);

		CComObject<CPhoneCall>* pPhoneCallObj = pPair->m_value;

		if (pPhoneCallObj->GetPhoneCallState() == state)
		{
			nCount++;
		}
	}

	return nCount;
}


void CPhoneCallManager::SetPhoneCallState(LONG lPhoneCallId, EnumPhoneCallState state)
{	
	CComObject<CPhoneCall>* pPhoneCallObj = InternalGetPhoneCall(lPhoneCallId);

	if (pPhoneCallObj != NULL)
	{
		pPhoneCallObj->SetPhoneCallState(state);
	}
}


void CPhoneCallManager::SetPhoneCallURI(LONG lPhoneCallId, const CString& strPhoneURI)
{
	CComObject<CPhoneCall>* pPhoneCallObj = InternalGetPhoneCall(lPhoneCallId);

	if (pPhoneCallObj != NULL)
	{
		pPhoneCallObj->SetPhoneURI(strPhoneURI);
	}
}


CComObject<CPhoneCall>* CPhoneCallManager::InternalGetPhoneCall(LONG lPhoneCallId)
{
	CComObject<CPhoneCall>* pPhoneCallObj = NULL;

	CPhoneCallMap::CPair* pPair = m_mapPhoneCalls.Lookup(lPhoneCallId);

	if (pPair)
	{
		pPhoneCallObj = pPair->m_value;
	}

	return pPhoneCallObj;
}
