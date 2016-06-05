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

/**********************************************************************
 *
 *  The type of items
 */
#define EZ_UNKOWN_ITEM                0
#define EZ_ITEM_LABEL                 1
#define EZ_ITEM_WIDGET                2
#define EZ_ITEM_FIG                   3
#define EZ_UDF_ITEM                   4
#define EZ_LABEL_ITEM                 EZ_ITEM_LABEL
#define EZ_WIDGET_ITEM                EZ_ITEM_WIDGET
#define EZ_FIG_ITEM                   EZ_ITEM_FIG

#define EZ_TOTAL_NUMBER_OF_ITEMS      10

#ifdef _EZ_ITEM_C_
/*****************************************************************
 *
 *  Widget Names. if reordered, make sure the types above is also
 *  reordered. 
 */
char *EZ_ItemDefaultClassNames[] =
{
  "Unknown",
  "LabelItem",
  "WidgetItem",
  "FigItem",
  "UDFItem",
  "I5",
  "I6",
  "I7",
  "I8",
  "I9",
  "I10",
  NULL,
};
#else
extern char *EZ_ItemDefaultClassNames[];
#endif

/*******************************************************************************/
#define EZ_SetItemTypeAndNames(iptr, type_) \
{ \
   ((iptr)->common.type) = type_; \
   ((iptr)->common.instanceName) = EZ_ItemDefaultQInstanceNames[type_];\
   ((iptr)->common.className) = EZ_ItemDefaultQClassNames[type_];\
}
/*******************************************************************************/
#define EZ_TREE_NODE                                0
#define EZ_LIST_TREE_NODE                           1
#define EZ_BRANCHED_LIST_TREE_NODE                  2
#define EZ_TAGED_TREE_NODE                          EZ_BRANCHED_LIST_TREE_NODE
#define EZ_TREE                                     EZ_TREE_NODE
#define EZ_LIST_TREE                                EZ_LIST_TREE_NODE
#define EZ_BRANCHED_LIST_TREE                       EZ_BRANCHED_LIST_TREE_NODE  
/*******************************************************************************/
#define EZ_TREE_NODE_INDENTATION                    20
#define EZ_TREE_NODE_VSPACING                       2
/*******************************************************************************/
#define EZ_DIR_NODE_OPEN_DIRECTORY_NONE             -1
#define EZ_DIR_NODE_OPEN_DIRECTORY_DEFAULT          0
#define EZ_DIR_NODE_OPEN_DIRECTORY_COLLAPSING       1

/*******************************************************************************/
#define EZ_DIR_NODE_OPEN_DIRECTORY_BEGIN  EZ_DIR_NODE_OPEN_DIRECTORY_NONE
#define EZ_DIR_NODE_OPEN_DIRECTORY_END    EZ_DIR_NODE_OPEN_DIRECTORY_COLLAPSING

/******************************************************************************
 *
 * 4/16/98: completely overhaul the implementation of DirTrees. The following
 *          structure is attached to each dirTreeNode->item. The golbal var
 *          DirTreeFileNodeCreator/DirTreeDirNodeCreator are removed.
 *          We still need a privite pointer in the item structure.
 */
typedef struct EZ_DirTreeNodeInfo_ {
  char                    *id;         /* used to identify the node=file_name */
  char                    *pattern;    /* used in globing children nodes      */
  EZ_DirTreeNodeCreator   creator;     /* node creator */
  void                    *cdata;      /* private data to be passed to creator*/
  void                    *node;       /* the node this item is attached to   */
  void                    *sorter;
  int                      mode;        /* directory openning mode             */
} EZ_DirTreeNodeInfo;

/**********************************************************************
 *                                             
 *  All items contain this part
 */
