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
#include "UIMCryptoPlugin.h"
#include "resource.h"

#include "FacadeWrapper.h"
#include "CryptoProtocolController.h"
#include "MessageWindowMonitor.h"
#include "ConfigDlg.h"


#define UMSDK_FILE_MAJOR		1
#define UMSDK_FILE_MINOR		4
#define UMSDK_BUILD_MAJOR		2
#define UMSDK_BUILD_MINOR		0


extern "C"
{
	__declspec(dllexport)
	CUOLMessengerPlugin* AllocUOLMessengerPlugin(IUOLMessengerFacade* pUOLMessengerFacade)
	{
		CFacadeWrapper::GetInstance()->Init(pUOLMessengerFacade);
		return new CUIMCryptoPlugin(pUOLMessengerFacade);
	}

	__declspec(dllexport)
	void FreeUOLMessengerPlugin(CUOLMessengerPlugin* pUOLMessengerPlugin)
	{
		CFacadeWrapper::GetInstance()->Finalize();

		ATLASSERT(FALSE == IsBadReadPtr(pUOLMessengerPlugin, sizeof(CUOLMessengerPlugin*)));
		delete pUOLMessengerPlugin;
	}
}






CUIMCryptoPlugin::CUIMCryptoPlugin(IUOLMessengerFacade* pUOLMessengerFacade) :
	CUOLMessengerPlugin(pUOLMessengerFacade)
{
	m_pIcon = m_pUOLMessengerFacade->GetUIManager()->LoadImage(ATL::_AtlBaseModule.GetResourceInstance(), IDB_PLUGIN_ICON);
}

CUIMCryptoPlugin::~CUIMCryptoPlugin(void)
{
	m_pIcon.reset();
}


BOOL CUIMCryptoPlugin::Init(ULONGLONG llUOLMessengerVersion)
{	
	ASSERT_THREAD_SAFE_FUNC;

	ULONG* pVersion = reinterpret_cast<ULONG*>(&llUOLMessengerVersion);

	ULONG lFileVersion = MAKELONG(UMSDK_FILE_MINOR, UMSDK_FILE_MAJOR);
	ULONG lBuildVersion = MAKELONG(UMSDK_BUILD_MINOR, UMSDK_BUILD_MAJOR);

	if ((pVersion[1] < lFileVersion) ||
		((pVersion[1] == lFileVersion) && (pVersion[0] < lBuildVersion)))
	{
		return FALSE;
	}

	if (CCryptoProtocolController::GetInstance()->Init(m_pUOLMessengerFacade))
	{
		return CMessageWindowMonitor::GetInstance()->Init(m_pUOLMessengerFacade);
	}

	return FALSE;
}


void CUIMCryptoPlugin::Finalize()
{
	ASSERT_THREAD_SAFE_FUNC;

	CMessageWindowMonitor::GetInstance()->Finalize();
	CCryptoProtocolController::GetInstance()->Finalize();
}


CString CUIMCryptoPlugin::GetInternalName() const
{
	return "uolcrypto";
}

ULONGLONG CUIMCryptoPlugin::GetVersion() const
{
	return CAppVersion(ATL::_AtlBaseModule.GetModuleInstance()).GetVersion();
}

CString CUIMCryptoPlugin::GetDesc() const
{
	return _T("Plugin de criptografia de mensagens");
}

CString CUIMCryptoPlugin::GetName() const
{
	return _T("Criptografia de Mensagens");
}


IUOLMessengerImagePtr CUIMCryptoPlugin::GetIcon() const
{
	return m_pIcon;
}

BOOL CUIMCryptoPlugin::HasConfigDialog()
{
	return TRUE;
}

void CUIMCryptoPlugin::ShowConfigDialog()
{
	CConfigDlgPtr pConfigDlg = new CConfigDlg();
	pConfigDlg->DoModal();
}
