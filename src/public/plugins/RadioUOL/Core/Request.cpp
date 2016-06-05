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
#include ".\request.h"

CRequest::CRequest(const CString& url):
	m_strUrl(url)
{
	ATLTRACE("%s %s\n",__FUNCTION__, url);
}

CRequest::~CRequest(void)
{
}


BOOL CRequest::ReadURL()
{
	if (m_strUrl.GetLength() == 0)
	{
		return FALSE;
	}

	CAtlHttpClient httpClient;
	
	if (httpClient.Navigate(m_strUrl))
	{	
		const BYTE* pByte = httpClient.GetBody();

		DWORD dwLen = httpClient.GetBodyLength();

		if (dwLen > 0)
		{	
			CAtlFile file;			

			m_strBody = pByte;

			return TRUE;
		}
	}
	
	CString tmp;
	
	int status = httpClient.GetStatus();

//	tmp.Format("Error code: %d ",status);

//	::MessageBox(NULL,tmp,"Error",MB_OK);

	ATLTRACE("%s http status code %d\n",__FUNCTION__, httpClient.GetStatus());

	return FALSE;
}


BOOL CRequest::Execute()
{
	if (ReadURL()==FALSE) 
	{
		return CRadioUOLError::HTTP_READ;
	}
	return Process();
}

BOOL CRequest::Process()
{
	return 0;
}




