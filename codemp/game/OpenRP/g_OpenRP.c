#include "OpenRP.h"

#include "../g_local.h"
#include "g_admin.h"
#include "g_OpenRP.h"

void InitializeSQL(void)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	sqlite3_stmt *stmt;
	int rc;
	qboolean columnFound = qfalse;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	//Create Account Table
	trap->Print("Initializing Account table.\n");

	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS 'Accounts' ('AccountID' INTEGER, 'Admin' INTEGER, 'AdminLevel' INTEGER, 'ClientID' INTEGER, 'LoggedIn' INTEGER, 'Password' TEXT, 'Username' TEXT, PRIMARY KEY(AccountID))", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	//Create columns (for backwards compatibility with older databases)
	trap->Print("Initializing AccountID column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Accounts')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "AccountID"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Accounts ADD AccountID INTEGER PRIMARY KEY", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Admin column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Accounts')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Admin"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Accounts ADD Admin INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;


	trap->Print("Initializing AdminLevel column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Accounts')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "AdminLevel"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Accounts ADD AdminLevel INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing ClientID column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Accounts')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "ClientID"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Accounts ADD ClientID INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing LoggedIn column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Accounts')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "LoggedIn"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Accounts ADD LoggedIn INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Password column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Accounts')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Password"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Accounts ADD Password TEXT", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Username column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Accounts')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Username"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Accounts ADD Username TEXT", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Ensuring that all LoggedIn values are set to 0.\n");

	rc = sqlite3_exec(db, "UPDATE Accounts set LoggedIn='0' WHERE LoggedIn='1'", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->Print("Done with Account table.\n");

	//Create Bounty Table
	trap->Print("Initializing Bounties table.\n");
	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS 'Bounties' ('BountyCreator' TEXT, 'BountyID' INTEGER, 'BountyName' TEXT, 'Reward' INTEGER, 'Wanted' INTEGER, PRIMARY KEY(BountyID))", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	//Create columns (for backwards compatibility with older databases)
	trap->Print("Initializing BountyCreator column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Bounties')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "BountyCreator"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Bounties ADD BountyCreator TEXT", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing BountyID column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Bounties')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "BountyID"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Bounties ADD BountyID INTEGER PRIMARY KEY", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing BountyName column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Bounties')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "BountyName"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Bounties ADD BountyName TEXT", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Reward column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Bounties')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Reward"))
			columnFound = qtrue;

	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Bounties ADD Reward INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Wanted column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Bounties')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Wanted"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Bounties ADD Wanted INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Done with Bounty table.\n");

	//Create Character Table
	trap->Print("Initializing Character Table.\n");

	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS 'Characters' ('AccountID' INTEGER, 'CharID' INTEGER, 'CheckInventory' INTEGER, 'Credits' INTEGER, 'Experience' INTEGER, 'FactionID' INTEGER, 'FactionRank' TEXT, 'Flags' INTEGER, 'ForcePowers' INTEGER, 'ForceSensitive' INTEGER, 'Level' INTEGER, 'Model' TEXT, 'ModelScale' INTEGER, 'Name' TEXT, 'SkillPoints' INTEGER, 'TrainingSaberLocked' INTEGER,	PRIMARY KEY(CharID))", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	//Create columns (for backwards compatibility with older databases)
	trap->Print("Initializing AccountID column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "AccountID"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD AccountID INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing CharID column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "CharID"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD CharID INTEGER PRIMARY KEY", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing CheckInventory column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "CheckInventory"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD CheckInventory INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Credits column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Credits"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD Credits INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Experience column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Experience"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD Experience INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing FactionID column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "FactionID"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD FactionID INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing FactionRank column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "FactionRank"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD FactionRank TEXT", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Flags column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Flags"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD Flags INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing ForcePowers column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "ForcePowers"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD ForcePowers INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing ForceSensitive column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "ForceSensitive"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD ForceSensitive INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Level column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Level"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD Level INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Model column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Model"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD Model TEXT", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing ModelScale column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "ModelScale"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD ModelScale INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Name column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Name"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD Name TEXT", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing SkillPoints column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "SkillPoints"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD SkillPoints INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing TrainingSaberLocked column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Characters')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "TrainingSaberLocked"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Characters ADD TrainingSaberLocked INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Done with Character table.\n");

	//Create Faction Table
	trap->Print("Initializing Faction Table.\n");


	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS 'Factions' ('Bank' INTEGER, 'FactionID' INTEGER, 'Name' TEXT, PRIMARY KEY(FactionID))", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->Print("Initializing Bank column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Factions')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Bank"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Factions ADD Bank INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	//Create columns (for backwards compatibility with older databases)
	trap->Print("Initializing FactionID column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Factions')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "FactionID"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Factions ADD FactionID INTEGER PRIMARY KEY", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Name column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Factions')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Name"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Factions ADD Name TEXT", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Done with Faction table.\n");

	//Create Item Table
	trap->Print("Initializing Items Table.\n");

	rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS 'Items' ('Bowcaster' INTEGER, 'CharID' TEXT, 'Concussion' INTEGER, 'Demp2' INTEGER, 'Disruptor' INTEGER, 'E11' INTEGER, 'Flechette' INTEGER, 'Pistol' INTEGER, 'Repeater' INTEGER, 'Rocket' INTEGER)", 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	//Create columns (for backwards compatibility with older databases)
	trap->Print("Initializing Bowcaster column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Items')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Bowcaster"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Items ADD Bowcaster INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing CharID column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Items')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "CharID"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Items ADD CharID INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Concussion column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Items')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Concussion"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Items ADD Concussion INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Demp2 column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Items')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Demp2"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Items ADD Demp2 INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Disruptor column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Items')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Disruptor"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Items ADD Disruptor INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing E11 column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Items')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "E11"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Items ADD E11 INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Flechette column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Items')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Flechette"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Items ADD Flechette INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Pistol column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Items')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Pistol"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Items ADD Pistol INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Repeater column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Items')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Repeater"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Items ADD Repeater INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Initializing Rocket column.\n");
	rc = sqlite3_prepare_v2(db, "PRAGMA table_info('Items')", -1, &stmt, NULL);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		return;
	}
	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		if (!Q_stricmp(sqlite3_column_text(stmt, 1), "Rocket"))
			columnFound = qtrue;
	}
	if (!columnFound)
	{
		rc = sqlite3_exec(db, "ALTER TABLE Items ADD Rocket INTEGER", 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
	if (columnFound)
		columnFound = qfalse;

	trap->Print("Done with Items table.\n");

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
	gentity_t	*client;

	if (!G_CheckAdmin(ent, ADMIN_ADMINWHOIS))
	{
		for (i = 0; i < level.maxclients; i++)
		{
			client = g_entities + i;
			if (level.clients[i].sess.isAdmin && !level.clients[i].sess.isDisguised && client->inuse && client->client && level.clients[i].pers.connected == CON_CONNECTED)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^2Name: %s ^2Admin level: ^7%i\n\"", level.clients[i].pers.netname, level.clients[i].sess.adminLevel));
			}
		}
	}
	else
	{
		for (i = 0; i < level.maxclients; i++)
		{
			client = g_entities + i;
			if (level.clients[i].sess.isAdmin && client->inuse && client->client && level.clients[i].pers.connected == CON_CONNECTED)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^2Name: %s ^2Admin level: ^7%i\n\"", level.clients[i].pers.netname, level.clients[i].sess.adminLevel));
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

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
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
	rc = sqlite3_prepare_v2(db, va("SELECT ClientID FROM Accounts WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
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

	rc = sqlite3_prepare_v2(db, va("SELECT LoggedIn FROM Accounts WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
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
		trap->SendServerCommand(ent-g_entities, "print \"^2You have ^7activated ^2stun mode.\n\"");
	}
	else
	{
		ent->client->sess.stunMode = qfalse;
		trap->SendServerCommand(ent-g_entities, "print \"^2You have ^7deactiveated ^2stun mode.\n\"");
	}
	return;
}

//Credit to ClanMod for NPC Order command and related code
void Cmd_OrderNPC_f(gentity_t *ent)
{
	NPCORDER_FUNC	*order;
	char			buffer[MAX_TOKEN_CHARS];
	qboolean		applyOrder[MAX_FOLLOWERS];
	int				i, index = -1;
	qboolean		found = qfalse;

	if (trap->Argc() < 2)
	{
		orderNPC_t *type;
		trap->SendServerCommand(ent - g_entities, "print \"usage: ordernpc *ordertype* [ *npcclass or npcname* [ *number* ] ]\npossible values for *ordertype* are :\n\"");
		type = orderNPCTable;
		while (type->order)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"'%s' - %s\n\"", type->name, type->description));
			type++;
		}
		return;
	}

	if (!ent->client->numPlFollowers)
	{
		trap->SendServerCommand(ent - g_entities, "print \"you don't have any follower!\n\"");
		return;
	}

	trap->Argv(1, buffer, sizeof(buffer));
	order = NPCF_GetOrderForName(buffer);
	if (!order)
	{
		trap->SendServerCommand(ent - g_entities, "print \"invalid order!\n\"");
		return;
	}

	//pour tous
	if (trap->Argc() == 2)
	{
		NPCF_OrderToAll(ent, order);
		return;
	}

	memset(buffer, 0, sizeof(buffer));
	trap->Argv(2, buffer, sizeof(buffer));

	for (i = 0; i < ent->client->numPlFollowers; i++)
	{
		gentity_t *fwr = ent->client->plFollower[i];

		applyOrder[i] = qfalse;

		if (!Q_stricmp(buffer, fwr->targetname))
		{
			NPCF_Order(ent, fwr, order, qtrue);
			return;
		}

		//if ( !Q_wildmat( fwr->NPC_type, buffer ) )
		//{
		applyOrder[i] = qtrue;
		found = qtrue;
		//}
	}

	if (!found)
	{
		trap->SendServerCommand(ent - g_entities, "print \"NPC not recognized!\n\"");
		return;
	}

	if (trap->Argc() > 3)
	{
		memset(buffer, 0, sizeof(buffer));
		trap->Argv(3, buffer, sizeof(buffer));
		index = atoi(buffer);

		if (index < 0)
			index = 0;
	}

	if (index == -1)
	{
		for (i = 0; i < ent->client->numPlFollowers; i++)
		{
			gentity_t *fwr = ent->client->plFollower[i];

			if (!applyOrder[i])
				continue;

			if (index == -1)
			{
				NPCF_Order(ent, fwr, order, qtrue);
				index = 0;
			}
			else
			{
				NPCF_Order(ent, fwr, order, qfalse);
			}
		}
	}
	else
	{
		for (i = 0; i < ent->client->numPlFollowers; i++)
		{
			gentity_t *fwr = ent->client->plFollower[i];

			if (!applyOrder[i])
				continue;

			if (!index)
			{
				NPCF_Order(ent, fwr, order, qtrue);
				break;
			}

			index--;
		}
	}

	if (index > 0)
	{
		trap->SendServerCommand(ent - g_entities, "print \"Bad NPC Index!\n\"");
	}
}