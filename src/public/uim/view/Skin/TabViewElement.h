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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  SeBe the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Universo Online S/A - A/C: UOL Messenger 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */
 
// TabView.h : interface of the CTabView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Element.h"
#include "../image/image.h"
#include "../Image/imageBuilder.h"


class CTabViewLeftImageElement;
class CTabViewFillImageElement;
class CTabViewRightImageElement;


MAKEAUTOPTR(CTabViewLeftImageElement);
MAKEAUTOPTR(CTabViewFillImageElement);
MAKEAUTOPTR(CTabViewRightImageElement);



class CTabViewButtonElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("TabButton");
	CTabViewButtonElement();
	CTabViewLeftImageElementPtr GetLeftImage();
	CTabViewFillImageElementPtr GetFillImage();
	CTabViewRightImageElementPtr GetRightImage();
	virtual ~CTabViewButtonElement();
protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	void OnElementBuilt();
	CTabViewLeftImageElementPtr m_pLeftImageElement;
	CTabViewFillImageElementPtr m_pFillImageElement;
	CTabViewRightImageElementPtr m_pRightImageElement;
};

MAKEAUTOPTR(CTabViewButtonElement);


class CTabViewLeftImageElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("LeftImage");
	CTabViewLeftImageElement();
	virtual ~CTabViewLeftImageElement();
	ATL::CImage* GetNormal()	{return m_pNormal;}
	ATL::CImage* GetOver()		{return m_pOver;}
	ATL::CImage* GetClicked()	{return m_pClicked;}
	ATL::CImage* GetSelected()	{return m_pSelected;}
	ATL::CImage* GetBlinking()	{return m_pBlinking;} 

protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	ATL::CImage*	m_pNormal;
	ATL::CImage*	m_pOver;
	ATL::CImage*	m_pClicked;
	ATL::CImage*	m_pSelected;
	ATL::CImage*	m_pBlinking;
};


class CTabViewFillImageElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("FillImage");
	CTabViewFillImageElement();
	virtual ~CTabViewFillImageElement();
	IUOLMessengerImagePtr GetNormal()	{return m_pNormal;}
	IUOLMessengerImagePtr GetOver()		{return m_pOver;}
	IUOLMessengerImagePtr GetClicked()	{return m_pClicked;}
	IUOLMessengerImagePtr GetSelected() {return m_pSelected;}
	IUOLMessengerImagePtr GetBlinking() {return m_pBlinking;} 

protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	IUOLMessengerImagePtr	m_pNormal;
	IUOLMessengerImagePtr	m_pOver;
	IUOLMessengerImagePtr	m_pClicked;
	IUOLMessengerImagePtr	m_pSelected;
	IUOLMessengerImagePtr	m_pBlinking;
};


class CTabViewRightImageElement : public CElement
{
public:
	DECLARE_ELEMENT_NAME("RightImage");
	CTabViewRightImageElement();
	virtual ~CTabViewRightImageElement();

	ATL::CImage* GetNormal()	{return m_pNormal;}
	ATL::CImage* GetOver()		{return m_pOver;}
	ATL::CImage* GetClicked()	{return m_pClicked;}
	ATL::CImage* GetSelected()	{return m_pSelected;}
	ATL::CImage* GetBlinking()	{return m_pBlinking;} 

protected:
	void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable);
	ATL::CImage*	m_pNormal;
	ATL::CImage*	m_pOver;
	ATL::CImage*	m_pClicked;
	ATL::CImage*	m_pSelected;
	ATL::CImage*	m_pBlinking;
};
