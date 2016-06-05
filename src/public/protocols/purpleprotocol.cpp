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
 
#include "purpleprotocol.h"


#include <glib.h>
#include <math.h>

// Purple headers
#include <connection.h>
#include <conversation.h>
#include <util.h>
#include <privacy.h>
#include <cmds.h>
#include <smiley.h>

#include "purplefiletransferinfo.h"
#include "purpleutils.h"
#include "purplebuddylisthelper.h"
#include "purpleprotocolstack.h"
#include "purpleservice.h"
#include "purplerequestuserauthorizationparam.h"
#include "PurpleRequestFileTransferAsyncInfo.h"


using namespace std;


typedef void (*LPPurpleMenuActionCallback)(gpointer, gpointer);

#define PURPLE_MENU_ACTION_CALLBACK(func) ((LPPurpleMenuActionCallback)func)


CPurpleProtocol::ConnectionErrorEntry CPurpleProtocol::ms_pConnectionErrorEntries[] = 
{
	// proxy.c
	{ CONNECTION_ERROR_PROXY_ACCESS_DENIED, "Access denied: proxy server forbids port " },
	{ CONNECTION_ERROR_PROXY_CONNECTION_ERROR, "Proxy connection error " }
};


// The idea on creating creating them as static is to assure we will access 
// libpurple throughout only one protocol instance by time.
//
BOOL	CPurpleProtocol::m_bEventsInitialized = FALSE;
CEvent	CPurpleProtocol::m_libPurpleHoldEvent;
CEvent	CPurpleProtocol::m_libPurpleReleaseEvent;
GMutex*	CPurpleProtocol::m_pUIMMultipleThreadSerializeMutex = NULL;



CPurpleProtocol::CPurpleProtocol(const string& strProtocolId, const string& strProtocolName) : 
	CIMProtocol7(),
	m_strId(strProtocolId),
	m_strName(strProtocolName),
	m_lNextFileTransferId(0)
{
	if (!m_bEventsInitialized)
	{
		m_bEventsInitialized = m_libPurpleHoldEvent.Create(
				NULL, TRUE, TRUE, NULL);
		
		if (m_bEventsInitialized)
		{
			m_bEventsInitialized = m_libPurpleReleaseEvent.Create(
					NULL, TRUE, TRUE, NULL);
		}
		
		if (m_bEventsInitialized)
		{
			m_pUIMMultipleThreadSerializeMutex = g_mutex_new();
			
			m_bEventsInitialized = (m_pUIMMultipleThreadSerializeMutex != NULL);
		}
	}
	
	m_pAuthorizeCancelationMutex = g_mutex_new();
}

CPurpleProtocol::~CPurpleProtocol(void)
{
	if (m_libPurpleHoldEvent)
	{
		m_libPurpleHoldEvent.Close();
	}
	
	if (m_libPurpleReleaseEvent)
	{
		m_libPurpleReleaseEvent.Close();
	}
	
	if (m_pUIMMultipleThreadSerializeMutex)
	{
		g_mutex_free(m_pUIMMultipleThreadSerializeMutex);
		m_pUIMMultipleThreadSerializeMutex = NULL;
	}
	
	if (m_pAuthorizeCancelationMutex)
	{
		g_mutex_free(m_pAuthorizeCancelationMutex);
		m_pAuthorizeCancelationMutex = NULL;
	}
	
	// Release memory allocated for callback map keys
	//CCallbackMap::iterator it;
	//string* pStrUser;
	//for (it = m_mapCallback.begin(); 
	//	 it != m_mapCallback.end(); 
	//	 ++it)
	//{
	//	pStrUser = it->first;
	//	delete pStrUser;

	//	//delete it->second;
	//}
}

unsigned CPurpleProtocol::GetProtocolPos()
{
	return UINT_MAX;
}


void CPurpleProtocol::Init()
{
	CIMProtocol::Init();

	// Just for testing purpouses ...
	/*
	purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
			new InitSerializerHelper((CPurpleProtocolSerializer*)this));
	*/
}


void CPurpleProtocol::Finalize()
{
	//SetFinalizeInProgress(true);
	
	// Destroy all accounts...
	GList* pListAccounts = NULL;
	GList* pListDestroyAccounts = NULL;
	PurpleAccount* pAccount = NULL;

	// Get Purple accounts associated with this protocol
	for (pListAccounts = purple_accounts_get_all(); pListAccounts != NULL; pListAccounts = pListAccounts->next)
	{
		if (pListAccounts->data != NULL)
		{
			pAccount = (PurpleAccount*) pListAccounts->data;
			if (0 == strcmp(pAccount->protocol_id, m_strId.c_str()))
			{
				pListDestroyAccounts = g_list_append(pListDestroyAccounts, pListAccounts->data);
			}
		}
	}

	// Free accounts
	for (; pListDestroyAccounts != NULL; pListDestroyAccounts = pListDestroyAccounts->next)
	{
		pAccount = (PurpleAccount*) pListDestroyAccounts->data;
		
		if (pAccount->gc != NULL)
		{
			pAccount->gc->wants_to_die = TRUE;
		}

		// First we need to disconnect the account.
		if (purple_account_get_enabled(pAccount, purple_core_get_ui()))
		{
			purple_account_set_enabled(pAccount, purple_core_get_ui(), FALSE);
		}
		
		purple_account_disconnect(pAccount);
		
		// Now we remove the account.
		purple_accounts_remove(pAccount);
		purple_account_destroy(pAccount);
	}


	if (pListDestroyAccounts != NULL)
	{
		g_list_free(pListDestroyAccounts);
	}

	CIMProtocol::Finalize();
}


string CPurpleProtocol::GetId()
{
	return m_strId;
}

string CPurpleProtocol::GetName()
{
	return m_strName;
}


bool CPurpleProtocol::HasUserDisplayIconSupport()
{
	return true;
}


bool CPurpleProtocol::HasContactBlockingSupport()
{
	return true;
}


bool CPurpleProtocol::HasFileTransferSupport()
{
	return true;
}


bool CPurpleProtocol::HasContactServerAliasSupport()
{
	return true;
}


bool CPurpleProtocol::HasReRequestAuthorizationSupport()
{
	return false;
}


int CPurpleProtocol::GetPrivacyFlags()
{
	return 0;
}


bool CPurpleProtocol::AutoAddContactOnAuthorization()
{
	return false;
}


std::string CPurpleProtocol::GetDefaultContactGroup()
{
	return "";
}


bool CPurpleProtocol::IsValidUserName(const string& /*strUser*/)
{
	return true;
}


void CPurpleProtocol::SetCallback(const string& strUser, CIMProtocolCallback* pCallback)
{
	if (strUser.length() > 0)
	{
		char* pszUTF8User = purple_utf8_try_convert(strUser.c_str());
		//const char* pszUTF8User = purple_normalize(NULL, strUser.c_str());

		if (pszUTF8User != NULL)
		{
			string strUTF8User = pszUTF8User;

			m_mapCallback[strUTF8User] = pCallback;

			g_free(pszUTF8User);
		}
	}
}




//
// Connection API
//
IMProtocolError CPurpleProtocol::Connect(const string& strUser, const string& strPass, CProtocolSettings* pExtraSettings, CProxyInfo* pProxyInfo)
{
	return Connect(strUser, strPass, pExtraSettings, pProxyInfo, NULL);
}

IMProtocolError CPurpleProtocol::Disconnect(const string& strUser)
{
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Disconnection method CAN NOT be completely blocked as other methods 
	// (disconnection while connecting leads to deadlock).
	// 
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());

	PurpleAccount* pAccount = GetPurpleAccount(strPurpleUser.c_str());

	// Check if account is registered
	if (pAccount == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_FAILED;
	}
	
	purple_timeout_add(0, (GSourceFunc)SerializedDisconnectMethod, pAccount);

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_SUCCESS;
}


int CPurpleProtocol::IsConnected(const string& strUser)
{
	int nConnected = FALSE;

	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());

	PurpleAccount* pAccount = GetPurpleAccount(strPurpleUser.c_str());

	// Check if account is registered
	if (pAccount != NULL)
	{
		nConnected = ((pAccount->gc != NULL) && (pAccount->gc->state == PURPLE_CONNECTED));
	}

	return nConnected;
}


