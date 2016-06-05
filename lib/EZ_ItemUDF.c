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
 *  Label item.
 *
 *****************************************************************/
#define _EZ_ITEM_UDF_C_
#include "EZ_Widget.h"

/*****************************************************************
 *
 * public functions
 */
EZ_Item *EZ_CreateUDFItem MY_ANSIARGS((int type));
void    EZ_setupUDFItem MY_ANSIARGS((EZ_Item *item));
void    *EZ_SearchUDFItemHandle MY_ANSIARGS(( int type));
void    EZ_DeleteAllUDFItemHandles MY_ANSIARGS((void));
int     EZ_RegisterItem MY_ANSIARGS((char *iname, char *cname,
				     void (*setup)  MY_ANSIARGS((EZ_Item *item)),
				     void (*computeSize) MY_ANSIARGS((EZ_Item *item, int *w, int *h)),
				     void (*draw) MY_ANSIARGS((EZ_Widget  *widget,
							       Drawable  drawable,
							       int       x0, int y0,
								EZ_Item   *item,
							       int       Rx, int Ry, int Rw, int Rh)),
				     void (*update) MY_ANSIARGS((EZ_Widget *widget,
								 Drawable  drawable,
								 EZ_Item   *item,
								 int       Rx, int Ry, int Rw, int Rh)),
				     void (*highlight) MY_ANSIARGS((EZ_Widget  *widget,
								    Drawable drawable,
								    EZ_Item  *item,
								    unsigned long fg, unsigned long bg,
								    int Rx, int Ry, int Rw, int Rh, int flag)),
				     void (*freePrivateData)  MY_ANSIARGS((EZ_Item *item)),
				     int  (*pick)  MY_ANSIARGS((EZ_Item *item, int x, int y)),
				     void (*postscript)  MY_ANSIARGS((EZ_Item *item, FILE *fp, void *psinfo)) ));
				     

EZ_UnknownDataType *EZ_GetUDFItemPrivateData MY_ANSIARGS((EZ_Item *item));
/*****************************************************************/

static void EZ_ComputeUDFItemSize MY_ANSIARGS((EZ_Item *item, int *w, int *h));
static void EZ_DrawUDFItem MY_ANSIARGS((EZ_Widget *widget,
					Drawable  drawable,
					int       x0, int y0,
					EZ_Item   *item,
					int       Rx, int Ry, int Rw, int Rh));
static void EZ_FreeUDFItemPrivateData MY_ANSIARGS((EZ_Item *item));
static void EZ_UpdateUDFItem MY_ANSIARGS((EZ_Widget *widget,
					  Drawable  drawable,
					  EZ_Item   *item,
					  int       Rx, int Ry, int Rw, int Rh));
static void EZ_HighlightUDFItem MY_ANSIARGS((EZ_Widget *widget,
					    Drawable  drawable,
					    EZ_Item   *item,
					    unsigned long fg, unsigned long bg,
					    int       Rx, int Ry, int Rw, int Rh, int hmode));
static int  EZ_PickAnUDFItem MY_ANSIARGS(( EZ_Item *item, int x, int y));
static void EZ_UDFItemPostscript MY_ANSIARGS(( EZ_Item *item, FILE *fp, void *psinfo));
/************************************************************************************/
static EZ_DpyItemHandle UDFItemHandle =
{
  EZ_ComputeUDFItemSize,
  EZ_DrawUDFItem,
  EZ_UpdateUDFItem,
  EZ_HighlightUDFItem,
  EZ_FreeUDFItemPrivateData,
  EZ_PickAnUDFItem,
  EZ_UDFItemPostscript,
};

/************************************************************************************/
typedef struct EZ_UDFItemHandle_ {
  int       type;
  XrmQuark  iname, cname;
  void      (*setup)  MY_ANSIARGS((EZ_Item *item));
  void      (*computeSize) MY_ANSIARGS((EZ_Item *item, int *w, int *h)); 
  void      (*draw) MY_ANSIARGS((EZ_Widget  *widget,
				 Drawable  drawable,
				 int       x0, int y0,
				 EZ_Item   *item,
				 int       Rx, int Ry, int Rw, int Rh));
  void      (*update) MY_ANSIARGS((EZ_Widget *widget,
				   Drawable  drawable,
				   EZ_Item   *item,
				   int       Rx, int Ry, int Rw, int Rh));
  void      (*highlight) MY_ANSIARGS((EZ_Widget  *widget,
				      Drawable drawable,
				      EZ_Item  *item,
				      unsigned long fg, unsigned long bg,
				      int       Rx, int Ry, int Rw, int Rh, int flag));       
  void      (*freePrivateData)  MY_ANSIARGS((EZ_Item *item));
  int       (*pick)  MY_ANSIARGS((EZ_Item *item, int x, int y));
  void      (*postscript)  MY_ANSIARGS((EZ_Item *item, FILE *fp, void *psinfo));

  struct EZ_UDFItemHandle_ *next;
} EZ_UDFItemHandle;
/************************************************************************************/
static EZ_UDFItemHandle *udfItemHandles;
/************************************************************************************/