typedef struct EZ_ItemCommon_ {
  char                 type;        /* type of item */
  char                 dirty;       /* size may be changed */ 
  char                 visible;     /* item is visible?    */
  char                 anchor;      /* anchor position inside its assigned rect */

  char                 borderType;
  char                 highlighted;
  char                 rowParity;    
  char                 isdir;        /* is a directory node ? */

  int                  borderWidth;
  int                  padX;
  int                  padY;

  int                  x,y;          /* UL corner in its container */
  int                  w,h;          /* minimal dimension    */
  int                  ax,ay;        /* assigned origin by the geom manager   */
  int                  aw, ah;       /* assigned width, height */

  int                  blocked;      /* callback in action */
  
  XFontStruct          *font;            /* the font */
  EZ_Bitmap            *pixmap;          /* pixmap       */ 
  unsigned long        foreground;       /* fg pixel value */
  unsigned long        background;       /* bg pixel value */

  union EZ_WWindow_    *parentWidget;    /* widget it belongs to: used to set BG color */
  EZ_CallBackStruct    *callbacks;       /* callbacks: called on double click */
  EZ_CallBackStruct    *destroyCallback; /* callback on destruction */

  EZ_DirTreeNodeInfo   *dirTreeNodeInfo; /* used in dirTrees, malloced */

  /************ DnD data *************************/
  Cursor            DnDDragCursor;            /* cursor to use if it is a drag src  */
  EZ_DnDDataEncoder *DnDDataEncoders;         /* list of data converters, encoding */
  EZ_DnDDataDecoder *DnDDataDecoders;         /* list of data converters, deoding */
  char              *DnDTargetHelp;           /* help message on this drop target*/

  XrmQuark          className;              /* class name */
  XrmQuark          instanceName;           /* instance name */
  
  int                  idata;            /* application data */
  void                 *ptrdata;         /* application data */
  int                  npvalues;         /* dynamically allocated storage */
  EZ_UnknownDataType   *pvalues;         /* for more general client data  */

  char                 *resourceString;
} EZ_ItemCommon;

#define EZ_ItemResourceString(item)   ((item)->common.resourceString)
#define EZ_ItemClassName(item)        ((item)->common.className)
#define EZ_ItemInstanceName(item)     ((item)->common.instanceName)
#define EZ_ItemCName(item)            ((item)->common.className)
#define EZ_ItemIName(item)            ((item)->common.instanceName)

#define EZ_ItemDirItemInfo(item)      ((item)->common.dirTreeNodeInfo)
#define EZ_ItemIsDirItem(item)        ((item)->common.isdir)
#define EZ_ItemBlocked(item)          ((item)->common.blocked)

#define EZ_ItemHighlighted(item)      ((item)->common.highlighted)
#define EZ_ItemRowParity(item)        ((item)->common.rowParity)

#define EZ_ItemDnDDragCursor(item)    ((item)->common.DnDDragCursor)
#define EZ_ItemDnDDataEncoders(item)  ((item)->common.DnDDataEncoders)
#define EZ_ItemDnDDataDecoders(item)  ((item)->common.DnDDataDecoders)
#define EZ_ItemDnDDataEncoder(item)   ((item)->common.DnDDataEncoders)
#define EZ_ItemDnDDataDecoder(item)   ((item)->common.DnDDataDecoders)
#define EZ_ItemDnDTargetHelp(item)    ((item)->common.DnDTargetHelp)

#define EZ_ItemIsDnDSrc(item)\
(((item)->common.DnDDataEncoders) && EZ_ItemType(item) != EZ_WIDGET_ITEM)
#define EZ_ItemIsDnDTarget(item)\
(((item)->common.DnDDataDecoders) && EZ_ItemType(item) != EZ_WIDGET_ITEM)

