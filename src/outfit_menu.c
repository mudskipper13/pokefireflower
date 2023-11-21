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
    GFX_COUNT,
};

enum {
    COLORID_NORMAL = 0,
    COLORID_HEADER,
    COLORID_BLUE,
    COLORID_NONE,
};

static const u8 sFontColors[][3] = { // bgColor, textColor, shadowColor
    [COLORID_NORMAL] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},
    [COLORID_HEADER] = {15,                     TEXT_COLOR_WHITE,      TEXT_COLOR_DARK_GRAY},
    [COLORID_BLUE]   = {TEXT_COLOR_WHITE,       TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_BLUE},
};

typedef struct {
    MainCallback retCB;
    u8 idx;
    u8 gfxState;
    u8 spriteIds[GFX_COUNT];
    u8 tilemapBuffers[2][BG_SCREEN_SIZE];
    u8 switchArrowsTask;
    u16 switchArrowsPos;
} OutfitMenuResources;

static const u8 sText_Controls[] =
_(
    "{DPAD_LEFTRIGHT}PICK {A_BUTTON}CONFIRM {B_BUTTON}CANCEL"
);

static const u16 sTiles[] = INCBIN_U16("graphics/outfit_menu/tiles.4bpp");
static const u32 sScrollingBG_Tilemap[] = INCBIN_U32("graphics/outfit_menu/tilemap.bin.lz");
static const u32 sMsgbox_Tilemap[] = INCBIN_U32("graphics/outfit_menu/msgbox.bin.lz");
static const u16 sPalette[] = INCBIN_U16("graphics/outfit_menu/tiles.gbapal");

static EWRAM_DATA OutfitMenuResources *sOutfitMenu = NULL;

#define TAG_SCROLL_WINDOW 0x1000

static const struct ScrollArrowsTemplate sOutfitMenuScrollArrowsTemplate = {
    .firstArrowType = SCROLL_ARROW_LEFT,
    .firstX = 72,
    .firstY = 56,
    .secondArrowType = SCROLL_ARROW_RIGHT,
    .secondX = 168,
    .secondY = 56,
    .fullyUpThreshold = -1,
    .fullyDownThreshold = -1,
    .tileTag = TAG_SCROLL_WINDOW,
    .palTag = TAG_SCROLL_WINDOW,
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
        .baseBlock = 0x20,
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
        // Alloc failed, exit
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
        SetBackdropFromColor(RGB2GBA(212, 115, 106));
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
    u32 i;
    for (i = 0; i < WIN_OUTFIT_MAX; i++)
        PrintTexts(i, FONT_NORMAL, 0, 0, 0, 0, COLORID_NORMAL, gOutfitNameDescTables[sOutfitMenu->idx][i]);

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

static void SetupOutfitMenu_Sprites_DrawOverworldSprite(bool32 update)
{
    u16 gfxId = gPlayerAvatarGfxIds[sOutfitMenu->idx][PLAYER_AVATAR_STATE_NORMAL][gSaveBlock2Ptr->playerGender];

    if (update)
        DestroySprite(&gSprites[sOutfitMenu->spriteIds[GFX_OW]]);

    sOutfitMenu->spriteIds[GFX_OW] = CreateObjectGraphicsSpriteNoTint(gfxId, SpriteCallbackDummy, 90, 70, 0);
    StartSpriteAnim(&gSprites[sOutfitMenu->spriteIds[GFX_OW]], ANIM_STD_GO_SOUTH);
}

static void SetupOutfitMenu_Sprites_DrawTrainerSprite(bool32 update)
{
    u16 id = gOutfitFrontPics[sOutfitMenu->idx][gSaveBlock2Ptr->playerGender];
    if (update)
        FreeAndDestroyTrainerPicSprite(sOutfitMenu->spriteIds[GFX_TS]);

    sOutfitMenu->spriteIds[GFX_TS] = CreateTrainerPicSprite(id, TRUE, 125, 58, 8, id);
}

static void SetupOutfitMenu_Sprites(void)
{
    SetupOutfitMenu_Sprites_DrawOverworldSprite(FALSE);
    SetupOutfitMenu_Sprites_DrawTrainerSprite(FALSE);
    CreateOutfitSwitchArrowPair();
}

//! Similar to above, but without redrawing the frame
//! and also clean up the frame.
static inline void UpdateOutfitInfo(void)
{
    u32 i;
    for (i = 0; i < WIN_OUTFIT_MAX; i++)
    {
        FillWindow(i, PIXEL_FILL(0));
        PrintTexts(i, FONT_NORMAL, 0, 0, 0, 0, COLORID_NORMAL, gOutfitNameDescTables[sOutfitMenu->idx][i]);
    }
    SetupOutfitMenu_Sprites_DrawOverworldSprite(TRUE);
    SetupOutfitMenu_Sprites_DrawTrainerSprite(TRUE);
    DebugPrintf("sOutfitMenu->spriteIds[GFX_OW] = %d", sOutfitMenu->spriteIds[GFX_OW]);
    DebugPrintf("sOutfitMenu->spriteIds[GFX_TS] = %d", sOutfitMenu->spriteIds[GFX_TS]);
}

static void Task_WaitFadeInOutfitMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        PlaySE(SE_RG_HELP_OPEN);
        gTasks[taskId].func = Task_OutfitMenuHandleInput;
    }
}

#define ANY_BUTTONS (DPAD_RIGHT | DPAD_LEFT | B_BUTTON)

static inline void CloseOutfitMenu(u8 taskId)
{
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_CloseOutfitMenu;
}

static void Task_OutfitMenuHandleInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        gSaveBlock2Ptr->currOutfitId = sOutfitMenu->idx;
        CloseOutfitMenu(taskId);
    }

    if (JOY_NEW(B_BUTTON))
        CloseOutfitMenu(taskId);

    if (JOY_NEW(DPAD_RIGHT))
    {
        if (sOutfitMenu->idx < OUTFIT_END)
            sOutfitMenu->idx++;
        else
            sOutfitMenu->idx = OUTFIT_BEGIN;

        UpdateOutfitInfo();
        DebugPrintf("DPAD_RIGHT, sOutfitMenu->idx = %d", sOutfitMenu->idx);
    }

    if (JOY_NEW(DPAD_LEFT))
    {
        if (sOutfitMenu->idx != OUTFIT_BEGIN)
            sOutfitMenu->idx--;
        else
            sOutfitMenu->idx = OUTFIT_END;

        UpdateOutfitInfo();
        DebugPrintf("DPAD_LEFT, sOutfitMenu->idx = %d", sOutfitMenu->idx);
    }

    if (JOY_NEW(ANY_BUTTONS))
        PlaySE(SE_RG_BAG_CURSOR);
}

static void FreeOutfitMenuResources(void)
{
    DestroySprite(&gSprites[sOutfitMenu->spriteIds[GFX_OW]]);
    FreeAndDestroyTrainerPicSprite(sOutfitMenu->spriteIds[GFX_TS]);
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
        PlaySE(SE_RG_HELP_CLOSE);
        SetMainCallback2(sOutfitMenu->retCB);
        FreeOutfitMenuResources();
        DestroyTask(taskId);
    }
}
