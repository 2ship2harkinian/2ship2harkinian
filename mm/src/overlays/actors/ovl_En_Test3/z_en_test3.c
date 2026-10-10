/*
 * File: z_en_test3.c
 * Overlay: ovl_En_Test3
 * Description: Kafei
 */

#include "prevent_bss_reordering.h"
#include "z_en_test3.h"

#include "z64malloc.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenPort.h"

#include "objects/object_test3/object_test3.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_mask_ki_tan/object_mask_ki_tan.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_CAN_PRESS_SWITCHES)

typedef struct {
    /* 0x0 */ s8 schActionIndex; // KafeiScheduledActionIndex
    /* 0x1 */ s8 pathIndexAndDirection : 4;
    /* 0x1 */ s8 speakDataIndex : 4;
} EnTest3ScheduleResultData; // size = 0x2

typedef s32 (*EnTest3ProcessScheduleFunc)(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                          ScheduleOutput* scheduleOutput);
typedef s32 (*EnTest3HandlerFunc)(EnTest3* this, PlayState* play);
typedef struct {
    /* 0x0 */ EnTest3ProcessScheduleFunc processSchFunc;
    /* 0x4 */ EnTest3HandlerFunc handleSchFunc;
} EnTest3ScheduledAction; // size = 0x8

typedef struct {
    /* 0x0 */ EnTest3ActionFunc effect;         // might not actually be an action function
    /* 0x4 */ EnTest3ActionFunc nextMainAction; // always NULL
} EnTest3PostTalkEffect;                        // size = 0x8

typedef enum EnTest3RunToTownPhase {
    /* 0 */ ENTEST3_RUN_TO_TOWN_WAITING,
    /* 1 */ ENTEST3_RUN_TO_TOWN_RUNNING,
    /* 2 */ ENTEST3_RUN_TO_TOWN_END,
    /* 3 */ ENTEST3_RUN_TO_TOWN_CLEARED,
} EnTest3RunToTownPhase;

typedef enum EnTest3CoupleCsPhase {
    /* 0 */ ENTEST3_COUPLE_CS_ENTERING,
    /* 1 */ ENTEST3_COUPLE_CS_STARTING,
    /* 2 */ ENTEST3_COUPLE_CS_PLAYING,
} EnTest3CoupleCsPhase;

void EnTest3_Init(Actor* thisx, PlayState* play2);
void EnTest3_Destroy(Actor* thisx, PlayState* play2);
void EnTest3_Update(Actor* thisx, PlayState* play2);
void EnTest3_Draw(Actor* thisx, PlayState* play2);
void EnTest3_Reset(void);

// Functions used to process and advance phases in Kafei's talk state machine
s32 EnTest3_SetupTalk_TriggerFaceRevealCs(EnTest3* this, PlayState* play);
s32 EnTest3_SetupTalk_DoNothing(EnTest3* this, PlayState* play);
s32 EnTest3_SetupTalk_NextMessage(EnTest3* this, PlayState* play);
s32 EnTest3_SetupTalk_SetNextMessageTimer(EnTest3* this, PlayState* play);
s32 EnTest3_HandleTalk_Wait(EnTest3* this, PlayState* play);
s32 EnTest3_StartCutscene(EnTest3* this, PlayState* play);
s32 EnTest3_SetupTalk_CutsceneCheck(EnTest3* this, PlayState* play);
s32 EnTest3_HandleTalk_Done(EnTest3* this, PlayState* play);
s32 EnTest3_HandleTalk_Event(EnTest3* this, PlayState* play);
s32 EnTest3_HandleTalk_LookAtPeephole(EnTest3* this, PlayState* play);
s32 EnTest3_HandleTalk_Closing(EnTest3* this, PlayState* play);
s32 EnTest3_HandleTalk_EndOfMessage(EnTest3* this, PlayState* play);
s32 EnTest3_HandleTalk_Choice(EnTest3* this, PlayState* play);

// Process and handle functions for all parts of Kafei's schedule
s32 EnTest3_ProcessSchedule_StopDrawing(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                        ScheduleOutput* scheduleOutput);
s32 EnTest3_HandleSchedule_StopDrawing(EnTest3* this, PlayState* play);
s32 EnTest3_ProcessSchedule_CanTalk(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                    ScheduleOutput* scheduleOutput);
s32 EnTest3_HandleSchedule_CanTalk(EnTest3* this, PlayState* play);
s32 EnTest3_ProcessSchedule_FindMailCarrier(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                            ScheduleOutput* scheduleOutput);
s32 EnTest3_HandleSchedule_TurnToMailCarrier(EnTest3* this, PlayState* play);
s32 EnTest3_ProcessSchedule_BellRung(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                     ScheduleOutput* scheduleOutput);
s32 EnTest3_HandleSchedule_BellRung(EnTest3* this, PlayState* play);
s32 EnTest3_ProcessSchedule_SakonHideout(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                         ScheduleOutput* scheduleOutput);
s32 EnTest3_HandleSchedule_SakonHideout(EnTest3* this, PlayState* play);
s32 EnTest3_ProcessSchedule_RunToTownCs(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                        ScheduleOutput* scheduleOutput);
s32 EnTest3_HandleSchedule_RunToTownCs(EnTest3* this, PlayState* play);
s32 EnTest3_ProcessSchedule_CouplesMaskCs(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                          ScheduleOutput* scheduleOutput);
s32 EnTest3_HandleSchedule_CouplesMaskCs(EnTest3* this, PlayState* play2);
s32 EnTest3_ProcessSchedule_ComputeWaypoint(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                            ScheduleOutput* scheduleOutput);
s32 EnTest3_HandleSchedule_MoveToWaypoint(EnTest3* this, PlayState* play);

// Action functions
void EnTest3_Action_FollowSchedule(EnTest3* this, PlayState* play);
void EnTest3_Action_Unused(EnTest3* this, PlayState* play);
void EnTest3_Action_HandleTalk(EnTest3* this, PlayState* play);
void EnTest3_Action_GivePendant(EnTest3* this, PlayState* play);
void EnTest3_PostTalk_EnablePeephole(EnTest3* this, PlayState* play);

typedef enum KafeiScheduleResult {
    /*  0 */ KAFEI_SCH_NONE,
    // Kafei either puts a letter in the postbox or receives one from the Postman
    /*  1 */ KAFEI_SCH_HANDLING_LETTER,
    // Unused
    /*  2 */ KAFEI_SCH_2,
    // Kafei is ready to speak with Link for the first time (and gives him the Pendant of Memories)
    /*  3 */ KAFEI_SCH_MEETING_LINK_IN_CURIOSITY_SHOP,
    // Kafei waits to spy on Sakon, but he didn't come because you stopped him stealing the Bomb Bag
    /*  4 */ KAFEI_SCH_WAITING_AT_CURIOSITY_SHOP,
    // Kafei waits outside Sakon's Hideout
    /*  5 */ KAFEI_SCH_WAITING_OUTSIDE_SAKONS_HIDEOUT,
    // Kafei laments that he couldn't keep his promise to Anju, though her bridal dress is lovely
    /*  6 */ KAFEI_SCH_COULDNT_KEEP_PROMISE,
    // Sakon's Hideout mission - Link helps Kafei retrieve the Sun's Mask
    /*  7 */ KAFEI_SCH_INSIDE_SAKONS_HIDEOUT,
    // Kafei reunites with Anju and they give Link the Couple's Mask
    /*  8 */ KAFEI_SCH_BECOMING_A_COUPLE,
    // Kafei runs back to Clock Town after retrieving the Sun's Mask
    /*  9 */ KAFEI_SCH_RUNNING_TO_TOWN,
    // Kafei heard Link ring the bell and steps outside briefly to look around
    /* 10 */ KAFEI_SCH_HEARD_BELL_RING,
    // Kafei enters Clock Town through the east gate on his way to reunite with Anju
    /* 11 */ KAFEI_SCH_ENTERING_CLOCK_TOWN_EAST,
    // Kafei has entered the Curiosity Shop with Anju's letter and is returning to his hiding spot
    /* 12 */ KAFEI_SCH_RETURNING_TO_CURIOSITY_SHOP_DAY_2,
    // Kafei goes to the postbox outside the Laundry Pool to deposit his letter
    /* 13 */ KAFEI_SCH_GOING_TO_POSTBOX,
    // Kafei returns to the Laundry Pool after dropping his letter in the postbox
    /* 14 */ KAFEI_SCH_RETURNING_TO_LAUNDRY_POOL,
    // Kafei runs to the Curiosity Shop backdoor after depositing his letter
    /* 15 */ KAFEI_SCH_RETURNING_TO_CURIOSITY_SHOP_DAY_1,
    // Kafei runs to the Postman to receive Anju's letter
    /* 16 */ KAFEI_SCH_RUNNING_TO_POSTMAN,
    // Kafei has the letter from Anju and is bringing it back to the Curiosity Shop
    /* 17 */ KAFEI_SCH_RUNNING_BACK_TO_DOOR_WITH_LETTER,
    // Kafei enters the Stock Pot Inn and heads upstairs to where Anju is waiting for him
    /* 18 */ KAFEI_SCH_ENTERING_STOCK_POT_INN,
    // Kafei slowly walks over to Anju's bridal dress
    /* 19 */ KAFEI_SCH_WALKING_TO_BRIDAL_DRESS,
    // Kafei sneaks inside Sakon's Hideout when the door opens
    /* 20 */ KAFEI_SCH_ENTERING_SAKONS_HIDEOUT,
    // Unused
    /* 21 */ KAFEI_SCH_21,
} KafeiScheduleResult;

