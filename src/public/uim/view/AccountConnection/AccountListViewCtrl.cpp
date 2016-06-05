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

#include "stdafx.h"
#include "AccountListViewCtrl.h"

#include "../../resource.h"
#include "../Image/Image.h"


struct LISTENTRY
{
	IAccountListViewEntryPtr pEntry;
};



CAccountListViewCtrl::CAccountListViewCtrl()
{
}


CAccountListViewCtrl::~CAccountListViewCtrl()
{
}


int CAccountListViewCtrl::AddEntry(IAccountListViewEntryPtr pEntry)
{
	int nIndex = -1;

	if (pEntry)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csEntries);

		nIndex = InsertItem(0, pEntry->GetAccountText());

		if (-1 != nIndex)
		{
			LISTENTRY* pListEntry = new LISTENTRY;
			pListEntry->pEntry = pEntry;
			
			SetItemText(nIndex, 1, pEntry->GetTimestamp());			
			SetItemData(nIndex, (DWORD_PTR) pListEntry);
		}
	}

	return nIndex;
}


IAccountListViewEntryPtr CAccountListViewCtrl::RemoveEntry(int nIndex)
{
	IAccountListViewEntryPtr pEntry;

	if (nIndex >= 0)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csEntries);

		DWORD_PTR pData = GetItemData(nIndex);

		if (pData != LB_ERR)
		{
			LISTENTRY* pListEntry = (LISTENTRY*) pData;

			pEntry = pListEntry->pEntry;

			_CAccountListViewCtrlBase::DeleteItem(nIndex);

			delete pListEntry;
		}
	}

	return pEntry;
}


IAccountListViewEntryPtr CAccountListViewCtrl::GetEntry(int nIndex)
{
	IAccountListViewEntryPtr pEntry;

	if (nIndex >= 0)
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csEntries);

		DWORD_PTR pData = GetItemData(nIndex);

		if (pData != LB_ERR)
		{
			LISTENTRY* pListEntry = (LISTENTRY*) pData;

			pEntry = pListEntry->pEntry;
		}
	}

	return pEntry;

}


void CAccountListViewCtrl::SetNormalBackground1(CBackgroundElementPtr pBackgroundElement)
{
	m_pBackgroundNormal1 = pBackgroundElement;
}


void CAccountListViewCtrl::SetNormalBackground2(CBackgroundElementPtr pBackgroundElement)
{
	m_pBackgroundNormal2 = pBackgroundElement;
}


void CAccountListViewCtrl::SetSelectedBackground(CBackgroundElementPtr pBackgroundElement)
{
	m_pBackgroundSelected = pBackgroundElement;
}


void CAccountListViewCtrl::SetFocusBackground(CBackgroundElementPtr pBackgroundElement)
{
	m_pBackgroundFocus = pBackgroundElement;
}


void CAccountListViewCtrl::SetNormalFont(CFontElementPtr pFontElement)
{
	m_pFontNormal = pFontElement;
}


void CAccountListViewCtrl::SetSelectedFont(CFontElementPtr pFontElement)
{
	m_pFontSelected = pFontElement;
}


LRESULT CAccountListViewCtrl::OnCreate(LPCREATESTRUCT lpCs)
{
	SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

	struct ColumnInfo
	{
		DWORD dwResourceId;
		UINT nWidth;
	};

	ColumnInfo vColumns[] = 
	{
		{ IDS_CONNECTION_ERROR_DIALOG_ACCOUNT_COLUMN, 250 },
		{ IDS_CONNECTION_ERROR_DIALOG_DATE_COLUMN, 150 }
	};
	
	for (int nInd = 0; nInd < sizeof(vColumns) / sizeof(vColumns[0]); nInd++)
	{
		CString strColumn;
		strColumn.LoadString(vColumns[nInd].dwResourceId);

		InsertColumn(nInd, strColumn, LVCFMT_LEFT, vColumns[nInd].nWidth, -1);
	}

	SetMsgHandled(FALSE);

	return 0L;
}


