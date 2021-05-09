#ifndef SHAREDMEM_H
#define SHAREDMEM_H

#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/sem.h>

#define STRING_MAX_SIZE          256

struct shared_conf
{
    int nightmode;
    int flip;
    char osdTimeDisplay[STRING_MAX_SIZE];
    int bitrate;
    uint32_t osdColor;
    int osdSize;
    int osdPosY;
    int osdSpace;
    bool osdFixedWidth;
    int sensitivity;
    int motionOSD;
    int detectionRegion[4]; // x0,y0,x1,y1
    bool motionTracking;
    int motionTimeout;
    int hardVolume;
    int softVolume;
    char filter;
    bool highfilter;
    bool aecfilter;
    int frmRateConfig[2];
    char osdFontName[STRING_MAX_SIZE];
};


void SharedMem_init();
void* SharedMem_getImage();
struct shared_conf *SharedMem_getConfig();
void SharedMem_setConfig();
int SharedMem_getImageSize();
void *SharedMem_getImageBuffer();
void SharedMem_copyImage(void *imageMemory, int imageSize);
void SharedMem_readConfig();

#endif
