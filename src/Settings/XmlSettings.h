#pragma once
#include "isettings.h"

#include "xml/SAXContentHandlerImpl.h"
#include "xml/SAXDTDHandlerImpl.h"
#include "xml/SAXErrorHandlerImpl.h" 

#include "AbstractSettings.h"

class CXmlSettings :
	public CAbstractSettings, 
	public SAXContentHandlerImpl,
	public SAXErrorHandlerImpl
{
public:
	CXmlSettings(const CString& strXmlFile);
	virtual ~CXmlSettings(void);

protected:
	// ISettings
	virtual BOOL Load();
	virtual BOOL Save();
	
	// SAXContentHandlerImpl
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

	virtual HRESULT STDMETHODCALLTYPE CXmlSettings::characters( 
            /* [in] */ wchar_t __RPC_FAR *pwchChars,
            /* [in] */ int cchChars);

    virtual HRESULT STDMETHODCALLTYPE startDocument();


	// SAXErrorHandlerImpl
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

private:
	CString						m_strXmlFile;
	MSXML2::ISAXXMLReaderPtr	m_pSAXXMLReader;

	int							m_nDeep;
	CStringW					m_strCurrentKey;
	CStringW					m_strCurrentValue;
	BOOL						m_bGatheringValue;
};
