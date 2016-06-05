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


#include "stdafx.h"
#include "UOLFoneController.h"

#include "../resource.h"
#include "../FacadeWrapper.h"
#include "../UOLFoneCapabilitiesHandler.h"
#include "com/ConnectionTestParam.h"
#include "com/ComFacade.h"


/// <TEST>
//#define FORCE_G729
/// </TEST>



typedef struct _UOLFONEACCOUNTPARAM
{
	CUOLFoneAccountPtr pAccount;
	BOOL bReleaseMemory;

	_UOLFONEACCOUNTPARAM operator=(const _UOLFONEACCOUNTPARAM& ufap)
	{
		pAccount = ufap.pAccount;
		bReleaseMemory = ufap.bReleaseMemory;

		return *this;
	}

} UOLFONEACCOUNTPARAM, * LPUOLFONEACCOUNTPARAM;


typedef struct _CALLPHONENUMBERPARAM
{
	CString strPhoneNumber;

	_CALLPHONENUMBERPARAM operator=(const _CALLPHONENUMBERPARAM& cpnp)
	{
		strPhoneNumber = cpnp.strPhoneNumber;

		return *this;
	}

} CALLPHONENUMBERPARAM, * LPCALLPHONENUMBERPARAM;


typedef struct _CALLCONTACTPARAM
{
	IUOLMessengerContactPtr pContact;

	_CALLCONTACTPARAM operator=(const _CALLCONTACTPARAM& ccp)
	{
		pContact = ccp.pContact;

		return *this;
	}

} CALLCONTACTPARAM, * LPCALLCONTACTPARAM;


typedef struct _CONFERENCECALLNUMBERSPARAM
{
	CString strPhoneNumber01;
	CString strPhoneNumber02;
	
	_CONFERENCECALLNUMBERSPARAM operator=(const _CONFERENCECALLNUMBERSPARAM& cpnp)
	{
		strPhoneNumber01 = cpnp.strPhoneNumber01;
		strPhoneNumber02 = cpnp.strPhoneNumber02;
		
		return *this;
	}
	
} CONFERENCECALLNUMBERSPARAM, * LPCONFERENCECALLNUMBERSPARAM;



CUOLFoneController::~CUOLFoneController()
{
	ATLASSERT(m_nInternalStatus == InternalStatusUnitialized);
	ATLASSERT(m_pControllerCallback == NULL);
	ATLASSERT(m_pCallback == NULL);
	ATLASSERT(m_pDownloadCallback == NULL);
	ATLASSERT(m_pCodecsConfigFileUpdaterCallback == NULL);
	ATLASSERT(m_pCodecsUpdaterCallback == NULL);
	ATLASSERT(m_pPhoneLineConfig == NULL);
	ATLASSERT(m_pPhoneLineInfo == NULL);
	ATLASSERT(m_pUserInfo == NULL);
	ATLASSERT(m_pNetworkConfig == NULL);
	ATLASSERT(m_pMediaNetworkConfig == NULL);
}


CUOLFoneController* CUOLFoneController::GetInstance()
{
	static CUOLFoneController instance;

	return &instance;
}


BOOL CUOLFoneController::Initialize(const CString& strLibDir, IUOLFoneControllerCallback* pControllerCallback)
{
	/// Initialization step 0...

	BOOL bRet = FALSE;

	SetInternalStatus(InternalStatusInitializing);

	m_strUIMLibDir = strLibDir;

	ATLASSERT(!m_strUIMLibDir.IsEmpty());

	if (pControllerCallback != NULL)
	{
		m_pControllerCallback = pControllerCallback;

		/// 0.1 Create UOLFoneController window

		HWND hWnd = Create(NULL, NULL, "UOLFoneController", WS_POPUP);

		if (hWnd != NULL)
		{
			m_bCheckCodecUpdates = TRUE;

			/// 0.2 Initialize UOLFoneAccountManager...
			m_pUOLFoneAccountManager = new CUOLFoneAccountManager();
			m_pUOLFoneAccountManager->Initialize();
			m_pUOLFoneAccountManager->RegisterObserver(this);

			IUOLMessengerAccountPtr pIMAccount;
			CString strAccount = CUOLFonePluginConfig::GetInstance()->GetCallAccount();
			
			if (FALSE == strAccount.IsEmpty())
			{
				pIMAccount = m_pUOLFoneAccountManager->GetIMAccount(strAccount);
			}
			
			if (!pIMAccount)
			{
				CAtlList<IUOLMessengerAccountPtr> listIMAccounts;

				m_pUOLFoneAccountManager->GetIMAccountList(listIMAccounts);

				if (listIMAccounts.GetCount() == 1)
				{
					pIMAccount = listIMAccounts.GetHead();

					CUOLFonePluginConfig::GetInstance()->SetCallAccount(pIMAccount->GetUserId());
				}
			}
			
			if (pIMAccount)
			{
				m_pUOLFoneAccountManager->SetCurrentAccount(pIMAccount);
			}

			m_pConnectionPendingAccount = CUOLFoneAccountPtr();
			m_pDisconnectionPendingAccount = CUOLFoneAccountPtr();

			/// 0.3 Prepare codecs directory
			CString strCodecsDir = GetCodecsPath();

			if (FALSE == ATLPath::FileExists(strCodecsDir))
			{
				::CreateDirectory(strCodecsDir, NULL);
			}

			/// 0.4 Start libraries download...
			CString strDllsUrl;
			strDllsUrl.LoadString(IDS_URL_RELEASE_DLLS);

			CString strDirNameWithBackSlash(m_strUIMLibDir);
			if (strDirNameWithBackSlash.Right(1) != '\\')
			{
				strDirNameWithBackSlash += '\\';
			}

			if (FALSE == ATLPath::FileExists(m_strUIMLibDir))
			{
				::CreateDirectory(m_strUIMLibDir, NULL);
			}

			m_VerifyDllsThread.SetCallback(this);

			CString strFile;
			CString strDllFilePath;
			CString strFileUrl;

			//UOLFoneClient
			strFile = GetUOLFoneClientDllName();
			strDllFilePath = strDirNameWithBackSlash + strFile;
			strFileUrl = strDllsUrl + strFile;
			m_VerifyDllsThread.AddItem(strFileUrl, strDllFilePath);

			//UOLFoneCrypt
			const int LOCAL_PATH = TRUE;
			strDllFilePath = strDirNameWithBackSlash + GetUOLFoneCryptDllName(LOCAL_PATH);
			strFileUrl = strDllsUrl + GetUOLFoneCryptDllName();
			m_VerifyDllsThread.AddItem(strFileUrl, strDllFilePath);

			//UOLFoneServerConfigProvider
			strFile = GetUOLFoneServerConfigProviderDllName();
			strDllFilePath = strDirNameWithBackSlash + strFile;
			strFileUrl = strDllsUrl + strFile;
			m_VerifyDllsThread.AddItem(strFileUrl, strDllFilePath);
		
			SetInternalStatus(GetInternalStatus() | InternalStatusDownloadingLibraries);
			
			m_VerifyDllsThread.Start();

			bRet = TRUE;
		}
	}

	return bRet;
}


void CUOLFoneController::Finalize()
{
	SetInternalStatus(InternalStatusFinalizing);

	m_pControllerCallback = NULL;

	if (m_VerifyDllsThread.IsAlive())
	{
		m_VerifyDllsThread.CancelVerification();
		m_VerifyDllsThread.MessageLoopWait();
	}

	// Clear UOLFone capabilities clear
	CUOLFoneAccountPtr pAccount = GetCurrentAccount();

	if (pAccount)
	{
		CUOLFoneCapabilitiesHandler capabilitiesHandler;
		capabilitiesHandler.RevokeCapabilities(pAccount->GetIMAccount());
		m_bCapabilitiesPublished = FALSE;
	}

	if (m_pUOLFoneAccountManager)
	{
		m_pUOLFoneAccountManager->UnregisterObserver(this);
		m_pUOLFoneAccountManager->Finalize();
		m_pUOLFoneAccountManager = NULL;
	}

	if (IsWindow())
	{
		DestroyWindow();
	}

	SetInternalStatus(InternalStatusUnitialized);
}


CUOLFoneAccountManagerPtr CUOLFoneController::GetUOLFoneAccountManager()
{
	return m_pUOLFoneAccountManager;
}


void CUOLFoneController::GetOutputDevices(CAtlList<CString>& listDevices)
{
	GetDevices(UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT, listDevices);
}


void CUOLFoneController::GetInputDevices(CAtlList<CString>& listDevices)
{
	GetDevices(UFC_AUDIO_DEVICE_TYPE_CALL_INPUT, listDevices);
}


CString CUOLFoneController::GetCurrentOutputDevice() const
{
	if (m_spUOLFoneClient)
	{
		CComBSTR bstrCurrentAudioDevice;

		HRESULT hr = m_spUOLFoneClient->GetAudioDevice(UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT, &bstrCurrentAudioDevice);

		if (SUCCEEDED(hr))
		{		
			CString strCurrentAudioDevice(bstrCurrentAudioDevice);
			return strCurrentAudioDevice;
		}
	}

	return _T("");
}


void CUOLFoneController::SetCurrentOutputDevice(const CString& strOutputDevice)
{
	if (m_spUOLFoneClient)
	{
		HRESULT hr = m_spUOLFoneClient->SetAudioDevice(UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT, strOutputDevice.AllocSysString());
		ATLASSERT(SUCCEEDED(hr));
	}
}


LONG CUOLFoneController::GetCurrentOutputDeviceVolume()
{
	LONG lLevel = 0;

	if (m_spUOLFoneClient)
	{
		HRESULT hr = m_spUOLFoneClient->GetVolumeLevel(UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT, &lLevel); 
		ATLASSERT(SUCCEEDED(hr));
	}

	return lLevel;
}


BOOL CUOLFoneController::GetCurrentOutputDeviceMute() const
{
	BOOL bMute = FALSE;

	if (m_spUOLFoneClient)
	{
		HRESULT hr = m_spUOLFoneClient->GetMute(UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT, &bMute); 
		ATLASSERT(SUCCEEDED(hr));
	}

	return bMute;
}


void CUOLFoneController::SetCurrentOutputDeviceVolume(LONG lLevel)
{
	if (m_spUOLFoneClient)
	{
		HRESULT hr = m_spUOLFoneClient->SetVolumeLevel(UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT, lLevel);
		ATLASSERT(SUCCEEDED(hr));
	}
}


void CUOLFoneController::SetCurrentOutputDeviceMute(BOOL bMute)
{
	if (m_spUOLFoneClient)
	{
		HRESULT hr = m_spUOLFoneClient->SetMute(UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT, bMute);
		ATLASSERT(SUCCEEDED(hr));
	}
}


CString CUOLFoneController::GetCurrentInputDevice() const
{
	if (m_spUOLFoneClient)
	{
		CComBSTR bstrCurrentAudioDevice;

		HRESULT hr = m_spUOLFoneClient->GetAudioDevice(UFC_AUDIO_DEVICE_TYPE_CALL_INPUT, &bstrCurrentAudioDevice);

		if (SUCCEEDED(hr))
		{		
			CString strCurrentAudioDevice(bstrCurrentAudioDevice);
			return strCurrentAudioDevice;
		}
	}

	return _T("");
}


void CUOLFoneController::SetCurrentInputDevice(const CString& strInputDevice)
{
	if (m_spUOLFoneClient)
	{
		HRESULT hr = m_spUOLFoneClient->SetAudioDevice(UFC_AUDIO_DEVICE_TYPE_CALL_INPUT, strInputDevice.AllocSysString());
		ATLASSERT(SUCCEEDED(hr));
	}
}


LONG CUOLFoneController::GetCurrentInputDeviceVolume()
{
	LONG lLevel = 0;

	if (m_spUOLFoneClient)
	{
		HRESULT hr = m_spUOLFoneClient->GetVolumeLevel(UFC_AUDIO_DEVICE_TYPE_CALL_INPUT, &lLevel); 
		ATLASSERT(SUCCEEDED(hr));
	}

	return lLevel;
}


BOOL CUOLFoneController::GetCurrentInputDeviceMute() const
{
	BOOL bMute = FALSE;

	if (m_spUOLFoneClient)
	{
		HRESULT hr = m_spUOLFoneClient->GetMute(UFC_AUDIO_DEVICE_TYPE_CALL_INPUT, &bMute); 
		ATLASSERT(SUCCEEDED(hr));
	}

	return bMute;
}


void CUOLFoneController::SetCurrentInputDeviceVolume(LONG lLevel)
{
	if (m_spUOLFoneClient)
	{
		HRESULT hr = m_spUOLFoneClient->SetVolumeLevel(UFC_AUDIO_DEVICE_TYPE_CALL_INPUT, lLevel);
		ATLASSERT(SUCCEEDED(hr));
	}
}


void CUOLFoneController::SetCurrentInputDeviceMute(BOOL bMute)
{
	if (m_spUOLFoneClient)
	{
		HRESULT hr = m_spUOLFoneClient->SetMute(UFC_AUDIO_DEVICE_TYPE_CALL_INPUT, bMute);
		ATLASSERT(SUCCEEDED(hr));
	}
}



BOOL CUOLFoneController::Connect()
{
	return Connect(GetCurrentAccount());
}


BOOL CUOLFoneController::Disconnect()
{
	return Disconnect(GetCurrentAccount());
}


BOOL CUOLFoneController::IsConnected()
{
	return IsConnected(GetCurrentAccount());
}



BOOL CUOLFoneController::IsValidPhoneNumber(const CString& strPhoneNumber)
{
	return (BOOL) SendMessage(WM_USER_CHECK_PHONE_NUMBER, (WPARAM) (LPCTSTR) strPhoneNumber);
}


BOOL CUOLFoneController::MakePhoneCall(IUOLMessengerContactPtr pContact)
{
	if (FALSE == IsCallInProgress())
	{
		LPCALLCONTACTPARAM pCcp = new CALLCONTACTPARAM();

		if (pCcp)
		{
			pCcp->pContact = pContact;

			return SendMessage(WM_USER_CALL_CONTACT, (WPARAM) pCcp);
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
					CUOLFoneFailureInfo::CALL_MESSAGE_ALLOC_FAILURE, 
					CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
		}
	}

	return FALSE;
}


BOOL CUOLFoneController::MakePhoneCall(const CString& strPhoneNumber)
{
	if (FALSE == IsCallInProgress())
	{
		LPCALLPHONENUMBERPARAM pCpnp = new CALLPHONENUMBERPARAM();

		if (pCpnp)
		{
			pCpnp->strPhoneNumber = strPhoneNumber;

			return SendMessage(WM_USER_CALL_PHONE_NUMBER, (WPARAM) pCpnp);
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
					CUOLFoneFailureInfo::CALL_MESSAGE_ALLOC_FAILURE, 
					CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
		}
	}

	return FALSE;
}


BOOL CUOLFoneController::HangUpPhoneCall()
{
	CUOLFoneAccountPtr pAccount = GetCurrentAccount();

	if (pAccount)
	{
		UOLFONEACCOUNTPARAM ufap = {0};
		ufap.pAccount = pAccount;
		ufap.bReleaseMemory = FALSE;

		return SendMessage(WM_USER_HANG_UP_CALL, (WPARAM) &ufap);
	}

	return FALSE;
}


BOOL CUOLFoneController::TransferPhoneCall(CString& strPhoneNumber)
{
	if (IsCallInProgress())
	{
		LPCALLPHONENUMBERPARAM pCpnp = new CALLPHONENUMBERPARAM();
		
		if (pCpnp)
		{
			pCpnp->strPhoneNumber = strPhoneNumber;
			
			return SendMessage(WM_USER_TRANSFER_CALL, (WPARAM) pCpnp);
		}
	}
	
	return FALSE;
}


BOOL CUOLFoneController::HoldPhoneCall()
{
	/*
	// TODO: Implement IsCallHeld() method on a trustworthy way.
	if ( (IsCallInProgress()) && (!IsCallHeld()) )
	{
		CUOLFoneAccountPtr pAccount = GetCurrentAccount();
		
		if (pAccount)
		{
			UOLFONEACCOUNTPARAM ufap = {0};
			ufap.pAccount = pAccount;
			ufap.bReleaseMemory = FALSE;
			
			return SendMessage(WM_USER_HOLD_UP_CALL, (WPARAM) &ufap);
		}
	}
	*/
	
	return FALSE;
}


BOOL CUOLFoneController::ResumePhoneCall()
{
	/*
	// TODO: Implement IsCallHeld() method on a trustworthy way.
	if ( (IsCallInProgress()) && (IsCallHeld()) )
	{
		CUOLFoneAccountPtr pAccount = GetCurrentAccount();
		
		if (pAccount)
		{
			UOLFONEACCOUNTPARAM ufap = {0};
			ufap.pAccount = pAccount;
			ufap.bReleaseMemory = FALSE;
			
			return SendMessage(WM_USER_RESUME_UP_CALL, (WPARAM) &ufap);
		}
	}
	*/
	
	return FALSE;
}


BOOL CUOLFoneController::MakeConferenceCall(CString& strPhoneNumber01, CString& strPhoneNumber02)
{
	/*
	// TODO: Implement IsConferenceInProgress() method on a trustworthy way.
	if (!IsConferenceInProgress())
	{
		LPCONFERENCECALLNUMBERSPARAM pCcnp = new CONFERENCECALLNUMBERSPARAM();
		
		if (pCcnp)
		{
			pCcnp->strPhoneNumber01 = strPhoneNumber01;
			pCcnp->strPhoneNumber02 = strPhoneNumber02;
			
			return SendMessage(WM_USER_MAKE_CONFERENCE_CALL, (WPARAM) pCcnp);
		}
	}
	*/
	
	return FALSE;
}


BOOL CUOLFoneController::HangUpConferenceCall()
{
	/*
	// TODO: Implement IsConferenceInProgress() method on a trustworthy way.
	if (IsConferenceInProgress())
	{
		CUOLFoneAccountPtr pAccount = GetCurrentAccount();
		
		if (pAccount)
		{
			UOLFONEACCOUNTPARAM ufap = {0};
			ufap.pAccount = pAccount;
			ufap.bReleaseMemory = FALSE;
			
			return SendMessage(WM_USER_HANG_UP_CONFERENCE_CALL, (WPARAM) &ufap);
		}
	}
	*/
	
	return FALSE;
}


