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
#include "HTMLFilter.h"


CString CHTMLFilter::HTMLToText(const CString& strHTML)
{
	CString strText;

	TCHAR* lpszHTML = (LPTSTR) (LPCTSTR) strHTML;

	while (*lpszHTML != NULL)
	{
		if (*lpszHTML == '<')
		{				
			++lpszHTML;

			if (0 == ::strnicmp(lpszHTML, "br>", 3))
			{
				// Replace <br> with \n
				strText += '\n';
				lpszHTML += 3;
			}
			else
			{
				// Discard html tags...
				while ((*lpszHTML != '\0') && (*lpszHTML != '>'))
				{
					++lpszHTML;
				}

				if (*lpszHTML == '>')
				{
					++lpszHTML;
				}
			}
		}
		else if (0 == ::strnicmp(lpszHTML, "&nbsp;", 6))
		{
			// Replace &nbsp; with ' '
			strText += ' ';
			lpszHTML += 6;
		}
		else if (0 == strnicmp(lpszHTML, "&amp;", 5))
		{
			strText += '&';
			lpszHTML += 5;
		}
		else if (0 == strnicmp(lpszHTML, "&lt;", 4))
		{
			strText += '<';
			lpszHTML += 4;
		}
		else if (0 == strnicmp(lpszHTML, "&gt;", 4))
		{
			strText += '>';
			lpszHTML += 4;
		}
		else if (0 == strnicmp(lpszHTML, "&quot;", 6))
		{
			strText += '"';
			lpszHTML += 6;
		} 
		else if (0 == strnicmp(lpszHTML, "&apos;", 6))
		{
			strText += '\'';
			lpszHTML += 6;
		}
		else
		{
			strText += *lpszHTML;
			++lpszHTML;
		}
	}

	return strText;
}


CString CHTMLFilter::TextToHTML(const CString& strText)
{
	CString strHTML = EscapeHTML(strText);

	return strHTML;
}


BOOL CHTMLFilter::IsBadHTMLChar(TCHAR ch)
{
	switch (ch) 
	{
		case ' ':
		case ',':
		case '\0':
		case '\n':
		case '<':
		case '>':
		case '"':
		case '\'':
			return TRUE;
		default:
			return FALSE;
	}
}


BOOL CHTMLFilter::IsBadEntity(LPCTSTR lpszText)
{
	if ((0 == ::_tcsnicmp(lpszText, "&lt;", 4)) ||
		(0 == ::_tcsnicmp(lpszText, "&gt;", 4)) ||
		(0 == ::_tcsnicmp(lpszText, "&quot;", 6)))
	{
		return TRUE;
	}

	return FALSE;
}


CString CHTMLFilter::HTMLUnescape(const CString& strHTML)
{
	CString strUnescaped;

	LPCSTR lpszHTML = (LPCTSTR) strHTML;

	while (*lpszHTML)
	{
		if (0 == ::_tcsnicmp(lpszHTML, "&amp;", 5))
		{
			strUnescaped += '&';
			lpszHTML += 5;
		}
		else if (0 == ::_tcsnicmp(lpszHTML, "&lt;", 4))
		{
			strUnescaped += '<';
			lpszHTML += 4;
		}
		else if (0 == ::_tcsnicmp(lpszHTML, "&gt;", 4))
		{
			strUnescaped += '>';
			lpszHTML += 4;
		}
		else if (0 == ::_tcsnicmp(lpszHTML, "&quot;", 6))
		{
			strUnescaped += '"';
			lpszHTML += 6;
		}
		else if (0 == ::_tcsnicmp(lpszHTML, "&apos;", 6))
		{
			strUnescaped += '\'';
			lpszHTML += 6;
		}
		else if (0 == ::_tcsnicmp(lpszHTML, "<br>", 4))
		{
			strUnescaped += '\n';
			lpszHTML += 4;
		}
		else
		{
			strUnescaped += *lpszHTML;
			lpszHTML++;
		}
	}

	return strUnescaped;
}


CString CHTMLFilter::EscapeHTML(const CString& strHTML)
{
	int nNonSpaceCount = 0;

	LPCTSTR lpszHTML = strHTML;
	CString strRet;

	while (*lpszHTML)
	{
		if (*lpszHTML == '&')
		{
			strRet += "&amp;";
			nNonSpaceCount++;
		}
		else if (*lpszHTML == '<')
		{
			strRet += "&lt;";
			nNonSpaceCount++;
		}
		else if (*lpszHTML == '>')
		{
			strRet += "&gt;";
			nNonSpaceCount++;
		}
		else if (*lpszHTML == '"')
		{
			strRet += "&quot;";
			nNonSpaceCount++;
		}
		else if ((*lpszHTML == '\n') || (*lpszHTML == '\r'))
		{
			strRet += "<br>";
		}
		else if (*lpszHTML == ' ')
		{
			if (nNonSpaceCount == 0)
			{
				strRet += "&nbsp;";
			}
			else
			{
				strRet += *lpszHTML;
			}
		}
		else
		{
			strRet += *lpszHTML;
			nNonSpaceCount++;
		}

		lpszHTML++;
	}

	return strRet;
}

