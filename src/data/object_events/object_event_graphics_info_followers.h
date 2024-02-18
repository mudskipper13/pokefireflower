#if OW_MON_POKEBALLS

#define POKEBALL_GFX_INFO(NAME)                                     \
    [BALL_##NAME] = {                                               \
        .tileTag = TAG_NONE,                                        \
        .paletteTag = OBJ_EVENT_PAL_TAG_BALL_##NAME,                \
        .size = 256,                                                \
        .width = 16,                                                \
        .height = 32,                                               \
        .textColor = NPC_TEXT_COLOR_NEUTRAL,                        \
        .shadowSize = SHADOW_SIZE_M,                                \
        .inanimate = TRUE,                                          \
        .oam = &gObjectEventBaseOam_16x32,                          \
        .subspriteTables = sOamTables_16x32,                        \
        .anims = OBJ_EVENT_ANIM_FOLLOWER,                           \
        .images = sPicTable_Ball_##NAME,                            \
        .affineAnims = gDummySpriteAffineAnimTable,                 \
    }


const struct ObjectEventGraphicsInfo gPokeballGraphics[POKEBALL_COUNT] = {
    // Vanilla
    POKEBALL_GFX_INFO(MASTER),
    POKEBALL_GFX_INFO(ULTRA),
    POKEBALL_GFX_INFO(GREAT),
    POKEBALL_GFX_INFO(SAFARI),
    POKEBALL_GFX_INFO(NET),
    POKEBALL_GFX_INFO(DIVE),
    POKEBALL_GFX_INFO(NEST),
    POKEBALL_GFX_INFO(REPEAT),
    POKEBALL_GFX_INFO(TIMER),
    POKEBALL_GFX_INFO(LUXURY),
    POKEBALL_GFX_INFO(PREMIER),
    // Gen IV/Sinnoh pokeballs
    #ifdef ITEM_DUSK_BALL
    POKEBALL_GFX_INFO(DUSK),
    POKEBALL_GFX_INFO(HEAL),
    POKEBALL_GFX_INFO(QUICK),
    POKEBALL_GFX_INFO(CHERISH),
    #endif
    #ifdef ITEM_PARK_BALL
    POKEBALL_GFX_INFO(PARK),
    #endif
    // Gen II/Johto Apricorn pokeballs
    #ifdef ITEM_FAST_BALL
    POKEBALL_GFX_INFO(FAST),
    POKEBALL_GFX_INFO(LEVEL),
    POKEBALL_GFX_INFO(LURE),
    POKEBALL_GFX_INFO(HEAVY),
    POKEBALL_GFX_INFO(LOVE),
    POKEBALL_GFX_INFO(FRIEND),
    POKEBALL_GFX_INFO(MOON),
    POKEBALL_GFX_INFO(SPORT),
    #endif
    // Gen V
    #ifdef ITEM_DREAM_BALL
    POKEBALL_GFX_INFO(DREAM),
    #endif
    // Gen VII
    #ifdef ITEM_BEAST_BALL
    POKEBALL_GFX_INFO(BEAST),
    #endif
    // Gen VIII
    #ifdef ITEM_STRANGE_BALL
    POKEBALL_GFX_INFO(STRANGE),
    #endif
};
#endif
