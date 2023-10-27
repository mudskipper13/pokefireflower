#include "constants/global.h"
#include "constants/event_objects.h"

const u8 gOutfitFrontPics[OUTFIT_COUNT][GENDER_COUNT] = 
{
    [OUTFIT_A] = {TRAINER_PIC_BRENDAN, TRAINER_PIC_MAY},
    [OUTFIT_B] = {TRAINER_PIC_RS_BRENDAN, TRAINER_PIC_RS_MAY},
};

const u8 gOutfitBackPics[OUTFIT_COUNT][GENDER_COUNT] = 
{
    [OUTFIT_A] = {TRAINER_BACK_PIC_BRENDAN, TRAINER_BACK_PIC_MAY},
    [OUTFIT_B] = {TRAINER_BACK_PIC_RUBY_SAPPHIRE_BRENDAN, TRAINER_BACK_PIC_RUBY_SAPPHIRE_MAY},
};

const u16 gPlayerAvatarGfxIds[OUTFIT_COUNT][4][GENDER_COUNT] =
{
    [OUTFIT_A] =
    {
        [PLAYER_AVATAR_STATE_NORMAL]     = {OBJ_EVENT_GFX_BRENDAN_NORMAL,     OBJ_EVENT_GFX_MAY_NORMAL},
        [PLAYER_AVATAR_STATE_BIKE]       = {OBJ_EVENT_GFX_BRENDAN_ACRO_BIKE,  OBJ_EVENT_GFX_MAY_ACRO_BIKE},
        [PLAYER_AVATAR_STATE_SURFING]    = {OBJ_EVENT_GFX_BRENDAN_SURFING,    OBJ_EVENT_GFX_MAY_SURFING},
        [PLAYER_AVATAR_STATE_UNDERWATER] = {OBJ_EVENT_GFX_BRENDAN_UNDERWATER, OBJ_EVENT_GFX_MAY_UNDERWATER},
    },
    [OUTFIT_B] =
    {
        [PLAYER_AVATAR_STATE_NORMAL]     = {OBJ_EVENT_GFX_LINK_RS_BRENDAN,      OBJ_EVENT_GFX_LINK_RS_MAY},
        [PLAYER_AVATAR_STATE_BIKE]       = {OBJ_EVENT_GFX_RS_BRENDAN_ACRO_BIKE, OBJ_EVENT_GFX_RS_MAY_ACRO_BIKE},
        [PLAYER_AVATAR_STATE_SURFING]    = {OBJ_EVENT_GFX_RS_BRENDAN_SURFING,   OBJ_EVENT_GFX_RS_MAY_SURFING},
        [PLAYER_AVATAR_STATE_UNDERWATER] = {OBJ_EVENT_GFX_BRENDAN_UNDERWATER,   OBJ_EVENT_GFX_MAY_UNDERWATER},
    }
};

const u16 gPlayerAvatarAnimGfxIds[OUTFIT_COUNT][3][GENDER_COUNT] =
{
    [OUTFIT_A] =
    {
        [PLAYER_AVATAR_GFX_FIELD_MOVE] = {OBJ_EVENT_GFX_BRENDAN_FIELD_MOVE, OBJ_EVENT_GFX_MAY_FIELD_MOVE},
        [PLAYER_AVATAR_GFX_FISHING]    = {OBJ_EVENT_GFX_BRENDAN_FISHING,    OBJ_EVENT_GFX_MAY_FISHING},
        [PLAYER_AVATAR_GFX_WATERING]   = {OBJ_EVENT_GFX_BRENDAN_WATERING,   OBJ_EVENT_GFX_MAY_WATERING},
    },
    [OUTFIT_B] =
    {
        [PLAYER_AVATAR_GFX_FIELD_MOVE] = {OBJ_EVENT_GFX_RS_BRENDAN_FIELD_MOVE, OBJ_EVENT_GFX_RS_MAY_FIELD_MOVE},
        [PLAYER_AVATAR_GFX_FISHING]    = {OBJ_EVENT_GFX_RS_BRENDAN_FISHING,    OBJ_EVENT_GFX_RS_MAY_FISHING},
        [PLAYER_AVATAR_GFX_WATERING]   = {OBJ_EVENT_GFX_RS_BRENDAN_WATERING,   OBJ_EVENT_GFX_RS_MAY_WATERING},
    }
};