#define EZ_ItemType(item)             ((item)->common.type)
#define EZ_ItemAnchor(item)           ((item)->common.anchor)
#define EZ_ItemDirty(item)            ((item)->common.dirty)
#define EZ_ItemVisible(item)          ((item)->common.visible)
#define EZ_ItemParentWidget(item)     ((item)->common.parentWidget)
#define EZ_ItemBorderType(item)       ((item)->common.borderType)
#define EZ_ItemBorderWidth(item)      ((item)->common.borderWidth)
#define EZ_ItemPadX(item)             ((item)->common.padX)
#define EZ_ItemPadY(item)             ((item)->common.padY)
#define EZ_ItemBitmap(item)           ((item)->common.pixmap)
#define EZ_ItemPixmap(item)           ((item)->common.pixmap)
#define EZ_ItemForeground(item)       ((item)->common.foreground)
#define EZ_ItemBackground(item)       ((item)->common.background)
#define EZ_ItemFont(item)             ((item)->common.font)
#define EZ_ItemCallback(item)         ((item)->common.callbacks)
#define EZ_ItemCallbacks(item)        ((item)->common.callbacks)
#define EZ_ItemDestroyCallback(item)  ((item)->common.destroyCallback)
#define EZ_ItemDestroyCallbacks(item) ((item)->common.destroyCallback)
#define EZ_ItemOriginX(item)          ((item)->common.x)
#define EZ_ItemOriginY(item)          ((item)->common.y)
#define EZ_ItemAOriginX(item)         ((item)->common.ax)
#define EZ_ItemAOriginY(item)         ((item)->common.ay)
#define EZ_ItemWidth(item)            ((item)->common.w)
#define EZ_ItemHeight(item)           ((item)->common.h)
#define EZ_ItemAWidth(item)           ((item)->common.aw)
#define EZ_ItemAHeight(item)          ((item)->common.ah)
#define EZ_ItemIntData(item)          ((item)->common.idata)
#define EZ_ItemPtrData(item)          ((item)->common.ptrdata)

#define EZ_ItemNPValues(item)         ((item)->common.npvalues)
#define EZ_ItemPValues(item)          ((item)->common.pvalues)

/**********************************************************************/
#define EZ_HandleItemCallBacks(_the_item_) \
{\
  if((_the_item_) && EZ_LookupItemFromHT(_the_item_))\
    {\
      EZ_CallBackStruct *_the_callback_list = EZ_ItemCallbacks((_the_item_));\
      EZ_ItemBlocked(_the_item_) += 1;\
      while(_the_callback_list) \
	{\
           if(_the_callback_list->callback)\
               (_the_callback_list->callback)(_the_item_, _the_callback_list->data);\
           _the_callback_list = _the_callback_list->next; \
	}\
      EZ_ItemBlocked(_the_item_) -= 1;\
    }\
 }
/**********************************************************************
 *                                             
 * A label item is the most common type of item.
 */
typedef struct EZ_LabelItem_ {
  char      common[sizeof(EZ_ItemCommon)];

  char                 *string;     /* text         */

  char                 orientation; /* how to arrange pixmap, string */
  char                 labjust;      /* label justification   */
  char                 shadow;      /* textual label shadow   */
  char                 c2;

  int                   length;     /* length of string */
  int                   llength;     /* line length      */ 
  int                   nlines;      /* n lines          */ 
  int                   pading;      /* spacing between pixmap and string */
  int                   swidth, sheight; /* string width and height */

} EZ_LabelItem;

