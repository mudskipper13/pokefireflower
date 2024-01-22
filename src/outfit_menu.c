#include "global.h"
#include "main.h"
#include "bg.h"
#include "malloc.h"
#include "gpu_regs.h"
#include "sprite.h"
#include "string_util.h"
#include "text.h"
#include "window.h"
#include "data.h"
#include "decompress.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "field_weather.h"
#include "graphics.h"
#include "list_menu.h"
#include "menu.h"
#include "menu_helpers.h"
#include "outfit_menu.h"
#include "overworld.h"
#include "palette.h"
#include "palette_util.h"
#include "scanline_effect.h"
#include "sound.h"
#include "task.h"
#include "text_window.h"
#include "trainer_pokemon_sprites.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/trainers.h"

enum BGs {
    BG_0 = 0,
    BG_1,
    BG_2,
    BG_MAX = 2,
    BG_COUNT,
};

enum States {
    STATE_BEGIN = 0,
    STATE_RESET,
    STATE_LOADBG,
    STATE_LOADGFX,
    STATE_LOADWIN,
    STATE_LOADSTR,
    STATE_LOADSPRITE,
    STATE_WAITFADE,
    STATE_TRANSITION,
    STATE_FADE,
    STATE_COUNT,
};

enum Windows {
    WIN_NAME = 0,
    WIN_DESC,
    WIN_HEADER,
    WIN_OUTFIT_MAX = 2,
    WIN_COUNT,
};

enum Sprites {
    GFX_OW = 0,
    GFX_TS,
    GFX_LOCK,
    GFX_INDICATOR,
    GFX_COUNT,
};

enum SpriteTags {
    TAG_SCROLL_ARROWS = 0x1000,
    GFXTAG_LOCK = 0x1100,
    GFXTAG_INDICATOR = 0x1110,
    PALTAG_LOCKINDICATOR = 0x1200,
};

enum ColorId {
    COLORID_NORMAL = 0,
    COLORID_HEADER,
    COLORID_MSGBOX,
    COLORID_NONE,
};

static const u8 sFontColors[][3] = { // bgColor, textColor, shadowColor
    [COLORID_NORMAL] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},
    [COLORID_HEADER] = {15,                     TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},
    [COLORID_MSGBOX] = {TEXT_COLOR_WHITE,       TEXT_COLOR_DARK_GRAY,  TEXT_COLOR_LIGHT_GRAY},
};

typedef struct {
    MainCallback retCB;
    u8 idx;
    u8 gfxState;
    u8 tilemapBuffers[2][BG_SCREEN_SIZE];
    u8 spriteIds[GFX_COUNT];
    u8 switchArrowsTask;
    u16 switchArrowsPos;
} OutfitMenuResources;

static void CB2_SetupOutfitMenu(void);
static void CB2_OutfitMenu(void);
static void VBlankCB_OutfitMenu(void);
static void SetupOutfitMenu_BGs(void);
static bool32 SetupOutfitMenu_Graphics(void);
static void SetupOutfitMenu_Windows(void);
static void SetupOutfitMenu_PrintStr(void);
static void SetupOutfitMenu_Sprites(void);
static void Task_WaitFadeInOutfitMenu(u8 taskId);
static void Task_OutfitMenuHandleInput(u8 taskId);
static void Task_CloseOutfitMenu(u8 taskId);

static const u8 sText_Controls[] =
_(
#ifdef INA
    "{DPAD_LEFTRIGHT}PILIH {A_BUTTON}KONFIRMASI {START_BUTTON}{B_BUTTON}TUTUP"
#else
    "{DPAD_LEFTRIGHT}PICK {A_BUTTON}CONFIRM {START_BUTTON}{B_BUTTON}CLOSE"
#endif
);

static const u8 sText_OutfitLocked[] = _("???");
static const u8 sText_OutfitLockedMsg[] =
_(
#ifdef INA
    "Kamu belum mempunyai PAKAIAN ini.\p"
    "Dapatkan PAKAIAN ini terlebih dahulu\n"
    "untuk bisa menggunakannya."
#else
    "You don't have this OUTFIT yet.\n"
    "Unlock it to be able to use it."
#endif
);

