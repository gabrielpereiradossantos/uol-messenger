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
#include "FileTransferNode.h"
#include "../../resource.h"
#include "../image/Image.h"
#include "../GroupListCtrl/GroupListCtrl.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"
#include "../../controller/HistoryRecordFileTransfer.h"
#include "../../controller/FileTransfer/FileTransferManager.h"


#define	LEFT_SHIFT_SPACE			10
#define	RIGHT_SHIFT_SPACE			20 // Normal 10px shift + 10px for the scrollbar.
#define	TOP_SHIFT_SPACE				5
//#define BUTTON_SPACE				80
#define	ICON_WIDTH					32
#define	ICON_HEIGHT					32
#define	COLUMN_SHIFT_SPACE			3
#define	COLUMN_ACTIONS_SHIFT_SPACE	5
#define	LINE_SHIFT_SPACE			10
#define PROGRESSBAR_HEIGHT			12


using namespace GroupListCtrl;


CFileTransferNode::CFileTransferNode(IUOLMessengerFileTransferPtr pFileTransfer, 
		CProgressBarElementPtr pProgressBarElement) :
	m_pDefaultPainter(NULL),
	m_pCurrentPainter(NULL),
	m_pSelectionPainter(NULL),
	m_clrTextNormal(RGB(0, 0, 0)),
	m_clrTextSelected(RGB(0, 0, 0)), 
	m_clrTextActions(RGB(0, 0, 0)), 
	m_hItemSeparatorBrush(NULL), 
	m_hItemSelectionBkgrnd(NULL), 
	m_bIsShowOpenAction(FALSE), 
	m_bIsShowCancelAction(FALSE), 
	m_bIsShowOnlyRemoveFromList(FALSE), 
	m_rectOpenAction(0, 0, 0, 0), 
	m_rectRemoveFromListAction(0, 0, 0, 0), 
	m_rectCancelAction(0, 0, 0, 0), 
	m_bIsShowingLinkCursor(FALSE), 
	m_hWaitCursor(NULL), 
	m_hDefaultCursor(NULL), 
	m_bIsVisible(TRUE)
{
	m_pFileTransfer = ap_dynamic_cast<IUOLMessengerFileTransfer2Ptr>(pFileTransfer);
	// Create ProgressBar.
	m_pProgressBar = new CUOLProgressBar(pProgressBarElement);
	ATLASSERT(m_pProgressBar);
	
	m_pProgressBar->SetRoundedCorners(5, 5);
	m_pProgressBar->SetRange(0, m_pFileTransfer->GetTotalBytes());

	m_pFileTransfer->RegisterProgressObserver(this);

	//Create FileTypeIcon
	m_pFileTypeIcon = new CFileTypeIcon();
	m_pFileTypeIcon->SetFileName(m_pFileTransfer->GetFileName());

	// Load the action strings.
	//
	m_strOpenAction = "Abrir";
	m_strRemoveFromListAction = "Apagar da lista";
	m_strCancelAction = "Cancelar";
	
	UOLLib::ISettings* pStringLoader =  
			static_cast<CUIMApplication*>(
					CUIMApplication::GetApplication())->GetStringLoader();
	
	if (pStringLoader)
	{
		m_strOpenAction = pStringLoader->GetValue(
				"uol.resource.IDS_FILE_TRANSFER_DLG_OPEN");
		m_strRemoveFromListAction = pStringLoader->GetValue(
				"uol.resource.IDS_FILE_TRANSFER_DLG_REMOVE_FROM_LIST");
		m_strCancelAction = pStringLoader->GetValue(
				"uol.resource.IDS_FILE_TRANSFER_DLG_CANCEL");
	}
	
	//HINSTANCE hInstance = ATL::_AtlBaseModule.GetResourceInstance();
	//m_hWaitCursor = ::LoadCursor(hInstance,MAKEINTRESOURCE(IDC_CUSTOM_HAND));
	m_hWaitCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_HAND));
	
	FileTransferStatus status = pFileTransfer->GetStatus();
	
	if (status == FILE_TRANSFER_STATUS_NOT_STARTED)
	{
		m_bIsShowCancelAction = TRUE;
	}
	else if (status == FILE_TRANSFER_STATUS_DONE)
	{
		m_bIsShowOpenAction = TRUE;
	}
	else if ((status == FILE_TRANSFER_STATUS_CANCEL_REMOTE) || 
			 (status == FILE_TRANSFER_STATUS_CANCEL_LOCAL))
	{
		m_bIsShowOnlyRemoveFromList = TRUE;
	}
}


