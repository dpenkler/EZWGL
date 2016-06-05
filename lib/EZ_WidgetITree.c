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
 ***              Widget Internal Tree                         ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_ITREE_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget  *EZ_CreateITree  MY_ANSIARGS((EZ_Widget *parent));
void       EZ_DrawWidgetITree MY_ANSIARGS((EZ_Widget *widget));
void       EZ_ComputeWidgetITreeSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void       EZ_FreeWidgetITreeData MY_ANSIARGS((EZ_Widget *widget));
void       EZ_ITreeEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EZ_TreeNode *EZ_FindPointerOnTreeNode MY_ANSIARGS((int x, int y, EZ_TreeNode *root));
EZ_TreeNode *EZ_FindTreeAdjBranch MY_ANSIARGS((EZ_TreeNode *root, EZ_TreeNode *node, int dir));
void        EZ_SetITreeWTree MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *tree));
void        EZ_FlashItemOutline MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, 
					     int Rx, int Ry, int Rw, int Rh));
void        EZ_DnDOutlineItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, 
					     int Rx, int Ry, int Rw, int Rh));

void        EZ_ITreeSelectNode MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *theNode, int *location));
void        EZ_ITreeSelectNodeUsingItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *theItem, int *location));

/*********************************************************************/
EZ_TreeNode **EZ_ITreeGetHighlightedNodesA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
EZ_Item     **EZ_ITreeGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
int           EZ_ITreeGetHighlightedNodes  MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode **ary, int siz));
int           EZ_ITreeGetHighlightedItems  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **ary, int siz));
/*********************************************************************/
void tree_scroll_update_selection MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *node));

static void EZ_ClearITreeHighlightedItems MY_ANSIARGS((EZ_Widget *widget, int Rx, int Ry, int Rw, int Rh));
static void EZ_ITreeHighlightItemsInRectangle MY_ANSIARGS((EZ_Widget *widget, 
							   int rx, int ry, int rw, int rh));
static EZ_TreeNode *itreeFindHighlightedNode MY_ANSIARGS((EZ_TreeNode *node));
static void itreeDnDNNodes MY_ANSIARGS((EZ_Widget *widget, int nhodes,  XEvent *xe,
					int Rx, int Ry,int Rw,int Rh));
static void itreeDnDOneNode MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *nd,  XEvent *xe,
					 int Rx, int Ry,int Rw,int Rh));
static int  itreeGetNumHighlightedNodes MY_ANSIARGS((EZ_Widget *widget));
static void itreeMakeNNodeOutlines MY_ANSIARGS((EZ_Widget *widget,int nnn,int wox,int woy));

static EZ_TreeNode *EZ_PointerOnBranchNode MY_ANSIARGS((int x, int y, EZ_TreeNode *node));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_ITreeHandle =
{ 
  EZ_ComputeWidgetITreeSize,
  EZ_DrawWidgetITree,
  EZ_FreeWidgetITreeData,
  EZ_ITreeEventHandle,
};

static void update_itree_scrollbars MY_ANSIARGS((EZ_Widget *widget));
static void ITreeWTimerCallBack MY_ANSIARGS((EZ_Timer *timer, void *data));
/*********************************************************************/

EZ_Widget  *EZ_CreateITree(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *tmp;
  
  tmp = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for label.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_ITREE] = &EZ_ITreeHandle;

  EZ_SetWidgetTypeAndNames(tmp,EZ_WIDGET_ITREE);
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_DOWN;
  EZ_WidgetBorderWidth(tmp)  = 2;
  EZ_WidgetPadX(tmp) = 1;
  EZ_WidgetPadY(tmp) = 1;
  EZ_WidgetPadB(tmp) = 2;

  EZ_ITreeWRoot(tmp) = (EZ_TreeNode *)NULL;
  EZ_ITreeWSelection(tmp) = (EZ_TreeNode *)NULL;

  EZ_ITreeWHighlightBG(tmp) = ~0; 
  EZ_ITreeWHighlightFG(tmp) = ~0;
  EZ_ITreeWTextBG(tmp) = ~0;

  EZ_ITreeWYSize(tmp)  = 1;
  EZ_ITreeWYPageSize(tmp) = 1;
  EZ_ITreeWYStart(tmp)   = 0;
  EZ_ITreeWXSize(tmp) = 1;
  EZ_ITreeWXPageSize(tmp) = 1;
  EZ_ITreeWXStart(tmp) = 0;
  EZ_ITreeWDisplayed(tmp) = 0;

  EZ_ITreeWHScroll(tmp) = (EZ_Widget *)NULL; 
  EZ_ITreeWVScroll(tmp) = (EZ_Widget *)NULL;
  EZ_ITreeWTree(tmp) = (EZ_Widget *)NULL;
  EZ_ITreeWDirty(tmp) = 1;
  /* blinking selection timer */
  (void)EZ_CreateTimer(0,350000,-1,(EZ_CallBack)ITreeWTimerCallBack, tmp, 0);
  
  EZ_SetWidgetFocusableFlag(tmp);  

  EZ_Insert2DnDWidgetList(tmp);
  EZ_WidgetDnDHandler(tmp) = (EZ_DnDMsgHandler)EZ_ITreeDnDHandle;   

  return(tmp);
}

void EZ_FreeWidgetITreeData(widget)
     EZ_Widget *widget;
{
  if(EZ_ITreeWRoot(widget) != (EZ_TreeNode *)NULL)
    EZ_TreeDestroySubtree(EZ_ITreeWRoot(widget));
}
 
/********************************************************************
 *
 *  Figure out the dimension of a label widget.
 */

void EZ_ComputeWidgetITreeSize(widget, w, h)
     EZ_Widget *widget;
     int             *w, *h;
{
  int  cw, ch;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      EZ_SetWidgetSizeComputedFlag(widget);
      EZ_ITreeWDirty(widget) = 1;
      EZ_ITreeWYPageSize(widget) = 30;
      EZ_ITreeWXPageSize(widget) = 30;
    }
  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
  /*
  if(EZ_WidgetMapped(widget) == 0)
    {
      EZ_ITreeWXStart(widget) = 0;
      EZ_ITreeWYStart(widget) = 0;
    }
    */
  cw = cw + cw + 30;
  ch = ch + ch + 30;

  *w = cw;
  *h = ch;
}

/*******************************************************************
 *
 *  Draw A Label Widget.
 */
void  EZ_DrawWidgetITree(wptr)
     EZ_Widget *wptr;
{
  int       w, h,x,y, bw, padb, padb2;
  Pixmap    pixmap;
  Window    win;
  GC        gc;
  unsigned long   bgpv;
  EZ_TreeNode *rt;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  bw  = EZ_WidgetBorderWidth(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;

  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 
  {
    if(EZ_ITreeWTextBG(wptr) != ~0)
      {
	bgpv = EZ_ITreeWTextBG(wptr);
	XSetForeground(EZ_Display, EZ_WRITABLEGC, bgpv);	
	gc = EZ_WRITABLEGC;
      }
    else 
      {
	if(EZ_GetBackgroundGC(wptr, &gc, &bgpv, 0, 0))
	  {
	    int ox, oy;
	    EZ_GetWidgetTileOrigin(wptr, &ox, &oy);      
	    XSetTSOrigin(EZ_Display, gc, ox, oy);
	  }
      }
  }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w -padb2, h-padb2); 
  
  rt = EZ_ITreeWRoot(wptr);
  if(rt)
    {
      EZ_Item *item = EZ_TreeNodeItem(rt);
      int xoff = EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadX(wptr) + padb;
      int yoff = EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + padb;
      int parity = 0;
      if(EZ_ItemDirty(item) || EZ_ITreeWDirty(wptr)) 
	{
	  EZ_ComputeSubtreeSize(rt, &x, &y, &parity);

	  /* update the geometry of the tree for scrolling */
	  EZ_ITreeWXSize(wptr) = EZ_TreeNodeTWidth(rt) + (xoff << 1);
	  EZ_ITreeWYSize(wptr) = EZ_TreeNodeTHeight(rt)+ (yoff << 1);
	  EZ_ITreeWXPageSize(wptr) = EZ_WidgetWidth(wptr);
	  EZ_ITreeWYPageSize(wptr) = EZ_WidgetHeight(wptr);

	  if(EZ_ITreeWXStart(wptr) + EZ_ITreeWXSize(wptr) < 0)
	    {
	      int itmp = w - EZ_ITreeWXSize(wptr);
	      EZ_ITreeWXStart(wptr) = itmp > 0? 0: itmp;

	    }
	  if(EZ_ITreeWYStart(wptr) + EZ_ITreeWYSize(wptr) < 0)
	    {
	      int itmp = h - EZ_ITreeWYSize(wptr); 
	      EZ_ITreeWYStart(wptr) = itmp > 0? 0: itmp;
	    }
	  EZ_ITreeWDirty(wptr) = 0;
	}

      x =  EZ_ITreeWXStart(wptr) + xoff;
      y =  EZ_ITreeWYStart(wptr) + yoff;

      EZ_DisplayTreeInDrawable(wptr, pixmap, x,y, rt, bw,bw, w-(bw<<1), h-(bw<<1));
    }
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  EZ_DrawRectBorder(wptr, pixmap);     
  XCopyArea(EZ_Display,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_Display, pixmap); 
  
  update_itree_scrollbars(wptr);
  if(EZ_ITreeWDisplayed(wptr) == 0)
    {
      EZ_TreeNode *sel = EZ_ITreeWSelection(wptr);
      EZ_ITreeWDisplayed(wptr) = 1;
      if(sel != NULL)
	{
	  tree_scroll_update_selection(wptr, sel);
	  EZ_HandleMotionCallBacks(wptr,EZ_ITreeWMotionCallBack(wptr));
	}
    }
}

/* */
static void update_itree_scrollbars(wptr)
     EZ_Widget *wptr;
{
  if(EZ_LookupWidgetFromAllHT(EZ_ITreeWHScroll(wptr)) != (EZ_Widget *)NULL)
    {
      EZ_UpdateScrollbarI(EZ_ITreeWHScroll(wptr),
			  EZ_ITreeWXSize(wptr),
			  EZ_ITreeWXPageSize(wptr),
			  EZ_ITreeWXStart(wptr));
    }
  else
    {
      EZ_Widget *p = EZ_ITreeWTree(wptr);
      EZ_ITreeWHScroll(wptr) = NULL;
      if(p) EZ_TreeWHScroll(p) = NULL;
    }
  
  if(EZ_LookupWidgetFromAllHT(EZ_ITreeWVScroll(wptr)) != (EZ_Widget *)NULL)
    {
      EZ_UpdateScrollbarI(EZ_ITreeWVScroll(wptr),
			  EZ_ITreeWYSize(wptr),
			  EZ_ITreeWYPageSize(wptr),
			  -EZ_ITreeWYStart(wptr));
    }
  else
    {
      EZ_Widget *p = EZ_ITreeWTree(wptr);
      EZ_ITreeWVScroll(wptr) = NULL;
      if(p) EZ_TreeWVScroll(p) = NULL;
    }

}

