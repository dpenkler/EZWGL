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
 *   Trees
 *
 ****************************************************************/
#define _EZ_TREE_C_
#include "EZ_Widget.h"
/*
 * public functions
 */
EZ_TreeNode  *EZ_CreateTreeNode  MY_ANSIARGS((EZ_TreeNode *parent, EZ_Item *item));      
EZ_TreeNode  *EZ_TreeNodeGetParent MY_ANSIARGS((EZ_TreeNode *node));      
EZ_TreeNode  *EZ_TreeNodeGetChildren MY_ANSIARGS((EZ_TreeNode *node));      
EZ_TreeNode  *EZ_TreeNodeGetSibling MY_ANSIARGS((EZ_TreeNode *node));      
EZ_TreeNode  *EZ_TreeFindNode MY_ANSIARGS((EZ_TreeNode *root, EZ_TreeNode *node));
EZ_TreeNode  *EZ_TreeFindNodeUsingItem MY_ANSIARGS((EZ_TreeNode *root, EZ_Item *item));
EZ_TreeNode  *EZ_TreeFindNodeUsingProcedure MY_ANSIARGS((EZ_TreeNode *root, 
			 EZ_TreeNode *(*f)MY_ANSIARGS((EZ_TreeNode *node, void *d)), void *data ));
void     EZ_TreeDestroySubtree MY_ANSIARGS((EZ_TreeNode *node));      
void     EZ_TreeDestroyNodeDescendants MY_ANSIARGS((EZ_TreeNode *node));      
void     EZ_TreeAttachSubtree MY_ANSIARGS((EZ_TreeNode *node, EZ_TreeNode *subtree));      
void     EZ_TreeDetachSubtree MY_ANSIARGS((EZ_TreeNode *subtree));      
void     EZ_TreeHideSubtree MY_ANSIARGS((EZ_TreeNode *node));      
void     EZ_TreeShowSubtree MY_ANSIARGS((EZ_TreeNode *node));      
void     EZ_TreeNodeMarkTerminal MY_ANSIARGS((EZ_TreeNode *node, int flag));      

void     EZ_TreeSetCharacter MY_ANSIARGS((EZ_TreeNode *node,
					      int type, int indent, int spacing, int drawBranch));      

void     EZ_TreeTraverseTree MY_ANSIARGS((EZ_TreeNode *node,
					  void (*preF) MY_ANSIARGS((EZ_TreeNode *node)),
					  void (*postF) MY_ANSIARGS((EZ_TreeNode *node))	  ));

void     EZ_TreeSetFGColor  MY_ANSIARGS((EZ_TreeNode *node, unsigned long pv));

void     EZ_TreeSetBGColor MY_ANSIARGS((EZ_TreeNode *node, unsigned long pv));

void     EZ_TreeSetFGColorName  MY_ANSIARGS((EZ_TreeNode *node, char *name));

void     EZ_TreeSetBGColorName MY_ANSIARGS((EZ_TreeNode *node, char *name));

void     EZ_ComputeSubtreeSize MY_ANSIARGS((EZ_TreeNode *node, int *w, int *h, int *par));

void     EZ_DisplayTreeInDrawable MY_ANSIARGS((EZ_Widget *widget, Drawable drawable,
					       int x0, int y0,
					       EZ_TreeNode *node,
					       int Rx, int Ry, int Rw, int Rh));

EZ_Item *EZ_TreeNodeGetItem MY_ANSIARGS((EZ_TreeNode *node));

/*
 *  Local functions.
 */
static void EZ_TreeTraverseTreeWork MY_ANSIARGS((EZ_TreeNode *node,
					     void (*preF) MY_ANSIARGS((EZ_TreeNode *node)),
					     void (*postF) MY_ANSIARGS((EZ_TreeNode *node)), int l));

static void EZ_TreeSetFGColorWork  MY_ANSIARGS((EZ_TreeNode *node, unsigned long pv, int lv));

static void EZ_TreeSetBGColorWork MY_ANSIARGS((EZ_TreeNode *node, unsigned long pv, int lv));

static void EZ_SetTreeCharacterWork  MY_ANSIARGS((EZ_TreeNode *node,
						  int type, int indent, int spacing, int drawBranch));

static void EZ_DestroyTreeWork MY_ANSIARGS((EZ_TreeNode *node, int level));
/*****************************************************************
 *
 *  Functions deal with tree nodes.
 */
