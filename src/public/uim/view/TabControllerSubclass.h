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

/*
 * Essa classe é uma adaptação da classe CShowTooltip para os casos 
 * onde o elemento que deverá apresentar o Tooltip nao seja criado, 
 * mas sim realize um SubclassWindow no elemento gráfico principal.
 * 
 * Neste caso, o componente CShowTooltip original não funciona, 
 * pois não recebe a mensagem WM_CREATE, e consequentemente não 
 * criará o componente de Tooltip.
 * 
 * Esse problema foi contornado, criando-se o componente Tooltip 
 * no primeiro momento em que ele se torne necessário na 
 * manipulação da mensagem WM_MOUSEMOVE.
 * 
 * Além disso, ele é destinado a controle individual, por componente, 
 * do Tooltip a ser apresentado. Diferentemente do CShowToolTip 
 * que se destina a componentes containers de elementos que 
 * apresentarão o seu Tooltip.
 * 
 */
 

#pragma once


class CTabControllerItem
{
public:
	
	CTabControllerItem(HWND hCtrlHandle, BOOL bShowFocusRect, BOOL bIsHTMLWindow = FALSE) : 
			m_hCtrlHandle(hCtrlHandle), 
			m_bShowFocusRect(bShowFocusRect), 
			m_bIsHTMLWindow(bIsHTMLWindow)
	{
	}
	
	void SetControllerItemHandle(HWND hCtrlHandle)
	{
		m_hCtrlHandle = hCtrlHandle;
	}
	
	HWND GetControllerItemHandle()
	{
		return m_hCtrlHandle;
	}
	
	BOOL GetShowFocusRect()
	{
		return m_bShowFocusRect;
	}
	
	BOOL GetIsHTMLWindow()
	{
		return m_bIsHTMLWindow;
	}
	
private:
	
	HWND m_hCtrlHandle;
	
	BOOL m_bShowFocusRect;
	
	BOOL m_bIsHTMLWindow;
};

MAKEAUTOPTR(CTabControllerItem);


template<class T>
class CTabControllerSubclass
{
public:
	
	enum PreFocusActivateWindowConsts
	{
		PRE_ACTIVATE_THIS_WINDOW = 0X2111,
		PRE_ACTIVATE_TOP_LEVEL_WINDOW,
		PRE_ACTIVATE_LAST_CHILD_WINDOW
	};
	
	CTabControllerSubclass()
	{
		m_pT = NULL;
		
		m_bInnerHTMLWindowsValidated = FALSE;
		
		m_hPreFocusActivateWindowHandle = NULL;
		m_cPreFocusActivateWindowModifier = PRE_ACTIVATE_THIS_WINDOW;
		m_bPreFocusActivationRan = FALSE;
	}
	
	int AddTabTargetCtrl(const HWND &hTargetHandle, BOOL bShowFocusRect, BOOL bIsHTMLWindow = FALSE)
	{
		HWND hFinalTargetHandle = NULL;
		
		if (!bIsHTMLWindow)
		{
			hFinalTargetHandle = hTargetHandle;
		}
		else
		{
			hFinalTargetHandle = GetLastChildWindow(hTargetHandle);
		}
		
		CTabControllerItemPtr pControllerItem = 
				new CTabControllerItem(hFinalTargetHandle, bShowFocusRect, bIsHTMLWindow);
		
		int nCurrentIndex = m_oTabIndexTargetMap.GetCount();
		
		m_oTabIndexTargetMap.SetAt(nCurrentIndex, pControllerItem);
		m_oTabTargetIndexMap.SetAt(hFinalTargetHandle, nCurrentIndex);
		
		return nCurrentIndex;
	}
	
