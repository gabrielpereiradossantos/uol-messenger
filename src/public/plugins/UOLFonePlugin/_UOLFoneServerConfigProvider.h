

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0499 */
/* at Mon Mar 30 10:35:36 2009
 */
/* Compiler settings for _UOLFoneServerConfigProvider.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef ___UOLFoneServerConfigProvider_h__
#define ___UOLFoneServerConfigProvider_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IUOLFoneCodecFileInfo_FWD_DEFINED__
#define __IUOLFoneCodecFileInfo_FWD_DEFINED__
typedef interface IUOLFoneCodecFileInfo IUOLFoneCodecFileInfo;
#endif 	/* __IUOLFoneCodecFileInfo_FWD_DEFINED__ */


#ifndef __IUOLFoneCodecInfo_FWD_DEFINED__
#define __IUOLFoneCodecInfo_FWD_DEFINED__
typedef interface IUOLFoneCodecInfo IUOLFoneCodecInfo;
#endif 	/* __IUOLFoneCodecInfo_FWD_DEFINED__ */


#ifndef __IUOLFoneCodecInfoList_FWD_DEFINED__
#define __IUOLFoneCodecInfoList_FWD_DEFINED__
typedef interface IUOLFoneCodecInfoList IUOLFoneCodecInfoList;
#endif 	/* __IUOLFoneCodecInfoList_FWD_DEFINED__ */


#ifndef __IUOLFoneCodecsConfigFileUpdater_FWD_DEFINED__
#define __IUOLFoneCodecsConfigFileUpdater_FWD_DEFINED__
typedef interface IUOLFoneCodecsConfigFileUpdater IUOLFoneCodecsConfigFileUpdater;
#endif 	/* __IUOLFoneCodecsConfigFileUpdater_FWD_DEFINED__ */


#ifndef __IUOLFoneCodecsConfigFileUpdater2_FWD_DEFINED__
#define __IUOLFoneCodecsConfigFileUpdater2_FWD_DEFINED__
typedef interface IUOLFoneCodecsConfigFileUpdater2 IUOLFoneCodecsConfigFileUpdater2;
#endif 	/* __IUOLFoneCodecsConfigFileUpdater2_FWD_DEFINED__ */


#ifndef __IUOLFoneCodecsConfigFileUpdater3_FWD_DEFINED__
#define __IUOLFoneCodecsConfigFileUpdater3_FWD_DEFINED__
typedef interface IUOLFoneCodecsConfigFileUpdater3 IUOLFoneCodecsConfigFileUpdater3;
#endif 	/* __IUOLFoneCodecsConfigFileUpdater3_FWD_DEFINED__ */


#ifndef __IUOLFoneCodecsConfigFileUpdaterCallback_FWD_DEFINED__
#define __IUOLFoneCodecsConfigFileUpdaterCallback_FWD_DEFINED__
typedef interface IUOLFoneCodecsConfigFileUpdaterCallback IUOLFoneCodecsConfigFileUpdaterCallback;
#endif 	/* __IUOLFoneCodecsConfigFileUpdaterCallback_FWD_DEFINED__ */


#ifndef __IUOLFoneCodecsUpdateInfo_FWD_DEFINED__
#define __IUOLFoneCodecsUpdateInfo_FWD_DEFINED__
typedef interface IUOLFoneCodecsUpdateInfo IUOLFoneCodecsUpdateInfo;
#endif 	/* __IUOLFoneCodecsUpdateInfo_FWD_DEFINED__ */


#ifndef __IUOLFoneCodecsUpdater_FWD_DEFINED__
#define __IUOLFoneCodecsUpdater_FWD_DEFINED__
typedef interface IUOLFoneCodecsUpdater IUOLFoneCodecsUpdater;
#endif 	/* __IUOLFoneCodecsUpdater_FWD_DEFINED__ */


#ifndef __IUOLFoneCodecsUpdater2_FWD_DEFINED__
#define __IUOLFoneCodecsUpdater2_FWD_DEFINED__
typedef interface IUOLFoneCodecsUpdater2 IUOLFoneCodecsUpdater2;
#endif 	/* __IUOLFoneCodecsUpdater2_FWD_DEFINED__ */


#ifndef __IUOLFoneCodecsUpdater3_FWD_DEFINED__
#define __IUOLFoneCodecsUpdater3_FWD_DEFINED__
typedef interface IUOLFoneCodecsUpdater3 IUOLFoneCodecsUpdater3;
#endif 	/* __IUOLFoneCodecsUpdater3_FWD_DEFINED__ */


#ifndef __IUOLFoneCodecsUpdaterCallback_FWD_DEFINED__
#define __IUOLFoneCodecsUpdaterCallback_FWD_DEFINED__
typedef interface IUOLFoneCodecsUpdaterCallback IUOLFoneCodecsUpdaterCallback;
#endif 	/* __IUOLFoneCodecsUpdaterCallback_FWD_DEFINED__ */


#ifndef __IUOLFoneDownload_FWD_DEFINED__
#define __IUOLFoneDownload_FWD_DEFINED__
typedef interface IUOLFoneDownload IUOLFoneDownload;
#endif 	/* __IUOLFoneDownload_FWD_DEFINED__ */


#ifndef __IUOLFoneDownloadCallback_FWD_DEFINED__
#define __IUOLFoneDownloadCallback_FWD_DEFINED__
typedef interface IUOLFoneDownloadCallback IUOLFoneDownloadCallback;
#endif 	/* __IUOLFoneDownloadCallback_FWD_DEFINED__ */


#ifndef __IUOLFoneMessageInfo_FWD_DEFINED__
#define __IUOLFoneMessageInfo_FWD_DEFINED__
typedef interface IUOLFoneMessageInfo IUOLFoneMessageInfo;
#endif 	/* __IUOLFoneMessageInfo_FWD_DEFINED__ */


#ifndef __IUOLFoneMessageInfoEnum_FWD_DEFINED__
#define __IUOLFoneMessageInfoEnum_FWD_DEFINED__
typedef interface IUOLFoneMessageInfoEnum IUOLFoneMessageInfoEnum;
#endif 	/* __IUOLFoneMessageInfoEnum_FWD_DEFINED__ */


#ifndef __IUOLFoneMessageParser_FWD_DEFINED__
#define __IUOLFoneMessageParser_FWD_DEFINED__
typedef interface IUOLFoneMessageParser IUOLFoneMessageParser;
#endif 	/* __IUOLFoneMessageParser_FWD_DEFINED__ */


#ifndef __IUOLFoneOption_FWD_DEFINED__
#define __IUOLFoneOption_FWD_DEFINED__
typedef interface IUOLFoneOption IUOLFoneOption;
#endif 	/* __IUOLFoneOption_FWD_DEFINED__ */


#ifndef __IUOLFoneOptionEnum_FWD_DEFINED__
#define __IUOLFoneOptionEnum_FWD_DEFINED__
typedef interface IUOLFoneOptionEnum IUOLFoneOptionEnum;
#endif 	/* __IUOLFoneOptionEnum_FWD_DEFINED__ */


#ifndef __IUOLFonePhoneValidator_FWD_DEFINED__
#define __IUOLFonePhoneValidator_FWD_DEFINED__
typedef interface IUOLFonePhoneValidator IUOLFonePhoneValidator;
#endif 	/* __IUOLFonePhoneValidator_FWD_DEFINED__ */


#ifndef __IUOLFoneSettings_FWD_DEFINED__
#define __IUOLFoneSettings_FWD_DEFINED__
typedef interface IUOLFoneSettings IUOLFoneSettings;
#endif 	/* __IUOLFoneSettings_FWD_DEFINED__ */


#ifndef __IUOLFonePhoneValidationRulesDownload_FWD_DEFINED__
#define __IUOLFonePhoneValidationRulesDownload_FWD_DEFINED__
typedef interface IUOLFonePhoneValidationRulesDownload IUOLFonePhoneValidationRulesDownload;
#endif 	/* __IUOLFonePhoneValidationRulesDownload_FWD_DEFINED__ */


#ifndef __IUOLFoneServerConfigProvider_FWD_DEFINED__
#define __IUOLFoneServerConfigProvider_FWD_DEFINED__
typedef interface IUOLFoneServerConfigProvider IUOLFoneServerConfigProvider;
#endif 	/* __IUOLFoneServerConfigProvider_FWD_DEFINED__ */


#ifndef __IUOLFoneServerConfigProvider2_FWD_DEFINED__
#define __IUOLFoneServerConfigProvider2_FWD_DEFINED__
typedef interface IUOLFoneServerConfigProvider2 IUOLFoneServerConfigProvider2;
#endif 	/* __IUOLFoneServerConfigProvider2_FWD_DEFINED__ */


#ifndef __IUOLFoneServerConfigProvider3_FWD_DEFINED__
#define __IUOLFoneServerConfigProvider3_FWD_DEFINED__
typedef interface IUOLFoneServerConfigProvider3 IUOLFoneServerConfigProvider3;
#endif 	/* __IUOLFoneServerConfigProvider3_FWD_DEFINED__ */


#ifndef __IUOLFoneServerConfigProvider4_FWD_DEFINED__
#define __IUOLFoneServerConfigProvider4_FWD_DEFINED__
typedef interface IUOLFoneServerConfigProvider4 IUOLFoneServerConfigProvider4;
#endif 	/* __IUOLFoneServerConfigProvider4_FWD_DEFINED__ */


#ifndef __CUOLFoneServerConfigProvider_FWD_DEFINED__
#define __CUOLFoneServerConfigProvider_FWD_DEFINED__

#ifdef __cplusplus
typedef class CUOLFoneServerConfigProvider CUOLFoneServerConfigProvider;
#else
typedef struct CUOLFoneServerConfigProvider CUOLFoneServerConfigProvider;
#endif /* __cplusplus */

#endif 	/* __CUOLFoneServerConfigProvider_FWD_DEFINED__ */


/* header files for imported files */
#include "prsht.h"
#include "mshtml.h"
#include "mshtmhst.h"
#include "exdisp.h"
#include "objsafe.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IUOLFoneCodecFileInfo_INTERFACE_DEFINED__
#define __IUOLFoneCodecFileInfo_INTERFACE_DEFINED__

/* interface IUOLFoneCodecFileInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecFileInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EF85199F-0B86-47ad-86B2-A576A38B1978")
    IUOLFoneCodecFileInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetFileName( 
            /* [in] */ BSTR *pbstrFileName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetFileVersion( 
            /* [in] */ BSTR *pbstrFileVersion) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetFileLocalPath( 
            /* [in] */ BSTR *pbstrFileLocalPath) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecFileInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecFileInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecFileInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecFileInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecFileInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecFileInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecFileInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecFileInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetFileName )( 
            IUOLFoneCodecFileInfo * This,
            /* [in] */ BSTR *pbstrFileName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetFileVersion )( 
            IUOLFoneCodecFileInfo * This,
            /* [in] */ BSTR *pbstrFileVersion);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetFileLocalPath )( 
            IUOLFoneCodecFileInfo * This,
            /* [in] */ BSTR *pbstrFileLocalPath);
        
        END_INTERFACE
    } IUOLFoneCodecFileInfoVtbl;

    interface IUOLFoneCodecFileInfo
    {
        CONST_VTBL struct IUOLFoneCodecFileInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecFileInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecFileInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecFileInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecFileInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecFileInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecFileInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecFileInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecFileInfo_GetFileName(This,pbstrFileName)	\
    ( (This)->lpVtbl -> GetFileName(This,pbstrFileName) ) 

#define IUOLFoneCodecFileInfo_GetFileVersion(This,pbstrFileVersion)	\
    ( (This)->lpVtbl -> GetFileVersion(This,pbstrFileVersion) ) 

#define IUOLFoneCodecFileInfo_GetFileLocalPath(This,pbstrFileLocalPath)	\
    ( (This)->lpVtbl -> GetFileLocalPath(This,pbstrFileLocalPath) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecFileInfo_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneCodecInfo_INTERFACE_DEFINED__
#define __IUOLFoneCodecInfo_INTERFACE_DEFINED__

/* interface IUOLFoneCodecInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B3611893-1CC4-4d4d-AC45-DC35659DE5EF")
    IUOLFoneCodecInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecName( 
            /* [in] */ BSTR *pbstrCodecName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecType( 
            /* [in] */ BSTR *pbstrCodecType) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecOrder( 
            /* [in] */ BSTR *pbstrCodecOrder) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecVersion( 
            /* [in] */ BSTR *pbstrCodecVersion) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecUrl( 
            /* [in] */ BSTR *pbstrCodecUrl) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecIsOptional( 
            /* [in] */ BOOL *pboolCodecIsOptional) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecFilesNumber( 
            /* [out] */ LONG *plFilesNumber) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecFileInfo( 
            /* [in] */ LONG lPosition,
            /* [out] */ IUOLFoneCodecFileInfo **ppCodecFileInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FindCodecFileInfo( 
            /* [in] */ BSTR fileName,
            /* [out] */ IUOLFoneCodecFileInfo **ppCodecFileInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecName )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ BSTR *pbstrCodecName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecType )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ BSTR *pbstrCodecType);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecOrder )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ BSTR *pbstrCodecOrder);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecVersion )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ BSTR *pbstrCodecVersion);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecUrl )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ BSTR *pbstrCodecUrl);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecIsOptional )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ BOOL *pboolCodecIsOptional);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecFilesNumber )( 
            IUOLFoneCodecInfo * This,
            /* [out] */ LONG *plFilesNumber);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecFileInfo )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ LONG lPosition,
            /* [out] */ IUOLFoneCodecFileInfo **ppCodecFileInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindCodecFileInfo )( 
            IUOLFoneCodecInfo * This,
            /* [in] */ BSTR fileName,
            /* [out] */ IUOLFoneCodecFileInfo **ppCodecFileInfo);
        
        END_INTERFACE
    } IUOLFoneCodecInfoVtbl;

    interface IUOLFoneCodecInfo
    {
        CONST_VTBL struct IUOLFoneCodecInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecInfo_GetCodecName(This,pbstrCodecName)	\
    ( (This)->lpVtbl -> GetCodecName(This,pbstrCodecName) ) 

#define IUOLFoneCodecInfo_GetCodecType(This,pbstrCodecType)	\
    ( (This)->lpVtbl -> GetCodecType(This,pbstrCodecType) ) 

#define IUOLFoneCodecInfo_GetCodecOrder(This,pbstrCodecOrder)	\
    ( (This)->lpVtbl -> GetCodecOrder(This,pbstrCodecOrder) ) 

#define IUOLFoneCodecInfo_GetCodecVersion(This,pbstrCodecVersion)	\
    ( (This)->lpVtbl -> GetCodecVersion(This,pbstrCodecVersion) ) 

#define IUOLFoneCodecInfo_GetCodecUrl(This,pbstrCodecUrl)	\
    ( (This)->lpVtbl -> GetCodecUrl(This,pbstrCodecUrl) ) 

#define IUOLFoneCodecInfo_GetCodecIsOptional(This,pboolCodecIsOptional)	\
    ( (This)->lpVtbl -> GetCodecIsOptional(This,pboolCodecIsOptional) ) 

#define IUOLFoneCodecInfo_GetCodecFilesNumber(This,plFilesNumber)	\
    ( (This)->lpVtbl -> GetCodecFilesNumber(This,plFilesNumber) ) 

#define IUOLFoneCodecInfo_GetCodecFileInfo(This,lPosition,ppCodecFileInfo)	\
    ( (This)->lpVtbl -> GetCodecFileInfo(This,lPosition,ppCodecFileInfo) ) 

#define IUOLFoneCodecInfo_FindCodecFileInfo(This,fileName,ppCodecFileInfo)	\
    ( (This)->lpVtbl -> FindCodecFileInfo(This,fileName,ppCodecFileInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecInfo_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneServerConfigProvider_0000_0002 */
/* [local] */ 


enum EnumUOLFoneCodecTypes
    {	UOLFONE_CODEC_TYPE_ANY	= 0,
	UOLFONE_CODEC_TYPE_AUDIO	= 1,
	UOLFONE_CODEC_TYPE_VIDEO	= 2
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneServerConfigProvider_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneServerConfigProvider_0000_0002_v0_0_s_ifspec;

#ifndef __IUOLFoneCodecInfoList_INTERFACE_DEFINED__
#define __IUOLFoneCodecInfoList_INTERFACE_DEFINED__

/* interface IUOLFoneCodecInfoList */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecInfoList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("383B646B-3989-4307-AC33-473F3CD576AD")
    IUOLFoneCodecInfoList : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecsNumber( 
            /* [out] */ LONG *plCodecsNumber) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecInfo( 
            /* [in] */ LONG lPosition,
            /* [out] */ IUOLFoneCodecInfo **ppCodecInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FindCodecInfo( 
            /* [in] */ BSTR codecName,
            /* [out] */ IUOLFoneCodecInfo **ppCodecInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecsPrecedence( 
            /* [out] */ BSTR *pbstrCodecsPrecedence) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecInfoOrdered( 
            /* [in] */ LONG lPosition,
            /* [out] */ IUOLFoneCodecInfo **ppCodecInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecsNumberByType( 
            /* [in] */ LONG codecTypeId,
            /* [out] */ LONG *plCodecsNumber) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecsPrecedenceByType( 
            /* [in] */ LONG codecTypeId,
            /* [out] */ BSTR *pbstrCodecsPrecedence) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecInfoOrderedByType( 
            /* [in] */ LONG codecTypeId,
            /* [in] */ LONG lPosition,
            /* [out] */ IUOLFoneCodecInfo **ppCodecInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecInfoListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecInfoList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecInfoList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecInfoList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecInfoList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecInfoList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecInfoList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecInfoList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecsNumber )( 
            IUOLFoneCodecInfoList * This,
            /* [out] */ LONG *plCodecsNumber);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecInfo )( 
            IUOLFoneCodecInfoList * This,
            /* [in] */ LONG lPosition,
            /* [out] */ IUOLFoneCodecInfo **ppCodecInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindCodecInfo )( 
            IUOLFoneCodecInfoList * This,
            /* [in] */ BSTR codecName,
            /* [out] */ IUOLFoneCodecInfo **ppCodecInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecsPrecedence )( 
            IUOLFoneCodecInfoList * This,
            /* [out] */ BSTR *pbstrCodecsPrecedence);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecInfoOrdered )( 
            IUOLFoneCodecInfoList * This,
            /* [in] */ LONG lPosition,
            /* [out] */ IUOLFoneCodecInfo **ppCodecInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecsNumberByType )( 
            IUOLFoneCodecInfoList * This,
            /* [in] */ LONG codecTypeId,
            /* [out] */ LONG *plCodecsNumber);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecsPrecedenceByType )( 
            IUOLFoneCodecInfoList * This,
            /* [in] */ LONG codecTypeId,
            /* [out] */ BSTR *pbstrCodecsPrecedence);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecInfoOrderedByType )( 
            IUOLFoneCodecInfoList * This,
            /* [in] */ LONG codecTypeId,
            /* [in] */ LONG lPosition,
            /* [out] */ IUOLFoneCodecInfo **ppCodecInfo);
        
        END_INTERFACE
    } IUOLFoneCodecInfoListVtbl;

    interface IUOLFoneCodecInfoList
    {
        CONST_VTBL struct IUOLFoneCodecInfoListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecInfoList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecInfoList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecInfoList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecInfoList_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecInfoList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecInfoList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecInfoList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecInfoList_GetCodecsNumber(This,plCodecsNumber)	\
    ( (This)->lpVtbl -> GetCodecsNumber(This,plCodecsNumber) ) 

#define IUOLFoneCodecInfoList_GetCodecInfo(This,lPosition,ppCodecInfo)	\
    ( (This)->lpVtbl -> GetCodecInfo(This,lPosition,ppCodecInfo) ) 

#define IUOLFoneCodecInfoList_FindCodecInfo(This,codecName,ppCodecInfo)	\
    ( (This)->lpVtbl -> FindCodecInfo(This,codecName,ppCodecInfo) ) 

#define IUOLFoneCodecInfoList_GetCodecsPrecedence(This,pbstrCodecsPrecedence)	\
    ( (This)->lpVtbl -> GetCodecsPrecedence(This,pbstrCodecsPrecedence) ) 

#define IUOLFoneCodecInfoList_GetCodecInfoOrdered(This,lPosition,ppCodecInfo)	\
    ( (This)->lpVtbl -> GetCodecInfoOrdered(This,lPosition,ppCodecInfo) ) 

#define IUOLFoneCodecInfoList_GetCodecsNumberByType(This,codecTypeId,plCodecsNumber)	\
    ( (This)->lpVtbl -> GetCodecsNumberByType(This,codecTypeId,plCodecsNumber) ) 

#define IUOLFoneCodecInfoList_GetCodecsPrecedenceByType(This,codecTypeId,pbstrCodecsPrecedence)	\
    ( (This)->lpVtbl -> GetCodecsPrecedenceByType(This,codecTypeId,pbstrCodecsPrecedence) ) 

#define IUOLFoneCodecInfoList_GetCodecInfoOrderedByType(This,codecTypeId,lPosition,ppCodecInfo)	\
    ( (This)->lpVtbl -> GetCodecInfoOrderedByType(This,codecTypeId,lPosition,ppCodecInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecInfoList_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneServerConfigProvider_0000_0003 */
/* [local] */ 


enum EnumUpdateConfigStatus
    {	UFS_UPDATE_CONFIG_STATUS_SUCCESS	= 0,
	UFS_UPDATE_CONFIG_MISSING_PARAM	= 1,
	UFS_UPDATE_CONFIG_STATUS_FAILED	= 2,
	UFS_UPDATE_CONFIG_DOWNLOAD_FAILED	= 3,
	UFS_UPDATE_CONFIG_XML_PARSE_FAILED	= 4,
	UFS_UPDATE_CONFIG_STOPPED	= 5
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneServerConfigProvider_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneServerConfigProvider_0000_0003_v0_0_s_ifspec;

#ifndef __IUOLFoneCodecsConfigFileUpdater_INTERFACE_DEFINED__
#define __IUOLFoneCodecsConfigFileUpdater_INTERFACE_DEFINED__

/* interface IUOLFoneCodecsConfigFileUpdater */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecsConfigFileUpdater;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("49EDC8FF-278C-461d-91A2-1779986528CF")
    IUOLFoneCodecsConfigFileUpdater : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CancelUpdate( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetStatus( 
            /* [in] */ LONG *plStatus) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetUpdateError( 
            /* [in] */ LONG *plError) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecsConfigFilePath( 
            /* [in] */ BSTR *pbstrCodecsConfigFilePath) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecInfoList( 
            /* [in] */ IUOLFoneCodecInfoList **ppCodecInfoList) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecsConfigFileUpdaterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecsConfigFileUpdater * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecsConfigFileUpdater * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecsConfigFileUpdater * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecsConfigFileUpdater * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecsConfigFileUpdater * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecsConfigFileUpdater * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecsConfigFileUpdater * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CancelUpdate )( 
            IUOLFoneCodecsConfigFileUpdater * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetStatus )( 
            IUOLFoneCodecsConfigFileUpdater * This,
            /* [in] */ LONG *plStatus);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUpdateError )( 
            IUOLFoneCodecsConfigFileUpdater * This,
            /* [in] */ LONG *plError);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecsConfigFilePath )( 
            IUOLFoneCodecsConfigFileUpdater * This,
            /* [in] */ BSTR *pbstrCodecsConfigFilePath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecInfoList )( 
            IUOLFoneCodecsConfigFileUpdater * This,
            /* [in] */ IUOLFoneCodecInfoList **ppCodecInfoList);
        
        END_INTERFACE
    } IUOLFoneCodecsConfigFileUpdaterVtbl;

    interface IUOLFoneCodecsConfigFileUpdater
    {
        CONST_VTBL struct IUOLFoneCodecsConfigFileUpdaterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecsConfigFileUpdater_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecsConfigFileUpdater_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecsConfigFileUpdater_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecsConfigFileUpdater_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecsConfigFileUpdater_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecsConfigFileUpdater_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecsConfigFileUpdater_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecsConfigFileUpdater_CancelUpdate(This)	\
    ( (This)->lpVtbl -> CancelUpdate(This) ) 

#define IUOLFoneCodecsConfigFileUpdater_GetStatus(This,plStatus)	\
    ( (This)->lpVtbl -> GetStatus(This,plStatus) ) 

#define IUOLFoneCodecsConfigFileUpdater_GetUpdateError(This,plError)	\
    ( (This)->lpVtbl -> GetUpdateError(This,plError) ) 

#define IUOLFoneCodecsConfigFileUpdater_GetCodecsConfigFilePath(This,pbstrCodecsConfigFilePath)	\
    ( (This)->lpVtbl -> GetCodecsConfigFilePath(This,pbstrCodecsConfigFilePath) ) 

#define IUOLFoneCodecsConfigFileUpdater_GetCodecInfoList(This,ppCodecInfoList)	\
    ( (This)->lpVtbl -> GetCodecInfoList(This,ppCodecInfoList) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecsConfigFileUpdater_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneCodecsConfigFileUpdater2_INTERFACE_DEFINED__
#define __IUOLFoneCodecsConfigFileUpdater2_INTERFACE_DEFINED__

/* interface IUOLFoneCodecsConfigFileUpdater2 */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecsConfigFileUpdater2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DBE8A198-0A3A-4e04-9083-A472CEAD948A")
    IUOLFoneCodecsConfigFileUpdater2 : public IUOLFoneCodecsConfigFileUpdater
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetDownloadFailureInfo( 
            /* [out] */ LONG *plAdditionalErrorCode,
            /* [out] */ LONG *plHTTPStatusCode,
            /* [out] */ BSTR *pbstrHTTPStatusMessage) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE StartUpdate( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecsConfigFileUpdater2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecsConfigFileUpdater2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecsConfigFileUpdater2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecsConfigFileUpdater2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecsConfigFileUpdater2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecsConfigFileUpdater2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecsConfigFileUpdater2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecsConfigFileUpdater2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CancelUpdate )( 
            IUOLFoneCodecsConfigFileUpdater2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetStatus )( 
            IUOLFoneCodecsConfigFileUpdater2 * This,
            /* [in] */ LONG *plStatus);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUpdateError )( 
            IUOLFoneCodecsConfigFileUpdater2 * This,
            /* [in] */ LONG *plError);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecsConfigFilePath )( 
            IUOLFoneCodecsConfigFileUpdater2 * This,
            /* [in] */ BSTR *pbstrCodecsConfigFilePath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecInfoList )( 
            IUOLFoneCodecsConfigFileUpdater2 * This,
            /* [in] */ IUOLFoneCodecInfoList **ppCodecInfoList);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetDownloadFailureInfo )( 
            IUOLFoneCodecsConfigFileUpdater2 * This,
            /* [out] */ LONG *plAdditionalErrorCode,
            /* [out] */ LONG *plHTTPStatusCode,
            /* [out] */ BSTR *pbstrHTTPStatusMessage);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *StartUpdate )( 
            IUOLFoneCodecsConfigFileUpdater2 * This);
        
        END_INTERFACE
    } IUOLFoneCodecsConfigFileUpdater2Vtbl;

    interface IUOLFoneCodecsConfigFileUpdater2
    {
        CONST_VTBL struct IUOLFoneCodecsConfigFileUpdater2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecsConfigFileUpdater2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecsConfigFileUpdater2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecsConfigFileUpdater2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecsConfigFileUpdater2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecsConfigFileUpdater2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecsConfigFileUpdater2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecsConfigFileUpdater2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecsConfigFileUpdater2_CancelUpdate(This)	\
    ( (This)->lpVtbl -> CancelUpdate(This) ) 

#define IUOLFoneCodecsConfigFileUpdater2_GetStatus(This,plStatus)	\
    ( (This)->lpVtbl -> GetStatus(This,plStatus) ) 

#define IUOLFoneCodecsConfigFileUpdater2_GetUpdateError(This,plError)	\
    ( (This)->lpVtbl -> GetUpdateError(This,plError) ) 

#define IUOLFoneCodecsConfigFileUpdater2_GetCodecsConfigFilePath(This,pbstrCodecsConfigFilePath)	\
    ( (This)->lpVtbl -> GetCodecsConfigFilePath(This,pbstrCodecsConfigFilePath) ) 

#define IUOLFoneCodecsConfigFileUpdater2_GetCodecInfoList(This,ppCodecInfoList)	\
    ( (This)->lpVtbl -> GetCodecInfoList(This,ppCodecInfoList) ) 


#define IUOLFoneCodecsConfigFileUpdater2_GetDownloadFailureInfo(This,plAdditionalErrorCode,plHTTPStatusCode,pbstrHTTPStatusMessage)	\
    ( (This)->lpVtbl -> GetDownloadFailureInfo(This,plAdditionalErrorCode,plHTTPStatusCode,pbstrHTTPStatusMessage) ) 

#define IUOLFoneCodecsConfigFileUpdater2_StartUpdate(This)	\
    ( (This)->lpVtbl -> StartUpdate(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecsConfigFileUpdater2_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneCodecsConfigFileUpdater3_INTERFACE_DEFINED__
#define __IUOLFoneCodecsConfigFileUpdater3_INTERFACE_DEFINED__

/* interface IUOLFoneCodecsConfigFileUpdater3 */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecsConfigFileUpdater3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8A7D69A1-1608-4558-8AE7-CF3913CDE616")
    IUOLFoneCodecsConfigFileUpdater3 : public IUOLFoneCodecsConfigFileUpdater2
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetUOLFoneClientVersion( 
            /* [in] */ BSTR bstrUOLFoneClientVersion) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RunCriticalCodecsUpdateCleanUp( 
            /* [in] */ BSTR bstrCodecsLocalDir) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecsConfigFileUpdater3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecsConfigFileUpdater3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecsConfigFileUpdater3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CancelUpdate )( 
            IUOLFoneCodecsConfigFileUpdater3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetStatus )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [in] */ LONG *plStatus);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUpdateError )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [in] */ LONG *plError);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecsConfigFilePath )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [in] */ BSTR *pbstrCodecsConfigFilePath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecInfoList )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [in] */ IUOLFoneCodecInfoList **ppCodecInfoList);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetDownloadFailureInfo )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [out] */ LONG *plAdditionalErrorCode,
            /* [out] */ LONG *plHTTPStatusCode,
            /* [out] */ BSTR *pbstrHTTPStatusMessage);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *StartUpdate )( 
            IUOLFoneCodecsConfigFileUpdater3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetUOLFoneClientVersion )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [in] */ BSTR bstrUOLFoneClientVersion);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RunCriticalCodecsUpdateCleanUp )( 
            IUOLFoneCodecsConfigFileUpdater3 * This,
            /* [in] */ BSTR bstrCodecsLocalDir);
        
        END_INTERFACE
    } IUOLFoneCodecsConfigFileUpdater3Vtbl;

    interface IUOLFoneCodecsConfigFileUpdater3
    {
        CONST_VTBL struct IUOLFoneCodecsConfigFileUpdater3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecsConfigFileUpdater3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecsConfigFileUpdater3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecsConfigFileUpdater3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecsConfigFileUpdater3_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecsConfigFileUpdater3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecsConfigFileUpdater3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecsConfigFileUpdater3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecsConfigFileUpdater3_CancelUpdate(This)	\
    ( (This)->lpVtbl -> CancelUpdate(This) ) 

#define IUOLFoneCodecsConfigFileUpdater3_GetStatus(This,plStatus)	\
    ( (This)->lpVtbl -> GetStatus(This,plStatus) ) 

#define IUOLFoneCodecsConfigFileUpdater3_GetUpdateError(This,plError)	\
    ( (This)->lpVtbl -> GetUpdateError(This,plError) ) 

#define IUOLFoneCodecsConfigFileUpdater3_GetCodecsConfigFilePath(This,pbstrCodecsConfigFilePath)	\
    ( (This)->lpVtbl -> GetCodecsConfigFilePath(This,pbstrCodecsConfigFilePath) ) 

#define IUOLFoneCodecsConfigFileUpdater3_GetCodecInfoList(This,ppCodecInfoList)	\
    ( (This)->lpVtbl -> GetCodecInfoList(This,ppCodecInfoList) ) 


#define IUOLFoneCodecsConfigFileUpdater3_GetDownloadFailureInfo(This,plAdditionalErrorCode,plHTTPStatusCode,pbstrHTTPStatusMessage)	\
    ( (This)->lpVtbl -> GetDownloadFailureInfo(This,plAdditionalErrorCode,plHTTPStatusCode,pbstrHTTPStatusMessage) ) 

#define IUOLFoneCodecsConfigFileUpdater3_StartUpdate(This)	\
    ( (This)->lpVtbl -> StartUpdate(This) ) 


#define IUOLFoneCodecsConfigFileUpdater3_SetUOLFoneClientVersion(This,bstrUOLFoneClientVersion)	\
    ( (This)->lpVtbl -> SetUOLFoneClientVersion(This,bstrUOLFoneClientVersion) ) 

#define IUOLFoneCodecsConfigFileUpdater3_RunCriticalCodecsUpdateCleanUp(This,bstrCodecsLocalDir)	\
    ( (This)->lpVtbl -> RunCriticalCodecsUpdateCleanUp(This,bstrCodecsLocalDir) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecsConfigFileUpdater3_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneCodecsConfigFileUpdaterCallback_INTERFACE_DEFINED__
#define __IUOLFoneCodecsConfigFileUpdaterCallback_INTERFACE_DEFINED__

/* interface IUOLFoneCodecsConfigFileUpdaterCallback */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecsConfigFileUpdaterCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FEB0B9B8-9331-4dbb-959D-996FD8CD7382")
    IUOLFoneCodecsConfigFileUpdaterCallback : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UpdateConfigComplete( 
            /* [in] */ LONG lResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecsConfigFileUpdaterCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecsConfigFileUpdaterCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecsConfigFileUpdaterCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecsConfigFileUpdaterCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecsConfigFileUpdaterCallback * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecsConfigFileUpdaterCallback * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecsConfigFileUpdaterCallback * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecsConfigFileUpdaterCallback * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UpdateConfigComplete )( 
            IUOLFoneCodecsConfigFileUpdaterCallback * This,
            /* [in] */ LONG lResult);
        
        END_INTERFACE
    } IUOLFoneCodecsConfigFileUpdaterCallbackVtbl;

    interface IUOLFoneCodecsConfigFileUpdaterCallback
    {
        CONST_VTBL struct IUOLFoneCodecsConfigFileUpdaterCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecsConfigFileUpdaterCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecsConfigFileUpdaterCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecsConfigFileUpdaterCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecsConfigFileUpdaterCallback_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecsConfigFileUpdaterCallback_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecsConfigFileUpdaterCallback_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecsConfigFileUpdaterCallback_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecsConfigFileUpdaterCallback_UpdateConfigComplete(This,lResult)	\
    ( (This)->lpVtbl -> UpdateConfigComplete(This,lResult) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecsConfigFileUpdaterCallback_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneCodecsUpdateInfo_INTERFACE_DEFINED__
#define __IUOLFoneCodecsUpdateInfo_INTERFACE_DEFINED__

/* interface IUOLFoneCodecsUpdateInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecsUpdateInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BD9D86BF-7FE3-4715-9C81-329602F76A07")
    IUOLFoneCodecsUpdateInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetAvailableUpdatesCount( 
            /* [out] */ LONG *plCount) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecsUpdateInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecsUpdateInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecsUpdateInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecsUpdateInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecsUpdateInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecsUpdateInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecsUpdateInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecsUpdateInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetAvailableUpdatesCount )( 
            IUOLFoneCodecsUpdateInfo * This,
            /* [out] */ LONG *plCount);
        
        END_INTERFACE
    } IUOLFoneCodecsUpdateInfoVtbl;

    interface IUOLFoneCodecsUpdateInfo
    {
        CONST_VTBL struct IUOLFoneCodecsUpdateInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecsUpdateInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecsUpdateInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecsUpdateInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecsUpdateInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecsUpdateInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecsUpdateInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecsUpdateInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecsUpdateInfo_GetAvailableUpdatesCount(This,plCount)	\
    ( (This)->lpVtbl -> GetAvailableUpdatesCount(This,plCount) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecsUpdateInfo_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneServerConfigProvider_0000_0008 */
/* [local] */ 


enum EnumUpdateStatus
    {	UFS_UPDATE_STATUS_SUCCESS	= 0,
	UFS_UPDATE_MISSING_PARAM	= 1,
	UFS_UPDATE_STATUS_FAILED	= 2,
	UFS_UPDATING_CONFIG_FILE	= 3,
	UFS_UPDATING_CODEC_FILE	= 4,
	UFS_UPDATE_PROCESS_XML_FAILED	= 5,
	UFS_UPDATE_DOWNLOAD_FAILED	= 6,
	UFS_UPDATE_STOPPED	= 7
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneServerConfigProvider_0000_0008_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneServerConfigProvider_0000_0008_v0_0_s_ifspec;

#ifndef __IUOLFoneCodecsUpdater_INTERFACE_DEFINED__
#define __IUOLFoneCodecsUpdater_INTERFACE_DEFINED__

/* interface IUOLFoneCodecsUpdater */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecsUpdater;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B0A5240F-5F08-431e-860F-8C19A12664BC")
    IUOLFoneCodecsUpdater : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ProcessUpdateList( 
            /* [out] */ IUOLFoneCodecsUpdateInfo **ppCodecsUpdateInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE StartUpdate( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CancelUpdate( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetStatus( 
            /* [in] */ LONG *plStatus) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetUpdateError( 
            /* [in] */ LONG *plError) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecsUpdaterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecsUpdater * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecsUpdater * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecsUpdater * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecsUpdater * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecsUpdater * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecsUpdater * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecsUpdater * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ProcessUpdateList )( 
            IUOLFoneCodecsUpdater * This,
            /* [out] */ IUOLFoneCodecsUpdateInfo **ppCodecsUpdateInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *StartUpdate )( 
            IUOLFoneCodecsUpdater * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CancelUpdate )( 
            IUOLFoneCodecsUpdater * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetStatus )( 
            IUOLFoneCodecsUpdater * This,
            /* [in] */ LONG *plStatus);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUpdateError )( 
            IUOLFoneCodecsUpdater * This,
            /* [in] */ LONG *plError);
        
        END_INTERFACE
    } IUOLFoneCodecsUpdaterVtbl;

    interface IUOLFoneCodecsUpdater
    {
        CONST_VTBL struct IUOLFoneCodecsUpdaterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecsUpdater_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecsUpdater_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecsUpdater_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecsUpdater_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecsUpdater_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecsUpdater_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecsUpdater_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecsUpdater_ProcessUpdateList(This,ppCodecsUpdateInfo)	\
    ( (This)->lpVtbl -> ProcessUpdateList(This,ppCodecsUpdateInfo) ) 

#define IUOLFoneCodecsUpdater_StartUpdate(This)	\
    ( (This)->lpVtbl -> StartUpdate(This) ) 

#define IUOLFoneCodecsUpdater_CancelUpdate(This)	\
    ( (This)->lpVtbl -> CancelUpdate(This) ) 

#define IUOLFoneCodecsUpdater_GetStatus(This,plStatus)	\
    ( (This)->lpVtbl -> GetStatus(This,plStatus) ) 

#define IUOLFoneCodecsUpdater_GetUpdateError(This,plError)	\
    ( (This)->lpVtbl -> GetUpdateError(This,plError) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecsUpdater_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneCodecsUpdater2_INTERFACE_DEFINED__
#define __IUOLFoneCodecsUpdater2_INTERFACE_DEFINED__

/* interface IUOLFoneCodecsUpdater2 */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecsUpdater2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("648C858C-1159-41ed-B2C2-49A931BC5AF1")
    IUOLFoneCodecsUpdater2 : public IUOLFoneCodecsUpdater
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetDownloadFailureInfo( 
            /* [out] */ LONG *plAdditionalErrorCode,
            /* [out] */ LONG *plHTTPStatusCode,
            /* [out] */ BSTR *pbstrHTTPStatusMessage,
            /* [out] */ BSTR *pbstrFailedFileURL,
            /* [out] */ BSTR *pbstrFailedFilePath) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecsUpdater2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecsUpdater2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecsUpdater2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecsUpdater2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecsUpdater2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecsUpdater2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecsUpdater2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecsUpdater2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ProcessUpdateList )( 
            IUOLFoneCodecsUpdater2 * This,
            /* [out] */ IUOLFoneCodecsUpdateInfo **ppCodecsUpdateInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *StartUpdate )( 
            IUOLFoneCodecsUpdater2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CancelUpdate )( 
            IUOLFoneCodecsUpdater2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetStatus )( 
            IUOLFoneCodecsUpdater2 * This,
            /* [in] */ LONG *plStatus);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUpdateError )( 
            IUOLFoneCodecsUpdater2 * This,
            /* [in] */ LONG *plError);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetDownloadFailureInfo )( 
            IUOLFoneCodecsUpdater2 * This,
            /* [out] */ LONG *plAdditionalErrorCode,
            /* [out] */ LONG *plHTTPStatusCode,
            /* [out] */ BSTR *pbstrHTTPStatusMessage,
            /* [out] */ BSTR *pbstrFailedFileURL,
            /* [out] */ BSTR *pbstrFailedFilePath);
        
        END_INTERFACE
    } IUOLFoneCodecsUpdater2Vtbl;

    interface IUOLFoneCodecsUpdater2
    {
        CONST_VTBL struct IUOLFoneCodecsUpdater2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecsUpdater2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecsUpdater2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecsUpdater2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecsUpdater2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecsUpdater2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecsUpdater2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecsUpdater2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecsUpdater2_ProcessUpdateList(This,ppCodecsUpdateInfo)	\
    ( (This)->lpVtbl -> ProcessUpdateList(This,ppCodecsUpdateInfo) ) 

#define IUOLFoneCodecsUpdater2_StartUpdate(This)	\
    ( (This)->lpVtbl -> StartUpdate(This) ) 

#define IUOLFoneCodecsUpdater2_CancelUpdate(This)	\
    ( (This)->lpVtbl -> CancelUpdate(This) ) 

#define IUOLFoneCodecsUpdater2_GetStatus(This,plStatus)	\
    ( (This)->lpVtbl -> GetStatus(This,plStatus) ) 

#define IUOLFoneCodecsUpdater2_GetUpdateError(This,plError)	\
    ( (This)->lpVtbl -> GetUpdateError(This,plError) ) 


#define IUOLFoneCodecsUpdater2_GetDownloadFailureInfo(This,plAdditionalErrorCode,plHTTPStatusCode,pbstrHTTPStatusMessage,pbstrFailedFileURL,pbstrFailedFilePath)	\
    ( (This)->lpVtbl -> GetDownloadFailureInfo(This,plAdditionalErrorCode,plHTTPStatusCode,pbstrHTTPStatusMessage,pbstrFailedFileURL,pbstrFailedFilePath) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecsUpdater2_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneCodecsUpdater3_INTERFACE_DEFINED__
#define __IUOLFoneCodecsUpdater3_INTERFACE_DEFINED__

/* interface IUOLFoneCodecsUpdater3 */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecsUpdater3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4E15B43E-3CEE-4407-AD7A-C18CC47C5F1D")
    IUOLFoneCodecsUpdater3 : public IUOLFoneCodecsUpdater2
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetUOLFoneClientVersion( 
            /* [in] */ BSTR bstrUOLFoneClientVersion) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecsUpdater3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecsUpdater3 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecsUpdater3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecsUpdater3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecsUpdater3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecsUpdater3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecsUpdater3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecsUpdater3 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ProcessUpdateList )( 
            IUOLFoneCodecsUpdater3 * This,
            /* [out] */ IUOLFoneCodecsUpdateInfo **ppCodecsUpdateInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *StartUpdate )( 
            IUOLFoneCodecsUpdater3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CancelUpdate )( 
            IUOLFoneCodecsUpdater3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetStatus )( 
            IUOLFoneCodecsUpdater3 * This,
            /* [in] */ LONG *plStatus);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUpdateError )( 
            IUOLFoneCodecsUpdater3 * This,
            /* [in] */ LONG *plError);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetDownloadFailureInfo )( 
            IUOLFoneCodecsUpdater3 * This,
            /* [out] */ LONG *plAdditionalErrorCode,
            /* [out] */ LONG *plHTTPStatusCode,
            /* [out] */ BSTR *pbstrHTTPStatusMessage,
            /* [out] */ BSTR *pbstrFailedFileURL,
            /* [out] */ BSTR *pbstrFailedFilePath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetUOLFoneClientVersion )( 
            IUOLFoneCodecsUpdater3 * This,
            /* [in] */ BSTR bstrUOLFoneClientVersion);
        
        END_INTERFACE
    } IUOLFoneCodecsUpdater3Vtbl;

    interface IUOLFoneCodecsUpdater3
    {
        CONST_VTBL struct IUOLFoneCodecsUpdater3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecsUpdater3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecsUpdater3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecsUpdater3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecsUpdater3_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecsUpdater3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecsUpdater3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecsUpdater3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecsUpdater3_ProcessUpdateList(This,ppCodecsUpdateInfo)	\
    ( (This)->lpVtbl -> ProcessUpdateList(This,ppCodecsUpdateInfo) ) 

#define IUOLFoneCodecsUpdater3_StartUpdate(This)	\
    ( (This)->lpVtbl -> StartUpdate(This) ) 

#define IUOLFoneCodecsUpdater3_CancelUpdate(This)	\
    ( (This)->lpVtbl -> CancelUpdate(This) ) 

#define IUOLFoneCodecsUpdater3_GetStatus(This,plStatus)	\
    ( (This)->lpVtbl -> GetStatus(This,plStatus) ) 

#define IUOLFoneCodecsUpdater3_GetUpdateError(This,plError)	\
    ( (This)->lpVtbl -> GetUpdateError(This,plError) ) 


#define IUOLFoneCodecsUpdater3_GetDownloadFailureInfo(This,plAdditionalErrorCode,plHTTPStatusCode,pbstrHTTPStatusMessage,pbstrFailedFileURL,pbstrFailedFilePath)	\
    ( (This)->lpVtbl -> GetDownloadFailureInfo(This,plAdditionalErrorCode,plHTTPStatusCode,pbstrHTTPStatusMessage,pbstrFailedFileURL,pbstrFailedFilePath) ) 


#define IUOLFoneCodecsUpdater3_SetUOLFoneClientVersion(This,bstrUOLFoneClientVersion)	\
    ( (This)->lpVtbl -> SetUOLFoneClientVersion(This,bstrUOLFoneClientVersion) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecsUpdater3_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneCodecsUpdaterCallback_INTERFACE_DEFINED__
#define __IUOLFoneCodecsUpdaterCallback_INTERFACE_DEFINED__

/* interface IUOLFoneCodecsUpdaterCallback */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneCodecsUpdaterCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8E2E60A8-58A1-4a44-860B-530A6BDB5AFE")
    IUOLFoneCodecsUpdaterCallback : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UpdateCodecFileBegin( 
            /* [in] */ BSTR codecName,
            /* [in] */ BSTR fileName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UpdateCodecFileProgressChange( 
            /* [in] */ BSTR codecName,
            /* [in] */ BSTR fileName,
            /* [in] */ ULONG nProgress,
            /* [in] */ ULONG nProgressMax) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UpdateCodecFileComplete( 
            /* [in] */ BSTR codecName,
            /* [in] */ BSTR fileName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UpdateCodecComplete( 
            /* [in] */ BSTR codecName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UpdateCodecsFinished( 
            /* [in] */ LONG lResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneCodecsUpdaterCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneCodecsUpdaterCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneCodecsUpdaterCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneCodecsUpdaterCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneCodecsUpdaterCallback * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneCodecsUpdaterCallback * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneCodecsUpdaterCallback * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneCodecsUpdaterCallback * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UpdateCodecFileBegin )( 
            IUOLFoneCodecsUpdaterCallback * This,
            /* [in] */ BSTR codecName,
            /* [in] */ BSTR fileName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UpdateCodecFileProgressChange )( 
            IUOLFoneCodecsUpdaterCallback * This,
            /* [in] */ BSTR codecName,
            /* [in] */ BSTR fileName,
            /* [in] */ ULONG nProgress,
            /* [in] */ ULONG nProgressMax);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UpdateCodecFileComplete )( 
            IUOLFoneCodecsUpdaterCallback * This,
            /* [in] */ BSTR codecName,
            /* [in] */ BSTR fileName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UpdateCodecComplete )( 
            IUOLFoneCodecsUpdaterCallback * This,
            /* [in] */ BSTR codecName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UpdateCodecsFinished )( 
            IUOLFoneCodecsUpdaterCallback * This,
            /* [in] */ LONG lResult);
        
        END_INTERFACE
    } IUOLFoneCodecsUpdaterCallbackVtbl;

    interface IUOLFoneCodecsUpdaterCallback
    {
        CONST_VTBL struct IUOLFoneCodecsUpdaterCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneCodecsUpdaterCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneCodecsUpdaterCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneCodecsUpdaterCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneCodecsUpdaterCallback_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneCodecsUpdaterCallback_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneCodecsUpdaterCallback_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneCodecsUpdaterCallback_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneCodecsUpdaterCallback_UpdateCodecFileBegin(This,codecName,fileName)	\
    ( (This)->lpVtbl -> UpdateCodecFileBegin(This,codecName,fileName) ) 

#define IUOLFoneCodecsUpdaterCallback_UpdateCodecFileProgressChange(This,codecName,fileName,nProgress,nProgressMax)	\
    ( (This)->lpVtbl -> UpdateCodecFileProgressChange(This,codecName,fileName,nProgress,nProgressMax) ) 

#define IUOLFoneCodecsUpdaterCallback_UpdateCodecFileComplete(This,codecName,fileName)	\
    ( (This)->lpVtbl -> UpdateCodecFileComplete(This,codecName,fileName) ) 

#define IUOLFoneCodecsUpdaterCallback_UpdateCodecComplete(This,codecName)	\
    ( (This)->lpVtbl -> UpdateCodecComplete(This,codecName) ) 

#define IUOLFoneCodecsUpdaterCallback_UpdateCodecsFinished(This,lResult)	\
    ( (This)->lpVtbl -> UpdateCodecsFinished(This,lResult) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneCodecsUpdaterCallback_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneServerConfigProvider_0000_0012 */
/* [local] */ 


enum EnumDownloadStatus
    {	UFS_DOWNLOAD_STATUS_SUCCESS	= 0,
	UFS_DOWNLOAD_STATUS_FAILED	= 1,
	UFS_DOWNLOAD_INVALID_PARAM	= 2,
	UFS_DOWNLOAD_URL_NOT_FOUND	= 3,
	UFS_DOWNLOAD_STOPPED	= 4
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneServerConfigProvider_0000_0012_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneServerConfigProvider_0000_0012_v0_0_s_ifspec;

#ifndef __IUOLFoneDownload_INTERFACE_DEFINED__
#define __IUOLFoneDownload_INTERFACE_DEFINED__

/* interface IUOLFoneDownload */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneDownload;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5603336C-7675-49c9-A630-8B85CBD4798A")
    IUOLFoneDownload : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetId( 
            /* [in] */ LONG *plId) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetUrl( 
            /* [in] */ BSTR *pbstrUrl) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetStatus( 
            /* [in] */ LONG *plStatus) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneDownloadVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneDownload * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneDownload * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneDownload * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneDownload * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneDownload * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneDownload * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneDownload * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetId )( 
            IUOLFoneDownload * This,
            /* [in] */ LONG *plId);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUrl )( 
            IUOLFoneDownload * This,
            /* [in] */ BSTR *pbstrUrl);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetStatus )( 
            IUOLFoneDownload * This,
            /* [in] */ LONG *plStatus);
        
        END_INTERFACE
    } IUOLFoneDownloadVtbl;

    interface IUOLFoneDownload
    {
        CONST_VTBL struct IUOLFoneDownloadVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneDownload_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneDownload_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneDownload_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneDownload_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneDownload_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneDownload_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneDownload_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneDownload_GetId(This,plId)	\
    ( (This)->lpVtbl -> GetId(This,plId) ) 

#define IUOLFoneDownload_GetUrl(This,pbstrUrl)	\
    ( (This)->lpVtbl -> GetUrl(This,pbstrUrl) ) 

#define IUOLFoneDownload_GetStatus(This,plStatus)	\
    ( (This)->lpVtbl -> GetStatus(This,plStatus) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneDownload_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneDownloadCallback_INTERFACE_DEFINED__
#define __IUOLFoneDownloadCallback_INTERFACE_DEFINED__

/* interface IUOLFoneDownloadCallback */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneDownloadCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E0BDA14F-0617-4bc5-9F71-5D48014D781F")
    IUOLFoneDownloadCallback : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DownloadComplete( 
            /* [in] */ IUOLFoneDownload *pDownload) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneDownloadCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneDownloadCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneDownloadCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneDownloadCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneDownloadCallback * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneDownloadCallback * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneDownloadCallback * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneDownloadCallback * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DownloadComplete )( 
            IUOLFoneDownloadCallback * This,
            /* [in] */ IUOLFoneDownload *pDownload);
        
        END_INTERFACE
    } IUOLFoneDownloadCallbackVtbl;

    interface IUOLFoneDownloadCallback
    {
        CONST_VTBL struct IUOLFoneDownloadCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneDownloadCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneDownloadCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneDownloadCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneDownloadCallback_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneDownloadCallback_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneDownloadCallback_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneDownloadCallback_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneDownloadCallback_DownloadComplete(This,pDownload)	\
    ( (This)->lpVtbl -> DownloadComplete(This,pDownload) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneDownloadCallback_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneServerConfigProvider_0000_0014 */
/* [local] */ 


enum EnumMessageType
    {	UFS_MESSAGE_TYPE_UNKNOWN	= 0,
	UFS_MESSAGE_TYPE_STATUS_CODE	= 1,
	UFS_MESSAGE_TYPE_ERROR_MESSAGE	= 10,
	UFS_MESSAGE_TYPE_ERROR_URL	= 11,
	UFS_MESSAGE_TYPE_DISPLAY_URL	= 12,
	UFS_MESSAGE_TYPE_CREDITS	= 20,
	UFS_MESSAGE_TYPE_CALL_COST	= 21,
	UFS_MESSAGE_TYPE_RATE	= 22,
	UFS_MESSAGE_TYPE_PLAN_NAME	= 40,
	UFS_MESSAGE_TYPE_PLAN_AVAIL_SECS	= 41,
	UFS_MESSAGE_TYPE_PLAN_USED_SECS	= 42,
	UFS_MESSAGE_TYPE_SESSION_USE_CREDITS	= 43,
	UFS_MESSAGE_TYPE_CONNECTION_FEE	= 44,
	UFS_MESSAGE_TYPE_AVAILABLE_PRODUCTS	= 45
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneServerConfigProvider_0000_0014_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneServerConfigProvider_0000_0014_v0_0_s_ifspec;

#ifndef __IUOLFoneMessageInfo_INTERFACE_DEFINED__
#define __IUOLFoneMessageInfo_INTERFACE_DEFINED__

/* interface IUOLFoneMessageInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneMessageInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("75F25BBD-91D1-4e3b-A12E-B3FD29E086B9")
    IUOLFoneMessageInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetType( 
            /* [in] */ LONG *plType) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetValue( 
            /* [in] */ VARIANT *pVarValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneMessageInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneMessageInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneMessageInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneMessageInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneMessageInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneMessageInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneMessageInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneMessageInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IUOLFoneMessageInfo * This,
            /* [in] */ LONG *plType);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IUOLFoneMessageInfo * This,
            /* [in] */ VARIANT *pVarValue);
        
        END_INTERFACE
    } IUOLFoneMessageInfoVtbl;

    interface IUOLFoneMessageInfo
    {
        CONST_VTBL struct IUOLFoneMessageInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneMessageInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneMessageInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneMessageInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneMessageInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneMessageInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneMessageInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneMessageInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneMessageInfo_GetType(This,plType)	\
    ( (This)->lpVtbl -> GetType(This,plType) ) 

#define IUOLFoneMessageInfo_GetValue(This,pVarValue)	\
    ( (This)->lpVtbl -> GetValue(This,pVarValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneMessageInfo_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneMessageInfoEnum_INTERFACE_DEFINED__
#define __IUOLFoneMessageInfoEnum_INTERFACE_DEFINED__

/* interface IUOLFoneMessageInfoEnum */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneMessageInfoEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C15A08CA-E523-4e6f-BB1A-BB45D6CC5B32")
    IUOLFoneMessageInfoEnum : public IUnknown
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [in] */ IUOLFoneMessageInfo **ppElements,
            /* [in] */ ULONG *pceltFetched) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [in] */ IUOLFoneMessageInfoEnum **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneMessageInfoEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneMessageInfoEnum * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneMessageInfoEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneMessageInfoEnum * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IUOLFoneMessageInfoEnum * This,
            /* [in] */ ULONG celt,
            /* [in] */ IUOLFoneMessageInfo **ppElements,
            /* [in] */ ULONG *pceltFetched);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IUOLFoneMessageInfoEnum * This,
            /* [in] */ ULONG celt);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IUOLFoneMessageInfoEnum * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IUOLFoneMessageInfoEnum * This,
            /* [in] */ IUOLFoneMessageInfoEnum **ppEnum);
        
        END_INTERFACE
    } IUOLFoneMessageInfoEnumVtbl;

    interface IUOLFoneMessageInfoEnum
    {
        CONST_VTBL struct IUOLFoneMessageInfoEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneMessageInfoEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneMessageInfoEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneMessageInfoEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneMessageInfoEnum_Next(This,celt,ppElements,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,ppElements,pceltFetched) ) 

