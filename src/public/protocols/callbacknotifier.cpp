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
 
#include "callbacknotifier.h"
#include "purpleservice.h"
#include "purplefiletransferinfo.h"
#include "contactinfo.h"

using namespace std;


struct VoidParam
{
	CIMProtocolCallback* pCallback;
	string strUser;
	void (CIMProtocolCallback::*pfnHandler)(const string&);
};


struct Param1
{
	CIMProtocolCallback* pCallback;
	string strUser;
	string strParam1;
	void (CIMProtocolCallback::*pfnHandler)(const string&, const string&);
};


struct Param2
{
	CIMProtocolCallback* pCallback;
	string strUser;
	string strParam1;
	string strParam2;
	void (CIMProtocolCallback::*pfnHandler)(const string&, const string&, const string&);
};


struct Param3
{
	CIMProtocolCallback* pCallback;
	string strUser;
	string strParam1;
	string strParam2;
	string strParam3;
	void (CIMProtocolCallback::*pfnHandler)(const string&, const string&, const string&, const string&);
};


struct ConnectionErrorParam
{
	CIMProtocolCallback* pCallback;
	string strUser;
	ConnectionError error;
	string strMessage;
};


struct ContactListUpdateParam
{
	CIMProtocolCallback* pCallback;
	string strUser;
	string strContact;
	string strAlias;
	string strGroup;
};


struct IMSendErrorParam
{
	CIMProtocolCallback* pCallback;
	string strUser;
	string strContact;
	IMProtocolError sendError;
};


struct ContactIconParam
{
	CIMProtocolCallback* pCallback;
	string strUser;
	string strContact;
	const char* pIconData;
	int nLen;
};


struct TypingParam
{
	CIMProtocolCallback* pCallback;
	string strUser;
	string strContact;
	TypingState state;
};


//struct ContactActionParam
//{
//	CIMProtocolCallback* pCallback;
//  string strUser;
//	NotifyReason reason;
//	string strContact;
//};


//struct FileTransferParam
//{
//	CIMProtocolCallback* pCallback;
//	PurpleXfer* pXfer;
//	void (CIMProtocolCallback::*pfnHandler)(const string&, const CFileTransferInfo*);
//};



CCallbackNotifier::CCallbackNotifier()
{
}


CCallbackNotifier::~CCallbackNotifier()
{
}


void CCallbackNotifier::HandleOnConnected(CIMProtocolCallback* pCallback, const string& strUser)
{
	VoidParam* pParam = new VoidParam();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->pfnHandler = &CIMProtocolCallback::OnConnected;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleVoidParamThread, pParam);
}


void CCallbackNotifier::HandleOnDisconnected(CIMProtocolCallback* pCallback, const string& strUser)
{
	VoidParam* pParam = new VoidParam();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->pfnHandler = &CIMProtocolCallback::OnDisconnected;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleVoidParamThread, pParam);
}


void CCallbackNotifier::HandleOnReportDisconnect(CIMProtocolCallback* pCallback, const string& strUser, ConnectionError errorCode, const string& strMessage)
{
	ConnectionErrorParam* pParam = new ConnectionErrorParam();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->error = errorCode;
	pParam->strMessage = strMessage;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleConnectionErrorThread, pParam);
}


void CCallbackNotifier::HandleOnNotifyAdded(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->pfnHandler = &CIMProtocolCallback::OnNotifyAdded;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnContactListUpdate(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact, const std::string& strAlias, const std::string& strGroup)
{
	ContactListUpdateParam* pParam = new ContactListUpdateParam();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strContact = strContact;
	pParam->strAlias = strAlias;
	pParam->strGroup = strGroup;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleContactListUpdateThread, pParam);
}


void CCallbackNotifier::HandleOnContactRemoved(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact, const std::string& strGroup)
{
	Param2* pParam = new Param2();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->strParam2 = strGroup;
	pParam->pfnHandler = &CIMProtocolCallback::OnContactRemoved;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread2, pParam);
}


void CCallbackNotifier::HandleOnContactMoved(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact, const std::string& strOldGroup, const std::string& strNewGroup)
{
	Param3* pParam = new Param3();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->strParam2 = strOldGroup;
	pParam->strParam3 = strNewGroup;
	pParam->pfnHandler = &CIMProtocolCallback::OnContactMoved;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread3, pParam);
}


//void CCallbackNotifier::HandleOnContactGotInfo(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact, const std::string& strInfo)
//{
//	Param2* pParam = new Param2();
//	pParam->pCallback = pCallback;
//	pParam->strParam1 = strContact;
//	pParam->strParam2 = strInfo;
//	pParam->pfnHandler = &CIMProtocolCallback::OnContactGotInfo;
//
//	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread2, pParam);
//}


