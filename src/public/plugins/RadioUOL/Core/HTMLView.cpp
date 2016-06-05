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
#include "htmlview.h"
#include "RadioUOLWindow.h"


CSampleFrameWindowView::~CSampleFrameWindowView()
{
	//HRESULT hr = DeattachControl();
}


HWND CSampleFrameWindowView::CreateView(HWND hWndParent)
{
	HWND hWnd ;//= Create(hWndParent);
	hWnd = Create(hWndParent, rcDefault, _T("about:blank"),
		WS_HSCROLL| WS_VSCROLL| WS_CHILD | WS_VISIBLE | 
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	return hWnd;
}


void CSampleFrameWindowView::GetMinSize(CSize& size)
{
	//size.cx = 490;
	//size.cy = 420;
	size.cx = 490;
	size.cy = 420;

}


BOOL CSampleFrameWindowView::PreTranslateMessage(MSG* pMsg)
{
	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
	   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	// give HTML page a chance to translate this message 
	//return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
	//SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
	if ((pMsg->message == WM_CHAR) ||
		(pMsg->message == WM_KEYDOWN) || 
		(pMsg->message == WM_KEYUP))

	{
		if ((pMsg->wParam == VK_UP) || 
			(pMsg->wParam == VK_DOWN) ||
			(pMsg->wParam == VK_LEFT) ||
			(pMsg->wParam == VK_RIGHT) ||
			(pMsg->wParam == VK_BACK))
		{
			return FALSE;
		}


		HWND hWndCtl = ::GetFocus();
		if(IsChild(hWndCtl))
		{
			// give control a chance to translate this message
			if(::SendMessage(m_hWnd, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
				return TRUE;
		}
	}

	return FALSE;
}


LRESULT CSampleFrameWindowView::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr = DeattachControl();
	//ATLASSERT(SUCCEEDED(hr));
	//m_wndButton.DestroyWindow();
	return hr;
}
LRESULT CSampleFrameWindowView::OnCreate(UINT uMsg, WPARAM wParam , LPARAM lParam, BOOL& bHandled)
{
	// First you must CWTLAxControl<...,...>::OnCreate ( it set this message Handled )
	return CWTLAxControl<CWTLIExplorer,IWebBrowser2>::OnCreate( uMsg, wParam, lParam, bHandled );
}


void CSampleFrameWindowView::OnSize(UINT nType, const CSize& size)
{
	SetMsgHandled(FALSE);
}


EVENTFN CSampleFrameWindowView::OnNavigateComplete2( IDispatch* pDisp,  VARIANT* URL )
{
}


EVENTFN CSampleFrameWindowView::OnBeforeNavigate2( IDispatch* /*pDisp*/, VARIANT* url, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers,  VARIANT_BOOL* Cancel )
{
	HWND hwnd = GetParent();
	if (hwnd!=NULL)
	{
		CString strURL(url->bstrVal);
		CString strText;
		CRadioUOLWindowPtr ptr = CRadioUOLWrapper::GetFacadeInstance();
	
		if (ptr!=NULL)
		{
			CRadioConfig* pConfig = ptr->GetConfig();
			if (pConfig!=NULL)
			{
				strText = GetField(strURL,pConfig->GetSiteChannelRegEx());
				if (strText.IsEmpty()==FALSE) 
				{
					int i;
					i = atoi(strText);
					::PostMessage(*ptr,WM_COMMAND,i,0);
					GetParent().PostMessage(WM_CLOSE);

				} else {
					strText = GetField(strURL,pConfig->GetSiteSearchRegEx());
					if (strText.IsEmpty()==FALSE)
					{
						ptr->PlayURL(strText);
						GetParent().PostMessage(WM_CLOSE);
					}
				}
			}
		}
	}
}