static u8 sScheduleScript[] = {
    /* 0x000 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(1, 0x046 - 0x004),
    /* 0x004 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_ALLEY, 0x020 - 0x008),
    /* 0x008 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(6, 50, 7, 20, 0x01A - 0x00E),
    /* 0x00E */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(7, 30, 0x019 - 0x012),
    /* 0x012 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_HIT_LAUNDRY_POOL_BELL, 0x017 - 0x016),
    /* 0x016 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x017 */ SCHEDULE_CMD_RET_VAL_S(10),
    /* 0x019 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x01A */ SCHEDULE_CMD_RET_TIME(6, 50, 7, 20, 15),
    /* 0x020 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_CLOCKTOWER, 0x045 - 0x024),
    /* 0x024 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(6, 0, 6, 30, 0x03F - 0x02A),
    /* 0x02A */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(6, 30, 6, 35, 0x03D - 0x030),
    /* 0x030 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(6, 35, 6, 50, 0x037 - 0x036),
    /* 0x036 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x037 */ SCHEDULE_CMD_RET_TIME(6, 35, 6, 50, 14),
    /* 0x03D */ SCHEDULE_CMD_RET_VAL_S(1),
    /* 0x03F */ SCHEDULE_CMD_RET_TIME(6, 0, 6, 30, 13),
    /* 0x045 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x046 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(2, 0x09B - 0x04A),
    /* 0x04A */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_LETTER_TO_KAFEI_SCHEDULED_TODAY, 0x058 - 0x04E),
    /* 0x04E */ SCHEDULE_CMD_BRANCH_S(0x0),
    /* 0x050 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_HIT_LAUNDRY_POOL_BELL, 0x055 - 0x054),
    /* 0x054 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x055 */ SCHEDULE_CMD_RET_VAL_L(10),
    /* 0x058 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_ALLEY, 0x081 - 0x05C),
    /* 0x05C */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(13, 0, 0x050 - 0x060),
    /* 0x060 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(15, 5, 15, 25, 0x07B - 0x066),
    /* 0x066 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(15, 25, 15, 40, 0x079 - 0x06C),
    /* 0x06C */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(15, 40, 16, 0, 0x073 - 0x072),
    /* 0x072 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x073 */ SCHEDULE_CMD_RET_TIME(15, 40, 16, 0, 17),
    /* 0x079 */ SCHEDULE_CMD_RET_VAL_S(1),
    /* 0x07B */ SCHEDULE_CMD_RET_TIME(15, 5, 15, 25, 16),
    /* 0x081 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_AYASHIISHOP, 0x09A - 0x085),
    /* 0x085 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(15, 55, 16, 10, 0x094 - 0x08B),
    /* 0x08B */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(16, 10, 22, 10, 0x092 - 0x091),
    /* 0x091 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x092 */ SCHEDULE_CMD_RET_VAL_S(3),
    /* 0x094 */ SCHEDULE_CMD_RET_TIME(15, 55, 16, 10, 12),
    /* 0x09A */ SCHEDULE_CMD_RET_NONE(),
    /* 0x09B */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_L(3, 0x138 - 0x0A0),
    /* 0x0A0 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_L(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG, 0x12C - 0x0A5),
    /* 0x0A5 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_L(WEEKEVENTREG_SAKON_DEAD, 0x12C - 0x0AA),
    /* 0x0AA */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_SECOM, 0x0B0 - 0x0AE),
    /* 0x0AE */ SCHEDULE_CMD_RET_VAL_S(7),
    /* 0x0B0 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_ESCAPED_SAKONS_HIDEOUT, 0x0DD - 0x0B4),
    /* 0x0B4 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_IKANA, 0x0DC - 0x0B8),
    /* 0x0B8 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_KAFEI_ENTRUSTED_LINK, 0x0BE - 0x0BC),
    /* 0x0BC */ SCHEDULE_CMD_RET_VAL_S(5),
    /* 0x0BE */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_61_08, 0x0DA - 0x0C2),
    /* 0x0C2 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_51_10, 0x0CD - 0x0C6),
    /* 0x0C6 */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(19, 0, 0x0CB - 0x0CA),
    /* 0x0CA */ SCHEDULE_CMD_RET_NONE(),
    /* 0x0CB */ SCHEDULE_CMD_RET_VAL_S(5),
    /* 0x0CD */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(19, 0, 19, 10, 0x0D4 - 0x0D3),
    /* 0x0D3 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x0D4 */ SCHEDULE_CMD_RET_TIME(19, 0, 19, 10, 20),
    /* 0x0DA */ SCHEDULE_CMD_RET_VAL_S(5),
    /* 0x0DC */ SCHEDULE_CMD_RET_NONE(),
    /* 0x0DD */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_IKANA, 0x0E8 - 0x0E1),
    /* 0x0E1 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_90_02, 0x0E7 - 0x0E5),
    /* 0x0E5 */ SCHEDULE_CMD_RET_VAL_S(9),
    /* 0x0E7 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x0E8 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_TOWN, 0x0F9 - 0x0EC),
    /* 0x0EC */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(4, 0, 4, 10, 0x0F3 - 0x0F2),
    /* 0x0F2 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x0F3 */ SCHEDULE_CMD_RET_TIME(4, 0, 4, 10, 11),
    /* 0x0F9 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_YADOYA, 0x12B - 0x0FD),
    /* 0x0FD */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(4, 10, 4, 30, 0x125 - 0x103),
    /* 0x103 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_DELIVERED_PENDANT_OF_MEMORIES, 0x11C - 0x107),
    /* 0x107 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(4, 30, 4, 45, 0x116 - 0x10D),
    /* 0x10D */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(4, 45, 6, 0, 0x114 - 0x113),
    /* 0x113 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x114 */ SCHEDULE_CMD_RET_VAL_S(6),
    /* 0x116 */ SCHEDULE_CMD_RET_TIME(4, 30, 4, 45, 19),
    /* 0x11C */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(4, 30, 6, 0, 0x123 - 0x122),
    /* 0x122 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x123 */ SCHEDULE_CMD_RET_VAL_S(8),
    /* 0x125 */ SCHEDULE_CMD_RET_TIME(4, 10, 4, 30, 18),
    /* 0x12B */ SCHEDULE_CMD_RET_NONE(),
    /* 0x12C */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_AYASHIISHOP, 0x137 - 0x130),
    /* 0x130 */ SCHEDULE_CMD_CHECK_WEEK_EVENT_REG_S(WEEKEVENTREG_KAFEI_ENTRUSTED_LINK, 0x135 - 0x134),
    /* 0x134 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x135 */ SCHEDULE_CMD_RET_VAL_S(4),
    /* 0x137 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x138 */ SCHEDULE_CMD_RET_NONE(),
};

ActorProfile En_Test3_Profile = {
    /**/ ACTOR_EN_TEST3,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TEST3,
    /**/ sizeof(EnTest3),
    /**/ EnTest3_Init,
    /**/ EnTest3_Destroy,
    /**/ EnTest3_Update,
    /**/ NULL,
    /**/ EnTest3_Reset,
};

static EnTest3PostTalkEffect sPostTalkEffects[] = {
    { EnTest3_PostTalk_EnablePeephole, NULL },
    { NULL, NULL },
};

typedef enum EnTest3TalkState {
    /* 0 */ ENTEST3_TALK_DONE,
    /* 1 */ ENTEST3_TALK_EVENT,
    /* 2 */ ENTEST3_TALK_CLOSING,
    /* 3 */ ENTEST3_TALK_CHOICE,
    /* 4 */ ENTEST3_TALK_END,
    /* 5 */ ENTEST3_TALK_START_CUTSCENE,
    /* 6 */ ENTEST3_TALK_LOOK_AT_PEEPHOLE,
    /* 7 */ ENTEST3_TALK_WAIT,
    /* 8 */ ENTEST3_TALK_FACE_REVEAL,
} EnTest3TalkState;

static EnTest3HandlerFunc sTalkStateSetupFuncs[] = {
    EnTest3_SetupTalk_NextMessage,         // ENTEST3_TALK_DONE
    EnTest3_SetupTalk_NextMessage,         // ENTEST3_TALK_EVENT
    EnTest3_SetupTalk_DoNothing,           // ENTEST3_TALK_CLOSING
    EnTest3_SetupTalk_NextMessage,         // ENTEST3_TALK_CHOICE
    EnTest3_SetupTalk_NextMessage,         // ENTEST3_TALK_END
    EnTest3_SetupTalk_CutsceneCheck,       // ENTEST3_TALK_START_CUTSCENE
    EnTest3_SetupTalk_NextMessage,         // ENTEST3_TALK_LOOK_AT_PEEPHOLE
    EnTest3_SetupTalk_SetNextMessageTimer, // ENTEST3_TALK_WAIT
    EnTest3_SetupTalk_TriggerFaceRevealCs, // ENTEST3_TALK_FACE_REVEAL
};

static EnTest3HandlerFunc sTalkStateHandlerFuncs[] = {
    EnTest3_HandleTalk_Done,           // ENTEST3_TALK_DONE
    EnTest3_HandleTalk_Event,          // ENTEST3_TALK_EVENT
    EnTest3_HandleTalk_Closing,        // ENTEST3_TALK_CLOSING
    EnTest3_HandleTalk_Choice,         // ENTEST3_TALK_CHOICE
    EnTest3_HandleTalk_EndOfMessage,   // ENTEST3_TALK_END
    EnTest3_StartCutscene,             // ENTEST3_TALK_START_CUTSCENE
    EnTest3_HandleTalk_LookAtPeephole, // ENTEST3_TALK_LOOK_AT_PEEPHOLE
    EnTest3_HandleTalk_Wait,           // ENTEST3_TALK_WAIT
    // ENTEST3_TALK_FACE_REVEAL has no entry here, so possibly an index out of bounds; thankfully, we've left the talk
    // state machine at this point because the GivePendant action has been set up
};

static PlayerAgeProperties sAgeProperties = {
    // ceilingCheckHeight
    40.0f,
    // shadowScale
    60.0f,
    // unk_08
    11.0f / 17.0f,
    // unk_0C
    71.0f,
    // unk_10
    50.0f,
    // unk_14
    49.0f,
    // unk_18
    39.0f,
    // unk_1C
    27.0f,
    // unk_20
    19.0f,
    // unk_24
    22.0f,
    // unk_28
    32.4f,
    // unk_2C
    32.0f,
    // unk_30
    48.0f,
    // unk_34
    11.0f / 17.0f * 70.0f,
    // wallCheckRadius
    14.0f,
    // unk_3C
    12.0f,
    // unk_40
    55.0f,
    // unk_44
    { 0xFFE8, 0x0DED, 0x036C },
    { { 0xFFE8, 0x0D92, 0x035E }, { 0xFFE8, 0x1371, 0x03A9 }, { 0x0008, 0x1256, 0x017C }, { 0x0009, 0x17EA, 0x0167 } },
    { { 0xFFE8, 0x1371, 0x03A9 }, { 0xFFE8, 0x195F, 0x03A9 }, { 0x0009, 0x17EA, 0x0167 }, { 0x0009, 0x1E0D, 0x017C } },
    { { 0x0008, 0x1256, 0x017C }, { 0x0009, 0x17EA, 0x0167 }, { 0xF9C8, 0x1256, 0x017C }, { 0xF9C9, 0x17EA, 0x0167 } },
    // voiceSfxIdOffset
    SFX_VOICE_BANK_SIZE * 1,
    // surfaceSfxIdOffset
    0,
    // unk_98
    22.0f,
    // unk_9C
    29.4343f,
    // openChestAnim
    &gPlayerAnim_clink_demo_Tbox_open,
    // timeTravelStartAnim
    &gPlayerAnim_clink_demo_goto_future,
    // timeTravelEndAnim
    &gPlayerAnim_clink_demo_return_to_future,
    // unk_AC
    &gPlayerAnim_clink_normal_climb_startA,
    // unk_B0
    &gPlayerAnim_clink_normal_climb_startB,
    // unk_B4
    {
        &gPlayerAnim_clink_normal_climb_upL,
        &gPlayerAnim_clink_normal_climb_upR,
        &gPlayerAnim_link_normal_Fclimb_upL,
        &gPlayerAnim_link_normal_Fclimb_upR,
    },
    // unk_C4
    {
        &gPlayerAnim_link_normal_Fclimb_sideL,
        &gPlayerAnim_link_normal_Fclimb_sideR,
    },
    // unk_CC
    {
        &gPlayerAnim_clink_normal_climb_endAL,
        &gPlayerAnim_clink_normal_climb_endAR,
    },
    // unk_D4
    {
        &gPlayerAnim_clink_normal_climb_endBR,
        &gPlayerAnim_clink_normal_climb_endBL,
    },
};

