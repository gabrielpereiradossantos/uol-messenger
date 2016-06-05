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
#include <vector>
#include ".\RichUOLEdit.h"
#include "../../controller/UIMApplication.h"
#include <atldlgs.H>
#include <interfaces/IUOLMessengerMessageSettings.h>
#include "../../controller/EmoticonParserManager.h"
#include "../../controller/CustomEmoticon/CustomEmoticonManager.h"

#include ".\ImageDataObject.h"

#define ID_FORMAT_COLOR                 32778
#define ID_FORMAT_HIGHLIGHT				32783

//////////////////////////////////////////////////////////////////////////
// CRichUOLView
//////////////////////////////////////////////////////////////////////////

CRichUOLView::CRichUOLView() : 
m_listener(NULL),
m_pTransferDataObject(NULL)
{
	m_newChar=FALSE;
	m_pRichEditOle	= NULL;
	m_lpObject		= NULL;
	m_lpStorage		= NULL;
	m_lpClientSite	= NULL;
}


CRichUOLView::~CRichUOLView()
{
}	

void CRichUOLView::ClearEditText()
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csEmoticonInfo);
	
	IRichEditOle* pRichEditOle = GetOleInterface() ;
	
	if (pRichEditOle)
	{
		int iObjectCount = pRichEditOle->GetObjectCount();
		
		for (int iObjectIndex = 0; iObjectIndex < iObjectCount; iObjectIndex++)
		{
			REOBJECT reObject;
			EmoticonInfo* pEmoticonInfo = NULL;
			
			ZeroMemory(&reObject, sizeof(REOBJECT));
			reObject.cbStruct = sizeof(REOBJECT);	
			pRichEditOle->GetObject(iObjectIndex, &reObject, REO_GETOBJ_NO_INTERFACES);
			
			if (reObject.dwUser != 0)
			{
				pEmoticonInfo = (EmoticonInfo*)reObject.dwUser;
			}
			
			if (pEmoticonInfo != NULL)
			{
				delete pEmoticonInfo;
				pEmoticonInfo = NULL;
				
				reObject.dwUser = 0;
			}
		}
	}
	
	SetWindowText(_T(""));
}

void CRichUOLView::Test()
{
}

void CRichUOLView::Trace()
{
	CopyToClipboard();
}


LRESULT CRichUOLView::OnChar(UINT uMsg, WPARAM wParam,LPARAM lParam, 	BOOL& bHandled)
{
	bHandled = FALSE;
	m_newChar=TRUE;
	if (m_listener!=NULL)
		m_listener->OnTextAreaChar();
	if (wParam==VK_RETURN){
		// TODO: Verificar uma outra solução para notificar o envio de mensagem

		CWindow tmpWindow;
		tmpWindow = GetParent();
		if (tmpWindow.IsWindow()) {
			tmpWindow = tmpWindow.GetParent();
			if (tmpWindow.IsWindow()) {
				tmpWindow.PostMessage(WM_COMMAND, MAKEWPARAM(0, MEP_EDIT_RETURN), (LPARAM) m_hWnd);
			}
		}
		
		bHandled = TRUE;
		m_newChar = FALSE;
		return S_FALSE;
	}
	return S_OK;
}


LRESULT CRichUOLView::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	// Always handle VK_RETURN KeyDown event, except if ctrl is pressed. 
	if ((wParam==VK_RETURN) && (HIBYTE(::GetKeyState(VK_CONTROL)) == 0))
	{
		bHandled = TRUE;
	}

	return S_OK;
}

LRESULT CRichUOLView::OnChange(WORD, WORD, HWND, BOOL&)
{
	return 0;
}

LRESULT CRichUOLView::OnLink(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	ENLINK *plnk =(ENLINK *)pnmh;
	if (WM_LBUTTONDOWN == plnk->msg)
	{		
		long start,end;
		GetSel(start,end);
		SetSel(plnk->chrg); 
		int cb = plnk->chrg.cpMax - plnk->chrg.cpMin + 1;
		TCHAR* url = 0;
		if (cb > 1024)
			url = new TCHAR[cb];
		else
			url = (TCHAR *) alloca(cb * sizeof TCHAR);
		GetSelText(url);	
		SetSel(start, end);
		CRichEditHyperLink::Open(url, CRichEditHyperLink::OPEN, m_hWnd); 
		if (cb > 1024)
			delete [] url;
	}
	return S_OK;
}

