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
 * São Paulo SP - CEP 01452-002 - BRASIL  */


#include "stdafx.h"
#include "PhCodecInfoAdapter.h"

#include "CodecInfo.h"
#include "CodecInfoEnum.h"


HRESULT CPhCodecInfoAdapter::GetCodecInfoEnum(const CString& strPhCodecs, IUOLFoneClientCodecInfoEnum** ppCodecInfoEnum)
{
	if (!ppCodecInfoEnum)
	{
		return E_POINTER;
	}

	HRESULT hr = E_FAIL;

	CComObject<CCodecInfoEnum>* pNewCodecInfoEnum;

	hr = CComObject<CCodecInfoEnum>::CreateInstance(&pNewCodecInfoEnum);

	if (SUCCEEDED(hr))
	{
		CString strPhCodecId;
		CString strTemp = strPhCodecs;
		int nFind;

		do
		{
			nFind = strTemp.Find(',');

			if (nFind != -1)
			{
				strPhCodecId = strTemp.Left(nFind);
				strTemp = strTemp.Mid(nFind + 1);
			}
			else
			{
				strPhCodecId = strTemp;
				strTemp.Empty();
			}

			if (FALSE == strPhCodecId.IsEmpty())
			{
				CComObject<CCodecInfo>* pCodecInfo;

				hr = CComObject<CCodecInfo>::CreateInstance(&pCodecInfo);

				if (SUCCEEDED(hr))
				{
					pCodecInfo->SetId(strPhCodecId);
					pCodecInfo->SetName(GetCodecName(strPhCodecId));

					pNewCodecInfoEnum->AddCodecInfo(pCodecInfo);
				}
				else
				{
					break;
				}
			}
		}
		while (FALSE == strTemp.IsEmpty());

		if (SUCCEEDED(hr))
		{
			pNewCodecInfoEnum->AddRef();
			*ppCodecInfoEnum = pNewCodecInfoEnum;
		}
	}

	return hr;
}



CString CPhCodecInfoAdapter::GetCodecName(const CString& strPhCodecId)
{
	const int MAX_CODEC_ID = 255;
	const int MAX_CODEC_NAME = 255;

	struct CodecInfo
	{
		char pszId[MAX_CODEC_ID + 1];
		char pszName[MAX_CODEC_NAME + 1];
	};

	CodecInfo vCodecs[] = 
	{
		{ "iLBC/8000", "iLBC" },
		{ "G723/8000", "G723" },
		{ "G729/8000", "G729" },
		{ "PCMA/8000", "PCMA" },
		{ "PCMU/8000", "PCMU" },
		{ "GSM/8000", "GSM" },
		{ "SPEEX/8000", "SPEEX" },
		{ "SPEEX/8000", "SPEEX-NB" },
		{ "SPEEX/16000", "SPEEX-WB" }
	};

	CString strName;

	int nCodecsCount = sizeof(vCodecs) / sizeof(vCodecs[0]);

	for (int nInd = 0; nInd < nCodecsCount; nInd++)
	{
		if (0 == strPhCodecId.CompareNoCase(vCodecs[nInd].pszId))
		{
			strName = vCodecs[nInd].pszName;
			break;
		}
	}

	return strName;
}