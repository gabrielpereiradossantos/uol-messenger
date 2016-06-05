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
#include ".\UOLFoneKeyboardCtrl.h"

#include "../FacadeWrapper.h"
#include "../Resource.h"
#include "../UOLFonePluginTheme.h"
#include "UOLFoneController.h"



/////////////////////////////////////////////////////////////////////
// CUOLFoneWindow positioning defines
/////////////////////////////////////////////////////////////////////


#define		DEFAULT_SHIFT						5

#define		LEFT_SHIFT							DEFAULT_SHIFT
#define		TOP_SHIFT							8
#define		RIGHT_SHIFT							DEFAULT_SHIFT
#define		BOTTOM_SHIFT						0

#define		INNER_COMPONENT_DEFAULT_HEIGHT				25
#define		INNER_COMPONENTS_HORIZONTAL_SEPARATION		5
#define		INNER_COMPONENTS_VERTICAL_SEPARATION		5

#define		VERTICAL_BLOCK_SEPARATION					5


/////////////////////////////////////////////////////////////////////
// CUOLFoneKeyboardCtrl
/////////////////////////////////////////////////////////////////////


CUOLFoneKeyboardCtrl::CUOLFoneKeyboardCtrl() :
	m_bIsAlfaNumeric(FALSE),
	m_destroyingWindow(FALSE),
	m_iLastAlfaNumericButton(0),
	m_iAlfaNumericButtonSequence(0),
	m_bCleanLast(FALSE)
{
	m_componentsOrganizer.Initialize(this, LEFT_SHIFT, TOP_SHIFT, 
			RIGHT_SHIFT, BOTTOM_SHIFT);

	m_dwAlfaNumericButtonTick = ::GetTickCount();
}


CUOLFoneKeyboardCtrl::~CUOLFoneKeyboardCtrl() 
{
	m_listObservers.RemoveAll();
}


int CUOLFoneKeyboardCtrl::GetWidth()
{
	return m_componentsOrganizer.GetMinimumWindowWidth();
}


int CUOLFoneKeyboardCtrl::GetHeight()
{
	return m_componentsOrganizer.GetMinimumWindowHeight();
}


void CUOLFoneKeyboardCtrl::SetAlfaNumeric(BOOL bIsAlfaNumeric)
{
	m_bIsAlfaNumeric = bIsAlfaNumeric;
}


