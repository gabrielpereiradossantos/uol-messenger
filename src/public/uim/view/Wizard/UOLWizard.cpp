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
#include "./uolwizard.h"
#include "MinDependencySpecialFolder.h"
#include "../../resource.h"

#include "../skin/solidbackgroundelement.h"
#include "../skin/AppDefaultIconElement.h"

#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"
#include "../../../protocols/improtocolfactory.h"



CUOLWizard::CUOLWizard(int nIDD) : 
	IDD(nIDD), 
	m_bCenterWindow(TRUE),
	m_rectInitial(0, 0, 0, 0)
{
	m_BtnFont.CreateFont(16, 0, 1,
			1, FW_NORMAL, FALSE, FALSE,
			FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
			_T("Arial"));
	
	m_pWizardSettings = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
    
    // Set Wizard is running in Purple service...
	CIMProtocolFactory::GetInstance()->SetWizardRunnig(TRUE);
}

CUOLWizard::~CUOLWizard(void)
{
	CloseAllProps();
    
    // Set Wizard is not running in Purple service...
	CIMProtocolFactory::GetInstance()->SetWizardRunnig(FALSE);
}

void CUOLWizard::SetInitialRect(const CRect& rect)
{
	m_rectInitial = rect;
}

void CUOLWizard::SetCenterWindow(BOOL bCenterWindow)
{
	m_bCenterWindow = bCenterWindow;
}

void CUOLWizard::SetPage(CUOLPropPagePtr pWindow)
{
	ATLASSERT(pWindow.get());

	if (pWindow)
	{
		pWindow->SetUOLWizard(this);

		if (FALSE == m_listPropPages.Find(pWindow))
		{
			m_listPropPages.AddTail(pWindow);

			if (m_hWnd)
			{
				CreatePropWindow(pWindow);
			}			
		}


		CUOLPropPage* pTemp = m_pWindow.get();

		m_pWindow = pWindow;		

		if (m_pWindow->m_hWnd)
		{	
			m_pWindow->ShowWindow(SW_SHOW);
		}

		if (pTemp)
		{
			DestroyPropWindow(pTemp);
		}

		SetupBtns();
	}	
}

int CUOLWizard::GetDefaultButtonHeight() const
{
	return 24;
}

int CUOLWizard::GetDefaultButtonSpacement() const
{
	return 10;
}

int CUOLWizard::GetDefaultButtonWidth() const
{
	return 80;
}

void CUOLWizard::ShowBtnPrevious(BOOL bShow)
{
	ATLASSERT(m_btnPrev.IsWindow());

	if (m_btnPrev.IsWindow())
	{	
		m_btnPrev.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	}
}

void CUOLWizard::ShowBtnNext(BOOL bShow)
{
	ATLASSERT(m_btnNext.IsWindow());

	if (m_btnNext.IsWindow())
	{	
		m_btnNext.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	}
}

void CUOLWizard::ShowBtnCancel(BOOL bShow)
{
	ATLASSERT(m_btnCancel.IsWindow());

	if (m_btnCancel.IsWindow())
	{	
		m_btnCancel.ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	}
}

void CUOLWizard::CreateBtnCustom(int x, int width)
{
	ATLASSERT(m_pWindow.get());

	if (m_pWindow.get())
	{
		m_btnCustom.Create(m_hWnd, 0, 0, WS_CHILD | WS_TABSTOP, 0, ID_BTNCUSTOM);
		ATLASSERT(::IsWindow(m_btnCustom));	

		m_btnCustom.ShowWindow(SW_HIDE);
		CRect rectClient;
		GetClientRect(&rectClient);

		const int DEFAULT_BUTTON_SIZE = GetDefaultButtonWidth();
		const int DEFAULT_BUTTON_SPACEMENT = GetDefaultButtonSpacement();
		const int DEFAULT_BUTTON_HEIGHT = GetDefaultButtonHeight();

		CRect rectBtn(x, 
					  rectClient.bottom - (5 + DEFAULT_BUTTON_HEIGHT),
					  x + width, 
					  rectClient.bottom - 5);


		m_btnCustom.SetWindowPos(NULL, &rectBtn, SWP_NOZORDER);

		m_btnCustom.SetFont(m_BtnFont);
	}
}

