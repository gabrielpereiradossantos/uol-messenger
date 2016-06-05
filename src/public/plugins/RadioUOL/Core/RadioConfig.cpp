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

#include "util.h"
#include "radioconfig.h"
#include "../resource.h"
#include "../facadeWrapper.h"

#include <atlpath.h>

int CRadioConfig::LoadConfig()
{
	CString filePath = GetConfigFilePath();
	ATLASSERT(filePath.IsEmpty()==FALSE);

	if (filePath.IsEmpty())
	{
		return CRadioUOLError::CONFIG_FILE;
	}

	CString url;
	url.LoadString(IDS_CONFIG_XML_URL);

	//Verifica se deve pegar novo arquivo de configuração
	if (TRUE == HasNewConfig(filePath))
	{
		int nResult;
		if ((nResult=DownloadFile(url,filePath))!=0)
		{
			switch (nResult)
			{
			case 1:
				//return CRadioUOLError::UPDATERADIOUOLXML;
				break;
			case 2:
				return CRadioUOLError::SAVECONFIG;
				break;
			case 3:
				//return CRadioUOLError::INTERNETOFF;
				break;
			}
		}
	}

	// Se não conseguir ler arquivo de configuração, tenta pegar novo do servidor
	if (FALSE == LoadConfigFile(filePath))
	{
		int nResult;
		if ((nResult=DownloadFile(url,filePath))!=0)
		{
			switch (nResult)
			{
			case 1:
				return CRadioUOLError::UPDATERADIOUOLXML;
				break;
			case 2:
				return CRadioUOLError::SAVECONFIG;
				break;
			case 3:
				return CRadioUOLError::CONFIG_INTERNET_OFF;
				break;
			}
		}
		else
		{
			if (FALSE == LoadConfigFile(filePath))
			{
				return CRadioUOLError::CONFIG_FILE;
			}
		}
	}

	return 0;
}

void CRadioConfig::Trace()
{
	ATLTRACE("m_strTicketURL %s\n", m_strTicketURL);
	ATLTRACE("m_strTicketRegex %s\n", m_strTicketRegex);
	ATLTRACE("m_strChannelURL %s\n", m_strChannelURL);
}


CString CRadioConfig::GetConfigFilePath()  const 
{
	// O codigo abaixo eh para o diretorio comun para todos os usuarios
	TCHAR szFilepath[512] = {0};
	HMODULE hmodule;

#ifdef _DEBUG	
	hmodule = GetModuleHandle("RadioUOLd");
#else
	hmodule = GetModuleHandle("RadioUOL");
#endif

	if (GetModuleFileName(hmodule, szFilepath, 512))
	{		
		CString tmpString;

		ATLPath::RemoveFileSpec(szFilepath);
		
		tmpString.LoadString(IDS_CONFIG_XML_FILENAME);
	
		CString  strPath = szFilepath ;
		strPath = strPath+ '\\' + tmpString;

		return strPath;
	}
	
	return "";
}


