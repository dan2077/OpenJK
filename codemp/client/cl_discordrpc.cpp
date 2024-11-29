/*
Author: Blackwolf
Discord Integration with some usefull functions, have fun.
You need to link the static library also 'discord_rpc.lib'.
*/
#if defined DISCORD && !(_DEBUG)
#include <discord_rpc.h>
#include <discord_register.h>

#include "client/client.h"

//Author: Blackwolf
//This is my Application ID of the Discord Developer section, feel free to use it.
static const char* APPLICATION_ID = "459923453139746816";

typedef struct statusIcon_s {
	char *string;
	char *icon;
} statusIcon_t;

static statusIcon_t mapIcons[] = {
	{ "academy1",			"academy1"			},
	{ "academy2",			"academy2"			},
	{ "academy3",			"academy3"			},
	{ "academy4",			"academy4"			},
	{ "academy5",			"academy5"			},
	{ "academy6",			"academy6"			},
	{ "hoth2",				"hoth2"				},
	{ "hoth3",				"hoth3"				},
	{ "kor1",				"kor1"				},
	{ "kor2",				"kor2"				},
	{ "t1_danger",			"t1_danger1"		},
	{ "t1_fatal",			"t1_fatal"			},
	{ "t1_inter",			"t1_inter"			},
	{ "t1_rail",			"t1_rail"			},
	{ "t1_sour",			"t1_sour"			},
	{ "t1_surprise",		"t1_surprise"		},
	{ "t2_dpred",			"t2_dpred"			},
	{ "t2_rancor",			"t2_rancor"			},
	{ "t2_rogue",			"t2_rogue"			},
	{ "t2_trip",			"t2_trip"			},
	{ "t2_wedge",			"t2_wedge"			},
	{ "t3_bounty",			"t3_bounty"			},
	{ "t3_byss",			"t3_byss"			},
	{ "t3_hevil",			"t3_hevil"			},
	{ "t3_rift",			"t3_rift"			},
	{ "t3_stamp",			"t3_stamp"			},
	{ "taspir1",			"taspir1"			},
	{ "taspir2",			"taspir2"			},
	{ "vjun1",				"vjun1"				},
	{ "vjun2",				"vjun2"				},
	{ "vjun3",				"vjun3"				},
	{ "yavin1",				"yavin1",			},
	{ "yavin1b",			"yavin1b"			},
	{ "yavin2",				"yavin2"			},
	{ "mp/ctf1",			"ctf1"				},
	{ "mp/ctf2",			"ctf2"				},
	{ "mp/ctf3",			"ctf3"				},
	{ "mp/ctf4",			"ctf4"				},
	{ "mp/ctf5",			"ctf5"				},
	{ "mp/duel1",			"duel1"				},
	{ "mp/duel2",			"duel2"				},
	{ "mp/duel3",			"duel3"				},
	{ "mp/duel4",			"duel4"				},
	{ "mp/duel5",			"duel5"				},
	{ "mp/duel6",			"duel6"				},
	{ "mp/duel7",			"duel7"				},
	{ "mp/duel8",			"duel8"				},
	{ "mp/duel9",			"duel9"				},
	{ "mp/duel10",			"duel10"			},
	{ "mp/ffa1",			"ffa1"				},
	{ "mp/ffa2",			"ffa2"				},
	{ "mp/ffa3",			"ffa3"				},
	{ "mp/ffa4",			"ffa4"				},
	{ "mp/ffa5",			"ffa5"				},
	{ "mp/siege_desert",	"siege_desert"		},
	{ "mp/siege_hoth",		"siege_hoth"		},
	{ "mp/siege_korriban",	"siege_korriban"	},

	{ "mp/ctf_bonus1",		"ctf_bonus1"		},
	{ "mp/duel_bonus1",		"duel_bonus1"		},
	{ "mp/ffa_bonus1",		"ffa_bonus1"		},
	{ "mp/ffa_bonus2",		"ffa_bonus2"		},
	{ "mp/ffa_bonus3",		"ffa_bonus3"		},
	{ "mp/ffa_bonus4",		"ffa_bonus4"		},
	{ "mp/siege_destroyer",	"siege_destroyer"	},

	{ "racearena",			"racearena",		},
	{ "racearena_pro",		"racearena",		},
	{ "racepack1",			"racepack1",		},
	{ "racepack2",			"racepack2",		},
	{ "racepack3",			"racepack3",		},
	{ "racepack4",			"racepack4",		},
	{ "racepack5",			"racepack5",		},
	{ "racepack6",			"racepack6",		},
	{ "racepack7",			"racepack7",		},
}; static const size_t numMapIcons = ARRAY_LEN( mapIcons );

