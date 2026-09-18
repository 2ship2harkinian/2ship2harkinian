#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "EntranceShuffle.h"
#include "Logic.h"

#include <map>
#include <set>
#include <spdlog/spdlog.h>
#include <vector>

extern "C" {
#include "z64scene.h"
}

namespace Rando {

namespace EntranceShuffle {

struct EntrancePair {
    s32 entrance; // the entrance you take to enter Target area from source
    s32 exit;     // the exit you take to leave Target area to source
};

std::map<s32, s32> sEntranceMap;

// Hand-curated pools. Every entry must correspond to a two-way exit in the Regions graph; an entry
// with no matching returnEntrance is silently dropped by ConvertSetToEntrancePairs.

std::set<s32> interiorEntrances = {
    // Clock Town
    ENTRANCE(ASTRAL_OBSERVATORY, 0),
    ENTRANCE(ASTRAL_OBSERVATORY, 1), // Termina Field-side door of the pass-through observatory
    ENTRANCE(TREASURE_CHEST_SHOP, 0),
    ENTRANCE(HONEY_AND_DARLINGS_SHOP, 0),
    ENTRANCE(MAYORS_RESIDENCE, 0),
    ENTRANCE(TOWN_SHOOTING_GALLERY, 0),
    ENTRANCE(STOCK_POT_INN, 0),
    ENTRANCE(STOCK_POT_INN, 1),
    ENTRANCE(MILK_BAR, 0),
    ENTRANCE(CURIOSITY_SHOP, 1),
    ENTRANCE(FAIRY_FOUNTAIN, 0),
    ENTRANCE(CLOCK_TOWER_INTERIOR, 1),
    ENTRANCE(SWORDMANS_SCHOOL, 0),
    ENTRANCE(CURIOSITY_SHOP, 0),
    ENTRANCE(TRADING_POST, 0),
    ENTRANCE(BOMB_SHOP, 0),
    ENTRANCE(POST_OFFICE, 0),
    ENTRANCE(LOTTERY_SHOP, 0),
    // Termina Field & Roads
    ENTRANCE(SWAMP_SHOOTING_GALLERY, 0),
    ENTRANCE(TOURIST_INFORMATION, 0),
    ENTRANCE(MAGIC_HAGS_POTION_SHOP, 0),
    // Milk Road
    ENTRANCE(CUCCO_SHACK, 0),
    ENTRANCE(DOGGY_RACETRACK, 0),
    ENTRANCE(RANCH_HOUSE, 0),
    ENTRANCE(RANCH_HOUSE, 1),
    // Mountain Village
    ENTRANCE(GORON_SHOP, 0),
    ENTRANCE(MOUNTAIN_SMITHY, 0),
    ENTRANCE(GORON_SHRINE, 0),
    // Great Bay
    ENTRANCE(FISHERMANS_HUT, 0),
    ENTRANCE(MARINE_RESEARCH_LAB, 0),
    ENTRANCE(ZORA_HALL_ROOMS, 0),
    ENTRANCE(ZORA_HALL_ROOMS, 1),
    ENTRANCE(ZORA_HALL_ROOMS, 2),
    ENTRANCE(ZORA_HALL_ROOMS, 3),
    ENTRANCE(ZORA_HALL_ROOMS, 5),
    ENTRANCE(OCEANSIDE_SPIDER_HOUSE, 0),
    // Ikana
    ENTRANCE(GHOST_HUT, 0),
    ENTRANCE(MUSIC_BOX_HOUSE, 0),
    ENTRANCE(SAKONS_HIDEOUT, 0),
    ENTRANCE(SECRET_SHRINE, 0),
    // Great Fairy Fountains
    ENTRANCE(FAIRY_FOUNTAIN, 1),
    ENTRANCE(FAIRY_FOUNTAIN, 2),
    ENTRANCE(FAIRY_FOUNTAIN, 3),
    ENTRANCE(FAIRY_FOUNTAIN, 4),
    // Other
    ENTRANCE(SWAMP_SPIDER_HOUSE, 0),
};

std::set<s32> dungeonEntrances = {
    ENTRANCE(WOODFALL_TEMPLE, 0),
    ENTRANCE(SNOWHEAD_TEMPLE, 0),
    ENTRANCE(GREAT_BAY_TEMPLE, 0),
    // The whole Stone Tower complex (exterior, both temple halves, and the light-arrow flip
    // between them) is treated as one dungeon entered from Ikana Canyon, so its shuffled entry
    // point is the exterior approach rather than a temple door.
    ENTRANCE(STONE_TOWER, 0),
};

// Dungeon interior scenes. Each dungeon's designated front door is shuffled through dungeonEntrances
// above; any *other* connection into these scenes (e.g. Woodfall's post-clear Deku Princess door)
// stays vanilla, so overworld pairs that touch them are skipped.
std::set<s32> dungeonScenes = {
    ENTR_SCENE_WOODFALL_TEMPLE,
    ENTR_SCENE_SNOWHEAD_TEMPLE,
    ENTR_SCENE_GREAT_BAY_TEMPLE,
};

// Self-contained multi-scene complexes. Connections *between* their rooms read as clean, reversible
// pairs but are really internal transitions, so any pair with both endpoints inside these scenes
// stays pinned to its vanilla connection and is never shuffled. Each complex is still reached
// through its external entrance(s), which are shuffled normally:
//   Stone Tower      -> entered from Ikana Canyon   (STONE_TOWER above, so via the dungeon pool)
//   Pirates Fortress -> entered from Great Bay Coast (overworld pool)
//   Deku Palace      -> entered from Southern Swamp  (overworld pool, two entrances)
//   Ikana Castle     -> entered from Ikana Canyon and Beneath the Well (overworld pool)
std::set<s32> complexScenes = {
    ENTR_SCENE_STONE_TOWER,
    ENTR_SCENE_STONE_TOWER_INVERTED,
    ENTR_SCENE_STONE_TOWER_TEMPLE,
    ENTR_SCENE_STONE_TOWER_TEMPLE_INVERTED,
    ENTR_SCENE_PIRATES_FORTRESS,
    ENTR_SCENE_PIRATES_FORTRESS_EXTERIOR,
    ENTR_SCENE_PIRATES_FORTRESS_INTERIOR,
    ENTR_SCENE_DEKU_PALACE,
    ENTR_SCENE_DEKU_KINGS_CHAMBER,
    ENTR_SCENE_DEKU_SHRINE,
    ENTR_SCENE_IKANA_CASTLE,
    ENTR_SCENE_IGOS_DU_IKANAS_LAIR,
};

std::vector<EntrancePair> ConvertSetToEntrancePairs(const std::set<s32>& entranceSet) {
    std::vector<EntrancePair> entrancePairs;
    for (s32 entrance : entranceSet) {
        auto randoRegionId = Rando::Logic::GetRegionIdFromEntrance(entrance);
        for (const auto& [exitId, regionExit] : Rando::Logic::Regions[randoRegionId].exits) {
            if (regionExit.returnEntrance == entrance) {
                entrancePairs.push_back({ entrance, exitId });
                break;
            }
        }
    }

    return entrancePairs;
}

s32 SceneOf(s32 entrance) {
    return (entrance >> 9) & 0x7F;
}

// Grotto entrances live in the shared GROTTOS scene and reuse a handful of spawn points
// between many grottos, so they can't be reversed cleanly and are never shuffled.
bool IsGrottoEntrance(s32 entrance) {
    return SceneOf(entrance) == ENTR_SCENE_GROTTOS;
}

bool IsComplexInternalEntrance(s32 entrance) {
    return complexScenes.count(SceneOf(entrance)) > 0;
}

bool IsDungeonSceneEntrance(s32 entrance) {
    return dungeonScenes.count(SceneOf(entrance)) > 0;
}

// Every reversible entrance pair that isn't already represented by the interior or dungeon
// pools, derived from the region graph instead of a hardcoded list. Entrances that are
// one-way, grottos, or that share a spawn point with another connection (e.g. the field
// spawn every Termina Field grotto returns to) can't be shuffled cleanly, so they're skipped.
std::vector<EntrancePair> GetOverworldEntrances() {
    // Collect every two-way exit as a (destination, returnEntrance) edge and count how often
    // each entrance is used on either side. Only entrances that are unique on both sides can
    // form an unambiguous, reversible pair.
    std::set<std::pair<s32, s32>> edges;
    std::map<s32, s32> destinationCount;
    std::map<s32, s32> returnCount;

    for (const auto& [regionId, region] : Rando::Logic::Regions) {
        for (const auto& [destination, regionExit] : region.exits) {
            s32 returnEntrance = regionExit.returnEntrance;
            if (returnEntrance == ONE_WAY_EXIT || IsGrottoEntrance(destination) || IsGrottoEntrance(returnEntrance)) {
                continue;
            }
            edges.insert({ destination, returnEntrance });
            destinationCount[destination]++;
            returnCount[returnEntrance]++;
        }
    }

    auto isUnique = [&](s32 entrance) { return destinationCount[entrance] == 1 && returnCount[entrance] == 1; };
    auto isReserved = [&](s32 entrance) {
        return interiorEntrances.count(entrance) || dungeonEntrances.count(entrance);
    };

    std::vector<EntrancePair> overworldEntrances;

    for (const auto& [destination, returnEntrance] : edges) {
        // Every connection appears as an edge in both directions; keep one orientation of each.
        if (destination > returnEntrance) {
            continue;
        }
        if (isReserved(destination) || isReserved(returnEntrance)) {
            continue;
        }
        if (IsComplexInternalEntrance(destination) && IsComplexInternalEntrance(returnEntrance)) {
            continue;
        }
        if (IsDungeonSceneEntrance(destination) || IsDungeonSceneEntrance(returnEntrance)) {
            continue;
        }
        // Skip transitions within a single scene (caves, internal doors) - they aren't area links.
        if (SceneOf(destination) == SceneOf(returnEntrance)) {
            continue;
        }
        // Only keep entrances that are unambiguous on both sides and genuinely bidirectional.
        if (!isUnique(destination) || !isUnique(returnEntrance) || !edges.count({ returnEntrance, destination })) {
            continue;
        }
        overworldEntrances.push_back({ returnEntrance, destination });
    }

    return overworldEntrances;
}

bool IsEntranceShuffleEnabled() {
    return IS_RANDO &&
           (RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENTRANCES_INTERIORS] || RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENTRANCES_DUNGEONS] ||
            RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENTRANCES_OVERWORLD]);
}

