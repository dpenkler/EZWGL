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
/*----------------------------------------------------------------------*
 * This module is all new by Robert Nation
 * <nation@rocket.sanders.lockheed.com>
 *
 * Extensive modifications by mj olesen <olesen@me.QueensU.CA>
 * No additional restrictions are applied.
 *
 * As usual, the author accepts no responsibility for anything, nor does
 * he guarantee anything whatsoever.
 *
 * Design of this module was heavily influenced by the original xvt
 * design of this module. See info relating to the original xvt elsewhere
 * in this package.
 *----------------------------------------------------------------------*/

/*****************************************************************
 ***                                                           ***
 ***       Terminal emulator screen functions                  ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_ITERM_SCREEN_C_

#include "EZ_Widget.h"

#define	TABSIZE		8	/* default tab size */
#define SPACE_CHAR ' '

/****************************************************************************/
/* zero both text and rendition */
#define ZERO_LINES(ezt, ptext,prend,nlines)	do {\
      int n = (nlines) * (EZ_ITermNCol(ezt)+1);\
      if (n > 0) {\
         memset ((ptext), 0, (n * sizeof(text_t)));\
         memset ((prend), 0, (n * sizeof(rend_t)));\
        }\
      } while (0)

/* fill text with spaces and fill rendition */
#define	BLANK_FILL(ptext,prend,nchars,fstyle)	do {\
     int n = (nchars);\
     if (n > 0) {\
        rend_t * p = prend;\
        memset ((ptext), SPACE_CHAR, (n * sizeof(text_t)));\
        while (n-- > 0) *p++ = (fstyle);\
      }\
    } while (0)
/****************************************************************************
 *
 * From EZ_ITermCommand.c
 */
EXTERN  void EZ_TermStartCommand MY_ANSIARGS((EZ_Widget *ezterm, char **args));
EXTERN  void EZ_TermStopCommand MY_ANSIARGS((EZ_Widget *ezterm, int sig));

EXTERN  void tt_resize MY_ANSIARGS((EZ_Widget *ezterm));
EXTERN  void tt_write MY_ANSIARGS((EZ_Widget* ezterm, char *buf, int nbytes));
EXTERN  void EZ_ITermEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EXTERN  void tt_printf  MY_VARARGS_D(void *, the_args);

EXTERN  void xterm_seq MY_ANSIARGS((EZ_Widget *ezterm, int op, char *str));
EXTERN  void set_width MY_ANSIARGS((EZ_Widget *ezterm, int width));
EXTERN  void EZ_iterm_resize_window MY_ANSIARGS((EZ_Widget *ezterm));
/****************************************************************************/
void EZ_ITermReverseVideo MY_ANSIARGS((EZ_Widget *ezterm, int mode));
void EZ_SetITermColors MY_ANSIARGS((EZ_Widget *ezterm, int i, unsigned long pixel_value));
void EZ_ITermBTimerCb MY_ANSIARGS((EZ_Timer *timer, void *pdata));

void scr_reset MY_ANSIARGS((EZ_Widget *ezterm));
void scr_poweron MY_ANSIARGS((EZ_Widget *ezterm));
void scr_cursor MY_ANSIARGS((EZ_Widget *ezterm, int mode));
void scr_color MY_ANSIARGS((EZ_Widget *ezterm, unsigned int color, unsigned int Intensity));
void scr_rendition MY_ANSIARGS((EZ_Widget *ezterm, int set, int style));
void scr_add_lines MY_ANSIARGS((EZ_Widget *ezterm, unsigned char *str, int nlines, int cnt));
void scr_backspace MY_ANSIARGS((EZ_Widget *ezterm));
void scr_tab MY_ANSIARGS((EZ_Widget *ezterm, int count));
void scr_gotorc MY_ANSIARGS((EZ_Widget *ezterm, int row, int col, int relative));
void scr_index MY_ANSIARGS((EZ_Widget *ezterm, int dirn));
void scr_erase_line MY_ANSIARGS((EZ_Widget *ezterm, int mode));
void scr_erase_screen MY_ANSIARGS((EZ_Widget *ezterm, int mode));
void scr_E MY_ANSIARGS((EZ_Widget *ezterm));
void scr_insdel_lines MY_ANSIARGS((EZ_Widget *ezterm, int count, int insdel));
void scr_insdel_chars MY_ANSIARGS((EZ_Widget *ezterm, int count, int insdel));
void scr_scroll_region MY_ANSIARGS((EZ_Widget *ezterm,int top, int bot));
void scr_cursor_visible MY_ANSIARGS((EZ_Widget *ezterm, int mode));
void scr_autowrap MY_ANSIARGS((EZ_Widget *ezterm, int mode));
void scr_relative_origin MY_ANSIARGS((EZ_Widget *ezterm, int mode));
void scr_insert_mode MY_ANSIARGS((EZ_Widget *ezterm, int mode));
int scr_move_to MY_ANSIARGS((EZ_Widget *ezterm, int y, int len));
int scr_page MY_ANSIARGS((EZ_Widget *ezterm, int dirn, int nlines));
void scr_report_position MY_ANSIARGS((EZ_Widget *ezterm));
void scr_charset_choose MY_ANSIARGS((EZ_Widget *ezterm, int set));
void scr_charset_set MY_ANSIARGS((EZ_Widget *ezterm, int set, unsigned int ch));
void scr_expose MY_ANSIARGS((EZ_Widget *ezterm, int x, int y, int width, int height));
void scr_touch MY_ANSIARGS((EZ_Widget *ezterm));
void scr_set_tab MY_ANSIARGS((EZ_Widget *ezterm, int mode));
void scr_bell MY_ANSIARGS((EZ_Widget *ezterm));
void mouse_report MY_ANSIARGS((EZ_Widget *ezterm, XButtonEvent * ev));
void selection_make MY_ANSIARGS((EZ_Widget *ezterm, Time tm));
void selection_paste MY_ANSIARGS((EZ_Widget *ezterm, Window win, unsigned prop, int Delete));
void selection_request MY_ANSIARGS((EZ_Widget *ezterm, Time tm, int x, int y));
void selection_reset MY_ANSIARGS((EZ_Widget *ezterm));
void selection_clear MY_ANSIARGS((EZ_Widget *ezterm));
void selection_start MY_ANSIARGS((EZ_Widget *ezterm, int x, int y));
void selection_extend MY_ANSIARGS((EZ_Widget *ezterm, int x, int y));
void selection_click MY_ANSIARGS((EZ_Widget *ezterm, int clicks, int x, int y));
void scr_refresh MY_ANSIARGS((EZ_Widget *ezterm, int type));
/****************************************************************************/
static void selection_extend_colrow MY_ANSIARGS((EZ_Widget *ezterm, int col, int row));
static void selection_start_colrow MY_ANSIARGS((EZ_Widget *ezterm, int col, int row));
static void selection_setclr MY_ANSIARGS((EZ_Widget *ezterm, int set, int startr, 
                                          int startc, int endr, int endc));
static void PasteIt MY_ANSIARGS((EZ_Widget *ezterm, unsigned char *data, unsigned int nitems));
static void selection_check MY_ANSIARGS((EZ_Widget *ezterm));
static int scroll_text MY_ANSIARGS((EZ_Widget *ezterm, int row1, int row2, int count));
static int scroll_text MY_ANSIARGS((EZ_Widget *term, int row1, int row2, int count));
static void selection_check MY_ANSIARGS((EZ_Widget *term));
static void blank_lines MY_ANSIARGS((EZ_Widget *ezterm, text_t * text, rend_t * rend,
                                     int count, int ncol));
static void set_font_style MY_ANSIARGS((EZ_Widget *ezterm));
/****************************************************************************/
static char *EZ_rs_cutchars = "\t \"&'()*,;<=>?@[\\]^`{|}~";
static char *xtermColors[] = 
{
  "White", 
  "Black",		
  /* low-intensity colors */
  "Black",			/* 0: black             (#000000) */
  "Red3",			/* 1: red               (#CD0000) */
  "Green3",			/* 2: green             (#00CD00) */
  "Yellow3",			/* 3: yellow            (#CDCD00) */
  "Blue3",			/* 4: blue              (#0000CD) */
  "Magenta3",			/* 5: magenta           (#CD00CD) */
  "Cyan3",			/* 6: cyan              (#00CDCD) */
  "AntiqueWhite",		/* 7: white             (#FAEBD7) */
  "Gray60",                     /* 8 */
   /* high-intensity colors */
  "Grey25",			/* 9: bright black      (#404040) */
  "Red",			/* 1/10: bright red     (#FF0000) */
  "Green",			/* 2/11: bright green   (#00FF00) */
  "Yellow",			/* 3/12: bright yellow  (#FFFF00) */
  "Blue",			/* 4/13: bright blue    (#0000FF) */
  "Magenta",			/* 5/14: bright magenta (#FF00FF) */
  "Cyan",			/* 6/15: bright cyan    (#00FFFF) */
  "White",			/* 7/16: bright white   (#FFFFFF) */
  "Gray90",			/* 7/17: bright white   (#FFFFFF) */
  NULL,
};
/****************************************************************************/
void EZ_SetITermColors(ezterm, i, pv)
     EZ_Widget *ezterm; int i; unsigned long pv;
{
  if(ezterm && i >=0 && i < 20)
    {
      screen_t *scr = EZ_ITermScreen(ezterm);
      if(i <= 8) i += 2; else i += 3;
      scr->pixColors[i] = pv;
      scr->cmasks |= (1 << i);
    }
}
unsigned long EZ_GetITermColors(ezterm, i)
     EZ_Widget *ezterm; int i;
{
  if(ezterm && i >=0 && i < 20)
    {
      screen_t *scr = EZ_ITermScreen(ezterm);
      if(i <= 10) i -= 2; else i -= 3;
      if(scr->cmasks & (1 << i))
        return( scr->pixColors[i]);
    }
  return(~0);
}
/****************************************************************************/

