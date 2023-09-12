#include "v4l2_service.h"

unsigned char *rgb24 = NULL;
struct buffer{
    void * start;
    unsigned int length;
}*buffers;

unsigned int fd;

struct v4l2_capability cap;
struct v4l2_fmtdesc fmtdesc;
struct v4l2_format format;
struct v4l2_streamparm fps;
struct v4l2_requestbuffers reqbuffer;
struct v4l2_buffer v_buffer;
unsigned char frame_buffer[640*480*3];

typedef struct tagBITMAPFILEHEADER{
    unsigned short  bfType;                // the flag of bmp, value is "BM"
    unsigned int    bfSize;                // size BMP file ,unit is bytes
    unsigned int    bfReserved;            // 0
    unsigned int    bfOffBits;             // must be 54

}BITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER{
    unsigned int    biSize;                // must be 0x28
    unsigned int    biWidth;           //
    unsigned int    biHeight;          //
    unsigned short  biPlanes;          // must be 1
    unsigned short  biBitCount;            //
    unsigned int    biCompression;         //
    unsigned int    biSizeImage;       //
    unsigned int    biXPelsPerMeter;   //
    unsigned int    biYPelsPerMeter;   //
    unsigned int    biClrUsed;             //
    unsigned int    biClrImportant;        //
}BITMAPINFOHEADER;

enum v4l2_buf_type type;

int initCamera(char *file_video[])
{
    // 1. open device
    if ((fd = open(*file_video, O_RDWR)) < 0 )  {
        printf("Error opening V4L interface\n");
        return -1;
    }

    // 2.get device info
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0 ){
        printf("Error get device info\n");
        return -1;
    }
    printf("driver:\t\t%s\n",cap.driver);
    printf("card:\t\t%s\n",cap.card);
    printf("bus_info:\t%s\n",cap.bus_info);
    printf("version:\t%d\n",cap.version);
    printf("capabilities:\t%x\n",cap.capabilities);

    if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
    {
        printf("Device %s: supports capture.\n",*file_video);
    }

    if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
    {
        printf("Device %s: supports streaming.\n",*file_video);
    }

    // 3.check all support type
    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) >= 0) {
        printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
        fmtdesc.index ++ ;
    }

    // 4.init

    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    format.fmt.pix.height = 480;
    format.fmt.pix.width = 640;
    format.fmt.pix.field = V4L2_FIELD_INTERLACED;
    if(ioctl(fd, VIDIOC_S_FMT, &format) < 0){
        printf("format set faild!\n");
        return -1;
    }

    if(ioctl(fd, VIDIOC_G_FMT, &format) < 0){
        printf("format get faild!\n");
        return -1;
    }

    printf("format type = %d\n",format.type);
    printf("format.fmt.pix.height = %d\n",format.fmt.pix.height);
    printf("format.fmt.pix.width = %d\n",format.fmt.pix.width);
    printf("format.fmt.pix.field = %d\n",format.fmt.pix.field);

    // 4. set fps
    fps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fps.parm.capture.timeperframe.numerator = 10;
    fps.parm.capture.timeperframe.denominator = 10;
    printf("camera init ok \n");
    return 0;
}

int requestMemory()
{
    unsigned int bufferCount;
    // request buff memory
    memset (&reqbuffer, 0, sizeof (reqbuffer));
    reqbuffer.count = 4;
    reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuffer.memory = V4L2_MEMORY_MMAP;

    int ret;
    ret = ioctl(fd, VIDIOC_REQBUFS, &reqbuffer);

    if( ret < 0){
        printf("request buffer faild  : %d\n",ret);
        return -1;
    }

    buffers = calloc (reqbuffer.count, sizeof (*buffers));
    if(!buffers){
        printf("Out of memery!\n");
        return -1;
    }

    for(bufferCount = 0; bufferCount < reqbuffer.count; bufferCount ++){
        v_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v_buffer.memory = V4L2_MEMORY_MMAP;
        v_buffer.index = bufferCount;

        if(ioctl(fd, VIDIOC_QUERYBUF, &v_buffer) < 0){
            printf("query buffer error!!\n");
            return -1;
        }

        buffers[bufferCount].length = v_buffer.length;
        buffers[bufferCount].start = mmap(NULL,v_buffer.length,PROT_READ | PROT_WRITE, MAP_SHARED, fd,v_buffer.m.offset);
        if(buffers[bufferCount].start == MAP_FAILED){
            printf("buffer map error!!\n");
            return -1;
        }
    }

    for(bufferCount = 0; bufferCount < reqbuffer.count; bufferCount ++){
        v_buffer.index = bufferCount;
        ioctl(fd, VIDIOC_QBUF, &v_buffer);
    }

    rgb24 = (unsigned char *)malloc(640*480*3*sizeof(char));
    assert(rgb24 != NULL);
    return 0;
}

int convert_yuv_to_rgb_pixel(int y, int u, int v){
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = r ;
    pixel[1] = g ;
    pixel[2] = b ;
    return pixel32;
}