EZ_TreeNode *EZ_CreateTreeNode(parent,item)
     EZ_TreeNode *parent;  /* parent node */
     EZ_Item     *item;
{
  EZ_TreeNode *node;

  if(item == NULL) return(NULL);
  node = (EZ_TreeNode *)my_malloc(sizeof(EZ_TreeNode), _TREE_NODE_);
  if(node)
    {
      memset(node, 0, sizeof(EZ_TreeNode));
      EZ_TreeNodeItem(node) = item;
      EZ_TreeNodeChildren(node) = (EZ_TreeNode *)NULL;
      EZ_TreeNodeSibling(node) = (EZ_TreeNode *)NULL;
      EZ_TreeNodeParent(node) = parent;
      EZ_TreeNodeDrawBranch(node) = 1;
      EZ_TreeNodeType(node) = EZ_LIST_TREE_NODE;

      if(parent)
	{
	  EZ_TreeNode *sib;
	  
	  EZ_TreeNodeIndent(node) = EZ_TreeNodeIndent(parent);
	  EZ_TreeNodeSpacing(node) = EZ_TreeNodeSpacing(parent);
	  sib = EZ_TreeNodeChildren(parent);
	  if(sib == (EZ_TreeNode *)NULL) EZ_TreeNodeChildren(parent) = node;
	  else
	    {
	      while(EZ_TreeNodeSibling(sib))
		sib = EZ_TreeNodeSibling(sib);
	      EZ_TreeNodeSibling(sib) = node;
	    }
	}
      else
	{
	  EZ_TreeNodeType(node) = EZ_LIST_TREE_NODE;
	  EZ_TreeNodeIndent(node) =  EZ_TREE_NODE_INDENTATION;
	  EZ_TreeNodeSpacing(node) = EZ_TREE_NODE_VSPACING;
	}
    }
  else EZ_OutOfMemory("EZ_CreateTreeNode");
  return(node);
}

/*****************************************************************
 *
 *  compute the geometry of a subtree.
 *
 */
void EZ_ComputeSubtreeSize(node, w_ret, h_ret, parity)
     EZ_TreeNode *node;
     int *w_ret, *h_ret; 
     int *parity;
{
  EZ_TreeNode *children, *sibling;
  EZ_Item *item;
  int type, w, h, tw, th, maxw, maxh;
  
  if(node == (EZ_TreeNode *)NULL)
    { *w_ret = 0; *h_ret = 0; return;}
  
  item = EZ_TreeNodeItem(node);
  EZ_ComputeItemSize(item, &tw, &th);
  EZ_ItemRowParity(item) = (*parity)? 1:0;
  *parity = ((*parity) == 0);
  maxw = tw; 
  maxh = th;

  type = EZ_TreeNodeType(node);
  
  switch(type)
    {
    case EZ_TREE_NODE:  /* toplogical tranversal */
      children = EZ_TreeNodeChildren(node);
      if(children)
        {
          int tmaxh;
          EZ_ComputeSubtreeSize(children, &w, &h, parity);
          maxw = (w > maxw ? w: maxw);
          tmaxh = maxh + (int) EZ_TreeNodeSpacing(node) + h;
	  
          sibling = EZ_TreeNodeSibling(children);
          while(sibling)
            {
              int itmp, stype = EZ_TreeNodeType(sibling);
              int xindent, yindent;

              EZ_ComputeSubtreeSize(sibling, &w, &h, parity);
              if(stype == EZ_TREE_NODE)
                {
                  xindent =  (int) EZ_TreeNodeIndent(node);
                  yindent = (int) EZ_TreeNodeSpacing(node);
                }
              else
                {
                  yindent =  (int) EZ_TreeNodeIndent(node);
                  xindent = (int) EZ_TreeNodeSpacing(node);
                }
              itmp = w + xindent;
              maxw += itmp;
              itmp = maxh + yindent + h; 
              tmaxh = (itmp > tmaxh ? itmp: tmaxh);
              sibling = EZ_TreeNodeSibling(sibling);
            }
          maxh = tmaxh;
        }
      break;
    case EZ_LIST_TREE_NODE:
      children = EZ_TreeNodeChildren(node);
      if(children)
        {
          int itmp;
          EZ_ComputeSubtreeSize(children, &w, &h, parity);
          itmp = w + (int) EZ_TreeNodeIndent(node);
          maxw = (itmp > maxw ? itmp: maxw);
          maxh += (int) EZ_TreeNodeSpacing(node) + h;
        }
      sibling = EZ_TreeNodeSibling(node);
      if(sibling)
        {
          int itmp;
          EZ_ComputeSubtreeSize(sibling, &w, &h, parity);
          itmp = h + (int) EZ_TreeNodeSpacing(node);
          maxw = (w > maxw ? w: maxw);
          maxh  += itmp;
        }
      break;
    case EZ_TAGED_TREE_NODE:
      if(EZ_TreeNodeHideSubtree(node)== 0)
	{
	  children = EZ_TreeNodeChildren(node);
	  if(children)
	    {
	      int itmp;
	      EZ_ComputeSubtreeSize(children, &w, &h, parity);
	      itmp = w + (int) EZ_TreeNodeIndent(node);
	      maxw = (itmp > maxw ? itmp: maxw);
	      maxh += (int) EZ_TreeNodeSpacing(node) + h;
	    }
	}
      sibling = EZ_TreeNodeSibling(node);
      if(sibling)
        {
          int itmp;
          EZ_ComputeSubtreeSize(sibling, &w, &h, parity);
          itmp = h + (int) EZ_TreeNodeSpacing(node);
          maxw = (w > maxw ? w: maxw);
          maxh  += itmp;
        }
      break;      
    default:
      break;
    }   
  
  if(type == EZ_TAGED_TREE_NODE && EZ_TreeNodeParent(node) == NULL)
    maxw += (int) EZ_TreeNodeIndent(node);

  EZ_TreeNodeTWidth(node) =  maxw;
  EZ_TreeNodeTHeight(node) = maxh;
  *w_ret = maxw;
  *h_ret = maxh;
}
/*************************************************************************
 *
 * Draw a tree on drawable. Drawable should be a pixmap.
 */
