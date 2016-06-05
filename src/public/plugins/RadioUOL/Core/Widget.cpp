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

#include "widget.h"
#include "../resource.h"
#include "RadioUOLWindow.h"
#include "../UOLMessengerRadioPlugin.h"

///////////////////////////////////////////////////////////////
// CWidget
///////////////////////////////////////////////////////////////

CWidget::CWidget(void):
m_bVisible(TRUE)
{
}

CWidget::~CWidget(void)
{
}


///////////////////////////////////////////////////////////////
// CWidgetButton
///////////////////////////////////////////////////////////////

CWidgetButton::CWidgetButton(	IUOLMessengerImagePtr pNormalBitmap,
							 IUOLMessengerImagePtr pOverBitmap,			
							 IUOLMessengerImagePtr pClicklBitmap,			
							 IUOLMessengerImagePtr pOffBitmap,			
							 eWMPCommand command,
							 BOOL bTransparent ) :
m_pNormalBitmap(pNormalBitmap),
m_pOverBitmap(pOverBitmap),
m_pClickedlBitmap(pClicklBitmap),
m_pOffBitmap(pOffBitmap),
m_bMouseClick(FALSE),
m_bMouseOver(FALSE),
m_bEnabled(TRUE),
m_command(command)
{
	m_clrText = RGB(130,130,130);

	IUOLMessengerUIManagerPtr pUIManager = CFacadeWrapper::GetFacadeInstance()->GetUIManager();

	if (pUIManager)
	{
		if (bTransparent)
		{
			m_pDrawStrategy = pUIManager->CreateDrawStrategy(IUOLMessengerUIManager::IDS_DRAW_TRANSPARENT_STRETCHED);
		} 
		else
		{
			m_pDrawStrategy = pUIManager->CreateDrawStrategy(IUOLMessengerUIManager::IDS_DRAW_OPAQUE);
		}
	}

}

void CWidgetButton::Draw(CDCHandle& dc)
{
	if (m_bVisible==FALSE)
	{
		return;
	}

	if (m_bEnabled) 
	{
		if (m_bMouseOver)
		{
			if (m_bMouseClick)
			{
				DrawClicked(dc);
			}
			else
			{
				DrawOver(dc);
			}
		}
		else
		{
			DrawNormal(dc);
		}
	}
	else
	{
		DrawOff(dc);
	}
}


void CWidgetButton::DrawOver(CDCHandle& dc)
{
	m_pOverBitmap->Draw(dc, m_rect.left, m_rect.top,m_rect.Width(),m_rect.Height(), m_pDrawStrategy);
}

void CWidgetButton::DrawClicked(CDCHandle& dc)
{
	m_pClickedlBitmap->Draw(dc, m_rect.left, m_rect.top,m_rect.Width(),m_rect.Height(), m_pDrawStrategy);
}

void CWidgetButton::DrawNormal(CDCHandle& dc)
{
	m_pNormalBitmap->Draw(dc, m_rect.left, m_rect.top,m_rect.Width(),m_rect.Height(), m_pDrawStrategy);
}

void CWidgetButton::DrawOff(CDCHandle& dc)
{
	m_pOffBitmap->Draw(dc, m_rect.left, m_rect.top,m_rect.Width(),m_rect.Height(), m_pDrawStrategy);

}

void CWidgetButton::OnMouseDown()
{
	if ((m_bVisible==TRUE)&&(m_bEnabled==TRUE))
	{
		m_bMouseClick = TRUE;
	}
}

void CWidgetButton::OnMouseOver()
{
	if ((m_bVisible==TRUE)&&(m_bEnabled==TRUE))
	{
		m_bMouseOver = TRUE;
	}
}

void CWidgetButton::OnMouseUp()
{
	if ((m_bVisible==TRUE)&&(m_bEnabled==TRUE))
	{
		m_bMouseClick = FALSE;
	}
}

void CWidgetButton::OnMouseLeave()
{
	if ((m_bVisible==TRUE)&&(m_bEnabled==TRUE))
	{
		m_bMouseOver = FALSE;
	}
}


void CWidgetButton::OnMouseCancelClick()
{
	if ((m_bVisible==TRUE)&&(m_bEnabled==TRUE))
	{
		m_bMouseClick = FALSE;
	}
}

