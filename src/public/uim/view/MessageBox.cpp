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
 
// MessageBox.cpp : implementation of the CMessageBox class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MessageBox.h"
#include "DialogManager.h"

CMessageBox::CMessageBox(void):
    CMainDialog(IDD_DIALOG_MESSAGEBOX),
    m_MessageBoxFlags(MB_OK | MB_ICONINFORMATION)
{
}

CMessageBox::~CMessageBox(void)
{
    if (IsWindow())
    {
        CDialogManager::GetInstance()->UnregisterDialog(m_hWnd);
        EndDialog(IDCANCEL);
    }
}

void CMessageBox::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
    __super::SetParameters(pMapParams, pElementTable);
    
    try
	{
		CString strIconError;
		GetParameterValue(strIconError, pMapParams, _T("icon_error"));

		if (FALSE == strIconError.IsEmpty())
		{
			IElementPtr pElement = pElementTable->GetElement(strIconError);

			if (pElement)
			{
				m_pIconError = ap_dynamic_cast<CStaticIconElementPtr>(pElement);
			}
		}
	}
	catch (const CSkinException&)
	{
	}

    try
	{
		CString strIconInformation;
		GetParameterValue(strIconInformation, pMapParams, _T("icon_information"));

		if (FALSE == strIconInformation.IsEmpty())
		{
			IElementPtr pElement = pElementTable->GetElement(strIconInformation);

			if (pElement)
			{
				m_pIconInformation = ap_dynamic_cast<CStaticIconElementPtr>(pElement);
			}
		}
	}
	catch (const CSkinException&)
	{
	}

    try
	{
		CString strIconQuestion;
		GetParameterValue(strIconQuestion, pMapParams, _T("icon_question"));

		if (FALSE == strIconQuestion.IsEmpty())
		{
			IElementPtr pElement = pElementTable->GetElement(strIconQuestion);

			if (pElement)
			{
				m_pIconQuestion = ap_dynamic_cast<CStaticIconElementPtr>(pElement);
			}
		}
	}
	catch (const CSkinException&)
	{
	}

    try
	{
		CString strIconWarning;
		GetParameterValue(strIconWarning, pMapParams, _T("icon_warning"));

		if (FALSE == strIconWarning.IsEmpty())
		{
			IElementPtr pElement = pElementTable->GetElement(strIconWarning);

			if (pElement)
			{
				m_pIconWarning = ap_dynamic_cast<CStaticIconElementPtr>(pElement);
			}
		}
	}
	catch (const CSkinException&)
	{
	}

    try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "font");

		m_pFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
		ATLASSERT(m_pFont.get());

	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "font");
		// use default system font
		m_pFont = new CFontElement();
	}
}

void CMessageBox::SetMessageBoxFlags(UINT msgboxFlags)
{
    m_MessageBoxFlags = msgboxFlags; 
}

void CMessageBox::SetMessageBoxTitle(CString strTitle)
{
    m_Title = strTitle;
}

void CMessageBox::SetMessageBoxText(CString strText)
{
    m_Text = strText;
}