static EffectBlureInit2 sBlureInit = {
    0,
    EFFECT_BLURE_ELEMENT_FLAG_8,
    0,
    { 255, 255, 255, 255 },
    { 255, 255, 255, 64 },
    { 255, 255, 255, 0 },
    { 255, 255, 255, 0 },
    4,
    0,
    EFF_BLURE_DRAW_MODE_SMOOTH,
    0,
    { 0, 0, 0, 0 },
    { 0, 0, 0, 0 },
};

static EffectTireMarkInit sTireMarkInit = {
    0,
    63,
    { 0, 0, 15, 100 },
};

typedef enum KafeiScheduledActionIndex {
    /* 0 */ KAFEI_SCH_ACTION_STOP_DRAWING,
    /* 1 */ KAFEI_SCH_ACTION_COMPUTE_WAYPOINT,
    /* 2 */ KAFEI_SCH_ACTION_CAN_TALK,
    /* 3 */ KAFEI_SCH_ACTION_FIND_MAIL_CARRIER,
    /* 4 */ KAFEI_SCH_ACTION_BELL_RUNG,
    /* 5 */ KAFEI_SCH_ACTION_SAKON_HIDEOUT,
    /* 6 */ KAFEI_SCH_ACTION_RUN_TO_TOWN_CS,
    /* 7 */ KAFEI_SCH_ACTION_COUPLE_CS,
} KafeiScheduledActionIndex;

static EnTest3ScheduledAction sScheduledActions[] = {
    // KAFEI_SCH_ACTION_STOP_DRAWING
    { EnTest3_ProcessSchedule_StopDrawing, EnTest3_HandleSchedule_StopDrawing },
    // KAFEI_SCH_ACTION_COMPUTE_WAYPOINT
    { EnTest3_ProcessSchedule_ComputeWaypoint, EnTest3_HandleSchedule_MoveToWaypoint },
    // KAFEI_SCH_ACTION_CAN_TALK
    { EnTest3_ProcessSchedule_CanTalk, EnTest3_HandleSchedule_CanTalk },
    // KAFEI_SCH_ACTION_FIND_MAIL_CARRIER
    { EnTest3_ProcessSchedule_FindMailCarrier, EnTest3_HandleSchedule_TurnToMailCarrier },
    // KAFEI_SCH_ACTION_BELL_RUNG
    { EnTest3_ProcessSchedule_BellRung, EnTest3_HandleSchedule_BellRung },
    // KAFEI_SCH_ACTION_SAKON_HIDEOUT
    { EnTest3_ProcessSchedule_SakonHideout, EnTest3_HandleSchedule_SakonHideout },
    // KAFEI_SCH_ACTION_RUN_TO_TOWN_CS
    { EnTest3_ProcessSchedule_RunToTownCs, EnTest3_HandleSchedule_RunToTownCs },
    // KAFEI_SCH_ACTION_COUPLE_CS
    { EnTest3_ProcessSchedule_CouplesMaskCs, EnTest3_HandleSchedule_CouplesMaskCs },
};

static EnTest3ScheduleResultData sKafeiScheduledResultData[] = {
    { KAFEI_SCH_ACTION_STOP_DRAWING, 0, 0 },       // KAFEI_SCH_NONE
    { KAFEI_SCH_ACTION_FIND_MAIL_CARRIER, -2, 0 }, // KAFEI_SCH_HANDLING_LETTER
    { KAFEI_SCH_ACTION_CAN_TALK, -1, 0 },          // KAFEI_SCH_2
    { KAFEI_SCH_ACTION_CAN_TALK, -1, 1 },          // KAFEI_SCH_MEETING_LINK_IN_CURIOSITY_SHOP
    { KAFEI_SCH_ACTION_CAN_TALK, -1, 2 },          // KAFEI_SCH_WAITING_AT_CURIOSITY_SHOP
    { KAFEI_SCH_ACTION_CAN_TALK, 1, 3 },           // KAFEI_SCH_WAITING_OUTSIDE_SAKONS_HIDEOUT
    { KAFEI_SCH_ACTION_CAN_TALK, -2, 4 },          // KAFEI_SCH_COULDNT_KEEP_PROMISE
    { KAFEI_SCH_ACTION_SAKON_HIDEOUT, 1, 0 },      // KAFEI_SCH_INSIDE_SAKONS_HIDEOUT
    { KAFEI_SCH_ACTION_COUPLE_CS, -1, 0 },         // KAFEI_SCH_BECOMING_A_COUPLE
    { KAFEI_SCH_ACTION_RUN_TO_TOWN_CS, 2, 0 },     // KAFEI_SCH_RUNNING_TO_TOWN
    { KAFEI_SCH_ACTION_BELL_RUNG, 4, 0 },          // KAFEI_SCH_HEARD_BELL_RING
    { KAFEI_SCH_ACTION_COMPUTE_WAYPOINT, 1, 0 },   // KAFEI_SCH_ENTERING_CLOCK_TOWN_EAST
    { KAFEI_SCH_ACTION_COMPUTE_WAYPOINT, 1, 0 },   // KAFEI_SCH_RETURNING_TO_CURIOSITY_SHOP_DAY_2
    { KAFEI_SCH_ACTION_COMPUTE_WAYPOINT, 2, 0 },   // KAFEI_SCH_GOING_TO_POSTBOX
    { KAFEI_SCH_ACTION_COMPUTE_WAYPOINT, 1, 0 },   // KAFEI_SCH_RETURNING_TO_LAUNDRY_POOL
    { KAFEI_SCH_ACTION_COMPUTE_WAYPOINT, 1, 0 },   // KAFEI_SCH_RETURNING_TO_CURIOSITY_SHOP_DAY_1
    { KAFEI_SCH_ACTION_COMPUTE_WAYPOINT, 2, 0 },   // KAFEI_SCH_RUNNING_TO_POSTMAN
    { KAFEI_SCH_ACTION_COMPUTE_WAYPOINT, 3, 0 },   // KAFEI_SCH_RUNNING_BACK_TO_DOOR_WITH_LETTER
    { KAFEI_SCH_ACTION_COMPUTE_WAYPOINT, 1, 0 },   // KAFEI_SCH_ENTERING_STOCK_POT_INN
    { KAFEI_SCH_ACTION_COMPUTE_WAYPOINT, 2, 0 },   // KAFEI_SCH_WALKING_TO_BRIDAL_DRESS
    { KAFEI_SCH_ACTION_COMPUTE_WAYPOINT, 1, 0 },   // KAFEI_SCH_ENTERING_SAKONS_HIDEOUT
    { KAFEI_SCH_ACTION_STOP_DRAWING, 0, 0 },       // KAFEI_SCH_21
};

static EnTest3SpeakData sSpeakData[] = {
    { 4, 0, 0x2B25 },  // Unused? NTSC message: "Oh, that's too bad."
    { 1, 0, 0x2969 },  // First thing Kafei says when he meets Link
    { 3, 1, 0x296A },  // Asking Link to keep a secret
    { 1, 0, 0x296B },  // If Link said no
    { 5, 1, 0x0000 },  // Starts the face reveal cutscene, regardless of what Link chose
    { 8, 0, 0x0000 },  // Sets Kafei's next action as GivePendant
    { 4, 0, 0x2976 },  // Last message after giving Link the pendant
    { 6, 0, 0x2977 },  // Talking to Kafei after he gave Link the pendant
    { 7, 10, 0x2978 }, // 10-frame delay as Kafei turns to look at the peephole on the wall
    { 4, 1, 0x0000 },  // Displays the previous message once 10 frames have passed
    { 4, 0, 0x2968 },  // Kafei doesn't recognize Link in Curiosity Shop; long silence
    { 4, 0, 0x297A },  // Kafei says Sakon will definitely appear
    { 1, 0, 0x145D },  // Talking to Kafei outside Sakon's Hideout; first message
    { 1, 0, 0x145E },  // Talking to Kafei outside Sakon's Hideout; second message
    { 5, 1, 0x145F },  // Starts cutscene where Kafei looks at the hideout door. The text ID is necessary to tell Kafei
                       // to look at it.
    { 1, 0, 0x145F },  // Third message, spoken while looking at the hideout door
    { 5, 0, 0x0000 },  // Ends cutscene where Kafei looks at the hideout door
    { 4, 0, 0x1460 },  // Last message in this conversation; in subsequent talks, he only says this
    { 4, 0, 0x145C },  // Kafei doesn't recognize Link at Ikana Canyon; medium silence
    { 4, 0, 0x2913 },  // Talking to Kafei at inn with Anju absent (Link didn't give her the pendant)
    { 4, 0, 0x1465 },  // Kafei tells Link to step on the switch
};

s32 sSakonHideoutPhase;
s32 sExists;
Input sKafeiControlInput;
f32 sKafeiControlStickMagnitude;
s16 sKafeiControlStickAngle;
s32 sKafeiIsFollowingPath;
Vec3f sKafeiCurrentPathTargetPos;
s32 sMetAtIkana;
s32 sKafeiGavePendantToLink;
EnTest3RunToTownPhase sKafeiRunToTownPhase;
EnTest3CoupleCsPhase sKafeiCoupleCsPhase;
Vec3f* sKafeiCurBodyPartPos;

