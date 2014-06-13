#include "OpenRP.h"

#include "../g_local.h"
#include "g_admin.h"
#include "g_OpenRP.h"

void InitializeSQL(void)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;

	//The database is not connected. Please do so.
	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		//trap->SendServerCommand( ent-g_entities, "print \"^1The server's database is not connected.\n\"" );
		sqlite3_close(db);
		return;
	}

	//Create Bounty Table
	trap->Print("Initializing Bounty Table.\n");

	//rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Bounties(BountyID INTEGER PRIMARY KEY, Wanted INTEGER, BountyCreator TEXT, BountyName TEXT, Reward INTEGER)", 0, 0, &zErrMsg);
	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Bounties(BountyID INTEGER, Wanted INTEGER, BountyCreator TEXT, BountyName TEXT, Reward INTEGER, PRIMARY KEY(BountyID))", 0, 0, &zErrMsg);

	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	//Create columns (for backwards compatibility with older databases)

	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Bounties WHERE COLUMN_NAME='BountyID' ) ) THEN ALTER TABLE Bounties ADD BountyID INTEGER PRIMARY KEY", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Bounties WHERE COLUMN_NAME='Wanted' ) ) THEN ALTER TABLE Bounties ADD Wanted INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Bounties WHERE COLUMN_NAME='BountyCreator' ) ) THEN ALTER TABLE Bounties ADD BountyCreator TEXT", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Bounties WHERE COLUMN_NAME='BountyName' ) ) THEN ALTER TABLE Bounties ADD BountyName TEXT", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Bounties WHERE COLUMN_NAME='Reward' ) ) THEN ALTER TABLE Bounties ADD Reward INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->Print("Done.\n");

	//Create Account Table
	trap->Print("Initializing Account Table.\n");

	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Users(LoggedIn INTEGER, Admin INTEGER, ClientID INTEGER, AdminLevel INTEGER, Password TEXT, Username TEXT, AccountID INTEGER PRIMARY KEY)", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	//Create columns (for backwards compatibility with older databases)
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Users WHERE COLUMN_NAME='LoggedIn' ) ) THEN ALTER TABLE Users ADD LoggedIn INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Users WHERE COLUMN_NAME='Admin' ) ) THEN ALTER TABLE Users ADD Admin INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Users WHERE COLUMN_NAME='ClientID' ) ) THEN ALTER TABLE Users ADD ClientID INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Users WHERE COLUMN_NAME='AdminLevel' ) ) THEN ALTER TABLE Users ADD AdminLevel INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Users WHERE COLUMN_NAME='Password' ) ) THEN ALTER TABLE Users ADD Password TEXT", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Users WHERE COLUMN_NAME='Username' ) ) THEN ALTER TABLE Users ADD Username TEXT", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Users WHERE COLUMN_NAME='AccountID' ) ) THEN ALTER TABLE Users ADD AccountID INTEGER PRIMARY KEY", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->Print("Done.\n");

	//Create Faction Table
	trap->Print("Initializing Faction Table.\n");

	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Factions( FactionID INTEGER PRIMARY KEY, Name TEXT, Bank INTEGER)", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	//Create columns (for backwards compatibility with older databases)
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Factions WHERE COLUMN_NAME='FactionID' ) ) THEN ALTER TABLE Factions ADD FactionID INTEGER PRIMARY KEY", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Factions WHERE COLUMN_NAME='Name' ) ) THEN ALTER TABLE Factions ADD Name TEXT", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Factions WHERE COLUMN_NAME='Bank' ) ) THEN ALTER TABLE Factions ADD Bank INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->Print("Done.\n");

	//Create Item Table
	trap->Print("Initializing Item Table.\n");

	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Items(Concussion INTEGER, Rocket INTEGER, Flechette INTEGER, Demp2 INTEGER, Repeater INTEGER, Bowcaster INTEGER, Disruptor INTEGER, E11 INTEGER, CharID INTEGER, Pistol INTEGER)", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	//Create columns (for backwards compatibility with older databases)
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Items WHERE COLUMN_NAME='Concussion' ) ) THEN ALTER TABLE Items ADD Concussion INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Items WHERE COLUMN_NAME='Rocket' ) ) THEN ALTER TABLE Items ADD Rocket INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Items WHERE COLUMN_NAME='Flechette' ) ) THEN ALTER TABLE Items ADD Flechette INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Items WHERE COLUMN_NAME='Demp2' ) ) THEN ALTER TABLE Items ADD Demp2 INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Items WHERE COLUMN_NAME='Repeater' ) ) THEN ALTER TABLE Items ADD Repeater INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Items WHERE COLUMN_NAME='Bowcaster' ) ) THEN ALTER TABLE Items ADD Bowcaster INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Items WHERE COLUMN_NAME='Disruptor' ) ) THEN ALTER TABLE Items ADD Disruptor INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Items WHERE COLUMN_NAME='E11' ) ) THEN ALTER TABLE Items ADD E11 INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Items WHERE COLUMN_NAME='CharID' ) ) THEN ALTER TABLE Items ADD CharID INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Items WHERE COLUMN_NAME='Pistol' ) ) THEN ALTER TABLE Items ADD Pistol INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->Print("Done.\n");

	//Create Character Table
	trap->Print("Initializing Character Table.\n");

	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Characters( TrainingSaberLocked INTEGER, CheckInventory INTEGER, SkillBuild TEXT, ModelScale INTEGER, Model TEXT, FactionRank TEXT, FeatBuild INTEGER, ForceBuild INTEGER, SkillPoints INTEGER, AccountID INTEGER, Experience INTEGER, FactionID INTEGER, CharID INTEGER PRIMARY KEY, Name TEXT, Credits INTEGER, ForceSensitive INTEGER, Level INTEGER)", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	//Create columns (for backwards compatibility with older databases)
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='TrainingSaberLocked' ) ) THEN ALTER TABLE Characters ADD TrainingSaberLocked INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='CheckInventory' ) ) THEN ALTER TABLE Characters ADD CheckInventory INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='SkillBuild' ) ) THEN ALTER TABLE Characters ADD SkillBuild TEXT", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='ModelScale' ) ) THEN ALTER TABLE Characters ADD ModelScale INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='Model' ) ) THEN ALTER TABLE Characters ADD Model TEXT", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='FactionRank' ) ) THEN ALTER TABLE Characters ADD FactionRank TEXT", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='FeatBuild' ) ) THEN ALTER TABLE Characters ADD FeatBuild INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='ForceBuild' ) ) THEN ALTER TABLE Characters ADD ForceBuild INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='SkillPoints' ) ) THEN ALTER TABLE Characters ADD SkillPoints INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='AccountID' ) ) THEN ALTER TABLE Characters ADD AccountID INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='Experience' ) ) THEN ALTER TABLE Characters ADD Experience INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='FactionID' ) ) THEN ALTER TABLE Characters ADD FactionID INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='CharID' ) ) THEN ALTER TABLE Characters ADD CharID INTEGER PRIMARY KEY", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='Name' ) ) THEN ALTER TABLE Characters ADD Name TEXT", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='Credits' ) ) THEN ALTER TABLE Characters ADD Credits INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='ForceSensitive' ) ) THEN ALTER TABLE Characters ADD ForceSensitive INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='Level' ) ) THEN ALTER TABLE Characters ADD Level INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, "IF NOT EXISTS( (SELECT * FROM Characters WHERE COLUMN_NAME='Flags' ) ) THEN ALTER TABLE CHARACTERS ADD Flags INTEGER", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->Print("Done.\n");

	trap->Print("All tables have been initialized.\n");
}