void CWidgetButton::OnMouseClick(CPoint pt )
{
	if ((m_bVisible==TRUE)&&(m_bEnabled==TRUE))
	{
		CRadioUOLWindowPtr ptr = CRadioUOLWrapper::GetFacadeInstance();
		if (ptr)
		{
			switch (m_command)
			{
			case WMPBuy:
				ptr->OnBuy();
				break;
			case WMPChannels:
				ptr->OnChannels();
				break;
			case WMPSearch:
				ptr->OnSearch();

				break;
			case WMPStyles:
				ptr->OnStyles(pt);

				break;
			default:
				break;
			}		
		}
	}
}

///////////////////////////////////////////////////////////////
// CWidgetSong
///////////////////////////////////////////////////////////////
CWidgetSong::CWidgetSong()
{
}


CWidgetSong::~CWidgetSong()
{
}


void CWidgetSong::SetTitle(const CString& songname, const CString& duration) 
{
	m_clrText = RGB(34,64,131);
	m_strSongName = songname;
	m_strDuration = duration;
}


void CWidgetSong::Draw(CDCHandle& dc)
{
	dc.FillSolidRect(m_rect, RGB(192,219,236));

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
	lf.lfHeight = 14;                      // request a 12-pixel-height font
	strcpy(lf.lfFaceName, _T("Arial"));    // request a face name "Arial"

	CLogFont logfont(lf);
	CFont font(logfont.CreateFontIndirect());
	dc.SelectFont(font);

	dc.SetTextColor(RGB(34,64,131));
	dc.SetBkMode(TRANSPARENT);

	CRect fitDursize(0, 0, 0, 0);
	CRect fitSongsize(0, 0, 0, 0);

	CRect song(0, 0, 0, 0);
	CRect dur(0, 0, 0, 0);

	dc.DrawText(m_strDuration, m_strDuration.GetLength(), &fitDursize, DT_CALCRECT  );
	dc.DrawText(m_strSongName, m_strSongName.GetLength(), &fitSongsize, DT_CALCRECT  );

	song = m_rect;
	dur = m_rect;
	int fitheight = max(fitDursize.Height(), fitSongsize.Height());
	int newtop;
	int newbottom;

	newtop	  = m_rect.top + ( (m_rect.Height() - fitheight) / 2 );
	newbottom = m_rect.bottom - ( (m_rect.Height() - fitheight) / 2 );

	dur.left = (dur.right - fitDursize.Width());
	dur.top     = song.top = newtop;
    
    song.bottom = newbottom;
	song.right =  dur.left - 6;
	song.left = song.left + 3;
	
	dc.DrawText(m_strSongName, m_strSongName.GetLength(), song, DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS  );
	dc.DrawText(m_strDuration, m_strDuration.GetLength(), dur, DT_VCENTER | DT_RIGHT  );
}


///////////////////////////////////////////////////////////////
// CWidgetCheckButton
///////////////////////////////////////////////////////////////


CWidgetCheckButton::CWidgetCheckButton(
									   IUOLMessengerImagePtr pNormalBitmap,
									   IUOLMessengerImagePtr pOverBitmap,			
									   IUOLMessengerImagePtr pClicklBitmap,			
									   IUOLMessengerImagePtr pOffBitmap,			
									   eWMPCommand command ):
CWidgetButton(pNormalBitmap, pOverBitmap, pClicklBitmap, pOffBitmap, command),
m_bChecked(FALSE)
{
	m_command = command;
}


void CWidgetCheckButton::Draw(CDCHandle& dc)
{
	if (m_bEnabled) 
	{
		if (m_bChecked==TRUE)
		{
			DrawClicked(dc);
		}
		else
		{
			if (m_bMouseOver)
			{
				if (m_bMouseClick)
				{
					DrawClicked(dc);
				}
				else
				{
					DrawOver(dc);
				}
			}
			else
			{
				DrawNormal(dc);
			}
		}
	}
	else
	{
		DrawOff(dc);
	}
}

void CWidgetCheckButton::OnMouseClick()
{
	if (m_bEnabled==TRUE)
	{
		m_bChecked = !m_bChecked ;
		switch (m_command)
		{
		case WMPMute:
			//CRadioUOLView::GetView()->OnMute();
			break;
		default:
			break;
		}
	}
}