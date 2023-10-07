/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2013 - 2015, OpenJK contributors

This file is part of the OpenJK source code.

OpenJK is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#include "ui_local.h"
extern void UI_UpdateWidescreen(void);
//
// Cvar callbacks
//

static qboolean startup = qfalse;

static int UI_GetScreenshotFormatForString( const char *str ) {
	if ( !Q_stricmp(str, "jpg") || !Q_stricmp(str, "jpeg") )
		return SSF_JPEG;
	else if ( !Q_stricmp(str, "tga") )
		return SSF_TGA;
	else if ( !Q_stricmp(str, "png") )
		return SSF_PNG;
	else
		return -1;
}

static const char *UI_GetScreenshotFormatString( int format )
{
	switch ( format )
	{
	default:
	case SSF_JPEG:
		return "jpg";
	case SSF_TGA:
		return "tga";
	case SSF_PNG:
		return "png";
	}
}

static void UI_UpdateScreenshot( void )
{
	qboolean changed = qfalse;
	// check some things
	if ( ui_screenshotType.string[0] && isalpha( ui_screenshotType.string[0] ) )
	{
		int ssf = UI_GetScreenshotFormatForString( ui_screenshotType.string );
		if ( ssf == -1 )
		{
			trap->Print( "UI Screenshot Format Type '%s' unrecognised, defaulting to JPEG\n", ui_screenshotType.string );
			uiInfo.uiDC.screenshotFormat = SSF_JPEG;
			changed = qtrue;
		}
		else
			uiInfo.uiDC.screenshotFormat = ssf;
	}
	else if ( ui_screenshotType.integer < SSF_JPEG || ui_screenshotType.integer > SSF_PNG )
	{
		trap->Print( "ui_screenshotType %i is out of range, defaulting to 0 (JPEG)\n", ui_screenshotType.integer );
		uiInfo.uiDC.screenshotFormat = SSF_JPEG;
		changed = qtrue;
	}
	else {
		uiInfo.uiDC.screenshotFormat = atoi( ui_screenshotType.string );
		changed = qtrue;
	}

	if ( changed ) {
		trap->Cvar_Set( "ui_screenshotType", UI_GetScreenshotFormatString( uiInfo.uiDC.screenshotFormat ) );
		trap->Cvar_Update( &ui_screenshotType );
	}
}

static void CVU_StrafeHelper (void) {
	trap->Cvar_Set( "cg_strafeHelperActiveColor", va("%i %i %i %i", ui_sha_r.integer, ui_sha_g.integer, ui_sha_b.integer, ui_sha_a.integer) );
}

static void CVU_UpdateModelList(void) {
	uiClientState_t cstate = {0};

	if (startup) {//we don't want to redundantly call these on startup
		UI_UpdateCurrentServerInfo();
		return;
	}

	trap->GetClientState(&cstate);
	if (ui_sv_pure.integer && (cstate.connState < CA_LOADING || ui_singlePlayerActive.integer)) {
		trap->Cvar_Set("ui_sv_pure", "0");
		return;
	}
	UI_UpdateSaberHiltInfo();
	UI_BuildQ3Model_List();
	UI_BuildPlayerModel_List(qtrue);
	UI_Load(); //refreshes the available species in the selection feeder */
}

//
// Cvar table
//
typedef struct cvarTable_s {
	vmCvar_t	*vmCvar;
	char		*cvarName;
	char		*defaultString;
	void		(*update)( void );
	uint32_t	cvarFlags;
} cvarTable_t;

#define XCVAR_DECL
	#include "ui_xcvar.h"
#undef XCVAR_DECL

static const cvarTable_t uiCvarTable[] = {
	#define XCVAR_LIST
		#include "ui_xcvar.h"
	#undef XCVAR_LIST
};
static const size_t uiCvarTableSize = ARRAY_LEN( uiCvarTable );

void UI_RegisterCvars( void ) {
	size_t i = 0;
	const cvarTable_t *cv = NULL;

	startup = qtrue;
	for ( i=0, cv=uiCvarTable; i<uiCvarTableSize; i++, cv++ ) {
		trap->Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags );
		if ( cv->update )
			cv->update();
	}
	startup = qfalse;
}

void UI_UpdateCvars( void ) {
	size_t i = 0;
	const cvarTable_t *cv = NULL;

	for ( i=0, cv=uiCvarTable; i<uiCvarTableSize; i++, cv++ ) {
		if ( cv->vmCvar ) {
			int modCount = cv->vmCvar->modificationCount;
			trap->Cvar_Update( cv->vmCvar );
			if ( cv->vmCvar->modificationCount != modCount ) {
				if ( cv->update )
					cv->update();
			}
		}
	}
}
