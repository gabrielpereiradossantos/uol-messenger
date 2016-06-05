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
#include "PluginNode.h"
#include "../../resource.h"
#include "../image/Image.h"
#include "../GroupListCtrl/GroupListCtrl.h"
#include "../../controller/UIMApplication.h"
#include "../../controller/UIManager.h"
#include "../Preferences/PluginDownloadDialog.h"

#include <interfaces/IUOLMessengerPreferencesPage.h>


#define	LEFT_SHIFT_SPACE	10
#define	RIGHT_SHIFT_SPACE	10
#define	TOP_SHIFT_SPACE		5
#define BUTTON_SPACE		80
#define CHECKBOX_WIDTH		13
#define CHECKBOX_HEIGHT		13
#define	ICON_WIDTH			32
#define	ICON_HEIGHT			32
#define	LINE_SHIFT_SPACE	3


using namespace GroupListCtrl;


CPluginNode::CPluginNode(CPluginInfoPtr pPluginInfo, 
						 IUOLMessengerImagePtr pCheckOnImage,
						 IUOLMessengerImagePtr pCheckOffImage,
						 IUOLMessengerImagePtr pUpdateImage,
						 IUOLMessengerImagePtr pConfigImage,
						 IUOLMessengerImagePtr pInstallImage) :
	m_pPluginInfo(pPluginInfo),
	m_pDefaultPainter(NULL),
	m_pCurrentPainter(NULL),
	m_pSelectionPainter(NULL),
	m_clrTextNormal(RGB(0, 0, 0)),
	m_clrTextSelected(RGB(0, 0, 0)),
	m_bIsChecked(FALSE)
{
	m_pPluginManager = static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetPluginManager();
	m_bIsChecked = m_pPluginManager->IsPluginEnabled(m_pPluginInfo->GetInternalName());
	
	// Create CheckOn button
	m_pCheckOnButton = new CImageButton();
	m_pCheckOnButton->SetImage(pCheckOnImage);
	m_pCheckOnButton->SetWidth(CHECKBOX_WIDTH);
	m_pCheckOnButton->SetDrawTransparent(FALSE);
	m_pCheckOnButton->SetCommandName("CheckOn");
	m_pCheckOnButton->AddEventListener(this);
	m_pCheckOnButton->SetVisible(m_bIsChecked);

	// Create CheckOff button
	m_pCheckOffButton = new CImageButton();
	m_pCheckOffButton->SetImage(pCheckOffImage);
	m_pCheckOffButton->SetWidth(CHECKBOX_WIDTH);
	m_pCheckOffButton->SetDrawTransparent(FALSE);
	m_pCheckOffButton->SetCommandName("CheckOff");
	m_pCheckOffButton->AddEventListener(this);
	m_pCheckOffButton->SetVisible(!m_bIsChecked);
	
	// Create Update button
	m_pUpdateButton = new CImageButton();
	m_pUpdateButton->SetImage(pUpdateImage);
	m_pUpdateButton->SetWidth((pUpdateImage->GetWidth()));
	m_pUpdateButton->SetDrawTransparent(TRUE);
	m_pUpdateButton->SetCommandName("Update");
	m_pUpdateButton->AddEventListener(this);

	// Create Config button
	m_pConfigButton = new CImageButton();
	m_pConfigButton->SetImage(pConfigImage);
	m_pConfigButton->SetWidth(pConfigImage->GetWidth());
	m_pConfigButton->SetDrawTransparent(TRUE);
	m_pConfigButton->SetCommandName("Config");
	m_pConfigButton->AddEventListener(this);

	// Create Install button
	m_pInstallButton = new CImageButton();
	m_pInstallButton->SetImage(pInstallImage);
	m_pInstallButton->SetWidth(pInstallImage->GetWidth());
	m_pInstallButton->SetDrawTransparent(TRUE);
	m_pInstallButton->SetCommandName("Install");
	m_pInstallButton->AddEventListener(this);
}


CPluginNode::~CPluginNode()
{
	if (m_pCheckOnButton)
	{
		m_pCheckOnButton->RemoveEventListener(this);
	}

	if (m_pCheckOffButton)
	{
		m_pCheckOffButton->RemoveEventListener(this);
	}

	if (m_pUpdateButton)
	{
		m_pUpdateButton->RemoveEventListener(this);
	}

	if (m_pConfigButton)
	{
		m_pConfigButton->RemoveEventListener(this);
	}

	if (m_pInstallButton)
	{
		m_pInstallButton->RemoveEventListener(this);
	}
}


