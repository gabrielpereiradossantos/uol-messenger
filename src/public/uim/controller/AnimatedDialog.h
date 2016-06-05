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

#include "../resource.h"

#include "MaskedBitmap.h"
#include "StaticLink.h"
#include "utils.h"

#define AW_HOR_POSITIVE             0x00000001
#define AW_HOR_NEGATIVE             0x00000002
#define AW_VER_POSITIVE             0x00000004
#define AW_VER_NEGATIVE             0x00000008
#define AW_CENTER                   0x00000010
#define AW_HIDE                     0x00010000
#define AW_ACTIVATE                 0x00020000
#define AW_SLIDE                    0x00040000
#define AW_BLEND                    0x00080000

#define EXITIFKILLRECEIVED() ExitIfKillReceived();

class CAnimatedDialog : public CDialogImpl<CAnimatedDialog>
{
/*
	// BEGIN_MSG_MAP(CAnimatedDialog)  expanded for debugging purposes
	public:
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) \
	{ \
		BOOL bHandled = TRUE; \
		(hWnd);(uMsg);(wParam);(lParam);(lResult);(bHandled); \
		CString strMsg; \
		strMsg.Format("%04x",(int)uMsg); \
		switch(uMsg) { \
			case WM_WINDOWPOSCHANGING: \
				strMsg="WM_WINDOWPOSCHANGING"; \
				break; \
			case WM_WINDOWPOSCHANGED: \
				strMsg="WM_WINDOWPOSCHANGED"; \
				break; \
			case WM_SHOWWINDOW: \
				strMsg="WM_SHOWWINDOW"; \
				break; \
			case WM_PRINT: \
				strMsg="WM_PRINT"; \
				break; \
			case WM_INITDIALOG: \
				strMsg="WM_INITDIALOG"; \
				break; \
			case WM_CREATE: \
				strMsg="WM_CREATE"; \
				break; \
			case WM_NCCALCSIZE: \
				strMsg="WM_NCCALCSIZE"; \
				break; \
			case WM_DESTROY: \
				strMsg="WM_DESTROY"; \
				break; \
			case WM_MOVE: \
				strMsg="WM_MOVE"; \
				break; \
			case WM_SIZE: \
				strMsg="WM_SIZE"; \
				break; \
			case WM_ACTIVATE: \
				strMsg="WM_ACTIVATE"; \
				break; \
			case WM_SETFOCUS: \
				strMsg="WM_SETFOCUS"; \
				break; \
			case WM_KILLFOCUS: \
				strMsg="WM_KILLFOCUS"; \
				break; \
			case WM_SETFONT: \
				strMsg="WM_SETFONT"; \
				break; \
			case WM_ACTIVATEAPP: \
				strMsg="WM_ACTIVATEAPP"; \
				break; \
			case WM_NCACTIVATE: \
				strMsg="WM_NCACTIVATE"; \
				break; \
			case WM_PAINT: \
				strMsg="WM_PAINT"; \
				break; \
		ATLTRACE("ProcWM: hWnd=08%x uMsg=%s wParam=%08x lParam=%d map=%d\n",hWnd,(LPCTSTR)strMsg,(int)wParam,lParam,dwMsgMapID);  \
		switch(dwMsgMapID) {  \
		case 0:
*/

	BEGIN_MSG_MAP(CAnimatedDialog)
		EXITIFKILLRECEIVED()
		MESSAGE_HANDLER( WM_WINDOWPOSCHANGED, OnWindowPosChanged )
		MESSAGE_HANDLER( WM_WINDOWPOSCHANGING, OnWindowPosChanging )
		MESSAGE_HANDLER( WM_ERASEBKGND, OnEraseBkgnd )
		//MESSAGE_HANDLER( WM_PAINT, OnPaint )
		MESSAGE_HANDLER( WM_SHOWWINDOW, OnShowWindow )
		MESSAGE_HANDLER( CStaticLink::WM_STATIC_LINK_CLICK , OnStaticLinkClick );
		MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
		MESSAGE_HANDLER( WM_QUERYNEWPALETTE, OnQueryNewPalette )
		MESSAGE_HANDLER( WM_PALETTECHANGED, OnPaletteChanged )
		MESSAGE_HANDLER( WM_QUERYENDSESSION , OnStaticLinkClick )
		//MESSAGE_HANDLER( WM_PRINT, OnPrint )
		REFLECT_NOTIFICATIONS()      // reflect messages back to static links
	END_MSG_MAP()

// Construction
public:
	enum { IDD = IDD_DIALOG_NEW_VERSION_NOTIFY };

