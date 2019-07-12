#ifndef _GUARD_DONE_BUTTON_H
#define _GUARD_DONE_BUTTON_H

// The done stats are defined in global.h.

enum DoneButtonStat
{
    DONE_BUTTON_FRAME_COUNT_TOTAL,
    DONE_BUTTON_FRAME_COUNT_OW,
    //... TODO, FINISH ME
};

u32 TryIncrementButtonStat(enum DoneButtonStat stat);
u32 GetDoneButtonStat(enum DoneButtonStat stat);

#endif // _GUARD_DONE_BUTTON_H
