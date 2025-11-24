# benchmark programs
PROGS=	fftw clfft
# test programs
PROGS+=	imageio

CFLAGS+=	-O3 -Wall \
		-I/usr/local/include \
		-I/usr/local/include/GraphicsMagick \
		-L/usr/local/lib
LDFLAGS+=	-lGraphicsMagick -lGraphicsMagickWand

BINOWN=	hiroo
BINDIR=	${.CURDIR}/../bin
MK_MAN=	no

PROGNAME_fftw=	bench-fftw
SRCS_fftw=	main_fftw.c fft_fftw.c process.c imageio.c utility.c
LDFLAGS_fftw=	-lfftw3_threads -lfftw3 -lm

PROGNAME_clfft=	bench-clfft
SRCS_clfft=	main_clfft.c fft_clfft.c process.c imageio.c utility.c
LDFLAGS_clfft=	-lclFFT -lOpenCL

PROGNAME_imageio=	test-imageio
SRCS_imageio=	main_imageio.c process.c imageio.c utility.c

.include <bsd.progs.mk>
