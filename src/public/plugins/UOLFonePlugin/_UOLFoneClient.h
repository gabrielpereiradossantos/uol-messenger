

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0499 */
/* at Tue Mar 03 08:58:04 2009
 */
/* Compiler settings for _UOLFoneClient.idl:
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

#ifndef ___UOLFoneClient_h__
#define ___UOLFoneClient_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IUOLFoneClientCodecInfo_FWD_DEFINED__
#define __IUOLFoneClientCodecInfo_FWD_DEFINED__
typedef interface IUOLFoneClientCodecInfo IUOLFoneClientCodecInfo;
#endif 	/* __IUOLFoneClientCodecInfo_FWD_DEFINED__ */


#ifndef __IUOLFoneClientCodecInfoEnum_FWD_DEFINED__
#define __IUOLFoneClientCodecInfoEnum_FWD_DEFINED__
typedef interface IUOLFoneClientCodecInfoEnum IUOLFoneClientCodecInfoEnum;
#endif 	/* __IUOLFoneClientCodecInfoEnum_FWD_DEFINED__ */


#ifndef __IUOLFoneClientConferenceCall_FWD_DEFINED__
#define __IUOLFoneClientConferenceCall_FWD_DEFINED__
typedef interface IUOLFoneClientConferenceCall IUOLFoneClientConferenceCall;
#endif 	/* __IUOLFoneClientConferenceCall_FWD_DEFINED__ */


#ifndef __IUOLFoneClientConnectionTestResult_FWD_DEFINED__
#define __IUOLFoneClientConnectionTestResult_FWD_DEFINED__
typedef interface IUOLFoneClientConnectionTestResult IUOLFoneClientConnectionTestResult;
#endif 	/* __IUOLFoneClientConnectionTestResult_FWD_DEFINED__ */


#ifndef __IUOLFoneClientDeviceInfo_FWD_DEFINED__
#define __IUOLFoneClientDeviceInfo_FWD_DEFINED__
typedef interface IUOLFoneClientDeviceInfo IUOLFoneClientDeviceInfo;
#endif 	/* __IUOLFoneClientDeviceInfo_FWD_DEFINED__ */


#ifndef __IUOLFoneClientDeviceInfoEnum_FWD_DEFINED__
#define __IUOLFoneClientDeviceInfoEnum_FWD_DEFINED__
typedef interface IUOLFoneClientDeviceInfoEnum IUOLFoneClientDeviceInfoEnum;
#endif 	/* __IUOLFoneClientDeviceInfoEnum_FWD_DEFINED__ */


#ifndef __IUOLFoneClientHeaderInfo_FWD_DEFINED__
#define __IUOLFoneClientHeaderInfo_FWD_DEFINED__
typedef interface IUOLFoneClientHeaderInfo IUOLFoneClientHeaderInfo;
#endif 	/* __IUOLFoneClientHeaderInfo_FWD_DEFINED__ */


#ifndef __IUOLFoneClientHeaderInfoEnum_FWD_DEFINED__
#define __IUOLFoneClientHeaderInfoEnum_FWD_DEFINED__
typedef interface IUOLFoneClientHeaderInfoEnum IUOLFoneClientHeaderInfoEnum;
#endif 	/* __IUOLFoneClientHeaderInfoEnum_FWD_DEFINED__ */


#ifndef __IUOLFoneClientExtraResponseData_FWD_DEFINED__
#define __IUOLFoneClientExtraResponseData_FWD_DEFINED__
typedef interface IUOLFoneClientExtraResponseData IUOLFoneClientExtraResponseData;
#endif 	/* __IUOLFoneClientExtraResponseData_FWD_DEFINED__ */


#ifndef __IUOLFoneClientExtraResponseData2_FWD_DEFINED__
#define __IUOLFoneClientExtraResponseData2_FWD_DEFINED__
typedef interface IUOLFoneClientExtraResponseData2 IUOLFoneClientExtraResponseData2;
#endif 	/* __IUOLFoneClientExtraResponseData2_FWD_DEFINED__ */


#ifndef __IUOLFoneClientUserInfo_FWD_DEFINED__
#define __IUOLFoneClientUserInfo_FWD_DEFINED__
typedef interface IUOLFoneClientUserInfo IUOLFoneClientUserInfo;
#endif 	/* __IUOLFoneClientUserInfo_FWD_DEFINED__ */


#ifndef __IUOLFoneClientServerInfo_FWD_DEFINED__
#define __IUOLFoneClientServerInfo_FWD_DEFINED__
typedef interface IUOLFoneClientServerInfo IUOLFoneClientServerInfo;
#endif 	/* __IUOLFoneClientServerInfo_FWD_DEFINED__ */


#ifndef __IUOLFoneClientPhoneLineInfo_FWD_DEFINED__
#define __IUOLFoneClientPhoneLineInfo_FWD_DEFINED__
typedef interface IUOLFoneClientPhoneLineInfo IUOLFoneClientPhoneLineInfo;
#endif 	/* __IUOLFoneClientPhoneLineInfo_FWD_DEFINED__ */


#ifndef __IUOLFoneClientPhoneLine_FWD_DEFINED__
#define __IUOLFoneClientPhoneLine_FWD_DEFINED__
typedef interface IUOLFoneClientPhoneLine IUOLFoneClientPhoneLine;
#endif 	/* __IUOLFoneClientPhoneLine_FWD_DEFINED__ */


#ifndef __IUOLFoneClientPhoneCall_FWD_DEFINED__
#define __IUOLFoneClientPhoneCall_FWD_DEFINED__
typedef interface IUOLFoneClientPhoneCall IUOLFoneClientPhoneCall;
#endif 	/* __IUOLFoneClientPhoneCall_FWD_DEFINED__ */


#ifndef __IUOLFoneClientMessageEvent_FWD_DEFINED__
#define __IUOLFoneClientMessageEvent_FWD_DEFINED__
typedef interface IUOLFoneClientMessageEvent IUOLFoneClientMessageEvent;
#endif 	/* __IUOLFoneClientMessageEvent_FWD_DEFINED__ */


#ifndef __IUOLFoneClientConnectionTestParam_FWD_DEFINED__
#define __IUOLFoneClientConnectionTestParam_FWD_DEFINED__
typedef interface IUOLFoneClientConnectionTestParam IUOLFoneClientConnectionTestParam;
#endif 	/* __IUOLFoneClientConnectionTestParam_FWD_DEFINED__ */


#ifndef __IUOLFoneClientOsipDebugInfo_FWD_DEFINED__
#define __IUOLFoneClientOsipDebugInfo_FWD_DEFINED__
typedef interface IUOLFoneClientOsipDebugInfo IUOLFoneClientOsipDebugInfo;
#endif 	/* __IUOLFoneClientOsipDebugInfo_FWD_DEFINED__ */


#ifndef __IUOLFoneClientNetworkConfig_FWD_DEFINED__
#define __IUOLFoneClientNetworkConfig_FWD_DEFINED__
typedef interface IUOLFoneClientNetworkConfig IUOLFoneClientNetworkConfig;
#endif 	/* __IUOLFoneClientNetworkConfig_FWD_DEFINED__ */


#ifndef __IUOLFoneClientPhoneLineConfig_FWD_DEFINED__
#define __IUOLFoneClientPhoneLineConfig_FWD_DEFINED__
typedef interface IUOLFoneClientPhoneLineConfig IUOLFoneClientPhoneLineConfig;
#endif 	/* __IUOLFoneClientPhoneLineConfig_FWD_DEFINED__ */


#ifndef __IUOLFoneClientCallback_FWD_DEFINED__
#define __IUOLFoneClientCallback_FWD_DEFINED__
typedef interface IUOLFoneClientCallback IUOLFoneClientCallback;
#endif 	/* __IUOLFoneClientCallback_FWD_DEFINED__ */


#ifndef __IUOLFoneClientCallback2_FWD_DEFINED__
#define __IUOLFoneClientCallback2_FWD_DEFINED__
typedef interface IUOLFoneClientCallback2 IUOLFoneClientCallback2;
#endif 	/* __IUOLFoneClientCallback2_FWD_DEFINED__ */


#ifndef __IUOLFoneClient_FWD_DEFINED__
#define __IUOLFoneClient_FWD_DEFINED__
typedef interface IUOLFoneClient IUOLFoneClient;
#endif 	/* __IUOLFoneClient_FWD_DEFINED__ */


#ifndef __IUOLFoneClient2_FWD_DEFINED__
#define __IUOLFoneClient2_FWD_DEFINED__
typedef interface IUOLFoneClient2 IUOLFoneClient2;
#endif 	/* __IUOLFoneClient2_FWD_DEFINED__ */


#ifndef __CUOLFoneClient_FWD_DEFINED__
#define __CUOLFoneClient_FWD_DEFINED__

#ifdef __cplusplus
typedef class CUOLFoneClient CUOLFoneClient;
#else
typedef struct CUOLFoneClient CUOLFoneClient;
#endif /* __cplusplus */

#endif 	/* __CUOLFoneClient_FWD_DEFINED__ */


/* header files for imported files */
#include "prsht.h"
#include "mshtml.h"
#include "mshtmhst.h"
#include "exdisp.h"
#include "objsafe.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IUOLFoneClientCodecInfo_INTERFACE_DEFINED__
#define __IUOLFoneClientCodecInfo_INTERFACE_DEFINED__

/* interface IUOLFoneClientCodecInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientCodecInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("95FAE098-B846-47da-8C54-900C9AC7C210")
    IUOLFoneClientCodecInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecId( 
            /* [out] */ BSTR *pbstrId) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetCodecName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientCodecInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientCodecInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientCodecInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientCodecInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientCodecInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientCodecInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientCodecInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientCodecInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecId )( 
            IUOLFoneClientCodecInfo * This,
            /* [out] */ BSTR *pbstrId);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetCodecName )( 
            IUOLFoneClientCodecInfo * This,
            /* [out] */ BSTR *pbstrName);
        
        END_INTERFACE
    } IUOLFoneClientCodecInfoVtbl;

    interface IUOLFoneClientCodecInfo
    {
        CONST_VTBL struct IUOLFoneClientCodecInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientCodecInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientCodecInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientCodecInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientCodecInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientCodecInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientCodecInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientCodecInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientCodecInfo_GetCodecId(This,pbstrId)	\
    ( (This)->lpVtbl -> GetCodecId(This,pbstrId) ) 

#define IUOLFoneClientCodecInfo_GetCodecName(This,pbstrName)	\
    ( (This)->lpVtbl -> GetCodecName(This,pbstrName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientCodecInfo_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClientCodecInfoEnum_INTERFACE_DEFINED__
#define __IUOLFoneClientCodecInfoEnum_INTERFACE_DEFINED__

/* interface IUOLFoneClientCodecInfoEnum */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientCodecInfoEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F0283509-2101-4192-A20D-11B8DB0F6937")
    IUOLFoneClientCodecInfoEnum : public IUnknown
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [in] */ IUOLFoneClientCodecInfo **ppElements,
            /* [in] */ ULONG *pceltFetched) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [in] */ IUOLFoneClientCodecInfoEnum **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientCodecInfoEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientCodecInfoEnum * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientCodecInfoEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientCodecInfoEnum * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IUOLFoneClientCodecInfoEnum * This,
            /* [in] */ ULONG celt,
            /* [in] */ IUOLFoneClientCodecInfo **ppElements,
            /* [in] */ ULONG *pceltFetched);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IUOLFoneClientCodecInfoEnum * This,
            /* [in] */ ULONG celt);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IUOLFoneClientCodecInfoEnum * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IUOLFoneClientCodecInfoEnum * This,
            /* [in] */ IUOLFoneClientCodecInfoEnum **ppEnum);
        
        END_INTERFACE
    } IUOLFoneClientCodecInfoEnumVtbl;

    interface IUOLFoneClientCodecInfoEnum
    {
        CONST_VTBL struct IUOLFoneClientCodecInfoEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientCodecInfoEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientCodecInfoEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientCodecInfoEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientCodecInfoEnum_Next(This,celt,ppElements,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,ppElements,pceltFetched) ) 