static void draw_tree_branch(drawable, parent, node)
     Drawable drawable;
     EZ_TreeNode *parent, *node;
{
  EZ_Item *pitem, *item;
  EZ_TreeNode *sibling;
  int x0,y0,x1,x2,y1,y2,tmp, type;
  int drawbranch = EZ_TreeNodeDrawBranch(node);
  GC gc;
  
  pitem = parent? EZ_TreeNodeItem(parent): NULL;
  item =  EZ_TreeNodeItem(node);
  type = EZ_TreeNodeType(node);
  if(item)
    {
      gc= EZ_DARKGC1;

      x0 = (int)(EZ_ItemOriginX(item));
      y0 = (int)(EZ_ItemOriginY(item));  
      switch(type)
        {
        case EZ_TAGED_TREE_NODE:
          {
	    EZ_TreeNode *children = EZ_TreeNodeChildren(node);
            int openable = EZ_TreeNodeOpenable(node);
            if(children || openable)
              {
                int hidden = EZ_TreeNodeHideSubtree(node);
                int indent = (int) EZ_TreeNodeIndent(node);
                int space = (int) EZ_TreeNodeSpacing(node);
                int hh = EZ_ItemHeight(item);
                int h2 = hh >> 1;
                x1 = x0 - indent;
                y1 = y0 + h2 - 4;
                y2 = y1+4;
                XDrawRectangle(EZ_Display,drawable,gc,x1,y1,8,8);
                XDrawLine(EZ_Display, drawable, gc, x1+2,y2,x1+7,y2);
                if(hidden || children == NULL)
                  XDrawLine(EZ_Display, drawable, gc, x1+4,y1+2,x1+4,y1+7);
                XDrawLine(EZ_Display, drawable, gc, x1+9,y2,x0,y2);

		if(drawbranch)
		  {
		    if(parent)
		      {
			x1 += 4;
			x2 = x1;
			y2 = y1 - h2 + 4;
			XDrawLine(EZ_Display, drawable, gc, x1, y1, x2,y2);
			sibling = EZ_TreeNodeSibling(node);
			if(sibling)
			  {
			    EZ_Item *sitem =  EZ_TreeNodeItem(sibling);
			    if(sitem)
			      {
				y2 = (int)(EZ_ItemOriginY(sitem));  
				y1 = y0 + h2 + 4;
				XDrawLine(EZ_Display, drawable, gc, x1, y1, x2,y2);
			      }
			  }
		      }
		    if(children && EZ_TreeNodeHideSubtree(node) == 0)
		      {
			tmp = indent >> 1;
			x1 =  x0 - tmp +4;
			y1 = y0 + h2;
			x2 = x1;
			y2 = y0 + hh + space;
			XDrawLine(EZ_Display, drawable, gc, x1, y1, x2,y2);
		      }
		  }
              }
            else if(drawbranch)
              {
                int indent = (int) EZ_TreeNodeIndent(node);
                int h2 =  ((EZ_ItemHeight(item))>> 1);
                x1 = x0 - indent + 4;    
                y1 = y0 + h2;
                x2 = x0;
                y2 = y1;
                XDrawLine(EZ_Display, drawable, gc, x1, y1, x2,y2);
                if(parent)
                  {
                    int space = (int) EZ_TreeNodeSpacing(node);
                    x2 = x1;
                    y2 = y0;
                    if(EZ_TreeNodeSibling(node)) y1 += h2 + space;
                    XDrawLine(EZ_Display, drawable, gc, x1, y1, x2,y2);
                  }
              }
          }
        break;
	case EZ_LIST_TREE_NODE:
	  if(drawbranch && pitem)
            {
              tmp = ((int) EZ_TreeNodeIndent(parent) >> 1);
              x1 =  (int)(EZ_ItemOriginX(pitem)) + tmp;
              y1 = (int)(EZ_ItemOriginY(pitem) + EZ_ItemHeight(pitem));
              x2 = x0;
              y2 = y0 + ((int) EZ_ItemHeight(item) >> 1);
	  
              XDrawLine(EZ_Display, drawable, gc, x1,y1,x1,y2);
              XDrawLine(EZ_Display, drawable, gc, x1,y2,x2,y2);
            }
	  break;
	case EZ_TREE_NODE:
          if(drawbranch && pitem)
            {
              if(drawbranch > 1)
                {
                  XPoint points[4];
                  int spacing = EZ_TreeNodeSpacing(node) >> 1;
                  points[0].x = (EZ_ItemOriginX(pitem) + ( EZ_ItemWidth(pitem)>>1));
                  points[0].y = EZ_ItemOriginY(pitem) + EZ_ItemHeight(pitem);
                  points[1].x = points[0].x;
                  points[1].y = points[0].y + spacing;
                  points[2].x = x0 + (EZ_ItemWidth(item)>>1);
                  points[2].y = points[1].y;
                  points[3].x = points[2].x;
                  points[3].y = y0;
                  XDrawLines(EZ_Display,drawable,gc,points, 4, CoordModeOrigin);
                }
              else
                {
                  x1 = (int)(EZ_ItemOriginX(pitem) + ( EZ_ItemWidth(pitem)>>1));
                  y1 = EZ_ItemOriginY(pitem) + EZ_ItemHeight(pitem);
                  x2 = x0 + (int)(EZ_ItemWidth(item)>>1);
                  y2 = y0;
                  XDrawLine(EZ_Display, drawable, gc, x1,y1,x2,y2);		    
                }
            }
	  break;
	default: 
	  break;
	}
    }
}
/*************************************************************************/

