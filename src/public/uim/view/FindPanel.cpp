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
#include "FindPanel.h"
#include "UOLToolbarBuilder.h"
#include "../controller/UIMApplication.h"


////////////////////////////////////////////////////////////////////////////////
// CFindPanelObserverNotifier Class

void CFindPanelObserverNotifier::NotifyAll(void (CFindPanelObserver::*pfnCallback)(CString), CString strText)
{
	CAtlList<CFindPanelObserver*> listObservers;
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);
		
		listObservers.AddTailList(&m_listObservers);
	}
	
	CFindPanelObserver* pObserver;
	
	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver = listObservers.GetNext(pos);
		
		(pObserver->*pfnCallback)(strText);
	}
}


void CFindPanelObserverNotifier::NotifyAll(void (CFindPanelObserver::*pfnCallback)(void))
{
	CAtlList<CFindPanelObserver*> listObservers;
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);
		
		listObservers.AddTailList(&m_listObservers);
	}
	
	CFindPanelObserver* pObserver;
	
	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver = listObservers.GetNext(pos);
		
		(pObserver->*pfnCallback)();
	}
}


void CFindPanelObserverNotifier::NotifyAll2(void (CFindPanelObserver2::*pfnCallback)(void))
{
	CAtlList<CFindPanelObserver*> listObservers;
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csObservers);
		
		listObservers.AddTailList(&m_listObservers);
	}
	
	CFindPanelObserver2* pObserver2;
	
	for (POSITION pos = listObservers.GetHeadPosition(); pos;)
	{
		pObserver2 = dynamic_cast<CFindPanelObserver2*>(listObservers.GetNext(pos));
		
		if (pObserver2)
		{		
			(pObserver2->*pfnCallback)();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// CEditControl Class

void CEditControl::InitializeEditFeatures()
{
	SetSendNewMessage(FALSE);
		
	m_actualFontText = m_inactiveFontText;
	
	m_defaultTextActive = TRUE;
    m_hasFocus = FALSE;

    m_backgroundColor = GetSysColor(COLOR_WINDOW);
	m_hbBackgroundColor = m_pBrush.CreateSolidBrush(m_backgroundColor);
	
	UpdateEditLayout(FALSE);
}

void CEditControl::SetActiveFont(LOGFONT* lf)
{
    m_activeFontText = ::CreateFontIndirect(lf);
}


void CEditControl::SetInactiveFont(LOGFONT* lf)
{
    m_inactiveFontText = ::CreateFontIndirect(lf);
}

void CEditControl::SetActiveTextColor(COLORREF textColor)
{
	m_activeTextColor = textColor;
}


COLORREF CEditControl::GetActiveTextColor()
{
    return m_activeTextColor;
}


void CEditControl::SetInactiveTextColor(COLORREF textColor)
{
	m_inactiveTextColor = textColor;
}


COLORREF CEditControl::GetInactiveTextColor()
{
    return m_inactiveTextColor;
}


BOOL CEditControl::IsDefaultTextActive()
{
    return m_defaultTextActive;
}


WTL::CBrush CEditControl::GetBrush()
{
    return m_pBrush;
}

void CEditControl::FinalizeEditFeatures()
{
	m_activeFontText.DeleteObject();
	m_inactiveFontText.DeleteObject();
    m_actualFontText.DeleteObject();
}


void CEditControl::OnSetFocusEventNotifier(HWND hLostFocusWnd)
{
	m_hasFocus = TRUE;
    UpdateEditLayout(TRUE);
}


void CEditControl::OnKillFocusEventNotifier(HWND hGetFocusWnd)
{
	m_hasFocus = FALSE;
    UpdateEditLayout(FALSE);
}


void CEditControl::UpdateEditLayout(BOOL settingFocus)
{
	if (settingFocus)
	{
		// Get edit string.
		CString strText;
		GetWindowText(strText);
		
		// Is the default text active ?
		if (m_defaultTextActive == TRUE)
		{   
			// Switch the flag off.
			m_defaultTextActive = FALSE;

            strText = "";
			SetWindowText(strText);
			
			SetFont(m_activeFontText, TRUE);
			m_actualFontText = m_activeFontText;			
		}
	}
	else
	{
		// Get edit string.
		CString strText;
		GetWindowText(strText);
		
		// Does it contains text ?
		if (strText.GetLength() == 0)
		{
			// Switch the flag on.
			m_defaultTextActive = TRUE;

            SetWindowText(m_defaultText);
			
			SetFont(m_inactiveFontText, TRUE);
			m_actualFontText = m_inactiveFontText;			
		}
	}

    UpdateWindow();
}

void CEditControl::SetDefaultText(const CString& defaultText)
{
	CString windowText;
	
	m_defaultText = defaultText;
	
	GetWindowText(windowText);
	
	if (windowText.IsEmpty())
	{
		SetWindowText(m_defaultText);
		
		if (m_actualFontText)
		{
			SetFont(m_actualFontText);
		}
	}
}

CString CEditControl::GetDefaultText()
{
	return m_defaultText;
}


void CEditControl::SetBackgroundColor(COLORREF bkColor)
{
	if (bkColor != NULL)
	{
		m_backgroundColor = bkColor;
		
		if (!m_pBrush.IsNull())
		{
			m_pBrush.DeleteObject();
		}
		m_hbBackgroundColor = m_pBrush.CreateSolidBrush(m_backgroundColor);
	}
}


LRESULT CEditControl::OnChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CString strText = GetText();

	WMS_FIND_TEXT_PARAM* pWMSParam = new WMS_FIND_TEXT_PARAM;
	if (pWMSParam)
	{
		pWMSParam->strText = strText;
		pWMSParam->bIsDefaultTextActive = m_defaultTextActive;
		
		::PostMessage(GetParent(), CFindPanel::WM_FIND_PANEL_TEXT_CHANGED, 0, (LPARAM) pWMSParam);
	}

	return 0L;
}


CString CEditControl::GetText()
{
    CString strText;
	GetWindowText(strText);

    return strText;
}


BOOL CEditControl::HasFocus()
{
    return m_hasFocus;
}


LRESULT CEditControl::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;

    int vKey;
    vKey = (int)wParam;

    switch (vKey)
    {
    case VK_ESCAPE: 
		::SendMessage(GetParent(), CFindPanel::WM_FIND_PANEL_ESC_PRESSED, 0, 0);
        break;
    case VK_UP:    
    case VK_DOWN: 
        ::SendMessage(GetParent(), CFindPanel::WM_FIND_PANEL_ARROW_PRESSED, 0, 0);
        break;
	case VK_RETURN: 
		::SendMessage(GetParent(), CFindPanel::WM_FIND_PANEL_RETURN_PRESSED, 0, 0);
        break;
    }

    return 0L;
}