CFileTransferNode::~CFileTransferNode()
{
	m_pFileTransfer->UnregisterProgressObserver(this);
	
	if (m_hItemSeparatorBrush)
	{
		::DeleteObject(m_hItemSeparatorBrush);
	}
	
	::SetCursor(m_hDefaultCursor);
}


IUOLMessengerFileTransferPtr CFileTransferNode::GetFileTransfer() const
{
	return m_pFileTransfer;
}


CString CFileTransferNode::GetFileTransferProgressText()
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	CString strFormat;
	CString strProgress;

	FileTransferStatus status = m_pFileTransfer->GetStatus();
	if (status == FILE_TRANSFER_STATUS_DONE)
	{
		strProgress = pStringLoader->GetValue("uol.resource.IDS_FILE_TRANSFER_DLG_DONE");
		return strProgress;
	}
	else if ((status == FILE_TRANSFER_STATUS_CANCEL_REMOTE) || 
			 (status == FILE_TRANSFER_STATUS_CANCEL_LOCAL))
	{
		strProgress = pStringLoader->GetValue("uol.resource.IDS_FILE_TRANSFER_DLG_CANCELED");
		return strProgress;
	}
	else if ((status == FILE_TRANSFER_STATUS_ACCEPTED) || 
			 (status == FILE_TRANSFER_STATUS_NOT_STARTED))
	{
		strProgress = pStringLoader->GetValue("uol.resource.IDS_FILE_TRANSFER_DLG_WAITING");
		return strProgress;
	}

	ULONG nTransferedBytes = m_pFileTransfer->GetTransferedBytes();
	ULONG nTotalBytes = m_pFileTransfer->GetTotalBytes();
	ULONG nTransferedKBytes = (ULONG) (double) ceil(nTransferedBytes / 1024.0);
	ULONG nTotalKBytes = (ULONG) (double) ceil(nTotalBytes / 1024.0);

	double dPercent = (nTotalBytes > 0) ? (100.0 * ((double) nTransferedBytes / nTotalBytes)) : 0.0;

	strFormat = pStringLoader->GetValue("uol.resource.IDS_FILE_TRANSFER_DLG_FILE_FORMAT");
	strProgress.Format(strFormat, nTransferedKBytes, nTotalKBytes, dPercent);

	return strProgress;
}


CString CFileTransferNode::GetFileTransferOperationText()
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
    CString strFormat;

	CString strOperation;
	DWORD dwOperation = m_pFileTransfer->GetTransferOperation();

	if (dwOperation == IUOLMessengerFileTransfer::FTO_SEND)
	{
		strFormat = pStringLoader->GetValue("uol.resource.IDS_FILE_TRANSFER_DLG_CONTACT_SEND_FORMAT");
		
	}
	else if (dwOperation == IUOLMessengerFileTransfer::FTO_RECEIVE)
	{
		strFormat = pStringLoader->GetValue("uol.resource.IDS_FILE_TRANSFER_DLG_CONTACT_RECEIVE_FORMAT");
	}

	strOperation.Format(strFormat, m_pFileTransfer->GetContactName());

	return strOperation;
}


CString CFileTransferNode::GetFileTransferInternalName() const
{
	CString strName;
	strName.Format("%s:%s:%d", m_pFileTransfer->GetContactName(), 
		m_pFileTransfer->GetFileName(), m_pFileTransfer->GetId());

	return strName.Trim();
}


void CFileTransferNode::RegisterObserver(CUOLMessengerFileTransferObserver* pObserver)
{
}


void CFileTransferNode::UnregisterObserver(CUOLMessengerFileTransferObserver* pObserver)
{
}


void CFileTransferNode::SetDefaultPainter(IPainter* pPainter)
{
	m_pDefaultPainter = pPainter;

	if (m_pCurrentPainter == NULL)
	{
		m_pCurrentPainter = pPainter;
	}
}


