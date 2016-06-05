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
 
#include "StdAfx.h"
#include ".\conversationWindow.h"

#include <strsafe.h>
#include <interfaces/IUOLMessengerAccount.h>
#include "../resource.h"
#include "../urlbuilder.h"
#include "../HTMLFilter.h"
#include "AddCustomEmoticonDialog.h"
#include "../controller/ParseEmoticon.h"
#include "../controller/UIMApplication.h"
#include "../controller/UIManager.h"
#include "../controller/HistoryRecordMessage.h"
#include "../controller/HistoryRecordChat.h"
#include "../controller/EmoticonParserManager.h"
#include "../controller/OpenBrowserHelper.h"
#include "ChatList/ReadyForChatListCtrlBuilder.h"
#include "ConversationWindowSplitterBuilder.h"

#include <commands/openbrowsercommand.h>
#include <commands/NudgeContactCommand.h>
#include <commands/ShowAddCustomEmoticonDialogCommand.h>
#include <commands/LeaveChatCommand.h>
#include <commands/SendDroppedFileCommand.h>
#include "../model/MessageTemplateRecord.h"

#include <mshtmcid.h>


///////////////////////////////////////////////////////////////////////////////
// CUIMHistoryCtrl class
CUIMHistoryCtrl::CUIMHistoryCtrl() :
	m_pWebBrowser(NULL),
	m_hAddCustomEmoticonMenu(NULL),
	m_hwndConversationWindow(NULL),
	m_pHistoryWrapper(NULL)
{
}


CUIMHistoryCtrl::~CUIMHistoryCtrl()
{
}


void CUIMHistoryCtrl::AddMessageTemplateRecord(IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord)
{
	m_listMessageTemplateRecords.AddTail(pMessageTemplateRecord);

	// If messages HTML template is loaded, add message,
	// else message will be added asynchronously...

	BOOL bEmptyMessageQueue;

	{ 	CComCritSecLock<CComAutoCriticalSection> lock(m_csAsyncAddEntries);

		bEmptyMessageQueue = m_listAsyncAddRecords.IsEmpty();
	}

	if ((m_pDocument != NULL) && (bEmptyMessageQueue))
	{
		AddMessage(m_pDocument, pMessageTemplateRecord);
	}
	else
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csAsyncAddEntries);

		m_listAsyncAddRecords.AddTail(pMessageTemplateRecord);
	}
}


int CUIMHistoryCtrl::GetMessageTemplateRecordCount() const
{
	return (int) m_listMessageTemplateRecords.GetCount();
}


void CUIMHistoryCtrl::SetConversation(IUOLMessengerConversation2Ptr pConversation)
{
	m_pConversation = pConversation;
	
	m_pDocumentBuilder = new CTemplateDocumentBuilder(m_pConversation);
}


IUOLMessengerConversation2Ptr CUIMHistoryCtrl::GetConversation() const
{
	return m_pConversation;
}


void CUIMHistoryCtrl::SetConversationWindowHWND(HWND hwndConversationWindow)
{
	m_hwndConversationWindow = hwndConversationWindow;
}


HWND CUIMHistoryCtrl::Create(HWND hWndParent, _U_RECT rect, LPCTSTR szWindowName,
			DWORD dwStyle, DWORD dwExStyle,
			_U_MENUorID MenuOrID, LPVOID lpCreateParam)
{
	HWND hWnd = __super::Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);

	m_hAddCustomEmoticonMenu = ::CreatePopupMenu();
	
	if (m_hAddCustomEmoticonMenu)
	{
		UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		ATLASSERT(pStringLoader);
		
		CString strAddEmoticonMenu;
		
		if (pStringLoader)
		{
			strAddEmoticonMenu = pStringLoader->GetValue(_T("uol.resource.IDS_ADD_CUSTOM_EMOTICON_MENU"));
		}
		
		if (strAddEmoticonMenu.IsEmpty())
		{
			strAddEmoticonMenu = "Adicionar";
		}
		
		::AppendMenu(m_hAddCustomEmoticonMenu, MF_ENABLED | MF_STRING, 
				COMMAND_ID_ADD_CUSTOM_EMOTICON, strAddEmoticonMenu);
	}
	
	if (hWnd != NULL)
	{
		ModifyStyle(0, /*WS_HSCROLL |*/ WS_VSCROLL);
		
		if (FAILED(CreateControl(L"about:blank")))
		{
			ATLASSERT(FALSE);
			DestroyWindow();

			return NULL;
		}	

		if (FAILED(QueryControl(&m_pWebBrowser)))
		{
			ATLASSERT(FALSE);
			DestroyWindow();

			return NULL;
		}
		
		ReceiveEvents(TRUE, m_pWebBrowser);
		
		SetTimer(TIMER_ASYNC_MESSAGE_QUEUE, 500);
		
		m_pHistoryWrapper = COleClientDocHostWrapperFactory::GetInstance()->
				GetWebBrowserWrapper(COleClientDocHostWrapperFactory::HISTORY_WEBBROWSER_WRAPPER, 
				m_pWebBrowser, CURLBuilder::ResourceToURL(IDR_HTML_MESSAGES), m_hWnd, this);
		
		if (m_pHistoryWrapper)
		{
			m_pHistoryWrapper->SetCustomMenuMode(m_hAddCustomEmoticonMenu, m_hWnd);
		}
	}
	
	// Register as observer of CustomEmoticonManager.
	//
	IUOLMessengerCustomEmoticonManagerPtr pCustomEmoticonManager = GetCustomEmoticonManager();
	
	if (pCustomEmoticonManager)
	{
		pCustomEmoticonManager->RegisterObserver(this);
	}
	
	return hWnd;
}


void CUIMHistoryCtrl::OnDestroy()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csClosingWindow);
	
	KillTimer(TIMER_ASYNC_MESSAGE_QUEUE);
	
	// Unregister as observer of CustomEmoticonManager.
	//
	IUOLMessengerCustomEmoticonManagerPtr pCustomEmoticonManager = GetCustomEmoticonManager();
	
	if (pCustomEmoticonManager)
	{
		pCustomEmoticonManager->UnregisterObserver(this);
	}
	
	if (m_hAddCustomEmoticonMenu)
	{
		::DestroyMenu(m_hAddCustomEmoticonMenu);
	}
	
	if (m_pLastMessageTextNode)
	{
		m_pLastMessageTextNode.Release();
	}
	
	if (m_pHistoryWrapper)
	{
		m_pHistoryWrapper->Release();
		m_pHistoryWrapper = NULL;
	}
	
	if (m_pWebBrowser)
	{
		ReceiveEvents(FALSE, m_pWebBrowser);
		m_pDocument.Release();
		m_pWebBrowser.Release();

		m_pDocument = NULL;
		m_pWebBrowser = NULL;
	}
}


LRESULT CUIMHistoryCtrl::OnEraseBackground(HDC hDc)
{
	return 1;
}


void CUIMHistoryCtrl::OnSize(UINT, CSize)
{
	Invalidate();

	SetMsgHandled(FALSE);
}


void CUIMHistoryCtrl::OnTimer(UINT nTimerId, TIMERPROC /*timerproc*/)
{
	if (nTimerId == TIMER_ASYNC_MESSAGE_QUEUE)
	{
		if (m_pDocument != NULL)
		{
			// Got IHTMLDocument2 pointer, add pending messages to IE control
			CComCritSecLock<CComAutoCriticalSection> lock(m_csAsyncAddEntries);

			while (!m_listAsyncAddRecords.IsEmpty())
			{
				IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord = m_listAsyncAddRecords.RemoveHead();

				ATLTRACE(_T("CUIMHistoryCtrl - Add message asynchronously\n"));
				AddMessage(m_pDocument, pMessageTemplateRecord);
			}
		}
	}
}


LRESULT CUIMHistoryCtrl::OnUpdateCustomEmoticon(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HRESULT hrDocumentUpdated = UpdateDocument(TRUE);
	
	::SendMessage(m_hwndConversationWindow, 
			CConversationWindow::WM_USER_SHOW_INPUT_FOCUS, 0, 0);
	
	return hrDocumentUpdated;
}


