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
 ***              Widget Radio Button                          ***
 ***                                                           ***
 *****************************************************************/
#define  _EZ_WIDGET_BUTTONR_C_

#include "EZ_Widget.h"

/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget         *EZ_CreateRadioButton MY_ANSIARGS((EZ_Widget *parent, char *lab, int ul, int gp, int val));
void              EZ_DrawWidgetRadioButton  MY_ANSIARGS((EZ_Widget *widget));
void              EZ_ComputeWidgetRButtonSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void              EZ_FreeWidgetRButtonData MY_ANSIARGS((EZ_Widget *widget));
void              EZ_RButtonEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
void              EZ_RemoveRBtnFromItsGroup MY_ANSIARGS(( EZ_Widget *rbutton));
void              EZ_SetRadioButtonGroupVariableValue MY_ANSIARGS((EZ_Widget *widget, int value));
int               EZ_GetRadioButtonGroupVariableValue MY_ANSIARGS((EZ_Widget *widget));
int               EZ_GetRadioButtonGroupID MY_ANSIARGS((EZ_Widget *widget));
void              EZ_FixRadioButtonList MY_ANSIARGS((EZ_Widget *w, int id));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_RButtonHandle =
{ 
  EZ_ComputeWidgetRButtonSize,
  EZ_DrawWidgetRadioButton,
  EZ_FreeWidgetRButtonData,
  EZ_RButtonEventHandle,
};


/*********************************************************************
 *
 * Radio buttons are saved on a special list for the purpose of
 * searching for ones that share the same control variable.
 */

static int    EZ_SearchRButtonInRButtonList MY_ANSIARGS(( EZ_Widget *rbutton,
							  EZ_RButtonList  *rblist));
/*********************************************************************
 *
 *  local data.
 */
static  EZ_RButtonGpStruct *rbuttongroups = (EZ_RButtonGpStruct *)NULL;

/*********************************************************************/

EZ_Widget  *EZ_CreateRadioButton(parent, label, underline, groupid, value)
     EZ_Widget  *parent;     /* parent widget    */
     char             *label;
     int              underline, value, groupid;
{
  EZ_Widget  *wptr;
  
  wptr = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for Button.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_RADIO_BUTTON] = &EZ_RButtonHandle;

  EZ_SetWidgetTypeAndNames(wptr,  EZ_WIDGET_RADIO_BUTTON);
  EZ_RButtonLabel(wptr) = EZ_AllocCopyString(label);
  EZ_RButtonLabelLength(wptr) = (label == (char *)NULL? 1 : strlen(label));
  EZ_RButtonPixmap(wptr) = (EZ_Bitmap *)NULL;
  EZ_RButtonFont(wptr) = EZ_GetFontFromId(EZ_BUTTON_FONT);
  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_NONE;
  EZ_RButtonTWidth(wptr)     = 0;
  EZ_RButtonTHeight(wptr)    = 0;
  EZ_RButtonFontAscent(wptr) = 0;
  EZ_RButtonLineLength(wptr) = EZ_DEFAULT_LABEL_LINE_LENGTH;
  EZ_RButtonNLines(wptr)  = 1;
  EZ_RButtonCheckBorderWidth(wptr) = 2;
  EZ_RButtonCheckBorderStyle(wptr) = EZ_BORDER_UP;
  EZ_RButtonCheckSizeAdjust(wptr) = 0;
  EZ_RButtonCheckType(wptr) = EZ_DIAMOND_INDICATOR;
  EZ_RButtonCheckColor(wptr)= 0;

  EZ_RButtonLabel2(wptr)  = (char *)NULL;
  EZ_RButtonLabel2Length(wptr) = 0;
  EZ_RButtonLabel2Width(wptr) = 0;

  EZ_WidgetSepX(wptr) = EZ_WIDGET_DEFAULT_PADX;
  EZ_WidgetSepY(wptr) = EZ_WIDGET_DEFAULT_PADY;
  EZ_WidgetPadB(wptr) = 2;

  EZ_SetWidgetFocusableFlag(wptr);

  EZ_RButtonGroup(wptr) = NULL;
  EZ_RButtonVarPtr(wptr) = NULL;

  EZ_RButtonVarValue(wptr) = value;
  EZ_FixRadioButtonList(wptr,groupid); 

  if(label && underline >= 0 && underline < EZ_ButtonLabelLength(wptr) && isprint(label[underline]))
    {
      EZ_RButtonUnderLine(wptr) = underline;
      EZ_RButtonModifiers(wptr) = Mod1Mask;
      EZ_RButtonShortcut(wptr)  = (EZ_RButtonLabel(wptr) + underline);
      /*-------------------------------------------------------------
       *  Register this shortcut in the globale KeyPress event table
       *------------------------------------------------------------*/
      EZ_InsertGlobalKeyPressEvent(wptr, Mod1Mask, (label+underline));
    }
  else
    {
      EZ_RButtonUnderLine(wptr) = -1;
      EZ_RButtonModifiers(wptr) = 0;
      EZ_RButtonShortcut(wptr)  = (char *)NULL;
    }
  EZ_RButtonCopy(wptr) = NULL;
  return(wptr);
}

