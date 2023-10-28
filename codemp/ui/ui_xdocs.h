#ifndef DEFAULT_FORMAT_CALLBACK
#error Please define a default format callback before including this file
#endif

#ifndef SIMPLE_FORMAT_CALLBACK
#error Please define a default format callback before including this file
#endif

#define NL "\n"
#define LINE( tabbing, color, prefix, separator, desc ) tabbing color prefix S_COLOR_WHITE separator desc
#define PADGROUP( specialChar, text ) specialChar text specialChar

// describes one possible setting. use their aliases when possible
#define DESCLINE( color, prefix, desc )		LINE( "    ", color, prefix, " " S_COLOR_GREY"-" S_COLOR_WHITE" ", desc )
#define EMPTYLINE( prefix, moreDesc )		LINE( "    ", "", prefix, "   ", moreDesc )
#define EXAMPLE( example, desc )			S_COLOR_WHITE "Example: " S_COLOR_GREY"\"" S_COLOR_WHITE example S_COLOR_GREY "\" -> " S_COLOR_WHITE desc ""

// aliases for DESCLINE
#define SETTING( setting, desc )			DESCLINE( S_COLOR_WHITE, PADGROUP( "\x1f", setting ), desc )
#define SPECIAL( special, desc )			DESCLINE( S_COLOR_YELLOW, PADGROUP( "\x1f", special ), desc )

// aliases for EMPTYLINE to continue the previous DESCLINE alias
#define SETTING_NEXTLINE( moreDesc )		EMPTYLINE( PADGROUP( "\x1f", "" ), moreDesc )
#define SPECIAL_NEXTLINE( moreDesc )		EMPTYLINE( PADGROUP( "\x1f", "" ), moreDesc )

#ifdef XDOCS_CVAR_HELP
#define XDOCS_CVAR_DEF(name, shortDesc, longDesc) { name, shortDesc, longDesc },
#define XDOCS_CVAR_BITFLAG_DEF( name, shortDesc, longDesc ) { name, shortDesc, longDesc },
#define XDOCS_CVAR_KEYVALUE_DEF( name, shortDesc, longDesc ) { name, shortDesc, longDesc },
#define XDOCS_CMD_DEF( name, shortDesc ) { name, shortDesc, DEFAULT_FORMAT_CALLBACK },
#endif

/* --------------------------------------------------- */
/* CVARS */

// EternalJK HUD cvars:
XDOCS_CVAR_DEF("cg_hudFiles", "HUD style",
    SETTING("0", "JKA HUD") NL
    SETTING("1", "Simple HUD") NL
    SETTING("2", "JK2 HUD") NL
    SETTING("3", "Elegant HUD") NL
    SETTING("4", "Quake HUD")
)


XDOCS_CVAR_DEF("cg_movementKeys", "Show the movement keys onscreen",
	SETTING("0", "Movement keys are hidden") NL
	SETTING("1", "Movement keys are shown") NL
    SETTING("2", "Movement keys are shown with attack/altattack inputs") NL
    SETTING("3", "Crosshair style") NL
    SETTING("4", "Moveable crosshair style")
)

XDOCS_CVAR_DEF("cg_movementKeysX", "Horizontal location of the movement keys",""
)

XDOCS_CVAR_DEF("cg_movementKeysY", "Vertical location of the movement keys",""
)

XDOCS_CVAR_DEF("cg_movementKeysSize", "Scale of the movement keys",""
)

XDOCS_CVAR_DEF("cg_speedometer", "Configure with the /speedometer command",""
)

XDOCS_CVAR_DEF("cg_speedometerX", "Horizontal location of the speedometer",""
)

XDOCS_CVAR_DEF("cg_speedometerY", "Vertical location of the speedometer",""
)

XDOCS_CVAR_DEF("cg_speedometerSize", "Scale of the speedometer",""
)

XDOCS_CVAR_DEF("cg_speedometerJumps", "The amount of jumps to store in the jumps array, enable with the /speedometer command",""
)

XDOCS_CVAR_DEF("cg_speedometerJumpsX", "Horizontal location of the jumps array",""
)

XDOCS_CVAR_DEF("cg_speedometerJumpsY", "Vertical location of the jumps array",""
)

XDOCS_CVAR_DEF("cg_jumpGoal", "Set a first jump speed goal speed, if first jump speed is above your goal speed, the pre-speed text will be green. Requires /speedometer 1 enabled",""
)

