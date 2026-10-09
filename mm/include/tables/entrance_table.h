#pragma once

/**
 * Majora's Mask Complete Entrance Table
 * 
 * Entrance IDs are composite 16-bit values:
 *   (External Scene Index << 9) | (Spawn Index << 4) | Offset
 * 
 * This table includes ALL valid spawn indices for every non-empty External Scene.
 * Naming Convention: ENTRANCE_[LOCATION]_[SPAWN_INDEX]
 * 
 * WARNING: Do not attempt to use a spawn index higher than the maximum listed 
 * for a scene, as it will result in an out-of-bounds memory read and a crash.
 */

// ========================================================================
// CLOCK TOWN & SURROUNDINGS
// ========================================================================
// Mayor's Residence (Ext: 0x00, Ent: 3)
#define ENTRANCE_MAYORS_RESIDENCE_0     0x0000
#define ENTRANCE_MAYORS_RESIDENCE_1     0x0010
//#define ENTRANCE_MAYORS_RESIDENCE_2     0x0020 Crashes

// East Clock Town (Ext: 0x69, Ent: 13)
#define ENTRANCE_EAST_CLOCK_TOWN_0      0xD200
#define ENTRANCE_EAST_CLOCK_TOWN_1      0xD210
#define ENTRANCE_EAST_CLOCK_TOWN_2      0xD220
#define ENTRANCE_EAST_CLOCK_TOWN_3      0xD230
#define ENTRANCE_EAST_CLOCK_TOWN_4      0xD240
#define ENTRANCE_EAST_CLOCK_TOWN_5      0xD250
#define ENTRANCE_EAST_CLOCK_TOWN_6      0xD260
#define ENTRANCE_EAST_CLOCK_TOWN_7      0xD270
#define ENTRANCE_EAST_CLOCK_TOWN_8      0xD280
#define ENTRANCE_EAST_CLOCK_TOWN_9      0xD290
#define ENTRANCE_EAST_CLOCK_TOWN_10     0xD2A0
#define ENTRANCE_EAST_CLOCK_TOWN_11     0xD2B0
//#define ENTRANCE_EAST_CLOCK_TOWN_12     0xD2C0 Crashes

// West Clock Town (Ext: 0x6A, Ent: 10)
#define ENTRANCE_WEST_CLOCK_TOWN_0      0xD400
#define ENTRANCE_WEST_CLOCK_TOWN_1      0xD410
#define ENTRANCE_WEST_CLOCK_TOWN_2      0xD420
#define ENTRANCE_WEST_CLOCK_TOWN_3      0xD430
#define ENTRANCE_WEST_CLOCK_TOWN_4      0xD440
#define ENTRANCE_WEST_CLOCK_TOWN_5      0xD450
#define ENTRANCE_WEST_CLOCK_TOWN_6      0xD460
#define ENTRANCE_WEST_CLOCK_TOWN_7      0xD470
#define ENTRANCE_WEST_CLOCK_TOWN_8      0xD480
#define ENTRANCE_WEST_CLOCK_TOWN_9      0xD490

// North Clock Town (Ext: 0x6B, Ent: 8)
#define ENTRANCE_NORTH_CLOCK_TOWN_0     0xD600
#define ENTRANCE_NORTH_CLOCK_TOWN_1     0xD610
#define ENTRANCE_NORTH_CLOCK_TOWN_2     0xD620
#define ENTRANCE_NORTH_CLOCK_TOWN_3     0xD630
#define ENTRANCE_NORTH_CLOCK_TOWN_4     0xD640
#define ENTRANCE_NORTH_CLOCK_TOWN_5     0xD650
#define ENTRANCE_NORTH_CLOCK_TOWN_6     0xD660
#define ENTRANCE_NORTH_CLOCK_TOWN_7     0xD670

// South Clock Town (Ext: 0x6C, Ent: 11)
#define ENTRANCE_SOUTH_CLOCK_TOWN_0     0xD800
#define ENTRANCE_SOUTH_CLOCK_TOWN_1     0xD810
#define ENTRANCE_SOUTH_CLOCK_TOWN_2     0xD820
#define ENTRANCE_SOUTH_CLOCK_TOWN_3     0xD830
#define ENTRANCE_SOUTH_CLOCK_TOWN_4     0xD840
#define ENTRANCE_SOUTH_CLOCK_TOWN_5     0xD850
#define ENTRANCE_SOUTH_CLOCK_TOWN_6     0xD860
#define ENTRANCE_SOUTH_CLOCK_TOWN_7     0xD870
#define ENTRANCE_SOUTH_CLOCK_TOWN_8     0xD880
#define ENTRANCE_SOUTH_CLOCK_TOWN_9     0xD890
// #define ENTRANCE_SOUTH_CLOCK_TOWN_10    0xD8A0 Cutscene from first Song of Time use

// Laundry Pool (Ext: 0x6D, Ent: 3)
#define ENTRANCE_LAUNDRY_POOL_0         0xDA00
#define ENTRANCE_LAUNDRY_POOL_1         0xDA10
// #define ENTRANCE_LAUNDRY_POOL_2         0xDA20 Crashes

// Clock Tower Interior (Ext: 0x60, Ent: 7)
// #define ENTRANCE_CLOCK_TOWER_INTERIOR_0 0xC000 First cutscene entering Clock Tower
#define ENTRANCE_CLOCK_TOWER_INTERIOR_1 0xC010
// #define ENTRANCE_CLOCK_TOWER_INTERIOR_2 0xC020 // Song of healing Deku Mask cutscene
// #define ENTRANCE_CLOCK_TOWER_INTERIOR_3 0xC030 // Cutscene with mask salesman about time left to get MM
// #define ENTRANCE_CLOCK_TOWER_INTERIOR_4 0xC040 // Similar cutscene to first Song of Time use
#define ENTRANCE_CLOCK_TOWER_INTERIOR_5 0xC050
// #define ENTRANCE_CLOCK_TOWER_INTERIOR_6 0xC060 // Mask salesman explaining about the power of MM and the imp having it

