#include "OpenRP.h"

#include "../g_local.h"
#include "g_admin.h"
#include "g_character.h"

extern void Admin_Teleport(gentity_t *ent);

qboolean G_CheckAdmin(gentity_t *ent, int bitvalue)
{
	int Bitvalues = 0;

	if (!(bitvalue & ADMIN_CHEATS))
		CheckAdmin(ent);

	if (!ent->client->sess.isAdmin)
		return qfalse;

	//Right they are admin so lets check what sort so we can assign bitvalues
	if (ent->client->sess.adminLevel == 1)
		Bitvalues = openrp_admin1Bitvalues.integer;

	else if (ent->client->sess.adminLevel == 2)
		Bitvalues = openrp_admin2Bitvalues.integer;

	else if (ent->client->sess.adminLevel == 3)
		Bitvalues = openrp_admin3Bitvalues.integer;

	else if (ent->client->sess.adminLevel == 4)
		Bitvalues = openrp_admin4Bitvalues.integer;

	else if (ent->client->sess.adminLevel == 5)
		Bitvalues = openrp_admin5Bitvalues.integer;

	else if (ent->client->sess.adminLevel == 6)
		Bitvalues = openrp_admin6Bitvalues.integer;

	else if (ent->client->sess.adminLevel == 7)
		Bitvalues = openrp_admin7Bitvalues.integer;

	else if (ent->client->sess.adminLevel == 8)
		Bitvalues = openrp_admin8Bitvalues.integer;

	else if (ent->client->sess.adminLevel == 9)
		Bitvalues = openrp_admin9Bitvalues.integer;

	else if (ent->client->sess.adminLevel == 10)
		Bitvalues = openrp_admin10Bitvalues.integer;

	//If bitvalues = 0, they don't have access to any admin command
	if (!Bitvalues)
		return qfalse;

	if (Bitvalues & bitvalue)
		return qtrue;
	else
		return qfalse;
}

qboolean G_AdminControl(gentity_t *ent, gentity_t *target)
{
	if (!openrp_adminControl.integer)
		return qtrue;

	if (!target->client->sess.isAdmin)
		return qtrue;

	//Less than is used instead of greater than because admin level 1 is higher than admin level 2
	if (openrp_adminControl.integer && (ent->client->sess.adminLevel <= target->client->sess.adminLevel))
		return qtrue;
	else
		return qfalse;
}

void Cmd_amBan_F(gentity_t *ent)
{
	char cmdTarget[256];
	int clientid = -1;

	if (!G_CheckAdmin(ent, ADMIN_BAN))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amban <name/clientid>\n\""));
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

	if (ent - g_entities == clientid)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2You can't ban yourself.\n\""));
		return;
	}

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	if (!(g_entities[clientid].r.svFlags & SVF_BOT))
	{
		AddIP(g_entities[clientid].client->sess.IP);
		trap->SendServerCommand(ent - g_entities, va("print \"^2The IP of the person you banned is ^7%s\n\"", g_entities[clientid].client->sess.IP));
	}
	trap->DropClient(clientid, "^1was ^1permanently ^1banned.\n");

	G_LogPrintf("Ban admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	return;
}

void Cmd_amKick_F(gentity_t *ent)
{
	char cmdTarget[256];
	int clientid = -1;

	if (!G_CheckAdmin(ent, ADMIN_KICK))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amkick <name/clientid>\n\""));
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

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^2The IP of the person you kicked is %s\n\"", g_entities[clientid].client->sess.IP));
	trap->DropClient(clientid, "^1was ^1kicked.");
	G_LogPrintf("Kick admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	return;
}

void Cmd_amWarn_F(gentity_t *ent)
{
	char cmdTarget[256];
	int clientid = -1;

	if (!G_CheckAdmin(ent, ADMIN_WARN))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amwarn <name/clientid>\n\""));
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

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	g_entities[clientid].client->sess.warnings++;

	trap->SendServerCommand(ent - g_entities, va("print \"^2Player %s ^2was warned.\nThey have ^7%i/%i ^1warnings.\n\"", g_entities[clientid].client->pers.netname, g_entities[clientid].client->sess.warnings, openrp_maxWarnings.integer));

	trap->SendServerCommand(clientid, va("print \"^1You have been warned by an admin.\nYou have ^7%i/%i ^1warnings.\n\"", g_entities[clientid].client->sess.warnings, openrp_maxWarnings.integer));
	trap->SendServerCommand(clientid, va("cp \"^1You have been warned by an admin.\nYou have ^7%i/%i ^1warnings.\n\"", g_entities[clientid].client->sess.warnings, openrp_maxWarnings.integer));
	G_LogPrintf("Warn admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);

	if (g_entities[clientid].client->sess.warnings == atoi(openrp_maxWarnings.string))
	{
		trap->DropClient(clientid, "^1was ^1kicked ^1- ^1received ^1maximum ^1number ^1of ^1warnings ^1from ^1admins.\n");
		G_LogPrintf("%s was kicked - received maximum number of warnings from admins.\n", g_entities[clientid].client->pers.netname);
		return;
	}
	return;
}

void Cmd_amTeleport_F(gentity_t *ent)
{
	vec3_t location;
	vec3_t forward;
	vec3_t origin;
	vec3_t yaw;
	int	clientid = -1;
	int	clientid2 = -1;
	char	arg1[256];
	char	arg2[256];
	char	buffer[256];

	if (!G_CheckAdmin(ent, ADMIN_TELEPORT))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() == 1)
	{
		//cm NOTE: This is where you teleport to a the telemark.
		if (ent->client->pers.amtelemark1 == 0 && ent->client->pers.amtelemark2 == 0 &&
			ent->client->pers.amtelemark3 == 0 && ent->client->pers.amtelemarkyaw == 0 &&
			!ent->client->pers.amtelemarkset)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1You do not have a telemark set.\nUse /amtelemark to establish a telemark.\n\""));
			return;
		}
		else
		{
			origin[0] = ent->client->pers.amtelemark1;
			origin[1] = ent->client->pers.amtelemark2;
			origin[2] = ent->client->pers.amtelemark3;
			yaw[0] = 0.0f;
			yaw[1] = ent->client->pers.amtelemarkyaw;
			yaw[2] = 0.0f;
			TeleportPlayer(ent, origin, yaw);
		}
	}
	//cm - Dom
	//Teleport to player
	if (trap->Argc() == 2)
	{
		trap->Argv(1, arg1, sizeof(arg1));
		clientid = G_ClientNumberFromName(arg1);
		if (clientid == -1)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Can't find client ID for %s\n\"", arg1));
			return;
		}
		if (clientid == -2)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Ambiguous client ID for %s\n\"", arg1));
			return;
		}
		if (clientid >= MAX_CLIENTS || clientid < 0)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Bad client ID for %s\n\"", arg1));
			return;
		}
		// either we have the client id or the string did not match 
		if (!g_entities[clientid].inuse)
		{ // check to make sure client slot is in use 
			trap->SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", arg1));
			return;
		}
		if (g_entities[clientid].health <= 0)
			return;
		if (clientid == ent - g_entities)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1You can't teleport yourself.\n\""));
			return;
		}
		//Copy their location
		VectorCopy(g_entities[clientid].client->ps.origin, location);
		AngleVectors(ent->client->ps.viewangles, forward, NULL, NULL);
		// set location out in front of your view
		forward[2] = 0; //no elevation change
		VectorNormalize(forward);
		VectorMA(g_entities[clientid].client->ps.origin, 100, forward, location);
		location[2] += 5; //add just a bit of height???
		//Teleport you to them
		TeleportPlayer(ent, location, g_entities[clientid].client->ps.viewangles);
		G_LogPrintf("Teleport admin command is executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	}
	//Teleport player to player
	if (trap->Argc() == 3)
	{
		trap->Argv(1, arg1, sizeof(arg1));
		trap->Argv(2, arg2, sizeof(arg2));
		clientid = G_ClientNumberFromName(arg1);
		clientid2 = G_ClientNumberFromName(arg2);

		if (clientid == -1)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Can't find client ID for %s\n\"", arg1));
			return;
		}
		if (clientid == -2)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Ambiguous client ID for %s\n\"", arg1));
			return;
		}

		if (clientid2 == -1)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Can't find client ID for %s\n\"", arg2));
			return;
		}
		if (clientid2 == -2)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Ambiguous client ID for %s\n\"", arg2));
			return;
		}
		if (clientid >= MAX_CLIENTS || clientid < 0)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Bad client ID for %s\n", arg1));
			return;
		}
		if (clientid2 >= MAX_CLIENTS || clientid2 < 0)
		{
			//OpenRP - Fixed a bug - was arg1 instead of arg2
			trap->SendServerCommand(ent - g_entities, va("print \"Bad client ID for %s\n", arg2));
			return;
		}

		// either we have the client id or the string did not match
		if (!g_entities[clientid].inuse)
		{ // check to make sure client slot is in use
			trap->SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", arg1));
			return;
		}
		if (g_entities[clientid].health <= 0)
			return;

		// either we have the client id or the string did not match
		if (!g_entities[clientid2].inuse)
		{ // check to make sure client slot is in use
			trap->SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", arg2));
			return;
		}
		if (g_entities[clientid2].health <= 0)
			return;

		if (clientid == clientid2)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Cant teleport client to same client.\n\""));
			return;
		}

		if (!G_AdminControl(ent, &g_entities[clientid]))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
			return;
		}

		//Copy client 2 origin
		VectorCopy(g_entities[clientid2].client->ps.origin, location);
		AngleVectors(g_entities[clientid2].client->ps.viewangles, forward, NULL, NULL);
		// set location out in front of your view
		forward[2] = 0; //no elevation change
		VectorNormalize(forward);
		VectorMA(g_entities[clientid2].client->ps.origin, 100, forward, location);
		location[2] += 5; //add just a bit of height???
		//Teleport the first player to the second player
		TeleportPlayer(&g_entities[clientid], location, g_entities[clientid2].client->ps.viewangles);
		G_LogPrintf("Teleport admin command is executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	}
	//Using manual coordinates
	if (trap->Argc() == 4)
	{
		Admin_Teleport(ent);
	}
	//cm - Dom
	//Teleport player to manual coordinates
	if (trap->Argc() == 5)
	{
		trap->Argv(1, arg1, sizeof(arg1));

		clientid = G_ClientNumberFromName(arg1);
		if (clientid == -1)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Can't find client ID for %s\n\"", arg1));
			return;
		}
		if (clientid == -2)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Ambiguous client ID for %s\n\"", arg1));
			return;
		}
		if (clientid >= MAX_CLIENTS || clientid < 0)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"Bad client ID for %s\n\"", arg1));
			return;
		}
		// either we have the client id or the string did not match 
		if (!g_entities[clientid].inuse)
		{ // check to make sure client slot is in use 
			trap->SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", arg1));
			return;
		}
		if (g_entities[clientid].health <= 0)
		{
			return;
		}

		//Taken from Admin_Teleport() with some mods
		trap->Argv(2, buffer, sizeof(buffer));
		origin[0] = atof(buffer);
		trap->Argv(3, buffer, sizeof(buffer));
		origin[1] = atof(buffer);
		trap->Argv(4, buffer, sizeof(buffer));
		origin[2] = atof(buffer);

		TeleportPlayer(&g_entities[clientid], origin, g_entities[clientid].client->ps.viewangles);
		G_LogPrintf("Teleport admin command is executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
		return;
	}
	return;
}

