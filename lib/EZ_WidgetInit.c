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
 ***          Initialize the widget Library                    ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_INIT_C_

#include "EZ_Widget.h"
/*****************************************************************
 *
 *     Function implemented in this file.
 */
int  EZ_InitializeXrm MY_ANSIARGS((char *className, XrmOptionDescRec *optsTable,
				   int noptions, int *argc, char **argv,
				   char *fbkresrc,int initgl));
int     EZ_Initialize MY_ANSIARGS((int ac, char **av, int flag));
Display *EZ_GetDisplay MY_ANSIARGS((void));
Visual  *EZ_GetVisual MY_ANSIARGS((void));
Colormap EZ_GetColormap MY_ANSIARGS((void));
int      EZ_GetDepth  MY_ANSIARGS((void));
Window   EZ_GetWidgetWindow MY_ANSIARGS((EZ_Widget *widget));
Window  EZ_GetCommunicationWindow MY_ANSIARGS((void));
int     EZ_GetScreen MY_ANSIARGS((void));
void    EZ_GetDisplayInfo MY_ANSIARGS((Display **dpy, int *scrn, Visual **vis, int *depth));
void    EZ_GetDisplaySize MY_ANSIARGS((int *w, int *h));
void    EZ_DisableHighlight MY_ANSIARGS((void));
void    EZ_EnableHighlight MY_ANSIARGS((void));
void    EZ_DisableLabelOffset MY_ANSIARGS((void));
void    EZ_EnableLabelOffset MY_ANSIARGS((void));
void    EZ_DisableSliderDepression MY_ANSIARGS((void));
void    EZ_EnableSliderDepression MY_ANSIARGS((void));
void    EZ_Shutdown MY_ANSIARGS((void));
void    EZ_ShutdownA MY_ANSIARGS((void));
void    EZ_DisableImageDithering MY_ANSIARGS((void));
void    EZ_EnableImageDithering MY_ANSIARGS((void));
void    EZ_EnableBubbleHelp MY_ANSIARGS((void));
void    EZ_DisableBubbleHelp MY_ANSIARGS((void));
void    EZ_AllowDualLabel  MY_ANSIARGS((int));

void    EZ_FixFocusHighlightBorder MY_ANSIARGS((EZ_Widget *wptr, Drawable drawable,
                                                int w, int h, int padb));
void    EZ_StartAsIcon  MY_ANSIARGS((int));

void    EZ_AddIdleCallBack    MY_ANSIARGS((EZ_CallBack cb, void *data));
void    EZ_RemoveIdleCallBack MY_ANSIARGS((EZ_CallBack cb, void *data));
void    EZ_RemoveAllIdleCallBack MY_ANSIARGS((void));
void    EZ_ExecuteIdleCallBacks MY_ANSIARGS((void));
void    EZ_SetGlobalEventFilter MY_ANSIARGS((EZ_EventFilter filter, void *data));
int     EZ_QueryPixelValue  MY_ANSIARGS((EZ_Widget *widget, int x, int y,
                                         unsigned long *pv));
EZ_Widget *EZ_GetWidgetGroupLeader MY_ANSIARGS((EZ_Widget *widget));
void       EZ_SetWidgetGroupLeader MY_ANSIARGS((EZ_Widget *widget, EZ_Widget *leader));
/******************************************************************************/
void    EZ_FlashWidgetWindowBorder MY_ANSIARGS((EZ_Widget *widget, Drawable drawable,
						int x, int y, int w,int h, int idx));
void    EZ_FlashWidgetBorder MY_ANSIARGS((EZ_Widget *widget));
char    *EZ_LibraryVersion MY_ANSIARGS((void));
EZ_Widget *EZ_GetBubbleHelp MY_ANSIARGS((void));
/******************************************************************************/

void  EZ_ResetGlobalVariables MY_ANSIARGS((void));
void  EZ_SetClientMessageHandler MY_ANSIARGS(( EZ_EventHandler handler, void *data));
void  EZ_UpdateBgGCs MY_ANSIARGS((void));
void  EZ_UpdateFgGCs MY_ANSIARGS((unsigned long old));
/*******************************************************************
 *  This is the default event handle. It only uses the common part
 *  of the widget union. Private data are never used.
 */
void   EZ_ComputeWidgetUnknownSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void   EZ_UnkownEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void   EZ_DrawWidgetUnknown MY_ANSIARGS((EZ_Widget *widget));
void   EZ_FreeUnknownData  MY_ANSIARGS((EZ_Widget *widget));

/****************** default handle for items ***************/
void   EZ_ComputeItemUnknownSize MY_ANSIARGS((EZ_Item *item, int *w, int *h)); 
void   EZ_DrawItemUnknown MY_ANSIARGS((EZ_Widget *widget,
				       Drawable  drawable,
				       int       x0, int y0,
				       EZ_Item   *item,
				       int       Rx, int Ry, int Rw, int Rh));
void   EZ_UpdateItemUnknown MY_ANSIARGS((EZ_Widget *widget,
					 Drawable  drawable,
					 EZ_Item   *item,
					 int       Rx, int Ry, int Rw, int Rh));
void   EZ_HighlightItemUnknown MY_ANSIARGS((EZ_Widget *widget,
					    Drawable  drawable,
					    EZ_Item   *item,
					    unsigned long fg, unsigned long bg,
					    int       Rx, int Ry, int Rw, int Rh, int flag));       
void   EZ_UnknownItemData  MY_ANSIARGS((EZ_Item *item));
int    EZ_UnknownItemPick   MY_ANSIARGS((EZ_Item *item, int x, int y));

