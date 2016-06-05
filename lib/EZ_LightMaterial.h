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
/**************************************************************
 *
 *  Header file for Lighting functions
 */

#ifndef _EZ_LIGHT_MAT_
#define _EZ_LIGHT_MAT_

/**************************************************************
 *
 *  Material entry
 */

typedef struct {
  float emission[4];        /* emission color    */
  float ambient[4];         /* ambient color     */
  float diffuse[4];         /* diffuse color     */
  float specular[4];        /* specular color    */
  float shininess;          /* specular exponent */
  float aindex;             /* lighting in CMap  */
  float dindex;
  float sindex;
} EZ_Material;

/**************************************************************
 *
 * Light source entry
 */
typedef struct {
  float ambient[4];         /* ambient color        */
  float diffuse[4];         /* diffuse color        */
  float specular[4];        /* specular color       */
  float direction[4];       /* lightSrc direction   */
  float position[4];        /* lightSrc position    */
  float nposition[4];       /* normalized position  */
  float spotE;              /* spot exponent        */
  float spotA;              /* cutoff angle         */
  float att[3];             /* attenuation coeffs   */

  /* 
   * the following data are kept for accelerating 
   * lighting calculations for infinite lighting models
   */
  float Fambient[4];        
  float Bambient[4];
  float Fdiffuse[4];
  float Bdiffuse[4];
  float Fspecular[4];
  float Bspecular[4];
  float specscalar;
} EZ_LightSrc;

/**************************************************************
 *
 * Lighting model
 */
typedef struct {
  float ambient[4];         /* ambient color     */
  int   localviewer;        /* local viewer ?    */
  int   twoside;            /* twoside shading ? */
} EZ_LightModel;

/**************************************************************
 *
 *  here is a list of predefined materials
 */
#define EZ_SILVER          0
#define EZ_SILVER_S        1
#define EZ_GOLD            2
#define EZ_GOLD_S          3
#define EZ_BRASS           4
#define EZ_COPPER          5    
#define EZ_COPPER_S        6
#define EZ_BRONZE          7
#define EZ_RUBY            8
#define EZ_TURQUOISE       9  
#define EZ_JADE            10
#define EZ_EMERALD         11
#define EZ_PERAL           12
/**************************************************************/
#endif