//
// Password API
//
IMProtocolError CPurpleProtocol::ChangePassword(const string& strUser, const string& strOldPassword, const string& strNewPassword)
{
	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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

	char* pszUTF8OldPass = purple_utf8_try_convert(strOldPassword.c_str());

	if (pszUTF8OldPass != NULL)
	{
		char* pszUTF8NewPass = purple_utf8_try_convert(strNewPassword.c_str());

		if (pszUTF8NewPass != NULL)
		{
			//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
					//new ChangePasswordSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, pszUTF8OldPass, pszUTF8NewPass));
			
			purple_account_change_password(pAccount, pszUTF8OldPass, pszUTF8NewPass);

			retError = IM_PROTOCOL_ERROR_SUCCESS;

			g_free(pszUTF8NewPass);
		}

		g_free(pszUTF8OldPass);
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return retError;
}


//
// Contact API
//
IMProtocolError CPurpleProtocol::AddContact(const string& strUser, const string& strContact, const string& strGroup)
{
	if ((strUser.length() == 0) ||
		(strContact.length() == 0) ||
		(strGroup.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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
			char* pszUTF8Group = purple_utf8_try_convert(strGroup.c_str());

			if (pszUTF8Group != NULL)
			{
				PurpleBuddy* pBuddy = NULL;

				// Look for buddy
				PurpleGroup* pGroup = purple_find_group(pszUTF8Group);
				if (pGroup != NULL)
				{
					pBuddy = purple_find_buddy_in_group(pAccount, pszNormalizedUTF8Contact, pGroup);
				}

				if (pBuddy == NULL)
				{
					// New contact, create purple buddy
					pBuddy = purple_buddy_new(pAccount, pszNormalizedUTF8Contact, NULL);

					// Add buddy to Purple local buddy list and to remote server list...
					if (pBuddy != NULL)
					{
						// Create/get group
						if (pGroup == NULL)
						{
							pGroup = purple_group_new(pszUTF8Group);
							purple_blist_add_group(pGroup, NULL);
						}

						if (pGroup != NULL)
						{
							//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
									//new AddContactSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, pBuddy, pGroup));
							
							// Add buddy to Purple local buddy list and to remote server list...
							purple_blist_add_buddy(pBuddy, NULL, pGroup, NULL);
							purple_account_add_buddy(pAccount, pBuddy);

							retError = IM_PROTOCOL_ERROR_SUCCESS;
						}
					}
				}
				else
				{
					retError = IM_PROTOCOL_ERROR_CONTACT_ALREADY_EXISTS;
				}

				g_free(pszUTF8Group);
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


IMProtocolError CPurpleProtocol::RemoveContact(const string& strUser, const string& strContact, const string& strGroup)
{
	if ((strUser.length() == 0) ||
		(strContact.length() == 0) ||
		(strGroup.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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
			char* pszUTF8Group = purple_utf8_try_convert(strGroup.c_str());

			if (pszUTF8Group != NULL)
			{
				// Look for contact...
				PurpleGroup* pGroup = purple_find_group(pszUTF8Group);

				if (pGroup != NULL)
				{					
					PurpleBuddy* pBuddy = purple_find_buddy_in_group(pAccount, pszNormalizedUTF8Contact, pGroup);

					if (pBuddy != NULL)
					{
						// Remove from remote server list and from Purple local buddy list...
						//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
								//new RemoveContactSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, pBuddy, pGroup));
						
						// Remove from remote server list and from Purple local buddy list...
						purple_account_remove_buddy(pAccount, pBuddy, pGroup);
						purple_blist_remove_buddy(pBuddy);

						retError = IM_PROTOCOL_ERROR_SUCCESS;
					}
					else
					{
						// Contact not exists
						retError = IM_PROTOCOL_ERROR_CONTACT_NOT_FOUND;
					}
				}
				else
				{
					retError = IM_PROTOCOL_ERROR_GROUP_NOT_FOUND;
				}

				g_free(pszUTF8Group);
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


IMProtocolError CPurpleProtocol::AliasContact(const string& strUser, const string& strContact, const string& strAlias)
{
	if ((strUser.length() == 0) ||
		(strContact.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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
			char* pszUTF8Alias = NULL;
			if (strAlias.length() > 0)
			{
				pszUTF8Alias = purple_utf8_try_convert(strAlias.c_str());
			}

			// Look for contact
			PurpleBuddy* pBuddy = purple_find_buddy(pAccount, pszNormalizedUTF8Contact);

			if (pBuddy != NULL)
			{
				// Change alias locally and remotely
				//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
						//new AliasContactSerializerHelper((CPurpleProtocolSerializer*)this, pBuddy, pszUTF8Alias));
				
				// Change alias locally and remotely
				purple_blist_alias_buddy(pBuddy, pszUTF8Alias);
				serv_alias_buddy(pBuddy);

				retError = IM_PROTOCOL_ERROR_SUCCESS;
			}

			if (pszUTF8Alias != NULL)
			{
				g_free(pszUTF8Alias);
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


IMProtocolError CPurpleProtocol::MoveContact(const string& strUser, const string& strContact, const string& strOldGroup, const string& strNewGroup)
{
	if ((strUser.length() == 0) ||
		(strContact.length() == 0) ||
		(strNewGroup.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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
			PurpleBuddy* pBuddy = NULL;

			char* psszUTF8OldGroup = purple_utf8_try_convert(strOldGroup.c_str());

			if (psszUTF8OldGroup != NULL)
			{
				PurpleGroup* pOldGroup = purple_find_group(psszUTF8OldGroup);

				if ((pOldGroup != NULL) && (pOldGroup->name != NULL))
				{
					PurpleBuddy* pBuddy = purple_find_buddy_in_group(pAccount, pszNormalizedUTF8Contact, pOldGroup);

					if (pBuddy != NULL)
					{
						char* pszUTF8NewGroup = purple_utf8_try_convert(strNewGroup.c_str());

						if (pszUTF8NewGroup != NULL)
						{
							// Create/get new group
							PurpleGroup* pNewGroup = purple_find_group(pszUTF8NewGroup);
							if (pNewGroup == NULL)
							{
								pNewGroup = purple_group_new(pszUTF8NewGroup);
								purple_blist_add_group(pNewGroup, NULL);
							}

							if (pNewGroup != NULL)
							{								
								// Move on remote server and on Purple buddy list
								CPurpleBuddyListHelper::RemovePurpleBuddy(pOldGroup, pBuddy);
								CPurpleBuddyListHelper::AddPurpleBuddy(pNewGroup, pBuddy);

								//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
										//new MoveContactSerializerHelper((CPurpleProtocolSerializer*)this, pBuddy, pOldGroup, pNewGroup));
								
								serv_move_buddy(pBuddy, pOldGroup, pNewGroup);

								retError = IM_PROTOCOL_ERROR_SUCCESS;

								string strOldGroup("");
								GetLocaleTextFromUTF8(pOldGroup->name, strOldGroup);

								// Move contact callback
								CIMProtocolCallback* pCallback = GetCallback(strUser.c_str());
								if (pCallback != NULL)
								{
									m_callbackNotifier.HandleOnContactMoved(pCallback, strUser, strContact, strOldGroup, strNewGroup);
								}
							}

							g_free(pszUTF8NewGroup);
						}

					}
					else
					{
						retError = IM_PROTOCOL_ERROR_CONTACT_NOT_FOUND;
					}
				}
				else
				{
					retError = IM_PROTOCOL_ERROR_CONTACT_NOT_FOUND;
				}

				g_free(psszUTF8OldGroup);
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


IMProtocolError CPurpleProtocol::GetContactInfo(const string& strUser, const string& strContact)
{
	if ((strUser.length() == 0) ||
		(strContact.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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
			if (pAccount->gc != NULL)
			{
				//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
						//new GetContactInfoSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, pszUTF8Contact));
				
				serv_get_info(pAccount->gc, pszUTF8Contact);

				retError = IM_PROTOCOL_ERROR_SUCCESS;
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


IMProtocolError CPurpleProtocol::CanRequestContactAuthorization(const std::string& strUser, const std::string& strContact, bool& bCanRequest)
{
	bCanRequest = false;

	return IM_PROTOCOL_ERROR_SUCCESS;
}


IMProtocolError CPurpleProtocol::RequestContactAuthorization(const std::string& strUser, const std::string& strContact)
{
	return IM_PROTOCOL_ERROR_FAILED;
}



IMProtocolError CPurpleProtocol::RenameGroup(const string& strUser, const string& strOldName, const string& strNewName)
{
	if ((strUser.length() == 0) ||
		(strOldName.length() == 0) ||
		(strNewName.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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

	// Check if group names are different
	if (0 == strcmpi(strOldName.c_str(), strNewName.c_str()))
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_SUCCESS;
	}

	IMProtocolError retError = IM_PROTOCOL_ERROR_SUCCESS;

	char* pszUTF8OldGroup = purple_utf8_try_convert(strOldName.c_str());

	if (pszUTF8OldGroup != NULL)
	{
		char* pszUTF8NewGroup = purple_utf8_try_convert(strNewName.c_str());

		if (pszUTF8NewGroup != NULL)
		{
			// Check if old group exists
			PurpleGroup* pOldGroup = purple_find_group(pszUTF8OldGroup);

			// Look for new group
			PurpleGroup* pNewGroup = purple_find_group(pszUTF8NewGroup);

			if (!pNewGroup)
			{
				// Create new group
				pNewGroup = purple_group_new(pszUTF8NewGroup);
			}
                
			if (pNewGroup != NULL)
			{
				int nAccountBuddiesCount = CPurpleBuddyListHelper::GetPurpleGroupBuddiesCount(pAccount, pNewGroup);

				if (0 == nAccountBuddiesCount)
				{
					// Silently add new group
					CPurpleBuddyListHelper::AddPurpleGroup(pNewGroup);

					// Move all buddies of this account from old group to new group
					GList* pMovedBuddies = NULL;
					if (pOldGroup != NULL)
					{
						CPurpleBuddyListHelper::RemovePurpleBuddies(pAccount, pOldGroup, pMovedBuddies);

						CPurpleBuddyListHelper::AddPurpleBuddies(pNewGroup, pMovedBuddies);
					}

					// Rename group on server list
					if ((pAccount->gc != NULL) &&
						(purple_connection_get_state(pAccount->gc) == PURPLE_CONNECTED))
					{
						PurplePluginProtocolInfo *prpl_info = NULL;
						
						if (pAccount->gc->prpl)
							prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(pAccount->gc->prpl);

						if(prpl_info)
						{
							if(prpl_info->rename_group)
							{
								//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
										//new RenameGroupSerializerHelper((CPurpleProtocolSerializer*)this, prpl_info, pAccount, 
												//pszUTF8OldGroup, pNewGroup, pMovedBuddies));
								
								// Rename group from server list...
								prpl_info->rename_group(pAccount->gc, pszUTF8OldGroup, pNewGroup, pMovedBuddies);
							}
							else
							{
								GList *cur, *groups = NULL;

								/* Make a list of what the groups each buddy is in */
								for(cur = pMovedBuddies; cur; cur = cur->next) {
									PurpleBlistNode *node = (PurpleBlistNode *)cur->data;
									groups = g_list_prepend(groups, node->parent->parent);
								}

								purple_account_remove_buddies(pAccount, pMovedBuddies, groups);
								g_list_free(groups);
								purple_account_add_buddies(pAccount, pMovedBuddies);
							}

							retError = IM_PROTOCOL_ERROR_SUCCESS;

							// Rename group callback
							CIMProtocolCallback* pCallback = GetCallback(strUser.c_str());
							if (pCallback != NULL)
							{
								m_callbackNotifier.HandleOnGroupRenamed(pCallback, strUser, strOldName, strNewName);
							}
						}
					}

					// If there are no children on Purple buddy list, remove old group
					if (pOldGroup != NULL)
					{
						CPurpleBuddyListHelper::RemovePurpleEmptyGroup(pOldGroup);
					}

					g_list_free(pMovedBuddies);
				}
				else
				{
					retError = IM_PROTOCOL_ERROR_GROUP_ALREADY_EXISTS;
				}
			}
			else
			{
				// Old group not exists
				retError = IM_PROTOCOL_ERROR_GROUP_NOT_FOUND; 
			}

			g_free(pszUTF8NewGroup);
		}

		g_free(pszUTF8OldGroup);
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return retError;
}


IMProtocolError CPurpleProtocol::RemoveGroup(const string& strUser, const string& strGroup)
{
	if ((strUser.length() == 0) ||
		(strGroup.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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

	IMProtocolError retError = IM_PROTOCOL_ERROR_SUCCESS;

	char* pszUTF8Group = purple_utf8_try_convert(strGroup.c_str());

	if (pszUTF8Group == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_FAILED;
	}

	// Look for group
	PurpleGroup* pGroup = purple_find_group(pszUTF8Group);

	if (pGroup != NULL)
	{
		PurpleBlistNode* pNode = (PurpleBlistNode*) pGroup;

		// Check if group has any buddies from this account...
		int nAccountBuddiesCount = 0;

		if (pNode->child)
		{
			nAccountBuddiesCount = CPurpleBuddyListHelper::GetPurpleGroupBuddiesCount(pAccount, pGroup);

			if (nAccountBuddiesCount > 0)
			{
				// Do not remove group if not empty
				retError =  IM_PROTOCOL_ERROR_GROUP_NOT_EMPTY;
			}
		}

		if ((pAccount->gc != NULL) && (nAccountBuddiesCount == 0))
		{			
			// Remove group from server list...
			//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
					//new RemoveGroupSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, pGroup));
			
			// Remove group from server list...
			purple_account_remove_group(pAccount, pGroup);

			// Remove from Purple buddy list if group get empty
			CPurpleBuddyListHelper::RemovePurpleEmptyGroup(pGroup);

			// Remove group callback...
			CIMProtocolCallback* pCallback = GetCallback(strUser.c_str());
			if (pCallback != NULL)
			{
				m_callbackNotifier.HandleOnGroupRemoved(pCallback, strUser, strGroup);
			}
		}
	}
	else
	{
		// Group not exists
		retError = IM_PROTOCOL_ERROR_GROUP_NOT_FOUND;
	}

	g_free(pszUTF8Group);

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return retError;
}



// 
// Alias API
//
IMProtocolError CPurpleProtocol::SetAlias(const std::string& strUser, const std::string& strAlias)
{
	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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

	PurpleConnection* pPurpleConnection = purple_account_get_connection(pAccount);
	
	if (pPurpleConnection == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_NOT_CONNECTED;
	}
	else
	{
		char* pszUTF8Alias = purple_utf8_try_convert(strAlias.c_str());
		
		//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
				//new SetAliasSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, pszUTF8Alias));
		
		purple_account_set_alias(pAccount, pszUTF8Alias);
		serv_set_alias(pPurpleConnection, pszUTF8Alias);
		
		if (pszUTF8Alias != NULL)
		{
			g_free(pszUTF8Alias);
		}
	}
	
	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_SUCCESS;
}



// 
// Icon API
//
void CPurpleProtocol::GetIconSpecification(CIconSpec& iconSpec)
{
	PurplePlugin* pPlugin = purple_plugins_find_with_id(m_strId.c_str());

	if ((pPlugin != NULL) &&
		(pPlugin->info != NULL) &&
		(pPlugin->info->extra_info != NULL))
	{
		PurplePluginProtocolInfo* pProtocolInfo = (PurplePluginProtocolInfo*) pPlugin->info->extra_info;

		string strImagesFormats = (pProtocolInfo->icon_spec.format != NULL) ? pProtocolInfo->icon_spec.format : "";
		iconSpec.SetImageFormats(strImagesFormats);

		iconSpec.SetMinWidth(pProtocolInfo->icon_spec.min_width);
		iconSpec.SetMinHeight(pProtocolInfo->icon_spec.min_height);
		iconSpec.SetMaxWidth(pProtocolInfo->icon_spec.max_width);
		iconSpec.SetMaxHeight(pProtocolInfo->icon_spec.max_height);
	}
}


IMProtocolError CPurpleProtocol::SetUserDisplayIcon(const string& strUser, const string& strFile)
{
	if ((strUser.length() == 0) ||
		(strFile.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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
	
	gchar *contents = NULL;
	gsize len = 0;
	
	char* pszUTF8File = purple_utf8_try_convert(strFile.c_str());
	
	if (pszUTF8File)
	{
		if (!g_file_get_contents(pszUTF8File, &contents, &len, NULL))
		{
			// Release libpurple's thread.
			//
			ReleaseLibPurpleThreadAccess();
			
			return IM_PROTOCOL_ERROR_FAILURE_LOADING_FILE;
		}
		else
		{
			purple_account_set_buddy_icon_path(pAccount, pszUTF8File);
			purple_buddy_icons_set_account_icon(pAccount, (guchar*)contents, len);
		}
		
		g_free(pszUTF8File);
	}
	
	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_SUCCESS;
}


//
// Status API
//
IMProtocolError CPurpleProtocol::SetUserStatus(const string& strUser, int nStatus)
{
	return SetUserStatus(strUser, nStatus, "");
}


//
// IM API
//
IMProtocolError CPurpleProtocol::SendIM(const string& strFromUser, const string& strToUser, const string& strMessage)
{
	if ((strFromUser.length() == 0) ||
		(strToUser.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	// Check if account is connected
	string strPurpleUser = GetNormalizedPurpleUserName(strFromUser.c_str());
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleUser.c_str());
	if (pAccount == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_NOT_CONNECTED;
	}

	IMProtocolError sendError = IM_PROTOCOL_ERROR_FAILED;

	char* pszUTF8Destination = purple_utf8_try_convert(strToUser.c_str());

	if (pszUTF8Destination != NULL)
	{
		char* pszNormalizedUTF8Destination = g_strdup(purple_normalize(pAccount, pszUTF8Destination));

		if (pszNormalizedUTF8Destination != NULL)
		{
			string strFormatedMessage = FormatSendingMessage(strMessage);

			char* pszUTF8Message = purple_utf8_try_convert(strFormatedMessage.c_str());

			if (pszUTF8Message != NULL)
			{
				// Get/create conversation associated to account and destination
				PurpleConversation* pPurpleConv = purple_conversation_new(PURPLE_CONV_TYPE_IM, pAccount, pszNormalizedUTF8Destination);
				if (pPurpleConv != NULL)
				{
					// Send IM
					PurpleConvIm* pConvIM = purple_conversation_get_im_data(pPurpleConv);
					if (pConvIM != NULL)
					{
						//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
								//new SendIMSerializerHelper((CPurpleProtocolSerializer*)this, pConvIM, pszUTF8Message));
						
						purple_conv_im_send(pConvIM, pszUTF8Message);
						
						sendError = IM_PROTOCOL_ERROR_SUCCESS;
					}
				}

				g_free(pszUTF8Message);
			}

			g_free(pszNormalizedUTF8Destination);
		}

		g_free(pszUTF8Destination);
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return sendError;
}


//
// Notifications API
//
IMProtocolError CPurpleProtocol::SendTyping(const string& strFromUser, const string& strToUser, TypingState state)
{
	if ((strFromUser.length() == 0) ||
		(strToUser.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	// Check if account is connected
	string strPurpleUser = GetNormalizedPurpleUserName(strFromUser.c_str());
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleUser.c_str());
	if (pAccount == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_NOT_CONNECTED;
	}

	IMProtocolError sendError = IM_PROTOCOL_ERROR_FAILED;

	char* pszUTF8Destination = purple_utf8_try_convert(strToUser.c_str());

	if (pszUTF8Destination != NULL)
	{
		char* pszNormalizedUTF8Destination = g_strdup(purple_normalize(pAccount, pszUTF8Destination));

		if (pszNormalizedUTF8Destination != NULL)
		{
			// Get/create conversation associated to account and destination
			PurpleConversation* pPurpleConv = purple_conversation_new(PURPLE_CONV_TYPE_IM, pAccount, pszNormalizedUTF8Destination);
			if (pPurpleConv != NULL)
			{
				PurpleConvIm* pConvIM = purple_conversation_get_im_data(pPurpleConv);
				if (pConvIM != NULL)
				{
					//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
							//new SendTypingSerializerHelper((CPurpleProtocolSerializer*)this, pConvIM, pPurpleConv, state));
					
					// Update PurpleConvIM typing state
					PurpleTypingState purpleTypingState = (PurpleTypingState) state;
					//purple_conv_im_set_typing_state(pConvIM, purpleTypingState);
					
					// Send typing state
					serv_send_typing(purple_conversation_get_gc(pPurpleConv),
									purple_conversation_get_name(pPurpleConv),
									purpleTypingState);

					sendError = IM_PROTOCOL_ERROR_SUCCESS;
				}
			}

			g_free(pszNormalizedUTF8Destination);
		}

		g_free(pszUTF8Destination);
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return sendError;
}


IMProtocolError CPurpleProtocol::SendConversationWindowClosed(const string& strFromUser, const string& strToUser)
{
	if ((strFromUser.length() == 0) ||
		(strToUser.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	// Check if account is connected
	string strPurpleUser = GetNormalizedPurpleUserName(strFromUser.c_str());
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleUser.c_str());
	if (pAccount == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_NOT_CONNECTED;
	}

	IMProtocolError sendError = IM_PROTOCOL_ERROR_FAILED;

	char* pszUTF8Destination = purple_utf8_try_convert(strToUser.c_str());

	if (pszUTF8Destination != NULL)
	{
		char* pszNormalizedUTF8Destination = g_strdup(purple_normalize(pAccount, pszUTF8Destination));

		if (pszNormalizedUTF8Destination != NULL)
		{
			// Get/create conversation associated to account and destination
			PurpleConversation* pPurpleConv = purple_find_conversation_with_account(
					PURPLE_CONV_TYPE_ANY, pszNormalizedUTF8Destination, pAccount);

			if (pPurpleConv != NULL)
			{
				//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
						//new SendConversationWindowClosedSerializerHelper(
								//(CPurpleProtocolSerializer*)this, pPurpleConv));
				
				purple_conversation_destroy(pPurpleConv);

				sendError = IM_PROTOCOL_ERROR_SUCCESS;
			}

			g_free(pszNormalizedUTF8Destination);
		}

		g_free(pszUTF8Destination);
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return sendError;
}


IMProtocolError CPurpleProtocol::SetIdle(const string& strUser, int nSeconds)
{
	// Check arguments
	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	// Check if account is registered and connected
	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleUser.c_str());
	if (pAccount == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_NOT_CONNECTED;
	}

	//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
			//new SetIdleSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, nSeconds));
	
	if (nSeconds > 0)
	{
		// Become idle
		//serv_set_idle(pAccount->gc, 0);
		//serv_set_idle(pAccount->gc, nSeconds);
		
		// Adjust connection idle flag
		
		// Inform the time when it start to be idle.
		purple_presence_set_idle(pAccount->presence, TRUE, time(NULL) - nSeconds);
	}
	else
	{
		// Return from iddle
		purple_presence_set_idle(pAccount->presence, FALSE, 0);
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_SUCCESS;
}


//
// Privacy API
//
IMProtocolError CPurpleProtocol::GetDenyContactsList(const string& strUser, CPrivacyList& listContacts)
{
	listContacts.clear();

	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	// Check if account is registered
	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());
	PurpleAccount* pAccount = GetPurpleAccount(strPurpleUser.c_str());
	if (pAccount == NULL)
	{
		return IM_PROTOCOL_ERROR_NOT_CONNECTED;
	}
	
	for (GSList* pList = pAccount->deny; pList != NULL; pList = pList->next)
	{
		if (pList->data != NULL)
		{
			string strUserName;
			GetLocaleTextFromUTF8((char*) pList->data, strUserName);

			listContacts.push_back(strUserName);
		}
	}

	return IM_PROTOCOL_ERROR_SUCCESS;
}


IMProtocolError CPurpleProtocol::GetPermitContactsList(const string& strUser, CPrivacyList& listContacts)
{
	listContacts.clear();

	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	// Check if account is registered
	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());
	PurpleAccount* pAccount = GetPurpleAccount(strPurpleUser.c_str());
	if (pAccount == NULL)
	{
		return IM_PROTOCOL_ERROR_NOT_CONNECTED;
	}

	for (GSList* pList = pAccount->permit; pList != NULL; pList = pList->next)
	{
		if (pList->data != NULL)
		{
			string strUserName;
			GetLocaleTextFromUTF8((char*) pList->data, strUserName);

			listContacts.push_back(strUserName);
		}
	}

	return IM_PROTOCOL_ERROR_SUCCESS;
}


IMProtocolError CPurpleProtocol::AddDenyContact(const string& strUser, const string& strContact)
{
	if ((strUser.length() == 0) ||
		(strContact.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	if (!HasContactBlockingSupport())
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_NOT_SUPPORTED;
	}

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

	IMProtocolError retError = IM_PROTOCOL_ERROR_SUCCESS;
	
	char* pszUTF8Contact = purple_utf8_try_convert(strContact.c_str());

	if (pszUTF8Contact != NULL)
	{
		char* pszNormalizedUTF8Contact = g_strdup(purple_normalize(pAccount, pszUTF8Contact));

		if (pszNormalizedUTF8Contact != NULL)
		{
			// Old implementation...
			if (!purple_privacy_deny_add(pAccount, pszNormalizedUTF8Contact, FALSE))
			{
				retError = IM_PROTOCOL_ERROR_FAILED;
			}
			
			/*
			// New code to make the only validation that results on result error 
			// equal to IM_PROTOCOL_ERROR_FAILED.
			//
			GSList *pDenyList;
			
			for (pDenyList = pAccount->deny; pDenyList != NULL; pDenyList = pDenyList->next)
			{
				if (!purple_utf8_strcasecmp(pszNormalizedUTF8Contact, 
						purple_normalize(pAccount, (char *)pDenyList->data)))
				{
					break;
				}
			}
			
			if (pDenyList != NULL)
			{
				retError = IM_PROTOCOL_ERROR_FAILED;
			}
			else
			{
				purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
						new AddDenyContactSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, pszNormalizedUTF8Contact));
			}
			*/

			g_free(pszNormalizedUTF8Contact);
		}

		g_free(pszUTF8Contact);
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return retError;
}


IMProtocolError CPurpleProtocol::AddPermitContact(const string& strUser, const string& strContact)
{
	if ((strUser.length() == 0) ||
		(strContact.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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

	IMProtocolError retError = IM_PROTOCOL_ERROR_SUCCESS;

	char* pszUTF8Contact = purple_utf8_try_convert(strContact.c_str());

	if (pszUTF8Contact != NULL)
	{
		char* pszNormalizedUTF8Contact = g_strdup(purple_normalize(pAccount, pszUTF8Contact));

		if (pszNormalizedUTF8Contact != NULL)
		{
			// Old implementation...
			if (!purple_privacy_permit_add(pAccount, pszNormalizedUTF8Contact, FALSE))
			{
				retError = IM_PROTOCOL_ERROR_FAILED;
			}
			
			/*
			// New code to make the only validation that results on result error 
			// equal to IM_PROTOCOL_ERROR_FAILED.
			//
			GSList *pPermitList;
			
			for (pPermitList = pAccount->permit; pPermitList != NULL; pPermitList = pPermitList->next)
			{
				if (!purple_utf8_strcasecmp(pszNormalizedUTF8Contact, 
						purple_normalize(pAccount, (char *)pPermitList->data)))
				{
					break;
				}
			}
			
			if (pPermitList != NULL)
			{
				retError = IM_PROTOCOL_ERROR_FAILED;
			}
			else
			{
				purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
						new AddPermitContactSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, pszNormalizedUTF8Contact));
			}
			*/

			g_free(pszNormalizedUTF8Contact);
		}

		g_free(pszUTF8Contact);
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return retError;
}


IMProtocolError CPurpleProtocol::RemoveDenyContact(const string& strUser, const string& strContact)
{
	if ((strUser.length() == 0) ||
		(strContact.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	if (!HasContactBlockingSupport())
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_NOT_SUPPORTED;
	}

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

	IMProtocolError retError = IM_PROTOCOL_ERROR_SUCCESS;
	
	char* pszUTF8Contact = purple_utf8_try_convert(strContact.c_str());

	if (pszUTF8Contact != NULL)
	{
		char* pszNormalizedUTF8Contact = g_strdup(purple_normalize(pAccount, pszUTF8Contact));

		if (pszNormalizedUTF8Contact != NULL)
		{
			// Old implementation...
			if (!purple_privacy_deny_remove(pAccount, pszNormalizedUTF8Contact, FALSE))
			{
				retError = IM_PROTOCOL_ERROR_FAILED;
			}
			
			/*
			// New code to make the only validation that results on result error 
			// equal to IM_PROTOCOL_ERROR_FAILED.
			//
			GSList *pDenyList;
			
			for (pDenyList = pAccount->deny; pDenyList != NULL; pDenyList = pDenyList->next)
			{
				if (!purple_utf8_strcasecmp(pszNormalizedUTF8Contact, 
						purple_normalize(pAccount, (char *)pDenyList->data)))
				{
					break;
				}
			}
			
			if (pDenyList == NULL)
			{
				retError = IM_PROTOCOL_ERROR_FAILED;
			}
			else
			{
				purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
						new RemoveDenyContactSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, pszNormalizedUTF8Contact));
			}
			*/

			g_free(pszNormalizedUTF8Contact);
		}

		g_free(pszUTF8Contact);
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return retError;
}


IMProtocolError CPurpleProtocol::RemovePermitContact(const string& strUser, const string& strContact)
{
	if ((strUser.length() == 0) ||
		(strContact.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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

	IMProtocolError retError = IM_PROTOCOL_ERROR_SUCCESS;

	char* pszUTF8Contact = purple_utf8_try_convert(strContact.c_str());

	if (pszUTF8Contact != NULL)
	{
		char* pszNormalizedUTF8Contact = g_strdup(purple_normalize(pAccount, pszUTF8Contact));

		if (pszNormalizedUTF8Contact != NULL)
		{
			// Old implementation...
			if (!purple_privacy_permit_remove(pAccount, pszNormalizedUTF8Contact, FALSE))
			{
				retError = IM_PROTOCOL_ERROR_FAILED;
			}
			
			/*
			// New code to make the only validation that results on result error 
			// equal to IM_PROTOCOL_ERROR_FAILED.
			//
			GSList *pPermitList;
			
			for (pPermitList = pAccount->permit; pPermitList != NULL; pPermitList = pPermitList->next)
			{
				if (!purple_utf8_strcasecmp(pszNormalizedUTF8Contact, 
						purple_normalize(pAccount, (char *)pPermitList->data)))
				{
					break;
				}
			}
			
			if (pPermitList == NULL)
			{
				retError = IM_PROTOCOL_ERROR_FAILED;
			}
			else
			{
				purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
						new RemovePermitContactSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, pszNormalizedUTF8Contact));
			}
			*/

			g_free(pszNormalizedUTF8Contact);
		}

		g_free(pszUTF8Contact);
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return retError;
}

IMProtocolError CPurpleProtocol::SendFile(const string& strFromUser, const string& strToUser, const string& strFilePath)
{
	if ((strFromUser.length() == 0) ||
		(strToUser.length() == 0) ||
		(strFilePath.length() == 0))
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	string strPurpleUser = GetNormalizedPurpleUserName(strFromUser.c_str());
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleUser.c_str());

	if (pAccount != NULL)
	{
		char* pszUTF8FilePath = purple_utf8_try_convert(strFilePath.c_str());

		if (pszUTF8FilePath != NULL)
		{
			//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
					//new SendFileSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, strToUser.c_str(), pszUTF8FilePath));
			
			serv_send_file(pAccount->gc, strToUser.c_str(), pszUTF8FilePath);

			g_free(pszUTF8FilePath);

			// Release libpurple's thread.
			//
			ReleaseLibPurpleThreadAccess();
			
			return IM_PROTOCOL_ERROR_SUCCESS;
		}

		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_FAILED;
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_NOT_CONNECTED;
}


IMProtocolError CPurpleProtocol::CancelFileTransfer(const std::string& strUser, long lFileTransferId)
{
	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());

	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleUser.c_str());

	if (pAccount != NULL)
	{
		PurpleXfer* pXfer = GetPurpleXfer(lFileTransferId);

		if ((pXfer != NULL) && (pAccount == pXfer->account))
		{
			//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
					//new CancelFileTransferSerializerHelper((CPurpleProtocolSerializer*)this, pXfer));
			
			purple_xfer_cancel_local(pXfer);
		}

		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_SUCCESS;
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_NOT_CONNECTED;
}


IMProtocolError CPurpleProtocol::CancelFileTransfers(const std::string& strUser, const std::string& strContact)
{
	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());
	
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleUser.c_str());

	if (pAccount != NULL)
	{
		PurpleXfer* pXfer = NULL;

		do
		{
			pXfer = GetPurpleXfer(pAccount, strContact);		
			if (pXfer != NULL)
			{
				// Wait for libpurple's thread access.
				WaitLibPurpleThreadAccess();

				// We have to use purple_xfer_cancel_remote(pXfer) here because purple_xfer_cancel_local(pXfer) does not
				// fire OnFileTransferCanceled() but purple_xfer_cancel_remote(pXfer) does.
				purple_xfer_cancel_remote(pXfer);

				// Release libpurple's thread.
				ReleaseLibPurpleThreadAccess();
			}
		}
		while (pXfer != NULL);

		return IM_PROTOCOL_ERROR_SUCCESS;
	}

	return IM_PROTOCOL_ERROR_NOT_CONNECTED;
}


IMProtocolError CPurpleProtocol::Connect(const string& strUser, const string& strPass, CProtocolSettings* pExtraSettings, CProxyInfo* pProxyInfo, CProtocolSettings2* pExtendedCapabilities)
{
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());

	PurpleAccount* pAccount = GetPurpleAccount(strPurpleUser.c_str());

	// If account is not registered, create new account
	if (pAccount == NULL)
	{
		char* pszUTF8User = purple_utf8_try_convert(strPurpleUser.c_str());
		//char* pszUTF8User = g_strdup(purple_normalize(NULL, strUser.c_str()));

		if (pszUTF8User != NULL)
		{
			pAccount = purple_account_new(pszUTF8User, m_strId.c_str());
			g_free(pszUTF8User);
		}

		if (pAccount == NULL)
		{
			// Release libpurple's thread.
			//
			ReleaseLibPurpleThreadAccess();
			
			return IM_PROTOCOL_ERROR_FAILED;
		}

		purple_accounts_add(pAccount);
	}

	// Check if account is connected
	if (purple_account_is_connected(pAccount))
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_ALREADY_CONNECTED;
	}

	// Set password...
	char* pszUTF8Pass = purple_utf8_try_convert(strPass.c_str());
	if (pszUTF8Pass != NULL)
	{
		purple_account_set_password(pAccount, pszUTF8Pass);
		g_free(pszUTF8Pass);
	}
	else
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_FAILED;
	}

	PurpleProxyInfo* pProxy = ProcessProxySettings(pProxyInfo);
	purple_account_set_proxy_info(pAccount, pProxy);

	ProcessExtraSettings(pAccount, pExtraSettings);
	ProcessExtendedCapabilities(pAccount, pExtendedCapabilities);

	//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
			//new ConnectSerializerHelper((CPurpleProtocolSerializer*)this, pAccount));
	
	// Start connection process...
	if (!purple_account_get_enabled(pAccount, purple_core_get_ui()))
	{
		purple_account_set_enabled(pAccount, purple_core_get_ui(), TRUE);
	}
	
	purple_account_connect(pAccount);
	
	//purple_account_set_enabled(pAccount, purple_core_get_ui(), TRUE);

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_SUCCESS;
}


IMProtocolError CPurpleProtocol::SetExtendedCapabilities(const string& strUser, CProtocolSettings2* pExtendedCapabilities)
{
	IMProtocolError retError = IM_PROTOCOL_ERROR_NOT_CONNECTED;
	
	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	string strPurpleUser = GetNormalizedPurpleUserName(strUser.c_str());
	
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleUser.c_str());
	
	if (pAccount != NULL)
	{
		ProcessExtendedCapabilities(pAccount, pExtendedCapabilities);
		
		retError = UpdateServerExtendedCapabilities(pAccount, (pExtendedCapabilities != NULL));
	}
	
	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return retError;
}



bool CPurpleProtocol::HasContactListSupport()
{
	return true;
}


bool CPurpleProtocol::HasCommandSupport(const std::string& strCmdId)
{
	return false;
}


IMProtocolError CPurpleProtocol::SendCommand(const std::string& strCmdId, const std::string& strCmdParameters, const string& strFromUser, const string& strToUser)
{
	if ( (strCmdId.length() == 0) || (strFromUser.length() == 0) ||
			(strToUser.length() == 0) )
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	// Check if command is supported
	if (!HasCommandSupport(strCmdId))
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_COMMAND_NOT_SUPPORTED;
	}
	
	// Check if account is connected
	string strPurpleFromUser = GetNormalizedPurpleUserName(strFromUser.c_str());
	string strPurpleToUser = GetNormalizedContactName(strToUser.c_str());
	
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleFromUser.c_str());
	if (pAccount == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_ACCOUNT_NOT_CONNECTED;
	}
	
	PurpleBuddy* pBuddy = GetPurpleBuddyConnected(pAccount, strPurpleToUser.c_str());
	if (pBuddy == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_CONTACT_NOT_CONNECTED;
	}
	
	IMProtocolError sendCommandError = IM_PROTOCOL_ERROR_FAILED;
	
	if (!FreeToSendCommand(strCmdId, strFromUser, strToUser, sendCommandError))
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return sendCommandError;
	}
	
	char* pszUTF8Destination = purple_utf8_try_convert(strToUser.c_str());
	
	if (pszUTF8Destination != NULL)
	{
		char* pszNormalizedUTF8Destination = g_strdup(purple_normalize(pAccount, pszUTF8Destination));
		
		if (pszNormalizedUTF8Destination != NULL)
		{
			// Get/create conversation associated to account and destination.
			PurpleConversation* pPurpleConv = purple_conversation_new(PURPLE_CONV_TYPE_IM, pAccount, pszNormalizedUTF8Destination);
			
			if (pPurpleConv != NULL)
			{
				// Send Nudge.
				PurpleCmdStatus commandStatus = PURPLE_CMD_STATUS_OK;
				char *markup = NULL;
				char *error = NULL;
				
				std::string strCmdLine = GetProtocolCommandName(strCmdId) + strCmdParameters;
				
				markup = g_markup_escape_text(strCmdLine.c_str(), -1);
				
				commandStatus = purple_cmd_do_command(pPurpleConv, strCmdLine.c_str(), markup, &error);
				
				//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
						//new SendCommandSerializerHelper((CPurpleProtocolSerializer*)this, pPurpleConv, strCmdLine.c_str(), markup));
				
				g_free(markup);
				
				if (commandStatus == PURPLE_CMD_STATUS_NOT_FOUND)
				{
					sendCommandError = IM_PROTOCOL_ERROR_COMMAND_NOT_FOUND;
				}
				else if (commandStatus == PURPLE_CMD_STATUS_WRONG_ARGS)
				{
					sendCommandError = IM_PROTOCOL_ERROR_COMMAND_WRONG_ARGS;
				}
				else if (commandStatus == PURPLE_CMD_STATUS_FAILED)
				{
					sendCommandError = IM_PROTOCOL_ERROR_COMMAND_FAILED;
				}
				else if (commandStatus == PURPLE_CMD_STATUS_WRONG_TYPE)
				{
					if(purple_conversation_get_type(pPurpleConv) == PURPLE_CONV_TYPE_IM)
					{
						sendCommandError = IM_PROTOCOL_ERROR_COMMAND_ONLY_CHAT_ALLOWED;
					}
					else
					{
						sendCommandError = IM_PROTOCOL_ERROR_COMMAND_ONLY_IM_ALLOWED;
					}
				}
				else if (commandStatus == PURPLE_CMD_STATUS_WRONG_PRPL)
				{
					sendCommandError = IM_PROTOCOL_ERROR_COMMAND_WRONG_PRPL;
				}
				else if (commandStatus == PURPLE_CMD_STATUS_OK)
				{
					sendCommandError = IM_PROTOCOL_ERROR_SUCCESS;
				}
				
				sendCommandError = IM_PROTOCOL_ERROR_SUCCESS;
			}
			
			g_free(pszNormalizedUTF8Destination);
		}
		
		g_free(pszUTF8Destination);
	}
	
	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return sendCommandError;
}


bool CPurpleProtocol::FreeToSendCommand(const std::string& strCmdId, const std::string& strFromUser, const std::string& strToUser, IMProtocolError& blockError)
{
	return true;
}


std::string CPurpleProtocol::GetProtocolCommandName(const std::string& strCmdId)
{
	return "";
}


/*
bool CPurpleProtocol::HasSendAttentionSupport(int nAttentionType)
{
	// TODO: Implement this method.
	return false;
}


IMProtocolError CPurpleProtocol::SendAttention(int nAttentionType, const std::string& strFromUser, const std::string& strToUser)
{
	// TODO: Implement this method.
	return IM_PROTOCOL_ERROR_FAILED;
}


bool CPurpleProtocol::FreeToSendAttention(int nAttentionType, const std::string& strFromUser, const std::string& strToUser, IMProtocolError& blockError)
{
	// TODO: Implement this method.
	return true;
}
*/


bool CPurpleProtocol::HasChatSupport()
{
	return false;
}


std::string CPurpleProtocol::GetChatInviteMenuName()
{
    return "";
}


IMProtocolError CPurpleProtocol::ChatInvite(const std::string& strFromUser, const std::string& strToUser, const std::string& strMessage, int& nChatId, bool& bNewChatInvite, const std::string& strChatInviteMenuName)
{
	if ( (strFromUser.length() == 0) || (strToUser.length() == 0) )
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	// Check if account is connected
	string strPurpleFromUser = GetNormalizedPurpleUserName(strFromUser.c_str());
	string strPurpleToUser = GetNormalizedContactName(strToUser.c_str());
	
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleFromUser.c_str());
	if (pAccount == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_ACCOUNT_NOT_CONNECTED;
	}
	
	PurpleBuddy* pBuddy = GetPurpleBuddyConnected(pAccount, strPurpleToUser.c_str());
	if (pBuddy == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_CONTACT_NOT_CONNECTED;
	}
	
	IMProtocolError chatInviteError = IM_PROTOCOL_ERROR_FAILED;
	
	char* pszUTF8Destination = purple_utf8_try_convert(strToUser.c_str());
	char* pszUTF8Message = NULL;
	
	if (pszUTF8Destination != NULL)
	{
		char* pszNormalizedUTF8Destination = g_strdup(purple_normalize(pAccount, pszUTF8Destination));
		
		if (pszNormalizedUTF8Destination != NULL)
		{
			// Check if is a new chat invite
            if (bNewChatInvite)
            {
                if (pBuddy != NULL)
                {
                    PurpleBlistNode* pNode = (PurpleBlistNode*) pBuddy;

                    bool bExecute = ExecuteBListNodeAction(pAccount, pNode, strChatInviteMenuName);

                    if (bExecute)
                    {
                        chatInviteError = IM_PROTOCOL_ERROR_SUCCESS;
                    }
                }
            }
            else
            {            
                // Get conversation associated to account and destination.
                PurpleConversation* pPurpleConv = purple_find_chat(pAccount->gc, nChatId);
                
                if (pPurpleConv != NULL)
                {
                    if (strMessage.length() > 0)
                    {
                        string strFormatedMessage = FormatSendingMessage(strMessage);

                        pszUTF8Message = purple_utf8_try_convert(strFormatedMessage.c_str());
                    }
                    else
                    {
                        pszUTF8Message = purple_utf8_try_convert("");
                    }

                    // Send chat invite.
                    //purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
						//new ChatInviteSerializerHelper((CPurpleProtocolSerializer*)this, 
						//pPurpleConv, pszNormalizedUTF8Destination, pszUTF8Message));
					
					if (0 == strMessage.compare(""))
					{
						serv_chat_invite(purple_conversation_get_gc(pPurpleConv), 
							purple_conv_chat_get_id(PURPLE_CONV_CHAT(pPurpleConv)), 
							NULL, pszNormalizedUTF8Destination);
					}
					else
					{
						serv_chat_invite(purple_conversation_get_gc(pPurpleConv), 
							purple_conv_chat_get_id(PURPLE_CONV_CHAT(pPurpleConv)), 
							pszUTF8Message, pszNormalizedUTF8Destination);
					}

                    // Save chat id.
                    nChatId = purple_conv_chat_get_id(PURPLE_CONV_CHAT(pPurpleConv));

                    chatInviteError = IM_PROTOCOL_ERROR_SUCCESS;

                    if (pszUTF8Message)
                    {
                        g_free(pszUTF8Message);
                    }
                }

                g_free(pszNormalizedUTF8Destination);
            }
		}
		
		g_free(pszUTF8Destination);
	}
	
	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return chatInviteError;
}


IMProtocolError CPurpleProtocol::ChatLeave(const std::string& strFromUser, const int& nChatId)
{
	if (strFromUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	// Check if account is connected
	string strPurpleFromUser = GetNormalizedPurpleUserName(strFromUser.c_str());
	
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleFromUser.c_str());
	if (pAccount == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_ACCOUNT_NOT_CONNECTED;
	}
	
	IMProtocolError chatLeaveError = IM_PROTOCOL_ERROR_FAILED;
	
    // Get/create conversation associated to account and destination.
    PurpleConversation* pPurpleConv = purple_find_chat(pAccount->gc, nChatId);
    
    if (pPurpleConv != NULL)
    {
        // Send chat leave.
        //purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
            //new ChatLeaveSerializerHelper((CPurpleProtocolSerializer*)this, pPurpleConv));
		
		serv_chat_leave(purple_conversation_get_gc(pPurpleConv), 
				purple_conv_chat_get_id(PURPLE_CONV_CHAT(pPurpleConv)));

        chatLeaveError = IM_PROTOCOL_ERROR_SUCCESS;
    }
			
	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return chatLeaveError;
}


IMProtocolError CPurpleProtocol::ChatSend(const std::string& strFromUser, const std::string& strMessage, const int& nChatId)
{
	if ( (strFromUser.length() == 0) || (strMessage.length() == 0) )
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	// Check if account is connected
	string strPurpleFromUser = GetNormalizedPurpleUserName(strFromUser.c_str());
	
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleFromUser.c_str());
	if (pAccount == NULL)
	{
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_ACCOUNT_NOT_CONNECTED;
	}
	
	IMProtocolError chatSendError = IM_PROTOCOL_ERROR_FAILED;
	
	char* pszUTF8Message = NULL;
	
    string strFormatedMessage = FormatSendingMessage(strMessage);

    pszUTF8Message = purple_utf8_try_convert(strFormatedMessage.c_str());

    if (pszUTF8Message != NULL)
    {
        // Get/create conversation associated to account and destination.
        PurpleConversation* pPurpleConv = purple_find_chat(pAccount->gc, nChatId);
        
        if (pPurpleConv != NULL)
        {
            // Send chat message.
            //purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
                //new ChatSendSerializerHelper((CPurpleProtocolSerializer*)this, 
                //pPurpleConv, pszUTF8Message));
			
			purple_conv_chat_send(PURPLE_CONV_CHAT(pPurpleConv), pszUTF8Message);

            chatSendError = IM_PROTOCOL_ERROR_SUCCESS;
        }

        g_free(pszUTF8Message);
    }

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return chatSendError;
}


bool CPurpleProtocol::HasCustomEmoticonSupport()
{
	return false;
}


/*
IMProtocolError CPurpleProtocol::AddCustomEmoticonToAccount(const std::string& strFromUser, 
		const std::string& strCustomEmoticonShortcut, const std::string& strCustomEmoticonFileName)
{
	if ( (strFromUser.length() == 0) || (strCustomEmoticonShortcut.length() == 0) ||
			(strCustomEmoticonFileName.length() == 0) )
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	// Get the account.
	string strPurpleFromUser = GetNormalizedPurpleUserName(strFromUser.c_str());
	PurpleAccount* pAccount = GetPurpleAccount(strPurpleFromUser.c_str());
	if (pAccount == NULL)
	{
		return IM_PROTOCOL_ERROR_ACCOUNT_NOT_FOUND;
	}
	
	// Add the custom emoticon to user account.
	purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
			new AddCustomEmoticonToAccountSerializerHelper((CPurpleProtocolSerializer*)this, 
					pAccount, strCustomEmoticonShortcut.c_str(), strCustomEmoticonFileName.c_str()));
	
	return IM_PROTOCOL_ERROR_SUCCESS;
}


IMProtocolError CPurpleProtocol::RemoveCustomEmoticonFromAccount(const std::string& strFromUser, 
		const std::string& strCustomEmoticonShortcut)
{
	if ( (strFromUser.length() == 0) || (strCustomEmoticonShortcut.length() == 0) )
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	// Check if account is connected
	string strPurpleFromUser = GetNormalizedPurpleUserName(strFromUser.c_str());
	
	PurpleAccount* pAccount = GetPurpleAccountConnected(strPurpleFromUser.c_str());
	if (pAccount == NULL)
	{
		return IM_PROTOCOL_ERROR_ACCOUNT_NOT_CONNECTED;
	}
	
	// Add the custom emoticon to user account.
	purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
			new RemoveCustomEmoticonToAccountSerializerHelper((CPurpleProtocolSerializer*)this, 
					pAccount, strCustomEmoticonShortcut.c_str()));
	
	return IM_PROTOCOL_ERROR_SUCCESS;
}
*/


IMProtocolError CPurpleProtocol::AddCustomEmoticon(
		const std::string& strCustomEmoticonShortcut, const std::string& strCustomEmoticonFileName)
{
	if ( (strCustomEmoticonShortcut.length() == 0) || (strCustomEmoticonFileName.length() == 0) )
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	char* pszUTF8_Shortcut = purple_utf8_try_convert(strCustomEmoticonShortcut.c_str());
	
	if (pszUTF8_Shortcut != NULL)
	{
		PurpleSmiley* pSmiley = purple_smileys_find_by_shortcut(pszUTF8_Shortcut);

		if (pSmiley)
		{
			// Release libpurple's thread.
			//
			ReleaseLibPurpleThreadAccess();
			
			return IM_PROTOCOL_ERROR_EMOTICON_ALREADY_EXIST;
		}

		char* pszUTF8CustomEmoticonFileName = purple_utf8_try_convert(strCustomEmoticonFileName.c_str());
		pSmiley = purple_smiley_new_from_file(pszUTF8_Shortcut,	pszUTF8CustomEmoticonFileName);

		if (!pSmiley)
		{
			// Release libpurple's thread.
			//
			ReleaseLibPurpleThreadAccess();
			
			return IM_PROTOCOL_ERROR_CANT_CREATE_EMOTICON;
		}

		// Add the custom emoticon to Smiley API.
		//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
			//new AddCustomEmoticonSerializerHelper((CPurpleProtocolSerializer*)this, pSmiley));
		
		// Removed because purple_smiley_new_from_file calls it automatically
		// http://pidgin.im/pipermail/commits/2008-May/008427.html
		//purple_smileys_add(pSmiley);

		g_free(pszUTF8_Shortcut);
		g_free(pszUTF8CustomEmoticonFileName);

		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_SUCCESS;
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_CANT_CREATE_EMOTICON;
}


IMProtocolError CPurpleProtocol::RemoveCustomEmoticon(const std::string& strCustomEmoticonShortcut)
{
	if (strCustomEmoticonShortcut.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	char* pszUTF8_Shortcut = purple_utf8_try_convert(strCustomEmoticonShortcut.c_str());

	if (pszUTF8_Shortcut != NULL)
	{
		PurpleSmiley* pSmiley = purple_smileys_find_by_shortcut(pszUTF8_Shortcut);

		if (!pSmiley)
		{
			// Release libpurple's thread.
			//
			ReleaseLibPurpleThreadAccess();
			
			return IM_PROTOCOL_ERROR_EMOTICON_NOT_FOUND;
		}

		// Add the custom emoticon to Smiley API.
		//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
			//new RemoveCustomEmoticonSerializerHelper((CPurpleProtocolSerializer*)this, pSmiley));
		
		purple_smiley_delete(pSmiley);

		g_free(pszUTF8_Shortcut);

		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_SUCCESS;
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_EMOTICON_NOT_FOUND;
}


IMProtocolError CPurpleProtocol::ChangeCustomEmoticonShortcut(const std::string& strCustomEmoticonOldShortcut, const std::string& strCustomEmoticonNewShortcut)
{
	if ( (strCustomEmoticonOldShortcut.length() == 0) || (strCustomEmoticonNewShortcut.length() == 0) )
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	char* pszUTF8_OldShortcut = purple_utf8_try_convert(strCustomEmoticonOldShortcut.c_str());
	char* pszUTF8_NewShortcut = purple_utf8_try_convert(strCustomEmoticonNewShortcut.c_str());

	if ((pszUTF8_OldShortcut != NULL) && (pszUTF8_NewShortcut != NULL))
	{	
		PurpleSmiley* pSmiley = purple_smileys_find_by_shortcut(pszUTF8_OldShortcut);

		if (!pSmiley)
		{
			// Release libpurple's thread.
			//
			ReleaseLibPurpleThreadAccess();
			
			return IM_PROTOCOL_ERROR_EMOTICON_NOT_FOUND;
		}

		// Add the custom emoticon to Smiley API.
		//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
			//new ChangeCustomEmoticonShortcutSerializerHelper((CPurpleProtocolSerializer*)this, 
			//pSmiley, pszUTF8_NewShortcut));
		
		purple_smiley_set_shortcut(pSmiley, pszUTF8_NewShortcut);

		g_free(pszUTF8_OldShortcut);
		g_free(pszUTF8_NewShortcut);

		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_SUCCESS;
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_EMOTICON_NOT_FOUND;
}


IMProtocolError CPurpleProtocol::AcceptFileAsync(const std::string& strUser, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	if ( (strUser.length() == 0) || (pFileTransferAsyncInfo == NULL) )
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	CPurpleRequestFileTransferAsyncInfo* pPurpleFileTransferAsyncInfo = 
			dynamic_cast<CPurpleRequestFileTransferAsyncInfo*>(pFileTransferAsyncInfo);
	
	if (pPurpleFileTransferAsyncInfo == NULL)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	if ( (pPurpleFileTransferAsyncInfo->GetOkCallback() != NULL) && 
			(pPurpleFileTransferAsyncInfo->GetPurpleXFer()->status != PURPLE_XFER_STATUS_CANCEL_REMOTE))
	{
		PurpleRequestActionCb pfnRequestActionCallback = 
				(PurpleRequestActionCb) pPurpleFileTransferAsyncInfo->GetOkCallback();
		
		pfnRequestActionCallback(pPurpleFileTransferAsyncInfo->GetUserData(), 0);
		
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_SUCCESS;
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_EMOTICON_NOT_FOUND;
}


IMProtocolError CPurpleProtocol::RejectFileAsync(const std::string& strUser, CRequestFileTransferAsyncInfo* pFileTransferAsyncInfo)
{
	if ( (strUser.length() == 0) || (pFileTransferAsyncInfo == NULL) )
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	CPurpleRequestFileTransferAsyncInfo* pPurpleFileTransferAsyncInfo = 
			dynamic_cast<CPurpleRequestFileTransferAsyncInfo*>(pFileTransferAsyncInfo);
	
	if (pPurpleFileTransferAsyncInfo == NULL)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	// Wait for libpurple's thread access.
	//
	WaitLibPurpleThreadAccess();
	
	if ( (pPurpleFileTransferAsyncInfo->GetCancelCallback() != NULL) && 
			(pPurpleFileTransferAsyncInfo->GetPurpleXFer()->status != PURPLE_XFER_STATUS_CANCEL_REMOTE))
	{
		PurpleRequestActionCb pfnRequestActionCallback = 
				(PurpleRequestActionCb) pPurpleFileTransferAsyncInfo->GetCancelCallback();
		
		pfnRequestActionCallback(pPurpleFileTransferAsyncInfo->GetUserData(), 0);
		
		RemovePurpleXfer(pPurpleFileTransferAsyncInfo->GetId());
		
		// Release libpurple's thread.
		//
		ReleaseLibPurpleThreadAccess();
		
		return IM_PROTOCOL_ERROR_SUCCESS;
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_EMOTICON_NOT_FOUND;
}


IMProtocolError CPurpleProtocol::DisconnectAsync(const std::string& strUser)
{
	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->GetAsyncOperationsExec())
	{
		return CPurpleService::GetInstance()->GetAsyncOperationsExec()->
				DisconnectAsync(this, strUser);
	}
	
	return IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
}


IMProtocolError CPurpleProtocol::SetAliasAsync(const std::string& strUser, const std::string& strAlias)
{
	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->GetAsyncOperationsExec())
	{
		return CPurpleService::GetInstance()->GetAsyncOperationsExec()->
				SetAliasAsync(this, strUser, strAlias);
	}
	
	return IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
}


IMProtocolError CPurpleProtocol::SetUserDisplayIconAsync(const string& strUser, const string& strFile)
{
	if ( (strUser.length() == 0) || (strFile.length() == 0) )
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->GetAsyncOperationsExec())
	{
		return CPurpleService::GetInstance()->GetAsyncOperationsExec()->
				SetUserDisplayIconAsync(this, strUser, strFile);
	}
	
	return IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
}


IMProtocolError CPurpleProtocol::SetUserStatusAsync(const string& strUser, int nStatus)
{
	return SetUserStatusAsync(strUser, nStatus, "");
}


IMProtocolError CPurpleProtocol::AliasContactAsync(const string& strUser, const string& strContact, const string& strAlias)
{
	if ( (strUser.length() == 0) || (strContact.length() == 0) )
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->GetAsyncOperationsExec())
	{
		return CPurpleService::GetInstance()->GetAsyncOperationsExec()->
				AliasContactAsync(this, strUser, strContact, strAlias);
	}
	
	return IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
}


bool CPurpleProtocol::HasUserAliasSupport()
{
	return false;
}

// CIMProtocol6
IMProtocolError CPurpleProtocol::SetUserStatus(const std::string& strUser, int nStatus, const std::string& strStatusMessage)
{
	gboolean changed = TRUE;
	PurpleStatus *status;
	string strStatusText;
	char* pszStatusText;
	
	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
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

	if (pAccount->gc != NULL)
	{
		// Get status text
		if (GetStatusText(nStatus, strStatusText))
		{
			// Update status
			//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
					//new SetUserStatusSerializerHelper((CPurpleProtocolSerializer*)this, pAccount, strStatusText.c_str()));
			
			// Update status...
			if (!strStatusMessage.empty())
			{
				char* pszStatusMessage = purple_utf8_try_convert(strStatusMessage.c_str());
				if (pszStatusMessage != NULL)
				{
					purple_account_set_status(pAccount, strStatusText.c_str(), TRUE, "message", pszStatusMessage, NULL);
				}
				g_free(pszStatusMessage);
			}
			else
			{
				purple_account_set_status(pAccount, strStatusText.c_str(), TRUE, NULL);
			}
			
			// Release libpurple's thread.
			//
			ReleaseLibPurpleThreadAccess();
			
			return IM_PROTOCOL_ERROR_SUCCESS;
		}
	}

	// Release libpurple's thread.
	//
	ReleaseLibPurpleThreadAccess();
	
	return IM_PROTOCOL_ERROR_FAILED;
}


IMProtocolError CPurpleProtocol::SetUserStatusAsync(const std::string& strUser, int nStatus, const std::string& strStatusMessage)
{
	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->GetAsyncOperationsExec())
	{
		return CPurpleService::GetInstance()->GetAsyncOperationsExec()->
				SetUserStatusAsync(this, strUser, nStatus, strStatusMessage);
	}
	
	return IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
}


bool CPurpleProtocol::HasOfflineMessageSupport()
{
	return false;
}


// CIMProtocol7
IMProtocolError CPurpleProtocol::SetExtendedCapabilitiesAsync(
		const std::string& strUser, CProtocolSettings2* pExtendedCapabilities)
{
	if (strUser.length() == 0)
	{
		return IM_PROTOCOL_ERROR_INVALID_ARG;
	}
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->GetAsyncOperationsExec())
	{
		return CPurpleService::GetInstance()->GetAsyncOperationsExec()->
				SetExtendedCapabilitiesAsync(this, strUser, pExtendedCapabilities);
	}
	
	return IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
}


//
// Purple service callbacks
//
void CPurpleProtocol::OnConnectProgress(PurpleAccount* /*pAccount*/, const char* pszText, int /*nStep*/, int /*nStepCount*/)
{
}

void CPurpleProtocol::OnConnected(PurpleAccount* pAccount)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if (pCallback != NULL)
		{
			m_callbackNotifier.HandleOnConnected(pCallback, strNormalizedUser);
		}
	}
}

void CPurpleProtocol::OnDisconnected(PurpleAccount* pAccount)
{
	if (pAccount->username != NULL)
	{
		// Remove account buddies...
		CPurpleBuddyListHelper::RemovePurpleAccountBuddies(pAccount);

		// Notify callback
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if (pCallback != NULL)
		{
			m_callbackNotifier.HandleOnDisconnected(pCallback, strNormalizedUser);
		}
	}
}

void CPurpleProtocol::OnReportDisconnect(PurpleAccount* pAccount, const char* pszText)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if (pCallback != NULL)
		{
			ConnectionError errorCode = CONNECTION_ERROR_UNKNOWN;

			string strLocaleText;
			if (pszText != NULL)
			{
				GetLocaleTextFromUTF8(pszText, strLocaleText);

				GetConnectionErrorCode(errorCode, strLocaleText.c_str());
			}
			
			m_callbackNotifier.HandleOnReportDisconnect(pCallback, strNormalizedUser, errorCode, strLocaleText);
		}
	}
}


void CPurpleProtocol::OnNotifyAdded(PurpleAccount* pAccount, const char* pszContact)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		string strContact("");
		GetLocaleTextFromUTF8(pszContact, strContact);

		strContact = GetNormalizedContactName(strContact.c_str());

		m_callbackNotifier.HandleOnNotifyAdded(pCallback, strNormalizedUser, strContact);
	}
}


void CPurpleProtocol::OnContactListUpdate(PurpleAccount* pAccount, PurpleBuddy* pBuddy, PurpleGroup* pGroup)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pBuddy->name != NULL) &&
			(pGroup->name != NULL))
		{
			string strAlias = pBuddy->name;

			const char* pszAlias = purple_buddy_get_alias(pBuddy);

			if (pszAlias != NULL)
			{				
				GetLocaleTextFromUTF8(pszAlias, strAlias);
			}

			string strLocaleGroup;
			GetLocaleTextFromUTF8(pGroup->name, strLocaleGroup);

			string strNormalizedContact = GetNormalizedContactName(pBuddy->name);

			CContactInfo contactInfo;
			contactInfo.SetUserName(strNormalizedContact);
			contactInfo.SetAlias(strAlias);

			pCallback->OnContactListUpdate(strNormalizedUser, &contactInfo, strLocaleGroup);
			
			// Here the "CanRequestAuthorization" checking must be after the 
			// normal pCallback execution.
			//
			CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
			
			if (pCallback6)
			{
				bool bCanRequest = false;
				CanRequestContactAuthorization(strNormalizedUser, strNormalizedContact, bCanRequest);
				
				pCallback6->OnContactCanRequestAuthorization(strNormalizedUser, 
						strNormalizedContact, (bCanRequest == true) ? TRUE : FALSE);
			}
		}
	}
}

void CPurpleProtocol::OnContactRemoved(PurpleAccount* pAccount, PurpleBuddy* pBuddy, PurpleGroup* pGroup)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pBuddy->name != NULL) &&
			(pGroup->name != NULL))
		{
			string strLocaleGroup;
			GetLocaleTextFromUTF8(pGroup->name, strLocaleGroup);

			string strNormalizedContact = GetNormalizedContactName(pBuddy->name);

			pCallback->OnContactRemoved(strNormalizedUser, strNormalizedContact, strLocaleGroup);
		}
	}
}


void CPurpleProtocol::OnContactIconReceived(PurpleAccount* pAccount, PurpleBuddy* pBuddy, PurpleBuddyIcon* pBuddyIcon)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pBuddy->name != NULL))
		{
			const char* pszIconData;
			size_t nLen = 0;
							
			pszIconData = (const char*) purple_buddy_icon_get_data(pBuddyIcon, &nLen);
							
			if ((pszIconData != NULL) && (nLen > 0 ))
			{
				string strNormalizedContact = GetNormalizedContactName(pBuddy->name);

				pCallback->OnContactIconReceived(strNormalizedUser, strNormalizedContact, pszIconData, (int) nLen);
			}
		}
	}
}

void CPurpleProtocol::OnContactAliasReceived(PurpleAccount* pAccount, PurpleBuddy* pBuddy, const char* pszAlias)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pBuddy->name != NULL))
		{
			string strAlias("");

			if (pszAlias != NULL)
			{
				GetLocaleTextFromUTF8(pszAlias, strAlias);
			}

			string strNormalizedContact = GetNormalizedContactName(pBuddy->name);

			pCallback->OnContactAliasReceived(strNormalizedUser, strNormalizedContact, strAlias);
		}
	}
}


void CPurpleProtocol::OnGroupAdded(PurpleGroup* pGroup)
{
	CIMProtocolCallback* pCallback = NULL;

	if (pGroup->name != NULL)
	{
		string strGroup("");

		GetLocaleTextFromUTF8(pGroup->name, strGroup);

		string strUser;
		CCallbackMap::iterator it;
		PurpleAccount* pAccount;
		
		for (it = m_mapCallback.begin(); 
			 it != m_mapCallback.end(); 
			 ++it)
		{
			strUser = GetNormalizedPurpleUserName((it->first));

			pAccount = GetPurpleAccountConnected(strUser.c_str());

			if (pAccount != NULL)
			{
				pCallback = it->second;
				pCallback->OnGroupAdded(strUser, strGroup);
			}
		}
	}
}


void CPurpleProtocol::OnIMReceived(PurpleAccount* pAccount, const char* pszFromUser, const char* pszText)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pszText != NULL))
		{
			string strFormatedMessage = FormatReceivedMessage(pszText);

			string strMessage("");
			GetLocaleTextFromUTF8(strFormatedMessage.c_str(), strMessage);

			string strNormalizedContact = GetNormalizedContactName(pszFromUser);

			pCallback->OnIMReceived(strNormalizedUser, strNormalizedContact, strMessage);
		}
	}
}


void CPurpleProtocol::OnIMSent(PurpleAccount* pAccount, const char* pszToUser, const char* pszText)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pszText != NULL))
		{
			string strFormatedMessage = FormatSentMessage(pszText);

			// Convert to locale encoding
			string strMessage("");
			GetLocaleTextFromUTF8(strFormatedMessage.c_str(), strMessage);

			string strNormalizedContact = GetNormalizedContactName(pszToUser);

			pCallback->OnIMSent(strNormalizedUser, strNormalizedContact, strMessage);
		}
	}
}


