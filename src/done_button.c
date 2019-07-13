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
#include "done_button.h"

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

#define TRY_INC_GAME_STAT(saveBlock, statName, max)              \
do {                                                             \
    if(gSaveBlock##saveBlock##Ptr->doneButtonStats.statName < max)    \
        gSaveBlock##saveBlock##Ptr->doneButtonStats.statName++;       \
}while(0)
    
// max is unused, copy paste from other macro
#define GET_GAME_STAT(saveBlock, statName, max)                  \
do {                                                             \
    return gSaveBlock##saveBlock##Ptr->doneButtonStats.statName; \
}while(0)

// UINT_MAX for u32
// USHRT_MAX for u16

void TryIncrementButtonStat(enum DoneButtonStat stat)
{
    switch(stat)
    {
        // DoneButtonStats1
        case DB_FRAME_COUNT_TOTAL:
            TRY_INC_GAME_STAT(1, frameCount, UINT_MAX);
            break;
        case DB_FRAME_COUNT_OW:
            TRY_INC_GAME_STAT(1, owFrameCount, UINT_MAX);
            break;
        case DB_FRAME_COUNT_BATTLE:
            TRY_INC_GAME_STAT(1, battleFrameCount, UINT_MAX);
            break;
        case DB_FRAME_COUNT_MENU:
            TRY_INC_GAME_STAT(1, menuFrameCount, UINT_MAX);
            break;
        case DB_FRAME_COUNT_INTROS: // This needs special handling due to intro not having loaded save block yet.
            TRY_INC_GAME_STAT(1, introsFrameCount, UINT_MAX);
            break;
        case DB_SAVE_COUNT:
            TRY_INC_GAME_STAT(1, saveCount, USHRT_MAX);
            break;
        case DB_RELOAD_COUNT:
            TRY_INC_GAME_STAT(1, reloadCount, USHRT_MAX);
            break;
        case DB_CLOCK_RESET_COUNT:
            TRY_INC_GAME_STAT(1, clockResetCount, USHRT_MAX);
            break;
        case DB_STEP_COUNT:
            TRY_INC_GAME_STAT(1, stepCount, UINT_MAX);
            break;
        case DB_STEP_COUNT_WALK:
            TRY_INC_GAME_STAT(1, stepCountWalk, UINT_MAX);
            break;
        case DB_STEP_COUNT_SURF:
            TRY_INC_GAME_STAT(1, stepCountSurf, UINT_MAX);
            break;
        case DB_STEP_COUNT_BIKE:
            TRY_INC_GAME_STAT(1, stepCountBike, UINT_MAX);
            break;
        case DB_STEP_COUNT_RUN:
            TRY_INC_GAME_STAT(1, stepCountRun, UINT_MAX);
            break;
        case DB_BONKS:
            TRY_INC_GAME_STAT(1, bonks, USHRT_MAX);
            break;
        case DB_TOTAL_DAMAGE_DEALT:
            TRY_INC_GAME_STAT(1, totalDamageDealt, UINT_MAX);
            break;
        case DB_ACTUAL_DAMAGE_DEALT:
            TRY_INC_GAME_STAT(1, actualDamageDealt, UINT_MAX);
            break;
        case DB_TOTAL_DAMAGE_TAKEN:
            TRY_INC_GAME_STAT(1, totalDamageTaken, UINT_MAX);
            break;
        case DB_ACTUAL_DAMAGE_TAKEN:
            TRY_INC_GAME_STAT(1, actualDamageTaken, UINT_MAX);
            break;
        case DB_OWN_MOVES_HIT:
            TRY_INC_GAME_STAT(1, ownMovesHit, USHRT_MAX);
            break;
        case DB_OWN_MOVES_MISSED:
            TRY_INC_GAME_STAT(1, ownMovesMissed, USHRT_MAX);
            break;
        case DB_ENEMY_MOVES_HIT:
            TRY_INC_GAME_STAT(1, enemyMovesHit, USHRT_MAX);
            break;
        case DB_ENEMY_MOVES_MISSED:
            TRY_INC_GAME_STAT(1, enemyMovesMissed, USHRT_MAX);
            break;
        // DoneButtonStats2
        case DB_OWN_MOVES_SE:
            TRY_INC_GAME_STAT(2, ownMovesSE, USHRT_MAX);
            break;
        case DB_OWN_MOVES_NVE:
            TRY_INC_GAME_STAT(2, ownMovesNVE, USHRT_MAX);
            break;
        case DB_ENEMY_MOVES_SE:
            TRY_INC_GAME_STAT(2, enemyMovesSE, USHRT_MAX);
            break;
        case DB_ENEMY_MOVES_NVE:
            TRY_INC_GAME_STAT(2, enemyMovesNVE, USHRT_MAX);
            break;
        case DB_CRITS_DEALT:
            TRY_INC_GAME_STAT(2, critsDealt, USHRT_MAX);
            break;
        case DB_OHKOS_DEALT:
            TRY_INC_GAME_STAT(2, OHKOsDealt, USHRT_MAX);
            break;
        case DB_CRITS_TAKEN:
            TRY_INC_GAME_STAT(2, critsTaken, USHRT_MAX);
            break;
        case DB_OHKOS_TAKEN:
            TRY_INC_GAME_STAT(2, OHKOsTaken, USHRT_MAX);
            break;
        case DB_PLAYER_HP_HEALED:
            TRY_INC_GAME_STAT(2, playerHPHealed, UINT_MAX);
            break;
        case DB_ENEMY_HP_HEALED:
            TRY_INC_GAME_STAT(2, enemyHPHealed, UINT_MAX);
            break;
        case DB_PLAYER_POKEMON_FAINTED:
            TRY_INC_GAME_STAT(2, playerPokemonFainted, USHRT_MAX);
            break;
        case DB_ENEMY_POKEMON_FAINTED:
            TRY_INC_GAME_STAT(2, enemyPokemonFainted, USHRT_MAX);
            break;
        case DB_EXP_GAINED:
            TRY_INC_GAME_STAT(2, expGained, UINT_MAX);
            break;
        case DB_SWITCHOUTS:
            TRY_INC_GAME_STAT(2, switchouts, USHRT_MAX);
            break;
        case DB_BATTLES:
            TRY_INC_GAME_STAT(2, battles, USHRT_MAX);
            break;
        case DB_TRAINER_BATTLES:
            TRY_INC_GAME_STAT(2, trainerBattles, USHRT_MAX);
            break;
        case DB_WILD_BATTLES:
            TRY_INC_GAME_STAT(2, wildBattles, USHRT_MAX);
            break;
        case DB_BATTLES_FLED:
            TRY_INC_GAME_STAT(2, battlesFled, USHRT_MAX);
            break;
        case DB_FAILED_RUNS:
            TRY_INC_GAME_STAT(2, failedRuns, USHRT_MAX);
            break;
        case DB_MONEY_MADE:
            TRY_INC_GAME_STAT(2, moneyMade, UINT_MAX);
            break;
        case DB_MONEY_SPENT:
            TRY_INC_GAME_STAT(2, moneySpent, UINT_MAX);
            break;
        case DB_MONEY_LOST:
            TRY_INC_GAME_STAT(2, moneyLost, UINT_MAX);
            break;
        case DB_ITEMS_PICKED_UP:
            TRY_INC_GAME_STAT(2, itemsPickedUp, USHRT_MAX);
            break;
        case DB_ITEMS_BOUGHT:
            TRY_INC_GAME_STAT(2, itemsBought, USHRT_MAX);
            break;
        case DB_ITEMS_SOLD:
            TRY_INC_GAME_STAT(2, itemsSold, USHRT_MAX);
            break;
        case DB_MOVES_LEARNT:
            TRY_INC_GAME_STAT(2, movesLearnt, USHRT_MAX);
            break;
        case DB_BALLS_THROWN:
            TRY_INC_GAME_STAT(2, ballsThrown, USHRT_MAX);
            break;
        case DB_POKEMON_CAUGHT_IN_BALLS:
            TRY_INC_GAME_STAT(2, pokemonCaughtInBalls, USHRT_MAX);
            break;
    }
}

u32 GetDoneButtonStat(enum DoneButtonStat stat)
{
    switch(stat)
    {
        // DoneButtonStats1
        case DB_FRAME_COUNT_TOTAL:
            GET_GAME_STAT(1, frameCount, UINT_MAX);
            break;
        case DB_FRAME_COUNT_OW:
            GET_GAME_STAT(1, owFrameCount, UINT_MAX);
            break;
        case DB_FRAME_COUNT_BATTLE:
            GET_GAME_STAT(1, battleFrameCount, UINT_MAX);
            break;
        case DB_FRAME_COUNT_MENU:
            GET_GAME_STAT(1, menuFrameCount, UINT_MAX);
            break;
        case DB_FRAME_COUNT_INTROS: // This needs special handling due to intro not having loaded save block yet.
            GET_GAME_STAT(1, introsFrameCount, UINT_MAX);
            break;
        case DB_SAVE_COUNT:
            GET_GAME_STAT(1, saveCount, USHRT_MAX);
            break;
        case DB_RELOAD_COUNT:
            GET_GAME_STAT(1, reloadCount, USHRT_MAX);
            break;
        case DB_CLOCK_RESET_COUNT:
            GET_GAME_STAT(1, clockResetCount, USHRT_MAX);
            break;
        case DB_STEP_COUNT:
            GET_GAME_STAT(1, stepCount, UINT_MAX);
            break;
        case DB_STEP_COUNT_WALK:
            GET_GAME_STAT(1, stepCountWalk, UINT_MAX);
            break;
        case DB_STEP_COUNT_SURF:
            GET_GAME_STAT(1, stepCountSurf, UINT_MAX);
            break;
        case DB_STEP_COUNT_BIKE:
            GET_GAME_STAT(1, stepCountBike, UINT_MAX);
            break;
        case DB_STEP_COUNT_RUN:
            GET_GAME_STAT(1, stepCountRun, UINT_MAX);
            break;
        case DB_BONKS:
            GET_GAME_STAT(1, bonks, USHRT_MAX);
            break;
        case DB_TOTAL_DAMAGE_DEALT:
            GET_GAME_STAT(1, totalDamageDealt, UINT_MAX);
            break;
        case DB_ACTUAL_DAMAGE_DEALT:
            GET_GAME_STAT(1, actualDamageDealt, UINT_MAX);
            break;
        case DB_TOTAL_DAMAGE_TAKEN:
            GET_GAME_STAT(1, totalDamageTaken, UINT_MAX);
            break;
        case DB_ACTUAL_DAMAGE_TAKEN:
            GET_GAME_STAT(1, actualDamageTaken, UINT_MAX);
            break;
        case DB_OWN_MOVES_HIT:
            GET_GAME_STAT(1, ownMovesHit, USHRT_MAX);
            break;
        case DB_OWN_MOVES_MISSED:
            GET_GAME_STAT(1, ownMovesMissed, USHRT_MAX);
            break;
        case DB_ENEMY_MOVES_HIT:
            GET_GAME_STAT(1, enemyMovesHit, USHRT_MAX);
            break;
        case DB_ENEMY_MOVES_MISSED:
            GET_GAME_STAT(1, enemyMovesMissed, USHRT_MAX);
            break;
        // DoneButtonStats2
        case DB_OWN_MOVES_SE:
            GET_GAME_STAT(2, ownMovesSE, USHRT_MAX);
            break;
        case DB_OWN_MOVES_NVE:
            GET_GAME_STAT(2, ownMovesNVE, USHRT_MAX);
            break;
        case DB_ENEMY_MOVES_SE:
            GET_GAME_STAT(2, enemyMovesSE, USHRT_MAX);
            break;
        case DB_ENEMY_MOVES_NVE:
            GET_GAME_STAT(2, enemyMovesNVE, USHRT_MAX);
            break;
        case DB_CRITS_DEALT:
            GET_GAME_STAT(2, critsDealt, USHRT_MAX);
            break;
        case DB_OHKOS_DEALT:
            GET_GAME_STAT(2, OHKOsDealt, USHRT_MAX);
            break;
        case DB_CRITS_TAKEN:
            GET_GAME_STAT(2, critsTaken, USHRT_MAX);
            break;
        case DB_OHKOS_TAKEN:
            GET_GAME_STAT(2, OHKOsTaken, USHRT_MAX);
            break;
        case DB_PLAYER_HP_HEALED:
            GET_GAME_STAT(2, playerHPHealed, UINT_MAX);
            break;
        case DB_ENEMY_HP_HEALED:
            GET_GAME_STAT(2, enemyHPHealed, UINT_MAX);
            break;
        case DB_PLAYER_POKEMON_FAINTED:
            GET_GAME_STAT(2, playerPokemonFainted, USHRT_MAX);
            break;
        case DB_ENEMY_POKEMON_FAINTED:
            GET_GAME_STAT(2, enemyPokemonFainted, USHRT_MAX);
            break;
        case DB_EXP_GAINED:
            GET_GAME_STAT(2, expGained, UINT_MAX);
            break;
        case DB_SWITCHOUTS:
            GET_GAME_STAT(2, switchouts, USHRT_MAX);
            break;
        case DB_BATTLES:
            GET_GAME_STAT(2, battles, USHRT_MAX);
            break;
        case DB_TRAINER_BATTLES:
            GET_GAME_STAT(2, trainerBattles, USHRT_MAX);
            break;
        case DB_WILD_BATTLES:
            GET_GAME_STAT(2, wildBattles, USHRT_MAX);
            break;
        case DB_BATTLES_FLED:
            GET_GAME_STAT(2, battlesFled, USHRT_MAX);
            break;
        case DB_FAILED_RUNS:
            GET_GAME_STAT(2, failedRuns, USHRT_MAX);
            break;
        case DB_MONEY_MADE:
            GET_GAME_STAT(2, moneyMade, UINT_MAX);
            break;
        case DB_MONEY_SPENT:
            GET_GAME_STAT(2, moneySpent, UINT_MAX);
            break;
        case DB_MONEY_LOST:
            GET_GAME_STAT(2, moneyLost, UINT_MAX);
            break;
        case DB_ITEMS_PICKED_UP:
            GET_GAME_STAT(2, itemsPickedUp, USHRT_MAX);
            break;
        case DB_ITEMS_BOUGHT:
            GET_GAME_STAT(2, itemsBought, USHRT_MAX);
            break;
        case DB_ITEMS_SOLD:
            GET_GAME_STAT(2, itemsSold, USHRT_MAX);
            break;
        case DB_MOVES_LEARNT:
            GET_GAME_STAT(2, movesLearnt, USHRT_MAX);
            break;
        case DB_BALLS_THROWN:
            GET_GAME_STAT(2, ballsThrown, USHRT_MAX);
            break;
        case DB_POKEMON_CAUGHT_IN_BALLS:
            GET_GAME_STAT(2, pokemonCaughtInBalls, USHRT_MAX);
            break;
    }
}

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
        if(items[i].printfn && items[i].name)
        {
            AddTextPrinterParameterized(0, 1, items[i].name, 1, 24 * i + 1, -1, NULL);
            value_s = items[i].printfn();
            width = GetStringWidth(0, value_s, 0);
            AddTextPrinterParameterized(0, 1, items[i].name, 220 - width, 24 * i + 9, -1, NULL);
        }
    }
}
