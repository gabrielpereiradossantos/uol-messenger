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
 
#include "purpleservice.h"

#include <vector>
#include <assert.h>
#ifdef WIN32
#	include <windows.h>
#endif

#include "uimeventloop.h"
#include "purpleprotocol.h"
#include "purplemsnprotocol.h"
#include "purpleoscaricqprotocol.h"
#include "purpleoscaraimprotocol.h"
#include "purpleyahooprotocol.h"
#include "purplejabberprotocol.h"
#include "purplejabberuolprotocol.h"
#include "purplejabberuoltoprotocol.h"
#include "purpleutils.h"


// Purple headers
#include <signals.h>
#include <buddyicon.h>

// libxml2 headers
#include <libxml/parser.h>


#ifdef _DEBUG
	// Uncomment for purple debug messages printing
	#define DEBUGPRINT 1
#endif


using namespace std;

const char* CPurpleService::UIM_PURPLE_CORE = "UIM";

const long CPurpleService::PURPLE_SERVICE_SIGNAL = 1717;

const long CPurpleService::PURPLE_SERVICE_HANDLE = 3475;



//
// Purple callbacks structs
//
PurpleCoreUiOps CPurpleService::m_coreOps =
{
	CoreUIPrefsInit,
	CoreDebugUIInit,
	CoreUIInit,
	CoreQuit,
	NULL,
	NULL,
	NULL,
	NULL
};

PurpleEventLoopUiOps CPurpleService::m_eventLoopOps =
{
	EventLoopTimeoutAdd,
	EventLoopTimeoutRemove,
	EventLoopInputAdd,
	EventLoopInputRemove,
	NULL,						// input_get_error
	NULL,						// reserved
	NULL,						// reserved
	NULL,						// reserved
	NULL						// reserved
};

PurpleDebugUiOps CPurpleService::m_debugOps =
{
	DebugPrint,
	NULL,						// pidgin_debug_is_enabled
	NULL,
	NULL,
	NULL,
	NULL
};

PurpleConnectionUiOps CPurpleService::m_connectionOps = 
{
	ConnectionConnectProgress,
	ConnectionConnected,
	ConnectionDisconnected,
	ConnectionNotice,
	ConnectionReportDisconnect,
	ConnectionNetworkConnected,
	ConnectionNetworkDisconnected,
	NULL,
	NULL,
	NULL,
	NULL
};

PurpleAccountUiOps CPurpleService::m_accountOps =
{
	AccountNotifyAdded,
	AccountStatusChanged,
	AccountRequestAdd,
	AccountRequestAuthorize,
	AccountRequestAuthorizeClose,	// close_account_request,
	NULL,							// AccountAddCustomSmiley
	NULL,							// AccountDelCustomSmiley
	NULL,
	NULL
};

PurpleBlistUiOps CPurpleService::m_buddyListOps = 
{
	BuddyListNewList,
	BuddyListNewNode,
	BuddyListShow,
	BuddyListUpdate,
	BuddyListRemove,
	BuddyListDestroy,
	BuddyListSetVisible,
	BuddyListRequestAddBuddy,
	BuddyListRequestAddChat,
	BuddyListRequestAddGroup,
	NULL,
	NULL,
	NULL,
	NULL
};

PurpleConversationUiOps CPurpleService::m_conversationOps =
{
	ConversationCreateConversation,
	ConversationDestroyConversation,
	ConversationWriteChat,
	ConversationWriteIm,
	ConversationWriteConv,
	ConversationChatAddUsers,
	ConversationChatRenameUser,
	ConversationChatRemoveUsers,
	ConversationChatUpdateUser,
	ConversationPresent,
	ConversationHasFocus,
	ConversationCustomSmileyAdd,
	ConversationCustomSmileyWrite,
	ConversationCustomSmileyClose,
	ConversationSendConfirm,
	NULL,
	NULL,
	NULL,
	NULL
};

/* UOL <PURPLE Version 2.0.0 Adjustments> << */
/*
PurpleConvWindowUiOps CPurpleService::m_conversationWindowOps =
{
	ConversationWindowGetConversationUIOps,
	ConversationWindowNewWindow,
	ConversationWindowDestroyWindow,
	ConversationWindowShow,
	ConversationWindowHide,
	ConversationWindowRaise,
	ConversationWindowFlash,
	ConversationWindowSwitchConversation,
	ConversationWindowAddConversation,
	ConversationWindowRemoveConversation,
	ConversationWindowMoveConversation,
	ConversationWindowGetActiveIndex,
	ConversationWindowHasFocus
};
*/
/* UOL >> */

PurpleNotifyUiOps CPurpleService::m_notifyOps =
{
	NotifyMessage,
	NotifyEmail,
	NotifyEmails,
	NotifyFormatted,
	NotifySearchResults,
	NotifySearchResultsNewRows,
	NotifyUserInfo,
	NotifyURI,
	NotifyCloseNotify,
	NULL,
	NULL,
	NULL,
	NULL
};

PurpleRequestUiOps CPurpleService::m_requestOps = 
{
	RequestInput,
	RequestChoice,
	RequestAction,
	RequestFields,
	RequestFile,
	RequestCloseRequest, 
	RequestFolder,
	NULL,
	NULL,
	NULL,
	NULL
};

PurpleXferUiOps CPurpleService::m_fileTransferOps =
{
	FileTransferNewXfer,
	FileTransferDestroy,
	FileTransferAddXfer,
	FileTransferUpdateProgress,
	FileTransferCancelLocal,
	FileTransferCancelRemote,
	NULL,
	NULL,
	NULL,
	NULL
};

//PurpleRoomlistUiOps CPurpleService::m_roomlistOps =
//{
//	RoomListShowWithAccount,
//	RoomListNew,
//	RoomListSetFields,
//	RoomListAddRoom,
//	RoomListInProgress,
//	RoomListDestroy,
//	NULL,
//	NULL,
//	NULL,
//	NULL
//};

PurplePrivacyUiOps CPurpleService::m_privacyOps = 
{
	PrivacyPermitAdded,
	PrivacyPermitRemoved,
	PrivacyDenyAdded,
	PrivacyDenyRemoved,
	NULL,
	NULL,
	NULL,
	NULL
};


typedef std::pair<char*, CPurpleProtocolStack*> CProtocolPair;


std::string CPurpleService::m_strIMCustomEmoticon = "";

BOOL	CPurpleService::m_bEventsInitialized = FALSE;
CEvent	CPurpleService::m_uimConnectionsFinalizerEvent;


GMutex*				CPurpleService::m_pMutex = NULL;
int					CPurpleService::m_nFinalizeInProgress = FALSE;
CPurpleProtocolMap	CPurpleService::m_mapProtocols;

int					CPurpleService::m_nPreFinalizeDone = FALSE;

CPurpleProtocolAsyncExec*	CPurpleService::m_pAsyncOperationsExec = NULL;

CAccountRequestParamList	CPurpleService::m_accountRequestParamList;

IPurpleDebugLogger*			CPurpleService::m_pPurpleDebugLogger = NULL;


CPurpleService* CPurpleService::GetInstance()
{
	static CPurpleService purpleService;

	return &purpleService;
}


CPurpleService::CPurpleService() : 
	m_pMainLoop(NULL),
	m_pPurpleServiceThread(NULL),
	m_nSignalHandle(PURPLE_SERVICE_SIGNAL),
	m_nServiceHandle(PURPLE_SERVICE_HANDLE),
    m_bWizardRunning(FALSE),
	m_dwUIMThreadId(0),
	m_dwAsyncOperationsThreadId(0)
{
	if (!g_thread_supported())
	{
		g_thread_init(NULL);
	}

	m_pMutex = g_mutex_new();
	
	m_strIMCustomEmoticon.clear();
	
	if (!m_bEventsInitialized)
	{
		m_bEventsInitialized = m_uimConnectionsFinalizerEvent.Create(
				NULL, TRUE, TRUE, NULL);
	}
}


CPurpleService::~CPurpleService(void)
{
	if (m_uimConnectionsFinalizerEvent)
	{
		m_uimConnectionsFinalizerEvent.Close();
	}
	
	g_mutex_free(m_pMutex);
}


int CPurpleService::Init(char* pszCurrentProfileFullPath, DWORD dwUIMThreadId)
{
	m_dwUIMThreadId = dwUIMThreadId;
	
#ifdef WIN32
	// Winsock init
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2 ) 
	{
		WSACleanup();
		return -1;
	}
