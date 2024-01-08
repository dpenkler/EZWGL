/*********************************************************************
 *
 *         EZWGL, the EZ Widget and Graphics Library
 *
 *             Copyright (C) 1996,  Maorong Zou
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
 *  April 1997.  Release Version 1.2
 *  November 1997.  Release Version 1.3
 *  November 1998.  Release Version 1.4
 *  November 1999.  Release Version 1.5
 */
/*******************************************************************
 ***                                                             ***
 ***      The EZ Widget and Graphics Library Header File         ***
 ***                                                             ***
 ******************************************************************/
#ifndef _EZ_H_
#define	_EZ_H_

/*******************************************************************
 ***                                                             ***
 ***                System Headers                               ***
 ***                                                             ***
 ******************************************************************/
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef HAVE_STRING_H
#  include <string.h>
#else 
#  include <strings.h>
#endif

/*******************************************************************
 ***                                                             ***
 ***                     Args                                    ***
 ***                                                             ***
 ******************************************************************/

/*
 * Var Args
 */
#if defined(__STDC__) || defined(HAS_STDARG)
#include <stdarg.h>
#define MY_VARARGS(type, name) (type name, ...)
#define MY_VARARGS_D(type, name) (type name, ...)
#define MY_VARARGS_DECL    /*nothing*/
#define MY_VARARGS_START(type, name, list) (va_start(list, name), name)
#else
#include <varargs.h>
#define MY_VARARGS(type, name) (va_alist)
#define MY_VARARGS_D(type, name) ()
#define MY_VARARGS_DECL    va_dcl
#define MY_VARARGS_START(type,name,list)  (va_start(list), va_arg(list, type))
#endif

/*
 * ANSI Args
 */
#undef MY_ANSIARGS
#undef CONST
#if defined (__cplusplus) || defined(USE_PROTOTYPE) || (defined(__STDC__) && !defined(NO_PROTOTYPE))
#define MY_ANSIARGS(x)   x
#define CONST const
#else
#define MY_ANSIARGS(x)   ()
#define CONST 
#endif

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

#ifndef  EZ_ArraySize
#define  EZ_ArraySize(_ary_) (sizeof(_ary_)/sizeof((_ary_)[0]))
#endif

/*******************************************************************
 ***                                                             ***
 ***                     Types                                   ***
 ***                                                             ***
 ******************************************************************/

/*
 * Internal data structures are effectively hidden from these typedefs.
 */
typedef void              EZ_Widget;
typedef void              EZ_LabelPixmap;
typedef void              EZ_TextProperty;
typedef void              EZ_TextProp;
typedef void              EZ_Item;
typedef void              EZ_TreeNode;
typedef void              EZ_Timer;
typedef void              EZ_Bitmap;
typedef void              EZ_Pixmap;
typedef void              EZ_Input;
typedef void              EZ_GLImage;
typedef void              EZ_FigPiece;

typedef float             EZ_Vector[4];
typedef float             EZ_Vector3[3];
typedef float             EZ_Matrix[4][4];
typedef float             EZ_Material;
typedef float             EZ_LightSrc;
typedef float             EZ_LightModel;

/*
 * func types
 */
typedef  void (*EZ_CallBack)      MY_ANSIARGS((void *object, void *data));
typedef  void (*EZ_Callback)      MY_ANSIARGS((void *object, void *data));
typedef  void (*EZ_InputCallback) MY_ANSIARGS((void *object, void *data, int fd, int mask));
typedef  void (*EZ_InputCallBack) MY_ANSIARGS((void *object, void *data, int fd, int mask));
typedef  void (*EZ_EventHandler)  MY_ANSIARGS((void *object, void *data, int etype, XEvent *event));
typedef  void (*EZ_EventFilter)   MY_ANSIARGS(( XEvent *event, void *data));
typedef  void (*EZ_MessageHandler)  MY_ANSIARGS((void *msgIn, void *client_data));
typedef  int  (*EZ_SChartHandle) MY_ANSIARGS((EZ_Widget *ss, void *chart, int r1, int c1, 
                                              int r2, int c2, int kind, void *data));
typedef  void *(*EZ_DirTreeNodeCreator) MY_ANSIARGS((char *path, void *data));

typedef  int  (*EZ_DnDEncoder)    MY_ANSIARGS((void *object, void *data, 
					       char **message,int *length, int *needfree));
typedef  int  (*EZ_DnDDecoder)    MY_ANSIARGS((void *object, void *data, 
					       char *message,int length));

typedef  void (*EZ_WorkAreaGeometryManager)  MY_ANSIARGS((void *GMData, EZ_Item **data, int nitems, 
							  int Rx,int Ry,int Rw, int Rh, int xsp, int ysp,
							  int *w_ret, int *h_ret));

/*****************************************************************
 * XImage
 */
typedef struct {
  XImage *ximage;
  int     shm;
  void   *shminfo;
} EZ_XImage;

/*****************************************************************
 * Resources;
 */
typedef struct {
  int     option;
  char    *iname;
  char    *cname;
  char    *vtypes;
} EZ_ResourceSpec;

/*****************************************************************
 *
 * Used to attach client data to a widget/item. 
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

/*********************************************************************
 *
 *  Used for setting application resource values.
 */
typedef struct EZ_Value_ {
  int  type;  /* see below */
  union {
    char    c;    unsigned char  uc;
    short   s;    unsigned short us;
    int     i;    unsigned int   ui;
    float   f;
    void    *p;
    void    *function;
    char    *str;
    unsigned long l;
  } value;
} EZ_Value;

#ifndef EZ_INT
#define EZ_INT     0
#define EZ_FLOAT   1
#define EZ_POINTER 2
#define EZ_ULONG   3
#define EZ_LONG    4
#define EZ_UCHAR   5
#define EZ_CHAR    6
#define EZ_USHORT  7
#define EZ_SHORT   8
#define EZ_UINT    9
#define EZ_STRING  10
#define EZ_ENUM    11
#define EZ_BOOL    12
#define EZ_FUNCTION 13
#define EZ_FONTNAME 14
#define EZ_FILENAME 15
#define EZ_COLOR    16
#endif

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

/*********************************************************************
 *
 *  Type used in listbox
 */
typedef struct listboxentry_ {
  int   length;
  char  *data;
  unsigned long color;
} EZ_ListBoxEntry;

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

/*********************************************************************/

#ifndef  EZ_ArraySize
#define  EZ_ArraySize(_ary_) (sizeof(_ary_)/sizeof((_ary_)[0]))
#endif

#ifndef EZ_StringToQuark
#define EZ_StringToQuark(_str__)      ((_str__)? XrmStringToQuark(_str__): NULLQUARK)
#define EZ_PermStringToQuark(_str__)  ((_str__)? XrmPermStringToQuark(_str__):NULLQUARK)
#define EZ_QuarkToString(_quk__)   ((_quk__)!=NULLQUARK? XrmQuarkToString(_quk__):"NULL")
#endif


/*******************************************************************
 ***                                                             ***
 ***                Message Info                                 ***
 ***                                                             ***
 ******************************************************************/
typedef struct {
  Atom       messageType;
  int        messageLength;
  char       *message;
  int        messageId;    /* set by the sender, like a transaction serial # */
  int        replyId;
  int        isReply;
  /* src info */
  Window     sender;
  int        senderIsSelf;
} EZ_Message;

/*******************************************************************
 ***                                                             ***
 ***                Global Variables                             ***
 ***                                                             ***
 ******************************************************************/

EXTERN int          EZ_PointerCoordinates[2];   /* coor of pointer rel to the event window  */
EXTERN int          EZ_MouseX;                  /* = EZ_PointerCoordinates[0] - borderWidth */
EXTERN int          EZ_MouseY;                  /* = width - EZ_PointerCoordinates[0] - borderWidth */
EXTERN unsigned int EZ_PressedKey;              /* for key events, the pressed key */
EXTERN unsigned int EZ_ButtonState;             /* states of the buttons */
/* Left, middle right 4th &5th btn, may be re-assigned */
EXTERN unsigned int EZ_Btn1, EZ_Btn2, EZ_Btn3, EZ_Btn4, EZ_Btn5;

EXTERN Atom      EZ_WM_DELETE_WINDOW_ATOM;
EXTERN Atom      EZ_WM_PROTOCOLS_ATOM;
EXTERN Atom      EZ_DND_DRAG_INIT_ATOM;
EXTERN Atom      EZ_DND_DRAG_START_ATOM;
EXTERN Atom      EZ_DND_DRAG_FINISH_ATOM;
EXTERN Atom      EZ_DND_DROP_START_ATOM;
EXTERN Atom      EZ_DND_DROP_FINISH_ATOM;
EXTERN Atom      EZ_MESSAGE_ATOM;
EXTERN Atom      EZ_RESOURCE_EDITOR_ATOM;

/*******************************************************************
 ***                                                             ***
 ***              Status for DND converters                      ***
 ***                                                             ***
 ******************************************************************/
#define EZ_DND_FAILURE            0
#define EZ_DND_SUCCESS            1

#define EZ_NO                     0
#define EZ_YES                    1
#define EZ_NULL                   NULL
#define EZ_TRUE                   True
#define EZ_FALSE                  False

#define EZ_NONE_MASK              0L
#define EZ_READABLE_MASK          (1L<<0)
#define EZ_WRITABLE_MASK          (1L<<1)
#define EZ_EXCEPTION_MASK         (1L<<2)

#define EZ_ROW                    1
#define EZ_COLUMN                 2
#define EZ_ALL_ROWS               4
#define EZ_ALL_COLUMNS            8


#define EZ_ON                     1
#define EZ_OFF                    0

#define EZ_DEFAULT_GEOMETRY_MANAGER   -1  
#define EZ_GRIDBAG_GEOMETRY_MANAGER    0
#define EZ_ROW_GEOMETRY_MANAGER        1
#define EZ_COLUMN_GEOMETRY_MANAGER     2
#define EZ_NULL_GEOMETRY_MANAGER       3

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
 ***                   ResourceEditor Command Codes                  ***
 ***                                                                 ***
 ***********************************************************************/

#define EZ_RE_GET_TREE          1
#define EZ_RE_GET_TREE1         2
#define EZ_RE_GET_RESOURCE      3
#define EZ_RE_SET_RESOURCE      4
#define EZ_RE_SET_RESOURCE1     5
#define EZ_RE_SET_RESOURCE2     6
#define EZ_RE_INSTALL_RESOURCE  7
#define EZ_RE_BLINK_WIDGET      8
#define EZ_RE_WHO_ARE_YOU       9

/*******************************************************************
 ***                                                             ***
 ***              Configuration options                          ***
 ***                                                             ***
 ******************************************************************/
#define EZ_X                      1
#define EZ_Y                      2
#define EZ_WIDTH                  3
#define EZ_HEIGHT                 4
#define EZ_WIDTH_HINT             5
#define EZ_HEIGHT_HINT            6
#define EZ_BORDER_WIDTH           7
#define EZ_BORDERWIDTH            EZ_BORDER_WIDTH
#define EZ_BORDER_TYPE            8
#define EZ_BORDER_STYLE           EZ_BORDER_TYPE
#define EZ_PADX                   9
#define EZ_PADY                   10
#define EZ_IPADX                  11
#define EZ_IPADY                  12
#define EZ_STACKING               13
#define EZ_ORIENTATION            EZ_STACKING
#define EZ_SIDE                   14
#define EZ_LABEL_POSITION         15
#define EZ_LABELPOSITION          EZ_LABEL_POSITION
#define EZ_JUSTIFICATION          16
#define EZ_LABEL_JUSTIFICATION    EZ_JUSTIFICATION
#define EZ_FILL_MODE              17
#define EZ_PROPAGATE              18
#define EZ_CALL_BACK              19
#define EZ_CALLBACK               EZ_CALL_BACK
#define EZ_FOREGROUND             20
#define EZ_BACKGROUND             21
#define EZ_BITMAP_FILE            22
#define EZ_PIXMAP_FILE            EZ_BITMAP_FILE
#define EZ_BITMAP_DATA            23   
#define EZ_PIXMAP_DATA            24
#define EZ_X_PIXMAP               25
#define EZ_LABEL_PIXMAP           26
#define EZ_LABELPIXMAP            EZ_LABEL_PIXMAP
#define EZ_FONT_ID                27
#define EZ_FONT_NAME              28
#define EZ_TEXT_LINE_LENGTH       29
#define EZ_SLIDER_LENGTH          30
#define EZ_SLIDERLENGTH           EZ_SLIDER_LENGTH
#define EZ_SLIDER_WIDTH           31
#define EZ_SLIDERWIDTH            EZ_SLIDER_WIDTH
#define EZ_SLIDER_BORDER_WIDTH    32
#define EZ_SLIDER_BORDERWIDTH     EZ_SLIDER_BORDER_WIDTH
#define EZ_SLIDER_RESOLUTION      33
#define EZ_SLIDER_RANGE           34
#define EZ_INDICATOR_SIZE_ADJUST  35
#define EZ_IMAGE_FILE             36
#define EZ_BUTTON_SHORTCUT        37
#define EZ_SHORTCUT_KEY           EZ_BUTTON_SHORTCUT  
#define EZ_EVENT_HANDLE           38
#define EZ_EVENT_HANDLER          EZ_EVENT_HANDLE
#define EZ_LABEL_STRING           39
#define EZ_RESERVE_MENU_BUTTON    40
#define EZ_BACKING_STORE          41
#define EZ_EXPAND                 42  
#define EZ_TRANSIENT              43
#define EZ_SCROLLBAR_WIDTH        44
#define EZ_SCROLLBAR_BORDER_WIDTH 45
#define EZ_TEXT_SPACING           46
#define EZ_UNDERLINE              47
#define EZ_BUBBLE_HELP            48
#define EZ_MOVABLE                49
#define EZ_FREELABEL_MOVABLE      EZ_MOVABLE
#define EZ_ITEM_MOVABLE           EZ_FREELABEL_MOVABLE
#define EZ_INDICATOR_TYPE         50
#define EZ_INDICATOR_COLOR        51
#define EZ_TEXT_BACKGROUND        52  
#define EZ_SLIDER_DISPLAY_VALUE   53
#define EZ_HIGHLIGHT_FOREGROUND   54
#define EZ_HIGHLIGHT_FG           EZ_HIGHLIGHT_FOREGROUND
#define EZ_HIGHLIGHT_BACKGROUND   55
#define EZ_HIGHLIGHT_BG           EZ_HIGHLIGHT_BACKGROUND
#define EZ_ATTACH_PTR_DATA        56
#define EZ_ATTACH_INT_DATA        57
#define EZ_USER_PTR_DATA          EZ_ATTACH_PTR_DATA    
#define EZ_USER_INT_DATA          EZ_ATTACH_INT_DATA
#define EZ_CLIENT_PTR_DATA        EZ_ATTACH_PTR_DATA    
#define EZ_CLIENT_INT_DATA        EZ_ATTACH_INT_DATA
#define EZ_DESTROY_CALLBACK       58
#define EZ_EMBEDER                59
#define EZ_IS_EMBEDER             EZ_EMBEDER
#define EZ_DND_DRAG_CURSOR        60
#define EZ_DND_BUBBLE_HELP        61
#define EZ_SHAPED_WINDOW          62
#define EZ_MOTION_CALLBACK        63

#define EZ_CHECK_BUTTON_ON_VALUE  64
#define EZ_CHECK_BUTTON_OFF_VALUE 65
#define EZ_CHECK_BUTTON_ON_OFF    66
#define EZ_RADIO_BUTTON_GROUP     67
#define EZ_RADIO_BUTTON_VALUE     68
#define EZ_OPTIONAL_HSCROLLBAR    69
#define EZ_OPTIONAL_VSCROLLBAR    70
#define EZ_FANCY_LISTBOX_TYPE     71
#define EZ_FANCY_LIST_BOX_TYPE     EZ_FANCY_LISTBOX_TYPE
#define EZ_FANCY_LISTBOX_COLUMNS  72
#define EZ_FANCY_LIST_BOX_COLUMNS EZ_FANCY_LISTBOX_COLUMNS
#define EZ_GLOB_PATTERN           73
#define EZ_TEXT_WIDGET_EDITABLE   74
#define EZ_ENTRY_STRING           75
#define EZ_RETURN_VALUE           76
#define EZ_SLIDER_INIT_VALUE      77
#define EZ_OPTIONAL_ENTRY_REMEMBER_HISTORY   78
#define EZ_OPTION_ENTRY_REMEMBER_HISTORY     EZ_OPTIONAL_ENTRY_REMEMBER_HISTORY
#define EZ_OPTIONAL_ENTRY_HISTORY   78
#define EZ_OPTION_ENTRY_HISTORY     EZ_OPTION_ENTRY_HISTORY
#define EZ_ENTRY_EDITABLE           79
#define EZ_OPTIONAL_ENTRY_EDITABLE  EZ_ENTRY_EDITABLE
#define EZ_OPTION_ENTRY_EDITABLE    EZ_ENTRY_EDITABLE
#define EZ_ITEM_WIDGET_WIDGET     80
#define EZ_RAW_RGB_DATA           81
#define EZ_MENU_TEAR_OFF          82
#define EZ_GRID_CELL_GEOMETRY     83
#define EZ_GRID_CELL_PLACEMENT    84
#define EZ_GRID_CONSTRAINS        85
#define EZ_GRID_ROW_CONSTRAINS    86
#define EZ_GRID_COLUMN_CONSTRAINS 87

