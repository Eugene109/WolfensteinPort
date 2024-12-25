// this is the humble start of an ambitious project: doom on a ti-84 plus ce
// will start with wolfenstein instead
// this code originated from the examples packaged with CEDev -
// https://ce-programming.github.io/toolchain/static/getting-started.html
// written by Eugene Choi
// WIP, some of these ideas are just straight trash

#include <graphx.h>
#include <sys/timers.h>
#include <ti/getcsc.h>

// should probably rotate images 90deg clockwise because row-major order
// then can just memcpy into different sprite;
#include "gfx/gfx.h"

#include <cassert>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matrixMath.h"
#include "vectorMath.h"

// #define DEBUG
#ifdef DEBUG
#include <debug.h>
#endif

#ifndef PI
#define PI 3.14159265359 * (1 << SHIFT)
#endif

#define DELTA 16 // maybe decrease a little?
// Axis-Aligned line

// +Y is forward,
// +X is right
// X is parallel to x-axis
enum LineType { X_LINE, Y_LINE };
class AA_Line {
  public:
    AA_Line(LineType X_Y_line, double off, double min, double max, double t1, double t2) {
        type = X_Y_line;
        pos = off * (1 << SHIFT);
        min_value = min * (1 << SHIFT);
        max_value = max * (1 << SHIFT);
        tex1 = t1 * (1 << SHIFT);
        tex2 = t2 * (1 << SHIFT);
        wPos = pos;
        wMin_value = min_value;
        wMax_value = max_value;
    }
    bool rayIntersection(const ivec2 &ray, ivec2 *hit, fixed *texCoord) {
        if (type == X_LINE) {
            if ((ray.y <= DELTA) && (ray.y >= -DELTA)) {
                return false;
            }
            int mult = (pos << SHIFT) / ray.y;
            if (mult >= DELTA) {
                int xPos = (ray.x * mult) >> SHIFT;
                if (min_value <= xPos + DELTA && xPos - DELTA <= max_value) {
                    // dbg_printf("hiiii");
                    *hit = ivec2(xPos, pos);
                    if (texCoord != nullptr) {
                        *texCoord =
                            ((((xPos - min_value) << SHIFT) / (max_value - min_value) * (tex2 - tex1)) >> SHIFT) + tex1;
                    }
                    return true;
                } else {
                    return false;
                }
            }
            return false;
        } else if (type == Y_LINE) {
            if ((ray.x <= DELTA) && (ray.x >= -DELTA)) {
                return false;
            }
            int mult = (pos << SHIFT) / ray.x;
            if (mult >= DELTA) {
                int yPos = (ray.y * mult) >> SHIFT;
                if (min_value <= yPos + DELTA && yPos - DELTA <= max_value) {
                    *hit = ivec2(pos, yPos);
                    if (texCoord != nullptr) {
                        *texCoord =
                            ((((yPos - min_value) << SHIFT) / (max_value - min_value) * (tex2 - tex1)) >> SHIFT) + tex1;
                    }
                    return true;
                } else {
                    return false;
                }
            }
            return false;
        }
        assert(0);
        return false;
    }
    LineType type;
    int pos;
    int min_value, max_value;
    int tex1, tex2;
    int wPos;
    int wMin_value, wMax_value;
};

inline void bresenhamLine(unsigned x0, unsigned y0, unsigned x1, unsigned y1) {
    unsigned dx = x1 - x0;
    unsigned dy = y1 - y0;
    int D = 2 * dy - dx;
    unsigned y = y0;

    for (int x = x0; x <= x1; ++x) {
        // plot(x, y);
        if (D > 0) {
            y = y + 1;
            D = D - 2 * dx;
        }
        D = D + 2 * dy;
    }
}
inline void writeTexStripToVBuffer(const unsigned char *start_arr_ptr, uint8_t *dest, uint8_t pxl_scl, unsigned xPos,
                                   int yPos, fixed tex_pos, unsigned dest_height) {
    unsigned SRC_H = 32;
    unsigned SRC_H_SHIFT = 5;
    unsigned img_offset = (((1 << ((SRC_H_SHIFT - 1) << 1)) << (SHIFT + 2)) / 3) >> SHIFT;
    const uint8_t *strip_arr_ptr =
        start_arr_ptr + img_offset + ((((tex_pos % (1 << SHIFT)) << SRC_H_SHIFT) >> SHIFT) << SRC_H_SHIFT);

    unsigned x0 = yPos;
    unsigned x1 = x0 + dest_height;
    unsigned dx = x1 - x0;
    unsigned dy = SRC_H;
    int D = 2 * dy - dx;
    unsigned y = 0;

    for (unsigned x = x0; x <= x1; ++x) {
        uint8_t *dest_strip_ptr = dest + xPos + x * GFX_LCD_WIDTH;
        for (int i = 0; i < pxl_scl; ++i) {
            *(dest_strip_ptr++) = *(strip_arr_ptr + y);
        }
        if (D > 0) {
            y = y + 1;
            D = D - 2 * dx;
        }
        D = D + 2 * dy;
    }
}

