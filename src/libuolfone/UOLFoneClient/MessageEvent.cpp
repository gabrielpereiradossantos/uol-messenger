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
#include "MessageEvent.h"


CMessageEvent::CMessageEvent()
{
}


CMessageEvent::~CMessageEvent()
{
}


STDMETHODIMP CMessageEvent::GetEventType(LONG* plEventType)
{
	HRESULT hr = E_POINTER;

	if (plEventType)
	{
		*plEventType = GetEventType();
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CMessageEvent::GetFrom(BSTR* pbstrFrom)
{
	HRESULT hr = E_POINTER;

	if (pbstrFrom)
	{
		*pbstrFrom = GetFrom().AllocSysString();
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CMessageEvent::GetTo(BSTR* pbstrTo)
{
	HRESULT hr = E_POINTER;

	if (pbstrTo)
	{
		*pbstrTo = GetTo().AllocSysString();
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CMessageEvent::GetContentType(BSTR* pbstrContentType)
{
	HRESULT hr = E_POINTER;

	if (pbstrContentType)
	{
		*pbstrContentType = GetContentType().AllocSysString();
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CMessageEvent::GetContentSubtype(BSTR* pbstrContentSubtype)
{
	HRESULT hr = E_POINTER;

	if (pbstrContentSubtype)
	{
		*pbstrContentSubtype = GetContentSubtype().AllocSysString();
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CMessageEvent::GetContent(BSTR* pbstrContent)
{
	HRESULT hr = E_POINTER;

	if (pbstrContent)
	{
		*pbstrContent = GetContent().AllocSysString();
		hr = S_OK;
	}

	return hr;
}


LONG CMessageEvent::GetEventType()
{
	return m_lEventType;
}


CString CMessageEvent::GetFrom()
{
	return m_strFrom;
}


CString CMessageEvent::GetTo()
{
	return m_strTo;
}


CString CMessageEvent::GetContentType()
{
	return m_strContentType;
}


CString CMessageEvent::GetContentSubtype()
{
	return m_strContentSubtype;
}


CString CMessageEvent::GetContent()
{
	return m_strContent;
}


void CMessageEvent::SetEventType(LONG lEventType)
{
	m_lEventType = lEventType;
}


void CMessageEvent::SetFrom(const CString& strFrom)
{
	m_strFrom = strFrom;
}


void CMessageEvent::SetTo(const CString& strTo)
{
	m_strTo = strTo;
}


void CMessageEvent::SetContentType(const CString& strContentType)
{
	m_strContentType = strContentType;
}


void CMessageEvent::SetContentSubtype(const CString& strContentSubtype)
{
	m_strContentSubtype = strContentSubtype;
}


void CMessageEvent::SetContent(const CString& strContent)
{
	m_strContent = strContent;
}