LRESULT CRichUOLView::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;

	if (m_newChar) 
	{
		Parse();
		m_newChar=FALSE;
	}

	return S_OK;
}

void CRichUOLView::SetListener(CRichTextEventsEventListener *listener)
{
	m_listener = listener;
}

void CRichUOLView::SetConversation(IUOLMessengerConversationPtr pConversation)
{
	m_pConversation = pConversation;
}

void CRichUOLView::Parse()
{
	TCHAR tmpchar[MAX_CHARACTER_MESSAGE_UOL+1];
	TCHAR objchar = 0x01;
	LPCSTR pobjchar = &objchar;
//	BOOL pbool ;
	GetTextEx(tmpchar,MAX_CHARACTER_MESSAGE_UOL,GT_DEFAULT,CP_ACP,pobjchar/*, &pbool*/);

	HideSelection(TRUE, FALSE);
	CString log;
	int desl = 0;
	int len;
	CNodeData auxNodaData;
	CString strEnteredTag;
	
	long start = CEmoticonParserManager::GetInstance()->FindTAGinString(
			m_pConversation->GetAccount()->GetProtocol()->GetId().c_str(), 
			tmpchar,len,&auxNodaData);
	long found = start;

	if (found >= 0)
	{
		while (found>=0) {

			LONG cyHimetric;
			LONG cxHimetric;

			HBITMAP hBitmap = NULL;
			if (IsValidImageType(auxNodaData.m_filepath))
			{
				hBitmap = LoadPicture(auxNodaData.m_filepath, cyHimetric, cxHimetric);
			}

			if (hBitmap!=NULL)
			{
				SetSel(start,start+len);
				ScrollCaret();

				HDC hDC = ::GetDC(m_hWnd);
				LONG cyImageLimit = (IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_HEIGHT * 2540) / GetDeviceCaps(hDC, LOGPIXELSY);
				LONG cxImageLimit = (IUOLMessengerCustomEmoticon::DEFAULT_CUSTOM_EMOTICON_WIDTH * 2540) / GetDeviceCaps(hDC, LOGPIXELSX);
				::ReleaseDC(m_hWnd,hDC);

				CCustomEmoticonManager::CalculateImagePresentationSize(cyHimetric, cxHimetric, cyImageLimit, cxImageLimit);

				CSize szImageLimit(cxHimetric, cyHimetric);

				EmoticonInfo* pEmoticonInfo = new EmoticonInfo;
				
				if (pEmoticonInfo != NULL)
				{
					strEnteredTag.Format("%s", tmpchar);
					strEnteredTag = strEnteredTag.Mid(start, len);
					
					pEmoticonInfo->nTagIndex = auxNodaData.m_index;
					pEmoticonInfo->strOriginalText = strEnteredTag;
					
					{
						CComCritSecLock<CComAutoCriticalSection> lock(m_csEmoticonInfo);
						
						CImageDataObject::InsertBitmap(GetOleInterface(), 
								hBitmap, (DWORD)pEmoticonInfo, &szImageLimit);
					}
				}
			}
			else 
			{
				desl = start + len;
			}

			GetTextEx(tmpchar,MAX_CHARACTER_MESSAGE_UOL,GT_DEFAULT,CP_ACP,pobjchar/*, &pbool*/);

			found = CEmoticonParserManager::GetInstance()->FindTAGinString(
				m_pConversation->GetAccount()->GetProtocol()->GetId().c_str(), 
				tmpchar+desl,len,&auxNodaData);

			if (found>=0)
			{
				start = found+desl;
			}
			else
			{
				start = -1;
			}

		}
	}
	HideSelection(FALSE, FALSE);
	SetFocus();
}


