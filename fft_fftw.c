#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wand/magick_wand.h>
#include <fftw3.h>
#include "process.h"
#include "utility.h"

MagickPassFail
execute_fft_fftw(MagickWand *magick_wand, char *filename, int nthreads)
{
    MagickPassFail status = MagickPass;
    MagickBool ret = MagickFalse;
    size_t height = 0, width = 0;
    double *pixels = NULL;
    struct timespec start, rap, stop;
    double psec, esec, tsec;
    fftw_complex *out_data = NULL;
    fftw_plan plan = NULL;
    int i, j, index;
    double scale;
    char *tmpstr;

    /* grayscale 情報の取り出し */
    if (status == MagickPass) {
        height = MagickGetImageHeight(magick_wand);
        width = MagickGetImageWidth(magick_wand);
        pixels = (double *)malloc(sizeof(double) * height * width);
        if (pixels == NULL) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        ret = MagickGetImagePixels(magick_wand, 0, 0, width, height,
                                      "I", DoublePixel, (void *)pixels);
        if (ret == MagickFalse) {
            status = MagickFail;
        }
    }

    /* thread 準備 */
    if (status == MagickPass) {
        i = fftw_init_threads();
        if (i == 0) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        fftw_plan_with_nthreads(nthreads);
    }

    /* fftw 用データへの変換 */
    clock_gettime(CLOCK_MONOTONIC, &start);
    size_t mem_size = sizeof(fftw_complex) * width * height;
    if (status == MagickPass) {
        out_data = (fftw_complex *)fftw_malloc(mem_size);
        if (out_data == NULL) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        plan = fftw_plan_dft_r2c_2d((int)height, (int)width, pixels, out_data,
                                    FFTW_ESTIMATE);
        if (plan == NULL) {
            status = MagickFail;
        }
    }

    /* FFT 実行 */
    clock_gettime(CLOCK_MONOTONIC, &rap);
    if (status == MagickPass) {
        fftw_execute(plan);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    psec = diff_timespec(&rap, &start);
    esec = diff_timespec(&stop, &rap);
    tsec = diff_timespec(&stop, &start);

    printf("preparation: %.5lfs; execution: %.5lfs; total: %.5lfs\n",
            psec, esec, tsec);

    fftw_destroy_plan(plan);
    plan = NULL;

    /* FFT 画像の保存 */
    if (status == MagickPass) {
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                index = width * i + j;
                // pixels[index] = cabs(out_data[index]);
                pixels[index] = creal(out_data[index]);
            }
        }
        ret = MagickSetImagePixels(magick_wand, 0, 0, width, height,
                                      "I", DoublePixel, (void *)pixels);
        if (ret == MagickFalse) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        status = MagickWriteImage(magick_wand, filename);
    }

    /* inverse FFT -- 検証 */
    if (status == MagickPass) {
        plan = fftw_plan_dft_c2r_2d((int)height, (int)width, out_data, pixels,
                                FFTW_ESTIMATE);
        if (plan == NULL) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        fftw_execute(plan);
    }

    fftw_destroy_plan(plan);
    plan = NULL;

    /* inverse FFT 画像の保存 */
    scale = 1. / (width * height);
    if (status == MagickPass) {
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                index = width * i + j;
                pixels[index] *= scale;
            }
        }
        ret = MagickSetImagePixels(magick_wand, 0, 0, width, height,
                                      "I", DoublePixel, (void *)pixels);
        if (ret == MagickFalse) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        tmpstr = strstr(filename, "_fft");
        ++tmpstr; *tmpstr = 'i';
        ++tmpstr; *tmpstr = 'n';
        ++tmpstr; *tmpstr = 'v';
        status = MagickWriteImage(magick_wand, filename);
    }

    fftw_cleanup_threads();

    free(pixels);
    pixels = NULL;
    fftw_free(out_data);
    out_data = NULL;
    return (status);
}
