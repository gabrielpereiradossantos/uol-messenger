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
 
#ifndef _PURPLE_SERVICE_H_
#define _PURPLE_SERVICE_H_

#include <stdlib.h>
#include <list>
#include <map>
#include <vector>

#include <glib.h>

#include "purpleconsts.h"

// Include Purple headers
#include <core.h>
#include <eventloop.h>
#include <debug.h>
#include <connection.h>
#include <blist.h>
#include <conversation.h>
#include <request.h>
#include <notify.h>
#include <ft.h>
//#include <roomlist.h>
#include <privacy.h>

#include "improtocol.h"
#include "purpleprotocol.h"
#include "purpleprotocolstack.h"

#include "iimprotocolstack.h"

#include "PurpleProtocolAsyncExec.h"
#include "PurpleServiceCallback.h"
#include "IPurpleDebugLogger.h"

struct strTraits
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

typedef std::map<const char*, CPurpleProtocolStack*, strTraits>  CPurpleProtocolMap;

typedef std::list<AccountRequestAuthorizeParam*> CAccountRequestParamList;



class CPurpleService : public IIMProtocolStack
{
public:
	int Init(char* pszCurrentProfileFullPath, DWORD dwUIMThreadId);
	int Finalize();
    void PreFinalize();

	int IsRunning();

	void SetPluginsSearchPath(int nCount, char** ppszPaths);

	void GetProtocolsList(std::list<CIMProtocol*>& listProtocols);	
	CPurpleProtocolStack* GetProtocolStack(const char* pszId);
	CPurpleProtocol* GetProtocol(const char* pszId);	

	virtual void AddProtocolToStack(const char* pszId, CIMProtocol* pProtocol, CIMProtocolCallback* pCallback);
	virtual void RemoveProtocolFromStack(const char* pszId, CIMProtocol* pProtocol, CIMProtocolCallback* pCallback);

	int FinalizeInProgress();
	void SetFinalizeInProgress(int bIsFinalizing);
	
	int PreFinalizeDone();
	
	void* GetPurpleServiceHandle();	
	void StartWorkerThread(GThreadFunc pfnThreadFunction, void* pParam);

    void SetWizardRunnig(BOOL bRunning);
    BOOL IsWizardRunning();
	
	DWORD GetUIMThreadId();
	
	void SetAsyncOperationsThreadId(DWORD dwAsyncOperationsThreadId);
	DWORD GetAsyncOperationsThreadId();
	
	CPurpleProtocolAsyncExec* GetAsyncOperationsExec();

	void EnablePurpleDebugLog(BOOL bEnabled);
	void SetPurpleDebugLogger(IPurpleDebugLogger* pPurpleDebugLogger);
    
public:
	static CPurpleService* GetInstance();

private:
	CPurpleService();
	virtual ~CPurpleService();

	int Run();

	void ConnectPurpleSignals();
	void DisconnectPurpleSignals();
	void* GetSignalHandle();

private:
	static int PurpleServiceThread(void* pData);

	static void GetStatusText(std::string& strStatusText, PurpleBuddy* pBuddy);

	// Purple core callbacks
	static void CoreUIPrefsInit();
	static void CoreDebugUIInit();
	static void CoreUIInit();
	static void CoreQuit();

	// Purple event loop callbacks
	static guint	EventLoopTimeoutAdd(guint nInterval, GSourceFunc function, gpointer pData);
	static gboolean	EventLoopTimeoutRemove(guint nHandle);
	static guint	EventLoopInputAdd(int nFd, PurpleInputCondition cond, PurpleInputFunction func, gpointer pData);
	static gboolean	EventLoopInputRemove(guint nHandle);

	// Purple debug callback
	static void DebugPrint(PurpleDebugLevel level, const char* pszCategory,
							  const char* pszArgs);

