// DESCRIPTION:
// This file allocates and frees buffers by using heap.
// Its primary purpose is to be used in embedded systems with limited stack space.
// It is not used in the original Doom source code.
// Lilka uses it to utilize PSRAM to store stuff like visplanes.

#include "d_alloc.h"
#include "r_main.h"
#include "r_plane.h"
#include "r_things.h"
#include "r_bsp.h"

void D_AllocBuffers() {
    printf("D_AllocBuffers: Allocating buffers\n");
    printf("R_AllocMain: allocated %d bytes\n", R_AllocMain());
    printf("R_AllocPlanes: allocated %d bytes\n", R_AllocPlanes());
    printf("R_AllocThings: allocated %d bytes\n", R_AllocThings());
    printf("R_AllocBSP: allocated %d bytes\n", R_AllocBSP());
}

void D_FreeBuffers() {
    printf("D_FreeBuffers: Freeing buffers\n");
    R_FreeMain();
    R_FreePlanes();
    R_FreeThings();
    R_FreeBSP();
}