void CUIMHistoryCtrl::OnAddCustomEmoticon(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csClosingWindow);
	
	if (nID == COMMAND_ID_ADD_CUSTOM_EMOTICON)
	{
		// Save current edit content.
		//
		::SendMessage(m_hwndConversationWindow, WM_USER_GET_CURRENT_TEXT, 0, 0);
		
		IUOLMessengerUIManager5Ptr pUIManager5 = 
				ap_dynamic_cast<IUOLMessengerUIManager5Ptr>(
						CUIMApplication::GetApplication()->GetUIManager());
		
		IUOLMessengerSettingsManager2Ptr pSettingsManager2 = 
				ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(
						CUIMApplication::GetApplication()->GetSettingsManager());
		
		IUOLMessengerEmoticonSettings3Ptr pEmoticonSettings3;
		
		ATLASSERT(pUIManager5);
		ATLASSERT(pSettingsManager2);
		
		if (pSettingsManager2)
		{
			pEmoticonSettings3 = ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>
					(pSettingsManager2->GetEmoticonSettings());
		}
		
		ATLASSERT(pEmoticonSettings3);
		
		if ( (pUIManager5) && (pEmoticonSettings3) )
		{
			IUOLMessengerAddCustomEmoticonWindowPtr pAddCustomEmoticonWindow = 
					pUIManager5->GetAddCustomEmoticonDialog(
							m_strAddingEmoticonShortcut, m_strAddingEmoticonPath, 
							CAddCustomEmoticonDialog::OPERATION_MODE_ADD_FROM_MESSAGE_WINDOW, NULL);
			
			if ( (pAddCustomEmoticonWindow) && (pAddCustomEmoticonWindow->ShowModal() == IDOK) )
			{
				pEmoticonSettings3->AddEmoticon2(
						pAddCustomEmoticonWindow->GetCustomEmoticonShortcut(), 
						pAddCustomEmoticonWindow->GetCustomEmoticonFileName());
				
				pEmoticonSettings3->SaveSettings();
				
				// Force update of emoticons parsers.
				//
				CEmoticonParserManager::GetInstance()->PrepareTags();
				
				// Restore current edit content, forcing the calculation of the 
				// existent emoticon indexes.
				//
				::SendMessage(m_hwndConversationWindow, WM_USER_REFRESH_CURRENT_TEXT, 0, 0);
			}
		}
	}
}


LRESULT CUIMHistoryCtrl::OnAcceptFile(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IUOLMessengerMessageTemplateRecordPtr pFileTransferRecord = 
			GetFileTransferMessageTemplateRecord(wParam);
	
	if (pFileTransferRecord)
	{
		IUOLMessengerConversationManager2Ptr pConversationManager2 = 
				ap_dynamic_cast<IUOLMessengerConversationManager2Ptr>(
						CUIMApplication::GetApplication()->GetConversationManager());
		
		CMessageTemplateRecordPtr pMessageTemplateRecord = 
				ap_dynamic_cast<CMessageTemplateRecordPtr>(pFileTransferRecord);
		
		if ( (pConversationManager2) && (pMessageTemplateRecord) && 
				(pMessageTemplateRecord->GetRequestFileTransferAsyncInfo()->IsWaitingDecision()) )
		{
			pMessageTemplateRecord->GetRequestFileTransferAsyncInfo()->SetTransferAccepted();
			
			pConversationManager2->AcceptFileAsync(
					m_pConversation, pMessageTemplateRecord->GetRequestFileTransferAsyncInfo());
			
			UpdateDocument(TRUE);
		}
	}
	
	return 0L;
}


LRESULT CUIMHistoryCtrl::OnRejectFile(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IUOLMessengerMessageTemplateRecordPtr pFileTransferRecord = 
			GetFileTransferMessageTemplateRecord(wParam);
	
	if (pFileTransferRecord)
	{
		IUOLMessengerConversationManager2Ptr pConversationManager2 = 
				ap_dynamic_cast<IUOLMessengerConversationManager2Ptr>(
						CUIMApplication::GetApplication()->GetConversationManager());
		
		CMessageTemplateRecordPtr pMessageTemplateRecord = 
				ap_dynamic_cast<CMessageTemplateRecordPtr>(pFileTransferRecord);
		
		if ( (pConversationManager2) && (pMessageTemplateRecord) && 
				(pMessageTemplateRecord->GetRequestFileTransferAsyncInfo()->IsWaitingDecision()) )
		{
			pMessageTemplateRecord->GetRequestFileTransferAsyncInfo()->SetTransferRejected();
			
			pConversationManager2->RejectFileAsync(
					m_pConversation, pMessageTemplateRecord->GetRequestFileTransferAsyncInfo());
			
			UpdateDocument(TRUE);
		}
	}
	
	return 0L;
}


STDMETHODIMP_(void) CUIMHistoryCtrl::BeforeNavigate2(IDispatch* /*pDisp*/, VARIANT* url, VARIANT* /*Flags*/, VARIANT* /*TargetFrameName*/, VARIANT* /*PostData*/, VARIANT* /*Headers*/,  VARIANT_BOOL* Cancel)
{
	CString strURL(url->bstrVal);

	if ((0 != strURL.Find(_T("res://"))) &&
		(0 != strURL.Compare(_T("about:blank"))))
	{
		*Cancel = VARIANT_TRUE;

		CUIMApplication::GetApplication()->ExecuteCommand(new COpenBrowserCommand(strURL));
	}
}


STDMETHODIMP_(void) CUIMHistoryCtrl::DocumentComplete(IDispatch* pDisp, VARIANT* URL)
{
	CComPtr<IDispatch> pDispatch;
	HRESULT hr;
	
	if (!m_pDocument)
	{
		// Get IDocument2 pointer.
		//
		hr = m_pWebBrowser->get_Document(&pDispatch);
		
		if (SUCCEEDED(hr))
		{
			m_pDocument = pDispatch;
		}
	}
	
	if (!m_strHtmlDocument.IsEmpty())
	{
		SAFEARRAY* psa = SafeArrayCreateVector(VT_VARIANT, 0, 1);
		VARIANT *param;
		bstr_t bsData = (LPCTSTR)m_strHtmlDocument;
		hr =  SafeArrayAccessData(psa, (LPVOID*)&param);
		
		if (hr != S_OK)
		{
			return;
		}
		
		param->vt = VT_BSTR;
		param->bstrVal = (BSTR)bsData;
		hr = m_pDocument->write(psa);	//write your buffer
		
		if (hr != S_OK)
		{
			return;
		}
		
		hr = m_pDocument->close();	//and closes the document, "applying" your code  
		if (hr != S_OK)
		{
			return;
		}
		
		SafeArrayDestroy(psa);
		
		ScrollView();
		/*
		CComQIPtr<IHTMLElement> pBody;
		hr = m_pDocument->get_body(&pBody);
		//IHTMLElement2::doScroll(
		
		if (SUCCEEDED(hr))
		{
			CComQIPtr<IHTMLElement2> pBody2;
			hr = pBody->QueryInterface(IID_IHTMLElement2, (void**)&pBody2);

			if (SUCCEEDED(hr))
			{
				pBody2->put_scrollTop(10000); 
			}
		}
		*/
	}
}


void CUIMHistoryCtrl::OnCustomEmoticonStored(IUOLMessengerCustomEmoticonPtr pCustomEmoticon)
{
	CustomEmoticonElement* pCustomEmoticonElmt = new CustomEmoticonElement();
	HRESULT hr = E_FAIL;
	
	if (pCustomEmoticonElmt)
	{
		pCustomEmoticonElmt->strEmoticonShortcut = pCustomEmoticon->GetEmoticonShortcut();
		pCustomEmoticonElmt->strEmoticonPath = pCustomEmoticon->GetEmoticonPath();
		pCustomEmoticonElmt->nEmoticonHeight = pCustomEmoticon->GetEmoticonHeight();
		pCustomEmoticonElmt->nEmoticonWidth = pCustomEmoticon->GetEmoticonWidth();
		
		hr = SendMessage(WM_UPDATE_CUSTOM_EMOTICON, 0, (WPARAM) pCustomEmoticonElmt);
		
		delete pCustomEmoticonElmt;
	}
}