s32 EnTest3_SetMainAction(EnTest3* this, EnTest3ActionFunc actionFunc) {
    if (actionFunc == this->mainActionFunc) {
        return false;
    } else {
        this->mainActionFunc = actionFunc;
        this->bellTimer = 0;
        this->scheduleResult = KAFEI_SCH_NONE;
        return true;
    }
}

s32 EnTest3_TalkTriggersAction(EnTest3* this, PlayState* play, s32 index) {
    s32 pad;

    sPostTalkEffects[index].effect(this, play);

    if (sPostTalkEffects[index].nextMainAction == NULL) {
        // sPostTalkEffects[index].nextMainAction is always NULL
        return false;
    } else {
        EnTest3_SetMainAction(this, sPostTalkEffects[index].nextMainAction);
        return true;
    }
}

s32 EnTest3_SetupTalk_TriggerFaceRevealCs(EnTest3* this, PlayState* play) {
    return true;
}

s32 EnTest3_SetupTalk_DoNothing(EnTest3* this, PlayState* play) {
    return false;
}

s32 EnTest3_SetupTalk_NextMessage(EnTest3* this, PlayState* play) {
    u16 textId = this->speakData->textId;

    if ((this->speakData->talkActionIndex == ENTEST3_TALK_END) &&
        CHECK_WEEKEVENTREG(WEEKEVENTREG_KAFEI_ENTRUSTED_LINK)) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_KAFEI);
    }
    if (textId == 0xFFFF) {
        // Never happens
        Message_CloseTextbox(play);
    } else if ((u32)textId != 0) {
        Message_ContinueTextbox(play, textId);
    }
    if (textId == 0x296B) {
        PlayerAnimation_PlayOnceSetSpeed(play, &this->player.skelAnime, &gPlayerAnim_al_yareyare,
                                         PLAYER_ANIM_ADJUSTED_SPEED);
    }
    return false;
}

s32 EnTest3_SetupTalk_SetNextMessageTimer(EnTest3* this, PlayState* play) {
    this->nextMsgTimer = this->speakData->argument;
    return false;
}

s32 EnTest3_HandleTalk_Wait(EnTest3* this, PlayState* play) {
    if (DECR(this->nextMsgTimer) == 0) {
        Message_StartTextbox(play, this->speakData->textId, NULL);
        return true;
    }
    return false;
}

s32 EnTest3_StartCutscene(EnTest3* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_StartWithPlayerCs(this->csId, &this->player.actor);
        return true;
    } else {
        CutsceneManager_Queue(this->csId);
        return false;
    }
}

s32 EnTest3_SetupTalk_CutsceneCheck(EnTest3* this, PlayState* play) {
    if (this->speakData->textId == 0x145F) {
        Actor* hideoutDoor = SubS_FindActor(play, NULL, ACTORCAT_BG, ACTOR_BG_IKNV_OBJ);

        if (hideoutDoor != NULL) {
            this->player.focusActor = hideoutDoor;
        }
    }
    if (this->speakData->argument != 0) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(this->csId);
        play->msgCtx.msgMode = MSGMODE_PAUSED;
    }
    return false;
}

s32 EnTest3_HandleTalk_Done(EnTest3* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) {
        return 1;
    }
    return 0;
}

s32 EnTest3_HandleTalk_Event(EnTest3* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->speakData->textId == 0x145F) {
            CutsceneManager_Stop(this->csId);
            this->csId = CS_ID_GLOBAL_TALK;
            CutsceneManager_Queue(this->csId);
            this->player.focusActor = &GET_PLAYER(play)->actor;
        }
        return 1;
    }
    return 0;
}

s32 EnTest3_HandleTalk_LookAtPeephole(EnTest3* this, PlayState* play) {
    if (EnTest3_HandleTalk_Event(this, play)) {
        Actor* hideoutObject = SubS_FindActor(play, NULL, ACTORCAT_ITEMACTION, ACTOR_OBJ_NOZOKI);

        if (hideoutObject != NULL) {
            this->player.focusActor = hideoutObject;
        }
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        return 1;
    }
    return 0;
}

s32 EnTest3_HandleTalk_Closing(EnTest3* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        return 1;
    }
    return 0;
}

s32 EnTest3_HandleTalk_EndOfMessage(EnTest3* this, PlayState* play) {
    return 0;
}

s32 EnTest3_HandleTalk_Choice(EnTest3* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        if (play->msgCtx.choiceIndex != 0) {
            Audio_PlaySfx_MessageCancel();
        } else {
            Audio_PlaySfx_MessageDecide();
        }
        if (play->msgCtx.choiceIndex != 0) {
            return 1;
        } else {
            s32 nextOffset = this->speakData->argument + 1;

            return nextOffset;
        }
    }
    return 0;
}

s32 EnTest3_ShouldTriggerCutscene(EnTest3* this, PlayState* play) {
    return sTalkStateSetupFuncs[this->speakData->talkActionIndex](this, play);
}

s32 EnTest3_NextTalkStateTriggersCutscene(EnTest3* this, PlayState* play) {
    s32 nextOffset = sTalkStateHandlerFuncs[this->speakData->talkActionIndex](this, play);

    if (nextOffset != 0) {
        this->speakData = &this->speakData[nextOffset];
        return EnTest3_ShouldTriggerCutscene(this, play);
    }
    return false;
}

void EnTest3_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTest3* this = (EnTest3*)thisx;
    Camera* subCam;

    if (sExists) {
        Actor_Kill(&this->player.actor);
        return;
    }
    sExists = true;

    this->player.actor.room = -1;
    this->player.csId = CS_ID_NONE;
    this->player.transformation = PLAYER_FORM_HUMAN;
    this->player.ageProperties = &sAgeProperties;
    this->player.heldItemAction = PLAYER_IA_NONE;
    this->player.heldItemId = ITEM_OCARINA_OF_TIME;

    Player_SetModelGroup(&this->player, PLAYER_MODELGROUP_DEFAULT);
    play->playerInit(&this->player, play, &gKafeiSkel);

    Effect_Add(play, &this->player.meleeWeaponEffectIndex[0], EFFECT_BLURE2, 0, 0, &sBlureInit);
    Effect_Add(play, &this->player.meleeWeaponEffectIndex[1], EFFECT_BLURE2, 0, 0, &sBlureInit);
    Effect_Add(play, &this->player.meleeWeaponEffectIndex[2], EFFECT_TIRE_MARK, 0, 0, &sTireMarkInit);

    this->player.maskObjectSegment = ZeldaArena_Malloc(0x3800);
    play->func_18780(&this->player, play);
    this->link = GET_PLAYER(play);
    this->player.giObjectSegment = this->link->giObjectSegment;
    this->player.tatlActor = this->link->tatlActor;

    if ((CURRENT_DAY != 3) || CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG) ||
        !CHECK_WEEKEVENTREG(WEEKEVENTREG_KAFEI_ENTRUSTED_LINK)) {
        this->player.currentMask = PLAYER_MASK_KEATON;
    }
    this->player.prevMask = this->player.currentMask;

    if (play->sceneId == SCENE_SECOM) {
        this->subCamId = Play_CreateSubCamera(play);
        subCam = Play_GetCamera(play, this->subCamId);
        Camera_InitFocalActorSettings(subCam, &this->player.actor);
        Camera_SetStateFlag(subCam, CAM_STATE_0 | CAM_STATE_6);
        Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_WAIT);
    }

    this->player.actor.colChkInfo.cylRadius = 20;
    this->player.actor.colChkInfo.cylHeight = 60;
    this->player.actor.colChkInfo.health = 255;

    if (KAFEI_GET_PARAM_1E0(&this->player.actor) == 0) {
        EnTest3_SetMainAction(this, EnTest3_Action_Unused);
    } else {
        EnTest3_SetMainAction(this, EnTest3_Action_FollowSchedule);
    }
}

void EnTest3_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTest3* this = (EnTest3*)thisx;

    ResourceMgr_UnregisterSkeleton(&this->player.skelAnime);
    ResourceMgr_UnregisterSkeleton(&this->player.skelAnimeUpper);
    Effect_Destroy(play, this->player.meleeWeaponEffectIndex[0]);
    Effect_Destroy(play, this->player.meleeWeaponEffectIndex[1]);
    Effect_Destroy(play, this->player.meleeWeaponEffectIndex[2]);
    Collider_DestroyCylinder(play, &this->player.cylinder);
    Collider_DestroyCylinder(play, &this->player.shieldCylinder);
    Collider_DestroyQuad(play, &this->player.meleeWeaponQuads[0]);
    Collider_DestroyQuad(play, &this->player.meleeWeaponQuads[1]);
    Collider_DestroyQuad(play, &this->player.shieldQuad);
    ZeldaArena_Free(this->player.maskObjectSegment);
    Environment_StartTime();
}

s32 EnTest3_ProcessSchedule_StopDrawing(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                        ScheduleOutput* scheduleOutput) {
    return true;
}

s32 EnTest3_HandleSchedule_StopDrawing(EnTest3* this, PlayState* play) {
    this->player.actor.draw = NULL;
    return true;
}

void EnTest3_ReturnControlToLink(EnTest3* this, PlayState* play) {
    Actor_ChangeCategory(play, &play->actorCtx, &this->link->actor, ACTORCAT_PLAYER);
    Actor_ChangeCategory(play, &play->actorCtx, &this->player.actor, ACTORCAT_NPC);
    this->link->stateFlags1 &= ~PLAYER_STATE1_20;
}

void EnTest3_EndCsAction(EnTest3* this, PlayState* play) {
    if (this->player.csAction != PLAYER_CSACTION_NONE) {
        play->tryPlayerCsAction(play, &this->player, PLAYER_CSACTION_END);
    }
}

s32 EnTest3_MoveTowardNextPoint(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData) {
    s32 limit;
    Path* path;
    Vec3s* curPathPoint;
    Vec3s* nextPathPoint;
    Vec3f curPathPos;
    Vec3f nextPathPos;

    limit = ABS_ALT(schResultData->pathIndexAndDirection) - 1;

    if (limit >= 0) {
        path = SubS_GetAdditionalPath(play, KAFEI_GET_PATH_INDEX(&this->player.actor), limit);

        curPathPoint = Lib_SegmentedToVirtual(path->points);
        if (schResultData->pathIndexAndDirection > 0) {
            nextPathPoint = curPathPoint + 1;
        } else {
            curPathPoint += path->count - 1;
            nextPathPoint = curPathPoint - 1;
        }
        Math_Vec3s_ToVec3f(&curPathPos, curPathPoint);
        Math_Vec3s_ToVec3f(&nextPathPos, nextPathPoint);
        if (Math_Vec3f_DistXZ(&this->player.actor.world.pos, &curPathPos) > 10.0f) {
            Math_Vec3f_Copy(&this->player.actor.world.pos, &curPathPos);
            Math_Vec3f_Copy(&this->player.actor.home.pos, &curPathPos);
            Math_Vec3f_Copy(&this->player.actor.prevPos, &curPathPos);
            this->player.yaw = Math_Vec3f_Yaw(&this->player.actor.world.pos, &nextPathPos);
            if (schResultData->pathIndexAndDirection < 0) {
                this->player.yaw += 0x8000;
            }
            this->player.actor.shape.rot.y = this->player.yaw;
            return true;
        }
    }
    return false;
}

