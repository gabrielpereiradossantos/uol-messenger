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
 
#include "purplejabberuolprotocol.h"
#include "jabberconstants.h"

#include <string>
#include <glib-object.h>

#include <util.h>
#include <protocols/uol/uol_extension.h>

#include "purpleutils.h"
#include "critseclock.h"
#include "purpleservice.h"

#define PRESENCE_CAPABILITIES_NODE "c"
#define PRESENCE_XMLNS_ATTRIB "xmlns"
#define PRESENCE_NODE_ATTRIB "node"
#define PRESENCE_VER_ATTRIB "ver"
#define PRESENCE_EXT_ATTRIB "ext"

#define PRESENCE_XMLNS_VALUE "http://jabber.org/protocol/caps"
#define PRESENCE_NODE_VALUE "http://xmpp.uol.com.br/caps"
#define PRESENCE_VER_VALUE "0.9"



using namespace std;


const char* CPurpleJabberUOLProtocol::PURPLE_JABBER_UOL_PLUGIN_ID = "prpl-uol";
const char* CPurpleJabberUOLProtocol::PURPLE_JABBER_UOL_PROTOCOL_NAME = "UOL";
CPurpleJabberUOLProtocol* CPurpleJabberUOLProtocol::ms_callbackHandler = NULL;
CAutoCriticalSectionWrapper CPurpleJabberUOLProtocol::ms_csCallback;
const char* CPurpleJabberUOLProtocol::PURPLE_JABBER_UOL_DEFAULT_RESOURCE = "UOLM";
char CPurpleJabberUOLProtocol::PURPLE_JABBER_UOL_RESOURCE_SPLITTER = '/';


CPurpleJabberUOLProtocol::JabberStatus CPurpleJabberUOLProtocol::ms_status[] = 
{
	{ JABBER_STATUS_ONLINE, "available" },
	{ JABBER_STATUS_CHATTY, "freeforchat" },
	{ JABBER_STATUS_AWAY, "away" },
	{ JABBER_STATUS_EXTENDED_AWAY, "extended_away" },
	{ JABBER_STATUS_DO_NOT_DISTURB, "dnd" },
	{ JABBER_STATUS_UNAVAILABLE, "offline" },
	{ JABBER_STATUS_ERROR, "error" }
};


CPurpleJabberUOLProtocol::CPurpleJabberUOLProtocol() :
	CPurpleJabberProtocol(PURPLE_JABBER_UOL_PLUGIN_ID, PURPLE_JABBER_UOL_PROTOCOL_NAME)
{
}

CPurpleJabberUOLProtocol::CPurpleJabberUOLProtocol(const std::string& strProtocolId, const std::string& strProtocolName) :
	CPurpleJabberProtocol(strProtocolId, strProtocolName)
{
}


CPurpleJabberUOLProtocol::~CPurpleJabberUOLProtocol()
{
}


bool CPurpleJabberUOLProtocol::HasUserDisplayIconSupport()
{
	return false;
}


bool CPurpleJabberUOLProtocol::IsValidUserName(const string& strUser)
{
	bool bValid = CPurpleJabberProtocol::IsValidUserName(strUser);

	if (bValid)
	{
		bValid = ((strUser.find("@bol.com.br") != -1) || (strUser.find("@uol.com.br") != -1));
	}

	return bValid;
}


void CPurpleJabberUOLProtocol::Init()
{
	CPurpleJabberProtocol::Init();

	CPurpleJabberUOLProtocol::SetExtensionsCallback(this);
}


void CPurpleJabberUOLProtocol::Finalize()
{
	CPurpleJabberUOLProtocol::SetExtensionsCallback(NULL);

	RemoveAllExtendedCapabilities();

	CPurpleJabberProtocol::Finalize();
}


IMProtocolError CPurpleJabberUOLProtocol::Disconnect(const string& strUser)
{
	string strNormalizedUser = GetNormalizedCallbackUserName(strUser.c_str());

	RemoveExtendedCapabilites(strNormalizedUser.c_str());

	IMProtocolError error = CPurpleJabberProtocol::Disconnect(strUser);

	return error;
}


