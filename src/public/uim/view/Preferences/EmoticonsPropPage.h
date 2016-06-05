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

#include "PreferencesPropPage.h"
#include "../../resource.h"
#include <interfaces/IUOLMessengerEmoticonSettings.h>
#include <interfaces/IUOLMessengerEmoticon.h>
#include <interfaces/IUOLMessengerSettingsManager.h>
#include "PictureExWnd.h"
#include <IEEventsObserver.h>
#include "../UOLOwnerDraw.h"
#include "../IECTRLMenuHandler.h"
#include "../AddCustomEmoticonDialog.h"



class CEmoticonsViewCtrl : public CWindowImpl<CEmoticonsViewCtrl, CAxWindow>, 
						   public CIEEventsObserver/*,
						   public CIECTRLMenuHandler<CEmoticonsViewCtrl>*/
{
public:
	DECLARE_WND_CLASS_EX("EmoticonsViewCtrl", CS_DBLCLKS, COLOR_WINDOW);
	
	enum
	{
		COMMAND_ID_EMOTICON_DEL = 0x311, 
		COMMAND_ID_EMOTICON_EDIT_SHORTCUT, 
		COMMAND_ID_EMOTICON_EDIT_FILEPATH
	};
	
	enum
	{
		WM_EMOTICON_DEL = WM_USER + 0x332, 
		WM_EMOTICON_EDIT_SHORTCUT, 
		WM_EMOTICON_EDIT_FILEPATH
	};
	
	CEmoticonsViewCtrl();
	virtual ~CEmoticonsViewCtrl();
	
	void SetText(const CString& strText);
	
	void SetEmoticonSetIds(int nEmoticonSetIds);
	
	void SetEmoticonChangingInfoFinder(IEmoticonChangingInfoFinder* pEmoticonChangingInfoFinder);
	
	IUOLMessengerEmoticon2Ptr GetSelectedEmoticon();
	
	CString GetSelectedEmoticonShortcut();
	CString GetSelectedEmoticonFileName();
	
	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL);
	
protected:
	
	BEGIN_MSG_MAP(CEmoticonsViewCtrl)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		COMMAND_ID_HANDLER_EX(COMMAND_ID_EMOTICON_DEL, OnEmoticonAction)
		COMMAND_ID_HANDLER_EX(COMMAND_ID_EMOTICON_EDIT_SHORTCUT, OnEmoticonAction)
		COMMAND_ID_HANDLER_EX(COMMAND_ID_EMOTICON_EDIT_FILEPATH, OnEmoticonAction)
		MESSAGE_HANDLER_EX(WM_PARENTNOTIFY, OnParentNotify)
		MESSAGE_HANDLER_EX(WM_MOUSEACTIVATE, OnMouseActivate)
		//CHAIN_MSG_MAP(CIECTRLMenuHandler<CEmoticonsViewCtrl>)
	END_MSG_MAP()
	
	void OnDestroy();
	void OnSize(UINT, CSize);
	//void OnRButtonDown(UINT, const CPoint& ptClick);
	void OnEmoticonAction(UINT /*uCode*/, int nID, HWND hwndCtrl);
	LRESULT OnParentNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam);
	//virtual void HandleMouseRButtonDown();
	
	STDMETHOD_(void, DocumentComplete)(IDispatch* /*pDisp*/, VARIANT* URL);
	
	BOOL IsClickOverEmoticonImageElement();
	
	void ShowCustomPopupMenu();
	
private:
	
	CComPtr<IWebBrowser2>		m_pWebBrowser;
	CComPtr<IHTMLDocument2>		m_pDocument;
	CString						m_strHtml;
	
	HMENU						m_hCustomEmoticonMenu;
	CString						m_strSelectedEmoticonShortcut;
	CString						m_strSelectedEmoticonPath;
	int							m_nEmoticonSetIds;
	
	IUOLMessengerEmoticonSettings3Ptr	m_pEmoticonSettings;
	
	IUOLMessengerEmoticon2Ptr			m_pSelectedEmoticon;
	
	IEmoticonChangingInfoFinder*		m_pEmoticonChangingInfoFinder;
};