static const u8 sText_OutfitError[] =
_(
#ifdef INA
    "Kamu tidak bisa mengganti pakaian-mu {STR_VAR_1}"
#else
    "You can't change your outfit {STR_VAR_1}"
#endif
);

static const u8 sText_OutfitError_Cycling[] =
_(
#ifdef INA
    "saat\nmenaiki sepeda! Kamu akan jatuh!"
#else
    "while\ncycling! You might get tripped over!"
#endif
);

static const u8 sText_OutfitError_Surfing[] =
_(
#ifdef INA
    "saat\n berenang! Kamu akan kebasahan!"
#else
    "while\nsurfing! You might get wet!"
#endif
);

static const u8 sText_OutfitError_Diving[] =
_(
#ifdef INA
    "saat\n menyelam! Punya akal sehat!"
#else
    "while\ndiving! Have common sense!" //! :masuda:
#endif
);

static const u8 sText_OutfitError_Default[] = _(
#ifdef INA
    "sekarang!"
#else
    "now!"
#endif
);

static const u16 sTiles[] = INCBIN_U16("graphics/outfit_menu/tiles.4bpp");
static const u32 sScrollingBG_Tilemap[] = INCBIN_U32("graphics/outfit_menu/tilemap.bin.lz");
static const u32 sMsgbox_Tilemap[] = INCBIN_U32("graphics/outfit_menu/msgbox.bin.lz");
static const u16 sPalette[] = INCBIN_U16("graphics/outfit_menu/tiles.gbapal");

static const u16 sLockSprite_Gfx[] = INCBIN_U16("graphics/outfit_menu/lock.4bpp");
static const u16 sLockIndicatorSprite_Pal[] = INCBIN_U16("graphics/outfit_menu/lock.gbapal");
static const u16 sIndicatorSprite_Gfx[] = INCBIN_U16("graphics/outfit_menu/indicator.4bpp");
static const u16 sIndicatorSprite_Pal[] = INCBIN_U16("graphics/outfit_menu/indicator.gbapal");

static EWRAM_DATA OutfitMenuResources *sOutfitMenu = NULL;

static const struct ScrollArrowsTemplate sOutfitMenuScrollArrowsTemplate = {
    .firstArrowType = SCROLL_ARROW_LEFT,
    .firstX = 72,
    .firstY = 56,
    .secondArrowType = SCROLL_ARROW_RIGHT,
    .secondX = 168,
    .secondY = 56,
    .fullyUpThreshold = -1,
    .fullyDownThreshold = -1,
    .tileTag = TAG_SCROLL_ARROWS,
    .palTag = TAG_SCROLL_ARROWS,
    .palNum = 0,
};

static const struct BgTemplate sBGTemplates[] =
{
    [BG_0] =
    { //! UI
        .baseTile = 0,
        .bg = 0,
        .charBaseIndex = 1,
        .mapBaseIndex = 29,
        .paletteMode = 0,
        .priority = 1,
        .screenSize = 0,
    },
    [BG_1] =
    { //! BG
        .baseTile = 0,
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .paletteMode = 0,
        .priority = 2,
        .screenSize = 0,
    },
    [BG_2] =
    { //! SCROLLING BG
        .baseTile = 0,
        .bg = 2,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .paletteMode = 0,
        .priority = 3,
        .screenSize = 0,
    },
};

static const struct WindowTemplate sWindowTemplates[] =
{
    [WIN_NAME] =
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 14,
        .width = 12,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    [WIN_DESC] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 16,
        .width = 26,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 25,
    },
    [WIN_HEADER] =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x90,
    },
    DUMMY_WIN_TEMPLATE,
};

static const struct SpriteSheet sLockSpriteSheet = {
    .data = sLockSprite_Gfx,
    .size = 0x200,
    .tag = GFXTAG_LOCK,
};

static const struct SpritePalette sLockIndicatorSpritePalette = {
    .data = sLockIndicatorSprite_Pal,
    .tag = PALTAG_LOCKINDICATOR,
};