Actor* EnTest3_FindNearbyActor(PlayState* play, EnTest3* this, s32 actorId, s32 category, f32 radiusXZ, f32 distY) {
    Actor* actor = play->actorCtx.actorLists[category].first;

    while (actor != NULL) {
        if (actorId == actor->id) {
            f32 dy = this->player.actor.world.pos.y - actor->world.pos.y;

            if ((fabsf(dy) < distY) && (Actor_WorldDistXZToActor(&this->player.actor, actor) < radiusXZ)) {
                return actor;
            }
        }
        actor = actor->next;
    }
    return NULL;
}

s32 EnTest3_IsOpeningDoor(EnTest3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnDoor* door = (EnDoor*)EnTest3_FindNearbyActor(play, this, ACTOR_EN_DOOR, ACTORCAT_DOOR, 55.0f, 20.0f);
    Vec3f offset;

    if ((door != NULL) && !door->knobDoor.requestOpen &&
        ((player->doorType == PLAYER_DOORTYPE_NONE) || (&door->knobDoor.dyna.actor != player->doorActor)) &&
        Actor_ActorAIsFacingActorB(&this->player.actor, &door->knobDoor.dyna.actor, 0x3000)) {
        Actor_WorldToActorCoords(&door->knobDoor.dyna.actor, &offset, &this->player.actor.world.pos);
        this->player.doorType = PLAYER_DOORTYPE_HANDLE;
        this->player.doorDirection = (offset.z >= 0.0f) ? 1.0f : -1.0f;
        this->player.doorActor = &door->knobDoor.dyna.actor;
        this->player.csId = CS_ID_NONE;
        return true;
    }
    return false;
}

// Checks if Kafei can recognize Link. He won't unless Link is wearing one of:
// - No mask
// - Bunny Hood
// - Postman's Hat
// - Keaton Mask
// - Kafei's Mask
bool EnTest3_RecognizesLink(PlayState* play) {
    return (Player_GetMask(play) == PLAYER_MASK_NONE) || (Player_GetMask(play) == PLAYER_MASK_BUNNY) ||
           (Player_GetMask(play) == PLAYER_MASK_POSTMAN) || (Player_GetMask(play) == PLAYER_MASK_KEATON) ||
           (Player_GetMask(play) == PLAYER_MASK_KAFEIS_MASK);
}

void EnTest3_GetSpeakDataAtShop(EnTest3* this, PlayState* play) {
    if (!EnTest3_RecognizesLink(play)) {
        this->speakData = &sSpeakData[10];
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_KAFEI_ENTRUSTED_LINK)) {
        this->speakData = &sSpeakData[7];
    } else {
        this->speakData = &sSpeakData[1];
    }
    this->csId = this->player.actor.csId;
}

void EnTest3_GetSpeakDataAtIkana(EnTest3* this, PlayState* play) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_KAFEI_ENTRUSTED_LINK) || !EnTest3_RecognizesLink(play)) {
        this->speakData = &sSpeakData[18];
        sMetAtIkana = false;
    } else if (sMetAtIkana) {
        this->speakData = &sSpeakData[17];
    } else {
        this->speakData = &sSpeakData[12];
    }
    this->csId = this->player.actor.csId;
}

s32 EnTest3_ProcessSchedule_CanTalk(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                    ScheduleOutput* scheduleOutput) {
    // This speak data array is separate from the normal one, containing only a few of its entries. The only ones
    // that actually matter are 11 and 19; the rest get overridden by the above functions.
    static EnTest3SpeakData* sNpcSpeakData[] = {
        &sSpeakData[0], &sSpeakData[1], &sSpeakData[11], &sSpeakData[12], &sSpeakData[19],
    };

    if (((EnTest3_MoveTowardNextPoint(this, play, schResultData) ||
          (this->scheduleResult >= KAFEI_SCH_BECOMING_A_COUPLE)) &&
         ((schResultData->speakDataIndex == 1) || (schResultData->speakDataIndex == 2))) ||
        (schResultData->speakDataIndex == 4)) {
        if (schResultData->speakDataIndex == 4) {
            this->player.actor.home.rot.y = 0x7FFF;
        } else {
            this->player.actor.home.rot.y = this->player.actor.shape.rot.y + 0x8000;
        }
        this->player.stateFlags2 |= PLAYER_STATE2_40000;
        play->tryPlayerCsAction(play, &this->player, PLAYER_CSACTION_NEG1);
    }
    this->speakData = sNpcSpeakData[schResultData->speakDataIndex];
    return true;
}

s32 EnTest3_HandleSchedule_CanTalk(EnTest3* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->player.actor, &play->state)) {
        EnTest3_SetMainAction(this, EnTest3_Action_HandleTalk);
        this->player.focusActor = &GET_PLAYER(play)->actor;
        this->player.stateFlags2 &= ~PLAYER_STATE2_40000;
        sMetAtIkana = true;
        if ((this->speakData->talkActionIndex == ENTEST3_TALK_END) &&
            CHECK_WEEKEVENTREG(WEEKEVENTREG_KAFEI_ENTRUSTED_LINK)) {
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_KAFEI);
        }
    } else {
        if (play->actorCtx.flags & ACTORCTX_FLAG_4) {
            play->actorCtx.flags &= ~ACTORCTX_FLAG_4;
            this->player.stateFlags2 &= ~PLAYER_STATE2_40000;
            this->link->stateFlags1 |= PLAYER_STATE1_20;
            Actor_ChangeCategory(play, &play->actorCtx, &this->link->actor, ACTORCAT_NPC);
            Actor_ChangeCategory(play, &play->actorCtx, &this->player.actor, ACTORCAT_PLAYER);
            CutsceneManager_SetReturnCamera(this->subCamId);
            play->tryPlayerCsAction(play, &this->player, PLAYER_CSACTION_WAIT);
        }
        Actor_OfferTalkNearColChkInfoCylinder(&this->player.actor, play);
        if (this->scheduleResult == KAFEI_SCH_MEETING_LINK_IN_CURIOSITY_SHOP) {
            EnTest3_GetSpeakDataAtShop(this, play);
        } else if (this->scheduleResult == KAFEI_SCH_WAITING_OUTSIDE_SAKONS_HIDEOUT) {
            EnTest3_GetSpeakDataAtIkana(this, play);
        }
        this->player.actor.textId = this->speakData->textId;
        this->player.actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    }
    return false;
}

s32 EnTest3_ProcessSchedule_FindMailCarrier(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                            ScheduleOutput* scheduleOutput) {
    Actor* postActor;

    EnTest3_MoveTowardNextPoint(this, play, schResultData);
    if (((postActor = EnTest3_FindNearbyActor(play, this, ACTOR_EN_PST, ACTORCAT_PROP, 100.0f, 20.0f)) != NULL) ||
        ((postActor = EnTest3_FindNearbyActor(play, this, ACTOR_EN_PM, ACTORCAT_NPC, 100.0f, 20.0f)) != NULL)) {
        this->player.actor.home.rot.y = Actor_WorldYawTowardActor(&this->player.actor, postActor);
    }
    play->tryPlayerCsAction(play, &this->player, PLAYER_CSACTION_97);
    return true;
}

s32 EnTest3_HandleSchedule_TurnToMailCarrier(EnTest3* this, PlayState* play) {
    Math_ScaledStepToS(&this->player.actor.shape.rot.y, this->player.actor.home.rot.y, 0x320);
    this->player.yaw = this->player.actor.shape.rot.y;
    return false;
}

s32 EnTest3_ProcessSchedule_BellRung(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                     ScheduleOutput* scheduleOutput) {
    scheduleOutput->time0 = (u16)SCRIPT_TIME_NOW;
    scheduleOutput->time1 = (u16)(scheduleOutput->time0 + 70);
    EnTest3_ProcessSchedule_ComputeWaypoint(this, play, schResultData, scheduleOutput);
    if (this->player.actor.xzDistToPlayer < 300.0f) {
        this->bellTimer = -1;
    } else {
        this->bellTimer = 120;
    }
    return true;
}

s32 EnTest3_HandleSchedule_BellRung(EnTest3* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    u32 cond;
    EnTest3ScheduleResultData backToDoor;
    ScheduleOutput scheduleOutput;

    if (player->stateFlags1 & PLAYER_STATE1_TALKING) {
        return false;
    }
    cond = EnTest3_HandleSchedule_MoveToWaypoint(this, play);
    if (this->bellTimer > 0) {
        this->bellTimer--;
        cond = cond && (this->player.actor.xzDistToPlayer < 200.0f);
        if (cond || this->bellTimer <= 0) {
            EnTest3_EndCsAction(this, play);
            backToDoor.pathIndexAndDirection = 5;
            scheduleOutput.time0 = (u16)SCRIPT_TIME_NOW;
            scheduleOutput.time1 = (u16)(scheduleOutput.time0 + (cond ? 80 : 140));

            EnTest3_ProcessSchedule_ComputeWaypoint(this, play, &backToDoor, &scheduleOutput);
            this->bellTimer = -40;
            return false;
        }
        if (this->bellTimer == 90) {
            play->tryPlayerCsAction(play, &this->player, PLAYER_CSACTION_21);
        }
    } else {
        this->bellTimer++;
        if (this->bellTimer == 0) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_HIT_LAUNDRY_POOL_BELL);
            this->scheduleResult = KAFEI_SCH_NONE;
        }
    }
    return false;
}

s32 EnTest3_ProcessSchedule_SakonHideout(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                         ScheduleOutput* scheduleOutput) {
    return true;
}

