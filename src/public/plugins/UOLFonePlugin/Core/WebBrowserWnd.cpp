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
 

#include "StdAfx.h"
#include ".\WebBrowserWnd.h"


CWebBrowserWindow::CWebBrowserWindow() : 
	m_pWebBrowser2(NULL), 
	m_pWindowHolder(NULL)
{
}


CWebBrowserWindow::~CWebBrowserWindow()
{
}


HWND CWebBrowserWindow::Create(HWND hWndParent, _U_RECT rect, LPCTSTR szWindowName,
			DWORD dwStyle, DWORD dwExStyle,
			_U_MENUorID MenuOrID, LPVOID lpCreateParam)
{
	HWND hWnd = __super::Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
	
	if (hWnd != NULL)
	{
		if (FAILED(CreateControl(L"about:blank")))
		{
			ATLASSERT(FALSE);
			DestroyWindow();
			
			return NULL;
		}	
		
		if (FAILED(QueryControl(&m_pWebBrowser2)))
		{
			ATLASSERT(FALSE);
			DestroyWindow();
			
			return NULL;
		}
		
		ReceiveEvents(TRUE, m_pWebBrowser2);
	}
	
	return hWnd;
}


BOOL CWebBrowserWindow::SetUrl(const CString& strUrl)
{	
	BOOL result = FALSE;
	
	if (m_pWebBrowser2)
	{
		CComVariant varUrl(strUrl);
		
		result = SUCCEEDED(m_pWebBrowser2->Navigate2(&varUrl, NULL, NULL, NULL, NULL));
	}
	
	return result;
}


void CWebBrowserWindow::SetWindowHolder(IWebBrowserWindowHolder* pWindowHolder)
{
	m_pWindowHolder = pWindowHolder;
}


void CWebBrowserWindow::OnDestroy()
{
	if (m_pWebBrowser2)
	{
		ReceiveEvents(FALSE, m_pWebBrowser2);
		
		m_pWebBrowser2.Release();
		m_pWebBrowser2 = NULL;
	}
	
	m_pWindowHolder = NULL;
}


LRESULT CWebBrowserWindow::OnEraseBackground(HDC hDc)
{
	return 1;
}


STDMETHODIMP_(void) CWebBrowserWindow::BeforeNavigate2(
		IDispatch* /*pDisp*/, VARIANT* url, VARIANT* /*Flags*/, VARIANT* /*TargetFrameName*/, VARIANT* /*PostData*/, 
		VARIANT* /*Headers*/,  VARIANT_BOOL* Cancel)
{
	CString strURL(url->bstrVal);
	
	if (m_pWindowHolder)
	{
		if (m_pWindowHolder->ShouldCancelNavigation(strURL))
		{
			*Cancel = VARIANT_TRUE;
		}
		else
		{
			*Cancel = VARIANT_FALSE;
		}
	}
}


STDMETHODIMP_(void) CWebBrowserWindow::DocumentComplete(IDispatch* pDisp, VARIANT* /*URL*/)
{
}

STDMETHODIMP_(void) CWebBrowserWindow::NavigateError(IDispatch* /*pDisp*/, VARIANT* url, VARIANT* /*TargetFrameName*/,VARIANT* /*StatusCode*/, VARIANT_BOOL* Cancel)
{
	*Cancel = VARIANT_TRUE;
}