// Clock Tower Rooftop (Ext: 0x16, Ent: 3)
#define ENTRANCE_CLOCK_TOWER_ROOFTOP_0  0x2C00
// #define ENTRANCE_CLOCK_TOWER_ROOFTOP_1  0x2C10 // After learning song of time from getting ocarina back cutscene
// #define ENTRANCE_CLOCK_TOWER_ROOFTOP_2  0x2C20 Crashes!

// ========================================================================
// TERMINA FIELD & OVERWORLD
// ========================================================================
// Termina Field (Ext: 0x2A, Ent: 15)
#define ENTRANCE_TERMINA_FIELD_0        0x5400
#define ENTRANCE_TERMINA_FIELD_1        0x5410
#define ENTRANCE_TERMINA_FIELD_2        0x5420
#define ENTRANCE_TERMINA_FIELD_3        0x5430
#define ENTRANCE_TERMINA_FIELD_4        0x5440
#define ENTRANCE_TERMINA_FIELD_5        0x5450
#define ENTRANCE_TERMINA_FIELD_6        0x5460
#define ENTRANCE_TERMINA_FIELD_7        0x5470
#define ENTRANCE_TERMINA_FIELD_8        0x5480
#define ENTRANCE_TERMINA_FIELD_9        0x5490
// #define ENTRANCE_TERMINA_FIELD_10       0x54A0 Telescope cutscene
#define ENTRANCE_TERMINA_FIELD_11       0x54B0
// #define ENTRANCE_TERMINA_FIELD_12       0x54C0 Mask Salesman cutscene?
#define ENTRANCE_TERMINA_FIELD_13       0x54D0
// #define ENTRANCE_TERMINA_FIELD_14       0x54E0 Drawing of Skull Kid on tree ending cutscene

// Road to Southern Swamp (Ext: 0x3D, Ent: 3)
#define ENTRANCE_ROAD_TO_SOUTHERN_SWAMP_0 0x7A00
#define ENTRANCE_ROAD_TO_SOUTHERN_SWAMP_1 0x7A10
#define ENTRANCE_ROAD_TO_SOUTHERN_SWAMP_2 0x7A20

// Road to Ikana (Ext: 0x50, Ent: 3)
#define ENTRANCE_ROAD_TO_IKANA_0        0xA000
#define ENTRANCE_ROAD_TO_IKANA_1        0xA010
#define ENTRANCE_ROAD_TO_IKANA_2        0xA020

// Milk Road (Ext: 0x1F, Ent: 7)
#define ENTRANCE_MILK_ROAD_0            0x3E00
#define ENTRANCE_MILK_ROAD_1            0x3E10
#define ENTRANCE_MILK_ROAD_2            0x3E20
#define ENTRANCE_MILK_ROAD_3            0x3E30
#define ENTRANCE_MILK_ROAD_4            0x3E40
#define ENTRANCE_MILK_ROAD_5            0x3E50
#define ENTRANCE_MILK_ROAD_6            0x3E60

// Romani Ranch (Ext: 0x32, Ent: 12)
#define ENTRANCE_ROMANI_RANCH_0         0x6400
#define ENTRANCE_ROMANI_RANCH_1         0x6410
#define ENTRANCE_ROMANI_RANCH_2         0x6420
#define ENTRANCE_ROMANI_RANCH_3         0x6430
#define ENTRANCE_ROMANI_RANCH_4         0x6440
#define ENTRANCE_ROMANI_RANCH_5         0x6450
#define ENTRANCE_ROMANI_RANCH_6         0x6460
// #define ENTRANCE_ROMANI_RANCH_7         0x6470 // Duplicate as above entrance?
#define ENTRANCE_ROMANI_RANCH_8         0x6480
#define ENTRANCE_ROMANI_RANCH_9         0x6490
#define ENTRANCE_ROMANI_RANCH_10        0x64A0
#define ENTRANCE_ROMANI_RANCH_11        0x64B0

// ========================================================================
// SWAMP REGION
// ========================================================================
// Southern Swamp - Clear (Ext: 0x06, Ent: 11)
#define ENTRANCE_SOUTHERN_SWAMP_CLEAR_0 0x0C00
#define ENTRANCE_SOUTHERN_SWAMP_CLEAR_1 0x0C10
#define ENTRANCE_SOUTHERN_SWAMP_CLEAR_2 0x0C20
#define ENTRANCE_SOUTHERN_SWAMP_CLEAR_3 0x0C30
#define ENTRANCE_SOUTHERN_SWAMP_CLEAR_4 0x0C40
#define ENTRANCE_SOUTHERN_SWAMP_CLEAR_5 0x0C50
// #define ENTRANCE_SOUTHERN_SWAMP_CLEAR_6 0x0C60 Boat ride at Swamp (cleared)
#define ENTRANCE_SOUTHERN_SWAMP_CLEAR_7 0x0C70
#define ENTRANCE_SOUTHERN_SWAMP_CLEAR_8 0x0C80
#define ENTRANCE_SOUTHERN_SWAMP_CLEAR_9 0x0C90
#define ENTRANCE_SOUTHERN_SWAMP_CLEAR_10 0x0CA0