	// Purple connection callbacks
	static void ConnectionConnectProgress(PurpleConnection* pPurpleConn, const char* pszText,
								   size_t nStep, size_t nStepCount);
	static void ConnectionConnected(PurpleConnection* pPurpleConn);
	static void ConnectionDisconnected(PurpleConnection* pPurpleConn);
	static void ConnectionNotice(PurpleConnection* pPurpleConn, const char* pszText);
	static void ConnectionReportDisconnect(PurpleConnection* pPurpleConn, const char* pszText);
	static void ConnectionNetworkConnected(void);
	static void ConnectionNetworkDisconnected(void);

	// Purple account callbacks
	static void AccountNotifyAdded(PurpleAccount* pAccount, const char* pszRemoteUser,
						const char* pszId, const char* pszAlias,
						const char* pszMessage);
	/* UOL <PURPLE Version 2.0.0 Adjustments> << */
	static void AccountStatusChanged(PurpleAccount* pAccount, PurpleStatus* pStatus);
	static void AccountRequestAdd(PurpleAccount* pAccount, const char* pszRemoteUser,
						const char* pszId, const char* pszAlias,
						const char* pszMessage);
	static void *AccountRequestAuthorize(PurpleAccount* pAccount, const char* pszRemoteUser, 
						const char* pszId, const char* pszAlias, const char* pszMessage, 
						gboolean bOnList, PurpleAccountRequestAuthorizationCb pAuthorizeCallback, 
						PurpleAccountRequestAuthorizationCb pDenyCallback, void* pUserData);
	/*
	static void AccountAddCustomSmiley(PurpleAccount *pAccount, const char *pszSmile, 
						const char *pszFile);
	static void AccountDelCustomSmiley(PurpleAccount *pAccount, const char *pszSmile);
	*/
	/* UOL >> */
	static void AccountRequestAuthorizeClose(void *ui_handle);

	// Purple buddy list callbacks
	static void BuddyListNewList(PurpleBuddyList* pList);
	static void BuddyListNewNode(PurpleBlistNode* pNode);
	static void BuddyListShow(PurpleBuddyList* pList);
	static void BuddyListUpdate(PurpleBuddyList* pList, PurpleBlistNode* pNode);
	static void BuddyListRemove(PurpleBuddyList* pList, PurpleBlistNode* pNode);
	static void BuddyListDestroy(PurpleBuddyList* pList);
	static void BuddyListSetVisible(PurpleBuddyList* pList, gboolean bShow);
	static void BuddyListRequestAddBuddy(PurpleAccount* pAccount, const char* pszUsername,
							  const char* pszGroup, const char* pszAlias);
	static void BuddyListRequestAddChat(PurpleAccount* pAccount, PurpleGroup* pGroup, 
							 const char* pszAlias, const char* pszName);
	static void BuddyListRequestAddGroup(void);

	// Purple conversation callbacks
	static void ConversationCreateConversation(PurpleConversation* pConv);
	static void ConversationDestroyConversation(PurpleConversation* pConv);
	static void ConversationWriteChat(PurpleConversation* pConv, const char* pszWho,
	                   const char* pszMessage, PurpleMessageFlags flags,
	                   time_t mtime);
	static void ConversationWriteIm(PurpleConversation* pConv, const char* pszWho,
	                 const char* pszMessage, PurpleMessageFlags flags,
	                 time_t mtime);
	static void ConversationWriteConv(PurpleConversation* pConv, const char* pszName,
	                   const char* pszAlias, const char* pszMessage, PurpleMessageFlags flags,
	                   time_t mtime);
	static void ConversationChatAddUsers(PurpleConversation* pConv, GList* pUsers, gboolean new_arrivals);
	static void ConversationChatRenameUser(PurpleConversation* pConv,
	                         const char* pszOldName, const char* pszNewName, 
							 const char* pszNewAlias);
	static void ConversationChatRemoveUsers(PurpleConversation* pConv, GList* pUsers);
	static void ConversationChatUpdateUser(PurpleConversation* pConv, const char* pszUser);
	static void ConversationPresent(PurpleConversation* pConv);
	static gboolean ConversationHasFocus(PurpleConversation* pConv);
	static gboolean ConversationCustomSmileyAdd(PurpleConversation* pConv, const char* pszSmiley, 
							   gboolean remote);
	static void ConversationCustomSmileyWrite(PurpleConversation* pConv, const char* pszSmiley, 
								const guchar* pData, gsize size);
	static void ConversationCustomSmileyClose(PurpleConversation* pConv, const char* pszSmiley);
	static void ConversationSendConfirm(PurpleConversation* pConv, const char* pszMessage);