/********************************************************************
 *
 *  Figure out the dimension of a radio button
 */
void EZ_ComputeWidgetRButtonSize(widget, w, h)
    EZ_Widget *widget;
     int             *w, *h;
{
  int length, height, cw, ch, padb;
  int pwidth, pheight, ww, hh;
  int orien = EZ_WidgetOrientation(widget);
  int ipadx=0, ipady = 0;

  if(!(EZ_GetWidgetSizeComputedFlag(widget)))
    { 
      int nlines, twidth;
      EZ_Bitmap *bitmap = EZ_RButtonPixmap(widget);

      if(!bitmap || EZ_PTLabel)
        {
          EZ_FigureLabelExtent(EZ_RButtonFont(widget),
                               EZ_RButtonLabel(widget),
                               (int)EZ_RButtonLabelLength(widget),
                               (int)EZ_RButtonLineLength(widget),
                               &nlines, &twidth);
          EZ_RButtonTWidth(widget) = twidth;
          EZ_RButtonNLines(widget) = nlines;

          EZ_RButtonTHeight(widget) = EZ_RButtonFont(widget)->ascent +
            EZ_RButtonFont(widget)->descent;
          EZ_RButtonFontAscent(widget) = EZ_RButtonFont(widget)->ascent;

          if(EZ_RButtonLabel2Length(widget) != 0)
            {
              EZ_RButtonLabel2Width(widget) = EZ_LABEL_SEPARATION +
                XTextWidth(EZ_RButtonFont(widget), 
                           EZ_RButtonLabel2(widget),
                           EZ_RButtonLabel2Length(widget));
              EZ_WidgetAuxXRight(widget) = EZ_RButtonLabel2Width(widget);
            } 
          else
            {
              EZ_RButtonLabel2Width(widget) = 0;
              EZ_WidgetAuxXRight(widget) = 0;
            }
        }
      else
        {
          EZ_RButtonTWidth(widget) =0;
          EZ_RButtonNLines(widget) =0;
          EZ_RButtonTHeight(widget) =0;
          EZ_RButtonFontAscent(widget) =0;
          EZ_RButtonLabel2Width(widget) = 0;
          EZ_WidgetAuxXRight(widget) = 0;
        }
      if(bitmap)
	{
          EZ_RButtonMWidth(widget) = bitmap ->width;
          EZ_RButtonMHeight(widget) = bitmap ->height;
        }
      else
        {
          EZ_RButtonMWidth(widget) = 0;
          EZ_RButtonMHeight(widget) = 0;
        }
      EZ_SetWidgetSizeComputedFlag(widget);
    }
  length = EZ_RButtonTWidth(widget) + EZ_RButtonLabel2Width(widget);
  height = EZ_RButtonTHeight(widget) * EZ_RButtonNLines(widget);
  pwidth = EZ_RButtonMWidth(widget);
  pheight = EZ_RButtonMHeight(widget);
  if(orien < EZ_VERTICAL) /* horizontal */
    {
      if(pwidth && length) ipadx = EZ_WidgetSepX(widget);
      ww = ipadx + length + pwidth;
      hh = pheight > height? pheight: height;
    }
  else
    {
      if(pheight && height) ipady = EZ_WidgetSepY(widget);
      ww = pwidth > length? pwidth: length;
      hh = ipady + height + pheight;
    }

  cw = EZ_WidgetPadX(widget) + EZ_WidgetBorderWidth(widget);
  ch = EZ_WidgetPadY(widget) + EZ_WidgetBorderWidth(widget);
  ch = ch + ch + hh;
  if(EZ_RButtonCheckType(widget) != EZ_EMPTY_INDICATOR)
    EZ_WidgetAuxXLeft(widget) = ( (ch << 1)/5) + 6 +  EZ_RButtonCheckSizeAdjust(widget);
  else EZ_WidgetAuxXLeft(widget) = 0;

  cw = cw + cw + ww + EZ_WidgetAuxXLeft(widget);
  padb =   EZ_WidgetPadB(widget)<<1;
  *w = cw + padb;
  *h = ch + padb;
}