void EZ_InitITermScreen(wptr) EZ_Widget *wptr;
{
  unsigned int i = 0;

  screen_t *scr = EZ_ITermScreen(wptr) = (screen_t *)my_malloc(sizeof(screen_t), _ITERM_DATA_);
  memset(scr, 0, sizeof(screen_t));
  scr->flags = Screen_DefaultFlags;
  
  while(xtermColors[i] != NULL)
    { scr->pixColors[i] = EZ_AllocateColorFromName(xtermColors[i]); i++;}
  while(i < 23) { scr->pixColors[i] = 0; i++;}

  scr->drawn_text = NULL;
  scr->drawn_rend = NULL;
  scr->tabs = NULL;
  scr->linebuf = NULL;

  {
    char *cc = scr->charsets;
    cc[0] = cc[1] = cc[2] = cc[3] = 'B';
  }


  {
    SaveScreen *sscr;
    sscr = EZ_ITermSaveScreen(wptr) = (SaveScreen *)my_malloc(sizeof(SaveScreen), _ITERM_DATA_);
    memset(sscr, 0, sizeof(SaveScreen));
    sscr->charset_char = 'B';
    sscr->rstyle = DEFAULT_RSTYLE;
  }

  {
    selection_t *sel;
    sel = EZ_ITermSelection(wptr) = (selection_t *)my_malloc(sizeof(selection_t), _ITERM_DATA_);
    memset(sel, 0, sizeof(selection_t));    
  }
  scr_reset(wptr);
  scr_poweron(wptr);
}


void EZ_DestroyITermScreen(wptr) EZ_Widget *wptr;
{
  if(EZ_ITermScreen(wptr))
    my_free((void *)EZ_ITermScreen(wptr));
  EZ_ITermScreen(wptr) = NULL;
  if(EZ_ITermSaveScreen(wptr))
    my_free((void *)EZ_ITermSaveScreen(wptr));
  EZ_ITermSaveScreen(wptr) = NULL;
  if(EZ_ITermSelection(wptr))
    my_free((void *)EZ_ITermSelection(wptr));
  EZ_ITermSelection(wptr) = NULL;
}

void EZ_ITermReverseVideo(ezterm, mode) EZ_Widget *ezterm; int mode;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int nrow = EZ_ITermONRow(ezterm);
  int ncol = EZ_ITermONCol(ezterm);      
  int saveLines = EZ_ITermSaveLines(ezterm);
  rend_t rstyle = EZ_ITermRStyle(ezterm);
  
  if(EZ_WidgetMapped(ezterm))      
    {
      EZ_ITermRVideoPending(ezterm) = 0;
      if(EZ_ITermRVideo(ezterm) != mode)
        {
          register int x, count;

          EZ_ITermRVideo(ezterm) = mode;
          rstyle ^= RS_RVid;
          EZ_ITermRStyle(ezterm) = rstyle;
          count = (nrow + saveLines) * (ncol + 1);
          for (x = 0; x < count; x++)
            screen->rend[x] ^= RS_RVid;
          EZ_DrawWidget(ezterm);
        }
    }
  else
    {
      EZ_ITermRVideoPending(ezterm) = mode;
    }
}

/****************************************************************************/

static void blank_lines(ezterm, text, rend, count, ncol)
  EZ_Widget *ezterm; text_t *text; rend_t *rend; int count, ncol;
{
  int r;
  if(count <= 0) return;

  /* fill with blank lines */
  {
    rend_t fstyle = EZ_ITermRStyle(ezterm);
    BLANK_FILL(text, rend, (count * (ncol + 1)), fstyle);
  }
  /* terminate each line */
  text += (ncol);
  for(r = 0; r < count; r++, text += (ncol + 1))
    *text = '\0';
}
/****************************************************************************/
/*
 * called whenever the screen needs to be repaired 
 * due to re-sizing or initialization
 */
void scr_reset(ezterm)
  EZ_Widget *ezterm;
{
  int i;
  int prev_nrow, prev_ncol;
  int nrow, ncol = 0, oncol;
  screen_t *scr;

  prev_nrow = EZ_ITermONRow(ezterm);
  prev_ncol = EZ_ITermONCol(ezterm);
  nrow = EZ_ITermNRow(ezterm);
  ncol = EZ_ITermNCol(ezterm);
  scr  = EZ_ITermScreen(ezterm);
  EZ_ITermViewStart(ezterm) = 0;

  if(prev_ncol == ncol && prev_nrow ==nrow)  return;

  /* In case rows/columns are invalid */
  if(ncol <= 0) { ncol = EZ_ITermNCol(ezterm) = 80;}
  if(nrow <= 0) { nrow = EZ_ITermNRow(ezterm) = 24;}
  
  oncol = MINV(prev_ncol, ncol);

  if(prev_nrow < 0) {EZ_ITermNScrolled(ezterm) = 0; }

  if(1)
    {
      text_t    *old_text;
      rend_t    *old_rend;
      int       r, oldr, oldr_max;
      int       histsize = (EZ_ITermSaveLines(ezterm));

      /* copy from old buffer to new buffer, as appropriate. */
      oldr = oldr_max = prev_nrow;
      if(prev_nrow > 0)
	{
	  int n = (nrow - prev_nrow);
	  if(n > 0)		/* window made taller */
	    {
	      oldr = (nrow);
	      if(histsize)
		{
		  int nscrolled = EZ_ITermNScrolled(ezterm);
		  if(n <= nscrolled)	/* enough */
		    {
		      scr->row += (n);
		      oldr = oldr_max;
		    }
		  else
		    {
		      scr->row += nscrolled;
		      oldr -= nscrolled;
		    }
		  EZ_ITermNScrolled(ezterm) -= n;
		}
	    }
	  else if(n < 0)	/* window made shorter */
	    {
	      if(scr->row < nrow)
		{
		  oldr_max = nrow;
		}
	      else
		/* put extra into scrolled */
		{
		  oldr_max = (scr->row + 1);
		  if(histsize)
		    { EZ_ITermNScrolled(ezterm) += (oldr_max - nrow);}
		}
	      oldr = oldr_max;
	    }
	  oldr_max += histsize;
	  oldr += histsize;
	  oldr--;
	}
      
      if(scr->row < 0)	scr->row = 0;
      else if(scr->row >= nrow)
	scr->row = (nrow - 1);

      if(scr->col < 0)	scr->col = 0;
      else if(scr->col >= ncol)
	scr->col = (ncol - 1);

      /* reset scroll regions */
      scr->tscroll = 0;
      scr->bscroll = (nrow - 1);

      old_text = scr->text;
      old_rend = scr->rend;

      scr->text = my_malloc( ((histsize + nrow) * (ncol + 1)) * sizeof(text_t), _ITERM_DATA_);
      scr->rend = my_malloc( ((histsize + nrow) * (ncol + 1)) * sizeof (rend_t), _ITERM_DATA_);

      blank_lines(ezterm, scr->text, scr->rend, (histsize + nrow), ncol);
      if(oncol > 0)
	{
	  for(r = (nrow + histsize - 1);  r >= 0 && oldr >= 0;   r--, oldr--)
	    {
	      if(oldr < oldr_max)
		{
		  int roffset = r * (ncol + 1);
		  int oroffset = oldr * (prev_ncol + 1);
		  memcpy (&scr->text[roffset], &old_text[oroffset],
			  oncol * sizeof (text_t));
		  memcpy (&scr->rend[roffset], &old_rend[oroffset],
			  oncol * sizeof (rend_t));
		}
	    }
	}
      my_free(old_text);
      my_free(old_rend);
    }

  /* Make sure the cursor is on the screen */
  if(EZ_ITermNScrolled(ezterm) < 0) {EZ_ITermNScrolled(ezterm) = 0;}
  else if(EZ_ITermNScrolled(ezterm) > EZ_ITermSaveLines(ezterm))
    { EZ_ITermNScrolled(ezterm)  = EZ_ITermSaveLines(ezterm);}

  EZ_ITermONCol(ezterm) = ncol;
  EZ_ITermONRow(ezterm) = nrow;

  if(scr->drawn_text)
    {
      scr->drawn_text =  my_realloc(scr->drawn_text,
                                    (nrow * (ncol + 1)) * sizeof (text_t),
                                    _ITERM_DATA_);
      scr->drawn_rend = my_realloc(scr->drawn_rend,
                                   (nrow * (ncol + 1)) * sizeof (rend_t),
                                   _ITERM_DATA_);
    }
  else
    {
      scr->drawn_text =  my_malloc((nrow * (ncol + 1)) * sizeof (text_t),
                                    _ITERM_DATA_);
      scr->drawn_rend = my_malloc((nrow * (ncol + 1)) * sizeof (rend_t),
                                   _ITERM_DATA_);

    }
  ZERO_LINES(ezterm, scr->drawn_text, scr->drawn_rend, nrow);
  
  if(scr->tabs)
    scr->tabs = my_realloc(scr->tabs, ncol * sizeof (char), _ITEMR_DATA_);
  else
    scr->tabs = my_malloc(ncol * sizeof (char), _ITEMR_DATA_);
  memset(scr->tabs, 0, ncol * sizeof (char));
  for(i = 0; i < ncol; i += TABSIZE)  scr->tabs[i] = 1;

  if(scr->linebuf)
    scr->linebuf = my_realloc(scr->linebuf, (ncol + 1) * sizeof (text_t), 
                              _ITERM_DATA_);
  else 
    scr->linebuf = my_malloc( (ncol + 1) * sizeof (text_t), 
                              _ITERM_DATA_);

  /* ensure the cursor is on the screen */
  {
    SaveScreen *save = EZ_ITermSaveScreen(ezterm);

    if(save->row >= nrow)  save->row = (nrow - 1);
    if(save->col >= ncol)  save->col = (ncol - 1);
    tt_resize(ezterm);
  }
}

/*  Restore power-on configuration 
 *
 * Clears screen, restores default fonts, etc
 * also reset the scrollback buffer
 */