BOOL CUOLFoneController::AcceptIncomingCall()
{
	CUOLFoneAccountPtr pAccount = GetCurrentAccount();

	if (pAccount)
	{
		UOLFONEACCOUNTPARAM ufap = {0};
		ufap.pAccount = pAccount;
		ufap.bReleaseMemory = FALSE;
	
		return SendMessage(WM_USER_ACCEPT_INCOMING_CALL, (WPARAM) &ufap);
	}

	return FALSE;
}


BOOL CUOLFoneController::RejectIncomingCall()
{
	CUOLFoneAccountPtr pAccount = GetCurrentAccount();

	if (pAccount)
	{
		UOLFONEACCOUNTPARAM ufap = {0};
		ufap.pAccount = pAccount;
		ufap.bReleaseMemory = FALSE;

		return SendMessage(WM_USER_REJECT_INCOMING_CALL, (WPARAM) &ufap);
	}

	return FALSE;
}


BOOL CUOLFoneController::IsCallInProgress()
{
	CPhoneCallPtr pPhoneCall;

	CUOLFoneAccountPtr pAccount = GetCurrentAccount();

	return IsCallInProgress(pAccount);
}


BOOL CUOLFoneController::PlayTone(UINT nChar)
{
	BOOL resultOk = FALSE;
	
	if ( (m_spUOLFoneClient) && (m_spPhoneCall) )
	{
		if ( ('*' == nChar) ||
				('#' == nChar) ||
				(::_istdigit(nChar)) )
		{
			int selectedTone = 0;
			
			if ('*' == nChar)
			{
				selectedTone = UFC_TONE_STAR;
			}
			else if ('#' == nChar)
			{
				selectedTone = UFC_TONE_POUND;
			}
			else 
			{
				// ID's from UFC_TONE_0 to UFC_TONE9 is "0" to "9", 
				// so...
				selectedTone = UFC_TONE_0 + (nChar - 0x30);
			}
			
			HRESULT hr = m_spUOLFoneClient->PlayTone(m_spPhoneCall, (EnumTone)selectedTone);
			
			if (SUCCEEDED(hr))
			{
				resultOk = TRUE;
			}
		}
	}
	
	return resultOk;
}







/////////////////////////////////////////////////
// protected functions
/////////////////////////////////////////////////


void CUOLFoneController::PhoneCallStateChanged(IUOLFoneClientPhoneCall* pPhoneCall, EnumPhoneCallState phoneCallState, IUOLFoneClientExtraResponseData2* pExtraResponseData)
{
	ATLTRACE(_T("%s - pPhoneCall [0x%x], phoneCallState [%d]\n"), __FUNCTION__, pPhoneCall, phoneCallState);

	CUOLFoneAccountPtr pAccount = GetCurrentAccount();
	CPhoneCallPtr pCurrentPhoneCall;

	LONG lPhoneCallId = 0L;
	pPhoneCall->GetPhoneCallId(&lPhoneCallId);

	if (pAccount)
	{
		if (phoneCallState == UFC_PHONE_CALL_STATE_INCOMING)
		{
			ATLTRACE(_T("%s - Incoming call received - check if user is busy...\n"), __FUNCTION__);

			if (FALSE == IsCallInProgress())
			{
				// User is not busy...

				CComBSTR bstrURI;

				HRESULT hr = pPhoneCall->GetPhoneURI(&bstrURI);

				CString strRemoteUri(bstrURI);

				ATLASSERT(FALSE == strRemoteUri.IsEmpty());

				if (FALSE == strRemoteUri.IsEmpty())
				{
					CCallContactParser callContactParser;

					if (callContactParser.ParseAddress(CString(strRemoteUri)))
					{
						// PC2PC call...
						IUOLMessengerContactPtr pContact = CFacadeWrapper::GetInstance()->FindContact(pAccount->GetIMAccount(), callContactParser.GetUsername());

						if (pContact)
						{
							CFacadeWrapper::GetInstance()->SetVoIPCallInfo( pContact->GetAlias(), 
																			pContact->GetUserIcon()->GetImage() );
							pCurrentPhoneCall = new CPhoneCall();
						}
						else
						{
							// Phone2PC call...
							CFacadeWrapper* pFacadeWrapper (CFacadeWrapper::GetInstance());
							CFacadeWrapper::GetInstance()->SetVoIPCallInfo( FormatDisplayName(callContactParser), 
																			pFacadeWrapper->IsUOLMessengerAppMode() ? IDB_PC2PHONE_IMAGE : IDB_PC2PHONE_IMAGE_ALT);
							pCurrentPhoneCall = new CPhoneCall();
						}
						
						pCurrentPhoneCall->SetPhoneCallId(lPhoneCallId);
					}
				}
			}

			if (pCurrentPhoneCall)
			{
				ATLTRACE(_T("%s - Notifying incoming call...\n"), __FUNCTION__);

				m_spPhoneCall = pPhoneCall;

				pAccount->SetCurrentPhoneCall(pCurrentPhoneCall);

				ATLASSERT(m_spUOLFoneClient);

				HRESULT hr = m_spUOLFoneClient->SendRingingNotification(pPhoneCall);
				m_nPhoneRingingCount = 1;

				ATLASSERT(SUCCEEDED(hr));

				m_nTimerSendRingingNotification = SetTimer(TIMER_SEND_RINGING_NOTIFICATION, 4 * 1000, NULL);
			}
			else
			{
				ATLTRACE(_T("%s - Rejecting incoming call...\n"), __FUNCTION__);

				// User is busy or error...
				ATLASSERT(m_spUOLFoneClient);
				
				///////////////////////////////
				CComBSTR bstrURI;

				HRESULT hr = pPhoneCall->GetPhoneURI(&bstrURI);

				CString strRemoteUri(bstrURI);

				ATLASSERT(FALSE == strRemoteUri.IsEmpty());

				if (FALSE == strRemoteUri.IsEmpty())
				{
					CCallContactParser callContactParser;

					if (callContactParser.ParseAddress(CString(strRemoteUri)))
					{
						// PC2PC call...
						IUOLMessengerContactPtr pContact = CFacadeWrapper::GetInstance()->FindContact(pAccount->GetIMAccount(), callContactParser.GetUsername());

						if (pContact)
						{
							CFacadeWrapper::GetInstance()->LogBusyCall(pContact);
						}
						else
						{
							// Phone2PC call...
							CFacadeWrapper::GetInstance()->LogBusyCall(FormatDisplayName(callContactParser));
						}
					}
				}
				////////////////////////////////////
				
				

				HRESULT hr2 = m_spUOLFoneClient->RejectPhoneCall(pPhoneCall, UFC_FAILURE_CODE_BUSY);

				ATLASSERT(SUCCEEDED(hr2));
				

				return;
			}
		}	// if (phoneCallState == UFC_PHONE_CALL_STATE_INCOMING)
		else
		{
			pCurrentPhoneCall = pAccount->GetCurrentPhoneCall();
		}

		if ( pCurrentPhoneCall && 
			 pCurrentPhoneCall->GetPhoneCallId() == lPhoneCallId )
		{
			ATLTRACE(_T("%s - Notifying call state change...\n"), __FUNCTION__);

			CPhoneCall::PhoneCallState mappedState = m_phoneCallStateChangeHandler.MapPhoneCallState(phoneCallState);

			pCurrentPhoneCall->SetPhoneCallState(mappedState);

			if ( (phoneCallState == UFC_PHONE_CALL_STATE_ERROR) && 
					(pExtraResponseData) )
			{
				// It should already been saved.
				//
				m_pLastFailureInfo.reset();
				
				// Retrieve additional info...
				//
				ExtractAdditionalErrorInfo(pExtraResponseData);
				
				if (m_pLastFailureInfo)
				{
					NotifyPhoneCallErrorReceived(pCurrentPhoneCall);
				}
			}
			
			NotifyPhoneCallStateChanged(pCurrentPhoneCall);
		}
	}

	if ((phoneCallState == UFC_PHONE_CALL_STATE_CLOSED) ||
		(phoneCallState == UFC_PHONE_CALL_STATE_ERROR) || 
		(phoneCallState == UFC_PHONE_CALL_STATE_BUSY) ||
		(phoneCallState == UFC_PHONE_CALL_STATE_NO_ANSWER))
	{
		ATLTRACE(_T("%s - Call closed/error...\n"), __FUNCTION__);

		if ( pCurrentPhoneCall && 
			 pCurrentPhoneCall->GetPhoneCallId() == lPhoneCallId )
		{
			if (pAccount)
			{
				pAccount->SetCurrentPhoneCall(CPhoneCallPtr());
			}

			m_spPhoneCall.Release();
		}

		if (m_pDisconnectionPendingAccount)
		{
			ATLTRACE(_T("%s - Disconnecting account...\n"), __FUNCTION__);

			CUOLFoneAccountPtr pDisconnectAccount = m_pDisconnectionPendingAccount;
			m_pDisconnectionPendingAccount = CUOLFoneAccountPtr();

			BOOL bDisconnect = Disconnect(pDisconnectAccount);
			ATLASSERT(bDisconnect);
		}
		
		if ( (CUOLFonePluginConfig::GetInstance()->RegisterOnlyOnCall()) || 
				( (GetCurrentAccount()) && (GetCurrentAccount()->GetIMAccount()) && 
				(!m_pUOLFoneAccountManager->IsIMAccountConnected(GetCurrentAccount()->GetIMAccount())) ) )
		{
			BOOL bDisconnect = Disconnect();
			ATLASSERT(bDisconnect);
		}
	}
}


void CUOLFoneController::PhoneLineStateChanged(IUOLFoneClientPhoneLine* pPhoneLine, EnumPhoneLineState phoneLineState, IUOLFoneClientExtraResponseData2* pExtraResponseData)
{
	ATLTRACE(_T("%s - pPhoneLine [0x%x], phoneLineState [%d]\n"), __FUNCTION__, pPhoneLine, phoneLineState);

	CUOLFoneAccountPtr pCurrentAccount = GetCurrentAccount();

	if (!pCurrentAccount)
	{
		return;
	}

	// Check if phone line is valid...
	LONG lEventPhoneLineId = CUOLFoneAccount::UNKNOWN_PHONE_LINE_ID;

	HRESULT hr = pPhoneLine->GetPhoneLineId(&lEventPhoneLineId);

	if (FAILED(hr))
	{
		return;
	}

	CUOLFoneAccount::ConnectionState connectionState = m_accountStateChangeHandler.MapAccountState(phoneLineState);

	if (connectionState != CUOLFoneAccount::ConnectionStateOnline)
	{
		ATLTRACE(_T("%s - Account is not online...\n"), __FUNCTION__);

		// Phone line unregistered...
		LONG lCurrentPhoneLineId = CUOLFoneAccount::UNKNOWN_PHONE_LINE_ID;

		// Release current IUOLFoneClientPhoneLine
		if (m_spPhoneLine)
		{
			hr = m_spPhoneLine->GetPhoneLineId(&lCurrentPhoneLineId);

			ATLASSERT(lCurrentPhoneLineId == lEventPhoneLineId);
			if (lCurrentPhoneLineId == lEventPhoneLineId)
			{
				ATLTRACE(_T("%s - Releasing m_spPhoneLine...\n"), __FUNCTION__);
				m_spPhoneLine.Release();
			}
		}

		// Check if has any account connection pending
		if (m_pConnectionPendingAccount)
		{
			ATLTRACE(_T("%s - Connecting pending account...\n"), __FUNCTION__);

			LPUOLFONEACCOUNTPARAM pUfap = new UOLFONEACCOUNTPARAM();
			pUfap->pAccount = m_pConnectionPendingAccount;
			pUfap->bReleaseMemory = TRUE;

			m_pConnectionPendingAccount = CUOLFoneAccountPtr();

			PostMessage(WM_USER_CONNECT_ACCOUNT, (WPARAM) pUfap);
		}
		else
		{
			ATLTRACE(_T("%s - Notifying account state change...\n"), __FUNCTION__);

			pCurrentAccount->SetConnectionState(connectionState);
			NotifyAccountConnectionStateChanged(pCurrentAccount);
		}

		m_pDisconnectionPendingAccount = CUOLFoneAccountPtr();
	}
	else
	{
		m_pDisconnectionPendingAccount = CUOLFoneAccountPtr();

		// Phone line registered...
		if (pCurrentAccount->GetConnectionState() != connectionState)
		{
			ATLTRACE(_T("%s - Account is online...\n"), __FUNCTION__);

			// Update account connection state...
			pCurrentAccount->SetConnectionState(connectionState);

			// Get account available inscriptions
			FillAccountInscriptions(pCurrentAccount, pExtraResponseData);

			if (FALSE == m_bCheckCodecUpdates)
			{
				ATLTRACE(_T("%s - No updates checks, notifying account state change...\n"), __FUNCTION__);

				NotifyPhoneLineRegistered(pCurrentAccount);
				NotifyAccountConnectionStateChanged(pCurrentAccount);
			}
			else
			{
				ATLTRACE(_T("%s - Scheduling codecs updates check task...\n"), __FUNCTION__);

				// Update audio/video codecs...
				m_bCheckCodecUpdates = FALSE;
				ScheduleTask(WM_USER_CHECK_CODECS_UPDATES_TASK);
			}
			
			// Reset the pending call whether it have a value.
			//
			if (!m_pPendingCallNumber.IsEmpty())
			{
				CString strPendingCallNumber = m_pPendingCallNumber;
				m_pPendingCallNumber.Empty();
				
				MakePhoneCallImpl(strPendingCallNumber);
			}
			
			if (m_pPendingCallContact != NULL)
			{
				IUOLMessengerContactPtr pContact = m_pPendingCallContact;
				m_pPendingCallContact = IUOLMessengerContactPtr();
				
				MakePhoneCallImpl(pContact);
			}
		}
	}
}


void CUOLFoneController::MessageEvent(IUOLFoneClientMessageEvent* pMessageEvent, IUOLFoneClientExtraResponseData2* pExtraResponseData)
{
	ATLTRACE(_T("%s - pMessageEvent [0x%x]\n"), __FUNCTION__, pMessageEvent);

	CUOLFoneAccountPtr pAccount = GetCurrentAccount();

	// Don't process Message Event if there is no current account, account is offline or unknown
	if ((!pAccount) || 
		(pAccount->GetConnectionState() == CUOLFoneAccount::ConnectionStateOffline) ||
		(pAccount->GetConnectionState() == CUOLFoneAccount::ConnectionStateUnknown))
	{
		return;
	}

	LONG lEventType = 0;
	HRESULT hr = pMessageEvent->GetEventType(&lEventType);

	if (SUCCEEDED(hr))
	{
		if (lEventType == UFC_MESSAGE_EVENT_TYPE_NEW)
		{
			CComBSTR bstrContentType;
			CComBSTR bstrContent;
			CComBSTR bstrFrom;

			hr = pMessageEvent->GetContent(&bstrContent);

			CServerMessageHandler::MessageType messageType = m_serverMessageHandler.HandleMessage(pAccount, bstrContent);

			if (messageType != CServerMessageHandler::MessageTypeUnknown)
			{
				if (messageType & CServerMessageHandler::MessageTypePhoneCallInfo)
				{
					CPhoneCallPtr pPhoneCall = pAccount->GetCurrentPhoneCall();

					if (pPhoneCall)
					{
						NotifyPhoneCallInfoReceived(pPhoneCall);

					}
				}
				
				if (messageType & CServerMessageHandler::MessageTypeError)
				{
					CPhoneCallPtr pPhoneCall = pAccount->GetCurrentPhoneCall();

					if (pPhoneCall)
					{
						NotifyPhoneCallServerErrorReceived(pPhoneCall);
					}
					else
					{
						NotifyAccountErrorReceived(pAccount);
					}
				}
				
				if (messageType & CServerMessageHandler::MessageTypeAccountInfo)
				{
					NotifyAccountInfoReceived(pAccount);
				}
			}
			else
			{
				// Unknown message type
				hr = pMessageEvent->GetContentType(&bstrContentType);
				hr = pMessageEvent->GetFrom(&bstrFrom);

				ATLTRACE(_T("Message from = %s, type = %s, content = %s\n"), CString(bstrFrom), CString(bstrContentType), CString(bstrContent));
			}
		}
	}
}


void CUOLFoneController::PhoneCallStreamError(IUOLFoneClientPhoneCall* pPhoneCall, EnumPhoneCallStreamError phoneCallStreamError)
{
	ATLTRACE(_T("%s - pPhoneCall [0x%x], phoneCallStreamError [%d]\n"), __FUNCTION__, pPhoneCall, phoneCallStreamError);
}


void CUOLFoneController::PhoneLineConnectionTimeout()
{
	ATLTRACE(_T("%s - m_spPhoneLine [0x%x]\n"), __FUNCTION__, m_spPhoneLine);

	if (m_spUOLFoneClient && m_spPhoneLine)
	{
		HangUpPhoneCall();
		
		LONG lPhoneLineState = UFC_PHONE_LINE_STATE_UNKNOWN;
		HRESULT hr = m_spPhoneLine->GetPhoneLineState(&lPhoneLineState);

		CString strDebugMsg;
		strDebugMsg.Format(_T("%s - m_spPhoneLine [0x%x], lPhoneLineState = %d\n"), __FUNCTION__, m_spPhoneLine, lPhoneLineState);

		::OutputDebugString(strDebugMsg);

		if (SUCCEEDED(hr) && lPhoneLineState == UFC_PHONE_LINE_STATE_REGISTERED)
		{
			hr = m_spUOLFoneClient->UnregisterPhoneLine(m_spPhoneLine);
		}

		m_spPhoneLine.Release();

		CUOLFoneAccountPtr pCurrentAccount = GetCurrentAccount();

		if (pCurrentAccount)
		{
			CUOLFoneAccount::ConnectionState state = pCurrentAccount->GetConnectionState();

			if ((state == CUOLFoneAccount::ConnectionStateOnline) ||
				(state == CUOLFoneAccount::ConnectionStateConnectionInProgress) ||
				(state == CUOLFoneAccount::ConnectionStateDisconnectionInProgress))
			{
				pCurrentAccount->SetConnectionState(CUOLFoneAccount::ConnectionStateErrorTimeout);
				NotifyAccountConnectionStateChanged(pCurrentAccount);
			}
		}
		
		// Reset the pending call whether it have a value.
		//
		m_pPendingCallNumber.Empty();
	}
}


