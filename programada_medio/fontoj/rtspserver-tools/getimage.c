#include <stdio.h>
#include <stdlib.h>


#include "sharedmem.h"

int main(int argc, char *argv[]) {

    SharedMem_init();
    void* memory = SharedMem_getImage();
    int memlen = SharedMem_getImageSize();

    fwrite(memory, memlen, 1, stdout);
    free(memory);

    return 0;
}