IPainter* CFileTransferNode::GetDefaultPainter(IPainter* pPainter) const
{
	return m_pDefaultPainter;
}


void CFileTransferNode::SetPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pCurrentPainter = pPainter;

		RestorePainter(pPainter->GetTimeout());
	}
}


IPainter* CFileTransferNode::GetPainter() const
{
	return m_pCurrentPainter;
}


void CFileTransferNode::SetSelectionPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pSelectionPainter = pPainter;
	}
}


IPainter* CFileTransferNode::GetSelectionPainter() const
{
	return m_pSelectionPainter;
}


void CFileTransferNode::SetTextFont(CFontElementPtr  pFont)
{
	m_pFontTextNormal = pFont;
}


void CFileTransferNode::SetSelectedTextFont(CFontElementPtr  pFont)
{
	m_pFontTextSelected = pFont;
}


void CFileTransferNode::SetTitleTextFont(CFontElementPtr  pFont)
{
	m_pFontTextTitle = pFont;
}


void CFileTransferNode::SetTextColor(COLORREF clrText)
{
	m_clrTextNormal = clrText;
}


void CFileTransferNode::SetSelectedTextColor(COLORREF clrText)
{
	m_clrTextSelected = clrText;
}


void CFileTransferNode::SetTitleTextColor(COLORREF clrText)
{
	m_clrTextTitle = clrText;
}


void CFileTransferNode::SetItemSelectionBkgrndImage(HBITMAP hItemSelectionBkgrndImage)
{
	m_hItemSelectionBkgrnd = hItemSelectionBkgrndImage;
}


void CFileTransferNode::SetActionsTextFont(CFontElementPtr  pFont)
{
	m_pFontTextActions = pFont;
}


void CFileTransferNode::SetActionsTextColor(COLORREF clrText)
{
	m_clrTextActions = clrText;
}


void CFileTransferNode::SetItemSeparatorColor(COLORREF clrBkgrnd)
{
	if (m_hItemSeparatorBrush)
	{
		::DeleteObject(m_hItemSeparatorBrush);
	}
	
	m_hItemSeparatorBrush = ::CreateSolidBrush(clrBkgrnd);
}


void CFileTransferNode::SetName(const CString& strName)
{
	m_strName = strName;
}


CString CFileTransferNode::GetName() const
{
	return m_strName;
}


BOOL CFileTransferNode::IsVisible() const
{
	return m_bIsVisible;
}


void CFileTransferNode::Paint(CDCHandle& dc)
{
	DrawIcon(dc);
	DrawTitle(dc);
	DrawProgress(dc);
	//DrawButton(dc);
	DrawActions(dc);
	DrawText(dc);
}


void CFileTransferNode::DrawFocusRect(CDCHandle& dc)
{
	if (NULL == m_pSelectionPainter)
	{
		CGroupListCtrlItem::DrawFocusRect(dc);
	}
}


void CFileTransferNode::DrawBackground(CDCHandle& dc)
{
	IPainter* pPainter = m_pCurrentPainter;
	CRect rect(CPoint(0, 0), GetSize());
	
	if (GetSelected())
	{
		CUOLProgressBar::DrawImageToCDC(dc, 
				rect.left, rect.top, rect.Width(), 0, m_hItemSelectionBkgrnd);
	}
	else if (pPainter != NULL)
	{
		CMemDC dcMem(dc, &rect);
		
		pPainter->Draw(CDCHandle(dcMem), rect);
	}
	
	// Draw item separator line.
	rect.top = rect.bottom - 2;
	::FillRect(dc.m_hDC, rect, m_hItemSeparatorBrush);
}


void CFileTransferNode::OnLBtnDblClick(const CPoint& pt)
{
}


void CFileTransferNode::OnLButtonDown(const CPoint& pt)
{
	CPoint scrollOffset;
	m_pGroupListCtrl->GetScrollOffset(scrollOffset);
	
	CPoint ptWithScroll = pt;
	ptWithScroll.Offset(scrollOffset.x, scrollOffset.y);
}


