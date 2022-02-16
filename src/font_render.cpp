#include "font_render.h"

FontRender init_font_render()
{
    FontRender fr;

    //Consolas
	const char *font_path = "c:/windows/fonts/consola.ttf";
	FILE *file = fopen(font_path, "rb");
	
	//Get file size
	fseek(file, 0L, SEEK_END);
	int32 size = ftell(file);
	rewind(file);
	uint8 *ttf_buffer = MALLOC(uint8, size);
	fread(ttf_buffer, 1, size, file);
	uint8 *bitmap = MALLOC(uint8, 512 * 512);
	fr.char_data = MALLOC(stbtt_bakedchar, 96);
	fr.height = 18.0f;
	stbtt_BakeFontBitmap(ttf_buffer, 0, fr.height, bitmap, 512, 512, 32, 96, fr.char_data);
	
	//Get width
	float32 tx = 0.0f, ty = 0.0f;
	
	stbtt_aligned_quad L;
	stbtt_GetBakedQuad(fr.char_data, 512, 512, 'f' - 32, &tx, &ty, &L, true);
	fr.width = round(tx);
	
	glGenTextures(1, &fr.tex);
	glBindTexture(GL_TEXTURE_2D, fr.tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	free(ttf_buffer);
	free(bitmap);
    fclose(file);

    return fr;
}

void release_font_render(FontRender *fr)
{
    free(fr->char_data);
    glDeleteTextures(1, &fr->tex);
}

void render_glyph(FontRender *fr, uint8 c, int32 x, int32 y, uint32 color)
{
    stbtt_aligned_quad q;
	stbtt_aligned_quad f;
	float32 fx=x,fy=y;
	stbtt_GetBakedQuad(fr->char_data, 512, 512, 'f' - 32, &fx, &fy, &f, false);
	
	float32 px = x, py = y;
	stbtt_GetBakedQuad(fr->char_data, 512, 512, c - 32, &px, &py, &q, false);
	float32 ydiff = y - f.y0;
	
	//rgba
	glColor4f((uint8)(color >> 24) / 255.0f, (uint8)(color >> 16) / 255.0f, (uint8)(color >> 8) / 255.0f, (uint8)(color) / 255.0f);
	
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, fr->tex);
	glBegin(GL_QUADS);
	glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y0 + ydiff);
	glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y0 + ydiff);
	glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y1 + ydiff);
	glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y1 + ydiff);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}