void scr_poweron(ezterm) EZ_Widget *ezterm;
{

  screen_t *scr = EZ_ITermScreen(ezterm);

  EZ_ITermViewStart(ezterm) = 0;
  EZ_ITermNScrolled(ezterm) = 0; 
  memset(scr->charsets, 'B', 4);
  
  EZ_ITermRVideo(ezterm) = 0;
  scr_rendition(ezterm, 0, ~RS_None);

  scr_erase_screen(ezterm,  2);
  if(1)
    {
      scr->tscroll = 0;
      scr->bscroll = (EZ_ITermNRow(ezterm) - 1);
      scr->row = scr->col = 0;
      scr->charset = 0;
      scr->flags = Screen_DefaultFlags;
    }
  scr_cursor(ezterm, SAVE);
  ZERO_LINES(ezterm, scr->drawn_text, scr->drawn_rend, EZ_ITermNRow(ezterm));

  scr_reset(ezterm);
  if(EZ_WidgetMapped(ezterm))
    {
      XClearWindow(EZ_Display, EZ_WidgetWindow(ezterm));
      scr_refresh(ezterm, SLOW_REFRESH);
      EZ_DrawWidget(ezterm);
    }
}

/* Set the rstyle parameter to reflect the selected font */
static void set_font_style(ezterm) EZ_Widget *ezterm;
{
  rend_t rstyle =  EZ_ITermRStyle(ezterm);
  screen_t *screen =  EZ_ITermScreen(ezterm);
  char   *charsets = screen->charsets;
  
  rstyle  &= ~RS_fontMask;
  switch(charsets[screen->charset])
    {
      /* DEC Special Character and Line Drawing Set */
    case '0':
      rstyle |= RS_acsFont;
      break;
      /* United Kingdom (UK) */
    case 'A':
      rstyle |= RS_ukFont;
      break;
      /* United States (USASCII) */
      /* case 'B': break; *//* ascii */

      /* <: Multinational character set */
      /* 5: Finnish character set */
      /* C: Finnish character set */
      /* K: German character set */
    }
  EZ_ITermRStyle(ezterm) = rstyle;
}

/* Save/restore the cursor position and rendition style */
void scr_cursor(ezterm, mode) EZ_Widget *ezterm; int mode;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  SaveScreen *save = EZ_ITermSaveScreen(ezterm);
  char   *charsets = screen->charsets;

  switch(mode)
    {
    case SAVE:
      save->row = screen->row;
      save->col = screen->col;
      save->rstyle = screen->rstyle;
      save->charset = screen->charset;
      save->charset_char = charsets[save->charset];
      break;
    case RESTORE:
      screen->row = save->row;
      screen->col = save->col;
      screen->rstyle = save->rstyle;
      screen->charset = save->charset;
      charsets[screen->charset] = save->charset_char;
      set_font_style(ezterm);
      break;
    }
}


/* Set foreground/background text color 
 *
 * Intensity:
 *      RS_Bold  = foreground
 *      RS_Blink = background
 */
void scr_color(ezterm, color, Intensity)
     EZ_Widget *ezterm; unsigned int color; unsigned int Intensity;
{
  rend_t rstyle =  EZ_ITermRStyle(ezterm);
  switch(color)
    {
    case restoreFG:
      color = itfgColor;
      Intensity = RS_Bold;
      break;
    case restoreBG:
      color = itbgColor;
      Intensity = RS_Blink;
      break;
    default:
      if( (rstyle & Intensity) && color >= minColor && color <= maxColor)
        {
          color += (minBright - minColor);
        }
      else if(color >= minBright && color <= maxBright)
        {
          if(rstyle & Intensity) return;		/* already bold enough! */
          color -= (minBright - minColor);
        }
      break;
    }
  switch(Intensity)
    {
    case RS_Bold:
      rstyle = SET_FGCOLOR(rstyle, color);
      EZ_ITermRStyle(ezterm) = rstyle;
      break;
    case RS_Blink:
      rstyle = SET_BGCOLOR(rstyle, color);
      EZ_ITermRStyle(ezterm) = rstyle;
      break;
    }
}

/* Change the rendition style */
void scr_rendition(ezterm, set, style)
     EZ_Widget *ezterm; int set; int style;
{
  unsigned int color, colorfgbg;
  rend_t rstyle = EZ_ITermRStyle(ezterm);
  colorfgbg = EZ_ITermColorFgBg(ezterm);

  if(set)
    {
      rstyle |= style;		/* set rendition */
      EZ_ITermRStyle(ezterm) = rstyle;
      switch(style)
	{
	case RS_Bold:
	  color = GET_FGCOLOR(rstyle);
	  scr_color(ezterm, (color == itfgColor ?
		     GET_FGCOLOR(colorfgbg) : color), RS_Bold);
	  break;
	case RS_Blink:
	  color = GET_BGCOLOR (rstyle);
	  scr_color(ezterm, (color == itbgColor ?
		      GET_BGCOLOR(colorfgbg) : color), RS_Blink);
	  break;
	case RS_RVid:
	  if(EZ_ITermRVideo(ezterm))
            {
              rstyle &= ~(RS_RVid);	/* reverse video mode */
              EZ_ITermRStyle(ezterm) = rstyle;
            }
	  break;
	}
    }
  else
    {
      rstyle &= ~(style);	/* unset rendition */
      EZ_ITermRStyle(ezterm) = rstyle;      
      switch (style)
	{
	case ~RS_None:		/* default fg/bg colors */
	  rstyle = DEFAULT_RSTYLE;
	  if(EZ_ITermRVideo(ezterm))
	    rstyle |= RS_RVid;	/* reverse video mode */
          EZ_ITermRStyle(ezterm) = rstyle;
	  break;
	case RS_Bold:
	  color = GET_FGCOLOR (rstyle);
	  if(color >= minBright && color <= maxBright)
	    {
	      scr_color(ezterm, color, RS_Bold);
              rstyle = EZ_ITermRStyle(ezterm);    
              /* scr_color(ezterm, color - (minBright - minColor), RS_Bold); */
	      if((rstyle & RS_fgMask) == (colorfgbg & RS_fgMask))
		scr_color(ezterm, restoreFG, RS_Bold);
	    }
	  break;
	case RS_Blink:
	  color = GET_BGCOLOR(rstyle);
	  if(color >= minBright && color <= maxBright)
	    {
	      scr_color(ezterm, color, RS_Blink);
              rstyle = EZ_ITermRStyle(ezterm);
	      /* scr_color(ezterm, color - (minBright - minColor), RS_Blink); */
	      if((rstyle & RS_bgMask) == (colorfgbg & RS_bgMask))
		scr_color(ezterm, restoreBG, RS_Blink);
	    }
	  break;
	case RS_RVid:
          if(EZ_ITermRVideo(ezterm))
	    rstyle |= RS_RVid;	/* reverse video mode */
          EZ_ITermRStyle(ezterm) = rstyle;
	  break;
	}
    }
}


/* Add lines */
void scr_add_lines(ezterm, str, nlines, cnt)
     EZ_Widget *ezterm; unsigned char *str; int nlines; int cnt;
{
  int i, roffset;		/* row offset */
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);
  rend_t rstyle = EZ_ITermRStyle(ezterm);

  EZ_ITermViewStart(ezterm) = 0;
  if(EZ_ITermSelection(ezterm)->op)
    selection_check(ezterm);

  /* do scrolling up front, most of the time */
  if(nlines > 0)
    {
      nlines += (screen->row - screen->bscroll);
      if((nlines > 0) &&
	 (screen->tscroll == 0) &&
	 (screen->bscroll == (nrow - 1)))
	{
	  scroll_text(ezterm, screen->tscroll, screen->bscroll, nlines);

	  /* add text into the scrollback buffer */
	  screen->row -= nlines;
	  if(screen->row < -saveLines)
	    screen->row = -saveLines;
	}
    }
  
  if(screen->col >= ncol)
    screen->col = (ncol - 1);

  roffset = (screen->row + saveLines) * (ncol + 1);
  for(i = 0; i < cnt; i++)
    {
      /*
       * Adds a single character at the current cursor location
       * new lines in the string
       */
      {
	switch(str[i])
	  {
	  case 127:
	    continue;
	    break;
	  case '\t':
	    scr_tab(ezterm, 1);
	    continue;
	    break;
	  case '\n':
	    screen->flags &= ~Screen_WrapNext;
	    screen->text[roffset + (ncol)] = '\0';
	    if(screen->row == screen->bscroll)
	      scroll_text(ezterm, screen->tscroll, screen->bscroll, 1);
	    else if(screen->row < (nrow - 1))
	      {
		screen->row++;
		roffset = (screen->row + saveLines) * (ncol + 1);
	      }
	    continue;
	    break;
	  case '\r':
	    screen->col = 0;
	    screen->flags &= ~Screen_WrapNext;
	    screen->text[roffset + (ncol)] = '\0';
	    continue;
	    break;
	  default:
	    break;
	  }
      }

      if(screen->flags & Screen_WrapNext)
	{
	  screen->text[roffset + (ncol)] = '\n';		/* wrap line */
	  if(screen->row == screen->bscroll)
	    scroll_text(ezterm, screen->tscroll, screen->bscroll, 1);
	  else if(screen->row < (nrow - 1))
	    {
	      screen->row++;
	      roffset = (screen->row + saveLines) * (ncol + 1);
	    }
	  screen->col = 0;
	  screen->flags &= ~Screen_WrapNext;
	}
      if(screen->flags & Screen_Insert)
	scr_insdel_chars(ezterm, 1, INSERT);
      screen->text[roffset + (screen->col)] = str[i];
      screen->rend[roffset + (screen->col)] = rstyle;
      screen->rend[roffset + ncol] |= (rstyle & RS_Blink); /* mark ! */
  
      screen->col++;
      if(screen->col == ncol)
	{
	  screen->col--;
	  if(screen->flags & Screen_Autowrap)
	    screen->flags |= Screen_WrapNext;
	  else
	    screen->flags &= ~Screen_WrapNext;
	}
    }
}
/* Scroll text on the screen 
 *
 * Scroll COUNT lines from ROW1 to ROW2 inclusive (ROW1 <= ROW2)
 * scrolling is up for a +ve COUNT and down for a -ve COUNT
 */