IMProtocolError CPurpleJabberUOLProtocol::SetUserStatus(const string& strUser, int nStatus)
{
	IMProtocolError error = CPurpleProtocol::SetUserStatus(strUser, nStatus);

	if (error == IM_PROTOCOL_ERROR_SUCCESS)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(strUser.c_str());

		SaveUserStatus(strNormalizedUser.c_str(), nStatus, _T(""));
	}

	return error;
}


IMProtocolError CPurpleJabberUOLProtocol::SetUserStatus(const std::string& strUser, int nStatus, const std::string& strStatusMessage)
{
	IMProtocolError error = CPurpleProtocol::SetUserStatus(strUser, nStatus, strStatusMessage);

	if (error == IM_PROTOCOL_ERROR_SUCCESS)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(strUser.c_str());

		SaveUserStatus(strNormalizedUser.c_str(), nStatus, strStatusMessage.c_str());
	}

	return error;
}


void CPurpleJabberUOLProtocol::OnReportDisconnect(PurpleAccount* pAccount, const char* pszText)
{
	if (pAccount && pAccount->username)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		RemoveExtendedCapabilites(strNormalizedUser.c_str());
	}

	CPurpleJabberProtocol::OnReportDisconnect(pAccount, pszText);
}


bool CPurpleJabberUOLProtocol::GetConnectionErrorCode(ConnectionError& errorCode, const char* pszReason)
{
	bool bFind = CPurpleJabberProtocol::GetConnectionErrorCode(errorCode, pszReason);

	if (!bFind)
	{
		errorCode = CONNECTION_ERROR_UOL_SERVER_ERROR;
	}

	return bFind;
}


void CPurpleJabberUOLProtocol::ProcessExtendedCapabilities(PurpleAccount* pAccount, CProtocolSettings2* pExtendedCapabilities)
{
	if (pAccount && pAccount->username)
	{
		string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

		SetUserExtendedCapabilities(strNormalizedUser.c_str(), pExtendedCapabilities);
	}
}


IMProtocolError CPurpleJabberUOLProtocol::UpdateServerExtendedCapabilities(PurpleAccount* pAccount, BOOL bPublishing)
{
	if ( (pAccount == NULL) || (pAccount->username == NULL) )
	{
		return IM_PROTOCOL_ERROR_FAILED;
	}
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return IM_PROTOCOL_ERROR_CANCELED_BY_APP_CLOSING;
	}
	
	string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);
	string strStatusMessage = _T("");
	int nStatus;
	
	if (!LoadUserStatus(strNormalizedUser.c_str(), nStatus, strStatusMessage))
	{
		return IM_PROTOCOL_ERROR_FAILED;
	}
	
	gboolean changed = TRUE;
	PurpleStatus *status;
	string strStatusText;
	char* pszStatusText;
	
	if (purple_account_is_connected(pAccount) == FALSE)
	{
		return IM_PROTOCOL_ERROR_NOT_CONNECTED;
	}

	if (pAccount->gc != NULL)
	{
		// Get status text.
		//
		if (GetStatusText(nStatus, strStatusText))
		{
			// Get capabilities text.
			//
			string strCapabilitiesInfo;
			
			if (bPublishing)
			{
				strCapabilitiesInfo = "Publishing capabilities ...";
			}
			else
			{
				strCapabilitiesInfo = "Unpublishing capabilities ...";
			}
			
			if (!strStatusMessage.empty())
			{
				char* pszStatusMessage = purple_utf8_try_convert(strStatusMessage.c_str());
				
				if (pszStatusMessage != NULL)
				{
					purple_account_set_status(pAccount, strStatusText.c_str(), TRUE, 
							"message", pszStatusMessage, 
							"UOLFoneCapabilities", strCapabilitiesInfo.c_str(), NULL);
				}
				
				g_free(pszStatusMessage);
			}
			else
			{
				purple_account_set_status(pAccount, strStatusText.c_str(), TRUE, 
						"UOLFoneCapabilities", strCapabilitiesInfo.c_str(), NULL);
			}
			
			return IM_PROTOCOL_ERROR_SUCCESS;
		}
	}
	
	return IM_PROTOCOL_ERROR_FAILED;
}



