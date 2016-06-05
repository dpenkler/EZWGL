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
 ***              Managing Resources                           ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_RESOURCEGET_C_
#include "EZ_Widget.h"

/*****************************************************************/
int  EZ_GetAndUseWidgetResources   MY_ANSIARGS((EZ_Widget *widget));
int  EZ_GetAndUseWidgetResources1  MY_ANSIARGS((EZ_Widget *widget));
int  EZ_GetAndUseItemResources     MY_ANSIARGS((EZ_Item *item));
int  EZ_DecodeApplicationResources  MY_ANSIARGS((void));
int  EZ_SetOneWidgetResource MY_ANSIARGS((EZ_Widget *widget, char *resname, char *resvalue));
int  EZ_ParseWidgetResourceString MY_ANSIARGS((EZ_Widget *widget, char *resString));
int  EZ_ParseItemResourceString MY_ANSIARGS((EZ_Item *item, char *resString));
/****************************************************************/
int  EZ_GetAndUseWidgetResourcesWithDB MY_ANSIARGS((EZ_Widget *widget, XrmDatabase db));
int  EZ_GetAndUseWidgetResources1WithDB MY_ANSIARGS((EZ_Widget *widget, XrmDatabase db));
int  EZ_GetAndUseItemResourcesWithDB     MY_ANSIARGS((EZ_Item *item,  XrmDatabase db));
int  EZ_DecodeApplicationResourcesWithDB  MY_ANSIARGS((XrmDatabase db));
/****************************************************************/
static int  EZ_GetSetWidgetResources   MY_ANSIARGS((EZ_Widget *widget, int lev, 
                                                     XrmDatabase db));
static int  EZ_GetSetWidgetResourcesWork   MY_ANSIARGS((EZ_Widget *widget,
                                                         XrmDatabase db, int flag));
static int  EZ_GetAndUseItemResourcesWithDBWork MY_ANSIARGS((EZ_Item *item,  XrmDatabase db, int pflag));
/****************************************************************/
typedef struct lex_unit_
{	
  int  type;    
  char *start;
  int  length; 
  union { int i; float  f; } v;
} lex_unit;

#define  STR_CST   0
#define  INT_CST   1
#define  FLOAT_CST 2
static lex_unit tokens[128];
/****************************************************************/
static int scan_number MY_ANSIARGS((char *str, int tidx));
static int  parseString(str_) char *str_;
{
  register int idx_, c;
  register int tidx;

  for(idx_ = tidx = 0; tidx < 128 && str_[idx_] != '\0'; idx_++) 
    {
      if(isspace(str_[idx_]))  continue;

      tokens[tidx].type = STR_CST;
      tokens[tidx].start = str_ + idx_;
      tokens[tidx].length = 1;

      if( (isdigit(str_[idx_]) || str_[idx_] == '.' ) ||
	       (str_[idx_] == '-' && (isdigit(str_[idx_+1]) || str_[idx_+1] == '.' )))
	{
	  tokens[tidx].length = scan_number(&str_[idx_], tidx);
	  idx_ += (tokens[tidx].length - 1);
	}
      else if(str_[idx_] == '\"')
        {
          idx_ ++;
          tokens[tidx].length++;
          c = str_[idx_];  
          while(c && c != '\"')
            {
              tokens[tidx].length++;
              idx_ ++;
              c = str_[idx_];  
	    }
        }
      else
	{
	  c = str_[idx_];  
	  while(c && !isspace(c))
	    {
	      tokens[tidx].length++;
	      idx_ ++;
	      c = str_[idx_];  
	    }
	  tokens[tidx].length--;
	}
      if(tokens[tidx].length > 0) tidx++;
    }
  return(tidx);
}
/***************************************************************************/
static int scan_number(str, tidx)   
     char *str; int tidx;
{
  register int count = 0;

  tokens[tidx].type = INT_CST;
  if(str[0] == '-') count++;
  while(isdigit(str[count])) count++;
  if(str[count] == '.')
    {
      tokens[tidx].type = FLOAT_CST;
      while (isdigit(str[++count])) ;
    }
  if(str[count] == 'e' || str[count] == 'E') 
    {
      tokens[tidx].type = FLOAT_CST;
      if(str[++count] == '-') count++;
      if(!isdigit(str[count])) 
	{
	  tokens[tidx].type = STR_CST;
	  while( str[count] && !isspace(str[count])) count++;
	}
      else while(isdigit(str[++count]));
    }
  if(tokens[tidx].type == INT_CST) 
    tokens[tidx].v.i = atoi(str);
  else if(tokens[tidx].type == FLOAT_CST)
    tokens[tidx].v.f = (float) atof(str);

  return(count);
}
/****************************************************************/
static char *copyToken(idx, ret)
     int idx; char *ret;
{
  char *src = tokens[idx].start;
  char *tgt = ret;
  int len = tokens[idx].length;
  while(len > 0)
    {
      *tgt++ = *src++;
      len--;
    }
  *tgt = 0;
  return(ret);
}
/****************************************************************/
static int getTokenIntValue(idx, v_ret)
     int idx, *v_ret;
{
  if(tokens[idx].type == STR_CST)
    {
      char tname[1024];
      if(EZ_RetrieveSymbolicInt(copyToken(idx,tname), v_ret) < 0)
	return(-1);
    }
  else if(tokens[idx].type == INT_CST)
    *v_ret = tokens[idx].v.i;
  else return(-1);
  return(0);
}