static int scroll_text(ezterm, row1, row2, count)
     EZ_Widget *ezterm; int row1; int row2; int count;
{
  int r;
  text_t *t_dst, *t_src;
  rend_t *r_dst, *r_src;
  selection_t *selection = EZ_ITermSelection(ezterm);
  int ncol = EZ_ITermONCol(ezterm);      
  int saveLines = EZ_ITermSaveLines(ezterm);
  screen_t *screen = EZ_ITermScreen(ezterm);


  if(selection->op)	/* move selected region too */
    {
      selection->beg.row -= count;
      selection->end.row -= count;
      selection->mark.row -= count;
    }
  if(count > 0)		/* scroll up */
    {
      int n, x;
      /* if the line scrolls off the top of the screen,
       * shift the entire scrollback buffer too */
      if(row1 == 0)
	{
	  row1 = -saveLines;
	  EZ_ITermNScrolled(ezterm) += count;
	  if(EZ_ITermNScrolled(ezterm) > saveLines)
	    EZ_ITermNScrolled(ezterm) = saveLines;
	}
      x = ((row1 + saveLines) * (ncol + 1));
      t_dst = &(screen->text[x]);
      r_dst = &(screen->rend[x]);

      n = (row2 - row1 + 1);
      if(count > n)
	{
	  count = n;
	  n = 0;
	}
      else
	{
	  n -= count;
	}

      x += count * (ncol + 1);
      t_src = &(screen->text[x]);
      r_src = &(screen->rend[x]);

      /* Forward overlapping memcpy's -- probably OK */
      if(n > 0)
	{
	  n *= (ncol + 1);
	  memcpy(t_dst, t_src, n * sizeof (text_t));
	  memcpy(r_dst, r_src, n * sizeof (rend_t));
	  t_dst += n;
	  r_dst += n;
	}
      /* copy blank lines in at the bottom */
      blank_lines(ezterm, t_dst, r_dst, count, ncol);
    }
  else if(count < 0)		/* scroll down */
    {
      int x;
      rend_t fstyle = EZ_ITermRStyle(ezterm);

      /* do one line at a time to avoid backward overlapping memcpy's */
      x = (row2 + saveLines) * (ncol + 1);
      t_dst = &(screen->text[x]);
      r_dst = &(screen->rend[x]);

      x += (count) * (ncol + 1);
      t_src = &(screen->text[x]);
      r_src = &(screen->rend[x]);
      for(r = row2; r >= (row1 - count); r--)
	{
	  memcpy(t_dst, t_src, (ncol + 1) * sizeof (text_t));
	  t_dst -= (ncol + 1);
	  t_src -= (ncol + 1);
	  memcpy(r_dst, r_src, (ncol + 1) * sizeof (rend_t));
	  r_dst -= (ncol + 1);
	  r_src -= (ncol + 1);
	}
      
      /* copy blank lines in at the top */
      for(; r >= row1; r--)
	{
	  BLANK_FILL(t_dst, r_dst, ncol, fstyle);
	  t_dst[ncol] = '\0';
	  t_dst -= (ncol + 1);
	  r_dst -= (ncol + 1);
	}
    }
  return count;
}

/* Handle a backspace */
void scr_backspace(ezterm) EZ_Widget *ezterm;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int ncol = EZ_ITermONCol(ezterm);      
  int saveLines = EZ_ITermSaveLines(ezterm);

  if(EZ_ITermSelection(ezterm)->op)
    selection_check(ezterm);
  if(screen->col == 0 && screen->row > 0)
    {
      screen->row--;
      screen->col = (ncol - 1);
    }
  else if (screen->flags & Screen_WrapNext)
    {
      int roffset = (screen->row + saveLines) * (ncol + 1);
      screen->text[roffset + (ncol)] = '\0';
      screen->flags &= ~Screen_WrapNext;
    }
  else /* zzz */
    scr_gotorc(ezterm, 0, -1, RELATIVE);
}

/* Move the cursor to a new tab position 
 *
 * COUNT is +ve, move forward.  COUNT is -ve, move backward
 */
void scr_tab(ezterm, count) EZ_Widget *ezterm; int count;
{
  int ncol = EZ_ITermONCol(ezterm);      
  screen_t *screen = EZ_ITermScreen(ezterm);
  char *tabs = screen->tabs;
  int x = screen->col;

  if(count > 0)		/* tab forward */
    {
      int i;
      for(i = x + 1; i < ncol; i++)
	{
	  if(tabs[i])
	    {
	      x = i;
	      count--;
	      if(!count)
		break;
	    }
	}
    }
  else if(count < 0)		/* tab backward */
    {
      int i;
      count = -count;
      for (i = x - 1; i >= 0; i--)
	{
	  if(tabs[i])
	    {
	      x = i;
	      count--;
	      if(!count)
		break;
	    }
	}
    }
  else
    return;
  if (x != screen->col) /* zzz */
    scr_gotorc(ezterm, 0, x, R_RELATIVE);
}
/* Move the cursor to a new position 
 *
 * The relative argument is a pair of flags that specify relative
 * rather than absolute motion.
 */
void scr_gotorc(ezterm, row, col, relative)
     EZ_Widget *ezterm; int row; int col; int relative;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);
  EZ_ITermViewStart(ezterm) = 0;

  screen->col = (relative & C_RELATIVE ? (screen->col + col) : col);

  if(screen->col < 0)
    screen->col = 0;
  else if(screen->col >= ncol)
    screen->col = (ncol - 1);

  if(screen->flags & Screen_WrapNext)
    {
      int roffset = (screen->row + saveLines) * (ncol + 1);
      screen->text[roffset + (ncol)] = '\0';
      screen->flags &= ~Screen_WrapNext;
    }

  if(relative & R_RELATIVE)
    {
      if(row > 0)
	{
	  if((screen->row <= screen->bscroll) &&
	     (screen->row + row > screen->bscroll))
	    screen->row = screen->bscroll;
	  else
	    screen->row += row;
	}
      else if(row < 0)
	{
	  if((screen->row >= screen->tscroll) &&
	      (screen->row + row < screen->tscroll))
	    screen->row = screen->tscroll;
	  else
	    screen->row += row;
	}
    }
  else
    {
      if(screen->flags & Screen_Relative)	/* relative origin mode */
	{
	  screen->row = row + screen->tscroll;
	  if(screen->row > screen->bscroll)
	    screen->row = screen->bscroll;
	}
      else
	screen->row = row;
    }
  if(screen->row < 0)
    screen->row = 0;
  else if(screen->row >= nrow)
    screen->row = (nrow - 1);
}

/* Move the cursor down one line and scroll if necessary */
void scr_index(ezterm, dirn) EZ_Widget *ezterm; int dirn;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);

  EZ_ITermViewStart(ezterm) = 0;

  if(screen->flags & Screen_WrapNext)
    {
      int roffset = (screen->row + saveLines) * (ncol + 1);
      screen->text[roffset + (ncol)] = '\0';
      screen->flags &= ~Screen_WrapNext;
    }

  if((screen->row == screen->bscroll && dirn == UP) ||
     (screen->row == screen->tscroll && dirn == DN))
    scroll_text(ezterm, screen->tscroll, screen->bscroll, dirn);
  else
    screen->row += dirn;
  if(EZ_ITermSelection(ezterm)->op)
    selection_check(ezterm);
}

/* Erase part or the whole of a line */
void scr_erase_line(ezterm, mode) EZ_Widget *ezterm; int mode;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);
  int count, roffset = (screen->row + saveLines) * (ncol + 1);
  rend_t fstyle = EZ_ITermRStyle(ezterm);


  EZ_ITermViewStart(ezterm) = 0;

  if(screen->flags & Screen_WrapNext)
    {
      screen->text[roffset + (ncol)] = '\0';
      screen->flags &= ~Screen_WrapNext;
    }
  switch(mode)
    {
    case 0:			/* erase to end */
      roffset += (screen->col);
      count = (ncol - screen->col);
      break;

    case 1:			/* erase to beginning */
      count = (screen->col + 1);
      break;

    case 2:			/* erase entire */
      count = (ncol);
      break;

    default:
      return;
    }
  
  BLANK_FILL(&screen->text[roffset], &screen->rend[roffset], count, fstyle);

  if(EZ_ITermSelection(ezterm)->op)
    selection_check(ezterm);
}

/* Erase part or the whole of the screen */
void scr_erase_screen(ezterm, mode) EZ_Widget *ezterm; int mode;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);
  int count, roffset = (saveLines) * (ncol + 1);
  EZ_ITermViewStart(ezterm) = 0;

  switch (mode)
    {
    case 0:			/* erase to end */
      scr_erase_line(ezterm, 0);
      count = (nrow - (screen->row + 1));
      roffset += ((screen->row + 1) * (ncol + 1));
      break;

    case 1:			/* erase to beginning */
      scr_erase_line(ezterm, 1);
      count = screen->row;
      break;
      
    case 2:			/* erase entire */
      count = nrow;
      break;

    default:
      return;
      break;
    }
  blank_lines(ezterm, &screen->text[roffset], &screen->rend[roffset], count, ncol);
}

/* Fill screen with E's */
void scr_E(ezterm) EZ_Widget *ezterm;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);
  int r, roffset = (saveLines) * (ncol + 1);

  EZ_ITermViewStart(ezterm) = 0;

  memset (&screen->text[roffset], 'E',
	  ((nrow) * (ncol + 1)) * sizeof (text_t));
  for(r = 0; r < (nrow); r++, roffset += (ncol + 1))
    screen->text[roffset + ncol] = '\0';
}

/* Insert or Delete COUNT lines and scroll 
 *
 * insdel == +1
 *      delete lines, scroll up the bottom of the screen to fill the gap
 * insdel == -1
 *      insert lines, scroll down the lower lines
 * other values of insdel are undefined
 */
void scr_insdel_lines(ezterm, count, insdel)
     EZ_Widget *ezterm; int count; int insdel;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);


  if(screen->row > screen->bscroll)
    return;

  if(count > (screen->bscroll - screen->row + 1))
    {
      if(insdel == DELETE)
	return;
      else if(insdel == INSERT)
	count = (screen->bscroll - screen->row + 1);
    }
  EZ_ITermViewStart(ezterm) = 0;

  if (screen->flags & Screen_WrapNext)
    {
      int roffset = (screen->row + saveLines) * (ncol + 1);
      screen->text[roffset + (ncol)] = '\0';
      screen->flags &= ~Screen_WrapNext;
    }
  scroll_text(ezterm, screen->row, screen->bscroll, insdel * count);
}