void EZ_FreeWidgetRButtonData(rbtn)
     EZ_Widget *rbtn;
{
  if(rbtn == (EZ_Widget *)NULL) return;

  /*-----------------------------------------------
   * remove it from its group list.
   *----------------------------------------------*/
  EZ_RemoveRBtnFromItsGroup(rbtn);
 
  /*----------------------------------
   * free allocated widget data.
   *---------------------------------*/
  if(EZ_RButtonLabel(rbtn) != (char *)NULL)
    (void)my_free((char *)EZ_RButtonLabel(rbtn));         /* free label string */
  if(EZ_RButtonLabel2(rbtn) != (char *)NULL)
    (void)my_free((char *)EZ_RButtonLabel2(rbtn));         /* free label string */
  if(EZ_RButtonPixmap(rbtn) != (EZ_Bitmap *)NULL)    /* free pixmap       */
    EZ_FreeBitmap(EZ_RButtonPixmap(rbtn));
  {
    EZ_Widget *clone = EZ_RButtonCopy(rbtn);
    if(clone) {EZ_RButtonCopy(clone) = NULL; }
  }
}

/*------------------------------------- 
 * remove a rbtn from its group
 *-----------------------------------*/

void EZ_RemoveRBtnFromItsGroup(rbtn)
     EZ_Widget *rbtn;
{
  EZ_RButtonList     *tmp, *tmp1;
  EZ_RButtonGpStruct *gpptr;

  gpptr = EZ_RButtonGroup(rbtn);
  EZ_RButtonGroup(rbtn) = NULL;
  EZ_RButtonVarPtr(rbtn) = NULL;

  if(gpptr)
    {
      tmp = gpptr->list;

      if(tmp->rbutton == rbtn)
	{
	  gpptr->list = tmp->next;
	  (void)my_free( (char *)tmp);
	}
      else
	{
	  while(tmp->next)
	    {
	      if(tmp->next->rbutton == rbtn)
		{
		  tmp1 = tmp->next;
		  tmp->next = tmp1->next;
		  (void)my_free( (char *)tmp1);
		  break;
		}
	      tmp = tmp->next;
	    }
	}
      if(gpptr->list == (EZ_RButtonList *)NULL)
	{
	  /* this group contains no buttons, so free it */
	  EZ_RButtonGpStruct *t = rbuttongroups;

	  if(t == gpptr) /* gpptr is the first one on the list */
	    {
	      rbuttongroups = gpptr->next;
	      (void)my_free((char *)gpptr);
	    }
	  else
	    {
	      while(t->next != gpptr)
		t = t->next;
	      t->next = gpptr->next;
	      (void)my_free( (char *)gpptr);
	    }
	}
    }
}

/*
 *  search in rblist to see if rbutton is in the list.
 */
static int  EZ_SearchRButtonInRButtonList(rbutton, rblist)
     EZ_Widget *rbutton;
     EZ_RButtonList  *rblist;
{
  int       value, exist=0, found=0;
  EZ_Widget *tmp;

  if(rblist == (EZ_RButtonList *)NULL)  return(0);
  value = EZ_RButtonVarValue(rbutton);
  while(rblist)
    {
      tmp = rblist->rbutton;
      if(tmp == rbutton) found = 1;
      else if(EZ_RButtonVarValue(tmp) == value)
        { 
          if(EZ_WidgetIsDestroyed(tmp) == 0) exist = 2; 
          else { EZ_RButtonVarValue(tmp) = (rand()>>5);}
        }
      rblist = rblist->next;
    }
  return(exist+found);
}

