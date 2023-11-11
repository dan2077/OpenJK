/*
===========================================================================
Copyright (C) 1999 - 2005, Id Software, Inc.
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2005 - 2015, ioquake3 contributors
Copyright (C) 2013 - 2015, OpenJK contributors
Copyright (C) 2015 - 2021, EternalJK contributors
Copyright (C) 2015 - 2023, TaystJK contributors


This file is part of the TaystJK source code.

TaystJK is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <https://www.gnu.org/licenses/>.
===========================================================================
*/

#include "hud_strafehelper.h"
#include "cg_local.h"

static float speedometerXPos, jumpsXPos;
static float firstSpeed;
static float speedSamples[SPEEDOMETER_NUM_SAMPLES];
static int oldestSpeedSample = 0;
static int maxSpeedSample = 0;
static dfstate state;
speedgraph_t speedgraph;

int DF_GetMovePhysics(){
    int physics;
    physics = PM_GetMovePhysics();
    return physics;
}

/* Physics Constants Getters */
//get the styles accelerate - acceleration on ground constant
float DF_GetAccelerate() {
    float accelerate;
    switch (state.moveStyle) {
        case MV_PJK:
        case MV_CPM:
        case MV_RJCPM:
        case MV_BOTCPM:
            accelerate = pm_cpm_accelerate;
            break;
        case MV_WSW:
            accelerate = pm_wsw_accelerate;
            break;
        case MV_SLICK:
            accelerate = pm_slick_accelerate;
            break;
        case MV_SP:
            accelerate = pm_sp_accelerate;
            break;
        default:
            accelerate = pm_accelerate;
    }
    return accelerate;
}
//get the styles airaccelerate - acceleration in air constant
float DF_GetAirAccelerate() {
    float airAccelerate;
    switch (state.moveStyle) {
        case MV_PJK:
        case MV_CPM:
        case MV_WSW:
        case MV_SLICK:
        case MV_RJCPM:
        case MV_BOTCPM:
            airAccelerate = pm_cpm_airaccelerate;
            break;
        case MV_JETPACK:
            airAccelerate = pm_jetpack_airaccelerate;
            break;
        case MV_SP:
            airAccelerate = pm_sp_airaccelerate;
            break;
        case MV_QW:
            airAccelerate = pm_qw_airaccelerate * 4.0f;
            break;
        default:
            airAccelerate = pm_airaccelerate;
    }
    return airAccelerate;
}
//get the styles airstrafeaccelerate - acceleration in air with air control constant
float DF_GetAirStrafeAccelerate() {
    float airStrafeAccelerate;
    switch (state.moveStyle) {
        case MV_PJK:
        case MV_CPM:
        case MV_WSW:
        case MV_RJCPM:
        case MV_BOTCPM:
            airStrafeAccelerate = pm_cpm_airstrafeaccelerate;
            break;
        case MV_SLICK:
            airStrafeAccelerate = pm_slick_airstrafeaccelerate;
            break;
        case MV_SP:
            airStrafeAccelerate = pm_sp_airaccelerate;
            break;
        default:
            airStrafeAccelerate = pm_airaccelerate;
    }
    return airStrafeAccelerate;
}
//get the styles airstopaccelerate - constant of air accel stop rate with air control
float DF_GetAirStopAccelerate() {
    float airStopAccelerate;
    switch (state.moveStyle) {
        case MV_PJK:
        case MV_CPM:
        case MV_WSW:
        case MV_SLICK:
        case MV_RJCPM:
        case MV_BOTCPM:
            airStopAccelerate = pm_cpm_airstopaccelerate;
            break;
        default:
            airStopAccelerate =  0.0f;
    }
    return airStopAccelerate;
}
//get the styles airstrafewishspeed - constant of wishspeed while in air with air control
float DF_GetAirStrafeWishspeed() {
    float airStrafeWishSpeed;
    switch (state.moveStyle) {
        case MV_PJK:
        case MV_CPM:
        case MV_WSW:
        case MV_SLICK:
        case MV_RJCPM:
        case MV_BOTCPM:
            airStrafeWishSpeed = pm_cpm_airstrafewishspeed;
            break;
        default:
            airStrafeWishSpeed = 0;
    }
    return airStrafeWishSpeed;
}
//get the styles friction - constant of friction
float DF_GetFriction() {
    float friction;
    switch (state.moveStyle) {
        case MV_CPM:
        case MV_WSW:
        case MV_RJCPM:
        case MV_BOTCPM:
            friction = pm_cpm_friction;
            break;
        case MV_SLICK:
            friction = pm_slick_friction;
            break;
        case MV_QW:
            friction = pm_qw_friction;
            break;
        default:
            friction = pm_friction;
    }
    return friction;
}
//get the styles duckscale - how much speed is scaled when ducking
float DF_GetDuckScale() {
    float duckScale;
    switch (state.moveStyle) {
        case MV_Q3:
        case MV_RJQ3:
            duckScale = pm_vq3_duckScale;
            break;
        case MV_WSW:
            duckScale = pm_wsw_duckScale;
            break;
        default:
            duckScale = pm_duckScale;
    }
    return duckScale;
}

/* Physics Constants Checkers */
//does the style have air control - using A/D turning to turn and accelerate fast in air
qboolean DF_HasAirControl() {
    qboolean hasAirControl;
    switch (state.moveStyle) {
        case MV_PJK:
        case MV_CPM:
        case MV_WSW:
        case MV_SLICK:
        case MV_RJCPM:
        case MV_BOTCPM:
            hasAirControl = qtrue;
            break;
        default:
            hasAirControl = qfalse;
    }
    return hasAirControl;
}
//does the style have forcejumps - holding jump to keep jumping higher (base mechanic)
qboolean DF_HasForceJumps() {
    qboolean hasForceJumps;
    switch (state.moveStyle) {
        case MV_JKA:
        case MV_PJK:
        case MV_QW:
        case MV_SPEED:
        case MV_SP:
            hasForceJumps = qtrue;
            break;
        default:
            hasForceJumps = qfalse;
    }
    return hasForceJumps;
}
//does the style have autojumps - hold to keep jumping on ground
qboolean DF_HasAutoJump() {
    qboolean hasAutoJump;
    switch (state.moveStyle) {
        case MV_JKA:
        case MV_SPEED:
        case MV_SP:
            hasAutoJump = qfalse;
            break;
        default:
            hasAutoJump = qtrue;
    }
    return hasAutoJump;
}

/* Draw HUD */
void DF_DrawStrafeHUD(centity_t	*cent)
{
    //set the playerstate
    DF_SetPlayerState(cent); //state.
    if (cg_strafeHelper.integer) {
        DF_StrafeHelper();
    }

    //japro movement keys
    if (cg_movementKeys.integer) {
        DF_DrawMovementKeys(cent);
    }

    if ((cg_speedometer.integer & SPEEDOMETER_ENABLE) || (cg_strafeHelper.integer & SHELPER_ACCELMETER)) {
        speedometerXPos = cg_speedometerX.value;
        jumpsXPos = cg_speedometerJumpsX.value;
		//Offset the speedometer
		if (cgs.newHud) {
			switch (cg_hudFiles.integer)
			{
				case 0: speedometerXPos -= 8; break;
				case 1: speedometerXPos -= 56; break;
				case 2: speedometerXPos -= 42; break;
				default: break;
			}
		}
        DF_DrawSpeedometer();

        if (((cg_speedometer.integer & SPEEDOMETER_ACCELMETER) || (cg_strafeHelper.integer & SHELPER_ACCELMETER)))
            DF_DrawAccelMeter();
        if (cg_speedometer.integer & SPEEDOMETER_JUMPHEIGHT)
            DF_DrawJumpHeight(cent);
        if (cg_speedometer.integer & SPEEDOMETER_JUMPDISTANCE)
            DF_DrawJumpDistance();
        if (cg_speedometer.integer & SPEEDOMETER_VERTICALSPEED)
            DF_DrawVerticalSpeed();
        if (cg_speedometer.integer & SPEEDOMETER_YAWSPEED)
            DF_DrawYawSpeed();
        if ((cg_speedometer.integer & SPEEDOMETER_SPEEDGRAPH)) {
            rectDef_c speedgraphRect;
            vec4_t foreColor = { 0.0f,0.8f,1.0f,0.8f };
            vec4_t backColor = { 0.0f,0.8f,1.0f,0.0f };
            speedgraphRect.x = (SCREEN_WIDTH * 0.5f - (150.0f / 2.0f));
            speedgraphRect.y = SCREEN_HEIGHT - 22 - 2;
            speedgraphRect.w = 150.0f;
            speedgraphRect.h = 22.0f;
            DF_GraphAddSpeed();
            DF_DrawSpeedGraph(&speedgraphRect, foreColor, backColor);
        }
        if ((cg_speedometer.integer & SPEEDOMETER_SPEEDGRAPHOLD))
            DF_DrawSpeedGraphOld();
    }

    if (cg_raceTimer.integer || cg_raceStart.integer)
        DF_RaceTimer();

    //jaPRO strafehelper line crosshair

    if (cg_strafeHelper.integer & SHELPER_CROSSHAIR) {
        vec4_t		hcolor;
        float		lineWidth;

        if (!cg.crosshairColor[0] && !cg.crosshairColor[1] && !cg.crosshairColor[2]) { //default to white
            hcolor[0] = 1.0f;
            hcolor[1] = 1.0f;
            hcolor[2] = 1.0f;
            hcolor[3] = 1.0f;
        }
        else {
            hcolor[0] = cg.crosshairColor[0];
            hcolor[1] = cg.crosshairColor[1];
            hcolor[2] = cg.crosshairColor[2];
            hcolor[3] = cg.crosshairColor[3];
        }

        lineWidth = cg_strafeHelperLineWidth.value;
        if (lineWidth < 0.25f)
            lineWidth = 0.25f;
        else if (lineWidth > 5)
            lineWidth = 5;

        DF_DrawLine((0.5f * SCREEN_WIDTH), (0.5f * SCREEN_HEIGHT) - 5.0f,
					(0.5f * SCREEN_WIDTH), (0.5f * SCREEN_HEIGHT) + 5.0f,
					lineWidth, hcolor, hcolor[3], 0); //640x480, 320x240
    }
}

/* Strafehelper */