void CMessageBox::DoPaint(CDCHandle& dc)
{
	__super::DoPaint(dc);

    if (m_pIconError && (((m_MessageBoxFlags & MB_ICONMASK) == MB_ICONERROR) |
                         ((m_MessageBoxFlags & MB_ICONMASK) == MB_ICONSTOP)  |
                         ((m_MessageBoxFlags & MB_ICONMASK) == MB_ICONHAND)))
	{
		IUOLMessengerImageDrawStrategyPtr pImageDrawStrategy = m_pIconError->GetImageDrawStrategy();

		if (pImageDrawStrategy)
		{
			pImageDrawStrategy = CUIMApplication::GetApplication()->GetUIManager()->CreateDrawStrategy(IUOLMessengerUIManager::IDS_DRAW_TRANSPARENT_STRETCHED);
		}

		IUOLMessengerImagePtr pImage = m_pIconError->GetImage();

		if (pImage)
		{
			pImage->Draw(CDCHandle(dc), m_rectMsgIcon.left, m_rectMsgIcon.top, 
				m_rectMsgIcon.Width(), m_rectMsgIcon.Height(), pImageDrawStrategy);
		}
	}

    if (m_pIconInformation && (((m_MessageBoxFlags & MB_ICONMASK) == MB_ICONINFORMATION) |
                               ((m_MessageBoxFlags & MB_ICONMASK) == MB_ICONASTERISK)))
	{
		IUOLMessengerImageDrawStrategyPtr pImageDrawStrategy = m_pIconInformation->GetImageDrawStrategy();

		if (pImageDrawStrategy)
		{
			pImageDrawStrategy = CUIMApplication::GetApplication()->GetUIManager()->CreateDrawStrategy(IUOLMessengerUIManager::IDS_DRAW_TRANSPARENT_STRETCHED);
		}

		IUOLMessengerImagePtr pImage = m_pIconInformation->GetImage();

		if (pImage)
		{
			pImage->Draw(CDCHandle(dc), m_rectMsgIcon.left, m_rectMsgIcon.top, 
				m_rectMsgIcon.Width(), m_rectMsgIcon.Height(), pImageDrawStrategy);
		}
	}

    if (m_pIconQuestion && ((m_MessageBoxFlags & MB_ICONMASK) == MB_ICONQUESTION))
	{
		IUOLMessengerImageDrawStrategyPtr pImageDrawStrategy = m_pIconQuestion->GetImageDrawStrategy();

		if (pImageDrawStrategy)
		{
			pImageDrawStrategy = CUIMApplication::GetApplication()->GetUIManager()->CreateDrawStrategy(IUOLMessengerUIManager::IDS_DRAW_TRANSPARENT_STRETCHED);
		}

		IUOLMessengerImagePtr pImage = m_pIconQuestion->GetImage();

		if (pImage)
		{
			pImage->Draw(CDCHandle(dc), m_rectMsgIcon.left, m_rectMsgIcon.top, 
				m_rectMsgIcon.Width(), m_rectMsgIcon.Height(), pImageDrawStrategy);
		}
	}

    if (m_pIconWarning && (((m_MessageBoxFlags & MB_ICONMASK) == MB_ICONWARNING) |
                           ((m_MessageBoxFlags & MB_ICONMASK) == MB_ICONEXCLAMATION)))
	{
		IUOLMessengerImageDrawStrategyPtr pImageDrawStrategy = m_pIconWarning->GetImageDrawStrategy();

		if (pImageDrawStrategy)
		{
			pImageDrawStrategy = CUIMApplication::GetApplication()->GetUIManager()->CreateDrawStrategy(IUOLMessengerUIManager::IDS_DRAW_TRANSPARENT_STRETCHED);
		}

		IUOLMessengerImagePtr pImage = m_pIconWarning->GetImage();

		if (pImage)
		{
			pImage->Draw(CDCHandle(dc), m_rectMsgIcon.left, m_rectMsgIcon.top, 
				m_rectMsgIcon.Width(), m_rectMsgIcon.Height(), pImageDrawStrategy);
		}
	}
}