void CCallbackNotifier::HandleOnContactIconReceived(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact, const char* pIconData, int nLen)
{
	ContactIconParam* pParam = new ContactIconParam();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strContact = strContact;
	pParam->pIconData = pIconData;
	pParam->nLen = nLen;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleContactIconThread, pParam);
}


void CCallbackNotifier::HandleOnContactAliasReceived(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact, const std::string& strAlias)
{
	Param2* pParam = new Param2();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->strParam2 = strAlias;
	pParam->pfnHandler = &CIMProtocolCallback::OnContactAliasReceived;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread2, pParam);
}


void CCallbackNotifier::HandleOnGroupAdded(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strGroup)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strGroup;
	pParam->pfnHandler = &CIMProtocolCallback::OnGroupAdded;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnGroupRemoved(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strGroup)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strGroup;
	pParam->pfnHandler = &CIMProtocolCallback::OnGroupRemoved;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnGroupRenamed(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strOldGroup, const std::string& strNewGroup)
{
	Param2* pParam = new Param2();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strOldGroup;
	pParam->strParam2 = strNewGroup;
	pParam->pfnHandler = &CIMProtocolCallback::OnGroupRenamed;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread2, pParam);
}


void CCallbackNotifier::HandleOnIMReceived(CIMProtocolCallback* pCallback, const std::string& strToUser, const std::string& strFromUser, const std::string& strText)
{
	Param2* pParam = new Param2();
	pParam->pCallback = pCallback;
	pParam->strUser = strToUser;
	pParam->strParam1 = strFromUser;
	pParam->strParam2 = strText;
	pParam->pfnHandler = &CIMProtocolCallback::OnIMReceived;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread2, pParam);
}


void CCallbackNotifier::HandleOnIMSent(CIMProtocolCallback* pCallback, const string& strFromUser, const std::string& strToUser, const std::string& strText)
{
	Param2* pParam = new Param2();
	pParam->pCallback = pCallback;
	pParam->strUser = strFromUser;
	pParam->strParam1 = strToUser;
	pParam->strParam2 = strText;
	pParam->pfnHandler = &CIMProtocolCallback::OnIMSent;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread2, pParam);
}


void CCallbackNotifier::HandleOnIMSendError(CIMProtocolCallback* pCallback, const string& strFromUser, const std::string& strToUser, IMProtocolError sendError)
{
	IMSendErrorParam* pParam = new IMSendErrorParam();
	pParam->pCallback = pCallback;
	pParam->strUser = strFromUser;
	pParam->strContact = strToUser;
	pParam->sendError = sendError;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleIMSendErrorThread, pParam);
}


