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
 
#include "purpleyahooprotocol.h"
#include "yahooconstants.h"

#include <string>
#include <glib-object.h>

#include <util.h>

#include "purpleutils.h"
#include "critseclock.h"


using namespace std;


const char* CPurpleYahooProtocol::PURPLE_YAHOO_PLUGIN_ID = "prpl-yahoo";
const char* CPurpleYahooProtocol::PURPLE_YAHOO_PROTOCOL_NAME = "Yahoo";


CPurpleYahooProtocol::YahooStatus CPurpleYahooProtocol::ms_status[] = 
{
	{ YAHOO_STATUS_AVAILABLE, "available" },			// Available
	{ YAHOO_STATUS_BRB, "brb" },						// Be Right Back
	{ YAHOO_STATUS_BUSY, "busy" },						// Busy
	{ YAHOO_STATUS_NOTATHOME, "notathome" },			// Not At Home
	{ YAHOO_STATUS_NOTATDESK, "notatdesk" },			// Not At Desk
	{ YAHOO_STATUS_NOTINOFFICE, "notinoffice" },		// Not In Office
	{ YAHOO_STATUS_INVISIBLE, "invisible" },			// Invisible
	{ YAHOO_STATUS_ONPHONE, "onphone" },				// On The Phone
	{ YAHOO_STATUS_ONVACATION, "onvacation" },			// On Vacation
	{ YAHOO_STATUS_OUTTOLUNCH, "outtolunch" },			// Out To Lunch
	{ YAHOO_STATUS_STEPPEDOUT, "steppedout" },			// Stepped Out
	{ YAHOO_STATUS_OFFLINE, "offline" },				// Off-line
	{ YAHOO_STATUS_AWAY, "away" },						// Away
	//{ YAHOO_STATUS_CUSTOM, "" },
	//{ YAHOO_STATUS_IDLE, "" },
	//{ YAHOO_STATUS_WEBLOGIN, "" },
	//{ YAHOO_STATUS_OFFLINE, "" },
	//{ YAHOO_STATUS_TYPING, "" }
};


CPurpleYahooProtocol::ConnectionErrorEntry CPurpleYahooProtocol::ms_pConnectionErrorEntries[] = 
{
	// yahoo.c
	{ CONNECTION_ERROR_SUSPENDED_ACCOUNT, "Your account is locked, please log in to the Yahoo! website." },
	{ CONNECTION_ERROR_AUTHENTICATION_DATA_INVALID, "Incorrect password." },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Unable to read" },
	{ CONNECTION_ERROR_INVALID_USERNAME, "Invalid username." },
	{ CONNECTION_ERROR_UNKNOWN_SERVER_ERROR, "Unknown error number" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Unable to connect." },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Connection problem" },
	{ CONNECTION_ERROR_SERVER_CONNECTION_FAILED, "Server closed the connection." },
	{ CONNECTION_ERROR_LOGGED_ON_ANOTHER_LOCATION, "You have been logged off as you have logged in on a different machine or device."},
	{ CONNECTION_ERROR_LOGGED_ON_ANOTHER_LOCATION, "You have signed on from another location."}
};




CPurpleYahooProtocol::CPurpleYahooProtocol() :
	CPurpleProtocol(PURPLE_YAHOO_PLUGIN_ID, PURPLE_YAHOO_PROTOCOL_NAME),
	m_bBlockContact(FALSE)
{
}


CPurpleYahooProtocol::~CPurpleYahooProtocol()
{
}


bool CPurpleYahooProtocol::HasContactServerAliasSupport()
{
	return false;
}


int CPurpleYahooProtocol::GetPrivacyFlags()
{
	return (PRIVACY_FLAGS_HAS_DENIED_LIST | PRIVACY_FLAGS_NO_LIST_INTERSECTION | 
			PRIVACY_FLAGS_REMOVE_DENIED_CONTACT);
}


