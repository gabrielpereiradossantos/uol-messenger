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


#include "ShowTooltipSubclass.h"

typedef  CWinTraits <WS_CHILD | WS_VISIBLE | BS_OWNERDRAW> CMultiStateButtonTraits;

class CUOLFoneMultiStateButton : 
		public CWindowImpl<CUOLFoneMultiStateButton, CWindow, CMultiStateButtonTraits>, 
		public CShowTooltipSubclass<CUOLFoneMultiStateButton>
{
public:
	
	DECLARE_WND_SUPERCLASS("CUOLFoneMultiStateButton", "BUTTON")
	
	CUOLFoneMultiStateButton(void);
	virtual ~CUOLFoneMultiStateButton(void);
	
	static LPCTSTR GetWndClassName();
	
	void SetMonoStateBitmap(int bitmapResourceID);
	void SetMultiStateBitmap(int bitmapResourceID);
	
	void SetMonoStateBitmap(CString strFileName);
	void SetMultiStateBitmap(CString strFileName);
	
	int  GetWidth();
	int  GetHeight();
	
	void SimulateButtonClick();
	
	void SetCharForwardingWnd(HWND charForwardingWnd);
	
protected:
	
	BEGIN_MSG_MAP(CUOLFoneMultiStateButton)
		CHAIN_MSG_MAP(CShowTooltipSubclass<CUOLFoneMultiStateButton>)
		MESSAGE_HANDLER(OCM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUP)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
	END_MSG_MAP()
	
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	void DrawBitmap(HDC hdc, RECT& rc);
	
	UINT GetState() const;
	void SetState(BOOL bPushed);
	
private:
	
	WTL::CBitmap		m_buttonBitmap;
	BOOL				m_bIsMouseOver;
	BOOL				m_bIsMouseLButtonDown;
	BOOL				m_bIsMultiStateBitmap;
	
	UINT_PTR			m_clickSimulationTimer;
	BOOL				m_isSimulatingClick;
	HWND				m_charForwardingWnd;
};

