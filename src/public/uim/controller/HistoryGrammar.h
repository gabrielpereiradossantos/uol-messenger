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

#include "HistoryRecord.h"
#include "../view/skin/xml/SAXContentHandlerImpl.h"
#include "../view/skin/xml/SAXErrorHandlerImpl.h"
#include "../view/skin/SkinException.h"
#include "../view/skin/ExceptionsConsts.h"

#include <interfaces/IUOLMessengerHistoryRecord.h>


class CHistoryGrammar : public SAXContentHandlerImpl,
						public SAXErrorHandlerImpl
{
public:
	CHistoryGrammar(void);
	virtual ~CHistoryGrammar(void);

	BOOL Parse(const CString& strFileName, const CTime& timeHistory);
	CHistoryRecordPtr GetHistoryRecord();

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

	virtual HRESULT STDMETHODCALLTYPE CHistoryGrammar::characters( 
        /* [in] */ wchar_t __RPC_FAR *pwchChars,
        /* [in] */ int cchChars);

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


	void ParseVersion0(const CString& strAttributeName, const CString& strAttributeValue);
	void ParseVersion1(const CString& strAttributeName, const CString& strAttributeValue);
	CTime EncodeTime();

private:
	UOLLib::ISettings*			m_pStringLoader;
	CHistoryRecordPtr			m_pHistoryRecord;
	MSXML2::ISAXXMLReaderPtr	m_pSAXXMLReader;
	CString						m_strVersion;
	CString						m_strElementName;
		
	// Event attributes
	CString		m_strDate;
	CString		m_strTime;
	CString		m_strStatus;
	CString		m_strType;
	CString		m_strEventFrom;
	CString		m_strEventTo;
	CString		m_strEventText;
};

