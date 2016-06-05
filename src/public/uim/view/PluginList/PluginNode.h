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
#include "../Skin/PluginEntryElement.h"
#include "../Skin/FontElement.h"
#include "../ImageButton.h"
#include "../UOLToolbar.h"
#include "../../model/PluginInfo.h"
#include "../../controller/HttpDownloader.h"
#include "../../controller/InstallPluginThread.h"


class CPluginNode : 
	public GroupListCtrl::CEntry,
    public CUOLMessengerImageButtonEventListener2
{
public:
	CPluginNode(CPluginInfoPtr pPluginInfo, 
				IUOLMessengerImagePtr pCheckOnImage,
				IUOLMessengerImagePtr pCheckOffImage,
				IUOLMessengerImagePtr pUpdateImage,
				IUOLMessengerImagePtr pConfigImage,
				IUOLMessengerImagePtr pInstallImage);
	virtual ~CPluginNode();

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
	
	CPluginInfoPtr GetPluginInfo();
	CString GetPluginInternalName() const;
	BOOL IsUpdateAvailable() const;
	BOOL IsInstallAvailable() const;
	void InstallUpdate();
	void InstallPlugin(CHttpDownloaderCallback* pCallback);
	void CancelInstallation();
	void WaitInstallation();
	DWORD GetInstallationError() const;
	BOOL IsPluginEnabled() const;
	BOOL HasConfigDialog() const;
	void ShowConfigDialog() const;
	void EnablePlugin() const;
	CString GetDescription() const;

	// CImageButtonEventListener methods
	void OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick);

protected:
	// CEntry interface
	virtual void SetName(const CString& strName);
	virtual CString GetName() const;

	virtual BOOL IsVisible() const;
	
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

	void RestorePainter(int nTimeout);

private:
	void DrawIcon(CDCHandle& dc);
	void DrawTitle(CDCHandle& dc);
	void DrawCheckbox(CDCHandle& dc);
	void DrawButtons(CDCHandle& dc);

	BOOL IsPointInButtonRect(const CPoint& pt, CImageButtonPtr pButton);
	void UpdateButtonState(const CPoint& pt, CImageButtonPtr pButton, CImageButton::MouseState mouseState);

private:
	CPluginInfoPtr		m_pPluginInfo;
	CUOLMessengerPluginManager* m_pPluginManager;
	
	IPainter*			m_pDefaultPainter;
	IPainter*			m_pCurrentPainter;
	IPainter*			m_pSelectionPainter;

	CRect				m_rectTitle;
	CRect				m_rectIcon;
	CRect				m_rectCheckbox;
	time_t				m_timeRestore;
	CFontElementPtr		m_pFontTextNormal;
	CFontElementPtr		m_pFontTextSelected;
	CFontElementPtr		m_pFontTextTitle;
	COLORREF			m_clrTextNormal;
	COLORREF			m_clrTextSelected;
	COLORREF			m_clrTextTitle;
	CString				m_strName;

	CImageButtonPtr		m_pCheckOnButton;
	CImageButtonPtr		m_pCheckOffButton;
	CImageButtonPtr		m_pUpdateButton;
	CImageButtonPtr		m_pConfigButton;
	CImageButtonPtr		m_pInstallButton;

	BOOL				m_bIsChecked;

	CInstallPluginThreadPtr		m_pInstallPluginThread;
	DWORD						m_dwErrorCode;
};


MAKEAUTOPTR(CPluginNode);
