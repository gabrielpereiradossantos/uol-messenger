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

#include <interfaces/IUOLMessengerAccount.h>
#include <interfaces/IUOLMessengerAccountConnectionManager.h>
#include "../image/image.h"
#include "../image/imagebuilder.h"

#include <interfaces/IUOLMessengerImagesProtocol.h>
#include "../../controller/UIMApplication.h"

#include "../AccountButton.h"

#include <interfaces/IUOLMessengerPropertyPageCommandFactory.h>
#include "../../controller/PreferencesCommands/NewAccountPropertyPageCommand.h"


class CUOLAccountList : 
	public CWindowImpl<CUOLAccountList, CListBox>,
	public COwnerDraw<CUOLAccountList>
{

	DECLARE_WND_SUPERCLASS(NULL, CListBox::GetWndClassName())

	BEGIN_MSG_MAP(CUOLAccountList)

		CHAIN_MSG_MAP_ALT(COwnerDraw<CUOLAccountList>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	CUOLAccountList& operator = (HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}	

	void DeleteItem(LPDELETEITEMSTRUCT /*lpDeleteItemStruct*/)
	{
		// default - nothing
	}

	BOOL DeleteItem(int nItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_DELETEITEM, nItem, 0L);
	}

	CUOLAccountList()
	{
		m_pImagesProtocol = CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol();
	}

	struct AccountHelper
	{
		IUOLMessengerAccountPtr	pAccount;
	};

	void DrawItem(LPDRAWITEMSTRUCT lpdis)
	{
		AccountHelper* pH;
		pH = (AccountHelper*) lpdis->itemData;
		
		if ( NULL == pH )
		{
			return;
		}

		CIMProtocol *pProtocol;
		pProtocol = pH->pAccount->GetProtocol();

		
		CDCHandle dc = lpdis->hDC;
		CDC dcMem;

		dcMem.CreateCompatibleDC ( dc );
		dc.SaveDC();
		dcMem.SaveDC();

		COLORREF txtColor;

		CString text = "camp",str;
		text.Format(_T("%s"), pH->pAccount->GetUserId());

		txtColor = RGB(0,0,0);
		CRect lineRect (lpdis->rcItem);
		CRect picRect (lineRect);
		picRect.right = picRect.left+16;


		dc.SetBkMode(OPAQUE);
		dc.SetTextColor(txtColor);

		txtColor=RGB(0,0,0);

		COLORREF bkColor;
		if (lpdis->itemState & ODS_SELECTED) {
			bkColor = RGB(10,36,106);
			txtColor=RGB(255,255,255);
		} else if (lpdis->itemState & ODS_FOCUS)  {
			bkColor = RGB(212,208,200);

		} else {
			bkColor = RGB(255,255,255);
		}
		picRect.right = picRect.left+16;

		dc.SetBkColor(bkColor);
		dc.SetTextColor(txtColor);

		if ( lpdis->itemState & ODS_SELECTED ) 
			dc.BitBlt ( 1, 1, 80, 80, dcMem, 0, 0, SRCCOPY );
		else
			dc.BitBlt ( 0, 0, 80, 80, dcMem, 0, 0, SRCCOPY );
		CRect  tmpRect(lineRect);


		tmpRect.left+=20;

		dc.DrawText(text,text.GetLength(),tmpRect,DT_LEFT|DT_END_ELLIPSIS|DT_VCENTER);

		IUOLMessengerAccountConnectionManagerPtr pAccountConnectionManager = CUIMApplication::GetApplication()->GetAccountConnectionManager();
		
		IUOLMessengerImagePtr m_pImage =
			m_pImagesProtocol->GetUserStatusImage(pH->pAccount->GetProtocolId(), pAccountConnectionManager->GetStatus(pH->pAccount) );
		
		if (m_pImage.get()) 				
		{
			IUOLMessengerImageDrawStrategyPtr pStrategy =
				new UOLMessengerImage::CDrawStretchTransparentStrategy();

			m_pImage->Draw(dc, picRect.left, 
				picRect.top,
				picRect.Width(),
				picRect.Height(),
				pStrategy);	
		} 
		else
		{
			ATLTRACE(_T("CHistoryLogsView::DrawItem WARNING !NOT FOUND PROTOCOL!\n"));	
		}

		dcMem.RestoreDC(-1);
		dc.RestoreDC(-1);
	}

	int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
	{
		ATLASSERT(lpCompareItemStruct->CtlType == ODT_LISTBOX);
		LPCTSTR lpszText1 = (LPCTSTR) lpCompareItemStruct->itemData1;
		ATLASSERT(lpszText1 != NULL);
		LPCTSTR lpszText2 = (LPCTSTR) lpCompareItemStruct->itemData2;
		ATLASSERT(lpszText2 != NULL);

		return strcmp(lpszText1, lpszText2);
	}

private:
	IUOLMessengerImagesProtocolPtr m_pImagesProtocol;
};

