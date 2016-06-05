#pragma once
#include "isettings.h"

class DECLSPEC_EXPORT CAbstractSettings :
	public UOLLib::ISettings
{
public:
	CAbstractSettings(void);	

protected:
	virtual ~CAbstractSettings(void) = 0;
	
	// ISettings
	virtual BOOL Load() = 0;
	virtual BOOL Save() = 0;
	
	virtual void SetValue(const CString& strKey, const CString& strValue);
	virtual CString GetValue(const CString& strKey);

	virtual void SetVar(const CString& strVarName, const CString& strVarValue);

private:
	CString ExpandVars(const CString& strValue);


private:
	CAtlMap<CString, CString>*	m_pMapKeys;
	CAtlMap<CString, CString>*	m_pMapVars;
};
