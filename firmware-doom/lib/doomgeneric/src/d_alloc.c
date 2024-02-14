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
    R_AllocMain();
    R_AllocPlanes();
    R_AllocThings();
    R_AllocBSP();
}

void D_FreeBuffers() {
    R_FreeMain();
    R_FreePlanes();
    R_FreeThings();
    R_FreeBSP();
}
