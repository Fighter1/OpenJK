
#ifdef XCVAR_PROTO
	#define XCVAR_DEF( name, defVal, update, flags, announce ) extern vmCvar_t name;
#endif

#ifdef XCVAR_DECL
	#define XCVAR_DEF( name, defVal, update, flags, announce ) vmCvar_t name;
#endif

#ifdef XCVAR_LIST
	#define XCVAR_DEF( name, defVal, update, flags, announce ) { & name , #name , defVal , update , flags , announce },
#endif

//Begin OpenRP cvars

//OpenRP Stuff
XCVAR_DEF(openrp_maxWarnings, "3", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_admin1Bitvalues, "4294967295", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_admin2Bitvalues, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_admin3Bitvalues, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_admin4Bitvalues, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_admin5Bitvalues, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_admin6Bitvalues, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_admin7Bitvalues, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_admin8Bitvalues, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_admin9Bitvalues, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_admin10Bitvalues, "1", NULL, CVAR_ARCHIVE, qfalse)

XCVAR_DEF(openrp_adminControl, "1", NULL, CVAR_ARCHIVE, qfalse)

//Server's own website
XCVAR_DEF(openrp_website, "", NULL, CVAR_ARCHIVE, qfalse)

//Shop stuff
//Thanks to Kavar for making the descriptions.
XCVAR_DEF(openrp_e11BuyCost, "400", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_e11SellCost, "200", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_e11Level, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_e11Description, "A BlasTech E-11 rifle.  Despite its infamous inaccuracy, it remains a favorite amongst mercenaries and infantry alike due to its light weight and notable power.", NULL, CVAR_ARCHIVE, qfalse)

XCVAR_DEF(openrp_pistolBuyCost, "250", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_pistolSellCost, "125", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_pistolLevel, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_pistolDescription, "A BlasTech DL-44 blaster pistol. Its accuracy, power, and ability to charge shots make it a favorite amongst smugglers, bounty hunters, and even military forces.", NULL, CVAR_ARCHIVE, qfalse)

XCVAR_DEF(openrp_disruptorBuyCost, "600", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_disruptorSellCost, "300", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_disruptorLevel, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_disruptorDescription, "A Tenloss DXR-6 disruptor rifle. An outlaw weapon known for its ability to completely disintegrate its targets, which makes even a glancing blow a serious one. Fitted with a zoom scope and optimized for sniping and long range engagements. ", NULL, CVAR_ARCHIVE, qfalse)

XCVAR_DEF(openrp_bowcasterBuyCost, "550", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_bowcasterSellCost, "275", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_bowcasterLevel, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_bowcasterDescription, "A Wookiee bowcaster. Modified for use by all species, the bowcaster features impressive stopping power with the ability to fire single, spread, or ricochet shots.", NULL, CVAR_ARCHIVE, qfalse)

XCVAR_DEF(openrp_repeaterBuyCost, "800", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_repeaterSellCost, "400", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_repeaterLevel, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_repeaterDescription, "An Imperial Heavy Repeater. Essentially a slugthrower which boasts an incredibly high rate of fire, as well as an attached concussion grenade launcher.", NULL, CVAR_ARCHIVE, qfalse)

XCVAR_DEF(openrp_demp2BuyCost, "700", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_demp2SellCost, "350", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_demp2Level, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_demp2Description, "A Destructive Electromagnetic Pulse 2 (DEMP2) ion carbine. Effective against electrical sources and organics alike, the DEMP2 features standard fire as well as a secondary fire mode which causes an explosion on impact and extends the shot's range.", NULL, CVAR_ARCHIVE, qfalse)

XCVAR_DEF(openrp_flechetteBuyCost, "850", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_flechetteSellCost, "425", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_flechetteLevel, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_flechetteDescription, "A Golan Arms FC-1 flechette launcher. While suited for close range combat, the FC-1 also carries proximity grenades for any range.", NULL, CVAR_ARCHIVE, qfalse)

XCVAR_DEF(openrp_rocketBuyCost, "10000", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_rocketSellCost, "5000", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_rocketLevel, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_rocketDescription, "A Merr-Sonn PLX-2M portable missile launcher. Features manual and lock-on firing modes.", NULL, CVAR_ARCHIVE, qfalse)

XCVAR_DEF(openrp_concussionBuyCost, "8000", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_concussionSellCost, "4000", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_concussionLevel, "1", NULL, CVAR_ARCHIVE, qfalse)
XCVAR_DEF(openrp_concussionDescription, "A Stouker concussion rifle, originally created by the Sissk. Able to fire concussive blasts as well as a secondary, more focused blast.", NULL, CVAR_ARCHIVE, qfalse)

