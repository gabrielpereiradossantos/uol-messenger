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
 
#include "StdAfx.h"
#include "./userstatuspanel.h"

#include "image/image.h"
#include "image/imagebuilder.h"
#include "../controller/UIMApplication.h"

#include "../resource.h"


CUserStatusPanel::CUserStatusPanel(void) : 
	m_clrText(RGB(0, 0, 0)),
	m_clrStatus(RGB(150, 150, 150)), 
	m_clrStatusMessage(RGB(0, 0, 0)), 
	m_cy(0), 
	m_rectStatus(0, 0, 0, 0), 
	m_rectStatusMessage(0, 0, 0, 0), 
	m_rectText(0, 0, 0, 0), 
	m_rectImageArrow(0, 0, 0, 0),
	m_nStatusComboBoxHeight(0)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	m_strNullStatusMessage = pStringLoader->GetValue("uol.resource.IDS_STATUS_PAINEL_STATUS_MESSAGE_TEXT");
	m_strStatusMessage = m_strNullStatusMessage;
}

CUserStatusPanel::~CUserStatusPanel(void)
{
}

void CUserStatusPanel::SetAvatar(CUserAvatarPtr pAvatar)
{
	m_pAvatar = pAvatar;
}

void CUserStatusPanel::SetText(const CString& strText)
{
	m_strText = strText;
}

void CUserStatusPanel::SetStatus(const CString& strStatus)
{
	m_strStatus = strStatus;
}

void CUserStatusPanel::SetStatusMessage(const CString& strStatusMessage)
{
	m_strStatusMessage = strStatusMessage;
}

CToolTipElement CUserStatusPanel::GetSelectedControl(CPoint& pos)
{
	UOLLib::ISettings* pStringLoader =  static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->GetStringLoader();
	ATLASSERT(pStringLoader);

	CToolTipElement toolTipElement;
	CString strTip;

	int nItem = CheckClickedItem(pos);

	switch(nItem)
	{
	case CLICKED_AVATAR:
		strTip = pStringLoader->GetValue("uol.resource.IDS_TOOLTIP_STATUS_PAINEL_AVATAR");
		break;

	case CLICKED_TEXT:
		strTip = pStringLoader->GetValue("uol.resource.IDS_TOOLTIP_STATUS_PAINEL_TEXT");
		break;

	case CLICKED_STATUS:
		strTip = pStringLoader->GetValue("uol.resource.IDS_TOOLTIP_STATUS_PAINEL_STATUS");
		break;

	case CLICKED_STATUS_MESSAGE:
		strTip = pStringLoader->GetValue("uol.resource.IDS_TOOLTIP_STATUS_PAINEL_STATUS_MESSAGE");
	}

	toolTipElement.SetToolTipText(strTip);

	return toolTipElement;
}

void CUserStatusPanel::GetMinSize(CSize& size)
{
	size = CSize(0, m_cy);
}


void CUserStatusPanel::AddEventListener(CUserStatusPanelEventListener* pListener)
{
	if (pListener != NULL)
	{
		POSITION pos = m_listEventListener.Find(pListener);
		if (pos == NULL)
		{
			m_listEventListener.AddTail(pListener);
		}
	}
}

void CUserStatusPanel::RemoveEventListener(CUserStatusPanelEventListener* pListener)
{
	if (pListener != NULL)
	{
		POSITION pos = m_listEventListener.Find(pListener);
		if (pos != NULL)
		{
			m_listEventListener.RemoveAt(pos);
		}
	}
}


void CUserStatusPanel::DoPaint(CDCHandle& dcHandle)
{
	__super::DoPaint(dcHandle);

	DrawAvatar(dcHandle);
	DrawStatus(dcHandle);
	DrawText(dcHandle);	
	DrawStatusMessage(dcHandle);
}

void CUserStatusPanel::UpdateLayout(BOOL bResizeBars)
{
	__super::UpdateLayout(bResizeBars);

	InvalidateRect(NULL);
}

LRESULT CUserStatusPanel::OnSetCursor(HWND wCtrl, UINT nHitTest, UINT nMouseMsg)
{	
	CPoint ptPos = ::GetMessagePos();	

	ScreenToClient(&ptPos);

	int nItem = CheckClickedItem(ptPos);

	if(CLICKED_AVATAR == nItem)
	{
		HCURSOR hCursor = LoadCursor(0, IDC_HAND);
		if(hCursor)
		{
			::SetCursor(hCursor);
			return TRUE;
		}

	}
	else if(CLICKED_TEXT == nItem)
	{
		HCURSOR hCursor = LoadCursor(0, IDC_HAND);
		if(hCursor)
		{
			::SetCursor(hCursor);	
			return TRUE;
		}

	}
	else if(CLICKED_STATUS == nItem)
	{
		HCURSOR hCursor = LoadCursor(0, IDC_ARROW);
		if(hCursor)
		{
			::SetCursor(hCursor);
			return TRUE;
		}

	}
	else if(CLICKED_STATUS_MESSAGE == nItem)
	{
		HCURSOR hCursor = LoadCursor(0, IDC_HAND);
		if(hCursor)
		{
			::SetCursor(hCursor);	
			return TRUE;
		}

	}
	SetMsgHandled(FALSE);
	return FALSE;

}

