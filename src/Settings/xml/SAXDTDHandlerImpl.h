// ******************************************************************
//
//	testSax: example of MSXML SAX2/COM use and base classes for handlers
//  (C) Microsoft Corp., 2000
//
// ******************************************************************
//
// SAXDTDHandlerImpl.h: interface for the SAXDTDHandlerImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAXDTDHANDLERIMPL_H__AC8BAE21_1449_464A_AF6B_26283ED0EA5D__INCLUDED_)
#define AFX_SAXDTDHANDLERIMPL_H__AC8BAE21_1449_464A_AF6B_26283ED0EA5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SAXDTDHandlerImpl : public MSXML2::ISAXDTDHandler  
{
public:
	SAXDTDHandlerImpl();
	virtual ~SAXDTDHandlerImpl();

		// This must be correctly implemented, if your handler must be a COM Object (in this example it does not)
		long __stdcall QueryInterface(const struct _GUID &,void ** );
		unsigned long __stdcall AddRef(void);
		unsigned long __stdcall Release(void);

        virtual HRESULT STDMETHODCALLTYPE notationDecl( 
            /* [in] */ wchar_t __RPC_FAR *pwchName,
            /* [in] */ int cchName,
            /* [in] */ wchar_t __RPC_FAR *pwchPublicId,
            /* [in] */ int cchPublicId,
            /* [in] */ wchar_t __RPC_FAR *pwchSystemId,
            /* [in] */ int cchSystemId);
        
        virtual HRESULT STDMETHODCALLTYPE unparsedEntityDecl( 
            /* [in] */ wchar_t __RPC_FAR *pwchName,
            /* [in] */ int cchName,
            /* [in] */ wchar_t __RPC_FAR *pwchPublicId,
            /* [in] */ int cchPublicId,
            /* [in] */ wchar_t __RPC_FAR *pwchSystemId,
            /* [in] */ int cchSystemId,
            /* [in] */ wchar_t __RPC_FAR *pwchNotationName,
            /* [in] */ int cchNotationName);


};

#endif // !defined(AFX_SAXDTDHANDLERIMPL_H__AC8BAE21_1449_464A_AF6B_26283ED0EA5D__INCLUDED_)
