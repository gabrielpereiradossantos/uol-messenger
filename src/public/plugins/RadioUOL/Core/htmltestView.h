// htmltestView.h : interface of the CHtmltestView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "WtlExt.h"

#define EVENTFN void __stdcall

//class CHtmltestView : public CWindowImpl<CHtmltestView, CAxWindow>
class CHtmltestView : public CWTLAxControl<CWTLIExplorer,IWebBrowser2>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		   (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
			return FALSE;

		// give HTML page a chance to translate this message
		return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
	}

	BEGIN_MSG_MAP(CHtmltestView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};



//#define EVENTFN void __stdcall
//
//class CWTLIExplorer : public CWTLAxControl<CWTLIExplorer,IWebBrowser2>
//{
//    public:
//        // BEGIN_MSG_MAP() ... END_MSG_MAP() is optional
//        
//        BEGIN_MSG_MAP( CWTLIExplorer  )            
//            MESSAGE_HANDLER(WM_CREATE,OnCreate) 
//        END_MSG_MAP() 
//        
//        LRESULT OnCreate(UINT uMsg, WPARAM wParam , LPARAM lParam,
//                BOOL& bHandled) 
//        { 
//
//            // First you must call CWTLAxControl<...,...>::OnCreate 
//            // ( it set bHandled = true )
//
//            return CWTLAxControl<CWTLIExplorer,IWebBrowser2,>::OnCreate(
//                    uMsg, wParam, lParam, bHandled );
//        }
//
//        // SINK_ENTRY(...,...,...) is optional, BEGIN_SINK_MAP()
//        // ... should be there
//
//        BEGIN_SINK_MAP( CWTLIExplorer )
//            SINK_ENTRY(0, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete2 )
//        END_SINK_MAP()
//        
//        EVENTFN OnNavigateComplete2( IDispatch* pDisp,  VARIANT* URL )
//        {
//            MessageBox( "OnNavigateComplete2" );
//        }
//};
//        