void CFileTransferNode::OnLButtonUp(const CPoint& pt)
{
	CPoint scrollOffset;
	m_pGroupListCtrl->GetScrollOffset(scrollOffset);
	
	CPoint ptWithScroll = pt;
	ptWithScroll.Offset(scrollOffset.x, scrollOffset.y);
	
	ExecuteAction(GetActionStringUnderCursor(ptWithScroll));
}


void CFileTransferNode::OnRButtonDown(const CPoint& pt)
{
}


void CFileTransferNode::OnRBtnDblClick(const CPoint& pt)
{
}


void CFileTransferNode::OnReturnKeyPressed()
{
}


void CFileTransferNode::OnKeyDown(TCHAR ch)
{
}


void CFileTransferNode::OnMouseHover(const CPoint& pt)
{
}


void CFileTransferNode::OnMouseLeave()
{
}


void CFileTransferNode::OnMouseMove(const CPoint& pt)
{
	CPoint scrollOffset;
	m_pGroupListCtrl->GetScrollOffset(scrollOffset);
	
	CPoint ptWithScroll = pt;
	ptWithScroll.Offset(scrollOffset.x, scrollOffset.y);
	
	{
		CComCritSecLock<CComAutoCriticalSection> lock(m_criticalSection);
		
		UpdateNodeCursor(GetActionStringUnderCursor(ptWithScroll) != 0);
	}
}


void CFileTransferNode::RestorePainter(int nTimeout)
{
	time_t curtime;
	time(&curtime);
	m_timeRestore = curtime + nTimeout;
}


void CFileTransferNode::OnTimer(UINT nId)
{
	if (CGroupListCtrl::SYSTEM_TIMER == nId)
	{
		time_t curtime;
		time(&curtime);

		if (m_timeRestore != 0 && curtime >= m_timeRestore)
		{
			m_timeRestore = 0;

			m_pCurrentPainter = m_pDefaultPainter;

			if (m_pCurrentPainter != NULL)
			{
				Invalidate();
			}
		}
	}
}


void CFileTransferNode::DrawIcon(CDCHandle& dc)
{
	CSize size = GetSize();
	int nTop = ((size.cy - ICON_HEIGHT) / 2);
	CRect rectIcon(CPoint(LEFT_SHIFT_SPACE, nTop), CSize(ICON_WIDTH, ICON_HEIGHT));
	
	m_rectIcon = rectIcon;
	
	m_pFileTypeIcon->Draw(dc, rectIcon);
}


void CFileTransferNode::DrawTitle(CDCHandle& dc)
{	
	CSize sizeTitle;
	CSize sizeOperation;
	
	// Draw title line
	CSize size = GetSize();
	int x = m_rectIcon.right + LEFT_SHIFT_SPACE;
	int y = TOP_SHIFT_SPACE;
	int cx = size.cx;
	int cy = size.cy - RIGHT_SHIFT_SPACE;
	CRect rectTitle(CPoint(x, y), CSize(cx, cy));
	m_rectTitle = rectTitle;
	
	CString strTitle;
	
	if (m_pFileTransfer->GetTransferOperation() == 
			IUOLMessengerFileTransfer::FTO_SEND)
	{
		strTitle = m_pFileTransfer->GetFileName();
	}
	else
	{
		CPath pathBreaker(m_pFileTransfer->GetLocalFilePath());
		pathBreaker.StripPath();
		
		strTitle = pathBreaker.m_strPath;
	}
	
	m_pFontTextTitle->DrawColoredText(dc, rectTitle, strTitle, m_clrTextTitle, 
			DT_LEFT | DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS | DT_NOPREFIX);
	m_pFontTextTitle->GetTextWidth(dc, strTitle, sizeTitle);


	// Select font according to selected status
	CFontElementPtr pFont;
	COLORREF clrText;

	if (FALSE == GetSelected())
	{
		pFont = m_pFontTextNormal;
		clrText = m_clrTextNormal;
	}
	else
	{	
		pFont = m_pFontTextSelected;
		clrText = m_clrTextSelected;
	}

	ATLASSERT(pFont);

	// Draw operation line
	y = rectTitle.top + sizeTitle.cy + COLUMN_SHIFT_SPACE;
	CRect rectOperation(CPoint(x, y), CSize(cx, cy));
		
	CString strOperation = GetFileTransferOperationText();
	pFont->DrawColoredText(dc, rectOperation, strOperation, clrText, 
			DT_LEFT | DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS | DT_NOPREFIX);
	pFont->GetTextWidth(dc, strOperation, sizeOperation);

	m_rectTitle.bottom = y + sizeOperation.cy;
}