/*
 *  Fix all current radio button group.
 */
void  EZ_FixRadioButtonList(widget, gid)
     EZ_Widget   *widget;
     int         gid;
{
  EZ_RButtonList      **rblist = NULL;
  EZ_RButtonGpStruct  **rbgsl;
  int                 itmp;

  if(widget == (EZ_Widget *)NULL ||
     ( (EZ_WidgetType(widget) != EZ_WIDGET_RADIO_BUTTON) &&
       (EZ_WidgetType(widget) != EZ_WIDGET_MENU_RADIO_BUTTON)))
    return;
  
  /* remove widget from its current group if it differs from the new group */
  {
    EZ_RButtonGpStruct *gpptr;

    gpptr = EZ_RButtonGroup(widget);
    if(gpptr) /* widget already belongs to a group */
      {
	int ogid = (gpptr)->id;
	if(ogid != gid)  /* widget is in a different rbutton group */
	  EZ_RemoveRBtnFromItsGroup(widget);
      }
  }
  /********************************************/
  rbgsl = &rbuttongroups;  

  while(*rbgsl != (EZ_RButtonGpStruct *)NULL)
    {
      if( (*rbgsl)->id == gid)
	{
	  rblist = &((*rbgsl)->list);
	  break;
	}
      rbgsl = &((*rbgsl)->next);
    }
  
  if(*rbgsl == (EZ_RButtonGpStruct *)NULL) /* a new group */
    {
      EZ_RButtonGpStruct  *t;

      t = (EZ_RButtonGpStruct *) my_malloc(sizeof(EZ_RButtonGpStruct),_R_BUTTON_GP_);
      if(!t) EZ_OutOfMemory("EZ_FixRadioButtonList");
      t->id = gid;
      t->list = (EZ_RButtonList *)NULL;
      t->next = *rbgsl;
      *rbgsl = t;
      rblist = &(t->list);      
    }
  itmp = EZ_SearchRButtonInRButtonList(widget, *rblist);
  if(itmp == 0) /* a new r button */
    {
      EZ_RButtonList *tmp;      
      tmp = (EZ_RButtonList *)my_malloc(sizeof(EZ_RButtonList),_R_BUTTON_GP_);
      if(!tmp) EZ_OutOfMemory("EZ_FixRadioButtonList");
      tmp->rbutton = widget;
      tmp->next = *rblist;
      *rblist = tmp;
    }
  if(itmp >= 2) /* there is a rbutton with the same set value */
    {
      (void)fprintf(stderr,"Duplicate radio buttons in group %d, deactivated.\n", gid);
      EZ_SetWidgetNonActiveFlag(widget);
      EZ_RButtonVarValue(widget) = (rand()>>5); /* just to be sure */
    }
  EZ_RButtonGroup(widget) = *rbgsl;
  EZ_RButtonVarPtr(widget) = &((*rbgsl)->var);
} 

/******************************************************************************
 *
 *  Draw a radio button
 */