//[LF - BlackNames]
//Toggles allowance of black names
XCVAR_DEF(g_allowBlackNames, "1", NULL, CVAR_ARCHIVE, qtrue)
//[/LF - BlackNames]

//XCVAR_DEF( openrp_jetpackFuel,	"1",	NULL,			CVAR_ARCHIVE,	qfalse )
//XCVAR_DEF( openrp_cloakFuel,		"1",	NULL,			CVAR_ARCHIVE,	qfalse )

XCVAR_DEF(openrp_showRenames, "1", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(openrp_databasePath, "OpenRP/openrpdatabase.db", NULL, CVAR_ARCHIVE, qfalse)

XCVAR_DEF(openrp_DistanceBasedChat, "1", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(openrp_forceSensitiveBuyGuns, "1", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(g_pistol_velocity, "1600", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_pistol_damage, "10", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(g_e11_velocity, "2300", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_e11_damage, "20", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(g_disruptor_damage, "30", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_disruptor_altdamage, "100", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(g_bowcaster_damage, "50", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_bowcaster_velocity, "1300", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(g_repeater_damage, "14", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_repeater_velocity, "1600", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_repeater_altdamage, "60", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_repeater_altsplashdamage, "60", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_repeater_altvelocity, "1100", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(g_demp2_damage, "35", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_demp2_velocity, "1800", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_demp2_altdamage, "8", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(g_flechette_damage, "12", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_flechette_velocity, "3500", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_flechette_altdamage, "60", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_fletchette_altsplashdamage, "60", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(g_rocket_velocity, "900", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_rocket_damage, "100", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_rocketsplash_damage, "100", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(g_concussion_velocity, "3000", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_concussion_damage, "75", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_concussion_altdamage, "25", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(g_stunbaton_damage, "20", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_stunbaton_altdamage, "20", NULL, CVAR_ARCHIVE, qtrue)

XCVAR_DEF(g_meleeswing1_damage, "10", NULL, CVAR_ARCHIVE, qtrue)
XCVAR_DEF(g_meleeswing2_damage, "12", NULL, CVAR_ARCHIVE, qtrue)

//End OpenRP cvars

XCVAR_DEF( bg_fighterAltControl,		"0",			NULL,				CVAR_SYSTEMINFO,								qtrue )
XCVAR_DEF( capturelimit,				"8",			NULL,				CVAR_SERVERINFO|CVAR_ARCHIVE|CVAR_NORESTART,	qtrue )
XCVAR_DEF( com_optvehtrace,				"0",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( d_altRoutes,					"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( d_asynchronousGroupAI,		"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( d_break,						"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( d_JediAI,					"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( d_noGroupAI,					"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( d_noroam,					"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( d_npcai,						"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( d_npcaiming,					"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( d_npcfreeze,					"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( d_noIntermissionWait,		"0",			NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( d_patched,					"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( d_perPlayerGhoul2,			"0",			NULL,				CVAR_CHEAT,										qtrue )
XCVAR_DEF( d_powerDuelPrint,			"0",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( d_projectileGhoul2Collision,	"1",			NULL,				CVAR_CHEAT,										qtrue )
XCVAR_DEF( d_saberAlwaysBoxTrace,		"0",			NULL,				CVAR_CHEAT,										qtrue )
XCVAR_DEF( d_saberBoxTraceSize,			"0",			NULL,				CVAR_CHEAT,										qtrue )
XCVAR_DEF( d_saberCombat,				"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( d_saberGhoul2Collision,		"1",			NULL,				CVAR_CHEAT,										qtrue )
XCVAR_DEF( d_saberInterpolate,			"0",			NULL,				CVAR_CHEAT,										qtrue )
XCVAR_DEF( d_saberKickTweak,			"1",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( d_saberSPStyleDamage,		"1",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( d_saberStanceDebug,			"0",			NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( d_siegeSeekerNPC,			"0",			NULL,				CVAR_CHEAT,										qtrue )
XCVAR_DEF( dedicated,					"0",			NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( developer,					"0",			NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( dmflags,						"0",			NULL,				CVAR_SERVERINFO|CVAR_ARCHIVE,					qtrue )
XCVAR_DEF( duel_fraglimit,				"10",			NULL,				CVAR_SERVERINFO|CVAR_ARCHIVE|CVAR_NORESTART,	qtrue )
XCVAR_DEF( fraglimit,					"20",			NULL,				CVAR_SERVERINFO|CVAR_ARCHIVE|CVAR_NORESTART,	qtrue )
XCVAR_DEF( g_adaptRespawn,				"1",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_allowDuelSuicide,			"1",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_allowHighPingDuelist,		"1",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_allowNPC,					"1",			NULL,				CVAR_CHEAT,										qtrue )
XCVAR_DEF( g_allowTeamVote,				"1",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_allowVote,					"-1",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_antiFakePlayer,			"1",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_armBreakage,				"0",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_austrian,					"0",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_autoMapCycle,				"0",			NULL,				CVAR_ARCHIVE|CVAR_NORESTART,					qtrue )
XCVAR_DEF( g_banIPs,					"",				NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_charRestrictRGB,			"1",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_duelWeaponDisable,			"1",			NULL,				CVAR_SERVERINFO|CVAR_ARCHIVE|CVAR_LATCH,		qtrue )
XCVAR_DEF( g_debugAlloc,				"0",			NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( g_debugDamage,				"0",			NULL,				CVAR_NONE,										qfalse )
//OpenRP - changed g_debugMelee to 1 to allow for kicks, throws, etc. with melee
XCVAR_DEF( g_debugMelee,				"1",			NULL,				CVAR_SERVERINFO,								qtrue )
XCVAR_DEF( g_debugMove,					"0",			NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( g_debugSaberLocks,			"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( g_debugServerSkel,			"0",			NULL,				CVAR_CHEAT,										qfalse )
#ifdef _DEBUG
XCVAR_DEF( g_disableServerG2,			"0",			NULL,				CVAR_NONE,										qtrue )
#endif
XCVAR_DEF( g_dismember,					"0",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_doWarmup,					"0",			NULL,				CVAR_NONE,										qtrue )
//XCVAR_DEF( g_engineModifications,		"1",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_ff_objectives,				"0",			NULL,				CVAR_CHEAT|CVAR_NORESTART,						qtrue )
XCVAR_DEF( g_filterBan,					"1",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_forceBasedTeams,			"0",			NULL,				CVAR_SERVERINFO|CVAR_ARCHIVE|CVAR_LATCH,		qfalse )
XCVAR_DEF( g_forceClientUpdateRate,		"250",			NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( g_forceDodge,				"1",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_forcePowerDisable,			"0",			NULL,				CVAR_SERVERINFO|CVAR_ARCHIVE|CVAR_LATCH,		qtrue )
XCVAR_DEF( g_forceRegenTime,			"200",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_forceRespawn,				"60",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_fraglimitVoteCorrection,	"1",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_friendlyFire,				"0",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_friendlySaber,				"0",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_g2TraceLod,				"3",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_gametype,					"0",			NULL,				CVAR_SERVERINFO|CVAR_LATCH,						qfalse )
XCVAR_DEF( g_gravity,					"800",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_inactivity,				"0",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_jediVmerc,					"0",			NULL,				CVAR_SERVERINFO|CVAR_LATCH|CVAR_ARCHIVE,		qtrue )
XCVAR_DEF( g_knockback,					"1000",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_locationBasedDamage,		"1",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_log,						"games.log",	NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_logClientInfo,				"0",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_logSync,					"0",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_maxConnPerIP,				"3",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_maxForceRank,				"7",			NULL,				CVAR_SERVERINFO|CVAR_ARCHIVE|CVAR_LATCH,		qfalse )
XCVAR_DEF( g_maxGameClients,			"0",			NULL,				CVAR_SERVERINFO|CVAR_LATCH|CVAR_ARCHIVE,		qfalse )
XCVAR_DEF( g_maxHolocronCarry,			"3",			NULL,				CVAR_LATCH,										qfalse )
XCVAR_DEF( g_motd,						"",				NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( g_needpass,					"0",			NULL,				CVAR_SERVERINFO|CVAR_ROM,						qfalse )
XCVAR_DEF( g_noSpecMove,				"0",			NULL,				CVAR_SERVERINFO,								qtrue )
XCVAR_DEF( g_npcspskill,				"0",			NULL,				CVAR_ARCHIVE|CVAR_INTERNAL,						qfalse )
XCVAR_DEF( g_password,					"",				NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( g_powerDuelEndHealth,		"90",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_powerDuelStartHealth,		"150",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_privateDuel,				"1",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_randFix,					"1",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_restarted,					"0",			NULL,				CVAR_ROM,										qfalse )
XCVAR_DEF( g_saberBladeFaces,			"1",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_saberDamageScale,			"1",			NULL,				CVAR_ARCHIVE,									qtrue )
#ifdef DEBUG_SABER_BOX
XCVAR_DEF( g_saberDebugBox,				"0",			NULL,				CVAR_CHEAT,										qfalse )
#endif
#ifndef FINAL_BUILD
XCVAR_DEF( g_saberDebugPrint,			"0",			NULL,				CVAR_CHEAT,										qfalse )
#endif
XCVAR_DEF( g_saberDmgDelay_Idle,		"350",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_saberDmgDelay_Wound,		"0",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_saberDmgVelocityScale,		"0",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_saberLockFactor,			"2",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_saberLocking,				"1",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_saberLockRandomNess,		"2",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_saberRealisticCombat,		"0",			NULL,				CVAR_CHEAT,										qfalse )
XCVAR_DEF( g_saberRestrictForce,		"0",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_saberTraceSaberFirst,		"0",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_saberWallDamageScale,		"0.4",			NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( g_securityLog,				"1",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_showDuelHealths,			"0",			NULL,				CVAR_SERVERINFO,								qfalse )
XCVAR_DEF( g_siegeRespawn,				"20",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_siegeTeam1,				"none",			NULL,				CVAR_ARCHIVE|CVAR_SERVERINFO,					qfalse )
XCVAR_DEF( g_siegeTeam2,				"none",			NULL,				CVAR_ARCHIVE|CVAR_SERVERINFO,					qfalse )
XCVAR_DEF( g_siegeTeamSwitch,			"1",			NULL,				CVAR_SERVERINFO|CVAR_ARCHIVE,					qfalse )
XCVAR_DEF( g_slowmoDuelEnd,				"0",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_smoothClients,				"1",			NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( g_spawnInvulnerability,		"3000",			NULL,				CVAR_ARCHIVE,									qtrue )
XCVAR_DEF( g_speed,						"250",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_statLog,					"0",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_statLogFile,				"statlog.log",	NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_stepSlideFix,				"1",			NULL,				CVAR_SERVERINFO,								qtrue )
XCVAR_DEF( g_synchronousClients,		"0",			NULL,				CVAR_SYSTEMINFO,								qfalse )
XCVAR_DEF( g_teamAutoJoin,				"0",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_teamForceBalance,			"0",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_timeouttospec,				"70",			NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_userinfoValidate,			"25165823",		NULL,				CVAR_ARCHIVE,									qfalse )
XCVAR_DEF( g_useWhileThrowing,			"1",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( g_voteDelay,					"3000",			NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( g_warmup,					"20",			NULL,				CVAR_ARCHIVE,									qtrue )
//OpenRP -  //Note: It's recommended to keep this at its current value(changed from default of 0) because of OpenRP's shop system.
XCVAR_DEF( g_weaponDisable,				"524275",			NULL,				CVAR_SERVERINFO|CVAR_ARCHIVE|CVAR_LATCH,		qtrue )
XCVAR_DEF( g_weaponRespawn,				"5",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( gamedate,					__DATE__,		NULL,				CVAR_ROM,										qfalse )
XCVAR_DEF( gamename,					GAMEVERSION,	NULL,				CVAR_SERVERINFO|CVAR_ROM,						qfalse )
XCVAR_DEF( pmove_fixed,					"0",			NULL,				CVAR_SYSTEMINFO|CVAR_ARCHIVE,					qtrue )
XCVAR_DEF( pmove_float,					"0",			NULL,				CVAR_SYSTEMINFO|CVAR_ARCHIVE,					qtrue )
XCVAR_DEF( pmove_msec,					"8",			NULL,				CVAR_SYSTEMINFO|CVAR_ARCHIVE,					qtrue )
XCVAR_DEF( RMG,							"0",			NULL,				CVAR_NONE,										qtrue )
XCVAR_DEF( sv_cheats,					"1",			NULL,				CVAR_NONE,										qfalse )
XCVAR_DEF( sv_fps,						"40",			NULL,				CVAR_ARCHIVE|CVAR_SERVERINFO,					qtrue )
XCVAR_DEF( sv_maxclients,				"8",			NULL,				CVAR_SERVERINFO|CVAR_LATCH|CVAR_ARCHIVE,		qfalse )
XCVAR_DEF( timelimit,					"0",			NULL,				CVAR_SERVERINFO|CVAR_ARCHIVE|CVAR_NORESTART,	qtrue )

#undef XCVAR_DEF