s32 EnTest3_HandleSchedule_SakonHideout(EnTest3* this, PlayState* play) {
    if (sSakonHideoutPhase == 0) {
        if (!Play_InCsMode(play)) {
            sSakonHideoutPhase = 1;
            this->speakData = &sSpeakData[20];
            this->csId = this->player.actor.csId;
            this->player.actor.textId = this->speakData->textId;
        }
    } else if (sSakonHideoutPhase == 1) {
        if (this->csId > CS_ID_NONE) {
            if (EnTest3_StartCutscene(this, play)) {
                this->csId = CS_ID_NONE;
                Environment_StopTime();
            }
        } else if ((play->actorCtx.flags & ACTORCTX_FLAG_6) || (play->actorCtx.flags & ACTORCTX_FLAG_5)) {
            this->csId = CutsceneManager_GetAdditionalCsId(this->player.actor.csId);
            SET_WEEKEVENTREG(WEEKEVENTREG_90_02);
            if (play->actorCtx.flags & ACTORCTX_FLAG_5) {
                this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
            }
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
            sSakonHideoutPhase = 2;
        } else {
            EnTest3_HandleSchedule_CanTalk(this, play);
        }
    } else if ((sSakonHideoutPhase == 2) && EnTest3_StartCutscene(this, play)) {
        CutsceneManager_SetReturnCamera(CAM_ID_MAIN);
        Environment_StartTime();
        if (CURRENT_TIME > CLOCK_TIME(6, 0)) {
            Environment_SetTimeJump(TIME_TO_MINUTES_ALT_F(fabsf((s16)-CURRENT_TIME)));
        }
        if (play->actorCtx.flags & ACTORCTX_FLAG_6) {
            SET_WEEKEVENTREG(WEEKEVENTREG_ESCAPED_SAKONS_HIDEOUT);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_90_02);
        }
        sSakonHideoutPhase = 3;
    }
    return false;
}

s32 EnTest3_ProcessSchedule_RunToTownCs(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                        ScheduleOutput* scheduleOutput) {
    this->csId = CutsceneManager_GetAdditionalCsId(this->player.actor.csId);
    return true;
}

s32 EnTest3_HandleSchedule_RunToTownCs(EnTest3* this, PlayState* play) {
    EnTest3ScheduleResultData endPoint;
    ScheduleOutput scheduleOutput;

    if (sKafeiRunToTownPhase == ENTEST3_RUN_TO_TOWN_WAITING) {
        if (EnTest3_StartCutscene(this, play)) {
            endPoint.pathIndexAndDirection = 2;
            scheduleOutput.time0 = (u16)SCRIPT_TIME_NOW;
            scheduleOutput.time1 = (u16)(scheduleOutput.time0 + 1000);
            EnTest3_ProcessSchedule_ComputeWaypoint(this, play, &endPoint, &scheduleOutput);
            sKafeiRunToTownPhase = ENTEST3_RUN_TO_TOWN_RUNNING;
            return false;
        }
    } else if (sKafeiRunToTownPhase == ENTEST3_RUN_TO_TOWN_RUNNING) {
        EnTest3_HandleSchedule_MoveToWaypoint(this, play);
    } else if (sKafeiRunToTownPhase == ENTEST3_RUN_TO_TOWN_END) {
        CutsceneManager_Stop(this->csId);
        SET_WEEKEVENTREG(WEEKEVENTREG_90_02);
        sKafeiRunToTownPhase = ENTEST3_RUN_TO_TOWN_CLEARED;
    }
    return false;
}

s32 EnTest3_ProcessSchedule_CouplesMaskCs(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                          ScheduleOutput* scheduleOutput) {
    static Vec3f holdHandsPos = { -420.0f, 210.0f, -162.0f };

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_COUPLES_MASK_CUTSCENE_FINISHED)) {
        sKafeiCoupleCsPhase = ENTEST3_COUPLE_CS_PLAYING;
        Math_Vec3f_Copy(&this->player.actor.world.pos, &holdHandsPos);
        Math_Vec3f_Copy(&this->player.actor.home.pos, &holdHandsPos);

        this->player.actor.home.rot.y = -0x2AAB;
        this->player.actor.shape.rot.y = -0x2AAB;
        this->player.yaw = -0x2AAB;
        //! FAKE:
        if (1) {}
        return true;
    } else {
        EnTest3_MoveTowardNextPoint(this, play, schResultData);
        this->csId = this->player.actor.csId;
        if (play->roomCtx.curRoom.num == 2) {
            this->csId = CutsceneManager_GetAdditionalCsId(this->csId);
        }
        return true;
    }
}

s32 EnTest3_HandleSchedule_CouplesMaskCs(EnTest3* this, PlayState* play2) {
    PlayState* play = play2;

    if (sKafeiCoupleCsPhase == ENTEST3_COUPLE_CS_ENTERING) {
        if (GameInteractor_Should(VB_PLAY_COUPLES_MASK_CS, !Play_InCsMode(play) && (play->roomCtx.curRoom.num == 2),
                                  this, &sKafeiCoupleCsPhase)) {
            sKafeiCoupleCsPhase = ENTEST3_COUPLE_CS_STARTING;
        }
    } else if (sKafeiCoupleCsPhase == ENTEST3_COUPLE_CS_STARTING) {
        if (EnTest3_StartCutscene(this, play)) {
            sKafeiCoupleCsPhase = ENTEST3_COUPLE_CS_PLAYING;
        }
    } else {
        SET_WEEKEVENTREG(WEEKEVENTREG_COUPLES_MASK_CUTSCENE_FINISHED);
        play->tryPlayerCsAction(play, &this->player, PLAYER_CSACTION_110);
    }
    return false;
}

s32 EnTest3_ProcessSchedule_ComputeWaypoint(EnTest3* this, PlayState* play, EnTest3ScheduleResultData* schResultData,
                                            ScheduleOutput* scheduleOutput) {
    u16 now = SCRIPT_TIME_NOW;
    u16 startTime;
    u16 numWaypoints;

    EnTest3_MoveTowardNextPoint(this, play, schResultData);
    this->currentPath = SubS_GetAdditionalPath(play, KAFEI_GET_PATH_INDEX(&this->player.actor),
                                               ABS_ALT(schResultData->pathIndexAndDirection) - 1);
    if ((this->scheduleResult < KAFEI_SCH_INSIDE_SAKONS_HIDEOUT) && (this->scheduleResult != KAFEI_SCH_NONE) &&
        (this->timeSpeed >= 0)) {
        startTime = now;
    } else {
        startTime = scheduleOutput->time0;
    }
    if (scheduleOutput->time1 < startTime) {
        this->schPathRemainingTime = (startTime - scheduleOutput->time1) + (DAY_LENGTH - 1);
    } else {
        this->schPathRemainingTime = scheduleOutput->time1 - startTime;
    }
    this->schPathElapsedTime = now - startTime;
    numWaypoints = startTime = this->currentPath->count - (SUBS_TIME_PATHING_ORDER - 1);
    this->waypointRemainingTime = this->schPathRemainingTime / numWaypoints;
    this->waypointIndex = (this->schPathElapsedTime / this->waypointRemainingTime) + (SUBS_TIME_PATHING_ORDER - 1);
    this->isTimePathStarted &= ~1;
    this->waypointProgress = 1.0f;
    return true;
}

s32 EnTest3_HandleSchedule_MoveToWaypoint(EnTest3* this, PlayState* play) {
    f32 knots[265];
    Vec3f worldPos;
    Vec3f nextTargetPos;
    Vec3f currentTargetPos;
    s32 tempElapsedTime = 0;
    s32 tempWaypointIndex = 0;
    s32 pad1;
    f32 dx;
    f32 dy;
    s32 ret = false;

    SubS_TimePathing_FillKnots(knots, 3, this->currentPath->count + 3);
    if (!(this->isTimePathStarted & 1)) {
        currentTargetPos = gZeroVec3f;
        SubS_TimePathing_Update(this->currentPath, &this->schPathProgress, &this->schPathElapsedTime,
                                this->waypointRemainingTime, this->schPathRemainingTime, &this->waypointIndex, knots,
                                &currentTargetPos, this->timeSpeed);
        SubS_TimePathing_ComputeInitialY(play, this->currentPath, this->waypointIndex, &currentTargetPos);
        Math_Vec3f_Copy(&this->player.actor.home.pos, &currentTargetPos);
        Math_Vec3f_Copy(&this->player.actor.prevPos, &currentTargetPos);
        this->player.actor.world.pos.y = currentTargetPos.y;
        this->isTimePathStarted |= 1;
    } else {
        currentTargetPos = this->targetPos;
    }
    this->player.actor.world.pos.x = currentTargetPos.x;
    this->player.actor.world.pos.z = currentTargetPos.z;
    if (play->transitionMode != TRANS_MODE_OFF) {
        tempElapsedTime = this->schPathElapsedTime;
        tempWaypointIndex = this->waypointIndex;
        currentTargetPos = this->player.actor.world.pos;
    }
    this->targetPos = gZeroVec3f;
    if (SubS_TimePathing_Update(this->currentPath, &this->schPathProgress, &this->schPathElapsedTime,
                                this->waypointRemainingTime, this->schPathRemainingTime, &this->waypointIndex, knots,
                                &this->targetPos, this->timeSpeed)) {
        if (this->scheduleResult == KAFEI_SCH_ENTERING_SAKONS_HIDEOUT) {
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_58_80);
            this->player.actor.draw = NULL;
        } else if (this->scheduleResult == KAFEI_SCH_RUNNING_TO_TOWN) {
            sKafeiRunToTownPhase = ENTEST3_RUN_TO_TOWN_END;
        }
        ret = true;
    } else {
        worldPos = this->player.actor.world.pos;
        nextTargetPos = this->targetPos;
        this->player.actor.world.rot.y = Math_Vec3f_Yaw(&worldPos, &nextTargetPos);
    }
    if (play->transitionMode != TRANS_MODE_OFF) {
        this->schPathElapsedTime = tempElapsedTime;
        this->waypointIndex = tempWaypointIndex;
        this->targetPos = currentTargetPos;
    }
    dx = this->player.actor.world.pos.x - this->player.actor.prevPos.x;
    dy = this->player.actor.world.pos.z - this->player.actor.prevPos.z;
    if (!Math_StepToF(&this->waypointProgress, 1.0f, 0.1f)) {
        this->player.actor.world.pos.x = this->player.actor.prevPos.x + (dx * this->waypointProgress);
        this->player.actor.world.pos.z = this->player.actor.prevPos.z + (dy * this->waypointProgress);
    }
    Math_Vec3f_Copy(&sKafeiCurrentPathTargetPos, &this->player.actor.world.pos);
    dx = this->player.actor.world.pos.x - this->player.actor.prevPos.x;
    dy = this->player.actor.world.pos.z - this->player.actor.prevPos.z;
    this->player.speedXZ = sqrtf(SQ(dx) + SQ(dy));
    this->player.speedXZ *= 1.0f + (1.05f * fabsf(Math_SinS(this->player.floorPitch)));
    sKafeiControlStickMagnitude = (this->player.speedXZ * 10.0f) + 20.0f;
    sKafeiControlStickMagnitude = CLAMP_MAX(sKafeiControlStickMagnitude, 60.0f);
    sKafeiControlStickAngle = this->player.actor.world.rot.y;
    this->player.actor.world.pos.x = this->player.actor.prevPos.x;
    this->player.actor.world.pos.z = this->player.actor.prevPos.z;
    if (!EnTest3_IsOpeningDoor(this, play)) {
        sKafeiIsFollowingPath = true;
    }
    return ret;
}

