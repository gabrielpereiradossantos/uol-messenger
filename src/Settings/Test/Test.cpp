// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\abstractSettings.h"
#include "..\Settingsfactory.h"

using namespace UOLLib;

int _tmain(int argc, _TCHAR* argv[])
{
	ISettings* pSettings = CSettingsFactory::AllocXmlSettings("C:\\Documents and Settings\\loliveira.UOLCORP\\Desktop\\resfinal.xml");
	ISettings& settings = *pSettings;

	settings.Load();

	settings.SetVar("VAR1", "var1");
	settings.SetVar("VAR2", "var2");
	settings.SetVar("APP_VERSION", "2.2.2.2.2");
	

	CString str = settings.GetValue("uol.resource.IDS_VERSION_FORMAT");


	CSettingsFactory::ReleaseXmlSettings(pSettings);

	return 0;
}