void CPurpleJabberUOLProtocol::SetUserExtendedCapabilities(const char* pszUser, CProtocolSettings2* pExtendedCapabilities)
{
	char* pszUTF8User = purple_utf8_try_convert(pszUser);

	if (pszUTF8User != NULL)
	{
		CPurpleJabberUOLAccountInfo* pAccountInfo = NULL;

		string strUTF8User = pszUTF8User;

		CAccountInfoMap::const_iterator it = m_mapAccountInfo.find(strUTF8User);

		if (it != m_mapAccountInfo.end())
		{
			pAccountInfo = it->second;
		}
		else
		{
			pAccountInfo = new CPurpleJabberUOLAccountInfo(strUTF8User);

			m_mapAccountInfo[strUTF8User] = pAccountInfo;
		}

		CProtocolSettings2* pNewExtendedCapabilities = NULL;
		
		if (pExtendedCapabilities)
		{
			pNewExtendedCapabilities = pExtendedCapabilities->Clone();
		}

		pAccountInfo->SetUserExtendedCapabilities(pNewExtendedCapabilities);

		g_free(pszUTF8User);
	}
}


CProtocolSettings2* CPurpleJabberUOLProtocol::GetUserExtendedCapabilities(const char* pszUser)
{
	CProtocolSettings2* pExtendedCapabilities = NULL;

	if (pszUser != NULL)
	{
		string strUser = pszUser;
		CAccountInfoMap::const_iterator it = m_mapAccountInfo.find(strUser);
		
		if (it != m_mapAccountInfo.end())
		{
			pExtendedCapabilities = (it->second)->GetUserExtendedCapabilities();
		}
	}

	return pExtendedCapabilities;
}


void CPurpleJabberUOLProtocol::SetContactExtendedCapabilities(const char* pszUser, const std::string& strContactKey, CProtocolSettings2* pExtendedCapabilities)
{
	CAccountInfoMap::const_iterator it = m_mapAccountInfo.find(pszUser);

	if ((it != m_mapAccountInfo.end()) && (it->second))
	{
		(it->second)->SetContactExtendedCapabilities(strContactKey, pExtendedCapabilities);
	}
}


CProtocolSettings2* CPurpleJabberUOLProtocol::GetContactExtendedCapabilities(const char* pszUser, const std::string& strContactKey)
{
	CProtocolSettings2* pExtendedCapabilities = NULL;

	CAccountInfoMap::const_iterator it = m_mapAccountInfo.find(pszUser);

	if ((it != m_mapAccountInfo.end()) && (it->second))
	{
		pExtendedCapabilities = (it->second)->GetContactExtendedCapabilities(strContactKey);
	}

	return pExtendedCapabilities;
}


std::string CPurpleJabberUOLProtocol::GetContactKey(PurpleBuddy* pBuddy)
{
	std::string strContactKey;

	if (pBuddy && pBuddy->name && pBuddy->account && pBuddy->account->username)
	{
		strContactKey = GetNormalizedContactName(pBuddy->name) + "::" + GetNormalizedCallbackUserName(pBuddy->account->username);
	}

	return strContactKey;
}


void CPurpleJabberUOLProtocol::RemoveExtendedCapabilites(const char* pszUser)
{
	CAccountInfoMap::iterator it = m_mapAccountInfo.find(pszUser);

	if (it != m_mapAccountInfo.end())
	{
		if (it->second)
		{
			delete it->second;
		}

		m_mapAccountInfo.erase(it);
	}
}


void CPurpleJabberUOLProtocol::RemoveAllExtendedCapabilities()
{
	CPurpleJabberUOLAccountInfo* pAccountInfo = NULL;

	for (CAccountInfoMap::const_iterator it = m_mapAccountInfo.begin(); it != m_mapAccountInfo.end(); it++)
	{
		pAccountInfo = (it->second);

		if (pAccountInfo)
		{
			delete pAccountInfo;
		}
	}
	
	m_mapAccountInfo.clear();
}


