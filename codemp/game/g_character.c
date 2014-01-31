#include "OpenRP.h"

#include "g_local.h"
#include "g_character.h"
#include "g_account.h"
#include "g_admin.h"

extern qboolean G_CheckAdmin(gentity_t *ent, int command);

void LoadCharacterInfo(gentity_t * ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	int modelScale = 100;
	qboolean trainingSaberLocked = qfalse;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT ModelScale FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		modelScale = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	ent->client->ps.iModelScale = ent->client->sess.modelScale = modelScale;

	rc = sqlite3_prepare_v2(db, va("SELECT TrainingSaberLocked FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		trainingSaberLocked = (qboolean)sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (trainingSaberLocked)
	{
		ent->client->sess.trainingSaber = qtrue;
		ent->client->sess.trainingSaberLocked = qtrue;
	}

	sqlite3_close(db);
	return;
}

void LevelCheck(int charID)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	int i = 0, num = 0;
	int nextLevel = 0, neededSkillPoints = 0, *timesLeveled = &num, accountID = 0, clientID = -1, loggedIn = 0, currentSkillPoints = 0, currentLevel = 1;
	char charName[256] = { 0 };

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		//trap->SendServerCommand( ent-g_entities, "print \"^1The server's database is not connected.\n\"" );
		sqlite3_close(db);
		return;
	}

	//Get their accountID
	rc = sqlite3_prepare_v2(db, va("SELECT AccountID FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		accountID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}
	//Get their clientID so we can send them messages
	rc = sqlite3_prepare_v2(db, va("SELECT ClientID FROM Users WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		clientID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT LoggedIn FROM Users WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		loggedIn = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT SkillPoints FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		currentSkillPoints = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		Q_strncpyz(charName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charName));
		sqlite3_finalize(stmt);
	}

	for (i = 0; i < 49; ++i)
	{
		if (currentLevel == 50)
			break;

		nextLevel = currentLevel + 1;
		//neededSkillPoints = Q_powf( nextLevel, 2 ) * 2;
		neededSkillPoints = nextLevel * 1.3f;

		if (currentSkillPoints >= neededSkillPoints)
		{
			rc = sqlite3_exec(db, va("UPDATE Characters set Level='%i' WHERE CharID='%i'", nextLevel, charID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
			*timesLeveled++;
		}
		else
			break;
	}

	if (*timesLeveled > 0)
	{
		rc = sqlite3_prepare_v2(db, va("SELECT Level FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			currentLevel = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (*timesLeveled > 1)
		{
			if (loggedIn)
			{
				//G_Sound( &g_entities[clientID], CHAN_MUSIC, G_SoundIndex( "sound/OpenRP/levelup.mp3" ) );
				trap->SendServerCommand(clientID, va("print \"^2Level up! You leveled up %i times and are now a level %i!\n\"", *timesLeveled, currentLevel));
				trap->SendServerCommand(clientID, va("cp \"^2Level up! You leveled up %i times and are now a level %i!\n\"", *timesLeveled, currentLevel));
			}
			sqlite3_close(db);
			return;
		}

		else
		{
			if (loggedIn)
			{
				//G_Sound( &g_entities[clientID], CHAN_MUSIC, G_SoundIndex( "sound/OpenRP/levelup.mp3" ) );
				trap->SendServerCommand(clientID, va("print \"^2Level up! You are now a level %i!\n\"", currentLevel));
				trap->SendServerCommand(clientID, va("cp \"^2Level up! You are now a level %i!\n\"", currentLevel));
			}
			sqlite3_close(db);
			return;
		}
	}

	else
	{
		sqlite3_close(db);
		return;
	}
}


void Cmd_ListCharacters_F(gentity_t * ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	int charID = 0;
	char charName[256] = { 0 };

	//The database is not connected. Please do so.
	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (!ent->client->sess.loggedIn)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in to see your characters.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1You must be logged in to see your characters.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT CharID, Name FROM Characters WHERE AccountID='%i'", ent->client->sess.accountID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	trap->SendServerCommand(ent - g_entities, "print \"^2Characters:\n\"");
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		charID = sqlite3_column_int(stmt, 0);
		Q_strncpyz(charName, (const char*)sqlite3_column_text(stmt, 1), sizeof(charName));
		trap->SendServerCommand(ent - g_entities, va("print \"^2ID: ^7%i ^2Name: ^7%s\n\"", charID, charName));
	}
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return;
}

void Cmd_CreateCharacter_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	int forceSensitive = 0;
	char charName[256] = { 0 }, charNameCleaned[256] = { 0 }, temp[6] = { 0 }, comparisonName[256] = { 0 };
	int charID = 0, charSkillPoints = 1;

	//The database is not connected. Please do so.
	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (!ent->client->sess.loggedIn)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() != 3)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /createCharacter <name> <forceSensitive>\nForceSensitive: <yes/true> or <no/false>\nExample: /createCharacter luke yes\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, charName, sizeof(charName));
	Q_strncpyz(charName, charNameCleaned, sizeof(charName));
	Q_StripColor(charNameCleaned);

	trap->Argv(2, temp, sizeof(temp));

	// as: added true/false for these, just in case.
	if (!Q_stricmp(temp, "yes") || Q_stricmp(temp, "true"))
	{
		forceSensitive = 1;
	}

	else if (!Q_stricmp(temp, "no") || Q_stricmp(temp, "false"))
	{
		forceSensitive = 0;
	}

	Q_strlwr(charName);

	rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Characters WHERE AccountID='%i' AND Name='%s'", ent->client->sess.accountID, charName), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		Q_strncpyz(comparisonName, (const char*)sqlite3_column_text(stmt, 0), sizeof(comparisonName));
		sqlite3_finalize(stmt);
	}

	if (comparisonName[0] != '\0')
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You already have a character named %s.\n\"", comparisonName));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("INSERT INTO Characters(AccountID,Name,ModelScale,Level,SkillPoints,FactionID,FactionRank,ForceSensitive,CheckInventory,Credits,TrainingSaberLocked) VALUES('%i','%s','100','1','1','0','none','%i','0','250','0')",
		ent->client->sess.accountID, charName, forceSensitive), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT CharID FROM Characters WHERE AccountID='%i' AND Name='%s'", ent->client->sess.accountID, charName), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	//For debugging purposes
	if (!charID)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1Character does not exist.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1Character does not exist.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("INSERT INTO Items(CharID,E11,Pistol) VALUES('%i', '0', '0')", charID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	if (ent->client->sess.characterSelected)
	{
		ent->client->sess.skillPoints = 1;

		ent->client->sess.characterSelected = qfalse;
		ent->client->sess.characterID = 0;
		ent->client->ps.iModelScale = ent->client->sess.modelScale = 100;
	}

	ent->client->sess.characterID = charID;
	ent->client->sess.characterSelected = qtrue;

	rc = sqlite3_prepare_v2(db, va("SELECT SkillPoints FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charSkillPoints = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	ent->client->sess.skillPoints = charSkillPoints;
	LoadCharacterInfo(ent);

	trap->SendServerCommand(ent - g_entities,
		va("print \"^2Character ^7%s ^2created. It is being selected as your current character.\nIf you had colors in the name, they were removed.\n\"", charName));
	trap->SendServerCommand(ent - g_entities,
		va("cp \"^2Character ^7%s ^2created.\n^2It is being selected as your current character.\n^2If you had colors in the name, they were removed.\n\"", charName));

	sqlite3_close(db);
	return;
}

void Cmd_Character_F(gentity_t * ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	char charName[256] = { 0 };
	int charID = 0, charSkillPoints = 1;

	//The database is not connected. Please do so.
	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (!ent->client->sess.loggedIn)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in to use this command.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1You must be logged in to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /character <name>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, charName, sizeof(charName));

	Q_strlwr(charName);

	rc = sqlite3_prepare_v2(db, va("SELECT CharID FROM Characters WHERE AccountID='%i' AND Name='%s'", ent->client->sess.accountID, charName), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!charID)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1This character does not exist.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1This character does not exist.\n\"");
		sqlite3_close(db);
		return;
	}

	if (ent->client->sess.characterSelected)
	{
		ent->client->sess.skillPoints = 1;

		ent->client->sess.characterSelected = qfalse;
		ent->client->sess.characterID = 0;

		ent->client->sess.trainingSaberLocked = qfalse;
		ent->client->sess.trainingSaber = qfalse;

		ent->client->ps.iModelScale = ent->client->sess.modelScale = 100;
	}

	ent->client->sess.characterID = charID;
	ent->client->sess.characterSelected = qtrue;

	rc = sqlite3_prepare_v2(db, va("SELECT SkillPoints FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charSkillPoints = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	ent->client->sess.skillPoints = charSkillPoints;

	LoadCharacterInfo(ent);

	trap->SendServerCommand(ent - g_entities, va("print \"^2Character changed to ^7%s^2.\n\"", charName));
	trap->SendServerCommand(ent - g_entities, va("cp \"^2Character changed to ^7%s^2.\n\"", charName));

	sqlite3_close(db);
	return;
}

void Cmd_GiveCredits_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char recipientCharName[256] = { 0 }, temp[64] = { 0 }, senderCharName[256] = { 0 };
	int changedCredits = 0, charID = 0, senderCurrentCredits = 0, recipientCurrentCredits = 0, newSenderCreditsTotal = 0, newRecipientCreditsTotal = 0, accountID = 0, clientID = 33, loggedIn = 0;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (!ent->client->sess.loggedIn || !ent->client->sess.characterSelected)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() != 3)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /giveCredits <characterName> <amount>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, recipientCharName, sizeof(recipientCharName));

	//Credits added or removed.
	trap->Argv(2, temp, sizeof(temp));
	changedCredits = atoi(temp);

	Q_strlwr(recipientCharName);
	rc = sqlite3_prepare_v2(db, va("SELECT CharID FROM Characters WHERE Name='%s'", recipientCharName), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!charID)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1Character %s does not exist.\n\"", recipientCharName));
		sqlite3_close(db);
		return;
	}

	//Thanks to Kannos'v Lightdust for this fix.
	if (ent->client->sess.characterID == charID)
	{
		trap_SendServerCommand(ent - g_entities, "print \"^1Recipient cannot be you.\n\"");
		sqlite3_close(db);
		return;
	}

	if (changedCredits < 0)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1Credits must be a positive number.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		Q_strncpyz(senderCharName, (const char*)sqlite3_column_text(stmt, 0), sizeof(senderCharName));
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Credits FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		senderCurrentCredits = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (changedCredits > senderCurrentCredits)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You don't have %i credits to give. You only have %i credits.\n\"", changedCredits, senderCurrentCredits));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT AccountID FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		accountID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}
	//ClientID so we can send them messages
	rc = sqlite3_prepare_v2(db, va("SELECT ClientID FROM Users WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		clientID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT LoggedIn FROM Users WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		loggedIn = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Credits FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		recipientCurrentCredits = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	newSenderCreditsTotal = senderCurrentCredits - changedCredits;
	newRecipientCreditsTotal = recipientCurrentCredits + changedCredits;

	rc = sqlite3_exec(db, va("UPDATE Characters set Credits='%i' WHERE CharID='%i'", newSenderCreditsTotal, ent->client->sess.characterID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, va("UPDATE Characters set Credits='%i' WHERE CharID='%i'", newRecipientCreditsTotal, charID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	if (loggedIn)
		G_Say(ent, NULL, SAY_ME, va("^3gives %i credits to ^3%s^3.", changedCredits, g_entities[clientID].client->pers.netname));

	trap->SendServerCommand(ent - g_entities,
		va("print \"^2^7%i ^2of your credits have been given to character ^7%s^2. You now have ^7%i ^2credits.\n\"", changedCredits, recipientCharName, newSenderCreditsTotal));
	trap->SendServerCommand(ent - g_entities,
		va("cp \"^2^7%i ^2of your credits have been given to character ^7%s^2. You now have ^7%i ^2credits.\n\"", changedCredits, recipientCharName, newSenderCreditsTotal));

	sqlite3_close(db);
	return;
}

void Cmd_CharacterInfo_F(gentity_t * ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	char charName[256] = { 0 }, charFactionName[256] = { 0 }, charFactionRank[256] = { 0 }, forceSensitiveText[8] = { 0 };
	int nextLevel = 0, neededSkillPoints = 0, forceSensitive = 0, charFactionID = 0, charLevel = 1, charSkillPoints = 1, charCredits = 0, charModelScale = 100, charID = 0;

	if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to view your character's info.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1You must be logged in and have a character selected in order to view your character's info.\n\"");
		return;
	}

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		//Name
		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(charName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charName));
			sqlite3_finalize(stmt);
		}
		//Force Sensitive
		rc = sqlite3_prepare_v2(db, va("SELECT ForceSensitive FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			forceSensitive = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		//Faction
		rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charFactionID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		if (!charFactionID)
		{
			Q_strncpyz(charFactionName, "none", sizeof(charFactionName));
		}
		else
		{
			rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				Q_strncpyz(charFactionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionName));
				sqlite3_finalize(stmt);
			}
		}
		//Faction Rank
		rc = sqlite3_prepare_v2(db, va("SELECT FactionRank FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(charFactionRank, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionRank));
			sqlite3_finalize(stmt);
		}
		//Level
		rc = sqlite3_prepare_v2(db, va("SELECT Level FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charLevel = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		//Skill Points
		rc = sqlite3_prepare_v2(db, va("SELECT SkillPoints FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charSkillPoints = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		//Credits
		rc = sqlite3_prepare_v2(db, va("SELECT Credits FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charCredits = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		//ModelScale
		rc = sqlite3_prepare_v2(db, va("SELECT ModelScale FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charModelScale = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		switch (forceSensitive)
		{
		case 0:
			Q_strncpyz(forceSensitiveText, "No", sizeof(forceSensitiveText));
			break;
		case 1:
			Q_strncpyz(forceSensitiveText, "Yes", sizeof(forceSensitiveText));
			break;
		default:
			Q_strncpyz(forceSensitiveText, "Unknown", sizeof(forceSensitiveText));
			break;
		}

		nextLevel = charLevel + 1;
		neededSkillPoints = nextLevel * 1.3f;

		//Show them the info.
		trap->SendServerCommand(ent - g_entities,
			va("print \"^2Character Info:\n^2Name: ^7%s\n^2CharID: ^7%i\n^2Force Sensitive: ^7%s\n^2Faction: ^7%s\n^2Faction Rank: ^7%s\n^2Level: ^7%i/50\n^2Skill Points: ^7%i/%i\n^2Credits: ^7%i\n^2Modelscale: ^7%i\n\"",
			charName, ent->client->sess.characterID, forceSensitiveText, charFactionName, charFactionRank, charLevel, charSkillPoints, neededSkillPoints, charCredits, charModelScale));
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, charName, sizeof(charName));

	Q_strlwr(charName);

	rc = sqlite3_prepare_v2(db, va("SELECT CharID FROM Characters WHERE Name='%s'", charName), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!charID)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1Character ^7%s ^1does not exist.\n\"", charName));
		trap->SendServerCommand(ent - g_entities, va("cp \"^1Character ^7%s does not exist.\n\"", charName));
		sqlite3_close(db);
		return;
	}

	if (!G_CheckAdmin(ent, ADMIN_SEARCH))
	{
		rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charFactionID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (!charFactionID)
			Q_strncpyz(charFactionName, "none", sizeof(charFactionName));
		else
		{
			rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				Q_strncpyz(charFactionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionName));
				sqlite3_finalize(stmt);
			}
		}

		rc = sqlite3_prepare_v2(db, va("SELECT FactionRank FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(charFactionRank, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionRank));
			sqlite3_finalize(stmt);
		}

		trap->SendServerCommand(ent - g_entities,
			va("print \"^2Character Info:\n^2Name: ^7%s\n^2Faction: ^7%s\n^2Faction Rank: ^7%s\n\"", charName, charFactionName, charFactionRank));

		sqlite3_close(db);
		return;
	}
	else
	{
		//Name
		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(charName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charName));
			sqlite3_finalize(stmt);
		}
		//Force Sensitive
		rc = sqlite3_prepare_v2(db, va("SELECT ForceSensitive FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			forceSensitive = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		//Faction
		rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charFactionID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (!charFactionID)
			Q_strncpyz(charFactionName, "none", sizeof(charFactionName));
		else
		{
			rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				Q_strncpyz(charFactionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionName));
				sqlite3_finalize(stmt);
			}
		}

		//Faction Rank
		rc = sqlite3_prepare_v2(db, va("SELECT FactionRank FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(charFactionRank, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionRank));
			sqlite3_finalize(stmt);
		}
		//Level
		rc = sqlite3_prepare_v2(db, va("SELECT Level FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charLevel = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		//Skill Points
		rc = sqlite3_prepare_v2(db, va("SELECT SkillPoints FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charSkillPoints = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		//Credits
		rc = sqlite3_prepare_v2(db, va("SELECT Credits FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charCredits = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		//ModelScale
		rc = sqlite3_prepare_v2(db, va("SELECT ModelScale FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charModelScale = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		switch (forceSensitive)
		{
		case 0:
			Q_strncpyz(forceSensitiveText, "No", sizeof(forceSensitiveText));
			break;
		case 1:
			Q_strncpyz(forceSensitiveText, "Yes", sizeof(forceSensitiveText));
			break;
		default:
			Q_strncpyz(forceSensitiveText, "Unknown", sizeof(forceSensitiveText));
			break;
		}

		nextLevel = charLevel + 1;
		neededSkillPoints = nextLevel * 1.3f;

		//Show them the info.
		trap->SendServerCommand(ent - g_entities,
			va("print \"^2Character Info:\n^2Name: ^7%s\n^2CharID: ^7%i\n^2Force Sensitive: ^7%s\n^2Faction: ^7%s\n^2Faction Rank: ^7%s\n^2Level: ^7%i/50\n^2Skill Points: ^7%i/%i\n^2Credits: ^7%i\n^2Modelscale: ^7%i\n\"",
			charName, charID, forceSensitiveText, charFactionName, charFactionRank, charLevel, charSkillPoints, neededSkillPoints, charCredits, charModelScale));

		sqlite3_close(db);
		return;
	}
}

void Cmd_FactionInfo_F(gentity_t * ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	int charFactionID = 0, factionBank = 0;
	char charFactionName[256] = { 0 }, charFactionRank[256] = { 0 }, charName[256] = { 0 }, parameter[256] = { 0 };

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	//Name
	rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charFactionID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}
	if (!charFactionID)
	{
		Q_strncpyz(charFactionName, "none", sizeof(charFactionName));
	}
	else
	{
		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(charFactionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionName));
			sqlite3_finalize(stmt);
		}
	}

	if (!Q_stricmp(charFactionName, "none"))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are not in a faction.\n\"");
		sqlite3_close(db);
		return;
	}
	if (trap->Argc() < 2)
	{
		//Bank
		rc = sqlite3_prepare_v2(db, va("SELECT Bank FROM Factions WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			factionBank = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		//Their Rank
		rc = sqlite3_prepare_v2(db, va("SELECT FactionRank FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(charFactionRank, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionRank));
			sqlite3_finalize(stmt);
		}

		trap->SendServerCommand(ent - g_entities, va("print \"^2Faction Information:\n^2Name: ^7%s\n^2ID: ^7%i\n^2Leader(s): \"", charFactionName, charFactionID));

		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Characters WHERE FactionID='%i' AND FactionRank='Leader'", charFactionID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			Q_strncpyz(charName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charName));
			trap->SendServerCommand(ent - g_entities, va("print \"^7%s, \"", charName));
		}
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		sqlite3_finalize(stmt);

		trap->SendServerCommand(ent - g_entities, va("print \"\n^2Bank: ^7%i\n^2Your Rank: ^7%s\n\"", factionBank, charFactionRank));

		sqlite3_close(db);
		return;
	}

	trap->Argv(1, parameter, sizeof(parameter));

	if (!Q_stricmp(parameter, "roster"))
	{
		rc = sqlite3_prepare_v2(db, va("SELECT Name, FactionRank FROM Characters WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		trap->SendServerCommand(ent - g_entities, "print \"^2Roster:\n\"");
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			Q_strncpyz(charFactionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionName));
			Q_strncpyz(charFactionRank, (const char*)sqlite3_column_text(stmt, 1), sizeof(charFactionRank));
			trap->SendServerCommand(ent - g_entities, va("print \"^2Name: ^7%s ^2Rank: ^7%s\n\"", charFactionName, charFactionRank));
		}
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		sqlite3_finalize(stmt);
	}
	sqlite3_close(db);
	return;
}

void Cmd_FactionWithdraw_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char temp[64] = { 0 }, charFactionName[256] = { 0 }, charFactionRank[256] = { 0 };
	int changedCredits = 0, charFactionID = 0, factionBank = 0, characterCredits = 0, newTotalFactionBank = 0, newTotalCharacterCredits = 0;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charFactionID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}
	if (!charFactionID)
	{
		Q_strncpyz(charFactionName, "none", sizeof(charFactionName));
	}
	else
	{
		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(charFactionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionName));
			sqlite3_finalize(stmt);
		}
	}

	if (!Q_stricmp(charFactionName, "none"))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are not in a faction.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT FactionRank FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		Q_strncpyz(charFactionRank, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionRank));
		sqlite3_finalize(stmt);
	}

	if (Q_stricmp(charFactionRank, "Leader"))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are not the leader of your faction.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Bank FROM Factions WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		factionBank = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: factionWithdraw <amount>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, temp, sizeof(temp));
	changedCredits = atoi(temp);

	//Trying to withdraw a negative amount of credits
	if (changedCredits < 0)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1Please enter a positive number.\n\"");
		sqlite3_close(db);
		return;
	}

	//Trying to withdraw more than what the faction bank has.
	if (changedCredits > factionBank)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1The faction does not have ^7%i ^1credits to withdraw.\n\"", changedCredits));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Credits FROM Characters WHERE CharID='%i'", ent->client->sess.characterID, ent->client->sess.accountID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		characterCredits = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}
	newTotalFactionBank = factionBank - changedCredits;
	newTotalCharacterCredits = characterCredits + changedCredits;

	rc = sqlite3_exec(db, va("UPDATE Factions set Bank='%i' WHERE FactionID='%i'", newTotalFactionBank, charFactionID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, va("UPDATE Characters set Credits='%i' WHERE CharID='%i'", newTotalCharacterCredits, ent->client->sess.characterID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^2You have withdrawn ^7%i ^2credits from your faction's bank.\n\"", changedCredits));
	sqlite3_close(db);
	return;
}

void Cmd_FactionDeposit_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char temp[64] = { 0 }, charFactionName[256] = { 0 };
	int changedCredits = 0, charFactionID = 0, characterCredits = 0, factionBank = 0, newTotalCharacterCredits = 0, newTotalFactionBank = 0;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charFactionID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!charFactionID)
	{
		Q_strncpyz(charFactionName, "none", sizeof(charFactionName));
	}
	else
	{
		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(charFactionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charFactionName));
			sqlite3_finalize(stmt);
		}
	}

	if (!Q_stricmp(charFactionName, "none"))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are not in a faction.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: factionDeposit <amount>\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Credits FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		characterCredits = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	trap->Argv(1, temp, sizeof(temp));
	changedCredits = atoi(temp);

	//Trying to deposit a negative amount of credits
	if (changedCredits < 0)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1Please enter a positive number.\n\"");
		sqlite3_close(db);
		return;
	}

	///Trying to deposit more than what they have.
	if (changedCredits > characterCredits)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You don't have ^7%i ^1credits to deposit.\n\"", changedCredits));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Bank FROM Factions WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		factionBank = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	newTotalCharacterCredits = characterCredits - changedCredits;
	newTotalFactionBank = factionBank + changedCredits;

	rc = sqlite3_exec(db, va("UPDATE Characters set Credits='%i' WHERE CharID='%i'", newTotalCharacterCredits, ent->client->sess.characterID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("UPDATE Factions set Bank='%i' WHERE FactionID='%i'", newTotalFactionBank, charFactionID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^2You have deposited ^7%i ^2credits into your faction's bank.\n\"", changedCredits));
	sqlite3_close(db);
	return;
}

void Cmd_ListFactions_F(gentity_t * ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	int ID = 0;
	char name[256] = { 0 };

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT FactionID, Name FROM Factions"), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	trap->SendServerCommand(ent - g_entities, "print \"^2Factions:\n\"");
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		ID = sqlite3_column_int(stmt, 0);
		Q_strncpyz(name, (const char*)sqlite3_column_text(stmt, 1), sizeof(name));
		trap->SendServerCommand(ent - g_entities, va("print \"^2ID: ^7%i ^2Name: ^7%s\n\"", ID, name));
	}
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return;
}