#define IUOLFoneMessageInfoEnum_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IUOLFoneMessageInfoEnum_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IUOLFoneMessageInfoEnum_Clone(This,ppEnum)	\
    ( (This)->lpVtbl -> Clone(This,ppEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneMessageInfoEnum_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneMessageParser_INTERFACE_DEFINED__
#define __IUOLFoneMessageParser_INTERFACE_DEFINED__

/* interface IUOLFoneMessageParser */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneMessageParser;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("03C89507-A7AE-4673-A3F6-2BEE6956F1C7")
    IUOLFoneMessageParser : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ParseMessage( 
            /* [in] */ BSTR bstrMessage,
            /* [in] */ IUOLFoneMessageInfoEnum **ppMessageInfoEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneMessageParserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneMessageParser * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneMessageParser * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneMessageParser * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneMessageParser * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneMessageParser * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneMessageParser * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneMessageParser * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ParseMessage )( 
            IUOLFoneMessageParser * This,
            /* [in] */ BSTR bstrMessage,
            /* [in] */ IUOLFoneMessageInfoEnum **ppMessageInfoEnum);
        
        END_INTERFACE
    } IUOLFoneMessageParserVtbl;

    interface IUOLFoneMessageParser
    {
        CONST_VTBL struct IUOLFoneMessageParserVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneMessageParser_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneMessageParser_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneMessageParser_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneMessageParser_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneMessageParser_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneMessageParser_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneMessageParser_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneMessageParser_ParseMessage(This,bstrMessage,ppMessageInfoEnum)	\
    ( (This)->lpVtbl -> ParseMessage(This,bstrMessage,ppMessageInfoEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneMessageParser_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneOption_INTERFACE_DEFINED__
#define __IUOLFoneOption_INTERFACE_DEFINED__

/* interface IUOLFoneOption */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneOption;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3FB7CE86-7DAB-46ea-BBDB-ED51C89804D4")
    IUOLFoneOption : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetValue( 
            /* [out] */ BSTR *pbstrValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneOptionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneOption * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneOption * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneOption * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneOption * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneOption * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneOption * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneOption * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IUOLFoneOption * This,
            /* [out] */ BSTR *pbstrName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IUOLFoneOption * This,
            /* [out] */ BSTR *pbstrValue);
        
        END_INTERFACE
    } IUOLFoneOptionVtbl;

    interface IUOLFoneOption
    {
        CONST_VTBL struct IUOLFoneOptionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneOption_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneOption_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneOption_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneOption_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneOption_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneOption_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneOption_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneOption_GetName(This,pbstrName)	\
    ( (This)->lpVtbl -> GetName(This,pbstrName) ) 

#define IUOLFoneOption_GetValue(This,pbstrValue)	\
    ( (This)->lpVtbl -> GetValue(This,pbstrValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneOption_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneOptionEnum_INTERFACE_DEFINED__
#define __IUOLFoneOptionEnum_INTERFACE_DEFINED__

/* interface IUOLFoneOptionEnum */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneOptionEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5D1F93E8-14A6-4557-98E9-2C75B3852946")
    IUOLFoneOptionEnum : public IUnknown
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [in] */ IUOLFoneOption **ppElements,
            /* [in] */ ULONG *pceltFetched) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [in] */ IUOLFoneOptionEnum **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneOptionEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneOptionEnum * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneOptionEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneOptionEnum * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IUOLFoneOptionEnum * This,
            /* [in] */ ULONG celt,
            /* [in] */ IUOLFoneOption **ppElements,
            /* [in] */ ULONG *pceltFetched);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IUOLFoneOptionEnum * This,
            /* [in] */ ULONG celt);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IUOLFoneOptionEnum * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IUOLFoneOptionEnum * This,
            /* [in] */ IUOLFoneOptionEnum **ppEnum);
        
        END_INTERFACE
    } IUOLFoneOptionEnumVtbl;

    interface IUOLFoneOptionEnum
    {
        CONST_VTBL struct IUOLFoneOptionEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneOptionEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneOptionEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneOptionEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneOptionEnum_Next(This,celt,ppElements,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,ppElements,pceltFetched) ) 

#define IUOLFoneOptionEnum_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IUOLFoneOptionEnum_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IUOLFoneOptionEnum_Clone(This,ppEnum)	\
    ( (This)->lpVtbl -> Clone(This,ppEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneOptionEnum_INTERFACE_DEFINED__ */


#ifndef __IUOLFonePhoneValidator_INTERFACE_DEFINED__
#define __IUOLFonePhoneValidator_INTERFACE_DEFINED__

/* interface IUOLFonePhoneValidator */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFonePhoneValidator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CC4514F7-9D77-4087-8F00-1D7783785398")
    IUOLFonePhoneValidator : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsValidPhoneNumber( 
            /* [in] */ BSTR bstrPhoneNumber,
            /* [out] */ BOOL *pValid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFonePhoneValidatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFonePhoneValidator * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFonePhoneValidator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFonePhoneValidator * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFonePhoneValidator * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFonePhoneValidator * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFonePhoneValidator * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFonePhoneValidator * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsValidPhoneNumber )( 
            IUOLFonePhoneValidator * This,
            /* [in] */ BSTR bstrPhoneNumber,
            /* [out] */ BOOL *pValid);
        
        END_INTERFACE
    } IUOLFonePhoneValidatorVtbl;

    interface IUOLFonePhoneValidator
    {
        CONST_VTBL struct IUOLFonePhoneValidatorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFonePhoneValidator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFonePhoneValidator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFonePhoneValidator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFonePhoneValidator_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFonePhoneValidator_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFonePhoneValidator_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFonePhoneValidator_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFonePhoneValidator_IsValidPhoneNumber(This,bstrPhoneNumber,pValid)	\
    ( (This)->lpVtbl -> IsValidPhoneNumber(This,bstrPhoneNumber,pValid) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFonePhoneValidator_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneSettings_INTERFACE_DEFINED__
#define __IUOLFoneSettings_INTERFACE_DEFINED__

/* interface IUOLFoneSettings */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneSettings;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EBBE573C-E5D3-4360-AA6E-7756FB82F3E6")
    IUOLFoneSettings : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetOptionValue( 
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrValue) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetOptionValue( 
            /* [in] */ BSTR bstrName,
            /* [out] */ BSTR *pbstrValue) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetOption( 
            /* [in] */ BSTR bstrName,
            /* [out] */ IUOLFoneOption **ppOption) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RemoveOption( 
            /* [in] */ BSTR bstrName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EnumerateOptions( 
            /* [out] */ IUOLFoneOptionEnum **ppOptionEnum) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ResetAll( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneSettingsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneSettings * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneSettings * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneSettings * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneSettings * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneSettings * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneSettings * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneSettings * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetOptionValue )( 
            IUOLFoneSettings * This,
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrValue);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetOptionValue )( 
            IUOLFoneSettings * This,
            /* [in] */ BSTR bstrName,
            /* [out] */ BSTR *pbstrValue);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetOption )( 
            IUOLFoneSettings * This,
            /* [in] */ BSTR bstrName,
            /* [out] */ IUOLFoneOption **ppOption);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RemoveOption )( 
            IUOLFoneSettings * This,
            /* [in] */ BSTR bstrName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnumerateOptions )( 
            IUOLFoneSettings * This,
            /* [out] */ IUOLFoneOptionEnum **ppOptionEnum);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ResetAll )( 
            IUOLFoneSettings * This);
        
        END_INTERFACE
    } IUOLFoneSettingsVtbl;

    interface IUOLFoneSettings
    {
        CONST_VTBL struct IUOLFoneSettingsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneSettings_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneSettings_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneSettings_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneSettings_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneSettings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneSettings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneSettings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneSettings_SetOptionValue(This,bstrName,bstrValue)	\
    ( (This)->lpVtbl -> SetOptionValue(This,bstrName,bstrValue) ) 

#define IUOLFoneSettings_GetOptionValue(This,bstrName,pbstrValue)	\
    ( (This)->lpVtbl -> GetOptionValue(This,bstrName,pbstrValue) ) 

#define IUOLFoneSettings_GetOption(This,bstrName,ppOption)	\
    ( (This)->lpVtbl -> GetOption(This,bstrName,ppOption) ) 

#define IUOLFoneSettings_RemoveOption(This,bstrName)	\
    ( (This)->lpVtbl -> RemoveOption(This,bstrName) ) 

#define IUOLFoneSettings_EnumerateOptions(This,ppOptionEnum)	\
    ( (This)->lpVtbl -> EnumerateOptions(This,ppOptionEnum) ) 

#define IUOLFoneSettings_ResetAll(This)	\
    ( (This)->lpVtbl -> ResetAll(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneSettings_INTERFACE_DEFINED__ */


#ifndef __IUOLFonePhoneValidationRulesDownload_INTERFACE_DEFINED__
#define __IUOLFonePhoneValidationRulesDownload_INTERFACE_DEFINED__

/* interface IUOLFonePhoneValidationRulesDownload */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFonePhoneValidationRulesDownload;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DE925F39-9E77-473e-AF7C-64894D22117A")
    IUOLFonePhoneValidationRulesDownload : public IUOLFoneDownload
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPhoneValidator( 
            /* [out] */ IUOLFonePhoneValidator **ppPhoneValidator) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFonePhoneValidationRulesDownloadVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFonePhoneValidationRulesDownload * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFonePhoneValidationRulesDownload * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFonePhoneValidationRulesDownload * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFonePhoneValidationRulesDownload * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFonePhoneValidationRulesDownload * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFonePhoneValidationRulesDownload * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFonePhoneValidationRulesDownload * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetId )( 
            IUOLFonePhoneValidationRulesDownload * This,
            /* [in] */ LONG *plId);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUrl )( 
            IUOLFonePhoneValidationRulesDownload * This,
            /* [in] */ BSTR *pbstrUrl);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetStatus )( 
            IUOLFonePhoneValidationRulesDownload * This,
            /* [in] */ LONG *plStatus);
        
        HRESULT ( STDMETHODCALLTYPE *GetPhoneValidator )( 
            IUOLFonePhoneValidationRulesDownload * This,
            /* [out] */ IUOLFonePhoneValidator **ppPhoneValidator);
        
        END_INTERFACE
    } IUOLFonePhoneValidationRulesDownloadVtbl;

    interface IUOLFonePhoneValidationRulesDownload
    {
        CONST_VTBL struct IUOLFonePhoneValidationRulesDownloadVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFonePhoneValidationRulesDownload_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFonePhoneValidationRulesDownload_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFonePhoneValidationRulesDownload_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFonePhoneValidationRulesDownload_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFonePhoneValidationRulesDownload_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFonePhoneValidationRulesDownload_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFonePhoneValidationRulesDownload_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFonePhoneValidationRulesDownload_GetId(This,plId)	\
    ( (This)->lpVtbl -> GetId(This,plId) ) 

#define IUOLFonePhoneValidationRulesDownload_GetUrl(This,pbstrUrl)	\
    ( (This)->lpVtbl -> GetUrl(This,pbstrUrl) ) 

#define IUOLFonePhoneValidationRulesDownload_GetStatus(This,plStatus)	\
    ( (This)->lpVtbl -> GetStatus(This,plStatus) ) 


#define IUOLFonePhoneValidationRulesDownload_GetPhoneValidator(This,ppPhoneValidator)	\
    ( (This)->lpVtbl -> GetPhoneValidator(This,ppPhoneValidator) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFonePhoneValidationRulesDownload_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneServerConfigProvider_INTERFACE_DEFINED__
#define __IUOLFoneServerConfigProvider_INTERFACE_DEFINED__

/* interface IUOLFoneServerConfigProvider */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneServerConfigProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D23B2E20-C592-434C-8716-7C6413A37559")
    IUOLFoneServerConfigProvider : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Initialize( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Finalize( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DownloadPhoneValidationRules( 
            /* [in] */ BSTR bstrUrl,
            /* [in] */ IUOLFoneDownloadCallback *pDownloadCallback,
            /* [out] */ IUOLFoneDownload **ppDownload) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CancelDownload( 
            /* [in] */ IUOLFoneDownload *pDownload) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPhoneValidator( 
            /* [in] */ IUOLFoneDownload *pDownload,
            /* [out] */ IUOLFonePhoneValidator **ppPhoneValidator) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetMessageParser( 
            /* [out] */ IUOLFoneMessageParser **ppMessageParser) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CreateCodecsConfigFileUpdater( 
            /* [in] */ BSTR bstrCodecsXMLUrl,
            /* [in] */ IUOLFoneCodecsConfigFileUpdaterCallback *pCodecsConfigFileUpdaterCallback,
            /* [out] */ IUOLFoneCodecsConfigFileUpdater2 **ppCodecsConfigFileUpdater) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CreateCodecsUpdater( 
            /* [in] */ BSTR bstrCodecsXMLPath,
            /* [in] */ BSTR bstrCodecsLocalDir,
            /* [in] */ BOOL bIsSubscriber,
            /* [in] */ IUOLFoneCodecsUpdaterCallback *pCodecsUpdaterCallback,
            /* [out] */ IUOLFoneCodecsUpdater2 **ppCodecsUpdater) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneServerConfigProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneServerConfigProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneServerConfigProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneServerConfigProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneServerConfigProvider * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneServerConfigProvider * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneServerConfigProvider * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneServerConfigProvider * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IUOLFoneServerConfigProvider * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Finalize )( 
            IUOLFoneServerConfigProvider * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DownloadPhoneValidationRules )( 
            IUOLFoneServerConfigProvider * This,
            /* [in] */ BSTR bstrUrl,
            /* [in] */ IUOLFoneDownloadCallback *pDownloadCallback,
            /* [out] */ IUOLFoneDownload **ppDownload);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CancelDownload )( 
            IUOLFoneServerConfigProvider * This,
            /* [in] */ IUOLFoneDownload *pDownload);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPhoneValidator )( 
            IUOLFoneServerConfigProvider * This,
            /* [in] */ IUOLFoneDownload *pDownload,
            /* [out] */ IUOLFonePhoneValidator **ppPhoneValidator);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetMessageParser )( 
            IUOLFoneServerConfigProvider * This,
            /* [out] */ IUOLFoneMessageParser **ppMessageParser);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateCodecsConfigFileUpdater )( 
            IUOLFoneServerConfigProvider * This,
            /* [in] */ BSTR bstrCodecsXMLUrl,
            /* [in] */ IUOLFoneCodecsConfigFileUpdaterCallback *pCodecsConfigFileUpdaterCallback,
            /* [out] */ IUOLFoneCodecsConfigFileUpdater2 **ppCodecsConfigFileUpdater);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateCodecsUpdater )( 
            IUOLFoneServerConfigProvider * This,
            /* [in] */ BSTR bstrCodecsXMLPath,
            /* [in] */ BSTR bstrCodecsLocalDir,
            /* [in] */ BOOL bIsSubscriber,
            /* [in] */ IUOLFoneCodecsUpdaterCallback *pCodecsUpdaterCallback,
            /* [out] */ IUOLFoneCodecsUpdater2 **ppCodecsUpdater);
        
        END_INTERFACE
    } IUOLFoneServerConfigProviderVtbl;

    interface IUOLFoneServerConfigProvider
    {
        CONST_VTBL struct IUOLFoneServerConfigProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneServerConfigProvider_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneServerConfigProvider_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneServerConfigProvider_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneServerConfigProvider_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneServerConfigProvider_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneServerConfigProvider_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneServerConfigProvider_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneServerConfigProvider_Initialize(This)	\
    ( (This)->lpVtbl -> Initialize(This) ) 

#define IUOLFoneServerConfigProvider_Finalize(This)	\
    ( (This)->lpVtbl -> Finalize(This) ) 

#define IUOLFoneServerConfigProvider_DownloadPhoneValidationRules(This,bstrUrl,pDownloadCallback,ppDownload)	\
    ( (This)->lpVtbl -> DownloadPhoneValidationRules(This,bstrUrl,pDownloadCallback,ppDownload) ) 

#define IUOLFoneServerConfigProvider_CancelDownload(This,pDownload)	\
    ( (This)->lpVtbl -> CancelDownload(This,pDownload) ) 

#define IUOLFoneServerConfigProvider_GetPhoneValidator(This,pDownload,ppPhoneValidator)	\
    ( (This)->lpVtbl -> GetPhoneValidator(This,pDownload,ppPhoneValidator) ) 

#define IUOLFoneServerConfigProvider_GetMessageParser(This,ppMessageParser)	\
    ( (This)->lpVtbl -> GetMessageParser(This,ppMessageParser) ) 

#define IUOLFoneServerConfigProvider_CreateCodecsConfigFileUpdater(This,bstrCodecsXMLUrl,pCodecsConfigFileUpdaterCallback,ppCodecsConfigFileUpdater)	\
    ( (This)->lpVtbl -> CreateCodecsConfigFileUpdater(This,bstrCodecsXMLUrl,pCodecsConfigFileUpdaterCallback,ppCodecsConfigFileUpdater) ) 

#define IUOLFoneServerConfigProvider_CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater)	\
    ( (This)->lpVtbl -> CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneServerConfigProvider_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneServerConfigProvider2_INTERFACE_DEFINED__
#define __IUOLFoneServerConfigProvider2_INTERFACE_DEFINED__

/* interface IUOLFoneServerConfigProvider2 */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneServerConfigProvider2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FE0CF1E0-900A-4bbb-89F9-8C4501A0C37A")
    IUOLFoneServerConfigProvider2 : public IUOLFoneServerConfigProvider
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetSettings( 
            /* [out] */ IUOLFoneSettings **ppSettings) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RunLocalCodecsCleanUp( 
            /* [in] */ BSTR bstrCodecsLocalDir) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneServerConfigProvider2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneServerConfigProvider2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneServerConfigProvider2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IUOLFoneServerConfigProvider2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Finalize )( 
            IUOLFoneServerConfigProvider2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DownloadPhoneValidationRules )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [in] */ BSTR bstrUrl,
            /* [in] */ IUOLFoneDownloadCallback *pDownloadCallback,
            /* [out] */ IUOLFoneDownload **ppDownload);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CancelDownload )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [in] */ IUOLFoneDownload *pDownload);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPhoneValidator )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [in] */ IUOLFoneDownload *pDownload,
            /* [out] */ IUOLFonePhoneValidator **ppPhoneValidator);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetMessageParser )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [out] */ IUOLFoneMessageParser **ppMessageParser);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateCodecsConfigFileUpdater )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [in] */ BSTR bstrCodecsXMLUrl,
            /* [in] */ IUOLFoneCodecsConfigFileUpdaterCallback *pCodecsConfigFileUpdaterCallback,
            /* [out] */ IUOLFoneCodecsConfigFileUpdater2 **ppCodecsConfigFileUpdater);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateCodecsUpdater )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [in] */ BSTR bstrCodecsXMLPath,
            /* [in] */ BSTR bstrCodecsLocalDir,
            /* [in] */ BOOL bIsSubscriber,
            /* [in] */ IUOLFoneCodecsUpdaterCallback *pCodecsUpdaterCallback,
            /* [out] */ IUOLFoneCodecsUpdater2 **ppCodecsUpdater);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetSettings )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [out] */ IUOLFoneSettings **ppSettings);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RunLocalCodecsCleanUp )( 
            IUOLFoneServerConfigProvider2 * This,
            /* [in] */ BSTR bstrCodecsLocalDir);
        
        END_INTERFACE
    } IUOLFoneServerConfigProvider2Vtbl;

    interface IUOLFoneServerConfigProvider2
    {
        CONST_VTBL struct IUOLFoneServerConfigProvider2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneServerConfigProvider2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneServerConfigProvider2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneServerConfigProvider2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneServerConfigProvider2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneServerConfigProvider2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneServerConfigProvider2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneServerConfigProvider2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneServerConfigProvider2_Initialize(This)	\
    ( (This)->lpVtbl -> Initialize(This) ) 

#define IUOLFoneServerConfigProvider2_Finalize(This)	\
    ( (This)->lpVtbl -> Finalize(This) ) 

#define IUOLFoneServerConfigProvider2_DownloadPhoneValidationRules(This,bstrUrl,pDownloadCallback,ppDownload)	\
    ( (This)->lpVtbl -> DownloadPhoneValidationRules(This,bstrUrl,pDownloadCallback,ppDownload) ) 

#define IUOLFoneServerConfigProvider2_CancelDownload(This,pDownload)	\
    ( (This)->lpVtbl -> CancelDownload(This,pDownload) ) 

#define IUOLFoneServerConfigProvider2_GetPhoneValidator(This,pDownload,ppPhoneValidator)	\
    ( (This)->lpVtbl -> GetPhoneValidator(This,pDownload,ppPhoneValidator) ) 

#define IUOLFoneServerConfigProvider2_GetMessageParser(This,ppMessageParser)	\
    ( (This)->lpVtbl -> GetMessageParser(This,ppMessageParser) ) 

#define IUOLFoneServerConfigProvider2_CreateCodecsConfigFileUpdater(This,bstrCodecsXMLUrl,pCodecsConfigFileUpdaterCallback,ppCodecsConfigFileUpdater)	\
    ( (This)->lpVtbl -> CreateCodecsConfigFileUpdater(This,bstrCodecsXMLUrl,pCodecsConfigFileUpdaterCallback,ppCodecsConfigFileUpdater) ) 

#define IUOLFoneServerConfigProvider2_CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater)	\
    ( (This)->lpVtbl -> CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater) ) 


