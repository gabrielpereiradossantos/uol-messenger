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
#include "EmoticonsFrame.h"


#define		EMOTICONS_PER_LINE					10
#define		EMOTICON_WIDTH						24
#define		EMOTICON_HEIGHT						24
#define		EMOTICON_BORDER_EXTENT				2
#define		EMOTICON_SPACEMENT					2


//
//CEmoticonsFrame::CEmoticonsFrame()
//{
//	m_pView = new CEmoticonsWindow(this);
//}


CEmoticonsFrame::CEmoticonsFrame(CString strEmoticonFile) :
	m_strEmoticonFile(strEmoticonFile)
{
	m_pView = new CEmoticonsWindow(this, strEmoticonFile);
}


CEmoticonsFrame::~CEmoticonsFrame()
{
}


void CEmoticonsFrame::RegisterMessageEditPanel(CEmoticonAdd* emoticon)
{
	static_cast<CEmoticonsWindow*>(m_pView.get())->RegisterMessageEditPanel(emoticon); 
}


void CEmoticonsFrame::GetBorderSize(CSize& sizeBorder)
{
	ATLASSERT(m_pCurrentBorder != NULL);
	m_pCurrentBorder->GetSize(sizeBorder);

	int cySystemCaption = ::GetSystemMetrics(SM_CYCAPTION);
	int cxSystemBorder = ::GetSystemMetrics(SM_CXSIZEFRAME);
	int cySystemBorder = ::GetSystemMetrics(SM_CYSIZEFRAME);

	sizeBorder.cx = sizeBorder.cx + 2 * cxSystemBorder;
	sizeBorder.cy = sizeBorder.cy + (2 * cySystemBorder) + cySystemCaption;
}


void CEmoticonsFrame::SetConversation(IUOLMessengerConversationPtr pConversation)
{
	static_cast<CEmoticonsWindow*>(m_pView.get())->SetConversation(pConversation);
}


LRESULT CEmoticonsFrame::OnCreate(LPCREATESTRUCT lpCs)
{
	__super::OnCreate(lpCs);

	return 0L;
}


LRESULT CEmoticonsFrame::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);

	return 0;
}


LRESULT CEmoticonsFrame::OnRecalculateFrameSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
{
	int nEmoticonsToShow = (int)wParam;
	
	if (nEmoticonsToShow)
	{
		RecalculateFrameSize(nEmoticonsToShow);
	}
	
	return 0L;
}


CRgnHandle CEmoticonsFrame::GetClippingRgn(const CRect& rectWindow) const
{	
	CRgnHandle hClippingRgn;

	if (m_bCustomRgnEnabled)
	{
		int cySystemCaption = ::GetSystemMetrics(SM_CYCAPTION);
		int cxSystemBorder = ::GetSystemMetrics(SM_CXSIZEFRAME);
		int cySystemBorder = ::GetSystemMetrics(SM_CYSIZEFRAME);
		
		int l = cxSystemBorder;
		int t = cySystemCaption + cySystemBorder;

		if (m_pMenu.get())
		{
			t += ::GetSystemMetrics(SM_CYMENU);
		}
		

		CRect rectNoFramesWindow = rectWindow;		
		rectNoFramesWindow += CPoint(l, t);	
									   
		hClippingRgn.CreateRectRgn(rectNoFramesWindow.left, 
								rectNoFramesWindow.top, 
								rectNoFramesWindow.left + rectNoFramesWindow.Width(), 
								rectNoFramesWindow.top + rectNoFramesWindow.Height());
	}
	else
	{	
		hClippingRgn = NULL;
	}

	return (HRGN) hClippingRgn;
}


BOOL CEmoticonsFrame::OnActivate(BOOL bActivate)
{
	if (!bActivate)
	{
		PostMessage(WM_CLOSE);
	}

	return __super::OnActivate(bActivate);
}


void CEmoticonsFrame::RecalculateFrameSize(int nEmoticonsToShow)
{
	int itensPerColumn = (nEmoticonsToShow / EMOTICONS_PER_LINE) + 1;
	int windowWidth = 0;
	int windowHeight = 0;
	WINDOWPLACEMENT windowPlacement;
	CSize sizeBorder;
	
	GetBorderSize(sizeBorder);
	
	windowHeight = (itensPerColumn * (EMOTICON_HEIGHT + (2 * EMOTICON_BORDER_EXTENT) + 
			EMOTICON_SPACEMENT)) + EMOTICON_SPACEMENT + sizeBorder.cy + 2;
	windowWidth = (EMOTICONS_PER_LINE * (EMOTICON_WIDTH + (2 * EMOTICON_BORDER_EXTENT) + 
			EMOTICON_SPACEMENT)) + EMOTICON_SPACEMENT + sizeBorder.cx + 3;
	
	GetWindowPlacement(&windowPlacement);
	
	windowPlacement.rcNormalPosition.right = windowPlacement.rcNormalPosition.left + windowWidth;
	windowPlacement.rcNormalPosition.top = windowPlacement.rcNormalPosition.bottom - windowHeight;
	
	SetWindowPlacement(&windowPlacement);
}