static int getTokenFloatValue(idx, v_ret)
     int idx; float *v_ret;
{
  if(tokens[idx].type == STR_CST) 
    {
      char tname[1024];
      if(EZ_RetrieveSymbolicFloat(copyToken(idx,tname), v_ret) < 0)
      return(-1);
    }
  else if(tokens[idx].type == INT_CST)
    *v_ret = (float) tokens[idx].v.i;
  else *v_ret = tokens[idx].v.f;
  return(0);
}
/****************************************************************/
int silenceOnConfigError = 0;
int  EZ_GetAndUseWidgetResources(widget) EZ_Widget *widget;
{
  return(EZ_GetAndUseWidgetResourcesWithDB(widget, EZ_Xrm_db));
}

int EZ_GetAndUseWidgetResourcesWithDB(widget, db)
     EZ_Widget *widget; XrmDatabase db;
{
  int retV = 0;
  if(widget)
    {
      silenceOnConfigError = 1;
      retV |= EZ_GetSetWidgetResources(widget, 0, db);
      silenceOnConfigError = 0;
      if(EZ_WidgetType(widget) == EZ_WIDGET_LED)
	EZ_LEDStatus(widget) &=  ~LED_BLOCKED;
    }
  return(retV);
}

int EZ_GetAndUseWidgetResources1(widget) EZ_Widget *widget;
{
  return(EZ_GetAndUseWidgetResources1WithDB(widget, EZ_Xrm_db)) ;
}

int EZ_GetAndUseWidgetResources1WithDB(widget, db) 
     EZ_Widget *widget; XrmDatabase db;
{
  int retV = 0;
  if(widget)
    {
      silenceOnConfigError = 1;
      retV |= EZ_GetSetWidgetResourcesWork(widget, db, 1);
      silenceOnConfigError = 0;
      if(EZ_WidgetType(widget) == EZ_WIDGET_LED)
	EZ_LEDStatus(widget) &=  ~LED_BLOCKED;
    }
  return(retV);
}

int EZ_SetOneWidgetResource(widget, resname, resvalue)
     EZ_Widget *widget; char *resname, *resvalue;
{
  char spec[1024], *path = NULL;
  int retV = 0;
  if(EZ_LookupWidgetFromAllHT(widget) == NULL || resname == NULL) return(0);
  EZ_GetWidgetFullPath(widget, &path, NULL);
  if(path)
    {
      XrmDatabase dspDb;
      sprintf(spec, "%s.%s: %s", path, resname, resvalue);
      if( (dspDb = XrmGetStringDatabase(spec)) != NULL )
        {
          retV = EZ_GetSetWidgetResources(widget, 0, dspDb);
          if(EZ_WidgetType(widget) == EZ_WIDGET_LED)
            EZ_LEDStatus(widget) &=  ~LED_BLOCKED;
          if(retV)
            {
              EZ_ClearWidgetSizeComputedFlag(widget);
              if(EZ_WidgetIsViewable(widget)) EZ_DisplayWidget(widget);
            }
          XrmDestroyDatabase(dspDb);          
        } 
      (void)my_free(path);
    }
  return(retV);
}
/**************************************************************************/

