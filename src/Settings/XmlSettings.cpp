#include "StdAfx.h"
#include ".\xmlsettings.h"

using namespace MSXML2;
using namespace UOLLib;

CXmlSettings::CXmlSettings(const CString& strXmlFile) :
	m_strXmlFile(strXmlFile), 
	m_nDeep(), 
	m_bGatheringValue()
{
	::CoInitialize(NULL);
}

CXmlSettings::~CXmlSettings(void)
{
	::CoUninitialize();
}

BOOL CXmlSettings::Load()
{
	HRESULT hr = m_pSAXXMLReader.CreateInstance(__uuidof(SAXXMLReader), NULL, CLSCTX_ALL);

	if ( SUCCEEDED(hr) && NULL != m_pSAXXMLReader )
	{
		m_pSAXXMLReader->putContentHandler(this);	
		m_pSAXXMLReader->putErrorHandler(this);
	}

	BOOL bRet = FALSE;

	if ( NULL == m_pSAXXMLReader )
	{
		return FALSE;
	}
	
	wchar_t URL[1000] = {0};
	::mbstowcs(URL, "file://" + m_strXmlFile, 999);	

	return SUCCEEDED(m_pSAXXMLReader->parseURL(URL));
}

BOOL CXmlSettings::Save()
{
	return TRUE;
}

HRESULT STDMETHODCALLTYPE CXmlSettings::startElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName,
            /* [in] */ ISAXAttributes __RPC_FAR *pAttributes)

{
	HRESULT hr = S_OK;

	if (m_nDeep == 1)
	{
		m_strCurrentKey = pwchLocalName;

		m_bGatheringValue = TRUE;		
	}	

	m_nDeep++;
		
    return hr;


}
       
HRESULT STDMETHODCALLTYPE CXmlSettings::endElement( 
            /* [in] */ wchar_t __RPC_FAR *pwchNamespaceUri,
            /* [in] */ int cchNamespaceUri,
            /* [in] */ wchar_t __RPC_FAR *pwchLocalName,
            /* [in] */ int cchLocalName,
            /* [in] */ wchar_t __RPC_FAR *pwchRawName,
            /* [in] */ int cchRawName)
{
	m_nDeep--;

	if (m_nDeep == 1)
	{
		m_bGatheringValue = FALSE;

		SetValue(CString(CW2A(m_strCurrentKey)), CString(CW2A(m_strCurrentValue)));

		m_strCurrentValue.Empty();
	}

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CXmlSettings::characters( 
            /* [in] */ wchar_t __RPC_FAR *pwchChars,
            /* [in] */ int cchChars)
{
	if (m_bGatheringValue)
	{
		CStringW strValue(pwchChars, cchChars);
		
		m_strCurrentValue += strValue;		
	}

    return S_OK;
}
      

HRESULT STDMETHODCALLTYPE CXmlSettings::startDocument()
{
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CXmlSettings::error( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode)
{
	int nLine, nCol;
	wchar_t* pwchPath;

	pLocator->getSystemId(&pwchPath);
	pLocator->getLineNumber(&nLine);
	pLocator->getColumnNumber(&nCol);	

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CXmlSettings::fatalError( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode)
{
	int nLine, nCol;
	wchar_t* pwchPath;

	pLocator->getSystemId(&pwchPath);
	pLocator->getLineNumber(&nLine);
	pLocator->getColumnNumber(&nCol);


	return S_OK;
}
    
HRESULT STDMETHODCALLTYPE CXmlSettings::ignorableWarning( 
        /* [in] */ MSXML2::ISAXLocator __RPC_FAR *pLocator,
        /* [in] */ unsigned short * pwchErrorMessage,
		/* [in] */ HRESULT errCode)
{
	int nLine, nCol;
	wchar_t* pwchPath;

	pLocator->getSystemId(&pwchPath);
	pLocator->getLineNumber(&nLine);
	pLocator->getColumnNumber(&nCol);	

	return S_OK;
}