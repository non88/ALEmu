#include "AlefWorldServerSystems.h"

constexpr auto SINGLEROW = 0; //For single 0th row values;

AlefServerCharSys::AlefServerCharSys()
{
	dbCharSys = new AlefDBCharSys();
}

AlefServerCharSys::~AlefServerCharSys()
{
	delete dbCharSys;
}

UInt32 AlefServerCharSys::getAcctID(Int32 authKey)
{
	UInt32 acctID = -1;
	SharedPtr<RecordSet> rs = dbCharSys->dbGetAcctID(authKey);

	if (!rs)
		return 0;
	if (rs->getTotalRowCount() != 1) //We should only get one row returned
		return 0;

	acctID = rs->row(SINGLEROW).get(COLIDX(ACCOUNTCOL::ACCTID));

	return acctID;
}

CharacterData AlefServerCharSys::getCharacter(UInt32 acctID, string& charName)
{
	string cleanName = charName.substr(0, charName.find("#", 0));
	CharacterData charData;
	charData.charID = -1;

	SharedPtr<RecordSet> rs = dbCharSys->dbGetChara(acctID, cleanName);

	if (!rs)
		return charData;
	if (rs->getTotalRowCount() != 1) //we should only get one character given to us
		return charData;

	for (RecordSet::Iterator itr = rs->begin(); itr != rs->end(); itr++)
	{
		charData.slot = 1; //current character slot will always be 1

		charData.charID = itr->get(COLIDX(CHARCOL::CHARID));
		charData.charName = itr->get(COLIDX(CHARCOL::CHARNAME)).toString();
		charData.charTID = itr->get(COLIDX(CHARCOL::CHARTID));
		charData.hair = itr->get(COLIDX(CHARCOL::HAIR));
		charData.face = itr->get(COLIDX(CHARCOL::FACE));

		//Position
		charData.charMove.curPos.x = itr->get(COLIDX(CHARCOL::POSX));
		charData.charMove.curPos.y = itr->get(COLIDX(CHARCOL::POSY));
		charData.charMove.curPos.z = itr->get(COLIDX(CHARCOL::POSZ));

		//I want to redo this but there isn't really a better way atm
		Int32 charLevel = itr->get(COLIDX(CHARCOL::LEVEL));
		charData.charFactors.result.status.level = charLevel;
		charData.charFactors.status.level = charLevel;

		Int32 charHP = itr->get(COLIDX(CHARCOL::HP)),
			charMana = itr->get(COLIDX(CHARCOL::MP)),
			charSP = itr->get(COLIDX(CHARCOL::SKILLPOINT));

		charData.charFactors.result.point.hp = charHP;
		charData.charFactors.result.point.mana = charMana;
		charData.charFactors.result.point.skillPoints = charSP;

		charData.charFactors.point.hp = charHP;
		charData.charFactors.point.mana = charMana;
		charData.charFactors.point.skillPoints = charSP;
	}

	initStatusFactor(charData);
	initTypeFactor(charData);
	initPointMaxFactor(charData);
	initAttackFactor(charData);

	return charData;
}

void AlefServerCharSys::initCharFactors(CharacterData& charData)
{
	initStatusFactor(charData);
	initTypeFactor(charData);
	initPointMaxFactor(charData);
	initAttackFactor(charData);
}

void AlefServerCharSys::initStatusFactor(CharacterData& charData)
{
	UInt32 TID = charData.charTID;

	SharedPtr<CharDataInfo> serverChar = serverDataSys->getCharDataFromTemplID(TID);

	if (serverChar.isNull())
		return;


	charData.charFactors.result.status.stamina = serverChar->CON * 100; //* levelFactor
	charData.charFactors.result.status.strength = serverChar->STR * 100;
	charData.charFactors.result.status.intelligence = serverChar->INT * 100;
	charData.charFactors.result.status.dexterity = serverChar->DEX * 100;
	charData.charFactors.result.status.charisma = serverChar->CHA * 100;
	charData.charFactors.result.status.luck = 0;
	charData.charFactors.result.status.wisdom = serverChar->WIS * 100;
	charData.charFactors.result.status.moveSpeed = serverChar->walkSpeed;
	charData.charFactors.result.status.runSpeed = serverChar->runSpeed;

	charData.charFactors.status.stamina = serverChar->CON * 100; //* levelFactor
	charData.charFactors.status.strength = serverChar->STR * 100;
	charData.charFactors.status.intelligence = serverChar->INT * 100;
	charData.charFactors.status.dexterity = serverChar->DEX * 100;
	charData.charFactors.status.charisma = serverChar->CHA * 100;
	charData.charFactors.status.luck = 0;
	charData.charFactors.status.wisdom = serverChar->WIS * 100;
	charData.charFactors.status.moveSpeed = serverChar->walkSpeed;
	charData.charFactors.status.runSpeed = serverChar->runSpeed;
}

