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

#include <interfaces/IUOLMessengerTooltipProvider.h>

#include "../dummywindow.h"
#include "../skin/windowelement.h"
#include "../skin/fontelement.h"
#include "../skin/borderelement.h"
#include "../../resource.h"


typedef CWinTraits<WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_TOPMOST>		CAbstractTooltipTraits;

class CAbstractTooltip;
typedef CWindowElement<CAbstractTooltip, CDummyWindow<CWindowImpl<CAbstractTooltip, CWindow, CAbstractTooltipTraits> > > _CAbstractTooltipBase;

class CAbstractTooltip : public _CAbstractTooltipBase
{
public:
	CAbstractTooltip();
	virtual ~CAbstractTooltip();

	enum { IDD = IDD_DIALOG_TOOLTIP };

	void SetTooltipProvider(IUOLMessengerTooltipProviderPtr pTooltipProvider);

	void Show(HWND hWndParent, const CPoint& ptScreenBase, UINT nKillTimeout = 10000);

protected:
	BEGIN_MSG_MAP(CAbstractTooltip)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP(_CAbstractTooltipBase)
	END_MSG_MAP()

	void OnDestroy();
	void OnTimer(UINT nTimerId, TIMERPROC timerProc);
	void DoPaint(CDCHandle& dc);

	virtual CString GetName() const = 0;
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);

protected:
	enum
	{
		TIMER_KILL_TOOLTIP = 0x350,
		MAX_TEXT_LINES = 5
	};

	enum TooltipMetrics
	{
		TM_DEFAULT_WIDTH = 300,
		TM_DEFAULT_HEIGHT = 100,
		TM_DEFAULT_TITLE_HEIGHT = 20,
		TM_TEXT_LINE_HEIGHT = 35,
		TM_HORIZONTAL_TEXT_SPACING = 6,
		TM_INTERNAL_BORDER = 2,
	};

	CBorderElementPtr		m_pBorder;
	CFontElementPtr			m_pFontBody;
	CFontElementPtr			m_pFontTitle;
	int						m_nTitleHeight;
	CCOLORREF				m_clrTitle;

	IUOLMessengerTooltipProviderPtr		m_pTooltipProvider;
};

MAKEAUTOPTR(CAbstractTooltip);