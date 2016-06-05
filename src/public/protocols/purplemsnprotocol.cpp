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
 
#include "purplemsnprotocol.h"
#include "msnconstants.h"

#include <string>
#include <glib-object.h>

#include <util.h>

#include "purpleutils.h"


using namespace std;


#define CONVERSATION_WINDOW_CLOSED_MESSAGE	"has closed the conversation window."
#define CONVERSATION_TIMEOUT_MESSAGE		"The conversation has become inactive and timed out."
#define CONVERSTAION_UNABLE_TO_SEND_MESSAGE	"Unable to send message."
#define CONVERSATION_NUDGE_RECEIVED_MESSAGE	"has nudged you!"
#define CONVERSATION_NUDGE_SENDING_MESSAGE	"Nudging"

// switchboard.c
#define SB_ERROR_TIMEOUT		"Message may have not been sent because a timeout occurred:"
#define SB_ERROR_INVISIBLE		"Message could not be sent, not allowed while invisible"
#define SB_ERROR_OFFLINE		"Message could not be sent because the user is offline:"
#define SB_ERROR_CONN_ERROR		"Message could not be sent because a connection error occurred:"
#define SB_ERROR_SWITCHBOARD	"Message could not be sent because an error with the switchboard occurred:"
#define SB_ERROR_UNKNOWN		"Message may have not been sent because an unknown error occurred:"


const char* CPurpleMSNProtocol::PURPLE_MSN_PLUGIN_ID = "prpl-msn";
const char* CPurpleMSNProtocol::PURPLE_MSN_PROTOCOL_NAME = "MSN";


CPurpleMSNProtocol::MSNStatus CPurpleMSNProtocol::ms_status[] = 
{
	{ MSN_STATUS_AVAILABLE, "available" },				// Available
	{ MSN_STATUS_AWAY, "away" },						// Away From Computer
	{ MSN_STATUS_BE_RIGHT_BACK, "brb" },				// Be Right Back
	{ MSN_STATUS_BUSY, "busy" },						// Busy
	{ MSN_STATUS_ON_PHONE, "phone" },					// On The Phone
	{ MSN_STATUS_OUT_TO_LUNCH, "lunch" },				// Out To Lunch
	{ MSN_STATUS_HIDDEN, "invisible" },					// Invisible
	{ MSN_STATUS_OFFLINE, "offline" },					// Offline
	{ MSN_STATUS_MOBILE, "mobile" },					// Mobile (available to receive SMS)
};


CPurpleMSNProtocol::ConnectionErrorEntry CPurpleMSNProtocol::ms_pConnectionErrorEntries[] = 
{
	// servconn.c
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Connection error from Notification server" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Connection error from Switchboard server" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Unknown error" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Unable to connect" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Writing error" },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Reading error" },

	// session.c
	{ CONNECTION_ERROR_SERVER_UNAVAILABLE, "The MSN servers are temporarily unavailable. Please wait and try again." },
	{ CONNECTION_ERROR_SERVER_UNAVAILABLE, "The MSN servers are going down temporarily." },
	{ CONNECTION_ERROR_LOGGED_ON_ANOTHER_LOCATION, "You have signed on from another location." },
	{ CONNECTION_ERROR_PARSE_ERROR, "Error parsing HTTP."},
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Unable to authenticate: " },
	{ CONNECTION_ERROR_UNAVAILABLE_BUDDY_LIST, "Your MSN buddy list is temporarily unavailable. Please wait and try again." },

	// error.c
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Authentication failed" },

	// msn.c
	{ CONNECTION_ERROR_SSL_SUPPORT_NEEDED, "SSL support is needed for MSN. Please install a supported SSL library. See http://purple.sf.net/faq-ssl.php for more information." },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Failed to connect to server." },
};




CPurpleMSNProtocol::CPurpleMSNProtocol() :
	CPurpleProtocol(PURPLE_MSN_PLUGIN_ID, PURPLE_MSN_PROTOCOL_NAME)
{
}


CPurpleMSNProtocol::~CPurpleMSNProtocol()
{
	m_lastNudgeSendingTimeMap.clear();
	
	m_lastNudgeReceivingTimeMap.clear();
}


bool CPurpleMSNProtocol::HasContactServerAliasSupport()
{
	return false;
}