inline uint8_t *writeTexStripToSpriteBuffer(const unsigned char *start_arr_ptr, uint8_t *dest, uint8_t pxl_scl,
                                            unsigned xPos, int yPos, fixed tex_pos, unsigned dest_height) {
    unsigned SRC_H = 32;
    unsigned SRC_H_SHIFT = 5;
    unsigned img_offset = (((1 << ((SRC_H_SHIFT - 1) << 1)) << (SHIFT + 2)) / 3) >> SHIFT;
    const uint8_t *strip_arr_ptr =
        start_arr_ptr + img_offset + ((((tex_pos % (1 << SHIFT)) << SRC_H_SHIFT) >> SHIFT) << SRC_H_SHIFT);

    unsigned x0 = yPos;
    unsigned x1 = x0 + dest_height;
    unsigned dx = x1 - x0;
    unsigned dy = SRC_H;
    int D = 2 * dy - dx;
    unsigned y = 0;

    for (unsigned x = x0; x <= x1; ++x) {
        for (int i = 0; i < pxl_scl; ++i) {
            *(dest++) = *(strip_arr_ptr + y);
        }
        if (D > 0) {
            y = y + 1;
            D = D - 2 * dx;
        }
        D = D + 2 * dy;
    }
    return dest;
}

gfx_UninitedSprite(img_strip, 1, GFX_LCD_HEIGHT);

inline void drawTexStrip(const unsigned char *start_arr_ptr, uint8_t pxl_scl, unsigned xPos, int yPos, fixed tex_pos,
                         unsigned dest_height) {
    img_strip->height = GFX_LCD_HEIGHT;
    img_strip->width = 1;
    unsigned SRC_H = 32;
    unsigned SRC_H_SHIFT = 5;
    unsigned img_offset = (((1 << ((SRC_H_SHIFT - 1) << 1)) << (SHIFT + 2)) / 3) >> SHIFT;
    const uint8_t *strip_arr_ptr =
        start_arr_ptr + img_offset + ((((tex_pos % (1 << SHIFT)) << SRC_H_SHIFT) >> SHIFT) << SRC_H_SHIFT);
    // gfx_sprite_t *img_strip = gfx_MallocSprite(1, dest_height); // maybe use gfx_tempSprite with set height
    // for (unsigned i = 0; i < dest_height; ++i) {
    //     img_strip->data[i] = *(strip_arr_ptr + (((i << SRC_H_SHIFT) / dest_height)));
    // }
    unsigned x0 = yPos;
    unsigned x1 = x0 + dest_height;
    unsigned dx = x1 - x0;
    unsigned dy = SRC_H;
    int D = 2 * dy - dx;
    unsigned y = 0;

    memset(img_strip->data, 0, GFX_LCD_HEIGHT);
    for (unsigned x = x0; x <= x1; ++x) {
        img_strip->data[x] = *(strip_arr_ptr + y);
        if (D > 0) {
            y = y + 1;
            D = D - 2 * dx;
        }
        D = D + 2 * dy;
    }
    // memmove(img_strip->data, strip_arr_ptr, height);

    if ((xPos + pxl_scl) <= GFX_LCD_WIDTH && (yPos + dest_height) < GFX_LCD_HEIGHT && xPos >= 0 && yPos >= 0) {
        gfx_ScaledSprite_NoClip(img_strip, xPos, 0, pxl_scl, 1);
    } else {
        gfx_SetColor(1);
        gfx_Rectangle(xPos, yPos, pxl_scl, dest_height);
        // continue;
    }
    free(img_strip);
}