void CUserStatusPanel::OnLButtonDown(UINT nFlags, CPoint& ptClick)
{
	int nItem = CheckClickedItem(ptClick);
		
	if (CLICKED_AVATAR == nItem)
	{
		m_pAvatar->OnLButtonDown(ptClick);

		NotifyEventListeners(&CUserStatusPanelEventListener::OnLButtonDownAvatar, ptClick);
	}
	else if (CLICKED_TEXT == nItem)
	{
		NotifyEventListeners(&CUserStatusPanelEventListener::OnLButtonDownText, ptClick);
	}
	else if (CLICKED_STATUS == nItem)
	{
		NotifyEventListeners(&CUserStatusPanelEventListener::OnLButtonDownStatusButton, ptClick);
	}
	else if (CLICKED_STATUS_MESSAGE == nItem)
	{
		NotifyEventListeners(&CUserStatusPanelEventListener::OnLButtonDownStatusMessage, ptClick);
	}
}

void CUserStatusPanel::OnRButtonDown(UINT nFlags, CPoint& ptClick)
{
	int nItem = CheckClickedItem(ptClick);

	if (CLICKED_AVATAR == nItem)
	{
		m_pAvatar->OnRButtonDown(ptClick);

		NotifyEventListeners(&CUserStatusPanelEventListener::OnRButtonDownAvatar, ptClick);
	}
	else if (CLICKED_TEXT == nItem)
	{
		NotifyEventListeners(&CUserStatusPanelEventListener::OnRButtonDownText, ptClick);
	}
}

void CUserStatusPanel::NotifyEventListeners(void (CUserStatusPanelEventListener::*pfnCallback)(const CPoint& ptClick), const CPoint& ptClick)
{
	CUserStatusPanelEventListener* pListener;
	for (POSITION pos = m_listEventListener.GetHeadPosition(); pos; )
	{
		pListener = m_listEventListener.GetNext(pos);
		(pListener->*pfnCallback)(ptClick);
	}
}

void CUserStatusPanel::DrawAvatar(CDCHandle& dc)
{
	if (m_pAvatar != NULL)
	{
		IUOLMessengerImagePtr pImage = m_pAvatar->GetImage();

		if (pImage != NULL)
		{
			//IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawStretchOpaqueStrategy();
			IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawPhotoStrategy();

			pImage->Draw(dc, 
					CUserStatusPanel::PM_HBORDER, CUserStatusPanel::PM_VBORDER, 
					CUserStatusPanel::PM_AVATAR_WIDTH, CUserStatusPanel::PM_AVATAR_HEIGHT, 
					pStrategy);
		}
	}
}

void CUserStatusPanel::DrawText(CDCHandle& dc)
{
	ATLASSERT(m_pFontText.get() != NULL);

	if (m_pFontText)
	{	
		CRect rectClient;
		GetClientRect(&rectClient);

		CRect rectParent;
		GetParent().GetClientRect(&rectParent);

		// Get text rectangle size with extra space
		CSize sizeText(0, m_nStatusComboBoxHeight);
		const int EXTRA_SPACE = 5;
		
		const int xText = (2 * CUserStatusPanel::PM_HBORDER) + CUserStatusPanel::PM_AVATAR_WIDTH + GetHorizontalSpacing();
		const int yText = GetVerticalSpacing();

		m_pFontText->GetTextWidth(dc, m_strText, sizeText);

		sizeText.cx = min(sizeText.cx, rectClient.Width() - xText - EXTRA_SPACE);

		m_rectText = CRect(CPoint(xText, yText), sizeText);	
			

		m_pFontText->DrawColoredText(dc, 
							m_rectText, 
							static_cast<LPCTSTR>(m_strText), 
							m_clrText,  
							DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS | DT_MODIFYSTRING);
	}
}	