static const struct SpriteSheet sIndicatorSpriteSheet = {
    .data = sIndicatorSprite_Gfx,
    .size = 0x100,
    .tag = GFXTAG_INDICATOR,
};

static const struct OamData sLockSpriteOamData = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
};

static const struct OamData sIndicatorSpriteOamData = {
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(16x16),
    .size = SPRITE_SIZE(16x16),
};

static const struct SpriteTemplate sLockSpriteTemplate = {
    .tileTag = GFXTAG_LOCK,
    .paletteTag = PALTAG_LOCKINDICATOR,
    .callback = SpriteCallbackDummy,
    .anims = gDummySpriteAnimTable,
    .affineAnims = gDummySpriteAffineAnimTable,
    .images = NULL,
    .oam = &sLockSpriteOamData,
};

static const struct SpriteTemplate sIndicatorSpriteTemplate = {
    .tileTag = GFXTAG_INDICATOR,
    .paletteTag = PALTAG_LOCKINDICATOR,
    .callback = SpriteCallbackDummy,
    .anims = gDummySpriteAnimTable,
    .affineAnims = gDummySpriteAffineAnimTable,
    .images = NULL,
    .oam = &sIndicatorSpriteOamData,
};

void Task_OpenOutfitMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        OpenOutfitMenu(CB2_ReturnToField);
        DestroyTask(taskId);
    }
}

void OpenOutfitMenu(MainCallback retCB)
{
    sOutfitMenu = AllocZeroed(sizeof(*sOutfitMenu));
    if (sOutfitMenu == NULL)
    {
        //! Alloc failed, exit
        SetMainCallback2(retCB);
    }
    sOutfitMenu->idx = gSaveBlock2Ptr->currOutfitId;
    sOutfitMenu->retCB = retCB;
    sOutfitMenu->switchArrowsTask = TASK_NONE;
    SetMainCallback2(CB2_SetupOutfitMenu);
}

static void CB2_SetupOutfitMenu(void)
{
    switch(gMain.state)
    {
    case STATE_BEGIN:
        CpuFill16(0, (void *) VRAM, VRAM_SIZE);
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
    case STATE_RESET:
        ScanlineEffect_Stop();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        gMain.state++;
        break;
    case STATE_LOADBG:
        SetupOutfitMenu_BGs();
        sOutfitMenu->gfxState = 0;
        gMain.state++;
        break;
    case STATE_LOADGFX:
        if (SetupOutfitMenu_Graphics())
            gMain.state++;
        break;
    case STATE_LOADWIN:
        SetupOutfitMenu_Windows();
        gMain.state++;
        break;
    case STATE_LOADSTR:
        SetupOutfitMenu_PrintStr();
        gMain.state++;
        break;
    case STATE_LOADSPRITE:
        SetupOutfitMenu_Sprites();
        gMain.state++;
        break;
    case STATE_WAITFADE:
        CreateTask(Task_WaitFadeInOutfitMenu, 0);
        gMain.state++;
        break;
    case STATE_TRANSITION:
        BlendPalettes(PALETTES_ALL, 16, 0);
        gMain.state++;
        break;
    case STATE_FADE:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    default:
        SetVBlankCallback(VBlankCB_OutfitMenu);
        SetMainCallback2(CB2_OutfitMenu);
        break;
    }
}

static void CB2_OutfitMenu(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlankCB_OutfitMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
    ChangeBgX(BG_2, 96, BG_COORD_SUB);
    ChangeBgY(BG_2, 96, BG_COORD_SUB);
}

static void SetupOutfitMenu_BGs(void)
{
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    ResetBgsAndClearDma3BusyFlags(0);
    ResetAllBgsCoordinates();
    InitBgsFromTemplates(0, sBGTemplates, ARRAY_COUNT(sBGTemplates));
    SetBgTilemapBuffer(BG_1, sOutfitMenu->tilemapBuffers[0]);
    SetBgTilemapBuffer(BG_2, sOutfitMenu->tilemapBuffers[1]);
    ScheduleBgCopyTilemapToVram(BG_1);
    ScheduleBgCopyTilemapToVram(BG_2);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(BG_0);
    ShowBg(BG_1);
    ShowBg(BG_2);
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_EFFECT_BLEND);
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(1, 9));
    SetGpuReg(REG_OFFSET_BLDY, 0);
}

