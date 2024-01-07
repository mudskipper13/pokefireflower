#ifndef GUARD_OUTFIT_MENU_H
#define GUARD_OUTFIT_MENU_H

void OpenOutfitMenu(MainCallback retCB);
void Task_OpenOutfitMenu(u8 taskId);

//! misc funcs
void UnlockOutfit(u8 outfitId);
u16 GetOutfitData(u8 outfitId, u8 dataType);
void BufferOutfitStrings(u8 *dest, u8 outfitId, u8 dataType);
u32 GetPlayerTrainerPicIdByOutfitGenderType(u32 outfitId, u32 gender, bool32 type);
const u16 *GetPlayerBattleTransitionMugshotPalette(void);
const void *GetPlayerHeadGfxOrPal(u8 which, bool32 isFP);

#endif //! GUARD_OUTFIT_MENU_H
