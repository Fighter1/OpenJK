#include "OpenRP.h"

#include "../g_local.h"
#include "g_datapad.h"


void M_Send_F( gentity_t *ent )
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char charName[256] = { 0 }, *message[1024] = { 0 }; //pointer for magic
	int charID = 0, accountID = 0, clientID = 33, loggedIn = 0;

	rc = sqlite3_open( (const char*)openrp_databasePath.string, &db );
	if( rc )
	{
		trap->Print( "Can't open database: %s\n", sqlite3_errmsg( db ) );
		trap->SendServerCommand( ent-g_entities, "print \"^1The server's database is not connected.\n\"" );
		sqlite3_close( db );
		return;
	}

	trap->Argv( 1, charName, sizeof( charName ) );
	Q_CleanStr( charName ); // I like my strings white like my bread

	Q_strlwr( charName );

	rc = sqlite3_prepare_v2( db, va( "SELECT CharID FROM Characters WHERE Name='%s'", charName ), -1, &stmt, NULL) ;
	if( rc != SQLITE_OK )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize(stmt);
		sqlite3_close( db );
		return;
	}
	rc = sqlite3_step( stmt );
	if ( rc != SQLITE_ROW && rc != SQLITE_DONE )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize( stmt );
		sqlite3_close( db );
		return;
	}
	if ( rc == SQLITE_ROW)
	{
		charID = sqlite3_column_int( stmt, 0 );
		sqlite3_finalize( stmt );
	}

	if( !charID )
	{
		trap->SendServerCommand( ent-g_entities, va( "print \"^1Character %s does not exist.\n\"", charName ) );
		sqlite3_close( db );
		return;
	}

	//Get their accountID
	rc = sqlite3_prepare_v2( db, va( "SELECT AccountID FROM Characters WHERE CharID='%i'", charID ), -1, &stmt, NULL) ;
	if( rc != SQLITE_OK )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize(stmt);
		sqlite3_close( db );
		return;
	}
	rc = sqlite3_step( stmt );
	if ( rc != SQLITE_ROW && rc != SQLITE_DONE )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize( stmt );
		sqlite3_close( db );
		return;
	}
	if ( rc == SQLITE_ROW)
	{
		accountID = sqlite3_column_int( stmt, 0 );
		sqlite3_finalize( stmt );
	}
	//Get their clientID so we can send them messages
	rc = sqlite3_prepare_v2( db, va( "SELECT ClientID FROM Users WHERE AccountID='%i'", accountID ), -1, &stmt, NULL) ;
	if( rc != SQLITE_OK )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize(stmt);
		sqlite3_close( db );
		return;
	}
	rc = sqlite3_step( stmt );
	if ( rc != SQLITE_ROW && rc != SQLITE_DONE )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize( stmt );
		sqlite3_close( db );
		return;
	}
	if ( rc == SQLITE_ROW)
	{
		clientID = sqlite3_column_int( stmt, 0 );
		sqlite3_finalize( stmt );
	}
	rc = sqlite3_prepare_v2( db, va( "SELECT LoggedIn FROM Users WHERE AccountID='%i'", accountID ), -1, &stmt, NULL) ;
	if( rc != SQLITE_OK )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize(stmt);
		sqlite3_close( db );
		return;
	}
	rc = sqlite3_step( stmt );
	if ( rc != SQLITE_ROW && rc != SQLITE_DONE )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize( stmt );
		sqlite3_close( db );
		return;
	}
	if ( rc == SQLITE_ROW)
	{
		loggedIn = sqlite3_column_int( stmt, 0 );
		sqlite3_finalize( stmt );
	}

	rc = sqlite3_prepare_v2( db, va( "SELECT Name FROM Characters WHERE Name='%s'", charName ), -1, &stmt, NULL) ;
	if( rc != SQLITE_OK )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize(stmt);
		sqlite3_close( db );
		return;
	}
	rc = sqlite3_step( stmt );
	if ( rc != SQLITE_ROW && rc != SQLITE_DONE )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize( stmt );
		sqlite3_close( db );
		return;
	}
	if ( rc == SQLITE_ROW)
	{
		Q_strncpyz( charName, (const char*)sqlite3_column_text( stmt, 0 ), sizeof( charName ) );
		sqlite3_finalize( stmt );
	}

	//fixme
	//trap->Cmd_Args( *message, sizeof(message) );
	// "mail send" 9 characters, + name + 1 for space at start of args = 10
	*message += 9; // Cut off 10 characters, this is what the pointer was for =p
	*message += strlen( charName ) + 1; // Cutting off the name

	//OPENRPTODO: date & time
	rc = sqlite3_exec( db, va( "INSERT INTO Mail(Sender, Recipient, Message) VALUES('%i','%i', '%s', '%i')", ent->client->sess.characterID, charID, *message, 0 ), 0, 0, &zErrMsg );
	if( rc != SQLITE_OK )
	{
		trap->Print( "SQL error: %s\n", zErrMsg );
		sqlite3_free( zErrMsg );
		sqlite3_close( db );
		return;
	}

	sqlite3_close( db );
	return;
}


void M_Read_F( gentity_t *ent )
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	int mailID = 0, recipientCharID = 0;
	char message[1024] = { 0 }, mailIDTemp[4] = { 0 };

	rc = sqlite3_open( (const char*)openrp_databasePath.string, &db );
	if( rc )
	{
		trap->Print( "Can't open database: %s\n", sqlite3_errmsg( db ) );
		trap->SendServerCommand( ent-g_entities, "print \"^1The server's database is not connected.\n\"" );
		sqlite3_close( db );
		return;
	}

	trap->Argv( 1, mailIDTemp, sizeof( mailIDTemp) );
	mailID = atoi( mailIDTemp );

	rc = sqlite3_prepare_v2( db, va( "SELECT Message FROM Mail WHERE MailID='%i'", mailID ), -1, &stmt, NULL) ;
	if( rc != SQLITE_OK )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize(stmt);
		sqlite3_close( db );
		return;
	}
	rc = sqlite3_step( stmt );
	if ( rc != SQLITE_ROW && rc != SQLITE_DONE )
	{
		trap->Print( "SQL error: %s\n", sqlite3_errmsg( db ) );
		sqlite3_finalize( stmt );
		sqlite3_close( db );
		return;
	}
	if ( rc == SQLITE_ROW)
	{
		Q_strncpyz( message, (const char*)sqlite3_column_text( stmt, 0 ), sizeof( message ) );
		sqlite3_finalize( stmt );
	}

	if(message[0] == '\0')
	{
		trap->SendServerCommand( ent-g_entities, va( "print \"^1There is no message with the mailID ^7%i^1.\n\"", mailID ) );
		return; //break out of the function prematurely
	}// help this dont works

	trap->SendServerCommand( ent-g_entities, va("print \"^2Reading mailID: ^7%s\n%s\n^2End of message.\"", mailID, message ));
}

void Cmd_Mail_F( gentity_t *ent ) // Sort mail cmd and carry it to respective funcs, because FUNCS ERRYWHERE
{
char arg[512] = { 0 };
trap->Argv( 1, arg, sizeof(arg) );

	if (Q_stricmp(arg, "send") )
		M_Send_F(ent);
	else if(Q_stricmp(arg, "read") )
		M_Read_F(ent);
//	else if(Q_stricmp(arg, "delete") )
//		M_Delete_F(ent);
//	else if(trap->Argc() <= 1)
//		M_List_F(ent);

}