void AlefServerCharSys::initTypeFactor(CharacterData& charData)
{
	UInt32 TID = charData.charTID;
	SharedPtr<CharDataInfo> serverChar = serverDataSys->getCharDataFromTemplID(TID);

	if (serverChar.isNull())
		return;

	charData.charFactors.result.type.charRace = serverChar->race;
	charData.charFactors.result.type.charGender = serverChar->gender;
	charData.charFactors.result.type.charClass = serverChar->clss;

	charData.charFactors.type.charRace = serverChar->race;
	charData.charFactors.type.charGender = serverChar->gender;
	charData.charFactors.type.charClass = serverChar->clss;
}

void AlefServerCharSys::initPointMaxFactor(CharacterData& charData)
{
	UInt32 TID = charData.charTID;

	SharedPtr<CharDataInfo> serverChar = serverDataSys->getCharDataFromTemplID(TID);

	if (serverChar.isNull())
		return;

	charData.charFactors.result.pointMax.maxHP = serverChar->MAXHP; //* levelFactor
	charData.charFactors.result.pointMax.maxMana = serverChar->MAXMP;
	charData.charFactors.result.pointMax.maxSkillPoints = serverChar->MAXSP;
	//charData.charFactors.result.pointMax.xpLow = serverChar->exp;
	//charData.charFactors.result.pointMax.xpHigh = serverDataSys->getTemplateField(TID, 32);
	//charData.charFactors.result.pointMax.AP = serverChar->;
	charData.charFactors.result.pointMax.MAP = serverChar->MAR; //Is this correct?
	//charData.charFactors.result.pointMax.MI = serverChar->;
	charData.charFactors.result.pointMax.AR = serverChar->AR;
	charData.charFactors.result.pointMax.DR = serverChar->DR;

	charData.charFactors.pointMax.maxHP = serverChar->MAXHP; //* levelFactor
	charData.charFactors.pointMax.maxMana = serverChar->MAXMP;
	charData.charFactors.pointMax.maxSkillPoints = serverChar->MAXSP;
	//charData.charFactors.pointMax.xpLow = serverChar->exp;
	//charData.charFactors.pointMax.xpHigh = serverDataSys->getTemplateField(TID, 32);
	//charData.charFactors.pointMax.AP = serverChar->;
	charData.charFactors.pointMax.MAP = serverChar->MAR; //Is this correct?
	//charData.charFactors.pointMax.MI = serverChar->;
	charData.charFactors.pointMax.AR = serverChar->AR;
	charData.charFactors.pointMax.DR = serverChar->DR;
}

void AlefServerCharSys::initAttackFactor(CharacterData& charData)
{
	UInt32 TID = charData.charTID;
	SharedPtr<CharDataInfo> serverChar = serverDataSys->getCharDataFromTemplID(TID);

	if (serverChar.isNull())
		return;

	charData.charFactors.result.attack.range = serverChar->range;
	//charData.charFactors.result.attack.hitRange = serverChar->hit;
	charData.charFactors.result.attack.attackSpeed = serverChar->atkSpeed;

	charData.charFactors.attack.range = serverChar->range;
	//charData.charFactors.attack.hitRange = serverDataSys->getTemplateField(TID, 65);
	charData.charFactors.attack.attackSpeed = serverChar->atkSpeed;
}
