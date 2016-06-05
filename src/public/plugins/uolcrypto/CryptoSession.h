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


#pragma once


struct _128Bits
{
	ULONGLONG _hi;
	ULONGLONG _lo;
};

typedef std::map<std::string, std::string>		CPubKeyMap;
typedef ULONGLONG								KeyType;	 

class CCryptoSession
{
public:	
	enum State
	{
		enInit,
		enInvite,
		enAccept,
		enSetKey,
		enMsg, 
		enEnd,
		enError = 0xfffffe,
		enUnknow = 0xffffff
	};

	enum ErrorReason
	{
		reasonNoError,
		reasonCryptoLib,
		reasonSessionNotFound,
		reasonAcceptTimeout
	};


	CCryptoSession(CPubKeyMap& mapPubKeys, 
				   CryptoPP::RandomNumberGenerator& rng, 
				   const char* szNonUIMMessage);
	~CCryptoSession(void);	

	std::string MakeInvite(const std::string& strFromUser);
	std::string MakeAccept();
	std::string MakeSetKey(const std::string& strFromUser, const std::string& strToUser);
	std::string MakeMsg(const std::string& strMsg);
	std::string MakeEnd();
	std::string MakeError(const std::string& strReason);
	std::string MakeError(ErrorReason reason);
	
	std::string ProcessCryptoMsg(const std::string& strFromUser, const std::string& strToUser, const std::string& strMsg);

	State GetCurrentState() const;
	ErrorReason GetError() const;

	static bool IsCryptoMsg(std::string& strMsg);
	static bool IsInviteMsg(std::string& strMsg);

	static std::string BuildSessionId(const std::string& strFromUser, const std::string& strToUser);

protected:
	std::string ProcessInvite(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams);
	std::string ProcessAccept(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams);
	std::string ProcessSetKey(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams);
	std::string ProcessMsg(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams);
	std::string ProcessEnd(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams);
	std::string ProcessError(const std::string& strFromUser, const std::string& strToUser, const std::string& strParams);

private:
	static std::ostream& GetPrologue(std::ostream& os, 
									 bool bInsertCrLf = true,
									 const char* szNonUIMMessage = NULL); 

	static std::ostream& GetEpilogue(std::ostream& os); 
	static std::string StripTags(const std::string& strText);

	std::string MakeSymmetricKey(CryptoPP::ElGamal::Encryptor& encryptor);
	std::string MakeVBlock(CryptoPP::ElGamal::Encryptor& encryptor);

	bool ProccessPubKey(const std::string& strUser, const std::string& strParams); 


private:
	typedef unsigned __int64 ULONGLONG;

	static const std::string			UOLCRYPT_TAG;
	static const std::string			UOLCRYPT_VERSION;

	CryptoPP::RandomNumberGenerator&	m_rng;

	CryptoPP::ElGamal::Decryptor		m_keyPriv;
	CryptoPP::ElGamal::Encryptor		m_keyPub;

	KeyType								m_keySymmetric;
	ULONGLONG							m_vBlock;

	CPubKeyMap&							m_mapPubKeys;

	State								m_stateCurrent;
	ErrorReason							m_errorReason;

	const char*							m_szNonUIMMessage;

	std::string							m_strStartedUser;
};
