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
#include ".\DetailsWindow.h"

#include <strsafe.h>
#include "../resource.h"
#include "../urlbuilder.h"

#include "../controller/UIMApplication.h"

void CDetailsWindow::SetDetailsInfo(const CString& strInfo)
{
	m_detailsCtrl.SetInfoText(strInfo);
}

void CDetailsWindow::OnSize(UINT nType, const CSize& size)
{
	m_detailsCtrl.SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER);
	SetMsgHandled(FALSE);
}

LRESULT CDetailsWindow::OnCreate(LPCREATESTRUCT lpCreatestruct)
{
	CRect rectClient;
	GetClientRect(&rectClient);

	m_detailsCtrl.Create(m_hWnd, rectClient, NULL, 
		WS_CHILDWINDOW | WS_VISIBLE  | WS_VSCROLL | ES_MULTILINE | ES_READONLY);
	m_detailsCtrl.ShowWindow(SW_NORMAL);

	CUIMApplication::GetApplication()->AddMessageFilter(this);	

	return 0L;
}

void CDetailsWindow::OnDestroy()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);

	CUIMApplication::GetApplication()->RemoveMessageFilter(this);	

	SetMsgHandled(FALSE);
}

BOOL CDetailsWindow::PreTranslateMessage(MSG* pMsg)
{
	// This was stolen from an SDI app using a form view.
	//
	// Pass keyboard messages along to the child window that has the focus.
	// When the browser has the focus, the message is passed to its containing
	// CAxWindow, which lets the control handle the message.

	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	HWND hWndCtl = ::GetFocus();

	if(IsChild(hWndCtl))
	{
		// find a direct child of the dialog from the window that has focus
		while(::GetParent(hWndCtl) != m_hWnd)
			hWndCtl = ::GetParent(hWndCtl);

		// give control a chance to translate this message
		if(::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
			return TRUE;
	}

	// A normal control has the focus, so call IsDialogMessage() so that
	// the dialog shortcut keys work (TAB, etc.)
	//return IsDialogMessage(pMsg);
	return FALSE;
}