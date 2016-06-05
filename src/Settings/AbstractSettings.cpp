#include "StdAfx.h"
#include "abstractsettings.h"

CAbstractSettings::CAbstractSettings(void)
{
	m_pMapKeys = new CAtlMap<CString, CString>();
	m_pMapVars = new CAtlMap<CString, CString>();
}

CAbstractSettings::~CAbstractSettings(void)
{
	if (m_pMapKeys)
	{
		delete m_pMapKeys;
	}

	if (m_pMapVars)
	{
		delete m_pMapVars;
	}
}

void CAbstractSettings::SetValue(const CString& strKey, const CString& strValue)
{
	ATLASSERT(m_pMapKeys);

	(*m_pMapKeys)[strKey] = strValue;	
}

CString CAbstractSettings::GetValue(const CString& strKey)
{
	ATLASSERT(m_pMapKeys);

	if (m_pMapKeys)
	{
		return ExpandVars((*m_pMapKeys)[strKey]);
	}

	return "";
}

void CAbstractSettings::SetVar(const CString& strVarName, const CString& strVarValue)
{
	ATLASSERT(m_pMapVars);

	(*m_pMapVars)[strVarName] = strVarValue;	
}

CString CAbstractSettings::ExpandVars(const CString& strValue)
{
	CString str;

	int nMark = 0;
	int nIndexStart = strValue.Find("$(");
	int nIndexEnd;

	while (-1 != nIndexStart)
	{
		str += strValue.Mid(nMark, nIndexStart - nMark);

		nIndexEnd = strValue.Find(')', nIndexStart);

		if (-1 != nIndexEnd)
		{
			CString strVarName = strValue.Mid(nIndexStart + 2, nIndexEnd - (nIndexStart + 2));
			CString strVarValue = (*m_pMapVars)[strVarName];

			str += strVarValue;

			nMark = nIndexEnd + 1;

			nIndexStart = strValue.Find("$(", ++nIndexStart);
		}
	}

	str += strValue.Mid(nMark);

	return str;
}