//main strafehelper function, sets states and then calls drawstrafeline function for each keypress
void DF_StrafeHelper() {
    dfsline line = { 0 }, rearLine = { 0 },
            maxLine = { 0 }, rearMaxLine = { 0 },
            activeLine = { 0 }, rearActiveLine = { 0 },
            centerLine = { 0 }, rearCenterLine = { 0 };
    float activeOpt, rearActiveOpt, activeMax, rearActiveMax;
    int i;

    //set strafehelper settings struct
    DF_SetStrafeHelper(); //state.strafehelper.

    //pointless for siege
    if(state.moveStyle == MV_SIEGE){
        return;
    }

    //get the active opt line
    activeLine = DF_GetLine(state.moveDir,qfalse, qfalse);
    if(activeLine.onScreen && activeLine.active){
        activeOpt = activeLine.x;
    }

    //get the rear opt line
    if(state.strafeHelper.rear || state.moveDir == KEY_W) {
        rearActiveLine = DF_GetLine(state.moveDir, qtrue, qfalse);
        if (rearActiveLine.onScreen && rearActiveLine.active) {
            rearActiveOpt = rearActiveLine.x;
        }
    }

    //get the active max lines
    if((state.strafeHelper.max || state.strafeHelper.triangles) //only draw max line or triangles for active key
       && !(state.cmd.forwardmove == 0 && state.cmd.rightmove == 0) //only do this if keys are pressed
       && (!(state.onGround && state.cgaz.wasOnGround) //only do this in the air
           || state.moveStyle == MV_SLICK)) //or if we are slick
    {
        //active max line
        maxLine = DF_GetLine(state.moveDir, qfalse, qtrue);
        if(maxLine.onScreen && maxLine.active){
            activeMax = maxLine.x;
        }
        //alternate active max angle
        if (state.strafeHelper.rear || state.moveDir == KEY_W) { //always draw both lines for W
            rearMaxLine = DF_GetLine(state.moveDir, qtrue, qtrue); //get the rear max line
            if(rearMaxLine.onScreen && rearMaxLine.active){
                rearActiveMax = rearMaxLine.x;
            }
        }

        //draw the active max line/triangle
        if (maxLine.onScreen && maxLine.active) {
            if (state.strafeHelper.max) {
                DF_DrawStrafeLine(maxLine);
            }
            if (state.strafeHelper.triangles && activeLine.onScreen && activeLine.active) {
                DF_DrawTriangle(activeOpt, activeMax);
            }
        }

        //draw the alternate max line/triangle
        if (rearMaxLine.onScreen && rearMaxLine.active) { //rear max line is on the screen
            if (state.strafeHelper.max) {
                DF_DrawStrafeLine(rearMaxLine);
            }
            if (state.strafeHelper.triangles && rearActiveLine.onScreen && rearActiveLine.active) {
                DF_DrawTriangle(rearActiveOpt, rearActiveMax);
            }
        }
    }

    //get the center lines
    if(state.strafeHelper.center && state.physics.hasAirControl){
        centerLine = DF_GetLine(KEY_CENTER, qfalse, qfalse);
        if(state.strafeHelper.rear){
            rearCenterLine = DF_GetLine(KEY_CENTER, qtrue, qfalse);
        }
    }

    //draw the inactive lines
    for(i = 0; i < KEY_CENTER; i++){
        if(i != state.moveDir){ //don't need to recalc it for active
            //normal opt angle
            line = DF_GetLine(i, qfalse, qfalse);
            if(line.onScreen && !line.active){
                DF_DrawStrafeLine(line);
            }

            //alternate opt angle
            if(state.strafeHelper.rear || i == KEY_W){ //player has rear lines enabled, or it is W
                rearLine = DF_GetLine(i, qtrue, qfalse);
                if(rearLine.onScreen && !rearLine.active){
                    DF_DrawStrafeLine(rearLine);
                }
            }
        }
    }

    //draw the center lines
    if(state.strafeHelper.center && state.physics.hasAirControl) {
        if(centerLine.onScreen){
            DF_DrawStrafeLine(centerLine);
        }
        if(state.strafeHelper.rear && rearCenterLine.onScreen){
            DF_DrawStrafeLine(rearCenterLine);
        }
    }

    //draw the active opt line
    if(activeLine.onScreen && activeLine.active){
        DF_DrawStrafeLine(activeLine);
    }

    //draw the alternate active opt line
    if((state.strafeHelper.rear || state.moveDir == KEY_W)
       && (rearActiveLine.onScreen && rearActiveLine.active)) {
        DF_DrawStrafeLine(rearActiveLine);
    }

    //set wasOnGround here, we need this as friction only gets applied if on the ground for more than 1 frame
    if(state.onGround){
        state.cgaz.wasOnGround = qtrue;
    } else {
        state.cgaz.wasOnGround = qfalse;
    }
}

/* Strafehelper Setters */

//sets the dfstate function used for strafehelper calculations
void DF_SetPlayerState(centity_t	*cent)
{
	state.moveStyle = DF_GetMovePhysics();
    state.velocity = cg.predictedPlayerState.velocity;
    if (state.moveStyle == MV_SWOOP && cg.predictedPlayerState.m_iVehicleNum) {
        centity_t *vehCent = &cg_entities[cg.predictedPlayerState.m_iVehicleNum];
        state.velocity = vehCent->currentState.pos.trDelta; //jerky otherwise?
    }
    if (cg.clientNum == cg.predictedPlayerState.clientNum && !cg.demoPlayback) // are we a real client
    {
        DF_SetClientReal();
    }
    else if (cg.snap) //or are we a spectator/demo
    {
        DF_SetClient();
    } else {
        return;
    }
    DF_SetPhysics();
    state.onGround = (qboolean)(cg.predictedPlayerState.groundEntityNum == ENTITYNUM_WORLD); //on ground this frame
    if (!(cg_strafeHelper.integer & SHELPER_ORIGINAL) && !cg.renderingThirdPerson)
        VectorCopy(cg.refdef.vieworg, state.viewOrg);
    else
        VectorCopy(cg.predictedPlayerState.origin, state.viewOrg);
    VectorCopy(cg.predictedPlayerState.viewangles, state.viewAngles);
    DF_SetCGAZ(cent);
}

//sets parts of the dfstate struct for a non-predicted client (spectator/demo playback)
void DF_SetClient(){
    state.moveDir = cg.snap->ps.movementDir;
    state.cmd = DF_DirToCmd(state.moveDir);
    if (cg.snap->ps.pm_flags & PMF_JUMP_HELD) {
        state.cmd.upmove = 127;
    }
}

//sets parts of the dfstate struct for a predicted client
void DF_SetClientReal(){
    state.moveDir = cg.predictedPlayerState.movementDir; //0-7 movement dir
    trap->GetUserCmd(trap->GetCurrentCmdNumber(), &state.cmd);
}

//sets the constants relative to the current movement styles physics
void DF_SetPhysics() {
    state.physics.stopspeed = pm_stopspeed;
    state.physics.duckscale = DF_GetDuckScale();
    state.physics.swimscale = pm_swimScale;
    state.physics.wadescale = pm_wadeScale;
    state.physics.accelerate = DF_GetAccelerate();
    state.physics.airaccelerate = DF_GetAirAccelerate();
    state.physics.wateraccelerate = pm_wateraccelerate;
    state.physics.flightfriction = pm_flightfriction;
    state.physics.friction = DF_GetFriction();
    state.physics.airaccelerate = DF_GetAirAccelerate();
    state.physics.airstopaccelerate = DF_GetAirStopAccelerate();
    state.physics.airstrafewishspeed = DF_GetAirStrafeWishspeed();
    state.physics.airstrafeaccelerate = DF_GetAirStrafeAccelerate();
    state.physics.airdecelrate = pm_sp_airDecelRate;
    state.physics.aircontrol = pm_cpm_aircontrol;
    state.physics.hasAirControl = DF_HasAirControl();
    state.physics.hasForceJumps = DF_HasForceJumps();
    state.physics.hasAutoJump = DF_HasAutoJump();
}

//calls functions that sets values to the cgaz struct
void DF_SetCGAZ(centity_t *cent){
    DF_SetFrameTime();
    DF_SetCurrentSpeed(cent);
    DF_SetVelocityAngles();
    state.cgaz.wishspeed = DF_GetWishspeed(state.cmd);
}

/* CGAZ Setters */

//sets the frametime for the cgaz struct
void DF_SetFrameTime(){
    float frameTime;
    //get the frametime
    if (cg_strafeHelper_FPS.value < 1) {
        frameTime = ((float) cg.frametime * 0.001f);
    }
    else if (cg_strafeHelper_FPS.value > 1000) {
        frameTime = 1;
    } else {
        frameTime = 1 / cg_strafeHelper_FPS.value;
    }
    //set the frametime
    state.cgaz.frametime = frameTime;
}

//sets the current speed for the cgaz struct
void DF_SetCurrentSpeed(centity_t *cent) {
	vec_t * velocity;
	if (cg.predictedPlayerState.m_iVehicleNum) {
		centity_t *vehCent = &cg_entities[cg.predictedPlayerState.m_iVehicleNum];
		velocity = (cent->currentState.clientNum == cg.clientNum ?
				vehCent->playerState->velocity : vehCent->currentState.pos.trDelta);
		state.cgaz.currentSpeed = sqrtf(velocity[0] * velocity[0] + velocity[1] * velocity[1]); // is this right?
	}
	else {
		velocity = (cent->currentState.clientNum == cg.clientNum ?
				cg.predictedPlayerState.velocity : cent->currentState.pos.trDelta);
		state.cgaz.currentSpeed = sqrtf(velocity[0] * velocity[0] + velocity[1] * velocity[1]); // is this right?
	}
}

//sets the velocity angle for the cgaz struct
void DF_SetVelocityAngles()
{
    vec3_t xyVel = { 0 };
    static vec3_t velAngles = { 0 };
    //set the velocity
    VectorCopy(state.velocity, xyVel);
    xyVel[2] = 0;
    //get the velocity angle
    vectoangles(xyVel, velAngles);
    //set the velocity angle
    VectorCopy(velAngles, state.cgaz.velocityAngles);
}

/* Strafehelper/Line Setters/Getters */

