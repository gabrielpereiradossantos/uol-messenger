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

#include "../DummyWindow.h"
#include "../skin/WindowElement.h"
#include "../skin/BorderElement.h"
#include "../../ObserverRegister.h"
#include "NotifyPopUpCaption.h"
#include "ScrollView.h"

#include <interfaces/IUOLMessengerNotifyPopUpHandler.h>
#include <interfaces/IUOLMessengerNotifyPopUpHyperLinkHandler.h>




class CNotifyPopUp;


class CNotifyPopUpObserver
{
public:
	virtual void OnClose(CNotifyPopUp* pNotifyPopUp) {};
	virtual void OnOptions(CNotifyPopUp* pNotifyPopUp) {};
	virtual void OnClick(CNotifyPopUp* pNotifyPopUp) {};
};



class CNotifyPopUpObserverNotifier : public CObserverRegister<CNotifyPopUpObserver>
{
public:
	CNotifyPopUpObserverNotifier(CNotifyPopUp* pNotifyPopUp);

	void NotifyAll(void (CNotifyPopUpObserver::*pEventHandler)(CNotifyPopUp* pNotifyPopUp));

private:
	CNotifyPopUp*	m_pNotifyPopUp;
};



typedef CWinTraits<WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_TOPMOST>		CNotifyPopUpTraits;
typedef CWindowElement<CNotifyPopUp, CDummyWindow< CWindowImpl<CNotifyPopUp, CWindow, CNotifyPopUpTraits> > > CNotifyPopUpBase;

class CNotifyPopUp : public CNotifyPopUpBase
{
public:
	enum { 
		WM_CHILD_POPUP_CLICKED = WM_USER + 0x1,
		WM_POPUP_MOUSE_ENTER,
		WM_POPUP_MOUSE_LEAVE
	};

public:
	DECLARE_WND_CLASS("UOLMessengerNotifyPopUpClass")

	CNotifyPopUp();
	virtual ~CNotifyPopUp();

	void SetNotifyPopUpHandler(IUOLMessengerNotifyPopUpHandlerPtr pNotifyPopUpHandler);

	virtual void Create(HWND hParent);

	DWORD GetShowTime();
	void SetShowTime(DWORD dwShowTime);

	DWORD GetTimeout();
	void SetTimeout(DWORD dwTimeout);

	BOOL IsExpired();
	void SetExpired(BOOL bExpired);

	BOOL IsProtected();
	void SetProtected(BOOL bProtected);

	BOOL IsFirstTime();
	void SetFirstTime(BOOL bFirstTime);

	virtual void AddNotifyPopUpObserver(CNotifyPopUpObserver* pObserver);
	virtual void RemoveNotifyPopUpObserver(CNotifyPopUpObserver* pObserver);

protected:
	BEGIN_MSG_MAP(CNotifyPopUp)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMouseDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnMouseDown)
		MESSAGE_HANDLER(CNotifyPopUpHyperLink::WM_USER_HYPERLINKLBUTTONDOWN, OnHyperlinkButtonDown)
		MESSAGE_HANDLER(CNotifyPopUpHyperLink::WM_USER_HYPERLINKRBUTTONDOWN, OnHyperlinkButtonDown)
		MESSAGE_HANDLER(CNotifyPopUpCaption::WM_USER_NOTIFYPOPUPCLOSEBUTTONDOWN, OnClose)
		MESSAGE_HANDLER(CNotifyPopUpCaption::WM_USER_NOTIFYPOPUPOPTIONSBUTTONDOWN, OnOptions)
		CHAIN_MSG_MAP(CNotifyPopUpBase)
	END_MSG_MAP()

	void UpdateLayout(BOOL bResizeBars = TRUE);

	CString GetName() const;
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	void DoPaint(CDCHandle& dcHandle);

private:
	//Window messages
	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	void OnDestroy();

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOptions(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHyperlinkButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void AddLink(const CString& strTitle, const CString& strUrl);

protected:
	CBorderElementPtr		m_pBorder;
	CNotifyPopUpCaptionPtr	m_pNotifyPopUpCaption;

	IUOLMessengerNotifyPopUpHandlerPtr m_pNotifyPopUpHandler;

private:
	CNotifyPopUpObserverNotifier	m_observerNotifier;

	UINT_PTR				m_nIDTimer;
	INT						m_nTimerInterval;

	CBrush					m_brush;

	CSize					m_size;
	CPoint					m_pt;

	CScrollView				m_scrollView;

	DWORD					m_dwShowTime;
	DWORD					m_dwTimeout;

	BOOL					m_bExpired;
	BOOL					m_bProtected;
	BOOL					m_bFirstTime;
};

MAKEAUTOPTR(CNotifyPopUp);
