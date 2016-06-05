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

#include "Skin/Element.h"
#include "Skin/COLORREF.h"
#include "Skin/WindowElement.h"
#include "Skin/BorderElement.h"
#include "Skin/BackgroundElement.h"
#include "image/image.h"
#include "Image/imageBuilder.h"
#include "WindowCaption.h"
#include "IWindowWithRegion.h"

class CMainDialog;
typedef CDummyWindow<CDialogImpl<CMainDialog> > CMainDialogBase;
class CMainDialog : public CMainDialogBase,
					public CElement,
 				    public CUOLMessengerImageButtonEventListener2,
                    public IWindowWithRegion
{
	BEGIN_MSG_MAP(CMainDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_ERASEBKGND(OnEraseBackground);
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ACTIVATE(OnActivate)
        MSG_WM_DESTROY(OnDestroy)		
		MSG_WM_CLOSE(OnClose)
        MSG_WM_MOVE(OnMove)
        MSG_WM_NCHITTEST(OnNcHitTest)
        MSG_WM_WINDOWPOSCHANGING(OnWindowPosChanging)				
		CHAIN_MSG_MAP(CMainDialogBase)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:
	CMainDialog(DWORD dwIDD, const CString& strTitle = _T(""));
	virtual ~CMainDialog();

	void SetTitle(const CString& strTitle);
	CString GetTitle() const;

	void SetIcon(IUOLMessengerImagePtr pIcon);
	IUOLMessengerImagePtr GetIcon() const;

	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);

    virtual void UpdateLayout();
	virtual void GetMinSize(CSize& size);
    virtual void DoPaint(CDCHandle& dc);
    virtual CRgnHandle GetClippingRgn(const CRect& rectWindow) const;

    virtual BOOL IsRgnEnabled() const;
	virtual BOOL EnableRgn(BOOL bEnable);
    virtual void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint& ptClick);
	
protected:
	
	BOOL OnActivate(UINT nActivate, BOOL, HWND);
    BOOL HitTestBorder(const CPoint& ptWindow, int& nTest);
    void OnPaint(HDC hDc);
	void OnClose();
    void DrawBackground(CDCHandle& dc, const CRect& rect);
    void DrawBorder(CDCHandle& dc);
    void SetBackground(CBackgroundElementPtr pActiveBackground, CBackgroundElementPtr pInactiveBackground);
    void OnDestroy();
    void OnMove(const CPoint& pt);
	void OnWindowPosChanging(LPWINDOWPOS lpWindowPos);
    void ProcessCommand(const CString& strCommand);
    LRESULT OnNcHitTest(const CPoint& pt);
    LRESULT OnInitDialog(HWND, LPARAM);
    LRESULT OnEraseBackground(HDC hDc);
	
public:
	DWORD	IDD;

protected:
	BOOL					m_bResizable;
    BOOL					m_bCustomRgnEnabled;
    CRect                   m_rectView;
    CBorderElementPtr		m_pActiveBorder;
	CBorderElementPtr		m_pInactiveBorder;
	CBorderElement*			m_pCurrentBorder;
    CWindowCaptionPtr		m_pWindowCaption;
    CBackgroundElementPtr	m_pActiveBackground;	
	CBackgroundElementPtr	m_pInactiveBackground;		
	CBackgroundElement*		m_pCurrentBackground;	
    int						m_nOpacity;

private:
	IUOLMessengerImagePtr	m_pIcon;
	CString m_strTitle;	

};

MAKEAUTOPTR(CMainDialog);