#define EZ_LabelItemType(item)             ((item)->common.type)
#define EZ_LabelItemDirty(item)            ((item)->common.dirty)
#define EZ_LabelItemVisible(item)          ((item)->common.visible)
#define EZ_LabelItemParentWidget(item)     ((item)->common.parentWidget)
#define EZ_LabelItemBorderType(item)       ((item)->common.borderType)
#define EZ_LabelItemBorderWidth(item)      ((item)->common.borderWidth)
#define EZ_LabelItemPadX(item)             ((item)->common.padX)
#define EZ_LabelItemPadY(item)             ((item)->common.padY)
#define EZ_LabelItemBitmap(item)           ((item)->common.pixmap)
#define EZ_LabelItemPixmap(item)           ((item)->common.pixmap)
#define EZ_LabelItemForeground(item)       ((item)->common.foreground)
#define EZ_LabelItemBackground(item)       ((item)->common.background)
#define EZ_LabelItemFont(item)             ((item)->common.font)
#define EZ_LabelItemCallback(item)         ((item)->common.callbacks)
#define EZ_LabelItemCallbacks(item)        ((item)->common.callbacks)
#define EZ_LabelItemDestroyCallback(item)  ((item)->common.destroyCallback)
#define EZ_LabelItemOriginX(item)          ((item)->common.x)
#define EZ_LabelItemOriginY(item)          ((item)->common.y)
#define EZ_LabelItemWidth(item)            ((item)->common.w)
#define EZ_LabelItemHeight(item)           ((item)->common.h)
#define EZ_LabelItemAWidth(item)           ((item)->common.aw)
#define EZ_LabelItemAHeight(item)          ((item)->common.ah)
#define EZ_LabelItemIntData(item)          ((item)->common.idata)
#define EZ_LabelItemPtrData(item)          ((item)->common.ptrdata)
#define EZ_LabelItemID(item)               ((item)->common.id)
#define EZ_LabelItemLabelPosition(item)    ((item)->common.anchor)
#define EZ_LabelItemOrientation(item)      ((item)->label.orientation)
#define EZ_LabelItemString(item)           ((item)->label.string)
#define EZ_LabelItemStringLength(item)     ((item)->label.length)
#define EZ_LabelItemStringLineLength(item) ((item)->label.llength)
#define EZ_LabelItemNLines(item)           ((item)->label.nlines)
#define EZ_LabelItemStringWidth(item)      ((item)->label.swidth)
#define EZ_LabelItemStringHeight(item)     ((item)->label.sheight)
#define EZ_LabelItemJustification(item)    ((item)->label.labjust)
#define EZ_LabelItemShadow(item)           ((item)->label.shadow)
#define EZ_LabelItemPad(item)              ((item)->label.pading)

/**********************************************************************
 *
 * widget item,
 */
typedef struct EZ_WidgetItem_ {
  char      common[sizeof(EZ_ItemCommon)];  

  Window             dummyWin;      /* dummy window for communication   */
  union EZ_WWindow_  *widget;
} EZ_WidgetItem;


#define EZ_WidgetItemDummyWin(item)      ((item)->widget.dummyWin)
#define EZ_WidgetItemWidget(item)        ((item)->widget.widget)

/**********************************************************************
 *
 * Entry in the action list in FIG items.
 */
typedef struct EZ_FigItemPiece_ {
  int               figType;    /* FIG_LINE, FIG_POLYGON, FIG_ARC, etc */
  GC                gc;         /* gc used to draw this piece */
  XFontStruct       *xfont;    
  int               lineWidth;
  int               x,y,width,height;
  int               npoints, i1, i2;
  XPoint            *data; 
  int               x1,y1,x2,y2; /* bounding box of this piece */
  char              clean;
  char              hidden;
  char              shadow;      /* shadowed text */
  char              cpad;
  struct EZ_FigItemPiece_ *next;
} EZ_FigPiece;

#define  EZ_FigPieceTShadow(ap)       ((ap)->shadow)
#define  EZ_FigPieceXFont(ap)         ((ap)->xfont)
#define  EZ_FigPieceLineWidth(ap)     ((ap)->lineWidth)
#define  EZ_FigPieceClean(ap)         ((ap)->clean)
#define  EZ_FigPieceHidden(ap)        ((ap)->hidden)
#define  EZ_FigPieceType(ap)          ((ap)->figType)
#define  EZ_FigPieceGC(ap)            ((ap)->gc)
#define  EZ_FigPieceX(ap)             ((ap)->x)
#define  EZ_FigPieceY(ap)             ((ap)->y)
#define  EZ_FigPieceWidth(ap)         ((ap)->width)
#define  EZ_FigPieceHeight(ap)        ((ap)->height)
#define  EZ_FigPieceAngle1(ap)        ((ap)->i1)
#define  EZ_FigPieceAngle2(ap)        ((ap)->i2)
#define  EZ_FigPieceData(ap)          ((ap)->data)
#define  EZ_FigPieceNPoints(ap)       ((ap)->npoints)

#define  EZ_FigPieceStrLength(ap)         ((ap)->npoints)
#define  EZ_FigPieceStrJustification(ap)  ((ap)->i1)
#define  EZ_FigPieceStrNLines(ap)         ((ap)->i2)
#define  EZ_FigPieceStrWidth(ap)          ((ap)->width)
#define  EZ_FigPieceStrHeight(ap)         ((ap)->height)

