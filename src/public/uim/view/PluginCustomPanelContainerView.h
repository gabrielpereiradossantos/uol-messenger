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

#include "../controller/PluginCustomPanelContainer.h"
#include "Tooltip/PluginTooltip.h"


class CPluginCustomPanelContainerView : 
	public CWindowImpl<CPluginCustomPanelContainerView>,
	public CPluginCustomPanelContainerObserver
{
public:
	CPluginCustomPanelContainerView();
	virtual ~CPluginCustomPanelContainerView();

	DECLARE_WND_CLASS(_T("PluginCustomPanelContainerView"))

	void Init(IUOLMessengerPluginCustomPanelContainerPtr pPanelContainer);
	void Finalize();

	virtual void GetMinSize(CSize& size);

protected:
	BEGIN_MSG_MAP_EX(CPluginCustomPanelContainerView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_ERASEBKGND(OnEraseBackground)
		MSG_WM_SETCURSOR(OnSetCursor)
	ALT_MSG_MAP(1)
		MSG_WM_MOUSEHOVER(OnMouseHover)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCs);
	void OnDestroy();
	void OnSize(UINT nType, const CSize& size);
	LRESULT OnEraseBackground(HDC hDc);
	LRESULT OnSetCursor(HWND hWnd, UINT nHitTest, UINT uMsg);
	void OnMouseHover(UINT nFlags, CPoint pt);
	void OnMouseLeave();

	virtual void OnAddPanel(IUOLMessengerPluginCustomPanelPtr pPanel);
	virtual void OnRemovePanel(IUOLMessengerPluginCustomPanelPtr pPanel);

private:
	void CreatePanels();
	void DestroyPanels();
	CString GetPanelId(IUOLMessengerPluginCustomPanelPtr pPanel);

	void OnSizeImpl(const CSize& size);
	void TrackMouseMove(HWND hWndChild);

private:
	struct PanelInfo
	{
		IUOLMessengerPluginCustomPanelPtr pPanel;
		HWND hWndPanel;
	};

	typedef CAtlMap<HWND, PanelInfo*> CPanelMap;

	CAtlList<PanelInfo*>	m_listPanels;
	CPanelMap				m_mapPanels;
	CPluginCustomPanelContainerPtr	m_pPanelContainer;

	PanelInfo*	m_pTooltipPanelInfo;
	BOOL		m_bHoverOn;
	CContainedWindow		m_wndTrackingWindow;
	CPluginTooltipPtr		m_pPluginTooltip;
};

MAKEAUTOPTR(CPluginCustomPanelContainerView);