void Cmd_amAnnounce_F(gentity_t *ent)
{
	int pos = 0;
	char real_msg[512];
	char *msg = ConcatArgs(2);
	char cmdTarget[256];
	int clientid = -1;
	int i;
	gentity_t	*client;

	if (!G_CheckAdmin(ent, ADMIN_ANNOUNCE))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
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
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amannounce <name/clientid> <message>\nUse all or -1 for the clientid if you want to announce something to all players.\n\""));
		return;
	}

	trap->Argv(1, cmdTarget, sizeof(cmdTarget));

	if (!Q_stricmp(cmdTarget, "all") || (!Q_stricmp(cmdTarget, "-1")))
	{
		for (i = 0; i < level.maxclients; i++)
		{
			client = g_entities + i;
			if (client->inuse && client->client && level.clients[i].pers.connected == CON_CONNECTED)
			{
				//G_Sound( &g_entities[i], CHAN_MUSIC, G_SoundIndex( "sound/OpenRP/info.mp3" ) );
			}
		}
		trap->SendServerCommand(-1, va("print \"%s\n\"", real_msg));
		trap->SendServerCommand(-1, va("cp \"%s\"", real_msg));
		G_LogPrintf("Announce admin command executed by %s. The announcement was: %s\n", ent->client->pers.netname, real_msg);
		return;
	}

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
	//trap->SendServerCommand( clientid, "localSound sound/OpenRP/info.mp3" );
	trap->SendServerCommand(clientid, va("print \"%s\n\"", real_msg));
	trap->SendServerCommand(clientid, va("cp \"%s\"", real_msg));
	G_LogPrintf("Announce admin command executed by %s. It was sent to %s. The announcement was: %s\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname, real_msg);
	return;
}

void Cmd_amSilence_F(gentity_t *ent)
{
	char cmdTarget[256];
	int clientid = -1;

	if (!G_CheckAdmin(ent, ADMIN_SILENCE))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amsilence <name/clientid>\n\""));
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

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	if (!g_entities[clientid].client->sess.isSilenced)
	{
		g_entities[clientid].client->sess.isSilenced = qtrue;
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^2Player silenced.\n\""));
	trap->SendServerCommand(clientid, va("cp \"^2You were silenced by an admin.\n\""));
	G_LogPrintf("Silence admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	return;
}

void Cmd_amUnSilence_F(gentity_t *ent)
{
	char cmdTarget[256];
	int clientid = -1;

	if (!G_CheckAdmin(ent, ADMIN_SILENCE))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amunsilence <name/clientid>\n\""));
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

	if (!g_entities[clientid].client->sess.isSilenced)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2This player is not silenced.\n\""));
		return;
	}

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	g_entities[clientid].client->sess.isSilenced = qfalse;

	trap->SendServerCommand(ent - g_entities, va("print \"^2Player unsilenced.\n\""));
	trap->SendServerCommand(clientid, va("cp \"^2You were unsilenced by an admin.\n\""));
	G_LogPrintf("Unsilence admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	return;
}

void Cmd_amSleep_F(gentity_t *ent)
{
	char cmdTarget[256];
	int clientid = -1;

	if (!G_CheckAdmin(ent, ADMIN_SLEEP))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amsleep <name/clientid>\n\""));
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

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	//TODO
	/*
	// MJN - are they in an emote?  Then unemote them :P
	if (InEmote(g_entities[clientid].client->emote_num ) || InSpecialEmote(g_entities[clientid].client->emote_num ))
	{
	G_SetTauntAnim(&g_entities[clientid], g_entities[clientid].client->emote_num);
	}
	*/

	if (!g_entities[clientid].client->sess.isSleeping)
	{
		g_entities[clientid].client->sess.isSleeping = qtrue;
	}
	else
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1Player %s ^1is already sleeping. You can unsleep them with /amunsleep %s\n\"", g_entities[clientid].client->pers.netname, g_entities[clientid].client->pers.netname));
		return;
	}
	//TODO
	/*
	M_HolsterThoseSabers(&g_entities[clientid]);
	*/

	g_entities[clientid].client->ps.forceHandExtend = HANDEXTEND_KNOCKDOWN;
	g_entities[clientid].client->ps.forceDodgeAnim = 0;
	g_entities[clientid].client->ps.forceHandExtendTime = level.time + Q3_INFINITE;
	g_entities[clientid].client->ps.quickerGetup = qfalse;

	G_SetAnim(&g_entities[clientid], NULL, SETANIM_BOTH, BOTH_STUMBLEDEATH1, SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD, 0);

	g_entities[clientid].flags |= FL_GODMODE;

	trap->SendServerCommand(ent - g_entities, va("print \"^2%s is now sleeping.\n\"", g_entities[clientid].client->pers.netname));
	trap->SendServerCommand(clientid, "cp \"^2You are now sleeping.\n\"");

	G_LogPrintf("Sleep admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	return;
}

void Cmd_amUnsleep_F(gentity_t *ent)
{
	char cmdTarget[256];
	int clientid = -1;

	if (!G_CheckAdmin(ent, ADMIN_SLEEP))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amunsleep <name/clientid>\n\""));
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

	if (!g_entities[clientid].client->sess.isSleeping)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2This player is not sleeping.\n\""));
		return;
	}

	if (g_entities[clientid].client->ps.duelInProgress)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2You cannot use this command on someone who is duelling.\n\""));
		return;
	}

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	g_entities[clientid].client->sess.isSleeping = qfalse;

	g_entities[clientid].client->ps.forceDodgeAnim = 0;
	g_entities[clientid].client->ps.forceHandExtendTime = 0;
	g_entities[clientid].client->ps.quickerGetup = qfalse;

	g_entities[clientid].flags &= ~FL_GODMODE;

	trap->SendServerCommand(ent - g_entities, va("print \"^2%s has been unslept.\n\"", g_entities[clientid].client->pers.netname));

	trap->SendServerCommand(clientid, va("print \"^2You are no longer sleeping.\n\""));
	trap->SendServerCommand(clientid, va("cp \"^2You are no longer sleeping.\n\""));

	G_LogPrintf("Unsleep admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	return;
}



void Cmd_amEffect_F(gentity_t *ent)
{
	char	effect[256];
	gentity_t *fx_runner = G_Spawn();
	int i = 0;

	if (!G_CheckAdmin(ent, ADMIN_EFFECT))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amEffect <effect> Example: /amEffect env/small_fire\n\""));
		return;
	}

	trap->Argv(1, effect, sizeof(effect));

	AddSpawnField("fxFile", effect);
#ifdef __LINUX__
	fx_runner->s.origin[2] = (int)ent->client->ps.origin[2];
#endif
#ifdef _GAME
	fx_runner->s.origin[2] = (int)ent->client->ps.origin[2] - 15;
#endif
	fx_runner->s.origin[1] = (int)ent->client->ps.origin[1];
	fx_runner->s.origin[0] = (int)ent->client->ps.origin[0];
	SP_fx_runner(fx_runner);

	trap->SendServerCommand(ent - g_entities, va("print \"^2Effect placed with ID: ^7%i. ^2Use /amListEnts to view your effects and ents and their IDs.\n\"", fx_runner->s.number));

	for (i = 0; i < 127; i++)
	{
		if (ent->client->sess.entListIDs[i])
			continue;

		ent->client->sess.entListIDs[i] = fx_runner->s.number;
		Q_strncpyz((char*)ent->client->sess.entListNames[i], effect, sizeof(ent->client->sess.entListNames[i]));
		break;
	}

	G_LogPrintf("Effect command executed by %s to create the effect %s.\n", ent->client->pers.netname, effect);
	return;
}

void Cmd_amForceTeam_F(gentity_t *ent)
{
	char teamname[10], cmdTarget[256];
	int clientid = -1;

	if (!G_CheckAdmin(ent, ADMIN_FORCETEAM))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() != 3) //If the user doesn't specify both args.
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amforceteam <name/clientid> <newTeam>\nValid newTeam values: ^7spectator/spec/s free/f\n\""));
		return;
	}

	trap->Argv(1, cmdTarget, sizeof(cmdTarget)); //The first command argument is the target's name.

	trap->Argv(2, teamname, sizeof(teamname)); //The second command argument is the team's name.

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

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	/*
	if ( !Q_stricmp( teamname, "red" ) || !Q_stricmp( teamname, "r" ) ) {
	SetTeam( &g_entities[clientid], "red" );
	G_LogPrintf("ForceTeam [RED] admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	}
	else if ( !Q_stricmp( teamname, "blue" ) || !Q_stricmp( teamname, "b" ) ) {
	SetTeam( &g_entities[clientid], "blue" );
	G_LogPrintf("ForceTeam [BLUE] admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	}
	*/
	if (!Q_stricmp(teamname, "spectator") || !Q_stricmp(teamname, "spec") || !Q_stricmp(teamname, "s")) {
		SetTeam(&g_entities[clientid], "spectator");
		G_LogPrintf("ForceTeam [SPECTATOR] admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	}
	else if (!Q_stricmp(teamname, "free") || !Q_stricmp(teamname, "f")) {
		SetTeam(&g_entities[clientid], "free");
		G_LogPrintf("ForceTeam [FREE] admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	}
	trap->SendServerCommand(ent - g_entities, va("print \"^2Player %s ^2was forceteamed.\n\"", g_entities[clientid].client->pers.netname));
	return;
}

void Cmd_amMap_F(gentity_t *ent)
{
	char map[256];

	if (!G_CheckAdmin(ent, ADMIN_MAP))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amMap <mapName>\n\"");
		return;
	}

	trap->Argv(1, map, sizeof(map));

	trap->SendConsoleCommand(EXEC_APPEND, va("map %s\n", map));
	G_LogPrintf("Map changed to %s by %s.\n", map, ent->client->pers.netname);
	return;
}

void G_RemoveWeather(void) //ensiform's whacky weather clearer code
{
	int i;
	char s[256];

	for (i = 1; i<MAX_FX; i++)
	{
		trap->GetConfigstring(CS_EFFECTS + i, s, sizeof(s));

		if (!*s || !s[0])
		{
			return;
		}

		if (s[0] == '*')
		{
			trap->SetConfigstring(CS_EFFECTS + i, "");
		}
	}
}

void Cmd_amWeather_F(gentity_t *ent)
{
	char weather[256];
	int num;

	if (!G_CheckAdmin(ent, ADMIN_WEATHER))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amWeather <weather>\nTypes of weather: snow, rain, sandstorm, blizzard, fog, spacedust, acidrain\n\"");
		return;
	}

	trap->Argv(1, weather, sizeof(weather));

	trap->SendServerCommand(ent - g_entities, va("print \"^2Changing the weather...\n\""));

	if (!Q_stricmp(weather, "snow"))
	{
		G_RemoveWeather();
		num = G_EffectIndex("*clear");
		trap->SetConfigstring(CS_EFFECTS + num, "");
		G_EffectIndex("*snow");
	}
	else if (!Q_stricmp(weather, "rain"))
	{
		G_RemoveWeather();
		num = G_EffectIndex("*clear");
		trap->SetConfigstring(CS_EFFECTS + num, "");
		G_EffectIndex("*rain 500");
	}
	else if (!Q_stricmp(weather, "sandstorm"))
	{
		G_RemoveWeather();
		num = G_EffectIndex("*clear");
		trap->SetConfigstring(CS_EFFECTS + num, "");
		G_EffectIndex("*wind");
		G_EffectIndex("*sand");
	}
	else if (!Q_stricmp(weather, "blizzard"))
	{
		G_RemoveWeather();
		num = G_EffectIndex("*clear");
		trap->SetConfigstring(CS_EFFECTS + num, "");
		G_EffectIndex("*constantwind (100 100 -100)");
		G_EffectIndex("*fog");
		G_EffectIndex("*snow");
	}
	else if (!Q_stricmp(weather, "fog"))
	{
		G_RemoveWeather();
		num = G_EffectIndex("*clear");
		trap->SetConfigstring(CS_EFFECTS + num, "");
		G_EffectIndex("*heavyrainfog");
	}
	else if (!Q_stricmp(weather, "spacedust"))
	{
		G_RemoveWeather();
		num = G_EffectIndex("*clear");
		trap->SetConfigstring(CS_EFFECTS + num, "");
		G_EffectIndex("*spacedust 4000");
	}
	else if (!Q_stricmp(weather, "acidrain"))
	{
		G_RemoveWeather();
		num = G_EffectIndex("*clear");
		trap->SetConfigstring(CS_EFFECTS + num, "");
		G_EffectIndex("*acidrain 500");
	}

	else if (!Q_stricmp(weather, "clear"))
	{
		G_RemoveWeather();
		num = G_EffectIndex("*clear");
		trap->SetConfigstring(CS_EFFECTS + num, "");
	}

	else
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1Invalid type of weather.\nTypes of weather: snow, rain, sandstorm, blizzard, fog, spacedust, acidrain\n\"");
		return;
	}

	if (!Q_stricmp(weather, "clear"))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Weather cleared.\n\""));
		G_LogPrintf("Weather cleared by %s.\n", ent->client->pers.netname);
		return;
	}

	else
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Weather changed to %s. To change it back, use /amWeather clear\n\"", weather));
		G_LogPrintf("Weather command executed by %s. The weather is now %s.\n", ent->client->pers.netname, weather);
		return;
	}
}