int CPurpleMSNProtocol::GetPrivacyFlags()
{
	return (PRIVACY_FLAGS_HAS_PERMIT_LIST | PRIVACY_FLAGS_HAS_DENIED_LIST | 
			PRIVACY_FLAGS_NO_LIST_INTERSECTION);
}


bool CPurpleMSNProtocol::IsValidUserName(const string& strUser)
{
	return (purple_email_is_valid(strUser.c_str()) != 0);
}


int CPurpleMSNProtocol::ProcessSystemMessage(PurpleAccount* pAccount, MessageParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessSystemMessage(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	// Look for msn.c defined messages...
	string::size_type indexFind;
	string::size_type npos = -1;

	indexFind = pParam->strMessage.find(CONVERSATION_WINDOW_CLOSED_MESSAGE, 0);

	string strNormalizedUser;

	CIMProtocolCallback* pCallback = NULL;
	if (pAccount->username != NULL)
	{
		strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		pCallback = GetCallback(strNormalizedUser.c_str());
	}

	if (pCallback != NULL)
	{
		if (indexFind != npos)
		{
			pCallback->OnConversationWindowClosed(strNormalizedUser, pParam->strFrom);
			nRet = TRUE;
		}
		else if ((indexFind = pParam->strMessage.find(CONVERSATION_TIMEOUT_MESSAGE, 0)) != npos)
		{
			pCallback->OnConversationTimeout(strNormalizedUser, pParam->strFrom);
			nRet = TRUE;
		}
		else if ((indexFind = pParam->strMessage.find(CONVERSATION_NUDGE_RECEIVED_MESSAGE, 0)) != npos)
		{
			// Check whether we received a nudge once from this contact, and whether the 
			// necessary time has already passed.
			CLastNudgeReceivingTimeMap::iterator itLastNudgeReceivingTime;
			itLastNudgeReceivingTime = m_lastNudgeReceivingTimeMap.find(pParam->strFrom);
			
			time_t tCurrentTime;
			struct tm* tmComparingTime;
			
			::time(&tCurrentTime);
			tmComparingTime = ::localtime(&tCurrentTime);
			tmComparingTime->tm_sec -= NUDGE_DEFAULT_RECEIVING_COOLDOWN_SECS;
			
			if ( (itLastNudgeReceivingTime != m_lastNudgeReceivingTimeMap.end()) && 
					(::mktime(tmComparingTime) <= itLastNudgeReceivingTime->second) )
			{
				nRet = FALSE;
			}
			else
			{
				// Stores new last nudge time.
				std::string strFromUser = pParam->strFrom;
				m_lastNudgeReceivingTimeMap[strFromUser] = tCurrentTime;
				
				CIMProtocolCallback4* pCallback4 = dynamic_cast<CIMProtocolCallback4*>(pCallback);
				
				if (pCallback4)
				{
					pCallback4->OnNudgeReceived(strNormalizedUser, pParam->strFrom, pParam->strMessage);
					nRet = TRUE;
				}
			}
		}
		else if ((indexFind = pParam->strMessage.find(CONVERSATION_NUDGE_SENDING_MESSAGE, 0)) != npos)
		{
			// Do nothing... Already displayed.
			nRet = TRUE;
		}
	}

	return nRet;
}


int CPurpleMSNProtocol::ProcessErrorMessage(PurpleAccount* pAccount, MessageParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessErrorMessage(pAccount, pParam);
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
		(pParam->strFrom.length() > 0) &&
		(pParam->strMessage.length() > 0))
	{
		IMProtocolError protocolError = IM_PROTOCOL_ERROR_SUCCESS;
		//string::size_type indexFind;
		string::size_type npos = -1;

		if (npos != pParam->strMessage.find(SB_ERROR_OFFLINE, 0))
		{
			protocolError = IM_PROTOCOL_ERROR_RECEIPT_NOT_ONLINE;
		}
		else if (npos != pParam->strMessage.find(SB_ERROR_INVISIBLE, 0))
		{
			protocolError = IM_PROTOCOL_ERROR_SENDER_NOT_ONLINE;
		}
		else if (npos != pParam->strMessage.find(SB_ERROR_TIMEOUT, 0))
		{
			protocolError = IM_PROTOCOL_ERROR_OPERATION_TIMEOUT;
		}
		else if ((npos != pParam->strMessage.find(SB_ERROR_CONN_ERROR, 0)) ||
				 (npos != pParam->strMessage.find(SB_ERROR_SWITCHBOARD, 0)) ||
				 (npos != pParam->strMessage.find(SB_ERROR_UNKNOWN, 0)))
		{
			protocolError = IM_PROTOCOL_ERROR_CONNECTION_ERROR;
		}

		if (protocolError != IM_PROTOCOL_ERROR_SUCCESS)
		{
			pCallback->OnIMSendError(strNormalizedUser, pParam->strFrom, protocolError);
			nRet = TRUE;
		}
	}

	return nRet;
}