#endif
	
	// Initialize libxml2 library.
	//
	xmlInitParser();
	
	// Initialize UI ops structures...
	purple_core_set_ui_ops(&m_coreOps);
	purple_eventloop_set_ui_ops(&m_eventLoopOps);
	
	if (pszCurrentProfileFullPath != NULL)
	{
		char* pszUOLLibPurpleDir = g_build_filename(
				pszCurrentProfileFullPath, ".purple", NULL);
		
		if (pszUOLLibPurpleDir)
		{
			char* pszUTF8UOLLibPurpleDir = purple_utf8_try_convert(pszUOLLibPurpleDir);
			
			purple_util_set_user_dir(pszUTF8UOLLibPurpleDir);
			
			g_free(pszUTF8UOLLibPurpleDir);
		}
		
		g_free(pszUOLLibPurpleDir);
	}
	
	gboolean init_ok = purple_core_init(CPurpleService::UIM_PURPLE_CORE);

	if (init_ok)
	{
		// Probe Purple plugins...
		purple_plugins_probe(NULL);		// TODO: Why do we need this ??????

		GList* pListPlugins = NULL;
		char* pszPluginId = NULL;
		
		CPurpleProtocol* pPurpleProtocol = NULL;
		CPurpleProtocolStack* pPurpleProtocolStack = NULL;

		for (pListPlugins = purple_plugins_get_protocols(); 
			 pListPlugins != NULL;
			 pListPlugins = pListPlugins->next)
		{
			pszPluginId = ((PurplePlugin*) pListPlugins->data)->info->id;

			if (0 == strcmp(pszPluginId, CPurpleMSNProtocol::PURPLE_MSN_PLUGIN_ID))
			{
				pPurpleProtocol = new CPurpleMSNProtocol();
			}
			else if (0 == strcmp(pszPluginId, CPurpleOscarICQProtocol::PURPLE_OSCAR_ICQ_PLUGIN_ID))
			{
				pPurpleProtocol = new CPurpleOscarICQProtocol();
			}
			//else if (0 == strcmp(pszPluginId, CPurpleOscarAIMProtocol::PURPLE_OSCAR_AIM_PLUGIN_ID))
			//{
				//pPurpleProtocol = new CPurpleOscarAIMProtocol();
			//}
			else if (0 == strcmp(pszPluginId, CPurpleJabberProtocol::PURPLE_JABBER_PLUGIN_ID))
			{
				pPurpleProtocol = new CPurpleJabberProtocol();
			}
			else if (0 == strcmp(pszPluginId, CPurpleJabberUOLTOProtocol::PURPLE_JABBER_UOLTO_PLUGIN_ID))
			{
				pPurpleProtocol = new CPurpleJabberUOLTOProtocol();
			}
			else if (0 == strcmp(pszPluginId, CPurpleJabberUOLProtocol::PURPLE_JABBER_UOL_PLUGIN_ID))
			{
				pPurpleProtocol = new CPurpleJabberUOLProtocol();
			}
			else if (0 == strcmp(pszPluginId, CPurpleYahooProtocol::PURPLE_YAHOO_PLUGIN_ID))
			{
				pPurpleProtocol = new CPurpleYahooProtocol();				
			}

			if (pPurpleProtocol != NULL)
			{
				pPurpleProtocolStack = new CPurpleProtocolStack();

				pPurpleProtocolStack->AddProtocol(pPurpleProtocol, NULL);

				pPurpleProtocolStack->Init();

				m_mapProtocols.insert(CProtocolPair(pszPluginId, pPurpleProtocolStack));				

				pPurpleProtocol = NULL;
			}			
		}

		// Initialize buddy list
		purple_set_blist(purple_blist_new());
		//purple_blist_load();
		
		/* TODO: Move prefs loading into purple_prefs_init() */
		//purple_prefs_load();
		//purple_prefs_update_old();
		//purple_gtk_prefs_update_old();
		
		/* load plugins we had when we quit */
		//purple_plugins_load_saved("/purple/gtk/plugins/loaded");
		//purple_gtk_docklet_init();
		
		/* TODO: Move pounces loading into purple_pounces_init() */
		//purple_pounces_load();
		
		m_pPurpleServiceThread = g_thread_create((GThreadFunc) PurpleServiceThread, (gpointer) this, TRUE, NULL);
		
		CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) StartAsyncOperationsThread, NULL);
	}
	
	return (m_pPurpleServiceThread != NULL);
}


int CPurpleService::Finalize()
{
	int nRet = 0;
	DWORD dwWaitReason = 0;
	
	g_mutex_lock(m_pMutex);
		m_nFinalizeInProgress = TRUE;
	g_mutex_unlock(m_pMutex);
	
	//
	// ProtocolStack must be finalized on libpurple's thread, or method "xmlParseChunk" 
	// causes a crash on "libuol" library (ticket 1306).
	//
	// Don't ask me why... I only know that it work's.
	// It should be another CRT library incompatibility issue for sure.
	//
	
	m_uimConnectionsFinalizerEvent.Reset();
	
	// "FinalizeInProgress" was already been set, so don't use "purple_timeout_add" 
	// from now on.
	//
	guint iEventSource = g_timeout_add(500, (GSourceFunc)UIMConnectionsFinalizerMethod, NULL);
	
	dwWaitReason = ::WaitForSingleObject(m_uimConnectionsFinalizerEvent, INFINITE);
	
	if (dwWaitReason != WAIT_OBJECT_0)
	{
		//TODO: log some information message.
	}
	
	//
	// Now proceed with normal finalization tasks.
	//
	
	// Finalize async operations thread handler.
	//
	if (CPurpleService::GetInstance()->GetAsyncOperationsExec())
	{
		CPurpleService::GetInstance()->GetAsyncOperationsExec()->FinishAsyncOperations();
	}
	
	CThreadVector::iterator itThread;
	GThread* pThread;
	for (itThread = m_vectorThreads.begin(); itThread != m_vectorThreads.end(); itThread++)
	{
		pThread = *itThread;
		g_thread_join(pThread);
	}
	
	GMainLoop* pLoop;
	
	g_mutex_lock(m_pMutex);
		pLoop = m_pMainLoop;
		m_pMainLoop = NULL;
	g_mutex_unlock(m_pMutex);
	
	g_main_loop_quit(pLoop);
	
	if (m_pPurpleServiceThread != NULL)
	{
		nRet = (int) g_thread_join(m_pPurpleServiceThread);
		m_pPurpleServiceThread = NULL;
	}

	// Release protocols...
	CPurpleProtocolMap::iterator itProtocols;

	CPurpleProtocolStack* pPurpleProtocolStack;	

	for (itProtocols = m_mapProtocols.begin(); 
			itProtocols != m_mapProtocols.end();
			itProtocols++)
	{
		pPurpleProtocolStack = itProtocols->second;

		CIMProtocol* pProtocol = pPurpleProtocolStack->GetNextProtocol();

		while (pProtocol && pProtocol->GetNextProtocol())
		{
			pProtocol = pProtocol->GetNextProtocol();
		}
		
		delete pProtocol;
		delete pPurpleProtocolStack;		
	}

	// Close notifications with service handle
	void* lpServiceHandle = GetPurpleServiceHandle();

	purple_notify_close(PURPLE_NOTIFY_MESSAGE, lpServiceHandle);
	purple_notify_close(PURPLE_NOTIFY_EMAIL, lpServiceHandle);
	purple_notify_close(PURPLE_NOTIFY_EMAILS, lpServiceHandle);
	purple_notify_close(PURPLE_NOTIFY_FORMATTED, lpServiceHandle);
	purple_notify_close(PURPLE_NOTIFY_USERINFO, lpServiceHandle);
	purple_notify_close(PURPLE_NOTIFY_URI, lpServiceHandle);
	purple_notify_close(PURPLE_NOTIFY_SEARCHRESULTS, lpServiceHandle);
	
	purple_request_close(PURPLE_REQUEST_INPUT, lpServiceHandle);
	purple_request_close(PURPLE_REQUEST_ACTION, lpServiceHandle);
	purple_request_close(PURPLE_REQUEST_FIELDS, lpServiceHandle);

	purple_core_quit();

	// Finalize libxml2 library.
	//
	xmlCleanupParser();
	
	m_mapProtocols.clear();

	// Clear AccountRequestAuthorize params list...
	//
	while (m_accountRequestParamList.size() > 0)
	{
		AccountRequestAuthorizeParam* pRequestParam = m_accountRequestParamList.front();
		
		if (pRequestParam)
		{
			m_accountRequestParamList.pop_front();
			
			delete pRequestParam;
		}
	}
	
	m_accountRequestParamList.clear();
	
#ifdef WIN32
	int nWSARet = WSACleanup();
	assert(0 == nWSARet);
#endif

	return nRet;
}

void CPurpleService::PreFinalize()
{
	g_mutex_lock(m_pMutex);
		m_nPreFinalizeDone = TRUE;
	g_mutex_unlock(m_pMutex);
}

int CPurpleService::IsRunning()
{
	int nRet = FALSE;

	g_mutex_lock(m_pMutex);
		nRet = (m_pMainLoop != NULL);
	g_mutex_unlock(m_pMutex);

	return nRet;
}


int CPurpleService::FinalizeInProgress()
{
    int nRet;

	g_mutex_lock(m_pMutex);
		nRet = m_nFinalizeInProgress;
	g_mutex_unlock(m_pMutex);

	return nRet;
}


void CPurpleService::SetFinalizeInProgress(int bIsFinalizing)
{
	g_mutex_lock(m_pMutex);
		m_nFinalizeInProgress = bIsFinalizing;
	g_mutex_unlock(m_pMutex);
}


int CPurpleService::PreFinalizeDone()
{
    int nRet;
	
	g_mutex_lock(m_pMutex);
		nRet = m_nPreFinalizeDone;
	g_mutex_unlock(m_pMutex);
	
	return nRet;
}


void* CPurpleService::GetPurpleServiceHandle()
{
	return (void*) m_nServiceHandle;
}


void CPurpleService::StartWorkerThread(GThreadFunc pfnThreadFunction, void* pParam)
{
	if (FinalizeInProgress())
	{
		return;
	}

	GThread* pThreadHandler = g_thread_create((GThreadFunc) pfnThreadFunction, (gpointer) pParam, TRUE, NULL);

	g_mutex_lock(m_pMutex);
		m_vectorThreads.push_back(pThreadHandler);
	g_mutex_unlock(m_pMutex);
}


void CPurpleService::SetPluginsSearchPath(int nCount, char** ppszPaths)
{
	if ((ppszPaths != NULL) && (nCount > 0))
	{
		for (int index = 0; index < nCount; index++)
		{
			purple_plugins_add_search_path(ppszPaths[index]);
		}
	}
}


void CPurpleService::GetProtocolsList(std::list<CIMProtocol*>& listProtocols)
{
	listProtocols.clear();

	CPurpleProtocolMap::iterator itProtocols;

	CIMProtocol* pPurpleProtocol;
	for (itProtocols = m_mapProtocols.begin(); 
		 itProtocols != m_mapProtocols.end();
		 itProtocols++)
	{
		pPurpleProtocol = itProtocols->second;
		listProtocols.push_back(pPurpleProtocol);
	}
}


CPurpleProtocolStack* CPurpleService::GetProtocolStack(const char* pszId)
{
	CPurpleProtocolStack* pProtocol = NULL;
	CPurpleProtocolMap::const_iterator itProtocols;

	if (pszId != NULL)
	{
		itProtocols = m_mapProtocols.find(pszId);

		if (itProtocols != m_mapProtocols.end())
		{
			pProtocol = itProtocols->second;
		}
	}

	return pProtocol;
}


CPurpleProtocol* CPurpleService::GetProtocol(const char* pszId)
{
	CIMProtocol* pProtocol = GetProtocolStack(pszId);

	while (pProtocol && pProtocol->GetNextProtocol())
	{
		pProtocol = pProtocol->GetNextProtocol();
	}

	return static_cast<CPurpleProtocol*>(pProtocol);
}


void CPurpleService::AddProtocolToStack(const char* pszId, CIMProtocol* pProtocol, CIMProtocolCallback* pCallback)
{
	if (!pProtocol)
	{
		return;
	}
	
	CPurpleProtocolStack* pPurpleProtocol = GetProtocolStack(pszId);

	if (pPurpleProtocol)
	{
		pProtocol->Init();
		pPurpleProtocol->AddProtocol(pProtocol, pCallback);		
	}
}