bool CPurpleYahooProtocol::IsValidUserName(const string& strUser)
{
	if (purple_email_is_valid(strUser.c_str()) != 0)
	{
		// Is a valid email address. So we need to validate the 
		// accepted prefixes.
		
		string::size_type npos = -1;
		
		if ( (strUser.find("@ymail.com") != npos) || 
				(strUser.find("@rocketmail.com") != npos) )
		{
			return true;
		}
	}
	else
	{
		// Is not a valid email, so the user should be informed only 
		// the id. For example, inform only "johndoe" of "johndoe@yahoo.com".
		//
		
		//Valid characters: [_], [-], [.], [a - z], and [0 - 9]
		
		char symbols[] = {'_','-', '.', '\0'};
		char chElem;
		size_t nLen = strUser.length();
		
		for (unsigned int i = 0; i < nLen; i++)
		{
			chElem = (strUser.c_str())[i];
			
			if (!isalpha(chElem) && !isdigit(chElem))
			{
				int j = 0;
				for (; symbols[j] != '\0'; j++)
				{
					if (chElem == symbols[j])
					{
						break;
					}
				}
				
				if (symbols[j] == '\0')
				{
					return false;
				}
			}
		}
		
		if (nLen > 0)
		{
			return true;
		}
	}
    
	return false;
}


int CPurpleYahooProtocol::ProcessSystemMessage(PurpleAccount* pAccount, MessageParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessSystemMessage(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	return nRet;
}


int CPurpleYahooProtocol::ProcessErrorMessage(PurpleAccount* pAccount, MessageParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessErrorMessage(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	return nRet;
}


int CPurpleYahooProtocol::ProcessNotifyMessage(PurpleAccount* pAccount, NotifyMessageParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessNotifyMessage(pAccount, pParam);
	if (nRet)
	{
		return nRet;
	}

	string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

	CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
	if (pCallback == NULL)
	{
		// Ignore notification
		return TRUE;
	}

	string strPrimary;
	string strSecondary;
	string strTemp;

	if (pParam->type == PURPLE_NOTIFY_MSG_INFO)
	{
		if ((pParam->strPrimary.length() > 0) && (pParam->strSecondary.length() > 0))
		{
			GetLocaleTextFromUTF8(pParam->strPrimary.c_str(), strPrimary);
			GetLocaleTextFromUTF8(pParam->strSecondary.c_str(), strSecondary);

			if (strPrimary.compare("Add buddy rejected") == 0)
			{
				//yahoo.c
				//"%s has (retroactively) deny your request to add them to your list."
				//"%s has (retroactively) denied your request to add them to your list for the following reason: %s."
				string strMessageReason(" has (retroactively) denied your request to add them to your list for the following reason:");
				string strMessage(" has (retroactively) denied your request to add them to your list.");
				string strContact, strReason;


				if (ExtractSubstring(strSecondary.c_str(), NULL, strMessageReason.c_str(), strTemp))
				{
					strContact = strTemp;

					if (ExtractSubstring(strSecondary.c_str(), strMessageReason.c_str(), NULL, strTemp))
					{
						GetLocaleTextFromUTF8(strTemp.c_str(), strReason);

						ProcessNotifyMessageCallback(pCallback, &CIMProtocolCallback::OnAddContactDenied, strNormalizedUser.c_str(), strContact.c_str(), strReason.c_str());
					}
				}
				else if (ExtractSubstring(strSecondary.c_str(), NULL, strMessage.c_str(), strTemp))
				{
					strContact = strTemp;

					ProcessNotifyMessageCallback(pCallback, &CIMProtocolCallback::OnAddContactDenied, strNormalizedUser.c_str(), strContact.c_str(), "");
				}
			}
		}
	}

	return nRet;
}


int CPurpleYahooProtocol::ProcessRequestInput(PurpleAccount* pAccount, RequestInputParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessRequestInput(pAccount, pParam);
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
			cbOK = pParam->okCallback;
			cbCancel = pParam->cancelCallback;

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
				nRet = ProcessRequestInputCallback(pCallback, &CIMProtocolCallback::OnRequestAddContact, strNormalizedUser, strTemp, cbOK, cbCancel, pParam->pUserData, -1);
			}
		}
	}

	return nRet;
}