#define EZ_LOCATION               88
#define EZ_SIZE                   89
#define EZ_DIMENSION              EZ_SIZE
#define EZ_GEOMETRY               90
#define EZ_SIZE_HINT              91

#define EZ_PADB                   92
#define EZ_HIGHLIGHT_PAD          EZ_PADB
#define EZ_OPAD                   EZ_PADB
#define EZ_FOCUS_PAD              EZ_PADB

#define EZ_NAME                   93

#define EZ_LCD_NDIGITS            94
#define EZ_LCD_FONT_SIZE          95
#define EZ_LCD_BACKGROUND         96
#define EZ_LCD_FOREGROUND         97

#define EZ_LED_BACKGROUND         98
#define EZ_LED_PIXEL_COLOR        99
#define EZ_LED_WIDTH              100
#define EZ_LED_HEIGHT             101
#define EZ_LED_PIXEL_SIZE         102
#define EZ_LED_COLOR_FUNCTION     103

#define EZ_WMHINTS                104
#define EZ_WM_HINTS               EZ_WMHINTS
#define EZ_GEOMETRY_MANAGER       105
#define EZ_CURSOR                 106
#define EZ_CLASS                  107
#define EZ_POPUP_NAME             108
#define EZ_LABELED_ICON           109

#define EZ_HSCROLL_INCREMENT      110
#define EZ_VSCROLL_INCREMENT      111

#define EZ_SMETER_VALUE           112
#define EZ_SMETER_RANGE           113
#define EZ_SMETER_STOP            114
#define EZ_SMETER_STOPED          EZ_SMETER_STOP
#define EZ_SMETER_BACKGROUND      115
#define EZ_SMETER_FOREGROUND      116
#define EZ_SMETER_FOREGROUND_N    117
#define EZ_SMETER_SHOW_VALUE      118
#define EZ_SMETER_VALUE_N         119
#define EZ_SMETER_RANGE_N         120
#define EZ_SMETER_STYLE           121
#define EZ_SMETER_ORIENTATION     122

#define EZ_FORGET_X               123
#define EZ_FORGET_Y               124
#define EZ_FORGET_W               125
#define EZ_FORGET_H               126
#define EZ_FORGET_LOCATION        127
#define EZ_FORGET_POSITION        EZ_FORGET_LOCATION
#define EZ_FORGET_SIZE            128
#define EZ_FORGET_GEOMETRY        129

#define EZ_WM_WINDOW_NAME         130
#define EZ_WM_INITIAL_STATE       131	
#define EZ_WM_ICON_NAME           132
#define EZ_WM_ICON_XPM_FILE       133
#define EZ_WM_ICON_PIXMAP         134
#define EZ_WM_ICON_POSITION       135
#define EZ_WM_POSITION_HINT       136
#define EZ_WM_SIZE_HINT           137
#define EZ_WM_MAX_SIZE_HINT       138
#define EZ_WM_MIN_SIZE_HINT       139

#define EZ_MARK_CLEAN             140

#define EZ_HIGHLIGHT_MODE         141
#define EZ_RUBBER_BAND            142
#define EZ_BG_IMAGE_FILE          143
#define EZ_BG_PIXMAP              144
#define EZ_BG_IMAGE_FILE_B        145
#define EZ_BG_PIXMAP_B            146
#define EZ_DRAG_HANDLE            147
#define EZ_DOCKABLE               148
#define EZ_SLIDER_STYLE           149
#define EZ_GRIP_BUTTON_STYLE      EZ_SLIDER_STYLE
#define EZ_ROW_BG                 150
#define EZ_ARROW_LABEL            151
#define EZ_ARROW_TYPE             EZ_ARROW_LABEL
#define EZ_INTERIOR_BORDER        152
#define EZ_OPTIONAL_ENTRY_STYLE   153
#define EZ_OPTION_ENTRY_STYLE     EZ_OPTION_ENTRY_STYLE
#define EZ_TILE_ORIGIN            154
#define EZ_WM_IGNORE              155
#define EZ_LABEL_SHADOW           156
#define EZ_TERM_COLOR_N           157
#define EZ_TERM_CURSOR_COLOR      158
#define EZ_TERM_SAVE_LINES        159
#define EZ_TERM_REVERSE_VIDEO     160
#define EZ_SELECTION_BACKGROUND   161
#define EZ_SELECTION_FOREGROUND   162
#define EZ_RULER_DISPLAY_VALUE    163
#define EZ_RULER_TICK_UNIT        164
#define EZ_RULER_TICK_LENGTH      165
#define EZ_RULER_POSITION         166
#define EZ_RULER_TICK_SCALE       167
#define EZ_RULER_TICK_OFFSET      168
#define EZ_OVERRIDE_REDIRECT      169
#define EZ_RESOURCES_HANDLE       170
#define EZ_RESOURCES_HANDLER      EZ_RESOURCES_HANDLE
#define EZ_SCROLL_TYPE            171
#define EZ_SPIN_FUNCTION          172
#define EZ_SPIN_VALUE             173
#define EZ_LOCATOR_XY             174
#define EZ_SPECIAL_EFFECT         175
#define EZ_SHOW_TICK              176
#define EZ_DIAL_RANGE             177
#define EZ_DIAL_RESOLUTION        178
#define EZ_DIAL_SPAN              179
#define EZ_DIAL_DISPLAY_VALUE     180
#define EZ_DIAL_STYLE             181
#define EZ_DIAL_COLOR             182
#define EZ_DIAL_VALUE             183
#define EZ_CURSOR_NAME            184
#define EZ_DND_DRAG_CURSOR_NAME   185
#define EZ_SHAPE_FILE             186
#define EZ_SHAPE_PIXMAP           187
#define EZ_WRAP_TEXT              188
#define EZ_BAR_EDITABLE           189
#define EZ_BAR_ORIENTATION        190
#define EZ_BAR_COLOR_N            191
#define EZ_BAR_RANGE              192
#define EZ_BAR_WIDTH              193
#define EZ_HISTOGRAM_SAMPLE       194
#define EZ_SEPARATOR_STYLE        195
#define EZ_ROW_COLUMN_MAX_SIZE    196 
#define EZ_RESOURCE_STRING        197
#define EZ_SHEET_HEADER_SIZE      198
#define EZ_SHEET_CELL_SIZE        199

#define EZ_WIDGET_CONFIGURE_LAST_OPTION       199

/***********************************************************************
 ***                                                                 ***
 ***                   Border Styles                                 ***
 ***                                                                 ***
 ***********************************************************************/
#define EZ_BORDER_NONE                  0
#define EZ_BORDER_FLAT                  1
#define EZ_BORDER_FRAMED_UP             2
#define EZ_BORDER_RIDGE                 EZ_BORDER_FRAMED_UP
#define EZ_BORDER_FRAMED_DOWN           3
#define EZ_BORDER_GROOVE                EZ_BORDER_FRAMED_DOWN
#define EZ_BORDER_UP                    4
#define EZ_BORDER_RAISED                EZ_BORDER_UP
#define EZ_BORDER_DOWN                  5
#define EZ_BORDER_SUNKEN                EZ_BORDER_DOWN
#define EZ_BORDER_NB_UP                 6
#define EZ_BORDER_NB_DOWN               7
#define EZ_BORDER_EMBOSSED              8
#define EZ_BORDER_ENGRAVED              9
#define EZ_BORDER_NB_UP_L               10
#define EZ_BORDER_NB_DOWN_L             11
#define EZ_BORDER_NB_UP_R               12
#define EZ_BORDER_NB_DOWN_R             13
#define EZ_BORDER_NB_UP_B               14
#define EZ_BORDER_NB_DOWN_B             15
#define EZ_BORDER_TB_UP                 16
#define EZ_BORDER_TB_RAISED             16
#define EZ_BORDER_TB_DOWN               17
#define EZ_BORDER_TB_SUNKEN             17
#define EZ_BORDER_LR_UP                 18
#define EZ_BORDER_LR_RAISED             18
#define EZ_BORDER_LR_DOWN               19
#define EZ_BORDER_LR_SUNKEN             19
#define EZ_BORDER_TB_FLAT               20
#define EZ_BORDER_LR_FLAT               21
#define EZ_BORDER_SHADOW                22
#define EZ_BORDER_SHADOW3               23
#define EZ_HSLIDER_BORDER               24
#define EZ_VSLIDER_BORDER               25

/***********************************************************************
 ***                                                                 ***
 ***                  Cursors                                        ***
 ***                                                                 ***
 ***********************************************************************/
#ifndef EZ_C_NUM_GLYPHS
#define EZ_C_NUM_GLYPHS            XC_num_glyphs
#define EZ_C_X_CURSOR              XC_X_cursor
#define EZ_C_ARROW                 XC_arrow
#define EZ_C_BASED_ARROW_DOWN      XC_based_arrow_down
#define EZ_C_BASED_ARROW_UP        XC_based_arrow_up
#define EZ_C_BOAT                  XC_boat
#define EZ_C_BOGOSITY              XC_bogosity
#define EZ_C_BOTTOM_LEFT_CORNER    XC_bottom_left_corner
#define EZ_C_BOTTOM_RIGHT_CORNER   XC_bottom_right_corner
#define EZ_C_BOTTOM_SIDE           XC_bottom_side
#define EZ_C_BOTTOM_TEE            XC_bottom_tee
#define EZ_C_BOX_SPIRAL            XC_box_spiral
#define EZ_C_CENTER_PTR            XC_center_ptr
#define EZ_C_CIRCLE                XC_circle
#define EZ_C_CLOCK                 XC_clock
#define EZ_C_COFFEE_MUG            XC_coffee_mug
#define EZ_C_CROSS                 XC_cross
#define EZ_C_CROSS_REVERSE         XC_cross_reverse
#define EZ_C_CROSSHAIR             XC_crosshair
#define EZ_C_DIAMOND_CROSS         XC_diamond_cross
#define EZ_C_DOT                   XC_dot
#define EZ_C_DOTBOX                XC_dotbox
#define EZ_C_DOUBLE_ARROW          XC_double_arrow
#define EZ_C_DRAFT_LARGE           XC_draft_large
#define EZ_C_DRAFT_SMALL           XC_draft_small
#define EZ_C_DRAPED_BOX            XC_draped_box
#define EZ_C_EXCHANGE              XC_exchange
#define EZ_C_FLEUR                 XC_fleur
#define EZ_C_GOBBLER               XC_gobbler
#define EZ_C_GUMBY                 XC_gumby
#define EZ_C_HAND1                 XC_hand1
#define EZ_C_HAND2                 XC_hand2
#define EZ_C_HEART                 XC_heart
#define EZ_C_ICON                  XC_icon
#define EZ_C_IRON_CROSS            XC_iron_cross
#define EZ_C_LEFT_PTR              XC_left_ptr
#define EZ_C_LEFT_SIDE             XC_left_side
#define EZ_C_LEFT_TEE              XC_left_tee
#define EZ_C_LEFTBUTTON            XC_leftbutton
#define EZ_C_LL_ANGLE              XC_ll_angle
#define EZ_C_LR_ANGLE              XC_lr_angle
#define EZ_C_MAN                   XC_man
#define EZ_C_MIDDLEBUTTON          XC_middlebutton
#define EZ_C_MOUSE                 XC_mouse
#define EZ_C_PENCIL                XC_pencil
#define EZ_C_PIRATE                XC_pirate
#define EZ_C_PLUS                  XC_plus
#define EZ_C_QUESTION_ARROW        XC_question_arrow
#define EZ_C_RIGHT_PTR             XC_right_ptr
#define EZ_C_RIGHT_SIDE            XC_right_side
#define EZ_C_RIGHT_TEE             XC_right_tee
#define EZ_C_RIGHTBUTTON           XC_rightbutton
#define EZ_C_RTL_LOGO              XC_rtl_logo
#define EZ_C_SAILBOAT              XC_sailboat
#define EZ_C_SB_DOWN_ARROW         XC_sb_down_arrow
#define EZ_C_SB_H_DOUBLE_ARROW     XC_sb_h_double_arrow
#define EZ_C_SB_LEFT_ARROW         XC_sb_left_arrow
#define EZ_C_SB_RIGHT_ARROW        XC_sb_right_arrow
#define EZ_C_SB_UP_ARROW           XC_sb_up_arrow
#define EZ_C_SB_V_DOUBLE_ARROW     XC_sb_v_double_arrow
#define EZ_C_SHUTTLE               XC_shuttle
#define EZ_C_SIZING                XC_sizing
#define EZ_C_SPIDER                XC_spider
#define EZ_C_SPRAYCAN              XC_spraycan
#define EZ_C_STAR                  XC_star
#define EZ_C_TARGET                XC_target
#define EZ_C_TCROSS                XC_tcross
#define EZ_C_TOP_LEFT_ARROW        XC_top_left_arrow
#define EZ_C_TOP_LEFT_CORNER       XC_top_left_corner
#define EZ_C_TOP_RIGHT_CORNER      XC_top_right_corner
#define EZ_C_TOP_SIDE              XC_top_side
#define EZ_C_TOP_TEE               XC_top_tee
#define EZ_C_TREK                  XC_trek
#define EZ_C_UL_ANGLE              XC_ul_angle
#define EZ_C_UMBRELLA              XC_umbrella
#define EZ_C_UR_ANGLE              XC_ur_angle
#define EZ_C_WATCH                 XC_watch
#define EZ_C_XTERM                 XC_xterm
#endif


/***********************************************************************
 ***                                                                 ***
 ***                   Item  Types                                   ***
 ***                                                                 ***
 ***********************************************************************/
#define EZ_ITEM_LABEL                 1
#define EZ_ITEM_WIDGET                2
#define EZ_LABEL_ITEM                 EZ_ITEM_LABEL
#define EZ_WIDGET_ITEM                EZ_ITEM_WIDGET
#define EZ_ITEM_FIG                   3
#define EZ_FIG_ITEM                   EZ_ITEM_FIG

/* Fig Item Building Blocks */
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

/***********************************************************************
 ***                                                                 ***
 ***     DirTree directory node opening modes                        ***
 ***                                                                 ***
 ***********************************************************************/
#define EZ_DIR_NODE_OPEN_DIRECTORY_NONE             -1
#define EZ_DIR_NODE_OPEN_DIRECTORY_DEFAULT          0
#define EZ_DIR_NODE_OPEN_DIRECTORY_COLLAPSING       1


/*  default font ids */
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
 ***                   Widget Types                                  ***
 ***                                                                 ***
 ***********************************************************************/
/*
 * do not change the order of this list
 */
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
#define EZ_WIDGET_ILIST_BOX              18
#define EZ_WIDGET_ILISTBOX               EZ_WIDGET_ILIST_BOX
#define EZ_WIDGET_LIST_BOX               19
#define EZ_WIDGET_LISTBOX                EZ_WIDGET_LIST_BOX
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
#define EZ_WIDGET_OPTION_ENTRY           EZ_WIDGET_OPTIONAL_ENTRY
#define EZ_WIDGET_FILE_SELECTOR          29
#define EZ_WIDGET_ITREE                  30
#define EZ_WIDGET_TREE                   31
#define EZ_WIDGET_LIST_TREE              EZ_WIDGET_TREE
#define EZ_WIDGET_IFANCY_LIST_BOX        32
#define EZ_WIDGET_IFANCY_LISTBOX         EZ_WIDGET_IFANCY_LIST_BOX 
#define EZ_WIDGET_FANCY_LIST_BOX         33
#define EZ_WIDGET_FANCY_LISTBOX          EZ_WIDGET_FANCY_LIST_BOX
#define EZ_WIDGET_IWORK_AREA             34
#define EZ_WIDGET_IWORKAREA              EZ_WIDGET_IWORK_AREA
#define EZ_WIDGET_WORK_AREA              35
#define EZ_WIDGET_WORKAREA               EZ_WIDGET_WORK_AREA
#define EZ_WIDGET_RAW_XWINDOW            36
#define EZ_WIDGET_TEAR_OFF               37
#define EZ_WIDGET_ROW_COLUMN             38
#define EZ_WIDGET_GRID_BAG               EZ_WIDGET_ROW_COLUMN
#define EZ_WIDGET_LCD                    39
#define EZ_WIDGET_LED                    40
#define EZ_WIDGET_STATUS_METER           41
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

