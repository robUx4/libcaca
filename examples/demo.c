/*
 *  demo          demo for libcaca
 *  Copyright (c) 2003 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <math.h>
#   include <string.h>
#   include <stdio.h>
#endif

#include "cucul.h"
#include "caca.h"

static void display_menu(void);

static void demo_all(void);

static void demo_dots(void);
static void demo_lines(void);
static void demo_boxes(void);
static void demo_ellipses(void);
static void demo_triangles(void);
/*static void demo_sprites(void);*/
static void demo_render(void);

int bounds = 0;
int outline = 0;
int dithering = 0;
#if 0
cucul_sprite_t *sprite = NULL;
#endif

cucul_canvas_t *cv;
caca_display_t *dp;

int main(int argc, char **argv)
{
    void (*demo)(void) = NULL;
    int quit = 0;

    cv = cucul_create_canvas(80, 24);
    if(cv == NULL)
    {
        printf("Failed to create canvas\n");
        return 1;
    }

    dp = caca_create_display(cv);
    if(dp == NULL)
    {
        printf("Failed to create display\n");
        return 1;
    }

    caca_set_display_time(dp, 40000);

    /* Initialize data */
#if 0
    sprite = cucul_load_sprite(DATADIR "/caca.txt");
    if(!sprite)
        sprite = cucul_load_sprite("caca.txt");
    if(!sprite)
        sprite = cucul_load_sprite("examples/caca.txt");
#endif

    /* Disable cursor */
    caca_set_mouse(dp, 0);

    /* Main menu */
    display_menu();
    caca_refresh_display(dp);

    /* Go ! */
    while(!quit)
    {
        caca_event_t ev;
        int menu = 0, mouse = 0, xmouse = 0, ymouse = 0;

        while(caca_get_event(dp, CACA_EVENT_ANY, &ev, 0))
        {
            if(demo && (caca_get_event_type(&ev) & CACA_EVENT_KEY_PRESS))
            {
                menu = 1;
                demo = NULL;
            }
            else if(caca_get_event_type(&ev) & CACA_EVENT_KEY_PRESS)
            {
                switch(caca_get_event_key_ch(&ev))
                {
                case 'q':
                case 'Q':
                case CACA_KEY_ESCAPE:
                    demo = NULL;
                    quit = 1;
                    break;
                case 'o':
                case 'O':
                    outline = (outline + 1) % 3;
                    display_menu();
                    break;
                case 'b':
                case 'B':
                    bounds = (bounds + 1) % 2;
                    display_menu();
                    break;
#if 0
                case 'd':
                case 'D':
                    dithering = (dithering + 1) % 5;
                    cucul_set_feature(cv, dithering);
                    display_menu();
                    break;
#endif
                case 'f':
                case 'F':
                    demo = demo_all;
                    break;
                case '1':
                    demo = demo_dots;
                    break;
                case '2':
                    demo = demo_lines;
                    break;
                case '3':
                    demo = demo_boxes;
                    break;
                case '4':
                    demo = demo_triangles;
                    break;
                case '5':
                    demo = demo_ellipses;
                    break;
#if 0
                case 's':
                case 'S':
                    if(sprite)
                        demo = demo_sprites;
                    break;
#endif
                case 'r':
                case 'R':
                    demo = demo_render;
                    break;
                }

                if(demo)
                {
                    cucul_set_color_ansi(cv, CUCUL_LIGHTGRAY, CUCUL_BLACK);
                    cucul_clear_canvas(cv);
                }
            }
            else if(caca_get_event_type(&ev) & CACA_EVENT_MOUSE_MOTION)
            {
                mouse = 1;
                xmouse = caca_get_event_mouse_x(&ev);
                ymouse = caca_get_event_mouse_y(&ev);
            }
            else if(caca_get_event_type(&ev) & CACA_EVENT_RESIZE)
            {
                mouse = 1; /* old hack */
            }
        }

        if(menu || (mouse && !demo))
        {
            display_menu();
            if(mouse && !demo)
            {
                cucul_set_color_ansi(cv, CUCUL_RED, CUCUL_BLACK);
                cucul_put_str(cv, xmouse, ymouse,     ".");
                cucul_put_str(cv, xmouse, ymouse + 1, "|\\");
            }
            caca_refresh_display(dp);
            mouse = menu = 0;
        }

        if(demo)
        {
            demo();

            cucul_set_color_ansi(cv, CUCUL_LIGHTGRAY, CUCUL_BLACK);
            cucul_draw_thin_box(cv, 1, 1, cucul_get_canvas_width(cv) - 2,
                                          cucul_get_canvas_height(cv) - 2);
            cucul_printf(cv, 4, 1, "[%i.%i fps]----",
                         1000000 / caca_get_display_time(dp),
                         (10000000 / caca_get_display_time(dp)) % 10);
            caca_refresh_display(dp);
        }
    }

    /* Clean up */
#if 0
    cucul_free_sprite(sprite);
#endif
    caca_free_display(dp);
    cucul_free_canvas(cv);

    return 0;
}

