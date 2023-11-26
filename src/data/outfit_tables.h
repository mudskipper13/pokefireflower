#include "constants/global.h"
#include "constants/event_objects.h"
#include "constants/trainers.h"

static const u8 sRegionMapPlayerIcon_BrendanGfx[] = INCBIN_U8("graphics/pokenav/region_map/brendan_icon.4bpp");
static const u16 sRegionMapPlayerIcon_BrendanPal[] = INCBIN_U16("graphics/pokenav/region_map/brendan_icon.gbapal");

static const u8 sRegionMapPlayerIcon_RSBrendanGfx[] = INCBIN_U8("graphics/pokenav/region_map/rs_brendan_icon.4bpp");
static const u16 sRegionMapPlayerIcon_RSBrendanPal[] = INCBIN_U16("graphics/pokenav/region_map/rs_brendan_icon.gbapal");

static const u8 sRegionMapPlayerIcon_MayGfx[] = INCBIN_U8("graphics/pokenav/region_map/may_icon.4bpp");
static const u16 sRegionMapPlayerIcon_MayPal[] = INCBIN_U16("graphics/pokenav/region_map/may_icon.gbapal");

static const u8 sRegionMapPlayerIcon_RSMayGfx[] = INCBIN_U8("graphics/pokenav/region_map/rs_may_icon.4bpp");
static const u16 sRegionMapPlayerIcon_RSMayPal[] = INCBIN_U16("graphics/pokenav/region_map/rs_may_icon.gbapal");

//! TODO: Should the gfx here be seperated?

static const u8 sFrontierPassPlayerIcons_BrendanMay_Gfx[] = INCBIN_U8("graphics/frontier_pass/map_heads.4bpp");
static const u16 sFrontierPassPlayerIcons_Brendan_Pal[] = INCBIN_U16("graphics/frontier_pass/map_heads.gbapal");
static const u16 sFrontierPassPlayerIcons_May_Pal[] = INCBIN_U16("graphics/frontier_pass/map_heads_female.gbapal");

static const u8 sFrontierPassPlayerIcons_RSBrendanMay_Gfx[] = INCBIN_U8("graphics/frontier_pass/rs_map_heads.4bpp");
static const u16 sFrontierPassPlayerIcons_RSBrendan_Pal[] = INCBIN_U16("graphics/frontier_pass/rs_map_heads.gbapal");
static const u16 sFrontierPassPlayerIcons_RSMay_Pal[] = INCBIN_U16("graphics/frontier_pass/rs_map_heads_female.gbapal");

static const u16 sMugshotPal_Brendan[] = INCBIN_U16("graphics/battle_transitions/brendan_bg.gbapal");
static const u16 sMugshotPal_May[] = INCBIN_U16("graphics/battle_transitions/may_bg.gbapal");