BOOL CUIMHistoryCtrl::CanShowCustomContextMenu()
{
	// Clear here to be updated on "IsClickOverCustomEmoticonImageElement".
	//
	m_strAddingEmoticonShortcut.Empty();
	m_strAddingEmoticonPath.Empty();
	
	return IsClickOverCustomEmoticonImageElement();
}


int CUIMHistoryCtrl::GetCustomContextMenuType()
{
	return CONTEXT_MENU_IMAGE;
}


BOOL CUIMHistoryCtrl::CanShowTextSelectionOnlyMenu()
{
	return TRUE;
}


BOOL CUIMHistoryCtrl::CanShowImageSelectionOnlyMenu()
{
	return TRUE;
}


BOOL CUIMHistoryCtrl::CanForwardContextMenuHandling()
{
	return FALSE;
}


BOOL CUIMHistoryCtrl::IsHistoryIEServerWindow(HWND hWnd)
{
	BOOL bIsHistoryIEServerWindow = FALSE;
	HWND hWndParent = ::GetParent(hWnd);
	
	static TCHAR tszIEClass[] = "Internet Explorer_Server";
    TCHAR   tszClass[sizeof(tszIEClass)+1];
	
	::GetClassName(hWnd, tszClass, sizeof(tszClass));
	
	if (_tcscmp(tszClass, tszIEClass) == 0)
	{
		while ( (hWndParent != NULL) && (::GetParent(hWndParent) != NULL) )
		{
			hWndParent = ::GetParent(hWndParent);
			
			if (hWndParent == m_hWnd)
			{
				bIsHistoryIEServerWindow = TRUE;
				
				break;
			}
		}
	}
	
	return bIsHistoryIEServerWindow;
}


BOOL CUIMHistoryCtrl::IsClickOverCustomEmoticonImageElement()
{
	BOOL bIsOver = FALSE;
	
    POINT  CurPoint;
    GetCursorPos(&CurPoint);
	
	::ScreenToClient(m_hWnd, &CurPoint);
	
	if (m_pDocument != NULL)
	{
		CComQIPtr<IHTMLElement> pHitElement;
		
		// Get the element given by the coordinates.
		HRESULT hr = m_pDocument->elementFromPoint(CurPoint.x, CurPoint.y, &pHitElement);
		
		if (SUCCEEDED(hr))
		{
			CComBSTR bstrClassName;
			
			hr = pHitElement->get_className(&bstrClassName);
			
			if (SUCCEEDED(hr))
			{
				CString strClassName(bstrClassName);
				
				if (strClassName.CompareNoCase(CUSTOM_EMOTICON_IMAGE_TAG_CLASS_NAME) == 0)
				{
					CComQIPtr<IHTMLImgElement> pImageElement = pHitElement;
					
					if (pImageElement)
					{
						CComBSTR bstrEmoticonShortcut;
						CComBSTR bstrEmoticonPath;
						
						hr = pImageElement->get_alt(&bstrEmoticonShortcut);
						
						if (SUCCEEDED(hr))
						{
							hr = pImageElement->get_src(&bstrEmoticonPath);
						}
						
						if (SUCCEEDED(hr))
						{
							m_strAddingEmoticonShortcut = bstrEmoticonShortcut;
							m_strAddingEmoticonPath = bstrEmoticonPath;
							
							if (m_strAddingEmoticonPath.Find("file://") == 0)
							{
								DWORD bufsize = MAX_PATH;
								CString strEmoticonPath;
								LPTSTR buf = strEmoticonPath.GetBuffer(bufsize);
								::PathCreateFromUrl(m_strAddingEmoticonPath, buf, &bufsize, NULL);
								strEmoticonPath.ReleaseBuffer(bufsize);
								m_strAddingEmoticonPath = strEmoticonPath;
							}
							
							if ( (!m_strAddingEmoticonShortcut.IsEmpty()) && 
									(ATLPath::FileExists(m_strAddingEmoticonPath)) )
							{
								bIsOver = TRUE;
							}
						}
					}
				}
			}
		}
	}
	
	return bIsOver;
}


void CUIMHistoryCtrl::ShowCustomPopupMenu()
{
    POINT  CurPoint;
    GetCursorPos(&CurPoint);
	
	if(::IsMenu(m_hAddCustomEmoticonMenu))
    {
		::TrackPopupMenu(m_hAddCustomEmoticonMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
                CurPoint.x, CurPoint.y, 0, m_hWnd, 0);
    }
}


IUOLMessengerMessageTemplateRecordPtr CUIMHistoryCtrl::GetFileTransferMessageTemplateRecord(long iFileTransferId)
{
	IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord;
	
	for (POSITION MessageTemplateRecordPos = 
			m_listMessageTemplateRecords.GetHeadPosition(); MessageTemplateRecordPos; )
	{
		pMessageTemplateRecord = m_listMessageTemplateRecords.GetNext(MessageTemplateRecordPos);
		
		if ( (pMessageTemplateRecord) && 
				(pMessageTemplateRecord->GetFileTransferId() == iFileTransferId) )
		{
			return pMessageTemplateRecord;
		}
	}
	
	return IUOLMessengerMessageTemplateRecordPtr();
}


HRESULT CUIMHistoryCtrl::UpdateDocument(BOOL bForceRebuild)
{
	HRESULT hr = E_FAIL;
	
	m_strHtmlDocument = m_pDocumentBuilder->GetUpdatedDocument(bForceRebuild);
	
	if ( (!m_strHtmlDocument.IsEmpty()) && (m_pWebBrowser != NULL) )
	{
		//CComVariant vURL = CURLBuilder::ResourceToURL(IDR_HTML_MESSAGES_TEMPLATE);
		CComVariant vURL = _T("about:blank");
		CComVariant vFlags = navNoHistory;
		CComVariant vTarget = _T("_self");
		CComVariant vDummy;
		
		hr = m_pWebBrowser->Navigate2(&vURL, &vFlags, &vTarget, &vDummy, &vDummy);
		ATLASSERT(SUCCEEDED(hr));
	}
	
	return hr;
}


HRESULT CUIMHistoryCtrl::ExecFormsCommand(DWORD dwCommandID,
		VARIANT* pVarIn, VARIANT* pVarOut)
{
	HRESULT hr = E_FAIL;
	
	if (m_pDocument)
	{
		CComQIPtr<IOleCommandTarget> spCmdTarget = m_pDocument;
		
		if (spCmdTarget != NULL)
		{
			hr = spCmdTarget->Exec(&CMDSETID_Forms3, dwCommandID,
					OLECMDEXECOPT_DONTPROMPTUSER, pVarOut, pVarIn);
		}
	}
	
	return hr;
}


BOOL CUIMHistoryCtrl::IsHistoryCtrlChildWindow(HWND hWnd)
{
	BOOL bIsHistoryCtrlChildWindow = FALSE;
	HWND hWndParent = ::GetParent(hWnd);
	
	while (hWndParent != NULL)
	{
		if (hWndParent == m_hWnd)
		{
			bIsHistoryCtrlChildWindow = TRUE;
			
			break;
		}
		
		hWndParent = ::GetParent(hWndParent);
	}
	
	return bIsHistoryCtrlChildWindow;
}


void CUIMHistoryCtrl::ForwardMessageToWebBrowser(MSG* pMsg)
{
	EnumChildWindows(m_hWnd, &EnumHistoryCtrlChildWinProc, (LPARAM)pMsg);
}


HRESULT CUIMHistoryCtrl::AddMessage(IHTMLDocument2* pDocument, IUOLMessengerMessageTemplateRecordPtr pNewMessageTemplateRecord)
{
	HRESULT hr = E_FAIL;
	
	if ( (pDocument != NULL) && (m_pDocumentBuilder) )
	{
		m_pDocumentBuilder->AddMessageTemplateRecord(pNewMessageTemplateRecord);
		
		hr = UpdateDocument(FALSE);
	}
	
	return hr;
}