void EZ_DisplayTreeInDrawable(widget,drawable,x0,y0, root,Rx,Ry,Rw,Rh)
     int x0, y0,Rx,Ry,Rw,Rh;
     EZ_Widget *widget;
     Drawable drawable;
     EZ_TreeNode *root;
{
  if(root)
    {
      EZ_Item *item = EZ_TreeNodeItem(root);
      unsigned long bg = EZ_ITreeWHighlightBG(widget);
      unsigned long fg = EZ_ITreeWHighlightFG(widget);
      int  hmode = EZ_ITreeWHighlightMode(widget);
      int  type = EZ_TreeNodeType(root);

      switch(type)
	{
	case EZ_TAGED_TREE_NODE:
          if(EZ_TreeNodeParent(root) == NULL) 
            x0 += (int) EZ_TreeNodeIndent(root);
          /* drop */
	case EZ_LIST_TREE_NODE:
	  {
	    EZ_TreeNode *children, *sibling;  
	    int ah, itmp;
	    int highlight = EZ_ItemHighlighted(item);
	    if(item && EZ_ITreeWRowBgSet(widget))
	      {
		unsigned long *bgs = EZ_ITreeWRowBg(widget);
		int rowp = EZ_ItemRowParity(item);
		int  ah = EZ_ItemHeight(item);
		unsigned long bgpv = bgs[rowp];
		if(bgpv)
		  {
		    GC  gc = EZ_WRITABLEGC2;
		    XSetForeground(EZ_Display, gc, bgpv);      
		    XFillRectangle(EZ_Display, drawable, gc, Rx, y0, Rw, ah);  
		  }
		else
		  {
		    GC darkgc, brightgc, normalgc;
		    int tile = EZ_GetDarkBrightNormalBDGC(widget, &darkgc, &brightgc, &normalgc);
		    if(rowp)
		      {
			if(tile)
			  {
			    int x = EZ_ITreeWXStart(widget);
			    int y = EZ_ITreeWYStart(widget);
			    XSetTSOrigin(EZ_Display, normalgc, x, y);
			  }
			XFillRectangle(EZ_Display, drawable, normalgc, Rx, y0, Rw, ah);  
		      }
		    else
		      {
			if(tile)
			  {
			    int x = EZ_ITreeWXStart(widget);
			    int y = EZ_ITreeWYStart(widget);
			    XSetTSOrigin(EZ_Display, brightgc, x, y);
			  }
			XFillRectangle(EZ_Display, drawable, brightgc, Rx, y0, Rw, ah);  
		      }
		  }
	      }
	    /* display the node first */
	    EZ_DisplayOneItem(widget, drawable, x0, y0, item, Rx,Ry,Rw,Rh);
	    if(highlight) EZ_HighlightOneItem(widget, drawable, item, fg, bg, Rx,Ry,Rw,Rh, hmode);	    

	    /* the relative h offset */
	    itmp = (item == NULL ? 0 : (EZ_ItemHeight(item)));
	    ah = (int)EZ_TreeNodeSpacing(root) + itmp;
	    children = EZ_TreeNodeChildren(root);
	    
	    if(children && EZ_TreeNodeHideSubtree(root) == 0)
	      {
		int  x, y;
                if(type == EZ_TAGED_TREE_NODE)
                  x = x0 + (((int) EZ_TreeNodeIndent(root)) >> 1);
                else
                  x = x0 + EZ_TreeNodeIndent(root);
		y = y0 + ah;
		EZ_DisplayTreeInDrawable(widget, drawable, x,y, children,Rx,Ry,Rw,Rh);
		ah += (int) EZ_TreeNodeTHeight(children);
	      }
	    sibling = EZ_TreeNodeSibling(root);  
	    if(sibling)
	      {
		int x,y;
		x = x0;
		y = y0 + ah;
		EZ_DisplayTreeInDrawable(widget, drawable,x,y, sibling,Rx,Ry,Rw,Rh);
	      }

	    /* draw line segs join root and its parent */
	    /* if(EZ_TreeNodeDrawBranch(root)) */
	      {
		EZ_TreeNode *parent = EZ_TreeNodeParent(root);
		draw_tree_branch(drawable, parent, root);
	      }
	  }
	  break;
	case EZ_TREE_NODE:  
	  {
	    EZ_TreeNode *children, *sibling;  
	    int ah, dx0, itmp;
	    int highlight = EZ_ItemHighlighted(item);
	    itmp = (item == NULL ? 0 : (EZ_ItemWidth(item)));
	    /* display the node first */
	    dx0 = (int)(EZ_TreeNodeTWidth(root) - itmp) >> 1; 
	    EZ_DisplayOneItem(widget, drawable, x0+dx0, y0, item, Rx,Ry,Rw,Rh);
	    if(highlight) EZ_HighlightOneItem(widget, drawable, item, fg, bg, Rx,Ry,Rw,Rh, hmode);	    
	    /* draw line segs join root and its parent */
	    /* if(EZ_TreeNodeDrawBranch(root)) */
	      {
		EZ_TreeNode *parent = EZ_TreeNodeParent(root);
		if(parent) draw_tree_branch(drawable, parent, root);
	      }
      
	    /* the relative h v offset */
	    itmp = (item == NULL ? 0 : (EZ_ItemHeight(item)));
	    ah = (int)EZ_TreeNodeSpacing(root) + itmp;
	    children = EZ_TreeNodeChildren(root);
	    if(children)
	      {
		int  x, y, aw;
		x = x0;
		y = y0 + ah;
		EZ_DisplayTreeInDrawable(widget, drawable, x,y, children,Rx,Ry,Rw,Rh);
		aw = (int) (EZ_TreeNodeTWidth(children));
		
		sibling = EZ_TreeNodeSibling(children);  
		while(sibling)
		  {
		    aw += (int)EZ_TreeNodeIndent(sibling);
		    x = x0 + aw;
		    EZ_DisplayTreeInDrawable(widget, drawable,x,y, sibling,Rx,Ry,Rw,Rh);
		    aw += (int) (EZ_TreeNodeTWidth(sibling));
		    sibling = EZ_TreeNodeSibling(sibling);  
		  }
	      }
	  }
	  break;
	default:
	  break;
	}
    }
}  

