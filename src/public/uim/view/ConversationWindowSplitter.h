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

#include "skin/WindowElement.h"
#include "DummyWindow.h"
#include "../controller/utils.h"
#include "image/Image.h"
#include "image/ImageBuilder.h"


/////////////////////////////////////////////////
// CDummySplitterImpl Class
template <class T, bool t_bVertical = true, class TBase = ATL::CWindow, class TWinTraits = ATL::CControlWinTraits>
class ATL_NO_VTABLE CDummySplitterImpl : public ATL::CWindowImpl< T, TBase, TWinTraits >, public CSplitterImpl< T , t_bVertical >
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, COLOR_WINDOW)

	typedef CSplitterImpl< T , t_bVertical >   _baseClass;

	BEGIN_MSG_MAP(CSplitterWindowImpl)
		CHAIN_MSG_MAP(_baseClass)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
};

/////////////////////////////////////////////////
// CConversationWindowHorizontalSplitter Class
class CConversationWindowHorizontalSplitter;
typedef CWindowElement<CConversationWindowHorizontalSplitter, CDummySplitterImpl<CConversationWindowHorizontalSplitter, false> > _CConversationWindowHorizontalSplitterBase;

class CConversationWindowHorizontalSplitter : public _CConversationWindowHorizontalSplitterBase					
{
public:
	CConversationWindowHorizontalSplitter();
	virtual ~CConversationWindowHorizontalSplitter();

	DECLARE_ELEMENT_NAME(_T("ConversationWindowHorizontalSplitter"));
	DECLARE_WND_CLASS_EX("ConversationWindowHorizontalSplitter", CS_DBLCLKS, COLOR_WINDOW);

	void DoPaint(CDCHandle& dc);
	void UpdateLayout();
	void UpdateSplitterPosition();

protected:
	virtual CString GetName() const;
	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);

	BEGIN_MSG_MAP(CConversationWindowHorizontalSplitter)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClick)
		CHAIN_MSG_MAP(_CConversationWindowHorizontalSplitterBase)
        REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	void OnDestroy();
	void OnLButtonDblClick(UINT nFlags, CPoint& ptClick);

	void DrawSplitterBar(CDCHandle& dc);

private:
	int             GetDefaultSplitterPosition();
    int             m_nBottomPanelHeight;
	CStringGUID	    m_strGUID;
	UINT            m_nSplitterBarHeight;
	BOOL            m_bSplitterPositioned;	
	IUOLMessengerImagePtr	m_pCenterImage;
};

MAKEAUTOPTR(CConversationWindowHorizontalSplitter);


/////////////////////////////////////////////////
// CConversationWindowVerticalSplitter Class
class CConversationWindowVerticalSplitter;
typedef CWindowElement<CConversationWindowVerticalSplitter, CDummySplitterImpl<CConversationWindowVerticalSplitter, true> > _CConversationWindowVerticalSplitterBase;

class CConversationWindowVerticalSplitter : public _CConversationWindowVerticalSplitterBase					
{
public:
	CConversationWindowVerticalSplitter();
	virtual ~CConversationWindowVerticalSplitter();

	DECLARE_ELEMENT_NAME(_T("ConversationWindowVerticalSplitter"));
	DECLARE_WND_CLASS_EX("ConversationWindowVerticalSplitter", CS_DBLCLKS, COLOR_WINDOW);

	void DoPaint(CDCHandle& dc);
	void UpdateLayout();
    void UpdateSplitterPosition();

protected:
	virtual CString GetName() const;
	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);

	BEGIN_MSG_MAP(CConversationWindowVerticalSplitter)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClick)
		CHAIN_MSG_MAP(_CConversationWindowVerticalSplitterBase)
        REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	void OnDestroy();
	void OnLButtonDblClick(UINT nFlags, CPoint& ptClick);

	void DrawSplitterBar(CDCHandle& dc);

private:
	int             GetDefaultSplitterPosition();
    int             m_nRightPanelHeight;
	CStringGUID	    m_strGUID;
	UINT            m_nSplitterBarHeight;
	BOOL            m_bSplitterPositioned;
	IUOLMessengerImagePtr	m_pCenterImage;
};

MAKEAUTOPTR(CConversationWindowVerticalSplitter);