static int EZ_GetSetWidgetResources(widget, level, db)
  EZ_Widget *widget; int level; XrmDatabase db;
{
  int retValue = 0;
  if(widget)
    {
      EZ_Widget *children, *sibling;

      retValue |= EZ_GetSetWidgetResourcesWork(widget, db, 1);
      children = EZ_WidgetChildren(widget);
      retValue |= EZ_GetSetWidgetResources(children, 1, db);
      if(level)
	{
	  sibling = EZ_WidgetSibling(widget);
	  retValue |= EZ_GetSetWidgetResources(sibling, 1, db);	  
	}
      if(EZ_WidgetType(widget) == EZ_WIDGET_OPTIONAL_ENTRY)
	retValue |= EZ_GetSetWidgetResources(EZ_OptEntryPopup(widget), 0, db);
      else if(EZ_WidgetType(widget) == EZ_WIDGET_NB_PAGE)
        retValue |= EZ_GetSetWidgetResources(EZ_NBPageCompanion(widget), 0, db);
    }
  return(retValue);
}
/****************************************************************/
int EZ_ParseWidgetResourceString(widget, resString)
     EZ_Widget *widget; char *resString;
{
  XrmDatabase dspDb;
  if( widget && resString && (dspDb = XrmGetStringDatabase(resString)) != NULL )
    {
      int retV = EZ_GetSetWidgetResourcesWork(widget, dspDb, 0);
      if(EZ_WidgetType(widget) == EZ_WIDGET_LED) EZ_LEDStatus(widget) &=  ~LED_BLOCKED;
      if(retV)
        {
          EZ_ClearWidgetSizeComputedFlag(widget);
          if(EZ_WidgetIsViewable(widget)) EZ_DisplayWidget(widget);
        }
      /*
      {
        XrmName names[2];
        XrmClass classes[2];
        extern enumdb();
        names[0] = NULLQUARK; names[1] = NULLQUARK;
        classes[0] = NULLQUARK; classes[1]= NULLQUARK;
        XrmEnumerateDatabase(dspDb, names, classes, XrmEnumAllLevels, enumdb, 
                             (XPointer)NULL);
      }
      */
      XrmDestroyDatabase(dspDb);          
      return(retV);
    } 
  return(0);
}
/****************************************************************/
static int EZ_GetSetWidgetResourcesWork(widget,db, pflag)
     EZ_Widget *widget; XrmDatabase db; int pflag; /* nonzero for full path */
{
  EZ_Widget *parent;
  XrmQuark  clist[128], nlist[128], Q_type[2];
  int i, j, position, doitnot, ntokens, retValue = 0;
  XrmValue value;
  /* vars needed for EZ_SetWidgetAttributes */
  int       i0,i1,i2,i3,i4;
  void      *p1, *p2;
  float     f1, f2;
  unsigned long ul1, ul2;
  char tmpstr[512];
  
  if(EZ_LookupWidgetFromAllHT(widget) == (EZ_Widget *)NULL) return(0);

  if(pflag)
    {
      parent = widget;
      i = 127;
      while(parent)
        {
          clist[i] = EZ_WidgetCName(parent);
          nlist[i] = EZ_WidgetIName(parent);	  
          parent = EZ_WidgetParent(parent);
          i--;
        }
      clist[0] = EZ_ClassQName;
      nlist[0] = EZ_InstanceQName;
      for(j = 1, i = i+1; i < 128; i++, j++)
        {
          clist[j] = clist[i];
          nlist[j] = nlist[i];
        }
    }
  else j = 0;

  clist[j] = NULLQUARK; clist[j+1] = NULLQUARK;
  nlist[j] = NULLQUARK; nlist[j+1] = NULLQUARK;
  position = j;
  /* path set */
  
  for(i = 1; i <= EZ_WIDGET_CONFIGURE_LAST_OPTION; i++)
    {
      if(EZ_OptionsNQs[i] != NULLQUARK)
	{
	  clist[position] = EZ_OptionsCQs[i];
	  nlist[position] = EZ_OptionsNQs[i];
	  value.addr = NULL;
	  if(XrmQGetResource(db, nlist, clist, Q_type, &value) == True)
	    {
	      i1=i2=i3=i4=0;
	      ul1 = ul2 = 0L;
	      p1 = p2 = NULL;
	      f1 = f2 = 0.0;
	      doitnot = 0;
              retValue++;
	      /* now we have to process string value.attr */
	      switch(i)
		{
		case EZ_DND_BUBBLE_STRING:
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
		case EZ_BUBBLE_STRING:
		case EZ_LABEL_STRING:
		case EZ_GLOB_PATTERN:
		case EZ_ENTRY_STRING:
		case EZ_NAME:
		case EZ_CLASS:
		case EZ_LCD_BACKGROUND:
		case EZ_LCD_FOREGROUND:
		case EZ_LED_BACKGROUND:
		case EZ_SMETER_BACKGROUND:
		case EZ_SMETER_FOREGROUND:
		case EZ_POPUP_NAME:
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
		  p1 = value.addr;
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
                case EZ_RULER_TICK_SCALE:
                case EZ_RULER_TICK_OFFSET:
                case EZ_RULER_TICK_LENGTH:
                case EZ_RULER_ORIENTATION:
                case EZ_OVERRIDE_REDIRECT:
                case EZ_SCROLL_TYPE:
		case EZ_DIAL_DISPLAY_VALUE:
		case EZ_DIAL_STYLE:
                case EZ_WRAP_TEXT:
                case EZ_BAR_ORIENTATION:
                case EZ_BAR_EDITABLE:
                case EZ_BAR_WIDTH:
                case EZ_HISTOGRAM_SAMPLE:
                case EZ_SEPARATOR_STYLE:
                case EZ_ROW_COLUMN_MAX_SIZE:
                  ntokens = parseString(value.addr);
		  if(ntokens > 0) doitnot = getTokenIntValue(0, &i1);
		  break;
		case  EZ_DND_DRAG_CURSOR:
		case  EZ_CURSOR:		  
		  ul1 = (unsigned long)EZ_GetCursorByName(value.addr);
		  if(ul1 == None) doitnot = 1;
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
		case EZ_LED_PIXEL_COLOR:
		case EZ_LED_PIXEL_SIZE:
		case EZ_SHEET_HEADER_SIZE:
		case EZ_SHEET_CELL_SIZE:
		  ntokens = parseString(value.addr);
		  if(ntokens > 1)
		    {
		      doitnot = getTokenIntValue(0, &i1);
		      doitnot += getTokenIntValue(1, &i2);
		    }
		  else doitnot = 1;
		  break; 
                case EZ_TILE_ORIGIN:
		  ntokens = parseString(value.addr);
		  if(ntokens > 2)
		    {
		      doitnot = getTokenIntValue(0, &i1);
		      doitnot += getTokenIntValue(1, &i2);
		      doitnot += getTokenIntValue(2, &i3);
		    }
		  else doitnot = 1;
                  break;
		case EZ_GRID_CELL_GEOMETRY:
		case EZ_GRID_ROW_CONSTRAINS:
		case EZ_GRID_COLUMN_CONSTRAINS:
		  ntokens = parseString(value.addr);
		  if(ntokens > 3)
		    {
                      int jj;
                      for(jj = 0; jj < ntokens; jj += 4)
                        {
                          doitnot =  getTokenIntValue(jj,   &i1);
                          doitnot += getTokenIntValue(jj+1, &i2);
                          doitnot += getTokenIntValue(jj+2, &i3);
                          doitnot += getTokenIntValue(jj+3, &i4);
                          if(!doitnot)
			    retValue |= 
                              EZ_SetWidgetAttributes(widget,i,i1,i2, p1,p2,ul1,ul2,f1,f2, i3,i4);
                        }
		    }
		  doitnot = 1;
		  break;
		case EZ_GEOMETRY:
		  {
                    int bits=0;
                    EZ_ParseGeometry(value.addr, &bits, &i1,&i2,&i3,&i4);
                    if(bits != 15 /*(1|2|4|8)*/) /* all bits must be set */ 
                      {
                        ntokens = parseString(value.addr);
                        if(ntokens > 3)
                          {
                            doitnot =  getTokenIntValue(0, &i1);
                            doitnot += getTokenIntValue(1, &i2);
                            doitnot += getTokenIntValue(2, &i3);
                            doitnot += getTokenIntValue(3, &i4);
                          }
                        else doitnot = 1;
                      }
                  }
		break;
		case EZ_GRID_CONSTRAINS:		  
		  ntokens = parseString(value.addr);
		  if(ntokens > 4)
		    {
                      int jj;
                      for(jj =0; jj < ntokens; jj += 5)
                        {
                          doitnot =  getTokenIntValue(jj,   &i0);
                          doitnot += getTokenIntValue(jj+1, &i1);
                          doitnot += getTokenIntValue(jj+2, &i2);
                          doitnot += getTokenIntValue(jj+3, &i3);
                          doitnot += getTokenIntValue(jj+4, &i4);
                          ul1 = (unsigned long)i0;
                          if(!doitnot)
			    retValue |= 
                              EZ_SetWidgetAttributes(widget,i,i1,i2, p1,p2,ul1,ul2,f1,f2, i3,i4);
                        }
                    }
                  doitnot = 1;
		  break;
		case EZ_SLIDER_RESOLUTION: 
		case EZ_SLIDER_INIT_VALUE:		
		case EZ_SMETER_VALUE:
		case EZ_DIAL_VALUE:
                case EZ_DIAL_SPAN:
                case EZ_DIAL_RESOLUTION:
		  ntokens = parseString(value.addr);
		  if(ntokens > 0)
		    doitnot =  getTokenFloatValue(0, &f1);
		  else doitnot = 1;
		  break;
		case EZ_SMETER_VALUE_N:
		  ntokens = parseString(value.addr);
		  if(ntokens > 1)
		    {
		      int jj;
		      for(jj = 0; jj < ntokens; jj += 2)
			{
			  doitnot =  getTokenIntValue(jj, &i1);
			  doitnot +=  getTokenFloatValue(jj+1, &f1);		      
			  if(!doitnot)
			    retValue |= 
                              EZ_SetWidgetAttributes(widget,i,i1,i2, p1,p2,ul1,ul2,f1,f2, i3,i4);
			}
		    }
		  doitnot = 1;		  
		  break;
		case EZ_SMETER_FOREGROUND_N:
                case EZ_TERM_COLOR_N:
                case EZ_SPIN_VALUE:
                case EZ_BAR_COLOR_N:
		  ntokens = parseString(value.addr);
		  if(ntokens > 1)
		    {
		      int jj;
		      for(jj = 0; jj < ntokens; jj += 2)
			{
			  doitnot =  getTokenIntValue(jj, &i1);
			  copyToken(jj+1, tmpstr);
			  p1 = tmpstr;
			  if(!doitnot)
			    retValue |= 
                              EZ_SetWidgetAttributes(widget,i,i1,i2, p1,p2,ul1,ul2,f1,f2, i3,i4);
			}
		    }
		  doitnot = 1;
		  break;
		case EZ_SLIDER_RANGE: 
		case EZ_SMETER_RANGE:
                case EZ_LOCATOR_XY:
                case EZ_DIAL_RANGE:
                case EZ_BAR_RANGE:
		  ntokens = parseString(value.addr);
		  if(ntokens > 1)
		    {
		      doitnot =  getTokenFloatValue(0, &f1);
		      doitnot += getTokenFloatValue(1, &f2);
		    }
		  else doitnot = 1;
		  break;
		case EZ_SMETER_RANGE_N:
		  ntokens = parseString(value.addr);
		  if(ntokens > 2)
		    {
		      int jj;
		      for(jj = 0; jj < ntokens; jj += 3)
			{
			  doitnot =  getTokenIntValue(jj, &i1);
			  doitnot +=  getTokenFloatValue(jj+1, &f1);
			  doitnot += getTokenFloatValue(jj+2, &f2);
			  if(!doitnot)
			    retValue |= 
                              EZ_SetWidgetAttributes(widget,i,i1,i2, p1,p2,ul1,ul2,f1,f2, i3,i4);
			}
		    } 
		  doitnot = 1;
		  break;
		case EZ_ROW_BG:
		  ntokens = parseString(value.addr);
		  if(ntokens > 2)
		    {
		      char clr1[256], clr2[256];
		      copyToken(1, clr1);
		      copyToken(2, clr2);
		      doitnot =  getTokenIntValue(0, &i1);
		      if(!doitnot)
			{
			  p1 = clr1;
			  p2 = clr2;
                          retValue |= 
                            EZ_SetWidgetAttributes(widget,i,i1,i2, p1,p2,ul1,ul2,f1,f2, i3,i4);
			}
		    }
		  doitnot = 1;
		  break;
		default:
                  doitnot = 1;
		  break;
		}
	      if(doitnot == 0)
                retValue |= 
                  EZ_SetWidgetAttributes(widget,i,i1,i2, p1,p2,ul1,ul2,f1,f2, i3,i4);
	    }
	}
    }
  {
    EZ_ResourceHandle *rhandle = EZ_WidgetResourceHandle(widget);
    while(rhandle)
      {
        int i, nn = rhandle->nresources;
        UDFResource *resources = rhandle->resources;
        for(i = 0; i < nn; i++)
          {
            EZ_Value values[64];
            char str[4][1024];

            clist[position] = resources[i].cname;
            nlist[position] = resources[i].iname;
            value.addr = NULL;
            if(XrmQGetResource(db, nlist, clist, Q_type, &value) == True)
              {
                unsigned char *vtypes = resources[i].vtypes;
                int jj, ii, mm = (int)vtypes[0];
                int option = resources[i].option;
                int repeat = (vtypes[mm] == EZ_REPEAT ? 1: 0);
                doitnot = 0;
                retValue++;
                ntokens = parseString(value.addr);
                if(ntokens >= mm)
                  {
                    for(jj = 0; jj < ntokens && repeat >= 0; jj += mm, repeat--)
                      {
                        for(ii = 0; ii < mm; ii++)
                          {
                            values[ii].type = (int)(vtypes[ii+1]);
                            switch(vtypes[ii+1])
                              {
                              case EZ_BOOL:
                              case EZ_INT:
                              case EZ_ENUM:
                              case EZ_SHORT:
                              case EZ_ULONG:
                                doitnot += getTokenIntValue(jj+ii, &(values[ii].value.i));
                                break;
                              case EZ_POINTER: /* has to be string !! */
                              case EZ_STRING:
                              case EZ_FONTNAME:
                              case EZ_FILENAME:
                              case EZ_COLOR:
                                if((jj|ii) == 0) values[ii].value.p = value.addr;
                                else  /* dangerous !!! */
                                  {
                                    copyToken(jj+ii, str[ii%4]);
                                    values[ii].value.p = str[ii%4];
                                  }
                                break;
                              case EZ_FLOAT:
                                doitnot += getTokenFloatValue(jj+ii, &(values[ii].value.f));
                                break;
                              default:
                                break;
                              }
                          }
                        if(!doitnot)
                          retValue |= EZ_SetWidgetUDFAttributes(widget, option, values);
                      }
                  }  
              }
          }
        rhandle = rhandle->next;
      }
  }

  return(retValue);
}
/*****************************************************************/
int  EZ_GetAndUseItemResources(item) EZ_Item *item;
{
  return(EZ_GetAndUseItemResourcesWithDB(item, EZ_Xrm_db));
}

