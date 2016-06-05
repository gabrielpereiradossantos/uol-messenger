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

#include "skin/WindowElement.h"

#include "UserAvatar.h"
#include "DummyWindow.h"

#include "tooltip/ShowToolTip.h"
#include "tooltip/ToolTipElement.h"

#include <interfaces/IUOLMessengerImageButton.h>

#include "skin/fontelement.h"

class CUserStatusPanelEventListener
{
public:
	virtual ~CUserStatusPanelEventListener() {}

	virtual void OnLButtonDownAvatar(const CPoint& ptClick) { }
	virtual void OnLButtonDownText(const CPoint& ptClick) { }
	virtual void OnLButtonDownStatusButton(const CPoint& ptClick) { }
	virtual void OnLButtonDownStatusMessage(const CPoint& ptClick) { }

	virtual void OnRButtonDownAvatar(const CPoint& ptClick) { }
	virtual void OnRButtonDownText(const CPoint& ptClick) { }
	virtual void OnRButtonDownStatusButton(const CPoint& ptClick) { }
};


class CUserStatusPanel;
typedef CWindowElement<CUserStatusPanel, CDummyWindow<CWindowImpl<CUserStatusPanel> > > _CUserStatusPanelBase;

class CUserStatusPanel : public _CUserStatusPanelBase,
							public CShowToolTip<CUserStatusPanel>
{
public:
	DECLARE_WND_CLASS_EX("UserStatusPanel", CS_DBLCLKS, COLOR_WINDOW);
	DECLARE_ELEMENT_NAME("UserStatusPanel");

	CUserStatusPanel(void);
	virtual ~CUserStatusPanel(void);

	void SetAvatar(CUserAvatarPtr pAvatar); 
	void SetText(const CString& strText);
	void SetStatus(const CString& strStatus);
	void SetStatusMessage(const CString& strStatusMessage);

	CRect GetRectStatus() const;

	CToolTipElement GetSelectedControl(CPoint& pos);

	virtual void GetMinSize(CSize& size);

	void AddEventListener(CUserStatusPanelEventListener* pListener);
	void RemoveEventListener(CUserStatusPanelEventListener* pListener);

protected:
	BEGIN_MSG_MAP(CUserStatusPanel)
		MSG_WM_SETCURSOR(OnSetCursor)
		CHAIN_MSG_MAP(CShowToolTip<CUserStatusPanel>)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		CHAIN_MSG_MAP(_CUserStatusPanelBase)
	END_MSG_MAP()
	
	LRESULT OnSetCursor(HWND wCtrl, UINT nHitTest, UINT nMouseMsg);
	void OnLButtonDown(UINT nFlags, CPoint& ptClick);
	void OnRButtonDown(UINT nFlags, CPoint& ptClick);

	virtual void DoPaint(CDCHandle& dcHandle);
	virtual void UpdateLayout(BOOL bResizeBars = TRUE);

	void NotifyEventListeners(void (CUserStatusPanelEventListener::*pfnCallback)(const CPoint& ptClick), const CPoint& ptClick);

private:	
	void DrawAvatar(CDCHandle& dc);
	void DrawText(CDCHandle& dc);
	void DrawStatus(CDCHandle& dc);
	void DrawStatusMessage(CDCHandle& dc);

	int GetVerticalSpacing() const;
	int GetHorizontalSpacing() const;
	int GetTextAreaHeight() const;

	int CheckClickedItem(const CPoint& ptClick);

	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	virtual CString GetName() const;

private:
	static enum
	{
		CLICKED_NONE = -1,
		CLICKED_AVATAR = -2,
		CLICKED_TEXT = -3,
		CLICKED_STATUS = -4,
		CLICKED_STATUS_MESSAGE = -5
	};

	static enum PanelMetrics
	{
		PM_AVATAR_WIDTH = 48,
		PM_AVATAR_HEIGHT = 48,
		PM_HBORDER = 2,
		PM_VBORDER = 2,
		PM_STATUS_COMBOBOX_WIDTH = 96
	};
	
	int					  m_cy;
	
	CUserAvatarPtr	m_pAvatar;
	
	CString m_strText;
	CString m_strStatus;
	CString m_strStatusMessage;
	CString m_strNullStatusMessage;

	CRect	m_rectText;
	CRect	m_rectStatus;
	CRect	m_rectStatusMessage;
	CRect	m_rectImageArrow;

	CFontElementPtr	m_pFontText;
	CFontElementPtr m_pFontStatus;
	CFontElementPtr m_pFontStatusMessage;

	CCOLORREF	m_clrText;	
	CCOLORREF	m_clrStatus;
	CCOLORREF	m_clrStatusMessage;

	int	m_nStatusComboBoxHeight;
	IUOLMessengerImagePtr m_pImageArrow;

	CAtlList<CUserStatusPanelEventListener*> m_listEventListener;
};

MAKEAUTOPTR(CUserStatusPanel);