void EZ_RedrawAll MY_ANSIARGS((void));
/****************************************************************/
static void AlterDefaultForeground MY_ANSIARGS((unsigned long old));
static void flashBDTimerCallback MY_ANSIARGS((EZ_Timer *timer, void *data));
static void InitEZWGL2  MY_ANSIARGS((void));
static int setGlobalState MY_ANSIARGS((EZ_Widget *nptr, int opt, EZ_Value *values));
/****************************************************************/
static EZ_ResourceSpec globalStateResources[] =
{
  {9876540,   "highlight",          "highlight",         "boolean"},
  {9876541,   "labelOffset",        "LabelOffset",       "boolean"},
  {9876542,   "sliderDepression",   "SliderDepression",  "boolean"},
  {9876543,   "bubbleHelp",         "BubbleHelp",        "boolean"},
  {9876544,   "labelFont",          "LabelFont",         "font"},
  {9876545,   "textFont",           "TextFont",          "font"},
  {9876546,   "menuTitleFont",      "MenuTitleFont",     "font"},
  {9876547,   "focusHighlightColor","FocusHighlightColor","color"},
  {9876548,   "background",         "Background",         "color"},
  {9876549,   "foreground",         "Foreground",         "color"},
  {9876550,   "dualLabel",          "DualLabel",          "boolean"},
};

/****************************************************************/

static EZ_WidgetHandle EZ_DefaultHandle =
{ 
  EZ_ComputeWidgetUnknownSize,
  EZ_DrawWidgetUnknown,
  EZ_FreeUnknownData,
  EZ_UnkownEventHandle,
};

static EZ_DpyItemHandle EZ_DefaultItemHandle =
{
  EZ_ComputeItemUnknownSize,
  EZ_DrawItemUnknown,
  EZ_UpdateItemUnknown,
  EZ_HighlightItemUnknown,
  EZ_UnknownItemData,
  EZ_UnknownItemPick
};

/*****************************************************************
 *
 *     Global Variables;
 */

Atom     EZ_WM_DELETE_WINDOW_ATOM;
Atom     EZ_WM_PROTOCOLS_ATOM;
Atom     EZ_DND_DRAG_INIT_ATOM;
Atom     EZ_DND_DRAG_START_ATOM;
Atom     EZ_DND_DRAG_FINISH_ATOM;
Atom     EZ_DND_DROP_START_ATOM;
Atom     EZ_DND_DROP_FINISH_ATOM;
Atom     EZ_MESSAGE_ATOM;
Atom     EZ_RESOURCE_EDITOR_ATOM;

XSInfo           XServerInfo;
EZ_WidgetHandle  *EZ_WidgetHandlingFunctions[EZ_TOTAL_NUMBER_OF_WIDGETS];
EZ_Widget        *EZ_BubbleHelp = (EZ_Widget *)NULL;
EZ_Widget        *EZ_CurrentFlashWidget = (EZ_Widget *)NULL;
EZ_Widget        *EZ_CurrentFlashWidget2 = (EZ_Widget *)NULL;
EZ_BitmapHTEntry *EZ_SnapShotBitmap = (EZ_Bitmap *)NULL;
EZ_BitmapHTEntry *EZ_Nails[4];
Time             EZ_LastButton1ClickTime = 0;
EZ_DnDInfoStruct EZ_DnDInfo;
EZ_EventHandlerStruct *EZ_NoWidgetEventHandlers = NULL;
EZ_CallBackStruct     *EZ_IdleCallBacks = NULL;
EZ_EventFilter    EZ_GlobalEventFilter = NULL;
void             *EZ_GlobalEventFilterData = NULL;

char              EZ_HighlightEnabled = 1;
char              EZ_OffsetEnabled    = 1;
char              EZ_DepresionDisabled = 0;
char              EZ_DitherImageOn = 0;
char              EZ_BubbleHelpOn = 1;
char              EZ_PointerGrabed = 0;
char              EZ_LabelFontIsBold = 1;
char              EZ_TopWinState = 1; /* NormalState */

int               EZ_ServerGrabed = 0;
int               EZ_CounterUT = 0;
int               EZ_PTLabel = 0;    /* don't alow both pixmap textual label */

EZ_Widget         *EZ_LastDropSite = (EZ_Widget *) NULL;

unsigned long     EZ_ApplicationSetupTime = 0;
unsigned long     EZ_KbdFocusHighlightColor = 0;

void              (*EZ_CreateNewWidgetHook) MY_ANSIARGS((EZ_Widget *widget));
/*****************************************************************/
EZ_EventHandlerStruct EZ_ClientMessageHandler;
/*****************************************************************
 *
 *     Local Variables
 */
static int  EZ_WidgetLibHasBeenInitialized = 0;
/*******************************************************************
 ***                                                             ***
 ***            Initialize the widget library.                   ***
 ***                                                             ***
 *******************************************************************/
