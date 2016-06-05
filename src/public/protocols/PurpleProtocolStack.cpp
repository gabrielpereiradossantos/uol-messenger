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


#include "./purpleprotocolstack.h"

#include "./callbackselector.h"


using namespace std;


CPurpleProtocolStack::CPurpleProtocolStack()
{
#ifdef WIN32
	::ZeroMemory(&m_cs, sizeof(CRITICAL_SECTION));
	::InitializeCriticalSection(&m_cs);
#endif

	if (NULL == GetPreviousCallback())
	{
		CCallbackSelector* pSelector = new CCallbackSelector(this);
		SetPreviousCallback(pSelector);
	}
}

CPurpleProtocolStack::~CPurpleProtocolStack(void)
{
	CIMProtocolCallback* pPrev = GetPreviousCallback();

	if (pPrev)
	{
		SetPreviousCallback(NULL);
		delete pPrev;
	}

	//CCallbackMap::iterator it;
	//string* pStrUser;
	//for (it = m_mapExternalCallbacks.begin(); 
	//	 it != m_mapExternalCallbacks.end(); 
	//	 ++it)
	//{
	//	pStrUser = it->first;
	//	delete pStrUser;

	//	//delete it->second;
	//}

#ifdef WIN32
	::DeleteCriticalSection(&m_cs);
#endif
}

void CPurpleProtocolStack::AddProtocol(CIMProtocol* pProtocol, CIMProtocolCallback* pCallback)
{
	__try
	{
#ifdef WIN32
		::EnterCriticalSection(&m_cs);
#endif

		if (pProtocol)
		{
			CIMProtocol* pCurrent = this;

			if (pCurrent->GetNextProtocol())
			{	
				CIMProtocol* pPrev = this;

				while (TRUE) 
				{
					if (pCurrent->GetProtocolPos() >= pProtocol->GetProtocolPos())
					{
						break;
					}

					pPrev = pCurrent;
					pCurrent = pCurrent->GetNextProtocol();
				}

				pProtocol->SetNextProtocol(pPrev->GetNextProtocol());
				pPrev->SetNextProtocol(pProtocol);
			}
			else
			{
				pCurrent->SetNextProtocol(pProtocol);
			}	
		}

		if (pCallback)
		{
			CIMProtocolCallback* pCurrent = this;		

			if (pCurrent->GetPreviousCallback())
			{	
				CIMProtocolCallback* pNext = this;

				while (TRUE) 
				{
					if (pCurrent->GetCallbackPos() <= pCallback->GetCallbackPos())
					{
						break;	
					}

					pNext = pCurrent;
					pCurrent = pCurrent->GetPreviousCallback();
				}

				pCallback->SetPreviousCallback(pNext->GetPreviousCallback());
				pNext->SetPreviousCallback(pCallback);
			}
			else
			{
				pCurrent->SetPreviousCallback(pCallback);
			}
		}
	}
	__finally
	{
#ifdef WIN32
		::LeaveCriticalSection(&m_cs);
#endif
	}
}

void CPurpleProtocolStack::RemoveProtocol(CIMProtocol* pProtocol, CIMProtocolCallback* pCallback)
{
	__try
	{
#ifdef WIN32
		::EnterCriticalSection(&m_cs);
#endif

		if (pProtocol)
		{		
			CIMProtocol* pCurrent = this;
			CIMProtocol* pPrev = this;

			while ((pCurrent) && (pCurrent != pProtocol))
			{	
				pPrev = pCurrent;
				pCurrent = pCurrent->GetNextProtocol();
			}

			if (pCurrent)
			{
				pPrev->SetNextProtocol(pCurrent->GetNextProtocol());
			}
		}

		if (pCallback)
		{
			CIMProtocolCallback* pCurrentCallback = this;
			CIMProtocolCallback* pPrevCallback = this;

			while ((pCurrentCallback) && (pCurrentCallback != pCallback))
			{	
				pPrevCallback = pCurrentCallback;
				pCurrentCallback = pCurrentCallback->GetPreviousCallback();
			}

			if (pCurrentCallback)
			{
				pPrevCallback->SetPreviousCallback(pCurrentCallback->GetPreviousCallback());
			}
		}
	}
	__finally
	{
#ifdef WIN32
		::LeaveCriticalSection(&m_cs);
#endif
	}
}


std::string CPurpleProtocolStack::GetId()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->GetId();
	}

	return "prpl-purplestack";
}

std::string CPurpleProtocolStack::GetName()
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		return pNext->GetName();
	}

	return "purpleprotocolstack";
}

void CPurpleProtocolStack::SetCallback(const std::string& strUser, CIMProtocolCallback* pCallback)
{
	CIMProtocol* pNext = GetNextProtocol();

	if (pNext)
	{
		pNext->SetCallback(strUser, this);

		//if (NULL == GetPreviousCallback())
		//{
		//	CCallbackSelector* pSelector = new CCallbackSelector(this);

		//	SetPreviousCallback(pSelector);
		//}
	}

	SetExternalCallback(strUser, pCallback);
}



CIMProtocolCallback* CPurpleProtocolStack::GetCallback(const char* pszUser)
{
	CIMProtocolCallback* pCallback = NULL;

	if (pszUser != NULL)
	{
		string strUser = pszUser;
		CCallbackMap::const_iterator it = m_mapExternalCallbacks.find(strUser);
		
		if (it != m_mapExternalCallbacks.end())
		{
			pCallback = it->second;
		}
	}

	return pCallback;
}


void CPurpleProtocolStack::SetExternalCallback(const std::string& strUser, CIMProtocolCallback* pCallback)
{
	if (strUser.length() > 0)
	{
		m_mapExternalCallbacks[strUser] = pCallback;
	}
}