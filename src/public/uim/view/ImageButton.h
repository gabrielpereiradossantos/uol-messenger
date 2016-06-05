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

#include "image/Image.h"
#include "skin/element.h"
#include "skin/elementBuilder.h"

#include "../controller/utils.h"
#include <interfaces/IUOLMessengerImageButton.h>

class CImageButton;


class CImageButtonBuilder;

class CImageButtonBase : public IUOLMessengerImageButton3
{	
public:
	CImageButtonBase(HWND hwndParent = NULL);

public:	
	virtual ~CImageButtonBase();	

	enum MouseState 
	{
		MouseOut = 0,
		MouseClick = 1,
		MouseOver = 2			
	};

	virtual CString GetCaption() const;
	virtual void SetCaption(const CString& strCaption);

	virtual CString GetTip() const;
	virtual void SetTip(const CString& strCaption);

	virtual CString GetCommandName() const;
	virtual void SetCommandName(const CString& strCommandName);

	virtual CString GetDragAndDropCommandName() const;
	virtual void SetDragAndDropCommandName(const CString& strCommandName);

	virtual int GetWidth() const;
	virtual void SetWidth(int nWidth);

	virtual BOOL IsPressed() const;
	virtual void SetPressed(BOOL bPressed);

	virtual BOOL IsPushButton() const;
	virtual void SetPushButton(BOOL bPushButton);

	virtual void GetTextRect(CRect &rect) const;
	virtual void SetTextRect(const CRect& rect);

	virtual BOOL GetDrawTransparent() const;
	virtual void SetDrawTransparent(BOOL bTransparent);

	virtual BOOL IsAuto() const;
	virtual void SetAuto(BOOL bAuto);

	virtual BOOL IsVisible() const;
	virtual void SetVisible(BOOL bVisible);

	virtual CImageButtonBase::MouseState GetState() const;
	virtual void SetState(CImageButtonBase::MouseState state);

	virtual IUOLMessengerImagePtr GetImage() const;
	virtual void SetImage(IUOLMessengerImagePtr pImage);

	virtual void Draw(CDCHandle& dc, int x, int y, int cx, int cy, MouseState state);

	virtual void AddEventListener(CUOLMessengerImageButtonEventListener2* pListener);
	virtual void RemoveEventListener(CUOLMessengerImageButtonEventListener2* pListener);

	virtual void SetAttachment(LPVOID /*lpVoid*/);
	virtual LPVOID GetAttachment();

	virtual void SetParent(HWND hWndParent);

	virtual void OnLButtonDown(const CPoint& ptClick);
	virtual void OnLButtonUp(const CPoint& ptClick);
	virtual void OnLButtonDblClick(const CPoint& ptClick);
	virtual void OnRButtonDown(const CPoint& ptClick);
	virtual void OnRButtonDblClick(const CPoint& ptClick);
	virtual void OnMouseMove(const CPoint& ptClick);

	virtual CRect GetButtonRect();
	virtual void SetButtonRect(CRect rect);
	
	// Just for internal use.
	//
	virtual void Release();
	
protected:
	void NotifyEventListeners(void (CUOLMessengerImageButtonEventListener2::*pEventHandler)(IUOLMessengerImageButton* pButton, const CPoint& ptClick), const CPoint& ptClick);

private:
	void Update();
	IUOLMessengerImageDrawStrategyPtr GetDrawStrategy() const;

protected:
	HWND						m_hWnd;

	int							m_nWidth;
	BOOL						m_bDrawTransparent;
	CString						m_strCommandName;
	CString						m_strCaption;
	CString						m_strTip;
	CString						m_strDragAndDropCommand;
	CRect						m_textRect;
	BOOL						m_bPushButton;
	BOOL						m_bIsPressed;
	BOOL						m_bAuto;
	BOOL						m_bVisible;
	HWND						m_hWndParent;

	CImageButtonBase::MouseState	m_state;

	IUOLMessengerImagePtr		m_pImage;
	
	CAtlList<CUOLMessengerImageButtonEventListener2*>	m_listListeners;

	LPVOID						m_lpAttachment;

	CRect						m_ButtonRect;
};

MAKEAUTOPTR(CImageButtonBase);



class CImageButton : public CElement,
					 public CImageButtonBase
{	
public:
	CImageButton(HWND hwndParent = NULL);

public:	
	virtual ~CImageButton();	

protected:
	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	virtual CString GetName() const;

private:
	void LoadImages(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);

protected:
	CStringGUID					m_strName;	

private:
	friend class CImageButtonBuilder;
};

MAKEAUTOPTR(CImageButton);



class CImageButtonBuilder : public CElementBuilder
{
public:
	DECLARE_ELEMENT_NAME("Button");

protected:
	virtual CElementPtr AllocElement()
	{
		CImageButtonPtr pElement = new CImageButton();

		ATLASSERT(pElement.get());

		return pElement;
	}

	virtual CString GetName() const
	{
		return m_strName;
	}

private:
	CStringGUID				m_strName;
};

MAKEAUTOPTR(CImageButtonBuilder);