IUOLMessengerCustomEmoticonManagerPtr CUIMHistoryCtrl::GetCustomEmoticonManager()
{
	IUOLMessengerConversationManagerPtr pConversationManager = 
			CUIMApplication::GetApplication()->GetConversationManager();
	
	IUOLMessengerConversationManager2Ptr pConversationManager2 = NULL;
	IUOLMessengerCustomEmoticonManagerPtr pCustomEmoticonManager = NULL;
	
	if (pConversationManager)
	{
		pConversationManager2 = 
				ap_dynamic_cast<IUOLMessengerConversationManager2Ptr>(pConversationManager);
	}
	
	if (pConversationManager2)
	{
		pCustomEmoticonManager = pConversationManager2->GetCustomEmoticonManager();
	}
	
	return pCustomEmoticonManager;
}


void CUIMHistoryCtrl::ScrollView()
{
	if (m_pDocument != NULL)
	{
		if (COpenBrowserHelper::IsIE7OrNewer())
		{
			HRESULT hr = E_FAIL;

			CComQIPtr<IHTMLElement> pBody;
			hr = m_pDocument->get_body(&pBody);

			if (SUCCEEDED(hr))
			{
				CComVariant vParam = VARIANT_FALSE;
				hr = pBody->scrollIntoView(vParam);
			}
		}
		else
		{
			long len;
			VARIANT vBottom;
			VARIANT empty;

			CComPtr<IHTMLElementCollection> pElementCollection;
			CComPtr<IDispatch> pDispatch;
			CComPtr<IHTMLElement> pElement;

			// Find last html element and scroll it to view
			HRESULT hr = m_pDocument->get_all(&pElementCollection);
			if (SUCCEEDED(hr) && pElementCollection)
			{
				hr = pElementCollection->get_length(&len);
				if (SUCCEEDED(hr))
				{
					vBottom.vt = VT_I4;
					vBottom.lVal = len-1;
					VariantClear(&empty);

					hr = pElementCollection->item(vBottom, empty, &pDispatch);
					if (SUCCEEDED(hr) && pDispatch)
					{
						hr = pDispatch->QueryInterface(IID_IHTMLElement, (void**)&pElement);
						if (SUCCEEDED(hr) && pElement)
						{
							vBottom.vt = VT_BOOL;
							vBottom.boolVal = VARIANT_FALSE;

							hr = pElement->scrollIntoView(vBottom);
							if (SUCCEEDED(hr))
							{
								return;
							}
						}
					}
				}
			}

			// Scroll window
			CComPtr<IHTMLWindow2> pWindow;
			hr = m_pDocument->get_parentWindow(&pWindow);
			if (SUCCEEDED(hr) && (pWindow != NULL)) 
			{
				pWindow->scrollBy(-0x01FFFFFF, 0x01FFFFFF);
			}
		}
	}
}








///////////////////////////////////////////////////////////////////////////////
// CConversationWindow Class
#ifdef _CONVERSATION_WINDOW_DEBUG
LONG CConversationWindow::ms_lConversationId = 0;
#endif


CConversationWindow::CConversationWindow(IUOLMessengerConversation2Ptr pConversation) :
	m_pConversation(pConversation),
	m_nCharCount(0),
	m_bConnected(TRUE),
	m_bIsLogMessageEnabled(TRUE)
{
	m_bUpdateProportionalPos = FALSE;
	m_nProportionalPos = 8500;
	m_cxyBarEdge = 3;
	//m_cxyMin = 40;
	//m_bFullDrag = TRUE;

#ifdef _CONVERSATION_WINDOW_DEBUG
	::InterlockedIncrement(&ms_lConversationId);

	m_lId = ms_lConversationId;
#endif

	
	// Store conversation start time
	m_timeStart = CTime::GetCurrentTime();
}


IUOLMessengerConversationPtr CConversationWindow::GetConversation() const
{
	return m_pConversation;
}


void CConversationWindow::OnTextAreaChar()
{
	m_nCharCount++;
}


void CConversationWindow::SetMessageEditPanelBuilder(CMessageEditPanelBuilderPtr pBuilder)
{
	ATLASSERT(pBuilder.get());
	m_pMessagaEditPanelBuilder = pBuilder;
}


void CConversationWindow::ShowInputFocus()
{
	m_pEditPanel->ShowInputFocus();
}


void CConversationWindow::OnSize(UINT nType, const CSize& size)
{
	if (nType != SIZE_MINIMIZED)
	{
		SetSplitterRect();
        m_messageHistoryWindow.ScrollView();

		if ((m_pHorizontalSplitter) && (m_pVerticalSplitter))
		{
			m_pVerticalSplitter->SetSplitterRect();
			m_pHorizontalSplitter->SetSplitterRect();
		}	
    }
}