XDOCS_CVAR_DEF("cg_startGoal", "start speed goal, if your start speed is above your goal speed, the text will be green, requires /cg_raceStart 1",""
)

XDOCS_CVAR_DEF("cg_drawTeamOverlay", "Draw the team overlay for team-based gametypes",
	SETTING("0", "The team overlay is not drawn") NL
	SETTING("1", "The team overlay is drawn")
    SETTING("2", "The team overlay is drawn, excluding the local client")
    SETTING("3", "The new team overlay is drawn")
    SETTING("4", "The team overlay is drawn, excluding the local client")
)

XDOCS_CVAR_DEF("cg_drawTeamOverlayX", "Horizontal location of the team overlay",
)

XDOCS_CVAR_DEF("cg_drawTeamOverlayY", "Vertical location of the team overlay",
)

XDOCS_CVAR_DEF("cg_raceTimer", "Show the race timer onscreen",
	SETTING("0", "Race timer is hidden") NL
	SETTING("1", "Race timer is shown") NL
	SETTING("2", "Race timer is shown with average and max speed") NL
	SETTING("3", "Race timer is shown with ms, average speed, and max speed")
)

XDOCS_CVAR_DEF("cg_raceTimerX", "Horizontal location of the race timer onscreen",""
)

XDOCS_CVAR_DEF("cg_raceTimerY", "Vertical location of the race timer onscreen",""
)

XDOCS_CVAR_DEF("cg_raceTimerSize", "Scale of the race timer",""
)

XDOCS_CVAR_DEF("cg_raceStart", "Enable race timer start speed separately from the race timer",""
)

XDOCS_CVAR_DEF("cg_raceStartX", "Horizontal location of the race start timer onscreen",""
)

XDOCS_CVAR_DEF("cg_raceStartY", "Vertical location of the race start timer onscreen",""
)

XDOCS_CVAR_DEF("cg_scoreboardTime", "Displays the local time on the scoreboard",""
)

XDOCS_CVAR_DEF("cg_smallScoreboard", "Always use the small version of the scoreboard",
	SETTING("0", "Scoreboard scales depending on number of players (Base behavior)") NL
	SETTING("1", "Scoreboard is always small")
)

XDOCS_CVAR_DEF("cg_scoreDeaths", "Display score AND deaths on the scoreboard",
	"This does not work on base." NL
	SETTING("0", "Scoreboard only shows score (Base JAMP behavior)") NL
	SETTING("1", "Scoreboard shows score and deaths on JA+ or jaPRO servers (default)") NL
	SETTING("2", "Scoreboard shows score and locally counted deaths")
)

XDOCS_CVAR_DEF("cg_killMessage", "Print a kill message on the screen when you kill someone",
	SETTING("0", "Kill messages won't be printed") NL
	SETTING("1", "Kill messages will be printed, showing score and placement in FFA mode (Base behavior)") NL
	SETTING("2", "Kill messages will be printed, without score or current place in FFA mode") NL
	SETTING("3", "Kill messages will be placed higher on the screen than normal")
)

XDOCS_CVAR_DEF("cg_newFont", "Uses a different font for the chat",
	SETTING("0", "Use the base font") NL
	SETTING("1", "Use the new font")
)

XDOCS_CVAR_DEF("cg_chatBoxShowHistory", "Allows showing message history in chatbox when console is open",
	SETTING("0", "Enabled") NL
	SETTING("1", "Disabled (baseJKA behavior)")
)

XDOCS_CVAR_DEF("cg_teamChatsOnly", "Hide non-team chat messages",
	"Ignored messages will only be printed in the console." NL
	SETTING("0", "Show all chat") NL
	SETTING("1", "Hide global chat from all players") NL
	SETTING("2", "Hide global chat from spectators while you are in-game (requires compatible server mod)")
)

XDOCS_CVAR_DEF("cg_chatBoxFontSize", "Scale of the chat box font",""
)

XDOCS_CVAR_DEF("cg_chatBoxCutOffLength", "Width of a line in the chatbox before breaking to a new one",
	SETTING("350", "jaPRO cutoff length") NL
	SETTING("550", "baseJKA default") NL
	SETTING("640", "Maximum")
)

XDOCS_CVAR_DEF("cg_crossHairRed", "Custom red color of the crosshair",""
)

