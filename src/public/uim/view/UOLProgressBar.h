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

#include <interfaces/IUOLMessengerProgressBar.h>
#include "./Skin/ProgressBarElement.h"


class CUOLProgressBar : public IUOLMessengerProgressBar
{
public:
	CUOLProgressBar(CProgressBarElementPtr pProgressBarElement);
	virtual ~CUOLProgressBar();
	
	// IUOLMessengerProgressBar methods
	virtual void SetRect(CRect rect);
	virtual void SetRoundedCorners(int rx, int ry);
	virtual void Draw(CDCHandle& dc);
	virtual void SetRange(int nMin = 0, int nMax = 100);
	virtual void SetPos(int nPos = 0);
	virtual int GetPos();
	
	/*
	 * Returns the image painted width...
	 */
	static int DrawImageToCDC(CDCHandle& dc, int x, int y, int width, int height, HBITMAP hImage);
	
	static void GetImageSize(HBITMAP hImage, CSize& sizeImage);

protected:
	
	int GetPosPixel(CRect rect);
	
private:
	CRect		m_BorderRect;
	int			m_nRoundX;
	int			m_nRoundY;
	int			m_nMin;
	int			m_nMax;
	int			m_nPos;
	CCOLORREF	m_clrBackground;
	CCOLORREF	m_clrBorder;
	CCOLORREF	m_clrFill;
	
	HBITMAP		m_hLeftImage;
	HBITMAP		m_hMiddleImage;
	HBITMAP		m_hRightImage;
	
	BOOL		m_bIsImageProgressBar;
}; 

MAKEAUTOPTR(CUOLProgressBar)