void CPurpleService::RemoveProtocolFromStack(const char* pszId, CIMProtocol* pProtocol, CIMProtocolCallback* pCallback)
{
	if (!pProtocol)
	{
		return;
	}
	
	CPurpleProtocolStack* pPurpleProtocol = GetProtocolStack(pszId);

	if (pPurpleProtocol)
	{
		pPurpleProtocol->RemoveProtocol(pProtocol, pCallback);
		pProtocol->Finalize();
	}
}

int CPurpleService::Run()
{
	int nRet = 0;

	// Main loop...
	GMainLoop* pLoop = g_main_loop_new(NULL, FALSE);

	g_mutex_lock(m_pMutex);
		m_nFinalizeInProgress = FALSE;
		m_nPreFinalizeDone = FALSE;
		m_pMainLoop = pLoop;
	g_mutex_unlock(m_pMutex);

	if (pLoop != NULL)
	{
		g_main_loop_run(pLoop);
	}

	return nRet;
}


void CPurpleService::ConnectPurpleSignals()
{
	void* pBlistHandle = purple_blist_get_handle();
	void* pPurpleServiceHandle = GetSignalHandle();
    void* pConnectionHandle = purple_conversations_get_handle();
	
	// Signals for sign-on/sign-off
	purple_signal_connect(pBlistHandle, "buddy-signed-on", pPurpleServiceHandle, 
						PURPLE_CALLBACK(PurpleBuddySignedOnEventCallback), NULL);
	purple_signal_connect(pBlistHandle, "buddy-signed-off", pPurpleServiceHandle, 
						PURPLE_CALLBACK(PurpleBuddySignedOffEventCallback), NULL);
	
	// Signals for away and status change
	purple_signal_connect(pBlistHandle, "buddy-status-changed", pPurpleServiceHandle, 
						PURPLE_CALLBACK(PurpleBuddyStatusChangeEventCallback), NULL);
	
	// Signals for idle change
	purple_signal_connect(pBlistHandle, "buddy-idle-changed", pPurpleServiceHandle, 
						PURPLE_CALLBACK(PurpleBuddyIdleChangedEventCallback), NULL);
	
	// Signals for icon change
	purple_signal_connect(pBlistHandle, "buddy-icon-changed", pPurpleServiceHandle, 
						PURPLE_CALLBACK(PurpleBuddyIconChangedEventCallback), NULL);
	
	// Signals for node alias change
	purple_signal_connect(pBlistHandle, "blist-node-aliased", pPurpleServiceHandle, 
						PURPLE_CALLBACK(PurpleBlistNodeAliasedEventCallback), NULL);

    // Signals for buddy typing
    purple_signal_connect(pConnectionHandle, "buddy-typing", pPurpleServiceHandle, 
						PURPLE_CALLBACK(PurpleBuddyTypingEventCallback), NULL);

	// Signals for buddy typing stoped
    purple_signal_connect(pConnectionHandle, "buddy-typing-stopped", pPurpleServiceHandle, 
						PURPLE_CALLBACK(PurpleBuddyTypingStoppedEventCallback), NULL);
}

void CPurpleService::DisconnectPurpleSignals()
{
	purple_signals_disconnect_by_handle(GetSignalHandle());
}

void* CPurpleService::GetSignalHandle()
{
	return (void*) m_nSignalHandle;
}



int CPurpleService::PurpleServiceThread(void* pData)
{
	CPurpleService* pPurpleService = (CPurpleService*) pData;

	int nRet = pPurpleService->Run();

	return nRet;
}


//
// Purple core callbacks
//
void CPurpleService::CoreUIPrefsInit(void)
{
	g_print("%s\n", __FUNCTION__);

	// Setup Purple core preferences...
	// Disable away handling
	purple_prefs_set_bool("/purple/away/away_when_idle", FALSE);		// New one...
	purple_prefs_set_string("/purple/away/idle_reporting", "none");		// New one...
	purple_prefs_set_string("/purple/away/auto_reply", "never");		// New one...
	
	purple_prefs_set_bool("/core/conversations/away_back_on_send", FALSE);	// TODO: Should be removed ???
	purple_prefs_set_bool("/core/away/auto_response/enabled", FALSE);		// TODO: Should be removed ???
	purple_prefs_set_string("/core/away/auto_reply","never");				// TODO: Should be removed ???

	// Disable conversation logging
	purple_prefs_set_bool("/purple/gtk/logging/log_chats", FALSE);
	purple_prefs_set_bool("/purple/gtk/logging/log_ims", FALSE);

	// Enable typing send
	purple_prefs_set_bool("/core/conversations/im/send_typing", TRUE);

	// Enable MSN notifications
	purple_prefs_set_bool("/plugins/prpl/msn/conv_close_notice", TRUE);
	purple_prefs_set_bool("/plugins/prpl/msn/conv_timeout_notice", TRUE);

	// User server alias
	purple_prefs_set_bool("/core/buddies/use_server_alias", TRUE);
}

void CPurpleService::CoreDebugUIInit(void)
{
	g_print("%s\n", __FUNCTION__);

	purple_debug_set_ui_ops(&m_debugOps);
}

void CPurpleService::CoreUIInit(void)
{
	g_print("%s\n", __FUNCTION__);

	purple_connections_set_ui_ops(&m_connectionOps);
	purple_accounts_set_ui_ops(&m_accountOps);
	purple_blist_set_ui_ops(&m_buddyListOps);
	purple_conversations_set_ui_ops(&m_conversationOps);
	purple_notify_set_ui_ops(&m_notifyOps);
	purple_request_set_ui_ops(&m_requestOps);
	purple_xfers_set_ui_ops(&m_fileTransferOps);
	//purple_roomlist_set_ui_ops(&m_roomlistOps);
	purple_privacy_set_ui_ops(&m_privacyOps);

	CPurpleService::GetInstance()->ConnectPurpleSignals();
}

void CPurpleService::CoreQuit(void)
{
	g_print("%s\n", __FUNCTION__);

	CPurpleService::GetInstance()->DisconnectPurpleSignals();
}


//
// Purple event loop callbacks
//
guint CPurpleService::EventLoopTimeoutAdd(guint nInterval, GSourceFunc function, gpointer pData)
{
	//g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}

	return g_timeout_add(nInterval, function, pData);
}

gboolean CPurpleService::EventLoopTimeoutRemove(guint nHandle)
{
	//g_print("%s\n", __FUNCTION__);

	return g_source_remove(nHandle);
}

guint CPurpleService::EventLoopInputAdd(int nFd, PurpleInputCondition cond, PurpleInputFunction func, gpointer pData)
{
	//g_print("%s - fd = %d, cond = %s\n", __FUNCTION__, nFd, (cond == PURPLE_INPUT_READ) ? "PURPLE_INPUT_READ" : "PURPLE_INPUT_WRITE");
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}

	return purple_glib_input_add((gint) nFd, cond, func, pData);
}

gboolean CPurpleService::EventLoopInputRemove(guint nHandle)
{
	//g_print("%s - handle = %d\n", __FUNCTION__, nHandle);

	return g_source_remove(nHandle);
}


//
// Purple debug callbacks
//
void CPurpleService::DebugPrint(PurpleDebugLevel level, const char* pszCategory,
					const char* pszArgs)
{
	if (m_pPurpleDebugLogger != NULL)
	{
		m_pPurpleDebugLogger->DebugLog(level, pszCategory, pszArgs);
	}
}


//
// Purple connection callbacks
//
void CPurpleService::ConnectionConnectProgress(PurpleConnection* pPurpleConn, const char* pszText,
							 size_t nStep, size_t nStepCount)
{
	g_print("%s - \"%s\", step = %d/%d\n", __FUNCTION__, pszText, nStep, nStepCount);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if ((pPurpleConn != NULL) &&
		(pPurpleConn->account != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pPurpleConn->account->protocol_id);

		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->OnConnectProgress(pPurpleConn->account, pszText, (int) nStep, (int) nStepCount);
		}
	}
}

void CPurpleService::ConnectionConnected(PurpleConnection* pPurpleConn)
{
	g_print("%s, pPurpleConn = 0x%x\n", __FUNCTION__, pPurpleConn);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if ((pPurpleConn != NULL) &&
		(pPurpleConn->account != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pPurpleConn->account->protocol_id);
		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->OnConnected(pPurpleConn->account);
		}
	}
}

void CPurpleService::ConnectionDisconnected(PurpleConnection* pPurpleConn)
{
	g_print("%s, pPurpleConn = 0x%x\n", __FUNCTION__, pPurpleConn);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if ((pPurpleConn != NULL) &&
		(pPurpleConn->account != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pPurpleConn->account->protocol_id);
		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->OnDisconnected(pPurpleConn->account);
		}
	}
}

void CPurpleService::ConnectionNotice(PurpleConnection* pPurpleConn, const char* pszText)
{
	g_print("%s, pPurpleConn = 0x%x, pszText = %s\n", __FUNCTION__, pPurpleConn, (pszText != NULL) ? pszText : NULL);

	//g_return_if_fail((pPurpleConn != NULL) &&
	//				 (pPurpleConn->account != NULL) &&
	//				 (pPurpleConn->account->protocol_id != NULL));

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}
}

void CPurpleService::ConnectionReportDisconnect(PurpleConnection* pPurpleConn, const char* pszText)
{
	g_print("%s, pPurpleConn = 0x%x, pszText = %s\n", __FUNCTION__, pPurpleConn, (pszText != NULL) ? pszText : NULL);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if ((pPurpleConn != NULL) &&
		(pPurpleConn->account != NULL) &&
		((pPurpleConn->state == PURPLE_CONNECTED) || (pPurpleConn->state == PURPLE_CONNECTING)) &&
		(pszText != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pPurpleConn->account->protocol_id);
		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->OnReportDisconnect(pPurpleConn->account, pszText);
		}
	}
}

void CPurpleService::ConnectionNetworkConnected(void)
{
	g_print("%s\n", __FUNCTION__);
}


void CPurpleService::ConnectionNetworkDisconnected(void)
{
	g_print("%s\n", __FUNCTION__);
}


//
// Purple account callback
//
void CPurpleService::AccountNotifyAdded(PurpleAccount* pAccount, const char* pszRemoteUser,
						const char* /*pszId*/, const char* /*pszAlias*/,
						const char* /*pszMessage*/)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if ((pAccount != NULL) &&
		(pszRemoteUser != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pAccount->protocol_id);
		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->OnNotifyAdded(pAccount, pszRemoteUser);
		}
	}
}