int EZ_InitializeXrm(className, optsTable, noptions,argc,argv,fbkresrc,initgl)
     char *className;
     XrmOptionDescRec *optsTable;
     int              noptions;
     int              *argc;
     char             **argv;
     char             *fbkresrc;
     int              initgl;
{
  int status;
  if(EZ_WidgetLibHasBeenInitialized) return(0);
  
#if defined(EZ_MEM_DEBUG) || defined(EZ_DEBUG)
  {
    extern  void initialize_mem_debug_data();
    (void)initialize_mem_debug_data();
  }
#endif
  /*------------------------------
   * Initialize X11. This must be
   * the first thing to do!
   *-----------------------------*/

  status = EZ_InitXrmAndX11(className, 
                            optsTable, noptions,
                            argc, argv, 
                            fbkresrc,
                            initgl);
  if(status >= 0)
    {
      InitEZWGL2();
      EZ_RegisterApplicationResources(EZ_ArraySize(globalStateResources),
                                      globalStateResources,
                                      (void *)setGlobalState);

    }
  return(status);
}
/****************************************************************/
int  EZ_Initialize(ac,av,initgl)
     int ac; char **av;
     int initgl;
{
  int status;
  if(EZ_WidgetLibHasBeenInitialized) return(0);

#if defined(EZ_MEM_DEBUG) || defined(EZ_DEBUG)
  {
    extern  void initialize_mem_debug_data();
    (void)initialize_mem_debug_data();
  }
#endif

  /*------------------------------
   * Initialize X11. This must be
   * the first thing to do!
   *-----------------------------*/
  /* 3-22-98: EZ_InitXrmAndX11 modifies av and ac.
   * This is not compatibale with the behavior of
   * old versions of EZWGL. Make a local copy
   * of args and pass them to EZ_InitXrmAndX11
   */
  {
    int i, length = 0;
    char *ptr, *data, **argv;

    for(i = 0; i < ac; i++) length += strlen(av[i]) + 2;
    data = (char *)my_malloc( (length)* sizeof(char), _MISC_DATA_);
    argv = (char **) my_malloc( (ac + 1) * sizeof(char *), _MISC_DATA_);
    ptr = data;
    for(i = 0; i < ac; i++)
      {
	argv[i] = ptr;
	(void)strcpy(ptr, av[i]);
	ptr += strlen(av[i])+1;
      }
    argv[i] = NULL;
    status = EZ_InitXrmAndX11(NULL, 
                              NULL, 0,
                              &ac, argv, 
                              NULL,
                              initgl);
    (void)my_free(data);
    (void)my_free(argv);
  }
  if(status>=0)
    {
      InitEZWGL2();
      EZ_RegisterApplicationResources(EZ_ArraySize(globalStateResources),
                                      globalStateResources,
                                      (void *)setGlobalState);
    }
  return(status);
}
/*********************************************************************/

static void InitEZWGL2()
{
  /*------------------------------
   * We initialize GL anyway, to
   * save the user the trouble of
   * core dumps.  So the only 
   * difference between initgl or
   * not is the # of colors 
   * allocated.
   *-----------------------------*/
  /* if(initgl) */  
  EZ_InitializeGL();

  /*---------------------------------
   * Device Buttons.
   *--------------------------------*/
  EZ_Btn1 = Button1;
  EZ_Btn2 = Button2;
  EZ_Btn3 = Button3;
  EZ_Btn4 = Button4;
  EZ_Btn5 = Button5;

  /*---------------------------------
   * Create the GC's to be used.
   * check if we need to alter the
   * default fonts.
   *------------------------------*/
  EZ_InitializeWidgetGC();
  EZ_SetDefaultAttributesFromResources(); /* fonts + global flags */
  /*---------------------------------
   * Initialize the widget hash table
   *--------------------------------*/
  EZ_InitializeWidgetHT();

  /*---------------------------------
   * Initialize the event hash table
   *--------------------------------*/
  EZ_InitializeEventHT();

  /*---------------------------------
   * Initialize the text property ht
   *--------------------------------*/
  EZ_InitializeTxetPropertyHashTable();

  EZ_CreateNewWidgetHook = NULL;
  /*---------------------------------
   * Clear Widget Handling Functions
   *--------------------------------*/
  {
    int i;
    for(i = 0; i < EZ_TOTAL_NUMBER_OF_WIDGETS; i++)
      EZ_WidgetHandlingFunctions[i] = (EZ_WidgetHandle *) &EZ_DefaultHandle;

    /* display item handle */
    for(i = 0; i < EZ_TOTAL_NUMBER_OF_ITEMS; i++)
      EZ_DpyItemHandlers[i] = (EZ_DpyItemHandle *) &EZ_DefaultItemHandle;
  }
  
  /*---------------------------------
   * Initialize timers
   *--------------------------------*/
  EZ_InitializeTimers();
  EZ_InitializeInputIdList();
  EZ_InitGarbageList();
  EZ_InitializeMessageHandlers();

  /*--------------------------------------
   * Create a few Atoms for internal use
   *-------------------------------------*/
  EZ_APPLICATION_ATOM = XInternAtom(EZ_Display,"EZWGL_APPLICATION", False);
  EZ_WLIST_ATOM = XInternAtom(EZ_Display,"EZWGL_WLIST", False);
  EZ_EMBEDING_MESSAGE_ATOM = XInternAtom(EZ_Display,"EZWGL_EMBEDING_MSG", False);

  EZ_DND_DRAG_INIT_ATOM = XInternAtom(EZ_Display,"EZ_DND_INIT_START", False);
  EZ_DND_DRAG_START_ATOM = XInternAtom(EZ_Display,"EZ_DND_DRAG_START", False);
  EZ_DND_DRAG_FINISH_ATOM = XInternAtom(EZ_Display,"EZ_DND_DRAG_FINISH", False);
  EZ_DND_DROP_START_ATOM = XInternAtom(EZ_Display,"EZ_DND_DROP_START", False);
  EZ_DND_DROP_FINISH_ATOM = XInternAtom(EZ_Display,"EZ_DND_DROP_FINISH", False);

  /* WM protocol. Thanks to: Tomi Sarvela <orava@nether.tky.hut.fi> */
  EZ_WM_DELETE_WINDOW_ATOM = XInternAtom(EZ_Display, "WM_DELETE_WINDOW", False);
  EZ_WM_PROTOCOLS_ATOM =  XInternAtom(EZ_Display, "WM_PROTOCOLS", False);

  EZ_MESSAGE_ATOM =  XInternAtom(EZ_Display, "EZ_MESSAGE", False);
  EZ_RESOURCE_EDITOR_ATOM =  XInternAtom(EZ_Display, "EZ_RESOURCE_EDITOR", False);

  /*---------------------------------
   * Must be set here !!! 
   * the rest of the code need it.
   *--------------------------------*/  
  EZ_WidgetLibHasBeenInitialized = 1;

  /*---------------------------------
   * something for Drag and Drop
   *--------------------------------*/
  EZ_InitializeDnD();
  /*---------------------------------
   * register this application
   *--------------------------------*/
  EZ_RegisterApplication();

  /*---------------------------------
   * ClientMessage handler
   *--------------------------------*/ 
  memset(&EZ_ClientMessageHandler, 0, sizeof(EZ_EventHandlerStruct));

  (void)EZ_CreateTimer(0,365432,-1, (EZ_CallBack)flashBDTimerCallback, NULL, 0);

  /*---------------------------------
   * Bubble help
   *--------------------------------*/  
  EZ_BubbleHelp = EZ_CreateFreeLabel(NULL,"Bubble Help");
  EZ_SetWidgetCINames(EZ_BubbleHelp, "Bubble","bubble");
  EZ_WidgetBorderWidth(EZ_BubbleHelp) = 1;
  EZ_WidgetBorderStyle(EZ_BubbleHelp) = EZ_BORDER_FLAT;
  EZ_SetWidgetTransientFlag(EZ_BubbleHelp);
  {
    unsigned long pixel = EZ_ColorArray[YELLOW];
    EZ_WidgetBackground(EZ_BubbleHelp) = pixel;
    EZ_WidgetJustification(EZ_BubbleHelp)= EZ_CENTER;
    if(EZ_Visual->class != TrueColor && EZ_Visual->class != DirectColor)
      EZ_DarkColor[pixel].pixel = EZ_ColorArray[DARKRED];
  }
  EZ_GetAndUseWidgetResources(EZ_BubbleHelp); /* set options from resources */
  EZ_CreateWindowForWidget(EZ_BubbleHelp, 0);
  EZ_SetupDragablePixmap(1);
}