	CAnimatedDialog(IShutdownFlag& shutdownFlag) : 
		m_shutdownFlag(shutdownFlag),
		m_iUpdateRequested(0), 
		m_dwTime(0),
		m_strText(_T("")),
		m_strTextLink(_T("")),
		m_pCommand(),
		m_hUser32(NULL),
		//m_rgbFrom(RGB(0xff,0x82,0x00)),
		//m_rgbTo(RGB(0xff,0xff,0x96)),
		m_rgbFrom(RGB(133,196,235)),
		m_rgbTo(RGB(21,128,165)),
		m_staticNotify(RGB(0x00,0x00,0x00), RGB(0x00,0x00,0x00), FALSE, 16, TRUE),		
		m_staticClose(RGB(0xff,0xff,0xff), RGB(0xff,0xff,0xff), FALSE, 14, FALSE, FALSE, FALSE)
	{
	}

	virtual ~CAnimatedDialog()
	{
		if (m_hUser32) {
			::FreeLibrary(m_hUser32);
		}
	}

	void SetTime(DWORD dwTime)
	{
		m_dwTime=dwTime;
		m_hUser32 = ::LoadLibrary("User32.dll");
		AnimateWindow = (BOOL (__stdcall * )(HWND, DWORD, DWORD))::GetProcAddress(m_hUser32, "AnimateWindow");
	}

	void SetText(CString strText)
	{
		m_strText = strText;
	}

	CString GetText()
	{
		return m_strText;
	}

	void SetTextLink(CString strTextLink)
	{
		m_strTextLink = strTextLink;
	}

	void SetCommand(IUOLMessengerCommandPtr pCommand)
	{
		m_pCommand = pCommand;
	}

	CString GetTextLink()
	{
		return m_strTextLink;
	}

protected:
	LRESULT OnStaticLinkClick ( UINT, WPARAM iDlgID , LPARAM, BOOL& )
	{
		if(IDC_STATIC_NEW_VERSION_MESSAGE == iDlgID) 
		{
			m_iUpdateRequested=1;

			if (m_pCommand.get())
			{
				CUIMApplication::GetApplication()->ExecuteCommand(m_pCommand);
			}
		}

		

		/*
		if (AnimateWindow) {
			DWORD dwFlags = AW_SLIDE;
			dwFlags |= FALSE ? (AW_ACTIVATE | AW_VER_NEGATIVE) : (AW_HIDE | AW_VER_POSITIVE);
			AnimateWindow(this->m_hWnd, m_dwTime, dwFlags);		
		}
		else {*/
			ShowWindow(SW_HIDE);
		//}

		EndDialog(m_iUpdateRequested);
		return TRUE;
	}

	LRESULT OnPaletteChanged ( UINT, WPARAM hPalChangeWnd, LPARAM, BOOL& )
	{
		if ((HPALETTE) m_palette == NULL)   // Shouldn't happen, but
			return 1;                         // let's be sure.

		if ((HWND)hPalChangeWnd != this->m_hWnd) 
		{
			CClientDC dc (this->m_hWnd);
			HPALETTE hOldPalette = dc.SelectPalette (m_palette.m_hPalette, FALSE);

			if (dc.RealizePalette ())
			{
				Invalidate ();
			}

			dc.SelectPalette (hOldPalette, FALSE);
		}

		return 0;
	}

	LRESULT OnQueryNewPalette( UINT, WPARAM , LPARAM, BOOL& )
	{
		if ((HPALETTE) m_palette == NULL)   // Shouldn't happen, but
			return 0;                       // let's be sure.

		CClientDC dc (this->m_hWnd);
		HPALETTE hOldPalette = dc.SelectPalette (m_palette.m_hPalette, FALSE);

		UINT nCount;

		if (0 != (nCount = dc.RealizePalette ()))
		{
			Invalidate ();
		}

		dc.SelectPalette (hOldPalette, FALSE);

		return nCount;
	}

