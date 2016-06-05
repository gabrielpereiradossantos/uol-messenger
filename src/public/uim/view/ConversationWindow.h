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

#include "MessageEditPanel.h"
#include <interfaces/IUOLMessengerContact.h>
#include <interfaces/IUOLMessengerConversation.h>
#include <interfaces/IUOLMessengerConversationWindow.h>
#include <interfaces/IUOLMessengerAccountConnectionManager.h>
#include <interfaces/IUOLMessengerCustomEmoticonManager.h>
#include <interfaces/IUOLMessengerSettingsEventListener.h>
#include <interfaces/IUOLMessengerMessageTemplateRecord.h>
#include <interfaces/IUOLMessengerFileTransferManager.h>
#include <IEEventsObserver.h>
#include "../controller/CustomEmoticon/CustomEmoticonManager.h"
#include "./WebBrowserDispatch/OleClientDocHostWrapperFactory.h"

#include "MessageEdit/RichUOLEdit.h"
#include "MessageEditPanelBuilder.h"
#include "SettingsContactListOrderTraits.h"

#include "ChatList/ChatListCtrl.h"
#include "ChatList/ReadyForChatListCtrl.h"
#include "ConversationWindowSplitter.h"

#include "ChatList/UpperChatContainer.h"
#include "ChatList/LowerChatContainer.h"

#include "IECTRLMenuHandler.h"

#include "../controller/MessageTemplates/TemplateDocumentBuilder.h"

#include "../resource.h"


///////////////////////////////////////////////////////////////////////////////
// CUIMHistoryCtrl class
class CUIMHistoryCtrl : public CWindowImpl<CUIMHistoryCtrl, CAxWindow>,
						public CIEEventsObserver,
						public CUOLMessengerCustomEmoticonManagerObserver, 
						public ICustomContextMenuHandler
{
public:
	DECLARE_WND_CLASS_EX("MessageHistoryWindow", CS_DBLCLKS, COLOR_WINDOW);
	
	enum
	{
		WM_UPDATE_CUSTOM_EMOTICON = WM_USER + 0x443,
		WM_USER_GET_CURRENT_TEXT,
		WM_USER_REFRESH_CURRENT_TEXT
	};
	
	enum
	{
		COMMAND_ID_ADD_CUSTOM_EMOTICON = 0x314
	};
	
	CUIMHistoryCtrl();
	virtual ~CUIMHistoryCtrl();
	
	struct CustomEmoticonElement
	{
		CString strEmoticonShortcut;
		CString strEmoticonPath;
		int nEmoticonHeight;
		int nEmoticonWidth;
	}; 
	
	void AddMessageTemplateRecord(IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord);
	int GetMessageTemplateRecordCount() const;
	
	void SetConversation(IUOLMessengerConversation2Ptr pConversation);
	IUOLMessengerConversation2Ptr GetConversation() const;
	
	void SetConversationWindowHWND(HWND hwndConversationWindow);
	
	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);
	
    void ScrollView();
	
	HRESULT ExecFormsCommand(DWORD dwCommandID, VARIANT* pVarIn, VARIANT* pVarOut);
	
	BOOL IsHistoryCtrlChildWindow(HWND hWnd);

	void ForwardMessageToWebBrowser(MSG* pMsg);
	