XDOCS_CVAR_DEF("cg_crossHairGreen", "Custom green color of the crosshair",""
)

XDOCS_CVAR_DEF("cg_crossHairBlue", "Custom blue color of the crosshair",""
)

XDOCS_CVAR_DEF("cg_crossHairAlpha", "Custom transparency of the crosshair",""
)

XDOCS_CVAR_DEF("cg_crossHairScope", "Minimalist crosshair disruptor scope replacement",""
)

XDOCS_CVAR_DEF("cg_hudColors", "Changes the colors of the HUD based on saber style and force",
	"This only applies to the simple HUD." NL
	SETTING("0", "Style and force colors will not change (Base behavior)") NL
	SETTING("1", "Style and force colors will change")
)

XDOCS_CVAR_DEF("cg_tintHud", "Changes the color of the HUD based on team color",
	"This does not apply to the simple HUD." NL
	SETTING("0", "HUD colors will not change") NL
	SETTING("1", "HUD colors will change")
)

XDOCS_CVAR_DEF("cg_drawScore", "Displays your score in the lower right",
	"This does not apply to the simple HUD." NL
	SETTING("0", "Score is hidden") NL
	SETTING("1", "Score is drawn beside the HUD") NL
	SETTING("2", "Score, team score, and bias is drawn")
)

XDOCS_CVAR_DEF("cg_drawScores", "Displays the team scores",
"This does not apply to the simple HUD." NL
SETTING("0", "Scores are hidden") NL
SETTING("1", "Team scores are drawn in the top right") NL
SETTING("2", "Team scores are drawn in the top right with color") NL
SETTING("3", "Large team scores")
)

XDOCS_CVAR_DEF("cg_drawTimer", "Draw the match timer",
	SETTING("0", "Big text") NL
	SETTING("1", "Small text 1") NL
	SETTING("2", "Small text 2") NL
    SETTING("3", "Small text 3") NL
    SETTING("4", "Medium text") NL
    SETTING("5", "Large text") NL
    SETTING("6", "Small text 4") NL
    SETTING("7", "Counter strike")
)

XDOCS_CVAR_DEF("cg_drawFPS", "Draw the match timer",
    SETTING("0", "Big text") NL
    SETTING("1", "Small text 1") NL
    SETTING("2", "Small text 2") NL
    SETTING("3", "Small text 3") NL
    SETTING("4", "Medium text") NL
    SETTING("5", "Large text") NL
    SETTING("6", "Small text 4")
)

XDOCS_CVAR_DEF("cg_drawVote", "Displays votecalls in the upper left",
	"If turned off, votecalls are still shown in the console." NL
	SETTING("0", "Callvotes are hidden") NL
	SETTING("1", "Callvotes are shown")
)

//Strafehelper

XDOCS_CVAR_DEF("cg_strafeHelper", "Configure with the /strafehelper command",""
)

//Pitchhelper

XDOCS_CVAR_DEF("cg_pitchHelper", "Draw pitch angle independently from /cg_showpos",""
)

XDOCS_CVAR_DEF("cg_pitchHelperRange", "The range of numbers to display as green (eg. range 30, offset 0 = +15 to -15)",""
)

XDOCS_CVAR_DEF("cg_pitchHelperOffset", "Offset the green range up or down (eg. range 30, offset 10 = +25 to -5)",""
)

XDOCS_CVAR_DEF("cg_pitchHelperX", "Move the pitch helper's horizontal location",""
)

XDOCS_CVAR_DEF("cg_pitchHelperY", "Move the pitch helper's vertical location",""
)

// Disruptor trail effects
XDOCS_CVAR_DEF("cg_disruptorNew", "Enable new disruptor effects",""
)

XDOCS_CVAR_DEF("cg_disruptorTeamColor", "Changes disruptor effects colors according to team","Requires cg_disruptorNew 1 or cg_disruptorSpiral 1"
)

XDOCS_CVAR_DEF("cg_disruptorCoreColor", "Changes disruptor line color (accepts hexcode eg. xffffff)","Requires cg_disruptorNew 1"
)

XDOCS_CVAR_DEF("cg_disruptorSpiral", "Enable disruptor spiral effect",""
)

XDOCS_CVAR_DEF("cg_disruptorSpiralColor", "Changes disruptor spiral color (accepts hexcode eg. xffffff)","Requires cg_disruptorSpiral 1"
)

