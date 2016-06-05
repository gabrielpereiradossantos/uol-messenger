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
#include "ProtocolImageElement.h"
#include "../../controller/uimapplication.h"

//////////////////////////////////////////////////////////////////////////
// CProtocolBagListElement
//////////////////////////////////////////////////////////////////////////

CProtocolBagListElement::CProtocolBagListElement(void)
{
}

CProtocolBagListElement::~CProtocolBagListElement(void)
{
	m_listProtocolBagElement.RemoveAll();
}


void CProtocolBagListElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
}


void CProtocolBagListElement::OnElementBuilt()
{
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos; )
	{
		CElementPtr pElement = ap_dynamic_cast<CElementPtr>(m_listChilds.GetNext(pos));
		CProtocolBagElementPtr pProtocolBag = ap_dynamic_cast<CProtocolBagElementPtr>(pElement);
		if (pProtocolBag) {
			m_listProtocolBagElement.AddTail(pProtocolBag);
		}
	}
}

CString CProtocolBagListElement::GetName() const
{
	return _T("__CProtocolBagListElementInstance");
}

CProtocolBagElementPtr CProtocolBagListElement::GetProtocolBagList(const CString &protocol)
{
	// Iterate through the entire list
	POSITION myPos = m_listProtocolBagElement.GetHeadPosition();
	CProtocolBagElementPtr pTmp;

	for (POSITION myPos = m_listProtocolBagElement.GetHeadPosition();myPos;)
	{
		pTmp = m_listProtocolBagElement.GetNext(myPos);
		ATLASSERT(pTmp.get());
		CString protocolName;
		protocolName = pTmp->GetProtocolName();
		if (protocol.CompareNoCase(protocolName)==0) {
			return pTmp;
		}
	} 
	return CProtocolBagElementPtr();
}

//////////////////////////////////////////////////////////////////////////
// CProtocolBagElement
//////////////////////////////////////////////////////////////////////////

CProtocolBagElement::CProtocolBagElement(void)
{
}

CProtocolBagElement::~CProtocolBagElement(void)
{
}


CProtocolImageListElementPtr CProtocolBagElement::GetProtocolImageList()
{
	return m_ProtocolImageListElement;
}


CString CProtocolBagElement::GetProtocolName()
{
	return m_strName;
}

void CProtocolBagElement::OnElementBuilt()
{
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos; )
	{
		CElementPtr pElement = ap_dynamic_cast<CElementPtr>(m_listChilds.GetNext(pos));
		CProtocolImageListElementPtr pProtocolImageList = ap_dynamic_cast<CProtocolImageListElementPtr>(pElement);
		if (pProtocolImageList) {
			m_ProtocolImageListElement=pProtocolImageList;
		}
	}
}

void CProtocolBagElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	// Image
	CString strName;
	GetParameterValue(strName, pMapParams, "protocol");
	ATLASSERT(strName.GetLength()>0);
	m_strName = strName;

	// Image
	CString strImageCaption;
	GetParameterValue(strImageCaption, pMapParams, "image");

	m_pIcon = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pIcon.get());
}

//////////////////////////////////////////////////////////////////////////
// CProtocolImageListElement
//////////////////////////////////////////////////////////////////////////

CProtocolImageListElement::CProtocolImageListElement(void)
{
}

CProtocolImageListElement::~CProtocolImageListElement(void)
{
}


CProtocolUserIconElementPtr CProtocolImageListElement::GetProtocolUserIconElement()
{
	return  m_pUserIconElement;
}

CProtocolUserStatusElementPtr CProtocolImageListElement::GetProtocolUserStatusElement()
{
	return  m_pUserStatusElement;
}

CProtocolContactIconElementPtr CProtocolImageListElement::GetProtocolContactIconElement()
{
	return m_pContactIconElement;
}

CProtocolContactStatusElementPtr CProtocolImageListElement::GetProtocolContactStatus()
{
	return  m_pContactStatusElement;
}

CProtocolStatusButtonElementPtr CProtocolImageListElement::GetProtocolStatusButton()
{
	return m_pStatusButtonElement;
}

CProtocolContactBlockedElementPtr CProtocolImageListElement::GetProtocolContactBlocked()
{
	return  m_pContactBlockedElement;
}

CProtocolContactAuthorizationElementPtr CProtocolImageListElement::GetProtocolContactAuthorizationElement()
{
	return m_pContactAuthorizationElement;
}