// Southern Swamp - Poisoned (Ext: 0x42, Ent: 11)
#define ENTRANCE_SOUTHERN_SWAMP_POISON_0 0x8400
#define ENTRANCE_SOUTHERN_SWAMP_POISON_1 0x8410
#define ENTRANCE_SOUTHERN_SWAMP_POISON_2 0x8420
#define ENTRANCE_SOUTHERN_SWAMP_POISON_3 0x8430
#define ENTRANCE_SOUTHERN_SWAMP_POISON_4 0x8440
#define ENTRANCE_SOUTHERN_SWAMP_POISON_5 0x8450
// #define ENTRANCE_SOUTHERN_SWAMP_POISON_6 0x8460 Boat ride at Swamp (Poisoned)
#define ENTRANCE_SOUTHERN_SWAMP_POISON_7 0x8470
#define ENTRANCE_SOUTHERN_SWAMP_POISON_8 0x8480
#define ENTRANCE_SOUTHERN_SWAMP_POISON_9 0x8490
#define ENTRANCE_SOUTHERN_SWAMP_POISON_10 0x84A0

// Woodfall (Ext: 0x43, Ent: 5)
#define ENTRANCE_WOODFALL_0             0x8600
// #define ENTRANCE_WOODFALL_1             0x8610 Not a good spawn in uncleared WoodFall (above swamp water)
#define ENTRANCE_WOODFALL_2             0x8620
// #define ENTRANCE_WOODFALL_3             0x8630 Also not a good spawn in uncleared WoodFall (above swamp water)
#define ENTRANCE_WOODFALL_4             0x8640

// Woodfall Temple (Ext: 0x18, Ent: 3)
#define ENTRANCE_WOODFALL_TEMPLE_0      0x3000
#define ENTRANCE_WOODFALL_TEMPLE_1      0x3010
#define ENTRANCE_WOODFALL_TEMPLE_2      0x3020

// Deku Palace (Ext: 0x28, Ent: 11)
#define ENTRANCE_DEKU_PALACE_0          0x5000
#define ENTRANCE_DEKU_PALACE_1          0x5010
#define ENTRANCE_DEKU_PALACE_2          0x5020
#define ENTRANCE_DEKU_PALACE_3          0x5030
#define ENTRANCE_DEKU_PALACE_4          0x5040
#define ENTRANCE_DEKU_PALACE_5          0x5050
#define ENTRANCE_DEKU_PALACE_6          0x5060
#define ENTRANCE_DEKU_PALACE_7          0x5070
#define ENTRANCE_DEKU_PALACE_8          0x5080
#define ENTRANCE_DEKU_PALACE_9          0x5090
#define ENTRANCE_DEKU_PALACE_10         0x50A0

// Deku Scrub Playground (Ext: 0x1B, Ent: 2)
#define ENTRANCE_DEKU_SCRUB_PLAYGROUND_0 0x3600
#define ENTRANCE_DEKU_SCRUB_PLAYGROUND_1 0x3610

// Swamp Spider House (Ext: 0x24, Ent: 1)
#define ENTRANCE_SWAMP_SPIDER_HOUSE_0   0x4800

// Magic Hags' Potion Shop (Ext: 0x02, Ent: 1)
#define ENTRANCE_MAGIC_HAGS_POTION_SHOP_0 0x0400

// ========================================================================
// MOUNTAIN REGION
// ========================================================================
// Path to Mountain Village (Ext: 0x19, Ent: 2)
#define ENTRANCE_PATH_TO_MOUNTAIN_VILLAGE_0 0x3200
#define ENTRANCE_PATH_TO_MOUNTAIN_VILLAGE_1 0x3210

// Mountain Village - Winter (Ext: 0x4D, Ent: 9)
#define ENTRANCE_MOUNTAIN_VILLAGE_WINTER_0 0x9A00
#define ENTRANCE_MOUNTAIN_VILLAGE_WINTER_1 0x9A10
#define ENTRANCE_MOUNTAIN_VILLAGE_WINTER_2 0x9A20
#define ENTRANCE_MOUNTAIN_VILLAGE_WINTER_3 0x9A30
#define ENTRANCE_MOUNTAIN_VILLAGE_WINTER_4 0x9A40
#define ENTRANCE_MOUNTAIN_VILLAGE_WINTER_5 0x9A50
#define ENTRANCE_MOUNTAIN_VILLAGE_WINTER_6 0x9A60
#define ENTRANCE_MOUNTAIN_VILLAGE_WINTER_7 0x9A70
#define ENTRANCE_MOUNTAIN_VILLAGE_WINTER_8 0x9A80

// Mountain Village - Spring (Ext: 0x57, Ent: 9)
#define ENTRANCE_MOUNTAIN_VILLAGE_SPRING_0 0xAE00
#define ENTRANCE_MOUNTAIN_VILLAGE_SPRING_1 0xAE10
#define ENTRANCE_MOUNTAIN_VILLAGE_SPRING_2 0xAE20
#define ENTRANCE_MOUNTAIN_VILLAGE_SPRING_3 0xAE30
#define ENTRANCE_MOUNTAIN_VILLAGE_SPRING_4 0xAE40
#define ENTRANCE_MOUNTAIN_VILLAGE_SPRING_5 0xAE50
#define ENTRANCE_MOUNTAIN_VILLAGE_SPRING_6 0xAE60
#define ENTRANCE_MOUNTAIN_VILLAGE_SPRING_7 0xAE70
#define ENTRANCE_MOUNTAIN_VILLAGE_SPRING_8 0xAE80

// Snowhead (Ext: 0x59, Ent: 4)
#define ENTRANCE_SNOWHEAD_0             0xB200
#define ENTRANCE_SNOWHEAD_1             0xB210
#define ENTRANCE_SNOWHEAD_2             0xB220
#define ENTRANCE_SNOWHEAD_3             0xB230

// Path to Snowhead (Ext: 0x58, Ent: 2)
#define ENTRANCE_PATH_TO_SNOWHEAD_0     0xB000
#define ENTRANCE_PATH_TO_SNOWHEAD_1     0xB010

