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
 ***              Widget NW Label.                             ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_NWLABEL_C_
#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget        *EZ_CreateNWLabel MY_ANSIARGS((EZ_Widget *parent, char *label));
EZ_Widget        *EZ_CreateNWFrame MY_ANSIARGS((EZ_Widget *parent));
void              EZ_DisplayNWLabel MY_ANSIARGS((EZ_Widget *widget,
						 Pixmap pixmap,
						 unsigned long fg, unsigned long bg,
						 int disabled, int highlight,
						 int xoff, int yoff, int laboff, 
						 int disp_sib));
/*********************************************************************/
void             EZ_DrawWidgetNWLabel(widget) EZ_Widget *widget; { }
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_NWLabelHandle =
{ 
  EZ_ComputeWidgetLabelSize,  /* same as a regular label */
  EZ_DrawWidgetNWLabel,
  EZ_FreeWidgetLabelData, /* same as a regular label */
  EZ_UnkownEventHandle,   /* never called */
};

/*********************************************************************/
EZ_Widget  *EZ_CreateNWFrame(parent)
     EZ_Widget  *parent;           /* parent widget    */
{
  EZ_Widget *tmp = EZ_CreateNWLabel(parent, NULL);
  if(tmp) EZ_IsNWFrame(tmp) = 1;
  
  return(tmp);
}
/*********************************************************************/

EZ_Widget  *EZ_CreateNWLabel(parent,label)
     EZ_Widget  *parent;           /* parent widget    */
     char             *label;      /* label string     */
{
  EZ_Widget  *tmp;
  if(parent == (EZ_Widget *)NULL)  /* cannot be a standalone widget */
    return(NULL);

  tmp = EZ_CreateNewWidget(parent);
  EZ_WidgetHandlingFunctions[EZ_WIDGET_NW_LABEL] = &EZ_NWLabelHandle;
  EZ_SetWidgetTypeAndNames(tmp, EZ_WIDGET_NW_LABEL);

  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(tmp)  = 0;
  EZ_WidgetSepX(tmp)    = EZ_WIDGET_DEFAULT_PADX;
  EZ_WidgetSepY(tmp)    = EZ_WIDGET_DEFAULT_PADY;
  EZ_WidgetPadB(tmp)  = 0;
  EZ_WidgetPadX(tmp)    = 0;
  EZ_WidgetPadY(tmp)    = 0;
  EZ_LabelPixmap(tmp) = (EZ_Bitmap *)NULL;
  EZ_LabelString(tmp) = EZ_AllocCopyString(label);
  EZ_LabelStringLength(tmp) = (label == (char *)NULL ? 1: strlen(label));
  EZ_LabelLineLength(tmp) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_LabelNLines(tmp) = 0;
  EZ_LabelFont(tmp) = EZ_GetFontFromId(EZ_LABEL_FONT);
  EZ_LabelTWidth(tmp)  = 0;
  EZ_LabelTHeight(tmp) = 0;
  EZ_LabelFontAscent(tmp) = 0;
  EZ_LabelAlwaysHighlight(tmp) = 0;
  /* for free label: serve for special popups */
  EZ_LabelMovable(tmp) = 1; 
  EZ_LabelDontTakeFocus(tmp) = 0;
  EZ_LabelIsMenuTitle(tmp) = 0;
  EZ_LabelIsLabeledIcon(tmp) = 0;
  EZ_LabelTextMask(tmp) = (Pixmap)None;
  EZ_LabelUnderline(tmp) = -1;
  EZ_LabelModifiers(tmp) = 0;
  EZ_LabelShortcut(tmp) = NULL;
  EZ_LabelMenu(tmp) = NULL;
  EZ_LabelCopy(tmp) = NULL;
  EZ_WidgetMapped(tmp)  = 1;

  return(tmp);
}

/*******************************************************************
 *
 *  Display A No Window Label 'Widget'.
 */
