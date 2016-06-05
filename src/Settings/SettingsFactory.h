#pragma once

#include "ISettings.h"

namespace UOLLib
{

class DECLSPEC_EXPORT CSettingsFactory
{
public:
	CSettingsFactory(void);
	~CSettingsFactory(void);

	static ISettings* AllocXmlSettings(const CString& strXmlFile);
	static void ReleaseXmlSettings(ISettings* pSettings);
};

}