static statusIcon_t gameTypes[] = {
	{ "FFA",			"ffa"			},
	{ "Holocron",		"holocron"		},
	{ "Jedi Master",	"jedimaster"	},
	{ "Duel",			"duel"			},
	{ "PowerDuel",		"powerduel"		},
	{ "SP",				"ffa"			},
	{ "Team FFA",		"tffa"			},
	{ "Siege",			"siege"			},
	{ "CTF",			"ctf"			},
	{ "CTY",			"cty"			},
}; static const size_t numGameTypes = ARRAY_LEN(gameTypes);

char *ReturnMapName() {
	char *mapname = cl.discord.mapName;

	if ( cls.state == CA_DISCONNECTED || cls.state == CA_CONNECTING )
	{
		return "menu";
	}

	Q_StripColor( mapname );
	return Q_strlwr(mapname);
}

char *ReturnServerName() {
	char *servername = cl.discord.hostName;

	//Q_StripColor( servername );
	Q_CleanStr(servername);
	return servername;
}

char *ReturnMapIcon() {
	char *mapname = ReturnMapName();

	for ( int i = 0; i < numMapIcons; i++ )
	{
		if ( !Q_stricmp( mapname, mapIcons[i].string ) )
		{
			return mapIcons[i].icon;
		}
	}

	return "icon";
}

char *GetState()
{
	usercmd_t cmd;
	CL_GetUserCmd( cl.cmdNumber, &cmd );

	if (cls.state == CA_ACTIVE && cl.snap.valid) {
		if (cl_discordRichPresence->integer > 1 && (cmd.buttons & BUTTON_TALK))
			return "chatting";
		else if (cl_afkName || (cls.realtime - cls.afkTime) >= (5*60000)) //5 minutes?
			return "idle";
		else if (cl.snap.valid && ((cl.snap.ps.pm_flags & PMF_FOLLOW) || cl.snap.ps.pm_type == PM_SPECTATOR))
			return "spectating";
		else
			return "playing";
	}
	else if (cls.state > CA_DISCONNECTED && cls.state < CA_PRIMED) {
		return "connecting";
	}
	else if (cls.state <= CA_DISCONNECTED) {
		return "menu";
	}

	return "";
}

static char *GetGameType(qboolean imageKey)
{
	if (cl.discord.gametype > GT_FFA)
		return imageKey ? gameTypes[cl.discord.gametype].icon : gameTypes[cl.discord.gametype].string;

	return imageKey ? GetState() : gameTypes[cl.discord.gametype].string;
}

cvar_t *cl_discordRichPresenceSharePassword;
char *joinSecret()
{
	static char buff[128];

	memset(buff, 0, sizeof(buff));

	if (clc.demoplaying)
		return NULL;

	if ( cls.state >= CA_LOADING && cls.state <= CA_ACTIVE )
	{
		char *password = Cvar_VariableString("password");

		if (cl_discordRichPresenceSharePassword->integer && cl.discord.needPassword && strlen(password)) {
			Com_sprintf(buff, sizeof(buff), "%s %s %s", cls.servername, cl.discord.fs_game, password);
		}
		else {
			Com_sprintf(buff, sizeof(buff), "%s %s \"\"", cls.servername, cl.discord.fs_game);
		}
		return buff;
	}

	return NULL;
}

char *PartyID()
{
	static char buff[128];

	memset(buff, 0 , sizeof(buff));
	if (clc.demoplaying)
		return NULL;

	if ( cls.state >= CA_LOADING && cls.state <= CA_ACTIVE ) 
	{
		Q_strncpyz(buff, cls.servername, sizeof(buff));
		strcat(buff, "x" );
		return buff;
	}

	return NULL;
}

char *GetServerState() {
	if ( cls.state == CA_ACTIVE ) {
		if (cl_discordRichPresence->integer > 1) {
			return va("%d / %d players [%d BOTS]", cl.discord.playerCount, cl.discord.maxPlayers, cl.discord.botCount);
		}
		
		if (clc.demoplaying)
			return GetGameType(qfalse);

		if (cl.discord.gametype >= GT_TEAM)
			return va("%s | %dv%d", GetGameType(qfalse), cl.discord.redTeam, cl.discord.blueTeam);

		return GetGameType(qfalse);
	}

	if ( cls.state <= CA_DISCONNECTED || cls.state == CA_CINEMATIC )
		return "";

	return GetState();
}