void Cmd_info_F(gentity_t *ent)
{
	return;
}
void Cmd_aminfo_F(gentity_t *ent)
{
	return;
}
void Cmd_eminfo_F(gentity_t *ent)
{
	return;
}

void Cmd_ListAdmins_F(gentity_t *ent)
{
	int i;

	if (!G_CheckAdmin(ent, ADMIN_ADMINWHOIS))
	{
		for (i = 0; i < level.maxclients; i++)
		{
			if (g_entities[i].client->sess.isAdmin && !g_entities[i].client->sess.isDisguised && g_entities[i].inuse && g_entities[i].client && g_entities[i].client->pers.connected == CON_CONNECTED)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^2Name: %s ^2Admin level: ^7%i\n\"", g_entities[i].client->pers.netname, g_entities[i].client->sess.adminLevel));
			}
		}
	}
	else
	{
		for (i = 0; i < level.maxclients; i++)
		{
			if (g_entities[i].client->sess.isAdmin && g_entities[i].inuse && g_entities[i].client && g_entities[i].client->pers.connected == CON_CONNECTED)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^2Name: %s ^2Admin level: ^7%i\n\"", g_entities[i].client->pers.netname, g_entities[i].client->sess.adminLevel));
			}
		}
	}
	return;
}

void Cmd_ToggleChat_F(gentity_t * ent)
{
	char chatModeName[MAX_STRING_CHARS];

	if (trap->Argc() < 2)
	{
		if (!ent->client->sess.chatMode || ent->client->sess.chatMode > 8)
		{
			ent->client->sess.chatMode = 1;
			ent->client->sess.chatMode++;
		}
		else if (ent->client->sess.chatMode < 6 && !ent->client->sess.isAdmin)
		{
			ent->client->sess.chatMode++;
		}
		else if (ent->client->sess.chatMode < 7 && ent->client->sess.isAdmin)
		{
			ent->client->sess.chatMode++;
		}
		else
		{
			ent->client->sess.chatMode = 1;
		}

		switch (ent->client->sess.chatMode)
		{
		case 1:
			Q_strncpyz(chatModeName, "OOC", sizeof(chatModeName));
			break;
		case 2:
			Q_strncpyz(chatModeName, "LOOC", sizeof(chatModeName));
			break;
		case 3:
			Q_strncpyz(chatModeName, "Yell", sizeof(chatModeName));
			break;
		case 4:
			Q_strncpyz(chatModeName, "Whisper", sizeof(chatModeName));
			break;
		case 5:
			Q_strncpyz(chatModeName, "Me (Action)", sizeof(chatModeName));
			break;
		case 6:
			Q_strncpyz(chatModeName, "It (Environmental Action/Event)", sizeof(chatModeName));
			break;
		case 7:
			Q_strncpyz(chatModeName, "Admin", sizeof(chatModeName));
			break;
		default:
			ent->client->sess.chatMode = 1;
			Q_strncpyz(chatModeName, "OOC", sizeof(chatModeName));
			break;
		}
		trap->SendServerCommand(ent - g_entities, va("print \"^2Your chat mode is set to ^7%s.\n\"", chatModeName));
		return;
	}

	trap->Argv(1, chatModeName, MAX_STRING_CHARS);

	if (!Q_stricmp(chatModeName, "OOC"))
	{
		ent->client->sess.chatMode = 1;
		Q_strncpyz(chatModeName, "OOC", sizeof(chatModeName));
	}
	else if (!Q_stricmp(chatModeName, "LOOC"))
	{
		ent->client->sess.chatMode = 2;
		Q_strncpyz(chatModeName, "LOOC", sizeof(chatModeName));
	}
	else if (!Q_stricmp(chatModeName, "yell") || !Q_stricmp(chatModeName, "y"))
	{
		ent->client->sess.chatMode = 3;
		Q_strncpyz(chatModeName, "Yell", sizeof(chatModeName));
	}
	else if (!Q_stricmp(chatModeName, "whisper") || !Q_stricmp(chatModeName, "w"))
	{
		ent->client->sess.chatMode = 4;
		Q_strncpyz(chatModeName, "Whisper", sizeof(chatModeName));
	}
	else if (!Q_stricmp(chatModeName, "me"))
	{
		ent->client->sess.chatMode = 5;
		Q_strncpyz(chatModeName, "Me (Action)", sizeof(chatModeName));
	}
	else if (!Q_stricmp(chatModeName, "it"))
	{
		ent->client->sess.chatMode = 6;
		Q_strncpyz(chatModeName, "It (Environmental Action/Event)", sizeof(chatModeName));
	}
	else if (!Q_stricmp(chatModeName, "admin") || !Q_stricmp(chatModeName, "a"))
	{
		if (!ent->client->sess.isAdmin)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this chat mode.\n\""));
			return;
		}
		else
		{
			ent->client->sess.chatMode = 7;
			Q_strncpyz(chatModeName, "Admin", sizeof(chatModeName));
		}
	}
	else
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1Invalid chat mode.\n\"");
		return;
	}
	trap->SendServerCommand(ent - g_entities, va("print \"^2Your chat mode is set to ^7%s.\n\"", chatModeName));
	return;
}

