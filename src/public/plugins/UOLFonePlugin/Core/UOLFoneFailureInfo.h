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


class CUOLFoneFailureInfo
{
public:
	
	enum
	{
		DEFAULT_ERROR_CODE = -1,
		
		FAILURE_TYPE_DOWNLOAD = 0, 
		FAILURE_TYPE_SYSTEM, 
		FAILURE_TYPE_INITIALIZATION, 
		FAILURE_TYPE_LIBRARY, 
		FAILURE_TYPE_CALL
	};
	
	enum ExtraFailureInfoId
	{
		EXTRA_INFO_FUNCTION_NAME,
		EXTRA_INFO_PARAMETER_NAME,
		EXTRA_INFO_PATH,
		EXTRA_INFO_URL,
		EXTRA_INFO_HTTP_STATUS_CODE,
		EXTRA_INFO_HTTP_STATUS_MESSAGE,
		EXTRA_INFO_ERROR_DISPLAY_URL,
		
		// Call error exclusive info...
		//
		EXTRA_INFO_REASON_PHRASE,
		EXTRA_INFO_STATUS_CODE,
		EXTRA_INFO_ERROR_SOURCE,
		EXTRA_INFO_TEXT_INFO,
	};
	
	enum SystemFailureCode
	{
		SYSTEM_PATH_CREATION_FAILURE,
		SYSTEM_FILE_CREATION_FAILURE,
		SYSTEM_FILE_WRITING_FAILURE
	};
	
	enum InitializationFailureCode
	{
		INITIALIZATION_INVALID_PARAM_FAILURE,
	};
	
	enum LibraryFailureCode
	{
		// Loading errors.
		//
		LIBRARY_LOAD_UOLFONECLIENT_FAILURE,						// DLL nao carregou.
		LIBRARY_LOAD_UOLFONESERVERCONFIG_FAILURE,				// DLL nao carregou.
		
		// Initialization and allocation errors.
		//
		LIBRARY_COINITIALIZE_FAILURE,							// CoInitialize falhou.
		LIBRARY_CREATE_UOLFONECLIENT_FAILURE,					// CoCreateInstance() falhou.
		LIBRARY_CREATE_UOLFONESERVERCONFIG_FAILURE,				// CoCreateInstance() falhou.
		LIBRARY_INSTANTIATE_UOLFONECLIENT_FAILURE,				// GetUOLFoneClientInstance() falhou.
		LIBRARY_INSTANTIATE_UOLFONESERVERCONFIG_FAILURE,		// CoInitialize() falhou.
		LIBRARY_INSTANTIATE_UOLFONECLIENTCALLBACK_FAILURE,		// CreateInstance() falhou.
		LIBRARY_INIT_UOLFONECLIENT_FAILURE,						// Initialize() falhou.
		LIBRARY_INIT_UOLFONESERVERCONFIG_FAILURE,				// Initialize() falhou.
		LIBRARY_INIT_SERVER_MESSAGE_HANDLER_FAILURE, 			// CServerMessageHandler Initialize() falhou.
		LIBRARY_INIT_SERVER_CONFIG_FAILURE,						// InitializeServerConfig() falhou.
		LIBRARY_INIT_NETWORK_CONFIG_FAILURE,					// InitializeNetworkConfig() falhou.
		LIBRARY_INIT_DOWNLOAD_PHONE_RULES_FAILURE,				// CDownloadCallback CreateInstance() falhou.
		LIBRARY_INIT_CODECS_CONFIG_FILE_UPDATER_FAILURE,		// CCodecsConfigFileUpdaterCallback CreateInstance() falhou.
		LIBRARY_INIT_CODECS_UPDATER_FAILURE,					// CCodecsUpdaterCallback CreateInstance() falhou.
		LIBRARY_TEST_CONNECTION_CREATE_PARAM_FAILURE,
		LIBRARY_CREATE_PHONE_LINE_CONFIG_FAILURE,
		LIBRARY_INITIALIZE_PHONE_LINES_FAILURE,
		
