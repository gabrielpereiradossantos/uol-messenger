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
#include "FileTransferEntryElement.h"

#include "../IPainterRepository.h"
#include "../../controller/UIMApplication.h"
//#include <interfaces/IUOLMessengerUIManager.h>


CFileTransferEntryElement::CFileTransferEntryElement() : 
		m_hItemSelectionBkgrndImage(NULL)
{
}


CFileTransferEntryElement::~CFileTransferEntryElement()
{
	if (m_hItemSelectionBkgrndImage)
	{
		::DeleteObject(m_hItemSelectionBkgrndImage);
	}
}


IPainter* CFileTransferEntryElement::GetPainter(UINT nPainterId) const
{
	CString strPainterName;

	switch (nPainterId)
	{
	case IPainterRepository::PR_FILETRANSFER_DEFAULT_PAINTER:
		strPainterName = "__FileTransferDefaultPainterInstance_";
		break;

	case IPainterRepository::PR_FILETRANSFER_SELECTION_PAINTER:
		strPainterName = "__FileTransferSelectionPainterInstance_";
		break;
	}

	return FindPainter(strPainterName);
}


CString CFileTransferEntryElement::GetName() const
{
	return "__FileTransferEntryElementInstance";
}


CFontElementPtr CFileTransferEntryElement::GetTitleTextFont() const
{
	return m_pTitleTextFont;
}


COLORREF CFileTransferEntryElement::GetTitleTextColor() const
{
	return m_clrTitleText;
}


HBITMAP CFileTransferEntryElement::GetItemSelectionBkgrndImage() const
{
	return m_hItemSelectionBkgrndImage;
}


CFontElementPtr CFileTransferEntryElement::GetActionsTextFont() const
{
	return m_pActionsTextFont;
}


COLORREF CFileTransferEntryElement::GetActionsTextColor() const
{
	return m_clrActionsText;
}


COLORREF CFileTransferEntryElement::GetItemSeparatorColor() const
{
	return m_clrItemSeparator;
}


CProgressBarElementPtr CFileTransferEntryElement::GetProgressBarElement() const
{
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		IElementPtr pElement = m_listChilds.GetNext(pos);
		
		if (0 == pElement->GetName().CompareNoCase(_T("__ProgressBarElementInstance")))
		{
			return ap_dynamic_cast<CProgressBarElementPtr>(pElement);
		}
	}
	
	return NULL;
}


void CFileTransferEntryElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{	
	__super::SetParameters(pMapParams, pElementTable);

	IUOLMessengerUIManager3Ptr pUIManager3 = 
			ap_dynamic_cast<IUOLMessengerUIManager3Ptr>(
					CUIMApplication::GetApplication()->GetUIManager());
	
	CString strThemePath;
	
	if (pUIManager3)
	{
		strThemePath = pUIManager3->
				GetCurrentThemeProperties()->GetDirPath();
	}
	
	try
	{
		CString strTitleFontName;
		GetParameterValue(strTitleFontName, pMapParams, "TitleFont");

		m_pTitleTextFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strTitleFontName));
		ATLASSERT(m_pTitleTextFont.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "TitleFont");
		// use default system font
		m_pTitleTextFont = new CFontElement();
	}

	try
	{
		CString strTitleTextColor;
		GetParameterValue(strTitleTextColor, pMapParams, "TitleTextColor");		

		m_clrTitleText.SetColor(strTitleTextColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "TitleTextColor");
		// Use font color
		m_clrTitleText = m_pTitleTextFont->GetColor();
	}
	
	try
	{
		CString strActionsFontName;
		GetParameterValue(strActionsFontName, pMapParams, "ActionsFont");
		
		m_pActionsTextFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strActionsFontName));
		ATLASSERT(m_pActionsTextFont.get());
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "ActionsFont");
		// use default system font
		m_pActionsTextFont = new CFontElement();
	}
	
	try
	{
		CString strActionsTextColor;
		GetParameterValue(strActionsTextColor, pMapParams, "ActionsTextColor");		
		
		m_clrActionsText.SetColor(strActionsTextColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "ActionsTextColor");
		// Use font color
		m_clrActionsText = m_pActionsTextFont->GetColor();
	}
	
	try
	{
		CString strSelectedBkgrndImagePath;
		GetParameterValue(strSelectedBkgrndImagePath, pMapParams, "selectedBkgrndImage");		
		
		if (!strThemePath.IsEmpty())
		{
			CString strFullFilePath = strThemePath + "\\" + strSelectedBkgrndImagePath;
			
			m_hItemSelectionBkgrndImage = 
					(HBITMAP)LoadImage(NULL, strFullFilePath, IMAGE_BITMAP, 0, 0,
					LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
		}
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "selectedBkgrndImage");
	}

	try
	{
		CString strItemSeparatorColor;
		GetParameterValue(strItemSeparatorColor, pMapParams, "ItemSeparatorColor");		
		
		m_clrItemSeparator.SetColor(strItemSeparatorColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "ItemSeparatorColor");
		m_clrItemSeparator.SetColor(_T("#E1F2F9"));
	}
}


IPainter* CFileTransferEntryElement::FindPainter(const CString& strElementName) const
{
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		const IElement* pElement = m_listChilds.GetNext(pos).get();

		CString strElement = pElement->GetName();
        if (-1 != strElement.Find(strElementName))
		{
			IPainter* pPainter = dynamic_cast<IPainter*>(const_cast<IElement*>(pElement));

			return pPainter;
		}
	}

	return NULL;
}