	LRESULT OnEraseBkgnd( UINT, WPARAM hDC, LPARAM, BOOL& )
	{
		CDC eraseDC((HDC)hDC);

		COLORREF clrTransparentColor = RGB(0, 255, 0);	

		m_maskedBitmap.Draw(&eraseDC, 1, 1);
		//m_maskedBitmap.Draw()

		//RECT rect;
		//GetClientRect(&rect);

		//CRect rectbar(rect);
		//CRect rectmiddle(rect);
		//CRect rectout(rect);
		//rectbar.top+=3;
		//rectbar.left+=3;
		//rectbar.right+=2;
		//rectbar.bottom = rectbar.top+35;

		//rectmiddle.top = rectbar.bottom+1;
		//rectmiddle.left+=3;
		//rectmiddle.right-=3;
		//rectmiddle.bottom-=3;


		//HPALETTE hOldPalette = NULL;

		//if (m_palette.m_hPalette != NULL) {
		//	hOldPalette = eraseDC.SelectPalette(m_palette.m_hPalette, FALSE);
		//	eraseDC.RealizePalette();
		//}

		//
		////eraseDC.SetDCPenColor(RGB(255,0,0));
		//eraseDC.SetBkMode(TRANSPARENT);
		//eraseDC.FillSolidRect(rectout,RGB(71,154,183));
		//rectout.DeflateRect(1,1,1,1);
		//eraseDC.FillSolidRect(rectout,RGB(128,186,212));
		//rectout.DeflateRect(1,1,1,1);
		//eraseDC.FillSolidRect(rectout,RGB(205,229,243));
		//rectout.DeflateRect(1,1,1,1);

		//eraseDC.FillSolidRect(rectmiddle,RGB(193,222,240));


		//DoGradientFill (&eraseDC, &rectbar);

		//if (m_palette.m_hPalette != NULL)
		//{
		//	hOldPalette = eraseDC.SelectPalette(hOldPalette, FALSE);		
		//}

		return 1;
	}

	/*
	LRESULT OnPrint( UINT, WPARAM hDC, LPARAM, BOOL& )
	{
		CDC eraseDC((HDC)hDC);

		RECT rect;
		GetClientRect(&rect);

		HPALETTE hOldPalette = NULL;

		if (m_palette.m_hPalette != NULL) {
			hOldPalette = eraseDC.SelectPalette(m_palette.m_hPalette, FALSE);
			eraseDC.RealizePalette();
		}

		DoGradientFill (&eraseDC, &rect);

		if (m_palette.m_hPalette != NULL)
		{
			hOldPalette = eraseDC.SelectPalette(hOldPalette, FALSE);		
		}

		return 1;
	}
	*/

	LRESULT OnInitDialog( UINT, WPARAM, LPARAM, BOOL& )
	{
		m_bKillReceived = FALSE;

		m_staticClose.SubclassWindow(GetDlgItem(IDC_STATIC_NEW_VERSION_CLOSE));
		m_staticNotify.SubclassWindow(GetDlgItem(IDC_STATIC_NEW_VERSION_MESSAGE));

		m_staticNotify.SetWindowText(m_strText);
		m_staticNotify.SetLinkText(m_strTextLink);

		m_staticClose.SetLinkText(_T(""));

		RECT rect,rectThis;
		APPBARDATA appBarData;

		::ZeroMemory(&appBarData, sizeof(APPBARDATA));
		appBarData.cbSize = sizeof(APPBARDATA);

		::GetWindowRect(::GetDesktopWindow(), &rect);

		::SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData);	

		GetWindowRect(&rectThis);

		int iX = (rect.right-rect.left) - (rectThis.right-rectThis.left) - 15;
		int iY;

		if (ABE_BOTTOM == appBarData.uEdge) {	
			iY = appBarData.rc.top - (rectThis.bottom-rectThis.top);
		}
		else {
			iY = (rect.bottom-rect.top) - (rectThis.bottom-rectThis.top) - 30;
		}

		::SetWindowPos(this->m_hWnd,HWND_TOPMOST, iX, iY, 0, 0, SWP_NOSIZE);


