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
    BG_3,
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
    WIN_COUNT,
};

enum Sprites {
    GFX_OW = 0,
    GFX_TS,
    GFX_COUNT,
};

enum {
    COLORID_NORMAL = 0,
    COLORID_RED,
    COLORID_BLUE,
    COLORID_NONE,
};

static const u8 sFontColors[][3] = { // bgColor, textColor, shadowColor
    [COLORID_NORMAL] = {TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY,  TEXT_COLOR_LIGHT_GRAY},
    [COLORID_RED]    = {TEXT_COLOR_WHITE, TEXT_COLOR_RED,        TEXT_COLOR_LIGHT_RED},
    [COLORID_BLUE]   = {TEXT_COLOR_WHITE, TEXT_COLOR_BLUE,       TEXT_COLOR_LIGHT_BLUE},
};

typedef struct {
    MainCallback retCB;
    u32 idx;
    u32 gfxState;
    u32 spriteIds[GFX_COUNT];
    u32 tilemapBuffer[BG_SCREEN_SIZE];
} OutfitMenuResources;

static const u8 sText_WouldYouLikeToUseOutfit[] =
{
    "Would you like to use\n"
    "{STR_VAR_1} outfit?"
};

static EWRAM_DATA OutfitMenuResources *sOutfitMenu = NULL;

static const struct BgTemplate sBGTemplates[] =
{
    [BG_0] =
    { //! UI
        .baseTile = 0,
        .bg = 0,
        .charBaseIndex = 1,
        .mapBaseIndex = 29,
        .paletteMode = 0,
        .priority = 0,
        .screenSize = 0,
    },
    [BG_1] =
    { //! BG
        .baseTile = 0,
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .paletteMode = 0,
        .priority = 1,
        .screenSize = 0,
    }
};

static const struct WindowTemplate sWindowTemplates[] =
{
    [WIN_NAME] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 12,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x20,
    },
    [WIN_DESC] =
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 26,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x40,
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
    sOutfitMenu = AllocZeroed(sizeof(OutfitMenuResources));
    if (sOutfitMenu == NULL)
    {
        // Alloc failed, exit
        SetMainCallback2(retCB);
        return;
    }
    sOutfitMenu->idx = gSaveBlock2Ptr->currOutfitId;
    sOutfitMenu->retCB = retCB;
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
}

static void SetupOutfitMenu_BGs(void)
{
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    CpuFastCopy(0, sOutfitMenu->tilemapBuffer, BG_SCREEN_SIZE);
    ResetBgsAndClearDma3BusyFlags(0);
    ChangeBgX(BG_0, 0, BG_COORD_SET);
    ChangeBgY(BG_0, 0, BG_COORD_SET);
    ChangeBgX(BG_1, 0, BG_COORD_SET);
    ChangeBgY(BG_1, 0, BG_COORD_SET);
    ChangeBgX(BG_2, 0, BG_COORD_SET);
    ChangeBgY(BG_2, 0, BG_COORD_SET);
    ChangeBgX(BG_3, 0, BG_COORD_SET);
    ChangeBgY(BG_3, 0, BG_COORD_SET);
    InitBgsFromTemplates(0, sBGTemplates, ARRAY_COUNT(sBGTemplates));
    SetBgTilemapBuffer(BG_1, sOutfitMenu->tilemapBuffer);
    ScheduleBgCopyTilemapToVram(BG_1);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(BG_0);
    ShowBg(BG_1);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_BLDALPHA, 0);
    SetGpuReg(REG_OFFSET_BLDY, 0);
}

static bool32 SetupOutfitMenu_Graphics(void)
{
    switch(sOutfitMenu->gfxState)
    {
    case 0:
        ResetTempTileDataBuffers();
        sOutfitMenu->gfxState++;
        break;
    case 1:
        LoadMessageBoxGfx(0, 0x100, BG_PLTT_ID(13));
        LoadUserWindowBorderGfx(0, 0x10A, BG_PLTT_ID(14));
        sOutfitMenu->gfxState++;
        break;
    case 2:
        LoadPalette(&gStandardMenuPalette, BG_PLTT_ID(15), PLTT_SIZE_4BPP);
        SetBackdropFromColor(RGB2GBA(212, 115, 106));
        sOutfitMenu->gfxState++;
        break;
    default:
        return TRUE;
    }
    return FALSE;
}

static void SetupOutfitMenu_Windows(void)
{
    u32 i;

    InitWindows(sWindowTemplates);
    DeactivateAllTextPrinters();
    for (i = 0; i < WIN_COUNT; i++)
    {
        FillWindowPixelBuffer(i, PIXEL_FILL(0));
        PutWindowTilemap(i);
    }
    ScheduleBgCopyTilemapToVram(BG_0);
}

