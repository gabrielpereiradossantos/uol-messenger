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
#include ".\requestchannels.h"

CRequestChannels::CRequestChannels(const CString& url, CAtlList<CStyle*>& styles ):
	CRequest(url),
	m_styles(styles)
{
	m_strUrl = url;
}

CRequestChannels::~CRequestChannels(void)
{
	//m_styles.RemoveAll();
}

BOOL CRequestChannels::Process()
{
	//Sleep(60000);
	ATLTRACE("%s url %s\n",__FUNCTION__,m_strUrl );

	// Remove os estilos antes de prosseguir
	for (POSITION pos = m_styles.GetHeadPosition(); pos; )
	{
		CStyle* pStyle = m_styles.GetNext(pos);
		ATLASSERT(pStyle!=NULL);
		delete pStyle;
	}
	m_styles.RemoveAll();

	CAtlString str( m_strBody);
	CAtlString resToken;
	int curPos= 0;
	CStyle* pStyle=NULL;
	resToken= str.Tokenize(";",curPos);
	while (resToken != "")
	{
		resToken.Trim();
		//printf("Resulting token: %s\n", resToken);

		if (resToken.Find("estilo")==0) 
		{
			CString stylename = resToken;
			int ipos = stylename.Find('\"');
			int lpos = stylename.ReverseFind('\"');
			if (lpos>ipos) 
			{
				stylename = stylename.Mid(ipos+1, lpos-ipos-1);
			}
			else
			{
				ATLASSERT(FALSE);
			}
			pStyle = new CStyle(stylename);
			m_styles.AddTail(pStyle);

			// novo estilo
		}
		else if (resToken.Find("canal")==0)
		{
			ATLASSERT(pStyle!=NULL);


			CString canais = resToken;
			int ipos = canais.Find('(');
			int lpos = canais.ReverseFind(')');
			if (lpos>ipos) 
			{
				canais = canais.Mid(ipos+1, lpos-ipos-1);
			}
			else
			{
				ATLASSERT(FALSE);
			}


			CAtlString str( canais );
			CAtlString resToken;
			int curPos= 0;

			resToken= str.Tokenize(",",curPos);
			while (resToken != "")
			{
				CString canalName;
				CString canalId;

				int ipos = resToken.Find('|');
				canalName = resToken.Mid(1,ipos-1);
				canalId = resToken.Mid(ipos+1);
				ATLASSERT(canalName!="");
				ATLASSERT(canalId!="");
				pStyle->AddChannel(canalName,canalId);
				resToken= str.Tokenize(",",curPos);
			};
		} 

		resToken= str.Tokenize(";",curPos);
	};
	return 0;
}

