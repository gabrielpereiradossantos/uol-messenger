#include "stdafx.h"
#include "MessageQueueController.h"


CMessageQueueController::CMessageQueueController()
{
}


CMessageQueueController::~CMessageQueueController()
{
	CAtlList<CInstantMessagePtr>* pListMessages;

	for (POSITION pos = m_mapMessages.GetStartPosition(); pos; )
	{
		pListMessages = m_mapMessages.GetNext(pos)->m_value;
		delete pListMessages;
	}
}


void CMessageQueueController::AddMessage(CInstantMessagePtr pInstantMessage)
{
	if (!pInstantMessage)
	{
		return;
	}

	IUOLMessengerContactPtr pContact = pInstantMessage->GetContact();

	ATLASSERT(pContact != NULL);

	CMessageQueueMap::CPair* pPair;
	CAtlList<CInstantMessagePtr>* pListMessages;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csMap);

	pPair = m_mapMessages.Lookup(pContact);

	if (!pPair)
	{
		pListMessages = new CAtlList<CInstantMessagePtr>;
		m_mapMessages.SetAt(pContact, pListMessages);
	}
	else
	{
		pListMessages = pPair->m_value;
	}
	
	pListMessages->AddTail(pInstantMessage);
}


void CMessageQueueController::RemoveMessages(IUOLMessengerContactPtr pContact, CAtlList<CInstantMessagePtr>& listMessages)
{
	if (!pContact)
	{
		return;
	}

	listMessages.RemoveAll();

	CMessageQueueMap::CPair* pPair;
	CAtlList<CInstantMessagePtr>* pListMessages;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csMap);

	pPair = m_mapMessages.Lookup(pContact);

	if (pPair != NULL)
	{
		pListMessages = pPair->m_value;

		ATLASSERT(pListMessages != NULL);

		listMessages.AddTailList(pListMessages);

		pListMessages->RemoveAll();

		delete pListMessages;

		m_mapMessages.RemoveKey(pContact);
	}
}


void CMessageQueueController::RemoveMessages(IUOLMessengerContactPtr pContact)
{
	if (!pContact)
	{
		return;
	}

	CMessageQueueMap::CPair* pPair;
	CAtlList<CInstantMessagePtr>* pListMessages;

	CComCritSecLock<CComAutoCriticalSection> lock(m_csMap);

	pPair = m_mapMessages.Lookup(pContact);

	if (pPair != NULL)
	{
		pListMessages = pPair->m_value;

		ATLASSERT(pListMessages != NULL);

		pListMessages->RemoveAll();

		delete pListMessages;

		m_mapMessages.RemoveKey(pContact);
	}
}


UINT CMessageQueueController::GetMessagesCount(IUOLMessengerContactPtr pContact)
{
	if (!pContact)
	{
		return 0;
	}

	CComCritSecLock<CComAutoCriticalSection> lock(m_csMap);

	CMessageQueueMap::CPair* pPair;
	CAtlList<CInstantMessagePtr>* pListMessages;

	pPair = m_mapMessages.Lookup(pContact);

	if (pPair != NULL)
	{
		pListMessages = pPair->m_value;

		return pListMessages->GetCount();
	}

	return 0;
}