void CPurpleService::AccountStatusChanged(PurpleAccount* pAccount, PurpleStatus* pStatus)
{
	g_print("%s\n", __FUNCTION__);
}


void CPurpleService::AccountRequestAdd(PurpleAccount* pAccount, const char* pszRemoteUser,
					const char* pszId, const char* pszAlias,
					const char* pszMessage)
{
	g_print("%s\n", __FUNCTION__);
}


void *CPurpleService::AccountRequestAuthorize(PurpleAccount* pAccount, const char* pszRemoteUser, 
					const char* pszId, const char* pszAlias, const char* pszMessage, 
					gboolean bOnList, PurpleAccountRequestAuthorizationCb pAuthorizeCallback, 
					PurpleAccountRequestAuthorizationCb pDenyCallback, void* pUserData)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return NULL;
	}
    
    if (CPurpleService::GetInstance()->IsWizardRunning())
	{
		return NULL;
	}
	
	AccountRequestAuthorizeParam* pParam = NULL;
	
	if ( (pAccount != NULL) && (pszRemoteUser != NULL) )
	{
		pParam = new AccountRequestAuthorizeParam;
		
		if (pParam)
		{
			pParam->pPurpleAccount = pAccount;
			pParam->strRemoteUser = (pszRemoteUser != NULL) ? pszRemoteUser : "";
			pParam->strId = (pszId != NULL) ? pszId : "";
			pParam->strAlias = (pszAlias != NULL) ? pszAlias : "";
			pParam->strMessage = (pszMessage != NULL) ? pszMessage : "";
			pParam->bOnList = bOnList;
			pParam->pAuthorizeCallback = pAuthorizeCallback;
			pParam->pDenyCallback = pDenyCallback;
			pParam->pUserData = pUserData;
			pParam->bRequestIsCanceled = 0;
			
			CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) AccountRequestAuthorizeThread, pParam);
			
			// Store it to be released only at the end of all...
			//
			m_accountRequestParamList.push_back(pParam);
		}
	}
	
	return (void*) pParam;
}


/*
void CPurpleService::AccountAddCustomSmiley(PurpleAccount *pAccount, const char *pszSmile, 
						const char *pszFile)
{
	PurplePlugin *proto;
	PurplePluginProtocolInfo *prpl_info = NULL;
	PurpleConnection *pConnection;
	
	purple_debug_info("custom-smiley", "add to list filename: %s\n", pszFile);
	
	pConnection = purple_account_get_connection(pAccount);
	
	proto = purple_find_prpl(pAccount->protocol_id);
	if (!proto)
	{
		return;
	}
	
	prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(proto);
	
	if ( (pConnection) && (prpl_info) && (prpl_info->add_smiley) )
	{
		gsize szSmileShortcut = lstrlen(pszSmile);
		guchar* smileShortcut = g_base64_decode(pszSmile, &szSmileShortcut);
		smileShortcut[szSmileShortcut] = '\0';
		
		if (smileShortcut)
		{
			prpl_info->add_smiley(pConnection, (char *)smileShortcut, pszFile, FALSE);
			
			g_free(smileShortcut);
		}
	}
}


void CPurpleService::AccountDelCustomSmiley(PurpleAccount *pAccount, const char *pszSmile)
{
	PurplePlugin *proto;
	PurplePluginProtocolInfo *prpl_info = NULL;
	
	proto = purple_find_prpl(pAccount->protocol_id);
	if (!proto)
	{
		return;
	}
	
	prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(proto);
	
	if ( (prpl_info) && (prpl_info->del_smiley) )
	{
		gsize szSmileShortcut = lstrlen(pszSmile);
		guchar* smileShortcut = g_base64_decode(pszSmile, &szSmileShortcut);
		smileShortcut[szSmileShortcut] = '\0';
		
		if (smileShortcut)
		{
			prpl_info->del_smiley(pAccount->gc, (char *)smileShortcut);
			
			g_free(smileShortcut);
		}
	}
}
*/


void CPurpleService::AccountRequestAuthorizeClose(void *ui_handle)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}
	
	AccountRequestAuthorizeParam* pParam = 
			(AccountRequestAuthorizeParam*)ui_handle;
	
	if (NULL == pParam)
	{
		return;
	}
	
	CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(
			pParam->pPurpleAccount->protocol_id);
	
	if (pPurpleProtocol != NULL)
	{
		pPurpleProtocol->ProcessAccountRequestAuthorizeCancel(pParam);
	}
	
	return;
}



//
// Purple buddy list callback
//
void CPurpleService::BuddyListNewList(PurpleBuddyList* pList)
{
	g_print("%s - pList = 0x%x\n", __FUNCTION__, pList);
}

void CPurpleService::BuddyListNewNode(PurpleBlistNode* pNode)
{
	g_print("%s - pNode = 0x%x, pNode->type = %d\n", __FUNCTION__, pNode, pNode->type);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if ((PURPLE_BLIST_NODE_IS_BUDDY(pNode)) ||
		(PURPLE_BLIST_NODE_IS_GROUP(pNode)))
	{
		pNode->ui_data = CPurpleService::GetInstance();
	}
	else
	{
		pNode->ui_data = NULL;
	}
}

void CPurpleService::BuddyListShow(PurpleBuddyList* pList)
{
	g_print("%s - pList = 0x%x\n", __FUNCTION__, pList);
}

void CPurpleService::BuddyListUpdate(PurpleBuddyList* /*pList*/, PurpleBlistNode* pNode)
{
	//g_print("%s - pList = 0x%x, pNode = 0x%x, pNode->type = %d\n", __FUNCTION__, pList, pNode, pNode->type);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	CPurpleProtocol* pPurpleProtocol;

	PurpleBuddy* pBuddy;
	PurpleGroup* pGroup;
	GList* pListConn;
	PurpleConnection* pPurpleConn;

	switch (pNode->type)
	{
		case PURPLE_BLIST_GROUP_NODE:
			pGroup = (PurpleGroup*) pNode;

			if (pNode->ui_data)
			{
				pNode->ui_data = NULL;

				for (pListConn = purple_connections_get_all(); pListConn != NULL; pListConn = pListConn->next)
				{
					pPurpleConn = (PurpleConnection*) pListConn->data;

					pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pPurpleConn->account->protocol_id);

					if (pPurpleProtocol != NULL)
					{
						pPurpleProtocol->OnGroupAdded(pGroup);
					}
				}
			}
			break;

		case PURPLE_BLIST_CONTACT_NODE:
			break;

		case PURPLE_BLIST_BUDDY_NODE:
			pBuddy = (PurpleBuddy*) pNode;
			pGroup = purple_buddy_get_group(pBuddy);

			// Check if contact was already added
			if (pNode->ui_data)
			{
				pNode->ui_data = NULL;

				if (pBuddy->account != NULL)
				{
					pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pBuddy->account->protocol_id);

					if ((pPurpleProtocol != NULL) &&
						(pGroup != NULL))
					{
						pPurpleProtocol->OnContactListUpdate(pBuddy->account, pBuddy, pGroup);
					}
				}
			}
			break;

		case PURPLE_BLIST_CHAT_NODE:
			break;
		
		case PURPLE_BLIST_OTHER_NODE:
			return;
	}
}

void CPurpleService::BuddyListRemove(PurpleBuddyList* pList, PurpleBlistNode* pNode)
{
	g_print("%s - pList = 0x%x, pNode = 0x%x\n", __FUNCTION__, pList, pNode);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if (PURPLE_BLIST_NODE_IS_BUDDY(pNode))
	{
		PurpleBuddy* pBuddy = (PurpleBuddy*) pNode;
		PurpleGroup* pGroup = purple_buddy_get_group(pBuddy);

		if ((pBuddy->account != NULL) && (pGroup != NULL) &&
			(purple_account_is_connected(pBuddy->account)))
		{
			CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pBuddy->account->protocol_id);
			if (pPurpleProtocol != NULL)
			{
				pPurpleProtocol->OnContactRemoved(pBuddy->account, pBuddy, pGroup);
			}
		}
	}
}

void CPurpleService::BuddyListDestroy(PurpleBuddyList* pList)
{
	g_print("%s - pList = 0x%x\n", __FUNCTION__, pList);
}

void CPurpleService::BuddyListSetVisible(PurpleBuddyList* pList, gboolean bShow)
{
	g_print("%s - pList = 0x%x, show = %d\n", __FUNCTION__, pList, bShow);
}

void CPurpleService::BuddyListRequestAddBuddy(PurpleAccount* pAccount, const char* pszUsername,
						  const char* pszGroup, const char* pszAlias)
{
	g_print("%s - pAccount = 0x%x, pszUsername = %s, pszGroup = %s, pszAlias = %s\n",
			__FUNCTION__, pAccount, pszUsername, pszGroup, pszAlias);
}

void CPurpleService::BuddyListRequestAddChat(PurpleAccount* pAccount, PurpleGroup* pGroup, 
						 const char* pszAlias, const char* pszName)
{
	g_print("%s - pAccount = 0x%x, pGroup = 0x%x, pszAlias = %s, pszName = %s\n",
			__FUNCTION__, pAccount, pGroup, pszAlias, pszName);
}

void CPurpleService::BuddyListRequestAddGroup(void)
{
	g_print("%s\n", __FUNCTION__);
}


//
// Purple conversation callbacks
//
void CPurpleService::ConversationCreateConversation(PurpleConversation* /*pConv*/)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::ConversationDestroyConversation(PurpleConversation* /*pConv*/)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::ConversationWriteChat(PurpleConversation* pConv, const char* pszWho,
	                const char* pszMessage, PurpleMessageFlags flags,
	                time_t /*mtime*/)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}
	
	CPurpleProtocol* pPurpleProtocol = NULL;

	if ((pConv != NULL) && 
		(pConv->account != NULL))
	{
		pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pConv->account->protocol_id);

		if ((pPurpleProtocol != NULL) &&
			(pConv->name != NULL) &&
			(pszMessage != NULL))
		{
			int nChatId = purple_conv_chat_get_id(PURPLE_CONV_CHAT(pConv));

            if (flags & PURPLE_MESSAGE_SYSTEM)
			{
				// We don't expect System Messages on chat conversations, except
				// contact nudge send, but we disable this feature here because
				// libPurple doesn't handle nudge on chat.
				purple_debug_warning("CPurpleService::ConversationWriteChat", 
					"Received not expected System Message on chat: %s - %s.\n", pszWho, pszMessage);
			}
			else if (flags & PURPLE_MESSAGE_RECV)
			{
				pPurpleProtocol->OnChatReceived(pConv->account, nChatId, pszWho, pszMessage, m_strIMCustomEmoticon.c_str());
				m_strIMCustomEmoticon.clear();
			}
			else if (flags & PURPLE_MESSAGE_SEND)
			{
				pPurpleProtocol->OnChatSent(pConv->account, nChatId, pConv->name, pszMessage);
			}
		}
	}
}

