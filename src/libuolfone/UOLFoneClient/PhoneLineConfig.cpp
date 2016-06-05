#include "stdafx.h"
#include "PhoneLineConfig.h"


CPhoneLineConfig::CPhoneLineConfig() :
	m_lPort(0)
{
}


CPhoneLineConfig::~CPhoneLineConfig()
{
}


STDMETHODIMP CPhoneLineConfig::GetConnectionPort(LONG* plPort)
{
	HRESULT hr = E_POINTER;

	if (plPort)
	{
		*plPort = m_lPort;
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CPhoneLineConfig::GetNetworkConfig(IUOLFoneClientNetworkConfig** ppNetworkConfig)
{
	HRESULT hr = E_POINTER;

	if (ppNetworkConfig)
	{
		*ppNetworkConfig = m_spNetworkConfig;

		if (ppNetworkConfig)
		{
			(*ppNetworkConfig)->AddRef();
		}

		hr = S_OK;
	}


	return hr;
}


STDMETHODIMP CPhoneLineConfig::GetAudioCodecsPrecedence(BSTR* pbstrAudioCodecsPrecedence)
{
	HRESULT hr = E_POINTER;

	if (pbstrAudioCodecsPrecedence)
	{
		*pbstrAudioCodecsPrecedence = m_bstrAudioCodecsPrecedence;
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CPhoneLineConfig::GetVideoCodecsPrecedence(BSTR* pbstrVideoCodecsPrecedence)
{
	HRESULT hr = E_POINTER;

	if (pbstrVideoCodecsPrecedence)
	{
		*pbstrVideoCodecsPrecedence = m_bstrVideoCodecsPrecedence;
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CPhoneLineConfig::SetConnectionPort(LONG lPort)
{
	m_lPort = lPort;

	return S_OK;
}


STDMETHODIMP CPhoneLineConfig::SetNetworkConfig(IUOLFoneClientNetworkConfig* pNetworkConfig)
{
	HRESULT hr = E_POINTER;

	if (pNetworkConfig)
	{
		m_spNetworkConfig = pNetworkConfig;
		hr = S_OK;
	}

	return hr;
}


STDMETHODIMP CPhoneLineConfig::SetAudioCodecsPrecedence(BSTR bstrAudioCodecsPrecedence)
{
	m_bstrAudioCodecsPrecedence = bstrAudioCodecsPrecedence;

	return S_OK;
}


STDMETHODIMP CPhoneLineConfig::SetVideoCodecsPrecedence(BSTR bstrVideoCodecsPrecedence)
{
	m_bstrVideoCodecsPrecedence = bstrVideoCodecsPrecedence;

	return S_OK;
}
