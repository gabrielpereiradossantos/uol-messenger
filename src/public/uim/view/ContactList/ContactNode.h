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
#include "ComparatorAdapter.h"
#include "../IPainter.h"
#include "../UOLToolBar.h"
#include "../skin/fontelement.h"
#include "../Tooltip/ContactTooltip.h"
#include "../FindContactPanel.h"

#include <interfaces/IUOLMessengerContactListSettings.h>
#include <interfaces/IUOLMessengerContactNode.h>

class CContactNode : 
	public IUOLMessengerContactNode,
	public GroupListCtrl::CEntry, 
	public CUOLMessengerContactObserver
{
public:
	CContactNode(IUOLMessengerContactPtr pContact);
	virtual ~CContactNode();

	void SetComparatorAdapter(CComparatorAdapter* pComparator);
	CComparatorAdapter* GetComparatorAdapter() const;

	void SetDefaultPainter(IPainter* pPainter);
	IPainter* GetDefaultPainter(IPainter* pPainter) const;

	void SetPainter(IPainter* pPainter);
	IPainter* GetPainter() const;

	void SetSelectionPainter(IPainter* pPainter);
	IPainter* GetSelectionPainter() const;

	void SetTextFont(CFontElementPtr pFont);
	void SetSelectedTextFont(CFontElementPtr pFont);
	void SetTextFontBlocked(CFontElementPtr pFont);
	void SetDropTargetTextFont(CFontElementPtr pFont);
	void SetTextColor(COLORREF clrText);
	void SetTextColorBlocked(COLORREF clrText);
	void SetSelectedTextColor(COLORREF clrText);
	void SetDropTargetTextColor(COLORREF clrText);

	void SetAllContactsGroup(const CString& strAllContactsGroup);
	CString GetAllContactsGroup() const;

	void SetFindContactPanel(CFindContactPanelPtr pFindContactPanel);

	void GetMenuItens(CAtlList<IUOLMessengerContextMenuItemPtr>& listItens);

	// IUOLMessengerContactNode interface
	virtual IUOLMessengerContactPtr GetContact() const;
	virtual void AddCustomButton(IUOLMessengerImageButton2Ptr pButton);
	virtual void RemoveCustomButton(IUOLMessengerImageButton2Ptr pButton);
	virtual void AddCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem);
	virtual void RemoveCustomMenuItem(IUOLMessengerContextMenuItemPtr pCustomMenuItem);

protected:
	// CEntry interface
	virtual void SetName(const CString& strName);
	virtual CString GetName() const;

	virtual BOOL IsVisible() const;

	virtual int Compare(const CEntry* pEntry) const;

	virtual void Paint(CDCHandle& dc);
	virtual void DrawFocusRect(CDCHandle& dc);
	virtual void DrawBackground(CDCHandle& dc);

	virtual void OnLButtonDown(const CPoint&);
	virtual void OnLButtonUp(const CPoint&);
	virtual void OnLBtnDblClick(const CPoint&);
	virtual void OnRButtonDown(const CPoint&);
	virtual void OnRBtnDblClick(const CPoint&);

	virtual void OnMouseHover(const CPoint&);
	virtual void OnMouseLeave();
	virtual void OnMouseMove(const CPoint& pt);
	virtual void OnReturnKeyPressed();
	virtual void OnTimer(UINT nId);

	// CUOLMessengerContactObserver interface
	virtual void Update(IUOLMessengerContact* pContact);
	virtual void UpdateAlias(IUOLMessengerContact* pContact);
	virtual void UpdateStatus(IUOLMessengerContact* pContact);
	virtual void UpdateIcon(IUOLMessengerContact* pContact);

	void RestorePainter(int nTimeout);

	virtual void RegisterObserver(CUOLMessengerContactNodeObserver* pObserver);
	virtual void UnregisterObserver(CUOLMessengerContactNodeObserver* pObserver);

private:
	void DrawAvatar(CDCHandle& dc);
	void DrawText(CDCHandle& dc);
	void DrawStatus(CDCHandle& dc);
	void DrawButtons(CDCHandle& dc);

	void LaunchMessageWindow();

	void UpdateImpl();

	void NotifyObservers(void (CUOLMessengerContactNodeObserver::*pfnCallback)(IUOLMessengerContactNode*, IUOLMessengerImageButton2Ptr), IUOLMessengerImageButton2Ptr);

	CImageButtonPtr GetImageButton(const CPoint& pt, CImageButton::MouseState mouseState);
	
	// Visibility methods.
	BOOL IsAvatarVisible();
	BOOL IsStatusVisible();
	
	// Positioning methods, always considering that the alias element is visible, and 
	// that the buttons element will be aways with and on the right side of the alias.
	//
	int  GetElementPosition(IUOLMessengerContactListSettings2::ContactNodeElements nContactElement);
	
	// For fixed elements (like status and avatar), returns the element drawing rect.
	// For variable elements, it returns the available rect.
	//
	void GetRectForElement(IUOLMessengerContactListSettings2::ContactNodeElements nContactElement, CRect& rElementRect);
	
	// Returns the avatar side expected size considering the space occupied by the ContactNode.
	// As the avatar must be a perfect square, this value can be used to inform it's height 
	// and width.
	//
	int  GetAvatarSideSize() const;

    // Find Contact method
    BOOL IsFindContactMatch() const;

private:
#ifdef _LEAKDEBUG
	static LONG ms_nRefs;
#endif

	IUOLMessengerContactPtr		m_pContact;
	CComparatorAdapter*			m_pComparatorAdapter;
	IPainter*					m_pDefaultPainter;
	IPainter*					m_pCurrentPainter;
	IPainter*					m_pSelectionPainter;
	CString						m_strName;
	CString						m_strAllContactsGroup;
	time_t						m_timeRestore;
	CRect						m_rectAvatar;		
	CRect						m_rectStatus;
	CRect						m_rectButtons;
	CFontElementPtr				m_pFontTextNormal;
	CFontElementPtr 			m_pFontTextSelected;
	CFontElementPtr 			m_pFontContactBlocked;
	CFontElementPtr 			m_pFontTextDropTarget;
	COLORREF					m_clrTextNormal;
	COLORREF					m_clrTextSelected;
	COLORREF					m_clrTextBlocked;
	COLORREF					m_clrTextDropTarget;
	CContactTooltipPtr			m_pContactTooltip;
	CUOLToolBarPtr				m_pToolbar;
	CComAutoCriticalSection		m_criticalSection;
	CFindContactPanelPtr		m_pFindContactPanel;

	IUOLMessengerContactListSettingsPtr m_pUOLMessengerContactListSettings;
	CAtlList<CUOLMessengerContactNodeObserver*> m_listObservers;
	CAtlList<IUOLMessengerImageButton2Ptr> m_listButtons;
	CAtlList<IUOLMessengerContextMenuItemPtr> m_listMenus;
}; 

MAKEAUTOPTR(CContactNode);