void Cmd_Shop_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char parameter[256] = { 0 }, itemName[256] = { 0 };
	int itemCost = 0, itemLevel = 1, newTotal = 0, currentCredits = 0, currentLevel = 1, newTotalCredits = 0, currentTotal = 0, forceSensitive = 0;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities,
			va("print \"^2Shop:\nWeapons:\n^2Pistol (Level ^7%i^2) - ^7%i ^2credits\nE-11(Level ^7%i^2) - ^7%i ^2credits\nDisruptor(Level ^7%i^2) - ^7%i ^2credits\nBowcaster(Level ^7%i^2) - ^7%i ^2credits\nRepeater(Level ^7%i^2) - ^7%i ^2credits\nDEMP2(Level ^7%i^2) - ^7%i ^2credits\nFlechette(Level ^7%i^2) - ^7%i ^2credits\nRocket(Level ^7%i^2) - ^7%i ^2credits\nConcussion(Level ^7%i^2) - ^7%i ^2credits\n^2Remember: You can also use /shop <buy/examine> <item>\n\"",
			openrp_pistolLevel.integer, openrp_pistolBuyCost.integer, openrp_e11Level.integer, openrp_e11BuyCost.integer, openrp_disruptorLevel.integer, openrp_disruptorBuyCost.integer, openrp_bowcasterLevel.integer,
			openrp_bowcasterBuyCost.integer, openrp_repeaterLevel.integer, openrp_repeaterBuyCost.integer, openrp_demp2Level.integer, openrp_demp2BuyCost.integer, openrp_flechetteLevel.integer, openrp_flechetteBuyCost.integer,
			openrp_rocketLevel.integer, openrp_rocketBuyCost.integer, openrp_concussionLevel.integer, openrp_concussionBuyCost.integer));
		//Binoculars(Level ^7%i^2) - ^7%i ^2credits\nJetpack(Level ^7%i^2) - ^7%i ^2credits\nCloak(Level ^7%i^2) - ^7%i ^2credits\nTD(Level ^7%i^2) - ^7%i ^2credits\nTripmine(Level ^7%i^2) - ^7%i ^2credits\nDetpack(Level ^7%i^2) - ^7%i ^2credits\nRemember: You can also use /shop <buy/examine> <item>\n\"",
		//openrp_rocketLevel.integer, openrp_rocketBuyCost.integer, openrp_concussionLevel.integer, openrp_concussionBuyCost.integer, openrp_binocularsLevel.integer, openrp_binocularsBuyCost.integer, openrp_jetpackLevel.integer, openrp_jetpackBuyCost.integer, openrp_concussionLevel.integer, openrp_concussionBuyCost.integer, openrp_concussionLevel.integer, openrp_concussionBuyCost.integer, openrp_concussionLevel.integer, openrp_concussionBuyCost.integer, openrp_concussionLevel.integer, openrp_concussionBuyCost.integer ) );
		sqlite3_close(db);
		return;
	}

	else if (trap->Argc() != 3)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /shop <buy/examine> <item> or just /shop to see all of the shop items.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Credits FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		currentCredits = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Level FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		currentLevel = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	trap->Argv(1, parameter, sizeof(parameter));
	trap->Argv(2, itemName, sizeof(itemName));

	if (!Q_stricmp(parameter, "buy"))
	{
		rc = sqlite3_prepare_v2(db, va("SELECT ForceSensitive FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			forceSensitive = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (forceSensitive && !openrp_forceSensitiveBuyGuns.integer)
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1This server does not allow force sensitives to buy weapons from the shop.\n\"");
			sqlite3_close(db);
			return;
		}
		if (!Q_stricmp(itemName, "pistol"))
		{
			itemCost = openrp_pistolBuyCost.integer;
			itemLevel = openrp_pistolLevel.integer;
		}
		else if (!Q_stricmp(itemName, "e-11"))
		{
			itemCost = openrp_e11BuyCost.integer;
			itemLevel = openrp_e11Level.integer;
		}
		else if (!Q_stricmp(itemName, "disruptor"))
		{
			itemCost = openrp_e11BuyCost.integer;
			itemLevel = openrp_e11Level.integer;
		}
		else if (!Q_stricmp(itemName, "bowcaster"))
		{
			itemCost = openrp_bowcasterBuyCost.integer;
			itemLevel = openrp_bowcasterLevel.integer;
		}
		else if (!Q_stricmp(itemName, "repeater"))
		{
			itemCost = openrp_repeaterBuyCost.integer;
			itemLevel = openrp_repeaterLevel.integer;
		}
		else if (!Q_stricmp(itemName, "demp2"))
		{
			itemCost = openrp_demp2BuyCost.integer;
			itemLevel = openrp_demp2Level.integer;
		}
		else if (!Q_stricmp(itemName, "flechette"))
		{
			itemCost = openrp_flechetteBuyCost.integer;
			itemLevel = openrp_flechetteLevel.integer;
		}
		else if (!Q_stricmp(itemName, "rocket"))
		{
			itemCost = openrp_rocketBuyCost.integer;
			itemLevel = openrp_rocketLevel.integer;
		}
		else if (!Q_stricmp(itemName, "concussion"))
		{
			itemCost = openrp_concussionBuyCost.integer;
			itemLevel = openrp_concussionLevel.integer;
		}
		/*
		else if ( !Q_stricmp( itemName, "binoculars" ) )
		{
		itemCost = openrp_binocularsBuyCost.integer;
		itemLevel = openrp_binocularsLevel.integer;
		}
		else if ( !Q_stricmp( itemName, "jetpack" ) )
		{
		itemCost = openrp_jetpackBuyCost.integer;
		itemLevel = openrp_jetpackLevel.integer;
		}
		else if ( !Q_stricmp( itemName, "cloak" ) )
		{
		itemCost = openrp_cloakBuyCost.integer;
		itemLevel = openrp_cloakLevel.integer;
		}
		else if ( !Q_stricmp( itemName, "td" ) )
		{
		itemCost = openrp_tdBuyCost.integer;
		itemLevel = openrp_tdLevel.integer;
		}
		else if ( !Q_stricmp( itemName, "tripmine" ) )
		{
		itemCost = openrp_tripmineBuyCost.integer;
		itemLevel = openrp_tripmineLevel.integer;
		}
		else if ( !Q_stricmp( itemName, "detpack" ) )
		{
		itemCost = openrp_detpackBuyCost.integer;
		itemLevel = openrp_detpackLevel.integer;
		}
		*/
		else
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1This item is not a valid item.\n\"");
			sqlite3_close(db);
			return;
		}

		newTotalCredits = currentCredits - itemCost;

		//Trying to buy something while not having enough credits for it
		if (newTotalCredits < 0)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1You don't have enough credits to buy a ^7%s^1. You have ^7%i ^1credits and this costs ^7%i ^1credits.\n\"",
				itemName, currentCredits, itemCost));
			sqlite3_close(db);
			return;
		}

		//Trying to buy something they can't at their level
		if (currentLevel < itemLevel)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1You are not a high enough level to buy this. You are level ^7%i ^1and need to be level ^7%i^1.\n\"", currentLevel, itemLevel));
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_exec(db, va("UPDATE Characters set Credits='%i' WHERE CharID='%i'", newTotalCredits, ent->client->sess.characterID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		if (!Q_stricmp(itemName, "pistol"))
		{
			rc = sqlite3_prepare_v2(db, va("SELECT Pistol FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				currentTotal = sqlite3_column_int(stmt, 0);
				sqlite3_finalize(stmt);
			}

			newTotal = currentTotal + 1;

			rc = sqlite3_exec(db, va("UPDATE Items set Pistol='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "e-11"))
		{
			rc = sqlite3_prepare_v2(db, va("SELECT E11 FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				currentTotal = sqlite3_column_int(stmt, 0);
				sqlite3_finalize(stmt);
			}

			newTotal = currentTotal + 1;

			rc = sqlite3_exec(db, va("UPDATE Items set E11='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "disruptor"))
		{
			rc = sqlite3_prepare_v2(db, va("SELECT Disruptor FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				currentTotal = sqlite3_column_int(stmt, 0);
				sqlite3_finalize(stmt);
			}

			newTotal = currentTotal + 1;

			rc = sqlite3_exec(db, va("UPDATE Items set Disruptor='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "bowcaster"))
		{
			rc = sqlite3_prepare_v2(db, va("SELECT Bowcaster FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				currentTotal = sqlite3_column_int(stmt, 0);
				sqlite3_finalize(stmt);
			}

			newTotal = currentTotal + 1;

			rc = sqlite3_exec(db, va("UPDATE Items set Bowcaster='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "repeater"))
		{
			rc = sqlite3_prepare_v2(db, va("SELECT Repeater FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				currentTotal = sqlite3_column_int(stmt, 0);
				sqlite3_finalize(stmt);
			}

			newTotal = currentTotal + 1;

			rc = sqlite3_exec(db, va("UPDATE Items set Repeater='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "demp2"))
		{
			rc = sqlite3_prepare_v2(db, va("SELECT Demp2 FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				currentTotal = sqlite3_column_int(stmt, 0);
				sqlite3_finalize(stmt);
			}

			newTotal = currentTotal + 1;

			rc = sqlite3_exec(db, va("UPDATE Items set Demp2='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "flechette"))
		{
			rc = sqlite3_prepare_v2(db, va("SELECT Flechette FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				currentTotal = sqlite3_column_int(stmt, 0);
				sqlite3_finalize(stmt);
			}

			newTotal = currentTotal + 1;

			rc = sqlite3_exec(db, va("UPDATE Items set Flechette='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "rocket"))
		{
			rc = sqlite3_prepare_v2(db, va("SELECT Rocket FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				currentTotal = sqlite3_column_int(stmt, 0);
				sqlite3_finalize(stmt);
			}

			newTotal = currentTotal + 1;

			rc = sqlite3_exec(db, va("UPDATE Items set Rocket='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "concussion"))
		{
			rc = sqlite3_prepare_v2(db, va("SELECT Concussion FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				currentTotal = sqlite3_column_int(stmt, 0);
				sqlite3_finalize(stmt);
			}

			newTotal = currentTotal + 1;

			rc = sqlite3_exec(db, va("UPDATE Items set Concussion='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
		}
		/*
		else if ( !Q_stricmp( itemName, "binoculars" ) )
		{
		currentTotal = q.get_num( va( "SELECT Binoculars FROM Items WHERE CharID='%i'", ent->client->sess.characterID ) );
		newTotal = currentTotal + 1;
		rc = sqlite3_exec( db, va( "UPDATE Items set Binoculars='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID ), 0, 0, &zErrMsg );
		if( rc != SQLITE_OK )
		{
		trap->Print( "SQL error: %s\n", zErrMsg );
		sqlite3_free( zErrMsg );
		sqlite3_close( db );
		return;
		}
		}
		else if ( !Q_stricmp( itemName, "jetpack" ) )
		{
		currentTotal = q.get_num( va( "SELECT Jetpack FROM Items WHERE CharID='%i'", ent->client->sess.characterID ) );
		newTotal = currentTotal + 1;
		rc = sqlite3_exec( db, va( "UPDATE Items set Jetpack='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID ), 0, 0, &zErrMsg );
		if( rc != SQLITE_OK )
		{
		trap->Print( "SQL error: %s\n", zErrMsg );
		sqlite3_free( zErrMsg );
		sqlite3_close( db );
		return;
		}
		}
		else if ( !Q_stricmp( itemName, "cloak" ) )
		{
		currentTotal = q.get_num( va( "SELECT Cloak FROM Items WHERE CharID='%i'", ent->client->sess.characterID ) );
		newTotal = currentTotal + 1;
		rc = sqlite3_exec( db, va( "UPDATE Items set Cloak='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID ), 0, 0, &zErrMsg );
		if( rc != SQLITE_OK )
		{
		trap->Print( "SQL error: %s\n", zErrMsg );
		sqlite3_free( zErrMsg );
		sqlite3_close( db );
		return;
		}
		}
		else if ( !Q_stricmp( itemName, "td" ) )
		{
		currentTotal = q.get_num( va( "SELECT Td FROM Items WHERE CharID='%i'", ent->client->sess.characterID ) );
		newTotal = currentTotal + 1;
		rc = sqlite3_exec( db, va( "UPDATE Items set Td='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID ), 0, 0, &zErrMsg );
		if( rc != SQLITE_OK )
		{
		trap->Print( "SQL error: %s\n", zErrMsg );
		sqlite3_free( zErrMsg );
		sqlite3_close( db );
		return;
		}
		}
		else if ( !Q_stricmp( itemName, "tripmine" ) )
		{
		currentTotal = q.get_num( va( "SELECT Tripmine FROM Items WHERE CharID='%i'", ent->client->sess.characterID ) );
		newTotal = currentTotal + 1;
		rc = sqlite3_exec( db, va( "UPDATE Items set Tripmine='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID ), 0, 0, &zErrMsg );
		if( rc != SQLITE_OK )
		{
		trap->Print( "SQL error: %s\n", zErrMsg );
		sqlite3_free( zErrMsg );
		sqlite3_close( db );
		return;
		}
		}
		else if ( !Q_stricmp( itemName, "detpack" ) )
		{
		currentTotal = q.get_num( va( "SELECT Detpack FROM Items WHERE CharID='%i'", ent->client->sess.characterID ) );
		newTotal = currentTotal + 1;
		rc = sqlite3_exec( db, va( "UPDATE Items set Detpack='%i' WHERE CharID='%i'", newTotal, ent->client->sess.characterID ), 0, 0, &zErrMsg );
		if( rc != SQLITE_OK )
		{
		trap->Print( "SQL error: %s\n", zErrMsg );
		sqlite3_free( zErrMsg );
		sqlite3_close( db );
		return;
		}
		}
		*/
		else
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1This item is not a valid item.\n\"");
			sqlite3_close(db);
			return;
		}

		trap->SendServerCommand(ent - g_entities, va("print \"^2You have purchased a ^7%s ^2for ^7%i ^2credits.\n\"", itemName, itemCost));
		sqlite3_close(db);
		return;
	}

	else if (!Q_stricmp(parameter, "examine"))
	{
		if (!Q_stricmp(itemName, "pistol"))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"%s\n\"", openrp_pistolDescription.string));
			sqlite3_close(db);
			return;
		}
		else if (!Q_stricmp(itemName, "e-11"))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"%s\n\"", openrp_e11Description.string));
			sqlite3_close(db);
			return;
		}
		else if (!Q_stricmp(itemName, "disruptor"))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"%s\n\"", openrp_disruptorDescription.string));
			sqlite3_close(db);
			return;
		}
		else if (!Q_stricmp(itemName, "bowcaster"))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"%s\n\"", openrp_bowcasterDescription.string));
			sqlite3_close(db);
			return;
		}
		else if (!Q_stricmp(itemName, "repeater"))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"%s\n\"", openrp_repeaterDescription.string));
			sqlite3_close(db);
			return;
		}
		else if (!Q_stricmp(itemName, "demp2"))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"%s\n\"", openrp_demp2Description.string));
			sqlite3_close(db);
			return;
		}
		else if (!Q_stricmp(itemName, "flechette"))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"%s\n\"", openrp_flechetteDescription.string));
			sqlite3_close(db);
			return;
		}
		else if (!Q_stricmp(itemName, "rocket"))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"%s\n\"", openrp_rocketDescription.string));
			sqlite3_close(db);
			return;
		}
		else if (!Q_stricmp(itemName, "concussion"))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"%s\n\"", openrp_concussionDescription.string));
			sqlite3_close(db);
			return;
		}
		/*
		else if ( !Q_stricmp( itemName, "binoculars" ) )
		{
		trap->SendServerCommand( ent-g_entities, va( "print \"%s\n\"", openrp_binocularsDescription.string ) );
		sqlite3_close( db );
		return;
		}
		else if ( !Q_stricmp( itemName, "jetpack" ) )
		{
		trap->SendServerCommand( ent-g_entities, va( "print \"%s\n\"", openrp_jetpackDescription.string ) );
		sqlite3_close( db );
		return;
		}
		else if ( !Q_stricmp( itemName, "cloak" ) )
		{
		trap->SendServerCommand( ent-g_entities, va( "print \"%s\n\"", openrp_cloakDescription.string ) );
		sqlite3_close( db );
		return;
		}
		else if ( !Q_stricmp( itemName, "td" ) )
		{
		trap->SendServerCommand( ent-g_entities, va( "print \"%s\n\"", openrp_tdDescription.string ) );
		sqlite3_close( db );
		return;
		}
		else if ( !Q_stricmp( itemName, "tripmine" ) )
		{
		trap->SendServerCommand( ent-g_entities, va( "print \"%s\n\"", openrp_tripmineDescription.string ) );
		sqlite3_close( db );
		return;
		}
		else if ( !Q_stricmp( itemName, "detpack" ) )
		{
		trap->SendServerCommand( ent-g_entities, va( "print \"%s\n\"", openrp_detpackDescription.string ) );
		}
		*/
		else
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1This item is not a valid item.\n\"");
			sqlite3_close(db);
			return;
		}
	}
	else
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /shop <buy/examine> <item> or just shop to see all of the shop items.\n\"");
		sqlite3_close(db);
		return;
	}
}

void Cmd_CheckInventory_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char charName[256] = { 0 };
	int charID = 0, checkInventory = 0, pistol = 0, e11 = 0, disruptor = 0, bowcaster = 0, repeater = 0, demp2 = 0, flechette = 0, rocket = 0, concussion = 0;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		rc = sqlite3_prepare_v2(db, va("SELECT CheckInventory FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			checkInventory = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (!checkInventory)
		{
			rc = sqlite3_exec(db, va("UPDATE Characters set CheckInventory='1' WHERE CharID='%i'", ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
			trap->SendServerCommand(
				ent - g_entities, "print \"^2Others ^7can ^2check your inventory.\nTip: You can check others' inventories by using /checkInventory <characterName> if they allow it.\n\"");
			sqlite3_close(db);
			return;
		}

		else
		{
			rc = sqlite3_exec(db, va("UPDATE Characters set CheckInventory='0' WHERE CharID='%i'", ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
			trap->SendServerCommand(
				ent - g_entities, "print \"^2Others ^7cannot ^2check your inventory.\n^2Tip: You can check others' inventories by using /checkInventory <characterName> if they allow it.\n\"");
			sqlite3_close(db);
			return;
		}
	}

	trap->Argv(1, charName, sizeof(charName));

	Q_strlwr(charName);

	rc = sqlite3_prepare_v2(db, va("SELECT CharID FROM Characters WHERE Name='%s'", charName), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!charID)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1Character %s does not exist.\n\"", charName));
		trap->SendServerCommand(ent - g_entities, va("cp \"^1Character %s does not exist.\n\"", charName));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT CheckInventory FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		checkInventory = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (checkInventory || G_CheckAdmin(ent, ADMIN_ITEM))
	{
		rc = sqlite3_prepare_v2(db, va("SELECT Pistol FROM Items WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			pistol = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT E11 FROM Items WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			e11 = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Disruptor FROM Items WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			disruptor = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Bowcaster FROM Items WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			bowcaster = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Repeater FROM Items WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			repeater = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Demp2 FROM Items WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			demp2 = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Flechette FROM Items WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			flechette = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Rocket FROM Items WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			rocket = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Concussion FROM Items WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			concussion = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		trap->SendServerCommand(ent - g_entities, va("print \"^7%s's ^2Inventory:\n^2Pistols: ^7%i\n^2E-11s: ^7%i\n^2Disruptors: ^7%i\n^2Bowcasters: ^7%i\n^2Repeaters: ^7%i\n^2DEMP2s: ^7%i\n^2Flechettes: ^7%i\n^2Rocket Launchers: ^7%i\n^2Concussion Rifles: ^7%i\n\"", charName, pistol, e11, disruptor, bowcaster, repeater, demp2, flechette, rocket, concussion));
		sqlite3_close(db);
		return;
	}
	else
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1This person is not allowing inventory checks. They can allow them by using /checkInventory\n\"");
		sqlite3_close(db);
		return;
	}

}

void Cmd_Inventory_F(gentity_t * ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	int currentCredits = 0, pistol = 0, e11 = 0, disruptor = 0, bowcaster = 0, repeater = 0, demp2 = 0, flechette = 0, rocket = 0, concussion = 0;
	char parameter[256] = { 0 }, itemName[256] = { 0 };

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Credits FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		currentCredits = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Pistol FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		pistol = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT E11 FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		e11 = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Disruptor FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		disruptor = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Bowcaster FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		bowcaster = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Repeater FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		repeater = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Demp2 FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		demp2 = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Flechette FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		flechette = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Rocket FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		rocket = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Concussion FROM Items WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		concussion = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Your Inventory:\n^2Pistols: ^7%i\n^2E-11s: ^7%i\n^2Disruptors: ^7%i\n^2Bowcasters: ^7%i\n^2Repeaters: ^7%i\n^2DEMP2s: ^7%i\n^2Flechettes: ^7%i\n^2Rocket Launchers: ^7%i\n^2Concussion Rifles: ^7%i\n\"", pistol, e11, disruptor, bowcaster, repeater, demp2, flechette, rocket, concussion));
		sqlite3_close(db);
		return;
	}

	else if (trap->Argc() != 3)
	{
		//trap->SendServerCommand( ent-g_entities, "print \"^2Command Usage: /inventory <use/sell/delete> <item> or just /inventory to see your own inventory.\n\"" );
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /inventory <use> <item> or just /inventory to see your own inventory.\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, parameter, sizeof(parameter));
	trap->Argv(2, itemName, sizeof(itemName));

	if (!Q_stricmp(parameter, "use"))
	{
		if (!Q_stricmp(itemName, "pistol"))
		{
			if (pistol < 1)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^1You do not have any ^7%ss^1.\n\"", itemName));
				sqlite3_close(db);
				return;
			}

			else
			{
				ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_BRYAR_PISTOL);
				trap->SendServerCommand(ent - g_entities, va("print \"^2You have equipped a ^7%s^2.\n\"", itemName));
				sqlite3_close(db);
				return;
			}
		}

		else if (!Q_stricmp(itemName, "e-11"))
		{
			if (e11 < 1)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^1You do not have any ^7%ss^1.\n\"", itemName));
				sqlite3_close(db);
				return;
			}

			else
			{
				ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_BLASTER);
				trap->SendServerCommand(ent - g_entities, va("print \"^2You have equipped a ^7%s^2.\n\"", itemName));
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "disruptor"))
		{
			if (disruptor < 1)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^1You do not have any ^7%ss^1.\n\"", itemName));
				sqlite3_close(db);
				return;
			}

			else
			{
				ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_DISRUPTOR);
				trap->SendServerCommand(ent - g_entities, va("print \"^2You have equipped a ^7%s^2.\n\"", itemName));
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "bowcaster"))
		{
			if (bowcaster < 1)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^1You do not have any ^7%ss^1.\n\"", itemName));
				sqlite3_close(db);
				return;
			}

			else
			{
				ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_BOWCASTER);
				trap->SendServerCommand(ent - g_entities, va("print \"^2You have equipped a ^7%s^2.\n\"", itemName));
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "repeater"))
		{
			if (repeater < 1)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^1You do not have any ^7%ss^1.\n\"", itemName));
				sqlite3_close(db);
				return;
			}

			else
			{
				ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_REPEATER);
				trap->SendServerCommand(ent - g_entities, va("print \"^2You have equipped a ^7%s^2.\n\"", itemName));
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "demp2"))
		{
			if (demp2 < 1)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^1You do not have any ^7%ss^1.\n\"", itemName));
				sqlite3_close(db);
				return;
			}

			else
			{
				ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_DEMP2);
				trap->SendServerCommand(ent - g_entities, va("print \"^2You have equipped a ^7%s^2.\n\"", itemName));
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "flechette"))
		{
			if (flechette < 1)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^1You do not have any ^7%ss^1.\n\"", itemName));
				sqlite3_close(db);
				return;
			}

			else
			{
				ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_FLECHETTE);
				trap->SendServerCommand(ent - g_entities, va("print \"^2You have equipped a ^7%s^2.\n\"", itemName));
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "rocket"))
		{
			if (rocket < 1)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^1You do not have any ^7%ss^1.\n\"", itemName));
				sqlite3_close(db);
				return;
			}

			else
			{
				ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_ROCKET_LAUNCHER);
				trap->SendServerCommand(ent - g_entities, va("print \"^2You have equipped a ^7%s^2.\n\"", itemName));
				sqlite3_close(db);
				return;
			}
		}
		else if (!Q_stricmp(itemName, "concussion"))
		{
			if (concussion < 1)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^1You do not have any ^7%ss^1.\n\"", itemName));
				sqlite3_close(db);
				return;
			}

			else
			{
				ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_CONCUSSION);
				trap->SendServerCommand(ent - g_entities, va("print \"^2You have equipped a ^7%s^2.\n\"", itemName));
				sqlite3_close(db);
				return;
			}
		}
		else
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1Invalid item.\n\"");
			sqlite3_close(db);
			return;
		}
	}

	/*
	else if ( !Q_stricmp( parameter, "sell" ) )
	{
	if ( !Q_stricmp( itemName, "pistol" ) || !Q_stricmp( itemName, "Pistol" ) )
	{
	if ( pistol < 1)
	{
	trap->SendServerCommand( ent-g_entities, va( "print \"^1You do not have any ^7%ss^1.\n\"", itemName ) );
	sqlite3_close( db );
	return;
	}

	else
	{
	rc = sqlite3_exec( db, va( "UPDATE Items set Pistol='%i' WHERE CharID='%i'", newTotalItems, ent->client->sess.characterID ), 0, 0, &zErrMsg );
	if( rc != SQLITE_OK )
	{
	trap->Print( "SQL error: %s\n", zErrMsg );
	sqlite3_free( zErrMsg );
	sqlite3_close( db );
	return;
	}
	int newTotalCredits = currentCredits + openrp_pistolSellCost.integer;
	rc = sqlite3_exec( db, va( "UPDATE Character set Credits='%i' WHERE CharID='%i'", newTotalCredits, ent->client->sess.characterID ), 0, 0, &zErrMsg );
	if( rc != SQLITE_OK )
	{
	trap->Print( "SQL error: %s\n", zErrMsg );
	sqlite3_free( zErrMsg );
	sqlite3_close( db );
	return;
	}
	trap->SendServerCommand( ent-g_entities, va( "print \"^2You have sold a(n) ^7%s ^2and got ^7%s ^2credits from selling it.\n\"", itemName, openrp_pistolSellCost.integer ) );
	sqlite3_close( db );
	return;
	}
	}

	else if ( !Q_stricmp( itemName, "e-11" ) ||  !Q_stricmp( itemName, "E-11" ) )
	{
	if ( e11 < 1)
	{
	trap->SendServerCommand( ent-g_entities, va( "print \"^1You do not have any ^7%ss^2.\n\"", itemName ) );
	sqlite3_close( db );
	return;
	}

	else
	{
	newTotalItems = e11 - 1;
	rc = sqlite3_exec( db, va( "UPDATE Items set E11='%i' WHERE CharID='%i'", newTotalItems, ent->client->sess.characterID ), 0, 0, &zErrMsg );
	if( rc != SQLITE_OK )
	{
	trap->Print( "SQL error: %s\n", zErrMsg );
	sqlite3_free( zErrMsg );
	sqlite3_close( db );
	return;
	}
	newTotalCredits = currentCredits + openrp_e11SellCost.integer;
	rc = sqlite3_exec( db, va( "UPDATE Character set Credits='%i' WHERE CharID='%i'", newTotalCredits, ent->client->sess.characterID ), 0, 0, &zErrMsg );
	if( rc != SQLITE_OK )
	{
	trap->Print( "SQL error: %s\n", zErrMsg );
	sqlite3_free( zErrMsg );
	sqlite3_close( db );
	return;
	}
	trap->SendServerCommand( ent-g_entities, va( "print \"^2You have sold a(n) ^7%s ^1and got ^7%s ^2credits from selling it.\n\"", itemName, openrp_e11SellCost.integer ) );
	sqlite3_close( db );
	return;
	}
	}

	else
	{
	trap->SendServerCommand( ent-g_entities, "print \"^1Invalid item.\n\"" );
	sqlite3_close( db );
	return;
	}
	}

	else if ( !Q_stricmp( parameter, "delete" ) )
	{
	if ( !Q_stricmp( itemName, "pistol" ) || !Q_stricmp( itemName, "Pistol" ) )
	{
	if ( pistol < 1)
	{
	trap->SendServerCommand( ent-g_entities, va( "print \"^1You do not have any ^7%ss^1.\n\"", itemName ) );
	sqlite3_close( db );
	return;
	}

	else
	{
	//remove their pistol
	newTotalItems = pistol - 1;
	rc = sqlite3_exec( db, va( "UPDATE Items set Pistol='%i' WHERE CharID='%i'", newTotalItems, ent->client->sess.characterID ), 0, 0, &zErrMsg );
	if( rc != SQLITE_OK )
	{
	trap->Print( "SQL error: %s\n", zErrMsg );
	sqlite3_free( zErrMsg );
	sqlite3_close( db );
	return;
	}
	trap->SendServerCommand( ent-g_entities, va( "print \"^2You have deleted a(n) ^7%s^2.\n\"", itemName ) );
	sqlite3_close( db );
	return;
	}
	}

	else if ( !Q_stricmp( itemName, "e-11" ) || !Q_stricmp( itemName, "E-11" ) )
	{
	if ( e11 < 1)
	{
	trap->SendServerCommand( ent-g_entities, va( "print \"^1You do not have any ^7%ss^1.\n\"", itemName ) );
	sqlite3_close( db );
	return;
	}

	else
	{
	//remove their e-11
	newTotalItems = e11 - 1;
	rc = sqlite3_exec( db, va( "UPDATE Items set E11='%i' WHERE CharID='%i'", newTotalItems, ent->client->sess.characterID ), 0, 0, &zErrMsg );
	if( rc != SQLITE_OK )
	{
	trap->Print( "SQL error: %s\n", zErrMsg );
	sqlite3_free( zErrMsg );
	sqlite3_close( db );
	return;
	}
	trap->SendServerCommand( ent-g_entities, va( "print \"^2You have deleted a(n) ^7%s^2.\n\"", itemName ) );
	sqlite3_close( db );
	return;
	}
	}

	else
	{
	trap->SendServerCommand( ent-g_entities, "print \"^1Invalid item.\n\"" );
	sqlite3_close( db );
	return;
	}
	}
	*/

	else
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /inventory use <item> or just inventory to see your own inventory.\n\"");
		sqlite3_close(db);
		return;
	}
}

