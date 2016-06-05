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
#include <mshtmdid.h>
#include "MessageEdit/RichUOLEdit.h"
#include "UOLToolbar.h"
#include "MessageEdit/EmoticonsFrame.h"
#include <interfaces/IUOLMessengerMessageEditPanel.h>
#include "../controller/Settings/SettingsManagerObserverNotifier.h"
#include "ButtonWMEventNotifier.h"

class CHTMLElementEventsObserver: public IDispEventImpl<0, CHTMLElementEventsObserver, &__uuidof(HTMLElementEvents)> 
{
public:
	CHTMLElementEventsObserver()
	{
	}

	virtual ~CHTMLElementEventsObserver()
	{
	}

	BEGIN_SINK_MAP(CHTMLElementEventsObserver)
		SINK_ENTRY_EX(0, (__uuidof(HTMLElementEvents)), DISPID_HTMLELEMENTEVENTS_ONPROPERTYCHANGE, OnPropertyChange)
		SINK_ENTRY_EX(0, (__uuidof(HTMLElementEvents)), DISPID_HTMLELEMENTEVENTS_ONKEYDOWN, OnKeyDown)
		SINK_ENTRY_EX(0, (__uuidof(HTMLElementEvents)), DISPID_HTMLELEMENTEVENTS_ONDROP, OnDrop)
		SINK_ENTRY_EX(0, (__uuidof(HTMLElementEvents)), DISPID_HTMLELEMENTEVENTS_ONCUT, OnCut)
		SINK_ENTRY_EX(0, (__uuidof(HTMLElementEvents)), DISPID_HTMLELEMENTEVENTS_ONPASTE, OnPaste)
	END_SINK_MAP()

	HRESULT ReceiveHTMLElementEvents(BOOL bReceive, IHTMLElement* pElement)
	{
		HRESULT hr = ((bReceive) ? 
				IDispEventImpl<0, CHTMLElementEventsObserver, &__uuidof(HTMLElementEvents)>::Advise(pElement) :
				IDispEventImpl<0, CHTMLElementEventsObserver, &__uuidof(HTMLElementEvents)>::Unadvise(pElement));

		ATLASSERT(SUCCEEDED(hr));

		return hr;
	}

public:
	STDMETHOD_(void, OnPropertyChange)() {}
	STDMETHOD_(void, OnKeyDown)() {}
	STDMETHOD_(VARIANT_BOOL, OnCut)() { return VARIANT_TRUE; }
	STDMETHOD_(VARIANT_BOOL, OnPaste)() { return VARIANT_TRUE; }
	STDMETHOD_(VARIANT_BOOL, OnDrop)() { return VARIANT_TRUE; }
};




//
// CMessageEditPanel - CMessageEdit + CButton
//


class CMessageEditPanel;
typedef CWindowElement<CMessageEditPanel, CDummyWindow<CWindowImpl<CMessageEditPanel> > > CMessageEditPanelBase;

class CMessageEditPanel : public CMessageEditPanelBase,
						  public CUOLMessengerImageButtonEventListener2,
						  public CEmoticonAdd,
						  public IUOLMessengerMessageEditPanel, 
						  public CUOLMessengerSettingsManagerObserver

{
public:
	DECLARE_WND_CLASS("CMessageEditPanel");

	enum
	{
		WM_NUDGE_BUTTON_CLICK = WM_USER + 0x324
	};
	
	CMessageEditPanel();
	virtual ~CMessageEditPanel();

	CString GetEditText();
	int GetEditTextLength();
	void ClearEditText();
	void SetRichListener(CRichTextEventsEventListener * listener);
	void ShowInputFocus();
	CString GetName() const;
	
	CString GetEditPlainText();
	void SetEditPlainText(CString strPlainText);

	// CWindowElement overridden functions
	virtual void UpdateLayout(BOOL bResizeBars = TRUE);
	// CImageButtonEventListener overridden functions
	void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick);
	void OnConnected();
	void OnDisconnected();
	void OnReportDisconnect();

	virtual void AddEmoticon(CString textTag);

	virtual IUOLMessengerImageButtonContainerPtr GetToolbar() const;
	
	// CUOLMessengerSettingsManagerObserver overridden functions
	virtual void OnEmoticonSettingsChanged();

	virtual void SelectAllText();
	
	HWND GetRichEditCtrlHandle();
	HWND GetSendButtonCtrlHandle();
	
	void UpdateMessageEditToolbarButtonVisible(const CString& strButtonCommand, BOOL bVisible);

	void SetConversation(IUOLMessengerConversationPtr pConversation);


protected:
	BEGIN_MSG_MAP(CMessageEditPanel)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_COMMAND(OnCommand)
		MESSAGE_HANDLER(WM_ON_CHAR_EVENT_NOTIFIER, OnCharEventNotifier)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CMessageEditPanelBase)
		//MESSAGE_RANGE_HANDLER(WM_CTLCOLOREDIT, WM_CTLCOLORSTATIC, OnCtlColor)
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpCs);
	void OnDestroy();
	void OnCommand(UINT nId, int nCode, HWND hWnd);
	LRESULT OnCharEventNotifier(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	enum UserMessages
	{
		MEP_EDIT_RETURN = WM_USER + 0x150,	// sent by WM_COMMAND
		WM_USER_DROPPED_FILE = WM_USER + 0x160
	};

private:
	void CreateToolbar();
	void UpdateToolbar();
	void ShowEmoticonsWindow(const CPoint& ptClick);

private:
	enum WindowMetrics
	{
		CONTROL_PADDING = 5,
	};
	
	
	CUOLToolBarPtr	m_pToolbar;
	CRichUOLView    m_editRichMessage;
	// TODO: Pode existir local melhor para o codigo dessa variavel
	CRichUOLView::IExRichEditOleCallback m_richCallBack;
	CButtonWMEventNotifier	m_btnSend;
	CString			m_strSendButtonText;
	CEmoticonsFramePtr m_pEmoticonsFrame;
	IUOLMessengerConversationPtr m_pConversation;

};


MAKEAUTOPTR(CMessageEditPanel);
