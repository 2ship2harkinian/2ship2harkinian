#ifndef RANDO_CSMC_H
#define RANDO_CSMC_H

#include "Rando/Rando.h"

RandoItemType CSMC_GetTintType(RandoCheckId randoCheckId);
void CSMC_BeginTint(Gfx** gfx, RandoItemType randoItemType);
void CSMC_EndTint(Gfx** gfx);
void CSMC_DrawActorOpa(Actor* actor, PlayState* play, const char* dList, ActorFunc vanillaDraw);
void CSMC_DrawDListOpa(PlayState* play, RandoCheckId randoCheckId, const char* dList, const char* vanillaDList);

#endif // RANDO_CSMC_H
