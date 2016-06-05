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
 /**********************************************************************
  *
  * X Cursor interface
  *
  *********************************************************************/
#define _EZ_CURSOR_C_
#include "EZ_Widget.h"

Cursor               EZ_CreateCursorFromBitmapFile MY_ANSIARGS((char *f1, char *f2,
								char *fc, char *bc,
								int  x,   int y,
								char *name,
								int *idx));
Cursor               EZ_CreateCursorFromBitmapData MY_ANSIARGS((int wid, int hig,
								char *d1, char *d2,
								char *fc, char *bc,
								int  x,   int y,
								char *name,
								int *idx_ret));
Cursor                EZ_GetCursor MY_ANSIARGS((int type));
Cursor                EZ_GetCursorByName MY_ANSIARGS((char *name));
char                  *EZ_GetCursorName MY_ANSIARGS((Cursor csr));
void                  EZ_BusyCursor MY_ANSIARGS((EZ_Widget *widget, Cursor cursor, int timeout));
void                  EZ_WaitCursor MY_ANSIARGS((EZ_Widget *widget, Cursor cursor));
void                  EZ_NormalCursor  MY_ANSIARGS((EZ_Widget *widget));
/***************************************************************/
void                  EZ_InitializeCursors MY_ANSIARGS((void));
void                  EZ_FreeAllCursors MY_ANSIARGS((void));
/***************************************************************/
typedef struct ctbe {
  Cursor cursor;
  char   *name;
} CTblE;
static   CTblE  cursor_table[] =
{
#define EZ_DefineCursor
#include "EZ_Cursor.h"
};

/***************************************************************/
void EZ_FreeAllCursors()
{
  int i;
  for(i = 0; i < EZ_TOTAL_NUM_OF_CURSORS ; i++)
    {
      if(cursor_table[i].cursor != (Cursor) None)
	{ XFreeCursor(EZ_Display,cursor_table[i].cursor); cursor_table[i].cursor = None;}
      if((i&1) && cursor_table[i].name)
         {
           my_free(cursor_table[i].name);
           cursor_table[i].name = NULL;
         }
    }
}
/***************************************************************/
Cursor EZ_GetCursor(cid) int cid;
{
  if(cid < 0 || cid >= EZ_TOTAL_NUM_OF_CURSORS) return(None);

  if((cid & 1) == 0)
    {
      if(cursor_table[cid].cursor == None)
	cursor_table[cid].cursor = XCreateFontCursor(EZ_Display, cid);
    }
  return(cursor_table[cid].cursor);
}
/***************************************************************/   
Cursor EZ_GetCursorByName(name)
     char *name;
{
  if(name)
    {
      int i;
      for(i = 0; i < EZ_TOTAL_NUM_OF_CURSORS; i++)
        {
          if(cursor_table[i].name && !strcmp(cursor_table[i].name, name))
            return(EZ_GetCursor(i));
        }
    }
  return(None);
}

char *EZ_GetCursorName(cursor)
     Cursor cursor;
{
  if(cursor != None)
    {
      int i;
      for(i = 0; i < EZ_TOTAL_NUM_OF_CURSORS; i++)
	if(cursor_table[i].cursor == cursor)
	  {
            return(cursor_table[i].name);
	    break;
	  }
    }
  return(NULL);
}
/***************************************************************/   
static void EZ_SetCursorWork(widget, cursor)
     EZ_Widget *widget;
     Cursor    cursor;
{
  if(widget)
    {
      EZ_Widget *children, *sibling;
      Window win = EZ_WidgetWindow(widget);
      Cursor cur;
      if(cursor == None) cur = EZ_WidgetCursor(widget);
      else cur = cursor;
      if(win) XDefineCursor(EZ_Display, win, cur);
      children = EZ_WidgetChildren(widget);
      sibling = EZ_WidgetSibling(widget);
      EZ_SetCursorWork(children, cursor);
      EZ_SetCursorWork(sibling, cursor);
    }
}

void  EZ_WaitCursor(widget, cursor)
     EZ_Widget *widget;
     Cursor    cursor;
{
  if(EZ_WidgetIsViewable(widget)) 
    {
      EZ_Widget *toplevel = widget;
      while(EZ_WidgetParent(toplevel)) toplevel = EZ_WidgetParent(toplevel);
      EZ_SetCursorWork(toplevel, cursor);
      XSync(EZ_Display, False);
    }
}

static void timer_callback(timer, cdata)
     EZ_Timer *timer; 
     void *cdata;
{
  EZ_Widget *widget = (EZ_Widget *)cdata;
  EZ_WaitCursor(widget, None);
}

void  EZ_BusyCursor(widget, cursor, tout)
     EZ_Widget *widget;
     Cursor    cursor;
     int tout;
{
  if(EZ_WidgetIsViewable(widget)) 
    {
      EZ_Widget *toplevel = widget;
      while(EZ_WidgetParent(toplevel)) toplevel = EZ_WidgetParent(toplevel);
      EZ_SetCursorWork(toplevel, cursor);
      XSync(EZ_Display, False);
      if(tout > 0) 
	(void) EZ_CreateTimer((long)tout, 0, 1, timer_callback, (void *)widget, 0);
    }
}

