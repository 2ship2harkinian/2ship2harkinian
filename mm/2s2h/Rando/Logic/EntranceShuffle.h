#ifndef RANDO_ENTRANCE_SHUFFLE_H
#define RANDO_ENTRANCE_SHUFFLE_H

#include "Rando/Rando.h"

extern "C" {
#include "functions.h"
#include "variables.h"
}

namespace Rando {

namespace EntranceShuffle {

// Builds the entrance layout for the current save. Deterministic in the save's seed, so file
// create and every later file load produce the same layout.
void ShuffleEntrances();

// Where an entrance leads under the current layout; identity for anything that isn't shuffled.
s32 GetShuffledEntrance(s32 originalEntrance);

bool IsEntranceShuffleEnabled();

// Where a file begins once the layout is applied. The start is shuffled like any other entrance, and
// anything that returns the player "to the start" (the Song of Time reset, a moon crash) has to use
// this rather than assuming South Clock Town.
s32 GetStartEntrance();

// Where an exit leads under the current layout. Anything that walks the region graph must resolve
// exits through this so no two consumers disagree about the shape of the world.
s32 ResolveExit(RandoRegionId fromRegion, s32 exitId, s32 returnEntrance);

} // namespace EntranceShuffle

} // namespace Rando

#endif // RANDO_ENTRANCE_SHUFFLE_H
