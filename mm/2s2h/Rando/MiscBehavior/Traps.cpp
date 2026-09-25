#include "Traps.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "MiscBehavior.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/GameInteractor/Actions/Actions.h"

static GIActionId currentTrap = GI_ACTION_FREEZE;

static const std::map<GIActionId, const char*> trapToCvarMap = {
    { GI_ACTION_FREEZE, "gRando.Traps.Freeze" },       { GI_ACTION_BLAST, "gRando.Traps.Blast" },
    { GI_ACTION_SHOCK, "gRando.Traps.Shock" },         { GI_ACTION_JINX, "gRando.Traps.Jinx" },
    { GI_ACTION_EMPTY_WALLET, "gRando.Traps.Wallet" }, { GI_ACTION_SPAWN_LIKE_LIKE, "gRando.Traps.Enemy" },
    { GI_ACTION_SKIP_TIME, "gRando.Traps.Time" },      { GI_ACTION_BURN, "gRando.Traps.Fire" },
    { GI_ACTION_KNOCKBACK, "gRando.Traps.Knockback" },
};

static std::vector<GIActionId> GetEnabledTrapTypes() {
    std::vector<GIActionId> enabledTrapTypes;
    for (auto& trap : trapToCvarMap) {
        if (CVarGetInteger(trap.second, 0)) {
            enabledTrapTypes.push_back(trap.first);
        }
    }
    if (enabledTrapTypes.size() == 0) {
        enabledTrapTypes.push_back(GI_ACTION_FREEZE);
    }
    return enabledTrapTypes;
}

void RollTrapType() {
    auto enabledTraps = GetEnabledTrapTypes();
    currentTrap = enabledTraps[rand() % enabledTraps.size()];
}

// clang-format off
std::vector<std::string> defaultTrapMessages = {
    "This item is available in the %bRando DLC%w.",
    "This is what happens when %gCaladius%w is left unsupervised.",
    "Oh no!",
    "Uh oh!",
    "KEKW",
    "We've been trying to reach you about your Horse's %gextended warranty%w.",
    "Admit it, you wish this was %gGreg%w.",
    "Error 404: Item Not Found",
    "Get dunked on!",
    "This %rTrap%w is brought to you by today's sponsor...\n%gRaid Shadow Legends%w!",
};

std::vector<std::string> freezeTrapMessages = {
    "%rOcarina of Time%w called, they want their %bIce Trap%w back.",
    "Greetings from %bSnowhead%w! Wish you were here.",
    "This item was too %bcool%w for you anyway.",
    "Let me see your best %bKing Zora%w impersonation.",
    "There's no business like %bSnow%w business!",
    "How much does a polar bear weigh? Enough to break the %bice%w.",
    "You found the %yTrifo%w... Wait, nevermind...",
    "Quick Time Event! Don't die.",
};

std::vector<std::string> blastTrapMessages = {
    "Coming to you live from the %yThunderdome%w!",
    "There was supposed to be an Earth shattering %yKaboom%w!",
    "Pardon me while I %yburst%w.",
    "This is not the greatest %yblast%w in the world, this is just a tribute.",
    "Hey look, this %yitem%w is ticking!",
    "This is sure to make %gPamela%w leave the house.",
    "You found a %yDodongo Snack%w!",
};

std::vector<std::string> shockTrapMessages = {
    "We're losing him!\n%gCLEAR%w",
    "It's got what %gplants%w need.",
    "On todays episode of Grey's Anatomy...",
    "I'm giving it all I've got, captain!",
    "Now simulating a CD in a Microwave.",
    "Hang in there, for %gMeryl%w."
};

std::vector<std::string> jinxTrapMessages = {
    "Looks like someone's got a case of the %bMondays%w. :( ",
    "Your wrist hurts! The doctor says no more swords.",
};

std::vector<std::string> walletTrapMessages = {
    "Spare some %rchange%w?",
    "%rBreaking News%w: the Moon hasn't crashed, but Termina's economy sure has."
};

std::vector<std::string> enemyTrapMessages = {
    "You made a new friend!",
    // Like Like Specific, will need to adjust if new enemies are added.
    "Someone likes you! They %rLike Like%w you!",
    "You don't need a shield anyway.",
    "This item sucks.",
};

std::vector<std::string> timeTrapMessages = {
    "%yTime%w flashes before your eyes!",
    "You have played the %ySun's Song%w!",
    "The %rGoddess of Time%w smites you!",
    "Spent an hour and a half admiring this %bfake item%w.",
    "You found a great place to take a nap!",
    "Break time! Not like the world is ending right?",
    "I just need you to tell me how to get to the %ytime machine%w.",
    "Mweep",
};

std::map<GIActionId, std::vector<std::string>> trapMessageList = {
    { GI_ACTION_FREEZE, freezeTrapMessages },
    { GI_ACTION_BLAST, blastTrapMessages },
    { GI_ACTION_SHOCK, shockTrapMessages },
    { GI_ACTION_JINX, jinxTrapMessages },
    { GI_ACTION_EMPTY_WALLET, walletTrapMessages },
    { GI_ACTION_SPAWN_LIKE_LIKE, enemyTrapMessages },
    { GI_ACTION_SKIP_TIME, timeTrapMessages },
    { GI_ACTION_BURN, defaultTrapMessages },
    { GI_ACTION_KNOCKBACK, defaultTrapMessages },
};
// clang-format on

std::string GetTrapMessage() {
    auto findIt = trapMessageList.find(currentTrap);
    if (findIt == trapMessageList.end()) {
        return defaultTrapMessages[rand() % defaultTrapMessages.size()];
    }
    return findIt->second[rand() % findIt->second.size()];
}

void Rando::MiscBehavior::OfferTrapItem() {
    if (!gPlayState) {
        return;
    }

    const GIActions::Definition* definition = GIActions::Get(currentTrap);
    if (definition == nullptr) {
        return;
    }

    if (auto request = definition->Build({})) {
        GameInteractor::Instance->Queue(std::move(*request));
    }
}