#define IUOLFoneServerConfigProvider2_GetSettings(This,ppSettings)	\
    ( (This)->lpVtbl -> GetSettings(This,ppSettings) ) 

#define IUOLFoneServerConfigProvider2_RunLocalCodecsCleanUp(This,bstrCodecsLocalDir)	\
    ( (This)->lpVtbl -> RunLocalCodecsCleanUp(This,bstrCodecsLocalDir) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneServerConfigProvider2_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneServerConfigProvider3_INTERFACE_DEFINED__
#define __IUOLFoneServerConfigProvider3_INTERFACE_DEFINED__

/* interface IUOLFoneServerConfigProvider3 */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneServerConfigProvider3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4D3E7786-C806-41dd-B20C-BA94F3A2B218")
    IUOLFoneServerConfigProvider3 : public IUOLFoneServerConfigProvider2
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CreateCodecsUpdater( 
            /* [in] */ BSTR bstrCodecsXMLPath,
            /* [in] */ BSTR bstrCodecsLocalDir,
            /* [in] */ BOOL bIsSubscriber,
            /* [in] */ IUOLFoneCodecsUpdaterCallback *pCodecsUpdaterCallback,
            /* [out] */ IUOLFoneCodecsUpdater3 **ppCodecsUpdater) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneServerConfigProvider3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneServerConfigProvider3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneServerConfigProvider3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IUOLFoneServerConfigProvider3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Finalize )( 
            IUOLFoneServerConfigProvider3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DownloadPhoneValidationRules )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [in] */ BSTR bstrUrl,
            /* [in] */ IUOLFoneDownloadCallback *pDownloadCallback,
            /* [out] */ IUOLFoneDownload **ppDownload);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CancelDownload )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [in] */ IUOLFoneDownload *pDownload);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPhoneValidator )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [in] */ IUOLFoneDownload *pDownload,
            /* [out] */ IUOLFonePhoneValidator **ppPhoneValidator);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetMessageParser )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [out] */ IUOLFoneMessageParser **ppMessageParser);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateCodecsConfigFileUpdater )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [in] */ BSTR bstrCodecsXMLUrl,
            /* [in] */ IUOLFoneCodecsConfigFileUpdaterCallback *pCodecsConfigFileUpdaterCallback,
            /* [out] */ IUOLFoneCodecsConfigFileUpdater2 **ppCodecsConfigFileUpdater);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateCodecsUpdater )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [in] */ BSTR bstrCodecsXMLPath,
            /* [in] */ BSTR bstrCodecsLocalDir,
            /* [in] */ BOOL bIsSubscriber,
            /* [in] */ IUOLFoneCodecsUpdaterCallback *pCodecsUpdaterCallback,
            /* [out] */ IUOLFoneCodecsUpdater2 **ppCodecsUpdater);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetSettings )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [out] */ IUOLFoneSettings **ppSettings);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RunLocalCodecsCleanUp )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [in] */ BSTR bstrCodecsLocalDir);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateCodecsUpdater )( 
            IUOLFoneServerConfigProvider3 * This,
            /* [in] */ BSTR bstrCodecsXMLPath,
            /* [in] */ BSTR bstrCodecsLocalDir,
            /* [in] */ BOOL bIsSubscriber,
            /* [in] */ IUOLFoneCodecsUpdaterCallback *pCodecsUpdaterCallback,
            /* [out] */ IUOLFoneCodecsUpdater3 **ppCodecsUpdater);
        
        END_INTERFACE
    } IUOLFoneServerConfigProvider3Vtbl;

    interface IUOLFoneServerConfigProvider3
    {
        CONST_VTBL struct IUOLFoneServerConfigProvider3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneServerConfigProvider3_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneServerConfigProvider3_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneServerConfigProvider3_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneServerConfigProvider3_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneServerConfigProvider3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneServerConfigProvider3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneServerConfigProvider3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneServerConfigProvider3_Initialize(This)	\
    ( (This)->lpVtbl -> Initialize(This) ) 

#define IUOLFoneServerConfigProvider3_Finalize(This)	\
    ( (This)->lpVtbl -> Finalize(This) ) 

#define IUOLFoneServerConfigProvider3_DownloadPhoneValidationRules(This,bstrUrl,pDownloadCallback,ppDownload)	\
    ( (This)->lpVtbl -> DownloadPhoneValidationRules(This,bstrUrl,pDownloadCallback,ppDownload) ) 

#define IUOLFoneServerConfigProvider3_CancelDownload(This,pDownload)	\
    ( (This)->lpVtbl -> CancelDownload(This,pDownload) ) 

#define IUOLFoneServerConfigProvider3_GetPhoneValidator(This,pDownload,ppPhoneValidator)	\
    ( (This)->lpVtbl -> GetPhoneValidator(This,pDownload,ppPhoneValidator) ) 

#define IUOLFoneServerConfigProvider3_GetMessageParser(This,ppMessageParser)	\
    ( (This)->lpVtbl -> GetMessageParser(This,ppMessageParser) ) 

#define IUOLFoneServerConfigProvider3_CreateCodecsConfigFileUpdater(This,bstrCodecsXMLUrl,pCodecsConfigFileUpdaterCallback,ppCodecsConfigFileUpdater)	\
    ( (This)->lpVtbl -> CreateCodecsConfigFileUpdater(This,bstrCodecsXMLUrl,pCodecsConfigFileUpdaterCallback,ppCodecsConfigFileUpdater) ) 

#define IUOLFoneServerConfigProvider3_CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater)	\
    ( (This)->lpVtbl -> CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater) ) 