/****************************************************************************/
void  EZ_ITreeEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent    *event;
{
  XEvent    xevent;
  int       x, y, bw, padb, Rx,Ry,Rw,Rh, xoffset, yoffset, done;
  int       Ox, Oy, oldx,oldy, moved;
  int       ww,hh,iww, ihh;
  EZ_TreeNode   *troot, *node = NULL;
  EZ_Item       *nsitem = NULL;
  int       hmode, rubberband;

  if(event->type == Expose && event->xexpose.count == 0)   EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) )  return;

  switch(event->type)
    {
    case ButtonPress:
      rubberband = EZ_ITreeWRubberBand(widget);
      hmode = EZ_ITreeWHighlightMode(widget);
      troot =  EZ_ITreeWRoot(widget);
      if(event->xbutton.button == EZ_Btn1) 	  /* get keyboard focus */
	{ 
	  Window fw; int rt;
	  EZ_Widget *tmp;
	  Time  time_stamp;
	  int   elapsedtime;

	  XGetInputFocus(EZ_Display, &fw, &rt);
	  tmp = EZ_LookupWidgetFromMappedHT(fw);
	  if(tmp != widget) EZ_SetFocusTo(widget);

	  if( troot == (EZ_TreeNode *)NULL) break;
	  
	  time_stamp = event->xbutton.time;   
	  elapsedtime = EZ_LastButton1ClickTime - time_stamp; 
	  EZ_LastButton1ClickTime = time_stamp;
	  padb = EZ_WidgetPadB(widget);
	  bw = EZ_WidgetBorderWidth(widget) + padb;
	  Rx = Ry = bw;
	  ww = (int)(EZ_WidgetWidth(widget));
	  Rw =  ww - (bw<<1);
	  hh = (int)(EZ_WidgetHeight(widget)); 
	  Rh =  hh - (bw<<1);
	  x = event->xbutton.x;
	  y = event->xbutton.y;

	  if( (EZ_TreeNodeType(troot) == EZ_TAGED_TREE_NODE) &&
	      (node = EZ_PointerOnBranchNode(x,y,troot)) != NULL)
	    {
	      int hidden = EZ_TreeNodeHideSubtree(node);
	      unsigned long bg = EZ_ITreeWHighlightBG(widget);
	      unsigned long fg = EZ_ITreeWHighlightFG(widget);
	      EZ_TreeNode   *tnode = EZ_ITreeWSelection(widget);
	      EZ_Item *item;
	      
	      EZ_ClearITreeHighlightedItems(widget,Rx, Ry, Rw, Rh);
	      EZ_ITreeWSelection(widget) = node; 

	      if(hidden) EZ_TreeShowSubtree(node);
              else EZ_TreeHideSubtree(node);
              
	      item = EZ_TreeNodeItem(node);
	      if(item && EZ_ItemHighlighted(item) ==0)
		{
		  EZ_ItemHighlighted(item) = 1;
		  EZ_HighlightOneItem(widget,EZ_WidgetWindow(widget),item,fg,bg, Rx, Ry, Rw, Rh,hmode);
		}
	      /* motion callback */
	      if(node != tnode) EZ_HandleMotionCallBacks(widget,EZ_ITreeWMotionCallBack(widget));
	      EZ_ITreeWDirty(widget) = 1;
	      EZ_DrawWidget(widget); 
	      return;
	    }

	  node = EZ_FindPointerOnTreeNode(x,y, troot);

	  if(node)
	    {
	      unsigned long bg = EZ_ITreeWHighlightBG(widget);
	      unsigned long fg = EZ_ITreeWHighlightFG(widget);
	      EZ_TreeNode   *tnode = EZ_ITreeWSelection(widget);
	      EZ_Item *item;
	      
	      EZ_ClearITreeHighlightedItems(widget,Rx, Ry, Rw, Rh);
	      EZ_ITreeWSelection(widget) = node; 
	      item = EZ_TreeNodeItem(node);
	      if(item)
		{
		  EZ_ItemHighlighted(item) = 1;
		  EZ_HighlightOneItem(widget,EZ_WidgetWindow(widget),item,fg,bg, Rx, Ry, Rw, Rh,hmode);
		}
	      /* motion callback */
	      if(node != tnode) EZ_HandleMotionCallBacks(widget,EZ_ITreeWMotionCallBack(widget));
	    }
	  /*
	   * a double click cause the execuation of the call back function
	   */
	  if(node && ABSV(elapsedtime) < DOUBLE_CLICK_TIME_INTERVAL)
	    { 
	      EZ_TreeNode *snode = EZ_ITreeWSelection(widget);
	      if(snode)
		{
		  EZ_Item *sitem = EZ_TreeNodeItem(snode);
		  if(sitem)
		    {
		      /* EZ_ItemPrivateData(sitem) = snode;*/
		      EZ_HandleItemCallBacks(sitem);
		    }
		}
	      EZ_ExecuteWidgetCallBacks(widget);
	      return;
	    }

	  if(rubberband == 0)  /* this is the default */
	    {
	      /* pick the item under the pointer */
	      if(node)
		{
		  nsitem = EZ_TreeNodeItem(node); /* must not be NULL */
	      
		  xoffset = bw + EZ_WidgetPadX(widget) ;
		  yoffset = bw + EZ_WidgetPadY(widget);

		  /* */ Ox = Oy = oldx = oldy = 0; iww = ihh = 1; 
		  {
		    EZ_TreeNode *SaveNode = NULL;
		
		    done = 0;
		    moved = 0;
		    while(!done)
		      {
			do {
			  XNextEvent(EZ_Display, &xevent); 	      
			  if(EZ_FilterEvent(&xevent))   EZ_InvokePrivateEventHandler(&xevent);
			  if(xevent.type == MotionNotify)
			    {
			      moved = 1; 
			      x = xevent.xmotion.x;
			      y = xevent.xmotion.y;
			    }
			  else if(xevent.type == ButtonRelease)
			    {
			      if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
			    }
			  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			    EZ_WidgetDispatchEvent(&xevent);
			} while(XPending(EZ_Display) && !done) ;
		    
			if(!done)
			  {
			    if(moved)
			      {
				EZ_TreeNode *tmp = EZ_FindPointerOnTreeNode(x, y, EZ_ITreeWRoot(widget));
				if(tmp != SaveNode && tmp != EZ_ITreeWSelection(widget))
				  {
				    EZ_Item *item = (SaveNode == NULL? NULL : EZ_TreeNodeItem(SaveNode));
				    EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx, Ry, Rw, Rh);
				    if(tmp)
				      {
					EZ_Item *item = EZ_TreeNodeItem(tmp);
					EZ_FlashItemOutline(widget,item, Rx,Ry,Rw,Rh);
				      }
				    SaveNode = tmp;
				  }
			      }
			  }
			else /* we are done */
			  {
			    if(SaveNode) /* cannot be the current selection */
			      {
				EZ_Item *item;
				EZ_TreeNode *osel = EZ_ITreeWSelection(widget);

				unsigned long bg = EZ_ITreeWHighlightBG(widget);
				unsigned long fg = EZ_ITreeWHighlightFG(widget);

				EZ_ClearITreeHighlightedItems(widget,Rx, Ry, Rw, Rh);
				EZ_ITreeWSelection(widget) = SaveNode; 
				item = EZ_TreeNodeItem(SaveNode);			      			    
				EZ_ItemHighlighted(item) = 1;
				EZ_HighlightOneItem(widget,EZ_WidgetWindow(widget),item,fg,bg, 
						    Rx, Ry, Rw, Rh, hmode);
				
				/* motion callback */
				if(SaveNode != osel)
				  EZ_HandleMotionCallBacks(widget,EZ_ITreeWMotionCallBack(widget));
			      }
			  }
		      } /* while */
		  }
		} /* if node */
	      else /* we are not on a node */
		{
		  xoffset = bw + EZ_WidgetPadX(widget) ;
		  yoffset = bw + EZ_WidgetPadY(widget);

		  done = 0;
		  xevent.type = 0;
		  while(!done)
		    {
		      if(XPending(EZ_Display))
			{
			  while(XPending(EZ_Display))
			    {
			      if(xevent.type == MotionNotify)
				{
				  x = xevent.xmotion.x;
				  y = xevent.xmotion.y;
				}
			      else if(xevent.type == ButtonRelease)
				{
				  if( xevent.xbutton.button == EZ_Btn1) break;
				}
			      else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)  
				EZ_WidgetDispatchEvent(&xevent);
			      XNextEvent(EZ_Display, &xevent);
			      if(EZ_FilterEvent(&xevent)) EZ_InvokePrivateEventHandler(&xevent);
			    }
			}
		      else { EZ_SitForALittleBit(50000); }	      
		      if(xevent.type == ButtonRelease && xevent.xbutton.button == EZ_Btn1) {done = 1; break;}
		      if(  x < xoffset || x > ww - xoffset ||  y < yoffset || y > hh - yoffset)
			{
			  if(y < yoffset)
			    {
			      /* scroll down */
			      if(EZ_ITreeWYStart(widget) < 0)
				{
				  int tmp = EZ_ITreeWYStart(widget) + 10;
				  EZ_ITreeWYStart(widget) = (tmp < 0 ? tmp: 0);
				  EZ_DrawWidget(widget);
				}
			    }
			  else if( y > EZ_WidgetHeight(widget) - yoffset)
			    {
			      /* scroll up */ 
			      EZ_TreeNode *rt = EZ_ITreeWRoot(widget);
			      if(rt)
				{
				  int  th = EZ_ITreeWYSize(widget);
				  if(th > EZ_WidgetHeight(widget))
				    {
				      int dif = EZ_WidgetHeight(widget) - th;
				      int tmp = EZ_ITreeWYStart(widget) - 10;
				      int nOff = (tmp > dif ? tmp: dif);
				      if( EZ_ITreeWYStart(widget) !=  nOff)
					{
					  EZ_ITreeWYStart(widget) =  nOff;
					  EZ_DrawWidget(widget);		
					}		  
				    }
				}
			    }
			  else if(x < xoffset)
			    {
			      /* scroll right */
			      if(EZ_ITreeWXStart(widget) < 0)
				{
				  int tmp = EZ_ITreeWXStart(widget) + 10;
				  EZ_ITreeWXStart(widget) = (tmp < 0 ? tmp: 0);
				  EZ_DrawWidget(widget);
				}
			    }
			  else if( x > EZ_WidgetWidth(widget) - xoffset)
			    {
			      /* scroll left */ 
			      EZ_TreeNode *rt = EZ_ITreeWRoot(widget);
			      if(rt)
				{
				  int  tw = EZ_ITreeWXSize(widget);
				  if(tw > EZ_WidgetWidth(widget))
				    {
				      int dif = EZ_WidgetWidth(widget) - tw;
				      int tmp = EZ_ITreeWXStart(widget) - 10;
				      int nOff =  (tmp > dif ? tmp: dif);
				      if(EZ_ITreeWXStart(widget) != nOff)
					{
					  EZ_ITreeWXStart(widget) = nOff;
					  EZ_DrawWidget(widget); 
					}		  
				    }
				}
			    }
			}
		    } /* while */
		}
	    } 
	  else /* rubberband enabled */
	    {
	      /* pick the item under the pointer */
	      if(node)
		{
		  nsitem = EZ_TreeNodeItem(node); /* must not be NULL */
	      
		  xoffset = bw + EZ_WidgetPadX(widget) ;
		  yoffset = bw + EZ_WidgetPadY(widget);

		  /* */ Ox = Oy = oldx = oldy = 0; iww = ihh = 1; 
		  if(rubberband > 1)
		    {
		      done = 0;
		      moved = 0;
		      while(!done)
			{
			  do {
			    XNextEvent(EZ_Display, &xevent); 	      
			    if(EZ_FilterEvent(&xevent))
			      EZ_InvokePrivateEventHandler(&xevent);
			    if(xevent.type == MotionNotify)
			      {
				moved = 1; 
				x = xevent.xmotion.x;
				y = xevent.xmotion.y;
			      }
			    else if(xevent.type == ButtonRelease)
			      {
				if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
			      }
			    else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			      EZ_WidgetDispatchEvent(&xevent);
			  } while(XPending(EZ_Display) && !done) ;
			
			  if(!done)
			    {
			      if(moved)
				{
				  EZ_TreeNode *tmp = EZ_FindPointerOnTreeNode(x, y, EZ_ITreeWRoot(widget));
				  if(tmp)
				    {
				      EZ_Item *item = EZ_TreeNodeItem(tmp);
				      if(item && EZ_ItemHighlighted(item) == 0)
					{
					  unsigned long bg = EZ_ITreeWHighlightBG(widget);
					  unsigned long fg = EZ_ITreeWHighlightFG(widget);
					  EZ_ItemHighlighted(item) = 1;
					  EZ_HighlightOneItem(widget,EZ_WidgetWindow(widget),item,fg,bg, 
							      Rx, Ry, Rw, Rh, hmode);
					}
				    }
				}
			    }
			} /* while */
		    }
		  else
		    {
		      EZ_TreeNode *SaveNode = NULL;
		
		      done = 0;
		      moved = 0;
		      while(!done)
			{
			  do {
			    XNextEvent(EZ_Display, &xevent); 	      
			    if(EZ_FilterEvent(&xevent))
			      EZ_InvokePrivateEventHandler(&xevent);
			    if(xevent.type == MotionNotify)
			      {
				moved = 1; 
				x = xevent.xmotion.x;
				y = xevent.xmotion.y;
			      }
			    else if(xevent.type == ButtonRelease)
			      {
				if(xevent.xbutton.button == EZ_Btn1) { done = 1; break;}
			      }
			    else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			      EZ_WidgetDispatchEvent(&xevent);
			  } while(XPending(EZ_Display) && !done) ;
		    
			  if(!done)
			    {
			      if(moved)
				{
				  EZ_TreeNode *tmp = EZ_FindPointerOnTreeNode(x, y, EZ_ITreeWRoot(widget));
				  if(tmp != SaveNode && tmp != EZ_ITreeWSelection(widget))
				    {
				      EZ_Item *item = (SaveNode == NULL? NULL : EZ_TreeNodeItem(SaveNode));
				      EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx, Ry, Rw, Rh);
				      if(tmp)
					{
					  EZ_Item *item = EZ_TreeNodeItem(tmp);
					  EZ_FlashItemOutline(widget,item, Rx,Ry,Rw,Rh);
					}
				      SaveNode = tmp;
				    }
				}
			    }
			  else /* we are done */
			    {
			      if(SaveNode) /* cannot be the current selection */
				{
				  EZ_Item *item;
				  EZ_TreeNode *osel = EZ_ITreeWSelection(widget);

				  unsigned long bg = EZ_ITreeWHighlightBG(widget);
				  unsigned long fg = EZ_ITreeWHighlightFG(widget);

				  EZ_ClearITreeHighlightedItems(widget,Rx, Ry, Rw, Rh);
				  EZ_ITreeWSelection(widget) = SaveNode; 
				  item = EZ_TreeNodeItem(SaveNode);			      			    
				  EZ_ItemHighlighted(item) = 1;
				  EZ_HighlightOneItem(widget,EZ_WidgetWindow(widget),item,fg,bg, 
						      Rx, Ry, Rw, Rh, hmode);
				
				  /* motion callback */
				  if(SaveNode != osel)
				    EZ_HandleMotionCallBacks(widget,EZ_ITreeWMotionCallBack(widget));
				}
			    }
			} /* while */
		    }
		} /* if node */
	      else /* click on background, do rubberband */
		{
		  int x0, y0, x1, y1, rx, ry, rw,rh, dflag = 0, moved = 0;
		  Window win = EZ_WidgetWindow(widget);
		  GC gc = EZ_XORGC;
		  x0 = x;      y0 = y;
		  rx = ry = rw = rh = 0;
		  
		  while(1)
		    {
		      XNextEvent(EZ_Display, &xevent);
		      if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
		      if(xevent.type == MotionNotify)
			{
			  if(moved == 0)
			    {
			      moved = 1;
			      EZ_ClearITreeHighlightedItems(widget,Rx, Ry, Rw, Rh);
			      EZ_ITreeWSelection(widget) = NULL;
			    }
			  if(dflag) 
			    {
			      XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
			      dflag = 0;
			    }
			  while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
			  x1 = xevent.xmotion.x;
			  y1 = xevent.xmotion.y;
			  rw = x1 - x0; if(rw<0) rw = -rw;
			  rh = y1 - y0; if(rh<0) rh = -rh;
			  rx = x0 < x1 ? x0: x1;
			  ry = y0 < y1 ? y0: y1;
			  if(moved) EZ_ITreeHighlightItemsInRectangle(widget, rx,ry,rw,rh);
			  XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
			  dflag = 1;
			}
		      else if(xevent.type == ButtonRelease &&  xevent.xbutton.button == EZ_Btn1)
			{
			  if(dflag) 
			    XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);		      
			  break;
			}
		      else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
			EZ_WidgetDispatchEvent(&xevent);
		    }
		  if(moved) /* select a node */
		    {
		      EZ_TreeNode *root = EZ_ITreeWRoot(widget);
		      EZ_TreeNode *nsl = itreeFindHighlightedNode(root);
		      EZ_ITreeWSelection(widget) = nsl;
		      if(nsl) EZ_HandleMotionCallBacks(widget,EZ_ITreeWMotionCallBack(widget));
		    }
		}
	    }
	}
      else if(event->xbutton.button == EZ_Btn3)   /* DnD */
	{
	  Window  ptWin = event->xbutton.window;
	  Window widgetwin = ptWin;
	  unsigned long bg = EZ_ITreeWHighlightBG(widget);
	  unsigned long fg = EZ_ITreeWHighlightFG(widget);

	  
	  padb = EZ_WidgetPadB(widget);
	  bw = EZ_WidgetBorderWidth(widget) + padb;
	  Rx = Ry = bw;
	  ww = (int)(EZ_WidgetWidth(widget));
	  Rw =  ww - (bw<<1);
	  hh = (int)(EZ_WidgetHeight(widget)); 
	  Rh =  hh - (bw<<1);
  
	  x = event->xbutton.x;
	  y = event->xbutton.y;

	  node = EZ_FindPointerOnTreeNode(x,y, EZ_ITreeWRoot(widget));

	  if(node)
	    {
	      if(rubberband == 0)
		{
		  if(node != EZ_ITreeWSelection(widget))
		    {
		      EZ_TreeNode   *tnode = EZ_ITreeWSelection(widget);
		      EZ_Item *item = (tnode == NULL? NULL : EZ_TreeNodeItem(tnode));
		      if(item)
			{
			  EZ_ItemHighlighted(item) = 0;
			  EZ_UpdateOneItem(widget,widgetwin,item, Rx, Ry, Rw, Rh);
			}
		      EZ_ITreeWSelection(widget) = node; 
		      item = EZ_TreeNodeItem(node);
		      if(item)
			{
			  EZ_ItemHighlighted(item) = 1;
			  EZ_HighlightOneItem(widget,widgetwin,item,fg,bg, Rx, Ry, Rw, Rh,hmode);
			}
	      
		      /* motion callback */
		      EZ_HandleMotionCallBacks(widget,EZ_ITreeWMotionCallBack(widget));
		    }
		  itreeDnDOneNode(widget, node, event, Rx,Ry,Rw,Rh);
		}
	      else /* rubber band */
		{
		  EZ_Item *item = EZ_TreeNodeItem(node);
		  if(item)
		    {
		      if(EZ_ItemHighlighted(item) == 0)  /* a new node */
			{
			  EZ_ClearITreeHighlightedItems(widget,Rx, Ry, Rw, Rh);
			  EZ_ItemHighlighted(item) = 1;
			  EZ_ITreeWSelection(widget) = node; 
			  EZ_HighlightOneItem(widget,widgetwin,item,fg,bg, Rx, Ry, Rw, Rh,hmode);
			  itreeDnDOneNode(widget, node, event, Rx,Ry,Rw,Rh);
			}
		      else
			{
			  int nhnodes = itreeGetNumHighlightedNodes(widget);
			  if(nhnodes == 1) itreeDnDOneNode(widget, node, event, Rx,Ry,Rw,Rh);
			  else itreeDnDNNodes(widget, nhnodes, event, Rx,Ry,Rw,Rh);
			}
		    }
		}
	    }
	  else if(rubberband)
	    {
	      int x0, y0, x1, y1, rx, ry, rw,rh, dflag = 0, moved = 0;
	      Window win = EZ_WidgetWindow(widget);
	      GC gc = EZ_XORGC;
	      x0 = x;      y0 = y;
	      rx = ry = rw = rh = 0;
		  
	      while(1)
		{
		  XNextEvent(EZ_Display, &xevent);
		  if(EZ_FilterEvent(&xevent))  EZ_InvokePrivateEventHandler(&xevent);
		  if(xevent.type == MotionNotify)
		    {
		      if(moved == 0)
			{
			  moved = 1;
			  EZ_ClearITreeHighlightedItems(widget,Rx, Ry, Rw, Rh);
			  EZ_ITreeWSelection(widget) = NULL;
			}
		      if(dflag) 
			{
			  XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
			  dflag = 0;
			}
		      while (XCheckTypedEvent(EZ_Display, MotionNotify, &xevent));
		      x1 = xevent.xmotion.x;
		      y1 = xevent.xmotion.y;
		      rw = x1 - x0; if(rw<0) rw = -rw;
		      rh = y1 - y0; if(rh<0) rh = -rh;
		      rx = x0 < x1 ? x0: x1;
		      ry = y0 < y1 ? y0: y1;
		      if(moved) EZ_ITreeHighlightItemsInRectangle(widget, rx,ry,rw,rh);
		      XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);
		      dflag = 1;
		    }
		  else if(xevent.type == ButtonRelease &&  xevent.xbutton.button == EZ_Btn3)
		    {
		      if(dflag) 
			XDrawRectangle(EZ_Display, win, gc, rx,ry,rw, rh);		      
		      break;
		    }
		  else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut)
		    EZ_WidgetDispatchEvent(&xevent);
		}
	      if(moved) /* select a node */
		{
		  EZ_TreeNode *root = EZ_ITreeWRoot(widget);
		  EZ_TreeNode *nsl = itreeFindHighlightedNode(root);
		  EZ_ITreeWSelection(widget) = nsl;
		  if(nsl) EZ_HandleMotionCallBacks(widget,EZ_ITreeWMotionCallBack(widget));
		}
	    }
	} /* button */
      break;
      break;
    case MotionNotify:
      break;
    case EnterNotify:
      break;
    case LeaveNotify:
      break;
    case KeyPress:
      if(EZ_ITreeWRoot(widget) == (EZ_TreeNode *)NULL) break;
      if(EZ_ITreeWSelection(widget) == (EZ_TreeNode *)NULL)
	EZ_ITreeWSelection(widget) = EZ_ITreeWRoot(widget);
      {
#define TEMP_BUFFER_SIZE    32
	int               count, move_unit = 1;
	KeySym            keysym;
	XComposeStatus    compose; 
	char              tmpbuffer[TEMP_BUFFER_SIZE];
	int               buffersize = TEMP_BUFFER_SIZE;
#undef TEMP_BUFFER_SIZE
	if(event->xkey.state & ShiftMask) move_unit = 2;
	if(event->xkey.state & ControlMask)     move_unit = 4;
	
	count = XLookupString(&(event->xkey), tmpbuffer, buffersize, &keysym, &compose);
	tmpbuffer[count] = '\0'; 
	
	switch(keysym)
	  {
	  case XK_Up: case XK_k:  case XK_p: case XK_K:  case XK_P:  
#ifdef XK_KP_Up
	  case XK_KP_Up:
#endif
	    {
	      EZ_TreeNode *node = EZ_ITreeWSelection(widget);
	      for(count = 0; count < move_unit; count++)
		{
		  EZ_TreeNode *tnode = EZ_FindTreeAdjBranch(EZ_ITreeWRoot(widget), node, -1);
		  if(tnode == (EZ_TreeNode *)NULL) break;
		  else node = tnode;
		}
	      if(node && node != EZ_ITreeWSelection(widget))
		{
		  tree_scroll_update_selection(widget, node);
		  EZ_HandleMotionCallBacks(widget,EZ_ITreeWMotionCallBack(widget));
		}
	    }
	    break;
	  case XK_Down:  case XK_n: case XK_j: case XK_N: case XK_J: 
#ifdef XK_KP_Down
	  case XK_KP_Down:
#endif
	    {
	      EZ_TreeNode *node = EZ_ITreeWSelection(widget);
	      for(count = 0; count < move_unit; count++)
		{
		  EZ_TreeNode *tnode = EZ_FindTreeAdjBranch(EZ_ITreeWRoot(widget), node, 1);
		  if(tnode == (EZ_TreeNode *)NULL) break;
		  else node = tnode;
		}
	      if(node && node != EZ_ITreeWSelection(widget))
		{
		  tree_scroll_update_selection(widget, node);
		  EZ_HandleMotionCallBacks(widget,EZ_ITreeWMotionCallBack(widget));
		}
	    }
	    break;
	  case XK_Left:  case XK_b:  case XK_h: case XK_B:  case XK_H: 
#ifdef XK_KP_Left
	  case XK_KP_Left:
#endif

	    {
	      EZ_TreeNode *rt = EZ_ITreeWRoot(widget);
	      int tw = EZ_TreeNodeTWidth(rt);
	      int xoffset = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadX(widget);
	      int w = EZ_WidgetWidth(widget) - (xoffset <<1);
	      if(w < tw)
		{
		  int aaa = (move_unit << 3);
		  int itmp = EZ_ITreeWXStart(widget) + aaa;
		  if(itmp >= 0) itmp = 0;
		  if(EZ_ITreeWXStart(widget) != itmp)
		    {
		      EZ_ITreeWXStart(widget) = itmp;
		      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
		    }
		}
	    }
	  break;
	  case XK_Right:  case XK_f: case XK_l: case XK_F: case XK_L: 
#ifdef XK_KP_Right
	  case XK_KP_Right:
#endif
	    {
	      EZ_TreeNode *rt = EZ_ITreeWRoot(widget);
	      int tw = EZ_TreeNodeTWidth(rt);
	      int xoffset = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadX(widget);
	      int w = EZ_WidgetWidth(widget) - (xoffset <<1);
	      if( w < tw)
		{
		  int aaa = -(move_unit << 3);
		  int itmp = EZ_ITreeWXStart(widget) + aaa;
		  if(itmp < w - tw) itmp = w -tw;
		  
		  if(EZ_ITreeWXStart(widget) != itmp)
		    {
		      EZ_ITreeWXStart(widget) = itmp;
		      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
		    }
		}
	    }
	    break;
	  case XK_Return: case XK_Linefeed: case XK_space:  
#ifdef XK_KP_Enter
	  case XK_KP_Enter:
#endif
	    { 
	      EZ_TreeNode *snode = EZ_ITreeWSelection(widget);
	      if(snode)
		{
		  EZ_Item *item = EZ_TreeNodeItem(snode);
		  if(item) 
		    {
		      /* EZ_ItemPrivateData(item) = snode;*/
		      EZ_HandleItemCallBacks(item);
		    }
		}
	      EZ_ExecuteWidgetCallBacks(widget);
	    }
	    break;
	  default:
	    break;
	  }
	{ 
	  XEvent tmpevent; 
	  while(XCheckTypedEvent(EZ_Display, KeyPress, &tmpevent))
	    if(EZ_FilterEvent(&tmpevent))
	      EZ_InvokePrivateEventHandler(&tmpevent);
	}
      }      
      break;
    default:
      break;
    }
}
/*****************************************************************************
 * 
 * given (x0,y0), figure out which node the point is in. and return the node.
 */