LRESULT CUOLFoneKeyboardCtrl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//
	// Creating controls...
	//
	
	CString labelStr;
		
	labelStr.LoadString(IDS_BTN_LABEL_KEY_0);
	m_key0.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_0);
	m_multiStateKey0.SubclassWindow(m_key0.m_hWnd);
	m_multiStateKey0.SetMultiStateBitmap(IDB_BTN_TECLADO_0);
	m_multiStateKey0.SetCharForwardingWnd(m_hWnd);
	
	labelStr.LoadString(IDS_BTN_LABEL_KEY_1);
	m_key1.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_1);
	m_multiStateKey1.SubclassWindow(m_key1.m_hWnd);
	m_multiStateKey1.SetMultiStateBitmap(IDB_BTN_TECLADO_1);
	m_multiStateKey1.SetCharForwardingWnd(m_hWnd);
	
	labelStr.LoadString(IDS_BTN_LABEL_KEY_2);
	m_key2.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_2);
	m_multiStateKey2.SubclassWindow(m_key2.m_hWnd);
	m_multiStateKey2.SetMultiStateBitmap(IDB_BTN_TECLADO_2);
	m_multiStateKey2.SetCharForwardingWnd(m_hWnd);
	
	labelStr.LoadString(IDS_BTN_LABEL_KEY_3);
	m_key3.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_3);
	m_multiStateKey3.SubclassWindow(m_key3.m_hWnd);
	m_multiStateKey3.SetMultiStateBitmap(IDB_BTN_TECLADO_3);
	m_multiStateKey3.SetCharForwardingWnd(m_hWnd);
	
	labelStr.LoadString(IDS_BTN_LABEL_KEY_4);
	m_key4.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_4);
	m_multiStateKey4.SubclassWindow(m_key4.m_hWnd);
	m_multiStateKey4.SetMultiStateBitmap(IDB_BTN_TECLADO_4);
	m_multiStateKey4.SetCharForwardingWnd(m_hWnd);
	
	labelStr.LoadString(IDS_BTN_LABEL_KEY_5);
	m_key5.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_5);
	m_multiStateKey5.SubclassWindow(m_key5.m_hWnd);
	m_multiStateKey5.SetMultiStateBitmap(IDB_BTN_TECLADO_5);
	m_multiStateKey5.SetCharForwardingWnd(m_hWnd);
	
	labelStr.LoadString(IDS_BTN_LABEL_KEY_6);
	m_key6.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_6);
	m_multiStateKey6.SubclassWindow(m_key6.m_hWnd);
	m_multiStateKey6.SetMultiStateBitmap(IDB_BTN_TECLADO_6);
	m_multiStateKey6.SetCharForwardingWnd(m_hWnd);
	
	labelStr.LoadString(IDS_BTN_LABEL_KEY_7);
	m_key7.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_7);
	m_multiStateKey7.SubclassWindow(m_key7.m_hWnd);
	m_multiStateKey7.SetMultiStateBitmap(IDB_BTN_TECLADO_7);
	m_multiStateKey7.SetCharForwardingWnd(m_hWnd);
	
	labelStr.LoadString(IDS_BTN_LABEL_KEY_8);
	m_key8.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_8);
	m_multiStateKey8.SubclassWindow(m_key8.m_hWnd);
	m_multiStateKey8.SetMultiStateBitmap(IDB_BTN_TECLADO_8);
	m_multiStateKey8.SetCharForwardingWnd(m_hWnd);
	
	labelStr.LoadString(IDS_BTN_LABEL_KEY_9);
	m_key9.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_9);
	m_multiStateKey9.SubclassWindow(m_key9.m_hWnd);
	m_multiStateKey9.SetMultiStateBitmap(IDB_BTN_TECLADO_9);
	m_multiStateKey9.SetCharForwardingWnd(m_hWnd);
	
	labelStr.LoadString(IDS_BTN_LABEL_KEY_ASTERISK);
	m_keyAsterisk.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_ASTERISK);
	m_multiStateKeyAsterisk.SubclassWindow(m_keyAsterisk.m_hWnd);
	m_multiStateKeyAsterisk.SetMultiStateBitmap(IDB_BTN_TECLADO_ASTERISK);
	m_multiStateKeyAsterisk.SetCharForwardingWnd(m_hWnd);
	
	labelStr.LoadString(IDS_BTN_LABEL_KEY_POUND);
	m_keyPound.Create(m_hWnd, CRect(0, 0, 0, 0), labelStr, 
			WS_CHILDWINDOW | WS_VISIBLE | BS_OWNERDRAW, 
			0, ID_BUTTON_KEY_POUND);
	m_multiStateKeyPound.SubclassWindow(m_keyPound.m_hWnd);
	m_multiStateKeyPound.SetMultiStateBitmap(IDB_BTN_TECLADO_POUND);
	m_multiStateKeyPound.SetCharForwardingWnd(m_hWnd);
	
	ATLASSERT(m_key0.IsWindow());
	ATLASSERT(m_key1.IsWindow());
	ATLASSERT(m_key2.IsWindow());
	ATLASSERT(m_key3.IsWindow());
	ATLASSERT(m_key4.IsWindow());
	ATLASSERT(m_key5.IsWindow());
	ATLASSERT(m_key6.IsWindow());
	ATLASSERT(m_key7.IsWindow());
	ATLASSERT(m_key8.IsWindow());
	ATLASSERT(m_key9.IsWindow());
	ATLASSERT(m_keyAsterisk.IsWindow());
	ATLASSERT(m_keyPound.IsWindow());
	
	//
	// Building layout...
	//
			
	// Build line 1.
	m_componentsOrganizer.AddLine(INNER_COMPONENTS_HORIZONTAL_SEPARATION, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_key1, m_key1.GetWndClassName(), 
			m_multiStateKey1.GetWidth(), 0, m_multiStateKey1.GetHeight());
	m_componentsOrganizer.AddComponent(&m_key2, m_key2.GetWndClassName(), 
			m_multiStateKey2.GetWidth(), 0, m_multiStateKey2.GetHeight());
	m_componentsOrganizer.AddComponent(&m_key3, m_key3.GetWndClassName(), 
			m_multiStateKey3.GetWidth(), 0, m_multiStateKey3.GetHeight());
	
	// Add a separator line.
	m_componentsOrganizer.AddSeparatorLine(INNER_COMPONENTS_VERTICAL_SEPARATION);
	
	// Build line 2.
	m_componentsOrganizer.AddLine(INNER_COMPONENTS_HORIZONTAL_SEPARATION, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_key4, m_key4.GetWndClassName(), 
			m_multiStateKey4.GetWidth(), 0, m_multiStateKey4.GetHeight());
	m_componentsOrganizer.AddComponent(&m_key5, m_key5.GetWndClassName(), 
			m_multiStateKey5.GetWidth(), 0, m_multiStateKey5.GetHeight());
	m_componentsOrganizer.AddComponent(&m_key6, m_key6.GetWndClassName(), 
			m_multiStateKey6.GetWidth(), 0, m_multiStateKey6.GetHeight());
	
	// Add a separator line.
	m_componentsOrganizer.AddSeparatorLine(INNER_COMPONENTS_VERTICAL_SEPARATION);
	
	// Build line 3.
	m_componentsOrganizer.AddLine(INNER_COMPONENTS_HORIZONTAL_SEPARATION, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_key7, m_key7.GetWndClassName(), 
			m_multiStateKey7.GetWidth(), 0, m_multiStateKey7.GetHeight());
	m_componentsOrganizer.AddComponent(&m_key8, m_key8.GetWndClassName(), 
			m_multiStateKey8.GetWidth(), 0, m_multiStateKey8.GetHeight());
	m_componentsOrganizer.AddComponent(&m_key9, m_key9.GetWndClassName(), 
			m_multiStateKey9.GetWidth(), 0, m_multiStateKey9.GetHeight());
	
	// Add a separator line.
	m_componentsOrganizer.AddSeparatorLine(INNER_COMPONENTS_VERTICAL_SEPARATION);
	
	// Build line 4.
	m_componentsOrganizer.AddLine(INNER_COMPONENTS_HORIZONTAL_SEPARATION, 
			CWTLWndComponentsOrganizer::LINE_ORIENTATION_CENTERED);
	m_componentsOrganizer.AddComponent(&m_keyAsterisk, m_keyAsterisk.GetWndClassName(), 
			m_multiStateKeyAsterisk.GetWidth(), 0, m_multiStateKeyAsterisk.GetHeight());
	m_componentsOrganizer.AddComponent(&m_key0, m_key0.GetWndClassName(), 
			m_multiStateKey0.GetWidth(), 0, m_multiStateKey0.GetHeight());
	m_componentsOrganizer.AddComponent(&m_keyPound, m_keyPound.GetWndClassName(), 
			m_multiStateKeyPound.GetWidth(), 0, m_multiStateKeyPound.GetHeight());
	
	return 0;
}