void Cmd_EditCharacter_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char parameter[256] = { 0 }, change[256] = { 0 }, changeCleaned[256] = { 0 }, comparisonName[256] = { 0 };
	int modelscale = 0;

	if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to edit your character's info.\n\"");
		return;
	}

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() != 3) //If the user doesn't specify both args.
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /editCharacter <name/modelscale> <value> \n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, parameter, sizeof(parameter));

	trap->Argv(2, change, sizeof(change));

	if (!Q_stricmp(parameter, "name"))
	{
		Q_strncpyz(change, changeCleaned, sizeof(change));
		Q_StripColor(changeCleaned);

		Q_strlwr(changeCleaned);

		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Characters WHERE Name='%s'", changeCleaned), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(comparisonName, (const char*)sqlite3_column_text(stmt, 0), sizeof(comparisonName));
			sqlite3_finalize(stmt);
		}

		if (comparisonName[0] != '\0')
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1Name ^7%s ^1is already in use.\n\"", comparisonName));
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_exec(db, va("UPDATE Characters set Name='%s' WHERE CharID= '%i'", changeCleaned, ent->client->sess.characterID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
		trap->SendServerCommand(ent - g_entities, va("print \"^2Name has been changed to ^7%s^2. If you had colors in the name, they were removed.\n\"", changeCleaned));
		sqlite3_close(db);
		return;
	}
	/*
	else if( !Q_stricmp( parameter, "model" ) )
	{
	rc = sqlite3_exec( db, va( "UPDATE Characters set Model='%s' WHERE CharID='%i'", changeSTR, ent->client->sess.characterID ), 0, 0, &zErrMsg );
	if( rc != SQLITE_OK )
	{
	trap->Print( "SQL error: %s\n", zErrMsg );
	sqlite3_free( zErrMsg );
	sqlite3_close( db );
	return;
	}
	trap->SendServerCommand ( ent-g_entities, va( "print \"^2Model has been changed to ^7%s^2.\n\"",changeSTR.c_str() ) );
	sqlite3_close( db );
	return;
	}
	*/
	else if (!Q_stricmp(parameter, "modelscale"))
	{
		modelscale = atoi(change);
		//if(!G_CheckAdmin(ent, ADMIN_SCALE))
		//{
		if (modelscale >= 65 && modelscale <= 140)
		{
			ent->client->ps.iModelScale = ent->client->sess.modelScale = modelscale;
			rc = sqlite3_exec(db, va("UPDATE Characters set ModelScale='%i' WHERE CharID='%i'", modelscale, ent->client->sess.characterID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
			trap->SendServerCommand(ent - g_entities, va("print \"^2Modelscale has been changed to ^7%i^2.\n\"", modelscale));
			sqlite3_close(db);
			return;
		}
		else
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1Modelscale must be from ^765 ^1- ^7140^1.\n\"");
			sqlite3_close(db);
			return;
		}
		//}
		/*
		else
		{
		if ( modelscale <= 0 || modelscale > 999 )
		{
		trap->SendServerCommand( ent-g_entities, "print \"^1Modelscale cannot be ^70^1, ^7less than 0^1, or ^7greater than 999^1.\n\"" );
		sqlite3_close( db );
		return;
		}
		ent->client->ps.iModelScale = ent->client->sess.modelScale = modelscale;
		rc = sqlite3_exec( db, va( "UPDATE Characters set ModelScale='%i' WHERE CharID='%i'", modelscale, ent->client->sess.characterID ), 0, 0, &zErrMsg );
		if( rc != SQLITE_OK )
		{
		trap->Print( "SQL error: %s\n", zErrMsg );
		sqlite3_free( zErrMsg );
		sqlite3_close( db );
		return;
		}
		trap->SendServerCommand ( ent-g_entities, va( "print \"^2Modelscale has been changed to ^7%i^2.\n\"", modelscale ) );
		sqlite3_close( db );
		return;
		}
		*/
	}

	else
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /editCharacter <name/modelscale> <value>\n\"");
		sqlite3_close(db);
		return;
	}
}

