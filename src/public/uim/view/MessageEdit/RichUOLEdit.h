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
#include "HyperLink.h"
#include "../DummyWindow.h"

#include "../../controller/ParseEmoticon.h"
#include <interfaces/IUOLMessengerConversation.h>
#include <DropTarget.h>

//
// CRichUOLView
//

class CRichTextEventsEventListener
{
public:
	CRichTextEventsEventListener(){};
	virtual ~CRichTextEventsEventListener() {};
	virtual void OnTextAreaChar() {}
};



//
// CRichUOLView
//
class CRichUOLView;
typedef CDummyWindow<CWindowImpl<CRichUOLView, CRichEditCtrl> > CRichUOLViewBase;
typedef CDropTarget<CRichUOLViewBase> CRichUOLViewTarget;

class CRichUOLView : public CRichUOLViewTarget
{
	enum TextType {
		PLAIN_TEXT = 1,	HTML_TEXT
	};

	enum UserMessages
	{
		MEP_EDIT_RETURN = WM_USER + 0x150,	// sent by WM_COMMAND
		WM_USER_DROPPED_FILE = WM_USER + 0x160
	};

	typedef struct _USERDROPPEDFILEPARAM
	{
		CString strFileName;
	} USERDROPPEDFILEPARAM, *LPUSERDROPPEDFILEPARAM;

public:
	DECLARE_WND_CLASS("CRichUOLView")
	// Font 
	BOOL IsBold();
	BOOL IsItalic();
	BOOL IsUnderline();
	BOOL IsStrikeout();
	void SetBold(BOOL state);
	void SetItalic(BOOL state);
	void SetUnderline(BOOL state);
	void SetStrikeout(BOOL state);
	void ChangeFontStrikeout();
	void ChangeFontBold();
	void ChangeFontItalic();
	void ChangeFontUnderline();
	void ChangeFontTextColor();
	void ChangeFontTextBkColor();
	void ChooseFont();
	void SaveFontSettings();
	void LoadFontSettings();

	// Emoticons
	void InsertTagText(CString text);

	// Default Constructor
	CRichUOLView();
	~CRichUOLView();
	
	// RichEdit Commands
	void ClearEditText();
	void SetListener(CRichTextEventsEventListener *listener);
	void ChangeFont();
	void CopyToClipboard(void);
	CString GetEditPlainText();
	CString GetEditHTMLText();
	CString GetSelectedText(TextType textType);
	
	void SetEditPlainText(CString strPlainText);

	//Development
	void Trace();
	void Test();
	void Parse();

	// Necessary parsing info.
	void SetConversation(IUOLMessengerConversationPtr pConversation);
	
protected:
	// CDropTarget methods
	virtual BOOL IsValidObject(IDataObject *pDataObject);
	virtual DWORD DoDragEnter(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
	virtual DWORD DoDragOver(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
	virtual void DoDragLeave();
	virtual DWORD DoDrop(IDataObject *pDataObject, POINTL pt);

	void ProcessDropFiles(IDataObject* pDataObject);

	struct EmoticonInfo
	{
		int nTagIndex;
		CString strOriginalText;
	};

private:	
	int AddTag(TCHAR * target, TCHAR *source);
	CString FindTag(CHARFORMAT oldeffects, CHARFORMAT neweffects,BOOL flag_begin);
	
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}
	BEGIN_MSG_MAP(CRichUOLView)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		REFLECTED_NOTIFY_CODE_HANDLER(EN_LINK, OnLink)
		CHAIN_MSG_MAP(CRichUOLViewTarget)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnChar(UINT uMsg, WPARAM wParam,LPARAM lParam, 	BOOL& bHandled);
	LRESULT OnKeyUp(UINT uMsg, WPARAM wParam,LPARAM lParam, 	BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam,LPARAM lParam, 	BOOL& bHandled);
	LRESULT OnChange(WORD, WORD, HWND, BOOL&);
	LRESULT OnLink(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void InvertEffect (DWORD dwMask, DWORD dwEffect);
	void InvertAllEffect (DWORD dwMask, DWORD dwEffect);
	HBITMAP LoadPicture(LPCTSTR pszPath, LONG& cyHimetric, LONG& cxHimetric);

	BOOL			m_newChar;
	
	// Ole support
	LPRICHEDITOLE	m_pRichEditOle;
	LPOLEOBJECT		m_lpObject;
	LPSTORAGE       m_lpStorage;
	LPOLECLIENTSITE	m_lpClientSite;
	
	//Listener
	CRichTextEventsEventListener * m_listener;
	
	//Font
	void GetCharFormatEx(CHARFORMAT2& cf, DWORD dwMask);
	void SetCharFormatEx(CHARFORMAT2& cf, DWORD dwMask);
	void SetColor (COLORREF clrNew/*, COLORREF clrOld*/, DWORD iItem);
	BOOL IsValidImageType(CString strFileName);

	COLORREF m_clrTextColor;
	COLORREF m_clrHighlightColor;
	COLORREF m_clrBackColor;

	IUOLMessengerConversationPtr	m_pConversation;
	IDataObject*					m_pTransferDataObject;
	
	CComAutoCriticalSection			m_csEmoticonInfo;

public:
	interface IExRichEditOleCallback : public IRichEditOleCallback
	{
	public:
		IExRichEditOleCallback() ;
		virtual ~IExRichEditOleCallback();
		int m_iNumStorages;
		IStorage* pStorage;
		DWORD m_dwRef;

		virtual HRESULT STDMETHODCALLTYPE GetNewStorage(LPSTORAGE* lplpstg);
		virtual HRESULT STDMETHODCALLTYPE 
			QueryInterface(REFIID iid, void ** ppvObject);
		virtual ULONG STDMETHODCALLTYPE AddRef();
		virtual ULONG STDMETHODCALLTYPE Release();
		virtual HRESULT STDMETHODCALLTYPE 
			GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame, 
			LPOLEINPLACEUIWINDOW FAR *lplpDoc, 
			LPOLEINPLACEFRAMEINFO lpFrameInfo);
		virtual HRESULT STDMETHODCALLTYPE ShowContainerUI(BOOL fShow);
		virtual HRESULT STDMETHODCALLTYPE 
			QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp);
		virtual HRESULT STDMETHODCALLTYPE DeleteObject(LPOLEOBJECT lpoleobj);
		virtual HRESULT STDMETHODCALLTYPE 
			QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat, 
			DWORD reco, BOOL fReally, HGLOBAL hMetaPict);
		virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode);
		virtual HRESULT STDMETHODCALLTYPE 
			GetClipboardData(CHARRANGE FAR *lpchrg, 
			DWORD reco, LPDATAOBJECT FAR *lplpdataobj);
		virtual HRESULT STDMETHODCALLTYPE 
			GetDragDropEffect(BOOL fDrag, 
			DWORD grfKeyState, LPDWORD pdwEffect);
		virtual HRESULT STDMETHODCALLTYPE 
			GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, 
			CHARRANGE FAR *lpchrg, HMENU FAR *lphmenu);
	};
};