void CPurpleJabberUOLProtocol::SaveUserStatus(const char* pszUser, int nStatus, const std::string& strStatusMessage)
{
	CAccountInfoMap::iterator it = m_mapAccountInfo.find(pszUser);

	if (it != m_mapAccountInfo.end())
	{
		(it->second)->SetUserCurrentStatus(nStatus);
		(it->second)->SetUserCurrentStatusMessage(strStatusMessage);
	}
}


bool CPurpleJabberUOLProtocol::LoadUserStatus(const char* pszUser, int& nStatus, std::string& strStatusMessage)
{
	CAccountInfoMap::const_iterator it = m_mapAccountInfo.find(pszUser);

	if (it != m_mapAccountInfo.end())
	{
		nStatus = (it->second)->GetUserCurrentStatus();
		strStatusMessage = (it->second)->GetUserCurrentStatusMessage();
		
		return true;
	}

	return false;
}



void CPurpleJabberUOLProtocol::PresencePreSendCallback(xmlnode* pXmlNode, PurpleConnection* pConnection)
{
	if (pXmlNode && pConnection && pConnection->account)
	{
		PurpleAccount* pAccount = pConnection->account;

		if (pAccount->username != NULL)
		{
			string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

			CProtocolSettings2* pExtendedCapabilities = GetUserExtendedCapabilities(strNormalizedUser.c_str());

			AddPresenceExtendedCapabilities(pXmlNode, pExtendedCapabilities);
		}
	}
}


void CPurpleJabberUOLProtocol::PresenceReceivedCallback(xmlnode* pXmlNode, PurpleBuddy* pBuddy)
{
	if (pXmlNode && pBuddy)
	{
		PurpleAccount* pAccount = pBuddy->account;

		if (pAccount->username != NULL)
		{
			string strNormalizedUser = GetNormalizedCallbackUserName(pAccount->username);

			CIMProtocolCallback* pCallback = GetCallback(strNormalizedUser.c_str());

			if (pCallback)
			{
				CIMProtocolCallback2* pCallback2 = dynamic_cast<CIMProtocolCallback2*>(pCallback);

				if (pCallback2 && pBuddy->name)
				{
					string strContactkey = GetContactKey(pBuddy);

					CProtocolSettings2* pExtendedCapabilites = ExtractPresenceExtendedCapabilities(pXmlNode);
					CProtocolSettings2* pOldExtendedCapabilities = GetContactExtendedCapabilities(strNormalizedUser.c_str(), strContactkey);

					if (pOldExtendedCapabilities != pExtendedCapabilites)
					{
						SetContactExtendedCapabilities(strNormalizedUser.c_str(), strContactkey, pExtendedCapabilites);

						string strNormalizedContact = GetNormalizedContactName(pBuddy->name);

						pCallback2->OnContactExtendedCapabilitiesChanged(strNormalizedUser, strNormalizedContact, pExtendedCapabilites);
					}
				}
			}
		}
	}
}


void CPurpleJabberUOLProtocol::SetExtensionsCallback(CPurpleJabberUOLProtocol* pProtocol)
{
	CUOLCritSecLock<CAutoCriticalSectionWrapper> lock(ms_csCallback);

	ms_callbackHandler = pProtocol;

	PurplePlugin* prpl = NULL;

	prpl = purple_find_prpl(CPurpleJabberUOLProtocol::PURPLE_JABBER_UOL_PLUGIN_ID);

	if (prpl != NULL)
	{
		PurplePluginProtocolInfo* prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);

		if (prpl_info && prpl_info->get_extensions)
		{
			JabberExtensions* pJabberExtensions = (JabberExtensions*) (prpl_info->get_extensions)();

			if (pJabberExtensions &&
				(pJabberExtensions->size >= sizeof(JabberExtensions)))
			{
				if (ms_callbackHandler != NULL)
				{
					(pJabberExtensions->jabber_set_presence_pre_send_callback)(PresencePreSendCallbackHandler);
					(pJabberExtensions->jabber_set_presence_received_callback)(PresenceReceivedCallbackHandler);
				}
				else
				{
					(pJabberExtensions->jabber_set_presence_pre_send_callback)(NULL);
					(pJabberExtensions->jabber_set_presence_received_callback)(NULL);
				}
			}
		}
	}
}


