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

#include "../resource.h"
#include <interfaces/IUOLMessengerAccount.h>
#include <interfaces/IUOLMessengerContact.h>
#include <interfaces/IUOLMessengerPrivacyManager.h>

#include "../controller/UOLMessengerContactTraits.h"

#include "image/image.h"
#include "image/imagebuilder.h"

#include "../controller/UIMApplication.h"
#include <interfaces/IUOLMessengerImagesProtocol.h>

#include "criticalsection.h"
#include "critseclock.h"
#include "MainDialog.h"
#include "TransparentStatic.h"

struct AccountData
{
	IUOLMessengerAccountPtr	pAccount;
};


struct ContactData
{
	IUOLMessengerContactPtr pContact;
	int nPermissionFlags;		// Original deny/permit flag
};

class CUOLContactList : 
	public CWindowImpl<CUOLContactList, CListBox>,
	public COwnerDraw<CUOLContactList>
{
public:
	enum
	{
		WM_SET_IS_UPDATING = WM_USER + 0x4432,
		WM_GET_IS_UPDATING
	};

private:

	DECLARE_WND_SUPERCLASS(NULL, CListBox::GetWndClassName())

	BEGIN_MSG_MAP(CUOLContactList)
		MESSAGE_HANDLER(WM_SET_IS_UPDATING, OnSetIsUpdating)
		MESSAGE_HANDLER(WM_GET_IS_UPDATING, OnGetIsUpdating)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CUOLContactList>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	CUOLContactList& operator = (HWND hWnd)
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

	CUOLContactList()
	{
		m_pImagesProtocol = CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol();
		m_bIsUpdating = FALSE;
	}

	void DrawItem(LPDRAWITEMSTRUCT lpdis)
	{
		BOOL bIsUpdating = (BOOL)::SendMessage(m_hWnd, WM_GET_IS_UPDATING, 0, 0);
		
		if (bIsUpdating)
		{
			ATLTRACE("DrawItem() -> Updating... get out...\n");
			
			return;
		}
		
		if(!lpdis)
		{
			return;
		}

		ContactData* pCD = (ContactData*) lpdis->itemData;
		if (::IsBadReadPtr(pCD, sizeof(pCD)))
		{
			ATLTRACE(_T("%s - Invalid ContactData\n"), __FUNCTION__);
			return;
		}

		IUOLMessengerContactPtr pContact = pCD->pContact;

		if (!pContact)
		{
			ATLTRACE(_T("%s - Invalid contact\n"), __FUNCTION__);
			return;
		}

		CString text;
		//try
		{
			text = pContact->GetAlias();

			ATLTRACE(_T("%s - [0x%x] Drawing [%s]\n  itemAction [%d]\n  itemState [0x%x]\n  rcItem [(%d,%d) x (%d,%d)]\n"), 
					__FUNCTION__, m_hWnd, text, lpdis->itemAction, lpdis->itemState,
					lpdis->rcItem.left, lpdis->rcItem.right, lpdis->rcItem.top, lpdis->rcItem.bottom);
		}
		//catch (...) {
		//	ATLTRACE("Pointer Error\n");
		//	return;
		//}

		CDCHandle dc = lpdis->hDC;
		CDC dcMem;

		dcMem.CreateCompatibleDC ( dc );
		dc.SaveDC();
		dcMem.SaveDC();

		COLORREF txtColor;

		txtColor = RGB(0,0,0);
		CRect lineRect (lpdis->rcItem);
		CRect picRect (lineRect);
		picRect.right = picRect.left+16;

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

		int OldBkMode = dc.SetBkMode(OPAQUE);
		COLORREF OldBkColor = dc.SetBkColor(bkColor);
		COLORREF OldTextColor = dc.SetTextColor(txtColor);

		if ( lpdis->itemState & ODS_SELECTED ) 
			dc.BitBlt ( 1, 1, 80, 80, dcMem, 0, 0, SRCCOPY );
		else
			dc.BitBlt ( 0, 0, 80, 80, dcMem, 0, 0, SRCCOPY );
		CRect  tmpRect(lineRect);

		tmpRect.left+=20;

		dc.DrawText(text,text.GetLength(),tmpRect,DT_LEFT|DT_END_ELLIPSIS|DT_VCENTER);

		IUOLMessengerImagePtr m_pImage;
		//try
		{
			m_pImage = m_pImagesProtocol->GetUserStatusImage(pContact->GetAccount()->GetProtocolId(), IUOLMessengerAccount::AS_ONLINE);
		}
		//catch (...) {
		//	ATLTRACE("Pointer Error\n");
		//	return;
		//}

		if (m_pImage.get()) 				
		{
			int nPicLength = min(picRect.Width(), picRect.Height());

			IUOLMessengerImageDrawStrategyPtr pStrategy =
				new UOLMessengerImage::CDrawStretchTransparentStrategy();

			m_pImage->Draw(dc, picRect.left, 
				picRect.top,
				nPicLength,
				nPicLength,
				pStrategy);	
		} 
		else
		{
			ATLTRACE(_T("CHistoryLogsView::DrawItem WARNING !NOT FOUND PROTOCOL!\n"));	
		}

		dc.SetBkMode(OldBkMode);
		dc.SetBkColor(OldBkColor);
		dc.SetTextColor(OldTextColor);

		dcMem.RestoreDC(-1);
		dc.RestoreDC(-1);
	}

	//int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct) 
	//{
	//	ATLASSERT(lpCompareItemStruct->CtlType == ODT_LISTBOX);
	//	LPCTSTR lpszText1 = (LPCTSTR) lpCompareItemStruct->itemData1;
	//	ATLASSERT(lpszText1 != NULL);
	//	LPCTSTR lpszText2 = (LPCTSTR) lpCompareItemStruct->itemData2;
	//	ATLASSERT(lpszText2 != NULL);

	//	return strcmp(lpszText1, lpszText2);
	//}

	LRESULT OnSetIsUpdating(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled)
	{
		m_bIsUpdating = (BOOL)wParam;
		
		return 0L;
	}

	LRESULT OnGetIsUpdating(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bHandled)
	{
		return m_bIsUpdating;
	}

private:
	IUOLMessengerImagesProtocolPtr m_pImagesProtocol;

	// Necessary for race condition during DrawItem method.
	BOOL m_bIsUpdating;
};