//-----------
//WeatherPlus
//-----------
void Cmd_amWeatherPlus_F(gentity_t *ent)
{
	char weather[256];
	int num;

	if (!G_CheckAdmin(ent, ADMIN_WEATHER))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amWeatherPlus <weather>\nTypes of weather: snow, rain, sandstorm, blizzard, fog, spacedust, acidrain\n\"");
		return;
	}

	trap->Argv(1, weather, sizeof(weather));

	if (!Q_stricmp(weather, "snow"))
	{
		trap->SetConfigstring(CS_EFFECTS, "");
		G_EffectIndex("*snow");
		trap->SendServerCommand(ent - g_entities, "print \"^2Adding weather: \nSnow\n\"");
	}
	else if (!Q_stricmp(weather, "rain"))
	{
		trap->SetConfigstring(CS_EFFECTS, "");
		G_EffectIndex("*rain 500");
		trap->SendServerCommand(ent - g_entities, "print \"^2Adding weather: \nSnow\n\"");
	}
	else if (!Q_stricmp(weather, "sandstorm"))
	{
		trap->SetConfigstring(CS_EFFECTS, "");
		G_EffectIndex("*wind");
		G_EffectIndex("*sand");
		trap->SendServerCommand(ent - g_entities, "print \"^2Adding weather:\nSand\nWind\n\"");
	}
	else if (!Q_stricmp(weather, "blizzard"))
	{
		trap->SetConfigstring(CS_EFFECTS, "");
		G_EffectIndex("*constantwind (100 100 -100)");
		G_EffectIndex("*fog");
		G_EffectIndex("*snow");
		trap->SendServerCommand(ent - g_entities, "print \"^2Adding weather:\nFog\nSnow\n\"");
	}
	else if (!Q_stricmp(weather, "heavyfog"))
	{
		trap->SetConfigstring(CS_EFFECTS, "");
		G_EffectIndex("*heavyrainfog");
		trap->SendServerCommand(ent - g_entities, "print \"^2Adding weather:\nHeavy Fog\n\"");
	}
	else if (!Q_stricmp(weather, "spacedust"))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Adding weather:\nSpace Dust\n\"");
		trap->SetConfigstring(CS_EFFECTS, "");
		G_EffectIndex("*spacedust 4000");
	}
	else if (!Q_stricmp(weather, "acidrain"))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Adding weather:\nAcid Rain\n\"");
		trap->SetConfigstring(CS_EFFECTS, "");
		G_EffectIndex("*acidrain 500");
	}

	else if (!Q_stricmp(weather, "fog"))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Adding weather:\nFog\n\"");
		trap->SetConfigstring(CS_EFFECTS, "");
		G_EffectIndex("*fog");
		
	}
	else if (!Q_stricmp(weather, "sand"))
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Adding weather:\nSand\n\"");
		trap->SetConfigstring(CS_EFFECTS, "");
		G_EffectIndex("*sand");
		
	}
	else if (!Q_stricmp(weather, "clear"))
	{
		G_RemoveWeather();
		num = G_EffectIndex("*clear");
		trap->SetConfigstring(CS_EFFECTS + num, "");
	}
	else
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1Invalid type of weather.\nTypes of weather: snow, rain, sandstorm, blizzard, fog, spacedust, acidrain\n\"");
		return;
	}

	if (!Q_stricmp(weather, "clear"))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Weather cleared.\n\""));
		G_LogPrintf("Weather cleared by %s.\n", ent->client->pers.netname);
		return;
	}

	else
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Weather %s added. To clear the weather, use /amWeatherPlus clear\n\"", weather));
		G_LogPrintf("WeatherPlus command executed by %s. The weather added was %s.\n", ent->client->pers.netname, weather);
		return;
	}
}

void Cmd_amStatus_F(gentity_t *ent)
{
	int i;
	gentity_t	*client;

	if (!G_CheckAdmin(ent, ADMIN_STATUS))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^2Status:\n\""));
	for (i = 0; i < level.maxclients; i++)
	{
		client = g_entities + i;
		if (client->inuse && client->client && level.clients[i].pers.connected == CON_CONNECTED)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^2ID: ^7%i ^2Name: ^7%s ^2IP: ^7%s\n\"",
				i, level.clients[i].pers.netname, level.clients[i].sess.IP));

		}
	}
	return;
}

void uwRename(gentity_t *player, const char *newname)
{
	char userinfo[MAX_INFO_STRING];
	int clientNum = player - g_entities;
	trap->GetUserinfo(clientNum, userinfo, sizeof(userinfo));
	Info_SetValueForKey(userinfo, "name", newname);
	trap->SetUserinfo(clientNum, userinfo);
	ClientUserinfoChanged(clientNum);
	player->client->pers.netnameTime = level.time + 5000;
}

void uw2Rename(gentity_t *player, const char *newname)
{
	char userinfo[MAX_INFO_STRING];
	int clientNum = player - g_entities;
	trap->GetUserinfo(clientNum, userinfo, sizeof(userinfo));
	Info_SetValueForKey(userinfo, "name", newname);
	trap->SetUserinfo(clientNum, userinfo);
	ClientUserinfoChanged(clientNum);
	player->client->pers.netnameTime = level.time + Q3_INFINITE;
}

void Cmd_amRename_F(gentity_t *ent)
{
	char currentname[256], newname[256];
	int clientid = -1;

	if (!G_CheckAdmin(ent, ADMIN_RENAME))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() != 3)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amrename <currentName/clientID> <newName>\n\""));
		return;
	}

	trap->Argv(1, currentname, sizeof(currentname));
	clientid = G_ClientNumberFromName(currentname);
	if (clientid == -1)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Can't find client ID for %s\n\"", currentname));
		return;
	}
	if (clientid == -2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Ambiguous client ID for %s\n\"", currentname));
		return;
	}
	if (clientid >= MAX_CLIENTS || clientid < 0)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Bad client ID for %s\n\"", currentname));
		return;
	}
	// either we have the client id or the string did not match 
	if (!g_entities[clientid].inuse)
	{ // check to make sure client slot is in use 
		trap->SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", currentname));
		return;
	}

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	trap->Argv(2, newname, sizeof(newname));
	G_LogPrintf("Rename admin command executed by %s on %s\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	trap->SendServerCommand(clientid, va("cvar name %s", newname));
	uwRename(&g_entities[clientid], newname);

	G_LogPrintf("Rename admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
	return;
}

void Cmd_GiveAdmin_F(gentity_t *ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char username[256] = { 0 }, temp[3] = { 0 }, comparisonName[256] = { 0 };
	int adminLevel = 11, accountID = 0;

	if (!G_CheckAdmin(ent, ADMIN_GRANTREMOVEADMIN))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
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
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amGiveAdmin <username> <adminLevel>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, username, sizeof(username));
	//Check if this username exists
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
		Q_strncpyz(comparisonName, (const char*)sqlite3_column_text(stmt, 0), sizeof(comparisonName));
		sqlite3_finalize(stmt);
	}

	if (comparisonName[0] == '\0')
	{
		//The username does not exist, thus, the error does.
		trap->SendServerCommand(ent - g_entities, va("print \"^1Username %s does not exist.\n\"", username));
		trap->SendServerCommand(ent - g_entities, va("cp \"^1Username %s does not exist.\n\"", username));
		sqlite3_close(db);
		return;
	}

	trap->Argv(2, temp, sizeof(temp));

	adminLevel = atoi(temp);

	if (adminLevel < 1 || adminLevel > 10)
	{
		trap->SendServerCommand(ent - g_entities, "print \"The admin level must be a number from 1-10. 1 is the highest level, 10 is the lowest.\n\"");
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

	if (!accountID)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Username %s does not exist\n\"", username));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("UPDATE Accounts set Admin='1' WHERE Username='%s'", username), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("UPDATE Accounts set AdminLevel='%i' WHERE Username='%s'", adminLevel, username), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^2Admin (level %i) given to %s.\n\"", adminLevel, username));
	sqlite3_close(db);
	return;
}

void Cmd_SVGiveAdmin_F(void)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char username[256] = { 0 }, temp[3] = { 0 }, comparisonName[256] = { 0 };
	int adminLevel = 11;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() != 3)
	{
		trap->Print("Command Usage: giveAdmin <username> <adminLevel>\n");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, username, sizeof(username));
	//Check if this username exists
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
		Q_strncpyz(comparisonName, (const char*)sqlite3_column_text(stmt, 0), sizeof(comparisonName));
		sqlite3_finalize(stmt);
	}

	if (comparisonName[0] == '\0')
	{
		//The username does not exist, thus, the error does.
		trap->Print("Username %s does not exist.\n", username);
		sqlite3_close(db);
		return;
	}

	trap->Argv(2, temp, sizeof(temp));

	adminLevel = atoi(temp);

	if (adminLevel < 1 || adminLevel > 10)
	{
		trap->Print("Error: The admin level must be a number from 1-10.\n");
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("UPDATE Accounts set Admin='1' WHERE Username='%s'", username), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("UPDATE Accounts set AdminLevel='%i' WHERE Username='%s'", adminLevel, username), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->Print("Admin (level %i) given to %s.\n", adminLevel, username);
	sqlite3_close(db);
	return;
}

