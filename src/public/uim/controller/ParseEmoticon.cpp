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
#include "ParseEmoticon.h"
#include "../HTMLFilter.h"
#include "./CustomEmoticon/CustomEmoticonManager.h"

#include "UIMApplication.h"
//////////////////////////////////////////////////////////////////////////
// CNode 
//////////////////////////////////////////////////////////////////////////
CNode::CNode()
{
	m_right		= NULL;

	m_down		= NULL;

	m_character	= NULL;

	m_data.m_tag ="";

	m_data.m_filepath ="";
}
CNode::~CNode()
{
}

//////////////////////////////////////////////////////////////////////////
// CParse
//////////////////////////////////////////////////////////////////////////


CParse::CParse(int comboEmoticonSetIds, BOOL bForceUppercase)
{
	m_root = NULL;

	m_needUpdateTags = TRUE;

	m_comboEmoticonSetIds = comboEmoticonSetIds;
	m_bForceUppercase = bForceUppercase;
}

CParse::~CParse()
{
	PurgeNodes();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CString CParse::ParseHtmlText(const CString &strSource)
{
	TCHAR * start;
	TCHAR * pStrSource = (LPTSTR) (LPCTSTR) strSource;
	TCHAR * pStart = pStrSource;
	CString strTarget="";

	while( *pStrSource ) {
		//Ignore HTML tags
		//Copying all HTML code to targer
		while (*pStrSource=='<') {
			pStrSource++;
			strTarget+='<';
			while((*pStrSource)&&(*pStrSource!='>')) {
				strTarget+=*pStrSource;
				pStrSource++;
			}
			if (*pStrSource=='>') {
				strTarget+='>';
				pStrSource++;
			}
		}
		// start - First char to be parsed
		// Find the last before next HTML Tag or before the end.
		start = pStrSource;
		while((*pStrSource)&&(*pStrSource!='<'))
			pStrSource++;
		if (pStrSource>start) {
			CString strToBeParsed = strSource.Mid(start-pStart,pStrSource-start );
			//strToBeParsed = CHTMLFilter::HTMLToText(strToBeParsed);
			//CString html;
			//Text2Html(strToBeParsed,html);
			//html = CHTMLFilter::TextToHTML(html);
			
			strTarget+=ParseText(strToBeParsed);
		}
	}
	return strTarget;
}

CString CParse::ParseText(const CString &strSource) 
{
	CString strOutput="";
	CString strTmp = strSource;
	strTmp = CHTMLFilter::HTMLToText(strTmp);
	//TCHAR * pStrSource = (LPTSTR) (LPCTSTR) strSource;
	TCHAR * pStrSource = (LPTSTR) (LPCTSTR)   strTmp;
	while (*pStrSource)
	{
		int len;
		CNodeData auxNodaData;
		//long start, end,
		long ret;
		//CNodeData tmpNodeData;
		ret = FindTAGinString(pStrSource,len,&auxNodaData);
		if (ret>=0) 
		{
			//for (int i=0;i<ret;i++) 
			//{
			if (ret>=1) 
			{
				CString aux;
				aux = pStrSource;
				strOutput  +=  CHTMLFilter::TextToHTML(aux.Left(ret));
				pStrSource+=ret;
			}
				


			//	strOutput = strOutput + *pStrSource;
			//	pStrSource++;
			//}
			

			CString strFormat;
			if (ATLPath::FileExists(auxNodaData.m_filepath)==TRUE)
			{
				IPicture *pIPicture = NULL;
				BOOL bLoaded = FALSE;
				LONG cx;
				LONG cy;
				OLE_XSIZE_HIMETRIC width;
				OLE_YSIZE_HIMETRIC height;
				WCHAR wpath[MAX_PATH];
				
				MultiByteToWideChar(CP_ACP, 0, auxNodaData.m_filepath, -1, wpath, MAX_PATH);
				
				OleLoadPicturePath(wpath, NULL, NULL, NULL, IID_IPicture,(LPVOID*)&pIPicture);
				
				if (pIPicture)
				{
					bLoaded = TRUE;
					
					HDC hdc = ::GetDC(::GetDesktopWindow());
					pIPicture->get_Width(&width);
					pIPicture->get_Height(&height);
					cx = (GetDeviceCaps(hdc, LOGPIXELSX) * width / 2540) + 1;
					cy = (GetDeviceCaps(hdc, LOGPIXELSY) * height / 2540) + 1;
					ReleaseDC(0, hdc);
					
					CCustomEmoticonManager::CalculateImagePresentationSize(cy, cx);
					
					pIPicture->Release();
				}
				
				if (bLoaded)
				{
					strFormat.Format("<img width=\"%d\" height=\"%d\" src=\"%s\" alt=\'%s\'>", 
							cx, cy, auxNodaData.m_filepath, auxNodaData.m_tag);
				}
				else
				{
					strFormat.Format("<img src=\"%s\" alt=\'%s\'>",auxNodaData.m_filepath,auxNodaData.m_tag);
				}
			}
			else
				strFormat.Format("%s",auxNodaData.m_tag);
			//newstr = newstr + strFormat;
			//strptr+=ret;

			strOutput += strFormat;
			
			pStrSource+=len;
		}
		else
		{
			break;
		}

	}
	
	//strOutput = strOutput + pStrSource;
	strOutput = strOutput + CHTMLFilter::TextToHTML(pStrSource);
	return strOutput;
		 

	//return "hello";
	//TCHAR * start;
	//TCHAR * pStrSource = (LPTSTR) (LPCTSTR) strSource;
	//TCHAR * pStart = pStrSource;
	//CString strTarget="";

	//while( *pStrSource ) {
	//	//Ignore HTML tags
	//	//Copying all HTML code to targer
	//	while (*pStrSource=='<') {
	//		pStrSource++;
	//		strTarget+='<';
	//		while((*pStrSource)&&(*pStrSource!='>')) {
	//			strTarget+=*pStrSource;
	//			pStrSource++;
	//		}
	//		if (*pStrSource=='>') {
	//			strTarget+='>';
	//			pStrSource++;
	//		}
	//	}
	//	// start - First char to be parsed
	//	// Find the last before next HTML Tag or before the end.
	//	start = pStrSource;
	//	while((*pStrSource)&&(*pStrSource!='<'))
	//		pStrSource++;
	//	if (pStrSource>start) {
	//		CString strToBeParsed = strSource.Mid(start-pStart,pStrSource-start );
	//		strToBeParsed = CHTMLFilter::HTMLToText(strToBeParsed);
	//		CString html;
	//		Text2Html(strToBeParsed,html);
	//		//html = CHTMLFilter::TextToHTML(html);
	//		strTarget+=html;
	//	}
	//}

	//ATLTRACE("%s %s\n",__FUNCTION__,strTarget);
	//
	//return strTarget;

}


//
// Parse a str to a newstr, replacing all tags by objects
//
int CParse::FindTAGinString(TCHAR * str, int & lenTag, CNodeData * node)
{
	//ATLTRACE("%s [%s]\n",__FUNCTION__, str);
	CString path;
	/*int len;*/
	CString searchStr = str;
	const TCHAR * auxStart;
	TCHAR * pSearchStr;
	
	PrepareTags();
	
	if (m_bForceUppercase)
	{
		searchStr.MakeUpper();
		pSearchStr = (LPTSTR) (LPCTSTR) searchStr;
	}
	else
	{
		pSearchStr = str;
	}
	
	auxStart = pSearchStr;
	while(*pSearchStr) {
		int ret;
		// Skip urls // http://
		SkipURL(&pSearchStr);

		ret = FindEmoticon(pSearchStr,path,node);
		if (ret==0) 
		{
			if (*pSearchStr!=NULL)
				pSearchStr++;
		}
		else
		{
			lenTag=ret;
			return pSearchStr-auxStart;
		}
	}
	return -1;
}



CString CParse::Index2Tag(int index)
{
	int len = vEmoticons.size();

	ATLASSERT(index<len);

	return vEmoticons[index]->m_data.m_tag;
}



void CParse::PrepareTags()
{
	if ( (m_needUpdateTags) && (m_comboEmoticonSetIds != 0) )
	{
		PurgeNodes();
		
		IUOLMessengerSettingsManager2Ptr pSettingsManager2 = ap_dynamic_cast<IUOLMessengerSettingsManager2Ptr>(CUIMApplication::GetApplication()->GetSettingsManager());
		IUOLMessengerEmoticonSettings3Ptr pEmoticonSettings3;

		ATLASSERT(pSettingsManager2);
		if (pSettingsManager2)
		{
			pEmoticonSettings3 = ap_dynamic_cast<IUOLMessengerEmoticonSettings3Ptr>(
					pSettingsManager2->GetEmoticonSettings());
		}

		ATLASSERT(pEmoticonSettings3);
		if (pEmoticonSettings3)
		{
			CAtlList<IUOLMessengerEmoticonPtr> emoticonsList;
			pEmoticonSettings3->GetEmoticonList(m_comboEmoticonSetIds, emoticonsList);

			for (POSITION pos = emoticonsList.GetHeadPosition(); pos; emoticonsList.GetNext(pos))
			{
				IUOLMessengerEmoticon2Ptr pEmoticon2 = 
						ap_dynamic_cast<IUOLMessengerEmoticon2Ptr>(emoticonsList.GetAt(pos));

				if (pEmoticon2)
				{
					CString path = pEmoticon2->GetFilePath();

					if (ATLPath::IsRelative(path))
					{
						CString themepath = pSettingsManager2->GetEmoticonSettings()->GetDefaultDirPath();

						path= themepath + '\\' +path;
					}

					//ATLTRACE("%s %s %s\n",__FUNCTION__, tag, path);

					CAtlList<CString> listShortcuts;
					pEmoticon2->GetShortcutList(listShortcuts);
					
					for (POSITION shortcutPos = listShortcuts.GetHeadPosition(); shortcutPos; 
							listShortcuts.GetNext(shortcutPos))
					{
						CString tempShortcut = listShortcuts.GetAt(shortcutPos);
						
						//if (m_bForceUppercase)
						//{
							//tempShortcut.MakeUpper();
						//}
						
						InsertEmoticon(tempShortcut, path);
					}
				}
			}
			m_needUpdateTags=FALSE;
			//TraceTree();
		}
	}
	//TraceTree();
}

void CParse::Text2Html(CString &text, CString &html)
{
	CString newHtml;
	PrepareTags();
	newHtml = ParseString(text);
	html = newHtml;
}


void CParse::UpdateTags()
{
	m_needUpdateTags = TRUE;
}

//
// Insert a new Emoticon in tree
//
void CParse::InsertEmoticon(CString TAG, CString filepath)
{
	CNodeData data;
	CNode * aux;
	data.m_filepath = filepath;
	data.m_tag=TAG;
	data.m_index=vEmoticons.size();
	
	CString tempUpperTag = TAG;
	if (m_bForceUppercase)
	{
		tempUpperTag.MakeUpper();
	}
	
	aux = InserCNode(&m_root,tempUpperTag,&data);
	vEmoticons.push_back(aux);
}

//
//  Find Emoticon in str
//
int CParse::FindEmoticonPos(TCHAR * str, long &start, long &end, CNodeData * node)
{
	ATLASSERT(_tclen(str)>0);
	CNode *tmpnode;
	TCHAR *where;
	node = NULL;
	tmpnode = FindTAGPosInTree(m_root,str,&where);
	if (tmpnode!=NULL) {
		end = where - str;
		start = end - tmpnode->m_data.m_tag.GetLength();
		node = &tmpnode->m_data;
		return tmpnode->m_data.m_tag.GetLength();
	}
	return 0;
}

//
// Parse the string 
//
CString CParse::ParseString(CString &newstr)
{
	CString newString;
	//CString tmpString = newstr;
	//tmpString.Replace(";lt","<");
	//tmpString.Replace(";gt",">");
	ConsumeString(newstr, newString);
	return newString;
}

//
// Find an Emoticon in str
//
int CParse::FindEmoticon(CString str, CString &filepath, CNodeData * node)
{
	//ATLASSERT(str.GetLength()>0);
	if (str.IsEmpty()==TRUE)
	{
		return 0;
	}

	CNode *tmpnode;

	tmpnode = FindTAGInTree(m_root,str,CString(""));

	if (tmpnode!=NULL)
	{
		filepath = tmpnode->m_data.m_filepath;

		*node = tmpnode->m_data;

		return tmpnode->m_data.m_tag.GetLength();
	}

	return 0;
}

//
// Show (trace) tree
//
void CParse::TraceTree()
{
	TCHAR backlog[MAX_TAG_LEN]=_T("");
	Visit(m_root,backlog,backlog);
}

//
//	Walk in tree showing all TAG nodes
//
void CParse::Visit(CNode *node,TCHAR *hist,TCHAR *histstart)
{
	if (node == NULL) return;
	ATLTRACE("%c ",node->m_character);
	if (node->m_down == NULL) {
		ATLTRACE("  %s %d\n",node->m_data.m_tag,node->m_data.m_index);
	}
	Visit(node->m_down,NULL,NULL);
	Visit(node->m_right,NULL,NULL);
}
//{
//	ATLTRACE("%s \n",__FUNCTION__);
//	if (node==NULL) {
//		return;
//	}
//	*hist=node->m_character;
//	hist++;
//	if (node->m_down==NULL) {
//		*hist= NULL;
//		ATLTRACE("%s tag %s ind %d\n",__FUNCTION__, node->m_data.m_tag, node->m_data.m_index);
//	} else {
//		Visit(node->m_down,hist,histstart);
//	}
//	Visit(node->m_right,hist,histstart);
//}

//
// Parse a str to a newstr, replacing all tags by objects
//
int CParse::ConsumeString(const TCHAR * str, CString &newstr)
{
	int nreplaces=0;
	CString path;
	//int len;
	//len = _tclen(str);
	CNodeData auxnode;
	TCHAR *strptr = ( TCHAR *) str;
	TCHAR *oldptr;
	while(*strptr) {

		oldptr = strptr;

		SkipURL(&strptr);

		while(oldptr<strptr) 
		{
			newstr=newstr + *oldptr;
			oldptr++;
		}

		int ret = FindEmoticon(strptr,path,&auxnode);
		if (ret==0) {
			newstr=newstr + *strptr;
			strptr++;
		}
		else
		{
			auxnode.m_filepath;
			CString strFormat;
			//<img src=__IMAGE__/>
			CString fileExt = ATLPath::FindExtension(auxnode.m_filepath);
			if ((ATLPath::FileExists(auxnode.m_filepath)==TRUE)&&((fileExt.CompareNoCase(".gif")==0))) 
				strFormat.Format("<img src=\"%s\">",auxnode.m_filepath);
			else
				strFormat.Format("%s",auxnode.m_tag);
			newstr = newstr + strFormat;
			strptr+=ret;
			nreplaces+=ret;
		}
	}
	return nreplaces;
}
void CParse::SkipURL(TCHAR **str)
{
#define MAX_URL_LEN 10	
	TCHAR keywords[][MAX_URL_LEN]={"http://","https://","ftp://","ftp.","mailto://","file://"};
	//TCHAR *ptr,*aux;
	int max = sizeof(keywords)/MAX_URL_LEN;
	int i;
	
	for (i=0;i<max;i++) {
		if (strncmp(keywords[i],*str,strlen(keywords[i]))==0)
		{
			*str+=strlen(keywords[i]);
			return;
		}
		//ptr = *str;
		//aux = keywords[i];
		//while ((*aux&&*ptr)&&(*aux==tolower(*ptr)))
		//	{aux++; ptr++;}
		//if ((*aux==NULL)/*||(*ptr==NULL)*/) {
		//	*str = ptr;
		//	return ;
		//}
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//	Search in tree for a node with TAG in ptr
//	Return a CNode if found a TAG in tree  Otherwise return NULL.
//
CNode * CParse::FindTAGPosInTree(CNode *node,  TCHAR * ptr, TCHAR **where)
{
	// Return NOT FOUND
	if (*ptr==NULL) { 
		return NULL;
	}
	// Return NOT FOUND
	if (node==NULL) {
		return NULL;
	}
	// We can continue searching in down nodes
	if (*ptr==node->m_character) {
		ptr++; 
		// If down node is NULL, we found a valid TAG
		if (node->m_down==NULL) {
			*where=ptr-1;
			return node;	// Return FOUND
		}
		//Searching in down bodes
		return FindTAGPosInTree(node->m_down,ptr,where);
		// We can continue searching in right nodes
	} else 
		return FindTAGPosInTree(node->m_right,ptr,where);
	// We didn't find any node in right neither down nodes
	return NULL;		// Return NOT FOUND
}

CNode * CParse::FindTAGInTree(CNode *node, const TCHAR * ptr, CString& completeTag)
{
	CNode *tmpnode  = node;
	const TCHAR *posChar = NULL;
	while (tmpnode!=NULL) {
		if (tmpnode->m_character == *ptr) 
			break;
		tmpnode = tmpnode->m_right;
	}
	if (tmpnode != NULL) {
		completeTag += tmpnode->m_character;
		posChar = ptr;
		posChar++;
		if ( (tmpnode->m_down==NULL) || ( (lstrcmpi(tmpnode->m_data.m_tag, completeTag) == 0) && 
				( (*posChar == '\0') || (*posChar == ' ') ) ) )
			return tmpnode;
		return FindTAGInTree(tmpnode->m_down,++ptr,completeTag);
	}
	return NULL;
}
//
//	Search in tree for a node with TAG in ptr
//	Return a CNode if found a TAG in tree  Otherwise return NULL.
//
//CNode * CParse::FindTAGInTree(CNode *node, const TCHAR * ptr)
//{
//	// Auxiliar point to walk hor.
//	//return NULL;
//	CNode *tmpnode;
//
//	// Return NOT FOUND
//	if (*ptr==NULL) { 
//		return NULL;
//	}
//
//	// Return NOT FOUND
//	if (node==NULL) {
//		return NULL;
//	}
//
//	// We can continue searching in down nodes
//	if (*ptr==node->m_character) {
//
//		ptr++; // Consume a char in TAG
//
//		// If down node is NULL, we found a valid TAG
//		if (node->m_down==NULL) 
//			return node;	// Return FOUND
//
//		//Searching in down bodes
//		return FindTAGInTree(node->m_down,ptr);
//
//		// We can continue searching in right nodes
//	} else {
//		tmpnode = FindTAGInTree(node->m_right,ptr);
//		if (tmpnode) 
//			return tmpnode;
//	}
//	// We didn't find any node in right neither down nodes
//	return NULL;		// Return NOT FOUND
//}

//
//  Insert a new TAG in the tree
//
BOOL CParse::InsertTree(CString ptr, CNodeData nodedata)
{
	CNode * aux;
	aux = InserCNode(&m_root,ptr,&nodedata);
	return TRUE;
}

//
// Inserting a TAG in the tree
//
CNode * CParse::InserCNode(CNode ** node ,const TCHAR * ptr, CNodeData * nodedata)
{
	CNode *tmpnode=NULL;

	if (*node==NULL) {
		tmpnode=new CNode;
		tmpnode->m_down=NULL;
		tmpnode->m_right=NULL;
		tmpnode->m_character=*ptr;
		*node =tmpnode;
		ptr++;
		if(*ptr)
			return InserCNode(&(tmpnode->m_down),ptr,nodedata);
		else {
			tmpnode->m_data = *nodedata;
			return tmpnode;
		}
	} else {
		tmpnode = *node;
		while(tmpnode){
			if (tmpnode->m_character == *ptr) break;
			tmpnode=tmpnode->m_right;
		}
		if (tmpnode==NULL) {
			tmpnode= new CNode;
			tmpnode->m_down=NULL;
			tmpnode->m_right=*node;
			tmpnode->m_character=*ptr;
			*node = tmpnode;
		}
		ptr++;
		if(*ptr)
			return InserCNode(&(tmpnode->m_down),ptr,nodedata);
		else {
			tmpnode->m_data = *nodedata;
			return tmpnode;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CParse::RemoveNodes(CNode **node)
{
	if (*node==NULL) {
		return;
	}
	CNode *tmpNode = *node;
	RemoveNodes(&tmpNode->m_down);
	RemoveNodes(&tmpNode->m_right);	
	delete tmpNode;
	*node=NULL;
}

void CParse::PushTag(CNode * ptrNode)
{
	EmoticonIndex * aux;
	aux = new EmoticonIndex;
	aux->index = 9;
	aux->ptr = ptrNode;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CParse::PrepareText(const CString& sText)
{
	CString m_sText;
	m_sText = sText;
	//m_Links.clear();

	enum {
		unknown,
		space,
		http0,		/* http://		*/
		http1, http2, http3, http4, http5, http6,
		ftp0,		/* ftp://		*/
		ftp1, ftp2, ftp3, ftp4, ftp5,
		ftp,		/* ftp.			*/
		www0,		/* www.			*/
		www1, www2, www3,
		mailto0, 	/* mailto:		*/
		mailto1, mailto2, mailto3, mailto4, mailto5, mailto6,
		mail		/* xxx@yyy		*/
	} state = space;

	int WordPos = 0;
	TCHAR sz[2];
	TCHAR& c = sz[0];
	sz[1] = 0;
	int last = m_sText.GetLength() -1;
	for(int i = 0; i <= last; i++)
	{
		c = m_sText[i];
		_tcslwr(sz);

		switch(state)
		{
		case unknown:
			if(tspace(c))
				state = space;
			else
				if(c == _T('@') && WordPos != i)
					state = mail;		
			break;

		case space:
			WordPos = i;
			switch(c)
			{
			case _T('h'): state = http0; break;
			case _T('f'): state = ftp0; break;
			case _T('w'): state = www0; break;
			case _T('m'): state = mailto0; break;
			default:
				if(!tspace(c))
					state = unknown;
			}
			break;

			/*----------------- http -----------------*/
		case http0:
			if(c == _T('t'))
				state = http1;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case http1:
			if(c == _T('t'))
				state = http2;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case http2:
			if(c == _T('p'))
				state = http3;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case http3:
			if(c == _T(':'))
				state = http4;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case http4:
			if(c == _T('/'))
				state = http5;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case http5:
			if(c == _T('/'))
				state = http6;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case http6:
			if(tspace(c) || i == last)
			{
				int len = i == last ? i - WordPos + 1 : i - WordPos;
				CString s = m_sText.Mid(WordPos, len);
				//RemoveLastSign(s);
				//m_Links.push_back(CHyperLink(WordPos, WordPos + len - 1, s, s, (LPCTSTR)NULL));
				state = space;
			}
			break;

			/*----------------- ftp -----------------*/
		case ftp0:
			if(c == _T('t'))
				state = ftp1;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case ftp1:
			if(c == _T('p'))
				state = ftp2;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case ftp2:
			if(c == _T(':'))
				state = ftp3;
			else
				if(c == _T('.'))
					state = ftp;
				else
					if(tspace(c))
						state = space;
					else
						state = unknown;
			break;

		case ftp3:
			if(c == _T('/'))
				state = ftp4;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case ftp4:
			if(c == _T('/'))
				state = ftp5;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case ftp:
			if(tspace(c) || i == last)
			{
				int len = i == last ? i - WordPos + 1 : i - WordPos;
				CString s = CString(_T("ftp://")) + m_sText.Mid(WordPos, len);
				//RemoveLastSign(s);
				//m_Links.push_back(CHyperLink(WordPos, WordPos + len - 1, s, s, (LPCTSTR)NULL));
				state = space;
			}
			break;

		case ftp5:
			if(tspace(c) || i == last)
			{
				int len = i == last ? i - WordPos + 1 : i - WordPos;
				CString s = m_sText.Mid(WordPos, len);
				//RemoveLastSign(s);
				//m_Links.push_back(CHyperLink(WordPos, WordPos + len - 1, s, s, (LPCTSTR)NULL));
				state = space;
			}
			break;

			/*----------------- www -----------------*/
		case www0:
			if(c == _T('w'))
				state = www1;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case www1:
			if(c == _T('w'))
				state = www2;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case www2:
			if(c == _T('.'))
				state = www3;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case www3:
			if(tspace(c) || i == last)
			{
				int len = i == last ? i - WordPos + 1 : i - WordPos;
				CString s = CString(_T("http://")) + m_sText.Mid(WordPos, len);
				//RemoveLastSign(s);
				//m_Links.push_back(CHyperLink(WordPos, WordPos + len - 1, s, s, (LPCTSTR)NULL));
				state = space;
			}
			break;

			/*----------------- mailto -----------------*/
		case mailto0:
			if(c == _T('a'))
				state = mailto1;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case mailto1:
			if(c == _T('i'))
				state = mailto2;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case mailto2:
			if(c == _T('l'))
				state = mailto3;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case mailto3:
			if(c == _T('t'))
				state = mailto4;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case mailto4:
			if(c == _T('o'))
				state = mailto5;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case mailto5:
			if(c == _T(':'))
				state = mailto6;
			else
				if(tspace(c))
					state = space;
				else
					state = unknown;
			break;

		case mailto6:
			if(tspace(c) || i == last)
			{
				int len = i == last ? i - WordPos + 1 : i - WordPos;
				CString s = m_sText.Mid(WordPos, len);
				//RemoveLastSign(s);
				//m_Links.push_back(CHyperLink(WordPos, WordPos + len - 1, s, s, (LPCTSTR)NULL));
				state = space;
			}
			break;

			/*----------------- mailto -----------------*/
		case mail:
			if(tspace(c) || i == last)
			{
				int len = i == last ? i - WordPos + 1 : i - WordPos;
				CString s = CString(_T("mailto:")) + m_sText.Mid(WordPos, len);
				//RemoveLastSign(s);
				//m_Links.push_back(CHyperLink(WordPos, WordPos + len - 1, s, s, (LPCTSTR)NULL));
				state = space;
			}
			break;
		}
	}

	//m_Links.sort();
}

bool CParse::tspace(TCHAR c)
{
	return _istspace(c) || c < _T(' ') || c == _T(';') || c == _T(',') || c == _T('!');
}

void CParse::PurgeNodes()
{
	RemoveNodes(&m_root); 
	
	vEmoticons.clear();
	
	m_root=NULL;
}