void CUOLFoneController::OsipDebugEvent(IUOLFoneClientOsipDebugInfo* pOsipDebugInfo)
{
	HRESULT hResult = E_FAIL;
	CComBSTR bstrFileName;
	LONG lLineNumber;
	LONG lLogLevel;
	CComBSTR bstrLogMessage;
	
	hResult = pOsipDebugInfo->GetFileName(&bstrFileName);
	
	if (SUCCEEDED(hResult))
	{
		hResult = pOsipDebugInfo->GetLineNumber(&lLineNumber);
	}
	
	if (SUCCEEDED(hResult))
	{
		hResult = pOsipDebugInfo->GetLogLevel(&lLogLevel);
	}
	
	if (SUCCEEDED(hResult))
	{
		hResult = pOsipDebugInfo->GetLogMessage(&bstrLogMessage);
	}
	
	if (SUCCEEDED(hResult))
	{
		CString strFileName(bstrFileName);
		CString strLogMessage(bstrLogMessage);
		
		CFacadeWrapper::GetInstance()->LogOsipDebugEvent(
				strFileName, 
				lLineNumber, 
				OsipToUIMLogLevel(lLogLevel), 
				strLogMessage);
	}
}



void CUOLFoneController::HandleDownloadComplete(IUOLFoneDownload* pDownload)
{
	if (pDownload)
	{
		LONG lStatus = UFS_DOWNLOAD_STATUS_FAILED;

		HRESULT hr = pDownload->GetStatus(&lStatus);

		if (SUCCEEDED(hr))
		{
			if (lStatus == UFS_DOWNLOAD_STATUS_SUCCESS)
			{
				hr = m_spUOLFoneServerConfigProvider->GetPhoneValidator(pDownload, &m_spPhoneValidator);
				ATLASSERT(SUCCEEDED(hr));

				if (FAILED(hr))
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_GET_PHONE_RULES_VALIDATOR_FAILURE, hr);
				}
			}
			else
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_DOWNLOAD_PHONE_RULES_ERROR_FAILURE, 
						CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
			}
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_GET_DOWNLOAD_STATUS_PHONE_RULES_FAILURE, hr);
		}

		if (FAILED(hr))
		{
			NotifyInitializationFailed(IUOLFoneControllerCallback::InitStatusCodeLibraryInitializationError);
		}

		m_spPhoneRulesDownload.Release();
	}
}


void CUOLFoneController::HandleUpdateConfigComplete(LONG lResult)
{
	/// Codec list download (step 1.4) result check

	HRESULT hr = (HRESULT) lResult;

	if (SUCCEEDED(hr))
	{
		BOOL bCriticalCleanUpFailed = FALSE;
		
		if (m_spUOLFoneServerConfigProvider)
		{
			// First we perform the local codecs clean up.
			//
			CComQIPtr<IUOLFoneServerConfigProvider2> spUOLFoneServerConfigProvider2 = m_spUOLFoneServerConfigProvider;
			
			if (spUOLFoneServerConfigProvider2)
			{
				CComBSTR bstrCodecsLocalDir = GetCodecsPath();
				
				hr = spUOLFoneServerConfigProvider2->RunLocalCodecsCleanUp(bstrCodecsLocalDir);
				
				if (FAILED(hr))
				{
					ATLTRACE(_T("%s - hr = 0x%x, RunLocalCodecsCleanUp failed.\n"), __FUNCTION__, hr);
				}
				
				hr = m_spCodecsConfigFileUpdater->RunCriticalCodecsUpdateCleanUp(bstrCodecsLocalDir);
				
				if (FAILED(hr))
				{
					ATLTRACE(_T("%s - hr = 0x%x, RunCriticalCodecsUpdateCleanUp failed.\n"), __FUNCTION__, hr);
					
					bCriticalCleanUpFailed = TRUE;
				}
			}
		}
		
		if (!bCriticalCleanUpFailed)
		{
			// Now initialize the rest.
			//
			ScheduleTask(WM_USER_INITIALIZE_PHONE_LINES_TASK);
		}
	}
	else
	{
		LONG lErrorCode = 0;
		LONG lStatus = 0;
		
		hr = m_spCodecsConfigFileUpdater->GetUpdateError(&lErrorCode);
		
		if (SUCCEEDED(hr))
		{
			hr = m_spCodecsConfigFileUpdater->GetStatus(&lStatus);
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_CODECS_CONFIG_FILE_UPDATER_GET_ERROR_FAILURE, hr);
		}
		
		if (SUCCEEDED(hr))
		{
			if (lStatus == UFS_UPDATE_CONFIG_MISSING_PARAM)
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_CODECS_CONFIG_FILE_UPDATER_MISSING_PARAM_FAILURE, hr);
			}
			else if (lStatus == UFS_UPDATE_CONFIG_XML_PARSE_FAILED)
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_CODECS_CONFIG_FILE_UPDATER_XML_PARSE_FAILURE, hr);
			}
			else if (lStatus == UFS_UPDATE_CONFIG_DOWNLOAD_FAILED)
			{
				// It was a download failure.
				//
				LONG lAdditionalError = 0;
				LONG lHTTPStatusCode = 0;
				CComBSTR bstrHTTPStatusMessage;
				
				hr = m_spCodecsConfigFileUpdater->GetDownloadFailureInfo(
						&lAdditionalError, &lHTTPStatusCode, &bstrHTTPStatusMessage);
				
				if (SUCCEEDED(hr))
				{
					CString strCodecsListUrl;
					strCodecsListUrl.LoadString(IDS_URL_CODECS_LIST);
					
					if (CHttpDownloader::DE_HTTP_STATUS_CODE_ERROR == lErrorCode)
					{
						CString strHTTPStatusMessage(bstrHTTPStatusMessage);
						
						m_pLastFailureInfo = 
								CUOLFoneFailureInfo::BuildDownloadFailure(
										lErrorCode, 
										lAdditionalError, 
										strCodecsListUrl, 
										lHTTPStatusCode, 
										strHTTPStatusMessage);
					}
					else
					{
						m_pLastFailureInfo = 
								CUOLFoneFailureInfo::BuildDownloadFailure(
										lErrorCode, 
										lAdditionalError, 
										strCodecsListUrl);
					}
				}
				else
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_CODECS_CONFIG_FILE_UPDATER_GET_ADDITIONAL_ERROR_FAILURE, hr);
				}
			}
			else
			{
				// It was another error, but which ??????
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_CODECS_CONFIG_FILE_UPDATER_UNKNOWN_FAILURE, hr);
			}
		}
		else if (m_pLastFailureInfo == NULL)
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_CODECS_CONFIG_FILE_UPDATER_GET_STATUS_FAILURE, hr);
		}
		
		NotifyInitializationFailed(IUOLFoneControllerCallback::InitStatusCodeCodecsListDownloadError);
	}
}


void CUOLFoneController::HandleUpdateCodecFileBegin(BSTR codecName, BSTR fileName)
{
}


void CUOLFoneController::HandleUpdateCodecFileProgressChange(BSTR codecName, BSTR fileName, ULONG nProgress, ULONG nProgressMax)
{
}


void CUOLFoneController::HandleUpdateCodecFileComplete(BSTR codecName, BSTR fileName)
{
}


void CUOLFoneController::HandleUpdateCodecComplete(BSTR codecName)
{
	m_nUpdatedCodecsCount++;
}


void CUOLFoneController::HandleUpdateCodecsFinished(LONG lResult)
{
	/// Codecs update (step 4.2) result check

	HRESULT hr = (HRESULT) lResult;

	ATLTRACE(_T("%s - hr = 0x%x, m_nUpdatedCodecsCount = %d\n"), __FUNCTION__, hr, m_nUpdatedCodecsCount);
	ATLASSERT(SUCCEEDED(hr));
	
	if (FAILED(hr))
	{
		LONG lErrorCode = 0;
		LONG lStatus = 0;
		
		hr = m_spCodecsUpdater->GetUpdateError(&lErrorCode);
		
		if (SUCCEEDED(hr))
		{
			hr = m_spCodecsUpdater->GetStatus(&lStatus);
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_CODECS_UPDATER_GET_ERROR_FAILURE, hr);
		}
		
		if (SUCCEEDED(hr))
		{
			if (lStatus == UFS_UPDATE_MISSING_PARAM)
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_CODECS_UPDATER_MISSING_PARAM_FAILURE, hr);
			}
			else if (lStatus == UFS_UPDATE_PROCESS_XML_FAILED)
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_CODECS_UPDATER_PROCESS_XML_FAILURE, hr);
			}
			else if (lStatus == UFS_UPDATE_DOWNLOAD_FAILED)
			{
				// It was a download failure.
				//
				LONG lAdditionalError = 0;
				LONG lHTTPStatusCode = 0;
				CComBSTR bstrHTTPStatusMessage;
				CComBSTR bstrFailedFileURL;
				CComBSTR bstrFailedFilePath;
				
				hr = m_spCodecsUpdater->GetDownloadFailureInfo(
						&lAdditionalError, &lHTTPStatusCode, &bstrHTTPStatusMessage, 
						&bstrFailedFileURL, &bstrFailedFilePath);
				
				if (SUCCEEDED(hr))
				{
					CString strFailedFileURL(bstrFailedFileURL);
					CString strFailedFilePath(bstrFailedFilePath);
					
					if (CHttpDownloader::DE_HTTP_STATUS_CODE_ERROR == lErrorCode)
					{
						CString strHTTPStatusMessage(bstrHTTPStatusMessage);
						
						m_pLastFailureInfo = 
								CUOLFoneFailureInfo::BuildDownloadFailure(
										lErrorCode, 
										lAdditionalError, 
										strFailedFileURL, 
										lHTTPStatusCode, 
										strHTTPStatusMessage);
					}
					else
					{
						m_pLastFailureInfo = 
								CUOLFoneFailureInfo::BuildDownloadFailure(
										lErrorCode, 
										lAdditionalError, 
										strFailedFileURL);
					}
					
					if ( (m_pLastFailureInfo) && (!strFailedFilePath.IsEmpty()) )
					{
						m_pLastFailureInfo->AddExtraFailureInfo(
								CUOLFoneFailureInfo::EXTRA_INFO_PATH, strFailedFilePath);
					}
				}
				else
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_CODECS_UPDATER_GET_ADDITIONAL_ERROR_FAILURE, hr);
				}
			}
			else
			{
				// It was another error, but which ??????
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_CODECS_UPDATER_UNKNOWN_FAILURE, hr);
			}
		}
		else if (m_pLastFailureInfo == NULL)
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_CODECS_UPDATER_GET_STATUS_FAILURE, hr);
		}
		
		NotifyExecutionFailure(IUOLFoneControllerCallback::ExecStatusCodecUpdateFailure);
	}
}


void CUOLFoneController::OnUOLFoneAccountChanged(CUOLFoneAccountPtr pOldAccount)
{
	if (m_pUOLFoneAccountManager)
	{
		BOOL bNewIMAccountIsConnected = FALSE;
		CUOLFoneAccountPtr pCurrentAccount = GetCurrentAccount();

		// Check if new account is connected
		if (pCurrentAccount)
		{
            bNewIMAccountIsConnected = m_pUOLFoneAccountManager->IsIMAccountConnected(pCurrentAccount->GetIMAccount());
			
			// Zero user balance and show buy credits button
			CFacadeWrapper::GetInstance()->UpdateBalancePanel(pCurrentAccount);
		}

		if (!CUOLFonePluginConfig::GetInstance()->RegisterOnlyOnCall())
		{
			if (NULL != pOldAccount && IsConnected(pOldAccount))
			{
				// Schedule new account connection
				if (bNewIMAccountIsConnected)
				{
					m_pConnectionPendingAccount = pCurrentAccount;
				}
				
				// Disconnect old account
				Disconnect(pOldAccount);
			}
			else
			{
				if (bNewIMAccountIsConnected)
				{
					Connect(pCurrentAccount);
				}
				else
				{
					if (NULL == pCurrentAccount)
					{
						CFacadeWrapper::GetInstance()->ShowUOLAccountNotFound();
					}
					else
					{
						pCurrentAccount->SetConnectionState(CUOLFoneAccount::ConnectionStateOffline);
						NotifyAccountConnectionStateChanged(pCurrentAccount);
					}
				}
			}
		}
		else
		{
			// Just revoke capatibilities of old account.
			//
			RevokeCapabilities(pOldAccount->GetIMAccount());
			
			if (bNewIMAccountIsConnected)
			{
				// And publish the capabilities of new one.
				//
				PublishCapabilities(pCurrentAccount->GetIMAccount());
			}
			
			NotifyAccountConnectionStateChanged(pCurrentAccount);
		}
	}
}


void CUOLFoneController::OnDownloadBegin(DWORD dwIndex)
{
}


void CUOLFoneController::OnProgressChange(DWORD dwIndex, ULONG nProgress, ULONG nProgressMax)
{
}


void CUOLFoneController::OnDownloadComplete(DWORD dwIndex, DWORD dwError)
{
}


void CUOLFoneController::OnVerifyDllsFinished(HRESULT hr)
{
	/// Libraries download (step 0.4) result check

	if ( SUCCEEDED(hr) )
	{
		ScheduleTask(WM_USER_INITIALIZE_LIBRARIES_TASK);
	}
	else
	{
		m_pLastFailureInfo = m_VerifyDllsThread.GetLastFailureInfo();
		
		NotifyInitializationFailed(IUOLFoneControllerCallback::InitStatusCodeLibraryDownloadError);
	}
}


LRESULT CUOLFoneController::OnInitializeLibrariesTask(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ATLTRACE(_T("%s\n"), __FUNCTION__);

	/// 1. Initialize libraries
	BOOL bInitOk = InitializeComLibs();

	if (!bInitOk)
	{
		NotifyInitializationFailed(IUOLFoneControllerCallback::InitStatusCodeLibraryInitializationError);
	}

	return 0L;
}


LRESULT CUOLFoneController::OnInitializePhoneLinesTask(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    /// 2. Initialize phone lines...

	/// 2.1 Test network connection
	if (!TestNetworkConnection())
	{
		NotifyInitializationFailed(IUOLFoneControllerCallback::InitStatusCodeServerConnectionError);

		// Network test FAILED!!!
		ATLTRACE(_T("%s - Network test FAILED!!!\n"), __FUNCTION__);
		return 0L;
	}

	/// 2.2 Initialize/setup phone lines
	BOOL bInitOk = InitializePhoneLineConfig();

	if (bInitOk)
	{
		bInitOk = InitializePhoneLines();
	}

	if (bInitOk)
	{
		ATLTRACE(_T("%s - InitializePhoneLines() = [%d]\n"), __FUNCTION__, bInitOk);

		SetInternalStatus(InternalStatusReady);
		NotifyInitializationCompleted();

		// Connect current account if IM account is connected...
		CUOLFoneAccountPtr pCurrentAccount = GetCurrentAccount();

		if (pCurrentAccount)
		{
			if ((m_pUOLFoneAccountManager != NULL) &&
				(m_pUOLFoneAccountManager->IsIMAccountConnected(pCurrentAccount->GetIMAccount())))
			{
				ConnectAccountImpl(pCurrentAccount);
			}
		}
	}
	else
	{
		SetInternalStatus(InternalStatusInitializationError);
		NotifyInitializationFailed(IUOLFoneControllerCallback::InitStatusCodeLibraryInitializationError);
	}

	return 0L;
}


LRESULT CUOLFoneController::OnCheckCodecsUpdateTask(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/// 3. Check codecs update...

	CUOLFoneAccountPtr pAccount = GetCurrentAccount();

	if (pAccount)
	{
		HRESULT hr = E_FAIL;

		// It should be already been saved.
		//
		m_pLastFailureInfo.reset();
		
		BOOL bHasInscription = pAccount->HasInscription();

		m_nUpdatedCodecsCount = 0;

		if (m_spUOLFoneServerConfigProvider && m_spCodecsConfigFileUpdater)
		{
			CComBSTR bstrConfigFilePath;

			hr = m_spCodecsConfigFileUpdater->GetCodecsConfigFilePath(&bstrConfigFilePath);

			if (SUCCEEDED(hr))
			{
				hr = CComObject<CCodecsUpdaterCallback>::CreateInstance(&m_pCodecsUpdaterCallback);
				
				if (SUCCEEDED(hr))
				{
					m_pCodecsUpdaterCallback->SetCallbackHandler(this);
					m_pCodecsUpdaterCallback->AddRef();
					
					CComPtr<IUOLFoneCodecsUpdaterCallback> pCodecsUpdaterCallback = m_pCodecsUpdaterCallback;
					
					CComBSTR bstrCodecsLocalDir = GetCodecsPath();

					SetInternalStatus(GetInternalStatus() | InternalStatusUpdatingCodecs);

					CComQIPtr<IUOLFoneServerConfigProvider3> spUOLFoneServerConfigProvider3 = m_spUOLFoneServerConfigProvider;
					
					if (spUOLFoneServerConfigProvider3)
					{
						hr = spUOLFoneServerConfigProvider3->CreateCodecsUpdater(
								bstrConfigFilePath, bstrCodecsLocalDir, bHasInscription, 
								pCodecsUpdaterCallback, &m_spCodecsUpdater);
							
						if (SUCCEEDED(hr))
						{
							CComPtr<IUOLFoneCodecsUpdateInfo> spCodecsUpdateInfo;
							
							CString strUOLFoneClientVersion;
							strUOLFoneClientVersion.LoadString(IDS_UOL_FONE_CLIENT_DLL_VERSION);
							
							hr = m_spCodecsUpdater->SetUOLFoneClientVersion(
									strUOLFoneClientVersion.AllocSysString());

							if (FAILED(hr))
							{
								m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
										CUOLFoneFailureInfo::LIBRARY_CODECS_SET_UOLFONECLIENT_VERSION_FAILURE, hr);
							}
							else
							{
								hr = m_spCodecsUpdater->ProcessUpdateList(&spCodecsUpdateInfo); // TODO: Why this is not handled ?
								
								hr = m_spCodecsUpdater->StartUpdate();
								ATLASSERT(SUCCEEDED(hr));
								
								if (FAILED(hr))
								{
									m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
											CUOLFoneFailureInfo::LIBRARY_CODECS_UPDATER_STARTUPDATE_FAILURE, hr);
								}
								
								// Don't matter we have updates, notify account registration
								hr = E_FAIL;
							}
						}
						else
						{
							m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
									CUOLFoneFailureInfo::LIBRARY_GET_CODECS_UPDATER_INSTANCE_FAILURE, hr);
						}
					}
				}
				else
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_INIT_CODECS_UPDATER_FAILURE, hr);
				}
			}
			else
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_CODECS_CONFIG_FILE_UPDATER_GET_FILE_PATH_FAILURE, hr);
			}
		}

		if (m_pLastFailureInfo)
		{
			NotifyExecutionFailure(IUOLFoneControllerCallback::ExecStatusCodecUpdateFailure);
		}
		
		if (FAILED(hr))
		{
			// Notify account registration...
			SetInternalStatus(InternalStatusReady);

			NotifyPhoneLineRegistered(pAccount);
			NotifyAccountConnectionStateChanged(pAccount);
		}
	}

	return 0L;
}