void Cmd_RemoveAdmin_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char username[256] = { 0 };
	int valid = 0;
	int adminLevel = 11;
	int isAdmin = 0;

	if (!G_CheckAdmin(ent, ADMIN_GRANTREMOVEADMIN))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
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
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amRemoveAdmin <username>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, username, sizeof(username));

	Q_StripColor(username);
	Q_strlwr(username);

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
		valid = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!valid)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Username %s does not exist\n\"", username));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Admin FROM Accounts WHERE Username='%s'", username), -1, &stmt, NULL);
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

	if (!isAdmin)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^7%s ^1is not an admin.\n\"", username));
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT AdminLevel FROM Accounts WHERE Username='%s'", username), -1, &stmt, NULL);
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

	//Greater than, not less than, because 1 is the highest level while 10 is the lowest.
	if (ent->client->sess.adminLevel >= adminLevel)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^7%s ^1has an equal or higher admin level compared to yours.\n\"", username));
		return;
	}

	rc = sqlite3_exec(db, va("UPDATE Accounts set Admin='0' WHERE Username='%s'", username), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	//Set their adminlevel to 11 just to be safe.
	rc = sqlite3_exec(db, va("UPDATE Accounts set adminlevel='11' WHERE Username='%s'", username), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->SendServerCommand(ent - g_entities, va("print \"Admin removed from account %s\n\"", username));
	sqlite3_close(db);
	return;
}

void Cmd_SVRemoveAdmin_F(void)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	sqlite3_stmt *stmt;
	int rc;
	char username[256] = { 0 };
	int valid = 0;

	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		//trap->SendServerCommand( ent-g_entities, "print \"^1The server's database is not connected.\n\"" );
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->Print("Command Usage: removeAdmin <username>\n");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, username, sizeof(username));

	Q_StripColor(username);
	Q_strlwr(username);

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
		valid = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (!valid)
	{
		trap->Print("Username %s does not exist\n\"", username);
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("UPDATE Accounts set Admin='0' WHERE Username='%s'", username), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	//Set their adminlevel to 11 just to be safe.
	rc = sqlite3_exec(db, va("UPDATE Accounts set AdminLevel='11' WHERE Username='%s'", username), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->Print("Admin removed from account %s.\n", username);
	sqlite3_close(db);
	return;
}

void AddSkill(gentity_t *self, int amount)
{
	//OpenRP - Skillpoints

	if (!amount)
		return;

	if ((self->client->sess.skillPoints + amount) < 1)
	{
		trap->Print("Error: Failed to give %i skill points to %s because they would have < 1 skill point if they were given.", amount, self->client->pers.netname);
		return;
	}

	self->client->sess.skillPoints += amount;
}

void Cmd_GiveSkillPoints_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char charName[256] = { 0 }, temp[64] = { 0 };
	int changedSkillPoints = 0, charID = 0, accountID = 0, clientID = 33, loggedIn = 0, currentLevel = 1, currentSkillPoints = 1, newSkillPointsTotal = 0, i = 0;
	extern void LevelCheck(int charID);

	if (!G_CheckAdmin(ent, ADMIN_SKILLPOINTS))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
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
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amGiveSkillPoints <characterName> <amount>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, charName, sizeof(charName));

	trap->Argv(2, temp, sizeof(temp));
	changedSkillPoints = atoi(temp);

	Q_StripColor(charName);
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

	newSkillPointsTotal = currentSkillPoints + changedSkillPoints;

	if (newSkillPointsTotal < 1)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't give %i skill points to %s, or else they would have < 1 skill point.\n\"", changedSkillPoints));
		return;
	}

	rc = sqlite3_exec(db, va("UPDATE Characters set SkillPoints='%i' WHERE CharID='%i'", newSkillPointsTotal, charID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	switch (currentLevel)
	{
	case 50:
		if (loggedIn)
		{
			AddSkill(&g_entities[clientID], changedSkillPoints);
			//trap->SendServerCommand( clientID, "localSound sound/OpenRP/xp.mp3" );
			trap->SendServerCommand(clientID, va("print \"^2You received %i skill points! You are the highest level, so you won't level up anymore!\n\"", changedSkillPoints));
		}
		break;
	default:
		if (loggedIn)
		{
			AddSkill(&g_entities[clientID], changedSkillPoints);
			//trap->SendServerCommand( clientID, "localSound sound/OpenRP/xp.mp3" );
			trap->SendServerCommand(clientID, va("print \"^2You received %i skill points!\n\"", changedSkillPoints));
		}
		LevelCheck(charID);
		break;
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^2%i skill points have been given to character %s.\n\"", changedSkillPoints, charName));
	trap->SendServerCommand(ent - g_entities, va("cp \"^2%i skill points have been given to character %s.\n\"", changedSkillPoints, charName));
	sqlite3_close(db);
	return;
}

void Cmd_GenerateCredits_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char charName[256] = { 0 }, temp[64] = { 0 };
	int changedCredits = 0, charID = 0, accountID = 0, clientID = 33, loggedIn = 0, currentCredits = 0, newCreditsTotal = 0;

	if (!G_CheckAdmin(ent, ADMIN_CREDITS))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
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
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amGenCredits <characterName> <amount>\n\"");
		sqlite3_close(db);
		return;
	}

	//Character name
	trap->Argv(1, charName, sizeof(charName));

	//Credits Added or removed.
	trap->Argv(2, temp, sizeof(temp));
	changedCredits = atoi(temp);

	Q_StripColor(charName);
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

	newCreditsTotal = currentCredits + changedCredits;

	rc = sqlite3_exec(db, va("UPDATE Characters set Credits='%i' WHERE CharID='%i'", newCreditsTotal, charID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	if (loggedIn)
	{
		trap->SendServerCommand(clientID, va("print \"^2You received some credits from an admin!\n\"", charName));
		trap->SendServerCommand(clientID, va("cp \"^2You received some credits from an admin!\n\"", charName));
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^2%i credits have been generated and given to character %s.\n\"", changedCredits, charName));
	trap->SendServerCommand(ent - g_entities, va("cp \"^2%i credits have been generated and given to character %s.\n\"", changedCredits, charName));
	sqlite3_close(db);
	return;
}

void Cmd_CreateFaction_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char factionName[256] = { 0 }, comparisonName[256] = { 0 };
	int charCurrentFactionID = 0, factionID = 0;

	if (!G_CheckAdmin(ent, ADMIN_FACTION))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (!ent->client->sess.characterSelected)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1You must be logged in and have a character selected in order to use this command.\n\"");
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
		charCurrentFactionID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
	}

	if (charCurrentFactionID > 0)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You must leave your current faction first before creating a faction.\n\""));
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amCreateFaction <factionName>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, factionName, sizeof(factionName));

	Q_StripColor(factionName);
	Q_strlwr(factionName);

	rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE Name='%s'", factionName), -1, &stmt, NULL);
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
		trap->SendServerCommand(ent - g_entities, va("print \"^1Faction %s already exists.\n\"", comparisonName));
		trap->SendServerCommand(ent - g_entities, va("cp \"^1Faction %s already exists.\n\"", comparisonName));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_exec(db, va("INSERT INTO Factions(Name,Bank) VALUES('%s','0')", factionName), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT FactionID FROM Factions WHERE Name='%s'", factionName), -1, &stmt, NULL);
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

	rc = sqlite3_exec(db, va("UPDATE Characters set FactionID='%i' WHERE CharID='%i'", factionID, ent->client->sess.characterID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	rc = sqlite3_exec(db, va("UPDATE Characters set FactionRank='Leader' WHERE CharID='%i'", ent->client->sess.characterID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}
	trap->SendServerCommand(ent - g_entities, va("print \"^2The %s faction has been created. To add people to it, use /amSetFaction %i <characterName>\n\"", factionName, factionID));
	sqlite3_close(db);
	return;
}

void Cmd_SetFaction_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char charName[256] = { 0 }, factionIDTemp[64] = { 0 }, factionName[256] = { 0 };
	int charID = 0, accountID = 0, clientID = 33, loggedIn = 0, currentFactionID = 0, factionID = 0;

	if (!G_CheckAdmin(ent, ADMIN_FACTION))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
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
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amSetFaction <characterName> <factionID>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, charName, sizeof(charName));

	trap->Argv(2, factionIDTemp, sizeof(factionIDTemp));
	factionID = atoi(factionIDTemp);

	Q_StripColor(charName);
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
		currentFactionID = sqlite3_column_int(stmt, 0);
		sqlite3_finalize(stmt);
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

	if (!Q_stricmp(factionIDTemp, "none"))
	{
		if (!currentFactionID)
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1This character is not in a faction.\n\"");
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_exec(db, va("UPDATE Characters set FactionID='0' WHERE CharID='%i'", charID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_exec(db, va("UPDATE Characters set FactionRank='none' WHERE CharID='%i'", charID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		if (loggedIn)
		{
			trap->SendServerCommand(clientID, "print \"^2You have been removed from your faction.\n\"");
			trap->SendServerCommand(clientID, "cp \"^2You have been removed from your faction.\n\"");
		}
		trap->SendServerCommand(ent - g_entities, va("print \"^2Character %s has been removed from their faction.\n\"", charName));
	}
	else
	{
		if (currentFactionID)
		{
			trap->SendServerCommand(ent - g_entities, "print \"^1This character is already in a faction.\n\"");
			sqlite3_close(db);
			return;
		}

		rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE ID='%i'", factionID), -1, &stmt, NULL);
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

		if (factionName[0] == '\0')
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1Faction with factionID of ^7%i ^1does not exist.\n\"", factionID));
			trap->SendServerCommand(ent - g_entities, va("cp \"^1Faction with factionID of ^7%i ^1does not exist.\n\"", factionID));
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_exec(db, va("UPDATE Characters set FactionID='%i' WHERE CharID='%i'", factionID, charID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}
		rc = sqlite3_exec(db, va("UPDATE Characters set FactionRank='Member' WHERE CharID='%i'", charID), 0, 0, &zErrMsg);
		if (rc != SQLITE_OK)
		{
			trap->Print("SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db);
			return;
		}

		if (loggedIn)
		{
			trap->SendServerCommand(clientID, va("print \"^2You have been put in the %s faction!\nYou can use /factionInfo to view info about it.\n\"", factionName));
			trap->SendServerCommand(clientID, va("cp \"^2You have been put in the %s faction!\n^2You can use /factionInfo to view info about it.\n\"", factionName));
		}

		trap->SendServerCommand(ent - g_entities, va("print \"^2Character %s has been put in the %s faction.\nUse /setFactionRank to change their rank. Is it currently set to: Member\n\"", charName, factionName));
	}
	sqlite3_close(db);
	return;
}

void Cmd_FactionGenerateCredits_F(gentity_t * ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	char temp[64] = { 0 }, temp2[64] = { 0 }, factionName[256] = { 0 };
	int factionID = 0, changedCredits = 0, currentCredits = 0, newCreditsTotal = 0;

	if (!G_CheckAdmin(ent, ADMIN_FACTION) && !G_CheckAdmin(ent, ADMIN_CREDITS))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
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
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amFactionGenCredits <factionID> <amount>\n\"");
		sqlite3_close(db);
		return;
	}

	//Faction ID
	trap->Argv(1, temp, sizeof(temp));
	factionID = atoi(temp);

	//Credits Added or removed.
	trap->Argv(2, temp2, sizeof(temp2));
	changedCredits = atoi(temp2);

	//Check if the faction exists
	if (!factionID)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1Faction with FactionID %i does not exist.\n\"", factionID));
		trap->SendServerCommand(ent - g_entities, va("cp \"^1Faction with FactionID %i does not exist.\n\"", factionID));
		sqlite3_close(db);
		return;
	}

	rc = sqlite3_prepare_v2(db, va("SELECT Name FROM Factions WHERE FactionID='%i'", factionID), -1, &stmt, NULL);
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

	rc = sqlite3_prepare_v2(db, va("SELECT Bank FROM Factions WHERE FactionID='%i'", factionID), -1, &stmt, NULL);
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

	newCreditsTotal = currentCredits + changedCredits;

	rc = sqlite3_exec(db, va("UPDATE Factions set Bank='%i' WHERE FactionID='%i'", newCreditsTotal, factionID), 0, 0, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		trap->Print("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		sqlite3_close(db);
		return;
	}

	trap->SendServerCommand(ent - g_entities, va("print \"^2%i credits have been generated and given to faction %s.\n\"", changedCredits, factionName));
	trap->SendServerCommand(ent - g_entities, va("cp \"^2%i credits have been generated and given to faction %s.\n\"", changedCredits, factionName));
	sqlite3_close(db);
	return;
}

