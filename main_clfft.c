#include <stdio.h>
#include <stdlib.h>
#include <wand/magick_wand.h>
#include "imageio.h"
#include "process.h"
#include "utility.h"

int main(int argc, char *argv[])
{
    int i, exit_status = 0;
    MagickPassFail status = MagickPass;
    MagickWand *magick_wand = NULL;

    /* 必要になったらオプション処理 */

    prepare_wand(argv[0], &magick_wand);
    for (i = 1; i < argc; i++) {
        status = process_image_clfft(magick_wand, argv[i]);
        if (status == MagickFail) {
            exit_status = 1;
        }
    }
    terminate_wand(magick_wand);

    return (exit_status);
}

MagickPassFail
process_image_clfft(MagickWand *magick_wand, const char *filename)
{
    MagickPassFail status = MagickPass;
    const char fftstr[] = "_clfft";
    char *fftfilename = NULL;

    if (status == MagickPass) {
        status = read_and_grayscale_image(magick_wand, filename);
        if (status == MagickFail) {
            fprintf(stderr, "Converting %s to a grayscale image failed.\n", filename);
        }
    }
    if (status == MagickPass) {
        status = string_add_tail(filename, fftstr, &fftfilename);
    }
    if (status == MagickPass) {
        printf("clFFT image: %s\n", filename);
        status = execute_fft_clfft(magick_wand, fftfilename);
        if (status == MagickFail) {
            fprintf(stderr, "Executing clFFT on %s failed.\n", filename);
        }
        free(fftfilename);
        fftfilename = NULL;
    }

    return (status);
}