#define IUOLFoneClientCodecInfoEnum_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IUOLFoneClientCodecInfoEnum_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IUOLFoneClientCodecInfoEnum_Clone(This,ppEnum)	\
    ( (This)->lpVtbl -> Clone(This,ppEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientCodecInfoEnum_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneClient_0000_0002 */
/* [local] */ 


enum EnumConfFailureCode
    {	UFC_CONF_FAILURE_CODE_NOT_CREATED	= 700,
	UFC_CONF_FAILURE_CODE_ALREADY_STARTED	= 701,
	UFC_CONF_FAILURE_CODE_TOO_MANY_PARTICIPANTS	= 702
    } ;

enum EnumConfConstants
    {	UFC_CONF_CONST_INVALID_PARTICIPANT_INDEX	= -1,
	UFC_CONF_CONST_INVALID_PHONE_CALL_ID	= -1,
	UFC_CONF_CONST_DEFAULT_CONFERENCE_ID_NUMBER	= 1,
	UFC_CONF_CONST_MAX_NUMBER_OF_PARTICIPANTS	= 2
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0002_v0_0_s_ifspec;

#ifndef __IUOLFoneClientConferenceCall_INTERFACE_DEFINED__
#define __IUOLFoneClientConferenceCall_INTERFACE_DEFINED__

/* interface IUOLFoneClientConferenceCall */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientConferenceCall;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("990862D7-2642-4e2c-8BBB-9CB9EE645BEC")
    IUOLFoneClientConferenceCall : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetConferenceId( 
            /* [out] */ LONG *plConferenceCallId) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetNumberOfParticipants( 
            /* [out] */ LONG *plNumberOfParticipants) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetParticipantCallId( 
            /* [in] */ LONG lParticipantIndex,
            /* [out] */ LONG *lpParticipantCallId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientConferenceCallVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientConferenceCall * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientConferenceCall * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientConferenceCall * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientConferenceCall * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientConferenceCall * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientConferenceCall * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientConferenceCall * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetConferenceId )( 
            IUOLFoneClientConferenceCall * This,
            /* [out] */ LONG *plConferenceCallId);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetNumberOfParticipants )( 
            IUOLFoneClientConferenceCall * This,
            /* [out] */ LONG *plNumberOfParticipants);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetParticipantCallId )( 
            IUOLFoneClientConferenceCall * This,
            /* [in] */ LONG lParticipantIndex,
            /* [out] */ LONG *lpParticipantCallId);
        
        END_INTERFACE
    } IUOLFoneClientConferenceCallVtbl;

    interface IUOLFoneClientConferenceCall
    {
        CONST_VTBL struct IUOLFoneClientConferenceCallVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientConferenceCall_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientConferenceCall_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientConferenceCall_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientConferenceCall_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientConferenceCall_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientConferenceCall_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientConferenceCall_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientConferenceCall_GetConferenceId(This,plConferenceCallId)	\
    ( (This)->lpVtbl -> GetConferenceId(This,plConferenceCallId) ) 

#define IUOLFoneClientConferenceCall_GetNumberOfParticipants(This,plNumberOfParticipants)	\
    ( (This)->lpVtbl -> GetNumberOfParticipants(This,plNumberOfParticipants) ) 

#define IUOLFoneClientConferenceCall_GetParticipantCallId(This,lParticipantIndex,lpParticipantCallId)	\
    ( (This)->lpVtbl -> GetParticipantCallId(This,lParticipantIndex,lpParticipantCallId) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientConferenceCall_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneClient_0000_0003 */
/* [local] */ 


enum EnumConnectionTestResult
    {	UFC_CONNECTION_TEST_RESULT_NO_CONNECTION	= 0,
	UFC_CONNECTION_TEST_RESULT_ALLOW_UDP	= 1,
	UFC_CONNECTION_TEST_RESULT_ALLOW_TCP	= 2,
	UFC_CONNECTION_TEST_RESULT_ALLOW_HTTP_TUNNEL	= 4
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0003_v0_0_s_ifspec;

#ifndef __IUOLFoneClientConnectionTestResult_INTERFACE_DEFINED__
#define __IUOLFoneClientConnectionTestResult_INTERFACE_DEFINED__

/* interface IUOLFoneClientConnectionTestResult */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientConnectionTestResult;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8EBD8140-0F4E-4ae3-ADFB-EFBA8698C37E")
    IUOLFoneClientConnectionTestResult : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetResult( 
            /* [out] */ LONG *plResult) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetAvailablePort( 
            /* [out] */ LONG *plPort) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientConnectionTestResultVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientConnectionTestResult * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientConnectionTestResult * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientConnectionTestResult * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientConnectionTestResult * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientConnectionTestResult * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientConnectionTestResult * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientConnectionTestResult * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetResult )( 
            IUOLFoneClientConnectionTestResult * This,
            /* [out] */ LONG *plResult);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetAvailablePort )( 
            IUOLFoneClientConnectionTestResult * This,
            /* [out] */ LONG *plPort);
        
        END_INTERFACE
    } IUOLFoneClientConnectionTestResultVtbl;

    interface IUOLFoneClientConnectionTestResult
    {
        CONST_VTBL struct IUOLFoneClientConnectionTestResultVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientConnectionTestResult_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientConnectionTestResult_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientConnectionTestResult_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientConnectionTestResult_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientConnectionTestResult_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientConnectionTestResult_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientConnectionTestResult_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientConnectionTestResult_GetResult(This,plResult)	\
    ( (This)->lpVtbl -> GetResult(This,plResult) ) 

#define IUOLFoneClientConnectionTestResult_GetAvailablePort(This,plPort)	\
    ( (This)->lpVtbl -> GetAvailablePort(This,plPort) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientConnectionTestResult_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClientDeviceInfo_INTERFACE_DEFINED__
#define __IUOLFoneClientDeviceInfo_INTERFACE_DEFINED__

/* interface IUOLFoneClientDeviceInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientDeviceInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6EF451EB-E7A4-4edd-B665-71BB4E366305")
    IUOLFoneClientDeviceInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetDeviceName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientDeviceInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientDeviceInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientDeviceInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientDeviceInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientDeviceInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientDeviceInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientDeviceInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientDeviceInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetDeviceName )( 
            IUOLFoneClientDeviceInfo * This,
            /* [out] */ BSTR *pbstrName);
        
        END_INTERFACE
    } IUOLFoneClientDeviceInfoVtbl;

    interface IUOLFoneClientDeviceInfo
    {
        CONST_VTBL struct IUOLFoneClientDeviceInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientDeviceInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientDeviceInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientDeviceInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientDeviceInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientDeviceInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientDeviceInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientDeviceInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientDeviceInfo_GetDeviceName(This,pbstrName)	\
    ( (This)->lpVtbl -> GetDeviceName(This,pbstrName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientDeviceInfo_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClientDeviceInfoEnum_INTERFACE_DEFINED__
#define __IUOLFoneClientDeviceInfoEnum_INTERFACE_DEFINED__

/* interface IUOLFoneClientDeviceInfoEnum */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientDeviceInfoEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2321FF5F-9D68-4e89-A669-23F7467BDEB1")
    IUOLFoneClientDeviceInfoEnum : public IUnknown
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [in] */ IUOLFoneClientDeviceInfo **ppElements,
            /* [in] */ ULONG *pceltFetched) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [in] */ IUOLFoneClientDeviceInfoEnum **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientDeviceInfoEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientDeviceInfoEnum * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientDeviceInfoEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientDeviceInfoEnum * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IUOLFoneClientDeviceInfoEnum * This,
            /* [in] */ ULONG celt,
            /* [in] */ IUOLFoneClientDeviceInfo **ppElements,
            /* [in] */ ULONG *pceltFetched);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IUOLFoneClientDeviceInfoEnum * This,
            /* [in] */ ULONG celt);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IUOLFoneClientDeviceInfoEnum * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IUOLFoneClientDeviceInfoEnum * This,
            /* [in] */ IUOLFoneClientDeviceInfoEnum **ppEnum);
        
        END_INTERFACE
    } IUOLFoneClientDeviceInfoEnumVtbl;

    interface IUOLFoneClientDeviceInfoEnum
    {
        CONST_VTBL struct IUOLFoneClientDeviceInfoEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientDeviceInfoEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientDeviceInfoEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientDeviceInfoEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientDeviceInfoEnum_Next(This,celt,ppElements,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,ppElements,pceltFetched) ) 