/********************************************************************************************
 *
 *           Supporting functions
 *
 ********************************************************************************************/
/*
 *  Set the basic property of a tree:  type is one of EZ_LIST_TREE_NODE and EZ_TREE_NODE
 *  indent and spacing:  for LIST_TREE, spacing is vspacing, indent is h indent
 *  for TREE, spacing is h spacing and indent is vspacing.
 */
void EZ_TreeSetCharacter(tree,type,indent,spacing,draw_branch)
     int indent, spacing, type, draw_branch;
     EZ_TreeNode *tree;
{
  if(tree)
    {
      if(type < EZ_TREE_NODE || type > EZ_TAGED_TREE_NODE) 
	type = EZ_LIST_TREE_NODE;
      EZ_SetTreeCharacterWork(tree, type, indent,spacing, draw_branch);
      {
	EZ_Item *item = EZ_TreeNodeItem(tree);
	if(item)
	  {
	    EZ_Widget *widget = EZ_ItemParentWidget(item);
	    if(widget && EZ_LookupWidgetFromAllHT(widget) == widget &&
	       EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
	  }
      }
    }
}

static void EZ_SetTreeCharacterWork(root,type,indent,spacing, draw_branch)
     int indent, spacing,type,draw_branch;
     EZ_TreeNode *root;
{
  if(root)
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(root);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(root);
      EZ_Item *item = EZ_TreeNodeItem(root);

      EZ_TreeNodeType(root) = type;
      EZ_TreeNodeDrawBranch(root) = draw_branch;
      EZ_TreeNodeIndent(root) =  indent;
      EZ_TreeNodeSpacing(root) = spacing;
      
      if(item) EZ_ItemDirty(item) = 1;
      if(children) EZ_SetTreeCharacterWork(children,type,indent, spacing,draw_branch);
      if(sibling)  EZ_SetTreeCharacterWork(sibling,type,indent, spacing,draw_branch);
    }
}      