Display *EZ_GetDisplay()
{
  return(EZ_Display);
}

int EZ_GetScreen()
{
  return(EZ_ScreenNum);
}

Visual *EZ_GetVisual()
{
  return(EZ_Visual);
}

Colormap EZ_GetColormap()
{
  return(EZ_Colormap);
}

int EZ_GetDepth()
{
  return(EZ_Depth);
}

Window EZ_GetWidgetWindow(widget)
     EZ_Widget *widget;
{
  if(widget) return(EZ_WidgetWindow(widget));
  return((Window)NULL);
}
Window  EZ_GetCommunicationWindow()
{
  return(EZ_DummyWindow);
}
/***************************************************************/


void  EZ_GetDisplayInfo(dpy, scrn, visual, depth)
     Display **dpy;
     int     *scrn, *depth;
     Visual  **visual;
{
  *dpy = EZ_Display;
  *scrn = EZ_ScreenNum;
  *visual = EZ_Visual;
  *depth = (int)EZ_Depth;
}

/*******************************************************************/
void EZ_UpdateBgGCs()
{
  if(EZ_WidgetLibHasBeenInitialized)
    {
      int i;
      unsigned long valuemask = GCForeground;
      XGCValues values;

      for(i = 1; i <= 7; i++)
	{
	  if(i != 3)
	    {
	      values.foreground = EZ_ColorArray[i];
	      XChangeGC(EZ_Display,EZ_GCList[i],valuemask, &values);
	    }
	}
      EZ_RedrawAll();
    }
}
void EZ_UpdateFgGCs(old) unsigned long old;
{
  if(EZ_WidgetLibHasBeenInitialized)
    {
      int i;
      unsigned long valuemask = GCForeground;
      XGCValues values;
      values.foreground = EZ_DefaultForeground;
      for(i = 1; i <= 4; i++)
	XChangeGC(EZ_Display,EZ_GCList[i+DARKRED],valuemask, &values);
      AlterDefaultForeground(old);
      EZ_RedrawAll();
    }
}
/*******************************************************************/
void EZ_SetKbdHighlightColor(color)
     char *color;
{
  EZ_KbdFocusHighlightColor = EZ_AllocateColorFromName(color);
}
void EZ_SetKbdHighlightColorRGB(r,g,b)
     int r,g,b;
{
  EZ_KbdFocusHighlightColor =  EZ_AllocateColorFromRGB(r,g,b);
}
/*******************************************************************/
void    EZ_DisableHighlight()
{
  EZ_HighlightEnabled = 0;
}
void    EZ_EnableHighlight()
{
  EZ_HighlightEnabled = 1;
}
void    EZ_DisableLabelOffset()
{
  EZ_OffsetEnabled = 0;
}
void    EZ_EnableLabelOffset()
{
  EZ_OffsetEnabled = 1;
}
void    EZ_TurnOnBubbleHelp() 
{
  EZ_BubbleHelpOn = 1;
}
void    EZ_TurnOffBubbleHelp() 
{
  EZ_BubbleHelpOn = 0;
}
void    EZ_EnableBubbleHelp() 
{
  EZ_BubbleHelpOn = 1;
}
void    EZ_DisableBubbleHelp() 
{
  EZ_BubbleHelpOn = 0;
}
void  EZ_DisableSliderDepression()
{
  EZ_DepresionDisabled = 1;
}
void  EZ_EnableSliderDepression()
{
  EZ_DepresionDisabled = 0;
}
/*******************************************************************/
extern void    EZ_DestroyWidgetGCsandFonts();
extern void    EZ_DestroyAllWidgets();
extern void    EZ_RemoveAllGlobalKeyEvents();
extern void    EZ_ClearTextKillBuffers();
extern void    EZ_DestroyAllBitmaps();
extern void    EZ_DestroyAllTimers();
extern void    EZ_ClearFocusRecord();