// Snowhead Temple (Ext: 0x1E, Ent: 2)
#define ENTRANCE_SNOWHEAD_TEMPLE_0      0x3C00
#define ENTRANCE_SNOWHEAD_TEMPLE_1      0x3C10

// Goron Village - Winter (Ext: 0x4A, Ent: 5)
#define ENTRANCE_GORON_VILLAGE_WINTER_0 0x9400
#define ENTRANCE_GORON_VILLAGE_WINTER_1 0x9410
#define ENTRANCE_GORON_VILLAGE_WINTER_2 0x9420
#define ENTRANCE_GORON_VILLAGE_WINTER_3 0x9430
#define ENTRANCE_GORON_VILLAGE_WINTER_4 0x9440

// Goron Village - Spring (Ext: 0x45, Ent: 5)
#define ENTRANCE_GORON_VILLAGE_SPRING_0 0x8A00
#define ENTRANCE_GORON_VILLAGE_SPRING_1 0x8A10
#define ENTRANCE_GORON_VILLAGE_SPRING_2 0x8A20
// #define ENTRANCE_GORON_VILLAGE_SPRING_3 0x8A30 Lens of Truth Shrine spawn in spring except, there's no shrine lol
#define ENTRANCE_GORON_VILLAGE_SPRING_4 0x8A40

// Goron Shrine (Ext: 0x2F, Ent: 4)
#define ENTRANCE_GORON_SHRINE_0         0x5E00
#define ENTRANCE_GORON_SHRINE_1         0x5E10
// #define ENTRANCE_GORON_SHRINE_2         0x5E20 Learn full Goron Lullaby cutscene
#define ENTRANCE_GORON_SHRINE_3         0x5E30

// Goron Graveyard (Ext: 0x4B, Ent: 2)
#define ENTRANCE_GORON_GRAVEYARD_0      0x9600
// #define ENTRANCE_GORON_GRAVEYARD_1      0x9610 //Song of healing Goron Mask cutscene

// Mountain Smithy (Ext: 0x29, Ent: 1)
#define ENTRANCE_MOUNTAIN_SMITHY_0      0x5200

// ========================================================================
// OCEAN / GREAT BAY REGION
// ========================================================================
// Great Bay Coast (Ext: 0x34, Ent: 14)
#define ENTRANCE_GREAT_BAY_COAST_0      0x6800
#define ENTRANCE_GREAT_BAY_COAST_1      0x6810
#define ENTRANCE_GREAT_BAY_COAST_2      0x6820
#define ENTRANCE_GREAT_BAY_COAST_3      0x6830
#define ENTRANCE_GREAT_BAY_COAST_4      0x6840
#define ENTRANCE_GREAT_BAY_COAST_5      0x6850
#define ENTRANCE_GREAT_BAY_COAST_6      0x6860
#define ENTRANCE_GREAT_BAY_COAST_7      0x6870
#define ENTRANCE_GREAT_BAY_COAST_8      0x6880
// #define ENTRANCE_GREAT_BAY_COAST_9      0x6890 Zora's Mask Song of Healing cutscene
#define ENTRANCE_GREAT_BAY_COAST_10     0x68A0
#define ENTRANCE_GREAT_BAY_COAST_11     0x68B0
#define ENTRANCE_GREAT_BAY_COAST_12     0x68C0
#define ENTRANCE_GREAT_BAY_COAST_13     0x68D0

// Zora Cape (Ext: 0x35, Ent: 10)
#define ENTRANCE_ZORA_CAPE_0            0x6A00
#define ENTRANCE_ZORA_CAPE_1            0x6A10
#define ENTRANCE_ZORA_CAPE_2            0x6A20
#define ENTRANCE_ZORA_CAPE_3            0x6A30
#define ENTRANCE_ZORA_CAPE_4            0x6A40
#define ENTRANCE_ZORA_CAPE_5            0x6A50
#define ENTRANCE_ZORA_CAPE_6            0x6A60
#define ENTRANCE_ZORA_CAPE_7            0x6A70
#define ENTRANCE_ZORA_CAPE_8            0x6A80
#define ENTRANCE_ZORA_CAPE_9            0x6A90

// Pirates' Fortress (Ext: 0x11, Ent: 15)
#define ENTRANCE_PIRATES_FORTRESS_0     0x2200
#define ENTRANCE_PIRATES_FORTRESS_1     0x2210
#define ENTRANCE_PIRATES_FORTRESS_2     0x2220
#define ENTRANCE_PIRATES_FORTRESS_3     0x2230
#define ENTRANCE_PIRATES_FORTRESS_4     0x2240
#define ENTRANCE_PIRATES_FORTRESS_5     0x2250
#define ENTRANCE_PIRATES_FORTRESS_6     0x2260
#define ENTRANCE_PIRATES_FORTRESS_7     0x2270
#define ENTRANCE_PIRATES_FORTRESS_8     0x2280
#define ENTRANCE_PIRATES_FORTRESS_9     0x2290
// #define ENTRANCE_PIRATES_FORTRESS_10    0x22A0 Pirate Telescope
// #define ENTRANCE_PIRATES_FORTRESS_11    0x22B0 Void out?
#define ENTRANCE_PIRATES_FORTRESS_12    0x22C0
#define ENTRANCE_PIRATES_FORTRESS_13    0x22D0
// #define ENTRANCE_PIRATES_FORTRESS_14    0x22E0 Crashes

