#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CL_TARGET_OPENCL_VERSION 300
#include <clFFT.h>
#include <wand/magick_wand.h>
#include "process.h"
#include "utility.h"

MagickPassFail
execute_fft_clfft(MagickWand *magick_wand, char *filename)
{
    MagickPassFail status = MagickPass;
    MagickBool ret = MagickFalse;
    size_t height = 0, width = 0;
    double *pixels = NULL;
    double *in_data = NULL;
    struct timespec start, rap, stop;
    double psec, esec, tsec;
    int i, j, index, clindex;

    clfftStatus clstatus;
    cl_platform_id platform = 0;
    cl_device_id device = 0;
    size_t cl_param_size = 0;
    char platform_name[128];
    char device_name[128];
    cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, 0, 0 };
    cl_context ctx = 0;
    cl_command_queue queue = 0;
    clfftPlanHandle plan;
    clfftDim dim = CLFFT_2D;
    size_t clLengths[2];
    clfftSetupData fftSetup;
    cl_mem buffer;

    /* grayscale 情報の取り出し */
    if (status == MagickPass) {
        height = MagickGetImageHeight(magick_wand);
        width = MagickGetImageWidth(magick_wand);
        clLengths[0] = height;
        clLengths[1] = width;
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

    /* OpenCL環境の設定 */
    if (status == MagickPass) {
        clstatus = clGetPlatformIDs(1, &platform, NULL);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clGetPlatformInfo(platform, CL_PLATFORM_NAME,
                sizeof(platform_name), platform_name, &cl_param_size);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        printf("Platform: %s\n", platform_name);
    }
    if (status == MagickPass) {
        clstatus = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1,
                &device, NULL );
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clGetDeviceInfo(device, CL_DEVICE_NAME,
                sizeof(device_name), device_name, &cl_param_size);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        printf("Device: %s\n", device_name);
    }
    if (status == MagickPass) {
        props[1] = (cl_context_properties)platform;
        ctx = clCreateContext(props, 1, &device, NULL, NULL, &clstatus);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        queue = clCreateCommandQueue(ctx, device, 0, &clstatus);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clfftInitSetupData(&fftSetup);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clfftSetup(&fftSetup);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }

    /* clFFT 用データへの変換 */
    clock_gettime(CLOCK_MONOTONIC, &start);
    size_t mem_size = sizeof(double) * width * height * 2;
    if (status == MagickPass) {
        in_data = (double *)malloc(mem_size);
        if (in_data == NULL) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                index = height * j + i;
                clindex = 2 * (width * i + j);
                in_data[clindex] = pixels[index];
                in_data[clindex + 1] = 0.;
            }
        }
        buffer = clCreateBuffer(ctx, CL_MEM_READ_WRITE, mem_size, NULL,
                &clstatus);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clEnqueueWriteBuffer(queue, buffer, CL_TRUE, 0, mem_size,
                in_data, 0, NULL, NULL);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clfftCreateDefaultPlan(&plan, ctx, dim, clLengths);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clfftSetPlanPrecision(plan, CLFFT_DOUBLE);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clfftSetLayout(plan, CLFFT_COMPLEX_INTERLEAVED,
                CLFFT_COMPLEX_INTERLEAVED);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clfftSetResultLocation(plan, CLFFT_INPLACE);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clfftBakePlan(plan, 1, &queue, NULL, NULL);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }

    /* FFT 実行 */
    clock_gettime(CLOCK_MONOTONIC, &rap);
    if (status == MagickPass) {
        clstatus = clfftEnqueueTransform(plan, CLFFT_FORWARD, 1, &queue, 0,
                NULL, NULL, &buffer, NULL, NULL);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clFinish(queue);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    if (status == MagickPass) {
        clstatus = clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, mem_size,
                in_data, 0, NULL, NULL);
        if (clstatus != CLFFT_SUCCESS) {
            status = MagickFail;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    psec = diff_timespec(&rap, &start);
    esec = diff_timespec(&stop, &rap);
    tsec = diff_timespec(&stop, &start);

    printf("preparation: %.5lfs; execution: %.5lfs; total: %.5lfs\n",
            psec, esec, tsec);

    clstatus = clfftTeardown();

    /* FFT 画像の保存 */
    if (status == MagickPass) {
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                index = height * j + i;
                clindex = 2 * (width * i + j);
                pixels[index] = in_data[clindex];
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

    clReleaseMemObject(buffer);
    free(pixels);
    pixels = NULL;
    free(in_data);
    in_data = NULL;
    return (status);
}