void CRichUOLView::ChangeFont()
{
	CHARFORMAT2 cf;
	GetDefaultCharFormat(cf);
	CRichEditFontDialog dlg(cf/*,CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE|CFM_FACE|CFM_COLOR*/);
	dlg.m_cf.Flags |= CF_INITTOLOGFONTSTRUCT|CF_LIMITSIZE;
	dlg.m_cf.nSizeMin = 8;
	dlg.m_cf.nSizeMax = 16;
	
	if (dlg.DoModal() == IDOK)
	{
		dlg.GetCharFormat(cf);
		SetDefaultCharFormat(cf);	
	}
}

CString CRichUOLView::GetEditPlainText()
{
	long start,end;
	GetSel(start,end);
	HideSelection(TRUE, FALSE);
	SetSel(0,-1);
	CString tmpString = GetSelectedText(PLAIN_TEXT);
	SetSel(start,end);
	HideSelection(FALSE, FALSE);
	return tmpString;
}


CString CRichUOLView::GetEditHTMLText()
{
	long start,end;
	GetSel(start,end);
	HideSelection(TRUE, FALSE);
	SetSel(0,-1);
	CString plainText = GetEditPlainText();
	CString tmpString;
	plainText.Trim();
	if (!plainText.IsEmpty())
	{
		tmpString = GetSelectedText(HTML_TEXT);
	}
	SetSel(start,end);
	HideSelection(FALSE, FALSE);
	return tmpString;
}

int CRichUOLView::AddTag(TCHAR * target, TCHAR *source)
{
	int cont=0;
	while(*source) {
		*target=*source;
		cont++;
		source++;
		target++;
	}
	return cont;
}


CString CRichUOLView::FindTag(CHARFORMAT oldeffects, CHARFORMAT neweffects, BOOL flag_begin)
{
	DWORD old_effects = oldeffects.dwEffects;
	DWORD new_effects = neweffects.dwEffects;
	CString retString;

	BOOL oldItalic;
	BOOL oldBold;
	BOOL oldUnderline;
	BOOL oldStrikeout;

	BOOL newItalic;
	BOOL newBold;
	BOOL newUnderline;
	BOOL newStrikeout;

	int Italic;
	int Bold;
	int Underline;
	int Strikeout;

	oldItalic		= (old_effects & CFE_ITALIC);
	oldBold			= (old_effects & CFE_BOLD);
	oldUnderline	= (old_effects & CFE_UNDERLINE);
	oldStrikeout	= (old_effects & CFE_STRIKEOUT);

	newItalic		= (new_effects & CFE_ITALIC);
	newBold			= (new_effects & CFE_BOLD);
	newUnderline	= (new_effects & CFE_UNDERLINE);
	newStrikeout	= (new_effects & CFE_STRIKEOUT);

	Italic = (oldItalic ^ newItalic) ? (newItalic ? 1 : -1) : 0;
	Bold = (oldBold ^ newBold) ? (newBold ? 1 : -1) : 0;
	Underline = (oldUnderline ^ newUnderline) ? (newUnderline ? 1 : -1) : 0;
	Strikeout= (oldStrikeout ^ newStrikeout) ? (newStrikeout ? 1 : -1) : 0;

	// TODO: Verificar a implementação utilizando pilha
	if (flag_begin) {
		switch(Italic){
		case 1:
			retString.Append("<I>");
			break;
			//case -1:
			//	retString.Append("</I>");
			//	break;
			//default:
			//	break;
		}

		switch(Strikeout){
		case 1:
			retString.Append("<S>");
			break;
			//case -1:
			//	retString.Append("</S>");
			//	break;
			//default:
			//	break;
		}

		switch(Bold){
		case 1:
			retString.Append("<B>");
			break;
			//case -1:
			//	retString.Append("</B>");
			//	break;
			//default:
			//	break;
		}

		switch(Underline){
		case 1:
			retString.Append("<U>");
			break;
			//case -1:
			//	retString.Append("</U>");
			//	break;
			//default:
			//	break;
		}
	} else {
		switch(Underline){
			case -1:
				retString.Append("</U>");
				break;
		}
		switch(Bold){
			case -1:
				retString.Append("</B>");
				break;
		}

		switch(Strikeout){
			case -1:
				retString.Append("</S>");
				break;
		}

		switch(Italic){
			case -1:
				retString.Append("</I>");
				break;
		}
	}

	// SAMPLE
	// <FONT COLOR="#FF0000">
	// <FONT FACE="Arial">

	return retString;
}


