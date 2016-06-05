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
 


#include "resource.h"
#include <commands/ShowPreferencesCommand.h>
#include "../controller/UIMApplication.h"
#pragma once

#pragma warning(disable: 4311)

using namespace WTL;

class CStaticLink : public CWindowImpl<CStaticLink> {
	/*
	Based on CStaticLink by Paul DiLascia, C++ Q&A, Microsoft Systems
	Journal 12/1997.
	Turns static controls into clickable "links" -- when the control is
	clicked, the file/program/webpage named in the control's text (or
	set by SetLinkText()) is opened via ShellExecute().  Static control
	can be either text or graphic (bitmap, icon, etc.).
	*/
public:
	const static UINT WM_STATIC_LINK_CLICK;// = ::RegisterWindowMessage("UOL_STATIC_LINK_CLICK");

	DECLARE_WND_SUPERCLASS( _T("StaticLink"), _T("Static") )

	CStaticLink(COLORREF clrUnvisited, COLORREF clrVisited, BOOL bVisited = FALSE, 
		int nHeight = 12, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bUnderline = TRUE) :
		m_colorUnvisited(clrUnvisited),
		m_colorVisited(clrVisited),
		m_bVisited(bVisited),
		m_bstrLink(_T(""))
	{
		m_font.CreateFont(nHeight, 0, 0, 0, (bBold ? FW_BOLD : FW_NORMAL), 
				static_cast<BYTE>(bItalic), static_cast<BYTE>(bUnderline), 
				0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
				DEFAULT_QUALITY, (DEFAULT_PITCH | FF_SWISS), _T("Arial"));
	}


	void SetLinkText(LPCTSTR szLink) 
	{
		USES_CONVERSION;
		m_bstrLink = T2OLE(szLink);
	}

	BEGIN_MSG_MAP(CStaticLink)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
		// uses message reflection: WM_* comes back as OCM_*
		MESSAGE_HANDLER(OCM_COMMAND, OnCommand)
		MESSAGE_HANDLER(OCM_CTLCOLORSTATIC, OnCtlColor)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy) // not a reflected message
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()


	static HCURSOR g_hCursorLink;

	LRESULT OnSetCursor(UINT, WPARAM , LPARAM, BOOL&)
	{
		if (g_hCursorLink == NULL)
		{
			HINSTANCE hInstance = ATL::_AtlBaseModule.GetResourceInstance();
			g_hCursorLink = ::LoadCursor(hInstance,MAKEINTRESOURCE(IDC_CUSTOM_HAND));
		}

		if (g_hCursorLink)
		{
			::SetCursor(g_hCursorLink);
			return TRUE;
		}

		return FALSE;
	}

	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&)
	{
		//if (m_hFont) DeleteObject(m_hFont);
		return 0;
	}

	LRESULT OnCommand(UINT, WPARAM wParam, LPARAM, BOOL&)
	{
		USES_CONVERSION;
		int code = HIWORD(wParam);
		if ((code == STN_CLICKED) || (code == STN_DBLCLK)) 
		{
			if (m_bstrLink.Length() != 0)
			{
				if ((int)ShellExecute(*this, _T("open"),
					OLE2T(m_bstrLink), NULL, NULL, SW_SHOWNORMAL) > 32 )
				{
					m_bVisited = TRUE;   // return codes > 32 => success
					Invalidate();
				}
				else
				{
					//MessageBeep(0);
					ATLTRACE(_T("Error: CStaticLink couldn't open file\n"));
				}
			}
			else
			{
				//IUOLMessengerFacade* pUOLMessengerFacade = CUIMApplication::GetApplication();

				//if (pUOLMessengerFacade)
				//{
				//	pUOLMessengerFacade->ExecuteCommand(new CShowPreferencesCommand());
				//}
				////return S_OK;
			}

			SendMessage((this->GetParent()).m_hWnd,WM_STATIC_LINK_CLICK, this->GetDlgCtrlID(), 0);
		}
		return 0;
	}

	LRESULT OnCtlColor(UINT, WPARAM wParam, LPARAM, BOOL&)
	{
		// notify bit must be set to get STN_* notifications
		ModifyStyle(0, SS_NOTIFY);
		HBRUSH hBr = NULL;
		if ((GetStyle() & 0xff) <= SS_RIGHT )
		{
			// it's a text control: set up font and colors
			if (!m_font.m_hFont)
			{
				ATLASSERT(FALSE);
				return 0;
			}

			HDC hDC = (HDC) wParam;
			SelectObject(hDC, m_font.m_hFont);
			SetTextColor(hDC, (m_bVisited ? m_colorVisited : m_colorUnvisited));
			SetBkMode(hDC, TRANSPARENT);
			hBr = (HBRUSH) GetStockObject(HOLLOW_BRUSH);
		}
		return (LRESULT) hBr;
	}

private:
	COLORREF	m_colorUnvisited;
	COLORREF	m_colorVisited;
	BOOL		m_bVisited;
	CFont		m_font;
	CComBSTR	m_bstrLink;

}; // CStaticLink

__declspec(selectany)
HCURSOR CStaticLink::g_hCursorLink = NULL;

__declspec(selectany)
const UINT CStaticLink::WM_STATIC_LINK_CLICK = ::RegisterWindowMessage("UOL_STATIC_LINK_CLICK");