void Cmd_TrainingSaber_F(gentity_t *ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	char charName[256] = { 0 };
	int charID = 0, accountID = 0, clientID = -1, loggedIn = 0, i = 0;

	//The database is not connected. Please do so.
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
		if (!ent->client->sess.trainingSaber)
		{
			ent->client->sess.trainingSaber = qtrue;
			trap->SendServerCommand(ent - g_entities, "print \"^2Saber training setting ^7ON^2.\n\"");
			sqlite3_close(db);
			return;
		}
		else
		{
			if (ent->client->sess.trainingSaberLocked)
			{
				trap->SendServerCommand(ent - g_entities, "print \"^1 Your saber is locked into training mode by an admin.\n\"");
				sqlite3_close(db);
				return;
			}

			ent->client->sess.trainingSaber = qfalse;
			trap->SendServerCommand(ent - g_entities, "print \"^2Saber training setting ^7OFF^2.\n\"");
			sqlite3_close(db);
			return;
		}
	}

	if (!G_CheckAdmin(ent, ADMIN_TRAININGSABER))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, charName, sizeof(charName));

	for (i = 0; charName[i]; i++)
	{
		charName[i] = tolower(charName[i]);
	}

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

	if (!G_AdminControl(ent, &g_entities[clientID]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		sqlite3_close(db);
		return;
	}
	if (loggedIn)
	{
		if (g_entities[clientID].client->sess.trainingSaberLocked)
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1This player's saber is locked into training mode. Unlock it using /amlockSaberSetting <characterName>\n\"");
			sqlite3_close(db);
			return;
		}

		if (g_entities[clientID].client->sess.trainingSaber)
		{
			g_entities[clientID].client->sess.trainingSaber = qfalse;
			trap->SendServerCommand(ent - g_entities, va("print \"^7%s^2's lightsaber's training setting set to ^7OFF^2.\n\"", g_entities[clientID].client->pers.netname));

			trap->SendServerCommand(clientID, "print \"^2Your lightsaber's training setting has been switched ^7OFF ^2by an admin.\n\"");
			trap->SendServerCommand(clientID, "cp \"^2Your lightsaber's training setting has been switched ^7OFF ^2by an admin.\n\"");
		}
		else
		{
			g_entities[clientID].client->sess.trainingSaber = qtrue;
			trap->SendServerCommand(ent - g_entities, va("print \"^7%s^2's lightsaber's training setting set to ^7ON^2.\n\"", g_entities[clientID].client->pers.netname));

			trap->SendServerCommand(clientID, "print \"^2Your lightsaber's training setting has been switched ^7ON ^2by an admin.\n\"");
			trap->SendServerCommand(clientID, "cp \"^2Your lightsaber's training setting has been switched ^7ON ^2by an admin.\n\"");
		}
	}
	else
		trap->SendServerCommand(ent - g_entities, "print \"^1This player is not on the server. They must be in order for you to be able to use this command.\n\"");

	sqlite3_close(db);
	return;
}

