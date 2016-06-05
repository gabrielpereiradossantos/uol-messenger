// ******************************************************************
//
//	testSax: example of MSXML SAX2/COM use and base classes for handlers
//  (C) Microsoft Corp., 2000
//
// ******************************************************************
//
// SAXDTDHandlerImpl.cpp: implementation of the SAXDTDHandlerImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SAXDTDHandlerImpl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SAXDTDHandlerImpl::SAXDTDHandlerImpl()
{

}

SAXDTDHandlerImpl::~SAXDTDHandlerImpl()
{

}

HRESULT STDMETHODCALLTYPE SAXDTDHandlerImpl::notationDecl( 
            /* [in] */ wchar_t __RPC_FAR *pwchName,
            /* [in] */ int cchName,
            /* [in] */ wchar_t __RPC_FAR *pwchPublicId,
            /* [in] */ int cchPublicId,
            /* [in] */ wchar_t __RPC_FAR *pwchSystemId,
            /* [in] */ int cchSystemId)
{
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE SAXDTDHandlerImpl::unparsedEntityDecl( 
            /* [in] */ wchar_t __RPC_FAR *pwchName,
            /* [in] */ int cchName,
            /* [in] */ wchar_t __RPC_FAR *pwchPublicId,
            /* [in] */ int cchPublicId,
            /* [in] */ wchar_t __RPC_FAR *pwchSystemId,
            /* [in] */ int cchSystemId,
            /* [in] */ wchar_t __RPC_FAR *pwchNotationName,
            /* [in] */ int cchNotationName)
{
	return S_OK;
}


long __stdcall SAXDTDHandlerImpl::QueryInterface(const struct _GUID &,void ** )
{
	// hack-hack-hack!
	return 0;
}

unsigned long __stdcall SAXDTDHandlerImpl::AddRef()
{
	// hack-hack-hack!
	return 0;
}

unsigned long __stdcall SAXDTDHandlerImpl::Release()
{
	// hack-hack-hack!
	return 0;
}