static void display_menu(void)
{
    int xo = cucul_get_canvas_width(cv) - 2;
    int yo = cucul_get_canvas_height(cv) - 2;

    cucul_set_color_ansi(cv, CUCUL_LIGHTGRAY, CUCUL_BLACK);
    cucul_clear_canvas(cv);
    cucul_draw_thin_box(cv, 1, 1, xo, yo);

    cucul_put_str(cv, (xo - strlen("libcaca demo")) / 2, 3, "libcaca demo");
    cucul_put_str(cv, (xo - strlen("==============")) / 2, 4, "==============");

    cucul_put_str(cv, 4, 6, "demos:");
    cucul_put_str(cv, 4, 7, "'f': full");
    cucul_put_str(cv, 4, 8, "'1': dots");
    cucul_put_str(cv, 4, 9, "'2': lines");
    cucul_put_str(cv, 4, 10, "'3': boxes");
    cucul_put_str(cv, 4, 11, "'4': triangles");
    cucul_put_str(cv, 4, 12, "'5': ellipses");
    cucul_put_str(cv, 4, 13, "'c': colour");
    cucul_put_str(cv, 4, 14, "'r': render");
#if 0
    if(sprite)
        cucul_put_str(cv, 4, 15, "'s': sprites");
#endif

    cucul_put_str(cv, 4, 16, "settings:");
    cucul_printf(cv, 4, 17, "'o': outline: %s",
                 outline == 0 ? "none" : outline == 1 ? "solid" : "thin");
    cucul_printf(cv, 4, 18, "'b': drawing boundaries: %s",
                 bounds == 0 ? "screen" : "infinite");
    //cucul_printf(cv, 4, 19, "'d': dithering (%s)",
    //             cucul_get_feature_name(dithering));

    cucul_put_str(cv, 4, yo - 2, "'q': quit");

    caca_refresh_display(dp);
}