		// Complement updates errors.
		//
		LIBRARY_RUN_DOWNLOAD_PHONE_RULES_FAILURE,				// DownloadPhoneValidationRules() falhou.
		LIBRARY_GET_DOWNLOAD_STATUS_PHONE_RULES_FAILURE,		// IUOLFoneDownload GetStatus() falhou.
		LIBRARY_DOWNLOAD_PHONE_RULES_ERROR_FAILURE,				// Download status retrieved indicates error.
		LIBRARY_GET_PHONE_RULES_VALIDATOR_FAILURE,				// GetPhoneValidator() falhou.
		LIBRARY_GET_CODECS_CONFIG_FILE_UPDATER_INSTANCE_FAILURE,// CreateCodecsConfigFileUpdater() falhou.
		LIBRARY_GET_CODECS_UPDATER_INSTANCE_FAILURE,			// CreateCodecsUpdater() falhou.
		LIBRARY_CODECS_CONFIG_FILE_UPDATER_STARTUPDATE_FAILURE,
		LIBRARY_CODECS_UPDATER_STARTUPDATE_FAILURE,
		LIBRARY_CODECS_CONFIG_FILE_UPDATER_GET_FILE_PATH_FAILURE,
		LIBRARY_CODECS_CONFIG_FILE_UPDATER_GET_ERROR_FAILURE,	// Updater returned error though HandleUpdateConfigComplete().
		LIBRARY_CODECS_CONFIG_FILE_UPDATER_GET_STATUS_FAILURE,	// Updater returned status though HandleUpdateConfigComplete().
		LIBRARY_CODECS_CONFIG_FILE_UPDATER_XML_PARSE_FAILURE,
		LIBRARY_CODECS_CONFIG_FILE_UPDATER_MISSING_PARAM_FAILURE,
		LIBRARY_CODECS_CONFIG_FILE_UPDATER_UNKNOWN_FAILURE,
		LIBRARY_CODECS_CONFIG_FILE_UPDATER_GET_ADDITIONAL_ERROR_FAILURE,
		LIBRARY_CODECS_UPDATER_GET_ERROR_FAILURE,				// Updater returned error though HandleUpdateConfigComplete().
		LIBRARY_CODECS_UPDATER_GET_STATUS_FAILURE,				// Updater returned status though HandleUpdateConfigComplete().
		LIBRARY_CODECS_UPDATER_PROCESS_XML_FAILURE,
		LIBRARY_CODECS_UPDATER_MISSING_PARAM_FAILURE,
		LIBRARY_CODECS_UPDATER_UNKNOWN_FAILURE,
		LIBRARY_CODECS_UPDATER_GET_ADDITIONAL_ERROR_FAILURE,
		
		// Execution and setting errors.
		//
		LIBRARY_SET_OUTPUT_AUDIO_DEVICE_FAILURE, 
		LIBRARY_SET_INPUT_AUDIO_DEVICE_FAILURE, 
		LIBRARY_NETWORK_CONFIG_GET_HTTP_TUNNEL_FAILURE, 
		LIBRARY_NETWORK_CONFIG_SET_HTTP_TUNNEL_FAILURE, 
		LIBRARY_NETWORK_CONFIG_GET_HTTP_PROXY_FAILURE, 
		LIBRARY_NETWORK_CONFIG_SET_HTTP_PROXY_FAILURE, 
		LIBRARY_NETWORK_CONFIG_RESET_HTTP_TUNNEL_FAILURE, 
		LIBRARY_NETWORK_CONFIG_RESET_HTTP_PROXY_FAILURE, 
		LIBRARY_MEDIA_NETWORK_CONFIG_GET_HTTP_TUNNEL_FAILURE, 
		LIBRARY_MEDIA_NETWORK_CONFIG_SET_HTTP_TUNNEL_FAILURE, 
		LIBRARY_MEDIA_NETWORK_CONFIG_GET_HTTP_PROXY_FAILURE, 
		LIBRARY_MEDIA_NETWORK_CONFIG_SET_HTTP_PROXY_FAILURE, 
		LIBRARY_MEDIA_NETWORK_CONFIG_RESET_HTTP_TUNNEL_FAILURE, 
		LIBRARY_MEDIA_NETWORK_CONFIG_RESET_HTTP_PROXY_FAILURE, 
		
		// Connection test errors.
		//
		LIBRARY_TEST_CONNECTION_GET_SERVER_INFO_FAILURE,
		LIBRARY_TEST_CONNECTION_SET_SERVER_INFO_FAILURE,
		LIBRARY_TEST_CONNECTION_GET_HTTP_TUNNEL_INFO_FAILURE,
		LIBRARY_TEST_CONNECTION_SET_HTTP_TUNNEL_INFO_FAILURE,
		LIBRARY_TEST_CONNECTION_GET_HTTP_PROXY_INFO_FAILURE,
		LIBRARY_TEST_CONNECTION_SET_HTTP_PROXY_INFO_FAILURE,
		LIBRARY_TEST_CONNECTION_TEST_FAILURE,
		LIBRARY_TEST_CONNECTION_GETRESULT_FAILURE,
		LIBRARY_TEST_CONNECTION_NO_CONNECTION_FAILURE,