XDOCS_CVAR_DEF("cg_disruptorMainTime", "Amount of time to display disruptor primary fire",""
)

XDOCS_CVAR_DEF("cg_disruptorAltTime", "Amount of time to display disruptor alt fire",""
)

XDOCS_CVAR_DEF("cg_ambientSounds", "Play ambient sounds",
SETTING("0", "Don't play ambient sounds") NL
SETTING("1", "Play ambient sounds")
)

XDOCS_CVAR_DEF("cg_rollSounds", "Play sound when players roll",
	SETTING("0", "Don't play roll sounds") NL
	SETTING("1", "Play roll sounds from all clients") NL
	SETTING("2", "Play roll sounds from other clients") NL
	SETTING("3", "Only play roll sounds from local client")
)

XDOCS_CVAR_DEF("cg_jumpSounds", "Play sound when players jump",
	SETTING("0", "Don't play jump sounds") NL
	SETTING("1", "Play all jump sounds") NL
	SETTING("2", "Play only when other clients jump") NL
	SETTING("3", "Play only when local client jumps")
)

XDOCS_CVAR_DEF("cg_chatSounds", "Play sound when chat messages are received",
	SETTING("0", "Don't play chat sounds") NL
	SETTING("1", "Play chat sounds for all messages") NL
	SETTING("2", "Play chat sounds for private and team messages only")
)

XDOCS_CVAR_DEF("cg_hitSounds", "Play a sound when you hit someone",
	SETTING("0", "Don't play the hit sound (Base behavior)") NL
	SETTING("1", "Play the hit sound from Quake 3: Arena") NL
	SETTING("2", "Play the hit sound from Legions: Overdrive") NL
	SETTING("3", "Play the hit sound from Warsow") NL
	SETTING("4", "Play the hit sound from Tribes: Ascend")
)

XDOCS_CVAR_DEF("cg_raceSounds", "Play sound when race is started",
	SETTING("0", "Don't play race sound") NL
	SETTING("1", "Play race sounds")
)

XDOCS_CVAR_DEF("cg_duelSounds", "Enables/disables announcer/center print at the start of private duels",
			   SETTING("0", "Do not announce the start of duel") NL
			   SETTING("1", "Announce and center print \"BEGIN\" (baseJKA behavior)") NL
			   SETTING("2", "Announce, but do not center print") NL
			   SETTING("3", "Center print, but do not announce")
)

XDOCS_CVAR_DEF("cg_duelMusic", "Enables special in-game music for private duels",
			SETTING("0", "Use map music while in private duels") NL
			SETTING("1", "Play music track for private duels (baseJKA behavior)")
)

XDOCS_CVAR_DEF("cg_forceCosmetics", "Forces all players to have cosmetics on non-japro servers",
    SETTING("1", "Santa Hat") NL
    SETTING("2", "Jack'o Lantern") NL
    SETTING("3", "Baseball Cap") NL
    SETTING("4", "Indiana Jones") NL
    SETTING("5", "Propeller Hat") NL
    SETTING("6", "Sombrero") NL
    SETTING("7", "Top Hat") NL
    SETTING("8", "Mask")


)
//Visuals

XDOCS_CVAR_DEF("cg_remaps", "Show or hide serverside remaps",
	"This requires a vid_restart." NL
	SETTING("0", "Don't show remaps") NL
	SETTING("1", "Show remaps")
)

XDOCS_CVAR_DEF("cg_screenShake", "Shake screen when hit or while charging weapons",
	SETTING("0", "Screen doesn't shake") NL
	SETTING("1", "Screen shakes only when taking damage") NL
	SETTING("2", "Screen shakes when charging weaspons and taking damage")
)

XDOCS_CVAR_DEF("cg_drawScreenTints", "Turn the tint from water on or off",
	SETTING("0", "Removes the screen tint") NL
	SETTING("1", "Shows the screen tint")
)

XDOCS_CVAR_DEF("cg_smoothCamera", "Smooth camera movement",
	"This works best with low cameradamp." NL
	SETTING("0", "No smooth camera movement") NL
	SETTING("1", "Camera movements are smooth")
)

XDOCS_CVAR_DEF("cg_blood", "Show blood when shot by guns",
	"Blood only shows from guns; gibs only show on JAPRO servers." NL
	SETTING("0", "No blood and no gibs") NL
	SETTING("1", "Only show blood") NL
	SETTING("2", "Show blood and gibs")
)


