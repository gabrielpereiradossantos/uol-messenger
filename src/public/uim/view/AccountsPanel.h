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

#include "tooltip/ShowToolTip.h"
#include "tooltip/ToolTipElement.h"

#include "skin/WindowElement.h"

#include "AccountButton.h"
#include "DummyWindow.h"


class CAccountsPanelEventListener
{
public:
	virtual ~CAccountsPanelEventListener() {}

	virtual void OnLButtonDownAccountButton(const CPoint& ptClick, CAccountButtonPtr pButton) { }
	virtual void OnRButtonDownAccountButton(const CPoint& ptClick, CAccountButtonPtr pButton) { }
};


class CAccountsPanel;
typedef CWindowElement<CAccountsPanel, CDummyWindow<CWindowImpl<CAccountsPanel> > > _CAccountsPanelBase;

class CAccountsPanel : public _CAccountsPanelBase,
						public CShowToolTip<CAccountsPanel>
{
public:
	DECLARE_WND_CLASS_EX("AccountsPanel", CS_DBLCLKS, COLOR_WINDOW);
	DECLARE_ELEMENT_NAME("AccountsPanel");

	CAccountsPanel(void);
	virtual ~CAccountsPanel(void);

	void AddAccountButton(CAccountButtonPtr pButton);
	CAccountButtonPtr RemoveAccountButton(IUOLMessengerAccountPtr pAccount);

	virtual void GetMinSize(CSize& size);

	CToolTipElement GetSelectedControl(CPoint& pos);

	void AddEventListener(CAccountsPanelEventListener* pListener);
	void RemoveEventListener(CAccountsPanelEventListener* pListener);

protected:
	BEGIN_MSG_MAP(CAccountsPanel)
		CHAIN_MSG_MAP(CShowToolTip<CAccountsPanel>)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		CHAIN_MSG_MAP(_CAccountsPanelBase)
	END_MSG_MAP()
	
	void OnLButtonDown(UINT nFlags, CPoint& ptClick);
	void OnRButtonDown(UINT nFlags, CPoint& ptClick);

	virtual void DoPaint(CDCHandle& dcHandle);
	virtual void UpdateLayout(BOOL bResizeBars = TRUE);

	void NotifyEventListeners(void (CAccountsPanelEventListener::*pfnCallback)(const CPoint& ptClick, CAccountButtonPtr pButton), const CPoint& ptClick, CAccountButtonPtr pButton);

private:
	void InsertAccountButton(CAccountButtonPtr pButton);

	void DrawAccountButtons(CDCHandle& dc);

	int GetAccountButtonsSpacing() const;

	CAccountButtonPtr CheckAccountButtonClick(const CPoint& ptClick);

	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	virtual CString GetName() const;

private:
	static enum PanelMetrics
	{
		PM_HBORDER = 2,
		PM_VBORDER = 2
	};
	
	int					  m_cy;
	
	CAtlList<CAccountButtonPtr> m_listAccountButtons;
	CAtlList<CAccountsPanelEventListener*> m_listEventListener;
};

MAKEAUTOPTR(CAccountsPanel);