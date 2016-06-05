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
#include ".\autoresizestatic.h"


CAutoResizeStatic::CAutoResizeStatic(void) : 
		m_maxWidth(0), 
		m_maxHeight(0), 
		m_height(0), 
		m_width(0)
{
}


CAutoResizeStatic::~CAutoResizeStatic(void)
{
}


void CAutoResizeStatic::SetText(LPCTSTR labelText)
{
	m_windowText = labelText;
	
	CalculateComponentSize();
}


void CAutoResizeStatic::GetText(CString& labelText)
{
	labelText = m_windowText;
}


void CAutoResizeStatic::SetMaxWidth(int width)
{
	if (width > 0)
	{
		m_maxWidth = width;
		m_maxHeight = 0;
	}
	
	CalculateComponentSize(FALSE);
}


void CAutoResizeStatic::SetMaxHeight(int height)
{
	if (height > 0)
	{
		m_maxHeight = height;
	}
	
	CalculateComponentSize();
}


int CAutoResizeStatic::GetHeight()
{
	return m_height;
}


int CAutoResizeStatic::GetWidth()
{
	return m_width;
}


LRESULT CAutoResizeStatic::OnSetMaxWidth(
		UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetMaxWidth((int)wParam);
	
	return 0L;
}


LRESULT CAutoResizeStatic::OnGetSize(
		UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	if (lParam == NULL)
	{
		return -1;
	}
	
	CSize* componentSize = (CSize*)lParam;
	componentSize->cx = GetWidth();
	componentSize->cy = GetHeight();
	
	return 0L;
}


void CAutoResizeStatic::CalculateComponentSize(BOOL showText)
{
	CString processingText;
	CString tempText;
	CSize textSize;
	CString finalWindowText;
	
	if (m_maxWidth == 0)
	{
		return;
	}
	
	processingText = m_windowText;
	finalWindowText = m_windowText;
	
	GetTextLineSize(processingText, textSize);
	
	if (textSize.cx <= m_maxWidth)
	{
		m_width = textSize.cx;
		m_height = textSize.cy;
	}
	else if (processingText.Find(' ') == -1)
	{
		m_width = m_maxWidth;
		m_height = textSize.cy;
	}
	else
	{
		m_width = m_maxWidth;
		m_height = 0;
		
		while (processingText.GetLength() > 0)
		{
			GetMaxTextForLine(processingText, tempText);
			
			processingText.Delete(0, tempText.GetLength());
			processingText.TrimLeft();
			
			m_height += textSize.cy;
			
			if ( (m_maxHeight != 0) && ( (m_height + textSize.cy) > m_maxHeight ) )
			{
				break;
			}
		}
		
		if (processingText.GetLength() > 0)
		{
			// Remove the remainder text that doesn't fit.
			finalWindowText.Delete(
					finalWindowText.GetLength() - processingText.GetLength(), 
					processingText.GetLength());
			finalWindowText.TrimRight();
			
			// Change the final characters by "...".
			finalWindowText.Delete(finalWindowText.GetLength() - 3, 3);
			finalWindowText.Append("...");
		}
	}
	
	if (showText)
	{
		CString windowText;
		GetWindowText(windowText);
		
		if (windowText.CompareNoCase(finalWindowText) != 0)
		{
			SetWindowText(finalWindowText);
		}
	}
	
	Invalidate();
}


LPCTSTR CAutoResizeStatic::GetWndClassName()
{
	return _T("CAutoResizeStatic");
}


void CAutoResizeStatic::GetTextLineSize(CString text, CSize& size)
{
	CPaintDC paintDC(m_hWnd);
	paintDC.SelectFont(GetFont());
	
	::GetTextExtentPoint32(paintDC.m_ps.hdc, text.GetBuffer(), text.GetLength(), &size);
}


void CAutoResizeStatic::GetMaxTextForLine(CString totalText, CString& maxText)
{
	BOOL sizeFits = FALSE;
	CSize textSize;
	int spacePosition;
	
	maxText = totalText;
	
	while (!sizeFits)
	{
		if ((spacePosition = maxText.ReverseFind(' ')) == -1)
		{
			// A linha nao cabe...
			break;
		}
		else
		{
			GetTextLineSize(maxText, textSize);
			
			if (textSize.cx <= m_maxWidth)
			{
				break;
			}
			
			maxText.Delete(spacePosition, maxText.GetLength() - spacePosition);
		}
	}
}