//set the strafehelper user settings to the struct
void DF_SetStrafeHelper(){
    float lineWidth;
    int sensitivity = cg_strafeHelperPrecision.integer;
    int LINE_HEIGHT = (int)((float)SCREEN_HEIGHT * 0.5f - 10.0f); //240 is midpoint, so it should be a little higher so crosshair is always on it.
    vec4_t twoKeyColor = { 1, 1, 1, 0.75f }; //WA,WD,SA,SD
    vec4_t oneKeyColor = { 0.5f, 1, 1, 0.75f }; //A, D
    vec4_t oneKeyColorAlt = { 1, 0.75f, 0.0f, 0.75f }; //W, S, Center
    vec4_t rearColor = { 0.75f, 0,1, 0.75f };
    vec4_t activeColor = { 0, 1, 0, 1.0f };

    //set the default colors
    Vector4Copy(twoKeyColor, state.strafeHelper.twoKeyColor);
    Vector4Copy(oneKeyColor, state.strafeHelper.oneKeyColor);
    Vector4Copy(oneKeyColorAlt, state.strafeHelper.oneKeyColorAlt);
    Vector4Copy(rearColor, state.strafeHelper.rearColor);
    Vector4Copy(activeColor, state.strafeHelper.activeColor);

    //set the line height
    state.strafeHelper.LINE_HEIGHT = LINE_HEIGHT;

    //set the precision/sensitivity
    if (cg_strafeHelperPrecision.integer < 100)
        sensitivity = 100;
    else if (cg_strafeHelperPrecision.integer > 10000)
        sensitivity = 10000;
    state.strafeHelper.sensitivity = sensitivity;

    //set the line width
    lineWidth = cg_strafeHelperLineWidth.value;
    if (lineWidth < 0.25f)
        lineWidth = 0.25f;
    else if (lineWidth > 5)
        lineWidth = 5;
    state.strafeHelper.lineWidth = lineWidth;

    //set the offset
    state.strafeHelper.offset = cg_strafeHelperOffset.value * 0.01f;

    //is center enabled
    state.strafeHelper.center = qfalse;
    if(cg_strafeHelper.integer & SHELPER_CENTER){
        state.strafeHelper.center = qtrue;
    }

    //is rear enabled
    state.strafeHelper.rear = qfalse;
    if(cg_strafeHelper.integer & SHELPER_REAR){
        state.strafeHelper.rear = qtrue;
    }

    //is max enabled
    state.strafeHelper.max = qfalse;
    if(cg_strafeHelper.integer & SHELPER_MAX){
        state.strafeHelper.max = qtrue;
    }

    //is triangle enabled
    state.strafeHelper.triangles = qfalse;
    if(cg_strafeHelper.integer & SHELPER_ACCELZONES){
        state.strafeHelper.triangles = qtrue;
    }
}

//Take a moveDir and returns a cmd
usercmd_t DF_DirToCmd(int moveDir){
    usercmd_t outCmd;
    memcpy(&outCmd, &state.cmd, sizeof(usercmd_t));
    switch(moveDir){
        case KEY_W:
            outCmd.forwardmove = 127;
            outCmd.rightmove = 0;
            break;
        case KEY_WA:
            outCmd.forwardmove = 127;
            outCmd.rightmove = -127;
            break;
        case KEY_A:
            outCmd.forwardmove = 0;
            outCmd.rightmove = -127;
            break;
        case KEY_AS:
            outCmd.forwardmove = -127;
            outCmd.rightmove = -127;
            break;
        case KEY_S:
            outCmd.forwardmove = -127;
            outCmd.rightmove = 0;
            break;
        case KEY_SD:
            outCmd.forwardmove = -127;
            outCmd.rightmove = 127;
            break;
        case KEY_D:
            outCmd.forwardmove = 0;
            outCmd.rightmove = 127;
            break;
        case KEY_DW:
            outCmd.forwardmove = 127;
            outCmd.rightmove = 127;
            break;
        default:
            break;
    }
    return outCmd;
}

//get the line struct - big function but no point simplifying it past this state
dfsline DF_GetLine(int moveDir, qboolean rear, qboolean max) {
    dfsline lineOut = { 0 }; //the line we will be returning
    qboolean active = qfalse, draw = qfalse;
    float fakeWishspeed;
    float delta, angle = 0;

    //make a fake usercmd for the line we are going to get
    usercmd_t fakeCmd = DF_DirToCmd(moveDir);
    fakeCmd.upmove = state.cmd.upmove; //get the real upmove value
    fakeWishspeed = DF_GetWishspeed(fakeCmd); //get the wishspeed for the fake cmd
    //check if the fake command matches the real command, if it does, the line is active (currently pressed)
    if((state.cmd.rightmove == fakeCmd.rightmove) && (state.cmd.forwardmove == fakeCmd.forwardmove)){
        active = qtrue;
    } else {
        active = qfalse;
    }

    //Here we do some checks that determine if a line should be drawn or not
    if(moveDir % 2 == 0){ //if moveDir is even - it's a single key press
        if(state.physics.hasAirControl){ //air control uses the center line if on for single key presses
            if(moveDir < KEY_CENTER){
                    draw = qtrue;
            } else { //it's the center line
                if (moveDir == KEY_CENTER && state.strafeHelper.center) {
                    draw = qtrue;
                    if ((state.cmd.forwardmove == 0 && state.cmd.rightmove != 0) || (state.cmd.forwardmove == 0 && state.cmd.rightmove == 0)) {
                        active = qtrue; //center is active when A/D or no keys are pressed
                    } else {
                        active = qfalse;
                    }
                    angle = 0.0f; //center line
                    if (rear) {
                        angle = 180.0f; //rear center line
                    }
                } else {
                    draw = qfalse;
                }
            }
        } else if(moveDir != KEY_CENTER){ //don't draw the center line when there is no air control
            draw = qtrue;
        }
    } else { //if moveDir is odd - 2 keys pressed (QW doesn't accel on these keys)
        draw = qtrue;
    }
    if(moveDir == KEY_S && !state.strafeHelper.rear) { //only show S lines when rear setting is on
        draw = qfalse;
    }
    //Now we get the angle offset by the key press
    if(moveDir != KEY_CENTER) { //center has a fixed location

        //are we getting the optimum angle or the maximum angle
        if (!max) {
            delta = CGAZ_Opt(state.cgaz.wasOnGround && state.onGround, state.physics.accelerate,
                             state.cgaz.currentSpeed,
                             fakeWishspeed, state.cgaz.frametime, state.physics.friction, state.physics.airaccelerate) +
                    state.strafeHelper.offset;
        } else {
            delta = CGAZ_Max(state.cgaz.wasOnGround && state.onGround, state.physics.accelerate,
                             state.cgaz.currentSpeed,
                             fakeWishspeed, state.cgaz.frametime, state.physics.friction, state.physics.airaccelerate) +
                    state.strafeHelper.offset;
        }

        //Now we get the angle offset by the key press and users strafehelper setting for that key press/line
        //Each keypress is offset by a difference of pi/4 (45 degrees) to its neighbour(moveDir +/- 1), and the rear
        //angle is offset by pi (180 degrees) away from it's corresponding forward angle, but doing it like this since
        //compiler can optimise it, and it is more readable this way.
        switch (moveDir) {
            case KEY_W:
                if (!(cg_strafeHelper.integer & SHELPER_W)) {
                    draw = qfalse;
                }
                if (draw) {
                    if(!rear)
                        angle = 45.0f + delta;
                    else
                        angle = -45.0f - delta;
                }
                break;
            case KEY_WA:
                if (!(cg_strafeHelper.integer & SHELPER_WA)) {
                    draw = qfalse;
                }
                if (draw) {
                    if(!rear)
                        angle = delta;
                    else
                        angle = -90.0f - delta;
                }
                break;
            case KEY_A:
                if (!(cg_strafeHelper.integer & SHELPER_A)) {
                    draw = qfalse;
                }
                if (draw) {
                    if(!rear)
                        angle = -45.0f + delta;
                    else
                        angle = 225.0f - delta;
                }
                break;
            case KEY_AS:
                if (!(cg_strafeHelper.integer & SHELPER_SA)) {
                    draw = qfalse;
                }
                if (draw) {
                    if(!rear)
                        angle = -90 + delta;
                    else
                        angle = 180.0f - delta;
                }
                break;
            case KEY_S:
                if (!(cg_strafeHelper.integer & SHELPER_S )) {
                    draw = qfalse;
                }
                if (draw) {
                    if(!rear)
                        angle = 225.0f + delta;
                    else
                        angle = -225.0f - delta;
                }
                break;
            case KEY_SD:
                if (!(cg_strafeHelper.integer & SHELPER_SD)) {
                    draw = qfalse;
                }
                if (draw) {
                    if(!rear)
                        angle = 90.0f - delta;
                    else
                        angle = 180.0f + delta;
                }
                break;
            case KEY_D:
                if (!(cg_strafeHelper.integer & SHELPER_D)) {
                    draw = qfalse;
                }
                if (draw) {
                    if(!rear)
                        angle = 45.0f - delta;
                    else
                        angle = 135.0f + delta;
                }
                break;
            case KEY_DW:
                if (!(cg_strafeHelper.integer & SHELPER_WD)) {
                    draw = qfalse;
                }
                if (draw) {
                    if(!rear)
                        angle = -delta;
                    else
                        angle = 90.0f + delta;
                }
                break;
            default:
                break;
        }
    }

    //Get the screen x position of the line
    if(draw){
        lineOut.active = active;
        lineOut.angle = angle;
        DF_SetAngleToX(&lineOut);
        if(lineOut.onScreen){
            DF_SetLineColor(&lineOut, moveDir, max);
        }
    }
    return lineOut;
}

//get line x values to pass to drawstrafeine
void DF_SetAngleToX(dfsline *inLine) {
    vec3_t start, angs, forward, delta, line;
    float x = 0, y = 0;
    //get the view angles
    VectorCopy(state.viewOrg, start);
    //get the velocity angles
    VectorCopy(state.cgaz.velocityAngles, angs);
    //set distance from velocity angles to optimum
    angs[YAW] += inLine->angle;
    //get forward angle
    AngleVectors(angs, forward, NULL, NULL);
    VectorCopy(angs, inLine->angs);
    // set the line length
    VectorScale(forward, (float)state.strafeHelper.sensitivity, delta);
    //set the line coords
    line[0] = delta[0] + start[0];
    line[1] = delta[1] + start[1];
    line[2] = start[2];
    // is it on the screen?
    if (!CG_WorldCoordToScreenCoord(line, &x, &y)) {
        inLine->onScreen = qfalse;
    } else {
        inLine->onScreen = qtrue;
        inLine->x = x;
        inLine->y = y;
    }
}

