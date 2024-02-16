#include "global.h"
#include "data.h"
#include "event_scripts.h"
#include "follower_helper.h"
#include "constants/battle.h"
#include "constants/metatile_behaviors.h"
#include "constants/pokemon.h"
#include "constants/region_map_sections.h"
#include "constants/songs.h"
#include "constants/weather.h"

#define TYPE_NOT_TYPE1 NUMBER_OF_MON_TYPES

// difficult conditional messages follow
static const u8 sCondMsg00[] = _("{STR_VAR_1} danced happily.");
static const u8 sCondMsg01[] = _("{STR_VAR_1} danced beautifully.");
static const u8* const sCelebiTexts[] = {sCondMsg00, sCondMsg01, NULL};
static const u8 sCondMsg02[] = _("{STR_VAR_1} emitted fire and shouted.");
static const u8 sCondMsg03[] = _("{STR_VAR_1} is vigorously producing\nfire!");
static const u8 sCondMsg04[] = _("{STR_VAR_1} spat fire!");
static const u8 sCondMsg05[] = _("{STR_VAR_1} is vigorously breathing\nfire!");
static const u8* const sFireTexts[] = {sCondMsg02, sCondMsg03, sCondMsg04, sCondMsg05, NULL};
static const u8 sCondMsg06[] = _("{STR_VAR_1} is staring straight at\nthe POKéMON LEAGUE.");
static const u8 sCondMsg07[] = _("Your POKéMON is staring intently at\nthe mountain peak.");
static const u8 sCondMsg08[] = _("Sniff sniff, something smells good!");
static const u8 sCondMsg09[] = _("Your POKéMON is surveying the\nshelves restlessly.");
static const u8 sCondMsg10[] = _("{STR_VAR_1} is staring intently at\nthe shelves.");
static const u8* const sShopTexts[] = {sCondMsg09, sCondMsg10, NULL};
static const u8 sCondMsg11[] = _("{STR_VAR_1} focused with a sharp\ngaze!");
static const u8 sCondMsg12[] = _("{STR_VAR_1} seems very interested in\nthe bicycles.");
static const u8 sCondMsg13[] = _("{STR_VAR_1} seems to want to touch\nthe machines!");
static const u8 sCondMsg14[] = _("{STR_VAR_1} is swaying with the\nboat!");
static const u8 sCondMsg15[] = _("{STR_VAR_1} is dancing along with\nthe rolling of the ship.");
static const u8 sCondMsg16[] = _("{STR_VAR_1} doesn't want to get off\nthe boat yet!");
static const u8* const sBoatTexts[] = {sCondMsg14, sCondMsg15, sCondMsg16, NULL};
static const u8 sCondMsg17[] = _("{STR_VAR_1} is listening to the\nsound of the machines.");
static const u8* const sMachineTexts[] = {sCondMsg13, sCondMsg17, NULL};
static const u8 sCondMsg18[] = _("Waah! your POKéMON suddenly splashed\nwater!");
static const u8 sCondMsg19[] = _("Your POKéMON is blowing sand in the\nair!");
static const u8 sCondMsg20[] = _("{STR_VAR_1} is playing around,\nplucking bits of grass.");
static const u8 sCondMsg21[] = _("Your POKéMON is happily looking at\nyour footprints!");
static const u8 sCondMsg22[] = _("{STR_VAR_1} is feeling nervous and a\ntouch claustrophobic.");
static const u8 sCondMsg23[] = _("{STR_VAR_1} is cautious about the\nconfined area!");
static const u8* const sElevatorTexts[] = {sCondMsg22, sCondMsg23, NULL};
static const u8 sCondMsg24[] = _("A cold wind suddenly blew by!");
static const u8 sCondMsg25[] = _("Your POKéMON almost slipped and fell\nover!");
static const u8 sCondMsg26[] = _("Your POKéMON seems surprised to\ntouch ice.");
static const u8* const sColdTexts[] = {sCondMsg24, sCondMsg25, sCondMsg26, NULL};
static const u8 sCondMsg27[] = _("Your POKéMON has a flower petal on\nits face!");
static const u8 sCondMsg28[] = _("{STR_VAR_1} is growling softly.");
static const u8 sCondMsg29[] = _("{STR_VAR_1} is trembling with fear.");
static const u8 sCondMsg30[] = _("{STR_VAR_1} seems somehow sad…");
static const u8* const sFearTexts[] = {sCondMsg29, sCondMsg30, NULL};
static const u8 sCondMsg31[] = _("{STR_VAR_1} is taking shelter in the\ngrass from the rain.");
static const u8 sCondMsg32[] = _("{STR_VAR_1} seems very cold.");
static const u8 sCondMsg33[] = _("{STR_VAR_1} is staring at the sea.");
static const u8 sCondMsg34[] = _("Your pokemon is staring intently at\nthe sea!");
static const u8 sCondMsg35[] = _("{STR_VAR_1} is looking at the\nsurging sea.");
static const u8* const sSeaTexts[] = {sCondMsg33, sCondMsg34, sCondMsg35, NULL};
static const u8 sCondMsg36[] = _("{STR_VAR_1} is listening to the\nsound of the waterfall.");
static const u8 sCondMsg37[] = _("{STR_VAR_1} seems to be happy about\nthe rain!");
static const u8 sCondMsg38[] = _("{STR_VAR_1} is staring at its\nreflection in the water.");
static const u8 sCondMsg39[] = _("{STR_VAR_1} seems to relax as it\nhears the sound of rustling leaves…");
static const u8 sCondMsg40[] = _("{STR_VAR_1} is gnawing at the ice.");
static const u8 sCondMsg41[] = _("{STR_VAR_1} is touching the ice.");
static const u8* const sIceTexts[] = {sCondMsg26, sCondMsg40, sCondMsg41, NULL};
static const u8 sCondMsg42[] = _("{STR_VAR_1}'s burn looks painful!");

