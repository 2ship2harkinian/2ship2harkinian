#include "Anchor.h"
#include "2s2h/NameTag/NameTag.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

extern "C" {
#include "macros.h"
#include "variables.h"
#include "functions.h"
#include "z64malloc.h"
extern PlayState* gPlayState;

#include "objects/object_test3/object_test3.h"
#include "objects/gameplay_keep/gameplay_keep.h"
extern PlayerAgeProperties sPlayerAgeProperties[PLAYER_FORM_MAX];
extern TexturePtr sPlayerEyesTextures[PLAYER_FORM_MAX][PLAYER_EYES_MAX];
extern EffectBlureInit2 D_8085D30C;
extern EffectTireMarkInit D_8085D330;
void Player_DrawGameplay(PlayState* play, Player* player, s32 lod, Gfx* cullDList,
                         OverrideLimbDrawFlex overrideLimbDraw);
void Player_Anim_PlayOnceMorph(PlayState* play, Player* player, PlayerAnimationHeader* anim);
PlayerAnimationHeader* Player_GetIdleAnim(Player* player);
}

// Hijacking player->zTargetActiveTimer (unused s32 for the dummy) to store the clientId for convenience
#define DUMMY_CLIENT_ID player->zTargetActiveTimer

static DamageTable DummyPlayerDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Horse trample  */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Explosives     */ DMG_ENTRY(3, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Zora boomerang */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_STUN),
    /* Normal arrow   */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE),
    /* Hookshot       */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_STUN),
    /* Goron punch    */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_KNOCKBACK_SMALL),
    /* Sword          */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Goron pound    */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE),
    /* Fire arrow     */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_FIRE),
    /* Ice arrow      */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_ICE_TRAP),
    /* Light arrow    */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK),
    /* Goron spikes   */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Deku spin      */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Deku bubble    */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Deku launch    */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_KNOCKBACK_SMALL),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_ICE_TRAP),
    /* Zora barrier   */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK),
    /* Normal shield  */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_NONE),
    /* Light ray      */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_NONE),
    /* Thrown object  */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Zora punch     */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Spin attack    */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Sword beam     */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK),
    /* Normal Roll    */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_NONE),
    /* Unblockable    */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE),
    /* Powder Keg     */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
};

// Modeled after EnTest3_Init and Player_Init
void DummyPlayer_Init(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    uint32_t clientId = Anchor::Instance->actorIndexToClientId[actor->params];
    DUMMY_CLIENT_ID = clientId;

    if (!Anchor::Instance->clients.contains(DUMMY_CLIENT_ID)) {
        Actor_Kill(actor);
        return;
    }

    AnchorClient& client = Anchor::Instance->clients[DUMMY_CLIENT_ID];

    player->actor.room = -1;
    player->csId = CS_ID_NONE;
    player->transformation = client.transformation;
    player->ageProperties = &sPlayerAgeProperties[player->transformation];
    player->heldItemAction = PLAYER_IA_NONE;
    player->heldItemId = ITEM_OCARINA_OF_TIME;

    Player_SetModelGroup(player, PLAYER_MODELGROUP_DEFAULT);
    play->playerInit(player, play, gPlayerSkeletons[player->transformation]);

    // Skipping Effect_Add(...), the dummy doesn't need weapon effects

    player->maskObjectSegment = ZeldaArena_Malloc(0x3800);
    // Skipping part of play->func_18780, specifically Player_SetAction
    Player_Anim_PlayOnceMorph(play, player, Player_GetIdleAnim(player));
    player->yaw = player->actor.shape.rot.y;

    // Ensures the actor is always updating/drawing even when culled/out of distance
    actor->flags =
        ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_INSIDE_CULLING_VOLUME;
    player->cylinder.base.acFlags = AC_ON | AC_TYPE_PLAYER;
    player->cylinder.base.ocFlags2 = OC2_TYPE_1;
    player->cylinder.elem.acElemFlags = ACELEM_ON | ACELEM_HOOKABLE | ACELEM_NO_HITMARK;
    player->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    player->cylinder.dim.radius = 30;
    player->actor.colChkInfo.damageTable = &DummyPlayerDamageTable;

    bool isGlobalRoom = (std::string("2ship-global") == CVarGetString("gNetwork.Anchor.RoomId", ""));
    if (!isGlobalRoom) {
        NameTag_RegisterForActorWithOptions(actor, client.name.c_str(), { .yOffset = 30 });
    }
}