static bool32 SetupOutfitMenu_Graphics(void)
{
    switch(sOutfitMenu->gfxState)
    {
    case 0:
        ResetTempTileDataBuffers();
        LoadBgTiles(BG_1, &sTiles, 512, 0);
        LoadMessageBoxGfx(BG_0, 0x100, BG_PLTT_ID(13));
        LoadUserWindowBorderGfx(BG_0, 0x10A, BG_PLTT_ID(14));
        sOutfitMenu->gfxState++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            LZDecompressWram(sMsgbox_Tilemap, sOutfitMenu->tilemapBuffers[0]);
            LZDecompressWram(sScrollingBG_Tilemap, sOutfitMenu->tilemapBuffers[1]);
            sOutfitMenu->gfxState++;
        }
        break;
    case 2:
        LoadPalette(&sPalette, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        LoadPalette(GetTextWindowPalette(2), BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        sOutfitMenu->gfxState++;
        break;
    default:
        return TRUE;
    }
    return FALSE;
}

static inline void FillWindow(u8 winId, u8 fillVal)
{
    FillWindowPixelBuffer(winId, fillVal);
    PutWindowTilemap(winId);
}

static inline void PrintTexts(u8 winId, u8 font, u8 x, u8 y, u8 letterSpacing, u8 lineSpacing, u8 color, const u8 *str)
{
    AddTextPrinterParameterized4(winId, font, x, y, letterSpacing, lineSpacing, sFontColors[color], TEXT_SKIP_DRAW, str);
    CopyWindowToVram(winId, COPYWIN_GFX);
}

static void SetupOutfitMenu_Windows(void)
{
    u32 i;
    InitWindows(sWindowTemplates);
    DeactivateAllTextPrinters();
    for (i = 0; i < WIN_OUTFIT_MAX; i++)
        FillWindow(i, PIXEL_FILL(0));

    FillWindow(WIN_HEADER, PIXEL_FILL(15));
    ScheduleBgCopyTilemapToVram(BG_0);
}

static void SetupOutfitMenu_PrintStr(void)
{
    PrintTexts(WIN_NAME, FONT_NORMAL, 0, 0, 0, 0, COLORID_NORMAL, gOutfits[sOutfitMenu->idx].name);
    PrintTexts(WIN_DESC, FONT_NORMAL, 0, 0, 0, 0, COLORID_NORMAL, gOutfits[sOutfitMenu->idx].desc);

    PrintTexts(WIN_HEADER, FONT_SMALL, 0, 0, 0, 0, COLORID_HEADER, sText_Controls);
}

static inline void DestroyPocketSwitchArrowPair(void)
{
    if (sOutfitMenu->switchArrowsTask != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sOutfitMenu->switchArrowsTask);
        sOutfitMenu->switchArrowsTask = TASK_NONE;
    }
}

static inline void CreateOutfitSwitchArrowPair(void)
{
    if (sOutfitMenu->switchArrowsTask == TASK_NONE)
        sOutfitMenu->switchArrowsTask = AddScrollIndicatorArrowPair(&sOutfitMenuScrollArrowsTemplate, &sOutfitMenu->switchArrowsPos);
}

static inline void SetupOutfitMenu_Sprites_DrawOverworldSprite(bool32 update, bool32 locked)
{
    u16 gfxId = GetPlayerAvatarGraphicsIdByOutfitStateIdAndGender(sOutfitMenu->idx, PLAYER_AVATAR_STATE_NORMAL, gSaveBlock2Ptr->playerGender);

    if (update)
        DestroySprite(&gSprites[sOutfitMenu->spriteIds[GFX_OW]]);

    sOutfitMenu->spriteIds[GFX_OW] = CreateObjectGraphicsSpriteNoTint(gfxId, SpriteCallbackDummy, 88, 76, 0);
    StartSpriteAnim(&gSprites[sOutfitMenu->spriteIds[GFX_OW]], ANIM_STD_GO_SOUTH);
}