void EnTest3_Action_FollowSchedule(EnTest3* this, PlayState* play) {
    EnTest3ScheduleResultData* schResultData;
    ScheduleOutput scheduleOutput;

    this->timeSpeed =
        ((this->scheduleResult == KAFEI_SCH_ENTERING_SAKONS_HIDEOUT) ||
         (this->scheduleResult == KAFEI_SCH_HEARD_BELL_RING) || (this->scheduleResult == KAFEI_SCH_RUNNING_TO_TOWN))
            ? 3
        : Play_InCsMode(play) ? 0
                              : R_TIME_SPEED + ((void)0, gSaveContext.save.timeSpeedOffset);

    if (Schedule_RunScript(play, sScheduleScript, &scheduleOutput)) {
        if (this->scheduleResult != scheduleOutput.result) {
            schResultData = &sKafeiScheduledResultData[scheduleOutput.result];
            EnTest3_EndCsAction(this, play);
            if (schResultData->schActionIndex != KAFEI_SCH_ACTION_BELL_RUNG) {
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_HIT_LAUNDRY_POOL_BELL);
            }
            if (!sScheduledActions[schResultData->schActionIndex].processSchFunc(this, play, schResultData,
                                                                                 &scheduleOutput)) {
                return;
            }
            if (scheduleOutput.result == 6) {
                this->player.actor.home.rot.y = 0x7FFF;
                this->player.stateFlags2 |= PLAYER_STATE2_40000;
                play->tryPlayerCsAction(play, &this->player, PLAYER_CSACTION_NEG1);
            }
        }
    } else {
        scheduleOutput.result = 0;
    }
    this->scheduleResult = scheduleOutput.result;
    schResultData = &sKafeiScheduledResultData[this->scheduleResult];
    sScheduledActions[schResultData->schActionIndex].handleSchFunc(this, play);
}

// Apparently unused as Kafei normally follows a schedule, but this function sets him up as a generic speakable NPC
// with one line of dialogue.
void EnTest3_Action_Unused(EnTest3* this, PlayState* play) {
    this->speakData = &sSpeakData[0];
    EnTest3_HandleSchedule_CanTalk(this, play);
}

void EnTest3_Action_HandleTalk(EnTest3* this, PlayState* play) {
    if (Actor_TextboxIsClosing(&this->player.actor, play)) {
        if ((this->speakData->argument == 0) ||
            !EnTest3_TalkTriggersAction(this, play, this->speakData->argument - 1)) {
            if (KAFEI_GET_PARAM_1E0(&this->player.actor) == 0) {
                EnTest3_SetMainAction(this, EnTest3_Action_Unused);
            } else {
                EnTest3_SetMainAction(this, EnTest3_Action_FollowSchedule);
            }
            this->player.focusActor = NULL;
        }
    } else if (EnTest3_NextTalkStateTriggersCutscene(this, play)) {
        EnTest3_SetMainAction(this, EnTest3_Action_GivePendant);
    }
}

void EnTest3_Action_GivePendant(EnTest3* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->player.actor, &play->state)) {
        EnTest3_SetMainAction(this, EnTest3_Action_HandleTalk);
        this->player.focusActor = &GET_PLAYER(play)->actor;
        SET_WEEKEVENTREG(WEEKEVENTREG_KAFEI_ENTRUSTED_LINK);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_PENDANT_OF_MEMORIES);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_KAFEI);
    } else {
        Actor_OfferTalkExchange(&this->player.actor, play, 9999.9f, 9999.9f, PLAYER_IA_MINUS1);
        this->speakData = &sSpeakData[6];
        this->player.actor.textId = this->speakData->textId;
        this->player.actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    }
}

void EnTest3_UpdateControl(EnTest3* this, PlayState* play) {
    if ((play->actorCtx.flags & ACTORCTX_FLAG_5) || (play->actorCtx.flags & ACTORCTX_FLAG_4)) {
        play->actorCtx.flags &= ~ACTORCTX_FLAG_4;
        EnTest3_ReturnControlToLink(this, play);
        CutsceneManager_SetReturnCamera(CAM_ID_MAIN);
    } else {
        sKafeiControlInput = *CONTROLLER1(&play->state);
    }
}

void EnTest3_PostTalk_EnablePeephole(EnTest3* this, PlayState* play) {
    SET_WEEKEVENTREG(WEEKEVENTREG_CAN_USE_CURIOSITY_SHOP_PEEPHOLE);
}

void EnTest3_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTest3* this = (EnTest3*)thisx;

    sKafeiControlInput.rel.button = sKafeiControlInput.cur.button;
    sKafeiControlInput.cur.button = 0;
    sKafeiControlInput.rel.stick_x = 0;
    sKafeiControlInput.rel.stick_y = 0;

    play->actorCtx.flags &= ~ACTORCTX_FLAG_7;
    this->player.actor.draw = EnTest3_Draw;
    sKafeiIsFollowingPath = false;
    this->player.actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_506) &&
        !((this->player.actor.category == ACTORCAT_PLAYER) &&
          ((play->actorCtx.flags & ACTORCTX_FLAG_5) || (play->actorCtx.flags & ACTORCTX_FLAG_4)))) {
        if (this->player.csAction != PLAYER_CSACTION_5) {
            play->tryPlayerCsAction(play, &this->player, PLAYER_CSACTION_5);
        }
        play->actorCtx.flags &= ~ACTORCTX_FLAG_4;
    } else if (this->player.actor.category == ACTORCAT_PLAYER) {
        EnTest3_UpdateControl(this, play);
    } else if (play->tryPlayerCsAction(play, &this->player, PLAYER_CSACTION_NONE)) {
        if (this->scheduleResult >= KAFEI_SCH_INSIDE_SAKONS_HIDEOUT) {
            Vec3f worldPos;

            Math_Vec3f_Copy(&worldPos, &this->player.actor.world.pos);
            this->timeSpeed = 4;
            EnTest3_HandleSchedule_MoveToWaypoint(this, play);
            Math_Vec3f_Copy(&this->player.actor.world.pos, &worldPos);
            sKafeiIsFollowingPath = false;
            this->waypointProgress = 0.0f;
        }
    } else {
        sKafeiControlStickMagnitude = 0.0f;
        sKafeiControlStickAngle = this->player.actor.shape.rot.y;
        this->mainActionFunc(this, play);
        sKafeiControlInput.press.button =
            (sKafeiControlInput.rel.button ^ sKafeiControlInput.cur.button) & sKafeiControlInput.cur.button;

        Actor_SetControlStickData(play, &sKafeiControlInput, sKafeiControlStickMagnitude, sKafeiControlStickAngle);
    }

    play->playerUpdate(&this->player, play, &sKafeiControlInput);

    if (sKafeiIsFollowingPath) {
        this->player.actor.world.pos.x = sKafeiCurrentPathTargetPos.x;
        this->player.actor.world.pos.z = sKafeiCurrentPathTargetPos.z;
        this->player.speedXZ = 0.0f;
    }
}

s32 D_80A418C8 = false;

s32 EnTest3_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnTest3* this = (EnTest3*)thisx;

    if (limbIndex == KAFEI_LIMB_ROOT) {
        sKafeiCurBodyPartPos = &this->player.bodyPartsPos[0] - 1;
        if (!(this->player.skelAnime.movementFlags & ANIM_FLAG_4) ||
            (this->player.skelAnime.movementFlags & ANIM_FLAG_1)) {
            pos->x *= this->player.ageProperties->unk_08;
            pos->z *= this->player.ageProperties->unk_08;
        }
        if (!(this->player.skelAnime.movementFlags & ANIM_FLAG_4) ||
            (this->player.skelAnime.movementFlags & ANIM_FLAG_UPDATE_Y)) {
            pos->y *= this->player.ageProperties->unk_08;
        }
        pos->y -= this->player.unk_AB8;
        if (this->player.unk_AAA != 0) {

            Matrix_Translate(pos->x, ((Math_CosS(this->player.unk_AAA) - 1.0f) * 200.0f) + pos->y, pos->z,
                             MTXMODE_APPLY);
            Matrix_RotateXS(this->player.unk_AAA, MTXMODE_APPLY);
            Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
            func_80125318(pos, rot);
        }
    } else {
        if (*dList != NULL) {
            sKafeiCurBodyPartPos++;
        }
        if (D_80A418C8) {
            *dList = NULL;
        }
        if (limbIndex == KAFEI_LIMB_HEAD) {
            rot->x += this->player.headLimbRot.z;
            rot->y -= this->player.headLimbRot.y;
            rot->z += this->player.headLimbRot.x;
        } else if (limbIndex == KAFEI_LIMB_UPPER_ROOT) {
            s32 requiredScopeTemp;

            if (this->player.upperLimbYawSecondary != 0) {
                Matrix_RotateZS(0x44C, MTXMODE_APPLY);
                Matrix_RotateYS(this->player.upperLimbYawSecondary, MTXMODE_APPLY);
            }
            if (this->player.upperLimbRot.y != 0) {
                Matrix_RotateYS(this->player.upperLimbRot.y, MTXMODE_APPLY);
            }
            Matrix_RotateXS(this->player.upperLimbRot.x, MTXMODE_APPLY);
            if (this->player.upperLimbRot.z != 0) {
                Matrix_RotateZS(this->player.upperLimbRot.z, MTXMODE_APPLY);
            }
        } else {
            func_80125500(play, &this->player, limbIndex, pos, rot);
        }
    }
    return false;
}