void Cmd_ShakeScreen_F(gentity_t * ent)
{
	int i;
	char temp[64], temp2[64];
	int intensity, length;
	gentity_t	*client;

	if (!G_CheckAdmin(ent, ADMIN_SHAKE))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() != 3)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amShakeScreen <intensity> <length>\nExample: /amShakeScreen 5 7\"");
		return;
	}

	trap->Argv(1, temp, sizeof(temp));
	intensity = atoi(temp);
	trap->Argv(2, temp2, sizeof(temp2));
	length = atoi(temp2) * 1000;


	for (i = 0; i < level.maxclients; i++)
	{
		client = g_entities + i;
		if (client->inuse && client->client && level.clients[i].pers.connected == CON_CONNECTED)
		{
			G_ScreenShake(client->s.origin, &g_entities[i], intensity, length, qtrue);
			//Don't do a center print for the target - it would distract from the shaking screen.
			trap->SendServerCommand(i, "print \"^3An admin shook your screen.\n\"");
		}
	}
	trap->SendServerCommand(ent - g_entities, "print \"^2You shook everybody's screen.\n\"");
	return;
}


void Cmd_Audio_F(gentity_t * ent)
{
	char audioPath[256] = { 0 };
	int i = 0;
	gentity_t	*client;

	if (!G_CheckAdmin(ent, ADMIN_AUDIO))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amAudio <path>\nor /amAudio none to turn off the map music.\n\""));
		return;
	}

	trap->Argv(1, audioPath, sizeof(audioPath));

	if (!Q_stricmpn(audioPath, "music/", 6))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2You started playing the music file: ^7%s\n\"", audioPath));
		trap->SetConfigstring(CS_MUSIC, audioPath);
	}
	else if (!Q_stricmpn(audioPath, "sound/", 6))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2You started playing the sound file: ^7%s\n\"", audioPath));
		for (i = 0; i < level.maxclients; i++)
		{
			client = g_entities + i;
			if (client->inuse && client->client && level.clients[i].pers.connected == CON_CONNECTED)
				G_Sound(&g_entities[i], CHAN_MUSIC, G_SoundIndex(va("%s", audioPath)));
		}
	}
	else if (!Q_stricmpn(audioPath, "none", 4))
	{
		trap->SetConfigstring(CS_MUSIC, "________");
		trap->SendServerCommand(ent - g_entities, "print \"^2You turned off the map music.\n\"");
	}
	else
		trap->SendServerCommand(ent - g_entities, "print \"^1Music or sound must be in the music or sound folders, or you're just using the command incorrectly.\n\"");

	return;
}

void Cmd_amTelemark_F(gentity_t * ent)
{
	if (!G_CheckAdmin(ent, ADMIN_TELEPORT))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	// mark the teleportation point with a mark.
	ent->client->ps.viewangles[0] = 0.0f;
	ent->client->ps.viewangles[2] = 0.0f;
	ent->client->pers.amtelemark1 = (int)ent->client->ps.origin[0];
	ent->client->pers.amtelemark2 = (int)ent->client->ps.origin[1];
	ent->client->pers.amtelemark3 = (int)ent->client->ps.origin[2];
	ent->client->pers.amtelemarkyaw = (int)ent->client->ps.viewangles[1];
	ent->client->pers.amtelemarkset = qtrue;
	trap->SendServerCommand(ent - g_entities, va("print \"^2Tele mark placed at: X:^7%d, ^2Y:^7%d, ^2Z:^7%d, ^2YAW:^7%d\n\"", (int)ent->client->ps.origin[0], (int)ent->client->ps.origin[1], (int)ent->client->ps.origin[2], ent->client->pers.amtelemarkyaw));
	return;
}

void Cmd_amOrigin_F(gentity_t * ent)
{
	int	clientid = -1;
	char	arg1[256];

	if (!G_CheckAdmin(ent, ADMIN_TELEPORT))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	trap->Argv(1, arg1, sizeof(arg1));

	clientid = G_ClientNumberFromName(arg1);
	if (clientid == -1)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Can't find client ID for %s\n\"", arg1));
		return;
	}
	if (clientid == -2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Ambiguous client ID for %s\n\"", arg1));
		return;
	}
	if (clientid >= MAX_CLIENTS || clientid < 0)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"Bad client ID for %s\n\"", arg1));
		return;
	}
	// either we have the client id or the string did not match 
	if (!g_entities[clientid].inuse)
	{ // check to make sure client slot is in use 
		trap->SendServerCommand(ent - g_entities, va("print \"Client %s is not active\n\"", arg1));
		return;
	}

	if (clientid)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2X:^7%d, ^2Y:^7%d, ^2Z:^7%d\n\"", (int)g_entities[clientid].client->ps.origin[0], (int)g_entities[clientid].client->ps.origin[1], (int)g_entities[clientid].client->ps.origin[2]));
		return;
	}
	else
		trap->SendServerCommand(ent - g_entities, va("print \"^2X:^7%d, ^2Y:^7%d, ^2Z:^7%d\n\"", (int)ent->client->ps.origin[0], (int)ent->client->ps.origin[1], (int)ent->client->ps.origin[2]));

	return;
}

void Cmd_AdminChat_F(gentity_t *ent)
{
	int pos = 0;
	char real_msg[512];
	char *msg = ConcatArgs(1);
	int i;
	gentity_t	*client;

	if (!ent->client->sess.isAdmin)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
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

	for (i = 0; i < level.maxclients; i++)
	{
		client = g_entities + i;
		if (level.clients[i].sess.isAdmin && client->inuse && client->client && level.clients[i].pers.connected == CON_CONNECTED)
			trap->SendServerCommand(i, va("chat \"^6<Admin Chat> ^7%s^6: ^6%s\"", ent->client->pers.netname, real_msg));
	}
	return;
}

void Cmd_AllChat_F(gentity_t * ent)
{
	char parameter[9];

	if (!G_CheckAdmin(ent, ADMIN_ALLCHAT))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (!openrp_DistanceBasedChat.integer)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^1Distance based chat is disabled on this server, so you cannot use this command.\n\"");
		return;
	}

	if (trap->Argc() < 2)
	{
		if (!ent->client->sess.allChat)
		{
			if (ent->client->sess.allChatComplete)
			{
				ent->client->sess.allChatComplete = qfalse;
				trap->SendServerCommand(ent - g_entities, "print \"^2All chat COMPLETE turned OFF.\n\"");
			}
			ent->client->sess.allChat = qtrue;
			trap->SendServerCommand(ent - g_entities, "print \"^2All chat turned ON.\n\"");
			return;
		}
		else
		{
			ent->client->sess.allChat = qfalse;
			trap->SendServerCommand(ent - g_entities, "print \"^2All chat turned OFF.\n\"");
			return;
		}
	}

	trap->Argv(1, parameter, sizeof(parameter));

	if (!Q_stricmp(parameter, "complete"))
	{
		if (ent->client->sess.allChat)
		{
			ent->client->sess.allChat = qfalse;
			trap->SendServerCommand(ent - g_entities, "print \"^2All chat turned OFF.\n\"");
		}

		if (!ent->client->sess.allChatComplete)
		{
			ent->client->sess.allChatComplete = qtrue;
			trap->SendServerCommand(ent - g_entities, "print \"^2All chat COMPLETE turned ON.\n\"");
		}
		else
		{
			ent->client->sess.allChatComplete = qfalse;
			trap->SendServerCommand(ent - g_entities, "print \"^2All chat COMPLETE turned OFF.\n\"");
		}
	}
	return;
}

void Cmd_amWarningList_F(gentity_t *ent)
{
	int i;
	gentity_t	*client;


	if (!G_CheckAdmin(ent, ADMIN_WARN))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	trap->SendServerCommand(ent - g_entities, "print \"^2Warning List:\n\n\"");
	for (i = 0; i < level.maxclients; i++)
	{
		client = g_entities + i;
		if (client->inuse && client->client && level.clients[i].pers.connected == CON_CONNECTED)
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^7%s %i^2/^7%i\n\"", level.clients[i].pers.netname, level.clients[i].sess.warnings, openrp_maxWarnings.integer));
		}
	}
	return;
}