/*****************************************************************
 *
 * attach a subtree to node.
 */
static int loop_in_tree MY_ANSIARGS((EZ_TreeNode *root, EZ_TreeNode *node));

void EZ_TreeAttachSubtree(node, subtree)
     EZ_TreeNode *node, *subtree;
{
  if(node && subtree)
    {
      /* check to see if there is a loop */
      {
	EZ_TreeNode *parent = node, *t = EZ_TreeNodeParent(parent);
	while(t) { parent = t; t = EZ_TreeNodeParent(parent);}
	if(loop_in_tree(parent, subtree)) 
	  {
	    (void)fprintf(stderr,
			  "EZ_AttachSubtree: subtree is already a branch of the specified tree\n");
	    return;
	  }
      }
      {
	EZ_TreeNode *t, *children = EZ_TreeNodeChildren(node);
	/* make sure subtree is not a branch of other trees */
	EZ_TreeDetachSubtree(subtree);
	if(children) 
	  {
	    t = EZ_TreeNodeSibling(children);
	    while(t)
	      {
		children = t;
		t = EZ_TreeNodeSibling(children);
	      }
	    EZ_TreeNodeSibling(children) = subtree;
	  }
	else EZ_TreeNodeChildren(node) = subtree;

	EZ_TreeNodeParent(subtree) = node;
      }
    }
}
/* 
 * return 1 if node is reachable from root 
 * root should be a descendant of node.
 */
static int loop_in_tree(root, node) 
     EZ_TreeNode *root, *node;
{
  if(root && node)
    {
      EZ_TreeNode *children, *sibling;
      if(root == node) return(1);
      children = EZ_TreeNodeChildren(root);
      sibling = EZ_TreeNodeSibling(root);
      if(loop_in_tree(children, node)) return(1);
      if(loop_in_tree(sibling, node)) return(1);      
    }
  return(0);
}
/*****************************************************************
 * detach a node (subtree) from a tree
 */
void EZ_TreeDetachSubtree(subtree)
     EZ_TreeNode *subtree;
{
  if(subtree)
    {
      EZ_TreeNode *root = subtree;
      EZ_TreeNode *parent = EZ_TreeNodeParent(root);

      if(parent)
	{
	  EZ_TreeNode *t, *c = EZ_TreeNodeChildren(parent);
	  if(c == root) 
	    {
	      EZ_TreeNodeChildren(parent) = EZ_TreeNodeSibling(c);
	      EZ_TreeNodeSibling(c) = (EZ_TreeNode *)NULL;
	    }
	  else
	    {
	      t = EZ_TreeNodeSibling(c);
	      while(t && t != root)
		{
		  c = t;
		  t = EZ_TreeNodeSibling(c);
		}
	      if(t)
		{
		  EZ_TreeNodeSibling(c) = EZ_TreeNodeSibling(t);
		  EZ_TreeNodeSibling(t) = (EZ_TreeNode *)NULL;
		}
	    }
	  EZ_TreeNodeParent(root) = (EZ_TreeNode *)NULL;
	}  
    }
} 

/*****************************************************************
 *
 * Destroy a subtree. Remove it from its parent's children list.
 */
void EZ_TreeDestroySubtree(root)
     EZ_TreeNode *root;
{
  if(root)
    {
      /* detach root from its parent */
      EZ_TreeDetachSubtree(root);
      /* only destroy the subtree from root, do not destroy the siblings of root */
      EZ_DestroyTreeWork(root, 0); 
    }  
}
/***************************************************************/
void EZ_TreeDestroyNodeDescendants(node)
     EZ_TreeNode *node;
{
  if(node)
    {
      EZ_Item *item = EZ_TreeNodeItem(node);
      EZ_TreeNode *children = EZ_TreeNodeChildren(node);
      EZ_TreeNodeChildren(node) = (EZ_TreeNode *)NULL;
      if(item) EZ_ItemDirty(item) = 1;
      EZ_DestroyTreeWork(children, 1);
    }
}