int CPurpleMSNProtocol::ProcessNotifyMessage(PurpleAccount* pAccount, NotifyMessageParam* pParam)
{
	return CPurpleProtocol::ProcessNotifyMessage(pAccount, pParam);
/*
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessNotifyMessage(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	if (pParam->type == PURPLE_NOTIFY_MSG_INFO)
	{

	}
	else if (pParam->type == PURPLE_NOTIFY_MSG_WARNING)
	{
	}
	else if (pParam->type == PURPLE_NOTIFY_MSG_ERROR)
	{
		// error.c:
		// pri = "MSN Error: %s\n"
		// "Unable to parse message"
		// "Syntax Error (probably a Purple bug)"
		// "Invalid email address"
		// "User does not exist"
		// "Fully Qualified Domain Name missing"
		// "Already Logged In"
		// "Invalid Username"
		// "Invalid Friendly Name"
		// "List Full"
		// "Already there"
		// "Not on list"
		// "User is offline"
		// "Already in the mode"
		// "Already in opposite list"
		// "Too many groups"
		// "Invalid group"
		// "User not in group"
		// "Group name too long"
		// "Cannot remove group zero"
		// "Tried to add a contact to a group that doesn't exist"
		// "Switchboard failed"
		// "Notify Transfer failed"
		// "Required fields missing"
		// "Too many hits to a FND"
		// "Not logged in"
		// "Service Temporarily Unavailable"
		// "Database server error"
		// "Command disabled"
		// "File operation error"
		// "Memory allocation error"
		// "Wrong CHL value sent to server"
		// "Server busy"
		// "Server unavailable"
		// "Peer Notification server down"
		// "Database connect error"
		// "Server is going down (abandon ship)"
		// "Server unavailable"
		// "Error creating connection"
		// "CVR parameters are either unknown or not allowed"
		// "Unable to write"
		// "Session overload"
		// "User is too active"
		// "Too many sessions"
		// "Passport not verified"
		// "Bad friend file"
		// "Not expected"
		// "Friendly name changes too rapidly"
		// "Server too busy"
		// "Authentication failed"
		// "Not allowed when offline"
		// "Server unavailable"
		// "Not accepting new users"
		// "Kids Passport without parental consent"
		// "Passport account not yet verified"
		// "Bad ticket"
		// "Unknown Error Code %d"

		// msn.c
		// pri = "Your new MSN friendly name is too long."

		// notification.c
		// pri = "%s on %s (%s)"
		// sec = "%s is not a valid group."

		// pri = "Unable to add user on %s (%s)"),
		//		 "Unable to block user on %s (%s)"),
		//		 "Unable to permit user on %s (%s)"),
		// sec = "%s could not be added because your buddy list is full.",
		//		 "%s is not a valid passport account.",
		//		 "Unknown error."
		//
	}

	return nRet;
*/
}