LRESULT CConversationWindow::OnCreate(LPCREATESTRUCT lpCreatestruct)
{
	////m_haccelerator = AtlLoadAccelerators(IDR_MAINFRAME);
	//ATL::_U_STRINGorID table = IDR_MAINFRAME;
	//m_haccelerator = ::LoadAccelerators(ATL::_AtlBaseModule.GetResourceInstance(), table.m_lpstr);

	SetWindowTitle();

	// Create CMessageEditPanel
	if (m_pMessagaEditPanelBuilder)
	{
		m_pEditPanel = ap_dynamic_cast<CMessageEditPanelPtr>(m_pMessagaEditPanelBuilder->CreateElement());
		ATLASSERT(m_pEditPanel);
	}

	if (m_pEditPanel.get())
	{
		CRect rect = CRect(0, 0, 0, 0);
		m_pEditPanel->Create(m_hWnd, rect);			
		ATLASSERT(m_pEditPanel.get());
		//m_pEditPanel->AddButtonsEventListener(this);

		m_pEditPanel->SetRichListener(this);
		m_pEditPanel->SetConversation(m_pConversation);
	}

	m_pEditPanel->ShowWindow(SW_NORMAL);
    
    CRect rectClient;
    GetClientRect(&rectClient);
    int nRectHeight = rectClient.Height();

    if (m_pConversation->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        // Create Splitter panes for chat conversation
        CreateVerticalSplitter();
        CreateHorizontalSplitter();
        
        // Create Upper and Lower containers
        m_pUpperContainer = new CUpperChatContainer();
        m_pLowerContainer = new CLowerChatContainer();
        m_pUpperContainer->SetConversation(m_pConversation);
        m_pLowerContainer->SetConversation(m_pConversation);
        if (m_pHorizontalSplitter)
        {
            CRect rect(0, 0, 0, 0);
            CWindow wndParent = (HWND) *m_pHorizontalSplitter.get();
            m_pUpperContainer->Create(wndParent, rect);
            m_pLowerContainer->Create(wndParent, rect);
        
            m_pHorizontalSplitter->SetSplitterPanes(*(m_pUpperContainer.get()), *(m_pLowerContainer.get()));
        }

        // Message History Window
        m_messageHistoryWindow.Create(m_pVerticalSplitter->m_hWnd, CRect(CPoint(0, 0), CSize(rectClient.Width(), (4 * nRectHeight) / 5)));
		m_messageHistoryWindow.SetConversation(m_pConversation);
		m_messageHistoryWindow.SetConversationWindowHWND(m_hWnd);

        m_pVerticalSplitter->SetSplitterPanes(m_messageHistoryWindow, m_pHorizontalSplitter->m_hWnd);
        SetSplitterPanes(m_pVerticalSplitter->m_hWnd, m_pEditPanel->m_hWnd);  

        //Set ContactList order traits
        m_pOrderTraits = new CSettingsContactListOrderTraits();        
        m_pUpperContainer->GetChatListCtrl()->SetOrderTraits(m_pOrderTraits);
        m_pLowerContainer->GetReadyForChatListCtrl()->SetOrderTraits(m_pOrderTraits);
	    m_pOrderTraits->SetSortType(CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->GetSortType());

        // Register as contact list settings event listener        
	    CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->AddEventListener(m_pUpperContainer->GetChatListCtrl().get());
        CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->AddEventListener(m_pLowerContainer->GetReadyForChatListCtrl().get());
        CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->AddEventListener(this);

        // Populate ReadyForChatList
        PopulateReadyForChatList();
        m_pLowerContainer->GetReadyForChatListCtrl()->Sort();
    }
    else
    {
        // Message History Window
        m_messageHistoryWindow.Create(m_hWnd, CRect(CPoint(0, 0), CSize(rectClient.Width(), (4 * nRectHeight) / 5)));
		m_messageHistoryWindow.SetConversation(m_pConversation);
		m_messageHistoryWindow.SetConversationWindowHWND(m_hWnd);

        SetSplitterPanes(m_messageHistoryWindow, m_pEditPanel->m_hWnd);
    }

	m_cxyMin = 80;
	SetSplitterPos(5000);

	m_pConversation->RegisterObserver(this);

	SetTimer(SEND_TYPING_TIMER, 5000, NULL);

	CUIMApplication::GetApplication()->AddMessageFilter(this);
	CUIMApplication::GetApplication()->GetAccountConnectionManager()->RegisterAccountObserver(GetConversation()->GetAccount(), this);

	NotifyAll(&CUOLMessengerConversationWindowObserver::OnCreate);

	// Get recent history
	IUOLMessengerMessageSettings3Ptr pMessageSettings3 = 
		ap_static_cast<IUOLMessengerMessageSettings3Ptr>(
					CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings());
	if ((pMessageSettings3) && (pMessageSettings3->IsShowRecentHistoryEnabled()))
	{
		CAtlList<IUOLMessengerHistoryEventPtr> listRecentEvents;
		CHistoryRecordMessagePtr pHistoryRecordMessage;
		pHistoryRecordMessage = new CHistoryRecordMessage(m_pConversation->GetAccount(), 
			m_pConversation->GetContactName());
		ap_static_cast<IUOLMessengerHistoryManager4Ptr>(
			CUIMApplication::GetApplication()->GetHistoryManager())->GetRecentEventRecord(pHistoryRecordMessage);
		pHistoryRecordMessage->GetRecentEvents(listRecentEvents);

		for (POSITION pos = listRecentEvents.GetHeadPosition(); pos;)
		{
			IUOLMessengerHistoryEventPtr pHistoryEvent = listRecentEvents.GetNext(pos);

			// Add message to history control...
			CMessageTemplateRecordPtr pMessageTemplateRecord = new CMessageTemplateRecord(
				m_pConversation, pHistoryEvent);

			MESSAGETEMPLATERECORDPARAM hrp;
			hrp.pMessageTemplateRecord = pMessageTemplateRecord;

			SendMessage(WM_USER_ADD_HISTORY_MESSAGE, (WPARAM) &hrp);
		}

		if (listRecentEvents.GetCount() > 0)
		{
			UOLLib::ISettings* pStringLoader = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			ATLASSERT(pStringLoader);
			CString strRecentInfo = pStringLoader->GetValue("uol.resource.IDS_RECENT_HISTORY_INFO");
			m_pConversation->AddSystemMessage(strRecentInfo, CTime::GetCurrentTime());
		}
	}

	return 0L;
}


void CConversationWindow::OnDestroy()
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	SetMsgHandled(FALSE);

	if (m_pEditPanel != NULL)
	{
		m_pEditPanel->SetRichListener(NULL);
	}

	CUIMApplication::GetApplication()->RemoveMessageFilter(this);

	KillTimer(SEND_TYPING_TIMER);

	m_pConversation->UnregisterObserver(this);
	
    if (m_pConversation->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->RemoveEventListener(m_pUpperContainer->GetChatListCtrl().get());
        CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->RemoveEventListener(m_pLowerContainer->GetReadyForChatListCtrl().get());
        CUIMApplication::GetApplication()->GetSettingsManager()->GetContactListSettings()->RemoveEventListener(this);

		ChatLeave();
    }

	CUIMApplication::GetApplication()->GetAccountConnectionManager()->UnregisterAccountObserver(GetConversation()->GetAccount(), this);

	NotifyAll(&CUOLMessengerConversationWindowObserver2::OnDestroy);

	if (m_pConversation->GetConversationType() != IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        CUIMApplication::GetApplication()->GetConversationManager()->DestroyConversation(m_pConversation->GetContact());
    }
    else
    {
        ap_static_cast<IUOLMessengerConversationManager2Ptr>(CUIMApplication::GetApplication()->GetConversationManager())->DestroyConversation(m_pConversation->GetChatKey());
    }
}


LRESULT CConversationWindow::OnEraseBackground(HDC hDc)
{
	return 1;
}


void CConversationWindow::OnTimer(UINT nTimerId, TIMERPROC /*timerproc*/)
{
	if (nTimerId == SEND_TYPING_TIMER)
	{
		if ((m_pEditPanel != NULL) &&
			(m_pEditPanel->IsWindow()))
		{
			BOOL bSendTyping = FALSE;

			if (m_nCharCount > 0)
			{
				m_nCharCount = 0;
				bSendTyping = TRUE;

				m_pConversation->SetTypingState(TYPING);
				//ATLTRACE(_T("CConversationWindow - TYPING\n"));
			}
			else if (m_pEditPanel->GetEditTextLength() > 0)
			{
				if (m_pConversation->GetTypingState() != TYPED)
				{
					bSendTyping = TRUE;
					m_pConversation->SetTypingState(TYPED);
					//ATLTRACE(_T("CConversationWindow - TYPED\n"));
				}
			}
			else if (m_pConversation->GetTypingState() != NOT_TYPING)
			{
				bSendTyping = TRUE;
				m_pConversation->SetTypingState(NOT_TYPING);
				//ATLTRACE(_T("CConversationWindow - NOT_TYPING\n"));
			}

			if (bSendTyping)
			{
				CUIMApplication::GetApplication()->GetConversationManager()->SendTyping(m_pConversation);
			}
		}
	}
}


void CConversationWindow::OnEditReturn(UINT nId, int nCode, HWND hWnd)
{
    CString strHTMLText = m_pEditPanel->GetEditText();
	
    ATLTRACE(_T("Send message: %s\n"), strHTMLText);
	
	if (!strHTMLText.IsEmpty())
    {
		if (m_pConversation->GetConversationType() != IUOLMessengerConversation::CT_CHAT_CONVERSATION)
		{
			IUOLMessengerContactPtr pContact = m_pConversation->GetContact();

			ATLASSERT(pContact != NULL);
			if (!pContact)
			{
				return;
			}

			IUOLMessengerAccountPtr pAccount = pContact->GetAccount();

			ATLASSERT(pAccount != NULL);
			if (!pAccount)
			{
				return;
			}

	#ifdef _CONVERSATION_WINDOW_DEBUG
			CString strDebugMessage;
			CString strWindowText;

			GetWindowText(strWindowText);

			strDebugMessage.Format(_T("[#%d] Sending message: Conv=0x%x,Win=%s\n"), m_lId, m_pConversation.get(), strWindowText);

			::OutputDebugString(strDebugMessage);
	#endif

			CUIMApplication::GetApplication()->GetConversationManager()->SendIM(m_pConversation, strHTMLText);

			m_nCharCount = 0;
		}
		else
		{
			IUOLMessengerAccountPtr pAccount = m_pConversation->GetAccount();

			ATLASSERT(pAccount != NULL);
			if (!pAccount)
			{
				return;
			}

			IUOLMessengerConversationManager2Ptr pConversationManager2 = ap_dynamic_cast<IUOLMessengerConversationManager2Ptr>(CUIMApplication::GetApplication()->GetConversationManager());
			if (pConversationManager2)
			{
				pConversationManager2->ChatSend(m_pConversation, strHTMLText);
			}
		}
	}

	m_pEditPanel->ClearEditText();

	m_nCharCount = 0;
}


