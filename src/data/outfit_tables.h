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

static const u8 sFrontierPassPlayerIcons_BrendanMay_Gfx[] = INCBIN_U8("graphics/frontier_pass/map_heads.4bpp");
static const u16 sFrontierPassPlayerIcons_Brendan_Pal[] = INCBIN_U16("graphics/frontier_pass/map_heads.gbapal");
static const u16 sFrontierPassPlayerIcons_May_Pal[] = INCBIN_U16("graphics/frontier_pass/map_heads_female.gbapal");

static const u8 sFrontierPassPlayerIcons_RSBrendanMay_Gfx[] = INCBIN_U8("graphics/frontier_pass/rs_map_heads.4bpp");
static const u16 sFrontierPassPlayerIcons_RSBrendan_Pal[] = INCBIN_U16("graphics/frontier_pass/rs_map_heads.gbapal");
static const u16 sFrontierPassPlayerIcons_RSMay_Pal[] = INCBIN_U16("graphics/frontier_pass/rs_map_heads_female.gbapal");

static const u16 sMugshotPal_Brendan[] = INCBIN_U16("graphics/battle_transitions/brendan_bg.gbapal");
static const u16 sMugshotPal_May[] = INCBIN_U16("graphics/battle_transitions/may_bg.gbapal");

#define MUGSHOT_PAL(m, f) { sMugshotPal_##m, sMugshotPal_##f }

#define TRAINER_ID(m, f) \
{ \
    { TRAINER_PIC_##m, TRAINER_BACK_PIC_##m, }, \
    { TRAINER_PIC_##f, TRAINER_BACK_PIC_##f, }, \
}

#define AVATAR_GFX_ID(m, f) \
{ \
    { \
        [PLAYER_AVATAR_STATE_NORMAL]     = CAT(OBJ_EVENT_GFX_##m, _NORMAL), \
        [PLAYER_AVATAR_STATE_BIKE]       = CAT(OBJ_EVENT_GFX_##m, _ACRO_BIKE), \
        [PLAYER_AVATAR_STATE_SURFING]    = CAT(OBJ_EVENT_GFX_##m, _SURFING), \
        [PLAYER_AVATAR_STATE_UNDERWATER] = CAT(OBJ_EVENT_GFX_##m, _UNDERWATER), \
    }, \
    { \
        [PLAYER_AVATAR_STATE_NORMAL]     = CAT(OBJ_EVENT_GFX_##f, _NORMAL), \
        [PLAYER_AVATAR_STATE_BIKE]       = CAT(OBJ_EVENT_GFX_##f, _ACRO_BIKE), \
        [PLAYER_AVATAR_STATE_SURFING]    = CAT(OBJ_EVENT_GFX_##f, _SURFING), \
        [PLAYER_AVATAR_STATE_UNDERWATER] = CAT(OBJ_EVENT_GFX_##f, _UNDERWATER), \
    }, \
}

#define ANIM_GFX_ID(m, f) \
{ \
    { \
        [PLAYER_AVATAR_GFX_FIELD_MOVE] = CAT(OBJ_EVENT_GFX_##m, _FIELD_MOVE), \
        [PLAYER_AVATAR_GFX_FISHING]    = CAT(OBJ_EVENT_GFX_##m, _FISHING), \
        [PLAYER_AVATAR_GFX_WATERING]   = CAT(OBJ_EVENT_GFX_##m, _WATERING), \
        [PLAYER_AVATAR_GFX_DECORATING] = CAT(OBJ_EVENT_GFX_##m, _DECORATING), \
    }, \
    { \
        [PLAYER_AVATAR_GFX_FIELD_MOVE] = CAT(OBJ_EVENT_GFX_##f, _FIELD_MOVE), \
        [PLAYER_AVATAR_GFX_FISHING]    = CAT(OBJ_EVENT_GFX_##f, _FISHING), \
        [PLAYER_AVATAR_GFX_WATERING]   = CAT(OBJ_EVENT_GFX_##f, _WATERING), \
        [PLAYER_AVATAR_GFX_DECORATING] = CAT(OBJ_EVENT_GFX_##f, _DECORATING), \
    }, \
}

#define REGION_MAP_GFX(m, f) { CAT(sRegionMapPlayerIcon_##m, Gfx), CAT(sRegionMapPlayerIcon_##f, Gfx) }
#define FRONTIER_PASS_GFX(id) { CAT(sFrontierPassPlayerIcons_##id, _Gfx), CAT(sFrontierPassPlayerIcons_##id, _Gfx) + 1 * 0x80 }

#define REGION_MAP_PAL(m, f) { CAT(sRegionMapPlayerIcon_##m, Pal), CAT(sRegionMapPlayerIcon_##f, Pal) }
#define FRONTIER_PASS_PAL(m, f) { CAT(sFrontierPassPlayerIcons_##m, _Pal), CAT(sFrontierPassPlayerIcons_##f, _Pal) }

const struct Outfit gOutfits[OUTFIT_COUNT] =
{
    [OUTFIT_USUAL_GREEN] = {
        .isHidden = FALSE,
        .prices = { 0, 0 },
        .name = COMPOUND_STRING("USUAL GREEN"),
        .desc = COMPOUND_STRING("The usual, but basic OUTFIT."),
        .mugshotPals = MUGSHOT_PAL(Brendan, May),
        .trainerPics = TRAINER_ID(BRENDAN, MAY),
        .avatarGfxIds = AVATAR_GFX_ID(BRENDAN, MAY),
        .animGfxIds = ANIM_GFX_ID(BRENDAN, MAY),
        .iconGfx = { REGION_MAP_GFX(Brendan, May), FRONTIER_PASS_GFX(BrendanMay) },
        .iconPal = { REGION_MAP_PAL(Brendan, May), FRONTIER_PASS_PAL(Brendan, May) },
    },
    [OUTFIT_UNUSUAL_RED] = {
        .isHidden = FALSE,
        .prices = { 200, 500 }, //! :^)
        .name = COMPOUND_STRING("UNUSUAL RED"),
        .desc = COMPOUND_STRING("Rather unusual, but still basic OUTFIT."),
        .mugshotPals = MUGSHOT_PAL(Brendan, May),
        .trainerPics = TRAINER_ID(RS_BRENDAN, RS_MAY),
        .avatarGfxIds = AVATAR_GFX_ID(RS_BRENDAN, RS_MAY),
        .animGfxIds = ANIM_GFX_ID(RS_BRENDAN, RS_MAY),
        .iconGfx = { REGION_MAP_GFX(RSBrendan, RSMay), FRONTIER_PASS_GFX(RSBrendanMay) },
        .iconPal = { REGION_MAP_PAL(RSBrendan, RSMay), FRONTIER_PASS_PAL(RSBrendan, RSMay) },
    },
};