/*****************************************************************
 *
 *  Functions dealing with LabelItem
 */
EZ_Item *EZ_CreateUDFItem(type)
     int type; 
{
  EZ_Item *item;
  EZ_UDFItemHandle  *handle = (EZ_UDFItemHandle *)EZ_SearchUDFItemHandle(type);
  if(handle)
    {
      if((item = (EZ_Item *)EZ_AllocateNewDisplayItem()))
	{
	  EZ_DpyItemHandlers[EZ_UDF_ITEM] = &UDFItemHandle;
          EZ_SetItemTypeAndNames(item, EZ_UDF_ITEM);
          if(handle->iname) EZ_ItemIName(item) = handle->iname;
          if(handle->cname) EZ_ItemCName(item) = handle->cname;

	  EZ_UDFItemData(item) = handle;
          memset(EZ_UDFItemCData(item), 0, sizeof(EZ_UDFItemCData(item)));
	}
    }
  else
    {
      fprintf(stderr, "Error creating item: Unknown item type: %d\n", type);
      item = NULL;
    }
  return(item);
}
/********************************************************************************/
void EZ_setupUDFItem(item)
     EZ_Item *item;
{
  if(item && EZ_ItemType(item) == EZ_UDF_ITEM)
    {
      EZ_UDFItemHandle *ptr = EZ_UDFItemData(item);
      if(ptr && ptr->setup) ptr->setup(item);
    }
}
/********************************************************************************/
static void EZ_ComputeUDFItemSize(item, w_ret, h_ret)
     EZ_Item *item;
     int *w_ret, *h_ret;
{
  if(item)
    {
      int w, h;
      EZ_UDFItemHandle *ptr = EZ_UDFItemData(item);
      if(ptr && ptr->computeSize)
	{
	  if( EZ_ItemDirty(item))
	    {
	      ptr->computeSize(item, &w, &h);
	      *w_ret = w;
	      *h_ret = h;
	    }
	  else
	    {
	      *w_ret = (int)EZ_ItemWidth(item);
	      *h_ret = (int)EZ_ItemHeight(item);
	    }
	  return;
	}
    }
  *w_ret = 0;
  *h_ret = 0;
}
/************************************************************************/

static void EZ_DrawUDFItem(widget, drawable, x0, y0, item, Rx, Ry, Rw,Rh)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int       x0, y0;
     int       Rx, Ry, Rw,Rh; /* confined region in drawable */
{
  if(item)
    {
      EZ_UDFItemHandle *ptr = EZ_UDFItemData(item);
      int aw = EZ_ItemAWidth(item);
      int ah = EZ_ItemAHeight(item);           
      EZ_ItemOriginX(item) = x0;
      EZ_ItemOriginY(item) = y0;
      EZ_ItemParentWidget(item) = widget;

      /* do a simple cliping.  Note: we still need to traverse
       * the whole tree to set the position of all nodes. The
       * geometry info is needed all over the place.
       */
      if(x0 > Rw + Rx || x0 + aw < Rx || y0 + ah < Ry || y0 > Rh +Ry)
	{
	  EZ_ItemVisible(item) = 0;	  
	  return;
	}
      EZ_ItemVisible(item) = 1;	  
      if(ptr && ptr->draw) ptr->draw(widget,drawable, x0, y0, item,Rx,Ry,Rw,Rh);
    }
}
/*****************************************************************************/

