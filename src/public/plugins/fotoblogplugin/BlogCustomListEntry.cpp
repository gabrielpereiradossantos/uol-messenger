#include "StdAfx.h"
#include ".\blogcustomlistentry.h"
#include "FotoBlogDllWrapper.h"
//////////////////////////////////////////////////////////////////////////
// CBlogCustomListEntry
//////////////////////////////////////////////////////////////////////////

CBlogCustomListEntry::CBlogCustomListEntry(const CString& strPluginInternalName, const CString& strEntryName) :
m_strPluginInternalName(strPluginInternalName),
m_strEntryName(strEntryName)
{
	;
	m_icon = g_pUOLMessengerFacade->GetUIManager()->LoadImage(_AtlBaseModule.GetModuleInstance(),(IDB_BLOG) );
	//m_listActions.AddTail(new CBlogAction(strEntryName + _T("/Abrir...")));
	//m_listActions.AddTail(new CBlogAction(strEntryName + _T("/Abrir...")));
	//m_listActions.AddTail(new CBlogAction(strEntryName + _T("/Configurar...")));
	//m_listActions.AddTail(new CBlogAction(strEntryName + _T("/Atualizar")));
}


CBlogCustomListEntry::~CBlogCustomListEntry()
{
}


CString CBlogCustomListEntry::GetEntryName() const
{
	return m_strEntryName;
}


CString CBlogCustomListEntry::GetPluginInternalName() const
{
	return m_strPluginInternalName;
}


IUOLMessengerPainterPtr CBlogCustomListEntry::GetPainter()
{
	return IUOLMessengerPainterPtr();
}


IUOLMessengerImagePtr CBlogCustomListEntry::GetIcon(BOOL& bDrawTransparent)
{
	bDrawTransparent = TRUE;

	//return IUOLMessengerImagePtr();
	return m_icon;
}


CString CBlogCustomListEntry::GetDisplayName()
{
	return m_strEntryName;
}


void CBlogCustomListEntry::OnLButtonDoubleClick(const CPoint& ptClient)
{
//	CString strMessage;
//	strMessage.Format(_T("Event: %s"), __FUNCTION__);
//	::MessageBox(NULL, strMessage, NULL, MB_OK);
	Show();
}


void CBlogCustomListEntry::OnReturnKeyPressed()
{
//	CString strMessage;
//	strMessage.Format(_T("Event: %s"), __FUNCTION__);

//	::MessageBox(NULL, strMessage, NULL, MB_OK);
	Show();
}


void CBlogCustomListEntry::OnSelectionChanged(BOOL bSelected)
{
}


void CBlogCustomListEntry::GetContextMenuActions(CAtlList<IUOLMessengerPluginActionPtr>& listActions)
{
	listActions.RemoveAll();

	for (POSITION pos = m_listActions.GetHeadPosition(); pos; )
	{
		IUOLMessengerPluginActionPtr pAction = m_listActions.GetNext(pos);
		listActions.AddTail(pAction);
	}
}

//////////////////////////////////////////////////////////////////////////
// CBlogAction
//////////////////////////////////////////////////////////////////////////

CBlogAction::CBlogAction(const CString& strName) :
m_strName(strName)
{
}


CBlogAction::~CBlogAction()
{
}


CString CBlogAction::GetName() const
{
	return m_strName;
}


void CBlogAction::DoAction()
{

}


typedef void  (*MYSHOWFUNC)(HWND);    // pointer to: void  function(HWND) 

void CBlogCustomListEntry::Show()
{
	g_fotoblog_dll.ShowBlog(g_pUOLMessengerFacade->GetUIManager()->GetAppWindow());
	//show = (MYSHOWFUNC) GetProcAddress(hMod, "ShowBlogDlg");
	//show(m_hWnd);

	////if (g_pUOLMessengerFacade->GetAuthenticationManager()->IsAuthenticated()==TRUE) {
	//	IUOLMessengerAuthenticationManagerPtr pAuth;
	//	pAuth = g_pUOLMessengerFacade->GetAuthenticationManager();
	//	if (pAuth.get()!=NULL) {
	//		CSimpleBlogController * pSimple;
	//		pSimple = CSimpleBlogController::GetInstance();
	//		CString cookie = pAuth->GetCookie();
	//		pSimple->SetCookie(cookie);
	//		pSimple->OnAuthenticated(TRUE);
	//		pSimple->Init();
	//		pSimple->DisplayBlogDialog(g_pUOLMessengerFacade->GetUIManager()->GetAppWindow());
	//		//pSimple->CloseBlogDialog();
	//		//pSimple->Finalize();
	//	}
	////} else {
	////	CString strMessage;
	////	strMessage = "Usuário não autenticado!";
	////	::MessageBox(NULL, strMessage, NULL, MB_OK);
	////}
}

