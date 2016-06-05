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
 ***              Widget Tree                                  ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_TREE_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget  *EZ_CreateListTree MY_ANSIARGS((EZ_Widget *parent, int hsc, int vsc));
void       EZ_DrawWidgetTree  MY_ANSIARGS((EZ_Widget *widget));

void           EZ_GetListTreeWidgetComponents MY_ANSIARGS((EZ_Widget *widget,
							   EZ_Widget **itr, EZ_Widget **hsc,
							   EZ_Widget **vsc));
void           EZ_SetListTreeWidgetTree MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *tree));
EZ_TreeNode    *EZ_GetListTreeWidgetTree MY_ANSIARGS((EZ_Widget *widget));
EZ_TreeNode    *EZ_GetListTreeWidgetSelection MY_ANSIARGS((EZ_Widget *widget));

void        EZ_ListTreeWidgetSelectNode MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *theNode, int *location));
void        EZ_ListTreeWidgetSelectNodeUsingItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *theItem, int *location));
int         EZ_UpdateDirTreeWidget MY_ANSIARGS((EZ_Widget *widget));

void        EZ_GetListTreeWidgetXYOffset  MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));
void        EZ_SetListTreeWidgetXYOffset  MY_ANSIARGS((EZ_Widget *widget, int x, int y));

EZ_TreeNode *EZ_ListTreeFindNodeAtXY MY_ANSIARGS((EZ_Widget *tree, int x, int y));

EZ_TreeNode **EZ_ListTreeGetHighlightedNodesA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
EZ_Item     **EZ_ListTreeGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
int           EZ_ListTreeGetHighlightedNodes  MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode **ary, int siz));
int           EZ_ListTreeGetHighlightedItems  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **ary, int siz));
void 	      EZ_SetTreeWidgetRowBg MY_ANSIARGS((EZ_Widget *widget, int mode, char *bg1, char *bg2));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_TreeHandle =
{ 
  EZ_ComputeWidgetUnknownSize,
  EZ_DrawWidgetTree,
  EZ_FreeUnknownData,
  EZ_UnkownEventHandle,
};

/*********************************************************************/
EZ_Widget  *EZ_CreateListTree(parent, hscrollbar, vscrollbar)
     EZ_Widget  *parent;     /* parent widget    */
     int hscrollbar, vscrollbar;
{
  EZ_Widget  *wptr, *itree, *hscroll=NULL, *vscroll=NULL;

  wptr = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_TREE] = &EZ_TreeHandle;
  
  EZ_WidgetStacking(wptr) = EZ_VERTICAL;
  EZ_SetWidgetTypeAndNames(wptr, EZ_WIDGET_TREE);
  EZ_WidgetBorderStyle(wptr)  = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(wptr)  = 0;
  EZ_WidgetFillMode(wptr) = EZ_FILL_VERTICALLY;

  EZ_WidgetPadX(wptr) = 0;
  EZ_WidgetPadY(wptr) = 0;
  EZ_WidgetSepX(wptr)= 0;
  EZ_WidgetSepY(wptr)= 0;

  EZ_SetRowColConstrains(wptr, EZ_ROW,    0, 64, 1000, 0);
  EZ_SetRowColConstrains(wptr, EZ_ROW,    1, 0,  0,    0);
  EZ_SetRowColConstrains(wptr, EZ_COLUMN, 0, 64, 1000, 0);
  EZ_SetRowColConstrains(wptr, EZ_COLUMN, 1, 0,  0,    0);

  itree = EZ_TreeWITree(wptr) = EZ_CreateITree(wptr);
  EZ_WidgetGroupLeader(itree) = wptr;
  EZ_WidgetGBX(itree) = 0;
  EZ_WidgetGBY(itree) = 0;
  EZ_WidgetGBW(itree) = 1;
  EZ_WidgetGBH(itree) = 1;

  EZ_SetWidgetExpandFlag(itree);

  if(vscrollbar)
    {
      vscroll = EZ_TreeWVScroll(wptr) = EZ_CreateScrollbar(wptr,EZ_WIDGET_VERTICAL_SCROLLBAR);
      EZ_SetWidgetExpandFlag(vscroll);
      EZ_ScrollObject(vscroll) = itree;
      EZ_WidgetGroupLeader(vscroll) = wptr;
      EZ_WidgetGBX(vscroll) = 1;
      EZ_WidgetGBY(vscroll) = 0;
      EZ_WidgetGBW(vscroll) = 1;
      EZ_WidgetGBH(vscroll) = 1;
    }
  if(hscrollbar)
    {
      hscroll = EZ_TreeWHScroll(wptr) = EZ_CreateScrollbar(wptr,EZ_WIDGET_HORIZONTAL_SCROLLBAR);
      EZ_SetWidgetExpandFlag(hscroll);
      EZ_ScrollObject(hscroll) = itree;
      EZ_WidgetGroupLeader(hscroll) = wptr;
      EZ_WidgetGBX(hscroll) = 0;
      EZ_WidgetGBY(hscroll) = 1;
      EZ_WidgetGBW(hscroll) = 1;
      EZ_WidgetGBH(hscroll) = 1;
    }
  EZ_ITreeWHScroll(itree) = hscroll;
  EZ_ITreeWVScroll(itree) = vscroll;  
  EZ_ITreeWTree(itree) = wptr;   

  return(wptr);
}
 
