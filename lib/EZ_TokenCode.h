/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 *             Copyright (C) 1996, 1997  Maorong Zou
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **********************************************************************/
/*
 *  June 1996.  Beta Release.
 *  Sept 1996.  Release Version 1.0
 *  Dec 1996.  Release Version 1.1 Beta
 *  April 1997.  Release Version 1.2
 *  November 1997.  Release Version 1.3
 *  November 1998.  Release Version 1.4
 *  December 1999.  Release Version 1.50
 */
/*
 * Token types
 */
#define     TOKEN_BEGIN                 1
#define     TOKEN_END                   2
#define     TOKEN_VERTEX                3
#define     TOKEN_NORMAL                4
#define     TOKEN_COLOR                 5
#define     TOKEN_INDEX                 6
#define     TOKEN_SHADE_MODEL           7
#define     TOKEN_POLYGON_MODE          8
#define     TOKEN_COLOR_MATERIAL        9
#define     TOKEN_CULL_FACE             10
#define     TOKEN_FRONT_FACE            11
#define     TOKEN_FOG                   12
#define     TOKEN_CLEAR_COLOR           13
#define     TOKEN_CLEAR_INDEX           14
#define     TOKEN_ENABLE                15
#define     TOKEN_DISABLE               16
#define     TOKEN_CALL_LIST             17
#define     TOKEN_CLEAR                 18
#define     TOKEN_MATERIAL              19
#define     TOKEN_LIGHT                 20
#define     TOKEN_LIGHT_MODEL           21

#define     TOKEN_MATRIX_MODE           22
#define     TOKEN_LOAD_IDENTITY         23
#define     TOKEN_LOAD_MATRIX           24
#define     TOKEN_MULT_MATRIX           25
#define     TOKEN_PUSH_MATRIX           26
#define     TOKEN_POP_MATRIX            27
#define     TOKEN_SCALE                 28
#define     TOKEN_TRANSLATE             29

#define     TOKEN_DRAW_BUFFER           30
#define     TOKEN_BIND_MATERIAL         31
#define     TOKEN_BIND_LIGHT            32
#define     TOKEN_BIND_LIGHT_MODEL      33

#define     TOKEN_TETRAHEDRON           34
#define     TOKEN_CUBE                  35
#define     TOKEN_OCTAHEDRON            36
#define     TOKEN_DODECAHEDRON          37
#define     TOKEN_ICOSAHEDRON           38
#define     TOKEN_SPHERE                39
#define     TOKEN_TUBE                  40

#define     TOKEN_CMOVE                 41
#define     TOKEN_CMOVE_SCREEN          42
#define     TOKEN_DRAW_STRING           43

#define     TOKEN_PUT_IMAGE             44
#define     TOKEN_PUT_IMAGE_SCREEN      45
#define     TOKEN_PUT_IMAGE_WINDOW      46
#define     TOKEN_PUT_IMAGE_XWINDOW     47
#define     TOKEN_GET_IMAGE_WINDOW      48
#define     TOKEN_GET_IMAGE_XWINDOW     50

#define     TOKEN_SKIP                  51
#define     TOKEN_STYLE                 52

#define     TOKEN_LAST_ONE              53