void CPurpleService::ConversationWriteIm(PurpleConversation* pConv, const char* /*pszWho*/,
	                const char* pszMessage, PurpleMessageFlags flags,
	                time_t /*mtime*/)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}
	
	CPurpleProtocol* pPurpleProtocol = NULL;

	if ((pConv != NULL) && 
		(pConv->account != NULL))
	{
		pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pConv->account->protocol_id);

		if ((pPurpleProtocol != NULL) &&
			(pConv->name != NULL) &&
			(pszMessage != NULL))
		{
			if (flags & PURPLE_MESSAGE_SYSTEM)
			{
				MessageParam msgParam;
				msgParam.strFrom = pConv->name;
				msgParam.strMessage = pszMessage;
				
				pPurpleProtocol->ProcessSystemMessage(pConv->account, &msgParam);
			}
			else if (flags & PURPLE_MESSAGE_RECV)
			{
				if (!m_strIMCustomEmoticon.empty())
				{
					pPurpleProtocol->OnIMReceived(pConv->account, pConv->name, pszMessage, m_strIMCustomEmoticon.c_str());
					
					m_strIMCustomEmoticon.clear();
				}
				else
				{
					pPurpleProtocol->OnIMReceived(pConv->account, pConv->name, pszMessage);
				}

				pPurpleProtocol->OnContactTyping(pConv->account, pConv->name, PURPLE_NOT_TYPING);
			}
			else if (flags & PURPLE_MESSAGE_SEND)
			{
				pPurpleProtocol->OnIMSent(pConv->account, pConv->name, pszMessage);
			}
		}
	}
}

void CPurpleService::ConversationWriteConv(PurpleConversation* pConv, const char* /*pszName*/,
	                const char* /*pszAlias*/, const char* pszMessage, PurpleMessageFlags flags,
	                time_t /*mtime*/)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	CPurpleProtocol* pPurpleProtocol = NULL;

	if ((pConv != NULL) && 
		(pConv->account != NULL))
	{
		pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pConv->account->protocol_id);

		if ((pPurpleProtocol != NULL) &&
			(pConv->name != NULL) &&
			(pszMessage != NULL))
		{
			if (flags & PURPLE_MESSAGE_SYSTEM)
			{
				MessageParam msgParam;
				msgParam.strFrom = pConv->name;
				msgParam.strMessage = pszMessage;

				pPurpleProtocol->ProcessSystemMessage(pConv->account, &msgParam);
			}
			else if (flags & PURPLE_MESSAGE_ERROR)
			{
				MessageParam msgParam;
				msgParam.strFrom = pConv->name;
				msgParam.strMessage = pszMessage;
				msgParam.conversationType = purple_conversation_get_type(pConv);
				msgParam.iChatId = 0;

				if (msgParam.conversationType != PURPLE_CONV_TYPE_IM)
				{
					msgParam.iChatId = pConv->u.chat->id;
				}

				pPurpleProtocol->ProcessErrorMessage(pConv->account, &msgParam);
			}
		}
	}
}

void CPurpleService::ConversationChatAddUsers(PurpleConversation* pConv, GList* pUsers, gboolean new_arrivals)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}
	
    CPurpleProtocol* pPurpleProtocol = NULL;

	if ((pConv != NULL) && 
		(pConv->account != NULL))
	{
		pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pConv->account->protocol_id);

		if ((pPurpleProtocol != NULL) &&
			(pConv->name != NULL))
		{
			GList* pListUsers;

            for (pListUsers = pUsers; 
			    pListUsers != NULL;
			    pListUsers = pListUsers->next)
		    {
                PurpleConvChatBuddy* user = NULL;
                user = (PurpleConvChatBuddy *)pListUsers->data;

                if (user)
                {
                    char* pszUsername = user->name;
                    char* pszAccountUsername = pConv->account->username;

                    if (0 != strcmp(pszUsername, pszAccountUsername))
                    {
                        int nChatId = purple_conv_chat_get_id(PURPLE_CONV_CHAT(pConv));

                        pPurpleProtocol->OnChatAddUser(pConv->account, nChatId, user->name, user->alias, user->buddy);
                    }
                }
            }
		}
	}
}

void CPurpleService::ConversationChatRenameUser(PurpleConversation* pConv,
	                        const char* pszOldName, const char* pszNewName, 
							const char* pszNewAlias)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

    CPurpleProtocol* pPurpleProtocol = NULL;

	if ((pConv != NULL) && 
		(pConv->account != NULL))
	{
		pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pConv->account->protocol_id);

		if ((pPurpleProtocol != NULL) &&
			(pConv->name != NULL))
		{
            int nChatId = purple_conv_chat_get_id(PURPLE_CONV_CHAT(pConv));

            pPurpleProtocol->OnChatRenameUser(pConv->account, nChatId, pszOldName, pszNewName, pszNewAlias);
        }
    }	
}

void CPurpleService::ConversationChatRemoveUsers(PurpleConversation* pConv, GList* pUsers)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}
	
    PurpleConvChat *chat;
	chat = PURPLE_CONV_CHAT(pConv);
    CPurpleProtocol* pPurpleProtocol = NULL;

	if ((pConv != NULL) && 
		(pConv->account != NULL))
	{
		pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pConv->account->protocol_id);

		if ((pPurpleProtocol != NULL) &&
			(pConv->name != NULL))
		{
			GList* pListUsers;

            for (pListUsers = pUsers; 
			    pListUsers != NULL;
			    pListUsers = pListUsers->next)
		    {
                const char* pszUser = (char*)pListUsers->data;
                int nChatId = purple_conv_chat_get_id(PURPLE_CONV_CHAT(pConv));

                pPurpleProtocol->OnChatRemoveUser(pConv->account, nChatId, pszUser);
            }
        }
    }
}

void CPurpleService::ConversationChatUpdateUser(PurpleConversation* pConv, const char* pszUser)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

    PurpleConvChat *chat;
	chat = PURPLE_CONV_CHAT(pConv);
    CPurpleProtocol* pPurpleProtocol = NULL;

	if ((pConv != NULL) && 
		(pConv->account != NULL))
	{
		pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pConv->account->protocol_id);

		if ((pPurpleProtocol != NULL) &&
			(pConv->name != NULL))
		{
            PurpleConvChatBuddy* user = NULL;
            user = purple_conv_chat_cb_find(chat, pszUser);

            if (user)
            {
                int nChatId = purple_conv_chat_get_id(PURPLE_CONV_CHAT(pConv));

                pPurpleProtocol->OnChatUpdateUser(pConv->account, nChatId, user->name, user->alias, user->buddy);
            }
        }
    }
}

void CPurpleService::ConversationPresent(PurpleConversation* /*pConv*/)
{
	g_print("%s\n", __FUNCTION__);
}

gboolean CPurpleService::ConversationHasFocus(PurpleConversation* /*pConv*/)
{
	g_print("%s\n", __FUNCTION__);

	return FALSE;
}

gboolean CPurpleService::ConversationCustomSmileyAdd(PurpleConversation* pConv, 
													 const char* pszSmiley, 
													 gboolean remote)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return FALSE;
	}
	
	CPurpleProtocol* pPurpleProtocol = NULL;
	
	if ( (pConv != NULL) && (pConv->account != NULL) )
	{
		pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pConv->account->protocol_id);
		
		if ( (pPurpleProtocol != NULL) && (pConv->name != NULL) && (pszSmiley != NULL) )
		{
			string strFormatedSmiley = pPurpleProtocol->FormatReceivedMessage(pszSmiley);
			
			string strSmiley("");
			GetLocaleTextFromUTF8(strFormatedSmiley.c_str(), strSmiley);
			
			CustomEmoticonAddResult emoticonAddResult = 
					pPurpleProtocol->OnCustomSmileyAdd(pConv, strSmiley.c_str(), remote);
			
			if ( (emoticonAddResult != CUSTOM_EMOTICON_ADD_RESULT_ERROR) && 
					(emoticonAddResult != CUSTOM_EMOTICON_ADD_RESULT_NOT_ALLOWED) )
			{
				if (m_strIMCustomEmoticon.empty())
				{
					m_strIMCustomEmoticon = strSmiley;
				}
				else
				{
					m_strIMCustomEmoticon.append(" ");
					m_strIMCustomEmoticon.append(strSmiley);
				}
			}
			
			return (emoticonAddResult == CUSTOM_EMOTICON_ADD_RESULT_NEW);
		}
	}
	
	return FALSE;
}

void CPurpleService::ConversationCustomSmileyWrite(PurpleConversation* pConv, 
												   const char* pszSmiley, 
												   const guchar* pData, 
												   gsize size)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}
	
	CPurpleProtocol* pPurpleProtocol = NULL;
	
	if ( (pConv != NULL) && (pConv->account != NULL) )
	{
		pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pConv->account->protocol_id);
		
		if ( (pPurpleProtocol != NULL) && (pConv->name != NULL) && (pszSmiley != NULL) && 
				(pData != NULL) && (size != 0) )
		{
			string strFormatedSmiley = pPurpleProtocol->FormatReceivedMessage(pszSmiley);
			
			string strSmiley("");
			GetLocaleTextFromUTF8(strFormatedSmiley.c_str(), strSmiley);
			
			char* pszFileExtension = g_strdup(purple_util_get_image_extension(pData, size));
			
			pPurpleProtocol->OnCustomSmileyWrite(pConv, strSmiley.c_str(), pData, size, pszFileExtension);
			
			g_free(pszFileExtension);
		}
	}
}

void CPurpleService::ConversationCustomSmileyClose(PurpleConversation* pConv, 
												   const char* pszSmiley)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}
	
	CPurpleProtocol* pPurpleProtocol = NULL;
	
	if ( (pConv != NULL) && (pConv->account != NULL) )
	{
		pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pConv->account->protocol_id);
		
		if ( (pPurpleProtocol != NULL) && (pConv->name != NULL) && (pszSmiley != NULL) )
		{
			string strFormatedSmiley = pPurpleProtocol->FormatReceivedMessage(pszSmiley);
			
			string strSmiley("");
			GetLocaleTextFromUTF8(strFormatedSmiley.c_str(), strSmiley);
			
			pPurpleProtocol->OnCustomSmileyClose(pConv, strSmiley.c_str());
		}
	}
}