#define EZ_WIDGET_POPUP_MENU              125
#define EZ_WIDGET_MENU                    EZ_WIDGET_POPUP_MENU
#define EZ_WIDGET_ITOPLEVEL               126
#define EZ_WIDGET_ICON                    EZ_WIDGET_ITOPLEVEL           
#define EZ_WIDGET_FREE_LABEL              EZ_WIDGET_ITOPLEVEL           
#define EZ_WIDGET_EMBEDER                 127

/***********************************************************************/
#define EZ_TREE                           0
#define EZ_LIST_TREE                      1
#define EZ_BRANCHED_LIST_TREE             2
/***********************************************************************
 ***                                                                 ***
 ***                   LED Widget actions                            ***
 ***                                                                 ***
 ***********************************************************************/

#define EZ_LED_SCROLL_LEFT        1
#define EZ_LED_SCROLL_RIGHT       2
#define EZ_LED_SCROLL_UP          3
#define EZ_LED_SCROLL_DOWN        4
#define EZ_LED_SCROLL_CENTE_H     5
#define EZ_LED_SCROLL_CENTER_V    6
#define EZ_LED_SWEEP_LEFT         7
#define EZ_LED_SWEEP_RIGHT        8
#define EZ_LED_SWEEP_UP           9
#define EZ_LED_SWEEP_DOWN         10
#define EZ_LED_SWEEP_CENTER       11
#define EZ_LED_SWEEP_CENTER_V     12
#define EZ_LED_BOUNCE             13
#define EZ_LED_BOUNCE_V           14
#define EZ_LED_SHOW               15
#define EZ_LED_BLINK              16
#define EZ_LED_BLINK_SCROLL_LEFT  17
#define EZ_LED_BLINK_SCROLL_RIGHT 18
#define EZ_LED_SLEEP              19

/***********************************************************************
 ***                                                                 ***
 ***                   Widget Arrangements                           ***
 ***                                                                 ***
 ***********************************************************************/
/*
 *  Orientation
 */
#define EZ_HORIZONTAL              1
#define EZ_HORIZONTAL_LEFT         2
#define EZ_HORIZONTAL_RIGHT        3
#define EZ_HORIZONTAL_CENTER       4
#define EZ_VERTICAL                5
#define EZ_VERTICAL_TOP            6
#define EZ_VERTICAL_BOTTOM         7
#define EZ_VERTICAL_CENTER         8
/*
 * Filling
 */
#define EZ_FILL_NONE               0
#define EZ_FILL_HORIZONTALLY       1
#define EZ_FILL_VERTICALLY         2
#define EZ_FILL_BOTH               3
/*
 * Allignment
 */               
#define EZ_CENTER_ALIGNED          0
#define EZ_LEFT_ALIGNED            1
#define EZ_RIGHT_ALIGNED           2
#define EZ_TOP_ALIGNED             3
#define EZ_BOTTOM_ALIGNED          4
/*
 * Label position, justification etc.
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

#define EZ_UP                      10
#define EZ_DOWN                    11
#define EZ_LEFT_RIGHT              12
#define EZ_UP_DOWN                 13

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
#define EZ_BALL_INDICATOR           10

/***********************************************************************
 ***                                                                 ***
 ***       Arrow Types (ArrowButton, Label, NWLabel)                 ***
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
#define EZ_DOWN_TRIANGLE_BAR    10 
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
 ***               Symbolic constants used in EZGL                   ***
 ***                                                                 ***
 ***********************************************************************/
/*
 *  For EZ_NewList()
 */
#define EZ_COMPILE                1
#define EZ_COMPILE_AND_EXECUTE    2
/*
 *  For FrontFace()
 */
#define EZ_CCW                    0
#define EZ_CW                     1
/*
 *  For MatrixMode()
 */
#define EZ_PROJECTION             0
#define EZ_MODELVIEW              1
#define EZ_TEXTURE                2
/*
 * For Begin()/End()
 */
#define  EZ_LINES                 1
#define  EZ_LINE_STRIP            2
#define  EZ_LINE_LOOP             3
#define  EZ_TRIANGLES             4
#define  EZ_TRIANGLE_STRIP        5
#define  EZ_TRIANGLE_FAN          6
#define  EZ_QUADS                 7
#define  EZ_QUAD_STRIP            8
#define  EZ_POLYGON               9
#define  EZ_POINTS                10
#define  EZ_TMESH                 11
#define  EZ_QMESH                 12
/*
 * For ColorMaterial()/Materialfv()/Lightfv()
 */
#define EZ_FRONT                  0
#define EZ_BACK                   1
#define EZ_FRONT_AND_BACK         2
#define EZ_EMISSION               1
#define EZ_AMBIENT                2
#define EZ_DIFFUSE                3
#define EZ_SPECULAR               4
#define EZ_SHININESS              5
#define EZ_AMBIENT_AND_DIFFUSE    6
#define EZ_COLOR_INDICES          7
#define EZ_POSITION               8
#define EZ_SPOT_DIRECTION         9
#define EZ_SPOT_EXPONENT          10
#define EZ_SPOT_CUTOFF            11
#define EZ_CONSTANT_ATTENUATION   12
#define EZ_LINEAR_ATTENUATION     13
#define EZ_QUADRATIC_ATTENUATION  14
/*
 * For LightModelf()
 */
#define EZ_LIGHT_MODEL_AMBIENT      1
#define EZ_LIGHT_MODEL_LOCAL_VIEWER 2
#define EZ_LIGHT_MODEL_TWO_SIDE     3

/*
 * EZ_Fogfv()
 */
#define EZ_FOG_MODE                 1
#define EZ_FOG_DENSITY              2
#define EZ_FOG_COLOR                3
#define EZ_FOG_INDEX                4
#define EZ_FOG_START                5
#define EZ_FOG_END                  6

#define EZ_LINEAR                   1
#define EZ_EXP                      2
#define EZ_EXP2                     3

/*
 *  For Enable()/Disable()
 */
#define EZ_LIGHT0                   0
#define EZ_LIGHT1                   1
#define EZ_LIGHT2                   2
#define EZ_LIGHT3                   3
#define EZ_LIGHT4                   4
#define EZ_LIGHT5                   5
#define EZ_LIGHT6                   6
#define EZ_LIGHT7                   7
#define EZ_LIGHTING                 8
#define EZ_NORMALIZE                9
#define EZ_COLOR_MATERIAL           10
#define EZ_CULL_FACE                11
#define EZ_FOG                      12
#define EZ_DEPTH_TEST               13
#define EZ_DITHER                   14

/*
 * EZ_DefineMaterial()/EZ_DefineLightSrc()
 */
#define EZ_NULL_F                     0.0 
#define EZ_EMISSION_F                 301.0
#define EZ_AMBIENT_F                  302.0
#define EZ_DIFFUSE_F                  303.0
#define EZ_SPECULAR_F                 304.0
#define EZ_SHININESS_F                305.0
#define EZ_COLOR_INDICES_F            306.0
#define EZ_POSITION_F                 307.0
#define EZ_SPOT_DIRECTION_F           308.0
#define EZ_SPOT_EXPONENT_F            309.0
#define EZ_SPOT_CUTOFF_F              310.0
#define EZ_CONSTANT_ATTENUATION_F     311.0
#define EZ_LINEAR_ATTENUATION_F       312.0
#define EZ_QUADRATIC_ATTENUATION_F    313.0
#define EZ_LIGHT_MODEL_AMBIENT_F      314.0 
#define EZ_LIGHT_MODEL_LOCAL_VIEWER_F 315.0
#define EZ_LIGHT_MODEL_TWO_SIDE_F     316.0

/*
 * For EZ_Clear()
 */
#define  EZ_COLOR_BUFFER_BIT          0x01
#define  EZ_DEPTH_BUFFER_BIT          0x02

/*
 * For EZ_PolygonMode()
 */
#define EZ_FILL                       0x0
#define EZ_LINE                       0x1
#define EZ_POINT                      0x2

/*
 * For EZ_DrawString()
 */
#define  EZ_VECTOR_FONT               0x01
#define  EZ_BITMAP_FONT               0x02
/*
 * For EZ_LogicOp
 */
#define  EZ_CLEAR                     0x0
#define  EZ_AND                       0x1
#define  EZ_AND_REVERSE               0x2
#define  EZ_COPY                      0x3
#define  EZ_AND_INVERTED              0x4
#define  EZ_NOOP                      0x5
#define  EZ_XOR                       0x6
#define  EZ_OR                        0x7
#define  EZ_NOR                       0x8
#define  EZ_EQUIV                     0x9
#define  EZ_INVERT                    0xa
#define  EZ_OR_REVERSE                0xb
#define  EZ_COPY_INVERTED             0xc
#define  EZ_OR_INVERTED               0xd
#define  EZ_NAND                      0xe
#define  EZ_SET                       0xf
/*
 * For EZ_Sphere()
 */
#define EZ_SPHERE_TRIANGLE            1
#define EZ_SPHERE_QUAD                2
#define EZ_SPHERE_PARAMETRIC          3
/*
 * For EZ_SetBackBuffer()
 */
#define  EZ_PIXMAP                    0x1
#define  EZ_XIMAGE                    0x2
/*
 * For EZ_ShadeModel()
 */
#define EZ_FLAT                       0
#define EZ_SMOOTH                     1

/*
 * For EZ_SetVectorFont()
 */
#define EZ_SMALL_VECTOR_FONT          0
#define EZ_LARGE_VECTOR_FONT          1


/*
 * For EZ_Save3DCanvas2PSA()
 */
#define EZ_PORTRAIT                   0
#define EZ_LANDSCAPE                  1

/*
 *   GL Device and Event types. Used in EZ_GL Event routines.
 */
#define EZ_BACKSPACE_KEY             0X7f
#define EZ_TAB_KEY                   0X09
#define EZ_RETURN_KEY                0X0D 
#define EZ_ESCAPE_KEY                0X1B
/*
 * some convinent function keys.
 */
#define EZ_HOME_KEY                  0xFF50
#define EZ_LEFT_KEY                  0xFF51
#define EZ_UP_KEY                    0xFF52
#define EZ_RIGHT_KEY                 0xFF53
#define EZ_DOWN_KEY                  0xFF54
#define EZ_PAGE_UP_KEY               0xFF55
#define EZ_PAGE_DOWN_KEY             0xFF56
#define EZ_PRIOR_KEY                 0xFF55
#define EZ_NEXT_KEY                  0xFF56
#define EZ_BEGIN_KEY                 0xFF57
#define EZ_END_KEY                   0xFF58

#define EZ_F1_KEY                    0xFFbe
#define EZ_F2_KEY                    0xFFbf
#define EZ_F3_KEY                    0xFFc0
#define EZ_F4_KEY                    0xFFc1
#define EZ_F5_KEY                    0xFFc2
#define EZ_F6_KEY                    0xFFc3
#define EZ_F7_KEY                    0xFFc4
#define EZ_F8_KEY                    0xFFc5
#define EZ_F9_KEY                    0xFFc6
#define EZ_F10_KEY                   0xFFc7
#define EZ_F11_KEY                   0xFFc8
#define EZ_F12_KEY                   0xFFc9

/*
 *  Button and Pointer events types.
 */
#define EZ_LEFT_BUTTON               0x1
#define EZ_MIDDLE_BUTTON             0x2
#define EZ_RIGHT_BUTTON              0x4
#define EZ_BUTTON1                   EZ_LEFT_BUTTON
#define EZ_BUTTON2                   EZ_MIDDLE_BUTTON
#define EZ_BUTTON3                   EZ_RIGHT_BUTTON
#define EZ_BUTTON4                   0x8
#define EZ_BUTTON5                   0x10

#define EZ_REDRAW                    256
#define EZ_RESIZE                    257
#define EZ_LEFT_BUTTON_PRESS         258
#define EZ_MIDDLE_BUTTON_PRESS       259
#define EZ_RIGHT_BUTTON_PRESS        260
#define EZ_LEFT_BUTTON_RELEASE       261  
#define EZ_MIDDLE_BUTTON_RELEASE     262
#define EZ_RIGHT_BUTTON_RELEASE      263
#define EZ_POINTER_MOTION            264
#define EZ_KEY_PRESS                 265
#define EZ_ENTER_WINDOW              266
#define EZ_LEAVE_WINDOW              267

#define EZ_CLIENT_MESSAGE            268
#define EZ_PROPERTY_NOTIFY           269

#define EZ_BUTTON1_PRESS             EZ_LEFT_BUTTON_PRESS
#define EZ_BUTTON2_PRESS             EZ_MIDDLE_BUTTON_PRESS
#define EZ_BUTTON3_PRESS             EZ_RIGHT_BUTTON_PRESS
#define EZ_BUTTON4_PRESS             270
#define EZ_BUTTON5_PRESS             271

#define EZ_BUTTON1_RELEASE           EZ_LEFT_BUTTON_RELEASE
#define EZ_BUTTON2_RELEASE           EZ_MIDDLE_BUTTON_RELEASE
#define EZ_BUTTON3_RELEASE           EZ_RIGHT_BUTTON_RELEASE
#define EZ_BUTTON4_RELEASE           272
#define EZ_BUTTON5_RELEASE           273
/* Add new constants here, update lib/EZ_GL.h also */

#define EZ_LeftButtonDown()          (EZ_ButtonState & EZ_Button1)
#define EZ_MiddleButtonDown()        (EZ_ButtonState & EZ_Button2)
#define EZ_RightButtonDown()         (EZ_ButtonState & EZ_Button3)

#define EZ_Button1Down()             (EZ_ButtonState & EZ_Button1)
#define EZ_Button2Down()             (EZ_ButtonState & EZ_Button2)
#define EZ_Button3Down()             (EZ_ButtonState & EZ_Button3)

/*
 *  A list of predefined materials
 */
#define EZ_SILVER                    0
#define EZ_SILVER_S                  1
#define EZ_GOLD                      2
#define EZ_GOLD_S                    3
#define EZ_BRASS                     4
#define EZ_COPPER                    5    
#define EZ_COPPER_S                  6
#define EZ_BRONZE                    7
#define EZ_RUBY                      8
#define EZ_TURQUOISE                 9  
#define EZ_JADE                      10
#define EZ_EMERALD                   11
#define EZ_PERAL                     12

/*
 *   Colors.
 */
#ifndef EZ_GRAY0 
#define EZ_GRAY0                     0
#define EZ_GRAY1                     1
#define EZ_GRAY2                     2
#define EZ_GRAY3                     3
#define EZ_GRAY4                     4
#define EZ_GRAY5                     5
#define EZ_GRAY6                     6
#define EZ_GRAY7                     7
#define EZ_GRAY8                     8
#define EZ_DARKRED                   9
#define EZ_RED                       10 
#define EZ_GREEN                     11 
#define EZ_BLUE                      12 
#define EZ_YELLOW                    13 
#define EZ_MAGENTA                   14 
#define EZ_CYAN                      15 

#define EZ_BLACK                     EZ_GRAY0
#define EZ_WHITE                     EZ_GRAY8

#define EZ_DARK_GRAY1              EZ_GRAY1
#define EZ_DARK_GRAY2              EZ_GRAY2
#define EZ_NORMAL_GRAY1            EZ_GRAY4
#define EZ_NORMAL_GRAY2            EZ_GRAY5
#define EZ_BRIGHT_GRAY1            EZ_GRAY6
#define EZ_BRIGHT_GRAY2            EZ_GRAY7
#define EZ_GRAY                    EZ_GRAY3
#endif

/*
 * Line Styles
 */

#define EZ_LineSolid               0
#define EZ_LineOnOffDash           1
#define EZ_LineDoubleDash          2

/*******************************************************************
 ***                                                             ***
 ***                Forward Declarations.                        ***
 ***                                                             ***
 ******************************************************************/
#define EZ_InvertMatrix(inv,mat)      EZ_InvertMatrix4_((inv),(mat),0)
#define EZ_InverseTranspose(inv,mat)  EZ_InvertMatrix4_((inv),(mat),1)
/*
 * From EZ_Render.c
 */