static void EZ_DestroyTreeWork(root,level)
     EZ_TreeNode *root; int level;
{
  if(root)
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(root);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(root);
      EZ_Item  *item = EZ_TreeNodeItem(root);

      (void)my_free((char *)root);
      EZ_DestroyItem(item);

      if(children) EZ_DestroyTreeWork(children, 1);
      if(level > 0 && sibling)  EZ_DestroyTreeWork(sibling,1);
    }
}
/***************************************************************
 *
 * Traverse a tree and exec func on all nodes. func takes only
 * one arg, a pointer to a node.
 */
void EZ_TreeTraverseTree(root, pre_func, post_func)
     EZ_TreeNode *root;
     void (*pre_func) MY_ANSIARGS((EZ_TreeNode *node)); 
     void (*post_func) MY_ANSIARGS((EZ_TreeNode *node));
{
  EZ_TreeTraverseTreeWork(root, pre_func, post_func, 0);
}

static void EZ_TreeTraverseTreeWork(root, pre_func, post_func, level)
     EZ_TreeNode *root; int level;
     void (*pre_func) MY_ANSIARGS((EZ_TreeNode *node)); 
     void (*post_func) MY_ANSIARGS((EZ_TreeNode *node));
{
  if(root && (pre_func || post_func))
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(root);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(root);
      EZ_Item  *item;

      if(pre_func) pre_func(root);
      if(children) EZ_TreeTraverseTreeWork(children, pre_func,post_func,1);
      if(sibling && level)  EZ_TreeTraverseTreeWork(sibling, pre_func,post_func,1);
      if(post_func) post_func(root);
      item = EZ_TreeNodeItem(root);
      if(item) EZ_ItemDirty(item) = 1;
    }  
}



void EZ_TreeSetBGColor(root, pv)
     EZ_TreeNode *root; unsigned long pv;
{
  EZ_TreeSetBGColorWork(root, pv, 0);
}

void EZ_TreeSetFGColor(root, pv)
     EZ_TreeNode *root; unsigned long pv;
{
  EZ_TreeSetFGColorWork(root, pv, 0);
}

void EZ_TreeSetBGColorName(root, clr)
     EZ_TreeNode *root; char *clr;
{
  unsigned long pv = EZ_AllocateColorFromName(clr);
  EZ_TreeSetBGColorWork(root, pv, 0);
}

void EZ_TreeSetFGColorName(root, clr)
     EZ_TreeNode *root; char *clr;
{
  unsigned long pv = EZ_AllocateColorFromName(clr);
  EZ_TreeSetFGColorWork(root, pv, 0);
}

/** the work */
static void EZ_TreeSetBGColorWork(root, pv, level)
     EZ_TreeNode *root; unsigned long pv; int level;
{
  if(root)
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(root);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(root);
      EZ_Item *item = EZ_TreeNodeItem(root);
      
      if(item) EZ_ItemBackground(item) = pv; 
      EZ_TreeSetBGColorWork(children, pv, 1);
      if(level) EZ_TreeSetBGColorWork(sibling, pv, 1);
    }
}

static void EZ_TreeSetFGColorWork(root, pv, level)
     EZ_TreeNode *root; unsigned long pv;  int level;
{
  if(root)
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(root);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(root);
      EZ_Item *item = EZ_TreeNodeItem(root);

      if(item) EZ_ItemForeground(item) = pv;
      EZ_TreeSetFGColorWork(children, pv, 1);
      if(level) EZ_TreeSetFGColorWork(sibling, pv, 1);
    }
}


/**************************************************************/
  
EZ_Item *EZ_TreeNodeGetItem(node) 
     EZ_TreeNode *node;
{
  if(node) return(EZ_TreeNodeItem(node));
  else  return(NULL);
}
/**************************************************************/      
EZ_TreeNode  *EZ_TreeNodeGetChildren(node)
     EZ_TreeNode *node;
{
  if(node) return(EZ_TreeNodeChildren(node));
  return(NULL);
}

EZ_TreeNode  *EZ_TreeNodeGetSibling(node)
     EZ_TreeNode *node;
{
  if(node) return(EZ_TreeNodeSibling(node));
  return(NULL);
}
EZ_TreeNode  *EZ_TreeNodeGetParent(node)
     EZ_TreeNode *node;
{
  if(node) return(EZ_TreeNodeParent(node));
  return(NULL);
}

EZ_TreeNode *EZ_TreeFindNode(root, node)
     EZ_TreeNode *root, *node;
{
  if(node == NULL) return(NULL);
  if(root == node) return(node);
  if(root)
    {
      EZ_TreeNode *children = EZ_TreeNodeChildren(root);
      EZ_TreeNode *sibling = EZ_TreeNodeSibling(root);

      if( EZ_TreeFindNode(children, node) != NULL) return(node);
      if( EZ_TreeFindNode(sibling, node)  != NULL) return(node);      
    }
  return(NULL);
}