LRESULT CUOLFoneKeyboardCtrl::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_destroyWindowLock);
	
	m_destroyingWindow = TRUE;
	
	m_key0.DestroyWindow();
	m_key1.DestroyWindow();
	m_key2.DestroyWindow();
	m_key3.DestroyWindow();
	m_key4.DestroyWindow();
	m_key5.DestroyWindow();
	m_key6.DestroyWindow();
	m_key7.DestroyWindow();
	m_key8.DestroyWindow();
	m_key9.DestroyWindow();
	m_keyAsterisk.DestroyWindow();
	m_keyPound.DestroyWindow();
	
	return 0;
}


LRESULT CUOLFoneKeyboardCtrl::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_componentsOrganizer.OrganizeComponents();
	
	return 0L;
}

void CUOLFoneKeyboardCtrl::OnButtonClicked(UINT uCode, int nButtonId, HWND hWndCtrl)
{
	/*switch (nID)
	{
	case ID_BUTTON_KEY_0:
		NotifyAllFoneKeyboardPressed('0');
		break;
	case ID_BUTTON_KEY_1:
		NotifyAllFoneKeyboardPressed('1');
		break;
	case ID_BUTTON_KEY_2:
		NotifyAllFoneKeyboardPressed('2');
		break;
	case ID_BUTTON_KEY_3:
		NotifyAllFoneKeyboardPressed('3');
		break;
	case ID_BUTTON_KEY_4:
		NotifyAllFoneKeyboardPressed('4');
		break;
	case ID_BUTTON_KEY_5:
		NotifyAllFoneKeyboardPressed('5');
		break;
	case ID_BUTTON_KEY_6:
		NotifyAllFoneKeyboardPressed('6');
		break;
	case ID_BUTTON_KEY_7:
		NotifyAllFoneKeyboardPressed('7');
		break;
	case ID_BUTTON_KEY_8:
		NotifyAllFoneKeyboardPressed('8');
		break;
	case ID_BUTTON_KEY_9:
		NotifyAllFoneKeyboardPressed('9');
		break;
	case ID_BUTTON_KEY_ASTERISK:
		NotifyAllFoneKeyboardPressed('*');
		break;
	case ID_BUTTON_KEY_POUND:
		NotifyAllFoneKeyboardPressed('#');
		break;
	}*/

	NotifyAlfaNumericButtonChar(nButtonId);
}


