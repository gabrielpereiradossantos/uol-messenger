#include "StdAfx.h"
#include ".\settingsfactory.h"

#include ".\xmlsettings.h"

namespace UOLLib
{
	CSettingsFactory::CSettingsFactory(void)
	{
	}

	CSettingsFactory::~CSettingsFactory(void)
	{
	}

	ISettings* CSettingsFactory::AllocXmlSettings(const CString& strXmlFile)
	{
		return new CXmlSettings(strXmlFile);
	}

	void CSettingsFactory::ReleaseXmlSettings(ISettings* pSettings)
	{
		ATLASSERT(pSettings);

		if (pSettings)
		{
			delete pSettings;
		}
	}
}