EZ_TreeNode *EZ_FindPointerOnTreeNode(x0,y0, root)
     int x0,y0;
     EZ_TreeNode *root;
{
  EZ_TreeNode *children;
  EZ_Item *item;
  int  x, y, w, h, tw, th;
  
  if(root == NULL) { return(NULL); }
  item = EZ_TreeNodeItem(root);
  if(item)
    {
      x = EZ_ItemOriginX(item);
      y = EZ_ItemOriginY(item);
      w = EZ_ItemAWidth(item);
      h = EZ_ItemAHeight(item);
      if( x0 >= x && x0 <= x + w && y0 >= y && y0 <= y + h)
	{ return(root); }
	
      if(EZ_TreeNodeType(root) == EZ_TREE_NODE)
	x -= ((int)(EZ_TreeNodeTWidth(root) - EZ_ItemWidth(item)) >> 1);

      tw = EZ_TreeNodeTWidth(root);
      th = EZ_TreeNodeTHeight(root);
      if(x0 >= x && y0 >= y && x0 < x + tw && y0 < y +th)
	{
	  EZ_TreeNode *tmp;
	  children = EZ_TreeNodeChildren(root);
	  while(children && EZ_TreeNodeHideSubtree(root) == 0)
	    {
	      tmp = EZ_FindPointerOnTreeNode(x0,y0, children);
	      if(tmp) { return(tmp); }
	      children = EZ_TreeNodeSibling(children);
	    }
	}
    }
  return(NULL);
}
  