void CFileTransferNode::DrawText(CDCHandle& dc)
{	
	// Select font according to selected status
	CFontElementPtr pFont;
	COLORREF clrText;

	if (FALSE == GetSelected())
	{
		pFont = m_pFontTextNormal;
		clrText = m_clrTextNormal;
	}
	else
	{	
		pFont = m_pFontTextSelected;
		clrText = m_clrTextSelected;
	}

	ATLASSERT(pFont);

	// Draw file transfer status line
	CRect rectText(CPoint(0, 0), GetSize());
	
	rectText.left = m_rectIcon.right + LEFT_SHIFT_SPACE;
	rectText.right -= RIGHT_SHIFT_SPACE;
	rectText.top = m_rectProgress.bottom + COLUMN_SHIFT_SPACE;
	
	CString strText = GetFileTransferProgressText();
	pFont->DrawColoredText(dc, rectText, strText, clrText, 
			DT_LEFT | DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS | DT_NOPREFIX);	
}	


void CFileTransferNode::DrawActions(CDCHandle& dc)
{
	CSize nodeSize = GetSize();
	CSize biggerActionStrSize = GetStringWidth(dc, m_strRemoveFromListAction);
	
	CRect rectNode = CGroupListCtrlItem::GetRect();
	
	int iNumberOfShowingActions = 0;
	
	if (m_bIsShowOpenAction)
	{
		iNumberOfShowingActions = 2;
	}
	else if (m_bIsShowCancelAction)
	{
		iNumberOfShowingActions = 1;
	}
	else if (m_bIsShowOnlyRemoveFromList)
	{
		iNumberOfShowingActions = 1;
	}
	
	int iTopShift = (nodeSize.cy -												// Total height.
			(iNumberOfShowingActions * biggerActionStrSize.cy) -				// Minus heights of visible action strings.
			((iNumberOfShowingActions - 1) * COLUMN_ACTIONS_SHIFT_SPACE)) /		// Minus inter action strings space (if any).
			2;																	// Divided by 2 to get the to shift.
	
	CRect drawingActionsRect;
	drawingActionsRect.left = m_rectProgress.right + LINE_SHIFT_SPACE;
	drawingActionsRect.top = iTopShift;
	drawingActionsRect.bottom = drawingActionsRect.top + biggerActionStrSize.cy;
	
	if (m_bIsShowOpenAction)
	{
		// Get the correct drawing rect width.
		CSize openStrSize = GetStringWidth(dc, m_strOpenAction);
		drawingActionsRect.right = drawingActionsRect.left + openStrSize.cx;
		
		m_rectOpenAction = drawingActionsRect;
		m_pFontTextActions->DrawColoredText(dc, m_rectOpenAction, 
				m_strOpenAction, m_clrTextActions, 
				DT_LEFT | DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS | DT_NOPREFIX);	
		
		// Adjust rect to global grouplist position.
		m_rectOpenAction.top += rectNode.top;
		m_rectOpenAction.bottom += rectNode.top;
		m_rectOpenAction.left += rectNode.left;
		m_rectOpenAction.right += rectNode.left;
		
		drawingActionsRect.top += biggerActionStrSize.cy + COLUMN_SHIFT_SPACE;
		drawingActionsRect.bottom = drawingActionsRect.top + biggerActionStrSize.cy;
		
		// Get the correct drawing rect width.
		drawingActionsRect.right = drawingActionsRect.left + biggerActionStrSize.cx;
		
		m_rectRemoveFromListAction = drawingActionsRect;
		m_pFontTextActions->DrawColoredText(dc, m_rectRemoveFromListAction, 
				m_strRemoveFromListAction, m_clrTextActions, 
				DT_LEFT | DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS | DT_NOPREFIX);	
		
		// Adjust rect to global grouplist position.
		m_rectRemoveFromListAction.top += rectNode.top;
		m_rectRemoveFromListAction.bottom += rectNode.top;
		m_rectRemoveFromListAction.left += rectNode.left;
		m_rectRemoveFromListAction.right += rectNode.left;
	}
	else if (m_bIsShowCancelAction)
	{
		// Get the correct drawing rect width.
		CSize cancelStrSize = GetStringWidth(dc, m_strCancelAction);
		drawingActionsRect.right = drawingActionsRect.left + cancelStrSize.cx;
		
		m_rectCancelAction = drawingActionsRect;
		m_pFontTextActions->DrawColoredText(dc, m_rectCancelAction, 
				m_strCancelAction, m_clrTextActions, 
				DT_LEFT | DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS | DT_NOPREFIX);	
		
		// Adjust rect to global grouplist position.
		m_rectCancelAction.top += rectNode.top;
		m_rectCancelAction.bottom += rectNode.top;
		m_rectCancelAction.left += rectNode.left;
		m_rectCancelAction.right += rectNode.left;
	}
	else if (m_bIsShowOnlyRemoveFromList)
	{
		// Get the correct drawing rect width.
		drawingActionsRect.right = drawingActionsRect.left + biggerActionStrSize.cx;
		
		m_rectRemoveFromListAction = drawingActionsRect;
		m_pFontTextActions->DrawColoredText(dc, m_rectRemoveFromListAction, 
				m_strRemoveFromListAction, m_clrTextActions, 
				DT_LEFT | DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS | DT_NOPREFIX);	
		
		// Adjust rect to global grouplist position.
		m_rectRemoveFromListAction.top += rectNode.top;
		m_rectRemoveFromListAction.bottom += rectNode.top;
		m_rectRemoveFromListAction.left += rectNode.left;
		m_rectRemoveFromListAction.right += rectNode.left;
	}
}


