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
 *
 *  Display items
 *
 *****************************************************************/
#define _EZ_CREATE_ITEM_C_

#include "EZ_Widget.h"
#include "EZ_WidgetConfig.h"


/*****************************************************************
 *
 * public functions
 */
EZ_Item *EZ_CreateItemXrm MY_VARARGS(int,  the_args)
     MY_VARARGS_DECL
{
  va_list         ap;
  EZ_Item         *item = NULL;
  int              type;
  char      *iname,  *cname;
  XrmQuark  qiname,  qcname; 

  type = (int)( MY_VARARGS_START(int, the_args, ap));
  iname = (char *)va_arg(ap, char *);
  cname = (char *)va_arg(ap, char *);
  if(iname != NULL) qiname = EZ_StringToQuark(iname);
  else qiname = NULLQUARK;
  if(cname != NULL) qcname = EZ_StringToQuark(cname);
  else qcname = NULLQUARK;

  switch(type)
    {
    case EZ_LABEL_ITEM:
      item = EZ_CreateLabelItem(NULL, NULL);
      break;
    case EZ_WIDGET_ITEM:
      item = EZ_CreateWidgetItem(NULL);
      break;
    case EZ_FIG_ITEM:
      item = EZ_CreateFigItem();      
      break;
    default:
      item = EZ_CreateUDFItem(type);
      break;
    }

  if(item)
    {
      if(qiname != NULLQUARK) EZ_ItemIName(item) = qiname;
      if(qcname != NULLQUARK) EZ_ItemCName(item) = qcname;
      
#include "EZ_ItemAttr.h"

      /* resources */
      EZ_GetAndUseItemResources(item);
    }
  return(item);
}
/*****************************************************************
 *
 * public functions
 */
EZ_Item *EZ_CreateItem MY_VARARGS(int,  the_args)
     MY_VARARGS_DECL
{
  va_list         ap;
  EZ_Item         *item = NULL;
  int              type;

  type = (int)( MY_VARARGS_START(int, the_args, ap));

  switch(type)
    {
    case EZ_LABEL_ITEM:
      item = EZ_CreateLabelItem(NULL, NULL);
      break;
    case EZ_WIDGET_ITEM:
      item = EZ_CreateWidgetItem(NULL);
      break;
    case EZ_FIG_ITEM:
      item = EZ_CreateFigItem();      
      break;
    default:
      item = EZ_CreateUDFItem(type);
      break;
    }

  if(item)
    {
#include "EZ_ItemAttr.h"
      /* resources */
      EZ_GetAndUseItemResources(item);
    }
  return(item);
}
/*****************************************************************/
#undef _EZ_CREATE_ITEM_C_