void CRichUOLView::InsertTagText(CString text)
{
	long start, end;
	GetSel(start,end);
	InsertText(start,text);
}

CString CRichUOLView::GetSelectedText(TextType textType)
{
	CComCritSecLock<CComAutoCriticalSection> lock(m_csEmoticonInfo);
	
	TCHAR inputString[MAX_CHARACTER_MESSAGE_UOL+1]=_T("");
	TCHAR outputString[MAX_CHARACTER_MESSAGE_UOL_TAG+1]=_T("");
	CString prefix, posfix;
	int i_input=0;
	int i_output=0;
	int objCount=0;
	int i_obj=0;
	IRichEditOle * olePtr = GetOleInterface() ;
	CString tmpTag;
	CHARFORMAT cf,cfOld;

	prefix="";
	posfix="";


	ATLASSERT(olePtr!=NULL);

	//GetSelText(inputString);
	GetWindowText(inputString, MAX_CHARACTER_MESSAGE_UOL);
	objCount = olePtr->GetObjectCount();

	if (::_tcslen(inputString)<=0)
		return "";

	if (textType==HTML_TEXT) {
		CString tmpColor;
		cfOld.dwEffects = 0;
		GetDefaultCharFormat(cf);
		tmpColor.Format("#%02X%02X%02X",GetRValue(cf.crTextColor),
			GetGValue(cf.crTextColor),
			GetBValue(cf.crTextColor));
		prefix.Format("<FONT FACE=\"%s\">%s<FONT COLOR=\"%s\">"
			,cf.szFaceName,FindTag(cfOld,cf,TRUE),
			tmpColor);
	}
	// Replace EMOTICONS by EMOTICON TAG
	for (i_obj=0;i_obj<objCount;i_obj++) {

		// Get the i_obj object 
		REOBJECT reobject;
		ZeroMemory(&reobject, sizeof(REOBJECT));
		reobject.cbStruct = sizeof(REOBJECT);	
		olePtr->GetObject(i_obj,&reobject,REO_GETOBJ_NO_INTERFACES);

		// Fill output until find a object
		while(i_input<reobject.cp) {
			outputString[i_output++]=inputString[i_input++];
		}

		// Get Tag to insert.
		EmoticonInfo* pEmoticonInfo = (EmoticonInfo*)reobject.dwUser;
		
		if (pEmoticonInfo)
		{
			int tagLen = pEmoticonInfo->strOriginalText.GetLength();
			for(int i_tag=0;i_tag<tagLen;i_tag++) {
				outputString[i_output++]=pEmoticonInfo->strOriginalText.GetAt(i_tag);
			}
		}

		// Go to next character in input
		i_input++;
	}

	while(inputString[i_input]!=NULL) {

		
		outputString[i_output]=inputString[i_input];
		i_input++;
		i_output++;
	}
	
	if (textType==HTML_TEXT) {
		posfix.Format("</FONT>%s</FONT>",FindTag(cf,cfOld,FALSE));
	}

	
	outputString[i_output]=NULL;
	
	CString tmpRet;
	tmpRet = outputString;
	if (textType==HTML_TEXT) {
		tmpRet.Replace("&","&amp;");
		tmpRet.Replace("<","&lt;");
		tmpRet.Replace(">","&gt;");
		tmpRet.Replace("\r\n","<BR>");
		tmpRet.Replace("\n","<BR>");
		tmpRet.Replace("\r","<BR>");
	}
	tmpRet = prefix+tmpRet+posfix;
	
	return tmpRet;

}