void CFileTransferNode::DrawProgress(CDCHandle& dc)
{
	FileTransferStatus status = m_pFileTransfer->GetStatus();
	
	int iActionsWidth = GetStringWidth(dc, m_strRemoveFromListAction).cx;
	
	if ( (status != FILE_TRANSFER_STATUS_DONE) && 
			(status != FILE_TRANSFER_STATUS_CANCEL_REMOTE) && 
			(status != FILE_TRANSFER_STATUS_CANCEL_LOCAL))
	{
		CSize size = GetSize();
		int x = m_rectIcon.right + LEFT_SHIFT_SPACE;
		int y = m_rectTitle.bottom + COLUMN_SHIFT_SPACE;
		int cx = size.cx - m_rectIcon.right - RIGHT_SHIFT_SPACE - 
				iActionsWidth - LINE_SHIFT_SPACE;
		int cy = PROGRESSBAR_HEIGHT;
		CRect rect(CPoint(x, y), CSize(cx, cy));
		m_rectProgress = rect;
		
		if (m_pProgressBar && m_pFileTransfer)
		{
			m_pProgressBar->SetRect(rect);
			m_pProgressBar->SetPos(m_pFileTransfer->GetTransferedBytes());
			m_pProgressBar->Draw(dc);	
		}
	}
	else
	{
		CSize size = GetSize();
		int x = m_rectIcon.right + LEFT_SHIFT_SPACE;
		int y = m_rectTitle.bottom + COLUMN_SHIFT_SPACE;
		int cx = size.cx - m_rectIcon.right - RIGHT_SHIFT_SPACE - 
				iActionsWidth - LINE_SHIFT_SPACE;
		int cy = 0;
		
		m_rectProgress.SetRect(x, y, x + cx, y + cy);
	}
}


