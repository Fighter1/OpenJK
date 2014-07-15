#include "OpenRP.h"

#include "../g_local.h"
#include "g_account.h"
#include "g_admin.h"

void CheckAdmin(gentity_t * ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	int isAdmin = 0;
	int adminLevel = 11;

	if (!ent->client->sess.loggedIn)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are not logged in.\n\"");
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

	rc = sqlite3_prepare_v2(db, va("SELECT Admin FROM Accounts WHERE AccountID='%i'", ent->client->sess.accountID), -1, &stmt, NULL);
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
		isAdmin = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (isAdmin)
	{//The user is an admin.
		//Check their adminlevel
		rc = sqlite3_prepare_v2(db, va("SELECT AdminLevel FROM Accounts WHERE AccountID='%i'", ent->client->sess.accountID), -1, &stmt, NULL);
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
			adminLevel = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		// ^ this was outside the if condition before, how silly.
		if (adminLevel > 0 || adminLevel < 11)
		{
			ent->client->sess.isAdmin = qtrue;
			ent->client->sess.adminLevel = adminLevel;

			if (!ent->client->sess.canUseCheatCmds && G_CheckAdmin(ent, ADMIN_CHEATS))
				ent->client->sess.canUseCheatCmds = qtrue;
		}
		else
		{
			ent->client->sess.adminLevel = 11;
		}
	}

	//If they're not an admin, make them an admin level 11, which isn't really an admin level, and it's below all other levels.
	//This is for use with functions like G_AdminControl()
	else
		ent->client->sess.adminLevel = 11;

	sqlite3_close(db);
	return;
}

