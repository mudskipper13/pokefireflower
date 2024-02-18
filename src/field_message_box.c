#include "global.h"
#include "menu.h"
#include "string_util.h"
#include "task.h"
#include "event_data.h"
#include "text.h"
#include "match_call.h"
#include "field_message_box.h"
#include "text_window.h"
#include "script.h"
#include "event_scripts.h"
#include "namebox.h"
#include "constants/namebox.h"

static EWRAM_DATA u8 sFieldMessageBoxMode = 0;
EWRAM_DATA const u8 *gSpeakerName = NULL;

static void ExpandStringAndStartDrawFieldMessage(const u8 *, bool32);
static void StartDrawFieldMessage(void);

void InitFieldMessageBox(void)
{
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
    gTextFlags.canABSpeedUpPrint = FALSE;
    gTextFlags.useAlternateDownArrow = FALSE;
    gTextFlags.autoScroll = FALSE;
    gTextFlags.forceMidTextSpeed = FALSE;
}

#define tState data[0]

static void Task_DrawFieldMessage(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
        case 0:
            if (IsMsgSignpost())
                LoadSignpostWindowFrameGfx();
            else
                LoadMessageBoxAndBorderGfx();
            task->tState++;
            break;
        case 1:
            DrawDialogueFrame(0, TRUE);
            if (gSpeakerName != NULL && !FlagGet(FLAG_SUPPRESS_SPEAKER_NAME) && !IsMsgSignpost())
                DrawNameplateFrame(1, FALSE);
            task->tState++;
            break;
        case 2:
            if (RunTextPrintersAndIsPrinter0Active() != TRUE)
            {
                sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
                DestroyTask(taskId);
            }
    }
}

#undef tState

static void CreateTask_DrawFieldMessage(void)
{
    CreateTask(Task_DrawFieldMessage, 0x50);
}

static void DestroyTask_DrawFieldMessage(void)
{
    u8 taskId = FindTaskIdByFunc(Task_DrawFieldMessage);
    if (taskId != TASK_NONE)
        DestroyTask(taskId);
}

bool8 ShowFieldMessage(const u8 *str)
{
    if (sFieldMessageBoxMode != FIELD_MESSAGE_BOX_HIDDEN)
        return FALSE;
    ExpandStringAndStartDrawFieldMessage(str, TRUE);
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_NORMAL;
    return TRUE;
}

static void Task_HidePokenavMessageWhenDone(u8 taskId)
{
    if (!IsMatchCallTaskActive())
    {
        sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
        DestroyTask(taskId);
    }
}

bool8 ShowPokenavFieldMessage(const u8 *str)
{
    if (sFieldMessageBoxMode != FIELD_MESSAGE_BOX_HIDDEN)
        return FALSE;
    StringExpandPlaceholders(gStringVar4, str);
    CreateTask(Task_HidePokenavMessageWhenDone, 0);
    StartMatchCallFromScript(str);
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_NORMAL;
    return TRUE;
}

bool8 ShowFieldAutoScrollMessage(const u8 *str)
{
    if (sFieldMessageBoxMode != FIELD_MESSAGE_BOX_HIDDEN)
        return FALSE;
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_AUTO_SCROLL;
    ExpandStringAndStartDrawFieldMessage(str, FALSE);
    return TRUE;
}

static bool8 UNUSED ForceShowFieldAutoScrollMessage(const u8 *str)
{
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_AUTO_SCROLL;
    ExpandStringAndStartDrawFieldMessage(str, TRUE);
    return TRUE;
}

// Same as ShowFieldMessage, but instead of accepting a
// string arg it just prints whats already in gStringVar4
bool8 ShowFieldMessageFromBuffer(void)
{
    if (sFieldMessageBoxMode != FIELD_MESSAGE_BOX_HIDDEN)
        return FALSE;
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_NORMAL;
    StartDrawFieldMessage();
    return TRUE;
}

static void ExpandStringAndStartDrawFieldMessage(const u8 *str, bool32 allowSkippingDelayWithButtonPress)
{
    if (gSpeakerName != NULL && !FlagGet(FLAG_SUPPRESS_SPEAKER_NAME) && !IsMsgSignpost())
    {
        int strLen = GetStringWidth(FONT_SMALL, gSpeakerName, -1);
        u32 i;

        for (i = 0; i < NAMES_COUNT; i++)
        {
            if (gSpeakerName == gNameInfo[i].name)
            {
                gSpecialVar_TextColor = gNameInfo[i].color == 0 ? NPC_TEXT_COLOR_NEUTRAL : gNameInfo[i].color;
                break;
            }
        }

        if (gNameInfo[i].buffer)
        {
            switch (i)
            {
                case NAME_PLAYER:
                    strLen = GetStringWidth(FONT_SMALL, gSaveBlock2Ptr->playerName, -1);
                    break;
                default:
                    break;
            }
        }

        if (strLen > 0)
        {
            strLen = GetDialogFramePlateWidth() / 2 - strLen / 2;
            gNamePlateBuffer[0] = EXT_CTRL_CODE_BEGIN;
            gNamePlateBuffer[1] = EXT_CTRL_CODE_CLEAR_TO;
            gNamePlateBuffer[2] = strLen;
            StringExpandPlaceholders(&gNamePlateBuffer[3], gSpeakerName);
        }
        else
            StringExpandPlaceholders(&gNamePlateBuffer[0], gSpeakerName);

        FillDialogFramePlate();
        AddTextPrinterDiffStyle(1, gNamePlateBuffer, FONT_SMALL, TEXT_SKIP_DRAW, FALSE);
        PutWindowTilemap(1);
        CopyWindowToVram(1, COPYWIN_GFX);
    }
    if (DECAP_ENABLED && DECAP_MIRRORING && !DECAP_FIELD_MSG)
    {
        gStringVar4[0] = CHAR_FIXED_CASE;
        StringExpandPlaceholders(gStringVar4+1, str);
    }
    else
        StringExpandPlaceholders(gStringVar4, str);

    AddTextPrinterDiffStyleFromBuffer(allowSkippingDelayWithButtonPress);
    CreateTask_DrawFieldMessage();
}

static void StartDrawFieldMessage(void)
{
    AddTextPrinterDiffStyleFromBuffer(TRUE);
    CreateTask_DrawFieldMessage();
}

void HideFieldMessageBox(void)
{
    DestroyTask_DrawFieldMessage();
    ClearDialogWindowAndFrame(0, TRUE);
    gSpecialVar_TextColor = NPC_TEXT_COLOR_NEUTRAL;
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
    gSpeakerName = NULL;
}

u8 GetFieldMessageBoxMode(void)
{
    return sFieldMessageBoxMode;
}

bool8 IsFieldMessageBoxHidden(void)
{
    if (sFieldMessageBoxMode == FIELD_MESSAGE_BOX_HIDDEN)
        return TRUE;
    return FALSE;
}

static void UNUSED ReplaceFieldMessageWithFrame(void)
{
    DestroyTask_DrawFieldMessage();
    DrawStdWindowFrame(0, TRUE);
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
}

void StopFieldMessage(void)
{
    DestroyTask_DrawFieldMessage();
    sFieldMessageBoxMode = FIELD_MESSAGE_BOX_HIDDEN;
}

void SetSpeakerName(const u8* name)
{
    gSpeakerName = name;
}