static void demo_all(void)
{
    static int i = 0;

    int j, xo, yo, xa, ya, xb, yb, xc, yc;

    i++;

    cucul_set_color_ansi(cv, CUCUL_LIGHTGRAY, CUCUL_BLACK);
    cucul_clear_canvas(cv);

    /* Draw the sun */
    cucul_set_color_ansi(cv, CUCUL_YELLOW, CUCUL_BLACK);
    xo = cucul_get_canvas_width(cv) / 4;
    yo = cucul_get_canvas_height(cv) / 4 + 5 * sin(0.03*i);

    for(j = 0; j < 16; j++)
    {
        xa = xo - (30 + sin(0.03*i) * 8) * sin(0.03*i + M_PI*j/8);
        ya = yo + (15 + sin(0.03*i) * 4) * cos(0.03*i + M_PI*j/8);
        cucul_draw_thin_line(cv, xo, yo, xa, ya);
    }

    j = 15 + sin(0.03*i) * 8;
    cucul_set_color_ansi(cv, CUCUL_WHITE, CUCUL_BLACK);
    cucul_fill_ellipse(cv, xo, yo, j, j / 2, '#');
    cucul_set_color_ansi(cv, CUCUL_YELLOW, CUCUL_BLACK);
    cucul_draw_ellipse(cv, xo, yo, j, j / 2, '#');

    /* Draw the pyramid */
    xo = cucul_get_canvas_width(cv) * 5 / 8;
    yo = 2;

    xa = cucul_get_canvas_width(cv) / 8 + sin(0.03*i) * 5;
    ya = cucul_get_canvas_height(cv) / 2 + cos(0.03*i) * 5;

    xb = cucul_get_canvas_width(cv) - 10 - cos(0.02*i) * 10;
    yb = cucul_get_canvas_height(cv) * 3 / 4 - 5 + sin(0.02*i) * 5;

    xc = cucul_get_canvas_width(cv) / 4 - sin(0.02*i) * 5;
    yc = cucul_get_canvas_height(cv) * 3 / 4 + cos(0.02*i) * 5;

    cucul_set_color_ansi(cv, CUCUL_GREEN, CUCUL_BLACK);
    cucul_fill_triangle(cv, xo, yo, xb, yb, xa, ya, '%');
    cucul_set_color_ansi(cv, CUCUL_YELLOW, CUCUL_BLACK);
    cucul_draw_thin_triangle(cv, xo, yo, xb, yb, xa, ya);

    cucul_set_color_ansi(cv, CUCUL_RED, CUCUL_BLACK);
    cucul_fill_triangle(cv, xa, ya, xb, yb, xc, yc, '#');
    cucul_set_color_ansi(cv, CUCUL_YELLOW, CUCUL_BLACK);
    cucul_draw_thin_triangle(cv, xa, ya, xb, yb, xc, yc);

    cucul_set_color_ansi(cv, CUCUL_BLUE, CUCUL_BLACK);
    cucul_fill_triangle(cv, xo, yo, xb, yb, xc, yc, '%');
    cucul_set_color_ansi(cv, CUCUL_YELLOW, CUCUL_BLACK);
    cucul_draw_thin_triangle(cv, xo, yo, xb, yb, xc, yc);

    /* Draw a background triangle */
    xa = 2;
    ya = 2;

    xb = cucul_get_canvas_width(cv) - 3;
    yb = cucul_get_canvas_height(cv) / 2;

    xc = cucul_get_canvas_width(cv) / 3;
    yc = cucul_get_canvas_height(cv) - 3;

    cucul_set_color_ansi(cv, CUCUL_CYAN, CUCUL_BLACK);
    cucul_draw_thin_triangle(cv, xa, ya, xb, yb, xc, yc);

    xo = cucul_get_canvas_width(cv) / 2 + cos(0.027*i) * cucul_get_canvas_width(cv) / 3;
    yo = cucul_get_canvas_height(cv) / 2 - sin(0.027*i) * cucul_get_canvas_height(cv) / 2;

    cucul_draw_thin_line(cv, xa, ya, xo, yo);
    cucul_draw_thin_line(cv, xb, yb, xo, yo);
    cucul_draw_thin_line(cv, xc, yc, xo, yo);

    /* Draw a sprite on the pyramid */
#if 0
    cucul_draw_sprite(cv, xo, yo, sprite, 0);
#endif

    /* Draw a trail behind the foreground sprite */
    for(j = i - 60; j < i; j++)
    {
        int delta = cucul_rand(-5, 6);
        cucul_set_color_ansi(cv, cucul_rand(0, 16), cucul_rand(0, 16));
        cucul_put_char(cv, cucul_get_canvas_width(cv) / 2
                    + cos(0.02*j) * (delta + cucul_get_canvas_width(cv) / 4),
                   cucul_get_canvas_height(cv) / 2
                    + sin(0.02*j) * (delta + cucul_get_canvas_height(cv) / 3),
                   '#');
    }

    /* Draw foreground sprite */
#if 0
    cucul_draw_sprite(cv, cucul_get_canvas_width(cv) / 2 + cos(0.02*i) * cucul_get_canvas_width(cv) / 4,
                   cucul_get_canvas_height(cv) / 2 + sin(0.02*i) * cucul_get_canvas_height(cv) / 3,
                   sprite, 0);
#endif
}

static void demo_dots(void)
{
    int xmax = cucul_get_canvas_width(cv);
    int ymax = cucul_get_canvas_height(cv);
    int i;
    static char chars[10] =
    {
        '+', '-', '*', '#', 'X', '@', '%', '$', 'M', 'W'
    };

    for(i = 1000; i--;)
    {
        /* Putpixel */
        cucul_set_color_ansi(cv, cucul_rand(0, 16), cucul_rand(0, 16));
        cucul_put_char(cv, cucul_rand(0, xmax), cucul_rand(0, ymax),
                      chars[cucul_rand(0, 9)]);
    }
}

