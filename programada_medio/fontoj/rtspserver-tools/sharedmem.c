#include <malloc.h>

#include "sharedmem.h"


static key_t key_image_mem;
static key_t key_image_semaphore;
static key_t key_config_mem;
static key_t key_config_semaphore;
static struct shared_conf currentConfig;
static void readMemory(key_t key, void *memory, int memorylenght);
static void lockSemaphore(key_t key);
static void unlockSemaphore(key_t key);
static void writeMemory(key_t key, void *memory, int memorylenght);
static int getMemorySize(key_t key);
static struct sembuf semaphore_lock[1];
static struct sembuf semaphore_unlock[1];
static bool inita=false;


void SharedMem_init() {
  if(inita) return;
  inita=true;
    currentConfig.nightmode = 0;
    currentConfig.flip = 0;
    memset(currentConfig.osdTimeDisplay,0,STRING_MAX_SIZE);
    currentConfig.osdColor = 0;
    currentConfig.osdSize = 0;
    currentConfig.osdFixedWidth = 0;
    currentConfig.sensitivity = -2;
    currentConfig.motionOSD = 0;
    currentConfig.detectionRegion[0] = 0;
    currentConfig.detectionRegion[1] = 0;
    currentConfig.detectionRegion[2] = 0;
    currentConfig.detectionRegion[3] = 0;
    currentConfig.motionTracking = false;
    currentConfig.motionTimeout = -1;
    currentConfig.softVolume = -1;
    currentConfig.hardVolume = -1;
    currentConfig.filter = 0;
    currentConfig.highfilter = 0;
    currentConfig.aecfilter = 0;
    currentConfig.frmRateConfig[0] = 25;
    currentConfig.frmRateConfig[1] = 1;
    memset(currentConfig.osdFontName,0,STRING_MAX_SIZE);

    key_image_mem = ftok("/usr/", '1');
    key_image_semaphore = ftok("/usr/", '2');
    key_config_mem = ftok("/usr/", '3');
    key_config_semaphore = ftok("/usr/", '4');

    semaphore_lock[0].sem_flg = 0;
    semaphore_lock[0].sem_num = (unsigned short) -1;
    semaphore_lock[0].sem_op = SEM_UNDO;


    semaphore_unlock[0].sem_flg = 0;
    semaphore_unlock[0].sem_num = (unsigned short) 1;
    semaphore_unlock[0].sem_op = SEM_UNDO;



}

int SharedMem_getImageSize() {
    return getMemorySize(key_image_mem);
}

void *SharedMem_getImageBuffer() {
    size_t memlen = SharedMem_getImageSize();
    void *memory = malloc(memlen);

    lockSemaphore(key_image_semaphore);
    readMemory(key_image_mem, memory, memlen);
    unlockSemaphore(key_image_semaphore);


    return memory;
}

void SharedMem_copyImage(void *imageMemory, int imageSize) {
    lockSemaphore(key_image_semaphore);
    writeMemory(key_image_mem, imageMemory, imageSize);
    unlockSemaphore(key_image_semaphore);
}

struct shared_conf *SharedMem_getConfig() {
    return &currentConfig;
}

void SharedMem_readConfig(){
    lockSemaphore(key_config_semaphore);
    readMemory(key_config_mem, &currentConfig, sizeof(struct shared_conf));
    unlockSemaphore(key_config_semaphore);
}

void SharedMem_setConfig() {
    lockSemaphore(key_config_semaphore);
    writeMemory(key_config_mem, &currentConfig, sizeof(struct shared_conf));
    unlockSemaphore(key_config_semaphore);
}

static void lockSemaphore(key_t key) {
    semop(key, &semaphore_lock[0], 1);
}

static void unlockSemaphore(key_t key) {
    semop(key, &semaphore_unlock[0], 1);


}

static void readMemory(key_t key, void *memory, int memorylenght) {
    void *shared_mem;
    int shm_id = shmget(key, 0, 0);
    if (shm_id == -1) {
        return;
    }
    shared_mem = shmat(shm_id, NULL, 0);
    memcpy(memory, shared_mem, (size_t) memorylenght);
    shmdt(shared_mem);
}

static int getMemorySize(key_t key) {
    int shm_id = shmget(key, 0, 0);
    struct shmid_ds buf;
    shmctl(shm_id, IPC_STAT, &buf);
    int memlen = buf.shm_segsz;
    return memlen;
}

static void writeMemory(key_t key, void *memory, int memorylenght) {

    int shm_id;

    shm_id = shmget(key, 0, 0);
    if (shm_id != -1) {
        int memlen = getMemorySize(key);
        if (memlen != memorylenght) {
            shmctl(shm_id, IPC_RMID, NULL);
        }
    }


    shm_id = shmget(key, memorylenght, IPC_CREAT);
    if (shm_id != -1) {
        void *shared_mem;
        shared_mem = shmat(shm_id, NULL, 0);
        memcpy(shared_mem, memory, memorylenght);
        shmdt(shared_mem);
    } else {

    }


}

void *SharedMem_getImage() {
    int memlen = SharedMem_getImageSize();
    void *memory = malloc((size_t) memlen);
    lockSemaphore(key_image_semaphore);
    readMemory(key_image_mem, memory, memlen);
    unlockSemaphore(key_image_semaphore);
    return memory;

}