char *GetServerDetails() {
	if ( cls.state == CA_ACTIVE ) {
		if (cl_discordRichPresence->integer > 1) {
			return ReturnServerName();
		}
		
		if (clc.demoplaying) {
			return va("Playing demo - %s", Q_CleanStr(Cvar_VariableString("ui_about_hostname")));
		}

		if (com_sv_running->integer) {
			if (Q_stricmp(Cvar_VariableString("sv_hostname"), "*Jedi*"))
				return va("Playing offline - %s\n", Q_CleanStr(Cvar_VariableString("sv_hostname")));
			
			return (char*)"Playing offline";
		}

		if (cl.snap.valid && ((cl.snap.ps.pm_flags & PMF_FOLLOW) || cl.snap.ps.pm_type == PM_SPECTATOR))
			return va("Spectating on %s", ReturnServerName());
		
		return va("Playing on %s", ReturnServerName());

		return ReturnServerName();
	}

	if (cls.state > CA_DISCONNECTED && cls.state < CA_ACTIVE)
		return "";

	if ( cls.state <= CA_DISCONNECTED || cls.state == CA_CINEMATIC )
		return "In Menu";

	return NULL;
}

static void handleDiscordReady( const DiscordUser* connectedUser )
{
	if (Q_stricmp(Cvar_VariableString("se_language"), "german")) {
		Com_Printf( "^5Discord: connected to user %s#%s - %s\n",
			connectedUser->username,
			connectedUser->discriminator,
			connectedUser->userId );
	}
	else {
		Com_Printf( "^1Discord: ^7verbunden zu Nutzer ^3%s^7#^3%s ^7- ^3%s^7\n",
			connectedUser->username,
			connectedUser->discriminator,
			connectedUser->userId );
	}
}

static void handleDiscordDisconnected( int errcode, const char* message )
{
	if (Q_stricmp(Cvar_VariableString("se_language"), "german"))
		Com_Printf( "^5Discord: ^3disconnected ^3(%d: ^3%s)\n", errcode, message );
	else
		Com_Printf( "^1Discord: ^7getrennt (^3%d^7: ^3%s^7)\n", errcode, message );
}

static void handleDiscordError( int errcode, const char* message )
{
	if (Q_stricmp(Cvar_VariableString("se_language"), "german"))
		Com_Printf( "^5Discord: ^1Error - (%d: %s)\n", errcode, message );
	else
		Com_Printf( "^1Discord: ^7Fehler (^3%d^7: ^3%s^7)\n", errcode, message );
}

static void handleDiscordJoin( const char* secret )
{
	char ip[22] =  { 0 }; //xxx.xxx.xxx.xxx:xxxxx\0
	char password[106] = { 0 };
	int i = 0;
	netadr_t adr;
	qboolean skippedFsgame = qfalse;

	if (Q_stricmp(Cvar_VariableString("se_language"), "german"))
		Com_Printf( "^5Discord: joining ^3(%s)^7\n", secret );
	else
		Com_Printf( "^1Discord: ^7join (^3%s^7)\n", secret );

	// 
	//This implementation is broken in EJK, when servers do not have fs_game set in their .cfg.
	//If that's the case, then there will be an extra space in the secret in between the ip and password, which leads to the variables being incorrectly set or not being set at all.
	//Ideally, this whole implementation should be scrapped and reimplemented.
	// 
	//Let's do a workaround for this.
	
	while (*secret && *secret != ' ') //Parse the ip first.
	{
		if (i >= sizeof(ip)) break;

		ip[i++] = *secret;
		secret++;
	}

	ip[i] = '\0';
	i = 0;

	if (*ip)
	{
		if (NET_StringToAdr(ip, &adr))
		{
			if (!strchr(secret, '\"')) //Check for quotes in the string, if they appear, it means we join a passwordless server.
			{
				if (*secret == ' ' && *(secret + 1) == ' ') //No quotes have been found, check whether the string contains 2 spaces between ip and fsgame.
				{
					secret += 2; //We found 2 spaces, which means the server has no fs_game set. move the pointer by 2 to skip the spaces.

					while (*secret)
					{
						if (i >= sizeof(password)) break;

						password[i++] = *secret;	//Everything after the spaces is the server password.
						secret++;
					}
					password[i] = '\0';
				}
				else //1 space is found, which means the server has fs_game set.
				{
					secret++; //Increment the pointer by 1 to skip the space.
					while (*secret)
					{
						while (!skippedFsgame && *secret != ' ' && *secret) //the first word after the space is fs_game, we can skip that.
						{
							secret++;

							if (*secret == ' ') //We found the next space. Increment pointer again to skip it.
							{
								secret++;
								skippedFsgame = qtrue;
							}
						}

						if (i >= sizeof(password)) break;

						password[i++] = *secret; //Every character after the second space, is part of the password. 
						secret++;
					}
					password[i] = '\0';
				}

				Q_strstrip(password, ";", NULL);

				Cbuf_AddText(va("set password %s; connect %i.%i.%i.%i:%hu\n", password, adr.ip[0], adr.ip[1], adr.ip[2], adr.ip[3], BigShort(adr.port)));
			}
			else
			{
				Cbuf_AddText(va("connect %i.%i.%i.%i:%hu\n", adr.ip[0], adr.ip[1], adr.ip[2], adr.ip[3], BigShort(adr.port)));
			}
		}
		else
		{
			Com_Printf("^5Discord: ^1Invalid IP address ^3(%s)\n", ip);
		}
	}
	Com_Printf("^5Discord: ^1Failed to parse server information from join secret\n");
}