static inline void SetupOutfitMenu_Sprites_DrawTrainerSprite(bool32 update, bool32 locked)
{
    u16 id = GetPlayerTrainerPicIdByOutfitGenderType(sOutfitMenu->idx, gSaveBlock2Ptr->playerGender, 0);
    if (update)
        FreeAndDestroyTrainerPicSprite(sOutfitMenu->spriteIds[GFX_TS]);

    sOutfitMenu->spriteIds[GFX_TS] = CreateTrainerPicSprite(id, TRUE, 128, 60, 8, id);
}

//! these sprites are sets to true for easier toggling.
static inline void SetupOutfitMenu_Sprites_DrawLockSprite(void)
{
    LoadSpriteSheet(&sLockSpriteSheet);
    LoadSpritePalette(&sLockIndicatorSpritePalette);
    sOutfitMenu->spriteIds[GFX_LOCK] = CreateSprite(&sLockSpriteTemplate, 120, 59, 0);
    gSprites[sOutfitMenu->spriteIds[GFX_LOCK]].invisible = TRUE;
}

static inline void SetupOutfitMenu_Sprites_DrawIndicatorSprite(void)
{
    LoadSpriteSheet(&sIndicatorSpriteSheet);
    // the palette is sLockIndicatorSpritePalette
    sOutfitMenu->spriteIds[GFX_INDICATOR] = CreateSprite(&sIndicatorSpriteTemplate, 104, 120, 0);
    gSprites[sOutfitMenu->spriteIds[GFX_INDICATOR]].invisible = FALSE;
}

static void SetupOutfitMenu_Sprites(void)
{
    SetupOutfitMenu_Sprites_DrawOverworldSprite(FALSE, GetOutfitStatus(sOutfitMenu->idx));
    SetupOutfitMenu_Sprites_DrawTrainerSprite(FALSE, GetOutfitStatus(sOutfitMenu->idx));
    SetupOutfitMenu_Sprites_DrawLockSprite();
    SetupOutfitMenu_Sprites_DrawIndicatorSprite();
    CreateOutfitSwitchArrowPair();
}

//! Similar to above, but without redrawing the frame
//! and also clean up the frame.
static inline void UpdateOutfitInfo(void)
{
    FillWindow(WIN_NAME, PIXEL_FILL(0));
    FillWindow(WIN_DESC, PIXEL_FILL(0));

    if (GetOutfitStatus(sOutfitMenu->idx) == FALSE)
    {
        PrintTexts(WIN_NAME, FONT_NORMAL, 0, 0, 0, 0, COLORID_NORMAL, sText_OutfitLocked);
        PrintTexts(WIN_DESC, FONT_NORMAL, 0, 0, 0, 0, COLORID_NORMAL, sText_OutfitLocked);
        gSprites[sOutfitMenu->spriteIds[GFX_LOCK]].invisible = FALSE;
    }
    else
    {
        PrintTexts(WIN_NAME, FONT_NORMAL, 0, 0, 0, 0, COLORID_NORMAL, gOutfits[sOutfitMenu->idx].name);
        PrintTexts(WIN_DESC, FONT_NORMAL, 0, 0, 0, 0, COLORID_NORMAL, gOutfits[sOutfitMenu->idx].desc);
        gSprites[sOutfitMenu->spriteIds[GFX_LOCK]].invisible = TRUE;
    }
    gSprites[sOutfitMenu->spriteIds[GFX_INDICATOR]].invisible = IsPlayerWearingOutfit(sOutfitMenu->idx) ? FALSE : TRUE;

    SetupOutfitMenu_Sprites_DrawOverworldSprite(TRUE, GetOutfitStatus(sOutfitMenu->idx));
    SetupOutfitMenu_Sprites_DrawTrainerSprite(TRUE, GetOutfitStatus(sOutfitMenu->idx));
}

static void Task_WaitFadeInOutfitMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        PlaySE(SE_RG_HELP_OPEN);
        gTasks[taskId].func = Task_OutfitMenuHandleInput;
    }
}