void CProtocolImageListElement::OnElementBuilt()
{
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos; )
	{
		CElementPtr pElement = ap_dynamic_cast<CElementPtr>(m_listChilds.GetNext(pos));
		CProtocolUserIconElementPtr ProtocolUserIconElement = ap_dynamic_cast<CProtocolUserIconElementPtr>(pElement);
		if (ProtocolUserIconElement) {
			m_pUserIconElement = ProtocolUserIconElement;
			continue;
		}

		CProtocolUserStatusElementPtr ProtocolUserStatusElement = ap_dynamic_cast<CProtocolUserStatusElementPtr>(pElement);
		if (ProtocolUserStatusElement) {
			m_pUserStatusElement = ProtocolUserStatusElement;
			continue;
		}

		CProtocolContactIconElementPtr ProtocolContactIconElement = ap_dynamic_cast<CProtocolContactIconElementPtr>(pElement);
		if (ProtocolContactIconElement) {
			m_pContactIconElement = ProtocolContactIconElement;
			continue;
		}

		CProtocolContactStatusElementPtr ProtocolContactStatusElement = ap_dynamic_cast<CProtocolContactStatusElementPtr>(pElement);
		if (ProtocolContactStatusElement) {
			m_pContactStatusElement = ProtocolContactStatusElement;
			continue;
		}

		CProtocolStatusButtonElementPtr ProtocolStatusButtonElement = ap_dynamic_cast<CProtocolStatusButtonElementPtr>(pElement);
		if (ProtocolStatusButtonElement) {
			m_pStatusButtonElement = ProtocolStatusButtonElement;
			continue;
		}

		CProtocolContactBlockedElementPtr ProtocolContactBlockedElement = ap_dynamic_cast<CProtocolContactBlockedElementPtr>(pElement);
		if (ProtocolContactBlockedElement) {
			m_pContactBlockedElement = ProtocolContactBlockedElement;
			continue;
		}

		CProtocolContactAuthorizationElementPtr pContactAuthorizationElement = ap_dynamic_cast<CProtocolContactAuthorizationElementPtr>(pElement);
		if (pContactAuthorizationElement)
		{
			m_pContactAuthorizationElement = pContactAuthorizationElement;
			continue;
		}
	}
}

void CProtocolImageListElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
}

//////////////////////////////////////////////////////////////////////////
// CProtocolUserIconElement
//////////////////////////////////////////////////////////////////////////

CProtocolUserIconElement::CProtocolUserIconElement(void)
{
}

CProtocolUserIconElement::~CProtocolUserIconElement(void)
{
}


void CProtocolUserIconElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	// Image
	CString strImageCaption;
	GetParameterValue(strImageCaption, pMapParams, "image");

	IUOLMessengerUIManager3Ptr pUIManager3 = ap_dynamic_cast<IUOLMessengerUIManager3Ptr>(CUIMApplication::GetApplication()->GetUIManager());

	if (pUIManager3)
	{
		IUOLMessengerThemePropertiesPtr pThemeProperties = pUIManager3->GetCurrentThemeProperties();

		if (pThemeProperties)
		{
			m_pathIcon = pThemeProperties->GetDirPath() + _T("\\") + strImageCaption;
		}
	}

	if (m_pathIcon.IsEmpty())
	{
		m_pathIcon = strImageCaption;
	}
	
	m_pIcon = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pIcon.get());
}

//////////////////////////////////////////////////////////////////////////
// CProtocolUserStatusElement
//////////////////////////////////////////////////////////////////////////

CProtocolUserStatusElement::CProtocolUserStatusElement(void)
{
}

CProtocolUserStatusElement::~CProtocolUserStatusElement(void)
{
}


void CProtocolUserStatusElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	CString strImageCaption;
	GetParameterValue(strImageCaption, pMapParams, "online_image");
	m_pOnline = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pOnline.get());

	GetParameterValue(strImageCaption, pMapParams, "offline_image");
	m_pOffline = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pOffline.get());

	try
	{
		GetParameterValue(strImageCaption, pMapParams, "away_image");
		m_pAway = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "away_image");
	}

	if (!m_pAway)
	{
		m_pAway = m_pOnline;
	}

	try
	{
		GetParameterValue(strImageCaption, pMapParams, "busy_image");
		m_pBusy = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "busy_image");
	}

	if (!m_pBusy)
	{
		m_pBusy = m_pOnline;
	}

	try
	{
		GetParameterValue(strImageCaption, pMapParams, "invisible_image");
		m_pInvisible = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "invisible_image");
	}

	if (!m_pInvisible)
	{
		m_pInvisible = m_pOnline;
	}

    try
	{
		GetParameterValue(strImageCaption, pMapParams, "unknown_image");
		m_pUnknown = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "invisible_image");
	}

	if (!m_pUnknown)
	{
		m_pUnknown = m_pOnline;
	}
}