void Cmd_Login_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char username[256] = { 0 }, password[256] = { 0 }, comparisonUsername[256] = { 0 }, comparisonPassword[256] = { 0 };
	int accountID = 0, i = 0;

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
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /login <username> <password>\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^2Command Usage: /login <username> <password>\n\"");
		sqlite3_close(db);
		return;
	}

	if (ent->client->sess.loggedIn)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You are already logged in to your account.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1You are already logged in to your account.\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, username, sizeof(username));
	trap->Argv(2, password, sizeof(password));

	Q_StripColor(username);
	Q_strlwr(username);

	rc = sqlite3_prepare_v2(db, va("SELECT Username FROM Accounts WHERE Username='%s'", username), -1, &stmt, NULL);
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
		Q_strncpyz(comparisonUsername, (const char *)sqlite3_column_text(stmt, 0), sizeof(comparisonUsername));
		sqlite3_finalize(stmt);
	}

	if (comparisonUsername[0] == '\0')
	{
		//The account does not exist, thus, the error does.
		trap->SendServerCommand(ent - g_entities, va("print \"^1An account with the username %s does not exist.\n\"", username));
		trap->SendServerCommand(ent - g_entities, va("cp \"^1An account with the username %s does not exist.\n\"", username));
		trap->SendServerCommand(ent - g_entities, "loginFailed");
		trap->SendServerCommand(ent - g_entities, "accountui");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Password FROM Accounts WHERE Username='%s'", username), -1, &stmt, NULL);
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
		Q_strncpyz(comparisonPassword, (const char*)sqlite3_column_text(stmt, 0), sizeof(comparisonPassword));
		sqlite3_finalize(stmt);
	}

	if (strcmp(comparisonPassword, password))
	{
		//Just as there is an incorrect password (and an error), does it tell you.
		trap->SendServerCommand(ent - g_entities, "print \"^1Incorrect password.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1Incorrect password.\n\"");
		trap->SendServerCommand(ent - g_entities, "loginFailed");
		trap->SendServerCommand(ent - g_entities, "accountui");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT AccountID FROM Accounts WHERE Username='%s'", username), -1, &stmt, NULL);
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

	ent->client->sess.accountID = accountID;

	rc = sqlite3_exec(db, va("UPDATE Accounts set ClientID='%i' WHERE AccountID='%i'", ent - g_entities, accountID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("UPDATE Accounts set LoggedIn='1' WHERE AccountID='%i'", accountID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	ent->client->sess.loggedIn = qtrue;

	CheckAdmin(ent);

	trap->SendServerCommand(ent - g_entities, va("loggedIn"));

	//Open the character selection/creation menu
	trap->SendServerCommand(ent - g_entities, "charui");

	sqlite3_close(db);
	return;
}

void Cmd_CheckAdminDB_F(gentity_t *ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	int isAdmin = 0;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Admin FROM Accounts WHERE AccountID='%i'", ent->client->sess.accountID), -1, &stmt, NULL);
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
		isAdmin = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	//Ensure the admin db values are set correctly
	if (isAdmin != 1)
	{
		rc = sqlite3_exec(db, va("UPDATE Accounts set Admin='0' WHERE AccountID='%i'", ent->client->sess.accountID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_exec(db, va("UPDATE Accounts set AdminLevel='11' WHERE AccountID='%i'", ent->client->sess.accountID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
	}
}

void Cmd_Logout_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;

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
		trap->SendServerCommand(ent - g_entities, "print \"^1You are not logged in, so you can't logout.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1You are not logged in, so you can't logout.\n\"");
		sqlite3_close(db);
		return;
	}

	//If they have a character selected, safely deselect their character and logout.
	if (ent->client->sess.characterSelected)
	{
		rc = sqlite3_exec(db, va("UPDATE Accounts set ClientID='33' WHERE AccountID='%i'", ent->client->sess.accountID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_exec(db, va("UPDATE Accounts set LoggedIn='0' WHERE AccountID='%i'", ent->client->sess.accountID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		ent->client->sess.isAdmin = qfalse;
		ent->client->sess.adminLevel = 11;
		ent->client->sess.loggedIn = qfalse;
		ent->client->sess.accountID = 0;

		ent->client->sess.skillPoints = 1;

		ent->client->sess.characterSelected = qfalse;
		ent->client->sess.characterID = 0;

		ent->client->sess.trainingSaberLocked = qfalse;
		ent->client->sess.trainingSaber = qfalse;

		ent->client->ps.iModelScale = ent->client->sess.modelScale = 100;

		SetTeam(ent, "s");

		trap->SendServerCommand(ent - g_entities, "print \"^2You've been logged out.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^2You've been logged out.\n\"");

		trap->SendServerCommand(ent - g_entities, va("loggedOut"));
	}
	else
	{
		rc = sqlite3_exec(db, va("UPDATE Accounts set ClientID='33' WHERE AccountID='%i'", ent->client->sess.accountID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_exec(db, va("UPDATE Accounts set LoggedIn='0' WHERE AccountID='%i'", ent->client->sess.accountID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		ent->client->sess.loggedIn = qfalse;
		ent->client->sess.accountID = 0;

		SetTeam(ent, "s");

		trap->SendServerCommand(ent - g_entities, "print \"^2You've been logged out.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^2You've been logged out.\n\"");

		trap->SendServerCommand(ent - g_entities, va("loggedOut"));
	}
	sqlite3_close(db);
	return;
}

void Cmd_Register_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char username[256] = { 0 }, password[256] = { 0 }, comparisonName[256] = { 0 };
	int accountID = 0, i = 0;

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
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /register <username> <password>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, username, sizeof(username));
	trap->Argv(2, password, sizeof(password));

	Q_StripColor(username);
	Q_strlwr(username);

	rc = sqlite3_prepare_v2(db, va("SELECT Username FROM Accounts WHERE Username='%s'", username), -1, &stmt, NULL);
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
		Q_strncpyz(comparisonName, (const char *)sqlite3_column_text(stmt, 0), sizeof(comparisonName));
		sqlite3_finalize(stmt);
	}

	if (comparisonName[0] != '\0')
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1Username ^7%s ^1is already in use.\n\"", comparisonName));
		trap->SendServerCommand(ent - g_entities, va("cp \"^1Username ^7%s ^1is already in use.\n\"", comparisonName));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("INSERT INTO Accounts(Username,Password,ClientID,Admin,AdminLevel,LoggedIn) VALUES('%s','%s','33','0','11','0')", username, password), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT AccountID FROM Accounts WHERE Username='%s'", username), -1, &stmt, NULL);
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

	rc = sqlite3_exec(db, va("UPDATE Accounts set ClientID='%i' WHERE AccountID='%i'", ent - g_entities, accountID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("UPDATE Accounts set LoggedIn='1' WHERE AccountID='%i'", accountID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	ent->client->sess.accountID = accountID;
	ent->client->sess.loggedIn = qtrue;

	trap->SendServerCommand(ent - g_entities, va("loggedIn"));

	trap->SendServerCommand(ent - g_entities, "print \"^2Your account has been successfully created and you are now logged in.\n\"");
	trap->SendServerCommand(ent - g_entities, "cp \"^2Your account has been successfully created and you are now logged in.\n\"");

	//Open the character selection/creation menu
	trap->SendServerCommand(ent - g_entities, "charui");

	sqlite3_close(db);
	return;
}

void Cmd_AccountInfo_F(gentity_t * ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	char accountName[256] = { 0 }, adminText[8] = { 0 };
	int admin = 0, adminLevel = 11, accountID = 0;

	if (!ent->client->sess.loggedIn)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in, in order to view your account info.\n\"");
		trap->SendServerCommand(ent - g_entities, "cp \"^1You must be logged in, in order to view your account info.\n\"");
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
		rc = sqlite3_prepare_v2(db, va("SELECT Username FROM Accounts WHERE AccountID='%i'", ent->client->sess.accountID), -1, &stmt, NULL);
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
			Q_strncpyz(accountName, (const char*)sqlite3_column_text(stmt, 0), sizeof(accountName));
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Admin FROM Accounts WHERE AccountID='%i'", ent->client->sess.accountID), -1, &stmt, NULL);
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
			admin = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT AdminLevel FROM Accounts WHERE AccountID='%i'", ent->client->sess.accountID), -1, &stmt, NULL);
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
			adminLevel = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		switch (admin)
		{
		case 0:
			Q_strncpyz(adminText, "No", sizeof(adminText));
			break;
		case 1:
			Q_strncpyz(adminText, "Yes", sizeof(adminText));
			break;
		default:
			Q_strncpyz(adminText, "Unknown", sizeof(adminText));
			break;
		}

		if (adminLevel < 11)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^2Account Info:\nAccount Name: ^7%s\n^2Account ID: ^7%i\n^2Admin: ^7%s\n^2Admin Level: ^7%i\n\"",
				accountName, ent->client->sess.accountID, adminText, adminLevel));
		}
		else
			trap->SendServerCommand(ent - g_entities, va("print \"^2Account Info:\nAccount Name: ^7%s\n^2Account ID: ^7%i\n^2Admin: ^7%s\n\"", accountName, ent->client->sess.accountID, adminText));

		sqlite3_close(db);
		return;
	}

	else
	{
		if (!G_CheckAdmin(ent, ADMIN_SEARCH))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
			sqlite3_close(db);
			return;
		}

		trap->Argv(1, accountName, sizeof(accountName));

		rc = sqlite3_prepare_v2(db, va("SELECT AccountID FROM Accounts WHERE Username='%s'", accountName), -1, &stmt, NULL);
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

		if (!accountID)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1Account with username %s does not exist.\n\"", accountName));
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Admin FROM Accounts WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
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
			admin = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		rc = sqlite3_prepare_v2(db, va("SELECT AdminLevel FROM Accounts WHERE AccountID='%i'", accountID), -1, &stmt, NULL);
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
			adminLevel = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
		}

		switch (admin)
		{
		case 0:
			Q_strncpyz(adminText, "No", sizeof(adminText));
			break;
		case 1:
			Q_strncpyz(adminText, "Yes", sizeof(adminText));
			break;
		default:
			Q_strncpyz(adminText, "Unknown", sizeof(adminText));
			break;
		}

		if (adminLevel < 11)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^2Account Info:\nAccount Name: ^7%s\n^2Account ID: ^7%i\n^2Admin: ^7%s\n^2Admin Level: ^7%i\n\"", accountName, accountID, adminText, adminLevel));
		}
		else
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^2Account Info:\nAccount Name: ^7%s\n^2Account ID: ^7%i\n^2Admin: ^7%s\n\"", accountName, accountID, adminText));
		}
		sqlite3_close(db);
		return;
	}
}

