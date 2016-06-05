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


#pragma once


#include "interfaces/IUOLFoneClientConferenceCall.h"


class CConferenceCall :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IUOLFoneClientConferenceCall>
{
public:
	
	CConferenceCall();
	virtual ~CConferenceCall();
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	
	HRESULT FinalConstruct()
	{
		m_lConferenceId = -1;
		m_lNumberOfParticipants = 0;
		
		return S_OK;
	}
	
	void FinalRelease() 
	{
		m_mapParticipantCallIds.RemoveAll();
	}
	
//	ULONG InternalAddRef()
//	{
//		ULONG l = __super::InternalAddRef();
//
//#ifdef _DEBUG
//		CString str;
//		str.Format("%s %i\n", __FUNCTION__, l);
//
//		::OutputDebugString(str);
//#endif
//		return l;
//	}
//
//	ULONG InternalRelease()
//	{
//		ULONG l = __super::InternalRelease();
//
//#ifdef _DEBUG
//		CString str;
//		str.Format("%s %i\n", __FUNCTION__, l);
//
//		::OutputDebugString(str);
//#endif	
//
//		return l;
//	}
	
	BEGIN_COM_MAP(CConferenceCall)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IUOLFoneClientConferenceCall)
	END_COM_MAP()
	
	STDMETHOD(GetConferenceId)(LONG* plConferenceCallId);
	STDMETHOD(GetNumberOfParticipants)(LONG* plNumberOfParticipants);
	STDMETHOD(GetParticipantCallId)(LONG lParticipantIndex, LONG* lpParticipantCallId);
	
	void SetConferenceId(LONG lConferenceId);
	LONG AddParticipant(LONG lParticipantCallId);
	LONG RemoveParticipant(LONG lParticipantCallId);
	LONG FindParticipant(LONG lParticipantCallId);
	
	LONG GetConferenceId();
	LONG GetNumberOfParticipants();
	LONG GetParticipantCallId(LONG lParticipantIndex);
	
private:
	
	LONG InternalGetParticipantCallId(LONG lParticipantIndex);
	LONG InternalGetParticipantIndex(LONG lParticipantCallId);
	
private:
	
	LONG					m_lConferenceId;
	LONG					m_lNumberOfParticipants;
	
	typedef CAtlMap<LONG, LONG> CParticipantCallIdMap;
	
	CParticipantCallIdMap	m_mapParticipantCallIds;
};

