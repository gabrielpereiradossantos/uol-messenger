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

#include <IEEventsObserver.h>

#include "../skin/WindowElement.h"
#include "../DummyWindow.h"
#include "../MainFrm.h"
#include <atlcoll.h>
#include "../IECTRLMenuHandler.h"

#include "interfaces/IUOLMessengerConversation.h"

typedef CAtlMap <CString, CString>	CEmoticonImageTag;

/************************************************************************/
/* CEmoticonAdd                                                         */
/************************************************************************/

class CEmoticonAdd {
public:
	virtual void AddEmoticon(CString textTag)=0;
};

/************************************************************************/
/* CEmoticonsWindow                                                     */
/************************************************************************/

class CEmoticonsWindow;

class CEmoticonsHTMLCtrl : public CWindowImpl<CEmoticonsHTMLCtrl, CAxWindow>,
						   public CIEEventsObserver/*,
						   public CIECTRLMenuHandler<CEmoticonsHTMLCtrl>*/
{
public:
	DECLARE_WND_CLASS_EX("HTMLDetailsWindow", CS_DBLCLKS, COLOR_WINDOW);

	CEmoticonsHTMLCtrl(CEmoticonsWindow* pEmoticonWindow);
	virtual ~CEmoticonsHTMLCtrl();
	void SetText(const CString& strText);
	CString GetTag() const;

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
		DWORD dwStyle = 0, DWORD dwExStyle = 0,
		_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);

protected:
	BEGIN_MSG_MAP(CEmoticonsHTMLCtrl)	
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_ERASEBKGND(OnEraseBackground)
		MSG_WM_SIZE(OnSize)
		//MESSAGE_HANDLER_EX(WM_MOUSEACTIVATE, OnMouseActivate)
		//CHAIN_MSG_MAP(CIECTRLMenuHandler<CEmoticonsHTMLCtrl>)
	END_MSG_MAP()
	void OnDestroy();
	LRESULT OnEraseBackground(HDC hDc);
	void OnSize(UINT, CSize);
	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam);

	STDMETHOD_(void, BeforeNavigate2)(IDispatch* /*pDisp*/, VARIANT* url, VARIANT* /*Flags*/, VARIANT* /*TargetFrameName*/, VARIANT* /*PostData*/, VARIANT* /*Headers*/,  VARIANT_BOOL* Cancel);
	STDMETHOD_(void, DocumentComplete)(IDispatch* /*pDisp*/, VARIANT* URL);

private:
	CEmoticonsWindow* pEmoticonWindow;
	CComPtr<IWebBrowser2>		m_pWebBrowser;
	CComPtr<IHTMLDocument2>		m_pDocument;
	CString m_strHtml;
	CString m_strTag;
};


/************************************************************************/
/* CEmoticonsWindow                                                     */
/************************************************************************/

class CEmoticonsWindow;
typedef CWindowElement<CEmoticonsWindow, CDummyWindow<CWindowImpl<CEmoticonsWindow> > > _CEmoticonsWindowBase;

class CEmoticonsWindow : public _CEmoticonsWindowBase ,
					public CMessageFilter
{
public:
	DECLARE_WND_CLASS_EX("EmoticonsWindow", CS_DBLCLKS, COLOR_WINDOW);
	//CEmoticonsWindow(CMainFrame* pMainFrame);
	CEmoticonsWindow(CMainFrame* pMainFrame, CString strEmoticonFile);
	BEGIN_MSG_MAP(CEmoticonsWindow)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_SIZE(OnSize)
		MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(_CEmoticonsWindowBase)
	END_MSG_MAP()

	void OnSize(UINT, const CSize& size);
	void OnDestroy();
	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void RegisterMessageEditPanel(CEmoticonAdd * emoticon);

	void SetConversation(IUOLMessengerConversationPtr pConversation);

protected:
	CEmoticonAdd * m_pEmoticonWindow;
	
private:
	CMainFrame*			m_pMainFrame;
	CEmoticonImageTag   m_imageTag;
	CEmoticonsHTMLCtrl	m_detailsCtrl;
	void LoadEmoticons();
	void InsertEmoticon(const CString &filepath, const CString &emoticontag);
	CString m_strEmoticonFile;
	IUOLMessengerConversationPtr m_pConversation;
};