LRESULT CUOLFoneController::OnReinitializePhonelinesTask(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	/// 5. Reinitialize phone lines

	/// 5.1 Initialize phone lines

	BOOL bOk = InitializePhoneLines();
	ATLASSERT(bOk);

	ATLTRACE(_T("%s - InitializePhoneLines = [%d]\n"), __FUNCTION__, bOk);

	if (bOk)
	{
		SetInternalStatus(InternalStatusReady);

		/// 5.2 Register user account

		CUOLFoneAccountPtr pAccount = m_pConnectionPendingAccount;
		m_pConnectionPendingAccount = CUOLFoneAccountPtr();

		ATLASSERT(pAccount);

		bOk = ConnectAccountImpl(pAccount);
		ATLASSERT(bOk);

		ATLTRACE(_T("%s - ConnectAccountImpl = [%d]\n"), __FUNCTION__, bOk);
	}

	return 0L;
}



LRESULT CUOLFoneController::OnConnectAccount(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;

	LPUOLFONEACCOUNTPARAM pUfap = (LPUOLFONEACCOUNTPARAM) wParam;

	if (pUfap)
	{
		bRet = ConnectAccountImpl(pUfap->pAccount);

		BOOL bReleaseMemory = pUfap->bReleaseMemory;

		if (bReleaseMemory)
		{
			delete pUfap;
		}
	}

	return (LRESULT) bRet;
}


LRESULT CUOLFoneController::OnDisconnectAccount(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;

	LPUOLFONEACCOUNTPARAM pUfap = (LPUOLFONEACCOUNTPARAM) wParam;

	if (pUfap)
	{
		bRet = DisconnectAccountImpl(pUfap->pAccount);

		BOOL bReleaseMemory = pUfap->bReleaseMemory;

		if (bReleaseMemory)
		{
			delete pUfap;
		}
	}

	return (LRESULT) bRet;
}


LRESULT CUOLFoneController::OnCheckPhoneNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpszPhoneNumber = (LPCTSTR) wParam;

	if (lpszPhoneNumber != NULL)
	{
		return IsValidPhoneNumberImpl(lpszPhoneNumber);
	}

	return FALSE;
}


LRESULT CUOLFoneController::OnCallPhoneNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;

	LPCALLPHONENUMBERPARAM pCpnp = (LPCALLPHONENUMBERPARAM) wParam;

	if (pCpnp)
	{
		if (!m_spPhoneLine)
		{
			m_pPendingCallNumber = pCpnp->strPhoneNumber;
			
			bRet = ConnectAccountImpl(GetCurrentAccount());
		}
		else
		{
			bRet = MakePhoneCallImpl(pCpnp->strPhoneNumber);
		}

		delete pCpnp;
	}

	return (LRESULT) bRet;
}


LRESULT CUOLFoneController::OnCallContact(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;

	LPCALLCONTACTPARAM pCcp = (LPCALLCONTACTPARAM) wParam;

	if (pCcp)
	{
		if (!m_spPhoneLine)
		{
			m_pPendingCallContact = pCcp->pContact;
			
			bRet = ConnectAccountImpl(GetCurrentAccount());
		}
		else
		{
			bRet = MakePhoneCallImpl(pCcp->pContact);
		}

		delete pCcp;
	}

	return (LRESULT) bRet;
}


LRESULT CUOLFoneController::OnHangUpCall(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;

	LPUOLFONEACCOUNTPARAM pUfap = (LPUOLFONEACCOUNTPARAM) wParam;

	if (pUfap)
	{
		if (pUfap->pAccount)
		{
			bRet = HangUpPhoneCallImpl(pUfap->pAccount);
		}

		if (pUfap->bReleaseMemory)
		{
			delete pUfap;
		}
	}

	return (LRESULT) bRet;
}


LRESULT CUOLFoneController::OnAcceptIncomingCall(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;

	LPUOLFONEACCOUNTPARAM pUfap = (LPUOLFONEACCOUNTPARAM) wParam;

	if (pUfap)
	{
		if (pUfap->pAccount)
		{
			bRet = AcceptIncomingCallImpl(pUfap->pAccount);
		}

		if (pUfap->bReleaseMemory)
		{
			delete pUfap;
		}
	}

	return (LRESULT) bRet;
}


LRESULT CUOLFoneController::OnRejectIncomingCall(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;

	LPUOLFONEACCOUNTPARAM pUfap = (LPUOLFONEACCOUNTPARAM) wParam;

	if (pUfap)
	{
		if (pUfap->pAccount)
		{
			bRet = RejectIncomingCallImpl(pUfap->pAccount);
		}

		if (pUfap->bReleaseMemory)
		{
			delete pUfap;
		}
	}

	return (LRESULT) bRet;
}


LRESULT CUOLFoneController::OnTransferCall(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;
	
	LPCALLPHONENUMBERPARAM pCpnp = (LPCALLPHONENUMBERPARAM) wParam;
	
	if (pCpnp)
	{
		bRet = TransferCallImpl(pCpnp->strPhoneNumber);
		
		delete pCpnp;
	}
	
	return (LRESULT) bRet;
}


LRESULT CUOLFoneController::OnHoldUpCall(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;
	
	LPUOLFONEACCOUNTPARAM pUfap = (LPUOLFONEACCOUNTPARAM) wParam;
	
	if (pUfap)
	{
		if (pUfap->pAccount)
		{
			bRet = HoldUpPhoneCallImpl(pUfap->pAccount);
		}
		
		if (pUfap->bReleaseMemory)
		{
			delete pUfap;
		}
	}
	
	return (LRESULT) bRet;
}


LRESULT CUOLFoneController::OnResumeUpCall(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;
	
	LPUOLFONEACCOUNTPARAM pUfap = (LPUOLFONEACCOUNTPARAM) wParam;
	
	if (pUfap)
	{
		if (pUfap->pAccount)
		{
			bRet = ResumeUpPhoneCallImpl(pUfap->pAccount);
		}
		
		if (pUfap->bReleaseMemory)
		{
			delete pUfap;
		}
	}
	
	return (LRESULT) bRet;
}


LRESULT CUOLFoneController::OnMakeConferenceCall(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;
	
	LPCONFERENCECALLNUMBERSPARAM pCcnp = (LPCONFERENCECALLNUMBERSPARAM) wParam;
	
	if (pCcnp)
	{
		bRet = MakeConferenceCallImpl(pCcnp->strPhoneNumber01, pCcnp->strPhoneNumber02);
		
		delete pCcnp;
	}
	
	return (LRESULT) bRet;
}


LRESULT CUOLFoneController::OnHangUpConferenceCall(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;
	
	LPUOLFONEACCOUNTPARAM pUfap = (LPUOLFONEACCOUNTPARAM) wParam;
	
	if (pUfap)
	{
		if (pUfap->pAccount)
		{
			bRet = HangUpConferenceCallImpl(pUfap->pAccount);
		}
		
		if (pUfap->bReleaseMemory)
		{
			delete pUfap;
		}
	}
	
	return (LRESULT) bRet;
}


void CUOLFoneController::OnTimer(UINT nTimerId, TIMERPROC timerproc)
{
	if (nTimerId != m_nTimerSendRingingNotification)
	{
		return;
	}

	if (m_spUOLFoneClient && m_spPhoneCall)
	{
		HRESULT hr = E_FAIL;

		if (m_nPhoneRingingCount < MAX_PHONE_RINGING_COUNT)
		{
			hr = m_spUOLFoneClient->SendRingingNotification(m_spPhoneCall);
			ATLASSERT(SUCCEEDED(hr));

			m_nPhoneRingingCount++;
		}
		else
		{
			// User is unavailable ?

			hr = m_spUOLFoneClient->RejectPhoneCall(m_spPhoneCall, UFC_FAILURE_CODE_TEMPORARILY_UNAVAILABLE);
			ATLASSERT(SUCCEEDED(hr));

			StopSendRingingNotificationTimer();
		}
	}
}


void CUOLFoneController::OnDestroy()
{
	if (m_nTimerSendRingingNotification != 0)
	{
		KillTimer(m_nTimerSendRingingNotification);
		m_nTimerSendRingingNotification = 0;
	}

	FinalizeComLibs();
}


CUOLFoneController::CUOLFoneController()
{
	m_nInternalStatus = InternalStatusUnitialized;
	m_bCheckCodecUpdates = FALSE;
	m_nUpdatedCodecsCount = 0;
	m_pControllerCallback = NULL;
	m_pCallback = NULL;
	m_pDownloadCallback = NULL;
	m_pCodecsConfigFileUpdaterCallback = NULL;
	m_pCodecsUpdaterCallback = NULL;
	m_pPhoneLineConfig = NULL;
	m_pPhoneLineInfo = NULL;
	m_pUserInfo = NULL;
	m_pNetworkConfig = NULL;
	m_pMediaNetworkConfig = NULL;
	m_nTimerSendRingingNotification = 0;
	m_nPhoneRingingCount = 0;
	m_bCapabilitiesPublished = FALSE;
	m_bIsCallPresenceDependent = FALSE;
}









/////////////////////////////////////////////////
// private functions
/////////////////////////////////////////////////



BOOL CUOLFoneController::IsInitialized()
{
	return GetInternalStatus() == InternalStatusReady;
}


void CUOLFoneController::SetInternalStatus(int nStatus)
{
	m_nInternalStatus = nStatus;
}


int CUOLFoneController::GetInternalStatus()
{
	return m_nInternalStatus;
}


CString CUOLFoneController::GetUOLFoneClientDllName() const
{
#ifdef _DEBUG
	return GetDebugDllName(IDS_UOL_FONE_CLIENT_DLL_NAME, IDS_UOL_FONE_CLIENT_DLL_VERSION) + _T(".dll");
#else
	return GetDllName(IDS_UOL_FONE_CLIENT_DLL_NAME, IDS_UOL_FONE_CLIENT_DLL_VERSION) + _T(".dll");
#endif
}


CString CUOLFoneController::GetUOLFoneCryptDllName(BOOL bLocal) const
{
	if (bLocal)
	{
		return GetDllName(IDS_UOL_FONE_CRYPT_DLL_NAME, -1) + _T(".dll");
	}
	else
	{
		return GetDllName(IDS_UOL_FONE_CRYPT_DLL_NAME, IDS_UOL_FONE_CRYPT_DLL_VERSION) + _T(".dll");
	}
}


CString CUOLFoneController::GetUOLFoneServerConfigProviderDllName() const
{
#ifdef _DEBUG
	return GetDebugDllName(IDS_UOL_FONE_SERVER_CONFIG_PROVIDER_DLL_NAME, IDS_UOL_FONE_SERVER_CONFIG_PROVIDER_DLL_VERSION) + _T(".dll");
#else
	return GetDllName(IDS_UOL_FONE_SERVER_CONFIG_PROVIDER_DLL_NAME, IDS_UOL_FONE_SERVER_CONFIG_PROVIDER_DLL_VERSION) + _T(".dll");
#endif
}


CString CUOLFoneController::GetDllName(UINT nResDllName, UINT nResDllVersion) const
{
	CString strName;
	strName.LoadString(nResDllName);

	if (-1 != nResDllVersion)
	{
		CString strVersion;
		strVersion.LoadString(nResDllVersion);

		CString strDllName;
		strDllName.Format("%s-%s", strName, strVersion);

		return strDllName;
	}
	else
	{
		return strName;
	}
}


#ifdef _DEBUG
CString CUOLFoneController::GetDebugDllName(UINT nResDllName, UINT nResDllVersion) const
{
	CString strDllName;

	CString strName;
	strName.LoadString(nResDllName);

	if (-1 != nResDllVersion)
	{
		CString strVersion;		
		strVersion.LoadString(nResDllVersion);

		strDllName.Format("%sd-%s", strName, strVersion);
	}
	else
	{
		strDllName.Format("%sd", strName);
	}

	return strDllName;
}
#endif


CString CUOLFoneController::GetCodecsPath()
{	
	CString strCodecsPath;
#ifdef _DEBUG
	strCodecsPath.LoadString(IDS_PLUGINS_DEBUG_DIR);
#else
	strCodecsPath.LoadString(IDS_PLUGINS_RELEASE_DIR);
#endif

	CString strDirNameWithBackSlash(m_strUIMLibDir);
	if (strDirNameWithBackSlash.Right(1) != '\\')
	{
		strDirNameWithBackSlash += '\\';
	}

	return strDirNameWithBackSlash + strCodecsPath;
}


BOOL CUOLFoneController::ScheduleTask(UINT nMessageId)
{
	return PostMessage(nMessageId);
}


BOOL CUOLFoneController::InitializeComLibs()
{
	CString strUOLFoneClientDllPath = m_strUIMLibDir + GetUOLFoneClientDllName();
	CString strUOLFoneServerConfigProviderDllPath = m_strUIMLibDir + GetUOLFoneServerConfigProviderDllName();

	BOOL bInitialized = CComFacade::GetInstance()->Initialize(strUOLFoneClientDllPath, strUOLFoneServerConfigProviderDllPath);

	if (bInitialized)
	{
		/// 1.1 Initialize UOLFoneServerConfigProvider API
		bInitialized = InitializeUOLFoneServerConfigProviderApi();

		if (bInitialized)
		{
			bInitialized = InitializeServerConfig();
			
			if (bInitialized)
			{
				bInitialized = InitializeNetworkConfig();
			}
			else
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_INIT_SERVER_CONFIG_FAILURE, 
						CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
			}

			if (bInitialized)
			{
				/// 1.2 Initialize UOLFoneClient API
				bInitialized = InitializeUOLFoneClientApi();
			}
			else if (m_pLastFailureInfo == NULL)
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_INIT_NETWORK_CONFIG_FAILURE, 
						CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
			}

			if (bInitialized)
			{
				/// 1.3 Start phone validation rules download
				bInitialized = DownloadPhoneValidationRules();

				/// 1.4 Start codecs list download
				if (bInitialized)
				{
					bInitialized = DownloadCodecsList();
				}
			}
		}
	}
	else
	{
		m_pLastFailureInfo = CComFacade::GetInstance()->GetLastFailureInfo();
	}

	return bInitialized;
}


void CUOLFoneController::FinalizeComLibs()
{
	FinalizeUOLFoneServerConfigProviderApi();
	FinalizeUOLFoneClientApi();
	FinalizePhoneLineConfig();
	FinalizeServerConfig();
	FinalizeNetworkConfig();
	FinalizeUserInfo();
	FinalizePhoneLineInfo();

	CComFacade::GetInstance()->Finalize();
}


BOOL CUOLFoneController::InitializeUOLFoneClientApi()
{
	BOOL bRet = FALSE;

	HRESULT hr = CComFacade::GetInstance()->GetUOLFoneClientInstance(&m_spUOLFoneClient);

	if ((SUCCEEDED(hr)) && m_spUOLFoneClient)
	{
		hr = CComObject<CUOLFoneClientCallback>::CreateInstance(&m_pCallback);

		if (SUCCEEDED(hr))
		{
			m_pCallback->AddRef();
			m_pCallback->SetCallbackHandler(this);

			CComQIPtr<IUOLFoneClientCallback2> pCallback2 = m_pCallback;
			ATLASSERT(pCallback2 != NULL);

			CComBSTR bstrCodecsPath = GetCodecsPath();
			CComQIPtr<IUOLFoneClient2> pUOLFoneClient2 = m_spUOLFoneClient;
			
			if ( (pUOLFoneClient2) && (pCallback2) )
			{
				hr = pUOLFoneClient2->Initialize(pCallback2, 
						bstrCodecsPath, (EnumOsipDebugLevel)UIMToOsipLogLevel(
								CFacadeWrapper::GetInstance()->GetOsipLogCurrentLevel()));

				if (SUCCEEDED(hr))
				{
					bRet = TRUE;
				}
				else
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_INIT_UOLFONECLIENT_FAILURE, hr);
				}
			}
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_INSTANTIATE_UOLFONECLIENTCALLBACK_FAILURE, hr);
		}
	}
	else
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
				CUOLFoneFailureInfo::LIBRARY_INSTANTIATE_UOLFONECLIENT_FAILURE, hr);
	}

	return bRet;
}


void CUOLFoneController::FinalizeUOLFoneClientApi()
{
	if (m_spConnectionTestResult)
	{
		m_spConnectionTestResult.Release();
	}

	if (m_spPhoneCall)
	{
		if (m_spUOLFoneClient)
		{
			HRESULT hr = m_spUOLFoneClient->ClosePhoneCall(m_spPhoneCall);
			ATLASSERT(SUCCEEDED(hr));
		}

		m_spPhoneCall.Release();
	}

	if (m_spPhoneLine)
	{
		m_spPhoneLine.Release();
	}

	if (m_pCallback)
	{
		m_pCallback->SetCallbackHandler(NULL);
		m_pCallback->Release();
		m_pCallback = NULL;
	}

	if (m_spUOLFoneClient)
	{
		FinalizePhoneLines();

		m_spUOLFoneClient->Finalize();

		m_spUOLFoneClient.Release();
	}
}


