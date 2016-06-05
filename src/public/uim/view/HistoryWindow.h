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

#include <interfaces/IUOLMessengerAccount.h>
#include <interfaces/IUOLMessengerContact.h>
#include <interfaces/IUOLMessengerSettingsEventSource.h>
#include <interfaces/IUOLMessengerSettingsEventListener.h>
#include <interfaces/IUOLMessengerImageButtonContainer.h>
#include <interfaces/IUOLMessengerHistoryWindow.h>
#include <interfaces/IUOLMessengerFindPanel.h>
#include "skin/FontElement.h"

#include "UOLToolbar.h"
#include "DummyWindow.h"
#include "ImageButtonContainer.h"

#include "image/image.h"
#include "image/imagebuilder.h"
 
#include <interfaces/IUOLMessengerImagesProtocol.h>

#include "skin/windowelement.h"
#include "ImageButton.h"
#include "MultiStateImageButton.h"
#include "../controller/HistoryManager.h"
#include "MessageWindow.h"
#include "./WMEventNotifier.h"
#include "./RichEditCtrlWMEventNotifier.h"


class CHistoryWindow;
MAKEAUTOPTR(CHistoryWindow);

class CHistoryWindowObserverNotifier : public CObserverRegister<CUOLMessengerHistoryWindowObserver>
{
public:
	void NotifyAll(void (CUOLMessengerHistoryWindowObserver::*pfnUpdate)(IUOLMessengerHistoryWindow2*), IUOLMessengerHistoryWindow2* pHistoryWindow);
};

///////////////////////////////////////////////////////////////////
//  CHistoryTreeView
///////////////////////////////////////////////////////////////////
template<class T>
class CHistoryTreeView : public CWindowImpl<CHistoryTreeView<T>, CTreeViewCtrl>,
						 public COwnerDraw<CHistoryTreeView<T>>, 
						 public CWMEventNotifier<CHistoryTreeView<T>>
{
public:
	CHistoryTreeView();

private:
	BEGIN_MSG_MAP(CHistoryTreeView)    
		CHAIN_MSG_MAP(CWMEventNotifier<CHistoryTreeView>)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnTreeItemChanged)
		REFLECTED_NOTIFY_CODE_HANDLER(TVN_DELETEITEM, OnDeleteItem)
		DEFAULT_REFLECTION_HANDLER()
		CHAIN_MSG_MAP_ALT(COwnerDraw<CHistoryTreeView>,1)
	END_MSG_MAP()          	

	enum UserMessages
	{
		HISTORY_ITEM_SELECTED = WM_USER + 0x150	// sent by WM_COMMAND
	};

	LRESULT OnTreeItemChanged( int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnDeleteItem(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);

	void DeleteItem( LPDELETEITEMSTRUCT ) {};
};



//////////////////////////////////////////////////////////////////////////////
// CHistoryWindow
//////////////////////////////////////////////////////////////////////////////
class CHistoryWindow;
typedef CWindowElement<CHistoryWindow, CDummyWindow<CWindowImpl<CHistoryWindow> > > CHistoryWindowBase;