#ifndef _EZ_RENDER_C_
EXTERN void              EZ_PolygonMode MY_ANSIARGS((int face, int mode));
EXTERN void              EZ_FrontFace MY_ANSIARGS((int face));
EXTERN void              EZ_CullFace MY_ANSIARGS((int face));
EXTERN void              EZ_Color3f MY_ANSIARGS((float x, float y, float z));
EXTERN void              EZ_Color3fv MY_ANSIARGS((float *rgb));
EXTERN void              EZ_Color4f MY_ANSIARGS((float x, float y, float z, float w));
EXTERN void              EZ_Color4fv  MY_ANSIARGS((float *rgba));
EXTERN void              EZ_Index  MY_ANSIARGS((int idx));
EXTERN void              EZ_Indexf  MY_ANSIARGS((float fidx));
EXTERN void              EZ_Indexi  MY_ANSIARGS((int idx));
EXTERN void              EZ_SetLineStyle MY_ANSIARGS((int style, int width));
EXTERN void              EZ_Normal3f MY_ANSIARGS((float x, float y, float z));
EXTERN void              EZ_Normal3fv MY_ANSIARGS((float *xyz));

EXTERN void              EZ_Vertex3f MY_ANSIARGS((float x, float y, float z));
EXTERN void              EZ_Vertex3fv MY_ANSIARGS((float *xyz));
EXTERN void              EZ_Vertex2f MY_ANSIARGS((float x, float y));
EXTERN void              EZ_Vertex2fv  MY_ANSIARGS((float *xyz));

EXTERN void              EZ_Begin  MY_ANSIARGS((int what));
EXTERN void              EZ_End  MY_ANSIARGS((void));
EXTERN void              EZ_SetTQMeshSize  MY_ANSIARGS((int rows, int cols));
#endif

/*
 * From EZ_String.c
 */
#ifndef _EZ_STRING_C_
EXTERN void       EZ_SetVectorFont MY_ANSIARGS((int fid));
EXTERN void       EZ_SetBitmapFont  MY_ANSIARGS((char *fname));
EXTERN void       EZ_CMove2f  MY_ANSIARGS((float x, float y)); 
EXTERN void       EZ_CMove3f MY_ANSIARGS((float x, float y, float z)); 
EXTERN void       EZ_CMove3fv MY_ANSIARGS((float *xyz)); 
EXTERN void       EZ_CMoveScreen2f  MY_ANSIARGS((float x, float y)); 
EXTERN void       EZ_CMoveScreen3f MY_ANSIARGS((float x, float y, float z)); 
EXTERN void       EZ_CMoveScreen3fv MY_ANSIARGS((float *xyz)); 
EXTERN void       EZ_DrawString MY_ANSIARGS((int font, char *str)); 
#endif

/*
 * From EZ_LightMaterial.c
 */
#ifndef _EZ_LIGHTMATERIAL_C_
EXTERN void         EZ_ColorMaterial MY_ANSIARGS((int face, int mode));
EXTERN void         EZ_ShadeModel MY_ANSIARGS((int mode));
EXTERN void         EZ_Materialfv MY_ANSIARGS((int face, int attr, float *values));
EXTERN void         EZ_Lightfv MY_ANSIARGS((int num, int attr, float *values));
EXTERN void         EZ_LightModelfv MY_ANSIARGS((int attr, float *values));
EXTERN void         EZ_Materialf MY_ANSIARGS((int face, int attr, float values));
EXTERN void         EZ_Lightf MY_ANSIARGS((int num, int attr, float values));
EXTERN void         EZ_LightModelf MY_ANSIARGS((int attr, float values));
#endif

/*
 * From EZ_LightMaterial1.c
 */
#ifndef _EZ_LIGHTMATERIAL1_C_
EXTERN void                EZ_BindMaterial MY_ANSIARGS((int face, EZ_Material *mat));
EXTERN void                EZ_BindLightSrc MY_ANSIARGS((int num, EZ_LightSrc *lghtsrc));
EXTERN void                EZ_BindLight MY_ANSIARGS((int num, EZ_LightSrc *lghtsrc));
EXTERN void                EZ_BindLightModel MY_ANSIARGS((EZ_LightModel *lghtmodel));
EXTERN EZ_Material         *EZ_GetDefaultMaterial MY_ANSIARGS((int idx));
EXTERN EZ_Material         *EZ_DefineMaterial MY_ANSIARGS((float *definition));
EXTERN EZ_LightSrc         *EZ_DefineLightSrc MY_ANSIARGS((float *definition));
EXTERN EZ_LightSrc         *EZ_DefineLight MY_ANSIARGS((float *definition));
EXTERN EZ_LightModel       *EZ_DefineLightModel MY_ANSIARGS((float *definition));
EXTERN void                EZ_MakeColorMapForMaterial MY_ANSIARGS((EZ_Material *mat, int n1, int n2));
#endif
/*
 * From EZ_Fog.c
 */
#ifndef _EZ_FOG_C_
EXTERN void      EZ_Fogfv MY_ANSIARGS((int name, float *values));
#endif
/*
 * From EZ_MatrixStack.c
 */
#ifndef _EZ_MATRIX_STACK_C_
EXTERN void        EZ_MatrixMode MY_ANSIARGS((int mode));
EXTERN void        EZ_PushMatrix MY_ANSIARGS((void));
EXTERN void        EZ_PopMatrix MY_ANSIARGS((void));
EXTERN void        EZ_LoadMatrix MY_ANSIARGS((EZ_Matrix mtx));
EXTERN void        EZ_MultMatrix MY_ANSIARGS((EZ_Matrix mtx));
EXTERN void        EZ_LoadIdentity MY_ANSIARGS((void));
EXTERN void        EZ_GetMatrix MY_ANSIARGS((EZ_Matrix mtx));
#endif
/*
 * From EZ_MatrixTrans.c
 */
#ifndef _EZ_MATRIX_TRANS_C_
EXTERN void             EZ_Scale MY_ANSIARGS((float x, float y, float z));
EXTERN void             EZ_Translate  MY_ANSIARGS((float x, float y, float z));
EXTERN void             EZ_Rotate  MY_ANSIARGS((float angle, float x, float y, float z));
EXTERN void             EZ_GenerateRotationMatrix MY_ANSIARGS((EZ_Matrix mtx, float angle, float x, float y, float z));
#endif

/*
 * From EZ_MatrixVect.c
 */
#ifndef _EZ_MATRIX_VECT_C_
EXTERN void      EZ_CopyVector MY_ANSIARGS((EZ_Vector des, EZ_Vector src));
EXTERN void      EZ_CopyMatrix MY_ANSIARGS((EZ_Matrix des, EZ_Matrix src));
EXTERN void      EZ_CopyTransposeMatrix MY_ANSIARGS((EZ_Matrix des, EZ_Matrix src));
EXTERN void      EZ_IdentityMatrix MY_ANSIARGS((EZ_Matrix mtx));
EXTERN void      EZ_ZeroMatrix MY_ANSIARGS((EZ_Matrix mtx));
EXTERN void      EZ_InvertMatrix4_ MY_ANSIARGS((EZ_Matrix imtx, EZ_Matrix mtx, int transp));
EXTERN void      EZ_MultiplyMatrix  MY_ANSIARGS((EZ_Matrix ans, EZ_Matrix a, EZ_Matrix b));
EXTERN void      EZ_MultVM MY_ANSIARGS((EZ_Vector ans, EZ_Vector v, EZ_Matrix m));
#endif
/*
 * From EZ_ProjView.c
 */
#ifndef _EZ_PROJ_VIEW_C_
EXTERN void   EZ_Ortho2 MY_ANSIARGS(( float   left, float right,
				      float   bottom, float top));
EXTERN void   EZ_Ortho MY_ANSIARGS(( float   left, float right,
				     float   bottom, float top,
				     float near, float far));
EXTERN void   EZ_Frustum MY_ANSIARGS(( float   left, float right,
				       float   bottom, float top,
				       float near, float far));
EXTERN void   EZ_Perspective MY_ANSIARGS(( float   fov, float aspect,
					   float   near, float far));
EXTERN void   EZ_LookAt MY_ANSIARGS((float  vx, float vy, float vz,
				     float  px, float py, float pz,
				     float  ux, float uy, float uz));
EXTERN void   EZ_ViewPort MY_ANSIARGS((float x0, float x1, float y0, float y1));
#endif
/*
 * From EZ_World2Screen.c
 */
#ifndef _EZ_WORLD_2_SCREEN_C_
EXTERN void         EZ_World2Screen MY_ANSIARGS((float *ptr, int *x, int *y, float *z));
EXTERN void         EZ_Screen2World MY_ANSIARGS((int x, int y, float *fx, float *fy));
#endif
/*
 * From EZ_InitGL.c
 */
#ifndef _EZ_INIT_GL_C_
EXTERN void                   EZ_InitializeGL  MY_ANSIARGS((void));
EXTERN void                   EZ_GLExit  MY_ANSIARGS((void));
#endif
/*
 * From EZ_Color.c
 */
#ifndef _EZ_COLOR_C_
EXTERN void           EZ_MapColor MY_ANSIARGS((int idx, int r, int g, int b));
EXTERN void           EZ_ColorMapMode MY_ANSIARGS((void));
EXTERN void           EZ_RGBMode MY_ANSIARGS((void));
EXTERN int            EZ_GetNumberOfReservedColorsForGL MY_ANSIARGS((void));
EXTERN int            EZ_GetNumberOfAllocatedColors MY_ANSIARGS((void));
EXTERN unsigned long  EZ_AllocateColorFromRGB MY_ANSIARGS((int r, int g, int b));
EXTERN unsigned long  EZ_AllocateColorFromName MY_ANSIARGS((char *name));
EXTERN void           EZ_PixelValue2RGB MY_ANSIARGS((unsigned long pv, int *r, int *g, int *b));
EXTERN void           EZ_SetKbdHighlightColorRGB MY_ANSIARGS((int r, int g, int b));
EXTERN void           EZ_SetKbdHighlightColor MY_ANSIARGS((char *name));
EXTERN int            EZ_SetGlobalBackground MY_ANSIARGS((char *name));
EXTERN int            EZ_SetGlobalForeground MY_ANSIARGS((char *name));
#endif

/*
 * From EZ_Sphere.c
 */
#ifndef _EZ_SPHERE_C_
EXTERN void EZ_Sphere MY_ANSIARGS((int type, int level, float x, float y, float z, float radius));
#endif

/*
 * From EZ_Sphere.c
 */
#ifndef _EZ_TUBE_C_
EXTERN void   EZ_Tube MY_ANSIARGS((float *ptr, int npts, int nsides));
#endif

/*
 * From EZ_Polyhedra.c
 */
#ifndef _EZ_POLYHEDRA_C_
EXTERN void         EZ_Tetrahedron MY_ANSIARGS((float x, float y, float z, float r));
EXTERN void         EZ_Cube MY_ANSIARGS((float x, float y, float z, float r));
EXTERN void         EZ_Octahedron MY_ANSIARGS((float x, float y, float z, float r));
EXTERN void         EZ_Dodecahedron MY_ANSIARGS((float x, float y, float z, float r));
EXTERN void         EZ_Icosahedron MY_ANSIARGS((float x, float y, float z, float r));
#endif
/*
 * From EZ_GLEvent.c
 */
#ifndef _EZ_GL_EVENT_C_
EXTERN int   EZ_XEvent2GLEvent MY_ANSIARGS((void *widget, XEvent *event));
EXTERN int   EZ_GLNextEvent MY_ANSIARGS((void *widget, XEvent *event));
EXTERN int   EZ_EnterEvent MY_ANSIARGS(( void *widget,
					 int             event_type,
					 int             *value));
EXTERN int   EZ_GLEnterEvent MY_ANSIARGS(( void *widget,
					   int             event_type,
					   int             *values));
EXTERN void  EZ_GLResetEvents MY_ANSIARGS((void *widget));
#endif
/*
 * From EZ_DpyList.c
 */
#ifndef _EZ_DPY_LIST_C_
EXTERN void          EZ_CallList MY_ANSIARGS((int list));
EXTERN void          EZ_CallLists MY_ANSIARGS((int start, int range));
EXTERN int           EZ_GenLists MY_ANSIARGS((int range));
EXTERN void          EZ_DeleteList MY_ANSIARGS((int list));
EXTERN void          EZ_DeleteLists  MY_ANSIARGS((int start, int range));
EXTERN void          EZ_NewList  MY_ANSIARGS((int lnum, int mode));
EXTERN void          EZ_EndList  MY_ANSIARGS((void));
EXTERN int           EZ_IsList  MY_ANSIARGS((int lnum));
#endif
/*
 * From EZ_Buffer.c
 */
#ifndef _EZ_BUFFERS_C_
EXTERN void                 EZ_LogicOp  MY_ANSIARGS((int op));
EXTERN void                 EZ_DrawBuffer MY_ANSIARGS((int buf));
EXTERN void                 EZ_SwapBuffers MY_ANSIARGS((void));
EXTERN void                 EZ_Clear MY_ANSIARGS((int flag));
EXTERN void                 EZ_ClearColorf MY_ANSIARGS((float r, float g, float b, float a));
EXTERN void                 EZ_ClearIndex MY_ANSIARGS((int idx));
EXTERN void                 EZ_SetBackBuffer MY_ANSIARGS((int buf));
EXTERN void                 EZ_AutoSelectBackBuffer MY_ANSIARGS((void));
EXTERN void                 EZ_SelectFastBackBuffer MY_ANSIARGS((void));
#endif

#ifndef _EZ_GLIMAGE_C_
EXTERN EZ_GLImage *EZ_CreateGLImage MY_ANSIARGS((EZ_Bitmap *pixmap));
EXTERN void        EZ_GetGLImageWindow MY_ANSIARGS((int x, int y, int w, int h, EZ_GLImage **imgRet));
EXTERN void        EZ_GetGLImageXWindow MY_ANSIARGS((int x, int y, int w, int h, EZ_GLImage **imgRet));
EXTERN void        EZ_FreeGLImage MY_ANSIARGS((EZ_GLImage *glimage));

EXTERN void EZ_PutGLImage MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
				       int Sh, float fx, float fy, float fz));
EXTERN void EZ_PutGLImageScreen MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
					     int Sh, float fx, float fy, float fz));
EXTERN void EZ_PutGLImageWindow MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
					     int Sh, int ix, int iy));
EXTERN void EZ_PutGLImageXWindow MY_ANSIARGS((EZ_GLImage *glimage, int Sx, int Sy, int Sw,
					     int Sh, int ix, int iy));
EXTERN int EZ_GetGLImageInfo MY_ANSIARGS((EZ_GLImage *image,
				   Pixmap *p_ret, Pixmap *s_ret, int *w, int *h));
EXTERN EZ_Bitmap *EZ_GetLabelPixmapFromGLImage MY_ANSIARGS((EZ_GLImage *image));
#endif

/*
 * From EZ_Enable.c
 */
#ifndef _EZ_ENABLE_C_
EXTERN void         EZ_Enable MY_ANSIARGS((int mode));
EXTERN void         EZ_Disable MY_ANSIARGS((int mode));
#endif

#ifndef _EZ_GL_ERROR_C_
void EZ_GLError MY_ANSIARGS((char *func, char *msg));
void EZ_GLFatal MY_ANSIARGS((char *func, char *msg));
#endif

/*******************************************************************
 ***                                                             ***
 ***                Forward Declarations.                        ***
 ***                                                             ***
 ******************************************************************/
#ifndef _EZ_WIDGET_C_
EXTERN  EZ_Widget            *EZ_CreateNewWidget MY_ANSIARGS((EZ_Widget *parent));
EXTERN  void                  EZ_DestroyWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_RemoveWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_DestroyWidgetReal MY_ANSIARGS((EZ_Widget *widget, int level));
EXTERN  void                  EZ_GetWidgetLabelPosition MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));
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
EXTERN  void                  EZ_GetWidgetGeometry MY_ANSIARGS((EZ_Widget *widget, int *x,int *y, int *w, int *h));
EXTERN  void                  EZ_GetWidgetAbsoluteGeometry MY_ANSIARGS((EZ_Widget *widget, int *x,int *y, int *w, int *h));
EXTERN  void                  EZ_GetWidgetMinDimension MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN  void                  EZ_DisableWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_EnableWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_DisableWidgetTree MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_EnableWidgetTree MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int                   EZ_SetWidgetLabelString MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN  char                  *EZ_GetWidgetLabelString MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int                   EZ_GetWidgetIntData MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                  EZ_GetWidgetClientData MY_ANSIARGS((EZ_Widget *widget, int *i, void **p));
EXTERN  void                  *EZ_GetWidgetPtrData MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int                   EZ_GetWidgetReturnedData MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int                   EZ_GetWidgetReturnData MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void                  EZ_SetWidgetIntData MY_ANSIARGS((EZ_Widget *widget, int idata));
EXTERN  void                  EZ_SetWidgetPtrData MY_ANSIARGS((EZ_Widget *widget, void *ptr));
EXTERN void                   EZ_SetWidgetClientData MY_ANSIARGS((EZ_Widget *widget, int i, void *p));
EXTERN  void                  EZ_HideWidget MY_ANSIARGS((EZ_Widget *widget));
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
EXTERN  void                  EZ_GetBackgroundPV MY_ANSIARGS((EZ_Widget *widget,
						      unsigned long *pv, 
						      int hlight, int specialhlight));
