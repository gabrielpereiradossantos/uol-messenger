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
 
// TabView.cpp : implementation of the CTabView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TabViewElement.h"


//////////////////////////////////////////////////////////////////////////
// CTabViewButtonElement
//////////////////////////////////////////////////////////////////////////


CTabViewButtonElement::CTabViewButtonElement(void)
{
}

CTabViewButtonElement::~CTabViewButtonElement(void)
{
}

CTabViewLeftImageElementPtr CTabViewButtonElement::GetLeftImage()
{
	return m_pLeftImageElement;
}

CTabViewFillImageElementPtr CTabViewButtonElement::GetFillImage()
{
	return  m_pFillImageElement;
}

CTabViewRightImageElementPtr CTabViewButtonElement::GetRightImage()
{
	return  m_pRightImageElement;
}

void CTabViewButtonElement::OnElementBuilt()
{
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos; )
	{
		CElementPtr pElement = ap_dynamic_cast<CElementPtr>(m_listChilds.GetNext(pos));
		CTabViewLeftImageElementPtr LeftImageElement = ap_dynamic_cast<CTabViewLeftImageElementPtr>(pElement);;
		if (LeftImageElement) {
			m_pLeftImageElement = LeftImageElement;
			continue;
		}

		CTabViewFillImageElementPtr FillImageElement = ap_dynamic_cast<CTabViewFillImageElementPtr>(pElement);;
		if (FillImageElement) {
			m_pFillImageElement = FillImageElement;
			continue;
		}

		CTabViewRightImageElementPtr RightImageElement = ap_dynamic_cast<CTabViewRightImageElementPtr>(pElement);;
		if (RightImageElement) {
			m_pRightImageElement = RightImageElement;
			continue;
		}
	}
}

void CTabViewButtonElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
}




//////////////////////////////////////////////////////////////////////////
// CTabViewLeftImageElement
//////////////////////////////////////////////////////////////////////////

CTabViewLeftImageElement::CTabViewLeftImageElement(void)
{
	m_pNormal=NULL;
	m_pOver=NULL;
	m_pClicked=NULL;
	m_pSelected=NULL;
	m_pBlinking=NULL;
}

CTabViewLeftImageElement::~CTabViewLeftImageElement(void)
{
	if (m_pNormal)
	{
		m_pNormal->Destroy();

		delete m_pNormal;
	}
	
	if (m_pOver)
	{
		m_pOver->Destroy();

		delete m_pOver;
	}

	if (m_pClicked)
	{
		m_pClicked->Destroy();
		
		delete m_pClicked;
	}

	if (m_pSelected)
	{
		m_pSelected->Destroy();

		delete m_pSelected;
	}

	if (m_pBlinking)
	{
		m_pBlinking->Destroy();

		delete m_pBlinking;
	}
}


void CTabViewLeftImageElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	CString strImageCaption;
	GetParameterValue(strImageCaption, pMapParams, "Normal");
	m_pNormal = UOLMessengerImage::CImageBuilder::LoadImage2(strImageCaption);
	ATLASSERT(m_pNormal);

	GetParameterValue(strImageCaption, pMapParams, "Over");
	m_pOver = UOLMessengerImage::CImageBuilder::LoadImage2(strImageCaption);
	ATLASSERT(m_pOver);

	GetParameterValue(strImageCaption, pMapParams, "Clicked");
	m_pClicked = UOLMessengerImage::CImageBuilder::LoadImage2(strImageCaption);
	ATLASSERT(m_pClicked);

	GetParameterValue(strImageCaption, pMapParams, "Selected");
	m_pSelected = UOLMessengerImage::CImageBuilder::LoadImage2(strImageCaption);
	ATLASSERT(m_pSelected);

	GetParameterValue(strImageCaption, pMapParams, "Blinking");
	m_pBlinking = UOLMessengerImage::CImageBuilder::LoadImage2(strImageCaption);
	ATLASSERT(m_pBlinking);
}

//////////////////////////////////////////////////////////////////////////
// CTabViewFillImageElement
//////////////////////////////////////////////////////////////////////////

CTabViewFillImageElement::CTabViewFillImageElement(void)
{
}

CTabViewFillImageElement::~CTabViewFillImageElement(void)
{
}


void CTabViewFillImageElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	CString strImageCaption;
	GetParameterValue(strImageCaption, pMapParams, "Normal");
	m_pNormal = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pNormal.get());

	GetParameterValue(strImageCaption, pMapParams, "Over");
	m_pOver = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pOver.get());

	GetParameterValue(strImageCaption, pMapParams, "Clicked");
	m_pClicked = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pClicked.get());

	GetParameterValue(strImageCaption, pMapParams, "Selected");
	m_pSelected = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pSelected.get());

	GetParameterValue(strImageCaption, pMapParams, "Blinking");
	m_pBlinking= UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pBlinking.get());
}



//////////////////////////////////////////////////////////////////////////
// CTabViewCenterImageElement
//////////////////////////////////////////////////////////////////////////

CTabViewRightImageElement::CTabViewRightImageElement(void)
{
	m_pNormal=NULL;
	m_pOver=NULL;
	m_pClicked=NULL;
	m_pSelected=NULL;
	m_pBlinking=NULL;
}

CTabViewRightImageElement::~CTabViewRightImageElement(void)
{
	delete m_pNormal;
	delete m_pOver;
	delete m_pClicked;
	delete m_pSelected;
	delete m_pBlinking;
}


void CTabViewRightImageElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	CString strImageCaption;
	GetParameterValue(strImageCaption, pMapParams, "Normal");
	m_pNormal = UOLMessengerImage::CImageBuilder::LoadImage2(strImageCaption);
	ATLASSERT(m_pNormal);

	GetParameterValue(strImageCaption, pMapParams, "Over");
	m_pOver = UOLMessengerImage::CImageBuilder::LoadImage2(strImageCaption);
	ATLASSERT(m_pOver);

	GetParameterValue(strImageCaption, pMapParams, "Clicked");
	m_pClicked = UOLMessengerImage::CImageBuilder::LoadImage2(strImageCaption);
	ATLASSERT(m_pClicked);

	GetParameterValue(strImageCaption, pMapParams, "Selected");
	m_pSelected = UOLMessengerImage::CImageBuilder::LoadImage2(strImageCaption);
	ATLASSERT(m_pSelected);

	GetParameterValue(strImageCaption, pMapParams, "Blinking");
	m_pBlinking= UOLMessengerImage::CImageBuilder::LoadImage2(strImageCaption);
	ATLASSERT(m_pBlinking);

}