class CUOLContactCombo : 
	public CWindowImpl<CUOLContactCombo, CComboBox>,
	public COwnerDraw<CUOLContactCombo>
{
	DECLARE_WND_SUPERCLASS(NULL, CComboBox::GetWndClassName())

	BEGIN_MSG_MAP(CUOLContactCombo)

		CHAIN_MSG_MAP_ALT(COwnerDraw<CUOLContactCombo>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	CUOLContactCombo & operator = (HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}	

	CUOLContactCombo()
	{
		m_pImagesProtocol = CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol();
	}

	struct AccountHelper
	{
		IUOLMessengerAccountPtr	pAccount;
	};

	void DrawItem(LPDRAWITEMSTRUCT lpdis)
	{
		if(!lpdis)
		{
			return;
		}

		AccountData* pAD = (AccountData*) lpdis->itemData;
		
		if(IsBadReadPtr(pAD))
		{
			return;
		}

		CDCHandle dc = lpdis->hDC;
		CDC dcMem;

		dcMem.CreateCompatibleDC ( dc );
		dc.SaveDC();
		dcMem.SaveDC();

		COLORREF txtColor;
		CString text = "camp",str;

		int n = GetLBTextLen( lpdis->itemID );
		GetLBText( lpdis->itemID , str.GetBuffer(n) );
		text = str;
		str.ReleaseBuffer();

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

		IUOLMessengerImagePtr m_pImage =
			m_pImagesProtocol->GetUserStatusImage(pAD->pAccount->GetProtocolId(), pAD->pAccount->AS_ONLINE);

		if (m_pImage.get()) 				
		{
			int nPicLength = min(picRect.Width(), picRect.Height());

			IUOLMessengerImageDrawStrategyPtr pStrategy = 
				new UOLMessengerImage::CDrawStretchTransparentStrategy();

			m_pImage->Draw(dc, picRect.left, 
				picRect.top,
				nPicLength,
				nPicLength,
				pStrategy);	
		} 
		else
		{
			ATLTRACE(_T("CHistoryLogsView::DrawItem WARNING !NOT FOUND PROTOCOL!\n"));	
		}
		dcMem.RestoreDC(-1);
		dc.RestoreDC(-1);
	}
private:
	IUOLMessengerImagesProtocolPtr m_pImagesProtocol;
};







class CPrivacyDialog : public CMainDialog,
					   public CUOLMessengerPrivacyManagerObserver
{
public:
	CPrivacyDialog();
	~CPrivacyDialog();

protected:
	BEGIN_MSG_MAP(CPrivacyDialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(IDC_COMBO_ACCOUNT, CBN_SELCHANGE, OnSelChangeComboAccount)
		COMMAND_HANDLER_EX(IDC_LIST_PERMITED, LBN_SELCHANGE, OnSelChangeListBoxPermited)
		COMMAND_HANDLER_EX(IDC_LIST_DENIED, LBN_SELCHANGE, OnSelChangeListBoxDenied)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_PERMIT, OnClickedButtonPermit)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_DENY, OnClickedButtonDeny)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_INFO, OnClickedButtonInfo)
		CHAIN_MSG_MAP(CMainDialog)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	
	void OnClose();
	void OnDestroy();

	void OnSelChangeComboAccount(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnSelChangeListBoxPermited(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnSelChangeListBoxDenied(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnClickedButtonPermit(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnClickedButtonDeny(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnClickedButtonInfo(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);

	void OnPermitAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void OnDenyAdded(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void OnPermitRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact);
	void OnDenyRemoved(IUOLMessengerAccountPtr pAccount, const CString& strContact);

private:
	void FillAccountList();
	void ClearAccountList();
	void OnSelChangeComboAccountImpl();

	typedef CAtlMap<CString, IUOLMessengerContactPtr> CContactMap;

	void GetDeniedList(IUOLMessengerAccountPtr pAccount, CContactMap& mapContacts);
	void GetPermitedList(IUOLMessengerAccountPtr pAccount, CContactMap& mapContacts);

	void FillPermissionList(CListBox wndList, CContactMap& mapContacts, int nPermissionFlags);
	void ClearPermissionList(CListBox wndList);

	void TogglePermission(CListBox wndFromList, int nPermissionFlags);
	void UpdatePermission(const CString& strContact, CListBox wndFromList, CListBox wndToList, CContactMap& m_mapFrom, CContactMap& m_mapTo);

private:
	CUOLContactCombo		m_wndComboAccounts;
	CUOLContactList		m_wndListPermited;
	CUOLContactList		m_wndListDenied;
	CButton			m_wndButtonPermit;
	CButton			m_wndButtonDeny;
	CButton			m_wndButtonInfo;

	IUOLMessengerAccountPtr m_pCurrentAccount;

	enum PermissionFlags
	{
		PF_PERMITED = 0,
		PF_DENIED = 1,
		//MP_MODIFIED = 2
	};

	CContactMap		m_mapDeniedContacts;
	CContactMap		m_mapPermitedContacts;

	CTransparentStatic	m_labelAccountTransparencyApplier;
	CTransparentStatic	m_labelAllowTransparencyApplier;
	CTransparentStatic	m_labelBlockTransparencyApplier;
};
MAKEAUTOPTR(CPrivacyDialog);