EXTERN  void                 EZ_GetWidgetTileOrigin MY_ANSIARGS((EZ_Widget *widget, int *x, int *y));
EXTERN  unsigned long        EZ_GetForegroundPV MY_ANSIARGS((EZ_Widget *widget));

EXTERN  int                  EZ_GetDarkBDGC MY_ANSIARGS((EZ_Widget *widget, GC *gc));
EXTERN  int                  EZ_GetBrightBDGC MY_ANSIARGS((EZ_Widget *widget, GC *gc));
EXTERN  int                  EZ_GetDarkBrightNormalBDGC MY_ANSIARGS((EZ_Widget *widget, GC *dk, GC *bt, GC *ngc));
EXTERN  void                 EZ_GetDarkBDpv MY_ANSIARGS((EZ_Widget *widget, unsigned long *pv));
EXTERN  void                 EZ_GetBrightBDpv MY_ANSIARGS((EZ_Widget *widget, unsigned long *pv));

EXTERN  int                   EZ_GetWidgetOrientation MY_ANSIARGS((EZ_Widget *widget));

EXTERN  void                  EZ_SetWidgetCallBack MY_ANSIARGS((EZ_Widget *widget, EZ_CallBack callback, void *data));
EXTERN  void                  EZ_AddWidgetCallBack MY_ANSIARGS((EZ_Widget *widget, int wlist,
								EZ_CallBack callback, void *data,int where));
EXTERN  void                  EZ_RemoveWidgetCallBack MY_ANSIARGS((EZ_Widget *widget, int wlist,
								   EZ_CallBack callback, void *data));
EXTERN  void                  EZ_RemoveAllWidgetCallBacks MY_ANSIARGS((EZ_Widget *widget, int wlist));

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
EXTERN EZ_LabelPixmap        *EZ_GetWidgetPixmap MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                  EZ_HighlightWidget MY_ANSIARGS((EZ_Widget *widget, int flag));
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
EXTERN  Window   EZ_GetWidgetWindow MY_ANSIARGS((EZ_Widget *widget));
EXTERN  int      EZ_GetDepth  MY_ANSIARGS((void));
EXTERN  Window  EZ_GetCommunicationWindow MY_ANSIARGS((void));
EXTERN  int     EZ_GetScreen MY_ANSIARGS((void));
EXTERN  void    EZ_GetDisplayInfo MY_ANSIARGS((Display **dpy, int *scrn, Visual **vis, int *depth));
EXTERN  void    EZ_GetDisplaySize MY_ANSIARGS((int *w, int *h));
EXTERN  void    EZ_SetApplicationName MY_ANSIARGS((char *name));
EXTERN  char    *EZ_GetApplicationName MY_ANSIARGS((void));
EXTERN  void    EZ_DisableHighlight MY_ANSIARGS((void));
EXTERN  void    EZ_EnableHighlight MY_ANSIARGS((void));
EXTERN  void    EZ_DisableLabelOffset MY_ANSIARGS((void));
EXTERN  void    EZ_EnableLabelOffset MY_ANSIARGS((void));
EXTERN  void    EZ_DisableSliderDepression MY_ANSIARGS((void));
EXTERN  void    EZ_EnableSliderDepression MY_ANSIARGS((void));
EXTERN  void    EZ_Shutdown MY_ANSIARGS((void));
EXTERN  void    EZ_DisableImageDithering MY_ANSIARGS((void));
EXTERN  void    EZ_EnableImageDithering MY_ANSIARGS((void));
EXTERN  void    EZ_AllowDualLabel  MY_ANSIARGS((int));
EXTERN  void    EZ_EnableBubbleHelp MY_ANSIARGS((void));
EXTERN  void    EZ_DisableBubbleHelp MY_ANSIARGS((void));

EXTERN void    EZ_FixFocusHighlightBorder MY_ANSIARGS((EZ_Widget *wptr, Drawable drawable,
                                                int w, int h, int padb));

EXTERN  void    EZ_SetClientMessageHandler MY_ANSIARGS((EZ_EventHandler handler, void *data ));
EXTERN  void    EZ_FlashWidgetWindowBorder MY_ANSIARGS((EZ_Widget *widget, Drawable drawable,
						int x, int y, int w,int h));
EXTERN  void    EZ_FlashWidgetBorder  MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void    EZ_FlashWidgetBorder2 MY_ANSIARGS((EZ_Widget *widget));
EXTERN  void    EZ_StartAsIcon MY_ANSIARGS((int));
EXTERN char    *EZ_LibraryVersion MY_ANSIARGS((void));
EXTERN void     EZ_GetVersionNumbers MY_ANSIARGS((int *mj, int *mi));
EXTERN EZ_Widget *EZ_GetBubbleHelp MY_ANSIARGS((void));
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
#endif

/*
 * From EZ_WidgetFrame.c
 */
#ifndef _EZ_WIDGET_FRAME_C_
EXTERN EZ_Widget         *EZ_CreateFrame MY_ANSIARGS((EZ_Widget *parent, char *label));
EXTERN int               EZ_OnFrameDragHandle MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN void              EZ_HighlightFrameDragHandle MY_ANSIARGS((EZ_Widget *widget, int onoff));
EXTERN int               EZ_DetachFrame MY_ANSIARGS((EZ_Widget *widget, int xoff, int yoff));
EXTERN int               EZ_DockFrame MY_ANSIARGS((EZ_Widget *widget, int force));
EXTERN int               EZ_XYIsAtCorner MY_ANSIARGS((int x,int y,int w,int h));
EXTERN int               EZ_FrameIsDetached MY_ANSIARGS((EZ_Widget *widget));

#endif

/*
 * From EZ_WidgetLabel.c
 */
#ifndef _EZ_WIDGET_LABEL_C_
EXTERN EZ_Widget  *EZ_CreateLabel MY_ANSIARGS((EZ_Widget *parent, char *label));
#endif
/*
 * From EZ_WidgetButtonN.c
 */
#ifndef _EZ_WIDGET_BUTTONN_C_
EXTERN EZ_Widget  *EZ_CreateButton MY_ANSIARGS((EZ_Widget *parent, char *label, int undline));
#endif

/*
 * From EZ_WidgetButtonC.c
 */
#ifndef _EZ_WIDGET_BUTTONC_C_
EXTERN EZ_Widget        *EZ_CreateCheckButton MY_ANSIARGS((EZ_Widget *parent,
							   char *label,
							   int underline,
							   int onvalue, int offvalue, int onff ));
EXTERN void             EZ_SetCheckButtonState MY_ANSIARGS((EZ_Widget *widget, int onoff));
EXTERN int              EZ_GetCheckButtonState MY_ANSIARGS((EZ_Widget *widget, int *value));
#endif
/*
 * From EZ_WidgetButtonR.c
 */
#ifndef _EZ_WIDGET_BUTTONR_C_
EXTERN EZ_Widget         *EZ_CreateRadioButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int ul, int gp, int val));
EXTERN void              EZ_SetRadioButtonGroupVariableValue MY_ANSIARGS((EZ_Widget *widget, int value));
EXTERN int               EZ_GetRadioButtonGroupVariableValue MY_ANSIARGS((EZ_Widget *widget));
EXTERN int               EZ_GetRadioButtonGroupID MY_ANSIARGS((EZ_Widget *widget));
#endif

/*
 * From EZ_WidgetButtonM.c
 */
#ifndef _EZ_WIDGET_BUTTONM_C_
EXTERN EZ_Widget  *EZ_CreateMenuButton MY_ANSIARGS((EZ_Widget *parent, char *label, int undline));
EXTERN void              EZ_SetMenuButtonMenu MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu));
EXTERN EZ_Widget         *EZ_GetMenuButtonMenu MY_ANSIARGS((EZ_Widget *widget));
EXTERN int               EZ_LookForPopupShortCuts MY_ANSIARGS((EZ_Widget *widget, int modfiers, 
							char *str, int *x, int *y, 
							EZ_Widget **item));
EXTERN int               EZ_MatchCharIgnoreCase MY_ANSIARGS((char c1, char c2));
#endif
/*
 * From EZ_WidgetSlider.c
 */
#ifndef _EZ_WIDGET_SLIDER_C_
EXTERN EZ_Widget   *EZ_CreateSlider  MY_ANSIARGS((EZ_Widget *parent, char *lab, 
					   float mnv, float mxv, float initv, int type));
EXTERN void         EZ_SetSliderValue  MY_ANSIARGS((EZ_Widget *widget, float value));
EXTERN float        EZ_GetSliderValue  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void         EZ_SetSliderRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));
#endif
/*
 * From EZ_WidgetMenuItems.c
 */
#ifndef _EZ_WIDGET_MENU_ITEMS_C_
EXTERN EZ_Widget        *EZ_CreateMenuSeparator MY_ANSIARGS((EZ_Widget *parent));
EXTERN EZ_Widget        *EZ_CreateMenuSubMenu MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl));
EXTERN EZ_Widget        *EZ_CreateMenuNormalButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl, int ret));
EXTERN EZ_Widget        *EZ_CreateMenuCheckButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl, 
							int onv, int offv, int onoff, int ret));
EXTERN EZ_Widget        *EZ_CreateMenuRadioButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl,
							int gid, int val, int ret));
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
#endif

/*
 * From EZ_WidgetDisp.c
 */
#ifndef _EZ_WIDGET_DISP_C_
EXTERN void                 EZ_DisplayWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                 EZ_DisplayWidgetUnderPointer MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN void                 EZ_ReDisplayWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN void                 EZ_DrawWidget MY_ANSIARGS((EZ_Widget *widget));
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
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromXBitmapFile MY_ANSIARGS((char *file));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromXBitmapData MY_ANSIARGS((char *file, unsigned int w, unsigned int h));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromImageFile MY_ANSIARGS((char *file));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromRawRGB MY_ANSIARGS((unsigned char *data, 
                                                                      int width, int height));
EXTERN EZ_Bitmap         *EZ_CreateLabelPixmapFromStaticRawRGB MY_ANSIARGS((unsigned char *data,
                                                                            int width, int height));

EXTERN EZ_Bitmap         *EZ_GetPixmapFromRawRGB  MY_ANSIARGS((unsigned char  *data, int width, int height));
EXTERN EZ_Bitmap         *EZ_GetPixmapFromStaticRawRGB  MY_ANSIARGS((unsigned char  *data, int width, int height));
EXTERN EZ_Bitmap         *EZ_GetPixmapFromPixmap  MY_ANSIARGS((Pixmap pix, int x, int y, int w, int h));
EXTERN EZ_Bitmap         *EZ_GetPixmapFromPermPixmap  MY_ANSIARGS((Pixmap pix, int x, int y, int w, int h));
EXTERN void              EZ_FreeLabelPixmap MY_ANSIARGS((EZ_Bitmap *bitmap));
EXTERN void              EZ_FreeXPixmap MY_ANSIARGS((Pixmap pixmap));
EXTERN void              EZ_FreeBitmap  MY_ANSIARGS((EZ_Bitmap *bitmap));
EXTERN int               EZ_GetLabelPixmapInfo MY_ANSIARGS((EZ_Bitmap *bitmap,Pixmap *p_ret, 
							    Pixmap *s_ret, int *w, int *h));

EXTERN EZ_Widget         *EZ_WindowIsWidgetWindow  MY_ANSIARGS((Window window));
EXTERN EZ_Widget         *EZ_WidgetExist  MY_ANSIARGS((EZ_Widget *widget));
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

EXTERN void              EZ_GetDarkBrightHighlightPixmap MY_ANSIARGS((EZ_Bitmap *bitmap,
                                      Pixmap *dk, Pixmap *bt, Pixmap *ht,Pixmap *sp));
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
EXTERN int               EZ_HashString  MY_ANSIARGS((char *str, int size));
#endif
/*
 * From EZ_WidgetPopup.c
 */
#ifndef _EZ_WIDGET_POPUP_C_
EXTERN EZ_Widget         *EZ_CreatePopupMenu MY_ANSIARGS((char *title));
EXTERN EZ_Widget         *EZ_CreateMenu MY_ANSIARGS((char *title));
EXTERN EZ_Widget         *EZ_CreateMenu MY_ANSIARGS((char *title));
EXTERN EZ_Widget         *EZ_GetPopupMenuTitleWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN int               EZ_DoPopup MY_ANSIARGS((EZ_Widget *widget, int where));
EXTERN int               EZ_DoPopupAt MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN int               EZ_DoPopupX MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu, int where));
EXTERN int               EZ_DoPopupAtX MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *menu, int x, int y));
EXTERN EZ_Widget         *EZ_GetSelectedMenuItem MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget         *EZ_CreateMenuFromList MY_ANSIARGS((EZ_MenuEntry *list));
#endif

/*
 * From EZ_WidgetInput.c
 */
#ifndef _EZ_WIDGET_INPUT_C_
EXTERN EZ_Widget        *EZ_CreateEntry MY_ANSIARGS((EZ_Widget *parent, char *init_str));
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
EXTERN int    EZ_EntryIsModified MY_ANSIARGS((EZ_Widget *widget));
#endif

/*
 * From EZ_Widget3DCanvas.c
 */

#ifndef _EZ_WIDGET_3D_CANVAS_C_
EXTERN EZ_Widget       *EZ_Create3DCanvas MY_ANSIARGS((EZ_Widget *parent));
EXTERN void             EZ_Set3DCanvas MY_ANSIARGS(( EZ_Widget *widget));
EXTERN void             EZ_Configure3DCanvas MY_ANSIARGS(( EZ_Widget *widget));
EXTERN void             EZ_Set3DCanvasEventHandle MY_ANSIARGS((EZ_Widget *widget, EZ_EventHandler handler, void *data));
EXTERN void             EZ_Save3DCanvas2PPMImage MY_ANSIARGS((EZ_Widget *widget, char *fname));
EXTERN void             EZ_Save3DCanvas2PSA MY_ANSIARGS((EZ_Widget *widget, int mode,
						  int dpi, float scale, char *fname));
EXTERN void             EZ_Save3DCanvas2PS MY_ANSIARGS((EZ_Widget *widget, char *fname));
EXTERN XImage           *EZ_Read3DCanvas2XImage MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_Get3DCanvasSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
EXTERN void             EZ_GLConfig MY_ANSIARGS((void));
EXTERN void             EZ_PSXImage MY_ANSIARGS((XImage *image, int w, int h, FILE *fp));
EXTERN void             EZ_PPMXImage MY_ANSIARGS((XImage *image, int w, int h, FILE *fp));
#endif

/*
 * From EZ_WidgetIListBox.c
 */
#ifndef _EZ_WIDGET_ILIST_BOX_C_
EXTERN EZ_Widget        *EZ_CreateIListBox MY_ANSIARGS((EZ_Widget *parent));
EXTERN void             EZ_SetIListBoxData MY_ANSIARGS((EZ_Widget *widget, char **items, int nitems));
EXTERN void             EZ_AppendIListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item));
EXTERN void             EZ_InsertIListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item, int where));
EXTERN void             EZ_DeleteIListBoxItem MY_ANSIARGS((EZ_Widget *widget, int idx));
EXTERN void             EZ_DeleteIListBoxString MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void             EZ_ClearIListBox MY_ANSIARGS((EZ_Widget *widget));
EXTERN char             *EZ_GetIListBoxSelectedItem MY_ANSIARGS((EZ_Widget *widget));
EXTERN char             *EZ_GetIListBoxSelection MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ModifyIListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item, int idx));
EXTERN void             EZ_ListBoxSelectItem MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void             EZ_ListBoxSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int idx));
#endif
/*
 * From EZ_WidgetListBox.c
 */
#ifndef _EZ_WIDGET_LIST_BOX_C_
EXTERN EZ_Widget        *EZ_CreateListBox MY_ANSIARGS((EZ_Widget *parent, int hsc, int vsc));
EXTERN void             EZ_ClearListBox MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_SetListBoxItems MY_ANSIARGS((EZ_Widget *widget, char **items, int nitems));
EXTERN void             EZ_AppendListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *item));
EXTERN void             EZ_InsertListBoxItem MY_ANSIARGS((EZ_Widget *widget, char *str, int idx));
EXTERN void             EZ_DeleteListBoxItem MY_ANSIARGS((EZ_Widget *widget, int idx));
EXTERN void             EZ_DeleteListBoxString MY_ANSIARGS((EZ_Widget *widget, char *str));
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
EXTERN void             EZ_GetListBoxState  MY_ANSIARGS((EZ_Widget *widget, int *xoff, int *fl)); 
EXTERN void             EZ_SetListBoxState  MY_ANSIARGS((EZ_Widget *widget, int xoff, int fl)); 