		CClientDC dc(this->m_hWnd);
		m_maskedBitmap.LoadBitmap(IDB_BITMAP_NOTIFY);	
		if (dc.GetDeviceCaps(RASTERCAPS) & RC_PALETTE)	{
			m_palette.Attach(::CreateHalftonePalette(NULL));
		}

		SetTimer(0, 100, NULL);

		return 0;
	}



	/*
	LRESULT  OnPaint(UINT, WPARAM, LPARAM , BOOL& ) 
	{

		PAINTSTRUCT ps;
		BOOL boolHandled;
		RECT rc;
		if(GetUpdateRect(&rc)) {
			HDC paintDC = BeginPaint(&ps);
			OnEraseBkgnd( 0, (WPARAM)paintDC, 0, boolHandled );
			EndPaint(&ps);
		}
		return 0;
	}
	*/

#ifdef _DEBUG
	CString GetStrFlags(UINT flags)
	{
		CAtlString strRepr;

		if(flags & SWP_DRAWFRAME) {
			strRepr.Append(" SWP_DRAWFRAME ");
		}
		if(flags & SWP_FRAMECHANGED) {
			strRepr.Append(" SWP_FRAMECHANGED ");
		}
		if(flags & SWP_HIDEWINDOW) {
			strRepr.Append(" SWP_HIDEWINDOW ");
		}
		if(flags & SWP_NOACTIVATE) {
			strRepr.Append(" SWP_NOACTIVATE ");
		}
		if(flags & SWP_NOCOPYBITS) {
			strRepr.Append(" SWP_NOCOPYBITS ");
		}
		if(flags & SWP_NOMOVE) {
			strRepr.Append(" SWP_NOMOVE ");
		}
		if(flags & SWP_NOOWNERZORDER) {
			strRepr.Append(" SWP_NOOWNERZORDER ");
		}
		if(flags & SWP_NOREDRAW) {
			strRepr.Append(" SWP_NOREDRAW ");
		}
		if(flags & SWP_NOREPOSITION) {
			strRepr.Append(" SWP_NOREPOSITION ");
		}
		if(flags & SWP_NOSENDCHANGING) {
			strRepr.Append(" SWP_NOSENDCHANGING ");
		}
		if(flags & SWP_NOSIZE) {
			strRepr.Append(" SWP_NOSIZE ");
		}
		if(flags & SWP_NOZORDER) {
			strRepr.Append(" SWP_NOZORDER ");
		}
		if(flags & SWP_SHOWWINDOW) {
			strRepr.Append(" SWP_SHOWWINDOW ");
		}
		return (CString)strRepr;
	}
