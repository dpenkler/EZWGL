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
 *  May 1997. Handling of raw rgb images added by Arno Dabekaussen.
 */
/*****************************************************************
 ***                                                           ***
 ***              Widget Configuration                         ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_CONFIGURE_C_

#include "EZ_Widget.h"
#include "EZ_WidgetConfig.h"

/*********************************************************************
 * 
 * global functions
 */
int  EZ_SetWidgetAttributes MY_ANSIARGS((EZ_Widget *widget, int opt, 
					 int i1, int i2, 
					 void *p1, void *p2,
					 unsigned long ul1, unsigned long ul2,
					 float f1, float f2,
					 int i3, int i4));

int  EZ_GetWidgetAttribute MY_ANSIARGS((EZ_Widget *widget, int option,  int oparam,
                                         EZ_Value *vreturn));
void  EZ_SetKeyShortcut MY_ANSIARGS((EZ_Widget *widget, char *str));
void  EZ_ConfigureError MY_ANSIARGS((EZ_Widget *widget, int option, char *msg));
int   EZ_SetWidgetPixmap MY_ANSIARGS((EZ_Widget *widget, EZ_Bitmap *bitmap, int option));
void  EZ_MarkWidgetDirty MY_ANSIARGS((EZ_Widget *widget));

char *EZ_GetOptionSymbolicName MY_ANSIARGS((int i));

/*********************************************************************
 * 
 *  Local functions
 */


static int  EZ_ScanShortcut MY_ANSIARGS((EZ_Widget *widget, char *str, char **list, int *modifier));
/*********************************************************************/
#define EZ_OptionIdNames(_id_, _type_, _name_, _i_name_, _c_name_) _name_,
char *EZ_ConfigureOptionNames[] = {
#include "EZ_WidgetConfig.h"
  "?", "?", "?", "?", "?",
NULL,
};
#undef EZ_OptionIdNames
/*********************************************************************/
#define EZ_OptionIdNames(_id_, _type_, _name_, _i_name_, _c_name_) _type_,
char *EZ_ConfigureOptionTypes[] = {
#include "EZ_WidgetConfig.h"
  "?", "?", "?", "?", "?",
  NULL,
};
#undef EZ_OptionIdNames
/**********************************************************************/


/*********************************************************************
 * 
 *  Configure a widget
 */
void  EZ_ConfigureWidget MY_VARARGS(void *,  the_args)
     MY_VARARGS_DECL
{
  EZ_Widget *widget;
  int       size_changed = 0;
  va_list   ap;

  widget = (EZ_Widget *)( MY_VARARGS_START(void *, the_args, ap));

  if(EZ_LookupWidgetFromAllHT(widget) == (EZ_Widget *)NULL)
    {
      (void)fprintf(stderr,"Error: In EZ_ConfigureWidget, widget %lx does not exist.\n",
		    (unsigned long)widget);
      return;
    }
  /* parse the var args */
  {
    int       option, done = 0;
    int       i1,i2,i3,i4;
    void      *p1, *p2;
    float     f1, f2;
    unsigned long ul1, ul2;
#include "EZ_WidgetAttr.h"
  }
  
  /* redisplay if needed */
  if(size_changed >= 0)
    {
      if(size_changed != 0) EZ_ClearWidgetSizeComputedFlag(widget);
      if(EZ_GetWidgetNonActiveFlag(widget) == 0 &&  
	 EZ_GetWidgetFreezedFlag(widget) == 0 && EZ_WidgetMapped(widget))
	EZ_ReDisplayWidget(widget);
    }
  else if( size_changed & (-(1<<30)))
    { EZ_SetWidgetSizeComputedFlag(widget); }
  else {  EZ_ClearWidgetSizeComputedFlag(widget);}
  
  if(EZ_WidgetType(widget) == EZ_WIDGET_LED)
    EZ_LEDStatus(widget) &= ~LED_BLOCKED;
}

