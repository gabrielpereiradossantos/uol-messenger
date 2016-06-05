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
 

#pragma once

#include <Tlhelp32.h.>

// Para usar CProcessSnapshot inclua tlhelp32.h em seu stdafx.h
struct PROCESSENTRY32Helper : public tagPROCESSENTRY32
{
	PROCESSENTRY32Helper()		
	{
		dwSize = sizeof(PROCESSENTRY32);
	}

	PROCESSENTRY32Helper(const PROCESSENTRY32Helper& ref)	
	{
		*this = ref;
	}

	PROCESSENTRY32Helper& operator= (const PROCESSENTRY32Helper& ref)
	{
		if (&ref != this)
		{
			dwSize				= ref.dwSize;
			cntUsage			= ref.cntUsage;
			th32ProcessID		= ref.th32ProcessID;         
			th32DefaultHeapID	= ref.th32DefaultHeapID;
			th32ModuleID		= ref.th32ModuleID;          
			cntThreads			= ref.cntThreads;
			th32ParentProcessID = ref.th32ParentProcessID;   
			pcPriClassBase		= ref.pcPriClassBase;        
			dwFlags				= ref.dwFlags;

			::_tcsncpy(szExeFile, ref.szExeFile, MAX_PATH);
		}

		return *this;
	}
};

class ProccesIterator
{
public:
	BOOL GetFirst(HANDLE hSnapshot, PROCESSENTRY32Helper& item) 
	{
		return ::Process32First(hSnapshot, &item);
	}

	BOOL GetNext(HANDLE hSnapshot, PROCESSENTRY32Helper& item) 
	{
		return ::Process32Next(hSnapshot, &item);
	}
	
	DWORD GetFlags()
	{
		return TH32CS_SNAPPROCESS;
	}
};

struct THREADENTRY32Helper : public tagTHREADENTRY32
{
	THREADENTRY32Helper()
	{
		dwSize = sizeof(THREADENTRY32);
	}
	
	THREADENTRY32Helper(const THREADENTRY32Helper& ref)
	{
		*this = ref;
	}

	THREADENTRY32Helper& operator= (const THREADENTRY32Helper& ref)
	{
		if (this != &ref)
		{
			::CopyMemory(this, &ref, sizeof(THREADENTRY32Helper));
		}
		
		return *this;
	}
} ;

class ThreadIterator
{
public:
	BOOL GetFirst(HANDLE hSnapshot, THREADENTRY32Helper& item) 
	{
		return ::Thread32First(hSnapshot, &item);
	}

	BOOL GetNext(HANDLE hSnapshot, THREADENTRY32Helper& item) 
	{
		return ::Thread32Next(hSnapshot, &item);
	}
	
	DWORD GetFlags()
	{
		return TH32CS_SNAPTHREAD;
	}
};

class CProcessSnapshot
{
public:
	CProcessSnapshot()
	{
	}

	void TakeProccesSnapshot(CAtlList<PROCESSENTRY32Helper>& snapshot)
	{
		TakeSnapshot(snapshot, ProccesIterator());
	}

	void TakeThreadSnapshot(CAtlList<THREADENTRY32Helper>& snapshot)
	{		
		TakeSnapshot(snapshot, ThreadIterator());
	}

	template <class T, class Iterator>
	void TakeSnapshot(CAtlList<T>& snapshot, Iterator it, DWORD dwProcess = 0)
	{
		snapshot.RemoveAll();

		HANDLE hSnapshot = ::CreateToolhelp32Snapshot(it.GetFlags(), dwProcess);

		if (INVALID_HANDLE_VALUE == hSnapshot)
		{
			return ;
		}

		T item;		

		if (it.GetFirst(hSnapshot, item))
		{
			do
			{
				snapshot.AddTail(item);

			} while(it.GetNext(hSnapshot, item));
		}

		::CloseHandle(hSnapshot);			
	}

};