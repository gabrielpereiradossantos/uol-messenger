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
 
#pragma once
#include "ielement.h"
#include "IElementTable.h"
#include ".\skinexception.h"
#include "ExceptionsConsts.h"
#include "../../lexical_casts.h"


#define DECLARE_ELEMENT_NAME(x) public: static const CString GetClassName() { return x;}

class CElement : public IElement
{
public:
	CElement(void);
	virtual ~CElement(void);

	virtual CString GetName() const;
	virtual void SetName(const CString& strName);

	virtual void AddElement(IElementPtr pElement);	
	virtual void RemoveElement(IElementPtr pElement);
	virtual void OnElementBuilt();		

	
	
	/*
	Call this method if your element should be found through IElementTable.

	The xml tag "name" will be used as lookup key by IElementTable.GetElement(). 
	If your element does not have this xml tag, CElement.GetName() must be 
	overridden to provide a custom name.
	
	This method throws a CSkinException if a name was not provided.
	*/	
	virtual void SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
	{
		CString strName = "name";
		try
		{
			GetParameterValue(m_strName, pMapParams, strName);
		}
		catch(const CSkinException&)
		{		
			m_strName = GetName();

			ATLASSERT(FALSE == m_strName.IsEmpty());

			if (m_strName.IsEmpty())
			{
				throw CSkinException();
			}
		}
	}

protected:
	template <typename T>
	void GetParameterValue(T& t, CString2StringMapPtr& pMapParams, const CString& str)
	{
		CString strLower = str;
		CAtlMap<CString, CString>::CPair* pPair = pMapParams->Lookup(strLower.MakeLower());	

		if (NULL == pPair)
		{
			throw CSkinException();
		}

		t = boost::lexical_cast<T>(pPair->m_value);		
	}

	template <typename T>
	T FindElement()
	{
		T t;

		for (POSITION pos = m_listChilds.GetHeadPosition(); ((pos) && (t.get() == NULL)); )
		{
			t = ap_dynamic_cast<T>(m_listChilds.GetNext(pos));
		}	
		
		return t;
	}


protected:
	CString					m_strName;
	CAtlList<IElementPtr>	m_listChilds;
};

MAKEAUTOPTR(CElement)




