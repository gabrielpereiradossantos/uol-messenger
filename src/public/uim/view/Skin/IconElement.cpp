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
#include ".\iconelement.h"

#include "../../controller/uimapplication.h"

CIconElement::CIconElement(void) :
	m_size(16, 16)
{
}

CIconElement::~CIconElement(void)
{
}

void CIconElement::DrawIcon(CDCHandle& dc, int x, int y)
{
	if (m_icon)
	{
		m_icon.DrawIconEx(dc, x, y, m_size.cx, m_size.cy);
	}
}

CSize CIconElement::GetSize() const
{
	return m_size;
}

void CIconElement::SetSize(const CSize& size)
{
	m_size = size;
}

void CIconElement::LoadIcon(const CString& strThemeBasedPath)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	if (m_icon.m_hIcon)
	{
		m_icon.DestroyIcon();
	}

	CString strFullFilePath;

	IUOLMessengerUIManager3Ptr pUIManager3 = ap_dynamic_cast<IUOLMessengerUIManager3Ptr>(CUIMApplication::GetApplication()->GetUIManager());

	if (pUIManager3)
	{
		IUOLMessengerThemePropertiesPtr pThemeProperties = pUIManager3->GetCurrentThemeProperties();

		if (pThemeProperties)
		{
			strFullFilePath = pThemeProperties->GetDirPath() + _T("\\") + strThemeBasedPath;
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"Error getting pThemeProperties.");
		}
	}
	else
	{
		static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
				GetLogger()->LogError(__FUNCTION__, 
				"Error getting pUIManager3.");
	}

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Got icon full path = [%s]. Now creates the icon.", strFullFilePath);
	
	BOOL bChooseIcon = FALSE;

	if ((m_size.cx == 16) && (m_size.cy == 16))
	{
		bChooseIcon = ChooseTheRightOne(strFullFilePath, FALSE);
	}
	else
	{
		bChooseIcon = ChooseTheRightOne(strFullFilePath, TRUE);
	}
	
	ATLASSERT(bChooseIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, "bChooseIcon = [%d].", bChooseIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

CIconHandle CIconElement::GetIcon() const
{
	return CIconHandle(m_icon);
}

CIconElement::operator CIconHandle() const
{
	return GetIcon();
}

void CIconElement::SetParameters(CString2StringMapPtr& pMapParams, IElementTable* pElementTable)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	__super::SetParameters(pMapParams, pElementTable);

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogInfo(__FUNCTION__, "Getting icon height.");
	
	try
	{
		GetParameterValue(m_size.cy, pMapParams, "height");		
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "height");
	}
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Got icon height = [%d]. Now get the icon width.", m_size.cy);
	
	try
	{
		GetParameterValue(m_size.cx, pMapParams, "width");		
	}
	catch(const CSkinException&)
	{
		ATLTRACE(_T("%s - %s\n"), __FUNCTION__, "width");
	}

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Got icon width = [%d]. Now get the icon file path.", m_size.cx);
	
	CString strFile;
	GetParameterValue(strFile, pMapParams, "file");

	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Got icon relative path = [%s]. Now loads the icon.", strFile);
	
	LoadIcon(strFile);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
}

BOOL CIconElement::ChooseTheRightOne(const CString strFullFilePath, BOOL bLarge)
{
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogStartMethod(__FUNCTION__);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"bLarge = [%d], m_size.cx = [%d], m_size.cy = [%d].", 
			bLarge, m_size.cx, m_size.cy);
	
	HICON hIcon = (HICON)::LoadImage(NULL, strFullFilePath, IMAGE_ICON, 
			m_size.cx, m_size.cy, LR_LOADFROMFILE);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"Icon hIcon = [%x]", hIcon);
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"System small icon size -> cx = [%d], cy = [%d].", 
			GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
	
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogDebug(__FUNCTION__, 
			"System large icon size -> cx = [%d], cy = [%d].", 
			GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	
	if (hIcon)
	{
		ICONINFO iconInfo = {0};
		if (::GetIconInfo(hIcon, &iconInfo))
		{
			CBitmapHandle bmp;
			bmp.Attach(iconInfo.hbmColor);
			
			CSize size(0, 0);
			bmp.GetSize(size);
			
			bmp.Detach();
			
			if (m_size == size)
			{
				m_icon.Attach(hIcon);
				
				return TRUE;
			}
			else
			{
				static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
						GetLogger()->LogError(__FUNCTION__, 
						"Different sizes -> m_size.cx = [%d], m_size.cy = [%d], size.cx = [%d], size.cy = [%d]", 
						m_size.cx, m_size.cy, size.cx, size.cy);
			}
		}
		else
		{
			static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
					GetLogger()->LogError(__FUNCTION__, 
					"Failure running GetIconInfo result = [%d]", ::GetLastError());
		}
		
		::DestroyIcon(hIcon);
	}
	
	ATLASSERT(m_icon);
		
	static_cast<CUIMApplication*>(CUIMApplication::GetApplication())->
			GetLogger()->LogEndMethod(__FUNCTION__);
	
	return FALSE;
}