XDOCS_CVAR_DEF("cg_thirdPersonFlagAlpha", "Custom transparency of the CTF flag",""
)

XDOCS_CVAR_DEF("cg_stylePlayer", "Configure with the /stylePlayer command",""
)

XDOCS_CVAR_DEF("cg_alwaysShowAbsorb", "Show absorb when it's in use",
	SETTING("0", "Absorb shows only when used against another power (Base behavior)") NL
	SETTING("1", "Absorb shows always when in use")
)

XDOCS_CVAR_DEF("cg_zoomFov", "The field of view when using +zoom",""
)

XDOCS_CVAR_DEF("cg_fleshSparks", "Maximum number of sparks from a saber hit",""
)

XDOCS_CVAR_DEF("r_renderClipBrushes", "Render clip brushes",""
)

XDOCS_CVAR_DEF("r_renderClipBrushesShader", "Shader for clip brush rendering",""
)

XDOCS_CVAR_DEF("r_renderSlickSurfaces", "Render slick surfaces",""
)

XDOCS_CVAR_DEF("r_renderSlickSurfacesShader", "Shader for slick surface rendering",""
)

XDOCS_CVAR_DEF("r_renderTriggerBrushes", "Render trigger brushes",""
)

XDOCS_CVAR_DEF("r_renderTriggerBrushesShader", "The shader for trigger brushes",""
)

XDOCS_CVAR_DEF("cg_noFX", "Determines if effects and map models are shown",
	SETTING("0", "No effects are removed") NL
	SETTING("1", "Removes effects") NL
	SETTING("2", "Removes effects and speakers") NL
	SETTING("3", "Removes above and replaces misc map models") NL
	SETTING("4", "Removes effects, speakers, and misc map models") NL
	SETTING("5", "Completely disables all scheduled FX")
)

XDOCS_CVAR_DEF("cg_noTeleFX", "Disables the teleportation effect",
	"This is also used for the spawn effect." NL
	SETTING("0", "Teleport effect is on") NL
	SETTING("1", "Teleport effect is off")
)

XDOCS_CVAR_DEF("g_gametype", "Gametype that the server is currently on",
	SETTING("0", "FFA") NL
	SETTING("1", "Holocron") NL
	SETTING("2", "Jedi Master") NL
	SETTING("3", "Duel") NL
	SETTING("4", "Powerduel") NL
	SETTING("5", "SP FFA") NL
	SETTING("6", "TFFA") NL
	SETTING("7", "Siege") NL
	SETTING("8", "CTF") NL
	SETTING("9", "CTY")
)

XDOCS_CVAR_DEF("cg_killfeed", "Draw a killfeed on the HUD",
SETTING("0", "Disable the HUD killfeed") NL
SETTING("1", "Draw killfeed HUD") NL
SETTING("2", "Draw killfeed HUD + console death messages")
)

XDOCS_CVAR_DEF("cg_killfeedAlignment", "Align the killfeed",
SETTING("0", "Align items to the right") NL
SETTING("1", "Align items to the left") NL
SETTING("2", "Align items to the center")
)

XDOCS_CVAR_DEF("cg_killfeedX", "Offset the killfeed's horizontal position from its current position",""
)

XDOCS_CVAR_DEF("cg_killfeedY", "Offset the killfeed's vertical position from its current position",""
)

XDOCS_CVAR_DEF("cg_killfeedIconSize", "Resize the killfeed",""
)
XDOCS_CVAR_DEF("cg_killfeedTextSize", "Resize the killfeed",""
)
XDOCS_CVAR_DEF("cg_killfeedColors", "Color the killfeed icons",""
)

//Work from above this line

// ...

/* --------------------------------------------------- */
/* COMMANDS */

// EternalJK console commands:

XDOCS_CMD_DEF("clientlist", "Displays a list of all connected clients and their real client numbers")

// ...

#undef NL
#undef LINE
#undef PADGROUP

#undef DESCLINE
#undef EMPTYLINE
#undef EXAMPLE

#undef SETTING
#undef SPECIAL

#undef SETTING_NEXTLINE
#undef SPECIAL_NEXTLINE

#undef XDOCS_CVAR_DEF
#undef XDOCS_CMD_DEF