EZ_TreeNode *EZ_TreeFindNodeUsingItem(root, item)
     EZ_TreeNode *root; EZ_Item *item;
{
  if(item == NULL) return(NULL);
  if(root)
    {
      if(EZ_TreeNodeItem(root) == item) return(root);
      else
	{
	  EZ_TreeNode *children = EZ_TreeNodeChildren(root);
	  EZ_TreeNode *sibling = EZ_TreeNodeSibling(root);
	  EZ_TreeNode *tmp;

	  tmp =  EZ_TreeFindNodeUsingItem(children, item);
	  if( tmp != NULL) return(tmp);
	  tmp = EZ_TreeFindNodeUsingItem(sibling, item);
	  if(tmp  != NULL) return(tmp);      
	}
    }
  return(NULL);
}
/**************************************************************/      
EZ_TreeNode  *EZ_TreeFindNodeUsingProcedure(root, f,data)
     EZ_TreeNode *root;
     EZ_TreeNode *(*f) MY_ANSIARGS((EZ_TreeNode *node, void *dt));
     void *data;
{
  if(root)
    {
      EZ_TreeNode *ans = f(root, data);
      if(ans) return(ans);
      else
	{
	  EZ_TreeNode *children = EZ_TreeNodeChildren(root);
	  ans = EZ_TreeFindNodeUsingProcedure(children,f,data);
	  if(ans) return(ans);
	  {
	    EZ_TreeNode *sibling = EZ_TreeNodeSibling(root);
	    ans = EZ_TreeFindNodeUsingProcedure(sibling,f,data);
	    return(ans);
	  }
	}
    }
  return(NULL);  
}
/**************************************************************/      
void EZ_TreeNodeMarkTerminal(node, flag)   EZ_TreeNode *node; int flag;
{ if(node) EZ_TreeNodeOpenable(node) = (flag==0);}
/**************************************************************/      
static void HideShowWidgetItems(node, flag)
     EZ_TreeNode *node; int flag;
{
  if(node)
    {
      EZ_Item *item = EZ_TreeNodeItem(node);
      if(item && EZ_ItemType(item) == EZ_WIDGET_ITEM)
        {
          EZ_Widget *widget =EZ_WidgetItemWidget(item);
          if(flag) EZ_DisplayWidget(widget);
          else EZ_HideWidget(widget);
        }
      HideShowWidgetItems(EZ_TreeNodeChildren(node), flag);
      HideShowWidgetItems(EZ_TreeNodeSibling(node), flag);
    }
}
/**************************************************************/      
void EZ_TreeHideSubtree(subtree)
     EZ_TreeNode *subtree;
{
  if(subtree)
    {
      EZ_Item *item = EZ_TreeNodeItem(subtree);
      if(item && EZ_ItemIsDirItem(item) != 0) /* dir tree, too bad */
	{
	  EZ_DirTreeNodeInfo *ninfo = EZ_ItemDirItemInfo(item);
	  if(ninfo)
	    {
	      int mode = ninfo->mode;
	      if(mode != EZ_DIR_NODE_OPEN_DIRECTORY_NONE)
                {
                  int collapse = (mode == EZ_DIR_NODE_OPEN_DIRECTORY_COLLAPSING);
                  EZ_OpenCloseDirTreeDirectoryNode(item,collapse);
                }
	    }
	}
      else
        {
          EZ_TreeNode *children = EZ_TreeNodeChildren(subtree);
          EZ_TreeNodeHideSubtree(subtree) = 1;
          HideShowWidgetItems(children, 0);
        }
    }
}

void EZ_TreeShowSubtree(subtree)
     EZ_TreeNode *subtree;
{
  if(subtree)
    {
      EZ_Item *item = EZ_TreeNodeItem(subtree);
      if(item && EZ_ItemIsDirItem(item) != 0)  /* dir tree, ... */
	{
	  EZ_DirTreeNodeInfo *ninfo = EZ_ItemDirItemInfo(item);
	  if(ninfo)
	    {
	      int mode = ninfo->mode;
	      if(mode != EZ_DIR_NODE_OPEN_DIRECTORY_NONE)
                {
                  int collapse = (mode == EZ_DIR_NODE_OPEN_DIRECTORY_COLLAPSING);
                  EZ_OpenCloseDirTreeDirectoryNode(item,collapse);
                }
	    }
	}
      else
        {
          EZ_TreeNode *children = EZ_TreeNodeChildren(subtree);
          EZ_TreeNodeHideSubtree(subtree) = 0;
          HideShowWidgetItems(children, 1);
        }
    }
}
/**************************************************************/      

#undef _EZ_TREE_C_