int  convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;

    for(in = 0; in < width * height * 2; in += 4)
    {
        pixel_16 =
                yuv[in + 3] << 24 |
                               yuv[in + 2] << 16 |
                                              yuv[in + 1] <<  8 |
                                                              yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}

void yuv2rgb888()
{
    int           i,j;
    unsigned char y1,y2,u,v;
    int r1,g1,b1,r2,g2,b2;
    char * pointer;

    pointer = (char*)buffers[0].start;

    for(i=0;i<480;i++)
    {
        for(j=0;j<320;j++)
        {
            y1 = *( pointer + (i*320+j)*4);
            u  = *( pointer + (i*320+j)*4 + 1);
            y2 = *( pointer + (i*320+j)*4 + 2);
            v  = *( pointer + (i*320+j)*4 + 3);

            r1 = y1 + 1.042*(v-128);
            g1 = y1 - 0.34414*(u-128) - 0.71414*(v-128);
            b1 = y1 + 1.772*(u-128);

            r2 = y2 + 1.042*(v-128);
            g2 = y2 - 0.34414*(u-128) - 0.71414*(v-128);
            b2 = y2 + 1.772*(u-128);

            if(r1>255)
                r1 = 255;
            else if(r1<0)
                r1 = 0;

            if(b1>255)
                b1 = 255;
            else if(b1<0)
                b1 = 0;

            if(g1>255)
                g1 = 255;
            else if(g1<0)
                g1 = 0;

            if(r2>255)
                r2 = 255;
            else if(r2<0)
                r2 = 0;

            if(b2>255)
                b2 = 255;
            else if(b2<0)
                b2 = 0;

            if(g2>255)
                g2 = 255;
            else if(g2<0)
                g2 = 0;

            *(frame_buffer + ((480-1-i)*320+j)*6    ) = (unsigned char)b1;
            *(frame_buffer + ((480-1-i)*320+j)*6 + 1) = (unsigned char)g1;
            *(frame_buffer + ((480-1-i)*320+j)*6 + 2) = (unsigned char)r1;
            *(frame_buffer + ((480-1-i)*320+j)*6 + 3) = (unsigned char)b2;
            *(frame_buffer + ((480-1-i)*320+j)*6 + 4) = (unsigned char)g2;
            *(frame_buffer + ((480-1-i)*320+j)*6 + 5) = (unsigned char)r2;
        }
    }
    printf("change to RGB OK \n");
}

int getFrame()
{
    v_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v_buffer.memory = V4L2_MEMORY_MMAP;

    if(ioctl(fd, VIDIOC_DQBUF, &v_buffer) < 0){
        printf("get VIDIOC_DQBUF failed!!\n");
        return -1;
    }else{
        // yuv2rgb888
        convert_yuv_to_rgb_buffer((unsigned char*)buffers[v_buffer.index].start, rgb24, 640, 480);
        if(ioctl(fd, VIDIOC_QBUF, &v_buffer) < 0){
            printf("get VIDIOC_QBUF failed!!\n");
            return -1;
        }
        return 0;
    }
}

int capture(const char *path, const char *format)
{
    FILE *file;
    file = fopen(path,"wb");
    BITMAPFILEHEADER   bf;
    BITMAPINFOHEADER   bi;

    int size = 640 * 480 * 3 * sizeof(char);

    if(!file){
        printf("file open faild!,maybe without permission!\n");
        return -1;
    }

    bi.biSize = sizeof (BITMAPINFOHEADER);
    bi.biWidth = 640;
    bi.biHeight = 480;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = 0;
    bi.biSizeImage = size;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    //Set BITMAPFILEHEADER
    bf.bfType = 0x4d42;
    bf.bfSize = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER)+ size;
    bf.bfReserved = 0;
    bf.bfOffBits = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER);

    yuv2rgb888();
//    fwrite(&bf, 14, 1, file);
//    fwrite(&bi, 40, 1, file);

    fwrite(&bf, 8, 1, file);
    fwrite(&bf.bfReserved, sizeof(bf.bfReserved), 1, file);
    fwrite(&bf.bfOffBits, sizeof(bf.bfOffBits), 1, file);
    fwrite(&bi, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(frame_buffer, size, 1, file);
    fclose(file);
    return 0;
}

int closeStream()
{
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd, VIDIOC_STREAMOFF, &type) >= 0 ){
        if(close(fd) < 0)
            printf("fd close faild\n");

        printf("stream close successful\n");
    }
    return 0;
}

int startStream()
{
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret = ioctl(fd, VIDIOC_STREAMON, &type);

    if(ret >= 0 ){
        printf("stram start successful\n");
        return 0;
    }
    return ret;
}

int clearMemory()
{
    int i;
    for(i = 0; i < (int)reqbuffer.count; i++){
        munmap(&buffers[i], buffers[i].length);
    }
    munmap(&reqbuffer,1);
    free(rgb24);
    rgb24 = NULL;
    return 0;
}
