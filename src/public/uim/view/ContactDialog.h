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
#include "MainDialog.h"
#include "TransparentStatic.h"

#include <interfaces/IUOLMessengerAccount.h>
#include <interfaces/IUOLMessengerContact.h>

#include "image/image.h"
#include "image/imagebuilder.h"

#include "interfaces/IUOLMessengerImagesProtocol.h"
#include "../controller/UIMApplication.h"

class CUOLAccountBox : 
	public CWindowImpl<CUOLAccountBox, CComboBox>,
	public COwnerDraw<CUOLAccountBox>
{

	BEGIN_MSG_MAP(CUOLAccountBox)

		CHAIN_MSG_MAP_ALT(COwnerDraw<CUOLAccountBox>, 1)
	END_MSG_MAP()

	CUOLAccountBox & operator = (HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}	

	CUOLAccountBox()
	{
		m_pImagesProtocol = CUIMApplication::GetApplication()->GetUIManager()->GetImagesProtocol();
	}

	struct AccountHelper
	{
		IUOLMessengerAccountPtr	pAccount;
	};

	void DrawItem(LPDRAWITEMSTRUCT lpdis)
	{
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

		AccountHelper* pH;
		CIMProtocol *pProtocol;
		pH = (AccountHelper*) lpdis->itemData;
		pProtocol = pH->pAccount->GetProtocol();

		IUOLMessengerImagePtr m_pImage =
			m_pImagesProtocol->GetUserStatusImage(pH->pAccount->GetProtocolId(), pH->pAccount->AS_ONLINE);

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
		// must be implemented
		//ATLASSERT(FALSE);

	}
private:
	IUOLMessengerImagesProtocolPtr m_pImagesProtocol;
};



class CContactDialog :
	public CMainDialog,
	public CWinDataExchange<CContactDialog>
{
public:
	CContactDialog();
	virtual ~CContactDialog(void);

	void Initialize(const CString& strContact = _T(""), IUOLMessengerAccountPtr pAccount = IUOLMessengerAccountPtr());

	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	
protected:
	BEGIN_MSG_MAP(CContactDialog)	
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(IDOK, OnBnClickedButtonOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnBnClickedButtonCancel)
		CHAIN_MSG_MAP(CMainDialog)
		REFLECT_NOTIFICATIONS();
	END_MSG_MAP()

	BEGIN_DDX_MAP(CContactDialog)
		DDX_TEXT(IDC_EDIT_USER, m_strContact)
		DDX_CONTROL_HANDLE(IDC_COMBO_GROUP, m_wndComboGroups)
		//DDX_CONTROL_HANDLE(IDC_COMBO_ACCOUNT, m_wndComboAccounts)
	END_DDX_MAP()

	virtual LRESULT OnInitDialog(HWND hwnd, LPARAM lParam);
	virtual LRESULT OnDestroy();

	void OnBnClickedButtonOK(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);
	void OnBnClickedButtonCancel(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/);

private:
	void FillGroupList();
	void FillAccountList();
	BOOL AddContact();

	struct AccountHelper
	{
		IUOLMessengerAccountPtr	pAccount;
	};

private:
	CAtlList<IUOLMessengerAccountPtr>	m_listAccounts;
	CAtlList<IUOLMessengerGroupPtr>		m_listGroups;

	IUOLMessengerAccountPtr	m_pAccount;
	CString					m_strContact;
	CComboBox				m_wndComboGroups;
//	CComboBoxEx				m_wndComboAccounts;
	CUOLAccountBox			m_wndComboAccounts;

	CTransparentStatic		m_userTransparencyApplier;
	CTransparentStatic		m_accountTransparencyApplier;
	CTransparentStatic		m_groupTransparencyApplier;
};

MAKEAUTOPTR(CContactDialog);
