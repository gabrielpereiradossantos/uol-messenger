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
#include ".\elementbuilder.h"

#include "../../controller/UIMApplication.h"
#include "../../controller/uimanager.h"


CElementBuilder::CElementBuilder(void)
{
}

CElementBuilder::~CElementBuilder(void)
{
}

void CElementBuilder::SetElementParameters(CElementPtr pElement)
{
	CUIManager* pUIManager = dynamic_cast<CUIManager*>(CUIMApplication::GetApplication()->GetUIManager().get());
	ATLASSERT(pUIManager);

	if (pUIManager)
	{
		pElement->SetParameters(m_pMapParams, const_cast<CUOLWindowManager*>(pUIManager->GetWindowManager()));

		for (POSITION pos = m_listChilds.GetHeadPosition(); pos;)
		{
			IElementPtr pTemp = m_listChilds.GetNext(pos);

			pElement->AddElement(pTemp);
		}
	}
}

IElementPtr CElementBuilder::CreateElement()
{
	CElementPtr pElement = AllocElement();

	ATLASSERT(pElement.get());

	SetElementParameters(pElement);

	return pElement;
}

void CElementBuilder::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	__super::SetParameters(pMapParams, pElementTable);

	m_pMapParams = pMapParams;
}