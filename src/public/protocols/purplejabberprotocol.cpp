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
 
#include "purplejabberprotocol.h"
#include "jabberconstants.h"
#include "filetransferinfo.h"

#include <string>
#include <glib-object.h>

#include <util.h>

#include "purpleutils.h"


using namespace std;


const char* CPurpleJabberProtocol::PURPLE_JABBER_PLUGIN_ID = "prpl-jabber";
const char* CPurpleJabberProtocol::PURPLE_JABBER_PROTOCOL_NAME = "Jabber";
const char* CPurpleJabberProtocol::PURPLE_JABBER_DEFAULT_RESOURCE = "UOLM";
char CPurpleJabberProtocol::PURPLE_JABBER_RESOURCE_SPLITTER = '/';


CPurpleJabberProtocol::JabberStatus CPurpleJabberProtocol::ms_status[] = 
{
	{ JABBER_STATUS_ONLINE, "available" },
	{ JABBER_STATUS_CHATTY, "freeforchat" },
	{ JABBER_STATUS_AWAY, "away" },
	{ JABBER_STATUS_EXTENDED_AWAY, "extended_away" },
	{ JABBER_STATUS_DO_NOT_DISTURB, "dnd" },
	{ JABBER_STATUS_UNAVAILABLE, "offline" },
	{ JABBER_STATUS_ERROR, "error" }
};