LRESULT CConversationWindow::OnAddHistoryMessage(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
	LPMESSAGETEMPLATERECORDPARAM lpMessageTemplateRecordParam = (LPMESSAGETEMPLATERECORDPARAM) wParam;

	m_messageHistoryWindow.AddMessageTemplateRecord(lpMessageTemplateRecordParam->pMessageTemplateRecord);

	return 0L;
}


LRESULT CConversationWindow::OnNudgeButtonClick(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pConversation)
	{
		if (m_pConversation->GetConversationType() != IUOLMessengerConversation::CT_CHAT_CONVERSATION)
        {
            CUIMApplication::GetApplication()->ExecuteCommand(new CNudgeContactCommand(m_pConversation));

			// Shake MessageWindowFrame
			CMessageWindowFramePtr pMessageWindow;
			IUOLMessengerConversationPtr pTempConversation;

			IUOLMessengerMessageWindowManagerPtr pConversationManager = CUIMApplication::GetApplication()->GetUIManager()->GetMessageWindowManager();
			pMessageWindow = ap_dynamic_cast<CMessageWindowFramePtr>(pConversationManager->FindConversationMessageWindow(m_pConversation->GetAccount(), m_pConversation->GetContact()->GetName(), pTempConversation));

			if (pMessageWindow)
			{
				CUIMApplication::GetApplication()->PlayNudgeSound(m_pConversation->GetContact());
				CUIMApplication::GetApplication()->ShakeWindow(pMessageWindow->m_hWnd);				
			}
		}
	}
	
	return 0L;
}


LRESULT CConversationWindow::OnShowInputFocus(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ShowInputFocus();
	
	return 0L;
}


LRESULT CConversationWindow::OnGetCurrentText(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_strCurrentText = m_pEditPanel->GetEditPlainText();
	
	return 0L;
}


LRESULT CConversationWindow::OnRefreshCurrentText(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!m_strCurrentText.IsEmpty())
	{
		m_pEditPanel->ClearEditText();
		m_pEditPanel->SetEditPlainText(m_strCurrentText);
	}
	
	return 0L;
}


void CConversationWindow::OnAddMessage(IUOLMessengerConversation* pConversation, IUOLMessengerConversation::MessageEntryPtr pMessageEntry)
{
	if ((FALSE == IsWindow()) ||
		(pConversation != m_pConversation.get()))
	{
		ATLASSERT(FALSE);
		return;
	}
		
	// Add message to history control...
	CMessageTemplateRecordPtr pMessageTemplateRecord = new CMessageTemplateRecord(
			m_pConversation, pMessageEntry, NULL);
	
	MESSAGETEMPLATERECORDPARAM hrp;
	hrp.pMessageTemplateRecord = pMessageTemplateRecord;
	
	SendMessage(WM_USER_ADD_HISTORY_MESSAGE, (WPARAM) &hrp);
	
	ATLTRACE(_T("From protocol: %s\n"), pMessageEntry->strText);
	
	// Log in history...
	if ((m_pConversation->IsLoggingEnabled()) && IsLogMessageEnabled())
	{
        // Verify conversation start time to avoid save different days on the same history file 
		CTime timeNow = CTime::GetCurrentTime();
		if ((timeNow.GetYear() != m_timeStart.GetYear()) ||
			(timeNow.GetMonth() != m_timeStart.GetMonth()) ||
			(timeNow.GetDay() != m_timeStart.GetDay()))
		{
			m_timeStart = timeNow;
		}		

		if (m_pConversation->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
		{
			IUOLMessengerAccountPtr pAccount = m_pConversation->GetAccount();

			// Log history for each chat participant
			CAtlList<IUOLMessengerContact3Ptr> listChatContacts;
			m_pConversation->GetChatParticipantList(listChatContacts);

			for (POSITION pos = listChatContacts.GetHeadPosition(); pos;)
			{
				IUOLMessengerContact3Ptr pChatContact = ap_dynamic_cast<IUOLMessengerContact3Ptr>(listChatContacts.GetNext(pos));
				if (pChatContact)
				{
					CHistoryRecordChatPtr pHistoryRecordChat;
					pHistoryRecordChat = new CHistoryRecordChat(pAccount, 
						pChatContact->GetName(), pMessageEntry->strAuthor, 
						pChatContact->GetName(), pMessageEntry);

					pHistoryRecordChat->SetTime(m_timeStart);
					ap_static_cast<IUOLMessengerHistoryManager4Ptr>(
						CUIMApplication::GetApplication()->GetHistoryManager())->LogEvent(pHistoryRecordChat);
				}
			}
		}
		else
		{
			CHistoryRecordMessagePtr pHistoryRecordMessage;
			pHistoryRecordMessage = new CHistoryRecordMessage(m_pConversation->GetAccount(), 
				m_pConversation->GetContactName(), pMessageEntry);
			pHistoryRecordMessage->SetTime(m_timeStart);
			ap_static_cast<IUOLMessengerHistoryManager4Ptr>(
				CUIMApplication::GetApplication()->GetHistoryManager())->LogEvent(pHistoryRecordMessage);
		}
	}
}


void CConversationWindow::OnAddChatParticipant(IUOLMessengerConversation2* pConversation, IUOLMessengerContact3Ptr pChatContact)
{
    m_pUpperContainer->GetChatListCtrl()->AddChatContact(pChatContact);

    if (m_pLowerContainer->GetReadyForChatListCtrl()->IsWindow())
    {
        m_pLowerContainer->GetReadyForChatListCtrl()->UpdateScrollSize();
    }

    SetWindowTitle();

    // Warn user
    UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
    ATLASSERT(pStringLoader);

    CString strMessage;
    CString strAlias;
    strAlias = pChatContact->GetAlias();
    if (strAlias.IsEmpty())
    {
        strAlias = pChatContact->GetName();
    }

    CString strFormat = pStringLoader->GetValue("uol.resource.IDS_CHAT_ADD_PARTICIPANT_MSG");
    strMessage.Format(strFormat, strAlias);
    m_pConversation->AddSystemMessage(strMessage, CTime::GetCurrentTime());

    NotifyAllChat(&CUOLMessengerConversationWindowObserver2::OnUpdateChatTitle);
}


void CConversationWindow::OnRemoveChatParticipant(IUOLMessengerConversation2* pConversation, IUOLMessengerContact3Ptr pChatContact)
{
    if ((!m_pConversation) || (FALSE == IsWindow()) || (pConversation != m_pConversation.get()))
	{
		ATLASSERT(FALSE);
		return;
	}
    
    m_pUpperContainer->GetChatListCtrl()->RemoveChatContact(pChatContact);

    if (m_pLowerContainer->GetReadyForChatListCtrl()->IsWindow())
    {
        m_pLowerContainer->GetReadyForChatListCtrl()->UpdateScrollSize();
    }

    SetWindowTitle();

    // Warn user
    UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
    ATLASSERT(pStringLoader);

    CString strMessage;
    CString strAlias;
    strAlias = pChatContact->GetAlias();
    if (strAlias.IsEmpty())
    {
        strAlias = pChatContact->GetName();
    }

    CString strFormat = pStringLoader->GetValue("uol.resource.IDS_CHAT_REMOVE_PARTICIPANT_MSG");
    strMessage.Format(strFormat, strAlias);
    m_pConversation->AddSystemMessage(strMessage, CTime::GetCurrentTime());

    // If there are no more chat participants, disable this chat
    if (m_pConversation->GetChatParticipantCount() <= 2)
    {
        m_pVerticalSplitter->SetSinglePaneMode(SPLIT_PANE_LEFT);

        m_pEditPanel->EnableWindow(FALSE);
    }

    NotifyAllChat(&CUOLMessengerConversationWindowObserver2::OnUpdateChatTitle);
}


void CConversationWindow::OnAddRequestFileTransferAsync(
		IUOLMessengerConversation2* pConversation, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	PlayFileTransferAlertSound();

	IUOLMessengerConversation::MessageEntryPtr pMessageEntry = new IUOLMessengerConversation::MessageEntry();
	
    UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(
			CUIMApplication::GetApplication())->GetStringLoader();
    
	ATLASSERT(pStringLoader);
    
	pMessageEntry->time = CTime::GetCurrentTime();
	pMessageEntry->nMessageSender = IUOLMessengerConversation::MT_FILE_TRANSFER;
	
	CString fileTransferMessageFormat;
	
	if (pStringLoader)
	{
		fileTransferMessageFormat = pStringLoader->GetValue(
				"uol.resource.IDS_HISTORY_FILE_TRANSFER_MESSAGE_FORMAT");
	}
	
	if (fileTransferMessageFormat.IsEmpty())
	{
		fileTransferMessageFormat = "\"%s\" quer lhe enviar o arquivo [%s]. Deseja recebê-lo?";
	}
	
	pMessageEntry->strText.Format(fileTransferMessageFormat, 
			pConversation->GetContact()->GetAlias(), 
			pFileTransferAsyncInfo->GetFileName().c_str());
	
	CMessageTemplateRecordPtr pMessageTemplateRecord = new CMessageTemplateRecord(
			m_pConversation, pMessageEntry, pFileTransferAsyncInfo);
	
	MESSAGETEMPLATERECORDPARAM hrp;
	hrp.pMessageTemplateRecord = pMessageTemplateRecord;
	
	SendMessage(WM_USER_ADD_HISTORY_MESSAGE, (WPARAM) &hrp);
}


BOOL CConversationWindow::PreTranslateMessage(MSG* pMsg)
{
	// << debug code
	//return FALSE;
	// >> debug code
	// Pass keyboard messages along to the child window that has the focus.
	// When the browser has the focus, the message is passed to its containing
	// CAxWindow, which lets the control handle the message.
	if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
	{
		return FALSE;
	}

	HWND hWndCtl = ::GetFocus();

	if (IsChild(hWndCtl))
	{
	    //if(::TranslateAccelerator(m_hWnd, m_haccelerator, pMsg))
		   // return TRUE;

		// Find a direct child of the dialog from the window that has focus
		while (::GetParent(hWndCtl) != m_hWnd)
		{
			hWndCtl = ::GetParent(hWndCtl);
		}

		// Give control a chance to translate this message
		if (::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
		{
			return TRUE;
		}

		if (m_pVerticalSplitter)
		{
			if (m_pVerticalSplitter->m_hWnd == hWndCtl)
			{
				if (::SendMessage(m_messageHistoryWindow.m_hWnd, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
				{
					return TRUE;
				}
			}
		}

		if (pMsg->message == WM_MOUSEWHEEL)
		{
			m_messageHistoryWindow.ForwardMessageToWebBrowser(pMsg);
		}
	}

	// A normal control has the focus, so call IsDialogMessage() so that
	// the dialog shortcut keys work (TAB, etc.)
	//return IsDialogMessage(pMsg);
	return FALSE;
}


void CConversationWindow::OnConnected(IUOLMessengerAccountPtr pAccount)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	m_pEditPanel->OnConnected();
	m_pEditPanel->EnableWindow(TRUE);
	m_bConnected = TRUE;
}


void CConversationWindow::OnDisconnected(IUOLMessengerAccountPtr pAccount)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
    HandleDisconnectionForChat();
	m_pEditPanel->OnDisconnected();
	m_pEditPanel->EnableWindow(FALSE);
	m_bConnected = FALSE;
}


void CConversationWindow::OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
    if (m_pConversation->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        if (m_pUpperContainer->GetChatListCtrl())
        {
            m_pUpperContainer->GetChatListCtrl()->RemoveChatContacts();

            // Hide ChatListCtrl
            if (m_pVerticalSplitter)
            {
                m_pVerticalSplitter->SetSinglePaneMode(SPLIT_PANE_LEFT);
            }
        }
    }

	m_pEditPanel->OnReportDisconnect();
	m_pEditPanel->EnableWindow(FALSE);
}


IUOLMessengerMessageEditPanelPtr CConversationWindow::GetMessageEditPanel() const
{
	return m_pEditPanel;
}


void CConversationWindow::RegisterObserver(CUOLMessengerConversationWindowObserver2* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION pos = m_listObservers.Find(pObserver);

		if (!pos)
		{
			m_listObservers.AddTail(pObserver);
		}
	}
}