	void UpdateTabTargetCtrlHandle(int nTabTargetIndex, HWND hTabTargetHandle)
	{
		CTabIndexTargetMap::CPair* pIndexTarget = m_oTabIndexTargetMap.Lookup(nTabTargetIndex);
		
		if (pIndexTarget)
		{
			HWND hFinalTargetHandle = NULL;
			
			if (!pIndexTarget->m_value->GetIsHTMLWindow())
			{
				hFinalTargetHandle = hTabTargetHandle;
			}
			else
			{
				hFinalTargetHandle = GetLastChildWindow(hTabTargetHandle);
			}
			
			// Save old handle value for future deletion of another map, and 
			// updates the index key map.
			//
			HWND oldHandleValue = pIndexTarget->m_value->GetControllerItemHandle();
			pIndexTarget->m_value->SetControllerItemHandle(hFinalTargetHandle);
			
			// Removes old value, and update the handle key map.
			//
			m_oTabTargetIndexMap.RemoveKey(oldHandleValue);
			m_oTabTargetIndexMap.SetAt(hFinalTargetHandle, nTabTargetIndex);
		}
	}
	
	/*
	 * This method is intended to set the window that must receive the WM_MOUSEACTIVATE 
	 * message so the buttons can be drawed correctly, showing the focus rect.
	 * 
	 * Without this message being sent to this window, the focus rect is not shown for 
	 * buttons.
	 * 
	 */
	void SetPreFocusActivateWindowHandle(HWND hPreFocusActivateWindowHandler, 
			PreFocusActivateWindowConsts cPreFocusActivateWindowModifier = PRE_ACTIVATE_THIS_WINDOW)
	{
		m_hPreFocusActivateWindowHandle = hPreFocusActivateWindowHandler;
		
		m_cPreFocusActivateWindowModifier = cPreFocusActivateWindowModifier;
	}
	
protected:
	
