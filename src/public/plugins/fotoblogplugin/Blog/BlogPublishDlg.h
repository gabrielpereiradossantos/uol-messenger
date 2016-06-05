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

#include "resource.h"
//#include "../custom_strings.h"
#include "IEEventsObserver.h"
//#include "../WindowEx.h"
//#include "../utils.h"



class IBlogIEEventsHandler
{
public:
	virtual void OnBeforeNavigate2(CString strURL, BOOL& bCancel) = 0;
	virtual void OnDocumentComplete(CString strURL) = 0;
	virtual void OnNavigateError(CString strURL, DWORD dwStatusCode, BOOL& bCancel) = 0;
	virtual void OnWindowClosing(BOOL& bCancel) = 0;
};


class CBlogIEEventsObserver : public CIEEventsObserver
{
public:
	CBlogIEEventsObserver(IBlogIEEventsHandler* pHandler, IWebBrowser2* pWebBrowser = NULL);
	~CBlogIEEventsObserver();

	IWebBrowser2* GetWebBrowser2();
	void SetWebBrowser2(IWebBrowser2* pWebBrowser);

	// Event handlers
	STDMETHOD_(void, BeforeNavigate2)(IDispatch* /*pDisp*/, VARIANT* /*url*/, VARIANT* /*Flags*/, VARIANT* /*TargetFrameName*/, VARIANT* /*PostData*/, VARIANT* /*Headers*/,  VARIANT_BOOL* /*Cancel*/);
	STDMETHOD_(void, DocumentComplete)(IDispatch* /*pDisp*/, VARIANT* /*URL*/);
	STDMETHOD_(void, NavigateError)(IDispatch* /*pDisp*/, VARIANT* /*URL*/, VARIANT* /*TargetFrameName*/, VARIANT* /*StatusCode*/, VARIANT_BOOL* /*Cancel*/);
	STDMETHOD_(void, WindowClosing)(VARIANT_BOOL /*IsChildWindow*/, VARIANT_BOOL* /*Cancel*/);

private:
	IWebBrowser2*			m_pWebBrowser;
	IBlogIEEventsHandler*	m_pHandler;
};


class CAbstractBlogController;


class CBlogPublishDlg : public CAxDialogImpl<CBlogPublishDlg>,						
						public IBlogIEEventsHandler,
						public CMessageFilter
{
public:
	enum { IDD = IDD_BLOG_PUBLISH };
	enum { ID_BLOG_TIMER };

	static enum 
	{ 
		WM_USER_DISPLAY_ERROR  = WM_USER + 0x0710, WM_USER_REAUTHENTICATE
	};

	CBlogPublishDlg();
	virtual ~CBlogPublishDlg();

	void SetBlogController(CAbstractBlogController* pBlogController);

	// IBlogIEEventsHandler interface
	void OnBeforeNavigate2(CString strURL, BOOL& bCancel);
	void OnDocumentComplete(CString strURL);
	void OnNavigateError(CString strURL, DWORD dwStatusCode, BOOL& bCancel);
	void OnWindowClosing(BOOL& bCancel);

protected:


	BEGIN_MSG_MAP(CBlogPublishDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(CBlogPublishDlg::WM_USER_DISPLAY_ERROR, OnUserDisplayError)
		MESSAGE_HANDLER(CBlogPublishDlg::WM_USER_REAUTHENTICATE, OnUserReauthenticate)
	END_MSG_MAP()

	// CMessageFilter
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	HWND Create(HWND hWndParent);

	// Message handlers
	LRESULT OnInitDialog(HWND hwnd, LPARAM lParam);
	LRESULT OnClose();
	LRESULT OnDestroy();
	LRESULT	OnTimer(UINT, TIMERPROC);
	LRESULT OnUserDisplayError(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUserReauthenticate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void	OnSetFocus(HWND hwnd);

private:
	void SetOnCloseHandler();

protected:
	CAbstractBlogController*	m_pBlogController;

	CString						m_strBlogURL;
	IWebBrowser2*				m_pWebBrowser;
	CBlogIEEventsObserver*		m_pBlogIEEventsObserver;

	CAxWindow	m_wndIE;

	CString		m_strRedirURL;
	CString		m_strErrorURL;
	BOOL		m_bLoadingErrorPage;
	BOOL		m_bReauthenticating;

	CString		m_strDialogHelper;
	HWND		m_hWndParent;
	LONG		m_nParentX;
	LONG		m_nParentY;

	CAtlList<CString>	m_UrlNotFoundList;
};