//set the color of the line
void DF_SetLineColor(dfsline* inLine, int moveDir, qboolean max){
    vec4_t color = { 1, 1, 1, 0.75f };
    //get the default line color
    Vector4Copy(color,  inLine->color);
    //set the colors
    if (inLine->active) {
        if (cg_strafeHelperActiveColor.value) { //does the user have a custom active color set
            color[0] = cg.strafeHelperActiveColor[0];
            color[1] = cg.strafeHelperActiveColor[1];
            color[2] = cg.strafeHelperActiveColor[2];
            color[3] = cg.strafeHelperActiveColor[3];
        } else { //make it green
            color[0] = state.strafeHelper.activeColor[0];
            color[1] = state.strafeHelper.activeColor[1];
            color[2] = state.strafeHelper.activeColor[2];
            color[3] = state.strafeHelper.activeColor[3];
        }
        if(!max) {
            memcpy(inLine->color, color, sizeof(vec4_t));
        } else {
            memcpy(inLine->color, colorRed, sizeof(vec4_t)); //max is always red
        }
    } else { //set the other colors
        if (moveDir == KEY_WA || moveDir == KEY_DW) {
            memcpy(inLine->color, state.strafeHelper.twoKeyColor, sizeof(vec4_t));
        } else if (moveDir == KEY_A || moveDir == KEY_D) {
            memcpy(inLine->color, state.strafeHelper.oneKeyColor, sizeof(vec4_t));
        } else if (moveDir == KEY_W || moveDir == KEY_S || moveDir == KEY_CENTER) {
            memcpy(inLine->color, state.strafeHelper.oneKeyColorAlt, sizeof(vec4_t));
        } else if (moveDir == KEY_AS || moveDir == KEY_SD) {
            memcpy(inLine->color, state.strafeHelper.rearColor, sizeof(vec4_t));
        }
		inLine->color[3] = cg_strafeHelperInactiveAlpha.value / 255.0f;
	}
}

/* Strafehelper Value Calculators */

//calculates the optimum cgaz angle
float CGAZ_Opt(qboolean onGround, float accelerate, float currentSpeed, float wishSpeed, float frametime, float friction, float airaccelerate){
    float optimumDelta;
    if (onGround) {
        optimumDelta = acosf(
                ((wishSpeed - (accelerate * wishSpeed * frametime)) /
                          (currentSpeed * (1 - friction * frametime)))) * (180.0f / M_PI) -
                       45.0f;
    } else {
        optimumDelta = acosf(
                ((wishSpeed - (airaccelerate * wishSpeed * frametime)) /
                          currentSpeed)) * (180.0f / M_PI) - 45.0f;
    }
    if (optimumDelta < 0 || optimumDelta > 360) {
        optimumDelta = 0;
    }
    return optimumDelta;
}

//calculates the maximum cgaz angle
 float CGAZ_Max(qboolean onGround, float accelerate, float currentSpeed, float wishSpeed, float frametime, float friction, float airaccelerate){
    float maxDeltaAngle = 0;
    if (!onGround || (onGround && state.moveStyle == MV_SLICK)) {
        maxDeltaAngle = acosf(((-(accelerate * wishSpeed * frametime) / (2.0f * currentSpeed)))) * (180.0f / M_PI) - 45.0f;
    }
    if (maxDeltaAngle < 0 || maxDeltaAngle > 360) {
        maxDeltaAngle = 0;
    }
    return maxDeltaAngle;
}

//takes a user commmand and returns the emulated wishspeed as a float
 float DF_GetWishspeed(usercmd_t inCmd){
    int         i;
    vec3_t		wishvel;
    float		fmove, smove;
    vec3_t		forward, right, up;
    float		wishspeed;
    //float		scale;

    fmove = inCmd.forwardmove;
    smove = inCmd.rightmove;

    //scale = DF_GetCmdScale( inCmd ); - for OCPM/SP

    AngleVectors(state.viewAngles, forward, right, up);
    // project moves down to flat plane
    forward[2] = 0;
    right[2] = 0;
    VectorNormalize (forward);
    VectorNormalize (right);

    for ( i = 0 ; i < 2 ; i++ ) {
        wishvel[i] = forward[i]*fmove + right[i]*smove;
    }
    wishvel[2] = 0; //wishdir
    wishspeed = VectorNormalize(wishvel);

    if (cg.predictedPlayerState.pm_type == PM_JETPACK) {
        if (inCmd.upmove <= 0)
            wishspeed *= 0.8f;
        else
            wishspeed *= 2.0f;
    }
    if (state.moveStyle == MV_SWOOP && cg.predictedPlayerState.m_iVehicleNum) {
        centity_t *vehCent = &cg_entities[cg.predictedPlayerState.m_iVehicleNum];
        if (cg.predictedPlayerState.commandTime < vehCent->m_pVehicle->m_iTurboTime) {
            wishspeed = vehCent->m_pVehicle->m_pVehicleInfo->turboSpeed;//1400
        }
        else {
            wishspeed = vehCent->m_pVehicle->m_pVehicleInfo->speedMax;//700
        }
    }

    if(state.moveStyle != MV_SP) {
        wishspeed = cg.predictedPlayerState.speed; //this seems more accurate than using scale?
        //air control has a different wishspeed when using A or D only in the air
        if(!(state.onGround && state.cgaz.wasOnGround) && state.physics.hasAirControl &&
           (wishspeed > state.physics.airstrafewishspeed) && (fmove == 0 && smove != 0)) {
            wishspeed = state.physics.airstrafewishspeed;
        }
    }
    //SP only applies the scale when on the ground and also encourages deceleration away from current velocity
    if(state.moveStyle == MV_SP){
        if (!(cg.predictedPlayerState.pm_flags & PMF_JUMP_HELD) && inCmd.upmove > 0) { //Also, wishspeed *= scale.  Scale is different cuz of upmove in air.  Only works ingame not from spec
            wishspeed /= 1.41421356237f; //umm.. dunno.. divide by sqrt(2)
        }
    }

    if(state.moveStyle == MV_QW){
        if(wishspeed > 30){
            wishspeed = 30;
        }
    }
    return wishspeed;
}

/*
//takes a user command and returns the emulated command scale as a float
 float DF_GetCmdScale( usercmd_t cmd) {
    int		max;
    float	total;
    float	scale;
    signed char		umove = 0; //cmd->upmove;
    //don't factor upmove into scaling speed

    if(state.moveStyle == MV_SP){ //upmove velocity scaling add ocpm
        umove = state.cmd.upmove;
    }
    max = abs( cmd.forwardmove );
    if ( abs( cmd.rightmove ) > max ) {
        max = abs( cmd.rightmove );
    }
    if ( abs( umove ) > max ) {
        max = abs( umove );
    }
    if ( !max ) {
        scale = 0;
    }

    total = (float)sqrt( cmd.forwardmove * cmd.forwardmove
                         + cmd.rightmove * cmd.rightmove + umove * umove );
    scale = (float)state.cgaz.currentSpeed * (float)max / ( 127.0f * total );

    return scale;
} */

/* Strafehelper Style Distributor */

//takes a strafe line and draws it according to the strafehelper style set
 void DF_DrawStrafeLine(dfsline line) {

    if (cg_strafeHelper.integer & SHELPER_ORIGINAL) {
        float startx, starty;
        int cutoff = SCREEN_HEIGHT - cg_strafeHelperCutoff.integer; //Should be between 480 and LINE_HEIGHT
        if (cutoff > SCREEN_HEIGHT)
            cutoff = SCREEN_HEIGHT;
        if (cutoff < state.strafeHelper.LINE_HEIGHT + 20)
            cutoff = state.strafeHelper.LINE_HEIGHT + 20;

        if (CG_WorldCoordToScreenCoord(state.viewOrg, &startx, &starty))
            DF_DrawLine(startx - state.strafeHelper.lineWidth / 2.0f, starty, line.x, line.y, state.strafeHelper.lineWidth, line.color, line.color[3], (float)cutoff);
    }

    if (cg_strafeHelper.integer & SHELPER_UPDATED) { //draw the updated style here
        int cutoff = (int)SCREEN_HEIGHT - cg_strafeHelperCutoff.integer;
        int heightIn = state.strafeHelper.LINE_HEIGHT;

        if (cg_strafeHelper.integer & SHELPER_TINY) {
            cutoff = state.strafeHelper.LINE_HEIGHT + 15;
            heightIn = state.strafeHelper.LINE_HEIGHT + 5;
        } else if (cutoff < state.strafeHelper.LINE_HEIGHT + 20) {
            cutoff = state.strafeHelper.LINE_HEIGHT + 20;
        } else if ((float)cutoff > SCREEN_HEIGHT) {
            cutoff = SCREEN_WIDTH;
        }
        DF_DrawLine((0.5f * SCREEN_WIDTH), SCREEN_HEIGHT, line.x, (float)heightIn, state.strafeHelper.lineWidth, line.color, line.color[3], (float)cutoff);
    }

    if (cg_strafeHelper.integer & SHELPER_CGAZ) { //draw the cgaz style strafehelper
        if (cg_strafeHelperCutoff.integer > 256) {
            DF_DrawLine(line.x, (0.5f * SCREEN_HEIGHT) + 4, line.x, (0.5f * SCREEN_HEIGHT) - 4, state.strafeHelper.lineWidth, line.color, 0.75f, 0); //maximum cutoff
        } else {
            DF_DrawLine(line.x, (0.5f * SCREEN_HEIGHT) + 20 - (float)cg_strafeHelperCutoff.integer / 16.0f, line.x, (0.5f * SCREEN_HEIGHT) - 20 + (float)cg_strafeHelperCutoff.integer / 16.0f, state.strafeHelper.lineWidth, line.color, 0.75f, 0); //default/custom cutoff
        }
    }

    if (cg_strafeHelper.integer & SHELPER_WSW && line.active) { //draw the wsw style strafehelper, not sure how to deal with multiple lines for W only so we don't draw any, the proper way is to tell which line we are closest to aiming at and display the strafehelper for that
        float width = (float)(-4.444 * AngleSubtract(state.viewAngles[YAW], line.angs[YAW]));
        CG_FillRect((0.5f * SCREEN_WIDTH), (0.5f * SCREEN_HEIGHT), width, 12, colorTable[CT_RED]);
    }

    if (cg_strafeHelper.integer & SHELPER_WEZE && line.active) { //call the weze style strafehelper function
        DF_DrawStrafehelperWeze(state.moveDir, line);
    }

    if (cg_strafeHelper.integer & SHELPER_SOUND && line.active) { //strafehelper sounds - don't do them for the center line, since it's not really a strafe
        DF_StrafeHelperSound(100 * AngleSubtract(state.viewAngles[YAW], line.angs[YAW]));
    }
}

