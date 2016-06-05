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

#include "../../_UOLFoneClient.h"



class CPhoneLineConfig :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IUOLFoneClientPhoneLineConfig>
{
public:
	enum
	{
		DEFAULT_CONNECTION_PORT = 5060
	};

public:
	CPhoneLineConfig();
	virtual ~CPhoneLineConfig();

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_lConnectionPort = DEFAULT_CONNECTION_PORT;

		return S_OK;
	}
	
	void FinalRelease() 
	{
		if (m_spServerNetworkConfig)
		{
			m_spServerNetworkConfig.Release();
		}

		if (m_spMediaNetworkConfig)
		{
			m_spMediaNetworkConfig.Release();
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

	BEGIN_COM_MAP(CPhoneLineConfig)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IUOLFoneClientPhoneLineConfig)
	END_COM_MAP()

	STDMETHOD(GetConnectionPort)(LONG* plPort);
	STDMETHOD(GetServerNetworkConfig)(IUOLFoneClientNetworkConfig** ppNetworkConfig);
	STDMETHOD(GetMediaNetworkConfig)(IUOLFoneClientNetworkConfig** ppNetworkConfig);
	STDMETHOD(GetAudioCodecsPrecedence)(BSTR* pbstrAudioCodecsPrecedence);
	STDMETHOD(GetVideoCodecsPrecedence)(BSTR* pbstrVideoCodecsPrecedence);

	void SetConnectionPort(LONG lPort);
	STDMETHOD(SetServerNetworkConfig)(IUOLFoneClientNetworkConfig* pNetworkConfig);
	STDMETHOD(SetMediaNetworkConfig)(IUOLFoneClientNetworkConfig* pNetworkConfig);
	void SetAudioCodecsPrecedence(const CString& strAudioCodecsPrecedence);
	void SetVideoCodecsPrecedence(const CString& strVideoCodecsPrecedence);

	LONG GetConnectionPort();
	CString GetAudioCodecsPrecedence();
	CString GetVideoCodecsPrecedence();

private:
	LONG	m_lConnectionPort;
	CComPtr<IUOLFoneClientNetworkConfig>	m_spServerNetworkConfig;
	CComPtr<IUOLFoneClientNetworkConfig>	m_spMediaNetworkConfig;
	CString		m_strAudioCodecsPrecedence;
	CString		m_strVideoCodecsPrecedence;
};