void Cmd_Bounty_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char parameter[256] = { 0 }, bountyName[256] = { 0 }, rewardTemp[64] = { 0 }, aliveDead[14] = { 0 }, bountyCreator[256] = { 0 };
	int bountyReward = 0, aliveDeadValue = 3, bountyID = 0, reward = 0, charID = 0, currentCredits = 0, newTotalCredits = 0;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		if (!G_CheckAdmin(ent, ADMIN_BOUNTY))
		{
			if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
			{
				trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_prepare_v2(db, "SELECT BountyName, Reward, Wanted, BountyID FROM Bounties", -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			trap->SendServerCommand(ent - g_entities, "print \"^2Bounties:\n\n");
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				Q_strncpyz(bountyName, (const char*)sqlite3_column_text(stmt, 0), sizeof(bountyName));
				bountyReward = sqlite3_column_int(stmt, 1);
				aliveDeadValue = sqlite3_column_int(stmt, 2);
				bountyID = sqlite3_column_int(stmt, 3);

				switch (aliveDeadValue)
				{
				case 0:
					Q_strncpyz(aliveDead, "Dead", sizeof(aliveDead));
					break;
				case 1:
					Q_strncpyz(aliveDead, "Alive", sizeof(aliveDead));
					break;
				case 2:
					Q_strncpyz(aliveDead, "Dead or Alive", sizeof(aliveDead));
					break;
				default:
					Q_strncpyz(aliveDead, "Error", sizeof(aliveDead));
					break;
				}

				trap->SendServerCommand(ent - g_entities, va("print \"^2BountyID: ^7%i ^2Name: ^7%s ^2Reward: ^7%i ^2Wanted: ^7%s\n\"",
					bountyID, bountyName, bountyReward, aliveDead));
			}
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			sqlite3_finalize(stmt);

			trap->SendServerCommand(ent - g_entities, "print \"\n^2Remember: You can add a bounty with ^2bounty add <characterName> <reward> <0(dead)/1(alive)/2(dead or alive)>\n\"");
			return;
		}

		else
		{
			rc = sqlite3_prepare_v2(db, "SELECT BountyCreator, BountyName, Reward, Wanted, BountyID FROM Bounties", -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			trap->SendServerCommand(ent - g_entities, "print \"^2Bounties:\n\n\"");
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				Q_strncpyz(bountyCreator, (const char*)sqlite3_column_text(stmt, 0), sizeof(bountyCreator));
				Q_strncpyz(bountyName, (const char*)sqlite3_column_text(stmt, 1), sizeof(bountyName));
				bountyReward = sqlite3_column_int(stmt, 2);
				aliveDeadValue = sqlite3_column_int(stmt, 3);
				bountyID = sqlite3_column_int(stmt, 4);

				switch (aliveDeadValue)
				{
				case 0:
					Q_strncpyz(aliveDead, "Dead", sizeof(aliveDead));
					break;
				case 1:
					Q_strncpyz(aliveDead, "Alive", sizeof(aliveDead));
					break;
				case 2:
					Q_strncpyz(aliveDead, "Dead or Alive", sizeof(aliveDead));
					break;
				default:
					Q_strncpyz(aliveDead, "Error", sizeof(aliveDead));
					break;
				}

				trap->SendServerCommand(ent - g_entities,
					va("print \"^2BountyID: ^7%i ^2Bounty Target: ^7%s ^2Bounty Creator: ^7%s ^2Reward: ^7%i ^2Wanted: ^7%s\n\"",
					bountyID, bountyName, bountyCreator, bountyReward, aliveDead));
			}
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			sqlite3_finalize(stmt);

			trap->SendServerCommand(ent - g_entities, "print \"\n^2Remember: You can add a bounty with ^2bounty add <characterName> <reward> <0(dead)/1(alive)/2(dead or alive)>\n\"");
			return;
		}
	}

	trap->Argv(1, parameter, sizeof(parameter));
	trap->Argv(2, bountyName, sizeof(bountyName));
	bountyID = atoi(bountyName);
	trap->Argv(3, rewardTemp, sizeof(rewardTemp));
	reward = atoi(rewardTemp);
	trap->Argv(4, aliveDead, sizeof(aliveDead));
	aliveDeadValue = atoi(aliveDead);

	if (!Q_stricmp(parameter, "add"))
	{
		if (trap->Argc() != 5)
		{
			trap->SendServerCommand(
				ent - g_entities, "print \"^2Command Usage: /bounty add <characterName> <reward> <0(dead)/1(alive)/2(dead or alive)>\nor just /bounty to view a list of current bounties.\n\"");
			if (G_CheckAdmin(ent, ADMIN_BOUNTY))
			{
				trap->SendServerCommand(ent - g_entities, "print \"^2There is also /bounty remove <bountyID>\n\"");
			}
			sqlite3_close(db);
			return;
		}

		if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
			sqlite3_close(db);
			return;
		}

		Q_strlwr(bountyName);

		rc = sqlite3_prepare_v2(db, va("SELECT CharID FROM Characters WHERE Name='%s'", bountyName), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (!charID)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1Character %s does not exist.\n\"", bountyName));
			trap->SendServerCommand(ent - g_entities, va("cp \"^1Character %s does not exist.\n\"", bountyName));
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(bountyCreator, (const char*)sqlite3_column_text(stmt, 0), sizeof(bountyCreator));
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Credits FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			currentCredits = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (reward < 500)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1You must put a bounty reward of at least ^7500^1. Your reward was ^7%i^1.\n\"", reward));
			sqlite3_close(db);
			return;
		}

		switch (aliveDeadValue)
		{
		case 0:
			Q_strncpyz(aliveDead, "Dead", sizeof(aliveDead));
			break;
		case 1:
			Q_strncpyz(aliveDead, "Alive", sizeof(aliveDead));
			break;
		case 2:
			Q_strncpyz(aliveDead, "Dead or Alive", sizeof(aliveDead));
			break;
		default:
			trap->SendServerCommand(ent - g_entities, "print \"^1Wanted must be 0 (dead), 1 (alive), or 2 (dead or alive).\n\"");
			sqlite3_close(db);
			return;
		}

		newTotalCredits = currentCredits - reward;

		if (newTotalCredits < 0)
		{
			trap->SendServerCommand(ent - g_entities,
				va("print \"^1You don't have enough credits for the reward you specified.\nYou have %i credits and your reward was %i credits.\n\"", currentCredits, reward));
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_exec(db, va("UPDATE Characters set Credits='%i' WHERE CharID='%i'", newTotalCredits, ent->client->sess.characterID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_exec(db, va("INSERT INTO Bounties(BountyCreator,BountyName,Reward,Wanted) VALUES('%s','%s','%i','%i')", bountyCreator, bountyName, reward, aliveDeadValue), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		trap->SendServerCommand(ent - g_entities, va("print \"^2You put a bounty on ^7%s (%s)^2with a reward of ^7%i ^2credits.\n\"", bountyName, aliveDead, reward));
		sqlite3_close(db);
		return;
	}

	else if (!Q_stricmp(parameter, "remove"))
	{
		if (!G_CheckAdmin(ent, ADMIN_BOUNTY))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to remove bounties.\n\""));
			sqlite3_close(db);
			return;
		}

		else
		{
			if (trap->Argc() != 3)
			{
				trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /bounty remove <bountyID>\n\"");
				trap->SendServerCommand(
					ent - g_entities, "print \"^2There is also /bounty add <characterName> <reward> <0(dead)/1(alive)/2(dead or alive)>\nor just /bounty to view a list of current bounties.\n\"");
				sqlite3_close(db);
				return;
			}

			rc = sqlite3_prepare_v2(db, va("SELECT BountyName FROM Bounties WHERE BountyID='%i'", bountyID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				Q_strncpyz(bountyName, (const char*)sqlite3_column_text(stmt, 0), sizeof(bountyName));
				sqlite3_finalize(stmt);
			}

			rc = sqlite3_prepare_v2(db, va("SELECT BountyCreator FROM Bounties WHERE BountyID='%i'", bountyID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				Q_strncpyz(bountyCreator, (const char*)sqlite3_column_text(stmt, 0), sizeof(bountyCreator));
				sqlite3_finalize(stmt);
			}

			rc = sqlite3_prepare_v2(db, va("SELECT Reward FROM Bounties WHERE BountyID='%i'", bountyID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				reward = sqlite3_column_int(stmt, 0);
				sqlite3_finalize(stmt);
			}

			rc = sqlite3_prepare_v2(db, va("SELECT Wanted FROM Bounties WHERE BountyID='%i'", bountyID), -1, &stmt, NULL);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_step(stmt);
			if (rc != SQLITE_ROW && rc != SQLITE_DONE)
			{
				trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
				sqlite3_finalize(stmt);
				sqlite3_close(db);
				return;
			}
			if (rc == SQLITE_ROW)
			{
				aliveDeadValue = sqlite3_column_int(stmt, 0);
				sqlite3_finalize(stmt);
			}

			switch (aliveDeadValue)
			{
			case 0:
				Q_strncpyz(aliveDead, "Dead", sizeof(aliveDead));
				break;
			case 1:
				Q_strncpyz(aliveDead, "Alive", sizeof(aliveDead));
				break;
			case 2:
				Q_strncpyz(aliveDead, "Dead or Alive", sizeof(aliveDead));
				break;
			default:
				Q_strncpyz(aliveDead, "Invalid Wanted Number", sizeof(aliveDead));
				sqlite3_close(db);
				return;
			}
			rc = sqlite3_exec(db, va("DELETE FROM Bounties WHERE BountyID='%i'", bountyID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
			trap->SendServerCommand(ent - g_entities,
				va("print \"^2You have removed the bounty on ^7%s (%s) ^2which had a reward of ^7%i ^2credits.\nThe bounty was put up by ^7%s^2.\n\"",
				bountyName, aliveDead, reward, bountyCreator));
			sqlite3_close(db);
			return;
		}
	}

	else
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /bounty add <characterName> <reward> <0(dead)/1(alive)/2(dead or alive)>\nor just /bounty to view a list of current bounties.\n\"");
		if (G_CheckAdmin(ent, ADMIN_BOUNTY))
		{
			trap->SendServerCommand(ent - g_entities, "print \"^2There is also /bounty remove <bountyID>\n\"");
		}
		sqlite3_close(db);
		return;
	}
}

void Cmd_CharName_F(gentity_t * ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	char cmdTarget[256] = { 0 }, charName[256] = { 0 };
	int clientid = -1;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /characterName <name/clientid>\n\""));
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, cmdTarget, sizeof(cmdTarget));

	clientid = G_ClientNumberFromName2(cmdTarget);
	if (clientid == -1)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Can't find client ID for %s\n\"", cmdTarget));
		return;
	}
	if (clientid == -2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Ambiguous client ID for %s\n\"", cmdTarget));
		return;
	}
	if (clientid >= MAX_CLIENTS || clientid < 0)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Bad client ID for %s\n\"", cmdTarget));
		return;
	}
	// either we have the client id or the string did not match 
	if (!g_entities[clientid].inuse)
	{ // check to make sure client slot is in use 
		trap->SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", cmdTarget));
		return;
	}

	if (&g_entities[clientid].client->sess.loggedIn || &g_entities[clientid].client->sess.characterSelected)
	{
		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Characters WHERE CharID='%i'", &g_entities[clientid].client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(charName, (const char*)sqlite3_column_text(stmt, 0), sizeof(charName));
			sqlite3_finalize(stmt);
		}

		trap->SendServerCommand(ent - g_entities, va("print \"^2Character Name: ^7%s\n\"", charName));
		sqlite3_close(db);
		return;
	}
	else
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^7% ^1is not logged in or does not have a character selected.\n\"", g_entities[clientid].client->pers.netname));
	}
	sqlite3_close(db);
	return;
}

