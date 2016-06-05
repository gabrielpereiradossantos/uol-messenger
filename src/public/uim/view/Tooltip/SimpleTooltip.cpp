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
#include "SimpleTooltip.h"

CSimpleTooltip::CSimpleTooltip()
{
}


CSimpleTooltip::~CSimpleTooltip()
{
    if (m_TooltipCtrl.IsWindow())
    {
        m_TooltipCtrl.SendMessage(TTM_POP, 0, 0);
    }
}


void CSimpleTooltip::ShowTooltip(HWND hwndParent, const CString& strTipText, CPoint& pt,
                                int nAutoPopDelay, int nInitialDelay, int nReshowDelay)
{
    if (!m_TooltipCtrl.IsWindow())
    {
        m_TooltipCtrl.Create(hwndParent, NULL, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP);
    }

    // Initialize TOOLINFO structure
    m_ToolInfo.cbSize = sizeof(TOOLINFO);
    m_ToolInfo.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
    m_ToolInfo.hwnd = hwndParent;
    m_ToolInfo.hinst = NULL;
    m_ToolInfo.uId = (UINT_PTR)hwndParent;
    m_ToolInfo.lpszText = (LPSTR)(LPCSTR) strTipText;		

    GetClientRect(hwndParent, &m_ToolInfo.rect);

    CPoint ptTooltipPos(pt);

    CRect rectDesktop;
	::GetClientRect(::GetDesktopWindow(), &rectDesktop);

	if (ptTooltipPos.y > (rectDesktop.bottom/2))
	{
		ptTooltipPos.y -= 20;
	}
	else
	{
		ptTooltipPos.y += 20;
	}

    m_TooltipCtrl.SendMessage(TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &m_ToolInfo);
    m_TooltipCtrl.SendMessage(TTM_TRACKPOSITION, 0, (LPARAM)(DWORD) MAKELONG(ptTooltipPos.x, ptTooltipPos.y));
    m_TooltipCtrl.SendMessage(TTM_SETDELAYTIME, (WPARAM) (DWORD) TTDT_AUTOPOP, (LPARAM) MAKELONG(nAutoPopDelay, 0));
    m_TooltipCtrl.SendMessage(TTM_SETDELAYTIME, (WPARAM) (DWORD) TTDT_INITIAL, (LPARAM) MAKELONG(nInitialDelay, 0));
    m_TooltipCtrl.SendMessage(TTM_SETDELAYTIME, (WPARAM) (DWORD) TTDT_RESHOW, (LPARAM) MAKELONG(nReshowDelay, 0));
    m_TooltipCtrl.SendMessage(TTM_UPDATETIPTEXT, 0, (LPARAM)(LPTOOLINFO) &m_ToolInfo);
    m_TooltipCtrl.SendMessage(TTM_TRACKACTIVATE, true, (LPARAM)(LPTOOLINFO) &m_ToolInfo);
}


void CSimpleTooltip::HideTooltip()
{
    if (m_TooltipCtrl.IsWindow())
    {
        m_TooltipCtrl.SendMessage(TTM_TRACKACTIVATE, false, (LPARAM)(LPTOOLINFO) &m_ToolInfo);
    }
}