void CPurpleProtocol::OnContactSignedOn(PurpleAccount* pAccount, PurpleBuddy* pBuddy)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pBuddy->name != NULL))
		{
			string strNormalizedContact = GetNormalizedContactName(pBuddy->name);
			
			// Here the "CanRequestAuthorization" checking must be before the 
			// normal pCallback execution.
			//
			CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
			
			if (pCallback6)
			{
				bool bCanRequest = false;
				CanRequestContactAuthorization(strNormalizedUser, strNormalizedContact, bCanRequest);
				
				pCallback6->OnContactCanRequestAuthorization(strNormalizedUser, 
						strNormalizedContact, (bCanRequest == true) ? TRUE : FALSE);
			}

			pCallback->OnContactSignedOn(strNormalizedUser, strNormalizedContact);
		}
	}
}

void CPurpleProtocol::OnContactSignedOff(PurpleAccount* pAccount, PurpleBuddy* pBuddy)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pBuddy->name != NULL))
		{
			string strNormalizedContact = GetNormalizedContactName(pBuddy->name);

			pCallback->OnContactSignedOff(strNormalizedUser, strNormalizedContact);
		}
	}
}

void CPurpleProtocol::OnContactAway(PurpleAccount* pAccount, PurpleBuddy* pBuddy)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pBuddy->name != NULL))
		{
			string strNormalizedContact = GetNormalizedContactName(pBuddy->name);

			pCallback->OnContactAway(strNormalizedUser, strNormalizedContact);
		}
	}
}