/* Insert or Delete COUNT characters from the current position 
 *
 * insdel == +2, erase  chars
 * insdel == +1, delete chars
 * insdel == -1, insert chars
 */
void scr_insdel_chars(ezterm, count, insdel)
     EZ_Widget *ezterm; int count; int insdel;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);
  int roffset = (screen->row + saveLines) * (ncol + 1);
  text_t *text, *textend;
  rend_t *rend, *rendend;


  if(insdel == ERASE)
    {
      if(count > screen->col)
	count = screen->col;
      if(count <= 0)
	return;
      screen->col -= count;	/* move backwards */
      insdel = DELETE;		/* delete chars */
    }
  else if(count > (ncol - screen->col))
    {
      count = (ncol - screen->col);
    }
  if(count <= 0)
    return;

  EZ_ITermViewStart(ezterm) = 0;

  if(EZ_ITermSelection(ezterm)->op)
    selection_check(ezterm);

  screen->text[roffset + (ncol)] = '\0';
  screen->flags &= ~Screen_WrapNext;
  
  text = &screen->text[roffset + (screen->col)];
  rend = &screen->rend[roffset + (screen->col)];
  if(insdel == DELETE)
    {
      rend_t fstyle = EZ_ITermRStyle(ezterm);
      /* overlapping copy */
      for( ; (*text && text[count]); text++, rend++)
	{
	  *text = text[count];
	  *rend = rend[count];
	}

      /* fill in the end of the line */
      for ( ; *text; text++, rend++)
	{
	  *text = SPACE_CHAR;
	  *rend = fstyle;
	}
    }
  else
    {
      /* INSERT count characters */
      rend_t fstyle = EZ_ITermRStyle(ezterm);
      textend = &screen->text[roffset + (ncol - 1)];
      rendend = &screen->rend[roffset + (ncol - 1)];

      for ( ; (textend - count >= text); textend--, rendend--)
	{
	  *textend = *(textend - count);
	  *rendend = *(rendend - count);
	}

      /* fill in the gap */
      for ( ; (textend >= text); textend--, rendend--)
	{
	  *textend = SPACE_CHAR;
	  *rendend = fstyle;
	}
    }
}

/* Set the scroll region */
void scr_scroll_region(ezterm, top, bot)
     EZ_Widget *ezterm; int top; int bot;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);

  if(top < 0)       top = 0;
  if(bot >= nrow)   bot = (nrow - 1);
  if(top > bot) return;

  screen->tscroll = top;
  screen->bscroll = bot;
  scr_gotorc(ezterm, 0, 0, 0);
}

/* set visible/invisible cursor */
void scr_cursor_visible(ezterm, mode) EZ_Widget *ezterm; int mode;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  if(mode)
    screen->flags |= Screen_VisibleCursor;
  else
    screen->flags &= ~Screen_VisibleCursor;
}

/* Set/Unset automatic wrapping */
void scr_autowrap(ezterm, mode) EZ_Widget *ezterm; int mode;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  if(mode)
    screen->flags |= Screen_Autowrap;
  else
    screen->flags &= ~Screen_Autowrap;
}

/* Set/Unset margin origin mode 
 *
 * In absolute origin mode, line numbers are counted relative to top margin
 * of screen, the cursor can be moved outside the scrolling region. In
 * relative mode line numbers are relative to top margin of scrolling
 * region and the cursor cannot be moved outside
 */
void scr_relative_origin(ezterm, mode) EZ_Widget *ezterm; int mode;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  if(mode)
    screen->flags |= Screen_Relative;
  else
    screen->flags &= ~Screen_Relative;
  scr_gotorc(ezterm, 0, 0, 0);
}

/* Set/Unset automatic insert mode */
void scr_insert_mode(ezterm, mode) EZ_Widget *ezterm; int mode;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  if(mode)
    screen->flags |= Screen_Insert;
  else
    screen->flags &= ~Screen_Insert;
}

/* Move the display to line represented by scrollbar 
 *
 * Move the display so that line represented by scrollbar value Y is at
 * the top of the screen
 */
int scr_move_to(ezterm, y, len) EZ_Widget *ezterm; int y; int len;
{
  int nstart, ostart = EZ_ITermViewStart(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int nscrolled = EZ_ITermNScrolled(ezterm);
  EZ_ITermViewStart(ezterm) =
    ((len - y) * ((nrow - 1) + nscrolled)/ (len)) - (nrow - 1);

  nstart = EZ_ITermViewStart(ezterm);
  if(nstart < 0)
    { nstart = EZ_ITermViewStart(ezterm) = 0;}
  else if (nstart > nscrolled)
    { nstart = EZ_ITermViewStart(ezterm) = nscrolled;}
  
  return(nstart - ostart);
}

/* page the screen up/down NLINES */
int scr_page(ezterm, dirn, nlines) EZ_Widget *ezterm; int dirn; int nlines;
{
  int nstart, start = EZ_ITermViewStart(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int nscrolled = EZ_ITermNScrolled(ezterm);

  if(!dirn || !nlines)
    return 0;

  if(nlines <= 0)   nlines = 1;
  else if(nlines > nrow)
    nlines = nrow;
  EZ_ITermViewStart(ezterm) +=  nlines * dirn;
  nstart = EZ_ITermViewStart(ezterm);

  if(nstart < 0)  { EZ_ITermViewStart(ezterm) = 0;}
  else if(nstart > nscrolled)
    { EZ_ITermViewStart(ezterm) =nscrolled;}

  return(EZ_ITermViewStart(ezterm) - start);
}

/* Report the current cursor position */
void scr_report_position(ezterm) EZ_Widget *ezterm;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  tt_printf(ezterm, "\033[%d;%dR", screen->row + 1, screen->col + 1);
}

/* Charset/Font functions */
/*
 * choose a font
 */
void scr_charset_choose(ezterm, set) EZ_Widget *ezterm; int set;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  screen->charset = set;
  set_font_style(ezterm);
}

/*
 * Set a font
 */
void scr_charset_set(ezterm, set, ch)
     EZ_Widget *ezterm; int set; unsigned int ch;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  char *charsets = screen->charsets;
  charsets[set] = (unsigned char) ch;
  set_font_style(ezterm);
}

/* scr_expose / scr_touch */
/*
 * for the box starting at x, y with size width, height
 * touch the displayed values
 */
void scr_expose(ezterm, x, y, width, height)
     EZ_Widget *ezterm; int x; int y; int width; int height;
{
  int row, col, end_row, end_col;
  screen_t *screen = EZ_ITermScreen(ezterm);  
  text_t *drawn_text = screen->drawn_text;
  rend_t *drawn_rend = screen->drawn_rend;
  int    nrow = EZ_ITermNRow(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);

  if(drawn_text == NULL)
    return;

  col = Pixel2Col(ezterm, x);
  row = Pixel2Row(ezterm, y);
  if(col < 0) col = 0;
  else if(col >= ncol) col = (ncol - 1);
  if(row < 0)  row = 0;
  else if(row >= nrow)  row = (nrow - 1);
  end_col = col + 1 + Pixel2Width(ezterm, width);
  end_row = row + 1 + Pixel2Height(ezterm,height);

  if(end_row >= nrow) end_row = (nrow - 1);
  if (end_col >= ncol) end_col = (ncol - 1);

  width = (end_col - col + 1);
  for( ; row <= end_row; row++)
    {
      int roffset = (col + row * (ncol + 1));
      memset (&drawn_text[roffset], 0, width * sizeof (text_t));
      memset (&drawn_rend[roffset], 0, width * sizeof (rend_t));
    }
}

/* touch the entire screen */
void scr_touch(ezterm) EZ_Widget *ezterm;
{
  scr_expose(ezterm, 0, 0, EZ_WidgetWidth(ezterm), EZ_WidgetHeight(ezterm));
}

/* Tabs: set/clear 
 *
 *  -1 = clear all tabs
 *  +0 = clear tab stop at current column
 *  +1 = set   tab stop at current column
 */
void scr_set_tab(ezterm, mode) EZ_Widget *ezterm; int mode;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int ncol = EZ_ITermONCol(ezterm);      
  char *tabs = screen->tabs;


  if(mode < 0)
    memset (tabs, 0, ncol * sizeof (char));
  else if(screen->col < ncol)
    tabs[screen->col] = (mode != 0);
}

/* toggle reverse video settings */

/* Handle receipt of a bell character */
void scr_bell(ezterm) EZ_Widget *ezterm;
{
  XBell(EZ_Display, 0);
}


/* Mouse Reporting 
 * add the bits:
 * @ 1 - shift
 * @ 2 - meta
 * @ 4 - ctrl
 */
#define ButtonNumber(x) ((x) == AnyButton ? 3 : ((x) - Button1))
#define KeyState(x) ((((x)&(ShiftMask|ControlMask))+(((x)&Mod1Mask)?2:0))<<2)
void mouse_report(ezterm, ev)
     EZ_Widget *ezterm; XButtonEvent *ev;
{
  tt_printf(ezterm, "\033[M%c%c%c",
	     (040 + ButtonNumber(ev->button) + KeyState(ev->state)),
	     (041 + Pixel2Col(ezterm, ev->x)),
	     (041 + Pixel2Row(ezterm, ev->y)));
}

/*
 * If (row,col) is within a selected region of text, remove the selection
 */