// Pirates' Fortress Interior (Ext: 0x20, Ent: 16)
#define ENTRANCE_PIRATES_FORTRESS_INTERIOR_0 0x4000
#define ENTRANCE_PIRATES_FORTRESS_INTERIOR_1 0x4010
#define ENTRANCE_PIRATES_FORTRESS_INTERIOR_2 0x4020
#define ENTRANCE_PIRATES_FORTRESS_INTERIOR_3 0x4030
#define ENTRANCE_PIRATES_FORTRESS_INTERIOR_4 0x4040
#define ENTRANCE_PIRATES_FORTRESS_INTERIOR_5 0x4050
#define ENTRANCE_PIRATES_FORTRESS_INTERIOR_6 0x4060
#define ENTRANCE_PIRATES_FORTRESS_INTERIOR_7 0x4070
#define ENTRANCE_PIRATES_FORTRESS_INTERIOR_8 0x4080
#define ENTRANCE_PIRATES_FORTRESS_INTERIOR_9 0x4090
#define ENTRANCE_PIRATES_FORTRESS_INTERIOR_10 0x40A0
// #define ENTRANCE_PIRATES_FORTRESS_INTERIOR_11 0x40B0 Out of bounds pirate fight?
// #define ENTRANCE_PIRATES_FORTRESS_INTERIOR_12 0x40C0 CRASH
// #define ENTRANCE_PIRATES_FORTRESS_INTERIOR_13 0x40D0 CRASH
// #define ENTRANCE_PIRATES_FORTRESS_INTERIOR_14 0x40E0 CRASH
// #define ENTRANCE_PIRATES_FORTRESS_INTERIOR_15 0x40F0 Out of bounds pirate fight 2?

// Pirates' Fortress Exterior (Ext: 0x38, Ent: 7)
#define ENTRANCE_PIRATES_FORTRESS_EXTERIOR_0 0x7000
#define ENTRANCE_PIRATES_FORTRESS_EXTERIOR_1 0x7010
#define ENTRANCE_PIRATES_FORTRESS_EXTERIOR_2 0x7020
#define ENTRANCE_PIRATES_FORTRESS_EXTERIOR_3 0x7030
#define ENTRANCE_PIRATES_FORTRESS_EXTERIOR_4 0x7040
#define ENTRANCE_PIRATES_FORTRESS_EXTERIOR_5 0x7050
#define ENTRANCE_PIRATES_FORTRESS_EXTERIOR_6 0x7060

// Pinnacle Rock (Ext: 0x22, Ent: 2)
#define ENTRANCE_PINNACLE_ROCK_0        0x4400
#define ENTRANCE_PINNACLE_ROCK_1        0x4410

// Great Bay Temple (Ext: 0x46, Ent: 3)
#define ENTRANCE_GREAT_BAY_TEMPLE_0     0x8C00
// #define ENTRANCE_GREAT_BAY_TEMPLE_1     0x8C10 first entrance with turtle cutscene
// #define ENTRANCE_GREAT_BAY_TEMPLE_2     0x8C20 Crash

// Zora Hall (Ext: 0x30, Ent: 9)
#define ENTRANCE_ZORA_HALL_0            0x6000
#define ENTRANCE_ZORA_HALL_1            0x6010
#define ENTRANCE_ZORA_HALL_2            0x6020
#define ENTRANCE_ZORA_HALL_3            0x6030
#define ENTRANCE_ZORA_HALL_4            0x6040
#define ENTRANCE_ZORA_HALL_5            0x6050
#define ENTRANCE_ZORA_HALL_6            0x6060
#define ENTRANCE_ZORA_HALL_7            0x6070
#define ENTRANCE_ZORA_HALL_8            0x6080

// Zora Hall Rooms (Ext: 0x49, Ent: 7)
#define ENTRANCE_ZORA_HALL_ROOMS_0      0x9200
#define ENTRANCE_ZORA_HALL_ROOMS_1      0x9210
#define ENTRANCE_ZORA_HALL_ROOMS_2      0x9220
#define ENTRANCE_ZORA_HALL_ROOMS_3      0x9230
// #define ENTRANCE_ZORA_HALL_ROOMS_4      0x9240 Jamming with zora cutscene Bassist?
#define ENTRANCE_ZORA_HALL_ROOMS_5      0x9250
// #define ENTRANCE_ZORA_HALL_ROOMS_6      0x9260 Jamming with Keyboard player?

// Marine Research Lab (Ext: 0x2C, Ent: 1)
#define ENTRANCE_MARINE_RESEARCH_LAB_0  0x5800

// Oceanside Spider House (Ext: 0x25, Ent: 1)
#define ENTRANCE_OCEANSIDE_SPIDER_HOUSE_0 0x4A00

// ========================================================================
// IKANA CANYON REGION
// ========================================================================
// Ikana Canyon (Ext: 0x10, Ent: 16)
#define ENTRANCE_IKANA_CANYON_0         0x2000
#define ENTRANCE_IKANA_CANYON_1         0x2010
#define ENTRANCE_IKANA_CANYON_2         0x2020
#define ENTRANCE_IKANA_CANYON_3         0x2030
#define ENTRANCE_IKANA_CANYON_4         0x2040
#define ENTRANCE_IKANA_CANYON_5         0x2050
#define ENTRANCE_IKANA_CANYON_6         0x2060
#define ENTRANCE_IKANA_CANYON_7         0x2070
#define ENTRANCE_IKANA_CANYON_8         0x2080
// #define ENTRANCE_IKANA_CANYON_9         0x2090 Song of storms cave cutscene and music playing from house
// #define ENTRANCE_IKANA_CANYON_10        0x20A0 Inside cave with Sharp cutscene after using song of storms
#define ENTRANCE_IKANA_CANYON_11        0x20B0
#define ENTRANCE_IKANA_CANYON_12        0x20C0
#define ENTRANCE_IKANA_CANYON_13        0x20D0
#define ENTRANCE_IKANA_CANYON_14        0x20E0
#define ENTRANCE_IKANA_CANYON_15        0x20F0

