#include <stdlib.h>
#include <string.h>
#include "imageio.h"
#include "utility.h"

MagickPassFail read_and_grayscale_image(
        MagickWand *magick_wand,
        const char *filename)
{
    const char addstr[] = "_gray";
    char *grayfilename = NULL;
    MagickPassFail status = MagickPass;

    if (status == MagickPass) {
        status = string_add_tail(filename, addstr, &grayfilename);
    }
    if (status == MagickPass) {
        status = MagickReadImage(magick_wand, filename);
    }
    if (status == MagickPass) {
        status = MagickSetImageColorspace(magick_wand, GRAYColorspace);
    }
    if (status == MagickPass) {
        status = MagickWriteImage(magick_wand, grayfilename);
    }

    free(grayfilename);
    grayfilename = NULL;
    return (status);
}
