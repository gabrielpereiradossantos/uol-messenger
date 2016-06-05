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


#include "StdAfx.h"
#include ".\cryptosession.h"

using namespace CryptoPP;
using namespace std;

#define SYMCRYPTOALGO	Blowfish

const string CCryptoSession::UOLCRYPT_TAG("UOLCRYPT");
const string CCryptoSession::UOLCRYPT_VERSION("0.9");

CCryptoSession::CCryptoSession(CPubKeyMap& mapPubKeys, 
							   RandomNumberGenerator& rng, 
							   const char* szNonUIMMessage) :
	m_rng(rng),
	m_mapPubKeys(mapPubKeys),
	m_keyPriv(rng, 128),
	m_keySymmetric(),
	m_keyPub(m_keyPriv), 
	m_vBlock(),
	m_stateCurrent(enInit),
	m_errorReason(reasonNoError),
	m_szNonUIMMessage(szNonUIMMessage),
	m_strStartedUser()
{
}

CCryptoSession::~CCryptoSession(void)
{
}

string CCryptoSession::MakeInvite(const std::string& strFromUser)
{
	ostrstream buf;

	try
	{	
		string strPubKey;
		m_keyPub.GetKey().BEREncode(HexEncoder(new StringSink(strPubKey)));

		GetPrologue(buf, true, m_szNonUIMMessage);

		buf << "INVITE pub:" + strPubKey << endl;

		//ATLTRACE(_T("%s - pub: %s\n"), __FUNCTION__, strPubKey.c_str());

		GetEpilogue(buf);

		m_strStartedUser = strFromUser;
	}
	catch(Exception& e) 	
	{
		ostrstream bufError;
		string strError;

		bufError << reasonCryptoLib << " DESC: " << e.what();

		m_errorReason = reasonCryptoLib;

		return MakeError(bufError.str());
	}
	
	return buf.str();	
}

string CCryptoSession::MakeAccept()
{
	ostrstream buf;
	
	try
	{
		string strPubKey;
		m_keyPub.GetKey().BEREncode(HexEncoder(new StringSink(strPubKey)));

		GetPrologue(buf);

		buf << "ACCEPT pub:" + strPubKey << endl;

		GetEpilogue(buf);

		//ATLTRACE(_T("%s - pub: %s\n"), __FUNCTION__, strPubKey.c_str());
	}
	catch(Exception& e) 
	{
		ostrstream bufError;
		string strError;

		bufError << reasonCryptoLib << " DESC: " << e.what();

		m_errorReason = reasonCryptoLib;

		return MakeError(bufError.str());
	}

	return buf.str();	
}

string CCryptoSession::MakeSetKey(const std::string& strFromUser, const string& strToUser)
{
	ostrstream buf;


	try
	{
		string strSessionId = BuildSessionId(strFromUser, strToUser);

		CPubKeyMap::iterator it = m_mapPubKeys.find(strSessionId);

		if (it != m_mapPubKeys.end())
		{
			string strUserPubKey = it->second;

			ElGamal::Encryptor encryptor(StringSource(strUserPubKey, true, new HexDecoder()));

			string strSymKey = MakeSymmetricKey(encryptor);
			string strVBlock = MakeVBlock(encryptor);

			GetPrologue(buf);

			buf << "SETKEY key:" + strSymKey << endl 
				<< "vBlock:" << strVBlock << endl;

			GetEpilogue(buf);
		}
	}
	catch(Exception& e) 
	{
		ostrstream bufError;
		string strError;

		bufError << reasonCryptoLib << " DESC: " << e.what();

		m_errorReason = reasonCryptoLib;

		return MakeError(bufError.str());
	}
	
	//ATLTRACE(_T("%s: %s\n"), __FUNCTION__, buf.str());

	return buf.str();	
}