/*******************************************************************
 *
 *  Draw A Label Widget.
 */
void  EZ_DrawWidgetTree(wptr)
     EZ_Widget *wptr;
{
  if( EZ_WidgetNumChildren(wptr)> 2 ||
      (EZ_TreeWHScroll(wptr) == NULL &&  EZ_WidgetNumChildren(wptr) > 1))
    {
      int             w, h;
      Pixmap          pixmap;
      Window          win;
      GC              gc;
      unsigned long   bgpv;
      
      win = EZ_WidgetWindow(wptr);
      w   = EZ_WidgetWidth(wptr);
      h   = EZ_WidgetHeight(wptr);
  
      /*-----------------------------------------------------------
       *  Create a pixmap, draw into this pixmap in background and
       *  copy to the button window when finished.
       *----------------------------------------------------------*/
      pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth);    
      if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	  XSetTSOrigin(EZ_Display, gc, ox, oy);
	}
      XFillRectangle(EZ_Display, pixmap, gc, 0,0, w, h); 
      
      EZ_DrawRectBorder(wptr, pixmap);     
      XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
      XFreePixmap(EZ_Display, pixmap); 
    }
}

/***************************************************************************
 *
 *  Supporting functions.
 */
void EZ_SetListTreeWidgetTree(widget, root)
     EZ_Widget *widget;
     EZ_TreeNode   *root;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)EZ_SetITreeWTree(twidget, root);
    }
}

EZ_TreeNode *EZ_GetListTreeWidgetTree(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget  = widget;
	default:
	  break;
	}
      if(twidget) return(EZ_ITreeWRoot(twidget));
    }
  return(NULL);
}
/************************************************************************************/
EZ_TreeNode *EZ_GetListTreeWidgetSelection(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget  = widget;
	default:
	  break;
	}
      if(twidget) return(EZ_ITreeWSelection(twidget));
    }
  return(NULL);
}
/************************************************************************************/
void  EZ_GetListTreeWidgetComponents(widget, itree_ret, h_ret, v_ret)
     EZ_Widget *widget;
     EZ_Widget **itree_ret, **h_ret, **v_ret;
{
  if(widget)
    {
      if( EZ_WidgetType(widget) == EZ_WIDGET_TREE)
        {
          if(itree_ret) *itree_ret = EZ_TreeWITree(widget);
          if(h_ret) *h_ret = EZ_TreeWHScroll(widget);
          if(v_ret) *v_ret = EZ_TreeWVScroll(widget);
        }
      else if( EZ_WidgetType(widget) == EZ_WIDGET_ITREE)
        {
          if(itree_ret) *itree_ret = widget;
          if(h_ret) *h_ret = EZ_ITreeWHScroll(widget);
          if(v_ret) *v_ret = EZ_ITreeWVScroll(widget);
        }
    }
}

void  EZ_ListTreeWidgetSelectNode(widget, node, location)
     EZ_Widget *widget; EZ_TreeNode *node; int *location;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget  = widget;
	default:
	  break;
	}
      if(twidget)
	EZ_ITreeSelectNode(twidget, node, location);
    }
}
void EZ_ListTreeWidgetSelectNodeUsingItem(widget, item, location)
     EZ_Widget *widget; EZ_Item *item; int *location;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget  = widget;
	default:
	  break;
	}
      if(twidget)
	EZ_ITreeSelectNodeUsingItem(twidget, item, location);
    }
}
/********************************************************************/
static int dirTreeVisible(root, flag)  EZ_TreeNode *root; int *flag;
{
  if(root)
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(root);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(root);
      EZ_Item *item = EZ_TreeNodeItem(root);
      if(item && EZ_ItemVisible(item)) 
	{ 
	  *flag += 1;
	  if(*flag > 1) return(1);
	}
      if(dirTreeVisible(children, flag)) return(1);
      if(dirTreeVisible(sibling, flag)) return(1);
    }
  return(0);
}

