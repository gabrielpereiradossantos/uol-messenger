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

#include "interfaces/IUOLFoneClientPhoneCall.h"


class CPhoneCall :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IUOLFoneClientPhoneCall>
{
public:
	CPhoneCall();
	virtual ~CPhoneCall();

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_bEnableVideo = FALSE;
		m_lCallId = -1;

		return S_OK;
	}
	
	void FinalRelease() 
	{
		if (m_spPhoneLine)
		{
			m_spPhoneLine.Release();
		}
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

	BEGIN_COM_MAP(CPhoneCall)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IUOLFoneClientPhoneCall)
	END_COM_MAP()

	STDMETHOD(GetPhoneLine)(IUOLFoneClientPhoneLine** ppPhoneLine);
	STDMETHOD(GetPhoneURI)(BSTR* pbstrPhoneURI);
	STDMETHOD(IsVideoEnabled)(BOOL* pbEnabled);
	STDMETHOD(GetPhoneCallId)(LONG* plCallId);
	STDMETHOD(GetPhoneCallState)(LONG* plCallState);

	STDMETHOD(SetPhoneLine)(IUOLFoneClientPhoneLine* pPhoneLine);
	void SetPhoneURI(const CString& strPhoneURI);
	void EnableVideo(BOOL bEnable);
	void SetPhoneCallId(LONG lCallId);
	void SetPhoneCallState(LONG lCallState);

	CString GetPhoneURI();
	BOOL IsVideoEnabled();
	LONG GetPhoneCallId();
	LONG GetPhoneCallState();

private:
	CComPtr<IUOLFoneClientPhoneLine>	m_spPhoneLine;
	CString		m_strPhoneURI;
	BOOL		m_bEnableVideo;
    LONG		m_lCallId;
	LONG		m_lCallState;
};