void CConversationWindow::UnregisterObserver(CUOLMessengerConversationWindowObserver2* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION pos = m_listObservers.Find(pObserver);

		if (pos != NULL)
		{
			m_listObservers.RemoveAt(pos);
		}
	}
}


void CConversationWindow::SelectAllText()
{
	m_pEditPanel->SelectAllText();
}


HWND CConversationWindow::GetHistoryCtrlHandle()
{
	return m_messageHistoryWindow.m_hWnd;
}

HWND CConversationWindow::GetRichEditCtrlHandle()
{
	return m_pEditPanel->GetRichEditCtrlHandle();
}


HWND CConversationWindow::GetSendButtonCtrlHandle()
{
	return m_pEditPanel->GetSendButtonCtrlHandle();
}

void CConversationWindow::UpdateMessageEditToolbarButtonVisible(const CString& strButtonCommand, BOOL bVisible)
{
	ATLTRACE("%s visible %d\n",__FUNCTION__, bVisible);
	
	m_pEditPanel->UpdateMessageEditToolbarButtonVisible(strButtonCommand, bVisible);
}

void CConversationWindow::NotifyAll(void (CUOLMessengerConversationWindowObserver2::*pfnUpdate)(IUOLMessengerConversationWindow*))
{
	CUOLMessengerConversationWindowObserver2* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
	{
		pObserver = m_listObservers.GetNext(pos);
		(pObserver->*pfnUpdate)(this);
	}
}


void CConversationWindow::NotifyAllChat(void (CUOLMessengerConversationWindowObserver2::*pfnUpdate)(IUOLMessengerConversationWindow2*))
{
	CUOLMessengerConversationWindowObserver2* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
	{
		pObserver = m_listObservers.GetNext(pos);
		(pObserver->*pfnUpdate)(this);
	}
}


CString CConversationWindow::GetConversationTitle()
{
	CString strTitle;
    if (m_pConversation->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();
		
		strTitle = m_pConversation->GetChatTitle();
        if ((strTitle.IsEmpty()) || 
			(FALSE == pAccountConnectionManager->IsConnected(m_pConversation->GetAccount())))
        {
            UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
            strTitle = pStringLoader->GetValue("uol.resource.IDS_CHAT_END_TAB_CAPTION");
        }
    }
    else
    {
        IUOLMessengerContactPtr pContact = m_pConversation->GetContact();
		CString strContact = pContact->GetAlias();

		if (strContact.IsEmpty())
		{
			strContact = pContact->GetName();
		}

		IUOLMessengerContact4Ptr pContact4 = ap_dynamic_cast<IUOLMessengerContact4Ptr>(pContact);
		if (pContact4)
		{
			CString strStatusMessage = pContact4->GetStatusMessage();
			if (!strStatusMessage.IsEmpty())
			{
				strTitle.Format("%s - %s", strContact, strStatusMessage);
			}
			else
			{
				strTitle = strContact;
			}
		}
		else
		{
			strTitle = strContact;
		}
    }
    
    return strTitle;
}

void CConversationWindow::SetWindowTitle()
{
    if (m_pConversation)
    {
        CString strTitle; 
        strTitle.Format(_T("%s"), GetConversationTitle());

	    SetWindowText(strTitle);
    }
}


void CConversationWindow::PopulateReadyForChatList()
{
    if (m_pLowerContainer->GetReadyForChatListCtrl())
    {
        IUOLMessengerAccountPtr pAccount;
        pAccount = m_pConversation->GetAccount();
        CString strAccountUserId;
        if (pAccount)
        {
            strAccountUserId = pAccount->GetUserId();

            IUOLMessengerContactListPtr pContactList = CUIMApplication::GetApplication()->GetContactList();
            CAtlList<IUOLMessengerContactPtr> listContacts;
            if (pContactList->FindContacts(pAccount, listContacts))
            {
                for (POSITION pos = listContacts.GetHeadPosition(); pos; )
                {
                    IUOLMessengerContactPtr pContact = listContacts.GetNext(pos);
                    if (pContact)
                    {
                        m_pLowerContainer->GetReadyForChatListCtrl()->AddChatContact(pContact);
                    }
                }
            }
        }
    }
}