static const u8 sRegionMapPlayerIcon_BrendanGfx[] = INCBIN_U8("graphics/pokenav/region_map/brendan_icon.4bpp");
static const u16 sRegionMapPlayerIcon_BrendanPal[] = INCBIN_U16("graphics/pokenav/region_map/brendan_icon.gbapal");

static const u8 sRegionMapPlayerIcon_RSBrendanGfx[] = INCBIN_U8("graphics/pokenav/region_map/rs_brendan_icon.4bpp");
static const u16 sRegionMapPlayerIcon_RSBrendanPal[] = INCBIN_U16("graphics/pokenav/region_map/rs_brendan_icon.gbapal");

static const u8 sRegionMapPlayerIcon_MayGfx[] = INCBIN_U8("graphics/pokenav/region_map/may_icon.4bpp");
static const u16 sRegionMapPlayerIcon_MayPal[] = INCBIN_U16("graphics/pokenav/region_map/may_icon.gbapal");

static const u8 sRegionMapPlayerIcon_RSMayGfx[] = INCBIN_U8("graphics/pokenav/region_map/rs_may_icon.4bpp");
static const u16 sRegionMapPlayerIcon_RSMayPal[] = INCBIN_U16("graphics/pokenav/region_map/rs_may_icon.gbapal");

const struct OutfitIcon gOutfitToRegionMapIcon[OUTFIT_COUNT * GENDER_COUNT] =
{
    { OUTFIT_A, MALE,   sRegionMapPlayerIcon_BrendanGfx,   sRegionMapPlayerIcon_BrendanPal },
    { OUTFIT_A, FEMALE, sRegionMapPlayerIcon_MayGfx,       sRegionMapPlayerIcon_MayPal },
    { OUTFIT_B, MALE,   sRegionMapPlayerIcon_RSBrendanGfx, sRegionMapPlayerIcon_RSBrendanPal },
    { OUTFIT_B, FEMALE, sRegionMapPlayerIcon_RSMayGfx,     sRegionMapPlayerIcon_RSMayPal },
};

static const u32 sFrontierPassPlayerIcons_Gfx[] = INCBIN_U32("graphics/frontier_pass/map_heads.4bpp.lz");
static const u16 sFrontierPassPlayerIcons_Brendan_Pal[] = INCBIN_U16("graphics/frontier_pass/map_heads.gbapal");
static const u16 sFrontierPassPlayerIcons_May_Pal[] = INCBIN_U16("graphics/frontier_pass/map_heads_female.gbapal");

static const u32 sFrontierPassPlayerIcons_RS_Gfx[] = INCBIN_U32("graphics/frontier_pass/rs_map_heads.4bpp.lz");
static const u16 sFrontierPassPlayerIcons_RSBrendan_Pal[] = INCBIN_U16("graphics/frontier_pass/rs_map_heads.gbapal");
static const u16 sFrontierPassPlayerIcons_RSMay_Pal[] = INCBIN_U16("graphics/frontier_pass/rs_map_heads_female.gbapal");

const struct OutfitIcon gOutfitToFrontierPassIcon[OUTFIT_COUNT * GENDER_COUNT] =
{
    { OUTFIT_A, MALE,   sFrontierPassPlayerIcons_Gfx, sFrontierPassPlayerIcons_Brendan_Pal },
    { OUTFIT_A, FEMALE, sFrontierPassPlayerIcons_Gfx, sFrontierPassPlayerIcons_May_Pal },
    { OUTFIT_B, MALE,   sFrontierPassPlayerIcons_RS_Gfx, sFrontierPassPlayerIcons_RSBrendan_Pal },
    { OUTFIT_B, FEMALE, sFrontierPassPlayerIcons_RS_Gfx, sFrontierPassPlayerIcons_RSMay_Pal },
};