// CAccountsPropPage dialog
class CAccountsPropPage : 
	public CPreferencesPropPage,
	public CUOLMessengerAccountConnectionManagerObserver,
	public CIdleHandler
{
public:
	
	enum
	{
		WM_NEW_ACCOUNT = WM_USER + 0x225
	};
	
	CAccountsPropPage();
	virtual ~CAccountsPropPage();

	void OnConnected(IUOLMessengerAccountPtr pAccount);
	void OnDisconnected(IUOLMessengerAccountPtr pAccount);
	void OnReportDisconnect(IUOLMessengerAccountPtr pAccount, ConnectionError error, const CString& strMessage);
	void OnStatusChanged(IUOLMessengerAccountPtr pAccount);
	
	virtual CString GetName() const
	{
		return "__CAccountsPropPageInstance";
	}

protected:
	BEGIN_MSG_MAP(CAccountsPropPage)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_CHANGE, OnButtonChange)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_ADDCONTACT, OnButtonAddAccount)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_REMOVE, OnButtonRemove)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_CONNECT, OnButtonConnect)
		NOTIFY_HANDLER_EX(IDC_LIST_ACCOUNTS, LVN_ITEMCHANGED, OnItemChangedAccountList)
		NOTIFY_HANDLER(IDC_LIST_ACCOUNTS, NM_DBLCLK, OnNMDblclkListAccounts)

		COMMAND_HANDLER(IDC_LIST_ACCOUNTS2, LBN_SELCHANGE, OnLbnSelchangeListAccounts2)
		COMMAND_HANDLER(IDC_LIST_ACCOUNTS2, LBN_DBLCLK, OnLbnDblclkListAccounts2)
		MESSAGE_HANDLER_EX(WM_NEW_ACCOUNT, OnNewAccount)
		CHAIN_MSG_MAP(CPreferencesPropPage)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()


	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	void OnDestroy();

	void OnButtonAddAccount(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnButtonChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnButtonRemove(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnButtonConnect(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	LRESULT OnLbnDblClkList(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnItemChangedAccountList(NMHDR *pNMHDR);
	LRESULT OnCtlColor(HDC hdc, HWND hwnd);
	LRESULT OnNewAccount(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL OnIdle();
	
	void OnButtonAddAccountImpl(IUOLMessengerPropertyPageCommand* pPropPageCommand);

private:
	struct AccountHelper
	{
		IUOLMessengerAccountPtr pAccount;

		AccountHelper& operator=(const AccountHelper& ah)
		{
			pAccount = ah.pAccount;
			return *this;
		}

		bool operator==(const AccountHelper& ah) const
		{
			return (pAccount == ah.pAccount);
		}
	};

private:
	static int CALLBACK CompareAccountHelper(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	void InitAccountList();
	void InitProtocolList();

	CString FormatAccountUserId(IUOLMessengerAccountPtr pAccount);
	AccountHelper* GetAccountHelper(int nInd);
	AccountHelper* GetCurrentSelectionAccountHelper(int& nCurSel);
	void ReleaseAccounts();
	void EditAccount();
	
	void OnSelChangeAccountListImpl();

	void ShowAccountConnectionState(DWORD dwConnectionState);
	BOOL CheckDuplicateAccount(const CString& strUserId, const CString& strProtocolId, IUOLMessengerAccountPtr pCurrentAccount);

	BOOL LoadImage(const CString& strImagePath);

private:
	CUOLAccountList	m_wndListAccounts2;
	CButton		m_wndButtonConnect;

	CString		m_strConnectCaption;
	CString		m_strDisconnectCaption;
	CString		m_strNewUserId;

	UINT		m_nUIState;
	IUOLMessengerAccountPtr		m_pCurrentAccount;
public:
	LRESULT OnNMDblclkListAccounts(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnLbnSelchangeListAccounts2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnLbnDblclkListAccounts2(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

MAKEAUTOPTR(CAccountsPropPage);