	// Purple conversation window callbacks
	/* UOL <PURPLE Version 2.0.0 Adjustments> << */
	/*
	static PurpleConversationUiOps*  ConversationWindowGetConversationUIOps(void);
	static void ConversationWindowNewWindow(PurpleConvWindow* pWin);
	static void ConversationWindowDestroyWindow(PurpleConvWindow* pWin);
	static void ConversationWindowShow(PurpleConvWindow* pWin);
	static void ConversationWindowHide(PurpleConvWindow* pWin);
	static void ConversationWindowRaise(PurpleConvWindow* pWin);
	static void ConversationWindowFlash(PurpleConvWindow* pWin);
	static void ConversationWindowSwitchConversation(PurpleConvWindow* pWin, unsigned int nIdex);
	static void ConversationWindowAddConversation(PurpleConvWindow* pWin, PurpleConversation* pConv);
	static void ConversationWindowRemoveConversation(PurpleConvWindow* pWin, PurpleConversation* pConv);
	static void ConversationWindowMoveConversation(PurpleConvWindow* pWin, PurpleConversation* pConv,
	                          unsigned int nNewIndex);
	static int ConversationWindowGetActiveIndex(const PurpleConvWindow* pWin);
	static gboolean ConversationWindowHasFocus(PurpleConvWindow* pWin);
	*/
	/* UOL >> */

	// Purple notify callbacks
	static void*  NotifyMessage(PurpleConnection *pPurpleConnection, PurpleNotifyMsgType type, 
							const char* pszTitle, const char* pszPrimary, 
							const char* pszSecondary);
	static void*  NotifyEmail(PurpleConnection *pPurpleConnection, const char* pszSubject, 
							const char* pszFrom, const char* pszTo, const char* pszUrl);
	static void*  NotifyEmails(PurpleConnection *pPurpleConnection, size_t count, gboolean detailed,
							const char** ppszSubjects, const char** ppszFroms,
							const char** ppszTos, const char** ppszUrls);
	static void*  NotifyFormatted(PurpleConnection *pPurpleConnection,
							const char* pszTitle, const char* pszPrimary,
							const char* pszSecondary, const char* pszText);
	static void*  NotifySearchResults(PurpleConnection* pPurpleConnection, const char* pszTitle,
							const char* pszPrimary, const char* pszSecondary,
	                        PurpleNotifySearchResults* pResults, gpointer pUserData);
	static void   NotifySearchResultsNewRows(PurpleConnection* pPurpleConnection, 
							PurpleNotifySearchResults* pResults,
							void* pData);
	static void*  NotifyUserInfo(PurpleConnection* pPurpleConnection, const char* pszWho,
							PurpleNotifyUserInfo* pUserInfo);
	static void*  NotifyURI(const char* pszUri);
	static void   NotifyCloseNotify(PurpleNotifyType type, void* pUIHandle);

