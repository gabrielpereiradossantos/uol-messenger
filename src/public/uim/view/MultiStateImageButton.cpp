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
 
#include "stdafx.h"
#include "MultiStateImageButton.h"

#include "ButtonStateInfoBuilder.h"


CMultiStateImageButton::CMultiStateImageButton(HWND hWndParent) :
	CImageButton(hWndParent)
{
}


CMultiStateImageButton::~CMultiStateImageButton()
{
}


CString CMultiStateImageButton::GetCaption() const
{
	return CImageButton::GetCaption();
}


void CMultiStateImageButton::SetCaption(const CString& strCaption)
{
	CImageButton::SetCaption(strCaption);
}


CString CMultiStateImageButton::GetTip() const
{
	return CImageButton::GetTip();
}


void CMultiStateImageButton::SetTip(const CString& strTip)
{
	CImageButton::SetTip(strTip);
}


CString CMultiStateImageButton::GetCommandName() const
{
	return CImageButton::GetCommandName();
}


void CMultiStateImageButton::SetCommandName(const CString& strCommandName)
{
	CImageButton::SetCommandName(strCommandName);
}


CString CMultiStateImageButton::GetDragAndDropCommandName() const
{
	return CImageButton::GetDragAndDropCommandName();
}


void CMultiStateImageButton::SetDragAndDropCommandName(const CString& strCommandName)
{
	CImageButton::SetDragAndDropCommandName(strCommandName);
}


int CMultiStateImageButton::GetWidth() const
{
	return CImageButton::GetWidth();
}


void CMultiStateImageButton::SetWidth(int nWidth)
{
	CImageButton::SetWidth(nWidth);
}


BOOL CMultiStateImageButton::IsPressed() const
{
	return CImageButton::IsPressed();
}


void CMultiStateImageButton::SetPressed(BOOL bPressed)
{
	CImageButton::SetPressed(bPressed);
}


BOOL CMultiStateImageButton::IsPushButton() const
{
	return CImageButton::IsPushButton();
}


void CMultiStateImageButton::SetPushButton(BOOL bPushButton)
{
	CImageButton::SetPushButton(bPushButton);
}


BOOL CMultiStateImageButton::GetDrawTransparent() const
{
	return CImageButton::GetDrawTransparent();
}


void CMultiStateImageButton::SetDrawTransparent(BOOL bTransparent)
{
	CImageButton::SetDrawTransparent(bTransparent);
}


BOOL CMultiStateImageButton::IsAuto() const
{
	return CImageButton::IsAuto();
}


void CMultiStateImageButton::SetAuto(BOOL bAuto)
{
	CImageButton::SetAuto(bAuto);
}


BOOL CMultiStateImageButton::IsVisible() const
{
	return CImageButton::IsVisible();
}


void CMultiStateImageButton::SetVisible(BOOL bVisible)
{
	CImageButton::SetVisible(bVisible);
}


IUOLMessengerImagePtr CMultiStateImageButton::GetImage() const
{
	return CImageButton::GetImage();
}


void CMultiStateImageButton::SetImage(IUOLMessengerImagePtr pImage)
{
	CImageButton::SetImage(pImage);
}


void CMultiStateImageButton::AddEventListener(CUOLMessengerImageButtonEventListener2* pListener)
{
	CImageButton::AddEventListener(pListener);
}


void CMultiStateImageButton::RemoveEventListener(CUOLMessengerImageButtonEventListener2* pListener)
{
	CImageButton::RemoveEventListener(pListener);
}


void CMultiStateImageButton::SetActiveState(const CString& strStateName)
{
	if (m_strActiveState.CompareNoCase(strStateName))
	{
		CButtonStateInfoMap::CPair* pPair = m_mapStateInfo.Lookup(strStateName);

		if (pPair)
		{
			IUOLMessengerButtonStateInfoPtr pDefaultStateInfo = pPair->m_value;

			SetTip(pDefaultStateInfo->GetTip());
			SetDrawTransparent(pDefaultStateInfo->GetDrawTransparent());

			IUOLMessengerImagePtr pImage = pDefaultStateInfo->GetImage();

			SetImage(pImage);

			m_strActiveState = pDefaultStateInfo->GetStateName();
		}
	}
}


CString CMultiStateImageButton::GetActiveState()
{
	return m_strActiveState;
}


void CMultiStateImageButton::AddStateInfo(IUOLMessengerButtonStateInfoPtr pButtonStateInfo)
{
	CString strStateName = pButtonStateInfo->GetStateName().MakeLower();

	CButtonStateInfoMap::CPair* pPair = m_mapStateInfo.Lookup(strStateName);

	if (!pPair)
	{
		m_mapStateInfo.SetAt(strStateName, pButtonStateInfo);
	}
}


