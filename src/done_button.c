#include "global.h"
#include "task.h"
#include "text.h"
#include "menu.h"
#include "menu_helpers.h"
#include "item_menu.h"
#include "overworld.h"

// TODO: Implement Done Button Screen.
// Basically, a lot of work.

extern void BagMenu_InitListsMenu(u8 taskId);
extern void DisplayItemMessage(u8 taskId, u8 fontId, const u8 *str, void ( *callback)(u8 taskId));

static const u8 gText_AreYouActuallyDoneWithRace[] = _("TODO: Implement this.");

static void DoneButtonTask01(u8 taskId);
static void DoneButtonTask02(u8 taskId);
static void LaunchDoneButton(void);

static const struct YesNoFuncTable DoneButtonYesNoFuncTable = {
    .yesFunc = DoneButtonTask02,
    .noFunc = BagMenu_InitListsMenu
};

void ItemUseOutOfBattle_DoneButton(u8 taskId)
{
    DisplayItemMessage(taskId, 1, gText_AreYouActuallyDoneWithRace, DoneButtonTask01);
}

static void DoneButtonTask01(u8 taskId)
{
    BagMenu_YesNo(taskId, 6, &DoneButtonYesNoFuncTable);
}

static void DoneButtonTask02(u8 taskId)
{
    gBagMenu->mainCallback2 = LaunchDoneButton;
    unknown_ItemMenu_Confirm(taskId);
}

void OpenDoneButton(int arg0, void *func)
{
    // TODO
}

static void LaunchDoneButton(void)
{
    OpenDoneButton(0, CB2_ReturnToField);
}
