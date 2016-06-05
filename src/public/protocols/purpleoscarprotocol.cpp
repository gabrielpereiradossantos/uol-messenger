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
 
#include "purpleoscarprotocol.h"
#include "oscarconstants.h"

#include <util.h>

#include "purpleutils.h"

using namespace std;


// "You missed %hu message from %s because it was invalid."
// "You missed %hu messages from %s because they were invalid."

// "You missed %hu message from %s because it was too large."
// "You missed %hu messages from %s because they were too large."

// "You missed %hu message from %s because the rate limit has been exceeded."
// "You missed %hu messages from %s because the rate limit has been exceeded."

// "You missed %hu message from %s because he/she was too evil."
// "You missed %hu messages from %s because he/she was too evil."

// "You missed %hu message from %s because you are too evil."
// "You missed %hu messages from %s because you are too evil."

// "You missed %hu message from %s for an unknown reason."
// "You missed %hu messages from %s for an unknown reason."



// "User information not available: %s"

// "Could not add the buddy %s because the screen name is invalid.  Screen names must either start with a letter and contain only letters, numbers and spaces, or contain only numbers."

// "Could not add the buddy %s because you have too many buddies in your buddy list.  Please remove one and try again."

// "Could not add the buddy %s for an unknown reason.  The most common reason for this is that you have the maximum number of allowed buddies in your buddy list."


CPurpleOscarProtocol::OscarStatus CPurpleOscarProtocol::ms_status[] = 
{
	{ OSCAR_STATUS_ICQ_AVAILABLE, "available" },		// Online
	{ OSCAR_STATUS_ICQ_AWAY, "away" },					// Away
	{ OSCAR_STATUS_ICQ_DO_NOT_DISTURB, "dnd"},			// Do Not Disturb
	{ OSCAR_STATUS_ICQ_NOT_AVAILABLE, "na"},			// Not Available
	{ OSCAR_STATUS_ICQ_OCCUPIED, "occupied"},			// Occupied
	{ OSCAR_STATUS_ICQ_FREE_FOR_CHAT, "free4chat"},		// Free For Chat
	{ OSCAR_STATUS_ICQ_INVISIBLE, "invisible"},			// Invisible
	{ OSCAR_STATUS_AIM_AWAY_CUSTOM, "custom" },			// Custom
	// { OSCAR_STATUS_AIM_BACK, "Back"},
	// { OSCAR_STATUS_AIM_VISIBLE, "Visible"},
	// { OSCAR_STATUS_AIM_INVISIBLE, "Invisible"}
};



CPurpleOscarProtocol::ConnectionErrorEntry CPurpleOscarProtocol::ms_pConnectionErrorEntries[] = 
{
	// oscar.c
	{ CONNECTION_ERROR_UNABLE_TO_CONNECT, "Could not connect to authentication server:\n%s" },
	{ CONNECTION_ERROR_UNABLE_TO_CONNECT, "Could not connect to BOS server:\n%s" },
	{ CONNECTION_ERROR_UNABLE_TO_CONNECT, "Couldn't connect to host" },
	{ CONNECTION_ERROR_UNABLE_TO_CONNECT, "The client version you are using is too old. Please upgrade at %s" },
	{ CONNECTION_ERROR_UNABLE_TO_CONNECT, "Could Not Connect" },
	
	{ CONNECTION_ERROR_INVALID_USERNAME, "Unable to login: Could not sign on as %s because the screen name is invalid.  Screen names must either start with a letter and contain only letters, numbers and spaces, or contain only numbers." },
	{ CONNECTION_ERROR_INVALID_USERNAME, "Invalid screen name." },
	
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Incorrect password." },
	
	{ CONNECTION_ERROR_SUSPENDED_ACCOUNT, "Your account is currently suspended." }, 
	
	{ CONNECTION_ERROR_SERVER_UNAVAILABLE, "The AOL Instant Messenger service is temporarily unavailable." },
	
	{ CONNECTION_ERROR_TOO_MANY_ATTEMPTS, "You have been connecting and disconnecting too frequently. Wait ten minutes and try again. If you continue to try, you will need to wait even longer." },
	
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Authentication failed" },
	
	{ CONNECTION_ERROR_SECURITY_ERROR, "The SecurID key entered is invalid." },
	
	{ CONNECTION_ERROR_LOGGED_ON_ANOTHER_LOCATION, "You have signed on from another location." },
	
	{ CONNECTION_ERROR_SERVER_CONNECTION_TERMINATED, "You have been signed off for an unknown reason." }

};




