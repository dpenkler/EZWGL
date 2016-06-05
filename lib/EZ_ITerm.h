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
 *  April 1997.  Release Version 1.2
 *  November 1997.  Release Version 1.3
 *  November 1998.  Release Version 1.4
 *  December 1999.  Release Version 1.50
 */
/*--------------------------------*-C-*---------------------------------*
 * File:	command.h
 *
 * Copyright 1992 John Bovey, University of Kent at Canterbury.
 *
 * You can do what you like with this source code as long as you don't try
 * to make money out of it and you include an unaltered copy of this
 * message (including the copyright).
 *
 * This module has been heavily modified by R. Nation
 * <nation@rocket.sanders.lockheed.com>
 * No additional restrictions are applied.
 *
 * Additional modifications by mj olesen <olesen@me.QueensU.CA>
 * No additional restrictions are applied.
 *
 * As usual, the author accepts no responsibility for anything, nor does
 * he guarantee anything whatsoever.
 *----------------------------------------------------------------------*/

/*****************************************************************
 *
 *  Macros and structured used in terminal emulator
 */

/* max size ?? */
#define MAX_COLS	200
#define MAX_ROWS	128
/* convert pixel dimensions to row/column values */
#define Pixel2Width(ezt,  x)   ((x) / EZ_ITermFWidth(ezt))
#define Pixel2Height(ezt, y)   ((y) / EZ_ITermFHeight(ezt))
#define Pixel2Col(ezt, x)      (Pixel2Width(ezt,(x) - (EZ_ITermXOff(ezt))))
#define Pixel2Row(ezt, x)      (Pixel2Height(ezt,(x)- (EZ_ITermYOff(ezt))))

#define Width2Pixel(ezt, n)	((n) * EZ_ITermFWidth(ezt))
#define Height2Pixel(ezt,n)	((n) * EZ_ITermFHeight(ezt))
#define Col2Pixel(ezt,col)	(Width2Pixel(ezt,col)  + EZ_ITermXOff(ezt))
#define Row2Pixel(ezt,row)	(Height2Pixel(ezt,row) + EZ_ITermYOff(ezt))

#define TermWin_TotalWidth(ezt)   (EZ_WidgetWidth(ezt))
#define TermWin_TotalHeight(ezt)  (EZ_WidgetHeight(ezt))

/* screen options */
#define	Opt_console	(1LU<<0)
#define Opt_loginShell	(1LU<<1)
#define Opt_iconic	(1LU<<2)
#define Opt_visualBell	(1LU<<3)
#define Opt_mapAlert	(1LU<<4)
#define Opt_reverseVideo (1LU<<5)
#define Opt_utmpInhibit	(1LU<<6)
#define Opt_scrollBar	(1LU<<7)
#define Opt_meta8	(1LU<<8)

/*
 * XTerm escape sequences: ESC ] Ps;Pt BEL
 */
#define XTerm_name	0
#define XTerm_iconName	1
#define XTerm_title	2
#define XTerm_font	50
/*
 * xiterm extensions of XTerm escape sequences: ESC ] Ps;Pt BEL
 */
#define XTerm_Pixmap	20	/* new bg pixmap */
#define XTerm_restoreFG	39	/* change default fg color */
#define XTerm_restoreBG	49	/* change default bg color */

#define restoreFG	39	/* restore default fg color */
#define restoreBG	49	/* restore default bg color */

#define itfgColor		0
#define itbgColor		1
/* 0-7: black, red, green, yellow, blue, magenta, cyan, white */
#define minColor	2
#define maxColor	(minColor+8)

#define arbColor        (maxColor)

#define minBright	(maxColor+1)
#define maxBright	(minBright+8)

#define arbHColor       (maxBright)

#define NCOLORS		(arbHColor+1)
#define NCURSOR	        0

#define colorBD	        (NCOLORS + NCURSOR)
#define colorUL     	(colorBD+1)
#define NBOLDULINE	2

#define NRS_COLORS	(NCOLORS + NCURSOR + NBOLDULINE)

/***************************************************************************************
 *
 *  misc control info of terminal emulator
 */
#ifndef BUFSIZ
#define BUFSIZ		1024
#endif

typedef struct terminal_control_ {
  char            *ttydev, tty_name[20];  /* master of the pseudo teletype system */
  char            *ptydev, pty_name[20];  /* slave  of the pseudo teletype system */
  int             cmd_fd;                 /* file descriptor connected to the command */
  int             tty_fd;                 
  int             refresh_count, refreshed, refresh_type;
  char            *args[32];             /* command to run    */
  void            *inputHandle;
  void            (*exitHandle) MY_ANSIARGS((void *, void *data));
  void            *exitHandleData;
  unsigned char   *outbuf, *outbufB, *outbufEnd, *outbufP;
  unsigned char   *cmdbuf_ptr, *cmdbuf_endp, *cmdbuf_base; /* to read  */
} control_t;
/***************************************************************************/
/*
 * how the screen accounting works
 *
 * `text' contains text including the scrollback buffer. Each line is a
 * fixed length [TermWin.ncol+1] with the final character of each:
 *      '\n':   for wrapped lines
 *      `\0':   for non-wrapped lines
 *
 * `rend' contains rendition information (font, bold, color, etc)
 *
 * the layout:
 * Rows [0 .. (saveLines-1)] == scrollback region
 * Rows [saveLines .. saveLines + (nrow-1)] ==
 *      screen region [0 .. (nrow-1)]
 *
 * `row', `tscroll', `bscroll' are bounded by (0, nrow)
 *
 * `col' is bounded by (0, ncol)
 *
 * `saveLines'
 *      is the maximum number of lines to save in the scrollback buffer.
 *      This is a fixed number for any particular xiterm instance and is set
 *      by the option `-sl' or resource `saveLines'
 *
 * `nscrolled'
 *      how many lines have been scrolled (saved)
 *              0 <= nscrolled <= saveLines
 *
 * `view_start'
 *      the offset back into the scrollback buffer for our current view
 *              0 <= view_start <= nscrolled
 *
 * The selection region is defined for [0 .. (nrow-1)], which
 * corresponds to the regular screen and for [-1 .. -(nscrolled)]
 * which corresponds to the scrolled region.
 */

