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
#include "CustomEmoticon.h"
#include "CustomEmoticonManager.h"


#define				ID_ELEMENTS_SEPARATOR				"#"


CCustomEmoticon::CCustomEmoticon(IUOLMessengerAccountPtr pAccount, IUOLMessengerContactPtr pContact, 
								 const CString& strEmoticon, const CString& strRootPath) : 
		m_pAccount(pAccount),
		m_pContact(pContact),
		m_strEmoticon(strEmoticon),
		m_strRootPath(strRootPath),
		m_bIsReadyForUse(FALSE),
		m_bEmoticonPathCreated(FALSE),
		m_nEmoticonHeight(IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_HEIGHT),
		m_nEmoticonWidth(IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_WIDTH),
		m_bFileWriteOk(TRUE)
{
	CString strFileName = strEmoticon;
	CCustomEmoticonManager::EncodeShortcutToFileName(strFileName);
	
	m_strEmoticonPath.Format("%s\\%s\\%s\\%s", strRootPath, 
			pAccount->GetUserId(), pContact->GetName(), strFileName);
}


CCustomEmoticon::~CCustomEmoticon()
{
}


CString CCustomEmoticon::GetEmoticonShortcut()
{
	return m_strEmoticon;
}


IUOLMessengerAccountPtr CCustomEmoticon::GetEmoticonAccount()
{
	return m_pAccount;
}


IUOLMessengerContactPtr CCustomEmoticon::GetEmoticonContact()
{
	return m_pContact;
}


void CCustomEmoticon::GetEmoticonSize(CSize& sEmoticonSize)
{
	sEmoticonSize.SetSize(m_nEmoticonWidth, m_nEmoticonHeight);
}


int CCustomEmoticon::GetEmoticonHeight()
{
	return m_nEmoticonHeight;
}


int CCustomEmoticon::GetEmoticonWidth()
{
	return m_nEmoticonWidth;
}


BOOL CCustomEmoticon::IsEmotionReadyForUse()
{
	return m_bIsReadyForUse;
}


CString CCustomEmoticon::GetEmoticonPath()
{
	return m_strEmoticonPath;
}


BOOL CCustomEmoticon::CreateEmoticonFile()
{
	if ( (!m_strRootPath.IsEmpty()) && 
			(m_pAccount) && (!m_pAccount->GetUserId().IsEmpty()) && 
			(m_pContact) && (!m_pContact->GetName().IsEmpty()) && 
			(!m_strEmoticon.IsEmpty()) )
	{
		CString strEmoticonPath = m_strRootPath + "\\" + m_pAccount->GetUserId();
		
		if ( (ATLPath::IsDirectory(strEmoticonPath)) || (::CreateDirectory(strEmoticonPath, NULL)) )
		{
			strEmoticonPath += "\\" + m_pContact->GetName();
			
			if ( (ATLPath::IsDirectory(strEmoticonPath)) || (::CreateDirectory(strEmoticonPath, NULL)) )
			{
				m_bEmoticonPathCreated = TRUE;
				
				return m_bEmoticonPathCreated;
			}
		}
	}
	
	return FALSE;
}


BOOL CCustomEmoticon::UpdateEmoticonData(const unsigned char* pEmoticonData, 
		unsigned int nEmoticonDataSize, const CString& strFileExtension)
{
	m_bFileWriteOk = FALSE;
	
	if (m_bEmoticonPathCreated)
	{
		if (strFileExtension.IsEmpty())
		{
			m_strEmoticonPath += ".gif";
		}
		else
		{
			m_strEmoticonPath += "." + strFileExtension;
		}
		
		if ((m_fileEmoticon = fopen(m_strEmoticonPath, "ab")) != NULL)
		{
			// Point second and third parameters on the inverse order to make a more 
			// compreensive comparition.
			//
			if (fwrite(pEmoticonData, 1, nEmoticonDataSize, m_fileEmoticon) == nEmoticonDataSize)
			{
				m_bFileWriteOk = TRUE;
			}
		}
	}
	
	return m_bFileWriteOk;
}


// Closes the file storing the new custom emoticon, and sets the flag that 
// indicates that it's ready for use.
//
BOOL CCustomEmoticon::StoreEmoticon()
{
	if (m_bFileWriteOk)
	{
		m_bIsReadyForUse = m_bFileWriteOk;
		
		fflush(m_fileEmoticon);
		fclose(m_fileEmoticon);
	}
	
	return m_bIsReadyForUse;
}


void CCustomEmoticon::SetEmoticonHeight(int nEmoticonHeight)
{
	m_nEmoticonHeight = nEmoticonHeight;
}


void CCustomEmoticon::SetEmoticonWidth(int nEmoticonWidth)
{
	m_nEmoticonWidth = nEmoticonWidth;
}


BOOL CCustomEmoticon::DestroyPermanently()
{
	BOOL bDeleted = ::DeleteFile(m_strEmoticonPath);
	
	if (bDeleted)
	{
		m_bIsReadyForUse = FALSE;
	}
	
	return bDeleted;
}


CString CCustomEmoticon::BuildEmoticonId(IUOLMessengerAccountPtr pAccount, IUOLMessengerContactPtr pContact, 
										 const CString& strEmoticon)
{
	CString strEmoticonId;
	
	if ( (pAccount) && (pContact) && (!strEmoticon.IsEmpty()) )
	{
		strEmoticonId.Format("%s%s%s%s%s", pAccount->GetUserId(), ID_ELEMENTS_SEPARATOR, 
				pContact->GetName(), ID_ELEMENTS_SEPARATOR, strEmoticon);
	}
	
	return strEmoticonId;
}


CString CCustomEmoticon::BuildEmoticonOwnerId(IUOLMessengerAccountPtr pAccount, IUOLMessengerContactPtr pContact)
{
	CString strEmoticonOwnerId;
	
	if ( (pAccount) && (pContact) )
	{
		strEmoticonOwnerId.Format("%s%s%s", pAccount->GetUserId(), ID_ELEMENTS_SEPARATOR, 
				pContact->GetName());
	}
	
	return strEmoticonOwnerId;
}


CString CCustomEmoticon::BuildEmoticonId(IUOLMessengerCustomEmoticonPtr pCustomEmoticon)
{
	return BuildEmoticonId(pCustomEmoticon->GetEmoticonAccount(), 
			pCustomEmoticon->GetEmoticonContact(), pCustomEmoticon->GetEmoticonShortcut());
}


CString CCustomEmoticon::BuildEmoticonOwnerId(IUOLMessengerCustomEmoticonPtr pCustomEmoticon)
{
	return BuildEmoticonOwnerId(pCustomEmoticon->GetEmoticonAccount(), 
			pCustomEmoticon->GetEmoticonContact());
}