void CPurpleService::ConversationSendConfirm(PurpleConversation* /*pConv*/, 
										   const char* /*pszMessage*/)
{
	g_print("%s\n", __FUNCTION__);
}



//
// Purple conversation window callbacks
//
/* UOL <PURPLE Version 2.0.0 Adjustments> << */
/*
PurpleConversationUiOps*  CPurpleService::ConversationWindowGetConversationUIOps(void)
{
	g_print("%s\n", __FUNCTION__);

	return &m_conversationOps;
}

void CPurpleService::ConversationWindowNewWindow(PurpleConvWindow* /*pWin)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::ConversationWindowDestroyWindow(PurpleConvWindow* /*pWin)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::ConversationWindowShow(PurpleConvWindow* /*pWin)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::ConversationWindowHide(PurpleConvWindow* /*pWin)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::ConversationWindowRaise(PurpleConvWindow* /*pWin)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::ConversationWindowFlash(PurpleConvWindow* /*pWin)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::ConversationWindowSwitchConversation(PurpleConvWindow* /*pWin, unsigned int /*nIndex)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::ConversationWindowAddConversation(PurpleConvWindow* /*pWin, PurpleConversation* /*pConv)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::ConversationWindowRemoveConversation(PurpleConvWindow* /*pWin, PurpleConversation* /*pConv)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::ConversationWindowMoveConversation(PurpleConvWindow* /*pWin, 
													  PurpleConversation* /*pConv,
													  unsigned int /*nNewIndex)
{
	g_print("%s\n", __FUNCTION__);
}

int CPurpleService::ConversationWindowGetActiveIndex(const PurpleConvWindow* /*pWin)
{
	g_print("%s\n", __FUNCTION__);

	return NULL;
}

gboolean CPurpleService::ConversationWindowHasFocus(PurpleConvWindow* /*pWin)
{
	g_print("%s\n", __FUNCTION__);

	return FALSE;
}
*/
/* UOL >> */


//
// Purple notify callbacks
//
void*  CPurpleService::NotifyMessage(PurpleConnection *pPurpleConnection, PurpleNotifyMsgType type, 
								   const char* pszTitle, const char* pszPrimary, 
								   const char* pszSecondary)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return NULL;
	}

	NotifyMessageParam* pParam = new NotifyMessageParam();

	pParam->type = type;
	pParam->pPurpleConn = pPurpleConnection;
	pParam->strTitle = (pszTitle != NULL) ? pszTitle : "";
	pParam->strPrimary = (pszPrimary != NULL) ? pszPrimary : "";
	pParam->strSecondary = (pszSecondary != NULL) ? pszSecondary : "";

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) NotifyMessageThread, pParam);

	return (void*) CPurpleService::GetInstance()->GetPurpleServiceHandle();
}

void*  CPurpleService::NotifyEmail(PurpleConnection* /*pPurpleConnection*/, const char* /*pszSubject*/, 
						const char* /*pszFrom*/, const char* /*pszTo*/, const char* /*pszUrl*/)
{
	g_print("%s\n", __FUNCTION__);

	return NULL;
}

void*  CPurpleService::NotifyEmails(PurpleConnection *pPurpleConnection, size_t /*count*/, 
						gboolean /*detailed*/, const char** /*ppszSubjects*/, 
						const char** /*ppszFroms*/, const char** /*ppszTos*/, 
						const char** /*ppszUrls*/)
{
	g_print("%s\n", __FUNCTION__);

	return NULL;
}

void*  CPurpleService::NotifyFormatted(PurpleConnection *pPurpleConnection,
						const char* pszTitle, const char* pszPrimary,
						const char* pszSecondary, const char* pszText)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return NULL;
	}

	NotifyFormattedParam* pParam = new NotifyFormattedParam();

	pParam->pPurpleConn = pPurpleConnection;
	pParam->strTitle = (pszTitle != NULL) ? pszTitle : "";
	pParam->strPrimary = (pszPrimary != NULL) ? pszPrimary : "";
	pParam->strSecondary = (pszSecondary != NULL) ? pszSecondary : "";
	pParam->strText = (pszText != NULL) ? pszText : "";
	
	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) NotifyFormattedThread, pParam);

	return (void*) CPurpleService::GetInstance()->GetPurpleServiceHandle();
}

void*  CPurpleService::NotifySearchResults(PurpleConnection* pPurpleConnection, const char* pszTitle,
						const char* pszPrimary, const char* pszSecondary,
	                    PurpleNotifySearchResults* pResults, gpointer pUserData)
{
	g_print("%s\n", __FUNCTION__);
	
	return NULL;
}
void CPurpleService::NotifySearchResultsNewRows(PurpleConnection* pPurpleConnection, 
						PurpleNotifySearchResults* pResults,
						void* pData)
{
	g_print("%s\n", __FUNCTION__);
}

void*  CPurpleService::NotifyUserInfo(PurpleConnection* pPurpleConnection, const char* pszWho,
						PurpleNotifyUserInfo* pUserInfo)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return NULL;
	}
	
	NotifyUserInfoParam* pParam = new NotifyUserInfoParam();
	
	pParam->pPurpleConn = pPurpleConnection;
	pParam->strWho = (pszWho != NULL) ? pszWho : "";
	pParam->strUserInfo = purple_notify_user_info_get_text_with_newline(pUserInfo, "<br />");
	
	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) NotifyUserInfoThread, pParam);
	
	return (void*) CPurpleService::GetInstance()->GetPurpleServiceHandle();
}

void*  CPurpleService::NotifyURI(const char* /*pszUri*/)
{
	g_print("%s\n", __FUNCTION__);
	
	return NULL;
}

void CPurpleService::NotifyCloseNotify(PurpleNotifyType /*type*/, void* /*pszUIHandle*/)
{
	g_print("%s\n", __FUNCTION__);
}


//
// Purple request callbacks
//
void*  CPurpleService::RequestInput(const char *pszTitle, const char *pszPrimary,
								const char *pszSecondary, const char *pszDefaultValue,
								gboolean bMultiline, gboolean bMasked, gchar *pszHint,
								const char *pszOkText, GCallback OkCallback,
								const char *pszCancelText, GCallback CancelCallback,
								PurpleAccount* pAccount, const char* pszWho, 
								PurpleConversation* pConversation, 
								void *pUserData)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return NULL;
	}

	RequestInputParam* pParam = new RequestInputParam();

	pParam->pPurpleAccount = pAccount;
	pParam->strTitle = (pszTitle != NULL) ? pszTitle : "";
	pParam->strPrimary = (pszPrimary != NULL) ? pszPrimary : "";
	pParam->strSecondary = (pszSecondary != NULL) ? pszSecondary : "";
	pParam->strDefaultValue = (pszDefaultValue != NULL) ? pszDefaultValue : "";
	pParam->okCallback = OkCallback;
	pParam->cancelCallback = CancelCallback;
	pParam->strWho = pszWho;
	pParam->pConversation = pConversation;
	pParam->pUserData = pUserData;
	
	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc)  RequestInputThread, pParam);

	return (void*) CPurpleService::GetInstance()->GetPurpleServiceHandle();
}


void*  CPurpleService::RequestChoice(const char* /*pszTitle*/, const char* /*pszPrimary*/,
								const char* /*pszSecondary*/, int /*nDefaultValue*/,
								const char* /*pszOkText*/, GCallback /*OkCallback*/,
								const char* /*pszCancelText*/, GCallback /*CancelCallback*/,
								PurpleAccount* /*pAccount*/, const char* /*pszWho*/, 
								PurpleConversation* /*pConversation*/, 
								void* /*pUserData*/, va_list /*choices*/)
{
	g_print("%s\n", __FUNCTION__);

	return NULL;
}


void*  CPurpleService::RequestAction(PurpleConnection *pPurpleConnection, 
								const char* pszTitle, const char* pszPrimary,
								const char* pszSecondary, int nDefaultAction,
								PurpleAccount* pAccount, const char* pszWho, 
								PurpleConversation* pConversation, 
								void* pUserData, size_t nActionCount,
								va_list actions)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return NULL;
	}

	RequestActionParam* pParam = new RequestActionParam();

	pParam->pPurpleAccount = pAccount;
	pParam->pPurpleConnection = pPurpleConnection;
	pParam->strTitle = (pszTitle != NULL) ? pszTitle : "";
	pParam->strPrimary = (pszPrimary != NULL) ? pszPrimary : "";
	pParam->strSecondary = (pszSecondary != NULL) ? pszSecondary : "";
	pParam->nDefaultAction = nDefaultAction;
	pParam->strWho = (pszWho != NULL) ? pszWho : "";
	pParam->pConversation = pConversation;
	pParam->pUserData = pUserData;

	string strActionLabel;
	GCallback actionCallback;
	for (size_t nInd = 0; nInd < nActionCount; nInd++)
	{
		strActionLabel = va_arg(actions, char*);
		actionCallback = va_arg(actions, GCallback);

		pParam->vActionLabel.push_back(strActionLabel);
		pParam->vActionCallback.push_back(actionCallback);
	}

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) RequestActionThread, pParam);
	
	return (void*) CPurpleService::GetInstance()->GetPurpleServiceHandle();
}

void*  CPurpleService::RequestFields(const char* /*pszTitle*/, const char* /*pszPrimary*/,
								const char* /*pszSecondary*/, PurpleRequestFields* /*pFields*/,
								const char* /*pszOkText*/, GCallback /*OkCallback*/,
								const char* /*pszCancelText*/, GCallback /*CancelCallback*/,
								PurpleAccount* /*pAccount*/, const char* /*pszWho*/, 
								PurpleConversation* /*pConversation*/, 
								void* /*pUserData*/)
{
	g_print("%s\n", __FUNCTION__);
	
	return NULL;
}