/****************************************************************
 *
 * Find the adj branch to item on the display.
 */
EZ_TreeNode *EZ_FindTreeAdjBranch(root, node, direction)
     EZ_TreeNode *root, *node;
     int direction;
{
  if(root == (EZ_TreeNode *)NULL || node == (EZ_TreeNode *)NULL)
    return((EZ_TreeNode *)NULL);

  if(direction == -1) /* up */
    {
      if(node == root) return(root);
      {
	EZ_TreeNode *parent = EZ_TreeNodeParent(node);
	EZ_TreeNode *children = EZ_TreeNodeChildren(parent);
	if(children == node) return(parent);
	else
	  {
	    while(children && EZ_TreeNodeSibling(children) != node)
	      children =  EZ_TreeNodeSibling(children);
	    while(1)
	      {
		if(children) 
		  {
		    EZ_TreeNode *tmp = EZ_TreeNodeChildren(children);
		    if(tmp == (EZ_TreeNode *)NULL) return(children);
		    else 
		      {
			while(tmp && EZ_TreeNodeSibling(tmp))
			  tmp =  EZ_TreeNodeSibling(tmp);
		      }
		    children = tmp;
		  }
		else break;
	      }
	    return(children);
	  }
      }
    }
  else /* down */
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(node);
      if(children) return(children);
      else
	{
	  EZ_TreeNode *sibling = EZ_TreeNodeSibling(node);
	  if(sibling) return(sibling);
	  else
	    {
	      EZ_TreeNode *parent = EZ_TreeNodeParent(node);
	      while(parent && parent != root)
		{
		  if(EZ_TreeNodeSibling(parent))
		    {
		      return(EZ_TreeNodeSibling(parent));
		      break;
		    }
		  else parent = EZ_TreeNodeParent(parent);
		}
	    }
	}
    }
  return(NULL);
}

void tree_scroll_update_selection(widget, node)
     EZ_Widget *widget;
     EZ_TreeNode   *node;
{
  EZ_Item *item  =  EZ_TreeNodeItem(node);
  
  if( item )
    {
      int x0 = EZ_ItemOriginX(item);
      int y0 = EZ_ItemOriginY(item);
      int iw = EZ_ItemWidth(item);
      int ih = EZ_ItemHeight(item);
      int padb = EZ_WidgetPadB(widget);
      int bw = EZ_WidgetBorderWidth(widget) + padb;
      int yoffset = bw + EZ_WidgetPadY(widget);
      int xoffset = bw + EZ_WidgetPadX(widget);
      int Rw = EZ_WidgetWidth(widget);
      int Rh = EZ_WidgetHeight(widget);
      int w = Rw - (xoffset <<1);
      int h = Rh - (yoffset <<1);
      int redraw = 0;

      if( y0 < 0 || x0 < 0 || x0 + iw > w || y0 + ih >h)
	{ /* part is outside */
	  if(y0 < 0)
	    {
	      int itmp  =  - y0  + EZ_ITreeWYStart(widget);
	      EZ_ITreeWYStart(widget) = (itmp < 0 ? itmp: 0);
	      if(x0 < 0)
		{
		  int itmp  = - x0  + EZ_ITreeWXStart(widget);
		  EZ_ITreeWXStart(widget) = (itmp < 0 ? itmp: 0);
		}
	      else if(x0 + iw > w)
		{
		  EZ_TreeNode *rt = EZ_ITreeWRoot(widget);
		  int  tw = EZ_TreeNodeTWidth(rt) + (xoffset <<1);
		  if(tw > EZ_WidgetWidth(widget))
		    {
		      int itmp = EZ_ITreeWXStart(widget) - iw + w - x0;
		      int dif = EZ_WidgetWidth(widget) - tw;
		      EZ_ITreeWXStart(widget) = (itmp > dif ? itmp:dif);
		    }
		}
	    }
	  else if(y0 + ih >h)
	    {
	      EZ_TreeNode *rt = EZ_ITreeWRoot(widget);
	      int  th = EZ_TreeNodeTHeight(rt) + (yoffset <<1);
	      if(th > EZ_WidgetHeight(widget))
		{
		  int itmp =  EZ_ITreeWYStart(widget) - ih + h - y0; 
		  int dif = EZ_WidgetHeight(widget) - th;
		  EZ_ITreeWYStart(widget) = (itmp > dif ? itmp: dif);
		}
	      if(x0 < 0)
		{
		  int itmp  = - x0  + EZ_ITreeWXStart(widget);
		  EZ_ITreeWXStart(widget) = (itmp < 0 ? itmp: 0);
		}
	      else if(x0 + iw > w)
		{
		  EZ_TreeNode *rt = EZ_ITreeWRoot(widget);
		  int  tw = EZ_TreeNodeTWidth(rt) + (xoffset <<1);
		  if(tw > EZ_WidgetWidth(widget))
		    {
		      int itmp = EZ_ITreeWXStart(widget) - iw + w - x0;
		      int dif = EZ_WidgetWidth(widget) - tw;
		      EZ_ITreeWXStart(widget) = (itmp > dif ? itmp:dif);
		    }
		}
	    }
	  else if(x0 < 0)
	    {
	      int itmp  = - x0  + EZ_ITreeWXStart(widget);
	      EZ_ITreeWXStart(widget) = (itmp < 0 ? itmp: 0);
	    }
	  else if(x0 + iw > w)
	    {
	      EZ_TreeNode *rt = EZ_ITreeWRoot(widget);
	      int  tw = EZ_TreeNodeTWidth(rt) + (xoffset <<1);
	      if(tw > EZ_WidgetWidth(widget))
		{
		  int itmp = EZ_ITreeWXStart(widget) - iw + w - x0;
		  int dif = EZ_WidgetWidth(widget) - tw;
		  EZ_ITreeWXStart(widget) = (itmp > dif ? itmp:dif);
		}
	    }
	  redraw = 1;
	}

      /* update selection */
	{
	  EZ_TreeNode *snode = EZ_ITreeWSelection(widget);
          unsigned long bg = EZ_ITreeWHighlightBG(widget);
          unsigned long fg = EZ_ITreeWHighlightFG(widget);
          int  hmode = EZ_ITreeWHighlightMode(widget);
          if(snode)
            {
              EZ_Item *item = EZ_TreeNodeItem(snode);
              if(item)
                {
                  EZ_ItemHighlighted(item) = 0;
                  EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item,
                                   bw,bw, Rw -(bw <<1), Rh - (bw << 1));		       
                }
            }
	  EZ_ITreeWSelection(widget) = node;	
	  item = EZ_TreeNodeItem(node);
	  EZ_ItemHighlighted(item) = 1;
	  EZ_HighlightOneItem(widget,EZ_WidgetWindow(widget),item,fg,bg,
			      bw,bw, Rw -(bw <<1), Rh - (bw << 1),hmode);
	}
      if(redraw) EZ_DrawWidget(widget);
    }
}

