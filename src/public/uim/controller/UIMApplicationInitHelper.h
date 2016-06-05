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


struct ITask
{
	virtual ~ITask() {}

	virtual BOOL DoTask() = 0;
	virtual void UndoTask() = 0;
    virtual void PreUndoTask() = 0;
	virtual BOOL GetRestartFlag() = 0;
	
	CString m_strTaskDescription;
};
MAKEAUTOPTR(ITask);


struct IInitHelperCallback
{
	virtual ~IInitHelperCallback()
	{
	}
	
	virtual void OnTaskAdded(const CString& m_strTaskDescription) {};
	virtual void OnRunningTask(const CString& m_strTaskDescription) {};
	virtual void OnTaskDone(const CString& m_strTaskDescription, BOOL bDoneOk) {};
	virtual void OnFinalizingTask(const CString& m_strTaskDescription) {};	// Maybe for future use.
	virtual void OnTaskFinalized(const CString& m_strTaskDescription) {};		// Maybe for future use.
};

MAKEAUTOPTR(IInitHelperCallback);


class CUIMApplicationInitHelper
{
public:
	CUIMApplicationInitHelper() : 
		m_nUndoTaskCount(0),
		m_bRestart(FALSE)
	{
	}

	~CUIMApplicationInitHelper()
	{
	}

	void SetInitHelperCallback(IInitHelperCallbackPtr pInitHelperCallback)
	{
		m_pInitHelperCallback = pInitHelperCallback;
	}

	void AddTask(ITaskPtr pTask)
	{
		m_list.AddTail(pTask);
		
		if (m_pInitHelperCallback)
		{
			m_pInitHelperCallback->OnTaskAdded(pTask->m_strTaskDescription);
		}
	}

	BOOL Init()
	{
		m_nUndoTaskCount = 0;
		m_bRestart = 0;

		for (POSITION pos = m_list.GetHeadPosition(); pos;)
		{
			ITaskPtr pTask = m_list.GetNext(pos);
			
			if (m_pInitHelperCallback)
			{
				m_pInitHelperCallback->OnRunningTask(pTask->m_strTaskDescription);
			}

			if (pTask->DoTask() == FALSE)
			{
				m_bRestart |= pTask->GetRestartFlag();
				
				if (m_pInitHelperCallback)
				{
					m_pInitHelperCallback->OnTaskDone(pTask->m_strTaskDescription, FALSE);
				}

				return FALSE;
			}
			else if (m_pInitHelperCallback)
			{
				m_pInitHelperCallback->OnTaskDone(pTask->m_strTaskDescription, TRUE);
			}

			m_nUndoTaskCount++;
		}

		return TRUE;
	}

	void Finalize()
	{
		while ((UINT) m_list.GetCount() > m_nUndoTaskCount)
		{
			m_list.RemoveTailNoReturn();
		}

		for (POSITION pos = m_list.GetTailPosition(); pos;)
		{
			m_list.GetPrev(pos)->UndoTask();
			m_nUndoTaskCount--;
		}

		m_list.RemoveAll();

		ATLASSERT(m_nUndoTaskCount == 0);
	}

    void PreFinalize()
	{
		while ((UINT) m_list.GetCount() > m_nUndoTaskCount)
		{
			m_list.RemoveTailNoReturn();
		}

		for (POSITION pos = m_list.GetTailPosition(); pos;)
		{
			m_list.GetPrev(pos)->PreUndoTask();
		}
	}

	BOOL GetRestartFlag()
	{
		return m_bRestart;
	}

private:
	CAtlList<ITaskPtr>	m_list;
	UINT	m_nUndoTaskCount;
	BOOL	m_bRestart;
	
	IInitHelperCallbackPtr	m_pInitHelperCallback;
};