//Thanks to Raz0r for posting this command.
void Cmd_SpawnEnt_F(gentity_t *ent)
{
	gentity_t *obj;
	char buf[32]; // arg1
	trace_t tr; //For tracing
	vec3_t fPos; //We're going to adjust the tr.endpos, and put it in here
	int i; //For looping

	if (!G_CheckAdmin(ent, ADMIN_BUILD))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amSpawnEnt <entName>\n\"");
		return;
	}

	//Trace to where we're looking
	AngleVectors(ent->client->ps.viewangles, fPos, 0, 0);
	for (i = 0; i < 3; i++)
	{
		fPos[i] = ent->client->ps.origin[i] + fPos[i] * Q3_INFINITE;
	}
	trap->Trace(&tr, ent->client->ps.origin, NULL, NULL, fPos, ent->s.number, ent->clipmask, qfalse, 0, 0);

	//Move along the normal of the traced plane
	VectorMA(tr.endpos, 48.0f, tr.plane.normal, fPos);

	//Grab the first argument (so /spawn misc_ammo_floor_unit)
	trap->Argv(1, buf, sizeof(buf));

	obj = G_Spawn(); //This will give us the first free slot in g_entities!
	//Now we have to set the classname and the origin
	//Then we tell the game to find the appropriate spawn function for the classname/entity
	obj->classname = buf;
	VectorCopy(fPos, obj->s.origin);
	G_CallSpawn(obj); // Will search through the list of known entities and react accordingly

	trap->SendServerCommand(ent - g_entities, va("print \"^2Ent with ID: ^7%i ^2spawned. Use /amListEnts to view your effects and ents and their IDs.\n\"", obj->s.number));

	for (i = 0; i < 127; i++)
	{
		if (ent->client->sess.entListIDs[i])
			continue;

		ent->client->sess.entListIDs[i] = obj->s.number;
		Q_strncpyz((char*)ent->client->sess.entListNames[i], buf, sizeof(ent->client->sess.entListNames[i]));
		break;
	}
	//The appropriate spawn function will take care of
	//any ent->think() ent->die() etc function pointers, so we don't have to worry.
	return;
}

//Thanks to Raz0r for his ent tutorial that included this command's code
void Cmd_RemoveEntity_F(gentity_t *ent)
{
	//trace_t   tr;
	//vec3_t   fPos;
	//vec3_t   mins;
	//vec3_t   maxs;
	//int      i;
	//int      ents[8];
	int entID = 0;
	char entIDTemp[64] = { 0 };
	int i = 0;

	if (!G_CheckAdmin(ent, ADMIN_BUILD))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"Command Usage: /amRemoveEnt <entID>\n\"");
		return;
		/*
		//Trace to where we're looking
		AngleVectors( ent->client->ps.viewangles, fPos, 0, 0 );
		for (i=0; i<3; i++)
		{
		fPos[i] = ent->client->ps.origin[i] + fPos[i]*Q3_INFINITE;
		}
		trap->Trace( &tr, ent->client->ps.origin, 0, 0, fPos, ent->s.number, ent->clipmask );

		//Create a box
		VectorSet( mins, tr.endpos[0]-64.0f, tr.endpos[1]-64.0f, tr.endpos[2]-64.0f );
		VectorSet( maxs, tr.endpos[0]+64.0f, tr.endpos[1]+64.0f, tr.endpos[2]+64.0f );
		trap->EntitiesInBox( mins, maxs, &ents[0], 8 );

		//Anything in this box will be removed
		for (i=0; i<8; i++)
		{
		if ( ents[i] > 0 && ents[i] < 1024 && &g_entities[ents[i]].inuse )
		{
		G_FreeEntity( &g_entities[ents[i]] ); // G_FreeEntity will free up the slot in g_entities so it can be re-used!
		}
		else
		{
		trap->SendServerCommand( ent-g_entities, "print \"^1Invalid or no ent(s) found.\n\"" );
		return;
		}
		}
		trap->SendServerCommand( ent-g_entities, "print \"^2Ent(s) you were looking at has(have) been removed.\n\"" );
		return;
		*/
	}

	trap->Argv(1, entIDTemp, sizeof(entIDTemp));
	entID = atoi(entIDTemp);

	if (entID > 31 && entID < 1024 && &g_entities[entID].inuse) //Make sure the ent isn't a player and is an ID that is in use.
	{
		G_FreeEntity(&g_entities[entID]); // G_FreeEntity will free up the slot in g_entities so it can be re-used!
		for (i = 0; i < 127; i++)
		{
			if (ent->client->sess.entListIDs[i] == entID)
			{
				ent->client->sess.entListIDs[i] = 0;
				Q_strncpyz((char*)ent->client->sess.entListNames[i], "null", sizeof(ent->client->sess.entListNames[i]));
			}
			else
				continue;
		}
		trap->SendServerCommand(ent - g_entities, va("print \"^2Entity with ID: ^7%i ^2removed.\n\"", entID));
		G_LogPrintf("Remove ent command executed by %s to remove ent with ID %i.\n", ent->client->pers.netname, entID);
	}
	else
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Invalid entID.\"", entID));
	}
	return;
}

void Cmd_ListEnts_F(gentity_t *ent)
{
	int i = 0;

	if (!G_CheckAdmin(ent, ADMIN_BUILD))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	trap->SendServerCommand(ent - g_entities, "print \"^2Your entities:\n\"");
	for (i = 0; i < 127; i++)
	{
		if (!ent->client->sess.entListIDs[i])
			continue;

		trap->SendServerCommand(ent - g_entities, va("print \"^2# ^7%i ^2Name: ^7%s\n\"", ent->client->sess.entListIDs[i], ent->client->sess.entListNames[i]));
	}
	return;
}

//Thanks to Raz0r for this command
void Cmd_Invisible_F(gentity_t *ent)
{
	if (!G_CheckAdmin(ent, ADMIN_INVISIBLE))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (ent->client->sess.isInvisible)
	{
		ent->client->sess.isInvisible = qfalse;
		ent->r.contents = CONTENTS_SOLID;
		ent->clipmask = CONTENTS_SOLID | CONTENTS_BODY;
		ent->s.trickedentindex = 0; ent->s.trickedentindex2 = 0; //This is entirely for client-side prediction. Please fix me.
		trap->SendServerCommand(ent - g_entities, "print \"^2You are no longer invisible.\n\"");
	}
	else
	{
		ent->client->sess.isInvisible = qtrue;
		ent->r.contents = CONTENTS_BODY;
		ent->clipmask = 267009/*CONTENTS_SOLID*/;
		{//This is *entirely* for client-side prediction. Do not rely on it. Please fix me.
			ent->client->ps.fd.forceMindtrickTargetIndex = ~(1 << ent - g_entities);
			ent->client->ps.fd.forceMindtrickTargetIndex2 = ~(1 << ent - g_entities);
		}
		trap->SendServerCommand(ent - g_entities, "print \"^2You are now invisible.\n\"");
	}
	return;
}

void Cmd_Disguise_F(gentity_t *ent)
{
	if (!ent->client->sess.isAdmin)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (!ent->client->sess.isDisguised)
	{
		ent->client->sess.isDisguised = qtrue;
		trap->SendServerCommand(ent - g_entities, "print \"^2You are now ^7disguised\n\"");
	}
	else
	{
		ent->client->sess.isDisguised = qfalse;
		trap->SendServerCommand(ent - g_entities, "print \"^2You are now ^7no longer disguised\n\"");
	}
	return;
}

void Cmd_CheckStats_F(gentity_t *ent)
{
	char cmdTarget[256];
	int clientid = -1;
	int i;

	if (!G_CheckAdmin(ent, ADMIN_CHECKSTATS))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^2Command Usage: /amcheckstats <name/clientid>\n\""));
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

	trap->SendServerCommand(ent - g_entities, va("print \"^2Force Stats for ^7%s^2:\n\"", g_entities[clientid].client->pers.netname));
	//It's NUM_FORCE_POWERS-1 because force powers go from 0-17, not 1-18
	for (i = 0; i < NUM_FORCE_POWERS - 1; i++)
	{
		if (FPTable[i].id != -1 && FPTable[i].name[0])
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^7%s ^2- Level: ^7%i\n\"", FPTable[i].name, g_entities[clientid].client->ps.fd.forcePowerLevel[i]));
		}

	}
	return;
}

void Cmd_ToBlack_F(gentity_t *ent)
{
	qboolean fadeToBlack, toBlackImmediately;
	char parameter[7];

	if (!G_CheckAdmin(ent, ADMIN_TOBLACK))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}
	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amToBlack <fade/nofade> Fading takes 10 seconds.\n\"");
		return;
	}

	trap->Argv(1, parameter, sizeof(parameter));

	if (!Q_stricmpn(parameter, "fade", 4))
	{

		if (!ent->client->sess.fadeToBlack)
			fadeToBlack = qtrue;
		else
			fadeToBlack = qfalse;

		if (fadeToBlack)
			trap->SendServerCommand(ent - g_entities, "print \"^2All players' screens now fade to black...\n\"");
		else
			trap->SendServerCommand(ent - g_entities, "print \"^2All players' screens now come back to normal.\n\"");

		ent->client->sess.fadeToBlack = fadeToBlack;

		trap->SendServerCommand(-1, "fadeToBlack");
	}
	else if (!Q_stricmpn(parameter, "nofade", 6))
	{
		if (!ent->client->sess.toBlackImmediately)
			toBlackImmediately = qtrue;
		else
			toBlackImmediately = qfalse;

		if (toBlackImmediately)
			trap->SendServerCommand(ent - g_entities, "print \"^2All players' screens now immediately turn black...\n\"");
		else
			trap->SendServerCommand(ent - g_entities, "print \"^2All players screens now come back to normal.\n\"");

		ent->client->sess.toBlackImmediately = toBlackImmediately;

		trap->SendServerCommand(-1, "toBlackImmediately");
	}
	else
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amToBlack <fade/nofade> Fading takes 10 seconds.\n\"");
		return;
	}

	return;
}

/*
void Cmd_Timer_F( gentity_t *ent )
{
char secondsTemp[10], color[10];
int seconds;
qboolean isMyTeam;

if( !G_CheckAdmin( ent, ADMIN_TIMER ) )
{
trap->SendServerCommand(ent-g_entities, va("print \"^1You are not allowed to use this command.\n\""));
return;
}

if ( trap->Argc() != 3 )
{
trap->SendServerCommand(ent-g_entities, "print \"^2Command Usage: /amTimer <seconds> <red/green>\nRed or green determines the color of the numbers displayed.\n\"" );
return;
}

trap->Argv( 1, secondsTemp, sizeof ( secondsTemp ) );
seconds = atoi( secondsTemp );
trap->Argv( 2, color, sizeof( color ) );

if ( !Q_stricmp( color, "red" ) )
isMyTeam = qtrue;
else if ( !Q_stricmp( color, "green" ) )
isMyTeam = qfalse;
else
{
trap->SendServerCommand(ent-g_entities, "print \"^2Command Usage: /amTimer <seconds> <red/green>\nRed or green determines the color of the numbers displayed.\n\"" );
return;
}

trap->SendServerCommand( -1, va( "timer %i %i", seconds, isMyTeam ) );
trap->SendServerCommand( ent-g_entities, va( "print \"^2Timer colored ^7%s ^2with ^7%i ^2seconds started.\n\"", color, seconds ) );
return;
}
*/