void CRichUOLView::SetEditPlainText(CString strPlainText)
{
	SetWindowText(strPlainText);
	Parse();
}

HBITMAP CRichUOLView::LoadPicture(LPCTSTR pszPath, LONG& cyHimetric, LONG& cxHimetric)
{
	HBITMAP hPicRet = NULL;
	
	if(pszPath[0]==0)
		return NULL;

	WCHAR wpath[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, pszPath, -1, wpath, MAX_PATH);

	IPicture* pPic;
	OleLoadPicturePath(wpath, NULL, NULL, NULL, IID_IPicture,(LPVOID*)&pPic);

	if (pPic != NULL)
	{
		HBITMAP hPic = NULL;
		pPic->get_Handle((UINT*)&hPic);

		DWORD pDwAttr = 0;
		pPic->get_Attributes(&pDwAttr);

		pPic->get_Width(&cxHimetric);
		pPic->get_Height(&cyHimetric);
		
		if (PICTURE_TRANSPARENT && pDwAttr)
		{
			hPicRet = (HBITMAP)CopyImage(hPic, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG | LR_LOADTRANSPARENT);
		}
		else
		{
			hPicRet = (HBITMAP)CopyImage(hPic, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
		}

		pPic->Release();
	}
	
	return hPicRet;
}

LRESULT CRichUOLView::OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}

void CRichUOLView::CopyToClipboard(void)
{
	Copy();
}

LRESULT CRichUOLView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetEventMask(ENM_NONE);
	SaveFontSettings();
	ClearEditText();
	return 0;
}

void CRichUOLView::InvertEffect (DWORD dwMask, DWORD dwEffect)
{
	CHARFORMAT2 cf;	

	GetCharFormatEx(cf, dwMask);	

	if (cf.dwEffects & dwEffect)
		cf.dwEffects &= ~dwEffect;
	else
		cf.dwEffects |= dwEffect;

	SetCharFormatEx(cf, dwMask);	
}


void CRichUOLView::InvertAllEffect (DWORD dwMask, DWORD dwEffect)
{
	CHARFORMAT2 cf;	

	GetDefaultCharFormat(cf);

	if (cf.dwEffects & dwEffect)
		cf.dwEffects &= ~dwEffect;
	else
		cf.dwEffects |= dwEffect;

	SetDefaultCharFormat(cf);
}


BOOL CRichUOLView::IsBold()
{
	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);
	return (cf.dwEffects & CFE_BOLD);
}

BOOL CRichUOLView::IsItalic()
{
	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);
	return (cf.dwEffects & CFE_ITALIC);
}

BOOL CRichUOLView::IsUnderline()
{
	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);
	return (cf.dwEffects & CFE_UNDERLINE);
}

BOOL CRichUOLView::IsStrikeout()
{
	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);
	return (cf.dwEffects & CFE_STRIKEOUT);
}

void CRichUOLView::SetBold(BOOL state)
{
	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);
	if (state) 
		cf.dwEffects |= CFE_BOLD;
	else
		cf.dwEffects &= ~CFE_BOLD;
	SetDefaultCharFormat(cf);
}

void CRichUOLView::SetItalic(BOOL state)
{
	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);
	if (state) 
		cf.dwEffects |= CFE_ITALIC;
	else
		cf.dwEffects &= ~CFE_ITALIC;
	SetDefaultCharFormat(cf);
}

void CRichUOLView::SetUnderline(BOOL state)
{
	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);
	if (state) 
		cf.dwEffects |= CFE_UNDERLINE;
	else
		cf.dwEffects &= ~CFE_UNDERLINE;
	SetDefaultCharFormat(cf);
}

void CRichUOLView::SetStrikeout(BOOL state)
{
	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);
	if (state) 
		cf.dwEffects |= CFE_STRIKEOUT;
	else
		cf.dwEffects &= ~CFE_STRIKEOUT;
	SetDefaultCharFormat(cf);
}

void CRichUOLView::ChangeFontBold()
{
	InvertAllEffect(CFM_BOLD,CFE_BOLD);
}

