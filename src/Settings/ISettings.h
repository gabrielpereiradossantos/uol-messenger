#pragma once

#ifdef SETTINGS_EXPORTS
	#define DECLSPEC_EXPORT __declspec(dllexport)
#else
	#define DECLSPEC_EXPORT __declspec(dllimport)
#endif 

namespace UOLLib
{
	class ISettings
	{
	public:
		virtual ~ISettings(void) = 0 { }

		virtual BOOL Load() = 0;
		virtual BOOL Save() = 0;
		
		virtual void SetValue(const CString& strKey, const CString& strValue) = 0;
		virtual CString GetValue(const CString& strKey) = 0;

		virtual void SetVar(const CString& strVarName, const CString& strVarValue) = 0;
	};
}