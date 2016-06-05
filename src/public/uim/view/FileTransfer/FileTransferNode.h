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

#include "../GroupListCtrl/Entry.h"
#include "../IPainter.h"
#include "../Skin/FileTransferEntryElement.h"
#include "../Skin/FontElement.h"
#include "../UOLProgressBar.h"
#include "../FileTypeIcon.h"
#include "../UOLToolbar.h"

#include <interfaces/IUOLMessengerFileTransfer.h>
#include <interfaces/IUOLMessengerFileTransferNode.h>
#include <interfaces/IUOLMessengerFileTransferManager.h>


class CFileTransferNode : 
	public GroupListCtrl::CEntry,
	public IUOLMessengerFileTransferNode,
	public CFileTransferProgressObserver
{
public:
	CFileTransferNode(IUOLMessengerFileTransferPtr pFileTransfer, 
			CProgressBarElementPtr pProgressBarElement);
	virtual ~CFileTransferNode();

	void SetDefaultPainter(IPainter* pPainter);
	IPainter* GetDefaultPainter(IPainter* pPainter) const;

	void SetPainter(IPainter* pPainter);
	IPainter* GetPainter() const;

	void SetSelectionPainter(IPainter* pPainter);
	IPainter* GetSelectionPainter() const;

	void SetTextFont(CFontElementPtr pFont);
	void SetSelectedTextFont(CFontElementPtr pFont);
	void SetTitleTextFont(CFontElementPtr  pFont);
	void SetTextColor(COLORREF clrText);
	void SetSelectedTextColor(COLORREF clrText);
	void SetTitleTextColor(COLORREF clrText);
	void SetItemSelectionBkgrndImage(HBITMAP hItemSelectionBkgrndImage);
	void SetProgressImages(HBITMAP hProgressLeftImage, 
			HBITMAP hProgressMiddleImage, HBITMAP hProgressRightImage);
	void SetActionsTextFont(CFontElementPtr  pFont);
	void SetActionsTextColor(COLORREF clrText);
	void SetItemSeparatorColor(COLORREF clrBkgrnd);

	// IUOLMessengerFileTransferNode methods
	virtual IUOLMessengerFileTransferPtr GetFileTransfer() const;
	virtual CString GetFileTransferInternalName() const;

	virtual void RegisterObserver(CUOLMessengerFileTransferObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerFileTransferObserver* pObserver);

protected:
	// CEntry interface
	virtual void SetName(const CString& strName);
	virtual CString GetName() const;

	virtual BOOL IsVisible() const;

	virtual int Compare(const CEntry* pEntry) const;
	
	virtual void Paint(CDCHandle& dc);
	virtual void DrawFocusRect(CDCHandle& dc);
	virtual void DrawBackground(CDCHandle& dc);

	virtual void OnLBtnDblClick(const CPoint& pt);	
	virtual void OnLButtonDown(const CPoint& pt);
	virtual void OnLButtonUp(const CPoint& pt);
	virtual void OnRButtonDown(const CPoint& pt);
	virtual void OnRBtnDblClick(const CPoint& pt);
	virtual void OnReturnKeyPressed();
	virtual void OnKeyDown(TCHAR ch);
	virtual void OnMouseHover(const CPoint& pt);
	virtual void OnMouseLeave();
	virtual void OnMouseMove(const CPoint& pt);
	virtual void OnTimer(UINT nId);

	// CFileTransferProgressObserver method
	void UpdateProgress(IUOLMessengerFileTransfer* pFileTransfer);

	void RestorePainter(int nTimeout);

private:
	
	enum ActionStringId
	{
		ACTION_STRING_OPEN = 1,
		ACTION_STRING_REMOVE_FROM_LIST,
		ACTION_STRING_CANCEL
	};
	
private:
	void DrawIcon(CDCHandle& dc);
	void DrawText(CDCHandle& dc);
	void DrawTitle(CDCHandle& dc);
	void DrawActions(CDCHandle& dc);
	void DrawProgress(CDCHandle& dc);

	BOOL IsPointInButtonRect(const CPoint& pt, CImageButtonPtr pButton);

	CString GetFileTransferProgressText();
	CString GetFileTransferOperationText();

	void PlayFileTransferFinishedSound();
	
	CSize GetStringWidth(CDCHandle& dc, CString strToMeasure);
	
	UINT GetActionStringUnderCursor(const CPoint& pt);
	
	void ExecuteAction(UINT uActionStringId);
	
	void UpdateNodeCursor(BOOL bIsOverActionString);
	
private:
	IUOLMessengerFileTransfer2Ptr	m_pFileTransfer;
	CUOLProgressBarPtr				m_pProgressBar;
	CFileTypeIconPtr				m_pFileTypeIcon;
	
	IPainter*			m_pDefaultPainter;
	IPainter*			m_pCurrentPainter;
	IPainter*			m_pSelectionPainter;

	CRect				m_rectTitle;
	CRect				m_rectProgress;
	CRect				m_rectIcon;
	//CRect				m_rectButton;
	time_t				m_timeRestore;
	CFontElementPtr		m_pFontTextNormal;
	CFontElementPtr		m_pFontTextSelected;
	CFontElementPtr		m_pFontTextTitle;
	CFontElementPtr		m_pFontTextActions;
	COLORREF			m_clrTextNormal;
	COLORREF			m_clrTextSelected;
	COLORREF			m_clrTextTitle;
	COLORREF			m_clrTextActions;
	HBRUSH				m_hItemSeparatorBrush;
	CString				m_strName;

	HBITMAP				m_hItemSelectionBkgrnd;
	
	CString				m_strOpenAction;
	CString				m_strRemoveFromListAction;
	CString				m_strCancelAction;
	
	// Showing "open" action implies showing "remove from list" too.
	BOOL				m_bIsShowOpenAction;
	BOOL				m_bIsShowCancelAction;
	BOOL				m_bIsShowOnlyRemoveFromList;
	
	CRect				m_rectOpenAction;
	CRect				m_rectRemoveFromListAction;
	CRect				m_rectCancelAction;
	
	CComAutoCriticalSection		m_criticalSection;
	BOOL						m_bIsShowingLinkCursor;
	
	HCURSOR				m_hWaitCursor;
	HCURSOR				m_hDefaultCursor;
	
	BOOL				m_bIsVisible;
};


MAKEAUTOPTR(CFileTransferNode);