void  EZ_DisplayNWLabel(wptr, pixmap, fg, bg, disabledIn ,highlight,
			xoffset,yoffset, labeloffset, disp_sibling)
     EZ_Widget *wptr;                 /* the  NW Label */
     Pixmap    pixmap;                /* the pixmap to draw into */
     unsigned  long fg,bg;            /* current parent fg, bg */
     int       xoffset, yoffset;      /* start position in pixmap (Origin) */
     int       labeloffset;           /*  1 or 0. (Button down or not)   */
     int       disabledIn;            /* children of a disabled widget */
     int       highlight;             /* highlight option              */
     int       disp_sibling;          /* disp sibling NW labels ?  */
{
  int       ox, oy,xstart, ystart, x,y,xx,yy,fillx,filly, padb, nhlight;
  int       NewXOffset, NewYOffset, disabled, tilex, tiley;
  GC        gc;
  unsigned  long ufg, dfg, bgpv;
  XFontStruct *ufont;
  EZ_Widget *parent, *children;
  EZ_Bitmap *bitmap = EZ_WidgetPixmap(wptr);

  if(EZ_GetWidgetNonActiveFlag(wptr)) goto Sibling;
    
  EZ_WidgetMapped(wptr) = 1;
  EZ_GetWidgetLabelPosition(wptr, &fillx, &filly);
  disabled = disabledIn || EZ_GetWidgetDisabledFlag(wptr);
  padb = EZ_WidgetPadB(wptr);
  NewXOffset =  xoffset + labeloffset + padb;
  NewYOffset =  yoffset + labeloffset + padb;
  ox = EZ_WidgetOriginX(wptr);
  oy = EZ_WidgetOriginY(wptr);

  xstart =  ox + NewXOffset;
  ystart =  oy + NewYOffset;
  nhlight = highlight | EZ_GetWidgetHighlightFlag(wptr);

  /* Backgroud color  */
  if((parent = EZ_WidgetParent(wptr))) EZ_PropagateBgInfo2Widget(parent, wptr);
  
  if(EZ_IsNWFrame(wptr) == 0)
    {
      int tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv, 1, highlight);  
      int arrowType = EZ_LabelArrowType(wptr);
      int bwpadx, bwpady;

      if(tile)
	{
	  EZ_GetWidgetTileOrigin(wptr, &tilex, &tiley);      
	  XSetTSOrigin(EZ_Display, gc, tilex, tiley);	  
	}
      XFillRectangle(EZ_Display, pixmap, gc, xstart, ystart,
		     EZ_WidgetWidth(wptr), EZ_WidgetHeight(wptr)); 
      
      bwpadx =  EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr);
      bwpady =  EZ_WidgetPadY(wptr) + EZ_WidgetBorderWidth(wptr);
      xx = xstart + fillx + EZ_WidgetXOffset(wptr);
      yy = ystart + filly + EZ_WidgetYOffset(wptr);
      x = xx + bwpadx;
      y = yy + bwpady;

      /* figure out the foreground color.  */
      dfg = EZ_DefaultForeground;
      ufg = EZ_LabelForeground(wptr);
      if(ufg == dfg && fg != dfg) ufg = fg;

      if(arrowType)
        {
          EZ_DrawSpecial(wptr,pixmap, None, arrowType, xstart+bwpadx,ystart+bwpady, 
                         EZ_WidgetWidth(wptr) - (bwpadx+bwpadx),
                         EZ_WidgetHeight(wptr) - (bwpady+bwpady));
        }
      else
        {
          int orien = EZ_WidgetOrientation(wptr);
          int tx0=0, ty0=0;
          int pwidth = EZ_LabelMWidth(wptr);
          int pheight = EZ_LabelMHeight(wptr);
          if(orien >= EZ_VERTICAL)
            {
              if(bitmap)
                {
                  int twidth = EZ_LabelTWidth(wptr);
                  int px0, py0;
                  if(twidth > pwidth) 
                    {
                      px0 = (twidth-pwidth)>>1; py0 = 0;
                      tx0 = 0; ty0 = bitmap->height + EZ_WidgetSepY(wptr);
                    }
                  else
                    {
                      px0 = 0; py0=0;
                      tx0 = (pwidth-twidth)>>1; 
                      ty0 = bitmap->height + EZ_WidgetSepY(wptr);                  
                    }
                  gc = EZ_WRITABLEGC;
                  XSetForeground(EZ_Display, gc, ufg);
                  XSetBackground(EZ_Display, gc, bg);
                  XSetClipMask(EZ_Display, gc, bitmap->shape);
                  XSetClipOrigin(EZ_Display, gc,x,y);
                  EZ_RenderPixmapLabelWithGC(bitmap, pixmap, gc, x+px0,y+py0);
                  if(bitmap->shape != (Pixmap)NULL) XSetClipMask(EZ_Display, gc, None);
                }
            }
          else /* horizontal */
            {
              if(bitmap)
                {
                  int theight = EZ_LabelTHeight(wptr) * EZ_LabelNLines(wptr);
                  int px0, py0;
                  if(theight > pheight) 
                    {
                      px0 = 0; py0 =  (theight-pheight)>>1;
                      tx0 = bitmap->width + EZ_WidgetSepX(wptr); ty0 = 0; 
                    }
                  else
                    {
                      px0 = 0; py0 = 0;
                      tx0 = bitmap->width + EZ_WidgetSepX(wptr);
                      ty0 = (pheight-theight)>>1;
                    }
                  gc = EZ_WRITABLEGC;
                  XSetForeground(EZ_Display, gc, ufg);
                  XSetBackground(EZ_Display, gc, bg);
                  XSetClipMask(EZ_Display, gc, bitmap->shape);
                  XSetClipOrigin(EZ_Display, gc,x,y);
                  EZ_RenderPixmapLabelWithGC(bitmap, pixmap, gc, x+px0,y+py0);
                  if(bitmap->shape != (Pixmap)NULL) XSetClipMask(EZ_Display, gc, None);
                }
            }
          x += tx0; y += ty0;
          if(!bitmap || EZ_PTLabel)
            {
              y +=  EZ_LabelFontAscent(wptr);
              ufont = EZ_LabelFont(wptr);

              if( disabled ) 
                {
                  GC BTgc, DKgc, Ngc;
                  if(EZ_GetDarkBrightNormalBDGC(wptr, &DKgc, &BTgc, &Ngc))
                    {
                      XSetTSOrigin(EZ_Display, DKgc, tilex, tiley);	  
                      XSetTSOrigin(EZ_Display, BTgc, tilex, tiley);	  
                      XSetTSOrigin(EZ_Display, Ngc, tilex, tiley);	  
                    }
                  XSetFont(EZ_Display, DKgc, ufont->fid);
                  XSetFont(EZ_Display, BTgc, ufont->fid);  
                  XSetFont(EZ_Display, Ngc, ufont->fid);	    
	  
                  EZ_RenderLabelTextDisabled(pixmap, BTgc,DKgc,Ngc, x, y, 
                                             (int)EZ_LabelTHeight(wptr), EZ_LabelString(wptr),
                                             (int)EZ_LabelStringLength(wptr), (int)EZ_LabelLineLength(wptr),
                                             (int)EZ_LabelNLines(wptr),
                                             (int)EZ_LabelTWidth(wptr),
                                             (int)EZ_WidgetJustification(wptr),
                                             ufont);
                }
              else
                {
                  int shadow = EZ_WidgetLabelShadow(wptr);
                  if(ufg != dfg || ufont != EZ_GetFontFromId(EZ_LABEL_FONT))
                    {
                      XSetFont(EZ_Display, EZ_WRITABLEGC, ufont->fid);
                      XSetForeground(EZ_Display, EZ_WRITABLEGC, ufg);
                      gc = EZ_WRITABLEGC;
                    }
                  else  gc = EZ_LABEL_TEXTGC;
	  
                  EZ_RenderLabelText(pixmap, gc, x, y, (int)EZ_LabelTHeight(wptr), EZ_LabelString(wptr),
                                     (int)EZ_LabelStringLength(wptr), (int)EZ_LabelLineLength(wptr),
                                     (int)EZ_LabelNLines(wptr),
                                     (int)EZ_LabelTWidth(wptr),
                                     (int)EZ_WidgetJustification(wptr),
                                     ufont, shadow);
                  if(EZ_LabelUnderline(wptr) >= 0 && 
                     EZ_LabelUnderline(wptr) < ((int)EZ_LabelLineLength(wptr)>>1))
                    {
                      int tlen = XTextWidth(EZ_LabelFont(wptr),EZ_LabelString(wptr),
                                            EZ_LabelUnderline(wptr));
                      int tlen1 = tlen + XTextWidth(EZ_LabelFont(wptr),
                                                    EZ_LabelString(wptr)+EZ_LabelUnderline(wptr), 1);
                      int base = y + EZ_LabelTHeight(wptr) - EZ_LabelFontAscent(wptr)-1;
                      int boldfont;

                      if(EZ_LabelFont(wptr) != EZ_GetFontFromId(EZ_LABEL_FONT))
                        boldfont = (EZ_GetWidgetThinFontFlag(wptr) == 0);
                      else boldfont = EZ_LabelFontIsBold;
                      if(shadow)
                        {
                          int shadowx, shadowy;
                          if(shadow &0xf0)
                            {
                              shadowx = (shadow & 0xf0) >> 4;
                              shadowy = (shadow & 0x0f);
                            }
                          else shadowx = shadowy = shadow;
                          if(shadowx & 8) shadowx= (8-shadowx);
                          if(shadowy & 8) shadowy= (8-shadowy);
                          XDrawLine(EZ_DisplayForWidgets, pixmap, EZ_SHADOWGC,
                                    x+tlen+shadowx, base+shadowy, x+tlen1+shadowx, base+shadowy);
                          if(boldfont)
                            XDrawLine(EZ_DisplayForWidgets, pixmap, EZ_SHADOWGC,
                                      x+tlen+shadowx, base+1+shadowy, x+tlen1+shadowx, base+1+shadowy);
                        }	      
                      XDrawLine(EZ_Display, pixmap, gc,
                                x+tlen, base, x+tlen1, base);
                      if(boldfont)
                        XDrawLine(EZ_Display, pixmap, gc,
                                  x+tlen, base+1, x+tlen1, base+1);
                    }
                }
            }
        }
    }
  if(EZ_WidgetBorderWidth(wptr) != 0 && EZ_WidgetBorderStyle(wptr)!= EZ_BORDER_NONE)
    EZ_DrawRectBorderWithSize(wptr, pixmap, xstart, ystart,
			      (int) EZ_WidgetWidth(wptr),
			      (int) EZ_WidgetHeight(wptr),
			      (int) EZ_WidgetBorderWidth(wptr),
			      (int) EZ_WidgetBorderStyle(wptr),0,0,0);

  /* Now recursively display children widgets */
  children = EZ_WidgetChildren(wptr);

  if(children && EZ_WidgetType(children) == EZ_WIDGET_NW_LABEL) /* can only have children of the same type */
    {
      int Xoff = NewXOffset + ox;
      int Yoff = NewYOffset + oy;
      EZ_DisplayNWLabel(children, pixmap, fg, bg, disabled, nhlight, Xoff,Yoff, 0, 1);
    }
Sibling:
  if(disp_sibling)
    {   /* have to handle NW sibling here */	
      EZ_Widget *sibling = EZ_WidgetSibling(wptr);
      while(sibling) 
	{
	  if(EZ_WidgetType(sibling) == EZ_WIDGET_NW_LABEL)
	    EZ_DisplayNWLabel(sibling, pixmap, fg, bg, disabledIn, highlight,
			      xoffset,yoffset, labeloffset, 0);
	  sibling = EZ_WidgetSibling(sibling);
	}
    }
}
/****************************************************************************/
#undef _EZ_WIDGET_NWLABEL_C_