void  EZ_DrawWidgetRadioButton(wptr)
     EZ_Widget *wptr;
{
  int             w, h,x,y, xx, yy, fillx, filly, padb, padb2, tile;
  Pixmap          pixmap;
  Window          win;
  GC              gc;
  int             offset, offseta = 0, tileOx=0, tileOy =0;
  int             orien, tx0 = 0, ty0 = 0, pwidth, twidth, pheight, theight;
  unsigned long   bgpv;
  EZ_Bitmap       *bitmap;

  /*=> if there is no indicator, need a way to indicates the 
   * selection. so we ignore the user-configured border here and
   * switche between UP-DOWN borders. Anything else ?
   */
  if(EZ_RButtonCheckType(wptr) == EZ_EMPTY_INDICATOR)
    {
      if(EZ_RButtonOn(wptr))
	{
	  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_DOWN;
	}
      else
	{
	  EZ_WidgetBorderStyle(wptr) = EZ_BORDER_UP;
	}
    }
  /*<=  */

  win = EZ_WidgetWindow(wptr);
  w   = EZ_WidgetWidth(wptr);
  h   = EZ_WidgetHeight(wptr);
  padb = EZ_WidgetPadB(wptr);
  padb2 = padb + padb;
  EZ_GetWidgetLabelPosition(wptr, &fillx, &filly);

  /*-----------------------------------------------------------
   *  Create a pixmap, draw into this pixmap in background and
   *  copy to the button window when finished.
   *----------------------------------------------------------*/
  pixmap = XCreatePixmap(EZ_DisplayForWidgets, win, w, h, EZ_DepthForWidgets);    
  EZ_FixFocusHighlightBorder(wptr, pixmap, w, h, padb);

  if(EZ_OffsetEnabled && EZ_WidgetBorderStyle(wptr) == EZ_BORDER_DOWN)  offseta = 1;
  tile = EZ_GetBackgroundGC(wptr, &gc, &bgpv,1,0);
  if(tile)
    {
      EZ_GetWidgetTileOrigin(wptr, &tileOx, &tileOy);      
      tileOx += offseta; tileOy += offseta;
      XSetTSOrigin(EZ_Display, gc, tileOx, tileOy);
    }
  XFillRectangle(EZ_DisplayForWidgets, pixmap, gc, padb,padb, w-padb2, h-padb2); 
  /*--------------------------------------------------------
   *  Draw the label.
   *-------------------------------------------------------*/
  bitmap = EZ_RButtonPixmap(wptr); 
  xx = EZ_WidgetXOffset(wptr) + fillx ;
  yy =  EZ_WidgetYOffset(wptr) + filly;
  orien = EZ_WidgetOrientation(wptr);
  pwidth = EZ_RButtonMWidth(wptr);
  pheight = EZ_RButtonMHeight(wptr);
  twidth = EZ_RButtonTWidth(wptr);
  theight = EZ_RButtonTHeight(wptr) * EZ_RButtonNLines(wptr);

  offset = padb + offseta;
  x = xx + EZ_WidgetPadX(wptr) + EZ_WidgetBorderWidth(wptr) + EZ_WidgetAuxXLeft(wptr) + offset;
  y = yy + EZ_RButtonFontAscent(wptr) + EZ_WidgetBorderWidth(wptr) + EZ_WidgetPadY(wptr) + offset;

  if(orien >= EZ_VERTICAL)
    {
      if(bitmap)
        {
          int px0, py0;
          if(twidth > pwidth)
            {
              px0=(twidth - pwidth)>>1; py0 = 0;
              tx0 = 0; ty0 = pheight + EZ_WidgetSepY(wptr);
            }
          else
            {
              px0 = 0; py0=0;
              tx0 = (pwidth-twidth)>>1; 
              ty0 = bitmap->height + EZ_WidgetSepY(wptr);                  
            }
          EZ_RenderPixmapLabel(wptr, bitmap, pixmap, x+px0, y+py0
                               - (int) (EZ_RButtonFontAscent(wptr)));
        }
    }
  else /* horizontal */
    {
      if(bitmap)
        {
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
          EZ_RenderPixmapLabel(wptr, bitmap, pixmap, x+px0, y+py0
                               - (int) (EZ_RButtonFontAscent(wptr)));
        }
    }
  x += tx0; y += ty0;

  if(!bitmap || EZ_PTLabel)
    {
      if( EZ_GetWidgetDisabledFlag(wptr) )
        {
          GC  bgc, lgc, ngc;
          if( EZ_GetDarkBrightNormalBDGC(wptr, &bgc, &lgc, &ngc))
            {
              XSetTSOrigin(EZ_Display, bgc, tileOx, tileOy);
              XSetTSOrigin(EZ_Display, lgc, tileOx, tileOy);
              XSetTSOrigin(EZ_Display, ngc, tileOx, tileOy);
            }
          XSetFont(EZ_DisplayForWidgets, bgc, EZ_WidgetFont(wptr)->fid);
          XSetFont(EZ_DisplayForWidgets, lgc, EZ_WidgetFont(wptr)->fid);
          XSetFont(EZ_DisplayForWidgets, ngc, EZ_WidgetFont(wptr)->fid);

          EZ_RenderLabelTextDisabled(pixmap, lgc, bgc, ngc, x, y, 
                                     (int)EZ_RButtonTHeight(wptr), EZ_RButtonLabel(wptr),
                                     (int)EZ_RButtonLabelLength(wptr), (int)EZ_RButtonLineLength(wptr),
                                     (int)EZ_RButtonNLines(wptr),
                                     (int)EZ_RButtonTWidth(wptr),
                                     (int)EZ_WidgetJustification(wptr),
                                     EZ_RButtonFont(wptr));
        }
      else
        {
          int shadow = EZ_WidgetLabelShadow(wptr);

          if(EZ_RButtonForeground(wptr) != EZ_DefaultForeground || 
             EZ_RButtonFont(wptr) != EZ_GetFontFromId(EZ_BUTTON_FONT))
            {
              XSetFont(EZ_DisplayForWidgets, EZ_WRITABLEGC, EZ_RButtonFont(wptr)->fid);
              XSetForeground(EZ_DisplayForWidgets, EZ_WRITABLEGC, EZ_RButtonForeground(wptr));	
              gc = EZ_WRITABLEGC;
            }
          else	gc = EZ_BUTTON_TEXTGC;
          EZ_RenderLabelText(pixmap, gc, x, y, (int)EZ_RButtonTHeight(wptr), EZ_RButtonLabel(wptr),
                             (int)EZ_RButtonLabelLength(wptr), (int)EZ_RButtonLineLength(wptr),
                             (int)EZ_RButtonNLines(wptr),
                             (int)EZ_RButtonTWidth(wptr),
                             (int)EZ_WidgetJustification(wptr),
                             EZ_RButtonFont(wptr), shadow);

          if(EZ_RButtonLabel2Length(wptr) != 0)
            {
              int tmpx, tmpy = ((h+EZ_RButtonFontAscent(wptr))>>1) + offseta;
              tmpx = EZ_GetLabel2XPosition(wptr,
                                           EZ_RButtonLabel2Width(wptr) -
                                           EZ_LABEL_SEPARATION, offseta);
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
                  XDrawString(EZ_DisplayForWidgets, 
                              pixmap,  EZ_SHADOWGC,
                              tmpx+shadowx,      tmpy+shadowy,
                              EZ_RButtonLabel2(wptr),
                              EZ_RButtonLabel2Length(wptr));                
                }
              XDrawString(EZ_DisplayForWidgets, 
                          pixmap,  gc,
                          tmpx,      tmpy,
                          EZ_RButtonLabel2(wptr),
                          EZ_RButtonLabel2Length(wptr));
            }
          else if(EZ_RButtonUnderLine(wptr) >= 0 && 
                  EZ_RButtonUnderLine(wptr) < ((int)EZ_RButtonLineLength(wptr)>>1))
            {
              int tlen = XTextWidth(EZ_RButtonFont(wptr),EZ_RButtonLabel(wptr),
                                    EZ_RButtonUnderLine(wptr));
              int tlen1 = tlen + XTextWidth(EZ_RButtonFont(wptr),
                                            EZ_RButtonLabel(wptr)+EZ_RButtonUnderLine(wptr), 1);
              int base = y + EZ_RButtonTHeight(wptr) - EZ_RButtonFontAscent(wptr)-1;
              int boldfont;

              if(EZ_RButtonFont(wptr) != EZ_GetFontFromId(EZ_BUTTON_FONT))
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
              XDrawLine(EZ_DisplayForWidgets, pixmap, gc,
                        x+tlen, base, x+tlen1, base);
              if(boldfont)
                XDrawLine(EZ_DisplayForWidgets, pixmap, gc,
                          x+tlen, base+1, x+tlen1, base+1);
            }
        }
    }
  /*---------------------
   * draw the check mark
   *--------------------*/
  EZ_DrawIndicator(wptr, pixmap, (int)EZ_RButtonOn(wptr),
		   (int) EZ_RButtonCheckType(wptr),
		   (int) EZ_RButtonCheckBorderWidth(wptr),
		   (int) EZ_RButtonCheckSizeAdjust(wptr),
		   EZ_RButtonCheckColor(wptr));

  /*============================================================
   *for complex button with  NW labels 
   */
  {
    EZ_Widget *children = EZ_WidgetChildren(wptr);
    if(children && EZ_WidgetType(children) == EZ_WIDGET_NW_LABEL)
      {
	EZ_DisplayNWLabel(children, pixmap, EZ_WidgetForeground(wptr), bgpv,
			  EZ_GetWidgetDisabledFlag(wptr),EZ_GetWidgetHighlightFlag(wptr),
			  0, 0, offseta,1);
      }
  }
  /*============================================================*/  
  /*------------------------------------
   *  Now draw the real border.
   *-----------------------------------*/
  EZ_DrawRectBorder(wptr, pixmap);
  XCopyArea(EZ_DisplayForWidgets,pixmap,win, EZ_WRITABLEGC,0,0,w,h,0,0); 
  XFreePixmap(EZ_DisplayForWidgets, pixmap); 
}