/* Screen refresh methods */
#define NO_REFRESH	2	/* Window not visible at all! */
#define SLOW_REFRESH	1	/* No Bitblt */
#define FAST_REFRESH	0

#define IGNORE	  0
#define SAVE	  's'
#define RESTORE	  'r'

/* flags for scr_gotorc() */
#define C_RELATIVE	1	/* col movement is relative */
#define R_RELATIVE	2	/* row movement is relative */
#define RELATIVE (R_RELATIVE|C_RELATIVE)	/* move row/col relative */

/* modes for scr_insdel_chars(), scr_insdel_lines() */
#define INSERT	-1		/* don't change these values */
#define DELETE	+1
#define ERASE	+2

/* modes for scr_page() */
#define UP	+1		/* don't change these values */
#define DN	-1

/* arguments for scr_change_screen() */
#define PRIMARY		0
#define SECONDARY	1

/* rendition style flags */
#define RS_None		0	/* Normal */
#define RS_Cursor	0x01000000u	/* cursor location */
#define RS_Select	0x02000000u	/* selected text */
#define RS_RVid		0x04000000u	/* reverse video */
#define RS_Uline	0x08000000u	/* underline */
#define RS_acsFont	0x10000000u	/* ACS - graphics character set */
#define RS_ukFont	0x20000000u	/* UK character set */
#define RS_fontMask	(RS_acsFont|RS_ukFont)

#define RS_fgMask	0x00001F00u	/* 32 colors */
#define RS_Bold		0x00008000u	/* bold */
#define RS_bgMask	0x001F0000u	/* 32 colors */
#define RS_Blink	0x00800000u	/* blink */

#define RS_attrMask	(0xFF000000u|RS_Bold|RS_Blink)


/* how to build & extract colors and attributes */
#define GET_FGCOLOR(r)	(((r) & RS_fgMask)>>8)
#define GET_BGCOLOR(r)	(((r) & RS_bgMask)>>16)
#define GET_ATTR(r)	(((r) & RS_attrMask))

#define SET_FGCOLOR(r,fg)	(((r) & ~RS_fgMask)  | ((fg)<<8))
#define SET_BGCOLOR(r,bg)	(((r) & ~RS_bgMask)  | ((bg)<<16))
#define SET_ATTR(r,a)		(((r) & ~RS_attrMask)| (a))
#define DEFAULT_RSTYLE		(RS_None | (itfgColor<<8) | (itbgColor<<16))

/**********************************************************************************/
typedef unsigned char text_t;
typedef unsigned int  rend_t;

typedef struct 
{
  char         charsets[4];
  short        row, col;		/* cursor position                      */
  short        tscroll, bscroll;	/* top/bottom of scroll region          */
  int          charset;		/* character set number [0..3]          */
  rend_t       rstyle;
  unsigned int flags;
  text_t       *linebuf;
  text_t       *text;	        /* all the text, including scrollback   */
  rend_t       *rend;  		/* rendition, using the `RS_' flags     */
  text_t       *drawn_text;     /* This tells what's actually on the screen */
  rend_t       *drawn_rend;
  char         *tabs;
  unsigned long pixColors[23];    
  int          cmasks;
} screen_t;

#define Screen_Relative		(1<<0)	/* relative origin mode flag      */
#define Screen_VisibleCursor	(1<<1)	/* cursor visible?                */
#define Screen_Autowrap		(1<<2)	/* auto-wrap flag                 */
#define Screen_Insert		(1<<3)	/* insert mode (vs. overstrike)   */
#define Screen_WrapNext		(1<<4)	/* need to wrap for next char?    */
#define Screen_DefaultFlags	(Screen_VisibleCursor|Screen_Autowrap)

/**********************************************************************************/
typedef struct
{
  short    row, col;
  short    charset;
  char     charset_char;
  rend_t   rstyle;
} SaveScreen;

/**********************************************************************************
 * 
 * selection record
 */
typedef struct
  {
    short op;			/* current operation */
    short screen;		/* which screen is being used */
    struct
      {
	short row, col;
      }  beg, end, mark;
} selection_t;

#define SELECTION_CLEAR	0
#define SELECTION_BEGIN	1
#define SELECTION_INIT	2
#define SELECTION_CONT	3
#define SELECTION_DONE	4
/***************************************************************************************/
