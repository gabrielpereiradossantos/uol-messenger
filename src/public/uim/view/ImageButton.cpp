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
#include "ImageButton.h"

#include "image/imagebuilder.h"
#include "skin/bitmapbackgroundelement.h"


CImageButtonBase::CImageButtonBase(HWND hwndParent) : 
	m_hWnd(hwndParent),
	m_nWidth(0),
	m_strCaption(""),
	m_strTip(""),
	m_bDrawTransparent(FALSE), 
	m_bPushButton(FALSE), 
	m_bIsPressed(FALSE),
	m_bAuto(TRUE),
	m_bVisible(TRUE),
	m_hWndParent(hwndParent), 
	m_lpAttachment(NULL),
	m_state(CImageButton::MouseOut)
{	
	m_ButtonRect = CRect(0, 0, 0, 0);
}

CImageButtonBase::~CImageButtonBase()
{
	//ATLTRACE(_T("CImageButtonBase::~CImageButtonBase()\n"));
}

CString CImageButtonBase::GetCommandName() const
{
	return m_strCommandName;
}

void CImageButtonBase::SetCommandName(const CString& strCommandName)
{
	m_strCommandName = strCommandName;
}

CString CImageButtonBase::GetDragAndDropCommandName() const
{
	return m_strDragAndDropCommand;
}

void CImageButtonBase::SetDragAndDropCommandName(const CString& strCommandName)
{
	m_strDragAndDropCommand = strCommandName;
}

int CImageButtonBase::GetWidth() const
{
	return m_nWidth;
}

void CImageButtonBase::SetWidth(int nWidth)
{
	m_nWidth = nWidth;
}

CString CImageButtonBase::GetCaption() const
{
	return m_strCaption;
}

void CImageButtonBase::SetCaption(const CString& strCaption)
{
	m_strCaption = strCaption;
}

CString CImageButtonBase::GetTip() const
{
	return m_strTip;
}

void CImageButtonBase::SetTip(const CString& strTip)
{
	m_strTip = strTip;
}

BOOL CImageButtonBase::IsPressed() const
{
	return m_bIsPressed;
}

void CImageButtonBase::SetPressed(BOOL pressed)
{
	m_bIsPressed = pressed;

	Update();
}

BOOL CImageButtonBase::IsPushButton() const
{
	return m_bPushButton;
}

void CImageButtonBase::SetPushButton(BOOL bPushButton)
{
	m_bPushButton = bPushButton;
}

void CImageButtonBase::SetTextRect(const CRect& rect)
{
	m_textRect = rect;
}

void CImageButtonBase::GetTextRect(CRect &rect) const
{
	rect = m_textRect;
}



void CImageButtonBase::SetDrawTransparent(BOOL bTransparent)
{
	m_bDrawTransparent = bTransparent;
}

BOOL CImageButtonBase::GetDrawTransparent() const
{
	return m_bDrawTransparent;
}

BOOL CImageButtonBase::IsAuto() const
{
	return m_bAuto;
}
void CImageButtonBase::SetAuto(BOOL bAuto) 
{
	m_bAuto = bAuto;
}

BOOL CImageButtonBase::IsVisible() const
{
	return m_bVisible;
}

void CImageButtonBase::SetVisible(BOOL visible) 
{
	m_bVisible = visible;

	Update();
}

CImageButtonBase::MouseState CImageButtonBase::GetState() const
{
	return m_state;
}

void CImageButtonBase::SetState(CImageButtonBase::MouseState state)
{
	m_state = state;
}

void CImageButtonBase::SetImage(IUOLMessengerImagePtr pImage)
{
	m_pImage = pImage;

	Update();
}

IUOLMessengerImagePtr CImageButtonBase::GetImage() const
{
	return m_pImage;
}

