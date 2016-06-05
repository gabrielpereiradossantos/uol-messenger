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
 
#include "purpleutils.h"

#include <glib.h>

using namespace std;



int GetLocaleTextFromUTF8(const char* pszUTF8Text, std::string& strLocaleText)
{
	int nRet = FALSE;

	if (pszUTF8Text != NULL)
	{
		char* pszText = g_strdup(pszUTF8Text);
		char* pszLocaleText = NULL;
		
		bool bTryAgain = false;

		do 
		{
			gsize pBytesRead;
			GError* pError = NULL;

			pszLocaleText = g_locale_from_utf8(pszText, -1, &pBytesRead, NULL, &pError);

			if (pError != NULL)
			{
				if (pError->code == G_CONVERT_ERROR_ILLEGAL_SEQUENCE)
				{
					// Remove invalid UTF8 char and try again
					std::string strText = pszText;
					string::size_type npos = (int)pBytesRead;
					strText = strText.erase(npos, 1);
					g_free(pszText);
					pszText = g_strdup(strText.c_str());
										
					bTryAgain = true;
				}
				g_error_free(pError);
			}
			else
			{
				bTryAgain = false;
			}
		} while (bTryAgain);

		if (pszLocaleText != NULL)
		{
			strLocaleText = pszLocaleText;
			g_free(pszLocaleText);

			nRet = TRUE;
		}

		if (pszText != NULL)
		{
			g_free(pszText);		
		}
	}

	return nRet;
}


std::string UnescapeHTML(const char* pszHTML)
{
	string strUnescaped = pszHTML;

	if (pszHTML != NULL)
	{
		const char *c = pszHTML;
		string strTemp;

		while (*c != NULL)
		{
			if (!strncmp(c, "&amp;", 5))
			{
				strTemp += '&';
				c += 5;
			} 
			else if (!strncmp(c, "&lt;", 4))
			{
				strTemp += '<';
				c += 4;
			}
			else if (!strncmp(c, "&gt;", 4))
			{
				strTemp += '>';
				c += 4;
			}
			else if (!strncmp(c, "&quot;", 6))
			{
				strTemp += '"';
				c += 6;
			}
			else if (!strncmp(c, "&apos;", 6))
			{
				strTemp += '\'';
				c += 6;
			}
			else if (!strncmp(c, "<br>", 4))
			{
				strTemp += '\n';
				c += 4;
			}
			else if (!strncmp(c, "&nbsp;", 6))
			{
				strTemp += ' ';
				c += 6;
			}
			else
			{
				strTemp += *c;
				c++;
			}
		}

		strUnescaped = strTemp;
	}

	return strUnescaped;
}


int ExtractSubstring(const char* pszText, const char* pszLeftToken, const char* pszRightToken, std::string& strResult)
{
	if (pszText != NULL)
	{
		string strText = pszText;

		string::size_type npos = -1;
		string::size_type nLeftPos;
		string::size_type nRightPos;
		size_t nLeftTokenLength = 0;

		// Search for left token
		if (pszLeftToken != NULL)
		{
			nLeftPos = strText.find(pszLeftToken, 0);
			if (nLeftPos == npos)
			{
				// Left token not found
				return FALSE;
			}

			nLeftTokenLength = strlen(pszLeftToken);
		}
		else
		{
			// No left token
			nLeftPos = 0;
		}

		// Search for right token
		if (pszRightToken != NULL)
		{
			nRightPos = strText.find(pszRightToken, nLeftPos + nLeftTokenLength);
			if (nRightPos != npos)
			{
				// Extract substring between tokens
				strResult = strText.substr(nLeftPos + nLeftTokenLength, nRightPos - (nLeftPos + nLeftTokenLength));
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			// Extract substring between left token and string end
			strResult = strText.substr(nLeftPos + nLeftTokenLength, strText.length() - (nLeftPos + nLeftTokenLength));
		}
	}

	return TRUE;
}