static void Task_WaitMessage(u8 taskId)
{
    if (--gTasks[taskId].data[0] == 0)
    {
        ClearDialogWindowAndFrame(WIN_DESC, TRUE);
        gWindows[WIN_DESC].window.tilemapTop = sWindowTemplates[WIN_DESC].tilemapTop;
        gWindows[WIN_DESC].window.tilemapLeft = sWindowTemplates[WIN_DESC].tilemapLeft;
        UpdateOutfitInfo();
        gTasks[taskId].func = Task_OutfitMenuHandleInput;
    }
}

static inline void PrintDialogueBoxWithDescWin(const u8 *str, bool32 expandPlaceholders, u8 taskId)
{
    const u8 *txt = expandPlaceholders ? gStringVar4 : str;
    gWindows[WIN_DESC].window.tilemapTop = 15;
    gWindows[WIN_DESC].window.tilemapLeft = 2;
    DrawDialogFrameWithCustomTileAndPalette(WIN_DESC, TRUE, 0x100, 13);

    if (expandPlaceholders)
        StringExpandPlaceholders(gStringVar4, str);

    PrintTexts(WIN_DESC, FONT_NORMAL, 0, 0, 0, 0, COLORID_MSGBOX, txt);
    gTasks[taskId].data[0] = 70;
    gTasks[taskId].func = Task_WaitMessage;
}

static void Task_PrintCantChangeOutfit(u8 taskId)
{
    if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_BIKE)
        StringCopy(gStringVar1, sText_OutfitError_Cycling);
    else if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_SURFING)
        StringCopy(gStringVar1, sText_OutfitError_Surfing);
    else if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_UNDERWATER)
        StringCopy(gStringVar1, sText_OutfitError_Diving);
    else
        StringCopy(gStringVar1, sText_OutfitError_Default);

    PrintDialogueBoxWithDescWin(sText_OutfitError, TRUE, taskId);
}

static void Task_PrintOutfitLocked(u8 taskId)
{
    PrintDialogueBoxWithDescWin(sText_OutfitLockedMsg, FALSE, taskId);
}

static inline void CloseOutfitMenu(u8 taskId)
{
    PlaySE(SE_RG_HELP_CLOSE);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_CloseOutfitMenu;
}

#define PICK_BUTTONS  (DPAD_RIGHT | DPAD_LEFT)
#define CLOSE_BUTTONS (B_BUTTON | START_BUTTON)

static void Task_OutfitMenuHandleInput(u8 taskId)
{
    if (!IsSEPlaying())
    {
        if (JOY_NEW(CLOSE_BUTTONS))
            CloseOutfitMenu(taskId);

        if (JOY_NEW(A_BUTTON))
        {
            if (gPlayerAvatar.flags & PLAYER_AVATAR_FLAG_ON_FOOT)
            {
                if (GetOutfitStatus(sOutfitMenu->idx))
                {
                    PlaySE(SE_SUCCESS);
                    gSprites[sOutfitMenu->spriteIds[GFX_INDICATOR]].invisible = FALSE;
                    gSaveBlock2Ptr->currOutfitId = sOutfitMenu->idx;
                }
                else
                {
                    PlaySE(SE_BOO);
                    gTasks[taskId].func = Task_PrintOutfitLocked;
                }
            }
            else
            {
                PlaySE(SE_BOO);
                if (GetOutfitStatus(sOutfitMenu->idx))
                    gTasks[taskId].func = Task_PrintCantChangeOutfit;
                else
                    gTasks[taskId].func = Task_PrintOutfitLocked; //! might be confusing?
            }
        }

        if (JOY_NEW(DPAD_RIGHT))
        {
            if (sOutfitMenu->idx < OUTFIT_END)
                sOutfitMenu->idx++;
            else
                sOutfitMenu->idx = OUTFIT_BEGIN;

            UpdateOutfitInfo();
        }

        if (JOY_NEW(DPAD_LEFT))
        {
            if (sOutfitMenu->idx != OUTFIT_BEGIN)
                sOutfitMenu->idx--;
            else
                sOutfitMenu->idx = OUTFIT_END;

            UpdateOutfitInfo();
        }

        if (JOY_NEW(PICK_BUTTONS))
            PlaySE(SE_RG_BAG_CURSOR);
    }
}