void EZ_ShutdownA()
{
  XSync(EZ_Display, True);
  EZ_FreezeDisplay();
  EZ_DestroyAllWidgets();
  XDestroyWindow(EZ_Display,EZ_DummyWindow);
  EZ_FreeResourceHandles();
  EZ_DummyWindow = (Window) NULL;
  EZ_DestroyWidgetGCsandFonts();
  EZ_RemoveAllGlobalKeyEvents();
  EZ_ClearTextKillBuffers();
  EZ_ClearTextPropHT();
  EZ_DestroyAllBitmaps();
  EZ_DestroyAllTimers();
  EZ_DestroyInputIdList();
  EZ_DestroyDnDList();
  EZ_ClearFocusRecord();
  EZ_FreeAllCursors();
  EZ_DeleteAllUDFWidgetHandles();
  EZ_DestroyXrmDB();
  EZ_DestroyColorBalls();
  /* delete all NoWidget event handlers, and idle callbacks */
  EZ_RemoveAllCallBacksWork((EZ_CallBackStruct **)&EZ_NoWidgetEventHandlers);
  EZ_RemoveAllCallBacksWork(&EZ_IdleCallBacks);
  EZ_ResetForeignWindowEventMask();
  EZ_CleanUpFigItemGCs();
  EZ_GLExit();
  EZ_SetupDragablePixmap(0);
  EZ_ThawDisplay();
}

void EZ_Shutdown()
{
  if(EZ_WidgetLibHasBeenInitialized == 0) return;;
  EZ_ShutdownA();
  XCloseDisplay(EZ_Display);

#if defined(EZ_MEM_DEBUG) || defined(EZ_DEBUG)  
  {
    extern void  EZ_DumpAndClearMemoryRecords();
    EZ_DumpAndClearMemoryRecords();
  }
#endif
  /* 
   * Bug found by: Arno Dabekaussen <piemail!arnodab@relay.NL.net> 08-01-97
   * Need to reset all "global" variables to their default values.
   */
  EZ_ResetGlobalVariables();
  EZ_WidgetLibHasBeenInitialized = 0;
}

/***********************************************************************
 *
 * widget handle for widget unknown...
 */

void EZ_ComputeWidgetUnknownSize(widget, w,h)
     EZ_Widget *widget;
     int       *w, *h;
{
  int cw, ch, tmp;

  EZ_SetWidgetSizeComputedFlag(widget);
  tmp = EZ_WidgetBorderWidth(widget);
  cw = EZ_WidgetPadX(widget) + tmp;
  ch = EZ_WidgetPadY(widget) + tmp;

  *w = cw + cw;
  *h = ch + ch;
}
/*******************************************************************/
void EZ_FreeUnknownData(widget)
     EZ_Widget *widget;
{
}
/*******************************************************************/