CPluginInfoPtr CPluginNode::GetPluginInfo()
{
	return m_pPluginInfo;
}


CString CPluginNode::GetPluginInternalName() const
{
	CString strName;
	strName.Format("%s:%s", m_pPluginInfo->GetName(), 
		m_pPluginInfo->GetInternalName());

	return strName.Trim();
}


void CPluginNode::SetDefaultPainter(IPainter* pPainter)
{
	m_pDefaultPainter = pPainter;

	if (m_pCurrentPainter == NULL)
	{
		m_pCurrentPainter = pPainter;
	}
}


IPainter* CPluginNode::GetDefaultPainter(IPainter* pPainter) const
{
	return m_pDefaultPainter;
}


void CPluginNode::SetPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pCurrentPainter = pPainter;

		RestorePainter(pPainter->GetTimeout());
	}
}


IPainter* CPluginNode::GetPainter() const
{
	return m_pCurrentPainter;
}


void CPluginNode::SetSelectionPainter(IPainter* pPainter)
{
	if (pPainter != NULL)
	{
		m_pSelectionPainter = pPainter;
	}
}


IPainter* CPluginNode::GetSelectionPainter() const
{
	return m_pSelectionPainter;
}


void CPluginNode::SetTextFont(CFontElementPtr  pFont)
{
	m_pFontTextNormal = pFont;
}


void CPluginNode::SetSelectedTextFont(CFontElementPtr  pFont)
{
	m_pFontTextSelected = pFont;
}


void CPluginNode::SetTitleTextFont(CFontElementPtr  pFont)
{
	m_pFontTextTitle = pFont;
}


void CPluginNode::SetTextColor(COLORREF clrText)
{
	m_clrTextNormal = clrText;
}


void CPluginNode::SetSelectedTextColor(COLORREF clrText)
{
	m_clrTextSelected = clrText;
}


void CPluginNode::SetTitleTextColor(COLORREF clrText)
{
	m_clrTextTitle = clrText;
}


void CPluginNode::SetName(const CString& strName)
{
	m_strName = strName;
}


CString CPluginNode::GetName() const
{
	return m_strName;
}


BOOL CPluginNode::IsVisible() const
{
	return TRUE;
}


void CPluginNode::Paint(CDCHandle& dc)
{
	DrawCheckbox(dc);
	DrawIcon(dc);
	DrawTitle(dc);	
	DrawButtons(dc);
}


void CPluginNode::DrawFocusRect(CDCHandle& dc)
{
	if (NULL == m_pSelectionPainter)
	{
		CGroupListCtrlItem::DrawFocusRect(dc);
	}
}


void CPluginNode::DrawBackground(CDCHandle& dc)
{
	CRect rect(CPoint(0, 0), GetSize());
	CMemDC dcMem(dc, &rect);
	
	IPainter* pPainter = m_pCurrentPainter;

	if (GetSelected() && m_pSelectionPainter != NULL)
	{
		pPainter = m_pSelectionPainter;
	}

	if (pPainter != NULL)
	{
		pPainter->Draw(CDCHandle(dcMem), rect);
	}

	// Draw separator
	COLORREF clrSeparator = RGB(0, 0, 0);
	CBrush brush;	
	brush.CreateSolidBrush(clrSeparator);

	CRect rectSeparator = rect;
	rectSeparator.top = rectSeparator.bottom - 1;
	rectSeparator.left += 2;
	rectSeparator.right -= 2;
	dcMem.FillRect(rectSeparator, brush);
}


void CPluginNode::OnLBtnDblClick(const CPoint& pt)
{
}


void CPluginNode::OnLButtonDown(const CPoint& pt)
{
	UpdateButtonState(pt, m_pCheckOnButton, CImageButton::MouseClick);
	UpdateButtonState(pt, m_pCheckOffButton, CImageButton::MouseClick);

	if ((m_pCheckOnButton != NULL) && (m_pCheckOnButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOnButton))
		{
			m_pCheckOnButton->OnLButtonDown(pt);
		}
	}
	else if ((m_pCheckOffButton != NULL) && (m_pCheckOffButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOffButton))
		{
			m_pCheckOffButton->OnLButtonDown(pt);
		}
	}
	
	if ((m_pUpdateButton != NULL) && (m_pUpdateButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pUpdateButton))
		{
			m_pUpdateButton->OnLButtonDown(pt);
		}
	}
	
	if ((m_pConfigButton != NULL) && (m_pConfigButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pConfigButton))
		{
			m_pConfigButton->OnLButtonDown(pt);
		}
	}

	if ((m_pInstallButton != NULL) && (m_pInstallButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pInstallButton))
		{
			m_pInstallButton->OnLButtonDown(pt);
		}
	}
}