int CPurpleMSNProtocol::ProcessRequestInput(PurpleAccount* pAccount, RequestInputParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessRequestInput(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	// msn.c:
	// pri = "Set your friendly name."
	// pri = "Set your home phone number."
	// pri = "Set your work phone number."
	// pri = "Set your mobile phone number."
	// pri = "Send a mobile message."

	return nRet;
}


int CPurpleMSNProtocol::ProcessRequestAction(PurpleAccount* pAccount, RequestActionParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessRequestAction(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	// dialog.c
	// pri = "Buddy list synchronization issue in %s (%s)"
	// sec = "%s on the local list is inside the group \"%s\" but not on the server list. Do you want this buddy to be added?",
	//       ""%s is on the local list but not on the server list. Do you want this buddy to be added?"
	//

	// msn.c:
	// pri = "Allow MSN Mobile pages?"
	// sec = "Do you want to allow or disallow people on your buddy list to send you MSN Mobile pages to your cell phone or other mobile device?"

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
		string::size_type indexFind;
		string::size_type npos = -1;
		string strTemp;
		GCallback cbOK;
		GCallback cbCancel;
		
		if (ExtractSubstring(pParam->strPrimary.c_str(), "The user ", " wants to add", strTemp))
		{
			// userlist.c:
			// pri = "The user %s (%s) wants to add %s to his or her buddy list.",
			//       "The user %s wants to add %s to his or her buddy list."
			if (pParam->vActionCallback.size() == 2)
			{
				cbOK = pParam->vActionCallback[0];
				cbCancel = pParam->vActionCallback[1];

				indexFind = strTemp.find(" (", 0);

				if (indexFind != npos)
				{
					strTemp = strTemp.substr(0, indexFind);
				}

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


bool CPurpleMSNProtocol::HasCommandSupport(const std::string& strCmdId)
{
	// Just one command implemented by now.
	// So just one comparition.
	//
	return (strCmdId.compare(NUDGE_COMMAND_ID) == 0);
}


bool CPurpleMSNProtocol::FreeToSendCommand(const std::string& strCmdId, const std::string& strFromUser, const std::string& strToUser, IMProtocolError& blockError)
{
	//if (strCmdId.compare(NUDGE_COMMAND_ID) == 0)
	//{
	//	// Check whether we sent a nudge once for this contact, and whether the 
	//	// necessary time has already passed.
	//	CLastNudgeSendingTimeMap::iterator itLastNudgeSendingTime;
	//	itLastNudgeSendingTime = m_lastNudgeSendingTimeMap.find(strToUser);
	//	
	//	time_t tCurrentTime;
	//	struct tm* tmComparingTime;
	//	
	//	::time(&tCurrentTime);
	//	tmComparingTime = ::localtime(&tCurrentTime);
	//	tmComparingTime->tm_sec -= NUDGE_DEFAULT_SENDING_COOLDOWN_SECS;
	//	
	//	if ( (itLastNudgeSendingTime != m_lastNudgeSendingTimeMap.end()) && 
	//			(::mktime(tmComparingTime) <= itLastNudgeSendingTime->second) )
	//	{
	//		blockError = IM_PROTOCOL_ERROR_WAIT_COMMAND_COOLDOWN;
	//		
	//		return false;
	//	}
	//	
	//	// Stores new last nudge time.
	//	m_lastNudgeSendingTimeMap[strToUser] = tCurrentTime;
	//}
	
	return true;
}


std::string CPurpleMSNProtocol::GetProtocolCommandName(const std::string& strCmdId)
{
	if (strCmdId.compare(NUDGE_COMMAND_ID) == 0)
	{
		// The name is the same of the id.
		// So... let's return it.
		//
		return NUDGE_COMMAND_ID;
	}
	
	return "";
}


bool CPurpleMSNProtocol::HasChatSupport()
{
	return true;
}


std::string CPurpleMSNProtocol::GetChatInviteMenuName()
{
    return "Initiate _Chat";
}


bool CPurpleMSNProtocol::HasCustomEmoticonSupport()
{
	return true;
}


bool CPurpleMSNProtocol::HasUserAliasSupport()
{
	return true;
}


bool CPurpleMSNProtocol::GetStatusText(int nStatus, string& strStatusText)
{
	bool bValidStatus = false;

	int nInd = nStatus - MSN_STATUS_AVAILABLE;

	if ((nInd < (sizeof(ms_status) / sizeof(ms_status[0]))) &&
		(ms_status[nInd].status == nStatus))
	{
		strStatusText = ms_status[nInd].statusText;
		bValidStatus = true;
	}

	return bValidStatus;
}


bool CPurpleMSNProtocol::GetStatusCode(const std::string& statusText, int& nStatus)
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



string CPurpleMSNProtocol::FormatSendingMessage(const string& strMessage)
{
	// Don't change HTML
	return strMessage;
}


string CPurpleMSNProtocol::FormatSentMessage(const string& strMessage)
{
	string strRet = strMessage;

	// Replace \n with <br>
	gchar* pszHTMLText = purple_strdup_withhtml(strMessage.c_str());

	if (pszHTMLText != NULL)
	{
		strRet = pszHTMLText;

		g_free(pszHTMLText);
	}

	return strRet;
}

/*
string CPurpleMSNProtocol::FormatReceivedMessage(const string& strMessage)
{
	return FormatSentMessage(strMessage);
}
*/

bool CPurpleMSNProtocol::GetConnectionErrorCode(ConnectionError& errorCode, const char* pszReason)
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
