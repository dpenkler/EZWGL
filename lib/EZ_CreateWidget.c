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
 ***              Creating Widgets                             ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_CREATE_WIDGET_C_
#include "EZ_Widget.h"
/*****************************************************************/
static EZ_Widget *newWidget MY_ANSIARGS((EZ_Widget *parent, int type,
                                         char *iname, char *cname));

extern int    EZ_CounterUT;
/*****************************************************************/
static void fixNBPage(widget) EZ_Widget *widget;
{
  char tmp[256], *tname;
  EZ_Widget *tab = EZ_NBPageCompanion(widget);
  tname = XrmQuarkToString(EZ_WidgetIName(widget));
  strcpy(tmp, tname); strcat(tmp, "Tab");
  EZ_WidgetIName(tab) = EZ_StringToQuark(tmp);
  tname = XrmQuarkToString(EZ_WidgetCName(widget));
  strcpy(tmp, tname); strcat(tmp, "Tab");
  EZ_WidgetCName(tab) = EZ_StringToQuark(tmp);
}
/*****************************************************************/

EZ_Widget *EZ_CreateWidgetXrm MY_VARARGS(int, the_args)
     MY_VARARGS_DECL
{
  EZ_Widget *parent, *widget;
  char      *iname,  *cname;
  XrmQuark  qiname,  qcname; 
  int       type;
  va_list   ap;  
  
  /* if(EZ_Display == NULL) return(NULL);*/
  type = (int)( MY_VARARGS_START(int, the_args, ap));
  parent = (EZ_Widget *)va_arg(ap, void *);
  iname = (char *)va_arg(ap, char *);
  cname = (char *)va_arg(ap, char *);
  if(iname != NULL) qiname = EZ_StringToQuark(iname);
  else qiname = NULLQUARK;
  if(cname != NULL) qcname = EZ_StringToQuark(cname);
  else qcname = NULLQUARK;
  
  if( (widget = newWidget(parent, type, iname, cname)) == NULL)
    {  va_end(ap); return(NULL);}

  /* set resources from app-defaults */
  if(qiname != NULLQUARK) EZ_WidgetIName(widget) = qiname;
  if(qcname != NULLQUARK) EZ_WidgetCName(widget) = qcname;
  
  /* set widget attributes. These are considered as fallback resources */
  {
    int     i1,i2,i3,i4;
    void    *p1, *p2;
    unsigned long ul1, ul2;
    float   f1,f2;
    int     option, done = 0, size_changed = 0;
#include "EZ_WidgetAttr.h"    
  }

  if(type == EZ_WIDGET_NB_PAGE) fixNBPage(widget) ;

  EZ_GetAndUseWidgetResources(widget);
  return(widget);
}
/**********************************************************************/

EZ_Widget *EZ_CreateWidget MY_VARARGS(int, the_args)
     MY_VARARGS_DECL
{
  EZ_Widget *parent, *widget;
  int type;
  va_list   ap;  

  /* if(EZ_Display == NULL) return(NULL);*/
  
  type = (int)( MY_VARARGS_START(int, the_args, ap));
  parent = (EZ_Widget *)va_arg(ap, void *);

  if( (widget = newWidget(parent, type, NULL, NULL)) == NULL)
    {  va_end(ap); return(NULL);}
  /* set widget attributes. These are considered as fallback resources */
  {
    int     i1,i2,i3,i4;
    void    *p1, *p2;
    unsigned long ul1, ul2;
    float   f1,f2;
    int     option, done = 0, size_changed = 0;
#include "EZ_WidgetAttr.h"   
  }

  if(type == EZ_WIDGET_NB_PAGE) fixNBPage(widget) ;

  /* set resources from app-defaults */
  EZ_GetAndUseWidgetResources(widget);
  return(widget);
}
/************************************************************************************/  