	BEGIN_MSG_MAP(CTabControllerSubclass<T>)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_NEXT_CTRL, OnAcceleratorNextCtrl)
		COMMAND_ID_HANDLER(ID_ACCELERATOR_PRIOR_CTRL, OnAcceleratorPriorCtrl)
	END_MSG_MAP()
	
	inline LRESULT OnCreate(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if (!m_pT)
		{
			m_pT = (T*)this;
		}
		
		bHandled = FALSE;
		
		return 0L;
	}
	
	inline LRESULT OnDestroy(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		m_oTabIndexTargetMap.RemoveAll();
		m_oTabTargetIndexMap.RemoveAll();
		
		bHandled = FALSE;
		
		return 0L;
	}
	
	inline LRESULT OnAcceleratorNextCtrl(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		SetNextTabTargetFocus();
		
		bHandled = FALSE;
		
		return 0L;
	}
	
	inline LRESULT OnAcceleratorPriorCtrl(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		SetPriorTabTargetFocus();
		
		bHandled = FALSE;
		
		return 0L;
	}
	
	void SetNextTabTargetFocus()
	{
		ATLTRACE("SetNextTabTargetFocus() - Focus now is on componente with handle [%x]...\n", 
				::GetFocus());
		
		ValidateHandlesFromHTMLWindows();
		
		//RunWindowPreFocusActivation();
		
		// First we try to find out whether the focus is on a not mapped window.
		// If it is, we reset our current index and set the focus to our first 
		// control.
		//
		CTabTargetIndexMap::CPair* pTargetIndexPair = m_oTabTargetIndexMap.Lookup(::GetFocus());
		int nCurrentTabIndex;
		BOOL bFocusOutOfSet = FALSE;
		
		if (pTargetIndexPair == NULL)
		{
			nCurrentTabIndex = 0;
			
			bFocusOutOfSet = TRUE;
			
			ATLTRACE("SetNextTabTargetFocus() - Component out of set...\n");
		}
		else
		{
			ATLTRACE("SetNextTabTargetFocus() - Focus on component number [%d] of the set...\n", 
					pTargetIndexPair->m_value);
			
			nCurrentTabIndex = pTargetIndexPair->m_value;
			
			nCurrentTabIndex++;
		}
		
		if (nCurrentTabIndex == m_oTabIndexTargetMap.GetCount())
		{
			nCurrentTabIndex = 0;
		}
		
		CTabIndexTargetMap::CPair* pIndexTargetPair = m_oTabIndexTargetMap.Lookup(nCurrentTabIndex);
		
		if (pIndexTargetPair)
		{
			ATLTRACE("SetNextTabTargetFocus() - Focusing on component number [%d] and handle [%x] of the set...\n", 
					pIndexTargetPair->m_key, pIndexTargetPair->m_value->GetControllerItemHandle());
			
			if (pIndexTargetPair->m_value->GetShowFocusRect())
			{
				LRESULT lr = ::SendMessage(pIndexTargetPair->m_value->GetControllerItemHandle(), 
						WM_CHANGEUISTATE, MAKEWPARAM(UIS_CLEAR, UISF_HIDEFOCUS), 0);
				
				ATLTRACE("SetNextTabTargetFocus() - WM_CHANGEUISTATE result = [%x]\n", lr);
			}
			
			::SetFocus(pIndexTargetPair->m_value->GetControllerItemHandle());
		}
	}
	
	void SetPriorTabTargetFocus()
	{
		ATLTRACE("SetPriorTabTargetFocus() - Focus now is on componente with handle [%x]...\n", 
				::GetFocus());
		
		ValidateHandlesFromHTMLWindows();
		
		//RunWindowPreFocusActivation();
		
		// First we try to find out whether the focus is on a not mapped window.
		// If it is, we reset our current index and set the focus to our last 
		// control.
		//
		CTabTargetIndexMap::CPair* pTargetIndexPair = m_oTabTargetIndexMap.Lookup(::GetFocus());
		int nCurrentTabIndex;
		
		if (pTargetIndexPair == NULL)
		{
			nCurrentTabIndex = m_oTabTargetIndexMap.GetCount() - 1;
			
			ATLTRACE("SetPriorTabTargetFocus() - Component out of set...\n");
		}
		else
		{
			ATLTRACE("SetPriorTabTargetFocus() - Focus on component number [%d] of the set...\n", 
					pTargetIndexPair->m_value);
			
			nCurrentTabIndex = pTargetIndexPair->m_value;
			
			nCurrentTabIndex--;
		}
		
		if (nCurrentTabIndex < 0)
		{
			nCurrentTabIndex = m_oTabIndexTargetMap.GetCount() - 1;
		}
		
		CTabIndexTargetMap::CPair* pIndexTargetPair = m_oTabIndexTargetMap.Lookup(nCurrentTabIndex);
		
		if (pIndexTargetPair)
		{
			ATLTRACE("SetPriorTabTargetFocus() - Focusing on component number [%d] and handle [%x] of the set...\n", 
					pIndexTargetPair->m_key, pIndexTargetPair->m_value->GetControllerItemHandle());
			
			if (pIndexTargetPair->m_value->GetShowFocusRect())
			{
				LRESULT lr = ::SendMessage(pIndexTargetPair->m_value->GetControllerItemHandle(), 
						WM_CHANGEUISTATE, MAKEWPARAM(UIS_CLEAR, UISF_HIDEFOCUS), 0);
				
				ATLTRACE("SetNextTabTargetFocus() - WM_CHANGEUISTATE result = [%x]\n", lr);
			}
			
			::SetFocus(pIndexTargetPair->m_value->GetControllerItemHandle());
		}
	}
	
	HWND GetTopLevelWindow(HWND hInnerWindow)
	{
		HWND hTopLevelWnd = hInnerWindow;
		HWND hParentWnd = ::GetParent(hInnerWindow);
		
		while (hParentWnd)
		{
			hTopLevelWnd = hParentWnd;
			hParentWnd = ::GetParent(hTopLevelWnd);
		}
		
		return hTopLevelWnd;
	}
	
	HWND GetLastChildWindow(HWND hMainWindow)
	{
		HWND hLastChildWnd = hMainWindow;
		HWND hChildWnd = ::GetWindow(hMainWindow, GW_CHILD);
		
		while (hChildWnd)
		{
			hLastChildWnd = hChildWnd;
			hChildWnd = ::GetWindow(hLastChildWnd, GW_CHILD);
		}
		
		return hLastChildWnd;
	}
	
	/*
	 * This method is intended to fix up the problem of storing the "Shell DocObject View" 
	 * handle, instead of the final child handle "Internet Explorer_Server".
	 * 
	 * This happens, because the last handle was not already created during the add of 
	 * the tab order elements.
	 * 
	 */
	void ValidateHandlesFromHTMLWindows()
	{
		if (!m_bInnerHTMLWindowsValidated)
		{
			for (POSITION nIndexPos = m_oTabIndexTargetMap.GetStartPosition(); nIndexPos; )
			{
				CTabIndexTargetMap::CPair* pIndexTargetPair = m_oTabIndexTargetMap.GetAt(
						m_oTabIndexTargetMap.GetNext(nIndexPos));
				
				if ( (pIndexTargetPair) && (pIndexTargetPair->m_value->GetIsHTMLWindow()) )
				{
					HWND hFinalTargetHandle = GetLastChildWindow(
							pIndexTargetPair->m_value->GetControllerItemHandle());
					
					if (hFinalTargetHandle != pIndexTargetPair->m_value->GetControllerItemHandle())
					{
						UpdateTabTargetCtrlHandle(pIndexTargetPair->m_key, hFinalTargetHandle);
					}
				}
			}
		}
		
		m_bInnerHTMLWindowsValidated = TRUE;
	}
	
	/*
	 * Makes the PreFocusActivation of the window happen.
	 * 
	 */
	void RunWindowPreFocusActivation()
	{
		HWND hPreFocusActivationFinalHandle = NULL;
		
		ATLTRACE("RunWindowPreFocusActivation() - m_bPreFocusActivationRan = [%d], e m_hPreFocusActivateWindowHandle = [%x]\n", 
				m_bPreFocusActivationRan, m_hPreFocusActivateWindowHandle);
		
		if ( (!m_bPreFocusActivationRan) && (m_hPreFocusActivateWindowHandle) )
		{
			if (m_cPreFocusActivateWindowModifier == PRE_ACTIVATE_THIS_WINDOW)
			{
				hPreFocusActivationFinalHandle = m_hPreFocusActivateWindowHandle;
			}
			else if (m_cPreFocusActivateWindowModifier == PRE_ACTIVATE_TOP_LEVEL_WINDOW)
			{
				hPreFocusActivationFinalHandle = GetTopLevelWindow(m_hPreFocusActivateWindowHandle);
			}
			else if (m_cPreFocusActivateWindowModifier == PRE_ACTIVATE_LAST_CHILD_WINDOW)
			{
				hPreFocusActivationFinalHandle = GetLastChildWindow(m_hPreFocusActivateWindowHandle);
			}
			
			char strClassName[70];
			memset(strClassName, 0, 70);
			::GetClassName(hPreFocusActivationFinalHandle, strClassName, 70);
			
			ATLTRACE("RunWindowPreFocusActivation() - WM_CHANGEUISTATE to handle [%x] of window [%s]\n", 
					hPreFocusActivationFinalHandle, strClassName);
			
			LRESULT lr;
			
			lr = ::SendMessage(hPreFocusActivationFinalHandle, WM_CHANGEUISTATE, 
					MAKEWPARAM(UIS_CLEAR, UISF_HIDEFOCUS), 0);
			
			ATLTRACE("RunWindowPreFocusActivation() - WM_CHANGEUISTATE result = [%x]\n", lr);
			
			m_bPreFocusActivationRan = TRUE;
		}
	}
	
	
private:
	
	T								*m_pT;
	
	typedef CAtlMap<int, CTabControllerItemPtr>		CTabIndexTargetMap;
	CTabIndexTargetMap								m_oTabIndexTargetMap;
	
	typedef CAtlMap<HWND, int>		CTabTargetIndexMap;
	CTabTargetIndexMap				m_oTabTargetIndexMap;
	
	BOOL							m_bInnerHTMLWindowsValidated;
	
	HWND							m_hPreFocusActivateWindowHandle;
	PreFocusActivateWindowConsts	m_cPreFocusActivateWindowModifier;
	BOOL							m_bPreFocusActivationRan;
};

