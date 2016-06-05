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
#include "./PluginsPropPage.h"

#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"
#include "../skin/SolidBackgroundElement.h"


using namespace GroupListCtrl;


CPluginsPropPage::CPluginsPropPage(void): 
	CPreferencesPropPage(IDD_DIALOG_PLUGIN), 
	m_clrHeader(RGB(255, 255, 255))
{	
	m_pFontHeader = new CFontElement();
	m_pFontHeader->SetFont(_T("MS Sans Serif"), 8);
	m_pFontHeader->SetColor(RGB(0, 0, 0));

	// Create PluginListCtrl
	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
	m_pPluginListCtrl = ap_dynamic_cast<CPluginListCtrlPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__PluginListInstance"));

	if (m_pPluginListCtrl != NULL)
	{
		m_pPluginListCtrl->Init();
	}

	CUOLMessengerPluginManager* pPluginManager = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetPluginManager();
	if (pPluginManager)
	{
		pPluginManager->AddPluginInfoListObserver(this);
	}
}


CPluginsPropPage::~CPluginsPropPage(void)
{
	if (m_pPluginListCtrl != NULL)
	{
		m_pPluginListCtrl->Finalize();
	}

	CUOLMessengerPluginManager* pPluginManager = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetPluginManager();
	if (pPluginManager)
	{
		pPluginManager->RemovePluginInfoListObserver(this);
	}
}


void CPluginsPropPage::PopulatePluginList()
{
	CUOLMessengerPluginManager* pPluginManager = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetPluginManager();
	
	CAtlList<CPluginInfoPtr> listPluginInfo;	
	pPluginManager->GetPluginInfoList(listPluginInfo);	

	CAtlListIterator<CPluginInfoPtr> pPluginInfo = listPluginInfo;

	while (pPluginInfo)
	{
		m_pPluginListCtrl->AddPlugin(*pPluginInfo++);			
	}		
}


LRESULT CPluginsPropPage::OnInitDialog(HWND, LPARAM)
{
	SetMsgHandled(FALSE);

	CRect rect;
	GetClientRect(&rect);

	rect.top += HEADER_HEIGHT;
	rect.right -= 4;
	rect.bottom -= FOOTER_HEIGHT;

	// Load plugin list
	m_pPluginListCtrl->Create(m_hWnd, rect, NULL, WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	PopulatePluginList();

	return FALSE;
}

LRESULT CPluginsPropPage::OnCtlColor(HDC hdc, HWND hwnd)
{
	return (LRESULT) m_brushStatic.m_hBrush;
}

void CPluginsPropPage::OnDestroy()
{
	SetMsgHandled(FALSE);
}


void CPluginsPropPage::OnPaint(HDC hdc)
{
	CPaintDC dcPaint(m_hWnd);

	CRect rectClient;
	GetClientRect(&rectClient);

	CDCHandle dcHandle(dcPaint);

	rectClient.bottom = HEADER_HEIGHT;

	PaintHeader(dcHandle, rectClient);

	SetMsgHandled(FALSE);
}


void CPluginsPropPage::PaintHeader(CDCHandle& dc, const CRect& rect)
{
	CBrush brushBkg;
	brushBkg.CreateSolidBrush(m_clrHeader);

	dc.FillRect(rect, brushBkg);

	CPen penBorder;
	penBorder.CreatePen(PS_SOLID, 1, m_clrBorder);

	HPEN hOldPen = dc.SelectPen(penBorder);

	dc.MoveTo(0, 0);
	dc.LineTo(rect.Width(), 0);
	dc.LineTo(rect.Width(), HEADER_HEIGHT - 1);
	dc.LineTo(0, HEADER_HEIGHT - 1);
	dc.LineTo(0, 0);

	int xColumnSeparator = (2 * BORDER_SIZE) + ACTIVE_COLUMN_WIDTH;
	dc.MoveTo(xColumnSeparator, 0);
	dc.LineTo(xColumnSeparator, HEADER_HEIGHT - 1);

	dc.SelectPen(hOldPen);

	CString strActivateColumnCaption;
	strActivateColumnCaption.LoadString(IDS_PLUGIN_PROPPAGE_HEADER_ACTIVE);

	CRect rectColumn = rect;
	rectColumn.left += BORDER_SIZE;
	rectColumn.right = ACTIVE_COLUMN_WIDTH;
	rectColumn.top += BORDER_SIZE;
	rectColumn.bottom -= BORDER_SIZE;

    m_pFontHeader->DrawText(dc, rectColumn, strActivateColumnCaption, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	CString strDescriptionColumnCaption;
	strDescriptionColumnCaption.LoadString(IDS_PLUGIN_PROPPAGE_HEADER_DESCRIPTION);

	rectColumn.left = (2 * BORDER_SIZE) + ACTIVE_COLUMN_WIDTH + 5;
	rectColumn.right = rect.right - (2 * BORDER_SIZE);

    m_pFontHeader->DrawText(dc, rectColumn, strDescriptionColumnCaption, DT_SINGLELINE | DT_VCENTER);
}


void CPluginsPropPage::OnApply()
{
	m_pPluginListCtrl->EnablePlugins();
}


LRESULT CPluginsPropPage::OnPrefsChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	GetParent().SendMessage(WM_USER_PREFS_CHANGED);

	return 0L;
}


void CPluginsPropPage::OnPluginInfoListChanged()
{
	BOOL bKillReceived = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->KillReceived();
	
	if (!bKillReceived)
	{
		// Reload plugin list
		m_pPluginListCtrl->RemoveAllPlugins();
		PopulatePluginList();
	}
}
