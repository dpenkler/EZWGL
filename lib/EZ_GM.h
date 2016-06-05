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
/******************************************************************
 ***                                                            ***
 ***   Data structures for non default gemoetry manager         ***
 ***                                                            ***
 ******************************************************************/
#ifndef _EZ_GM_H_
#define _EZ_GM_H_
/* Grid bag: row or column */
typedef struct gbrowcol_ {
  int   minWH;      /* minimal size, width or height, configured */
  int   cminWH;     /* minimal size, width or height, computed */
  int   offset;     /* offset in grid bag, from top or left */
  int   weight;     /* weight of the current row or column  */
  int   padding;    /* padding */
  int   tmp;  
} EZ_RowColStruct;

#define EZ_GridBagRCMinSize(ptr)    ((ptr)->minWH)
#define EZ_GridBagRCCMinSize(ptr)   ((ptr)->cminWH)
#define EZ_GridBagRCOffset(ptr)     ((ptr)->offset)
#define EZ_GridBagRCWeight(ptr)     ((ptr)->weight)
#define EZ_GridBagRCPadding(ptr)    ((ptr)->padding)
#define EZ_GridBagRCTmp(ptr)        ((ptr)->tmp)

typedef struct gbgmdata_ {
  int             type;
  EZ_RowColStruct *rows;
  EZ_RowColStruct *cols;
  int       nrows;
  int       ncols;
  int       allocedrows;
  int       allocedcols;
  int       xoffset;
  int       yoffset;
} EZ_GridBagGMData;

#define EZ_GBGMType(gbdptr)          ((gbdptr)->type)
#define EZ_GBGMRowPtr(gbdptr)        ((gbdptr)->rows)
#define EZ_GBGMColPtr(gbdptr)        ((gbdptr)->cols)
#define EZ_GBGMNRows(gbdptr)         ((gbdptr)->nrows)
#define EZ_GBGMNCols(gbdptr)         ((gbdptr)->ncols)
#define EZ_GBGMRowSpace(gbdptr)      ((gbdptr)->allocedrows)
#define EZ_GBGMColSpace(gbdptr)      ((gbdptr)->allocedcols)
#define EZ_GBGMXOffset(gbdptr)       ((gbdptr)->xoffset)
#define EZ_GBGMYOffset(gbdptr)       ((gbdptr)->yoffset)

/*****************************************************************/
typedef struct nullgmdata_ {
  int             type;
} EZ_NullGMData;

/*****************************************************************/
typedef struct rcgmdata_ {
  int             type;
  int             maxsize;

} EZ_RCGMData;

#define EZ_RCGMMaxSize(rcptr)        ((rcptr)->maxsize)

/*****************************************************************/
union geom_manager_data_ {
  int  type;
  EZ_GridBagGMData gbdt;
  EZ_NullGMData    nulldt;
  EZ_RCGMData      rcdt;
  /* may be other geometry managers later */
};

typedef union geom_manager_data_  EZ_GManager;

#define EZ_GMType(ptr)       ((ptr)->type)
#define EZ_GMGB(ptr)         ((ptr)->gbdt)
#define EZ_GMRC(ptr)         ((ptr)->rcdt)
/*****************************************************************/

#define EZ_DEFAULT_GEOMETRY_MANAGER   -1  
#define EZ_GRIDBAG_GEOMETRY_MANAGER    0
#define EZ_ROW_GEOMETRY_MANAGER        1
#define EZ_COLUMN_GEOMETRY_MANAGER     2
#define EZ_NULL_GEOMETRY_MANAGER       3

#define EZ_LAST_GEOMETRY_MANAGER       3

#define EZ_DEFAULT_GEOM_MANAGER   EZ_DEFAULT_GEOMETRY_MANAGER
#define EZ_GB_GEOM_MANAGER        EZ_GRIDBAG_GEOMETRY_MANAGER
#define EZ_ROW_GEOM_MANAGER       EZ_ROW_GEOMETRY_MANAGER  
#define EZ_COLUMN_GEOM_MANAGER    EZ_COLUMN_GEOMETRY_MANAGER  
#define EZ_NULL_GEOM_MANAGER      EZ_NULL_GEOMETRY_MANAGER  
#endif
/*****************************************************************/