void CPluginNode::OnLButtonUp(const CPoint& pt)
{
	UpdateButtonState(pt, m_pCheckOnButton, CImageButton::MouseOut);
	UpdateButtonState(pt, m_pCheckOffButton, CImageButton::MouseOut);

	if ((m_pCheckOnButton != NULL) && (m_pCheckOnButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOnButton))
		{
			m_pCheckOnButton->OnLButtonUp(pt);
		}
	}
	else if ((m_pCheckOffButton != NULL) && (m_pCheckOffButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOffButton))
		{
			m_pCheckOffButton->OnLButtonUp(pt);
		}
	}

	if ((m_pUpdateButton != NULL) && (m_pUpdateButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pUpdateButton))
		{
			m_pUpdateButton->OnLButtonUp(pt);
		}
	}
	
	if ((m_pConfigButton != NULL) && (m_pConfigButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pConfigButton))
		{
			m_pConfigButton->OnLButtonUp(pt);
		}
	}

	if ((m_pInstallButton != NULL) && (m_pInstallButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pInstallButton))
		{
			m_pInstallButton->OnLButtonUp(pt);
		}
	}
}


void CPluginNode::OnRButtonDown(const CPoint& pt)
{
}


void CPluginNode::OnRBtnDblClick(const CPoint& pt)
{
}


void CPluginNode::OnReturnKeyPressed()
{
}


void CPluginNode::OnKeyDown(TCHAR ch)
{
}


void CPluginNode::OnMouseHover(const CPoint& pt)
{
}


void CPluginNode::OnMouseLeave()
{
	CPoint pt;
	UpdateButtonState(pt, m_pCheckOnButton, CImageButton::MouseOut);
	UpdateButtonState(pt, m_pCheckOffButton, CImageButton::MouseOut);
}


void CPluginNode::OnMouseMove(const CPoint& pt)
{
	UpdateButtonState(pt, m_pCheckOnButton, CImageButton::MouseOver);
	UpdateButtonState(pt, m_pCheckOffButton, CImageButton::MouseOver);

	if ((m_pCheckOnButton != NULL) && (m_pCheckOnButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOnButton))
		{
			m_pCheckOnButton->OnMouseMove(pt);
		}
	}

	if ((m_pCheckOffButton != NULL) && (m_pCheckOffButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pCheckOffButton))
		{
			m_pCheckOffButton->OnMouseMove(pt);
		}
	}

	if ((m_pUpdateButton != NULL) && (m_pUpdateButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pUpdateButton))
		{
			m_pUpdateButton->OnMouseMove(pt);
		}
	}

	if ((m_pConfigButton != NULL) && (m_pConfigButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pConfigButton))
		{
			m_pConfigButton->OnMouseMove(pt);
		}
	}

	if ((m_pInstallButton != NULL) && (m_pInstallButton->IsVisible()))
	{
		if (IsPointInButtonRect(pt, m_pInstallButton))
		{
			m_pInstallButton->OnMouseMove(pt);
		}
	}
}


void CPluginNode::RestorePainter(int nTimeout)
{
	time_t curtime;
	time(&curtime);
	m_timeRestore = curtime + nTimeout;
}


void CPluginNode::OnTimer(UINT nId)
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


void CPluginNode::DrawIcon(CDCHandle& dc)
{
	CSize size = GetSize();
	int x = m_rectCheckbox.right + LEFT_SHIFT_SPACE;
	int y = ((size.cy - ICON_HEIGHT) / 2);
	
	CRect rectIcon(CPoint(x, y), CSize(ICON_WIDTH, ICON_HEIGHT));
	
	IUOLMessengerImagePtr pImage;

	if (m_pPluginInfo)
	{
		pImage = m_pPluginInfo->GetIcon();
	}

	if (pImage != NULL)
	{
		IUOLMessengerImageDrawStrategyPtr pDrawStrategy;
		pDrawStrategy = new UOLMessengerImage::CDrawTransparentStrategy();
		
		rectIcon.left += ((rectIcon.Width() - pImage->GetWidth()) / 2);

		pImage->Draw(dc, rectIcon.left, rectIcon.top, pImage->GetWidth(), 
			pImage->GetHeight(), pDrawStrategy);
	}

	m_rectIcon = rectIcon;
}