void*  CPurpleService::RequestFile(PurpleConnection *pPurpleConnection, const char* pszTitle, 
								const char* pszFilename, gboolean bSaveDialog, 
								GCallback OkCallback, GCallback CancelCallback, 
								PurpleAccount* pAccount, const char* pszWho, 
								PurpleConversation* pConversation, void* pUserData)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return NULL;
	}

    if (CPurpleService::GetInstance()->IsWizardRunning())
	{
		return NULL;
	}
	
	RequestFileParam* pParam = new RequestFileParam();

	pParam->pPurpleAccount = pAccount;
	pParam->pPurpleConnection = pPurpleConnection;
	pParam->strTitle = (pszTitle != NULL) ? pszTitle : "";
	pParam->strFileName = (pszFilename != NULL) ? pszFilename : "";
	pParam->bSaveDialog = bSaveDialog;
	pParam->okCallback = OkCallback;
	pParam->cancelCallback = CancelCallback;
	pParam->strWho = pszWho;
	pParam->pConversation = pConversation;
	pParam->pUserData = pUserData;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) RequestFileThread, pParam);

	return (void*) CPurpleService::GetInstance()->GetPurpleServiceHandle();
}

void CPurpleService::RequestCloseRequest(PurpleRequestType /*type*/, void* /*pUIHandle*/)
{
	g_print("%s\n", __FUNCTION__);
}

void* CPurpleService::RequestFolder(const char *pszTitle, const char *pszDirName,
									GCallback OkCallback, GCallback CancelCallback,
									PurpleAccount* pAccount, const char* pszWho, 
									PurpleConversation* pConversation, void *pUserData)
{
	g_print("%s\n", __FUNCTION__);
	
	return NULL;
}


//
// Purple file transfer callbacks
//
void CPurpleService::FileTransferNewXfer(PurpleXfer* pXfer)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::FileTransferDestroy(PurpleXfer* pXfer)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if (pXfer != NULL)
	{
		PurpleXfer *pPurpleXfer = (PurpleXfer*) pXfer;

		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pPurpleXfer->account->protocol_id);
		
		if (pPurpleProtocol != NULL)
		{
            pPurpleProtocol->OnFileTransferDestroyed(pXfer);
		}
	}
}

void CPurpleService::FileTransferAddXfer(PurpleXfer* pXfer)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if (pXfer != NULL)
	{
		PurpleXfer *pPurpleXfer = (PurpleXfer*) pXfer;

		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pPurpleXfer->account->protocol_id);
		
		if (pPurpleProtocol != NULL)
		{
            pPurpleProtocol->OnFileTransferAdded(pXfer);
		}
	}
}

void CPurpleService::FileTransferUpdateProgress(PurpleXfer* pXfer, double dPercent)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if (pXfer != NULL)
	{
		PurpleXfer *pPurpleXfer = (PurpleXfer*) pXfer;

		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pPurpleXfer->account->protocol_id);
		
		if (pPurpleProtocol != NULL)
		{
            pPurpleProtocol->OnFileTransferUpdateProgress(pXfer, dPercent);
		}
	}
}

void CPurpleService::FileTransferCancelLocal(PurpleXfer* /*pXfer*/)
{
	g_print("%s\n", __FUNCTION__);
}

void CPurpleService::FileTransferCancelRemote(PurpleXfer* pXfer)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}
	
	if (pXfer != NULL)
	{
		PurpleXfer *pPurpleXfer = (PurpleXfer*) pXfer;

		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pPurpleXfer->account->protocol_id);
		
		if (pPurpleProtocol != NULL)
		{
            pPurpleProtocol->OnFileTransferCanceled(pXfer);
		}
	}
}


//
// Purple room list callbacks
//
//void CPurpleService::RoomListShowWithAccount(PurpleAccount* pAccount)
//{
//	g_print("%s\n", __FUNCTION__);
//}
//
//void CPurpleService::RoomListNew(PurpleRoomlist* pList)
//{
//	g_print("%s\n", __FUNCTION__);
//}
//
//void CPurpleService::RoomListSetFields(PurpleRoomlist* pList, GList* pFields)
//{
//	g_print("%s\n", __FUNCTION__);
//}
//
//void CPurpleService::RoomListAddRoom(PurpleRoomlist* pList, PurpleRoomlistRoom* pRoom)
//{
//	g_print("%s\n", __FUNCTION__);
//}
//
//void CPurpleService::RoomListInProgress(PurpleRoomlist* pList, gboolean bFlag)
//{
//	g_print("%s\n", __FUNCTION__);
//}
//
//void CPurpleService::RoomListDestroy(PurpleRoomlist* pList)
//{
//	g_print("%s\n", __FUNCTION__);
//}


//
// Purple privacy callbacks
//
void CPurpleService::PrivacyPermitAdded(PurpleAccount* pAccount, const char* pszName)
{
	g_print("%s\n", __FUNCTION__);
	
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if ((pAccount != NULL) &&
		(pszName != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pAccount->protocol_id);

		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->OnPermitAdded(pAccount, pszName);
		}
	}
}

void CPurpleService::PrivacyPermitRemoved(PurpleAccount* pAccount, const char* pszName)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}
	
	if ((pAccount != NULL) &&
		(pszName != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pAccount->protocol_id);

		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->OnPermitRemoved(pAccount, pszName);
		}
	}
}

void CPurpleService::PrivacyDenyAdded(PurpleAccount* pAccount, const char* pszName)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if ((pAccount != NULL) &&
		(pszName != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pAccount->protocol_id);

		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->OnDenyAdded(pAccount, pszName);
		}
	}
}

void CPurpleService::PrivacyDenyRemoved(PurpleAccount* pAccount, const char* pszName)
{
	g_print("%s\n", __FUNCTION__);

	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if ((pAccount != NULL) &&
		(pszName != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pAccount->protocol_id);

		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->OnDenyRemoved(pAccount, pszName);
		}
	}
}


//
// Purple buddy list signals callbacks
//
void CPurpleService::PurpleBuddySignedOnEventCallback(PurpleBuddy* pBuddy)
{
	g_print("%s - pBuddy->name = %s\n", __FUNCTION__, pBuddy->name);
	
	CPurpleProtocol* pPurpleProtocol = NULL;
	
	if ( (CPurpleService::GetInstance()->FinalizeInProgress()) || 
			((pPurpleProtocol = GetBuddyProtocol(pBuddy)) == NULL) )
	{
		return;
	}
	
	pPurpleProtocol->OnContactSignedOn(pBuddy->account, pBuddy);
}


void CPurpleService::PurpleBuddySignedOffEventCallback(PurpleBuddy* pBuddy)
{
	CPurpleProtocol* pPurpleProtocol = NULL;
	
	g_print("%s - pBuddy->name = %s\n", __FUNCTION__, pBuddy->name);
	
	if ( (CPurpleService::GetInstance()->FinalizeInProgress()) || 
			((pPurpleProtocol = GetBuddyProtocol(pBuddy)) == NULL) )
	{
		return;
	}
	
	pPurpleProtocol->OnContactSignedOff(pBuddy->account, pBuddy);
}


void CPurpleService::PurpleBuddyIdleChangedEventCallback(PurpleBuddy* pBuddy, 
		gboolean oldIdle, gboolean newIdle)
{
	CPurpleProtocol* pPurpleProtocol = NULL;
	
	g_print("%s - pBuddy->name = %s, oldIdle = %d, and newIdle = %d\n", 
			__FUNCTION__, pBuddy->name, oldIdle, newIdle);
	
	if ( (CPurpleService::GetInstance()->FinalizeInProgress()) || 
			((pPurpleProtocol = GetBuddyProtocol(pBuddy)) == NULL) )
	{
		return;
	}
	
	if (newIdle)
	{
        pPurpleProtocol->OnContactIdle(pBuddy->account, pBuddy);
	}
	else
	{
		pPurpleProtocol->OnContactIdleReturn(pBuddy->account, pBuddy);
	}
}


void CPurpleService::PurpleBuddyStatusChangeEventCallback(PurpleBuddy* pBuddy, 
		PurpleStatus* pOldStatus, PurpleStatus* pNewStatus)
{
	CPurpleProtocol* pPurpleProtocol = NULL;
	
	g_print("%s - pBuddy->name = %s, pOldStatus->title = %s, and pNewStatus->title = %s\n", 
			__FUNCTION__, pBuddy->name, 
			purple_status_get_id(pOldStatus) ? purple_status_get_id(pOldStatus) : "", 
			purple_status_get_id(pNewStatus) ? purple_status_get_id(pNewStatus) : "");
	
	if ( (CPurpleService::GetInstance()->FinalizeInProgress()) || 
			((pPurpleProtocol = GetBuddyProtocol(pBuddy)) == NULL) )
	{
		return;
	}
	
	pPurpleProtocol->OnContactStatusChanged(pBuddy->account, pBuddy);
}


void CPurpleService::PurpleBuddyIconChangedEventCallback(PurpleBuddy* pBuddy)
{
	CPurpleProtocol* pPurpleProtocol = NULL;
	
	g_print("%s - pBuddy->name = %s\n", __FUNCTION__, pBuddy->name);
	
	if ( (CPurpleService::GetInstance()->FinalizeInProgress()) || 
			((pPurpleProtocol = GetBuddyProtocol(pBuddy)) == NULL) )
	{
		return;
	}
	
	{
		PurpleBuddyIcon* pBuddyIcon = purple_buddy_get_icon(pBuddy);
		
		if (pBuddyIcon != NULL)
		{
			pPurpleProtocol->OnContactIconReceived(pBuddy->account, pBuddy, pBuddyIcon);
		}
	}
}


void CPurpleService::PurpleBlistNodeAliasedEventCallback(PurpleBuddy* pBuddy, 
		const char* pOldAlias)
{
	CPurpleProtocol* pPurpleProtocol = NULL;
	
	g_print("%s - pBuddy->name = %s, and pOldAlias = %s\n", __FUNCTION__, 
		pBuddy->name, pOldAlias ? pOldAlias : "");
	
	if ( (CPurpleService::GetInstance()->FinalizeInProgress()) || 
			((pPurpleProtocol = GetBuddyProtocol(pBuddy)) == NULL) )
	{
		return;
	}
	
	const char* pszAlias = (pBuddy->alias != NULL) ? pBuddy->alias : pBuddy->server_alias;
	
	// Notifies the update only whether the alias is changed.
	if ( ( (pOldAlias) && (!pszAlias) ) || 
			( (!pOldAlias) && (pszAlias) ) || 
			(::lstrcmpi(pOldAlias, pszAlias) != 0) )
	{
		pPurpleProtocol->OnContactAliasReceived(pBuddy->account, pBuddy, pszAlias);
	}
}