static void demo_lines(void)
{
    int w = cucul_get_canvas_width(cv);
    int h = cucul_get_canvas_height(cv);
    int xa, ya, xb, yb;

    if(bounds)
    {
        xa = cucul_rand(- w, 2 * w); ya = cucul_rand(- h, 2 * h);
        xb = cucul_rand(- w, 2 * w); yb = cucul_rand(- h, 2 * h);
    }
    else
    {
        xa = cucul_rand(0, w); ya = cucul_rand(0, h);
        xb = cucul_rand(0, w); yb = cucul_rand(0, h);
    }

    cucul_set_color_ansi(cv, cucul_rand(0, 16), CUCUL_BLACK);
    if(outline > 1)
        cucul_draw_thin_line(cv, xa, ya, xb, yb);
    else
        cucul_draw_line(cv, xa, ya, xb, yb, '#');
}

static void demo_boxes(void)
{
    int w = cucul_get_canvas_width(cv);
    int h = cucul_get_canvas_height(cv);
    int xa, ya, xb, yb;

    if(bounds)
    {
        xa = cucul_rand(- w, 2 * w); ya = cucul_rand(- h, 2 * h);
        xb = cucul_rand(- w, 2 * w); yb = cucul_rand(- h, 2 * h);
    }
    else
    {
        xa = cucul_rand(0, w); ya = cucul_rand(0, h);
        xb = cucul_rand(0, w); yb = cucul_rand(0, h);
    }

    cucul_set_color_ansi(cv, cucul_rand(0, 16), cucul_rand(0, 16));
    cucul_fill_box(cv, xa, ya, xb, yb, '#');

    cucul_set_color_ansi(cv, cucul_rand(0, 16), CUCUL_BLACK);
    if(outline == 2)
        cucul_draw_thin_box(cv, xa, ya, xb, yb);
    else if(outline == 1)
        cucul_draw_box(cv, xa, ya, xb, yb, '#');
}

static void demo_ellipses(void)
{
    int w = cucul_get_canvas_width(cv);
    int h = cucul_get_canvas_height(cv);
    int x, y, a, b;

    if(bounds)
    {
        x = cucul_rand(- w, 2 * w); y = cucul_rand(- h, 2 * h);
        a = cucul_rand(0, w); b = cucul_rand(0, h);
    }
    else
    {
        do
        {
            x = cucul_rand(0, w); y = cucul_rand(0, h);
            a = cucul_rand(0, w); b = cucul_rand(0, h);

        } while(x - a < 0 || x + a >= w || y - b < 0 || y + b >= h);
    }

    cucul_set_color_ansi(cv, cucul_rand(0, 16), cucul_rand(0, 16));
    cucul_fill_ellipse(cv, x, y, a, b, '#');

    cucul_set_color_ansi(cv, cucul_rand(0, 16), CUCUL_BLACK);
    if(outline == 2)
        cucul_draw_thin_ellipse(cv, x, y, a, b);
    else if(outline == 1)
        cucul_draw_ellipse(cv, x, y, a, b, '#');
}