#define  EZ_FigPiecePixmapX(ap)         ((ap)->i1)
#define  EZ_FigPiecePixmapY(ap)         ((ap)->i2)
#define  EZ_FigPiecePixmapW(ap)         ((ap)->width)
#define  EZ_FigPiecePixmapH(ap)         ((ap)->height)
/**********************************************************************/
#define  EZ_FIG_POINTS            1
#define  EZ_FIG_POLYLINE          2
#define  EZ_FIG_POLYGON           3
#define  EZ_FIG_RECTANGLE         4
#define  EZ_FIG_FILLED_RECTANGLE  5
#define  EZ_FIG_ARC               6
#define  EZ_FIG_FILLED_ARC        7
#define  EZ_FIG_RECTANGLES        8
#define  EZ_FIG_FILLED_RECTANGLES 9
#define  EZ_FIG_ARCS              10
#define  EZ_FIG_FILLED_ARCS       11
#define  EZ_FIG_STRING            12
#define  EZ_FIG_SEGMENTS          13
#define  EZ_FIG_PIXMAP            14

/**********************************************************************
 *
 * geometric figures (composed of points, lines, polygons, etc)
 *
 */
typedef struct EZ_SSChartJunk_ {
  EZ_SChartHandle   chandle; 
  void              *data;
  int                kind;
  short              r1, c1, r2, c2;  /* ssheet window */
} EZ_SSChartJunk;
  
typedef struct EZ_FigItem_ {
  char              common[sizeof(EZ_ItemCommon)];  
  int               xoff,yoff;       /* accumulated UL offset  */
  EZ_FigPiece       *partList;
  float             sx, sy;          /* accumulated scaling factors */
  /* the following fields are used in sheet charts */
  EZ_SSChartJunk    *sschartJunk;
  int               freezed;
} EZ_FigItem;

#define  EZ_FigItemPartList(item)          ((item)->fig.partList)
#define  EZ_FigItemXOffset(item)           ((item)->fig.xoff)
#define  EZ_FigItemYOffset(item)           ((item)->fig.yoff)
#define  EZ_FigItemSx(item)                ((item)->fig.sx)
#define  EZ_FigItemSy(item)                ((item)->fig.sy)
#define  EZ_FigItemSSChartJunk(item)       ((item)->fig.sschartJunk)
#define  EZ_FigItemFreezed(item)           ((item)->fig.freezed)

#define  EZ_FigItemR1(item)                (((item)->fig.sschartJunk)->r1)
#define  EZ_FigItemC1(item)                (((item)->fig.sschartJunk)->c1)
#define  EZ_FigItemR2(item)                (((item)->fig.sschartJunk)->r2)
#define  EZ_FigItemC2(item)                (((item)->fig.sschartJunk)->c2)
#define  EZ_FigItemSCHandle(item)          (((item)->fig.sschartJunk)->chandle)
#define  EZ_FigItemSKind(item)             (((item)->fig.sschartJunk)->kind)
#define  EZ_FigItemSCData(item)            (((item)->fig.sschartJunk)->data)


/**********************************************************************
 *
 * User defined items.
 */
typedef struct EZ_UDFItem_ {
  char               common[sizeof(EZ_ItemCommon)];  
  void               *udfHandle;
  EZ_UnknownDataType cdata[7];
} EZ_UDFItem;

#define EZ_UDFItemData(item)   ((item)->udf.udfHandle)
#define EZ_UDFItemCData(item)  ((item)->udf.cdata)
/**********************************************************************
 *                                             
 *  Item union
 */
union EZ_ItemUnion {
  EZ_ItemCommon  common;
  EZ_LabelItem   label;
  EZ_WidgetItem  widget;
  EZ_FigItem     fig;
  EZ_UDFItem     udf;
};

