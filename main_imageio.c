#include <stdio.h>
#include <wand/magick_wand.h>
#include "imageio.h"
#include "process.h"

int main(int argc, char *argv[])
{
    int i, exit_status = 0;
    MagickPassFail status = MagickPass;
    MagickWand *magick_wand = NULL;

    /* 必要になったらオプション処理 */

    prepare_wand(argv[0], &magick_wand);
    for (i = 1; i < argc; i++) {
        status = process_image_imageio(magick_wand, argv[i]);
        if (status == MagickFail) {
            exit_status = 1;
        }
    }
    terminate_wand(magick_wand);

    return (exit_status);
}

MagickPassFail
process_image_imageio(MagickWand *magick_wand, const char *filename)
{
    MagickPassFail status = MagickPass;

    status = read_and_grayscale_image(magick_wand, filename);
    if (status == MagickFail) {
        fprintf(stderr, "Converting %s to a grayscale image failed.\n", filename);
    }
    return (status);
}