#define TRAINER_ID(m, f) \
{ \
    [MALE] =   { TRAINER_PIC_ ## m, TRAINER_BACK_PIC_ ## m, }, \
    [FEMALE] = { TRAINER_PIC_ ## f, TRAINER_BACK_PIC_ ## f, }, \
}

#define AVATAR_GFX_ID(m, f) \
{ \
    [MALE] = { \
        [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_ ## m ## _NORMAL, \
        [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_ ## m ## _ACRO_BIKE, \
        [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_ ## m ## _SURFING, \
        [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_ ## m ## _UNDERWATER \
    }, \
    [FEMALE] = { \
        [PLAYER_AVATAR_STATE_NORMAL] =     OBJ_EVENT_GFX_ ## f ## _NORMAL, \
        [PLAYER_AVATAR_STATE_BIKE] =       OBJ_EVENT_GFX_ ## f ## _ACRO_BIKE, \
        [PLAYER_AVATAR_STATE_SURFING] =    OBJ_EVENT_GFX_ ## f ## _SURFING, \
        [PLAYER_AVATAR_STATE_UNDERWATER] = OBJ_EVENT_GFX_ ## f ## _UNDERWATER \
    }, \
}

#define ANIM_GFX_ID(m, f) \
{ \
    [MALE] = { \
        [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_ ## m ## _FIELD_MOVE, \
        [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_ ## m ## _FISHING, \
        [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_ ## m ## _WATERING, \
        [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_ ## m ## _DECORATING \
    }, \
    [FEMALE] = { \
        [PLAYER_AVATAR_GFX_FIELD_MOVE] = OBJ_EVENT_GFX_ ## f ## _FIELD_MOVE, \
        [PLAYER_AVATAR_GFX_FISHING] =    OBJ_EVENT_GFX_ ## f ## _FISHING, \
        [PLAYER_AVATAR_GFX_WATERING] =   OBJ_EVENT_GFX_ ## f ## _WATERING, \
        [PLAYER_AVATAR_GFX_DECORATING] = OBJ_EVENT_GFX_ ## f ## _DECORATING \
    }, \
}

#define REGION_MAP_GFX(m, f) \
    [MALE] = { sRegionMapPlayerIcon_ ## m ## Gfx, sRegionMapPlayerIcon_ ## m ## Pal }, \
    [FEMALE] = { sRegionMapPlayerIcon_ ## f ## Gfx, sRegionMapPlayerIcon_ ## f ## Pal },

#define FRONTIER_PASS_GFX(id, m, f) \
    [MALE] = { sFrontierPassPlayerIcons_ ## id ## _Gfx, sFrontierPassPlayerIcons_ ## m ## _Pal }, \
    [FEMALE] = { sFrontierPassPlayerIcons_ ## id ## _Gfx + 1 * 0x80, sFrontierPassPlayerIcons_ ## f ## _Pal },

const struct Outfit gOutfits[OUTFIT_COUNT] =
{
    [OUTFIT_USUAL_GREEN] = {
        //! TODO: implement
        .isHidden = FALSE, //! DESC: if sets to TRUE, it will not shows in the OUTFIT menu.

        //! TODO: implement
        .prices = { 0, 0 }, //! DESC: prices for purchasing them.

        //! DESC: outfit's name
        .name = COMPOUND_STRING("USUAL GREEN"),

        //! DESC: outfit's description
        .desc = COMPOUND_STRING("The usual, but basic OUTFIT."),

        //! DESC: palettes for the bg of the mugshot battle transition
        .mugshotPals = { sMugshotPal_Brendan, sMugshotPal_May },

        //! DESC: trainer front & back pic index
        //! (see include/constants/trainers.h)
        .trainerPics = TRAINER_ID(BRENDAN, MAY),

        //! DESC: overworld avatars, consisting of: walking, cycling,
        //! surfing, and underwater. (see include/constants/event_object.h)
        .avatarGfxIds = AVATAR_GFX_ID(BRENDAN, MAY),

        //! DESC: overworld anims, consisting of: field move, fishing,
        //! water, and decorating. (see include/constants/event_object.h)
        .animGfxIds = ANIM_GFX_ID(BRENDAN, MAY),

        //! DESC: head icons gfx&pal for region map
        .iconsRM = { REGION_MAP_GFX(Brendan, May) },

        //! DESC: head icons gfx&pal for frontier pass
        //! note that frontier pass needs to be in one sprite instead of two,
        //! unlike region map. (probably should split them tbh)
        .iconsFP = { FRONTIER_PASS_GFX(BrendanMay, Brendan, May) },
    },
    [OUTFIT_UNUSUAL_RED] = {
        .isHidden = FALSE,
        .prices = { 200, 500 },
        .name = COMPOUND_STRING("UNUSUAL RED"),
        .desc = COMPOUND_STRING("Rather unusual, but still basic OUTFIT."),
        .mugshotPals = { sMugshotPal_Brendan, sMugshotPal_May },
        .trainerPics = TRAINER_ID(RS_BRENDAN, RS_MAY),
        .avatarGfxIds = AVATAR_GFX_ID(RS_BRENDAN, RS_MAY),
        .animGfxIds = ANIM_GFX_ID(RS_BRENDAN, RS_MAY),
        .iconsRM = { REGION_MAP_GFX(RSBrendan, RSMay) },
        .iconsFP = { FRONTIER_PASS_GFX(RSBrendanMay, RSBrendan, RSMay) },
    },
};
