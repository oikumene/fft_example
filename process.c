#include "process.h"

void prepare_wand(const char *path, MagickWand **magick_wand)
{
    InitializeMagick(path);
    *magick_wand = NewMagickWand();
}

void terminate_wand(MagickWand *magick_wand)
{
    DestroyMagickWand(magick_wand);
    DestroyMagick();
}