/*************************************************************************
 *
 *  Event handling
 */
void  EZ_RButtonEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent          *event;
{
  if(widget == (EZ_Widget *)NULL) return;
  if(event->type == Expose)   EZ_DrawWidget(widget);      
  if( EZ_GetWidgetDisabledFlag(widget) )  return;
  /*
  if(event->type == ButtonPress && event->xbutton.button != EZ_Btn1)
    return;
    */
  switch(event->type)
    {
    case ButtonPress:
      if(event->xbutton.button == EZ_Btn1)
	{
	  if( !( EZ_RButtonOn(widget)) )
	    {
	      EZ_RButtonList     *friends;
	      EZ_Widget *old = (EZ_Widget *)NULL;
	      
	      friends = EZ_RButtonGroup(widget)->list;
	      
	      while(friends)
		{
		  if(EZ_RButtonOn( friends->rbutton) )
		    {
		      old = friends->rbutton;  /* currently checked  */
		      break;
		    }
		  friends = friends->next;
		}
	      EZ_RButtonSetValue(widget);
	      EZ_DrawWidget(widget);
	      if(old && EZ_WidgetMapped(old)) EZ_DrawWidget(old);
	      { EZ_ExecuteWidgetCallBacks(widget);}
	    }
	}
      else if(event->xbutton.button == EZ_Btn3)
	{
          /*
            #include "EZ_DnDHandler1.c"
            EZ_DnDEventHandler1(widget, event);
            */
          EZ_HandleDnDEvents(widget, event);
	}
      break;
    case EnterNotify:
      EZ_SetWidgetHighlightFlag(widget);
      { EZ_InstallHighlightColor(widget); } /*4/29/99*/
      EZ_DrawWidget(widget);
      break;
    case LeaveNotify:
      EZ_ClearWidgetHighlightFlag(widget);
      { EZ_UninstallHighlightColor(widget); } /*4/29/99*/
      EZ_DrawWidget(widget);
      break;
    default:
      break;
    }
}