void CRichUOLView::ChangeFontItalic()
{
	InvertAllEffect(CFM_ITALIC,CFE_ITALIC);
}

void CRichUOLView::ChangeFontUnderline()
{
	InvertAllEffect(CFM_UNDERLINE, CFE_UNDERLINE);
}

void CRichUOLView::ChangeFontStrikeout()
{
	InvertAllEffect(CFM_STRIKEOUT, CFE_STRIKEOUT);
}

void CRichUOLView::ChangeFontTextColor()
{
	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);
	CColorDialog dlg(cf.crTextColor);
	if (dlg.DoModal()!=IDCANCEL)  {
		m_clrTextColor = dlg.m_cc.rgbResult; 
		SetColor(m_clrTextColor, ID_FORMAT_COLOR);
	}
}

void CRichUOLView::ChangeFontTextBkColor()
{
	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);
	//CColorDialog dlg(cf.crTextColor);
	CColorDialog dlg(cf.crBackColor);
	if (dlg.DoModal()!=IDCANCEL)  {
		m_clrHighlightColor = dlg.m_cc.rgbResult; 
		SetColor(m_clrHighlightColor, ID_FORMAT_HIGHLIGHT);
	}
}


void CRichUOLView::ChooseFont()
{
	ChangeFont();
}

void CRichUOLView::SaveFontSettings()
{
	CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->EnableFontBold(IsBold());
	CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->EnableFontUnderline(IsUnderline());
	CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->EnableFontItalic(IsItalic());
	CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->EnableFontStrikeout(IsStrikeout());

	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);

	CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->SetFontSize(cf.yHeight);
	CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->SetFontColor(cf.crTextColor);
	CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->SetFontFace(cf.szFaceName);
}

void CRichUOLView::LoadFontSettings()
{
	BOOL bItalic			= FALSE;
	BOOL bBold				= FALSE;
	BOOL bUnderline			= FALSE;
	BOOL bStrikeout			= FALSE;

	bBold = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->IsFontBoldEnabled();
	bUnderline = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->IsFontUnderlineEnabled();
	bItalic = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->IsFontItalicEnabled();
	bStrikeout = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->IsFontStrikeoutEnabled();

	
	CHARFORMAT2 cf;	
	GetDefaultCharFormat(cf);

	cf.yHeight = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->GetFontSize();
	cf.crTextColor = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->GetFontColor();
	CString fontFaceName = CUIMApplication::GetApplication()->GetSettingsManager()->GetMessageSettings()->GetFontFace();
	if (fontFaceName.GetLength()>0)
		strcpy(cf.szFaceName, fontFaceName);
	else {
		strcpy(cf.szFaceName, _T("Arial"));
		cf.yHeight=200;
	}

	//cf.crTextColor = m_clrTextColor;
	//cf.crBackColor = m_clrBackColor;
	if (bItalic) 
		cf.dwEffects |= CFE_ITALIC;
	else
		cf.dwEffects &= ~CFE_ITALIC;

	if (bBold) 
		cf.dwEffects |= CFE_BOLD;
	else
		cf.dwEffects &= ~CFE_BOLD;


	if (bUnderline) 
		cf.dwEffects |= CFE_UNDERLINE;
	else
		cf.dwEffects &= ~CFE_UNDERLINE;

	if (bStrikeout) 
		cf.dwEffects |= CFM_STRIKEOUT;
	else
		cf.dwEffects &= ~CFM_STRIKEOUT;


	cf.dwMask = CFM_BOLD|CFM_ITALIC|CFM_STRIKEOUT|CFM_UNDERLINE|CFM_SIZE|CFM_FACE;

	SetDefaultCharFormat(cf);
	SetColor(cf.crTextColor,ID_FORMAT_COLOR);
}



inline void CRichUOLView::GetCharFormatEx(CHARFORMAT2& cf, DWORD dwMask)
{	
	cf.dwMask |= dwMask;
	cf.cbSize = sizeof CHARFORMAT2;
	SendMessage(EM_GETCHARFORMAT, 1, (LPARAM)&cf);
}