/* Drawing Functions */

//draws a line on the screen
 void DF_DrawLine(float x1, float y1, float x2, float y2, float size, vec4_t color, float alpha, float ycutoff) {
    float stepx, stepy, length = sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    int i;

    if (length < 1)
        length = 1;
    else if (length > 2000)
        length = 2000;
    if (!ycutoff)
        ycutoff = SCREEN_HEIGHT;

    x1 -= 0.5f * size;
    x2 -= 0.5f * size;
    stepx = (x2 - x1) / (length / size);
    stepy = (y2 - y1) / (length / size);

    trap->R_SetColor(color);

    for (i = 0; i <= (int)(length / size); i++) {
        if (x1 < SCREEN_WIDTH && y1 < SCREEN_HEIGHT && y1 < ycutoff)
            CG_DrawPic(x1, y1, size,  size, cgs.media.whiteShader);
        x1 += stepx;
        y1 += stepy;
    }
    trap->R_SetColor(NULL);

}

//draws the weze strafehelper
 void DF_DrawStrafehelperWeze(int moveDir, dfsline inLine) {
    usercmd_t cmd = { 0 };
    float length;
    float diff;
    float midx;
    float midy;
    vec3_t velocity_copy;
    vec3_t viewangle_copy;
    vec3_t velocity_angle;
    float optiangle, g_speed, accel;

    midx = (0.5f * SCREEN_WIDTH);
    midy = 0.5f * SCREEN_HEIGHT;
    VectorCopy(state.velocity, velocity_copy);
    velocity_copy[2] = 0;
    VectorCopy(state.viewAngles, viewangle_copy);
    viewangle_copy[PITCH] = 0;
    length = VectorNormalize(velocity_copy);
    cmd = DF_DirToCmd(moveDir);


    g_speed=cg.predictedPlayerState.speed;
    accel = g_speed;
    accel*=8.0f;
    accel/=1000;
    optiangle=(g_speed-accel)/length;
    if ((optiangle <= 1) && (optiangle >= -1))
        optiangle = acosf(optiangle);
    else
        optiangle = 0;
    length /= 5;
    if (length > (0.5f * SCREEN_HEIGHT))
        length = (float)(0.5f * SCREEN_HEIGHT);
    vectoangles(velocity_copy, velocity_angle);
    diff = AngleSubtract(viewangle_copy[YAW], velocity_angle[YAW]);
    diff = diff / 180 * M_PI;

    DF_DrawLine(midx, midy, midx + length * sinf(diff), midy - length * cosf(diff), 1, colorRed, 0.75f, 0);
    DF_DrawLine(midx, midy, midx + (float)cmd.rightmove, midy - (float)cmd.forwardmove, 1, colorCyan, 0.75f, 0);
    DF_DrawLine(midx, midy, midx + length / 2 * sinf(diff + optiangle), midy - length / 2 * cosf(diff + optiangle), 1, colorRed, 0.75f, 0);
    DF_DrawLine(midx, midy, midx + length / 2 * sinf(diff - optiangle), midy - length / 2 * cosf(diff - optiangle), 1, colorRed, 0.75f, 0);
}

//plays the strafehelper sounds
 void DF_StrafeHelperSound(float difference) {
    if (difference > -40.0f && difference < 10.0f) //Under aiming by a bit, but still good
        trap->S_StartLocalSound(cgs.media.hitSound4, CHAN_LOCAL_SOUND);
}

//sets the color of the triangles based on accel - code repeated from speedometer
 void DF_SetAccelColor(){
    int t, i;
    float total, avgAccel;
    const float currentSpeed = state.cgaz.currentSpeed;
    static float lastSpeed = 0, previousAccels[ACCEL_SAMPLES];
    const float accel = currentSpeed - lastSpeed;
    static unsigned int index;
    static int lastupdate;
    vec4_t color = {0,0,0,0};

    lastSpeed = currentSpeed;
    t = trap->Milliseconds();

    if (t - lastupdate > 5)	//don't sample faster than this
    {
        lastupdate = t;
        previousAccels[index % ACCEL_SAMPLES] = accel;
        index++;
    }

    total = 0;
    for (i = 0; i < ACCEL_SAMPLES; i++) {
        total += previousAccels[i];
    }
    if (!total) {
        total = 1;
    }

    avgAccel = total / (float)ACCEL_SAMPLES - 0.0625f;//fucking why does it offset by this number

    if (avgAccel > 0.0f) //good accel = green
    {
        color[0] = 0;
        color[1] = 1.0f;
        color[2] = 0;
        color[3] = 0.4f;
    }
    else if (avgAccel < 0.0f) //bad accel = red
    {
        color[0] = 1.0f;
        color[1] = 0;
        color[2] = 0;
        color[3] = 0.4f;
    }
    else //no accel = white
    {
        color[0] = 1.0f;
        color[1] = 1.0f;
        color[2] = 1.0f;
        color[3] = 0.4f;
    }
    trap->R_SetColor(NULL);
    trap->R_SetColor(color);
}

//draws the acceleration zone triangle
 void DF_DrawTriangle(float start, float end) {
    if(start <= SCREEN_WIDTH && start >= 0 && end <= SCREEN_WIDTH && end >= 0) {
        DF_SetAccelColor();
        CG_DrawPic(end, (0.5f * SCREEN_HEIGHT) - 4.0f, (start - end), 8.0f, cgs.media.leftTriangle);
    }
    trap->R_SetColor(NULL); //set the color back to null
}

