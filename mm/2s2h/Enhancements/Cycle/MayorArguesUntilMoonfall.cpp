#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "z64save.h"
#include "z64scene.h"
#include "variables.h"
}

#define CVAR_NAME "gEnhancements.Cycle.MayorArguesUntilMoonfall"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

// The arguing participants are absent from the Mayor's Residence actor list on the night of the Final Day. Their
// scene actor entries carry a half-day mask that excludes it, so they are neither spawned on room load nor allowed
// to survive the half-day rolling over while the player is standing in the room.
//
// Mayor Dotour is present either way, so his init is used to put the other four back before any of the one-time
// actor lookups run: EnDt_SetupCutsceneNpcs and EnBaisen's soldier lookup both scan the NPC list exactly once, on
// their first update, and never again.
typedef struct {
    s16 actorId;
    f32 posX;
    f32 posY;
    f32 posZ;
    s16 rotY;
    s16 params;
} MayorMeetingNpc;

static const MayorMeetingNpc sMayorMeetingNpcs[] = {
    { ACTOR_EN_MUTO, -245.0f, 0.0f, -314.0f, 0x4000, 1 },   // Mutoh
    { ACTOR_EN_BAISEN, -84.0f, 0.0f, -315.0f, -0x4000, 1 }, // Viscen
    { ACTOR_EN_HEISHI, -53.0f, 0.0f, -286.0f, -0x4000, 1 }, // Soldier
    { ACTOR_EN_DAIKU, -276.0f, 0.0f, -284.0f, 0x4000, 0 },  // Carpenter
};

static bool IsFinalNightMeetingActive(void) {
    return (gPlayState != NULL) && (gPlayState->sceneId == SCENE_SONCHONOIE) &&
           !CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING) && (gSaveContext.save.day == 3) &&
           gSaveContext.save.isNight;
}

// A culled actor lingers in the list for a frame with its update cleared before it is freed, so liveness is
// checked rather than mere presence.
static bool IsMayorMeetingNpcPresent(s16 actorId, s16 params) {
    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (actor != NULL) {
        if ((actor->id == actorId) && (actor->params == params) && (actor->update != NULL)) {
            return true;
        }
        actor = actor->next;
    }

    return false;
}

// Runs from Mayor Dotour's init, which is a frame ahead of his first update, so the spawned actors are already in
// the NPC list by the time he looks for them.
static void SpawnMayorMeetingNpcs(Actor* actor) {
    size_t i;

    if (!IsFinalNightMeetingActive()) {
        return;
    }

    for (i = 0; i < ARRAY_COUNT(sMayorMeetingNpcs); i++) {
        const MayorMeetingNpc* npc = &sMayorMeetingNpcs[i];

        if (!IsMayorMeetingNpcPresent(npc->actorId, npc->params)) {
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, npc->actorId, npc->posX, npc->posY, npc->posZ, 0, npc->rotY,
                        0, npc->params);
        }
    }
}

// Grant the Final Day night to the participants already in the room, so that a half-day rollover with the player
// standing there does not cull them out from under Dotour's stored pointers.
static void KeepMayorMeetingNpcOnFinalNight(Actor* actor) {
    if ((gPlayState != NULL) && (gPlayState->sceneId == SCENE_SONCHONOIE)) {
        actor->halfDaysBits |= HALFDAYBIT_DAY3_NIGHT;
    }
}

void RegisterMayorArguesUntilMoonfall() {
    COND_VB_SHOULD(VB_MAYOR_MEETING_END_ON_FINAL_NIGHT, CVAR, { *should = false; });

    COND_ID_HOOK(OnActorInit, ACTOR_EN_DT, CVAR, SpawnMayorMeetingNpcs);

    COND_ID_HOOK(OnActorInit, ACTOR_EN_MUTO, CVAR, KeepMayorMeetingNpcOnFinalNight);
    COND_ID_HOOK(OnActorInit, ACTOR_EN_BAISEN, CVAR, KeepMayorMeetingNpcOnFinalNight);
    COND_ID_HOOK(OnActorInit, ACTOR_EN_HEISHI, CVAR, KeepMayorMeetingNpcOnFinalNight);
    COND_ID_HOOK(OnActorInit, ACTOR_EN_DAIKU, CVAR, KeepMayorMeetingNpcOnFinalNight);
}

static RegisterShipInitFunc initFunc(RegisterMayorArguesUntilMoonfall, { CVAR_NAME });