// CEmoticonsPropPage dialog

class CEmoticonsPropPage : public CPreferencesPropPage,
						   public CWinDataExchange<CEmoticonsPropPage>,
						   public IEmoticonChangingInfoFinder, 
						   public CUOLMessengerSettingsManagerObserver
{
public:
	CEmoticonsPropPage();
	virtual ~CEmoticonsPropPage();

	virtual void OnApply();

protected:
	BEGIN_MSG_MAP(CEmoticonsPropPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_ERASEBKGND(OnEraseBackground)
		COMMAND_HANDLER(IDC_EMOTICON_NEW, BN_CLICKED, OnClickedNew)
		COMMAND_CODE_HANDLER_EX(CBN_SELCHANGE, OnCbnSelChange)
		MESSAGE_HANDLER_EX(CEmoticonsViewCtrl::WM_EMOTICON_DEL, OnEmoticonDel)
		MESSAGE_HANDLER_EX(CEmoticonsViewCtrl::WM_EMOTICON_EDIT_SHORTCUT, OnEmoticonEditShortcut)
		MESSAGE_HANDLER_EX(CEmoticonsViewCtrl::WM_EMOTICON_EDIT_FILEPATH, OnEmoticonEditFilePath)
		MESSAGE_HANDLER_EX(IEmoticonChangingInfoFinder::WM_FIND_CHANGING_INFO, OnFindChangingInfo)
		CHAIN_MSG_MAP(CPreferencesPropPage)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CEmoticonsPropPage)
		DDX_CONTROL_HANDLE(IDC_EMOTICON_NEW, m_wndButtonNew)
	END_DDX_MAP()

// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);


	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnDestroy();
	LRESULT OnShowWindow(BOOL bShow, int nStatus);
	LRESULT OnEraseBackground(HDC hDc);
	LRESULT OnClickedNew(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCbnSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtl);
	
	LRESULT OnEmoticonDel(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnEmoticonEditShortcut(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnEmoticonEditFilePath(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnFindChangingInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	
	LRESULT OnCtlColor(HDC hdc, HWND hwnd);
	
	// CUOLMessengerSettingsManagerObserver overridden functions
	virtual void OnEmoticonSettingsChanged();
	
	void UpdateChangingList(CString strOldShortcut, CString strShortcut, 
			CString strFileName, int nOperationId);
	EmoticonChangingInfoPtr FindChangingItem(CString strShortcut, 
			int cmbOperationIds, BOOL bConsiderOldShortcut);
	
	void UpdateEmoticonList();
	void UpdateEmoticonListSimple(CString strSelectedEmoticonSet);
	void UpdateEmoticonListWithCustom(CString strSelectedEmoticonSet);
	void GenerateEmoticonListHtmlBlock(CString& strGeneratedHtmlBlock, const CString& strTemplateRow, 
			CAtlList<IUOLMessengerEmoticonPtr>& listEmoticon, BOOL bIsCustomList);
	
	void InitProtocolList();
	
	void CalculatePaintingRgns();
	
protected:

	IUOLMessengerEmoticonSettings3Ptr	m_pEmoticonSettings;

	CButton			m_wndButtonNew;
	CUOLComboBox	m_cmbEmoticonSetNames;
	CString			m_strEmoticonSetCustomName;
	
	CAtlList<EmoticonChangingInfoPtr>	m_changingList;
	
private:
	
	CString				m_templateFile;
	CString				m_templateFileWithCustom;
	
	CEmoticonsViewCtrl	m_emoticonCtrl;
	CRect				m_rectEmoticonCtrl;
	
	CCOLORREF			m_clrEmoticonCtrlBorder;
	HBRUSH				m_hEmoticonCtrlBorderBrush;
	
	HRGN				m_mainWindowRgn;
	HRGN				m_emoticonCtrlBorderRgn;
};

MAKEAUTOPTR(CEmoticonsPropPage);