/***************************************************************************/
static void ITreeWTimerCallBack(timer, cdata)
     EZ_Timer *timer; 
     void *cdata;
{
  EZ_Widget *widget = (EZ_Widget *)cdata;
  if(widget && EZ_LookupWidgetFromAllHT(widget) == widget &&
     EZ_WidgetType(widget) == EZ_WIDGET_ITREE)
    {
      int hmode = (int)EZ_ITreeWHighlightMode(widget) & 1;
      if(hmode == 0 && EZ_WidgetWindow(widget) && EZ_WidgetMapped(widget))
	{
	  EZ_TreeNode *rt = EZ_ITreeWRoot(widget);
	  EZ_TreeNode *sel = EZ_ITreeWSelection(widget);
	  if(rt && sel)
	    {
	      EZ_Item *item = EZ_TreeNodeItem(sel);
	      if(item)
		{
		  int padb = EZ_WidgetPadB(widget);
		  int Rx = EZ_WidgetBorderWidth(widget) + padb;
		  int Ry = Rx;
		  int Rw = EZ_WidgetWidth(widget) - (Rx+Rx);
		  int Rh = EZ_WidgetHeight(widget) -(Rx+Rx) ;

		  EZ_FlashItemOutline(widget,item, Rx,Ry,Rw,Rh);
		}
	    }
	}
    }
  else EZ_CancelTimer(timer);
}

/*********************************************************************/
void  EZ_FlashItemOutline(widget,item, Rx,Ry,Rw,Rh)
     EZ_Widget *widget;
     EZ_Item *item;
     int Rx,Ry,Rw,Rh;
{
  if(item != NULL)
    {
      int y0 = (int)EZ_ItemOriginY(item);
      int x0 = (int)EZ_ItemOriginX(item);  
      int w = (int)EZ_ItemAWidth(item)-1;
      int h = (int)EZ_ItemAHeight(item)-1;  
      
      if(y0 >= Ry && y0 + h <= Ry+Rh && x0 >= Rx && x0 +w <= Rx+Rw)
	{  /* item is completely inside the drawable region */
	  XPoint points[5];
	  Window win = EZ_WidgetWindow(widget);
	  GC gc;
	  EZ_GetMovingDashedLineGC(&gc, 0);
	  points[0].x = x0;     points[0].y = y0;
	  points[1].x = x0+w;   points[1].y = y0;
	  points[2].x = x0+w;   points[2].y = y0+h;
	  points[3].x = x0;     points[3].y = y0+h;
	  points[4].x = x0;     points[4].y = y0;
	  XDrawLines(EZ_Display, win, gc, points, 5, CoordModeOrigin);
	}
      else /* item is partially outside */
	{
	  int x_r,y_r,w_r,h_r;
	  XPoint points[2];
	  Window win = EZ_WidgetWindow(widget);
	  GC gc;
	  EZ_GetMovingDashedLineGC(&gc,0);
	  EZ_IntersectRectangles(Rx,Ry,Rw,Rh, 
				 x0,y0,w,h,
				 &x_r,&y_r, &w_r, &h_r);
	  if(y_r == y0) /* top edge is inside */
	    {
	      points[0].x = x_r;          points[0].y = y_r;
	      points[1].x = x_r + w_r;    points[1].y = y_r;
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);
	    }
	  if(w_r + x_r == x0 +w) /* right edge is inside */
	    {
	      points[0].x = x_r + w_r;          points[0].y = y_r;
	      points[1].x = x_r + w_r;    points[1].y = y_r + h_r;
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);
	    }
	  if(y_r+h_r == h+y0)
	    {
	      points[0].x = x_r + w_r;          points[0].y = y_r + h_r;
	      points[1].x = x_r;    points[1].y = y_r + h_r;
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);
	    }
	  if(x_r == x0)
	    {
	      points[0].x = x_r;          points[0].y = y_r + h_r;
	      points[1].x = x_r;          points[1].y = y_r;
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);
	    }
	}
    }
}
/***************************************************************************/
void EZ_DnDOutlineItem(widget,item, Rx,Ry,Rw,Rh)
     EZ_Widget *widget;
     EZ_Item *item;
     int Rx,Ry,Rw,Rh;
{
  if(item != NULL)
    {
      int y0 = (int)EZ_ItemOriginY(item);
      int x0 = (int)EZ_ItemOriginX(item);  
      int w = (int)EZ_ItemAWidth(item)-1;
      int h = (int)EZ_ItemAHeight(item)-1;  
      
      if(y0 >= Ry && y0 + h <= Ry+Rh && x0 >= Rx && x0 +w <= Rx+Rw)
	{  /* item is completely inside the drawable region */
	  XPoint points[5];
	  Window win = EZ_WidgetWindow(widget);
	  GC gc = EZ_DNDXORGC; 
	  
	  points[0].x = x0;     points[0].y = y0;
	  points[1].x = x0+w;   points[1].y = y0;
	  points[2].x = x0+w;   points[2].y = y0+h;
	  points[3].x = x0;     points[3].y = y0+h;
	  points[4].x = x0;     points[4].y = y0;
	  XDrawLines(EZ_Display, win, gc, points, 5, CoordModeOrigin);
	}
      else /* item is partially outside */
	{
	  int x_r,y_r,w_r,h_r;
	  XPoint points[2];
	  Window win = EZ_WidgetWindow(widget);
	  GC gc = EZ_DNDXORGC; 
	   
	  EZ_IntersectRectangles(Rx,Ry,Rw,Rh, 
				 x0,y0,w,h,
				 &x_r,&y_r, &w_r, &h_r);
	  if(y_r == y0) /* top edge is inside */
	    {
	      points[0].x = x_r;          points[0].y = y_r;
	      points[1].x = x_r + w_r;    points[1].y = y_r;
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);
	    }
	  if(w_r + x_r == x0 +w) /* right edge is inside */
	    {
	      points[0].x = x_r + w_r;          points[0].y = y_r;
	      points[1].x = x_r + w_r;    points[1].y = y_r + h_r;
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);
	    }
	  if(y_r+h_r == h+y0)
	    {
	      points[0].x = x_r + w_r;          points[0].y = y_r + h_r;
	      points[1].x = x_r;    points[1].y = y_r + h_r;
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);
	    }
	  if(x_r == x0)
	    {
	      points[0].x = x_r;          points[0].y = y_r + h_r;
	      points[1].x = x_r;          points[1].y = y_r;
	      XDrawLines(EZ_Display, win, gc, points, 2, CoordModeOrigin);
	    }
	}
    }
}


/***************************************************************************
 *
 *  Supporting functions.
 *
 ***************************************************************************/
void EZ_SetITreeWTree(widget, root)
     EZ_Widget *widget;
     EZ_TreeNode  *root;
{
  if(widget)
    {
      EZ_TreeNode *tmp = EZ_ITreeWRoot(widget);
      if(tmp && tmp != root) EZ_TreeDestroySubtree(tmp);
      EZ_ITreeWRoot(widget) = root;
      EZ_ITreeWSelection(widget) = NULL;
      if(root)
	{
	  EZ_Item *item = EZ_TreeNodeItem(root);
	  EZ_ItemDirty(item) = 1;
	}
      EZ_ITreeWXStart(widget) = 0;
      EZ_ITreeWYStart(widget) = 0;
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
}
/************************************************************************/

void EZ_ITreeSelectNode(widget, theNode, location)
     EZ_Widget *widget;
     EZ_TreeNode *theNode; 
     int *location;
{
  EZ_TreeNode *root = EZ_ITreeWRoot(widget);
  if(root)
    {
      int padb = EZ_WidgetPadB(widget);
      int bw = EZ_WidgetBorderWidth(widget) + padb;
      int Rx, Ry, Rw, Rh, ww, hh;
      EZ_TreeNode *onode = EZ_ITreeWSelection(widget);

      Rx = Ry = bw;
      ww = (int)(EZ_WidgetWidth(widget));
      Rw =  ww - (bw<<1);
      hh = (int)(EZ_WidgetHeight(widget)); 
      Rh =  hh - (bw<<1);
      if(EZ_TreeFindNode(root, theNode) == NULL) theNode = NULL;
      EZ_ClearITreeHighlightedItems(widget,Rx, Ry, Rw, Rh);
      EZ_ITreeWSelection(widget) = theNode;
      if(theNode)
	{
	  EZ_Item *item = EZ_TreeNodeItem(theNode);
	  if(item)
	    {
	      EZ_ItemHighlighted(item) = 1;
	      if(EZ_WidgetMapped(widget))
		{
		  if(location)
		    {
		      EZ_Item *item = EZ_TreeNodeItem(theNode);
		      int x = EZ_ItemOriginX(item);
		      int y = EZ_ItemOriginY(item);
		      EZ_ITreeWXStart(widget) += location[0] - x;
		      EZ_ITreeWYStart(widget) += location[1] - y;
		      EZ_DrawWidget(widget);
		    }
		  else
		    {
		      unsigned long bg = EZ_ITreeWHighlightBG(widget);
		      unsigned long fg = EZ_ITreeWHighlightFG(widget);	      
		      int hmode = (int)EZ_ITreeWHighlightMode(widget);		      
		      EZ_HighlightOneItem(widget,EZ_WidgetWindow(widget),item,fg,bg, 
					  Rx, Ry, Rw, Rh, hmode);
		    }
		  tree_scroll_update_selection(widget, theNode);
		}
	    }
	  if(theNode != onode)
	    EZ_HandleMotionCallBacks(widget,EZ_ITreeWMotionCallBack(widget));
	}
    }
}


void EZ_ITreeSelectNodeUsingItem(widget, theItem, location)
     EZ_Widget *widget;
     EZ_Item *theItem;
     int *location;
{
  EZ_TreeNode *root = EZ_ITreeWRoot(widget);
  if(root)
    {
      EZ_TreeNode *theNode = EZ_TreeFindNodeUsingItem(root, theItem);
      EZ_ITreeSelectNode(widget, theNode, location);
    }
}
/************************************************************************************/
static void EZ_ClearITreeHighlightedItemsWork(widget, node, Rx, Ry, Rw, Rh)
     EZ_Widget *widget; int Rx,Ry,Rw,Rh;
     EZ_TreeNode *node;
{
  if(node)
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(node);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(node);
      EZ_Item  *item = EZ_TreeNodeItem(node);
      if(item && EZ_ItemHighlighted(item))
	{
	  EZ_ItemHighlighted(item) = 0;
	  EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx, Ry, Rw, Rh);
	}
      EZ_ClearITreeHighlightedItemsWork(widget, children, Rx, Ry, Rw, Rh);
      EZ_ClearITreeHighlightedItemsWork(widget, sibling, Rx, Ry, Rw, Rh);      
    }
}
static void EZ_ClearITreeHighlightedItems(widget,Rx, Ry, Rw, Rh)
     EZ_Widget *widget; int Rx,Ry,Rw,Rh;
{
  EZ_TreeNode *root = EZ_ITreeWRoot(widget);  
  EZ_ClearITreeHighlightedItemsWork(widget, root, Rx, Ry, Rw, Rh);
}