void CPurpleProtocol::OnContactAwayReturn(PurpleAccount* pAccount, PurpleBuddy* pBuddy)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pBuddy->name != NULL))
		{
			string strNormalizedContact = GetNormalizedContactName(pBuddy->name);

			pCallback->OnContactAwayReturn(strNormalizedUser, strNormalizedContact);
		}
	}
}

void CPurpleProtocol::OnContactIdle(PurpleAccount* pAccount, PurpleBuddy* pBuddy)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pBuddy->name != NULL))
		{
			string strNormalizedContact = GetNormalizedContactName(pBuddy->name);

			pCallback->OnContactIdle(strNormalizedUser, strNormalizedContact);
		}
	}
}

void CPurpleProtocol::OnContactIdleReturn(PurpleAccount* pAccount, PurpleBuddy* pBuddy)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pBuddy->name != NULL))
		{
			string strNormalizedContact = GetNormalizedContactName(pBuddy->name);

			pCallback->OnContactIdleReturn(strNormalizedUser, strNormalizedContact);
		}
	}
}

void CPurpleProtocol::OnContactTyping(PurpleAccount* pAccount, const char* pszFromUser, PurpleTypingState purpleTypingState)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pszFromUser != NULL))
		{
			TypingState typingState = NOT_TYPING;

			if (purpleTypingState == PURPLE_TYPING)
			{
				typingState = TYPING;
			}
			else if (purpleTypingState == PURPLE_TYPED)
			{
				typingState = TYPED;
			}

			string strNormalizedContact = GetNormalizedContactName(pszFromUser);

			pCallback->OnContactTyping(strNormalizedUser, strNormalizedContact, typingState);
		}
	}
}


