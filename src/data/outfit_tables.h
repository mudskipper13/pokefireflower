#include "constants/global.h"
#include "constants/event_objects.h"
#include "constants/trainers.h"

#if defined(__APPLE__) || defined(__CYGWIN__) || defined(__INTELLISENSE__)
struct Outfit
{
    u8 isHidden:1; //! Will not shows up in the menu if locked.
    u32 prices[GENDER_COUNT]; //! heh
    const u8 *name;
    const u8 *desc;
    u16 trainerPics[GENDER_COUNT][2];
    u16 avatarGfxIds[GENDER_COUNT][4];
    u16 animGfxIds[GENDER_COUNT][4];
    //! region map & frontier pass
    const void *iconsRM[GENDER_COUNT][2]; //! region map
    const void *iconsFP; //! frontier pass
};
#endif

static const u16 sRegionMapPlayerIcon_BrendanGfx[] = INCBIN_U16("graphics/pokenav/region_map/brendan_icon.4bpp");

static const u16 sRegionMapPlayerIcon_RSBrendanGfx[] = INCBIN_U16("graphics/pokenav/region_map/rs_brendan_icon.4bpp");

static const u16 sRegionMapPlayerIcon_MayGfx[] = INCBIN_U16("graphics/pokenav/region_map/may_icon.4bpp");

static const u16 sRegionMapPlayerIcon_RSMayGfx[] = INCBIN_U16("graphics/pokenav/region_map/rs_may_icon.4bpp");

//! TODO: Should the gfx here be seperated?

static const u8 sFrontierPassPlayerIcons_BrendanMay_Gfx[] = INCBIN_U8("graphics/frontier_pass/map_heads.4bpp");

static const u8 sFrontierPassPlayerIcons_RSBrendanMay_Gfx[] = INCBIN_U8("graphics/frontier_pass/rs_map_heads.4bpp");

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

#define REGION_MAP_GFX(m, f) { sRegionMapPlayerIcon_ ## m ## Gfx, sRegionMapPlayerIcon_ ## f ## Gfx }

#ifdef INA
#define USUAL_GREEN_NAME "HIJAU BIASA"
#define USUAL_GREEN_DESC "PAKAIAN biasa, tapi dasar."

#define UNUSUAL_RED_NAME "MERAH TAKBIASA"
#define UNUSUAL_RED_DESC "PAKAIAN yang agak tidak biasa,\ntapi tetap dasar."
#else
#define USUAL_GREEN_NAME "USUAL GREEN"
#define USUAL_GREEN_DESC "The usual, but basic OUTFIT."

#define UNUSUAL_RED_NAME "UNUSUAL RED"
#define UNUSUAL_RED_DESC "Rather unusual, but still basic OUTFIT."
#endif

const struct Outfit gOutfits[OUTFIT_COUNT] =
{
    [OUTFIT_USUAL_GREEN] = {
        //! DESC: if sets to TRUE, it will not be shown in the OUTFIT menu if it's locked.
        .isHidden = FALSE,

        //! DESC: prices for purchasing them.
        .prices = { 0, 0 },

        //! agbcc doesnt like COMPOUND_STRING on my end
        //! DESC: outfit's name
        .name = COMPOUND_STRING(USUAL_GREEN_NAME),

        //! DESC: outfit's description
        .desc = COMPOUND_STRING(USUAL_GREEN_DESC),

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
        .iconsRM = REGION_MAP_GFX(Brendan, May),

        //! DESC: head icons gfx&pal for frontier pass
        //! note that frontier pass needs to be in one sprite instead of two,
        //! unlike region map. (probably should split them tbh)
        .iconsFP = sFrontierPassPlayerIcons_BrendanMay_Gfx,
    },
    [OUTFIT_UNUSUAL_RED] = {
        .isHidden = FALSE,
        .prices = { 200, 500 },
        .name = COMPOUND_STRING(UNUSUAL_RED_NAME),
        .desc = COMPOUND_STRING(UNUSUAL_RED_DESC),
        .trainerPics = TRAINER_ID(RS_BRENDAN, RS_MAY),
        .avatarGfxIds = AVATAR_GFX_ID(RS_BRENDAN, RS_MAY),
        .animGfxIds = ANIM_GFX_ID(RS_BRENDAN, RS_MAY),
        .iconsRM = REGION_MAP_GFX(RSBrendan, RSMay),
        .iconsFP = sFrontierPassPlayerIcons_RSBrendanMay_Gfx,
    },
};
