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
 
// HTMLViewCtrl.cpp : implementation of the CHTMLViewCtrl class
//
/////////////////////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "HTMLViewCtrl.h"
#include <comdef.h>


CHTMLViewCtrl::CHTMLViewCtrl() :
	m_pWebBrowser(NULL),
	m_strHtml("")
{
}


CHTMLViewCtrl::~CHTMLViewCtrl()
{
}


void CHTMLViewCtrl::SetHtml(const CString& strHtml)
{
	HTMLMESSAGEPARAM htmlParam;
	htmlParam.strHtml = strHtml;

	SendMessage(WM_ON_SET_HTML, (WPARAM) &htmlParam);
}


HWND CHTMLViewCtrl::Create(HWND hWndParent, _U_RECT rect, LPCTSTR szWindowName,
								 DWORD dwStyle, DWORD dwExStyle,
								 _U_MENUorID MenuOrID, LPVOID lpCreateParam)
{
	HWND hWnd = __super::Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
	
	if (hWnd != NULL)
	{
		ModifyStyle(0, WS_VSCROLL);
		
		if (FAILED(CreateControl(L"about:blank")))
		{
			ATLASSERT(FALSE);
			DestroyWindow();
			
			return NULL;
		}	
		
		if (FAILED(QueryControl(&m_pWebBrowser)))
		{
			ATLASSERT(FALSE);
			DestroyWindow();
			
			return NULL;
		}
		
		ReceiveEvents(TRUE, m_pWebBrowser);
	}
	
	return hWnd;
}


void CHTMLViewCtrl::OnDestroy()
{
	if (m_pWebBrowser)
	{
		ReceiveEvents(FALSE, m_pWebBrowser);
		m_pDocument.Release();
		m_pWebBrowser.Release();
		
		m_pDocument = NULL;
		m_pWebBrowser = NULL;
	}
}


void CHTMLViewCtrl::OnSize(UINT, CSize)
{
	Invalidate();

	SetMsgHandled(FALSE);
}


STDMETHODIMP_(void) CHTMLViewCtrl::DocumentComplete(IDispatch* pDisp, VARIANT* /*URL*/)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_htmlCS);
	if (!m_strHtml.IsEmpty())
	{
		CComPtr<IDispatch> pDispatch;
		HRESULT hr;
		
		if (!m_pDocument)
		{
			// Get IDocument2 pointer
			hr = m_pWebBrowser->get_Document(&pDispatch);
			
			if (SUCCEEDED(hr))
			{
				m_pDocument = pDispatch;
			}
		}
		
		SAFEARRAY* psa = SafeArrayCreateVector(VT_VARIANT, 0, 1);
		VARIANT *param;
		bstr_t bsData = (LPCTSTR)m_strHtml;
		hr =  SafeArrayAccessData(psa, (LPVOID*)&param);
		
		if (hr != S_OK)
		{
			return;
		}
		
		param->vt = VT_BSTR;
		param->bstrVal = (BSTR)bsData;
		hr = m_pDocument->write(psa);	//write your buffer
		
		if (hr != S_OK)
		{
			return;
		}
		
		hr = m_pDocument->close();	//and closes the document, "applying" your code  
		if (hr != S_OK)
		{
			return;
		}
		
		SafeArrayDestroy(psa);
	}
}


LRESULT CHTMLViewCtrl::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DWORD code = HIWORD(lParam);
	
	if (code == WM_RBUTTONDOWN)
	{
		return MA_ACTIVATEANDEAT;
	}
	else
	{
		return MA_ACTIVATE;
	}
}


LRESULT CHTMLViewCtrl::OnSetHtml(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPHTMLMESSAGEPARAM pHtml = (LPHTMLMESSAGEPARAM) wParam;

	if (pHtml != NULL)
	{
		m_strHtml = pHtml->strHtml;
		
		if ((!m_strHtml.IsEmpty()) && (m_pWebBrowser != NULL))
		{
			CComVariant vURL = _T("about:blank");
			CComVariant vFlags = navNoHistory;
			CComVariant vTarget = _T("_self");
			CComVariant vDummy;
			
			HRESULT hr = m_pWebBrowser->Navigate2(&vURL, &vFlags, &vTarget, &vDummy, &vDummy);
			ATLASSERT(SUCCEEDED(hr));
		}
	}

	bHandled = TRUE;
	return 0L;
}