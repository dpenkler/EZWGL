/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 * *             Copyright (C) 1996, 1997, 1998  Maorong Zou
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
/*****************************************************************
 ***                                                           ***
 ***              Widget RowColumn (aka GridBag)               ***
 ***                 (removed)                                 ***
 *****************************************************************/
#define _EZ_WIDGET_ROWCOL_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget  *EZ_CreateRowColumn MY_ANSIARGS((EZ_Widget *parent));
EZ_Widget  *EZ_CreateGridBag MY_ANSIARGS((EZ_Widget *parent));

/*********************************************************************/
EZ_Widget  *EZ_CreateGridBag(parent) EZ_Widget *parent;
{ return(EZ_CreateFrame(parent, NULL));}

EZ_Widget  *EZ_CreateRowColumn(parent) EZ_Widget *parent; 
{ return(EZ_CreateFrame(parent, NULL));}
/****************************************************************************/
#undef _EZ_WIDGET_ROWCOL_C_