BOOL CUOLFoneController::InitializeUOLFoneServerConfigProviderApi()
{
	BOOL bRet = FALSE;

	HRESULT hr = CComFacade::GetInstance()->GetUOLFoneServerConfigProviderInstance(&m_spUOLFoneServerConfigProvider);

	if ((SUCCEEDED(hr)) && m_spUOLFoneServerConfigProvider)
	{
		hr = m_spUOLFoneServerConfigProvider->Initialize();

		if (SUCCEEDED(hr))
		{
			if ((bRet = m_serverMessageHandler.Initialize(m_spUOLFoneServerConfigProvider)) == TRUE)
			{
				// Set install cookie param
				CComQIPtr<IUOLFoneServerConfigProvider2> spUOLFoneServerConfigProvider2 = m_spUOLFoneServerConfigProvider;

				if (spUOLFoneServerConfigProvider2)
				{
					CComPtr<IUOLFoneSettings> pSettings;

					hr = spUOLFoneServerConfigProvider2->GetSettings(&pSettings);

					if (SUCCEEDED(hr))
					{
						CString strInstallCookie = GetInstallCookie();

						if (FALSE == strInstallCookie.IsEmpty())
						{
							CComBSTR bstrXidParam = _T("__url_param__xid");
							CComBSTR bstrXidValue = strInstallCookie;

							hr = pSettings->SetOptionValue(bstrXidParam, bstrXidValue);
							ATLASSERT(SUCCEEDED(hr));
						}
					}
				}
			}
			else
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_INIT_SERVER_MESSAGE_HANDLER_FAILURE, 
						m_serverMessageHandler.GetInitializeError());
			}
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_INIT_UOLFONESERVERCONFIG_FAILURE, hr);
		}
	}
	else
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
				CUOLFoneFailureInfo::LIBRARY_INSTANTIATE_UOLFONESERVERCONFIG_FAILURE, hr);
	}

	return bRet;
}


void CUOLFoneController::FinalizeUOLFoneServerConfigProviderApi()
{
	m_serverMessageHandler.Finalize();

	if (m_spPhoneRulesDownload)
	{
		m_spPhoneRulesDownload.Release();
	}

	if (m_spPhoneValidator)
	{
		m_spPhoneValidator.Release();
	}

	if (m_spCodecsConfigFileUpdater)
	{
		m_spCodecsConfigFileUpdater->CancelUpdate();
		m_spCodecsConfigFileUpdater.Release();
	}

	if (m_spCodecsUpdater)
	{
		m_spCodecsUpdater->CancelUpdate();
		m_spCodecsUpdater.Release();
	}

	if (m_spUOLFoneServerConfigProvider)
	{
		HRESULT hr = m_spUOLFoneServerConfigProvider->Finalize();

		ATLASSERT(SUCCEEDED(hr));

		m_spUOLFoneServerConfigProvider.Release();
	}

	if (m_pDownloadCallback)
	{
		m_pDownloadCallback->Release();
		m_pDownloadCallback = NULL;
	}

	if (m_pCodecsConfigFileUpdaterCallback)
	{
		m_pCodecsConfigFileUpdaterCallback->Release();
		m_pCodecsConfigFileUpdaterCallback = NULL;
	}

	if (m_pCodecsUpdaterCallback)
	{
		m_pCodecsUpdaterCallback->Release();
		m_pCodecsUpdaterCallback = NULL;
	}
}


BOOL CUOLFoneController::InitializePhoneLineConfig()
{
	HRESULT hr = CComObject<CPhoneLineConfig>::CreateInstance(&m_pPhoneLineConfig);

	if (SUCCEEDED(hr))
	{
		hr = m_pPhoneLineConfig->AddRef();

		hr = SetupNetworkConfig();
		
		// Set codecs precedence
		CString strAudioCodecsPrecedence = GetAudioCodecsPrecedence(FALSE);

		if (FALSE == strAudioCodecsPrecedence.IsEmpty())
		{
			m_pPhoneLineConfig->SetAudioCodecsPrecedence(strAudioCodecsPrecedence);
		}
		//m_pPhoneLineConfig->SetVideoCodecsPrecedence();
	}
	else
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
				CUOLFoneFailureInfo::LIBRARY_CREATE_PHONE_LINE_CONFIG_FAILURE, hr);
	}

	return SUCCEEDED(hr);
}


void CUOLFoneController::FinalizePhoneLineConfig()
{
	if (m_pPhoneLineConfig)
	{
		m_pPhoneLineConfig->Release();
		m_pPhoneLineConfig = NULL;
	}
}


BOOL CUOLFoneController::InitializeServerConfig()
{
	m_pServerConfig = new CServerConfig();
	
	if (m_pServerConfig)
	{
		return m_pServerConfig->AllocConfig();
	}
	
	return FALSE;
}


void CUOLFoneController::FinalizeServerConfig()
{
	if (m_pServerConfig)
	{
		m_pServerConfig->ReleaseConfig();
		m_pServerConfig = NULL;
	}
}


BOOL CUOLFoneController::InitializeNetworkConfig()
{
	HRESULT hr = CComObject<CNetworkConfig>::CreateInstance(&m_pNetworkConfig);

	if (SUCCEEDED(hr))
	{
		m_pNetworkConfig->AddRef();
	}

	hr = CComObject<CNetworkConfig>::CreateInstance(&m_pMediaNetworkConfig);

	if (SUCCEEDED(hr))
	{
		m_pMediaNetworkConfig->AddRef();
	}

	return SUCCEEDED(hr);
}


void CUOLFoneController::FinalizeNetworkConfig()
{
	if (m_pNetworkConfig)
	{
		m_pNetworkConfig->Release();
		m_pNetworkConfig = NULL;
	}

	if (m_pMediaNetworkConfig)
	{
		m_pMediaNetworkConfig->Release();
		m_pMediaNetworkConfig = NULL;
	}
}


BOOL CUOLFoneController::InitializePhoneLines()
{
	CString strLineDevice;
	
	// It should already been saved.
	//
	m_pLastFailureInfo.reset();
	
	HRESULT hr = m_spUOLFoneClient->InitializePhoneLines(m_pPhoneLineConfig);
	
	if (SUCCEEDED(hr))
	{
		strLineDevice = CUOLFonePluginConfig::GetInstance()->GetLineOutDevice();
		
		if (!strLineDevice.IsEmpty())
		{
			hr = m_spUOLFoneClient->SetAudioDevice(
					UFC_AUDIO_DEVICE_TYPE_CALL_OUTPUT, strLineDevice.AllocSysString());
		}
	}
	else
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
				CUOLFoneFailureInfo::LIBRARY_INITIALIZE_PHONE_LINES_FAILURE, hr);
	}
	
	if (SUCCEEDED(hr))
	{
		strLineDevice = CUOLFonePluginConfig::GetInstance()->GetLineInDevice();
		
		if (!strLineDevice.IsEmpty())
		{
			hr = m_spUOLFoneClient->SetAudioDevice(
					UFC_AUDIO_DEVICE_TYPE_CALL_INPUT, strLineDevice.AllocSysString());
		}
	}
	else if (m_pLastFailureInfo == NULL)
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
				CUOLFoneFailureInfo::LIBRARY_SET_OUTPUT_AUDIO_DEVICE_FAILURE, hr);
	}
	
	if ( (FAILED(hr)) && (m_pLastFailureInfo == NULL) )
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
				CUOLFoneFailureInfo::LIBRARY_SET_INPUT_AUDIO_DEVICE_FAILURE, hr);
	}
	
	return (SUCCEEDED(hr));
}


BOOL CUOLFoneController::FinalizePhoneLines()
{
	HRESULT hr = m_spUOLFoneClient->FinalizePhoneLines();

	return (SUCCEEDED(hr));
}


BOOL CUOLFoneController::InitializeUserInfo()
{
	HRESULT hr = CComObject<CUserInfo>::CreateInstance(&m_pUserInfo);

	if (SUCCEEDED(hr))
	{
		m_pUserInfo->AddRef();
	}

	return SUCCEEDED(hr);
}


void CUOLFoneController::FinalizeUserInfo()
{
	if (m_pUserInfo)
	{
		m_pUserInfo->Release();	
		m_pUserInfo = NULL;
	}
}


BOOL CUOLFoneController::InitializePhoneLineInfo()
{
	HRESULT hr = CComObject<CPhoneLineInfo>::CreateInstance(&m_pPhoneLineInfo);

	if (SUCCEEDED(hr))
	{
		m_pPhoneLineInfo->AddRef();

		if (m_pUserInfo)
		{
			CComQIPtr<IUOLFoneClientUserInfo> pUserInfo = m_pUserInfo;

			hr = m_pPhoneLineInfo->SetUserInfo(pUserInfo);

			// Default register expiration: 60 minutes
			m_pPhoneLineInfo->SetRegisterTimeout(60 * 60);
		}
	}

	return SUCCEEDED(hr);
}


void CUOLFoneController::FinalizePhoneLineInfo()
{
	if (m_pPhoneLineInfo)
	{
		m_pPhoneLineInfo->Release();
		m_pPhoneLineInfo = NULL;
	}
}


BOOL CUOLFoneController::TestNetworkConnection()
{
	if (m_pServerConfig == NULL)
	{
		return FALSE;
	}

	LONG lResult = UFC_CONNECTION_TEST_RESULT_NO_CONNECTION;

	HRESULT hr = E_FAIL;
	CComObject<CConnectionTestParam>* pConnectionTestParamObj = NULL;

	do
	{
		hr = CComObject<CConnectionTestParam>::CreateInstance(&pConnectionTestParamObj);
		if (FAILED(hr))
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_TEST_CONNECTION_CREATE_PARAM_FAILURE, hr);
			
			break;
		}

		// Server info
		CComPtr<IUOLFoneClientServerInfo> pServerInfo;		
		hr = m_pServerConfig->GetServerInfo(&pServerInfo);
		if (FAILED(hr))
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_TEST_CONNECTION_GET_SERVER_INFO_FAILURE, hr);
			
			break;
		}

		hr = pConnectionTestParamObj->SetServerInfo(pServerInfo);
		if (FAILED(hr))
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_TEST_CONNECTION_SET_SERVER_INFO_FAILURE, hr);
			
			break;
		}

		// HTTP tunnel info
		CComPtr<IUOLFoneClientServerInfo> pHttpTunnelInfo = NULL;
		hr = m_pServerConfig->GetHttpTunnelInfo(&pHttpTunnelInfo);
		if (FAILED(hr))
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_TEST_CONNECTION_GET_HTTP_TUNNEL_INFO_FAILURE, hr);
			
			break;
		}

		hr = pConnectionTestParamObj->SetHttpTunnelInfo(pHttpTunnelInfo);
		if (FAILED(hr))
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_TEST_CONNECTION_SET_HTTP_TUNNEL_INFO_FAILURE, hr);
			
			break;
		}

		// Http tunnel proxy info
		CComPtr<IUOLFoneClientServerInfo> pHttpProxyInfo = NULL;
		hr = m_pServerConfig->GetHttpProxyInfo(&pHttpProxyInfo);
		if (FAILED(hr))
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_TEST_CONNECTION_GET_HTTP_PROXY_INFO_FAILURE, hr);
			
			break;
		}

		hr = pConnectionTestParamObj->SetHttpProxyInfo(pHttpProxyInfo);
		if (FAILED(hr))
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_TEST_CONNECTION_SET_HTTP_PROXY_INFO_FAILURE, hr);
			
			break;
		}

		pConnectionTestParamObj->SetHttpTunnelMode(UFC_HTTP_TUNNEL_MODE_CONNECT);

		CComQIPtr<IUOLFoneClientConnectionTestParam> pConnectionTestParam = pConnectionTestParamObj;

		if (m_spConnectionTestResult)
		{
			m_spConnectionTestResult.Release();
		}

		hr = m_spUOLFoneClient->TestConnection(pConnectionTestParam, &m_spConnectionTestResult);
		if (FAILED(hr))
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_TEST_CONNECTION_TEST_FAILURE, hr);
			
			break;
		}

		hr = m_spConnectionTestResult->GetResult(&lResult);
		if (FAILED(hr))
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_TEST_CONNECTION_GETRESULT_FAILURE, hr);
			
			break;
		}

#ifdef _DEBUG

		CString strText = "Error";

		if (UFC_CONNECTION_TEST_RESULT_NO_CONNECTION == lResult)
		{
			strText.Format("Result = UFC_CONNECTION_TEST_RESULT_NO_CONNECTION");
		}
		else if (UFC_CONNECTION_TEST_RESULT_ALLOW_UDP == lResult)
		{
			strText.Format("Result = UFC_CONNECTION_TEST_RESULT_ALLOW_UDP");
		}
		else if (UFC_CONNECTION_TEST_RESULT_ALLOW_TCP == lResult)
		{
			strText.Format("Result = UFC_CONNECTION_TEST_RESULT_ALLOW_TCP");
		}
		else if (UFC_CONNECTION_TEST_RESULT_ALLOW_HTTP_TUNNEL == lResult)
		{
			strText.Format("Result = UFC_CONNECTION_TEST_RESULT_ALLOW_HTTP_TUNNEL");
		}

		ATLTRACE(_T("CUOLFoneController::TestNetworkConnection %s\n"), strText);
#endif

	}while (FALSE);

	if (UFC_CONNECTION_TEST_RESULT_NO_CONNECTION == lResult)
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
				CUOLFoneFailureInfo::LIBRARY_TEST_CONNECTION_NO_CONNECTION_FAILURE, hr);
	}
	
	return (UFC_CONNECTION_TEST_RESULT_NO_CONNECTION != lResult);
}


CString CUOLFoneController::GetAudioCodecsPrecedence(BOOL bWithSubscriberCodec)
{
	CString strAudioCodecs;

	if (m_spUOLFoneClient && m_spCodecsConfigFileUpdater)
	{
		CComPtr<IUOLFoneClientCodecInfoEnum> pCodecInfoEnum;

		HRESULT hr = m_spUOLFoneClient->EnumerateAudioCodecs(&pCodecInfoEnum);

		if (SUCCEEDED(hr))
		{
			// Fill map of UOLFoneClient supported codecs...
			CAtlMap<CString, CString> mapCodecs;

			do
			{
				CComPtr<IUOLFoneClientCodecInfo> pCodecInfo;
				ULONG lFetched = 0;

				hr = pCodecInfoEnum->Next(1, &pCodecInfo, &lFetched);

				if (FAILED(hr))
				{
					break;
				}

				if (pCodecInfo)
				{
					CComBSTR bstrName;
					pCodecInfo->GetCodecName(&bstrName);

					CComBSTR bstrId;
					pCodecInfo->GetCodecId(&bstrId);

					CString strName(bstrName);
					CString strId(bstrId);

					if ((FALSE == strName.IsEmpty()) &&
						(FALSE == strId.IsEmpty()))
					{
						strName = strName.MakeUpper();
						mapCodecs.SetAt(strName, strId);
					}

					pCodecInfo.Release();
				}
			}
			while (hr != S_FALSE);

			// Read server list codecs order and build codec list with codecs Id...
			BOOL bFirst = TRUE;

/// <TEST>
#ifdef FORCE_G729
			bFirst = FALSE;
			strAudioCodecs = _T("G729");
#endif
/// </TEST>

			CComPtr<IUOLFoneCodecInfoList> pCodecInfoList;

			hr = m_spCodecsConfigFileUpdater->GetCodecInfoList(&pCodecInfoList);

			if (SUCCEEDED(hr))
			{
				LONG nCodecsCount = 0;
				CComPtr<IUOLFoneCodecInfo> pCodecInfo;
				CComBSTR bstrCodecName;
				CComBSTR bstrCodecType;
		
				hr = pCodecInfoList->GetCodecsNumber(&nCodecsCount);

				if (SUCCEEDED(hr))
				{
					for (int nInd = 0; nInd < nCodecsCount; nInd++)
					{
						hr = pCodecInfoList->GetCodecInfoOrderedByType(UOLFONE_CODEC_TYPE_AUDIO, nInd, &pCodecInfo);

						if (FAILED(hr))
						{
							break;
						}
				
						if (pCodecInfo)
						{
							hr = pCodecInfo->GetCodecType(&bstrCodecType);

							if (SUCCEEDED(hr))
							{
								CString strCodecType(bstrCodecType);

								if (0 == strCodecType.CompareNoCase("audio"))
								{
									hr = pCodecInfo->GetCodecName(&bstrCodecName);

									if (SUCCEEDED(hr))
									{
										CString strCodecName(bstrCodecName);
										strCodecName = strCodecName.MakeUpper();

										CAtlMap<CString, CString>::CPair* pPair = mapCodecs.Lookup(strCodecName);

										if (pPair)
										{
											if (bFirst)
											{
												strAudioCodecs = pPair->m_value;
												bFirst = FALSE;
											}
											else
											{
												strAudioCodecs += _T(",") + pPair->m_value;
											}
										}
									}
								}
							}
					
							pCodecInfo.Release();
						}
					}	// for (int nInd = 0; nInd < nCodecsCount; nInd++)
				}

				pCodecInfoList.Release();
			}
		}

		pCodecInfoEnum.Release();
	}

	return strAudioCodecs;
}


BOOL CUOLFoneController::DownloadPhoneValidationRules()
{
	BOOL bRet = FALSE;

	if (m_spUOLFoneServerConfigProvider)
	{
		HRESULT hr = CComObject<CDownloadCallback>::CreateInstance(&m_pDownloadCallback);

		if (SUCCEEDED(hr))
		{
			m_pDownloadCallback->SetCallbackHandler(this);
			m_pDownloadCallback->AddRef();

			CComPtr<IUOLFoneDownloadCallback> pDownloadCallback = m_pDownloadCallback;

			CString strPhoneValidationUrl;
			strPhoneValidationUrl.LoadString(IDS_URL_PHONE_VALIDATION_RULES);

			CComBSTR bstrUrlPhoneValidationRules = strPhoneValidationUrl;

			hr = m_spUOLFoneServerConfigProvider->DownloadPhoneValidationRules(bstrUrlPhoneValidationRules, pDownloadCallback, &m_spPhoneRulesDownload);

			ATLASSERT(SUCCEEDED(hr));

			bRet = SUCCEEDED(hr);

			if (FAILED(hr))
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_RUN_DOWNLOAD_PHONE_RULES_FAILURE, hr);
			}
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_INIT_DOWNLOAD_PHONE_RULES_FAILURE, hr);
		}
	}

	return bRet;
}