inline void drawTexStrip_scl_h(const unsigned char *start_arr_ptr, uint8_t pxl_scl, unsigned xPos, int yPos,
                               fixed tex_pos, unsigned height) {
    unsigned SRC_H = 32;
    unsigned SRC_H_SHIFT = 5;
    unsigned img_offset = (((1 << ((SRC_H_SHIFT - 1) << 1)) << (SHIFT + 2)) / 3) >> SHIFT;
    const uint8_t *strip_arr_ptr =
        start_arr_ptr + img_offset + ((((tex_pos % (1 << SHIFT)) << SRC_H_SHIFT) >> SHIFT) << SRC_H_SHIFT);
    gfx_sprite_t *img_strip = gfx_MallocSprite(1, height);
    for (unsigned i = 0; i < height; ++i) {
        uint8_t color = *(strip_arr_ptr + (((i << SRC_H_SHIFT) / height)));
        img_strip->data[i] = color;
    }
    // memmove(img_strip->data, strip_arr_ptr, height);

    if ((xPos + pxl_scl) <= GFX_LCD_WIDTH && (yPos + height * pxl_scl) < GFX_LCD_HEIGHT && xPos >= 0 && yPos >= 0) {
        gfx_ScaledSprite_NoClip(img_strip, xPos, yPos, pxl_scl, pxl_scl);
    } else {
        gfx_SetColor(1);
        gfx_Rectangle(xPos, yPos, pxl_scl, height * pxl_scl);
        // continue;
    }
    free(img_strip);
}
// void drawTexStrip__NoClip(const unsigned char *start_arr_ptr, uint8_t pxl_scl, unsigned xPos, unsigned yPos,
//                           fixed tex_pos, unsigned dest_height) {}

void draw();

// placeholders
ivec2 playerPos = ivec2(0);

// maybe do something like this in future?
// class Player {
//   public:
//     Player() : pos(0) {}
//     ivec2 pos;
// };

// gfx_UninitedSprite(scrBuff, GFX_LCD_HEIGHT, GFX_LCD_WIDTH / 2);