void CPurpleProtocol::OnContactStatusChanged(PurpleAccount* pAccount, PurpleBuddy* pBuddy)
{
	if (pAccount->username != NULL)
	{
		// Get Contact status message
		string strStatusMessage("");
		PurplePluginProtocolInfo *prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(pAccount->gc->prpl);
		if (prpl_info && prpl_info->status_text) 
		{
			GetLocaleTextFromUTF8(prpl_info->status_text(pBuddy), strStatusMessage);
			strStatusMessage = UnescapeHTML(strStatusMessage.c_str());
		}
		
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);
		
		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
		
		if ((pCallback != NULL) &&
			(pBuddy != NULL))
		{
			PurplePresence* pBuddyPresence = purple_buddy_get_presence(pBuddy);
			
			if (pBuddyPresence != NULL)
			{
				PurpleStatus* pBuddyStatus = purple_presence_get_active_status(pBuddyPresence);
				
				if (pBuddyStatus)
				{
					const char* pszStatusText = purple_status_get_id(pBuddyStatus);
					
					// Especial condition on which the primitive status must overlap 
					// main status type.
					//
					if (purple_presence_is_status_primitive_active(pBuddyPresence, PURPLE_STATUS_MOBILE))
					{
						pszStatusText = purple_primitive_get_id_from_type(PURPLE_STATUS_MOBILE);
					}
					
					if (pszStatusText != NULL)
					{
						string strStatusText("");
						GetLocaleTextFromUTF8(pszStatusText, strStatusText);
						
						int nStatus;
						if ((strStatusText.length() > 0) && 
							(GetStatusCode(strStatusText, nStatus)) && 
							(pBuddy->name))
						{
							string strNormalizedContact = GetNormalizedContactName(pBuddy->name);
							
							CIMProtocolCallback7* pCallback7 = dynamic_cast<CIMProtocolCallback7*>(pCallback);
							if (pCallback7)
							{
								pCallback7->OnContactStatusChanged(strNormalizedUser, strNormalizedContact, nStatus, strStatusMessage);
							}
						}
					}
				}
			}
		}
	}
}



