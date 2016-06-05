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

#include "StdAfx.h"
#include <vector>
#include <atlctrls.h>
#include <RichOle.h>
#include <atldlgs.h>						// Font dialog


#define MAX_TAG_LEN 10					// Maximun len of a TAG
#define MAX_CHARACTER_MESSAGE_UOL 1000 	// Maximun characters in message
#define MAX_CHARACTER_MESSAGE_UOL_TAG  MAX_CHARACTER_MESSAGE_UOL * MAX_TAG_LEN

//
// Node in tree struct
//

class CNodeData {
public:
	CString m_tag;
	CString m_filepath;
	int m_index;
};

class CNode {
public:
	CNode();
	~CNode();
	CNode * m_right;
	CNode * m_down;
	TCHAR m_character;
	CNodeData m_data;
};

//
//	Parse for Emoticon
//
class CParse {
public:
	CParse(int comboEmoticonSetIds, BOOL bForceUppercase);
	~CParse();
	// Transformar texto com TAGS em um texto com referencias aos arquivos GIF
	CString ParseHtmlText(const CString &strSource) ;
	// Procura em str alguma string , retorna o tamanho da tag encontrada (lenTag) e a posicao no retorno
	int     FindTAGinString(TCHAR * str, int & lenTag, CNodeData * node);
	// Utilizada para consultar a tag dado um index, (limitacao do uso do COM (Anim))
	CString Index2Tag(int index);
	// Informar que precisa carregar novamente os gifs do tema
	void    UpdateTags(); 
	// Carrega os gifs do tema
	void	PrepareTags();
	// Just for Debug
	void	TraceTree();
private:
	
	CString CParse::ParseText(const CString &strSource) ;
	struct EmoticonIndex{
		int index;
		CNode * ptr;
	};
	void	Text2Html(CString &text, CString &html);
	void	PushTag(CNode *);
	void    SkipURL(TCHAR ** str);
	void	InsertEmoticon(CString TAG, CString filepath);
	int		FindEmoticonPos(TCHAR * str, long &start, long &end, CNodeData * node);
	int		FindEmoticon(CString str, CString &filepath, CNodeData * node);
	int		ConsumeString(const TCHAR * str, CString &newstr);
	BOOL	InsertTree(CString ptr, CNodeData nodedata);
	CNode * InserCNode(CNode ** node ,const TCHAR * ptr, CNodeData * nodedata);
	void	RemoveNodes(CNode **node);
	void	Visit(CNode *node,TCHAR *hist,TCHAR *histstart);
	CNode*	FindTAGPosInTree(CNode *node,  TCHAR * ptr, TCHAR **where);
	CNode*	FindTAGInTree(CNode *node, const TCHAR * ptr, CString& completeTag);
	CString ParseString(CString & newstr);
	CNode  *m_root;
	BOOL	m_needUpdateTags;
	std::vector<CNode *> vEmoticons;

	void PrepareText(const CString& sText);
	bool tspace(TCHAR c);
	
	void PurgeNodes();

private:
	int		m_comboEmoticonSetIds;
	BOOL	m_bForceUppercase;
};