	// Purple request callbacks
	static void*  RequestInput(const char *pszTitle, const char *pszPrimary,
								const char *pszSecondary, const char *pszDefaultValue,
								gboolean bMultiline, gboolean bMasked, gchar *pszHint,
								const char *pszOkText, GCallback OkCallback,
								const char *pszCancelText, GCallback CancelCallback,
								PurpleAccount* pAccount, const char* pszWho, 
								PurpleConversation* pConversation, void *pUserData);
	static void*  RequestChoice(const char* /*pszTitle*/, const char* /*pszPrimary*/,
								const char* /*pszSecondary*/, int /*nDefaultValue*/,
								const char* /*pszOkText*/, GCallback /*OkCallback*/,
								const char* /*pszCancelText*/, GCallback /*CancelCallback*/,
								PurpleAccount* /*pAccount*/, const char* /*pszWho*/, 
								PurpleConversation* /*pConversation*/, 
								void* /*pUserData*/, va_list /*choices*/);
	static void*  RequestAction(PurpleConnection *pPurpleConnection, const char* pszTitle,
								const char* pszPrimary, const char* pszSecondary, 
								int nDefaultAction, PurpleAccount* pAccount, 
								const char* pszWho, PurpleConversation* pConversation, 
								void* pUserData, size_t nActionCount,
								va_list actions);
	static void*  RequestFields(const char* /*pszTitle*/, const char* /*pszPrimary*/,
								const char* /*pszSecondary*/, PurpleRequestFields* /*pFields*/,
								const char* /*pszOkText*/, GCallback /*OkCallback*/,
								const char* /*pszCancelText*/, GCallback /*CancelCallback*/,
								PurpleAccount* /*pAccount*/, const char* /*pszWho*/, 
								PurpleConversation* /*pConversation*/, 
								void* /*pUserData*/);
	static void*  RequestFile(PurpleConnection *pPurpleConnection, const char* pszTitle, 
								const char* pszFilename, gboolean bSaveDialog, 
								GCallback OkCallback, GCallback CancelCallback, 
								PurpleAccount* pAccount, const char* pszWho, 
								PurpleConversation* pConversation, void* pUserData);
	static void   RequestCloseRequest(PurpleRequestType /*type*/, void* /*pUIHandle*/);
	static void*  RequestFolder(const char *pszTitle, const char *pszDirName,
								GCallback OkCallback, GCallback CancelCallback,
								PurpleAccount* pAccount, const char* pszWho, 
								PurpleConversation* pConversation, 
								void *pUserData);

	// Purple file transfer callbacks
	static void FileTransferNewXfer(PurpleXfer* pXfer);
	static void FileTransferDestroy(PurpleXfer* pXfer);
	static void FileTransferAddXfer(PurpleXfer* pXfer);
	static void FileTransferUpdateProgress(PurpleXfer* pXfer, double dPercent);
	static void FileTransferCancelLocal(PurpleXfer* pXfer);
	static void FileTransferCancelRemote(PurpleXfer* pXfer);

	// Purple room list callbacks
	//static void RoomListShowWithAccount(PurpleAccount* pAccount);
	//static void RoomListNew(PurpleRoomlist* pList);
	//static void RoomListSetFields(PurpleRoomlist* pList, GList* pFields);
	//static void RoomListAddRoom(PurpleRoomlist* pList, PurpleRoomlistRoom* pRoom);
	//static void RoomListInProgress(PurpleRoomlist* pList, gboolean bFlag);
	//static void RoomListDestroy(PurpleRoomlist* pList);

	// Purple privacy callbacks
	static void PrivacyPermitAdded(PurpleAccount* pAccount, const char* pszName);
	static void PrivacyPermitRemoved(PurpleAccount* pAccount, const char* pszName);
	static void PrivacyDenyAdded(PurpleAccount* pAccount, const char* pszName);
	static void PrivacyDenyRemoved(PurpleAccount* pAccount, const char* pszName);