CPurpleJabberProtocol::ConnectionErrorEntry CPurpleJabberProtocol::ms_pConnectionErrorEntries[] = 
{
	// auth.c
	{ CONNECTION_ERROR_SSL_SUPPORT_NEEDED, "Server requires TLS/SSL for login.  Select \"Use TLS if available\" in account properties" },
	{ CONNECTION_ERROR_SSL_SUPPORT_NEEDED, "Server requires TLS/SSL for login.  No TLS/SSL support found." },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Server requires plaintext authentication over an unencrypted stream" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Invalid response from server." },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Server does not use any supported authentication method" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Bad Request" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Conflict" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Feature Not Implemented" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Forbidden" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Gone" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Internal Server Error" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Item Not Found" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Malformed Jabber ID" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Not Acceptable" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Not Allowed" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Not Authorized" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Password is required to sign on." },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Payment Required" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Recipient Unavailable" },
	{ CONNECTION_ERROR_REGISTRATION_REQUIRED, "Registration Required" },
	{ CONNECTION_ERROR_SERVER_UNAVAILABLE, "Remote Server Not Found" },
	{ CONNECTION_ERROR_SERVER_UNAVAILABLE, "Remote Server Timeout" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Server Overloaded" },
	{ CONNECTION_ERROR_SERVER_UNAVAILABLE, "Service Unavailable" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Subscription Required" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Unexpected Request" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Unknown Error" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Authorization Aborted" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Incorrect encoding in authorization" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Invalid authzid" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Invalid Authorization Mechanism" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Authorization mechanism too weak" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Not Authorized" },
	{ CONNECTION_ERROR_SERVER_UNAVAILABLE, "Temporary Authentication Failure" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Authentication Failure" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "SASL authentication failed\n" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Bad Format" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Bad Namespace Prefix" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Resource Conflict" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Connection Timeout" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Host Gone" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Host Unknown" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Improper Addressing" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Internal Server Error" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Invalid ID" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Invalid Namespace" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Invalid XML" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Non-matching Hosts" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Not Authorized" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Policy Violation" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Remote Connection Failed" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Resource Constraint" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Restricted XML" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "See Other Host" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "System Shutdown" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Undefined Condition" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Unsupported Encoding" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Unsupported Stanza Type" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Unsupported Version" },
	{ CONNECTION_ERROR_PARSE_ERROR, "XML Not Well Formed" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Stream Error" },
	{ CONNECTION_ERROR_PARSE_ERROR, "Invalid challenge from server" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Unauthorized" },

	// jabber.c
	{ CONNECTION_ERROR_UNABLE_TO_CONNECT, "Error initializing session" },
	{ CONNECTION_ERROR_UNABLE_TO_CONNECT, "Couldn't connect to host" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Write error" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Read Error" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Registration Failed" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Already Registered" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Error initializing session" },
	{ CONNECTION_ERROR_SSL_SUPPORT_NEEDED, "Connection Failed" },
	{ CONNECTION_ERROR_SSL_SUPPORT_NEEDED, "SSL Handshake Failed" },
	{ CONNECTION_ERROR_SSL_SUPPORT_NEEDED, "SSL support unavailable" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Invalid Jabber ID" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Invalid XMPP ID" },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Invalid XMPP ID. Domain must be set." },
	{ CONNECTION_ERROR_UNABLE_TO_CONNECT, "Unable to create socket" },
	{ CONNECTION_ERROR_PARSE_ERROR, "XML Parse error" },
	{ CONNECTION_ERROR_UNABLE_TO_CONNECT, "Could not establish a connection with the server:\n" },
};




CPurpleJabberProtocol::CPurpleJabberProtocol() :
	CPurpleProtocol(PURPLE_JABBER_PLUGIN_ID, PURPLE_JABBER_PROTOCOL_NAME)
{
}

CPurpleJabberProtocol::CPurpleJabberProtocol(const string& strProtocolId, const string& strProtocolName) :
	CPurpleProtocol(strProtocolId, strProtocolName)
{
}

CPurpleJabberProtocol::~CPurpleJabberProtocol()
{
}


bool CPurpleJabberProtocol::HasContactBlockingSupport()
{
	return false;
}


bool CPurpleJabberProtocol::HasReRequestAuthorizationSupport()
{
	return true;
}


bool CPurpleJabberProtocol::AutoAddContactOnAuthorization()
{
	return true;
}


std::string CPurpleJabberProtocol::GetDefaultContactGroup()
{
	return string("Buddies");
}


bool CPurpleJabberProtocol::IsValidUserName(const string& strUser)
{
	return (purple_email_is_valid(strUser.c_str()) != 0);
}


IMProtocolError CPurpleJabberProtocol::CanRequestContactAuthorization(const std::string& strUser, const std::string& strContact, bool& bCanRequest)
{
	bCanRequest = false;

	if ((strUser.length() == 0) ||
		(strContact.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	// Check if account is connected
	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleUser.c_str());
	if (pAccount == NULL)
	{
		return IM_PROTOCOL_ERROR_NOT_CONNECTED;
	}

	IMProtocolError retError = IM_PROTOCOL_ERROR_FAILED;

	char* pszUTF8Contact = purple_utf8_try_convert(strContact.c_str());

	if (pszUTF8Contact != NULL)
	{
		char* pszNormalizedUTF8Contact = g_strdup(purple_normalize(pAccount, pszUTF8Contact));

		if (pszNormalizedUTF8Contact != NULL)
		{
			// Check if contact exists...
			PurpleBuddy* pBuddy = purple_find_buddy(pAccount, pszNormalizedUTF8Contact);

			if ((pBuddy != NULL) && 
				(pAccount->gc != NULL) && (pAccount->gc->prpl != NULL))
			{
				PurplePluginProtocolInfo *prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(pAccount->gc->prpl);

				if (prpl_info && (prpl_info->status_text))
				{
					char* pszStatusText = (prpl_info->status_text)(pBuddy);

					string strNotAuthorized("Not Authorized");
					if (pszStatusText != 0)
					{
						if (0 == strNotAuthorized.compare(pszStatusText))
						{
							bCanRequest = true;
						}

						g_free(pszStatusText);
					}

					retError = IM_PROTOCOL_ERROR_SUCCESS;
				}
			}

			g_free(pszNormalizedUTF8Contact);
		}

		g_free(pszUTF8Contact);
	}

	return retError;
}


IMProtocolError CPurpleJabberProtocol::RequestContactAuthorization(const std::string& strUser, const std::string& strContact)
{
	if ((strUser.length() == 0) ||
		(strContact.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	// Check if account is connected
	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleUser.c_str());
	if (pAccount == NULL)
	{
		return IM_PROTOCOL_ERROR_NOT_CONNECTED;
	}

	IMProtocolError retError = IM_PROTOCOL_ERROR_FAILED;

	char* pszUTF8Contact = purple_utf8_try_convert(strContact.c_str());

	if (pszUTF8Contact != NULL)
	{
		char* pszNormalizedUTF8Contact = g_strdup(purple_normalize(pAccount, pszUTF8Contact));

		if (pszNormalizedUTF8Contact != NULL)
		{
			// Check if contact exists...
			PurpleBuddy* pBuddy = purple_find_buddy(pAccount, pszNormalizedUTF8Contact);

			if (pBuddy != NULL)
			{
				PurpleBlistNode* pNode = (PurpleBlistNode*) pBuddy;

				bool bExecute = ExecuteBListNodeAction(pAccount, pNode, "(Re-)Request authorization");
				
				if (bExecute)
				{
					retError = IM_PROTOCOL_ERROR_SUCCESS;
				}
			}

			g_free(pszNormalizedUTF8Contact);
		}

		g_free(pszUTF8Contact);
	}

	return retError;
}


void CPurpleJabberProtocol::OnContactSignedOn(PurpleAccount* pAccount, PurpleBuddy* pBuddy)
{
	// First we execute normal SignOn routines.
	//
	CPurpleProtocol::OnContactSignedOn(pAccount, pBuddy);
	
	// Now, considering that Jabber don't receive ChangeStatus events during 
	// initialization, we force the execution of "OnContactStatusChanged" for 
	// each Signed On account.
	//
	CPurpleProtocol::OnContactStatusChanged(pAccount, pBuddy);
}


int CPurpleJabberProtocol::ProcessRequestAction(PurpleAccount* pAccount, RequestActionParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessRequestAction(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	// ...

	string strNormalizedUser;
	CIMProtocolCallback* pCallback = NULL;

	if (pAccount->username != NULL)
	{
		strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		pCallback = GetCallback(strNormalizedUser.c_str());
	}

	if ((NULL != pCallback) && 
		(NULL != pParam) && 
		(pParam->strPrimary.length() > 0))
	{
		//string::size_type indexFind;
		//string::size_type npos = -1;
		string strTemp;
		GCallback cbOK;
		GCallback cbCancel;
		
		if (ExtractSubstring(pParam->strPrimary.c_str(), "The user ", " wants to add you", strTemp))
		{
			// presence.c:
			// pri = "The user %s wants to add you to their buddy list.",
			if (pParam->vActionCallback.size() == 2)
			{
				strTemp = GetNormalizedContactName(strTemp.c_str());

				cbOK = pParam->vActionCallback[0];
				cbCancel = pParam->vActionCallback[1];

				// Use new callback notification if available
				CIMProtocolCallback3* pCallback3 = dynamic_cast<CIMProtocolCallback3*>(pCallback);

				if (pCallback3)
				{
					nRet = ProcessRequestUserAuthorizationCallback(pCallback3, strNormalizedUser, strTemp, "", cbOK, cbCancel, pParam->pUserData, -1);
				}
				else
				{
					nRet = ProcessRequestActionCallback(pCallback, &CIMProtocolCallback::OnRequestAddContact, strNormalizedUser, strTemp, cbOK, cbCancel, pParam->pUserData, -1);
				}
			}
		}
	}

	return nRet;
}


bool CPurpleJabberProtocol::GetStatusText(int nStatus, string& strStatusText)
{
	bool bValidStatus = false;

	int nInd = nStatus - JABBER_STATUS_ONLINE;

	if ((nInd < (sizeof(ms_status) / sizeof(ms_status[0]))) &&
		(ms_status[nInd].status == nStatus))
	{
		strStatusText = ms_status[nInd].statusText;
		bValidStatus = true;
	}

	return bValidStatus;
}


bool CPurpleJabberProtocol::GetStatusCode(const std::string& statusText, int& nStatus)
{
	bool bValidStatus = false;

	int nMax = sizeof(ms_status) / sizeof(ms_status[0]);
	for (int nInd = 0; nInd < nMax; nInd++)
	{
		if (0 == statusText.compare(ms_status[nInd].statusText))
		{
			nStatus = ms_status[nInd].status;
			bValidStatus = true;
			break;
		}
	}

	return bValidStatus;
}



void CPurpleJabberProtocol::ProcessExtraSettings(PurpleAccount* pAccount, CProtocolSettings* pExtraSettings)
{
	if (pExtraSettings)
	{
		int nPort = 5222;
		string strServer;

		pExtraSettings->GetInteger(JABBER_SETTING_CONNECT_PORT, nPort);
		pExtraSettings->GetString(JABBER_SETTING_CONNECT_SERVER, strServer);

		//purple_account_set_bool(pAccount, "auth_plain_in_clear", TRUE);
		//purple_account_set_bool(pAccount, "use_tls", FALSE);
		purple_account_set_int(pAccount, JABBER_SETTING_CONNECT_PORT, nPort);
		purple_account_set_string(pAccount, JABBER_SETTING_CONNECT_SERVER, strServer.c_str());
	}
}


string CPurpleJabberProtocol::GetNormalizedPurpleUserName(const string& strUserName)
{
	string strNormalizedName = strUserName;

	string::size_type indexFind;
	string::size_type npos = -1;

	indexFind = strNormalizedName.find(PURPLE_JABBER_RESOURCE_SPLITTER);

	if (indexFind != npos)
	{
		strNormalizedName = strNormalizedName.erase(indexFind);
	}

	strNormalizedName += PURPLE_JABBER_RESOURCE_SPLITTER;
	strNormalizedName += PURPLE_JABBER_DEFAULT_RESOURCE;

	return strNormalizedName;
}


string CPurpleJabberProtocol::GetNormalizedCallbackUserName(const char* pszPurpleUserName)
{
	string strNormalizedName = pszPurpleUserName;

	string::size_type indexFind;
	string::size_type npos = -1;

	indexFind = strNormalizedName.find(PURPLE_JABBER_RESOURCE_SPLITTER);

	if (indexFind != npos)
	{
		strNormalizedName = strNormalizedName.erase(indexFind);
	}

	return strNormalizedName;
}


string CPurpleJabberProtocol::GetNormalizedContactName(const char* pszContact)
{
	return GetNormalizedCallbackUserName(pszContact);
}


bool CPurpleJabberProtocol::GetConnectionErrorCode(ConnectionError& errorCode, const char* pszReason)
{
	bool bFind = CPurpleProtocol::GetConnectionErrorCode(errorCode, pszReason);

	if ((!bFind) && (pszReason != NULL))
	{
		string strReason = pszReason;

		int nEntryCount = sizeof(ms_pConnectionErrorEntries) / sizeof(ms_pConnectionErrorEntries[0]);

		const string::size_type npos = -1;

		for (int nInd = 0; nInd < nEntryCount; nInd++)
		{
			if (npos != strReason.find(ms_pConnectionErrorEntries[nInd].pszText))
			{
				errorCode = ms_pConnectionErrorEntries[nInd].errorCode;
				bFind = true;
				break;
			}
		}
	}

	return bFind;
}


int CPurpleJabberProtocol::ProcessNotifyFormatted(PurpleAccount* pAccount, NotifyFormattedParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessNotifyFormatted(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	string strNormalizedUser;

	CIMProtocolCallback* pCallback = NULL;
	if (pAccount->username != NULL)
	{
		strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		pCallback = GetCallback(strNormalizedUser.c_str());
	}

	if ((pCallback != NULL) &&
		 pParam->strSecondary.length() > 0 )
	{
		IMProtocolError protocolError = IM_PROTOCOL_ERROR_SUCCESS;

		string strNormalizedContact;

		if ( FALSE != ExtractSubstring(pParam->strSecondary.c_str(), "delivery to ", " failed", strNormalizedContact) )
		{
			strNormalizedContact = GetNormalizedContactName(strNormalizedContact.c_str());
			protocolError = IM_PROTOCOL_ERROR_RECEIPT_NOT_ONLINE;
		}

		if (protocolError != IM_PROTOCOL_ERROR_SUCCESS)
		{
			pCallback->OnIMSendError(strNormalizedUser, strNormalizedContact, protocolError);
			nRet = TRUE;
		}
	}

	return nRet;
}


int CPurpleJabberProtocol::ProcessNotifyMessage(PurpleAccount* pAccount, NotifyMessageParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessNotifyMessage(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	string strNormalizedUser;

	CIMProtocolCallback* pCallback = NULL;
	if (pAccount->username != NULL)
	{
		strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		pCallback = GetCallback(strNormalizedUser.c_str());
	}

	if ((pCallback != NULL) && (pParam->type == PURPLE_NOTIFY_MSG_ERROR))
	{
		string strPrimary;
		string strSecondary;
		string strNormalizedUser;

		if (pParam->strPrimary.length() > 0)
		{
			GetLocaleTextFromUTF8(pParam->strPrimary.c_str(), strPrimary);
		}

		if (pParam->strSecondary.length() > 0)
		{
			GetLocaleTextFromUTF8(pParam->strSecondary.c_str(), strSecondary);
		}
				
		if (pAccount->username != NULL)
		{
			strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);
		}

		if (pParam->strPrimary.length() > 0)
		{
			if (0 == strPrimary.compare("File Send Failed"))
			{
				// si.c:
				// pri = "File Send Failed"
				// sec = "Unable to send file to %s, user does not support file transfers"
				// user@gmail.com/Talk.v1040037CD30
				
				IMProtocolError protocolError = IM_PROTOCOL_ERROR_SUCCESS;

				string strContact;

				if ( FALSE != ExtractSubstring(pParam->strSecondary.c_str(), "send file to ", ", user", strContact) )
				{
					string strNormalizedContact = GetNormalizedContactName(strContact.c_str());
					pCallback->OnFileTransferError(strNormalizedUser, strNormalizedContact, FILE_TRANSFER_ERROR_UNSUPORTED);
					nRet = CPurpleProtocol::CancelFileTransfers(strNormalizedUser, strContact);
				}
			}
		}
	}



	return nRet;
}