void CConversationWindow::ChatLeave()
{
    if (m_pConversation)
	{
		if (m_pConversation->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
        {
            CUIMApplication::GetApplication()->ExecuteCommand(new CLeaveChatCommand(m_pConversation));
        }
	}
}

void CConversationWindow::HandleDisconnectionForChat()
{
    if (m_pConversation->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        if (m_pUpperContainer->GetChatListCtrl())
        {
            m_pEditPanel->EnableWindow(FALSE);

            m_pUpperContainer->GetChatListCtrl()->RemoveChatContacts();

            // Hide ChatListCtrl
            if (m_pVerticalSplitter)
            {
                m_pVerticalSplitter->SetSinglePaneMode(SPLIT_PANE_LEFT);
            }

            UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
            ATLASSERT(pStringLoader);

            CString disconnectedWarning;
            disconnectedWarning = pStringLoader->GetValue("uol.resource.IDS_CHAT_ERROR_DISCONNECTED");
            m_pConversation->AddSystemMessage(disconnectedWarning, CTime::GetCurrentTime());
        }

		NotifyAllChat(&CUOLMessengerConversationWindowObserver2::OnUpdateChatTitle);
    }
}


void CConversationWindow::CreateVerticalSplitter()
{
    CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
    IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__ConversationWindowVerticalSplitterBuilderInstance");

    if (pElement)
    {
        CConversationWindowVerticalSplitterBuilderPtr pBuilder;
        pBuilder = ap_dynamic_cast<CConversationWindowVerticalSplitterBuilderPtr>(pElement);

        if (pBuilder)
        {
            m_pVerticalSplitter = ap_dynamic_cast<CConversationWindowVerticalSplitterPtr>(pBuilder->CreateElement());

            if (m_pVerticalSplitter)
            {
                DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
                CRect rect = CRect(0, 0, 0, 0);

                m_pVerticalSplitter->Create(m_hWnd, rect, NULL, dwStyle);
            }
        }
    }
}


void CConversationWindow::CreateHorizontalSplitter()
{
    CUIManagerPtr pUIManagerPtr = ap_dynamic_cast<CUIManagerPtr>(CUIMApplication::GetApplication()->GetUIManager());
    IElementPtr pElement = pUIManagerPtr->GetWindowManager()->GetElement("__ConversationWindowHorizontalSplitterBuilderInstance");

    if (pElement)
    {
        CConversationWindowHorizontalSplitterBuilderPtr pBuilder;
        pBuilder = ap_dynamic_cast<CConversationWindowHorizontalSplitterBuilderPtr>(pElement);

        if (pBuilder)
        {
            m_pHorizontalSplitter = ap_dynamic_cast<CConversationWindowHorizontalSplitterPtr>(pBuilder->CreateElement());
            
            if ((m_pVerticalSplitter) && (m_pHorizontalSplitter))
            {
                DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
                CRect rect = CRect(0, 0, 0, 0);

                m_pHorizontalSplitter->Create(m_pVerticalSplitter->m_hWnd, rect, NULL, dwStyle);
            }
        }
    }
}


void CConversationWindow::OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId)
{
    if (m_pConversation->GetConversationType() == IUOLMessengerConversation::CT_CHAT_CONVERSATION)
    {
        IUOLMessengerFacade* pApplication = CUIMApplication::GetApplication();
        IUOLMessengerContactListSettingsPtr pContactListSettings = pApplication->GetSettingsManager()->GetContactListSettings();

        if (pSource == pContactListSettings.get())
        {
            if (IUOLMessengerContactListSettings::CL_SORT_TYPE == nSettingId)
            {
                // Update order traits sort method
                m_pOrderTraits->SetSortType(pContactListSettings->GetSortType());

                ATLASSERT(m_pLowerContainer->GetReadyForChatListCtrl() != NULL);
                m_pUpperContainer->GetChatListCtrl()->Sort();
                m_pLowerContainer->GetReadyForChatListCtrl()->Sort();
            }
            else if (IUOLMessengerContactListSettings::CL_CONTACT_NODE_LAYOUT == nSettingId)
            {
                m_pUpperContainer->GetChatListCtrl()->RequestUpdateWindow();
                m_pLowerContainer->GetReadyForChatListCtrl()->RequestUpdateWindow();
            }
        }
    }
}


void CConversationWindow::UpdateLayout()
{
    if ((m_pHorizontalSplitter) && (m_pVerticalSplitter))
    {
        m_pHorizontalSplitter->UpdateSplitterPosition();
        m_pVerticalSplitter->UpdateSplitterPosition();
    }
}


void CConversationWindow::HandleEditCopy()
{
	HWND hFocusWnd = ::GetFocus();
	
	if (hFocusWnd == m_pEditPanel->GetRichEditCtrlHandle())
	{
		::SendMessage(m_pEditPanel->GetRichEditCtrlHandle(), WM_COPY, 0, 0);
	}
	else if (m_messageHistoryWindow.IsHistoryCtrlChildWindow(hFocusWnd))
	{
		m_messageHistoryWindow.ExecFormsCommand(IDM_COPY, NULL, NULL);
	}
}


LRESULT CConversationWindow::OnUserDroppedFile(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPUSERDROPPEDFILEPARAM lpUserDroppedFileParam = (LPUSERDROPPEDFILEPARAM) wParam;

	CString strFileName = lpUserDroppedFileParam->strFileName;
	CUIMApplication::GetApplication()->ExecuteCommand(new CSendDroppedFileCommand(m_pConversation->GetContact(), m_hWnd, strFileName)); 

	return 0L;
}

void CConversationWindow::SetIsLogMessageEnabled(BOOL bEnabled)
{
	m_bIsLogMessageEnabled = bEnabled;
}


BOOL CConversationWindow::IsLogMessageEnabled()
{
	return m_bIsLogMessageEnabled;
}


void CConversationWindow::PlayFileTransferAlertSound()
{
	IUOLMessengerContactPtr pContact = m_pConversation->GetContact();

	BOOL bEnableSound = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->IsEnableSound();
	BOOL bIsMuteWhileAwayEnabled = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->IsMuteWhileAwayEnabled();
	DWORD dwAccountStatus = CUIMApplication::GetApplication()->GetAccountConnectionManager()->GetStatus(pContact->GetAccount());
	BOOL bAwayStatus = (dwAccountStatus == IUOLMessengerAccountConnectionManager::AS_AWAY);
	
	if (!bEnableSound || (bAwayStatus &&  bIsMuteWhileAwayEnabled))
	{
		// Do not play if mute when away
		return;
	}
	
	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
	if (pSettingsManager2)
	{
		CString strSoundPath;
		
		if (pSettingsManager2->GetSoundSettings()->GetSoundEventFilePath(IUOLMessengerSoundSettings3::FILE_TRANSFER_ALERT_SOUND_EVENT, strSoundPath, TRUE))
		{
			CUIMApplication::GetApplication()->GetUIManager()->PlaySound(strSoundPath, FALSE);
		}
	}
}


BOOL CALLBACK EnumHistoryCtrlChildWinProc(HWND hWnd, LPARAM lParam)  
{  
	MSG* pMsg = (MSG*)lParam;
	TCHAR szClassName[256];  
	GetClassName(hWnd, (LPTSTR)&szClassName, 256);  

	if (_tcscmp(szClassName, _T("Internet Explorer_Server")) == 0)  
	{  
		if (::SendMessage(hWnd, pMsg->message, pMsg->wParam, pMsg->lParam) != 0)
		{
			return FALSE;
		}
	}  
	return TRUE;  
}  