static EZ_TreeNode *itreeFindHighlightedNode(node) EZ_TreeNode *node;
{
  if(node)
    {
      EZ_Item  *item = EZ_TreeNodeItem(node);
      if(item && EZ_ItemHighlighted(item))
	{
	  return(node);
	}
      else
	{
	  EZ_TreeNode *children = EZ_TreeNodeChildren(node);
	  EZ_TreeNode *ans = itreeFindHighlightedNode(children);
	  if(ans) return(ans);
	  else
	    {
	      EZ_TreeNode *sibling = EZ_TreeNodeSibling(node);
	      return(itreeFindHighlightedNode(sibling));
	    }
	}      
    }
  return(NULL);
}
/************************************************************************************/
static void EZ_ITreeHighlightItemsInRectangleWork(widget, node,rx,ry,rw,rh, Rx,Ry,Rw,Rh)
     EZ_Widget *widget; int rx, ry, rw, rh, Rx,Ry,Rw,Rh;
     EZ_TreeNode *node;
{
  if(node)
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(node);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(node);
      EZ_Item  *item = EZ_TreeNodeItem(node);
      if(item)
	{
	  int x0 = EZ_ItemOriginX(item);
	  int y0 = EZ_ItemOriginY(item);
	  int w0 = EZ_ItemAWidth(item);
	  int h0 = EZ_ItemAHeight(item);
	  int cx = x0 + (w0>>1);
	  int cy = y0 + (h0>>1);
	  if(cx >= rx && cx <= rx + rw && cy >= ry && cy <= ry + rh)
	    {
	      if(EZ_ItemHighlighted(item) == 0)
		{
		  int hmode = (int)EZ_ITreeWHighlightMode(widget);
		  unsigned long bg = EZ_ITreeWHighlightBG(widget);
		  unsigned long fg = EZ_ITreeWHighlightFG(widget);
		  EZ_ItemHighlighted(item) = 1;
		  EZ_HighlightOneItem(widget, EZ_WidgetWindow(widget), item, fg, bg, Rx,Ry,Rw,Rh, hmode);
		}
	    }
	  else if(EZ_ItemHighlighted(item))
	    {
	      EZ_ItemHighlighted(item) = 0;
	      EZ_UpdateOneItem(widget,EZ_WidgetWindow(widget),item, Rx, Ry, Rw, Rh);
	    }
	}
      EZ_ITreeHighlightItemsInRectangleWork(widget, children,rx,ry,rw,rh, Rx,Ry,Rw,Rh);
      EZ_ITreeHighlightItemsInRectangleWork(widget, sibling,rx,ry,rw,rh, Rx,Ry,Rw,Rh);     
    }
}
     