void CPurpleProtocol::OnPermitAdded(PurpleAccount* pAccount, const char* pszUser)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if (pCallback != NULL)
		{
			string strNormalizedContact = GetNormalizedContactName(pszUser);

			pCallback->OnPermitAdded(strNormalizedUser, strNormalizedContact);
		}
	}
}

void CPurpleProtocol::OnDenyAdded(PurpleAccount* pAccount, const char* pszUser)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if (pCallback != NULL)
		{
			string strNormalizedContact = GetNormalizedContactName(pszUser);

			pCallback->OnDenyAdded(strNormalizedUser, strNormalizedContact);
		}
	}
}

void CPurpleProtocol::OnPermitRemoved(PurpleAccount* pAccount, const char* pszUser)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if (pCallback != NULL)
		{
			string strNormalizedContact = GetNormalizedContactName(pszUser);

			pCallback->OnPermitRemoved(strNormalizedUser, strNormalizedContact);
		}
	}
}

void CPurpleProtocol::OnDenyRemoved(PurpleAccount* pAccount, const char* pszUser)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if (pCallback != NULL)
		{
			string strNormalizedContact = GetNormalizedContactName(pszUser);

			pCallback->OnDenyRemoved(strNormalizedUser, strNormalizedContact);
		}
	}
}


void CPurpleProtocol::OnFileTransferAdded(PurpleXfer* pXfer)
{
	if ((pXfer->account != NULL) &&
		(pXfer->account->username != NULL))
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pXfer->account->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ((pCallback != NULL) &&
			(pXfer->who != NULL))
		{
			// Now it will be probably always be initialized by method 
			// "ProcessRequestFileTransferAction".
			//
			if (pXfer->ui_data == 0)
			{
				AddPurpleXfer(pXfer);
			}

			CPurpleFileTransferInfo fileTransferInfo(pXfer);

			string strNormalizedContact = GetNormalizedContactName(pXfer->who);

			pCallback->OnFileTransferInit(strNormalizedUser, strNormalizedContact, &fileTransferInfo);
		}
	}
}


void CPurpleProtocol::OnFileTransferDestroyed(PurpleXfer* pXfer)
{
	unsigned long lTransferId = (unsigned long) pXfer->ui_data;

	RemovePurpleXfer(lTransferId);
}


void CPurpleProtocol::OnFileTransferUpdateProgress(PurpleXfer* pXfer, double /*dPercent*/)
{
	if ((pXfer->account != NULL) &&
		(pXfer->account->username != NULL) &&
		(pXfer->filename != NULL) &&
		(pXfer->who != NULL))
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pXfer->account->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if (pCallback != NULL)
		{
			CPurpleFileTransferInfo fileTransferInfo(pXfer);

			string strNormalizedContact = GetNormalizedContactName(pXfer->who);

			pCallback->OnFileTransferUpdateProgress(strNormalizedUser, strNormalizedContact, &fileTransferInfo);
		}
	}
}


void CPurpleProtocol::OnFileTransferCanceled(PurpleXfer* pXfer)
{
	unsigned long lTransferId = (unsigned long) pXfer->ui_data;

	RemovePurpleXfer(lTransferId);

	if ((pXfer->account != NULL) &&
		(pXfer->account->username != NULL) &&
		(pXfer->filename != NULL) &&
		(pXfer->who != NULL))
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pXfer->account->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if (pCallback != NULL)
		{
			CPurpleFileTransferInfo fileTransferInfo(pXfer);

			string strNormalizedContact = GetNormalizedContactName(pXfer->who);

			pCallback->OnFileTransferCanceled(strNormalizedUser, strNormalizedContact, &fileTransferInfo);
		}
	}
}


CustomEmoticonAddResult CPurpleProtocol::OnCustomSmileyAdd(PurpleConversation* pConversation, const char* pszSmiley, gboolean bRemote)
{
	if ( (pConversation->account != NULL) &&
			(pConversation->account->username != NULL) && 
			(pConversation->name != NULL) )
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pConversation->account->username);
		
		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
		
		if (pCallback != NULL)
		{
			CIMProtocolCallback5* pCallback5 = dynamic_cast<CIMProtocolCallback5*>(pCallback);
			
			if (pCallback5)
			{
				string strNormalizedContact = GetNormalizedContactName(pConversation->name);
				string strSmiley(pszSmiley);
				
				return pCallback5->OnCustomSmileyAdd(strNormalizedUser, strNormalizedContact, strSmiley, bRemote);
			}
		}
	}
	
	return CUSTOM_EMOTICON_ADD_RESULT_ERROR;
}


void CPurpleProtocol::OnCustomSmileyWrite(PurpleConversation* pConversation, const char* pszSmiley, const guchar* pData, gsize nSize, const char* pszFileExtension)
{
	if ( (pConversation->account != NULL) &&
			(pConversation->account->username != NULL) && 
			(pConversation->name != NULL) )
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pConversation->account->username);
		
		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
		
		if (pCallback != NULL)
		{
			CIMProtocolCallback5* pCallback5 = dynamic_cast<CIMProtocolCallback5*>(pCallback);
			
			if (pCallback5)
			{
				string strNormalizedContact = GetNormalizedContactName(pConversation->name);
				string strSmiley(pszSmiley);
				
				pCallback5->OnCustomSmileyWrite(strNormalizedUser, strNormalizedContact, strSmiley, pData, nSize, pszFileExtension);
			}
		}
	}
}


void CPurpleProtocol::OnCustomSmileyClose(PurpleConversation* pConversation, const char* pszSmiley)
{
	if ( (pConversation->account != NULL) &&
			(pConversation->account->username != NULL) && 
			(pConversation->name != NULL) )
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pConversation->account->username);
		
		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
		
		if (pCallback != NULL)
		{
			CIMProtocolCallback5* pCallback5 = dynamic_cast<CIMProtocolCallback5*>(pCallback);
			
			if (pCallback5)
			{
				string strNormalizedContact = GetNormalizedContactName(pConversation->name);
				string strSmiley(pszSmiley);
				
				return pCallback5->OnCustomSmileyClose(strNormalizedUser, strNormalizedContact, strSmiley);
			}
		}
	}
}


void CPurpleProtocol::OnIMReceived(PurpleAccount* pAccount, const char* pszFromUser, const char* pszText, const char* pszSmileyList)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);
		
		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
		
		if ( (pCallback != NULL) && (pszText != NULL) )
		{
			CIMProtocolCallback5* pCallback5 = dynamic_cast<CIMProtocolCallback5*>(pCallback);
			
			if (pCallback5)
			{
				string strFormatedMessage = FormatReceivedMessage(pszText);
				
				string strMessage("");
				GetLocaleTextFromUTF8(strFormatedMessage.c_str(), strMessage);
				
				string strSmileyList(pszSmileyList);
				
				string strNormalizedContact = GetNormalizedContactName(pszFromUser);
				
				pCallback5->OnIMReceived(strNormalizedUser, strNormalizedContact, strMessage, strSmileyList);
			}
		}
	}
}


void CPurpleProtocol::OnChatAddUser(PurpleAccount* pAccount, const int nChatId, const char* pszChatContactName, const char* pszChatContactAlias, const int bIsContact)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);
		
		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
		
		if ( (pCallback != NULL) && (nChatId != -1) && (pszChatContactName != NULL) )
		{
			CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
			
			if (pCallback6)
			{
				string strNormalizedChatContactName = GetNormalizedContactName(pszChatContactName);
				string strChatContactAlias(pszChatContactAlias);
				
				pCallback6->OnChatAddUser(strNormalizedUser, nChatId, strNormalizedChatContactName, strChatContactAlias, bIsContact);
			}
		}
	}
}


void CPurpleProtocol::OnChatRenameUser(PurpleAccount* pAccount, const int nChatId, const char* pszChatContactOldName, const char* pszChatContactNewName, const char* pszChatContactNewAlias)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);
		
		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
		
		if ( (pCallback != NULL) && (nChatId != -1) && 
				(pszChatContactOldName != NULL) && (pszChatContactNewName != NULL) )
		{
			CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
			
			if (pCallback6)
			{
				string strNormalizedChatContactOldName = GetNormalizedContactName(pszChatContactOldName);
				string strNormalizedChatContactNewName = GetNormalizedContactName(pszChatContactNewName);
				string strChatContactNewAlias(pszChatContactNewAlias);
				
				pCallback6->OnChatRenameUser(strNormalizedUser, nChatId, strNormalizedChatContactOldName, 
						strNormalizedChatContactNewName, strChatContactNewAlias);
			}
		}
	}
}


void CPurpleProtocol::OnChatRemoveUser(PurpleAccount* pAccount, const int nChatId, const char* pszChatContactName)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);
		
		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
		
		if ( (pCallback != NULL) && (nChatId != -1) && (pszChatContactName != NULL) )
		{
			CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
			
			if (pCallback6)
			{
				string strNormalizedChatContactName = GetNormalizedContactName(pszChatContactName);
				
				pCallback6->OnChatRemoveUser(strNormalizedUser, nChatId, strNormalizedChatContactName);
			}
		}
	}
}


void CPurpleProtocol::OnChatUpdateUser(PurpleAccount* pAccount, const int nChatId, const char* pszChatContactName, const char* pszChatContactAlias, const int bIsContact)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);
		
		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
		
		if ( (pCallback != NULL) && (nChatId != -1) && (pszChatContactName != NULL) )
		{
			CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
			
			if (pCallback6)
			{
				string strNormalizedChatContactName = GetNormalizedContactName(pszChatContactName);
				string strChatContactAlias(pszChatContactAlias);
				
				pCallback6->OnChatUpdateUser(strNormalizedUser, nChatId, strNormalizedChatContactName, strChatContactAlias, bIsContact);
			}
		}
	}
}


void CPurpleProtocol::OnChatReceived(PurpleAccount* pAccount, const int nChatId, const char* pszFromUser, const char* pszText, const char* pszSmileyList)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ( (pCallback != NULL) && (nChatId != -1) && (pszText != NULL) )
		{
			CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
			
			if (pCallback6)
            {
                string strFormatedMessage = FormatReceivedMessage(pszText);

                string strMessage("");
                GetLocaleTextFromUTF8(strFormatedMessage.c_str(), strMessage);

                string strSmileyList(pszSmileyList);

                string strNormalizedContact = GetNormalizedContactName(pszFromUser);

                pCallback6->OnChatReceived(strNormalizedUser, nChatId, strNormalizedContact, strMessage, strSmileyList);
            }
		}
	}
}


void CPurpleProtocol::OnChatSent(PurpleAccount* pAccount, const int nChatId, const char* pszToUser, const char* pszText)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if ( (pCallback != NULL) && (nChatId != -1) && (pszText != NULL) )
		{
			CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
			
			if (pCallback6)
            {
                string strFormatedMessage = FormatSentMessage(pszText);

                // Convert to locale encoding
                string strMessage("");
                GetLocaleTextFromUTF8(strFormatedMessage.c_str(), strMessage);

                string strNormalizedContact = GetNormalizedContactName(pszToUser);

                pCallback6->OnChatSent(strNormalizedUser, nChatId, strNormalizedContact, strMessage);
            }
		}
	}
}


int CPurpleProtocol::ProcessSystemMessage(PurpleAccount* /*pAccount*/, MessageParam* /*pParam*/)
{
	return FALSE;
}


int CPurpleProtocol::ProcessErrorMessage(PurpleAccount* pAccount, MessageParam* pParam)
{
	// conversation.c
	// "Unable to send message. The message is too large."
	// "Unable to send message."

	IMProtocolError protocolError = IM_PROTOCOL_ERROR_SUCCESS;

	if (0 == pParam->strMessage.compare("Unable to send message. The message is too large."))
	{
		protocolError = IM_PROTOCOL_ERROR_MESSAGE_TOO_LARGE;
	}
	else if (0 == pParam->strMessage.compare("Unable to send message."))
	{
		protocolError = IM_PROTOCOL_ERROR_UNKNOWN_SEND_ERROR;
	}

	CIMProtocolCallback* pCallback = NULL;
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		pCallback = GetCallback(strNormalizedUser.c_str());

		if ((protocolError != IM_PROTOCOL_ERROR_SUCCESS) &&
			(pCallback != NULL))
		{
			string strNormalizedContact = GetNormalizedContactName((pParam->strFrom).c_str());

			if (pParam->conversationType == PURPLE_CONV_TYPE_IM)
			{
				pCallback->OnIMSendError(strNormalizedUser, strNormalizedContact, protocolError);
			}
			else
			{
				CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
				
				if (pCallback6)
				{
					pCallback6->OnChatSendError(strNormalizedUser, pParam->iChatId, 
							strNormalizedContact, protocolError);
				}
			}

			// Notification was handled
			return TRUE;
		}
	}

	return FALSE;
}


int CPurpleProtocol::ProcessAccountRequestAuthorize(AccountRequestAuthorizeParam* pParam)
{
	if ((pParam->pPurpleAccount->username != NULL) && (pParam->strRemoteUser.length() > 0))
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pParam->pPurpleAccount->username);
		string strMessage;
		
		CPurpleRequestUserAuthorizationParam purpleRequestUserAuthorizationParam(
				pParam->strRemoteUser, pParam->strMessage);
		purpleRequestUserAuthorizationParam.SetContactAlreadyOnList(pParam->bOnList);
		
		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
		CIMProtocolCallback4* pCallback4 = NULL;
		
		if (pCallback != NULL)
		{
			pCallback4 = dynamic_cast<CIMProtocolCallback4*>(pCallback);
		}
		
		if (pCallback4)
		{
			RequestResult result = pCallback4->OnRequestAddContact(strNormalizedUser, 
					&purpleRequestUserAuthorizationParam);
			
			BOOL bIsAuthorizeCanceled = FALSE;
			
			g_mutex_lock(m_pAuthorizeCancelationMutex);
				
				bIsAuthorizeCanceled = pParam->bRequestIsCanceled;
				
			g_mutex_unlock(m_pAuthorizeCancelationMutex);
			
			if (!bIsAuthorizeCanceled)
			{
				if ( (result == REQUEST_RESULT_OK) && (pParam->pAuthorizeCallback != NULL) )
				{	
					pParam->pAuthorizeCallback(pParam->pUserData);
					
					if ( (!pParam->bOnList) && (purpleRequestUserAuthorizationParam.GetAddContactFlag()) )
					{
						IMProtocolError protocolError = IM_PROTOCOL_ERROR_FAILED;
						string strAddGroup = purpleRequestUserAuthorizationParam.GetContactGroup();
						
						string strDefaultGroup = GetDefaultContactGroup();
						
						if ((AutoAddContactOnAuthorization()) &&
							(strDefaultGroup.size() > 0))
						{
							protocolError = MoveContact(strNormalizedUser, pParam->strRemoteUser, 
									strDefaultGroup, strAddGroup);
						}
						
						if (protocolError != IM_PROTOCOL_ERROR_SUCCESS)
						{
							protocolError = AddContact(strNormalizedUser, pParam->strRemoteUser, strAddGroup);
						}
					}
				}
				else if ( (result == REQUEST_RESULT_CANCEL) && (pParam->pDenyCallback != NULL) )
				{
					pParam->pDenyCallback(pParam->pUserData);
				}
				
				return TRUE;
			}
		}
	}
	
	return FALSE;
}


void CPurpleProtocol::ProcessAccountRequestAuthorizeCancel(AccountRequestAuthorizeParam* pParam)
{
	g_mutex_lock(m_pAuthorizeCancelationMutex);
		
		pParam->bRequestIsCanceled = TRUE;
		
	g_mutex_unlock(m_pAuthorizeCancelationMutex);
}