CPurpleOscarProtocol::CPurpleOscarProtocol(const std::string& strProtocolId, const std::string& strProtocolName) :
		CPurpleProtocol(strProtocolId, strProtocolName)
{
}


CPurpleOscarProtocol::~CPurpleOscarProtocol()
{
}


int CPurpleOscarProtocol::GetPrivacyFlags()
{
	return (PRIVACY_FLAGS_HAS_PERMIT_LIST | PRIVACY_FLAGS_HAS_DENIED_LIST);
}


bool CPurpleOscarProtocol::HasReRequestAuthorizationSupport()
{
	return true;
}


bool CPurpleOscarProtocol::IsValidUserName(const string& strUser)
{
	const char* pszUser = strUser.c_str();

	while (*pszUser)
	{
		if (!isdigit(*pszUser))
		{
			return false;
		}

		pszUser++;
	}

	return true;
}


IMProtocolError CPurpleOscarProtocol::CanRequestContactAuthorization(const std::string& strUser, const std::string& strContact, bool& bCanRequest)
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


IMProtocolError CPurpleOscarProtocol::RequestContactAuthorization(const std::string& strUser, const std::string& strContact)
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

				bool bExecute = ExecuteBListNodeAction(pAccount, pNode, "Re-request Authorization");
				
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



int CPurpleOscarProtocol::ProcessSystemMessage(PurpleAccount* pAccount, MessageParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessSystemMessage(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	// oscar.c
	// "Direct IM with %s closed"
	// "Direct IM with %s established"
	// "Attempting to connect to %s at %s:%hu for Direct IM."
	// "Direct IM with %s established"
	// "Asking %s to connect to us at %s:%hu for Direct IM."
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pParam->strFrom.length() > 0) &&
			(pParam->strMessage.length() > 0))
		{
			IMProtocolError protocolError = IM_PROTOCOL_ERROR_SUCCESS;
			//string::size_type indexFind;
			string::size_type npos = -1;

			if ((npos != pParam->strMessage.find("Direct IM with", 0)) &&
				(npos != pParam->strMessage.find("failed", 0)))
			{
				// oscar.c:
				// "Direct IM with %s failed"
				protocolError = IM_PROTOCOL_ERROR_CONNECTION_ERROR;
			}

			if ((protocolError != IM_PROTOCOL_ERROR_SUCCESS) && 
				(pCallback != NULL))
			{
				pCallback->OnIMSendError(strNormalizedUser, pParam->strFrom, protocolError);
				nRet = TRUE;
			}
		}
	}

	return nRet;
}