static void demo_triangles(void)
{
    int w = cucul_get_canvas_width(cv);
    int h = cucul_get_canvas_height(cv);
    int xa, ya, xb, yb, xc, yc;

    if(bounds)
    {
        xa = cucul_rand(- w, 2 * w); ya = cucul_rand(- h, 2 * h);
        xb = cucul_rand(- w, 2 * w); yb = cucul_rand(- h, 2 * h);
        xc = cucul_rand(- w, 2 * w); yc = cucul_rand(- h, 2 * h);
    }
    else
    {

        xa = cucul_rand(0, w); ya = cucul_rand(0, h);
        xb = cucul_rand(0, w); yb = cucul_rand(0, h);
        xc = cucul_rand(0, w); yc = cucul_rand(0, h);
    }

    cucul_set_color_ansi(cv, cucul_rand(0, 16), cucul_rand(0, 16));
    cucul_fill_triangle(cv, xa, ya, xb, yb, xc, yc, '#');

    cucul_set_color_ansi(cv, cucul_rand(0, 16), CUCUL_BLACK);
    if(outline == 2)
        cucul_draw_thin_triangle(cv, xa, ya, xb, yb, xc, yc);
    else if(outline == 1)
        cucul_draw_triangle(cv, xa, ya, xb, yb, xc, yc, '#');
}
#if 0
static void demo_sprites(void)
{

    cucul_draw_sprite(cv, cucul_rand(0, cucul_get_canvas_width(cv)),
                      cucul_rand(0, cucul_get_canvas_height(cv)), sprite, 0);

}
#endif
#if 0
static void demo_render(void)
{
    cucul_dither_t *dither;
    //short buffer[256*256];
    //short *dest = buffer;
    int buffer[256*256];
    int *dest = buffer;
    int x, y, z;
    static int i = 0;

    i = (i + 1) % 512;
    z = i < 256 ? i : 511 - i;

    for(x = 0; x < 256; x++)
        for(y = 0; y < 256; y++)
    {
        //*dest++ = ((x >> 3) << 11) | ((y >> 2) << 5) | ((z >> 3));
        *dest++ = (x << 16) | (y << 8) | (z);
    }
    cucul_set_dither_invert(dither, 1);
    //dither = cucul_create_dither(16, 256, 256, 2 * 256, 0xf800, 0x07e0, 0x001f, 0x0000);
    dither = cucul_create_dither(32, 256, 256, 4 * 256, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    cucul_dither_bitmap(cv, 0, 0, cucul_get_canvas_width(cv), cucul_get_canvas_height(cv),
                     dither, buffer);
    cucul_free_dither(dither);
}
#endif

static void draw_circle(int *buffer, int xo, int yo, int r, int mask, int val);

static void demo_render(void)
{
    cucul_dither_t *dither;
    int buffer[256*256];
    int *dest;
    int x, y, z, xo, yo;
    static int i = 0;

    i++;

    dest = buffer;
    for(x = 0; x < 256; x++)
        for(y = 0; y < 256; y++)
    {
        *dest++ = 0xff000000;
    }

    /* red */
    xo = 128 + 48 * sin(0.02 * i);
    yo = 128 + 48 * cos(0.03 * i);
    for(z = 0; z < 240; z++)
        draw_circle(buffer, xo, yo, z, 0x00ff0000, 200 << 16);

    /* green */
    xo = 128 + 48 * sin(2 + 0.06 * i);
    yo = 128 + 48 * cos(2 + 0.05 * i);
    for(z = 0; z < 240; z++)
        draw_circle(buffer, xo, yo, z, 0x0000ff00, 200 << 8);

    /* blue */
    xo = 128 + 48 * sin(1 + 0.04 * i);
    yo = 128 + 48 * cos(1 + 0.03 * i);
    for(z = 0; z < 240; z++)
        draw_circle(buffer, xo, yo, z, 0x000000ff, 200);

    dither = cucul_create_dither(32, 256, 256, 4 * 256, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    cucul_set_dither_gamma(dither, -1.0);
    cucul_dither_bitmap(cv, 0, 0, cucul_get_canvas_width(cv), cucul_get_canvas_height(cv), dither, (char *)buffer);
    cucul_free_dither(dither);
}

static void draw_circle(int *buffer, int x, int y, int r, int mask, int val)
{
    int t, dx, dy;

#define POINT(X,Y) \
    buffer[(X) + 256 * (Y)] = 0xff000000 | (buffer[(X) + 256 * (Y)] & ~mask) | val;

    for(t = 0, dx = 0, dy = r; dx <= dy; dx++)
    {
        POINT(x - dx / 3, y - dy / 3);
        POINT(x + dx / 3, y - dy / 3);
        POINT(x - dx / 3, y + dy / 3);
        POINT(x + dx / 3, y + dy / 3);

        POINT(x - dy / 3, y - dx / 3);
        POINT(x + dy / 3, y - dx / 3);
        POINT(x - dy / 3, y + dx / 3);
        POINT(x + dy / 3, y + dx / 3);

        t += t > 0 ? dx - dy-- : dx;
    }
}