//////////////////////////////////////////////////////////////////////////
// CProtocolContactIconElement
//////////////////////////////////////////////////////////////////////////

CProtocolContactIconElement::CProtocolContactIconElement(void)
{
}

CProtocolContactIconElement::~CProtocolContactIconElement(void)
{
}

void CProtocolContactIconElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	// Image
	CString strImageCaption;
	GetParameterValue(strImageCaption, pMapParams, "image");
	
	IUOLMessengerUIManager3Ptr pUIManager3 = ap_dynamic_cast<IUOLMessengerUIManager3Ptr>(CUIMApplication::GetApplication()->GetUIManager());

	if (pUIManager3)
	{
		IUOLMessengerThemePropertiesPtr pThemeProperties = pUIManager3->GetCurrentThemeProperties();

		if (pThemeProperties)
		{
			m_pathIcon = pThemeProperties->GetDirPath() + _T("\\") + strImageCaption;
		}
	}

	if (m_pathIcon.IsEmpty())
	{
		m_pathIcon = strImageCaption;
	}
	
	m_pIcon = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pIcon.get());

}

//////////////////////////////////////////////////////////////////////////
// CProtocolContactStatusElement
//////////////////////////////////////////////////////////////////////////

CProtocolContactStatusElement::CProtocolContactStatusElement(void)
{
}

CProtocolContactStatusElement::~CProtocolContactStatusElement(void)
{
}


void CProtocolContactStatusElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	CString strImageCaption;
	GetParameterValue(strImageCaption, pMapParams, "online_image");
	m_pOnline = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pOnline.get());

	GetParameterValue(strImageCaption, pMapParams, "offline_image");
	m_pOffline = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pOffline.get());

	GetParameterValue(strImageCaption, pMapParams, "busy_image");
	m_pBusy = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pBusy.get());

	GetParameterValue(strImageCaption, pMapParams, "idle_image");
	m_pIdle = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pIdle.get());

	GetParameterValue(strImageCaption, pMapParams, "away_image");
	m_pAway = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pAway.get());

    GetParameterValue(strImageCaption, pMapParams, "unknown_image");
	m_pUnknown = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pUnknown.get());
}

//////////////////////////////////////////////////////////////////////////
// CProtocolStatusButtonElement
//////////////////////////////////////////////////////////////////////////

CProtocolStatusButtonElement::CProtocolStatusButtonElement(void)
{
}

CProtocolStatusButtonElement::~CProtocolStatusButtonElement(void)
{
}

void CProtocolStatusButtonElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	CString strImageCaption;
	GetParameterValue(strImageCaption, pMapParams, "online_image");
	m_pOnline = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pOnline.get());

	GetParameterValue(strImageCaption, pMapParams, "offline_image");
	m_pOffline = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pOffline.get());

	GetParameterValue(strImageCaption, pMapParams, "busy_image");
	m_pBusy = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pBusy.get());

	GetParameterValue(strImageCaption, pMapParams, "away_image");
	m_pAway = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pAway.get());

	GetParameterValue(strImageCaption, pMapParams, "invisible_image");
	m_pInvisible = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pInvisible.get());

    GetParameterValue(strImageCaption, pMapParams, "unknown_image");
	m_pUnknown = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pUnknown.get());
}

//////////////////////////////////////////////////////////////////////////
// CProtocolContactBlockedElement
//////////////////////////////////////////////////////////////////////////

CProtocolContactBlockedElement::CProtocolContactBlockedElement()
{
}

CProtocolContactBlockedElement::~CProtocolContactBlockedElement()
{
}

void CProtocolContactBlockedElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	CString strImageCaption;

	GetParameterValue(strImageCaption, pMapParams, "image");
	m_pIcon = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
	ATLASSERT(m_pIcon.get());
}

//////////////////////////////////////////////////////////////////////////
// CProtocolContactAuthorizationElement
//////////////////////////////////////////////////////////////////////////

CProtocolContactAuthorizationElement::CProtocolContactAuthorizationElement()
{
}

CProtocolContactAuthorizationElement::~CProtocolContactAuthorizationElement()
{
}

void CProtocolContactAuthorizationElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	CString strImageCaption;

	try
	{
		GetParameterValue(strImageCaption, pMapParams, "not_authorized_image");
		m_pNotAuthorizedIcon = UOLMessengerImage::CImageBuilder::LoadImage(strImageCaption);
		ATLASSERT(m_pNotAuthorizedIcon.get());
	}
	catch (const CSkinException&)
	{
	}
}
