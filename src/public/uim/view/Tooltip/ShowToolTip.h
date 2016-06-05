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
 


#pragma once

#include <interfaces/IUOLMessengerImageButton.h>
#include "ToolTipElement.h"

template<class T>
class CShowToolTip
{
public:

	BEGIN_MSG_MAP(CShowToolTip<T>)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

protected:

	inline LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_pT = (T*)this;

		ATLASSERT(m_pT != NULL);

		if (m_pT != NULL)
		{
			m_ToolTip.Create(m_pT->GetWindow(), NULL, NULL, TTS_NOPREFIX /*| TTS_BALLOON*/);
			m_ToolTip.SetDelayTime(TTDT_AUTOPOP, 5000);
			m_ToolTip.SetDelayTime(TTDT_INITIAL, 400);
			m_ToolTip.AddTool(m_pT->GetWindow(), _T(""));

			m_mouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
			m_mouseEvent.dwFlags = TME_LEAVE;
			m_mouseEvent.dwHoverTime = 0;
			m_mouseEvent.hwndTrack = m_pT->m_hWnd;
		}

		bHandled = FALSE;

		return 0L;
	}

	inline LRESULT OnMouseMove(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
        CPoint pos(lParam);

		if (m_pT)
		{
			CToolTipElement toolTipElement = m_pT->GetSelectedControl(pos);
			CString strTip = toolTipElement.GetToolTipText();

			if (strTip.IsEmpty())
			{
				m_ToolTip.Pop();
			}
			else if ((m_strSelectedTip.Compare(strTip) != 0)&& m_ToolTip.IsWindow())
			{
				m_ToolTip.UpdateTipText((LPCSTR) strTip, m_pT->m_hWnd);
				m_ToolTip.RelayEvent((LPMSG)m_pT->GetCurrentMessage());
			}
	
			m_strSelectedTip = strTip;
		}

		TrackMouseEvent(&m_mouseEvent);

		bHandled = FALSE;

		return 0L;
	}

	inline LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_strSelectedTip.Empty();

		bHandled = FALSE;

		return 0L;
	}


private:
	IUOLMessengerImageButtonPtr m_pImageButton;
	CAtlMap<int, CString>		m_mapTips;
	T							*m_pT;
    CToolTipCtrl				m_ToolTip;
	CString						m_strSelectedTip;
	TRACKMOUSEEVENT				m_mouseEvent;

};