LRESULT CUOLFoneKeyboardCtrl::OnChar(UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nChar(wParam);

	NotifyAllFoneKeyboardPressed(nChar);
	
	return 0L;
}

void CUOLFoneKeyboardCtrl::PressFoneKeyboardKey(UINT nChar)
{
	if ( this->IsWindowVisible() )
	{
		switch (nChar)
		{
		case '#' :
			m_multiStateKeyPound.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('#');
			break;

		case '*':
			m_multiStateKeyAsterisk.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('*');
			break;

		case '0':
			m_multiStateKey0.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('0');
			break;

		case '1':
		case '@':
		case '.':
		case '-':
		case '_':
			m_multiStateKey1.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('1');
			break;

		case '2' :
		case 'a' :
		case 'A' :
		case 'b' :
		case 'B' :
		case 'c' :
		case 'C' :
			m_multiStateKey2.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('2');
			break;

		case '3' :
		case 'd' :
		case 'D' :
		case 'e' :
		case 'E' :
		case 'f' :
		case 'F' :
			m_multiStateKey3.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('3');
			break;

		case '4' :
		case 'g' :
		case 'G' :
		case 'h' :
		case 'H' :
		case 'i' :
		case 'I' :
			m_multiStateKey4.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('4');
			break;

		case '5' :
		case 'j' :
		case 'J' :
		case 'k' :
		case 'K' :
		case 'l' :
		case 'L' :
			m_multiStateKey5.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('5');
			break;

		case '6' :
		case 'm' :
		case 'M' :
		case 'n' :
		case 'N' :
		case 'o' :
		case 'O' :
			m_multiStateKey6.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('6');
			break;

		case '7' :
		case 'p' :
		case 'P' :
		case 'q' :
		case 'Q' :
		case 'r' :
		case 'R' :
		case 's' :
		case 'S' :
			m_multiStateKey7.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('7');
			break;

		case '8' :
		case 't' :
		case 'T' :
		case 'u' :
		case 'U' :
		case 'v' :
		case 'V' :
			m_multiStateKey8.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('8');
			break;

		case '9' :
		case 'w' :
		case 'W' :
		case 'x' :
		case 'X' :
		case 'y' :
		case 'Y' :
		case 'z' :
		case 'Z' :
			m_multiStateKey9.SimulateButtonClick();
			CFacadeWrapper::GetInstance()->PlaySound('9');
			break;
		}
	}
}


void CUOLFoneKeyboardCtrl::SendButtonClickedMessage(WORD ctrlID, HWND ctrlHandle)
{
	WPARAM sendingWParam = 0;
	
	sendingWParam = MAKEWPARAM(ctrlID, BN_CLICKED);
	
	::SendMessage(m_hWnd, WM_COMMAND, sendingWParam, (LPARAM)ctrlHandle);
}


void CUOLFoneKeyboardCtrl::RegisterObserver(CUOLFoneKeyboardCtrlObserver* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION posFind = m_listObservers.Find(pObserver);

		if (!posFind)
		{
			m_listObservers.AddTail(pObserver);
		}
	}
}