void CImageButtonBase::Draw(CDCHandle& dc, int x, int y, int cx, int cy, MouseState state)
{
	ATLASSERT(m_pImage.get());

	if (!m_pImage.get())
	{
		return;
	}

	IUOLMessengerImageDrawStrategyPtr pUOLMessengerImageDrawStrategy = GetDrawStrategy();

	int w = m_pImage->GetWidth();
	int h = m_pImage->GetHeight();

	int xOffset = cx;
	
	if ( m_bIsPressed )
	{
		xOffset = cx * MouseState::MouseClick;
	}
	else
	{
		xOffset *= state;
	}

	m_pImage->Draw(dc, 
			x, y, 
			cx, cy, 
			xOffset, 0, 
			pUOLMessengerImageDrawStrategy);
}

void CImageButtonBase::AddEventListener(CUOLMessengerImageButtonEventListener2* pListener)
{
	if (pListener != NULL)
	{
		POSITION pos = m_listListeners.Find(pListener);

		if (pos == NULL)
		{
			m_listListeners.AddTail(pListener);
		}
	}
}

void CImageButtonBase::RemoveEventListener(CUOLMessengerImageButtonEventListener2* pListener)
{
	if (pListener != NULL)
	{
		POSITION pos = m_listListeners.Find(pListener);

		if (pos)
		{
			m_listListeners.RemoveAt(pos);
		}
	}
}


void CImageButtonBase::SetAttachment(LPVOID lpVoid)
{
	m_lpAttachment = lpVoid;
}

LPVOID CImageButtonBase::GetAttachment()
{
	return m_lpAttachment;
}

void CImageButtonBase::SetParent(HWND hWndParent)
{
	m_hWndParent = hWndParent;
}


void CImageButtonBase::OnLButtonDown(const CPoint& ptClick)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	NotifyEventListeners(&CUOLMessengerImageButtonEventListener2::OnLButtonDown, ptClick);
}


void CImageButtonBase::OnLButtonUp(const CPoint& ptClick)
{
	if ((m_bAuto) && (m_bPushButton))
	{
		m_bIsPressed = !m_bIsPressed;
	}

	NotifyEventListeners(&CUOLMessengerImageButtonEventListener2::OnLButtonUp, ptClick);	
}

void CImageButtonBase::OnLButtonDblClick(const CPoint& ptClick)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	NotifyEventListeners(&CUOLMessengerImageButtonEventListener2::OnLButtonDblClick, ptClick);
}

void CImageButtonBase::OnRButtonDown(const CPoint& ptClick)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	NotifyEventListeners(&CUOLMessengerImageButtonEventListener2::OnRButtonDown, ptClick);
}

void CImageButtonBase::OnRButtonDblClick(const CPoint& ptClick)
{
	//ATLTRACE(_T("%s\n"), __FUNCTION__);
	NotifyEventListeners(&CUOLMessengerImageButtonEventListener2::OnRButtonDblClick, ptClick);
}

void CImageButtonBase::OnMouseMove(const CPoint& ptClick)
{
	NotifyEventListeners(&CUOLMessengerImageButtonEventListener2::OnMouseMove, ptClick);
}

//void CImageButtonBase::OnLButtonDown(UINT, const CPoint& ptClick)
//{
//	OnLButtonDown(ptClick);
//}
//
//void CImageButtonBase::OnLButtonUp(UINT, const CPoint& ptClick)
//{
//	OnLButtonUp(ptClick);
//}
//
//void CImageButtonBase::OnLButtonDblClick(UINT, const CPoint& ptClick)
//{
//	OnLButtonDblClick(ptClick);
//}
//
//void CImageButtonBase::OnRButtonDown(UINT, const CPoint& ptClick)
//{
//	OnRButtonDown(ptClick);
//}
//
//void CImageButtonBase::OnRButtonDblClick(UINT, const CPoint& ptClick)
//{
//	OnRButtonDblClick(ptClick);
//}