int CPurpleYahooProtocol::ProcessRequestAction(PurpleAccount* pAccount, RequestActionParam* pParam)
{
	// If superclass handles it, return
	int nRet = CPurpleProtocol::ProcessRequestAction(pAccount, pParam);
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

int CPurpleYahooProtocol::ProcessNotifyMessageCallback(CIMProtocolCallback* pCallback, void (CIMProtocolCallback::*pfnCallback)(const std::string&, const std::string&, const std::string&), const std::string& strUser, const std::string& strContact, const std::string& strMessage)
{
	(pCallback->*pfnCallback)(strUser, strContact, strMessage);

	return 0;
}

bool CPurpleYahooProtocol::GetStatusText(int nStatus, string& strStatusText)
{
	bool bValidStatus = false;

	int nInd = nStatus - YAHOO_STATUS_AVAILABLE;

	if ((nInd < (sizeof(ms_status) / sizeof(ms_status[0]))) &&
		(ms_status[nInd].status == nStatus))
	{
		strStatusText = ms_status[nInd].statusText;
		bValidStatus = true;
	}

	return bValidStatus;
}


bool CPurpleYahooProtocol::GetStatusCode(const std::string& statusText, int& nStatus)
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


bool CPurpleYahooProtocol::GetConnectionErrorCode(ConnectionError& errorCode, const char* pszReason)
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


IMProtocolError CPurpleYahooProtocol::AddDenyContact(const string& strUser, const string& strContact)
{
	CUOLCritSecLock<CAutoCriticalSectionWrapper> lock(m_csBlockContact);

    m_bBlockContact = TRUE;

	IMProtocolError error = RemoveContact(strUser, strContact, "");
	
	// Block contact after remove
	if (IM_PROTOCOL_ERROR_SUCCESS == error) 
	{
		error = CPurpleProtocol::AddDenyContact(strUser, strContact);
	}

    return (error);
}

IMProtocolError CPurpleYahooProtocol::RemoveContact(const string& strUser, const string& strContact, const string& strGroup)
{
	if ((strUser.length() == 0) ||
		(strContact.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	// Check if account is connected
	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleUser.c_str());
	if (pAccount == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_NOT_CONNECTED;
	}

	IMProtocolError retError = IM_PROTOCOL_ERROR_FAILED;

	char* pszUTF8Contact = purple_utf8_try_convert(strContact.c_str());

	if (pszUTF8Contact != NULL)
	{
		char* pszNormalizedUTF8Contact = g_strdup(purple_normalize(pAccount, pszUTF8Contact));

		if (pszNormalizedUTF8Contact != NULL)
		{
			PurpleBuddy* pBuddy = purple_find_buddy(pAccount, pszNormalizedUTF8Contact);

			if (pBuddy != NULL)
			{
				PurpleGroup *pPurpleGroup = purple_buddy_get_group(pBuddy);

				if (pPurpleGroup != NULL)
				{
					// Remove from remote server list and from Purple local buddy list...
					//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
							//new RemoveContactSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, pBuddy, pPurpleGroup));
					
					// Remove from remote server list and from Purple local buddy list...
					purple_account_remove_buddy(pAccount, pBuddy, pPurpleGroup);
					purple_blist_remove_buddy(pBuddy);

					retError = IM_PROTOCOL_ERROR_SUCCESS;
				}
			}
			else
			{
				// Contact not exists
				retError = IM_PROTOCOL_ERROR_CONTACT_NOT_FOUND;
			}

			g_free(pszNormalizedUTF8Contact);
		}

		g_free(pszUTF8Contact);
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return retError;
}

void CPurpleYahooProtocol::OnContactRemoved(PurpleAccount* pAccount, PurpleBuddy* pBuddy, PurpleGroup* pGroup)
{
	CUOLCritSecLock<CAutoCriticalSectionWrapper> lock(m_csBlockContact);

	if (m_bBlockContact)
	{
		m_bBlockContact = FALSE;

		// Moved this call to CPurpleYahooProtocol::AddDenyContact() 
		// avoiding WaitLibPurpleThreadAccess() deadlock.
		//CPurpleProtocol::AddDenyContact(pAccount->username, pBuddy->name);
	}

	CPurpleProtocol::OnContactRemoved(pAccount, pBuddy, pGroup);
}


void CPurpleYahooProtocol::OnIMReceived(PurpleAccount* pAccount, const char* pszFromUser, const char* pszText)
{
	CPurpleProtocol::OnIMReceived(pAccount, pszFromUser, pszText);

	OnContactTyping(pAccount, pszFromUser, PURPLE_NOT_TYPING);
}