void CUOLFoneKeyboardCtrl::UnregisterObserver(CUOLFoneKeyboardCtrlObserver* pObserver)
{
	if (pObserver != NULL)
	{
		POSITION posFind = m_listObservers.Find(pObserver);

		if (posFind != NULL)
		{
			m_listObservers.RemoveAt(posFind);
		}
	}
}


void CUOLFoneKeyboardCtrl::NotifyAllFoneKeyboardPressed(UINT nChar)
{
	CUOLFoneKeyboardCtrlObserver* pObserver;

	for (POSITION pos = m_listObservers.GetHeadPosition(); pos;)
	{
		pObserver = m_listObservers.GetNext(pos);

		pObserver->OnFoneKeyboardPressed(nChar, m_bCleanLast);
	}
}


void CUOLFoneKeyboardCtrl::UpdateAlfaNumericButtonControl(int nButtonId)
{
	if ((nButtonId == m_iLastAlfaNumericButton) && (DoesAlfaNumericButtonAllowSequence(nButtonId)))
	{
		DWORD dwTime = ::GetTickCount();
		if ((dwTime - m_dwAlfaNumericButtonTick) < 1000)
		{
			m_iAlfaNumericButtonSequence = GetNextAlfaNumericButtonSequence(nButtonId,
				m_iAlfaNumericButtonSequence);
			m_dwAlfaNumericButtonTick = ::GetTickCount();
			m_bCleanLast = TRUE;
		}
		else
		{
			m_iAlfaNumericButtonSequence = 0;
			m_dwAlfaNumericButtonTick = ::GetTickCount();
			m_bCleanLast = FALSE;
		}
	}
	else
	{
		m_iLastAlfaNumericButton = nButtonId;
		m_iAlfaNumericButtonSequence = 0;
		m_dwAlfaNumericButtonTick = ::GetTickCount();
		m_bCleanLast = FALSE;
	}
}


BOOL CUOLFoneKeyboardCtrl::DoesAlfaNumericButtonAllowSequence(int nButtonId)
{
	BOOL bAllow = FALSE;

	if (m_bIsAlfaNumeric)
	{
		switch (nButtonId)
		{
		case ID_BUTTON_KEY_1:
		case ID_BUTTON_KEY_2:
		case ID_BUTTON_KEY_3:
		case ID_BUTTON_KEY_4:
		case ID_BUTTON_KEY_5:
		case ID_BUTTON_KEY_6:
		case ID_BUTTON_KEY_8:
		case ID_BUTTON_KEY_7:
		case ID_BUTTON_KEY_9:
			bAllow = TRUE;
			break;
		default:
			bAllow = FALSE;
			break;
		}
	}

	return bAllow;
}


int CUOLFoneKeyboardCtrl::GetNextAlfaNumericButtonSequence(int nButtonId, int iSequence)
{
	int iResultSequence = 0;

	if (m_bIsAlfaNumeric)
	{
		switch (nButtonId)
		{
		case ID_BUTTON_KEY_1:
			if (iSequence < 5) 
			{
				iResultSequence = iSequence + 1;
			}
			break;
		case ID_BUTTON_KEY_2:
		case ID_BUTTON_KEY_3:
		case ID_BUTTON_KEY_4:
		case ID_BUTTON_KEY_5:
		case ID_BUTTON_KEY_6:
		case ID_BUTTON_KEY_8:
			if (iSequence < 3) 
			{
				iResultSequence = iSequence + 1;
			}
			break;
		case ID_BUTTON_KEY_7:
		case ID_BUTTON_KEY_9:
			if (iSequence < 4) 
			{
				iResultSequence = iSequence + 1;
			}
			break;
		}
	}

	return iResultSequence;
}