int CPurpleProtocol::ProcessNotifyMessage(PurpleAccount* pAccount, NotifyMessageParam* pParam)
{
	string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

	CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());
	if (pCallback == NULL)
	{
		// Ignore notification
		return TRUE;
	}

	int nRet = FALSE;

	string strTemp;
	string strTitle;
	string strPrimary;
	string strSecondary;
	string::size_type npos = -1;

	if (pParam->type == PURPLE_NOTIFY_MSG_ERROR)
	{
		if (pParam->strPrimary.length() > 0)
		{
			GetLocaleTextFromUTF8(pParam->strPrimary.c_str(), strPrimary);
		}

		if (pParam->strSecondary.length() > 0)
		{
			GetLocaleTextFromUTF8(pParam->strSecondary.c_str(), strSecondary);
		}

		if (strSecondary.length() > 0)
		{
			if (0 == strSecondary.compare("The message is too large."))
			{
				// conversation.c:
				// pri = "Unable to send message to %s:"
				// sec = "The message is too large."

				// Get recipient
				if ((pParam->strPrimary.length() > 0) &&
					(FALSE != ExtractSubstring(pParam->strPrimary.c_str(), "message to ", ":", strTemp)))
				{
					strTemp = GetNormalizedContactName(strTemp.c_str());

					pCallback->OnIMSendError(strNormalizedUser, strTemp, IM_PROTOCOL_ERROR_MESSAGE_TOO_LARGE);
					nRet = TRUE;
				}
			}
		}
		else if (strPrimary.length() > 0)
		{
			if ((npos != strPrimary.find("Unable to send message", 0)) &&
				(FALSE != ExtractSubstring(pParam->strPrimary.c_str(), "message to ", ":", strTemp)))
			{
				// conversation.c:
				// pri = "Unable to send message to %s:"

				strTemp = GetNormalizedContactName(strTemp.c_str());

				pCallback->OnIMSendError(strNormalizedUser, strTemp, IM_PROTOCOL_ERROR_UNKNOWN_SEND_ERROR);
				nRet = TRUE;
			}			
		}

		// ft.c
		// pri = "Cannot send a file of 0 bytes."
		// pri = "Cannot send a directory."
		// pri = "%s is not a regular file. Cowardly refusing to overwrite it.\n"
		// pri = "File transfer to %s aborted.\n",
		//		 "File transfer from %s aborted.\n"
		// sec = "Error reading %s: \n%s.\n",
		//		 "Error writing %s: \n%s.\n",
		//		 "Error accessing %s: \n%s.\n",
		//		 "%s is not a valid filename.\n",
		//		 "%s canceled the transfer of %s",
		//		 "%s canceled the file transfer",
		
		// plugin.c
		// pri = "Purple was unable to load your plugin."

		// proxy.c
		// pri = "Invalid proxy settings"
	}
	else if (pParam->type == PURPLE_NOTIFY_MSG_WARNING)
	{
		// server.c
		// pri = "%s has just been warned by %s.\nYour new warning level is %d%%"
	}
	else if (pParam->type == PURPLE_NOTIFY_MSG_INFO)	
	{
	}

	return nRet;
}


int CPurpleProtocol::ProcessNotifyFormatted(PurpleAccount* pAccount, NotifyFormattedParam* pParam)
{
	return FALSE;
}


int CPurpleProtocol::ProcessNotifyUserInfo(PurpleAccount* pAccount, NotifyUserInfoParam* pParam)
{
	if (pAccount->username != NULL)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if (pCallback != NULL)
		{
			string strInfo("");
			GetLocaleTextFromUTF8(pParam->strUserInfo.c_str(), strInfo);
			
			string strNormalizedContact = GetNormalizedContactName((pParam->strWho).c_str());
			
			pCallback->OnContactGotInfo(strNormalizedUser, strNormalizedContact, strInfo);
		}
	}

	return TRUE;
}