////////////////////////////////////////////////////////////////////////////////
// CFindPanel Class

CFindPanel::CFindPanel()
{
    // Tells "CWMEventNotifier" to not post new messages, but call 
	// the "OnCtlColorEventNotifier" overrided method.
	SetSendNewMessage(FALSE);

    m_pToolbar = new CUOLToolBar();
}


CFindPanel::~CFindPanel()
{
}



void CFindPanel::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		CString strValue;
        GetParameterValue(strValue, pMapParams, "activetextcolor");

        CCOLORREF rgbColor;
        rgbColor.SetColor(strValue);
        m_editControl.SetActiveTextColor(rgbColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "activetextcolor");
		m_editControl.SetActiveTextColor(GetSysColor(COLOR_WINDOWTEXT));
	}

    try
	{
		CString strValue;
        GetParameterValue(strValue, pMapParams, "inactivetextcolor");
        
        CCOLORREF rgbColor;
        rgbColor.SetColor(strValue);
        m_editControl.SetInactiveTextColor(rgbColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "inactivetextcolor");
        m_editControl.SetInactiveTextColor(GetSysColor(COLOR_GRAYTEXT));
	}

    try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "activefont");

		m_pActiveFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
		ATLASSERT(m_pActiveFont.get());
        if (m_pActiveFont.get())
        {
            m_editControl.SetActiveFont(m_pActiveFont->GetFont().get());
        }

	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "activefont");
		// use default system font
	    m_pActiveFont = new CFontElement();
        m_editControl.SetActiveFont(m_pActiveFont->GetFont().get());
	}

    try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "inactivefont");

		CFontElementPtr pFont;
        m_pInactiveFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
		ATLASSERT(m_pInactiveFont.get());
        if (m_pInactiveFont.get())
        {
            m_editControl.SetInactiveFont(m_pInactiveFont->GetFont().get());
        }

	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "inactivefont");
		// use default system font
	    m_pInactiveFont = new CFontElement();
        m_editControl.SetInactiveFont(m_pInactiveFont->GetFont().get());
	}
}