void CG_AddSpeedGraphFrameInfo( void ) {  //CG_DrawSpeedGraph
    speedgraph.frameSamples[ speedgraph.frameCount & ( SPEED_SAMPLES - 1) ] = (int)state.cgaz.currentSpeed;
    speedgraph.frameCount++;
}
/*  Speedometer */
/*
===================
CG_GraphAddSpeed
tremulous - append a speed to the sample history for the speed graph
===================
*/
 void DF_GraphAddSpeed(void) {
    float speed;
    vec3_t vel;

    VectorCopy(cg.snap->ps.velocity, vel);

    speed = VectorLength(vel);

    if (speed > speedSamples[maxSpeedSample])
    {
        maxSpeedSample = oldestSpeedSample;
        speedSamples[oldestSpeedSample++] = speed;
        oldestSpeedSample %= SPEEDOMETER_NUM_SAMPLES;
        return;
    }

    speedSamples[oldestSpeedSample] = speed;
    if (maxSpeedSample == oldestSpeedSample++) // if old max was overwritten find a new one
    {
        int i;
        for (maxSpeedSample = 0, i = 1; i < SPEEDOMETER_NUM_SAMPLES; i++)
        {
            if (speedSamples[i] > speedSamples[maxSpeedSample])
                maxSpeedSample = i;
        }
    }

    oldestSpeedSample %= SPEEDOMETER_NUM_SAMPLES;
}
/*
===================
CG_DrawSpeedGraph
tremulous - speedgraph initially ported by TomArrow
===================
*/
 void DF_DrawSpeedGraph(rectDef_c* rect, const vec4_t foreColor, vec4_t backColor) {
    int i;
    float val, max, top;
    // color of graph is interpolated between these values
    const vec3_t slow = { 0.0f, 0.0f, 1.0f };
    const vec3_t medium = { 0.0f, 1.0f, 0.0f };
    const vec3_t fast = { 1.0f, 0.0f, 0.0f };
    vec4_t color;

    max = speedSamples[maxSpeedSample];
    if (max < SPEEDOMETER_MIN_RANGE)
        max = SPEEDOMETER_MIN_RANGE;

    trap->R_SetColor(backColor);
    CG_DrawPic(rect->x, rect->y, rect->w, rect->h, cgs.media.whiteShader);

    Vector4Copy(foreColor, color);

    for (i = 1; i < SPEEDOMETER_NUM_SAMPLES; i++)
    {
        val = speedSamples[(oldestSpeedSample + i) % SPEEDOMETER_NUM_SAMPLES];
        if (val < SPEED_MED)
            VectorLerp(val / SPEED_MED, slow, medium, color);
        else if (val < SPEED_FAST)
            VectorLerp((val - SPEED_MED) / (SPEED_FAST - SPEED_MED),
                       medium, fast, color);
        else
            VectorCopy(fast, color);
        trap->R_SetColor(color);
        top = rect->y + (1 - val / max) * rect->h;
        CG_DrawPic((rect->x + ((float)i / (float)SPEEDOMETER_NUM_SAMPLES) * rect->w) , top,
                   (rect->w / (float)SPEEDOMETER_NUM_SAMPLES), val * rect->h / max,
                   cgs.media.whiteShader);
    }
    trap->R_SetColor(NULL);
}

 void DF_DrawSpeedGraphOld( void ) {
    int		a, i, aw;
    float	x, y, v;
    float	ax, ay, ah, range; // mid, range;
//	int		color;
    float	vscale;

    x = SCREEN_WIDTH - (float)cg_lagometerX.integer * cgs.widthRatioCoef;
    y = SCREEN_HEIGHT - cg_lagometerY.integer - 56;

    if (cg_hudFiles.integer == 0) {
        y -= 16;
    }


    trap->R_SetColor(NULL);

    if (cg_lagometer.integer == 1 || cg_lagometer.integer == 2)
        CG_DrawPic(x, y, 48 * cgs.widthRatioCoef, 48, cgs.media.lagometerShader);

    if (cg_lagometer.integer == 2 || (cg_lagometer.integer == 3 && cg.currentSpeed != 0))
        CG_Text_Paint(x + 2 * cgs.widthRatioCoef, y, 0.5f, colorWhite, va("%.0f", cg.currentSpeed), 0, 0, ITEM_TEXTSTYLE_SHADOWEDMORE, FONT_SMALL);

    if (cg_lagometer.integer != 1 && cg_lagometer.integer != 2)
        y -= 96;

    x -= 1.0f * cgs.widthRatioCoef;

    ax = x;
    ay = y;
    aw = 48;
    if (cg_lagometer.integer != 1 && cg_lagometer.integer != 2)
        ah = 48*3;
    else
        ah = 48;

//	color = -1;
    range = ah / 3;
//	mid = ay + range;

    // draw the speed graph
    range = ah;
    vscale = range / (3000);

    for (a = 0 ; a < aw ; a++) {
        i = ( speedgraph.frameCount - 1 - a ) & (SPEED_SAMPLES - 1);
        v = (float)speedgraph.frameSamples[i];
        if (v > 0) {
            trap->R_SetColor(g_color_table[ColorIndex(COLOR_GREEN)]);
            v = v * vscale;
            if (v > range)
                v = range;
            trap->R_DrawStretchPic(ax + (float)(aw - a) * cgs.widthRatioCoef, ay + ah - v, 1 * cgs.widthRatioCoef, v, 0, 0, 0, 0, cgs.media.whiteShader);
        }
    }
    trap->R_SetColor(NULL);
}
/*
===================
CG_DrawJumpHeight
japro - Draw speedometer jump height
===================
*/
 void DF_DrawJumpHeight(centity_t* cent) {
    const vec_t* const velocity = (cent->currentState.clientNum == cg.clientNum ? cg.predictedPlayerState.velocity : cent->currentState.pos.trDelta);
    char jumpHeightStr[32] = { 0 };

    if (!pm || !pm->ps)
        return;

    if (pm->ps->fd.forceJumpZStart == -65536) //Coming back from a tele
        return;

    if (pm->ps->fd.forceJumpZStart && (cg.lastZSpeed > 0) && (velocity[2] <= 0)) {//If we were going up, and we are now going down, print our height.
        cg.lastJumpHeight = pm->ps->origin[2] - pm->ps->fd.forceJumpZStart;
        cg.lastJumpHeightTime = cg.time;
    }

    if ((cg.lastJumpHeightTime > cg.time - 1500) && (cg.lastJumpHeight > 0.0f)) {
        Com_sprintf(jumpHeightStr, sizeof(jumpHeightStr), "%.1f", cg.lastJumpHeight);
        CG_Text_Paint(speedometerXPos * cgs.widthRatioCoef, (float)cg_speedometerY.integer, cg_speedometerSize.value, colorTable[CT_WHITE], jumpHeightStr, 0.0f, 0, ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
    }

    speedometerXPos += 42;

    cg.lastZSpeed = velocity[2];
}
/*
===================
CG_DrawJumpDistance
japro - Draw speedometer jump distance
===================
*/
 void DF_DrawJumpDistance(void) {
    char jumpDistanceStr[64] = { 0 };

    if (!cg.snap)
        return;

    if (cg.predictedPlayerState.groundEntityNum == ENTITYNUM_WORLD) {

        if (!cg.wasOnGround) {//We were just in the air, but now we arnt
            vec3_t distance;

            VectorSubtract(cg.predictedPlayerState.origin, cg.lastGroundPosition, distance);
            cg.lastJumpDistance = sqrtf(distance[0] * distance[0] + distance[1] * distance[1]);
            cg.lastJumpDistanceTime = cg.time;
        }

        VectorCopy(cg.predictedPlayerState.origin, cg.lastGroundPosition);
        cg.wasOnGround = qtrue;
    }
    else {
        cg.wasOnGround = qfalse;
    }

    if ((cg.lastJumpDistanceTime > cg.time - 1500) && (cg.lastJumpDistance > 0.0f)) {
        Com_sprintf(jumpDistanceStr, sizeof(jumpDistanceStr), "%.1f", cg.lastJumpDistance);
        CG_Text_Paint(speedometerXPos * cgs.widthRatioCoef, (float)cg_speedometerY.integer, cg_speedometerSize.value, colorTable[CT_WHITE], jumpDistanceStr, 0.0f, 0, ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
    }

    speedometerXPos += 62;
}
/*
===================
CG_DrawVerticalSpeed
japro - Draw speedometer vertical speed
===================
*/
 void DF_DrawVerticalSpeed(void) {
    char speedStr5[64] = { 0 };
    float vertspeed = cg.predictedPlayerState.velocity[2];

    if (vertspeed < 0)
        vertspeed = -vertspeed;

    if (vertspeed) {
        Com_sprintf(speedStr5, sizeof(speedStr5), "%.0f", vertspeed);
        CG_Text_Paint(speedometerXPos * cgs.widthRatioCoef, (float)cg_speedometerY.integer, cg_speedometerSize.value, colorWhite, speedStr5, 0.0f, 0, ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
    }

    speedometerXPos += 42;
}

 void DF_DrawYawSpeed( void ) {
    static unsigned short previousYaws[YAW_FRAMES];
    static unsigned short index;
    static int    lastupdate; //previous, lastupdate;
    int        t, i, yaw, total;
//    unsigned short frameTime;
//    const int        xOffset = 0;

    const float diff = AngleSubtract(cg.predictedPlayerState.viewangles[YAW], cg.lastYawSpeed);
    float yawspeed = diff / ((float)cg.frametime * 0.001f);
    if (yawspeed < 0)
        yawspeed = -yawspeed;

    t = trap->Milliseconds();
//    frameTime = t - previous;
//    previous = t;
    if (t - lastupdate > 20)    //don't sample faster than this
    {
        lastupdate = t;
        previousYaws[index % YAW_FRAMES] = (int)yawspeed;
        index++;
    }

    total = 0;
    for (i = 0; i < YAW_FRAMES; i++) {
        total += previousYaws[i];
    }
    if (!total) {
        total = 1;
    }
    yaw = (int)((float)total / (float)YAW_FRAMES);

    if (yaw) {
        char yawStr[64] = { 0 };
        if (yawspeed > 320)
            Com_sprintf(yawStr, sizeof(yawStr), "^1%03i", (int)((float)yaw + 0.5f)); //added 8 whitespaces idk how much to add - fixme
        else if (yawspeed > 265)
            Com_sprintf(yawStr, sizeof(yawStr), "^3%03i", (int)((float)yaw + 0.5f)); //added 8 whitespaces idk how much to add - fixme
        else
            Com_sprintf(yawStr, sizeof(yawStr), "%03i", (int)((float)yaw + 0.5f)); //added 8 whitespaces idk how much to add - fixme
        CG_Text_Paint(speedometerXPos * cgs.widthRatioCoef, (float)cg_speedometerY.integer, cg_speedometerSize.value, colorTable[CT_WHITE], yawStr, 0.0f, 0, ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
    }

    cg.lastYawSpeed = cg.predictedPlayerState.viewangles[YAW];

    speedometerXPos += 16;
}

/*
===================
CG_DrawAccelMeter
japro - Draw acceleration meter
===================
*/
 void DF_DrawAccelMeter(void) {
    const float optimalAccel = cg.predictedPlayerState.speed * ((float)cg.frametime / 1000.0f);
    const float potentialSpeed = sqrtf(cg.previousSpeed * cg.previousSpeed - optimalAccel * optimalAccel + 2 * (state.cgaz.wishspeed * optimalAccel));
    float actualAccel, total, percentAccel, x;
    const float accel = state.cgaz.currentSpeed - cg.previousSpeed;
    int i;
    static float previousTimes[PERCENT_SAMPLES];
    static unsigned int index;

    x = speedometerXPos;

    if (cg_speedometer.integer & SPEEDOMETER_GROUNDSPEED)
        x -= 88;
    else
        x -= 52;

    CG_DrawRect((x - 0.75f) * cgs.widthRatioCoef,
                cg_speedometerY.value - 10.75f,
                37.75f * cgs.widthRatioCoef,
                13.75f,
                0.5f,
                colorTable[CT_BLACK]);

    actualAccel = accel;
    if (actualAccel < 0)
        actualAccel = 0.001f;
    else if (actualAccel > (potentialSpeed - state.cgaz.currentSpeed)) //idk how
        actualAccel = (potentialSpeed - state.cgaz.currentSpeed) * 0.99f;

    previousTimes[index % PERCENT_SAMPLES] = actualAccel / (potentialSpeed - state.cgaz.currentSpeed);
    index++;

    total = 0;
    for (i = 0; i < PERCENT_SAMPLES; i++) {
        total += previousTimes[i];
    }
    if (!total) {
        total = 1;
    }
    percentAccel = total / (float)PERCENT_SAMPLES;

    if (percentAccel && state.cgaz.currentSpeed) {
        CG_FillRect((x + 0.25f) * cgs.widthRatioCoef,
                    cg_speedometerY.value - 9.9f,
                    (36 * percentAccel) * cgs.widthRatioCoef,
                    12,
                    colorTable[CT_RED]);
    }
    cg.previousSpeed = state.cgaz.currentSpeed;
}
/*
===================
CG_DrawSpeedometer
japro - Draw the speedometer
===================
*/
 void DF_DrawSpeedometer(void) {
    const char* accelStr, * accelStr2, * accelStr3;
    char speedStr[32] = { 0 }, speedStr2[32] = { 0 }, speedStr3[32] = { 0 };
    vec4_t colorSpeed = { 1, 1, 1, 1 };
    const float currentSpeed = state.cgaz.currentSpeed;
    static float lastSpeed = 0, previousAccels[ACCEL_SAMPLES];
    const float accel = currentSpeed - lastSpeed;
    float total, avgAccel, groundSpeedColor, groundSpeedsColor, currentSpeedColor;
    int t, i;
    static unsigned int index;
    static int lastupdate, jumpsCounter = 0;
    static qboolean clearOnNextJump = qfalse;

    lastSpeed = currentSpeed;

    if (currentSpeed > state.cgaz.wishspeed && !(cg_speedometer.integer & SPEEDOMETER_COLORS))
    {
        currentSpeedColor = 1 / ((currentSpeed / state.cgaz.wishspeed) * (currentSpeed / state.cgaz.wishspeed));
        colorSpeed[1] = currentSpeedColor;
        colorSpeed[2] = currentSpeedColor;
    }

    t = trap->Milliseconds();

    if (t - lastupdate > 5)	//don't sample faster than this
    {
        lastupdate = t;
        previousAccels[index % ACCEL_SAMPLES] = accel;
        index++;
    }

    total = 0;
    for (i = 0; i < ACCEL_SAMPLES; i++) {
        total += previousAccels[i];
    }
    if (!total) {
        total = 1;
    }
    avgAccel = total / (float)ACCEL_SAMPLES - 0.0625f;//fucking why does it offset by this number

    if (avgAccel > 0.0f)
    {
        accelStr = S_COLOR_GREEN "\xb5:";
        accelStr2 = S_COLOR_GREEN "k:";
        accelStr3 = S_COLOR_GREEN "m: ";
    }
    else if (avgAccel < 0.0f)
    {
        accelStr = S_COLOR_RED "\xb5:";
        accelStr2 = S_COLOR_RED "k:";
        accelStr3 = S_COLOR_RED "m: ";
    }
    else
    {
        accelStr = S_COLOR_WHITE "\xb5:";
        accelStr2 = S_COLOR_WHITE "k:";
        accelStr3 = S_COLOR_WHITE "m: ";
    }

    if (!(cg_speedometer.integer & SPEEDOMETER_KPH) && !(cg_speedometer.integer & SPEEDOMETER_MPH))
    {
        Com_sprintf(speedStr, sizeof(speedStr), "   %.0f", floorf(currentSpeed + 0.5f));
        CG_Text_Paint(speedometerXPos * cgs.widthRatioCoef, cg_speedometerY.value, cg_speedometerSize.value, colorWhite, accelStr, 0.0f, 0, ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
        CG_Text_Paint(speedometerXPos * cgs.widthRatioCoef, cg_speedometerY.value, cg_speedometerSize.value, colorSpeed, speedStr, 0.0f, 0, ITEM_ALIGN_LEFT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
    }
    else if (cg_speedometer.integer & SPEEDOMETER_KPH)
    {
        Com_sprintf(speedStr2, sizeof(speedStr2), "   %.1f", currentSpeed * 0.05);
        CG_Text_Paint(speedometerXPos * cgs.widthRatioCoef, cg_speedometerY.value, cg_speedometerSize.value, colorWhite, accelStr2, 0.0f, 0, ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
        CG_Text_Paint(speedometerXPos * cgs.widthRatioCoef, cg_speedometerY.value, cg_speedometerSize.value, colorSpeed, speedStr2, 0.0f, 0, ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
    }
    else if (cg_speedometer.integer & SPEEDOMETER_MPH)
    {
        Com_sprintf(speedStr3, sizeof(speedStr3), "   %.1f", currentSpeed * 0.03106855);
        CG_Text_Paint(speedometerXPos * cgs.widthRatioCoef, cg_speedometerY.value, cg_speedometerSize.value, colorWhite, accelStr3, 0.0f, 0, ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
        CG_Text_Paint(speedometerXPos * cgs.widthRatioCoef, cg_speedometerY.value, cg_speedometerSize.value, colorSpeed, speedStr3, 0.0f, 0, ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
    }
    speedometerXPos += 52;

    if (cg_speedometer.integer & SPEEDOMETER_GROUNDSPEED || (cg_speedometer.integer && (cg_speedometer.integer & SPEEDOMETER_JUMPS))) {
        char speedStr4[32] = { 0 };
        char speedsStr4[32] = { 0 };

        vec4_t colorGroundSpeed = { 1, 1, 1, 1 };
        vec4_t colorGroundSpeeds = { 1, 1, 1, 1 };

        if (cg.predictedPlayerState.groundEntityNum != ENTITYNUM_NONE || cg.predictedPlayerState.velocity[2] < 0) { //On ground or Moving down
            cg.firstTimeInAir = qfalse;
        }
        else if (!cg.firstTimeInAir) { //Moving up for first time
            cg.firstTimeInAir = qtrue;
            cg.lastGroundSpeed = currentSpeed;
            cg.lastGroundTime = cg.time;
            if (cg_speedometer.integer & SPEEDOMETER_JUMPS) {

                if (clearOnNextJump == qtrue) {
                    memset(&cg.lastGroundSpeeds, 0, sizeof(cg.lastGroundSpeeds));
                    jumpsCounter = 0;
                    clearOnNextJump = qfalse;
                }
                cg.lastGroundSpeeds[++jumpsCounter] = cg.lastGroundSpeed; //add last ground speed to the array
            }
        }

        if (cg_speedometer.integer & SPEEDOMETER_JUMPS) {
            if ((cg.predictedPlayerState.groundEntityNum != ENTITYNUM_NONE &&
                 cg.predictedPlayerState.pm_time <= 0 && state.cgaz.currentSpeed < state.cgaz.wishspeed) || state.cgaz.currentSpeed == 0) {
                clearOnNextJump = qtrue;
            }
            if (cg_speedometerJumps.value &&
                (jumpsCounter < cg_speedometerJumps.integer)) { //if we are in the first n jumps
                for (i = 0; i <= cg_speedometerJumps.integer; i++) { //print the jumps
                    groundSpeedsColor = 1 / ((cg.lastGroundSpeeds[i] / state.cgaz.wishspeed) * (cg.lastGroundSpeeds[i] / state.cgaz.wishspeed));
                    Com_sprintf(speedsStr4, sizeof(speedsStr4), "%.0f", cg.lastGroundSpeeds[i]); //create the string
                    if (cg_speedometer.integer & SPEEDOMETER_JUMPSCOLORS1) { //color the string
                        colorGroundSpeeds[1] = groundSpeedsColor;
                        colorGroundSpeeds[2] = groundSpeedsColor;
                    }
                    else if (cg_speedometer.integer & SPEEDOMETER_JUMPSCOLORS2) {
                        if ((jumpsCounter > 0 && (cg.lastGroundSpeeds[i] > cg.lastGroundSpeeds[i - 1])) ||
                            (i == 0 && (cg.lastGroundSpeeds[0] > firstSpeed))) {
                            colorGroundSpeeds[0] = groundSpeedsColor;
                            colorGroundSpeeds[1] = 1;
                            colorGroundSpeeds[2] = groundSpeedsColor;
                        }
                        else {
                            colorGroundSpeeds[0] = 1;
                            colorGroundSpeeds[1] = groundSpeedsColor;
                            colorGroundSpeeds[2] = groundSpeedsColor;
                        }
                    }
                    if (strcmp(speedsStr4, "0") != 0) {
                        CG_Text_Paint((jumpsXPos * cgs.widthRatioCoef), cg_speedometerJumpsY.value,
                                      cg_speedometerSize.value, colorGroundSpeeds, speedsStr4, 0.0f, 0,
                                      ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE); //print the jump
                        jumpsXPos += 52; //shift x
                    }
                }
            }
            else if (cg_speedometerJumps.value &&
                     jumpsCounter == cg_speedometerJumps.integer) { //we out of the first n jumps
                firstSpeed = cg.lastGroundSpeeds[0];
                for (i = 0; i <= cg_speedometerJumps.integer; i++) { //shuffle jumps array down
                    cg.lastGroundSpeeds[i] = cg.lastGroundSpeeds[i + 1];
                }
                jumpsCounter--;  //reduce jump counter
            }
        }

        groundSpeedColor = 1 / ((cg.lastGroundSpeed / state.cgaz.wishspeed) * (cg.lastGroundSpeed / state.cgaz.wishspeed));
        if (cg_jumpGoal.value && (cg_jumpGoal.value <= cg.lastGroundSpeed) && jumpsCounter == 1) {
            colorGroundSpeed[0] = groundSpeedColor;
            colorGroundSpeed[1] = 1;
            colorGroundSpeed[2] = groundSpeedColor;
        }
        else if (cg.lastGroundSpeed > state.cgaz.wishspeed && !(cg_speedometer.integer & SPEEDOMETER_COLORS)) {
            colorGroundSpeed[0] = 1;
            colorGroundSpeed[1] = groundSpeedColor;
            colorGroundSpeed[2] = groundSpeedColor;
        }

        if ((cg.lastGroundTime > cg.time - 1500) && (cg_speedometer.integer & SPEEDOMETER_GROUNDSPEED)) {
            if (cg.lastGroundSpeed) {
                Com_sprintf(speedStr4, sizeof(speedStr4), "%.0f", cg.lastGroundSpeed);
                CG_Text_Paint(speedometerXPos * cgs.widthRatioCoef, cg_speedometerY.value, cg_speedometerSize.value, colorGroundSpeed, speedStr4, 0.0f, 0, ITEM_ALIGN_LEFT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
            }
        }
        speedometerXPos += 52;
    }
}

/*  Movement Keys */
/*
===================
CG_GetGroundDistance
japro - Ground Distance function for use in jump detection for movement keys
===================
*/
 float DF_GetGroundDistance(void) {
    trace_t tr;
    vec3_t down;

    VectorCopy(cg.predictedPlayerState.origin, down);
    down[2] -= 4096;
    CG_Trace(&tr, cg.predictedPlayerState.origin, NULL, NULL, down, cg.predictedPlayerState.clientNum, MASK_SOLID);
    VectorSubtract(cg.predictedPlayerState.origin, tr.endpos, down);

    return VectorLength(down) - 24.0f;
}
/*
===================
CG_DrawMovementKeys
japro - Draw the movement keys
===================
*/
 void DF_DrawMovementKeys(centity_t* cent) {
    usercmd_t cmd = { 0 };
    playerState_t* ps = NULL;
    int moveDir;
    float w, h, x, y;

    if (!cg.snap)
        return;

    ps = &cg.predictedPlayerState;
    moveDir = ps->movementDir;

    //get the current key presses from the client
    if (cg.clientNum == cg.predictedPlayerState.clientNum && !cg.demoPlayback) { //real client
        trap->GetUserCmd(trap->GetCurrentCmdNumber(), &cmd);
    }
    else //spectating or demo
    {
        float xyspeed = sqrtf(ps->velocity[0] * ps->velocity[0] + ps->velocity[1] * ps->velocity[1]);
        float zspeed = ps->velocity[2];
        static float lastZSpeed = 0.0f;

        cmd = DF_DirToCmd(moveDir);
        if ((DF_GetGroundDistance() > 1 && zspeed > 8 && zspeed > lastZSpeed && !cg.snap->ps.fd.forceGripCripple) || (cg.snap->ps.pm_flags & PMF_JUMP_HELD))
            cmd.upmove = 1;
        else if ((ps->pm_flags & PMF_DUCKED) || CG_InRollAnim(cent))
            cmd.upmove = -1;
        if (xyspeed < 9)
            moveDir = -1;
        lastZSpeed = zspeed;

        if ((cent->currentState.eFlags & EF_FIRING) && !(cent->currentState.eFlags & EF_ALT_FIRING)) {
            cmd.buttons |= BUTTON_ATTACK;
            cmd.buttons &= ~BUTTON_ALT_ATTACK;
        }
        else if (cent->currentState.eFlags & EF_ALT_FIRING) {
            cmd.buttons |= BUTTON_ALT_ATTACK;
            cmd.buttons &= ~BUTTON_ATTACK;
        }
    }

    //set positions based on which setting is used
    if(cg_movementKeys.integer == 1) {
        w = 16 * cg_movementKeysSize.value * cgs.widthRatioCoef;
        h = 16 * cg_movementKeysSize.value;
        x = SCREEN_WIDTH * 0.5f + cg_movementKeysX.value - w * 1.5f;
        y = SCREEN_HEIGHT * 0.9f + cg_movementKeysY.value- h * 1.0f;
    } else if(cg_movementKeys.integer == 2) {
        w = 16 * cg_movementKeysSize.value * cgs.widthRatioCoef;
        h = 16 * cg_movementKeysSize.value;
        x = SCREEN_WIDTH * 0.5f + cg_movementKeysX.value - w * 2.0f;
        y = SCREEN_HEIGHT * 0.9f + cg_movementKeysY.value- h * 1.0f;
    } else if(cg_movementKeys.integer == 3) {
        w = 6 * cg_movementKeysSize.value * cgs.widthRatioCoef;
        h = 6 * cg_movementKeysSize.value;
        x = 0.5f * SCREEN_WIDTH - w * 1.5f;
        y = 0.5f * SCREEN_HEIGHT - h * 1.5f;
    } else if(cg_movementKeys.integer == 4) {
        w = 12 * cg_movementKeysSize.value * cgs.widthRatioCoef;
        h = 12 * cg_movementKeysSize.value;
        x = 0.5f * SCREEN_WIDTH + cg_movementKeysX.value - w * 1.5f;
        y = 0.9f * SCREEN_HEIGHT + cg_movementKeysY.value - h * 1.5f;
    } else {
		return;
	}

    //draw the keys
    if (cg_movementKeys.integer == 3 || cg_movementKeys.integer == 4) { //new movement keys style
        if (cmd.upmove < 0)
            CG_DrawPic(w * 2 + x, y, w, h, cgs.media.keyCrouchOnShader2);
        if (cmd.upmove > 0)
            CG_DrawPic(x, y, w, h, cgs.media.keyJumpOnShader2);
        if (cmd.forwardmove < 0)
            CG_DrawPic(w + x, h * 2 + y, w, h, cgs.media.keyBackOnShader2);
        if (cmd.forwardmove > 0)
            CG_DrawPic(w + x, y, w, h, cgs.media.keyForwardOnShader2);
        if (cmd.rightmove < 0)
            CG_DrawPic(x, h + y, w, h, cgs.media.keyLeftOnShader2);
        if (cmd.rightmove > 0)
            CG_DrawPic(w * 2 + x, h + y, w, h, cgs.media.keyRightOnShader2);
        if (cmd.buttons & BUTTON_ATTACK)
            CG_DrawPic(x, 2 * h + y, w, h, cgs.media.keyAttackOn2);
        if (cmd.buttons & BUTTON_ALT_ATTACK)
            CG_DrawPic(w * 2 + x, 2 * h + y, w, h, cgs.media.keyAltOn2);
    } else if (cg_movementKeys.integer == 1 || cg_movementKeys.integer == 2) { //original movement keys style
        if (cmd.upmove < 0)
            CG_DrawPic(w * 2 + x, y, w, h, cgs.media.keyCrouchOnShader);
        else
            CG_DrawPic(w * 2 + x, y, w, h, cgs.media.keyCrouchOffShader);
        if (cmd.upmove > 0)
            CG_DrawPic(x, y, w, h, cgs.media.keyJumpOnShader);
        else
            CG_DrawPic(x, y, w, h, cgs.media.keyJumpOffShader);
        if (cmd.forwardmove < 0)
            CG_DrawPic(w + x, h + y, w, h, cgs.media.keyBackOnShader);
        else
            CG_DrawPic(w + x, h + y, w, h, cgs.media.keyBackOffShader);
        if (cmd.forwardmove > 0)
            CG_DrawPic(w + x, y, w, h, cgs.media.keyForwardOnShader);
        else
            CG_DrawPic(w + x, y, w, h, cgs.media.keyForwardOffShader);
        if (cmd.rightmove < 0)
            CG_DrawPic(x, h + y, w, h, cgs.media.keyLeftOnShader);
        else
            CG_DrawPic(x, h + y, w, h, cgs.media.keyLeftOffShader);
        if (cmd.rightmove > 0)
            CG_DrawPic(w * 2 + x, h + y, w, h, cgs.media.keyRightOnShader);
        else
            CG_DrawPic(w * 2 + x, h + y, w, h, cgs.media.keyRightOffShader);
        if(cg_movementKeys.integer == 2) {
            if (cmd.buttons & BUTTON_ATTACK)
                CG_DrawPic(w * 3 + x, y, w, h, cgs.media.keyAttackOn);
            else
                CG_DrawPic(w * 3 + x, y, w, h, cgs.media.keyAttackOff);
            if (cmd.buttons & BUTTON_ALT_ATTACK)
                CG_DrawPic(w * 3 + x, h + y, w, h, cgs.media.keyAltOn);
            else
                CG_DrawPic(w * 3 + x, h + y, w, h, cgs.media.keyAltOff);
        }
    }
}

 void DF_RaceTimer(void)
{
    if (!cg.predictedPlayerState.stats[STAT_RACEMODE] || !cg.predictedPlayerState.duelTime) {
        cg.startSpeed = 0;
        cg.displacement = 0;
        cg.maxSpeed = 0;
        cg.displacementSamples = 0;
        return;
    }

    {
        char timerStr[48] = { 0 };
        char startStr[48] = { 0 };
        vec4_t colorStartSpeed = {1, 1, 1, 1};

        const int time = (cg.time - cg.predictedPlayerState.duelTime);
        const int minutes = (time / 1000) / 60;
        const int seconds = (time / 1000) % 60;
        const int milliseconds = (time % 1000);

        if (time < cg.lastRaceTime) {
            cg.startSpeed = 0;
            cg.displacement = 0;
            cg.maxSpeed = 0;
            cg.displacementSamples = 0;
        }

        if (cg_raceTimer.integer > 1 || cg_raceStart.integer) {
            if (time > 0) {
                if (!cg.startSpeed)
                    cg.startSpeed = (int)(state.cgaz.currentSpeed + 0.5f);
                if (state.cgaz.currentSpeed > (float)cg.maxSpeed)
                    cg.maxSpeed = (int)(state.cgaz.currentSpeed + 0.5f);
                cg.displacement += (int)state.cgaz.currentSpeed;
                cg.displacementSamples++;
            }
        }

        cg.lastRaceTime = time;
        if (cg_raceTimer.integer != 0) {
            if (cg_raceTimer.integer < 3)
                Com_sprintf(timerStr, sizeof(timerStr), "%i:%02i.%i\n", minutes, seconds, milliseconds / 100);
            else
                Com_sprintf(timerStr, sizeof(timerStr), "%i:%02i.%03i\n", minutes, seconds, milliseconds);

            if (cg_raceTimer.integer > 1) {
                if (cg.displacementSamples)
                    Q_strcat(timerStr, sizeof(timerStr), va("Max: %i\nAvg: %i", (int) ((float)cg.maxSpeed + 0.5f),
                                                            cg.displacement / cg.displacementSamples));
                if (time < 3000 && !cg_raceStart.integer)
                    Q_strcat(timerStr, sizeof(timerStr), va("\nStart: %i", cg.startSpeed));

            }

            CG_Text_Paint((float)cg_raceTimerX.integer * cgs.widthRatioCoef, (float)cg_raceTimerY.integer, cg_raceTimerSize.value, colorTable[CT_WHITE],
                          timerStr, 0.0f, 0, ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
        }
        if(cg_raceStart.integer)
        {
            if(cg_startGoal.value && (cg_startGoal.value <= (float)cg.startSpeed)){
                float startColor = 1 / (((float)cg.startSpeed/state.cgaz.wishspeed)*((float)cg.startSpeed/state.cgaz.wishspeed));
                colorStartSpeed[0] = startColor;
                colorStartSpeed[1] = 1;
                colorStartSpeed[2] = startColor;
            }
            Com_sprintf(startStr, sizeof(startStr), "Start: %i", cg.startSpeed);
            CG_Text_Paint((float)cg_raceStartX.integer * cgs.widthRatioCoef, (float)cg_raceStartY.integer, cg_raceTimerSize.value, colorStartSpeed, startStr, 0.0f, 0, ITEM_ALIGN_RIGHT | ITEM_TEXTSTYLE_OUTLINED, FONT_NONE);
        }
    }
}

void DF_DrawShowPos(void)
{
    static char showPosString[128];
    static char showPitchString[8];
    vec4_t colorPitch = {0, 0, 0, 1};
    playerState_t *ps = &cg.predictedPlayerState;
    float vel;
    float pitchAngle;
    float pitchColor;
    float pitchRange;
    pitchAngle = fabsf(ps->viewangles[PITCH] + cg_pitchHelperOffset.value);
    if(pitchAngle > 90){
        pitchAngle = 90;
    }
    pitchRange = cg_pitchHelperRange.value / 2;
    if(cg_pitchHelper.value && (pitchAngle < pitchRange)) {
        pitchColor = pitchAngle / pitchRange;
        colorPitch[0] = pitchColor;
        pitchColor = 1.0f - (pitchAngle / pitchRange) / 2.0f;
        colorPitch[1] = pitchColor;
    }else if(cg_pitchHelper.value){
        colorPitch[0] = 1;
        colorPitch[1] = 0;
    }

    if(cg_pitchHelper.value){
        Com_sprintf(showPitchString, sizeof(showPitchString), "%.1f", (float)ps->viewangles[PITCH]);
        CG_Text_Paint((float)cg_pitchHelperX.integer, (float)cg_pitchHelperY.integer, 1.0f, colorPitch, showPitchString, 0, 0, ITEM_TEXTSTYLE_OUTLINESHADOWED, FONT_SMALL2);
    }

    if (!cg_showpos.integer)
        return;

    if (!cg.snap)
        return;

    if (!ps)
        return;

    vel = sqrtf(state.cgaz.currentSpeed * state.cgaz.currentSpeed + ps->velocity[2] * ps->velocity[2]);

    Com_sprintf(showPosString, sizeof(showPosString), "pos:   %.2f   %.2f   %.2f\nang:   %.2f   %.2f\nvel:     %.2f",
                (float)ps->origin[0], (float)ps->origin[1], (float)ps->origin[2], (float)ps->viewangles[PITCH], (float)ps->viewangles[YAW], vel);

    CG_Text_Paint(SCREEN_WIDTH - (SCREEN_WIDTH - 340) * cgs.widthRatioCoef, 0, 0.6f, colorWhite,
                  showPosString, 0, 0, ITEM_TEXTSTYLE_OUTLINESHADOWED, FONT_SMALL2);
}