#define IUOLFoneServerConfigProvider3_GetSettings(This,ppSettings)	\
    ( (This)->lpVtbl -> GetSettings(This,ppSettings) ) 

#define IUOLFoneServerConfigProvider3_RunLocalCodecsCleanUp(This,bstrCodecsLocalDir)	\
    ( (This)->lpVtbl -> RunLocalCodecsCleanUp(This,bstrCodecsLocalDir) ) 


#define IUOLFoneServerConfigProvider3_CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater)	\
    ( (This)->lpVtbl -> CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneServerConfigProvider3_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneServerConfigProvider4_INTERFACE_DEFINED__
#define __IUOLFoneServerConfigProvider4_INTERFACE_DEFINED__

/* interface IUOLFoneServerConfigProvider4 */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneServerConfigProvider4;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2268734F-C52E-4e49-8334-35B99E0F79C1")
    IUOLFoneServerConfigProvider4 : public IUOLFoneServerConfigProvider3
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CreateCodecsConfigFileUpdater( 
            /* [in] */ BSTR bstrCodecsXMLUrl,
            /* [in] */ IUOLFoneCodecsConfigFileUpdaterCallback *pCodecsConfigFileUpdaterCallback,
            /* [out] */ IUOLFoneCodecsConfigFileUpdater3 **ppCodecsConfigFileUpdater) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneServerConfigProvider4Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneServerConfigProvider4 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneServerConfigProvider4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IUOLFoneServerConfigProvider4 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Finalize )( 
            IUOLFoneServerConfigProvider4 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DownloadPhoneValidationRules )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ BSTR bstrUrl,
            /* [in] */ IUOLFoneDownloadCallback *pDownloadCallback,
            /* [out] */ IUOLFoneDownload **ppDownload);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CancelDownload )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ IUOLFoneDownload *pDownload);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPhoneValidator )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ IUOLFoneDownload *pDownload,
            /* [out] */ IUOLFonePhoneValidator **ppPhoneValidator);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetMessageParser )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [out] */ IUOLFoneMessageParser **ppMessageParser);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateCodecsConfigFileUpdater )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ BSTR bstrCodecsXMLUrl,
            /* [in] */ IUOLFoneCodecsConfigFileUpdaterCallback *pCodecsConfigFileUpdaterCallback,
            /* [out] */ IUOLFoneCodecsConfigFileUpdater2 **ppCodecsConfigFileUpdater);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateCodecsUpdater )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ BSTR bstrCodecsXMLPath,
            /* [in] */ BSTR bstrCodecsLocalDir,
            /* [in] */ BOOL bIsSubscriber,
            /* [in] */ IUOLFoneCodecsUpdaterCallback *pCodecsUpdaterCallback,
            /* [out] */ IUOLFoneCodecsUpdater2 **ppCodecsUpdater);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetSettings )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [out] */ IUOLFoneSettings **ppSettings);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RunLocalCodecsCleanUp )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ BSTR bstrCodecsLocalDir);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateCodecsUpdater )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ BSTR bstrCodecsXMLPath,
            /* [in] */ BSTR bstrCodecsLocalDir,
            /* [in] */ BOOL bIsSubscriber,
            /* [in] */ IUOLFoneCodecsUpdaterCallback *pCodecsUpdaterCallback,
            /* [out] */ IUOLFoneCodecsUpdater3 **ppCodecsUpdater);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateCodecsConfigFileUpdater )( 
            IUOLFoneServerConfigProvider4 * This,
            /* [in] */ BSTR bstrCodecsXMLUrl,
            /* [in] */ IUOLFoneCodecsConfigFileUpdaterCallback *pCodecsConfigFileUpdaterCallback,
            /* [out] */ IUOLFoneCodecsConfigFileUpdater3 **ppCodecsConfigFileUpdater);
        
        END_INTERFACE
    } IUOLFoneServerConfigProvider4Vtbl;

    interface IUOLFoneServerConfigProvider4
    {
        CONST_VTBL struct IUOLFoneServerConfigProvider4Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneServerConfigProvider4_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneServerConfigProvider4_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneServerConfigProvider4_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneServerConfigProvider4_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneServerConfigProvider4_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneServerConfigProvider4_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneServerConfigProvider4_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneServerConfigProvider4_Initialize(This)	\
    ( (This)->lpVtbl -> Initialize(This) ) 

#define IUOLFoneServerConfigProvider4_Finalize(This)	\
    ( (This)->lpVtbl -> Finalize(This) ) 

#define IUOLFoneServerConfigProvider4_DownloadPhoneValidationRules(This,bstrUrl,pDownloadCallback,ppDownload)	\
    ( (This)->lpVtbl -> DownloadPhoneValidationRules(This,bstrUrl,pDownloadCallback,ppDownload) ) 

#define IUOLFoneServerConfigProvider4_CancelDownload(This,pDownload)	\
    ( (This)->lpVtbl -> CancelDownload(This,pDownload) ) 

#define IUOLFoneServerConfigProvider4_GetPhoneValidator(This,pDownload,ppPhoneValidator)	\
    ( (This)->lpVtbl -> GetPhoneValidator(This,pDownload,ppPhoneValidator) ) 

#define IUOLFoneServerConfigProvider4_GetMessageParser(This,ppMessageParser)	\
    ( (This)->lpVtbl -> GetMessageParser(This,ppMessageParser) ) 

#define IUOLFoneServerConfigProvider4_CreateCodecsConfigFileUpdater(This,bstrCodecsXMLUrl,pCodecsConfigFileUpdaterCallback,ppCodecsConfigFileUpdater)	\
    ( (This)->lpVtbl -> CreateCodecsConfigFileUpdater(This,bstrCodecsXMLUrl,pCodecsConfigFileUpdaterCallback,ppCodecsConfigFileUpdater) ) 

#define IUOLFoneServerConfigProvider4_CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater)	\
    ( (This)->lpVtbl -> CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater) ) 


