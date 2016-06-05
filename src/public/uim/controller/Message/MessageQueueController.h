#pragma once

#include <interfaces/IUOLMessengerContact.h>
#include "../../model/InstantMessage.h"
#include "../UOLMessengerContactTraits.h"


class CMessageQueueController
{
public:
	CMessageQueueController();
	~CMessageQueueController();

	void AddMessage(CInstantMessagePtr pInstantMessage);
	void RemoveMessages(IUOLMessengerContactPtr pContact, CAtlList<CInstantMessagePtr>& listMessages);
	void RemoveMessages(IUOLMessengerContactPtr pContact);
	UINT GetMessagesCount(IUOLMessengerContactPtr pContact);

private:
	typedef CAtlMap<IUOLMessengerContactPtr, CAtlList<CInstantMessagePtr>*, CUOLMessengerContactTraits> CMessageQueueMap;

	CMessageQueueMap	m_mapMessages;

	CComAutoCriticalSection		m_csMap;
};