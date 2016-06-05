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
#include "ConnectionTestParam.h"


CConnectionTestParam::CConnectionTestParam()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
}


CConnectionTestParam::~CConnectionTestParam()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
}


STDMETHODIMP CConnectionTestParam::GetServerInfo(IUOLFoneClientServerInfo** ppServerInfo)
{
	if (ppServerInfo != NULL)
	{
		*ppServerInfo = m_spServerInfo;
		
		if (*ppServerInfo != NULL)
		{
			(*ppServerInfo)->AddRef();
		}

		return S_OK;
	}

	return E_FAIL;
}


STDMETHODIMP CConnectionTestParam::GetLocalPort(LONG* plPort)
{
	if (plPort)
	{
		*plPort = m_lPort;
		return S_OK;
	}

	return E_FAIL;
}


STDMETHODIMP CConnectionTestParam::GetHttpTunnelInfo(IUOLFoneClientServerInfo** ppHttpTunnelInfo)
{
	if (ppHttpTunnelInfo != NULL)
	{
		*ppHttpTunnelInfo = m_spHttpTunnelInfo;
		
		if (*ppHttpTunnelInfo != NULL)
		{
			(*ppHttpTunnelInfo)->AddRef();
		}

		return S_OK;
	}

	return E_FAIL;
}


STDMETHODIMP CConnectionTestParam::GetHttpProxyInfo(IUOLFoneClientServerInfo** ppHttpProxyInfo)
{
	if (ppHttpProxyInfo != NULL)
	{
		*ppHttpProxyInfo = m_spHttpProxyInfo;

		if (*ppHttpProxyInfo != NULL)
		{
			(*ppHttpProxyInfo)->AddRef();
		}

		return S_OK;
	}

	return E_FAIL;
}


STDMETHODIMP CConnectionTestParam::GetServerResponseTimeout(LONG* plTimeout)
{
	if (plTimeout != NULL)
	{
		*plTimeout = m_lServerResponseTimeout;
		
		return S_OK;
	}

	return E_POINTER;
}


STDMETHODIMP CConnectionTestParam::GetHttpTunnelResponseTimeout(LONG* plTimeout)
{
	if (plTimeout != NULL)
	{
		*plTimeout = m_lHttpTunnelResponseTimeout;
		
		return S_OK;
	}

	return E_POINTER;
}


STDMETHODIMP CConnectionTestParam::GetHttpTunnelMode(LONG* plMode)
{
	if (plMode != NULL)
	{
		*plMode = m_lHttpTunnelMode;
		
		return S_OK;
	}

	return E_POINTER;
}


STDMETHODIMP CConnectionTestParam::SetServerInfo(IUOLFoneClientServerInfo* pServerInfo)
{
	if (pServerInfo != NULL)
	{
		m_spServerInfo = pServerInfo;
	}
	else
	{
		m_spServerInfo.Release();
	}

	return S_OK;
}


STDMETHODIMP CConnectionTestParam::SetLocalPort(LONG lPort)
{
	m_lPort = lPort;
	return S_OK;
}


STDMETHODIMP CConnectionTestParam::SetHttpTunnelInfo(IUOLFoneClientServerInfo* pHttpTunnelInfo)
{
	if (pHttpTunnelInfo != NULL)
	{
		m_spHttpTunnelInfo = pHttpTunnelInfo;
	}
	else
	{
		m_spHttpTunnelInfo.Release();
	}

	return S_OK;
}


STDMETHODIMP CConnectionTestParam::SetHttpProxyInfo(IUOLFoneClientServerInfo* pHttpProxyInfo)
{
	if (pHttpProxyInfo != NULL)
	{
		m_spHttpProxyInfo = pHttpProxyInfo;
	}
	else
	{
		m_spHttpProxyInfo.Release();		
	}

	return S_OK;
}


STDMETHODIMP CConnectionTestParam::SetServerResponseTimeout(LONG lTimeout)
{
	m_lServerResponseTimeout = lTimeout;
	return S_OK;
}


STDMETHODIMP CConnectionTestParam::SetHttpTunnelResponseTimeout(LONG lTimeout)
{
	m_lHttpTunnelResponseTimeout = lTimeout;
	return S_OK;
}


STDMETHODIMP CConnectionTestParam::SetHttpTunnelMode(LONG lMode)
{
	m_lHttpTunnelMode = lMode;
	return S_OK;
}