#define IUOLFoneClientDeviceInfoEnum_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IUOLFoneClientDeviceInfoEnum_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IUOLFoneClientDeviceInfoEnum_Clone(This,ppEnum)	\
    ( (This)->lpVtbl -> Clone(This,ppEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientDeviceInfoEnum_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClientHeaderInfo_INTERFACE_DEFINED__
#define __IUOLFoneClientHeaderInfo_INTERFACE_DEFINED__

/* interface IUOLFoneClientHeaderInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientHeaderInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7CA48D53-687F-47ff-ACD1-C35E026BC136")
    IUOLFoneClientHeaderInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetName( 
            /* [out] */ BSTR *pbstrName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetValue( 
            /* [out] */ BSTR *pbstrValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientHeaderInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientHeaderInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientHeaderInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientHeaderInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientHeaderInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientHeaderInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientHeaderInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientHeaderInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetName )( 
            IUOLFoneClientHeaderInfo * This,
            /* [out] */ BSTR *pbstrName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IUOLFoneClientHeaderInfo * This,
            /* [out] */ BSTR *pbstrValue);
        
        END_INTERFACE
    } IUOLFoneClientHeaderInfoVtbl;

    interface IUOLFoneClientHeaderInfo
    {
        CONST_VTBL struct IUOLFoneClientHeaderInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientHeaderInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientHeaderInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientHeaderInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientHeaderInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientHeaderInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientHeaderInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientHeaderInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientHeaderInfo_GetName(This,pbstrName)	\
    ( (This)->lpVtbl -> GetName(This,pbstrName) ) 

#define IUOLFoneClientHeaderInfo_GetValue(This,pbstrValue)	\
    ( (This)->lpVtbl -> GetValue(This,pbstrValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientHeaderInfo_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClientHeaderInfoEnum_INTERFACE_DEFINED__
#define __IUOLFoneClientHeaderInfoEnum_INTERFACE_DEFINED__

/* interface IUOLFoneClientHeaderInfoEnum */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientHeaderInfoEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("60CB945D-E483-4d72-8FF4-F1F240325409")
    IUOLFoneClientHeaderInfoEnum : public IUnknown
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [in] */ IUOLFoneClientHeaderInfo **ppElements,
            /* [in] */ ULONG *pceltFetched) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [in] */ IUOLFoneClientHeaderInfoEnum **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientHeaderInfoEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientHeaderInfoEnum * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientHeaderInfoEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientHeaderInfoEnum * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IUOLFoneClientHeaderInfoEnum * This,
            /* [in] */ ULONG celt,
            /* [in] */ IUOLFoneClientHeaderInfo **ppElements,
            /* [in] */ ULONG *pceltFetched);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IUOLFoneClientHeaderInfoEnum * This,
            /* [in] */ ULONG celt);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IUOLFoneClientHeaderInfoEnum * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IUOLFoneClientHeaderInfoEnum * This,
            /* [in] */ IUOLFoneClientHeaderInfoEnum **ppEnum);
        
        END_INTERFACE
    } IUOLFoneClientHeaderInfoEnumVtbl;

    interface IUOLFoneClientHeaderInfoEnum
    {
        CONST_VTBL struct IUOLFoneClientHeaderInfoEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientHeaderInfoEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientHeaderInfoEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientHeaderInfoEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientHeaderInfoEnum_Next(This,celt,ppElements,pceltFetched)	\
    ( (This)->lpVtbl -> Next(This,celt,ppElements,pceltFetched) ) 

#define IUOLFoneClientHeaderInfoEnum_Skip(This,celt)	\
    ( (This)->lpVtbl -> Skip(This,celt) ) 

#define IUOLFoneClientHeaderInfoEnum_Reset(This)	\
    ( (This)->lpVtbl -> Reset(This) ) 

#define IUOLFoneClientHeaderInfoEnum_Clone(This,ppEnum)	\
    ( (This)->lpVtbl -> Clone(This,ppEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientHeaderInfoEnum_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneClient_0000_0008 */
/* [local] */ 


enum ExtraResponseDataId
    {	CALL_ERROR_REASON_PHRASE	= 0,
	CALL_ERROR_STATUS_CODE	= 1,
	CALL_ERROR_ERROR_SOURCE	= 2,
	CALL_ERROR_TEXT_INFO	= 3,
	CALL_ERROR_INVALID_RESPONSE_ID	= 4
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0008_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0008_v0_0_s_ifspec;

#ifndef __IUOLFoneClientExtraResponseData_INTERFACE_DEFINED__
#define __IUOLFoneClientExtraResponseData_INTERFACE_DEFINED__

/* interface IUOLFoneClientExtraResponseData */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientExtraResponseData;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("09A0A5BE-0DDB-492c-A008-165D2C834050")
    IUOLFoneClientExtraResponseData : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EnumerateExtraHeaders( 
            /* [out] */ IUOLFoneClientHeaderInfoEnum **ppHeaderInfoEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientExtraResponseDataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientExtraResponseData * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientExtraResponseData * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientExtraResponseData * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientExtraResponseData * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientExtraResponseData * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientExtraResponseData * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientExtraResponseData * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnumerateExtraHeaders )( 
            IUOLFoneClientExtraResponseData * This,
            /* [out] */ IUOLFoneClientHeaderInfoEnum **ppHeaderInfoEnum);
        
        END_INTERFACE
    } IUOLFoneClientExtraResponseDataVtbl;

    interface IUOLFoneClientExtraResponseData
    {
        CONST_VTBL struct IUOLFoneClientExtraResponseDataVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientExtraResponseData_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientExtraResponseData_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientExtraResponseData_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientExtraResponseData_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientExtraResponseData_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientExtraResponseData_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientExtraResponseData_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientExtraResponseData_EnumerateExtraHeaders(This,ppHeaderInfoEnum)	\
    ( (This)->lpVtbl -> EnumerateExtraHeaders(This,ppHeaderInfoEnum) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientExtraResponseData_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClientExtraResponseData2_INTERFACE_DEFINED__
#define __IUOLFoneClientExtraResponseData2_INTERFACE_DEFINED__

/* interface IUOLFoneClientExtraResponseData2 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientExtraResponseData2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("894D5A4F-E013-4e58-99C5-813C630A480E")
    IUOLFoneClientExtraResponseData2 : public IUOLFoneClientExtraResponseData
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetExtraResponseDataId( 
            /* [in] */ BSTR bstrExtraResponseDataName,
            /* [out] */ LONG *plExtraResponseDataId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientExtraResponseData2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientExtraResponseData2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientExtraResponseData2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientExtraResponseData2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientExtraResponseData2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientExtraResponseData2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientExtraResponseData2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientExtraResponseData2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnumerateExtraHeaders )( 
            IUOLFoneClientExtraResponseData2 * This,
            /* [out] */ IUOLFoneClientHeaderInfoEnum **ppHeaderInfoEnum);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetExtraResponseDataId )( 
            IUOLFoneClientExtraResponseData2 * This,
            /* [in] */ BSTR bstrExtraResponseDataName,
            /* [out] */ LONG *plExtraResponseDataId);
        
        END_INTERFACE
    } IUOLFoneClientExtraResponseData2Vtbl;

    interface IUOLFoneClientExtraResponseData2
    {
        CONST_VTBL struct IUOLFoneClientExtraResponseData2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientExtraResponseData2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientExtraResponseData2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientExtraResponseData2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientExtraResponseData2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientExtraResponseData2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientExtraResponseData2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientExtraResponseData2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientExtraResponseData2_EnumerateExtraHeaders(This,ppHeaderInfoEnum)	\
    ( (This)->lpVtbl -> EnumerateExtraHeaders(This,ppHeaderInfoEnum) ) 


#define IUOLFoneClientExtraResponseData2_GetExtraResponseDataId(This,bstrExtraResponseDataName,plExtraResponseDataId)	\
    ( (This)->lpVtbl -> GetExtraResponseDataId(This,bstrExtraResponseDataName,plExtraResponseDataId) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientExtraResponseData2_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClientUserInfo_INTERFACE_DEFINED__
#define __IUOLFoneClientUserInfo_INTERFACE_DEFINED__

/* interface IUOLFoneClientUserInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientUserInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFBB04F0-6216-4901-B21F-938DE2899D2D")
    IUOLFoneClientUserInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetDisplayName( 
            /* [out] */ BSTR *pbstrDisplayName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetUsername( 
            /* [out] */ BSTR *pbstrUsername) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetIdentity( 
            /* [out] */ BSTR *pbstrIdentity) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPassword( 
            /* [out] */ BSTR *pbstrPassword) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetRealm( 
            /* [out] */ BSTR *pbstrRealm) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientUserInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientUserInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientUserInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientUserInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientUserInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientUserInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientUserInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientUserInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetDisplayName )( 
            IUOLFoneClientUserInfo * This,
            /* [out] */ BSTR *pbstrDisplayName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUsername )( 
            IUOLFoneClientUserInfo * This,
            /* [out] */ BSTR *pbstrUsername);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetIdentity )( 
            IUOLFoneClientUserInfo * This,
            /* [out] */ BSTR *pbstrIdentity);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPassword )( 
            IUOLFoneClientUserInfo * This,
            /* [out] */ BSTR *pbstrPassword);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetRealm )( 
            IUOLFoneClientUserInfo * This,
            /* [out] */ BSTR *pbstrRealm);
        
        END_INTERFACE
    } IUOLFoneClientUserInfoVtbl;

    interface IUOLFoneClientUserInfo
    {
        CONST_VTBL struct IUOLFoneClientUserInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientUserInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientUserInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientUserInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientUserInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientUserInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientUserInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientUserInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientUserInfo_GetDisplayName(This,pbstrDisplayName)	\
    ( (This)->lpVtbl -> GetDisplayName(This,pbstrDisplayName) ) 

#define IUOLFoneClientUserInfo_GetUsername(This,pbstrUsername)	\
    ( (This)->lpVtbl -> GetUsername(This,pbstrUsername) ) 

#define IUOLFoneClientUserInfo_GetIdentity(This,pbstrIdentity)	\
    ( (This)->lpVtbl -> GetIdentity(This,pbstrIdentity) ) 

#define IUOLFoneClientUserInfo_GetPassword(This,pbstrPassword)	\
    ( (This)->lpVtbl -> GetPassword(This,pbstrPassword) ) 

#define IUOLFoneClientUserInfo_GetRealm(This,pbstrRealm)	\
    ( (This)->lpVtbl -> GetRealm(This,pbstrRealm) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientUserInfo_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClientServerInfo_INTERFACE_DEFINED__
#define __IUOLFoneClientServerInfo_INTERFACE_DEFINED__

/* interface IUOLFoneClientServerInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientServerInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("91B2F018-3AD4-433c-9D5C-812E2447A39F")
    IUOLFoneClientServerInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetServerHost( 
            /* [out] */ BSTR *pbstrHost) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetServerPort( 
            /* [out] */ LONG *plPort) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetUsername( 
            /* [out] */ BSTR *pbstrUsername) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPassword( 
            /* [out] */ BSTR *pbstrPassword) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetSipRealm( 
            /* [out] */ BSTR *pbstrSipRealm) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientServerInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientServerInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientServerInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientServerInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientServerInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientServerInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientServerInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientServerInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetServerHost )( 
            IUOLFoneClientServerInfo * This,
            /* [out] */ BSTR *pbstrHost);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetServerPort )( 
            IUOLFoneClientServerInfo * This,
            /* [out] */ LONG *plPort);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUsername )( 
            IUOLFoneClientServerInfo * This,
            /* [out] */ BSTR *pbstrUsername);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPassword )( 
            IUOLFoneClientServerInfo * This,
            /* [out] */ BSTR *pbstrPassword);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetSipRealm )( 
            IUOLFoneClientServerInfo * This,
            /* [out] */ BSTR *pbstrSipRealm);
        
        END_INTERFACE
    } IUOLFoneClientServerInfoVtbl;

    interface IUOLFoneClientServerInfo
    {
        CONST_VTBL struct IUOLFoneClientServerInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientServerInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientServerInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientServerInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientServerInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientServerInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientServerInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientServerInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientServerInfo_GetServerHost(This,pbstrHost)	\
    ( (This)->lpVtbl -> GetServerHost(This,pbstrHost) ) 

#define IUOLFoneClientServerInfo_GetServerPort(This,plPort)	\
    ( (This)->lpVtbl -> GetServerPort(This,plPort) ) 

#define IUOLFoneClientServerInfo_GetUsername(This,pbstrUsername)	\
    ( (This)->lpVtbl -> GetUsername(This,pbstrUsername) ) 

#define IUOLFoneClientServerInfo_GetPassword(This,pbstrPassword)	\
    ( (This)->lpVtbl -> GetPassword(This,pbstrPassword) ) 

#define IUOLFoneClientServerInfo_GetSipRealm(This,pbstrSipRealm)	\
    ( (This)->lpVtbl -> GetSipRealm(This,pbstrSipRealm) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientServerInfo_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClientPhoneLineInfo_INTERFACE_DEFINED__
#define __IUOLFoneClientPhoneLineInfo_INTERFACE_DEFINED__

/* interface IUOLFoneClientPhoneLineInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientPhoneLineInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D3D324E3-A489-45b8-87C5-F2BDF23BA881")
    IUOLFoneClientPhoneLineInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetUserInfo( 
            /* [out] */ IUOLFoneClientUserInfo **ppUserInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetServerInfo( 
            /* [out] */ IUOLFoneClientServerInfo **ppServerInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetProxyInfo( 
            /* [out] */ IUOLFoneClientServerInfo **ppProxyInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetRegisterTimeout( 
            /* [out] */ LONG *plTimeout) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientPhoneLineInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientPhoneLineInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientPhoneLineInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientPhoneLineInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientPhoneLineInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientPhoneLineInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientPhoneLineInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientPhoneLineInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetUserInfo )( 
            IUOLFoneClientPhoneLineInfo * This,
            /* [out] */ IUOLFoneClientUserInfo **ppUserInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetServerInfo )( 
            IUOLFoneClientPhoneLineInfo * This,
            /* [out] */ IUOLFoneClientServerInfo **ppServerInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetProxyInfo )( 
            IUOLFoneClientPhoneLineInfo * This,
            /* [out] */ IUOLFoneClientServerInfo **ppProxyInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetRegisterTimeout )( 
            IUOLFoneClientPhoneLineInfo * This,
            /* [out] */ LONG *plTimeout);
        
        END_INTERFACE
    } IUOLFoneClientPhoneLineInfoVtbl;

    interface IUOLFoneClientPhoneLineInfo
    {
        CONST_VTBL struct IUOLFoneClientPhoneLineInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientPhoneLineInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientPhoneLineInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientPhoneLineInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientPhoneLineInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientPhoneLineInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientPhoneLineInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientPhoneLineInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientPhoneLineInfo_GetUserInfo(This,ppUserInfo)	\
    ( (This)->lpVtbl -> GetUserInfo(This,ppUserInfo) ) 

#define IUOLFoneClientPhoneLineInfo_GetServerInfo(This,ppServerInfo)	\
    ( (This)->lpVtbl -> GetServerInfo(This,ppServerInfo) ) 

#define IUOLFoneClientPhoneLineInfo_GetProxyInfo(This,ppProxyInfo)	\
    ( (This)->lpVtbl -> GetProxyInfo(This,ppProxyInfo) ) 

#define IUOLFoneClientPhoneLineInfo_GetRegisterTimeout(This,plTimeout)	\
    ( (This)->lpVtbl -> GetRegisterTimeout(This,plTimeout) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientPhoneLineInfo_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneClient_0000_0013 */
/* [local] */ 


enum EnumPhoneLineState
    {	UFC_PHONE_LINE_STATE_UNKNOWN	= 0,
	UFC_PHONE_LINE_STATE_UNAUTHORIZED	= 1,
	UFC_PHONE_LINE_STATE_SERVER_ERROR	= 2,
	UFC_PHONE_LINE_STATE_TIMEOUT	= 3,
	UFC_PHONE_LINE_STATE_REGISTERED	= 4,
	UFC_PHONE_LINE_STATE_UNREGISTERED	= 5,
	UFC_PHONE_LINE_STATE_CONNECTION_TIMEOUT	= 6
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0013_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0013_v0_0_s_ifspec;

#ifndef __IUOLFoneClientPhoneLine_INTERFACE_DEFINED__
#define __IUOLFoneClientPhoneLine_INTERFACE_DEFINED__

/* interface IUOLFoneClientPhoneLine */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientPhoneLine;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6F85FA98-C723-48b2-8CD6-1A9F60B74740")
    IUOLFoneClientPhoneLine : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPhoneLineInfo( 
            /* [out] */ IUOLFoneClientPhoneLineInfo **pPhoneLineInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPhoneLineId( 
            /* [out] */ LONG *plLineId) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPhoneLineState( 
            /* [out] */ LONG *plPhoneLineState) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientPhoneLineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientPhoneLine * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientPhoneLine * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientPhoneLine * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientPhoneLine * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientPhoneLine * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientPhoneLine * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientPhoneLine * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPhoneLineInfo )( 
            IUOLFoneClientPhoneLine * This,
            /* [out] */ IUOLFoneClientPhoneLineInfo **pPhoneLineInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPhoneLineId )( 
            IUOLFoneClientPhoneLine * This,
            /* [out] */ LONG *plLineId);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPhoneLineState )( 
            IUOLFoneClientPhoneLine * This,
            /* [out] */ LONG *plPhoneLineState);
        
        END_INTERFACE
    } IUOLFoneClientPhoneLineVtbl;

    interface IUOLFoneClientPhoneLine
    {
        CONST_VTBL struct IUOLFoneClientPhoneLineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientPhoneLine_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientPhoneLine_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientPhoneLine_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientPhoneLine_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientPhoneLine_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientPhoneLine_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientPhoneLine_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientPhoneLine_GetPhoneLineInfo(This,pPhoneLineInfo)	\
    ( (This)->lpVtbl -> GetPhoneLineInfo(This,pPhoneLineInfo) ) 

#define IUOLFoneClientPhoneLine_GetPhoneLineId(This,plLineId)	\
    ( (This)->lpVtbl -> GetPhoneLineId(This,plLineId) ) 

#define IUOLFoneClientPhoneLine_GetPhoneLineState(This,plPhoneLineState)	\
    ( (This)->lpVtbl -> GetPhoneLineState(This,plPhoneLineState) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientPhoneLine_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneClient_0000_0014 */
/* [local] */ 


enum EnumPhoneCallState
    {	UFC_PHONE_CALL_STATE_UNKNOWN	= 0,
	UFC_PHONE_CALL_STATE_ERROR	= 1,
	UFC_PHONE_CALL_STATE_RESUMED	= 2,
	UFC_PHONE_CALL_STATE_TALKING	= 3,
	UFC_PHONE_CALL_STATE_DIALING	= 4,
	UFC_PHONE_CALL_STATE_RINGING	= 5,
	UFC_PHONE_CALL_STATE_CLOSED	= 6,
	UFC_PHONE_CALL_STATE_INCOMING	= 7,
	UFC_PHONE_CALL_STATE_HOLD	= 8,
	UFC_PHONE_CALL_STATE_BUSY	= 9,
	UFC_PHONE_CALL_STATE_NO_ANSWER	= 10,
	UFC_PHONE_CALL_STATE_REDIRECTED	= 11
    } ;

enum EnumTone
    {	UFC_TONE_0	= 0,
	UFC_TONE_1	= 1,
	UFC_TONE_2	= 2,
	UFC_TONE_3	= 3,
	UFC_TONE_4	= 4,
	UFC_TONE_5	= 5,
	UFC_TONE_6	= 6,
	UFC_TONE_7	= 7,
	UFC_TONE_8	= 8,
	UFC_TONE_9	= 9,
	UFC_TONE_STAR	= 10,
	UFC_TONE_POUND	= 11
    } ;

enum EnumFailureCode
    {	UFC_FAILURE_CODE_BAD_REQUEST	= 400,
	UFC_FAILURE_CODE_UNAUTHORIZED	= 401,
	UFC_FAILURE_CODE_PAYMENT_REQUIRED	= 402,
	UFC_FAILURE_CODE_FORBIDDEN	= 403,
	UFC_FAILURE_CODE_NOT_FOUND	= 404,
	UFC_FAILURE_CODE_METHOD_NOT_ALLOWED	= 405,
	UFC_FAILURE_CODE_NOT_ACCEPTABLE	= 406,
	UFC_FAILURE_CODE_PROXY_AUTHENTICATION_REQUIRED	= 407,
	UFC_FAILURE_CODE_TIMEOUT	= 408,
	UFC_FAILURE_CODE_GONE	= 410,
	UFC_FAILURE_CODE_REQUEST_ENTITY_TOO_LARGE	= 413,
	UFC_FAILURE_CODE_REQUEST_URI_TOO_LONG	= 414,
	UFC_FAILURE_CODE_UNSUPORTED_MEDIA	= 415,
	UFC_FAILURE_CODE_UNSUPORTED_URI_SCHEME	= 416,
	UFC_FAILURE_CODE_BAD_EXTENSION	= 420,
	UFC_FAILURE_CODE_EXTENSION_REQUIRED	= 421,
	UFC_FAILURE_CODE_INTERVAL_TOO_BRIEF	= 423,
	UFC_FAILURE_CODE_TEMPORARILY_UNAVAILABLE	= 480,
	UFC_FAILURE_CODE_TRANSACTION_NOT_EXIST	= 481,
	UFC_FAILURE_CODE_LOOP_DETECTED	= 482,
	UFC_FAILURE_CODE_TOO_MANY_HOPS	= 483,
	UFC_FAILURE_CODE_ADDRESS_INCOMPLETE	= 484,
	UFC_FAILURE_CODE_AMBIGUOUS	= 485,
	UFC_FAILURE_CODE_BUSY	= 486,
	UFC_FAILURE_CODE_REQUEST_TERMINATED	= 487,
	UFC_FAILURE_CODE_NOT_ACCEPTABLE_HERE	= 488,
	UFC_FAILURE_CODE_REQUEST_PENDING	= 491,
	UFC_FAILURE_CODE_UNDECIPHERABLE	= 493
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0014_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0014_v0_0_s_ifspec;

#ifndef __IUOLFoneClientPhoneCall_INTERFACE_DEFINED__
#define __IUOLFoneClientPhoneCall_INTERFACE_DEFINED__

/* interface IUOLFoneClientPhoneCall */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientPhoneCall;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B47AC133-41B8-48f5-8E10-D259E4F4E215")
    IUOLFoneClientPhoneCall : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPhoneLine( 
            /* [out] */ IUOLFoneClientPhoneLine **ppPhoneLine) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPhoneURI( 
            /* [out] */ BSTR *pbstrPhoneURI) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsVideoEnabled( 
            /* [out] */ BOOL *pbEnabled) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPhoneCallId( 
            /* [out] */ LONG *plCallId) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetPhoneCallState( 
            /* [out] */ LONG *plCallState) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientPhoneCallVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientPhoneCall * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientPhoneCall * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientPhoneCall * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientPhoneCall * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientPhoneCall * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientPhoneCall * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientPhoneCall * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPhoneLine )( 
            IUOLFoneClientPhoneCall * This,
            /* [out] */ IUOLFoneClientPhoneLine **ppPhoneLine);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPhoneURI )( 
            IUOLFoneClientPhoneCall * This,
            /* [out] */ BSTR *pbstrPhoneURI);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsVideoEnabled )( 
            IUOLFoneClientPhoneCall * This,
            /* [out] */ BOOL *pbEnabled);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPhoneCallId )( 
            IUOLFoneClientPhoneCall * This,
            /* [out] */ LONG *plCallId);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetPhoneCallState )( 
            IUOLFoneClientPhoneCall * This,
            /* [out] */ LONG *plCallState);
        
        END_INTERFACE
    } IUOLFoneClientPhoneCallVtbl;

    interface IUOLFoneClientPhoneCall
    {
        CONST_VTBL struct IUOLFoneClientPhoneCallVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientPhoneCall_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientPhoneCall_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientPhoneCall_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientPhoneCall_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientPhoneCall_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientPhoneCall_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientPhoneCall_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientPhoneCall_GetPhoneLine(This,ppPhoneLine)	\
    ( (This)->lpVtbl -> GetPhoneLine(This,ppPhoneLine) ) 

#define IUOLFoneClientPhoneCall_GetPhoneURI(This,pbstrPhoneURI)	\
    ( (This)->lpVtbl -> GetPhoneURI(This,pbstrPhoneURI) ) 

#define IUOLFoneClientPhoneCall_IsVideoEnabled(This,pbEnabled)	\
    ( (This)->lpVtbl -> IsVideoEnabled(This,pbEnabled) ) 

#define IUOLFoneClientPhoneCall_GetPhoneCallId(This,plCallId)	\
    ( (This)->lpVtbl -> GetPhoneCallId(This,plCallId) ) 

#define IUOLFoneClientPhoneCall_GetPhoneCallState(This,plCallState)	\
    ( (This)->lpVtbl -> GetPhoneCallState(This,plCallState) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientPhoneCall_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneClient_0000_0015 */
/* [local] */ 


enum EnumMessageEventType
    {	UFC_MESSAGE_EVENT_TYPE_UNKNOWN	= -1,
	UFC_MESSAGE_EVENT_TYPE_NEW	= 0,
	UFC_MESSAGE_EVENT_TYPE_SUCCESS	= 1,
	UFC_MESSAGE_EVENT_TYPE_ERROR	= 2
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0015_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0015_v0_0_s_ifspec;

#ifndef __IUOLFoneClientMessageEvent_INTERFACE_DEFINED__
#define __IUOLFoneClientMessageEvent_INTERFACE_DEFINED__

/* interface IUOLFoneClientMessageEvent */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientMessageEvent;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BF2698D5-2628-4dba-B8F9-50D2FEC3CACC")
    IUOLFoneClientMessageEvent : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetEventType( 
            /* [in] */ LONG *plEventType) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetFrom( 
            /* [in] */ BSTR *pbstrFrom) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetTo( 
            /* [in] */ BSTR *pbstrTo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetContentType( 
            /* [in] */ BSTR *pbstrContentType) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetContentSubtype( 
            /* [in] */ BSTR *pbstrContentSubtype) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetContent( 
            /* [in] */ BSTR *pbstrContent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientMessageEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientMessageEvent * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientMessageEvent * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientMessageEvent * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientMessageEvent * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientMessageEvent * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientMessageEvent * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientMessageEvent * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetEventType )( 
            IUOLFoneClientMessageEvent * This,
            /* [in] */ LONG *plEventType);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetFrom )( 
            IUOLFoneClientMessageEvent * This,
            /* [in] */ BSTR *pbstrFrom);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetTo )( 
            IUOLFoneClientMessageEvent * This,
            /* [in] */ BSTR *pbstrTo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetContentType )( 
            IUOLFoneClientMessageEvent * This,
            /* [in] */ BSTR *pbstrContentType);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetContentSubtype )( 
            IUOLFoneClientMessageEvent * This,
            /* [in] */ BSTR *pbstrContentSubtype);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetContent )( 
            IUOLFoneClientMessageEvent * This,
            /* [in] */ BSTR *pbstrContent);
        
        END_INTERFACE
    } IUOLFoneClientMessageEventVtbl;

    interface IUOLFoneClientMessageEvent
    {
        CONST_VTBL struct IUOLFoneClientMessageEventVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientMessageEvent_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientMessageEvent_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientMessageEvent_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientMessageEvent_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientMessageEvent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientMessageEvent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientMessageEvent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientMessageEvent_GetEventType(This,plEventType)	\
    ( (This)->lpVtbl -> GetEventType(This,plEventType) ) 

#define IUOLFoneClientMessageEvent_GetFrom(This,pbstrFrom)	\
    ( (This)->lpVtbl -> GetFrom(This,pbstrFrom) ) 

#define IUOLFoneClientMessageEvent_GetTo(This,pbstrTo)	\
    ( (This)->lpVtbl -> GetTo(This,pbstrTo) ) 

#define IUOLFoneClientMessageEvent_GetContentType(This,pbstrContentType)	\
    ( (This)->lpVtbl -> GetContentType(This,pbstrContentType) ) 

#define IUOLFoneClientMessageEvent_GetContentSubtype(This,pbstrContentSubtype)	\
    ( (This)->lpVtbl -> GetContentSubtype(This,pbstrContentSubtype) ) 

#define IUOLFoneClientMessageEvent_GetContent(This,pbstrContent)	\
    ( (This)->lpVtbl -> GetContent(This,pbstrContent) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientMessageEvent_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneClient_0000_0016 */
/* [local] */ 


enum EnumHttpTunnelMode
    {	UFC_HTTP_TUNNEL_MODE_CONNECT	= 0,
	UFC_HTTP_TUNNEL_MODE_ACCEPT	= 1
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0016_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0016_v0_0_s_ifspec;

#ifndef __IUOLFoneClientConnectionTestParam_INTERFACE_DEFINED__
#define __IUOLFoneClientConnectionTestParam_INTERFACE_DEFINED__

/* interface IUOLFoneClientConnectionTestParam */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientConnectionTestParam;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("99494F1E-9291-4fc8-996D-4937029C3AFF")
    IUOLFoneClientConnectionTestParam : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetServerInfo( 
            /* [out] */ IUOLFoneClientServerInfo **ppServerInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetLocalPort( 
            /* [out] */ LONG *plPort) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetHttpTunnelInfo( 
            /* [out] */ IUOLFoneClientServerInfo **ppHttpTunnelInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetHttpProxyInfo( 
            /* [out] */ IUOLFoneClientServerInfo **ppHttpProxyInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetServerResponseTimeout( 
            /* [out] */ LONG *plTimeout) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetHttpTunnelResponseTimeout( 
            /* [out] */ LONG *plTimeout) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetHttpTunnelMode( 
            /* [out] */ LONG *plMode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientConnectionTestParamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientConnectionTestParam * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientConnectionTestParam * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetServerInfo )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [out] */ IUOLFoneClientServerInfo **ppServerInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetLocalPort )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [out] */ LONG *plPort);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetHttpTunnelInfo )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [out] */ IUOLFoneClientServerInfo **ppHttpTunnelInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetHttpProxyInfo )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [out] */ IUOLFoneClientServerInfo **ppHttpProxyInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetServerResponseTimeout )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [out] */ LONG *plTimeout);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetHttpTunnelResponseTimeout )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [out] */ LONG *plTimeout);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetHttpTunnelMode )( 
            IUOLFoneClientConnectionTestParam * This,
            /* [out] */ LONG *plMode);
        
        END_INTERFACE
    } IUOLFoneClientConnectionTestParamVtbl;

    interface IUOLFoneClientConnectionTestParam
    {
        CONST_VTBL struct IUOLFoneClientConnectionTestParamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientConnectionTestParam_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientConnectionTestParam_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientConnectionTestParam_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientConnectionTestParam_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientConnectionTestParam_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientConnectionTestParam_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientConnectionTestParam_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientConnectionTestParam_GetServerInfo(This,ppServerInfo)	\
    ( (This)->lpVtbl -> GetServerInfo(This,ppServerInfo) ) 

#define IUOLFoneClientConnectionTestParam_GetLocalPort(This,plPort)	\
    ( (This)->lpVtbl -> GetLocalPort(This,plPort) ) 

#define IUOLFoneClientConnectionTestParam_GetHttpTunnelInfo(This,ppHttpTunnelInfo)	\
    ( (This)->lpVtbl -> GetHttpTunnelInfo(This,ppHttpTunnelInfo) ) 

#define IUOLFoneClientConnectionTestParam_GetHttpProxyInfo(This,ppHttpProxyInfo)	\
    ( (This)->lpVtbl -> GetHttpProxyInfo(This,ppHttpProxyInfo) ) 

#define IUOLFoneClientConnectionTestParam_GetServerResponseTimeout(This,plTimeout)	\
    ( (This)->lpVtbl -> GetServerResponseTimeout(This,plTimeout) ) 

#define IUOLFoneClientConnectionTestParam_GetHttpTunnelResponseTimeout(This,plTimeout)	\
    ( (This)->lpVtbl -> GetHttpTunnelResponseTimeout(This,plTimeout) ) 

#define IUOLFoneClientConnectionTestParam_GetHttpTunnelMode(This,plMode)	\
    ( (This)->lpVtbl -> GetHttpTunnelMode(This,plMode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientConnectionTestParam_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneClient_0000_0017 */
/* [local] */ 


enum EnumOsipDebugLevel
    {	OSIP_DEBUG_LEVEL_FATAL	= 0,
	OSIP_DEBUG_LEVEL_BUG	= 1,
	OSIP_DEBUG_LEVEL_ERROR	= 2,
	OSIP_DEBUG_LEVEL_WARNING	= 3,
	OSIP_DEBUG_LEVEL_INFO1	= 4,
	OSIP_DEBUG_LEVEL_INFO2	= 5,
	OSIP_DEBUG_LEVEL_INFO3	= 6,
	OSIP_DEBUG_LEVEL_INFO4	= 7,
	END_OSIP_DEBUG_LEVEL	= 8
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0017_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0017_v0_0_s_ifspec;

#ifndef __IUOLFoneClientOsipDebugInfo_INTERFACE_DEFINED__
#define __IUOLFoneClientOsipDebugInfo_INTERFACE_DEFINED__

/* interface IUOLFoneClientOsipDebugInfo */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientOsipDebugInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B11D71EB-17BC-49d8-9D5E-7D3BD6811AA7")
    IUOLFoneClientOsipDebugInfo : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetFileName( 
            /* [in] */ BSTR *pbstrFileName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetLineNumber( 
            /* [in] */ LONG *plLineNumber) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetLogLevel( 
            /* [in] */ LONG *plLogLevel) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetLogMessage( 
            /* [in] */ BSTR *pbstrLogMessage) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientOsipDebugInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientOsipDebugInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientOsipDebugInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientOsipDebugInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientOsipDebugInfo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientOsipDebugInfo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientOsipDebugInfo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientOsipDebugInfo * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetFileName )( 
            IUOLFoneClientOsipDebugInfo * This,
            /* [in] */ BSTR *pbstrFileName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetLineNumber )( 
            IUOLFoneClientOsipDebugInfo * This,
            /* [in] */ LONG *plLineNumber);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetLogLevel )( 
            IUOLFoneClientOsipDebugInfo * This,
            /* [in] */ LONG *plLogLevel);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetLogMessage )( 
            IUOLFoneClientOsipDebugInfo * This,
            /* [in] */ BSTR *pbstrLogMessage);
        
        END_INTERFACE
    } IUOLFoneClientOsipDebugInfoVtbl;

    interface IUOLFoneClientOsipDebugInfo
    {
        CONST_VTBL struct IUOLFoneClientOsipDebugInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientOsipDebugInfo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientOsipDebugInfo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientOsipDebugInfo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientOsipDebugInfo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientOsipDebugInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientOsipDebugInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientOsipDebugInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientOsipDebugInfo_GetFileName(This,pbstrFileName)	\
    ( (This)->lpVtbl -> GetFileName(This,pbstrFileName) ) 

#define IUOLFoneClientOsipDebugInfo_GetLineNumber(This,plLineNumber)	\
    ( (This)->lpVtbl -> GetLineNumber(This,plLineNumber) ) 

#define IUOLFoneClientOsipDebugInfo_GetLogLevel(This,plLogLevel)	\
    ( (This)->lpVtbl -> GetLogLevel(This,plLogLevel) ) 

#define IUOLFoneClientOsipDebugInfo_GetLogMessage(This,pbstrLogMessage)	\
    ( (This)->lpVtbl -> GetLogMessage(This,pbstrLogMessage) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientOsipDebugInfo_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneClient_0000_0018 */
/* [local] */ 


enum EnumConnectionFlags
    {	UFC_CONNECTION_FLAGS_USE_HTTP_TUNNEL	= 1,
	UFC_CONNECTION_FLAGS_USE_HTTP_PROXY	= 2,
	UFC_CONNECTION_FLAGS_USE_SSL	= 4
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0018_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0018_v0_0_s_ifspec;

#ifndef __IUOLFoneClientNetworkConfig_INTERFACE_DEFINED__
#define __IUOLFoneClientNetworkConfig_INTERFACE_DEFINED__

/* interface IUOLFoneClientNetworkConfig */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientNetworkConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7DF5F072-FB83-4a24-9932-E03EBCA7BE13")
    IUOLFoneClientNetworkConfig : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetConnectionFlags( 
            /* [out] */ LONG *plConnectionFlags) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetHttpTunnelInfo( 
            /* [out] */ IUOLFoneClientServerInfo **ppHttpTunnelInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetHttpProxyInfo( 
            /* [out] */ IUOLFoneClientServerInfo **ppHttpProxyInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientNetworkConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientNetworkConfig * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientNetworkConfig * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientNetworkConfig * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientNetworkConfig * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientNetworkConfig * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientNetworkConfig * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientNetworkConfig * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetConnectionFlags )( 
            IUOLFoneClientNetworkConfig * This,
            /* [out] */ LONG *plConnectionFlags);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetHttpTunnelInfo )( 
            IUOLFoneClientNetworkConfig * This,
            /* [out] */ IUOLFoneClientServerInfo **ppHttpTunnelInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetHttpProxyInfo )( 
            IUOLFoneClientNetworkConfig * This,
            /* [out] */ IUOLFoneClientServerInfo **ppHttpProxyInfo);
        
        END_INTERFACE
    } IUOLFoneClientNetworkConfigVtbl;

    interface IUOLFoneClientNetworkConfig
    {
        CONST_VTBL struct IUOLFoneClientNetworkConfigVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientNetworkConfig_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientNetworkConfig_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientNetworkConfig_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientNetworkConfig_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientNetworkConfig_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientNetworkConfig_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientNetworkConfig_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientNetworkConfig_GetConnectionFlags(This,plConnectionFlags)	\
    ( (This)->lpVtbl -> GetConnectionFlags(This,plConnectionFlags) ) 

#define IUOLFoneClientNetworkConfig_GetHttpTunnelInfo(This,ppHttpTunnelInfo)	\
    ( (This)->lpVtbl -> GetHttpTunnelInfo(This,ppHttpTunnelInfo) ) 

#define IUOLFoneClientNetworkConfig_GetHttpProxyInfo(This,ppHttpProxyInfo)	\
    ( (This)->lpVtbl -> GetHttpProxyInfo(This,ppHttpProxyInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientNetworkConfig_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClientPhoneLineConfig_INTERFACE_DEFINED__
#define __IUOLFoneClientPhoneLineConfig_INTERFACE_DEFINED__

/* interface IUOLFoneClientPhoneLineConfig */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientPhoneLineConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("23583CC4-435E-4061-AEBB-FAC1DF2E9018")
    IUOLFoneClientPhoneLineConfig : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetConnectionPort( 
            /* [out] */ LONG *plPort) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetServerNetworkConfig( 
            /* [out] */ IUOLFoneClientNetworkConfig **ppNetworkConfig) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetMediaNetworkConfig( 
            /* [out] */ IUOLFoneClientNetworkConfig **ppNetworkConfig) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetAudioCodecsPrecedence( 
            /* [out] */ BSTR *pbstrAudioCodecsPrecedence) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetVideoCodecsPrecedence( 
            /* [out] */ BSTR *pbstrVideoCodecsPrecedence) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientPhoneLineConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientPhoneLineConfig * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientPhoneLineConfig * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientPhoneLineConfig * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientPhoneLineConfig * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientPhoneLineConfig * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientPhoneLineConfig * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientPhoneLineConfig * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetConnectionPort )( 
            IUOLFoneClientPhoneLineConfig * This,
            /* [out] */ LONG *plPort);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetServerNetworkConfig )( 
            IUOLFoneClientPhoneLineConfig * This,
            /* [out] */ IUOLFoneClientNetworkConfig **ppNetworkConfig);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetMediaNetworkConfig )( 
            IUOLFoneClientPhoneLineConfig * This,
            /* [out] */ IUOLFoneClientNetworkConfig **ppNetworkConfig);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetAudioCodecsPrecedence )( 
            IUOLFoneClientPhoneLineConfig * This,
            /* [out] */ BSTR *pbstrAudioCodecsPrecedence);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVideoCodecsPrecedence )( 
            IUOLFoneClientPhoneLineConfig * This,
            /* [out] */ BSTR *pbstrVideoCodecsPrecedence);
        
        END_INTERFACE
    } IUOLFoneClientPhoneLineConfigVtbl;

    interface IUOLFoneClientPhoneLineConfig
    {
        CONST_VTBL struct IUOLFoneClientPhoneLineConfigVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientPhoneLineConfig_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientPhoneLineConfig_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientPhoneLineConfig_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientPhoneLineConfig_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientPhoneLineConfig_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientPhoneLineConfig_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientPhoneLineConfig_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientPhoneLineConfig_GetConnectionPort(This,plPort)	\
    ( (This)->lpVtbl -> GetConnectionPort(This,plPort) ) 

#define IUOLFoneClientPhoneLineConfig_GetServerNetworkConfig(This,ppNetworkConfig)	\
    ( (This)->lpVtbl -> GetServerNetworkConfig(This,ppNetworkConfig) ) 

#define IUOLFoneClientPhoneLineConfig_GetMediaNetworkConfig(This,ppNetworkConfig)	\
    ( (This)->lpVtbl -> GetMediaNetworkConfig(This,ppNetworkConfig) ) 

#define IUOLFoneClientPhoneLineConfig_GetAudioCodecsPrecedence(This,pbstrAudioCodecsPrecedence)	\
    ( (This)->lpVtbl -> GetAudioCodecsPrecedence(This,pbstrAudioCodecsPrecedence) ) 

#define IUOLFoneClientPhoneLineConfig_GetVideoCodecsPrecedence(This,pbstrVideoCodecsPrecedence)	\
    ( (This)->lpVtbl -> GetVideoCodecsPrecedence(This,pbstrVideoCodecsPrecedence) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientPhoneLineConfig_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneClient_0000_0020 */
/* [local] */ 


enum EnumPhoneCallStreamError
    {	UFC_PHONE_CALL_STREAM_AUDIO_RECEIVE_TIMEOUT	= 0,
	UFC_PHONE_CALL_STREAM_VIDEO_RECEIVE_TIMEOUT	= 1
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0020_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0020_v0_0_s_ifspec;

#ifndef __IUOLFoneClientCallback_INTERFACE_DEFINED__
#define __IUOLFoneClientCallback_INTERFACE_DEFINED__

/* interface IUOLFoneClientCallback */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6C345696-8407-4d24-BBD0-53CEED2EF8D0")
    IUOLFoneClientCallback : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PhoneLineStateChanged( 
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ enum EnumPhoneLineState phoneLineState,
            /* [in] */ IUOLFoneClientExtraResponseData2 *pExtraResponseData) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PhoneCallStateChanged( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumPhoneCallState phoneCallState,
            /* [in] */ IUOLFoneClientExtraResponseData2 *pExtraResponseData) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE MessageEvent( 
            /* [in] */ IUOLFoneClientMessageEvent *pMessageEvent,
            /* [in] */ IUOLFoneClientExtraResponseData2 *pExtraResponseData) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PhoneCallStreamError( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumPhoneCallStreamError phoneCallStreamError) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PhoneLineConnectionTimeout( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientCallback * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientCallback * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientCallback * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientCallback * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PhoneLineStateChanged )( 
            IUOLFoneClientCallback * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ enum EnumPhoneLineState phoneLineState,
            /* [in] */ IUOLFoneClientExtraResponseData2 *pExtraResponseData);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PhoneCallStateChanged )( 
            IUOLFoneClientCallback * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumPhoneCallState phoneCallState,
            /* [in] */ IUOLFoneClientExtraResponseData2 *pExtraResponseData);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *MessageEvent )( 
            IUOLFoneClientCallback * This,
            /* [in] */ IUOLFoneClientMessageEvent *pMessageEvent,
            /* [in] */ IUOLFoneClientExtraResponseData2 *pExtraResponseData);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PhoneCallStreamError )( 
            IUOLFoneClientCallback * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumPhoneCallStreamError phoneCallStreamError);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PhoneLineConnectionTimeout )( 
            IUOLFoneClientCallback * This);
        
        END_INTERFACE
    } IUOLFoneClientCallbackVtbl;

    interface IUOLFoneClientCallback
    {
        CONST_VTBL struct IUOLFoneClientCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientCallback_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientCallback_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientCallback_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientCallback_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientCallback_PhoneLineStateChanged(This,pPhoneLine,phoneLineState,pExtraResponseData)	\
    ( (This)->lpVtbl -> PhoneLineStateChanged(This,pPhoneLine,phoneLineState,pExtraResponseData) ) 

#define IUOLFoneClientCallback_PhoneCallStateChanged(This,pPhoneCall,phoneCallState,pExtraResponseData)	\
    ( (This)->lpVtbl -> PhoneCallStateChanged(This,pPhoneCall,phoneCallState,pExtraResponseData) ) 

#define IUOLFoneClientCallback_MessageEvent(This,pMessageEvent,pExtraResponseData)	\
    ( (This)->lpVtbl -> MessageEvent(This,pMessageEvent,pExtraResponseData) ) 

#define IUOLFoneClientCallback_PhoneCallStreamError(This,pPhoneCall,phoneCallStreamError)	\
    ( (This)->lpVtbl -> PhoneCallStreamError(This,pPhoneCall,phoneCallStreamError) ) 

#define IUOLFoneClientCallback_PhoneLineConnectionTimeout(This)	\
    ( (This)->lpVtbl -> PhoneLineConnectionTimeout(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientCallback_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClientCallback2_INTERFACE_DEFINED__
#define __IUOLFoneClientCallback2_INTERFACE_DEFINED__

/* interface IUOLFoneClientCallback2 */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClientCallback2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("135D74CC-E412-48a5-B8D4-C91828167094")
    IUOLFoneClientCallback2 : public IUOLFoneClientCallback
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE OsipDebugEvent( 
            /* [in] */ IUOLFoneClientOsipDebugInfo *pOsipDebugInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientCallback2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClientCallback2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClientCallback2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClientCallback2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClientCallback2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClientCallback2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClientCallback2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClientCallback2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PhoneLineStateChanged )( 
            IUOLFoneClientCallback2 * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ enum EnumPhoneLineState phoneLineState,
            /* [in] */ IUOLFoneClientExtraResponseData2 *pExtraResponseData);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PhoneCallStateChanged )( 
            IUOLFoneClientCallback2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumPhoneCallState phoneCallState,
            /* [in] */ IUOLFoneClientExtraResponseData2 *pExtraResponseData);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *MessageEvent )( 
            IUOLFoneClientCallback2 * This,
            /* [in] */ IUOLFoneClientMessageEvent *pMessageEvent,
            /* [in] */ IUOLFoneClientExtraResponseData2 *pExtraResponseData);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PhoneCallStreamError )( 
            IUOLFoneClientCallback2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumPhoneCallStreamError phoneCallStreamError);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PhoneLineConnectionTimeout )( 
            IUOLFoneClientCallback2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *OsipDebugEvent )( 
            IUOLFoneClientCallback2 * This,
            /* [in] */ IUOLFoneClientOsipDebugInfo *pOsipDebugInfo);
        
        END_INTERFACE
    } IUOLFoneClientCallback2Vtbl;

    interface IUOLFoneClientCallback2
    {
        CONST_VTBL struct IUOLFoneClientCallback2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClientCallback2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClientCallback2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClientCallback2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClientCallback2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClientCallback2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClientCallback2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClientCallback2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClientCallback2_PhoneLineStateChanged(This,pPhoneLine,phoneLineState,pExtraResponseData)	\
    ( (This)->lpVtbl -> PhoneLineStateChanged(This,pPhoneLine,phoneLineState,pExtraResponseData) ) 

#define IUOLFoneClientCallback2_PhoneCallStateChanged(This,pPhoneCall,phoneCallState,pExtraResponseData)	\
    ( (This)->lpVtbl -> PhoneCallStateChanged(This,pPhoneCall,phoneCallState,pExtraResponseData) ) 

#define IUOLFoneClientCallback2_MessageEvent(This,pMessageEvent,pExtraResponseData)	\
    ( (This)->lpVtbl -> MessageEvent(This,pMessageEvent,pExtraResponseData) ) 

#define IUOLFoneClientCallback2_PhoneCallStreamError(This,pPhoneCall,phoneCallStreamError)	\
    ( (This)->lpVtbl -> PhoneCallStreamError(This,pPhoneCall,phoneCallStreamError) ) 

#define IUOLFoneClientCallback2_PhoneLineConnectionTimeout(This)	\
    ( (This)->lpVtbl -> PhoneLineConnectionTimeout(This) ) 


#define IUOLFoneClientCallback2_OsipDebugEvent(This,pOsipDebugInfo)	\
    ( (This)->lpVtbl -> OsipDebugEvent(This,pOsipDebugInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClientCallback2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf__UOLFoneClient_0000_0022 */
/* [local] */ 


enum EnumAudioDeviceType
    {	UFC_AUDIO_DEVICE_TYPE_CALL_INPUT	= 0,
	UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT	= 1
    } ;

enum EnumVideoQuality
    {	UFC_VIDEO_QUALITY_NORMAL	= 0,
	UFC_VIDEO_QUALITY_GOOD	= 1,
	UFC_VIDEO_QUALITY_VERY_GOOD	= 2,
	UFC_VIDEO_QUALITY_EXCELLENT	= 3
    } ;


extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0022_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf__UOLFoneClient_0000_0022_v0_0_s_ifspec;

#ifndef __IUOLFoneClient_INTERFACE_DEFINED__
#define __IUOLFoneClient_INTERFACE_DEFINED__

/* interface IUOLFoneClient */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClient;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1B96FC0D-CA4D-4ABC-BF64-6985E97E0717")
    IUOLFoneClient : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IUOLFoneClientCallback *pCallback,
            /* [in] */ BSTR bstrCodecsPath) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Finalize( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE TestConnection( 
            /* [in] */ IUOLFoneClientConnectionTestParam *pConnectionTestParam,
            /* [out] */ IUOLFoneClientConnectionTestResult **ppConnectionTestResult) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EnumerateAudioCodecs( 
            /* [out] */ IUOLFoneClientCodecInfoEnum **ppCodecInfoEnum) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EnumerateVideoCodecs( 
            /* [out] */ IUOLFoneClientCodecInfoEnum **ppCodecInfoEnum) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetAudioCodecsPrecedence( 
            /* [out] */ BSTR *pbstrAudioCodecs) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetVideoCodecsPrecedence( 
            /* [out] */ BSTR *pbstrVideoCodecs) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE InitializePhoneLines( 
            /* [in] */ IUOLFoneClientPhoneLineConfig *pPhoneLineConfig) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FinalizePhoneLines( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RegisterPhoneLine( 
            /* [in] */ IUOLFoneClientPhoneLineInfo *pPhoneLineInfo,
            /* [in] */ IUOLFoneClientNetworkConfig *pNetworkConfig,
            /* [out] */ IUOLFoneClientPhoneLine **ppPhoneLine) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE UnregisterPhoneLine( 
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE MakePhoneCall( 
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BSTR bstrPhoneURI,
            /* [in] */ BOOL bEnableVideo,
            /* [out] */ IUOLFoneClientPhoneCall **ppPhoneCall) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetMediaNetworkConfig( 
            /* [in] */ IUOLFoneClientNetworkConfig *pNetworkConfig) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AcceptPhoneCall( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ BOOL bEnableVideo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE RejectPhoneCall( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumFailureCode rejectReason) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ClosePhoneCall( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE HoldPhoneCall( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ResumePhoneCall( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE TransferPhoneCall( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ BSTR bstrPhoneURI) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ReconfigureCallMedia( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ IUOLFoneClientNetworkConfig *pNetworkConfig) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PlayTone( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumTone tone) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PlaySoundFile( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ BSTR bstrSoundFilePath) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetAudioCodec( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [out] */ IUOLFoneClientCodecInfo **pCodecInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetVideoCodec( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [out] */ IUOLFoneClientCodecInfo **pCodecInfo) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EnumerateAudioDevices( 
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ IUOLFoneClientDeviceInfoEnum **ppDeviceInfoEnum) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EnumerateVideoDevices( 
            /* [out] */ IUOLFoneClientDeviceInfoEnum **ppDeviceInfoEnum) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetAudioDevice( 
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ BSTR bstrDeviceName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetAudioDevice( 
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [out] */ BSTR *pbstrDeviceName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EnableAcousticEchoCancellation( 
            /* [in] */ BOOL bEnable) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsAcousticEchoCancellationEnabled( 
            /* [out] */ BOOL *pbEnabled) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EnableHalfDuplex( 
            /* [in] */ BOOL bEnable) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsHalfDuplexEnabled( 
            /* [in] */ BOOL *pbEnabled) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetVolumeLevel( 
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ LONG lLevel) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetVolumeLevel( 
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [out] */ LONG *plLevel) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetMute( 
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ BOOL bMute) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetMute( 
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [out] */ BOOL *pbMute) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetVideoDevice( 
            /* [in] */ BSTR bstrDeviceName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetVideoDevice( 
            /* [out] */ BSTR *pbstrDeviceName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetVideoQuality( 
            /* [in] */ LONG lVideoQuality) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetVideoQuality( 
            /* [out] */ LONG *plVideoQuality) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SendInstantMessage( 
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BSTR bstrMessage,
            /* [in] */ BSTR bstrUserURI) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SendRingingNotification( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CreateConferenceCall( 
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [out] */ IUOLFoneClientConferenceCall **pConferenceCall) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ConferenceInvite( 
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall,
            /* [in] */ BSTR bstrPhoneURI,
            /* [in] */ BOOL bEnableVideo,
            /* [out] */ IUOLFoneClientPhoneCall **ppPhoneCall) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ConferenceAddMember( 
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ConferenceRemoveMember( 
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DestroyConferenceCall( 
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetLineCallForwardNumber( 
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BSTR bstrCallForwardNumber) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetLineBusyFlag( 
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BOOL bIsBusy) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClientVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClient * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClient * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClient * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClient * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClient * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClient * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClient * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientCallback *pCallback,
            /* [in] */ BSTR bstrCodecsPath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Finalize )( 
            IUOLFoneClient * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *TestConnection )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientConnectionTestParam *pConnectionTestParam,
            /* [out] */ IUOLFoneClientConnectionTestResult **ppConnectionTestResult);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnumerateAudioCodecs )( 
            IUOLFoneClient * This,
            /* [out] */ IUOLFoneClientCodecInfoEnum **ppCodecInfoEnum);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnumerateVideoCodecs )( 
            IUOLFoneClient * This,
            /* [out] */ IUOLFoneClientCodecInfoEnum **ppCodecInfoEnum);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetAudioCodecsPrecedence )( 
            IUOLFoneClient * This,
            /* [out] */ BSTR *pbstrAudioCodecs);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVideoCodecsPrecedence )( 
            IUOLFoneClient * This,
            /* [out] */ BSTR *pbstrVideoCodecs);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *InitializePhoneLines )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneLineConfig *pPhoneLineConfig);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FinalizePhoneLines )( 
            IUOLFoneClient * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RegisterPhoneLine )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneLineInfo *pPhoneLineInfo,
            /* [in] */ IUOLFoneClientNetworkConfig *pNetworkConfig,
            /* [out] */ IUOLFoneClientPhoneLine **ppPhoneLine);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UnregisterPhoneLine )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *MakePhoneCall )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BSTR bstrPhoneURI,
            /* [in] */ BOOL bEnableVideo,
            /* [out] */ IUOLFoneClientPhoneCall **ppPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetMediaNetworkConfig )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientNetworkConfig *pNetworkConfig);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AcceptPhoneCall )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ BOOL bEnableVideo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RejectPhoneCall )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumFailureCode rejectReason);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ClosePhoneCall )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *HoldPhoneCall )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ResumePhoneCall )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *TransferPhoneCall )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ BSTR bstrPhoneURI);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ReconfigureCallMedia )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ IUOLFoneClientNetworkConfig *pNetworkConfig);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PlayTone )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumTone tone);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PlaySoundFile )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ BSTR bstrSoundFilePath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetAudioCodec )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [out] */ IUOLFoneClientCodecInfo **pCodecInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVideoCodec )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [out] */ IUOLFoneClientCodecInfo **pCodecInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnumerateAudioDevices )( 
            IUOLFoneClient * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ IUOLFoneClientDeviceInfoEnum **ppDeviceInfoEnum);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnumerateVideoDevices )( 
            IUOLFoneClient * This,
            /* [out] */ IUOLFoneClientDeviceInfoEnum **ppDeviceInfoEnum);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetAudioDevice )( 
            IUOLFoneClient * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ BSTR bstrDeviceName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetAudioDevice )( 
            IUOLFoneClient * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [out] */ BSTR *pbstrDeviceName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnableAcousticEchoCancellation )( 
            IUOLFoneClient * This,
            /* [in] */ BOOL bEnable);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsAcousticEchoCancellationEnabled )( 
            IUOLFoneClient * This,
            /* [out] */ BOOL *pbEnabled);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnableHalfDuplex )( 
            IUOLFoneClient * This,
            /* [in] */ BOOL bEnable);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsHalfDuplexEnabled )( 
            IUOLFoneClient * This,
            /* [in] */ BOOL *pbEnabled);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetVolumeLevel )( 
            IUOLFoneClient * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ LONG lLevel);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVolumeLevel )( 
            IUOLFoneClient * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [out] */ LONG *plLevel);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetMute )( 
            IUOLFoneClient * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ BOOL bMute);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetMute )( 
            IUOLFoneClient * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [out] */ BOOL *pbMute);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetVideoDevice )( 
            IUOLFoneClient * This,
            /* [in] */ BSTR bstrDeviceName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVideoDevice )( 
            IUOLFoneClient * This,
            /* [out] */ BSTR *pbstrDeviceName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetVideoQuality )( 
            IUOLFoneClient * This,
            /* [in] */ LONG lVideoQuality);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVideoQuality )( 
            IUOLFoneClient * This,
            /* [out] */ LONG *plVideoQuality);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SendInstantMessage )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BSTR bstrMessage,
            /* [in] */ BSTR bstrUserURI);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SendRingingNotification )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateConferenceCall )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [out] */ IUOLFoneClientConferenceCall **pConferenceCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ConferenceInvite )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall,
            /* [in] */ BSTR bstrPhoneURI,
            /* [in] */ BOOL bEnableVideo,
            /* [out] */ IUOLFoneClientPhoneCall **ppPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ConferenceAddMember )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ConferenceRemoveMember )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DestroyConferenceCall )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetLineCallForwardNumber )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BSTR bstrCallForwardNumber);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetLineBusyFlag )( 
            IUOLFoneClient * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BOOL bIsBusy);
        
        END_INTERFACE
    } IUOLFoneClientVtbl;

    interface IUOLFoneClient
    {
        CONST_VTBL struct IUOLFoneClientVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClient_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClient_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClient_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClient_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClient_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClient_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClient_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClient_Initialize(This,pCallback,bstrCodecsPath)	\
    ( (This)->lpVtbl -> Initialize(This,pCallback,bstrCodecsPath) ) 

#define IUOLFoneClient_Finalize(This)	\
    ( (This)->lpVtbl -> Finalize(This) ) 

#define IUOLFoneClient_TestConnection(This,pConnectionTestParam,ppConnectionTestResult)	\
    ( (This)->lpVtbl -> TestConnection(This,pConnectionTestParam,ppConnectionTestResult) ) 

#define IUOLFoneClient_EnumerateAudioCodecs(This,ppCodecInfoEnum)	\
    ( (This)->lpVtbl -> EnumerateAudioCodecs(This,ppCodecInfoEnum) ) 

#define IUOLFoneClient_EnumerateVideoCodecs(This,ppCodecInfoEnum)	\
    ( (This)->lpVtbl -> EnumerateVideoCodecs(This,ppCodecInfoEnum) ) 

#define IUOLFoneClient_GetAudioCodecsPrecedence(This,pbstrAudioCodecs)	\
    ( (This)->lpVtbl -> GetAudioCodecsPrecedence(This,pbstrAudioCodecs) ) 

#define IUOLFoneClient_GetVideoCodecsPrecedence(This,pbstrVideoCodecs)	\
    ( (This)->lpVtbl -> GetVideoCodecsPrecedence(This,pbstrVideoCodecs) ) 

#define IUOLFoneClient_InitializePhoneLines(This,pPhoneLineConfig)	\
    ( (This)->lpVtbl -> InitializePhoneLines(This,pPhoneLineConfig) ) 

#define IUOLFoneClient_FinalizePhoneLines(This)	\
    ( (This)->lpVtbl -> FinalizePhoneLines(This) ) 

#define IUOLFoneClient_RegisterPhoneLine(This,pPhoneLineInfo,pNetworkConfig,ppPhoneLine)	\
    ( (This)->lpVtbl -> RegisterPhoneLine(This,pPhoneLineInfo,pNetworkConfig,ppPhoneLine) ) 

#define IUOLFoneClient_UnregisterPhoneLine(This,pPhoneLine)	\
    ( (This)->lpVtbl -> UnregisterPhoneLine(This,pPhoneLine) ) 

#define IUOLFoneClient_MakePhoneCall(This,pPhoneLine,bstrPhoneURI,bEnableVideo,ppPhoneCall)	\
    ( (This)->lpVtbl -> MakePhoneCall(This,pPhoneLine,bstrPhoneURI,bEnableVideo,ppPhoneCall) ) 

#define IUOLFoneClient_SetMediaNetworkConfig(This,pNetworkConfig)	\
    ( (This)->lpVtbl -> SetMediaNetworkConfig(This,pNetworkConfig) ) 

#define IUOLFoneClient_AcceptPhoneCall(This,pPhoneCall,bEnableVideo)	\
    ( (This)->lpVtbl -> AcceptPhoneCall(This,pPhoneCall,bEnableVideo) ) 

#define IUOLFoneClient_RejectPhoneCall(This,pPhoneCall,rejectReason)	\
    ( (This)->lpVtbl -> RejectPhoneCall(This,pPhoneCall,rejectReason) ) 

#define IUOLFoneClient_ClosePhoneCall(This,pPhoneCall)	\
    ( (This)->lpVtbl -> ClosePhoneCall(This,pPhoneCall) ) 

#define IUOLFoneClient_HoldPhoneCall(This,pPhoneCall)	\
    ( (This)->lpVtbl -> HoldPhoneCall(This,pPhoneCall) ) 

#define IUOLFoneClient_ResumePhoneCall(This,pPhoneCall)	\
    ( (This)->lpVtbl -> ResumePhoneCall(This,pPhoneCall) ) 

#define IUOLFoneClient_TransferPhoneCall(This,pPhoneCall,bstrPhoneURI)	\
    ( (This)->lpVtbl -> TransferPhoneCall(This,pPhoneCall,bstrPhoneURI) ) 

#define IUOLFoneClient_ReconfigureCallMedia(This,pPhoneCall,pNetworkConfig)	\
    ( (This)->lpVtbl -> ReconfigureCallMedia(This,pPhoneCall,pNetworkConfig) ) 

#define IUOLFoneClient_PlayTone(This,pPhoneCall,tone)	\
    ( (This)->lpVtbl -> PlayTone(This,pPhoneCall,tone) ) 

#define IUOLFoneClient_PlaySoundFile(This,pPhoneCall,bstrSoundFilePath)	\
    ( (This)->lpVtbl -> PlaySoundFile(This,pPhoneCall,bstrSoundFilePath) ) 

#define IUOLFoneClient_GetAudioCodec(This,pPhoneCall,pCodecInfo)	\
    ( (This)->lpVtbl -> GetAudioCodec(This,pPhoneCall,pCodecInfo) ) 

#define IUOLFoneClient_GetVideoCodec(This,pPhoneCall,pCodecInfo)	\
    ( (This)->lpVtbl -> GetVideoCodec(This,pPhoneCall,pCodecInfo) ) 

#define IUOLFoneClient_EnumerateAudioDevices(This,audioDeviceType,ppDeviceInfoEnum)	\
    ( (This)->lpVtbl -> EnumerateAudioDevices(This,audioDeviceType,ppDeviceInfoEnum) ) 

#define IUOLFoneClient_EnumerateVideoDevices(This,ppDeviceInfoEnum)	\
    ( (This)->lpVtbl -> EnumerateVideoDevices(This,ppDeviceInfoEnum) ) 

#define IUOLFoneClient_SetAudioDevice(This,audioDeviceType,bstrDeviceName)	\
    ( (This)->lpVtbl -> SetAudioDevice(This,audioDeviceType,bstrDeviceName) ) 

#define IUOLFoneClient_GetAudioDevice(This,audioDeviceType,pbstrDeviceName)	\
    ( (This)->lpVtbl -> GetAudioDevice(This,audioDeviceType,pbstrDeviceName) ) 

#define IUOLFoneClient_EnableAcousticEchoCancellation(This,bEnable)	\
    ( (This)->lpVtbl -> EnableAcousticEchoCancellation(This,bEnable) ) 

#define IUOLFoneClient_IsAcousticEchoCancellationEnabled(This,pbEnabled)	\
    ( (This)->lpVtbl -> IsAcousticEchoCancellationEnabled(This,pbEnabled) ) 

#define IUOLFoneClient_EnableHalfDuplex(This,bEnable)	\
    ( (This)->lpVtbl -> EnableHalfDuplex(This,bEnable) ) 

#define IUOLFoneClient_IsHalfDuplexEnabled(This,pbEnabled)	\
    ( (This)->lpVtbl -> IsHalfDuplexEnabled(This,pbEnabled) ) 

#define IUOLFoneClient_SetVolumeLevel(This,audioDeviceType,lLevel)	\
    ( (This)->lpVtbl -> SetVolumeLevel(This,audioDeviceType,lLevel) ) 

#define IUOLFoneClient_GetVolumeLevel(This,audioDeviceType,plLevel)	\
    ( (This)->lpVtbl -> GetVolumeLevel(This,audioDeviceType,plLevel) ) 

#define IUOLFoneClient_SetMute(This,audioDeviceType,bMute)	\
    ( (This)->lpVtbl -> SetMute(This,audioDeviceType,bMute) ) 

#define IUOLFoneClient_GetMute(This,audioDeviceType,pbMute)	\
    ( (This)->lpVtbl -> GetMute(This,audioDeviceType,pbMute) ) 

#define IUOLFoneClient_SetVideoDevice(This,bstrDeviceName)	\
    ( (This)->lpVtbl -> SetVideoDevice(This,bstrDeviceName) ) 

#define IUOLFoneClient_GetVideoDevice(This,pbstrDeviceName)	\
    ( (This)->lpVtbl -> GetVideoDevice(This,pbstrDeviceName) ) 

#define IUOLFoneClient_SetVideoQuality(This,lVideoQuality)	\
    ( (This)->lpVtbl -> SetVideoQuality(This,lVideoQuality) ) 

#define IUOLFoneClient_GetVideoQuality(This,plVideoQuality)	\
    ( (This)->lpVtbl -> GetVideoQuality(This,plVideoQuality) ) 

#define IUOLFoneClient_SendInstantMessage(This,pPhoneLine,bstrMessage,bstrUserURI)	\
    ( (This)->lpVtbl -> SendInstantMessage(This,pPhoneLine,bstrMessage,bstrUserURI) ) 

#define IUOLFoneClient_SendRingingNotification(This,pPhoneCall)	\
    ( (This)->lpVtbl -> SendRingingNotification(This,pPhoneCall) ) 

#define IUOLFoneClient_CreateConferenceCall(This,pPhoneCall,pConferenceCall)	\
    ( (This)->lpVtbl -> CreateConferenceCall(This,pPhoneCall,pConferenceCall) ) 

#define IUOLFoneClient_ConferenceInvite(This,pPhoneLine,pConferenceCall,bstrPhoneURI,bEnableVideo,ppPhoneCall)	\
    ( (This)->lpVtbl -> ConferenceInvite(This,pPhoneLine,pConferenceCall,bstrPhoneURI,bEnableVideo,ppPhoneCall) ) 

#define IUOLFoneClient_ConferenceAddMember(This,pConferenceCall,pPhoneCall)	\
    ( (This)->lpVtbl -> ConferenceAddMember(This,pConferenceCall,pPhoneCall) ) 

#define IUOLFoneClient_ConferenceRemoveMember(This,pConferenceCall,pPhoneCall)	\
    ( (This)->lpVtbl -> ConferenceRemoveMember(This,pConferenceCall,pPhoneCall) ) 

#define IUOLFoneClient_DestroyConferenceCall(This,pConferenceCall)	\
    ( (This)->lpVtbl -> DestroyConferenceCall(This,pConferenceCall) ) 

#define IUOLFoneClient_SetLineCallForwardNumber(This,pPhoneLine,bstrCallForwardNumber)	\
    ( (This)->lpVtbl -> SetLineCallForwardNumber(This,pPhoneLine,bstrCallForwardNumber) ) 

#define IUOLFoneClient_SetLineBusyFlag(This,pPhoneLine,bIsBusy)	\
    ( (This)->lpVtbl -> SetLineBusyFlag(This,pPhoneLine,bIsBusy) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClient_INTERFACE_DEFINED__ */


#ifndef __IUOLFoneClient2_INTERFACE_DEFINED__
#define __IUOLFoneClient2_INTERFACE_DEFINED__

/* interface IUOLFoneClient2 */
/* [helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IUOLFoneClient2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FBAEDE5C-6971-4c74-9B8E-11B407566B40")
    IUOLFoneClient2 : public IUOLFoneClient
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IUOLFoneClientCallback2 *pCallback2,
            /* [in] */ BSTR bstrCodecsPath,
            /* [in] */ enum EnumOsipDebugLevel lLogLevel) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUOLFoneClient2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUOLFoneClient2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUOLFoneClient2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUOLFoneClient2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IUOLFoneClient2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IUOLFoneClient2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IUOLFoneClient2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IUOLFoneClient2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientCallback *pCallback,
            /* [in] */ BSTR bstrCodecsPath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Finalize )( 
            IUOLFoneClient2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *TestConnection )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientConnectionTestParam *pConnectionTestParam,
            /* [out] */ IUOLFoneClientConnectionTestResult **ppConnectionTestResult);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnumerateAudioCodecs )( 
            IUOLFoneClient2 * This,
            /* [out] */ IUOLFoneClientCodecInfoEnum **ppCodecInfoEnum);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnumerateVideoCodecs )( 
            IUOLFoneClient2 * This,
            /* [out] */ IUOLFoneClientCodecInfoEnum **ppCodecInfoEnum);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetAudioCodecsPrecedence )( 
            IUOLFoneClient2 * This,
            /* [out] */ BSTR *pbstrAudioCodecs);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVideoCodecsPrecedence )( 
            IUOLFoneClient2 * This,
            /* [out] */ BSTR *pbstrVideoCodecs);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *InitializePhoneLines )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneLineConfig *pPhoneLineConfig);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FinalizePhoneLines )( 
            IUOLFoneClient2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RegisterPhoneLine )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneLineInfo *pPhoneLineInfo,
            /* [in] */ IUOLFoneClientNetworkConfig *pNetworkConfig,
            /* [out] */ IUOLFoneClientPhoneLine **ppPhoneLine);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *UnregisterPhoneLine )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *MakePhoneCall )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BSTR bstrPhoneURI,
            /* [in] */ BOOL bEnableVideo,
            /* [out] */ IUOLFoneClientPhoneCall **ppPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetMediaNetworkConfig )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientNetworkConfig *pNetworkConfig);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AcceptPhoneCall )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ BOOL bEnableVideo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *RejectPhoneCall )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumFailureCode rejectReason);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ClosePhoneCall )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *HoldPhoneCall )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ResumePhoneCall )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *TransferPhoneCall )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ BSTR bstrPhoneURI);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ReconfigureCallMedia )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ IUOLFoneClientNetworkConfig *pNetworkConfig);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PlayTone )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ enum EnumTone tone);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PlaySoundFile )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [in] */ BSTR bstrSoundFilePath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetAudioCodec )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [out] */ IUOLFoneClientCodecInfo **pCodecInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVideoCodec )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [out] */ IUOLFoneClientCodecInfo **pCodecInfo);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnumerateAudioDevices )( 
            IUOLFoneClient2 * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ IUOLFoneClientDeviceInfoEnum **ppDeviceInfoEnum);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnumerateVideoDevices )( 
            IUOLFoneClient2 * This,
            /* [out] */ IUOLFoneClientDeviceInfoEnum **ppDeviceInfoEnum);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetAudioDevice )( 
            IUOLFoneClient2 * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ BSTR bstrDeviceName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetAudioDevice )( 
            IUOLFoneClient2 * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [out] */ BSTR *pbstrDeviceName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnableAcousticEchoCancellation )( 
            IUOLFoneClient2 * This,
            /* [in] */ BOOL bEnable);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsAcousticEchoCancellationEnabled )( 
            IUOLFoneClient2 * This,
            /* [out] */ BOOL *pbEnabled);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EnableHalfDuplex )( 
            IUOLFoneClient2 * This,
            /* [in] */ BOOL bEnable);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsHalfDuplexEnabled )( 
            IUOLFoneClient2 * This,
            /* [in] */ BOOL *pbEnabled);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetVolumeLevel )( 
            IUOLFoneClient2 * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ LONG lLevel);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVolumeLevel )( 
            IUOLFoneClient2 * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [out] */ LONG *plLevel);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetMute )( 
            IUOLFoneClient2 * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [in] */ BOOL bMute);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetMute )( 
            IUOLFoneClient2 * This,
            /* [in] */ enum EnumAudioDeviceType audioDeviceType,
            /* [out] */ BOOL *pbMute);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetVideoDevice )( 
            IUOLFoneClient2 * This,
            /* [in] */ BSTR bstrDeviceName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVideoDevice )( 
            IUOLFoneClient2 * This,
            /* [out] */ BSTR *pbstrDeviceName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetVideoQuality )( 
            IUOLFoneClient2 * This,
            /* [in] */ LONG lVideoQuality);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetVideoQuality )( 
            IUOLFoneClient2 * This,
            /* [out] */ LONG *plVideoQuality);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SendInstantMessage )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BSTR bstrMessage,
            /* [in] */ BSTR bstrUserURI);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SendRingingNotification )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CreateConferenceCall )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall,
            /* [out] */ IUOLFoneClientConferenceCall **pConferenceCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ConferenceInvite )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall,
            /* [in] */ BSTR bstrPhoneURI,
            /* [in] */ BOOL bEnableVideo,
            /* [out] */ IUOLFoneClientPhoneCall **ppPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ConferenceAddMember )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ConferenceRemoveMember )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall,
            /* [in] */ IUOLFoneClientPhoneCall *pPhoneCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DestroyConferenceCall )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientConferenceCall *pConferenceCall);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetLineCallForwardNumber )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BSTR bstrCallForwardNumber);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetLineBusyFlag )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientPhoneLine *pPhoneLine,
            /* [in] */ BOOL bIsBusy);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IUOLFoneClient2 * This,
            /* [in] */ IUOLFoneClientCallback2 *pCallback2,
            /* [in] */ BSTR bstrCodecsPath,
            /* [in] */ enum EnumOsipDebugLevel lLogLevel);
        
        END_INTERFACE
    } IUOLFoneClient2Vtbl;

    interface IUOLFoneClient2
    {
        CONST_VTBL struct IUOLFoneClient2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUOLFoneClient2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUOLFoneClient2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUOLFoneClient2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUOLFoneClient2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IUOLFoneClient2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IUOLFoneClient2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IUOLFoneClient2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IUOLFoneClient2_Initialize(This,pCallback,bstrCodecsPath)	\
    ( (This)->lpVtbl -> Initialize(This,pCallback,bstrCodecsPath) ) 

#define IUOLFoneClient2_Finalize(This)	\
    ( (This)->lpVtbl -> Finalize(This) ) 

#define IUOLFoneClient2_TestConnection(This,pConnectionTestParam,ppConnectionTestResult)	\
    ( (This)->lpVtbl -> TestConnection(This,pConnectionTestParam,ppConnectionTestResult) ) 

#define IUOLFoneClient2_EnumerateAudioCodecs(This,ppCodecInfoEnum)	\
    ( (This)->lpVtbl -> EnumerateAudioCodecs(This,ppCodecInfoEnum) ) 

#define IUOLFoneClient2_EnumerateVideoCodecs(This,ppCodecInfoEnum)	\
    ( (This)->lpVtbl -> EnumerateVideoCodecs(This,ppCodecInfoEnum) ) 

#define IUOLFoneClient2_GetAudioCodecsPrecedence(This,pbstrAudioCodecs)	\
    ( (This)->lpVtbl -> GetAudioCodecsPrecedence(This,pbstrAudioCodecs) ) 

#define IUOLFoneClient2_GetVideoCodecsPrecedence(This,pbstrVideoCodecs)	\
    ( (This)->lpVtbl -> GetVideoCodecsPrecedence(This,pbstrVideoCodecs) ) 

#define IUOLFoneClient2_InitializePhoneLines(This,pPhoneLineConfig)	\
    ( (This)->lpVtbl -> InitializePhoneLines(This,pPhoneLineConfig) ) 

#define IUOLFoneClient2_FinalizePhoneLines(This)	\
    ( (This)->lpVtbl -> FinalizePhoneLines(This) ) 

#define IUOLFoneClient2_RegisterPhoneLine(This,pPhoneLineInfo,pNetworkConfig,ppPhoneLine)	\
    ( (This)->lpVtbl -> RegisterPhoneLine(This,pPhoneLineInfo,pNetworkConfig,ppPhoneLine) ) 

#define IUOLFoneClient2_UnregisterPhoneLine(This,pPhoneLine)	\
    ( (This)->lpVtbl -> UnregisterPhoneLine(This,pPhoneLine) ) 

#define IUOLFoneClient2_MakePhoneCall(This,pPhoneLine,bstrPhoneURI,bEnableVideo,ppPhoneCall)	\
    ( (This)->lpVtbl -> MakePhoneCall(This,pPhoneLine,bstrPhoneURI,bEnableVideo,ppPhoneCall) ) 

#define IUOLFoneClient2_SetMediaNetworkConfig(This,pNetworkConfig)	\
    ( (This)->lpVtbl -> SetMediaNetworkConfig(This,pNetworkConfig) ) 

#define IUOLFoneClient2_AcceptPhoneCall(This,pPhoneCall,bEnableVideo)	\
    ( (This)->lpVtbl -> AcceptPhoneCall(This,pPhoneCall,bEnableVideo) ) 

#define IUOLFoneClient2_RejectPhoneCall(This,pPhoneCall,rejectReason)	\
    ( (This)->lpVtbl -> RejectPhoneCall(This,pPhoneCall,rejectReason) ) 

#define IUOLFoneClient2_ClosePhoneCall(This,pPhoneCall)	\
    ( (This)->lpVtbl -> ClosePhoneCall(This,pPhoneCall) ) 

#define IUOLFoneClient2_HoldPhoneCall(This,pPhoneCall)	\
    ( (This)->lpVtbl -> HoldPhoneCall(This,pPhoneCall) ) 

#define IUOLFoneClient2_ResumePhoneCall(This,pPhoneCall)	\
    ( (This)->lpVtbl -> ResumePhoneCall(This,pPhoneCall) ) 

#define IUOLFoneClient2_TransferPhoneCall(This,pPhoneCall,bstrPhoneURI)	\
    ( (This)->lpVtbl -> TransferPhoneCall(This,pPhoneCall,bstrPhoneURI) ) 

#define IUOLFoneClient2_ReconfigureCallMedia(This,pPhoneCall,pNetworkConfig)	\
    ( (This)->lpVtbl -> ReconfigureCallMedia(This,pPhoneCall,pNetworkConfig) ) 

#define IUOLFoneClient2_PlayTone(This,pPhoneCall,tone)	\
    ( (This)->lpVtbl -> PlayTone(This,pPhoneCall,tone) ) 

#define IUOLFoneClient2_PlaySoundFile(This,pPhoneCall,bstrSoundFilePath)	\
    ( (This)->lpVtbl -> PlaySoundFile(This,pPhoneCall,bstrSoundFilePath) ) 

#define IUOLFoneClient2_GetAudioCodec(This,pPhoneCall,pCodecInfo)	\
    ( (This)->lpVtbl -> GetAudioCodec(This,pPhoneCall,pCodecInfo) ) 

#define IUOLFoneClient2_GetVideoCodec(This,pPhoneCall,pCodecInfo)	\
    ( (This)->lpVtbl -> GetVideoCodec(This,pPhoneCall,pCodecInfo) ) 

#define IUOLFoneClient2_EnumerateAudioDevices(This,audioDeviceType,ppDeviceInfoEnum)	\
    ( (This)->lpVtbl -> EnumerateAudioDevices(This,audioDeviceType,ppDeviceInfoEnum) ) 

#define IUOLFoneClient2_EnumerateVideoDevices(This,ppDeviceInfoEnum)	\
    ( (This)->lpVtbl -> EnumerateVideoDevices(This,ppDeviceInfoEnum) ) 

#define IUOLFoneClient2_SetAudioDevice(This,audioDeviceType,bstrDeviceName)	\
    ( (This)->lpVtbl -> SetAudioDevice(This,audioDeviceType,bstrDeviceName) ) 

#define IUOLFoneClient2_GetAudioDevice(This,audioDeviceType,pbstrDeviceName)	\
    ( (This)->lpVtbl -> GetAudioDevice(This,audioDeviceType,pbstrDeviceName) ) 

#define IUOLFoneClient2_EnableAcousticEchoCancellation(This,bEnable)	\
    ( (This)->lpVtbl -> EnableAcousticEchoCancellation(This,bEnable) ) 

#define IUOLFoneClient2_IsAcousticEchoCancellationEnabled(This,pbEnabled)	\
    ( (This)->lpVtbl -> IsAcousticEchoCancellationEnabled(This,pbEnabled) ) 

#define IUOLFoneClient2_EnableHalfDuplex(This,bEnable)	\
    ( (This)->lpVtbl -> EnableHalfDuplex(This,bEnable) ) 

#define IUOLFoneClient2_IsHalfDuplexEnabled(This,pbEnabled)	\
    ( (This)->lpVtbl -> IsHalfDuplexEnabled(This,pbEnabled) ) 

#define IUOLFoneClient2_SetVolumeLevel(This,audioDeviceType,lLevel)	\
    ( (This)->lpVtbl -> SetVolumeLevel(This,audioDeviceType,lLevel) ) 

#define IUOLFoneClient2_GetVolumeLevel(This,audioDeviceType,plLevel)	\
    ( (This)->lpVtbl -> GetVolumeLevel(This,audioDeviceType,plLevel) ) 

#define IUOLFoneClient2_SetMute(This,audioDeviceType,bMute)	\
    ( (This)->lpVtbl -> SetMute(This,audioDeviceType,bMute) ) 

#define IUOLFoneClient2_GetMute(This,audioDeviceType,pbMute)	\
    ( (This)->lpVtbl -> GetMute(This,audioDeviceType,pbMute) ) 

#define IUOLFoneClient2_SetVideoDevice(This,bstrDeviceName)	\
    ( (This)->lpVtbl -> SetVideoDevice(This,bstrDeviceName) ) 

#define IUOLFoneClient2_GetVideoDevice(This,pbstrDeviceName)	\
    ( (This)->lpVtbl -> GetVideoDevice(This,pbstrDeviceName) ) 

#define IUOLFoneClient2_SetVideoQuality(This,lVideoQuality)	\
    ( (This)->lpVtbl -> SetVideoQuality(This,lVideoQuality) ) 

#define IUOLFoneClient2_GetVideoQuality(This,plVideoQuality)	\
    ( (This)->lpVtbl -> GetVideoQuality(This,plVideoQuality) ) 

#define IUOLFoneClient2_SendInstantMessage(This,pPhoneLine,bstrMessage,bstrUserURI)	\
    ( (This)->lpVtbl -> SendInstantMessage(This,pPhoneLine,bstrMessage,bstrUserURI) ) 

#define IUOLFoneClient2_SendRingingNotification(This,pPhoneCall)	\
    ( (This)->lpVtbl -> SendRingingNotification(This,pPhoneCall) ) 

#define IUOLFoneClient2_CreateConferenceCall(This,pPhoneCall,pConferenceCall)	\
    ( (This)->lpVtbl -> CreateConferenceCall(This,pPhoneCall,pConferenceCall) ) 

#define IUOLFoneClient2_ConferenceInvite(This,pPhoneLine,pConferenceCall,bstrPhoneURI,bEnableVideo,ppPhoneCall)	\
    ( (This)->lpVtbl -> ConferenceInvite(This,pPhoneLine,pConferenceCall,bstrPhoneURI,bEnableVideo,ppPhoneCall) ) 

#define IUOLFoneClient2_ConferenceAddMember(This,pConferenceCall,pPhoneCall)	\
    ( (This)->lpVtbl -> ConferenceAddMember(This,pConferenceCall,pPhoneCall) ) 

#define IUOLFoneClient2_ConferenceRemoveMember(This,pConferenceCall,pPhoneCall)	\
    ( (This)->lpVtbl -> ConferenceRemoveMember(This,pConferenceCall,pPhoneCall) ) 

#define IUOLFoneClient2_DestroyConferenceCall(This,pConferenceCall)	\
    ( (This)->lpVtbl -> DestroyConferenceCall(This,pConferenceCall) ) 

#define IUOLFoneClient2_SetLineCallForwardNumber(This,pPhoneLine,bstrCallForwardNumber)	\
    ( (This)->lpVtbl -> SetLineCallForwardNumber(This,pPhoneLine,bstrCallForwardNumber) ) 

#define IUOLFoneClient2_SetLineBusyFlag(This,pPhoneLine,bIsBusy)	\
    ( (This)->lpVtbl -> SetLineBusyFlag(This,pPhoneLine,bIsBusy) ) 


#define IUOLFoneClient2_Initialize(This,pCallback2,bstrCodecsPath,lLogLevel)	\
    ( (This)->lpVtbl -> Initialize(This,pCallback2,bstrCodecsPath,lLogLevel) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUOLFoneClient2_INTERFACE_DEFINED__ */



#ifndef __UOLFoneClient_LIBRARY_DEFINED__
#define __UOLFoneClient_LIBRARY_DEFINED__

/* library UOLFoneClient */
/* [helpstring][uuid][version] */ 


EXTERN_C const IID LIBID_UOLFoneClient;

EXTERN_C const CLSID CLSID_CUOLFoneClient;

#ifdef __cplusplus

class DECLSPEC_UUID("03824518-4A74-4594-947C-6C44FE01E133")
CUOLFoneClient;
#endif
#endif /* __UOLFoneClient_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


