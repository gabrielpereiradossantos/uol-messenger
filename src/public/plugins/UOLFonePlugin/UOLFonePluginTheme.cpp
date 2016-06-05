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
#include ".\uolfoneplugintheme.h"


CUOLFonePluginTheme::CUOLFonePluginTheme(void) : 
		m_initialized(FALSE)
{
	if ( (!m_initialized) && Initialize() )
	{
		m_initialized = TRUE;
	}
}


CUOLFonePluginTheme::~CUOLFonePluginTheme(void)
{
	m_brushEdit.DeleteObject();
	m_brushListBox.DeleteObject();
	m_brushButton.DeleteObject();
	m_brushStatic.DeleteObject();
	m_brushScrollBar.DeleteObject();
	m_brushWindow.DeleteObject();
}


BOOL CUOLFonePluginTheme::Initialize()
{
	// TODO: Ver uma forma de jogar essas informacoes em um arquivo XML.
	
	m_clrbrushEdit = RGB(0xFE, 0xFE, 0xFE);
	m_clrbrushListBox = RGB(0xFE, 0xFE, 0xFE);
	m_clrbrushButton = RGB(0xFE, 0xFE, 0xFE);
	m_clrbrushScrollBar = RGB(0xFE, 0xFE, 0xFE);
	m_clrbrushStatic = RGB(0xFE, 0xFE, 0xFE);
	m_clrbrushWindow = RGB(0xFE, 0xFE, 0xFE);
	m_clrbrushToolbar = RGB(0xBF, 0xDB, 0xEB);
	
	m_brushEdit.CreateSolidBrush(m_clrbrushEdit);
	m_brushListBox.CreateSolidBrush(m_clrbrushListBox);
	m_brushButton.CreateSolidBrush(m_clrbrushButton);
	m_brushStatic.CreateSolidBrush(m_clrbrushScrollBar);
	m_brushScrollBar.CreateSolidBrush(m_clrbrushStatic);
	m_brushWindow.CreateSolidBrush(m_clrbrushWindow);
	
	return TRUE;
}


CUOLFonePluginTheme* CUOLFonePluginTheme::GetInstance()
{
	static CUOLFonePluginTheme configInstance;
	
	return &configInstance;
}


COLORREF CUOLFonePluginTheme::GetEditColor()
{
	return m_clrbrushEdit;
}


COLORREF CUOLFonePluginTheme::GetListBoxColor()
{
	return m_clrbrushListBox;
}


COLORREF CUOLFonePluginTheme::GetButtonColor()
{
	return m_clrbrushButton;
}


COLORREF CUOLFonePluginTheme::GetScrollBarColor()
{
	return m_clrbrushScrollBar;
}


COLORREF CUOLFonePluginTheme::GetStaticColor()
{
	return m_clrbrushStatic;
}


COLORREF CUOLFonePluginTheme::GetWindowColor()
{
	return m_clrbrushWindow;
}


COLORREF CUOLFonePluginTheme::GetToolbarColor()
{
	return m_clrbrushToolbar;
}


HBRUSH CUOLFonePluginTheme::GetEditBrush()
{
	return m_brushEdit.m_hBrush;
}


HBRUSH CUOLFonePluginTheme::GetListBoxBrush()
{
	return m_brushListBox.m_hBrush;
}


HBRUSH CUOLFonePluginTheme::GetButtonBrush()
{
	return m_brushButton.m_hBrush;
}


HBRUSH CUOLFonePluginTheme::GetScrollBarBrush()
{
	return m_brushScrollBar.m_hBrush;
}


HBRUSH CUOLFonePluginTheme::GetStaticBrush()
{
	return m_brushStatic.m_hBrush;
}


HBRUSH CUOLFonePluginTheme::GetWindowBrush()
{
	return m_brushWindow.m_hBrush;
}