void Cmd_Radio_F(gentity_t *ent)
{
	extern char	*ConcatArgs(int start);
	int pos = 0;
	char real_msg[MAX_SAY_TEXT];
	char *msg = ConcatArgs(1);
	int i = 0, j = 0; //i = A player (possibly) on the same frequency as the speaker, or standing near the speaker. j = Player who is near a recipient of the radio message (so they will hear it too)

	if (ent->client->sess.isSilenced)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are silenced and can't speak.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1You are silenced and can't speak.\n\"");
		return;
	}

	if ( ent->client->sess.radioFrequency <= 0 || ent->client->sess.radioFrequency > 100 )
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are silenced and can't speak.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1You are silenced and can't speak.\n\"");
		return;
	}


	while (*msg)
	{
		if (msg[0] == '\\' && msg[1] == 'n')
		{
			msg++;
			real_msg[pos++] = '\n';
		}
		else
		{
			real_msg[pos++] = *msg;
		}
		msg++;
	}

	real_msg[pos] = 0;

	if (trap->Argc() < 2)
	{
		if (!ent->client->sess.radioOn)
		{
			ent->client->sess.radioOn = qtrue;
			trap->SendServerCommand(ent - g_entities, "print \"^2Radio is now ON.\n\"");
			return;
		}
		else
		{
			ent->client->sess.radioOn = qfalse;
			trap->SendServerCommand(ent - g_entities, "print \"^2Radio is now OFF.\n\"");
			return;
		}
	}

	if (!ent->client->sess.radioOn)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1Your radio is off. Use /radio to turn it on.\n\"");
		return;
	}

	if (!openrp_DistanceBasedChat.integer)
	{
		for (i = 0; i < level.maxclients; i++)
		{
			if (ent->client->sess.radioFrequency == g_entities[i].client->sess.radioFrequency)
			{
				//Don't have a check for if i == ent-g_entities as this is so they see their own radio message
				//This person is on the same freq as the one who is talking, so they hear the talker
				trap->SendServerCommand(i, va("chat \"^4Radio (Freq. ^7%i^4) ^7%s ^4- ^4%s\"",
					ent->client->sess.radioFrequency, ent->client->pers.netname, real_msg));
			}
		}
	}
	else
	{
		for (i = 0; i < level.maxclients; i++)
		{
			
			//Check the distance from speaker to other players to see if any of them are near the speaker
			if (Distance(ent->client->ps.origin, g_entities[i].client->ps.origin) <= 600)
			{
				if (i == ent - g_entities)
					continue;
				trap->SendServerCommand(i, va("chat \"^7Talking on Their Radio ^7%s ^7- ^2%s\"", ent->client->pers.netname, real_msg));
			}
			else
				continue;

			//Check if any players are on the same frequency as the speaker
			if (ent->client->sess.radioFrequency == g_entities[i].client->sess.radioFrequency)
			{
				trap->SendServerCommand(i, va("chat \"^4Radio (Freq. ^7%i^4) ^7%s ^4- ^4%s\"",
					ent->client->sess.radioFrequency, ent->client->pers.netname, real_msg));

				
				for (j = 0; j < level.maxclients; j++)
				{
					//Check the distance from a player receiving the radio chatter (i) to the other players (j) to see if any are near them 
					if (Distance(g_entities[i].client->ps.origin, g_entities[j].client->ps.origin) <= 600)
					{
						//Make it so the one who is receiving the radio chatter doesn't hear this
						//We could do j == ent-g_entities too so the original player who is talking over the radio doesn't see this
						//However it's more realistic this way
						if (i == j)
							continue;
						trap->SendServerCommand(i, va("chat \"^4Heard on ^7%s's ^4radio - ^4%s\"", g_entities[i].client->ps.origin, real_msg));
					}
				}
			}

			if (g_entities[i].client->sess.allChat || g_entities[i].client->sess.allChatComplete || g_entities[i].client->sess.sessionTeam == TEAM_SPECTATOR || g_entities[i].client->tempSpectate >= level.time)
			{
				trap->SendServerCommand(i, va("chat \"^1<All Chat>^7Radio (Freq. ^7%i^4) ^7%s ^4- ^4%s\"",
					ent->client->sess.radioFrequency, ent->client->pers.netname, real_msg));

				if (g_entities[i].client->sess.sessionTeam == TEAM_SPECTATOR || g_entities[i].client->tempSpectate >= level.time)
					return;
			}
		}
	}
	return;
}