void CImageButtonBase::NotifyEventListeners(void (CUOLMessengerImageButtonEventListener2::*pEventHandler)(IUOLMessengerImageButton* pButton, const CPoint& ptClick), const CPoint& ptClick)
{
	CUOLMessengerImageButtonEventListener2* pListener;

	for (POSITION pos = m_listListeners.GetHeadPosition(); pos; )
	{
		pListener = m_listListeners.GetNext(pos);
		(pListener->*pEventHandler)(this, ptClick);
	}
}


void CImageButtonBase::Update()
{
	CWindow wnd(m_hWndParent);

	if (wnd.IsWindow())
	{
		wnd.Invalidate();
		wnd.UpdateWindow();
	}
}

IUOLMessengerImageDrawStrategyPtr CImageButtonBase::GetDrawStrategy() const
{
	IUOLMessengerImageDrawStrategyPtr pUOLMessengerImageDrawStrategy;

	//if (m_pLeftImage.get() || m_pRightImage.get() )
	//{
	//	pUOLMessengerImageDrawStrategy = new UOLMessengerImage::CPatternStrategy();
	//}
	//else if (GetDrawTransparent())
	if (GetDrawTransparent())
	{
		pUOLMessengerImageDrawStrategy = new UOLMessengerImage::CDrawTransparentStrategy();
	}
	else
	{
		pUOLMessengerImageDrawStrategy = new UOLMessengerImage::CDrawOpaqueStrategy();
	}

	return pUOLMessengerImageDrawStrategy;
}


CRect CImageButtonBase::GetButtonRect()
{
	return m_ButtonRect;
}

void CImageButtonBase::SetButtonRect(CRect rect)
{
	m_ButtonRect = rect;
}


void CImageButtonBase::Release()
{
}


CImageButton::CImageButton(HWND hwndParent) : 
		CImageButtonBase(hwndParent)
{	
}

CImageButton::~CImageButton()
{
	//ATLTRACE(_T("CImageButton::~CImageButton()\n"));
}

void CImageButton::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);	

	try
	{
		GetParameterValue(m_strCaption, pMapParams, "caption");			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "caption");
		m_strCaption = "";
	}

	LoadImages(pMapParams, pElementTable);

	try
	{
		GetParameterValue(m_nWidth, pMapParams, "width");			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "width");
		m_nWidth = m_pImage->GetWidth();
	}

	try
	{
		CString strPushButton;
		GetParameterValue(strPushButton, pMapParams, "pushbutton");	

		m_bPushButton = (strPushButton == '1') || (0 == strPushButton.CompareNoCase("true"));
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "pushbutton");
		m_bPushButton = FALSE;
	}

	CString strTransparent;
	GetParameterValue(strTransparent, pMapParams, "transparent");

	m_bDrawTransparent = (0 == strTransparent.CompareNoCase("true") || ('1' == strTransparent));	

	GetParameterValue(m_strCommandName, pMapParams, "command");	

	try
	{
		CString strTip;

		GetParameterValue(strTip, pMapParams, "tip");
		SetTip(strTip);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "tip");
	}

	try
	{
		GetParameterValue(m_strDragAndDropCommand, pMapParams, "dndcommand");	
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "dndcommand");
		m_strDragAndDropCommand = "";
	}
}

CString CImageButton::GetName() const
{
	return m_strName;
}

void CImageButton::LoadImages(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	CString strImage;
	
	GetParameterValue(strImage, pMapParams, "image");

	m_pImage = UOLMessengerImage::CImageBuilder::LoadImage(strImage);
	ATLASSERT(m_pImage.get());

	//try
	//{	
	//	GetParameterValue(strImage, pMapParams, "left_image");

	//	m_pLeftImage = UOLMessengerImage::CImageBuilder::LoadImage(strImage);
	//	ATLASSERT(m_pLeftImage.get());

	//	GetParameterValue(strImage, pMapParams, "right_image");

	//	m_pRightImage = UOLMessengerImage::CImageBuilder::LoadImage(strImage);
	//	ATLASSERT(m_pRightImage.get());
	//}
	//catch(const CSkinException&)
	//{
	//	m_pLeftImage = NULL;
	//	m_pRightImage = NULL;
	//}
}

