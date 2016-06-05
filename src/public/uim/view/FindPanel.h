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

#include "WMEventNotifier.h"
#include "skin/WindowElement.h"
#include "skin/FontElement.h"
#include "skin/COLORREF.h"
#include "DummyWindow.h"
#include "UOLToolbar.h"
#include "../ObserverRegister.h"

#include <interfaces/IUOLMessengerAccountManager.h>
#include <interfaces/IUOLMessengerAccountConnectionManager.h>
#include <interfaces/IUOLMessengerFindPanel.h>

struct WMS_FIND_TEXT_PARAM
{
	CString strText;
	BOOL bIsDefaultTextActive;
};

////////////////////////////////////////////////////////////////////////////////
// CEditControl Class
class CEditControl;
typedef CWindowImpl<CEditControl, CEdit> _CEditControlBase;

class CEditControl : 
    public _CEditControlBase, 
    public CWMEventNotifier<CEditControl>    
{
public:
    void SetDefaultText(const CString& defaultText);
	CString GetDefaultText();	
    void SetActiveFont(LOGFONT* lf);
    void SetInactiveFont(LOGFONT* lf);
    void SetActiveTextColor(COLORREF textColor);
    COLORREF GetActiveTextColor();
	void SetInactiveTextColor(COLORREF textColor);
    COLORREF GetInactiveTextColor();
    void SetBackgroundColor(COLORREF bkColor);
    BOOL IsDefaultTextActive();
    WTL::CBrush GetBrush();
    CString GetText();
    BOOL HasFocus();
    
    COLORREF		m_backgroundColor;

protected:
    CString			m_defaultText;
    CFont			m_activeFontText;
    CFont			m_inactiveFontText;
    CFont			m_actualFontText;
    COLORREF		m_inactiveTextColor;
    COLORREF		m_activeTextColor;
    WTL::CBrush		m_pBrush;
    HBRUSH			m_hbBackgroundColor;    
    BOOL			m_defaultTextActive;
    BOOL            m_hasFocus;

    BEGIN_MSG_MAP(CEditControl)
        MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
        REFLECTED_COMMAND_CODE_HANDLER(EN_CHANGE, OnChanged)
        CHAIN_MSG_MAP(CWMEventNotifier<CEditControl>)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    virtual void OnSetFocusEventNotifier(HWND hLostFocusWnd);
    virtual void OnKillFocusEventNotifier(HWND hGetFocusWnd);
    
    LRESULT OnChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    void InitializeEditFeatures();
    void FinalizeEditFeatures();
    void UpdateEditLayout(BOOL settingFocus);
};


////////////////////////////////////////////////////////////////////////////////
// CFindPanelObserverNotifier Class
class CFindPanelObserverNotifier : public CObserverRegister<CFindPanelObserver>
{
public:
    void NotifyAll(void (CFindPanelObserver::*pfnCallback)(CString), CString strText);
    void NotifyAll(void (CFindPanelObserver::*pfnCallback)(void));
	void NotifyAll2(void (CFindPanelObserver2::*pfnCallback)(void));
};


////////////////////////////////////////////////////////////////////////////////
// CFindPanel Class
class CFindPanel;
typedef CWindowElement<CFindPanel, CDummyWindow<CWindowImpl<CFindPanel> > > _CFindPanelBase;

class CFindPanel : 
    public _CFindPanelBase,
	public IUOLMessengerFindPanel2, 
    public CWMEventNotifier<CFindPanel>,
    public CUOLMessengerImageButtonEventListener2
{
public:
    static const int EDGE = 4;

	enum 
	{
		WM_FIND_PANEL_TEXT_CHANGED     = WM_USER + 0x020,
		WM_FIND_PANEL_ESC_PRESSED      = WM_USER + 0x030,
		WM_FIND_PANEL_ARROW_PRESSED    = WM_USER + 0x040,
		WM_FIND_PANEL_RETURN_PRESSED   = WM_USER + 0x050
	};

	CFindPanel();
	virtual ~CFindPanel();

	void DoPaint(CDCHandle& dc);
	void UpdateLayout();
    virtual void GetMinSize(CSize& size);
	void SimulateKillFocus();

    void RegisterObserver(CFindPanelObserver* pObserver);
	void UnregisterObserver(CFindPanelObserver* pObserver);
	void RegisterObserver2(CFindPanelObserver2* pObserver);
	void UnregisterObserver2(CFindPanelObserver2* pObserver);
    void SetText(CString strText, BOOL bAppend = TRUE);
    void SetFocus();
	BOOL HasFocus();

    CString GetText() const;
    BOOL IsFinding() const;

protected:
	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
    virtual HBRUSH OnCtlColorEventNotifier(UINT uMsg, HDC hdc, HWND hWnd);
    virtual IUOLMessengerImageButtonContainerPtr GetToolbar() const;
    
    // CImageButtonEventListener methods
	void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick);

	BEGIN_MSG_MAP(CFindPanel)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
        MSG_WM_ERASEBKGND(OnEraseBackground)
        MESSAGE_HANDLER(WM_FIND_PANEL_TEXT_CHANGED, OnTextChanged)
        MESSAGE_HANDLER(WM_FIND_PANEL_ESC_PRESSED, OnFindPanelEscPressed)
        MESSAGE_HANDLER(WM_FIND_PANEL_ARROW_PRESSED, OnArrowPressed)
		MESSAGE_HANDLER(WM_FIND_PANEL_RETURN_PRESSED, OnReturnPressed)
        CHAIN_MSG_MAP(_CFindPanelBase)
        CHAIN_MSG_MAP(CWMEventNotifier<CFindPanel>)		
        REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
    LRESULT OnEraseBackground(HDC hDc);
    LRESULT OnTextChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnFindPanelEscPressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnArrowPressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnReturnPressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnAccelaratorEscape(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    
	void OnDestroy();
	void OnLButtonDblClick(UINT nFlags, CPoint& ptClick);    
	void DrawFindBar(CDCHandle& dc);
    void CreateToolbar();
    void ClearText();

protected:
    CString                             m_defaultText;
	
private:
    CString								m_findText;
    CEditControl                        m_editControl;	
    CFontElementPtr                     m_pActiveFont;
    CFontElementPtr                     m_pInactiveFont;
    CUOLToolBarPtr			            m_pToolbar;
    CFindPanelObserverNotifier			m_observerNotifier;
};

MAKEAUTOPTR(CFindPanel);