		// Added on version 1.0.0.21 to not cause changes on 
		// sequence above.
		LIBRARY_CODECS_SET_UOLFONECLIENT_VERSION_FAILURE,
	};
	
	enum CallFailureCode
	{
		// Normal conditions...
		//
		CALL_ERROR_RECEIVED_FAILURE,
		CALL_SERVER_ERROR_RECEIVED_FAILURE,
		CALL_ERROR_UNDEFINED_FAILURE,
		CALL_MESSAGE_ALLOC_FAILURE,
		CALL_UOLFONEACCOUNT_NOT_FOUND_FAILURE,
		CALL_EMPTY_NUMBER_FAILURE,
		CALL_MAKE_PHONE_CALL_FAILURE,
		CALL_GET_CALL_ID_FAILURE,
		
		// Error getting additional data...
		//
		CALL_NO_EXTRA_DATA_FAILURE,
		CALL_ENUMERATE_HEADERS_FAILURE,
		CALL_CANT_ALLOCATE_ERROR,
		CALL_RESPONSE_ID_NAME_INVALID
	};
	
public:
	
	virtual ~CUOLFoneFailureInfo();
	
	UINT GetFailureType();
	DWORD GetErrorCode();
	DWORD GetAdditionalErrorCode();
	
	CUOLFoneFailureInfo* Clone();
	
	void AddExtraFailureInfo(ExtraFailureInfoId extraInfoId, CString strExtraInfo);
	void AddExtraFailureInfo(ExtraFailureInfoId extraInfoId, DWORD dwExtraInfo);
	
	CString GetExtraFailureInfoStr(ExtraFailureInfoId extraInfoId);
	DWORD GetExtraFailureInfoDWORD(ExtraFailureInfoId extraInfoId);
	
	CString GetFailureInfoText();
	
	BOOL IsFailureImmutable();
	
	// Handy methods.
	//
	static CUOLFoneFailureInfo* BuildDownloadFailure(
			DWORD dwErrorCode, DWORD dwAdditionalErrorCode, 
			CString strDownloadUrl);
	
	static CUOLFoneFailureInfo* BuildDownloadFailure(DWORD dwErrorCode, 
			DWORD dwAdditionalErrorCode, CString strDownloadUrl, 
			DWORD dwHTTPStatusCode, CString strHTTPStatusMessage);
	
	static CUOLFoneFailureInfo* BuildSystemFailure(DWORD dwErrorCode, 
			DWORD dwAdditionalErrorCode);
	
	static CUOLFoneFailureInfo* BuildSystemFailure(DWORD dwErrorCode, 
			DWORD dwAdditionalErrorCode, CString strFailedPath);
	
	static CUOLFoneFailureInfo* BuildInitializationFailure(
			DWORD dwErrorCode);
	
	static CUOLFoneFailureInfo* BuildInitializationFailure(DWORD dwErrorCode, 
			CString strFunction, CString strInvalidParameter);
	
	static CUOLFoneFailureInfo* BuildLibraryFailure(DWORD dwErrorCode, 
			DWORD dwAdditionalErrorCode);
	
	static CUOLFoneFailureInfo* BuildCallFailure(DWORD dwErrorCode, 
			DWORD dwAdditionalErrorCode);
	
protected:
	
	enum ExtraInfoType
	{
		EXTRA_INFO_TYPE_STR,
		EXTRA_INFO_TYPE_DWORD
	};
	
	struct ExtraInfo
	{
		int			iExtraInfoType;
		CString		strExtraInfo;
		DWORD		dwExtraInfo;
	};
	
	MAKEAUTOPTR(ExtraInfo);
	
	CUOLFoneFailureInfo(UINT nFailureType, DWORD dwErrorCode, 
			DWORD dwAdditionalErrorCode);
	
	ExtraInfoPtr AllocExtraInfo();
	
private:
	
	UINT		m_nFailureType;
	DWORD		m_dwErrorCode;
	DWORD		m_dwAdditionalErrorCode;
	
	typedef CAtlMap<ExtraFailureInfoId, ExtraInfoPtr>	CExtraInfoMap;
	CExtraInfoMap		m_mapExtraInfo;
};

MAKEAUTOPTR(CUOLFoneFailureInfo);