void CFileTransferNode::UpdateProgress(IUOLMessengerFileTransfer* pFileTransfer)
{
	Redraw(FALSE);

	DWORD status = pFileTransfer->GetStatus();
	if ((status == FILE_TRANSFER_STATUS_DONE) ||
		(status == FILE_TRANSFER_STATUS_CANCEL_REMOTE) || 
		(status == FILE_TRANSFER_STATUS_CANCEL_LOCAL))
	{
		if (status == FILE_TRANSFER_STATUS_DONE)
		{
			m_bIsShowOpenAction = TRUE;
		}
		else
		{
			m_bIsShowOnlyRemoveFromList = TRUE;
		}
		
		m_bIsShowCancelAction = FALSE;
		
		PlayFileTransferFinishedSound();

		// First we need to get the SmartPointer from the FileTransferManager list 
		// of file transfers. We shouldn't work with the original "this" pointer
		// received here.
		//
		CFileTransferManagerPtr pFileTransferManager = 
				ap_dynamic_cast<CFileTransferManagerPtr>(
						CUIMApplication::GetApplication()->GetFileTransferManager());
		
		IUOLMessengerFileTransferPtr pSmrtFileTransfer;
		
		if (pFileTransferManager)
		{
			pSmrtFileTransfer = 
					pFileTransferManager->GetSmartFromThisPointer(pFileTransfer);
		}
		
		if (pSmrtFileTransfer)
		{
			// Log this file transfer event into history
			CHistoryRecordFileTransferPtr pHistoryRecordFileTransfer;
			pHistoryRecordFileTransfer = new CHistoryRecordFileTransfer(pSmrtFileTransfer);
			ap_static_cast<IUOLMessengerHistoryManager4Ptr>(
					CUIMApplication::GetApplication()->GetHistoryManager())
							->LogEvent(pHistoryRecordFileTransfer);
		}
	}
	else
	{
		m_bIsShowCancelAction = TRUE;
	}
}


BOOL CFileTransferNode::IsPointInButtonRect(const CPoint& pt, CImageButtonPtr pButton)
{
	CRect rectNode = CGroupListCtrlItem::GetRect();
	
	CRect rectButton = pButton->GetButtonRect();
	rectButton.top += rectNode.top;
	rectButton.bottom += rectNode.top;
	rectButton.left += rectNode.left;
	rectButton.right += rectNode.left;

	return rectButton.PtInRect(pt);
}


void CFileTransferNode::PlayFileTransferFinishedSound()
{
	BOOL bEnableSound = CUIMApplication::GetApplication()->GetSettingsManager()->GetGeneralSettings()->IsEnableSound();
	
	if (!bEnableSound)
	{
		// Do not play if mute when away
		return;
	}
	
	IUOLMessengerSettingsManager2Ptr pSettingsManager2 = ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
	if (pSettingsManager2)
	{
		CString strSoundPath;
		
		if (pSettingsManager2->GetSoundSettings()->GetSoundEventFilePath(IUOLMessengerSoundSettings3::FILE_TRANSFER_FINISHED_SOUND_EVENT, strSoundPath, TRUE))
		{
			CUIMApplication::GetApplication()->GetUIManager()->PlaySound(strSoundPath, FALSE);
		}
	}
}


CSize CFileTransferNode::GetStringWidth(CDCHandle& dc, CString strToMeasure)
{
	CSize textSize;
	
	m_pFontTextActions->GetTextWidth(dc, strToMeasure, textSize);
	
	return textSize;
}


UINT CFileTransferNode::GetActionStringUnderCursor(const CPoint& pt)
{
	UINT uActionStringId = 0;
	
	if ( (m_bIsShowOpenAction) && (m_rectOpenAction.PtInRect(pt)) )
	{
		uActionStringId = ACTION_STRING_OPEN;
	}
	else if ( ( (m_bIsShowOpenAction) && (m_rectRemoveFromListAction.PtInRect(pt)) ) || 
			( (m_bIsShowOnlyRemoveFromList) && (m_rectRemoveFromListAction.PtInRect(pt)) ) )
	{
		uActionStringId = ACTION_STRING_REMOVE_FROM_LIST;
	}
	else if ( (m_bIsShowCancelAction) && (m_rectCancelAction.PtInRect(pt)) )
	{
		uActionStringId = ACTION_STRING_CANCEL;
	}
	
	return uActionStringId;
}


