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
 

#include "stdafx.h"
#include "ComFacade.h"


CComFacade::~CComFacade()
{
}


CComFacade* CComFacade::GetInstance()
{
	static CComFacade facadeInstance;
	
	return &facadeInstance;
}


BOOL CComFacade::Initialize(const CString& strUOLFoneClientDllPath, const CString& strUOLFoneServerConfigProviderDllPath)
{
	BOOL bInitialized = FALSE;

	BOOL bLoad = FALSE;

	bLoad = m_dllUOLFoneClient.LoadDll(strUOLFoneClientDllPath);

	ATLASSERT(bLoad);
	if (bLoad)
	{
		bLoad = m_dllUOLFoneServerConfigProvider.LoadDll(strUOLFoneServerConfigProviderDllPath);

		ATLASSERT(bLoad);
		if (bLoad)
		{
			HRESULT hr = ::CoInitialize(NULL);

			if (SUCCEEDED(hr))
			{
				hr = m_dllUOLFoneClient.CoCreateInstance(__uuidof(CUOLFoneClient), __uuidof(IUOLFoneClient), reinterpret_cast<LPVOID*>(&m_spUOLFoneClient));
				ATLASSERT(SUCCEEDED(hr));

				if (SUCCEEDED(hr))
				{
					hr = m_dllUOLFoneServerConfigProvider.CoCreateInstance(__uuidof(CUOLFoneServerConfigProvider), __uuidof(IUOLFoneServerConfigProvider), reinterpret_cast<LPVOID*>(&m_spUOLFoneServerConfigProvider));
					ATLASSERT(SUCCEEDED(hr));
					
					if (FAILED(hr))
					{
						m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
								CUOLFoneFailureInfo::LIBRARY_CREATE_UOLFONESERVERCONFIG_FAILURE, hr);
						
						if (m_pLastFailureInfo)
						{
							m_pLastFailureInfo->AddExtraFailureInfo(
									CUOLFoneFailureInfo::EXTRA_INFO_PATH, 
									strUOLFoneServerConfigProviderDllPath);
						}
					}
					
					bInitialized = TRUE;
				}
				else
				{
					m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
							CUOLFoneFailureInfo::LIBRARY_CREATE_UOLFONECLIENT_FAILURE, hr);
					
					if (m_pLastFailureInfo)
					{
						m_pLastFailureInfo->AddExtraFailureInfo(
								CUOLFoneFailureInfo::EXTRA_INFO_PATH, 
								strUOLFoneClientDllPath);
					}
				}
			}
			else
			{
				m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
						CUOLFoneFailureInfo::LIBRARY_COINITIALIZE_FAILURE, hr);
			}
		}
		else
		{
			m_dllUOLFoneClient.UnloadDll();
			
			m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
					CUOLFoneFailureInfo::LIBRARY_LOAD_UOLFONESERVERCONFIG_FAILURE, 
					m_dllUOLFoneServerConfigProvider.GetLastLoadError());
			
			if (m_pLastFailureInfo)
			{
				m_pLastFailureInfo->AddExtraFailureInfo(
						CUOLFoneFailureInfo::EXTRA_INFO_PATH, 
						strUOLFoneServerConfigProviderDllPath);
			}
		}
	}
	else
	{
		m_pLastFailureInfo = CUOLFoneFailureInfo::BuildLibraryFailure(
				CUOLFoneFailureInfo::LIBRARY_LOAD_UOLFONECLIENT_FAILURE, 
				m_dllUOLFoneClient.GetLastLoadError());
		
		if (m_pLastFailureInfo)
		{
			m_pLastFailureInfo->AddExtraFailureInfo(
					CUOLFoneFailureInfo::EXTRA_INFO_PATH, 
					strUOLFoneClientDllPath);
		}
	}

	return bInitialized;
}


void CComFacade::Finalize()
{
	if (m_spUOLFoneClient)
	{
		m_spUOLFoneClient.Release();
	}

	if (m_spUOLFoneServerConfigProvider)
	{
		m_spUOLFoneServerConfigProvider.Release();
	}

	::CoUninitialize();

	m_dllUOLFoneClient.UnloadDll();
	m_dllUOLFoneServerConfigProvider.UnloadDll();
}


HRESULT CComFacade::GetUOLFoneClientInstance(IUOLFoneClient** ppUOLFoneClient)
{
	if (ppUOLFoneClient)
	{
		*ppUOLFoneClient = m_spUOLFoneClient;
		
		if (*ppUOLFoneClient)
		{
			(*ppUOLFoneClient)->AddRef();

			return S_OK;
		}
	}

	return E_FAIL;
}


HRESULT CComFacade::GetUOLFoneServerConfigProviderInstance(IUOLFoneServerConfigProvider** ppUOLFoneServerConfigProvider)
{
	if (ppUOLFoneServerConfigProvider)
	{
		*ppUOLFoneServerConfigProvider = m_spUOLFoneServerConfigProvider;

		if (*ppUOLFoneServerConfigProvider)
		{
			(*ppUOLFoneServerConfigProvider)->AddRef();

			return S_OK;
		}
	}

	return E_FAIL;
}


CUOLFoneFailureInfoPtr CComFacade::GetLastFailureInfo()
{
	return m_pLastFailureInfo;
}


CComFacade::CComFacade()
{
}