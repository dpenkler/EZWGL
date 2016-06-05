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
 *  For EZ_NewList()
 */
#define EZ_COMPILE                1
#define EZ_COMPILE_AND_EXECUTE    2

/*
 *  For FrontFace()
 */
#define EZ_CCW               0
#define EZ_CW                1

/*
 *  For MatrixMode()
 */
#define EZ_PROJECTION      0
#define EZ_MODELVIEW       1
#define EZ_TEXTURE         2

/*
 * For Begin()/End()
 */
#define  EZ_LINES                        1
#define  EZ_LINE_STRIP                   2
#define  EZ_LINE_LOOP                    3
#define  EZ_TRIANGLES                    4
#define  EZ_TRIANGLE_STRIP               5
#define  EZ_TRIANGLE_FAN                 6
#define  EZ_QUADS                        7
#define  EZ_QUAD_STRIP                   8
#define  EZ_POLYGON                      9
#define  EZ_POINTS                       10
#define  EZ_TMESH                        11
#define  EZ_QMESH                        12
#define  EZ_SPHERE_TMESH                 EZ_TMESH
#define  EZ_SPHERE_QMESH                 EZ_QMESH

/*
 * For ColorMaterial()/Materialfv()/Lightfv()
 */
#define EZ_FRONT                  0
#define EZ_BACK                   1
#define EZ_FRONT_AND_BACK         2

#define EZ_EMISSION               1
#define EZ_AMBIENT                2
#define EZ_DIFFUSE                3
#define EZ_SPECULAR               4

#define EZ_SHININESS              5
#define EZ_AMBIENT_AND_DIFFUSE    6
#define EZ_COLOR_INDICES          7

#define EZ_POSITION               8
#define EZ_SPOT_DIRECTION         9
#define EZ_SPOT_EXPONENT          10
#define EZ_SPOT_CUTOFF            11
#define EZ_CONSTANT_ATTENUATION   12
#define EZ_LINEAR_ATTENUATION     13
#define EZ_QUADRATIC_ATTENUATION  14

/*
 * For LightModelf()
 */
#define EZ_LIGHT_MODEL_AMBIENT         1
#define EZ_LIGHT_MODEL_LOCAL_VIEWER    2
#define EZ_LIGHT_MODEL_TWO_SIDE        3



/*
 * EZ_Fogfv()
 */
#define EZ_FOG_MODE                    1
#define EZ_FOG_DENSITY                 2
#define EZ_FOG_COLOR                   3
#define EZ_FOG_INDEX                   4
#define EZ_FOG_START                   5
#define EZ_FOG_END                     6

#define EZ_LINEAR                      1
#define EZ_EXP                         2
#define EZ_EXP2                        3

/*
 *  For Enable()/Disable()
 */
#define EZ_LIGHT0                 0
#define EZ_LIGHT1                 1
#define EZ_LIGHT2                 2
#define EZ_LIGHT3                 3
#define EZ_LIGHT4                 4
#define EZ_LIGHT5                 5
#define EZ_LIGHT6                 6
#define EZ_LIGHT7                 7

#define EZ_LIGHTING               8

#define EZ_NORMALIZE              9
#define EZ_COLOR_MATERIAL         10
#define EZ_CULL_FACE              11
#define EZ_FOG                    12
#define EZ_DEPTH_TEST             13
#define EZ_DITHER                 14

/*
 * EZ_DefMaterial()/EZ_DefLight()
 */
#define EZ_NULL_F                     0.0 
#define EZ_EMISSION_F               301.0
#define EZ_AMBIENT_F                302.0
#define EZ_DIFFUSE_F                303.0
#define EZ_SPECULAR_F               304.0

#define EZ_SHININESS_F              305.0
#define EZ_COLOR_INDICES_F          306.0

#define EZ_POSITION_F               307.0
#define EZ_SPOT_DIRECTION_F         308.0
#define EZ_SPOT_EXPONENT_F          309.0
#define EZ_SPOT_CUTOFF_F            310.0
#define EZ_CONSTANT_ATTENUATION_F   311.0
#define EZ_LINEAR_ATTENUATION_F     312.0
#define EZ_QUADRATIC_ATTENUATION_F  313.0

#define EZ_LIGHT_MODEL_AMBIENT_F       314.0 
#define EZ_LIGHT_MODEL_LOCAL_VIEWER_F  315.0
#define EZ_LIGHT_MODEL_TWO_SIDE_F      316.0


/*
 * For EZ_Clear()
 */
#define EZ_COLOR_BUFFER_BIT       0x01
#define EZ_DEPTH_BUFFER_BIT       0x02

/*
 * For EZ_Clear()
 */
#define  EZ_VECTOR_FONT           0x01
#define  EZ_BITMAP_FONT           0x02

/*
 * For EZ_PolygonMode()
 */
#define EZ_FILL             0x0
#define EZ_LINE             0x1
#define EZ_POINT            0x2

/*
 * For EZ_LogicOp
 */
#define  EZ_CLEAR            0x0
#define  EZ_AND              0x1
#define  EZ_AND_REVERSE      0x2
#define  EZ_COPY             0x3
#define  EZ_AND_INVERTED     0x4
#define  EZ_NOOP             0x5
#define  EZ_XOR              0x6
#define  EZ_OR               0x7
#define  EZ_NOR              0x8
#define  EZ_EQUIV            0x9
#define  EZ_INVERT           0xa
#define  EZ_OR_REVERSE       0xb
#define  EZ_COPY_INVERTED    0xc
#define  EZ_OR_INVERTED      0xd
#define  EZ_NAND             0xe
#define  EZ_SET              0xf
