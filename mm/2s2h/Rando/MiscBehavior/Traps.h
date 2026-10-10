#ifndef RANDO_TRAP_H
#define RANDO_TRAP_H

#include "Rando/Rando.h"
#include "2s2h/GameInteractor/GameInteractorAction.h"

// Picks the next trap from the enabled pool and remembers it, so that GetTrapMessage() and the
// eventual OfferTrapItem() agree on what the player is getting.
void RollTrapType();
std::string GetTrapMessage();

#endif
