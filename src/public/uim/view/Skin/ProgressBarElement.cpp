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
#include "ProgressBarElement.h"

#include "../IPainterRepository.h"
#include "../../controller/UIMApplication.h"


CProgressBarElement::CProgressBarElement() : 
		m_hProgressLeftImage(NULL), 
		m_hProgressMiddleImage(NULL), 
		m_hProgressRightImage(NULL)
{
}


CProgressBarElement::~CProgressBarElement()
{
	if (m_hProgressLeftImage)
	{
		::DeleteObject(m_hProgressLeftImage);
	}
	
	if (m_hProgressMiddleImage)
	{
		::DeleteObject(m_hProgressMiddleImage);
	}
	
	if (m_hProgressRightImage)
	{
		::DeleteObject(m_hProgressRightImage);
	}
}


CString CProgressBarElement::GetName() const
{
	return "__ProgressBarElementInstance";
}


COLORREF CProgressBarElement::GetBackgroundColor() const
{
	return m_clrBackground;
}


COLORREF CProgressBarElement::GetBorderColor() const
{
	return m_clrBorder;
}


COLORREF CProgressBarElement::GetFillColor() const
{
	return m_clrFill;
}


HBITMAP CProgressBarElement::GetProgressLeftImage() const
{
	return m_hProgressLeftImage;
}


HBITMAP CProgressBarElement::GetProgressMiddleImage() const
{
	return m_hProgressMiddleImage;
}


HBITMAP CProgressBarElement::GetProgressRightImage() const
{
	return m_hProgressRightImage;
}


void CProgressBarElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
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
		CString strBackgroundColor;
		GetParameterValue(strBackgroundColor, pMapParams, "progressBkgrndColor");		
		
		m_clrBackground.SetColor(strBackgroundColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "progressBkgrndColor");
		m_clrBackground.SetColor(_T("#FFFFFF"));
	}
	
	try
	{
		CString strBorderColor;
		GetParameterValue(strBorderColor, pMapParams, "progressBorderColor");		
		
		m_clrBorder.SetColor(strBorderColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "progressBorderColor");
		m_clrBorder.SetColor(_T("#D2D2D2"));
	}
	
	try
	{
		CString strFillColor;
		GetParameterValue(strFillColor, pMapParams, "progressFillColor");		
		
		m_clrFill.SetColor(strFillColor);
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "progressFillColor");
		m_clrFill.SetColor(_T("#F9B129"));
	}
	
	try
	{
		CString strProgressLeftImagePath;
		GetParameterValue(strProgressLeftImagePath, pMapParams, "progressLeftImage");		
		
		if (!strThemePath.IsEmpty())
		{
			CString strFullFilePath = strThemePath + "\\" + strProgressLeftImagePath;
			
			m_hProgressLeftImage = 
					(HBITMAP)LoadImage(NULL, strFullFilePath, IMAGE_BITMAP, 0, 0,
					LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
		}
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "progressLeftImage");
	}
	
	try
	{
		CString strProgressMiddleImagePath;
		GetParameterValue(strProgressMiddleImagePath, pMapParams, "progressMiddleImage");		
		
		if (!strThemePath.IsEmpty())
		{
			CString strFullFilePath = strThemePath + "\\" + strProgressMiddleImagePath;
			
			m_hProgressMiddleImage = 
					(HBITMAP)LoadImage(NULL, strFullFilePath, IMAGE_BITMAP, 0, 0,
					LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
		}
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "progressMiddleImage");
	}
	
	try
	{
		CString strProgressRightImagePath;
		GetParameterValue(strProgressRightImagePath, pMapParams, "progressRightImage");		
		
		if (!strThemePath.IsEmpty())
		{
			CString strFullFilePath = strThemePath + "\\" + strProgressRightImagePath;
			
			m_hProgressRightImage = 
					(HBITMAP)LoadImage(NULL, strFullFilePath, IMAGE_BITMAP, 0, 0,
					LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
		}
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "progressRightImage");
	}
}