static void EZ_ITreeHighlightItemsInRectangle(widget, rx,ry,rw,rh)
     EZ_Widget *widget; int rx, ry, rw, rh;
{
  int padb, bw, Rx,Ry,Rh,Rw,hh,ww;
  EZ_TreeNode *root = EZ_ITreeWRoot(widget);      

  padb = EZ_WidgetPadB(widget);
  bw = EZ_WidgetBorderWidth(widget) + padb;
  Rx = Ry = bw;
  ww = (int)(EZ_WidgetWidth(widget));
  Rw =  ww - (bw<<1);
  hh = (int)(EZ_WidgetHeight(widget)); 
  Rh =  hh - (bw<<1);
  EZ_ITreeHighlightItemsInRectangleWork(widget, root,rx,ry,rw,rh, Rx,Ry,Rw,Rh);
}
/************************************************************************************/
static void getHLNodes(node, cnt) EZ_TreeNode *node; int *cnt;
{
  if(node)
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(node);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(node);
      EZ_Item  *item = EZ_TreeNodeItem(node);
      if(item && EZ_ItemHighlighted(item)) *cnt += 1;
      getHLNodes(children, cnt);
      getHLNodes(sibling, cnt);
    }
}
static int itreeGetNumHighlightedNodes(widget) EZ_Widget *widget;
{
  int total = 0;
  EZ_TreeNode *root = EZ_ITreeWRoot(widget);
  getHLNodes(root, &total);
  return(total);
}
/************************************************************************************/
static void itreeDnDOneNode(widget, node, event, Rx, Ry,Rw,Rh)
     EZ_Widget *widget; EZ_TreeNode *node; int Rx, Ry, Rw,Rh; XEvent *event;
{
  int dndType = 0;
  void  *dndObj = NULL;
  EZ_Item *nsitem;
  /* pick the item under the pointer */
  if(node && (nsitem = EZ_TreeNodeItem(node)))
    {
      int rubberband = EZ_ITreeWRubberBand(widget);
      
      if(EZ_ItemIsDnDSrc(nsitem))
	{
	  dndType = EZ_DND_OBJ_IS_ITEM;
	  dndObj = nsitem;
	}
      else if(rubberband && EZ_WidgetIsDnDSrc(widget))
	{
	  dndType = EZ_DND_OBJ_IS_WIDGET;
	  dndObj = widget;
	}

      if(dndObj != NULL)
	{
	  Window  ptWin = event->xbutton.window;
	  Window oldPtWin = ptWin;
	  Window widgetwin = ptWin;
	  unsigned int actionType = (event->xbutton.state) & (ShiftMask | ControlMask | Mod1Mask);
	  int helpRequested = 0;
	  int dragCancelled = 0;
	  Window    root, child;
	  unsigned  int    mask;
	  int       x,y,ix0, iy0, iww, ihh, iarea, snapshot = 0, wox,woy;
	  int       Ox, Oy, oldx, oldy, dx,dy, rootx, rooty,tx,ty,Sx,Sy, moved, grabed;
	  int      xoffset, yoffset, bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
	  XEvent   xevent;
	      
	  xoffset = bw + EZ_WidgetPadX(widget) ;
	  yoffset = bw + EZ_WidgetPadY(widget);
	  x = event->xbutton.x;
	  y = event->xbutton.y;

	  /* */ Ox = Oy = oldx = oldy = 0; iww = ihh = 1; 
	  {
	    unsigned long serial = 0L;
	    int newEvent = 0;
	    int done = 0;
	    moved = 0;
	    grabed = 0;
	    while(!done)
	      {
		while(XPending(EZ_Display) && !done) 
		  {
		    XNextEvent(EZ_Display, &xevent); 	      
		    if(EZ_FilterEvent(&xevent))
		      EZ_InvokePrivateEventHandler(&xevent);
		    newEvent = 1;
		    if(xevent.type == MotionNotify) 
		      {
			moved = 1;
			serial = xevent.xmotion.serial; 
			ptWin = xevent.xmotion.window; 
			if(xevent.xmotion.window == widgetwin)
			  {
			    if(xevent.xmotion.x < 0 ||  xevent.xmotion.y < 0 ||
			       xevent.xmotion.x >= EZ_WidgetWidth(widget) ||  
			       xevent.xmotion.y >= EZ_WidgetHeight(widget))
			      ptWin = 0L;
			  }
			if(helpRequested)
			  {
			    EZ_DnDSendCancelHelpMsg();
			    EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			    helpRequested = 0;
			  }
		      }
		    else if(xevent.type == ButtonRelease)
		      {
			if(xevent.xbutton.button == EZ_Btn3) 
			  {
			    ptWin = xevent.xbutton.window; 
			    if(xevent.xbutton.window == widgetwin)
			      {
				if(xevent.xbutton.x < 0 ||  xevent.xbutton.y < 0 ||
				   xevent.xbutton.x >= EZ_WidgetWidth(widget) ||  
				   xevent.xbutton.y >= EZ_WidgetHeight(widget))
				  ptWin = 0L;
			      }
			    done = 1;
			    break;
			  }
		      }
		    else if(xevent.type == KeyPress)
		      {
			KeySym            keysym;
			XComposeStatus    compose; 
			char              tmpbuffer[4];
			int               count;
			count = XLookupString(&(xevent.xkey), tmpbuffer, 4, &keysym, &compose);
			switch(keysym)
			  {
			  case XK_F1: 
#ifdef XK_KP_F1
			  case XK_KP_F1:  /* help */
#endif
			    if(grabed && helpRequested == 0 && ptWin != 0)
			      {
				EZ_DnDInfo.ptWin = (unsigned long)ptWin;
				EZ_DnDSendRequestHelpMsg();
				helpRequested = 1;
			      }
			    break;
			  case XK_Escape:   /* cancel */
			    {
			      dragCancelled = 1;
			      done = 1;
			    }
			  break;
			  default:
			    break;
			  }
		      }
		    else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
		      EZ_WidgetDispatchEvent(&xevent);

		    if(oldPtWin != ptWin)
		      {
			if(grabed && oldPtWin != 0)
			  {
			    unsigned long twin = EZ_DnDInfo.ptWin;
			    EZ_DnDInfo.ptWin = (unsigned long)(oldPtWin);
			    EZ_DnDSendLeaveWindowMsg();
			    EZ_DnDInfo.ptWin = twin;
			  }
			oldPtWin = ptWin;
		      }
		  }
		if(newEvent == 0)
		  {
		    EZ_CheckDnDTimer();
		    EZ_SitForALittleBit(1000);
		  }
		
		if(!done && newEvent) /* not done yet */
		  {
		    newEvent = 0;
		    if(moved)
		      {
			if(grabed == 0)
			  {
			    XTranslateCoordinates(EZ_Display, widgetwin,
						  RootWindow(EZ_Display, EZ_ScreenNum),
						  0, 0,
						  &wox,&woy, &child);
			    ix0 = EZ_ItemOriginX(nsitem);
			    iy0 = EZ_ItemOriginY(nsitem);
			    iww = EZ_ItemAWidth(nsitem);
			    ihh = EZ_ItemAHeight(nsitem);
			    iarea  = iww * ihh;
			    oldx = wox + x;
			    oldy = woy + y;
				  
			    tx = ix0 - x;
			    ty = iy0 - y;
				  
			    Sx = Ox = oldx + tx;
			    Sy = Oy = oldy + ty;
			    EZ_InitDrag(serial, dndType, widgetwin,
					widget, dndObj, Sx,Sy, iww, ihh, oldx, oldy, tx, ty);
			    if(EZ_DnDInfo.id == serial)				  
			      {
				      
				EZ_CallDragInitProcedure();
				if(EZ_DnDInfo.cursor == None)
				  EZ_DnDInfo.cursor = EZ_ItemDnDDragCursor(nsitem);
				if(EZ_DnDInfo.cursor != None || EZ_DnDInfo.dragIcon != NULL)
				  snapshot = EZ_DND_DRAG_ICON_CURSOR;
				else if(iarea < 40000)
				  snapshot = EZ_MakeSnapShot(widget,1,ix0,iy0,iww,ihh);
				else snapshot = EZ_DND_DRAG_ICON_OUTLINE;
				{
				  EZ_DnDInfo.actionType = actionType;
				  EZ_DnDInfo.px = oldx;
				  EZ_DnDInfo.py = oldy;
				  EZ_DnDInfo.dragIconType = snapshot;
				  {
				    char *smsg; int smsg_len;
				    EZ_GenerateDragStartedMessage(&smsg, &smsg_len);
				    EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,smsg, smsg_len, 0);
				  }
				  /* EZ_BroadcastDnDMessage(EZ_DND_DRAG_START, NULL, 0, 0);*/
				}
				if(EZ_PointerGrabed == 0)  /* grab the pointer ! */
				  {
				    if(XGrabPointer(EZ_Display,widgetwin, True,
						    ButtonReleaseMask|ButtonPressMask|PointerMotionMask,
						    GrabModeAsync, GrabModeAsync, None, 
						    EZ_DnDInfo.cursor, CurrentTime)
				       == GrabSuccess) EZ_PointerGrabed = 1;
				  }	
				grabed = 1;
			      }	
			  }
			if(grabed)
			  {
			    XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
					  &root,       /* root return win */
					  &child,      /* child ret win   */
					  &rootx, &rooty,   /* x, y in root    */
					  &tx,&ty,            /* x, y in win     */
					  &mask);  
			    dx = rootx - oldx;  dy = rooty - oldy;
			    oldx = rootx;   oldy = rooty;

			    if(dx | dy)
			      {
				Ox += dx; Oy += dy;
				if(snapshot != EZ_DND_DRAG_ICON_CURSOR)
				  EZ_MoveSnapShotOrOutline(snapshot, Ox, Oy, iww, ihh, 1);
				else if(EZ_DnDInfo.dragIcon != NULL) EZ_ShowDragIcon(rootx, rooty);
				EZ_DnDInfo.px = oldx;
				EZ_DnDInfo.py = oldy;
				EZ_DnDInfo.ptWin = (unsigned long)ptWin;
				EZ_BroadcastDnDMessage(EZ_DND_DRAG_MOTION, NULL, 0, 0);
				if(snapshot != EZ_DND_DRAG_ICON_CURSOR)
				  EZ_MoveSnapShotOrOutline(snapshot, Ox, Oy, iww, ihh, 0);
			      }
			  } /* grabed */
		      }/* moved */
		  } /* not done */			    
		else if(done) /* we are done */
		  {
		    if(EZ_PointerGrabed)
		      {
			XUngrabPointer(EZ_Display,CurrentTime);
			EZ_PointerGrabed = 0;
		      }
		    if(grabed) /* dragged */
		      {
			char *message;
			int length;
			Window commWin;
			      
			if(snapshot != EZ_DND_DRAG_ICON_CURSOR)
			  EZ_MoveSnapShotOrOutline(snapshot, 0,0,0,0,1);
			else if(EZ_DnDInfo.dragIcon != NULL) EZ_HideWidget(EZ_DnDInfo.dragIcon);
			if(ptWin != 0)  commWin = (Window) EZ_WindowIsDnDTarget(ptWin);
			else commWin = 0L;
			EZ_FinishDrag(commWin, ptWin, oldx, oldy); /* prepare for drop */

			if(commWin != 0L)
			  {
			    if(helpRequested)
			      {
				EZ_DnDSendCancelHelpMsg();
				helpRequested = 0;
			      }

			    if(dragCancelled == 0)
			      {
				EZ_DnDInfo.actionType = actionType;
				EZ_GenerateDragIsDropedMessage(&message, &length);
				EZ_SendDnDMessage(EZ_DND_DRAG_DROPPED, message, length, 1);
			      }
			    else  /* cancelled */
			      {
				EZ_SendDnDMessage(EZ_DND_DRAG_CANCELLED,NULL, 0, 0);
				EZ_AbortDrag();
			      }
			  }
			else if(ptWin == RootWindow(EZ_Display, EZ_ScreenNum) &&
				dragCancelled == 0)
			  {
			    /* drop to the root window, handle this internally */
			    EZ_DnDDataEncoder  *encoder = EZ_FindSpecialEncoder();
			    if(encoder)
			      {
				char *msg; int mlen, needFree, ok = EZ_DND_FAILURE;

				EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;
				ok = (encoder->encoder) (encoder->object,
							 encoder->data,
							 &msg, &mlen, &needFree);
				if(ok != EZ_DND_FAILURE)
				  {
				    if((encoder->callback).callback)
				      {
					EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;
					((encoder->callback).callback)(encoder->object, 
								       (encoder->callback).data);
				      }
				    EZ_DnDInfo.id = 0; /* forget about this drag */   
				  }
				else EZ_AbortDrag();
			      }
			    else  EZ_AbortDrag();
			  }
			else    EZ_AbortDrag();
		      }
		  }
	      }/* while */
	  }  /* is a drag src */
	}
    }
}
/************************************************************************************/
static void itreeDnDNNodes(widget, nnn, event,Rx, Ry,Rw,Rh)
     EZ_Widget *widget; int Rx, Ry, Rw,Rh, nnn; XEvent *event;
{
  int dndType = EZ_DND_OBJ_IS_WIDGET;
  void  *dndObj = widget;

  /* pick the item under the pointer */
  if(nnn &&  EZ_WidgetIsDnDSrc(widget))
    {
      Window  ptWin = event->xbutton.window;
      Window oldPtWin = ptWin;
      Window widgetwin = ptWin;
      unsigned int actionType = (event->xbutton.state) & (ShiftMask | ControlMask | Mod1Mask);
      int helpRequested = 0;
      int dragCancelled = 0, dcnt = 0;
      Window    root, child;
      unsigned  int    mask;
      int       x,y, iww, ihh, snapshot = 0, wox,woy;
      int       Ox, Oy, oldx, oldy, dx,dy, rootx, rooty,tx,ty,Sx,Sy, moved, grabed;
      int      xoffset, yoffset, bw = EZ_WidgetBorderWidth(widget) + EZ_WidgetPadB(widget);
      XEvent   xevent;
	      
      xoffset = bw + EZ_WidgetPadX(widget) ;
      yoffset = bw + EZ_WidgetPadY(widget);
      x = event->xbutton.x;
      y = event->xbutton.y;

      /* */ Ox = Oy = oldx = oldy = 0; iww = ihh = 1; 
      {
	unsigned long serial = 0L;
	int newEvent = 0;
	int done = 0;
	moved = 0;
	grabed = 0;
	while(!done)
	  {
	    while(XPending(EZ_Display) && !done) 
	      {
		XNextEvent(EZ_Display, &xevent); 	      
		if(EZ_FilterEvent(&xevent))
		  EZ_InvokePrivateEventHandler(&xevent);
		newEvent = 1;
		if(xevent.type == MotionNotify) 
		  {
		    moved = 1;
		    serial = xevent.xmotion.serial; 
		    ptWin = xevent.xmotion.window; 
		    if(xevent.xmotion.window == widgetwin)
		      {
			if(xevent.xmotion.x < 0 ||  xevent.xmotion.y < 0 ||
			   xevent.xmotion.x >= EZ_WidgetWidth(widget) ||  
			   xevent.xmotion.y >= EZ_WidgetHeight(widget))
			  ptWin = 0L;
		      }
		    if(helpRequested)
		      {
			EZ_DnDSendCancelHelpMsg();
			EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			helpRequested = 0;
		      }
		  }
		else if(xevent.type == ButtonRelease)
		  {
		    if(xevent.xbutton.button == EZ_Btn3) 
		      {
			ptWin = xevent.xbutton.window; 
			if(xevent.xbutton.window == widgetwin)
			  {
			    if(xevent.xbutton.x < 0 ||  xevent.xbutton.y < 0 ||
			       xevent.xbutton.x >= EZ_WidgetWidth(widget) ||  
			       xevent.xbutton.y >= EZ_WidgetHeight(widget))
			      ptWin = 0L;
			  }
			done = 1;
			break;
		      }
		  }
		else if(xevent.type == KeyPress)
		  {
		    KeySym            keysym;
		    XComposeStatus    compose; 
		    char              tmpbuffer[4];
		    int               count;
		    count = XLookupString(&(xevent.xkey), tmpbuffer, 4, &keysym, &compose);
		    switch(keysym)
		      {
		      case XK_F1: 
#ifdef XK_KP_F1
		      case XK_KP_F1:  /* help */
#endif
			if(grabed && helpRequested == 0 && ptWin != 0)
			  {
			    EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			    EZ_DnDSendRequestHelpMsg();
			    helpRequested = 1;
			  }
			break;
		      case XK_Escape:   /* cancel */
			{
			  dragCancelled = 1;
			  done = 1;
			}
		      break;
		      default:
			break;
		      }
		  }
		else if(xevent.type == Expose || xevent.type == FocusIn || xevent.type == FocusOut) 
		  EZ_WidgetDispatchEvent(&xevent);

		if(oldPtWin != ptWin)
		  {
		    if(grabed && oldPtWin != 0)
		      {
			unsigned long twin = EZ_DnDInfo.ptWin;
			EZ_DnDInfo.ptWin = (unsigned long)(oldPtWin);
			EZ_DnDSendLeaveWindowMsg();
			EZ_DnDInfo.ptWin = twin;
		      }
		    oldPtWin = ptWin;
		  }
	      }
	    if(newEvent == 0)
	      {
		EZ_CheckDnDTimer();
		EZ_SitForALittleBit(1000);
	      }
		
	    if(!done && newEvent) /* not done yet */
	      {
		newEvent = 0;
		if(moved)
		  {
		    if(grabed == 0)
		      {
			XTranslateCoordinates(EZ_Display, widgetwin,
					      RootWindow(EZ_Display, EZ_ScreenNum),
					      0, 0,
					      &wox,&woy, &child);
			oldx = wox + x;
			oldy = woy + y;
			Sx = oldx;
			Sy = oldy;
			EZ_InitDrag(serial, dndType, widgetwin,
				    widget, dndObj, Sx,Sy, 100, 100, oldx, oldy, 0, 0);
			if(EZ_DnDInfo.id == serial)				  
			  {
			    EZ_CallDragInitProcedure();

			    if(EZ_DnDInfo.cursor == None)
			      EZ_DnDInfo.cursor = EZ_WidgetDnDDragCursor(widget);
			    if(EZ_DnDInfo.cursor != None || EZ_DnDInfo.dragIcon != NULL)
			      snapshot = EZ_DND_DRAG_ICON_CURSOR;
			    else 
			      {
				snapshot = EZ_DND_DRAG_ICON_SPECIAL;
				itreeMakeNNodeOutlines(widget,nnn,wox,woy);
			      }

			    {
			      EZ_DnDInfo.actionType = actionType;
			      EZ_DnDInfo.px = oldx;
			      EZ_DnDInfo.py = oldy;
			      EZ_DnDInfo.dragIconType = snapshot;
			      {
				char *smsg; int smsg_len;
				EZ_GenerateDragStartedMessage(&smsg, &smsg_len);
				EZ_BroadcastDnDMessage(EZ_DND_DRAG_START,smsg, smsg_len, 0);
			      }
			    }
			    if(EZ_PointerGrabed == 0)  /* grab the pointer ! */
			      {
				if(XGrabPointer(EZ_Display,widgetwin, True,
						ButtonReleaseMask|ButtonPressMask|PointerMotionMask,
						GrabModeAsync, GrabModeAsync, None, 
						EZ_DnDInfo.cursor, CurrentTime)
				   == GrabSuccess) EZ_PointerGrabed = 1;
			      }	
			    grabed = 1;
			  }	
		      }
		    if(grabed)
		      {
			XQueryPointer(EZ_Display, RootWindow(EZ_Display, EZ_ScreenNum),
				      &root,       /* root return win */
				      &child,      /* child ret win   */
				      &rootx, &rooty,   /* x, y in root    */
				      &tx,&ty,            /* x, y in win     */
				      &mask);  
			dx = rootx - oldx;  dy = rooty - oldy;
			oldx = rootx;   oldy = rooty;
			if(dx | dy)
			  {
			    if(snapshot == EZ_DND_DRAG_ICON_SPECIAL)
			      {
				EZ_MoveNItemOutlines(dx,dy, 0, dcnt,1);
				dcnt++;
				EZ_MoveNItemOutlines(dx,dy, 1, dcnt,1);
				XFlush(EZ_Display);
			      }
			    else if(EZ_DnDInfo.dragIcon != NULL) EZ_ShowDragIcon(rootx, rooty);
			    EZ_DnDInfo.px = oldx;
			    EZ_DnDInfo.py = oldy;
			    EZ_DnDInfo.ptWin = (unsigned long)ptWin;
			    EZ_BroadcastDnDMessage(EZ_DND_DRAG_MOTION, NULL, 0, 0);
			  }
		      } /* grabed */
		  }/* moved */
	      } /* not done */			    
	    else if(done) /* we are done */
	      {
		if(EZ_PointerGrabed)
		  {
		    XUngrabPointer(EZ_Display,CurrentTime);
		    EZ_PointerGrabed = 0;
		  }
		if(grabed) /* dragged */
		  {
		    char *message;
		    int length;
		    Window commWin;
		    if(snapshot == EZ_DND_DRAG_ICON_SPECIAL)
		      EZ_MoveNItemOutlines(0,0, 0, 1,1);			      
		    else if(EZ_DnDInfo.dragIcon != NULL) EZ_HideWidget(EZ_DnDInfo.dragIcon);
		    if(ptWin != 0)  commWin = (Window) EZ_WindowIsDnDTarget(ptWin);
		    else commWin = 0L;
		    EZ_FinishDrag(commWin, ptWin, oldx, oldy); /* prepare for drop */

		    if(commWin != 0L)
		      {
			if(helpRequested)
			  {
			    EZ_DnDSendCancelHelpMsg();
			    helpRequested = 0;
			  }

			if(dragCancelled == 0)
			  {
			    EZ_DnDInfo.actionType = actionType;
			    EZ_GenerateDragIsDropedMessage(&message, &length);
			    EZ_SendDnDMessage(EZ_DND_DRAG_DROPPED, message, length, 1);
			  }
			else  /* cancelled */
			  {
			    EZ_SendDnDMessage(EZ_DND_DRAG_CANCELLED,NULL, 0, 0);
			    EZ_AbortDrag();
			  }
		      }
		    else if(ptWin == RootWindow(EZ_Display, EZ_ScreenNum) &&
			    dragCancelled == 0)
		      {
			/* drop to the root window, handle this internally */
			EZ_DnDDataEncoder  *encoder = EZ_FindSpecialEncoder();
			if(encoder)
			  {
			    char *msg; int mlen, needFree, ok = EZ_DND_FAILURE;

			    EZ_DnDInfo.srcStatus = EZ_DRAG_CONVERTING;
			    ok = (encoder->encoder) (encoder->object,
						     encoder->data,
						     &msg, &mlen, &needFree);
			    if(ok != EZ_DND_FAILURE)
			      {
				if((encoder->callback).callback)
				  {
				    EZ_DnDInfo.srcStatus = EZ_DRAG_EXECUTING_CALLBACK;
				    ((encoder->callback).callback)(encoder->object, 
								   (encoder->callback).data);
				  }
				EZ_DnDInfo.id = 0; /* forget about this drag */   
			      }
			    else EZ_AbortDrag();
			  }
			else  EZ_AbortDrag();
		      }
		    else    EZ_AbortDrag();
		  }
	      }
	  }/* while */
      }  /* is a drag src */
    }
}
/************************************************************************************/
static void itreeGetHNodes(node, iholder, idx)
     EZ_TreeNode *node;  EZ_Item **iholder; int *idx;
{
  if(node)
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(node);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(node);
      EZ_Item  *item = EZ_TreeNodeItem(node);
      if(item && EZ_ItemHighlighted(item))
	{
	  iholder[*idx] = item;
	  *idx += 1;
	}
      itreeGetHNodes(children, iholder, idx);
      itreeGetHNodes(sibling, iholder, idx);
    }
}
static void itreeMakeNNodeOutlines(widget, nnn, wox, woy)
     EZ_Widget *widget; int nnn, wox,woy;
{
  EZ_Item **items, *itemsStor[256];
  if(nnn >= 256) items = (EZ_Item **)malloc( (nnn+1) * sizeof(EZ_Item *));
  else items = itemsStor;
  {
    int idx = 0;
    EZ_TreeNode *root = EZ_ITreeWRoot(widget);
    itreeGetHNodes(root, items, &idx);
  }
  EZ_MakeNItemOutlines(items, nnn, wox,woy);
  if(nnn >= 256) (void)free(items);
}
/************************************************************************************/
static void findHLNodes(node, iholder, idx)
     EZ_TreeNode *node;  EZ_TreeNode **iholder; int *idx;
{
  if(node)
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(node);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(node);
      EZ_Item  *item = EZ_TreeNodeItem(node);
      if(item && EZ_ItemHighlighted(item))
	{
	  iholder[*idx] = node;
	  *idx += 1;
	}
      findHLNodes(children, iholder, idx);
      findHLNodes(sibling, iholder, idx);
    }
}
/************************************************************************************/
EZ_TreeNode **EZ_ITreeGetHighlightedNodesA(widget, cnt_ret)
     EZ_Widget *widget; int *cnt_ret;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_ITREE)
    {
      int total = itreeGetNumHighlightedNodes(widget);
      if(total > 0)
	{
	  EZ_TreeNode **ret = (EZ_TreeNode **)malloc((total + 1)*sizeof(EZ_TreeNode *));
	  int idx = 0;
	  EZ_TreeNode *root = EZ_ITreeWRoot(widget);
	  findHLNodes(root, ret, &idx);
	  if(cnt_ret) *cnt_ret = idx;
	  return(ret);
	}
    }
  return(NULL);
}
/************************************************************************************/
EZ_Item **EZ_ITreeGetHighlightedItemsA(widget, cnt_ret)
     EZ_Widget *widget; int *cnt_ret;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_ITREE)
    {
      int total = itreeGetNumHighlightedNodes(widget);
      if(total > 0)
	{
	  EZ_Item **items = (EZ_Item **)malloc((total+1)*sizeof(EZ_Item *));
	  int idx = 0;
	  EZ_TreeNode *root = EZ_ITreeWRoot(widget);
	  itreeGetHNodes(root, items, &idx);	  
	  if(cnt_ret) *cnt_ret = idx;
	  return(items);	  
	}
    }
  return(NULL);
}
/************************************************************************************/
int EZ_ITreeGetHighlightedNodes(widget, ary, siz)
     EZ_Widget *widget; EZ_TreeNode **ary; int siz;
{
  int cnt = 0;
  EZ_TreeNode **hnodes = EZ_ITreeGetHighlightedNodesA(widget, &cnt);
  if(hnodes)
    {
      int i, total = cnt < siz? cnt: siz;
      for(i = 0; i < total; i++) ary[i]=hnodes[i];
      (void)free(hnodes);
    }
  return(cnt);
}
/************************************************************************************/
int EZ_ITreeGetHighlightedItems(widget, ary, siz)
     EZ_Widget *widget; EZ_Item **ary; int siz;
{
  int cnt = 0;
  EZ_Item **hitems = EZ_ITreeGetHighlightedItemsA(widget, &cnt);
  if(hitems)
    {
      int i, total = cnt < siz? cnt: siz;
      for(i = 0; i < total; i++) ary[i]=hitems[i];
      (void)free(hitems);
     }
  return(cnt);
}
/************************************************************************************/
static EZ_TreeNode *EZ_PointerOnBranchNode(x0, y0, root)
     int x0, y0; EZ_TreeNode *root;
{
  EZ_TreeNode *children, *sibling;
 
  if(root == NULL) { return(NULL); }
  children = EZ_TreeNodeChildren(root);  
  sibling  = EZ_TreeNodeSibling(root);  

  if(children || EZ_TreeNodeOpenable(root))
    {
      EZ_Item *item;
      item = EZ_TreeNodeItem(root);
      if(item)
	{
	  int x1, y1, indent = EZ_TreeNodeIndent(root);
	  int x = EZ_ItemOriginX(item);
	  int y = EZ_ItemOriginY(item);
	  int h = EZ_ItemAHeight(item);
	  
	  x1 = x - indent;
	  y1 = y + (h>> 1) - 4;
	  
	  if( x0 >= x1 && x0 <= x1 + 8 && y0 >= y1 && y0 <= y1 + 8)
	    { return(root); }

	  if(children && EZ_TreeNodeHideSubtree(root) == 0)
	    {
	      EZ_TreeNode *tmp = EZ_PointerOnBranchNode(x0,y0, children);
	      if(tmp) return(tmp);
	    }
	}
    }
  return(EZ_PointerOnBranchNode(x0,y0, sibling));
}
/************************************************************************************/
#undef _EZ_WIDGET_ITREE_C_

