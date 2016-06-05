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
#include ".\BitmapElement.h"

#include "../../controller/uimapplication.h"

CBitmapElement::CBitmapElement(void):
    m_hBitmap(NULL)
{
}

CBitmapElement::~CBitmapElement(void)
{
    DeleteObject(m_hBitmap);
}

void CBitmapElement::LoadBitmap(const CString& strThemeBasedPath)
{
	if (m_hBitmap)
	{
		DeleteObject(m_hBitmap);
	}

	CString strFullFilePath;

	IUOLMessengerUIManager3Ptr pUIManager3 = ap_dynamic_cast<IUOLMessengerUIManager3Ptr>(CUIMApplication::GetApplication()->GetUIManager());

	if (pUIManager3)
	{
		IUOLMessengerThemePropertiesPtr pThemeProperties = pUIManager3->GetCurrentThemeProperties();

		if (pThemeProperties)
		{
			strFullFilePath = pThemeProperties->GetDirPath() + _T("\\") + strThemeBasedPath;
		}
	}

    m_hBitmap = (HBITMAP)LoadImage(NULL, strFullFilePath, IMAGE_BITMAP, 0, 0,
	            LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
}

HBITMAP CBitmapElement::GetBitmap() const
{
	return m_hBitmap;
}

void CBitmapElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	CString strFile;
	GetParameterValue(strFile, pMapParams, "file");

	LoadBitmap(strFile);	
}