static void selection_check(ezterm) EZ_Widget *ezterm;
{
  int c1, c2, r1, r2;
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int nscrolled = EZ_ITermNScrolled(ezterm);
  selection_t *selection = EZ_ITermSelection(ezterm);

  if((selection->mark.row < -nscrolled) ||
      (selection->mark.row >= nrow) ||
      (selection->beg.row < -nscrolled) ||
      (selection->beg.row >= nrow) ||
      (selection->end.row < -nscrolled) ||
      (selection->end.row >= nrow))
    {
      selection_reset(ezterm);
      return;
    }

  r1 = (screen->row - EZ_ITermViewStart(ezterm));
  c1 = ((r1 - selection->mark.row) * (r1 - selection->end.row));

  /* selection->mark.row > screen->row - view_start
   * or
   * selection->end.row > screen->row - view_start
   */
  if (c1 < 0) selection_reset(ezterm);
  /* selection->mark.row == screen.row || selection->end.row == screen.row */
  else if(c1 == 0)
    {
      /* We're on the same row as the start or end of selection */
      if ((selection->mark.row < selection->end.row) ||
	  ((selection->mark.row == selection->end.row) &&
	   (selection->mark.col < selection->end.col)))
	{
	  r1 = selection->mark.row;
	  c1 = selection->mark.col;
	  r2 = selection->end.row;
	  c2 = selection->end.col;
	}
      else
	{
	  r1 = selection->end.row;
	  c1 = selection->end.col;
	  r2 = selection->mark.row;
	  c2 = selection->mark.col;
	}
      if ((screen->row == r1) && (screen->row == r2))
	{
	  if ((screen->col >= c1) && (screen->col <= c2))
	    selection_reset(ezterm);
	}
      else if(((screen->row == r1) && (screen->col >= c1)) ||
	       ((screen->row == r2) && (screen->col <= c2)))
	selection_reset(ezterm);
    }
}

/*
 * make the selection currently delimited by the selection end markers
 */
extern ITextLine TextWidgetKillBuffer;

void selection_make(ezterm, tm) 
     EZ_Widget *ezterm; Time tm;
{
  int  r, startr, startc, endr, endc;
  int  roffset;			/* row offset */
  int  klen;
  char *str, *sstr;
  selection_t *selection = EZ_ITermSelection(ezterm);
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);
  int nscrolled = EZ_ITermNScrolled(ezterm);

  switch (selection->op)
    {
    case SELECTION_CONT:
      break;

    case SELECTION_INIT:
      selection_reset(ezterm);
      selection->end.row = selection->mark.row = selection->beg.row;
      selection->end.col = selection->mark.col = selection->beg.col;
      /*drop */
    case SELECTION_BEGIN:
      selection->op = SELECTION_DONE;
      /*drop */
    default:
      return;
      break;
    }
  selection->op = SELECTION_DONE;
  
  /* Set start/end row/col to point to the selection endpoints */
  if(selection->end.row < selection->mark.row ||
     (selection->end.row == selection->mark.row &&
      selection->end.col <= selection->mark.col))
    {
      startr = selection->end.row;
      endr = selection->mark.row;
      startc = selection->end.col;
      endc = selection->mark.col;
    }
  else
    {
      startr = selection->mark.row;
      endr = selection->end.row;
      startc = selection->mark.col;
      endc = selection->end.col;
    }

  if ((startr < -nscrolled || endr >= nrow))
    {
      selection_reset(ezterm);
      return;
    }
  
  /* copy to kill buffer */
  klen = (endr - startr + 1) * (ncol + 1)+1;
  EZ_GrowITextKillBuffer(&TextWidgetKillBuffer,klen+8);  
  sstr = str = TextWidgetKillBuffer.data;

  /* save all points between start and end with selection flag */
  roffset = ((startr + saveLines) * (ncol + 1));
  for(r = startr; r <= endr; r++, roffset += (ncol + 1))
    {
      int c;
      int c1 = (r == startr ? startc : 0);
      int c2 = (r == endr ? endc : (ncol - 1));

      for (c = c1; c <= c2; c++)
	*str++ = screen->text[roffset + c];
      /*
       * end-of-line and not autowrap
       * remove trailing space, but don't remove an entire line!
       */
      if(c2 == (ncol - 1) && !screen->text[roffset + (ncol)])
	{
	  str--;
	  for(c = c2; c >= c1 && isspace(*str); c--)
	    str--;
	  str++;
	  *str++ = '\n';
	}
    }
  *str = '\0';
  klen = (str - sstr);
  klen = (klen > 0 ? klen : 0);
  TextWidgetKillBuffer.length = klen;
  TextWidgetKillBuffer.width = 0;

  if(klen <= 0) return;
  EZ_SetSelectionOwner(ezterm);
}

/* paste selection */
static void PasteIt(ezterm, data, nitems)
     EZ_Widget *ezterm; unsigned char *data; unsigned int nitems;
{
  unsigned char *p = data, *pmax = data + nitems;

  for(nitems = 0; p < pmax; p++)
    {
      /* do newline -> carriage-return mapping */
      if(*p == '\n')
	{
	  unsigned char cr = '\r';
	  tt_write(ezterm, data, nitems);
	  tt_write(ezterm, &cr, 1);
	  data += (nitems + 1);
	  nitems = 0;
	}
      else
	nitems++;
    }
  if(nitems)
    tt_write(ezterm, data, nitems);
}

/*
 * Respond to a notification that a primary selection has been sent
 */
void selection_paste(ezterm, win, prop, Delete)
   EZ_Widget *ezterm; Window win; unsigned prop; int Delete;
{
  long nread, bytes_after;
  if(prop == None) return;

  nread = 0;
  do
    {
      unsigned char *data;
      Atom actual_type;
      int actual_fmt;
      long nitems;

      if ((XGetWindowProperty (EZ_Display, win, prop,
			       nread / 4, 1024, Delete,
			       AnyPropertyType, &actual_type, &actual_fmt,
			       &nitems, &bytes_after,
			       &data) != Success) ||
	  (actual_type != XA_STRING))
	{
	  XFree(data);
	  return;
	}

      nread += nitems;
      PasteIt(ezterm, data, nitems);
      XFree (data);
    }
  while(bytes_after > 0);
}

/* Request the current primary selection */
void selection_request(ezterm, tm, x, y)
     EZ_Widget *ezterm; Time tm; int x; int y;
{
  Window    owner;
  EZ_Widget *tmp;

  owner = XGetSelectionOwner(EZ_Display, XA_PRIMARY);
  tmp = EZ_LookupWidgetFromMappedHT(owner);

  if(tmp != (EZ_Widget *)NULL) /* owner is an internal widget */
    {
      PasteIt(ezterm, TextWidgetKillBuffer.data,   TextWidgetKillBuffer.length);
    }
  else if(owner != None)
    {
      Atom property = XInternAtom(EZ_DisplayForWidgets,"VT_SELECTION",False);
      XConvertSelection(EZ_Display,
			XA_PRIMARY,   /* the selection */
			XA_STRING,    /* the target    */
			property,     /* property      */
			EZ_WidgetWindow(ezterm),  /* the requestor */
			CurrentTime);
    }
  else
    {
      /* no primary selection - use CUT_BUFFER0 */
      selection_paste(ezterm, DefaultRootWindow(EZ_Display), XA_CUT_BUFFER0, False);
    }
}


/* Clear the current selection */
void selection_reset(ezterm) EZ_Widget *ezterm;
{
  selection_t *selection = EZ_ITermSelection(ezterm);
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);
  int    x;

  selection->op = SELECTION_CLEAR;
  selection->end.row = selection->mark.row = 0;
  selection->end.col = selection->mark.col = 0;

  nrow += saveLines;
  for(x = 0; x < nrow * (ncol + 1); x++)
    screen->rend[x] &= ~(RS_Select);
}

void selection_clear(ezterm) EZ_Widget *ezterm;
{
  selection_reset(ezterm);
}


/* mark selected points (used by selection_extend) */
static void selection_setclr(ezterm, set, startr, startc, endr, endc)
     EZ_Widget *ezterm; int set, startr, startc, endr,  endc;
{
  int    nrow = EZ_ITermNRow(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);
  int nscrolled = EZ_ITermNScrolled(ezterm);
  screen_t *screen = EZ_ITermScreen(ezterm);

  
  int r, roffset = ((startr + saveLines) * (ncol + 1));

  /* startr <= endr */
  if((startr < -nscrolled) || (endr >= nrow))
    {
      selection_reset(ezterm);
      return;
    }

  for(r = startr; r <= endr; r++)
    {
      int c1 = (r == startr ? startc : 0);
      int c2 = (r == endr ? endc : (ncol - 1));

      for(  ; c1 <= c2; c1++)
	{
	  if (set)
	    screen->rend[roffset + c1] |= RS_Select;
	  else
	    screen->rend[roffset + c1] &= ~(RS_Select);
	}
      roffset += (ncol + 1);
    }
}

/* start a selection at the specified col/row */
static void selection_start_colrow(ezterm, col, row)
     EZ_Widget *ezterm; int col; int row;
{
  int nscrolled = EZ_ITermNScrolled(ezterm);
  selection_t *selection = EZ_ITermSelection(ezterm);

  if(selection->op)
    {
      /* startr <= endr */
      if ((selection->end.row < -nscrolled) ||
	  (selection->mark.row < -nscrolled))
	{
	  selection_reset(ezterm);
	}
      else
	/* direction of new selection */
	{
	  if (selection->end.row < selection->mark.row ||
	      (selection->end.row == selection->mark.row &&
	       selection->end.col <= selection->mark.col))
	    selection_setclr(ezterm, 0,	/* up */
                             selection->end.row, selection->end.col,
                             selection->mark.row, selection->mark.col);
	  else
	    selection_setclr(ezterm,0,	/* down */
                             selection->mark.row, selection->mark.col,
                             selection->end.row, selection->end.col);
	}
    }
  selection->op = SELECTION_INIT;

  selection->beg.col = col;
  selection->beg.row = row;
  selection->beg.row -= EZ_ITermViewStart(ezterm);
}

/*
 * start a selection at the specified x/y pixel location
 */
void selection_start(ezterm, x, y)
     EZ_Widget *ezterm; int x; int y;
{
  selection_start_colrow(ezterm, Pixel2Col(ezterm, x), Pixel2Row(ezterm, y));
}