void EZ_SetRadioButtonGroupVariableValue(widget, value)
     EZ_Widget *widget;
     int       value;
{
  EZ_RButtonList     *list;

  if(widget != (EZ_Widget *)NULL)
    {
      if( EZ_WidgetType(widget) == EZ_WIDGET_RADIO_BUTTON ||
	 EZ_WidgetType(widget) == EZ_WIDGET_MENU_RADIO_BUTTON)
	{
	  *(EZ_RButtonVarPtr(widget)) = value;
	  
	  if(EZ_WidgetWindow(widget) != (Window )NULL)
	    {
	      list = EZ_RButtonGroup(widget)->list;
	      while(list)
		{
		  if(EZ_WidgetMapped(list->rbutton)) EZ_DrawWidget(list->rbutton);
		  list = list->next;
		}
	    }
	}
    }
}

int  EZ_GetRadioButtonGroupVariableValue(widget)
     EZ_Widget *widget;
{
  if(widget == (EZ_Widget *)NULL) return(-1);
  return( *(EZ_RButtonVarPtr(widget)));
}

int  EZ_GetRadioButtonGroupID(widget)
     EZ_Widget *widget;
{
  if(widget != (EZ_Widget *)NULL)
    if( EZ_WidgetType(widget) == EZ_WIDGET_RADIO_BUTTON ||
       EZ_WidgetType(widget) == EZ_WIDGET_MENU_RADIO_BUTTON)
      return(EZ_RButtonGroup(widget)->id);
  return(-1);
}
/****************************************************************/
void EZ_ResetGVButtonRC()
{
  rbuttongroups = (EZ_RButtonGpStruct *)NULL;
}
/****************************************************************/
#undef _EZ_WIDGET_BUTTONR_C_