void Cmd_Hologram_F(gentity_t *ent)
{
	/*
	if ( !ent->client->sess.isHologram )
	{
	ent->client->sess.isHologram = qtrue;
	trap->SendServerCommand( ent-g_entities, "hologram" );
	trap->SendServerCommand( ent-g_entities, "print \"^2Hologram of yourself ^7ON^2.\n\"" );
	}
	else
	{
	ent->client->sess.isHologram = qfalse;
	trap->SendServerCommand( ent-g_entities, "hologram" );
	trap->SendServerCommand( ent-g_entities, "print \"^2Hologram of yourself ^7OFF^2.\n\"" );
	}
	return;
	*/
	return;
}

void Cmd_AutoWalk_F(gentity_t *ent)
{
	if (ent - g_entities < MAX_CLIENTS && ent->client->ps.m_iVehicleNum)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You can't use autowalk while in a vehicle.\n\"");
		return;
	}

	if (!ent->client->sess.isAutoWalking)
		ent->client->sess.isAutoWalking = qtrue;
	else
		ent->client->sess.isAutoWalking = qfalse;
	return;
}

void Cmd_Roll_F(gentity_t *ent)
{
	char maxNumTemp[10] = { 0 };
	int maxNum = 0;

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /roll <numberOfSidesOnDice> (Example: /roll 50 results in 'Name rolls a(n) x on a 50 sided dice.')\n\"");
		return;
	}

	trap->Argv(1, maxNumTemp, sizeof(maxNumTemp));
	maxNum = atoi(maxNumTemp);

	if (maxNum < 3 || maxNum > 100)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1The dice must have at least 3 sides and no more than 100 sides.\n\"");
		return;
	}

	G_Say(ent, NULL, SAY_ME, va("^3rolls a(n) %i on a %i sided dice.", Q_irand(1, maxNum), maxNum));
	return;
}

//Stun command created by Kannos'v Lightdust. All credit goes to him for creating it. Slight modifications made in variables, comments, function name, and printed messages.
void Cmd_StunMode_F(gentity_t * ent)
{
	// Set weapons to stun!
	if (!ent->client->sess.stunMode)
	{
		ent->client->sess.stunMode = qtrue;
		trap->SendServerCommand(-1, va("chat \"^2You have ^7activated ^2stun mode.\n\"", ent->client->pers.netname));
	}
	else
	{
		ent->client->sess.stunMode = qfalse;
		trap->SendServerCommand(-1, va("chat \"^2You have ^7deactiveated ^2stun mode.\n\"", ent->client->pers.netname));
	}
	return;
}