void CAccountListViewCtrl::OnDestroy()
{
	RemoveAllEntries();

	SetMsgHandled(FALSE);
}


void CAccountListViewCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (ODT_LISTVIEW == lpMeasureItemStruct->CtlType)
	{
		lpMeasureItemStruct->itemHeight = 20;
	}
}


void CAccountListViewCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (ODT_LISTVIEW == lpDrawItemStruct->CtlType)
	{
		// Select background and font
		CBackgroundElementPtr pBackgroundElement;
		CFontElementPtr pFontElement = m_pFontNormal;

		if (lpDrawItemStruct->itemState & ODS_SELECTED)
		{
			pBackgroundElement = m_pBackgroundSelected;
			pFontElement = m_pFontSelected;
		}
		else if (lpDrawItemStruct->itemState & ODS_FOCUS)
		{
			pBackgroundElement = m_pBackgroundFocus;
		}
		else
		{
			if (lpDrawItemStruct->itemID % 2 == 0)
			{
				pBackgroundElement = m_pBackgroundNormal1;
			}
			else
			{
				pBackgroundElement = m_pBackgroundNormal2;
			}
		}

		ATLASSERT(pBackgroundElement.get());
		ATLASSERT(pFontElement.get());

		CDCHandle dc(lpDrawItemStruct->hDC);
		CRect rectItem = lpDrawItemStruct->rcItem;

		// Draw background
		pBackgroundElement->Draw(dc, rectItem);

		LISTENTRY* pListEntry = (LISTENTRY*) lpDrawItemStruct->itemData;

		if (pListEntry)
		{
			CHeaderCtrl headerCtrl = GetHeader();

			int nColumns = headerCtrl.GetItemCount();

			for (int nColumnInd = 0; nColumnInd < nColumns; nColumnInd++)
			{
				CRect rectCell;
				GetCellRect(nColumnInd, rectItem, rectCell);

				TCHAR pszItemText[1024 + 1] = {0};
				GetItemText(lpDrawItemStruct->itemID, nColumnInd, pszItemText, 1024);
				
				if (0 == nColumnInd)
				{
					// Draw protocol icon and text
					IUOLMessengerImagePtr pImage = (pListEntry->pEntry)->GetIcon();

					if (pImage.get())
					{
						IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawStretchTransparentStrategy();

						CRect rectImage;
						rectImage.left = rectCell.left + 2;
						rectImage.top = rectCell.top + 2;
						rectImage.bottom = rectCell.bottom - 2;
						rectImage.right = rectCell.left + rectImage.Height();

						pImage->Draw(dc, rectImage.left, rectImage.top,
								rectImage.Width(), rectImage.Height(),
								pStrategy);

						rectCell.left = rectImage.right;
					}
				}

				// Draw text
				rectCell.left += 2;
				pFontElement->DrawText(dc, rectCell, pszItemText);
			}
		}
	}
}


void CAccountListViewCtrl::GetCellRect(int nColumn, const CRect& rectItem, CRect& rectCell)
{
	CHeaderCtrl header = GetHeader();
	CRect rectHeader;
	header.GetItemRect(nColumn, &rectHeader);

	int xOffset = -GetScrollPos(SB_HORZ);

	rectCell.left = xOffset + rectHeader.left;
	rectCell.right = xOffset + rectHeader.right;
	rectCell.top = rectItem.top;
	rectCell.bottom = rectItem.bottom;
}


void CAccountListViewCtrl::RemoveAllEntries()
{
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_csEntries);

		int nIemCount = GetItemCount();

		LISTENTRY* pListEntry;
		DWORD_PTR pData;

		for (int nInd = 0; nInd < nIemCount; nInd++)
		{
			pData = GetItemData(nInd);

			if (pData != LB_ERR)
			{
				pListEntry = (LISTENTRY*) pData;

				delete pListEntry;
			}
		}
	}

	DeleteAllItems();
}