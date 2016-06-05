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
#include "DefaultPropertyPageCommand.h"


CDefaultPropertyPageCommand::~CDefaultPropertyPageCommand(void)
{
	m_commandValueMap.RemoveAll();
}


BOOL CDefaultPropertyPageCommand::AddCommandValue(
		const int nCmdValueId, const CString strCmdValue)
{
	// Validation process.
	//
	if ( (nCmdValueId < CVT_ACCOUNT_TYPE) && (nCmdValueId > CVT_DUMMY_VALIDATOR) )
	{
		return FALSE;
	}
	
	// Firstly, we store the new informed pair.
	// We don't know for now whether it will be usefull in future.
	//
	m_commandValueMap.SetAt(nCmdValueId, strCmdValue);
	
	// Right after, we forward the processing to the especific 
	// implementing class.
	//
	return ProcessNewCommandValue(nCmdValueId, strCmdValue);
}


BOOL CDefaultPropertyPageCommand::AddCommandValue(
		const int nCmdValueId, const int nCmdValue)
{
	CString formatingStr;
	
	formatingStr.Format("%d", nCmdValue);
	
	return AddCommandValue(nCmdValueId, formatingStr);
}


BOOL CDefaultPropertyPageCommand::GetCommandValueBOOL(
		const int nCmdValueId, BOOL& bValue)
{
	commandValueMapType::CPair* pMapElement = 
			m_commandValueMap.Lookup(nCmdValueId);
	
	if (pMapElement)
	{
		bValue = atoi(pMapElement->m_value);
		
		return TRUE;
	}
	
	return FALSE;
}


BOOL CDefaultPropertyPageCommand::GetCommandValueInt(
		const int nCmdValueId, int& nValue)
{
	commandValueMapType::CPair*	pMapElement = 
			m_commandValueMap.Lookup(nCmdValueId);
	
	if (pMapElement)
	{
		nValue = atoi(pMapElement->m_value);
		
		return TRUE;
	}
	
	return FALSE;
}


BOOL CDefaultPropertyPageCommand::GetCommandValueString(
		const int nCmdValueId, CString& strValue)
{
	commandValueMapType::CPair*	pMapElement = 
			m_commandValueMap.Lookup(nCmdValueId);
	
	if (pMapElement)
	{
		strValue = pMapElement->m_value;
		
		return TRUE;
	}
	
	return FALSE;
}

