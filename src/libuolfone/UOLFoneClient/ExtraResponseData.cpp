/* UOL Fone
 * Copyright (c) 2006 Universo Online S/A
 *
 * Direitos Autorais Reservados
 * All rights reserved
 *
 * Este programa é software livre; você pode redistribuí-lo e/ou modificá-lo
 * sob os termos da Licença Pública Geral GNU conforme publicada pela Free
 * Software Foundation; tanto a versão 2 da Licença, como (a seu critério)
 * qualquer versão posterior.
 * Este programa é distribuído na expectativa de que seja útil, porém,
 * SEM NENHUMA GARANTIA; nem mesmo a garantia implícita de COMERCIABILIDADE
 * OU ADEQUAÇÃO A UMA FINALIDADE ESPECÍFICA. Consulte a Licença Pública Geral
 * do GNU para mais detalhes. 
 * Você deve ter recebido uma cópia da Licença Pública Geral do GNU junto
 * com este programa; se não, escreva para a Free Software Foundation, Inc.,
 * no endereço 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Universo Online S/A - A/C: UOL Fone 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  *//* UOL Fone
 * Copyright (c) 2006 Universo Online S/A
 *
 * Direitos Autorais Reservados
 * All rights reserved
 *
 * Este programa é software livre; você pode redistribuí-lo e/ou modificá-lo
 * sob os termos da Licença Pública Geral GNU conforme publicada pela Free
 * Software Foundation; tanto a versão 2 da Licença, como (a seu critério)
 * qualquer versão posterior.
 * Este programa é distribuído na expectativa de que seja útil, porém,
 * SEM NENHUMA GARANTIA; nem mesmo a garantia implícita de COMERCIABILIDADE
 * OU ADEQUAÇÃO A UMA FINALIDADE ESPECÍFICA. Consulte a Licença Pública Geral
 * do GNU para mais detalhes. 
 * Você deve ter recebido uma cópia da Licença Pública Geral do GNU junto
 * com este programa; se não, escreva para a Free Software Foundation, Inc.,
 * no endereço 59 Temple Street, Suite 330, Boston, MA 02111-1307 USA. 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * Universo Online S/A - A/C: UOL Fone 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */


#include "stdafx.h"
#include "ExtraResponseData.h"

#include "HeaderInfo.h"
#include "HeaderInfoEnum.h"


#define			RESPONSE_DATA_NAME_STATUS_CODE			"status_code"
#define			RESPONSE_DATA_NAME_TEXT_INFO			"text_info"
#define			RESPONSE_DATA_NAME_REASON_PHRASE		"reason_phrase"
#define			RESPONSE_DATA_NAME_ERROR_SOURCE			"error_source"


CExtraResponseData::CExtraResponseData()
{
}


CExtraResponseData::~CExtraResponseData()
{
}


STDMETHODIMP CExtraResponseData::EnumerateExtraHeaders(IUOLFoneClientHeaderInfoEnum** ppHeaderInfoEnum)
{
	HRESULT hr = E_FAIL;

	CComObject<CHeaderInfoEnum>* pNewHeaderInfoEnum;

	hr = CComObject<CHeaderInfoEnum>::CreateInstance(&pNewHeaderInfoEnum);

	if (SUCCEEDED(hr))
	{
		for (POSITION pos = m_listHeaderData.GetHeadPosition(); pos; )
		{
			CHeaderDataPtr pHeaderData = m_listHeaderData.GetNext(pos);

			CComObject<CHeaderInfo>* pHeaderInfo;

			hr = CComObject<CHeaderInfo>::CreateInstance(&pHeaderInfo);

			if (SUCCEEDED(hr))
			{
				pHeaderInfo->SetHeaderData(pHeaderData);
				pNewHeaderInfoEnum->AddHeaderInfo(pHeaderInfo);
			}
			else
			{
				break;
			}
		}

		if (SUCCEEDED(hr))
		{
			pNewHeaderInfoEnum->AddRef();
			*ppHeaderInfoEnum = pNewHeaderInfoEnum;
		}
	}

	return hr;
}


STDMETHODIMP CExtraResponseData::GetExtraResponseDataId(
		BSTR bstrExtraResponseDataName, LONG* plExtraResponseDataId)
{
	HRESULT hr = E_FAIL;
	
	if (plExtraResponseDataId)
	{
		CString strExtraResponseDataName(bstrExtraResponseDataName);
		
		int iExtraResponseDataId = GetExtraResponseDataId(
				strExtraResponseDataName);
		
		if (iExtraResponseDataId != CALL_ERROR_INVALID_RESPONSE_ID)
		{
			*plExtraResponseDataId = iExtraResponseDataId;
			
			hr = S_OK;
		}
	}
	
	return hr;
}


void CExtraResponseData::SetExtraHeaders(const IPhoneLineProgressInfo* pPhoneLineProgressInfo)
{
	if (pPhoneLineProgressInfo)
	{
		pPhoneLineProgressInfo->GetExtraHeaders(m_listHeaderData);
	}
}


void CExtraResponseData::SetExtraHeaders(const IPhoneCallProgressInfo* pPhoneCallProgressInfo)
{
	if (pPhoneCallProgressInfo)
	{
		pPhoneCallProgressInfo->GetExtraInfo(m_listHeaderData);
	}
}


int CExtraResponseData::GetExtraResponseDataId(CString strExtraResponseDataName)
{
	int iExtraResponseDataId = CALL_ERROR_INVALID_RESPONSE_ID;
	
	if (!strExtraResponseDataName.CompareNoCase(RESPONSE_DATA_NAME_REASON_PHRASE))
	{
		iExtraResponseDataId = CALL_ERROR_REASON_PHRASE;
	}
	else if (!strExtraResponseDataName.CompareNoCase(RESPONSE_DATA_NAME_STATUS_CODE))
	{
		iExtraResponseDataId = CALL_ERROR_STATUS_CODE;
	}
	else if (!strExtraResponseDataName.CompareNoCase(RESPONSE_DATA_NAME_TEXT_INFO))
	{
		iExtraResponseDataId = CALL_ERROR_TEXT_INFO;
	}
	else if (!strExtraResponseDataName.CompareNoCase(RESPONSE_DATA_NAME_ERROR_SOURCE))
	{
		iExtraResponseDataId = CALL_ERROR_ERROR_SOURCE;
	}
	
	return iExtraResponseDataId;
}


CString CExtraResponseData::GetExtraResponseDataName(int iExtraResponseDataId)
{
	CString strExtraResponseDataName;
	
	switch (iExtraResponseDataId)
	{
		case CALL_ERROR_REASON_PHRASE:
			
			strExtraResponseDataName = RESPONSE_DATA_NAME_REASON_PHRASE;
			
		break;
		
		case CALL_ERROR_STATUS_CODE:
			
			strExtraResponseDataName = RESPONSE_DATA_NAME_STATUS_CODE;
			
		break;
		
		case CALL_ERROR_TEXT_INFO:
			
			strExtraResponseDataName = RESPONSE_DATA_NAME_TEXT_INFO;
			
		break;
		
		case CALL_ERROR_ERROR_SOURCE:
			
			strExtraResponseDataName = RESPONSE_DATA_NAME_ERROR_SOURCE;
			
		break;
	}
	
	return strExtraResponseDataName;
}