void CFindPanel::DoPaint(CDCHandle& dc)
{
	DrawFindBar(dc);
}


void CFindPanel::UpdateLayout()
{
    CRect rect;
	GetClientRect(&rect);
    CSize size(rect.Width(), rect.Height());

    //Edit control
    int x = rect.left + EDGE;
    int y = rect.top + EDGE;
    int cx = (size.cx - (2 * EDGE));
    int cy = size.cy - (2 * EDGE);

    if (m_editControl)
    {
        m_editControl.SetWindowPos(NULL, x, y, cx, cy,  SWP_NOZORDER);
    }

    //Toolbar control
	CSize sizeToolbar;
    if (m_pToolbar.get())
	{
		m_pToolbar->GetMinSize(sizeToolbar);
    }

    if (m_pToolbar.get())
	{
		m_pToolbar->SetWindowPos(NULL, cx - sizeToolbar.cx - EDGE, -2, sizeToolbar.cx, 
			sizeToolbar.cy, SWP_NOZORDER);
	}
}


LRESULT CFindPanel::OnCreate(LPCREATESTRUCT /*lpcs*/)
{
	//Init Edit Control
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP
                  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | ES_LEFT 
                  | ES_AUTOHSCROLL | ES_AUTOVSCROLL;
    DWORD dwExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR
                    | WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE;
    
    CRect rect;
	GetClientRect(&rect);

    m_editControl.Create(m_hWnd, rect, NULL, dwStyle, dwExStyle);

    if (m_editControl)
    {
        m_editControl.InitializeEditFeatures();
        m_editControl.SetDefaultText(m_defaultText);
    }

    //Create toolbar
    CreateToolbar();

    return 0L;
}


void CFindPanel::OnDestroy()
{
    m_editControl.FinalizeEditFeatures();
    m_editControl.DestroyWindow();
}


void CFindPanel::DrawFindBar(CDCHandle& dc)
{
	CRect rect;
    if (m_pActiveBackground != NULL)
    {
        m_pActiveBackground->Draw(dc, rect);
    }
    __super::DrawBackground(dc, rect);
}

void CFindPanel::GetMinSize(CSize& size)
{
	size = CSize(0, 31);
}


void CFindPanel::SimulateKillFocus()
{
	m_editControl.UpdateEditLayout(FALSE);
}


HBRUSH CFindPanel::OnCtlColorEventNotifier(UINT uMsg, HDC hdc, HWND hWnd)
{
	HBRUSH retBrush = NULL;
	
	if ( (uMsg == WM_CTLCOLOREDIT) && (hWnd == m_editControl.m_hWnd) )
	{
		if (m_editControl.IsDefaultTextActive())
		{
			::SetTextColor(hdc, m_editControl.GetInactiveTextColor());
		}
		else
		{
			::SetTextColor(hdc, m_editControl.GetActiveTextColor());
		}
		
		retBrush = m_editControl.GetBrush();
	}
	
	return retBrush;
}


LRESULT CFindPanel::OnEraseBackground(HDC hDc)
{
	return 1L;
}


IUOLMessengerImageButtonContainerPtr CFindPanel::GetToolbar() const
{
	return m_pToolbar;
}