void EnTest3_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* thisx) {
    s32 pad;
    EnTest3* this = (EnTest3*)thisx;

    if (*dList2 != NULL) {
        Matrix_MultZero(sKafeiCurBodyPartPos);
    }
    if (limbIndex == KAFEI_LIMB_LEFT_HAND) {
        MtxF curMtxF;
        Actor* leftHandActor;

        Math_Vec3f_Copy(&this->player.leftHandWorld.pos, sKafeiCurBodyPartPos);
        if (*dList1 != NULL) {
            func_80128640(play, &this->player, *dList1);
            if (this->player.stateFlags3 & PLAYER_STATE3_20000000) {
                OPEN_DISPS(play->state.gfxCtx);

                gSPDisplayList(POLY_OPA_DISP++, gKafeiSunMaskDL);

                CLOSE_DISPS(play->state.gfxCtx);
            }
        }
        leftHandActor = this->player.heldActor;
        if ((leftHandActor != NULL) && (this->player.stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            Vec3s curRot;

            Matrix_Get(&curMtxF);
            Matrix_MtxFToYXZRot(&curMtxF, &curRot, false);
            leftHandActor->world.rot.y = this->player.actor.shape.rot.y + this->player.leftHandWorld.rot.y;
            leftHandActor->shape.rot.y = leftHandActor->world.rot.y;
        } else {
            Matrix_Get(&this->player.leftHandMf);
            Matrix_MtxFToYXZRot(&this->player.leftHandMf, &this->player.leftHandWorld.rot, false);
            func_80126B8C(play, &this->player);
        }

    } else if (limbIndex == KAFEI_LIMB_RIGHT_HAND) {
        Actor* leftHandActor = this->player.heldActor;

        if (leftHandActor != NULL) {
            leftHandActor->world.pos.x =
                (this->player.bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].x + this->player.leftHandWorld.pos.x) / 2.0f;
            leftHandActor->world.pos.y =
                (this->player.bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].y + this->player.leftHandWorld.pos.y) / 2.0f;
            leftHandActor->world.pos.z =
                (this->player.bodyPartsPos[PLAYER_BODYPART_RIGHT_HAND].z + this->player.leftHandWorld.pos.z) / 2.0f;
        }
    } else if (limbIndex == KAFEI_LIMB_HEAD) {
        Actor* focusActor = this->player.focusActor;

        if ((*dList1 != NULL) && ((u32)this->player.currentMask != PLAYER_MASK_NONE) &&
            !(this->player.stateFlags2 & PLAYER_STATE2_1000000)) {
            if ((!BEN_ANIM_EQUAL(this->player.skelAnime.animation, gPlayerAnim_cl_maskoff)) &&
                ((!BEN_ANIM_EQUAL(this->player.skelAnime.animation, gPlayerAnim_cl_setmask)) ||
                 (this->player.skelAnime.curFrame >= 12.0f))) {
                if (func_80127438(play, &this->player, this->player.currentMask)) {
                    OPEN_DISPS(play->state.gfxCtx);

                    gSPDisplayList(POLY_OPA_DISP++, object_mask_ki_tan_DL_0004A0);

                    CLOSE_DISPS(play->state.gfxCtx);
                }
            }
        }
        if ((focusActor != NULL) && (focusActor->id == ACTOR_BG_IKNV_OBJ)) {
            Math_Vec3f_Copy(&this->player.actor.focus.pos, &focusActor->focus.pos);
        } else {
            static Vec3f focusOffset = { 1100.0f, -700.0f, 0.0f };

            Matrix_MultVec3f(&focusOffset, &this->player.actor.focus.pos);
        }
    } else if (limbIndex == KAFEI_LIMB_TORSO) {
        if (sKafeiGavePendantToLink || CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_PENDANT_OF_MEMORIES) ||
            (INV_CONTENT(ITEM_PENDANT_OF_MEMORIES) == ITEM_PENDANT_OF_MEMORIES) ||
            (this->player.getItemDrawIdPlusOne - 1 == GID_PENDANT_OF_MEMORIES)) {
            sKafeiGavePendantToLink = true;
        } else {
            OPEN_DISPS(play->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, gKafeiPendantOfMemoriesDL);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    } else {
        Player_SetFeetPos(play, &this->player, limbIndex);
    }
}

static TexturePtr sEyeTextures[PLAYER_EYES_MAX] = {
    gKafeiEyesOpenTex,    // PLAYER_EYES_OPEN
    gKafeiEyesHalfTex,    // PLAYER_EYES_HALF
    gKafeiEyesClosedTex,  // PLAYER_EYES_CLOSED
    gKafeiEyesRightTex,   // PLAYER_EYES_RIGHT
    gKafeiEyesLeftTex,    // PLAYER_EYES_LEFT
    gKafeiEyesUpTex,      // PLAYER_EYES_UP
    gKafeiEyesDownTex,    // PLAYER_EYES_DOWN
    gKafeiEyesWincingTex, // PLAYER_EYES_WINCING
};

static TexturePtr sMouthTextures[PLAYER_MOUTH_MAX] = {
    gKafeiMouthClosedTex, // PLAYER_MOUTH_CLOSED
    gKafeiMouthHalfTex,   // PLAYER_MOUTH_HALF
    gKafeiMouthOpenTex,   // PLAYER_MOUTH_OPEN
    gKafeiMouthSmileTex,  // PLAYER_MOUTH_SMILE
};

static PlayerFaceIndices sKafeiFaces[PLAYER_FACE_MAX] = {
    // The first 6 faces defined must be default blinking faces. See relevant code in `Player_UpdateCommon`.
    { PLAYER_EYES_OPEN, PLAYER_MOUTH_CLOSED },   // PLAYER_FACE_NEUTRAL
    { PLAYER_EYES_HALF, PLAYER_MOUTH_CLOSED },   // PLAYER_FACE_NEUTRAL_BLINKING_HALF
    { PLAYER_EYES_CLOSED, PLAYER_MOUTH_CLOSED }, // PLAYER_FACE_NEUTRAL_BLINKING_CLOSED

    // This duplicate set of blinking faces is defined because Player will choose between the first and second set
    // based on gameplayFrames. See relevant code in `Player_UpdateCommon`.
    // This, in theory, allows for psuedo-random variance in the faces used. But in practice, duplicate faces are used.
    { PLAYER_EYES_OPEN, PLAYER_MOUTH_CLOSED },   // PLAYER_FACE_NEUTRAL_2
    { PLAYER_EYES_HALF, PLAYER_MOUTH_CLOSED },   // PLAYER_FACE_NEUTRAL_BLINKING_HALF_2
    { PLAYER_EYES_CLOSED, PLAYER_MOUTH_CLOSED }, // PLAYER_FACE_NEUTRAL_BLINKING_CLOSED_2

    // Additional faces. Most faces are encoded within animations.
    { PLAYER_EYES_LEFT, PLAYER_MOUTH_CLOSED },  // PLAYER_FACE_LOOK_LEFT
    { PLAYER_EYES_UP, PLAYER_MOUTH_HALF },      // PLAYER_FACE_SURPRISED
    { PLAYER_EYES_WINCING, PLAYER_MOUTH_OPEN }, // PLAYER_FACE_HURT
    { PLAYER_EYES_OPEN, PLAYER_MOUTH_OPEN },    // PLAYER_FACE_GASP
    { PLAYER_EYES_RIGHT, PLAYER_MOUTH_CLOSED }, // PLAYER_FACE_LOOK_RIGHT
    { PLAYER_EYES_LEFT, PLAYER_MOUTH_CLOSED },  // PLAYER_FACE_LOOK_LEFT_2
    { PLAYER_EYES_CLOSED, PLAYER_MOUTH_OPEN },  // PLAYER_FACE_EYES_CLOSED_MOUTH_OPEN
    { PLAYER_EYES_HALF, PLAYER_MOUTH_HALF },    // PLAYER_FACE_OPENING
    { PLAYER_EYES_OPEN, PLAYER_MOUTH_OPEN },    // PLAYER_FACE_EYES_AND_MOUTH_OPEN
    // The mouth in this entry deviates from player. Similar to OoT's `sPlayerFaces`.
    { PLAYER_EYES_OPEN, PLAYER_MOUTH_CLOSED }, // PLAYER_FACE_EYES_AND_MOUTH_OPEN
};

void EnTest3_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTest3* this = (EnTest3*)thisx;
    s32 eyeIndex = GET_EYE_INDEX_FROM_JOINT_TABLE(this->player.skelAnime.jointTable);
    s32 mouthIndex = GET_MOUTH_INDEX_FROM_JOINT_TABLE(this->player.skelAnime.jointTable);
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    func_8012C268(&play->state);
    if (this->player.invincibilityTimer > 0) {
        s32 temp2; // Must exist for stack order. Could hold the result of CLAMP instead.

        this->player.unk_B5F += CLAMP(50 - this->player.invincibilityTimer, 8, 40);
        temp2 = Math_CosS(this->player.unk_B5F * 0x100) * 2000.0f;
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 0, 0, 4000 - temp2);
    }
    func_800B8050(&this->player.actor, play, 0);
    D_80A418C8 = false;
    if (this->player.stateFlags1 & PLAYER_STATE1_100000) {
        Vec3f sp4C;

        SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &this->player.actor.focus.pos, &sp4C);
        if (sp4C.z < -4.0f) {
            D_80A418C8 = true;
        }
    }

    gfx = POLY_OPA_DISP;

    // If the eyes index provided by the animation is negative, use the value provided by the `face` argument instead
    if (eyeIndex < 0) {
        eyeIndex = sKafeiFaces[this->player.actor.shape.face].eyeIndex;
    }

    gSPSegment(&gfx[0], 0x08, Lib_SegmentedToVirtual(sEyeTextures[eyeIndex]));

    // If the mouth index provided by the animation is negative, use the value provided by the `face` argument instead
    if (mouthIndex < 0) {
        mouthIndex = sKafeiFaces[this->player.actor.shape.face].mouthIndex;
    }

    gSPSegment(&gfx[1], 0x09, Lib_SegmentedToVirtual(sMouthTextures[mouthIndex]));

    POLY_OPA_DISP = &gfx[2];

    SkelAnime_DrawFlexLod(play, this->player.skelAnime.skeleton, this->player.skelAnime.jointTable,
                          this->player.skelAnime.dListCount, EnTest3_OverrideLimbDraw, EnTest3_PostLimbDraw,
                          &this->player.actor, 0);

    if (this->player.invincibilityTimer > 0) {
        POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
    }

    if ((this->player.getItemDrawIdPlusOne - 1) != GID_NONE) {
        Player_DrawGetItem(play, &this->player);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnTest3_Reset(void) {
    sSakonHideoutPhase = 0;
    sExists = false;
    sMetAtIkana = false;
    sKafeiGavePendantToLink = false;
    sKafeiRunToTownPhase = ENTEST3_RUN_TO_TOWN_WAITING;
    sKafeiCoupleCsPhase = ENTEST3_COUPLE_CS_ENTERING;
}
