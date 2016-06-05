/* UOL Messenger
 * Copyright (c) 2005 Universo Online S/A
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
 * Universo Online S/A - A/C: UOL Messenger 5o. Andar
 * Avenida Brigadeiro Faria Lima, 1.384 - Jardim Paulistano
 * São Paulo SP - CEP 01452-002 - BRASIL  */
 


// RijndaelWrapper.cpp: implementation of the CRijndaelWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma warning(push, 1)
#pragma warning(disable: 4311)
#pragma warning(disable: 4081)
#pragma warning(disable: 4267)


#include "RijndaelWrapper.h"

USING_NAMESPACE(CryptoPP);
USING_NAMESPACE(std);

//
//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRijndaelWrapper::CRijndaelWrapper() 
{

}

CRijndaelWrapper::~CRijndaelWrapper()
{
}

void CRijndaelWrapper::SetKey(LPCTSTR lpszKey)
{
	m_Key.CleanNew(RijndaelEncryption::DEFAULT_KEYLENGTH);
	int iLength=min(RijndaelEncryption::DEFAULT_KEYLENGTH,strlen(lpszKey));
	memcpy((char*)(unsigned char *)(m_Key),lpszKey,iLength);

}

BOOL CRijndaelWrapper::Encrypt(LPCTSTR lpszPlainText,LPTSTR *plpszCipherText, int *piLength)
{
	
	int iBlockSize = m_Encryption.BlockSize();
	int iTotalSize = *piLength;
	int iDelta;
	int iProcessed = 0;


	m_OuterCipher.CleanNew((iTotalSize/iBlockSize + 1)*iBlockSize + sizeof(long) );
	long* pLength = (long*)(unsigned char*)m_OuterCipher;
	*pLength = iTotalSize;

	RijndaelEncryption enc(m_Key);

	while(iProcessed < iTotalSize) {
		iDelta = min(iBlockSize,iTotalSize-iProcessed);
		m_Plain.CleanNew(iBlockSize);
		m_Cipher.CleanNew(iBlockSize);
		memcpy((char*)(unsigned char*)m_Plain,lpszPlainText+iProcessed,iDelta);
		
		enc.ProcessBlock(m_Plain, m_Cipher);

		
		memcpy( ((unsigned char*)m_OuterCipher) + sizeof (long) + iProcessed,(unsigned char*)m_Cipher, iBlockSize);
		
		iProcessed += iBlockSize;
	}

	*plpszCipherText = (char*)(unsigned char*)m_OuterCipher;
	*piLength = iProcessed + sizeof (long);

	return TRUE;
}

BOOL CRijndaelWrapper::Decrypt(LPCTSTR lpszCipherText, LPTSTR *plpszPlainText, int* piLength)
{
	int iBlockSize = m_Encryption.BlockSize();
	int iTotalSize = *piLength;
	iTotalSize -= sizeof(long);
	int iDelta;
	int iProcessed = 0;

	m_OuterPlain.CleanNew(iTotalSize);  

	RijndaelDecryption dec(m_Key);

	while(iProcessed < iTotalSize) {
		iDelta = min(iBlockSize,iTotalSize-iProcessed);
		m_Cipher.CleanNew(iBlockSize);
		m_Plain.CleanNew(iBlockSize);
		memcpy((char*)(unsigned char*)m_Cipher,lpszCipherText + iProcessed + sizeof (long),iDelta);
		
		
		dec.ProcessBlock(m_Cipher, m_Plain);
		
		memcpy( ((unsigned char*)m_OuterPlain) + iProcessed,(unsigned char*)m_Plain, iBlockSize);
		
		iProcessed += iDelta;
	}

	*plpszPlainText = (char*)(unsigned char*)m_OuterPlain;

	*piLength = *((long*)lpszCipherText);

	return TRUE;
}


#include <randpool.h>
#include <osrng.h>


void CRijndaelWrapper::TestRijndael()
{
	char lpszKey[17];
	char lpszData[477];


	LPTSTR lpszCipher;
	LPTSTR lpszPlain;

	CRijndaelWrapper rw;

	int iEncSize=0;
	int iSize=0;
	NonblockingRng rng;
	RandomPool randPool;

	for(int i = 0 ; i < 1000000 ; i++) {
		rng.GenerateBlock((unsigned char*)lpszKey,sizeof(lpszKey));

		randPool.Put((byte *)lpszKey, sizeof(lpszKey));

		rng.GenerateBlock((unsigned char*)lpszData,sizeof(lpszData));

		randPool.Put((byte *)lpszData, sizeof(lpszData));


		for(int iKeyIndex = 0; iKeyIndex < sizeof(lpszKey) ; iKeyIndex++) {
			if('\0' == lpszKey[iKeyIndex]) {
				lpszKey[iKeyIndex]++;
			}
		}
		lpszKey[16]='\0';

		rw.SetKey(lpszKey);

		iEncSize=sizeof(lpszData);

		rw.Encrypt(lpszData,&lpszCipher,&iEncSize);

		iSize = iEncSize;
		rw.Decrypt(lpszCipher,&lpszPlain,&iSize);

		if(memcmp(lpszData,lpszPlain,min(iSize,sizeof(lpszData))) ) {
			DebugBreak();
		}

		lpszKey[0]++;
		rw.SetKey(lpszKey);

		iSize = iEncSize;
		rw.Decrypt(lpszCipher,&lpszPlain,&iSize);

		if(!memcmp(lpszData,lpszPlain,min(iSize,sizeof(lpszData))) ) {
			DebugBreak();
		}
	}
		::MessageBox(NULL,"tested","OK",MB_OK);
}

#pragma warning(pop)