void CUserStatusPanel::DrawStatus(CDCHandle& dc)
{
	if (m_strText.IsEmpty())
	{
		return;
	}

	ATLASSERT(m_pFontStatus.get());
	ATLASSERT(m_pImageArrow.get());
	
	CRect rectClient;
	GetClientRect(&rectClient);

	if (0 == m_nStatusComboBoxHeight)
	{
		m_nStatusComboBoxHeight = rectClient.Height() / 2;
	}

	// Get status text rectangle size with extra space
	CRect statusRect(CPoint((2 * CUserStatusPanel::PM_HBORDER) + CUserStatusPanel::PM_AVATAR_WIDTH + GetHorizontalSpacing(), 
							rectClient.Height() - m_nStatusComboBoxHeight), 
					 CSize(CUserStatusPanel::PM_STATUS_COMBOBOX_WIDTH, m_nStatusComboBoxHeight));

	// Draw status text
	if (m_pFontStatus)
	{
		const int EXTRA_SPACE = 5;

		CRect rectText = statusRect;
		rectText.right -= EXTRA_SPACE;

		m_pFontStatus->DrawColoredText(dc, rectText, m_strStatus, m_clrStatus, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
	}	
	
	// Draw arrow...
	if (m_pImageArrow)
	{
		int nArrowWidth = m_pImageArrow->GetWidth();
		int nArrowHeight = m_pImageArrow->GetHeight();

		m_rectImageArrow.right = statusRect.right;
		m_rectImageArrow.left = m_rectImageArrow.right - nArrowWidth;

		m_rectImageArrow.top = statusRect.top + ((statusRect.Height() - nArrowHeight) / 2);
		m_rectImageArrow.bottom = m_rectImageArrow.top + nArrowHeight;

		IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawTransparentStrategy();

		m_pImageArrow->Draw(dc, 
							m_rectImageArrow.left, m_rectImageArrow.top,
							nArrowWidth, nArrowHeight,
							pStrategy);
	}

	m_rectStatus = statusRect;
}

void CUserStatusPanel::DrawStatusMessage(CDCHandle& dc)
{
	ATLASSERT(m_pFontStatusMessage.get() != NULL);

	if (m_pFontStatusMessage)
	{	
		CRect rectClient;
		GetClientRect(&rectClient);

		CRect rectParent;
		GetParent().GetClientRect(&rectParent);

		// Get text rectangle size with extra space
		CSize sizeText(0, m_nStatusComboBoxHeight);
		const int EXTRA_SPACE = 5;
		
		const int xText = (2 * CUserStatusPanel::PM_HBORDER) + CUserStatusPanel::PM_AVATAR_WIDTH + GetHorizontalSpacing();
		const int yText = m_rectText.bottom + 2;

		CString strMessage = m_strStatusMessage.IsEmpty() ? m_strNullStatusMessage : m_strStatusMessage;

		m_pFontStatusMessage->GetTextWidth(dc, strMessage, sizeText);

		sizeText.cx = min(sizeText.cx, rectClient.Width() - xText - EXTRA_SPACE);

		m_rectStatusMessage = CRect(CPoint(xText, yText), sizeText);	
			
		m_pFontStatusMessage->DrawColoredText(dc, 
							m_rectStatusMessage, 
							static_cast<LPCTSTR>(strMessage), 
							m_clrStatusMessage,  
							DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS | DT_MODIFYSTRING);
	}
}	

int CUserStatusPanel::GetVerticalSpacing() const
{
	return 2;
}

int CUserStatusPanel::GetHorizontalSpacing() const
{
	return 10;
}

int CUserStatusPanel::GetTextAreaHeight() const
{
	CRect rectClient;
	GetClientRect(&rectClient);

	return (rectClient.Height() / 2);
}

int CUserStatusPanel::CheckClickedItem(const CPoint& ptClick)
{
	if (m_pAvatar != NULL)
	{
		IUOLMessengerImagePtr pImage = m_pAvatar->GetImage();

		if (pImage != NULL)
		{
			CRect rectTest(CPoint(0, 0), CSize(CUserStatusPanel::PM_AVATAR_WIDTH, CUserStatusPanel::PM_AVATAR_HEIGHT));

			if (rectTest.PtInRect(ptClick))
			{
				return CLICKED_AVATAR;
			}

			if (m_rectText.PtInRect(ptClick))
			{
				return CLICKED_TEXT;
			}

			if (m_rectStatus.PtInRect(ptClick))
			{
				return CLICKED_STATUS;
			}

			if (m_rectStatusMessage.PtInRect(ptClick))
			{
				return CLICKED_STATUS_MESSAGE;
			}
		}
	}

	return CLICKED_NONE;
}

void CUserStatusPanel::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);	

	GetParameterValue(m_cy, pMapParams, "Height");

	try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "StatusFont");

		m_pFontStatus = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "StatusFont");
		m_pFontStatus = new CFontElement();
	}

	try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "StatusMessageFont");

		m_pFontStatusMessage = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "StatusMessageFont");
		m_pFontStatusMessage = new CFontElement();
	}

	try
	{
		GetParameterValue(m_nStatusComboBoxHeight, pMapParams, "StatusComboBoxHeight");
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "StatusComboBoxHeight");
		m_nStatusComboBoxHeight = 0;
	}

	CString strImagePath;
	GetParameterValue(strImagePath, pMapParams, "StatusComboBoxArrow");

	m_pImageArrow = UOLMessengerImage::CImageBuilder::LoadImage(strImagePath);
	ATLASSERT(m_pImageArrow.get());

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "StatusColor");

		m_clrStatus.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "StatusColor");
	}

	try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "TextFont");

		m_pFontText = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "TextFont");
		m_pFontText = new CFontElement();
	}

	try
	{
		CString strColor;
		GetParameterValue(strColor, pMapParams, "TextColor");

		m_clrText.SetColor(strColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "TextColor");
		m_clrText = m_pFontText->GetColor();
	}
}

CString CUserStatusPanel::GetName() const
{
	return "__UserStatusPanelInstance";
}

CRect CUserStatusPanel::GetRectStatus() const
{
	return m_rectStatus;
}