static void SetupOutfitMenu_PrintStr(void)
{
    DrawStdFrameWithCustomTileAndPalette(WIN_NAME, FALSE, 0x10A, 14);
    DrawDialogFrameWithCustomTileAndPalette(WIN_DESC, FALSE, 0x100, 13);

    AddTextPrinterParameterized4(WIN_NAME, FONT_NORMAL, 0, 0, 0, 0, sFontColors[COLORID_NORMAL], TEXT_SKIP_DRAW, gOutfitNameDescTables[sOutfitMenu->idx][0]);
    AddTextPrinterParameterized4(WIN_DESC, FONT_NORMAL, 0, 0, 0, 0, sFontColors[COLORID_NORMAL], TEXT_SKIP_DRAW, gOutfitNameDescTables[sOutfitMenu->idx][1]);

    CopyWindowToVram(WIN_NAME, COPYWIN_FULL);
    CopyWindowToVram(WIN_DESC, COPYWIN_FULL);
}

static void SetupOutfitMenu_Sprites_DrawOverworldSprite(bool32 update)
{
    u16 gfxId = gPlayerAvatarGfxIds[sOutfitMenu->idx][PLAYER_AVATAR_STATE_NORMAL][gSaveBlock2Ptr->playerGender];

    if (update)
        DestroySprite(&gSprites[sOutfitMenu->spriteIds[GFX_OW]]);

    sOutfitMenu->spriteIds[GFX_OW] = CreateObjectGraphicsSpriteNoTint(gfxId, SpriteCallbackDummy, 150, 40, 0);
    StartSpriteAnim(&gSprites[sOutfitMenu->spriteIds[GFX_OW]], ANIM_STD_GO_SOUTH);
}

static void SetupOutfitMenu_Sprites_DrawTrainerSprite(bool32 update)
{
    u16 id = gOutfitFrontPics[sOutfitMenu->idx][gSaveBlock2Ptr->playerGender];
    if (update)
        FreeAndDestroyTrainerPicSprite(sOutfitMenu->spriteIds[GFX_TS]);

    sOutfitMenu->spriteIds[GFX_TS] = CreateTrainerPicSprite(id, TRUE, 200, 40, 8, id);
}

static void SetupOutfitMenu_Sprites(void)
{
    SetupOutfitMenu_Sprites_DrawOverworldSprite(FALSE);
    SetupOutfitMenu_Sprites_DrawTrainerSprite(FALSE);
}

//! Similar to above, but without redrawing the frame
//! and also clean up the frame.
static void UpdateOutfitInfo(void)
{
    u32 i;

    for (i = 0; i < WIN_COUNT; i++)
    {
        FillWindowPixelBuffer(i, PIXEL_FILL(TEXT_COLOR_WHITE));
        PutWindowTilemap(i);
        AddTextPrinterParameterized4(i, FONT_NORMAL, 0, 0, 0, 0, sFontColors[COLORID_NORMAL], TEXT_SKIP_DRAW, gOutfitNameDescTables[sOutfitMenu->idx][i]);
        CopyWindowToVram(i, COPYWIN_FULL);
    }
    SetupOutfitMenu_Sprites_DrawOverworldSprite(TRUE);
    SetupOutfitMenu_Sprites_DrawTrainerSprite(TRUE);
    #ifdef DINFO
    DebugPrintf("sOutfitMenu->spriteIds[GFX_OW] = %d", sOutfitMenu->spriteIds[GFX_OW]);
    DebugPrintf("sOutfitMenu->spriteIds[GFX_TS] = %d", sOutfitMenu->spriteIds[GFX_TS]);
    #endif
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

static void Task_OutfitMenuHandleInput(u8 taskId)
{
    if (JOY_NEW(A_BUTTON))
    {
        gSaveBlock2Ptr->currOutfitId = sOutfitMenu->idx;
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
        gTasks[taskId].func = Task_CloseOutfitMenu;
    }

    if (JOY_NEW(DPAD_RIGHT))
    {
        if (sOutfitMenu->idx < OUTFIT_END)
            sOutfitMenu->idx++;
        else
            sOutfitMenu->idx = OUTFIT_BEGIN;

        UpdateOutfitInfo();
        #ifdef DINFO
        DebugPrintf("DPAD_RIGHT, sOutfitMenu->idx = %d", sOutfitMenu->idx);
        #endif
    }

    if (JOY_NEW(DPAD_LEFT))
    {
        if (sOutfitMenu->idx != OUTFIT_BEGIN)
            sOutfitMenu->idx--;
        else
            sOutfitMenu->idx = OUTFIT_END;

        UpdateOutfitInfo();
        #ifdef DINFO
        DebugPrintf("DPAD_LEFT, sOutfitMenu->idx = %d", sOutfitMenu->idx);
        #endif
    }

    if (JOY_NEW(ANY_BUTTONS))
        PlaySE(SE_RG_BAG_CURSOR);
}

static void FreeOutfitMenuResources(void)
{
    DestroySprite(&gSprites[sOutfitMenu->spriteIds[GFX_OW]]);
    FreeAndDestroyTrainerPicSprite(sOutfitMenu->spriteIds[GFX_TS]);
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