	// Purple buddy list signals callbacks
	static void PurpleBuddySignedOnEventCallback(PurpleBuddy* pBuddy);
	static void PurpleBuddySignedOffEventCallback(PurpleBuddy* pBuddy);
	static void PurpleBuddyStatusChangeEventCallback(PurpleBuddy* pBuddy, 
			PurpleStatus* pOldStatus, PurpleStatus* pNewStatus);
	static void PurpleBuddyIdleChangedEventCallback(PurpleBuddy* pBuddy, 
			gboolean oldIdle, gboolean newIdle);
	static void PurpleBuddyIconChangedEventCallback(PurpleBuddy* pBuddy);
	static void PurpleBlistNodeAliasedEventCallback(PurpleBuddy* pBuddy, 
			const char* pOldAlias);
    static void PurpleBuddyTypingEventCallback(PurpleAccount* pAccount, const char* pszWho);
    static void PurpleBuddyTypingStoppedEventCallback(PurpleAccount* pAccount, const char* pszWho);
	
	static CPurpleProtocol* GetBuddyProtocol(PurpleBuddy* pBuddy);
	
	static PurpleAccount* FindPurpleAccount(PurpleConnection* pConnection);
	
	
	// Purple account threads
	static int AccountRequestAuthorizeThread(void* pAccountRequestAuthorizeData);
	
	// Purple notify threads
	static int NotifyMessageThread(void* pNotifyMessageData);
	static int NotifyEmailThread(void* pNotifyEmailData);
	static int NotifyEmailsThread(void* pNotifyEmailsData);
	static int NotifyFormattedThread(void* pNotifyFormattedData);
	static int NotifyUserInfoThread(void* pNotifyUserInfoData);
	static int NotifyURIThread(void* pNotifyURIData);

	// Purple request threads
	static int RequestInputThread(void* pRequestInputData);
	static int RequestChoiceThread(void* pRequestChoiceData);
	static int RequestActionThread(void* pRequestActionData);
	static int RequestFieldsThread(void* pRequestFieldsData);
	static int RequestFileThread(void* pRequestFileData);
	
	// Async Operations thread.
	static int StartAsyncOperationsThread();
	
	static gboolean UIMConnectionsFinalizerMethod(gpointer data);
	
private:
	static const char* UIM_PURPLE_CORE;
	static const long PURPLE_SERVICE_SIGNAL;
	static const long PURPLE_SERVICE_HANDLE;

	static PurpleEventLoopUiOps m_eventLoopOps;
	static PurpleCoreUiOps m_coreOps;
	static PurpleDebugUiOps m_debugOps;
	static PurpleConnectionUiOps m_connectionOps;
	static PurpleAccountUiOps m_accountOps;
	static PurpleBlistUiOps m_buddyListOps;
	static PurpleConversationUiOps m_conversationOps;
	static PurpleNotifyUiOps m_notifyOps;
	static PurpleRequestUiOps m_requestOps;
	static PurpleXferUiOps m_fileTransferOps;
	//static PurpleRoomlistUiOps m_roomlistOps;
	static PurplePrivacyUiOps m_privacyOps;

private:
	typedef std::vector<GThread*> CThreadVector;

	GMainLoop*				m_pMainLoop;
	GThread*				m_pPurpleServiceThread;
	static GMutex*			m_pMutex;
	long					m_nSignalHandle;
	static int				m_nFinalizeInProgress;
	long					m_nServiceHandle;
	static int				m_nPreFinalizeDone;

	static CPurpleProtocolMap		m_mapProtocols;

	CThreadVector			m_vectorThreads;

    BOOL                    m_bWizardRunning;
	
	static std::string		m_strIMCustomEmoticon;
	
	DWORD					m_dwUIMThreadId;

	static CPurpleProtocolAsyncExec*	m_pAsyncOperationsExec;
	DWORD					m_dwAsyncOperationsThreadId;
	
	static BOOL				m_bEventsInitialized;
	static CEvent			m_uimConnectionsFinalizerEvent;
	
	static CAccountRequestParamList		m_accountRequestParamList;

	static IPurpleDebugLogger*		m_pPurpleDebugLogger;
};

#endif // _PURPLE_SERVICE_H_