static void EZ_UpdateUDFItem(widget, drawable, item, cx,cy,cw,ch)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int cx,cy,cw,ch;     /* confined region in drawable */
{
  if(item && EZ_ItemVisible(item))
    {
      EZ_UDFItemHandle *ptr = EZ_UDFItemData(item);
      if(ptr && ptr->update) ptr->update(widget,drawable, item,cx,cy,cw,ch);
    }
}
/********************************************************************************************/
static void EZ_HighlightUDFItem(widget, drawable, item, fg, bg, cx,cy,cw,ch, hmode)    
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     unsigned long fg,bg; /* ~0 ~0 for default setting */
     int cx,cy,cw,ch;     /* confined region in drawable */
     int hmode;           /* method to highlight item */
{
  if(item)
    {
      if(item && EZ_ItemVisible(item))
	{
	  EZ_UDFItemHandle *ptr = EZ_UDFItemData(item);
	  if(ptr && ptr->highlight) ptr->highlight(widget,drawable, item, fg,bg,cx,cy,cw,ch, hmode);      
	}
    }
}
/**********************************************************************************************/
static int  EZ_PickAnUDFItem(item,x,y)
     EZ_Item *item;
     int x,y;
{
  if(item && EZ_ItemVisible(item))
    {
      EZ_UDFItemHandle *ptr = EZ_UDFItemData(item);
      if(ptr && ptr->pick) return(ptr->pick(item, x,y));
    }
  return(0);
}
/**********************************************************************************************/
static void EZ_FreeUDFItemPrivateData(item)
     EZ_Item *item;
{
  if(item)
    {
      EZ_UDFItemHandle *ptr = EZ_UDFItemData(item);
      if(ptr && ptr->freePrivateData)
	ptr->freePrivateData(item);
    }
}
/**********************************************************************************************/
static void EZ_UDFItemPostscript(item, fp, psinfo)
     EZ_Item *item; FILE *fp; void *psinfo;
{
}
/**************************************************************************************/     
void *EZ_SearchUDFItemHandle(type)
     int type;
{
  EZ_UDFItemHandle **ptr = &udfItemHandles;
  while(*ptr != NULL)
    {
      if( (*ptr)->type == type)
	{
	  return(*ptr);
	  break;
	}
      ptr = &((*ptr)->next);
    }
  return(NULL);
}
/************************************************************************/
void EZ_DeleteAllUDFItemHandles()
{
  EZ_UDFItemHandle *tmp, *ptr = udfItemHandles;  
  while(ptr)
    {
      tmp = ptr->next;
      (void)my_free((char *)ptr);
      ptr = tmp;
    }
  udfItemHandles = NULL;
}
/**************************************************************************************/     
int EZ_RegisterItem(iname, cname, setup, computeSize, draw, update, highlight, freePD, pick, ps)
     char *iname; char *cname;
     void (*setup)  MY_ANSIARGS((EZ_Item *item));
     void (*computeSize) MY_ANSIARGS((EZ_Item *item, int *w, int *h));
     void (*draw) MY_ANSIARGS((EZ_Widget  *widget,
			       Drawable  drawable,
			       int       x0, int y0,
			       EZ_Item   *item,
			       int       Rx, int Ry, int Rw, int Rh));
     void (*update) MY_ANSIARGS((EZ_Widget *widget,
				 Drawable  drawable,
				 EZ_Item   *item,
				 int       Rx, int Ry, int Rw, int Rh));
     void (*highlight) MY_ANSIARGS((EZ_Widget  *widget,
				    Drawable drawable,
				    EZ_Item  *item,
				    unsigned long fg, unsigned long bg,
				    int Rx, int Ry, int Rw, int Rh, int flag));
     void (*freePD)  MY_ANSIARGS((EZ_Item *item));
     int  (*pick)  MY_ANSIARGS((EZ_Item *item, int x, int y));
     void (*ps)  MY_ANSIARGS((EZ_Item *item, FILE *fp, void *psinfo));
{				     
  EZ_UDFItemHandle *udf;
  int type;

  if(udfItemHandles) type = udfItemHandles->type; else type=1024;
  udf = (EZ_UDFItemHandle *)my_malloc(sizeof(EZ_UDFItemHandle), _MISC_DATA_);
  if(udf == NULL) EZ_OutOfMemory("EZ_RegisterItem");
  udf->type = type;
  udf->setup = setup;
  udf->computeSize = computeSize;
  udf->draw = draw;
  udf->update = update;
  udf->highlight = highlight;
  udf->freePrivateData = freePD;
  udf->pick = pick;
  udf->postscript = ps;
  if(iname) udf->iname = XrmStringToQuark(iname);
  else  udf->iname = EZ_ItemDefaultQInstanceNames[EZ_UDF_ITEM];
  if(cname) udf->cname = XrmStringToQuark(cname);
  else  udf->cname = EZ_ItemDefaultQClassNames[EZ_UDF_ITEM];
  udf->next = udfItemHandles;
  udfItemHandles = udf;
  return(udf->type);
}
/**************************************************************************************/
EZ_UnknownDataType *EZ_GetUDFItemPrivateData(item) 
     EZ_Item *item;
{
  if(item && EZ_ItemType(item)== EZ_UDF_ITEM)
    return(EZ_UDFItemCData(item));
  return(NULL);
}
/**************************************************************************************/
#undef _EZ_ITEM_UDF_C_
