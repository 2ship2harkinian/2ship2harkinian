#include "Actions.h"

#include "2s2h/ShipUtils.h"

extern "C" {
#include "z64.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
#include "overlays/actors/ovl_En_Time_Tag/z_en_time_tag.h"
void EnTimeTag_KickOut_Transition(EnTimeTag* enTimeTag, PlayState* play);
}

extern void UpdateGameTime(u16 gameTime);

static GIActions::Register skipTimeAction({
    .id = GI_ACTION_SKIP_TIME,
    .name = "skipTime",
    .displayName = "Skip Time",
    .valence = GI_VALENCE_NEGATIVE,
    .schema =
        {
            // Game-clock units, not frames: DAY_LENGTH (0x10000) is a full day.
            { .name = "length", .type = GI_PARAM_INT, .defaultValue = 4000, .min = 1, .max = 65535 },
        },
    .canApply = GIActions::Gates::NotOnHorse,
    .onStart =
        [](GIAction& action) {
            u16 timeSkipInterval = static_cast<u16>(action.params.Int("length"));
            u16 previous_time = gSaveContext.save.time;
            u16 new_time = gSaveContext.save.time + timeSkipInterval;

            // Normalised to 6AM so the test holds for any length; a raw MORNING_TIME comparison
            // misses skips that wrap past 0xFFFF.
            bool crossedIntoNewDay = (u32)ZERO_DAY_START(previous_time) + timeSkipInterval >= DAY_LENGTH;

            if (crossedIntoNewDay) {
                // Handles case where Night -> Day
                if (gSaveContext.save.day != 3) {
                    gSaveContext.save.day++;
                    gSaveContext.save.eventDayCount++;
                    UpdateGameTime(new_time);
                    Interface_NewDay(gPlayState, CURRENT_DAY);
                    // Load environment values for new day
                    Environment_NewDay(&gPlayState->envCtx);
                    // Environment_NewDay only picks the skybox; without this Day 2's rain follows into Day 3.
                    gWeatherMode = WEATHER_MODE_CLEAR;
                    gPlayState->envCtx.lightningState = LIGHTNING_OFF;
                } else {
                    // Handles Moonfall case, prevents skipping past it by setting time right before
                    // Moonfall.
                    UpdateGameTime(MORNING_TIME - (timeSkipInterval / 10));
                }
            } else {
                // Every other case
                UpdateGameTime(new_time);
            }

            TransitionFade_SetColor(&gPlayState->unk_18E48, 0x000000);
            R_TRANS_FADE_FLASH_ALPHA_STEP = -1;
            Player_PlaySfx(GET_PLAYER(gPlayState), NA_SE_SY_TRANSFORM_MASK_FLASH);

            // Handle kickouts, if needed
            EnTimeTag* enTimeTag = (EnTimeTag*)Actor_FindNearby(gPlayState, &GET_PLAYER(gPlayState)->actor,
                                                                ACTOR_EN_TIME_TAG, ACTORCAT_ITEMACTION, 99999.9f);
            if (enTimeTag == nullptr) {
                return;
            }

            TimeTagType timeTagType = (TimeTagType)TIMETAG_GET_TYPE(&enTimeTag->actor);
            if (timeTagType != TIMETAG_KICKOUT_DOOR && timeTagType < TIMETAG_KICKOUT_FINAL_HOURS) {
                return;
            }

            s16 kickoutHour = TIMETAG_KICKOUT_HOUR(&enTimeTag->actor);
            s16 kickoutMinute = TIMETAG_KICKOUT_MINUTE(&enTimeTag->actor);
            s32 kickoutTime = CLOCK_TIME(kickoutHour, kickoutMinute);
            kickoutTime = ZERO_DAY_START(kickoutTime);
            previous_time = ZERO_DAY_START(previous_time);
            new_time = ZERO_DAY_START(new_time);

            // If we were here before the kickout time, and now it's after, then get out of my house
            if (previous_time <= kickoutTime && new_time >= kickoutTime) {
                // Unless this is the Stock Pot Inn and the room key is obtained. Asked through the
                // VB hook so rando can answer with its inf flag.
                bool hasRoomKey =
                    GameInteractor_Should(VB_CHECK_FOR_ROOM_KEY, INV_CONTENT(ITEM_ROOM_KEY) == ITEM_ROOM_KEY);
                if (!(gPlayState->sceneId == SCENE_YADOYA && hasRoomKey)) {
                    // This comes from EnTimeTag_KickOut_WaitForTime
                    Player_SetCsActionWithHaltedActors(gPlayState, &enTimeTag->actor, PLAYER_CSACTION_WAIT);
                    Message_StartTextbox(gPlayState, 0x1883 + TIMETAG_KICKOUT_GET_TEXT(&enTimeTag->actor), NULL);
                    enTimeTag->actionFunc = EnTimeTag_KickOut_Transition;
                }
            }
        },
});
