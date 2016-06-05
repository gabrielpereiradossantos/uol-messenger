// SAXErrorHandler.cpp: implementation of the SAXErrorHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SAXErrorHandlerImpl.h"
#include <stdio.h>

using namespace MSXML2;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SAXErrorHandlerImpl::SAXErrorHandlerImpl()
{

}

SAXErrorHandlerImpl::~SAXErrorHandlerImpl()
{

}

HRESULT STDMETHODCALLTYPE SAXErrorHandlerImpl::error( 
            /* [in] */ ISAXLocator __RPC_FAR *pLocator,
            /* [in] */ unsigned short * pwchErrorMessage,
			/* [in] */ HRESULT errCode)
{
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE SAXErrorHandlerImpl::fatalError( 
            /* [in] */ ISAXLocator __RPC_FAR *pLocator,
            /* [in] */ unsigned short * pwchErrorMessage,
			/* [in] */ HRESULT errCode)
{
	return S_OK;
}
        
HRESULT STDMETHODCALLTYPE SAXErrorHandlerImpl::ignorableWarning( 
            /* [in] */ ISAXLocator __RPC_FAR *pLocator,
            /* [in] */ unsigned short * pwchErrorMessage,
			/* [in] */ HRESULT errCode)
{
	return S_OK;
}

long __stdcall SAXErrorHandlerImpl::QueryInterface(const struct _GUID &,void ** )
{
	// hack-hack-hack!
	return 0;
}

unsigned long __stdcall SAXErrorHandlerImpl::AddRef()
{
	// hack-hack-hack!
	return 0;
}

unsigned long __stdcall SAXErrorHandlerImpl::Release()
{
	// hack-hack-hack!
	return 0;
}