// Ikana Graveyard (Ext: 0x40, Ent: 6)
#define ENTRANCE_IKANA_GRAVEYARD_0      0x8000
#define ENTRANCE_IKANA_GRAVEYARD_1      0x8010
#define ENTRANCE_IKANA_GRAVEYARD_2      0x8020
#define ENTRANCE_IKANA_GRAVEYARD_3      0x8030
#define ENTRANCE_IKANA_GRAVEYARD_4      0x8040
#define ENTRANCE_IKANA_GRAVEYARD_5      0x8050

// Ancient Castle of Ikana (Ext: 0x1A, Ent: 7)
#define ENTRANCE_ANCIENT_CASTLE_IKANA_0 0x3400
#define ENTRANCE_ANCIENT_CASTLE_IKANA_1 0x3410
#define ENTRANCE_ANCIENT_CASTLE_IKANA_2 0x3420
#define ENTRANCE_ANCIENT_CASTLE_IKANA_3 0x3430
#define ENTRANCE_ANCIENT_CASTLE_IKANA_4 0x3440
#define ENTRANCE_ANCIENT_CASTLE_IKANA_5 0x3450
#define ENTRANCE_ANCIENT_CASTLE_IKANA_6 0x3460

// Beneath the Well (Ext: 0x48, Ent: 2)
#define ENTRANCE_BENEATH_THE_WELL_0     0x9000
#define ENTRANCE_BENEATH_THE_WELL_1     0x9010

// Secret Shrine (Ext: 0x5D, Ent: 1)
#define ENTRANCE_SECRET_SHRINE_0        0xBA00

// Sakon's Hideout (Ext: 0x4C, Ent: 1)
#define ENTRANCE_SAKONS_HIDEOUT_0       0x9800

// Swordsman's School (Ext: 0x51, Ent: 1)
#define ENTRANCE_SWORDSMANS_SCHOOL_0    0xA200

// Ghost Hut (Ext: 0x4E, Ent: 3)
#define ENTRANCE_GHOST_HUT_0            0x9C00
#define ENTRANCE_GHOST_HUT_1            0x9C10
#define ENTRANCE_GHOST_HUT_2            0x9C20

// Music Box House (Ext: 0x52, Ent: 1)
#define ENTRANCE_MUSIC_BOX_HOUSE_0      0xA400

// Igos du Ikana's Lair (Ext: 0x53, Ent: 1)
#define ENTRANCE_IGOS_DU_IKANAS_LAIR_0  0xA600

// Deku Shrine (Ext: 0x4F, Ent: 3)
#define ENTRANCE_DEKU_SHRINE_0          0x9E00
#define ENTRANCE_DEKU_SHRINE_1          0x9E10
// #define ENTRANCE_DEKU_SHRINE_2          0x9E20 Black screen?

// ========================================================================
// SHOPS & INDOOR LOCATIONS
// ========================================================================
// Bomb Shop (Ext: 0x65, Ent: 2)
#define ENTRANCE_BOMB_SHOP_0            0xCA00
#define ENTRANCE_BOMB_SHOP_1            0xCA10

// Milk Bar (Ext: 0x12, Ent: 1)
#define ENTRANCE_MILK_BAR_0             0x2400

// Trading Post (Ext: 0x31, Ent: 2)
#define ENTRANCE_TRADING_POST_0         0x6200
#define ENTRANCE_TRADING_POST_1         0x6210

// Lottery Shop (Ext: 0x36, Ent: 1)
#define ENTRANCE_LOTTERY_SHOP_0         0x6C00

// Tourist Information (Ext: 0x54, Ent: 3)
#define ENTRANCE_TOURIST_INFORMATION_0  0xA800
#define ENTRANCE_TOURIST_INFORMATION_1  0xA810
#define ENTRANCE_TOURIST_INFORMATION_2  0xA820

// Treasure Chest Shop (Ext: 0x14, Ent: 2)
#define ENTRANCE_TREASURE_CHEST_SHOP_0  0x2800
#define ENTRANCE_TREASURE_CHEST_SHOP_1  0x2810

// Curiosity Shop (Ext: 0x07, Ent: 4)
#define ENTRANCE_CURIOSITY_SHOP_0       0x0E00
#define ENTRANCE_CURIOSITY_SHOP_1       0x0E10
// #define ENTRANCE_CURIOSITY_SHOP_2       0x0E20 Peeking into back of curiosity shop
#define ENTRANCE_CURIOSITY_SHOP_3       0x0E30

// Stock Pot Inn (Ext: 0x5E, Ent: 6)
#define ENTRANCE_STOCK_POT_INN_0        0xBC00
#define ENTRANCE_STOCK_POT_INN_1        0xBC10
#define ENTRANCE_STOCK_POT_INN_2        0xBC20
#define ENTRANCE_STOCK_POT_INN_3        0xBC30
// #define ENTRANCE_STOCK_POT_INN_4        0xBC40 ANJU and family cutscene about Kafei running off
#define ENTRANCE_STOCK_POT_INN_5        0xBC50

// Post Office (Ext: 0x2B, Ent: 1)
#define ENTRANCE_POST_OFFICE_0          0x5600

// Town Shooting Gallery (Ext: 0x1D, Ent: 2)
#define ENTRANCE_TOWN_SHOOTING_GALLERY_0 0x3A00
#define ENTRANCE_TOWN_SHOOTING_GALLERY_1 0x3A10

// Honey & Darling's Shop (Ext: 0x04, Ent: 1)
#define ENTRANCE_HONEY_AND_DARLINGS_0   0x0800

// Goron Shop (Ext: 0x3A, Ent: 1)
#define ENTRANCE_GORON_SHOP_0           0x7400

