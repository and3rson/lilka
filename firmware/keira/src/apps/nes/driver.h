#include "nesapp.h"

extern "C" {
#include <event.h>
#include <gui.h>
#include <nes/nes.h>
#include <nes/nes_pal.h>
#include <nes/nesinput.h>
#include <nofconfig.h>
}

class Driver {
public:
    static void setNesApp(NesApp* app);

    static int init(int width, int height);
    static void shutdown();
    static int setMode(int width, int height);
    static void setPalette(rgb_t* pal);
    static void clear(uint8 color);
    static bitmap_t* lockWrite();
    static void freeFrite(int numDirties, rect_t* dirtyRects);
    static void customBlit(bitmap_t* bmp, int numDirties, rect_t* dirtyRects);

    static char fb[1];
    static bitmap_t* bitmap;
    static uint16 nesPalette[256];
    static viddriver_t driver;

    static int16_t w, h, frame_x, frame_y, frame_x_offset, frame_width, frame_height, frame_line_pixels;
    static int64_t last_render;
    static int64_t last_frame_duration;

    static NesApp* app;
};
