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
 *  April 1997.  Release Version 1.1
 *  May 1997. Handling of raw rgb images added by Arno Dabekaussen.
 */
{
  option = va_arg(ap, int);

  while(!done)
    {
      i1=i2=i3=i4=0;
      ul1 = ul2 = 0L;
      p1 = p2 = NULL;
      f1 = f2 = 0.0;

      switch(option)
	{
	case EZ_DND_BUBBLE_HELP:
	case EZ_SELECTION_BACKGROUND:
	case EZ_SELECTION_FOREGROUND:
	case EZ_INDICATOR_COLOR:
	case EZ_FOREGROUND:
	case EZ_TEXT_BACKGROUND:
	case EZ_BACKGROUND:
	case EZ_HIGHLIGHT_BG:
        case EZ_HIGHLIGHT_FG:
	case EZ_BITMAP_FILE:
	case EZ_IMAGE_FILE:
	case EZ_FONT_NAME:
	case EZ_BUTTON_SHORTCUT:
	case EZ_BUBBLE_HELP:
	case EZ_LABEL_STRING:
        case EZ_GLOB_PATTERN:
	case EZ_ENTRY_STRING:
	case EZ_NAME:
	case EZ_CLASS:
	case EZ_LCD_BACKGROUND:
	case EZ_LCD_FOREGROUND:
	case EZ_LED_BACKGROUND:
	case EZ_LED_PIXEL_COLOR:
	case EZ_POPUP_NAME:
	case EZ_SMETER_BACKGROUND:
	case EZ_SMETER_FOREGROUND:
	case EZ_WM_WINDOW_NAME:
	case EZ_WM_ICON_NAME:
	case EZ_WM_ICON_XPM_FILE:
	case EZ_BG_IMAGE_FILE:
	case EZ_BG_IMAGE_FILE_B:
        case EZ_TERM_CURSOR_COLOR:
        case EZ_DIAL_COLOR:
        case EZ_CURSOR_NAME:
        case EZ_DND_DRAG_CURSOR_NAME:
        case EZ_SHAPE_FILE:
        case EZ_RESOURCE_STRING:
	  p1  = (void *)va_arg(ap, char *);
	  break;
	case EZ_SMETER_FOREGROUND_N:
        case EZ_TERM_COLOR_N:
        case EZ_SPIN_VALUE:
        case EZ_BAR_COLOR_N:
	  i1 = va_arg(ap, int);
	  p1  = (void *)va_arg(ap, char *);
	  break;
	case EZ_X:
	case EZ_Y:
	case EZ_SHAPED_WINDOW:
	case EZ_WIDTH:
	case EZ_WIDTH_HINT:
	case EZ_HEIGHT:
	case EZ_HEIGHT_HINT:
	case EZ_INDICATOR_TYPE:
	case EZ_EMBEDER:
	case EZ_BORDER_WIDTH:
	case EZ_BORDER_TYPE:
	case EZ_PADX:
	case EZ_PADY:
	case EZ_IPADX:
	case EZ_IPADY:
	case EZ_STACKING:
	case EZ_SIDE:
	case EZ_LABEL_POSITION:
	case EZ_JUSTIFICATION:
	case EZ_FILL_MODE:
	case EZ_TEXT_SPACING:
	case EZ_FONT_ID:
	case EZ_SLIDER_DISPLAY_VALUE:
	case EZ_SLIDER_LENGTH:
	case EZ_SLIDER_WIDTH:
	case EZ_SLIDER_BORDER_WIDTH:
	case EZ_TEXT_LINE_LENGTH:
	case EZ_INDICATOR_SIZE_ADJUST:
	case EZ_MOVABLE:
	case EZ_RESERVE_MENU_BUTTON:
	case EZ_BACKING_STORE:
	case EZ_EXPAND:
	case EZ_TRANSIENT:
	case EZ_SCROLLBAR_WIDTH:
	case EZ_SCROLLBAR_BORDER_WIDTH:
	case EZ_PROPAGATE:
	case EZ_ATTACH_INT_DATA:
	case EZ_UNDERLINE:
	case EZ_CHECK_BUTTON_ON_VALUE:
	case EZ_CHECK_BUTTON_OFF_VALUE:
	case EZ_CHECK_BUTTON_ON_OFF:
	case EZ_RADIO_BUTTON_GROUP:
	case EZ_RADIO_BUTTON_VALUE:
	case EZ_OPTIONAL_HSCROLLBAR:
	case EZ_OPTIONAL_VSCROLLBAR:
	case EZ_FANCY_LIST_BOX_TYPE:
	case EZ_FANCY_LIST_BOX_COLUMNS:
	case EZ_TEXT_WIDGET_EDITABLE:
	case EZ_RETURN_VALUE:
	case EZ_OPTIONAL_ENTRY_EDITABLE:
	case EZ_MENU_TEAR_OFF:
	case EZ_OPTIONAL_ENTRY_REMEMBER_HISTORY:
	case EZ_PADB:
	case EZ_LCD_NDIGITS:
	case EZ_LCD_FONT_SIZE:
	case EZ_LED_WIDTH:
	case EZ_LED_HEIGHT:
	case EZ_WMHINTS:
	case EZ_GEOMETRY_MANAGER:
	case EZ_LABELED_ICON:
	case EZ_HSCROLL_INCREMENT:
	case EZ_VSCROLL_INCREMENT:
	case EZ_SMETER_STOPED:
	case EZ_SMETER_SHOW_VALUE:
	case EZ_SMETER_ORIENTATION:
	case EZ_FORGET_X:
	case EZ_FORGET_Y:
	case EZ_FORGET_W:
	case EZ_FORGET_H:
	case EZ_FORGET_POSITION:
	case EZ_FORGET_SIZE:
	case EZ_FORGET_GEOMETRY:
	case EZ_WM_INITIAL_STATE:
	case EZ_MARK_CLEAN:
	case EZ_HIGHLIGHT_MODE:
	case EZ_RUBBER_BAND:
	case EZ_DRAG_HANDLE:
	case EZ_DOCKABLE:
	case EZ_SLIDER_STYLE:
        case EZ_ARROW_TYPE:
        case EZ_OPTIONAL_ENTRY_STYLE:
        case EZ_WM_IGNORE:
        case EZ_TERM_SAVE_LINES:
        case EZ_TERM_REVERSE_VIDEO:
        case EZ_RULER_DISPLAY_VALUE:
        case EZ_RULER_TICK_UNIT:
        case EZ_RULER_TICK_LENGTH:
        case EZ_RULER_TICK_SCALE:
        case EZ_RULER_TICK_OFFSET:
        case EZ_RULER_ORIENTATION:
        case EZ_OVERRIDE_REDIRECT:
        case EZ_SCROLL_TYPE:
        case EZ_DIAL_STYLE:
        case EZ_DIAL_DISPLAY_VALUE:
        case EZ_WRAP_TEXT:
        case EZ_BAR_EDITABLE:
        case EZ_BAR_ORIENTATION:
        case EZ_BAR_WIDTH:
        case EZ_HISTOGRAM_SAMPLE:
        case EZ_SEPARATOR_STYLE:
        case EZ_ROW_COLUMN_MAX_SIZE:
	  i1 = va_arg(ap, int);
	  break;
	case  EZ_DND_DRAG_CURSOR:
	case  EZ_CURSOR:
	  ul1 = (unsigned long)va_arg(ap,Cursor);
	  break;
	case EZ_GRID_CELL_PLACEMENT:
	case EZ_LOCATION:
	case EZ_SIZE:
	case EZ_SIZE_HINT:
	case EZ_SMETER_STYLE:
	case EZ_WM_ICON_LOCATION:
	case EZ_WM_LOCATION_HINT:
	case EZ_WM_SIZE_HINT:
	case EZ_WM_MAX_SIZE_HINT:
	case EZ_WM_MIN_SIZE_HINT:
        case EZ_INTERIOR_BORDER:
        case EZ_LABEL_SHADOW:
        case EZ_SPECIAL_EFFECT:
        case EZ_SHOW_TICK:
	case EZ_LED_PIXEL_SIZE:
	case EZ_SHEET_HEADER_SIZE:
	case EZ_SHEET_CELL_SIZE:
	  i1 = va_arg(ap,int);
	  i2 = va_arg(ap,int);
	  break;
        case EZ_TILE_ORIGIN:
	  i1 = va_arg(ap,int);
	  i2 = va_arg(ap,int);
	  i3 = va_arg(ap,int);
          break;
	case EZ_GRID_CELL_GEOMETRY:
	case EZ_GRID_ROW_CONSTRAINS:
	case EZ_GRID_COLUMN_CONSTRAINS:
	case EZ_GEOMETRY:
	  i1 = va_arg(ap,int);
	  i2 = va_arg(ap,int);
	  i3 = va_arg(ap,int);
	  i4 = va_arg(ap,int);
	  break;
	case EZ_GRID_CONSTRAINS:
	  ul1 = (unsigned long)va_arg(ap,int);   /* EZ_ROW or EZ_COLUMN */
	  i1 = va_arg(ap,int);                   /* index */
	  i2 = va_arg(ap,int);                   /* minsize */
	  i3 = va_arg(ap,int);                   /* weight */
	  i4 = va_arg(ap, int);                  /* pad    */
	  break;
	case EZ_SLIDER_RESOLUTION: 
	case EZ_SLIDER_INIT_VALUE:
	case EZ_SMETER_VALUE:
	case EZ_DIAL_RESOLUTION: 
	case EZ_DIAL_SPAN:
        case EZ_DIAL_VALUE:
	  {
	    double dvalue =  va_arg(ap, double);
	    f1 = (float)dvalue;
	  }
	  break;
	case EZ_SMETER_VALUE_N:
	  i1 = va_arg(ap,int);
	  {
	    double dvalue =  va_arg(ap, double);
	    f1 = (float)dvalue;
	  }
	  break;	  
	case EZ_SLIDER_RANGE: 
	case EZ_SMETER_RANGE:
        case EZ_LOCATOR_XY:
	case EZ_DIAL_RANGE: 
	case EZ_BAR_RANGE: 
	  {
	    double from, to;
	    from = va_arg(ap, double);
	    to =  va_arg(ap, double);
	    f1 = (float)from;
	    f2 = (float)to;
	  }
	  break;
	case EZ_SMETER_RANGE_N:
	  i1 = va_arg(ap,int);
	  {
	    double from, to;
	    from = va_arg(ap, double);
	    to =  va_arg(ap, double);
	    f1 = (float)from;
	    f2 = (float)to;
	  }
	  break;
	  /* the following options cannot be set by resources yet */
	case EZ_ATTACH_PTR_DATA:
	case EZ_LABEL_PIXMAP:
	case EZ_WM_ICON_PIXMAP:
	case EZ_BG_PIXMAP:
	case EZ_BG_PIXMAP_B:
        case EZ_SHAPE_PIXMAP:
	  p1 =  va_arg(ap, void *);
	  break;
	case EZ_BITMAP_DATA:
	case EZ_RAW_RGB_DATA:
	  p1 = (void *)  va_arg(ap, char *);
	  i1 = va_arg(ap, int);
	  i2 = va_arg(ap, int);
	  break;
	case EZ_PIXMAP_DATA:
	  p1 = (void *)va_arg(ap, char **);
	  break;
	case EZ_X_PIXMAP:
	  ul1 = (unsigned long)va_arg(ap,Pixmap);
	  i1 = va_arg(ap,int);
	  i2 = va_arg(ap,int);
	  i3 = va_arg(ap,int);
	  i4 = va_arg(ap, int);
	  break;
	case EZ_LED_COLOR_FUNCTION :
	  p1 = (void *)va_arg(ap, EZ_LedPixelColorF);	  
	  break;
	case EZ_CALL_BACK:
	case EZ_MOTION_CALLBACK:
	case EZ_DESTROY_CALLBACK:
        case EZ_SPIN_FUNCTION:
	  p1 = (void *)va_arg(ap, EZ_CallBack);
	  p2 = (void *)va_arg(ap, void *);
	  break;
	case EZ_EVENT_HANDLE:
	  p1 = (void *)va_arg(ap, EZ_EventHandler);
	  p2 = va_arg(ap, void *);
	  break;
	case EZ_ROW_BG:
	  i1 = va_arg(ap,int);
	  p1 = (void *)va_arg(ap, char *);
	  p2 = (void *)va_arg(ap, char *);
	  break;
        case EZ_RESOURCES_HANDLE:
          i1 = va_arg(ap,int);
	  p1 = (void *)va_arg(ap, EZ_ResourceSpec *);
	  p2 = (void *)va_arg(ap, void *);
          break;
	case 0:
	  done = 1;
	  break;
	default:
          {
            EZ_ResourceHandle *rhandle = EZ_WidgetResourceHandle(widget);
            unsigned char *vtypes;
            while(rhandle)
              {
                if((vtypes = EZ_GetUDFResourceVTypes(rhandle, option)) != NULL)
                  {
                    int ii = 0;
                    int nv = (int) vtypes[0];
                    EZ_Value values[32];

                    for(ii = 0; ii < nv; ii++)
                      {
                        values[ii].type = (int)vtypes[ii+1];
                        switch(vtypes[ii+1])
                          {
                          case EZ_INT:
                            values[ii].value.i = va_arg(ap,int);
                            break;
                          case EZ_POINTER:
                            values[ii].value.p = (void *)va_arg(ap, char *);
                            break;
                          case EZ_ULONG:
                            values[ii].value.l = (unsigned long)va_arg(ap,Pixmap);
                            break;
                          case EZ_FLOAT:
                            {
                              double dvalue =  va_arg(ap, double);
                              values[ii].value.f= (float) dvalue;
                            }
                          break;
                          default:
                            break;
                          }
                      }
                    size_changed |= EZ_SetWidgetUDFAttributes(widget, option, values);
                    option = 0;
                    break;
                  }
                rhandle = rhandle->next;
              }
            if(option)
              {
                EZ_ConfigureError(widget,option, "Unknown configuration option.");
                done = 1;
              }
          }
          break;
        }
      if(!done) 
	{
	  size_changed |= EZ_SetWidgetAttributes(widget,option,
						 i1,i2, p1,p2,ul1,ul2,f1,f2, i3,i4);
	  option = va_arg(ap, int);
	}
    }
  va_end(ap);
}


