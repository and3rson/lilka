#include <AnimatedGIF.h>

#include "petpet.h"
#include "petpet_gif.h"
#include "utils/defer.h"

// Idea by @imbirWIthSugar

PetPetApp::PetPetApp() : App("PetPet") {
}

void GIFDraw(GIFDRAW* pDraw) {
    // Called once per line
    PetPetApp* app = static_cast<PetPetApp*>(pDraw->pUser);
    if (pDraw->y == 0) {
        // First line of the frame
        app->canvas->fillScreen(lilka::colors::Black);
    }
    int16_t x = (app->canvas->width() - petpet_gif_width) / 2;
    int16_t y = (app->canvas->height() - petpet_gif_height) / 2 + pDraw->y;
    uint16_t* pixels = reinterpret_cast<uint16_t*>(pDraw->pPixels);
    if (pDraw->ucHasTransparency) {
        app->canvas->draw16bitRGBBitmapWithTranColor(
            x, y, pixels, pDraw->pPalette[pDraw->ucTransparent], petpet_gif_width, 1
        );
    } else {
        app->canvas->draw16bitRGBBitmap(x, y, pixels, petpet_gif_width, 1);
    }
    if (pDraw->y == petpet_gif_height - 1) {
        // Last line of the frame
        app->queueDraw();
    }
}

void* GIFAlloc(uint32_t u32Size) {
    return ps_malloc(u32Size);
}

void GIFFree(void* p) {
    free(p);
}

void PetPetApp::run() {
    AnimatedGIF* gif = new AnimatedGIF();
    std::unique_ptr<AnimatedGIF> gif_ptr(gif);

    gif->begin();
    if (!gif->open(const_cast<uint8_t*>(petpet_gif), petpet_gif_size, GIFDraw)) {
        lilka::serial_log("Failed to open GIF");
        return;
    }
    Defer closeGif([&gif] { gif->close(); });

    if (gif->allocFrameBuf(GIFAlloc) != GIF_SUCCESS) {
        lilka::serial_log("Failed to allocate frame buffer");
        return;
    }
    Defer freeFrameBuf([&gif] { gif->freeFrameBuf(GIFFree); });

    gif->setDrawType(GIF_DRAW_COOKED);
    while (1) {
        gif->playFrame(true, NULL, this);
        lilka::State state = lilka::controller.getState();
        if (state.a.justPressed) {
            break;
        }
    }
}