void CRichUOLView::SetCharFormatEx(CHARFORMAT2& cf, DWORD dwMask)
{
	cf.cbSize = sizeof CHARFORMAT2;
	cf.dwMask |= dwMask;

	CHARRANGE cr;
	GetSel(cr);
	if (cr.cpMax != cr.cpMin)
		SendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);	
	else
		SendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);		
}

void CRichUOLView::SetColor (COLORREF clrNew, /*COLORREF clrOld,*/ DWORD iItem)
{
	CHARFORMAT2 cf;
	cf.cbSize = sizeof CHARFORMAT2;
	cf.dwEffects = 0;		

	if (ID_FORMAT_COLOR == iItem) 
	{
		cf.dwMask = CFM_COLOR;
		cf.crTextColor = clrNew;
		//SendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);	
		SendMessage(EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);	
		m_clrTextColor = clrNew;
	}
	else if (ID_FORMAT_HIGHLIGHT == iItem) 
	{
		cf.dwMask = CFM_BACKCOLOR;
		cf.crBackColor = clrNew;
		//SendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf);	
		SendMessage(EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);	
		m_clrHighlightColor = clrNew;
	}		
	else 
	{
		SendMessage(EM_SETBKGNDCOLOR, 0, clrNew);
		m_clrBackColor = clrNew;
		::InvalidateRect(GetParent(), 0, TRUE);
	}
}


// RichUOLView can't show PNG images very well, so if emoticon is a PNG return false
BOOL CRichUOLView::IsValidImageType(CString strFileName)
{
	if (strFileName.IsEmpty())
	{
		return FALSE;
	}

	CString strFileExt;
	strFileExt = ATLPath::FindExtension(strFileName);

	return (0 != strFileExt.CompareNoCase(".png"));
}



CRichUOLView::IExRichEditOleCallback::IExRichEditOleCallback()
{
	pStorage = NULL;
	m_iNumStorages = 0;
	m_dwRef = 0;

	// set up OLE storage

	HRESULT hResult = ::StgCreateDocfile(NULL,
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE /*| STGM_DELETEONRELEASE */|STGM_CREATE ,
		0, &pStorage );

	if ( pStorage == NULL ||
		hResult != S_OK )
	{
//		AfxThrowOleException( hResult );
	}
}

CRichUOLView::IExRichEditOleCallback::~IExRichEditOleCallback()
{
}


BOOL CRichUOLView::IsValidObject(IDataObject *pDataObject)
{
	// Dragging Shell Files
	FORMATETC fmtetcFiles = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	if (m_pTransferDataObject != pDataObject)
	{
		m_pTransferDataObject = pDataObject;
	}

	// Accept only "Shell Files"
	return (pDataObject->QueryGetData(&fmtetcFiles) == S_OK);
}


DWORD CRichUOLView::DoDragEnter(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	DWORD dwEffect = DROPEFFECT_NONE;

	// Verify if drag object is Shell File
	FORMATETC fmtetcFiles = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	if (m_pTransferDataObject)
	{
		if (dwAllowed && (m_pTransferDataObject->QueryGetData(&fmtetcFiles) == S_OK))
		{
			dwEffect = DROPEFFECT_COPY;
		}
	}

	return dwEffect;
}


DWORD CRichUOLView::DoDragOver(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	DWORD dwEffect = DROPEFFECT_NONE;

	// Verify if drag object is Shell File
	FORMATETC fmtetcFiles = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	if (m_pTransferDataObject)
	{
		if (dwAllowed && (m_pTransferDataObject->QueryGetData(&fmtetcFiles) == S_OK))
		{
			dwEffect = DROPEFFECT_COPY;
		}
	}

	return dwEffect;
}


void CRichUOLView::DoDragLeave()
{
	if (m_pTransferDataObject)
	{
		m_pTransferDataObject = NULL;
	}
}


