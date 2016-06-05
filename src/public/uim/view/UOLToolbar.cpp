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
#include "UOLToolbar.h"

#include <interfaces/IUOLMessengerContact.h>
#include <commands/BlockContactCommand.h>
#include "../controller/UIMApplication.h"


CUOLToolBar::CUOLToolBar() 
{

}

CUOLToolBar::~CUOLToolBar()
{
}


CImageButtonBasePtr CUOLToolBar::FindImageButton(const CString &strCommand)
{
//	return CImageButtonBasePtr();
	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
	{
		CAddElementHelperPtr pHTmp = ap_static_cast<CAddElementHelperPtr>(m_listChilds.GetNext(pos));
		CImageButtonBasePtr	pBtn = pHTmp->m_pButton;
		ATLASSERT(pBtn != NULL);				
		CString tmpcommandname =  pBtn->GetCommandName();
		if (tmpcommandname.CompareNoCase(strCommand)==0) {
			return pBtn;
		}
	}
	return CImageButtonBasePtr();
}


//CImageButtonBasePtr CUOLToolBar::GetImageButton(const CString& strCommand)
//{
//	CImageButtonBasePtr	pBtn;
//
//	for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
//	{
//		CUOLToolBar::CAddElementHelperPtr pHTmp = m_listChilds.GetNext(pos);
//		CImageButtonBasePtr	pBtn = pHTmp->m_pImage;
//		ATLASSERT(pBtn != NULL);
//
//		if (pBtn->GetCommandName() == strCommand)
//		{
//			break;
//		}
//	}
//
//	return pBtn;
//}



#include "image/PngImage.h"
#include "image/png/PngHelper.h"


LRESULT CUOLToolBar::OnEraseBackground(HDC hDc)
{	
	return 1;
}

void CUOLToolBar::OnNcMouseMove(UINT nFlags, CPoint& ptPos)
{
	CAddElementHelperPtr pHelper = GetImageButton(ptPos, CImageButton::MouseOut);
}

void CUOLToolBar::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	// Fonts
	try
	{
		CString strFont;
		GetParameterValue(strFont, pMapParams, "Font");

		m_pFont = ap_dynamic_cast<CFontElementPtr>(pElementTable->GetElement(strFont));
		ATLASSERT(m_pFont.get());

	}
	catch (const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "Font");
		// use default system font
		m_pFont = new CFontElement();
	}
}

CString CUOLToolBar::GetName() const
{
	return "__UOLToolbarInstance_" + m_strGUID;
}


void CUOLToolBar::SetFont(CFontElementPtr pFont)
{
	__super::SetFont(pFont);	
}

BOOL CUOLToolBar::IsValidObject(IDataObject *pDataObject)
{
	FORMATETC fmtetc = { CF_PRIVATEFIRST, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	return pDataObject->QueryGetData(&fmtetc) == S_OK ? true : false;
}


DWORD CUOLToolBar::DoDragEnter(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	return QueryDragEffect(grfKeyState, pt);
}

DWORD CUOLToolBar::DoDragOver(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	return QueryDragEffect(grfKeyState, pt);
}


void CUOLToolBar::DoDragLeave()
{
}

DWORD CUOLToolBar::DoDrop(IDataObject *pDataObject, POINTL pt)
{
	DWORD dwEffect = DROPEFFECT_NONE;

	CPoint point(pt.x, pt.y);
	::ScreenToClient(m_hWnd, &point);

	CAddElementHelperPtr pHelper = GetImageButton(point, CImageButton::MouseOver);	

	if (pHelper != NULL)
	{
		CImageButtonBasePtr pButton = pHelper->m_pButton;

		CString strCommandName = pButton->GetDragAndDropCommandName();

		if (FALSE == strCommandName.IsEmpty())
		{
			FORMATETC fmtetc = { CF_PRIVATEFIRST, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			STGMEDIUM stgmed;

			if(pDataObject->QueryGetData(&fmtetc) == S_OK)
			{
				if(pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
				{
					if (ExecuteDragAndDropCommand(strCommandName, stgmed))
					{
						dwEffect = DROPEFFECT_MOVE;
					}

					::GlobalUnlock(stgmed.hGlobal);
					::ReleaseStgMedium(&stgmed);
				}
			}
		}
	}

	return dwEffect;
}

DWORD CUOLToolBar::QueryDragEffect(DWORD grfKeyState, POINTL pt)
{
	CPoint point(pt.x, pt.y);
	::ScreenToClient(m_hWnd, &point);

	CAddElementHelperPtr pHelper = GetImageButton(point, CImageButton::MouseOver);	

	if (pHelper != NULL)
	{
		CImageButtonBasePtr pButton = pHelper->m_pButton;

		if (FALSE == pButton->GetDragAndDropCommandName().IsEmpty())
		{
			return DROPEFFECT_MOVE;
		}
	}

	return DROPEFFECT_NONE;
}

BOOL CUOLToolBar::ExecuteDragAndDropCommand(const CString& strCommandName, STGMEDIUM& stgmed)
{
	if (0 == strCommandName.CompareNoCase("BlockContact"))
	{
		IUOLMessengerContact** ppTransferedContact = (IUOLMessengerContact**) ::GlobalLock(stgmed.hGlobal);

		if (FALSE == ::IsBadReadPtr(ppTransferedContact, sizeof(IUOLMessengerContact**)))
		{
			IUOLMessengerContact* pContact = *ppTransferedContact;
					
			if (pContact)
			{
				IUOLMessengerAccountPtr pAccount = pContact->GetAccount();

				if (pAccount.get())				
				{
					CUIMApplication::GetApplication()->ExecuteCommand(new CBlockContactCommand(pAccount, pContact->GetName(), m_hWnd));
				}
			}
		}
	}

	return TRUE;
}