void  EZ_DrawWidgetUnknown(wptr)
     EZ_Widget *wptr;
{
  int             w, h;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  unsigned long   bgpv;

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  
  pixmap = XCreatePixmap(EZ_Display, win, w, h, EZ_Depth); 
  if(EZ_GetBackgroundGC(wptr, &gc,&bgpv, 1, 0))
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

/*******************************************************************/
void EZ_UnkownEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent *event;
{
  if(event->type == Expose)  EZ_DrawWidget(widget); 
  else if(event->type == ButtonPress && event->xbutton.button == EZ_Btn3)
    {
      EZ_HandleDnDEvents(widget, event);
    }
}
/*********************************************************************/
void EZ_EnableImageDithering()
{
}
/*********************************************************************/
void EZ_DisableImageDithering()
{
}
/*********************************************************************/
void  EZ_SetClientMessageHandler(handler, data)
     EZ_EventHandler handler;
     void *data;
{
  EZ_ClientMessageHandler.handler = handler;
  EZ_ClientMessageHandler.data = data;
  EZ_ClientMessageHandler.internal = 0;
  EZ_ClientMessageHandler.next = NULL;
}
/*********************************************************************/
extern EZ_Widget *EZ_GrabedWidget;
/*
 * 1-15-97
 */
void  EZ_ResetGlobalVariables()
{
  { /* from this file */
    EZ_LastButton1ClickTime = 0;

    EZ_HighlightEnabled = 1;
    EZ_OffsetEnabled    = 1;
    EZ_DepresionDisabled = 0;
    EZ_BubbleHelp = (EZ_Widget *)NULL;
    EZ_GrabedWidget = (EZ_Widget *)NULL;
    EZ_CurrentFlashWidget = (EZ_Widget *)NULL;
    EZ_CurrentFlashWidget2 = (EZ_Widget *)NULL;
    EZ_LastDropSite = (EZ_Widget *)NULL;
    EZ_SnapShotBitmap = (EZ_Bitmap *)NULL;
    EZ_Nails[0]=EZ_Nails[1]=EZ_Nails[2]=EZ_Nails[3] = NULL;
    EZ_BubbleHelpOn = 1;
    EZ_PTLabel = 0;
    EZ_PointerGrabed = 0;
    EZ_ServerGrabed = 0;
    EZ_CounterUT = 0;
    EZ_LabelFontIsBold = 1;

    EZ_TopWinState = 1;

    EZ_ApplicationSetupTime = 0;
    EZ_KbdFocusHighlightColor = 0;

    memset(&EZ_ClientMessageHandler, 0, sizeof(EZ_EventHandlerStruct));
    EZ_NoWidgetEventHandlers = NULL;
    EZ_IdleCallBacks = NULL;
    EZ_GlobalEventFilter = NULL;
    EZ_GlobalEventFilterData = NULL;
  }

  {
    extern void EZ_ResetGVDnDC MY_ANSIARGS((void));
    EZ_ResetGVDnDC();
  }

  {
    extern void EZ_ResetGVColorC MY_ANSIARGS((void));
    EZ_ResetGVColorC();
  }

  {
    extern void EZ_ResetGVX11InitC MY_ANSIARGS((void));
    EZ_ResetGVX11InitC();
  }

  {
    extern void EZ_ResetGVPopupC MY_ANSIARGS((void));
    EZ_ResetGVPopupC();
  }

  {
    extern void EZ_RestGVFocusC MY_ANSIARGS((void));
    EZ_RestGVFocusC();
  }

  {
    extern void EZ_ResetGVTreeDirC MY_ANSIARGS((void));
    EZ_ResetGVTreeDirC();
  }

  {
    extern void EZ_ResetGVButtonRC MY_ANSIARGS((void));
    EZ_ResetGVButtonRC();
  }
  {
    extern void EZ_ResetGarbageList MY_ANSIARGS((void));     
    EZ_ResetGarbageList();
  }

  {
    extern void EZ_ResetMessageHandlers();
    EZ_ResetMessageHandlers();
  }

  EZ_ClearFocusRecord();
  EZ_ResetForeignWindowEventMask();
}

/*********************************************************************/
void EZ_ComputeItemUnknownSize(item, w,h)
     EZ_Item *item; int *w, *h;
{ }
  
void EZ_DrawItemUnknown(widget, drawable, x0,y0, item, Rx,Ry,Rw,Rh)
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int       x0,  y0;
     int       Rx,  Ry,  Rw,  Rh;
{ }
void EZ_UpdateItemUnknown(widget, drawable, item,Rx,Ry,Rw,Rh)
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     int       Rx,  Ry,  Rw,  Rh;
{ }
void EZ_HighlightItemUnknown(widget, drawable, item, fg,bg,Rx,Ry,Rw,Rh, hmode)
     EZ_Widget *widget;
     Drawable  drawable;
     EZ_Item   *item;
     unsigned long fg, bg;
     int       Rx,  Ry,  Rw,  Rh;
     int hmode;
{ }

void EZ_UnknownItemData(item)
     EZ_Item *item;
{ }
int  EZ_UnknownItemPick(item, x, y)
     EZ_Item *item; int x, y;
{ return(0); }
/*********************************************************************/
static void flashBDTimerCallback(timer, data)
     EZ_Timer *timer; void *data;
{
  EZ_Widget *widget = EZ_CurrentFlashWidget;
  if(widget && EZ_LookupWidgetFromAllHT(widget))
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_NW_LABEL)
        {
          EZ_Widget *parent = widget;
          int  x = 1, y = 1;
          while(parent && EZ_WidgetType(parent) ==  EZ_WIDGET_NW_LABEL)
            {
              x += EZ_WidgetOriginX(parent);
              y += EZ_WidgetOriginY(parent);
              parent = EZ_WidgetParent(parent);
            }
          if(parent && EZ_WidgetMapped(parent))
            {
              Window win = EZ_WidgetWindow(parent);  
              EZ_FlashWidgetWindowBorder(parent, win, x, y,
                                         EZ_WidgetWidth(widget)-2,
                                         EZ_WidgetHeight(widget)-2, 0);
            }
        }
      else
        {
          if(EZ_WidgetMapped(widget))
            {
              Window win = EZ_WidgetWindow(widget);  
              EZ_FlashWidgetWindowBorder(widget, win, 0, 0,
                                         EZ_WidgetWidth(widget)-1,
                                         EZ_WidgetHeight(widget)-1, 0);
            }
	}
    }
  else EZ_CurrentFlashWidget = NULL;

  /* the second one */
  widget = EZ_CurrentFlashWidget2;
  if(widget && EZ_LookupWidgetFromAllHT(widget))
    {
      if(EZ_WidgetType(widget) == EZ_WIDGET_NW_LABEL)
        {
          EZ_Widget *parent = widget;
          int  x = 1, y = 1;
          while(parent && EZ_WidgetType(parent) ==  EZ_WIDGET_NW_LABEL)
            {
              x += EZ_WidgetOriginX(parent);
              y += EZ_WidgetOriginY(parent);
              parent = EZ_WidgetParent(parent);
            }
          if(parent && EZ_WidgetMapped(parent))
            {
              Window win = EZ_WidgetWindow(parent);  
              EZ_FlashWidgetWindowBorder(parent, win, x, y,
                                         EZ_WidgetWidth(widget)-2,
                                         EZ_WidgetHeight(widget)-2, 1);
            }
        }
      else
        {
          if(EZ_WidgetMapped(widget))
            {
              Window win = EZ_WidgetWindow(widget);  
              EZ_FlashWidgetWindowBorder(widget, win, 0, 0,
                                         EZ_WidgetWidth(widget)-1,
                                         EZ_WidgetHeight(widget)-1, 1);
            }
	}
    }
  else EZ_CurrentFlashWidget2 = NULL;
}

void EZ_FlashWidgetWindowBorder(widget, drawable, x,y,w,h, idx)
     EZ_Widget *widget;
     Drawable drawable;
     int x,y,w,h,idx;
{
  if(drawable != None)
    {
      XPoint points[5];
      GC gc;
      EZ_GetMovingDashedLineGC(&gc, idx);
      points[0].x = x;     points[0].y = y;
      points[1].x = x+w;   points[1].y = y;
      points[2].x = x+w;   points[2].y = y+h;
      points[3].x = x;     points[3].y = y+h;
      points[4].x = x;     points[4].y = y;
      XDrawLines(EZ_Display, drawable, gc, points, 5, CoordModeOrigin);
    }
}

void EZ_FlashWidgetBorder(widget)
     EZ_Widget *widget;
{
  EZ_Widget *tmp = EZ_CurrentFlashWidget;

  if(widget == tmp) return;
  EZ_CurrentFlashWidget = widget;  
  if(tmp && EZ_LookupWidgetFromAllHT(tmp) && EZ_WidgetMapped(tmp))
    {
      while(tmp && EZ_WidgetType(tmp) == EZ_WIDGET_NW_LABEL)
        tmp = EZ_WidgetParent(tmp);
      if(tmp && EZ_WidgetMapped(tmp)) EZ_DisplayWidget(tmp);
    }
  if(widget && EZ_LookupWidgetFromAllHT(widget) == widget)
    {
      if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget);
    }
  else  EZ_CurrentFlashWidget = NULL;
}

