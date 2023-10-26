static const u8 sText_OutfitName_OutfitA[] = _("BASIC A");
static const u8 sText_OutfitName_OutfitB[] = _("BASIC B");
static const u8 sText_OutfitDesc_Outfit[] = _(
    "A basic OUTFIT."
);

const u8 *const gOutfitNameDescTables[OUTFIT_COUNT][2] = {
    {
        sText_OutfitName_OutfitA,
        sText_OutfitDesc_Outfit,
    },
    {
        sText_OutfitName_OutfitB,
        sText_OutfitDesc_Outfit,
    },
};