BOOL CRadioConfig::LoadConfigFile(const CString& filePath)
{
	// Le os dados do arquivo de configuracao
	CoInitialize(NULL);

	CXmlDocumentWrapper doc;
	if (doc.Load(filePath)==FALSE)
	{
		return FALSE;
	}

	

	BOOL te = doc.IsValid(); 
	
	
	//MSXML2::IXMLDOMNodePtr ptr = doc.AsNode();
	CXmlNodeWrapper sourceNode(doc.AsNode());
	//ptr.Release();
	

	
	//CXmlNodeWrapper sourceNode(doc.AsNode());
	
	

	CString tmp = sourceNode.GetXML();
	BOOL tes = sourceNode.IsValid();
	tmp = sourceNode.GetAttribName(0);
	tmp = sourceNode.GetAttribVal(0);

	
	// Ticket
	CXmlNodeWrapper childNode(sourceNode.GetNode("ticket"));
	m_strTicketRegex = childNode.GetValue("regex");
	m_strTicketURL = childNode.GetValue("url");
		

	
	// Channels
	CXmlNodeWrapper childNodeChannel(sourceNode.GetNode("channels"));
	//childNode =  sourceNode.GetNode("channels");
	m_strChannelURL = childNodeChannel.GetValue("url");
	

	// WMP
	CXmlNodeWrapper childNodeWmpUrl(sourceNode.GetNode("wmpurl"));
	//childNode =  sourceNode.GetNode("wmpurl");
	m_strWMPURL = childNodeWmpUrl.GetValue("url");
	m_strWMPURLRegex = childNodeWmpUrl.GetValue("regex");

	// SongInfo
	CXmlNodeWrapper childNodeSong(sourceNode.GetNode("songinfo"));
	//childNode =  sourceNode.GetNode("songinfo");
	m_strSongInfoURL = childNodeSong.GetValue("url");
	m_strSongInfoSongRegExURL = childNodeSong.GetValue("songregex");
	m_strSongInfoArtistRegExURL = childNodeSong.GetValue("artistregex");
	m_strSongInfoJpegRegExURL = childNodeSong.GetValue("jpegregex");
	m_strSongInfoCdRegExURL = childNodeSong.GetValue("cdregex");
	m_strSongInfoNextSongRegExURL = childNodeSong.GetValue("nextsongregex");

	//Ferrugem
	CXmlNodeWrapper childNodeFerrugem(sourceNode.GetNode("ferrugem"));
	//childNode =  sourceNode.GetNode("ferrugem");
	m_strFerrugemURL = childNodeFerrugem.GetValue("url");
	m_strFerrugemURLRegEx = childNodeFerrugem.GetValue("regex");

	//Sites
	CXmlNodeWrapper childNodeSites(sourceNode.GetNode("sites"));
	//childNode =  sourceNode.GetNode("sites");
	m_strSiteSearchURL = childNodeSites.GetValue("search");
	m_strSiteChannelURL = childNodeSites.GetValue("channel");
	m_strSiteSearchRegEx = childNodeSites.GetValue("searchregex");
	m_strSiteChannelRegEx = childNodeSites.GetValue("channelregex");


	//XMLConfig
	CXmlNodeWrapper childNodeXmlConfig(sourceNode.GetNode("xmlconfig"));
	//childNode =  sourceNode.GetNode("xmlconfig");
	m_strXMLConfigURL = childNodeXmlConfig.GetValue("url");

	

	if ( m_strTicketRegex.IsEmpty()||
		 m_strTicketURL.IsEmpty()||
		 m_strChannelURL.IsEmpty()||
		 m_strXMLConfigURL.IsEmpty()||
		 m_strWMPURL.IsEmpty()||
	 	 m_strSongInfoURL.IsEmpty()||
		 m_strWMPURLRegex.IsEmpty()||
		 //m_strFerrugemURL.IsEmpty()||
	 	// m_strFerrugemURLRegEx.IsEmpty()||
		 m_strSiteSearchURL.IsEmpty()||
		 m_strSiteChannelURL.IsEmpty()||
		 m_strSiteSearchRegEx.IsEmpty()||
		 m_strSiteChannelRegEx.IsEmpty()||

		 m_strSongInfoSongRegExURL.IsEmpty()||
		 m_strSongInfoArtistRegExURL.IsEmpty()||
		 m_strSongInfoCdRegExURL.IsEmpty()||
		 m_strSongInfoNextSongRegExURL.IsEmpty()||
		 m_strSongInfoJpegRegExURL.IsEmpty())
	{
		return FALSE;
	}

	//doc.Detach();

	return TRUE;
}

BOOL CRadioConfig::HasNewConfig(const CString & filepath)
{
	HANDLE hFile = ::CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) 
	{
		ATLTRACE (_T("Invalid file handle. Error is %u\n"), GetLastError());
		ATLASSERT(FALSE);

		return TRUE;
	} 
	else
	{
		FILETIME ftFile;
		::GetFileTime(hFile, NULL, NULL, &ftFile);
		
		CTime tFile(ftFile);	
		
		if (CTime::GetCurrentTime() >= (tFile + CTimeSpan(1, 0, 0, 0)))
		{
			CloseHandle(hFile);
			return TRUE;
		}
	}

	CloseHandle(hFile);
	return FALSE;
}