void CFileTransferNode::ExecuteAction(UINT uActionStringId)
{
	if (!uActionStringId)
	{
		return;
	}
	
	if (uActionStringId == ACTION_STRING_CANCEL)
    {
        UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
		CString strConfirmMessage;
		CString strFormat;
		strFormat = pStringLoader->GetValue("uol.resource.IDS_FILE_TRANSFER_DLG_CANCEL_CONFIRMATION");
		strConfirmMessage.Format(strFormat, m_pFileTransfer->GetFileName());
		
        UINT ret = CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(static_cast<HWND>(*m_pGroupListCtrl), 
			strConfirmMessage, MB_YESNO | MB_ICONQUESTION);
		
		if (ret == IDNO)
		{
			return;
		}
		
		m_bIsShowCancelAction = FALSE;
		m_bIsShowOnlyRemoveFromList = TRUE;
		
		CUIMApplication::GetApplication()->GetFileTransferManager()->CancelFileTransfer(m_pFileTransfer);
		
		Invalidate();
    }
	else if (uActionStringId == ACTION_STRING_OPEN)
	{
		if (m_pFileTransfer)
		{
			CString strFilePath = m_pFileTransfer->GetLocalFilePath();
			
			SHELLEXECUTEINFO shExecInfo;
			
			shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			shExecInfo.fMask = NULL;
			shExecInfo.hwnd = NULL;
			shExecInfo.lpVerb = NULL;
			shExecInfo.lpFile = (LPCTSTR) strFilePath;
			shExecInfo.lpParameters = NULL;
			shExecInfo.lpDirectory = NULL;
			shExecInfo.nShow = SW_SHOWNORMAL;
			shExecInfo.hInstApp = NULL;
			
			BOOL bExecute = ::ShellExecuteEx(&shExecInfo);
			if (!bExecute)
			{
				DWORD dwError = ::GetLastError();
				ATLTRACE(_T("Error %d: couldn't open file %s\n"), dwError, (LPCTSTR) strFilePath);
			}
		}
	}
	else if (uActionStringId == ACTION_STRING_REMOVE_FROM_LIST)
	{
		if ( (m_pFileTransfer) && 
				(CUIMApplication::GetApplication()->GetHistoryManager()) )
		{
			IUOLMessengerHistoryManager4Ptr pHistoryManager4 = 
					ap_static_cast<IUOLMessengerHistoryManager4Ptr>(
							CUIMApplication::GetApplication()->GetHistoryManager());
			
			if (pHistoryManager4)
			{
				CHistoryRecordFileTransferPtr pHistoryRecordFileTransfer;
				pHistoryRecordFileTransfer = new CHistoryRecordFileTransfer(m_pFileTransfer);
				
				if (pHistoryRecordFileTransfer)
				{
					pHistoryManager4->DeleteHistoryEvents(pHistoryRecordFileTransfer);
					
					m_bIsVisible = FALSE;
					
					m_pGroupListCtrl->UpdateWindow();
				}
			}
		}
	}
}


void CFileTransferNode::UpdateNodeCursor(BOOL bIsOverActionString)
{
	if (bIsOverActionString)
	{
		if (!m_hDefaultCursor)
		{
			m_hDefaultCursor = ::SetCursor(m_hWaitCursor);
		}
		else
		{
			::SetCursor(m_hWaitCursor);
		}
		
		m_bIsShowingLinkCursor = TRUE;
	}
	else if ( (!bIsOverActionString) && (m_hDefaultCursor) && (m_bIsShowingLinkCursor) )
	{
		::SetCursor(m_hDefaultCursor);
		
		m_bIsShowingLinkCursor = FALSE;
	}
}


int CFileTransferNode::Compare(const CEntry* pEntry) const
{
	const CFileTransferNode* pFileTransferNode = dynamic_cast<const CFileTransferNode*>(pEntry);
	if (pFileTransferNode)
	{
		IUOLMessengerFileTransfer2Ptr pFileTransfer2 = 
			ap_dynamic_cast<IUOLMessengerFileTransfer2Ptr>(pFileTransferNode->GetFileTransfer());
		
		if (pFileTransfer2)
		{
			if (m_pFileTransfer->GetTime() > pFileTransfer2->GetTime())
			{
				return -1;
			}
			else if (m_pFileTransfer->GetTime() < pFileTransfer2->GetTime())
			{
				return 1;
			}
			else 
			{
				return 0;
			}
		}
	}

	return 0;
}
