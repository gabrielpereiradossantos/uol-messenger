#include "StdAfx.h"
#include ".\plugin.h"

#include "resource.h"
#include "ConfigDlg.h"

#include ".\blogcustomlistentry.h"
#include ".\fotoBlogcustomlistentry.h"
#include "FotoBlogDllWrapper.h"
#include "util/MinDependencySpecialFolder.h"


#define UMSDK_FILE_MAJOR		1
#define UMSDK_FILE_MINOR		3
#define UMSDK_BUILD_MAJOR		2
#define UMSDK_BUILD_MINOR		0


//////////////////////////////////////////////////////////////////////////

// {FD27BD74-30EE-4dd4-94A8-F8869B356B86}
static const GUID LIBID_PluingModuleLib = 
{ 0xfd27bd74, 0x30ee, 0x4dd4, { 0x94, 0xa8, 0xf8, 0x86, 0x9b, 0x35, 0x6b, 0x86 } };


class CPluingModule : public CAtlDllModuleT<CPluingModule>
{
public :
	DECLARE_LIBID(LIBID_PluingModuleLib)
};


CPluingModule _AtlModule;

IUOLMessengerFacade * g_pUOLMessengerFacade;

//////////////////////////////////////////////////////////////////////////

extern "C"
{
	__declspec(dllexport)
		CUOLMessengerPlugin* AllocUOLMessengerPlugin(IUOLMessengerFacade* pUOLMessengerFacade)
	{
		ATLTRACE("\n%s",__FUNCTION__);

		g_pUOLMessengerFacade = pUOLMessengerFacade;

		return new CPlugin(pUOLMessengerFacade);
	}

	__declspec(dllexport)
		void FreeUOLMessengerPlugin(CUOLMessengerPlugin* pUOLMessengerPlugin)
	{
		ATLTRACE("\n%s",__FUNCTION__);

		ATLASSERT(FALSE == IsBadReadPtr(pUOLMessengerPlugin, sizeof(CUOLMessengerPlugin*)));

		delete pUOLMessengerPlugin;
	}
}

//////////////////////////////////////////////////////////////////////////


CPlugin::CPlugin(IUOLMessengerFacade* pUOLMessengerFacade) : 
CUOLMessengerPlugin(pUOLMessengerFacade)
{
	ATLTRACE("%s\n",__FUNCTION__);

	m_pUOLMessengerFacade = pUOLMessengerFacade;

	g_pUOLMessengerFacade = m_pUOLMessengerFacade;

	m_pIcon = m_pUOLMessengerFacade->GetUIManager()->LoadImage(_AtlBaseModule.GetModuleInstance(), IDB_BLOGFOTOBLOG);
}

CPlugin::~CPlugin(void)
{
	ATLTRACE("%s\n",__FUNCTION__);

	m_pIcon.reset();
}


BOOL CPlugin::Init(ULONGLONG llUOLMessengerVersion)
{
	ULONG* pVersion = reinterpret_cast<ULONG*>(&llUOLMessengerVersion);

	ULONG lFileVersion = MAKELONG(UMSDK_FILE_MINOR, UMSDK_FILE_MAJOR);
	ULONG lBuildVersion = MAKELONG(UMSDK_BUILD_MINOR, UMSDK_BUILD_MAJOR);

	if ((pVersion[1] < lFileVersion) ||
		((pVersion[1] == lFileVersion) && (pVersion[0] < lBuildVersion)))
	{
		return FALSE;
	}


	CAtlList<IUOLMessengerAccountPtr> listAccounts;

	CMinDependencySpecialFolder mdsf;

	CString strUolPlugin = mdsf.GetCommonAppData();

	//CString strPath;

	CString strDllfolder; 

	CString strDllFileName;

	CString strDllFilePath;

	strDllfolder = m_pUOLMessengerFacade->GetSettingsManager()->GetGeneralSettings()->GetPluginLibFolder();
		
	CString strFile;
	CString strDllVersion;
#ifdef _DEBUG
	strFile.LoadString(IDS_PHOTOBLOGD_DLL_FILENAME);
#else
	strFile.LoadString(IDS_PHOTOBLOG_DLL_FILENAME);
#endif
	strDllVersion.LoadString(IDS_PHOTOBLOG_DLL_VERSION);

	strDllFileName.Format(_T("%s-%s.dll"),strFile,strDllVersion);
	
	ATLASSERT(strDllFileName.GetLength()>0);

	strDllFilePath = strDllfolder + strDllFileName;

	if (ATLPath::FileExists(strDllFilePath )==FALSE) {

		if (DownloadPhotoBlog(strDllfolder, strDllFileName)==FALSE) 
		{
			CString strMessage;

			strMessage.LoadString(IDS_DOWNLOAD_ERROR);

			::MessageBox(NULL, strMessage, NULL, MB_OK);

			return FALSE;
		}
	}

	if (g_fotoblog_dll.LoadDll(strDllFilePath)==FALSE)

		return FALSE;

	m_list.AddTail(new CBlogCustomListEntry(_T("Blog"), _T("UOL Blog")));

	m_list.AddTail(new CPhotoBlogCustomListEntry(_T("Fotoblog"), _T("UOL Fotoblog")));

	IUOLMessengerPluginCustomListPtr pPluginCustomList= m_pUOLMessengerFacade->GetUIManager()->GetPluginCustomList();

	if (pPluginCustomList)
	{
		BOOL bAdded = FALSE;

		for (POSITION pos = m_list.GetHeadPosition(); pos; )
		{
			IUOLMessengerPluginCustomListEntryPtr pEntry = m_list.GetNext(pos);

			bAdded = pPluginCustomList->AddEntry(pEntry);

			ATLASSERT(bAdded);
		}

	}

	return TRUE;
}