void CPurpleJabberUOLProtocol::PresencePreSendCallbackHandler(xmlnode* pXmlNode, PurpleConnection* pConnection)
{
	CUOLCritSecLock<CAutoCriticalSectionWrapper> lock(ms_csCallback);

	if (ms_callbackHandler)
	{
		ms_callbackHandler->PresencePreSendCallback(pXmlNode, pConnection);
	}
}


void CPurpleJabberUOLProtocol::PresenceReceivedCallbackHandler(xmlnode* pXmlNode, PurpleBuddy* pBuddy)
{
	CUOLCritSecLock<CAutoCriticalSectionWrapper> lock(ms_csCallback);

	if (ms_callbackHandler)
	{
		ms_callbackHandler->PresenceReceivedCallback(pXmlNode, pBuddy);
	}
}


void CPurpleJabberUOLProtocol::AddPresenceExtendedCapabilities(xmlnode* pXmlNode, CProtocolSettings2* pExtendedCapabilities)
{
	xmlnode* pTempCapsNode = NULL;
	
	// Firstly, we need to remove any "c" (capabilities) tag already added by 
	// libpurple.
	//
	pTempCapsNode = xmlnode_get_child(pXmlNode, PRESENCE_CAPABILITIES_NODE);
	
	if (pTempCapsNode)
	{
		xmlnode_free(pTempCapsNode);
	}
	
	// Now we add the UOL presence tag.
	//
	if (pXmlNode && pExtendedCapabilities)
	{
		xmlnode* c = xmlnode_new_child(pXmlNode, PRESENCE_CAPABILITIES_NODE);
		if (c)
		{
			string strExt;

			pExtendedCapabilities->GetString(PRESENCE_EXT_ATTRIB, strExt);

			xmlnode_set_attrib(c, PRESENCE_XMLNS_ATTRIB, PRESENCE_XMLNS_VALUE);
			xmlnode_set_attrib(c, PRESENCE_NODE_ATTRIB, PRESENCE_NODE_VALUE);
			xmlnode_set_attrib(c, PRESENCE_VER_ATTRIB, PRESENCE_VER_VALUE);
			xmlnode_set_attrib(c, PRESENCE_EXT_ATTRIB, strExt.c_str());
		}
	}
}


 CProtocolSettings2* CPurpleJabberUOLProtocol::ExtractPresenceExtendedCapabilities(xmlnode* pXmlNode)
{
	CProtocolSettings2* pExtendedCapabilities = NULL;

	xmlnode *pChild;

	for (pChild = pXmlNode->child; pChild; pChild = pChild->next)
	{
		if (pChild->type != XMLNODE_TYPE_TAG)
			continue;

		if (!strcmp(pChild->name, PRESENCE_CAPABILITIES_NODE))
		{
			const char *pszXmlns, *pszNode, *pszVer, *pszExt;

			pExtendedCapabilities = new CProtocolSettings2();
				
			pszXmlns = xmlnode_get_attrib(pChild, PRESENCE_XMLNS_ATTRIB);
			if (pszXmlns)
			{
				pExtendedCapabilities->SetString(PRESENCE_XMLNS_ATTRIB, pszXmlns);
			}

			pszNode = xmlnode_get_attrib(pChild, PRESENCE_NODE_ATTRIB);
			if (pszNode)
			{
				pExtendedCapabilities->SetString(PRESENCE_NODE_ATTRIB, pszNode);
			}

			pszVer = xmlnode_get_attrib(pChild, PRESENCE_VER_ATTRIB);
			if (pszVer)
			{
				pExtendedCapabilities->SetString(PRESENCE_VER_ATTRIB, pszVer);
			}

			pszExt = xmlnode_get_attrib(pChild, PRESENCE_EXT_ATTRIB);
			if (pszExt)
			{
				pExtendedCapabilities->SetString(PRESENCE_EXT_ATTRIB, pszExt);
			}

			break;
		}
	}

	return pExtendedCapabilities;
}