int main(void) {
    gfx_Begin();
    gfx_SetDrawBuffer();

    uint8_t key = os_GetCSC();
    uint8_t add = 0;

    uint8_t count = 16;

    clock_t start_t, end_t;
    double total_t = 1;

    const uint8_t NUM_LINES = 10;
    AA_Line lines[NUM_LINES] = {AA_Line(X_LINE, 10, -10, 7, 0, 17),  AA_Line(Y_LINE, 7, 7, 10, 0, 3),
                                AA_Line(X_LINE, 7, 7, 10, 0, 3),     AA_Line(Y_LINE, 10, -10, 7, 0, 17),

                                AA_Line(X_LINE, -10, -7, 10, 0, 17), AA_Line(Y_LINE, -7, -10, -7, 0, 3),
                                AA_Line(X_LINE, -7, -10, -7, 0, 3),  AA_Line(Y_LINE, -10, -7, 10, 0, 17),

                                AA_Line(X_LINE, 7, -5, 2, 0, 7),     AA_Line(Y_LINE, 7, -5, 2, 0, 7)};

    int counter = 0;

    const uint8_t SKIP = 4;
    const uint8_t NUM_RAYS = GFX_LCD_WIDTH / SKIP;
    ivec2 rayOffsets[NUM_RAYS];
    for (uint8_t i = 0; i < NUM_RAYS; ++i) {
        rayOffsets[i] =
            (ivec2((((double(i * SKIP) / double(GFX_LCD_WIDTH - 1.0)) * 2.0) - 1.0) * (1 << SHIFT), (1 << SHIFT)));
    }

    imat2 rot = rotate(0);
    ivec2 cam_forward = rot * ivec2(0, 1 << SHIFT);

    // uint8_t *colorBuffer = new uint8_t[GFX_LCD_HEIGHT * GFX_LCD_WIDTH * sizeof(uint8_t)];
    // printf("%d", int(colorBuffer));
    do {
        start_t = clock();
        gfx_ZeroScreen();

        if (key == sk_Up) {
            count += 2;
            playerPos += cam_forward;
            // loop through lines, find ones with pos < certain value, limit playerPos
        } else if (key == sk_Down) {
            count -= 2;
            playerPos -= cam_forward;
        }
        for (uint8_t i = 0; i < NUM_LINES; ++i) {
            if (lines[i].type == X_LINE) {
                lines[i].pos = lines[i].wPos - playerPos.y;
                lines[i].min_value = lines[i].wMin_value - playerPos.x;
                lines[i].max_value = lines[i].wMax_value - playerPos.x;
            } else {
                lines[i].pos = lines[i].wPos - playerPos.x;
                lines[i].min_value = lines[i].wMin_value - playerPos.y;
                lines[i].max_value = lines[i].wMax_value - playerPos.y;
            }
        }

        if (key == sk_Right) {
            ++counter;
        }
        if (key == sk_Left) {
            --counter;
        }
        rot = rotate(int(-0.05 * counter * PI));
        ivec2 ray;

        ivec2 current_hit = ivec2();
        int current_dist = 0;
        fixed current_texCoord = 0;

        cam_forward = rot * ivec2(0, 1 << SHIFT);

        // dist, texCoord, maybe add a texture index
        fixed rayCastBuff[NUM_RAYS * 2];
        fixed *rayCastBuffPtr = &(rayCastBuff[0]);
        // gfx_sprite_t *scrBuff;
        // scrBuff->width = GFX_LCD_HEIGHT;
        // scrBuff->height = GFX_LCD_WIDTH / 2;
        // uint8_t *scrBuffPtr = &(scrBuff->data[0]);

        for (uint8_t a = 0; a < NUM_RAYS; ++a) {

            ray = rot * rayOffsets[a];

            ivec2 hit = ivec2();
            int dist = 100 * (1 << SHIFT);
            fixed texCoord = 0;

            for (uint8_t i = 0; i < NUM_LINES; ++i) {
                if (lines[i].rayIntersection(ray, &current_hit, &current_texCoord)) {
                    current_dist = dot(current_hit, cam_forward);
                    if (current_dist < dist) {
                        hit = current_hit;
                        dist = current_dist;
                        texCoord = current_texCoord;
#ifdef DEBUG
                        dbg_printf("dist: %f\n, a = %d\n", dist / double(1 << SHIFT), a);
                        dbg_printf("hit: %f, %f\n", hit.x / double(1 << SHIFT), hit.y / double(1 << SHIFT));
#endif
                    }
                }
            }
            int stripLen = ((1 << SHIFT) * (1 << SHIFT) / dist) * 500;
#ifdef DEBUG
            dbg_printf("striplen: %d\n", stripLen);
#endif

// #define LOW_RES
#ifdef LOW_RES
            // striplen>>2 for skip of 4
            drawTexStrip_scl_h(brick_wall_arr_data, SKIP, a * SKIP,
                               (GFX_LCD_HEIGHT - (((stripLen >> 2) >> SHIFT) << 2)) >> 1, texCoord,
                               ((stripLen >> 2) >> SHIFT));
#else
            // writeTexStripToVBuffer(brick_wall_arr_data, &(gfx_vbuffer[0][0]), SKIP, a * SKIP,
            //                        (GFX_LCD_HEIGHT - ((stripLen) >> SHIFT)) >> 1, texCoord, (stripLen >> SHIFT));
            drawTexStrip(brick_wall_arr_data, SKIP, a * SKIP, (GFX_LCD_HEIGHT - ((stripLen) >> SHIFT)) >> 1, texCoord,
                         (stripLen >> SHIFT));
            // scrBuffPtr = writeTexStripToSpriteBuffer(brick_wall_arr_data, scrBuffPtr, SKIP, a * SKIP,
            //                                          (GFX_LCD_HEIGHT - ((stripLen) >> SHIFT)) >> 1, texCoord,
            //                                          (stripLen >> SHIFT));
            // scrBuffPtr = writeTexStripToSpriteBuffer(brick_wall_arr_data, scrBuffPtr, SKIP, a * SKIP,
            //                                          (GFX_LCD_HEIGHT - ((stripLen) >> SHIFT)) >> 1, texCoord,
            //                                          (stripLen >> SHIFT));
            // scrBuffPtr = writeTexStripToSpriteBuffer(brick_wall_arr_data, scrBuffPtr, SKIP, a * SKIP,
            //                                          (GFX_LCD_HEIGHT - ((stripLen) >> SHIFT)) >> 1, texCoord,
            //                                          (stripLen >> SHIFT));
            // scrBuffPtr = writeTexStripToSpriteBuffer(brick_wall_arr_data, scrBuffPtr, SKIP, a * SKIP,
            //                                          (GFX_LCD_HEIGHT - ((stripLen) >> SHIFT)) >> 1, texCoord,
            //                                          (stripLen >> SHIFT));

#endif
        }
        gfx_SetPalette(global_palette, sizeof_global_palette, 0);
        // memmove(gfx_vbuffer, colorBuffer, GFX_LCD_HEIGHT * GFX_LCD_WIDTH);
        // gfx_RotatedSprite_NoClip(scrBuff, 0, 0, 128);

        end_t = clock();
        total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
        char str[100];
        sprintf(str, "REV 0.2.7 render time: %f", total_t);
        gfx_SetTextFGColor(254);
        uint8_t offsetX = (GFX_LCD_WIDTH - gfx_GetStringWidth(str)) / 2;
        gfx_PrintStringXY(str, offsetX, 4);

        gfx_SwapDraw();
        add = (add + 1) % (254 - count);
        key = os_GetCSC();
    } while (key != sk_Enter);
    // delete[] (colorBuffer);

    /* End graphics drawing */
    gfx_End();

    return 0;
}