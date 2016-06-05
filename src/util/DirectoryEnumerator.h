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
 

#pragma once

class CDefaultMatchCriteria
{
public:
	BOOL Match(LPWIN32_FIND_DATA) const
	{
		return TRUE;
	}
};

class CFileCriteria
{
public:
	// Retorna TRUE se o elemento apontado por pFD
	// for um arquivo
	BOOL Match(LPWIN32_FIND_DATA pFD) const
	{
		return pFD->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE;
	};
};


class CDirectoryCriteria
{
public:
	// Retorna TRUE se o elemento apontado por pFD
	// for um diretório
	BOOL Match(LPWIN32_FIND_DATA pFD) const
	{
		return (pFD->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			   (pFD->cFileName != CString(_T("."))) && 
			   (pFD->cFileName != CString(_T("..")));
	};
};

/* 
	Classe que enumera todos o sub-itens de um diretório
de acordo com um critério definino.

	Existem 3 critérios pré-definidos:

		CDefaultMatchCriteria:
			Usado por default por CDirectoryEnumerator::GetItems(), todos os 
			elementos do diretório serão retornados.

		CFileCriteria
			Usado por CDirectoryEnumerator::GetFiles(), apenas os arquivos do
            diretório serão retornados.

		CDirectoryCriteria
			Usado por CDirectoryEnumerator::GetDirectories(), apenas os sub-diretórios 
			do diretório serão retornados.

	Criando um critério customizado:

	O seguinte código cria um critério por tamanho, match retornará
	TRUE se o tamanho do arquivo apontado por pFD  for >= m_nSize

	template<class T64bits> // T64bits suporta 64 de dados						
	class CSizeGreaterCriteria
	{
	private:
		T64bits m_nSize;

		inline T64bits MakeSize(DWORD dwFileSizeHigh, DWORD dwFileSizeLow) const
		{		
			return static_cast<T64bits>(dwFileSizeHigh) << (sizeof(DWORD) * 8)) | static_cast<T64bits>(dwFileSizeLow);
		}
		
	public:		
		CSizeGreaterCriteria(T64bits nSize)
			:m_nSize(nSize)
		{		
		}

		BOOL Match(LPWIN32_FIND_DATA pFD) const
		{
			return ((pFD->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					(MakeSize(pFD->nFileSizeHigh, pFD->nFileSizeLow) >= m_nSize));			
		};
	};

	Para usar:

	class CUseCriteria
	{
	public:
		static void Use()
		{
			CAtlArray<CString> array;

			CDirectoryEnumerator::GetItems(array); 
												// retorna em array todos o arquivos/diretórios 
												// do diretório corrente.

			CDirectoryEnumerator::GetItems(array, _T("w*")); 
												// retorna em array todos o arquivos/diretórios 
												// que começam com a letra w do diretório corrente.
		
			CDirectoryEnumerator::GetDirectories(array, _T("*.*"), _T("c:\\windows")); 
												// retorna em array todos o sub-diretórios de
												// c:\windows

			CDirectoryEnumerator::GetItems(array, CSizeGreaterCriteria<__int64>(1000),
										   _T("*.*"), _T("c:\\windows")); 
												// retorna em array todos o arquivos com tamanho >= 1000
												// do diretório c:\windows.
		}
	};
*/
class CDirectoryEnumerator
{
public:
	static void GetFiles(CAtlList<CString>& listFiles,
						 const CString& strSearchCriteria = _T("*.*"), 
					     const CString& strBaseDir = _T(""))
	{
		return GetItems(listFiles, CFileCriteria(), strSearchCriteria, strBaseDir);
	}

	static void GetDirectories(CAtlList<CString>& listDirs,
							   const CString& strSearchCriteria = _T("*.*"),
							   const CString& strBaseDir = _T(""))
	{
		return GetItems(listDirs, CDirectoryCriteria(), strSearchCriteria, strBaseDir);
	}

	static void GetItems(CAtlList<CString>& listItens,
						 const CString& strSearchCriteria = _T("*.*"), 
						 const CString& strBaseDir = _T(""))
	{
		return GetItems(listItens, CDefaultMatchCriteria(), strSearchCriteria, strBaseDir);
	}


	template <class MatchCriteria>
	static void GetItems(CAtlList<CString>& listItens,
						 const MatchCriteria& matchCriteria,
						 const CString& strSearchCriteria = _T("*.*"), 
						 const CString& strBaseDir = _T(""))
	{
		listItens.RemoveAll();
		
		CString strFileName;
		
		if (FALSE == strBaseDir.IsEmpty())
		{
			strFileName = strBaseDir + _T("\\"); 
		}

		strFileName += strSearchCriteria;


		WIN32_FIND_DATA wfd;		
		HANDLE hFind = ::FindFirstFile(strFileName , &wfd);

		if (INVALID_HANDLE_VALUE == hFind)
		{
			return;
		}

		do
		{			
			if (matchCriteria.Match(&wfd))
			{
				listItens.AddTail(wfd.cFileName);
			}

		} while (::FindNextFile(hFind, &wfd));


		::FindClose(hFind);
	}
};


