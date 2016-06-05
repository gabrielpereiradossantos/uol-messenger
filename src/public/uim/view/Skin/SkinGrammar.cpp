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
#include ".\SkinGrammar.h"

#include ".\elementfactory.h"
#include ".\skinexception.h"
#include ".\skinelement.h"

#include "../../controller/UIMApplication.h"


#define MIN_SKIN_REQ_VERSION	2



using namespace MSXML2;

CSkinGrammar::CSkinGrammar(void) :
	m_pElementTable(NULL)
{
	::CoInitialize(NULL); 	

	HRESULT hr = m_pSAXXMLReader.CreateInstance(__uuidof(SAXXMLReader), NULL, CLSCTX_ALL);
	if ( SUCCEEDED(hr) && NULL != m_pSAXXMLReader )
	{
		m_pSAXXMLReader->putContentHandler(this);	
		m_pSAXXMLReader->putErrorHandler(this);
	}
}

CSkinGrammar::~CSkinGrammar(void)
{
	m_pSAXXMLReader = NULL;

	::CoUninitialize();
}

BOOL CSkinGrammar::Parse(const CString& strFile, IElementTable*	pElementTable)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	BOOL bRet = FALSE;

	if ( NULL == m_pSAXXMLReader )
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "m_pSAXXMLReader == NULL");
		
		return FALSE;
	}
	
	m_pElementTable = pElementTable;

	wchar_t URL[1000] = {0};
	::mbstowcs(URL, "file://" + strFile, 999);	

	HRESULT hResult = S_OK;

	if (ATLPath::FileExists(strFile) && !ATLPath::IsDirectory(strFile))
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogInfo(__FUNCTION__, "Let's get it parsed man!!!!!!");
		
		hResult = m_pSAXXMLReader->parseURL(URL);

		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, "parseURL() executed... [%d]", hResult);
		
		CSkinElementPtr pSkinElement = ap_dynamic_cast<CSkinElementPtr>(m_pElementTable->GetElement("__SkinElementInstance"));
		
		if ( pSkinElement )
		{
			LONG lVersion = pSkinElement->GetSkinVersion();

			if ( lVersion < MIN_SKIN_REQ_VERSION )
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, "Invalid version: file = [%d], current = [%d]", 
								lVersion, MIN_SKIN_REQ_VERSION);
				
				throw CSkinException(E_INVALID_VERSION, "", "", 0, 0);
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, "pSkinElement not found!!!");
			
			throw CSkinException(E_INVALID_VERSION, "", "", 0, 0);
		}

		bRet = TRUE;
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "Invalid skin file!!!");
		
		hResult = E_FILE_INVALID;
	}

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "hResult = [%d]", hResult);
	
	if (FAILED(hResult))
	{
		throw m_skinException;
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return bRet;
}


HRESULT STDMETHODCALLTYPE CSkinGrammar::startElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName,
            /* [in] */ ISAXAttributes __RPC_FAR *pAttributes)

{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	HRESULT hr;
	int l = 0;

	pAttributes->getLength(&l);

	CString2StringMapPtr pMapParams = new CString2StringMap();

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "Got skin element [%s].", 
					CString(CW2A(CStringW(pwchLocalName, cchLocalName))));
	
	for (int i = 0; i < l; i++)
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, "Getting parameter [%d] from [%d].", 
						i + 1, l);
		
		wchar_t* ln, * vl; int lnl, vll;

		pAttributes->getLocalName(i, &ln, &lnl); 
		CStringW strName(ln, lnl);

		pAttributes->getValue(i, &vl, &vll);
		CStringW strValue(vl, vll);

		(*pMapParams)[CString(CW2A(strName)).MakeLower()] = CString(CW2A(strValue));
		
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogDebug(__FUNCTION__, "Saving parameter [%s] with value [%s].", 
						CString(CW2A(strName)), CString(CW2A(strValue)));
	}

	IElementPtr pElement = CElementFactory::GetElement(CString(CW2A(CStringW(pwchLocalName, cchLocalName))), pMapParams, this);			
	
	if (pElement)
	{	
		InsertElement(pElement);

		m_theStack.Push(pElement);
		hr = S_OK;
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, "pElement not found!!!!");
		
		hr = E_TAG_NOTFOUND;
	}	
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "hResult = [%d]", hr);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
    return hr;
}
       
HRESULT STDMETHODCALLTYPE CSkinGrammar::endElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	IElementPtr pElement = m_theStack.Pop();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "Ending with skin element [%s].", 
					CString(CW2A(CStringW(pwchLocalName, cchLocalName))));
	
	if (m_theStack.GetCount())
	{
		IElementPtr pRootElement = m_theStack.Pop();

		pRootElement->AddElement(pElement);
		
		m_theStack.Push(pRootElement);
	}

	pElement->OnElementBuilt();
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSkinGrammar::startDocument()
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
    printf("<?xml version=\"1.0\" ?>");
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSkinGrammar::error( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	int nLine, nCol;
	wchar_t* pwchPath;

	pLocator->getSystemId(&pwchPath);
	pLocator->getLineNumber(&nLine);
	pLocator->getColumnNumber(&nCol);

	m_skinException = CSkinException(errCode, pwchErrorMessage, pwchPath, nLine, nCol);   
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogError(__FUNCTION__, 
			"Error processing skin file: Path = [%s], Message = [%s], "
			"Line = [%d], Column = [%d] and ErrorCode = [%x].", 
			m_skinException.GetFilename(), m_skinException.GetWhatError(), 
			nLine, nCol, errCode);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSkinGrammar::fatalError( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	int nLine, nCol;
	wchar_t* pwchPath;

	pLocator->getSystemId(&pwchPath);
	pLocator->getLineNumber(&nLine);
	pLocator->getColumnNumber(&nCol);

	m_skinException = CSkinException(errCode, pwchErrorMessage, pwchPath, nLine, nCol);   
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogError(__FUNCTION__, 
			"Fatal error processing skin file: Path = [%s], Message = [%s], "
			"Line = [%d], Column = [%d] and ErrorCode = [%x].", 
			m_skinException.GetFilename(), m_skinException.GetWhatError(), 
			nLine, nCol, errCode);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return S_OK;
}
    
HRESULT STDMETHODCALLTYPE CSkinGrammar::ignorableWarning( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	int nLine, nCol;
	wchar_t* pwchPath;

	pLocator->getSystemId(&pwchPath);
	pLocator->getLineNumber(&nLine);
	pLocator->getColumnNumber(&nCol);

	m_skinException = CSkinException(errCode, pwchErrorMessage, pwchPath, nLine, nCol);   
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogWarning(__FUNCTION__, 
			"Ignorable warning processing skin file: Path = [%s], Message = [%s], "
			"Line = [%d], Column = [%d] and ErrorCode = [%x].", 
			m_skinException.GetFilename(), m_skinException.GetWhatError(), 
			nLine, nCol, errCode);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);

	return S_OK;
}

IElementPtr CSkinGrammar::GetElement(const CString& strName) const
{
	return m_pElementTable->GetElement(strName);
}

void CSkinGrammar::InsertElement(IElementPtr& pElement)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	m_pElementTable->InsertElement(pElement);	
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