BOOL CUOLFoneController::DownloadCodecsList()
{
	BOOL bRet = FALSE;

	if (m_spUOLFoneServerConfigProvider)
	{
		HRESULT hr = CComObject<CCodecsConfigFileUpdaterCallback>::CreateInstance(&m_pCodecsConfigFileUpdaterCallback);

		if (SUCCEEDED(hr))
		{
			m_pCodecsConfigFileUpdaterCallback->SetCallbackHandler(this);
			m_pCodecsConfigFileUpdaterCallback->AddRef();

			// Set registration user id param
			CComQIPtr<IUOLFoneServerConfigProvider4> spUOLFoneServerConfigProvider4 = m_spUOLFoneServerConfigProvider;

			if (spUOLFoneServerConfigProvider4)
			{
				CComPtr<IUOLFoneSettings> pSettings;

				HRESULT hr2 = spUOLFoneServerConfigProvider4->GetSettings(&pSettings);

				if (SUCCEEDED(hr2))
				{
					CUOLFoneAccountPtr pAccount = GetCurrentAccount();

					if (pAccount)
					{
						IUOLMessengerAccountPtr pIMAccount = pAccount->GetIMAccount();

						if (pIMAccount)
						{
							CString strRidPlainValue = pIMAccount->GetUserId();

							CString strRid64Value;
							int nRid64Length = (strRidPlainValue.GetLength() * 3) + 1;
							LPSTR lpszRid64Buffer = strRid64Value.GetBuffer(nRid64Length);

							if (Base64Encode((LPBYTE)(LPCTSTR) strRidPlainValue, strRidPlainValue.GetLength(), lpszRid64Buffer, &nRid64Length))
							{
								lpszRid64Buffer[nRid64Length] = '\0';
								strRid64Value.ReleaseBuffer();

								CComBSTR bstrRidParam = _T("__url_param__rid");
								CComBSTR bstrRidValue = strRid64Value;

								hr2 = pSettings->SetOptionValue(bstrRidParam, bstrRidValue);
								ATLASSERT(SUCCEEDED(hr2));
							}
							else
							{
								strRid64Value.ReleaseBuffer();
							}
						}
					}
				}
			}
			
			// Create codecs config file updater
			CComPtr<IUOLFoneCodecsConfigFileUpdaterCallback> pCodecsConfigFileUpdaterCallback = m_pCodecsConfigFileUpdaterCallback;

			CString strCodecsListUrl;
			strCodecsListUrl.LoadString(IDS_URL_CODECS_LIST);

			CComBSTR bstrCodecsXmlUrl = strCodecsListUrl;

			hr = spUOLFoneServerConfigProvider4->CreateCodecsConfigFileUpdater(
					bstrCodecsXmlUrl, pCodecsConfigFileUpdaterCallback, 
					&m_spCodecsConfigFileUpdater);

			ATLASSERT(SUCCEEDED(hr));

			if (FAILED(hr))
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_GET_CODECS_CONFIG_FILE_UPDATER_INSTANCE_FAILURE, hr);
			}
			else
			{
				CString strUOLFoneClientVersion;
				strUOLFoneClientVersion.LoadString(IDS_UOL_FONE_CLIENT_DLL_VERSION);
				
				hr = m_spCodecsConfigFileUpdater->SetUOLFoneClientVersion(
						strUOLFoneClientVersion.AllocSysString());
				
				if (FAILED(hr))
				{
					ATLTRACE(_T("%s - hr = 0x%x, m_spCodecsConfigFileUpdater->SetUOLFoneClientVersion() failed.\n"), __FUNCTION__, hr);
				}
				
				hr = m_spCodecsConfigFileUpdater->StartUpdate();
				
				ATLASSERT(SUCCEEDED(hr));

				bRet = SUCCEEDED(hr);

				if (FAILED(hr))
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_CODECS_CONFIG_FILE_UPDATER_STARTUPDATE_FAILURE, hr);
				}
			}
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_INIT_CODECS_CONFIG_FILE_UPDATER_FAILURE, hr);
		}
	}

	return bRet;
}


CUOLFoneAccountPtr CUOLFoneController::GetCurrentAccount()
{
	if (m_pUOLFoneAccountManager)
	{
		CUOLFoneAccountPtr pAccount = m_pUOLFoneAccountManager->GetCurrentAccount();

		return pAccount;
	}

	return CUOLFoneAccountPtr();
}


void CUOLFoneController::UpdateNetworkConfig()
{
	HRESULT hr = SetupNetworkConfig();
	
	if ( (SUCCEEDED(hr)) && 
			(m_pUOLFoneAccountManager->IsIMAccountConnected(GetCurrentAccount()->GetIMAccount())) )
	{
		if (m_spPhoneLine)
		{
			// Already initialized and registered.
			// Reset it all.
			//
			m_pConnectionPendingAccount = GetCurrentAccount();
			
			BOOL bOk = DisconnectAccountImpl(m_pConnectionPendingAccount);
			ATLASSERT(bOk);
		}
		else
		{
			// Probably last configuration leaded to a register problem.
			// Start connection with new configuration.
			//
			CUOLFoneAccountPtr pAccount = GetCurrentAccount();
			
			ATLASSERT(pAccount);
			
			BOOL bOk = ConnectAccountImpl(pAccount);
			ATLASSERT(bOk);
			
			//ScheduleTask(WM_USER_REINITIALIZE_PHONELINES_TASK);
		}
	}
}


BOOL CUOLFoneController::IsIMAccountConnected()
{
	return m_pUOLFoneAccountManager->IsIMAccountConnected(GetCurrentAccount()->GetIMAccount());
}


CUOLFoneFailureInfoPtr CUOLFoneController::GetLastFailureInfo()
{
	return m_pLastFailureInfo;
}


void CUOLFoneController::PublishCapabilities(IUOLMessengerAccountPtr pAccount)
{
	if (!m_bCapabilitiesPublished)
	{
		CUOLFoneCapabilitiesHandler capabilitiesHandler;
		
		if (pAccount)
		{
			m_bCapabilitiesPublished = 
					capabilitiesHandler.PublishCapabilities(pAccount);
		}
	}
}


void CUOLFoneController::RevokeCapabilities(IUOLMessengerAccountPtr pAccount)
{
	CUOLFoneCapabilitiesHandler capabilitiesHandler;
	
	if (pAccount)
	{
		BOOL bOk = capabilitiesHandler.RevokeCapabilities(pAccount);
		
		m_bCapabilitiesPublished = FALSE;
	}
}


BOOL CUOLFoneController::HasPendingCallToProceed()
{
	if ( (!m_pPendingCallNumber.IsEmpty()) || (m_pPendingCallContact != NULL) )
	{
		return TRUE;
	}
	
	return FALSE;
}


void CUOLFoneController::FillAccountInscriptions(CUOLFoneAccountPtr pAccount, IUOLFoneClientExtraResponseData2* pExtraResponseData)
{
	if (pExtraResponseData)
	{
		CComPtr<IUOLFoneClientHeaderInfoEnum> pHeaderInfoEnum;

		HRESULT hr = pExtraResponseData->EnumerateExtraHeaders(&pHeaderInfoEnum);

		if ((SUCCEEDED(hr)) && pHeaderInfoEnum)
		{
			CString strProductsHeaderInfo;
			strProductsHeaderInfo.LoadString(IDS_UOL_FONE_INSCRIPTIONS_HEADER);
			
			BOOL bFound = FALSE;

			do
			{
				CComPtr<IUOLFoneClientHeaderInfo> pHeaderInfo;
				ULONG lFetched = 0;

				hr = pHeaderInfoEnum->Next(1, &pHeaderInfo, &lFetched);

				if (FAILED(hr))
				{
					break;
				}

				if (pHeaderInfo)
				{
					CComBSTR bstrName;
					pHeaderInfo->GetName(&bstrName);

					CString strName(bstrName);

					if (0 == strName.CompareNoCase(strProductsHeaderInfo))
					{
						bFound = TRUE;

						CComBSTR bstrValue;
						pHeaderInfo->GetValue(&bstrValue);

						CString strValue(bstrValue);
						pAccount->SetInscriptions(strValue);
					}

					pHeaderInfo.Release();
				}
			}
			while ((hr != S_FALSE) && (FALSE == bFound));
			
			pHeaderInfoEnum.Release();
		}
	}
}



BOOL CUOLFoneController::Connect(CUOLFoneAccountPtr pAccount)
{
	if (pAccount && IsInitialized())
	{
		UOLFONEACCOUNTPARAM ufap = {0};
		ufap.pAccount = pAccount;
		ufap.bReleaseMemory = FALSE;

		return (BOOL) SendMessage(WM_USER_CONNECT_ACCOUNT, (WPARAM) &ufap);
	}

	return FALSE;
}


BOOL CUOLFoneController::Disconnect(CUOLFoneAccountPtr pAccount)
{
	BOOL bRet = FALSE;

	if (pAccount && IsInitialized())
	{
		CUOLFoneAccount::ConnectionState connectionState = pAccount->GetConnectionState();

		if ((CUOLFoneAccount::ConnectionStateOffline != connectionState) &&
			(CUOLFoneAccount::ConnectionStateDisconnectionInProgress != connectionState) &&
			(CUOLFoneAccount::ConnectionStateErrorTimeout != connectionState))
		{
			UOLFONEACCOUNTPARAM ufap = {0};
			ufap.pAccount = pAccount;
			ufap.bReleaseMemory = FALSE;

			if (FALSE == IsCallInProgress(pAccount))
			{
				pAccount->SetConnectionState(CUOLFoneAccount::ConnectionStateDisconnectionInProgress);	

				bRet = (BOOL) SendMessage(WM_USER_DISCONNECT_ACCOUNT, (WPARAM) &ufap);
			}
			else if (m_bIsCallPresenceDependent)
			{
				ATLTRACE(_T("%s - Call in progress, hang-up before disconnect...\n"), __FUNCTION__);

				ATLASSERT(!m_pDisconnectionPendingAccount);
				m_pDisconnectionPendingAccount = pAccount;

				bRet = (BOOL) SendMessage(WM_USER_HANG_UP_CALL, (WPARAM) &ufap);
			}
			else
			{
				if (!CUOLFonePluginConfig::GetInstance()->RegisterOnlyOnCall())
				{
					RevokeCapabilities(pAccount->GetIMAccount());
				}
				
				NotifyAccountConnectionStateChanged(pAccount);
				
				ATLTRACE(_T("%s - Call in progress is not presence dependent. So let it be...\n"), __FUNCTION__);
			}
		}
		else
		{
			ATLTRACE(_T("%s - Account disconnected or disconnection in progress\n"), __FUNCTION__);
		}
	}

	return bRet;
}


BOOL CUOLFoneController::IsConnected(CUOLFoneAccountPtr pAccount)
{
	BOOL bConnected = FALSE;

	if (pAccount)
	{
		bConnected = (pAccount->GetConnectionState() == CUOLFoneAccount::ConnectionStateOnline);
	}

	return bConnected;
}


CString CUOLFoneController::FormatUsername(const CString& strIMAccountUsername)
{
	CString strUsername;

	int nFind = strIMAccountUsername.Find('@');

	if (nFind != -1)
	{
		strUsername = strIMAccountUsername.Left(nFind);

		CString strUOLUserRealm;
		strUOLUserRealm.LoadString(IDS_UOL_REALM);

		CString strRealm = strIMAccountUsername.Mid(nFind+1);

		if (0 != strUOLUserRealm.Compare(strRealm))
		{
			strUsername += _T("+");
			strUsername += strRealm;
		}
	}

	return strUsername;
}


CString CUOLFoneController::FormatURI(const CString& strUsername)
{
	if (strUsername.IsEmpty())
	{
		return _T("");
	}

	CString strURI(_T("sip:"));
	
	BOOL bAppendServerHost = FALSE;

	int nFind = strUsername.Find("@");

	if (nFind != -1)
	{
		CString strUser = strUsername.Left(nFind);

		CString strUOLUserRealm;
		strUOLUserRealm.LoadString(IDS_UOL_REALM);

		CString strRealm = strUsername.Mid(nFind+1);

		if (0 == strUOLUserRealm.Compare(strRealm))
		{
			strURI += strUsername;
		}
		else
		{
			bAppendServerHost = TRUE;
			strURI += strUser + _T("+") + strRealm;			
		}
	}
	else
	{
		bAppendServerHost = TRUE;
		strURI += strUsername;
	}

	if (bAppendServerHost)
	{
		// Append server domain
		if (m_pServerConfig)
		{
			CComPtr<IUOLFoneClientServerInfo> pServerInfo;

			HRESULT hr = m_pServerConfig->GetServerInfo(&pServerInfo);

			if (SUCCEEDED(hr))
			{
				CComBSTR bstrHost;

				hr = pServerInfo->GetServerHost(&bstrHost);

				if (SUCCEEDED(hr))
				{
					strURI = strURI + '@' + CString(bstrHost);
				}
			}
		}
	}

	return strURI;
}


CString CUOLFoneController::FormatDisplayName(const CCallContactParser& callContactParser)
{
	CString strDisplayName; /*= callContactParser.GetDisplayName();

	if (strDisplayName.IsEmpty())*/
	{
		strDisplayName = callContactParser.GetUsername();

		int nFind = strDisplayName.Find('@');
		
		if (nFind != -1)
		{
			LPCTSTR lpszName = strDisplayName.LockBuffer();
			int nInd;

			for (nInd = 0; nInd < nFind; nInd++)
			{
				if (FALSE == ::_istdigit(lpszName[nInd]))
				{
					break;
				}
			}

			strDisplayName.UnlockBuffer();

			if (nInd == nFind)
			{
				strDisplayName = strDisplayName.Left(nFind);
			}
		}
	}

	return strDisplayName;
}


BOOL CUOLFoneController::CheckIdentity(CUOLFoneAccountPtr pSourceAccount, IUOLFoneClientPhoneLine* pTargetPhoneLine)
{
	if (pSourceAccount && pTargetPhoneLine)
	{
		LONG lTargetLineId = CUOLFoneAccount::UNKNOWN_PHONE_LINE_ID;

		HRESULT hr = pTargetPhoneLine->GetPhoneLineId(&lTargetLineId);

		if (SUCCEEDED(hr) && (lTargetLineId != CUOLFoneAccount::UNKNOWN_PHONE_LINE_ID))
		{
			LONG lSourceLineId = pSourceAccount->GetPhoneLineId();

			return (lSourceLineId == lTargetLineId);
		}
	}

	return FALSE;
}


BOOL CUOLFoneController::CheckIdentity(CPhoneCallPtr pSourcePhoneCall, IUOLFoneClientPhoneCall* pTargetPhoneCall)
{
	if (pSourcePhoneCall && pTargetPhoneCall)
	{
		LONG lTargetPhoneCallId = CPhoneCall::UNKNOWN_PHONE_CALL_ID;

		HRESULT hr = pTargetPhoneCall->GetPhoneCallId(&lTargetPhoneCallId);

		if (SUCCEEDED(hr) && (lTargetPhoneCallId != CPhoneCall::UNKNOWN_PHONE_CALL_ID))
		{
			LONG lSourcePhoneCallId = pSourcePhoneCall->GetPhoneCallId();

			return (lSourcePhoneCallId == lTargetPhoneCallId);
		}
	}

	return FALSE;
}


BOOL CUOLFoneController::ConnectAccountImpl(CUOLFoneAccountPtr pAccount)
{
	if ( pAccount->GetConnectionState() == CUOLFoneAccount::ConnectionStateOnline  ||
		 pAccount->GetConnectionState() == CUOLFoneAccount::ConnectionStateConnectionInProgress )
	{
		::OutputDebugString(_T("CUOLFoneController::ConnectAccountImpl() - Account connection already in progress\n"));
		
		if ( (pAccount->GetConnectionState() == CUOLFoneAccount::ConnectionStateOnline) && 
				(!m_bCapabilitiesPublished) )
		{
			NotifyPhoneLineRegistered(pAccount);
			NotifyAccountConnectionStateChanged(pAccount);
		}
		
		return FALSE;
	}
	
	IUOLMessengerAccountPtr pIMAccount;

	if (pAccount)
	{
		pIMAccount = pAccount->GetIMAccount();
	}

	if ((!pIMAccount) || 
		(!m_pServerConfig))
	{
		return FALSE;
	}

	HRESULT hr = E_FAIL;

	if (!m_pUserInfo)
	{
		if (!InitializeUserInfo())
		{
			::OutputDebugString(_T("CUOLFoneController::ConnectAccountImpl() - InitializeUserInfo() failed\n"));

			return FALSE;
		}
	}

	CString strUsername = FormatUsername(pIMAccount->GetUserId());
	CString strPassword = pIMAccount->GetPassword();

	// TODO: remove dummy user/pass!!!
	//if (-1 != (pIMAccount->GetUserId()).Find("mrykoga"))
	//{
	//	strUsername = _T("poetto5");
	//	strPassword = _T("12pedro");
	//}
	//else if (-1 != (pIMAccount->GetUserId()).Find("marcos.koga"))
	//{
	//	strUsername = _T("poetto7");
	//	strPassword = _T("12pedro");
	//}
	//else if (-1 != (pIMAccount->GetUserId()).Find("guilherme.pelliciotti"))
	//{
	//	strUsername = _T("poetto2");
	//	strPassword = _T("12pedro");
	//}
	//else if (-1 != (pIMAccount->GetUserId()).Find("leandroboliveira"))
	//{
	//	strUsername = _T("poetto7");
	//	strPassword = _T("12pedro");
	//}

	CString strDebugMsg;
	strDebugMsg.Format(_T("Connecting user [%s], password is empty [%d]\n"), strUsername, strPassword.IsEmpty());

	::OutputDebugString(strDebugMsg);

	CString strDisplayName = pIMAccount->GetNickName();

	if (-1 != strDisplayName.Find("@"))
	{
		strDisplayName.Empty();
	}

	m_pUserInfo->SetDisplayName(strDisplayName);
	m_pUserInfo->SetUsername(strUsername);
	m_pUserInfo->SetIdentify(strUsername);
	m_pUserInfo->SetPassword(strPassword);
	m_pUserInfo->SetRealm(m_pServerConfig->GetAuthenticationRealm());

	if (!m_pPhoneLineInfo)
	{
		if (!InitializePhoneLineInfo())
		{
			::OutputDebugString(_T("CUOLFoneController::ConnectAccountImpl() - InitializePhoneLineInfo() failed\n"));

			return FALSE;
		}
	}

	UpdateServerConfig();

	hr = m_spUOLFoneClient->RegisterPhoneLine(m_pPhoneLineInfo, m_pNetworkConfig, &m_spPhoneLine);
	ATLASSERT(SUCCEEDED(hr));

	if (SUCCEEDED(hr))
	{
		LONG lPhoneLineId = CUOLFoneAccount::UNKNOWN_PHONE_LINE_ID;

		HRESULT hr2 = m_spPhoneLine->GetPhoneLineId(&lPhoneLineId);

		pAccount->SetPhoneLineId(lPhoneLineId);
		pAccount->SetConnectionState(CUOLFoneAccount::ConnectionStateConnectionInProgress);

		NotifyAccountConnectionStateChanged(pAccount);
	}

	return SUCCEEDED(hr);
}


