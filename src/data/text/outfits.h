static const u8 sText_OutfitName_OutfitUsualGreen[] = _("USUAL GREEN");
static const u8 sText_OutfitDesc_OutfitUsualGreen[] = _(
    "The usual, but basic OUTFIT."
);

static const u8 sText_OutfitName_OutfitUnusualRed[] = _("UNUSUAL RED");
static const u8 sText_OutfitDesc_OutfitUnusualRed[] = _(
    "Rather unusual, but still basic OUTFIT."
);

const u8 *const gOutfitNameDescTables[OUTFIT_COUNT][2] = {
    {
        sText_OutfitName_OutfitUsualGreen,
        sText_OutfitDesc_OutfitUsualGreen,
    },
    {
        sText_OutfitName_OutfitUnusualRed,
        sText_OutfitDesc_OutfitUnusualRed,
    },
};
