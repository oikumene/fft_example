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
        status = process_image_fftw(magick_wand, argv[i]);
        if (status == MagickFail) {
            exit_status = 1;
        }
    }
    terminate_wand(magick_wand);

    return (exit_status);
}

MagickPassFail
process_image_fftw(MagickWand *magick_wand, const char *filename)
{
    MagickPassFail status = MagickPass;
    const char fftstr1[] = "_fft1";
    const char fftstr6[] = "_fft6";
    char *fftfilename = NULL;

    if (status == MagickPass) {
        status = read_and_grayscale_image(magick_wand, filename);
        if (status == MagickFail) {
            fprintf(stderr, "Converting %s to a grayscale image failed.\n", filename);
        }
    }
    if (status == MagickPass) {
        status = string_add_tail(filename, fftstr1, &fftfilename);
    }
    if (status == MagickPass) {
        printf("FFTW(double) image (nthread = 1): %s\n", filename);
        status = execute_fft_fftw(magick_wand, fftfilename, 1);
        if (status == MagickFail) {
            fprintf(stderr, "Executing FFT(nthread = 1) on %s failed.\n", filename);
        }
        free(fftfilename);
        fftfilename = NULL;
    }
    if (status == MagickPass) {
        status = string_add_tail(filename, fftstr6, &fftfilename);
    }
    if (status == MagickPass) {
        printf("FFTW(double) image (nthread = 6): %s\n", filename);
        status = execute_fft_fftw(magick_wand, fftfilename, 6);
        if (status == MagickFail) {
            fprintf(stderr, "Executing FFT(nthreads = 6) on %s failed.\n", filename);
        }
        free(fftfilename);
        fftfilename = NULL;
    }

    return (status);
}