void EZ_FlashWidgetBorder2(widget)
     EZ_Widget *widget;
{
  EZ_Widget *tmp = EZ_CurrentFlashWidget2;

  if(widget == tmp) return;
  EZ_CurrentFlashWidget2 = widget;  
  if(tmp && EZ_LookupWidgetFromAllHT(tmp) && EZ_WidgetMapped(tmp))
    {
      while(tmp && EZ_WidgetType(tmp) == EZ_WIDGET_NW_LABEL)
        tmp = EZ_WidgetParent(tmp);
      if(tmp && EZ_WidgetMapped(tmp)) EZ_DisplayWidget(tmp);
    }
  if(widget && EZ_LookupWidgetFromAllHT(widget) == widget)
    { if(EZ_WidgetMapped(widget)) EZ_DrawWidget(widget); }
  else EZ_CurrentFlashWidget2 = NULL;
}

/*************************************************************************/
void EZ_GetDisplaySize(w_ret, h_ret) int *w_ret, *h_ret;
{
  if(w_ret) *w_ret = EZ_XDisplayWidth;
  if(h_ret) *h_ret = EZ_XDisplayHeight;
}
/*************************************************************************/
#include "VERSION.h"
char *EZ_LibraryVersion()
{
  static char  version[] = EZWGL_VERSION_STRING;
  return(version);
}
void EZ_GetVersionNumbers( mj, mi) int *mj, *mi;
{
  *mj = EZWGL_MAJOR_VERSION;
  *mi = EZWGL_MINOR_VERSION;
}

EZ_Widget *EZ_GetBubbleHelp() { return(EZ_BubbleHelp);}

void EZ_StartAsIcon(v)  int v;
{  EZ_TopWinState = (v != 0 ? IconicState: NormalState);}
/*************************************************************************/
void EZ_FixFocusHighlightBorder(wptr, drawable, w, h, padb)
     EZ_Widget *wptr; Drawable drawable; int w, h, padb;
{
  if(padb > 0)
    {
      GC  gc; unsigned long bgpv;
      int kind = EZ_GetParentBgGC(wptr, &gc,&bgpv);
      XRectangle rects[4];
      if(kind)
	{
	  int ox, oy;
	  EZ_GetWidgetTileOrigin(EZ_WidgetParent(wptr), &ox, &oy);
	  if(kind>1) { ox -= EZ_WidgetOriginX(wptr);   oy -= EZ_WidgetOriginY(wptr); }
	  XSetTSOrigin(EZ_Display, gc, ox, oy);
	}
      rects[0].x = 0;               rects[0].y = 0;
      rects[0].width = w;           rects[0].height = padb;
      rects[1].x = 0;               rects[1].y = padb;
      rects[1].width = padb;        rects[1].height = h-padb;
      rects[2].x = w-padb;          rects[2].y = padb;
      rects[2].width = padb;        rects[2].height = h-padb;
      rects[3].x = padb;            rects[3].y = h-padb;
      rects[3].width = w-padb-padb; rects[3].height = padb;
      XFillRectangles(EZ_Display, drawable, gc, rects, 4);
    }
}
/*************************************************************************/
void EZ_AddIdleCallBack(cb,data)
     EZ_CallBack cb;   void *data;
{
  EZ_AddCallBackWork(&EZ_IdleCallBacks, cb, data, 0, 0);
}
void EZ_RemoveIdleCallBack(cb, data)
     EZ_CallBack cb;   void *data;
{
  EZ_RemoveCallBackWork(&EZ_IdleCallBacks,cb, data);
}
void EZ_RemoveAllIdleCallBack()
{
  EZ_RemoveAllCallBacksWork(&EZ_IdleCallBacks);
}
void EZ_ExecuteIdleCallBacks()
{
  EZ_CallBackStruct *save, *ptr, *cblist = EZ_IdleCallBacks;
  if(cblist)
    {
      /* first rearrange the cblist */
      save = ptr = cblist->next;
      if(ptr)
        {
          while(ptr->next) ptr = ptr->next;
          ptr->next = cblist;
          cblist->next = NULL;
          EZ_IdleCallBacks = save;
        }
      if(cblist->callback) (cblist->callback)(NULL, cblist->data);
    }
}

