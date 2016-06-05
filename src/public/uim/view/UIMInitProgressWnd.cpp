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
 
#include "StdAfx.h"
#include "./UIMInitProgressWnd.h"


CUIMInitProgressWnd::CUIMInitProgressWnd(void) : 
		m_iTasksCount(0), 
		m_iCurrentTask(0), 
		m_bHasInitializedProgress(FALSE)
{
}


CUIMInitProgressWnd::~CUIMInitProgressWnd(void)
{		
}


BOOL CUIMInitProgressWnd::Initialize()
{
	ATLASSERT(FALSE == IsWindow());
	
	HWND hWnd = Create(NULL, NULL);
	
	if (::IsWindow(hWnd))
	{
		ShowWindow(SW_SHOW);
	}
	
	return ::IsWindow(hWnd);
}


void CUIMInitProgressWnd::Finalize(BOOL bUrgent)
{
	if (!bUrgent)
	{
		ShowWindow(SW_HIDE);
		
		PostMessage(WM_USER_FINALIZE_DIALOG, 0, 0);
	}
	else if (IsWindow())
	{
		DestroyWindow();
	}
}


void CUIMInitProgressWnd::OnTaskAdded(const CString& strTaskDescription)
{
	m_iTasksCount++;
	
	/* No messages for now...
	
	ProgressMessage* pProgressMessage = new ProgressMessage;
	
	if (pProgressMessage)
	{
		pProgressMessage->strProgressMsg.Format("Added task [%s].", strTaskDescription);
		
		SendMessage(WM_TASK_PROGRESS_MSG, 0, (LPARAM)pProgressMessage);
		
		delete pProgressMessage;
	}
	*/
}


void CUIMInitProgressWnd::OnRunningTask(const CString& strTaskDescription)
{
	m_iCurrentTask++;
	
	if (!m_bHasInitializedProgress)
	{
		m_bHasInitializedProgress = TRUE;
		
		m_progressBar.SetRange(0, 100);
	}
	
	int iProgressPosition = static_cast<int>(
				static_cast<float>(m_iCurrentTask) / static_cast<float>(m_iTasksCount) * 100.0);
	
	m_progressBar.SetPos(iProgressPosition);
	
	ProgressMessage* pProgressMessage = new ProgressMessage;
	
	if (pProgressMessage)
	{
		//pProgressMessage->strProgressMsg.Format("Running task %d from %d -> [%s].", 
				//m_iCurrentTask, m_iTasksCount, strTaskDescription);
		
		pProgressMessage->strProgressMsg.Format("Carregando... %d%%", iProgressPosition);
		
		SendMessage(WM_TASK_PROGRESS_MSG, 0, (LPARAM)pProgressMessage);
		
		delete pProgressMessage;
	}
}


void CUIMInitProgressWnd::OnTaskDone(const CString& strTaskDescription, BOOL bDoneOk)
{
	/* No messages for now...
	
	ProgressMessage* pProgressMessage = new ProgressMessage;
	
	if (pProgressMessage)
	{
		CString strStatus;
		strStatus = (bDoneOk == TRUE) ? "success" : "failure";
		
		pProgressMessage->strProgressMsg.Format("Taks [%s] ended with %s.", 
				strTaskDescription, strStatus);
		
		SendMessage(WM_TASK_PROGRESS_MSG, 0, (LPARAM)pProgressMessage);
		
		delete pProgressMessage;
	}
	*/
}


LRESULT CUIMInitProgressWnd::OnInitDialog(HWND hwnd, LPARAM lParam)
{
	CenterWindow();
	
	m_staticProgressText = GetDlgItem(IDC_STATIC_PROGRESS);
	m_progressBar = GetDlgItem(IDC_PROGRESS_INIT);
	
	return 0L;
}


LRESULT CUIMInitProgressWnd::OnFinalizeDialog(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ATLASSERT(IsWindow());
	
	if (IsWindow())
	{
		DestroyWindow();
	}
	
	return 0L;
}


LRESULT CUIMInitProgressWnd::OnProgressMessage(
		UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ProgressMessage* pProgressMessage = (ProgressMessage*)lParam;
	
	if (pProgressMessage)
	{
		m_staticProgressText.SetWindowText(pProgressMessage->strProgressMsg);
	}
	
	return 0L;
}

