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


class CConnectionTestParam :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IUOLFoneClientConnectionTestParam>
{
public:
	CConnectionTestParam();
	virtual ~CConnectionTestParam();

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		m_lPort = 0;
		m_lServerResponseTimeout = 3;
		m_lHttpTunnelResponseTimeout = 6;
		m_lHttpTunnelMode = UFC_HTTP_TUNNEL_MODE_CONNECT;

		return S_OK;
	}
	
	void FinalRelease() 
	{
		if (m_spServerInfo)
		{
			m_spServerInfo.Release();
		}

		if (m_spHttpTunnelInfo)
		{
			m_spHttpTunnelInfo.Release();
		}

		if (m_spHttpProxyInfo)
		{
			m_spHttpProxyInfo.Release();
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

	BEGIN_COM_MAP(CConnectionTestParam)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY(IUOLFoneClientConnectionTestParam)
	END_COM_MAP()

	STDMETHOD(GetServerInfo)(IUOLFoneClientServerInfo** ppServerInfo);
	STDMETHOD(GetLocalPort)(LONG* plPort);
	STDMETHOD(GetHttpTunnelInfo)(IUOLFoneClientServerInfo** ppHttpTunnelInfo);
	STDMETHOD(GetHttpProxyInfo)(IUOLFoneClientServerInfo** ppHttpProxyInfo);
	STDMETHOD(GetServerResponseTimeout)(LONG* plTimeout);
	STDMETHOD(GetHttpTunnelResponseTimeout)(LONG* plTimeout);
	STDMETHOD(GetHttpTunnelMode)(LONG* plMode);

	STDMETHOD(SetServerInfo)(IUOLFoneClientServerInfo* pServerInfo);
	STDMETHOD(SetLocalPort)(LONG lPort);
	STDMETHOD(SetHttpTunnelInfo)(IUOLFoneClientServerInfo* pHttpTunnelInfo);
	STDMETHOD(SetHttpProxyInfo)(IUOLFoneClientServerInfo* pHttpProxyInfo);
	STDMETHOD(SetServerResponseTimeout)(LONG lTimeout);
	STDMETHOD(SetHttpTunnelResponseTimeout)(LONG lTimeout);
	STDMETHOD(SetHttpTunnelMode)(LONG lMode);

private:
	CComPtr<IUOLFoneClientServerInfo>	m_spServerInfo;
	LONG								m_lPort;
	CComPtr<IUOLFoneClientServerInfo>	m_spHttpTunnelInfo;
	CComPtr<IUOLFoneClientServerInfo>	m_spHttpProxyInfo;
	LONG								m_lServerResponseTimeout;
	LONG								m_lHttpTunnelResponseTimeout;
	LONG								m_lHttpTunnelMode;
};