void EZ_SetGlobalEventFilter(filter, data)
     EZ_EventFilter   filter; void *data;
{ EZ_GlobalEventFilter = filter; EZ_GlobalEventFilterData = data;}
/*************************************************************************/
int EZ_QueryPixelValue(widget, x, y, pv)
     EZ_Widget *widget; int x, y; unsigned long *pv;
{
  if(widget && EZ_WidgetMapped(widget))
    {
      if(x >= 0 && x < EZ_WidgetWidth(widget) && 
         y >=0 && y <= EZ_WidgetHeight(widget))
        {
          int    (*OldErrorHandler)();
          XImage *image;
          EZ_XErrorCode = 0;
          OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler);
          image = XGetImage(EZ_Display, EZ_WidgetWindow(widget),
                            x, y, 1, 1,
                            (1<< EZ_Depth)-1,
                            ZPixmap);
          XSync(EZ_Display, False);
          XSetErrorHandler(OldErrorHandler);
          if(EZ_XErrorCode == 0)
            {
              *pv = XGetPixel(image,0,0);
              XDestroyImage(image);
              return(1);
            }
        }
    }
  return(0);
}
/*************************************************************************/
static int setGlobalState(nptr, option, values)
     EZ_Widget *nptr;/*notused*/ int option; EZ_Value *values;
{
  switch(option)
    {
    case 9876540: 
      if(values[0].value.i) EZ_HighlightEnabled = 1;
      else  EZ_HighlightEnabled = 0;
      break;
    case 9876541: 
      if(values[0].value.i) EZ_OffsetEnabled = 1;
      else  EZ_OffsetEnabled = 0;
      break;
    case 9876542: 
      if(values[0].value.i) EZ_DepresionDisabled = 0;
      else  EZ_DepresionDisabled = 1;
      break;
    case 9876543: 
      if(values[0].value.i) EZ_BubbleHelpOn = 1;
      else  EZ_BubbleHelpOn = 0;
      break;
    case 9876544:
      {
        char *fname = values[0].value.p;
        EZ_SetDefaultLabelFont(fname);
      }
    break;
    case 9876545:
      {
        char *fname = values[0].value.p;
        EZ_SetDefaultEntryTextFont(fname);
      }
    break;
    case 9876546:
      {
        char *fname = values[0].value.p;
        EZ_SetDefaultMenuTitleFont(fname);
      }
    break;
    case 9876547:
      {
        char *color = values[0].value.p;
        EZ_KbdFocusHighlightColor = EZ_AllocateColorFromName(color);
      }
    break;
    case 9876548:
      {
        char *color = values[0].value.p;
        EZ_SetGlobalBackground(color);
      }
    break;
    case 9876549:
      {
        char *color = values[0].value.p;
        EZ_SetGlobalForeground(color);
      }
    break;
    case 9876550:
      EZ_AllowDualLabel(values[0].value.i);
      break;
    default:
      break;
    }
  return(0);
}
/****************************************************************/
static void setTreeLeaderWork(widget, leader, depth)
     EZ_Widget *widget, *leader; int depth;
{
  if(widget)
    {
      EZ_Widget *child = EZ_WidgetChildren(widget);
      EZ_WidgetGroupLeader(widget) = leader;
      setTreeLeaderWork(child, leader, 1);
      if(depth)
        {
          EZ_Widget *sib = EZ_WidgetSibling(widget);
          setTreeLeaderWork(sib, leader, 1);
        }
    }
}
/****************************************************************/

EZ_Widget *EZ_GetWidgetGroupLeader(widget)
     EZ_Widget *widget;
{
  if(widget) return(EZ_WidgetExist(EZ_WidgetGroupLeader(widget)));
  return(NULL);
}
void EZ_SetWidgetGroupLeader(widget, leader)
     EZ_Widget *widget, *leader;
{
  if(widget) { EZ_WidgetGroupLeader(widget) = leader;}
}
void EZ_SetWidgetTreeGroupLeader(widget, leader)
     EZ_Widget *widget, *leader;
{
  setTreeLeaderWork(widget, leader, 0);
}
void  EZ_SetCreateNewWidgetHook(hook)
     void (hook)();
{
  EZ_CreateNewWidgetHook = hook;
}
/****************************************************************/
extern EZ_AllWidgetHTEntry EZ_AllWidgetsHashTable[];

static void AlterDefaultForeground(oldfg) unsigned long oldfg;
{

  EZ_AllWidgetHTEntry  *tmp;  
  unsigned long newfg = EZ_ColorArray[3];
  int i;

  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_AllWidgetsHashTable[i].next;
      while(tmp)
        {
          EZ_Widget *widget = tmp->widget;
          if(EZ_WidgetForeground(widget) == oldfg)
            {
              EZ_WidgetForeground(widget) = newfg;
            }
          tmp = tmp->next;
        }
    }
  EZ_FreezeDisplay();
  EZ_SetItemDefaultFgAll(oldfg, newfg);
  EZ_ThawDisplay();
}
/****************************************************************/
void EZ_RedrawAll()
{
  EZ_AllWidgetHTEntry  *tmp;  
  int i;
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_AllWidgetsHashTable[i].next;
      while(tmp)
        {
          EZ_Widget *widget = tmp->widget;
          if(EZ_WidgetParent(widget) == NULL && EZ_WidgetMapped(widget))
            {
              EZ_DisplayWidget(widget);
            }
          tmp = tmp->next;
        }
    }
}
/****************************************************************/
void   EZ_AllowDualLabel(ok) int ok;
{
  int i, n =  (ok != 0); 
  if( EZ_PTLabel != n)
    {
      EZ_AllWidgetHTEntry  *tmp;  
      EZ_PTLabel = n;
      for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
        {
          tmp = EZ_AllWidgetsHashTable[i].next;
          while(tmp)
            {
              EZ_Widget *widget = tmp->widget;
              EZ_Widget *parent;
              int type = EZ_WidgetType(widget);
              switch(type)
                {
                case EZ_WIDGET_FRAME:
                case EZ_WIDGET_LABEL:
                case EZ_WIDGET_NORMAL_BUTTON:
                case EZ_WIDGET_CHECK_BUTTON:
                case EZ_WIDGET_RADIO_BUTTON:
                case EZ_WIDGET_MENU_BUTTON:
                case EZ_WIDGET_MENU_NORMAL_BUTTON:
                case EZ_WIDGET_MENU_CHECK_BUTTON:
                case EZ_WIDGET_MENU_RADIO_BUTTON:
                case EZ_WIDGET_MENU_SUBMENU:
                  parent = widget;
                  while(parent)
                    {
                      EZ_Widget *t = EZ_WidgetParent(parent);
                      EZ_ClearWidgetSizeComputedFlag(parent);
                      parent = t;
                    }
                  break;
                default:
                  break;
                }
              tmp = tmp->next;
            }
        }
      EZ_RedrawAll();
    }
}
/*********************************************************************/
#undef _EZ_WIDGET_INIT_C_
