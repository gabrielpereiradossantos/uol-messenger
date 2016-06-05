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

#include "../resource.h"
#include "../controller/UIMApplication.h"
#include "../controller/UIManager.h"
#include "HistoryFrame.h"


CHistoryFrame::CHistoryFrame()	
{
	m_pView = new  CHistoryWindow();
}


CHistoryFrame::~CHistoryFrame()
{
}


void CHistoryFrame::SetHistoryRecordList(IUOLMessengerHistoryRecordListPtr pHistoryRecordList)
{
	static_cast<CHistoryWindow*>(m_pView.get())->SetHistoryRecordList(pHistoryRecordList);
}


void CHistoryFrame::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "font");

		m_pFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
		ATLASSERT(m_pFont.get());

		static_cast<CHistoryWindow*>(m_pView.get())->SetFont(m_pFont); 
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "font");
		// use default system font
		m_pFont = new CFontElement();
	}

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "HighlightTextColor");

		m_clrHighlightText.SetColor(strColor);
		static_cast<CHistoryWindow*>(m_pView.get())->SetHighlightTextColor(m_clrHighlightText); 
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "HighlightTextColor");
		// use default color
		m_clrHighlightText.SetColor("ffff80");
	}
}


HWND CHistoryFrame::CreateHistoryWindow(HWND hWndParent)
{
	CRect rectWindow(100, 100, 630, 580);

	IUOLMessengerMessageSettings4Ptr pMessageSettings4 = ap_dynamic_cast<IUOLMessengerMessageSettings4Ptr>(CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
	if( NULL != pMessageSettings4)
	{
		int nCordX = pMessageSettings4->GetHistoryWindowCordX();
		int nCordY = pMessageSettings4->GetHistoryWindowCordY();
		int nSizeX = pMessageSettings4->GetHistoryWindowSizeX();
		int nSizeY = pMessageSettings4->GetHistoryWindowSizeY();

		rectWindow = CRect(CPoint(nCordX, nCordY), CSize(nSizeX, nSizeY));
	}

	return __super::CreateEx(hWndParent, rectWindow);
}


void CHistoryFrame::UpdateLayout(BOOL bResizeBars)
{
	__super::UpdateLayout(bResizeBars);
}


LRESULT CHistoryFrame::OnCreate(LPCREATESTRUCT lpCs)
{
	__super::OnCreate(lpCs);

	return 0L;
}


CString CHistoryFrame::GetName() const
{
	return "__HistoryWindowInstance";
}


IUOLMessengerHistoryWindow2* CHistoryFrame::getHistoryWindow()
{
	return static_cast<CHistoryWindow*>(m_pView.get());
}


void CHistoryFrame::UpdateEventsList()
{
	static_cast<CHistoryWindow*>(m_pView.get())->UpdateEventsList();
}


void CHistoryFrame::OnClose()
{
	WINDOWPLACEMENT wp;
	if (::GetWindowPlacement(m_hWnd, &wp))
	{
		IUOLMessengerMessageSettings4Ptr pMessageSettings4 = ap_dynamic_cast<IUOLMessengerMessageSettings4Ptr>(CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
		if( NULL != pMessageSettings4)
		{
			int nCordX = wp.rcNormalPosition.left;
			int nCordY = wp.rcNormalPosition.top;
			int nSizeX = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
			int nSizeY = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;

			pMessageSettings4->SetHistoryWindowCordX(nCordX);
			pMessageSettings4->SetHistoryWindowCordY(nCordY);
			pMessageSettings4->SetHistoryWindowSizeX(nSizeX);
			pMessageSettings4->SetHistoryWindowSizeY(nSizeY);

			CUIMApplication::GetApplication()->GetSettingsManager()->SaveSettings();
		}
	}

	static_cast<CHistoryWindow*>(m_pView.get())->OnClose();

	SetMsgHandled(FALSE);
}