// Deku King's Chamber (Ext: 0x3B, Ent: 4)
#define ENTRANCE_DEKU_KINGS_CHAMBER_0   0x7600
#define ENTRANCE_DEKU_KINGS_CHAMBER_1   0x7610
// #define ENTRANCE_DEKU_KINGS_CHAMBER_2   0x7620 Deku Princess saves Monkey cutscene
#define ENTRANCE_DEKU_KINGS_CHAMBER_3   0x7630

// ========================================================================
// BOSSES & THE MOON
// ========================================================================
// Odolwa's Lair (Ext: 0x1C, Ent: 1)
#define ENTRANCE_ODOLWAS_LAIR_0         0x3800

// Goht's Lair (Ext: 0x41, Ent: 1)
#define ENTRANCE_GOHTS_LAIR_0           0x8200

// Gyorg's Lair (Ext: 0x5C, Ent: 2)
#define ENTRANCE_GYORGS_LAIR_0          0xB800
#define ENTRANCE_GYORGS_LAIR_1          0xB810

// Twinmold's Lair (Ext: 0x33, Ent: 1)
#define ENTRANCE_TWINMOLDS_LAIR_0       0x6600

// Majora's Lair (Ext: 0x01, Ent: 1)
#define ENTRANCE_MAJORAS_LAIR_0         0x0200

// The Moon (Ext: 0x64, Ent: 1)
#define ENTRANCE_THE_MOON_0             0xC800

// Moon Deku Trial (Ext: 0x27, Ent: 1)
#define ENTRANCE_MOON_DEKU_TRIAL_0      0x4E00

// Moon Goron Trial (Ext: 0x3C, Ent: 1)
#define ENTRANCE_MOON_GORON_TRIAL_0     0x7800

// Moon Zora Trial (Ext: 0x44, Ent: 2)
#define ENTRANCE_MOON_ZORA_TRIAL_0      0x8800
#define ENTRANCE_MOON_ZORA_TRIAL_1      0x8810

// Moon Link Trial (Ext: 0x63, Ent: 1)
#define ENTRANCE_MOON_LINK_TRIAL_0      0xC600

// Giants' Chamber (Ext: 0x66, Ent: 1)
#define ENTRANCE_GIANTS_CHAMBER_0       0xCC00

// ========================================================================
// MISC / GROTTOS / CUTSCENES
// ========================================================================
// Grottos (Ext: 0x0A, Ent: 17)
#define ENTRANCE_GROTTOS_0              0x1400
#define ENTRANCE_GROTTOS_1              0x1410
#define ENTRANCE_GROTTOS_2              0x1420
#define ENTRANCE_GROTTOS_3              0x1430
#define ENTRANCE_GROTTOS_4              0x1440
#define ENTRANCE_GROTTOS_5              0x1450
#define ENTRANCE_GROTTOS_6              0x1460
#define ENTRANCE_GROTTOS_7              0x1470
#define ENTRANCE_GROTTOS_8              0x1480
#define ENTRANCE_GROTTOS_9              0x1490
#define ENTRANCE_GROTTOS_10             0x14A0
#define ENTRANCE_GROTTOS_11             0x14B0
#define ENTRANCE_GROTTOS_12             0x14C0
#define ENTRANCE_GROTTOS_13             0x14D0
#define ENTRANCE_GROTTOS_14             0x14E0
#define ENTRANCE_GROTTOS_15             0x14F0
#define ENTRANCE_GROTTOS_16             0x1500

// Fairy's Fountain (Ext: 0x23, Ent: 10)
#define ENTRANCE_FAIRYS_FOUNTAIN_0      0x4600
#define ENTRANCE_FAIRYS_FOUNTAIN_1      0x4610
#define ENTRANCE_FAIRYS_FOUNTAIN_2      0x4620
#define ENTRANCE_FAIRYS_FOUNTAIN_3      0x4630
#define ENTRANCE_FAIRYS_FOUNTAIN_4      0x4640
// #define ENTRANCE_FAIRYS_FOUNTAIN_5      0x4650 Magic Power cutscene
// #define ENTRANCE_FAIRYS_FOUNTAIN_6      0x4660 mastered Spin Attack cutscene?
// #define ENTRANCE_FAIRYS_FOUNTAIN_7      0x4670 Magic Power Enhanced
// #define ENTRANCE_FAIRYS_FOUNTAIN_8      0x4680 Double Defense
// #define ENTRANCE_FAIRYS_FOUNTAIN_9      0x4690 Great Fairy's Sword

// Astral Observatory (Ext: 0x26, Ent: 3)
#define ENTRANCE_ASTRAL_OBSERVATORY_0   0x4C00
#define ENTRANCE_ASTRAL_OBSERVATORY_1   0x4C10
#define ENTRANCE_ASTRAL_OBSERVATORY_2   0x4C20

// Cutscene Map (Ext: 0x0E, Ent: 10)
#define ENTRANCE_CUTSCENE_MAP_0         0x1C00
#define ENTRANCE_CUTSCENE_MAP_1         0x1C10
// #define ENTRANCE_CUTSCENE_MAP_2         0x1C20 CRASH
#define ENTRANCE_CUTSCENE_MAP_3         0x1C30
// #define ENTRANCE_CUTSCENE_MAP_4         0x1C40 CRASH
// #define ENTRANCE_CUTSCENE_MAP_5         0x1C50 CRASH
// #define ENTRANCE_CUTSCENE_MAP_6         0x1C60 CRASH
// #define ENTRANCE_CUTSCENE_MAP_7         0x1C70 CRASH
#define ENTRANCE_CUTSCENE_MAP_8         0x1C80
#define ENTRANCE_CUTSCENE_MAP_9         0x1C90

