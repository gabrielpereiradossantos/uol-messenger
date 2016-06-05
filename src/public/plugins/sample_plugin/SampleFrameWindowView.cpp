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
#include "SampleFrameWindowView.h"


CSampleFrameWindowView::~CSampleFrameWindowView()
{
}


HWND CSampleFrameWindowView::CreateView(HWND hWndParent)
{
	HWND hWnd = Create(hWndParent);

	return hWnd;
}


void CSampleFrameWindowView::GetMinSize(CSize& size)
{
	size.cx = 180;
	size.cy = 140;
}


BOOL CSampleFrameWindowView::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}


LRESULT CSampleFrameWindowView::OnCreate(LPCREATESTRUCT lpCreatestruct)
{
	CRect rectClient;
	GetClientRect(&rectClient);

	m_wndButton.Create(m_hWnd, rectClient, _T("Test"), 
			WS_VISIBLE | WS_CHILD, 0, ID_BTN_TEST);

	return 0L;
}


void CSampleFrameWindowView::OnDestroy()
{
	m_wndButton.DestroyWindow();
}


void CSampleFrameWindowView::OnSize(UINT nType, const CSize& size)
{
	if (SIZE_MINIMIZED != nType)
	{
		m_wndButton.SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOZORDER);
	}

	SetMsgHandled(FALSE);
}


void CSampleFrameWindowView::OnClickedButtonTest(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	MessageBox(_T("OnClickedButtonTest"), _T("CSampleFrameWindowView"), MB_OK);
}