void CFindPanel::CreateToolbar()
{
	IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();
	CUOLToolBarBuilderPtr pBuilder;

	for (POSITION pos = m_listChilds.GetHeadPosition(); ((pos) && (pBuilder.get() == NULL)); )
	{
		pBuilder = ap_dynamic_cast<CUOLToolBarBuilderPtr>(m_listChilds.GetNext(pos));
	}	

	if (pBuilder)
	{
		m_pToolbar = ap_dynamic_cast<CUOLToolBarPtr>(pBuilder->CreateElement());
		ATLASSERT(m_pToolbar);
	}

	if (m_pToolbar.get())
	{
		CRect rect = CRect(CPoint(0, 0), CSize(0, 0));
		m_pToolbar->Create(m_editControl.m_hWnd, rect);			
		ATLASSERT(m_pToolbar.get());
		m_pToolbar->AddButtonsEventListener(this); 

	}
}

void CFindPanel::ClearText()
{
    if (!m_editControl.IsDefaultTextActive())
    {
		CString strText = "";
        m_editControl.SetWindowText(strText);

        ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "Clean text");
        m_observerNotifier.NotifyAll(&CFindPanelObserver::OnFindPanelTextChanged, strText);

        if (!m_editControl.HasFocus())
        {
            SimulateKillFocus();
        }
    }
}


void CFindPanel::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	CString cmdName = pButton->GetCommandName();
	if (cmdName.CompareNoCase(_T("Clean"))==0)
    {
        ClearText();
    }
}


LRESULT CFindPanel::OnTextChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    WMS_FIND_TEXT_PARAM* pWMSParam = (WMS_FIND_TEXT_PARAM*)lParam;
	if (pWMSParam)
	{
		CString strText = pWMSParam->strText;
		BOOL bIsDefaultTextActive = pWMSParam->bIsDefaultTextActive;
		delete pWMSParam;

		if (bIsDefaultTextActive)
		{
			strText = "";
		}

		m_findText = strText;
	
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, strText);
		m_observerNotifier.NotifyAll(&CFindPanelObserver::OnFindPanelTextChanged, strText);
	}

    return 0L;
}


void CFindPanel::RegisterObserver(CFindPanelObserver* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CFindPanel::UnregisterObserver(CFindPanelObserver* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


void CFindPanel::RegisterObserver2(CFindPanelObserver2* pObserver)
{
	m_observerNotifier.RegisterObserver(pObserver);
}


void CFindPanel::UnregisterObserver2(CFindPanelObserver2* pObserver)
{
	m_observerNotifier.UnregisterObserver(pObserver);
}


void CFindPanel::SetText(CString strText, BOOL bAppend)
{
	if (bAppend)
    {
    	m_editControl.ReplaceSel(strText);
    }
	else
	{
		m_editControl.SetWindowText(strText);
	}
    
	if (!m_editControl.HasFocus())
	{
		m_editControl.UpdateEditLayout(TRUE);
	}

    ATLTRACE(_T("%s - %s\n"), __FUNCTION__, strText);
    m_observerNotifier.NotifyAll(&CFindPanelObserver::OnFindPanelTextChanged, strText);
}


void CFindPanel::SetFocus()
{
    m_editControl.SetFocus();
}


BOOL CFindPanel::HasFocus()
{
	return m_editControl.HasFocus();
}


CString CFindPanel::GetText() const
{
    return m_findText;
}

BOOL CFindPanel::IsFinding() const
{
    return !m_findText.IsEmpty();
}


LRESULT CFindPanel::OnFindPanelEscPressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ClearText();
	
    m_observerNotifier.NotifyAll(&CFindPanelObserver::OnFindPanelEscPressed);
    
	return 0L;
}


LRESULT CFindPanel::OnArrowPressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    m_observerNotifier.NotifyAll(&CFindPanelObserver::OnFindPanelArrowKeyPressed);
    
    return 0L;
}


LRESULT CFindPanel::OnReturnPressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (IsFinding() && HasFocus())
	{
		m_observerNotifier.NotifyAll2(&CFindPanelObserver2::OnFindPanelReturnKeyPressed);
	}
    
    return 0L;
}