int CPurpleOscarProtocol::ProcessErrorMessage(PurpleAccount* pAccount, MessageParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessErrorMessage(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	// oscar.c
	// "You have been disconnected from chat room %s."
	// "Your IM Image was not sent. You must be Direct Connected to send IM Images."
	// "Your IM Image was not sent. You cannot send IM Images in AIM chats."
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pParam->strFrom.length() > 0) &&
			(pParam->strMessage.length() > 0))
		{
			IMProtocolError protocolError = IM_PROTOCOL_ERROR_SUCCESS;

			string::size_type npos = -1;
			string strText;
			string strTemp;

			if (!GetLocaleTextFromUTF8(pParam->strMessage.c_str(), strText))
			{
				return FALSE;
			}

			if (npos != strText.find("Unable to send message: ", 0))
			{
				// oscar.c
				// "Unable to send message: %s"
				//   "Invalid error",
				//   "Invalid SNAC",
				//   "Rate to host",
				//   "Rate to client",
				//   "Not logged in",
				//   "Service unavailable",
				//   "Service not defined",
				//   "Obsolete SNAC",
				//   "Not supported by host",
				//   "Not supported by client",
				//   "Refused by client",
				//   "Responses lost",
				//   "Request denied",
				//   "Busted SNAC payload",
				//   "Insufficient rights",
				//   "In local permit/deny",
				//   "Too evil (sender)",
				//   "Too evil (receiver)",
				//   "User temporarily unavailable",
				//   "No match",
				//   "List overflow",
				//   "Request ambiguous",
				//   "Queue full",
				//   "Not while on AOL"
				if (ExtractSubstring(strText.c_str(), "Unable to send message: ", NULL, strTemp))
				{
					if (0 == strTemp.compare("Reply too big"))
					{
						protocolError = IM_PROTOCOL_ERROR_MESSAGE_TOO_LARGE;
					}
					else
					{
						protocolError = IM_PROTOCOL_ERROR_UNKNOWN_SEND_ERROR;
					}
				}
			}

			if ((pCallback != NULL) &&
				(protocolError != IM_PROTOCOL_ERROR_SUCCESS))
			{
				pCallback->OnIMSendError(strNormalizedUser, pParam->strFrom, protocolError);
				nRet = TRUE;
			}
		}
	}

	return nRet;
}


int CPurpleOscarProtocol::ProcessNotifyMessage(PurpleAccount* pAccount, NotifyMessageParam* pParam)
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

	if (!pCallback)
	{
		return nRet;
	}

	if (pParam->type == PURPLE_NOTIFY_MSG_ERROR)
	{
		// oscar.c:
		// pri = "Direct Connect failed"
		// pri = "DirectIM already open."
		// pri = "Unable to open Direct IM"
		// pri = "You have been disconnected from chat room %s.")
		// pri = "Chat is currently unavailable"
		// pri = "You missed %hu message from %s because it was invalid.",
		//       "You missed %hu messages from %s because they were invalid.",
		// pri = "You missed %hu message from %s because it was too large.",
		//		 "You missed %hu messages from %s because they were too large.",
		// pri = "You missed %hu message from %s because the rate limit has been exceeded.",
		//		 "You missed %hu messages from %s because the rate limit has been exceeded.",
		// pri = "You missed %hu message from %s because he/she was too evil.",
		//		 "You missed %hu messages from %s because he/she was too evil.",
		// pri = "You missed %hu message from %s because you are too evil.",
		//		 "You missed %hu messages from %s because you are too evil.",
		// pri = "You missed %hu message from %s for an unknown reason.",
		//		 "You missed %hu messages from %s for an unknown reason.",
		// pri = "SNAC threw error: %s\n"
		// pri = "Unable to send message to %s:"
		// pri = "User information for %s unavailable:"
		// pri = "Rate limiting error."
		// pri = "No results found for email address %s"
		// pri = "Error Changing Account Info"
		// pri = "Unable To Add"
		// sec = "Could not add the buddy %s because the screen name is invalid.  Screen names must either start with a letter and contain only letters, numbers and spaces, or contain only numbers."
		// pri = "Unable To Retrieve Buddy List"
		// pri = "Unable To Add"
		// sec = "Could not add the buddy %s because you have too many buddies in your buddy list.  Please remove one and try again."
		// pri = "Unable To Add"
		// sec = "Could not add the buddy %s for an unknown reason.  The most common reason for this is that you have the maximum number of allowed buddies in your buddy list."
		// pri = "Invalid chat name specified."
		// pri = "The new formatting is invalid."
	}
	else if (pParam->type == PURPLE_NOTIFY_MSG_WARNING)
	{
		// oscar.c:
		// "Purple was unable to get a valid AIM login hash."
		// "Purple was unable to get a valid AIM login hash."
		// "Purple was unable to get a valid login hash."
		// "Your AIM connection may be lost."
		// "Unable to set AIM profile."
		// "Profile too long."
		// "Unable to set AIM away message."
		// "Away message too long."
	}
	else if (pParam->type == PURPLE_NOTIFY_MSG_INFO)
	{
		// oscar.c:
		// pri = "ICQ Server Message"
		// sec = "You have received a special message\n\nFrom: %s [%s]\n%s"

		// pri = "ICQ Page"
		// sec = "You have received an ICQ page\n\nFrom: %s [%s]\n%s"

		// pri = "ICQ Email"
		// sec = "You have received an ICQ email from %s [%s]\n\nMessage is:\n%s"

		// pri = "Account Confirmation Requested"
		// sec = "You should receive an email asking to confirm %s."

		// pri = "Account Info"
		// sec = "Your screen name is currently formatted as follows:\n%s"

		// pri = "Account Info"
		// sec = "The email address for %s is %s"

		// pri = "Authorization Granted"
		// sec = "The user %s has granted your request to add them to your buddy list."

		// pri = "Authorization Denied"
		// sec = "The user %s has denied your request to add them to your buddy list for the following reason:\n%s"

		if ((pParam->strPrimary.length() > 0) && (pParam->strSecondary.length() > 0))
		{
			string strTemp;
			NotifyReason reason = NOTIFY_REASON_AUTHORIZATION_GRANTED;
			bool bNotifyAction = false;

			if ((0 == pParam->strPrimary.compare("ICQ authorization denied.")) || (0 == pParam->strPrimary.compare("Authorization Denied")))
			{
				// pri = "ICQ authorization denied."
				// sec = "The user %u has denied your request to add them to your buddy list for the following reason:\n%s"
				if (ExtractSubstring(pParam->strSecondary.c_str(), "The user ", " has denied your request to add", strTemp))
				{
					reason = NOTIFY_REASON_AUTHORIZATION_DENIED;
					bNotifyAction = true;
				}
			}
			else if ((0 == pParam->strPrimary.compare("ICQ authorization accepted.")) || (0 == pParam->strPrimary.compare("Authorization Granted")))
			{
				// pri = "ICQ authorization accepted."
				// sec = "The user %u has granted your request to add them to your buddy list."
				if (ExtractSubstring(pParam->strSecondary.c_str(), "The user ", " has granted your request to add", strTemp))
				{
					reason = NOTIFY_REASON_AUTHORIZATION_GRANTED;
					bNotifyAction = true;
				}
			}

			if (bNotifyAction)
			{
				pCallback->OnNotifyContactAction(strNormalizedUser, reason, strTemp);
			}
		}
	}

	return nRet;
}