void Cmd_Frequency_F(gentity_t *ent)
{
	char frequencyTemp[12];
	int frequency = 0;

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /frequency <frequency> Frequencies must be from 0 to 100.\nExample: /frequency 43\n\"");
		return;
	}

	trap->Argv(1, frequencyTemp, sizeof(frequencyTemp));
	frequency = strtod(frequencyTemp, NULL);

	if (frequency < 0 || frequency > 100)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1Frequencies must be from 0 to 100.\nExample: /frequency 43\n\"");
		return;
	}
	ent->client->sess.radioFrequency = frequency;
	trap->SendServerCommand(ent - g_entities, va("print \"^2Frequency set to ^7%i\n\"", frequency));
	return;
}

void Cmd_ForceMessage_F(gentity_t *ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	char *ConcatArgs(int start);
	int pos = 0, clientid = -1, i = 0, forceSensitive = 0;
	char real_msg[MAX_SAY_TEXT] = { 0 }, *msg = ConcatArgs(2), cmdTarget[512] = { 0 };

	if (ent->client->sess.isSilenced)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are silenced and can't speak.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1You are silenced and can't speak.\n\"");
		return;
	}

	//The database is not connected. Please do so.
	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (!ent->client->sess.characterSelected)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must have a character selected to use this command.\nThis is to confirm that you're force sensitive.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT ForceSensitive FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		forceSensitive = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!forceSensitive)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1Your character isn't force sensitive.\n\"");
		sqlite3_close(db);
		return;
	}

	while (*msg)
	{
		if (msg[0] == '\\' && msg[1] == 'n')
		{
			msg++;
			real_msg[pos++] = '\n';
		}
		else
		{
			real_msg[pos++] = *msg;
		}
		msg++;
	}

	real_msg[pos] = 0;

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /force <name/clientid> <message>\n\""));
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, cmdTarget, sizeof(cmdTarget));

	clientid = G_ClientNumberFromName2(cmdTarget);
	if (clientid == -1)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Can't find client ID for %s\n\"", cmdTarget));
		return;
	}
	if (clientid == -2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Ambiguous client ID for %s\n\"", cmdTarget));
		return;
	}
	if (clientid >= MAX_CLIENTS || clientid < 0)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Bad client ID for %s\n\"", cmdTarget));
		return;
	}
	// either we have the client id or the string did not match 
	if (!g_entities[clientid].inuse)
	{ // check to make sure client slot is in use 
		trap->SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", cmdTarget));
		return;
	}

	for (i = 0; i < level.maxclients; i++)
	{
		if ((g_entities[i].client->sess.allChat && Q_stricmp(ent->client->pers.netname, g_entities[clientid].client->pers.netname)) || g_entities[i].client->sess.allChatComplete || g_entities[i].client->sess.sessionTeam == TEAM_SPECTATOR || g_entities[i].client->tempSpectate >= level.time)
		{
			trap->SendServerCommand(i, va("chat \"^1<All Chat>^7<%s ^7to %s^7> ^5%s\"", ent->client->pers.netname, g_entities[clientid].client->pers.netname, real_msg));
		}
		else
			continue;
	}

	trap->SendServerCommand(ent - g_entities, va("chat \"^7<%s ^7to %s^7> ^5%s\"", ent->client->pers.netname, g_entities[clientid].client->pers.netname, real_msg));
	trap->SendServerCommand(clientid, va("chat \"^7<%s ^7to %s^7> ^5%s\"", ent->client->pers.netname, g_entities[clientid].client->pers.netname, real_msg));
	sqlite3_close(db);
	return;
}