s32 GetStartEntrance() {
    return GetShuffledEntrance(ENTRANCE(SOUTH_CLOCK_TOWN, 0));
}

s32 ResolveExit(RandoRegionId fromRegion, s32 exitId, s32 returnEntrance) {
    if (!IsEntranceShuffleEnabled()) {
        return exitId;
    }

    if (fromRegion == RR_MAX) {
        return (exitId == ENTRANCE(SOUTH_CLOCK_TOWN, 0)) ? GetStartEntrance() : exitId;
    }

    if (IsGrottoEntrance(returnEntrance)) {
        return exitId;
    }

    return GetShuffledEntrance(exitId);
}

// Structural reachability from the start under the current shuffle map: the save warp plus every
// two-way door and in-world one-way, ignoring item/time gates. Owl warps are excluded because they
// require activating the owl on foot first, which never happens during generation.
std::set<RandoRegionId> ReachableRegions() {
    std::set<RandoRegionId> reachable;
    std::vector<RandoRegionId> frontier = { RR_MAX };
    while (!frontier.empty()) {
        RandoRegionId regionId = frontier.back();
        frontier.pop_back();
        if (!reachable.insert(regionId).second) {
            continue;
        }
        if (regionId == RR_MAX) { // only the save warp
            frontier.push_back(Rando::Logic::GetRegionIdFromEntrance(GetStartEntrance()));
            continue;
        }
        for (auto& [connectedRegion, condition] : Rando::Logic::Regions[regionId].connections) {
            frontier.push_back(connectedRegion);
        }
        for (auto& [exitId, regionExit] : Rando::Logic::Regions[regionId].exits) {
            frontier.push_back(
                Rando::Logic::GetRegionIdFromEntrance(ResolveExit(regionId, exitId, regionExit.returnEntrance)));
        }
    }
    return reachable;
}