int CPurpleProtocol::ProcessRequestInput(PurpleAccount* pAccount, RequestInputParam* pParam)
{
	// connection.c
	// "Enter password for %s (%s)"
	if ((pAccount->username != NULL) && (pParam->strPrimary.length() > 0))
	{
		string::size_type npos = -1;

		if (npos != pParam->strPrimary.find("Enter password for ", 0))
		{
			string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

			CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

			if (pCallback != NULL)
			{
				const int MAX_BUFFER_LENGTH = 256;
				char* pszBuffer = g_new0(char, MAX_BUFFER_LENGTH + 1);
			
				if (pszBuffer != NULL)
				{
					RequestResult result = pCallback->OnRequestInput(strNormalizedUser, REQUEST_REASON_ENTER_PASSWORD, pszBuffer, MAX_BUFFER_LENGTH);
					pszBuffer[MAX_BUFFER_LENGTH] = '\0';

					char* pszUTF8Pass = purple_utf8_try_convert(pszBuffer);

					if (pszUTF8Pass)
					{
						if ((result == REQUEST_RESULT_OK) && (pParam->okCallback != NULL))
						{	
							((PurpleRequestInputCb) (pParam->okCallback)) (pParam->pUserData, pszUTF8Pass);
						}
						else if (pParam->cancelCallback != NULL)
						{
							((PurpleRequestInputCb) (pParam->cancelCallback))(pParam->pUserData, pszUTF8Pass);
						}

						g_free(pszUTF8Pass);
					}

					g_free(pszBuffer);
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}


int CPurpleProtocol::ProcessRequestAction(PurpleAccount* /*pAccount*/, RequestActionParam* /*pParam*/)
{
	return FALSE;
}

int CPurpleProtocol::ProcessRequestFileTransferAction(PurpleXfer* pXfer, RequestActionParam* pParam)
{
	int nRet = TRUE;

	CIMProtocolCallback* pCallback = NULL;
	string strNormalizedUser;

	if ((pXfer->account != NULL) && (pXfer->account->username != NULL))
	{
		strNormalizedUser = GetNormalizedCallbackUserName(pXfer->account->username);

		pCallback = GetCallback(strNormalizedUser.c_str());
	}

	if ((pCallback != NULL) && 
		(pXfer->who != NULL) && (pXfer->filename != NULL) &&
		(pParam->vActionCallback.size() == 2))
	{
		GCallback cbOK;
		GCallback cbCancel;

		cbOK = pParam->vActionCallback[0];
		cbCancel = pParam->vActionCallback[1];

		purple_xfer_ref(pXfer);

		AddPurpleXfer(pXfer);
		
		string strFileName("");		
		GetLocaleTextFromUTF8(pXfer->filename, strFileName);

		string strNormalizedContact = GetNormalizedContactName(pXfer->who);

		CIMProtocolCallback6* pCallback6 = dynamic_cast<CIMProtocolCallback6*>(pCallback);
		
		CPurpleRequestFileTransferAsyncInfo* pFileTransferAsyncInfo = 
				new CPurpleRequestFileTransferAsyncInfo();
		
		if ( (pCallback6) && (pFileTransferAsyncInfo) )
		{
			pFileTransferAsyncInfo->SetPurpleXFer(pXfer);
			pFileTransferAsyncInfo->SetOkCallback(cbOK);
			pFileTransferAsyncInfo->SetCancelCallback(cbCancel);
			pFileTransferAsyncInfo->SetUserData(pParam->pUserData);
			pFileTransferAsyncInfo->SetSender(strNormalizedContact);
			pFileTransferAsyncInfo->SetFileName(strFileName);
			
			RequestResult result = pCallback6->OnRequestAcceptFileAsync(strNormalizedUser, strNormalizedContact, pFileTransferAsyncInfo);
		}
		
		/*
		RequestResult result = pCallback->OnRequestAcceptFile(strNormalizedUser, strNormalizedContact, strFileName.c_str());

		PurpleRequestActionCb pfnRequestActionCallback;

		if (result == REQUEST_RESULT_OK)
		{
			if ((cbOK != NULL) && (pXfer->status != PURPLE_XFER_STATUS_CANCEL_REMOTE))
			{
				pfnRequestActionCallback = (PurpleRequestActionCb) cbOK;
				pfnRequestActionCallback(pParam->pUserData, 0);
			}
		}
		else if ((cbCancel != NULL) && (pXfer->status != PURPLE_XFER_STATUS_CANCEL_REMOTE))
		{
			pfnRequestActionCallback = (PurpleRequestActionCb) cbCancel;
			pfnRequestActionCallback(pParam->pUserData, 1);
		}
		*/

		purple_xfer_unref(pXfer);
	}

	return nRet;
}

int CPurpleProtocol::ProcessRequestFile(PurpleXfer* pXfer, RequestFileParam* pParam)
{
	int nRet = FALSE;

	PurpleAccount* pAccount = pXfer->account;

	if ((pAccount != NULL) &&
		(pAccount->username != NULL) &&
		(pXfer->filename != NULL) &&
		(pXfer->who != NULL))
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

		if (pCallback != NULL)
		{
			purple_xfer_ref(pXfer);

			nRet = ProcessRequestRecvFileCallback(pCallback, &CIMProtocolCallback::OnRequestFilePath, 
					pXfer, pParam->okCallback, pParam->cancelCallback, pParam->pUserData);

			purple_xfer_unref(pXfer);
		}
	}

	return nRet;
}


CIMProtocolCallback* CPurpleProtocol::GetCallback(const char* pszUser)
{
	CIMProtocolCallback* pCallback = NULL;

	if (pszUser != NULL)
	{
		string strUser = pszUser;
		CCallbackMap::const_iterator it = m_mapCallback.find(strUser);
		
		if (it != m_mapCallback.end())
		{
			pCallback = it->second;
		}
	}

	return pCallback;
}



//
// Protected interface
//
bool CPurpleProtocol::GetStatusText(int nStatus, std::string& statusText)
{
	return false;
}


bool CPurpleProtocol::GetStatusCode(const std::string& statusText, int& nStatus)
{
	return false;
}


PurpleAccount* CPurpleProtocol::GetPurpleAccount(const char* pszUser)
{
	PurpleAccount* pAccount = NULL;
	char* pszUTF8User = purple_utf8_try_convert(pszUser);

	if (pszUTF8User != NULL)
	{
		pAccount = purple_accounts_find(pszUTF8User, m_strId.c_str());

		g_free(pszUTF8User);
	}

	return pAccount;
}

PurpleAccount* CPurpleProtocol::GetPurpleAccountConnected(const char* pszUser)
{
	PurpleAccount* pAccount = GetPurpleAccount(pszUser);

	// Check if account is connected
	if ((pAccount != NULL) &&
		(FALSE == purple_account_is_connected(pAccount)))
	{
		pAccount = NULL;
	}

	return pAccount;
}

int CPurpleProtocol::IsConnected(PurpleAccount* pAccount)
{
	return ((pAccount->gc != NULL) && (pAccount->gc->state == PURPLE_CONNECTED));
}

int CPurpleProtocol::ProcessRequestActionCallback(CIMProtocolCallback* pCallback, RequestResult (CIMProtocolCallback::*pfnCallback)(const std::string&, const string&), const string& strUser, const string& strContact, GCallback cbOK, GCallback cbCancel, void* pUserData, int nDefaultAction)
{
	PurpleRequestActionCb pfnRequestActionCallback;

	string strNormalizedUser = GetNormalizedCallbackUserName(strUser.c_str());
	string strNormalizedContact = GetNormalizedContactName(strContact.c_str());

	RequestResult result = (pCallback->*pfnCallback)(strNormalizedUser, strNormalizedContact);

	if (result == REQUEST_RESULT_NONE)
	{
		if (0 == nDefaultAction)
		{
			result = REQUEST_RESULT_OK;
		}
		else if (1 == nDefaultAction)
		{
			result = REQUEST_RESULT_CANCEL;
		}
	}

	if (result == REQUEST_RESULT_OK)
	{
		if (cbOK != NULL)
		{
			pfnRequestActionCallback = (PurpleRequestActionCb) cbOK;
			pfnRequestActionCallback(pUserData, 0);
		}
	}
	else  if (result == REQUEST_RESULT_CANCEL)
	{
		if (cbCancel != NULL)
		{
			pfnRequestActionCallback = (PurpleRequestActionCb) cbCancel;
			pfnRequestActionCallback(pUserData, 1);
		}
	}

	return TRUE;
}

int CPurpleProtocol::ProcessRequestActionCallback(CIMProtocolCallback* pCallback, RequestResult (CIMProtocolCallback::*pfnCallback)(const std::string&, const string&, const string&), const string& strUser, const string& strContact, const string& strReason, GCallback cbOK, GCallback cbCancel, void* pUserData, int nDefaultAction)
{
	PurpleRequestActionCb pfnRequestActionCallback;

	string strNormalizedUser = GetNormalizedCallbackUserName(strUser.c_str());
	string strNormalizedContact = GetNormalizedContactName(strContact.c_str());

	RequestResult result = (pCallback->*pfnCallback)(strNormalizedUser, strNormalizedContact, strReason);

	if (result == REQUEST_RESULT_NONE)
	{
		if (0 == nDefaultAction)
		{
			result = REQUEST_RESULT_OK;
		}
		else if (1 == nDefaultAction)
		{
			result = REQUEST_RESULT_CANCEL;
		}
	}

	if (result == REQUEST_RESULT_OK)
	{
		if (cbOK != NULL)
		{
			pfnRequestActionCallback = (PurpleRequestActionCb) cbOK;
			pfnRequestActionCallback(pUserData, 0);
		}
	}
	else if (result == REQUEST_RESULT_CANCEL)
	{
		if (cbCancel != NULL)
		{
			pfnRequestActionCallback = (PurpleRequestActionCb) cbCancel;
			pfnRequestActionCallback(pUserData, 1);
		}
	}

	return TRUE;
}

int CPurpleProtocol::ProcessRequestInputCallback(CIMProtocolCallback* pCallback, RequestResult (CIMProtocolCallback::*pfnCallback)(const std::string&, const std::string&), const std::string& strUser, const std::string& strContact, /*const std::string& strReason,*/ GCallback cbOK, GCallback cbCancel, void* pUserData, int nDefaultAction)
{
	PurpleRequestInputCb pfnRequestInputCallback;

	string strNormalizedUser = GetNormalizedCallbackUserName(strUser.c_str());
	string strNormalizedContact = GetNormalizedContactName(strContact.c_str());

	RequestResult result = (pCallback->*pfnCallback)(strNormalizedUser, strNormalizedContact);

	if (result == REQUEST_RESULT_NONE)
	{
		if (0 == nDefaultAction)
		{
			result = REQUEST_RESULT_OK;
		}
		else if (1 == nDefaultAction)
		{
			result = REQUEST_RESULT_CANCEL;
		}
	}

	if (result == REQUEST_RESULT_OK)
	{
		if (cbOK != NULL)
		{
			pfnRequestInputCallback = (PurpleRequestInputCb) cbOK;
			pfnRequestInputCallback(pUserData, NULL);
		}
	}
	else if (result == REQUEST_RESULT_CANCEL)
	{
		if (cbCancel != NULL)
		{
			pfnRequestInputCallback = (PurpleRequestInputCb) cbCancel;
			pfnRequestInputCallback(pUserData, NULL);
		}
	}

	return TRUE;
}

int CPurpleProtocol::ProcessRequestRecvFileCallback(CIMProtocolCallback* pCallback, RequestResult (CIMProtocolCallback::*pfnCallback)(const string&, const string&, char*, int), PurpleXfer* pXfer, GCallback cbOK, GCallback cbCancel, void* pUserData)
{
	PurpleRequestFileCb pfnReqRecvFileCallback;

	const int MAX_BUFFER_LENGTH = 256;
	char* pszBuffer = g_new0(char, MAX_BUFFER_LENGTH + 1);
			
	if ((pszBuffer != NULL) &&
		(pXfer->filename != NULL) && (pXfer->who != NULL))
	{
		string strLocaleFileName("");
		if (GetLocaleTextFromUTF8(pXfer->filename, strLocaleFileName))
		{
			strncpy(pszBuffer, strLocaleFileName.c_str(), MAX_BUFFER_LENGTH);
		}
		else
		{
			strncpy(pszBuffer, pXfer->filename, MAX_BUFFER_LENGTH);
		}

		pszBuffer[MAX_BUFFER_LENGTH] = '\0';

		string strNormalizedUser = GetNormalizedCallbackUserName(pXfer->account->username);
		string strNormalizedContact = GetNormalizedContactName(pXfer->who);

		RequestResult result = (pCallback->*pfnCallback)(strNormalizedUser, strNormalizedContact, pszBuffer, MAX_BUFFER_LENGTH);

		pszBuffer[MAX_BUFFER_LENGTH] = '\0';

		gchar* pszUTF8FileName = purple_utf8_try_convert(pszBuffer);

		if (pszUTF8FileName)
		{
			if (result == REQUEST_RESULT_OK)
			{
				if ((cbOK != NULL) &&
					(pXfer->status != PURPLE_XFER_STATUS_CANCEL_REMOTE))
				{
					pfnReqRecvFileCallback = (PurpleRequestFileCb) cbOK;
					pfnReqRecvFileCallback(pUserData, pszUTF8FileName);
				}
			}
			else if ((cbCancel != NULL) &&
				(pXfer->status != PURPLE_XFER_STATUS_CANCEL_REMOTE))
			{
				pfnReqRecvFileCallback = (PurpleRequestFileCb) cbCancel;
				pfnReqRecvFileCallback(pUserData, pszUTF8FileName);
			}

			g_free(pszUTF8FileName);
		}

		g_free(pszBuffer);
	}

	return TRUE;
}


int CPurpleProtocol::ProcessRequestUserAuthorizationCallback(CIMProtocolCallback3* pCallback3, const std::string& strUser, const std::string& strContact, const std::string& strReason, GCallback cbOK, GCallback cbCancel, void* pUserData, int nDefaultAction)
{
	PurpleRequestInputCb pfnRequestInputCallback;

	string strNormalizedUser = GetNormalizedCallbackUserName(strUser.c_str());
	string strNormalizedContact = GetNormalizedContactName(strContact.c_str());
	string strMessage;

	GetLocaleTextFromUTF8(strReason.c_str(), strMessage);

	CPurpleRequestUserAuthorizationParam purpleRequestUserAuthorizationParam(strNormalizedContact, strMessage);

	RequestResult result = pCallback3->OnRequestUserAuthorization(strNormalizedUser, &purpleRequestUserAuthorizationParam);

	if (result == REQUEST_RESULT_NONE)
	{
		if (0 == nDefaultAction)
		{
			result = REQUEST_RESULT_OK;
		}
		else if (1 == nDefaultAction)
		{
			result = REQUEST_RESULT_CANCEL;
		}
	}

	if (result == REQUEST_RESULT_OK)
	{
		if (cbOK != NULL)
		{
			pfnRequestInputCallback = (PurpleRequestInputCb) cbOK;
			pfnRequestInputCallback(pUserData, NULL);

			// Add authorized user
			if (purpleRequestUserAuthorizationParam.GetAddContactFlag())
			{
				IMProtocolError protocolError = IM_PROTOCOL_ERROR_FAILED;
				string strAddGroup = purpleRequestUserAuthorizationParam.GetContactGroup();

				string strDefaultGroup = GetDefaultContactGroup();

				if ((AutoAddContactOnAuthorization()) &&
					(strDefaultGroup.size() > 0))
				{
					protocolError = MoveContact(strUser, strContact, strDefaultGroup, strAddGroup);
				}

				if (protocolError != IM_PROTOCOL_ERROR_SUCCESS)
				{
					protocolError = AddContact(strUser, strContact, strAddGroup);
				}
			}
		}
	}
	else if (result == REQUEST_RESULT_CANCEL)
	{
		if (cbCancel != NULL)
		{
			pfnRequestInputCallback = (PurpleRequestInputCb) cbCancel;
			pfnRequestInputCallback(pUserData, NULL);
		}
	}

	return TRUE;
}


PurpleProxyInfo* CPurpleProtocol::ProcessProxySettings(CProxyInfo* pProxyInfo)
{
	PurpleProxyInfo* pProxy = NULL;

	if (pProxyInfo != NULL)
	{
		pProxy = purple_proxy_info_new();

		if (pProxy != NULL)
		{
			CProxyInfo::ProxyType ptType = pProxyInfo->GetType();

			PurpleProxyType purpleProxyType = PURPLE_PROXY_NONE;

			if (ptType == CProxyInfo::PT_HTTP)
			{
				purpleProxyType = PURPLE_PROXY_HTTP;
			}
			else if (ptType == CProxyInfo::PT_SOCKS4)
			{
				purpleProxyType = PURPLE_PROXY_SOCKS4;
			}
			else if (ptType == CProxyInfo::PT_SOCKS5)
			{
				purpleProxyType = PURPLE_PROXY_SOCKS5;
			}

			purple_proxy_info_set_type(pProxy, purpleProxyType);
			purple_proxy_info_set_host(pProxy, pProxyInfo->GetHost().c_str());
			purple_proxy_info_set_port(pProxy, pProxyInfo->GetPort());
			purple_proxy_info_set_username(pProxy, pProxyInfo->GetUsername().c_str());
			purple_proxy_info_set_password(pProxy, pProxyInfo->GetPassword().c_str());
		}
	}

	return pProxy;
}


void CPurpleProtocol::ProcessExtraSettings(PurpleAccount* pAccount, CProtocolSettings* pExtraSettings)
{
	// Override in derived classes
}


void CPurpleProtocol::ProcessExtendedCapabilities(PurpleAccount* pAccount, CProtocolSettings2* pExtendedCapabilities)
{
	// Override in derived classes
}


IMProtocolError CPurpleProtocol::UpdateServerExtendedCapabilities(PurpleAccount* pAccount, BOOL bPublishing)
{
	// Override in derived classes
	return IM_PROTOCOL_ERROR_SUCCESS;
}


string CPurpleProtocol::FormatSendingMessage(const string& strMessage)
{
	return strMessage;
}


string CPurpleProtocol::FormatSentMessage(const string& strMessage)
{
	return strMessage;
}


string CPurpleProtocol::FormatReceivedMessage(const string& strMessage)
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


std::string CPurpleProtocol::GetNormalizedPurpleUserName(const std::string& strUserName)
{
	return strUserName;
}


std::string CPurpleProtocol::GetNormalizedCallbackUserName(const char* pszPurpleUserName)
{
	return string(pszPurpleUserName);
}


string CPurpleProtocol::GetNormalizedContactName(const char* pszContact)
{
	return string(pszContact);
}


long CPurpleProtocol::AddPurpleXfer(PurpleXfer* pXfer)
{
	long lFileTransferId = 0;

	if (pXfer->ui_data == 0)
	{
		lFileTransferId = ++m_lNextFileTransferId;

		pXfer->ui_data = (void*) lFileTransferId;

		m_mapFileTransfer[lFileTransferId] = pXfer;
	}

	return lFileTransferId;
}


void CPurpleProtocol::RemovePurpleXfer(long lTransferId)
{
	CFileTransferMap::iterator it = m_mapFileTransfer.find(lTransferId);
	
	if (it != m_mapFileTransfer.end())
	{
		m_mapFileTransfer.erase(it);
	}
}


PurpleXfer* CPurpleProtocol::GetPurpleXfer(long lTransferId)
{
	CFileTransferMap::const_iterator it = m_mapFileTransfer.find(lTransferId);

	if (it != m_mapFileTransfer.end())
	{
		return it->second;
	}

	return NULL;
}


PurpleXfer* CPurpleProtocol::GetPurpleXfer(PurpleAccount* pAccount, const std::string& strContact)
{
	PurpleXfer* pXfer = NULL;	

	CFileTransferMap::const_iterator it;

	for (it = m_mapFileTransfer.begin(); it != m_mapFileTransfer.end(); it++)
	{
		pXfer = it->second;
		if ((pXfer != NULL) && (pAccount == pXfer->account) && (0 == strContact.compare(pXfer->who)))
		{
			break;
		}
	}

	return pXfer;
}


// This method MUST be called from serialized methods.
//
bool CPurpleProtocol::ExecuteBListNodeAction(PurpleAccount* pAccount, PurpleBlistNode* pNode, const std::string& strAction)
{
	LPPurpleMenuActionCallback pCallbackFunction;
	
	if ((!pAccount) || (!pNode) || (0 == strAction.length()))
	{
		return false;
	}

	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	bool bExecuted = false;

	if (pAccount->gc)
	{
		PurplePluginProtocolInfo *prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(pAccount->gc->prpl);

		GList *l, *ll;

		if (prpl_info && (prpl_info->blist_node_menu))
		{
			for(l = ll = prpl_info->blist_node_menu(pNode); l; l = l->next)
			{
				PurpleMenuAction *pMenuAction = (PurpleMenuAction *) l->data;
				pCallbackFunction = PURPLE_MENU_ACTION_CALLBACK(pMenuAction->callback);

				if ((pMenuAction->label) && 
					(0 == strAction.compare(pMenuAction->label)) &&
					(pCallbackFunction))
				{
					//purple_timeout_add(0, (GSourceFunc)SerializerCallbackMethod, 
							//new ExecuteBlistNodeActionSerializerHelper((CPurpleProtocolSerializer*)this, pCallbackFunction, pNode, pMenuAction->data));
					
					pCallbackFunction(pNode, pMenuAction->data);
					bExecuted = true;
					break;
				}
			}

			g_list_free(ll);
		}
	}

	return bExecuted;
}


bool CPurpleProtocol::GetConnectionErrorCode(ConnectionError& errorCode, const char* pszReason)
{
	bool bFind = false;

	if (pszReason != NULL)
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

void CPurpleProtocol::AddProtocolToStack(const char* pszUser, CIMProtocolCallback * pProtocolCallback)
{	
	/*CIMProtocolCallback* pPurpleProtocolCallbackStack = GetCallback(pszUser);

	if (pPurpleProtocolCallbackStack)
	{
		pPurpleProtocolCallbackStack->AddProtocol(pProtocolCallback);
	}*/
}

PurpleBuddy* CPurpleProtocol::GetPurpleBuddy(PurpleAccount* pAccount, const char* pszContact)
{
	PurpleBuddy* pBuddy = NULL;
	char* pszUTF8User = purple_utf8_try_convert(pAccount->username);
	char* pszUTF8Contact = purple_utf8_try_convert(pszContact);
	
	if ( (pszUTF8User != NULL) && (pszContact != NULL) )
	{
		pBuddy = purple_find_buddy(pAccount, pszContact);
	}
	
	g_free(pszUTF8User);
	g_free(pszUTF8Contact);
	
	return pBuddy;
}

PurpleBuddy* CPurpleProtocol::GetPurpleBuddyConnected(PurpleAccount* pAccount, const char* pszContact)
{
	PurpleBuddy* pBuddy = GetPurpleBuddy(pAccount, pszContact);
	
	// Check if buddy is on-line.
	if ((pBuddy != NULL) &&
			(FALSE == PURPLE_BUDDY_IS_ONLINE(pBuddy)))
	{
		pBuddy = NULL;
	}
	
	return pBuddy;
}

void CPurpleProtocol::WaitLibPurpleThreadAccess()
{
	DWORD dwWaitReason;
	DWORD dwUIMThreadId = CPurpleService::GetInstance()->GetUIMThreadId();
	DWORD dwAsyncOperationsThreadId = CPurpleService::GetInstance()->GetAsyncOperationsThreadId();
	DWORD dwCurrentThreadId = ::GetCurrentThreadId();
	
	ATLTRACE("CPurpleProtocol::%s -> dwUIMThreadId = [%u], dwCurrentThreadId = [%u], dwAsyncOperationsThreadId = [%u]\n", 
			__FUNCTION__, dwUIMThreadId, dwCurrentThreadId, dwAsyncOperationsThreadId);
	
	if ( (m_bEventsInitialized) && 
			( (dwCurrentThreadId == dwUIMThreadId) || (dwCurrentThreadId == dwAsyncOperationsThreadId) ) )
	{
		ATLTRACE("CPurpleProtocol::%s -> Block multiple threaded UIM method invocations. \n", __FUNCTION__);
		
		g_mutex_lock(m_pUIMMultipleThreadSerializeMutex);
		
		ATLTRACE("CPurpleProtocol::%s -> Reset m_libPurpleHoldEvent \n", __FUNCTION__);
		
		// Reset this to wait for libpurple's thread let us access 
		// their methods on a serialized fashion.
		//
		m_libPurpleHoldEvent.Reset();
		
		ATLTRACE("CPurpleProtocol::%s -> Add LibPurpleHolderMethod \n", __FUNCTION__);
		
		guint iEventSource = purple_timeout_add(0, (GSourceFunc)LibPurpleHolderMethod, NULL);
		
		ATLTRACE("CPurpleProtocol::%s -> iEventSource = [%d] \n", __FUNCTION__, iEventSource);
		
		ATLTRACE("CPurpleProtocol::%s -> Wait m_libPurpleHoldEvent \n", __FUNCTION__);
		
		dwWaitReason = ::WaitForSingleObject(m_libPurpleHoldEvent, INFINITE);
		
		if (dwWaitReason != WAIT_OBJECT_0)
		{
			//TODO: log some information message.
		}
	}
	
	ATLTRACE("CPurpleProtocol::%s -> Leaving ... \n", __FUNCTION__);
}

void CPurpleProtocol::ReleaseLibPurpleThreadAccess()
{
	DWORD dwUIMThreadId = CPurpleService::GetInstance()->GetUIMThreadId();
	DWORD dwAsyncOperationsThreadId = CPurpleService::GetInstance()->GetAsyncOperationsThreadId();
	DWORD dwCurrentThreadId = ::GetCurrentThreadId();
	
	ATLTRACE("CPurpleProtocol::%s -> dwUIMThreadId = [%u], dwCurrentThreadId = [%u], dwAsyncOperationsThreadId = [%u]\n", 
			__FUNCTION__, dwUIMThreadId, dwCurrentThreadId, dwAsyncOperationsThreadId);
	
	if ( (m_bEventsInitialized) && 
			( (dwCurrentThreadId == dwUIMThreadId) || (dwCurrentThreadId == dwAsyncOperationsThreadId) ) )
	{
		ATLTRACE("CPurpleProtocol::%s -> Set m_libPurpleReleaseEvent \n", __FUNCTION__);
		
		m_libPurpleReleaseEvent.Set();
		
		ATLTRACE("CPurpleProtocol::%s -> Unblock multiple threaded UIM method invocations. \n", __FUNCTION__);
		
		g_mutex_unlock(m_pUIMMultipleThreadSerializeMutex);
	}
	
	ATLTRACE("CPurpleProtocol::%s -> Leaving ... \n", __FUNCTION__);
}

gboolean CPurpleProtocol::LibPurpleHolderMethod(gpointer data)
{
	DWORD dwWaitReason;
	
	ATLTRACE("CPurpleProtocol::%s\n", __FUNCTION__);
	
	if (m_bEventsInitialized)
	{
		ATLTRACE("CPurpleProtocol::%s -> Reset m_libPurpleReleaseEvent \n", __FUNCTION__);
		
		// Reset this to wait for UOLMessenger's thread finish to access 
		// libpurple's methods on a serialized fashion and release 
		// execution to next serialized method.
		//
		m_libPurpleReleaseEvent.Reset();
		
		ATLTRACE("CPurpleProtocol::%s -> Set m_libPurpleHoldEvent \n", __FUNCTION__);
		
		// Release UOLMessenger thread execution and access to libpurple's 
		// methods now that we are serialized.
		//
		m_libPurpleHoldEvent.Set();
		
		ATLTRACE("CPurpleProtocol::%s -> Wait m_libPurpleReleaseEvent \n", __FUNCTION__);
		
		dwWaitReason = ::WaitForSingleObject(m_libPurpleReleaseEvent, INFINITE);
		
		if (dwWaitReason != WAIT_OBJECT_0)
		{
			//TODO: log some information message.
		}
	}
	
	ATLTRACE("CPurpleProtocol::%s -> Leaving ... \n", __FUNCTION__);
	
	// Must return false, so the resource is removed.
	//
	return FALSE;
}

gboolean CPurpleProtocol::SerializedDisconnectMethod(gpointer data)
{
	PurpleAccount* pAccount = (PurpleAccount*)data;
	
	DWORD dwUIMThreadId = CPurpleService::GetInstance()->GetUIMThreadId();
	DWORD dwCurrentThreadId = ::GetCurrentThreadId();
	
	ATLTRACE("CPurpleProtocol::%s -> dwUIMThreadId = [%u], dwCurrentThreadId = [%u], pAccount->username = [%s]\n", 
			__FUNCTION__, dwUIMThreadId, dwCurrentThreadId, pAccount->username);
	
	// Check if account is connected
	if (purple_account_get_enabled(pAccount, purple_core_get_ui()))
	{
		purple_account_set_enabled(pAccount, purple_core_get_ui(), FALSE);
	}
	
	purple_account_disconnect(pAccount);
	
	ATLTRACE("CPurpleProtocol::%s -> Leaving ... \n", __FUNCTION__);
	
	// Must return false, so the resource is removed.
	//
	return FALSE;
}

