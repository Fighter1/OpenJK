#include "../g_local.h"
#include "g_emote.h"
#include "g_admin.h"

//Credit to ClanMod for emote code

//This void is a mix of requirements to meet for the emote to work, and the custom emote animation itself.
void cm_TheEmote(int animation, gentity_t *ent, qboolean freeze)
{
	//OpenRP - Emote + /me support
	void G_Say(gentity_t *ent, gentity_t *target, int mode, const char *chatText);
	char *msg = ConcatArgs(1);

	if (ent->client->sess.spectatorState == SPECTATOR_FOLLOW || ent->client->sess.spectatorState == SPECTATOR_FREE) {
		return;
	}
	if (ent->client->ps.groundEntityNum == ENTITYNUM_NONE){
		return;
	}
	if (ent->client->ps.saberHolstered < 2){
		ent->client->ps.saberHolstered = 2;
	}
	if (BG_SaberInAttack(ent->client->ps.saberMove) || BG_SaberInSpecialAttack(ent->client->ps.saberMove) || ent->client->ps.saberLockTime){
		return;
	}
	if (freeze == qtrue)
	{
		if (ent->client->ps.forceDodgeAnim == animation)
		{
			//ent->client->emote_freeze=0;
			ent->client->ps.saberCanThrow = qtrue;
			ent->client->ps.forceDodgeAnim = 0;
			ent->client->ps.forceHandExtendTime = 0;
		}
		else
		{
			ent->client->ps.forceHandExtend = HANDEXTEND_TAUNT;
			ent->client->ps.forceDodgeAnim = animation;
			ent->client->ps.forceHandExtendTime = level.time + Q3_INFINITE;
			//ent->client->ps.persistant[PERS_REGEN] = 1;
			ent->client->ps.saberCanThrow = qfalse;
			//ent->client->emote_freeze=1;
		}
	}
	else
	{
		StandardSetBodyAnim(ent, animation, SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD | SETANIM_FLAG_HOLDLESS);
	}
	//OpenRP - Emote + /me support
	if (trap->Argc() >= 2)
		G_Say(ent, NULL, SAY_ME, msg);

	trap->SendServerCommand(ent - g_entities, "print \"^3Note: Use the emote's command again to get out of the emote.\n\"");
	return;
}