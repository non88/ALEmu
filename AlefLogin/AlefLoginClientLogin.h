#pragma once

#include "Poco/Net/StreamSocket.h"
using Poco::Net::StreamSocket;

#include "AlefPacketProcessor.h"
#include "AlefSocket.h"

class AlefLoginClientLogin : public AlefPacketProcessor
{
public:
	AlefLoginClientLogin();
	virtual ~AlefLoginClientLogin();

	//virtual bool processPacket(localInfo& local);
	virtual bool processPacket(const localInfo& local);
	bool processHashKeyPacket(localInfo& local);
	bool processUserLoginPacket(localInfo& local, string acct, UInt8 acctLen, string pw, UInt8 pwLen);
	bool processUnionInfo(localInfo& local, string worldName);
	bool processCharacterList(localInfo& local);
	bool processCharacterCreation(localInfo& local);
	bool processCreateCharacter(localInfo& local, SharedPtr<AlefPacket> charDetail);
	bool processWorldEnterRequest(localInfo& local, SharedPtr<AlefPacket> charDetail);
	bool processDeleteCharacter(localInfo& local, SharedPtr<AlefPacket> charDetail);

	enum CLIENTLOGINOPERATION
	{
		CLIENTLOGIN_HASHKEY,
		CLIENTLOGIN_USERLOGIN,
		CLIENTLOGIN_UNIONINFO = 3,
		CLIENTLOGIN_CHARNAME = 4,
		CLIENTLOGIN_CHARNAMEFINISH = 5,
		CLIENTLOGIN_CHARLIST = 6,
		CLIENTLOGIN_CHARLISTFINISH = 7,
		CLIENTLOGIN_WORLDENTER = 8,
		CLIENTLOGIN_CHARCREATIONENTER = 10,
		CLIENTLOGIN_CREATECHAR = 11,
		CLIENTLOGIN_CREATECHARFINISH = 12,
		CLIENTLOGIN_DELETECHAR = 13,
		CLIENTLOGIN_LOGINRESULT = 15,
	};

	enum LOGINRESULT
	{
		INCORRECTUSER = 0,
		GIBBERISH = 1,
		TOOMANYATTEMPTS,
		TOSAGREEMENT,
	};

	enum LOGINSTEP
	{
		HASHKEY = 0,
		AUTHENTICATED,
		CHARLIST,
		CHARCREATE,
		ENTERWORLD,
	};
	
private:
	void sendLoginResult(AlefSocket& sock, int loginResult);

	enum class BASECHAR : UInt32
	{
		KNIGHT = 1,
		ARCHER,
		MAGE,
		ZERKER,
		HUNTER,
		SORC,
		SWASH,
		RANGER,
		ELEMENTALIST,
		SCION,
		SLAYER,
		ORBITER,
	};

	bool sendBaseCharPackets(AlefSocket& sock);
	SharedPtr<AlefPacket> getBaseCharPacket(BASECHAR baseChar);
	SharedPtr<AlefPacket> buildCharPacket(CharacterData &data);
	SharedPtr<AlefPacket> buildCharMovePacket(CharacterMove& move);
	SharedPtr<AlefPacket> buildFactorPacket(CharacterFactors& factors);
	SharedPtr<AlefPacket> buildFactorResultPacket(CharacterFactorResult& result);
	clientCharDataVec baseChars;

	int baseSessionID;
	string curWorld;
};