DWORD CRichUOLView::DoDrop(IDataObject *pDataObject, POINTL pt)
{
	DWORD dwEffect = DROPEFFECT_NONE;

	// Verify if drag object is Shell File
	BOOL bIsShellFile = FALSE;
	FORMATETC fmtetcFiles = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	if (m_pTransferDataObject)
	{
		if (pDataObject->QueryGetData(&fmtetcFiles) == S_OK)
		{
			bIsShellFile = TRUE;
			ProcessDropFiles(pDataObject);
		}
	}

	return dwEffect;
}


void CRichUOLView::ProcessDropFiles(IDataObject* pDataObject)
{
	FORMATETC fmtetcFiles = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmed;
	if (pDataObject->GetData(&fmtetcFiles, &stgmed) == S_OK)
	{
		HDROP hdrop = (HDROP)GlobalLock(stgmed.hGlobal);

		if (hdrop != NULL)
		{
			TCHAR szFileName[_MAX_PATH + 1];
			UINT nFiles = DragQueryFile(hdrop, (UINT)-1, NULL, 0);
			for(UINT nNames = 0; nNames < nFiles; nNames++)
			{
				ZeroMemory(szFileName, _MAX_PATH + 1);
				DragQueryFile(hdrop, nNames, (LPTSTR)szFileName, _MAX_PATH + 1);

				USERDROPPEDFILEPARAM param;
				param.strFileName = szFileName;

				CWindow tmpWindow;
				tmpWindow = GetParent();
				if (tmpWindow.IsWindow()) 
				{
					tmpWindow = tmpWindow.GetParent();
					if (tmpWindow.IsWindow()) 
					{
						SendMessage(tmpWindow, WM_USER_DROPPED_FILE, (WPARAM)&param, 0);
					}
				}
			}
			GlobalUnlock(hdrop);
		}
		ReleaseStgMedium(&stgmed);
	}
}


HRESULT STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::GetNewStorage(LPSTORAGE* lplpstg)
{
	m_iNumStorages++;
	WCHAR tName[50];
	swprintf(tName, L"REOLEStorage%d", m_iNumStorages);

	HRESULT hResult = pStorage->CreateStorage(tName, 
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
		0, 0, lplpstg );

	if (hResult != S_OK )
	{
		//::AfxThrowOleException( hResult );
	}

	return hResult;
}

HRESULT STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = S_OK;
	*ppvObject = NULL;

	if ( iid == IID_IUnknown ||
		iid == IID_IRichEditOleCallback )
	{
		*ppvObject = this;
		AddRef();
		hr = NOERROR;
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}



ULONG STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::AddRef()
{
	return ++m_dwRef;
}



ULONG STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::Release()
{
	if ( --m_dwRef == 0 )
	{
		delete this;
		return 0;
	}

	return m_dwRef;
}


HRESULT STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
															LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	//return E_NOTIMPL;
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	//return E_NOTIMPL;
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	//return E_NOTIMPL;
	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
	//return E_NOTIMPL;
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, 
										CLIPFORMAT FAR *lpcfFormat,
										DWORD reco,
										BOOL fReally,
										HGLOBAL hMetaPict)
{
	ATLASSERT(lpcfFormat != NULL);
	if (!fReally) // not actually pasting
		return S_OK;
	if (*lpcfFormat == CF_TEXT)
		return S_OK;
//	STGMEDIUM tmp;
	
	CComPtr<IDataObject> data(lpdataobj);
	FORMATETC fe;
	fe.cfFormat = CF_TEXT;
	fe.ptd = NULL;
	fe.dwAspect = DVASPECT_CONTENT;
	fe.lindex = -1;
	fe.tymed = TYMED_HGLOBAL;
	lpdataobj = data;

	if (S_OK == data->QueryGetData(&fe)) {
		*lpcfFormat = CF_TEXT;
		return S_OK;
	} else {
		return S_FALSE;
	}
}


HRESULT STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
	//return E_NOTIMPL;
	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
	return E_NOTIMPL;
	//return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	//return E_NOTIMPL;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
CRichUOLView::IExRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
														 HMENU FAR *lphmenu)
{
	//return E_NOTIMPL;
	return S_OK;
}