static EZ_Widget *newWidget(parent, type, iname, cname)
     EZ_Widget *parent; int type;
     char *iname, *cname;
{
  EZ_Widget *widget = NULL;


  switch(type)
    {
    case EZ_WIDGET_FRAME:
    case EZ_WIDGET_ROW_COLUMN:  /* grid bag is obsolete */
      widget = EZ_CreateFrame(parent, NULL);
      break;
    case EZ_WIDGET_LABEL:
      widget = EZ_CreateLabel(parent,NULL);
      break;
    case EZ_WIDGET_ARROW_BUTTON:
      widget = EZ_CreateArrowButton(parent, 13);
      break;
    case EZ_WIDGET_NORMAL_BUTTON:
      widget = EZ_CreateButton(parent, NULL, -1);
      break;
    case EZ_WIDGET_CHECK_BUTTON:
      widget = EZ_CreateCheckButton(parent, NULL, -1, 1, 0, 0);
      break;
    case EZ_WIDGET_RADIO_BUTTON:
      widget = EZ_CreateRadioButton(parent, NULL, -1, 
				    (0x12345 + EZ_CounterUT++), (0x12345 + EZ_CounterUT++));
      break;
    case EZ_WIDGET_MENU_BUTTON:
      widget = EZ_CreateMenuButton(parent, NULL, -1);
      break;
    case EZ_WIDGET_MENU_SEPARATOR:
      widget = EZ_CreateMenuSeparator(parent);
      break;
    case EZ_WIDGET_MENU_SUBMENU:
      widget = EZ_CreateMenuSubMenu(parent, NULL, -1);
      break;
    case EZ_WIDGET_MENU_NORMAL_BUTTON:
      widget = EZ_CreateMenuNormalButton(parent, NULL, -1, 0);
      break;
    case EZ_WIDGET_MENU_CHECK_BUTTON:
      widget = EZ_CreateMenuCheckButton(parent, NULL, -1, 1, 0, 0, 0);
      break;
    case EZ_WIDGET_MENU_RADIO_BUTTON:
      widget = EZ_CreateMenuRadioButton(parent, NULL, -1, 
					(0x12345 + EZ_CounterUT++), (0x12345 + EZ_CounterUT++), 0);
      break;
    case EZ_WIDGET_VERTICAL_SLIDER:
      widget = EZ_CreateSlider(parent, NULL, 0.0, 1.0, 0.0, EZ_WIDGET_VERTICAL_SLIDER);
      break;
    case EZ_WIDGET_HORIZONTAL_SLIDER:
      widget = EZ_CreateSlider(parent, NULL, 0.0, 1.0, 0.0, EZ_WIDGET_HORIZONTAL_SLIDER);
      break;
    case EZ_WIDGET_ENTRY:
      widget = EZ_CreateEntry(parent, NULL);
      break;
    case EZ_WIDGET_3D_CANVAS:
      widget = EZ_Create3DCanvas(parent);
      break;
    case EZ_WIDGET_VERTICAL_SCROLLBAR:
      widget = EZ_CreateScrollbar(parent, EZ_WIDGET_VERTICAL_SCROLLBAR);
      break;
    case EZ_WIDGET_HORIZONTAL_SCROLLBAR:
      widget = EZ_CreateScrollbar(parent, EZ_WIDGET_HORIZONTAL_SCROLLBAR);
      break;
    case EZ_WIDGET_ILIST_BOX:
      widget = EZ_CreateIListBox(parent);
      break;
    case EZ_WIDGET_LIST_BOX:
      widget = EZ_CreateListBox(parent, 1, 1);
      break;
    case EZ_WIDGET_ITEXT:
      widget = EZ_CreateIText(parent,1);
      break;
    case EZ_WIDGET_TEXT:
      widget = EZ_CreateTextWidget(parent, 1, 1, 1);
      break;
    case EZ_WIDGET_NOTE_BOOK:
      widget = EZ_CreateNoteBook(parent);
      break;
    case EZ_WIDGET_NB_PAGE:
      widget = EZ_CreateNoteBookPage(parent, NULL, -1);
      break;
    case EZ_WIDGET_NW_LABEL:
      widget = EZ_CreateNWLabel(parent, NULL);
      break;
    case EZ_WIDGET_PANE_HANDLE:
      widget = EZ_CreatePaneHandle(parent);
      break;
    case EZ_WIDGET_EXECUTOR:
      widget = EZ_CreateExecutor(parent);
      break;
    case EZ_WIDGET_OPTIONAL_ENTRY:
      widget = EZ_CreateOptionalEntry(parent, NULL,1,1);
      break;
    case EZ_WIDGET_FILE_SELECTOR:
      widget = EZ_CreateFileSelector(parent, NULL);
      break;
    case EZ_WIDGET_ITREE:
      widget = EZ_CreateITree(parent);
      break;
    case EZ_WIDGET_TREE:
      widget = EZ_CreateListTree(parent, 1,1);
      break;
    case EZ_WIDGET_IFANCY_LIST_BOX:
      widget = EZ_CreateIFListBox(parent, 1, 0);
      break;
    case EZ_WIDGET_FANCY_LIST_BOX:
      widget = EZ_CreateFancyListBox(parent,1,1,1,0);
      break;
    case EZ_WIDGET_IWORK_AREA:
      widget = EZ_CreateIWorkArea(parent);
      break;
    case EZ_WIDGET_WORK_AREA:
      widget = EZ_CreateWorkArea(parent,1,1);
      break;
    case EZ_WIDGET_POPUP_MENU:
      widget = EZ_CreatePopupMenu(NULL);
      break;
    case EZ_WIDGET_ITOPLEVEL:
      widget = EZ_CreateIToplevelFrame(parent, NULL);
      break;
    case  EZ_WIDGET_EMBEDER:
      widget = EZ_CreateEmbeder(parent);
      break;
    case EZ_WIDGET_RAW_XWINDOW:
      widget = EZ_CreateRawXWindow(parent);
      break;
    case EZ_WIDGET_MENU_TEAR_OFF_BAR:
      widget = EZ_CreateMenuTearOffItem(parent);
      break;   
    case EZ_WIDGET_LCD:
      widget = EZ_CreateLcd(parent, 6);
      break;
    case EZ_WIDGET_LED:
      widget = EZ_CreateLed(parent);
      break;
    case EZ_WIDGET_STATUS_METER:
      widget = EZ_CreateStatusMeter(parent);
      break;      
    case EZ_WIDGET_MENU_BAR:
      widget = EZ_CreateMBar(parent);
      break;
    case EZ_WIDGET_ITERM:
      widget = EZ_CreateITerm(parent);
      break;
    case EZ_WIDGET_TERM:
      widget = EZ_CreateTerm(parent,1);
      break;
    case EZ_WIDGET_HORIZONTAL_RULER:   
      widget=EZ_CreateRuler(parent,EZ_WIDGET_HORIZONTAL_RULER);
      break;
    case EZ_WIDGET_VERTICAL_RULER:   
      widget=EZ_CreateRuler(parent,EZ_WIDGET_VERTICAL_RULER);
      break;
    case EZ_WIDGET_SCROLL_BUTTON:
      widget=EZ_CreateScrollButton(parent, EZ_UP_DOWN);
      break;
    case EZ_WIDGET_SPIN_BUTTON:
      widget=EZ_CreateSpinButton(parent);
      break;
    case EZ_WIDGET_LOCATOR:
      widget=EZ_CreateLocator(parent);
      break;
    case EZ_WIDGET_DIAL:
      widget=EZ_CreateDial(parent);      
      break;
    case EZ_WIDGET_GRADIENT_BAR:
      widget=EZ_CreateGradientBar(parent);      
      break;      
    case EZ_WIDGET_HISTOGRAM:
      widget=EZ_CreateHistogram(parent);      
      break;
    case EZ_WIDGET_SEPARATOR:
      widget=EZ_CreateSeparator(parent);      
      break;
    case EZ_WIDGET_SPREAD_SHEET: 
      widget=EZ_CreateSpreadSheet(parent);           
      break;
    case EZ_WIDGET_THUMB_WHEEL:
      widget=EZ_CreateThumbWheel(parent);                 
      break;
    default:
      widget = EZ_CreateUDFWidget(type, parent, iname, cname);
      break;
    }
  return(widget);
}
/**********************************************************************************************/
#undef _EZ_CREATE_WIDGET_C_



