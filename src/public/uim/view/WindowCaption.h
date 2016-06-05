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

#include "ImageButtonContainer.h"
#include "DummyWindow.h"
#include "skin/fontelement.h"
#include "skin/iconelement.h"
#include "WMEventNotifier.h"
#include "IWindowWithRegion.h"

#include "tooltip/ToolTipElement.h"

class CWindowCaption;
typedef CImageButtonContainer<CWindowCaption, CDummyWindow<CWindowImpl<CWindowCaption> > > _CWindowCaptionBase;

class CWindowCaption : public _CWindowCaptionBase, 
					   public CWMEventNotifier<CWindowCaption>
{
public:
	DECLARE_WND_CLASS("WindowCaption");

	CWindowCaption(void);
	virtual ~CWindowCaption(void);

	void SetText(const CString& strText);
	CString GetText() const;

	void SetActiveImage(IUOLMessengerImagePtr pImage);
	IUOLMessengerImagePtr GetActiveImage();

	void SetInactiveImage(IUOLMessengerImagePtr pImage);
	IUOLMessengerImagePtr GetInactiveImage();

	void SetDrawTransparentImage(BOOL bDrawTransparent);
	BOOL GetDrawTransparentImage() const;

	CToolTipElement GetSelectedControl(CPoint& pos);

	void SetRoundCaptionFactor(int nRoundCaptionFactor);
	void SetWindowWithRegion(IWindowWithRegion* pWindowWithRegion);

	//CIconElementPtr GetSmallIcon() const;
	//CIconElementPtr GetLargeIcon() const;

protected:
	BEGIN_MSG_MAP(CWindowCaption)
		MSG_WM_NCHITTEST(OnNcHitTest)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		MSG_WM_CREATE(OnCreate)
		CHAIN_MSG_MAP(_CWindowCaptionBase)
		CHAIN_MSG_MAP(CWMEventNotifier<CWindowCaption>)
	END_MSG_MAP()

	LRESULT OnNcHitTest(const CPoint& pt);
	void OnLButtonDown(UINT, const CPoint& pt);
	void OnLButtonDblClk(UINT, const CPoint& pt);
	void OnRButtonUp(UINT, const CPoint& pt);
	LRESULT OnCreate(LPCREATESTRUCT lpCreateStruct);

	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);		

	virtual void DrawBackground(CDCHandle& dc, CRect& rect);
	virtual void DoPaint(CDCHandle& dc);	
	virtual void UpdateLayout(BOOL bResizeBars = TRUE);
	virtual BOOL HitTest(const CPoint& pt, int& nTest) const;
	virtual CRgnHandle GetClippingRgn(const CRect& rectWindow) const;

	virtual int GetFirstButtonPosition();
	virtual int GetNextButtonPosition(int x, CImageButtonBasePtr& pButton);
	virtual int GetButtonOffset(CImageButtonBasePtr& pButton);

	virtual BOOL OnActivate(BOOL);

private:
	BOOL IsPointInSwitcher(const CPoint& pt) const;
	BOOL IsRgnEnabled() const;
	BOOL IsActive() const;

protected:
	CFontElementPtr		m_pFont;

	IUOLMessengerImagePtr	m_pActiveFrameSwitcher;
	IUOLMessengerImagePtr	m_pInactiveFrameSwitcher;
	IUOLMessengerImage*		m_pCurrentFrameSwitcher;
	
	IUOLMessengerImagePtr	m_pImageActiveCaption;
	IUOLMessengerImagePtr	m_pImageInactiveCaption;
	IUOLMessengerImage*		m_pCurrentImageCaption;
	
	BOOL			m_bShowText;
	BOOL			m_bDrawTransparentImage;

	int				m_xFirstCaptionButtonPosition;
	int				m_nRoundCaptionFactor;

	//CIconElementPtr m_pIconSmall;
	//CIconElementPtr m_pIconLarge;

private:
	enum {START_FRAME_X = 3};

	CString				m_strText;	
	IWindowWithRegion*	m_pWindowWithRegion;
	CToolTipElement		m_toolTipSwitcher;

	BOOL				m_bActive;
};
 MAKEAUTOPTR(CWindowCaption);