void Cmd_LockSaberSetting_F(gentity_t *ent)
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	sqlite3_stmt *stmt;
	extern qboolean G_AdminControl(gentity_t *ent, gentity_t *target);
	char charName[256] = { 0 };
	int charID = 0, accountID = 0, clientID = 33, loggedIn = 0, i = 0;

	//The database is not connected. Please do so.
	rc = sqlite3_open((const char*)openrp_databasePath.string, &db);
	if (rc)
	{
		trap->Print("Can't open database: %s\n", sqlite3_errmsg(db));
		trap->SendServerCommand(ent - g_entities, "print \"^1The server's database is not connected.\n\"");
		sqlite3_close(db);
		return;
	}

	if (!G_CheckAdmin(ent, ADMIN_TRAININGSABER))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		sqlite3_close(db);
		return;
	}

	if (trap->Argc() < 2)
	{
		trap->SendServerCommand(ent - g_entities, "print \"^2Command Usage: /amLockSaberSetting <characterName>\n\"");
		sqlite3_close(db);
		return;
	}

	trap->Argv(1, charName, sizeof(charName));

	Q_StripColor(charName);
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

	if (loggedIn)
	{
		if (!G_AdminControl(ent, &g_entities[clientID]))
		{
			trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
			sqlite3_close(db);
			return;
		}

		if (g_entities[clientID].client->sess.trainingSaberLocked)
		{
			rc = sqlite3_exec(db, va("UPDATE Characters set TrainingSaberLocked='0' WHERE CharID='%i'", charID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}

			g_entities[clientID].client->sess.trainingSaberLocked = qfalse;
			g_entities[clientID].client->sess.trainingSaber = qfalse;
			trap->SendServerCommand(ent - g_entities, va("print \"^7%s^2's lightsaber has been unlocked its training setting.\n\"", g_entities[clientID].client->pers.netname));

			trap->SendServerCommand(clientID, "print \"^2Your lightsaber has been unlocked from its training setting by an admin.\n\"");
			trap->SendServerCommand(clientID, "cp \"^2Your lightsaber has been unlocked from its training setting by an admin.\n\"");
		}
		else
		{
			if (!g_entities[clientID].client->sess.trainingSaber)
				g_entities[clientID].client->sess.trainingSaber = qtrue;

			rc = sqlite3_exec(db, va("UPDATE Characters set TrainingSaberLocked='1' WHERE CharID='%i'", charID), 0, 0, &zErrMsg);
			if (rc != SQLITE_OK)
			{
				trap->Print("SQL error: %s\n", zErrMsg);
				sqlite3_free(zErrMsg);
				sqlite3_close(db);
				return;
			}
			g_entities[clientID].client->sess.trainingSaberLocked = qtrue;
			trap->SendServerCommand(ent - g_entities, va("print \"^7%s^2's lightsaber has been locked into its training setting.\n\"", g_entities[clientID].client->pers.netname));

			trap->SendServerCommand(clientID, "print \"^2Your lightsaber has been locked into its training setting by an admin.\n\"");
			trap->SendServerCommand(clientID, "cp \"^2Your lightsaber has been locked into its training setting by an admin.\n\"");
		}
	}
	sqlite3_close(db);
	return;
}

void Cmd_Alarm_F(gentity_t *ent)
{
	/*
	qboolean alarm;

	if ( !G_CheckAdmin( ent, ADMIN_ALARM ) )
	{
	trap->SendServerCommand(ent-g_entities, va("print \"^1You are not allowed to use this command.\n\""));
	return;
	}

	if ( !ent->client->sess.alarm )
	alarm = qtrue;
	else
	alarm = qfalse;

	if ( alarm )
	trap->SendServerCommand( ent-g_entities, "print \"^2Alarm activated.\n\"" );
	else
	trap->SendServerCommand( ent-g_entities, "print \"^2Alarm deactivated.\n\"" );

	ent->client->sess.alarm = alarm;

	trap->SendServerCommand( -1, "alarm" );

	return;
	*/
	return;
}

/*
void Cmd_AddFlag_F( gentity_t * ent )
{
	sqlite3 *db;
	char *zErrMsg = 0;
	sqlite3_stmt *stmt;
	int rc;
	char charName[256] = { 0 }, flag[64] = { 0 };
	int charID = 0, accountID = 0, clientID = 33, loggedIn = 0, i = 0;

	//The database is not connected. Please do so.
	rc = sqlite3_open( (const char*)openrp_databasePath.string, &db );
	if(rc)
	{
		trap->Print( "Can't open database: %s\n", sqlite3_errmsg( db ) );
		trap->SendServerCommand( ent-g_entities, "print \"^1The server's database is not connected.\n\"" );
		sqlite3_close( db );
		return;
	}

	if ( !G_CheckAdmin( ent, ADMIN_FLAGS ) )
	{
		trap->SendServerCommand(ent-g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		sqlite3_close( db );
		return;
	}

	if ( trap->Argc() < 2 )
	{
		trap->SendServerCommand( ent-g_entities, "print \"^2Command Usage: /amaddflag <characterName> <flag>\n\"" );
		sqlite3_close( db );
		return;
	}

	trap->Argv( 1, charName, sizeof( charName ) );

	Q_StripColor(charName);
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

	if( !charID )
	{
		trap->SendServerCommand( ent-g_entities, va( "print \"^1Character %s does not exist.\n\"", charName ) );
		trap->SendServerCommand( ent-g_entities, va( "cp \"^1Character %s does not exist.\n\"", charName ) );
		sqlite3_close( db );
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

	trap->Argv( 2, flag, sizeof( flag ) );

	sqlite3_close( db );
	return;
}
*/