LRESULT CMessageBox::OnInitDialog(HWND, LPARAM)
{
    __super::OnInitDialog(NULL, NULL);

    CRect rectMsgIcon;
    CSize sizeWindowCaption = CSize(0, 0);
	m_pWindowCaption->GetMinSize(sizeWindowCaption);
    rectMsgIcon = CRect(CPoint(DM_INTERNAL_SPACE, DM_INTERNAL_SPACE + sizeWindowCaption.cy - 1), 
        CSize(DM_IMAGE_WIDTH, DM_IMAGE_HEIGHT));
	m_rectMsgIcon = rectMsgIcon;

    int btnY = m_rectView.Height();
    
    switch (m_MessageBoxFlags & MB_TYPEMASK)
    {
    case MB_OKCANCEL:
        {
            CWindow btnOk = GetDlgItem(IDOK);
            CRect rectOk;
            btnOk.GetWindowRect(&rectOk);

            CWindow btnCancel = GetDlgItem(IDCANCEL);
            CRect rectCancel;
            btnCancel.GetWindowRect(&rectCancel);

            //calculate buttons positions
            int x, y;
            int m = m_rectView.Width() / 2;
            x = (m - rectOk.Width()) - (DM_BUTTON_HSPACE / 2);
            y = (m_rectView.bottom - rectOk.Height()) - DM_BUTTON_VSPACE;

            btnOk.MoveWindow(x, y, rectOk.Width(), rectOk.Height(), TRUE);
            btnOk.ShowWindow(SW_SHOW);

            x = (x + rectCancel.Width()) + (DM_BUTTON_HSPACE / 2);
            y = (m_rectView.bottom - rectCancel.Height()) - DM_BUTTON_VSPACE;                
            btnY = y;

            btnCancel.MoveWindow(x, y, rectCancel.Width(), rectCancel.Height(), TRUE);
            btnCancel.ShowWindow(SW_SHOW);
        }
        break;
    case MB_YESNO:
        {
            CWindow btnYes = GetDlgItem(IDYES);
            CRect rectYes;
            btnYes.GetWindowRect(&rectYes);

            CWindow btnNo = GetDlgItem(IDNO);
            CRect rectNo;
            btnNo.GetWindowRect(&rectNo);

            //calculate buttons positions
            int x, y;
            int m = m_rectView.Width() / 2;
            x = (m - rectYes.Width()) - (DM_BUTTON_HSPACE / 2);
            y = (m_rectView.bottom - rectYes.Height()) - DM_BUTTON_VSPACE;

            btnYes.MoveWindow(x, y, rectYes.Width(), rectYes.Height(), TRUE);
            btnYes.ShowWindow(SW_SHOW);

            x = (x + rectNo.Width()) + (DM_BUTTON_HSPACE / 2);
            y = (m_rectView.bottom - rectNo.Height()) - DM_BUTTON_VSPACE;                
            btnY = y;

            btnNo.MoveWindow(x, y, rectNo.Width(), rectNo.Height(), TRUE);
            btnNo.ShowWindow(SW_SHOW);
        }
        break;

    default:
    case MB_OK:
        {
            CWindow btnOk = GetDlgItem(IDOK);
            CRect rectOk;
            btnOk.GetWindowRect(&rectOk);

            //calculate button position
            int x = (m_rectView.Width() - rectOk.Width()) / 2;
            int y = (m_rectView.bottom - rectOk.Height()) - DM_BUTTON_VSPACE;
            btnY = y;

            btnOk.MoveWindow(x, y, rectOk.Width(), rectOk.Height(), TRUE);
            btnOk.ShowWindow(SW_SHOW);
        }
        break;
    }

    //Set title and text
    SetTitle(m_Title);

    //Calculate Static position
    int x, y, cx, cy;
    x = rectMsgIcon.left + rectMsgIcon.Width() + DM_STATIC_HSPACE;
    y = m_rectView.top + DM_STATIC_VSPACE;
    cx = m_rectView.Width() - x - DM_STATIC_HSPACE;
    cy = btnY - DM_STATIC_VSPACE - y; 

    m_StaticText.SubclassWindow(GetDlgItem(IDC_STATIC_TEXT));
	m_StaticText.SetWindowText(m_Text);
    m_StaticText.MoveWindow(x, y, cx, cy, TRUE);
    
    // Set Static font
    LOGFONTPtr pLogFont = m_pFont->GetFont();
	ATLASSERT(pLogFont);
	m_FontText.CreateFontIndirect(pLogFont.get());
    m_StaticText.SetFont(m_FontText);
    m_StaticText.SetColor(m_pFont->GetColor());

    CDialogManager::GetInstance()->RegisterDialog(m_hWnd);

    return FALSE;
}

LRESULT CMessageBox::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
    EndDialog(wID);

	return 0;
}

LRESULT CMessageBox::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	EndDialog(wID);

	return 0;
}

LRESULT CMessageBox::OnYes(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
    EndDialog(wID);

	return 0;
}

LRESULT CMessageBox::OnNo(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	EndDialog(wID);

	return 0;
}