protected:
	typedef CWindowImpl<CUIMHistoryCtrl, CAxWindow> CChainClass;

	BEGIN_MSG_MAP(CUIMHistoryCtrl)	
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_ERASEBKGND(OnEraseBackground)
		MSG_WM_SIZE(OnSize)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER_EX(WM_UPDATE_CUSTOM_EMOTICON, OnUpdateCustomEmoticon)
		COMMAND_ID_HANDLER_EX(COMMAND_ID_ADD_CUSTOM_EMOTICON, OnAddCustomEmoticon)
		MESSAGE_HANDLER_EX(CHistoryCtrlExternalDispatch::WM_DISPATCH_ACCEPT_FILE, OnAcceptFile)
		MESSAGE_HANDLER_EX(CHistoryCtrlExternalDispatch::WM_DISPATCH_REJECT_FILE, OnRejectFile)
		//CHAIN_MSG_MAP(CChainClass)
		//CHAIN_MSG_MAP(CIECTRLMenuHandler<CUIMHistoryCtrl>)
	END_MSG_MAP()
	
	void OnDestroy();
	LRESULT OnEraseBackground(HDC hDc);
	void OnSize(UINT, CSize);
	void OnTimer(UINT nTimerId, TIMERPROC timerproc);

	LRESULT OnUpdateCustomEmoticon(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void OnAddCustomEmoticon(UINT /*uCode*/, int nID, HWND hwndCtrl);

	LRESULT OnAcceptFile(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRejectFile(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	STDMETHOD_(void, BeforeNavigate2)(IDispatch* /*pDisp*/, VARIANT* url, VARIANT* /*Flags*/, VARIANT* /*TargetFrameName*/, VARIANT* /*PostData*/, VARIANT* /*Headers*/,  VARIANT_BOOL* Cancel);
	STDMETHOD_(void, DocumentComplete)(IDispatch* /*pDisp*/, VARIANT* URL);
	
	// CUOLMessengerCustomEmoticonManagerObserver interface.
	//
	virtual void OnCustomEmoticonStored(IUOLMessengerCustomEmoticonPtr pCustomEmoticon);
	
	// ICustomContextMenuHandler interface.
	//
	BOOL CanShowCustomContextMenu();
	int  GetCustomContextMenuType();
	BOOL CanShowTextSelectionOnlyMenu();
	BOOL CanShowImageSelectionOnlyMenu();
	BOOL CanForwardContextMenuHandling();
	
	BOOL IsHistoryIEServerWindow(HWND hWnd);
	BOOL IsClickOverCustomEmoticonImageElement();
	
	void ShowCustomPopupMenu();
	
	IUOLMessengerMessageTemplateRecordPtr GetFileTransferMessageTemplateRecord(long iFileTransferId);
	
	HRESULT UpdateDocument(BOOL bForceRebuild);
	
protected:
	static const UINT TIMER_ASYNC_MESSAGE_QUEUE = 0x510;

private:
	typedef CAtlMap<CString, CString> CAttributeMap;
	
	HRESULT AddMessage(IHTMLDocument2* pDocument, IUOLMessengerMessageTemplateRecordPtr pNewMessageTemplateRecord);
	
	IUOLMessengerCustomEmoticonManagerPtr GetCustomEmoticonManager();
	
private:
	CComPtr<IWebBrowser2>		m_pWebBrowser;
	CComPtr<IHTMLDocument2>		m_pDocument;

	CAtlList<IUOLMessengerMessageTemplateRecordPtr>   m_listMessageTemplateRecords;

	IUOLMessengerConversation2Ptr	m_pConversation;

	CComPtr<IHTMLDOMNode>		m_pLastMessageTextNode;
	IUOLMessengerMessageTemplateRecordPtr	m_pLastMessageTemplateRecord;

	CComAutoCriticalSection		m_csAsyncAddEntries;
	CAtlList<IUOLMessengerMessageTemplateRecordPtr>		m_listAsyncAddRecords;
	
	HMENU						m_hAddCustomEmoticonMenu;
	CString						m_strAddingEmoticonShortcut;
	CString						m_strAddingEmoticonPath;
	
	HWND						m_hwndConversationWindow;
	
	COleClientDocHostWrapper*	m_pHistoryWrapper;
	
	CTemplateDocumentBuilderPtr		m_pDocumentBuilder;
	CString							m_strHtmlDocument;
	
	CComAutoCriticalSection		m_csClosingWindow;
};





///////////////////////////////////////////////////////////////////////////////
// CConversationWindow class

// Debug code flag
//#define _CONVERSATION_WINDOW_DEBUG


class CConversationWindow : public CHorSplitterWindow,
                            public CUOLMessengerConversationObserver2,
							public CRichTextEventsEventListener, 
							public CMessageFilter,
							public CUOLMessengerAccountConnectionManagerObserver,
							public IUOLMessengerConversationWindow2,
                            public IUOLMessengerSettingsEventListener
{
public:
	DECLARE_WND_CLASS_EX("ConversationWindow", CS_DBLCLKS, COLOR_WINDOW);
	
	enum
	{
		WM_USER_SHOW_INPUT_FOCUS = WM_USER + 0x455, 
	};
	
	CConversationWindow(IUOLMessengerConversation2Ptr pConversation);

	// IUOLMessengerConversationWindow2 methods
	virtual IUOLMessengerConversationPtr GetConversation() const;
	virtual IUOLMessengerMessageEditPanelPtr GetMessageEditPanel() const;
	virtual CString GetConversationTitle();
	virtual void SetIsLogMessageEnabled(BOOL bEnabled);
	virtual BOOL IsLogMessageEnabled();

	void OnTextAreaChar();
	void SetMessageEditPanelBuilder(CMessageEditPanelBuilderPtr);

	void ShowInputFocus();

	void OnConnected(IUOLMessengerAccountPtr pAccount);
	void OnDisconnected(IUOLMessengerAccountPtr pAccount);
	void OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage);

	// CUOLMessengerConversationWindow overriden functions
	void RegisterObserver(CUOLMessengerConversationWindowObserver2* pObserver);
	void UnregisterObserver(CUOLMessengerConversationWindowObserver2* pObserver);

	BOOL isConnected() {return m_bConnected;} 
	
	void SelectAllText();
	
	HWND GetHistoryCtrlHandle();
	HWND GetRichEditCtrlHandle();
	HWND GetSendButtonCtrlHandle();
	
	void UpdateMessageEditToolbarButtonVisible(const CString& strButtonCommand, BOOL bVisible);
    
    void UpdateLayout();
	
	void HandleEditCopy();
	
protected:
	enum
	{
		WM_USER_ADD_HISTORY_MESSAGE = WM_USER + 0x320,
		SEND_TYPING_TIMER = 0xd300
	};
	
	typedef struct _HISTORYENTRYPARAM
	{
		IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord;
	} MESSAGETEMPLATERECORDPARAM, *LPMESSAGETEMPLATERECORDPARAM;

	typedef struct _USERDROPPEDFILEPARAM
	{
		CString strFileName;
	} USERDROPPEDFILEPARAM, *LPUSERDROPPEDFILEPARAM;


	BEGIN_MSG_MAP(CMessageWindow)	
		MSG_WM_SIZE(OnSize)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_ERASEBKGND(OnEraseBackground)
		MSG_WM_TIMER(OnTimer)
		COMMAND_CODE_HANDLER_EX(CMessageEditPanel::MEP_EDIT_RETURN, OnEditReturn)
		MESSAGE_HANDLER_EX(WM_USER_ADD_HISTORY_MESSAGE, OnAddHistoryMessage)
		MESSAGE_HANDLER_EX(CMessageEditPanel::WM_NUDGE_BUTTON_CLICK, OnNudgeButtonClick)
		MESSAGE_HANDLER_EX(CMessageEditPanel::WM_USER_DROPPED_FILE, OnUserDroppedFile)
		MESSAGE_HANDLER_EX(WM_USER_SHOW_INPUT_FOCUS, OnShowInputFocus)
		MESSAGE_HANDLER_EX(CUIMHistoryCtrl::WM_USER_GET_CURRENT_TEXT, OnGetCurrentText)
		MESSAGE_HANDLER_EX(CUIMHistoryCtrl::WM_USER_REFRESH_CURRENT_TEXT, OnRefreshCurrentText)
		CHAIN_MSG_MAP(CHorSplitterWindow)
	END_MSG_MAP()

	void OnSize(UINT, const CSize& size);
	LRESULT OnCreate(LPCREATESTRUCT lpCreatestruct);
	void OnDestroy();
	LRESULT OnEraseBackground(HDC hDc);
	LRESULT OnShowWindow(HDC hDc);
	void OnTimer(UINT nTimerId, TIMERPROC timerproc);
	void OnEditReturn(UINT nId, int nCode, HWND hWnd);
	LRESULT OnAddHistoryMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnNudgeButtonClick(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnUserDroppedFile(UINT uMsg, WPARAM wParam, LPARAM lParam);	
	LRESULT OnShowInputFocus(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnGetCurrentText(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRefreshCurrentText(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// CUOLMessengerConversationObserver2 interface
	virtual void OnAddMessage(IUOLMessengerConversation* pConversation, IUOLMessengerConversation::MessageEntryPtr pMessageEntry);
    virtual void OnAddChatParticipant(IUOLMessengerConversation2* pConversation, IUOLMessengerContact3Ptr pChatContact);
	virtual void OnRemoveChatParticipant(IUOLMessengerConversation2* pConversation, IUOLMessengerContact3Ptr pChatContact);
	virtual void OnAddRequestFileTransferAsync(IUOLMessengerConversation2* pConversation, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

    // IUOLMessengerSettingsEventSource interface
    void OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId);

private:


	//LRESULT OnAccellaratorHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//{
	//	ATLTRACE("%s",__FUNCTION__);
	//	return S_OK;
	//}

	//LRESULT OnAccellaratorInfo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//{
	//	ATLTRACE("%s",__FUNCTION__);
	//	return S_OK;
	//}

	//LRESULT OnAccellEscape(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//{
	//	ATLTRACE("%s",__FUNCTION__);
	//	GetParent().PostMessage(WM_CLOSE);
	//	return S_OK;
	//}

	//LRESULT OnAccellTab(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//{
	//	ATLTRACE("%s",__FUNCTION__);

	//	return S_OK;
	//}

	void NotifyAll(void (CUOLMessengerConversationWindowObserver2::*pfnUpdate)(IUOLMessengerConversationWindow*));
	void NotifyAllChat(void (CUOLMessengerConversationWindowObserver2::*pfnUpdate)(IUOLMessengerConversationWindow2*));

    void SetWindowTitle();
    void CreateVerticalSplitter();
    void CreateHorizontalSplitter();
    void PopulateReadyForChatList();
    void ChatLeave();
    void HandleDisconnectionForChat();
	void PlayFileTransferAlertSound();

private:
#ifdef _CONVERSATION_WINDOW_DEBUG
	static LONG ms_lConversationId;

	LONG	m_lId;
#endif

	BOOL m_bConnected;
	IUOLMessengerConversation2Ptr m_pConversation;
	CUIMHistoryCtrl		m_messageHistoryWindow;
	HACCEL    m_haccelerator;

	CMessageEditPanelPtr		m_pEditPanel;
	CMessageEditPanelBuilderPtr m_pMessagaEditPanelBuilder;

	UINT		m_nCharCount;

	CTime		m_timeStart;

	CAtlList<CUOLMessengerConversationWindowObserver2*> m_listObservers;

    // Chat window controls
    CConversationWindowVerticalSplitterPtr      m_pVerticalSplitter;    
    CConversationWindowHorizontalSplitterPtr    m_pHorizontalSplitter;
    CUpperChatContainerPtr                      m_pUpperContainer;
    CLowerChatContainerPtr                      m_pLowerContainer;
    CSettingsContactListOrderTraitsPtr	        m_pOrderTraits;

	BOOL	m_bIsLogMessageEnabled;

	CString		m_strCurrentText;
};

MAKEAUTOPTR(CConversationWindow);

BOOL CALLBACK EnumHistoryCtrlChildWinProc(HWND hWnd, LPARAM lParam);