static void handleDiscordSpectate( const char* secret )
{
	if (Q_stricmp(Cvar_VariableString("se_language"), "german"))
		Com_Printf( "^5Discord: spectating ^3(%s)^7\n", secret );
	else
		Com_Printf( "^1Discord: ^7spectate (^3%s^7)\n", secret );
}

static void handleDiscordJoinRequest( const DiscordUser* request )
{
	int response = -1;

	if (Q_stricmp(Cvar_VariableString("se_language"), "german")) {
		Com_Printf( "^5Discord: ^7join request from ^7%s#%s - %s\n",
			request->username,
			request->discriminator,
			request->userId );
	}
	else {
		Com_Printf( "^1Discord: ^7join request from ^3%s^7#^3%s ^7- ^3%s^7\n",
				request->username,
				request->discriminator,
				request->userId );
	}

	if ( response != -1 ) {
		Discord_Respond( request->userId, response );
	}
}

static DiscordRichPresence discordPresence;
static DiscordEventHandlers handlers;
void CL_DiscordInitialize(void)
{
	Com_Memset( &handlers, 0, sizeof( handlers ) );
	handlers.ready = handleDiscordReady;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.errored = handleDiscordError;
	handlers.joinGame = handleDiscordJoin;
	handlers.spectateGame = handleDiscordSpectate;
	handlers.joinRequest = handleDiscordJoinRequest;
	
	Discord_Initialize( APPLICATION_ID, &handlers, 1, "6020" );
	Discord_Register( APPLICATION_ID, NULL );

	Discord_UpdateHandlers( &handlers );

	cl_discordRichPresenceSharePassword = Cvar_Get("cl_discordRichPresenceSharePassword", "1", CVAR_ARCHIVE_ND, "If set, sends password to Discord friends who request to join your game");

	Q_strncpyz(cl.discord.hostName, "*Jedi*", sizeof(cl.discord.hostName));
	Q_strncpyz(cl.discord.mapName, "menu", sizeof(cl.discord.mapName));
	Q_strncpyz(cl.discord.fs_game, BASEGAME, sizeof(cl.discord.fs_game));
}

void CL_DiscordShutdown(void)
{
	Discord_Shutdown();
}

void CL_DiscordUpdatePresence(void)
{
	if (!cls.discordInitialized)
		return;

	if (cl.discord.gametype < GT_FFA || cl.discord.gametype >= numGameTypes)
		cl.discord.gametype = GT_FFA;

	Com_Memset( &discordPresence, 0, sizeof( discordPresence ) );
	
	discordPresence.state = GetServerState();
	discordPresence.details = GetServerDetails();
	discordPresence.largeImageKey = ReturnMapIcon();
	discordPresence.largeImageText = ReturnMapName();
	if (cl_discordRichPresence->integer > 1 || cls.state < CA_ACTIVE || cl.discord.gametype == GT_FFA) {
		discordPresence.smallImageKey = GetState();
		discordPresence.smallImageText = GetState();
	}
	else {
		discordPresence.smallImageKey = GetGameType(qtrue);
		discordPresence.smallImageText = GetGameType(qfalse);
	}
	if (!clc.demoplaying && !com_sv_running->integer)
	{ //send join information blank since it won't do anything in this case
		discordPresence.partyId = PartyID(); // Server-IP zum abgleichen discordchat - send join request in discord chat
		if (cl_discordRichPresence->integer > 1) {
			discordPresence.partySize = cls.state == CA_ACTIVE ? 1 : 0;
			discordPresence.partyMax = cls.state == CA_ACTIVE ? ((cl.discord.maxPlayers - cl.discord.playerCount) + discordPresence.partySize) : 0;
		}
		else {
			discordPresence.partySize = cls.state >= CA_LOADING ? cl.discord.playerCount : 0;
			discordPresence.partyMax = cls.state >= CA_LOADING ? cl.discord.maxPlayers : 0;
		}
		discordPresence.joinSecret = joinSecret(); // Server-IP zum discordJoin ausf�hren - serverip for discordjoin to execute
	}
	Discord_UpdatePresence( &discordPresence );

	Discord_RunCallbacks();
}

#endif