int CPurpleOscarProtocol::ProcessRequestInput(PurpleAccount* pAccount, RequestInputParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessRequestInput(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	// oscar.c:
	// pri = "Enter SecurID"

	// tit = "Buddy Comment for %s"

	// pri = "New screen name formatting:"
	// pri = "Change Address To:"
	// pri = "Search for a buddy by e-mail address"

	// tit = "Available Message:"

	string strNormalizedUser;

	CIMProtocolCallback* pCallback = NULL;
	if (pAccount->username != NULL)
	{
		strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		pCallback = GetCallback(strNormalizedUser.c_str());
	}

	if ((pCallback != NULL) && (pParam->strPrimary.length() > 0))
	{
		RequestReason reason = REQUEST_REASON_AUTHORIZE_MESSAGE;
		bool bRequestInput = false;

		if (0 == pParam->strPrimary.compare("Authorization Request Message:")) 
		{
			// pri = "Authorization Request Message:"
			bRequestInput = true;
			reason = REQUEST_REASON_AUTHORIZE_MESSAGE;
		}
		else if (0 == pParam->strPrimary.compare("Authorization Denied Message:"))
		{
			// pri = "Authorization Denied Message:"
			bRequestInput = true;
			reason = REQUEST_REASON_DENY_MESSAGE;
		}

		if (bRequestInput)
		{
			nRet = TRUE;

			int nBufferLength = 512;
			char* pszBuffer = g_new0(char, nBufferLength + 1);
			
			::strncpy(pszBuffer, "Por favor, me autorize!", nBufferLength);
			
			RequestResult result = pCallback->OnRequestInput(strNormalizedUser, reason, pszBuffer, nBufferLength);
			pszBuffer[nBufferLength] = '\0';

			char* pszUTF8Message = NULL;
			
			if (strlen(pszBuffer))
			{
				pszUTF8Message = purple_utf8_try_convert(pszBuffer);
			}
			else if (pParam->strDefaultValue.length())
			{
				pszUTF8Message = purple_utf8_try_convert(pParam->strDefaultValue.c_str());
			}

			if (result == REQUEST_RESULT_OK)
			{	
				if (pParam->okCallback != NULL)
				{
					((PurpleRequestInputCb) (pParam->okCallback)) (pParam->pUserData, pszUTF8Message);
				}
			}
			else if (pParam->cancelCallback != NULL)
			{
				((PurpleRequestInputCb) (pParam->cancelCallback))(pParam->pUserData, pszUTF8Message);
			}
			
			if (pszUTF8Message)
			{
				g_free(pszUTF8Message);
			}
			
			g_free(pszBuffer);
		}
	}

	return nRet;
}



int CPurpleOscarProtocol::ProcessRequestAction(PurpleAccount* pAccount, RequestActionParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessRequestAction(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	// oscar.c:
	// pri = "You have selected to open a Direct IM connection with %s."
	// pri = "%s has just asked to directly connect to %s"

	// pri = "ICQ user %u has sent you a buddy: %s (%s)"
	// sec = "Do you want to add this buddy to your buddy list?"
	string strNormalizedUser;

	CIMProtocolCallback* pCallback = NULL;
	if (pAccount->username != NULL)
	{
		strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		pCallback = GetCallback(strNormalizedUser.c_str());
	}

	if ((pCallback != NULL) && 
		(pParam->strPrimary.length() > 0) && 
		(pParam->strSecondary.length() > 0))
	{
		string strTemp;
		GCallback cbOK;
		GCallback cbCancel;

		if (0 == pParam->strPrimary.compare("Authorization Request"))
		{
			// oscar.c
			// pri = "Authorization Request"
			// sec = "The user %s wants to add you to their buddy list for the following reason:\n%s"

			// pri = "Authorization Request"
			// sec = "The user %u wants to add you to their buddy list for the following reason:\n%s"
			if (pParam->vActionCallback.size() == 2)
			{
				cbOK = pParam->vActionCallback[0];
				cbCancel = pParam->vActionCallback[1];

				if (ExtractSubstring(pParam->strSecondary.c_str(), "The user ", " wants to add you to their buddy list", strTemp))
				{
					string strContactId;
					if (!ExtractSubstring(strTemp.c_str(), NULL, " (", strContactId))
					{
						strContactId = strTemp;
					}

					string strReason;
					if (ExtractSubstring(pParam->strSecondary.c_str(), "for the following reason:\n", NULL, strReason))
					{
						// Use new callback notification if available
						CIMProtocolCallback3* pCallback3 = dynamic_cast<CIMProtocolCallback3*>(pCallback);

						if (pCallback3)
						{
							nRet = ProcessRequestUserAuthorizationCallback(pCallback3, strNormalizedUser, strContactId, strReason, cbOK, cbCancel, pParam->pUserData, -1);
						}
						else
						{
							nRet = ProcessRequestActionCallback(pCallback, &CIMProtocolCallback::OnRequestAddUser, strNormalizedUser, strContactId, strReason, cbOK, cbCancel, pParam->pUserData, -1);
						}
					}
				}
			}
		}
		else if (0 == pParam->strPrimary.compare("Request Authorization"))
		{
			// pri = "Request Authorization"
			// sec = "The user %s requires authorization before being added to a buddy list.  Do you want to send an authorization request?"
			if (pParam->vActionCallback.size() == 2)
			{
				cbOK = pParam->vActionCallback[0];
				cbCancel = pParam->vActionCallback[1];

				if (ExtractSubstring(pParam->strSecondary.c_str(), "The user ", " requires authorization before being added", strTemp))
				{
					nRet = ProcessRequestActionCallback(pCallback, &CIMProtocolCallback::OnRequestSendAuthorization, strNormalizedUser, strTemp, cbOK, cbCancel, pParam->pUserData, 0);
				}
			}
		}
		else if (0 == pParam->strPrimary.compare("Authorization Given"))
		{
			// pri = "Authorization Given"
			// sec = "The user %s has given you permission to add you to their buddy list.  Do you want to add them?"
			if (pParam->vActionCallback.size() == 2)
			{
				cbOK = pParam->vActionCallback[0];
				cbCancel = pParam->vActionCallback[1];

				//if (ExtractSubstring(pszSecondary, "The user ", " has given you permission to add you to their buddy list", strTemp))
				//{
				//	nRet = ProcessRequestAction(pCallback, CIMProtocolCallback::OnRequestAddContact, strTemp, cbOK, cbCancel, pUserData);
				//}
			}
		}
	}

	return nRet;
}


bool CPurpleOscarProtocol::GetStatusText(int nStatus, string& strStatusText)
{
	bool bValidStatus = false;

	int nInd = nStatus - OSCAR_STATUS_ICQ_AVAILABLE;

	if ((nInd < (sizeof(ms_status) / sizeof(ms_status[0]))) &&
		(ms_status[nInd].status == nStatus))
	{
		strStatusText = ms_status[nInd].statusText;
		bValidStatus = true;
	}

	return bValidStatus;
}


bool CPurpleOscarProtocol::GetStatusCode(const std::string& statusText, int& nStatus)
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


void CPurpleOscarProtocol::ProcessExtraSettings(PurpleAccount* pAccount, CProtocolSettings* pExtraSettings)
{
	if (pExtraSettings)
	{
		// Get "Web aware" option
		bool bWebAware = false;
		pExtraSettings->GetBoolean("web_aware", bWebAware);
		purple_account_set_bool(pAccount, "web_aware", bWebAware);

		// Get "Request Authorization" option
		bool bRequestAuthorization = true;
		pExtraSettings->GetBoolean("authorization", bRequestAuthorization);
		purple_account_set_bool(pAccount, "authorization", bRequestAuthorization);
	}
}


string CPurpleOscarProtocol::FormatSendingMessage(const string& strMessage)
{
	string strRet = StripHTML(strMessage);

	return strRet;
}


string CPurpleOscarProtocol::FormatSentMessage(const string& strMessage)
{
	string strRet = strMessage;

	// Build links <A href=""></A>
	char* pszLinkifiedText = purple_markup_linkify(strMessage.c_str());

	if (pszLinkifiedText != NULL)
	{
		// Replace \n with <br>
		gchar* pszHTMLText = purple_strdup_withhtml(pszLinkifiedText);

		if (pszHTMLText != NULL)
		{
			strRet = pszHTMLText;
			g_free(pszHTMLText);
		}

		g_free(pszLinkifiedText);
	}

	return strRet;
}


/*
string CPurpleOscarProtocol::FormatReceivedMessage(const string& strMessage)
{
	return FormatSentMessage(strMessage);
}
*/


string CPurpleOscarProtocol::StripHTML(const string& strMessage)
{
	string strRet;

	if (strMessage.length() > 0)
	{	
		const char* lpszHTML = strMessage.c_str();
	
		while (*lpszHTML != NULL)
		{
			if (*lpszHTML == '<')
			{				
				++lpszHTML;

				if (0 == ::strnicmp(lpszHTML, "br>", 3))
				{
					// Replace <br> with \n
					strRet += '\n';
					lpszHTML += 3;
				}
				else
				{
					// Discard html tags...
					while ((*lpszHTML != '\0') && (*lpszHTML != '>'))
					{
						++lpszHTML;
					}

					if (*lpszHTML == '>')
					{
						++lpszHTML;
					}
				}
			}
			else if (0 == ::strnicmp(lpszHTML, "&nbsp;", 6))
			{
				// Replace &nbsp; with ' '
				strRet += ' ';
				lpszHTML += 6;
			}
			else
			{
				strRet += *lpszHTML;
				++lpszHTML;
			}
		}
	}

	return strRet;
}


bool CPurpleOscarProtocol::GetConnectionErrorCode(ConnectionError& errorCode, const char* pszReason)
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