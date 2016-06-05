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
 
// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "skin/windowelement.h"

#include "windowCaption.h"
#include "UIMStatusBarCtrl.h"
#include "skin/borderelement.h"
#include "ImageButton.h"

#include "uolrebarctrl.h"
#include "skin/menuelement.h"

#include "IWindowWithRegion.h"

class CMainFrame;

#include "../controller/commandfactory.h"
#include "IAppThreadCallback.h"

//typedef CWinTraits<WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_SYSMENU | WS_MINIMIZEBOX,

typedef CWinTraits<WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW |
				   WS_EX_APPWINDOW | WS_EX_WINDOWEDGE>		CMainFrameTraits;
typedef CWindowElement<CMainFrame, CFrameWindowImpl<CMainFrame, ATL::CWindow, CMainFrameTraits> > _CMainFrameBase;


class CMainFrame : public _CMainFrameBase,	
				   public CUpdateUI<CMainFrame>,				   
				   public CMessageFilter, 
				   public CIdleHandler,
				   public CUOLMessengerImageButtonEventListener2, 
				   public IWindowWithRegion

{
public:
	DECLARE_FRAME_WND_CLASS(NULL, 128)		

	CMainFrame();
	virtual ~CMainFrame();

	void SetCaptionText(const CString& strText);
	CString GetCaptionText() const;

	void SetStatusBarText(const CString& strText);
	CString GetStatusBarText() const;

	CWindowCaptionPtr GetWindowCaption() const;

	UINT RegisterCustomWindowMessage();
	void UnregisterCustomWindowMessage(UINT /*nId*/);
	
	void CallCustomWindowMessage(UINT nId, IAppThreadCallback* pCallBack);

	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	virtual void UpdateLayout(BOOL bResizeBars = TRUE);
	virtual void GetMinSize(CSize& size);

	virtual BOOL IsRgnEnabled() const;
	virtual BOOL EnableRgn(BOOL bEnable);	


	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

protected:
	BEGIN_MSG_MAP(CMainFrame)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)		
		MSG_WM_SIZE(OnSize)
		MSG_WM_NCHITTEST(OnNcHitTest)		
		MSG_WM_CLOSE(OnClose)		
		MSG_WM_MOVE(OnMove)
		MSG_WM_WINDOWPOSCHANGING(OnWindowPosChanging)				
		MSG_WM_COMMAND(OnCommand)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)		
		//MESSAGE_RANGE_HANDLER(WM_CTLCOLOREDIT, WM_CTLCOLORSTATIC, OnCtlColor)

		MSG_WM_USER(CUSTOM_USER_MESSAGE, OnCustomUserMessage)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(_CMainFrameBase)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(LPCREATESTRUCT lpCs);
	void OnDestroy();	
	void OnSize(UINT, const CSize& size);
	LRESULT OnNcHitTest(const CPoint& pt);	
	//LRESULT OnCtlColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	void OnClose();	
	void OnMove(const CPoint& pt);
	void OnWindowPosChanging(LPWINDOWPOS lpWindowPos);
	void OnCommand(UINT nId, int nCode, HWND hWnd);
	void OnGetMinMaxInfo(LPMINMAXINFO lpMinMaxInfo); 
	void OnCustomUserMessage(WPARAM wParam, LPARAM lParam);

	void OnInitMenu(HMENU hMenu);

	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	virtual void DoPaint(CDCHandle& dc);

	virtual void AddElement(IElementPtr pElement);
	virtual void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint& ptClick);
	virtual BOOL OnActivate(BOOL);
	virtual CRgnHandle GetClippingRgn(const CRect& rectWindow) const;

	virtual IUOLMessengerCommandPtr GetCommand(const CString& strCommand);

private:	
	enum 
	{
		CUSTOM_USER_MESSAGE = WM_USER + 0x100
	};

	BOOL HitTestBorder(const CPoint& ptWindow, int& nTest);
	int GetRelativeWindowYPos() const;

	class CUserMessageMap
	{
	public:	

		CUserMessageMap() 
		{
		}

		UINT GetNextID()
		{
			UINT nId = 0;

			CComCritSecLock<CComAutoCriticalSection> lock(m_cs);

			if (FALSE == m_set.empty())
			{		
				nId = (*m_set.rbegin());//.first;

				TSET::iterator end = m_set.end();
				
				while (m_set.find(++nId) != end) ;
			}

			m_set.insert(nId);

			return nId;
		}

		void ReleaseID(UINT nId)
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_cs);
			
			m_set.erase(nId);		
		}

		void Execute(UINT nId, IAppThreadCallback* pCallback)
		{
			CComCritSecLock<CComAutoCriticalSection> lock(m_cs);
			
			TSET::iterator it =  m_set.find(nId);

			if (it != m_set.end())
			{
				(*pCallback)(nId);				
			}
		}

	private:			
		typedef std::set<UINT> TSET;

		TSET					m_set;
		CComAutoCriticalSection m_cs;
	};

protected:
	IWindowElementPtr		m_pView;	
	CWindowCaptionPtr		m_pWindowCaption;
	CUIMStatusBarCtrlPtr	m_pStatusbar;	
	CMenuElementPtr			m_pMenu;
	CUOLRebarCtrl			m_rebar;

	BOOL					m_bResizable;
	BOOL					m_bShowStatusBar;
	BOOL					m_bCustomRgnEnabled;

	CBorderElementPtr		m_pActiveBorder;
	CBorderElementPtr		m_pInactiveBorder;
	CBorderElement*			m_pCurrentBorder;

private:
	CUserMessageMap			m_mapUserMessage;	
};

MAKEAUTOPTR(CMainFrame);


class CMainFrameCommandFactory
{
public:
	static IUOLMessengerCommandPtr GetCommand(const CString& strCommand, CWindow* pWindow);
};