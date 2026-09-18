#include "Actions.h"

#include "2s2h/NameTag/NameTag.h"

extern "C" {
#include "z64.h"
#include "z64math.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
}

static GIActions::Register spawnActorAction({
    .id = GI_ACTION_SPAWN_ACTOR,
    .name = "spawnActor",
    .displayName = "Spawn Actor",
    .schema =
        {
            { .name = "actorId", .type = GI_PARAM_INT, .required = true, .min = 0.0f, .max = ACTOR_ID_MAX - 1.0f },
            { .name = "posX", .type = GI_PARAM_FLOAT },
            { .name = "posY", .type = GI_PARAM_FLOAT },
            { .name = "posZ", .type = GI_PARAM_FLOAT },
            { .name = "rotX", .type = GI_PARAM_INT },
            { .name = "rotY", .type = GI_PARAM_INT },
            { .name = "rotZ", .type = GI_PARAM_INT },
            { .name = "params", .type = GI_PARAM_INT },
            // Coordinates relative to the player: x+ is to their right, y+ up, z+ in front.
            { .name = "relativeCoords", .type = GI_PARAM_BOOL },
            // Optional label drawn above the spawned actor.
            { .name = "nametag", .type = GI_PARAM_STRING },
        },
    .onStart =
        [](GIAction& action) {
            Player* player = GET_PLAYER(gPlayState);
            f32 posX = action.params.Float("posX");
            f32 posY = action.params.Float("posY");
            f32 posZ = action.params.Float("posZ");
            s16 rotX = (s16)action.params.Int("rotX");
            s16 rotY = (s16)action.params.Int("rotY");
            s16 rotZ = (s16)action.params.Int("rotZ");
            Actor* actor;

            if (action.params.Bool("relativeCoords")) {
                // Math_SinS/CosS, not sinf/cosf: rot.y is a binary angle, not radians.
                f32 s = Math_SinS(player->actor.world.rot.y);
                f32 c = Math_CosS(player->actor.world.rot.y);
                f32 x = posX * c - posZ * s;
                f32 z = posX * s + posZ * c;
                actor = Actor_Spawn(&gPlayState->actorCtx, gPlayState, (s16)action.params.Int("actorId"),
                                    player->actor.world.pos.x + x, player->actor.world.pos.y + posY,
                                    player->actor.world.pos.z + z, 0, rotY + player->actor.world.rot.y, 0,
                                    action.params.Int("params"));
            } else {
                actor = Actor_Spawn(&gPlayState->actorCtx, gPlayState, (s16)action.params.Int("actorId"), posX, posY,
                                    posZ, rotX, rotY, rotZ, action.params.Int("params"));
            }

            const std::string& nametag = action.params.String("nametag");
            if (actor != NULL && !nametag.empty()) {
                NameTag_RegisterForActor(actor, nametag.c_str());
            }
        },
});
