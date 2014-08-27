//Each number is previous number * 2, beginning at 1.
#define ADMIN_KICK					1 // /amkick
#define ADMIN_BAN					2 // /amban
#define ADMIN_TELEPORT				4 // /amtele
#define ADMIN_SEARCH				8 // /characterInfo and /accountInfo on another character or account
#define ADMIN_SLEEP					16 // /amsleep and /amunsleep
#define ADMIN_SILENCE				32 // /amsilence and /amunsilence
#define ADMIN_CHECKSTATS			64 // /amcheckstats
#define ADMIN_GRANTREMOVEADMIN		128 // Grant or remove admin
#define ADMIN_MAP					256 // /ammap
#define ADMIN_SKILLPOINTS			512 // skill point related
#define ADMIN_BUILD					1024 // building related commands
#define ADMIN_ANNOUNCE				2048 // /amannounce
#define ADMIN_WARN					4096 // /amwarn
#define ADMIN_ALLCHAT				8192 // /amAllChat
#define ADMIN_INVISIBLE			    16384 // /aminvisible
#define ADMIN_CHEATS				32768 // Cheat commands related
#define	ADMIN_ADMINWHOIS			65536 // /listadmins
#define ADMIN_MERCEMP				131072 // /ammerc and /amemp
#define ADMIN_SHAKE					262144 // /amshakescreen
#define ADMIN_EFFECT				524288 // /ameffect
#define ADMIN_FORCETEAM				1048576 // /amforceteam
#define ADMIN_WEATHER				2097152 // /amweather
#define ADMIN_STATUS				4194304 // /amstatus
#define ADMIN_RENAME				8388608 // /amrename
#define	ADMIN_FACTION				16777216 // Faction related
#define ADMIN_CREDITS				33554432 // Credits related
#define ADMIN_GUNITEM				67108864 // Gun or item related
#define ADMIN_AUDIO					134217728 // /amaudio
#define ADMIN_BOUNTY				268435456 // /bounty remove
#define ADMIN_TOBLACK				536870912 // /amtoblack
#define ADMIN_TRAININGSABER				1073741824 // Training saber related

void AddSpawnField(char *field, char *value);
void SP_fx_runner(gentity_t *ent);
void AddIP(char *str);
void Admin_Teleport(gentity_t *ent);
char *ConcatArgs(int start);
qboolean G_CallSpawn(gentity_t *ent);
void CheckAdmin(gentity_t * ent);
void AddSkill(gentity_t *self, int amount);
qboolean G_CheckAdmin(gentity_t *ent, int bitvalue);
qboolean G_AdminControl(gentity_t *ent, gentity_t *target);

void M_HolsterThoseSabers(gentity_t *ent);

void Cmd_amBan_F(gentity_t *ent);
void Cmd_amKick_F(gentity_t *ent);
void Cmd_amWarn_F(gentity_t *ent);
void Cmd_amTeleport_F(gentity_t *ent);
void Cmd_amAnnounce_F(gentity_t *ent);
void Cmd_amSilence_F(gentity_t *ent);
void Cmd_amUnSilence_F(gentity_t *ent);
void Cmd_amSleep_F(gentity_t *ent);
void Cmd_amUnsleep_F(gentity_t *ent);
void Cmd_amProtect_F(gentity_t *ent);
void Cmd_amEffect_F(gentity_t *ent);
void Cmd_amForceTeam_F(gentity_t *ent);
void Cmd_amMap_F(gentity_t *ent);
void G_RemoveWeather(void);
void Cmd_amWeather_F(gentity_t *ent);
void Cmd_amStatus_F(gentity_t *ent);
void uwRename(gentity_t *player, const char *newname);
void uw2Rename(gentity_t *player, const char *newname);
void Cmd_amRename_F(gentity_t *ent);
void Cmd_amSlap_F(gentity_t *ent);
void Cmd_GiveAdmin_F(gentity_t *ent);
void Cmd_SVGiveAdmin_F(void);
void Cmd_RemoveAdmin_F(gentity_t * ent);
void Cmd_SVRemoveAdmin_F(void);
void Cmd_GiveSkillPoints_F(gentity_t * ent);
void Cmd_FactionGenerateCredits_F(gentity_t * ent);
void Cmd_CreateFaction_F(gentity_t * ent);
void Cmd_SetFaction_F(gentity_t * ent);
void Cmd_FactionGenerate_F(gentity_t * ent);
void Cmd_GenerateCredits_F(gentity_t * ent);
void Cmd_amWeatherPlus_F(gentity_t *ent);
void Cmd_ShakeScreen_F(gentity_t * ent);
void Cmd_Audio_F(gentity_t * ent);
void Cmd_amTelemark_F(gentity_t * ent);
void Cmd_amOrigin_F(gentity_t * ent);
void Cmd_AdminChat_F(gentity_t *ent);
void Cmd_AllChat_F(gentity_t * ent);
void Cmd_amWarningList_F(gentity_t * ent);
void Cmd_SpawnEnt_F(gentity_t *ent);
void Cmd_RemoveEntity_F(gentity_t *ent);
void Cmd_ListEnts_F(gentity_t *ent);
void Cmd_Invisible_F(gentity_t *ent);
void Cmd_Disguise_F(gentity_t *ent);
void Cmd_CheckStats_F(gentity_t *ent);
void Cmd_ToBlack_F(gentity_t *ent);
//void Cmd_Timer_F( gentity_t *ent );
void Cmd_LockSaberSetting_F(gentity_t *ent);
void Cmd_Alarm_F(gentity_t *ent);
extern stringID_table_t FPTable[];
void Cmd_Merc_F(gentity_t *ent);
void SaveForcePowers(gentity_t *ent);
void RestoreForcePowers(gentity_t *ent);
void Cmd_Empower_F(gentity_t *ent);
void Cmd_amGiveGun_F(gentity_t *ent);
void Cmd_FrequencyCheck_F(gentity_t *ent);