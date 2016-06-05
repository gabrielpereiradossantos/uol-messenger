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
#include ".\UIMDetailsCtrl.h"

#include <strsafe.h>
#include "../resource.h"
#include "../urlbuilder.h"
#include "../controller/UIMApplication.h"
#include <commands/OpenBrowserCommand.h>


CUIMDetailsCtrl::CUIMDetailsCtrl() :
	m_pWebBrowser(NULL)
{
}


CUIMDetailsCtrl::~CUIMDetailsCtrl()
{
}

void CUIMDetailsCtrl::SetInfoText(const CString& strText) 
{
	HRESULT hr = E_FAIL;

	if (m_pDocument)
	{
		CComPtr<IHTMLElement> pBody;
		hr = m_pDocument->get_body(&pBody);

		if (SUCCEEDED(hr))
		{
			SetInfoTextImpl(pBody, strText);
		}
	}
}

HWND CUIMDetailsCtrl::Create(HWND hWndParent, _U_RECT rect, LPCTSTR szWindowName,
			DWORD dwStyle, DWORD dwExStyle,
			_U_MENUorID MenuOrID, LPVOID lpCreateParam)
{
	HWND hWnd = __super::Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);

	if (hWnd != NULL)
	{
		ModifyStyle(0, /*WS_HSCROLL |*/ WS_VSCROLL);

		CComBSTR bstrURL = CURLBuilder::ResourceToURL(IDR_HTML_CONTACT_INFO);

		if (FAILED(CreateControl(bstrURL)))
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


void CUIMDetailsCtrl::OnDestroy()
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


LRESULT CUIMDetailsCtrl::OnEraseBackground(HDC hDc)
{
	return 1;
}

void CUIMDetailsCtrl::OnSize(UINT, CSize)
{
	Invalidate();

	SetMsgHandled(FALSE);
}

STDMETHODIMP_(void) CUIMDetailsCtrl::BeforeNavigate2(IDispatch* /*pDisp*/, VARIANT* url, VARIANT* /*Flags*/, VARIANT* /*TargetFrameName*/, VARIANT* /*PostData*/, VARIANT* /*Headers*/,  VARIANT_BOOL* Cancel)
{
	CString strURL(url->bstrVal);

	if ((0 != strURL.Find(_T("res://"))) &&
		(0 != strURL.Compare(_T("about:blank"))))
	{
		*Cancel = VARIANT_TRUE;

		CUIMApplication::GetApplication()->ExecuteCommand(new COpenBrowserCommand(strURL));
	}
}

STDMETHODIMP_(void) CUIMDetailsCtrl::DocumentComplete(IDispatch* pDisp, VARIANT* /*URL*/)
{
	// Get IDocument2 pointer
	if (!m_pDocument)
	{
		CComPtr<IDispatch> pDispatch;
		HRESULT hr;

		hr = m_pWebBrowser->get_Document(&pDispatch);
		if (SUCCEEDED(hr))
		{
			m_pDocument = pDispatch;
		}
	}
}

HRESULT CUIMDetailsCtrl::SetInfoTextImpl(CComPtr<IHTMLElement> pBody, const CString& strText)
{
	HRESULT hr = pBody->put_innerHTML(CComBSTR(strText));
	return hr;
}