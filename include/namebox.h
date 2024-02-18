#ifndef GUARD_NAMEBOX_H
#define GUARD_NAMEBOX_H

struct Names
{
    const u8 *name;
    // this is kind of redundant since we use
    // AddTextPrinterDiffStyle for namebox, but
    // at least no more setting the color manually.
    u8 color;
    bool8 buffer; // the name is actually stored somewhere else
};

extern const struct Names gNameInfo[];

#endif // GUARD_NAMEBOX_H