int  EZ_GetAndUseItemResourcesWithDB(item, db) 
     EZ_Item *item; XrmDatabase db;
{
  return(EZ_GetAndUseItemResourcesWithDBWork(item, db, 1));
}

int EZ_ParseItemResourceString(item, resString)
     EZ_Item *item; char *resString;
{
  XrmDatabase dspDb;
  if( item && resString && (dspDb = XrmGetStringDatabase(resString)) != NULL )
    {
      int retV = EZ_GetAndUseItemResourcesWithDBWork(item, dspDb, 0);
      XrmDestroyDatabase(dspDb);          
      return(retV);
    } 
  return(0);
}
/*****************************************************************/
static int  EZ_GetAndUseItemResourcesWithDBWork(item, db, pflag) 
     EZ_Item *item; XrmDatabase db; int pflag;

{
  if(item)
    {
      XrmQuark  clist[10], nlist[10], Q_type[2];
      int j, i, position, doitnot, ntokens, retValue=0;
      XrmValue value;
      /* vars needed for EZ_SetItemAttributes */
      int       i1,i2,i3,i4;
      void      *p1, *p2;
      float     f1, f2;
      unsigned long ul1, ul2;

      if(EZ_LookupItemFromHT(item) == NULL) return(0);
      if(pflag) 
        {
          clist[0] = EZ_ClassQName;
          nlist[0] = EZ_InstanceQName;
          j = 1;
        }
      else j = 0;

      clist[j] = EZ_ItemCName(item);
      nlist[j] = EZ_ItemIName(item);	  
      clist[j+1] = NULLQUARK; clist[j+2] = NULLQUARK;
      nlist[j+1] = NULLQUARK; nlist[j+2] = NULLQUARK;
      position = j+1;
      
      silenceOnConfigError = 1;
      /* path set */
      for(i = 1; i <= EZ_WIDGET_CONFIGURE_LAST_OPTION; i++)
        {
          if(EZ_OptionsNQs[i] != NULLQUARK)
            {
              clist[position] = EZ_OptionsCQs[i];
              nlist[position] = EZ_OptionsNQs[i];
              value.addr = NULL;
              if(XrmQGetResource(db, nlist, clist, Q_type, &value) == True)
                {
                  retValue++;
                  i1=i2=i3=i4=0;
                  ul1 = ul2 = 0L;
                  p1 = p2 = NULL;
                  f1 = f2 = 0.0;
                  doitnot = 0;
                  /* now we have to process string value.attr */
                  switch(i)
                    {
                    case EZ_DND_BUBBLE_STRING:
                    case EZ_FOREGROUND:
                    case EZ_BACKGROUND:
                    case EZ_BITMAP_FILE:
                    case EZ_IMAGE_FILE:
                    case EZ_FONT_NAME:
                    case EZ_LABEL_STRING:
                    case EZ_NAME:
                    case EZ_CLASS:
                    case EZ_RESOURCE_STRING:
                      p1 = value.addr;
                      break;
                    case EZ_X:
                    case EZ_Y:
                    case EZ_WIDTH:
                    case EZ_HEIGHT:
                    case EZ_BORDER_WIDTH:
                    case EZ_BORDER_TYPE:
                    case EZ_PADX:
                    case EZ_PADY:
                    case EZ_IPADX:
                    case EZ_IPADY:
                    case EZ_STACKING:
                    case EZ_LABEL_POSITION:
                    case EZ_JUSTIFICATION:
                    case EZ_FONT_ID:
                    case EZ_TEXT_LINE_LENGTH:
                    case EZ_ATTACH_INT_DATA:
                      ntokens = parseString(value.addr);
                      if(ntokens > 0) doitnot = getTokenIntValue(0, &i1);
                      break;
                    case  EZ_DND_DRAG_CURSOR:
                      ul1 = (unsigned long)EZ_GetCursorByName(value.addr);
                      if(ul1 == None) doitnot = 1;
                      break;
                    case EZ_LOCATION:
                    case EZ_SIZE:
                    case EZ_LABEL_SHADOW:
                      ntokens = parseString(value.addr);
                      if(ntokens > 1)
                        {
                          doitnot = getTokenIntValue(0, &i1);
                          doitnot += getTokenIntValue(1, &i2);
                        }
                      else doitnot = 1;
                      break; 
                    case EZ_GEOMETRY:
                      {
                        int bits=0;
                        EZ_ParseGeometry(value.addr, &bits, &i1,&i2,&i3,&i4);
                        if(bits != 15 /*(1|2|4|8)*/) /* all bits must be set */ 
                          {
                            ntokens = parseString(value.addr);
                            if(ntokens > 3)
                              {
                                doitnot =  getTokenIntValue(0, &i1);
                                doitnot += getTokenIntValue(1, &i2);
                                doitnot += getTokenIntValue(2, &i3);
                                doitnot += getTokenIntValue(3, &i4);
                              }
                            else doitnot = 1;
                          }
                      }
                    break;
                    default:
                      doitnot = 1;
                      break;
                    }
                  if(doitnot == 0)
                    EZ_SetItemAttributes(item, i, i1, i2, p1, p2, ul1, ul2, i3, i4);
                }
            }
        }
      silenceOnConfigError = 0;
      return(retValue);
    }
  return(0);
}
/**********************************************************************/
void EZ_RegisterApplicationResources(n, spec, configure)
     int n; EZ_ResourceSpec *spec;
     int (*configure) MY_ANSIARGS((void *nptr, int opt, EZ_Value *values));
{
  if(n > 0 && spec && configure)
    {
      EZ_ResourceHandle *ptr, *tmp;

      tmp = (EZ_ResourceHandle *) my_malloc( sizeof(EZ_ResourceHandle),_MISC_DATA_);
      memset(tmp, 0, sizeof(EZ_ResourceHandle));
      ptr = EZ_ApplicationResourceHandle;
      if(ptr != NULL)
        {
          while(ptr->next != NULL) ptr = ptr->next;
          ptr->next = tmp;
        }
      else EZ_ApplicationResourceHandle = tmp;
      EZ_RecordResourceSpec(tmp, n, spec, (void *)configure);
      EZ_DecodeApplicationResources();
    }
}