void Cmd_Faction_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char factionID[64] = { 0 }, factionName[512] = { 0 }, currentFactionName[512] = { 0 };
	int charFactionID = 0;

	if (!ent->client->sess.characterSelected)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must have a character selected to use this command.\n\"");
		return;
	}

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charFactionID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!charFactionID)
	{
		Q_strncpyz(currentFactionName, "none", sizeof(currentFactionName));
	}
	else
	{
		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(currentFactionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(currentFactionName));
			sqlite3_finalize(stmt);
		}
	}

	//If they're already in a faction
	if (Q_stricmp(currentFactionName, "none"))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are already in a faction and must leave it using /factionLeave before joining a new one.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /faction <factionID> \nUse /listFactions for factionIDs.\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, factionID, sizeof(factionID));

	rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE ID='%i'", atoi(factionID)), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		Q_strncpyz(factionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(factionName));
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_exec(db, va("UPDATE Characters set FactionID='%i' WHERE CharID='%i'", factionID, ent->client->sess.characterID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, va("UPDATE Characters set FactionRank='Member' WHERE CharID='%i'", ent->client->sess.characterID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^2You have joined the %s faction!\nYou can use /factionInfo to view info about it.\n\"", factionName));
	sqlite3_close(db);
	return;
}

void Cmd_FactionLeave_F(gentity_t *ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	int factionID = 0;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (!ent->client->sess.characterSelected)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must have a character selected to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		factionID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!factionID)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You aren't in a faction.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("UPDATE Characters set FactionID='0' WHERE CharID='%i'", ent->client->sess.characterID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, va("UPDATE Characters set FactionRank='none' WHERE CharID='%i'", ent->client->sess.characterID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->SendServerCommand(ent - g_entities, "print \"^2You left your faction.\n\"");
	sqlite3_close(db);
	return;
}