class CHistoryWindow :	public CHistoryWindowBase,
						public CUOLMessengerImageButtonEventListener2,
						public IUOLMessengerHistoryWindow2, 
						public CFindPanelObserver,
						public CUOLMessengerHistoryRecordListObserver
{

public:
	DECLARE_WND_CLASS_EX("HistoryWindow", CS_DBLCLKS, COLOR_WINDOW);

	CHistoryWindow();
	virtual ~CHistoryWindow();

	void SetFont(CFontElementPtr pFont) { m_pFont = pFont; }
	void SetHighlightTextColor(CCOLORREF clrHighlightText) { m_clrHighlightText = clrHighlightText; }
	void GetPreviousEvent(HTREEITEM hSelectedItem);
	void GetNextEvent(HTREEITEM hSelectedItem);
	void SetHistoryRecordList(IUOLMessengerHistoryRecordListPtr pHistoryRecordList);
	void UpdateEventsList();
	
	void FocusFindHistoryPanel();
	
	// IUOLMessengerHistoryWindow interface
	virtual IUOLMessengerImageButtonContainerPtr GetToolbar();
	/*deprecated*/ virtual IUOLMessengerHistoryContactPtr getContact();
	/*deprecated*/ virtual void SetContact(IUOLMessengerHistoryContactPtr pHistContact);
	virtual void RegisterObserver(CUOLMessengerHistoryWindowObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerHistoryWindowObserver* pObserver);

	// IUOLMessengerHistoryWindow2 interface
	virtual CString GetProtocolId();

	virtual void OnFindPanelTextChanged(CString strFindText);
    virtual void OnFindPanelArrowKeyPressed();
	virtual void OnFindPanelEscPressed();

	// CUOLMessengerHistoryRecordListObserver Interface
	virtual void OnUpdateHistoryRecordListStart(IUOLMessengerHistoryRecordPtr pHistoryRecord);
	virtual void OnUpdateHistoryRecordListFinish();
	
protected:
	BEGIN_MSG_MAP(CHistoryWindow)			
		MSG_WM_CREATE(OnCreate)	
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		COMMAND_CODE_HANDLER_EX(CHistoryTreeView<CString>::HISTORY_ITEM_SELECTED, OnItemSelected)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_ON_KEYDOWN_EVENT_NOTIFIER, OnKeyDownEventNotifier)
		MESSAGE_HANDLER(WM_ON_KEYUP_EVENT_NOTIFIER, OnKeyUpEventNotifier)
		MESSAGE_HANDLER(WM_ON_CHAR_EVENT_NOTIFIER, OnCharEventNotifier)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CHistoryWindowBase)
	END_MSG_MAP()
	
	LRESULT OnCreate(LPCREATESTRUCT lpCreatestruct);
	void OnClose();
	void OnDestroy();
	void OnSize(UINT, const CSize& size);
	void OnItemSelected(UINT nId, int nCode, HWND hWnd);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDownEventNotifier(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyUpEventNotifier(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCharEventNotifier(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void UpdateLayout(BOOL bResizeBars = TRUE);
	virtual void OnSettingChanged(IUOLMessengerSettingsEventSource* pSource, UINT nSettingId);
	virtual void GetMinSize(CSize& size);

private:
	enum FoundTextDirection
	{
		ftdFirst,
		ftdNext,
		ftdPrior
	};
	
	void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick);
	
	void CreateToolbars();
	void InsertTreeItens(IUOLMessengerHistoryRecordPtr pHistoryRecord);
	void FilterTreeItens();
	BOOL IsItem (const CString& strText, HTREEITEM& hFather);
	HTREEITEM GetLast(HTREEITEM hItem);
	int GetSelectedEvent();
	void ClearTreeItens();
	void ClearHistoryEdit();
	void HighLightFindText();
	void GotoFindText(FoundTextDirection direction);
	
	void HandleKeyDownEvent(int vKey);
	void HandleKeyUpEvent(int vKey);
	void HandleCharEvent(int vKey);

	void ShowWarningEmptyHistory();
	
private:
	CUOLToolBarPtr						m_pToolbar;
	CUOLToolBarPtr						m_pFinderToolbar;
	CSplitterWindow						m_wndSplitter;
	CFontElementPtr						m_pFont;
	CHistoryWindowObserverNotifier		m_ObserverNotifier;
	IUOLMessengerHistoryRecordListPtr	m_pHistoryRecordList;
	CComAutoCriticalSection				m_lockLog;
	CHistoryTreeView<CString>			m_HistoryTreeView;
	CRichEditCtrlWMEventNotifier		m_RichEdit;
	BOOL								m_bSplitterPositioned;
	CCOLORREF							m_clrHighlightText;
	CString								m_strFindText;
	BOOL								m_bIsCtrlKeyPressed;
	BOOL								m_bIsShowingHistoryText;
	CString								m_strLastMonthYearItem;
	CString								m_strLastDateItem;
	HTREEITEM							m_hLastMonthYearItem;
	HTREEITEM							m_hLastDateItem;
	CAtlList<CHARRANGE>					m_listFoundTextPos;
	int									m_nFoundTextIndex;
}; 
