/*
 *   ttyvaders     Textmode shoot'em up
 *   Copyright (c) 2002 Sam Hocevar <sam@zoy.org>
 *                 All Rights Reserved
 *
 *   $Id: tunnel.c,v 1.7 2002/12/23 12:03:31 sam Exp $
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>

#include "common.h"

static void draw_wall( game *g, int *wall, int delta );

/* Init tunnel */
tunnel * create_tunnel( game *g, int w, int h )
{
    int i;
    tunnel *t = malloc(sizeof(tunnel));

    t->left = malloc(h*sizeof(int));
    t->right = malloc(h*sizeof(int));
    t->w = w;
    t->h = h;

    if( t->w >= g->w )
    {
        for( i = 0; i < g->h; i++ )
        {
            t->left[i] = -10;
            t->right[i] = g->w + 10;
        }
    }
    else
    {
        t->left[0] = (g->w - w) / 2;
        t->right[0] = (g->w + w) / 2;
        /* Yeah, sub-efficient, but less code to do :-) */
        for( i = 0; i < g->h; i++ )
        {
            update_tunnel( g, t );
        }
    }

    return t;
}

void free_tunnel( tunnel *t )
{
    free( t->left );
    free( t->right );
    free( t );
}

void draw_tunnel( game *g, tunnel *t )
{
    /* Print tunnel */
    draw_wall( g, t->left, 1 );
    draw_wall( g, t->right, -1 );
}

void update_tunnel( game *g, tunnel *t )
{
    static int const delta[] = { -3, -2, -1, 1, 2, 3 };
    int i,j,k;

    /* Slide tunnel one block vertically */
    for( i = t->h - 1; i--; )
    {
        t->left[i+1] = t->left[i];
        t->right[i+1] = t->right[i];
    }

    /* Generate new values */
    i = delta[GET_RAND(0,6)];
    j = delta[GET_RAND(0,6)];

    /* Check in which direction we need to alter tunnel */
    if( t->right[1] - t->left[1] < t->w )
    {
        /* Not wide enough */
        if( i > j )
        {
            k = j; j = i; i = k;
        }
    }
    else if( t->right[1] - t->left[1] - 2 > t->w )
    {
        /* Too wide */
        if( i < j )
        {
            k = j; j = i; i = k;
        }
    }
    else
    {
        /* No need to mess with i and j: width is OK */
    }

    /* If width doesn't exceed game size, update coords */
    if( t->w <= g->w || t->right[1] - t->left[1] < t->w )
    {
        t->left[0] = t->left[1] + i;
        t->right[0] = t->right[1] + j;
    }
    else
    {
        t->left[0] = -10;
        t->right[0] = g->w + 10;
    }

    if( t->w > g->w )
    {
        if( t->left[0] < 0 && t->right[0] < g->w - 2 )
        {
             t->left[0] = t->left[1] + 1;
        }

        if( t->left[0] > 1 && t->right[0] > g->w - 1 )
        {
             t->right[0] = t->right[1] - 1;
        }
    }
    else
    {
        if( t->left[0] < 0 )
        {
            t->left[0] = t->left[1] + 1;
        }

        if( t->right[0] > g->w - 1 )
        {
            t->right[0] = t->right[1] - 1;
        }
    }
}

static void draw_wall( game *g, int *wall, int delta )
{
    int i, j;

    gfx_color( RED );

    if( delta == -1 )
    {
        for( i = 0; i < g->h ; i++ )
        {
            for( j = wall[i] ; j < g->w ; j++ )
            {
                gfx_goto( j, i );
                gfx_putchar( '#' );
            }
        }
    }
    else
    {
        for( i = 0; i < g->h ; i++ )
        {
            for( j = 0 ; j <= wall[i]; j++ )
            {
                gfx_goto( j, i );
                gfx_putchar( '#' );
            }
        }
    }

    gfx_color( GREEN );

    for( i = 0; i < g->h ; i++ )
    {
        char c;

        if( wall[i] <= -10 || wall[i] >= g->w + 10 )
        {
            continue;
        }

        if( i + 1 == g->h || wall[i] > wall[i+1] )
        {
            c = ( i == 0 || wall[i] > wall[i-1] ) ? '>' : '/';
        }
        else
        {
            c = ( i == 0 || wall[i] > wall[i-1] ) ? '\\' : '<';
        }

        if( delta == -1 && i + 1 < g->h )
        {
            for( j = 1; j < wall[i] - wall[i+1]; j++ )
            {
                gfx_goto( wall[i+1] + j - 1, i );
                gfx_putchar( '_' );
            }
        }

        gfx_goto( wall[i] + delta, i );
        gfx_putchar( c );

        if( delta == +1 && i + 1 < g->h )
        {
            for( j = 1; j < wall[i+1] - wall[i]; j++ )
            {
                gfx_goto( wall[i] + j + 1, i );
                gfx_putchar( '_' );
            }
        }
    }
}