CPurpleProtocol* CPurpleService::GetBuddyProtocol(PurpleBuddy* pBuddy)
{
	CPurpleProtocol* pPurpleProtocol = NULL;
	
	if ((pBuddy != NULL) && 
			(pBuddy->account != NULL))
	{
		pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pBuddy->account->protocol_id);
	}
	
	return pPurpleProtocol;
}


PurpleAccount* CPurpleService::FindPurpleAccount(PurpleConnection* pConnection)
{
	PurpleAccount* pAccount = NULL;
	GList* pListConn;
	PurpleConnection* pTmpConn;

	for (pListConn = purple_connections_get_all(); pListConn != NULL; pListConn = pListConn->next)
	{
		pTmpConn = (PurpleConnection*) pListConn->data;
		if (pTmpConn == pConnection)
		{
			pAccount = pTmpConn->account;
			break;
		}
	}

	return pAccount;
}


// Purple account threads
int CPurpleService::AccountRequestAuthorizeThread(void* pAccountRequestAuthorizeData)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	AccountRequestAuthorizeParam* pParam = 
			(AccountRequestAuthorizeParam*) pAccountRequestAuthorizeData;
	
	if (NULL == pParam)
	{
		return -1;
	}
	
	CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(
			pParam->pPurpleAccount->protocol_id);
	
	if (pPurpleProtocol != NULL)
	{
		pPurpleProtocol->ProcessAccountRequestAuthorize(pParam);
	}
	
	return 0;
}


// Purple notify threads
int CPurpleService::NotifyMessageThread(void* pNotifyMessageData)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	NotifyMessageParam* pParam = (NotifyMessageParam*) pNotifyMessageData;

	if (NULL == pParam)
	{
		return -1;
	}

    PurpleAccount* pAccount = FindPurpleAccount(pParam->pPurpleConn);
	
	if (pAccount != NULL)
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pAccount->protocol_id);
		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->ProcessNotifyMessage(pAccount, pParam);
		}
	}

	delete pParam;

	return 0;
}


int CPurpleService::NotifyEmailThread(void* /*pNotifyEmailData*/)
{
	return 0;
}

int CPurpleService::NotifyEmailsThread(void* /*pNotifyEmailsData*/)
{
	return 0;
}


int CPurpleService::NotifyFormattedThread(void* pNotifyFormattedData)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	NotifyFormattedParam* pParam = (NotifyFormattedParam*) pNotifyFormattedData;

	if (NULL == pParam)
	{
		return -1;
	}

    PurpleAccount* pAccount = FindPurpleAccount(pParam->pPurpleConn);
	
	if (pAccount != NULL)
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pAccount->protocol_id);
		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->ProcessNotifyFormatted(pAccount, pParam);
		}
	}

	delete pParam;

	return 0;
}


int CPurpleService::NotifyUserInfoThread(void* pNotifyUserInfoData)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	NotifyUserInfoParam* pParam = (NotifyUserInfoParam*) pNotifyUserInfoData;
	
	if (NULL == pParam)
	{
		return -1;
	}
	
	if ( (pParam->pPurpleConn != NULL) &&
			(pParam->pPurpleConn->account != NULL) )
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pParam->pPurpleConn->account->protocol_id);
		
		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->ProcessNotifyUserInfo(pParam->pPurpleConn->account, pParam);
			
			purple_notify_close(PURPLE_NOTIFY_USERINFO, GetInstance()->GetPurpleServiceHandle());
		}
	}
	
	delete pParam;

	return 0;
}


int CPurpleService::NotifyURIThread(void* /*pNotifyURIData*/)
{
	return 0;
}



// Purple request threads
int CPurpleService::RequestInputThread(void* pRequestInputData)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	RequestInputParam* pParam = (RequestInputParam*) pRequestInputData;

	if (NULL == pParam)
	{
		return -1;
	}

	if (pParam->pPurpleAccount != NULL)
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(
				pParam->pPurpleAccount->protocol_id);
		
		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->ProcessRequestInput(pParam->pPurpleAccount, pParam);
		}
	}

	delete pParam;

	return 0;
}


int CPurpleService::RequestChoiceThread(void* /*pRequestChoiceData*/)
{
	return 0;
}


int CPurpleService::RequestActionThread(void* pRequestActionData)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	RequestActionParam* pParam = (RequestActionParam*) pRequestActionData;
	
	// Auto accept certificates from "talk.google.com" and "xmpp.uol.com.br"
	string strMsgUOL = "Accept certificate for xmpp.uol.com.br?";
	string strMsgGMail = "Accept certificate for talk.google.com?";
	if ((0 == pParam->strPrimary.compare(strMsgUOL)) || 
		(0 == pParam->strPrimary.compare(strMsgGMail)))
	{
		GCallback cbOK;
		cbOK = pParam->vActionCallback[0];
		
		PurpleRequestActionCb pfnRequestActionCallback = (PurpleRequestActionCb) cbOK;
		pfnRequestActionCallback(pParam->pUserData, 0);

		return 0;
	}

	if ( (NULL == pParam) || (NULL == pParam->pPurpleAccount) )
	{
		return -1;
	}

	
	// Validate file transfer
	string strMsg = "wants to send you";
	string::size_type indexFind = pParam->strPrimary.find(strMsg); 
	string::size_type npos = -1;

	CPurpleProtocol* pPurpleProtocol = NULL;
	PurpleXfer* pPurpleXfer = NULL;

	if (indexFind != npos)
	{
		pPurpleXfer = (PurpleXfer*) pParam->pPurpleConnection;
	}

	pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pParam->pPurpleAccount->protocol_id);
	
	if (pPurpleProtocol != NULL)
	{
		if (indexFind != npos)
		{
			pPurpleProtocol->ProcessRequestFileTransferAction(pPurpleXfer, pParam);
		}
		else
		{
			pPurpleProtocol->ProcessRequestAction(pParam->pPurpleAccount, pParam);
		}
	}

	delete pParam;

	return 0;
}


int CPurpleService::RequestFieldsThread(void* /*pRequestFieldsData*/)
{
	return 0;
}


int CPurpleService::RequestFileThread(void* pRequestFileData)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	RequestFileParam* pParam = (RequestFileParam*) pRequestFileData;

	if (pParam == NULL)
	{
		return -1;
	}

	PurpleXfer* pPurpleXfer = (PurpleXfer*) pParam->pPurpleConnection;

	if ((pPurpleXfer->account != NULL) &&
		(pPurpleXfer->account->protocol_id != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(
				pPurpleXfer->account->protocol_id);
		
		if (pPurpleProtocol != NULL)
		{
			pPurpleProtocol->ProcessRequestFile(pPurpleXfer, pParam);
		}
	}

	delete pParam;

	return 0;
}

int CPurpleService::StartAsyncOperationsThread()
{
	// Create async operations handler.
	//
	g_mutex_lock(m_pMutex);
		
		m_pAsyncOperationsExec = new CPurpleProtocolAsyncExec();
		
		if (m_pAsyncOperationsExec)
		{
			CPurpleService::GetInstance()->SetAsyncOperationsThreadId(::GetCurrentThreadId());
		}
		
	g_mutex_unlock(m_pMutex);
	
	// Start handling.
	//
	m_pAsyncOperationsExec->StartAsyncOperations();
	
	// If we get here, the application is being terminated, so...
	//
	g_mutex_lock(m_pMutex);
		
		delete m_pAsyncOperationsExec;
		m_pAsyncOperationsExec = NULL;
		
		CPurpleService::GetInstance()->SetAsyncOperationsThreadId(0);
		
	g_mutex_unlock(m_pMutex);
	
	return 0;
}


void CPurpleService::SetWizardRunnig(BOOL bRunning)
{
    m_bWizardRunning = bRunning;
}

BOOL CPurpleService::IsWizardRunning()
{
    return m_bWizardRunning;
}

DWORD CPurpleService::GetUIMThreadId()
{
	return m_dwUIMThreadId;
}

void CPurpleService::SetAsyncOperationsThreadId(DWORD dwAsyncOperationsThreadId)
{
	m_dwAsyncOperationsThreadId = dwAsyncOperationsThreadId;
}

DWORD CPurpleService::GetAsyncOperationsThreadId()
{
	return m_dwAsyncOperationsThreadId;
}

CPurpleProtocolAsyncExec* CPurpleService::GetAsyncOperationsExec()
{
	return m_pAsyncOperationsExec;
}

void CPurpleService::PurpleBuddyTypingEventCallback(PurpleAccount* pAccount, const char* pszWho)
{
    if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if ((pAccount != NULL) &&
		(pszWho != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pAccount->protocol_id);
		if (pPurpleProtocol != NULL)
		{
            pPurpleProtocol->OnContactTyping(pAccount, pszWho, PURPLE_TYPING);
        }
    }
}

void CPurpleService::PurpleBuddyTypingStoppedEventCallback(PurpleAccount* pAccount, const char* pszWho)
{
    if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return;
	}

	if ((pAccount != NULL) &&
		(pszWho != NULL))
	{
		CPurpleProtocol* pPurpleProtocol = CPurpleService::GetInstance()->GetProtocol(pAccount->protocol_id);
		if (pPurpleProtocol != NULL)
		{
            pPurpleProtocol->OnContactTyping(pAccount, pszWho, PURPLE_NOT_TYPING);
        }
    }
}

gboolean CPurpleService::UIMConnectionsFinalizerMethod(gpointer data)
{
	// Finalize protocols stacks and libpurple connections...
	//
	CPurpleProtocolMap::iterator itProtocols;
	
	CPurpleProtocolStack* pPurpleProtocolStack;	
	
	for (itProtocols = m_mapProtocols.begin(); 
			itProtocols != m_mapProtocols.end();
			itProtocols++)
	{
		pPurpleProtocolStack = itProtocols->second;
		
		pPurpleProtocolStack->Finalize();
		
		CIMProtocol* pProtocol = pPurpleProtocolStack->GetNextProtocol();
		
		while (pProtocol && pProtocol->GetNextProtocol())
		{
			pProtocol = pProtocol->GetNextProtocol();
		}
		
		pProtocol->Finalize();
	}
	
	m_uimConnectionsFinalizerEvent.Set();
	
	// Must return false, so the resource is removed.
	//
	return FALSE;
}

void CPurpleService::EnablePurpleDebugLog(BOOL bEnabled)
{
	purple_debug_set_enabled(bEnabled);
}


void CPurpleService::SetPurpleDebugLogger(IPurpleDebugLogger* pPurpleDebugLogger)
{
	m_pPurpleDebugLogger = pPurpleDebugLogger;
}