void CUOLWizard::ShowBtnCustom(BOOL bShow, const CString& strText)
{
	if (bShow)
	{
        m_btnCustom.SetWindowText(strText);
        m_btnCustom.ShowWindow(SW_NORMAL);
	}
	else
	{
        m_btnCustom.ShowWindow(SW_HIDE);
	}
}


void CUOLWizard::SetDefaultButton(int nIdButton)
{
	if (IsWindow())
	{
		SendMessage(DM_SETDEFID, (WPARAM) nIdButton);
	}
}

void CUOLWizard::ShowNextPage()
{
	ATLASSERT(m_pWindow.get());

	if (m_pWindow)
	{
		if (m_pWindow->OnBtnNext())
		{
			SetPage(m_pWindow->GetNextPage(m_pWindow));
		}
	}
}

UOLLib::ISettings* CUOLWizard::GetWizardSettings()
{
	return m_pWizardSettings;
}


LRESULT CUOLWizard::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	if (FALSE == m_rectInitial.IsRectEmpty())
	{
		SetWindowPos(NULL, &m_rectInitial, SWP_NOZORDER);
	}

	CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());

	CAppDefaultIconElementPtr pAppDefaultIcon = ap_dynamic_cast<CAppDefaultIconElementPtr>(pUIManagerPtr->GetWindowManager()->GetElement("__AppDefaultIconInstance"));

	if (pAppDefaultIcon)
	{
		CIconElementPtr pIconSmall = pAppDefaultIcon->GetSmallIcon();

		if (pIconSmall)
		{
			SetIcon(pIconSmall->GetIcon(), FALSE);
		}

		CIconElementPtr pIconLarge = pAppDefaultIcon->GetLargeIcon();

		if (pIconLarge)
		{
			SetIcon(pIconLarge->GetIcon(), TRUE);
		}
	}

	CString strTitle = m_pWizardSettings->GetValue(_T("uol.resource.IDS_APP_NAME"));

	ATLASSERT(FALSE == strTitle.IsEmpty());
	SetWindowText(strTitle);

	if (m_bCenterWindow)
	{
		CenterWindow();
	}

	CreateBtns();
	SetupBtns();

	UpdateWindow();

	CreatePropWindow(m_pWindow);

	SetMsgHandled(FALSE);

	return 0L;
}

void CUOLWizard::OnPrev(UINT, int, HWND)
{
	ATLASSERT(m_pWindow.get());

	if (m_pWindow)
	{
		if (m_pWindow->OnBtnPrevious())
		{
			SetPage(m_pWindow->GetPreviousPage(m_pWindow));
		}
	}
}

void CUOLWizard::OnNext(UINT, int, HWND)
{
	ATLASSERT(m_pWindow.get());

	if (m_pWindow)
	{
		if (m_pWindow->OnBtnNext())
		{
			SetPage(m_pWindow->GetNextPage(m_pWindow));
		}
	}
}

void CUOLWizard::OnCancel(UINT, int, HWND)
{
	ATLASSERT(m_pWindow.get());

	if (m_pWindow)
	{
		if (m_pWindow->OnBtnCancel())
		{
			EndDialog(IDCANCEL);
		}
	}
}

void CUOLWizard::OnCommand(UINT uMsg, int nId, HWND hWnd)
{
	ATLASSERT(m_pWindow.get());

	if (m_pWindow)
	{
		m_pWindow->OnBtnCustom(m_pWindow, nId);
	}
}