string CCryptoSession::MakeMsg(const std::string& strMsg)
{	
	ostrstream buf;
	try
	{
		Blowfish::Encryption encryption(reinterpret_cast<byte*>(&m_keySymmetric), sizeof(KeyType));
		CBC_Mode_ExternalCipher::Encryption cbcEncryption(encryption, reinterpret_cast<byte*>(&m_vBlock));

		string strResult;

		//char pszKey[256] = {0};
		//::_ui64tot(m_keySymmetric, pszKey, 10);

		//ATLTRACE(_T("%s: key: %s\n"), __FUNCTION__, pszKey);
		//ATLTRACE(_T("%s: vblock: %u\n"), __FUNCTION__, m_vBlock);
		//ATLTRACE(_T("%s: message: %s, %d\n"), __FUNCTION__, strMsg.c_str(), strMsg.size());

		StreamTransformationFilter stfEncryptor(cbcEncryption, new Base64Encoder(new StringSink(strResult), 0));
		stfEncryptor.Put(reinterpret_cast<const unsigned char*>(strMsg.c_str()), static_cast<unsigned>(strMsg.size() + 1));
		stfEncryptor.MessageEnd();

		/*string strResult;

		StringSource(strMsg, 
						true, 
						new Base64Encoder(new StringSink(strResult), 1, 32));*/

		GetPrologue(buf);	

		buf << "MSG " << strResult;

		GetEpilogue(buf);
	}
	catch(Exception& e) 
	{
		ostrstream bufError;
		string strError;

		bufError << reasonCryptoLib << " DESC: " << e.what();

		//ATLTRACE(_T("%s: error: %s\n"), __FUNCTION__, MakeError(bufError.str()).c_str());

		m_errorReason = reasonCryptoLib;

		return MakeError(bufError.str());
	}
	
	//ATLTRACE(_T("%s: %s\n"), __FUNCTION__, buf.str());

	return buf.str();	
}

string CCryptoSession::MakeEnd()
{
	ostrstream buf;

	GetPrologue(buf);

	buf << "END";

	GetEpilogue(buf);
	
	return buf.str();
}


std::string CCryptoSession::MakeError(const std::string& strReason)
{
	strstream buf;

	GetPrologue(buf);

	buf << "ERROR " << strReason;

	GetEpilogue(buf);
	
	return buf.str();
}


std::string CCryptoSession::MakeError(ErrorReason reason)
{
	string strArg;

	if (reason == reasonSessionNotFound)
	{
		strArg = "404 Session not found";
	}
	
	return MakeError(strArg.c_str());
}

string CCryptoSession::ProcessCryptoMsg(const std::string& strFromUser, const std::string& strToUser, const std::string& strMsg)
{
	ATLTRACE("%s -> strFromUser = [%s], strToUser = [%s], strMsg = [%s]\n", 
			__FUNCTION__, strFromUser.c_str(), strToUser.c_str(), strMsg.c_str());
	
	istrstream is(strMsg.c_str());

    string strTag;
	string strVersion;
	string strVerb;
	
	is >> strTag >> strVersion >> strVerb;

	ATLTRACE("%s -> is = [%s]\n", __FUNCTION__, is.str());
	
	ATLTRACE("%s -> strTag = [%s], strVersion = [%s], strVerb = [%s]\n", 
			__FUNCTION__, strTag.c_str(), strVersion.c_str(), strVerb.c_str());
	
	if (strVerb == "INVITE")
	{
		return ProcessInvite(strFromUser, strToUser, is.str());	
	}	
	
	if (strVerb == "ACCEPT")
	{
		return ProcessAccept(strFromUser, strToUser, is.str());
	}
	
	if (strVerb == "SETKEY")
	{
		return ProcessSetKey(strFromUser, strToUser, is.str());
	}
	
	if (strVerb == "MSG")
	{
		return ProcessMsg(strFromUser, strToUser, is.str());
	}

	if (strVerb == "END")
	{
		return ProcessEnd(strFromUser, strToUser, is.str());
	}

	if (strVerb == "ERROR")
	{
		return ProcessError(strFromUser, strToUser, is.str());
	}

	m_stateCurrent = enUnknow;

	return string();
}

CCryptoSession::State CCryptoSession::GetCurrentState() const
{
	return m_stateCurrent;
}

bool CCryptoSession::IsCryptoMsg(std::string& strMsg)
{
	ostrstream buf;
	GetPrologue(buf, false);	
	buf << '\0';

	string strPrologue = buf.str();

	string::size_type nIndStart = strMsg.find(strPrologue);
	
	if (-1 != nIndStart)
	{	
		if (strPrologue == strMsg.substr(nIndStart, strPrologue.size()))
		{
			strMsg = StripTags(strMsg.substr(nIndStart));			

			return true;
		}	
	}

	return false;
}