void Cmd_EditAccount_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	sqlite3_stmt *stmt;
	int rc;
	char parameter[9] = { 0 }, change[256] = { 0 }, comparisonName[256] = { 0 };

	if (!ent->client->sess.loggedIn)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in to edit your account.\n\"");
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

	if (trap->Argc() != 3)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /editaccount <username/password> <value> \n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, parameter, sizeof(parameter));
	trap->Argv(2, change, sizeof(change));

	if (!Q_stricmp(parameter, "username"))
	{
		Q_StripColor(change);
		Q_strlwr(change);

		rc = sqlite3_prepare_v2(db, va("SELECT Username FROM Accounts WHERE Username='%s'", change), -1, &stmt, NULL);
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
			Q_strncpyz(comparisonName, (const char *)sqlite3_column_text(stmt, 0), sizeof(comparisonName));
			sqlite3_finalize(stmt);
		}

		if (comparisonName[0] != '\0')
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1The username %s is taken.\n\"", comparisonName));
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_exec(db, va("UPDATE Accounts set Username='%s' WHERE AccountID= '%i'", change, ent->client->sess.accountID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
		trap->SendServerCommand(ent - g_entities, va("print \"^2Your username has been changed to ^7%s ^2If you had colors in the name, they were removed.\n\"", change));
		sqlite3_close(db);
		return;
	}
	else if (!Q_stricmp(parameter, "password"))
	{
		rc = sqlite3_exec(db, va("UPDATE Accounts set Password='%s' WHERE AccountID='%i'", change, ent->client->sess.accountID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
		trap->SendServerCommand(ent - g_entities, "print \"^2Your password has been changed.\n\"");
		sqlite3_close(db);
		return;
	}
	else
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /editaccount <username/password> <value>\n\"");
		sqlite3_close(db);
		return;
	}
}

void Cmd_AccountName_F(gentity_t * ent)
{
	sqlite3 *db;
	int rc;
	sqlite3_stmt *stmt;
	char cmdTarget[256] = { 0 }, username[256] = { 0 };
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
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /accountName <name/clientid>\n\""));
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, cmdTarget, sizeof(cmdTarget));

	clientid = G_ClientNumberFromName(cmdTarget);
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

	if (&g_entities[clientid].client->sess.loggedIn)
	{
		rc = sqlite3_prepare_v2(db, va("SELECT Username FROM Accounts WHERE AccountID='%i'", g_entities[clientid].client->sess.accountID), -1, &stmt, NULL);
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
			Q_strncpyz(username, (const char*)sqlite3_column_text(stmt, 0), sizeof(username));
			sqlite3_finalize(stmt);
		}

		trap->SendServerCommand(ent - g_entities, va("print \"^2Account Name: ^7%s\n\"", username));
		sqlite3_close(db);
		return;
	}

	else
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^7% ^1is not logged in.\n\"", g_entities[clientid].client->pers.netname));
	}
	sqlite3_close(db);
	return;
}