/*****************************************************************
 *
 * Record of item handling functions: 
 *  void      (*computeSize)(EZ_Item *item, int *width_ret, int *height_ret); 
 *    compute the dimesion of item (width and height).  
 *
 *  void      (*draw)(EZ_Widget *widget, Drawable drawable, int x0, int y0, 
 *                       EZ_Item *item, int Rx, int Ry, int Rw,int Rh)    
 *    draw item in a drawable with a given position. 
 *
 *  void      (*update)(EZ_Widget *widget, Drawable drawable, EZ_Item *item,
 *                     int Rx, int Ry, int Rw,int Rh)    
 *     refresh one item.
 *
 *  void      (*highlight)(EZ_Widget *widget, Drawable drawable, EZ_Item *item,
 *                         unsigned long fg, unsigned long bg,
 *                     int Rx, int Ry, int Rw,int Rh, int flag)    
 *  void      (*freePrivateData)(EZ_Item *item)
 *  
 *  int       (*pick)(EZ_Item *item, int x, int y)
 *               decide whether or not (x,y) is over item.
 *            
 *  void      (*postscript)(EZ_Item *item, File *fp)
 */


typedef struct EZ_DpyItemHandle_ {
  /* compute geometry  */
  void      (*computeSize) MY_ANSIARGS((EZ_Item *item, int *w, int *h)); 
  /* draw it in a given drawable and position */
  void      (*draw) MY_ANSIARGS((EZ_Widget  *widget,
				 Drawable  drawable,
				 int       x0, int y0,
				 EZ_Item   *item,
				 int       Rx, int Ry, int Rw, int Rh));
  /* update */  
  void      (*update) MY_ANSIARGS((EZ_Widget *widget,
				   Drawable  drawable,
				   EZ_Item   *item,
				   int       Rx, int Ry, int Rw, int Rh));
 /* update when selected */
  void      (*highlight) MY_ANSIARGS((EZ_Widget  *widget,
				      Drawable drawable,
				      EZ_Item  *item,
				      unsigned long fg, unsigned long bg,
				      int       Rx, int Ry, int Rw, int Rh, int flag));       
  /* how to free private data */
  void      (*freePrivateData)  MY_ANSIARGS((EZ_Item *item));
  /* how to decide the pointer is on an item */
  int       (*pick)  MY_ANSIARGS((EZ_Item *item, int x, int y));
  /* how to generate postscript */
  void      (*postscript)  MY_ANSIARGS((EZ_Item *item, FILE *fp, void *psinfo));
} EZ_DpyItemHandle;

/*****************************************************************/
#ifndef _EZ_ITEM_C_
extern EZ_DpyItemHandle   *EZ_DpyItemHandlers[];
#endif

/*****************************************************************
 *
 *  Tree Node. Not an item.
 *
 ****************************************************************/
typedef struct EZ_TreeNode_ {
  EZ_Item               *item;
  char                   type;
  char                   drawbranch;
  char                   hideSubtree;
  char                   openable; 

  int                    width;
  int                    height;
  int                    indent1;
  int                    indent2;

  struct EZ_TreeNode_   *parent;
  struct EZ_TreeNode_   *children;
  struct EZ_TreeNode_   *sibling;
} EZ_TreeNode;

#define EZ_TreeNodeType(node)           ((node)->type)
#define EZ_TreeNodeHideSubtree(node)    ((node)->hideSubtree)
#define EZ_TreeNodeOpenable(node)       ((node)->openable)
#define EZ_TreeNodeItem(node)           ((node)->item)
#define EZ_TreeNodeChildren(node)       ((node)->children)
#define EZ_TreeNodeChildren(node)       ((node)->children)
#define EZ_TreeNodeSibling(node)        ((node)->sibling)
#define EZ_TreeNodeParent(node)         ((node)->parent)
#define EZ_TreeNodeSpacing(node)        ((node)->indent1)
#define EZ_TreeNodeIndent(node)         ((node)->indent2)
#define EZ_TreeNodeDrawBranch(node)     ((node)->drawbranch)
#define EZ_TreeNodeTWidth(node)         ((node)->width)
#define EZ_TreeNodeTHeight(node)        ((node)->height)
#define EZ_TreeNodeTHeight(node)        ((node)->height)
/*******************************************************************************/