#define IUOLFoneServerConfigProvider4_GetSettings(This,ppSettings)	\
    ( (This)->lpVtbl -> GetSettings(This,ppSettings) ) 

#define IUOLFoneServerConfigProvider4_RunLocalCodecsCleanUp(This,bstrCodecsLocalDir)	\
    ( (This)->lpVtbl -> RunLocalCodecsCleanUp(This,bstrCodecsLocalDir) ) 


#define IUOLFoneServerConfigProvider4_CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater)	\
    ( (This)->lpVtbl -> CreateCodecsUpdater(This,bstrCodecsXMLPath,bstrCodecsLocalDir,bIsSubscriber,pCodecsUpdaterCallback,ppCodecsUpdater) ) 


#define IUOLFoneServerConfigProvider4_CreateCodecsConfigFileUpdater(This,bstrCodecsXMLUrl,pCodecsConfigFileUpdaterCallback,ppCodecsConfigFileUpdater)	\
    ( (This)->lpVtbl -> CreateCodecsConfigFileUpdater(This,bstrCodecsXMLUrl,pCodecsConfigFileUpdaterCallback,ppCodecsConfigFileUpdater) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneServerConfigProvider4_INTERFACE_DEFINED__ */



#ifndef __UOLFoneServerConfigProvider_LIBRARY_DEFINED__
#define __UOLFoneServerConfigProvider_LIBRARY_DEFINED__

/* library UOLFoneServerConfigProvider */
/* [helpstring][uuid][version] */ 


EXTERN_C const IID LIBID_UOLFoneServerConfigProvider;

EXTERN_C const CLSID CLSID_CUOLFoneServerConfigProvider;

#ifdef __cplusplus

class DECLSPEC_UUID("2ACBB173-9624-418F-A7A9-D9F0FCD2CD37")
CUOLFoneServerConfigProvider;
#endif
#endif /* __UOLFoneServerConfigProvider_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


