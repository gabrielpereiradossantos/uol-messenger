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


#include <interfaces/IUOLMessengerConversation.h>
#include <interfaces/IUOLMessengerMessageTemplateRecord.h>
#include <interfaces/IUOLMessengerMessageTemplateManager.h>
#include "../../model/MessageTemplate.h"


class CTemplateDocumentBuilder
{
public:
	
	CTemplateDocumentBuilder(IUOLMessengerConversationPtr pConversation);
	virtual ~CTemplateDocumentBuilder();
	
	void AddMessageTemplateRecord(IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord);
	
	CString GetUpdatedDocument(BOOL bForceRebuild);
	
protected:
	
	IUOLMessengerMessageTemplateManagerPtr GetTemplateManager();
	
	BOOL InitializeMainBlocks();
	BOOL LoadTemplateElements();
	BOOL InitializeReplacementTags();
	
	CString GetReplacingTagInfo(CString strMainTemplate);
	
	void AppendMessageTemplateRecordToDocument(IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord);
	
	BOOL ProcessSystemMessage(IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord);
	BOOL ProcessFileTransferMessage(IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord);
	BOOL ProcessNewMessageBlock(IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord);
	BOOL ProcessMessageBlockAppend(IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord);
	
	CString ReplaceTagsOnTemplate(
			IUOLMessengerMessageTemplate::MessageTemplateElementId nElementId, 
			IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord);
	
	void ReplaceTagsOnTemplate(CString& strTemplateUpdated, 
			IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord, 
			BOOL bIsHeaderOrFooter, BOOL bIsIncomingMessage);
	
	BOOL AppendToLastBlock(CString strAppendingBlock);
	
	BOOL GenerateTimeInfo(IUOLMessengerMessageTemplateRecordPtr pMessageTemplateRecord,
		const CString& strTemplate, const CString& strTimeInitialTag, 
		CString& strTimeCompleteTag, CString& strReplacingTime);
	
	CString RebuildDocument(BOOL bNeedReloadTemplateElements);
	
private:
	
	BOOL			m_bInitialized;
	BOOL			m_bTemplateElementsLoaded;
	BOOL			m_bReplacingTagsLoaded;
	
	IUOLMessengerConversation2Ptr		m_pConversation2;
	
	IUOLMessengerMessageTemplatePtr		m_pMessageTemplate;
	IUOLMessengerMessageTemplateRecordPtr		m_pLastMessageTemplateRecord;
	
	CString			m_messageTemplateAditionPoint;
	CString			m_messageTemplateReplaceInfoTag;
	CString			m_messageTemplateAppendingInfoTag;
	CString			m_messageDefaultTemplate;
	CString			m_fileTransferStatusMessageFormat;
	CString			m_fileTransferSaveFormat;
	CString			m_fileTransferSaveAsFormat;
	CString			m_fileTransferCancelFormat;
	CString			m_strFileTransferAcceptedMessageFormat;
	CString			m_strFileTransferRejectedMessageFormat;
	
	CString			m_strTemplateTagChatName;
	CString			m_strTemplateTagSourceName;
	CString			m_strTemplateTagDestinationName;
	CString			m_strTemplateTagDestinationDisplayName;
	CString			m_strTemplateTagIncomingIconPath;
	CString			m_strTemplateTagOutgoingIconPath;
	CString			m_strTemplateTagTimeOpened;
	CString			m_strTemplateTagUserIconPath;
	CString			m_strTemplateTagSenderScreenName;
	CString			m_strTemplateTagSender;
	CString			m_strTemplateTagService;
	CString			m_strTemplateTagMessage;
	CString			m_strTemplateTagTime;
	CString			m_strTemplateTagFileIconPath;
	CString			m_strTemplateTagFileName;
	CString			m_strTemplateTagSaveFileHandler;
	CString			m_strTemplateTagSaveAsFileHandler;
	CString			m_strTemplateTagCancelRequestHandler;
	
	CString			m_strMainTemplate;
	CString			m_strCSSImport;
	CString			m_strHeaderTemplate;
	CString			m_strFooterTemplate;
	CString			m_strStatusTemplate;
	CString			m_strNextStatusTemplate;
	CString			m_strIncomingContentTemplate;
	CString			m_strIncomingNextContentTemplate;
	CString			m_strIncomingFileTransferTemplate;
	CString			m_strOutgoingContentTemplate;
	CString			m_strOutgoingNextContentTemplate;
	
	CString			m_templateDocumentTop;
	CString			m_templateDocumentMessages;
	CString			m_lastMessageBlock;
	CString			m_templateDocumentBottom;
	
	CAtlList<IUOLMessengerMessageTemplateRecordPtr> m_MessageTemplateRecordList;
};

MAKEAUTOPTR(CTemplateDocumentBuilder);