void CPluginNode::DrawTitle(CDCHandle& dc)
{	
	CSize sizeTitle;
	CSize sizeDescription;
	
	// Draw title line
	CSize size = GetSize();
	int x = m_rectIcon.right + LEFT_SHIFT_SPACE;
	int y = TOP_SHIFT_SPACE;
	int cx = size.cx;
	int cy = size.cy - RIGHT_SHIFT_SPACE;
	CRect rectTitle(CPoint(x, y), CSize(cx, cy));
	m_rectTitle = rectTitle;
	
	CString strTitle = m_pPluginInfo->GetName();
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

	// Draw Description line
	y = rectTitle.top + sizeTitle.cy + LINE_SHIFT_SPACE;
	CRect rectDescription(CPoint(x, y), CSize(cx, cy));
		
	CString strDescription = m_pPluginInfo->GetDesc();
	pFont->DrawColoredText(dc, rectDescription, strDescription, clrText, 
			DT_LEFT | DT_SINGLELINE | DT_TOP | DT_END_ELLIPSIS | DT_NOPREFIX);
	pFont->GetTextWidth(dc, strDescription, sizeDescription);

	m_rectTitle.bottom = y + sizeDescription.cy;
}


void CPluginNode::DrawCheckbox(CDCHandle& dc)
{
	CSize size = GetSize();
	int nTop = ((size.cy - CHECKBOX_HEIGHT) / 2);
	m_rectCheckbox = CRect(CPoint(LEFT_SHIFT_SPACE, nTop), 
		CPoint(LEFT_SHIFT_SPACE + CHECKBOX_WIDTH, nTop + CHECKBOX_HEIGHT));

	int x = m_rectCheckbox.left;
	int y = m_rectCheckbox.top;
	int cx = m_rectCheckbox.Width();
	int cy = m_rectCheckbox.Height();
	
	ATLASSERT(m_pCheckOnButton != NULL);
	ATLASSERT(m_pCheckOffButton != NULL);
	
	// Draw CheckOn button
	if ((m_pCheckOnButton.get()) && (m_pCheckOnButton->GetImage().get()) && 
		(m_pCheckOnButton->IsVisible()) && (m_bIsChecked))
	{
		IUOLMessengerImagePtr pImage = m_pCheckOnButton->GetImage();
		CImageButton::MouseState state = m_pCheckOnButton->GetState();
		m_pCheckOnButton->SetButtonRect(CRect(x, y, x+ cx, y + cy));
		m_pCheckOnButton->Draw(dc, x, y, cx, cy, state);
	}

	// Draw CheckOff button
	if ((m_pCheckOffButton.get()) && (m_pCheckOffButton->GetImage().get()) &&
		(m_pCheckOffButton->IsVisible())  && (!m_bIsChecked))
	{
		IUOLMessengerImagePtr pImage = m_pCheckOffButton->GetImage();
		CImageButton::MouseState state = m_pCheckOffButton->GetState();
		m_pCheckOffButton->SetButtonRect(CRect(x, y, x + cx, y + cy));
		m_pCheckOffButton->Draw(dc, x, y, cx, cy, state);
	}
}


