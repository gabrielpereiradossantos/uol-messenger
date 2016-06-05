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
#include ".\UOLFoneSlider.h"
#include "../FacadeWrapper.h"



CUOLFoneSlider::CUOLFoneSlider(void)
{
	m_rgbBackground = GetSysColor(COLOR_WINDOW);
	
	m_backgroundBrushHandle = m_pBrush.CreateSolidBrush(m_rgbBackground);
}

CUOLFoneSlider::~CUOLFoneSlider(void)
{
	m_pBrush.DeleteObject();
}


LPCTSTR CUOLFoneSlider::GetWndClassName()
{
	return _T("CUOLFoneSlider");
}


void CUOLFoneSlider::SetBackgroundColor(COLORREF bkColor)
{
	if (bkColor != NULL) {
		
		m_rgbBackground = bkColor;
		
		m_pBrush.DeleteObject();
		m_backgroundBrushHandle = m_pBrush.CreateSolidBrush(m_rgbBackground);
	}
}


LRESULT CUOLFoneSlider::OnEraseBackground(HDC hDc)
{
	return 1L;
}


BOOL CUOLFoneSlider::PreTranslateMessage(MSG* pMsg)
{
	if ( WM_MOUSEWHEEL == pMsg->message )
	{
		if (pMsg->hwnd  == m_hWnd)
		{
			short zDelta = GET_WHEEL_DELTA_WPARAM(pMsg->wParam);
			pMsg->wParam = MAKEWPARAM(LOWORD(pMsg->wParam), -zDelta);
		}
	}
	
	return FALSE;
}

LRESULT CUOLFoneSlider::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(FALSE);

	BOOL bAdded = CFacadeWrapper::GetInstance()->GetFacadeInstance()->AddMessageFilter(this);
	ATLASSERT(FALSE != bAdded);

	return 0L;
}

LRESULT CUOLFoneSlider::OnDestroy(void)
{
	SetMsgHandled(FALSE);

	BOOL bRemoved = CFacadeWrapper::GetInstance()->GetFacadeInstance()->RemoveMessageFilter(this);
	ATLASSERT(FALSE != bRemoved);

	return 0L;
}