void CPlugin::Finalize()
{
	IUOLMessengerPluginCustomListPtr pPluginCustomList = m_pUOLMessengerFacade->GetUIManager()->GetPluginCustomList();

	if (pPluginCustomList)
	{
		BOOL bRemoved = FALSE;
		for (POSITION pos = m_list.GetHeadPosition(); pos; )
		{
			IUOLMessengerPluginCustomListEntryPtr pEntry = m_list.GetNext(pos);

			pPluginCustomList->RemoveEntry(pEntry->GetEntryName(), pEntry->GetPluginInternalName());
		}
	}

	m_list.RemoveAll();

	g_fotoblog_dll.ReleaseDll();
}

CString CPlugin::GetName() const
{
	return "UOL Blog / UOL Fotoblog";
}

ULONGLONG CPlugin::GetVersion() const
{
	return CAppVersion(ATL::_AtlBaseModule.GetModuleInstance()).GetVersion();
}

CString CPlugin::GetInternalName() const
{
	return "FotoBlogPlugin";
}

CString CPlugin::GetDesc() const
{
	return "Plugin para posts.";
}

IUOLMessengerImagePtr CPlugin::GetIcon() const
{
	return m_pIcon;
}

BOOL CPlugin::HasConfigDialog()
{
	return FALSE;
}

void CPlugin::ShowConfigDialog()
{
}

void CPlugin::HelloWorldDlg()
{
}

void CPlugin::Trace()
{
	BOOL isAuth = m_pUOLMessengerFacade->GetAuthenticationManager()->IsAuthenticated();

	CString cookie = m_pUOLMessengerFacade->GetAuthenticationManager()->GetCookie();

	ATLTRACE("\n%s: Auth: %d cookie %s",__FUNCTION__, isAuth, cookie);
}


BOOL CPlugin::DownloadPhotoBlog(const CString & strFolder, 	const CString & strFileName)
{


	CString strUrl;
	
//#if _DEBUG
//	strUrl.LoadString(IDS_PHOTOBLOG_DLL_URL_DEVEL);
//#else
//	strUrl.LoadString(IDS_PHOTOBLOG_DLL_URL_DEVEL);
//	//strUrl.LoadString(IDS_PHOTOBLOG_DLL_URL_RELEASE);
//#endif 
	strUrl.LoadString(IDS_PHOTOBLOG_DLL_URL_RELEASE);

	//strUrl.LoadString(IDS_PHOTOBLOG_DLL_URL);
	//strUrl = strUrl + strFileName;

	CString strDirNameWithBackSlash = strFolder;

	if (strDirNameWithBackSlash.Right(1) != '\\')
	{
		strDirNameWithBackSlash += '\\';
	}

	CAtlHttpClient httpClient;

	if (httpClient.Navigate(strUrl))
	{	
		const BYTE* pByte = httpClient.GetBody();
		DWORD dwLen = httpClient.GetBodyLength();

		if (dwLen)
		{	
			CAtlFile file;			

			if (! strFileName.IsEmpty())
			{

				if (FALSE == ATLPath::FileExists(strFolder))
				{
					::CreateDirectory(strFolder, NULL);
				}

				CString strPluginPath = strDirNameWithBackSlash + strFileName;

				ATLTRACE("%s %s %d",__FUNCTION__,strUrl,strPluginPath);

				if (SUCCEEDED(file.Create(strPluginPath, GENERIC_WRITE, 0, CREATE_ALWAYS)))
				{
					file.Write(pByte, dwLen);

					file.Close();

					return TRUE;

				}
			}
		}				
	}

	return FALSE;
}