void CCallbackNotifier::HandleOnContactSignedOn(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->pfnHandler = &CIMProtocolCallback::OnContactSignedOn;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnContactSignedOff(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->pfnHandler = &CIMProtocolCallback::OnContactSignedOff;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnContactAway(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->pfnHandler = &CIMProtocolCallback::OnContactAway;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnContactAwayReturn(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->pfnHandler = &CIMProtocolCallback::OnContactAwayReturn;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnContactIdle(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->pfnHandler = &CIMProtocolCallback::OnContactIdle;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnContactIdleReturn(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->pfnHandler = &CIMProtocolCallback::OnContactIdleReturn;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnContactTyping(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact, TypingState state)
{
	TypingParam* pParam = new TypingParam();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strContact = strContact;
	pParam->state = state;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleTypingThread, pParam);
}


//void CCallbackNotifier::HandleOnConversationWindowClosed(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
//{
//	Param1* pParam = new Param1();
//	pParam->pCallback = pCallback;
//	pParam->strParam1 = strContact;
//	pParam->pfnHandler = &CIMProtocolCallback::OnConversationWindowClosed;
//
//	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
//}
//
//
//void CCallbackNotifier::HandleOnConversationTimeout(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
//{
//	Param1* pParam = new Param1();
//	pParam->pCallback = pCallback;
//	pParam->strParam1 = strContact;
//	pParam->pfnHandler = &CIMProtocolCallback::OnConversationTimeout;
//
//	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
//}


void CCallbackNotifier::HandleOnPermitAdded(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->pfnHandler = &CIMProtocolCallback::OnPermitAdded;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnDenyAdded(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->pfnHandler = &CIMProtocolCallback::OnDenyAdded;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnPermitRemoved(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->pfnHandler = &CIMProtocolCallback::OnPermitRemoved;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


void CCallbackNotifier::HandleOnDenyRemoved(CIMProtocolCallback* pCallback, const string& strUser, const std::string& strContact)
{
	Param1* pParam = new Param1();
	pParam->pCallback = pCallback;
	pParam->strUser = strUser;
	pParam->strParam1 = strContact;
	pParam->pfnHandler = &CIMProtocolCallback::OnDenyRemoved;

	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleParamThread1, pParam);
}


//RequestResult HandleOnRequestInput(CIMProtocolCallback* pCallback, const string& strUser, RequestReason reason, char* pszBuffer, int nBufferLength);

//RequestResult HandleOnRequestAddContact(CIMProtocolCallback* pCallback, const std::string& strContact);
//RequestResult HandleOnRequestAddUser(CIMProtocolCallback* pCallback, const std::string& strContact, const std::string& strAddReason);
//RequestResult HandleOnRequestSendAuthorization(CIMProtocolCallback* pCallback, const std::string& strContact);


//void CCallbackNotifier::HandleOnNotifyContactAction(CIMProtocolCallback* pCallback, const string& strUser, NotifyReason reason, const std::string& strContact)
//{
//	ContactActionParam* pParam = new ContactActionParam();
//	pParam->pCallback = pCallback;
//	pParam->reason = reason;
//	pParam->strContact = strContact;
//
//	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleContactActionThread, pParam);
//}


//RequestResult HandleOnRequestAcceptFile(CIMProtocolCallback* pCallback, const std::string& strContact, const std::string& strFilename);
//RequestResult HandleOnRequestFilePath(CIMProtocolCallback* pCallback, const std::string& strContact, char* pszFilePath, int nMaxPathLength);

//void CCallbackNotifier::HandleOnFileTransferInit(CIMProtocolCallback* pCallback, PurpleXfer* pXfer)
//{
//	FileTransferParam* pParam = new FileTransferParam();
//	pParam->pCallback = pCallback;
//	pParam->pXfer = pXfer;
//	pParam->pfnHandler = &CIMProtocolCallback::OnFileTransferInit;
//
//	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleFileTransferThread, pParam);
//}
//
//
//void CCallbackNotifier::HandleOnFileTransferUpdateProgress(CIMProtocolCallback* pCallback, PurpleXfer* pXfer)
//{
//	FileTransferParam* pParam = new FileTransferParam();
//	pParam->pCallback = pCallback;
//	pParam->pXfer = pXfer;
//	pParam->pfnHandler = &CIMProtocolCallback::OnFileTransferUpdateProgress;
//
//	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleFileTransferThread, pParam);
//}
//
//
//void CCallbackNotifier::HandleOnFileTransferCanceled(CIMProtocolCallback* pCallback, PurpleXfer* pXfer)
//{
//	FileTransferParam* pParam = new FileTransferParam();
//	pParam->pCallback = pCallback;
//	pParam->pXfer = pXfer;
//	pParam->pfnHandler = &CIMProtocolCallback::OnFileTransferCanceled;
//
//	CPurpleService::GetInstance()->StartWorkerThread((GThreadFunc) HandleFileTransferThread, pParam);
//}




int CCallbackNotifier::HandleVoidParamThread(void* pParam)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	VoidParam* pHandleParam = (VoidParam*) pParam;

	if (pHandleParam != NULL)
	{
		if ((pHandleParam->pCallback != NULL) &&
			(pHandleParam->pfnHandler != NULL))
		{
			((pHandleParam->pCallback)->*(pHandleParam->pfnHandler))(pHandleParam->strUser);
		}

		delete pHandleParam;
	}

	return 0;
}


int CCallbackNotifier::HandleParamThread1(void* pParam)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	Param1* pHandleParam = (Param1*) pParam;

	if (pHandleParam != NULL)
	{
		if ((pHandleParam->pCallback != NULL) &&
			(pHandleParam->pfnHandler != NULL))
		{
			((pHandleParam->pCallback)->*(pHandleParam->pfnHandler))(pHandleParam->strUser, pHandleParam->strParam1);
		}

		delete pHandleParam;
	}

	return 0;
}


int CCallbackNotifier::HandleParamThread2(void* pParam)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	Param2* pHandleParam = (Param2*) pParam;

	if (pHandleParam != NULL)
	{
		if ((pHandleParam->pCallback != NULL) &&
			(pHandleParam->pfnHandler != NULL))
		{
			((pHandleParam->pCallback)->*(pHandleParam->pfnHandler))(pHandleParam->strUser, pHandleParam->strParam1, pHandleParam->strParam2);
		}

		delete pHandleParam;
	}

	return 0;
}


int CCallbackNotifier::HandleParamThread3(void* pParam)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	Param3* pHandleParam = (Param3*) pParam;

	if (pHandleParam != NULL)
	{
		if ((pHandleParam->pCallback != NULL) &&
			(pHandleParam->pfnHandler != NULL))
		{
			((pHandleParam->pCallback)->*(pHandleParam->pfnHandler))(pHandleParam->strUser, pHandleParam->strParam1, pHandleParam->strParam2, pHandleParam->strParam3);
		}

		delete pHandleParam;
	}

	return 0;
}


int CCallbackNotifier::HandleConnectionErrorThread(void* pParam)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	ConnectionErrorParam* pHandleParam = (ConnectionErrorParam*) pParam;

	if (pHandleParam != NULL)
	{
		if (pHandleParam->pCallback != NULL)
		{
			(pHandleParam->pCallback)->OnReportDisconnect(pHandleParam->strUser, pHandleParam->error, pHandleParam->strMessage);
		}

		delete pHandleParam;
	}

	return 0;
}


int CCallbackNotifier::HandleContactListUpdateThread(void* pParam)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	ContactListUpdateParam* pHandleParam = (ContactListUpdateParam*) pParam;

	if (pHandleParam != NULL)
	{
		if (pHandleParam->pCallback != NULL)
		{
			CContactInfo contactInfo;
			contactInfo.SetUserName(pHandleParam->strContact);
			contactInfo.SetAlias(pHandleParam->strAlias);

			(pHandleParam->pCallback)->OnContactListUpdate(pHandleParam->strUser, &contactInfo, pHandleParam->strGroup);
		}

		delete pHandleParam;
	}

	return 0;
}


int CCallbackNotifier::HandleIMSendErrorThread(void* pParam)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	IMSendErrorParam* pHandleParam = (IMSendErrorParam*) pParam;

	if (pHandleParam != NULL)
	{
		if (pHandleParam->pCallback != NULL)
		{
			(pHandleParam->pCallback)->OnIMSendError(pHandleParam->strUser, pHandleParam->strContact, pHandleParam->sendError);
		}

		delete pHandleParam;
	}

	return 0;
}


