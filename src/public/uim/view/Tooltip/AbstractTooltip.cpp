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
#include "AbstractTooltip.h"

#include "../Image/image.h"


CAbstractTooltip::CAbstractTooltip() :
	m_nTitleHeight(TM_DEFAULT_TITLE_HEIGHT)
{
}


CAbstractTooltip::~CAbstractTooltip()
{
}


void CAbstractTooltip::SetTooltipProvider(IUOLMessengerTooltipProviderPtr pTooltipProvider)
{
	m_pTooltipProvider = pTooltipProvider;
}


void CAbstractTooltip::Show(HWND hWndParent, const CPoint& ptScreenBase, UINT nKillTimeout)
{
	// Calculate tooltip position...
	CPoint ptTooltipPos = ptScreenBase;

	CSize sizeBorder(0, 0);

	if (m_pBorder.get())
	{
		m_pBorder->GetSize(sizeBorder);
	}

	int nXTooltipSize = TM_DEFAULT_WIDTH;
	int nYTooltipSize = TM_DEFAULT_HEIGHT + sizeBorder.cy + m_nTitleHeight;

	CRect rectDesktop;
	::GetClientRect(::GetDesktopWindow(), &rectDesktop);

	if (ptTooltipPos.x > (rectDesktop.right/2))
	{
		ptTooltipPos.x = ptTooltipPos.x - nXTooltipSize;
		ptTooltipPos.x -= 5;
	}
	else
	{
		ptTooltipPos.x += 5;
	}

	if (ptTooltipPos.y > (rectDesktop.bottom/2))
	{
		ptTooltipPos.y = ptTooltipPos.y - nYTooltipSize;
		ptTooltipPos.y -= 5;
	}
	else
	{
		ptTooltipPos.y += 5;
	}

	// Show tooltip without activating window
	SetWindowPos(NULL, ptTooltipPos.x, ptTooltipPos.y, nXTooltipSize, nYTooltipSize, SWP_NOZORDER | SWP_NOACTIVATE);
	//SetWindowPos(HWND_TOPMOST, ptTooltipPos.x, ptTooltipPos.y, nXTooltipSize, nYTooltipSize, SWP_NOACTIVATE);
	ShowWindow(SW_SHOWNOACTIVATE);

	// Start tooltip killer timer
	SetTimer(TIMER_KILL_TOOLTIP, nKillTimeout, NULL);
}


void CAbstractTooltip::OnDestroy()
{
}


void CAbstractTooltip::OnTimer(UINT nTimerId, TIMERPROC timerProc)
{
	if (TIMER_KILL_TOOLTIP == nTimerId)
	{
		KillTimer(TIMER_KILL_TOOLTIP);
		DestroyWindow();
	}
}


void CAbstractTooltip::DoPaint(CDCHandle& dcHandle)
{
	int oldBk = dcHandle.SetBkMode(TRANSPARENT);

	CRect rectClient;
	GetClientRect(&rectClient);

	CSize sizeBorder;
	sizeBorder.SetSize(0,0);

	// Draw window border
	if (m_pBorder.get())
	{
		m_pBorder->GetSize(sizeBorder);

		CBrush brush;
		brush.CreateSolidBrush(m_pBorder->GetCOLORREF());

		dcHandle.FillRect(&rectClient, brush);

		rectClient.left += sizeBorder.cx;
		rectClient.right -= sizeBorder.cx;
		rectClient.top += sizeBorder.cy;
		rectClient.bottom -= sizeBorder.cy;
	}

	// Draw window background
	m_pActiveBackground->Draw(dcHandle, rectClient);
	dcHandle.SetBkMode(oldBk);

	// Draw window title background...
	CRect rectTitle = rectClient;
	rectTitle.bottom = sizeBorder.cy + m_nTitleHeight;
	
	CBrush brush;
	brush.CreateSolidBrush(m_clrTitle);

	dcHandle.FillRect(&rectTitle, brush);

	// Draw text and image...
	if (m_pTooltipProvider)
	{
		// Draw window title...
		rectTitle.left += TM_HORIZONTAL_TEXT_SPACING;

		CString strTitle = m_pTooltipProvider->GetTitle();

		if (m_pFontTitle.get())
		{
			m_pFontTitle->DrawText(dcHandle, rectTitle, strTitle, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
		}
		else
		{
			dcHandle.DrawText(strTitle, strTitle.GetLength(), &rectTitle, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
		}

		rectClient.top += m_nTitleHeight + TM_INTERNAL_BORDER;
		rectClient.bottom -= TM_INTERNAL_BORDER;
		rectClient.left += TM_INTERNAL_BORDER;
		rectClient.right -= TM_INTERNAL_BORDER;

		// Draw image...
		IUOLMessengerImagePtr pImage = m_pTooltipProvider->GetImage();
		
		if (pImage != NULL)
		{
			IUOLMessengerImageDrawStrategyPtr pStrategy = new UOLMessengerImage::CDrawStretchOpaqueStrategy();
			int nImageHeight = rectClient.bottom - rectClient.top;

			pImage->Draw(dcHandle, 
						 rectClient.left, rectClient.top, 
						 nImageHeight, nImageHeight,
						 pStrategy);

			// Adjust text left position
			rectClient.left += nImageHeight + TM_HORIZONTAL_TEXT_SPACING;
		}
		else
		{
			// Adjust text left position
			rectClient.left += TM_HORIZONTAL_TEXT_SPACING;
		}

		rectClient.bottom = rectClient.top + TM_TEXT_LINE_HEIGHT / 2;

		// Draw text lines...
		CAtlList<CString> listLines;
		m_pTooltipProvider->GetTextLines(listLines);

		int nLineCount = 0;

		for (POSITION pos = listLines.GetHeadPosition(); (nLineCount < MAX_TEXT_LINES) && pos; )
		{
			CString strTextLine = listLines.GetNext(pos);

			if (m_pFontBody.get())
			{
				m_pFontBody->DrawText(dcHandle, rectClient, strTextLine, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			}
			else
			{
				dcHandle.DrawText(strTextLine, strTextLine.GetLength(), &rectClient, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX);
			}

			rectClient.bottom += TM_TEXT_LINE_HEIGHT;
			++nLineCount;
		}

		listLines.RemoveAll();
	}
}


void CAbstractTooltip::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	try
	{
		CString strBorder;
		GetParameterValue(strBorder, pMapParams, "border");
		
		m_pBorder = ap_dynamic_cast<CBorderElementPtr>(pElementTable->GetElement(strBorder));
		ATLASSERT(m_pBorder.get());			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "border");
	}

	try
	{
		CString strFontBody;
		GetParameterValue(strFontBody, pMapParams, "bodyfont");
		
		m_pFontBody = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFontBody));
		ATLASSERT(m_pFontBody.get());			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "bodyfont");
	}

	try
	{
		CString strFontTitle;
		GetParameterValue(strFontTitle, pMapParams, "titlefont");
		
		m_pFontTitle = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFontTitle));
		ATLASSERT(m_pFontTitle.get());			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "titlefont");
	}

	try
	{
		GetParameterValue(m_nTitleHeight, pMapParams, "titleheight");

		if (m_nTitleHeight <= 0)
		{
			m_nTitleHeight = TM_DEFAULT_TITLE_HEIGHT;
		}
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "titleheight");
	}

	try
	{
		CString str;
		GetParameterValue(str, pMapParams, "titlecolor");

		m_clrTitle.SetColor(str);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "titlecolor");
	}
}
