// this is the humble start of an ambitious project: doom on a ti-84 plus ce
// will start with wolfenstein instead
// this code originated from the examples packaged with CEDev -
// https://ce-programming.github.io/toolchain/static/getting-started.html
// written by Eugene Choi
// WIP, some of these ideas are just straight trash

#include <graphx.h>
#include <sys/timers.h>
#include <ti/getcsc.h>

#include <cassert>
#include <cstring>
#include <stdio.h>
#include <time.h>

#include "matrixMath.h"
#include "vectorMath.h"

// #define DEBUG
#ifdef DEBUG
#include <debug.h>
#endif

const double PI = 3.14159265;

// Axis-Aligned line

// +Y is forward,
// +X is right
// X is parallel to x-axis
enum LineType { X_LINE, Y_LINE };
class AA_Line {
  public:
    AA_Line(LineType X_Y_line, double off, double min, double max) {
        type = X_Y_line;
        pos = off;
        min_value = min;
        max_value = max;
    }
    bool rayIntersection(const vec2 &ray, vec2 *hit) {
        if (type == X_LINE) {
            if (ray.y == 0) {
                return false;
            }
            double mult = pos / ray.y;
            if (mult > 0) {
                double xPos = ray.x * mult;
                if (min_value <= xPos && xPos <= max_value) {
                    // dbg_printf("hiiii");
                    *hit = vec2(xPos, pos);
                    return true;
                } else {
                    return false;
                }
            }
            return false;
        } else if (type == Y_LINE) {
            if (ray.x == 0) {
                return false;
            }
            double mult = pos / ray.x;
            if (mult > 0) {
                double yPos = ray.y * mult;
                if (min_value <= yPos && yPos <= max_value) {
                    *hit = vec2(pos, yPos);
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
    double pos;
    double min_value, max_value;
};

int main(void) {
    gfx_Begin();
    gfx_SetDrawBuffer();

    uint8_t key = os_GetCSC();
    uint8_t add = 0;

    uint8_t count = 16;

    clock_t start_t, end_t;
    double total_t = 1;

    const uint8_t NUM_LINES = 10;
    AA_Line lines[NUM_LINES] = {AA_Line(X_LINE, 10, -10, 7),  AA_Line(Y_LINE, 7, 7, 10),
                                AA_Line(X_LINE, 7, 7, 10),    AA_Line(Y_LINE, 10, -10, 7),

                                AA_Line(X_LINE, -10, -7, 10), AA_Line(Y_LINE, -7, -10, -7),
                                AA_Line(X_LINE, -7, -10, -7), AA_Line(Y_LINE, -10, -7, 10),

                                AA_Line(X_LINE, 7, -5, 2),    AA_Line(Y_LINE, 7, -5, 2)};

    int counter = 0;

    const uint8_t SKIP = 2;
    vec2 rayOffsets[GFX_LCD_WIDTH / SKIP];
    for (int i = 0; i < GFX_LCD_WIDTH / SKIP; ++i) {
        rayOffsets[i] = (vec2((((i * SKIP) / (GFX_LCD_WIDTH - 1.0)) * 2.0) - 1.0, 1.0));
    }

    mat2 rot = rotate(0);
    do {
        start_t = clock();
        gfx_ZeroScreen();

        if (key == sk_Down) {
            count -= 2;
        } else if (key == sk_Up) {
            count += 2;
        }
        if (key == sk_Right) {
            ++counter;
            rot = rotate(-0.05 * PI) * rot;
        }
        if (key == sk_Left) {
            --counter;
            rot = rotate(0.05 * PI) * rot;
        }
        vec2 ray;
        vec2 hit = vec2();

        vec2 current_hit = vec2();
        double current_dist = 0;

        vec2 cam_forward = rot * vec2(0, 1);
        for (int a = 0; a < GFX_LCD_WIDTH / SKIP; ++a) {
            //
            ray = rot * rayOffsets[a];
            double dist = 100;
            for (int i = 0; i < NUM_LINES; ++i) {
                if (lines[i].rayIntersection(ray, &current_hit)) {
                    current_dist = dot(current_hit, cam_forward);
                    // current_dist = dot(cam_forward, current_hit);
                    if (current_dist < dist) {
                        ++counter;
                        // counter = dist;
                        hit = current_hit;
                        dist = current_dist;
#ifdef DEBUG
                        dbg_printf("dist: %f\n, a = %d", dist, a);
#endif
                    }
                }
            }
            gfx_SetColor(gfx_Darken(224, min(dist * 25, 254)));
            double stripLen = 500 / dist;
            gfx_FillRectangle_NoClip(a * SKIP, (GFX_LCD_HEIGHT - stripLen) / 2, 2, stripLen);
        }
        // uint8_t color_data[25000] = {0};
        // // memcpy(&(gfx_vbuffer[0][0]), &(color_data[0]), sizeof(color_data));
        // memmove(gfx_vbuffer, color_data, sizeof(color_data));

        end_t = clock();
        total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
        char str[100];
        sprintf(str, "render time: %f, counter: %d", total_t, counter);
        counter = 0;
        gfx_SetTextFGColor(254);
        uint8_t offsetX = (GFX_LCD_WIDTH - gfx_GetStringWidth(str)) / 2;
        gfx_PrintStringXY(str, offsetX, 4);

        gfx_SwapDraw();
        add = (add + 1) % (254 - count);
        key = os_GetCSC();
    } while (key != sk_Enter);

    /* End graphics drawing */
    gfx_End();

    return 0;
}