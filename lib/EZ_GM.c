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
/******************************************************************
 ***                                                            ***
 ***      managing gemoetry manager data                        ***
 ***                                                            ***
 ******************************************************************/
#define _EZ_GM_C_
#include "EZ_Widget.h"

/*****************************************************************/
EZ_GManager *EZ_AllocateGManager MY_ANSIARGS((int type));
void         EZ_DestroyGManager  MY_ANSIARGS((EZ_GManager *gma));
/*****************************************************************/

EZ_GManager *EZ_AllocateGManager(type) int type;
{
  EZ_GManager *gm;

  if(type > EZ_LAST_GEOMETRY_MANAGER) type = EZ_DEFAULT_GEOMETRY_MANAGER;
  gm = (EZ_GManager *)my_malloc(sizeof(EZ_GManager), _GEOM_MANAGER_);
  if(gm == NULL) EZ_OutOfMemory("EZ_AllocateGManager");
  memset(gm, 0, sizeof(EZ_GManager));
  EZ_GMType(gm) = type;
  return(gm);
}
/******************************************************************/ 
void  EZ_DestroyGManager(gm)
     EZ_GManager *gm;
{
  if(gm)
    {
      if(EZ_GMType(gm) == EZ_GB_GEOM_MANAGER)
	{
	  EZ_GridBagGMData *gbdt = &(EZ_GMGB(gm));
	  EZ_RowColStruct  *rows = EZ_GBGMRowPtr(gbdt);
	  EZ_RowColStruct  *cols = EZ_GBGMColPtr(gbdt);

	  if(rows) (void)my_free((char *)rows);
	  if(cols) (void)my_free((char *)cols);
	}
      (void)my_free( (char *)gm);
    }
}
/******************************************************************/ 
