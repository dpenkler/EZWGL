/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 *             COPYRIGHT (C) 1996, 1997  Maorong Zou
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABRILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
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
 *  May 1997. Handling of raw rgb images added by Arno Dabekaussen.
 *  November 1997.  Release Version 1.3
 *  November 1998.  Release Version 1.4
 *  December 1999.  Release Version 1.50
 */
/*****************************************************************
 *
 *  Widget Header File
 *
 */
#ifndef _EZ_WIDGET_H_
#define	_EZ_WIDGET_H_

#include "config.h"

#include "feature.h"

#include <stdio.h>

#include <math.h>

#include <ctype.h>

#include <errno.h>

#include <signal.h>

#include <limits.h>


#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#  include <string.h>
#else 
#  include <strings.h>
#endif


#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif


#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif


#include <sys/types.h>

#include <sys/stat.h>

#include <sys/wait.h>


#ifdef TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif


#ifdef HAVE_TERMIOS_H
# include <termios.h>
#else
# include <sgtty.h>
#endif

#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>

#if defined(HAVE_XSHM) && defined(USE_XSHM)
#include <X11/extensions/XShm.h>
#endif

/*****************************************************************/

#ifndef   PI
#define   PI 3.141592653589793231
#endif
#include "EZ_Args.h"    
#include "EZ_Cursor.h"
#include "EZ_Resource.h"

/***********************************************************************/
typedef union EZ_WWindow_   EZ_Widget;
typedef union EZ_ItemUnion  EZ_Item;
typedef  void (*EZ_CallBack)      MY_ANSIARGS((void *object, void *data));
typedef  void (*EZ_Callback)      MY_ANSIARGS((void *object, void *data));
typedef  void (*EZ_InputCallback) MY_ANSIARGS((void *object, void *data, int fd, int mask));
typedef  void (*EZ_InputCallBack) MY_ANSIARGS((void *object, void *data, int fd, int mask));
typedef  void (*EZ_EventHandler)  MY_ANSIARGS((void *object, void *data, 
					       int etype, XEvent *event));
typedef  void (*EZ_EventFilter)  MY_ANSIARGS(( XEvent *event, void *data));
typedef  void (*EZ_MessageHandler)  MY_ANSIARGS((void *msgIn, void *client_data));
typedef  int  (*EZ_DnDMsgHandler) MY_ANSIARGS((void *object, void *message));

typedef  int  (*EZ_SChartHandle) MY_ANSIARGS((EZ_Widget *ss, void *chart, int r1, int c1, 
                                              int r2, int c2, int kind, void *data));
/* 4-16-98: overhaul dirTree implementation */
typedef  void *(*EZ_DirTreeNodeCreator)      MY_ANSIARGS((char *path, void *data));
/************************************************************************/

typedef  int  (*EZ_DnDEncoder)    MY_ANSIARGS((void *object, void *data, 
					       char **message,int *length, int *needfree));
typedef  int  (*EZ_DnDDecoder)    MY_ANSIARGS((void *object, void *data,
					       char *message,int length));

typedef  void (*EZ_WorkAreaGeometryManager)  MY_ANSIARGS((void *GMData, EZ_Item **data, int nitems, 
							  int Rx,int Ry,int Rw, int Rh, int xsp, int ysp,
							  int *w_ret, int *h_ret));

/*****************************************************************/

#ifndef EZ_StringToQuark
#define EZ_StringToQuark(_str__)      ((_str__)? XrmStringToQuark(_str__): NULLQUARK)
#define EZ_PermStringToQuark(_str__)  ((_str__)? XrmPermStringToQuark(_str__):NULLQUARK)
#define EZ_QuarkToString(_quk__)   ((_quk__)!=NULLQUARK? XrmQuarkToString(_quk__):"NULL")
#endif

/*************************************************************************
 *
 * Record for user defined resources
 */
typedef struct UDFResource_ {
  int             option;
  XrmQuark        iname, cname;
  unsigned char   vtypes[8];
} UDFResource;

/* resource specification */
typedef struct {  
  int     option;
  char    *iname;
  char    *cname;
  char    *vtypes;
} EZ_ResourceSpec;

typedef struct EZ_ResourceHandle_ {
  short             mark;  /* mark resources registered for all */
  short             nresources;
  UDFResource       *resources;
  int               (*configure)   MY_ANSIARGS((EZ_Widget *widget, int opt, EZ_Value *values));
  struct  EZ_ResourceHandle_ *next;
} EZ_ResourceHandle;

/***********************************************************************
 ***                                                                 ***
 *** The data structure of another simple hash table. Used to store  ***
 *** bitmaps                                                         ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_Bitmap_ {
  Pixmap pixmap;                   /* the stored pixmap */
  Pixmap shape;                    /* shape mask        */
  char   *name;                    /* the file_name     */
  int    width, height;            /* size of the pixmap*/
  int    ref;                      /* reference count   */
  int    bucket;                   /* saved in bucket   */
  char   type;                     /* depth = 1 or > 1  */
  char   special;                  /* special ref mark  */
  char   notused[2];
  Pixmap dkpixmap;                 /* testing ...       */
  Pixmap btpixmap;                 /* testing ...       */
  Pixmap htpixmap;                 /* testing ...       */
  int    x, y;                     /* special fields    */
  struct EZ_Bitmap_ *next;         /* the chain         */
} EZ_Bitmap;

typedef  EZ_Bitmap  EZ_BitmapHTEntry;
typedef  EZ_Bitmap  EZ_Pixmap;

#define EZ_BITMAP_IS_BITMAP       0
#define EZ_BITMAP_IS_PIXMAP       1

#define EZ_BitmapType(ptr)        ((ptr)->type)
#define EZ_BitmapPixmap(ptr)      ((ptr)->pixmap)
#define EZ_BitmapWidth(ptr)       ((ptr)->width)
#define EZ_BitmapHeight(ptr)      ((ptr)->height)
#define EZ_BitmapRefCount(ptr)    ((ptr)->ref)
#define EZ_BitmapName(ptr)        ((ptr)->name)
#define EZ_BitmapNext(ptr)        ((ptr)->next)
#define EZ_BitmapSpecial(ptr)     ((ptr)->special)

/*********************************************************************************/

typedef struct EZ_CallBackStruct_ 
{
  EZ_CallBack               callback;    /* the callback procedure               */
  void                      *data;       /* client data to be passed to callback */
  int                       internal;    /* is this an internal callback ?       */
  struct EZ_CallBackStruct_ *next;
} EZ_CallBackStruct;
/*********************************************************************************/
typedef struct EZ_EventHandlerStruct_ 
{
  EZ_EventHandler               handler; /* the eventhandler                     */
  void                          *data;   /* client data to be passed to handler  */
  int                           internal;/* do we have any internal handlers ?   */
  struct EZ_EventHandlerStruct_ *next;
} EZ_EventHandlerStruct;

/*********************************************************************************
 *
 *  General type for client data.
 */
typedef  union {
  char            c;
  short           s;
  int             i;
  long            l;
  unsigned char   uc;
  unsigned short  us;
  unsigned int    ui;
  unsigned long   ul;
  float           f;
  void            *p;
}  EZ_UnknownDataType;

/*********************************************************************************/

#include "EZ_GL.h"
#include "EZ_Timer.h"
#include "EZ_Input.h"
#include "EZ_EmbedMsg.h"
#include "EZ_MemDebug.h"
#include "EZ_WidgetConfig.h"
#include "EZ_DnD.h"
#include "EZ_Item.h"
#include "EZ_GM.h"
/******************************************************************************/
typedef struct {
  Atom       messageType;      
  int        messageLength;    
  char       *message;
  int        messageId;    /* set by sender, transaction serial # */
  int        replyId;      /* set by sender, reply id             */
  int        isReply;
  /* src info */
  Window     sender;
  Atom       senderClass;
  Atom       senderInstance;
  int        senderIsSelf;
} EZ_Message;

#define EZ_MSG_HEADER_LENGTH     137  /* 17*8+1 */

/*
 * Each messageHandler handles exactly one message defined by
 * the messaget type (atom) and the serial number (messageId).
 */
typedef struct EZ_MsgHdlrList_ {
  Atom                   atom;        /* type of message the handler handles */
  int                    messageId;   /* which message to handler */
  void                   *data;
  EZ_MessageHandler      handler;
  EZ_Timer               *timer;
  EZ_CallBack            timerCB;
  void                   *timerData;
  struct EZ_MsgHdlrList_ *next;
} EZ_MessageHandle;

/*********************************************************************************/
typedef struct EZ_XImage_ {
  XImage          *ximage;
  int             shm;
  void            *shminfo;
} EZ_XImage;

/*********************************************************************
 *
 *  Type used  EZ_CreateMenuFromList
 */
typedef struct menuEntry_ {
  int           type;
  char          *name;
  char          *label;
  char          *accelerator;
  int           underline;
  int           returnValue;
  EZ_CallBack   callback;
  void          *clientData;
  int           v1,v2,v3;
} EZ_MenuEntry;
/*********************************************************************************/

#ifndef MAXV
#define MAXV(a,b) ((a)>(b)? (a):(b))
#endif
#ifndef MINV
#define MINV(a,b) ((a)<(b)? (a):(b))
#endif
#ifndef ABSV
#define ABSV(a) ((a)>=0? (a): -(a))
#endif
#define EZ_RingBell()   XBell(EZ_Display, 30)
/**************************************************************************/

#define EZ_EXIT_OK           0
#define EZ_EXIT_ERROR        1
#define EZ_NO                0
#define EZ_YES               1
#define EZ_NULL              NULL
#define EZ_TRUE              True
#define EZ_FALSE             False

/**************************************************************************
 *
 *  some special hack
 */

#define EZ_NAIL_IT          1

/*********************************************************************
 *
 *  Supported Image types, returned by EZ_GetImageFileType(char *fname)
 */
#define EZ_UNKNOWN_IMAGE_FORMAT   -1
#define EZ_PPM_FORMAT             0
#define EZ_GIF_FORMAT             1
#define EZ_XPM_FORMAT             2
#define EZ_BMP_FORMAT             3
#define EZ_JPG_FORMAT             4
#define EZ_PNG_FORMAT             5
#define EZ_TIF_FORMAT             6


/***********************************************************************
 ***                                                                 ***
 ***                   Border Styles                                 ***
 ***                                                                 ***
 ***********************************************************************/

#define EZ_BORDER_NONE                  0
#define EZ_BORDER_FLAT                  1
#define EZ_BORDER_FRAMED_UP             2
#define EZ_BORDER_FRAMED_DOWN           3
#define EZ_BORDER_UP                    4
#define EZ_BORDER_DOWN                  5
#define EZ_BORDER_NB_UP                 6
#define EZ_BORDER_NB_DOWN               7
#define EZ_BORDER_FRAMED                8
#define EZ_BORDER_CARVED                9
#define EZ_BORDER_EMBOSSED              EZ_BORDER_FRAMED
#define EZ_BORDER_ENGRAVED              EZ_BORDER_CARVED
#define EZ_BORDER_NB_UP_L               10
#define EZ_BORDER_NB_DOWN_L             11
#define EZ_BORDER_NB_UP_R               12
#define EZ_BORDER_NB_DOWN_R             13
#define EZ_BORDER_NB_UP_B               14
#define EZ_BORDER_NB_DOWN_B             15
#define EZ_BORDER_TB_UP                 16
#define EZ_BORDER_TB_DOWN               17
#define EZ_BORDER_LR_UP                 18
#define EZ_BORDER_LR_DOWN               19
#define EZ_BORDER_TB_FLAT               20
#define EZ_BORDER_LR_FLAT               21
#define EZ_BORDER_SHADOW                22
#define EZ_BORDER_SHADOW3               23
#define EZ_HSLIDER_BORDER               24
#define EZ_VSLIDER_BORDER               25

#define EZ_BORDER_STYLE_BEGIN           0
#define EZ_BORDER_STYLE_END             25

/***********************************************************************
 ***                                                                 ***
 ***                   GRID BAG CONSTRAINS                           ***
 ***                                                                 ***
 ***********************************************************************/

#define EZ_ROW          1
#define EZ_COLUMN       2
#define EZ_ALL_ROWS     4
#define EZ_ALL_COLUMNS  8

#define EZ_ON           1
#define EZ_OFF          0

#define EZ_GARBAGE_IS_WIDGET  0
#define EZ_GARBAGE_IS_ITEM    1

/***********************************************************************
***                                                                 ***
***                   Indicator Types                               ***
***                                                                 ***
***********************************************************************/
#define EZ_EMPTY_INDICATOR          0
#define EZ_SQUARE_INDICATOR         1
#define EZ_SUNKEN_SQUARE_INDICATOR  2
#define EZ_DIAMOND_INDICATOR        3
#define EZ_SUNKEN_DIAMOND_INDICATOR 4
#define EZ_CIRCLE_INDICATOR         5
#define EZ_CHECK_INDICATOR          6
#define EZ_RECTANGLE_INDICATOR      7
#define EZ_MENU_BUTTON_INDICATOR    8
#define EZ_TICK_INDICATOR           9

/***********************************************************************
***                                                                 ***
***                   Internal Fonts                                ***
***                                                                 ***
***********************************************************************/

#define EZ_DEFAULT_FONT                 0
#define EZ_NORMAL_FONT                  1
#define EZ_THIN_FONT                    2 
#define EZ_BOLD_FONT                    3 
#define EZ_ITALIC_FONT                  4
#define EZ_TINY_FONT                    5
#define EZ_MEDIUM_FONT                  6
#define EZ_HUGE_FONT                    7

#define EZ_BUTTON_FONT                  EZ_BOLD_FONT
#define EZ_LABEL_FONT                   EZ_BOLD_FONT
#define EZ_SLIDER_FONT                  EZ_BOLD_FONT
#define EZ_MENU_FONT                    EZ_BOLD_FONT
#define EZ_INPUT_FONT                   EZ_THIN_FONT
#define EZ_TEXT_FONT                    EZ_THIN_FONT
#define EZ_MENU_TITLE_FONT              EZ_ITALIC_FONT

/***********************************************************************
***                                                                 ***
***                   Default GCs                                   ***
 ***                                                                 ***
 ***********************************************************************/
#define TOTAL_NUM_GCS  DARKRED+22 

#define EZ_DEFAULTGC                   (DefaultGC(EZ_Display,EZ_ScreenNum))

#define EZ_BLACKGC                     (EZ_GCList[BLACK])
#define EZ_DARKGC1                     (EZ_GCList[EZ_DARK_GRAY1])
#define EZ_DARKGC2                     (EZ_GCList[EZ_DARK_GRAY2])
#define EZ_NORMALGC1                   (EZ_GCList[EZ_NORMAL_GRAY1])
#define EZ_NORMALGC2                   (EZ_GCList[EZ_NORMAL_GRAY2])
#define EZ_BRIGHTGC1                   (EZ_GCList[EZ_BRIGHT_GRAY1])
#define EZ_BRIGHTGC2                   (EZ_GCList[EZ_BRIGHT_GRAY2])
#define EZ_WHITEGC                     (EZ_GCList[WHITE])

#define EZ_SHADOWGC                     (EZ_GCList[EZ_DARK_GRAY2])

#define EZ_GRAYFGC                     (EZ_GCList[EZ_GRAYF])
#define EZ_DARKREDGC                   (EZ_GCList[DARKRED])

#define EZ_NORMAL_TEXTGC               (EZ_GCList[DARKRED+1])
#define EZ_THIN_TEXTGC                 (EZ_GCList[DARKRED+2])
#define EZ_BOLD_TEXTGC                 (EZ_GCList[DARKRED+3])
#define EZ_ITALIC_TEXTGC               (EZ_GCList[DARKRED+4])

#define EZ_WRITABLEGC                  (EZ_GCList[DARKRED+5])

#define EZ_BD_DARKGC                   (EZ_GCList[DARKRED+6])
#define EZ_BD_BRIGHTGC                 (EZ_GCList[DARKRED+7])
#define EZ_BD_NORMALGC                 (EZ_GCList[DARKRED+8])

#define EZ_XORGC                       (EZ_GCList[DARKRED+9])
#define EZ_FIGGC                       (EZ_GCList[DARKRED+10]) /* for ItemFig */
#define EZ_DRAGGC                      (EZ_GCList[DARKRED+11])
#define EZ_DASHEDLINEGC                (EZ_GCList[DARKRED+12])
#define EZ_DNDXORGC                    (EZ_GCList[DARKRED+13])
#define EZ_WRITABLEGC2                 (EZ_GCList[DARKRED+14])

#define EZ_DTILEGC1                    (EZ_GCList[DARKRED+15])
#define EZ_DTILEGC2                    (EZ_GCList[DARKRED+16])
#define EZ_BTILEGC1                    (EZ_GCList[DARKRED+17])
#define EZ_BTILEGC2                    (EZ_GCList[DARKRED+18])
#define EZ_NTILEGC                     (EZ_GCList[DARKRED+19])
#define EZ_NTILEGC1                    (EZ_GCList[DARKRED+19])
#define EZ_NTILEGC2                    (EZ_GCList[DARKRED+20])

#define EZ_FIGUPDATEGC                 (EZ_GCList[DARKRED+21])

#define EZ_BUTTON_TEXTGC               EZ_BOLD_TEXTGC
#define EZ_LABEL_TEXTGC                EZ_BOLD_TEXTGC
#define EZ_MENU_TEXTGC                 EZ_BOLD_TEXTGC
#define EZ_INPUT_TEXTGC                EZ_THIN_TEXTGC
#define EZ_TEXT_TEXTGC                 EZ_THIN_TEXTGC

/***********************************************************************
 ***                                                                 ***
 ***                   Generic Event Mask                            ***
 ***                                                                 ***
 ***********************************************************************/

#define ALL_EVENTS_MASK  KeyPressMask|ButtonReleaseMask|ExposureMask|\
                    ButtonPressMask|StructureNotifyMask|ButtonMotionMask|\
                    EnterWindowMask|LeaveWindowMask|PointerMotionMask|\
                    KeymapStateMask|FocusChangeMask|VisibilityChangeMask

#define ALL_EVENT_MASK  ALL_EVENTS_MASK

/***********************************************************************
 ***                                                                 ***
 ***                   Default Foreground                            ***
 ***                                                                 ***
 ***********************************************************************/

#define EZ_DefaultForeground  (EZ_ColorArray[EZ_GRAYF])

/***********************************************************************
 ***                                                                 ***
 ***                   Special labels                                ***
 ***                                                                 ***
 ***********************************************************************/

#define EZ_UP_TRIANGLE          1
#define EZ_DOWN_TRIANGLE        2
#define EZ_LEFT_TRIANGLE        3
#define EZ_RIGHT_TRIANGLE       4
#define EZ_UP_ARROW             5
#define EZ_DOWN_ARROW           6
#define EZ_LEFT_ARROW           7
#define EZ_RIGHT_ARROW          8

#define EZ_UP_TRIANGLE_BAR      9
#define EZ_DOWN_TRIANGLE_BAR    10 /* thanks to Tomi Sarvela <orava@nether.tky.hut.fi> */
#define EZ_LEFT_TRIANGLE_BAR    11
#define EZ_RIGHT_TRIANGLE_BAR   12

#define EZ_UP_ARROW_BAR         13
#define EZ_DOWN_ARROW_BAR       14
#define EZ_LEFT_ARROW_BAR       15
#define EZ_RIGHT_ARROW_BAR      16

#define EZ_DOUBLE_LEFT_TRIANGLE  17
#define EZ_DOUBLE_RIGHT_TRIANGLE 18
#define EZ_DOUBLE_UP_TRIANGLE    19
#define EZ_DOUBLE_DOWN_TRIANGLE  20

#define EZ_LEFT_RIGHT_ARROW     21
#define EZ_UP_DOWN_ARROW        22

#define EZ_LEFT_BAR_TRIANGLE    23
#define EZ_RIGHT_BAR_TRIANGLE   24
#define EZ_UP_BAR_TRIANGLE      25
#define EZ_DOWN_BAR_TRIANGLE    26

#define EZ_ELLIPSE              27
#define EZ_RECTANGLE            28

#define EZ_UP_TRIANGLE_UP       29
#define EZ_UP_TRIANGLE_DOWN     30
#define EZ_DOWN_TRIANGLE_UP     31
#define EZ_DOWN_TRIANGLE_DOWN   32
#define EZ_LEFT_TRIANGLE_UP     33
#define EZ_LEFT_TRIANGLE_DOWN   34
#define EZ_RIGHT_TRIANGLE_UP    35
#define EZ_RIGHT_TRIANGLE_DOWN  36


/***********************************************************************
 ***                                                                 ***
 ***                   Widget Types                                  ***
 ***                                                                 ***
 ***********************************************************************/

/* if reordered, make sure the names below is also reordered correspondingly */
#define EZ_WIDGET_UNKNOWN                0
#define EZ_WIDGET_FRAME                  1
#define EZ_WIDGET_LABEL                  2
#define EZ_WIDGET_NORMAL_BUTTON          3
#define EZ_WIDGET_CHECK_BUTTON           4
#define EZ_WIDGET_RADIO_BUTTON           5
#define EZ_WIDGET_MENU_BUTTON            6
#define EZ_WIDGET_MENU_SEPARATOR         7
#define EZ_WIDGET_MENU_SUBMENU           8
#define EZ_WIDGET_MENU_NORMAL_BUTTON     9
#define EZ_WIDGET_MENU_CHECK_BUTTON      10
#define EZ_WIDGET_MENU_RADIO_BUTTON      11
#define EZ_WIDGET_VERTICAL_SLIDER        12
#define EZ_WIDGET_HORIZONTAL_SLIDER      13
#define EZ_VERTICAL_SLIDER               EZ_WIDGET_VERTICAL_SLIDER
#define EZ_HORIZONTAL_SLIDER             EZ_WIDGET_HORIZONTAL_SLIDER
#define EZ_WIDGET_ENTRY                  14
#define EZ_WIDGET_3D_CANVAS              15
#define EZ_WIDGET_VERTICAL_SCROLLBAR     16
#define EZ_WIDGET_HORIZONTAL_SCROLLBAR   17
#define EZ_WIDGET_ILISTBOX               18
#define EZ_WIDGET_ILIST_BOX              EZ_WIDGET_ILISTBOX
#define EZ_WIDGET_LISTBOX                19
#define EZ_WIDGET_LIST_BOX               EZ_WIDGET_LISTBOX
#define EZ_WIDGET_ITEXT                  20
#define EZ_WIDGET_TEXT                   21
#define EZ_WIDGET_NOTE_BOOK              22
#define EZ_WIDGET_NOTEBOOK               EZ_WIDGET_NOTE_BOOK 
#define EZ_WIDGET_NB_PAGE                23
#define EZ_WIDGET_NOTEBOOK_PAGE          EZ_WIDGET_NB_PAGE
#define EZ_WIDGET_NW_LABEL               24
#define EZ_WIDGET_PANE_HANDLE            25
#define EZ_WIDGET_ARROW_BUTTON           26
#define EZ_WIDGET_EXECUTOR               27
#define EZ_WIDGET_OPTIONAL_ENTRY         28
#define EZ_WIDGET_FILE_SELECTOR          29
#define EZ_WIDGET_ITREE                  30
#define EZ_WIDGET_TREE                   31
#define EZ_WIDGET_LIST_TREE              EZ_WIDGET_TREE
#define EZ_WIDGET_IFANCY_LISTBOX         32
#define EZ_WIDGET_IFANCY_LIST_BOX        EZ_WIDGET_IFANCY_LISTBOX
#define EZ_WIDGET_FANCY_LISTBOX          33
#define EZ_WIDGET_FANCY_LIST_BOX         EZ_WIDGET_FANCY_LISTBOX
#define EZ_WIDGET_IWORK_AREA             34
#define EZ_WIDGET_WORK_AREA              35
#define EZ_WIDGET_RAW_XWINDOW            36
#define EZ_WIDGET_MENU_TEAR_OFF_BAR      37
#define EZ_WIDGET_ROW_COLUMN             38
#define EZ_WIDGET_GRID_BAG               EZ_WIDGET_ROW_COLUMN
#define EZ_WIDGET_LCD                    39
#define EZ_WIDGET_LED                    40
#define EZ_WIDGET_STATUS_METER           41
#define EZ_WIDGET_STATUS_BAR             EZ_WIDGET_STATUS_METER 
#define EZ_WIDGET_MENU_BAR               42
#define EZ_WIDGET_ITERM                  43
#define EZ_WIDGET_TERM                   44
#define EZ_WIDGET_HORIZONTAL_RULER       45
#define EZ_WIDGET_VERTICAL_RULER         46
#define EZ_WIDGET_SCROLL_BUTTON          47
#define EZ_WIDGET_SPIN_BUTTON            48
#define EZ_WIDGET_LOCATOR                49
#define EZ_WIDGET_DIAL                   50
#define EZ_WIDGET_GRADIENT_BAR           51
#define EZ_WIDGET_HISTOGRAM              52
#define EZ_WIDGET_SEPARATOR              53
#define EZ_WIDGET_SPREAD_SHEET           54
#define EZ_WIDGET_THUMB_WHEEL            55

#define EZ_WIDGET_F56                    56
#define EZ_WIDGET_F57                    57
#define EZ_WIDGET_F58                    58
#define EZ_WIDGET_F59                    59
#define EZ_WIDGET_F60                    60
#define EZ_WIDGET_F61                    61
#define EZ_WIDGET_F62                    62
#define EZ_WIDGET_F63                    63
#define EZ_WIDGET_F64                    64
#define EZ_WIDGET_F65                    65
#define EZ_WIDGET_F66                    66
#define EZ_WIDGET_F67                    67
#define EZ_WIDGET_F68                    68
#define EZ_WIDGET_F69                    69
#define EZ_WIDGET_F70                    70
#define EZ_WIDGET_F71                    71
#define EZ_WIDGET_F72                    72
#define EZ_WIDGET_F73                    73
#define EZ_WIDGET_F74                    74
#define EZ_WIDGET_F75                    75
#define EZ_WIDGET_F76                    76
#define EZ_WIDGET_F77                    77
#define EZ_WIDGET_F78                    78
#define EZ_WIDGET_F79                    79
#define EZ_WIDGET_F80                    80
#define EZ_WIDGET_F81                    81
#define EZ_WIDGET_F82                    82
#define EZ_WIDGET_F83                    83
#define EZ_WIDGET_F84                    84
#define EZ_WIDGET_F85                    85
#define EZ_WIDGET_F86                    86
#define EZ_WIDGET_F87                    87
#define EZ_WIDGET_F88                    88
#define EZ_WIDGET_F89                    89
#define EZ_WIDGET_F90                    90
#define EZ_WIDGET_F91                    91
#define EZ_WIDGET_F92                    92
#define EZ_WIDGET_F93                    93
#define EZ_WIDGET_F94                    94
#define EZ_WIDGET_F95                    95
#define EZ_WIDGET_F96                    96
#define EZ_WIDGET_F97                    97
#define EZ_WIDGET_F98                    98
#define EZ_WIDGET_F99                    99
#define EZ_WIDGET_F100                    100
#define EZ_WIDGET_F101                    101
#define EZ_WIDGET_F102                    102
#define EZ_WIDGET_F103                    103
#define EZ_WIDGET_F104                    104
#define EZ_WIDGET_F105                    105
#define EZ_WIDGET_F106                    106
#define EZ_WIDGET_F107                    107
#define EZ_WIDGET_F108                    108
#define EZ_WIDGET_F109                    109
#define EZ_WIDGET_F110                    110
#define EZ_WIDGET_F111                    111
#define EZ_WIDGET_F112                    112
#define EZ_WIDGET_F113                    113
#define EZ_WIDGET_F114                    114
#define EZ_WIDGET_F115                    115
#define EZ_WIDGET_F116                    116
#define EZ_WIDGET_F117                    117
#define EZ_WIDGET_F118                    118
#define EZ_WIDGET_F119                    119
#define EZ_WIDGET_F120                    120
#define EZ_WIDGET_F121                    121
#define EZ_WIDGET_F122                    122
#define EZ_WIDGET_F123                    123
#define EZ_WIDGET_F124                    124
/* 
 * the following few have to be the last and
 * has to be in the given order. They are
 * special toplevel widgets. In the case 
 * when this lib grows to more than 128 widgets,
 * change the type field to unsigned char or int
 * this field is used only to identify widgets,
 * not used in arithmetic so should be ok.
 */
#define EZ_WIDGET_POPUP_MENU              125
#define EZ_WIDGET_MENU                    EZ_WIDGET_POPUP_MENU
#define EZ_WIDGET_ITOPLEVEL               126
#define EZ_WIDGET_ICON                    EZ_WIDGET_ITOPLEVEL           
#define EZ_WIDGET_FREE_LABEL              EZ_WIDGET_ITOPLEVEL           
#define EZ_WIDGET_EMBEDER                 127
#define EZ_TOTAL_NUMBER_OF_WIDGETS        128

#ifdef _EZ_WIDGET_CONFIGURE_C_
/*****************************************************************
 *
 *  Widget Names. if reordered, make sure the types above is also
 *  reordered. 
 */
char *EZ_WidgetDefaultClassNames[] =
{
  "Unknown",
  "Frame",
  "Label",
  "Button", 
  "CheckButton",
  "RadioButton",  
  "MenuButton",
  "MenuSeparatorItem",
  "MenuSubmenuItem",
  "MenuButtonItem",
  "MenuCheckButtonItem", 
  "MenuRadioButtonItem",
  "VerticalSlider",
  "HorizontalSlider",
  "Entry",
  "ThreeDCanvas",
  "VerticalScrollbar",
  "HorizontalScrollbar",
  "IListBox",
  "ListBox",
  "IText",
  "Text",
  "Notebook",
  "NotebookPage",
  "NWLabel",
  "PaneHandle",
  "ArrowButton",
  "Executor",
  "OptionalEntry",
  "FileSelector",
  "ITree",
  "Tree",
  "IFancyListBox",
  "FancyListBox",
  "IWorkArea",
  "WorkArea",
  "RawXWindow",
  "TearOffBar",
  "RowColumn",
  "Lcd",
  "Led",
  "StatusMeter",
  "MenuBar",
  "ITerm",
  "Term",
  "HorizontalRuler",
  "VerticalRuler",
  "ScrollButton",
  "SpinButton",
  "Locator",
  "Dial",
  "GradientBar",
  "Histogram",
  "Separator",
  "SpreadSheet",
  "ThumbWheel",

  "F56",
  "F57",
  "F58",
  "F59",
  "F60",
  "F61",
  "F62",
  "F63",
  "F64",
  "F65",
  "F66",
  "F67",
  "F68",
  "F69",
  "F70",
  "F71",
  "F72",
  "F73",
  "F74",
  "F75",
  "F76",
  "F77",
  "F78",
  "F79",
  "F80",
  "F81",
  "F82",
  "F83",
  "F84",
  "F85",
  "F86",
  "F87",
  "F88",
  "F89",
  "F90",
  "F91",
  "F92",
  "F93",
  "F94",
  "F95",
  "F96",
  "F97",
  "F98",
  "F99",
  "F100",
  "F101",
  "F102",
  "F103",
  "F104",
  "F105",
  "F106",
  "F107",
  "F108",
  "F109",
  "F110",
  "F111",
  "F112",
  "F113",
  "F114",
  "F115",
  "F116",
  "F117",
  "F118",
  "F119",
  "F120",
  "F121",
  "F122",
  "F123",
  "F124",

  "Menu",
  "FreeLabel",
  "Embeder",

  NULL,
};
#else
extern char *EZ_WidgetDefaultClassNames[];
#endif

/* Quarks are declared in EZ_Resource.c */
#ifdef _EZ_RESOURCE_C_
XrmQuark EZ_WidgetDefaultQClassNames[128];
XrmQuark EZ_WidgetDefaultQInstanceNames[128];
XrmQuark EZ_ClassQName, EZ_InstanceQName;
XrmQuark EZ_ItemDefaultQClassNames[10];
XrmQuark EZ_ItemDefaultQInstanceNames[10];
Atom     EZ_ClassAName, EZ_InstanceAName;
#else
extern XrmQuark EZ_WidgetDefaultQClassNames[];
extern XrmQuark EZ_WidgetDefaultQInstanceNames[];
extern XrmQuark EZ_ClassQName, EZ_InstanceQName;
extern XrmQuark EZ_ItemDefaultQClassNames[10];
extern XrmQuark EZ_ItemDefaultQInstanceNames[10];
extern Atom     EZ_ClassAName, EZ_InstanceAName;
#endif
/*************************************************************************/

#define EZ_SetWidgetTypeAndNames(wptr, type_) \
{ \
   ((wptr)->common.type) = type_; \
   ((wptr)->common.instanceName) = EZ_WidgetDefaultQInstanceNames[type_];\
   ((wptr)->common.className) = EZ_WidgetDefaultQClassNames[type_];\
   ((wptr)->common.resourceHandle) = EZ_GetDefaultResourceHandle(type_);\
   if(EZ_CreateNewWidgetHook){ EZ_CreateNewWidgetHook(wptr);}\
}

/***********************************************************************
 ***                                                                 ***
 ***                   DoubleClick time interval                     ***
 ***                                                                 ***
 ***********************************************************************/
/*
 * if two Button1 click happened within 250 ms, it is regardes as a
 * double click. Used in widget listbox.
 */
#define DOUBLE_CLICK_TIME_INTERVAL        300

/***********************************************************************
 ***                                                                 ***
 ***                   Label line length                             ***
 ***                                                                 ***
 ***********************************************************************/

#define EZ_DEFAULT_LABEL_LINE_LENGTH   30
#define IS_END_OF_WORD(c) ( (c)==' ' || (c) == '\t' || (c) ==',' )
#define EZ_LABEL_SEPARATION  16

/***********************************************************************
 ***                                                                 ***
 ***                   Widget Arrangements                           ***
 ***                                                                 ***
 ***********************************************************************/
/*
 *  Policy to place children widget. (Stacking)
 */
#define EZ_HORIZONTAL              1         /* stack children horizontally, evenly spaced in parent' win */
#define EZ_HORIZONTAL_LEFT         2         /* stack children horizontally, start at the left boundary   */
#define EZ_HORIZONTAL_RIGHT        3         /* stack children horizontally, finish at the right bdy      */
#define EZ_HORIZONTAL_CENTER       4         /* stack children horizontally at the center of parent's win */

#define EZ_VERTICAL                5         /* stack children vertically, evenly spaced in parent's win  */
#define EZ_VERTICAL_TOP            6         /* stack children vertically, start at the top boundary      */
#define EZ_VERTICAL_BOTTOM         7         /* stack children vertically, finish at the bottom bdy       */
#define EZ_VERTICAL_CENTER         8         /* stack children vertically at the center of parent's win   */ 


#define EZ_WIDGET_STACKING_BEGIN   EZ_HORIZONTAL
#define EZ_WIDGET_STACKING_END     EZ_VERTICAL_CENTER

/*
 * Policy to place children widget. (Filling)
 * 
 * If the children widget are stacked vertically,
 * one can fill all its children widgets horizontally.
 * There are three different horizontal filling modes:
 *  + Do not fill (default)
 *  + Fill horizontally
 *  + Fill vertically
 *  + Fill both horizontally and vertically
 *
 * The situation is similar for vertically stacked widgets.
 *
 */
#define EZ_FILL_NONE               0
#define EZ_FILL_HORIZONTALLY       1
#define EZ_FILL_VERTICALLY         2
#define EZ_FILL_BOTH               3

#define EZ_WIDGET_FILL_BEGIN       EZ_FILL_NONE
#define EZ_WIDGET_FILL_END         EZ_FILL_BOTH
/*
 *  Policy to place children widget. (Alignment)
 *
 *  If children are stacked horizontally, one can
 *  align the children either against the top boundary
 *  or against the bottom boundary or against the
 *  horizontal center line (default).
 *
 *  If children widget are stacked vertically, one
 *  can align the children widget either against the
 *  left boundary or against the right boundary or
 *  against the vertical center line (default).
 *
 */               
#define EZ_CENTER_ALIGNED          0        /* align children widgets against the center (V or H) line      */

#define EZ_LEFT_ALIGNED            1        /* align children widgets agianst the left bdy of parent's win  */
#define EZ_RIGHT_ALIGNED           2        /* align children widgets agianst the right bdy of parent's win */

#define EZ_TOP_ALIGNED             3        /* align children widgets agianst the top bdy of parent's win   */
#define EZ_BOTTOM_ALIGNED          4        /* align children widgets agianst the bottom bdy of parent's win*/

#define EZ_WIDGET_SIDE_BEGIN       EZ_CENTER_ALIGNED
#define EZ_WIDGET_SIDE_END         EZ_BOTTOM_ALIGNED

/*
 * Policy to place LABEL on terminal widgets (buttons, sliders etc)
 */
#define EZ_CENTER                  0
#define EZ_LEFT                    1
#define EZ_RIGHT                   2
#define EZ_TOP                     3
#define EZ_BOTTOM                  4
#define EZ_TOP_LEFT                5
#define EZ_TOP_RIGHT               6
#define EZ_BOTTOM_LEFT             7
#define EZ_BOTTOM_RIGHT            8
#define EZ_LEFT_2_RIGHT            9
#define EZ_WIDGET_LABEL_POSITION_BEGIN    EZ_CENTER
#define EZ_WIDGET_LABEL_POSITION_END      EZ_LEFT_2_RIGHT
#define EZ_UP                      10
#define EZ_DOWN                    11
#define EZ_LEFT_RIGHT              12
#define EZ_UP_DOWN                 13


/***********************************************************************
 ***                                                                 ***
 ***                   Directions                                    ***
 ***                                                                 ***
 ***********************************************************************/

#define EZ_EAST        0x1     
#define EZ_SOUTH       0x2
#define EZ_WEST        0x4
#define EZ_NORTH       0x8
#define EZ_NORTHEAST   (EZ_NORTH | EZ_EAST)
#define EZ_NORTHWEST   (EZ_NORTH | EZ_WEST)
#define EZ_SOUTHEAST   (EZ_SOUTH | EZ_EAST)
#define EZ_SOUTHWEST   (EZ_SOUTH | EZ_WEST)


/***********************************************************************
 ***                                                                 ***
 ***                   Default Padding                               ***
 ***                                                                 ***
 ***********************************************************************/

#define EZ_WIDGET_DEFAULT_PADX                8
#define EZ_WIDGET_DEFAULT_PADY                6

/****************************************************************************
 *
 *  This structure records some of the important information about
 *  the display.
 */
/* these are used only for PseudoColor and GrayScale visuals   */
#define EZ_DEFAULT_COLORMAP     0      /* read only, shared    */
#define EZ_READWRITE_COLORMAP   1      /* default,  read/write */
#define EZ_PRIVATE_COLORMAP     2      /* private, read/write  */

typedef struct {
  Display           *TheDisplay;     /* the display connected    */
  int               TheScreenNum;    /* the screen number        */
  Visual            *TheVisual;      /* the visual               */
  Window            dummyWindow;     /* for creating gc's        */
  int               TheDepth;        /* bpp, 8, 16 or 24         */

  char              CmapType;        /* what colormap to use     */
  char              WidgetOnly;      /* do not be greedy on clrs */
  char              BackingStore;    /* server support b store ? */
  char              hasSHMExt;       /* server has SHM extension */
  char              hasSHAPEExt;     /* server has Shape ext.    */
  char              ByteOrder;       /* byte order in ximages    */
  char              HByteOrder;      /* host byte order          */
  char              bfgset;          /* bg color set by usr ?    */

  int               NColors;         /* # of colors allocated    */
  int               FreeColorStart;  /* indx of next unused clr  */
  int               AvailableColorsForGL; /* num clrs for GL     */
  int               HDimension;      /* hori size of display     */
  int               VDimension;      /* veri size of display     */
  int               bpp;
  
  unsigned long     ColorArray[COLOR_TAB_SIZE]; /* pixel values  */
  XColor            xcolors[COLOR_TAB_SIZE];    /* rgb values    */
  XColor            dark[COLOR_TAB_SIZE];       /* for widget bd */
  XColor            bright[COLOR_TAB_SIZE];     /* for widget bd */
  XColor            highlight[COLOR_TAB_SIZE];  /* and highlight */
  Colormap          TheColormap;     /* the colormap             */
  Atom EZWGL_APPLICATION;   /* 3 private atoms for communication */
  Atom EZWGL_EMBEDING_MSG;
  Atom EZWGL_WLIST;
  long              max_req_size;
} XSInfo;

#define EZ_Display            (XServerInfo.TheDisplay)
#define EZ_ScreenNum          (XServerInfo.TheScreenNum)
#define EZ_Visual             (XServerInfo.TheVisual)
#define EZ_DummyWindow        (XServerInfo.dummyWindow)
#define EZ_Depth              (XServerInfo.TheDepth)
#define EZ_BitsPerPixel       (XServerInfo.bpp)
#define EZ_ColorArray         (XServerInfo.ColorArray)
#define EZ_ColorArrayFix      (_ColorArrayForImage_)
#define EZ_Colormap           (XServerInfo.TheColormap)
#define EZ_ColormapType       (XServerInfo.CmapType)
#define EZ_TotalNumOfColors   (XServerInfo.NColors)
#define EZ_UnusedColorIdx     (XServerInfo.FreeColorStart)
#define EZ_XColorArray        (XServerInfo.xcolors)
#define EZ_WidgetOnly         (XServerInfo.WidgetOnly)
#define EZ_UseBackingStore    (XServerInfo.BackingStore)
#define EZ_AvailableColors    (XServerInfo.AvailableColorsForGL)
#define EZ_XServerHasSHMExt   (XServerInfo.hasSHMExt)
#define EZ_XServerHasSHAPEExt (XServerInfo.hasSHAPEExt)
#define EZ_XImageByteOrder    (XServerInfo.ByteOrder)
#define EZ_HostByteOrder      (XServerInfo.HByteOrder)
#define EZ_BFgSet             (XServerInfo.bfgset)

#define EZ_XDisplayWidth      (XServerInfo.HDimension)
#define EZ_XDisplayHeight     (XServerInfo.VDimension)

#define EZ_DarkColor          (XServerInfo.dark)
#define EZ_BrightColor        (XServerInfo.bright)
#define EZ_HighlightColor     (XServerInfo.highlight)

#define EZ_DisplayForWidgets         EZ_Display
#define EZ_ScreenNumForWidgets       EZ_ScreenNum
#define EZ_VisualForWidgets          EZ_Visual
#define EZ_DepthForWidgets           EZ_Depth
#define EZ_ColorArrayForWidgets      EZ_ColorArray
#define EZ_ColormapForWidgets        EZ_Colormap

#define EZ_XServerDataReqSize         (XServerInfo.max_req_size)
#define EZ_APPLICATION_ATOM           (XServerInfo.EZWGL_APPLICATION)
#define EZ_WLIST_ATOM                 (XServerInfo.EZWGL_WLIST)
#define EZ_EMBEDING_MESSAGE_ATOM      (XServerInfo.EZWGL_EMBEDING_MSG)

/***********************************************************************/
#define EZ_GrabServer() { \
    if( EZ_ServerGrabed == 0) XGrabServer(EZ_Display);\
     EZ_ServerGrabed++; \
}

#define EZ_UngrabServer() { \
    if( EZ_ServerGrabed == 1) XUngrabServer(EZ_Display);\
    EZ_ServerGrabed--; \
    if( EZ_ServerGrabed < 0) EZ_ServerGrabed = 0;\
}

/***********************************************************************
 ***                                                                 ***
 ***                   HashTableSize                                 ***
 ***                                                                 ***
 ***********************************************************************/

#define EZ_WIDGET_HASH_TABLE_SIZE           499
#define EZ_SMALL_HASH_TABLE_SIZE            41

/***********************************************************************
 ***                                                                 ***
 ***               Common parts of all Widgets                       ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_WidgetCommon_ {
  int               x,y;              /* origin in its parent    */
  int               w,h;              /* actual dimension        */
  Window            win;              /* the window ID           */
  union EZ_WWindow_ *parent;          /* the parent widget       */
  union EZ_WWindow_ *sibling;         /* the sibling list        */
  union EZ_WWindow_ *children;        /* list of children        */
  int               blocked;          /* callback... in action   */

  /* these three fields are used for embeding */
  Window            DnDName;          /* embeding comm win       */
  Window            DnDParent;        /* embeding  parent win    */
  int               aminW, aminH;     /* computed min size       */

  Cursor            cursor;           /* cursor for this widget  */
  EZ_Bitmap         *pixmap;          /* bitmap label            */
  EZ_Bitmap         *bgpixmap;        /* bg tile                 */
  EZ_Bitmap         *parentbgpixmap;  /* parent bg tile          */
  EZ_Bitmap         *bgpixmap2;       /* ...                     */
  EZ_Bitmap         *parentbgpixmap2; /* parent bg tile          */
  EZ_Bitmap         *shape;           /* window shape mask       */

  unsigned long     foreground;       /* foreground color        */
  unsigned long     background;       /* background color        */
  unsigned long     *parentbg;        /* parent background color */
  unsigned long     highlightBG;      /* highlight bg color      */
  unsigned long     highlightFG;      /* highlight fg color      */
  XFontStruct       *font;            /* default font            */ 

  char              *bubble_string;   /* bubble help string      */

  unsigned long     flags;            /* various flags           */

  unsigned char     type;             /* widget type             */
  unsigned char     maped;            /* window's mapped or not  */
  unsigned char     borderstyle;      /* style of border         */
  /* fields control how to display textual lable */
  unsigned char     labelposition;    /* anchor pos for label    */
  unsigned char     justification;    /* label justification     */
  /* fields used for the default geometry manager */
  unsigned char     stacking;         /* horizontal or vertical  */
  unsigned char     alignment;        /* against left bdy or ... */
  unsigned char     fillmode;         /* fill mode               */

  unsigned char     toset;            /* tile origin set ?   */
  unsigned char     destroyed;        /* widget is destroyed */
  unsigned char     labelShadow;      /* label Shadow set ?  */
  unsigned char     overrideRedirect;
  int               tox;              /* tile Origin         */
  int               toy;

  /*
   * the following 4 fields are very special. They are used only
   * in Frame, Button, CheckButton and RadioButton widgets. The reason
   * for them to be here is to simplify the coding for the default
   * geometry manager.
   */
  int               yaux;             /* special y padding Top & Bottom. For labeled frame  */
  int               hAdjust;          /* special height adjust. label height in Labled frame  */
  int               xauxL;            /* special x padding, left */
  int               xauxR;            /* special xpad, right     */
  
  int               minW;             /* minimal dimension       */
  int               minH;             /* may be modified by fill */

  /* if internal size is larger than the size needed to fit its children, */
  /* the difference is stored in the following two fields */
  int               cW;               
  int               cH;

  int               aW;               /* assigned width by parent*/
  int               aH;               /* assigned height         */

  int               whint;            /* user prefered width     */
  int               hhint;            /* user prefered height    */

  int               padx;             /* hori size  adjustment   */
  int               pady;             /* vertical suzeadjustment */
  int               padb;             /* h v padding outside bdr */

  int               sepx;             /* internal dist between   */
  int               sepy;             /* widgets on the same win */

  int               numchildren;      /* number of children      */
  int               borderwidth;      /* width of border         */

  int               RetData;          /* menu (items) return value */  

  EZ_EventHandlerStruct *eventHandler;     /* user-defined event handler */
  EZ_CallBackStruct     *callbacks;        /* array of callbacks      */
  EZ_CallBackStruct     *motionCallbacks;  /* not available to all widgets */
  EZ_CallBackStruct     *destroyCallbacks; /* callback on destruction */

  /* for Drag and Drop */
  Cursor            DnDDragCursor;         /* cursor to use if it is a drag src */
  void              *DnDtmp;               /* tmp storage for DnD  */
  EZ_DnDMsgHandler  DnDHandler;            /* Internal Msg handler: used only in 3 widgets */
  EZ_DnDDataEncoder *DnDDataEncoders;      /* list of data converters, encoding */
  EZ_DnDDataDecoder *DnDDataDecoders;      /* list of data converters, deoding */
  char              *DnDTargetHelp;        /* help info for DnDTargets  */

  /* for the gridbag geometry manager */
  int  gridX,  gridY;                      /* location of TL corner in grid */
  int  gridW,  gridH;                      /* Horizontal and vertical span  */
  unsigned  char gridfill, gridanchor;     /* fill mode and anchor */
  unsigned  char seCode;                   /* special effects */
  unsigned  char seParam;                  /* special effects */

  EZ_GManager       *gmanager;             /* geometry manager */
  /* resources manager data */
  XrmQuark         className;              /* class name */
  XrmQuark         instanceName;           /* instance name */

  void              *wmhints;               /* window manager hints */

  int               IntData;              /* extra space for usrdata */
  void              *PtrData;             /* extra space for usrdata */

  int                npvalues;            /* dynamically allocated storage */
  EZ_UnknownDataType *pvalues;            /* for more general client data  */

  /* for user defined resources */
  EZ_ResourceHandle *resourceHandle;
  char              *resourceString;

  union EZ_WWindow_   *groupLeader; 

} EZ_WidgetCommon;

#define EZ_WidgetGroupLeader(wptr)      ((wptr)->common.groupLeader)
#define EZ_WidgetResourceHandle(wptr)   ((wptr)->common.resourceHandle)
#define EZ_WidgetResourceString(wptr)   ((wptr)->common.resourceString)
#define EZ_WidgetOverrideRedirect(wptr) ((wptr)->common.overrideRedirect)
#define EZ_WidgetLabelShadow(wptr)   ((wptr)->common.labelShadow)
#define EZ_WidgetIsDestroyed(wptr)   ((wptr)->common.destroyed)
#define EZ_WidgetTOSet(wptr)         ((wptr)->common.toset)
#define EZ_WidgetTOX(wptr)           ((wptr)->common.tox)
#define EZ_WidgetTOY(wptr)           ((wptr)->common.toy)
#define EZ_WidgetBlocked(wptr)       ((wptr)->common.blocked)
#define EZ_WidgetCName(wptr)         ((wptr)->common.className)
#define EZ_WidgetIName(wptr)         ((wptr)->common.instanceName)
#define EZ_WidgetCursor(wptr)        ((wptr)->common.cursor)
#define EZ_WidgetWMHints(wptr)       ((wptr)->common.wmhints)
#define EZ_WidgetGManager(wptr)      ((wptr)->common.gmanager)

#define EZ_WidgetOriginX(wptr)       ((wptr)->common.x)
#define EZ_WidgetOriginY(wptr)       ((wptr)->common.y)
#define EZ_WidgetWidth(wptr)         ((wptr)->common.w)
#define EZ_WidgetHeight(wptr)        ((wptr)->common.h)
#define EZ_WidgetWindow(wptr)        ((wptr)->common.win)
#define EZ_WidgetParent(wptr)        ((wptr)->common.parent)
#define EZ_WidgetChildren(wptr)      ((wptr)->common.children)
#define EZ_WidgetSibling(wptr)       ((wptr)->common.sibling)

#define EZ_WidgetCW(wptr)            ((wptr)->common.cW)
#define EZ_WidgetCH(wptr)            ((wptr)->common.cH)

#define EZ_WidgetDnDApplName(wptr)   ((wptr)->common.DnDName)
#define EZ_WidgetDnDParent(wptr)     ((wptr)->common.DnDParent)

#define EZ_WidgetShapeMask(wptr)     ((wptr)->common.shape)

#define EZ_WidgetPixmap(wptr)        ((wptr)->common.pixmap)
#define EZ_WidgetBGPixmap(wptr)      ((wptr)->common.bgpixmap)
#define EZ_WidgetBGPixmap2(wptr)     ((wptr)->common.bgpixmap2)
#define EZ_WidgetBGPixmapB(wptr)     ((wptr)->common.bgpixmap2)
#define EZ_WidgetParentBGPixmap(wptr)  ((wptr)->common.parentbgpixmap)
#define EZ_WidgetParentBGPixmap2(wptr) ((wptr)->common.parentbgpixmap2)
#define EZ_WidgetParentBGPixmapB(wptr) ((wptr)->common.parentbgpixmap2)
#define EZ_WidgetForeground(wptr)    ((wptr)->common.foreground)
#define EZ_WidgetBackground(wptr)    ((wptr)->common.background)
#define EZ_WidgetHighlightBG(wptr)   ((wptr)->common.highlightBG)
#define EZ_WidgetHighlightFG(wptr)   ((wptr)->common.highlightFG)
#define EZ_WidgetParentBG(wptr)      ((wptr)->common.parentbg)
#define EZ_WidgetFont(wptr)          ((wptr)->common.font)
#define EZ_WidgetBubbleString(wptr)  ((wptr)->common.bubble_string)
#define EZ_WidgetFlags(wptr)         ((wptr)->common.flags)
#define EZ_WidgetType(wptr)          ((wptr)->common.type)
#define EZ_WidgetMinWidth(wptr)      ((wptr)->common.minW)
#define EZ_WidgetMinHeight(wptr)     ((wptr)->common.minH)
#define EZ_WidgetAMinWidth(wptr)     ((wptr)->common.aminW)
#define EZ_WidgetAMinHeight(wptr)    ((wptr)->common.aminH)
#define EZ_WidgetAWidth(wptr)        ((wptr)->common.aW)
#define EZ_WidgetAHeight(wptr)       ((wptr)->common.aH)
#define EZ_WidgetWidthHint(wptr)     ((wptr)->common.whint)
#define EZ_WidgetHeightHint(wptr)    ((wptr)->common.hhint)
#define EZ_WidgetStacking(wptr)      ((wptr)->common.stacking)
#define EZ_WidgetOrientation(wptr)   ((wptr)->common.stacking)
#define EZ_WidgetAlignment(wptr)     ((wptr)->common.alignment)
#define EZ_WidgetLabelPosition(wptr) ((wptr)->common.labelposition)
#define EZ_WidgetJustification(wptr) ((wptr)->common.justification)
#define EZ_WidgetFillMode(wptr)      ((wptr)->common.fillmode)
#define EZ_WidgetNumChildren(wptr)   ((wptr)->common.numchildren)
#define EZ_WidgetPadX(wptr)          ((wptr)->common.padx)
#define EZ_WidgetPadY(wptr)          ((wptr)->common.pady)
#define EZ_WidgetPadB(wptr)          ((wptr)->common.padb)
#define EZ_WidgetSepX(wptr)          ((wptr)->common.sepx)
#define EZ_WidgetSepY(wptr)          ((wptr)->common.sepy)
#define EZ_WidgetAuxXLeft(wptr)      ((wptr)->common.xauxL)
#define EZ_WidgetAuxXRight(wptr)     ((wptr)->common.xauxR)
#define EZ_WidgetAuxY(wptr)          ((wptr)->common.yaux)
#define EZ_WidgetHAdjust(wptr)       ((wptr)->common.hAdjust)
#define EZ_WidgetMaped(wptr)         ((wptr)->common.maped)
#define EZ_WidgetMapped(wptr)        ((wptr)->common.maped)
#define EZ_WidgetBorderWidth(wptr)   ((wptr)->common.borderwidth)
#define EZ_WidgetBorderStyle(wptr)   ((wptr)->common.borderstyle)
#define EZ_WidgetIntData(wptr)       ((wptr)->common.IntData)
#define EZ_WidgetPtrData(wptr)       ((wptr)->common.PtrData)
#define EZ_WidgetRetData(wptr)       ((wptr)->common.RetData)
#define EZ_WidgetCallBackFunc(wptr)  ((wptr)->common.callbacks)
#define EZ_WidgetCallbackFunc(wptr)  ((wptr)->common.callbacks)
#define EZ_WidgetCallBack(wptr)      ((wptr)->common.callbacks)
#define EZ_WidgetCallback(wptr)      ((wptr)->common.callbacks)
#define EZ_WidgetCallback(wptr)      ((wptr)->common.callbacks)
#define EZ_WidgetMotionCallBack(wptr)  ((wptr)->common.motionCallbacks)
#define EZ_WidgetMotionCallback(wptr)  ((wptr)->common.motionCallbacks)
#define EZ_WidgetDestroyCallback(wptr)  ((wptr)->common.destroyCallbacks)
#define EZ_WidgetDestroyCallBack(wptr)  ((wptr)->common.destroyCallbacks)
#define EZ_WidgetEventHandler(wptr)  ((wptr)->common.eventHandler)
#define EZ_WidgetEventHandlers(wptr) ((wptr)->common.eventHandler)

#define EZ_WidgetDnDDragCursor(wptr)     ((wptr)->common.DnDDragCursor)
#define EZ_WidgetDnDTmp(wptr)            ((wptr)->common.DnDtmp)
#define EZ_WidgetDnDHandler(wptr)        ((wptr)->common.DnDHandler)
#define EZ_WidgetDnDDataEncoders(wptr)   ((wptr)->common.DnDDataEncoders)
#define EZ_WidgetDnDDataDecoders(wptr)   ((wptr)->common.DnDDataDecoders)
#define EZ_WidgetDnDTargetHelp(wptr)     ((wptr)->common.DnDTargetHelp)

#define EZ_WidgetIsDnDSrc(wptr)          EZ_WidgetDnDDataEncoders(wptr)
#define EZ_WidgetIsDnDTarget(wptr)       EZ_WidgetDnDDataDecoders(wptr)


#define EZ_WidgetXOffset(wptr)       (((wptr)->common.w -(int) ((wptr)->common.minW))>>1)
#define EZ_WidgetYOffset(wptr)       (((wptr)->common.h -(int) ((wptr)->common.minH))>>1)

#define EZ_WidgetGBX(wptr)           ((wptr)->common.gridX)
#define EZ_WidgetGBY(wptr)           ((wptr)->common.gridY)
#define EZ_WidgetGBW(wptr)           ((wptr)->common.gridW)
#define EZ_WidgetGBH(wptr)           ((wptr)->common.gridH)
#define EZ_WidgetGBFill(wptr)        ((wptr)->common.gridfill)
#define EZ_WidgetGBAnchor(wptr)      ((wptr)->common.gridanchor)

#define EZ_WidgetSECode(wptr)       ((wptr)->common.seCode)
#define EZ_WidgetSEParam(wptr)      ((wptr)->common.seParam)

#define EZ_WidgetNPValues(wptr)      ((wptr)->common.npvalues)
#define EZ_WidgetPValues(wptr)       ((wptr)->common.pvalues)

/***********************************************************************
 *
 *    invoke callbacks 
 */
#define EZ_ExecuteWidgetCallBacks(_the_widget_) \
{ \
   if(_the_widget_ && EZ_LookupWidgetFromAllHT(_the_widget_)) { \
    EZ_CallBackStruct *_the_callback_list = EZ_WidgetCallBackFunc((_the_widget_));\
    EZ_WidgetBlocked(_the_widget_) += 1;\
    while(_the_callback_list) \
     {\
         if(_the_callback_list->callback)\
            { (_the_callback_list->callback)(_the_widget_, _the_callback_list->data); }\
	   _the_callback_list = _the_callback_list->next; \
      }\
    EZ_WidgetBlocked(_the_widget_) -= 1;\
    }\
}


#define	 EZ_HandleMotionCallBacks(_widget_, _cbcks_) \
{\
 if(_widget_ && EZ_LookupWidgetFromAllHT(_widget_)) { \
    EZ_CallBackStruct *cbks__ = _cbcks_;\
    EZ_WidgetBlocked(_widget_) += 1;\
    while(cbks__) \
     {\
       if(cbks__->callback)\
         { (cbks__->callback)(_widget_, cbks__->data); }\
        cbks__= cbks__->next;\
     }\
    EZ_WidgetBlocked(_widget_) -= 1;\
  }\
}

/***********************************************************************
 ***                                                                 ***
 ***               Common Flags for all Widgets                      ***
 ***                                                                 ***
 ***********************************************************************/

#define EZ_WIDGET_X_SET_BIT                   (1L<<0)
#define EZ_WIDGET_Y_SET_BIT                   (1L<<1)

#define EZ_WIDGET_XY_SET_BITS                 (EZ_WIDGET_X_SET_BIT | EZ_WIDGET_Y_SET_BIT)

#define EZ_WIDGET_WIDTH_SET_BIT               (1L<<2)
#define EZ_WIDGET_HEIGHT_SET_BIT              (1L<<3)
#define EZ_WIDGET_SIZE_SET_BITS               (EZ_WIDGET_HEIGHT_SET_BIT | EZ_WIDGET_WIDTH_SET_BIT)
#define EZ_WIDGET_WIDTH_HINT_BIT              (1L<<4)
#define EZ_WIDGET_HEIGHT_HINT_BIT             (1L<<5)
#define EZ_WIDGET_SIZE_COMPUTED_BIT           (1L<<6)
#define EZ_WIDGET_SIZE_MAY_BE_CHANGED_BIT     (1L<<7)
#define EZ_WIDGET_EXPAND_BIT                  (1L<<8)
#define EZ_WIDGET_NONACTIVE_BIT               (1L<<9)
#define EZ_WIDGET_TRANSIENT_BIT               (1L<<10)
#define EZ_WIDGET_DISABLED_BIT                (1L<<11)
#define EZ_WIDGET_FREEZED_BIT                 (1L<<12)
#define EZ_WIDGET_HIGHLIGHT_BIT               (1L<<13)
#define EZ_WIDGET_DO_NOT_PROPAGATE_BIT        (1L<<14)
#define EZ_WIDGET_EMBEDED_BIT                 (1L<<15)
#define EZ_WIDGET_EMBEDER_BIT                 (1L<<16)
#define EZ_WIDGET_FOCUSABLE_BIT               (1L<<17)
#define EZ_WIDGET_SHAPED_WINDOW_BIT           (1L<<18)
#define EZ_WIDGET_THIN_FONT_BIT               (1L<<19)
#define EZ_WIDGET_IS_CLONE_BIT                (1L<<20)
#define EZ_WIDGET_SHAPE_SHIFT                 (1L<<21)
#define EZ_WIDGET_DETACHED_BIT                (1L<<22)

#define EZ_WIDGET_FORGET_X_BIT                (1L<<23)
#define EZ_WIDGET_FORGET_Y_BIT                (1L<<24)
#define EZ_WIDGET_FORGET_XY_BITS              ((1L<<23)|(1<<24))

#define EZ_WIDGET_FORGET_W_BIT                (1L<<25)
#define EZ_WIDGET_FORGET_H_BIT                (1L<<26)
#define EZ_WIDGET_FORGET_WH_BITS              ((1L<<25)|(1<<26))

#define EZ_WIDGET_FORGET_G_BITS               ((1L<<23)|(1L<<24)|(1L<<25)|(1L<<26))

#define EZ_WIDGET_SHAPE_SET_BIT               (1L<<27)
#define EZ_WIDGET_WM_IGNORE_BIT               (1L<<28)
#define EZ_WIDGET_HIGHLIGHT_FG_BIT            (1L<<29)
#define EZ_WIDGET_HIGHLIGHT_BG_BIT            (1L<<30)
#define EZ_WIDGET_HCLR_USED_BIT               (1L<<31)

/***********************************************************************************************/
#define EZ_GetWidgetNoGeomFlag(wptr) \
          (EZ_WidgetFlags(wptr) & (EZ_WIDGET_NONACTIVE_BIT | EZ_WIDGET_DETACHED_BIT))
/***********************************************************************************************/
#define EZ_GetWidgetDetachedFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_DETACHED_BIT)
#define EZ_SetWidgetDetachedFlag(wptr)   {EZ_WidgetFlags(wptr) |= EZ_WIDGET_DETACHED_BIT;}
#define EZ_ClearWidgetDetachedFlag(wptr) {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_DETACHED_BIT;}
/***********************************************************************************************/
#define EZ_GetWidgetToplevelFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_WM_IGNORE_BIT)
#define EZ_SetWidgetToplevelFlag(wptr)   {EZ_WidgetFlags(wptr) |= EZ_WIDGET_WM_IGNORE_BIT;}
#define EZ_ClearWidgetToplevelFlag(wptr) {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_WM_IGNORE_BIT;}
/***********************************************************************************************/
#define EZ_GetWidgetForgetGFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_FORGET_G_BITS)
#define EZ_SetWidgetForgetGFlag(wptr)   {EZ_WidgetFlags(wptr) |= EZ_WIDGET_FORGET_G_BITS;}
#define EZ_ClearWidgetForgetGFlag(wptr) {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_FORGET_G_BITS;}
/***********************************************************************************************/
#define EZ_SetWidgetForgetXFlag(wptr)   {EZ_WidgetFlags(wptr) |= EZ_WIDGET_FORGET_X_BIT;}
#define EZ_GetWidgetForgetXFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_FORGET_X_BIT)
#define EZ_ClearWidgetForgetXFlag(wptr) {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_FORGET_X_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetForgetYFlag(wptr)   {EZ_WidgetFlags(wptr) |= EZ_WIDGET_FORGET_Y_BIT;}
#define EZ_GetWidgetForgetYFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_FORGET_Y_BIT)
#define EZ_ClearWidgetForgetYFlag(wptr) {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_FORGET_Y_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetForgetXYFlag(wptr)   {EZ_WidgetFlags(wptr) |= EZ_WIDGET_FORGET_XY_BITS;}
#define EZ_GetWidgetForgetXYFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_FORGET_XY_BITS)
#define EZ_ClearWidgetForgetXYFlag(wptr) {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_FORGET_XY_BITS;}
/***********************************************************************************************/
#define EZ_SetWidgetForgetWFlag(wptr)   {EZ_WidgetFlags(wptr) |= EZ_WIDGET_FORGET_W_BIT;}
#define EZ_GetWidgetForgetWFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_FORGET_W_BIT)
#define EZ_ClearWidgetForgetWFlag(wptr) {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_FORGET_W_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetForgetHFlag(wptr)   {EZ_WidgetFlags(wptr) |= EZ_WIDGET_FORGET_H_BIT;}
#define EZ_GetWidgetForgetHFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_FORGET_H_BIT)
#define EZ_ClearWidgetForgetHFlag(wptr) {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_FORGET_H_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetForgetWHFlag(wptr)   {EZ_WidgetFlags(wptr) |= EZ_WIDGET_FORGET_WH_BITS;}
#define EZ_GetWidgetForgetWHFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_FORGET_WH_BITS)
#define EZ_ClearWidgetForgetWHFlag(wptr) {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_FORGET_WH_BITS;}
/***********************************************************************************************/

#define EZ_SetWidgetThinFontFlag(wptr)       {EZ_WidgetFlags(wptr) |= EZ_WIDGET_THIN_FONT_BIT;}
#define EZ_GetWidgetThinFontFlag(wptr)       (EZ_WidgetFlags(wptr) & EZ_WIDGET_THIN_FONT_BIT)
#define EZ_ClearWidgetThinFontFlag(wptr)     {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_THIN_FONT_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetShapedWinFlag(wptr)       {EZ_WidgetFlags(wptr) |= EZ_WIDGET_SHAPED_WINDOW_BIT;}
#define EZ_GetWidgetShapedWinFlag(wptr)       (EZ_WidgetFlags(wptr) & EZ_WIDGET_SHAPED_WINDOW_BIT)
#define EZ_ClearWidgetShapedWinFlag(wptr)     {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_SHAPED_WINDOW_BIT;}

#define EZ_SetWidgetShapedSetFlag(wptr)       {EZ_WidgetFlags(wptr) |= EZ_WIDGET_SHAPE_SET_BIT;}
#define EZ_GetWidgetShapedSetFlag(wptr)       (EZ_WidgetFlags(wptr) & EZ_WIDGET_SHAPE_SET_BIT)
#define EZ_ClearWidgetShapedSetFlag(wptr)     {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_SHAPE_SET_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetFocusableFlag(wptr)       {EZ_WidgetFlags(wptr) |= EZ_WIDGET_FOCUSABLE_BIT;}
#define EZ_GetWidgetFocusableFlag(wptr)       (EZ_WidgetFlags(wptr) & EZ_WIDGET_FOCUSABLE_BIT)
#define EZ_ClearWidgetFocusableFlag(wptr)     {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_FOCUSABLE_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetEmbederFlag(wptr)      {EZ_WidgetFlags(wptr) |= EZ_WIDGET_EMBEDER_BIT;}
#define EZ_GetWidgetEmbederFlag(wptr)      (EZ_WidgetFlags(wptr) & EZ_WIDGET_EMBEDER_BIT)
#define EZ_ClearWidgetEmbederFlag(wptr)    {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_EMBEDER_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetExpandFlag(wptr)          {EZ_WidgetFlags(wptr) |= EZ_WIDGET_EXPAND_BIT;}
#define EZ_GetWidgetExpandFlag(wptr)          (EZ_WidgetFlags(wptr) & EZ_WIDGET_EXPAND_BIT)
#define EZ_ClearWidgetExpandFlag(wptr)        {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_EXPAND_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetNonActiveFlag(wptr)       {EZ_WidgetFlags(wptr) |= EZ_WIDGET_NONACTIVE_BIT;}
#define EZ_GetWidgetNonActiveFlag(wptr)       (EZ_WidgetFlags(wptr) & EZ_WIDGET_NONACTIVE_BIT)
#define EZ_ClearWidgetNonActiveFlag(wptr)     {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_NONACTIVE_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetTransientFlag(wptr)       {EZ_WidgetFlags(wptr) |= EZ_WIDGET_TRANSIENT_BIT;}
#define EZ_GetWidgetTransientFlag(wptr)       (EZ_WidgetFlags(wptr) & EZ_WIDGET_TRANSIENT_BIT)
#define EZ_ClearWidgetTransientFlag(wptr)     {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_TRANSIENT_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetDisabledFlag(wptr)        {EZ_WidgetFlags(wptr) |= EZ_WIDGET_DISABLED_BIT;}
#define EZ_GetWidgetDisabledFlag(wptr)        (EZ_WidgetFlags(wptr) & EZ_WIDGET_DISABLED_BIT)
#define EZ_ClearWidgetDisabledFlag(wptr)      {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_DISABLED_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetFreezedFlag(wptr)         {EZ_WidgetFlags(wptr) |= EZ_WIDGET_FREEZED_BIT;}
#define EZ_GetWidgetFreezedFlag(wptr)         (EZ_WidgetFlags(wptr) & EZ_WIDGET_FREEZED_BIT)
#define EZ_ClearWidgetFreezedFlag(wptr)       {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_FREEZED_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetHighlightFlag(wptr)       {EZ_WidgetFlags(wptr) |= EZ_WIDGET_HIGHLIGHT_BIT;}
#define EZ_GetWidgetHighlightFlag(wptr) \
          (EZ_HighlightEnabled && (EZ_WidgetFlags(wptr) & EZ_WIDGET_HIGHLIGHT_BIT))
#define EZ_ClearWidgetHighlightFlag(wptr)     {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_HIGHLIGHT_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetXSetFlag(wptr)            {EZ_WidgetFlags(wptr) |= EZ_WIDGET_X_SET_BIT;}
#define EZ_SetWidgetYSetFlag(wptr)            {EZ_WidgetFlags(wptr) |= EZ_WIDGET_Y_SET_BIT;}
#define EZ_GetWidgetXSetFlag(wptr)            (EZ_WidgetFlags(wptr) & EZ_WIDGET_X_SET_BIT)
#define EZ_GetWidgetYSetFlag(wptr)            (EZ_WidgetFlags(wptr) & EZ_WIDGET_Y_SET_BIT)
#define EZ_ClearWidgetXSetFlag(wptr)          {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_X_SET_BIT;}
#define EZ_ClearWidgetYSetFlag(wptr)          {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_Y_SET_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetXYSetFlag(wptr)           {EZ_WidgetFlags(wptr) |= EZ_WIDGET_XY_SET_BITS;}
#define EZ_GetWidgetXYSetFlag(wptr)           (EZ_WidgetFlags(wptr) & EZ_WIDGET_XY_SET_BITS)
#define EZ_ClearWidgetXYSetFlag(wptr)         (EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_XY_SET_BITS)
/***********************************************************************************************/
#define EZ_SetWidgetWidthSetFlag(wptr)        {EZ_WidgetFlags(wptr) |= EZ_WIDGET_WIDTH_SET_BIT;}
#define EZ_SetWidgetHeightSetFlag(wptr)       {EZ_WidgetFlags(wptr) |= EZ_WIDGET_HEIGHT_SET_BIT;}
#define EZ_SetWidgetSizeSetFlag(wptr) \
{EZ_WidgetFlags(wptr) |= (EZ_WIDGET_WIDTH_SET_BIT | EZ_WIDGET_HEIGHT_SET_BIT);}
#define EZ_ClearWidgetWidthSetFlag(wptr)        {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_WIDTH_SET_BIT;}
#define EZ_ClearWidgetHeightSetFlag(wptr)       {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_HEIGHT_SET_BIT;}
#define EZ_GetWidgetWidthSetFlag(wptr)        (EZ_WidgetFlags(wptr) & EZ_WIDGET_WIDTH_SET_BIT)
#define EZ_GetWidgetHeightSetFlag(wptr)       (EZ_WidgetFlags(wptr) & EZ_WIDGET_HEIGHT_SET_BIT)
#define EZ_GetWidgetSizeSetFlag(wptr)  (EZ_GetWidgetWidthSetFlag(wptr) &&  EZ_GetWidgetHeightSetFlag(wptr))
/***********************************************************************************************/
#define EZ_SetWidgetWidthHintFlag(wptr)        {EZ_WidgetFlags(wptr) |= EZ_WIDGET_WIDTH_HINT_BIT;}
#define EZ_SetWidgetHeightHintFlag(wptr)       {EZ_WidgetFlags(wptr) |= EZ_WIDGET_HEIGHT_HINT_BIT;}
#define EZ_GetWidgetWidthHintFlag(wptr)        (EZ_WidgetFlags(wptr) & EZ_WIDGET_WIDTH_HINT_BIT)
#define EZ_GetWidgetHeightHintFlag(wptr)       (EZ_WidgetFlags(wptr) & EZ_WIDGET_HEIGHT_HINT_BIT)
#define EZ_ClearWidgetWidthHintFlag(wptr)      {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_WIDTH_HINT_BIT;}
#define EZ_ClearWidgetHeightHintFlag(wptr)     {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_HEIGHT_HINT_BIT;}
/***********************************************************************************************/
#define EZ_SetWidgetSizeComputedFlag(wptr)   {EZ_WidgetFlags(wptr) |= EZ_WIDGET_SIZE_COMPUTED_BIT;}
#define EZ_GetWidgetSizeComputedFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_SIZE_COMPUTED_BIT)
#define EZ_ClearWidgetSizeComputedFlag(wptr) \
{\
   EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_SIZE_COMPUTED_BIT;\
   EZ_WidgetFlags(wptr) |= EZ_WIDGET_SIZE_MAY_BE_CHANGED_BIT;\
}
#define EZ_WidgetSizeMayBeChangedFlag(wptr)  (EZ_WidgetFlags(wptr) & EZ_WIDGET_SIZE_MAY_BE_CHANGED_BIT)
#define EZ_SetWidgetSizeMayBeChangedFlag(wptr) \
{ EZ_WidgetFlags(wptr) |= EZ_WIDGET_SIZE_MAY_BE_CHANGED_BIT; }
#define EZ_ClearWidgetSizeMayBeChangedFlag(wptr) \
{ EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_SIZE_MAY_BE_CHANGED_BIT;}
/***********************************************************************************************/
#define EZ_GetDoNotPropagateFlag(wptr)     (EZ_WidgetFlags(wptr) & (EZ_WIDGET_DO_NOT_PROPAGATE_BIT))
#define EZ_SetDoNotPropagateFlag(wptr)     {EZ_WidgetFlags(wptr) |= EZ_WIDGET_DO_NOT_PROPAGATE_BIT;}
#define EZ_ClearDoNotPropagateFlag(wptr)   {EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_DO_NOT_PROPAGATE_BIT;}
/***********************************************************************************************/
#define EZ_GetWidgetEmbededFlag(wptr)    (EZ_WidgetFlags(wptr) & EZ_WIDGET_EMBEDED_BIT)
#define EZ_SetWidgetEmbededFlag(wptr)    (EZ_WidgetFlags(wptr) |= EZ_WIDGET_EMBEDED_BIT)
#define EZ_ClearWidgetEmbededFlag(wptr)  (EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_EMBEDED_BIT)
/***********************************************************************************************/
#define EZ_GetWidgetIsCloneFlag(wptr)    (EZ_WidgetFlags(wptr) & EZ_WIDGET_IS_CLONE_BIT)
#define EZ_SetWidgetIsCloneFlag(wptr)    (EZ_WidgetFlags(wptr) |= EZ_WIDGET_IS_CLONE_BIT)
#define EZ_ClearWidgetIsCloneFlag(wptr)  (EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_IS_CLONE_BIT)
/***********************************************************************************************/
#define EZ_GetWidgetShapeShiftFlag(wptr)    (EZ_WidgetFlags(wptr) & EZ_WIDGET_SHAPE_SHIFT)
#define EZ_SetWidgetShapeShiftFlag(wptr)    (EZ_WidgetFlags(wptr) |= EZ_WIDGET_SHAPE_SHIFT)
#define EZ_ClearWidgetShapeShiftFlag(wptr)  (EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_SHAPE_SHIFT)
/***********************************************************************************************/
#define EZ_GetWidgetHighlightBGFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_HIGHLIGHT_BG_BIT)
#define EZ_SetWidgetHighlightBGFlag(wptr)    (EZ_WidgetFlags(wptr) |= EZ_WIDGET_HIGHLIGHT_BG_BIT)
#define EZ_ClearWidgetHighlightBGFlag(wptr)  (EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_HIGHLIGHT_BG_BIT)
/***********************************************************************************************/
#define EZ_GetWidgetHighlightFGFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_HIGHLIGHT_FG_BIT)
#define EZ_SetWidgetHighlightFGFlag(wptr)    (EZ_WidgetFlags(wptr) |= EZ_WIDGET_HIGHLIGHT_FG_BIT)
#define EZ_ClearWidgetHighlightFGFlag(wptr)  (EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_HIGHLIGHT_FG_BIT)
/***********************************************************************************************/
#define EZ_GetWidgetHCLRUsedFlag(wptr)   (EZ_WidgetFlags(wptr) & EZ_WIDGET_HCLR_USED_BIT)
#define EZ_SetWidgetHCLRUsedFlag(wptr)    (EZ_WidgetFlags(wptr) |= EZ_WIDGET_HCLR_USED_BIT)
#define EZ_ClearWidgetHCLRUsedFlag(wptr)  (EZ_WidgetFlags(wptr) &= ~EZ_WIDGET_HCLR_USED_BIT)
/***********************************************************************************************/
#define EZ_GetHighlightFGBG(wptr) \
    (EZ_WidgetFlags(wptr) & (EZ_WIDGET_HIGHLIGHT_FG_BIT|EZ_WIDGET_HIGHLIGHT_BG_BIT))
/***********************************************************************************************/
#define EZ_InstallHighlightColor(_wgt_) \
   if(EZ_GetWidgetHCLRUsedFlag(_wgt_)==0) {\
         int _hfbgflag_= EZ_GetHighlightFGBG(_wgt_);\
         if(_hfbgflag_)\
           {\
             unsigned long _fgbg_;\
             if(_hfbgflag_ & EZ_WIDGET_HIGHLIGHT_FG_BIT)\
               {\
                 _fgbg_ = EZ_WidgetForeground(_wgt_);\
                 EZ_WidgetForeground(_wgt_) = EZ_WidgetHighlightFG(_wgt_);\
                 EZ_WidgetHighlightFG(_wgt_) = _fgbg_; \
               }\
             if(_hfbgflag_ & EZ_WIDGET_HIGHLIGHT_BG_BIT)\
               {\
                 _fgbg_ = EZ_WidgetBackground(_wgt_);\
                 EZ_WidgetBackground(_wgt_) = EZ_WidgetHighlightBG(_wgt_);\
                 EZ_WidgetHighlightBG(_wgt_) = _fgbg_;\
               }\
  	     {EZ_SetWidgetHCLRUsedFlag(_wgt_);}\
           }\
 }
#define EZ_UninstallHighlightColor(_wgt_) \
   if(EZ_GetWidgetHCLRUsedFlag(_wgt_)!=0) {\
         int _hfbgflag_= EZ_GetHighlightFGBG(_wgt_);\
         if(_hfbgflag_)\
           {\
             unsigned long _fgbg_;\
             if(_hfbgflag_ & EZ_WIDGET_HIGHLIGHT_FG_BIT)\
               {\
                 _fgbg_ = EZ_WidgetForeground(_wgt_);\
                 EZ_WidgetForeground(_wgt_) = EZ_WidgetHighlightFG(_wgt_);\
                 EZ_WidgetHighlightFG(_wgt_) = _fgbg_; \
               }\
             if(_hfbgflag_ & EZ_WIDGET_HIGHLIGHT_BG_BIT)\
               {\
                 _fgbg_ = EZ_WidgetBackground(_wgt_);\
                 EZ_WidgetBackground(_wgt_) = EZ_WidgetHighlightBG(_wgt_);\
                 EZ_WidgetHighlightBG(_wgt_) = _fgbg_;\
               }\
  	     {EZ_ClearWidgetHCLRUsedFlag(_wgt_);}\
           }\
 }

/***********************************************************************
 ***                                                                 ***
 ***                 Unknown Widget, not really useful.              ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct EZ_UnknownWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /* privite storage */

} EZ_UnknownWidget;

/***********************************************************************
 ***                                                                 ***
 ***                        Frame Widget                             ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct EZ_FrameWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------------------------------------
   * Frame data. For special purpose frames such as labeled frame
   *-------------------------------------------------------------*/
  char              *label;           /* for labeled frame,      */
  int               label_length;     /* length of label         */
  int               line_length;      /* length of a dpyed line  */
  int               nlines;           /* total # of lines        */
  int               twidth;           /* label width in pixels   */
  int               theight;          /* label height in pixels  */
  int               ascent;           /* font ascent             */
  int               xindent;          /* internal border indent  */
  int               yindent;          /* internal border indent  */

  int               lpad;             /* internal label padding  */
  int               mwidth;           
  int               mheight;          

  int               internalBdWidth;  /* internal border width   */
  int               brokenIntBdLength;/* length of broken int bd */
  int               wadjust;          /* width adjustment        */

  char              internalBdType;   /* internal border type    */
  char              haslabel;         /* do we have a label ?    */
  char              draghandle;       /* dragHandle type         */
  char              dockable;         /* detachable, dockable    */
  char              dtmp;    
  char              cursorChanged;
  char              savedBdType;
  char              savedBdWidth;
  char              undockable;
  char              iframe;
  char              labelOrientation;
  char              pad;

  int               saveX, saveY;
  int               dhxl, dhxr, dhy;  /* dragHandle adjustment */
  /* for menubar */
  union EZ_WWindow_   *lastItem; 

} EZ_FrameWidget;

#define EZ_MBarLastItem(wptr)           ((wptr)->mbar.lastItem)

#define EZ_FrameLPad(wptr)              ((wptr)->frame.lpad)
#define EZ_FrameBarOrientation(wptr)    ((wptr)->frame.labelOrientation)
#define EZ_FrameMWidth(wptr)            ((wptr)->frame.mwidth)
#define EZ_FrameMHeight(wptr)           ((wptr)->frame.mheight)
#define EZ_FrameInternal(wptr)          ((wptr)->frame.iframe)
#define EZ_FrameSavedBdType(wptr)       ((wptr)->frame.savedBdType)
#define EZ_FrameSavedBdWidth(wptr)      ((wptr)->frame.savedBdWidth)
#define EZ_FrameSaveX(wptr)             ((wptr)->frame.saveX)
#define EZ_FrameSaveY(wptr)             ((wptr)->frame.saveY)
#define EZ_FrameDetachable(wptr)        ((wptr)->frame.dockable)
#define EZ_FrameDockable(wptr)          ((wptr)->frame.dockable)
#define EZ_FrameUndockable(wptr)        ((wptr)->frame.undockable)
#define EZ_FrameCursorChanged(wptr)     ((wptr)->frame.cursorChanged)
#define EZ_FrameDHXL(wptr)              ((wptr)->frame.dhxl)
#define EZ_FrameDHXR(wptr)              ((wptr)->frame.dhxr)
#define EZ_FrameDHY(wptr)               ((wptr)->frame.dhy)
#define EZ_FrameDragHandle(wptr)        ((wptr)->frame.draghandle)
#define EZ_FrameDragTmp(wptr)           ((wptr)->frame.dtmp)
#define EZ_FrameIntBdWidth(wptr)        ((wptr)->frame.internalBdWidth)
#define EZ_FrameIntBdType(wptr)         ((wptr)->frame.internalBdType)
#define EZ_FrameBrokenIntBdLength(wptr) ((wptr)->frame.brokenIntBdLength)

#define EZ_FramePixmap(wptr)          ((wptr)->common.pixmap)
#define EZ_FrameBackground(wptr)      ((wptr)->common.background) 
#define EZ_FrameForeground(wptr)      ((wptr)->common.foreground) 
#define EZ_FrameFont(wptr)            ((wptr)->common.font)
#define EZ_FrameLabel(wptr)           ((wptr)->frame.label)
#define EZ_FrameLabelLength(wptr)     ((wptr)->frame.label_length)
#define EZ_FrameLineLength(wptr)      ((wptr)->frame.line_length)
#define EZ_FrameNLines(wptr)          ((wptr)->frame.nlines)
#define EZ_FrameTWidth(wptr)          ((wptr)->frame.twidth)
#define EZ_FrameTHeight(wptr)         ((wptr)->frame.theight)
#define EZ_FrameFontAscent(wptr)      ((wptr)->frame.ascent)
#define EZ_FrameXIndent(wptr)         ((wptr)->frame.xindent)
#define EZ_FrameYIndent(wptr)         ((wptr)->frame.yindent)
#define EZ_FrameHasLabel(wptr)        ((wptr)->frame.haslabel)
#define EZ_FrameWAdjust(wptr)         ((wptr)->frame.wadjust)
#define EZ_FrameWAdjust(wptr)         ((wptr)->frame.wadjust)

/***********************************************************************
 ***                                                                 ***
 ***                        Label Widget                             ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct EZ_LabelWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-----------------------------
   * Private data. Don't need much
   *----------------------------*/
  char              *label;           /* label string            */
  int               label_length;     /* length of label         */
  int               line_length;      /* length of a dpyed line  */
  int               nlines;           /* total # of lines        */
  int               twidth;           /* label width in pixels   */
  int               theight;          /* label height in pixels  */
  int               ascent;           /*  font ascent            */
  int               mwidth;           
  int               mheight;          
  
  int               ismenutitle;      /* is it a menu title ?   */
  int               alwayshighlight;

  /* for free label, icon        */
  int               pixW;
  int               pixH;
  int               ibdWidth;
  char              isLabeledIcon;
  char              movable;    
  char              dontTakeFocus;
  char              maskDirty;
  char              maskNotDirty;
  char              textMaskDirty;
  char              isDrag;
  char              iconKind;
  char              ibdType;
  char              shapeSet;
  char              constrained; /* for free label */
  char              cpad;

  Pixmap            textMask;
  int               underline;  
  int               modifiers; 
  char              *shortcut;      
  union EZ_WWindow_ *menu;         /* used in menubar items*/
  union EZ_WWindow_ *copy;         /* used in menubar items*/
  union EZ_WWindow_ *initiator;    /* used in optEntry popup listbox */

  int               arrowType;     /* used in nwlabel      */
  short             txtPos[4];
} EZ_LabelWidget;

#define EZ_LabelMWidth(wptr)            ((wptr)->label.mwidth)
#define EZ_LabelMHeight(wptr)           ((wptr)->label.mheight)
#define EZ_IToplevelConstrained(wptr) ((wptr)->label.constrained)
#define EZ_IToplevelTextGeom(wptr)    ((wptr)->label.txtPos)
#define EZ_IToplevelInitiator(wptr)   ((wptr)->label.initiator)
#define EZ_IconShapeSet(wptr)         ((wptr)->label.shapeSet)
#define EZ_LabelIBorderType(wptr)     ((wptr)->label.ibdType)
#define EZ_LabelIBorderWidth(wptr)    ((wptr)->label.ibdWidth)
#define EZ_LabelArrowType(wptr)       ((wptr)->label.arrowType)
#define EZ_LabelTextMask(wptr)        ((wptr)->label.textMask)
#define EZ_LabelTextMaskDirty(wptr)   ((wptr)->label.textMaskDirty)
#define EZ_LabelPixmap(wptr)          ((wptr)->common.pixmap)
#define EZ_LabelFont(wptr)            ((wptr)->common.font)
#define EZ_LabelForeground(wptr)      ((wptr)->common.foreground) 
#define EZ_LabelBackground(wptr)      ((wptr)->common.background) 
#define EZ_LabelString(wptr)          ((wptr)->label.label)
#define EZ_LabelStringLength(wptr)    ((wptr)->label.label_length)
#define EZ_LabelLineLength(wptr)      ((wptr)->label.line_length)
#define EZ_LabelNLines(wptr)          ((wptr)->label.nlines)
#define EZ_LabelTWidth(wptr)          ((wptr)->label.twidth)
#define EZ_LabelTHeight(wptr)         ((wptr)->label.theight)
#define EZ_LabelFontAscent(wptr)      ((wptr)->label.ascent)
#define EZ_LabelMovable(wptr)         ((wptr)->label.movable)
#define EZ_LabelAlwaysHighlight(wptr) ((wptr)->label.alwayshighlight)
#define EZ_IsNWFrame(wptr)            ((wptr)->label.alwayshighlight)
#define EZ_LabelMotionCallBack(wptr)  ((wptr)->common.motionCallbacks)
#define EZ_LabelDontTakeFocus(wptr)   ((wptr)->label.dontTakeFocus)
#define EZ_LabelIsMenuTitle(wptr)     ((wptr)->label.ismenutitle)
#define EZ_LabelIsLabeledIcon(wptr)   ((wptr)->label.isLabeledIcon)
#define EZ_LabelMaskDirty(wptr)       ((wptr)->label.maskDirty)
#define EZ_LabelMaskNotDirty(wptr)    ((wptr)->label.maskNotDirty)
#define EZ_LabelPixW(wptr)            ((wptr)->label.pixW)
#define EZ_LabelPixH(wptr)            ((wptr)->label.pixH)
#define EZ_LabelModifiers(wptr)       ((wptr)->label.modifiers)
#define EZ_LabelShortcut(wptr)        ((wptr)->label.shortcut)
#define EZ_LabelUnderline(wptr)       ((wptr)->label.underline)
#define EZ_LabelUnderLine(wptr)       ((wptr)->label.underline)
#define EZ_LabelMenu(wptr)            ((wptr)->label.menu)
#define EZ_NWLabelMenu(wptr)          ((wptr)->label.menu)
#define EZ_LabelCopy(wptr)            ((wptr)->label.copy)
#define EZ_NWLabelCopy(wptr)          ((wptr)->label.copy)
#define EZ_LabelIsDrag(wptr)          ((wptr)->label.isDrag)
#define EZ_IconKind(wptr)             ((wptr)->label.iconKind)

/***********************************************************************
 ***                                                                 ***
 ***                        Normal Button                            ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_ButtonWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-----------------------------------------------
   * Button data.
   *----------------------------------------------*/
  char              *label;           /* text on button          */

  int               label_length;     /* length of label         */
  int               line_length;      /* length of a line        */
  int               nlines;           /* number of lines in label*/
  int               twidth;           /* label width in pixels   */
  int               theight;          /* label height in pixels  */
  int               ascent;           /*  font ascent            */
  int               mwidth;           
  int               mheight;          

  char              *label2;          /* second label, for menu  */
  int               label2_length;    /* length of label2        */
  int               label2_width;     /* length of label2 in pix */
  int               underline;        /* for shortcut character  */
  int               modifiers;        /* menu-shortcut stuff     */
  char              *shortcut;      

  char              pressed;          /* set if button is pressed*/
  char              setbdr;           /* border has been modified? */
  char              nohighlight;      /* set if never highlight  */
  char              arrowButton;      /* used only in arrow button */
  
  /* special field used in NoteBookPage */
  union EZ_WWindow_ *companion;       /* special companion widget*/
  union EZ_WWindow_ *fsb;
  union EZ_WWindow_ *copy;

  void             *optEntry;         /* used only in arrow button */
  int               arrowType;        /* used only in arrow button */
  
} EZ_ButtonWidget;

#define EZ_ButtonMWidth(wptr)            ((wptr)->button.mwidth)
#define EZ_ButtonMHeight(wptr)           ((wptr)->button.mheight)

#define EZ_ArrowButtonType(wptr)        ((wptr)->button.arrowType)
#define EZ_ArrowButtonOptEntry(wptr)    ((wptr)->button.optEntry)
#define EZ_ButtonIsArrowButton(wptr)    ((wptr)->button.arrowButton)

#define EZ_ButtonFSB(wptr)            ((wptr)->button.fsb)
#define EZ_ButtonCompanion(wptr)      ((wptr)->button.companion)
#define EZ_ButtonLabel(wptr)          ((wptr)->button.label)
#define EZ_ButtonLabelLength(wptr)    ((wptr)->button.label_length)
#define EZ_ButtonFont(wptr)           ((wptr)->common.font)
#define EZ_ButtonPixmap(wptr)         ((wptr)->common.pixmap)
#define EZ_ButtonForeground(wptr)     ((wptr)->common.foreground)
#define EZ_ButtonBackground(wptr)     ((wptr)->common.background)
#define EZ_ButtonLineLength(wptr)     ((wptr)->button.line_length)
#define EZ_ButtonNLines(wptr)         ((wptr)->button.nlines)
#define EZ_ButtonTWidth(wptr)         ((wptr)->button.twidth)
#define EZ_ButtonTHeight(wptr)        ((wptr)->button.theight)
#define EZ_ButtonFontAscent(wptr)     ((wptr)->button.ascent)
#define EZ_ButtonLabel2(wptr)         ((wptr)->button.label2)
#define EZ_ButtonLabel2Length(wptr)   ((wptr)->button.label2_length)
#define EZ_ButtonLabel2Width(wptr)    ((wptr)->button.label2_width)
#define EZ_ButtonPressed(wptr)        ((wptr)->button.pressed)
#define EZ_ButtonModifiers(wptr)      ((wptr)->button.modifiers)
#define EZ_ButtonShortcut(wptr)       ((wptr)->button.shortcut)
#define EZ_ButtonUnderline(wptr)      ((wptr)->button.underline)
#define EZ_ButtonUnderLine(wptr)      ((wptr)->button.underline)
#define EZ_ButtonNoHighlight(wptr)    ((wptr)->button.nohighlight)
#define EZ_ButtonNoHighLight(wptr)    ((wptr)->button.nohighlight)
#define EZ_ButtonSetBorder(wptr)      ((wptr)->button.setbdr)
#define EZ_ButtonCopy(wptr)           ((wptr)->button.copy)

/***********************************************************************
 ***                                                                 ***
 ***                        Check Button                             ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_CButtonWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-----------------------------
   * Check Button data. 
   *----------------------------*/
  char              *label;           /* text on button          */
  int               label_length;     /* length of label         */
  int               line_length;      /* length of a line        */
  int               nlines;           /* number of lines in label*/
  int               twidth;           /* label width in pixels   */
  int               theight;          /* label height in pixels  */
  int               ascent;           /* font ascent             */
  int               mwidth;           
  int               mheight;          

  char              borderwidth;      /* border width of check   */
  char              borderstyle;      /* border style of check   */
  char              checksizeadjust;  /* check mark size adjust  */
  char              checkType;        /* type of indicator       */
  unsigned long     checkColor;       /* color of indicator when active */

  int               *vdat;
  /*
  int               value;      
  int               onvalue;    
  int               offvalue;   
  */

  char              *label2;          /* second label, for menu  */
  int               label2_length;    /* length of label2        */
  int               label2_width;     /* length of label2 in pix */
  int               underline;        /* for shortcut character  */
  int               modifiers;        /* menu-shortcut stuff     */
  char              *shortcut;      
  union EZ_WWindow_ *copy;

} EZ_CButtonWidget;

#define EZ_CButtonMWidth(wptr)         ((wptr)->cbutton.mwidth)
#define EZ_CButtonMHeight(wptr)        ((wptr)->cbutton.mheight)

#define EZ_CButtonLabel(wptr)          ((wptr)->cbutton.label)
#define EZ_CButtonLabelLength(wptr)    ((wptr)->cbutton.label_length)
#define EZ_CButtonPixmap(wptr)         ((wptr)->common.pixmap)
#define EZ_CButtonFont(wptr)           ((wptr)->common.font)
#define EZ_CButtonForeground(wptr)     ((wptr)->common.foreground)
#define EZ_CButtonBackground(wptr)     ((wptr)->common.background)
#define EZ_CButtonTWidth(wptr)         ((wptr)->cbutton.twidth)
#define EZ_CButtonTHeight(wptr)        ((wptr)->cbutton.theight)
#define EZ_CButtonFontAscent(wptr)     ((wptr)->cbutton.ascent)
#define EZ_CButtonLineLength(wptr)     ((wptr)->cbutton.line_length)
#define EZ_CButtonNLines(wptr)         ((wptr)->cbutton.nlines)
#define EZ_CButtonCheckBorderWidth(wptr)  ((wptr)->cbutton.borderwidth)
#define EZ_CButtonCheckBorderStyle(wptr)  ((wptr)->cbutton.borderstyle)
#define EZ_CButtonCheckSizeAdjust(wptr)   ((wptr)->cbutton.checksizeadjust)
#define EZ_CButtonCheckType(wptr)         ((wptr)->cbutton.checkType)
#define EZ_CButtonCheckColor(wptr)        ((wptr)->cbutton.checkColor)
#define EZ_CButtonData(wptr)              ((wptr)->cbutton.vdat)
#define EZ_CButtonVarValue(wptr)          (((wptr)->cbutton.vdat)[0])
#define EZ_CButtonVarOnValue(wptr)     (((wptr)->cbutton.vdat)[1])
#define EZ_CButtonVarOffValue(wptr)    (((wptr)->cbutton.vdat)[2])
#define EZ_CButtonSetOnValue(wptr)     (((wptr)->cbutton.vdat)[0] = ((wptr)->cbutton.vdat)[1])
#define EZ_CButtonSetOffValue(wptr)    (((wptr)->cbutton.vdat)[0] = ((wptr)->cbutton.vdat)[2])
#define EZ_CButtonOn(wptr)             (((wptr)->cbutton.vdat)[0] == ((wptr)->cbutton.vdat)[1])
#define EZ_CButtonCheck(wptr)          (((wptr)->cbutton.vdat)[0] == ((wptr)->cbutton.vdat)[1])

#define EZ_CButtonLabel2(wptr)         ((wptr)->cbutton.label2)
#define EZ_CButtonLabel2Length(wptr)   ((wptr)->cbutton.label2_length)
#define EZ_CButtonLabel2Width(wptr)    ((wptr)->cbutton.label2_width)
#define EZ_CButtonModifiers(wptr)      ((wptr)->cbutton.modifiers)
#define EZ_CButtonShortcut(wptr)       ((wptr)->cbutton.shortcut)
#define EZ_CButtonUnderline(wptr)      ((wptr)->cbutton.underline)
#define EZ_CButtonUnderLine(wptr)      ((wptr)->cbutton.underline)
#define EZ_CButtonCopy(wptr)           ((wptr)->cbutton.copy)

/***********************************************************************
 ***                                                                 ***
 ***                      Radio Button                               ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_RButtonList_ {
  union EZ_WWindow_       *rbutton;
  struct EZ_RButtonList_  *next;
} EZ_RButtonList;

typedef struct EZ_RBGS_ {
  int              var;
  int              id;
  EZ_RButtonList   *list;
  struct EZ_RBGS_  *next;
} EZ_RButtonGpStruct;
  
typedef struct EZ_RButtonWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-----------------------------
   * Radio Button data. 
   *----------------------------*/
  char              *label;           /* text on button          */
  int               label_length;     /* length of label         */
  int               line_length;      /* length of a line        */
  int               nlines;           /* number of lines in label*/
  int               twidth;           /* label width in pixels   */
  int               theight;          /* label height in pixels  */
  int               mwidth;          
  int               mheight;         

  int               ascent;           /*  font ascent            */
  char              borderwidth;      /* border width of check   */
  char              borderstyle;      /* border style of check   */
  char              checksizeadjust;  /* check mark size adjust  */
  char              checkType;        /* indicator type          */
  unsigned long     checkColor;       /* color of indicator when active */
  int               *var;             /* the address of check var*/
  int               value;            /* val of var when checked */
  EZ_RButtonGpStruct *group;          /* the group the button belongs to */
  char              *label2;          /* second label, for menu  */
  int               label2_length;    /* length of label2        */
  int               label2_width;     /* length of label2 in pix */
  int               underline;        /* for shortcut character  */
  int               modifiers;        /* menu-shortcut stuff     */
  char              *shortcut;      
  union EZ_WWindow_ *copy;
} EZ_RButtonWidget;

#define EZ_RButtonMWidth(wptr)            ((wptr)->rbutton.mwidth)
#define EZ_RButtonMHeight(wptr)           ((wptr)->rbutton.mheight)

#define EZ_RButtonLabel(wptr)          ((wptr)->rbutton.label)
#define EZ_RButtonLabelLength(wptr)    ((wptr)->rbutton.label_length)
#define EZ_RButtonPixmap(wptr)         ((wptr)->common.pixmap)
#define EZ_RButtonFont(wptr)           ((wptr)->common.font)
#define EZ_RButtonForeground(wptr)     ((wptr)->common.foreground)
#define EZ_RButtonBackground(wptr)     ((wptr)->common.background)
#define EZ_RButtonTWidth(wptr)         ((wptr)->rbutton.twidth)
#define EZ_RButtonTHeight(wptr)        ((wptr)->rbutton.theight)
#define EZ_RButtonFontAscent(wptr)     ((wptr)->rbutton.ascent)
#define EZ_RButtonLineLength(wptr)     ((wptr)->rbutton.line_length)
#define EZ_RButtonNLines(wptr)         ((wptr)->rbutton.nlines)
#define EZ_RButtonCheckBorderWidth(wptr)  ((wptr)->rbutton.borderwidth)
#define EZ_RButtonCheckBorderStyle(wptr)  ((wptr)->rbutton.borderstyle)
#define EZ_RButtonCheckSizeAdjust(wptr)   ((wptr)->rbutton.checksizeadjust)
#define EZ_RButtonChecked(wptr)        (*((wptr)->rbutton.var) == (wptr)->rbutton.value)
#define EZ_RButtonOn(wptr)             (*((wptr)->rbutton.var) == (wptr)->rbutton.value)
#define EZ_RButtonSetValue(wptr)       (*((wptr)->rbutton.var) = (wptr)->rbutton.value)
#define EZ_RButtonGroup(wptr)          ((wptr)->rbutton.group)
#define EZ_RButtonVarValue(wptr)       ((wptr)->rbutton.value)
#define EZ_RButtonVarPtr(wptr)         ((wptr)->rbutton.var)
#define EZ_RButtonCheckType(wptr)      ((wptr)->rbutton.checkType)
#define EZ_RButtonCheckColor(wptr)     ((wptr)->rbutton.checkColor)

#define EZ_RButtonLabel2(wptr)         ((wptr)->rbutton.label2)
#define EZ_RButtonLabel2Length(wptr)   ((wptr)->rbutton.label2_length)
#define EZ_RButtonLabel2Width(wptr)    ((wptr)->rbutton.label2_width)
#define EZ_RButtonModifiers(wptr)      ((wptr)->rbutton.modifiers)
#define EZ_RButtonShortcut(wptr)       ((wptr)->rbutton.shortcut)
#define EZ_RButtonUnderline(wptr)      ((wptr)->rbutton.underline)
#define EZ_RButtonUnderLine(wptr)      ((wptr)->rbutton.underline)
#define EZ_RButtonCopy(wptr)           ((wptr)->rbutton.copy)
/***********************************************************************
 ***                                                                 ***
 ***                     MenuButton                                  ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_MButtonWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-----------------------------
   * Menu Button data. 
   *----------------------------*/
  char              *label;           /* text on button          */
  int               label_length;     /* length of label         */
  int               line_length;      /* length of a line        */
  int               nlines;           /* number of lines in label*/
  int               twidth;           /* label width in pixels   */
  int               theight;          /* label height in pixels  */
  int               ascent;           /*  font ascent            */
  int               mwidth;         
  int               mheight;        

  char              *label2;          /* second label, for menu  */
  int               label2_length;    /* length of label2        */
  int               label2_width;     /* length of label2 in pix */

  int               underline;        /* for shortcut character  */
  int               modifiers;        /* menu-shortcut stuff     */
  char              *shortcut;      
  
  union EZ_WWindow_  *menu;           /* the menu                */
  union EZ_WWindow_  *lastSelection;  /* last selected item      */

  char              borderwidth;      /* border width of check   */
  char              borderstyle;      /* border style of check   */
  char              checksizeadjust;  /* check mark size adjust  */
  char              checkType;        /* type of indicator       */
  unsigned long     checkColor;       /* color of indicator when active */
  
} EZ_MButtonWidget;

#define EZ_MButtonMWidth(wptr)            ((wptr)->mbutton.mwidth)
#define EZ_MButtonMHeight(wptr)           ((wptr)->mbutton.mheight)

#define EZ_MButtonLabel(wptr)          ((wptr)->mbutton.label)
#define EZ_MButtonLabelLength(wptr)    ((wptr)->mbutton.label_length)
#define EZ_MButtonPixmap(wptr)         ((wptr)->common.pixmap)
#define EZ_MButtonFont(wptr)           ((wptr)->common.font)
#define EZ_MButtonForeground(wptr)     ((wptr)->common.foreground)
#define EZ_MButtonBackground(wptr)     ((wptr)->common.background)
#define EZ_MButtonTWidth(wptr)         ((wptr)->mbutton.twidth)
#define EZ_MButtonTHeight(wptr)        ((wptr)->mbutton.theight)
#define EZ_MButtonFontAscent(wptr)     ((wptr)->mbutton.ascent)
#define EZ_MButtonLineLength(wptr)     ((wptr)->mbutton.line_length)
#define EZ_MButtonNLines(wptr)         ((wptr)->mbutton.nlines)

#define EZ_MButtonLabel2(wptr)         ((wptr)->mbutton.label2)
#define EZ_MButtonLabel2Length(wptr)   ((wptr)->mbutton.label2_length)
#define EZ_MButtonLabel2Width(wptr)    ((wptr)->mbutton.label2_width)
#define EZ_MButtonUnderLine(wptr)      ((wptr)->mbutton.underline)
#define EZ_MButtonUnderline(wptr)      ((wptr)->mbutton.underline)
#define EZ_MButtonModifiers(wptr)      ((wptr)->mbutton.modifiers)
#define EZ_MButtonShortcut(wptr)       ((wptr)->mbutton.shortcut)
#define EZ_MButtonTheMenu(wptr)        ((wptr)->mbutton.menu)
#define EZ_MButtonLSItem(wptr)         ((wptr)->mbutton.lastSelection)

#define EZ_MButtonCheckType(wptr)         ((wptr)->mbutton.checkType)
#define EZ_MButtonCheckColor(wptr)        ((wptr)->mbutton.checkColor)
#define EZ_MButtonCheckBorderWidth(wptr)  ((wptr)->mbutton.borderwidth)
#define EZ_MButtonCheckBorderStyle(wptr)  ((wptr)->mbutton.borderstyle)
#define EZ_MButtonCheckSizeAdjust(wptr)   ((wptr)->mbutton.checksizeadjust)

/***********************************************************************
 ***                                                                 ***
 ***                     Slider Widget                               ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_SliderWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-----------------------------
   * Slider data. 
   *----------------------------*/
  char              *label;           /* label if any            */

  int               label_length;     /* length of label         */
  int               twidth;           /* label width in pixels   */
  int               theight;          /* label height in pixels  */
  int               ascent;           /*  font ascent            */

  int               width;            /* width of slider         */
  int               length;           /* length of slider        */

  float             var;              /* tracking variable       */
  float             minvalue;         /* min value of slider     */
  float             maxvalue;         /* max value of slider     */
  float             factor;           /* from scrn coor to slider*/
  float             resolution;

  int               sliderposition;   /* center coor of slider   */
  int               sliderpositionmin;/* lowest coor of slider   */
  int               minx, maxx;       /* slider window           */
  int               miny, maxy;
  
  char              format[8];        /* printing format         */

  int               vwidth;           /* value width in pixels   */
  int               vwidtha;          /* value width in pixels   */
  int               vheight;          /* value height            */
  int               sliderlength;     /* length of slider        */  
  int               borderwidth;      /* of the little button    */

  unsigned char     dispvalue;        /* whether disply value    */
  unsigned char     borderstyle;      /* of the button           */
  unsigned char     widthset;
  unsigned char     noCvalue;         /* whther to display current slider value */
  unsigned char     style;
  unsigned char     highlight;
  unsigned char     tickSize;
  unsigned char     nTicks;
} EZ_SliderWidget;

#define EZ_SliderNoCValue(wptr)       ((wptr)->slider.noCvalue)
#define EZ_SliderShowTick(wptr)       ((wptr)->slider.nTicks)
#define EZ_SliderNTicks(wptr)         ((wptr)->slider.nTicks)
#define EZ_SliderTickSize(wptr)       ((wptr)->slider.tickSize)
#define EZ_SliderHighlight(wptr)      ((wptr)->slider.highlight)
#define EZ_SliderStyle(wptr)          ((wptr)->slider.style)
#define EZ_SliderLabel(wptr)          ((wptr)->slider.label)
#define EZ_SliderLabelLength(wptr)    ((wptr)->slider.label_length)
#define EZ_SliderFont(wptr)           ((wptr)->common.font)
#define EZ_SliderForeground(wptr)     ((wptr)->common.foreground)
#define EZ_SliderBackground(wptr)     ((wptr)->common.background)
#define EZ_SliderTWidth(wptr)         ((wptr)->slider.twidth)
#define EZ_SliderTHeight(wptr)        ((wptr)->slider.theight)
#define EZ_SliderFontAscent(wptr)     ((wptr)->slider.ascent)
#define EZ_SliderWidth(wptr)          ((wptr)->slider.width)
#define EZ_SliderLength(wptr)         ((wptr)->slider.length)
#define EZ_SliderPosition(wptr)       ((wptr)->slider.sliderposition)
#define EZ_SliderPositionMin(wptr)    ((wptr)->slider.sliderpositionmin)
#define EZ_SliderValue(wptr)          ((wptr)->slider.var)
#define EZ_SliderResolution(wptr)     ((wptr)->slider.resolution)
#define EZ_SliderMinValue(wptr)       ((wptr)->slider.minvalue)
#define EZ_SliderMaxValue(wptr)       ((wptr)->slider.maxvalue)
#define EZ_SliderFactor(wptr)         ((wptr)->slider.factor)
#define EZ_SliderFormat(wptr)         ((wptr)->slider.format)
#define EZ_SliderBorderStyle(wptr)    ((wptr)->slider.borderstyle)
#define EZ_SliderBorderWidth(wptr)    ((wptr)->slider.borderwidth)
#define EZ_SliderDisplayValue(wptr)   ((wptr)->slider.dispvalue)
#define EZ_SliderVWidth(wptr)         ((wptr)->slider.vwidth)
#define EZ_SliderVWidtha(wptr)        ((wptr)->slider.vwidtha)
#define EZ_SliderVHeight(wptr)        ((wptr)->slider.vheight)
#define EZ_SliderSliderLength(wptr)   ((wptr)->slider.sliderlength)
#define EZ_SliderMinx(wptr)           ((wptr)->slider.minx)
#define EZ_SliderMaxx(wptr)           ((wptr)->slider.maxx)
#define EZ_SliderMiny(wptr)           ((wptr)->slider.miny)
#define EZ_SliderMaxy(wptr)           ((wptr)->slider.maxy)
#define EZ_SliderWidthSet(wptr)       ((wptr)->slider.widthset)

/***********************************************************************
 ***                                                                 ***
 ***                     PopupMenu Item: Separator                   ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_MenuSeparator_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------
   *  Don't need much for a separator
   *---------------------------------*/
  union EZ_WWindow_ *copy; 
} EZ_MenuSeparator;

#define EZ_MenuSeparatorCopy(wptr)          ((wptr)->mseparator.copy)

/***********************************************************************
 ***                                                                 ***
 ***                     PopupMenu Item: SubMenu                     ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_MenuSubMenu_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------
   *  Walkthrough menu
   *---------------------------------*/
  char              *label;           /* text on button          */
  int               label_length;     /* length of label         */
  int               line_length;      /* length of a line        */
  int               nlines;           /* number of lines in label*/
  int               twidth;           /* label width in pixels   */
  int               theight;          /* label height in pixels  */
  int               ascent;           /*  font ascent            */
  int               mwidth;           
  int               mheight;          
  int               arrowsize;        /* size of arrow           */
  int               arrowbwidth;      /* borderwidth of arrow    */

  char              *label2;          /* second label, for menu  */
  int               label2_length;    /* length of label2        */
  int               label2_width;     /* length of label2 in pix */
  int               underline;        /* for shortcut character  */
  int               modifiers;        /* menu-shortcut stuff     */
  char              *shortcut;      

  union EZ_WWindow_ *menu;            /* the submenu             */
  union EZ_WWindow_ *copy; 

} EZ_MenuSubMenu;

#define EZ_SubMenuMWidth(wptr)       ((wptr)->msubmenu.mwidth)
#define EZ_SubMenuMHeight(wptr)      ((wptr)->msubmenu.mheight)

#define EZ_SubMenuLabel(wptr)        ((wptr)->msubmenu.label)
#define EZ_SubMenuLabelLength(wptr)  ((wptr)->msubmenu.label_length)
#define EZ_SubMenuLineLength(wptr)   ((wptr)->msubmenu.line_length)
#define EZ_SubMenuNLines(wptr)       ((wptr)->msubmenu.nlines)
#define EZ_SubMenuPixmap(wptr)       ((wptr)->common.pixmap)
#define EZ_SubMenuFont(wptr)         ((wptr)->common.font)
#define EZ_SubMenuForeground(wptr)   ((wptr)->common.foreground)
#define EZ_SubMenuBackground(wptr)   ((wptr)->common.background)
#define EZ_SubMenuTWidth(wptr)       ((wptr)->msubmenu.twidth)
#define EZ_SubMenuTHeight(wptr)      ((wptr)->msubmenu.theight)
#define EZ_SubMenuFontAscent(wptr)   ((wptr)->msubmenu.ascent)
#define EZ_SubMenuArrowSize(wptr)    ((wptr)->msubmenu.arrowsize)
#define EZ_SubMenuArrowBWidth(wptr)  ((wptr)->msubmenu.arrowbwidth)
#define EZ_SubMenuTheMenu(wptr)      ((wptr)->msubmenu.menu)

#define EZ_SubMenuLabel2(wptr)         ((wptr)->msubmenu.label2)
#define EZ_SubMenuLabel2Length(wptr)   ((wptr)->msubmenu.label2_length)
#define EZ_SubMenuLabel2Width(wptr)    ((wptr)->msubmenu.label2_width)
#define EZ_SubMenuUnderLine(wptr)      ((wptr)->msubmenu.underline)
#define EZ_SubMenuUnderline(wptr)      ((wptr)->msubmenu.underline)
#define EZ_SubMenuModifiers(wptr)      ((wptr)->msubmenu.modifiers)
#define EZ_SubMenuShortcut(wptr)       ((wptr)->msubmenu.shortcut)
#define EZ_SubMenuCopy(wptr)           ((wptr)->msubmenu.copy)

/***********************************************************************
 ***                                                                 ***
 ***                     Popup Menu                                  ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_PopupMenu_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block            */
  union EZ_WWindow_  *lastSelection;    /* last selected menu item */
  /* 6-15-97 */
  int  istearoff;                    
  union EZ_WWindow_  *tearlink;  
  union EZ_WWindow_  *companion;  
  /* tmp links */
  union EZ_WWindow_  *link1;  
  union EZ_WWindow_  *link2; 
  EZ_CallBack        setupF;
  void               *setupD;
  union EZ_WWindow_  *initiator;
  int                xoff, yoff;

} EZ_PopupMenu;

#define EZ_PopupInitiator(wptr)      ((wptr)->popupmenu.initiator)
#define EZ_PopupXoff(wptr)           ((wptr)->popupmenu.xoff)
#define EZ_PopupYoff(wptr)           ((wptr)->popupmenu.yoff)
#define EZ_PopupSetupF(wptr)         ((wptr)->popupmenu.setupF)
#define EZ_PopupSetupFD(wptr)        ((wptr)->popupmenu.setupD)
#define EZ_PopupLSItem(wptr)         ((wptr)->popupmenu.lastSelection)
#define EZ_PopupIsTearOff(wptr)      ((wptr)->popupmenu.istearoff)
#define EZ_PopupIsTearLink(wptr)     ((wptr)->popupmenu.tearlink)
#define EZ_PopupCompanion(wptr)      ((wptr)->popupmenu.companion)
#define EZ_PopupLink1(wptr)          ((wptr)->popupmenu.link1)
#define EZ_PopupLink2(wptr)          ((wptr)->popupmenu.link2)

/***********************************************************************
 ***                                                                 ***
 ***                     Tear Off  MenuItem                          ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_MenuTearOffBar_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block            */
  union EZ_WWindow_  *tearedmenu; 
} EZ_MenuTearOffBar;

#define EZ_MenuTearOffBarMenu(wptr)     ((wptr)->tearoffbar.tearedmenu)

/***********************************************************************
 ***                                                                 ***
 ***                     Entry                                       ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_EntryWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------
   *  Data for Entry
   *---------------------------------*/
  char              *data;            /* storage for entry       */
  int               data_size;        /* size of data            */

  int               line_length;      /* size in characters      */
  int               twidth;           /* width in pixels         */
  int               theight;          /* font height in pixels   */
  int               ascent;           /* font ascent             */

  int               bgn;              /* begin index, displayed  */
  int               end;              /* end index,              */
  
  unsigned long     selBG;            /* highlight selection     */
  unsigned long     selFG;            /* highlight selection     */

  int               selectionbgn;     /* selected text bgn       */
  int               selectionend;     /* selected text end       */

  int               marker;           /* current marker          */

  int               position;         /* current cursor position */
  int               lastchar;         /* number of chars on entry*/

  union EZ_WWindow_ *optEntry;        /* is an optional entry ? */ 
  union EZ_WWindow_ *fse;
  union EZ_WWindow_ *spin;

  char              selectionflag; 
  char              editable;
  char              secrecy;
  /* for optional entry */
  char              setHistory;       /* remember history ?     */
  int               owidth;
  int               wadjust;
  int               hadjust;
  unsigned long     textBG;
  
  char              modified;
  char              cpad[3];

} EZ_EntryWidget;

#define EZ_EntryModified(wptr)         ((wptr)->entry.modified)
#define EZ_EntryTextBG(wptr)           ((wptr)->entry.textBG)
#define EZ_EntryData(wptr)             ((wptr)->entry.data)
#define EZ_EntryDataSize(wptr)         ((wptr)->entry.data_size)
#define EZ_EntryForeground(wptr)       ((wptr)->common.foreground)
#define EZ_EntryBackground(wptr)       ((wptr)->common.background)
#define EZ_EntryFont(wptr)             ((wptr)->common.font)
#define EZ_EntryLineLength(wptr)       ((wptr)->entry.line_length)
#define EZ_EntryTWidth(wptr)           ((wptr)->entry.twidth)
#define EZ_EntryTHeight(wptr)          ((wptr)->entry.theight)
#define EZ_EntryFontAscent(wptr)       ((wptr)->entry.ascent)
#define EZ_EntryLastChar(wptr)         ((wptr)->entry.lastchar)
#define EZ_EntryBgnIndex(wptr)         ((wptr)->entry.bgn)
#define EZ_EntryEndIndex(wptr)         ((wptr)->entry.end)
#define EZ_EntryPosition(wptr)         ((wptr)->entry.position)
#define EZ_EntrySelectionBG(wptr)      ((wptr)->entry.selBG)
#define EZ_EntrySelectionFG(wptr)      ((wptr)->entry.selFG)
#define EZ_EntrySelectionBgn(wptr)     ((wptr)->entry.selectionbgn)
#define EZ_EntrySelectionEnd(wptr)     ((wptr)->entry.selectionend)
#define EZ_EntrySelectionFlag(wptr)    ((wptr)->entry.selectionflag)
#define EZ_EntryRemberHistory(wptr)    ((wptr)->entry.setHistory)
#define EZ_EntryEditable(wptr)         ((wptr)->entry.editable)
#define EZ_EntryMarker(wptr)           ((wptr)->entry.marker)
#define EZ_EntryOptEntry(wptr)         ((wptr)->entry.optEntry)
#define EZ_EntryFSE(wptr)              ((wptr)->entry.fse)
#define EZ_EntrySpin(wptr)             ((wptr)->entry.spin)
#define EZ_EntrySecrecy(wptr)          ((wptr)->entry.secrecy)
#define EZ_EntryOWidth(wptr)           ((wptr)->entry.owidth)
#define EZ_EntryWAdjust(wptr)          ((wptr)->entry.wadjust)
#define EZ_EntryHAdjust(wptr)          ((wptr)->entry.hadjust)
/***********************************************************************
 ***                                                                 ***
 ***                  Optional Entry                                 ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_OptEntryWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------
   *  Data for OptionalEntry
   *---------------------------------*/
  union EZ_WWindow_ *entry;           /* the associated entry */
  union EZ_WWindow_ *button;          /* the associated button */
  union EZ_WWindow_ *listbox;         /* the associated listbox */
  union EZ_WWindow_ *popup;           /* the popup (parent of listbox) */  
} EZ_OptEntryWidget;

#define EZ_OptEntryEntry(wptr)            ((wptr)->optEntry.entry)
#define EZ_OptEntryButton(wptr)           ((wptr)->optEntry.button)
#define EZ_OptEntryListbox(wptr)          ((wptr)->optEntry.listbox)
#define EZ_OptEntryPopup(wptr)            ((wptr)->optEntry.popup)

/***********************************************************************
 ***                                                                 ***
 ***                  File Selector                                  ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_FileSelector_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------
   *  Data for Entry
   *---------------------------------*/
  union EZ_WWindow_ *patternEntry;    /* optional entries  */
  union EZ_WWindow_ *selectionEntry; 
  union EZ_WWindow_ *dirListbox;      /* the two listboxes */
  union EZ_WWindow_ *fileListbox;
  union EZ_WWindow_ *okButton;        /* a few buttons     */
  union EZ_WWindow_ *filterButton;
  union EZ_WWindow_ *cancelButton;
  char              *currentDir;      /* current Directory */
  char              **selection;      /* pointer to the current Selection */

} EZ_FileSelector;

#define EZ_FileSelectorPatternEntry(wptr)            ((wptr)->fileSelector.patternEntry)
#define EZ_FileSelectorSelectionEntry(wptr)          ((wptr)->fileSelector.selectionEntry)
#define EZ_FileSelectorDirListbox(wptr)              ((wptr)->fileSelector.dirListbox)
#define EZ_FileSelectorFileListbox(wptr)             ((wptr)->fileSelector.fileListbox)
#define EZ_FileSelectorOkButton(wptr)                ((wptr)->fileSelector.okButton)
#define EZ_FileSelectorFilterButton(wptr)            ((wptr)->fileSelector.filterButton)
#define EZ_FileSelectorCancelButton(wptr)            ((wptr)->fileSelector.cancelButton)
#define EZ_FileSelectorCurrentDirectory(wptr)        ((wptr)->fileSelector.currentDir)
#define EZ_FileSelectorCurrentSelection(wptr)        ((wptr)->fileSelector.selection)

/***********************************************************************
 ***                                                                 ***
 ***                     3D Canvas                                   ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_3D_Canvas_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------
   *  3D Canvas.
   *---------------------------------*/
  char              releaseMbutton;   /* whether reserve Menu button   */        
  char              backing_store;    /* whether to use X's b-store */
  char              pad1;
  char              pad2;
  
  EZ_GL_Context     *glcontext;       /* the graphics context       */
} EZ_3D_Canvas;

#define EZ_3DCanvasReleaseMenuButton(wptr)    ((wptr)->threedcanvas.releaseMbutton)
#define EZ_3DCanvasBackingStore(wptr)         ((wptr)->threedcanvas.backing_store)
#define EZ_3DCanvasGLContext(wptr)            ((wptr)->threedcanvas.glcontext)

/***********************************************************************
 ***                                                                 ***
 ***                Widget Internal List Box                         ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct listboxentry_ {
  int   length;
  char  *data;
  unsigned long color;
} listboxentry;

typedef struct EZ_IListBoxWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------
   *  Data for list box
   *---------------------------------*/
  
  int               theight;          /* font height in pixels   */
  int               ascent;           /* font ascent             */
  unsigned long     textbg;           /* bg for main window      */
  unsigned long     selectionBG;      /* highlight selection     */
  unsigned long     selectionFG;      /* highlight selection     */
  /*
   * vertical dimension setup. Measurement in lines, borderwidth
   * excluded. These are also needed to update the V scrollbar.
   */
  int               nlines;           /* total # of lines        */
  int               pagesize;         /* # of lines in a page    */
  int               first;            /* first line in display   */
  int               last;             /* last line in display    */

  /*
   * horizontal dim setup. Measurement are in pixels, 
   * excluding borderwidth. (X clip objects to its windows anyway)
   * These info are not needed for IListBox, they are needed to
   * update the H scrollbar if any.
   */
  int               maximalHsize;     /* maximal h size in page  */
  int               pageHsize;        /* window width - border   */
  int               Hstart;           /* x offset                */
  int               Vstart;           /* y offset, for tile      */

  int               sline;            /* current selected line   */
  int               allocatedsize;    /* # of entries allocated  */
  listboxentry      *data;            /* the list                */
  union EZ_WWindow_ *listbox; 
  union EZ_WWindow_ *hscroll; 
  union EZ_WWindow_ *vscroll; 
  /* for when used as a special popup, in optional entry */
  union EZ_WWindow_ *optEntry;       /* the optional Entry      */
  union EZ_WWindow_ *fsl;            /* the optional Entry      */
  int               slineEnd;       /*  */
  char              takeMotion;
  char              callbackOnRelease; /* release cause callback ? */
  char              rowBgSet;
  char              cpad;
  unsigned long     rowBg[2];
  int               space;            
} EZ_IListBoxWidget;

#define  EZ_IListBoxSpacing(wptr)    ((wptr)->ilistbox.space)
#define  EZ_IListBoxRowBGSet(wptr)   ((wptr)->ilistbox.rowBgSet)
#define  EZ_IListBoxRowBG(wptr)      ((wptr)->ilistbox.rowBg)
#define  EZ_IListBoxMaxHSize(wptr)   ((wptr)->ilistbox.maximalHsize)
#define  EZ_IListBoxHPageSize(wptr)  ((wptr)->ilistbox.pageHsize)
#define  EZ_IListBoxHStart(wptr)     ((wptr)->ilistbox.Hstart)
#define  EZ_IListBoxYStart(wptr)     ((wptr)->ilistbox.Vstart)
#define  EZ_IListBoxHScroll(wptr)    ((wptr)->ilistbox.hscroll)
#define  EZ_IListBoxVScroll(wptr)    ((wptr)->ilistbox.vscroll)
#define  EZ_IListBoxListBox(wptr)    ((wptr)->ilistbox.listbox)
#define  EZ_IListBoxTextBG(wptr)     ((wptr)->ilistbox.textbg)
#define  EZ_IListBoxSelectionBG(wptr)   ((wptr)->ilistbox.selectionBG)
#define  EZ_IListBoxSelectionFG(wptr)   ((wptr)->ilistbox.selectionFG)

#define  EZ_IListBoxForeground(wptr) ((wptr)->common.foreground)
#define  EZ_IListBoxBackground(wptr) ((wptr)->common.background)
#define  EZ_IListBoxFont(wptr)       ((wptr)->common.font)
#define  EZ_IListBoxTHeight(wptr)    ((wptr)->ilistbox.theight)
#define  EZ_IListBoxWidth(wptr)      ((wptr)->ilistbox.width)
#define  EZ_IListBoxHeight(wptr)     ((wptr)->ilistbox.height)
#define  EZ_IListBoxFontAscent(wptr) ((wptr)->ilistbox.ascent)

#define  EZ_IListBoxNLines(wptr)     ((wptr)->ilistbox.nlines)
#define  EZ_IListBoxSLine(wptr)      ((wptr)->ilistbox.sline)
#define  EZ_IListBoxSLineEnd(wptr)   ((wptr)->ilistbox.slineEnd)
#define  EZ_IListBoxFirstLine(wptr)  ((wptr)->ilistbox.first)
#define  EZ_IListBoxVStart(wptr)     ((wptr)->ilistbox.first)
#define  EZ_IListBoxLastLine(wptr)   ((wptr)->ilistbox.last)
#define  EZ_IListBoxData(wptr)       ((wptr)->ilistbox.data)
#define  EZ_IListBoxSpace(wptr)      ((wptr)->ilistbox.allocatedsize)
#define  EZ_IListBoxPageSize(wptr)   ((wptr)->ilistbox.pagesize)
#define  EZ_IListBoxMotionCallBack(wptr)   ((wptr)->common.motionCallbacks)
#define  EZ_IListBoxTakeMotion(wptr)    ((wptr)->ilistbox.takeMotion)
#define  EZ_IListBoxCallBackOnRelease(wptr)    ((wptr)->ilistbox.callbackOnRelease)

#define  EZ_IListBoxOptEntry(wptr)   ((wptr)->ilistbox.optEntry)
#define  EZ_IListBoxFSL(wptr)        ((wptr)->ilistbox.fsl)

/***********************************************************************
 ***                                                                 ***
 ***                Widget List Box                                  ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct EZ_ListBoxWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------
   *  Data for list box
   *---------------------------------*/
  union EZ_WWindow_ *ilistbox; 
  union EZ_WWindow_ *hscroll; 
  union EZ_WWindow_ *vscroll; 

} EZ_ListBoxWidget;

#define  EZ_ListBoxIListBox(wptr)   ((wptr)->listbox.ilistbox)
#define  EZ_ListBoxHScroll(wptr)    ((wptr)->listbox.hscroll)
#define  EZ_ListBoxVScroll(wptr)    ((wptr)->listbox.vscroll)
#define  EZ_ListBoxFont(wptr)       EZ_IListBoxFont((wptr)->listbox.ilistbox)
#define  EZ_ListBoxTHeight(wptr)    EZ_IListBoxTHeight((wptr)->listbox.ilistbox)
#define  EZ_ListBoxFontAscent(wptr) EZ_IListBoxFontAscent((wptr)->listbox.ilistbox)
#define  EZ_ListBoxNLines(wptr)     EZ_IListBoxNLines((wptr)->listbox.ilistbox)
#define  EZ_ListBoxSLine(wptr)      EZ_IListBoxSLine((wptr)->listbox.ilistbox)
#define  EZ_ListBoxFirstLine(wptr)  EZ_IListBoxFirstLine((wptr)->listbox.ilistbox)
#define  EZ_ListBoxLastLine(wptr)   EZ_IListBoxLastLine((wptr)->listbox.ilistbox)
#define  EZ_ListBoxData(wptr)       EZ_IListBoxData((wptr)->listbox.ilistbox)
#define  EZ_ListBoxSpace(wptr)      EZ_IListBoxSpace((wptr)->listbox.ilistbox)
#define  EZ_ListBoxPageSize(wptr)   EZ_IListBoxPageSize((wptr)->listbox.ilistbox)
#define  EZ_ListBoxMotionCallBack(wptr)   EZ_IListBoxMotionCallBack((wptr)->listbox.ilistbox)
#define  EZ_ListBoxCallBackOnRelease(wptr) EZ_IListBoxCallBackOnRelease((wptr)->listbox.ilistbox)

#define  EZ_ListBoxOptEntry(wptr)   EZ_IListBoxOptEntry((wptr)->listbox.ilistbox)
#define  EZ_ListBoxFSL(wptr)        EZ_IListBoxFSL((wptr)->listbox.ilistbox)
/***********************************************************************
 ***                                                                 ***
 ***                     Widget Scrollbar                            ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_ScrollbarWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------
   *  Data for Scrollbar
   *---------------------------------*/
  union EZ_WWindow_ *object;          /* widget to bind to       */
  int               borderwidth;      /* button border width     */
  int               bordertype;       /* button border style     */
  int               bgn;              /* lowest position of top/left edge  */
  int               end;              /* highest position of bottom/right  */
  int               position;         /* current position of top/left edge */
  int               arrowsize;        /* arrow_size = ssize + bw*2 */
  int               ssize;            /* width, cpted dynamically  */
  int               lsize;            /* height, computed          */

  char              arrow1highlight;
  char              arrow2highlight;
  char              arrow1borderstyle;
  char              arrow2borderstyle;

  /*
   * these are the informations need to setup the scroll.
   */
  int               totalsize;        /* the total size of ...  */
  int               pagesize;         /* page size              */
  int               beginObj;         /* top line or ..,        */
  int               endObj;           /* bottom line or         */

  float             fincrement;       /* scroll increment       */
  int               iincrement;       /* discrete increment     */
  int               iifactor;         /* discrete increment factor */

  char              dirty;            /* need to recalculate size etc */
  char              style;
  char              highlight;
  char              cpad;

} EZ_ScrollbarWidget;

#define EZ_ScrollHighlight(wptr)     ((wptr)->scrollbar.highlight)
#define EZ_ScrollStyle(wptr)         ((wptr)->scrollbar.style)
#define EZ_ScrollObject(wptr)        ((wptr)->scrollbar.object)
#define EZ_ScrollForeground(wptr)    ((wptr)->common.foreground)
#define EZ_ScrollBackground(wptr)    ((wptr)->common.background)
#define EZ_ScrollBorderWidth(wptr)   ((wptr)->scrollbar.borderwidth)
#define EZ_ScrollBorderStyle(wptr)   ((wptr)->scrollbar.bordertype)
#define EZ_ScrollArrowSize(wptr)     ((wptr)->scrollbar.arrowsize)
#define EZ_ScrollSSize(wptr)         ((wptr)->scrollbar.ssize)
#define EZ_ScrollLSize(wptr)         ((wptr)->scrollbar.lsize)
#define EZ_ScrollPosition(wptr)      ((wptr)->scrollbar.position)
#define EZ_ScrollBgnPosition(wptr)   ((wptr)->scrollbar.bgn)
#define EZ_ScrollEndPosition(wptr)   ((wptr)->scrollbar.end)
#define EZ_ScrollAW1Highlight(wptr)  ((wptr)->scrollbar.arrow1highlight)
#define EZ_ScrollAW2Highlight(wptr)  ((wptr)->scrollbar.arrow2highlight)
#define EZ_ScrollAW1BorderStyle(wptr) ((wptr)->scrollbar.arrow1borderstyle)
#define EZ_ScrollAW2BorderStyle(wptr) ((wptr)->scrollbar.arrow2borderstyle)

#define EZ_ScrollTotalSize(wptr)     ((wptr)->scrollbar.totalsize)
#define EZ_ScrollPageSize(wptr)      ((wptr)->scrollbar.pagesize)
#define EZ_ScrollObjectBegin(wptr)   ((wptr)->scrollbar.beginObj)
#define EZ_ScrollObjectEnd(wptr)     ((wptr)->scrollbar.endObj)
#define EZ_ScrollFIncrement(wptr)    ((wptr)->scrollbar.fincrement)
#define EZ_ScrollIIncrement(wptr)    ((wptr)->scrollbar.iincrement)
#define EZ_ScrollIIFactor(wptr)      ((wptr)->scrollbar.iifactor)
#define EZ_ScrollDirty(wptr)         ((wptr)->scrollbar.dirty)

/***********************************************************************
 ***                                                                 ***
 ***                     Widget Internal Text                        ***
 ***                                                                 ***
 ***********************************************************************/
#include "EZ_Text.h"

typedef struct EZ_ITextWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  int                theight;          /* font height in pixels   */

  /*----------------------------------
   *  Data for TextWidget
   *---------------------------------*/
  union EZ_WWindow_ *hscroll;         /* scroll bars             */
  union EZ_WWindow_ *vscroll; 
  union EZ_WWindow_ *textW;           /* textwidget assoc. to    */
  unsigned long      textbg;
  unsigned long      selectionBG;     /* highlight selection     */

  int               numlines;         /* total # of lines of text*/
  int               first;            /* the 1st line on display */
  int               last;             /* last line on display    */

  int               maxHSize;         /* maximum line length, in pixels */
  int               pageHSize;        /* x width, width - border */
  int               Hstart;           /* starting X offset       */

  int               yposition;        /* cursor line             */
  int               xposition;        /* cursor position         */
  int               lxpos;            /*  */
  int               xmark;            /* current marker          */
  int               ymark;

  int               selectionYbgn;
  int               selectionYend;
  int               selectionXbgn;
  int               selectionXend;

  ITextLine         **data;
  int                allocatedlines;
  int                vspacing;

  char              selectionFlag;
  char              editable;
  char              hscroll_in_action;
  char              lastcmdiskill;    /* keep track of C-k */

  char              modified;
  char              wrap;
  char              dirty;
  char              wrapped;
} EZ_ITextWidget;

#define EZ_ITextDirty(wptr)             ((wptr)->itext.dirty)
#define EZ_ITextWrapText(wptr)          ((wptr)->itext.wrap)
#define EZ_ITextWraped(wptr)            ((wptr)->itext.wrapped)
#define EZ_ITextModified(wptr)          ((wptr)->itext.modified)
#define EZ_ITextForeground(wptr)        ((wptr)->common.foreground)
#define EZ_ITextBackground(wptr)        ((wptr)->common.background)
#define EZ_ITextFont(wptr)              ((wptr)->common.font)
#define EZ_ITextTHeight(wptr)           ((wptr)->itext.theight)
#define EZ_ITextTextBG(wptr)            ((wptr)->itext.textbg)
#define EZ_ITextTextSelectionBG(wptr)   ((wptr)->itext.selectionBG)
#define EZ_ITextVSpacing(wptr)          ((wptr)->itext.vspacing)

#define EZ_ITextTextWidget(wptr)        ((wptr)->itext.textW)
#define EZ_ITextHScroll(wptr)           ((wptr)->itext.hscroll)
#define EZ_ITextVScroll(wptr)           ((wptr)->itext.vscroll)

#define EZ_ITextMaxHSize(wptr)          ((wptr)->itext.maxHSize)
#define EZ_ITextHPageSize(wptr)         ((wptr)->itext.pageHSize)
#define EZ_ITextHStart(wptr)            ((wptr)->itext.Hstart)

#define EZ_ITextSpace(wptr)             ((wptr)->itext.allocatedlines)
#define EZ_ITextData(wptr)              ((wptr)->itext.data)

#define EZ_ITextXPosition(wptr)         ((wptr)->itext.xposition)
#define EZ_ITextYPosition(wptr)         ((wptr)->itext.yposition)
#define EZ_ITextLXPosition(wptr)        ((wptr)->itext.lxpos)

#define EZ_ITextXMark(wptr)             ((wptr)->itext.xmark)
#define EZ_ITextYMark(wptr)             ((wptr)->itext.ymark)

#define EZ_ITextSelectionYBgn(wptr)     ((wptr)->itext.selectionYbgn)
#define EZ_ITextSelectionYEnd(wptr)     ((wptr)->itext.selectionYend)
#define EZ_ITextSelectionXBgn(wptr)     ((wptr)->itext.selectionXbgn)
#define EZ_ITextSelectionXEnd(wptr)     ((wptr)->itext.selectionXend)
#define EZ_ITextSelectionFlag(wptr)     ((wptr)->itext.selectionFlag)

#define EZ_ITextEditable(wptr)          ((wptr)->itext.editable)
#define EZ_ITextHScrollInAction(wptr)   ((wptr)->itext.hscroll_in_action)
#define EZ_ITextLastCmdIsKill(wptr)     ((wptr)->itext.lastcmdiskill)

#define EZ_ITextNLines(wptr)            ((wptr)->itext.numlines)
#define EZ_ITextVStart(wptr)            ((wptr)->itext.first)
#define EZ_ITextFirstLine(wptr)         ((wptr)->itext.first)
#define EZ_ITextLastLine(wptr)          ((wptr)->itext.last)
#define EZ_ITextPageSize(wptr)          (((wptr)->itext.last) - ((wptr)->itext.first) + 1)


/***********************************************************************
 ***                                                                 ***
 ***                     Widget Text                                 ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct EZ_TextWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------
   *  Data for Text
   *---------------------------------*/
  union EZ_WWindow_ *itext; 
  union EZ_WWindow_ *hscroll; 
  union EZ_WWindow_ *vscroll; 

} EZ_TextWidget;

#define  EZ_TextIText(wptr)      ((wptr)->text.itext)
#define  EZ_TextHScroll(wptr)    ((wptr)->text.hscroll)
#define  EZ_TextVScroll(wptr)    ((wptr)->text.vscroll)
#define  EZ_TextFont(wptr)       EZ_ITextFont((wptr)->text.itext)
#define  EZ_TextTHeight(wptr)    EZ_ITextTHeight((wptr)->text.itext)
#define  EZ_TextNLines(wptr)     EZ_ITextNLines((wptr)->text.itext)
#define  EZ_TextFirstLine(wptr)  EZ_ITextFirstLine((wptr)->text.itext)
#define  EZ_TextLastLine(wptr)   EZ_ITextLastLine((wptr)->text.itext)
#define  EZ_TextData(wptr)       EZ_ITextData((wptr)->text.itext)
#define  EZ_TextPageSize(wptr)   EZ_ITextPageSize((wptr)->text.itext)


/***********************************************************************
 ***                                                                 ***
 ***                     Widget NoteBook Page                        ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct EZ_NBPageWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------------------------------------
   *  Data for notebook data, just like a regular frame
   *-------------------------------------------------------------*/
  union EZ_WWindow_ *notebook;        /* notebook this belongs to*/
  union EZ_WWindow_ *companion;       /* special companion widget*/

} EZ_NBPage;

#define EZ_NBPageNoteBook(wptr)          ((wptr)->nbpage.notebook)
#define EZ_NBPageCompanion(wptr)         ((wptr)->nbpage.companion)

#define EZ_NBPageBackground(wptr)        ((wptr)->common.background) 
#define EZ_NBPageForeground(wptr)        ((wptr)->common.foreground) 
#define EZ_NBPageFont(wptr)              ((wptr)->common.font)

/***********************************************************************
 ***                                                                 ***
 ***                     Widget NoteBook(hack)                       ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct EZ_NoteBookWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*----------------------------------
   *  Data for NoteBook.
   *---------------------------------*/
  union EZ_WWindow_ *Frame; 
  union EZ_WWindow_ *TFrame;
  union EZ_WWindow_ *BFrame;
  union EZ_WWindow_ *ActivePage;
  int   maxMinW;
  int   maxMinH;

} EZ_NoteBook;

#define  EZ_NoteBookForeground(wptr)   ((wptr)->common.foreground)
#define  EZ_NoteBookBackground(wptr)   ((wptr)->common.background)
#define  EZ_NoteBookFont(wptr)         ((wptr)->common.font)
#define  EZ_NoteBookFrame(wptr)        ((wptr)->notebook.Frame)
#define  EZ_NoteBookTFrame(wptr)       ((wptr)->notebook.TFrame)
#define  EZ_NoteBookBFrame(wptr)       ((wptr)->notebook.BFrame)
#define  EZ_NoteBookActivePage(wptr)   ((wptr)->notebook.ActivePage)
#define  EZ_NoteBookMaxMinWidth(wptr)  ((wptr)->notebook.maxMinW)
#define  EZ_NoteBookMaxMinHeight(wptr) ((wptr)->notebook.maxMinH)

/***********************************************************************
 ***                                                                 ***
 ***                     Widget PaneHandle                           ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct EZ_PaneHandle_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  int orien;
  int style;
  int movable;
} EZ_PaneHandle;
#define EZ_PaneHandleOrientation(wptr) ((wptr)->panehandle.orien)
#define EZ_PaneHandleStyle(wptr)       ((wptr)->panehandle.style)
#define EZ_PaneHandleMovable(wptr)     ((wptr)->panehandle.movable)
/***********************************************************************
 ***                                                                 ***
 ***                     Widget Embeder                              ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct EZ_Embeder_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  int  confirmed;                       /* has been confirmed ?  */
  int  x, y;                            /* position of the embeded window */
  int  minW, minH;                      /* min dimension of embeded appl  */
  int  width, height;                   /* actual dimen of the embeded win*/
  Window commu;                         /* commu win for external application */
  Window win;                           /* the embeded window    */
  unsigned long widgetAddr;             /* the embeded widget addr  */
  unsigned long  time;                  /* init time of the embeded appl  */

  /* for executor */
  char *command;                        /* the command               */
  char *cargs;                          /* command args              */
  int  pW, pH;                          /* previous size of executor */
  int  mayBeDirty;                      /* size of the embeded window may 
					 * have been changed */
  int depth;                            /* depth of this embeding    */
  char cursorChanged;             
} EZ_Embeder;

#define EZ_EmbederCursorChanged(wptr)  ((wptr)->Embeder.cursorChanged)
#define EZ_EmbederConfirmed(wptr)      ((wptr)->Embeder.confirmed)
#define EZ_EmbederCommuWin(wptr)       ((wptr)->Embeder.commu)
#define EZ_EmbederWin(wptr)            ((wptr)->Embeder.win)
#define EZ_EmbederId(wptr)             ((wptr)->Embeder.time)
#define EZ_EmbederX(wptr)              ((wptr)->Embeder.x)
#define EZ_EmbederY(wptr)              ((wptr)->Embeder.y)
#define EZ_EmbederWidgetAddr(wptr)     ((wptr)->Embeder.widgetAddr)
#define EZ_EmbederWidth(wptr)          ((wptr)->Embeder.width)
#define EZ_EmbederHeight(wptr)         ((wptr)->Embeder.height)
#define EZ_EmbederCommand(wptr)        ((wptr)->Embeder.command)

#define EZ_ExecutorCommand(wptr)       ((wptr)->Embeder.command)
#define EZ_ExecutorCommandArgs(wptr)   ((wptr)->Embeder.cargs)
#define EZ_ExecutorPreviousW(wptr)     ((wptr)->Embeder.pW)
#define EZ_ExecutorPreviousH(wptr)     ((wptr)->Embeder.pH)
#define EZ_ExecutorMinW(wptr)          ((wptr)->Embeder.minW)
#define EZ_ExecutorMinH(wptr)          ((wptr)->Embeder.minH)
#define EZ_ExecutorMayBeDirty(wptr)    ((wptr)->Embeder.mayBeDirty)
#define EZ_ExecutorDepth(wptr)         ((wptr)->Embeder.depth)


/***********************************************************************
 ***                                                                 ***
 ***                  Internal Tree                                  ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_ITreeWidget_ { 
  char common[sizeof(EZ_WidgetCommon)]; /* common block     */

  EZ_TreeNode         *root;            /* root node        */
  EZ_TreeNode         *selection;       /* the selected item */
  unsigned long       highlightBG; 
  unsigned long       highlightFG; 
  unsigned long       textBG;          /* window background */

  int                 Ysize;          /* size in the vert dir */
  int                 Ypagesize;      /* window height - border - padY */
  int                 Ystart;         /* start pos of display   */

  int                 Xsize;          /* maximal h size in page  */
  int                 Xpagesize;      /* window width - border   */
  int                 Xstart;         /* h offset                */

  union EZ_WWindow_   *hscroll; 
  union EZ_WWindow_   *vscroll; 
  union EZ_WWindow_   *tree;          /* parent: WIDGET_TREE */

  int                 indent;         /* indentation and spacing ?*/
  int                 spacing;     
  char                drawEdge;
  char                treetype;
  char                displayed;
  char                dirty;
  char                highlightMode;
  char                rubberband;
  char                pad;
  char                rowBgSet;
  unsigned long       rowBg[2];
  
} EZ_ITreeWidget;

#define EZ_ITreeWRowBgSet(wptr)           ((wptr)->itree.rowBgSet)
#define EZ_ITreeWRowBg(wptr)              ((wptr)->itree.rowBg)
#define EZ_ITreeWHighlightMode(wptr)      ((wptr)->itree.highlightMode)
#define EZ_ITreeWRubberBand(wptr)         ((wptr)->itree.rubberband)
#define EZ_ITreeWRoot(wptr)               ((wptr)->itree.root)
#define EZ_ITreeWSelection(wptr)          ((wptr)->itree.selection)
#define EZ_ITreeWMotionCallBack(wptr)     ((wptr)->common.motionCallbacks)
#define EZ_ITreeWHighlightBG(wptr)        ((wptr)->itree.highlightBG)
#define EZ_ITreeWHighlightFG(wptr)        ((wptr)->itree.highlightFG)
#define EZ_ITreeWTextBG(wptr)             ((wptr)->itree.textBG)
#define EZ_ITreeWDisplayed(wptr)          ((wptr)->itree.displayed)

#define EZ_ITreeWYSize(wptr)              ((wptr)->itree.Ysize)
#define EZ_ITreeWYPageSize(wptr)          ((wptr)->itree.Ypagesize)
#define EZ_ITreeWYStart(wptr)             ((wptr)->itree.Ystart)

#define EZ_ITreeWXSize(wptr)              ((wptr)->itree.Xsize)
#define EZ_ITreeWXPageSize(wptr)          ((wptr)->itree.Xpagesize)
#define EZ_ITreeWXStart(wptr)             ((wptr)->itree.Xstart)

#define EZ_ITreeWHScroll(wptr)            ((wptr)->itree.hscroll)
#define EZ_ITreeWVScroll(wptr)            ((wptr)->itree.vscroll)
#define EZ_ITreeWTree(wptr)               ((wptr)->itree.tree)
#define EZ_ITreeWDirty(wptr)              ((wptr)->itree.dirty)

/*****************************************************************************/
typedef struct EZ_TreeWidget_ { 
  char common[sizeof(EZ_WidgetCommon)]; /* common block     */

  union EZ_WWindow_ *itree; 
  union EZ_WWindow_ *hscroll; 
  union EZ_WWindow_ *vscroll; 
} EZ_TreeWidget;

#define EZ_TreeWITree(wptr)                     ((wptr)->tree.itree)
#define EZ_TreeWHScroll(wptr)                   ((wptr)->tree.hscroll)
#define EZ_TreeWVScroll(wptr)                   ((wptr)->tree.vscroll)

/* from ITree */
#define EZ_TreeWIndentation(wptr)       EZ_ITreeWIndentation((wptr)->tree.itree)
#define EZ_TreeWSpacing(wptr)           EZ_ITreeWSpacing((wptr)->tree.itree)
#define EZ_TreeWRoot(wptr)               EZ_ITreeWRoot((wptr)->tree.itree)
#define EZ_TreeWSelection(wptr)          EZ_ITreeWSelection((wptr)->tree.itree)
#define EZ_TreeWMotionCallBack(wptr)     EZ_ITreeWMotionCallBack((wptr)->tree.itree)
#define EZ_TreeWHighlightBG(wptr)        EZ_ITreeWHighlightBG((wptr)->tree.itree)
#define EZ_TreeWHighlightFG(wptr)        EZ_ITreeWHighlightFG((wptr)->tree.itree)
#define EZ_TreeWYSize(wptr)              EZ_ITreeWYSize((wptr)->tree.itree)
#define EZ_TreeWYPageSize(wptr)          EZ_ITreeWYPageSize((wptr)->tree.itree)
#define EZ_TreeWYStart(wptr)             EZ_ITreeWYStart((wptr)->tree.itree)
#define EZ_TreeWXSize(wptr)              EZ_ITreeWXSize((wptr)->tree.itree)
#define EZ_TreeWXPageSize(wptr)          EZ_ITreeWXPageSize((wptr)->tree.itree)
#define EZ_TreeWXStart(wptr)             EZ_ITreeWXStart((wptr)->tree.itree)
#define EZ_TreeWTreeType(wptr)           EZ_ITreeWTreeType((wptr)->tree.itree)

/***********************************************************************
 ***                                                                 ***
 ***                  Internal FancyListBox                          ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_IFancyListBox_ { 
  char common[sizeof(EZ_WidgetCommon)]; /* common block     */

  int                 columns;          /* number of columns */
  int                 rows;             /* n rows */
  int                 allocatedspace;   /* total allocated storage */
  int                 selectionIDX;     /* the selection (row)     */
  int                 selectionCol;     /* the selection (col)     */

  unsigned long       rowBg[2];

  char                selectionType;
  char                rowBgSet;
  char                dirty;
  char                displayed;
  char                highlightMode;
  char                rubberband;
  char                rowmode;  /* not used */
  char                cpad;

  EZ_Item             ***data;           /* the selected item  */
  int                 *hsizes;           /* size of columns  */
  int                 *vsizes;           /* size of the rows */

  unsigned long       highlightBG; 
  unsigned long       highlightFG; 
  unsigned long       textBG;          /* window background */

  int                 Ysize;          /* size in the vert dir */
  int                 Ypagesize;      /* window height - border - padY */
  int                 Ystart;         /* start pos of display   */

  int                 Xsize;          /* maximal h size in page  */
  int                 Xpagesize;      /* window width - border   */
  int                 Xstart;         /* h offset                */

  union EZ_WWindow_   *hscroll; 
  union EZ_WWindow_   *vscroll; 
  union EZ_WWindow_   *flistbox;      /* parent: WIDGET_FANCY_LIST_BOX */

} EZ_IFancyListBox;

#define EZ_IFListBoxRowBGSet(wptr)         ((wptr)->iflistbox.rowBgSet)
#define EZ_IFListBoxRowBG(wptr)            ((wptr)->iflistbox.rowBg)
#define EZ_IFListBoxRowMode(wptr)          ((wptr)->iflistbox.rowmode)
#define EZ_IFListBoxHighlightMode(wptr)    ((wptr)->iflistbox.highlightMode)
#define EZ_IFListBoxRubberBand(wptr)       ((wptr)->iflistbox.rubberband)
#define EZ_IFListBoxDisplayed(wptr)        ((wptr)->iflistbox.displayed)
#define EZ_IFListBoxColumns(wptr)          ((wptr)->iflistbox.columns)
#define EZ_IFListBoxRows(wptr)             ((wptr)->iflistbox.rows)
#define EZ_IFListBoxSpace(wptr)            ((wptr)->iflistbox.allocatedspace)
#define EZ_IFListBoxSelectionIDX(wptr)     ((wptr)->iflistbox.selectionIDX)
#define EZ_IFListBoxSelectionIDX2(wptr)    ((wptr)->iflistbox.selectionIDX2)
#define EZ_IFListBoxSelectionCol(wptr)     ((wptr)->iflistbox.selectionCol)
#define EZ_IFListBoxSelectionCol2(wptr)    ((wptr)->iflistbox.selectionCol2)
#define EZ_IFListBoxData(wptr)             ((wptr)->iflistbox.data)
#define EZ_IFListBoxHighlightBG(wptr)      ((wptr)->iflistbox.highlightBG)
#define EZ_IFListBoxHighlightFG(wptr)      ((wptr)->iflistbox.highlightFG)
#define EZ_IFListBoxTextBG(wptr)           ((wptr)->iflistbox.textBG)
#define EZ_IFListBoxXSize(wptr)            ((wptr)->iflistbox.Xsize)
#define EZ_IFListBoxXPageSize(wptr)        ((wptr)->iflistbox.Xpagesize)
#define EZ_IFListBoxXStart(wptr)           ((wptr)->iflistbox.Xstart)
#define EZ_IFListBoxYSize(wptr)            ((wptr)->iflistbox.Ysize)
#define EZ_IFListBoxYPageSize(wptr)        ((wptr)->iflistbox.Ypagesize)
#define EZ_IFListBoxYStart(wptr)           ((wptr)->iflistbox.Ystart)

#define EZ_IFListBoxDirty(wptr)            ((wptr)->iflistbox.dirty)
#define EZ_IFListBoxHSizes(wptr)            ((wptr)->iflistbox.hsizes)
#define EZ_IFListBoxVSizes(wptr)            ((wptr)->iflistbox.vsizes)

#define EZ_IFListBoxHScroll(wptr)          ((wptr)->iflistbox.hscroll)
#define EZ_IFListBoxVScroll(wptr)          ((wptr)->iflistbox.vscroll)
#define EZ_IFListBoxFListBox(wptr)         ((wptr)->iflistbox.flistbox)
#define EZ_IFListBoxMotionCallBack(wptr)   ((wptr)->common.motionCallbacks)
#define EZ_IFListBoxSelectionType(wptr)    ((wptr)->iflistbox.selectionType)
#define EZ_IFListBoxSItem(wptr)            ((wptr)->iflistbox.sitem)

typedef struct EZ_FancyListBox_ { 
  char common[sizeof(EZ_WidgetCommon)]; /* common block     */

  union EZ_WWindow_ *iflistbox; 
  union EZ_WWindow_ *hscroll; 
  union EZ_WWindow_ *vscroll; 
} EZ_FancyListBox;

#define EZ_FListBoxIFListBox(wptr)                 ((wptr)->flistbox.iflistbox)
#define EZ_FListBoxHScroll(wptr)                   ((wptr)->flistbox.hscroll)
#define EZ_FListBoxVScroll(wptr)                   ((wptr)->flistbox.vscroll)


/***********************************************************************
 ***                                                                 ***
 ***                  Work Area                                      ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_IWorkArea_ { 
  char common[sizeof(EZ_WidgetCommon)]; /* common block     */

  EZ_Item             **items;           /* an array of items */
  EZ_Item             *sitem;            /* selected item     */

  int                 nitems;            /* total number of items */
  int                 space;             /* allocated size of items */

  unsigned long       highlightBG; 
  unsigned long       highlightFG; 
  unsigned long       textBG;          /* window background */

  int                 Ysize;          /* size in the vert dir */
  int                 Ypagesize;      /* window height - border - padY */
  int                 Ystart;         /* start pos of display   */

  int                 Xsize;          /* maximal h size in page  */
  int                 Xpagesize;      /* window width - border   */
  int                 Xstart;         /* h offset                */

  union EZ_WWindow_   *hscroll; 
  union EZ_WWindow_   *vscroll; 
  union EZ_WWindow_   *workArea;

  /* this is reserved for geometry manager */
  void               *GMData;
  EZ_WorkAreaGeometryManager    geometryManager;

  char                dirty;
  char                dpyed;
  char                movable;
  char                drawn;
  char                highlightMode;
  char                rubberband;
  char                pad[2];

} EZ_IWorkArea;

#define EZ_IWorkAreaHighlightMode(wptr)    ((wptr)->iworkArea.highlightMode)
#define EZ_IWorkAreaRubberBand(wptr)       ((wptr)->iworkArea.rubberband)
#define EZ_IWorkAreaData(wptr)             ((wptr)->iworkArea.items)
#define EZ_IWorkAreaSelection(wptr)        ((wptr)->iworkArea.sitem)
#define EZ_IWorkAreaNItems(wptr)           ((wptr)->iworkArea.nitems)
#define EZ_IWorkAreaDataSpace(wptr)        ((wptr)->iworkArea.space)
#define EZ_IWorkAreaHighlightBG(wptr)      ((wptr)->iworkArea.highlightBG)
#define EZ_IWorkAreaHighlightFG(wptr)      ((wptr)->iworkArea.highlightFG)
#define EZ_IWorkAreaTextBG(wptr)           ((wptr)->iworkArea.textBG)

#define EZ_IWorkAreaDirty(wptr)             ((wptr)->iworkArea.dirty)

#define EZ_IWorkAreaYSize(wptr)            ((wptr)->iworkArea.Ysize)
#define EZ_IWorkAreaYPageSize(wptr)        ((wptr)->iworkArea.Ypagesize)
#define EZ_IWorkAreaYStart(wptr)           ((wptr)->iworkArea.Ystart)

#define EZ_IWorkAreaXSize(wptr)            ((wptr)->iworkArea.Xsize)
#define EZ_IWorkAreaXPageSize(wptr)        ((wptr)->iworkArea.Xpagesize)
#define EZ_IWorkAreaXStart(wptr)           ((wptr)->iworkArea.Xstart)

#define EZ_IWorkAreaHScroll(wptr)          ((wptr)->iworkArea.hscroll)
#define EZ_IWorkAreaVScroll(wptr)          ((wptr)->iworkArea.vscroll)

#define EZ_IWorkAreaWorkArea(wptr)         ((wptr)->iworkArea.workArea)
#define EZ_IWorkAreaGeometryManager(wptr)  ((wptr)->iworkArea.geometryManager)
#define EZ_IWorkAreaGMData(wptr)           ((wptr)->iworkArea.GMData)
#define EZ_IWorkAreaDisplayed(wptr)        ((wptr)->iworkArea.dpyed)
#define EZ_IWorkAreaDrawn(wptr)            ((wptr)->iworkArea.drawn)
#define EZ_IWorkAreaItemMovable(wptr)      ((wptr)->iworkArea.movable)


typedef struct EZ_WorkArea_ { 
  char common[sizeof(EZ_WidgetCommon)]; /* common block     */

  union EZ_WWindow_   *iworkarea;
  union EZ_WWindow_   *hscroll; 
  union EZ_WWindow_   *vscroll; 

} EZ_WorkArea;

#define EZ_WorkAreaHScroll(wptr)          ((wptr)->workArea.hscroll)
#define EZ_WorkAreaVScroll(wptr)          ((wptr)->workArea.vscroll)
#define EZ_WorkAreaIWorkArea(wptr)        ((wptr)->workArea.iworkarea)

/***********************************************************************
 ***                                                                 ***
 ***                     Widget Raw Xwin                             ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_RawXWindow_ { 
  char common[sizeof(EZ_WidgetCommon)]; /* common block     */
  void                  *phandle;
  void                  *pdata[16];
  EZ_UnknownDataType    cdata[10];
} EZ_RawXWindow;

#define EZ_RawXWinPHandle(wptr)                ((wptr)->rawxwindow.phandle)
#define EZ_RawXWinPData(wptr)                  ((wptr)->rawxwindow.pdata)
#define EZ_RawXWinCData(wptr)                  ((wptr)->rawxwindow.cdata)

/***********************************************************************
 ***                                                                 ***
 ***                     Widget Lcd                                  ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_LcdWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /* private data */
  int            ndigits;
  int            size;
  unsigned long  bg;
  unsigned long  fg;
  unsigned long  fgd;
  unsigned long  fgb;
  char           *str;
  int            strlen;
} EZ_LcdWidget;

#define EZ_LCDBG(wptr)           ((wptr)->lcd.bg)
#define EZ_LCDFG(wptr)           ((wptr)->lcd.fg)
#define EZ_LCDFGDark(wptr)       ((wptr)->lcd.fgd)
#define EZ_LCDFGBright(wptr)     ((wptr)->lcd.fgb)
#define EZ_LCDNDigits(wptr)      ((wptr)->lcd.ndigits)
#define EZ_LCDString(wptr)       ((wptr)->lcd.str)
#define EZ_LCDSize(wptr)         ((wptr)->lcd.size)
#define EZ_LCDStringLength(wptr) ((wptr)->lcd.strlen)

/***********************************************************************
 ***                                                                 ***
 ***                     Widget Led                                  ***
 ***                                                                 ***
 ***********************************************************************/
typedef  unsigned long  (*EZ_LedPixelColorF)  MY_ANSIARGS((EZ_Widget *w, int x, int y));
typedef  struct EZ_LedAction_
{
  union EZ_WWindow_   *led; 
  int    type;
  char   *str;
  int    delay;
  int    count;
  int    serial;
  int    status;
  unsigned long color;
  EZ_LedPixelColorF colorf;
  struct EZ_LedAction_  *next;
} EZ_LedAction;


#define     LED_SC_LEFT       1
#define     LED_SC_RIGHT      2
#define     LED_SC_UP         3
#define     LED_SC_DOWN       4
#define     LED_SC_CENTER     5
#define     LED_SC_CENTER_V   6
#define     LED_SW_LEFT       7
#define     LED_SW_RIGHT      8
#define     LED_SW_UP         9
#define     LED_SW_DOWN       10
#define     LED_SW_CENTER     11
#define     LED_SW_CENTER_V   12
#define     LED_BOUNCE        13
#define     LED_BOUNCE_V      14
#define     LED_SHOW          15
#define     LED_BLINK         16
#define     LED_SB_LEFT       17
#define     LED_SB_RIGHT      18
#define     LED_SLEEP         19
#define     LED_LAST_ACTION   LED_SLEEP


#define     LED_AT_FINISHED    0x0
#define     LED_AT_WAITING     0x1
#define     LED_AT_RUNNING     0x2

#define     LED_ALL_DIRTY     0xf
#define     LED_MASK_DIRTY    0x1
#define     LED_PIXMAPC_DIRTY 0x2
#define     LED_PIXMAP_DIRTY  0x4
#define     LED_BLOCKED       0x800

typedef struct EZ_LedWidget_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /* private data */
  short          srows, scols; /* number of rows and cols configured */
  short          rows, cols;   /* actual number of rows and cols */
  short          ww,hh;        /* width/height of widget exclude border */
  short          xsize;         /* pixel size */
  short          ysize;
  unsigned char  bitmap;
  unsigned char  indicator;   
  unsigned char  bb;
  unsigned char  cc;
  unsigned long  bg;           /* bg color */
  unsigned long  bgl;          /* bg color */
  Pixmap         pixmap;       /* current image */
  Pixmap         pixmapC;      /* a clear image */
  int            textwidth, textheight;
  char           *str;
  int            length, nlines, linelength;
  int            maskw, maskh; /* width and height of mask */
  unsigned char  *image;       /* the bitmap */
  int            xoff, yoff;   /* UL of pixmap relative to UL of mask */
  int            dirty;
  int            status;
  EZ_LedPixelColorF colorf;    /* color function */
  EZ_LedAction      *at;       /* action table   */
  int            xyidx;     
  int            fflag;   
  short          xxx;
  short          yyy;  
} EZ_LedWidget;

#define EZ_LEDXXX(wptr)          ((wptr)->led.xxx)
#define EZ_LEDYYY(wptr)          ((wptr)->led.yyy)
#define EZ_LEDIndicator(wptr)    ((wptr)->led.indicator)
#define EZ_LEDXYIdx(wptr)        ((wptr)->led.xyidx)
#define EZ_LEDOther(wptr)        ((wptr)->led.fflag)
#define EZ_LEDAction(wptr)       ((wptr)->led.at)
#define EZ_LEDBG(wptr)           ((wptr)->led.bg)
#define EZ_LEDBGL(wptr)          ((wptr)->led.bgl)
#define EZ_LEDXSize(wptr)        ((wptr)->led.xsize)
#define EZ_LEDYSize(wptr)        ((wptr)->led.ysize)
#define EZ_LEDPixmap(wptr)       ((wptr)->led.pixmap)
#define EZ_LEDPixmapC(wptr)      ((wptr)->led.pixmapC)
#define EZ_LEDString(wptr)       ((wptr)->led.str)
#define EZ_LEDStringLength(wptr) ((wptr)->led.length)
#define EZ_LEDLineLength(wptr)   ((wptr)->led.linelength)
#define EZ_LEDNLines(wptr)       ((wptr)->led.nlines)
#define EZ_LEDRows(wptr)         ((wptr)->led.rows)
#define EZ_LEDCols(wptr)         ((wptr)->led.cols)
#define EZ_LEDSRows(wptr)        ((wptr)->led.srows)
#define EZ_LEDSCols(wptr)        ((wptr)->led.scols)
#define EZ_LEDImage(wptr)        ((wptr)->led.image)
#define EZ_LEDOldW(wptr)         ((wptr)->led.ww)
#define EZ_LEDOldH(wptr)         ((wptr)->led.hh)
#define EZ_LEDDirty(wptr)        ((wptr)->led.dirty)
#define EZ_LEDStatus(wptr)       ((wptr)->led.status)
#define EZ_LEDTextWidth(wptr)    ((wptr)->led.textwidth)
#define EZ_LEDTextHeight(wptr)   ((wptr)->led.textheight)
#define EZ_LEDMaskW(wptr)        ((wptr)->led.maskw)
#define EZ_LEDAdvance(wptr)      ((wptr)->led.maskw)
#define EZ_LEDMaskH(wptr)        ((wptr)->led.maskh)
#define EZ_LEDXOffset(wptr)      ((wptr)->led.xoff)
#define EZ_LEDYOffset(wptr)      ((wptr)->led.yoff)
#define EZ_LEDColorF(wptr)       ((wptr)->led.colorf)
#define EZ_LEDHasBitmap(wptr)    ((wptr)->led.bitmap)

/***********************************************************************
 ***                                                                 ***
 ***                     Status Meter                                ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_StatusMeter_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /* private data */
  int      n;                  /* num entries */
  unsigned long bcolor;        /* bg color    */
  unsigned long fcolor[5];     /* fg color    */
  float f[15];          /*  control parameters      */
  float range[2];       /*  total range             */
  int   pos;            /* current position, tmp    */
  char  orientation;    /* vertical or horizontal   */
  char  style;          /* different styles         */
  char  showValue;      /* whether to display value */
  char  stoped;         /* whether stoped */

  unsigned char barwidth; 
  unsigned char barsep;
  
} EZ_StatusMeter;

#define EZ_SMeterBColor(wptr)       ((wptr)->smeter.bcolor)
#define EZ_SMeterFColor(wptr)       ((wptr)->smeter.fcolor)
#define EZ_SMeterNControls(wptr)    ((wptr)->smeter.n)
#define EZ_SMeterValues(wptr)       ((wptr)->smeter.f)
#define EZ_SMeterRange(wptr)        ((wptr)->smeter.range)
#define EZ_SMeterPosition(wptr)     ((wptr)->smeter.pos)
#define EZ_SMeterOrientation(wptr)  ((wptr)->smeter.orientation)
#define EZ_SMeterStyle(wptr)        ((wptr)->smeter.style)
#define EZ_SMeterShowValue(wptr)    ((wptr)->smeter.showValue)
#define EZ_SMeterStoped(wptr)       ((wptr)->smeter.stoped)
#define EZ_SMeterBarWidth(wptr)     ((wptr)->smeter.barwidth)
#define EZ_SMeterBarSep(wptr)       ((wptr)->smeter.barsep)

/************************************************************************************
 *
 *   Terminal Emulator
 *
 ************************************************************************************/
#include "EZ_ITerm.h"

typedef struct EZ_ITerm_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for Terminal emulator
   *------------------------------------*/  
  short             xdiff, ydiff;
  short             xoff, yoff;
  short             onrow, oncol;     /* saved window size   */
  short             nrow, ncol;       /* window size in chars    */
  short             fwidth, fheight;  /* font dimen in pixels   */
  short             ascent;           /* font ascent             */
  short             savelines;	      /* number of lines that fit in scrollback */
  short             nscrolled;	      /* number of line actually scrolled */
  short             view_start;       /* scrollback view starts here */

  unsigned long     cursorColor;     /* cursorColor             */
  unsigned long     textBG;          /* background              */
  unsigned int      colorfgbg;
  rend_t            rstyle;          /* current rendition */
  int               last_xcursor;   
  Pixmap            pixmap;
  short             pwidth, pheight;
  screen_t          *screen;
  control_t         *control;
  SaveScreen        *saveScreen;
  selection_t       *selection;
  unsigned long     privateModes;
  unsigned long     saveModes;
  void              *timer;                
  void              *btimer;

  pid_t             childPid;              /* pid of child process */

  union EZ_WWindow_ *vscroll;        /* the scroll bar          */
  union EZ_WWindow_ *term;           /* textwidget assoc. to    */
  char              rvideo;
  char              rvideoPending;
  char              dirty;
  char              focus;
  char              fixedfont;
  char              cpad[3];

} EZ_ITermWidget;

#define EZ_ITermFont(wptr)          ((wptr)->common.font)

#define EZ_ITermFixedFont(wptr)     ((wptr)->iterm.fixedfont)

#define EZ_ITermColorFgBg(wptr)     ((wptr)->iterm.colorfgbg)
#define EZ_ITermVScroll(wptr)       ((wptr)->iterm.vscroll)

#define EZ_ITermFocus(wptr)         ((wptr)->iterm.focus)
#define EZ_ITermXOff(wptr)          ((wptr)->iterm.xoff)
#define EZ_ITermYOff(wptr)          ((wptr)->iterm.yoff)
#define EZ_ITermXDiff(wptr)         ((wptr)->iterm.xdiff)
#define EZ_ITermYDiff(wptr)         ((wptr)->iterm.ydiff)

#define EZ_ITermLPixmap(wptr)       ((wptr)->iterm.pixmap)
#define EZ_ITermLPixmapWidth(wptr)  ((wptr)->iterm.pwidth)
#define EZ_ITermLPixmapHeight(wptr) ((wptr)->iterm.pheight)

#define EZ_ITermCursorColor(wptr)   ((wptr)->iterm.cursorColor)
#define EZ_ITermTextBG(wptr)        ((wptr)->iterm.textBG)
#define EZ_ITermLastXCursor(wptr)   ((wptr)->iterm.last_xcursor)
#define EZ_ITermTerm(wptr)          ((wptr)->iterm.term)

#define EZ_ITermFWidth(wptr)        ((wptr)->iterm.fwidth)
#define EZ_ITermFHeight(wptr)       ((wptr)->iterm.fheight)
#define EZ_ITermFAscent(wptr)       ((wptr)->iterm.ascent)

#define EZ_ITermNCol(wptr)          ((wptr)->iterm.ncol)
#define EZ_ITermNRow(wptr)          ((wptr)->iterm.nrow)
#define EZ_ITermONCol(wptr)         ((wptr)->iterm.oncol)
#define EZ_ITermONRow(wptr)         ((wptr)->iterm.onrow)
#define EZ_ITermViewStart(wptr)     ((wptr)->iterm.view_start)
#define EZ_ITermNScrolled(wptr)     ((wptr)->iterm.nscrolled)
#define EZ_ITermSaveLines(wptr)     ((wptr)->iterm.savelines)
#define EZ_ITermScreen(wptr)        ((wptr)->iterm.screen)
#define EZ_ITermSaveScreen(wptr)    ((wptr)->iterm.saveScreen)

#define EZ_ITermRStyle(wptr)        ((wptr)->iterm.rstyle)
#define EZ_ITermRVideo(wptr)        ((wptr)->iterm.rvideo)
#define EZ_ITermRVideoPending(wptr) ((wptr)->iterm.rvideoPending)
#define EZ_ITermDirty(wptr)         ((wptr)->iterm.dirty)
#define EZ_ITermSelection(wptr)     ((wptr)->iterm.selection)
#define EZ_ITermControl(wptr)       ((wptr)->iterm.control)
#define EZ_ITermPrivateModes(wptr)  ((wptr)->iterm.privateModes)
#define EZ_ITermSaveModes(wptr)     ((wptr)->iterm.saveModes)
#define EZ_ITermSavedModes(wptr)    ((wptr)->iterm.saveModes)
#define EZ_ITermChildPid(wptr)      ((wptr)->iterm.childPid)
#define EZ_ITermTimer(wptr)         ((wptr)->iterm.timer)
#define EZ_ITermBTimer(wptr)        ((wptr)->iterm.btimer)
/************************************************************************************
 *
 *   Terminal Emulator
 *
 ************************************************************************************/

typedef struct EZ_Term_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for Terminal emulator
   *------------------------------------*/  
  union EZ_WWindow_ *vscroll;        /* the scroll bar          */
  union EZ_WWindow_ *iterm;           /* textwidget assoc. to    */

} EZ_TermWidget;

#define EZ_TermITerm(wptr)         ((wptr)->term.iterm)
#define EZ_TermVScroll(wptr)       ((wptr)->term.vscroll)


/************************************************************************************
 *
 *   Ruler
 *
 ************************************************************************************/

typedef struct EZ_Ruler_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for Ruler
   *------------------------------------*/  
  int  twidth, theight, ascent; /* font tick label */
  int  tickSize, tickLength;    /* tick spec       */
  int  bigTick;                 /*  */
  float tickErr;
  int  toffset;                 /* tickmarker offset */
  int  scale;                   /* scale factor for numerical ticks */
  int  xy[8];                   /* marker location */
  int  mark;
  char mkstatus[4];

  unsigned char orientation;             /* ruler location */
  unsigned char dispValue;
  unsigned char pad[2];
  void *timer;

} EZ_Ruler;

#define EZ_RulerFontHeight(wptr)         ((wptr)->ruler.theight)
#define EZ_RulerFontWidth(wptr)          ((wptr)->ruler.twidth)
#define EZ_RulerFontAscent(wptr)         ((wptr)->ruler.ascent)
#define EZ_RulerDisplayValue(wptr)       ((wptr)->ruler.dispValue)
#define EZ_RulerTickSize(wptr)           ((wptr)->ruler.tickSize)
#define EZ_RulerBigTick(wptr)            ((wptr)->ruler.bigTick)
#define EZ_RulerTickError(wptr)          ((wptr)->ruler.tickErr)
#define EZ_RulerTickLength(wptr)         ((wptr)->ruler.tickLength)
#define EZ_RulerTickScale(wptr)          ((wptr)->ruler.scale)
#define EZ_RulerOrientation(wptr)        ((wptr)->ruler.orientation)
#define EZ_RulerTOffset(wptr)            ((wptr)->ruler.toffset)
#define EZ_RulerPosition(wptr)           ((wptr)->ruler.xy)
#define EZ_RulerMarkStatus(wptr)         ((wptr)->ruler.mkstatus)
#define EZ_RulerMarker(wptr)             ((wptr)->ruler.mark)
#define EZ_RulerTimer(wptr)              ((wptr)->ruler.timer)

/************************************************************************************
 *
 *   Scroll Button
 *
 ************************************************************************************/

typedef struct EZ_ScrollButton_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for scrollbutton
   *------------------------------------*/  
  int  type;                 /* left,right,up,down,leftRight,upDown */
  int  bdwidth;              /* arrow bd width */
  int  arrowType;            
  int  scrollCount;
  char bdstyle[2];           /* arrow bd type  */
  char highlight[2];
  int  box1[3], box2[3];      
  union EZ_WWindow_ *spin;   /* spin button connected to */

} EZ_ScrollButton;

#define EZ_ScrollButtonType(wptr)         ((wptr)->scrollBtn.type)
#define EZ_ScrollButtonBorderWidth(wptr)  ((wptr)->scrollBtn.bdwidth)
#define EZ_ScrollButtonScrollCount(wptr)  ((wptr)->scrollBtn.scrollCount)
#define EZ_ScrollButtonBorderStyle(wptr)  ((wptr)->scrollBtn.bdstyle)
#define EZ_ScrollButtonHighlight(wptr)    ((wptr)->scrollBtn.highlight)
#define EZ_ScrollButtonBox1(wptr)         ((wptr)->scrollBtn.box1)
#define EZ_ScrollButtonBox2(wptr)         ((wptr)->scrollBtn.box2)
#define EZ_ScrollButtonArrowType(wptr)    ((wptr)->scrollBtn.arrowType)
#define EZ_ScrollButtonSpin(wptr)         ((wptr)->scrollBtn.spin)

/************************************************************************************
 *
 *   Spin Button
 *
 ************************************************************************************/

typedef struct EZ_SpinButton_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for spinbutton
   *------------------------------------*/  
  EZ_Widget   *entry;
  EZ_Widget   *sbutton;
  int         current;     /* current scroll position */
  void        *clientData;
  char        *(*next)(int current, int next, void *cdata);  

} EZ_SpinButton;

#define EZ_SpinButtonEntry(wptr)          ((wptr)->spinBtn.entry)
#define EZ_SpinButtonSButton(wptr)        ((wptr)->spinBtn.sbutton)
#define EZ_SpinButtonCurrent(wptr)        ((wptr)->spinBtn.current)
#define EZ_SpinButtonCData(wptr)          ((wptr)->spinBtn.clientData)
#define EZ_SpinButtonNextF(wptr)          ((wptr)->spinBtn.next)

/************************************************************************************
 *
 *  Locator
 *
 ************************************************************************************/

typedef struct EZ_Locator_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for locator
   *------------------------------------*/  
  float xx, yy;  /* locator value */
  
} EZ_Locator;

#define EZ_LocatorXX(wptr)     ((wptr)->locator.xx)
#define EZ_LocatorYY(wptr)     ((wptr)->locator.yy)


/************************************************************************************
 *
 *  Dial
 *
 ************************************************************************************/
typedef struct EZ_Dial_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for Dial
   *------------------------------------*/  
  char              *label;           /* label if any            */
  int               label_length;     /* length of label         */
  int               lwidth;           /* label width in pixels   */
  int               lheight;
  int               ascent;           /*  font ascent            */

  float             from, to, resolution;
  float             span;          /* angle in degrees */
  float             offset, angle; /* angle in degrees */

  float             value, factor;
  float             aratio; 
  int               theight, twidth, hadj;
  int               cx, cy, radius; /* center */
  char              format[8];
  unsigned long     fg;
  unsigned long     tfg;

  int               nticks, tickSize;
  unsigned char     showValue;
  unsigned char     noCValue;
  unsigned char     vset;
  unsigned char     style;
  
} EZ_Dial;

#define EZ_DialLabel(wptr)         ((wptr)->dial.label)
#define EZ_DialLabelLength(wptr)   ((wptr)->dial.label_length)
#define EZ_DialLabelWidth(wptr)    ((wptr)->dial.lwidth)
#define EZ_DialLabelHeight(wptr)   ((wptr)->dial.lheight)
#define EZ_DialAscent(wptr)        ((wptr)->dial.ascent)

#define EZ_DialRadius(wptr)        ((wptr)->dial.radius)
#define EZ_DialCenterX(wptr)       ((wptr)->dial.cx)
#define EZ_DialCenterY(wptr)       ((wptr)->dial.cy)
#define EZ_DialValue(wptr)         ((wptr)->dial.value)
#define EZ_DialAngle(wptr)         ((wptr)->dial.angle)
#define EZ_DialARatio(wptr)        ((wptr)->dial.aratio)
#define EZ_DialTHeight(wptr)       ((wptr)->dial.theight)
#define EZ_DialTWidth(wptr)        ((wptr)->dial.twidth)
#define EZ_DialHAdjustment(wptr)   ((wptr)->dial.hadj)
#define EZ_DialForeground(wptr)    ((wptr)->dial.fg)
#define EZ_DialTForeground(wptr)   ((wptr)->dial.tfg)
#define EZ_DialFrom(wptr)          ((wptr)->dial.from)
#define EZ_DialTo(wptr)            ((wptr)->dial.to)
#define EZ_DialFactor(wptr)        ((wptr)->dial.factor)
#define EZ_DialResolution(wptr)    ((wptr)->dial.resolution)
#define EZ_DialFormat(wptr)        ((wptr)->dial.format)
#define EZ_DialNTicks(wptr)        ((wptr)->dial.nticks)
#define EZ_DialTickSize(wptr)      ((wptr)->dial.tickSize)
#define EZ_DialSpan(wptr)          ((wptr)->dial.span)
#define EZ_DialOffset(wptr)        ((wptr)->dial.offset)
#define EZ_DialStyle(wptr)         ((wptr)->dial.style)
#define EZ_DialShowValue(wptr)     ((wptr)->dial.showValue)
#define EZ_DialNoCValue(wptr)      ((wptr)->dial.noCValue)
#define EZ_DialVSet(wptr)          ((wptr)->dial.vset)

/************************************************************************************
 *
 * GradientBar
 *
 ************************************************************************************/
  
typedef struct EZ_GradientBar_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for gradientBar
   *------------------------------------*/  
  unsigned long     colors[12];         /* starting and ending colors */
  int               ncolors;
  float             range[2];           /* range */
  float             value;              /* current value */
  float             factor;             /* value->screen conversion factor */
  float             resolution;
  int               pwidth, pheight;    /* last width and height */
  Pixmap            theBar;             /* gbar */
  int               bwset;
  int               bwidth;             /* slider width */
  int               position;           /* position of the bar */
  
  unsigned char     orientation;
  unsigned char     orien_save;
  unsigned char     style;
  unsigned char     editable;
} EZ_GradientBar;

#define EZ_GBarColors(wptr)              ((wptr)->gbar.colors)
#define EZ_GBarNColors(wptr)             ((wptr)->gbar.ncolors)
#define EZ_GBarRange(wptr)               ((wptr)->gbar.range)
#define EZ_GBarValue(wptr)               ((wptr)->gbar.value)
#define EZ_GBarFactor(wptr)              ((wptr)->gbar.factor)
#define EZ_GBarResolution(wptr)          ((wptr)->gbar.resolution)
#define EZ_GBarPixmap(wptr)              ((wptr)->gbar.theBar)
#define EZ_GBarPWidth(wptr)              ((wptr)->gbar.pwidth)
#define EZ_GBarPHeight(wptr)             ((wptr)->gbar.pheight)
#define EZ_GBarOrientation(wptr)         ((wptr)->gbar.orientation)
#define EZ_GBarOrientationOld(wptr)      ((wptr)->gbar.orien_save)
#define EZ_GBarStyle(wptr)               ((wptr)->gbar.style)
#define EZ_GBarEditable(wptr)            ((wptr)->gbar.editable)
#define EZ_GBarBWidth(wptr)              ((wptr)->gbar.bwidth)
#define EZ_GBarBWidthSet(wptr)           ((wptr)->gbar.bwset)
#define EZ_GBarPosition(wptr)            ((wptr)->gbar.position)


/************************************************************************************
 *
 * Histogram
 *
 ************************************************************************************/
  
typedef struct EZ_Histogram_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for histogram
   *------------------------------------*/  
  int               nbins;      /* number of bins or -1 */
  int               nbinsUsed;
  int               binwidth;   /* width of bar */
  int               binwidthUsed; /* width of bar */
  int               ipadUsed;
  int               selection;
  float             range[2];
  float             *values;
  int               nvalues;
  int               cbin;
  int               ncolors;
  int               clrcells;
  unsigned long     *colors;  
  unsigned char     orientation;
  unsigned char     saveOrien;
} EZ_Histogram;

#define EZ_HgramNBins(wptr)             ((wptr)->histogram.nbins)
#define EZ_HgramNBinsUsed(wptr)         ((wptr)->histogram.nbinsUsed)
#define EZ_HgramBinWidth(wptr)          ((wptr)->histogram.binwidth)
#define EZ_HgramBinWidthUsed(wptr)      ((wptr)->histogram.binwidthUsed)
#define EZ_HgramSelection(wptr)         ((wptr)->histogram.selection)
#define EZ_HgramCBin(wptr)              ((wptr)->histogram.cbin)
#define EZ_HgramRange(wptr)             ((wptr)->histogram.range)
#define EZ_HgramValues(wptr)            ((wptr)->histogram.values)
#define EZ_HgramNValues(wptr)           ((wptr)->histogram.nvalues)
#define EZ_HgramColors(wptr)            ((wptr)->histogram.colors)
#define EZ_HgramNColors(wptr)           ((wptr)->histogram.ncolors)
#define EZ_HgramNColorCells(wptr)       ((wptr)->histogram.clrcells)
#define EZ_HgramOrientation(wptr)       ((wptr)->histogram.orientation)
#define EZ_HgramOrientationOld(wptr)    ((wptr)->histogram.saveOrien)
#define EZ_HgramBinSep(wptr)            ((wptr)->histogram.ipadUsed)

/***********************************************************************
 ***                                                                 ***
 ***                     The Separator Widget                        ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_Separator_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for histogram
   *------------------------------------*/  
  int               width;
  int               style;
} EZ_Separator;
#define EZ_SeparatorWidth(wptr)            ((wptr)->separator.width)
#define EZ_SeparatorStyle(wptr)            ((wptr)->separator.style)

/***********************************************************************
 ***                                                                 ***
 ***                     The Spreadsheet Widget                      ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_SSItem_ {
  ITextLine          *text;
  int                rowOrWidth;
  int                colOrHeight;
  unsigned char      just;    /* justification */
  unsigned char      unused;
  unsigned char      dirty;
  unsigned char      status; 
  struct EZ_SSItem_  *next;
} EZ_SSItem;

typedef struct EZ_SpreadSheet_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for histogram
   *------------------------------------*/  
  int               tx, ty; /* tile origin */
  short             cwidth, cheight; /* default cell width and height */
  short             x0, y0, x1, y1; /* index of the first cell displayed*/
  short             sx0,sy0, sx1,sy1;  /* selection */
  short             cx, cy;  /* current active cell */
  short             tabx, taby;
  short             nrows, ncolumns; /* grid size */
  short             bwpad, curx;
  short             ncharts, acharts;
  TextProp          *property;
  EZ_SSItem         *aitem;  /* active item */
  EZ_SSItem         *headrow;
  EZ_SSItem         *data;
  unsigned long     textbg;
  unsigned long     selectionBg;
  unsigned long     selectionFg;
  unsigned long     dfg, dbg;
  EZ_Item           **charts, *schart; /* fig items */
  union EZ_WWindow_ *hscroll, *vscroll;
  char              tab;
  char              newline;
  char              showhead;
  char              dirty;
  unsigned char     selectionMode;
  unsigned char     hmode;
  unsigned char     theight, twidth;
} EZ_SpreadSheet;

#define EZ_SSheetHMode(wptr)         ((wptr)->spreadSheet.hmode)
#define EZ_SSheetHScroll(wptr)       ((wptr)->spreadSheet.hscroll)
#define EZ_SSheetVScroll(wptr)       ((wptr)->spreadSheet.vscroll)
#define EZ_SSheetTabChar(wptr)       ((wptr)->spreadSheet.tab)
#define EZ_SSheetTabChar(wptr)       ((wptr)->spreadSheet.tab)
#define EZ_SSheetNewlineChar(wptr)   ((wptr)->spreadSheet.newline)
#define EZ_SSheetSChart(wptr)        ((wptr)->spreadSheet.schart)
#define EZ_SSheetCharts(wptr)        ((wptr)->spreadSheet.charts)
#define EZ_SSheetNCharts(wptr)       ((wptr)->spreadSheet.ncharts)
#define EZ_SSheetACharts(wptr)       ((wptr)->spreadSheet.acharts)
#define EZ_SSheetTabX(wptr)          ((wptr)->spreadSheet.tabx)
#define EZ_SSheetTabY(wptr)          ((wptr)->spreadSheet.taby)
#define EZ_SSheetCursorX(wptr)       ((wptr)->spreadSheet.curx)
#define EZ_SSheetActiveItem(wptr)    ((wptr)->spreadSheet.aitem)
#define EZ_SSheetTextProperty(wptr)  ((wptr)->spreadSheet.property)
#define EZ_SSheetDFG(wptr)           ((wptr)->spreadSheet.dfg)
#define EZ_SSheetDBG(wptr)           ((wptr)->spreadSheet.dbg)
#define EZ_SSheetBwPad(wptr)         ((wptr)->spreadSheet.bwpad)
#define EZ_SSheetCx(wptr)            ((wptr)->spreadSheet.cx)
#define EZ_SSheetCy(wptr)            ((wptr)->spreadSheet.cy)
#define EZ_SSheetSx0(wptr)           ((wptr)->spreadSheet.sx0)
#define EZ_SSheetSy0(wptr)           ((wptr)->spreadSheet.sy0)
#define EZ_SSheetSx1(wptr)           ((wptr)->spreadSheet.sx1)
#define EZ_SSheetSy1(wptr)           ((wptr)->spreadSheet.sy1)
#define EZ_SSheetTx(wptr)            ((wptr)->spreadSheet.tx)
#define EZ_SSheetTy(wptr)            ((wptr)->spreadSheet.ty)
#define EZ_SSheetShowHead(wptr)      ((wptr)->spreadSheet.showhead)
#define EZ_SSheetFont(wptr)          ((wptr)->common.font)
#define EZ_SSheetX0(wptr)            ((wptr)->spreadSheet.x0)
#define EZ_SSheetY0(wptr)            ((wptr)->spreadSheet.y0)
#define EZ_SSheetX1(wptr)            ((wptr)->spreadSheet.x1)
#define EZ_SSheetY1(wptr)            ((wptr)->spreadSheet.y1)
#define EZ_SSheetNRows(wptr)         ((wptr)->spreadSheet.nrows)
#define EZ_SSheetNColumns(wptr)      ((wptr)->spreadSheet.ncolumns)
#define EZ_SSheetTWidth(wptr)        ((wptr)->spreadSheet.twidth)
#define EZ_SSheetTHeight(wptr)       ((wptr)->spreadSheet.theight)
#define EZ_SSheetCellWidth(wptr)     ((wptr)->spreadSheet.cwidth)
#define EZ_SSheetCellHeight(wptr)    ((wptr)->spreadSheet.cheight)
#define EZ_SSheetHeadRow(wptr)       ((wptr)->spreadSheet.headrow)
#define EZ_SSheetData(wptr)          ((wptr)->spreadSheet.data)
#define EZ_SSheetTextBG(wptr)        ((wptr)->spreadSheet.textbg)
#define EZ_SSheetSelectionBG(wptr)   ((wptr)->spreadSheet.selectionBg)
#define EZ_SSheetSelectionFG(wptr)   ((wptr)->spreadSheet.selectionFg)
#define EZ_SSheetDirty(wptr)         ((wptr)->spreadSheet.dirty)
#define EZ_SSheetSelectionMode(wptr) ((wptr)->spreadSheet.selectionMode)

#define EZ_SSheetRowBG(wptr)         ((wptr)->spreadSheet.rowBg)
#define EZ_SSheetRowBGSet(wptr)      ((wptr)->spreadSheet.rowBgSet)


/***********************************************************************
 ***                                                                 ***
 ***                     The ThumbWheel Widget                        ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_ThumbWheel_ {
  char common[sizeof(EZ_WidgetCommon)]; /* common block          */
  /*-------------------------------------
   *  Private data for histogram
   *------------------------------------*/  
  float  from, to, value, fincr;     /* range: default 0-100 ... */
  float  offset, factor;
  int    width, length;
  int    ioffset, nbars;       
} EZ_ThumbWheel;

#define EZ_ThumFactor(wptr)       ((wptr)->ThumbWheel.factor)
#define EZ_ThumOffset(wptr)       ((wptr)->ThumbWheel.offset)
#define EZ_ThumIOffset(wptr)      ((wptr)->ThumbWheel.ioffset)
#define EZ_ThumFrom(wptr)         ((wptr)->ThumbWheel.from)
#define EZ_ThumTo(wptr)           ((wptr)->ThumbWheel.to)
#define EZ_ThumFIncr(wptr)        ((wptr)->ThumbWheel.fincr)
#define EZ_ThumValue(wptr)        ((wptr)->ThumbWheel.value)
#define EZ_ThumbWidth(wptr)        ((wptr)->ThumbWheel.width)
#define EZ_ThumLength(wptr)       ((wptr)->ThumbWheel.length)
#define EZ_ThumNBars(wptr)        ((wptr)->ThumbWheel.nbars)

/***********************************************************************
 ***                                                                 ***
 ***                     The Widget Union                            ***
 ***                                                                 ***
 ***********************************************************************/

union EZ_WWindow_ {
  EZ_WidgetCommon    common;
  EZ_UnknownWidget   unknownWidget;
  EZ_FrameWidget     frame;
  EZ_LabelWidget     label;
  EZ_ButtonWidget    button;
  EZ_CButtonWidget   cbutton;
  EZ_RButtonWidget   rbutton;
  EZ_MButtonWidget   mbutton;
  EZ_SliderWidget    slider;
  EZ_MenuSeparator   mseparator;
  EZ_MenuSubMenu     msubmenu;
  EZ_PopupMenu       popupmenu;
  EZ_EntryWidget     entry;
  EZ_OptEntryWidget  optEntry;
  EZ_ScrollbarWidget scrollbar;
  EZ_3D_Canvas       threedcanvas;
  EZ_IListBoxWidget  ilistbox;       
  EZ_ListBoxWidget   listbox;       
  EZ_ITextWidget     itext;
  EZ_TextWidget      text;
  EZ_NBPage          nbpage;
  EZ_NoteBook        notebook;
  EZ_PaneHandle      panehandle;
  EZ_Embeder         Embeder;
  EZ_FileSelector    fileSelector;
  EZ_ITreeWidget     itree;
  EZ_TreeWidget      tree;
  EZ_IFancyListBox   iflistbox;
  EZ_FancyListBox    flistbox;
  EZ_IWorkArea       iworkArea;
  EZ_WorkArea        workArea;
  EZ_RawXWindow      rawxwindow;
  EZ_MenuTearOffBar  tearoffbar;
  EZ_LcdWidget       lcd;
  EZ_LedWidget       led;
  EZ_StatusMeter     smeter;
  EZ_FrameWidget     mbar;
  EZ_ITermWidget     iterm;
  EZ_TermWidget      term;
  EZ_Ruler           ruler;
  EZ_ScrollButton    scrollBtn;
  EZ_SpinButton      spinBtn;
  EZ_Locator         locator;
  EZ_Dial            dial;
  EZ_GradientBar     gbar;
  EZ_Histogram       histogram;
  EZ_Separator       separator;
  EZ_SpreadSheet     spreadSheet;
  EZ_ThumbWheel       ThumbWheel;
};

/***********************************************************************
 ***                                                                 ***
 ***        Data Structure for Widget Handling                       ***
 ***                                                                 ***
 ***  There are 4 handling functions for each widget,                ***
 ***                                                                 *** 
 ***  void        ComputeSize();                                     ***
 ***  void        DrawWidget();                                      ***
 ***  void        FreeData();                                        ***
 ***  void        EventsHandle();                                    ***
 ***                                                                 *** 
 ***********************************************************************/

typedef struct EZ_WidgetHandle_ {
  void  (*ComputeSize) MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
  void  (*DrawWidget)  MY_ANSIARGS((EZ_Widget *widget));
  void  (*FreeData)    MY_ANSIARGS((EZ_Widget *widget));
  void  (*EventHandle) MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
} EZ_WidgetHandle;

/***********************************************************************
 ***                                                                 ***
 *** The data structure of two simple hash tables. Used to store     ***
 *** pointers for simple loopup of widgets.                          ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct EZ_WidgetListEntry_ {
  union EZ_WWindow_          *widget;
  struct EZ_WidgetListEntry_ *next;
} EZ_WidgetListEntry;
typedef EZ_WidgetListEntry   EZ_MappedWidgetHTEntry;
typedef EZ_WidgetListEntry   EZ_AllWidgetHTEntry;
typedef EZ_WidgetListEntry   EZ_DnDWidgetListEntry;

/***********************************************************************
 ***                                                                 ***
 ***            Data structure for border pieces.                    ***
 ***                                                                 ***
 ***********************************************************************/

typedef struct {
  XPoint  xp[4];
  GC      gc;
} EZ_BorderPiece;

/***********************************************************************
 ***                                                                 ***
 ***            Data structure used in registry                      ***
 ***                                                                 ***
 ***********************************************************************/
typedef struct _ApplRoster {
  char grabServer;
  char needReset;
  char needXFree;
  char pad;
  unsigned long length;
  char *data;
} EZ_ApplRoster;


/***********************************************************************
 ***                                                                 ***
 ***                    Global Variables                             ***
 ***                                                                 ***
 ***********************************************************************/

/* atoms for Drag and Drop */
#ifndef _EZ_DND_C_
#define EZ_DefineDnDAtom(a,b)  extern Atom a ;
#include "EZ_DnDAtom.h"
#undef EZ_DefineDnDAtom
#endif

#ifndef _EZ_WIDGET_INIT_C_
EXTERN XSInfo  XServerInfo;
EXTERN EZ_WidgetHandle  *EZ_WidgetHandlingFunctions[EZ_TOTAL_NUMBER_OF_WIDGETS];
EXTERN EZ_BitmapHTEntry  *EZ_SnapShotBitmap;
EXTERN EZ_Widget         *EZ_BubbleHelp;
EXTERN EZ_Widget         *EZ_CurrentFlashWidget;
EXTERN EZ_Widget         *EZ_CurrentFlashWidget2;
EXTERN Time              EZ_LastButton1ClickTime;
EXTERN EZ_Widget         *EZ_LastDropSite;
EXTERN EZ_DnDInfoStruct  EZ_DnDInfo;

EXTERN char     EZ_HighlightEnabled;
EXTERN char     EZ_OffsetEnabled;
EXTERN char     EZ_DepresionDisabled;
EXTERN char     EZ_BubbleHelpOn;

EXTERN char     EZ_PTLabel;
EXTERN char     EZ_PointerGrabed;
EXTERN char     EZ_LabelFontIsBold;

EXTERN int      EZ_ServerGrabed;

EXTERN Atom     EZ_WM_DELETE_WINDOW_ATOM;
EXTERN Atom     EZ_WM_PROTOCOLS_ATOM;
EXTERN Atom     EZ_DND_DRAG_INIT_ATOM;
EXTERN Atom     EZ_DND_DRAG_START_ATOM;
EXTERN Atom     EZ_DND_DRAG_FINISH_ATOM;
EXTERN Atom     EZ_DND_DROP_START_ATOM;
EXTERN Atom     EZ_DND_DROP_FINISH_ATOM;
EXTERN Atom     EZ_MESSAGE_ATOM;
EXTERN Atom     EZ_RESOURCE_EDITOR_ATOM;

EXTERN unsigned long  EZ_ApplicationSetupTime;
EXTERN unsigned long  EZ_KbdFocusHighlightColor;
EXTERN EZ_EventHandlerStruct EZ_ClientMessageHandler;
EXTERN EZ_EventHandlerStruct *EZ_NoWidgetEventHandlers;
EXTERN EZ_EventFilter  EZ_GlobalEventFilter;
EXTERN void            *EZ_GlobalEventFilterData;
EXTERN void  (*EZ_CreateNewWidgetHook) MY_ANSIARGS((EZ_Widget *widget));
#endif

#ifndef  _EZ_WIDGET_DISP_C_
EXTERN int EZ_DisplayIsFreezed;
#endif

#ifndef _EZ_WIDGET_GC_C_
EXTERN GC   EZ_GCList[];
#endif

#ifndef  _EZ_WIDGET_POPUP_C_
EXTERN EZ_Widget        *EZ_CurrentPopupMenu;
#endif

#ifndef  _EZ_X11INIT_C_
EXTERN int EZ_XErrorCode;
EXTERN Window EZ_DnDCommuWin, EZ_DnDParentWin;
EXTERN int EZ_GeometryX, EZ_GeometryY, EZ_GeometryW, EZ_GeometryH,EZ_GeometrySet; 
EXTERN int EZ_EmbedingDepth;
#endif

#ifndef _EZ_FOCUS_C_
EXTERN EZ_Widget *EZ_CurrentFocusingWidget;
EXTERN EZ_Widget *EZ_PendingFocus;
EXTERN int        EZ_WeAreInFocus;
#endif

#ifndef _EZ_DND_C_
EXTERN EZ_DnDWidgetListEntry  EZ_DnDWidgetList;
#endif

#ifndef _EZ_RESOURCE_C_
EXTERN XrmDatabase EZ_Xrm_db;
EXTERN XrmQuark EZ_OptionsNQs[EZ_WIDGET_CONFIGURE_LAST_OPTION+1];
EXTERN XrmQuark EZ_OptionsCQs[EZ_WIDGET_CONFIGURE_LAST_OPTION+1];
#endif

#ifndef _EZ_WIDGET_GRAB_C_
EXTERN EZ_Widget *EZ_GrabedWidget;
#endif

#ifndef _EZ_WIDGET_CONFIGURE_C_
EXTERN char *EZ_ConfigureOptionNames[];
EXTERN char *EZ_ConfigureOptionTypes[];
#endif

#ifndef _EZ_WIDGET_RAWXWIN_C_
EXTERN EZ_ResourceHandle *EZ_WResourceHandles[EZ_TOTAL_NUMBER_OF_WIDGETS];
EXTERN EZ_ResourceHandle *EZ_ApplicationResourceHandle;
#endif

/*************************************************************************
 *
 */
/*
 *  From EZ_Widget.c
 */
#ifndef _EZ_WIDGET_C_   
EXTERN  EZ_Widget             *EZ_CreateNewWidget MY_ANSIARGS((EZ_Widget *parent));
EXTERN  void                  EZ_DestroyWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_DestroyWidgetFinal MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_DestroyCloneWidget MY_ANSIARGS((EZ_Widget *widget,int l));
EXTERN  void                  EZ_RemoveWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_DestroyWidgetReal MY_ANSIARGS((EZ_Widget *widget, int level));
EXTERN  void                  EZ_GetWidgetLabelPosition MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));
EXTERN  void                  EZ_MarkAllChildrenWidgetSizeChanged MY_ANSIARGS((EZ_Widget *widget, int flag));
EXTERN  void                  EZ_FigureLabelExtent MY_ANSIARGS((XFontStruct  *font_info,
								char         *string,
								int          length, int linelength,
								int          *numlines, int *twidth
								));
EXTERN void                  EZ_FigureLabelExtentWork MY_ANSIARGS((XFontStruct  *font_info,
								   char         *string,
								   int          length, int linelength,
								   int          *numlines, int *twidth,
								   char         **idxes
								   ));
EXTERN  void                  EZ_RenderLabelText MY_ANSIARGS((Drawable drawable,
							      GC       gc,
							      int      x, int y,
							      int height, 
							      char     *string,
							      int length,
							      int linelength, int nlines,
							      int twidth, int justification,
							      XFontStruct  *font_info, int ss
							      ));
EXTERN  void                  EZ_RenderLabelTextDisabled MY_ANSIARGS((Drawable drawable,
								      GC       BTgc, GC DKgc, GC Ngc,
								      int      x, int y,
								      int height, 
								      char     *string,
								      int length,
								      int linelength, int nlines,
								      int twidth, int justification,
								      XFontStruct  *font_info
								      ));

EXTERN  void                  EZ_RenderPixmapLabel MY_ANSIARGS((EZ_Widget *wptr,
								EZ_Bitmap *bitmap,
								Drawable  drawable,
								int       x,int y
								));

EXTERN  void                  EZ_RenderPixmapLabelWithGC  MY_ANSIARGS((EZ_Bitmap *bitmap,
								       Drawable  drawable,
								       GC        gc,
								       int       x,int y
								       ));
EXTERN  void                  EZ_MoveWidgetWindow MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN  void                  EZ_ResizeWidgetWindow MY_ANSIARGS((EZ_Widget *widget, int w, int h));
EXTERN  void                  EZ_MoveResizeWidgetWindow MY_ANSIARGS((EZ_Widget *widget, 
                                                             int x, int y, int w, int h));
EXTERN  void                  EZ_SetWidgetPosition MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN  void                  EZ_SetWidgetWidth MY_ANSIARGS((EZ_Widget *widget, int width));
EXTERN  void                  EZ_SetWidgetHeight MY_ANSIARGS((EZ_Widget *widget, int height));
EXTERN  void                  EZ_GetWidgetPosition MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));
EXTERN  int                   EZ_GetWidgetWidth MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int                   EZ_GetWidgetHeight MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int                   EZ_GetWidgetMinWidth MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int                   EZ_GetWidgetMinHeight MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int                   EZ_GetWidgetBorderWidth MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_GetWidgetPaddings MY_ANSIARGS((EZ_Widget *wg, int *px, int *py, int *pb));
EXTERN  void                  EZ_GetWidgetBorder MY_ANSIARGS((EZ_Widget *wg, int *bw, int *bs));
EXTERN  void                  EZ_GetWidgetDimension MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN  void                  EZ_GetWidgetSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN  void                  EZ_GetWidgetGeometry MY_ANSIARGS((EZ_Widget *widget, int *x,int *y, int *w, int *h));
EXTERN  void                  EZ_GetWidgetAbsoluteGeometry MY_ANSIARGS((EZ_Widget *widget, int *x,int *y, int *w, int *h));
EXTERN  void                  EZ_GetWidgetMinDimension MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN  void                  EZ_DisableWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_EnableWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_DisableWidgetTree MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_EnableWidgetTree MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int                   EZ_SetWidgetLabelString MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN  int                   EZ_GetWidgetIntData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                  EZ_GetWidgetClientData MY_ANSIARGS((EZ_Widget *widget, int *i, void **p));
EXTERN  void                  *EZ_GetWidgetPtrData MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int                   EZ_GetWidgetReturnedData MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int                   EZ_GetWidgetReturnData MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_SetWidgetIntData MY_ANSIARGS((EZ_Widget *widget, int idata));
EXTERN  void                  EZ_SetWidgetPtrData MY_ANSIARGS((EZ_Widget *widget, void *ptr));
EXTERN void                   EZ_SetWidgetClientData MY_ANSIARGS((EZ_Widget *widget, int i, void *p));
EXTERN  void                  EZ_HideWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_HideWidgetWork MY_ANSIARGS((EZ_Widget *widget, int rec));
EXTERN  void                  EZ_DeActivateWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_ActivateWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_DeActivateChildren MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_ActivateChildren MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_DeActivateSibling MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_ActivateSibling MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_FreezeWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_UnFreezeWidget MY_ANSIARGS((EZ_Widget *widget));

EXTERN  int                   EZ_GetBackgroundGC MY_ANSIARGS((EZ_Widget *widget,
						      GC *gc, unsigned long *pv,
						      int highlight, int specialhighlight));
EXTERN int                   EZ_GetBackgroundTileGC MY_ANSIARGS((EZ_Widget *widget,
                                                          GC *gc, unsigned long *pv,
                                                          int highlight, int specialhighlight));
EXTERN int                    EZ_GetParentBgGC MY_ANSIARGS((EZ_Widget *widget,
							     GC *gc, unsigned long *pv));
EXTERN int                    EZ_GetParentBgTileGC MY_ANSIARGS((EZ_Widget *widget,
                                                                GC *gc, unsigned long *pv));
EXTERN  int                   EZ_GetBackgroundPV MY_ANSIARGS((EZ_Widget *widget,
						      unsigned long *pv, 
						      int hlight, int specialhlight));
EXTERN  int                   EZ_GetDarkBDGC MY_ANSIARGS((EZ_Widget *widget, GC *gc));
EXTERN  int                   EZ_GetBrightBDGC MY_ANSIARGS((EZ_Widget *widget, GC *gc));
EXTERN  int                   EZ_GetDarkBrightNormalBDGC MY_ANSIARGS((EZ_Widget *widget, GC *dk, GC *bt, GC *ngc));
EXTERN  void                  EZ_GetDarkBDpv MY_ANSIARGS((EZ_Widget *widget, unsigned long *pv));
EXTERN  void                  EZ_GetBrightBDpv MY_ANSIARGS((EZ_Widget *widget, unsigned long *pv));
EXTERN  void                  EZ_GetWidgetTileOrigin MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));

EXTERN  int                   EZ_GetLabel2XPosition MY_ANSIARGS((EZ_Widget *widget,int l2width, int offset));
EXTERN  int                   EZ_GetWidgetOrientation MY_ANSIARGS((EZ_Widget *widget));

EXTERN  Window                EZ_GetWidgetWindow MY_ANSIARGS((EZ_Widget *widget));


EXTERN  void                  EZ_SetWidgetCallBack MY_ANSIARGS((EZ_Widget *widget, EZ_CallBack callback, void *data));
EXTERN  void                  EZ_AddWidgetCallBack MY_ANSIARGS((EZ_Widget *widget, int wlist,
							EZ_CallBack callback, void *data,int where));
EXTERN  void                  EZ_AddWidgetCallBackInternal MY_ANSIARGS((EZ_Widget *widget, int wlist,
								EZ_CallBack callback, void *data,int where));
EXTERN  void                  EZ_RemoveWidgetCallBack MY_ANSIARGS((EZ_Widget *widget, int wlist,
							   EZ_CallBack callback, void *data));
EXTERN  void                  EZ_RemoveAllWidgetCallBacks MY_ANSIARGS((EZ_Widget *widget, int wlist));

EXTERN  void                  EZ_AddCallBackWork MY_ANSIARGS((EZ_CallBackStruct **list, EZ_CallBack callback,
						      void *data, int where, int flag));
EXTERN  void                  EZ_RemoveCallBackWork MY_ANSIARGS((EZ_CallBackStruct **list, EZ_CallBack callback, void *data));
EXTERN  void                  EZ_RemoveAllCallBacksWork MY_ANSIARGS((EZ_CallBackStruct **list ));

EXTERN  void                  EZ_AddEventHandler MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler,
						      void *data, int where));
EXTERN void                   EZ_AddEventHandlerAll MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler,
                                                       void *data, int where));
EXTERN  void                  EZ_RemoveEventHandler MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler, void *data));
EXTERN  void                  EZ_RemoveAllEventHandlers MY_ANSIARGS((EZ_Widget *widget));

EXTERN  int                   EZ_GetWidgetUnknownData MY_ANSIARGS((EZ_Widget *widget, int idx,
								   EZ_UnknownDataType *ret));
EXTERN  void                  EZ_SetWidgetUnknownData  MY_ANSIARGS((EZ_Widget *widget, int idx,
								    EZ_UnknownDataType data));
EXTERN EZ_UnknownDataType     *EZ_GetWidgetUnknownDataPtr MY_ANSIARGS((EZ_Widget *widget));

EXTERN EZ_Widget             *EZ_GetParentWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget             *EZ_GetChildrenWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget             *EZ_GetSiblingWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Bitmap             *EZ_GetWidgetPixmap MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                  EZ_GetDetachedInferior  MY_ANSIARGS((EZ_Widget *widget, int sib, int *idx,
						  EZ_Widget **storage));
EXTERN void                  EZ_HighlightWidget MY_ANSIARGS((EZ_Widget *widget, int flag));
EXTERN EZ_CallBackStruct     *EZ_TheIthCallBack MY_ANSIARGS((EZ_CallBackStruct **funcList, int i));
EXTERN EZ_CallBackStruct     **EZ_GetCallbackType MY_ANSIARGS((EZ_Widget *widget, int which));
EXTERN EZ_EventHandlerStruct **GetWidgetEventHandlers MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                  EZ_AddWidgetCallBack00 MY_ANSIARGS((EZ_Widget *widget, int wlist,
							EZ_CallBack callback, void *data,int where));
EXTERN void                  EZ_RemoveWidgetCallBack00 MY_ANSIARGS((EZ_Widget *widget, int wlist,
                                                               EZ_CallBack callback, void *data));
EXTERN void                  EZ_RemoveAllWidgetCallBacks00 MY_ANSIARGS((EZ_Widget *widget, int wlist));

EXTERN void                  EZ_AddEventHandler00 MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler,
                                                          void *data, int where));
EXTERN void                  EZ_RemoveEventHandler00 MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler, void *data));
EXTERN void                  EZ_RemoveAllEventHandlers00 MY_ANSIARGS((EZ_Widget *widget));
#endif

/*
 * From EZ_WidgetInit.c
 */
#ifndef _EZ_WIDGET_INIT_C_
EXTERN int  EZ_InitializeXrm MY_ANSIARGS((char *className, XrmOptionDescRec *optsTable,
					  int noptions, int *argc, char **argv,
					  char *fbkresrc,int initgl));
EXTERN  int    EZ_Initialize MY_ANSIARGS((int ac, char **av, int flag));
EXTERN  Display *EZ_GetDisplay MY_ANSIARGS((void));
EXTERN  Visual  *EZ_GetVisual MY_ANSIARGS((void));
EXTERN  Colormap EZ_GetColormap MY_ANSIARGS((void));
EXTERN  int      EZ_GetDepth  MY_ANSIARGS((void));
EXTERN  Window   EZ_GetWidgetWindow MY_ANSIARGS((EZ_Widget *widget));
EXTERN  Window  EZ_GetCommunicationWindow MY_ANSIARGS((void));
EXTERN  int     EZ_GetScreen MY_ANSIARGS((void));
EXTERN  void    EZ_GetDisplayInfo MY_ANSIARGS((Display **dpy, int *scrn, Visual **vis, int *depth));
EXTERN  void    EZ_GetDisplaySize MY_ANSIARGS((int *w, int *h));
EXTERN  void    EZ_DisableHighlight MY_ANSIARGS((void));
EXTERN  void    EZ_EnableHighlight MY_ANSIARGS((void));
EXTERN  void    EZ_DisableLabelOffset MY_ANSIARGS((void));
EXTERN  void    EZ_EnableLabelOffset MY_ANSIARGS((void));
EXTERN  void    EZ_DisableSliderDepression MY_ANSIARGS((void));
EXTERN  void    EZ_EnableSliderDepression MY_ANSIARGS((void));
EXTERN  void    EZ_Shutdown MY_ANSIARGS((void));
EXTERN  void    EZ_ShutdownA MY_ANSIARGS((void));
EXTERN  void    EZ_DisableImageDithering MY_ANSIARGS((void));
EXTERN  void    EZ_EnableImageDithering MY_ANSIARGS((void));
EXTERN  void    EZ_AllowDualLabel  MY_ANSIARGS((int));
EXTERN  void    EZ_EnableBubbleHelp MY_ANSIARGS((void));
EXTERN  void    EZ_DisableBubbleHelp MY_ANSIARGS((void));
EXTERN  void    EZ_FlashWidgetWindowBorder MY_ANSIARGS((EZ_Widget *widget, Drawable drawable,
						int x, int y, int w,int h, int idx));
EXTERN  void    EZ_FlashWidgetBorder MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void    EZ_FlashWidgetBorder2 MY_ANSIARGS((EZ_Widget *widget));

EXTERN void    EZ_FixFocusHighlightBorder MY_ANSIARGS((EZ_Widget *wptr, Drawable drawable,
                                                int w, int h, int padb));
EXTERN  void  EZ_ResetGlobalVariables MY_ANSIARGS((void));
EXTERN  void  EZ_SetClientMessageHandler MY_ANSIARGS((EZ_EventHandler handler, void *data ));
EXTERN  void  EZ_SetPropertyNotifyHandler MY_ANSIARGS(( EZ_EventHandler handler, void *data));

EXTERN  void   EZ_ComputeWidgetUnknownSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN  void   EZ_UnkownEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN  void   EZ_DrawWidgetUnknown MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void   EZ_FreeUnknownData MY_ANSIARGS((EZ_Widget *widget));

EXTERN  void   EZ_ComputeItemUnknownSize MY_ANSIARGS((EZ_Item *item, int *w, int *h)); 
EXTERN void   EZ_DrawItemUnknown MY_ANSIARGS((EZ_Widget *widget,
				       Drawable  drawable,
				       int       x0, int y0,
				       EZ_Item   *item,
				       int       Rx, int Ry, int Rw, int Rh));
EXTERN  void   EZ_UpdateItemUnknown MY_ANSIARGS((EZ_Widget *widget,
					 Drawable  drawable,
					 EZ_Item   *item,
					 int       Rx, int Ry, int Rw, int Rh));
EXTERN  void   EZ_HighlightItemUnknown MY_ANSIARGS((EZ_Widget *widget,
					    Drawable  drawable,
					    EZ_Item   *item,
					    unsigned long fg, unsigned long bg,
					    int       Rx, int Ry, int Rw, int Rh));       
EXTERN  void   EZ_UnknownItemData  MY_ANSIARGS((EZ_Item *item));
EXTERN  int    EZ_UnknownItemPick   MY_ANSIARGS((EZ_Item *item, int x, int y));
EXTERN  void   EZ_UpdateBgGCs MY_ANSIARGS((void));
EXTERN  void   EZ_UpdateFgGCs MY_ANSIARGS((unsigned long old));
EXTERN char    *EZ_LibraryVersion MY_ANSIARGS((void));
EXTERN EZ_Widget *EZ_GetBubbleHelp MY_ANSIARGS((void));
EXTERN void   EZ_StartAsIcon MY_ANSIARGS((int));

EXTERN void    EZ_AddIdleCallBack    MY_ANSIARGS((EZ_CallBack cb, void *data));
EXTERN void    EZ_RemoveIdleCallBack MY_ANSIARGS((EZ_CallBack cb, void *data));
EXTERN void    EZ_RemoveAllIdleCallBack MY_ANSIARGS((void));
EXTERN void    EZ_ExecuteIdleCallBacks MY_ANSIARGS((void));
EXTERN void    EZ_SetGlobalEventFilter MY_ANSIARGS((EZ_EventFilter filter, void *data));
EXTERN int     EZ_QueryPixelValue  MY_ANSIARGS((EZ_Widget *widget, int x, int y,
                                         unsigned long *pv));
EXTERN EZ_Widget *EZ_GetWidgetGroupLeader MY_ANSIARGS((EZ_Widget *widget));
EXTERN void       EZ_SetWidgetGroupLeader MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *leader));
EXTERN void       EZ_SetWidgetTreeGroupLeader MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *leader));
EXTERN void     EZ_SetCreateNewWidgetHook MY_ANSIARGS(( void (*hook) MY_ANSIARGS((EZ_Widget *wptr)) ));
EXTERN void EZ_RedrawAll MY_ANSIARGS((void));
#endif

/*
 * From EZ_Misc.c
 */
#ifndef _EZ_MISC_C_
EXTERN void *EZ_Malloc MY_ANSIARGS((int nbytes));
EXTERN void *EZ_Realloc MY_ANSIARGS((void *ptr, int nbytes));
EXTERN char *EZ_AllocCopyString    MY_ANSIARGS((char *str));
EXTERN char *EZ_ReallocCopyString  MY_ANSIARGS((char *ptr, char *str));
EXTERN void  EZ_Free MY_ANSIARGS((void *ptr));
EXTERN void  EZ_SitForALittleBit   MY_ANSIARGS((unsigned long length));
EXTERN void  EZ_OutOfMemory        MY_ANSIARGS((char *msg));
EXTERN void EZ_GLError MY_ANSIARGS((char *func, char *msg));
EXTERN void EZ_GLFatal MY_ANSIARGS((char *func, char *msg));
EXTERN void EZ_SlideOpenWidgetRelativeTo MY_ANSIARGS((EZ_Widget *w, EZ_Widget *r, int dir, int sp));
EXTERN void EZ_SlideOpenWidget MY_ANSIARGS((EZ_Widget *w, int x, int y, int dir, int sp));
EXTERN void EZ_SlideCloseWidget MY_ANSIARGS((EZ_Widget *w, int dir, int sp));
#endif

/*
 * From EZ_WidgetMisc.c
 */
#ifndef _EZ_WIDGET_MISC_C_
EXTERN EZ_Widget *EZ_GetToplevelWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_CallWidgetCallbacks  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_CallWidgetMotionCallbacks MY_ANSIARGS((EZ_Widget *widget));
EXTERN Window EZ_FindClientWindow  MY_ANSIARGS((Window win));
#endif

/*
 * From EZ_WidgetFrame.c
 */
#ifndef _EZ_WIDGET_FRAME_C_
EXTERN EZ_Widget         *EZ_CreateFrame MY_ANSIARGS((EZ_Widget *parent, char *label));
EXTERN void              EZ_DrawWidgetFrame  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_FreeWidgetFrameData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_FrameEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void              EZ_ComputeWidgetFrameSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN int               EZ_OnFrameDragHandle MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN void              EZ_HighlightFrameDragHandle MY_ANSIARGS((EZ_Widget *widget, int onoff));
EXTERN int               EZ_DetachFrame MY_ANSIARGS((EZ_Widget *widget, int xoff, int yoff));
EXTERN int               EZ_DockFrame MY_ANSIARGS((EZ_Widget *widget, int force));
EXTERN int               EZ_XYIsAtCorner MY_ANSIARGS((int x,int y,int w,int h));
EXTERN int               EZ_FrameIsDetached MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_ResetWidgetShape MY_ANSIARGS((EZ_Widget *widget, int sib));
#endif

/*
 * From EZ_WidgetLabel.c
 */
#ifndef _EZ_WIDGET_LABEL_C_
EXTERN EZ_Widget  *EZ_CreateLabel MY_ANSIARGS((EZ_Widget *parent, char *label));
EXTERN void             EZ_DrawWidgetLabel MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetLabelSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_FreeWidgetLabelData MY_ANSIARGS((EZ_Widget *widget));
#endif
/*
 * From EZ_WidgetButtonN.c
 */
#ifndef _EZ_WIDGET_BUTTONN_C_
EXTERN EZ_Widget  *EZ_CreateButton MY_ANSIARGS((EZ_Widget *parent, char *label, int undline));
EXTERN void             EZ_DrawWidgetButton MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetButtonSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_FreeWidgetNButtonData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_NButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void             EZ_SetButtonBorderType MY_ANSIARGS((EZ_Widget *widget, int flag));
EXTERN int              EZ_OppsiteBorder MY_ANSIARGS((int bd, int flag));
#endif

/*
 * From EZ_WidgetButtonC.c
 */
#ifndef _EZ_WIDGET_BUTTONC_C_
EXTERN EZ_Widget        *EZ_CreateCheckButton MY_ANSIARGS((EZ_Widget *parent,
							   char *label,
							   int underline,
							   int onvalue, int offvalue, int onff ));
EXTERN void             EZ_DrawWidgetCheckButton MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetCButtonSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_FreeWidgetCButtonData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_CButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void             EZ_SetCheckButtonState MY_ANSIARGS((EZ_Widget *widget, int onoff));
EXTERN int              EZ_GetCheckButtonState MY_ANSIARGS((EZ_Widget *widget, int *value));
#endif
/*
 * From EZ_WidgetButtonR.c
 */
#ifndef _EZ_WIDGET_BUTTONR_C_
EXTERN EZ_Widget         *EZ_CreateRadioButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int ul, int gp, int val));
EXTERN void              EZ_DrawWidgetRadioButton  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_ComputeWidgetRButtonSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void              EZ_FreeWidgetRButtonData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_RButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void              EZ_RemoveRBtnFromItsGroup MY_ANSIARGS(( EZ_Widget *rbutton));
EXTERN void              EZ_SetRadioButtonGroupVariableValue MY_ANSIARGS((EZ_Widget *widget, int value));
EXTERN int               EZ_GetRadioButtonGroupVariableValue MY_ANSIARGS((EZ_Widget *widget));
EXTERN int               EZ_GetRadioButtonGroupID MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_FixRadioButtonList MY_ANSIARGS((EZ_Widget *w, int id));
#endif

/*
 * From EZ_WidgetButtonM.c
 */
#ifndef _EZ_WIDGET_BUTTONM_C_
EXTERN EZ_Widget  *EZ_CreateMenuButton MY_ANSIARGS((EZ_Widget *parent, char *label, int undline));
EXTERN void              EZ_DrawWidgetMenuButton MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_ComputeWidgetMButtonSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void              EZ_FreeWidgetMButtonData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_MButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EXTERN void              EZ_SetMenuButtonMenu MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu));
EXTERN EZ_Widget         *EZ_GetMenuButtonMenu MY_ANSIARGS((EZ_Widget *widget));
EXTERN int               EZ_LookForPopupShortCuts MY_ANSIARGS((EZ_Widget *widget, int modfiers, 
							char *str, int *x, int *y, 
							EZ_Widget **item));
EXTERN int               EZ_MatchCharIgnoreCase MY_ANSIARGS((char c1, char c2));
EXTERN EZ_Widget         *EZ_FirstMenuItem MY_ANSIARGS((EZ_Widget *widget));
EXTERN int               EZ_MenuItemReachable MY_ANSIARGS((EZ_Widget *menu, EZ_Widget *item));
EXTERN EZ_Widget         *EZ_FirstMenuItem MY_ANSIARGS((EZ_Widget *widget));
#endif
/*
 * From EZ_WidgetSlider.c
 */
#ifndef _EZ_WIDGET_SLIDER_C_
EXTERN EZ_Widget   *EZ_CreateSlider  MY_ANSIARGS((EZ_Widget *parent, char *lab, 
					   float mnv, float mxv, float initv, int type));
EXTERN void         EZ_SliderEventHandle  MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void         EZ_SetSliderValue  MY_ANSIARGS((EZ_Widget *widget, float value));
EXTERN float        EZ_GetSliderValue  MY_ANSIARGS((EZ_Widget *widget));

EXTERN void         EZ_ComputeWidgetHSliderSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void         EZ_ComputeWidgetVSliderSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void         EZ_DrawWidgetHorizontalSlider  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void         EZ_DrawWidgetVerticalSlider MY_ANSIARGS((EZ_Widget *widget));
EXTERN void         EZ_FreeWidgetSliderData  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void         EZ_SetSliderRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));
EXTERN void         EZ_SetPrintFFormat MY_ANSIARGS((float from, float to, float resolution,
                                                    float olength, char *ret));
EXTERN float        EZ_FixSliderValue MY_ANSIARGS((float v, float res));
#endif
/*
 * From EZ_WidgetMenuItems.c
 */
#ifndef _EZ_WIDGET_MENU_ITEMS_C_
EXTERN EZ_Widget        *EZ_CreateMenuSeparator MY_ANSIARGS((EZ_Widget *parent));
EXTERN void             EZ_DrawWidgetMenuSeparator MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetMenuSeparatorSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));

EXTERN EZ_Widget        *EZ_CreateMenuSubMenu MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl));
EXTERN void             EZ_DrawWidgetMenuSubMenu MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetMenuSubMenuSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_FreeWidgetMenuSubMenuData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_MenuSubMenuEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EXTERN EZ_Widget        *EZ_CreateMenuNormalButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl, int ret));
EXTERN void             EZ_MenuNormalButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EXTERN EZ_Widget        *EZ_CreateMenuCheckButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl, 
							int onv, int offv, int onoff, int ret));
							
EXTERN void             EZ_MenuCheckButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EXTERN EZ_Widget        *EZ_CreateMenuRadioButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl,
							int gid, int val, int ret));
EXTERN void             EZ_MenuRadioButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EXTERN void             EZ_SetSubMenuMenu MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu));
EXTERN EZ_Widget        *EZ_GetMenuItemCompaion MY_ANSIARGS((EZ_Widget *widget));

#endif

/*
 * From EZ_WidgetGeom.c
 */
#ifndef _EZ_WIDGET_GEOM_C_
EXTERN void                 EZ_ComputeWidgetWindowSize MY_ANSIARGS((EZ_Widget *widget,
							     int *w, int *h,
							     EZ_Widget *dirtyAns));
EXTERN void                 EZ_SetChildrenPositionAndSize MY_ANSIARGS((EZ_Widget *widget, int w, int h));
EXTERN int EZ_CountWidthScalableChildren MY_ANSIARGS((EZ_Widget *widget));
EXTERN int EZ_CountHeightScalableChildren MY_ANSIARGS((EZ_Widget *widget));
EXTERN int EZ_GetNumberOfActiveChildren MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_FixCheckMarkOffsets MY_ANSIARGS((EZ_Widget *widget, int h1, int h2));
#endif

/*
 * From EZ_WidgetDisp.c
 */
#ifndef _EZ_WIDGET_DISP_C_
EXTERN void                 EZ_DisplayWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                 EZ_DisplayWidgetUnderPointer MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN void                 EZ_ReDisplayWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                 EZ_DisplayWidgetWithChoice MY_ANSIARGS((EZ_Widget *widget, int chi, int sib));
EXTERN void                 EZ_DrawWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                 EZ_DrawWidgetAndDescendants MY_ANSIARGS((EZ_Widget *widget, int l));
EXTERN void                 EZ_CreateWindowForWidget MY_ANSIARGS((EZ_Widget *widget, int flag));
EXTERN void                 EZ_SetWindowShape MY_ANSIARGS((EZ_Widget *widget, int rem_flag));
EXTERN void                 EZ_ForgetInitialGeometry MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                 EZ_MakeWidgetWindows MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                 EZ_FreezeDisplay MY_ANSIARGS((void));
EXTERN void                 EZ_ThawDisplay MY_ANSIARGS((void));
#endif

/*
 * From EZ_Focus.c
 */
#ifndef _EZ_FOCUS_C_
EXTERN int  EZ_FocusNextWidget MY_ANSIARGS(( EZ_Widget *widget, int dir));
EXTERN void EZ_SetFocusTo MY_ANSIARGS(( EZ_Widget *widget));
EXTERN void EZ_ResetFocus MY_ANSIARGS((EZ_Widget *widget, int flag));
EXTERN EZ_Widget *EZ_FindFocusingWidget MY_ANSIARGS(( EZ_Widget *widget));
EXTERN EZ_Widget *EZ_GetFocus MY_ANSIARGS((void));
EXTERN int  EZ_WidgetIsFocusable MY_ANSIARGS(( EZ_Widget *widget));
#endif
/*
 * From EZ_WidgetHash.c
 */
#ifndef _EZ_WIDGET_HASH_C_
EXTERN EZ_Bitmap         *EZ_CreateShapeMaskFromFigure MY_ANSIARGS((EZ_Item *fitem));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromXPixmap MY_ANSIARGS((Pixmap pix, Pixmap shape,
                                                            int x, int y, int w, int h, int flag));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromXpmFile MY_ANSIARGS((char *file));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromXpmData MY_ANSIARGS((char **data));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromRawRGB MY_ANSIARGS((unsigned char *data, 
                                                                      int width, int height));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromStaticRawRGB MY_ANSIARGS((unsigned char *data, 
                                                                            int width, int height));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromXBitmapFile MY_ANSIARGS((char *file));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromXBitmapData MY_ANSIARGS((char *file, unsigned int w, unsigned int h));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromImageFile MY_ANSIARGS((char *file));
EXTERN void              EZ_FreeLabelPixmap MY_ANSIARGS((EZ_Bitmap *bitmap));
EXTERN void              EZ_FreeXPixmap MY_ANSIARGS((Pixmap pixmap));
EXTERN void              EZ_InitializeWidgetHT MY_ANSIARGS((void));
EXTERN void              EZ_RemoveWidgetFromMappedHT  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_RemoveWidgetFromAllHT  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_InsertWidgetToMappedHT  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_InsertWidgetToAllHT  MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget         *EZ_LookupWidgetFromMappedHT  MY_ANSIARGS((Window win));
EXTERN EZ_Widget         *EZ_LookupWidgetFromAllHT  MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget         *EZ_LookupWidgetFromAllHTUL  MY_ANSIARGS((unsigned long addr));

EXTERN EZ_Widget         *EZ_WindowIsWidgetWindow  MY_ANSIARGS((Window window));
EXTERN EZ_Widget         *EZ_WidgetExist  MY_ANSIARGS((EZ_Widget *widget));

EXTERN int               EZ_HashString  MY_ANSIARGS((char *str, int size));
EXTERN EZ_Bitmap         *EZ_GetBitmapFromFile  MY_ANSIARGS((char *file, int ref));

EXTERN EZ_Bitmap         *EZ_GetBitmapFromData  MY_ANSIARGS((char *data, unsigned int w, unsigned int h));
EXTERN EZ_Bitmap         *EZ_GetPixmapFromData  MY_ANSIARGS((char  **data));
EXTERN EZ_Bitmap         *EZ_GetPixmapFromRawRGB  MY_ANSIARGS((unsigned char  *data, int width, int height));
EXTERN EZ_Bitmap         *EZ_GetPixmapFromStaticRawRGB  MY_ANSIARGS((unsigned char  *data,
                                                                     int width, int height));
EXTERN EZ_Bitmap         *EZ_GetPixmapFromPixmap  MY_ANSIARGS((Pixmap pix, int x, int y, int w, int h));
EXTERN EZ_Bitmap         *EZ_GetPixmapFromPermPixmap  MY_ANSIARGS((Pixmap pix, int x, int y, int w, int h));
EXTERN EZ_Bitmap         *EZ_GetImageFromFile  MY_ANSIARGS((char *file, int ref));
EXTERN EZ_Bitmap         *EZ_GetAnyPixmapFromFile  MY_ANSIARGS((char *file));
EXTERN char              *EZ_GetBitmapFileName  MY_ANSIARGS((EZ_Bitmap *bitmap));
EXTERN void              EZ_FreeBitmap  MY_ANSIARGS((EZ_Bitmap *bitmap));

EXTERN void              EZ_DestroyAllWidgets  MY_ANSIARGS((void));

EXTERN int               EZ_GetLabelPixmapInfo MY_ANSIARGS((EZ_Bitmap *bitmap,Pixmap *p_ret, 
							    Pixmap *s_ret, int *w, int *h));
EXTERN void              EZ_SetIconPath MY_ANSIARGS((char *path));
EXTERN char              *EZ_GetIconPath MY_ANSIARGS((void));
EXTERN EZ_Bitmap         *EZ_CreateBackgroundPixmapsFromImageFile MY_ANSIARGS((char *file));
EXTERN EZ_Bitmap         *EZ_CreateBackgroundPixmapsFromXPixmap MY_ANSIARGS((Pixmap pix,int x,int y,
                                                                             int w,int h));
EXTERN EZ_Bitmap         *EZ_CreateBackgroundPixmapsFromXPixmapX MY_ANSIARGS((Pixmap pix,int x,int y,
                                                                             int w,int h,
                                                                             int p1, int r, int g, int b, 
                                                                             int p2));
EXTERN EZ_Bitmap         *EZ_CreateBackgroundPixmapsFromRawRGB MY_ANSIARGS((unsigned char *data,
                                                                            int w, int h));
EXTERN EZ_Bitmap         *EZ_CheckBGPixmap MY_ANSIARGS((EZ_Bitmap *vv));
EXTERN EZ_Bitmap         *EZ_ScaleLabelPixmap MY_ANSIARGS((EZ_Bitmap *pix, int w, int h, 
                                                           int darkness, int smooth));
EXTERN unsigned char     *EZ_ScaleRGB MY_ANSIARGS((unsigned char *rgb, int sw, int sh,  int tw, int th,
                                                   int darkness, int smooth));
EXTERN int               EZ_SwapLabelPixmaps  MY_ANSIARGS((EZ_Bitmap *b1, EZ_Bitmap *b2));
EXTERN int               EZ_CopyLabelPixmaps  MY_ANSIARGS((EZ_Bitmap *dest, EZ_Bitmap *src));
EXTERN void              EZ_TraverseAllWidgets  MY_ANSIARGS((EZ_CallBack cb, void *data));
EXTERN unsigned char     *EZ_ScaleRGBX  MY_ANSIARGS((unsigned char *rgb, int sw, int sh,
                                              int w, int h, int sr, int sg, int sb,
                                              int r, int g, int b, int brightness, int smooth));
EXTERN unsigned char     *EZ_XPixmap2RGB MY_ANSIARGS((Pixmap pixmap, int x, int y, int w, int h));
#endif
/*
 * From EZ_WidgetPopup.c
 */
#ifndef _EZ_WIDGET_POPUP_C_
EXTERN EZ_Widget  *EZ_CreatePopupMenu MY_ANSIARGS((char *title));
EXTERN EZ_Widget  *EZ_CreateMenu MY_ANSIARGS((char *title));
EXTERN EZ_Widget  *EZ_CreatePopupMenuInternal MY_ANSIARGS((char *title));
EXTERN EZ_Widget  *EZ_GetPopupMenuTitleWidget MY_ANSIARGS((EZ_Widget *widget));

EXTERN void              EZ_ComputeWidgetPopupSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN int               EZ_DoPopup MY_ANSIARGS((EZ_Widget *widget, int where));
EXTERN int               EZ_DoPopupAt MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN void              EZ_DisplayPopup MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN int               EZ_DoPopupX MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu, int where));
EXTERN int               EZ_DoPopupAtX MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu, int x, int y));
EXTERN void              EZ_DisplayPopupSubMenuOf MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_HideCurrentPopupMenu MY_ANSIARGS((void));
EXTERN void              EZ_HidePopupMenu MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_HideSubMenuOf MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_HideCurrentPopupMenuA MY_ANSIARGS((void));
EXTERN void              EZ_HidePopupMenuA MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_HideSubMenuOfA MY_ANSIARGS((EZ_Widget *widget));

EXTERN void              EZ_HandlePopupCallBack MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget         *EZ_GetSelectedMenuItem MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget         *EZ_FindMenuItemOnMenu MY_ANSIARGS((EZ_Widget *menu,
							     EZ_Widget *item, int dir));
EXTERN EZ_Widget         *EZ_MoveMenuSelection MY_ANSIARGS((EZ_Widget *menu, 
							    EZ_Widget *item, int dir));
EXTERN void               EZ_SetupPopup  MY_ANSIARGS((EZ_Widget *menu));
EXTERN void               EZ_PropagateBgInfo2Widget MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu));
#endif

/*
 * From EZ_WidgetInput.c
 */
#ifndef _EZ_WIDGET_INPUT_C_
EXTERN EZ_Widget        *EZ_CreateEntry MY_ANSIARGS((EZ_Widget *parent, char *init_str));
EXTERN void             EZ_DrawWidgetEntry MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetEntrySize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_FreeWidgetEntryData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_EntryEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void             EZ_SetEntryString MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN char             *EZ_GetEntryString MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_OnOffSecretEntry MY_ANSIARGS((EZ_Widget *widget, int onoff));

EXTERN void   EZ_EntryEditDeleteChar MY_ANSIARGS((EZ_Widget *widget, int dir));
EXTERN void   EZ_EntryEditBeginOfInput MY_ANSIARGS((EZ_Widget *widget));
EXTERN void   EZ_EntryEditBeginningOfInput MY_ANSIARGS((EZ_Widget *widget));
EXTERN void   EZ_EntryEditEndOfInput MY_ANSIARGS((EZ_Widget *widget));
EXTERN void   EZ_EntryEditForward MY_ANSIARGS((EZ_Widget *widget));
EXTERN void   EZ_EntryEditBackward MY_ANSIARGS((EZ_Widget *widget));
EXTERN void   EZ_EntryEditSetMarker MY_ANSIARGS((EZ_Widget *widget));
EXTERN void   EZ_EntryEditKillRegion MY_ANSIARGS((EZ_Widget *widget));
EXTERN void   EZ_EntryEditKillEndOfLine MY_ANSIARGS((EZ_Widget *widget, int flag));
EXTERN void   EZ_EntryCopySelectedText MY_ANSIARGS((EZ_Widget *widget, int start, int end));
EXTERN void   EZ_EntryPasteSelection MY_ANSIARGS((EZ_Widget *widget));
EXTERN void   EZ_EntryEditInsertString  MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void   EZ_EntryEditForwardWord MY_ANSIARGS((EZ_Widget *widget));
EXTERN void   EZ_EntryEditBackwardWord MY_ANSIARGS((EZ_Widget *widget));
EXTERN void   EZ_EntryEditGotoPosition MY_ANSIARGS((EZ_Widget *widget, int p));
EXTERN int    EZ_EntryIsModified MY_ANSIARGS((EZ_Widget *widget));
#endif

/*
 * From EZ_Widget3DCanvas.c
 */

#ifndef _EZ_WIDGET_3D_CANVAS_C_
EXTERN EZ_Widget  *EZ_Create3DCanvas MY_ANSIARGS((EZ_Widget *parent));
EXTERN void             EZ_DrawWidget3DCanvas MY_ANSIARGS(( EZ_Widget *widget));
EXTERN void             EZ_ComputeWidget3DCanvasSize MY_ANSIARGS(( EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_FreeWidget3DCanvasData MY_ANSIARGS(( EZ_Widget *widget));
EXTERN void             EZ_3DCanvasEventHandling MY_ANSIARGS(( EZ_Widget *widget, XEvent *event));

EXTERN void             EZ_Set3DCanvas MY_ANSIARGS(( EZ_Widget *widget));
EXTERN void             EZ_Configure3DCanvas MY_ANSIARGS(( EZ_Widget *widget));
EXTERN void             EZ_SelectRenderFunctions MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_Set3DCanvasEventHandle MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler, void *data));
EXTERN void             EZ_Save3DCanvas2PPMImage MY_ANSIARGS((EZ_Widget *widget, char *fname));
EXTERN void             EZ_Save3DCanvas2PSA MY_ANSIARGS((EZ_Widget *widget, int mode,
						  int dpi, float scale, char *fname));
EXTERN void             EZ_Save3DCanvas2PS MY_ANSIARGS((EZ_Widget *widget, char *fname));
EXTERN XImage           *EZ_Read3DCanvas2XImage MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_Get3DCanvasSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_GLConfig MY_ANSIARGS((void));
EXTERN void             EZ_SelectPolygonRenderFuncs MY_ANSIARGS((void));
EXTERN XImage           *EZ_ReadDrawable2XImage MY_ANSIARGS((Drawable d, int x, int y, int w, int h));
EXTERN void             EZ_PSXImage MY_ANSIARGS((XImage *image, int w, int h, FILE *fp));
EXTERN void             EZ_PPMXImage MY_ANSIARGS((XImage *image, int w, int h, FILE *fp));
EXTERN XImage           *EZ_AllocateSHMXImage MY_ANSIARGS((int w, int h, void *shmp));
#endif

/*
 * From EZ_WidgetIListBox.c
 */
#ifndef _EZ_WIDGET_ILIST_BOX_C_
EXTERN EZ_Widget        *EZ_CreateIListBox MY_ANSIARGS((EZ_Widget *parent));
EXTERN void             EZ_DrawWidgetIListBox MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetIListBoxSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_FreeWidgetIListBoxData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_IListBoxEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EXTERN void             EZ_SetIListBoxData MY_ANSIARGS((EZ_Widget *widget, char **items, int nitems));
EXTERN void             EZ_AppendIListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item));
EXTERN void             EZ_InsertIListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item, int where));
EXTERN void             EZ_DeleteIListBoxItem MY_ANSIARGS((EZ_Widget *widget, int idx));
EXTERN void             EZ_DeleteIListBoxString MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void             EZ_ClearIListBox MY_ANSIARGS((EZ_Widget *widget));
EXTERN char             *EZ_GetIListBoxSelectedItem MY_ANSIARGS((EZ_Widget *widget));
EXTERN char             *EZ_GetIListBoxSelection MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_GetIListBoxData MY_ANSIARGS((EZ_Widget *widget, listboxentry **dat, int *cont));
EXTERN void             EZ_ModifyIListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item, int idx));
EXTERN void             EZ_IListBoxSelectItem MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void             EZ_IListBoxSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx));
EXTERN void          EZ_InsertIListBoxColorItem MY_ANSIARGS((EZ_Widget *widget, char *item, 
                                                         unsigned long color, int where));
EXTERN void          EZ_ColorIListBoxItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx ,
                                                        unsigned long clr));
EXTERN void          EZ_ColorIListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item,
                                                unsigned long clr));
#endif
/*
 * From EZ_WidgetListBox.c
 */
#ifndef _EZ_WIDGET_LIST_BOX_C_
EXTERN EZ_Widget        *EZ_CreateListBox MY_ANSIARGS((EZ_Widget *parent, int hsc, int vsc));
EXTERN void             EZ_DrawWidgetListBox MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetListBoxSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));

EXTERN void             EZ_ClearListBox MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_SetListBoxItems MY_ANSIARGS((EZ_Widget *widget, char **items, int nitems));
EXTERN void             EZ_AppendListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item));
EXTERN void             EZ_InsertListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *str, int idx));
EXTERN void             EZ_DeleteListBoxItem MY_ANSIARGS((EZ_Widget *widget, int idx));
EXTERN void             EZ_DeleteListBoxString MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void             EZ_GetListBoxData MY_ANSIARGS((EZ_Widget *widget, listboxentry **dat, int *cnt));
EXTERN char             *EZ_GetListBoxSelectedItem MY_ANSIARGS((EZ_Widget *widget));
EXTERN char             *EZ_GetListBoxSelection MY_ANSIARGS((EZ_Widget *widget));
EXTERN int              EZ_GetListBoxSelectionLineNumber MY_ANSIARGS((EZ_Widget *widget));

EXTERN void             EZ_GetListBoxVState MY_ANSIARGS((EZ_Widget *widget, int *total, int *page, int *first));
EXTERN void             EZ_GetListBoxHState MY_ANSIARGS((EZ_Widget *widget, int *total, int *page, int *first));
EXTERN void             EZ_SetListBoxVState MY_ANSIARGS((EZ_Widget *widget, int begin));
EXTERN void             EZ_SetListBoxHState MY_ANSIARGS((EZ_Widget *widget, int begin));
EXTERN void             EZ_SetListBoxHScrollbar MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *scr));
EXTERN void             EZ_SetListBoxVScrollbar MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *scr));
EXTERN void             EZ_GetListBoxWidgetComponents MY_ANSIARGS((EZ_Widget *widget, EZ_Widget **ilb, 
						      EZ_Widget **hsc,  EZ_Widget **vsc));
EXTERN void             EZ_ModifyListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item, int idx));
EXTERN void             EZ_ListBoxSelectItem MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void             EZ_ListBoxSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx));
EXTERN void             EZ_GetListBoxState  MY_ANSIARGS((EZ_Widget *widget, int *xoff, int *fl)); 
EXTERN void             EZ_SetListBoxState  MY_ANSIARGS((EZ_Widget *widget, int xoff, int fl)); 
EXTERN void             EZ_SetListBoxRowBg MY_ANSIARGS((EZ_Widget *widget, int mode,
                                                        char *bg1, char *bg2));

EXTERN void          EZ_InsertListBoxColorItem MY_ANSIARGS((EZ_Widget *widget, char *item, 
                                                         unsigned long color, int where));
EXTERN void          EZ_ColorListBoxItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx ,
                                                        unsigned long clr));
EXTERN void          EZ_ColorListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item,
                                                unsigned long clr));

#endif

/*
 * From EZ_WidgetScroll.c
 */
#ifndef _EZ_WIDGET_SCROLL_C_
EXTERN EZ_Widget        *EZ_CreateScrollbar MY_ANSIARGS((EZ_Widget *widget, int type));
EXTERN void             EZ_UpdateScrollbar MY_ANSIARGS((EZ_Widget *widget, int maxs, int pags, int start));
EXTERN void             EZ_GetScrollbarState MY_ANSIARGS((EZ_Widget *widget, int *ms, int *ps, int *st));
EXTERN int              EZ_SetScrollbarDiscreteSpeed MY_ANSIARGS((EZ_Widget *widget, int speed));
EXTERN int              EZ_SetHScrollbarDiscreteSpeed MY_ANSIARGS((EZ_Widget *widget, int speed));
EXTERN int              EZ_SetVScrollbarDiscreteSpeed MY_ANSIARGS((EZ_Widget *widget, int speed));
EXTERN void             EZ_UpdateScrollbarI MY_ANSIARGS((EZ_Widget *widget, int maxs, int pags, int start));
EXTERN void             EZ_DoHScroll MY_ANSIARGS((EZ_Widget *widget, int offset, int flag));
EXTERN void             EZ_DoVScroll  MY_ANSIARGS((EZ_Widget *widget, int offset, int flag));

EXTERN void             EZ_ComputeWidgetHScrollBarSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_ComputeWidgetVScrollBarSize  MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_DrawWidgetHScrollBar  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_DrawWidgetVScrollBar  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_HScrollBarEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void             EZ_VScrollBarEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
#endif
/*
 * From EZ_WidgetIToplevel.c
 */
#ifndef _EZ_WIDGET_ITOPLEVEL_C_
EXTERN EZ_Widget       *EZ_CreateFreeLabel MY_ANSIARGS((EZ_Widget *widget, char *label));
EXTERN EZ_Widget       *EZ_CreateIcon MY_ANSIARGS((void));
EXTERN EZ_Widget       *EZ_CreateIToplevelFrame MY_ANSIARGS((EZ_Widget *widget, char *label));
EXTERN void            EZ_IToplevelEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void            EZ_ConstrainFreeLabel MY_ANSIARGS((EZ_Widget *widget, int flag));
#endif
/*
 * From EZ_WidgetBorder.c
 */
#ifndef _EZ_WIDGET_BORDER_C_
EXTERN void                EZ_FillRectBorderCoor MY_ANSIARGS((
						       int width, int height, int bdwidth,
						       int xoff, int yoff,
						       XPoint *top, XPoint *bottom,
						       XPoint *left, XPoint *right));
EXTERN void                EZ_FillDiamondBorderCoor MY_ANSIARGS((
							  int width, int height, int bdwidth,
							  int xoff, int yoff,
							  XPoint *tr, XPoint *bl,
							  XPoint *tl, XPoint *br));
EXTERN void                EZ_DrawRectBorder MY_ANSIARGS((EZ_Widget *widget, Drawable drawable));
EXTERN void                EZ_DrawRectBorderWithSize MY_ANSIARGS((
							   EZ_Widget *widget, Drawable drawable,
							   int x, int y, int w, int h, int bw,
							   int bdtype,int a, int b, int c));
EXTERN void                EZ_DrawDiamondBorderWithSize MY_ANSIARGS((
							      EZ_Widget *widget, Drawable drawable,
							      int x, int y, int w, int h, int bw,
							      int bdtype, int a, int b, int c));
EXTERN void                EZ_RenderBorder  MY_ANSIARGS((
						  Drawable  drawable,
						  int       type,
						  EZ_BorderPiece *top, 
						  EZ_BorderPiece *bottom, 
						  EZ_BorderPiece *left,
						  EZ_BorderPiece *right,
						  EZ_BorderPiece *extra,
						  EZ_BorderPiece *extraB  ));

EXTERN void                EZ_FillNBBorderCoor  MY_ANSIARGS((int width, int height, int bwidth, int padb,
							      int xoffset, int yoffset,
							     XPoint *left, XPoint *ul, XPoint *top,
							     XPoint *ur, XPoint *right  ));
EXTERN void                EZ_SetupUpBdGC MY_ANSIARGS((
						EZ_Widget *widget,
						GC        *left,  GC *uleft, GC *top, GC *uright,
						GC *right, GC *bottom, int a, int b, int c));
EXTERN void                EZ_SetupDownBdGC MY_ANSIARGS((
						  EZ_Widget *widget,
						  GC        *left,  GC *uleft, GC *top, GC *uright,
						  GC *right, GC *bottom, int a, int b, int c ));
EXTERN void                EZ_DrawFocusIndicator MY_ANSIARGS((EZ_Widget *widget, Drawable drawable, 
							      int inout));
#endif

/*
 * From EZ_WidgetFont.c
 */
#ifndef _EZ_WIDGET_FONT_C_
EXTERN XFontStruct    *EZ_GetWidgetFont MY_ANSIARGS((EZ_Widget *widget));
EXTERN XFontStruct    *EZ_GetFontFromId MY_ANSIARGS((int id));
EXTERN XFontStruct    *EZ_GetFontFromXFontId MY_ANSIARGS((Font font));
EXTERN XFontStruct    *EZ_GetFontFromName MY_ANSIARGS((char *fname));
EXTERN void           EZ_InitFontList MY_ANSIARGS((void));
EXTERN int            EZ_LoadXFont MY_ANSIARGS((char *fname));
EXTERN void           EZ_SetXFont MY_ANSIARGS((XFontStruct **ptr, int id));
EXTERN char           *EZ_GetFontNameFromId MY_ANSIARGS((int id));
EXTERN char           *EZ_GetFontName MY_ANSIARGS((XFontStruct *fptr));
EXTERN int            EZ_GetFontID MY_ANSIARGS((XFontStruct *fptr));
EXTERN void           EZ_DestroyFontList MY_ANSIARGS((void));
EXTERN void           EZ_SetDefaultLabelFont MY_ANSIARGS((char *fname));
EXTERN void           EZ_SetDefaultEntryTextFont MY_ANSIARGS((char *fname));
EXTERN void           EZ_SetDefaultMenuTitleFont MY_ANSIARGS((char *fname));
EXTERN int            EZ_FontWeightIsBold MY_ANSIARGS((char *fname));
#endif
/*
 * From EZ_WidgetEvent.c
 */
#ifndef _EZ_WIDGET_EVENT_C_
EXTERN void            EZ_WidgetDispatchEvent MY_ANSIARGS((XEvent *event));
EXTERN void            EZ_HandleWidgetWindowEvent MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void            EZ_ServiceEvents MY_ANSIARGS((void));
EXTERN void            EZ_WaitAndServiceNextEvent MY_ANSIARGS((void));
EXTERN void            EZ_EventMainLoop MY_ANSIARGS((void));
EXTERN void            EZ_InvokePrivateEventHandler MY_ANSIARGS((XEvent *event));
EXTERN void            EZ_InitializeEventHT MY_ANSIARGS((void));
EXTERN void            EZ_InsertGlobalKeyPressEvent  MY_ANSIARGS((EZ_Widget *widget,int modif, char *key));
EXTERN void            EZ_RemoveGlobalKeyEvent  MY_ANSIARGS((int modi, char *key, EZ_Widget *widget));
EXTERN void            EZ_RemoveAllKeyShortcutForWidget  MY_ANSIARGS((EZ_Widget *widget));
EXTERN int             EZ_GetGlobalKeyEvent  MY_ANSIARGS((int mod, char *key, EZ_Widget *root, 
							  EZ_Widget **widget_ret, int *x_r, int *y_r));
EXTERN void            EZ_TiePopupMenu  MY_ANSIARGS((EZ_Widget *widget, int btnNm, int where));
EXTERN void            EZ_WarpPointerToWidgetWindow  MY_ANSIARGS((EZ_Widget *widget, int where));
EXTERN void            EZ_RemoveEvent  MY_ANSIARGS((XEvent *event));
EXTERN int             EZ_CheckTimerEvents MY_ANSIARGS((void));
EXTERN void            EZ_HandleGlobalKeyEvent MY_ANSIARGS((EZ_Widget *w, int x, int y));
EXTERN void            EZ_TFEvents MY_ANSIARGS((void));
EXTERN int             EZ_FilterEvent MY_ANSIARGS((XEvent *event));
EXTERN void            EZ_WarpPointer MY_ANSIARGS((int x, int y));
#endif
/*
 * From EZ_WidgetGC.c
 */

#ifndef _EZ_WIDGET_GC_C_
EXTERN void  EZ_InitializeWidgetGC MY_ANSIARGS((void));
EXTERN void  EZ_DestroyWidgetGCsandFonts MY_ANSIARGS((void));
EXTERN GC    EZ_CreateGCFor3DCanvas MY_ANSIARGS((void));
EXTERN void  EZ_GetMovingDashedLineGC MY_ANSIARGS((GC *gcp_ret, int which));
#endif
/*
 * From EZ_WidgetFile.c
 */
#ifndef _EZ_WIDGET_FILE_C_
EXTERN int  EZ_FileReadable  MY_ANSIARGS((char *name));
#endif


/*
 * From EZ_WidgetImage.c
 */
#ifndef _EZ_WIDGET_IMAGE_C_
EXTERN int   EZ_CreateXPixmapFromImageFile MY_ANSIARGS((char *file, unsigned int *w,
							unsigned int *h, Pixmap *pix));
EXTERN int   EZ_CreateXPixmapFromRawRGB MY_ANSIARGS((unsigned char *data,
                                                     int width, int height, Pixmap *pixmap_return));
EXTERN void  EZ_RawRGBData2Pixmap MY_ANSIARGS((unsigned char *data, int w, int h, Pixmap *pix));
EXTERN void  EZ_RawRGBData2PixmapWork MY_ANSIARGS((unsigned char *data, int w, int h,
                                                   Pixmap *pix, EZ_XImage *im));
EXTERN void  EZ_PixelValueData2Pixmap MY_ANSIARGS((unsigned int *data, int w, int h, Pixmap *pix));
EXTERN void  EZ_SetDarkAndBrightPixmapReturns MY_ANSIARGS((Pixmap *dk, Pixmap *bt, Pixmap *ht));
EXTERN int   EZ_GetImageFileType MY_ANSIARGS((char *file));
EXTERN int   EZ_ReadImageFile MY_ANSIARGS(( char *file, int *width_return, int *height_return,
                                            unsigned char **rgb_return));
EXTERN EZ_XImage *EZ_CreateXImage MY_ANSIARGS((int w, int h, int shmflag));
EXTERN void  EZ_DestroyXImage  MY_ANSIARGS((EZ_XImage *image));
#endif
/*
 * From EZ_X11Init.c
 */
#ifndef _EZ_X11INIT_C_
EXTERN int  EZ_InitX11 MY_ANSIARGS((char *dpy, char *bg, char *fg, char *gm, 
				    int vis, int priv, int bks, int gl));
EXTERN int  EZ_XErrorHandler MY_ANSIARGS((Display *dpy, XErrorEvent *error));
EXTERN void EZ_ParseGlobalGeometry MY_ANSIARGS((char *spec));
EXTERN void EZ_ParseGeometry MY_ANSIARGS((char *spec, int *bits, int *x, int *y, int *w, int *h));
#endif
/*
 * From EZ_Text.c
 */
#ifndef _EZ_TEXT_C_
EXTERN void       EZ_ITextLineDimension MY_ANSIARGS((
					      EZ_Widget *widget,
					      ITextLine *line,
					      int       start, int length,
					      int       *w_ret, int *h_ret, int *a_ret));
EXTERN void       EZ_ITextLineDimensionWork MY_ANSIARGS((
					      XFontStruct *defaultFont,
					      int       spacing,
					      ITextLine *line,
					      int       start, int length,
					      int       *w_ret, int *h_ret, int *a_ret));
EXTERN void       EZ_ITextRenderText MY_ANSIARGS((
					   EZ_Widget    *widget,
					   ITextLine    *line,
					   int          x, int y,
					   int maxheight, int  maxAscent,
					   Drawable     drawable,
					   int lineNum, int CurX, int CurY));
EXTERN void       EZ_ITextRenderTextWork MY_ANSIARGS((
					   XFontStruct  *dfont,
					   unsigned long dfg,
					   unsigned long dbg,
					   int           spacing,
					   int           editable,
					   int           xoffset,
					   ITextLine    *line,
					   int          x, int y,
					   int maxheight, int  maxAscent,
					   Drawable     drawable,
					   int lineNum, int CurX, int CurY));

EXTERN TextProp   *EZ_GetTextPropertyFromID MY_ANSIARGS((
						  int font_id, 
						  int fg_set, int bg_set, int underline,
						  unsigned long fg, unsigned long bg,
						  EZ_Bitmap *bitmap));

EXTERN TextProp   *EZ_CopyTextPropIgnoreSpecial MY_ANSIARGS((TextProp *tprop));

EXTERN void       EZ_InitializeTxetPropertyHashTable MY_ANSIARGS((void));
EXTERN void       EZ_TPHTCollectGarbage MY_ANSIARGS((void));
EXTERN void       EZ_ClearTextPropHT MY_ANSIARGS((void));
EXTERN void       EZ_CountNewLines MY_ANSIARGS((
					 char *str,
					 int  *len_ret, int *nNewlines));
EXTERN void       EZ_GrowITextLine  MY_ANSIARGS((
					  ITextLine **tline,
					  int       increment));
EXTERN void       EZ_FreeTextLine  MY_ANSIARGS((ITextLine *tline));
					  
EXTERN void       EZ_FreeTextProperty MY_ANSIARGS((TextProp *tprop));
EXTERN void       EZ_GrowITextKillBuffer MY_ANSIARGS((ITextLine *buf, int len));

EXTERN ITextLine  *EZ_GetWrappedTextLine MY_ANSIARGS((void));
EXTERN void       EZ_BreakITextLines  MY_ANSIARGS((EZ_Widget *itxt, int flag));
#endif
/*
 * From EZ_WidgetIText.c
 */
#ifndef _EZ_WIDGET_ITEXT_C_
EXTERN EZ_Widget        *EZ_CreateIText MY_ANSIARGS((EZ_Widget *widget, int editable));
EXTERN void             EZ_DrawWidgetIText MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetITextSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_FreeWidgetITextData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EXTERN int              EZ_ITextGetPosition MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextGotoPosition MY_ANSIARGS((EZ_Widget *widget, int pos));
EXTERN int              EZ_ITextSetMarker MY_ANSIARGS((EZ_Widget *widget));
EXTERN char             *EZ_ITextGetRegionString MY_ANSIARGS((EZ_Widget *widget));
EXTERN char             *EZ_ITextGetBufferString MY_ANSIARGS((EZ_Widget *widget));

EXTERN void             EZ_ITextLoadFile MY_ANSIARGS((EZ_Widget *widget,char *file));
EXTERN void             EZ_ITextLoadAnnotatedFile MY_ANSIARGS((EZ_Widget *widget, char *file));
EXTERN void             EZ_ITextClear MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextNextLine MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextPreviousLine MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextBackwardChar MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextForwardChar MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextForwardWord MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextBackwardWord MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextPreviousPage MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextNextPage MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextEndOfBuffer MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextBeginningOfBuffer MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextEndOfLine MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextBeginningOfLine MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextInsertChar MY_ANSIARGS((EZ_Widget *widget, char c));
EXTERN void             EZ_ITextDeletePreviousChar MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextDeleteNextChar MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextInsertNewLine MY_ANSIARGS((EZ_Widget *widget, int flag));
EXTERN void             EZ_ITextKillEndOfLine MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextKillRegion MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextCopyToKillBuffer MY_ANSIARGS((EZ_Widget *widget,int fx, int fy, int tx, int ty));
EXTERN void             EZ_ITextPasteSelection MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextReDisplay MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextInsertString MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void             EZ_ITextInsertAnnotatedString MY_ANSIARGS((EZ_Widget *widget, char *str));

EXTERN int              EZ_ITextGrowBuffer MY_ANSIARGS((EZ_Widget *widget, int incr));
EXTERN void             EZ_ITextInsertStringWithProperty MY_ANSIARGS((EZ_Widget *widget, char *str, 
							       EZ_TextProperty *prop));
EXTERN void             EZ_ITextSetRegionFont MY_ANSIARGS((EZ_Widget *widget, char *fontName));
EXTERN void             EZ_ITextSetRegionForeground MY_ANSIARGS((EZ_Widget *widget, char *clrName));
EXTERN void             EZ_ITextSetRegionBackground  MY_ANSIARGS((EZ_Widget *widget, char *clrName));
EXTERN void             EZ_ITextSetRegionUnderline  MY_ANSIARGS((EZ_Widget *widget, int flag));
EXTERN void             EZ_ITextSetRegionProperty  MY_ANSIARGS((EZ_Widget *widget, EZ_TextProperty *prop));

EXTERN void             EZ_ITextReplaceRegionWithBitmap MY_ANSIARGS((EZ_Widget *widget, char *file));
EXTERN int              EZ_ITextSaveText MY_ANSIARGS((EZ_Widget *widget, char *file));
EXTERN int              EZ_ITextSaveAnnotatedText MY_ANSIARGS((EZ_Widget *widget, char *file));
EXTERN void             EZ_PasteSelection MY_ANSIARGS((EZ_Widget *widget, Window win,
						Atom prop, int del,
						void (*insertF) MY_ANSIARGS((EZ_Widget *widget, char *str)) ));
EXTERN void             EZ_SetSelectionOwner MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_HandleSelectionRequest MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
#endif
/*
 * From EZ_WidgetText.c
 */
#ifndef _EZ_WIDGET_TEXT_C_
EXTERN EZ_Widget        *EZ_CreateTextWidget MY_ANSIARGS((EZ_Widget *parent, int editable, int hs,int vs));
EXTERN void             EZ_DrawWidgetText MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetTextSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));

EXTERN void             EZ_TextLoadFile MY_ANSIARGS((EZ_Widget *widget, char *fname));
EXTERN void             EZ_TextLoadAnnotatedFile MY_ANSIARGS((EZ_Widget *widget, char *fname));
EXTERN void             EZ_TextClear MY_ANSIARGS((EZ_Widget *widget));

EXTERN void             EZ_TextNextLine MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextPreviousLine MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextBackwardChar  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextForwardChar  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextPreviousPage  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextForwardWord  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextBackwardWord  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextNextPage  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextEndOfBuffer  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextBeginningOfBuffer  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextEndOfLine  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextBeginningOfLine  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextInsertChar  MY_ANSIARGS((EZ_Widget *widget, char c));
EXTERN void             EZ_TextDeletePreviousChar  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextDeleteNextChar  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextInsertNewLine  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextKillEndOfLine  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextKillRegion  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextPasteSelection  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextReDisplay  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_TextInsertString  MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void             EZ_TexInsertAnnotatedString MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void             EZ_TextInsertStringWithProperty  MY_ANSIARGS((EZ_Widget *widget, char *str, 
							       EZ_TextProperty *prop));

EXTERN void             EZ_TextSetRegionFont  MY_ANSIARGS((EZ_Widget *widget, char *fontname));
EXTERN void             EZ_TextSetRegionForeground  MY_ANSIARGS((EZ_Widget *widget, char *clr));
EXTERN void             EZ_TextSetRegionBackground  MY_ANSIARGS((EZ_Widget *widget, char *clr));
EXTERN void             EZ_TextSetRegionUnderline  MY_ANSIARGS((EZ_Widget *widget, int udl));
EXTERN void             EZ_TextSetRegionProperty  MY_ANSIARGS((EZ_Widget *widget, EZ_TextProperty *prop));

EXTERN void             EZ_TextReplaceRegionWithPixmap  MY_ANSIARGS((EZ_Widget *widget, char *file));
EXTERN int              EZ_TextSaveText  MY_ANSIARGS((EZ_Widget *widget, char *file));
EXTERN int              EZ_TextSaveAnnotatedText  MY_ANSIARGS((EZ_Widget *widget, char *file));
EXTERN int              EZ_TextSetMarker  MY_ANSIARGS((EZ_Widget *widget));
EXTERN int              EZ_TextGetPosition  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_TextGotoPosition  MY_ANSIARGS((EZ_Widget *widget, int pos));
EXTERN char             *EZ_TextGetRegionString  MY_ANSIARGS((EZ_Widget *widget));
EXTERN char             *EZ_TextGetBufferString  MY_ANSIARGS((EZ_Widget *widget));
EXTERN int              EZ_TextBufferModified MY_ANSIARGS((EZ_Widget *widget));

EXTERN void             EZ_GetTextWidgetComponents  MY_ANSIARGS((EZ_Widget *widget,
								 EZ_Widget **itxt,
								 EZ_Widget **hscr,
								 EZ_Widget **vscr ));
#endif

/*
 * From EZ_WidgetNBPage.c
 */
#ifndef _EZ_WIDGET_NBPAGE_C_
EXTERN EZ_Widget   *EZ_CreateNoteBookPage  MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl));
EXTERN void        EZ_ComputeWidgetNBPageSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void        EZ_DrawWidgetNBPage MY_ANSIARGS((EZ_Widget *widget));
EXTERN void        EZ_ActivateNoteBookPage MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget   *EZ_GetNoteBookPageTab MY_ANSIARGS((EZ_Widget *widget));
#endif
/*
 * From EZ_WidgetNB.c
 */
#ifndef _EZ_WIDGET_NB_C_
EXTERN EZ_Widget   *EZ_CreateNoteBook  MY_ANSIARGS((EZ_Widget *parent));
EXTERN int          EZ_SetNoteBookOrientation MY_ANSIARGS((EZ_Widget *widget, int value));
EXTERN int          EZ_SetNoteBookOrientationA MY_ANSIARGS((EZ_Widget *widget, int value));
#endif


/*
 * From EZ_WidgetPaneHandle.c
 */
#ifndef _EZ_WIDGET_PANE_HANDLE_C_
EXTERN EZ_Widget       *EZ_CreatePaneHandle MY_ANSIARGS((EZ_Widget *parent));
EXTERN void            EZ_DrawPaneHandle MY_ANSIARGS((EZ_Widget *widget));
EXTERN void            EZ_PaneHEventHandle MY_ANSIARGS((EZ_Widget *widget,XEvent *event));
EXTERN void            EZ_ComputePaneHandleSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
#endif
							
/*
 * From EZ_Arrow.c
 */
#ifndef _EZ_ARROW_C_
EXTERN void  EZ_DrawHLArrow MY_ANSIARGS((EZ_Widget *widget, 
				  Drawable drawable,
				  int x, int y, int size, 
				  int borderstyle,
				  int borderwidth,
				  int highlight));
EXTERN void  EZ_DrawHRArrow MY_ANSIARGS((EZ_Widget *widget, 
				  Drawable drawable,
				  int x, int y, int size, 
				  int borderstyle,
				  int borderwidth,
				  int highlight));
EXTERN void  EZ_DrawVUArrow MY_ANSIARGS((EZ_Widget *widget, 
				  Drawable drawable,
				  int x, int y, int size, 
				  int borderstyle,
				  int borderwidth,
				  int highlight));
EXTERN void  EZ_DrawVDArrow MY_ANSIARGS((EZ_Widget *widget, 
				  Drawable drawable,
				  int x, int y, int size, 
				  int borderstyle,
				  int borderwidth,
				  int highlight));
EXTERN void  EZ_DrawArrow4 MY_ANSIARGS((int type, EZ_Widget *widget, 
				  Drawable drawable,
				  int x, int y, int size, 
				  int borderstyle,
				  int borderwidth,
				  int highlight));

#endif

/*
 * From EZ_WidgetGrab.c
 */
#ifndef _EZ_WIDGET_GRAB_C_
EXTERN void            EZ_SetGrab MY_ANSIARGS((EZ_Widget *widget));
EXTERN void            EZ_ReleaseGrab MY_ANSIARGS((void));
EXTERN int             EZ_WidgetIsViewable MY_ANSIARGS((EZ_Widget *widget));
EXTERN void            EZ_RedisplayWidgetTree MY_ANSIARGS((EZ_Widget *widget));
EXTERN void            EZ_RaiseWidgetWindow MY_ANSIARGS((EZ_Widget *widget));
EXTERN void            EZ_BlinkWidget  MY_ANSIARGS((EZ_Widget *widget, int cnt));
EXTERN int             EZ_GrabPointer MY_ANSIARGS((EZ_Widget *widget, Cursor csr, int tout));
EXTERN void            EZ_UngrabPointer MY_ANSIARGS((void));
#endif
/*
 * From EZ_Xpm.c
 */
#ifndef _EZ_XPM_C_
EXTERN int EZ_CreateXPixmapFromXpmData MY_ANSIARGS((char **data,
					     unsigned int *w, unsigned int *h, 
					     Pixmap *pix, Pixmap *shape));
EXTERN int EZ_CreateXPixmapFromXpmFile MY_ANSIARGS((char *file,
					     unsigned int *w, unsigned int *h, 
					     Pixmap *pix, Pixmap *shape));
EXTERN int EZ_CreateXPixmapFromBitmapFile MY_ANSIARGS((char *file,
					     unsigned int *w, unsigned int *h, 
					     Pixmap *pix, Pixmap *shape));
EXTERN int EZ_ReadXPMToRGB MY_ANSIARGS((char *file,
                                        int *w,  int *h, 
                                        unsigned char **rgb_return));
#endif

/*
 * From EZ_WidgetArrange.c
 */
#ifndef _EZ_WIDGET_ARRANGE_C_
EXTERN void                  EZ_InsertWidgetBefore MY_ANSIARGS((EZ_Widget *a, EZ_Widget *b));
EXTERN void                  EZ_InsertWidgetAfter MY_ANSIARGS((EZ_Widget *a, EZ_Widget *b));
EXTERN void                  EZ_SwapWidgets MY_ANSIARGS((EZ_Widget *a, EZ_Widget *b));
EXTERN void                  EZ_DestroyWidgetWindows MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                  EZ_MakeWidgetWindowExist MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                  EZ_ReparentWidget  MY_ANSIARGS((EZ_Widget *c, EZ_Widget *p));
EXTERN void                  EZ_ReparentWidgetXY  MY_ANSIARGS((EZ_Widget *c, EZ_Widget *p, int x, int y));
#endif

/*
 * From EZ_WidgetNWLabel.c
 */
#ifndef _EZ_WIDGET_NWLABEL_C_
EXTERN EZ_Widget        *EZ_CreateNWLabel MY_ANSIARGS((EZ_Widget *parent, char *label));
EXTERN EZ_Widget        *EZ_CreateNWFrame MY_ANSIARGS((EZ_Widget *parent));

EXTERN void              EZ_DisplayNWLabel MY_ANSIARGS((EZ_Widget *widget,
							Pixmap pixmap,
							unsigned long fg, unsigned long bg,
							int disabled, int highlight,
							int xoff, int yoff, int laboff, 
							int disp_sib));
#endif

/*
 * From EZ_Timer.c
 */
#ifndef _EZ_TIMER_C_
EXTERN void        EZ_InitializeTimers MY_ANSIARGS((void));
EXTERN EZ_Timer    *EZ_CreateTimer MY_ANSIARGS((long sec, long usec,int repeat,
					 EZ_CallBack callback, void *pdata, int idata));
EXTERN EZ_Timer    *EZ_ResetTimer MY_ANSIARGS((EZ_Timer *timer, long sec, long usec,int repeat,
					EZ_CallBack callback, void *pdata, int idata));

EXTERN int         EZ_CheckRealTimers MY_ANSIARGS((long sec, long usec));
EXTERN void        EZ_GetTimerState MY_ANSIARGS((EZ_Timer *timer, long *sec, long *usec,int *repeat));
EXTERN void        EZ_CancelTimer MY_ANSIARGS((EZ_Timer *timer));
EXTERN void        EZ_DestroyAllTimers  MY_ANSIARGS((void));
EXTERN void        EZ_RestartTimer  MY_ANSIARGS((EZ_Timer *timer));
EXTERN void        EZ_SetTimerPtrData  MY_ANSIARGS((EZ_Timer *timer, void *ptr));
EXTERN void        EZ_SetTimerIntData  MY_ANSIARGS((EZ_Timer *timer,int data));
EXTERN void        EZ_SetTimerClientData  MY_ANSIARGS((EZ_Timer *timer, void *ptr, int idata));      
EXTERN void        EZ_SetTimerCallBack  MY_ANSIARGS((EZ_Timer *timer, EZ_CallBack callback, void *data));
EXTERN void        *EZ_GetTimerPtrData      MY_ANSIARGS((EZ_Timer *timer));
EXTERN int         EZ_GetTimerIntData MY_ANSIARGS((EZ_Timer *timer));
EXTERN void        EZ_GetTimerClientData  MY_ANSIARGS((EZ_Timer *timer, void **ptr, int *idata));      
#endif


/*
 * From EZ_DrawSpecial.c
 */
#ifndef _EZ_DRAW_SPECIAL_C_
EXTERN void  EZ_DrawSpecial MY_ANSIARGS(( EZ_Widget     *widget,
					  Drawable      drawable,
					  GC            tgc,
					  int           type,
					  int           x, int y,
					  int           w, int h));
#endif

/*
 * From EZ_WidgetSpecial.c
 */
#ifndef _EZ_WIDGET_ARROW_BUTTON_C_
EXTERN EZ_Widget         *EZ_CreateArrowButton MY_ANSIARGS((EZ_Widget *parent, int type));
EXTERN void              EZ_DrawArrowButton MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_FreeArrowButtonData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_ArrowButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void              EZ_ComputeArrowButtonSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void              EZ_InitArrowButtonHandle MY_ANSIARGS((void));
EXTERN int               EZ_resetArrowType MY_ANSIARGS((int type));
#endif

/*
 * From EZ_Comm.c
 */
#ifndef _EZ_COMM_C_
EXTERN void        *EZ_OpenEZWGLRoster MY_ANSIARGS((int lock));
EXTERN void         EZ_CloseEZWGLRoster MY_ANSIARGS((EZ_ApplRoster *roster));
EXTERN void         EZ_RegisterApplication MY_ANSIARGS((void));
EXTERN void         EZ_UpdateRoster  MY_ANSIARGS((EZ_ApplRoster *roster));
EXTERN int          EZ_WindowExist  MY_ANSIARGS((Window window));
EXTERN int          EZ_SendEmbedingMessage  MY_ANSIARGS((int type,
						  Window fmdummy, Window fmwin,
						  unsigned long fmwidget,
						  Window todummy, Window towin,
						  unsigned long towidget,
						  unsigned long id,
						  int x, int y, int w, int h, 
						  int aux1, int aux2));
						  
EXTERN void         EZ_HandlePropertyNotify MY_ANSIARGS((XEvent *event));
EXTERN void         EZ_GenerateEmbedingRequest MY_ANSIARGS((EZ_Widget *widget,
						     int x, int y, int w, int h));
EXTERN EZ_Widget    *EZ_SearchForEmbder MY_ANSIARGS((Window hint, int x, int y, int w, int h,
					      int *xx, int *yy));
EXTERN EZ_Widget    *EZ_SetupAnEmbederIn MY_ANSIARGS((EZ_Widget *widget,
					       Window ecwin, Window  ewin,
					       unsigned long ewidget,
					       unsigned long timeS,
					       int x, int y, int w, int h ));
EXTERN void         EZ_GrowAllocatedBuffer MY_ANSIARGS((char **buf, int *clength, 
						 int newlen, int unit));
EXTERN void         EZ_HandleEmbedingMessages MY_ANSIARGS((char *data, 
						    unsigned long length));

EXTERN void         EZ_DecodeEmbedingMessage MY_ANSIARGS((char *data, 
						   EZ_EmbedingMessage *msg));
EXTERN void         EZ_EncodeEmbedingMessage MY_ANSIARGS((char *data, 
						   EZ_EmbedingMessage *msg));

EXTERN unsigned long EZ_VerifyExistence  MY_ANSIARGS((Window window));
EXTERN unsigned long EZ_WindowIsDnDTarget  MY_ANSIARGS((Window window));
EXTERN unsigned long EZ_WindowIsDnDSrc  MY_ANSIARGS((Window window));

#endif

/*
 * From EZ_WidgetEmbeder.c
 */
#ifndef _EZ_WIDGET_EMBEDER_C_
EXTERN int             EZ_MakeSnapShot MY_ANSIARGS((EZ_Widget *widget,int type, int x, int y, int w, int h));
EXTERN void            EZ_MoveSnapShotOrOutline MY_ANSIARGS((int which, int x, int y, int w, int h, int flag));
EXTERN void            EZ_SetExecutorCommand MY_ANSIARGS((EZ_Widget *widget, char *cmd, char *args));

EXTERN EZ_Widget       *EZ_CreateEmbeder MY_ANSIARGS((EZ_Widget *parent));
EXTERN EZ_Widget       *EZ_CreateExecutor MY_ANSIARGS((EZ_Widget *parent));
EXTERN void            EZ_EmbederEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void            EZ_DrawEmbeder MY_ANSIARGS((EZ_Widget *widget));
EXTERN void            EZ_ComputeEmbederSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void            EZ_FreeEmbederData MY_ANSIARGS((EZ_Widget *widget));

EXTERN void            EZ_ComputeExecutorSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void            EZ_DrawExecutor MY_ANSIARGS((EZ_Widget *widget));
EXTERN void            EZ_FreeExecutorData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void            EZ_IntersectRectangles MY_ANSIARGS((int px, int py, int pw, int ph,
						    int x, int y, int w, int h,
						    int *xr, int *yr, int *wr, int *hr));
#endif

/*
 * From EZ_Indicator.c
 */
#ifndef _EZ_INDICATOR_C_
EXTERN void             EZ_DrawIndicator MY_ANSIARGS((
					       EZ_Widget *wptr,
					       Pixmap pixmap, 
					       int onoff,
					       int type, 
					       int bwidth,
					       int adjust,
					       unsigned long color));

#endif

/*
 * From EZ_WidgetOptInput.c
 */
#ifndef _EZ_WIDGET_OPT_INPUT_C_
EXTERN EZ_Widget        *EZ_CreateOptionalEntry MY_ANSIARGS((EZ_Widget *parent, char *str, int hist, int editable));
EXTERN void              EZ_SetOptionalEntryString MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN char             *EZ_GetOptionalEntryString MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_GetOptionalEntryWidgetComponents MY_ANSIARGS((EZ_Widget *widget, 
								    EZ_Widget **entry, EZ_Widget **listbox, 
								    EZ_Widget **btn  ));
EXTERN void  EZ_OptionalEntryClearOptions     MY_ANSIARGS((EZ_Widget *widget));
EXTERN void  EZ_SetOptionalEntryOptions       MY_ANSIARGS((EZ_Widget *widget, char **items, int count));
EXTERN void  EZ_OptionalEntryAppendOption     MY_ANSIARGS((EZ_Widget *widget, char *item));
EXTERN void  EZ_OptionalEntryInsertOption     MY_ANSIARGS((EZ_Widget *widget, char *str, int idx));
EXTERN void  EZ_OptionalEntryDeleteOption     MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void  EZ_SetOptionalEntryStyle         MY_ANSIARGS((EZ_Widget *widget, int style));
EXTERN int   EZ_GetOptionalEntryStyle         MY_ANSIARGS((EZ_Widget *widget));
#endif
/*
 * From EZ_WidgetFileSelector.c
 */
#ifndef _EZ_WIDGET_FILE_SELECT_C_
EXTERN EZ_Widget        *EZ_CreateFileSelector  MY_ANSIARGS((EZ_Widget *parent, char *pattern));
EXTERN void             EZ_GetFileSelectorWidgetComponents  MY_ANSIARGS((EZ_Widget *widget,
								   EZ_Widget **pe, EZ_Widget **se,
								   EZ_Widget **dl, EZ_Widget **fl,
								   EZ_Widget **ok, EZ_Widget **ft,
								   EZ_Widget **cancel));
EXTERN void             EZ_SetFileSelectorInitialPattern MY_ANSIARGS((EZ_Widget *widget, char *pattern));
EXTERN char             *EZ_GetFileSelectorSelection MY_ANSIARGS((EZ_Widget *widget));
EXTERN int              EZ_ProcessFileSelectorPattern MY_ANSIARGS((char **dirName, char *pattern));
EXTERN int              EZ_CompareStr MY_ANSIARGS((CONST void *ptr1, CONST void *ptr2));
EXTERN int              EZ_CountSetFilePatterns MY_ANSIARGS((char *pat, char **patterns));
EXTERN int              EZ_IsGlobPattern MY_ANSIARGS((char *str));
#endif

/*
 * From EZ_WidgetITree.c
 */
#ifndef _EZ_WIDGET_ITREE_C_
EXTERN EZ_Widget  *EZ_CreateITree  MY_ANSIARGS((EZ_Widget *parent));
EXTERN void       EZ_DrawWidgetITree MY_ANSIARGS((EZ_Widget *widget));
EXTERN void       EZ_ComputeWidgetITreeSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void       EZ_FreeWidgetITreeData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void       EZ_ITreeEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EXTERN EZ_TreeNode *EZ_FindPointerOnTreeNode MY_ANSIARGS((int x, int y, EZ_TreeNode *root));
EXTERN EZ_TreeNode *EZ_FindTreeAdjBranch MY_ANSIARGS((EZ_TreeNode *root, EZ_TreeNode *node, int dir));
EXTERN void        EZ_SetITreeWTree MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *tree));
EXTERN void        EZ_FlashItemOutline MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, 
					     int Rx, int Ry, int Rw, int Rh));
EXTERN void        EZ_DnDOutlineItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, 
					     int Rx, int Ry, int Rw, int Rh));
EXTERN void        EZ_ITreeSelectNode MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *theNode, int *location));
EXTERN void        EZ_ITreeSelectNodeUsingItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *theItem, int *location));

EXTERN EZ_TreeNode **EZ_ITreeGetHighlightedNodesA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
EXTERN EZ_Item     **EZ_ITreeGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
EXTERN int           EZ_ITreeGetHighlightedNodes  MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode **ary, int siz));
EXTERN int           EZ_ITreeGetHighlightedItems  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **ary, int siz));
#endif
/*
 * From EZ_WidgetTree.c
 */
#ifndef _EZ_WIDGET_TREE_C_
EXTERN EZ_Widget  *EZ_CreateListTree MY_ANSIARGS((EZ_Widget *parent, int hsc, int vsc));
EXTERN void        EZ_DrawWidgetTree  MY_ANSIARGS((EZ_Widget *widget));

EXTERN void           EZ_GetListTreeWidgetComponents MY_ANSIARGS((EZ_Widget *widget,
							   EZ_Widget **itr, EZ_Widget **hsc,
							   EZ_Widget **vsc));
EXTERN void           EZ_SetListTreeWidgetTree MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *tree));
EXTERN EZ_TreeNode    *EZ_GetListTreeWidgetTree MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_TreeNode    *EZ_GetListTreeWidgetSelection MY_ANSIARGS((EZ_Widget *widget));
EXTERN void           EZ_ListTreeWidgetSelectNode MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *theNode, int *location));
EXTERN void           EZ_ListTreeWidgetSelectNodeUsingItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *theItem, int *location));

EXTERN void        EZ_GetListTreeWidgetXYOffset  MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));
EXTERN void        EZ_SetListTreeWidgetXYOffset  MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN EZ_TreeNode *EZ_ListTreeFindNodeAtXY MY_ANSIARGS((EZ_Widget *tree, int x, int y));

EXTERN EZ_TreeNode **EZ_ListTreeGetHighlightedNodesA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
EXTERN EZ_Item     **EZ_ListTreeGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
EXTERN int           EZ_ListTreeGetHighlightedNodes  MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode **ary, int siz));
EXTERN int           EZ_ListTreeGetHighlightedItems  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **ary, int siz));
EXTERN void          EZ_SetTreeWidgetRowBg MY_ANSIARGS((EZ_Widget *widget, int mode, char *bg1, char *bg2));
#endif

/*
 * From EZ_WidgetIFListBox.c
 */
#ifndef _EZ_WIDGET_IFLIST_BOX_C_
EXTERN EZ_Widget        *EZ_CreateIFListBox MY_ANSIARGS((EZ_Widget *parent, int ncols, int seltype));
EXTERN void             EZ_DrawWidgetIFListBox MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetIFListBoxSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_FreeWidgetIFListBoxData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_IFListBoxEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EXTERN void             EZ_SetIFListBoxData MY_ANSIARGS((EZ_Widget *widget, EZ_Item **items, int row, int col));
EXTERN void             EZ_InsertIFListBoxRow  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **items, int nitems, int where));
EXTERN void             EZ_DeleteIFListBoxRow  MY_ANSIARGS((EZ_Widget *widget, int idx));
EXTERN void             EZ_DeleteIFListBoxSomeRows MY_ANSIARGS((EZ_Widget *widget, int *idces, int cnt));
EXTERN void             EZ_ReplaceIFListBoxItem  MY_ANSIARGS((EZ_Widget *widget, EZ_Item *old, EZ_Item *newItem));
EXTERN void             EZ_ReplaceIFListBoxItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int row, int col, EZ_Item *newItem));
EXTERN void             EZ_ClearIFListBox  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_SetIFListBoxHeader  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **elts, int nelts));

EXTERN EZ_Item          **EZ_GetIFListBoxSelection  MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Item          *EZ_GetIFListBoxItemUsingIdx  MY_ANSIARGS((EZ_Widget *widget, int row, int col));

EXTERN int              EZ_FindPointerOnRowIFL  MY_ANSIARGS((EZ_Item ***array, int nrow, int ncol, int x, int y,
						      int Rx, int Ry, int RW, int RH));
EXTERN void             EZ_FindPointerOnRowAndColIFL  MY_ANSIARGS((EZ_Item ***array, int nrow, int ncol, int x, int y,
							    int *rowR, int *colR, int Rx, int Ry, int RW, int RH));

EXTERN void             EZ_FlashRowOutline MY_ANSIARGS((EZ_Widget *widget, EZ_Item **row, int nelts));
EXTERN void             EZ_DnDOutlineRow MY_ANSIARGS((EZ_Widget *widget, EZ_Item **row, int nelts,
					       int Rx, int Ry, int Rw, int Rh));
EXTERN void             EZ_IFListBoxSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int Srow, int Scol, int *location));
EXTERN void             EZ_IFListBoxSelectItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int *location));
EXTERN void             EZ_IFListBoxAllocNewStorage MY_ANSIARGS((EZ_Widget *widget,int rows, int cols));

EXTERN int              *EZ_IFListBoxGetHighlightedRowsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
EXTERN int              EZ_IFListBoxGetHighlightedRows MY_ANSIARGS((EZ_Widget *widget, int *ary, int siz));
EXTERN EZ_Item          **EZ_IFListBoxGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
EXTERN int              EZ_IFListBoxGetHighlightedItems MY_ANSIARGS((EZ_Widget *widget, EZ_Item **arg,int siz));
EXTERN int              EZ_IFListBoxGetHithlightedItemsAndIndices MY_ANSIARGS((EZ_Widget *wgt, EZ_Item ***itm_ret,
									       int **rowIdx_ret, int **colIdx_ret));
#endif
/*
 * From EZ_WidgetFListBox.c
 */
#ifndef _EZ_WIDGET_FLIST_BOX_C_
EXTERN EZ_Widget  *EZ_CreateFancyListBox MY_ANSIARGS((EZ_Widget *parent,
					       int hsc, int vsc, int ncol, int selctype));
EXTERN void       EZ_DrawWidgetFancyListBox MY_ANSIARGS((EZ_Widget *widget));
EXTERN void       EZ_GetFancyListBoxWidgetComponents MY_ANSIARGS((EZ_Widget *widget,
							   EZ_Widget **ifl,
							   EZ_Widget **hsc,
							   EZ_Widget **vsc));

EXTERN EZ_Item    **EZ_GetFancyListBoxSelection MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Item    *EZ_GetFancyListBoxItemUsingIdx  MY_ANSIARGS((EZ_Widget *widget, int row, int col));
EXTERN void       EZ_GetFancyListBoxSelectionIdx  MY_ANSIARGS((EZ_Widget *widget, int *row, int *col));
EXTERN void       EZ_SetFancyListBoxData  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **data, int nrow, int ncol));
EXTERN void       EZ_FancyListBoxClear  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void       EZ_GetFancyListBoxXYOffset  MY_ANSIARGS((EZ_Widget *widget, int *xoff, int *yoff));
EXTERN void       EZ_SetFancyListBoxXYOffset  MY_ANSIARGS((EZ_Widget *widget, int xoff, int yoff));
EXTERN void       EZ_SetFancyListBoxHeader  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **header, int nelts));

EXTERN void       EZ_FancyListBoxInsertRow  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **items, int nitems, int where));
EXTERN void       EZ_FancyListBoxDeleteRow  MY_ANSIARGS((EZ_Widget *widget, int idx));
EXTERN void       EZ_FancyListBoxDeleteSomeRows MY_ANSIARGS((EZ_Widget *widget, int *idces, int cnt));
EXTERN void       EZ_FancyListBoxReplaceItem  MY_ANSIARGS((EZ_Widget *widget, EZ_Item *old, EZ_Item *newItem));
EXTERN void       EZ_FancyListBoxReplaceItemUsingIdx  MY_ANSIARGS((EZ_Widget *widget, int row, int col, EZ_Item *newItem));
EXTERN void       EZ_FancyListBoxSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int Srow, int Scol, int *location));
EXTERN void       EZ_FancyListBoxSelectItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int *location));
EXTERN void       EZ_FancyListBoxSelectRow MY_ANSIARGS((EZ_Widget *widget, int rowN, int *location));
EXTERN EZ_Item    **EZ_FancyListBoxFindRowAtXY  MY_ANSIARGS((EZ_Widget *fl, int x, int y, int *r_ret));
EXTERN EZ_Item    *EZ_FancyListBoxFindItemAtXY  MY_ANSIARGS((EZ_Widget *fl, int x, int y, int *r_ret, int *c_ret));
EXTERN EZ_Item    ***EZ_GetFancyListBoxData  MY_ANSIARGS((EZ_Widget *widget));

EXTERN int        *EZ_FancyListBoxGetHighlightedRowsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
EXTERN int        EZ_FancyListBoxGetHighlightedRows MY_ANSIARGS((EZ_Widget *widget, int *ary, int siz));
EXTERN EZ_Item    **EZ_FancyListBoxGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *cnt));
EXTERN int        EZ_FancyListBoxGetHighlightedItems MY_ANSIARGS((EZ_Widget *widget, EZ_Item **arg,int siz));
EXTERN int        EZ_FancyListBoxGetHithlightedItemsAndIndices MY_ANSIARGS((EZ_Widget *wgt, EZ_Item ***itm_ret,
									    int **rowIdx_ret, int **colIdx_ret));
EXTERN void    EZ_SetFancyListBoxRowBg MY_ANSIARGS((EZ_Widget *widget, int mode, char *bg1, char *bg2));
#endif
/*
 * From EZ_Item.c
 */
#ifndef _EZ_ITEM_C_

EXTERN EZ_Item *EZ_AllocateNewDisplayItem MY_ANSIARGS((void));
EXTERN EZ_Item *EZ_DuplicateItem MY_ANSIARGS((EZ_Item *item));
EXTERN void    EZ_ComputeItemSize MY_ANSIARGS((
					EZ_Item *item,
					int *w_ret, int *h_ret));
EXTERN void    EZ_DisplayOneItem MY_ANSIARGS((
				       EZ_Widget *widget,
				       Drawable  drawable,
				       int       x0, int y0,
				       EZ_Item   *item,
				       int       Rx, int Ry, int  Rw, int Rh));
EXTERN void    EZ_UpdateOneItem MY_ANSIARGS((
				      EZ_Widget *widget,
				      Drawable  drawable,
				      EZ_Item   *item,
				      int       Rx, int Ry, int  Rw, int Rh));

EXTERN void    EZ_HighlightOneItem MY_ANSIARGS((
					 EZ_Widget *widget,
					 Drawable  drawable,
					 EZ_Item   *item,
					 unsigned long fg,  unsigned long bg, 
					 int       Rx, int Ry, int  Rw, int Rh, int flag));
EXTERN void    EZ_DestroyItem MY_ANSIARGS((EZ_Item *item));
EXTERN void    EZ_DestroyItemFinal MY_ANSIARGS((EZ_Item *item));

EXTERN void    EZ_GetItemXYOffset MY_ANSIARGS((EZ_Item *item, int *xoff, int *yoff));
EXTERN void    EZ_GetItemBwAndXYPading MY_ANSIARGS((EZ_Item *item, int *bw, int *xpad, int *ypad));

EXTERN void    EZ_GetItemClientData MY_ANSIARGS((EZ_Item *item,
					  void **pdata_ret,
					  int *idata_ret));
EXTERN int     EZ_GetItemIntData MY_ANSIARGS((EZ_Item *item));
EXTERN void    *EZ_GetItemPtrData MY_ANSIARGS((EZ_Item *item));
EXTERN void     EZ_SetItemClientData MY_ANSIARGS((EZ_Item *item,
					  int idata, void *pdata ));
EXTERN void     EZ_SetItemIntData MY_ANSIARGS((EZ_Item *item, int i));
EXTERN void     EZ_SetItemPtrData MY_ANSIARGS((EZ_Item *item, void *p));


EXTERN void    EZ_ComputeItemArrayGeometry MY_ANSIARGS((EZ_Item ***array,
						 int nrows,
						 int ncolumns,
						 int *w_ret, int *h_ret,
						 int *w_det_ret, int *h_det_ret));
EXTERN void    EZ_DisplayOneRowOfItems MY_ANSIARGS((EZ_Widget *widget,
						    Drawable drawable,
						    EZ_Item **row,
						    int nelts,
						    int x0,int y0,int hsep,
						    int *w_sizes,
						    int Rx,int Ry,int Rw,int Rh,
						    unsigned long fg, unsigned long bg, 
						    int hmode, int selmode
						    ));
EXTERN void    EZ_DisplayHeaderRowIFLB MY_ANSIARGS((EZ_Widget *widget,
						    Drawable drawable,
						    EZ_Item **row,
						    int nelts,
						    int x0,int y0,int hsep,
						    int *w_sizes,
						    int Rx,int Ry,int Rw,int Rh,
						    unsigned long fg, unsigned long bg, 
						    int hmode, int selmode
						    ));

EXTERN void    EZ_UpdateOneRow MY_ANSIARGS((EZ_Widget *widget,
					    Drawable drawable,
					    EZ_Item **row,
					    int nelts,
					    int cx,int cy,int cw, int ch));
EXTERN void    EZ_HighlightOneRow MY_ANSIARGS((EZ_Widget *widget,
					       Drawable drawable,
					       EZ_Item **row,
					       int nelts,
					       unsigned long fg, unsigned long bg,
					       int cx,int cy,int cw, int ch, int hmode));

EXTERN int     EZ_PickAnItem MY_ANSIARGS(( EZ_Item *item, int x, int y));

EXTERN void    EZ_ItemConfigureSentienl MY_ANSIARGS(( EZ_Item *item, int dirty));


EXTERN void    EZ_AddItemCallBack MY_ANSIARGS((EZ_Item *item, int which,
					EZ_CallBack callback, void *cdata,
					int where));
EXTERN void    EZ_AddItemCallBackInternal MY_ANSIARGS((EZ_Item *item, int which,
						EZ_CallBack callback, void *cdata,
						int where));
EXTERN void    EZ_RemoveItemCallBack MY_ANSIARGS((EZ_Item *item,  int which,
					   EZ_CallBack callback, void *cdata));
EXTERN void    EZ_RemoveAllItemCallBacks MY_ANSIARGS((EZ_Item *item, int which));

EXTERN void    EZ_SetItemPosition MY_ANSIARGS((EZ_Item *item, int x, int y));
EXTERN void    EZ_SetItemWidth MY_ANSIARGS((EZ_Item *item, int width));
EXTERN void    EZ_SetItemHeight MY_ANSIARGS((EZ_Item *item, int height));
EXTERN void    EZ_SetItemDimension MY_ANSIARGS((EZ_Item *item, int w, int h));
EXTERN int     EZ_GetItemWidth MY_ANSIARGS((EZ_Item *item));
EXTERN int     EZ_GetItemHeight MY_ANSIARGS((EZ_Item *item));
EXTERN void    EZ_GetItemDimension MY_ANSIARGS((EZ_Item *item, int *w, int *h));
EXTERN void    EZ_GetItemADimension MY_ANSIARGS((EZ_Item *item, int *w, int *h));
EXTERN void    EZ_GetItemPosition MY_ANSIARGS((EZ_Item *item, int *x, int *y));
EXTERN void    EZ_GetItemAPosition MY_ANSIARGS((EZ_Item *item, int *x, int *y));
EXTERN void    EZ_SetItemAttributes MY_ANSIARGS((EZ_Item *item, int option, int i1, int i2, 
						 void *p1, void *p2, 
						 unsigned long ul1, unsigned long ul2,
						 int i3, int i4));
EXTERN int      EZ_FillItemBackground MY_ANSIARGS((EZ_Widget *wptr, EZ_Item *item, Drawable d, int x0, int y0,
						   int w0, int h0, int x1, int y1, 
						   unsigned long *bpv));
EXTERN EZ_Widget *EZ_GetItemHostWidget MY_ANSIARGS((EZ_Item *item));
EXTERN void       EZ_UpdateDisplayItem  MY_ANSIARGS((EZ_Item *item));

EXTERN int      EZ_GetItemUnknownData MY_ANSIARGS((EZ_Item *item, int idx,
						   EZ_UnknownDataType *ret));
EXTERN void     EZ_SetItemUnknownData  MY_ANSIARGS((EZ_Item *item, int idx,
						    EZ_UnknownDataType data));
EXTERN EZ_UnknownDataType *EZ_GetItemUnknownDataPtr MY_ANSIARGS((EZ_Item *item));
EXTERN int     EZ_GetItemType MY_ANSIARGS((EZ_Item *item));
#endif
/*
 * From EZ_LabelItem.c
 */
#ifndef _EZ_ITEM_LABEL_C_
EXTERN EZ_Item    *EZ_CreateLabelItem MY_ANSIARGS((char *str, EZ_TextProperty *prop));
EXTERN void        EZ_LabelItemSetProperty MY_ANSIARGS((EZ_Item *item, EZ_TextProperty *prop));
EXTERN void        EZ_GetLabelItemStringInfo MY_ANSIARGS((EZ_Item *item, char **str, int *len));
EXTERN EZ_Bitmap   *EZ_GetLabelItemPixmap MY_ANSIARGS((EZ_Item *item));
#endif

#ifndef _EZ_ITEM_FIG_C_
EXTERN void   EZ_FreezeFigItem MY_ANSIARGS((EZ_Item *item));
EXTERN void   EZ_UnFreezeFigItem MY_ANSIARGS((EZ_Item *item));
EXTERN void   EZ_GetFigPieceBoundingBox MY_ANSIARGS((EZ_FigPiece *fpiece,
						     int *x1, int *y1, int *x2, int *y2));
EXTERN EZ_Item    *EZ_CreateFigItem MY_ANSIARGS((void));
EXTERN void       EZ_FigItemHidePiece MY_ANSIARGS((EZ_Item *item, EZ_FigPiece *fpiece, int upd));
EXTERN void       EZ_FigItemShowPiece MY_ANSIARGS((EZ_Item *item, EZ_FigPiece *fpiece, int upd));

EXTERN void       EZ_FigItemDeletePiece MY_ANSIARGS((EZ_Item *item, EZ_FigPiece *fpiece, int upd));
EXTERN void       EZ_FigItemDeleteAllPieces MY_ANSIARGS((EZ_Item *item, int upd));
EXTERN EZ_FigPiece *EZ_FigItemAddPiece MY_ANSIARGS((EZ_Item *fitem, int ptype,
						    GC theGC,
						    int x, int y, int width, int height, int other,
						    int npoints, XPoint *xpts, int mode));
EXTERN void EZ_DuplicatePartList MY_ANSIARGS((EZ_Item *item, EZ_FigPiece **np, EZ_FigPiece *op));
EXTERN void EZ_ScaleFigItem MY_ANSIARGS((EZ_Item *item, float sx, float sy, int mode));
EXTERN void EZ_CleanUpFigItemGCs MY_ANSIARGS((void));
EXTERN GC     EZ_GetGC MY_ANSIARGS((unsigned long mask, XGCValues *gcv));
EXTERN void   EZ_FreeGC MY_ANSIARGS((GC gc));
EXTERN Pixmap EZ_MakePixmapFromFigItem MY_ANSIARGS((EZ_Item *fitem, int *width, int *height));
#endif

#ifndef _EZ_ITEM_UDF_C_
EXTERN EZ_Item   *EZ_CreateUDFItem MY_ANSIARGS((int type));
EXTERN void      EZ_SetupUDFItem MY_ANSIARGS((EZ_Item *item));
EXTERN void      *EZ_SearchUDFItemHandle MY_ANSIARGS(( int type));
EXTERN void      EZ_DeleteAllUDFItemHandles MY_ANSIARGS((void));
EXTERN int       EZ_RegisterItem MY_ANSIARGS((char *iname, char *cname,
				     void (*Setup)  MY_ANSIARGS((EZ_Item *item)),
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
EXTERN EZ_UnknownDataType *EZ_GetUDFItemPrivateData MY_ANSIARGS((EZ_Item *item));
#endif

/*
 * From EZ_WidgetItem.c
 */
#ifndef _EZ_ITEM_WIDGET_C_
EXTERN EZ_Item    *EZ_CreateWidgetItem  MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget  *EZ_GetWidgetItemWidget MY_ANSIARGS((EZ_Item *item));
EXTERN void        EZ_SetWidgetItemWidget MY_ANSIARGS((EZ_Item *item, EZ_Widget *widget));
EXTERN void        EZ_PostscriptWidget MY_ANSIARGS((EZ_Widget *widget, FILE *fp));
EXTERN void        EZ_SaveWidget2Postscript MY_ANSIARGS((EZ_Widget *widget, int landscape, int dpi, 
						  float scale, FILE *fp));
EXTERN void        EZ_SaveWidget2PPM  MY_ANSIARGS((EZ_Widget *widget, FILE *fp));
#endif


/*
 * From EZ_Tree.c
 */
#ifndef _EZ_TREE_C_
EXTERN EZ_TreeNode  *EZ_CreateTreeNode  MY_ANSIARGS((EZ_TreeNode *parent, EZ_Item *item));      
EXTERN EZ_TreeNode  *EZ_TreeNodeGetParent MY_ANSIARGS((EZ_TreeNode *node));      
EXTERN EZ_TreeNode  *EZ_TreeNodeGetChildren MY_ANSIARGS((EZ_TreeNode *node));      
EXTERN EZ_TreeNode  *EZ_TreeNodeGetSibling MY_ANSIARGS((EZ_TreeNode *node));      
EXTERN EZ_TreeNode  *EZ_TreeFindNode MY_ANSIARGS((EZ_TreeNode *root, EZ_TreeNode *node));
EXTERN EZ_TreeNode  *EZ_TreeFindNodeUsingItem MY_ANSIARGS((EZ_TreeNode *root, EZ_Item *item));
EXTERN EZ_TreeNode  *EZ_TreeFindNodeUsingProcedure MY_ANSIARGS((EZ_TreeNode *root, 
			 EZ_TreeNode *(*f)MY_ANSIARGS((EZ_TreeNode *node, void *d)), void *data ));
EXTERN void     EZ_TreeDestroySubtree MY_ANSIARGS((EZ_TreeNode *node));      
EXTERN void     EZ_TreeDestroyNodeDescendants MY_ANSIARGS((EZ_TreeNode *node));      
EXTERN void     EZ_TreeAttachSubtree MY_ANSIARGS((EZ_TreeNode *node, EZ_TreeNode *subtree));      
EXTERN void     EZ_TreeSetCharacter MY_ANSIARGS((EZ_TreeNode *node,
						     int type, int indent, int spacint, int drawBranch));      
EXTERN void     EZ_TreeTraverseTree MY_ANSIARGS((EZ_TreeNode *node,
					  void (*preF) MY_ANSIARGS((EZ_TreeNode *node)),
						 void (*postF) MY_ANSIARGS((EZ_TreeNode *node))	  ));

EXTERN void     EZ_TreeSetFGColor  MY_ANSIARGS((EZ_TreeNode *node, unsigned long pv));

EXTERN void     EZ_TreeSetBGColor MY_ANSIARGS((EZ_TreeNode *node, unsigned long pv));

EXTERN void     EZ_ComputeSubtreeSize MY_ANSIARGS((EZ_TreeNode *node, int *w, int *h, int *p));

EXTERN void     EZ_DisplayTreeInDrawable MY_ANSIARGS((EZ_Widget *widget, Drawable drawable,
					       int x0, int y0,
					       EZ_TreeNode *node,
						      int Rx, int Ry, int Rw, int Rh));

EXTERN EZ_Item *EZ_TreeNodeGetItem MY_ANSIARGS((EZ_TreeNode *node));
EXTERN int      EZ_UpdateDirTreeWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN void     EZ_TreeNodeMarkTerminal MY_ANSIARGS((EZ_TreeNode *node, int flag));      
EXTERN void     EZ_TreeHideSubtree MY_ANSIARGS((EZ_TreeNode *node));      
EXTERN void     EZ_TreeShowSubtree MY_ANSIARGS((EZ_TreeNode *node));      
#endif
/*
 * From EZ_TreeDir.c
 */
#ifndef _EZ_TREE_DIR_C_
EXTERN EZ_TreeNode      *EZ_CreateDirTree MY_ANSIARGS((EZ_DirTreeNodeCreator nctr,
						char *pattern, void *cdata, int omode));
EXTERN EZ_TreeNode      *EZ_CreateDirTreeS MY_ANSIARGS((EZ_DirTreeNodeCreator nctr,
						char *pattern, void *cdata, int omode, void *sorter));
EXTERN EZ_TreeNode      *EZ_CreateDirTreeNodeDefault MY_ANSIARGS((char *fname));
EXTERN void             EZ_OpenCloseDirTreeDirectoryNode MY_ANSIARGS((EZ_Item *item, int col));
EXTERN char             *EZ_GetDirTreeNodeFullPath MY_ANSIARGS((EZ_TreeNode *node));
EXTERN void             EZ_SetDirTreeOpenDirectoryMode MY_ANSIARGS((EZ_TreeNode *node, int mode));
EXTERN void             EZ_SetDirTreeDefaultPixmaps MY_ANSIARGS((EZ_Bitmap *o, EZ_Bitmap *c, EZ_Bitmap *f));
EXTERN void             EZ_SetDirTreeDefaultPixmapsFromXpmFiles MY_ANSIARGS((char *o, char *c, char *f));
EXTERN int              EZ_UpdateDirTreeWork MY_ANSIARGS((EZ_TreeNode *root,EZ_TreeNode *sel,int *flag));
EXTERN void             EZ_SetDirTreeSorter MY_ANSIARGS((EZ_TreeNode *root, int (*cmp)MY_ANSIARGS((void *, void *)) ));
#endif
/*
 * From EZ_WidgetIWorkArea.c
 */
#ifndef _EZ_WIDGET_IWORK_AREA_C_
EXTERN EZ_Widget        *EZ_CreateIWorkArea MY_ANSIARGS((EZ_Widget *parent));
EXTERN void             EZ_DrawWidgetIWorkArea MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ComputeWidgetIWorkAreaSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_FreeWidgetIWorkAreaData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_IWorkAreaEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EXTERN void             EZ_GrowIWorkAreaDataSpace MY_ANSIARGS((EZ_Widget *widget, int increment));

EXTERN EZ_Item          *EZ_FindPointerOnItemIWA MY_ANSIARGS((EZ_Item **data, int count,  EZ_Item *guess, 
							     int x, int y));

EXTERN void              EZ_IWorkAreaInsertItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));
EXTERN void              EZ_IWorkAreaInsertItems MY_ANSIARGS((EZ_Widget *widget, EZ_Item **item, int n));
EXTERN void              EZ_IWorkAreaDeleteItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));
EXTERN void              EZ_IWorkAreaDeleteItemO MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int dp));
EXTERN void              EZ_IWorkAreaDeleteAllItems MY_ANSIARGS((EZ_Widget *widget));

EXTERN void              EZ_IWorkAreaInsertItemBeforeAfter MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item,
								 EZ_Item *newitem, int BefAft));
EXTERN void              EZ_IWorkAreaInsertItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx, EZ_Item *item));
EXTERN void              EZ_IWorkAreaDeleteItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx));
EXTERN void              EZ_IWorkAreaDeleteItemUsingIdxO MY_ANSIARGS((EZ_Widget *widget, int idx, int dp));
EXTERN void              EZ_IWorkAreaDeleteSomeItemsUsingIndices  MY_ANSIARGS((EZ_Widget *widget, int *ids, int cnt));
EXTERN void              EZ_IWorkAreaSelectItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int *location));
EXTERN void              EZ_IWorkAreaSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx, int *location));

EXTERN EZ_Item           **EZ_IWorkAreaGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *n_ret));
EXTERN int               EZ_IWorkAreaGetHighlightedItems MY_ANSIARGS((EZ_Widget *widget,  EZ_Item ** a, int siz));

EXTERN void EZ_IWorkAreaMoveOneItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, 
						 int x, int y, int bx,int by,int bw,int bh));
EXTERN void EZ_WorkAreaInsertAndDrawItemAt MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, 
							int *location, int disp));
EXTERN void EZ_WorkAreaDrawItemAt MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int x, int y));

EXTERN void EZ_WorkAreaScaleItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int mode,
					      float sx, float sy, int tx, int ty, int disp));

EXTERN void EZ_WorkAreaRaiseItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));
EXTERN void EZ_MakeNItemOutlines MY_ANSIARGS((EZ_Item **items, int nitems, int xof, int yof));
EXTERN void EZ_MoveNItemOutlines MY_ANSIARGS((int x, int y, int flag, int dcnt, int dndflag));

EXTERN void EZ_IFListBoxMakeNRowOutLines MY_ANSIARGS((EZ_Widget *widget, int nnn, int wox, int woy));
EXTERN void EZ_IFListBoxMakeNItemOutLines MY_ANSIARGS((EZ_Widget *widget, int nnn, int wox, int woy));
#endif
/*
 * From EZ_WidgetWorkArea.c
 */
#ifndef _EZ_WIDGET_WORK_AREA_C_
EXTERN EZ_Widget        *EZ_CreateWorkArea MY_ANSIARGS((EZ_Widget *parent, int hsc, int vsc));
EXTERN void             EZ_DrawWidgetWorkArea MY_ANSIARGS((EZ_Widget *widget));

EXTERN EZ_Item          *EZ_GetWorkAreaSelection MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_SetWorkAreaGeometryManager MY_ANSIARGS((EZ_Widget *widget,
								    EZ_WorkAreaGeometryManager managet,
								    void *pdata  ));

EXTERN void              EZ_WorkAreaInsertItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));
EXTERN void              EZ_WorkAreaInsertItems MY_ANSIARGS((EZ_Widget *widget, EZ_Item **item, int n));
EXTERN void              EZ_WorkAreaDeleteItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));
EXTERN void              EZ_WorkAreaDeleteItemO MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int dp));
EXTERN void              EZ_WorkAreaClear MY_ANSIARGS((EZ_Widget *widget));

EXTERN void              EZ_WorkAreaRearrange MY_ANSIARGS((EZ_Widget *widget));

EXTERN void              EZ_WorkAreaInsertItemBefore MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, EZ_Item *newItem));
EXTERN void              EZ_WorkAreaInsertItemAfter  MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, EZ_Item *newItem));
EXTERN void              EZ_WorkAreaInsertItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int idx));
EXTERN void              EZ_WorkAreaDeleteItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx));
EXTERN void              EZ_WorkAreaDeleteItemUsingIdxO MY_ANSIARGS((EZ_Widget *widget, int idx, int dp));
EXTERN void              EZ_WorkAreaDeleteSomeItemsUsingIndices  MY_ANSIARGS((EZ_Widget *widget, int *ids, int cnt));
EXTERN void              EZ_GetWorkAreaWidgetComponents MY_ANSIARGS((EZ_Widget *widget, EZ_Widget **iwa, 
							EZ_Widget **hsc, EZ_Widget **vsc));
EXTERN void              EZ_WorkAreaSelectItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int *location));
EXTERN void              EZ_WorkAreaSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx, int *location));

EXTERN EZ_Item           *EZ_WorkAreaFindItemAtXY MY_ANSIARGS((EZ_Widget *wa, int x, int y));

EXTERN void              EZ_GetWorkAreaXYOffset MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));
EXTERN void              EZ_SetWorkAreaXYOffset MY_ANSIARGS((EZ_Widget *widget, int x, int y));

EXTERN int               EZ_GetWorkAreaNumberOfItems MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Item           *EZ_GetWorkAreaItemUsingIdx MY_ANSIARGS((EZ_Widget *wa, int x));
EXTERN EZ_Item           **EZ_GetWorkAreaData MY_ANSIARGS((EZ_Widget *wa));

EXTERN EZ_Item           **EZ_WorkAreaGetHighlightedItemsA MY_ANSIARGS((EZ_Widget *widget, int *n_ret));
EXTERN int               EZ_WorkAreaGetHighlightedItems MY_ANSIARGS((EZ_Widget *widget,  EZ_Item ** a, int siz));
#endif

/*
 * From EZ_WorkAreaGeom.c
 */
#ifndef _EZ_WORK_AREA_GEOM_C_
EXTERN void             EZ_WorkAreaGeometryManagerDefault MY_ANSIARGS((void *GMData, EZ_Item **data, int nitems,
								       int Rx,int Ry,int Rw, int Rh, int Wxsp, int Wysp, 
								       int *w_ret, int *h_ret));
#endif


/*
 * From EZ_WidgetRawXWin.c
 */
#ifndef _EZ_WIDGET_RAWXWIN_C_
EXTERN EZ_Widget       *EZ_CreateRawXWindow MY_ANSIARGS((EZ_Widget *parent));
EZ_Widget              *EZ_CreateUDFWidget MY_ANSIARGS((int type, EZ_Widget *parent,
                                                        char *iname, char *cname));
EXTERN void             EZ_SetupRawXWinWidget MY_ANSIARGS(( EZ_Widget *widget,
	         	    void (*ComputeSize) MY_ANSIARGS(( EZ_Widget *widget, int *w, int *h)),
                            void  (*DrawWidget)  MY_ANSIARGS((EZ_Widget *widget)),
                            void  (*FreeData)    MY_ANSIARGS((EZ_Widget *widget)),
                            void  (*EventHandle) MY_ANSIARGS((EZ_Widget *widget, XEvent *event)) ));

EXTERN int    EZ_RegisterPrimitiveWidget MY_ANSIARGS((char *iname, char *cname,
                    void  (*Setup)  MY_ANSIARGS((EZ_Widget *widget)),
		    void (*ComputeSize) MY_ANSIARGS(( EZ_Widget *widget, int *w, int *h)),
                    void  (*DrawWidget)  MY_ANSIARGS((EZ_Widget *widget)),
                    void  (*FreeData)    MY_ANSIARGS((EZ_Widget *widget)),
                    void  (*EventHandle) MY_ANSIARGS((EZ_Widget *widget, XEvent *event)) ));


EXTERN int    EZ_RegisterCompositeWidget MY_ANSIARGS(( char *iname, char *cname,
                    EZ_Widget  *(*Create)  MY_ANSIARGS((EZ_Widget *parent, char *in, char *cn))
                                                       ));

EXTERN void  EZ_RegisterResourcesAll MY_ANSIARGS((int type, int nresrc, EZ_ResourceSpec *spec, 
                    int  (*Configure)   MY_ANSIARGS((EZ_Widget *widget, int opt, EZ_Value *values))
                                        ));
EXTERN void  EZ_RegisterResourcesWidget MY_ANSIARGS((EZ_Widget *widget, int nresrc, EZ_ResourceSpec *spec, 
                    int  (*Configure)   MY_ANSIARGS((EZ_Widget *widget, int opt, EZ_Value *values))
                                        ));


EXTERN EZ_ResourceHandle *EZ_GetDefaultResourceHandle MY_ANSIARGS(( int type));
EXTERN void EZ_FreeResourceHandles  MY_ANSIARGS((void));

EXTERN void   *EZ_SearchUDFWidgetHandle MY_ANSIARGS(( int type));
EXTERN void   EZ_DeleteAllUDFWidgetHandles MY_ANSIARGS((void));

EXTERN EZ_UnknownDataType *EZ_GetRawXWinPrivateData MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_UnknownDataType *EZ_GetUDFWidgetPrivateData MY_ANSIARGS((EZ_Widget *widget));

EXTERN GC   EZ_GetWritableGC MY_ANSIARGS((void));
EXTERN GC   EZ_GetXorGC MY_ANSIARGS((void));
EXTERN GC   EZ_GetDnDXorGC MY_ANSIARGS((void));
EXTERN GC   EZ_GetDnDDragGC MY_ANSIARGS((void));
EXTERN GC   EZ_GetDashedLineGC MY_ANSIARGS((void));

EXTERN int  EZ_SetWidgetUDFAttributes MY_ANSIARGS((EZ_Widget *widget, int opt, EZ_Value *values));
EXTERN unsigned char  *EZ_GetUDFResourceVTypes MY_ANSIARGS((EZ_ResourceHandle *handle, int option));
EXTERN void   EZ_extractValueTypes MY_ANSIARGS((char *spec, unsigned char *out));
EXTERN void  EZ_RecordResourceSpec MY_ANSIARGS((EZ_ResourceHandle *tmp,
                                                int nspec, EZ_ResourceSpec *spec,
          int (*configure) MY_ANSIARGS((EZ_Widget *widget, int opt, EZ_Value *values))
                                                ));
#endif

/*
 * From EZ_DnDData.c
 */
#ifndef _EZ_DND_DATA_C_

EXTERN void EZ_ItemAddDnDDataEncoder MY_ANSIARGS((EZ_Item *item, Atom type, unsigned int tag,
						     EZ_DnDEncoder encoder, void *edata,
						     EZ_CallBack callback, void *cdata));
EXTERN void EZ_ItemDeleteDnDDataEncoder MY_ANSIARGS((EZ_Item *item, Atom type));
EXTERN void EZ_ItemDeleteAllDnDDataEncoders MY_ANSIARGS((EZ_Item *item));
EXTERN void EZ_ItemAddDnDDataDecoder MY_ANSIARGS((EZ_Item *item, Atom type, unsigned int tag,
						     EZ_DnDDecoder decoder, void *edata,
						     EZ_CallBack callback, void *cdata));
EXTERN void EZ_ItemDeleteDnDDataDecoder MY_ANSIARGS((EZ_Item *item, Atom type));
EXTERN void EZ_ItemDeleteAllDnDDataDecoders MY_ANSIARGS((EZ_Item *item));


EXTERN void EZ_WidgetAddDnDDataEncoder MY_ANSIARGS((EZ_Widget *widget, Atom type, unsigned int tag,
						         EZ_DnDEncoder encoder, void *edata,
						         EZ_CallBack callback, void *cdata));
EXTERN void EZ_WidgetDeleteDnDDataEncoder MY_ANSIARGS((EZ_Widget *widget, Atom type));
EXTERN void EZ_WidgetDeleteAllDnDDataEncoders MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_WidgetAddDnDDataDecoder MY_ANSIARGS((EZ_Widget *widget,Atom type, unsigned int tag,
						    EZ_DnDDecoder encoder, void *edata,
						    EZ_CallBack callback, void *cdata));
EXTERN void EZ_WidgetDeleteDnDDataDecoder MY_ANSIARGS((EZ_Widget *widget, Atom type));
EXTERN void EZ_WidgetDeleteAllDnDDataDecoders MY_ANSIARGS((EZ_Widget *widget));

EXTERN EZ_DnDDataEncoder *EZ_FindEncoderGivenTarget MY_ANSIARGS((EZ_DnDDataEncoder *list, Atom target));
EXTERN EZ_DnDDataDecoder *EZ_FindDecoderGivenTarget MY_ANSIARGS((EZ_DnDDataDecoder *list, Atom target));

EXTERN void EZ_WidgetAddDnDDataEncoder00 MY_ANSIARGS((EZ_Widget *widget, Atom type, unsigned int tag,
					     EZ_DnDEncoder encoder, void *edata,
					     EZ_CallBack callback, void *cdata));
EXTERN void EZ_WidgetDeleteDnDDataEncoder00 MY_ANSIARGS((EZ_Widget *widget, 
						EZ_DnDEncoder encoder, void *edata));
EXTERN void EZ_WidgetDeleteAllDnDDataEncoders00 MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_WidgetAddDnDDataDecoder00 MY_ANSIARGS((EZ_Widget *widget,Atom type, unsigned int tag,
					     EZ_DnDDecoder encoder, void *edata,
					     EZ_CallBack callback, void *cdata));
EXTERN void EZ_WidgetDeleteDnDDataDecoder00 MY_ANSIARGS((EZ_Widget *widget,
						EZ_DnDDecoder decoder, void *edata));
EXTERN void EZ_WidgetDeleteAllDnDDataDecoders00 MY_ANSIARGS((EZ_Widget *widget));
#endif
/*
 * From EZ_DnD.c
 */
#ifndef _EZ_DND_C_
EXTERN Atom EZ_GetAtom  MY_ANSIARGS((char *name));
EXTERN int  EZ_DnDTargetTryNextTarget MY_ANSIARGS((void));
EXTERN void EZ_InitializeDnD MY_ANSIARGS((void));
EXTERN void EZ_Insert2DnDWidgetList MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_RemveWidgetFromDnDList MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_DestroyDnDList MY_ANSIARGS((void));
EXTERN void EZ_GrabButtonRelease MY_ANSIARGS((void));
EXTERN void EZ_UngrabButtonRelease MY_ANSIARGS((void));
EXTERN void EZ_DnDSendDataConversionRequest MY_ANSIARGS((void));
EXTERN void EZ_InitDrag MY_ANSIARGS((unsigned long id,
			      int type,
			      Window window,
			      EZ_Widget *widget,
			      void      *item,
			      int x, int y, int w, int h,
			      int px, int py, int xoff, int yoff));
EXTERN void EZ_FinishDrag MY_ANSIARGS((Window cwin, Window win, int px, int py));
EXTERN void EZ_AbortDrag MY_ANSIARGS((void));
EXTERN void EZ_DnDMatchTargetsWithDecoders MY_ANSIARGS((void));
EXTERN void EZ_DnDFindNextDecoder MY_ANSIARGS((void));
EXTERN void EZ_DnDFindDataEncoder MY_ANSIARGS((Atom target));
EXTERN void EZ_DnDSendDataConversionRequest MY_ANSIARGS((void));
EXTERN void  EZ_DnDSendErrorMessage MY_ANSIARGS((int sender, int errType, EZ_DnDMessage *theMsg));
EXTERN void EZ_DnDSendConversion MY_ANSIARGS((Atom type, char *message, int length ));
EXTERN void EZ_HighlightDropSiteWidget MY_ANSIARGS((void));
EXTERN void EZ_DnDInformDeliverError MY_ANSIARGS((int type, EZ_DnDMessage *theMsg));
EXTERN void EZ_EncodeDnDMessageHeader MY_ANSIARGS((int type, char *message, int length, Window *receiver));
EXTERN void EZ_DecodeDnDMessageHeader  MY_ANSIARGS((char *message, EZ_DnDMessage *theMsg));
 
EXTERN void EZ_DnDSendRequestHelpMsg MY_ANSIARGS((void));
EXTERN void EZ_DnDSendCancelHelpMsg MY_ANSIARGS((void));
EXTERN void EZ_DnDShowTargetHelp MY_ANSIARGS((void));
EXTERN void EZ_DnDCancelTargetHelp MY_ANSIARGS((void));
EXTERN void EZ_DnDSendLeaveWindowMsg MY_ANSIARGS((void));

EXTERN void EZ_EnableDnDBubbleHelp MY_ANSIARGS((void));
EXTERN void EZ_DisableDnDBubbleHelp MY_ANSIARGS((void));
EXTERN int  EZ_CheckDnDTimer MY_ANSIARGS((void));
EXTERN void EZ_Insert2DnDWindowList MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_RemoveFromDnDWindowList MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_DnDFinishDrag MY_ANSIARGS((void));
EXTERN void EZ_DnDFinishDrop MY_ANSIARGS((void));
EXTERN void EZ_DnDGetDragConversionTargets MY_ANSIARGS((Atom **targets_ret, int **ntargets_ret, 
							unsigned int **t));
EXTERN void EZ_DnDGetMatchedConversionTargets MY_ANSIARGS((Atom **targets_ret, int **ntargets_ret, 
							   unsigned int **t));
EXTERN void EZ_CallDragInitProcedure  MY_ANSIARGS((void));
EXTERN void EZ_SetDragIcon MY_ANSIARGS((EZ_Widget *widget, int dpyStyle));
EXTERN void EZ_SetDragCursor MY_ANSIARGS((Cursor cursor));
EXTERN void EZ_ShowDragIcon MY_ANSIARGS((int x, int y));
EXTERN void EZ_HandleDnDEvents MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void EZ_SelectInput MY_ANSIARGS((Window win, long mask));
EXTERN long EZ_GetForeignWindowEventMask MY_ANSIARGS((Window win));
EXTERN void EZ_ResetForeignWindowEventMask MY_ANSIARGS((void));
#endif
/*
 * From EZ_DnDMsg.c
 */
#ifndef _EZ_DND_MSG_C_
EXTERN void EZ_HandleDnDMessage MY_ANSIARGS((char *data, unsigned long length));
EXTERN void EZ_SendDnDMessage MY_ANSIARGS((int type, char *msg, int len, int needFree));
EXTERN void EZ_BroadcastDnDMessage MY_ANSIARGS((int type, char *msg, int len, int needFree));
EXTERN void EZ_GenerateDragIsDropedMessage MY_ANSIARGS((char **msg_ret, int *len_ret));
EXTERN void EZ_GenerateDragStartedMessage MY_ANSIARGS((char **msg_ret, int *len_ret));
EXTERN void EZ_DnDGetTargetTypesFromMessage MY_ANSIARGS((EZ_DnDMessage *theMsg,
						  Atom *atoms,
						  unsigned int *cont));
EXTERN EZ_DnDDataEncoder *EZ_FindSpecialEncoder MY_ANSIARGS((void));
EXTERN void EZ_GetDnDDropLocation MY_ANSIARGS((int *x, int *y, int *xx, int *yy));
EXTERN void EZ_IWorkAreaDnDHandle MY_ANSIARGS((EZ_Widget *widget, EZ_DnDMessage *theMsg));
EXTERN void EZ_ITreeDnDHandle MY_ANSIARGS((EZ_Widget *widget, EZ_DnDMessage *theMsg));
EXTERN void EZ_IFListBoxDnDHandle MY_ANSIARGS((EZ_Widget *widget, EZ_DnDMessage *theMsg));

#endif

#ifndef _EZ_GIF_C_
EXTERN int EZ_ReadGIFToPixmap MY_ANSIARGS(( char *file,  int *width_return,   int *height_return,
                                            Pixmap *pixmap_return));

EXTERN int EZ_ReadGIFToRGB MY_ANSIARGS((
				    char     *file,
				    int      *width_return,
				    int       *height_return,
				    unsigned char   **rgb_return));
#endif

#ifndef _EZ_INPUT_C_
EXTERN void         EZ_InitializeInputIdList  MY_ANSIARGS((void));
EXTERN void         EZ_DestroyInputIdList  MY_ANSIARGS((void));
EXTERN void         *EZ_AddInput MY_ANSIARGS((int fd, int mask, 
				       EZ_InputCallBack callback,
				       void *data));
EXTERN void         EZ_RemoveInput MY_ANSIARGS((void *id));
EXTERN int          EZ_CheckAppInputs MY_ANSIARGS((unsigned long length));
#endif

/*
 * From EZ_WidgetRowCol.c
 */
#ifndef _EZ_WIDGET_ROWCOL_C_
EXTERN EZ_Widget  *EZ_CreateRowColumn MY_ANSIARGS((EZ_Widget *parent));
EXTERN EZ_Widget  *EZ_CreateGridBag MY_ANSIARGS((EZ_Widget *parent));
EXTERN void  EZ_ShowGrid MY_ANSIARGS((EZ_Widget *w));
EXTERN void  EZ_ShowGridAll  MY_ANSIARGS((EZ_Widget *w, int flag));
#endif

/*
 * From EZ_RowCol.c
 */
#ifndef _EZ_ROWCOL_C_
EXTERN void  EZ_RowColumnComputeGeometry MY_ANSIARGS((EZ_Widget *widget, 
						int *w, int *h,
						EZ_Widget *dirtyAns));
EXTERN void  EZ_RowColSetChildrenPositionAndSize MY_ANSIARGS((EZ_Widget *widget,
						       int w, int h));
EXTERN int   EZ_SetRowColConstrains MY_ANSIARGS((EZ_Widget *widget, 
					  int rc, int idx, int minsize, int weight, 
					  int pad));
#endif


#ifndef _EZ_RCGM_C_
EXTERN void  EZ_RowComputeGeometry MY_ANSIARGS((EZ_Widget *widget, 
						int *w, int *h,
						EZ_Widget *dirtyAns));
EXTERN void  EZ_ColComputeGeometry MY_ANSIARGS((EZ_Widget *widget, 
						int *w, int *h,
						EZ_Widget *dirtyAns));
EXTERN void  EZ_RowSetChildrenPositionAndSize MY_ANSIARGS((EZ_Widget *widget,
						       int w, int h));
EXTERN void  EZ_ColSetChildrenPositionAndSize MY_ANSIARGS((EZ_Widget *widget,
						       int w, int h));
EXTERN int   EZ_SetRowColMaxSize MY_ANSIARGS((EZ_Widget *widget, int size));
#endif



/*
 * From EZ_WidgetTearOff.c
 */
#ifndef _EZ_WIDGET_TEAR_OFF_C_
EXTERN EZ_Widget   *EZ_CreateMenuTearOffItem MY_ANSIARGS((EZ_Widget *parent));
EXTERN void         EZ_HandleToplevelMenuEvents  MY_ANSIARGS((EZ_Widget *widget, XEvent *ev));
EXTERN int          EZ_ConfigureTearOffMenu  MY_ANSIARGS((EZ_Widget *menu, int onoff));
EXTERN void         EZ_DeselectCurrentMenuItem  MY_ANSIARGS((EZ_Widget *item));
EXTERN EZ_Widget    *EZ_GetMenuCompanion  MY_ANSIARGS((EZ_Widget *item));
EXTERN void         EZ_RedisplayTearOffMenu MY_ANSIARGS((EZ_Widget *menu));
#endif


/*
 * From EZ_WidgetLcd.c
 */
#ifndef _EZ_WIDGET_LCD_C_
EXTERN void       EZ_SetLcdString MY_ANSIARGS((EZ_Widget *parent, char *str));
EXTERN void       EZ_SetLcdInteger MY_ANSIARGS((EZ_Widget *lcd, int n));
EXTERN void       EZ_SetLcdFloat MY_ANSIARGS((EZ_Widget *cd, float f));
EXTERN void       EZ_ClearLcd  MY_ANSIARGS((EZ_Widget *lcd));
EXTERN void       EZ_SetLcdNDigits  MY_ANSIARGS((EZ_Widget *lcd, int n));
EXTERN void       EZ_SetLcdFontSize  MY_ANSIARGS((EZ_Widget *lcd, int n));
EXTERN void       EZ_SetLcdForeground  MY_ANSIARGS((EZ_Widget *lcd, char *clr));
EXTERN void       EZ_SetLcdBackground  MY_ANSIARGS((EZ_Widget *lcd, char *clr));
EXTERN EZ_Widget  *EZ_CreateLcd MY_ANSIARGS((EZ_Widget *parent, int n));
#endif

#ifndef _EZ_WIDGET_LED_C_
EXTERN int   EZ_QueueLedAction MY_ANSIARGS((EZ_Widget *led, int type, int delay, int count,
					     char *msg,  char *clr));
EXTERN void  EZ_RemoveLedAction MY_ANSIARGS((EZ_Widget *led, int action));
EXTERN void  EZ_SetLedColorFunction MY_ANSIARGS((EZ_Widget *led, EZ_LedPixelColorF f));
EXTERN void  EZ_SetLedString MY_ANSIARGS((EZ_Widget *parent, char *str));
EXTERN void  EZ_ClearLed  MY_ANSIARGS((EZ_Widget *led));
EXTERN void  EZ_SetLedForeground  MY_ANSIARGS((EZ_Widget *led, char *clr));
EXTERN void  EZ_SetLedBackground  MY_ANSIARGS((EZ_Widget *led, char *clr));
EXTERN void  EZ_SetLedOffPixelColor  MY_ANSIARGS((EZ_Widget *led, char *clr));
EXTERN void  EZ_SetLedFont  MY_ANSIARGS((EZ_Widget *led, char *font));
EXTERN void  EZ_SetLedWidth  MY_ANSIARGS((EZ_Widget *led, int w));
EXTERN void  EZ_SetLedHeight  MY_ANSIARGS((EZ_Widget *led, int h));
EXTERN void  EZ_SetLedSize    MY_ANSIARGS((EZ_Widget *led, int w, int h));
EXTERN void  EZ_SetLedPixelSize  MY_ANSIARGS((EZ_Widget *led, int s, int t));
EXTERN void  EZ_GetLedGeometry  MY_ANSIARGS((EZ_Widget *led, int *w, int *h, int *pw, int *ph));
EXTERN int   EZ_LedIsFree  MY_ANSIARGS((EZ_Widget *led));
EXTERN char  *EZ_GetLedString MY_ANSIARGS((EZ_Widget *led));

EXTERN EZ_Widget  *EZ_CreateLed MY_ANSIARGS((EZ_Widget *parent));
EXTERN void        EZ_DrawWidgetLed MY_ANSIARGS((EZ_Widget *widget));
EXTERN void        EZ_ComputeWidgetLedSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void        EZ_FreeWidgetLedData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void        EZ_LedEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void  EZ_OnOffLedPixel MY_ANSIARGS((EZ_Widget *lcd, int onff, int x, int y, char *clr));
EXTERN void  EZ_LedLoadBitmap MY_ANSIARGS((EZ_Widget *wptr, unsigned char *data,
                                           int w, int h, int x0, int y0));
EXTERN void  EZ_LedLoadPixelMatrix MY_ANSIARGS((EZ_Widget *wptr, unsigned long *data,
                                                int w, int h, int x0, int y0, int d));
EXTERN void  EZ_LedLoadOnOffMatrix MY_ANSIARGS((EZ_Widget *wptr, char *data,
                                         int w, int h, int x0, int y0, int d));

EXTERN int    EZ_GetLedSelection  MY_ANSIARGS((EZ_Widget *lcd, int *x, int *y));
EXTERN void   EZ_LedWindow2Led    MY_ANSIARGS((EZ_Widget *lcd, int wx, int wy, int *lx, int *ly));
EXTERN Pixmap EZ_MakeColorBall MY_ANSIARGS((unsigned long pv, int size));
EXTERN void   EZ_DestroyColorBalls MY_ANSIARGS((void));
#endif

#ifndef _EZ_WIDGET_SMETER_C_
EXTERN EZ_Widget   *EZ_CreateStatusMeter  MY_ANSIARGS((EZ_Widget *parent)); 
EXTERN void         EZ_SetStatusMeterValue  MY_ANSIARGS((EZ_Widget *widget, float value));
EXTERN void         EZ_SetStatusMeterRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));
EXTERN float        EZ_GetStatusMeterValue  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void         EZ_StopStatusMeter MY_ANSIARGS((EZ_Widget *widget));
EXTERN void         EZ_StartStatusMeter MY_ANSIARGS((EZ_Widget *widget));
EXTERN void         EZ_SetStatusMeterForeground MY_ANSIARGS((EZ_Widget *widget, char *clr));
EXTERN void         EZ_SetStatusMeterBackground MY_ANSIARGS((EZ_Widget *widget, char *clr));

EXTERN void         EZ_SetStatusMeterForegroundN MY_ANSIARGS((EZ_Widget *widget, int idx, char *clr));
EXTERN void         EZ_SetStatusMeterValueN  MY_ANSIARGS((EZ_Widget *widget, int idx, float value));
EXTERN void         EZ_SetStatusMeterRangeN MY_ANSIARGS((EZ_Widget *widget, int idx, float f, float t));
EXTERN float        EZ_GetStatusMeterValueN  MY_ANSIARGS((EZ_Widget *widget, int idx));
#endif


#ifndef _EZ_WIDGET_MBAR_C_
EXTERN EZ_Widget        *EZ_CreateMBar MY_ANSIARGS((EZ_Widget *parent));
EXTERN void              EZ_DrawWidgetMBar MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_ComputeWidgetMBarSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void              EZ_FreeWidgetMBarData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void              EZ_MBarEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

EXTERN EZ_Widget        *EZ_MenuBarAddItemAndMenu MY_ANSIARGS((EZ_Widget *widget, char *label, int ul, EZ_Widget *menu));
EXTERN void              EZ_MenuBarSetItemMenu MY_ANSIARGS((EZ_Widget *widget, char *label, EZ_Widget *menu));
EXTERN EZ_Widget         *EZ_MenuBarFindItemAtXY MY_ANSIARGS(( EZ_Widget *widget, int x, int y));
EXTERN EZ_Widget         *EZ_MenuBarFindItemAndMenuAtXY MY_ANSIARGS(( EZ_Widget *widget, int x, int y, EZ_Widget **ret));
EXTERN EZ_Widget         *EZ_MenuBarFindItem MY_ANSIARGS(( EZ_Widget *widget, char *name));
EXTERN EZ_Widget         *EZ_MenuBarFindItemAndMenu MY_ANSIARGS(( EZ_Widget *widget, char *name, EZ_Widget **ret));
EXTERN void              EZ_MenuBarDeleteItem MY_ANSIARGS((EZ_Widget *widget, char *label));
EXTERN void              EZ_MenuBarDeleteItemAndMenu MY_ANSIARGS((EZ_Widget *widget, char *label));
EXTERN void              EZ_MenuBarInsertItemAndMenu MY_ANSIARGS((EZ_Widget *mbar, char *label, int ul, EZ_Widget *menu,
								  char *ref, int where));
#endif

#ifndef _EZ_WIDGET_ITERM_C_
EXTERN EZ_Widget *EZ_CreateITerm MY_ANSIARGS((EZ_Widget *parent));
EXTERN void       EZ_update_iterm_scrollbars MY_ANSIARGS((EZ_Widget *widget));
#endif

#ifndef _EZ_WIDGET_TERM_C_
EXTERN EZ_Widget *EZ_CreateTerm MY_ANSIARGS((EZ_Widget *parent, int vscroll));
#endif

#ifndef _EZ_ITERM_COMMAND_C_
EXTERN void EZ_TermStartCommand MY_ANSIARGS((EZ_Widget *ezterm, char **args));
EXTERN void EZ_TermStopCommand MY_ANSIARGS((EZ_Widget *ezterm, int sig));
EXTERN void EZ_TermSetExitHandler MY_ANSIARGS((EZ_Widget *ezterm, EZ_CallBack cb, void *data));
EXTERN void EZ_ITermReverseVideo  MY_ANSIARGS((EZ_Widget *widget, int mode));
EXTERN void EZ_SetITermColors MY_ANSIARGS((EZ_Widget *widget, int i, unsigned long pv));
EXTERN unsigned long EZ_GetITermColors MY_ANSIARGS((EZ_Widget *widget, int i));
#endif

#ifndef _EZ_WIDGET_RULER_C_
EXTERN EZ_Widget   *EZ_CreateRuler  MY_ANSIARGS((EZ_Widget *parent, int type));
EXTERN void         EZ_WidgetNotifyRuler  MY_ANSIARGS((EZ_Widget *ruler, int x, int y, 
                                                EZ_Widget *fromWgt));
EXTERN void         EZ_XWindowNotifyRuler  MY_ANSIARGS((EZ_Widget *ruler, int x, int y, 
                                                 Window fromWin));
EXTERN void         EZ_WidgetMarkRuler  MY_ANSIARGS((EZ_Widget *ruler, int x, int y, 
                                                EZ_Widget *fromWgt));
EXTERN void         EZ_XWindowMarkRuler  MY_ANSIARGS((EZ_Widget *ruler, int x, int y, 
                                                 Window fromWin));
EXTERN void         EZ_SetRulerOffset MY_ANSIARGS((EZ_Widget *ruler, int offset));
EXTERN void         EZ_SetupRuler MY_ANSIARGS((EZ_Widget *ruler, int tunit, int tlen,
                                               int nscale, int offset));
EXTERN int          *EZ_GetRulerMarks  MY_ANSIARGS((EZ_Widget *ruler));
#endif

#ifndef _EZ_WIDGET_SCROLL_BUTTON_C_
EXTERN int         EZ_GetScrollButtonScrollCount MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget   *EZ_CreateScrollButton MY_ANSIARGS((EZ_Widget *parent, int type));
#endif

#ifndef _EZ_WIDGET_CONFIGURE_C_
EXTERN int  EZ_SetWidgetAttributes MY_ANSIARGS((EZ_Widget *widget, int opt, 
						int i1, int i2, 
						void *p1, void *p2,
						unsigned long ul1, unsigned long ul2,
						float f1, float f2,
						int i3, int i4));
EXTERN void  EZ_ConfigureError MY_ANSIARGS((EZ_Widget *widget, int option, char *msg));
EXTERN int   EZ_SetWidgetPixmap MY_ANSIARGS((EZ_Widget *widget, EZ_Bitmap *bitmap, int flag));
EXTERN int   EZ_GetWidgetAttributes MY_ANSIARGS((EZ_Widget *widget, int option,  int oparam,
                                                 EZ_Value *vreturn));
EXTERN char *EZ_GetOptionSymbolicName MY_ANSIARGS((int i));
EXTERN int  EZ_GetWidgetAttribute MY_ANSIARGS((EZ_Widget *widget, int option,  int oparam,
                                               EZ_Value *vreturn));
#endif

#ifndef _EZ_WIDGET_SPIN_BUTTON_C_
EXTERN EZ_Widget  *EZ_CreateSpinButton   MY_ANSIARGS((EZ_Widget *parent));
EXTERN char       *EZ_GetSpinButtonValue MY_ANSIARGS((EZ_Widget *spin));
EXTERN void        EZ_GetSpinButtonComponents MY_ANSIARGS((EZ_Widget *spin,
                                                    EZ_Widget **entry,
                                                    EZ_Widget **sbtn));
EXTERN void EZ_SetupSpinButton  MY_ANSIARGS((EZ_Widget *spin, char *value,
                                      char *(func) MY_ANSIARGS((int, int, void *)),
                                      void *cdata));
#endif

#ifndef _EZ_WIDGET_LOCATOR_C_
EXTERN EZ_Widget *EZ_CreateLocator   MY_ANSIARGS((EZ_Widget *parent));
EXTERN void      EZ_GetLocatorXY    MY_ANSIARGS((EZ_Widget *locator, float *x, float *y));
EXTERN void      EZ_SetLocatorXY    MY_ANSIARGS((EZ_Widget *locator, float x, float y));
#endif

#ifndef _EZ_WIDGET_DIAL_C_
EXTERN EZ_Widget   *EZ_CreateDial MY_ANSIARGS((EZ_Widget *parent));
EXTERN float        EZ_GetDialValue MY_ANSIARGS((EZ_Widget *dial));
EXTERN void         EZ_SetDialValue  MY_ANSIARGS((EZ_Widget *dial, float value));
#endif


#ifndef _EZ_WIDGET_GBAR_C_
EXTERN EZ_Widget   *EZ_CreateGradientBar  MY_ANSIARGS((EZ_Widget *parent)); 
EXTERN void         EZ_SetGradientBarValue  MY_ANSIARGS((EZ_Widget *widget, float value));
EXTERN float        EZ_GetGradientBarValue  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void         EZ_SetGradientBarRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));
EXTERN void         EZ_SetGradientBarColors MY_ANSIARGS((EZ_Widget *widget, char **clrs, int cnt));
EXTERN void         EZ_GetGradientBarColor  MY_ANSIARGS((EZ_Widget *widget, int *r, int *b, int *g));
EXTERN Pixmap EZ_MakeGradientPixmap MY_ANSIARGS((int n, float x0, float x1,float y0, float y1,
                                                 float vts[][2], float rgbs[][3], int kind));
#endif

#ifndef _EZ_WIDGET_HISTOGRAM_C_
EXTERN EZ_Widget   *EZ_CreateHistogram  MY_ANSIARGS((EZ_Widget *parent)); 
EXTERN void         EZ_SetHistogramRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));
EXTERN void         EZ_SetHistogramValues  MY_ANSIARGS((EZ_Widget *widget, float *values, int n));
EXTERN void         EZ_SetHistogramValue  MY_ANSIARGS((EZ_Widget *widget, float value));
EXTERN float        EZ_GetHistogramValue  MY_ANSIARGS((EZ_Widget *widget, int n));
EXTERN int          EZ_GetHistogramSelection  MY_ANSIARGS((EZ_Widget *widget, float *vret));
EXTERN void         EZ_SetHistogramColors MY_ANSIARGS((EZ_Widget *widget, unsigned long *clrs,
                                                       int cnt));
#endif

#ifndef _EZ_WIDGET_SEPARATOR_C_
EXTERN EZ_Widget       *EZ_CreateSeparator MY_ANSIARGS((EZ_Widget *parent));
#endif

#ifndef _EZ_WIDGET_SSHEET_C_
EXTERN EZ_Widget *EZ_CreateSpreadSheet MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_SSheetGetSizes MY_ANSIARGS((EZ_Widget *widget,int *nrow, int *ncol));
EXTERN void EZ_SSheetSetFirstCell MY_ANSIARGS((EZ_Widget *widget,int row, int col));
EXTERN void EZ_SSheetSetTextProperty MY_ANSIARGS((EZ_Widget *widget, TextProp *prop));
EXTERN void EZ_SSheetSetHScrollbar  MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *hscroll));
EXTERN void EZ_SSheetSetVScrollbar  MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *vscroll));

EXTERN void EZ_SSheetHideColumns   MY_ANSIARGS((EZ_Widget *widget, int r1, int r2));
EXTERN void EZ_SSheetUnhideColumns   MY_ANSIARGS((EZ_Widget *widget, int r1, int r2));
EXTERN void EZ_SSheetHideRows   MY_ANSIARGS((EZ_Widget *widget, int r1, int r2));
EXTERN void EZ_SSheetUnhideRows   MY_ANSIARGS((EZ_Widget *widget, int r1, int r2));

EXTERN void EZ_SSheetSwapRow     MY_ANSIARGS((EZ_Widget *widget, int r1, int r2));
EXTERN void EZ_SSheetSwapColumn  MY_ANSIARGS((EZ_Widget *widget, int c1, int c2));
EXTERN void EZ_SSheetSwapCell    MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2));
EXTERN void EZ_SSheetSortRegion MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, int cc,
                                             int (*cmp)MY_ANSIARGS((CONST void *, CONST void *)) ));
EXTERN void EZ_SSheetSortRegionR MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, int rr,
                                      int (*cmp)MY_ANSIARGS((CONST void *, CONST void *)) ));
EXTERN void EZ_SSheetSortColumns MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, 
                                      int (*cmp)MY_ANSIARGS((CONST void *, CONST void *)) ));
EXTERN void EZ_SSheetSortRows MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, 
                                      int (*cmp)MY_ANSIARGS((CONST void *, CONST void *)) ));

EXTERN int  EZ_SSheetFindCell MY_ANSIARGS((EZ_Widget *widget, char *str, int *r, int *c));
EXTERN void EZ_SSheetValidateCharts MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2));
EXTERN EZ_Item * EZ_SSheetGetActiveChart MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Item **EZ_SSheetGetAllCharts MY_ANSIARGS((EZ_Widget *widget, int *cnt));
EXTERN void  EZ_SSheetSetTabChar MY_ANSIARGS((EZ_Widget *widget, char tab, char newline));
EXTERN int   EZ_SSheetFindCursorCell MY_ANSIARGS((EZ_Widget *widget, int x, int y, int *cx, int *cy));
EXTERN void  EZ_SSheetHighlightRegion MY_ANSIARGS((EZ_Widget *widget, int x1, int y1, int x2, int y2));

EXTERN char  *EZ_SSheetGetCellWithProperty MY_ANSIARGS((EZ_Widget *wt, int row, int col, TextProp ***prop));
EXTERN void  EZ_SSheetGetActiveCellWithProperty MY_ANSIARGS((EZ_Widget *wt, int *row, int *col, char **str, TextProp ***prop));

EXTERN void  EZ_SSheetGetActiveCell MY_ANSIARGS((EZ_Widget *wt, int *row, int *col, char **str));
EXTERN char  *EZ_SSheetGetCell MY_ANSIARGS((EZ_Widget *wt, int row, int col));
EXTERN void  EZ_SSheetSetActiveCell MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN void  EZ_SSheetDrawCell MY_ANSIARGS((EZ_Widget *wt, int row, int col,  Drawable d, 
                                     int x, int y, int w, int h));
EXTERN int  EZ_SSheetGetHighlightedRegion  MY_ANSIARGS((EZ_Widget *wt, int *r1, int *c1, int *r2, int *c2));
EXTERN char  *EZ_SSheetGetRegionString MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2));
EXTERN char  *EZ_SSheetGetRegionStringWithProperty  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2,
                                                          TextProp ***prop));
EXTERN void  EZ_SSheetInsertRegionString MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, char *str));
EXTERN void  EZ_SSheetInsertRegionStringWithProperty MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, char *str,
							   TextProp **tp));
EXTERN char  **EZ_SSheetGetRegionItems  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2));
EXTERN char  **EZ_SSheetGetRegionItemsWithProperty  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2,
							  TextProp ****tprop));
EXTERN void  EZ_SSheetInsertRegionItems  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2, char **items));
EXTERN void  EZ_SSheetInsertRegionItemsWithProperty  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2, 
							   char **items, TextProp ***tprop));
EXTERN void  EZ_SSheetClearRegion MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2)); 

EXTERN void  EZ_SSheetCopyRegion MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2, int r0, int c0, int delete));
EXTERN void  EZ_SSheetInsertItem MY_ANSIARGS((EZ_Widget *widget, int row, int col, char *str));
EXTERN void  EZ_SSheetInsertItemWithProperty MY_ANSIARGS((EZ_Widget *widget, int row, int col, char *str,
						   TextProp **p));
EXTERN void  EZ_SSheetSetRegionJustification MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, int just));
EXTERN void  EZ_SSheetSetRegionProperty MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, TextProp *p));

EXTERN char  *EZ_SSheetIndexToString MY_ANSIARGS((int col, int row, char *str));
EXTERN void  EZ_SSheetStringToIndex MY_ANSIARGS(( char *str, int *col, int *row));

EXTERN EZ_Item *EZ_SSheetMakeChart MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2,
                                         EZ_SChartHandle handle, int kind, void *data, int cmode));
EXTERN void     EZ_SSheetDeleteChart MY_ANSIARGS((EZ_Widget *widget, EZ_Item *chart));

EXTERN int   EZ_SSheetRedrawCharts MY_ANSIARGS((EZ_Widget *wptr, Drawable drawable));

EXTERN int   EZ_SSheetSelectCellWork  MY_ANSIARGS((EZ_Widget *widget, Drawable d, int x, int y, int flag));
EXTERN void  EZ_SSheetRedrawRegion MY_ANSIARGS((EZ_Widget *widget, Drawable d, int x, int y, 
					 int x1, int y1, int flag));
EXTERN void  EZ_MarkSSheetUsedTextProperties MY_ANSIARGS((EZ_Widget *widget));
EXTERN void  EZ_SSheetPasteSelection MY_ANSIARGS((EZ_Widget *widget));
EXTERN void  EZ_DoScaleItem MY_ANSIARGS(( EZ_Item *im, int x, int y,
                                            int cr, int x0, int y0, int w, int h));
EXTERN int  EZ_MatchWord MY_ANSIARGS((char *buf, char *word, int blen, int wlen, int *pos_ret));
#endif

#ifndef _EZ_WIDGET_THUM_WHEEL_C_
EXTERN EZ_Widget  *EZ_CreateThumbWheel MY_ANSIARGS((EZ_Widget *parent));
EXTERN float       EZ_GetThumbWheelValue MY_ANSIARGS((EZ_Widget *widget));
EXTERN void        EZ_SetThumbWheelValue MY_ANSIARGS((EZ_Widget *widget, float value));
EXTERN void        EZ_SetThumbWheelRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));
#endif

#ifndef _EZ_BMP_C_
EXTERN int EZ_ReadBMPToPixmap MY_ANSIARGS((char *filename, int *width_return, int *height_return,
					   Pixmap *pixmap_return));
EXTERN int EZ_ReadBMPToRGB MY_ANSIARGS((char *filename, int *width_return, int *height_return,
					   unsigned char **rgb_return));
#endif

#ifndef _EZ_JPEG_C_
EXTERN int EZ_ReadJpegToPixmap MY_ANSIARGS((
					    char     *file,
					    int      *width_return,
					    int       *height_return,
					    Pixmap   *pixmap_return));
EXTERN int EZ_ReadJpegToRGB MY_ANSIARGS((
					    char     *file,
					    int      *width_return,
					    int       *height_return,
					    unsigned char   **rgb_return));
#endif

#ifndef _EZ_PNG_C_
EXTERN int EZ_ReadPngToPixmap MY_ANSIARGS((
					    char     *file,
					    int      *width_return,
					    int       *height_return,
					    Pixmap   *pixmap_return));
EXTERN int EZ_ReadPngToRGB MY_ANSIARGS((
					    char     *file,
					    int      *width_return,
					    int       *height_return,
                                            unsigned char   **rgb_return));
#endif

#ifndef _EZ_TIFF_C_
EXTERN int EZ_ReadTiffToPixmap MY_ANSIARGS((
					    char     *file,
					    int      *width_return,
					    int       *height_return,
					    Pixmap   *pixmap_return));
EXTERN int EZ_ReadTiffToRGB MY_ANSIARGS((
					    char     *file,
					    int      *width_return,
					    int       *height_return,
                                            unsigned char   **rgb_return));
#endif



#ifndef _EZ_GM_C_
EXTERN EZ_GManager *EZ_AllocateGManager MY_ANSIARGS((int type));
EXTERN void         EZ_DestroyGManager  MY_ANSIARGS((EZ_GManager *gm));
#endif

#ifndef _EZ_CURSOR_C_
EXTERN  void                  EZ_InitializeCursors MY_ANSIARGS((void));
EXTERN  void                  EZ_FreeAllCursors MY_ANSIARGS((void));
EXTERN  Cursor                EZ_GetCursor MY_ANSIARGS((int type));
EXTERN  Cursor                EZ_GetCursorByName MY_ANSIARGS((char *name));
EXTERN  char                  *EZ_GetCursorName MY_ANSIARGS((Cursor csr));
EXTERN  void                  EZ_WaitCursor MY_ANSIARGS((EZ_Widget *widget, Cursor cursor));
EXTERN  void                  EZ_BusyCursor MY_ANSIARGS((EZ_Widget *widget, Cursor cursor, int timeout));
EXTERN  void                  EZ_NormalCursor  MY_ANSIARGS((EZ_Widget *widget));
#endif

#ifndef _EZ_ITEM_HASH_C_
EXTERN void      EZ_InsertItemToHashTable MY_ANSIARGS((EZ_Item *item));
EXTERN EZ_Item   *EZ_LookupItemFromHT MY_ANSIARGS((EZ_Item *item));
EXTERN EZ_Item   *EZ_ItemExist MY_ANSIARGS((EZ_Item *item));
EXTERN void      EZ_RemoveItemFromHT MY_ANSIARGS((EZ_Item *item));
EXTERN void      EZ_DestroyAllItems  MY_ANSIARGS((void));
EXTERN void      EZ_EditItemResources MY_ANSIARGS((XrmDatabase db));
EXTERN void      EZ_SetItemDefaultFontAll MY_ANSIARGS((XFontStruct *old, XFontStruct *n));
EXTERN void      EZ_ListAllWidgetItems MY_ANSIARGS((EZ_Widget **widget_ret, int *cnt_ret));
EXTERN void      EZ_SetItemDefaultFgAll MY_ANSIARGS((unsigned long old, unsigned long newfg));
#endif

#ifndef _EZ_RESOUCE_C_
EXTERN void    EZ_PrintStandardOptions MY_ANSIARGS((void));
EXTERN void    EZ_SetApplicationName MY_ANSIARGS((char *name));
EXTERN char    *EZ_GetApplicationName MY_ANSIARGS((void));
EXTERN void    EZ_DestroyXrmDB          MY_ANSIARGS((void));
EXTERN XrmDatabase  *EZ_GetResourceDatabase MY_ANSIARGS(( void ));
EXTERN int    EZ_InitXrmAndX11 MY_ANSIARGS((char *className,
					  XrmOptionDescRec *optstable,  int nopts,
					  int *ac, char **av, char *fbkstr, int initgl));
EXTERN char *EZ_GetInstanceName      MY_ANSIARGS((void));
EXTERN char *EZ_GetClassName         MY_ANSIARGS((void));
EXTERN void  EZ_SetWidgetCINames  MY_ANSIARGS((EZ_Widget *w, char *cn, char *in));
EXTERN void  EZ_GetWidgetCINames  MY_ANSIARGS((EZ_Widget *w, char **cn, char **in));
EXTERN void  EZ_SetDefaultFontsFromResources MY_ANSIARGS((void));
EXTERN void  EZ_SetDefaultAttributesFromResources MY_ANSIARGS((void));

EXTERN int   EZ_InstallSymbolicInt   MY_ANSIARGS((char *name, int value));
EXTERN int   EZ_RetrieveSymbolicInt  MY_ANSIARGS((char *name, int *value_ret));

EXTERN int   EZ_InstallSymbolicFloat   MY_ANSIARGS((char *name, float value));
EXTERN int   EZ_RetrieveSymbolicFloat  MY_ANSIARGS((char *name, float *value_ret));

EXTERN int   EZ_InstallSymbolicPointer   MY_ANSIARGS((char *name, void *value));
EXTERN int   EZ_RetrieveSymbolicPointer  MY_ANSIARGS((char *name, void **value_ret));

EXTERN int   EZ_InstallSymbolicULong   MY_ANSIARGS((char *name, unsigned long value));
EXTERN int   EZ_RetrieveSymbolicULong  MY_ANSIARGS((char *name, unsigned long *value_ret));

EXTERN int       EZ_InstallSymbol  MY_ANSIARGS((char *name, EZ_Value value));
EXTERN EZ_Value  *EZ_RetrieveSymbol MY_ANSIARGS((char *name));

EXTERN int EZ_GetApplicationResource MY_ANSIARGS((char *rname, char **v_ret));
EXTERN int EZ_GetWidgetResource MY_ANSIARGS((EZ_Widget *, char *rname, char **v_ret));
EXTERN int EZ_GetItemResource   MY_ANSIARGS((EZ_Item *, char *rname, char **v_ret));
EXTERN int   EZ_GetOptionCINames MY_ANSIARGS((int i, char **cn, char **in));
#endif

#ifndef _EZ_RESOUCEGET_C_
EXTERN int   EZ_GetAndUseWidgetResources   MY_ANSIARGS((EZ_Widget *widget));
EXTERN int  EZ_GetAndUseWidgetResourcesWithDB MY_ANSIARGS((EZ_Widget *widget, XrmDatabase db));

EXTERN int   EZ_GetAndUseItemResources     MY_ANSIARGS((EZ_Item *item));
EXTERN int  EZ_GetAndUseItemResourcesWithDB     MY_ANSIARGS((EZ_Item *item,  XrmDatabase db));

EXTERN int   EZ_GetAndUseWidgetResources1   MY_ANSIARGS((EZ_Widget *widget));
EXTERN int  EZ_GetAndUseWidgetResources1WithDB MY_ANSIARGS((EZ_Widget *widget, XrmDatabase db));

EXTERN int  EZ_DecodeApplicationResources   MY_ANSIARGS((void));
EXTERN int  EZ_DecodeApplicationResourcesWithDB  MY_ANSIARGS((XrmDatabase db));

EXTERN void EZ_RegisterApplicationResources MY_ANSIARGS((int n, EZ_ResourceSpec *spec, 
                           int (*configure) MY_ANSIARGS((void *p, int opt, EZ_Value *values))
                                                         ));
EXTERN int  EZ_SetOneWidgetResource MY_ANSIARGS((EZ_Widget *widget, char *resname, char *resvalue));
EXTERN int  EZ_ParseWidgetResourceString MY_ANSIARGS((EZ_Widget *widget, char *resString));
EXTERN int  EZ_ParseItemResourceString MY_ANSIARGS((EZ_Item *item, char *resString));
#endif

#ifndef _EZ_RESOURCE_EDIT_C_
EXTERN int EZ_HandleResourceEditorMessage MY_ANSIARGS((EZ_Message *msg));
EXTERN char         *EZ_PrintOneWidgetTree  MY_ANSIARGS((EZ_Widget *widget));
EXTERN char         *EZ_PrintWidgetTree  MY_ANSIARGS((void));
EXTERN EZ_TreeNode  *EZ_ParseWidgetTree  MY_ANSIARGS((char *str));
EXTERN EZ_TreeNode  *EZ_ParseWidgetTreeA  MY_ANSIARGS((char *str, int which));
EXTERN Window        EZ_SelectWindowWithButtonClick MY_ANSIARGS((void));
EXTERN char         *EZ_PrintApplicationResources MY_ANSIARGS((void));
EXTERN char         *EZ_PrintWidgetResources MY_ANSIARGS((EZ_Widget *widget));
EXTERN char         *EZ_PrintWidgetResourcesAll MY_ANSIARGS((EZ_Widget *widget));
EXTERN char         *EZ_PrintSetApplicationResources MY_ANSIARGS((char *msg, int len));
EXTERN char         *EZ_PrintSetWidgetResources MY_ANSIARGS((EZ_Widget *widget, char *msg, int len));
EXTERN char         *EZ_PrintSetWidgetResourcesN MY_ANSIARGS((char *msg, int len));
EXTERN EZ_Item      **EZ_ParseResourcesTable MY_ANSIARGS((char *str, int *cntRet));
EXTERN void         EZ_GetWidgetFullPath MY_ANSIARGS((EZ_Widget *widget, char **n, char **c));
EXTERN void         EZ_GetWidgetFullPathQuarks MY_ANSIARGS((EZ_Widget *widget, XrmQuark **n, XrmQuark **c));
EXTERN void         EZ_RecordResourceString MY_ANSIARGS((char **dest, char *src));
EXTERN int          EZ_CompareItem MY_ANSIARGS((void *a, void *b));
#endif

#ifndef _EZ_GARBAGE_C_
EXTERN void EZ_InitGarbageList MY_ANSIARGS((void));
EXTERN void EZ_InsertObjToGarbageList MY_ANSIARGS((int type, void *obk));
#endif

#ifndef _EZ_MESSAGE_C_
EXTERN void  EZ_SetDefaultMessageHandler MY_ANSIARGS((EZ_MessageHandler handler, void *data));
EXTERN void  EZ_AddMessageHandler MY_ANSIARGS((Atom mtype, int mId,  void *data,
					       EZ_MessageHandler handler,
                                               int timeout, EZ_CallBack tcb, void *cdata));
EXTERN void  EZ_DeleteMessageHandler MY_ANSIARGS((Atom mtype, int mId,  void *data,
						  EZ_MessageHandler handler));

EXTERN void  EZ_SendMessage MY_ANSIARGS((Atom mtype, char *message, int length, int msgid,
					 Window recipient, int replayId, int isReply));
EXTERN void  EZ_BroadcastMessage MY_ANSIARGS((Atom mtype, char *message, int length, int msgid,
					      int replyId));

EXTERN void  EZ_ReplyMessage  MY_ANSIARGS((EZ_Message *fromsg, char *reply, int replylength));
EXTERN void  EZ_ReplyMessageR MY_ANSIARGS((EZ_Message *fromsg, char *reply, int len, 
					   int repltId));

EXTERN void  EZ_HandleGeneralMessage MY_ANSIARGS((char *msg, int length));
EXTERN void  EZ_InitializeMessageHandlers MY_ANSIARGS((void));
#endif

#ifndef _EZ_WM_C_
EXTERN void EZ_SetWMHintsAndSizeHints MY_ANSIARGS((EZ_Widget *widget,int flag));
EXTERN void EZ_SetWMSizeHints MY_ANSIARGS((EZ_Widget *widget,int flag));
EXTERN void EZ_SetWMHintForWindow MY_ANSIARGS((Window win, int x, int y, int w, int h));
EXTERN void EZ_DeleteWidgetWMHints MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_SetWidgetWMHints MY_ANSIARGS((EZ_Widget *wt, int opt, int i, int j, void *p));
EXTERN int  EZ_WidgetStartsAsIcon MY_ANSIARGS((EZ_Widget *w));
#endif

#ifndef _EZ_DRAG_XPM_C_
EXTERN void EZ_SetupDragablePixmap MY_ANSIARGS((int flag));
EXTERN void EZ_OverlayDragablePixmap MY_ANSIARGS((EZ_Widget *wgt, Drawable d,
					   int x, int y, int w, int h));
#endif

#ifndef _EZ_RTEXT_C_
EXTERN EZ_Bitmap *EZ_CreateRotateTextPixmap MY_ANSIARGS((char *str, int fId, int llen, int jt, int angle));
EXTERN EZ_Bitmap *EZ_CreateRotateTextPixmapU MY_ANSIARGS((char *str, int fId, int llen, int jt, int ul, int angle));
#endif

#ifndef _EZ_XDEBUG_C_
EXTERN  Pixmap MyXCreatePixmap MY_ANSIARGS((Display *d, Drawable dra, int w, int h, int depth));
EXTERN  XImage *MyXCreateImage  MY_ANSIARGS((Display *d, Visual *v, int depth, int format,
                                            int offset, char *data, int width, int height, int bbp,
                                            int bpl));
EXTERN  void  MyXDestroyImage  MY_ANSIARGS((XImage *image));
EXTERN  void  MyXFreePixmap  MY_ANSIARGS((Display *dpy, Pixmap pix));
#endif

/* glob.c and tilde.c */
EXTERN char       **EZ_GlobFileName  MY_ANSIARGS((char *name));
EXTERN char       **EZ_GlobFiles  MY_ANSIARGS((char *name, int showHidden));
EXTERN int        EZ_GlobHiddenFiles MY_ANSIARGS((int yesNo ));
EXTERN char       *EZ_TildeExpand MY_ANSIARGS((char *name));

/*
 * Var arg things
 */
EXTERN EZ_TextProp *EZ_GetTextProperty  MY_VARARGS_D(int, name);
EXTERN void        EZ_ConfigureWidget   MY_VARARGS_D(void, *args);
EXTERN void        EZ_ConfigureItem     MY_VARARGS_D(void, *args);
EXTERN EZ_Widget  *EZ_CreateSimpleMenu  MY_VARARGS_D(char, *args);
EXTERN EZ_Widget  *EZ_CreateWidget      MY_VARARGS_D(int,  the_args);
EXTERN EZ_Widget  *EZ_CreateWidgetXrm   MY_VARARGS_D(int, args);
EXTERN EZ_Item    *EZ_CreateItem        MY_VARARGS_D(int,  the_args);
EXTERN EZ_Item    *EZ_CreateItemXrm     MY_VARARGS_D(int, args);
/* Xrm related routine */
#endif

