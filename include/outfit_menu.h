#ifndef GUARD_OUTFIT_MENU_H
#define GUARD_OUTFIT_MENU_H

void OpenOutfitMenu(MainCallback retCB);
void Task_OpenOutfitMenu(u8 taskId);

//! misc script funcs
void UnlockOutfit(u8 outfitId);
u16 GetOutfitData(u8 outfitId, u8 dataType);
void BufferOutfitStrings(u8 *dest, u8 outfitId, u8 dataType);

#endif //! GUARD_OUTFIT_MENU_H
