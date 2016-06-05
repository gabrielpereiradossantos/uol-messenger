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

#include "xml/SAXContentHandlerImpl.h"
#include "xml/SAXErrorHandlerImpl.h"
#include "IElement.h"
#include "IElementTable.h"
#include "SkinException.h"
#include "ExceptionsConsts.h"


template <typename E>
class CAtlStack : private CAtlList<E>
{
public:
	using CAtlList<E>::GetCount;

	void Push(E e)
	{
		AddTail(e);
	}

	E Pop()
	{
		return RemoveTail();
	}
};

class CSkinGrammar : public SAXContentHandlerImpl,
					 public SAXErrorHandlerImpl, 
					 public IElementTable
{
public:
	CSkinGrammar(void);
	virtual ~CSkinGrammar(void);

	BOOL Parse(const CString& strFile, IElementTable* pElementTable);

protected:
	virtual HRESULT STDMETHODCALLTYPE startElement( 
        /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
        /* [in] */ int cchNamespaceUri,
        /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
        /* [in] */ int cchLocalName,
        /* [in] */ wchar_t __RPC_FAR *pwchRawName,
        /* [in] */ int cchRawName,
        /* [in] */ MSXML2::ISAXAttributes __RPC_FAR *pAttributes);
        
    virtual HRESULT STDMETHODCALLTYPE endElement( 
        /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
        /* [in] */ int cchNamespaceUri,
        /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
        /* [in] */ int cchLocalName,
        /* [in] */ wchar_t __RPC_FAR *pwchRawName,
        /* [in] */ int cchRawName);

    virtual HRESULT STDMETHODCALLTYPE startDocument();

    virtual HRESULT STDMETHODCALLTYPE error( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode);
    
    virtual HRESULT STDMETHODCALLTYPE fatalError( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode);
    
    virtual HRESULT STDMETHODCALLTYPE ignorableWarning( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode);



	virtual IElementPtr GetElement(const CString& strName) const;
	virtual void InsertElement(IElementPtr& pElement);

private:
	MSXML2::ISAXXMLReaderPtr	m_pSAXXMLReader;
	
	CAtlStack<IElementPtr>		m_theStack;	
	IElementTable*				m_pElementTable;
	CSkinException 				m_skinException;
};