bool CCryptoSession::IsInviteMsg(std::string& strMsg)
{
	ATLTRACE("%s -> strMsg = [%s]\n", __FUNCTION__, strMsg.c_str());
	
	istrstream is(strMsg.c_str());
	
    string strTag;
	string strVersion;
	string strVerb;
	
	is >> strTag >> strVersion >> strVerb;
	
	ATLTRACE("%s -> is = [%s]\n", __FUNCTION__, is.str());
	
	ATLTRACE("%s -> strTag = [%s], strVersion = [%s], strVerb = [%s]\n", 
			__FUNCTION__, strTag.c_str(), strVersion.c_str(), strVerb.c_str());
	
	if (strVerb == "INVITE")
	{
		return true;
	}
	
	return false;
}

std::string CCryptoSession::BuildSessionId(const std::string& strFromUser, const std::string& strToUser)
{
	string strId = strToUser + "\\" + strFromUser;

	return strId;
}


string CCryptoSession::ProcessInvite(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams)
{
	string strSessionId = CCryptoSession::BuildSessionId(strFromUser, strToUser);

	if (ProccessPubKey(strSessionId, strParams))
	{
		m_stateCurrent = enInvite;
	}

	return string();
}

string CCryptoSession::ProcessAccept(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams)
{
	string strSessionId = CCryptoSession::BuildSessionId(strFromUser, strToUser);

	if (ProccessPubKey(strSessionId, strParams))
	{
		m_stateCurrent = enAccept;
	}

	return string();
}

string CCryptoSession::ProcessSetKey(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams)
{
	if (0 == strFromUser.compare(m_strStartedUser))
	{
		return string();
	}

	try
	{
		istrstream is(strParams.c_str());

		string::size_type nInd;

		string strKey;
		string strKeyValue;

		string strVBlock;
		string strVBlockValue;
	    
		is >> strKey >> strVBlock;

		nInd = strKey.find("key:");

		if (-1 != nInd)
		{
			strKeyValue = strKey.substr(4);		
		}	

		nInd = strVBlock.find("vBlock:");

		if (-1 != nInd)
		{
			strVBlockValue = strVBlock.substr(7);		
		}	

		if (strKeyValue.size() && strVBlockValue.size())
		{
			StringSource(strKeyValue, 
						true, 
						new HexDecoder(new PK_DecryptorFilter(m_rng, 
															m_keyPriv, 
															new ArraySink(reinterpret_cast<byte*>(&m_keySymmetric), sizeof(KeyType)))));

			StringSource(strVBlockValue, 
						true, 
						new HexDecoder(new PK_DecryptorFilter(m_rng, 
															m_keyPriv, 
															new ArraySink(reinterpret_cast<byte*>(&m_vBlock), sizeof(ULONGLONG)))));

			m_stateCurrent = enSetKey;
		}	
	}
	catch(Exception&) 
	{
		// Invalid key
		m_stateCurrent = enError;

		m_errorReason = reasonCryptoLib;
	}

	return string();
}

string CCryptoSession::ProcessMsg(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams)
{
	string strPlainText;

	try
	{
		istrstream is(strParams.c_str());

		string strLine;    
		string strMsg;    

		do
		{
			is >> strLine;

			if (strLine.empty())
			{
				break;
			}
			
			strMsg += strLine;

			strLine.clear();
		
		} while (TRUE);

		if (strMsg.size())
		{
			Blowfish::Decryption decryption(reinterpret_cast<byte*>(&m_keySymmetric), sizeof(KeyType));
			CBC_Mode_ExternalCipher::Decryption cbcDecryption(decryption, reinterpret_cast<byte*>(&m_vBlock));

			string strRawMsg;			
			StringSource(strMsg, true, new Base64Decoder(new StringSink(strRawMsg)));

			//char pszKey[256] = {0};
			//::_ui64tot(m_keySymmetric, pszKey, 10);

			//ATLTRACE(_T("%s: key: %s\n"), __FUNCTION__, pszKey);
			//ATLTRACE(_T("%s: vblock: %u\n"), __FUNCTION__, m_vBlock);
			//ATLTRACE(_T("%s: message: %s, %d\n"), __FUNCTION__, strMsg.c_str(), strMsg.size());

			StreamTransformationFilter stfDecryptor(cbcDecryption, new StringSink(strPlainText));
			stfDecryptor.Put(reinterpret_cast<const unsigned char*>(strRawMsg.c_str()), static_cast<unsigned>(strRawMsg.size()));
			stfDecryptor.MessageEnd();

			/*StringSource(strMsg, 
						true, 
						new Base64Decoder(new StringSink(strPlainText)));*/

			m_stateCurrent = enMsg;
		}
	}
	catch(Exception& e) 
	{
		ostrstream bufError;
		string strError;

		bufError << reasonCryptoLib << " DESC: " << e.what();

		//ATLTRACE(_T("%s: error: %s\n"), __FUNCTION__, bufError.str());

		m_stateCurrent = enError;

		m_errorReason = reasonCryptoLib;
	}

	//ATLTRACE(_T("%s: %s\n"), __FUNCTION__, strPlainText.c_str());

	return strPlainText;
}