static void FreeOutfitMenuResources(void)
{
    DestroySprite(&gSprites[sOutfitMenu->spriteIds[GFX_OW]]);
    FreeAndDestroyTrainerPicSprite(sOutfitMenu->spriteIds[GFX_TS]);
    DestroySprite(&gSprites[sOutfitMenu->spriteIds[GFX_LOCK]]);
    DestroySprite(&gSprites[sOutfitMenu->spriteIds[GFX_INDICATOR]]);
    DestroyPocketSwitchArrowPair();
    TRY_FREE_AND_SET_NULL(sOutfitMenu);
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeAllWindowBuffers();
}

static void Task_CloseOutfitMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sOutfitMenu->retCB);
        FreeOutfitMenuResources();
        DestroyTask(taskId);
    }
}

//! misc funcs

void BufferOutfitStrings(u8 *dest, u8 outfitId, u8 dataType)
{
    const u8 *src = NULL;
    switch(dataType)
    {
    default:
    case OUTFIT_BUFFER_NAME:
        src = gOutfits[outfitId].name;
        break;
    case OUTFIT_BUFFER_DESC:
        src = gOutfits[outfitId].desc;
        break;
    }
    StringCopy(dest, src);
}

u32 GetPlayerTrainerPicIdByOutfitGenderType(u32 outfitId, u32 gender, bool32 type)
{
    if (outfitId < OUTFIT_COUNT)
        return gOutfits[outfitId].trainerPics[gender][type];
    else
        return gOutfits[0].trainerPics[gender][type];
}

const u16 *GetPlayerBattleTransitionMugshotPalette(void)
{
    return gOutfits[gSaveBlock2Ptr->currOutfitId].mugshotPals[gSaveBlock2Ptr->playerGender];
}

const void *GetPlayerHeadGfxOrPal(u8 which, bool32 isFP)
{
    if (isFP)
    {
        if (which == GFX)
            return gOutfits[gSaveBlock2Ptr->currOutfitId].iconsFP[gSaveBlock2Ptr->playerGender][GFX];
        else
            return gOutfits[gSaveBlock2Ptr->currOutfitId].iconsFP[gSaveBlock2Ptr->playerGender][PAL];
    }
    else
    {
        if (which == GFX)
            return gOutfits[gSaveBlock2Ptr->currOutfitId].iconsRM[gSaveBlock2Ptr->playerGender][GFX];
        else
            return gOutfits[gSaveBlock2Ptr->currOutfitId].iconsRM[gSaveBlock2Ptr->playerGender][PAL];
    }
}

u16 *GetOutfitPointer(u16 id)
{
    if (id > OUTFIT_COUNT)
        return NULL;
    else
        return &gSaveBlock2Ptr->outfits[id / 8];
}

u16 UnlockOutfit(u16 id)
{
    u16 *ptr = GetOutfitPointer(id);
    if (ptr)
        *ptr |= 1 << (id & 7);
    return 0;
}

u16 ToggleOutfit(u16 id)
{
    u16 *ptr = GetOutfitPointer(id);
    if (ptr)
        *ptr ^= 1 << (id & 7);
    return 0;
}

u16 LockOutfit(u16 id)
{
    u16 *ptr = GetOutfitPointer(id);
    if (ptr)
        *ptr &= ~(1 << (id & 7));
    return 0;
}

bool8 GetOutfitStatus(u16 id)
{
    u16 *ptr = GetOutfitPointer(id);

    // return false if GetOutfitPointer returns NULL
    if (!ptr)
        return FALSE;

    // return false if flag is not set
    if (!(((*ptr) >> (id & 7)) & 1))
        return FALSE;

    // rest
    return TRUE;
}

bool8 IsPlayerWearingOutfit(u16 id)
{
    if (gSaveBlock2Ptr->currOutfitId == id)
        return TRUE;

    return FALSE;
}