EXTERN void      EZ_GetListBoxData MY_ANSIARGS((EZ_Widget *widget, EZ_ListBoxEntry **dat, int *cnt));
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
EXTERN void             EZ_DoScroll   MY_ANSIARGS((EZ_Widget *widget, int offset, int flag));
#endif
/*
 * From EZ_WidgetIToplevel.c
 */
#ifndef _EZ_WIDGET_ITOPLEVEL_C_
EXTERN EZ_Widget       *EZ_CreateFreeLabel MY_ANSIARGS((EZ_Widget *widget, char *label));
EXTERN EZ_Widget       *EZ_CreateIcon MY_ANSIARGS((void));
EXTERN int             EZ_GetIconTextGeometry MY_ANSIARGS((EZ_Widget *icon, int *x, int *y,
                                                           int *w, int *h));
EXTERN void            EZ_ConstrainFreeLabel MY_ANSIARGS((EZ_Widget *widget, int flag));
#endif

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
EXTERN int            EZ_LoadXFont MY_ANSIARGS((char *fname));
EXTERN void           EZ_SetXFont MY_ANSIARGS((XFontStruct **ptr, int id));
EXTERN char           *EZ_GetFontNameFromId MY_ANSIARGS((int id));
EXTERN char           *EZ_GetFontName MY_ANSIARGS((XFontStruct *fptr));
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
 * From EZ_WidgetFile.c
 */
#ifndef _EZ_WIDGET_FILE_C_
EXTERN int  EZ_FileReadable  MY_ANSIARGS((char *name));
#endif

/*
 * From EZ_WidgetMisc.c
 */
#ifndef _EZ_WIDGET_MISC_C_
EXTERN EZ_Widget *EZ_GetToplevelWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_CallWidgetCallbacks  MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_CallWidgetMotionCallbacks MY_ANSIARGS((EZ_Widget *widget));
EXTERN Window EZ_FindClientWindow  MY_ANSIARGS((Window win));
EXTERN void EZ_SlideOpenWidgetRelativeTo MY_ANSIARGS((EZ_Widget *w, EZ_Widget *r, int dir, int sp));
EXTERN void EZ_SlideOpenWidget MY_ANSIARGS((EZ_Widget *w, int x, int y, int dir, int sp));
EXTERN void EZ_SlideCloseWidget MY_ANSIARGS((EZ_Widget *w, int dir, int sp));

#endif

/*
 * From EZ_Misc.c
 */
#ifndef _EZ_MISC_C_
EXTERN void *EZ_Malloc MY_ANSIARGS((int nbytes));
EXTERN void *EZ_Realloc MY_ANSIARGS((void *ptr, int nbytes));
EXTERN char *EZ_AllocCopyString    MY_ANSIARGS((char *str));
EXTERN char *EZ_ReallocCopyString  MY_ANSIARGS((char *ptr, char *str));
EXTERN void EZ_Free MY_ANSIARGS((void *ptr));
EXTERN void  EZ_SitForALittleBit   MY_ANSIARGS((unsigned long length));
EXTERN void  EZ_OutOfMemory        MY_ANSIARGS((char *msg));
#endif

/*
 * From EZ_WidgetImage.c
 */
#ifndef _EZ_WIDGET_IMAGE_C_
EXTERN int  EZ_CreateXPixmapFromImageFile MY_ANSIARGS((char *file, unsigned int *w, 
							unsigned int *h, Pixmap *pix));
EXTERN int  EZ_GetImageFileType MY_ANSIARGS((char *file));
EXTERN EZ_XImage *EZ_CreateXImage MY_ANSIARGS((int w, int h, int shmflag));
EXTERN void  EZ_DestroyXImage  MY_ANSIARGS((EZ_XImage *image));
EXTERN int   EZ_ReadImageFile MY_ANSIARGS(( char *file, int *width_return, int *height_return, unsigned char **rgb_return));
#endif
/*
 * From EZ_X11Init.c
 */
#ifndef _EZ_X11INIT_C_
EXTERN int  EZ_XErrorHandler MY_ANSIARGS((Display *dpy, XErrorEvent *error));
 EXTERN void EZ_ParseGeometry MY_ANSIARGS((char *spec, int *bits, int *x, int *y, int *w, int *h));
#endif
/*
 * From EZ_WidgetIText.c
 */
#ifndef _EZ_WIDGET_ITEXT_C_
EXTERN EZ_Widget        *EZ_CreateIText MY_ANSIARGS((EZ_Widget *widget, int editable));

EXTERN int              EZ_ITextGetPosition MY_ANSIARGS((EZ_Widget *widget));
EXTERN void             EZ_ITextGotoPosition MY_ANSIARGS((EZ_Widget *widget, int pos));
EXTERN int              EZ_ITextSetMarker MY_ANSIARGS((EZ_Widget *widget));
EXTERN char             *EZ_ITextGetRegionString MY_ANSIARGS((EZ_Widget *widget));
EXTERN char             *EZ_ITextGetBufferString  MY_ANSIARGS((EZ_Widget *widget));
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
EXTERN void             EZ_TextInsertAnnotatedString MY_ANSIARGS((EZ_Widget *widget, char *str));
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

EXTERN int EZ_DecodeTextProperty MY_ANSIARGS((EZ_TextProp *tp, unsigned long *fg,
                              unsigned long *bg, int *font, int *ul, EZ_Bitmap **bmap));
#endif

/*
 * From EZ_WidgetNBPage.c
 */
#ifndef _EZ_WIDGET_NBPAGE_C_
EXTERN EZ_Widget   *EZ_CreateNoteBookPage  MY_ANSIARGS((EZ_Widget *parent, char *lab, int udl));
EXTERN void        EZ_ActivateNoteBookPage MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget   *EZ_GetNoteBookPageTab MY_ANSIARGS((EZ_Widget *widget));
#endif
/*
 * From EZ_WidgetNB.c
 */
#ifndef _EZ_WIDGET_NB_C_
EXTERN EZ_Widget   *EZ_CreateNoteBook  MY_ANSIARGS((EZ_Widget *parent));
EXTERN int         EZ_SetNoteBookOrientation MY_ANSIARGS((EZ_Widget *widget, int value));
#endif


/*
 * From EZ_WidgetPaneHandle.c
 */
#ifndef _EZ_WIDGET_PANE_HANDLE_C_
EXTERN EZ_Widget       *EZ_CreatePaneHandle MY_ANSIARGS((EZ_Widget *parent));
#endif
							
/*
 * From EZ_WidgetGrab.c
 */
#ifndef _EZ_WIDGET_GRAB_C_
EXTERN void            EZ_SetGrab MY_ANSIARGS((EZ_Widget *widget));
EXTERN void            EZ_ReleaseGrab MY_ANSIARGS((void));
EXTERN int             EZ_WidgetIsViewable MY_ANSIARGS((EZ_Widget *widget));
EXTERN void            EZ_RedisplayWidgetTree MY_ANSIARGS((EZ_Widget *widget));
EXTERN int             EZ_GrabPointer MY_ANSIARGS((EZ_Widget *widget, Cursor csr, int tout));
EXTERN void            EZ_UngrabPointer MY_ANSIARGS((void));
EXTERN void            EZ_BlinkWidget  MY_ANSIARGS((EZ_Widget *widget, int cnt));
EXTERN void            EZ_RaiseWidgetWindow MY_ANSIARGS((EZ_Widget *widget));
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
EXTERN void        EZ_RestartTimer  MY_ANSIARGS((EZ_Timer *timer));
EXTERN EZ_Timer    *EZ_ResetTimer MY_ANSIARGS((EZ_Timer *timer, long sec, long usec,int repeat,
					EZ_CallBack callback, void *pdata, int idata));

EXTERN void        EZ_GetTimerState MY_ANSIARGS((EZ_Timer *timer, long *sec, long *usec,int *repeat));
EXTERN void        EZ_CancelTimer MY_ANSIARGS((EZ_Timer *timer));
EXTERN void        EZ_DestroyAllTimers  MY_ANSIARGS((void));

EXTERN void        EZ_SetTimerPtrData  MY_ANSIARGS((EZ_Timer *timer, void *ptr));
EXTERN void        EZ_SetTimerIntData  MY_ANSIARGS((EZ_Timer *timer, int data));
EXTERN void        EZ_SetTimerClientData  MY_ANSIARGS((EZ_Timer *timer, void *ptr, int idata));      
EXTERN void        EZ_SetTimerCallBack  MY_ANSIARGS((EZ_Timer *timer, EZ_CallBack callback, void *data));
EXTERN void        *EZ_GetTimerPtrData      MY_ANSIARGS((EZ_Timer *timer));
EXTERN int         EZ_GetTimerIntData MY_ANSIARGS((EZ_Timer *timer));
EXTERN void        EZ_GetTimerClientData  MY_ANSIARGS((EZ_Timer *timer, void **ptr, int *idata));      
#endif

/*
 * From EZ_Comm.c
 */
#ifndef _EZ_COMM_C_
EXTERN int           EZ_WindowExist  MY_ANSIARGS((Window window));
EXTERN unsigned long EZ_WindowIsDnDTarget  MY_ANSIARGS((Window window));
EXTERN unsigned long EZ_WindowIsDnDSrc  MY_ANSIARGS((Window window));
EXTERN Window        *EZ_GetAllEZWGLClients MY_ANSIARGS((void));
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
EXTERN void              EZ_SetOptionalEntryOptions MY_ANSIARGS((EZ_Widget *widget, char **items, int count));
EXTERN void              EZ_GetOptionalEntryWidgetComponents MY_ANSIARGS((EZ_Widget *widget, 
								    EZ_Widget **entry, EZ_Widget **listbox, 
								    EZ_Widget **btn  ));
EXTERN void  EZ_OptionalEntryClearOptions     MY_ANSIARGS((EZ_Widget *widget));
EXTERN void  EZ_OptionalEntryAppendOption     MY_ANSIARGS((EZ_Widget *widget, char *item));
EXTERN void  EZ_OptionalEntryInsertOption MY_ANSIARGS((EZ_Widget *widget, char *str, int idx));
EXTERN void  EZ_OptionalEntryDeleteOption     MY_ANSIARGS((EZ_Widget *widget, char *str));
EXTERN void  EZ_SetOptionalEntryStyle         MY_ANSIARGS((EZ_Widget *widget, int style));
EXTERN void  EZ_OptionalEntryInsertListOption MY_ANSIARGS((EZ_Widget *widget, char *str, int idx));
EXTERN void  EZ_SetOptionalEntryOptionalItems MY_ANSIARGS((EZ_Widget *widget, char **items, int count));
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
EXTERN int              position_of_last_slash_before_glob_chars MY_ANSIARGS((char *str));
EXTERN int              EZ_CompareStr MY_ANSIARGS((CONST void *ptr1, CONST void *ptr2));
EXTERN int              EZ_CmpStr MY_ANSIARGS((CONST void *ptr1, CONST void *ptr2));
EXTERN int              EZ_CountSetFilePatterns MY_ANSIARGS((char *pat, char **patterns));
#endif

/*
 * From EZ_WidgetITree.c
 */
#ifndef _EZ_WIDGET_ITREE_C_
EXTERN EZ_Widget  *EZ_CreateITree  MY_ANSIARGS((EZ_Widget *parent));
EXTERN EZ_TreeNode *EZ_FindPointerOnTreeNode MY_ANSIARGS((int x, int y, EZ_TreeNode *root));
EXTERN EZ_TreeNode *EZ_FindTreeAdjBranch MY_ANSIARGS((EZ_TreeNode *root, EZ_TreeNode *node, int dir));
EXTERN void        EZ_SetITreeWTree MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *tree));
EXTERN void        EZ_FlashItemOutline MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, 
					     int Rx, int Ry, int Rw, int Rh));
EXTERN void        EZ_DnDOutlineItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, 
					     int Rx, int Ry, int Rw, int Rh));
EXTERN void        EZ_ITreeSelectNode MY_ANSIARGS((EZ_Widget *widget, EZ_TreeNode *theNode, int *location));
EXTERN void        EZ_ITreeSelectNodeUsingItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *theItem, int *location));
#endif
/*
 * From EZ_WidgetTree.c
 */
#ifndef _EZ_WIDGET_TREE_C_
EXTERN EZ_Widget  *EZ_CreateListTree MY_ANSIARGS((EZ_Widget *parent, int hsc, int vsc));
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
EXTERN void             EZ_SetIFListBoxData MY_ANSIARGS((EZ_Widget *widget, EZ_Item **items, int row, int col));
EXTERN void             EZ_InsertIFListBoxRow  MY_ANSIARGS((EZ_Widget *widget, EZ_Item **items, int nitems, int where));
EXTERN void             EZ_DeleteIFListBoxRow  MY_ANSIARGS((EZ_Widget *widget, int idx));
EXTERN void             EZ_DeleteIFListBoxSomeRows MY_ANSIARGS((EZ_Widget *widget, int *idces, int cnt));
EXTERN void             EZ_ReplaceIFListBoxItem  MY_ANSIARGS((EZ_Widget *widget, EZ_Item *old, EZ_Item *newitem));
EXTERN void             EZ_ReplaceIFListBoxItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int row, int col, EZ_Item *newitem));
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

#endif
/*
 * From EZ_WidgetFListBox.c
 */
#ifndef _EZ_WIDGET_FLIST_BOX_C_
EXTERN EZ_Widget  *EZ_CreateFancyListBox MY_ANSIARGS((EZ_Widget *parent,
					       int hsc, int vsc, int ncol, int selctype));
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
EXTERN void       EZ_FancyListBoxReplaceItem  MY_ANSIARGS((EZ_Widget *widget, EZ_Item *old, EZ_Item *newitem));
EXTERN void       EZ_FancyListBoxReplaceItemUsingIdx  MY_ANSIARGS((EZ_Widget *widget, int row, int col, EZ_Item *newitem));
EXTERN void       EZ_FancyListBoxSelectItemUsingIdx MY_ANSIARGS((EZ_Widget *widget, int Srow, int Scol, int *location));
EXTERN void       EZ_FancyListBoxSelectItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int *location));
EXTERN int        EZ_GetFancyListBoxNumberOfRows MY_ANSIARGS((EZ_Widget *widget));
EXTERN int        EZ_GetFancyListBoxNumberOfColumns MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Item    ***EZ_GetFancyListBoxData  MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Item    **EZ_FancyListBoxFindRowAtXY  MY_ANSIARGS((EZ_Widget *fl, int x, int y, int *r_ret));
EXTERN EZ_Item    *EZ_FancyListBoxFindItemAtXY  MY_ANSIARGS((EZ_Widget *fl, int x, int y, int *r_ret, int *c_ret));

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

EXTERN void    EZ_GetItemXYOffset MY_ANSIARGS((EZ_Item *item, int *xoff, int *yoff));
EXTERN void    EZ_GetItemBwAndXYPading MY_ANSIARGS((EZ_Item *item, int *bw, int *xpad, int *ypad));

EXTERN void    EZ_GetItemClientData MY_ANSIARGS((EZ_Item *item,
					  int *idata_ret,
					  void **pdata_ret));
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
EXTERN EZ_Widget *EZ_GetItemHostWidget MY_ANSIARGS((EZ_Item *item));
EXTERN void       EZ_UpdateDisplayItem  MY_ANSIARGS((EZ_Item *item));

EXTERN int     EZ_GetItemUnknownData MY_ANSIARGS((EZ_Item *item, int idx,
						  EZ_UnknownDataType *ret));
