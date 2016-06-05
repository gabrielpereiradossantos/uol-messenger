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
 
/* Copyright (c) - UOL Inc,
 * Todos os direitos reservados
 *
 * Este arquivo e uma propriedade confidencial do Universo Online Inc.
 * Nenhuma parte do mesmo pode ser copiada, reproduzida, impressa ou
 * transmitida por qualquer meio sem autorizacao expressa e por escrito
 * de um representante legal do Universo Online Inc.
 *
 * All rights reserved
 *
 * This file is a confidential property of Universo Online Inc.
 * No part of this file may be reproduced or copied in any form or by
 * any means without written permission from an authorized person from
 * Universo Online Inc. 
 */

#include "StdAfx.h"
#include ".\bitmappanelwnd.h"

CBitmapPanelWnd::CBitmapPanelWnd(void) 
{	
}

CBitmapPanelWnd::~CBitmapPanelWnd(void)
{
}

BOOL CBitmapPanelWnd::SetUrl(const CString& strUrl)
{	
	return Navigate(strUrl);
}

void CBitmapPanelWnd::SetOfflineUrl(const CString&  strUrl)
{
	m_strOfflineUrl = strUrl;
}

void CBitmapPanelWnd::SetNotFoundUrlList(const CString& strUrlList)
{
	m_UrlNotFoundList.RemoveAll();

	int nPos = 0;
	CString strResToken = strUrlList.Tokenize(_T(";"), nPos);
	while (strResToken != "")
	{
		// Add URL to list
		strResToken.TrimLeft();
		strResToken.TrimRight();
		m_UrlNotFoundList.AddTail(strResToken);

		strResToken = strUrlList.Tokenize(_T(";"), nPos);
	}
}

HWND CBitmapPanelWnd::Create(HWND hWndParent, _U_RECT rect, LPCTSTR szWindowName,
			DWORD dwStyle, DWORD dwExStyle,
			_U_MENUorID MenuOrID, LPVOID lpCreateParam)
{
	//CPoint pt(10, 15);
	//CRect rectNew = CRect(rect.m_lpRect) + CRect(pt, CPoint(0, 0));

	HWND hwnd = __super::Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
	//HWND hwnd = __super::Create(hWndParent, rectNew, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);

	if (hwnd)
	{
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

	return hwnd;
}



void CBitmapPanelWnd::OnDestroy()
{
	if (m_pWebBrowser)
	{
		ReceiveEvents(FALSE, m_pWebBrowser);
		m_pWebBrowser = NULL;
	}

	SetMsgHandled(FALSE);
}

STDMETHODIMP_(void) CBitmapPanelWnd::NavigateError(IDispatch* /*pDisp*/, VARIANT* /*URL*/, VARIANT* /*TargetFrameName*/, VARIANT* /*StatusCode*/, VARIANT_BOOL* /*Cancel*/)
{
	if (FALSE == m_strOfflineUrl.IsEmpty())
	{	
		Navigate(m_strOfflineUrl);
	}
}

STDMETHODIMP_(void) CBitmapPanelWnd::DocumentComplete(IDispatch* pDisp, VARIANT* URL)
{
	CString strURL(URL->bstrVal);

	// Navigation to any not found error page?
	CString strNotFoundURL;
	int nFind;
	for (POSITION pos = m_UrlNotFoundList.GetHeadPosition(); pos; )
	{
		strNotFoundURL = m_UrlNotFoundList.GetNext(pos);

		nFind = strURL.Find(strNotFoundURL);
		if (-1 != nFind)
		{
			// Redir to offline page
			Navigate(m_strOfflineUrl);
			return;
		}
	}

	// If loaded resource (res://) page, redir to offline page
	HRESULT hr;
	IDispatch* pDispDocument = NULL;
	hr = m_pWebBrowser->get_Document(&pDispDocument);

	if ((SUCCEEDED(hr)) && (pDispDocument != NULL))
	{
		IHTMLDocument2* pDocument = NULL;
		hr = pDispDocument->QueryInterface(__uuidof(IHTMLDocument2), (LPVOID*) &pDocument);
		if ((SUCCEEDED(hr)) && (pDocument != NULL))
		{
			BSTR bstrURL;
			hr = pDocument->get_URL(&bstrURL);
			if (SUCCEEDED(hr))
			{
				strURL = bstrURL;
				::SysFreeString(bstrURL);

				if ((0 == strURL.Find(_T("res://"))) &&
					(-1 != strURL.Find(m_strOfflineUrl)))
				{
					Navigate(m_strOfflineUrl);
				}
			}

			pDocument->Release();
		}

		pDispDocument->Release();
	}
}

BOOL CBitmapPanelWnd::Navigate(const CString& strUrl)
{
	if (m_pWebBrowser)
	{
		CComVariant varUrl(strUrl);			
		
		return SUCCEEDED(m_pWebBrowser->Navigate2(&varUrl, NULL, NULL, NULL, NULL));
	}

	return FALSE;
}