bool AllRegionsReachable() {
    return ReachableRegions().size() == Rando::Logic::Regions.size();
}

// Randomly reconnect every pair in each pool (shuffle + positional coupling) into sEntranceMap.
// Assumes the RNG is already seeded.
void ApplyEntranceShuffle(const std::vector<std::vector<EntrancePair>>& pools) {
    for (const auto& originalPool : pools) {
        auto pool = originalPool;

        if (pool.size() < 2) {
            continue;
        }

        for (size_t i = 0; i < pool.size(); i++) {
            size_t j = Ship_Random(0, pool.size() - 1);
            std::swap(pool[i], pool[j]);
        }

        for (size_t i = 0; i < pool.size(); ++i) {
            const auto& from = originalPool[i];
            const auto& to = pool[i];

            if (from.entrance != to.entrance) {
                sEntranceMap[from.entrance] = to.entrance;
            }

            if (to.exit != from.exit) {
                sEntranceMap[to.exit] = from.exit;
            }
        }
    }
}

void ShuffleEntrances() {
    sEntranceMap.clear();

    if (!IsEntranceShuffleEnabled()) {
        return;
    }

    std::vector<std::vector<EntrancePair>> pools;
    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENTRANCES_INTERIORS]) {
        pools.push_back(ConvertSetToEntrancePairs(interiorEntrances));
    }
    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENTRANCES_DUNGEONS]) {
        pools.push_back(ConvertSetToEntrancePairs(dungeonEntrances));
    }
    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_ENTRANCES_OVERWORLD]) {
        pools.push_back(GetOverworldEntrances());
    }

    // A single random layout islands some region off from the start most of the time, so reshuffle
    // with a fresh sub-seed until every region is reachable. The same finalSeed always converges on
    // the same accepted layout, so file load reproduces exactly what generation produced.
    constexpr uint32_t maxAttempts = 256;
    uint32_t baseSeed = gSaveContext.save.shipSaveInfo.rando.finalSeed;
    for (uint32_t attempt = 0; attempt < maxAttempts; attempt++) {
        sEntranceMap.clear();
        Ship_Random_Seed(baseSeed + attempt);
        ApplyEntranceShuffle(pools);
        if (AllRegionsReachable()) {
            return;
        }
    }

    SPDLOG_WARN("Entrance shuffle: no fully-connected layout found after {} attempts", maxAttempts);
}

s32 GetShuffledEntrance(s32 originalEntrance) {
    // The map is empty when shuffle is disabled, making this the identity.
    auto it = sEntranceMap.find(originalEntrance);
    return it != sEntranceMap.end() ? it->second : originalEntrance;
}

} // namespace EntranceShuffle

} // namespace Rando