// See the struct definition in follower_helper.h for more info
const struct FollowerMsgInfoExtended gFollowerConditionalMessages[COND_MSG_COUNT] = {
    [COND_MSG_CELEBI] =
    {
    .text = (u8*)sCelebiTexts,
    .textSpread = 1,
    .script = EventScript_FollowerDance,
    .st = {.species = SPECIES_CELEBI},
    .stFlags = ST_FLAGS_SPECIES,
    .emotion = FOLLOWER_EMOTION_NEUTRAL,
    },
    [COND_MSG_FIRE] =
    {
    .text = (u8*)sFireTexts,
    .st = {.types = {.type1 = TYPE_FIRE, .type2 = TYPE_FIRE}},
    .stFlags = ST_FLAGS_TYPE,
    .emotion = FOLLOWER_EMOTION_NEUTRAL,
    .textSpread = 1,
    },
    [COND_MSG_EVER_GRANDE] =
    {
    .text = sCondMsg06,
    .script = EventScript_FollowerFaceUp,
    .mm = {.map = {.mapNum = MAP_NUM(EVER_GRANDE_CITY), .mapGroup = MAP_GROUP(EVER_GRANDE_CITY)}},
    .mmFlags = MM_FLAGS_MAP,
    .emotion = FOLLOWER_EMOTION_HAPPY,
    },
    [COND_MSG_ROUTE_112] =
    {
    .text = sCondMsg07,
    .mm = {.map = {.mapNum = MAP_NUM(ROUTE112), .mapGroup = MAP_GROUP(ROUTE112)}},
    .mmFlags = MM_FLAGS_MAP,
    .emotion = FOLLOWER_EMOTION_HAPPY,
    },
    [COND_MSG_DAY_CARE] =
    {
    .text = sCondMsg08,
    .script = EventScript_FollowerNostalgia,
    .mm = {.map = {.mapNum = MAP_NUM(ROUTE117_POKEMON_DAY_CARE), .mapGroup = MAP_GROUP(ROUTE117_POKEMON_DAY_CARE)}},
    .mmFlags = MM_FLAGS_MAP,
    .emotion = FOLLOWER_EMOTION_NEUTRAL,
    },
    [COND_MSG_MART] =
    {
    .text = (u8*)sShopTexts,
    .textSpread = 1,
    .script = EventScript_FollowerLookAround,
    .wt = {.song = MUS_POKE_MART},
    .wtFlags = WT_FLAGS_MUSIC,
    .emotion = FOLLOWER_EMOTION_NEUTRAL,
    },
    [COND_MSG_VICTORY_ROAD] =
    {
    .text = sCondMsg11,
    .wt = {.song = MUS_VICTORY_ROAD},
    .wtFlags = WT_FLAGS_MUSIC,
    .emotion = FOLLOWER_EMOTION_PENSIVE,
    },
    [COND_MSG_BIKE_SHOP] =
    {
    .text = sCondMsg12,
    .mm = {.map = {.mapNum = MAP_NUM(MAUVILLE_CITY_BIKE_SHOP), .mapGroup = MAP_GROUP(MAUVILLE_CITY_BIKE_SHOP)}},
    .mmFlags = MM_FLAGS_MAP,
    .emotion = FOLLOWER_EMOTION_PENSIVE,
    },
    [COND_MSG_MACHINES] =
    {
    .text = (u8*)sMachineTexts,
    .mm = {.map = {.mapNum = MAP_NUM(NEW_MAUVILLE_INSIDE), .mapGroup = MAP_GROUP(NEW_MAUVILLE_INSIDE)}},
    .mmFlags = MM_FLAGS_MAP,
    .emotion = FOLLOWER_EMOTION_MUSIC,
    .textSpread = 1,
    },
    [COND_MSG_SAILING] =
    {
    .text = (u8*)sBoatTexts,
    .script = EventScript_FollowerLookAround,
    .wt = {.song = MUS_SAILING},
    .wtFlags = WT_FLAGS_MUSIC,
    .emotion = FOLLOWER_EMOTION_MUSIC,
    .textSpread = 1,
    },
    [COND_MSG_PUDDLE] =
    {
    .text = sCondMsg18,
    .script = EventScript_FollowerHopping,
    .mm = {.mb = {.behavior1 = MB_SHALLOW_WATER, .behavior2 = MB_PUDDLE}},
    .mmFlags = MM_FLAGS_MB,
    .emotion = FOLLOWER_EMOTION_MUSIC,
    },
    [COND_MSG_SAND] =
    {
    .text = sCondMsg19,
    .mm = {.mb = {.behavior1 = MB_SAND, .behavior2 = MB_DEEP_SAND}},
    .mmFlags = MM_FLAGS_MB,
    .emotion = FOLLOWER_EMOTION_MUSIC,
    },
    [COND_MSG_GRASS] =
    {
    .text = sCondMsg20,
    .mm = {.mb = {.behavior1 = MB_TALL_GRASS, .behavior2 = MB_LONG_GRASS}},
    .mmFlags = MM_FLAGS_MB,
    .emotion = FOLLOWER_EMOTION_MUSIC,
    },
    [COND_MSG_FOOTPRINTS] =
    {
    .text = sCondMsg21,
    .mm = {.mb = {.behavior1 = MB_SAND, .behavior2 = MB_FOOTPRINTS}},
    .mmFlags = MM_FLAGS_MB,
    .emotion = FOLLOWER_EMOTION_MUSIC,
    },
    [COND_MSG_ELEVATOR] =
    {
    .text = (u8*)sElevatorTexts,
    .textSpread = 1,
    .mm = {.map = {.mapNum = MAP_NUM(LILYCOVE_CITY_DEPARTMENT_STORE_ELEVATOR), .mapGroup = MAP_GROUP(LILYCOVE_CITY_DEPARTMENT_STORE_ELEVATOR)}},
    .mmFlags = MM_FLAGS_MAP,
    .emotion = FOLLOWER_EMOTION_SURPRISE,
    },
    [COND_MSG_ICE_ROOM] =
    {
    .text = (u8*)sColdTexts,
    .textSpread = 1,
    .mm = {.map = {.mapNum = MAP_NUM(SHOAL_CAVE_LOW_TIDE_ICE_ROOM), .mapGroup = MAP_GROUP(SHOAL_CAVE_LOW_TIDE_ICE_ROOM)}},
    .mmFlags = MM_FLAGS_MAP,
    .emotion = FOLLOWER_EMOTION_SURPRISE,
    },
    [COND_MSG_ROUTE_117] =
    {
    .text = sCondMsg27,
    .mm = {.map = {.mapNum = MAP_NUM(ROUTE117), .mapGroup = MAP_GROUP(ROUTE117)}},
    .mmFlags = MM_FLAGS_MAP,
    .emotion = FOLLOWER_EMOTION_SURPRISE,
    },
    [COND_MSG_DRAGON_GROWL] =
    {
    .text = sCondMsg28,
    .st = {.types = {.type1 = TYPE_DRAGON, .type2 = TYPE_DRAGON}},
    .stFlags = ST_FLAGS_TYPE,
    .mm = {.mapSec = {.mapSec = MAPSEC_SKY_PILLAR}},
    .mmFlags = MM_FLAGS_MAPSEC,
    .emotion = FOLLOWER_EMOTION_UPSET,
    },
    [COND_MSG_FEAR] =
    {
    .text = (u8*)sFearTexts,
    .textSpread = 1,
    .st = {.types = {.type1 = TYPE_GHOST, .type2 = TYPE_NOT_TYPE1}},
    .stFlags = ST_FLAGS_TYPE,
    .mm = {.mapSec = {.mapSec = MAPSEC_MT_PYRE}},
    .mmFlags = MM_FLAGS_MAPSEC,
    .wt = {.song = MUS_MT_PYRE},
    .wtFlags = WT_FLAGS_MUSIC,
    .emotion = FOLLOWER_EMOTION_UPSET,
    },
    [COND_MSG_FIRE_RAIN] =
    {
    .text = sCondMsg31,
    .st = {.types = {.type1 = TYPE_FIRE, .type2 = TYPE_FIRE}},
    .stFlags = ST_FLAGS_TYPE,
    .wt = {.weather = {.weather1 = WEATHER_RAIN, .weather2 = WEATHER_RAIN_THUNDERSTORM}},
    .wtFlags = WT_FLAGS_WEATHER,
    .emotion = FOLLOWER_EMOTION_UPSET,
    },
    [COND_MSG_FROZEN] =
    {
    .text = sCondMsg32,
    .st = {.status = STATUS1_FREEZE},
    .stFlags = ST_FLAGS_STATUS,
    .emotion = FOLLOWER_EMOTION_UPSET,
    },
    [COND_MSG_SEASIDE] =
    {
    .text = (u8*)sSeaTexts,
    .textSpread = 1,
    .script = EventScript_FollowerFaceResult,
    .near = {.mb = {.behavior = MB_OCEAN_WATER, .distance = 5}},
    .nearFlags = NEAR_FLAGS_MB,
    .emotion = FOLLOWER_EMOTION_MUSIC,
    },
    [COND_MSG_WATERFALL] =
    {
    .text = sCondMsg36,
    .script = EventScript_FollowerFaceResult,
    .near = {.mb = {.behavior = MB_WATERFALL, .distance = 5}},
    .nearFlags = NEAR_FLAGS_MB,
    .emotion = FOLLOWER_EMOTION_MUSIC,
    },
    [COND_MSG_RAIN] =
    {
    .text = sCondMsg37,
    .st = {.types = {.type1 = TYPE_FIRE, .type2 = TYPE_NOT_TYPE1}},
    .stFlags = ST_FLAGS_TYPE,
    .wt = {.weather = {.weather1 = WEATHER_RAIN, .weather2 = WEATHER_RAIN_THUNDERSTORM}},
    .wtFlags = WT_FLAGS_WEATHER,
    .emotion = FOLLOWER_EMOTION_MUSIC,
    },
    [COND_MSG_REFLECTION] =
    {
    .text = sCondMsg38,
    .script = EventScript_FollowerFaceResult,
    .near = {.mb = {.behavior = MB_POND_WATER, .distance = 1}},
    .nearFlags = NEAR_FLAGS_MB,
    .emotion = FOLLOWER_EMOTION_PENSIVE,
    },
    [COND_MSG_LEAVES] =
    {
    .text = sCondMsg39,
    .mm = {.mapSec = {.mapSec = MAPSEC_PETALBURG_WOODS}},
    .mmFlags = MM_FLAGS_MAPSEC,
    .emotion = FOLLOWER_EMOTION_PENSIVE,
    },
    [COND_MSG_ICE] =
    {
    .text = (u8*)sIceTexts,
    .textSpread = 1,
    .script = EventScript_FollowerFaceResult,
    .near = {.mb = {.behavior = MB_ICE, .distance = 1}},
    .nearFlags = NEAR_FLAGS_MB,
    .emotion = FOLLOWER_EMOTION_PENSIVE,
    },
    [COND_MSG_BURN] =
    {
    .text = sCondMsg42,
    .st = {.status = STATUS1_BURN},
    .stFlags = ST_FLAGS_STATUS,
    .emotion = FOLLOWER_EMOTION_SAD,
    },
};