EXTERN void    EZ_SetItemUnknownData  MY_ANSIARGS((EZ_Item *item, int idx,
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
EXTERN EZ_LabelPixmap   *EZ_GetLabelItemPixmap MY_ANSIARGS((EZ_Item *item));
#endif

#ifndef _EZ_ITEM_FIG_C_
EXTERN void   EZ_FreezeFigItem MY_ANSIARGS((EZ_Item *item));
EXTERN void   EZ_UnFreezeFigItem MY_ANSIARGS((EZ_Item *item));

EXTERN void   EZ_GetFigPieceBoundingBox MY_ANSIARGS((EZ_FigPiece *fpiece,
						     int *x1, int *y1, int *x2, int *y2));
EXTERN GC     EZ_GetGC MY_ANSIARGS((unsigned long mask, XGCValues *gcv));
EXTERN void   EZ_FreeGC MY_ANSIARGS((GC gc));
EXTERN EZ_Item    *EZ_CreateFigItem MY_ANSIARGS((void));
EXTERN void       EZ_FigItemHidePiece MY_ANSIARGS((EZ_Item *item, EZ_FigPiece *fpiece, int upd));
EXTERN void       EZ_FigItemShowPiece MY_ANSIARGS((EZ_Item *item, EZ_FigPiece *fpiece, int upd));
EXTERN void       EZ_FigItemDeletePiece MY_ANSIARGS((EZ_Item *item, EZ_FigPiece *fpiece, int upd));
EXTERN void       EZ_FigItemDeleteAllPieces MY_ANSIARGS((EZ_Item *item, int upd));
EXTERN void       EZ_ScaleFigItem MY_ANSIARGS((EZ_Item *item, float sx, float sy, int mode));
EXTERN EZ_FigPiece *EZ_FigItemAddPiece MY_ANSIARGS((EZ_Item *fitem, int ptype,
						    GC theGc,
						    int x, int y, int width, int height, int other,
						    int npoints, XPoint *xpts, int mode));
EXTERN EZ_FigPiece *EZ_FigItemAddString MY_ANSIARGS((EZ_Item *fitem, GC theGc,
					    char *str, int slen, int justification, int x, int y));
EXTERN EZ_FigPiece *EZ_FigItemAddPixmap MY_ANSIARGS((EZ_Item *fitem, GC theGc,
				       EZ_Bitmap *bmp, int sx, int sy, int sw, int sh, int dx, int dy));

EXTERN EZ_FigPiece *EZ_FigItemAddPoints MY_ANSIARGS((EZ_Item *fitem, GC theGc,
				       XPoint *pts, int npts));

EXTERN EZ_FigPiece *EZ_FigItemAddLines  MY_ANSIARGS((EZ_Item *fitem, GC theGc,
				      XPoint *pts, int npts, int mode));

EXTERN EZ_FigPiece *EZ_FigItemAddPolygon MY_ANSIARGS((EZ_Item *fitem, GC theGc,
				      XPoint *pts, int npts, int mode));

EXTERN EZ_FigPiece *EZ_FigItemAddRectangle MY_ANSIARGS((EZ_Item *fitem, GC theGc,
					 int x, int y, int w, int h));

EXTERN EZ_FigPiece *EZ_FigItemAddFilledRectangle MY_ANSIARGS((EZ_Item *fitem, GC theGc,
					       int x, int y, int w, int h));

EXTERN EZ_FigPiece *EZ_FigItemAddArc MY_ANSIARGS((EZ_Item *fitem, GC theGc,
				   int x, int y, int w, int h, int a1, int a2));

EXTERN EZ_FigPiece *EZ_FigItemAddFilledArc MY_ANSIARGS((EZ_Item *fitem, GC theGc,
					 int x, int y, int w, int h, int a1, int a2));

EXTERN EZ_FigPiece *EZ_FigItemAddRectangles MY_ANSIARGS((EZ_Item *fitem, GC theGc,
					  XRectangle *rects, int nrects));

EXTERN EZ_FigPiece *EZ_FigItemAddFilledRectangles MY_ANSIARGS((EZ_Item *fitem, GC theGc,
						XRectangle *rects, int nrects));

EXTERN EZ_FigPiece *EZ_FigItemAddArcs MY_ANSIARGS((EZ_Item *fitem, GC theGc,
				    XArc *arcs, int narcs));

EXTERN EZ_FigPiece *EZ_FigItemAddFilledArcs MY_ANSIARGS((EZ_Item *fitem, GC theGc,
							 XArc *arcs, int narcs));

EXTERN EZ_FigPiece *EZ_FigItemAddSegments MY_ANSIARGS((EZ_Item *fitem, GC theGc,
						       XSegment *segs, int nsegs));
EXTERN Pixmap EZ_MakePixmapFromFigItem MY_ANSIARGS((EZ_Item *fitem, int *width, int *height));
#endif

#ifndef _EZ_ITEM_UDF_C_
EXTERN EZ_Item *EZ_CreateUDFItem MY_ANSIARGS((int type));
EXTERN void    EZ_SetupUDFItem MY_ANSIARGS((EZ_Item *item));
EXTERN int     EZ_RegisterItem MY_ANSIARGS((char *iname, char *cname,
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
EXTERN void     EZ_TreeSetFGColorName  MY_ANSIARGS((EZ_TreeNode *node, char *clr));

EXTERN void     EZ_TreeSetBGColor MY_ANSIARGS((EZ_TreeNode *node, unsigned long pv));
EXTERN void     EZ_TreeSetBGColorName MY_ANSIARGS((EZ_TreeNode *node, char *clr));

EXTERN void     EZ_ComputeSubtreeSize MY_ANSIARGS((EZ_TreeNode *node, int *w, int *h, int *p));

EXTERN void     EZ_DisplayTreeInDrawable MY_ANSIARGS((EZ_Widget *widget, Drawable drawable,
					       int x0, int y0,
					       EZ_TreeNode *node,
						      int Rx, int Ry, int Rw, int Rh));

EXTERN EZ_Item *EZ_TreeNodeGetItem MY_ANSIARGS((EZ_TreeNode *node));
EXTERN int      EZ_UpdateDirTreeWidget MY_ANSIARGS((EZ_Widget *widget));
EXTERN void     EZ_TreeNodeMarkTerminal MY_ANSIARGS((EZ_TreeNode *node, int flag));
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
EXTERN void       EZ_OpenCloseDirTreeDirectoryNode MY_ANSIARGS((EZ_Item *item, int col));
EXTERN char       *EZ_GetDirTreeNodeFullPath MY_ANSIARGS((EZ_TreeNode *node));
EXTERN void        EZ_SetDirTreeDefaultPixmaps MY_ANSIARGS((EZ_Bitmap *o, EZ_Bitmap *c, EZ_Bitmap *f));
EXTERN void        EZ_SetDirTreeDefaultPixmapsFromXpmFiles MY_ANSIARGS((char *o, char *c, char *f));
EXTERN int         EZ_UpdateDirTreeWork MY_ANSIARGS((EZ_TreeNode *root,EZ_TreeNode *sel,int *flag));
EXTERN void        EZ_SetDirTreeSorter MY_ANSIARGS((EZ_TreeNode *root, int (*cmp)MY_ANSIARGS((void *, void *)) ));
#endif
/*
 * From EZ_WidgetIWorkArea.c
 */
#ifndef _EZ_WIDGET_IWORK_AREA_C_
EXTERN EZ_Widget        *EZ_CreateIWorkArea MY_ANSIARGS((EZ_Widget *parent));

EXTERN void             EZ_GrowIWorkAreaDataSpace MY_ANSIARGS((EZ_Widget *widget, int increment));

EXTERN EZ_Item          *EZ_FindPointerOnItemIWA MY_ANSIARGS((EZ_Item **data, int count, EZ_Item *guess,
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
EXTERN void              EZ_WorkAreaRaiseItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item));

EXTERN void              EZ_WorkAreaInsertAndDrawItemAt MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, 
								     int *location, int disp));
EXTERN void EZ_WorkAreaDrawItemAt MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int x, int y));
EXTERN void EZ_WorkAreaScaleItem MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, int mode,
					      float sx, float sy, int tx, int ty, int disp));
#endif
/*
 * From EZ_WidgetWorkArea.c
 */
#ifndef _EZ_WIDGET_WORK_AREA_C_
EXTERN EZ_Widget        *EZ_CreateWorkArea MY_ANSIARGS((EZ_Widget *parent, int hsc, int vsc));
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

EXTERN void              EZ_WorkAreaInsertItemBefore MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, EZ_Item *newitem));
EXTERN void              EZ_WorkAreaInsertItemAfter  MY_ANSIARGS((EZ_Widget *widget, EZ_Item *item, EZ_Item *newitem));
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

EXTERN EZ_UnknownDataType *EZ_GetRawXWinPrivateData MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_UnknownDataType *EZ_GetUDFWidgetPrivateData MY_ANSIARGS((EZ_Widget *widget));
EXTERN GC   EZ_GetWritableGC MY_ANSIARGS((void));
EXTERN GC   EZ_GetXorGC MY_ANSIARGS((void));
EXTERN GC   EZ_GetDnDXorGC MY_ANSIARGS((void));
EXTERN GC   EZ_GetDnDDragGC MY_ANSIARGS((void));
EXTERN GC   EZ_GetDashedLineGC MY_ANSIARGS((void));
#endif

/*
 * From EZ_DnDMsg.c
 */
#ifndef _EZ_DND_MSG_C_
EXTERN void EZ_GetDnDDropLocation MY_ANSIARGS((int *x, int *y, int *xx, int *yy));
EXTERN void EZ_GetDnDDragSrcXYOffset MY_ANSIARGS((int *x, int *y));
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

#ifndef _EZ_DND_C_
EXTERN Atom EZ_GetAtom  MY_ANSIARGS((char *name));
EXTERN void EZ_DnDGetDragConversionTargets MY_ANSIARGS((Atom **targets_ret, int **ntargets_ret, 
							unsigned int **t));
EXTERN void EZ_DnDGetMatchedConversionTargets MY_ANSIARGS((Atom **targets_ret, int **ntargets_ret, 
							   unsigned int **t));
EXTERN void EZ_SetDragIcon MY_ANSIARGS((EZ_Widget *widget, int dpyStyle));
EXTERN void EZ_SetDragCursor MY_ANSIARGS((Cursor cursor));
EXTERN void EZ_ShowDragIcon MY_ANSIARGS((int x, int y));
EXTERN void EZ_HandleDnDEvents MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN void EZ_SelectInput MY_ANSIARGS((Window win, long mask));
#endif

#ifndef _EZ_INPUT_C_
EXTERN void         EZ_InitializeInputIdList  MY_ANSIARGS((void));
EXTERN void         EZ_DestroyInputIdList  MY_ANSIARGS((void));
EXTERN EZ_Input     *EZ_AddInput MY_ANSIARGS((int fd, int mask, 
				       EZ_InputCallBack callback,
				       void *data));
EXTERN void         EZ_RemoveInput MY_ANSIARGS((EZ_Input *id));
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

#ifndef _EZ_WIDGET_TEAR_OFF_C_
EXTERN EZ_Widget    *EZ_GetMenuCompanion  MY_ANSIARGS((EZ_Widget *item));
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
				   char *msg,  char *color));
EXTERN void  EZ_RemoveLedAction MY_ANSIARGS((EZ_Widget *led, int action));
EXTERN void  EZ_SetLedColorFunction MY_ANSIARGS((EZ_Widget *led, 
				 unsigned long (*colorf)(void *w, int x, int y) ));
EXTERN void  EZ_SetLedString MY_ANSIARGS((EZ_Widget *parent, char *str, char *clr));
EXTERN void  EZ_ClearLed  MY_ANSIARGS((EZ_Widget *lcd));
EXTERN void  EZ_SetLedForeground  MY_ANSIARGS((EZ_Widget *lcd, char *clr));
EXTERN void  EZ_SetLedBackground  MY_ANSIARGS((EZ_Widget *lcd, char *clr));
EXTERN void  EZ_SetLedOffPixelColor  MY_ANSIARGS((EZ_Widget *lcd, char *clr));
EXTERN void  EZ_SetLedFont  MY_ANSIARGS((EZ_Widget *lcd, char *font));
EXTERN void  EZ_SetLedWidth  MY_ANSIARGS((EZ_Widget *lcd, int w));
EXTERN void  EZ_SetLedHeight  MY_ANSIARGS((EZ_Widget *lcd, int h));
EXTERN void  EZ_SetLedSize    MY_ANSIARGS((EZ_Widget *lcd, int w, int h));
EXTERN void  EZ_SetLedPixelSize  MY_ANSIARGS((EZ_Widget *led, int s, int t));
EXTERN void  EZ_GetLedGeometry  MY_ANSIARGS((EZ_Widget *led, int *w, int *h, int *pw, int *ph));
EXTERN EZ_Widget  *EZ_CreateLed MY_ANSIARGS((EZ_Widget *parent));
EXTERN int   EZ_LedIsFree  MY_ANSIARGS((EZ_Widget *lcd));
EXTERN char  *EZ_GetLedString MY_ANSIARGS((EZ_Widget *led));
EXTERN void  EZ_OnOffLedPixel MY_ANSIARGS((EZ_Widget *lcd, int onff, int x, int y, char *clr));
EXTERN void  EZ_LedLoadBitmap MY_ANSIARGS((EZ_Widget *wptr, unsigned char *data,
                                    int w, int h, int x0, int y0));
EXTERN void  EZ_LedLoadPixelMatrix MY_ANSIARGS((EZ_Widget *wptr, unsigned long *data,
                                         int w, int h, int x0, int y0, int d));
EXTERN void  EZ_LedLoadOnOffMatrix MY_ANSIARGS((EZ_Widget *wptr, char *data,
                                         int w, int h, int x0, int y0, int d));
EXTERN int  EZ_GetLedSelection  MY_ANSIARGS((EZ_Widget *lcd, int *x, int *y));
EXTERN void EZ_LedWindow2Led    MY_ANSIARGS((EZ_Widget *lcd, int wx, int wy, int *lx, int *ly));
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
EXTERN EZ_Widget   *EZ_CreateMBar MY_ANSIARGS((EZ_Widget *parent));

EXTERN EZ_Widget   *EZ_MenuBarAddItemAndMenu MY_ANSIARGS((EZ_Widget *widget, char *label, int ul, EZ_Widget *menu));
EXTERN void         EZ_MenuBarSetItemMenu MY_ANSIARGS((EZ_Widget *widget, char *label, EZ_Widget *menu));
EXTERN EZ_Widget   *EZ_MenuBarFindItemAtXY MY_ANSIARGS(( EZ_Widget *widget, int x, int y));
EXTERN EZ_Widget   *EZ_MenuBarFindItemAndMenuAtXY MY_ANSIARGS(( EZ_Widget *widget, int x, int y, EZ_Widget **ret));
EXTERN EZ_Widget   *EZ_MenuBarFindItem MY_ANSIARGS(( EZ_Widget *widget, char *name));
EXTERN EZ_Widget   *EZ_MenuBarFindItemAndMenu MY_ANSIARGS(( EZ_Widget *widget, char *name, EZ_Widget **ret));
EXTERN void         EZ_MenuBarDeleteItem MY_ANSIARGS((EZ_Widget *widget, char *label));
EXTERN void         EZ_MenuBarDeleteItemAndMenu MY_ANSIARGS((EZ_Widget *widget, char *label));
EXTERN void         EZ_MenuBarInsertItemAndMenu MY_ANSIARGS((EZ_Widget *mbar, char *label, int ul, EZ_Widget *menu,
							     char *ref, int where));
#endif

#ifndef _EZ_WIDGET_ITERM_C_
EXTERN EZ_Widget *EZ_CreateITerm MY_ANSIARGS((EZ_Widget *parent));
EXTERN void       EZ_update_iterm_scrollbars MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_TermTouchScreen MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_TermPowerOn     MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_GetTermComponents MY_ANSIARGS((EZ_Widget *term,
                                              EZ_Widget **iterm,
                                              EZ_Widget **vscroll));
#endif

#ifndef _EZ_WIDGET_TERM_C_
EXTERN EZ_Widget *EZ_CreateTerm MY_ANSIARGS((EZ_Widget *parent, int vscroll));
EXTERN void      EZ_GetTermWidgetComponents MY_ANSIARGS((EZ_Widget *widget, EZ_Widget **iterm,
                                                         EZ_Widget **vscro));
#endif

#ifndef _EZ_ITERM_COMMAND_C_
EXTERN void EZ_TermStartCommand MY_ANSIARGS((EZ_Widget *ezterm, char **args));
EXTERN void EZ_TermStopCommand MY_ANSIARGS((EZ_Widget *ezterm, int sig));
EXTERN void EZ_TermSetExitHandler MY_ANSIARGS((EZ_Widget *ezterm, EZ_CallBack cb, void *data));
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
EXTERN void         EZ_SetupRuler MY_ANSIARGS((EZ_Widget *ruler, int tunit, int tlength,
                                               int nscale, int offset));
EXTERN int          *EZ_GetRulerMarks  MY_ANSIARGS((EZ_Widget *ruler));
#endif


