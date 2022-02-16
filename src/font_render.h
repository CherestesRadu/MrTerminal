#pragma once

#include "stb_truetype.h"
#include "defines.h"

#include <stdio.h>

struct FontRender
{
    int32 width = 0;
    int32 height = 0;

    stbtt_bakedchar *char_data = NULL;
    uint32 tex = 0;
};

FontRender init_font_render();
void release_font_render(FontRender *fr);

void render_glyph(FontRender *fr, uint8 c, int32 x, int32 y, uint32 color);