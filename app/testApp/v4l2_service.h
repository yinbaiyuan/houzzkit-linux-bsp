#ifndef V4L2_SERVER_H
#define V4L2_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned char *rgb24;

int initCamera(char *file_video[]);
int requestMemory();
int getFrame();
int convert_yuv_to_rgb_pixel(int y, int u, int v);
int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
void yuv2rgb888();
int closeStream();
int startStream();
int clearMemory();
int capture(const char *path, const char *format);

#ifdef __cplusplus
}
#endif
#endif