void CUOLWizard::CreateBtns()
{
	ATLASSERT(m_pWindow.get());

	if (m_pWindow.get())
	{
		m_btnPrev.Create(m_hWnd, 0, 0, WS_CHILD | WS_TABSTOP, 0, ID_PREV_PANE);
		ATLASSERT(::IsWindow(m_btnPrev));

		m_btnNext.Create(m_hWnd, 0, 0, WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON, 0, ID_NEXT_PANE);
		ATLASSERT(::IsWindow(m_btnNext));

		m_btnCancel.Create(m_hWnd, 0, 0, WS_CHILD | WS_TABSTOP, 0, IDCANCEL);
		ATLASSERT(::IsWindow(m_btnCancel));	

		m_btnCancel.SetFont(m_BtnFont);
		m_btnCancel.ShowWindow(SW_NORMAL);
		
		m_btnNext.SetFont(m_BtnFont);
		m_btnNext.ShowWindow(SW_NORMAL);
		
		m_btnPrev.SetFont(m_BtnFont);
		m_btnPrev.ShowWindow(SW_NORMAL);

		SetDefaultButton(ID_NEXT_PANE);
	}
}

void CUOLWizard::SetupBtns()
{
	if (::IsWindow(m_btnPrev) &&
		::IsWindow(m_btnNext) &&
		::IsWindow(m_btnCancel))
	{
		CRect rectClient;
		GetClientRect(&rectClient);

		int x = rectClient.right;
		const int DEFAULT_BUTTON_SIZE = GetDefaultButtonWidth();
		const int DEFAULT_BUTTON_SPACEMENT = GetDefaultButtonSpacement();
		const int DEFAULT_BUTTON_HEIGHT = GetDefaultButtonHeight();

		m_btnCancel.SetWindowText(m_pWindow->GetCancelBtnText());
		m_btnNext.SetWindowText(m_pWindow->GetNextBtnText());
		m_btnPrev.SetWindowText(m_pWindow->GetPreviousBtnText());

		m_btnPrev.EnableWindow(m_pWindow->HasPreviousPage());
		m_btnNext.EnableWindow(m_pWindow->HasNextPage());		

		CRect rectBtn(rectClient.right - (DEFAULT_BUTTON_SPACEMENT + DEFAULT_BUTTON_SIZE), 
					  rectClient.bottom - (5 + DEFAULT_BUTTON_HEIGHT),
					  rectClient.right - DEFAULT_BUTTON_SPACEMENT, 
					  rectClient.bottom - 5);


		m_btnCancel.SetWindowPos(NULL, &rectBtn, SWP_NOZORDER);

		rectBtn -= CPoint(DEFAULT_BUTTON_SPACEMENT + DEFAULT_BUTTON_SIZE, 0);

		m_btnNext.SetWindowPos(NULL, &rectBtn, SWP_NOZORDER);

		rectBtn -= CPoint(DEFAULT_BUTTON_SPACEMENT + DEFAULT_BUTTON_SIZE, 0);

		m_btnPrev.SetWindowPos(NULL, &rectBtn, SWP_NOZORDER);
		
	}
}

void CUOLWizard::CreatePropWindow(CUOLPropPagePtr pWindow)
{
	CRect rect = m_rectInitial;
	rect.bottom -= 60;			
	
	pWindow->Create(m_hWnd);				
	ATLASSERT(pWindow->IsWindow());

	if (pWindow->IsWindow())
	{
		pWindow->SetWindowPos(NULL, &rect, SWP_NOZORDER);
		pWindow->ShowWindow(SW_SHOW);
		pWindow->SetUOLWizard(this);
	}
}

void CUOLWizard::DestroyPropWindow(CUOLPropPage* pWindow)
{
	if (pWindow->m_hWnd)
	{
		pWindow->DestroyWindow();
	}
}

void CUOLWizard::CloseAllProps()
{
	for (POSITION pos = m_listPropPages.GetHeadPosition(); pos; )
	{
		CUOLPropPagePtr pPropPage =  m_listPropPages.GetNext(pos);

		//if (pPropPage->m_hWnd)
		//{
		//	pPropPage->DestroyWindow();
		//}

		pPropPage->Finalize();
	}

	m_listPropPages.RemoveAll();

	m_pWindow.reset();
}