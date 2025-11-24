#include <wand/magick_wand.h>

void prepare_wand(const char *path, MagickWand **magick_wand);
void terminate_wand(MagickWand *magick_wand);

MagickPassFail
process_image_fftw(MagickWand *magick_wand, const char *filename);
MagickPassFail
process_image_clfft(MagickWand *magick_wand, const char *filename);
MagickPassFail
process_image_imageio(MagickWand *magick_wand, const char *filename);

MagickPassFail
execute_fft_fftw(MagickWand *magick_wand, char *filename, int nthreads);
MagickPassFail
execute_fft_clfft(MagickWand *magick_wand, char *filename);