void CPluginNode::DrawButtons(CDCHandle& dc)
{
	CSize size = GetSize(); 
	CUOLMessengerPlugin* pPlugin = m_pPluginManager->GetPlugin(m_pPluginInfo->GetInternalName());

	if (pPlugin != NULL)
	{
		ATLASSERT(m_pUpdateButton != NULL);
		ATLASSERT(m_pConfigButton != NULL);
		ATLASSERT(m_pInstallButton != NULL);

		// Draw Update button
		if ((m_pUpdateButton.get()) && (m_pUpdateButton->GetImage().get()) && 
			(m_pUpdateButton->IsVisible()) && (m_pPluginInfo->IsUpdateAvailable()))
		{
			IUOLMessengerImagePtr pImage = m_pUpdateButton->GetImage();
			int cx = m_pUpdateButton->GetWidth();
			int cy = pImage->GetHeight();
			int x = ((size.cx - BUTTON_SPACE - LEFT_SHIFT_SPACE) + ((BUTTON_SPACE - cx) / 2));
			int y = ((size.cy - cy) / 2);
			CImageButton::MouseState state = m_pUpdateButton->GetState();
			m_pUpdateButton->SetButtonRect(CRect(x, y, x+ cx, y + cy));
			m_pUpdateButton->Draw(dc, x, y, cx, cy, state);
		}

		// Draw Config button
		if ((m_pConfigButton.get()) && (m_pConfigButton->GetImage().get()) &&
			(m_pConfigButton->IsVisible())  && (pPlugin->HasConfigDialog()))
		{
			IUOLMessengerImagePtr pImage = m_pConfigButton->GetImage();
			int cx = m_pConfigButton->GetWidth();
			int cy = pImage->GetHeight();
			int x = ((size.cx - BUTTON_SPACE - LEFT_SHIFT_SPACE) + ((BUTTON_SPACE - cx) / 2));
			int y = ((size.cy - cy) / 2);
			CImageButton::MouseState state = m_pConfigButton->GetState();
			m_pConfigButton->SetButtonRect(CRect(x, y, x + cx, y + cy));
			m_pConfigButton->Draw(dc, x, y, cx, cy, state);
		}

		// Draw Install button
		if ((m_pInstallButton.get()) && (m_pInstallButton->GetImage().get()) &&
			(m_pInstallButton->IsVisible())  && (m_pPluginInfo->IsInstallAvailable()))
		{
			IUOLMessengerImagePtr pImage = m_pInstallButton->GetImage();
			int cx = m_pInstallButton->GetWidth();
			int cy = pImage->GetHeight();
			int x = ((size.cx - BUTTON_SPACE - LEFT_SHIFT_SPACE) + ((BUTTON_SPACE - cx) / 2));
			int y = ((size.cy - cy) / 2);
			CImageButton::MouseState state = m_pInstallButton->GetState();
			m_pInstallButton->SetButtonRect(CRect(x, y, x + cx, y + cy));
			m_pInstallButton->Draw(dc, x, y, cx, cy, state);
		}
	}
}


void CPluginNode::OnLButtonUp(IUOLMessengerImageButton* pButton, const CPoint &ptClick)
{
	CString cmdName = pButton->GetCommandName();

	if (cmdName.CompareNoCase(_T("CheckOn")) == 0)
    {
		m_bIsChecked = FALSE;
		m_pGroupListCtrl->GetParent().SendMessage(WM_USER_PREFS_CHANGED);
    }
	else if (cmdName.CompareNoCase(_T("CheckOff")) == 0)
    {
		m_bIsChecked = TRUE;
		m_pGroupListCtrl->GetParent().SendMessage(WM_USER_PREFS_CHANGED);
    }
	else if (cmdName.CompareNoCase(_T("Config")) == 0)
    {
		ShowConfigDialog();
    }
	else if (cmdName.CompareNoCase(_T("Update")) == 0)
    {
		InstallUpdate();
    }
	else if (cmdName.CompareNoCase(_T("Install")) == 0)
    {
		InstallUpdate();
    }

	m_pCheckOnButton->SetVisible(m_bIsChecked);
	m_pCheckOffButton->SetVisible(!m_bIsChecked);
}


BOOL CPluginNode::IsPointInButtonRect(const CPoint& pt, CImageButtonPtr pButton)
{
	CRect rectNode = CGroupListCtrlItem::GetRect();
	
	CRect rectButton = pButton->GetButtonRect();
	rectButton.top += rectNode.top;
	rectButton.bottom += rectNode.top;
	rectButton.left += rectNode.left;
	rectButton.right += rectNode.left;

	return rectButton.PtInRect(pt);

}


void CPluginNode::UpdateButtonState(const CPoint& pt, CImageButtonPtr pButton, CImageButton::MouseState mouseState)
{
	BOOL bPtInImage = IsPointInButtonRect(pt, pButton);

	CImageButton::MouseState stateCurrent = pButton->GetState();

	if (bPtInImage)
	{
		pButton->SetState(mouseState);
	}
	else
	{
		pButton->SetState(CImageButton::MouseOut);
	}

	if (stateCurrent != pButton->GetState())
	{
		Invalidate();
	}
}