void Cmd_FactionInvite_F(gentity_t *ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	char charName[256] = { 0 }, inviterFactionRank[256] = { 0 }, inviterFactionName[256] = { 0 };
	int charID = 0, accountID = 0, clientID = 33, loggedIn = 0, inviterFactionID = 0, inviteeFactionID = 0;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		inviterFactionID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE FactionID='%i'", inviterFactionID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		Q_strncpyz(inviterFactionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(inviterFactionName));
		sqlite3_finalize(stmt);
	}

	if (!inviterFactionID)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You aren't in a faction.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT FactionRank FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		Q_strncpyz(inviterFactionRank, (const char*)sqlite3_column_text(stmt, 0), sizeof(inviterFactionRank));
		sqlite3_finalize(stmt);
	}

	if (Q_stricmp(inviterFactionRank, "Leader"))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are not the leader of your faction.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /factionInvite <characterName>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, charName, sizeof(charName));

	Q_strlwr(charName);

	rc = sqlite3_prepare_v2(db, va("SELECT CharID FROM Characters WHERE Name='%s'", charName), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		charID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!charID)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1Character ^7%s ^1does not exist.\n\"", charName));
		trap->SendServerCommand(ent - g_entities, va("cp \"^1Character ^7%s does not exist.\n\"", charName));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT AccountID FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		accountID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}
	//ClientID so we can send them messages
	rc = sqlite3_prepare_v2(db, va("SELECT ClientID FROM Users WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		clientID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}
	rc = sqlite3_prepare_v2(db, va("SELECT LoggedIn FROM Users WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		loggedIn = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!loggedIn)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1This character is not online. They must be online to be invited to your faction.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", g_entities[clientID].client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		inviteeFactionID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (inviteeFactionID != 0)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1This character is already in a faction. They must leave it to be invited to yours.\n\"");
		sqlite3_close(db);
		return;
	}

	g_entities[clientID].client->sess.invitedFactionID = inviterFactionID;

	trap->SendServerCommand(ent - g_entities, "print \"^2Character invited to your faction.\n\"");
	trap->SendServerCommand(clientID, va("print \"^2You have been invited to the %s faction. Use /factionInviteAccept to accept the invite\nor /factionInviteDecline to decline it.\n\"", inviterFactionName));
	sqlite3_close(db);
	return;
}

void Cmd_FactionInviteAccept_F(gentity_t *ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	int factionID = 0;
	char factionName[256] = { 0 };

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (!ent->client->sess.loggedIn || !ent->client->sess.characterSelected)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		factionID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (factionID != 0)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are already in a faction and must leave it using /factionLeave before joining a new one.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE ID='%i'", ent->client->sess.invitedFactionID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		Q_strncpyz(factionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(factionName));
		sqlite3_finalize(stmt);
	}

	rc = sqlite3_exec(db, va("UPDATE Characters set FactionID='%i' WHERE CharID='%i'", ent->client->sess.invitedFactionID, ent->client->sess.characterID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, va("UPDATE Characters set FactionRank='Member' WHERE CharID='%i'", ent->client->sess.characterID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^2You have joined the %s faction!\nYou can use /factionInfo to view info about it.\n\"", factionName));
	sqlite3_close(db);
	return;
}

void Cmd_FactionInviteDecline_F(gentity_t *ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	char factionName[256] = { 0 };

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if ((!ent->client->sess.loggedIn) || (!ent->client->sess.characterSelected))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
		sqlite3_close(db);
		return;
	}

	ent->client->sess.invitedFactionID = 0;

	rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE ID='%i'", ent->client->sess.invitedFactionID), -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	if (rc == SQLITE_ROW)
	{
		Q_strncpyz(factionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(factionName));
		sqlite3_finalize(stmt);
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^1You declined the invite to the %s faction\n\"", factionName));
	sqlite3_close(db);
	return;
}

void Cmd_SetFactionRank_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char charName[512] = { 0 }, factionRank[256] = { 0 }, factionName[256] = { 0 }, cmdUserFactionRank[256] = { 0 };
	int accountID = 0, clientID = -1, loggedIn = 0, charFactionID = 0, cmdUserFactionID = 0, charID = 0;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() != 3)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /setFactionRank <characterName> <rankName>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, charName, sizeof(charName));

	trap->Argv(2, factionRank, sizeof(factionRank));

	if (G_CheckAdmin(ent, ADMIN_FACTION))
	{
		Q_strlwr(charName);

		rc = sqlite3_prepare_v2(db, va("SELECT CharID FROM Characters WHERE Name='%s'", charName), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (!charID)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1Character %s does not exist.\n\"", charName));
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_prepare_v2(db, va("SELECT AccountID FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			accountID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		//ClientID so we can send them messages
		rc = sqlite3_prepare_v2(db, va("SELECT ClientID FROM Users WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			clientID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		rc = sqlite3_prepare_v2(db, va("SELECT LoggedIn FROM Users WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			loggedIn = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charFactionID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (!charFactionID)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^7%s ^1isn't in a faction.\n\"", charName));
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE FactionID='%i'", charFactionID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(factionName, (const char*)sqlite3_column_text(stmt, 0), sizeof(factionName));
			sqlite3_finalize(stmt);
		}

		if (!Q_stricmp(factionRank, "leader"))
		{
			//Make sure the L is capital as that's how other commands expect it to be.
			Q_strncpyz(factionRank, "Leader", sizeof(factionRank));
		}

		rc = sqlite3_exec(db, va("UPDATE Characters set FactionRank='%s' WHERE CharID='%i'", factionRank, charID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		if (loggedIn)
		{
			trap->SendServerCommand(clientID, va("print \"^2You are now the %s rank in the %s faction!\n\"", factionRank, factionName));
			trap->SendServerCommand(clientID, va("cp \"^2You are now the %s rank in the %s faction!\n\"", factionRank, factionName));
		}

		trap->SendServerCommand(ent - g_entities, va("print \"^2Character %s is now the %s rank in the %s faction.\n\"", charName, factionRank, factionName));
		sqlite3_close(db);
		return;
	}

	else
	{
		if (!ent->client->sess.characterSelected)
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1You must have a character selected to use this command.\n\"");
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			cmdUserFactionID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (!cmdUserFactionID)
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1You aren't in a faction.\n\"");
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_prepare_v2(db, va("SELECT FactionRank FROM Characters WHERE CharID='%i'", ent->client->sess.characterID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			Q_strncpyz(cmdUserFactionRank, (const char*)sqlite3_column_text(stmt, 0), sizeof(cmdUserFactionRank));
			sqlite3_finalize(stmt);
		}

		if (!Q_stricmp(cmdUserFactionRank, "Leader"))
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1You are not the leader of your faction.\n\"");
			sqlite3_close(db);
			return;
		}

		Q_strlwr(charName);

		rc = sqlite3_prepare_v2(db, va("SELECT CharID FROM Characters WHERE Name='%s'", charName), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (!charID)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1Character %s does not exist.\n\"", charName));
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_prepare_v2(db, va("SELECT AccountID FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			accountID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}
		//Get their clientID so we can send them messages
		rc = sqlite3_prepare_v2(db, va("SELECT ClientID FROM Users WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			clientID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT LoggedIn FROM Users WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			loggedIn = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Characters WHERE CharID='%i'", charID), -1, &stmt, NULL);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE)
		{
			trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_close(db);
			return;
		}
		if (rc == SQLITE_ROW)
		{
			charFactionID = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		if (cmdUserFactionID != charFactionID)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1You aren't in the same faction as %s. You can't change their rank.", charName));
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_exec(db, va("UPDATE Characters set FactionRank='%s' WHERE CharID='%i'", factionRank, charID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		if (loggedIn)
		{
			trap->SendServerCommand(clientID, va("print \"^2You are now the %s rank in the %s faction!\n\"", factionRank, factionName));
			trap->SendServerCommand(clientID, va("cp \"^2You are now the %s rank in the %s faction!\n\"", factionRank, factionName));
		}

		trap->SendServerCommand(ent - g_entities, va("print \"^2Character %s is now the %s rank in the %s faction.\n\"", charName, factionRank, factionName));
		sqlite3_close(db);
		return;
	}
}