// Update the actor with new data from the client
void DummyPlayer_Update(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    if (!Anchor::Instance->clients.contains(DUMMY_CLIENT_ID)) {
        Actor_Kill(actor);
        return;
    }

    AnchorClient& client = Anchor::Instance->clients[DUMMY_CLIENT_ID];

    if (client.sceneId != gPlayState->sceneId || !client.online || !client.isSaveLoaded) {
        actor->world.pos.x = -9999.0f;
        actor->world.pos.y = -9999.0f;
        actor->world.pos.z = -9999.0f;
        actor->shape.shadowAlpha = 0;
        return;
    }

    actor->shape.shadowAlpha = 255;
    Math_Vec3s_Copy(&actor->shape.rot, &client.posRot.rot);
    Math_Vec3f_Copy(&actor->world.pos, &client.posRot.pos);
    memcpy(&player->jointTableBuffer, &client.jointTable, 159);
    memcpy(&player->jointTableUpperBuffer, &client.upperJointTable, 159);
    player->maskObjectLoadState = 0;
    player->maskId = player->currentMask;
    player->currentMask = client.currentMask;
    player->rightHandType = client.rightHandType;
    player->leftHandType = client.leftHandType;
    player->currentShield = client.currentShield;
    player->sheathType = client.sheathType;
    player->heldItemAction = client.heldItemAction;
    player->heldItemId = client.heldItemId;
    player->itemAction = client.itemAction;
    player->stateFlags1 = client.stateFlags1;
    player->stateFlags2 = client.stateFlags2;
    player->stateFlags3 = client.stateFlags3;
    player->unk_B0C = client.unk_B0C;
    player->unk_B28 = client.unk_B28;
    player->unk_ACC = client.unk_ACC;
    player->invincibilityTimer = client.invincibilityTimer;

    PlayerItemAction modelItemAction =
        (PlayerItemAction)(player->itemAction > PLAYER_IA_MINUS1 ? player->itemAction : player->heldItemAction);
    if (modelItemAction < PLAYER_IA_NONE || modelItemAction >= PLAYER_IA_MAX) {
        modelItemAction = PLAYER_IA_NONE;
    }
    Player_SetModels(player, Player_ActionToModelGroup(player, modelItemAction));

    if (Anchor::Instance->roomState.pvpMode == 0 ||
        (Anchor::Instance->roomState.pvpMode == 1 &&
         client.teamId == CVarGetString("gNetwork.Anchor.TeamId", "default"))) {
        actor->flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
        return;
    }

    actor->flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;

    if (player->cylinder.base.acFlags & AC_HIT && player->invincibilityTimer == 0) {
        Anchor::Instance->SendPacket_DamagePlayer(client.clientId, player->actor.colChkInfo.damageEffect,
                                                  player->actor.colChkInfo.damage);
        player->actor.colChkInfo.damage = 0;
        if (player->actor.colChkInfo.damageEffect == DUMMY_PLAYER_HIT_RESPONSE_STUN) {
            Actor_SetColorFilter(&player->actor, 0, 0xFF, 0, 24);
        } else {
            player->invincibilityTimer = 20;
        }
    }

    Collider_UpdateCylinder(&player->actor, &player->cylinder);

    if (!(player->stateFlags2 & PLAYER_STATE2_4000)) {
        if (!(player->stateFlags1 & (PLAYER_STATE1_4 | PLAYER_STATE1_DEAD | PLAYER_STATE1_2000 | PLAYER_STATE1_4000 |
                                     PLAYER_STATE1_800000))) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &player->cylinder.base);
        }

        if (!(player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_4000000)) &&
            (player->invincibilityTimer <= 0)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &player->cylinder.base);

            if (player->invincibilityTimer < 0) {
                CollisionCheck_SetAT(play, &play->colChkCtx, &player->cylinder.base);
            }
        }
    }

    if (player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000)) {
        player->actor.colChkInfo.mass = MASS_IMMOVABLE;
    } else {
        player->actor.colChkInfo.mass = 50;
    }

    Collider_ResetCylinderAC(play, &player->cylinder.base);
}

void DummyPlayer_Draw(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    if (!Anchor::Instance->clients.contains(DUMMY_CLIENT_ID)) {
        Actor_Kill(actor);
        return;
    }

    AnchorClient& client = Anchor::Instance->clients[DUMMY_CLIENT_ID];

    if (client.sceneId != gPlayState->sceneId || !client.online || !client.isSaveLoaded) {
        return;
    }

    Player_DrawGameplay(play, player, 1, gCullBackDList, Player_OverrideLimbDrawGameplayDefault);
}

void DummyPlayer_Destroy(Actor* actor, PlayState* play) {
}