void CPluginNode::EnablePlugin() const
{
	CString strInternalName = m_pPluginInfo->GetInternalName();

	BOOL bIsPluginEnabled = m_pPluginManager->IsPluginEnabled(strInternalName);
	
	if (!bIsPluginEnabled && m_bIsChecked)
	{
		m_pPluginManager->EnablePlugin(strInternalName, m_bIsChecked);

		CUOLMessengerPlugin* pPlugin = m_pPluginManager->GetPlugin(strInternalName);

		BOOL bInit = FALSE;

		if (pPlugin != NULL)
		{
			bInit = m_pPluginManager->InitPlugin(pPlugin);
		}

		if (!bInit)
		{
			// Plugin not found or initialization failed
			m_pPluginManager->EnablePlugin(strInternalName, FALSE);
		}
	}
	else if (bIsPluginEnabled && !m_bIsChecked)
	{
		CUOLMessengerPlugin* pPlugin = m_pPluginManager->GetPlugin(strInternalName);
		if (pPlugin != NULL)
		{
			m_pPluginManager->FinalizePlugin(pPlugin);
		}

		m_pPluginManager->EnablePlugin(strInternalName, m_bIsChecked);
	}		
}


BOOL CPluginNode::IsUpdateAvailable() const
{
	return m_pPluginInfo->IsUpdateAvailable();
}


BOOL CPluginNode::IsInstallAvailable() const
{
	return m_pPluginInfo->IsInstallAvailable();
}


void CPluginNode::InstallPlugin(CHttpDownloaderCallback* pCallback)
{
	ATLASSERT(IsUpdateAvailable() || IsInstallAvailable());		

	m_dwErrorCode = CUOLMessengerPluginManager::PIE_ERROR_FAILED;

	m_pInstallPluginThread = new CInstallPluginThread(m_pPluginInfo);
	m_pInstallPluginThread->SetCallback(pCallback);
	m_pInstallPluginThread->Start();
}


void CPluginNode::CancelInstallation()
{
	if (m_pInstallPluginThread)
	{
		m_pInstallPluginThread->CancelInstallation();
		m_pInstallPluginThread->MessageLoopWait(30000);

		m_dwErrorCode = m_pInstallPluginThread->GetInstallationError();
	}
}


void CPluginNode::WaitInstallation()
{
	if (m_pInstallPluginThread)
	{
		m_pInstallPluginThread->MessageLoopWait();

		m_dwErrorCode = m_pInstallPluginThread->GetInstallationError();
	}
}


DWORD CPluginNode::GetInstallationError() const
{
	return m_dwErrorCode;
}


BOOL CPluginNode::IsPluginEnabled() const
{
	return m_pPluginManager->IsPluginEnabled(m_pPluginInfo->GetInternalName());
}


BOOL CPluginNode::HasConfigDialog() const
{
	CUOLMessengerPlugin* pPlugin = m_pPluginManager->GetPlugin(m_pPluginInfo->GetInternalName());

	if (pPlugin)
	{
		return pPlugin->HasConfigDialog();
	}

	return FALSE;
}


void CPluginNode::ShowConfigDialog() const
{
	CUOLMessengerPlugin* pPlugin = m_pPluginManager->GetPlugin(m_pPluginInfo->GetInternalName());

	if (pPlugin)
	{
		pPlugin->ShowConfigDialog();
	}
}


CString CPluginNode::GetDescription() const
{
	return m_pPluginInfo->GetDesc();
}

void CPluginNode::InstallUpdate()
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	
	if (IsUpdateAvailable() || IsInstallAvailable())
	{
		CPluginDownloadDialog downloadDialog(this);
		downloadDialog.DoModal();

		WaitInstallation();

		DWORD dwError = GetInstallationError();

		if ((dwError != CUOLMessengerPluginManager::PIE_ERROR_SUCCESS) &&
			(dwError != CUOLMessengerPluginManager::PIE_USER_CANCELED))
		{
            CString strMessage;

			UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
			if (IsUpdateAvailable())
			{
				strMessage = pStringLoader->GetValue("uol.resource.error.IDS_PLUGIN_UPDATE_FAILED_MESSAGE");
			}
			else
			{
				strMessage = pStringLoader->GetValue("uol.resource.error.IDS_PLUGIN_INSTALLATION_FAILED_MESSAGE");
			}

			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"Ocorreu o erro [%s] durante a instalação do plugin [%s], da URL [%s], na pasta [%s].", 
					strMessage, GetPluginInfo()->GetInternalName(), GetPluginInfo()->GetDownloadUrl(), 
					GetPluginInfo()->GetFileName());
			
			CUIMApplication::GetApplication()->GetUIManager()->ShowMessageBox(strMessage, MB_OK | MB_ICONERROR);
		}
	}	
}