int  EZ_SetWidgetAttributes(widget,option, i1,i2,p1,p2,ul1,ul2,f1,f2,i3,i4)
     EZ_Widget *widget;
     int option, i1,i2,i3,i4;
     void *p1, *p2;
     unsigned long ul1, ul2;
     float f1,f2;
{
  int size_changed = 0;
  int shape_changed = 0;

  if(widget)
    {
      int type = EZ_WidgetType(widget);
      int value;

      switch(option)
	{
	case EZ_SHEET_HEADER_SIZE:
	  if(type == EZ_WIDGET_SPREAD_SHEET)
	    {
	      EZ_SSheetTWidth(widget) = i1;
	      EZ_SSheetTHeight(widget) = i2;
	      EZ_SSheetDirty(widget)=1;
	      size_changed = 1;
	    }
	  break;
	case EZ_SHEET_CELL_SIZE:
	  if(type == EZ_WIDGET_SPREAD_SHEET)
	    {
	      EZ_SSheetCellWidth(widget) = i1;
	      EZ_SSheetCellHeight(widget) = i2;
	      EZ_SSheetDirty(widget)=1;
	      size_changed = 1;
	    }
	  break;
        case EZ_RESOURCE_STRING:
          if(EZ_ParseWidgetResourceString(widget, (char *)p1))
            {
              size_changed = 1;
              EZ_RecordResourceString(&(EZ_WidgetResourceString(widget)), (char *)p1);
            }
          break;
        case EZ_ROW_COLUMN_MAX_SIZE:
	  size_changed = EZ_SetRowColMaxSize(widget, i1);          
          break;
        case EZ_SEPARATOR_STYLE:
          if(type == EZ_WIDGET_SEPARATOR)
            {
              EZ_SeparatorStyle(widget) = i1;
            }
          break;
        case  EZ_BAR_ORIENTATION:
          if(type == EZ_WIDGET_GRADIENT_BAR)
            {
              EZ_GBarOrientation(widget) = i1;
            }
          else if(type == EZ_WIDGET_HISTOGRAM)
            {
              EZ_HgramOrientation(widget) = i1;
            }
          else if(type == EZ_WIDGET_FRAME)
            {
              EZ_FrameBarOrientation(widget) = i1;
            }
          break;          
        case EZ_BAR_EDITABLE:
          if(type == EZ_WIDGET_GRADIENT_BAR)
            {
              EZ_GBarEditable(widget) = i1;
            }
          break;
        case EZ_BAR_COLOR_N:
          {
            char *color = (char *)p1;
            if(type == EZ_WIDGET_GRADIENT_BAR)
              {
                i1 = (i1 < 0? 0 : i1 >=12 ? 12 : i1);
                if(!color || !strcmp(color,"NULL") || !strcmp(color,"nil"))
                  EZ_GBarNColors(widget) = i1;
                else if(color)
                  {
                    unsigned long  pixel_value = EZ_AllocateColorFromName(color);
                    EZ_GBarColors(widget)[i1] = pixel_value;
                    if(EZ_GBarNColors(widget) <= i1) EZ_GBarNColors(widget) = i1+1;
                  }
                EZ_GBarOrientationOld(widget) = 123; /* force recreate color bar*/
                size_changed = 1;
              }
            else if(type == EZ_WIDGET_HISTOGRAM)
              {
                if(!color || !strcmp(color,"NULL") || !strcmp(color,"nil"))
                  EZ_HgramNColors(widget) = i1;
                else if(color)
                  {
                    unsigned long  pixel_value = EZ_AllocateColorFromName(color);
                    if(i1 < EZ_HgramNColorCells(widget))
                      {
                        EZ_HgramColors(widget)[i1] = pixel_value;
                        if(EZ_HgramNColors(widget) <= i1)
                          EZ_HgramNColors(widget) = i1+1;
                      }
                  }
                EZ_HgramOrientationOld(widget) = 123; /* force recreate color bar*/
                size_changed = 1;
              }
          }
         break;
	case EZ_BAR_RANGE: 
	  {
	    float from, to;
	    from = f1;
	    to =  f2;
            if(from == to) to += 1.0;
            if(type == EZ_WIDGET_GRADIENT_BAR)
              {
                float *ff = EZ_GBarRange(widget);
                ff[0] = from; ff[1] = to;
                size_changed = 1;
              }
            else if(type == EZ_WIDGET_HISTOGRAM)
              {
                float *ff = EZ_HgramRange(widget);
                ff[0] = from; ff[1] = to;
                size_changed = 1;
                EZ_HgramOrientationOld(widget) = 123; /* force recreate color bar*/                
              }
	  }
	  break;
        case EZ_BAR_WIDTH:
          {
            int value = i1 > 0? i1 : 1;
            if(type == EZ_WIDGET_GRADIENT_BAR)
              {
                EZ_GBarBWidthSet(widget) = value;
                size_changed = 1;
              }
            else if(type == EZ_WIDGET_HISTOGRAM)
              {
                EZ_HgramBinWidth(widget) = value;
                size_changed = 1 ;
              }
            else if(type == EZ_WIDGET_SEPARATOR)
              {
                EZ_SeparatorWidth(widget) = value;
                size_changed = 1 ;
              }
          }
          break;
        case EZ_HISTOGRAM_SAMPLE:
          {
            if(type == EZ_WIDGET_HISTOGRAM)
              {
                EZ_HgramNBins(widget) = i1;
                size_changed = 1 ;
              }
          }
          break;
        case EZ_DIAL_COLOR:
          if(type == EZ_WIDGET_DIAL)
            {
              char *color = (char *)p1;
              unsigned long  pixel_value;
              pixel_value = EZ_AllocateColorFromName(color);
              if(pixel_value == 0) pixel_value = ~0;
              EZ_DialForeground(widget) = pixel_value;
            }
          break;
        case EZ_DIAL_VALUE:
          if(type == EZ_WIDGET_DIAL)
            EZ_SetDialValue(widget, (float)f1);
          break;
        case EZ_DIAL_STYLE:
          if(type == EZ_WIDGET_DIAL)
            {          
              EZ_DialStyle(widget) = (i1 != 0);
              size_changed =1 ;
            }
          break;
        case EZ_DIAL_DISPLAY_VALUE:
          if(type == EZ_WIDGET_DIAL)
            {
              int value = i1, nnn = 0;
              if(value < 0) {nnn = 1; value = (value == -1)? 0: -value;}
	      if(EZ_DialShowValue(widget) != value ||
                 EZ_DialNoCValue(widget) != nnn)
                {
                  EZ_DialShowValue(widget) = (unsigned char)value;
                  EZ_DialNoCValue(widget) =  (unsigned char)nnn;
                  size_changed =1 ;
                }
            }
          break;
        case EZ_DIAL_SPAN:
          if(type == EZ_WIDGET_DIAL)
            {
              EZ_DialSpan(widget)=(float)f1;
              size_changed=1;
            }
          break;
        case EZ_DIAL_RANGE:
          if(type == EZ_WIDGET_DIAL)
            {
              float from = (float)f1;
              float to = (float)f2;
              if(from == to) to += 1.0;
              EZ_DialFrom(widget)= from;
              EZ_DialTo(widget)= to;
              size_changed =1;
            }
          break;
        case EZ_DIAL_RESOLUTION:
          if(type == EZ_WIDGET_DIAL)
            {
              float ff = (float)f1;
              EZ_DialResolution(widget) = ff >=0.0? ff: -ff;
            }
          break;
        case EZ_SHOW_TICK:
          {
            if((type == EZ_WIDGET_HORIZONTAL_SLIDER || type == EZ_WIDGET_VERTICAL_SLIDER))
              {
                EZ_SliderNTicks(widget) = i1;
                EZ_SliderTickSize(widget) = i2;
                size_changed =1 ;
              }
            else if(type == EZ_WIDGET_DIAL)
              {
                EZ_DialNTicks(widget) = i1;
                EZ_DialTickSize(widget) = i2;
                size_changed =1 ;
              }
            else if(type == EZ_WIDGET_SPREAD_SHEET)
              {
                EZ_SSheetTabChar(widget) = (char)i1;
                EZ_SSheetNewlineChar(widget) = (char)i2;
              }
          }
          break;
        case EZ_SPECIAL_EFFECT:
          {
            int ocode = EZ_WidgetSECode(widget);
            int oparam = EZ_WidgetSEParam(widget);
            size_changed = (ocode != i1 || oparam != i2);
            if( size_changed)
              {
                EZ_WidgetSECode(widget) = (unsigned char)i1;
                EZ_WidgetSEParam(widget) = (unsigned char)i2;  
              }
          }
          break;
        case EZ_LOCATOR_XY:
          if(type == EZ_WIDGET_LOCATOR)
            EZ_SetLocatorXY(widget, f1, f2);
          break;
        case EZ_SPIN_VALUE:
          if(type == EZ_WIDGET_SPIN_BUTTON)
            {
              EZ_Widget *entry = EZ_SpinButtonEntry(widget);
              EZ_SetEntryString(entry, (char *)p1);
              EZ_SpinButtonCurrent(widget) = i1;
            }
          break;
        case EZ_SPIN_FUNCTION:
          if(type == EZ_WIDGET_SPIN_BUTTON)
            {
              EZ_SpinButtonNextF(widget) = (char *(*) MY_ANSIARGS((int, int, void *))) p1;
              EZ_SpinButtonCData(widget) = (void *)p2;
            }
          break;
        case EZ_SCROLL_TYPE:
          if(type == EZ_WIDGET_SCROLL_BUTTON)
            { EZ_ScrollButtonType(widget) = i1; }
          break;
        case EZ_RESOURCES_HANDLE:
          EZ_RegisterResourcesWidget(widget, i1, p1, p2);
          break;
        case EZ_OVERRIDE_REDIRECT:
          EZ_WidgetOverrideRedirect(widget) = (i1 != 0);
          break;
        case EZ_RULER_DISPLAY_VALUE:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
	      EZ_RulerDisplayValue(widget) = (i1 != 0);
	      size_changed = 1;
            }
          else EZ_ConfigureError(widget,option, "option not applicable");	  
          break;
        case EZ_RULER_TICK_UNIT:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
	      EZ_RulerTickSize(widget) = i1;
              EZ_RulerTickError(widget)= 0.0;
	      size_changed = 1;
            }
          else EZ_ConfigureError(widget,option, "option not applicable");	  
          break;
        case EZ_RULER_TICK_SCALE:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
	      EZ_RulerTickScale(widget) = i1;
	      size_changed = 1;
            }
          else EZ_ConfigureError(widget,option, "option not applicable");	  
          break;
        case EZ_RULER_TICK_OFFSET:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
	      EZ_RulerTOffset(widget) = i1;
	      size_changed = 1;
            }
          else EZ_ConfigureError(widget,option, "option not applicable");	  
          break;
        case EZ_RULER_TICK_LENGTH:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
	      EZ_RulerTickLength(widget) = i1;
	      size_changed = 1;
            }
          else EZ_ConfigureError(widget,option, "option not applicable");	  
          break;
        case EZ_RULER_POSITION:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
	      EZ_RulerOrientation(widget) = i1;
	      size_changed = 1;
            }
          else EZ_ConfigureError(widget,option, "option not applicable");	  
          break;
        case EZ_TERM_SAVE_LINES:
          {
            EZ_Widget *twidget = NULL;
            if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)
              twidget = EZ_TermITerm(widget);
            else  if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)
              twidget = widget;
            if(twidget)
              {
                i1 = i1 >=0 ? i1: 0;
                EZ_ITermSaveLines(twidget) = i1;
                if(i1==0) { EZ_DeActivateWidget(EZ_ITermVScroll(twidget));}
                else { EZ_ActivateWidget(EZ_ITermVScroll(twidget));}
              }
          }
        break;
        case EZ_TERM_REVERSE_VIDEO:
          {
            EZ_Widget *twidget = NULL;
            if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)
              twidget = EZ_TermITerm(widget);
            else  if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)
              twidget = widget;
            if(twidget) { EZ_ITermReverseVideo(twidget, (i1 !=0)); }
          }
        break;
        case EZ_TERM_CURSOR_COLOR:
          {
            EZ_Widget *twidget = NULL;
            char *color = (char *)p1;
            unsigned long  pixel_value = EZ_AllocateColorFromName(color);
            if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)
              twidget = EZ_TermITerm(widget);
            else  if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)
              twidget = widget;
            if(twidget) EZ_ITermCursorColor(widget) = pixel_value;          
          }
        break;
        case EZ_TERM_COLOR_N:        
          {
            EZ_Widget *twidget = NULL;
            char *color = (char *)p1;
            unsigned long  pixel_value = EZ_AllocateColorFromName(color);
            if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)
              twidget = EZ_TermITerm(widget);
            else  if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)
              twidget = widget;
            EZ_SetITermColors(twidget, i1, pixel_value);
          }
        break;
        case EZ_LABEL_SHADOW: 
          {
            int sx, sy, sgnx=0, sgny=0;
            unsigned char uc;
            if(i1<=0) { sgnx=8; i1 = -i1;}
            if(i2<0)  { sgny=8; i2 = -i2;}
            sx = (i1 & 0xf)|sgnx; 
            sy = (i2 & 0xf)|sgny;
            uc = (unsigned char)((sx<<4) | (sy));
            EZ_WidgetLabelShadow(widget) = uc;
            size_changed = 1;
          }
          break;
        case EZ_WM_IGNORE:
          if(i1 && EZ_GetWidgetToplevelFlag(widget) == 0)
            {
              Window win = EZ_WidgetWindow(widget);
              EZ_SetWidgetToplevelFlag(widget);
              if(win)
                {
                  XSetWindowAttributes   setWAttr;
                  unsigned long          valuemask = CWOverrideRedirect;
                  setWAttr.override_redirect = True;
                  XChangeWindowAttributes(EZ_Display, win, valuemask, &setWAttr);
                }
            }
          else if(EZ_GetWidgetToplevelFlag(widget))
            { 
              Window win = EZ_WidgetWindow(widget);
              EZ_ClearWidgetToplevelFlag(widget);
              if(win)
                {
                  XSetWindowAttributes   setWAttr;
                  unsigned long          valuemask = CWOverrideRedirect;
                  setWAttr.override_redirect = False;
                  XChangeWindowAttributes(EZ_Display, win, valuemask, &setWAttr);
                }                
            }
          break;
        case EZ_TILE_ORIGIN:
          if(i1)
            {
              EZ_WidgetTOX(widget) = i2;
              EZ_WidgetTOY(widget) = i3;
              EZ_WidgetTOSet(widget) = i1;
            }
          else {  EZ_WidgetTOSet(widget) = 0;}
          size_changed = 1;
          break;
        case EZ_OPTIONAL_ENTRY_STYLE:
          if(type == EZ_WIDGET_OPTIONAL_ENTRY)
            EZ_SetOptionalEntryStyle(widget, i1);
          break;
        case EZ_INTERIOR_BORDER:          
          if(type == EZ_WIDGET_FRAME)
            {
              int j1 = EZ_FrameIntBdWidth(widget);
              int j2 = EZ_FrameIntBdType(widget);
              EZ_FrameIntBdWidth(widget) = i1;
              EZ_FrameIntBdType(widget)  = i2;
              if(i1 != j1 || i2 != j2) size_changed = 1;
            }
          else if(type == EZ_WIDGET_ITOPLEVEL)
            {
              int j1 = EZ_LabelIBorderType(widget);
              int j2 = EZ_LabelIBorderWidth(widget);
              EZ_LabelIBorderWidth(widget) = i1;
              EZ_LabelIBorderType(widget)= i2;
              if(i1 != j1 || i2 != j2) size_changed = 1;
            }
          break;
        case EZ_ARROW_TYPE:
          {
            if(type == EZ_WIDGET_NORMAL_BUTTON)
              {
                if(i1)
                  {
                    EZ_ArrowButtonType(widget) = i1; 
                    EZ_WidgetType(widget) = EZ_WIDGET_ARROW_BUTTON;
                    EZ_InitArrowButtonHandle();
                    size_changed  = 1;
                  }
              }
            else if(type ==  EZ_WIDGET_ARROW_BUTTON)
              {
                if(i1 == 0 && EZ_ButtonIsArrowButton(widget) == 0)
                  {
                    EZ_WidgetType(widget) = EZ_WIDGET_NORMAL_BUTTON;
                  }
                EZ_ArrowButtonType(widget) = i1; 
                size_changed  = 1;
              }
            else if(type == EZ_WIDGET_LABEL || type == EZ_WIDGET_NW_LABEL)
              { EZ_LabelArrowType(widget) = (char)i1; size_changed  = 1;}
            else if(type == EZ_WIDGET_SCROLL_BUTTON)
              { EZ_ScrollButtonArrowType(widget) = (char)i1; size_changed  = 1;} 
            else EZ_ConfigureError(widget,option, "option not available");
          }
        break;
	case EZ_ROW_BG:
          switch(type)
            {
            case EZ_WIDGET_FANCY_LIST_BOX: case EZ_WIDGET_IFANCY_LIST_BOX:
	      EZ_SetFancyListBoxRowBg(widget, i1, p1, p2);
	      size_changed  = 1;
              break;
            case EZ_WIDGET_TREE: case EZ_WIDGET_ITREE:
	      EZ_SetTreeWidgetRowBg(widget, i1, p1, p2);
	      size_changed  = 1;
              break;
            case EZ_WIDGET_LIST_BOX: case EZ_WIDGET_ILIST_BOX:
	      EZ_SetListBoxRowBg(widget, i1, p1, p2);
	      size_changed  = 1;
              break;
            case EZ_WIDGET_OPTIONAL_ENTRY:
              {
                EZ_Widget *listbox = (EZ_Widget *)EZ_OptEntryListbox(widget);
                EZ_SetListBoxRowBg(listbox, i1, p1, p2);                
              }
              break;
            case EZ_WIDGET_FILE_SELECTOR:
              {
                EZ_Widget *oea =EZ_FileSelectorPatternEntry(widget);
                EZ_Widget *oeb =EZ_FileSelectorSelectionEntry(widget);
                EZ_Widget *lbxa=EZ_FileSelectorDirListbox(widget);
                EZ_Widget *lbxb=EZ_FileSelectorFileListbox(widget);
                EZ_SetListBoxRowBg(lbxa, i1, p1, p2);                
                EZ_SetListBoxRowBg(lbxb, i1, p1, p2); 
                EZ_SetListBoxRowBg(EZ_OptEntryListbox(oea), i1, p1, p2);
                EZ_SetListBoxRowBg(EZ_OptEntryListbox(oeb), i1, p1, p2);
              }
              break;
            default:
              break;
            }
	  break;
	case EZ_DOCKABLE:
	  if((type == EZ_WIDGET_FRAME || type== EZ_WIDGET_MENU_BAR) &&
	     EZ_FrameUndockable(widget) == 0)
	    {
	      EZ_FrameDockable(widget) = (i1 != 0);
	      if(i1 && EZ_FrameDragHandle(widget) == 0)
		EZ_FrameDragHandle(widget) = (i1&3) ? (i1&3): 1;
	      size_changed = 1;
	    }
	  else EZ_ConfigureError(widget,option, "option not available");
	  break;
	case EZ_DRAG_HANDLE:
	  if( i1 >= 0 && i1 < 4) 
	    {
	      if((type == EZ_WIDGET_FRAME || type== EZ_WIDGET_MENU_BAR) &&
		 EZ_FrameUndockable(widget) == 0)
		{
		  if(EZ_FrameDragHandle(widget) != i1)
		    {
		      EZ_FrameDragHandle(widget) = i1;
		      size_changed = 1;
		    }
		}
	      else EZ_ConfigureError(widget,option, "option not available");
	    }
	  else EZ_ConfigureError(widget,option, "value out of range");
	  break;
	case EZ_FORGET_X:
	  if(i1) {EZ_SetWidgetForgetXFlag(widget);}
	  else  {EZ_ClearWidgetForgetXFlag(widget);}
	  break;
	case EZ_FORGET_Y:
	  if(i1) {EZ_SetWidgetForgetYFlag(widget);}
	  else  {EZ_ClearWidgetForgetYFlag(widget);}
	  break;
	case EZ_FORGET_W:
	  if(i1) {EZ_SetWidgetForgetWFlag(widget);}
	  else  {EZ_ClearWidgetForgetWFlag(widget);}
	  break;
	case EZ_FORGET_H:
	  if(i1) {EZ_SetWidgetForgetHFlag(widget);}
	  else  {EZ_ClearWidgetForgetHFlag(widget);}
	  break;
	case EZ_FORGET_POSITION:
	  if(i1) {EZ_SetWidgetForgetXYFlag(widget);}
	  else  {EZ_ClearWidgetForgetXYFlag(widget);}
	  break;
	case EZ_FORGET_SIZE:
	  if(i1) {EZ_SetWidgetForgetWHFlag(widget);}
	  else {EZ_ClearWidgetForgetWHFlag(widget);}
	  break;
	case EZ_FORGET_GEOMETRY:
	  if(i1) {EZ_SetWidgetForgetGFlag(widget);}
	  else {EZ_ClearWidgetForgetGFlag(widget);}
	  break;
	  /* status meter config options. 2/6/98 */
	case EZ_SMETER_STYLE:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
	      if(i1 > 0 && i2 > 0 && i1 < 100 && i2 < 100)
		{
		  EZ_SMeterBarWidth(widget) = i1;
		  EZ_SMeterBarSep(widget) = i2;
		  EZ_SMeterStyle(widget) = 1;
		}
	      else EZ_SMeterStyle(widget) = 0;
	    }
	  break;
	case EZ_SMETER_ORIENTATION:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    if(EZ_SMeterOrientation(widget) != i1)
	      {
		EZ_SMeterOrientation(widget) = (char )i1;
		size_changed = 1;
	      }
	  break;
	case EZ_SMETER_FOREGROUND_N:
	    if(type == EZ_WIDGET_STATUS_BAR)
	      {
		if(i1 >= 0 && i1 < 5)
		  {
		    char *color = (char *)p1;
		    unsigned long  pixel_value = EZ_AllocateColorFromName(color);
		    EZ_SMeterFColor(widget)[i1] = pixel_value;
		  }
	      }
	    break;
	case EZ_SMETER_RANGE_N:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
	      if(i1 >= 0 && i1 < 5)
		{
		  float *ff = EZ_SMeterValues(widget);
		  ff[ 3*i1] = f1; ff[ 3*i1 +1] = f2;
		  if(EZ_SMeterNControls(widget) <= i1)
		    EZ_SMeterNControls(widget) = i1+1;
		}
	    }
	  break;
	case EZ_SMETER_VALUE_N:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
	      if(i1 >= 0 && i1 < 5)
		{
		  float *ff = EZ_SMeterValues(widget);
		  ff[ 3*i1 + 2] = f1;
		  if(EZ_SMeterNControls(widget) <= i1)
		    EZ_SMeterNControls(widget) = i1+1;
		}
	    }
	  break;
	case EZ_SMETER_SHOW_VALUE:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    EZ_SMeterShowValue(widget) = (i1 != 0);
	  break;
	case EZ_SMETER_VALUE:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
	      float *ff = EZ_SMeterValues(widget);
	      ff[2] = f1;
	    }
	  break;
	case EZ_SMETER_STOPED:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    EZ_SMeterStoped(widget) = (i1 != 0);
	  break;
	case EZ_SMETER_RANGE:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
	      float *ff = EZ_SMeterRange(widget);
	      ff[0] = f1; ff[1] = f2;
	    }
	  break;	  
	case EZ_SMETER_BACKGROUND:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
	      char *color = (char *)p1;
	      unsigned long  pixel_value = EZ_AllocateColorFromName(color);
              if(pixel_value == 0) pixel_value = ~0;
	      EZ_SMeterBColor(widget) = pixel_value;
	    }
	  break;
	case EZ_SMETER_FOREGROUND:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
	      char *color = (char *)p1;
	      unsigned long  pixel_value = EZ_AllocateColorFromName(color);
	      EZ_SMeterFColor(widget)[0] = pixel_value;
	    }
	  break;
	  /* end of status meter config options. 2/6/98 */
	  
	case EZ_HSCROLL_INCREMENT:
	  if(i1 > 0) (void) EZ_SetHScrollbarDiscreteSpeed(widget, i1);
	  break;
	case EZ_VSCROLL_INCREMENT:
	  if(i1 > 0) (void) EZ_SetVScrollbarDiscreteSpeed(widget, i1);
	  break;
	case EZ_GEOMETRY_MANAGER:
	  {
	    switch(type)
	      {
	      case EZ_WIDGET_TREE:
	      case EZ_WIDGET_LIST_BOX:
	      case EZ_WIDGET_TEXT:
	      case EZ_WIDGET_WORK_AREA:
	      case EZ_WIDGET_FANCY_LISTBOX:
	      case EZ_WIDGET_TERM:
		EZ_ConfigureError(widget,option, "option not available");
		break;
	      default:
                {
                  EZ_GManager *ptr;
                  if(EZ_WidgetGManager(widget) == (EZ_GManager *)NULL)
                    EZ_WidgetGManager(widget) = EZ_AllocateGManager(i1);
                  ptr = EZ_WidgetGManager(widget);
                  EZ_GMType(ptr) = i1;
                }
		break;
	      }
	  }
	break;
	case EZ_LED_COLOR_FUNCTION :
	  if(type == EZ_WIDGET_LED)
	    {
	      EZ_LEDColorF(widget) = (EZ_LedPixelColorF)p1;
	    }
	  break;
	case EZ_LED_BACKGROUND:
	  if(type == EZ_WIDGET_LED)
	    {
	      char *color;
	      unsigned long  pixel_value;
	      color     =  (char *)p1;
	      pixel_value = EZ_AllocateColorFromName(color);
              if(pixel_value == 0) pixel_value = ~0;
	      if(EZ_LEDBG(widget) != pixel_value)
		{
		  EZ_LEDStatus(widget) |= LED_BLOCKED;
		  EZ_LEDBG(widget) = pixel_value;
		  EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
		}
	    }
	  break;
	case EZ_LED_PIXEL_COLOR:
	  if(type == EZ_WIDGET_LED)
	    {
	      char *color;
	      unsigned long  pixel_value;
	      color     =  (char *)p1;
	      pixel_value = EZ_AllocateColorFromName(color);
	      if(EZ_LEDBGL(widget) != pixel_value)
		{
		  EZ_LEDStatus(widget) |= LED_BLOCKED;
		  EZ_LEDBGL(widget) = pixel_value;
		  EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
		}
	    }
	  break;
	case EZ_LED_WIDTH:
	  if(type == EZ_WIDGET_LED)
	    {
	      if(i1 > 0 && i1 != EZ_LEDSCols(widget))
		{
		  EZ_LEDStatus(widget) |= LED_BLOCKED;
		  EZ_LEDSCols(widget) = i1;
		  EZ_LEDStatus(widget) |= LED_BLOCKED;
		  EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
		  size_changed = 1;	      
		}
	    }
	  break;
	case EZ_LED_HEIGHT:
	  if(type == EZ_WIDGET_LED)
	    {
	      if(i1 > 0 && i1 != EZ_LEDSRows(widget))
		{
		  EZ_LEDStatus(widget) |= LED_BLOCKED;
		  EZ_LEDSRows(widget) = i1;
		  EZ_LEDStatus(widget) |= LED_BLOCKED;
		  EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
		  size_changed = 1;	      
		}
	    }
	  break;
	case EZ_LED_PIXEL_SIZE:
	  if(type == EZ_WIDGET_LED)
	    {
	      if(i1 < 0) i1 = 1; else if(i1 > 62) i1 = 62; i1++;
	      if(i2 < 0) i2 = 1; else if(i2 > 62) i2 = 62; i2++;
	      if(EZ_LEDXSize(widget) != i1 || EZ_LEDYSize(widget) != i2)
                {
                  EZ_LEDXSize(widget) = i1;
                  EZ_LEDYSize(widget) = i2;
                  EZ_LEDStatus(widget) |= LED_BLOCKED;
		  EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
                }
              size_changed = 1;
	    }
	  break;
	case EZ_LCD_BACKGROUND:
	  if(type == EZ_WIDGET_LCD)
	    {
	      char *color;
	      unsigned long  pixel_value;
	      color     =  (char *)p1;
	      pixel_value = EZ_AllocateColorFromName(color);
              if(pixel_value == 0) pixel_value = ~0;
	      if(EZ_LCDBG(widget) != pixel_value)
		EZ_LCDBG(widget) = pixel_value;
	    }
	  break;
	case EZ_LCD_FOREGROUND:
	  if(type == EZ_WIDGET_LCD)
	    {
	      char *color;
	      unsigned long  pixel_value;
	      color     =  (char *)p1;
	      pixel_value = EZ_AllocateNewBackground(color);
	      if(pixel_value != 0) 
		{
		  if(EZ_LCDFG(widget) != pixel_value)
		    {
		      EZ_LCDFG(widget) = pixel_value;
		      EZ_LCDFGBright(widget) = EZ_GetBrightColor(pixel_value);
		      EZ_LCDFGDark(widget) = EZ_GetDarkColor(pixel_value);
		    }
		}
	      else
		{
		  pixel_value = EZ_AllocateColorFromName(color);
		  EZ_LCDFGBright(widget) = ~0;
		  EZ_LCDFGDark(widget) = ~0;
		  EZ_LCDFG(widget) = pixel_value; 
		}
	    }
	  break;	  
	case EZ_LCD_NDIGITS:
	  if(type == EZ_WIDGET_LCD)
	    {
	      if(i1 > 0 && i1 != EZ_LCDNDigits(widget))
		{
		  EZ_LCDNDigits(widget) = i1;
		  size_changed = 1;
		}
	    }
	  break;
	case EZ_LCD_FONT_SIZE:
	  if(type == EZ_WIDGET_LCD)
	    {
	      if(i1 > 0 && i1 != EZ_LCDSize(widget))
		{
		  EZ_LCDSize(widget) = i1;
		  size_changed = 1;
		}
	    }
	  break;
	case EZ_PADB:
	  if(i1 >= 0)
	    {
	      if( type != EZ_WIDGET_3D_CANVAS && i1 != EZ_WidgetPadB(widget))
		{
		  EZ_WidgetPadB(widget) = i1;
		  size_changed = 1;
		}
	    }
	  else EZ_ConfigureError(widget,option, "Value out of range"); 
	  break;
	case EZ_GRID_CELL_GEOMETRY:
	  {
	    int err = 0;
	    EZ_Widget *parent = EZ_WidgetParent(widget);
	    if(parent && EZ_WidgetGManager(parent) == (EZ_GManager *)NULL)
	      EZ_WidgetGManager(parent) = EZ_AllocateGManager(EZ_GB_GEOM_MANAGER);
	    if(i1 < 0) {err++; i1 = 0;}
	    if(i2 < 0) {err++; i2 = 0;}
	    if(i3 <=0) {err++; i3 = 1;}
	    if(i4 <=0) {err++; i4 = 1;}
	    EZ_WidgetGBX(widget) = i1;
	    EZ_WidgetGBY(widget) = i2;
	    EZ_WidgetGBW(widget) = i3;
	    EZ_WidgetGBH(widget) = i4;
	    size_changed = (err != 4);
	    if(err) EZ_ConfigureError(widget,option, "Value out of range"); 
	  }
	break;
	case EZ_GRID_CELL_PLACEMENT:
	  {
	    int err = 0;
	    EZ_Widget *parent = EZ_WidgetParent(widget);
	    if(parent && EZ_WidgetGManager(parent) == (EZ_GManager *)NULL)
	      EZ_WidgetGManager(parent) = EZ_AllocateGManager(EZ_GB_GEOM_MANAGER);

	    if(i1 >= EZ_WIDGET_FILL_BEGIN && i1 <= EZ_WIDGET_FILL_END)
	      EZ_WidgetGBFill(widget) = i1;
	    else err++;
	    if(i2 >= EZ_WIDGET_LABEL_POSITION_BEGIN &&
	       i2 <= EZ_WIDGET_LABEL_POSITION_END -1)
	      EZ_WidgetGBAnchor(widget) = i2;	  
	    else err++;
	    size_changed = (err != 2);
	    if(err)  EZ_ConfigureError(widget,option, "Value out of range"); 
	  }
	break;
	case EZ_GRID_CONSTRAINS:
	  size_changed = EZ_SetRowColConstrains(widget,(int)ul1, i1,i2,i3,i4);
	  break;
	case EZ_GRID_ROW_CONSTRAINS:
	  size_changed = EZ_SetRowColConstrains(widget,EZ_ROW, i1,i2,i3,i4);
	  break;
	case EZ_GRID_COLUMN_CONSTRAINS:
	  size_changed = EZ_SetRowColConstrains(widget,EZ_COLUMN, i1,i2,i3,i4);
	  break;
	case EZ_MENU_TEAR_OFF:
	  if(type == EZ_WIDGET_POPUP_MENU)
	    size_changed = EZ_ConfigureTearOffMenu(widget, i1);
	  break;
	case EZ_UNDERLINE:
	  value = i1;
	  switch(type)
	    {
	    case EZ_WIDGET_NW_LABEL:
	      {
		char *label = EZ_LabelString(widget);
		if(label && value >= 0 && value < EZ_LabelLineLength(widget) && isprint(label[value]))
		  {
		    EZ_LabelUnderline(widget) = value;
		    EZ_LabelModifiers(widget) = Mod1Mask;
		    EZ_LabelShortcut(widget)  = (EZ_LabelString(widget) + value);
		    EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (label+ value));
		  }
		else
		  {
		    EZ_LabelUnderline(widget) = -1;
		    EZ_LabelModifiers(widget) = 0;
		    EZ_LabelShortcut(widget)  = (char *)NULL;
		  }
	      }
	    break;
	    case EZ_WIDGET_NORMAL_BUTTON:
	    case EZ_WIDGET_ARROW_BUTTON:
	    case EZ_WIDGET_MENU_NORMAL_BUTTON:
	      {
		char *label = EZ_ButtonLabel(widget);
		if(label && value >= 0 && value < EZ_ButtonLabelLength(widget) && isprint(label[value]))
		  {
		    EZ_ButtonUnderLine(widget) = value;
		    EZ_ButtonModifiers(widget) = Mod1Mask;
		    EZ_ButtonShortcut(widget)  = (EZ_ButtonLabel(widget) + value);
		    EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (label+ value));
		  }
		else
		  {
		    EZ_ButtonUnderLine(widget) = -1;
		    EZ_ButtonModifiers(widget) = 0;
		    EZ_ButtonShortcut(widget)  = (char *)NULL;
		  }
	      }
	    break;
	    case EZ_WIDGET_CHECK_BUTTON:
	    case EZ_WIDGET_MENU_CHECK_BUTTON:
	      {
		char *label = EZ_CButtonLabel(widget);
		if(label && value >= 0 && value < EZ_CButtonLabelLength(widget) && isprint(label[value]))
		  {
		    EZ_CButtonUnderLine(widget) = value;
		    EZ_CButtonModifiers(widget) = Mod1Mask;
		    EZ_CButtonShortcut(widget)  = (EZ_CButtonLabel(widget) + value);
		    EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (label+ value));
		  }
		else
		  {
		    EZ_CButtonUnderLine(widget) = -1;
		    EZ_CButtonModifiers(widget) = 0;
		    EZ_CButtonShortcut(widget)  = (char *)NULL;
		  }
	      }
	    break;
	    case EZ_WIDGET_RADIO_BUTTON:
	    case EZ_WIDGET_MENU_RADIO_BUTTON:
	      {
		char *label = EZ_RButtonLabel(widget);
		if(label && value >= 0 && value < EZ_RButtonLabelLength(widget) && isprint(label[value]))
		  {
		    EZ_RButtonUnderLine(widget) = value;
		    EZ_RButtonModifiers(widget) = Mod1Mask;
		    EZ_RButtonShortcut(widget)  = (EZ_RButtonLabel(widget) + value);
		    EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (label+ value));
		  }
		else
		  {
		    EZ_RButtonUnderLine(widget) = -1;
		    EZ_RButtonModifiers(widget) = 0;
		    EZ_RButtonShortcut(widget)  = (char *)NULL;
		  }
	      }
	    break;
	    case EZ_WIDGET_MENU_BUTTON:
	      {
		char *label = EZ_MButtonLabel(widget);
		if(label && value >= 0 && value < EZ_MButtonLabelLength(widget) && isprint(label[value]))
		  {
		    EZ_MButtonUnderLine(widget) = value;
		    EZ_MButtonModifiers(widget) = Mod1Mask;
		    EZ_MButtonShortcut(widget)  = (EZ_MButtonLabel(widget) + value);
		    EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (label+ value));
		  }
		else
		  {
		    EZ_MButtonUnderLine(widget) = -1;
		    EZ_MButtonModifiers(widget) = 0;
		    EZ_MButtonShortcut(widget)  = (char *)NULL;
		  }
	      }
	    break;
	    case EZ_WIDGET_MENU_SUBMENU:
	      {
		char *label = EZ_SubMenuLabel(widget);
		if(label && value >= 0 && value < EZ_SubMenuLabelLength(widget) && isprint(label[value]))
		  {
		    EZ_SubMenuUnderLine(widget) = value;
		    EZ_SubMenuModifiers(widget) = Mod1Mask;
		    EZ_SubMenuShortcut(widget)  = (EZ_SubMenuLabel(widget) + value);
		    EZ_InsertGlobalKeyPressEvent(widget, Mod1Mask, (label+value));
		  }
		else
		  {
		    EZ_SubMenuUnderLine(widget) = -1;
		    EZ_SubMenuModifiers(widget) = 0;
		    EZ_SubMenuShortcut(widget)  = (char *)NULL;
		  }
	      }
	    break;
	    case EZ_WIDGET_NB_PAGE:
	      {
		EZ_Widget *btn = EZ_NBPageCompanion(widget);
		if(btn)
		  {
		    char *label = EZ_ButtonLabel(btn);
		    if(label && value >= 0 && value < EZ_ButtonLabelLength(btn) && isprint(label[value]))
		      {
			EZ_ButtonUnderLine(btn) = value;
			EZ_ButtonModifiers(btn) = Mod1Mask;
			EZ_ButtonShortcut(btn)  = (EZ_ButtonLabel(btn) + value);
			EZ_InsertGlobalKeyPressEvent(btn, Mod1Mask, (label+ value));
		      }
		    else
		      {
			EZ_ButtonUnderLine(btn) = -1;
			EZ_ButtonModifiers(btn) = 0;
			EZ_ButtonShortcut(btn)  = (char *)NULL;
		      }
		  }
	      }
	      break;
	    default:
	      break;
	    }
	  break;
	case EZ_FANCY_LIST_BOX_COLUMNS:
	  {
	    EZ_Widget *iflbox = NULL;
	    if(type == EZ_WIDGET_FANCY_LIST_BOX)
	      iflbox = EZ_FListBoxIFListBox(widget);
	    else if(type == EZ_WIDGET_IFANCY_LIST_BOX)
	      iflbox = widget;
	    if(iflbox)
	      {
		EZ_IFListBoxAllocNewStorage(iflbox, 64, i1);  
	      }
	  }
	  break;
	case EZ_RETURN_VALUE:
	  EZ_WidgetRetData(widget) = i1;
	  break;
	case EZ_FANCY_LIST_BOX_TYPE:
	  {
	    EZ_Widget *iflbox = NULL;
	    if(type == EZ_WIDGET_FANCY_LIST_BOX)
	      iflbox = EZ_FListBoxIFListBox(widget);
	    else if(type == EZ_WIDGET_IFANCY_LIST_BOX)
	      iflbox = widget;
	    if(iflbox)
	      {
		EZ_IFListBoxSelectionType(iflbox) = (i1 != 0);
		EZ_IFListBoxRowMode(iflbox) = (i1 > 1);
	      }
	  }
	  break;
	case EZ_TEXT_WIDGET_EDITABLE:
	  {
	    EZ_Widget *iText = NULL;
	    if(type == EZ_WIDGET_TEXT)
	      iText = EZ_TextIText(widget);
	    else if(type == EZ_WIDGET_ITEXT)
	      iText = widget;
	    if(iText)
	      {
		if(i1) { EZ_ITextEditable(iText) = 1; EZ_SetWidgetFocusableFlag(iText);}
		else { EZ_ITextEditable(iText) = 0; EZ_ClearWidgetFocusableFlag(iText);}
	      }
	  }
	break;
        case EZ_WRAP_TEXT:
          {
	    EZ_Widget *iText = NULL;
	    if(type == EZ_WIDGET_TEXT)
	      iText = EZ_TextIText(widget);
	    else if(type == EZ_WIDGET_ITEXT)
	      iText = widget;
	    if(iText)
              {
                EZ_ITextWrapText(iText) = i1;
                EZ_ITextDirty(iText) = 1;
              }
          }
        break;
	case EZ_OPTIONAL_ENTRY_EDITABLE:
	  {
	    EZ_Widget *entry = NULL;
	    if(type == EZ_WIDGET_OPTIONAL_ENTRY)
	      {	entry = EZ_OptEntryEntry(widget);}
	    else if(type == EZ_WIDGET_ENTRY)
	      entry = widget;
	    if(entry)
	      {
		if(i1) { EZ_EntryEditable(entry) = 1; EZ_SetWidgetFocusableFlag(entry);}
		else   { EZ_EntryEditable(entry) = 0; EZ_ClearWidgetFocusableFlag(entry);}
	      }
	  }
	break;
	case EZ_OPTIONAL_ENTRY_REMEMBER_HISTORY:
	  if(type == EZ_WIDGET_OPTIONAL_ENTRY)
	    {
	      EZ_Widget *entry =  EZ_OptEntryEntry(widget);
	      if(entry)EZ_EntryRemberHistory(entry) = (i1 != 0);
	    }
	  break;
	case EZ_SLIDER_STYLE:
	  if((type == EZ_WIDGET_HORIZONTAL_SLIDER ||
	     type == EZ_WIDGET_VERTICAL_SLIDER))
	    { 
	      if(EZ_SliderStyle(widget) != i1)
		{
		  EZ_SliderStyle(widget) = i1;
		  size_changed = 1;
		}
	    }
          else if(type == EZ_WIDGET_HORIZONTAL_SCROLLBAR ||
                  type ==  EZ_WIDGET_VERTICAL_SCROLLBAR)
            {
	      if(EZ_ScrollStyle(widget) != i1)
		{
		  EZ_ScrollStyle(widget) = i1;
		  size_changed = 1;
		}
            }
          else if(type == EZ_WIDGET_GRADIENT_BAR)
            {
              EZ_GBarStyle(widget) = i1;
              size_changed = 1;
            }
	  break;
	case EZ_SLIDER_INIT_VALUE:
	  if(type == EZ_WIDGET_HORIZONTAL_SLIDER ||
	     type == EZ_WIDGET_VERTICAL_SLIDER)
	    { EZ_SliderValue(widget) = f1; size_changed = 1;}
          else if(type == EZ_WIDGET_GRADIENT_BAR)
            {
              EZ_GBarValue(widget) = f1;
              size_changed = 1;
            }
          else if(type == EZ_WIDGET_THUMB_WHEEL)
            {
              EZ_ThumValue(widget) = f1;
            }          
	  else EZ_ConfigureError(widget,option, "option not applicable"); 
	  break;
	case EZ_ENTRY_STRING:
	  {
	    if(type == EZ_WIDGET_ENTRY)
	      EZ_SetEntryString(widget, (char *)p1);
	    else if(type == EZ_WIDGET_OPTIONAL_ENTRY)
	      EZ_SetOptionalEntryString(widget, (char *)p1);
	  }
	break;
        case EZ_GLOB_PATTERN:
	  switch(type)
	    {
	    case EZ_WIDGET_OPTIONAL_ENTRY:
	      {
		EZ_Widget *twidget =  EZ_OptEntryEntry(widget);
		if(twidget)
		  {
		    EZ_SetEntryString(twidget, (char *)p1);
		    size_changed = 1;
		  }
	      }
	      break;
	    case EZ_WIDGET_FILE_SELECTOR:
	      {
		EZ_SetFileSelectorInitialPattern(widget, (char *)p1);
		size_changed = 1;
	      }
	    break;
	    default:
	      break;
	    }
	  break;
	case EZ_CHECK_BUTTON_ON_VALUE:
	  if(type == EZ_WIDGET_CHECK_BUTTON ||
	     type ==  EZ_WIDGET_MENU_CHECK_BUTTON)
	    {
	      if(EZ_CButtonVarValue(widget) == EZ_CButtonVarOnValue(widget))
		EZ_CButtonVarValue(widget) = i1;
	      EZ_CButtonVarOnValue(widget) = i1;
	      size_changed = 1;
	    }
	  break;

	case EZ_CHECK_BUTTON_OFF_VALUE:
	  if(type == EZ_WIDGET_CHECK_BUTTON ||
	     type ==  EZ_WIDGET_MENU_CHECK_BUTTON)
	    {
	      if(EZ_CButtonVarValue(widget) == EZ_CButtonVarOffValue(widget))
		EZ_CButtonVarValue(widget) = i1;
	      EZ_CButtonVarOffValue(widget) = i1;
	      size_changed = 1;
	    }
	  break;
	case EZ_CHECK_BUTTON_ON_OFF:
	  if(type == EZ_WIDGET_CHECK_BUTTON ||
	     type ==  EZ_WIDGET_MENU_CHECK_BUTTON)
	    {
	      if(i1) EZ_CButtonVarValue(widget) = EZ_CButtonVarOnValue(widget);
	      else EZ_CButtonVarValue(widget) = EZ_CButtonVarOffValue(widget);
	      size_changed = 1;
	    }
	  break;
	case EZ_RADIO_BUTTON_GROUP:
	  if(type == EZ_WIDGET_RADIO_BUTTON ||
	     type == EZ_WIDGET_MENU_RADIO_BUTTON)
	    {
	      EZ_FixRadioButtonList(widget,i1);
	    }
	  break;
	case EZ_RADIO_BUTTON_VALUE:
	  if(type == EZ_WIDGET_RADIO_BUTTON ||
	     type == EZ_WIDGET_MENU_RADIO_BUTTON)
	    {
	      int gid;
	      if(EZ_RButtonGroup(widget) == NULL) gid = 0;
	      else gid = (EZ_RButtonGroup(widget))->id;
	      EZ_RButtonVarValue(widget) = i1;
	      EZ_FixRadioButtonList(widget,gid);
	    }
	  break;
	case EZ_OPTIONAL_HSCROLLBAR:
	  if(i1) 
	    {
	      switch(type)
		{
		case EZ_WIDGET_TEXT:
		  {
		    EZ_Widget *hscroll = EZ_TextHScroll(widget);
		    if(hscroll == NULL)
		      {
			EZ_Widget *itext =  EZ_TextIText(widget);
			hscroll = EZ_TextHScroll(widget) = 
			  EZ_CreateScrollbar(widget,EZ_WIDGET_HORIZONTAL_SCROLLBAR);
			EZ_SetWidgetExpandFlag(hscroll);
			EZ_ScrollObject(hscroll) = itext;
			EZ_ITextHScroll(itext) = hscroll;
			EZ_GetAndUseWidgetResources(hscroll);
		      }
		    else EZ_ActivateWidget(hscroll);
		  }
		break;
		case EZ_WIDGET_LIST_BOX:
		  {
		    EZ_Widget *hscroll = EZ_ListBoxHScroll(widget);
		    if(hscroll == NULL)
		      {
			EZ_Widget *ilistbox = EZ_ListBoxIListBox(widget);
			hscroll = EZ_ListBoxHScroll(widget) = 
			  EZ_CreateScrollbar(widget,EZ_WIDGET_HORIZONTAL_SCROLLBAR);
			EZ_SetWidgetExpandFlag(hscroll);
			EZ_ScrollObject(hscroll) = ilistbox;
			EZ_IListBoxHScroll(ilistbox) = hscroll;
			EZ_GetAndUseWidgetResources(hscroll);
		      }
		    else EZ_ActivateWidget(hscroll);
		  }
		break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  {
		    EZ_Widget *hscroll = EZ_FListBoxHScroll(widget);
		    if(hscroll == NULL)
		      {
			EZ_Widget *iflistbox = EZ_FListBoxIFListBox(widget);
			hscroll = EZ_FListBoxHScroll(widget) = 
			  EZ_CreateScrollbar(widget,EZ_WIDGET_HORIZONTAL_SCROLLBAR);
			EZ_SetWidgetExpandFlag(hscroll);
			EZ_ScrollObject(hscroll) = iflistbox;
			EZ_IFListBoxHScroll(iflistbox) = hscroll; 
			EZ_GetAndUseWidgetResources(hscroll);
		      }
		    else EZ_ActivateWidget(hscroll);
		  }
		break;
		case EZ_WIDGET_TREE:
		  {
		    EZ_Widget *hscroll = EZ_TreeWHScroll(widget);
		    if(hscroll == NULL)
		      {
			EZ_Widget *itree = EZ_TreeWITree(widget);
			hscroll = EZ_TreeWHScroll(widget) =
			  EZ_CreateScrollbar(widget,EZ_WIDGET_HORIZONTAL_SCROLLBAR);
			EZ_SetWidgetExpandFlag(hscroll);
			EZ_ScrollObject(hscroll) = itree;
			EZ_ITreeWVScroll(itree) = hscroll;  
			EZ_GetAndUseWidgetResources(hscroll);
		      }
		    else EZ_ActivateWidget(hscroll);
		  }
		break;
		case EZ_WIDGET_WORK_AREA:
		  {
		    EZ_Widget *hscroll = EZ_WorkAreaHScroll(widget);
		    if(hscroll == NULL)
		      {
			EZ_Widget *iwa = EZ_WorkAreaIWorkArea(widget);
			hscroll = EZ_WorkAreaHScroll(widget) =
			  EZ_CreateScrollbar(widget,EZ_WIDGET_HORIZONTAL_SCROLLBAR);
			EZ_SetWidgetExpandFlag(hscroll);
			EZ_ScrollObject(hscroll) = iwa;
			EZ_IWorkAreaHScroll(iwa) = hscroll;
			EZ_GetAndUseWidgetResources(hscroll);
		      }
		    else EZ_ActivateWidget(hscroll);
		  }
		break;
		default:
		  break;
		}
	    }
	  else
	    {
	      switch(type)
		{
		case EZ_WIDGET_TEXT:
		  {
		    EZ_Widget *hscroll = EZ_TextHScroll(widget);
		    if(hscroll != NULL) EZ_DeActivateWidget(hscroll);
		  }
		break;
		case EZ_WIDGET_LIST_BOX:
		  {
		    EZ_Widget *hscroll = EZ_ListBoxHScroll(widget);
		    if(hscroll != NULL) EZ_DeActivateWidget(hscroll);
		  }
		break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  {
		    EZ_Widget *hscroll = EZ_FListBoxHScroll(widget);
		    if(hscroll != NULL) EZ_DeActivateWidget(hscroll);
		  }
		break;
		case EZ_WIDGET_TREE:
		  {
		    EZ_Widget *hscroll = EZ_TreeWHScroll(widget);
		    if(hscroll != NULL) EZ_DeActivateWidget(hscroll);
		  }
		break;
		case EZ_WIDGET_WORK_AREA:
		  {
		    EZ_Widget *hscroll = EZ_WorkAreaHScroll(widget);
		    if(hscroll != NULL) EZ_DeActivateWidget(hscroll);
		  }
		break;
		default:
		  break;
		}
	    }
	  break;
	case EZ_OPTIONAL_VSCROLLBAR:
	  if(i1)
	    {
	      switch(type)
		{
                case EZ_WIDGET_TERM:
		  {
		    EZ_Widget *vscroll = EZ_TermVScroll(widget);
		    if(vscroll == NULL)
		      {
			EZ_Widget *iterm =  EZ_TermITerm(widget);
			vscroll = EZ_TermVScroll(widget) = 
			  EZ_CreateScrollbar(widget,EZ_WIDGET_VERTICAL_SCROLLBAR);
			EZ_SetWidgetExpandFlag(vscroll);
			EZ_ScrollObject(vscroll) = iterm;
			EZ_ITermVScroll(iterm) = vscroll;
			EZ_GetAndUseWidgetResources(vscroll);
		      }
		    else EZ_ActivateWidget(vscroll);
                  }
                break;
		case EZ_WIDGET_TEXT:
		  {
		    EZ_Widget *vscroll = EZ_TextVScroll(widget);
		    if(vscroll == NULL)
		      {
			EZ_Widget *itext =  EZ_TextIText(widget);
			vscroll = EZ_TextVScroll(widget) = 
			  EZ_CreateScrollbar(widget,EZ_WIDGET_VERTICAL_SCROLLBAR);
			EZ_SetWidgetExpandFlag(vscroll);
			EZ_ScrollObject(vscroll) = itext;
			EZ_ITextVScroll(itext) = vscroll;
			EZ_GetAndUseWidgetResources(vscroll);
		      }
		    else EZ_ActivateWidget(vscroll);
		  }
                break;
		case EZ_WIDGET_LIST_BOX:
		  {
		    EZ_Widget *vscroll = EZ_ListBoxVScroll(widget);
		    if(vscroll == NULL)
		      {
			EZ_Widget *ilistbox = EZ_ListBoxIListBox(widget);
			vscroll = EZ_ListBoxVScroll(widget) = 
			  EZ_CreateScrollbar(widget,EZ_WIDGET_VERTICAL_SCROLLBAR);
			EZ_SetWidgetExpandFlag(vscroll);
			EZ_ScrollObject(vscroll) = ilistbox;
			EZ_IListBoxVScroll(ilistbox) = vscroll;
			EZ_GetAndUseWidgetResources(vscroll);
		      }
		    else EZ_ActivateWidget(vscroll);
		  }
		break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  {
		    EZ_Widget *vscroll = EZ_FListBoxVScroll(widget);
		    if(vscroll == NULL)
		      {
			EZ_Widget *iflistbox = EZ_FListBoxIFListBox(widget);
			vscroll = EZ_FListBoxVScroll(widget) = 
			  EZ_CreateScrollbar(widget,EZ_WIDGET_VERTICAL_SCROLLBAR);
			EZ_SetWidgetExpandFlag(vscroll);
			EZ_ScrollObject(vscroll) = iflistbox;
			EZ_IFListBoxVScroll(iflistbox) = vscroll; 
			EZ_GetAndUseWidgetResources(vscroll);
		      }
		    else EZ_ActivateWidget(vscroll);
		  }
		break;
		case EZ_WIDGET_TREE:
		  {
		    EZ_Widget *vscroll = EZ_TreeWVScroll(widget);
		    if(vscroll == NULL)
		      {
			EZ_Widget *itree = EZ_TreeWITree(widget);
			vscroll = EZ_TreeWVScroll(widget) =
			  EZ_CreateScrollbar(widget,EZ_WIDGET_VERTICAL_SCROLLBAR);
			EZ_SetWidgetExpandFlag(vscroll);
			EZ_ScrollObject(vscroll) = itree;
			EZ_ITreeWVScroll(itree) = vscroll;  
			EZ_GetAndUseWidgetResources(vscroll);
		      }
		    else EZ_ActivateWidget(vscroll);
		  }
		break;
		case EZ_WIDGET_WORK_AREA:
		  {
		    EZ_Widget *vscroll = EZ_WorkAreaVScroll(widget);
		    if(vscroll == NULL)
		      {
			EZ_Widget *iwa = EZ_WorkAreaIWorkArea(widget);
			vscroll = EZ_WorkAreaVScroll(widget) =
			  EZ_CreateScrollbar(widget,EZ_WIDGET_VERTICAL_SCROLLBAR);
			EZ_SetWidgetExpandFlag(vscroll);
			EZ_ScrollObject(vscroll) = iwa;
			EZ_IWorkAreaVScroll(iwa) = vscroll;
			EZ_GetAndUseWidgetResources(vscroll);
		      }
		    else EZ_ActivateWidget(vscroll);
		  }
		break;
		default:
		  break;
		}
	    }
	  else
	    {
	      switch(type)
		{
                case EZ_WIDGET_TERM:
                  {
		    EZ_Widget *vscroll = EZ_TermVScroll(widget);
		    if(vscroll != NULL) EZ_DeActivateWidget(vscroll);
                  }
                break;
		case EZ_WIDGET_TEXT:
		  {
		    EZ_Widget *vscroll = EZ_TextVScroll(widget);
		    if(vscroll != NULL) EZ_DeActivateWidget(vscroll);
		  }
                break;
		case EZ_WIDGET_LIST_BOX:
		  {
		    EZ_Widget *vscroll = EZ_ListBoxVScroll(widget);
		    if(vscroll != NULL) EZ_DeActivateWidget(vscroll);
		  }
		break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  {
		    EZ_Widget *vscroll = EZ_FListBoxVScroll(widget);
		    if(vscroll != NULL) EZ_DeActivateWidget(vscroll);
		  }
		break;
		case EZ_WIDGET_TREE:
		  {
		    EZ_Widget *vscroll = EZ_TreeWVScroll(widget);
		    if(vscroll != NULL) EZ_DeActivateWidget(vscroll);
		  }
		break;
		case EZ_WIDGET_WORK_AREA:
		  {
		    EZ_Widget *vscroll = EZ_WorkAreaVScroll(widget);
		    if(vscroll != NULL) EZ_DeActivateWidget(vscroll);
		  }
		break;
		default:
		  break;
		}
	    }
	  break;
	case EZ_SHAPE_FILE:
          if(EZ_XServerHasSHAPEExt)          
            {
              char *file = (char *)p1;
              EZ_Bitmap *old, *bitmap = (file && file[0] ? EZ_GetBitmapFromFile(file, 1):
                                         (EZ_Bitmap *)NULL);
              old=EZ_WidgetShapeMask(widget);
              if(old) EZ_FreeBitmap(old);
              if(bitmap && bitmap->shape)
                {
                  EZ_WidgetShapeMask(widget) = bitmap;
                  EZ_SetWidgetShapedWinFlag(widget);
                  if(EZ_WidgetWindow(widget) != None) shape_changed = 1<<21; /* set shape */
                }
              else
                {
                  if(bitmap)EZ_FreeBitmap(bitmap);
                  EZ_WidgetShapeMask(widget) = NULL;
                  EZ_ClearWidgetShapedWinFlag(widget);
                  if(EZ_WidgetWindow(widget) != None) shape_changed = 1<<20; /* remove shape */
                }
            }
          break;
        case EZ_SHAPE_PIXMAP:
          if(EZ_XServerHasSHAPEExt)          
            {
              EZ_Bitmap *old, *bitmap = (EZ_Bitmap *)p1;
              old=EZ_WidgetShapeMask(widget);
              if(old) EZ_FreeBitmap(old);
              if(bitmap && bitmap->shape)
                {
                  bitmap->ref += 1;
                  EZ_WidgetShapeMask(widget) = bitmap;
                  if(EZ_WidgetWindow(widget) != None) shape_changed = 1<<21; /* set shape */
                  EZ_SetWidgetShapedWinFlag(widget);
                }
              else
                {
                  EZ_WidgetShapeMask(widget) = NULL;
                  EZ_ClearWidgetShapedWinFlag(widget);
                  if(EZ_WidgetWindow(widget) != None) shape_changed = 1<<20; /* remove shape */
                }
            }
          break;
	case EZ_SHAPED_WINDOW:
	  {
	    int ii = i1;
	    if(EZ_XServerHasSHAPEExt)
	      {
		if(ii)
		  {
		    EZ_SetWidgetShapedWinFlag(widget);
		    if(ii > 1) EZ_SetWidgetShapeShiftFlag(widget);
		    if(EZ_WidgetWindow(widget) != None) shape_changed = (1<<21);
		  }
		else
		  {
		    EZ_ClearWidgetShapedWinFlag(widget);
		    EZ_ClearWidgetShapeShiftFlag(widget);
		    if(EZ_WidgetWindow(widget) != None) shape_changed = (1<<20);
		  }
                if(type == EZ_WIDGET_ITOPLEVEL) { EZ_IconShapeSet(widget) = 1;}
		size_changed = 1;
	      }
	  }
	break;
	case EZ_LABELED_ICON:
	  {
	    if(type == EZ_WIDGET_ITOPLEVEL)
	      {
		if(i1)
		  {
		    EZ_WidgetBorderStyle(widget) = EZ_BORDER_UP;
		    EZ_LabelIsLabeledIcon(widget) = (char)i1;
		    if(EZ_XServerHasSHAPEExt)
                      {
                        if(EZ_IconShapeSet(widget) == 0) 
                          EZ_SetWidgetShapedWinFlag(widget);
                        EZ_LabelMaskDirty(widget) = 1; 
                      }
		  }
		else
		  {
		    EZ_LabelIsLabeledIcon(widget) = 0; 
		    if(EZ_WidgetWindow(widget) != None) shape_changed = (1<<20) | (1<<21);
		  }
		size_changed = 1;
	      }
	  }
	break;
	case  EZ_CURSOR:
        case EZ_CURSOR_NAME:
	  {
            Cursor cursor;
            if(option == EZ_CURSOR) cursor= (Cursor) ul1;
            else cursor = EZ_GetCursorByName((char *)p1);
	    if(cursor != None)
	      {
		EZ_Widget *twidget = widget;
		switch(type)
		  {
		  case EZ_WIDGET_TREE:
		    twidget = EZ_TreeWITree(widget);		  
		    break;
		  case EZ_WIDGET_LIST_BOX:
		    twidget = EZ_ListBoxIListBox(widget);
		    break;
		  case EZ_WIDGET_FANCY_LIST_BOX:
		    twidget = EZ_FListBoxIFListBox(widget);
		    break;
		  case EZ_WIDGET_WORK_AREA:
		    twidget = EZ_WorkAreaIWorkArea(widget);
		    break;
		  case EZ_WIDGET_TEXT:
		    twidget = EZ_TextIText(widget);
		    break;
                  case EZ_WIDGET_TERM:
		    twidget = EZ_TermITerm(widget);
		    break;
		  default:
		    break;
		  }
		EZ_WidgetCursor(twidget) = cursor;
		if(EZ_WidgetWindow(twidget) != (Cursor)NULL)
		  XDefineCursor(EZ_Display, EZ_WidgetWindow(twidget), cursor);
	      }
	  }
	break;
	case  EZ_DND_DRAG_CURSOR:
	case  EZ_DND_DRAG_CURSOR_NAME:
	  {
            Cursor cursor;
            if(option == EZ_DND_DRAG_CURSOR) cursor= (Cursor) ul1;
            else cursor = EZ_GetCursorByName((char *)p1);

            if(cursor)
              {
                EZ_Widget *twidget = widget;
                switch(type)
                  {
                  case EZ_WIDGET_TREE:
                    twidget = EZ_TreeWITree(widget);		  
                    break;
                  case EZ_WIDGET_LIST_BOX:
                    twidget = EZ_ListBoxIListBox(widget);
                    break;
                  case EZ_WIDGET_FANCY_LIST_BOX:
                    twidget = EZ_FListBoxIFListBox(widget);
                    break;
                  case EZ_WIDGET_WORK_AREA:
                    twidget = EZ_WorkAreaIWorkArea(widget);
                    break;
                  case EZ_WIDGET_TEXT:
                    twidget = EZ_TextIText(widget);
                    break;
                  case EZ_WIDGET_TERM:
                    twidget = EZ_TermITerm(widget);
                    break;
                  default:
                    break;
                  }
                EZ_WidgetDnDDragCursor(twidget) = cursor;
              }
	  }
	break;
	case  EZ_DND_BUBBLE_HELP:
	  {
	    char *ostr, *str = (char *)p1;
	    int olen, len = str? strlen(str): 0;

	    switch(type)
	      {
	      case EZ_WIDGET_TREE:
		widget = EZ_TreeWITree(widget);		  
		break;
	      case EZ_WIDGET_LIST_BOX:
		widget = EZ_ListBoxIListBox(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		widget = EZ_FListBoxIFListBox(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		widget = EZ_WorkAreaIWorkArea(widget);
		break;
	      case EZ_WIDGET_TEXT:
		widget = EZ_TextIText(widget);
		break;
              case EZ_WIDGET_TERM:
                widget = EZ_TermITerm(widget);
                break;
	      default:
		break;
	      }
	    ostr = EZ_WidgetDnDTargetHelp(widget);
	    if(len == 0) /* disable bubble help */
	      {
		if(ostr) (void)my_free(ostr);
		EZ_WidgetDnDTargetHelp(widget) = (char *)NULL;
	      }
	    else
	      {
		if(ostr)
		  {
		    olen = strlen(ostr);
		    if(olen < len)
		      ostr = EZ_WidgetDnDTargetHelp(widget) = (char *)my_realloc(ostr, len, _BUBBLE_STRING_);
		    if(!ostr) EZ_OutOfMemory("EZ_ConfigureWidget:DnDBubbleString");
		    (void)strcpy(ostr, str);
		  }
		else
		  EZ_WidgetDnDTargetHelp(widget) = EZ_AllocCopyString(str);
	      }
	  }
	  break;
	case EZ_ATTACH_PTR_DATA:
	  {
	    void *ptr =  p1;
	    EZ_Widget *twidget = NULL;
	    switch(type)
	      {
	      case EZ_WIDGET_TREE:
		twidget = EZ_TreeWITree(widget);		  
		break;
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		twidget = EZ_FListBoxIFListBox(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		twidget = EZ_WorkAreaIWorkArea(widget);
		break;
	      case EZ_WIDGET_TEXT:
		twidget = EZ_TextIText(widget);
		break;
              case EZ_WIDGET_TERM:
                twidget = EZ_TermITerm(widget);
                break;
	      default:
		break;
	      }
	    EZ_WidgetPtrData(widget) = ptr;
	    if(twidget) EZ_WidgetPtrData(twidget) = ptr;
	  }
	  break;
	case EZ_ATTACH_INT_DATA:
	  {
	    int ii =  i1;
	    int type = EZ_WidgetType(widget);
	    EZ_Widget *twidget = NULL;
	    switch(type)
	      {
	      case EZ_WIDGET_TREE:
		twidget = EZ_TreeWITree(widget);		  
		break;
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		twidget = EZ_FListBoxIFListBox(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		twidget = EZ_WorkAreaIWorkArea(widget);
		break;
	      case EZ_WIDGET_TEXT:
		twidget = EZ_TextIText(widget);
		break;
              case EZ_WIDGET_TERM:
                twidget = EZ_TermITerm(widget);
                break;
	      default:
		break;
	      }
	    EZ_WidgetIntData(widget) = ii;
	    if(twidget) EZ_WidgetIntData(twidget) = ii;
	  }
	  break;
	case EZ_SELECTION_BACKGROUND:
	  {
	    char *color;
	    EZ_Widget *twidget;
	    unsigned long  pixel_value;

	    color     =  (char *)p1;
	    pixel_value = EZ_AllocateColorFromName(color);
            if(pixel_value == 0) pixel_value = ~0;
	    switch(type)
	      {
	      case EZ_WIDGET_SPREAD_SHEET:
		EZ_SSheetSelectionBG(widget) = pixel_value;
		break;
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		EZ_IListBoxSelectionBG(twidget) = pixel_value;
		break;
	      case EZ_WIDGET_ILIST_BOX:
		EZ_IListBoxSelectionBG(widget) = pixel_value;
		break;
	      case EZ_WIDGET_TEXT:
		twidget = EZ_TextIText(widget); 
		EZ_ITextTextSelectionBG(twidget) = pixel_value;
		break;
	      case EZ_WIDGET_ITEXT:
		EZ_ITextTextSelectionBG(widget) = pixel_value;
		break;
	      case EZ_WIDGET_ENTRY:
		EZ_EntrySelectionBG(widget) = pixel_value;
		break;
	      case EZ_WIDGET_ITREE:
		EZ_ITreeWHighlightBG(widget) = pixel_value;
		break;
	      case EZ_WIDGET_TREE:
		twidget = EZ_TreeWITree(widget);
		EZ_ITreeWHighlightBG(twidget) = pixel_value;
		break;
	      case EZ_WIDGET_IFANCY_LIST_BOX:
		EZ_IFListBoxHighlightBG(widget) = pixel_value;		
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		twidget = EZ_FListBoxIFListBox(widget);
		EZ_IFListBoxHighlightBG(twidget) = pixel_value;		
		break;
	      case EZ_WIDGET_IWORK_AREA:
		EZ_IWorkAreaHighlightBG(widget) = pixel_value;		
		break;
	      case EZ_WIDGET_WORK_AREA:
		twidget = EZ_WorkAreaIWorkArea(widget);
		EZ_IWorkAreaHighlightBG(twidget) = pixel_value;		
		break;
	      case EZ_WIDGET_OPTIONAL_ENTRY:
		{
		  EZ_Widget *listbox = (EZ_Widget *)EZ_OptEntryListbox(widget);
		  twidget = (EZ_Widget *)EZ_OptEntryEntry(widget);
		  EZ_EntrySelectionBG(twidget) = pixel_value;
		  twidget =  EZ_ListBoxIListBox(listbox);
		  EZ_IListBoxSelectionBG(twidget) = pixel_value;
		}
		break;
              case EZ_WIDGET_FILE_SELECTOR:
                {
                  EZ_Widget *oea =EZ_FileSelectorPatternEntry(widget);
                  EZ_Widget *oeb =EZ_FileSelectorSelectionEntry(widget);
                  EZ_Widget *lbxa=EZ_FileSelectorDirListbox(widget);
                  EZ_Widget *lbxb=EZ_FileSelectorFileListbox(widget);
                  EZ_Widget *tmp =EZ_OptEntryEntry(oea);
                  EZ_EntrySelectionBG(tmp) = pixel_value;
                  tmp = EZ_OptEntryListbox(oea); 
                  tmp = EZ_ListBoxIListBox(tmp);
		  EZ_IListBoxSelectionBG(tmp) = pixel_value;
                  tmp =EZ_OptEntryEntry(oeb);
                  EZ_EntrySelectionBG(tmp) = pixel_value;
                  tmp = EZ_OptEntryListbox(oeb); 
                  tmp = EZ_ListBoxIListBox(tmp);
		  EZ_IListBoxSelectionBG(tmp) = pixel_value;
                  tmp = EZ_ListBoxIListBox(lbxa);
		  EZ_IListBoxSelectionBG(tmp) = pixel_value;
                  tmp = EZ_ListBoxIListBox(lbxb);
		  EZ_IListBoxSelectionBG(tmp) = pixel_value;
                }
                break;
	      default:
		EZ_ConfigureError(widget,option, "option not applicable"); 
		break;
	      }
	  }
	  break;
	case EZ_SELECTION_FOREGROUND:
	  {
	    char *color;
	    EZ_Widget *twidget;
	    unsigned long  pixel_value;

	    color     = (char *)p1;
	    pixel_value = EZ_AllocateColorFromName(color);
            if(pixel_value == 0) pixel_value = ~0;
	    switch(type)
	      {
              case EZ_WIDGET_FILE_SELECTOR:
                {
                  EZ_Widget *oea =EZ_FileSelectorPatternEntry(widget);
                  EZ_Widget *oeb =EZ_FileSelectorSelectionEntry(widget);
                  EZ_Widget *lbxa=EZ_FileSelectorDirListbox(widget);
                  EZ_Widget *lbxb=EZ_FileSelectorFileListbox(widget);
                  EZ_Widget *tmp =EZ_OptEntryEntry(oea);
                  EZ_EntrySelectionFG(tmp) = pixel_value;
                  tmp = EZ_OptEntryListbox(oea); 
                  tmp = EZ_ListBoxIListBox(tmp);
		  EZ_IListBoxSelectionFG(tmp) = pixel_value;
                  tmp =EZ_OptEntryEntry(oeb);
                  EZ_EntrySelectionFG(tmp) = pixel_value;
                  tmp = EZ_OptEntryListbox(oeb); 
                  tmp = EZ_ListBoxIListBox(tmp);
		  EZ_IListBoxSelectionFG(tmp) = pixel_value;
                  tmp = EZ_ListBoxIListBox(lbxa);
		  EZ_IListBoxSelectionFG(tmp) = pixel_value;
                  tmp = EZ_ListBoxIListBox(lbxb);
		  EZ_IListBoxSelectionFG(tmp) = pixel_value;
                }
                break;
	      case EZ_WIDGET_OPTIONAL_ENTRY:
		{
		  EZ_Widget *listbox = (EZ_Widget *)EZ_OptEntryListbox(widget);
		  twidget = (EZ_Widget *)EZ_OptEntryEntry(widget);
		  EZ_EntrySelectionFG(twidget) = pixel_value;
		  twidget =  EZ_ListBoxIListBox(listbox);
		  EZ_IListBoxSelectionFG(twidget) = pixel_value;
		}
		break;
	      case EZ_WIDGET_ENTRY:
		EZ_EntrySelectionFG(widget) = pixel_value;
		break;
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		EZ_IListBoxSelectionFG(twidget) = pixel_value;
		break;
	      case EZ_WIDGET_ILIST_BOX:
		EZ_IListBoxSelectionFG(widget) = pixel_value;
		break;
	      case EZ_WIDGET_IWORK_AREA:
		EZ_IWorkAreaHighlightFG(widget) = pixel_value;		
		break;
	      case EZ_WIDGET_WORK_AREA:
		twidget = EZ_WorkAreaIWorkArea(widget);
		EZ_IWorkAreaHighlightFG(twidget) = pixel_value;		
		break;
	      case EZ_WIDGET_IFANCY_LIST_BOX:
		EZ_IFListBoxHighlightFG(widget) = pixel_value;		
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		twidget = EZ_FListBoxIFListBox(widget);
		EZ_IFListBoxHighlightFG(twidget) = pixel_value;		
		break;
	      case EZ_WIDGET_ITREE:
		EZ_ITreeWHighlightFG(widget) = pixel_value;
		break;
	      case EZ_WIDGET_TREE:
		twidget = EZ_TreeWITree(widget);
		EZ_ITreeWHighlightFG(twidget) = pixel_value;
		break;
	      case EZ_WIDGET_SPREAD_SHEET:
		EZ_SSheetSelectionFG(widget) = pixel_value;
		break;
	      default:
		EZ_ConfigureError(widget,option, "option not applicable"); 
		break;
	      }
	  }
	  break;
	case EZ_X:
	case EZ_Y:
	  value = i1;
	  if(option == EZ_X)
	    {
	      EZ_SetWidgetXSetFlag(widget);
	      EZ_WidgetOriginX(widget) = value;
	    }
	  else
	    {
	      EZ_SetWidgetYSetFlag(widget);
	      EZ_WidgetOriginY(widget) = value;		  
	    }
	  size_changed = 1;
	  break;
	case EZ_LOCATION:
	  {
	    EZ_SetWidgetXYSetFlag(widget);
	    EZ_WidgetOriginX(widget) = i1;
	    EZ_WidgetOriginY(widget) = i2;
	  }
	break;
	case EZ_GEOMETRY:
	  {
	    EZ_SetWidgetXYSetFlag(widget);
	    EZ_SetWidgetSizeSetFlag(widget);
	    EZ_WidgetOriginX(widget) = i1;
	    EZ_WidgetOriginY(widget) = i2;
	    EZ_WidgetWidthHint(widget) = i3;
	    EZ_WidgetHeightHint(widget) = i4;
	    size_changed = 1;
	  }
	break;
	case EZ_SIZE:
	  {
	    EZ_SetWidgetSizeSetFlag(widget);
	    EZ_WidgetWidthHint(widget) = i1;
	    EZ_WidgetHeightHint(widget) = i2;
	    if(EZ_WidgetWidth(widget) != i1 ||
	       EZ_WidgetHeight(widget) != i2)
	      size_changed = 1;
	  }
	break;
	case EZ_SIZE_HINT:
	  {
	    EZ_SetWidgetWidthHintFlag(widget);
	    EZ_SetWidgetHeightHintFlag(widget);
	    EZ_WidgetWidthHint(widget) = i1;
	    EZ_WidgetHeightHint(widget) = i2;
	    if(EZ_WidgetWidth(widget) != i1 ||
	       EZ_WidgetHeight(widget) != i2)
	      size_changed = 1;
	  }
	break;

	case EZ_WIDTH:
	case EZ_WIDTH_HINT:
	  value = i1;
	  if(EZ_WidgetWidthHint(widget) != value)
	    {
	      EZ_WidgetWidthHint(widget) = value;
	      if(EZ_WidgetWidth(widget) != value)  size_changed = 1;
	    }
	  if(option == EZ_WIDTH) { EZ_SetWidgetWidthSetFlag(widget);}
	  else { EZ_SetWidgetWidthHintFlag(widget);}
	  break;

	case EZ_HEIGHT:
	case EZ_HEIGHT_HINT:
	  value = i1;
	  if(EZ_WidgetHeightHint(widget) != value)
	    {
	      EZ_WidgetHeightHint(widget) = value;
	      if(EZ_WidgetHeight(widget) != value)
		size_changed = 1;
	    }
	  if(option == EZ_HEIGHT){ EZ_SetWidgetHeightSetFlag(widget);}
	  else { EZ_SetWidgetHeightHintFlag(widget); }
	  break;
	case EZ_INDICATOR_TYPE:
	  value = i1;
	  if(value >= 0 && value <= EZ_TICK_INDICATOR)
	    {
              size_changed = 1;
	      switch(type)
		{
		case EZ_WIDGET_CHECK_BUTTON:
		case EZ_WIDGET_MENU_CHECK_BUTTON:
		  EZ_CButtonCheckType(widget) = value;
		  break;
		case EZ_WIDGET_RADIO_BUTTON:
		case EZ_WIDGET_MENU_RADIO_BUTTON:
		  EZ_RButtonCheckType(widget) = value;
		  break;
		case EZ_WIDGET_MENU_BUTTON:
		  EZ_MButtonCheckType(widget) = value;
		  break;
                case EZ_WIDGET_LED:
                  if(EZ_LEDIndicator(widget) != value)
                    {
                      EZ_LEDIndicator(widget) = value;
                      EZ_LEDStatus(widget) |= LED_BLOCKED;
                      EZ_LEDDirty(widget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
                    }
                  break;
                case EZ_WIDGET_PANE_HANDLE:
                  EZ_PaneHandleStyle(widget) = value;
                  break;
		default:
                  size_changed = 0;
		  EZ_ConfigureError(widget,option, "option not applicable"); 
		  break;
		}
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");
	  break;
	case EZ_INDICATOR_COLOR:
	  {
	    char *color;
	    unsigned long  pixel_value;
	    color     = (char *)p1;
	    pixel_value = EZ_AllocateColorFromName(color);
	    switch(type)
	      {
	      case EZ_WIDGET_CHECK_BUTTON:
	      case EZ_WIDGET_MENU_CHECK_BUTTON:
		EZ_CButtonCheckColor(widget) = pixel_value;
		break;
	      case EZ_WIDGET_RADIO_BUTTON:
	      case EZ_WIDGET_MENU_RADIO_BUTTON:
		EZ_RButtonCheckColor(widget) = pixel_value;
		break;
	      case EZ_WIDGET_MENU_BUTTON:
		EZ_MButtonCheckColor(widget) = pixel_value;		
		break;
              case EZ_WIDGET_DIAL:
                EZ_DialTForeground(widget) = pixel_value;
                break;
	      default:
		EZ_ConfigureError(widget,option, "option not applicable"); 
		break;	    
	      }
	  }
	  break;
	case EZ_EMBEDER:
	  value = i1;
	  if(value) { EZ_SetWidgetEmbederFlag(widget);}
	  else { EZ_ClearWidgetEmbederFlag(widget); }
	  break;
	case EZ_BORDER_WIDTH:
	  value = i1;
	  if(value >= 0 && value <= 100 )
	    {
	      EZ_Widget *twidget;
	      switch(type)
		{
		case EZ_WIDGET_WORK_AREA:
		  {
		    twidget = EZ_WorkAreaIWorkArea(widget);
		    if(EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		    twidget = EZ_WorkAreaHScroll(widget);
		    if(twidget && EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ScrollBorderWidth(twidget) = value; /* 6.3.96 */
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		    twidget = EZ_WorkAreaVScroll(widget);
		    if(twidget && EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ScrollBorderWidth(twidget) = value; /* 6.3.96 */
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		  }
		  break;		  
		case EZ_WIDGET_FANCY_LIST_BOX:
		  {
		    twidget = EZ_FListBoxIFListBox(widget);
		    if(EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		    twidget = EZ_FListBoxHScroll(widget);
		    if(twidget && EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ScrollBorderWidth(twidget) = value; /* 6.3.96 */
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		    twidget = EZ_FListBoxVScroll(widget);
		    if(twidget && EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ScrollBorderWidth(twidget) = value; /* 6.3.96 */
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		  }
		  break;
		case EZ_WIDGET_TREE:
		  {
		    twidget = EZ_TreeWITree(widget);
		    if(EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		    twidget = EZ_TreeWHScroll(widget);
		    if(twidget && EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ScrollBorderWidth(twidget) = value; /* 6.3.96 */
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		    twidget = EZ_TreeWVScroll(widget);
		    if(twidget && EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ScrollBorderWidth(twidget) = value; /* 6.3.96 */
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		  }
		  break;
		case EZ_WIDGET_LIST_BOX:
		  {
		    twidget = EZ_ListBoxIListBox(widget);
		    if(EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		    twidget = EZ_ListBoxHScroll(widget);
		    if(twidget && EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ScrollBorderWidth(twidget) = value; /* 6.3.96 */
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		    twidget = EZ_ListBoxVScroll(widget);
		    if(twidget && EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ScrollBorderWidth(twidget) = value; /* 6.3.96 */
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		  }
		  break;
		case EZ_WIDGET_TEXT:
		  {
		    twidget = EZ_TextIText(widget);		
		    if(EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		    twidget = EZ_TextHScroll(widget);	      
		    if(twidget && EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ScrollBorderWidth(twidget) = value;
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		    twidget = EZ_TextVScroll(widget);	      
		    if(twidget && EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ScrollBorderWidth(twidget) = value;
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		  }
		  break;
                case EZ_WIDGET_TERM:
		  {
		    twidget = EZ_TermITerm(widget);		
		    if(EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		    twidget = EZ_TermVScroll(widget);	      
		    if(twidget && EZ_WidgetBorderWidth(twidget) != value)
		      {
			EZ_WidgetBorderWidth(twidget) = value;
			EZ_ScrollBorderWidth(twidget) = value;
			EZ_ClearWidgetSizeComputedFlag(twidget);  
			size_changed = 1;
		      }
		  }
                  break;
		case EZ_WIDGET_NB_PAGE:
		  {
		    EZ_Widget *twidget = EZ_NBPageCompanion(widget);
		    EZ_WidgetBorderWidth(widget) = value;
		    EZ_WidgetBorderWidth(twidget) = value;
		    EZ_ClearWidgetSizeComputedFlag(twidget);  
		    EZ_ClearWidgetSizeComputedFlag(widget);  
		    size_changed = 1;
		  }
		  break;
		case EZ_WIDGET_HORIZONTAL_SCROLLBAR:
		case EZ_WIDGET_VERTICAL_SCROLLBAR:
		  {
		    if(EZ_WidgetBorderWidth(widget) != value)
		      {
			EZ_WidgetBorderWidth(widget) = value;
			EZ_ScrollBorderWidth(widget) = value; /* 6.3.96 */
			EZ_ClearWidgetSizeComputedFlag(widget);  
			size_changed = 1;
		      }
		  }
		  break;
		case EZ_WIDGET_3D_CANVAS:
		  break;
		default:
		  if(EZ_WidgetBorderWidth(widget) != value)
		    {
		      EZ_WidgetBorderWidth(widget) = value;
		      EZ_ClearWidgetSizeComputedFlag(widget);  
		      size_changed = 1;
		    }
		break;
		}
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");
	  break;

	case EZ_BORDER_TYPE:
	  value = i1;
	  if(value >= EZ_BORDER_STYLE_BEGIN && 
	     value <= EZ_BORDER_STYLE_END &&
	     type != EZ_WIDGET_NB_PAGE)
	    {
	      if(EZ_WidgetBorderStyle(widget) != value)
		{
		  EZ_WidgetBorderStyle(widget) = value;
		  EZ_ClearWidgetSizeComputedFlag(widget);  
		  size_changed = 1;
		}
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");
	  break;
	case EZ_PADX:
	  value = i1;
	  {
	      EZ_Widget *twidget;
	      switch(type)
		{
		case EZ_WIDGET_TREE:
		  twidget = EZ_TreeWITree(widget);		  
		  break;
		case EZ_WIDGET_LIST_BOX:
		  twidget = EZ_ListBoxIListBox(widget);
		  break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  twidget = EZ_FListBoxIFListBox(widget);
		  break;
		case EZ_WIDGET_WORK_AREA:
		  twidget = EZ_WorkAreaIWorkArea(widget);
		  break;
		default:
		  twidget = widget;
		  break;
		}
	      if(EZ_WidgetPadX(twidget) != value)
		{
		  EZ_WidgetPadX(twidget) = value;
		  EZ_ClearWidgetSizeComputedFlag(twidget);  
		  size_changed = 1;
		}
	    }
	  break;
	case EZ_PADY:
	  value = i1;
	    {
	      EZ_Widget *twidget;
	      switch(type)
		{
                case EZ_WIDGET_TERM:
                  twidget = EZ_TermITerm(widget);
                  break;
		case EZ_WIDGET_TREE:
		  twidget = EZ_TreeWITree(widget);		  
		  break;
		case EZ_WIDGET_LIST_BOX:
		  twidget = EZ_ListBoxIListBox(widget);
		  break;
		case EZ_WIDGET_TEXT:
		  twidget = EZ_TextIText(widget);		
		  break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  twidget = EZ_FListBoxIFListBox(widget);
		  break;
		case EZ_WIDGET_WORK_AREA:
		  twidget = EZ_WorkAreaIWorkArea(widget);
		  break;
		default:
		  twidget = widget;
		  break;
		}
	      if( EZ_WidgetPadY(twidget) != value)
		{
		  EZ_WidgetPadY(twidget) = value;
		  EZ_ClearWidgetSizeComputedFlag(twidget);  
		  size_changed = 1;
		}
	    }
	  break;
	case EZ_IPADX:
	  value = i1;
	    {
	      EZ_Widget *twidget;
	      switch(type)
		{
                case EZ_WIDGET_TERM:
                  twidget = EZ_TermITerm(widget);
                  break;
		case EZ_WIDGET_LIST_BOX:
		  twidget = EZ_ListBoxIListBox(widget);
		  break;
		case EZ_WIDGET_TEXT:
		  twidget = EZ_TextIText(widget);
		  break;
		case EZ_WIDGET_NOTE_BOOK:
		  twidget = NULL;
		  break;
		case EZ_WIDGET_TREE:
		  twidget = EZ_TreeWITree(widget);
		  break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  twidget = EZ_FListBoxIFListBox(widget);
		  break;
		case EZ_WIDGET_WORK_AREA:
		  twidget = EZ_WorkAreaIWorkArea(widget);
		  break;
		default:
		  twidget = widget;
		  break;
		}
	      if(twidget && EZ_WidgetSepX(twidget) != value)
		{
		  EZ_WidgetSepX(twidget) = value;
		  size_changed = 1;
		}
	    }
	  break;
	case EZ_IPADY:
	  value = i1;
	    {
	      EZ_Widget *twidget;
	      switch(type)
		{
                case EZ_WIDGET_TERM:
                  twidget = EZ_TermITerm(widget);
                  break;
		case EZ_WIDGET_LIST_BOX:
		  twidget = EZ_ListBoxIListBox(widget);
		  break;
		case EZ_WIDGET_TEXT:
		  twidget = EZ_TextIText(widget);
		  break;
		case EZ_WIDGET_NOTE_BOOK:
		  twidget = NULL;
		  break;
		case EZ_WIDGET_TREE:
		  twidget = EZ_TreeWITree(widget);
		  break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  twidget = EZ_FListBoxIFListBox(widget);
		  break;
		case EZ_WIDGET_WORK_AREA:
		  twidget = EZ_WorkAreaIWorkArea(widget);
		  break;
		default:
		  twidget = widget;
		  break;
		}
	      if(twidget && EZ_WidgetSepY(twidget) != value)
		{
		  EZ_WidgetSepY(twidget) = value;
		  size_changed = 1;
		}
	    }
	  break;
	case EZ_STACKING:
	  value = i1;
	  if(value >= EZ_WIDGET_STACKING_BEGIN &&
	     value <= EZ_WIDGET_STACKING_END)
	    {
	      switch(type)
		{
		case EZ_WIDGET_TREE:
		case EZ_WIDGET_LIST_BOX:
		case EZ_WIDGET_TEXT:
		case EZ_WIDGET_WORK_AREA:
		case EZ_WIDGET_OPTIONAL_ENTRY:
		case EZ_WIDGET_FILE_SELECTOR:
                case EZ_WIDGET_TERM:
		  EZ_ConfigureError(widget,option, "option not available");
		  break;
		case EZ_WIDGET_NOTE_BOOK:
		  size_changed = EZ_SetNoteBookOrientationA(widget, value);
		  break;
		default:
		  if(EZ_WidgetStacking(widget) != value)
		    {
		      EZ_WidgetStacking(widget) = value;
		      size_changed = 1;
		    }
		  break;
		}
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");
	  break;
	case EZ_SIDE:
	  value = i1;
	  if(value >= EZ_WIDGET_SIDE_BEGIN &&
	     value <= EZ_WIDGET_SIDE_END)
	    {
	      if(type != EZ_WIDGET_LIST_BOX && type != EZ_WIDGET_TEXT &&
		 type != EZ_WIDGET_NOTE_BOOK && type != EZ_WIDGET_TREE &&
                 type != EZ_WIDGET_TERM)
		if(EZ_WidgetAlignment(widget) != value)
		  {
		    EZ_WidgetAlignment(widget) = value;	  
		    size_changed = 1;
		  }
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");
	  break;
	case EZ_LABEL_POSITION:
	  value = i1;
	  if(value >= EZ_WIDGET_LABEL_POSITION_BEGIN &&
	     value <= EZ_WIDGET_LABEL_POSITION_END)
	    {
	      if(EZ_WidgetLabelPosition(widget) != value)
		{
		  EZ_WidgetLabelPosition(widget) = value;	  
		  size_changed = 1;
		}
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");
	  break;
	case EZ_JUSTIFICATION:
	  value = i1;
	  if(value >= EZ_CENTER &&   value <= EZ_RIGHT)  /* 0, 1, 2 :: CENTER, LEFT, RIGHT */
	    {
	      if(EZ_WidgetJustification(widget) != value)
		{
		  EZ_WidgetJustification(widget) = value;	  
		  size_changed = 1;
		}
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");
	  break;
	case EZ_FILL_MODE:
	  value = i1;
	  if(value >= EZ_WIDGET_FILL_BEGIN &&
	     value <= EZ_WIDGET_FILL_END)
	    {
	      switch(type)
		{
		case EZ_WIDGET_TREE:
		case EZ_WIDGET_LIST_BOX:
		case EZ_WIDGET_TEXT:
		case EZ_WIDGET_NOTE_BOOK:
		case EZ_WIDGET_FILE_SELECTOR:
		case EZ_WIDGET_OPTIONAL_ENTRY:
                case EZ_WIDGET_TERM:
		  break;
		default:
		  if(EZ_WidgetFillMode(widget) != value)
		    {
		      EZ_WidgetFillMode(widget) = value; 
		      size_changed = 1;
		    }
		}
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");
	  break;
	case EZ_TEXT_SPACING:
	  {
	    EZ_Widget *twidget = NULL;
	    int        value = i1;
            switch(type)
              {
              case EZ_WIDGET_TEXT:
                widget = EZ_TextIText(widget);
                /* drop */
              case EZ_WIDGET_ITEXT:
                twidget = widget;
		EZ_ITextVSpacing(twidget) = value;
		size_changed = 1;
                break;
              case EZ_WIDGET_LIST_BOX:
		widget = EZ_ListBoxIListBox(widget);
                /*drop*/
              case EZ_WIDGET_ILIST_BOX:
                twidget = widget;
                EZ_IListBoxSpacing(twidget) = value;
		size_changed = 1;
                break;
              case EZ_WIDGET_OPTIONAL_ENTRY:
                widget = (EZ_Widget *)EZ_OptEntryListbox(widget);                
                widget = EZ_ListBoxIListBox(widget);
                EZ_IListBoxSpacing(widget) = value;
                size_changed = 1;
                break;
              case EZ_WIDGET_FILE_SELECTOR:
                {
                  EZ_Widget *oea =EZ_FileSelectorPatternEntry(widget);
                  EZ_Widget *oeb =EZ_FileSelectorSelectionEntry(widget);
                  EZ_Widget *lbxa=EZ_FileSelectorDirListbox(widget);
                  EZ_Widget *lbxb=EZ_FileSelectorFileListbox(widget);
                  widget = (EZ_Widget *)EZ_OptEntryListbox(oea);                
                  widget = EZ_ListBoxIListBox(widget);         
                  EZ_IListBoxSpacing(widget) = value;
                  widget = (EZ_Widget *)EZ_OptEntryListbox(oeb);                
                  widget = EZ_ListBoxIListBox(widget);         
                  EZ_IListBoxSpacing(widget) = value;
                  widget = EZ_ListBoxIListBox(lbxa);         
                  EZ_IListBoxSpacing(widget) = value;
                  widget = EZ_ListBoxIListBox(lbxb);         
                  EZ_IListBoxSpacing(widget) = value;
                  size_changed = 1;
                }
                break;
              default:
                EZ_ConfigureError(widget,option, "option not applicable"); 
                break;
              }
          }
	  break;
	case EZ_FOREGROUND:
	  {
	    char *color;
	    EZ_Widget *twidget = widget;
	    unsigned long  pixel_value;

	    color     = (char *)p1;
	    pixel_value = EZ_AllocateColorFromName(color);
	    if(pixel_value == 0) { pixel_value = EZ_DefaultForeground; }
	    switch(type)
	      {
              case EZ_WIDGET_TERM:
                twidget = EZ_TermITerm(widget);
                EZ_ITermDirty(twidget) = 1;
                break;
	      case EZ_WIDGET_TREE:
		twidget = EZ_TreeWITree(widget);		  
		break;
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		break;
	      case EZ_WIDGET_TEXT:
		twidget = EZ_TextIText(widget); 
		break;
	      case  EZ_WIDGET_NOTE_BOOK:
		{
		  EZ_Widget *tframe = EZ_NoteBookTFrame(widget);
		  EZ_Widget *stub = EZ_WidgetChildren(tframe);
		  while(stub)
		    {
		      if(EZ_WidgetForeground(stub) == EZ_DefaultForeground)
			EZ_WidgetForeground(stub) = pixel_value;
		      stub = EZ_WidgetSibling(stub);
		    }
		}
		break;
	      case EZ_WIDGET_NB_PAGE:
		{
		  EZ_Widget *companion = EZ_NBPageCompanion(widget);
		  EZ_WidgetForeground(companion) = pixel_value;
		}
		break;
	      case  EZ_WIDGET_OPTIONAL_ENTRY:
		{
		  EZ_Widget *entry = (EZ_Widget *)EZ_OptEntryEntry(widget);
		  EZ_Widget *listbox = (EZ_Widget *)EZ_OptEntryListbox(widget);
		  EZ_WidgetForeground(entry) = pixel_value;
		  twidget =  EZ_ListBoxIListBox(listbox);
		}
              break;
              case EZ_WIDGET_LED:
                EZ_LEDStatus(twidget) |= LED_BLOCKED;
                EZ_LEDDirty(twidget) |= (LED_PIXMAP_DIRTY | LED_PIXMAPC_DIRTY);
                break;
	      default:
		break;
	      }
	    EZ_WidgetForeground(twidget) = pixel_value;
	  }
	  break;
	case EZ_TEXT_BACKGROUND:
	  {
	    char *color;
	    unsigned long  pixel_value;
	    EZ_Widget *twidget;

	    color = (char *)p1;
            pixel_value = EZ_AllocateColorFromName(color);
            if(pixel_value == 0) pixel_value = ~0;
	    switch(type)
	      {
              case EZ_WIDGET_FILE_SELECTOR:
                {
                  EZ_Widget *oea =EZ_FileSelectorPatternEntry(widget);
                  EZ_Widget *oeb =EZ_FileSelectorSelectionEntry(widget);
                  EZ_Widget *lbxa=EZ_FileSelectorDirListbox(widget);
                  EZ_Widget *lbxb=EZ_FileSelectorFileListbox(widget);
                  EZ_Widget *tmp =EZ_OptEntryEntry(oea);
                  EZ_EntryTextBG(tmp) = pixel_value;                  
                  tmp = EZ_OptEntryListbox(oea); 
                  tmp = EZ_ListBoxIListBox(tmp);
                  EZ_IListBoxTextBG(tmp) = pixel_value;
                  tmp =EZ_OptEntryEntry(oeb);
                  EZ_EntryTextBG(tmp) = pixel_value;                  
                  tmp = EZ_OptEntryListbox(oeb); 
                  tmp = EZ_ListBoxIListBox(tmp);
                  EZ_IListBoxTextBG(tmp) = pixel_value;
                  tmp = EZ_ListBoxIListBox(lbxa);
                  EZ_IListBoxTextBG(tmp) = pixel_value;
                  tmp = EZ_ListBoxIListBox(lbxb);
                  EZ_IListBoxTextBG(tmp) = pixel_value;
                }
                break;
              case EZ_WIDGET_OPTIONAL_ENTRY:
                {
                  EZ_Widget *listbox = EZ_OptEntryListbox(widget);                  
                  listbox = EZ_ListBoxIListBox(listbox);
                  EZ_IListBoxTextBG(listbox) = pixel_value;
                }
                widget = EZ_OptEntryEntry(widget);
                /* drop */
              case EZ_WIDGET_ENTRY:
                if(EZ_EntryTextBG(widget) != pixel_value)
                  {
                    EZ_EntryTextBG(widget) = pixel_value;
                    size_changed = 1;
                  }
                break;
	      case EZ_WIDGET_TERM:
		twidget = EZ_TermITerm(widget); 
		if(EZ_ITermTextBG(twidget) != pixel_value)
		  {
		    EZ_ITermTextBG(twidget) = pixel_value;
		    size_changed = 1;
		  }
		break;
	      case EZ_WIDGET_ITERM:
		if(EZ_ITermTextBG(widget) != pixel_value)
		  {
		    EZ_ITermTextBG(widget) = pixel_value;
		    size_changed = 1;
		  }
		break;
	      case EZ_WIDGET_TREE:
		twidget = EZ_TreeWITree(widget); 
		if(EZ_ITreeWTextBG(twidget) != pixel_value)
		  {
		    EZ_ITreeWTextBG(twidget) = pixel_value;
		    size_changed = 1;
		  }
		break;
	      case EZ_WIDGET_ITREE:
		if(EZ_ITreeWTextBG(widget) != pixel_value)
		  {
		    EZ_ITreeWTextBG(widget) = pixel_value;
		    size_changed = 1;
		  }
		break;
	      case EZ_WIDGET_TEXT:
		twidget = EZ_TextIText(widget);
		if(EZ_ITextTextBG(twidget) != pixel_value)
		  {
		    EZ_ITextTextBG(twidget) = pixel_value;
		    size_changed = 1;
		  }
		break;
	      case EZ_WIDGET_ITEXT:
		if(EZ_ITextTextBG(widget) != pixel_value)
		  {
		    EZ_ITextTextBG(widget) = pixel_value;
		    size_changed = 1;
		  }		
		break;
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		if(EZ_IListBoxTextBG(twidget) != pixel_value)
		  {
		    EZ_IListBoxTextBG(twidget) = pixel_value;
		    size_changed = 1;
		  }
		break;
	      case EZ_WIDGET_ILIST_BOX:
		if(EZ_IListBoxTextBG(widget) != pixel_value)
		  {
		    EZ_IListBoxTextBG(widget) = pixel_value;
		    size_changed = 1;
		  }
		break;
	      case EZ_WIDGET_IFANCY_LIST_BOX:
		if(EZ_IFListBoxTextBG(widget) != pixel_value)
		  {
		    EZ_IFListBoxTextBG(widget) = pixel_value;
		    size_changed = 1;
		  }
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		twidget = EZ_FListBoxIFListBox(widget);
		if(EZ_IFListBoxTextBG(twidget) != pixel_value)
		  {
		    EZ_IFListBoxTextBG(twidget) = pixel_value;
		    size_changed = 1;
		  }
		break;
	      case EZ_WIDGET_IWORK_AREA:
		if(EZ_IWorkAreaTextBG(widget) != pixel_value)
		    {
		      EZ_IWorkAreaTextBG(widget) = pixel_value;
		      size_changed = 1;
		    }
		break;		
	      case EZ_WIDGET_WORK_AREA:
		twidget = EZ_WorkAreaIWorkArea(widget);
		if(EZ_IWorkAreaTextBG(twidget) != pixel_value)
		  {
		    EZ_IWorkAreaTextBG(twidget) = pixel_value;
		    size_changed = 1;
		  }
		break;
	      case EZ_WIDGET_SPREAD_SHEET:
		if(EZ_SSheetTextBG(widget) != pixel_value)  
		  {
		    EZ_SSheetTextBG(widget) = pixel_value;
		    size_changed = 1;
		  }
		break;
	      default:
		EZ_ConfigureError(widget,option, "Optional not available");
		break;
	      }
	  }
	  break;
	case EZ_BACKGROUND:
	  {
	    char *color;
	    unsigned long  pixel_value;
	    
	    color     = (char *)p1;
	    pixel_value = EZ_AllocateNewBackground(color);
	    if(type == EZ_WIDGET_NB_PAGE)
	      {
		EZ_Widget *companion = EZ_NBPageCompanion(widget);
		EZ_WidgetBackground(companion) = pixel_value;
	      }
	    EZ_WidgetBackground(widget) = pixel_value; 
	  }
	  break;
	case EZ_HIGHLIGHT_BG:
	  {
	    char *color;
	    unsigned long  pixel_value, spv;
	    EZ_Widget *twidget = widget, *hs = NULL, *vs = NULL;

	    color     = (char *)p1;
	    pixel_value = EZ_AllocateNewBackground(color);
            if(pixel_value)spv = pixel_value; else spv = ~0;
            switch(type)
              {
              case EZ_WIDGET_NB_PAGE:
                {
                  EZ_Widget *companion = EZ_NBPageCompanion(widget);
                  {EZ_UninstallHighlightColor(companion); }
                  EZ_WidgetHighlightBG(companion) = pixel_value;
                  if(pixel_value) { EZ_SetWidgetHighlightBGFlag(companion);}
                  else { EZ_ClearWidgetHighlightBGFlag(companion); }
                }
                break;
	      case EZ_WIDGET_NOTE_BOOK:
		{
		  EZ_Widget *tframe = EZ_NoteBookTFrame(widget);
		  EZ_Widget *stub = EZ_WidgetChildren(tframe);
		  while(stub)
		    {
                      {EZ_UninstallHighlightColor(stub); }
                      EZ_WidgetHighlightBG(stub) = pixel_value;
                      if(pixel_value) { EZ_SetWidgetHighlightBGFlag(stub);}
                      else { EZ_ClearWidgetHighlightBGFlag(stub); }
		      stub = EZ_WidgetSibling(stub);
		    }
		}
                break;
	      case EZ_WIDGET_TREE: 
		hs = EZ_TreeWHScroll(widget);
		vs = EZ_TreeWVScroll(widget);
                twidget = EZ_TreeWITree(widget);		                  
                /* drop */
	      case EZ_WIDGET_ITREE: 
                EZ_ITreeWHighlightBG(twidget) = spv;
		break;
	      case EZ_WIDGET_WORK_AREA:
		hs = EZ_WorkAreaHScroll(widget);
		vs = EZ_WorkAreaVScroll(widget);
                twidget = EZ_WorkAreaIWorkArea(widget);
                /* drop */
	      case EZ_WIDGET_IWORK_AREA:
		EZ_IWorkAreaHighlightBG(twidget) = spv;
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		hs = EZ_FListBoxHScroll(widget);
		vs = EZ_FListBoxVScroll(widget);
                twidget = EZ_FListBoxIFListBox(widget);
                /* drop */
	      case EZ_WIDGET_IFANCY_LIST_BOX:
		EZ_IFListBoxHighlightBG(twidget) = spv;
		break;
	      case EZ_WIDGET_LIST_BOX:
		hs = EZ_ListBoxHScroll(widget);
		vs = EZ_ListBoxVScroll(widget);
                twidget = EZ_ListBoxIListBox(widget);
                /* drop */
	      case EZ_WIDGET_ILIST_BOX:
		EZ_IListBoxSelectionBG(twidget) = spv;
		break;
	      case EZ_WIDGET_TEXT:
		hs = EZ_TextHScroll(widget);
		vs = EZ_TextVScroll(widget);
                twidget = EZ_TextIText(widget);
                /* drop */
	      case EZ_WIDGET_ITEXT:
		EZ_ITextTextSelectionBG(twidget) = spv;
		break;
	      case EZ_WIDGET_ENTRY:
		EZ_EntrySelectionBG(widget) = spv;
		break;
	      case EZ_WIDGET_OPTIONAL_ENTRY:
		{
		  EZ_Widget *listbox = (EZ_Widget *)EZ_OptEntryListbox(widget);
		  twidget = (EZ_Widget *)EZ_OptEntryEntry(widget);
		  EZ_EntrySelectionBG(twidget) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightBGFlag(twidget);}
                  else { EZ_ClearWidgetHighlightBGFlag(twidget); }
		  twidget =  EZ_ListBoxIListBox(listbox);
		  EZ_IListBoxSelectionBG(twidget) = spv;
		  hs = EZ_ListBoxHScroll(listbox);
		  vs = EZ_ListBoxVScroll(listbox);                  
		}
              break;
              case EZ_WIDGET_FILE_SELECTOR:
                {
                  EZ_Widget *oea =EZ_FileSelectorPatternEntry(widget);
                  EZ_Widget *oeb =EZ_FileSelectorSelectionEntry(widget);
                  EZ_Widget *lbxa=EZ_FileSelectorDirListbox(widget);
                  EZ_Widget *lbxb=EZ_FileSelectorFileListbox(widget);
                  EZ_Widget *tmp =EZ_OptEntryEntry(oea);
                  EZ_EntrySelectionBG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightBGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightBGFlag(tmp); }
                  tmp = EZ_OptEntryListbox(oea); 
		  hs = EZ_ListBoxHScroll(tmp);
		  vs = EZ_ListBoxVScroll(tmp);
                  if(hs)
                    {
                      EZ_WidgetHighlightBG(hs) = pixel_value;
                      if(pixel_value) { EZ_SetWidgetHighlightBGFlag(hs);}
                      else { EZ_ClearWidgetHighlightBGFlag(hs); }
                    }
                  if(vs)
                    {
                      EZ_WidgetHighlightBG(vs) = pixel_value;
                      if(pixel_value) { EZ_SetWidgetHighlightBGFlag(vs);}
                      else { EZ_ClearWidgetHighlightBGFlag(vs); }
                    }
                  tmp = EZ_ListBoxIListBox(tmp);
                  EZ_IListBoxSelectionBG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightBGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightBGFlag(tmp); }
                  tmp =EZ_OptEntryEntry(oeb);
                  EZ_EntrySelectionBG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightBGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightBGFlag(tmp); }
                  tmp = EZ_OptEntryListbox(oeb); 
		  hs = EZ_ListBoxHScroll(tmp);
		  vs = EZ_ListBoxVScroll(tmp);
                  if(hs)
                    {
                      EZ_WidgetHighlightBG(hs) = pixel_value;
                      if(pixel_value) { EZ_SetWidgetHighlightBGFlag(hs);}
                      else { EZ_ClearWidgetHighlightBGFlag(hs); }
                    }
                  if(vs)
                    {
                      EZ_WidgetHighlightBG(vs) = pixel_value;
                      if(pixel_value) { EZ_SetWidgetHighlightBGFlag(vs);}
                      else { EZ_ClearWidgetHighlightBGFlag(vs); }
                    }
                  tmp = EZ_ListBoxIListBox(tmp);
                  EZ_IListBoxSelectionBG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightBGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightBGFlag(tmp); }
                  tmp = EZ_ListBoxIListBox(lbxa);
                  EZ_IListBoxSelectionBG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightBGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightBGFlag(tmp); }
		  hs = EZ_ListBoxHScroll(lbxa);
		  vs = EZ_ListBoxVScroll(lbxa);
                  if(hs)
                    {
                      EZ_WidgetHighlightBG(hs) = pixel_value;
                      if(pixel_value) { EZ_SetWidgetHighlightBGFlag(hs);}
                      else { EZ_ClearWidgetHighlightBGFlag(hs); }
                    }
                  if(vs)
                    {
                      EZ_WidgetHighlightBG(vs) = pixel_value;
                      if(pixel_value) { EZ_SetWidgetHighlightBGFlag(vs);}
                      else { EZ_ClearWidgetHighlightBGFlag(vs); }
                    }
                  tmp = EZ_ListBoxIListBox(lbxb);
                  EZ_IListBoxSelectionBG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightBGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightBGFlag(tmp); }
		  hs = EZ_ListBoxHScroll(lbxa);
		  vs = EZ_ListBoxVScroll(lbxa);                  
                }
                break;
	      case EZ_WIDGET_HORIZONTAL_SCROLLBAR:
		hs = widget;
		break;
	      case EZ_WIDGET_VERTICAL_SCROLLBAR:
		vs = widget;
		break;
              case EZ_WIDGET_TERM:
		vs = EZ_TermVScroll(widget);
		break;
              default: 
                break;
	      }
            {EZ_UninstallHighlightColor(twidget); }
	    EZ_WidgetHighlightBG(twidget) = pixel_value; 
            if(pixel_value) { EZ_SetWidgetHighlightBGFlag(twidget);}
            else { EZ_ClearWidgetHighlightBGFlag(twidget); }
            if(hs)
              {
                {EZ_UninstallHighlightColor(hs); }
                EZ_WidgetHighlightBG(hs) = pixel_value;
                if(pixel_value) { EZ_SetWidgetHighlightBGFlag(hs);}
                else { EZ_ClearWidgetHighlightBGFlag(hs); }
              }
            if(vs)
              {
                {EZ_UninstallHighlightColor(vs); }
                EZ_WidgetHighlightBG(vs) = pixel_value;
                if(pixel_value) { EZ_SetWidgetHighlightBGFlag(vs);}
                else { EZ_ClearWidgetHighlightBGFlag(vs); }
              }
	  }
          break;
	case EZ_HIGHLIGHT_FG:
	  {
	    char *color;
	    EZ_Widget *twidget = widget;
	    unsigned long  pixel_value, spv;
            
	    color     = (char *)p1;
	    pixel_value = EZ_AllocateColorFromName(color);
            if(pixel_value) spv = pixel_value; else spv = ~0;
	    switch(type)
	      {
	      case  EZ_WIDGET_NOTE_BOOK:
		{
		  EZ_Widget *tframe = EZ_NoteBookTFrame(widget);
		  EZ_Widget *stub = EZ_WidgetChildren(tframe);
		  while(stub)
		    {
                      {EZ_UninstallHighlightColor(stub); }
                      EZ_WidgetHighlightFG(stub) = pixel_value;
                      if(pixel_value) { EZ_SetWidgetHighlightFGFlag(stub);}
                      else { EZ_ClearWidgetHighlightFGFlag(stub); }
		      stub = EZ_WidgetSibling(stub);
		    }
		}
		break;
	      case EZ_WIDGET_NB_PAGE:
		{
		  EZ_Widget *companion = EZ_NBPageCompanion(widget);
                  {EZ_UninstallHighlightColor(companion); }
                  EZ_WidgetHighlightFG(companion) = pixel_value;
                  if(pixel_value) { EZ_SetWidgetHighlightFGFlag(companion);}
                  else { EZ_ClearWidgetHighlightFGFlag(companion); }
		}
              break;
              case EZ_WIDGET_TREE:
                twidget = EZ_TreeWITree(widget);		  
                /* drop */
              case EZ_WIDGET_ITREE:
                EZ_ITreeWHighlightFG(twidget) = spv;
                break;
              case EZ_WIDGET_LIST_BOX:
                twidget = EZ_ListBoxIListBox(widget);
                /* drop */
              case EZ_WIDGET_ILIST_BOX:
		EZ_IListBoxSelectionFG(twidget) = spv;
                break;
              case EZ_WIDGET_FANCY_LIST_BOX:
                twidget = EZ_FListBoxIFListBox(widget);
                /* drop */
              case EZ_WIDGET_IFANCY_LIST_BOX:
		EZ_IFListBoxHighlightFG(twidget) = spv;
                break;
              case EZ_WIDGET_WORK_AREA:
                twidget = EZ_WorkAreaIWorkArea(widget);
                /* drop */
              case EZ_WIDGET_IWORK_AREA:
		EZ_IWorkAreaHighlightFG(twidget) = spv;
                break;
              case EZ_WIDGET_TEXT:
                twidget = EZ_TextIText(widget);
                break;
	      case EZ_WIDGET_ENTRY:
		EZ_EntrySelectionFG(widget) = spv;
		break;
	      case EZ_WIDGET_OPTIONAL_ENTRY:
		{
		  EZ_Widget *listbox = (EZ_Widget *)EZ_OptEntryListbox(widget);
		  twidget = (EZ_Widget *)EZ_OptEntryEntry(widget);
		  EZ_EntrySelectionFG(twidget) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightFGFlag(twidget);}
                  else { EZ_ClearWidgetHighlightFGFlag(twidget); }
		  twidget =  EZ_ListBoxIListBox(listbox);
		  EZ_IListBoxSelectionFG(twidget) = spv;
		}
              break;
              case EZ_WIDGET_FILE_SELECTOR:
                {
                  EZ_Widget *oea =EZ_FileSelectorPatternEntry(widget);
                  EZ_Widget *oeb =EZ_FileSelectorSelectionEntry(widget);
                  EZ_Widget *lbxa=EZ_FileSelectorDirListbox(widget);
                  EZ_Widget *lbxb=EZ_FileSelectorFileListbox(widget);
                  EZ_Widget *tmp =EZ_OptEntryEntry(oea);
                  EZ_EntrySelectionFG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightFGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightFGFlag(tmp); }
                  tmp = EZ_OptEntryListbox(oea); 
                  tmp = EZ_ListBoxIListBox(tmp);
                  EZ_IListBoxSelectionFG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightFGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightFGFlag(tmp); }
                  tmp =EZ_OptEntryEntry(oeb);
                  EZ_EntrySelectionFG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightFGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightFGFlag(tmp); }
                  tmp = EZ_OptEntryListbox(oeb); 
                  tmp = EZ_ListBoxIListBox(tmp);
                  EZ_IListBoxSelectionFG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightFGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightFGFlag(tmp); }

                  tmp = EZ_ListBoxIListBox(lbxa);
                  EZ_IListBoxSelectionFG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightFGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightFGFlag(tmp); }
                  tmp = EZ_ListBoxIListBox(lbxb);
                  EZ_IListBoxSelectionFG(tmp) = spv;
                  if(pixel_value) { EZ_SetWidgetHighlightFGFlag(tmp);}
                  else { EZ_ClearWidgetHighlightFGFlag(tmp); }
                }
                break;
	      default:
		break;
	      }
            {EZ_UninstallHighlightColor(twidget); }
            EZ_WidgetHighlightFG(twidget) = pixel_value;
            if(pixel_value) { EZ_SetWidgetHighlightFGFlag(twidget);}
            else { EZ_ClearWidgetHighlightFGFlag(twidget); }
	  }
	  break;
	case EZ_BITMAP_FILE:
	  {
	    char *file = (char *)p1;
	    EZ_Bitmap *bitmap = (file && file[0] ? EZ_GetBitmapFromFile(file, 1):
				 (EZ_Bitmap *)NULL);
	    shape_changed |= EZ_SetWidgetPixmap(widget,bitmap,option);
	  }
	  break;
	case EZ_BITMAP_DATA:  /* this is the only option which needs 3 values */
	  {
	    char *data = (char *)p1;
	    int width = (unsigned int) i1;
	    int height = (unsigned int) i2;
	    if(width > 0 && height > 0)
	      {
		EZ_Bitmap *bitmap = EZ_GetBitmapFromData(data, 
							 (unsigned int)width,
							 (unsigned int)height);
		shape_changed |= EZ_SetWidgetPixmap(widget,bitmap,option);
	      }
	  }
	  break;
	case EZ_PIXMAP_DATA:
	  {
	    char **file = (char **)p1;
	    EZ_Bitmap *bitmap = (file == (char **)NULL? (EZ_Bitmap *)NULL:
				 EZ_GetPixmapFromData(file));
	    shape_changed |= EZ_SetWidgetPixmap(widget,bitmap,option);
	  }
	  break;
	case EZ_X_PIXMAP:
	  {
	    Pixmap pixmap = (Pixmap)ul1;
	    int    x = i1;
	    int    y = i2;
	    int width = i3;
	    int height = i4;
	    EZ_Bitmap *bitmap = EZ_GetPixmapFromPixmap(pixmap,x,y,width,height);
	    shape_changed |= EZ_SetWidgetPixmap(widget,bitmap,option);
	  }
	  break;

	  /*
	   * Allow raw rgb data to be displayed.
	   * Arno Dabekaussen, 03-05-97, Maastricht, NL.
	   */

	case EZ_RAW_RGB_DATA:
	  {
	    char *RGBmap = (char *) p1;
	    int width = (int) i1;
	    int height = (int) i2;
	    
	    EZ_Bitmap *bitmap = EZ_GetPixmapFromRawRGB(RGBmap, width, height);
	    shape_changed |= EZ_SetWidgetPixmap(widget, bitmap, option);
	  }
	  break;
	case EZ_LABEL_PIXMAP:
	  {
	    char *tmp =  ( char *)p1;
	    EZ_Bitmap *bitmap = (EZ_Bitmap *)tmp;
	    if(bitmap)	bitmap->ref += 1;
	    shape_changed |= EZ_SetWidgetPixmap(widget,bitmap,option); 
	  }
	  break;
	case EZ_IMAGE_FILE:
	  {
	    char *file = (char *)p1;
	    EZ_Bitmap *bitmap = (file == (char *)NULL? (EZ_Bitmap *)NULL:
				 EZ_GetImageFromFile(file, 1));
            shape_changed |= EZ_SetWidgetPixmap(widget,bitmap,option);
	  }
	  break;
	case EZ_BG_IMAGE_FILE:
	  {
	    char *file = (char *)p1;
	    EZ_Bitmap *old, *bitmap = NULL;
	    if(file) bitmap = EZ_CreateBackgroundPixmapsFromImageFile(file);
	    old = EZ_WidgetBGPixmap(widget);
	    EZ_WidgetBGPixmap(widget) = bitmap;
	    if(old) EZ_FreeLabelPixmap(old);
	    size_changed = 1;
            if(type ==  EZ_WIDGET_NB_PAGE)
              {
                EZ_Widget *companion = EZ_NBPageCompanion(widget);
                EZ_WidgetBGPixmap(companion) = bitmap;
                size_changed = 1;
              }
	  }
	  break;
	case EZ_BG_IMAGE_FILE_B:
	  {
	    char *file = (char *)p1;
	    EZ_Bitmap *old, *bitmap = NULL;
	    if(file) bitmap = EZ_CreateBackgroundPixmapsFromImageFile(file);
	    old = EZ_WidgetBGPixmapB(widget);
	    EZ_WidgetBGPixmapB(widget) = bitmap;
	    if(old) EZ_FreeLabelPixmap(old);
	    size_changed = 1;
	  }
	break;
	case EZ_BG_PIXMAP:
	  {
	    EZ_Bitmap *old, *cc, *bitmap = (EZ_Bitmap *)p1;
	    cc = EZ_CheckBGPixmap(bitmap);
	    if(cc) cc->ref++;
	    old = EZ_WidgetBGPixmap(widget);
	    EZ_WidgetBGPixmap(widget) = cc;
	    if(old) EZ_FreeLabelPixmap(old);
	    size_changed = 1;	    
	  }
	  break;
	case EZ_BG_PIXMAP_B:
	  {
	    EZ_Bitmap *old, *cc, *bitmap = (EZ_Bitmap *)p1;
	    cc = EZ_CheckBGPixmap(bitmap);
	    if(cc) cc->ref++;
	    old = EZ_WidgetBGPixmapB(widget);
	    EZ_WidgetBGPixmapB(widget) = cc;
	    if(old) EZ_FreeLabelPixmap(old);
	    size_changed = 1;	    
	  }
	  break;
	case EZ_FONT_ID:
	case EZ_FONT_NAME:
	  {
	    EZ_Widget *twidget = widget;
	    XFontStruct *tmpfont;
	    int boldfont;

	    if(option == EZ_FONT_ID)
	      {
		value =  i1;
		tmpfont = EZ_GetFontFromId(value);
		boldfont = EZ_FontWeightIsBold(EZ_GetFontNameFromId(value));
	      }
	    else
	      {
		char *fontname = (char *)p1;
		tmpfont = EZ_GetFontFromName(fontname);
		boldfont = EZ_FontWeightIsBold(fontname);
	      }

	    switch(type)
	      {
              case EZ_WIDGET_TERM:
                twidget = EZ_TermITerm(widget);
                break;
	      case EZ_WIDGET_TREE: 
		twidget = EZ_TreeWITree(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		twidget = EZ_WorkAreaIWorkArea(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX: 
		twidget = EZ_FListBoxIFListBox(widget);
		break;
	      case EZ_WIDGET_TEXT: 
		twidget = EZ_TextIText(widget);
		break;
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		break;
	      case EZ_WIDGET_NOTE_BOOK:
		{
		  EZ_Widget *tframe = EZ_NoteBookTFrame(widget);
		  EZ_Widget *stub = EZ_WidgetChildren(tframe);
		  while(stub)
		    {
		      if(EZ_WidgetFont(stub) != tmpfont && 
			 EZ_WidgetFont(stub) == EZ_GetFontFromId(EZ_ITALIC_FONT))
			{
			  EZ_WidgetFont(stub) = tmpfont;
			  EZ_ClearWidgetSizeComputedFlag(stub);
			  if(boldfont) {EZ_ClearWidgetThinFontFlag(stub);}
			  else {EZ_SetWidgetThinFontFlag(stub);}
			  EZ_ClearWidgetSizeComputedFlag(tframe);
			  size_changed = 1;
			}
		      stub = EZ_WidgetSibling(stub);
		    }
		}
		break;
	      case EZ_WIDGET_NB_PAGE:
		{
		  EZ_Widget *companion = EZ_NBPageCompanion(widget);
		  if(EZ_WidgetFont(companion) != tmpfont)
		    {
		      EZ_WidgetFont(companion) = tmpfont;
		      EZ_ClearWidgetSizeComputedFlag(companion);
		      EZ_ClearWidgetSizeComputedFlag(EZ_WidgetParent(companion));
		      if(boldfont) {EZ_ClearWidgetThinFontFlag(companion);}
		      else {EZ_SetWidgetThinFontFlag(companion);}
		      size_changed = 1;
		    }
		}
		break;
	      case  EZ_WIDGET_OPTIONAL_ENTRY:
		{
		  EZ_Widget *entry = (EZ_Widget *)EZ_OptEntryEntry(widget);
		  EZ_Widget *listbox = (EZ_Widget *)EZ_OptEntryListbox(widget);
		  if(EZ_WidgetFont(entry) != tmpfont)
		    {
		      EZ_WidgetFont(entry) = tmpfont;
		      EZ_ClearWidgetSizeComputedFlag(entry);
		      if(boldfont) {EZ_ClearWidgetThinFontFlag(entry);}
		      else {EZ_SetWidgetThinFontFlag(entry);}
		    }
		  twidget =  EZ_ListBoxIListBox(listbox);
		}
		break;
	      case EZ_WIDGET_LED:
		if(EZ_WidgetFont(widget) != tmpfont)
		  {
		    EZ_LEDStatus(widget) |= LED_BLOCKED;
		    EZ_LEDDirty(widget) |= (LED_MASK_DIRTY | LED_PIXMAP_DIRTY);
		  }
		break;
	      default:
		break;
	      }
	    if(EZ_WidgetFont(twidget) != tmpfont)
	      {
		EZ_WidgetFont(twidget) = tmpfont;
		EZ_ClearWidgetSizeComputedFlag(twidget);
		if(boldfont) {EZ_ClearWidgetThinFontFlag(twidget);}
		else {EZ_SetWidgetThinFontFlag(twidget);}
		size_changed = 1;		    
	      }
	  }
	  break;
	case EZ_TEXT_LINE_LENGTH:
	  value = i1;
	  if(value > 0)
	    {
	      switch(type)
		{
		case EZ_WIDGET_LED:
		  if(EZ_LEDLineLength(widget) != value)
		    {
		      EZ_LEDLineLength(widget) = value;
		      size_changed = 1;
		    }
		  break;
		case EZ_WIDGET_LABEL:
		case EZ_WIDGET_ITOPLEVEL:
		case EZ_WIDGET_NW_LABEL:
		  if(EZ_LabelLineLength(widget) != value)
		    {
		      EZ_LabelLineLength(widget) = value;
		      size_changed = 1;
		    }
		  break;
		case EZ_WIDGET_NORMAL_BUTTON:
                case EZ_WIDGET_ARROW_BUTTON:
		case EZ_WIDGET_MENU_NORMAL_BUTTON:
		  if(EZ_ButtonLineLength(widget) != value)
		    {
		      EZ_ButtonLineLength(widget) = value;
		      size_changed = 1;
		    }
		  break;
		case EZ_WIDGET_CHECK_BUTTON:
		case EZ_WIDGET_MENU_CHECK_BUTTON:
		  if(EZ_CButtonLineLength(widget) != value)
		    {
		      EZ_CButtonLineLength(widget) = value;
		      size_changed = 1;
		    }
		  break;
		case EZ_WIDGET_RADIO_BUTTON:
		case EZ_WIDGET_MENU_RADIO_BUTTON:
		  if(EZ_RButtonLineLength(widget) != value)
		    {
		      EZ_RButtonLineLength(widget) = value;
		      size_changed = 1;
		    }
		  break;
		case EZ_WIDGET_MENU_BUTTON:
		  if(EZ_MButtonLineLength(widget) != value)
		    {
		      EZ_MButtonLineLength(widget) = value;
		      size_changed = 1;
		    }
		  break;
		case EZ_WIDGET_MENU_SUBMENU:
		  if(EZ_SubMenuLineLength(widget) != value)
		    {
		      EZ_SubMenuLineLength(widget) = value;
		      size_changed = 1;
		    }
		  break;
		case EZ_WIDGET_FRAME:
		  {
		    if(EZ_FrameLineLength(widget) != value)
		      {
			EZ_FrameLineLength(widget) = value;
			size_changed = 1;
		      }
		  }
		  break;
		case EZ_WIDGET_ENTRY:
		  if(EZ_EntryLineLength(widget) != value)
		    {
		      EZ_EntryLineLength(widget) = value;
		      size_changed = 1;
		    }
		  break;
		case  EZ_WIDGET_OPTIONAL_ENTRY:
		  {
		    EZ_Widget *entry = (EZ_Widget *)EZ_OptEntryEntry(widget);
		    if(EZ_EntryLineLength(entry) != value)
		      {
			EZ_EntryLineLength(entry) = value;
			size_changed = 1;
		      }
		  }
		  break;
		case EZ_WIDGET_NOTE_BOOK:
		  {
		    EZ_Widget *tframe = EZ_NoteBookTFrame(widget);
		    EZ_Widget *stub = EZ_WidgetChildren(tframe);
		    while(stub)
		      {
			if(EZ_ButtonLineLength(stub) != value &&
			   EZ_ButtonLineLength(stub) == EZ_DEFAULT_LABEL_LINE_LENGTH)
			  {
			    EZ_ButtonLineLength(stub) = value;
			    EZ_ClearWidgetSizeComputedFlag(stub);
			    EZ_ClearWidgetSizeComputedFlag(tframe);
			    size_changed = 1;
			  }
			stub = EZ_WidgetSibling(stub);
		      }
		  }
		  break;
		case EZ_WIDGET_NB_PAGE:		  
		  {
		    EZ_Widget *companion = EZ_NBPageCompanion(widget);
		    if(EZ_ButtonLineLength(companion) != value)
		      {
			EZ_ButtonLineLength(companion) = value;
			EZ_ClearWidgetSizeComputedFlag(companion);
			EZ_ClearWidgetSizeComputedFlag(EZ_WidgetParent(companion));
			size_changed = 1;
		      }
		  }
		  break;
		default:
		  EZ_ConfigureError(widget,option, "option not applicable");	  
		  break;
		}
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");	  
	  break;
	case EZ_SLIDER_DISPLAY_VALUE:
	  if(type == EZ_WIDGET_HORIZONTAL_SLIDER || type == EZ_WIDGET_VERTICAL_SLIDER )
	    {
              int value = i1, nnn = 0;
              if(value < 0) {nnn = 1; value = (value == -1)? 0: -value;}
	      if(EZ_SliderDisplayValue(widget) != value ||
                 EZ_SliderNoCValue(widget) != nnn)
                {
                  EZ_SliderDisplayValue(widget) = (unsigned char)value;
                  EZ_SliderNoCValue(widget) =  (unsigned char)nnn;
                  size_changed = 1;
                }
	    }
          else EZ_ConfigureError(widget,option, "option not applicable");	  
	  break;
	case EZ_SLIDER_LENGTH:
	  value = i1;
	  if(value > 0)
	    {
	      if(type == EZ_WIDGET_HORIZONTAL_SLIDER ||
		 type == EZ_WIDGET_VERTICAL_SLIDER )
		{
		      EZ_SliderSliderLength(widget) = value;
		      size_changed = 1;
		}
              else if(type == EZ_WIDGET_GRADIENT_BAR)
                {
                  EZ_GBarBWidthSet(widget) = value;
                  size_changed = 1;
                }
	      else EZ_ConfigureError(widget,option, "option not applicable");	  
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");	  
	  break;
	case EZ_SLIDER_WIDTH:
	  value = i1;
	  if(value > 0)
	    {
	      if(type == EZ_WIDGET_HORIZONTAL_SLIDER ||
		 type == EZ_WIDGET_VERTICAL_SLIDER )
		{
		      EZ_SliderWidth(widget) = value;
		      EZ_SliderWidthSet(widget) = 1;
		      size_changed = 1;
		}
              else   EZ_ConfigureError(widget,option, "option not applicable");	  
            }
          else EZ_ConfigureError(widget,option, "Value Out of range");	  
	  break;
	case EZ_SLIDER_BORDER_WIDTH:
	  value = i1;
	  if(value > 0)
	    {
	      if(type == EZ_WIDGET_HORIZONTAL_SLIDER ||
		 type == EZ_WIDGET_VERTICAL_SLIDER )
		{
		      EZ_SliderBorderWidth(widget) = value;
		      size_changed = 1;
		}
	      else  EZ_ConfigureError(widget,option, "option not applicable");	  
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");	  
	  break;
	case EZ_SLIDER_RESOLUTION: 
	  {
	    float fvalue = (float)f1;
	    if(fvalue > 0.0)
	      {
		if(type == EZ_WIDGET_HORIZONTAL_SLIDER ||
		   type == EZ_WIDGET_VERTICAL_SLIDER )
		  {
		    EZ_SliderResolution(widget) = fvalue;
		    size_changed = 1;
		  }
		else EZ_ConfigureError(widget,option, "option not applicable");
	      }
	    else EZ_ConfigureError(widget,option, "Value Out of range");
	  }
	  break;
	case EZ_SLIDER_RANGE: 
	  {
	    float from, to;
	    from = f1;
	    to =  f2;
            if(from == to) to += 1.0;
	    if(type == EZ_WIDGET_HORIZONTAL_SLIDER ||
	       type == EZ_WIDGET_VERTICAL_SLIDER )
	      {
		EZ_SliderMinValue(widget) = (float)from;
		EZ_SliderMaxValue(widget) = (float)to;
		size_changed = 1;
	      }
            else if(type == EZ_WIDGET_THUMB_WHEEL)
              {
                EZ_ThumFrom(widget) = (float)from;
                EZ_ThumTo(widget) = (float) to;
              }
	    else EZ_ConfigureError(widget,option, "option not applicable");
	  }
	  break;
	case EZ_INDICATOR_SIZE_ADJUST:
	  value = i1;
	  if(value >= -4 )
	    {
	      switch(type)
		{
		case EZ_WIDGET_CHECK_BUTTON:  case EZ_WIDGET_MENU_CHECK_BUTTON:
		  if(EZ_CButtonCheckSizeAdjust(widget) != value)
		    {
		      EZ_CButtonCheckSizeAdjust(widget) = value;
		      size_changed = 1;
		    }
		  break;
		case EZ_WIDGET_RADIO_BUTTON: case EZ_WIDGET_MENU_RADIO_BUTTON:
		  if(EZ_RButtonCheckSizeAdjust(widget) != value)
		    {
		      EZ_RButtonCheckSizeAdjust(widget) = value;
		      size_changed = 1;
		    }
		  break;
		case EZ_WIDGET_MENU_BUTTON:
		  if(EZ_MButtonCheckSizeAdjust(widget) != value)
		    {
		      EZ_MButtonCheckSizeAdjust(widget) = value;
		      size_changed = 1;
		    }
		  break;
		default:
		  EZ_ConfigureError(widget,option, "option not applicable");	  
		  break;
		}
	    }
	  else EZ_ConfigureError(widget,option, "Value Out of range");	  
	  break; 
	case EZ_BUTTON_SHORTCUT:
	  {
	    char *str = (char *)p1;
	    EZ_SetKeyShortcut(widget, str);
	    size_changed = 1;
	  }
	  break;
	case EZ_CALL_BACK:
	  {
	    EZ_CallBack tmpfunc = (EZ_CallBack)p1;
	    void *data = p2;
	    EZ_RemoveAllWidgetCallBacks(widget, EZ_CALLBACK);
	    EZ_AddWidgetCallBack(widget,EZ_CALLBACK, tmpfunc, data, 0);
	  }
	break;
	case EZ_MOTION_CALLBACK:
	  {
	    EZ_CallBack tmpfunc = (EZ_CallBack)p1;
	    void *data = p2;
	    EZ_RemoveAllWidgetCallBacks(widget, EZ_MOTION_CALLBACK);
	    EZ_AddWidgetCallBack(widget,EZ_MOTION_CALLBACK, tmpfunc,data, 0);
	  }
	  break;
	case EZ_EVENT_HANDLE:
	  {
	    EZ_EventHandler tmpfunc = (EZ_EventHandler)p1;
	    void *data = p2;
	    EZ_RemoveAllEventHandlers(widget);
	    EZ_AddEventHandler(widget, tmpfunc, data, 0);
	  }
	break;
	case EZ_DESTROY_CALLBACK:
	  {
	    EZ_CallBack tmpfunc = (EZ_CallBack)p1;
	    void *data = p2;
	    EZ_RemoveAllWidgetCallBacks(widget, EZ_DESTROY_CALLBACK);
	    EZ_AddWidgetCallBack(widget,EZ_DESTROY_CALLBACK, tmpfunc, data, 0);
	  }
	break;
	case EZ_BUBBLE_HELP:
	  {
	    char *ostr, *str = (char *)p1;
	    int olen, len = str? strlen(str): 0;
	    
	    ostr = EZ_WidgetBubbleString(widget);
	    if(len == 0) /* disable bubble help */
	      {
		if(ostr) (void)my_free(ostr);
		EZ_WidgetBubbleString(widget) = (char *)NULL;
	      }
	    else
	      {
		if(ostr)
		  {
		    olen = strlen(ostr);
		    if(olen < len)
		      ostr = EZ_WidgetBubbleString(widget) = (char *)my_realloc(ostr, len, _BUBBLE_STRING_);
		    if(!ostr) EZ_OutOfMemory("EZ_ConfigureWidget:BubbelString");
		    (void)strcpy(ostr, str);
		  }
		else
		  EZ_WidgetBubbleString(widget) = EZ_AllocCopyString(str);
		size_changed = 1;
	      }
	  }
	  break;
	case EZ_MOVABLE:
	  value = i1;
	  switch(type)
	    {
	    case EZ_WIDGET_PANE_HANDLE:
              EZ_PaneHandleMovable(widget) = (value != 0);
              break;
	    case EZ_WIDGET_ITOPLEVEL:
	      EZ_LabelMovable(widget) = (value == 0? 0: 1);
	      break;
	    case EZ_WIDGET_WORK_AREA:
	      {
		EZ_Widget *tw = EZ_WorkAreaIWorkArea(widget);
		EZ_IWorkAreaItemMovable(tw) = (value == 0? 0: 1);
	      }
	    break;
	    case EZ_WIDGET_IWORK_AREA:
	      {
		EZ_IWorkAreaItemMovable(widget) = (value == 0? 0: 1);
	      }
	    break;
	    default:
	      EZ_ConfigureError(widget,option, "option not applicable"); 
	    }
	  break;	  
	case EZ_LABEL_STRING:
	  {
	    char *str = (char *)p1;
	    size_changed = EZ_SetWidgetLabelString(widget,str);
	  }
	  break;
	case EZ_NAME:
	  {
	    char *str = (char *)p1;
	    if(str && *str) 
	      EZ_WidgetIName(widget) = EZ_StringToQuark(str);
	  }
	  break;
	case EZ_POPUP_NAME:
	  if(type == EZ_WIDGET_OPTIONAL_ENTRY)
	    {
	      EZ_Widget *tmp = EZ_OptEntryPopup(widget);
	      char *str = (char *)p1;
	      if(str && *str)
		EZ_WidgetIName(tmp) = EZ_StringToQuark(str);
	    }
	  else if(type == EZ_WIDGET_FILE_SELECTOR)
	    {
	      char *ptr = (char *)p1;
	      if(ptr && *ptr)
		{
		  char str[256];
		  int len = 0;
		  EZ_Widget *ee,*pp;
		  while(*ptr) str[len++] = *ptr++;
		  str[len] = str[len+1] = 0;
		  ee = EZ_FileSelectorPatternEntry(widget);
		  pp = EZ_OptEntryPopup(ee);
		  str[len]='A';
		  EZ_WidgetIName(pp) = EZ_StringToQuark(str);
		  ee = EZ_FileSelectorSelectionEntry(widget);
		  pp = EZ_OptEntryPopup(ee);
		  str[len]='B';
		  EZ_WidgetIName(pp) = EZ_StringToQuark(str);
		}
	    }
	  break;
	case EZ_CLASS:
	  {
	    char *str = (char *)p1;
	    if(str && *str)
	      EZ_WidgetCName(widget) = EZ_StringToQuark(str);
	  }
	  break;
	case EZ_RESERVE_MENU_BUTTON:
	  value = i1;
	  if(type == EZ_WIDGET_3D_CANVAS)
	    EZ_3DCanvasReleaseMenuButton(widget) = (value == 0? 1: 0);
	  else
	    EZ_ConfigureError(widget,option, "option not applicable"); 
	  break;
	case EZ_BACKING_STORE:
	  value = i1;
	  if(type == EZ_WIDGET_3D_CANVAS)
	    EZ_3DCanvasBackingStore(widget) = (value == 0? 0:1);
	  else
	    EZ_ConfigureError(widget,option, "option not applicable"); 
	  break;
	case EZ_EXPAND:
	  value = i1;
	  if(value == 0) {EZ_ClearWidgetExpandFlag(widget);}
	  else   EZ_SetWidgetExpandFlag(widget);
	  break;
	case EZ_TRANSIENT:
	  value = i1;
	  if(value == 0) { EZ_ClearWidgetTransientFlag(widget);}
	  else  {EZ_SetWidgetTransientFlag(widget);}
	  break;
	case EZ_SCROLLBAR_WIDTH:
	  value = i1;
	  {
	    EZ_Widget *hwidget = (EZ_Widget *)NULL;
	    EZ_Widget *vwidget = (EZ_Widget *)NULL;

	    switch( type )
	      {
	      case EZ_WIDGET_TREE: 
		hwidget = EZ_TreeWHScroll(widget);
		vwidget = EZ_TreeWVScroll(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		hwidget = EZ_WorkAreaHScroll(widget);
		vwidget = EZ_WorkAreaVScroll(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		hwidget = EZ_FListBoxHScroll(widget);
		vwidget = EZ_FListBoxVScroll(widget);
		break;
	      case EZ_WIDGET_LIST_BOX:
		hwidget = EZ_ListBoxHScroll(widget);
		vwidget = EZ_ListBoxVScroll(widget);
		break;
	      case EZ_WIDGET_TEXT:
		hwidget = EZ_TextHScroll(widget);
		vwidget = EZ_TextVScroll(widget);
		break;
	      case EZ_WIDGET_HORIZONTAL_SCROLLBAR:
		hwidget = widget;
		break;
	      case EZ_WIDGET_VERTICAL_SCROLLBAR:
		vwidget = widget;
		break;
              case EZ_WIDGET_TERM:
		vwidget = EZ_TermVScroll(widget);
                break;
	      case  EZ_WIDGET_OPTIONAL_ENTRY:
		{
		  EZ_Widget *listbox = EZ_OptEntryListbox(widget);
		  hwidget = EZ_ListBoxHScroll(listbox);
		  vwidget = EZ_ListBoxVScroll(listbox);
		}
		break;
	      default:
		break;
	      }
	    if(hwidget) 
	      {
		if(EZ_ScrollSSize(hwidget) != value)
		  {
		    EZ_ScrollSSize(hwidget) = value;
		    EZ_ClearWidgetSizeComputedFlag(hwidget);  
		    size_changed = 1;
		  }
	      }
	    if(vwidget) 
	      {
		if(EZ_ScrollSSize(vwidget) != value)
		  {
		    EZ_ScrollSSize(vwidget) = value;
		    EZ_ClearWidgetSizeComputedFlag(vwidget);  
		    size_changed = 1;
		  }
	      }
	  }
	  break;
	case EZ_SCROLLBAR_BORDER_WIDTH:
	  value = i1;
	  if(value >= 0 && value <= 100)
	    {
	      EZ_Widget *hwidget = (EZ_Widget *)NULL;
	      EZ_Widget *vwidget = (EZ_Widget *)NULL;

	      switch( type )
		{
                case EZ_WIDGET_TERM:
                  vwidget = EZ_TermVScroll(widget);
                  break;
		case EZ_WIDGET_TREE: 
		  hwidget = EZ_TreeWHScroll(widget);
		  vwidget = EZ_TreeWVScroll(widget);
		  break;
		case EZ_WIDGET_WORK_AREA:
		  hwidget = EZ_WorkAreaHScroll(widget);
		  vwidget = EZ_WorkAreaVScroll(widget);
		  break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  hwidget = EZ_FListBoxHScroll(widget);
		  vwidget = EZ_FListBoxVScroll(widget);
		  break;
		case EZ_WIDGET_LIST_BOX:
		  hwidget = EZ_ListBoxHScroll(widget);
		  vwidget = EZ_ListBoxVScroll(widget);
		  break;
		case EZ_WIDGET_TEXT:
		  hwidget = EZ_TextHScroll(widget);
		  vwidget = EZ_TextVScroll(widget);
		  break;
		case EZ_WIDGET_HORIZONTAL_SCROLLBAR:
		  hwidget = widget;
		  break;
		case EZ_WIDGET_VERTICAL_SCROLLBAR:
		  vwidget = widget;
		  break;
		case  EZ_WIDGET_OPTIONAL_ENTRY:
		  {
		    EZ_Widget *listbox = EZ_OptEntryListbox(widget);
		    hwidget = EZ_ListBoxHScroll(listbox);
		    vwidget = EZ_ListBoxVScroll(listbox);
		  }
		  break;
		case  EZ_WIDGET_SCROLL_BUTTON:
                  EZ_ScrollButtonBorderWidth(widget) = value;
                  break;
		default:
		  break;
		}
	      if(hwidget) 
		{
		  if(EZ_WidgetBorderWidth(hwidget) != value)
		    {
		      EZ_WidgetBorderWidth(hwidget) = value;
		      EZ_ScrollBorderWidth(hwidget) = value; /* 6.3.96 */
		      EZ_ClearWidgetSizeComputedFlag(hwidget);  
		      size_changed = 1;
		    }
		}
	      if(vwidget) 
		{
		  if(EZ_WidgetBorderWidth(vwidget) != value)
		    {
		      EZ_WidgetBorderWidth(vwidget) = value;
		      EZ_ScrollBorderWidth(vwidget) = value; /* 6.3.96 */
		      EZ_ClearWidgetSizeComputedFlag(vwidget);  
		      size_changed = 1;
		    }
		}
	    }
	  break;
	case EZ_PROPAGATE:
	  value = i1;
	  if(value==0) { EZ_SetDoNotPropagateFlag(widget);}
	  else  {EZ_ClearDoNotPropagateFlag(widget);}
	  break;
	case EZ_WMHINTS: /* inherited from previous versions: sizeHints */
	case EZ_WM_WINDOW_NAME:
	case EZ_WM_ICON_NAME:
	case EZ_WM_ICON_XPM_FILE:
	case EZ_WM_ICON_PIXMAP:
	case EZ_WM_ICON_LOCATION:
	case EZ_WM_LOCATION_HINT:
	case EZ_WM_SIZE_HINT:
	case EZ_WM_MAX_SIZE_HINT:
	case EZ_WM_MIN_SIZE_HINT:
	case EZ_WM_INITIAL_STATE:
	  EZ_SetWidgetWMHints(widget,option, i1, i2, p1 );
	  break;
	case EZ_MARK_CLEAN:
	  if(i1 > 0) size_changed = -(1<<30);
	  else if(i1 < 0) size_changed = -(1<<29);
          else { size_changed = 1; EZ_MarkWidgetDirty(widget);}
	  break;
	case EZ_HIGHLIGHT_MODE:	  
	  {
	    i1 &= 0xff;
	    switch(type)
	      {
              case EZ_WIDGET_SPREAD_SHEET:
                EZ_SSheetHMode(widget) = i1;
                break;
	      case EZ_WIDGET_ITREE:
		EZ_ITreeWHighlightMode(widget) = i1;
		break;
	      case EZ_WIDGET_TREE:
		EZ_ITreeWHighlightMode(EZ_TreeWITree(widget)) = i1;
		break;
	      case EZ_WIDGET_IFANCY_LIST_BOX:
		EZ_IFListBoxHighlightMode(widget) = i1;
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		EZ_IFListBoxHighlightMode(EZ_FListBoxIFListBox(widget)) = i1;	       
		break;
	      case EZ_WIDGET_IWORK_AREA:
		EZ_IWorkAreaHighlightMode(widget) = i1;
		break;
	      case EZ_WIDGET_WORK_AREA:
		EZ_IWorkAreaHighlightMode(EZ_WorkAreaIWorkArea(widget)) = i1;
		break;
	      default:
		break;
	      }
	  }
	break;
	case EZ_RUBBER_BAND:
	  {
	    i1 &= 0xff;
	    switch(type)
	      {
	      case EZ_WIDGET_ITREE:
		EZ_ITreeWRubberBand(widget) = i1;
		break;
	      case EZ_WIDGET_TREE:
		EZ_ITreeWRubberBand(EZ_TreeWITree(widget)) = i1;
		break;
	      case EZ_WIDGET_IFANCY_LIST_BOX:
		EZ_IFListBoxRubberBand(widget) = i1;
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		EZ_IFListBoxRubberBand(EZ_FListBoxIFListBox(widget)) = i1;	       
		break;
	      case EZ_WIDGET_IWORK_AREA:
		EZ_IWorkAreaRubberBand(widget) = i1;
		break;
	      case EZ_WIDGET_WORK_AREA:
		EZ_IWorkAreaRubberBand(EZ_WorkAreaIWorkArea(widget)) = i1;
		break;
	      default:
		break;
	      }	  
	  }
	break;
	case 0:
	  break;
	default:
          EZ_ConfigureError(widget,option, "Unknown configuration option.");
	  break;
	}
    }
  size_changed = size_changed | shape_changed;
  if(EZ_WidgetWindow(widget) != None)
    { 
      if(size_changed & (1 << 20)) EZ_SetWindowShape(widget, 1); /* remove shape */
      if(size_changed & (1 << 21)) EZ_SetWindowShape(widget, 0); /* set shape */
    }
  return(size_changed);
}

/*****************************************************************
 *
 *  Print an error message or warning message for invalid
 *  configure options.
 */
void  EZ_ConfigureError(widget,option, message)
     EZ_Widget *widget;
     int             option;
     char            *message;
{
  int  type;
  char *name;
  char *option_name;
  extern int silenceOnConfigError;
  
  if(silenceOnConfigError) return;
  type = EZ_WidgetType(widget);
  name = EZ_WidgetDefaultClassNames[type];
  if(option > 0 && option <= EZ_WIDGET_CONFIGURE_LAST_OPTION)
    option_name = EZ_ConfigureOptionNames[option];
  else
    option_name = (char *)NULL;
  
  (void)fprintf(stderr,"Warning: In function EZ_ConfigureWidget, '%s'.\n\t Widget type is '%s'[%d], configure option is '%s'[%d]\n",
		message,name, type, option_name, option);
}
/***************************************************************************
 * 
 *   Set the background bitmap/pixmap if applicable.
 */

int  EZ_SetWidgetPixmap(widget,bitmap,option)
     EZ_Widget *widget;
     EZ_Bitmap       *bitmap;
     int             option;  /* EZ_bitmap or EZ_image */
{
  EZ_Widget *twidget = widget;
  EZ_Bitmap *oldbmp;      
  int shapeChanged = 0;
  int alter = 0;
  switch(EZ_WidgetType(widget))
    {
    case EZ_WIDGET_NB_PAGE:
      twidget = EZ_NBPageCompanion(widget);
     break;
    case EZ_WIDGET_TEXT:
      alter = 1;
      twidget = EZ_TextIText(widget);
      break;
    case EZ_WIDGET_TERM:
      alter = 1;
      twidget = EZ_TermITerm(widget);
      break;
    case EZ_WIDGET_LIST_BOX:
      alter = 1;
      twidget = EZ_ListBoxIListBox(widget);
      break;
    case  EZ_WIDGET_TREE:
      twidget = EZ_TreeWITree(widget);
      alter = 1;
      break;
    case EZ_WIDGET_FANCY_LIST_BOX:
      twidget = EZ_FListBoxIFListBox(widget);
      alter = 1;
      break;
    case EZ_WIDGET_WORK_AREA:
      twidget = EZ_WorkAreaIWorkArea(widget);
      alter = 1;
      break;
    case EZ_WIDGET_ITERM:
    case EZ_WIDGET_ITEXT:
    case EZ_WIDGET_IWORK_AREA:
    case EZ_WIDGET_ITREE:
    case EZ_WIDGET_IFANCY_LIST_BOX:
    case EZ_WIDGET_ILIST_BOX:
      alter = 1;
      break;
    default:
      break;
    }
  if(alter) /* for ITree, IWorkarea and IFListbox */
    {
      oldbmp = EZ_WidgetBGPixmap(twidget);
      if(oldbmp == NULL && bitmap == NULL) return(0);
      EZ_FreeBitmap(oldbmp);
      if(bitmap && EZ_BitmapType(bitmap) == EZ_BITMAP_IS_BITMAP)
	{ EZ_FreeBitmap(bitmap); bitmap = NULL;}
      bitmap = EZ_CheckBGPixmap(bitmap);
      EZ_WidgetBGPixmap(twidget) = bitmap;
    }
  else
    {
      oldbmp = EZ_WidgetPixmap(twidget);
      if(oldbmp == NULL && bitmap == NULL) return(0);
      EZ_FreeBitmap(oldbmp);
      EZ_WidgetPixmap(twidget) = bitmap;
      /* if widget has a shape mask, update it. */
      if(EZ_GetWidgetShapedWinFlag(twidget) && EZ_WidgetShapeMask(twidget) == NULL)
	{
	  /* labeled icon is treated separately */
	  if(EZ_WidgetType(twidget) == EZ_WIDGET_ITOPLEVEL)
	    {  EZ_LabelMaskDirty(twidget) = 1;}
	  if(EZ_WidgetWindow(twidget) != None)
	    {
	      if(bitmap && bitmap->shape) shapeChanged = (1<<21); /* set shape */
	      else shapeChanged = (1<<20);  /* remove shape */
	    }
	}
    }
  return(1 | shapeChanged);
}
/*********************************************************************************/
void EZ_SetKeyShortcut(widget, str) EZ_Widget *widget; char *str;
{
  int fail = 0;
  if(str && *str)
    {
      switch(EZ_WidgetType(widget))
	{
	case EZ_WIDGET_MENU_NORMAL_BUTTON: 
	case EZ_WIDGET_NORMAL_BUTTON:
        case EZ_WIDGET_ARROW_BUTTON:
	  EZ_ButtonLabel2(widget) = EZ_AllocCopyString(str);
	  EZ_ButtonLabel2Length(widget) = strlen(str);
	  fail = EZ_ScanShortcut(widget, EZ_ButtonLabel2(widget),
                             &(EZ_ButtonShortcut(widget)), &(EZ_ButtonModifiers(widget)));
          if(fail) EZ_ButtonLabel2Length(widget) = 0;
	  break;
	case EZ_WIDGET_MENU_CHECK_BUTTON:
	case EZ_WIDGET_CHECK_BUTTON:
	  EZ_CButtonLabel2(widget) = EZ_AllocCopyString(str);
	  EZ_CButtonLabel2Length(widget) = strlen(str);
	  fail=EZ_ScanShortcut(widget, EZ_CButtonLabel2(widget),
			  &(EZ_CButtonShortcut(widget)), &(EZ_CButtonModifiers(widget)));
          if(fail) EZ_CButtonLabel2Length(widget) = 0;
	  break;
	case EZ_WIDGET_MENU_RADIO_BUTTON:
	case EZ_WIDGET_RADIO_BUTTON:
	  EZ_RButtonLabel2(widget) = EZ_AllocCopyString(str);
	  EZ_RButtonLabel2Length(widget) = strlen(str);
	  fail=EZ_ScanShortcut(widget, EZ_RButtonLabel2(widget),
			  &(EZ_RButtonShortcut(widget)), &(EZ_RButtonModifiers(widget)));
          if(fail) EZ_RButtonLabel2Length(widget) = 0;
	  break;
	case EZ_WIDGET_MENU_BUTTON:
	  EZ_MButtonLabel2(widget) = EZ_AllocCopyString(str);
	  EZ_MButtonLabel2Length(widget) = strlen(str);
	  fail=EZ_ScanShortcut(widget, EZ_MButtonLabel2(widget),
			  &(EZ_MButtonShortcut(widget)), &(EZ_MButtonModifiers(widget)));
          if(fail) EZ_MButtonLabel2Length(widget) = 0;
	  EZ_MButtonUnderLine(widget) = -1;
	  break;		
	case EZ_WIDGET_MENU_SUBMENU:
	  EZ_SubMenuLabel2(widget) = EZ_AllocCopyString(str);
	  EZ_SubMenuLabel2Length(widget) = strlen(str);
	  fail=EZ_ScanShortcut(widget, EZ_SubMenuLabel2(widget),
			  &(EZ_SubMenuShortcut(widget)), &(EZ_SubMenuModifiers(widget)));
          if(fail) EZ_SubMenuLabel2Length(widget) = 0;
	  EZ_SubMenuUnderLine(widget) = -1;
	  break;		
	default:
	  EZ_ConfigureError(widget, EZ_BUTTON_SHORTCUT, "option not applicable"); 
	  break;
	}
    }
  else fail=1;
  if(fail) EZ_RemoveAllKeyShortcutForWidget(widget);
}
/***********************************************************************************
 *
 *  Given a string, figure out the exact shortcut keystroke.
 *   Accepted combinations are:
 *
 *    Control-letter, ctrl-letter, Meta-letter
 *    C-letter,    M-letter
 * or <anyone of the above>
 *
 * Note: str must not be a tmp string or ...
 */
static int EZ_ScanShortcut(widget,str, shortcut, modifier)
     EZ_Widget *widget;
     char            *str, **shortcut;
     int             *modifier;
{
  char  *tmp = str;
  int   fail = 0;
  int   mode = 0;

  /*---------------------
   * skip spaces or '<'
   *--------------------*/
  while( *tmp != '\0' && (*tmp == ' ' || *tmp == '\t' || *tmp == '<'))  tmp++;
  /*---------------------
   * get the modifier 
   *--------------------*/
  if(*tmp == 'C' || *tmp == 'c' || *tmp == '^')
    mode = ControlMask;
  else  if(*tmp == 'M' || *tmp == 'm')  
    mode =  Mod1Mask;
  else fail = 1;

  if(!fail)
    {
      while( *tmp != '-' && *tmp != '\0')
	tmp++;
      if(*tmp != '\0' && isprint(*(tmp+1)))
	{
	  *modifier = mode;
	  *shortcut = (tmp+1);
	  /*-------------------------------------------------
	   * register it in the global KeyPress Event table
	   *------------------------------------------------*/
	  EZ_InsertGlobalKeyPressEvent(widget, mode, tmp+1);
	}
      else fail = 1;
    }
  if(fail)
    (void)fprintf(stderr,"Invalid keybord shortcut: %s\n",str);
  return(fail);
}
/***********************************************************************************/
void EZ_MarkWidgetDirty(widget) EZ_Widget *widget;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_ClearWidgetSizeComputedFlag(widget);  
      switch(type)
        {
        case EZ_WIDGET_ITREE:
          EZ_ITreeWDirty(widget) = 1;
          break;
        case EZ_WIDGET_TREE:
          EZ_ITreeWDirty(EZ_TreeWITree(widget)) = 1;
          break;
        case EZ_WIDGET_IFANCY_LIST_BOX:
          EZ_IFListBoxDirty(widget) = 1;
          break;
        case EZ_WIDGET_FANCY_LIST_BOX:
          EZ_IFListBoxDirty(EZ_FListBoxIFListBox(widget)) = 1;	       
          break;
        case EZ_WIDGET_IWORK_AREA:
          EZ_IWorkAreaDirty(widget) = 1;
          break;
        case EZ_WIDGET_WORK_AREA:
          EZ_IWorkAreaDirty(EZ_WorkAreaIWorkArea(widget)) = 1;
          break;
        default:
          break;
        }
    }
}

/***********************************************************************************/
static char *getPrimaryColor(pv) unsigned long pv;
{
  static char str[120];
  static int  ii = 0;
  int r,g,b;
  char *ptr;
  EZ_PixelValue2RGB(pv, &r,&g,&b);
  ii=(ii > 100)? 0 : ii;
  ptr = str+ii; ii += 20;
  (void)sprintf(ptr, "\"#%02x%02x%02x\"", r, g, b);
  return(ptr);
}
static char *getSecondaryColor(pv) unsigned long pv;
{
  if(pv == ~0 || pv == 0 ) return("none");
  else return(getPrimaryColor(pv));
}
/***********************************************************************************
 *
 *
 */
int  EZ_GetWidgetAttribute(widget, option, oparam, vret)
     EZ_Widget *widget;
     int option, oparam;
     EZ_Value *vret;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      switch(option)
	{
	case EZ_SHEET_HEADER_SIZE:
	  if(type == EZ_WIDGET_SPREAD_SHEET)
	    {
	      vret[0].type = EZ_INT;
	      vret[1].type = EZ_INT;
	      vret[0].value.i = EZ_SSheetTWidth(widget);
	      vret[1].value.i = EZ_SSheetTHeight(widget);
	      return(2);
	    }
	  break;
	case EZ_SHEET_CELL_SIZE:
	  if(type == EZ_WIDGET_SPREAD_SHEET)
	    {
	      vret[0].type = EZ_INT;
	      vret[1].type = EZ_INT;
	      vret[0].value.i = EZ_SSheetCellWidth(widget);
	      vret[1].value.i = EZ_SSheetCellHeight(widget);
	      return(2);
	    }
	  break;
        case EZ_RESOURCE_STRING:
          vret[0].type= EZ_STRING;
          vret[0].value.str = EZ_WidgetResourceString(widget);
          return(1);          
          break;
        case EZ_SEPARATOR_STYLE:
          if(type == EZ_WIDGET_SEPARATOR)
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_SeparatorStyle(widget);
              return(1);
            }
          break;              
        case  EZ_BAR_ORIENTATION:
          if(type == EZ_WIDGET_GRADIENT_BAR)
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_GBarOrientation(widget);
              return(1);
            }
          else if(type == EZ_WIDGET_HISTOGRAM)
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_HgramOrientation(widget);
              return(1);
            }
          else if(type == EZ_WIDGET_FRAME)
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_FrameBarOrientation(widget);
              return(1);
            }
          break;          
        case EZ_BAR_EDITABLE:
          if(type == EZ_WIDGET_GRADIENT_BAR)
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_GBarEditable(widget);
              return(1);
            }
          break;
        case EZ_BAR_COLOR_N:
          if(type == EZ_WIDGET_GRADIENT_BAR)
            {
              unsigned long *clrs = EZ_GBarColors(widget);
              vret[0].type= EZ_STRING;
              vret[0].value.str = getPrimaryColor(clrs[oparam]);
              return(1);        
            }
          else if(type == EZ_WIDGET_HISTOGRAM)
            {
              unsigned long *clrs = EZ_HgramColors(widget);
              vret[0].type= EZ_STRING;
              vret[0].value.str = getPrimaryColor(clrs[oparam]);
              return(1);        
            }
          break;
        case EZ_HISTOGRAM_SAMPLE:
          if(type == EZ_WIDGET_HISTOGRAM)
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_HgramNBins(widget);
              return(1); 
            }
          break;
        case EZ_BAR_RANGE:
          if(type == EZ_WIDGET_GRADIENT_BAR)
            {
              vret[0].type = vret[1].type = EZ_FLOAT;
              vret[0].value.f = EZ_GBarRange(widget)[0];
              vret[1].value.f = EZ_GBarRange(widget)[1];
              return(2);

            }
          else if(type == EZ_WIDGET_HISTOGRAM)
            {
              vret[0].type = vret[1].type = EZ_FLOAT;
              vret[0].value.f = EZ_HgramRange(widget)[0];
              vret[1].value.f = EZ_HgramRange(widget)[1];
              return(2);
            }
          break;
        case EZ_BAR_WIDTH:
          if(type == EZ_WIDGET_GRADIENT_BAR)
            {
              vret[0].type = EZ_INT;
              vret[0].value.i =EZ_GBarBWidthSet(widget);
              return(1);
            }
          else if(type == EZ_WIDGET_HISTOGRAM)
            {
              vret[0].type = EZ_INT;
              vret[0].value.i =EZ_HgramBinWidth(widget);
              return(1);
            }
          else if(type == EZ_WIDGET_SEPARATOR)
            {
              vret[0].type = EZ_INT;
              vret[0].value.i =EZ_SeparatorWidth(widget);
              return(1);
            }
          break;
        case EZ_WRAP_TEXT:
          {
	    EZ_Widget *iText = NULL;
	    if(type == EZ_WIDGET_TEXT) iText = EZ_TextIText(widget);
	    else if(type == EZ_WIDGET_ITEXT) iText = widget;
	    if(iText)
              {
                vret[0].type = EZ_INT;
                vret[0].value.i = (int)(EZ_ITextWrapText(iText));
                return(1);
              }
          }
         break;
        case EZ_DIAL_COLOR:
          if(type == EZ_WIDGET_DIAL)
            {
              vret[0].type= EZ_STRING;
              vret[0].value.str = getSecondaryColor(EZ_DialForeground(widget));
              return(1);
            }
          break;
        case EZ_DIAL_VALUE:
          if(type == EZ_WIDGET_DIAL)
            {
              vret[0].type= EZ_FLOAT;
              vret[0].value.f = EZ_DialValue(widget);
              return(1);
            }
          break;
        case EZ_DIAL_STYLE:
          if(type == EZ_WIDGET_DIAL)
            {          
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_DialStyle(widget);
              return(1);
            }
          break;
        case EZ_DIAL_DISPLAY_VALUE:
          if(type == EZ_WIDGET_DIAL)
            {
              int ii = EZ_DialShowValue(widget);
              int ss = EZ_DialNoCValue(widget);
              vret[0].type= EZ_INT;
              if(ss) { vret[0].value.i = (ii > 1)? -ii: -1;}
              else  vret[0].value.i=ii;
              return(1);
            }
          break;
        case EZ_DIAL_SPAN:
          if(type == EZ_WIDGET_DIAL)
            {
              vret[0].type= EZ_FLOAT;
              vret[0].value.f = EZ_DialSpan(widget);
              return(1);
            }
          break;
        case EZ_DIAL_RANGE:
          if(type == EZ_WIDGET_DIAL)
            {
              vret[0].type= EZ_FLOAT;
              vret[1].type= EZ_FLOAT;
              vret[0].value.f = EZ_DialFrom(widget);
              vret[1].value.f = EZ_DialTo(widget);
              return(2);
            }
          break;
        case EZ_DIAL_RESOLUTION:
          if(type == EZ_WIDGET_DIAL)
            {
              vret[0].type= EZ_FLOAT;
              vret[0].value.f = EZ_DialResolution(widget);
              return(1);
            }
          break;
        case EZ_SHOW_TICK:
          {
            vret[0].type= EZ_INT;
            vret[1].type= EZ_INT;
            if((type == EZ_WIDGET_HORIZONTAL_SLIDER || type == EZ_WIDGET_VERTICAL_SLIDER))
              {
                vret[0].value.i = EZ_SliderNTicks(widget);
                vret[1].value.i = EZ_SliderTickSize(widget);
                return(2);
              }
            else if(type == EZ_WIDGET_DIAL)
              {
                vret[0].value.i=EZ_DialNTicks(widget);
                vret[1].value.i= EZ_DialTickSize(widget) ;
                return(2);
              }
            else if(type == EZ_WIDGET_SPREAD_SHEET)
              {
                vret[0].value.i= EZ_SSheetTabChar(widget);
                vret[1].value.i= EZ_SSheetNewlineChar(widget);
              }
          }
          break;
        case EZ_SPECIAL_EFFECT:
          {
            vret[0].type= EZ_INT;
            vret[1].type= EZ_INT;
            vret[0].value.i=EZ_WidgetSECode(widget);
            vret[1].value.i=EZ_WidgetSEParam(widget);
            return(2);
          }
          break;
        case EZ_LOCATOR_XY:
          if(type == EZ_WIDGET_LOCATOR)
            {
              vret[0].type= EZ_FLOAT;
              vret[1].type= EZ_FLOAT;
              vret[0].value.f = EZ_LocatorXX(widget);
              vret[1].value.f = EZ_LocatorYY(widget);
              return(2);
            }
          break;
        case EZ_SPIN_VALUE:
          if(type == EZ_WIDGET_SPIN_BUTTON)
            {
              EZ_Widget *entry = EZ_SpinButtonEntry(widget);
              char *ptr = EZ_GetEntryString(entry);
              vret[0].type= EZ_STRING;
              vret[0].value.str = ptr;
              return(1);
            }
          break;
        case EZ_SPIN_FUNCTION:
          if(type == EZ_WIDGET_SPIN_BUTTON)
            {
              vret[0].type= EZ_FUNCTION;
              vret[1].type= EZ_POINTER;
              vret[0].value.p = (void *) EZ_SpinButtonNextF(widget);
              vret[1].value.p = EZ_SpinButtonCData(widget);
              return(2);
            }
          break;
        case EZ_SCROLL_TYPE:
          if(type == EZ_WIDGET_SCROLL_BUTTON)
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_ScrollButtonType(widget);
            }
          break;
        case EZ_RESOURCES_HANDLE:
          {
            EZ_ResourceHandle *handle=EZ_WidgetResourceHandle(widget);
            vret[0].type= EZ_INT;
            vret[1].type= EZ_POINTER;
            vret[2].type= EZ_FUNCTION;
            if(handle) 
              {
                vret[0].value.i = handle->nresources;
                vret[1].value.p = handle->resources;
                vret[2].value.p = (void *)handle->configure;
              }
            else
              {
                vret[0].value.i = 0;
                vret[1].value.p = NULL;
                vret[2].value.p = NULL;
              }
            return(3);
          }
          break;
        case EZ_OVERRIDE_REDIRECT:
          {
            vret[0].type= EZ_BOOL;
            vret[0].value.i = EZ_WidgetOverrideRedirect(widget);
            return(1);
          }
          break;
        case EZ_RULER_DISPLAY_VALUE:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
              vret[0].type= EZ_BOOL;
              vret[0].value.i = EZ_RulerDisplayValue(widget);
              return(1);
            }
          break;
        case EZ_RULER_TICK_UNIT:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_RulerTickSize(widget);
              return(1);
            }
          break;
        case EZ_RULER_TICK_SCALE:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_RulerTickScale(widget);
              return(1);
            }
          break;
        case EZ_RULER_TICK_OFFSET:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_RulerTOffset(widget);
              return(1);
            }
          break;
        case EZ_RULER_TICK_LENGTH:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_RulerTickLength(widget);
              return(1);
            }
          break;
        case EZ_RULER_POSITION:
          if(type == EZ_WIDGET_HORIZONTAL_RULER ||
             type == EZ_WIDGET_VERTICAL_RULER )
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_RulerOrientation(widget);
              return(1);
            }
          break;
        case EZ_TERM_SAVE_LINES:
          {
            EZ_Widget *twidget = NULL;
            if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)
              twidget = EZ_TermITerm(widget);
            else  if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)
              twidget = widget;
            if(twidget)
              {
                vret[0].type= EZ_INT;
                vret[0].value.i = EZ_ITermSaveLines(twidget);
                return(1);
              }
          }
        break;
        case EZ_TERM_REVERSE_VIDEO:
          {
            EZ_Widget *twidget = NULL;
            if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)
              twidget = EZ_TermITerm(widget);
            else  if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)
              twidget = widget;
            if(twidget) 
              {
                vret[0].type= EZ_INT;
                vret[0].value.i = EZ_ITermRVideo(twidget);
                return(1);
              }
          }
        break;
        case EZ_TERM_CURSOR_COLOR:
          {
            EZ_Widget *twidget = NULL;
            if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)
              twidget = EZ_TermITerm(widget);
            else  if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)
              twidget = widget;
            if(twidget) 
              {
                char *ptr = getPrimaryColor(EZ_ITermCursorColor(widget));
                vret[0].type= EZ_STRING;
                vret[0].value.str = ptr;
                return(1);        
              }        
          }
        break;
        case EZ_TERM_COLOR_N:        
          {
            EZ_Widget *twidget = NULL;
            if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)
              twidget = EZ_TermITerm(widget);
            else  if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)
              twidget = widget;
            if(twidget)
              {
                vret[0].type= EZ_STRING;
                vret[0].value.str = getSecondaryColor(EZ_GetITermColors(twidget, oparam));
                return(1);        
              }
          }
        break;
        case EZ_LABEL_SHADOW: 
          {
            int shadow, shadowx,shadowy;
            shadow= EZ_WidgetLabelShadow(widget);
            shadowx = shadowy = shadow;
            if(shadow)
              {
                if(shadow &0xf0)
                  {
                    shadowx = (shadow & 0xf0) >> 4;
                    shadowy = (shadow & 0x0f);
                  }
                else shadowx = shadowy = shadow;
                if(shadowx & 8) shadowx= (8-shadowx);
                if(shadowy & 8) shadowy= (8-shadowy);
              }
            vret[0].type= EZ_INT;
            vret[0].value.i = shadowx;
            vret[1].type= EZ_INT;
            vret[1].value.i = shadowy;
            return(2);        
          }
          break;
        case EZ_WM_IGNORE:
          return(0);
          break;
        case EZ_TILE_ORIGIN:
          vret[0].type= EZ_INT;
          vret[1].type= EZ_INT;
          if(EZ_WidgetTOSet(widget))
            {
              vret[0].value.i= EZ_WidgetTOX(widget);
              vret[1].value.i= EZ_WidgetTOY(widget);
            }
          else
            {
              vret[0].value.i = 0;
              vret[1].value.i = 0;
            }
          return(2);        
          break;
        case EZ_OPTIONAL_ENTRY_STYLE:
          if(type == EZ_WIDGET_OPTIONAL_ENTRY)
            {
              vret[0].type= EZ_INT;
              vret[0].value.i = EZ_GetOptionalEntryStyle(widget);
              return(1);
            }
          break;
        case EZ_INTERIOR_BORDER:          
          if(type == EZ_WIDGET_FRAME)
            {
              vret[0].type= EZ_INT;
              vret[1].type= EZ_INT;
              vret[0].value.i= EZ_FrameIntBdWidth(widget);
              vret[1].value.i= EZ_FrameIntBdType(widget);
              return(2);
            }
          else if(type == EZ_WIDGET_ITOPLEVEL)
            {
              vret[0].type= EZ_INT;
              vret[1].type= EZ_INT;
              vret[0].value.i= EZ_LabelIBorderType(widget);
              vret[1].value.i= EZ_LabelIBorderWidth(widget);
              return(2);
            }
          break;
        case EZ_ARROW_TYPE:
          {
            vret[0].type = EZ_INT;
            if(type == EZ_WIDGET_NORMAL_BUTTON || type ==  EZ_WIDGET_ARROW_BUTTON)
              {
                vret[0].value.i = EZ_ArrowButtonType(widget);
                return(1);
              }
            else if(type == EZ_WIDGET_LABEL || type == EZ_WIDGET_NW_LABEL)
              { vret[0].value.i = EZ_LabelArrowType(widget); return(1);}
            else if(type == EZ_WIDGET_SCROLL_BUTTON)
              { vret[0].value.i= EZ_ScrollButtonArrowType(widget); return(1);} 
          }
        break;
	case EZ_ROW_BG:
          {
            EZ_Widget *twidget = NULL;
            unsigned long *bg = NULL;
            switch(type)
              {
              case EZ_WIDGET_FANCY_LIST_BOX:
                widget = EZ_FListBoxIFListBox(widget);
                /*drop*/
              case EZ_WIDGET_IFANCY_LIST_BOX:
                twidget = widget;
                bg = EZ_IFListBoxRowBG(twidget);
                vret[0].type = EZ_INT;
                vret[1].type = EZ_STRING;
                vret[2].type = EZ_STRING;
                vret[0].value.i=EZ_IFListBoxRowBGSet(twidget);
                vret[1].value.str=getSecondaryColor(bg[0]==0?~0:bg[0]);
                vret[2].value.str=getSecondaryColor(bg[1]==0?~0:bg[1]);
                return(3);
                break;

              case EZ_WIDGET_TREE:
                widget = EZ_TreeWITree(widget);
                /*drop*/
              case EZ_WIDGET_ITREE:
                twidget = widget;
                bg = EZ_ITreeWRowBg(twidget);
                vret[0].type = EZ_INT;
                vret[1].type = EZ_STRING;
                vret[2].type = EZ_STRING;
                vret[0].value.i= EZ_ITreeWRowBgSet(twidget);
                vret[1].value.str=getSecondaryColor(bg[0]==0?~0:bg[0]);
                vret[2].value.str=getSecondaryColor(bg[1]==0?~0:bg[1]);
                return(3);
                break;
              case EZ_WIDGET_FILE_SELECTOR:
                widget =EZ_FileSelectorPatternEntry(widget);
                /*drop*/
              case EZ_WIDGET_OPTIONAL_ENTRY:
                widget = (EZ_Widget *)EZ_OptEntryListbox(widget);                
                /* drop */
              case EZ_WIDGET_LIST_BOX:
                widget = EZ_ListBoxIListBox(widget);
              case EZ_WIDGET_ILIST_BOX:
                twidget = widget;
                /*drop*/
                bg = EZ_IListBoxRowBG(twidget);
                vret[0].type = EZ_INT;
                vret[1].type = EZ_STRING;
                vret[2].type = EZ_STRING;
                vret[0].value.i= EZ_IListBoxRowBGSet(twidget);
                vret[1].value.str=getSecondaryColor(bg[0]==0?~0:bg[0]);
                vret[2].value.str=getSecondaryColor(bg[1]==0?~0:bg[1]);
                return(3);
                break;
              default:
                return(-1);
                break;
              }
          }
	  break;
	case EZ_DOCKABLE:
	  if((type == EZ_WIDGET_FRAME || type== EZ_WIDGET_MENU_BAR) &&
	     EZ_FrameUndockable(widget) == 0)
	    {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_FrameDockable(widget);
              return(1);
	    }
	  break;
	case EZ_DRAG_HANDLE:
          if((type == EZ_WIDGET_FRAME || type== EZ_WIDGET_MENU_BAR) &&
             EZ_FrameUndockable(widget) == 0)
            {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_FrameDragHandle(widget);
              return(1);
            }
	  break;
	case EZ_FORGET_X:
          vret[0].type = EZ_INT;
          vret[0].value.i = EZ_GetWidgetForgetXFlag(widget)? 1: 0;
          return(1);
	  break;
	case EZ_FORGET_Y:
          vret[0].type = EZ_INT;
          vret[0].value.i = EZ_GetWidgetForgetYFlag(widget)? 1: 0;
          return(1);
	  break;
	case EZ_FORGET_W:
          vret[0].type = EZ_INT;
          vret[0].value.i = EZ_GetWidgetForgetWFlag(widget)? 1: 0;
          return(1);
	  break;
	case EZ_FORGET_H:
          vret[0].type = EZ_INT;
          vret[0].value.i = EZ_GetWidgetForgetHFlag(widget)? 1: 0;
          return(1);
	  break;
	case EZ_FORGET_POSITION:
          vret[0].type = EZ_INT;
          vret[0].value.i = EZ_GetWidgetForgetXYFlag(widget)? 1: 0;
          return(1);
	  break;
	case EZ_FORGET_SIZE:
          vret[0].type = EZ_INT;
          vret[0].value.i = EZ_GetWidgetForgetWHFlag(widget)? 1: 0;
          return(1);
	  break;
	case EZ_FORGET_GEOMETRY:
          vret[0].type = EZ_INT;
          vret[0].value.i = EZ_GetWidgetForgetGFlag(widget)? 1: 0;
          return(1);
	  break;
	  /* status meter config options. 2/6/98 */
	case EZ_SMETER_STYLE:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
              vret[0].type = EZ_INT;
              vret[1].type = EZ_INT;
              vret[0].value.i =  EZ_SMeterBarWidth(widget);
              vret[1].value.i = EZ_SMeterBarSep(widget);
              return(2);
	    }
	  break;
	case EZ_SMETER_ORIENTATION:
	  if(type == EZ_WIDGET_STATUS_BAR)
            {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_SMeterOrientation(widget);
              return(1);
            }
	  break;
	case EZ_SMETER_FOREGROUND_N:
	    if(type == EZ_WIDGET_STATUS_BAR)
	      {
		if(oparam >= 0 && oparam < 5)
		  {
                    vret[0].type = EZ_STRING;
                    vret[0].value.str = getPrimaryColor(EZ_SMeterFColor(widget)[oparam]);
                    return(1);
		  }
	      }
	    break;
	case EZ_SMETER_RANGE_N:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
	      if(oparam >= 0 && oparam < 5)
		{
		  float *ff = EZ_SMeterValues(widget);
                  vret[0].type = EZ_FLOAT;
                  vret[1].type = EZ_FLOAT;
                  vret[0].value.f = ff[ 3*oparam];
                  vret[1].value.f = ff[ 3*oparam+1];
                  return(2);
		}
	    }
	  break;
	case EZ_SMETER_VALUE_N:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
              float *ff = EZ_SMeterValues(widget);
              vret[0].type = EZ_FLOAT;
              vret[0].value.f = ff[ 3*oparam+2];
              return(1);
	    }
	  break;
	case EZ_SMETER_SHOW_VALUE:
          if(type == EZ_WIDGET_STATUS_BAR)
            {
              vret[0].type = EZ_BOOL;
              vret[0].value.i = EZ_SMeterShowValue(widget);
              return(1);
            }
	  break;
	case EZ_SMETER_VALUE:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
              float *ff = EZ_SMeterValues(widget);
              vret[0].type = EZ_FLOAT;
              vret[0].value.f = ff[2];
              return(1);
	    }
	  break;
	case EZ_SMETER_STOPED:
	  if(type == EZ_WIDGET_STATUS_BAR)
            {
              vret[0].type = EZ_BOOL;
              vret[0].value.i = EZ_SMeterStoped(widget);
              return(1);
            }
	  break;
	case EZ_SMETER_RANGE:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
	      float *ff = EZ_SMeterRange(widget);
              vret[0].type = EZ_FLOAT;
              vret[1].type = EZ_FLOAT;
              vret[0].value.f = ff[0];
              vret[1].value.f = ff[1];
              return(2);
	    }
	  break;	  
	case EZ_SMETER_BACKGROUND:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
              vret[0].type = EZ_STRING;
              vret[0].value.str = getSecondaryColor(EZ_SMeterBColor(widget));
              return(1);
	    }
	  break;
	case EZ_SMETER_FOREGROUND:
	  if(type == EZ_WIDGET_STATUS_BAR)
	    {
              vret[0].type = EZ_STRING;
              vret[0].value.str = getPrimaryColor(EZ_SMeterFColor(widget)[0]);
              return(1);
	    }
	  break;
	  /* end of status meter config options. 2/6/98 */
	  
	case EZ_HSCROLL_INCREMENT:
          {
            EZ_Widget *twidget = NULL;
            switch(type)
              {
              case  EZ_WIDGET_HORIZONTAL_SCROLLBAR:
                twidget = widget;
                break;
              case EZ_WIDGET_TEXT:
                twidget = EZ_TextHScroll(widget);
                break;
              case EZ_WIDGET_ITEXT:
                twidget = EZ_ITextHScroll(widget);
                break;
              case EZ_WIDGET_LIST_BOX:
                twidget = EZ_ListBoxHScroll(widget);
                break;
              case EZ_WIDGET_ILIST_BOX:
                twidget = EZ_IListBoxHScroll(widget);
                break;
              case EZ_WIDGET_FANCY_LIST_BOX:
                twidget = EZ_FListBoxHScroll(widget);
                break;
              case EZ_WIDGET_IFANCY_LIST_BOX:
                twidget = EZ_IFListBoxHScroll(widget);
                break;
              case EZ_WIDGET_TREE:
                twidget = EZ_TreeWHScroll(widget);
                break;
              case EZ_WIDGET_ITREE:
                twidget = EZ_ITreeWHScroll(widget);
                break;
              case EZ_WIDGET_WORK_AREA:
                twidget = EZ_WorkAreaHScroll(widget);
                break;
              case EZ_WIDGET_IWORK_AREA:
                twidget = EZ_IWorkAreaHScroll(widget);
                break;
              default:
                return(-1);
                break;
              }
            if(twidget)
              {
                vret[0].type = EZ_INT;
                vret[0].value.i = EZ_ScrollIIFactor(twidget);
                return(1);
              }
          }
        break;
	case EZ_VSCROLL_INCREMENT:
          {
            EZ_Widget *twidget = NULL;
            switch(type)
              {
              case  EZ_WIDGET_VERTICAL_SCROLLBAR:
                twidget = widget;
                break;
              case EZ_WIDGET_TERM:
                twidget = EZ_TermVScroll(widget);          
                break;
              case EZ_WIDGET_ITERM:
                twidget = EZ_ITermVScroll(widget);          
                break;
              case EZ_WIDGET_TEXT:
                twidget = EZ_TextVScroll(widget);
                break;
              case EZ_WIDGET_ITEXT:
                twidget = EZ_ITextVScroll(widget);
                break;
              case EZ_WIDGET_LIST_BOX:
                twidget = EZ_ListBoxVScroll(widget);
                break;
              case EZ_WIDGET_ILIST_BOX:
                twidget = EZ_IListBoxVScroll(widget);
                break;
              case EZ_WIDGET_FANCY_LIST_BOX:
                twidget = EZ_FListBoxVScroll(widget);
                break;
              case EZ_WIDGET_IFANCY_LIST_BOX:
                twidget = EZ_IFListBoxVScroll(widget);
                break;
              case EZ_WIDGET_TREE:
                twidget = EZ_TreeWVScroll(widget);
                break;
              case EZ_WIDGET_ITREE:
                twidget = EZ_ITreeWVScroll(widget);
                break;
              case EZ_WIDGET_WORK_AREA:
                twidget = EZ_WorkAreaVScroll(widget);
                break;
              case EZ_WIDGET_IWORK_AREA:
                twidget = EZ_IWorkAreaVScroll(widget);
                break;
              default:
                return(-1);
                break;
              }
            if(twidget)
              {
                vret[0].type =EZ_INT;
                vret[0].value.i = EZ_ScrollIIFactor(twidget);
                return(1);
              }
          }
          break;
	case EZ_GEOMETRY_MANAGER:
          {
            EZ_GManager *ptr = EZ_WidgetGManager(widget);
            vret[0].type =EZ_INT;
            if(ptr) vret[0].value.i = EZ_GMType(ptr);
            else  vret[0].value.i = EZ_DEFAULT_GEOMETRY_MANAGER;
            return(1);
          }
	break;
	case EZ_LED_COLOR_FUNCTION :
	  if(type == EZ_WIDGET_LED)
            {          
              vret[0].type = EZ_FUNCTION;
              vret[0].value.p = (void *)EZ_LEDColorF(widget);
              return(1);
            }
	  break;
	case EZ_LED_BACKGROUND:
	  if(type == EZ_WIDGET_LED)
	    {
              vret[0].type = EZ_STRING;
              vret[0].value.str = getSecondaryColor(EZ_LEDBG(widget));
              return(1);
	    }
	  break;
	case EZ_LED_PIXEL_COLOR:
	  if(type == EZ_WIDGET_LED)
	    {
	      vret[0].type = EZ_STRING;
              vret[0].value.str = getPrimaryColor(EZ_LEDBGL(widget));
              return(1);
	    }
	  break;
	case EZ_LED_WIDTH:
	  if(type == EZ_WIDGET_LED)
	    {
	      vret[0].type = EZ_INT;
              vret[0].value.i = EZ_LEDSCols(widget);
              return(1);
	    }
	  break;
	case EZ_LED_HEIGHT:
	  if(type == EZ_WIDGET_LED)
	    {
	      vret[0].type = EZ_INT;
              vret[0].value.i = EZ_LEDSRows(widget);
              return(1);
	    }
	  break;
	case EZ_LED_PIXEL_SIZE:
	  if(type == EZ_WIDGET_LED)
	    {
	      vret[0].type = EZ_INT;
              vret[0].value.i = EZ_LEDXSize(widget);
	      vret[1].type = EZ_INT;
              vret[1].value.i = EZ_LEDYSize(widget);
              return(2);
	    }
	  break;
	case EZ_LCD_BACKGROUND:
	  if(type == EZ_WIDGET_LCD)
	    {
              vret[0].type = EZ_STRING;
              vret[0].value.str = getSecondaryColor(EZ_LCDBG(widget));
              return(1);
	    }
	  break;
	case EZ_LCD_FOREGROUND:
	  if(type == EZ_WIDGET_LCD)
	    {
              vret[0].type = EZ_STRING;
              vret[0].value.str = getPrimaryColor(EZ_LCDFG(widget));
              return(1);
	    }
	  break;	  
	case EZ_LCD_NDIGITS:
	  if(type == EZ_WIDGET_LCD)
	    {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_LCDNDigits(widget);
              return(1);
	    }
	  break;
	case EZ_LCD_FONT_SIZE:
	  if(type == EZ_WIDGET_LCD)
	    {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_LCDSize(widget);
              return(1);
	    }
	  break;
	case EZ_PADB:
          vret[0].type = EZ_INT;
          vret[0].value.i = EZ_WidgetPadB(widget);
          return(1);
	  break;
	case EZ_GRID_CELL_GEOMETRY:
	  {
	    EZ_Widget *parent = EZ_WidgetParent(widget);
	    if(parent && EZ_WidgetGManager(parent) != (EZ_GManager *)NULL &&
               EZ_GMType(EZ_WidgetGManager(parent)) ==EZ_GB_GEOM_MANAGER)
              {
                vret[0].type=vret[1].type=vret[2].type=vret[3].type=EZ_INT;
                vret[0].value.i=EZ_WidgetGBX(widget);
                vret[1].value.i=EZ_WidgetGBY(widget);
                vret[2].value.i=EZ_WidgetGBW(widget);
                vret[3].value.i=EZ_WidgetGBH(widget);
                return(4);
              }
	  }
          break;
	case EZ_GRID_CELL_PLACEMENT:
	  {
	    EZ_Widget *parent = EZ_WidgetParent(widget);
	    if(parent && EZ_WidgetGManager(parent) != (EZ_GManager *)NULL &&
               EZ_GMType(EZ_WidgetGManager(parent)) ==EZ_GB_GEOM_MANAGER)
              {
                vret[0].type=vret[1].type=EZ_INT;
                vret[0].value.i=EZ_WidgetGBFill(widget);
                vret[1].value.i=EZ_WidgetGBAnchor(widget);
                return(2);
              }
	  }
          break;
	case EZ_GRID_CONSTRAINS:
          return(0);
	  break;
	case EZ_GRID_ROW_CONSTRAINS:
          {
            EZ_GManager *gmgr= EZ_WidgetGManager(widget);
            if(gmgr && EZ_GMType(gmgr) == EZ_GB_GEOM_MANAGER)
              {
                EZ_GridBagGMData *gbdata = &(EZ_GMGB(gmgr));
                EZ_RowColStruct *rw, **ptr = &(EZ_GBGMRowPtr(gbdata));
                int    space = EZ_GBGMRowSpace(gbdata);
                if(oparam >= 0 && oparam < space)
                  {
                    vret[0].type=vret[1].type=vret[2].type = EZ_INT;
                    rw = (*ptr) + oparam;
                    vret[0].value.i= EZ_GridBagRCMinSize(rw);
                    vret[1].value.i= EZ_GridBagRCWeight(rw);
                    vret[2].value.i= EZ_GridBagRCPadding(rw);
                    return(3);
                  }
              }
          }
        return(0);
        break;
	case EZ_GRID_COLUMN_CONSTRAINS:
          {
            EZ_GManager *gmgr= EZ_WidgetGManager(widget);
            if(gmgr && EZ_GMType(gmgr) == EZ_GB_GEOM_MANAGER)
              {
                EZ_GridBagGMData *gbdata = &(EZ_GMGB(gmgr));
                EZ_RowColStruct *rw, **ptr = &(EZ_GBGMColPtr(gbdata));
                int    space = EZ_GBGMColSpace(gbdata);
                if(oparam >= 0 && oparam < space)
                  {
                    vret[0].type=vret[1].type=vret[2].type = EZ_INT;
                    rw = (*ptr) + oparam;
                    vret[0].value.i= EZ_GridBagRCMinSize(rw);
                    vret[1].value.i= EZ_GridBagRCWeight(rw);
                    vret[2].value.i= EZ_GridBagRCPadding(rw);
                    return(3);
                  }
              }
          }
        return(0);
        break;
        case EZ_ROW_COLUMN_MAX_SIZE:
          {
            EZ_GManager *gmgr;
            if((gmgr= EZ_WidgetGManager(widget)))
              {
                int gtype = EZ_GMType(gmgr);
                if(gtype == EZ_ROW_GEOM_MANAGER || gtype == EZ_COLUMN_GEOM_MANAGER)
                  {
                    EZ_RCGMData *rcdata = &(EZ_GMRC(gmgr));
                    vret[0].type= EZ_RCGMMaxSize(rcdata);
                    return(1);
                  }
              }
          }
        return(0);
        break;
	case EZ_MENU_TEAR_OFF:
	  if(type == EZ_WIDGET_POPUP_MENU)
            {

              EZ_Widget *children = EZ_WidgetChildren(widget);
              while(children)
                {
                  if(EZ_WidgetType(children) == EZ_WIDGET_MENU_TEAR_OFF_BAR)
                    break;
                  children = EZ_WidgetSibling(children);
                }
              vret[0].type = EZ_BOOL;
              if(children)
                vret[0].value.i = (EZ_GetWidgetNonActiveFlag(children) == 0);
              else 
                vret[0].value.i = 0;
              return(1);
	    }
	  break;
	case EZ_UNDERLINE:
	  switch(type)
	    {
	    case EZ_WIDGET_NW_LABEL:
	      {
		vret[0].type = EZ_INT;
		vret[0].value.i= EZ_LabelUnderline(widget);
		return(1);
	      }
	    break;
	    case EZ_WIDGET_NORMAL_BUTTON:
	    case EZ_WIDGET_ARROW_BUTTON:
	    case EZ_WIDGET_MENU_NORMAL_BUTTON:
	      {
		vret[0].type = EZ_INT;
		vret[0].value.i= EZ_ButtonUnderLine(widget);
		return(1);
	      }
	    break;
	    case EZ_WIDGET_CHECK_BUTTON:
	    case EZ_WIDGET_MENU_CHECK_BUTTON:
	      {
		vret[0].type = EZ_INT;
		vret[0].value.i= EZ_CButtonUnderLine(widget);
		return(1);
	      }
	    break;
	    case EZ_WIDGET_RADIO_BUTTON:
	    case EZ_WIDGET_MENU_RADIO_BUTTON:
	      {
		vret[0].type = EZ_INT;
		vret[0].value.i= EZ_RButtonUnderLine(widget);
		return(1);
	      }
	    break;
	    case EZ_WIDGET_MENU_BUTTON:
	      {
		vret[0].type = EZ_INT;
		vret[0].value.i= EZ_MButtonUnderLine(widget);
		return(1);
	      }
	    break;
	    case EZ_WIDGET_MENU_SUBMENU:
	      {
		vret[0].type = EZ_INT;
		vret[0].value.i= EZ_SubMenuUnderLine(widget);
		return(1);
	      }
	    break;
	    case EZ_WIDGET_NB_PAGE:
	      {
		EZ_Widget *btn = EZ_NBPageCompanion(widget);
		if(btn)
		  {
		    vret[0].type = EZ_INT;
		    vret[0].value.i= EZ_ButtonUnderLine(btn);
		    return(1);
		  }
	      }
	      break;
	    default:
	      break;
	    }
	  break;
	case EZ_FANCY_LIST_BOX_COLUMNS:
	  {
	    EZ_Widget *iflbox = NULL;
	    if(type == EZ_WIDGET_FANCY_LIST_BOX)
	      iflbox = EZ_FListBoxIFListBox(widget);
	    else if(type == EZ_WIDGET_IFANCY_LIST_BOX)
	      iflbox = widget;
	    if(iflbox)
	      {
		vret[0].type = EZ_INT;
		vret[0].value.i=  EZ_IFListBoxColumns(widget);
		return(1);
	      }
	  }
	  break;
	case EZ_RETURN_VALUE:
	  vret[0].type = EZ_INT;
	  vret[0].value.i= EZ_WidgetRetData(widget);
	  return(1);
	  break;
	case EZ_FANCY_LIST_BOX_TYPE:
	  {
	    EZ_Widget *iflbox = NULL;
	    if(type == EZ_WIDGET_FANCY_LIST_BOX)
	      iflbox = EZ_FListBoxIFListBox(widget);
	    else if(type == EZ_WIDGET_IFANCY_LIST_BOX)
	      iflbox = widget;
	    if(iflbox)
	      {
		vret[0].type = EZ_INT;
		vret[0].value.i= EZ_IFListBoxRowMode(iflbox);
		return(1);
	      }
	  }
	  break;
	case EZ_TEXT_WIDGET_EDITABLE:
	  {
	    EZ_Widget *iText = NULL;
	    if(type == EZ_WIDGET_TEXT)
	      iText = EZ_TextIText(widget);
	    else if(type == EZ_WIDGET_ITEXT)
	      iText = widget;
	    if(iText)
	      {
		vret[0].type = EZ_INT;
		vret[0].value.i= EZ_ITextEditable(iText);
                return(1);		
	      }
	  }
          break;
	case EZ_OPTIONAL_ENTRY_EDITABLE:
	  {
	    EZ_Widget *entry = NULL;
	    if(type == EZ_WIDGET_OPTIONAL_ENTRY)
	      {	entry = EZ_OptEntryEntry(widget);}
	    else if(type == EZ_WIDGET_ENTRY)
	      entry = widget;
	    if(entry)
	      {
		vret[0].type = EZ_INT;
		vret[0].value.i=  EZ_EntryEditable(entry);
		return(1);		
	      }
	  }
          break;
	case EZ_OPTIONAL_ENTRY_REMEMBER_HISTORY:
	  if(type == EZ_WIDGET_OPTIONAL_ENTRY)
	    {
	      EZ_Widget *entry =  EZ_OptEntryEntry(widget);
	      vret[0].type = EZ_INT;
	      vret[0].value.i=  EZ_EntryRemberHistory(entry);
	      return(1);		
	    }
	  break;
	case EZ_SLIDER_STYLE:
	  if((type == EZ_WIDGET_HORIZONTAL_SLIDER ||
	     type == EZ_WIDGET_VERTICAL_SLIDER))
	    { 
	      vret[0].type = EZ_INT;
	      vret[0].value.i=  EZ_SliderStyle(widget);
	      return(1);		
	    }
          else if(type == EZ_WIDGET_HORIZONTAL_SCROLLBAR ||
                  type ==  EZ_WIDGET_VERTICAL_SCROLLBAR)
            {
		  vret[0].type = EZ_INT;
		  vret[0].value.i=  EZ_ScrollStyle(widget);
		  return(1);		
            }
          else if(type == EZ_WIDGET_GRADIENT_BAR)
            {
              vret[0].type = EZ_INT;
              vret[0].value.i =EZ_GBarStyle(widget);
              return(1);
            }
	  break;
	case EZ_SLIDER_INIT_VALUE:
	  if(type == EZ_WIDGET_HORIZONTAL_SLIDER ||
	     type == EZ_WIDGET_VERTICAL_SLIDER)
	    {
		  vret[0].type = EZ_FLOAT;
		  vret[0].value.f=   EZ_SliderValue(widget);
		  return(1);		
	    }
          else if(type == EZ_WIDGET_GRADIENT_BAR)
            {
              vret[0].type = EZ_FLOAT;
              vret[0].value.i =EZ_GBarValue(widget);
              return(1);
            }
          else if(type == EZ_WIDGET_THUMB_WHEEL)
            {
              vret[0].type = EZ_FLOAT;
              vret[0].value.i = EZ_ThumValue(widget);
              return(1);
            }
	  break;
	case EZ_ENTRY_STRING:
	  {
	    if(type == EZ_WIDGET_ENTRY || type == EZ_WIDGET_OPTIONAL_ENTRY)
	      {
                vret[0].type = EZ_STRING;
                vret[0].value.str= EZ_GetEntryString(widget);
                return(1);		
	      }
          }
          break;
        case EZ_GLOB_PATTERN:
	  switch(type)
	    {
	    case EZ_WIDGET_OPTIONAL_ENTRY:
	      {
		EZ_Widget *twidget =  EZ_OptEntryEntry(widget);
		if(twidget)
		  {
		    vret[0].type = EZ_STRING;
		    vret[0].value.str= EZ_GetEntryString(twidget);
		    return(1);		
		  }
	      }
	      break;
	    case EZ_WIDGET_FILE_SELECTOR:
	      {
                EZ_Widget *rentry, *entry = EZ_FileSelectorPatternEntry(widget);
                rentry = EZ_OptEntryEntry(entry);
		if(rentry)
		  {
		    vret[0].type = EZ_STRING;
		    vret[0].value.str= EZ_GetEntryString(rentry);
		    return(1);		
		  }
              }
	    break;
	    default:
	      break;
	    }
	  break;
         case EZ_CHECK_BUTTON_ON_VALUE:
           if(type == EZ_WIDGET_CHECK_BUTTON || type ==  EZ_WIDGET_MENU_CHECK_BUTTON)
	    {
	      vret[0].type = EZ_INT;
	      vret[0].value.i= EZ_CButtonVarOnValue(widget);
	      return(1);
	    }
	  break;
	case EZ_CHECK_BUTTON_OFF_VALUE:
	  if(type == EZ_WIDGET_CHECK_BUTTON ||
	     type ==  EZ_WIDGET_MENU_CHECK_BUTTON)
	    {
	      vret[0].type = EZ_INT;
	      vret[0].value.i= EZ_CButtonVarOffValue(widget);
	      return(1);
	    }
	  break;
         case EZ_CHECK_BUTTON_ON_OFF:
	  if(type == EZ_WIDGET_CHECK_BUTTON ||
	     type ==  EZ_WIDGET_MENU_CHECK_BUTTON)
	    {
	      vret[0].type = EZ_INT;
	      vret[0].value.i= (EZ_CButtonVarValue(widget) == EZ_CButtonVarOnValue(widget));
	      return(1);
	    }
	  break;
         case EZ_RADIO_BUTTON_GROUP:
	  if(type == EZ_WIDGET_RADIO_BUTTON ||
	     type == EZ_WIDGET_MENU_RADIO_BUTTON)
	    {
	      vret[0].type = EZ_INT;
	      if(EZ_RButtonGroup(widget))
		{
		  vret[0].value.i= EZ_RButtonGroup(widget)->id;
		  return(1);
		}
	      return(0);
	    }
	  break;
        case EZ_RADIO_BUTTON_VALUE:
	  if(type == EZ_WIDGET_RADIO_BUTTON || type == EZ_WIDGET_MENU_RADIO_BUTTON)
	    {
	      vret[0].type = EZ_INT;
	      vret[0].value.i= EZ_RButtonVarValue(widget);
	    }
	  break;
	case EZ_OPTIONAL_HSCROLLBAR:
	  {
	    EZ_Widget *hscroll = NULL;
	    vret[0].type = EZ_INT;
	    switch(type)
	      {
	      case EZ_WIDGET_TEXT:
		hscroll = EZ_TextHScroll(widget);
		break;
	      case EZ_WIDGET_LIST_BOX:
		hscroll = EZ_ListBoxHScroll(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		hscroll = EZ_FListBoxHScroll(widget);
		break;
	      case EZ_WIDGET_TREE:
		hscroll = EZ_TreeWHScroll(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		hscroll = EZ_WorkAreaHScroll(widget);
		break;
		default:
		  return(-1);
		  break;
              }
	    vret[0].value.i = (hscroll && (EZ_GetWidgetNonActiveFlag(hscroll) == 0));
	    return(1);
          }
	  break;
        case EZ_OPTIONAL_VSCROLLBAR:
	  {
	    EZ_Widget *hscroll=NULL, *vscroll = NULL;
	    vret[0].type = EZ_INT;
	    switch(type)
	      {
	      case EZ_WIDGET_TEXT:
		hscroll = EZ_TextVScroll(widget);
		break;
	      case EZ_WIDGET_LIST_BOX:
		hscroll = EZ_ListBoxVScroll(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		hscroll = EZ_FListBoxVScroll(widget);
		break;
	      case EZ_WIDGET_TREE:
		hscroll = EZ_TreeWVScroll(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		hscroll = EZ_WorkAreaVScroll(widget);
		break;
	      case EZ_WIDGET_TERM:
		vscroll = EZ_TermVScroll(widget);
		break;
	      default:
		return(-1);
		break;
	      }
	    vret[0].value.i = (vscroll && (EZ_GetWidgetNonActiveFlag(vscroll) == 0));
	    return(1);
	  }
          break;
        case EZ_SHAPED_WINDOW:
          vret[0].type = EZ_INT;
          vret[0].value.i = (EZ_GetWidgetShapedWinFlag(widget) != 0); 
          return(1);
          break;
        case EZ_LABELED_ICON:
          if(type == EZ_WIDGET_ITOPLEVEL)
            {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_LabelIsLabeledIcon(widget);
              return(1);
            }
          break;
        case  EZ_CURSOR:
	case  EZ_CURSOR_NAME:
	  {
            EZ_Widget *twidget = widget;
            switch(type)
              {
              case EZ_WIDGET_TREE:
                twidget = EZ_TreeWITree(widget);		  
                break;
              case EZ_WIDGET_LIST_BOX:
                twidget = EZ_ListBoxIListBox(widget);
                break;
              case EZ_WIDGET_FANCY_LIST_BOX:
                twidget = EZ_FListBoxIFListBox(widget);
                break;
              case EZ_WIDGET_WORK_AREA:
                twidget = EZ_WorkAreaIWorkArea(widget);
                break;
              case EZ_WIDGET_TEXT:
                twidget = EZ_TextIText(widget);
                break;
              case EZ_WIDGET_TERM:
                twidget = EZ_TermITerm(widget);
                break;
              default:
                break;
              }
            if(option == EZ_CURSOR)
              {
                vret[0].type = EZ_ULONG;
                vret[0].value.l = EZ_WidgetCursor(twidget);
              }
            else
              {
                vret[0].type = EZ_STRING;
                vret[0].value.str = EZ_GetCursorName(EZ_WidgetCursor(twidget));
              }
            return(1);
          }
          break;
        case  EZ_DND_DRAG_CURSOR:
	case  EZ_DND_DRAG_CURSOR_NAME:
	  {
            EZ_Widget *twidget = widget;
            switch(type)
              {
              case EZ_WIDGET_TREE:
                twidget = EZ_TreeWITree(widget);		  
                break;
              case EZ_WIDGET_LIST_BOX:
                twidget = EZ_ListBoxIListBox(widget);
                break;
              case EZ_WIDGET_FANCY_LIST_BOX:
                twidget = EZ_FListBoxIFListBox(widget);
                break;
              case EZ_WIDGET_WORK_AREA:
                twidget = EZ_WorkAreaIWorkArea(widget);
                break;
              case EZ_WIDGET_TEXT:
                twidget = EZ_TextIText(widget);
                break;
              case EZ_WIDGET_TERM:
                twidget = EZ_TermITerm(widget);
                break;
              default:
                break;
              }
            if(option == EZ_DND_DRAG_CURSOR)
              {
                vret[0].type = EZ_ULONG;
                vret[0].value.l = EZ_WidgetDnDDragCursor(twidget);
              }
            else
              {
                vret[0].type = EZ_STRING;
                vret[0].value.str = EZ_GetCursorName(EZ_WidgetDnDDragCursor(twidget));
              }
            return(1);
	  }
          break;
	case  EZ_DND_BUBBLE_HELP:
	  {
	    switch(type)
	      {
	      case EZ_WIDGET_TREE:
		widget = EZ_TreeWITree(widget);		  
		break;
	      case EZ_WIDGET_LIST_BOX:
		widget = EZ_ListBoxIListBox(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		widget = EZ_FListBoxIFListBox(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		widget = EZ_WorkAreaIWorkArea(widget);
		break;
	      case EZ_WIDGET_TEXT:
		widget = EZ_TextIText(widget);
		break;
              case EZ_WIDGET_TERM:
                widget = EZ_TermITerm(widget);
                break;
	      default:
		break;
	      }
            vret[0].type =EZ_STRING;
            vret[0].value.str = EZ_WidgetDnDTargetHelp(widget);
            return(1);
          }
          break;
	case EZ_ATTACH_PTR_DATA:
	  {
	    EZ_Widget *twidget = widget;
	    switch(type)
	      {
	      case EZ_WIDGET_TREE:
		twidget = EZ_TreeWITree(widget);		  
		break;
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		twidget = EZ_FListBoxIFListBox(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		twidget = EZ_WorkAreaIWorkArea(widget);
		break;
	      case EZ_WIDGET_TEXT:
		twidget = EZ_TextIText(widget);
		break;
              case EZ_WIDGET_TERM:
                twidget = EZ_TermITerm(widget);
                break;
	      default:
		break;
	      }
            vret[0].type = EZ_POINTER;
            vret[0].value.p = EZ_WidgetPtrData(twidget);
            return(1);
	  }
	  break;
	case EZ_ATTACH_INT_DATA:
	  {
            vret[0].type = EZ_INT;
            vret[0].value.i = EZ_WidgetIntData(widget);
            return(1);
	  }
	  break;
	case EZ_SELECTION_BACKGROUND:
	  {
	    EZ_Widget *twidget;
            unsigned long pixel_value=0;
	    switch(type)
	      {
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		 pixel_value=EZ_IListBoxSelectionBG(twidget) ;
		break;
	      case EZ_WIDGET_ILIST_BOX:
                pixel_value =EZ_IListBoxSelectionBG(widget);
		break;
	      case EZ_WIDGET_TEXT:
		twidget = EZ_TextIText(widget); 
		pixel_value =EZ_ITextTextSelectionBG(twidget);
		break;
	      case EZ_WIDGET_ITEXT:
		pixel_value =EZ_ITextTextSelectionBG(widget);
		break;
	      case EZ_WIDGET_ENTRY:
		pixel_value =EZ_EntrySelectionBG(widget);
		break;
	      case EZ_WIDGET_ITREE:
		pixel_value =EZ_ITreeWHighlightBG(widget);
		break;
	      case EZ_WIDGET_TREE:
		twidget = EZ_TreeWITree(widget);
		pixel_value =EZ_ITreeWHighlightBG(twidget);
		break;
	      case EZ_WIDGET_IFANCY_LIST_BOX:
		pixel_value =EZ_IFListBoxHighlightBG(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		twidget = EZ_FListBoxIFListBox(widget);
		pixel_value =EZ_IFListBoxHighlightBG(twidget);
		break;
	      case EZ_WIDGET_IWORK_AREA:
		pixel_value =EZ_IWorkAreaHighlightBG(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		twidget = EZ_WorkAreaIWorkArea(widget);
		pixel_value =EZ_IWorkAreaHighlightBG(twidget);
		break;
	      case EZ_WIDGET_OPTIONAL_ENTRY:
		{
		  twidget = (EZ_Widget *)EZ_OptEntryEntry(widget);
		  pixel_value =EZ_EntrySelectionBG(twidget);
		}
		break;
	      case EZ_WIDGET_SPREAD_SHEET:
		pixel_value = EZ_SSheetSelectionBG(widget);
		break;
	      default:
                return(-1);
		break;
	      }
            vret[0].type = EZ_STRING;
            vret[0].value.str = getSecondaryColor(pixel_value);
            return(1);
	  }
	  break;
	case EZ_SELECTION_FOREGROUND:
	  {
	    EZ_Widget *twidget;
	    unsigned long  pixel_value=0;
	    switch(type)
	      {
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		 pixel_value = EZ_IListBoxSelectionFG(twidget) ;
		break;
	      case EZ_WIDGET_ILIST_BOX:
		 pixel_value = EZ_IListBoxSelectionFG(widget) ;
		break;
	      case EZ_WIDGET_IWORK_AREA:
		 pixel_value = EZ_IWorkAreaHighlightFG(widget) ;		
		break;
	      case EZ_WIDGET_WORK_AREA:
		twidget = EZ_WorkAreaIWorkArea(widget);
		 pixel_value = EZ_IWorkAreaHighlightFG(twidget) ;		
		break;
	      case EZ_WIDGET_IFANCY_LIST_BOX:
		 pixel_value = EZ_IFListBoxHighlightFG(widget) ;		
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		twidget = EZ_FListBoxIFListBox(widget);
		 pixel_value = EZ_IFListBoxHighlightFG(twidget) ;		
		break;
	      case EZ_WIDGET_ENTRY:
		pixel_value=EZ_EntrySelectionFG(widget);
		break;
	      case EZ_WIDGET_ITREE:
		 pixel_value = EZ_ITreeWHighlightFG(widget) ;
		break;
	      case EZ_WIDGET_TREE:
		twidget = EZ_TreeWITree(widget);
                pixel_value = EZ_ITreeWHighlightFG(twidget) ;
                break;
	      case EZ_WIDGET_SPREAD_SHEET:
		pixel_value = EZ_SSheetSelectionFG(widget);
		break;
	      default:
                return(-1);
		break;
	      }
            vret[0].type =EZ_STRING;
            vret[0].value.str = getPrimaryColor(pixel_value);
	  }
	  break;
	case EZ_X:
          /*if(EZ_GetWidgetXSetFlag(widget))*/
            {
              vret[0].type = EZ_INT;
              vret[0].value.i=EZ_WidgetOriginX(widget);
              return(1);
            }
          return(0);
          break;
	case EZ_Y:
          /* if(EZ_GetWidgetYSetFlag(widget))*/
            {
              vret[0].type = EZ_INT;
              vret[0].value.i=EZ_WidgetOriginY(widget);
              return(1);
            }
          return(0);
          break;
	case EZ_LOCATION:
          return(0);
          break;
	case EZ_GEOMETRY:
          return(0);
          break;
	case EZ_SIZE:
          return(0);
          break;
	case EZ_SIZE_HINT:
          return(0);
          break;
	case EZ_WIDTH:
          /*if(EZ_GetWidgetWidthSetFlag(widget))*/
            {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_WidgetWidth(widget);
              return(1);
            }
          return(0);
          break;
	case EZ_WIDTH_HINT:
          /*if(EZ_GetWidgetWidthHintFlag(widget))*/
            {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_WidgetWidthHint(widget);
              return(1);
            }
	  break;
	case EZ_HEIGHT:
          /* if(EZ_GetWidgetHeightSetFlag(widget))*/
            {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_WidgetHeight(widget);
              return(1);
            }
          return(0);
          break;
	case EZ_HEIGHT_HINT:
          /*if(EZ_GetWidgetHeightHintFlag(widget))*/
            {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_WidgetHeightHint(widget);
              return(1);
            }
	  break;
	case EZ_INDICATOR_TYPE:
          vret[0].type = EZ_INT;
          switch(type)
            {
            case EZ_WIDGET_CHECK_BUTTON:
            case EZ_WIDGET_MENU_CHECK_BUTTON:
              vret[0].value.i=EZ_CButtonCheckType(widget);
              break;
            case EZ_WIDGET_RADIO_BUTTON:
            case EZ_WIDGET_MENU_RADIO_BUTTON:
              vret[0].value.i=EZ_RButtonCheckType(widget);
              break;
            case EZ_WIDGET_MENU_BUTTON:
              vret[0].value.i=EZ_MButtonCheckType(widget);
              break;
            case EZ_WIDGET_LED:
              vret[0].value.i=(int)EZ_LEDIndicator(widget);
              break;
            case EZ_WIDGET_PANE_HANDLE:
              vret[0].value.i= (int)EZ_PaneHandleStyle(widget);
              break;
            default:
              return(-1);
              break;
            }
          return(1);
	  break;
	case EZ_INDICATOR_COLOR:
	  {
            vret[0].type = EZ_STRING;
	    switch(type)
	      {
	      case EZ_WIDGET_CHECK_BUTTON:
	      case EZ_WIDGET_MENU_CHECK_BUTTON:
		vret[0].value.str=getPrimaryColor(EZ_CButtonCheckColor(widget));
		break;
	      case EZ_WIDGET_RADIO_BUTTON:
	      case EZ_WIDGET_MENU_RADIO_BUTTON:
		vret[0].value.str=getPrimaryColor(EZ_RButtonCheckColor(widget));
                  break;
	      case EZ_WIDGET_MENU_BUTTON:
		vret[0].value.str=getPrimaryColor(EZ_MButtonCheckColor(widget));
		break;
              case EZ_WIDGET_DIAL:
                vret[0].value.str=getPrimaryColor(EZ_DialTForeground(widget));
                break;
	      default:
                return(-1);
		break;	    
	      }
            return(1);
	  }
	  break;
	case EZ_EMBEDER:
          vret[0].type = EZ_BOOL;
          vret[0].value.i = (EZ_GetWidgetEmbederFlag(widget) != 0);
	  break;
	case EZ_BORDER_WIDTH:
          {
              EZ_Widget *twidget = widget;
	      switch(type)
		{
		case EZ_WIDGET_WORK_AREA:
                  twidget = EZ_WorkAreaIWorkArea(widget);
                  break;
		case EZ_WIDGET_FANCY_LIST_BOX:
                  twidget = EZ_FListBoxIFListBox(widget);
                  break;
		case EZ_WIDGET_TREE:
                  twidget = EZ_TreeWITree(widget);
		  break;
		case EZ_WIDGET_LIST_BOX:
                  twidget = EZ_ListBoxIListBox(widget);
		  break;
		case EZ_WIDGET_TEXT:
                  twidget = EZ_TextIText(widget);		
		  break;
                case EZ_WIDGET_TERM:
                  twidget = EZ_TermITerm(widget);		
                  break;
		case EZ_WIDGET_3D_CANVAS:
                  twidget = NULL;
		  break;
		default:
                  break;
		}
              if(twidget)
                {
                  vret[0].type =EZ_INT;
                  vret[0].value.i = EZ_WidgetBorderWidth(twidget);
                  return(1);
                }
              return(0);
	    }
	  break;
	case EZ_BORDER_TYPE:
          vret[0].type =EZ_INT;
          vret[0].value.i = EZ_WidgetBorderStyle(widget);
          return(1);
	  break;
	case EZ_PADX:
	  {
	      EZ_Widget *twidget;
	      switch(type)
		{
		case EZ_WIDGET_TREE:
		  twidget = EZ_TreeWITree(widget);		  
		  break;
		case EZ_WIDGET_LIST_BOX:
		  twidget = EZ_ListBoxIListBox(widget);
		  break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  twidget = EZ_FListBoxIFListBox(widget);
		  break;
		case EZ_WIDGET_WORK_AREA:
		  twidget = EZ_WorkAreaIWorkArea(widget);
		  break;
		default:
		  twidget = widget;
		  break;
		}
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_WidgetPadX(twidget);
              return(1);
          }
	  break;
	case EZ_PADY:
	    {
	      EZ_Widget *twidget;
	      switch(type)
		{
                case EZ_WIDGET_TERM:
                  twidget = EZ_TermITerm(widget);
                  break;
		case EZ_WIDGET_TREE:
		  twidget = EZ_TreeWITree(widget);		  
		  break;
		case EZ_WIDGET_LIST_BOX:
		  twidget = EZ_ListBoxIListBox(widget);
		  break;
		case EZ_WIDGET_TEXT:
		  twidget = EZ_TextIText(widget);		
		  break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  twidget = EZ_FListBoxIFListBox(widget);
		  break;
		case EZ_WIDGET_WORK_AREA:
		  twidget = EZ_WorkAreaIWorkArea(widget);
		  break;
		default:
		  twidget = widget;
		  break;
		}
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_WidgetPadY(twidget);
              return(1);
	    }
	  break;
	case EZ_IPADX:
            {
	      EZ_Widget *twidget;
	      switch(type)
		{
                case EZ_WIDGET_TERM:
                  twidget = EZ_TermITerm(widget);
                  break;
		case EZ_WIDGET_LIST_BOX:
		  twidget = EZ_ListBoxIListBox(widget);
		  break;
		case EZ_WIDGET_TEXT:
		  twidget = EZ_TextIText(widget);
		  break;
		case EZ_WIDGET_NOTE_BOOK:
		  twidget = NULL;
		  break;
		case EZ_WIDGET_TREE:
		  twidget = EZ_TreeWITree(widget);
		  break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  twidget = EZ_FListBoxIFListBox(widget);
		  break;
		case EZ_WIDGET_WORK_AREA:
		  twidget = EZ_WorkAreaIWorkArea(widget);
		  break;
		default:
		  twidget = widget;
		  break;
		}
              if(twidget)
                {
                  vret[0].type = EZ_INT;
                  vret[0].value.i = EZ_WidgetSepX(twidget);
                  return(1);
                }
	    }
	  break;
	case EZ_IPADY:
	    {
	      EZ_Widget *twidget;
	      switch(type)
		{
                case EZ_WIDGET_TERM:
                  twidget = EZ_TermITerm(widget);
                  break;
		case EZ_WIDGET_LIST_BOX:
		  twidget = EZ_ListBoxIListBox(widget);
		  break;
		case EZ_WIDGET_TEXT:
		  twidget = EZ_TextIText(widget);
		  break;
		case EZ_WIDGET_NOTE_BOOK:
		  twidget = NULL;
		  break;
		case EZ_WIDGET_TREE:
		  twidget = EZ_TreeWITree(widget);
		  break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  twidget = EZ_FListBoxIFListBox(widget);
		  break;
		case EZ_WIDGET_WORK_AREA:
		  twidget = EZ_WorkAreaIWorkArea(widget);
		  break;
		default:
		  twidget = widget;
		  break;
		}
              if(twidget)
                {
                  vret[0].type = EZ_INT;
                  vret[0].value.i = EZ_WidgetSepY(twidget);
                  return(1);
                }
	    }
	  break;
	case EZ_STACKING:
	      switch(type)
		{
		case EZ_WIDGET_TREE:
		case EZ_WIDGET_LIST_BOX:
		case EZ_WIDGET_TEXT:
		case EZ_WIDGET_WORK_AREA:
		case EZ_WIDGET_OPTIONAL_ENTRY:
		case EZ_WIDGET_FILE_SELECTOR:
                case EZ_WIDGET_TERM:
                  return(-1);
		  break;
		default:
                  vret[0].type = EZ_INT;
                  vret[0].value.i = EZ_WidgetStacking(widget);
                  return(1);
		  break;
		}
              break;
        case EZ_SIDE:
	      if(type != EZ_WIDGET_LIST_BOX && type != EZ_WIDGET_TEXT &&
		 type != EZ_WIDGET_NOTE_BOOK && type != EZ_WIDGET_TREE &&
                 type != EZ_WIDGET_TERM)
                {
                  vret[0].type = EZ_INT;
                  vret[0].value.i = EZ_WidgetAlignment(widget);
                  return(1);
                }
              break;
	case EZ_LABEL_POSITION:
          vret[0].type = EZ_INT;
          vret[0].value.i = EZ_WidgetLabelPosition(widget);
          return(1);
	  break;
	case EZ_JUSTIFICATION:
          vret[0].type = EZ_INT;
          vret[0].value.i = EZ_WidgetJustification(widget);
          return(1);
	  break;
        case EZ_FILL_MODE:
	      switch(type)
		{
		case EZ_WIDGET_TREE:
		case EZ_WIDGET_LIST_BOX:
		case EZ_WIDGET_TEXT:
		case EZ_WIDGET_NOTE_BOOK:
		case EZ_WIDGET_FILE_SELECTOR:
		case EZ_WIDGET_OPTIONAL_ENTRY:
                case EZ_WIDGET_TERM:
                  return(-1);
		  break;
		default:
                  vret[0].type = EZ_INT;
                  vret[0].value.i = EZ_WidgetFillMode(widget);
                  return(1);
                  break;
                }
              break;
        case EZ_TEXT_SPACING:
	  {
	    EZ_Widget *twidget = NULL;
            switch(type)
              {
              case EZ_WIDGET_TEXT:
                {widget = EZ_TextIText(widget);}
                /*drop*/
              case EZ_WIDGET_ITEXT:
                twidget = widget;
                vret[0].type = EZ_INT;
                vret[0].value.i = EZ_ITextVSpacing(twidget);
                return(1);
                break;
              case EZ_WIDGET_LIST_BOX:
		widget = EZ_ListBoxIListBox(widget);
                /*drop*/
              case EZ_WIDGET_ILIST_BOX:
                twidget = widget;
                vret[0].type = EZ_INT;
                vret[0].value.i =EZ_IListBoxSpacing(twidget);                
                return(1);
                break;
              case EZ_WIDGET_OPTIONAL_ENTRY:
                widget = (EZ_Widget *)EZ_OptEntryListbox(widget);                
                twidget = EZ_ListBoxIListBox(widget);
                vret[0].type = EZ_INT;
                vret[0].value.i =EZ_IListBoxSpacing(twidget);  
                return(1);
                break;
              case EZ_WIDGET_FILE_SELECTOR:
                widget=EZ_FileSelectorDirListbox(widget);                
                twidget = EZ_ListBoxIListBox(widget);
                vret[0].type = EZ_INT;
                vret[0].value.i =EZ_IListBoxSpacing(twidget);  
                return(1);
                break;
	      }
	  }
	  break;
	case EZ_FOREGROUND:
	  {
            vret[0].type = EZ_STRING;
            vret[0].value.str= getPrimaryColor(EZ_WidgetForeground(widget));
            return(1);
	  }
	  break;
	case EZ_TEXT_BACKGROUND:
	  {
            unsigned long pixel_value=0;
            EZ_Widget *twidget;
	    switch(type)
	      {
              case EZ_WIDGET_ENTRY:
                pixel_value = EZ_EntryTextBG(widget);
                break;
	      case EZ_WIDGET_TERM:
		twidget = EZ_TermITerm(widget); 
		pixel_value = EZ_ITermTextBG(twidget);
		break;
	      case EZ_WIDGET_ITERM:
		pixel_value = EZ_ITermTextBG(widget);
		break;
	      case EZ_WIDGET_TREE:
		twidget = EZ_TreeWITree(widget); 
		pixel_value = EZ_ITreeWTextBG(twidget);
		break;
	      case EZ_WIDGET_ITREE:
		pixel_value = EZ_ITreeWTextBG(widget);
		break;
	      case EZ_WIDGET_TEXT:
		twidget = EZ_TextIText(widget);
		pixel_value = EZ_ITextTextBG(twidget);
		break;
	      case EZ_WIDGET_ITEXT:
		pixel_value = EZ_ITextTextBG(widget);
                break;
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		pixel_value = EZ_IListBoxTextBG(twidget);
		break;
	      case EZ_WIDGET_ILIST_BOX:
		pixel_value=EZ_IListBoxTextBG(widget);
		break;
	      case EZ_WIDGET_IFANCY_LIST_BOX:
		pixel_value = EZ_IFListBoxTextBG(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		twidget = EZ_FListBoxIFListBox(widget);
		pixel_value=EZ_IFListBoxTextBG(twidget);
		break;
	      case EZ_WIDGET_IWORK_AREA:
		pixel_value=EZ_IWorkAreaTextBG(widget);
		break;		
	      case EZ_WIDGET_WORK_AREA:
		twidget = EZ_WorkAreaIWorkArea(widget);
		pixel_value=EZ_IWorkAreaTextBG(twidget);
		break;
	      case EZ_WIDGET_SPREAD_SHEET:
		pixel_value = EZ_SSheetTextBG(widget);
		break;
	      default:
		break;
	      }
            if(pixel_value)
              {
                vret[0].type = EZ_STRING;
                vret[0].value.str = getSecondaryColor(pixel_value);
                return(1);
              }
	  }
	  break;
	case EZ_BACKGROUND:
	  {
            vret[0].type = EZ_STRING;
            vret[0].value.str = getSecondaryColor(EZ_WidgetBackground(widget));
            return(1);            
	  }
	  break;
	case EZ_HIGHLIGHT_BG:
          vret[0].type = EZ_STRING;
          if(EZ_GetWidgetHighlightBGFlag(widget))
            vret[0].value.str = getSecondaryColor(EZ_WidgetHighlightBG(widget));
          else
            vret[0].value.str = "nil";
          return(1);
          break;
	case EZ_HIGHLIGHT_FG:
          vret[0].type = EZ_STRING;
          if(EZ_GetWidgetHighlightFGFlag(widget))
            vret[0].value.str = getSecondaryColor(EZ_WidgetHighlightFG(widget));
          else 
            vret[0].value.str = "nil";
          return(1);
	  break;
	case EZ_BITMAP_FILE:
          vret[0].type = EZ_STRING;
          vret[0].value.str = EZ_GetBitmapFileName(EZ_WidgetPixmap(widget));
          return(1);
	  break;
	case EZ_BITMAP_DATA:  /* this is the only option which needs 3 values */
          return(0);
	  break;
	case EZ_PIXMAP_DATA:
          return(0);
	  break;
	case EZ_X_PIXMAP:
          return(0);
	  break;
	case EZ_RAW_RGB_DATA:
          return(0);
	  break;
	case EZ_SHAPE_FILE:
          {
            EZ_Bitmap *bmp = EZ_WidgetShapeMask(widget);
            vret[0].type = EZ_STRING;
            vret[0].value.str = EZ_GetBitmapFileName(bmp);
            return(1);
          }
        break;
	case EZ_SHAPE_PIXMAP:
          {
            EZ_Bitmap *bmp = EZ_WidgetShapeMask(widget);
            vret[0].type = EZ_POINTER;
            vret[0].value.p = bmp;
            return(1);
          }
	  break;
	case EZ_LABEL_PIXMAP:
          {
            EZ_Bitmap *bmp = EZ_WidgetPixmap(widget);
            vret[0].type = EZ_POINTER;
            vret[0].value.p = bmp;
            return(1);
          }
	  break;
	case EZ_IMAGE_FILE:
          {
            EZ_Bitmap *bmp = EZ_WidgetPixmap(widget);
            vret[0].type = EZ_STRING;
            vret[0].value.str = EZ_GetBitmapFileName(bmp);
            return(1);
          }
        break;
	case EZ_BG_IMAGE_FILE:
          {
            EZ_Bitmap *bmp = EZ_WidgetBGPixmap(widget);
            vret[0].type = EZ_STRING;
            vret[0].value.str = EZ_GetBitmapFileName(bmp);
            return(1);
          }
	  break;
	case EZ_BG_IMAGE_FILE_B:
          {
            EZ_Bitmap *bmp = EZ_WidgetBGPixmapB(widget);
            vret[0].type = EZ_STRING;
            vret[0].value.str = EZ_GetBitmapFileName(bmp);
            return(1);
          }
          break;
	case EZ_BG_PIXMAP:
          {
            EZ_Bitmap *bmp = EZ_WidgetBGPixmap(widget);
            vret[0].type = EZ_POINTER;
            vret[0].value.p = bmp;
            return(1);
          }
	  break;
	case EZ_BG_PIXMAP_B:
          {
            EZ_Bitmap *bmp = EZ_WidgetBGPixmapB(widget);
            vret[0].type = EZ_POINTER;
            vret[0].value.p = bmp;
            return(1);
          }
	  break;
	case EZ_FONT_ID:
	case EZ_FONT_NAME:
	  {
	    EZ_Widget *twidget = widget;
	    switch(type)
	      {
              case EZ_WIDGET_TERM:
                twidget = EZ_TermITerm(widget);
                break;
	      case EZ_WIDGET_TREE: 
		twidget = EZ_TreeWITree(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		twidget = EZ_WorkAreaIWorkArea(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX: 
		twidget = EZ_FListBoxIFListBox(widget);
		break;
	      case EZ_WIDGET_TEXT: 
		twidget = EZ_TextIText(widget);
		break;
	      case EZ_WIDGET_LIST_BOX:
		twidget = EZ_ListBoxIListBox(widget);
		break;
	      case  EZ_WIDGET_OPTIONAL_ENTRY:
                {
		  EZ_Widget *listbox = (EZ_Widget *)EZ_OptEntryListbox(widget);
                  twidget =  EZ_ListBoxIListBox(listbox);
                }
		break;
	      default:
		break;
	      }
	    if(option == EZ_FONT_ID)
	      {
                vret[0].type = EZ_INT;
                vret[0].value.i= EZ_GetFontID(EZ_WidgetFont(twidget));
                return(1);
	      }
	    else
	      {
                vret[0].type = EZ_STRING;
                vret[0].value.str= EZ_GetFontName(EZ_WidgetFont(twidget));
                return(1);                
	      }
            return(0);
	  }
	  break;
	case EZ_TEXT_LINE_LENGTH:
          { 
             int value = -1;
	      switch(type)
		{
		case EZ_WIDGET_LED:
                  value=EZ_LEDLineLength(widget);
		  break;
		case EZ_WIDGET_LABEL:
		case EZ_WIDGET_ITOPLEVEL:
		case EZ_WIDGET_NW_LABEL:
		  value=EZ_LabelLineLength(widget);
		  break;
		case EZ_WIDGET_NORMAL_BUTTON:
                case EZ_WIDGET_ARROW_BUTTON:
		case EZ_WIDGET_MENU_NORMAL_BUTTON:
		  value=EZ_ButtonLineLength(widget);
		  break;
		case EZ_WIDGET_CHECK_BUTTON:
		case EZ_WIDGET_MENU_CHECK_BUTTON:
		  value=EZ_CButtonLineLength(widget);
		  break;
		case EZ_WIDGET_RADIO_BUTTON:
		case EZ_WIDGET_MENU_RADIO_BUTTON:
		  value=EZ_RButtonLineLength(widget);
		  break;
		case EZ_WIDGET_MENU_BUTTON:
		  value=EZ_MButtonLineLength(widget);
		  break;
		case EZ_WIDGET_MENU_SUBMENU:
		  value=EZ_SubMenuLineLength(widget);
		  break;
		case EZ_WIDGET_FRAME:
                  value=EZ_FrameLineLength(widget);
		  break;
		case EZ_WIDGET_ENTRY:
		  value=EZ_EntryLineLength(widget);
		  break;
		case  EZ_WIDGET_OPTIONAL_ENTRY:
		  {
		    EZ_Widget *entry = (EZ_Widget *)EZ_OptEntryEntry(widget);
		    value=EZ_EntryLineLength(entry);
		  }
		  break;
		case EZ_WIDGET_NOTE_BOOK:
		  {
		    EZ_Widget *tframe = EZ_NoteBookTFrame(widget);
		    EZ_Widget *stub = EZ_WidgetChildren(tframe);
		    if(stub) value=EZ_ButtonLineLength(stub);
		  }
		  break;
		case EZ_WIDGET_NB_PAGE:		  
		  {
		    EZ_Widget *companion = EZ_NBPageCompanion(widget);
		    value=EZ_ButtonLineLength(companion);
		  }
		  break;
		default:
                  break;
		  break;
		}
              if(value > 0) 
                {
                  vret[0].type = EZ_INT;
                  vret[0].value.i=value;
                  return(1);
                }
              return(0);
          }
	  break;
	case EZ_SLIDER_DISPLAY_VALUE:
	  if(type == EZ_WIDGET_HORIZONTAL_SLIDER || type == EZ_WIDGET_VERTICAL_SLIDER )
	    {
              int noc = EZ_SliderNoCValue(widget);
              int value = EZ_SliderDisplayValue(widget);
              if(noc) { value = (value ==0) ? -1 : -value; }
              vret[0].type = EZ_INT;
              vret[0].value.i = value;
              return(1);
	    }
	  break;
	case EZ_SLIDER_LENGTH:
          if(type == EZ_WIDGET_HORIZONTAL_SLIDER || type == EZ_WIDGET_VERTICAL_SLIDER )
            {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_SliderSliderLength(widget);
              return(1);
            }
	  break;
	case EZ_SLIDER_WIDTH:
          if(type == EZ_WIDGET_HORIZONTAL_SLIDER || type == EZ_WIDGET_VERTICAL_SLIDER )
            {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_SliderWidth(widget);
              return(1);
	    }
	  break;
	case EZ_SLIDER_BORDER_WIDTH:
          if(type == EZ_WIDGET_HORIZONTAL_SLIDER || type == EZ_WIDGET_VERTICAL_SLIDER )
            {
              vret[0].type = EZ_INT;
              vret[0].value.i = EZ_SliderBorderWidth(widget);
              return(1);
            }
	  break;
	case EZ_SLIDER_RESOLUTION: 
          if(type == EZ_WIDGET_HORIZONTAL_SLIDER || type == EZ_WIDGET_VERTICAL_SLIDER )
            {
              vret[0].type = EZ_FLOAT;
              vret[0].value.f = EZ_SliderResolution(widget);
              return(1);
            }
	  break;
	case EZ_SLIDER_RANGE: 
          if(type == EZ_WIDGET_HORIZONTAL_SLIDER || type == EZ_WIDGET_VERTICAL_SLIDER )
            {
              vret[0].type = vret[1].type = EZ_FLOAT;
              vret[0].value.f = EZ_SliderMinValue(widget);
              vret[1].value.f = EZ_SliderMaxValue(widget);
              return(2);
            }
          else if(type == EZ_WIDGET_THUMB_WHEEL)
            {
              vret[0].type = vret[1].type = EZ_FLOAT;
              vret[0].value.f = EZ_ThumFrom(widget);
              vret[1].value.f = EZ_ThumTo(widget);
              return(2);
            }
          break;
	case EZ_INDICATOR_SIZE_ADJUST:
          vret[0].type = EZ_INT;
          switch(type)
            {
            case EZ_WIDGET_CHECK_BUTTON:  case EZ_WIDGET_MENU_CHECK_BUTTON:
              vret[0].value.i= EZ_CButtonCheckSizeAdjust(widget);
              break;
            case EZ_WIDGET_RADIO_BUTTON: case EZ_WIDGET_MENU_RADIO_BUTTON:
              vret[0].value.i=EZ_RButtonCheckSizeAdjust(widget);
              break;
            case EZ_WIDGET_MENU_BUTTON:
              vret[0].value.i=EZ_MButtonCheckSizeAdjust(widget);
              break;
            default:
              return(-1);
              break;
	    }
          return(1);
	  break; 
	case EZ_BUTTON_SHORTCUT:
          vret[0].type = EZ_STRING;
          switch(type)
            {
            case EZ_WIDGET_MENU_NORMAL_BUTTON: 
            case EZ_WIDGET_NORMAL_BUTTON:
            case EZ_WIDGET_ARROW_BUTTON:
              vret[0].value.str=  EZ_ButtonLabel2(widget);
              break;
            case EZ_WIDGET_MENU_CHECK_BUTTON:
            case EZ_WIDGET_CHECK_BUTTON:
              vret[0].value.str=  EZ_CButtonLabel2(widget);
              break;
            case EZ_WIDGET_MENU_RADIO_BUTTON:
            case EZ_WIDGET_RADIO_BUTTON:
              vret[0].value.str=EZ_RButtonLabel2(widget);
              break;
            case EZ_WIDGET_MENU_BUTTON:
              vret[0].value.str=EZ_MButtonLabel2(widget);
              break;		
            case EZ_WIDGET_MENU_SUBMENU:
              vret[0].value.str=EZ_SubMenuLabel2(widget);
              break;		
            default:
              return(-1);
              break;
            }
          return(1);
	  break;
	case EZ_CALL_BACK:
	  {
            EZ_CallBackStruct *cb;
            cb =EZ_TheIthCallBack(EZ_GetCallbackType(widget, EZ_CALLBACK),oparam);
            vret[0].type= EZ_FUNCTION;
            vret[1].type= EZ_POINTER;
            vret[0].value.function = (void *) (cb? cb->callback: NULL);
            vret[1].value.p = (cb? cb->data : NULL);
            return(2);
	  }
	break;
	case EZ_MOTION_CALLBACK:
	  {
            EZ_CallBackStruct *cb;
            cb =EZ_TheIthCallBack(EZ_GetCallbackType(widget, EZ_MOTION_CALLBACK),oparam);
            vret[0].type= EZ_FUNCTION;
            vret[1].type= EZ_POINTER;
            vret[0].value.function = (void *)(cb ? cb->callback: NULL);
            vret[1].value.p = cb? cb->data: NULL;
            return(2);
	  }
	  break;
	case EZ_EVENT_HANDLE:
          {
            EZ_CallBackStruct *cb;
            cb =EZ_TheIthCallBack((EZ_CallBackStruct **)GetWidgetEventHandlers(widget), oparam);
            vret[0].type= EZ_FUNCTION;
            vret[1].type= EZ_POINTER;
            vret[0].value.function = (void *)(cb? cb->callback: NULL);
            vret[1].value.p = cb ? cb->data: NULL;
            return(2);
	  }
	break;
	case EZ_DESTROY_CALLBACK:
          {
            EZ_CallBackStruct *cb;
            cb =EZ_TheIthCallBack(EZ_GetCallbackType(widget, EZ_DESTROY_CALLBACK),oparam);
            vret[0].type= EZ_FUNCTION;
            vret[1].type= EZ_POINTER;
            vret[0].value.function = (void *)(cb? cb->callback: NULL);
            vret[1].value.p = cb? cb->data: NULL;
            return(2);
          }
   	  break;
	case EZ_BUBBLE_HELP:
          vret[0].type =EZ_STRING;
          vret[0].value.str=EZ_WidgetBubbleString(widget);
          return(1);
          break;
	case EZ_MOVABLE:
          vret[0].type = EZ_BOOL;
	  switch(type)
	    {
	    case EZ_WIDGET_PANE_HANDLE:
              vret[0].value.i= EZ_PaneHandleMovable(widget);
	    case EZ_WIDGET_ITOPLEVEL:
	      vret[0].value.i=EZ_LabelMovable(widget);
	      break;
	    case EZ_WIDGET_WORK_AREA:
              {
		EZ_Widget *tw = EZ_WorkAreaIWorkArea(widget);
                vret[0].value.i=EZ_IWorkAreaItemMovable(tw);
	      }
	    break;
	    case EZ_WIDGET_IWORK_AREA:
	      {
		vret[0].value.i=EZ_IWorkAreaItemMovable(widget);
	      }
	    break;
	    default:
              return(-1);
              break;
	    }
          return(1);
	  break;	  
	case EZ_LABEL_STRING:
	  {
            vret[0].type = EZ_STRING;
            switch(type)
              {
              case EZ_WIDGET_FRAME:
                vret[0].value.str=EZ_FrameLabel(widget);
                break;
              case EZ_WIDGET_POPUP_MENU:
                {
                  EZ_Widget *lab = EZ_WidgetChildren(widget);
                  if(lab != NULL && EZ_WidgetType(lab) == EZ_WIDGET_LABEL)
                    vret[0].value.str=EZ_LabelString(lab);
                }
              break;
              case EZ_WIDGET_ITOPLEVEL:
              case EZ_WIDGET_LABEL:
              case EZ_WIDGET_NW_LABEL:
                vret[0].value.str=EZ_LabelString(widget);
                break;
              case EZ_WIDGET_NORMAL_BUTTON:
              case EZ_WIDGET_ARROW_BUTTON:
              case EZ_WIDGET_MENU_NORMAL_BUTTON:
                vret[0].value.str=EZ_ButtonLabel(widget);
                break;
              case EZ_WIDGET_CHECK_BUTTON:
              case EZ_WIDGET_MENU_CHECK_BUTTON:
                vret[0].value.str=EZ_CButtonLabel(widget);
                break;
              case EZ_WIDGET_RADIO_BUTTON:
              case EZ_WIDGET_MENU_RADIO_BUTTON:
                vret[0].value.str=EZ_RButtonLabel(widget);
                break;
              case EZ_WIDGET_MENU_BUTTON:
                vret[0].value.str=EZ_MButtonLabel(widget);
                break;
              case EZ_WIDGET_MENU_SUBMENU:
                vret[0].value.str= EZ_SubMenuLabel(widget);
                break;
              case EZ_WIDGET_NB_PAGE:
                {
                  EZ_Widget *companion = EZ_NBPageCompanion(widget);
                  vret[0].value.str= EZ_ButtonLabel(companion);
                }
              break;
              case EZ_WIDGET_VERTICAL_SLIDER:
              case EZ_WIDGET_HORIZONTAL_SLIDER:
                vret[0].value.str= EZ_SliderLabel(widget);
                break;
              case EZ_WIDGET_LCD:
                vret[0].value.str= EZ_LCDString(widget);
                break;
              case EZ_WIDGET_LED:
                vret[0].value.str= EZ_LEDString(widget);
                break;
              case EZ_WIDGET_DIAL:
                vret[0].value.str= EZ_DialLabel(widget);
                break;
              default:
                return(-1);
                break;
              }
            return(1);
          }
	  break;
	case EZ_NAME:
	  {
            vret[0].type = EZ_STRING;
            vret[0].value.str= EZ_QuarkToString(EZ_WidgetIName(widget));
            return(1);
	  }
	  break;
	case EZ_POPUP_NAME:
	  if(type == EZ_WIDGET_OPTIONAL_ENTRY)
	    {
	      EZ_Widget *tmp = EZ_OptEntryPopup(widget);
              vret[0].type = EZ_STRING;
              vret[0].value.str= EZ_QuarkToString(EZ_WidgetIName(tmp));
              return(1);
	    }
	  else if(type == EZ_WIDGET_FILE_SELECTOR)
	    {
              EZ_Widget *ee,*pp;
              ee = EZ_FileSelectorPatternEntry(widget);
              pp = EZ_OptEntryPopup(ee);
              vret[0].type = EZ_STRING;
              vret[0].value.str= EZ_QuarkToString(EZ_WidgetIName(pp));
              return(1);
	    }
	  break;
	case EZ_CLASS:
	  {
            vret[0].type = EZ_STRING;
            vret[0].value.str= EZ_QuarkToString(EZ_WidgetCName(widget));
            return(1);
	  }
	  break;
	case EZ_RESERVE_MENU_BUTTON:
	  if(type == EZ_WIDGET_3D_CANVAS)
            {
              vret[0].type = EZ_BOOL;
              vret[0].value.i= EZ_3DCanvasReleaseMenuButton(widget);
              return(1);
            }
	  break;
	case EZ_BACKING_STORE:
	  if(type == EZ_WIDGET_3D_CANVAS)
            {
              vret[0].type = EZ_BOOL;
              vret[0].value.i= EZ_3DCanvasBackingStore(widget);
              return(1);
            }
	  break;
	case EZ_EXPAND:
          vret[0].type = EZ_BOOL;
          vret[0].value.i= (EZ_GetWidgetExpandFlag(widget) == 0);
          return(1);
	case EZ_TRANSIENT:
          vret[0].type = EZ_BOOL;
          vret[0].value.i= (EZ_GetWidgetTransientFlag(widget) == 0);
          return(1);
	  break;
	case EZ_SCROLLBAR_WIDTH:
	  {
	    EZ_Widget *hwidget = (EZ_Widget *)NULL;
	    EZ_Widget *vwidget = (EZ_Widget *)NULL;

	    switch( type )
	      {
	      case EZ_WIDGET_TREE: 
		hwidget = EZ_TreeWHScroll(widget);
		vwidget = EZ_TreeWVScroll(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		hwidget = EZ_WorkAreaHScroll(widget);
		vwidget = EZ_WorkAreaVScroll(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		hwidget = EZ_FListBoxHScroll(widget);
		vwidget = EZ_FListBoxVScroll(widget);
		break;
	      case EZ_WIDGET_LIST_BOX:
		hwidget = EZ_ListBoxHScroll(widget);
		vwidget = EZ_ListBoxVScroll(widget);
		break;
	      case EZ_WIDGET_TEXT:
		hwidget = EZ_TextHScroll(widget);
		vwidget = EZ_TextVScroll(widget);
		break;
	      case EZ_WIDGET_HORIZONTAL_SCROLLBAR:
		hwidget = widget;
		break;
	      case EZ_WIDGET_VERTICAL_SCROLLBAR:
		vwidget = widget;
		break;
              case EZ_WIDGET_TERM:
		vwidget = EZ_TermVScroll(widget);
                break;
	      case  EZ_WIDGET_OPTIONAL_ENTRY:
		{
		  EZ_Widget *listbox = EZ_OptEntryListbox(widget);
		  hwidget = EZ_ListBoxHScroll(listbox);
		  vwidget = EZ_ListBoxVScroll(listbox);
		}
		break;
	      default:
		break;
	      }
	    if(hwidget) 
	      {
                vret[0].type =EZ_INT;
                vret[0].value.i=EZ_ScrollSSize(hwidget);
                return(1);
	      }
	    else if(vwidget) 
	      {
                vret[0].type =EZ_INT;
                vret[0].value.i=EZ_ScrollSSize(vwidget);
                return(1);
              }
	  }
	  break;
	case EZ_SCROLLBAR_BORDER_WIDTH:
          {
	      EZ_Widget *hwidget = (EZ_Widget *)NULL;
	      EZ_Widget *vwidget = (EZ_Widget *)NULL;

	      switch( type )
		{
                case EZ_WIDGET_TERM:
                  vwidget = EZ_TermVScroll(widget);
                  break;
		case EZ_WIDGET_TREE: 
		  hwidget = EZ_TreeWHScroll(widget);
		  vwidget = EZ_TreeWVScroll(widget);
		  break;
		case EZ_WIDGET_WORK_AREA:
		  hwidget = EZ_WorkAreaHScroll(widget);
		  vwidget = EZ_WorkAreaVScroll(widget);
		  break;
		case EZ_WIDGET_FANCY_LIST_BOX:
		  hwidget = EZ_FListBoxHScroll(widget);
		  vwidget = EZ_FListBoxVScroll(widget);
		  break;
		case EZ_WIDGET_LIST_BOX:
		  hwidget = EZ_ListBoxHScroll(widget);
		  vwidget = EZ_ListBoxVScroll(widget);
		  break;
		case EZ_WIDGET_TEXT:
		  hwidget = EZ_TextHScroll(widget);
		  vwidget = EZ_TextVScroll(widget);
		  break;
		case EZ_WIDGET_HORIZONTAL_SCROLLBAR:
		  hwidget = widget;
		  break;
		case EZ_WIDGET_VERTICAL_SCROLLBAR:
		  vwidget = widget;
		  break;
		case  EZ_WIDGET_OPTIONAL_ENTRY:
		  {
		    EZ_Widget *listbox = EZ_OptEntryListbox(widget);
		    hwidget = EZ_ListBoxHScroll(listbox);
		    vwidget = EZ_ListBoxVScroll(listbox);
		  }
		  break;
		case  EZ_WIDGET_SCROLL_BUTTON:
                  hwidget = widget;
                  break;
		default:
		  break;
		}
	    if(hwidget) 
	      {
                vret[0].type =EZ_INT;
                vret[0].value.i=EZ_WidgetBorderWidth(hwidget);
                return(1);
	      }
	    else if(vwidget) 
	      {
                vret[0].type =EZ_INT;
                vret[0].value.i=EZ_WidgetBorderWidth(vwidget);
                return(1);
              }
          }
	  break;
	case EZ_PROPAGATE:
          vret[0].type =EZ_INT;
          vret[0].value.i= (EZ_GetDoNotPropagateFlag(widget) == 0);
          return(1);
	  break;
	case EZ_WMHINTS: /* inherited from previous versions: sizeHints */
	case EZ_WM_WINDOW_NAME:
	case EZ_WM_ICON_NAME:
	case EZ_WM_ICON_XPM_FILE:
	case EZ_WM_ICON_PIXMAP:
	case EZ_WM_ICON_LOCATION:
	case EZ_WM_LOCATION_HINT:
	case EZ_WM_SIZE_HINT:
	case EZ_WM_MAX_SIZE_HINT:
	case EZ_WM_MIN_SIZE_HINT:
	case EZ_WM_INITIAL_STATE:
          /* to be done later */
          return(0);
	  break;
	case EZ_MARK_CLEAN:
          vret[0].type = EZ_BOOL;
          vret[0].value.i= (EZ_GetWidgetSizeComputedFlag(widget) != 0);
          return(1);
	  break;
	case EZ_HIGHLIGHT_MODE:	  
	  {
            vret[0].type = EZ_INT;
	    switch(type)
	      {
              case EZ_WIDGET_SPREAD_SHEET:
		vret[0].value.i=EZ_SSheetHMode(widget);
                break;
	      case EZ_WIDGET_ITREE:
		vret[0].value.i=EZ_ITreeWHighlightMode(widget);
		break;
	      case EZ_WIDGET_TREE:
		vret[0].value.i=EZ_ITreeWHighlightMode(EZ_TreeWITree(widget));
		break;
	      case EZ_WIDGET_IFANCY_LIST_BOX:
		vret[0].value.i=EZ_IFListBoxHighlightMode(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		vret[0].value.i=EZ_IFListBoxHighlightMode(EZ_FListBoxIFListBox(widget));
		break;
	      case EZ_WIDGET_IWORK_AREA:
		vret[0].value.i=EZ_IWorkAreaHighlightMode(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		vret[0].value.i=EZ_IWorkAreaHighlightMode(EZ_WorkAreaIWorkArea(widget));
		break;
	      default:
                return(-1);
		break;
	      }
            return(1);
	  }
          break;
	case EZ_RUBBER_BAND:
	  {
            vret[0].type = EZ_INT;
	    switch(type)
	      {
	      case EZ_WIDGET_ITREE:
		vret[0].value.i=EZ_ITreeWRubberBand(widget);
		break;
	      case EZ_WIDGET_TREE:
		vret[0].value.i=EZ_ITreeWRubberBand(EZ_TreeWITree(widget));
		break;
	      case EZ_WIDGET_IFANCY_LIST_BOX:
		vret[0].value.i=EZ_IFListBoxRubberBand(widget);
		break;
	      case EZ_WIDGET_FANCY_LIST_BOX:
		vret[0].value.i=EZ_IFListBoxRubberBand(EZ_FListBoxIFListBox(widget));
		break;
	      case EZ_WIDGET_IWORK_AREA:
		vret[0].value.i=EZ_IWorkAreaRubberBand(widget);
		break;
	      case EZ_WIDGET_WORK_AREA:
		vret[0].value.i=EZ_IWorkAreaRubberBand(EZ_WorkAreaIWorkArea(widget));
		break;
	      default:
                return(-1);
		break;
	      }	  
            return(1);
	  }
          break;
	case 0: return(0); break;
	default:
	  break;
	}
    }
  return(-1);
}
/*************************************************************************************/
char *EZ_GetOptionSymbolicName(i) int i;
{
  if(i>0 && i <= EZ_WIDGET_CONFIGURE_LAST_OPTION)
    return(EZ_ConfigureOptionNames[i]);
  return(NULL);
}
/*************************************************************************************/
#undef _EZ_WIDGET_CONFIGURE_C_