string CCryptoSession::ProcessEnd(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams)
{
	ATLTRACE("%s\n", __FUNCTION__);
	
	m_stateCurrent = enEnd;

	return string();
}

string CCryptoSession::ProcessError(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams)
{
	m_stateCurrent = enError;

	string::size_type nInd;

	nInd = strParams.find("404");

	if (-1 != nInd)
	{
		m_errorReason = reasonSessionNotFound;
	}

	return strParams;
}


std::ostream& CCryptoSession::GetPrologue(std::ostream& os, 
										  bool bInsertCrLf,
										  const char* szNonUIMMessage)
{
	if (szNonUIMMessage)
	{	
		os << " " << szNonUIMMessage << endl;
	}
	
	os << UOLCRYPT_TAG << " " << UOLCRYPT_VERSION;

	if (bInsertCrLf)
	{
		os << endl;	
	}

	return os;
}

std::ostream& CCryptoSession::GetEpilogue(std::ostream& os)
{
	os << endl << '\0';

	return os;
}

string CCryptoSession::StripTags(const std::string& strText)
{
	string str;	

	bool bStrip = false;

	string::const_iterator itEnd = strText.end();

	for (string::const_iterator it = strText.begin();
		 it != itEnd;
		 ++it)
	{
		if ('<' == *it)
		{
			bStrip = true;
			str += ' ';

			continue;
		}		
		else if ('>' == *it)
		{
			bStrip = false;			

			continue;
		}

		if (false == bStrip)
		{
			str +=  *it;
		}
	}

	return str;
}

string CCryptoSession::MakeSymmetricKey(ElGamal::Encryptor& encryptor)
{
	string strResult;

	try
	{
		m_rng.GenerateBlock(reinterpret_cast<byte*>(&m_keySymmetric), sizeof(KeyType));
		StringSource(reinterpret_cast<byte*>(&m_keySymmetric), 
					 sizeof(KeyType), 
					 true, 
					 new PK_EncryptorFilter(m_rng, encryptor, new HexEncoder(new StringSink(strResult))));
	}
	catch(Exception&) 
	{
		m_stateCurrent = enError;
	}
	
	return strResult;
} 

string CCryptoSession::MakeVBlock(ElGamal::Encryptor& encryptor)
{
	string strResult;

	try
	{		
		m_rng.GenerateBlock(reinterpret_cast<byte*>(&m_vBlock), sizeof(ULONGLONG));
		StringSource(reinterpret_cast<byte*>(&m_vBlock), 
					 sizeof(ULONGLONG), 
					 true, 
					 new PK_EncryptorFilter(m_rng, encryptor, new HexEncoder(new StringSink(strResult))));
	}
	catch(Exception&) 
	{
		m_stateCurrent = enError;
	}

	return strResult;
}

bool CCryptoSession::ProccessPubKey(const std::string& strSessionId, const std::string& strParams)
{
	istrstream is(strParams.c_str());

	string strKey;
	string strkeyValue;
    
	is >> strKey;

	string::size_type nInd = strKey.find("pub:");

	if (-1 != nInd)
	{
		strkeyValue = strKey.substr(4);

		//ATLTRACE(_T("%s - pub: %s\n"), __FUNCTION__, strkeyValue.c_str());

		m_mapPubKeys[strSessionId] = strkeyValue;

		return true;
	}

	return false;
}