#endif

	LRESULT OnWindowPosChanging(UINT /*iMsg*/, WPARAM /*wShow*/, LPARAM lParam, BOOL& /*boolHandled*/)
	{
		WINDOWPOS* pWindowPos=(WINDOWPOS*)lParam;
		pWindowPos->flags &= ~(UINT)SWP_NOREDRAW;
		return 0;
	}

	LRESULT OnWindowPosChanged(UINT /*iMsg*/, WPARAM /*wShow*/, LPARAM lParam, BOOL& /*boolHandled*/)
	{
		WINDOWPOS* pWindowPos=(WINDOWPOS*)lParam;
		pWindowPos->flags &= ~(UINT)SWP_NOREDRAW;
		return 0;
	}

	LRESULT OnShowWindow(UINT /*iMsg*/, WPARAM wShow, LPARAM /*lParam*/, BOOL& boolHandled) 
	{

		BOOL bShow=(BOOL)wShow;
		if (AnimateWindow) {

			DWORD dwFlags = AW_SLIDE;

			dwFlags |= bShow ? (AW_ACTIVATE | AW_VER_NEGATIVE) : (AW_HIDE | AW_VER_POSITIVE);

			AnimateWindow(this->m_hWnd, m_dwTime, dwFlags);
		}
		else {
			boolHandled=FALSE;
		}

		return 0;
	}

	void DoGradientFill(CDC* pDC, LPRECT pRect)
	{
		int nWidth = pRect->right - pRect->left;
		int nHeight = pRect->bottom - pRect->top;

		if (NULL == m_dcBackGnd.m_hDC)
		{
			CBitmap		bitmap;	

			bitmap.CreateCompatibleBitmap(pDC->m_hDC, nWidth, nHeight);

			m_dcBackGnd.CreateCompatibleDC(pDC->m_hDC);		
			m_dcBackGnd.SelectBitmap(bitmap);

			const int STEPS = 192;
			const int OFFSET = static_cast<int>(STEPS * 0.2);


			CBrush* pBrush[STEPS];

			//ATLTRACE("\nFrom: %d - %d - %d\n", GetRValue(m_rgbFrom), GetGValue(m_rgbFrom), GetBValue(m_rgbFrom));
			//ATLTRACE("\nTo: %d - %d - %d\n\n", GetRValue(m_rgbTo), GetGValue(m_rgbTo), GetBValue(m_rgbTo) );

			for (int i = 0; i < STEPS; i++)
			{
				double r = 0;
				double g = 0;
				double b = 0;


				if (i < OFFSET)
				{
					r = GetRValue(m_rgbFrom);
					g = GetGValue(m_rgbFrom);
					b = GetBValue(m_rgbFrom);
				}
				else if (i > (STEPS - OFFSET))
				{
					r = GetRValue(m_rgbTo);
					g = GetGValue(m_rgbTo);
					b = GetBValue(m_rgbTo);
				}
				else
				{
					double d =  1.6 * (static_cast<double>(i - OFFSET) / static_cast<double>(STEPS));												

					r = GetRValue(m_rgbFrom) + (((GetRValue(m_rgbTo) - GetRValue(m_rgbFrom)) * d));
					g = GetGValue(m_rgbFrom) + (((GetGValue(m_rgbTo) - GetGValue(m_rgbFrom)) * d));
					b = GetBValue(m_rgbFrom) + (((GetBValue(m_rgbTo) - GetBValue(m_rgbFrom)) * d));
				}

				/*
				CString str;
				str.Format("%i: %f - %f - %f\n", i, r, g, b);
				ATLTRACE(str);
				*/

				pBrush[i] = new CBrush ();
				pBrush[i]->CreateSolidBrush(RGB (r, g, b));
			}


			//ATLTRACE("\n\n\n");



			for (i = 0; i < nHeight; i++) 
			{
				RECT rect={0, i, nWidth, i + 1};
				m_dcBackGnd.FillRect (&rect, pBrush[(i * (STEPS - 1)) / nHeight]->m_hBrush);
			}

			for (i = 0; i < STEPS; i++)
			{
				delete pBrush[i];
			}

			/*
			DWORD dwClr;
			BITMAP bm;

			m_maskedBitmap.GetBitmap(&bm);
			m_maskedBitmap.GetBitmapBits(sizeof(DWORD), &dwClr);

			m_maskedBitmap.DrawTransparent(&m_dcBackGnd, 7, 7, ColorConvert(bm.bmBitsPixel, dwClr));
			*/

			//#if defined(RC_BOL)
			//COLORREF clrTransparentColor = RGB(255, 0, 0);

			//#elif defined(RC_UOL)
			COLORREF clrTransparentColor = RGB(0, 255, 0);		
			//#endif

			m_maskedBitmap.DrawTransparent(&m_dcBackGnd, 1, 1, clrTransparentColor);
			// TODO: use masked

		}

		pDC->BitBlt(0, 0, nWidth, nHeight, m_dcBackGnd.m_hDC, 0, 0, SRCCOPY);	
	}

	virtual void ExitIfKillReceived()
	{
		if (m_shutdownFlag.KillReceived() && !m_bKillReceived)
		{
			m_bKillReceived = TRUE;
			EndDialog(IDCANCEL);
		}
	}

protected:
	IShutdownFlag&	m_shutdownFlag;
	BOOL			m_bKillReceived;

	DWORD			m_dwTime;
	CString			m_strText;
	CString			m_strTextLink;

	IUOLMessengerCommandPtr m_pCommand;

	CStaticLink		m_staticNotify;
	CStaticLink		m_staticClose;
	int				m_iUpdateRequested;

	HMODULE			m_hUser32;
	BOOL (__stdcall * AnimateWindow)(HWND, DWORD, DWORD);

	CDC				m_dcBackGnd;
	CMaskedBitmap m_maskedBitmap;
	CPalette		m_palette;

	COLORREF		m_rgbFrom;
	COLORREF		m_rgbTo;
	
};