void EZ_NormalCursor(widget)
     EZ_Widget *widget;
{
  EZ_WaitCursor(widget, None);
}
/*****************************************************************************/
static Cursor installCursor(pixmap, mask, fg, bg, xx,yy, name, idx_ret)
     Pixmap pixmap, mask; char *fg, *bg, *name; int xx, yy; int *idx_ret;
{
  int    (*OldErrorHandler)();
  XColor fgc, bgc;
  Cursor cursor = None;
  int vvv = -1;

  if(!XParseColor(EZ_Display,EZ_Colormap, fg ,&fgc))
    {fgc.red = fgc.green = fgc.blue = 0xffff;}
  if(!XParseColor(EZ_Display,EZ_Colormap, bg ,&bgc))
    {bgc.red = bgc.green = bgc.blue = 0;}

  EZ_XErrorCode = 0;
  OldErrorHandler = XSetErrorHandler(EZ_XErrorHandler); 
  cursor = XCreatePixmapCursor(EZ_Display, pixmap, mask, &fgc,&bgc, xx, yy);
  XSync(EZ_Display, False);
  XSetErrorHandler(OldErrorHandler);  
  if(mask != pixmap) XFreePixmap(EZ_Display, mask);
  XFreePixmap(EZ_Display, pixmap);
  if(EZ_XErrorCode == 0 && cursor != None)
    {
      int i;
      for(i = 1; i < EZ_TOTAL_NUM_OF_CURSORS ; i+=2)
	{
	  if(cursor_table[i].cursor == (Cursor) None) 
	    {
              cursor_table[i].cursor = cursor;
              cursor_table[i].name = EZ_AllocCopyString(name);
              vvv = i;
              break;
            }
        }
    }
  if(idx_ret) *idx_ret = vvv;
  return(cursor);
}
/*********************************************************************************/

Cursor EZ_CreateCursorFromBitmapFile(f1,f2,fg, bg, x,y, name, idx_ret)
     char *f1, *f2, *fg, *bg, *name;  int x, y; int *idx_ret;
{
  if(!access(f1, R_OK))
    {
      Pixmap pixmap, mask;
      unsigned int width1, height1, width2, height2; 
      int x1=-1,x2,y1,y2,xx=0,yy=0,hasmask=0;

      if(XReadBitmapFile(EZ_Display,
			 RootWindow(EZ_Display, EZ_ScreenNum),
			 f1,
			 &width1,
			 &height1,
			 &pixmap,
			 &x1,
			 &y1) != BitmapSuccess)
	{
	  if(idx_ret) *idx_ret = -1;
	  return(None);
	}
      if(f2 && strcmp(f1,f2) && !access(f2, R_OK))
	{
	  if(XReadBitmapFile(EZ_Display,
			     RootWindow(EZ_Display, EZ_ScreenNum),
			     f2,
			     &width2,
			     &height2,
			     &mask,
			     &x2,
			     &y2) == BitmapSuccess)
	    {
	      if(width1 == width2 && height1 == height2)
		hasmask = 1;
	      else { XFreePixmap(EZ_Display, mask); hasmask = 0; }
	    }
	  
	}
      if(hasmask == 0) mask = pixmap;
      if(x1 != -1) { xx = x1; yy = y1;}
      else if(x >=0 && x < width1 && y >=0 && y < height1)
	{ xx = x; yy = y; }
      return(installCursor(pixmap, mask, fg, bg, xx,yy, name, idx_ret));
    }
  if(idx_ret) *idx_ret = -1; return(None);
}

/***********************************************************************/
Cursor EZ_CreateCursorFromBitmapData(w, h, d1, d2,fg,bg, x,y, name, idx_ret)
     char *d1, *d2, *fg, *bg, *name;  int w, h, x, y; int *idx_ret;
{
  if(d1 != NULL)
    {
      Pixmap pixmap, mask=None;
      int xx=0,yy=0,hasmask=0;
      
      if((pixmap = XCreateBitmapFromData(EZ_Display,
					  RootWindow(EZ_Display, EZ_ScreenNum),
					  d1,
					  w, h)) == None)
	{
	  if(idx_ret) *idx_ret = -1;
	  return(None);
	}
      
      if(d2 && d1 != d2)
	{
	  if((mask = XCreateBitmapFromData(EZ_Display,
					    RootWindow(EZ_Display, EZ_ScreenNum),
					    d2,
					    w, h)) != None)
	    hasmask = 1;
	}
      if(hasmask == 0) mask = pixmap;
      if(x >=0 && x < w && y >=0 && y < h)
	{ xx = x; yy = y; }
      return( installCursor(pixmap, mask, fg, bg, xx,yy, name, idx_ret));
    }
  if(idx_ret) *idx_ret = -1; return(None);
}
/*********************************************************************************/
#undef _EZ_CURSOR_C_









