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


#include <atltypes.h>
#include <atlstr.h>
#include "membm.h"
#include "../FacadeWrapper.h"

class CRadioUOLView;

////////////////////////////////////////////////////////////////////////////
// CWidget
///////////////////////////////////////////////////////////////////////////

enum eWMPCommand{ WMPNone , WMPBuy, WMPMute, WMPChannels, WMPSearch, WMPStyles};


class CWidget
{
public:
	CWidget(void);
	virtual ~CWidget(void);
	
	// Visibilidade
    BOOL GetVisible() { return m_bVisible; }
	void SetVisible(BOOL visible) { m_bVisible = visible; }

	// Posicionamento
	void SetRect(const CRect& rect) { m_rect = rect; }
	CRect GetRect() const { return m_rect; }
	BOOL PtInRect(const CPoint& pt) { return m_rect.PtInRect(pt); }
	
	virtual void Draw(CDCHandle& /*dc*/)	{};
	virtual void OnMouseDown()	{};
	virtual void OnMouseOver()	{};
	virtual void OnMouseLeave() {};
	virtual void OnMouseUp()	{};
	virtual void OnMouseCancelClick() {};
	virtual void OnMouseClick(CPoint pt = CPoint(0,0)) {};
protected:
	BOOL m_bVisible;
	CRect m_rect;
};

////////////////////////////////////////////////////////////////////////////
// CWidgetButton
///////////////////////////////////////////////////////////////////////////

class CWidgetButton : public CWidget
{
public:
	CWidgetButton(	IUOLMessengerImagePtr pNormalBitmap,
					IUOLMessengerImagePtr pOverBitmap,			
					IUOLMessengerImagePtr pClicklBitmap,			
					IUOLMessengerImagePtr pOffBitmap,			
					eWMPCommand command = WMPNone,
					BOOL bTransparent = TRUE
					) ;
	virtual ~CWidgetButton() {};
	
	void SetHelp(const CString& help ) { m_strHelp = help; }	
	virtual void SetEnabled(BOOL enabled) { m_bEnabled = enabled; }
	
	virtual void Draw(CDCHandle& dc);
	virtual void OnMouseDown();
	virtual void OnMouseOver();
	virtual void OnMouseUp();
	virtual void OnMouseLeave();
	virtual void OnMouseCancelClick();
	virtual void OnMouseClick(CPoint pt = CPoint(0,0));
	
protected:
	virtual void DrawOver(CDCHandle& dc);
	virtual void DrawClicked(CDCHandle& dc);
	virtual void DrawNormal(CDCHandle& dc);
	virtual void DrawOff(CDCHandle& dc);

	/*CSize GetImageSize() const;*/

	eWMPCommand m_command;

	IUOLMessengerImagePtr	m_pNormalBitmap;
	IUOLMessengerImagePtr	m_pOverBitmap;
	IUOLMessengerImagePtr	m_pClickedlBitmap;
	IUOLMessengerImagePtr	m_pOffBitmap;
	
	IUOLMessengerImageDrawStrategyPtr m_pDrawStrategy;

	BOOL m_bMouseOver;
	BOOL m_bMouseClick;
	BOOL m_bEnabled;
	
	COLORREF m_clrText;
	CString m_strHelp;
};

////////////////////////////////////////////////////////////////////////////
// CWidgetCheckButton
///////////////////////////////////////////////////////////////////////////

class CWidgetCheckButton : public CWidgetButton
{
public:
	CWidgetCheckButton( IUOLMessengerImagePtr pNormalBitmap,
						IUOLMessengerImagePtr pOverBitmap,			
						IUOLMessengerImagePtr pClicklBitmap,			
						IUOLMessengerImagePtr pOffBitmap,			
						eWMPCommand command = WMPNone );
	virtual void Draw(CDCHandle& dc);
	virtual void OnMouseClick();
private:
	BOOL m_bChecked;
};

////////////////////////////////////////////////////////////////////////////
// CWidgetSong
///////////////////////////////////////////////////////////////////////////

class CWidgetSong : public CWidget
{
public:
	CWidgetSong();
	virtual ~CWidgetSong();
	
	void SetTitle(const CString& songname, const CString& duration);
	virtual void Draw(CDCHandle& dc);

private:
	COLORREF		m_clrText;
	CString			m_strSongName;
	CString			m_strDuration;
	eWMPCommand		m_command;
	BOOL			m_bEnabled;
};