BOOL CUOLFoneController::DisconnectAccountImpl(CUOLFoneAccountPtr pAccount)
{
	if (m_spPhoneLine)
	{
		if (m_spPhoneCall != NULL)
		{
			ATLASSERT(FALSE);
			ATLTRACE(_T("%s - Phone call should be released first!"), __FUNCTION__);
			
			HRESULT hr2 = m_spUOLFoneClient->ClosePhoneCall(m_spPhoneCall);
			ATLASSERT(SUCCEEDED(hr2));

			m_spPhoneCall.Release();

			pAccount->SetCurrentPhoneCall(CPhoneCallPtr());
		}

		ATLASSERT(m_spUOLFoneClient);

		LONG lPhoneLineState = UFC_PHONE_LINE_STATE_UNKNOWN;
		HRESULT hr = m_spPhoneLine->GetPhoneLineState(&lPhoneLineState);

		if (SUCCEEDED(hr) && lPhoneLineState == UFC_PHONE_LINE_STATE_REGISTERED)
		{
			ATLTRACE(_T("%s - Unregistering phone line"), __FUNCTION__);

			hr = m_spUOLFoneClient->UnregisterPhoneLine(m_spPhoneLine);
			ATLASSERT(SUCCEEDED(hr));

			m_spPhoneLine.Release();

			pAccount->SetConnectionState(CUOLFoneAccount::ConnectionStateDisconnectionInProgress);

			if (!CUOLFonePluginConfig::GetInstance()->RegisterOnlyOnCall())
			{
				RevokeCapabilities(pAccount->GetIMAccount());
			}
			
			NotifyAccountConnectionStateChanged(pAccount);

			return SUCCEEDED(hr);
		}
	}

	return FALSE;
}


BOOL CUOLFoneController::IsValidPhoneNumberImpl(const CString& strPhoneNumber)
{
	ATLASSERT(NULL != m_spPhoneValidator.p);

	BOOL bResult = FALSE;
	m_spPhoneValidator->IsValidPhoneNumber(strPhoneNumber.AllocSysString(), &bResult);
	ATLTRACE(_T("CUOLFoneController::IsValidPhoneNumberImpl result=%d\n"), bResult);

	return bResult;
}


BOOL CUOLFoneController::IsCallInProgress(CUOLFoneAccountPtr pAccount)
{
	CPhoneCallPtr pPhoneCall;

	if (pAccount)
	{
		pPhoneCall = pAccount->GetCurrentPhoneCall();
	}

	return (pPhoneCall != NULL);
}


BOOL CUOLFoneController::MakePhoneCallImpl(IUOLMessengerContactPtr pContact)
{
	CUOLFoneAccountPtr pAccount = GetCurrentAccount();

	if (!pAccount)
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
				CUOLFoneFailureInfo::CALL_UOLFONEACCOUNT_NOT_FOUND_FAILURE, 
				CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
		
		return FALSE;
	}

	CString strPhoneURI = FormatURI(pContact->GetName());

	//if (-1 != strPhoneURI.Find("mrykoga"))
	//{
	//	strPhoneURI = _T("sip:poetto5@sip.uol.com.br");
	//}
	//else if (-1 != strPhoneURI.Find("marcos.koga"))
	//{
	//	strPhoneURI = _T("sip:poetto6@sip.uol.com.br");
	//}
	//else if (-1 != strPhoneURI.Find("guilherme.pelliciotti"))
	//{
	//	strPhoneURI = _T("sip:poetto2@sip.uol.com.br");
	//}
	//else if (-1 != strPhoneURI.Find("leandroboliveira"))
	//{
	//	strPhoneURI = _T("sip:poetto7@sip.uol.com.br");
	//}

	if (strPhoneURI.IsEmpty())
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
				CUOLFoneFailureInfo::CALL_EMPTY_NUMBER_FAILURE, 
				CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
		
		return FALSE;
	}

	ATLASSERT(m_spUOLFoneClient);
	ATLASSERT(m_spPhoneLine);

	if (m_spUOLFoneClient && m_spPhoneLine)
	{
		CComBSTR bstrPhoneURI = strPhoneURI;

		HRESULT hr = m_spUOLFoneClient->MakePhoneCall(m_spPhoneLine, bstrPhoneURI, FALSE, &m_spPhoneCall);

		if (SUCCEEDED(hr))
		{
			ATLASSERT(m_spPhoneCall);

			m_bIsCallPresenceDependent = TRUE;
			
			LONG lPhoneCallId = 0;

			hr = m_spPhoneCall->GetPhoneCallId(&lPhoneCallId);

			if (SUCCEEDED(hr))
			{
				CPhoneCallPtr pPhoneCall = new CPhoneCall();
				pPhoneCall->SetPhoneCallId(lPhoneCallId);

				pAccount->SetCurrentPhoneCall(pPhoneCall);

				return TRUE;
			}
			else
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
						CUOLFoneFailureInfo::CALL_GET_CALL_ID_FAILURE, hr);
			}
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
					CUOLFoneFailureInfo::CALL_MAKE_PHONE_CALL_FAILURE, hr);
		}
	}

	return FALSE;
}


BOOL CUOLFoneController::MakePhoneCallImpl(const CString& strPhoneNumber)
{
	CUOLFoneAccountPtr pAccount = GetCurrentAccount();

	if (!pAccount)
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
				CUOLFoneFailureInfo::CALL_UOLFONEACCOUNT_NOT_FOUND_FAILURE, 
				CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
		
		return FALSE;
	}

	CString strPhoneURI = FormatURI(strPhoneNumber);

	if (strPhoneURI.IsEmpty())
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
				CUOLFoneFailureInfo::CALL_EMPTY_NUMBER_FAILURE, 
				CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
		
		return FALSE;
	}

	ATLASSERT(m_spUOLFoneClient);
	ATLASSERT(m_spPhoneLine);

	if (m_spUOLFoneClient && m_spPhoneLine)
	{
		CComBSTR bstrPhoneURI = strPhoneURI;

		HRESULT hr = m_spUOLFoneClient->MakePhoneCall(m_spPhoneLine, bstrPhoneURI, FALSE, &m_spPhoneCall);

		if (SUCCEEDED(hr))
		{
			ATLASSERT(m_spPhoneCall);

			m_bIsCallPresenceDependent = FALSE;
			
			LONG lPhoneCallId = 0;

			hr = m_spPhoneCall->GetPhoneCallId(&lPhoneCallId);

			if (SUCCEEDED(hr))
			{
				CPhoneCallPtr pPhoneCall = new CPhoneCall();
				pPhoneCall->SetPhoneCallId(lPhoneCallId);

				pAccount->SetCurrentPhoneCall(pPhoneCall);

				return TRUE;
			}
			else
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
						CUOLFoneFailureInfo::CALL_GET_CALL_ID_FAILURE, hr);
			}
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
					CUOLFoneFailureInfo::CALL_MAKE_PHONE_CALL_FAILURE, hr);
		}
	}

	return FALSE;
}


BOOL CUOLFoneController::HangUpPhoneCallImpl(CUOLFoneAccountPtr pAccount)
{
	BOOL bRet = FALSE;

	if (pAccount)
	{
		if (m_spUOLFoneClient && m_spPhoneCall)
		{
			if (CheckIdentity(pAccount->GetCurrentPhoneCall(), m_spPhoneCall))
			{
				HRESULT hr = m_spUOLFoneClient->ClosePhoneCall(m_spPhoneCall);

				ATLTRACE(_T("%s - ClosePhoneCall(), hr = [0x%x]\n"), __FUNCTION__, hr);

				if (SUCCEEDED(hr))
				{
					bRet = TRUE;
				}

				pAccount->SetCurrentPhoneCall(CPhoneCallPtr());
				m_spPhoneCall.Release();
			}
			else
			{
				ATLTRACE(_T("%s - phone call id doesn't match\n!"), __FUNCTION__);
			}
		}
	}

	return bRet;
}


BOOL CUOLFoneController::AcceptIncomingCallImpl(CUOLFoneAccountPtr pAccount)
{
	BOOL bRet = FALSE;

	if (pAccount)
	{
		StopSendRingingNotificationTimer();

		ATLASSERT(m_spUOLFoneClient);
		ATLASSERT(m_spPhoneCall);

		if (m_spUOLFoneClient && m_spPhoneCall)
		{
			HRESULT hr = m_spUOLFoneClient->AcceptPhoneCall(m_spPhoneCall, FALSE);

			ATLASSERT(SUCCEEDED(hr));
			
			if (SUCCEEDED(hr))
			{
				m_bIsCallPresenceDependent = TRUE;
			}

			bRet = SUCCEEDED(hr);
		}
	}

	return bRet;
}


BOOL CUOLFoneController::RejectIncomingCallImpl(CUOLFoneAccountPtr pAccount)
{
	BOOL bRet = FALSE;

	if (pAccount)
	{
		StopSendRingingNotificationTimer();

		ATLASSERT(m_spUOLFoneClient);
		ATLASSERT(m_spPhoneCall);

		if (m_spUOLFoneClient && m_spPhoneCall)
		{
			HRESULT hr = m_spUOLFoneClient->RejectPhoneCall(m_spPhoneCall, UFC_FAILURE_CODE_BUSY);

			if (SUCCEEDED(hr))
			{
				bRet = TRUE;
				pAccount->SetCurrentPhoneCall(CPhoneCallPtr());
			}
		}
	}

	return bRet;
}


BOOL CUOLFoneController::TransferCallImpl(const CString& strPhoneNumber)
{
	CUOLFoneAccountPtr pAccount = GetCurrentAccount();
	
	if (!pAccount)
	{
		return FALSE;
	}
	
	CString strPhoneURI = FormatURI(strPhoneNumber);
	
	if (strPhoneURI.IsEmpty())
	{
		return FALSE;
	}
	
	BOOL bRet = FALSE;
	
	ATLASSERT(m_spUOLFoneClient);
	ATLASSERT(m_spPhoneCall);
	
	if (m_spUOLFoneClient && m_spPhoneCall)
	{
		CComBSTR bstrPhoneURI = strPhoneURI;
		
		HRESULT hr = m_spUOLFoneClient->TransferPhoneCall(m_spPhoneCall, bstrPhoneURI);
		
		if (SUCCEEDED(hr))
		{
			bRet = TRUE;
		}
	}
	
	return bRet;
}


BOOL CUOLFoneController::HoldUpPhoneCallImpl(CUOLFoneAccountPtr pAccount)
{
	BOOL bRet = FALSE;
	
	if (pAccount)
	{
		ATLASSERT(m_spUOLFoneClient);
		ATLASSERT(m_spPhoneCall);
		
		if (m_spUOLFoneClient && m_spPhoneCall)
		{
			HRESULT hr = m_spUOLFoneClient->HoldPhoneCall(m_spPhoneCall);
			
			if (SUCCEEDED(hr))
			{
				bRet = TRUE;
			}
		}
	}
	
	return bRet;
}


BOOL CUOLFoneController::ResumeUpPhoneCallImpl(CUOLFoneAccountPtr pAccount)
{
	BOOL bRet = FALSE;
	
	if (pAccount)
	{
		ATLASSERT(m_spUOLFoneClient);
		ATLASSERT(m_spPhoneCall);
		
		if (m_spUOLFoneClient && m_spPhoneCall)
		{
			HRESULT hr = m_spUOLFoneClient->ResumePhoneCall(m_spPhoneCall);
			
			if (SUCCEEDED(hr))
			{
				bRet = TRUE;
			}
		}
	}
	
	return bRet;
}


BOOL CUOLFoneController::MakeConferenceCallImpl(const CString& strPhoneNumber01, const CString& strPhoneNumber02)
{
	/*
	CUOLFoneAccountPtr pAccount = GetCurrentAccount();
	
	if (!pAccount)
	{
		return FALSE;
	}
	
	CString strPhoneURI_01 = FormatURI(strPhoneNumber01);
	CString strPhoneURI_02 = FormatURI(strPhoneNumber02);
	
	if (strPhoneURI_01.IsEmpty() || strPhoneURI_02.IsEmpty())
	{
		return FALSE;
	}
	
	BOOL bRet = FALSE;
	
	ATLASSERT(m_spUOLFoneClient);
	ATLASSERT(m_spPhoneLine);
	
	if (m_spUOLFoneClient && m_spPhoneLine)
	{
		CComBSTR bstrPhoneURI_01 = strPhoneURI_01;
		CComBSTR bstrPhoneURI_02 = strPhoneURI_02;
		
		HRESULT hr = m_spUOLFoneClient->MakeConferenceCall(m_spPhoneLine, 
				bstrPhoneURI_01, bstrPhoneURI_02, FALSE, 
				&m_spConferenceCall_01, &m_spConferenceCall_02);
		
		if (SUCCEEDED(hr))
		{
			ATLASSERT(m_spConferenceCall_01);
			ATLASSERT(m_spConferenceCall_02);
			
			LONG lPhoneCallId = 0;

			hr = m_spPhoneCall->GetPhoneCallId(&lPhoneCallId);

			if (SUCCEEDED(hr))
			{
				CPhoneCallPtr pPhoneCall = new CPhoneCall();
				pPhoneCall->SetPhoneCallId(lPhoneCallId);
				
				pAccount->SetCurrentPhoneCall(pPhoneCall);
				
				return TRUE;
			}
		}
	}
	
	return bRet;
	*/
	
	return FALSE;
}


BOOL CUOLFoneController::HangUpConferenceCallImpl(CUOLFoneAccountPtr pAccount)
{
	/*
	BOOL bRet = FALSE;
	
	if (pAccount)
	{
		ATLASSERT(m_spUOLFoneClient);
		ATLASSERT(m_spConferenceCall_01);
		ATLASSERT(m_spConferenceCall_02);
		
		if (m_spUOLFoneClient && m_spConferenceCall_01 && m_spConferenceCall_02)
		{
			HRESULT hr = m_spUOLFoneClient->HangUpConferenceCall(m_spPhoneCall);
			
			ATLTRACE(_T("%s - HangUpConferenceCall(), hr = [0x%x]\n"), __FUNCTION__, hr);
			
			if (SUCCEEDED(hr))
			{
				bRet = TRUE;
			}
			
			m_spConferenceCall_01.Release();
			m_spConferenceCall_02.Release();
		}
	}
	
	return bRet;
	*/
	
	return FALSE;
}


void CUOLFoneController::StopSendRingingNotificationTimer()
{
	if (m_nTimerSendRingingNotification != 0)
	{
		KillTimer(m_nTimerSendRingingNotification);
		m_nTimerSendRingingNotification = 0;
		m_nPhoneRingingCount = 0;
	}
}


void CUOLFoneController::UpdateServerConfig()
{
	if (m_pServerConfig)
	{
		CComPtr<IUOLFoneClientServerInfo> pServerInfo;
		CComPtr<IUOLFoneClientServerInfo> pProxyInfo;
		//CComPtr<IUOLFoneClientServerInfo> pHttpTunnelInfo;
		//CComPtr<IUOLFoneClientServerInfo> pHttpProxyInfo;
		//CComPtr<IUOLFoneClientServerInfo> pMediaHttpTunnelInfo;

		ATLASSERT(m_pPhoneLineInfo != NULL);

		HRESULT hr = m_pServerConfig->GetServerInfo(&pServerInfo);

		if (SUCCEEDED(hr))
		{
			hr = m_pPhoneLineInfo->SetServerInfo(pServerInfo);
			ATLASSERT(SUCCEEDED(hr));
		}

		hr = m_pServerConfig->GetProxyInfo(&pProxyInfo);

		if (SUCCEEDED(hr))
		{
			hr = m_pPhoneLineInfo->SetProxyInfo(pProxyInfo);
			ATLASSERT(SUCCEEDED(hr));
		}

		//UpdateNetworkConfig();

/*
		ATLASSERT(m_pNetworkConfig != NULL);

		hr = m_pServerConfig->GetHttpTunnelInfo(&pHttpTunnelInfo);

		if (SUCCEEDED(hr))
		{
			hr = m_pNetworkConfig->SetHttpTunnelInfo(pHttpTunnelInfo);
			ATLASSERT(SUCCEEDED(hr));
		}

		ATLASSERT(m_pMediaNetworkConfig != NULL);

		hr = m_pServerConfig->GetMediaHttpTunnelInfo(&pMediaHttpTunnelInfo);

		if (SUCCEEDED(hr))
		{
			hr = m_pMediaNetworkConfig->SetHttpTunnelInfo(pMediaHttpTunnelInfo);
			ATLASSERT(SUCCEEDED(hr));
		}

		hr = m_pServerConfig->GetHttpProxyInfo(&pHttpProxyInfo);

		if (SUCCEEDED(hr))
		{
			hr = m_pNetworkConfig->SetHttpProxyInfo(pHttpProxyInfo);
			ATLASSERT(SUCCEEDED(hr));

			hr = m_pMediaNetworkConfig->SetHttpProxyInfo(pHttpProxyInfo);
			ATLASSERT(SUCCEEDED(hr));
		}*/
	}
}