void CUOLFoneKeyboardCtrl::NotifyAlfaNumericButtonChar(int nButtonId)
{
	UpdateAlfaNumericButtonControl(nButtonId);
	int iSequence = m_iAlfaNumericButtonSequence;
	
	switch (nButtonId)
	{
	case ID_BUTTON_KEY_0:
		NotifyAllFoneKeyboardPressed('0');
		break;
	case ID_BUTTON_KEY_1:
		switch (iSequence)
		{
		case 0: 
			NotifyAllFoneKeyboardPressed('1');
			break;
		case 1: 
			NotifyAllFoneKeyboardPressed('@');
			break;
		case 2: 
			NotifyAllFoneKeyboardPressed('.');
			break;
		case 3: 
			NotifyAllFoneKeyboardPressed('-');
			break;
		case 4: 
			NotifyAllFoneKeyboardPressed('_');
			break;
		}
		break;
	case ID_BUTTON_KEY_2:
		switch (iSequence)
		{
		case 0: 
			NotifyAllFoneKeyboardPressed('2');
			break;
		case 1: 
			NotifyAllFoneKeyboardPressed('a');
			break;
		case 2: 
			NotifyAllFoneKeyboardPressed('b');
			break;
		case 3: 
			NotifyAllFoneKeyboardPressed('c');
			break;
		}
		break;
	case ID_BUTTON_KEY_3:
		switch (iSequence)
		{
		case 0: 
			NotifyAllFoneKeyboardPressed('3');
			break;
		case 1: 
			NotifyAllFoneKeyboardPressed('d');
			break;
		case 2: 
			NotifyAllFoneKeyboardPressed('e');
			break;
		case 3: 
			NotifyAllFoneKeyboardPressed('f');
			break;
		}
		break;
	case ID_BUTTON_KEY_4:
		switch (iSequence)
		{
		case 0: 
			NotifyAllFoneKeyboardPressed('4');
			break;
		case 1: 
			NotifyAllFoneKeyboardPressed('g');
			break;
		case 2: 
			NotifyAllFoneKeyboardPressed('h');
			break;
		case 3: 
			NotifyAllFoneKeyboardPressed('i');
			break;
		}
		break;
	case ID_BUTTON_KEY_5:
		switch (iSequence)
		{
		case 0: 
			NotifyAllFoneKeyboardPressed('5');
			break;
		case 1: 
			NotifyAllFoneKeyboardPressed('j');
			break;
		case 2: 
			NotifyAllFoneKeyboardPressed('k');
			break;
		case 3: 
			NotifyAllFoneKeyboardPressed('l');
			break;
		}
		break;
	case ID_BUTTON_KEY_6:
		switch (iSequence)
		{
		case 0: 
			NotifyAllFoneKeyboardPressed('6');
			break;
		case 1: 
			NotifyAllFoneKeyboardPressed('m');
			break;
		case 2: 
			NotifyAllFoneKeyboardPressed('n');
			break;
		case 3: 
			NotifyAllFoneKeyboardPressed('o');
			break;
		}
		break;
	case ID_BUTTON_KEY_7:
		switch (iSequence)
		{
		case 0: 
			NotifyAllFoneKeyboardPressed('7');
			break;
		case 1: 
			NotifyAllFoneKeyboardPressed('p');
			break;
		case 2: 
			NotifyAllFoneKeyboardPressed('q');
			break;
		case 3: 
			NotifyAllFoneKeyboardPressed('r');
			break;
		case 4: 
			NotifyAllFoneKeyboardPressed('s');
			break;
		}
		break;
	case ID_BUTTON_KEY_8:
		switch (iSequence)
		{
		case 0: 
			NotifyAllFoneKeyboardPressed('8');
			break;
		case 1: 
			NotifyAllFoneKeyboardPressed('t');
			break;
		case 2: 
			NotifyAllFoneKeyboardPressed('u');
			break;
		case 3: 
			NotifyAllFoneKeyboardPressed('v');
			break;
		}
		break;
	case ID_BUTTON_KEY_9:
		switch (iSequence)
		{
		case 0: 
			NotifyAllFoneKeyboardPressed('9');
			break;
		case 1: 
			NotifyAllFoneKeyboardPressed('w');
			break;
		case 2: 
			NotifyAllFoneKeyboardPressed('x');
			break;
		case 3: 
			NotifyAllFoneKeyboardPressed('y');
			break;
		case 4: 
			NotifyAllFoneKeyboardPressed('z');
			break;
		}
		break;
	case ID_BUTTON_KEY_ASTERISK:
		NotifyAllFoneKeyboardPressed('*');
		break;
	case ID_BUTTON_KEY_POUND:
		NotifyAllFoneKeyboardPressed('#');
		break;
	}		
}