//Credit to ClanMod for this cmd (modified for OpenRP)
void Cmd_Merc_F(gentity_t *ent)
{
	char cmdTarget[256];
	int clientid = -1;
	int i = 0;

	if (!G_CheckAdmin(ent, ADMIN_MERCEMP))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	//Mercing yourself
	if (trap->Argc() < 2 && !ent->client->sess.isMerc) //If the person who used the command did not specify a name, and if they are not currently a merc, then merc them.
	{
		//Give them every item.
		ent->client->ps.stats[STAT_HOLDABLE_ITEMS] |= (1 << HI_BINOCULARS) | (1 << HI_SEEKER) | (1 << HI_CLOAK) | (1 << HI_EWEB) | (1 << HI_SENTRY_GUN);
		//Take away saber and melee. We'll give it back in the next line along with the other weapons.
		ent->client->ps.stats[STAT_WEAPONS] &= ~(1 << WP_SABER) & ~(1 << WP_MELEE);
		//Give them every weapon.
		ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_SABER) | (1 << WP_MELEE) | (1 << WP_BLASTER) | (1 << WP_DISRUPTOR) | (1 << WP_BOWCASTER)
			| (1 << WP_REPEATER) | (1 << WP_DEMP2) | (1 << WP_FLECHETTE) | (1 << WP_ROCKET_LAUNCHER) | (1 << WP_THERMAL) | (1 << WP_DET_PACK)
			| (1 << WP_BRYAR_OLD) | (1 << WP_CONCUSSION) | (1 << WP_TRIP_MINE) | (1 << WP_BRYAR_PISTOL);
		{
			for (i = 0; i < MAX_WEAPONS; i++)
				//Give them max ammo
				ent->client->ps.ammo[i] = 999;
		}

		ent->client->ps.weapon = WP_BLASTER; //Switch their active weapon to the E-11.

		ent->client->sess.isMerc = qtrue;

		trap->SendServerCommand(ent - g_entities, va("print \"^2You have been been made a merc.\n\""));
		G_LogPrintf("Merc admin command executed by %s on themself.\n", ent->client->pers.netname);
		return;
	}

	//Unmercing yourself
	if (trap->Argc() < 2 && ent->client->sess.isMerc) //If the user is already a merc and they use the command again on themself, then unmerc them.
	{
		//Take away every item.
		ent->client->ps.eFlags &= ~EF_SEEKERDRONE;
		ent->client->ps.stats[STAT_HOLDABLE_ITEMS] &= ~(1 << HI_SEEKER) & ~(1 << HI_BINOCULARS) & ~(1 << HI_SENTRY_GUN) & ~(1 << HI_EWEB) & ~(1 << HI_CLOAK);
		//Take away every weapon.
		ent->client->ps.stats[STAT_WEAPONS] &= ~(1 << WP_STUN_BATON) & ~(1 << WP_BLASTER) & ~(1 << WP_DISRUPTOR) & ~(1 << WP_BOWCASTER)
			& ~(1 << WP_REPEATER) & ~(1 << WP_DEMP2) & ~(1 << WP_FLECHETTE) & ~(1 << WP_ROCKET_LAUNCHER) & ~(1 << WP_THERMAL) & ~(1 << WP_DET_PACK)
			& ~(1 << WP_BRYAR_OLD) & ~(1 << WP_CONCUSSION) & ~(1 << WP_TRIP_MINE) & ~(1 << WP_BRYAR_PISTOL);

		//Give them melee and saber. They should already have these but this seems to prevent a bug with them not being switched to the correct active weapon.
		//ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_MELEE) | (1 << WP_SABER); 

		ent->client->ps.weapon = WP_SABER; //Switch their active weapon to the saber.

		ent->client->sess.isMerc = qfalse;

		trap->SendServerCommand(ent - g_entities, va("print \"^2You are no longer a merc.\n\""));
		G_LogPrintf("Unmerc admin command executed by %s on themself.\n", ent->client->pers.netname);
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

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	if (!g_entities[clientid].client->sess.isMerc) //If the target is not currently a merc, then merc them.
	{
		//Give them every item.
		g_entities[clientid].client->ps.stats[STAT_HOLDABLE_ITEMS] |= (1 << HI_BINOCULARS) | (1 << HI_SEEKER) | (1 << HI_CLOAK) | (1 << HI_EWEB) | (1 << HI_SENTRY_GUN);
		//Take away saber and melee. We'll give it back in the next line along with the other weapons.
		g_entities[clientid].client->ps.stats[STAT_WEAPONS] &= ~(1 << WP_SABER) & ~(1 << WP_MELEE);
		//Give them every weapon.
		g_entities[clientid].client->ps.stats[STAT_WEAPONS] |= (1 << WP_SABER) | (1 << WP_MELEE) | (1 << WP_BLASTER) | (1 << WP_DISRUPTOR) | (1 << WP_BOWCASTER)
			| (1 << WP_REPEATER) | (1 << WP_DEMP2) | (1 << WP_FLECHETTE) | (1 << WP_ROCKET_LAUNCHER) | (1 << WP_THERMAL) | (1 << WP_DET_PACK)
			| (1 << WP_BRYAR_OLD) | (1 << WP_CONCUSSION) | (1 << WP_TRIP_MINE) | (1 << WP_BRYAR_PISTOL);

		{
			for (i = 0; i < MAX_WEAPONS; i++)
				//Give them max ammo
				g_entities[clientid].client->ps.ammo[i] = 999;
		}

		g_entities[clientid].client->ps.weapon = WP_BLASTER; //Switch their active weapon to the E-11.

		g_entities[clientid].client->sess.isMerc = qtrue;

		trap->SendServerCommand(ent - g_entities, va("print \"^2Player ^7%s ^2is now a merc.\n\"", g_entities[clientid].client->pers.netname));
		trap->SendServerCommand(clientid, "print \"^2You have been made a merc.\n\"");
		trap->SendServerCommand(clientid, "cp \"^2You have been made a merc.\n\"");
		G_LogPrintf("Merc admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
		return;
	}

	if (g_entities[clientid].client->sess.isMerc) //If the target is currently a merc, then unmerc them.
	{
		//Take away every item.
		g_entities[clientid].client->ps.eFlags &= ~EF_SEEKERDRONE;
		g_entities[clientid].client->ps.stats[STAT_HOLDABLE_ITEMS] &= ~(1 << HI_SEEKER) & ~(1 << HI_BINOCULARS) & ~(1 << HI_SENTRY_GUN) & ~(1 << HI_EWEB) & ~(1 << HI_CLOAK);
		//Take away every weapon.
		g_entities[clientid].client->ps.stats[STAT_WEAPONS] &= ~(1 << WP_STUN_BATON) & ~(1 << WP_BLASTER) & ~(1 << WP_DISRUPTOR) & ~(1 << WP_BOWCASTER)
			& ~(1 << WP_REPEATER) & ~(1 << WP_DEMP2) & ~(1 << WP_FLECHETTE) & ~(1 << WP_ROCKET_LAUNCHER) & ~(1 << WP_THERMAL) & ~(1 << WP_DET_PACK)
			& ~(1 << WP_BRYAR_OLD) & ~(1 << WP_CONCUSSION) & ~(1 << WP_TRIP_MINE) & ~(1 << WP_BRYAR_PISTOL);

		//Give them melee and saber. They should already have these but this seems to prevent a bug with them not being switched to the correct active weapon.
		//tent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_MELEE) | (1 << WP_SABER); 

		g_entities[clientid].client->ps.weapon = WP_SABER; //Switch their active weapon to the saber.

		g_entities[clientid].client->sess.isMerc = qfalse;

		trap->SendServerCommand(ent - g_entities, va("print \"^2Player ^7%s ^2is no longer a merc.\n\"", g_entities[clientid].client->pers.netname));
		trap->SendServerCommand(clientid, "print \"^2You are no longer a merc.\n\"");
		trap->SendServerCommand(clientid, "cp \"^2You are no longer a merc.\"");
		G_LogPrintf("Unmerc admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
		return;
	}
	return;
}

void SaveForcePowers(gentity_t *ent)
{
	int i = 0;

	ent->client->sess.SavedPowers = ent->client->ps.fd.forcePowersKnown;

	while (i < NUM_FORCE_POWERS)
	{
		ent->client->sess.SavedPowersLevels[i] = ent->client->ps.fd.forcePowerLevel[i];
		i++;
	}
}

void RestoreForcePowers(gentity_t *ent)
{
	int i = 0;


	while (i < NUM_FORCE_POWERS)
	{
		ent->client->ps.fd.forcePowerLevel[i] = ent->client->sess.SavedPowersLevels[i];
		i++;
	}

	ent->client->ps.fd.forcePowersKnown = ent->client->sess.SavedPowers;

}

//Credit to ClanMod for this cmd (modified for OpenRP)
void Cmd_Empower_F(gentity_t *ent)
{
	char cmdTarget[256];
	int clientid = -1;
	int i = 0;

	if (!G_CheckAdmin(ent, ADMIN_MERCEMP))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	//Emping yourself
	if (trap->Argc() < 2 && !ent->client->sess.isEmp) //If the person who used the command did not specify a name, and if they are not currently a empowered, then empower them.
	{
		SaveForcePowers(ent);

		while (i < NUM_FORCE_POWERS)
		{
			ent->client->ps.fd.forcePowerLevel[i] = 3;
			i++;
		}

		//Give every force power
		ent->client->ps.fd.forcePowersKnown |= (1 << FP_HEAL) | (1 << FP_LEVITATION) | (1 << FP_SPEED) | (1 << FP_PUSH) | (1 << FP_PULL)
			| (1 << FP_TELEPATHY) | (1 << FP_GRIP) | (1 << FP_LIGHTNING) | (1 << FP_RAGE) | (1 << FP_PROTECT) | (1 << FP_ABSORB)
			| (1 << FP_TEAM_HEAL) | (1 << FP_TEAM_FORCE) | (1 << FP_DRAIN) | (1 << FP_SEE) | (1 << FP_SABER_OFFENSE) | (1 << FP_SABER_DEFENSE) | (1 << FP_SABERTHROW);

		ent->client->ps.eFlags |= EF_BODYPUSH; //Thanks to ClanMod for this
		ent->client->sess.isEmp = qtrue;

		trap->SendServerCommand(ent - g_entities, va("print \"^2You have been been empowered.\n\""));
		G_LogPrintf("Empower admin command executed by %s on themself.\n", ent->client->pers.netname);
		return;
	}

	//Unempowering yourself
	if (trap->Argc() < 2 && ent->client->sess.isEmp) //If the user is already empowered and they use the command again on themself, then unempower them.
	{
		//Take away every force power
		ent->client->ps.stats[STAT_WEAPONS] &= ~(1 << FP_HEAL) & ~(1 << FP_LEVITATION) & ~(1 << FP_SPEED) & ~(1 << FP_PUSH) & ~(1 << FP_PULL)
			& ~(1 << FP_TELEPATHY) & ~(1 << FP_GRIP) & ~(1 << FP_LIGHTNING) & ~(1 << FP_RAGE) & ~(1 << FP_PROTECT) & ~(1 << FP_ABSORB)
			& ~(1 << FP_TEAM_HEAL) & ~(1 << FP_TEAM_FORCE) & ~(1 << FP_DRAIN) & ~(1 << FP_SEE) & ~(1 << FP_SABER_OFFENSE) & ~(1 << FP_SABER_DEFENSE) & ~(1 << FP_SABERTHROW);

		ent->client->ps.eFlags &= ~EF_BODYPUSH;

		RestoreForcePowers(ent);

		if (ent->client->ps.fd.forcePowerLevel[FP_SABER_OFFENSE] >= 1)
			ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_SABER) | (1 << WP_MELEE);
		else
			ent->client->ps.stats[STAT_WEAPONS] |= (1 << WP_MELEE);

		ent->client->sess.isEmp = qfalse;

		trap->SendServerCommand(ent - g_entities, va("print \"^2You are no longer empowered.\n\""));
		G_LogPrintf("Unempower admin command executed by %s on themself.\n", ent->client->pers.netname);
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

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	if (!G_AdminControl(ent, &g_entities[clientid]))
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You can't use this command on them. They are a higher admin level than you.\n\""));
		return;
	}

	if (!g_entities[clientid].client->sess.isEmp) //If the target is not currently empowered, then empower them.
	{
		SaveForcePowers(&g_entities[clientid]);

		while (i < NUM_FORCE_POWERS)
		{
			g_entities[clientid].client->ps.fd.forcePowerLevel[i] = 3;
			i++;
		}

		//Give every force power
		g_entities[clientid].client->ps.fd.forcePowersKnown |= (1 << FP_HEAL) | (1 << FP_LEVITATION) | (1 << FP_SPEED) | (1 << FP_PUSH) | (1 << FP_PULL)
			| (1 << FP_TELEPATHY) | (1 << FP_GRIP) | (1 << FP_LIGHTNING) | (1 << FP_RAGE) | (1 << FP_PROTECT) | (1 << FP_ABSORB)
			| (1 << FP_TEAM_HEAL) | (1 << FP_TEAM_FORCE) | (1 << FP_DRAIN) | (1 << FP_SEE) | (1 << FP_SABER_OFFENSE) | (1 << FP_SABER_DEFENSE) | (1 << FP_SABERTHROW);

		g_entities[clientid].client->ps.eFlags |= EF_BODYPUSH;
		g_entities[clientid].client->sess.isEmp = qtrue;

		trap->SendServerCommand(ent - g_entities, va("print \"^2Player ^7%s ^2has been empowered.\n\"", g_entities[clientid].client->pers.netname));
		trap->SendServerCommand(clientid, "print \"^2You have been empowered.\n\"");
		trap->SendServerCommand(clientid, "cp \"^2You have been empowered.\"");
		G_LogPrintf("Empower admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
		return;
	}

	if (g_entities[clientid].client->sess.isEmp) //If the target is currently empowered, then unempower them.
	{

		//Take away every force power
		g_entities[clientid].client->ps.fd.forcePowersKnown &= ~(1 << FP_HEAL) & ~(1 << FP_LEVITATION) & ~(1 << FP_SPEED) & ~(1 << FP_PUSH) & ~(1 << FP_PULL)
			& ~(1 << FP_TELEPATHY) & ~(1 << FP_GRIP) & ~(1 << FP_LIGHTNING) & ~(1 << FP_RAGE) & ~(1 << FP_PROTECT) & ~(1 << FP_ABSORB)
			& ~(1 << FP_TEAM_HEAL) & ~(1 << FP_TEAM_FORCE) & ~(1 << FP_DRAIN) & ~(1 << FP_SEE) & ~(1 << FP_SABER_OFFENSE) & ~(1 << FP_SABER_DEFENSE) & ~(1 << FP_SABERTHROW);

		g_entities[clientid].client->ps.eFlags &= ~EF_BODYPUSH;

		RestoreForcePowers(&g_entities[clientid]);

		if (g_entities[clientid].client->ps.fd.forcePowerLevel[FP_SABER_OFFENSE] >= 1)
			g_entities[clientid].client->ps.stats[STAT_WEAPONS] |= (1 << WP_SABER) | (1 << WP_MELEE);
		else
			g_entities[clientid].client->ps.stats[STAT_WEAPONS] |= (1 << WP_MELEE);

		g_entities[clientid].client->sess.isEmp = qfalse;

		trap->SendServerCommand(ent - g_entities, va("print \"^2Player ^7%s ^2has been unempowered.\n\"", g_entities[clientid].client->pers.netname));
		trap->SendServerCommand(clientid, "print \"^2You are no longer empowered.\n\"");
		trap->SendServerCommand(clientid, "cp \"^2You are no longer empowered.\"");
		G_LogPrintf("Unempower admin command executed by %s on %s.\n", ent->client->pers.netname, g_entities[clientid].client->pers.netname);
		return;
	}
	return;
}

void Cmd_FrequencyCheck_F(gentity_t *ent)
{
	int i = 0;
	char radioStatus[11] = { 0 };
	gentity_t	*client;

	if (!ent->client->sess.isAdmin)
	{
		trap->SendServerCommand(ent - g_entities, va("print \"^1You are not allowed to use this command.\n\""));
		return;
	}

	trap->SendServerCommand(ent - g_entities, "print \"^2Frequency Check:\n\n\"");
	for (i = 0; i < level.maxclients; i++)
	{
		client = g_entities + i;
		if (client->inuse && client->client && level.clients[i].pers.connected == CON_CONNECTED)
		{
			if (level.clients[i].sess.radioFrequency == -1)
			{
				trap->SendServerCommand(ent - g_entities, va("print \"^7%s - No Frequency Set\n\"", level.clients[i].pers.netname, level.clients[i].sess.radioFrequency));
			}
			else
			{
				if (level.clients[i].sess.radioOn)
					Q_strncpyz(radioStatus, "Radio: On", sizeof(radioStatus));
				else
					Q_strncpyz(radioStatus, "Radio: Off", sizeof(radioStatus));
				trap->SendServerCommand(ent - g_entities, va("print \"^7%s ^2- ^7%i ^2- ^7%s\n\"", level.clients[i].pers.netname, level.clients[i].sess.radioFrequency, radioStatus));
			}
		}
	}
	return;
}