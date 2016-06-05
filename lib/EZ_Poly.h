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
/************************************************************
 *
 *  Header file polygon conversion functions.
 *
 */

/************************************************************
 *
 * These are the data structure for storing color info for
 * an edge.
 */
#ifndef EZ_POLY_H
#define EZ_POLY_H
typedef struct {
  int ri, dri;       /* rgb components and increments   */
  int g, dg;
  int b, db;
} EZ_EdgeColorInfo;

/************************************************************
 *
 *  This is the data structure needed for Bresenham 
 *  algorithm for scan converting polygons. 
 */
typedef struct {
  int               x;               /* minor axis start             */
  int               d;               /* decision variable            */
  int               s,s1;            /* slopes of this edge          */
  int               incr1, incr2;    /* error increments             */
  int               z, dz;           /* starting depth, z increment  */
  EZ_EdgeColorInfo  clr;             /* color info                   */
} B_Info;

/************************************************************
 *
 * This is the data structure for edge table entry. Edges
 * are linked together in the Edge Table, ordered by scanlines.
 * 
 */
typedef struct _EZ_Edge {
  int             ymax;    /* y coor at which this edge expires */
  B_Info          info;    /* info for Bresenham algorithm      */
  struct _EZ_Edge *next;   /* next edge in the list             */
  struct _EZ_Edge *prev;   /* previous edge in the list         */
} EZ_Edge;

/************************************************************
 *
 * This is the data structure of scanline list. 
 */
typedef struct _EZ_Scanline {
  int                  sline;      /* the current scanline               */
  EZ_Edge              *edgelist;  /* edges which enter at this scanline */
  struct _EZ_Scanline  *next;      /* ptr to the next scanline           */
} EZ_Scanline;


/************************************************************
 *
 * This is the data structure for an edge table.
 * Note: the Active Edge Table is just a list of edges.
 *
 *  Edge table structure for a polygon
 *    _________________
 *   |                 |
 *   | ymin, ymax      |
 *   | scanline_list   |
 *   |       |         |
 *    -------+---------
 *           |
 *    _______v_____    __________      _________
 *   |             |  |          |    |         |
 *   | scanline 1 -+->| edge 11  | -> | edge 12 | -> ...
 *   |     |       |  |          | <- |         | <- 
 *    -----+-------    ----------      ---------
 *         |
 *    _____v_______    __________      _________
 *   |             |  |          |    |         |
 *   | scanline 2 -+->| edge 21  | -> | edge 22 | -> ...
 *   |     |       |  |          | <- |         | <- 
 *    -----+-------    ----------      ---------
 *         |
 *         v
 *       .....
 *
 */

typedef struct {
  int         ymin;         /* ymin for the current polygon */
  int         ymax;         /* ymax for the current polygon */
  EZ_Scanline slines;       /* scanline list for this pgon  */
} EZ_ETable;
/************************************************************/
#endif