int EZ_DecodeApplicationResources()
{
  return(EZ_DecodeApplicationResourcesWithDB(EZ_Xrm_db));
}

int EZ_DecodeApplicationResourcesWithDB(db)
     XrmDatabase db;
{
  EZ_ResourceHandle *rhandle, *tmp = EZ_ApplicationResourceHandle;
  int retValue = 0;
  if(tmp)
    {
      XrmQuark  inameQ[3], cnameQ[3], Q_type[3];/* not used */
      XrmValue value;
      inameQ[0] = EZ_InstanceQName;
      cnameQ[0] = EZ_ClassQName;
      inameQ[2] = cnameQ[2] = NULLQUARK;
      rhandle = tmp;

      while(rhandle)
        {
          int i, nn = rhandle->nresources;
          UDFResource *resources = rhandle->resources;
          for(i = 0; i < nn; i++)
            {
              cnameQ[1] = resources[i].cname;
              inameQ[1] = resources[i].iname;
              if(XrmQGetResource(db, inameQ, cnameQ, Q_type, &value) == True)
                {
                  EZ_Value values[32];
                  char str[4][1024];
                  int  repeat, doitnot, ntokens, ii, jj, mm, option = resources[i].option;
                  unsigned char *vtypes = resources[i].vtypes;

                  retValue++;
                  mm = (int)vtypes[0];
                  repeat = (vtypes[mm] == EZ_REPEAT? 1 : 0);
                  ntokens = parseString(value.addr);
                  doitnot = 0;
                  if(ntokens >= mm)
                    {
                      for(jj = 0; jj < ntokens && repeat >= 0; jj+=mm, repeat--)
                        {
                          for(ii = 0; ii < mm; ii++)
                            {
                              values[ii].type = (int)(vtypes[ii+1]);
                              switch(vtypes[ii+1])
                                {
                                case EZ_BOOL:
                                case EZ_INT:
                                case EZ_ENUM:
                                case EZ_SHORT:
                                case EZ_ULONG:
                                  doitnot += getTokenIntValue(jj+ii, &(values[ii].value.i));
                                  break;
                                case EZ_POINTER: /* has to be string !! */
                                case EZ_STRING:
                                case EZ_FONTNAME:
                                case EZ_FILENAME:
                                case EZ_COLOR:
                                  if((jj|ii) == 0) values[ii].value.p = value.addr;
                                  else  /* dangerous !!! */
                                    {
                                      copyToken(jj+ii, str[ii%4]);
                                      values[ii].value.p = str[ii%4];
                                    }
                                  break;
                                case EZ_FLOAT:
                                  doitnot += getTokenFloatValue(jj+ii, &(values[ii].value.f));
                                  break;
                                default:
                                  break;
                                }
                            }
                          if(!doitnot)
                            {
                              if(rhandle->configure)
                                (rhandle->configure)(NULL, option,values);
                            }
                        }
                    }
                }
            }
          rhandle = rhandle->next;          
        }
    }
  return(retValue);
}
/*****************************************************************/
#undef  STR_ID   
#undef  INT_CST  
#undef  FLOAT_CST
/*****************************************************************/
#undef  _EZ_RESOURCEGET_C_