// Before the Portal to Termina (Ext: 0x17, Ent: 5)
// #define ENTRANCE_OPENING_DUNGEON_0      0x2E00 Falling into Termina cutscene
#define ENTRANCE_OPENING_DUNGEON_1      0x2E10
// #define ENTRANCE_OPENING_DUNGEON_2      0x2E20 Turned to Deku Scrub cutscene
#define ENTRANCE_OPENING_DUNGEON_3      0x2E30
// #define ENTRANCE_OPENING_DUNGEON_4      0x2E40 First Song of Time played cutscene

// Lost Woods (Ext: 0x62, Ent: 3)
// #define ENTRANCE_LOST_WOODS_0           0xC400 Dragged by horse and skull kid
#define ENTRANCE_LOST_WOODS_1           0xC410 
#define ENTRANCE_LOST_WOODS_2           0xC420

// Woods of Mystery (Ext: 0x61, Ent: 1)
#define ENTRANCE_WOODS_OF_MYSTERY_0     0xC200

// Ranch House & Barn (Ext: 0x03, Ent: 2)
#define ENTRANCE_RANCH_HOUSE_0          0x0600
#define ENTRANCE_RANCH_HOUSE_1          0x0610

// Beneath the Graveyard (Ext: 0x05, Ent: 2)
#define ENTRANCE_BENEATH_THE_GRAVEYARD_0 0x0A00
#define ENTRANCE_BENEATH_THE_GRAVEYARD_1 0x0A10

// Dampe's House (Ext: 0x2D, Ent: 2)
#define ENTRANCE_DAMPES_HOUSE_0         0x5A00
#define ENTRANCE_DAMPES_HOUSE_1         0x5A10

// Fisherman's Hut (Ext: 0x39, Ent: 1)
#define ENTRANCE_FISHERMANS_HUT_0       0x7200

// Waterfall Rapids (Ext: 0x47, Ent: 4)
#define ENTRANCE_WATERFALL_RAPIDS_0     0x8E00
// #define ENTRANCE_WATERFALL_RAPIDS_1     0x8E10 Beaver Race
// #define ENTRANCE_WATERFALL_RAPIDS_2     0x8E20 End of beaver race, you didn't get all the rings and cheated?
#define ENTRANCE_WATERFALL_RAPIDS_3     0x8E30

// Doggy Racetrack (Ext: 0x3E, Ent: 2)
#define ENTRANCE_DOGGY_RACETRACK_0      0x7C00
#define ENTRANCE_DOGGY_RACETRACK_1      0x7C10

// Cucco Shack (Ext: 0x3F, Ent: 2)
#define ENTRANCE_CUCCO_SHACK_0          0x7E00
// #define ENTRANCE_CUCCO_SHACK_1          0x7E10 After marching with cuccos

// Stone Tower (Ext: 0x55, Ent: 4)
#define ENTRANCE_STONE_TOWER_0          0xAA00
#define ENTRANCE_STONE_TOWER_1          0xAA10
#define ENTRANCE_STONE_TOWER_2          0xAA20
#define ENTRANCE_STONE_TOWER_3          0xAA30

// Stone Tower - Inverted (Ext: 0x56, Ent: 2)
#define ENTRANCE_STONE_TOWER_INVERTED_0 0xAC00
#define ENTRANCE_STONE_TOWER_INVERTED_1 0xAC10

// Stone Tower Temple (Ext: 0x13, Ent: 2)
#define ENTRANCE_STONE_TOWER_TEMPLE_0   0x2600
#define ENTRANCE_STONE_TOWER_TEMPLE_1   0x2610

// Stone Tower Temple - Inverted (Ext: 0x15, Ent: 3)
#define ENTRANCE_STONE_TOWER_TEMPLE_INVERTED_0 0x2A00
#define ENTRANCE_STONE_TOWER_TEMPLE_INVERTED_1 0x2A10
// #define ENTRANCE_STONE_TOWER_TEMPLE_INVERTED_2 0x2A20 CRASH

// Path to Goron Village - Winter (Ext: 0x5A, Ent: 3)
#define ENTRANCE_PATH_TO_GORON_VILLAGE_WINTER_0 0xB400
#define ENTRANCE_PATH_TO_GORON_VILLAGE_WINTER_1 0xB410
#define ENTRANCE_PATH_TO_GORON_VILLAGE_WINTER_2 0xB420

// Path to Goron Village - Spring (Ext: 0x5B, Ent: 3)
#define ENTRANCE_PATH_TO_GORON_VILLAGE_SPRING_0 0xB600
#define ENTRANCE_PATH_TO_GORON_VILLAGE_SPRING_1 0xB610
#define ENTRANCE_PATH_TO_GORON_VILLAGE_SPRING_2 0xB620

// Goron Racetrack (Ext: 0x68, Ent: 3)
#define ENTRANCE_GORON_RACETRACK_0      0xD000
// #define ENTRANCE_GORON_RACETRACK_1      0xD010 Goron Race start cutscene
#define ENTRANCE_GORON_RACETRACK_2      0xD020

// Gorman Track (Ext: 0x67, Ent: 6)
#define ENTRANCE_GORMAN_TRACK_0         0xCE00
#define ENTRANCE_GORMAN_TRACK_1         0xCE10
#define ENTRANCE_GORMAN_TRACK_2         0xCE20
#define ENTRANCE_GORMAN_TRACK_3         0xCE30
#define ENTRANCE_GORMAN_TRACK_4         0xCE40
#define ENTRANCE_GORMAN_TRACK_5         0xCE50

// Great Bay Cutscene (Ext: 0x5F, Ent: 1)
// #define ENTRANCE_GREAT_BAY_CUTSCENE_0   0xBE00 Pirates sail into the storm

// Swamp Shooting Gallery (Ext: 0x21, Ent: 1)
#define ENTRANCE_SWAMP_SHOOTING_GALLERY_0 0x4200