IUOLMessengerButtonStateInfoPtr CMultiStateImageButton::RemoveStateInfo(const CString& strStateName)
{
	IUOLMessengerButtonStateInfoPtr pButtonStateInfo;

	CString strRemoveState = strStateName;
	strRemoveState.MakeLower();

	CButtonStateInfoMap::CPair* pPair = m_mapStateInfo.Lookup(strRemoveState);

	if (pPair)
	{
		pButtonStateInfo = pPair->m_value;

		m_mapStateInfo.RemoveKey(strRemoveState);
	}

	return pButtonStateInfo;
}


IUOLMessengerButtonStateInfoPtr CMultiStateImageButton::GetStateInfo(const CString& strStateName)
{
	IUOLMessengerButtonStateInfoPtr pButtonStateInfo;

	CString strRemoveState = strStateName;
	strRemoveState.MakeLower();

	CButtonStateInfoMap::CPair* pPair = m_mapStateInfo.Lookup(strRemoveState);

	if (pPair)
	{
		pButtonStateInfo = pPair->m_value;
	}

	return IUOLMessengerButtonStateInfoPtr();
}


void CMultiStateImageButton::GetStateInfoList(CAtlList<IUOLMessengerButtonStateInfoPtr>& listStateInfos)
{
	CButtonStateInfoMap::CPair* pPair;

	for (POSITION pos = m_mapStateInfo.GetStartPosition(); pos; )
	{
		pPair = m_mapStateInfo.GetNext(pos);

		listStateInfos.AddTail(pPair->m_value);
	}
}


void CMultiStateImageButton::AddElement(IElementPtr pElement)
{
	IUOLMessengerButtonStateInfoPtr pDefaultStateInfo;

	CButtonStateInfoBuilderPtr pBuilder = ap_dynamic_cast<CButtonStateInfoBuilderPtr>(pElement);
	ATLASSERT(pBuilder.get());

	if (pBuilder)
	{
		IUOLMessengerButtonStateInfoPtr pButtonStateInfo = ap_dynamic_cast<CButtonStateInfoPtr>(pBuilder->CreateElement());

		if (pButtonStateInfo)
		{
			CString strStateName = pButtonStateInfo->GetStateName();

			if (!strStateName.IsEmpty())
			{
				CButtonStateInfoMap::CPair* pPair = m_mapStateInfo.Lookup(strStateName);

				if (!pPair)
				{
					m_mapStateInfo.SetAt(strStateName, pButtonStateInfo);

					if (0 == strStateName.CompareNoCase(m_strDefaultState))
					{
						pDefaultStateInfo = pButtonStateInfo;
					}
				}
			}
		}
	}

	if ((!pDefaultStateInfo) && (!m_strDefaultState.IsEmpty()))
	{
		POSITION posFirst = m_mapStateInfo.GetStartPosition();

		if (posFirst)
		{
			pDefaultStateInfo = m_mapStateInfo.GetValueAt(posFirst);
		}
	}

	if (pDefaultStateInfo)
	{
		IUOLMessengerImagePtr pImage = pDefaultStateInfo->GetImage();

		SetImage(pImage);
		m_strActiveState = pDefaultStateInfo->GetStateName();
		SetTip(pDefaultStateInfo->GetTip());
		SetDrawTransparent(pDefaultStateInfo->GetDrawTransparent());

		if ((m_nWidth == 0) && (pImage != NULL))
		{
			m_nWidth = pImage->GetWidth();
		}
	}

	CImageButton::AddElement(pElement);
}


void CMultiStateImageButton::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	try
	{
		GetParameterValue(m_nWidth, pMapParams, "width");			
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "width");
	}

	try
	{
		CString strPushButton;
		GetParameterValue(strPushButton, pMapParams, "pushbutton");	

		m_bPushButton = (strPushButton == '1') || (0 == strPushButton.CompareNoCase("true"));
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "pushbutton");
		m_bPushButton = FALSE;
	}

	try
	{
		GetParameterValue(m_strDefaultState, pMapParams, "defaultstate");
		m_strActiveState = m_strDefaultState;
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "defaultstate");
		m_strDefaultState = "";
	}

	GetParameterValue(m_strCommandName, pMapParams, "command");	

	try
	{
		GetParameterValue(m_strDragAndDropCommand, pMapParams, "dndcommand");	
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "dndcommand");
		m_strDragAndDropCommand = _T("");
	}
}