void CUOLFoneController::GetDevices(EnumAudioDeviceType audioDeviceType, CAtlList<CString>& listDevices)
{
	CComPtr<IUOLFoneClientDeviceInfoEnum> pDeviceInfoEnum;

	HRESULT hr;

	ATLASSERT(m_spUOLFoneClient);

	listDevices.RemoveAll();

	if (m_spUOLFoneClient)
	{
		hr = m_spUOLFoneClient->EnumerateAudioDevices(audioDeviceType, &pDeviceInfoEnum); 

		while (hr == S_OK)
		{
			CComPtr<IUOLFoneClientDeviceInfo> pDeviceInfo;
			ULONG lFetched = 0;

			hr = pDeviceInfoEnum->Next(1, &pDeviceInfo, &lFetched);

			if (hr == S_OK && pDeviceInfo)
			{
				CComBSTR bstrName;
				pDeviceInfo->GetDeviceName(&bstrName);

				CString strName(bstrName); 
				listDevices.AddTail(strName);

				pDeviceInfo.Release();
			}
		}

		pDeviceInfoEnum.Release();
	}
}


void CUOLFoneController::NotifyPhoneLineRegistered(CUOLFoneAccountPtr pAccount)
{
	if ( (pAccount->GetConnectionState() == CUOLFoneAccount::ConnectionStateOnline) && 
			(IsIMAccountConnected()) )
	{
		if (!m_bCapabilitiesPublished)
		{
			CUOLFoneCapabilitiesHandler capabilitiesHandler;
			
			m_bCapabilitiesPublished = 
					capabilitiesHandler.PublishCapabilities(pAccount->GetIMAccount());
		}
		
		// If application is being finalized this won't be true...
		//
		//ATLASSERT(m_bCapabilitiesPublished);
	}
}


void CUOLFoneController::NotifyInitializationCompleted()
{
	if (m_pControllerCallback)
	{
		m_pControllerCallback->OnInitializationCompleted();
	}
}


void CUOLFoneController::NotifyInitializationFailed(IUOLFoneControllerCallback::InitStatusCode initStatusCode)
{
	if (m_pControllerCallback)
	{
		m_pControllerCallback->OnInitializationFailed(initStatusCode, m_pLastFailureInfo);
	}
}


void CUOLFoneController::NotifyAccountConnectionStateChanged(CUOLFoneAccountPtr pAccount)
{
	if (m_pControllerCallback)
	{
		m_pControllerCallback->OnAccountConnectionStateChanged(pAccount);
	}
}


void CUOLFoneController::NotifyAccountInfoReceived(CUOLFoneAccountPtr pAccount)
{
	if (m_pControllerCallback)
	{
		m_pControllerCallback->OnAccountInfoReceived(pAccount);
	}
}


void CUOLFoneController::NotifyAccountErrorReceived(CUOLFoneAccountPtr pAccount)
{
	if (m_pControllerCallback)
	{
		m_pControllerCallback->OnAccountErrorReceived(pAccount);
	}
}


void CUOLFoneController::NotifyPhoneCallStateChanged(CPhoneCallPtr pPhoneCall)
{
	if (m_pControllerCallback)
	{
		m_pControllerCallback->OnPhoneCallStateChanged(pPhoneCall);
	}
}


void CUOLFoneController::NotifyPhoneCallInfoReceived(CPhoneCallPtr pPhoneCall)
{
	if (m_pControllerCallback)
	{
		m_pControllerCallback->OnPhoneCallInfoReceived(pPhoneCall);
	}
}


void CUOLFoneController::NotifyPhoneCallErrorReceived(CPhoneCallPtr pPhoneCall)
{
	if (m_pControllerCallback)
	{
		m_pControllerCallback->OnPhoneCallErrorReceived(
				pPhoneCall, m_pLastFailureInfo);
	}
}


void CUOLFoneController::NotifyPhoneCallServerErrorReceived(CPhoneCallPtr pPhoneCall)
{
	if (m_pControllerCallback)
	{
		m_pControllerCallback->OnPhoneCallServerErrorReceived(pPhoneCall);
	}
}


void CUOLFoneController::NotifyExecutionFailure(
		IUOLFoneControllerCallback::ExecStatusCode execStatusCode)
{
	if (m_pControllerCallback)
	{
		m_pControllerCallback->OnExecutionFailure(execStatusCode, m_pLastFailureInfo);
	}
}


CString CUOLFoneController::GetInstallCookie()
{
	CString strInstallCookie;

	IUOLMessengerFacade* pFacade = CFacadeWrapper::GetInstance()->GetFacadeInstance();

	if (pFacade)
	{
		GUID guidCookie;
		pFacade->GetInstallCookie(guidCookie);

		CString strGUID = "{00160409-78E1-11D2-B60F-006097C998E7}  ";   // dummy value
		CComBSTR bstrGUID = strGUID;

		CString strTextGUID;

		int nCnvStatus = StringFromGUID2(guidCookie, bstrGUID, bstrGUID.Length());

		if (nCnvStatus > 0) 
		{
			strGUID = bstrGUID;

			strTextGUID = strGUID.Mid(1, strGUID.GetLength()-2);
			strTextGUID = strTextGUID.Mid(0,8) + 
				strTextGUID.Mid(9,4) + 
				strTextGUID.Mid(14,4) + 
				strTextGUID.Mid(19,4) + 
				strTextGUID.Mid(24,12);

			strInstallCookie = strTextGUID;
		}
	}

	return strInstallCookie;
}


HRESULT CUOLFoneController::SetupNetworkConfig()
{
	if (!m_pPhoneLineConfig)
	{
		return E_FAIL;
	}
	
	CComPtr<IUOLFoneClientServerInfo> pHttpTunnelInfo;
	CComPtr<IUOLFoneClientServerInfo> pHttpProxyInfo;
	CComPtr<IUOLFoneClientServerInfo> pMediaHttpTunnelInfo;
	
	LONG lAvailablePort = 0;
	LONG lConnectionTestResult = UFC_CONNECTION_TEST_RESULT_NO_CONNECTION;
	
	HRESULT hr = S_OK;

	if ( (m_spConnectionTestResult) && 
			(!CUOLFonePluginConfig::GetInstance()->GetForceTunnel()) )
	{
		m_spConnectionTestResult->GetAvailablePort(&lAvailablePort);
		m_spConnectionTestResult->GetResult(&lConnectionTestResult);
	}

	// It should already been saved.
	//
	m_pLastFailureInfo.reset();
	
	// Set connection port
	m_pPhoneLineConfig->SetConnectionPort(lAvailablePort);

	// Set signaling server config 
	if (m_pNetworkConfig)
	{
		LONG lServerConnectionFlags = 0;

		m_pPhoneLineConfig->SetServerNetworkConfig(m_pNetworkConfig);

		if ( (CUOLFonePluginConfig::GetInstance()->GetForceTunnel()) || 
				( !(lConnectionTestResult & UFC_CONNECTION_TEST_RESULT_ALLOW_UDP) ) )
		{
			if (m_pServerConfig)
			{
				lServerConnectionFlags |= UFC_CONNECTION_FLAGS_USE_HTTP_TUNNEL;

				if ( m_pServerConfig->IsHttpProxyEnabled() )
				{
					lServerConnectionFlags |= UFC_CONNECTION_FLAGS_USE_HTTP_PROXY;
				}
				
				hr = m_pServerConfig->GetHttpTunnelInfo(&pHttpTunnelInfo);
				ATLASSERT(SUCCEEDED(hr));
				
				if (SUCCEEDED(hr))
				{
					hr = m_pNetworkConfig->SetHttpTunnelInfo(pHttpTunnelInfo);
					ATLASSERT(SUCCEEDED(hr));
				}
				else
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_NETWORK_CONFIG_GET_HTTP_TUNNEL_FAILURE, hr);
				}

				if (SUCCEEDED(hr))
				{
					hr = m_pServerConfig->GetHttpProxyInfo(&pHttpProxyInfo);
					ATLASSERT(SUCCEEDED(hr));
				}
				else if (m_pLastFailureInfo == NULL)
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_NETWORK_CONFIG_SET_HTTP_TUNNEL_FAILURE, hr);
				}
				
				if (SUCCEEDED(hr))
				{
					hr = m_pNetworkConfig->SetHttpProxyInfo(pHttpProxyInfo);
					ATLASSERT(SUCCEEDED(hr));
				}
				else if (m_pLastFailureInfo == NULL)
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_NETWORK_CONFIG_GET_HTTP_PROXY_FAILURE, hr);
				}
				
				if ( (FAILED(hr)) && (m_pLastFailureInfo == NULL) )
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_NETWORK_CONFIG_SET_HTTP_PROXY_FAILURE, hr);
				}
				
				pHttpProxyInfo = NULL;
			}
		}
		else
		{
			hr = m_pNetworkConfig->SetHttpTunnelInfo(NULL);
			ATLASSERT(SUCCEEDED(hr));
			
			if (SUCCEEDED(hr))
			{
				hr = m_pNetworkConfig->SetHttpProxyInfo(NULL);
				ATLASSERT(SUCCEEDED(hr));
			}
			else
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_NETWORK_CONFIG_RESET_HTTP_TUNNEL_FAILURE, hr);
			}

			if ( (FAILED(hr)) && (m_pLastFailureInfo == NULL) )
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_NETWORK_CONFIG_RESET_HTTP_PROXY_FAILURE, hr);
			}
		}

		m_pNetworkConfig->SetConnectionFlags(lServerConnectionFlags);
	}

	// Set media server config
	if ( (m_pMediaNetworkConfig) && (SUCCEEDED(hr)) )
	{
		LONG lMediaServerConnectionFlags = 0;

		m_pPhoneLineConfig->SetMediaNetworkConfig(m_pMediaNetworkConfig);

		if ( (CUOLFonePluginConfig::GetInstance()->GetForceTunnel()) || 
				( !(lConnectionTestResult & UFC_CONNECTION_TEST_RESULT_ALLOW_UDP) ) )
		{
			if (m_pServerConfig)
			{
				lMediaServerConnectionFlags |= UFC_CONNECTION_FLAGS_USE_HTTP_TUNNEL;

				if ( m_pServerConfig->IsHttpProxyEnabled() )
				{
					lMediaServerConnectionFlags |= UFC_CONNECTION_FLAGS_USE_HTTP_PROXY;
				}

				hr = m_pServerConfig->GetMediaHttpTunnelInfo(&pMediaHttpTunnelInfo);
				ATLASSERT(SUCCEEDED(hr));
				
				if (SUCCEEDED(hr))
				{
					hr = m_pMediaNetworkConfig->SetHttpTunnelInfo(pMediaHttpTunnelInfo);
					ATLASSERT(SUCCEEDED(hr));
				}
				else
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_MEDIA_NETWORK_CONFIG_GET_HTTP_TUNNEL_FAILURE, hr);
				}

				if (SUCCEEDED(hr))
				{
					hr = m_pServerConfig->GetHttpProxyInfo(&pHttpProxyInfo);
					ATLASSERT(SUCCEEDED(hr));
				}
				else if (m_pLastFailureInfo == NULL)
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_MEDIA_NETWORK_CONFIG_SET_HTTP_TUNNEL_FAILURE, hr);
				}
				
				if (SUCCEEDED(hr))
				{
					hr = m_pMediaNetworkConfig->SetHttpProxyInfo(pHttpProxyInfo);
					ATLASSERT(SUCCEEDED(hr));
				}
				else if (m_pLastFailureInfo == NULL)
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_MEDIA_NETWORK_CONFIG_GET_HTTP_PROXY_FAILURE, hr);
				}

				if ( (FAILED(hr)) && (m_pLastFailureInfo == NULL) )
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_MEDIA_NETWORK_CONFIG_SET_HTTP_PROXY_FAILURE, hr);
				}
			}
		}
		else
		{
			hr = m_pMediaNetworkConfig->SetHttpTunnelInfo(NULL);
			ATLASSERT(SUCCEEDED(hr));
			
			if (SUCCEEDED(hr))
			{
				hr = m_pMediaNetworkConfig->SetHttpProxyInfo(NULL);
				ATLASSERT(SUCCEEDED(hr));
			}
			else
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_MEDIA_NETWORK_CONFIG_RESET_HTTP_TUNNEL_FAILURE, hr);
			}

			if ( (FAILED(hr)) && (m_pLastFailureInfo == NULL) )
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_MEDIA_NETWORK_CONFIG_RESET_HTTP_PROXY_FAILURE, hr);
			}
		}

		m_pMediaNetworkConfig->SetConnectionFlags(lMediaServerConnectionFlags);
	}
	
	return hr;
}


void CUOLFoneController::ExtractAdditionalErrorInfo(IUOLFoneClientExtraResponseData2* pExtraResponseData)
{
	if (pExtraResponseData)
	{
		CComPtr<IUOLFoneClientHeaderInfoEnum> pHeaderInfoEnum;
		
		HRESULT hr = pExtraResponseData->EnumerateExtraHeaders(&pHeaderInfoEnum);
		
		if ((SUCCEEDED(hr)) && pHeaderInfoEnum)
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
					CUOLFoneFailureInfo::CALL_ERROR_RECEIVED_FAILURE, 
					CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
			
			if (m_pLastFailureInfo)
			{
				do
				{
					CComPtr<IUOLFoneClientHeaderInfo> pHeaderInfo;
					ULONG lFetched = 0;
					
					hr = pHeaderInfoEnum->Next(1, &pHeaderInfo, &lFetched);
					
					if (FAILED(hr))
					{
						break;
					}
					
					if (pHeaderInfo)
					{
						CComBSTR bstrName;
						pHeaderInfo->GetName(&bstrName);
						
						CComBSTR bstrValue;
						pHeaderInfo->GetValue(&bstrValue);
						
						LONG lExtraResponseDataId = 0;
						
						hr = pExtraResponseData->GetExtraResponseDataId(
								bstrName, &lExtraResponseDataId);
						
						if (SUCCEEDED(hr))
						{
							CString strValue(bstrValue);
							
							switch (lExtraResponseDataId)
							{
								case CALL_ERROR_REASON_PHRASE:
									
									m_pLastFailureInfo->AddExtraFailureInfo(
											CUOLFoneFailureInfo::EXTRA_INFO_REASON_PHRASE, 
											strValue);
									
								break;
								
								case CALL_ERROR_STATUS_CODE:
									
									m_pLastFailureInfo->AddExtraFailureInfo(
											CUOLFoneFailureInfo::EXTRA_INFO_STATUS_CODE, 
											strValue);
									
								break;
								
								case CALL_ERROR_ERROR_SOURCE:
									
									m_pLastFailureInfo->AddExtraFailureInfo(
											CUOLFoneFailureInfo::EXTRA_INFO_ERROR_SOURCE, 
											strValue);
									
								break;
								
								case CALL_ERROR_TEXT_INFO:
									
									m_pLastFailureInfo->AddExtraFailureInfo(
											CUOLFoneFailureInfo::EXTRA_INFO_TEXT_INFO, 
											strValue);
									
								break;
							}
						}
						else
						{
							m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
									CUOLFoneFailureInfo::CALL_RESPONSE_ID_NAME_INVALID, 
									CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
							
							break;
						}
						
						pHeaderInfo.Release();
					}
				}
				while (hr != S_FALSE);
			}
			else
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
						CUOLFoneFailureInfo::CALL_CANT_ALLOCATE_ERROR, 
						CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
			}
			
			pHeaderInfoEnum.Release();
		}
		else if (FAILED(hr))
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
					CUOLFoneFailureInfo::CALL_ENUMERATE_HEADERS_FAILURE, hr);
		}
		else
		{
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
					CUOLFoneFailureInfo::CALL_NO_EXTRA_DATA_FAILURE, 
					CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
		}
	}
	else
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildCallFailure(
				CUOLFoneFailureInfo::CALL_NO_EXTRA_DATA_FAILURE, 
				CUOLFoneFailureInfo::DEFAULT_ERROR_CODE);
	}
}


int CUOLFoneController::OsipToUIMLogLevel(LONG lOsipLogLevel)
{
	int iUIMLogLevel = 
			IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_WARNING;
	
	switch (lOsipLogLevel)
	{
		case OSIP_DEBUG_LEVEL_FATAL:
		case OSIP_DEBUG_LEVEL_BUG:
		case OSIP_DEBUG_LEVEL_ERROR:
			
			iUIMLogLevel = IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_ERROR;
			
		break;
		
		case OSIP_DEBUG_LEVEL_WARNING:
			
			iUIMLogLevel = IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_WARNING;
			
		break;
		
		case OSIP_DEBUG_LEVEL_INFO1:
			
			iUIMLogLevel = IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_DEBUG;
			
		break;
		
		case OSIP_DEBUG_LEVEL_INFO2:
		case OSIP_DEBUG_LEVEL_INFO3:
		case OSIP_DEBUG_LEVEL_INFO4:
			
			iUIMLogLevel = IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_INFO;
			
		break;
	}
	
	return iUIMLogLevel;
}


int CUOLFoneController::UIMToOsipLogLevel(int iUIMLogLevel)
{
	int iOsipLogLevel = OSIP_DEBUG_LEVEL_WARNING;
	
	switch (iUIMLogLevel)
	{
		case IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_ERROR:
			
			iOsipLogLevel = OSIP_DEBUG_LEVEL_ERROR;
			
		break;
		
		case IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_WARNING:
			
			iOsipLogLevel = OSIP_DEBUG_LEVEL_WARNING;
			
		break;
		
		case IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_DEBUG:
			
			iOsipLogLevel = OSIP_DEBUG_LEVEL_INFO1;
			
		break;
		
		case IUOLMessengerLogger::GLOBAL_LOGGER_LOG_LEVEL_INFO:
			
			iOsipLogLevel = OSIP_DEBUG_LEVEL_INFO4;
			
		break;
	}
	
	return iOsipLogLevel;
}