int CCallbackNotifier::HandleContactIconThread(void* pParam)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	ContactIconParam* pHandleParam = (ContactIconParam*) pParam;

	if (pHandleParam != NULL)
	{
		if (pHandleParam->pCallback != NULL)
		{
			(pHandleParam->pCallback)->OnContactIconReceived(pHandleParam->strUser, pHandleParam->strContact, pHandleParam->pIconData, pHandleParam->nLen);
		}

		delete pHandleParam;
	}

	return 0;
}


int CCallbackNotifier::HandleTypingThread(void* pParam)
{
	if (CPurpleService::GetInstance()->FinalizeInProgress())
	{
		return 0;
	}
	
	TypingParam* pHandleParam = (TypingParam*) pParam;

	if (pHandleParam != NULL)
	{
		if (pHandleParam->pCallback != NULL)
		{
			(pHandleParam->pCallback)->OnContactTyping(pHandleParam->strUser, pHandleParam->strContact, pHandleParam->state);
		}

		delete pHandleParam;
	}

	return 0;
}


//int CCallbackNotifier::HandleContactActionThread(void* pParam)
//{
//	if (CPurpleService::GetInstance()->FinalizeInProgress())
//	{
//		return 0;
//	}
//	
//	ContactActionParam* pHandleParam = (ContactActionParam*) pParam;
//
//	if (pHandleParam != NULL)
//	{
//		if (pHandleParam->pCallback != NULL)
//		{
//			(pHandleParam->pCallback)->OnNotifyContactAction(pHandleParam->reason, pHandleParam->strContact);
//		}
//
//		delete pHandleParam;
//	}
//
//	return 0;
//}


//int CCallbackNotifier::HandleFileTransferThread(void* pParam)
//{
//	if (CPurpleService::GetInstance()->FinalizeInProgress())
//	{
//		return 0;
//	}
//	
//	FileTransferParam* pHandleParam = (FileTransferParam*) pParam;
//
//	if (pHandleParam != NULL)
//	{
//		if ((pHandleParam->pCallback != NULL) &&
//			(pHandleParam->pfnHandler != NULL) &&
//			(pHandleParam->pXfer != NULL) &&
//			(pHandleParam->pXfer->who != NULL))
//		{
//			CPurpleFileTransferInfo pFileTransferInfo(pHandleParam->pXfer);
//
//			((pHandleParam->pCallback)->*(pHandleParam->pfnHandler))(pHandleParam->pXfer->who, &pFileTransferInfo);
//		}
//
//		delete pHandleParam;
//	}
//
//	return 0;
//}