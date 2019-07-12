#include "global.h"
#include "main.h"
#include "task.h"
#include "bg.h"
#include "window.h"
#include "palette.h"
#include "alloc.h"
#include "sound.h"
#include "constants/songs.h"
#include "constants/rgb.h"
#include "gpu_regs.h"
#include "scanline_effect.h"
#include "text.h"
#include "menu.h"

struct DoneButton
{
    MainCallback doneCallback;
    u8 taskId;
    u8 page;
    u16 tilemapBuffer[0x800];
};

#define NELEMS ARRAY_COUNT

static EWRAM_DATA struct DoneButton *doneButton = NULL;

static void DoneButtonCB(void);
static void PrintGameStatsPage(void);
static void Task_DoneButton(u8 taskId);
static void Task_DestroyDoneButton(u8 taskId);

struct DoneButtonLineItem
{
    const u8 * name;
    const u8 * (*printfn)(void); // string formatter for each type.
};


const u8 *GetStringSample(void)
{
    return NULL; // TODO
}

const u8 gTODOString[] = _("TODO");

const struct DoneButtonLineItem sLineItems[8][8] = {
    { // PAGE 1 (TODO)
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL}
    },
    { // PAGE 2 (TODO)
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL}
    },
    { // PAGE 3 (TODO)
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL}
    },
    { // PAGE 4 (TODO)
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL}
    },
    { // PAGE 5 (TODO)
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL}
    },
    { // PAGE 6 (TODO)
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL}
    },
    { // PAGE 7 (TODO)
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL}
    },
    { // PAGE 8 (TODO)
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL},
        {gTODOString, NULL}
    }
};

#define NPAGES (NELEMS(sLineItems))

static const struct BgTemplate sDiplomaBgTemplates[2] =
{
    {
        .bg = 0,
        .charBaseIndex = 1,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 6,
        .screenSize = 1,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0,
    },
};

static const struct WindowTemplate sWinTemplates[2] =
{
    {
        .bg = 0,
        .tilemapLeft = 5,
        .tilemapTop = 2,
        .width = 20,
        .height = 16,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    DUMMY_WIN_TEMPLATE,
};

void OpenDoneButton(MainCallback doneCallback)
{
    doneButton = AllocZeroed(sizeof(*doneButton));
    if (doneButton == NULL)
        SetMainCallback2(doneCallback);
    else
    {
        doneButton->doneCallback = doneCallback;
        doneButton->taskId = 0xFF;
        doneButton->page = 0;
        SetMainCallback2(DoneButtonCB);
    }
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

extern const struct BgTemplate sMainMenuBgTemplates[];
extern const struct WindowTemplate sSpeedchoiceMenuWinTemplates[];

void DoneButtonCB(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        SetGpuReg(REG_OFFSET_BG3CNT, 0);
        SetGpuReg(REG_OFFSET_BG2CNT, 0);
        SetGpuReg(REG_OFFSET_BG1CNT, 0);
        SetGpuReg(REG_OFFSET_BG0CNT, 0);
        SetGpuReg(REG_OFFSET_BG3HOFS, 0);
        SetGpuReg(REG_OFFSET_BG3VOFS, 0);
        SetGpuReg(REG_OFFSET_BG2HOFS, 0);
        SetGpuReg(REG_OFFSET_BG2VOFS, 0);
        SetGpuReg(REG_OFFSET_BG1HOFS, 0);
        SetGpuReg(REG_OFFSET_BG1VOFS, 0);
        SetGpuReg(REG_OFFSET_BG0HOFS, 0);
        SetGpuReg(REG_OFFSET_BG0VOFS, 0);
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        ResetPaletteFade();
        FreeAllSpritePalettes();
        gMain.state++;
        break;
    case 1:
        DmaFill16(3, 0, VRAM, VRAM_SIZE);
        gMain.state++;
        break;
    case 2:
        DmaFill32(3, 0, OAM, OAM_SIZE);
        gMain.state++;
        break;
    case 3:
        DmaFill16(3, 0, PLTT, PLTT_SIZE);
        gMain.state++;
        break;
    case 4:
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sMainMenuBgTemplates, 2);
        SetBgTilemapBuffer(1, doneButton->tilemapBuffer);
        InitWindows(sSpeedchoiceMenuWinTemplates);
        DeactivateAllTextPrinters();
        FillWindowPixelBuffer(0, PIXEL_FILL(0));
        PutWindowTilemap(0);
        reset_temp_tile_data_buffers();
        gMain.state++;
        break;
    case 5:
        PrintGameStatsPage();
        PlayBGM(MUS_FIELD13);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    case 6:
        if (!gPaletteFade.active)
        {
            ShowBg(0);
            ShowBg(1);
            SetGpuReg(REG_OFFSET_BLDCNT, 0);
            SetGpuReg(REG_OFFSET_BLDALPHA, 0);
            SetGpuReg(REG_OFFSET_BLDY, 0);
            SetVBlankCallback(VBlankCB);
            SetMainCallback2(MainCB2);
            doneButton->taskId = CreateTask(Task_DoneButton, 0);
        }
        break;
    }
}

static void Task_DoneButton(u8 taskId)
{
    struct DoneButton *data = doneButton;

    if (JOY_NEW(DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        if (++data->page >= NPAGES)
            data->page = 0;
        PrintGameStatsPage();
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        PlaySE(SE_SELECT);
        if (data->page == 0)
            data->page = NPAGES - 1;
        else
            data->page--;
        PrintGameStatsPage();
    }
    else if (JOY_NEW(A_BUTTON | B_BUTTON | START_BUTTON))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].func = Task_DestroyDoneButton;
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
    }
}

static void Task_DestroyDoneButton(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        FreeAllWindowBuffers();
        SetMainCallback2(doneButton->doneCallback);
        Free(doneButton);
        DestroyTask(taskId);
    }
}

static void PrintGameStatsPage(void)
{
    const struct DoneButtonLineItem * items = sLineItems[doneButton->page];
    s32 i;

    FillWindowPixelBuffer(0, PIXEL_FILL(0));
    for (i = 0; i < 8; i++)
    {
        s32 width;
        const char * value_s;
        AddTextPrinterParameterized(0, 1, items[i].name, 1, 24 * i + 1, -1, NULL);
        value_s = items[i].printfn();
        width = GetStringWidth(0, value_s, 0);
        AddTextPrinterParameterized(0, 1, items[i].name, 220 - width, 24 * i + 9, -1, NULL);
    }
}