/* extend the selection to the specified col/row */
static void selection_extend_colrow(ezterm, col, row)
     EZ_Widget *ezterm; int col; int row;
{
  int old_row, old_col, old_dirn, dirn;
  selection_t *selection = EZ_ITermSelection(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int nscrolled = EZ_ITermNScrolled(ezterm);

  switch (selection->op)
    {
    case SELECTION_INIT:
      selection_reset(ezterm);
      selection->end.col = selection->mark.col = selection->beg.col;
      selection->end.row = selection->mark.row = selection->beg.row;
      /*drop */
    case SELECTION_BEGIN:
      selection->op = SELECTION_BEGIN;
      break;

    case SELECTION_DONE:
    case SELECTION_CONT:
      selection->op = SELECTION_CONT;
      break;

    case SELECTION_CLEAR:
      selection_start_colrow(ezterm, col, row);
      /*drop */
    default:
      return;
      break;
    }

  /* Remember old selection for virtual removal */
  old_row = selection->end.row;
  old_col = selection->end.col;

  if ((old_row < -nscrolled) ||
      (selection->mark.row < -nscrolled))
    {
      selection_reset(ezterm);
      return;
    }

  /* Figure out where new selection is */
  selection->end.col = col;
  selection->end.row = row;

  if(selection->end.col < 0)
    selection->end.col = 0;
  else if(selection->end.col >= ncol)
    selection->end.col = (ncol - 1);

  selection->end.row -= EZ_ITermViewStart(ezterm);
  if (selection->end.row < -nscrolled)
    {
      selection_reset(ezterm);
      return;
    }
  else if(selection->end.row >= nrow)
    selection->end.row = (nrow - 1);

  if((selection->op == SELECTION_BEGIN) &&
     ((selection->end.col != selection->mark.col) ||
      (selection->end.row != selection->mark.row)))
    selection->op = SELECTION_CONT;

  /* If new selection is same as old selection just return
   * or if no highlighting was requested
   */
  if (selection->end.row == old_row && selection->end.col == old_col)
    return;

  /* virtual removal -- delete old highlighting and replace with new */

  /* determine direction of old selection */
  old_dirn = ((old_row < selection->mark.row ||
	       (old_row == selection->mark.row &&
		old_col <= selection->mark.col)) ? UP : DN);

  /* determine direction of new selection */
  dirn = ((selection->end.row < selection->mark.row ||
	   (selection->end.row == selection->mark.row &&
	    selection->end.col <= selection->mark.col)) ? UP : DN);

  /* If old and new direction are different, clear old, set new */
  if(dirn != old_dirn)
    {
      if (old_dirn == UP)
	{
	  selection_setclr(ezterm, 0,
			    old_row, old_col,
			    selection->mark.row, selection->mark.col);
	  selection_setclr(ezterm, 1,
			    selection->mark.row, selection->mark.col,
			    selection->end.row, selection->end.col);
	}
      else
	{
	  selection_setclr(ezterm, 0,
			    selection->mark.row, selection->mark.col,
			    old_row, old_col);
	  selection_setclr (ezterm, 1,
			    selection->end.row, selection->end.col,
			    selection->mark.row, selection->mark.col);
	}
    }
  else
    {
      if (old_dirn == UP)
	{
	  if (old_row < selection->end.row ||
	      (old_row == selection->end.row &&
	       old_col < selection->end.col))
	    {
	      selection_setclr(ezterm, 0,
				old_row, old_col,
				selection->end.row, selection->end.col);
	      selection_setclr(ezterm, 1,
				selection->end.row, selection->end.col,
				selection->end.row, selection->end.col);
	    }
	  else
	    {
	      selection_setclr(ezterm, 1,
				selection->end.row, selection->end.col,
				old_row, old_col);
	    }
	}
      else
	{
	  if(selection->end.row < old_row ||
	      (selection->end.row == old_row &&
	       selection->end.col < old_col))
	    {
	      selection_setclr(ezterm, 0,
				selection->end.row, selection->end.col,
				old_row, old_col);
	      selection_setclr(ezterm, 1,
				selection->end.row, selection->end.col,
				selection->end.row, selection->end.col);
	    }
	  else
	    {
	      selection_setclr(ezterm,1,
				old_row, old_col,
                               selection->end.row, selection->end.col);
	    }
	}
    }
}

/*
 * extend the selection to the specified x/y pixel location
 */
void selection_extend(ezterm, x, y)
     EZ_Widget *ezterm; int x; int y;
{
  selection_extend_colrow(ezterm, Pixel2Col(ezterm, x), Pixel2Row(ezterm, y));
}


/* double/triple click selection */
/*
 * by Edward. Der-Hua Liu, Taiwan
 * cut char support added by A. Haritsis <ah@doc.ic.ac.uk>
 */
void selection_click(ezterm, clicks, x, y)
     EZ_Widget *ezterm; int clicks; int x; int y;
{
  if(clicks <= 1)
    {
      selection_start(ezterm, x, y);	/* single click */
    }
  else
    {
      screen_t *screen = EZ_ITermScreen(ezterm);
      int    nrow = EZ_ITermNRow(ezterm);
      int    ncol = EZ_ITermNCol(ezterm);
      int saveLines = EZ_ITermSaveLines(ezterm);
      int view_start = EZ_ITermViewStart(ezterm);
      int nscrolled = EZ_ITermNScrolled(ezterm);
      int beg_c, end_c, beg_r, end_r;
      text_t *text;

      /* ensure rows/columns are on the screen */
      x = Pixel2Col(ezterm, x);
      x = (x <= 0 ? 0 : (x >= ncol ? (ncol - 1) : x));
      beg_c = end_c = x;

      y = Pixel2Row(ezterm, y);
      y = (y <= 0 ? 0 : (y >= nrow ? (nrow - 1) : y));
      beg_r = end_r = y;

      switch (clicks)
	{
	case 3:
	  /*
	   * triple click
	   */
	  beg_c = 0;
	  end_c = (ncol - 1);
	  break;

	case 2:
	  /*
	   * double click: handle autowrapped lines
	   */
	  for(text = (screen->text +
		       (beg_r + saveLines - view_start) *
		       (ncol + 1));
              /*forever */ ;
              beg_r--, text -= (ncol + 1))
	    {
	      while(beg_c > 0 &&
                    !strchr(EZ_rs_cutchars, text[beg_c - 1]))
		beg_c--;

	      if(beg_c == 0 &&
                 beg_r > (view_start - nscrolled) &&
                 *(text - 1) == '\n' &&
                 !strchr (EZ_rs_cutchars, *(text - 2)))
		beg_c = (ncol - 1);
	      else
		break;
	    }
          
	  for(text = (screen->text +
		       (end_r + saveLines - view_start) *
                      (ncol + 1));
              /*forever */ ;
              end_r++, text += (ncol + 1))
	    {
	      while (end_c < (ncol - 1) &&
		     !strchr (EZ_rs_cutchars, text[end_c + 1]))
		end_c++;

	      if (end_c == (ncol - 1) &&
		  end_r < (view_start + nrow - 1) &&
		  text[ncol] == '\n' &&
		  !strchr (EZ_rs_cutchars, text[ncol + 1]))
		end_c = 0;
	      else
		break;
	    }
	  break;

	default:
	  return;
	  break;
	}
      selection_start_colrow(ezterm, beg_c, beg_r);
      selection_extend_colrow(ezterm, end_c, end_r);
    }
}
/*****************************************************************************************/
/*
 * refresh the region defined by rows STARTR and ENDR, inclusively.
 *
 * Actually draws to the X window
 * For X related speed-ups, this is a good place to fiddle.
 * The arrays drawn_text and drawn_rend contain what I
 * believe is currently shown on the screen. The arrays in screen contain
 * what should be displayed. This routine can decide how to refresh the
 * screen. Calls in command.c decide when to refresh.
 */
void scr_refresh(ezterm, type) EZ_Widget *ezterm; int type;
{
  screen_t *screen = EZ_ITermScreen(ezterm);
  int last_xcursor = EZ_ITermLastXCursor(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);
  int view_start = EZ_ITermViewStart(ezterm);
  rend_t *drawn_rend = screen->drawn_rend;
  text_t *drawn_text = screen->drawn_text;
  Pixmap pixmap;
  GC     gc; /* bg gc */
  unsigned long bgpv;
  int r, roffset, doffset, xcursor, tox, toy, tile;
  int xoff, yoff, w, h, ascent, shadow, shadowx, shadowy, fixedFont;

  if(type == NO_REFRESH)	/* Window not visible, don't update */
    return;

  fixedFont = EZ_ITermFixedFont(ezterm);
  shadow = EZ_WidgetLabelShadow(ezterm);
  if(shadow &0xf0)
    {
      shadowx = (shadow & 0xf0) >> 4;
      shadowy = (shadow & 0x0f);
    }
  else shadowx = shadowy = shadow;
  if(shadowx & 8) shadowx= (8-shadowx);
  if(shadowy & 8) shadowy= (8-shadowy);      
  
  xoff = EZ_ITermXOff(ezterm);
  yoff = EZ_ITermYOff(ezterm);
  if(last_xcursor < (nrow * ncol + 1))
    {
      /* make sure to update it */
      drawn_rend[last_xcursor] = RS_attrMask;
    }
  xcursor = ((screen->row + saveLines) * (ncol + 1) + screen->col);
  last_xcursor = (screen->row + view_start);
  if(last_xcursor >= nrow) 
    last_xcursor = 0;
  else
    {
      last_xcursor *= (ncol + 1);
      if(screen->flags & Screen_VisibleCursor)
	{
	  screen->rend[xcursor] |= RS_Cursor;
	}
    }
  last_xcursor += screen->col;
  EZ_ITermLastXCursor(ezterm) = last_xcursor;
  
  tile = 0;
  tox = toy = 0;
  if(EZ_ITermTextBG(ezterm)  != ~0)
    {
      gc = EZ_WRITABLEGC;
      bgpv = EZ_ITermTextBG(ezterm);
      XSetForeground(EZ_Display, gc, bgpv); 
    }
  else 
    {
      tile = EZ_GetBackgroundGC(ezterm, &gc, &bgpv,0, 0);
      if(tile) EZ_GetWidgetTileOrigin(ezterm, &tox, &toy);
    }
  screen->pixColors[0] =EZ_WidgetForeground(ezterm); /* current fg color */
  screen->pixColors[1] = bgpv;                       /* current bg color */
  
  /* use a cached pixmap to avoid create/free pixmaps */
  if(EZ_ITermLPixmap(ezterm) == None || 
     EZ_ITermLPixmapWidth(ezterm) < EZ_WidgetWidth(ezterm) ||
     EZ_ITermLPixmapHeight(ezterm) < EZ_ITermFHeight(ezterm))
    {
      if( EZ_ITermLPixmap(ezterm) != None) XFreePixmap(EZ_Display, EZ_ITermLPixmap(ezterm));
      EZ_ITermLPixmap(ezterm) 
        = XCreatePixmap(EZ_Display, EZ_WidgetWindow(ezterm), EZ_WidgetWidth(ezterm),
                        EZ_ITermFHeight(ezterm), EZ_Depth);  
      EZ_ITermLPixmapWidth(ezterm) =  EZ_WidgetWidth(ezterm);
      EZ_ITermLPixmapHeight(ezterm) = EZ_ITermFHeight(ezterm);
    }
  pixmap = EZ_ITermLPixmap(ezterm);

  /* For a first cut, do it one character at a time */
  doffset = 0;
  roffset = ((saveLines - view_start) * (ncol + 1));  
  XSetFont(EZ_Display, EZ_WRITABLEGC2, EZ_WidgetFont(ezterm)->fid);
  if(shadow) XSetFont(EZ_Display, EZ_DARKGC2, EZ_WidgetFont(ezterm)->fid);
  w = 0;
  h = EZ_ITermFHeight(ezterm);
  ascent = EZ_ITermFAscent(ezterm);
  for(r = 0; r < nrow;
      roffset += (ncol + 1), doffset += (ncol + 1), r++)
    {
      int c;
      int ypixel = ascent + Row2Pixel(ezterm,r);
      char *linebuf = screen->linebuf;

      for(c = 0; c < ncol; c++)
	{
	  int count;
	  int x = roffset + c;
	  int x1 = doffset + c;

	  if((drawn_text[x1] != screen->text[x]) ||
             (drawn_rend[x1] != screen->rend[x]))
	    {
	      int fore, back, rend;
	      XGCValues gcvalue;	/* GC values */
	      unsigned long gcmask = GCForeground|GCBackground;	/* GC mask */
	      int outlineCursor = False;	/* block cursor */
	      int xpixel = Col2Pixel(ezterm, c);

	      drawn_text[x1] = screen->text[x];
	      drawn_rend[x1] = screen->rend[x];
	      linebuf[0] = screen->text[x];
	      rend = screen->rend[x];

	      x++;
	      c++;
	      for(count = 1;
                  (c < ncol &&
                   (rend == screen->rend[x]) &&
                   (drawn_text[doffset + c] != screen->text[x] ||
                    drawn_rend[doffset + c] != screen->rend[x] ||
                    (c + 1 < ncol && drawn_text[doffset + c + 1] != screen->text[x + 1])));
                  count++, x++, c++)
		{
		  drawn_text[doffset + c] = screen->text[x];
		  drawn_rend[doffset + c] = screen->rend[x];
		  linebuf[count] = screen->text[x];
		}
	      c--;
	      linebuf[count] = '\0';	/* zero-terminate */
	      fore = GET_FGCOLOR(rend);
	      back = GET_BGCOLOR(rend);
	      rend = GET_ATTR(rend);

	      if(rend)
		{
		  int rvid = 0;

		  if((rend & (RS_RVid | RS_Select)) == (RS_RVid | RS_Select))
		    rend &= ~(RS_RVid | RS_Select);
		  else if(rend & (RS_RVid | RS_Select))
		    rvid = 1;

		  if(rend & RS_Cursor)
		    {
		      if(EZ_ITermFocus(ezterm)==0)
			{
			  outlineCursor = True;
			  rend &= ~(RS_Cursor);
			}
		      else
			rvid = (!rvid);
		    }

		  /* swap foreground/background colors */
		  if(rvid)
		    {
		      int tmp = back;
		      back = fore;
		      fore = tmp;
		    }

		  /*
		   * do some font character switching
		   */
		  switch(rend & RS_fontMask)
		    {
		    case RS_acsFont:
		      for(x = 0; x < count; x++)
			if(linebuf[x] >= 0x5F && linebuf[x] < 0x7F)
			  linebuf[x] = (linebuf[x] == 0x5F ?
					0x7F : linebuf[x] - 0x5F);
		      break;
		    case RS_ukFont:
		      for(x = 0; x < count; x++)
			if(linebuf[x] == '#')
			  linebuf[x] = '\036';
		      break;
		    }
		}
	      /* bold characters - order of preference:
	       * 1 - change the foreground color to colorBD
	       * 2 - change the foreground color to bright
	       * 3 - use boldFont
	       * 4 - simulate with overstrike
	       */
	      gcvalue.foreground = screen->pixColors[fore];
	      gcvalue.background = screen->pixColors[back];
              
              if(fore != itfgColor)
                {
                  /* empty */
                }
              else
                {
                  if(rend & RS_Bold)	/* do bold first */
                    {
                      gcvalue.foreground = screen->pixColors[colorBD];
                      if(gcvalue.foreground != screen->pixColors[itfgColor])
                        { rend &= ~RS_Bold;}
                    }
                  else if(rend & RS_Uline)
                    {
                      gcvalue.foreground = screen->pixColors[colorUL];
                      if(gcvalue.foreground != screen->pixColors[itfgColor])
                        { rend &= ~RS_Uline; }
                    }
                }
              {
                unsigned long cclr = EZ_ITermCursorColor(ezterm);
                if(cclr && ( (rend & RS_Cursor) || outlineCursor))
                  {
                    gcvalue.background = cclr;
                    gcvalue.foreground = cclr;
                  }
              }
              XChangeGC(EZ_Display, EZ_WRITABLEGC2, gcmask, &gcvalue);

              /*
               * how to write the strings
               */
              w = count * EZ_ITermFWidth(ezterm);
              if(tile) XSetTSOrigin(EZ_Display, gc, tox, toy - (ypixel - ascent));

              XFillRectangle(EZ_Display, pixmap, gc, xpixel,0, w, h); 	      

              if(fixedFont)
                {
                  if(shadow)
                    XDrawString(EZ_Display, pixmap, EZ_DARKGC2,
                                xpixel+shadowx, ascent+shadowy,
                                linebuf, count);
              
                  if(back == itbgColor)
                    XDrawString(EZ_Display, pixmap, EZ_WRITABLEGC2,
                                xpixel, ascent,
                                linebuf, count);

                  else 
                    XDrawImageString(EZ_Display, pixmap, EZ_WRITABLEGC2,
                                     xpixel, ascent,
                                     linebuf, count);
                }
              else
                {
                  int fwidth = EZ_ITermFWidth(ezterm);
                  int i, x0=xpixel;
                  char *cptr = linebuf;
                  for(i = 0; i < count; i++)
                    {
                      if(shadow)
                        XDrawString(EZ_Display, pixmap, EZ_DARKGC2,
                                    x0+shadowx, ascent+shadowy,
                                    cptr, 1);
              
                      if(back == itbgColor)
                        XDrawString(EZ_Display, pixmap, EZ_WRITABLEGC2,
                                    x0, ascent,
                                    cptr, 1);

                      else 
                        XDrawImageString(EZ_Display, pixmap, EZ_WRITABLEGC2,
                                         x0, ascent,
                                         cptr, 1);
                      cptr++;
                      x0 += fwidth;
                    }
                }

              /*
               * On the smallest font, underline overwrites next row
               */
	      if((rend & RS_Uline))
		{
                  int hh = ascent+2;
		  XDrawLine(EZ_Display,
                            pixmap,
                            EZ_WRITABLEGC2,
                            xpixel, 
                            hh,
                            xpixel + Width2Pixel(ezterm,count) - 1,
                            hh);
		}
              
	      if(outlineCursor)
                XDrawRectangle(EZ_Display,
                               pixmap,
                               EZ_WRITABLEGC2,                               
                               xpixel,
                               0,
                               Width2Pixel(ezterm,1),
                               h);

              if(w > 0)
                XCopyArea(EZ_Display, pixmap, EZ_WidgetWindow(ezterm), EZ_WRITABLEGC2,
                          xpixel, 0,
                          w, h,
                          xpixel, (ypixel-ascent));
	    }
	}
    }
  if(screen->flags & Screen_VisibleCursor)
    {
      screen->rend[xcursor] &= ~(RS_Cursor);
    }
  if(EZ_ITermVScroll(ezterm) != NULL)
    EZ_update_iterm_scrollbars(ezterm);

  XFlush(EZ_Display);
}
/*****************************************************************************/
void EZ_ITermBTimerCb(timer, pdata) EZ_Timer *timer; void *pdata;
{
  EZ_Widget *ezterm = (EZ_Widget *)pdata;
  screen_t *screen = EZ_ITermScreen(ezterm);
  int    nrow = EZ_ITermNRow(ezterm);
  int    ncol = EZ_ITermNCol(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);
  int view_start = EZ_ITermViewStart(ezterm);
  int r, roffset, modified =0;
  
  roffset = ((saveLines - view_start) * (ncol + 1));  
  for(r = 0; r < nrow;  roffset += (ncol + 1), r++)
    {
      int k = roffset + ncol;
      if(screen->rend[k])
        {
          int c, ok=0;
          for(c = 0; c < ncol; c++)
            {

              int x = roffset + c;
              rend_t rstyle = screen->rend[x];
              if(rstyle & RS_Blink)
                {
                  int fclr = GET_FGCOLOR(rstyle);
                  int bclr = GET_BGCOLOR(rstyle);
                  rstyle = SET_FGCOLOR(rstyle, (fclr ^ bclr));  
                  screen->rend[x] = rstyle;
                  modified = 1;
                  ok++;
                }
            }
          if(ok==0) screen->rend[k] = 0;
        }
    }
  if(modified) scr_refresh(ezterm, SLOW_REFRESH);
}
/*****************************************************************************/