int EZ_UpdateDirTreeWidget(widget)
     EZ_Widget *widget;
{
  int modified = 0;
  if(EZ_LookupWidgetFromAllHT(widget))
    {
      EZ_Widget *twidget = NULL;
      int type = EZ_WidgetType(widget);
      if(type ==  EZ_WIDGET_TREE) twidget = EZ_TreeWITree(widget);
      else if(type ==  EZ_WIDGET_ITREE) twidget = widget;
      if(twidget)
	{
	  EZ_TreeNode *root =  EZ_ITreeWRoot(twidget);
	  if(root)
	    {
	      EZ_Item *item = EZ_TreeNodeItem(root);
	      if(item && EZ_ItemDirItemInfo(item) != NULL)
		{
		  EZ_TreeNode *sel = EZ_ITreeWSelection(twidget);
		  int sel_deleted = 0;
                  modified = EZ_UpdateDirTreeWork(root, sel, &sel_deleted);
		  if(sel_deleted)
		    {
		      modified++;
		      EZ_ITreeWSelection(twidget) = NULL;
		    }
		  if(modified)
		    {
		      int flag = 0;
		      EZ_ItemDirty(item) = 1;
		      EZ_DrawWidget(twidget);
		      if(sel && sel_deleted == 0)
			EZ_ITreeSelectNode(twidget, sel, NULL);
		      else if(!dirTreeVisible(root, &flag))
			{
			  int cw = EZ_WidgetPadX(twidget) + EZ_WidgetBorderWidth(twidget) +
			    EZ_WidgetPadB(twidget);
			  int ch = EZ_WidgetPadY(twidget) + EZ_WidgetBorderWidth(twidget) + 
			    EZ_WidgetPadB(twidget);

			  EZ_ITreeWXStart(twidget) = cw;
			  EZ_ITreeWYStart(twidget) = ch;
			  EZ_DrawWidget(twidget);
			}
		    }
		}
	    }
	}
    }
  return(modified);
}
/****************************************************************************/
void EZ_GetListTreeWidgetXYOffset(widget, x, y)
     EZ_Widget *widget; int *x, *y;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget  = widget;
	default:
	  break;
	}
      if(twidget)
	{
	  if(x) *x = EZ_ITreeWXStart(twidget);
	  if(y) *y = EZ_ITreeWYStart(twidget);
	}
    }
}
void EZ_SetListTreeWidgetXYOffset(widget, x, y)
     EZ_Widget *widget; int x, y;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget  = widget;
	default:
	  break;
	}
      if(twidget)
	{
	  EZ_ITreeWXStart(twidget) = x;
	  EZ_ITreeWYStart(twidget) = y;
	  EZ_ITreeWDirty(twidget) = 1;
	  if(EZ_WidgetMapped(widget)) EZ_DisplayWidget(widget);
	}
    }
}

/****************************************************************************/
EZ_TreeNode *EZ_ListTreeFindNodeAtXY(widget, x, y)
     EZ_Widget *widget; int x; int y;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget  = widget;
	default:
	  break;
	}
      widget = twidget;
      if(widget)
	{
	  EZ_TreeNode *root = EZ_ITreeWRoot(widget);
	  return( EZ_FindPointerOnTreeNode(x,y, root));
	}
    }
  return(NULL);
}
/****************************************************************************/
EZ_TreeNode **EZ_ListTreeGetHighlightedNodesA(widget, cnt) EZ_Widget *widget; int *cnt;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget  = widget;
	default:
	  break;
	}
      if(twidget) return(EZ_ITreeGetHighlightedNodesA(twidget, cnt));
    }
  return(NULL);
}

EZ_Item  **EZ_ListTreeGetHighlightedItemsA(widget, cnt) EZ_Widget *widget; int *cnt;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget  = widget;
	default:
	  break;
	}
      if(twidget) return(EZ_ITreeGetHighlightedItemsA(twidget, cnt));
    }
  return(NULL);
}

int EZ_ListTreeGetHighlightedNodes(widget, ary, siz)
     EZ_Widget *widget; EZ_TreeNode **ary; int siz;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget  = widget;
	default:
	  break;
	}
      if(twidget) return(EZ_ITreeGetHighlightedNodes(twidget, ary, siz));
    }
  return(0);
}

int EZ_ListTreeGetHighlightedItems(widget, ary, siz)
     EZ_Widget *widget; EZ_Item **ary; int siz;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget  = widget;
	default:
	  break;
	}
      if(twidget) return(EZ_ITreeGetHighlightedItems(twidget, ary, siz));
    }
  return(0);
}
/****************************************************************************/
void EZ_SetTreeWidgetRowBg (widget, mode, bg1, bg2)
     EZ_Widget *widget; char *bg1, *bg2; int mode;
{
  if(widget)
    {
      EZ_Widget *twidget = NULL;
      switch( EZ_WidgetType(widget))
	{
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_ITREE:
	  twidget = widget;
	  break;
	default:
	  break;
	}
      if(twidget)
	{
	  unsigned long *bg = EZ_ITreeWRowBg(twidget);
	  EZ_ITreeWRowBgSet(twidget) = mode;
	  if(bg1) bg[0]= EZ_AllocateColorFromName(bg1);
	  else  bg[0] = 0;
	  if(bg2) bg[1]= EZ_AllocateColorFromName(bg2);
	  else bg[1] = 0;
	}
    }
}
/****************************************************************************/
#undef _EZ_WIDGET_TREE_C_