#ifndef _EZ_WIDGET_SCROLL_BUTTON_C_
EXTERN EZ_Widget   *EZ_CreateScrollButton MY_ANSIARGS((EZ_Widget *parent, int type));
EXTERN int         EZ_GetScrollButtonScrollCount MY_ANSIARGS((EZ_Widget *widget));
EXTERN EZ_Widget   *EZ_CreateScrollButton MY_ANSIARGS((EZ_Widget *parent, int type));
#endif

#ifndef _EZ_WIDGET_SPIN_BUTTON_C_
EXTERN EZ_Widget  *EZ_CreateSpinButton   MY_ANSIARGS((EZ_Widget *parent));
EXTERN char       *EZ_GetSpinButtonValue MY_ANSIARGS((EZ_Widget *spin));
EXTERN void        EZ_GetSpinButtonComponents MY_ANSIARGS((EZ_Widget *spin,
                                                    EZ_Widget **entry,
                                                    EZ_Widget **sbtn));
EXTERN void        EZ_SetupSpinButton  MY_ANSIARGS((EZ_Widget *spin, char *value,
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
EXTERN Pixmap EZ_MakeGradientPixmap MY_ANSIARGS((int n, int pw, int ph, 
                                                 float vts[][2], float rgbs[][3]));
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
EXTERN void EZ_SSheetSetTextProperty MY_ANSIARGS((EZ_Widget *widget, EZ_TextProp *prop));
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
                                             int (*cmp)MY_ANSIARGS((CONST void *,CONST void *)) ));
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
EXTERN void  EZ_SSheetGetActiveCell MY_ANSIARGS((EZ_Widget *wt, int *row, int *col, char **str));
EXTERN char  *EZ_SSheetGetCell MY_ANSIARGS((EZ_Widget *wt, int row, int col));
EXTERN void  EZ_SSheetSetActiveCell MY_ANSIARGS((EZ_Widget *widget, int x, int y));
EXTERN void  EZ_SSheetDrawCell MY_ANSIARGS((EZ_Widget *wt, int row, int col,  Drawable d, 
                                     int x, int y, int w, int h));
EXTERN char  *EZ_SSheetGetCellWithProperty MY_ANSIARGS((EZ_Widget *wt, int row, int col, EZ_TextProp ***prop));
EXTERN void  EZ_SSheetGetActiveCellWithProperty MY_ANSIARGS((EZ_Widget *wt, int *row, int *col, char **str,
                                                      EZ_TextProp ***prop));

EXTERN int   EZ_SSheetGetHighlightedRegion  MY_ANSIARGS((EZ_Widget *wt, int *r1, int *c1, int *r2, int *c2));
EXTERN char  *EZ_SSheetGetRegionString MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2));
EXTERN char  *EZ_SSheetGetRegionStringWithProperty  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2,
                                                          EZ_TextProp ***prop));
EXTERN void  EZ_SSheetInsertRegionString MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, char *str));
EXTERN void  EZ_SSheetInsertRegionStringWithProperty MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, char *str,
							   EZ_TextProp **tp));
EXTERN char  **EZ_SSheetGetRegionItems  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2));
EXTERN char  **EZ_SSheetGetRegionItemsWithProperty  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2,
							  EZ_TextProp ****tprop));
EXTERN void  EZ_SSheetInsertRegionItems  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2, char **items));
EXTERN void  EZ_SSheetInsertRegionItemsWithProperty  MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2, 
							   char **items, EZ_TextProp ***tprop));
EXTERN void  EZ_SSheetClearRegion MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2)); 

EXTERN void  EZ_SSheetCopyRegion MY_ANSIARGS((EZ_Widget *wt, int r1, int c1, int r2, int c2, int r0, int c0, int del));
EXTERN void  EZ_SSheetInsertItem MY_ANSIARGS((EZ_Widget *widget, int row, int col, char *str));
EXTERN void  EZ_SSheetInsertItemWithProperty MY_ANSIARGS((EZ_Widget *widget, int row, int col, char *str,
						   EZ_TextProp **p));
EXTERN void  EZ_SSheetSetRegionJustification MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, int just));
EXTERN void  EZ_SSheetSetRegionProperty MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2, EZ_TextProp *p));

EXTERN char  *EZ_SSheetIndexToString MY_ANSIARGS((int col, int row, char *str));
EXTERN void  EZ_SSheetStringToIndex MY_ANSIARGS(( char *str, int *col, int *row));

EXTERN EZ_Item *EZ_SSheetMakeChart MY_ANSIARGS((EZ_Widget *widget, int r1, int c1, int r2, int c2,
                                         EZ_SChartHandle handle, int kind, void *data, int cmod));
EXTERN void     EZ_SSheetDeleteChart MY_ANSIARGS((EZ_Widget *widget, EZ_Item *chart));

EXTERN int   EZ_SSheetRedrawCharts MY_ANSIARGS((EZ_Widget *wptr, Drawable drawable));

EXTERN int  EZ_SSheetSelectCellWork  MY_ANSIARGS((EZ_Widget *widget, Drawable d, int x, int y, int flag));
EXTERN void  EZ_SSheetRedrawRegion MY_ANSIARGS((EZ_Widget *widget, Drawable d, int x, int y, 
					 int x1, int y1, int flag));
EXTERN void  EZ_MarkSSheetUsedTextProperties MY_ANSIARGS((EZ_Widget *widget));
EXTERN void  EZ_SSheetPasteSelection MY_ANSIARGS((EZ_Widget *widget));
EXTERN int  EZ_MatchWord MY_ANSIARGS((char *buf, char *word, int blen, int wlen, int *pos_ret);)
#endif

#ifndef _EZ_WIDGET_THUM_WHEEL_C_
EXTERN EZ_Widget  *EZ_CreateThumbWheel MY_ANSIARGS((EZ_Widget *parent));
EXTERN float       EZ_GetThumbWheelValue MY_ANSIARGS((EZ_Widget *widget));
EXTERN void        EZ_SetThumbWheelValue MY_ANSIARGS((EZ_Widget *widget, float value));
EXTERN void        EZ_SetThumbWheelRange MY_ANSIARGS((EZ_Widget *widget, float f, float t));
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
EXTERN int   EZ_GetWidgetAttribute MY_ANSIARGS((EZ_Widget *widget, int option,  int oparam,
                                                 EZ_Value *vreturn));
EXTERN char *EZ_GetOptionSymbolicName MY_ANSIARGS((int i));
#endif


/* Gnu glob and Gnu tilde */
EXTERN char       **EZ_GlobFileName  MY_ANSIARGS((char *name));
EXTERN char       **EZ_GlobFiles  MY_ANSIARGS((char *name, int showHidden));
EXTERN int        EZ_GlobHiddenFiles MY_ANSIARGS((int yesNo ));
EXTERN char       *EZ_TildeExpand MY_ANSIARGS((char *name));


#ifndef _EZ_CURSOR_C_
EXTERN  void                  EZ_InitializeCursors MY_ANSIARGS((void));
EXTERN  void                  EZ_FreeAllCursors MY_ANSIARGS((void));
EXTERN  Cursor                EZ_GetCursor MY_ANSIARGS((int type));
EXTERN  Cursor                EZ_GetCursorByName MY_ANSIARGS((char *name));
EXTERN  char                  *EZ_GetCursorName MY_ANSIARGS((Cursor csr));

EXTERN Cursor               EZ_CreateCursorFromBitmapFile MY_ANSIARGS((char *f1, char *f2,
								char *fc, char *bc,
								int  x,   int y,
								char *name,
								int *idx));
EXTERN Cursor               EZ_CreateCursorFromBitmapData MY_ANSIARGS((int wid, int hig,
								char *d1, char *d2,
								char *fc, char *bc,
								int  x,   int y,
								char *name,
								int *idx_ret));

EXTERN void                  EZ_BusyCursor MY_ANSIARGS((EZ_Widget *widget, Cursor cursor, int timeout));
EXTERN void                  EZ_WaitCursor MY_ANSIARGS((EZ_Widget *widget, Cursor cursor));
EXTERN void                  EZ_NormalCursor  MY_ANSIARGS((EZ_Widget *widget));

#endif


#ifndef _EZ_ITEM_HASH_C_
EXTERN EZ_Item   *EZ_ItemExist MY_ANSIARGS((EZ_Item *item));
#endif


#ifndef _EZ_RESOUCE_C_
EXTERN void    EZ_PrintStandardOptions MY_ANSIARGS((void));
EXTERN void    EZ_SetApplicationName MY_ANSIARGS((char *name));
EXTERN char    *EZ_GetApplicationName MY_ANSIARGS((void));
EXTERN void    EZ_DestroyXrmDB          MY_ANSIARGS((void));
EXTERN XrmDatabase  *EZ_GetResourceDatabase MY_ANSIARGS(( void ));
EXTERN void    EZ_InitXrmAndX11 MY_ANSIARGS((char *className,
					  XrmOptionDescRec *optstable,  int nopts,
					  int *ac, char **av, char *fbkstr, int initgl));
EXTERN char *EZ_GetInstanceName      MY_ANSIARGS((void));
EXTERN char *EZ_GetClassName         MY_ANSIARGS((void));
EXTERN void  EZ_SetWidgetCINames  MY_ANSIARGS((EZ_Widget *w, char *cn, char *in));
EXTERN void  EZ_GetWidgetCINames  MY_ANSIARGS((EZ_Widget *w, char **cn, char **in));
EXTERN int   EZ_GetWidgetType     MY_ANSIARGS((EZ_Widget *w));
EXTERN void  EZ_ExtractGlobalResources MY_ANSIARGS((void));

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
EXTERN void  EZ_SetDefaultAttributesFromResources MY_ANSIARGS((void));
EXTERN int   EZ_GetOptionCINames MY_ANSIARGS((int i, char **cn, char **in));
#endif

#ifndef _EZ_RESOUCEGET_C_
EXTERN int  EZ_GetAndUseWidgetResources   MY_ANSIARGS((EZ_Widget *widget));
EXTERN int  EZ_GetAndUseWidgetResources1  MY_ANSIARGS((EZ_Widget *widget));
EXTERN int  EZ_GetAndUseItemResources     MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_RegisterApplicationResources MY_ANSIARGS((int n, EZ_ResourceSpec *spec, 
                                 int (*configure) MY_ANSIARGS((void *p, int opt, EZ_Value *values))
                                           ));
EXTERN int EZ_DecodeApplicationResources   MY_ANSIARGS((void));
EXTERN int EZ_SetOneWidgetResource MY_ANSIARGS((EZ_Widget *widget, char *resname, char *resvalue));
EXTERN int EZ_ParseWidgetResourceString MY_ANSIARGS((EZ_Widget *widget, char *resString));
EXTERN int EZ_ParseItemResourceString MY_ANSIARGS((EZ_Item *item, char *resString));
#endif

#ifndef _EZ_RESOURCE_EDIT_C_
EXTERN int EZ_HandleResourceEditorMessage MY_ANSIARGS((EZ_Message *msg));
EXTERN char         *EZ_PrintOneWidgetTree  MY_ANSIARGS((EZ_Widget *widget));
EXTERN char         *EZ_PrintWidgetTree  MY_ANSIARGS((void));
EXTERN EZ_TreeNode  *EZ_ParseWidgetTreeA  MY_ANSIARGS((char *str, int which));
EXTERN EZ_TreeNode  *EZ_ParseWidgetTree  MY_ANSIARGS((char *str));
EXTERN Window        EZ_SelectWindowWithButtonClick MY_ANSIARGS((void));
EXTERN char         *EZ_PrintApplicationResources MY_ANSIARGS((void));
EXTERN char         *EZ_PrintWidgetResources MY_ANSIARGS((EZ_Widget *widget));
EXTERN char         *EZ_PrintWidgetResourcesAll MY_ANSIARGS((EZ_Widget *widget));
EXTERN char         *EZ_PrintSetApplicationResources MY_ANSIARGS((char *msg, int len));
EXTERN char         *EZ_PrintSetWidgetResources MY_ANSIARGS((EZ_Widget *widget, char *msg, int len));
EXTERN EZ_Item      **EZ_ParseResourcesTable MY_ANSIARGS((char *str, int *cntRet));
EXTERN void         EZ_GetWidgetFullPath MY_ANSIARGS((EZ_Widget *widget, char **n, char **c));
EXTERN void         EZ_GetWidgetFullPathQuarks MY_ANSIARGS((EZ_Widget *widget, XrmQuark **n, XrmQuark **c));
EXTERN void         EZ_RecordResourceString MY_ANSIARGS((char **dest, char *src));
EXTERN int          EZ_CompareItem MY_ANSIARGS((void *a, void *b));
#endif



#ifndef _EZ_MESSAGE_C_
EXTERN void  EZ_SetDefaultMessageHandler MY_ANSIARGS((EZ_MessageHandler handler, void *data));
EXTERN void  EZ_AddMessageHandler MY_ANSIARGS((Atom mtype, int mId,  void *data,
					       EZ_MessageHandler handler));
EXTERN void  EZ_RegisterMessageHandler MY_ANSIARGS((Atom mtype, int mId,  void *data,
					       EZ_MessageHandler handler,
                                               int timeout, EZ_CallBack tcb, void *cdata));
EXTERN void  EZ_DeleteMessageHandler MY_ANSIARGS((Atom mtype, int mId,  void *data,
					EZ_MessageHandler handler));

EXTERN void  EZ_SendMessage MY_ANSIARGS((Atom mtype, char *message, int length, int msgid,
				  Window recipient, int replayId, int isReply));
EXTERN void  EZ_BroadcastMessage MY_ANSIARGS((Atom mtype, char *message, int length, 
					      int msgid, int replyId));

EXTERN void  EZ_ReplyMessage  MY_ANSIARGS((EZ_Message *fromsg, char *reply, int replylength));
EXTERN void  EZ_ReplyMessageR MY_ANSIARGS((EZ_Message *fromsg, char *reply, 
					   int replylength, int replyId));
#endif


#ifndef _EZ_WM_C_
EXTERN void EZ_SetWMHintsAndSizeHints MY_ANSIARGS((EZ_Widget *widget,int flag));
EXTERN void EZ_SetWMSizeHints MY_ANSIARGS((EZ_Widget *widget,int flag));
EXTERN void EZ_SetWMHintForWindow MY_ANSIARGS((Window win, int x, int y, int w, int h));
EXTERN void EZ_DeleteWidgetWMHints MY_ANSIARGS((EZ_Widget *widget));
EXTERN void EZ_SetWidgetWMHints MY_ANSIARGS((EZ_Widget *wt, int opt, int i, int j, void *p));
#endif

#ifndef _EZ_DRAG_XPM_C_
EXTERN void EZ_OverlayDragablePixmap MY_ANSIARGS((EZ_Widget *wgt, Drawable d,
					   int x, int y, int w, int h));
#endif

#ifndef _EZ_RTEXT_C_
EXTERN EZ_Bitmap *EZ_CreateRotateTextPixmap MY_ANSIARGS((char *str, int fId, int llen, int jt, int angle));
EXTERN EZ_Bitmap *EZ_CreateRotateTextPixmapU MY_ANSIARGS((char *str, int fId, int llen, int jt, int ul, int angle));
#endif

/*
 * Var arg things
 */
EXTERN EZ_TextProp *EZ_GetTextProperty  MY_VARARGS_D(int, name);
EXTERN void        EZ_ConfigureWidget   MY_VARARGS_D(void, *args);
EXTERN void        EZ_ConfigureItem     MY_VARARGS_D(void, *args);
EXTERN EZ_Widget  *EZ_CreateSimpleMenu  MY_VARARGS_D(char, *args);
EXTERN EZ_Widget  *EZ_CreateWidget      MY_VARARGS_D(int,  the_args);
EXTERN EZ_Widget  *EZ_CreateWidgetXrm   MY_VARARGS_D(int, args);
EXTERN EZ_Widget  *EZ_CreateWidgetXrm   MY_VARARGS_D(int,  the_args);
EXTERN EZ_Item    *EZ_CreateItem        MY_VARARGS_D(int,  the_args);
EXTERN EZ_Item    *EZ_CreateItemXrm     MY_VARARGS_D(int, args);
#endif


#ifdef EZ_DEBUG 
EXTERN void EZ_DumpLedActionTable         MY_ANSIARGS(( EZ_Widget *led));
EXTERN void EZ_DumpWidgetHashTable        MY_ANSIARGS((void));
EXTERN void EZ_DumpTimers                 MY_ANSIARGS((void));
EXTERN void EZ_DumpGEventTable            MY_ANSIARGS((void)); 
EXTERN void EZ_DumpTextPropertyHashTable  MY_ANSIARGS((void)); 
EXTERN void EZ_DumpItemHashTable          MY_ANSIARGS((void)); 
EXTERN void EZ_DumpMemoryRecords          MY_ANSIARGS((void)); 
#endif
/*************************************************************************/
