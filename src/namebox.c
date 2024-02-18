#include "global.h"
#include "script.h"
#include "event_scripts.h"
#include "namebox.h"
#include "constants/namebox.h"

// expandable
// we need to include the _ from the arg because it could clash with existing defines
// e.g. FEMALE & _FEMALE
#define NAME(_name, _color, ...) { .name = Common_Names_ ## _name, .color = NPC_TEXT_COLOR ## _color, .buffer = DEFAULT(FALSE, __VA_ARGS__) }

const struct Names gNameInfo[] = {
    [NAME_PLAYER] = NAME(Player, _PLAYER, TRUE),
    [NAME_MOM] = NAME(Mom, _FEMALE),
    [NAME_RIVAL] = NAME(Rival, _RIVAL),
    [NAME_GRAMPA] = NAME(Grampa, _MALE),
    [NAME_NAVY] = NAME(Navy, _MALE),
    [NAME_MEL] = NAME(Mel, _FEMALE),
    [NAME_MEI] = NAME(Mei, _FEMALE),
};
