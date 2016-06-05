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
 * Copyright 1992 John Bovey, University of Kent at Canterbury.
 *
 * You can do what you like with this source code as long as
 * you don't try to make money out of it and you include an
 * unaltered copy of this message (including the copyright).
 *
 * This module has been very heavily modified by R. Nation
 * <nation@rocket.sanders.lockheed.com>
 * No additional restrictions are applied
 *
 * Additional modification by Garrett D'Amore <garrett@netcom.com> to
 * allow vt100 printing.  No additional restrictions are applied.
 *
 * Integrated modifications by Steven Hirsch <hirsch@emba.uvm.edu> to
 * properly support X11 mouse report mode and support for DEC
 * "private mode" save/restore functions.
 *
 * Integrated key-related changes by Jakub Jelinek <jj@gnu.ai.mit.edu>
 * to handle Shift+function keys properly.
 * Should be used with enclosed termcap / terminfo database.
 *
 * Extensive modifications by mj olesen <olesen@me.QueensU.CA>
 * No additional restrictions.
 *
 * Further modification and cleanups for Solaris 2.x and Linux 1.2.x
 * by Raul Garcia Garcia <rgg@tid.es>. No additional restrictions.
 *
 * As usual, the author accepts no responsibility for anything, nor does
 * he guarantee anything whatsoever.
 *----------------------------------------------------------------------*/
/*****************************************************************
 ***                                                           ***
 ***       Terminal emulator control functions                 ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_ITERM_COMMAND_C_
/***************************************************************************/

#include "EZ_Widget.h"

/* Consistent defines - please report on the necessity
 * @ Unixware: defines (__svr4__)
 */
#if defined (SVR4) && !defined (__svr4__)
#define __svr4__
#endif
#if defined (sun) && !defined (__sun__)
#define __sun__
#endif

/*
 * sun <sys/ioctl.h> isn't properly protected?
 * anyway, it causes problems when <termios.h> is also included
 */
#if defined (__sun__)
#undef HAVE_SYS_IOCTL_H
#endif

#ifdef USE_GETGRNAME
# include <grp.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#if defined (__svr4__)
# include <sys/resource.h>	/* for struct rlimit */
# include <sys/stropts.h>	/* for I_PUSH */

# ifdef HAVE_SYS_STRTIO_H
#  include <sys/strtio.h> 
# endif

# ifdef HAVE_BSDTTY_H
#  include <bsdtty.h>
# endif

# define _NEW_TTY_CTRL		/* to get proper defines in <termios.h> */
#endif

/*
 * Solaris defines SRIOCSREDIR in sys/strredir.h .
 * Needed for displaying console messages under solaris
 */
#if defined(__sun) && defined(__SVR4)
# include <sys/strredir.h>
#endif

/* For N_TTY_BUF_SIZE. */
#if defined(linux)
# include <linux/tty.h>
#endif
/***************************************************************************/
/* ways to deal with getting/setting termios structure */
#ifdef HAVE_TERMIOS_H
typedef struct termios ttymode_t;
#ifdef TCSANOW			/* POSIX */
#define GET_TERMIOS(fd,tios)	tcgetattr (fd, tios)
#define SET_TERMIOS(fd,tios)	do {\
  cfsetospeed (tios, BAUDRATE);\
  cfsetispeed (tios, BAUDRATE);\
  tcsetattr(fd, TCSANOW, tios);\
} while (0)
#else
#ifdef TIOCSETA
#define GET_TERMIOS(fd,tios)	ioctl (fd, TIOCGETA, tios)
#define SET_TERMIOS(fd,tios)	do {\
  tios->c_cflag |= BAUDRATE;\
  ioctl (fd, TIOCSETA, tios);\
} while (0)
#else
#define GET_TERMIOS(fd,tios)	ioctl (fd, TCGETS, tios)
#define SET_TERMIOS(fd,tios)	do {\
  tios->c_cflag |= BAUDRATE;\
  ioctl (fd, TCSETS, tios);\
} while (0)
#endif
#endif
#define SET_TTYMODE(fd,tios)		SET_TERMIOS (fd, tios)
#else
/* sgtty interface */
typedef struct 
{
  struct sgttyb sg;
  struct tchars tc;
  struct ltchars lc;
  int line;
  int local;
} ttymode_t;

#define SET_TTYMODE(fd,tt)	do {	\
    tt->sg.sg_ispeed = tt->sg.sg_ospeed = BAUDRATE;\
    ioctl (fd, TIOCSETP, &(tt->sg));\
    ioctl (fd, TIOCSETC, &(tt->tc));\
    ioctl (fd, TIOCSLTC, &(tt->lc));\
    ioctl (fd, TIOCSETD, &(tt->line));\
    ioctl (fd, TIOCLSET, &(tt->local));\
} while(0)
#endif /* HAVE_TERMIOS_H */
/****************************************************************************
 *
 * From EZ_ITermScrn.c
 */
EXTERN  void EZ_SetITermColors MY_ANSIARGS((EZ_Widget *ezterm, int i, unsigned long pixel_value));
EXTERN  void scr_reset MY_ANSIARGS((EZ_Widget *ezterm));
EXTERN  void scr_poweron MY_ANSIARGS((EZ_Widget *ezterm));
EXTERN  void scr_cursor MY_ANSIARGS((EZ_Widget *ezterm, int mode));
EXTERN  void scr_color MY_ANSIARGS((EZ_Widget *ezterm, unsigned int color, unsigned int Intensity));
EXTERN  void scr_rendition MY_ANSIARGS((EZ_Widget *ezterm, int set, int style));
EXTERN  void scr_add_lines MY_ANSIARGS((EZ_Widget *ezterm, unsigned char *str, int nlines, int cnt));
EXTERN  void scr_backspace MY_ANSIARGS((EZ_Widget *ezterm));
EXTERN  void scr_tab MY_ANSIARGS((EZ_Widget *ezterm, int count));
EXTERN  void scr_gotorc MY_ANSIARGS((EZ_Widget *ezterm, int row, int col, int relative));
EXTERN  void scr_index MY_ANSIARGS((EZ_Widget *ezterm, int dirn));
EXTERN  void scr_erase_line MY_ANSIARGS((EZ_Widget *ezterm, int mode));
EXTERN  void scr_erase_screen MY_ANSIARGS((EZ_Widget *ezterm, int mode));
EXTERN  void scr_E MY_ANSIARGS((EZ_Widget *ezterm));
EXTERN  void scr_insdel_lines MY_ANSIARGS((EZ_Widget *ezterm, int count, int insdel));
EXTERN  void scr_insdel_chars MY_ANSIARGS((EZ_Widget *ezterm, int count, int insdel));
EXTERN  void scr_scroll_region MY_ANSIARGS((EZ_Widget *ezterm,int top, int bot));
EXTERN  void scr_cursor_visible MY_ANSIARGS((EZ_Widget *ezterm, int mode));
EXTERN  void scr_autowrap MY_ANSIARGS((EZ_Widget *ezterm, int mode));
EXTERN  void scr_relative_origin MY_ANSIARGS((EZ_Widget *ezterm, int mode));
EXTERN  void scr_insert_mode MY_ANSIARGS((EZ_Widget *ezterm, int mode));
EXTERN  int scr_move_to MY_ANSIARGS((EZ_Widget *ezterm, int y, int len));
EXTERN  int scr_page MY_ANSIARGS((EZ_Widget *ezterm, int dirn, int nlines));
EXTERN  void scr_report_position MY_ANSIARGS((EZ_Widget *ezterm));
EXTERN  void scr_charset_choose MY_ANSIARGS((EZ_Widget *ezterm, int set));
EXTERN  void scr_charset_set MY_ANSIARGS((EZ_Widget *ezterm, int set, unsigned int ch));
EXTERN  void scr_expose MY_ANSIARGS((EZ_Widget *ezterm, int x, int y, int width, int height));
EXTERN  void scr_touch MY_ANSIARGS((EZ_Widget *ezterm));
EXTERN  void scr_set_tab MY_ANSIARGS((EZ_Widget *ezterm, int mode));
EXTERN  void scr_bell MY_ANSIARGS((EZ_Widget *ezterm));
EXTERN  void mouse_report MY_ANSIARGS((EZ_Widget *ezterm, XButtonEvent * ev));
EXTERN  void selection_make MY_ANSIARGS((EZ_Widget *ezterm, Time tm));
EXTERN  void selection_paste MY_ANSIARGS((EZ_Widget *ezterm, Window win, unsigned prop, int Delete));
EXTERN  void selection_request MY_ANSIARGS((EZ_Widget *ezterm, Time tm, int x, int y));
EXTERN  void selection_reset MY_ANSIARGS((EZ_Widget *ezterm));
EXTERN  void selection_clear MY_ANSIARGS((EZ_Widget *ezterm));
EXTERN  void selection_start MY_ANSIARGS((EZ_Widget *ezterm, int x, int y));
EXTERN  void selection_extend MY_ANSIARGS((EZ_Widget *ezterm, int x, int y));
EXTERN  void selection_click MY_ANSIARGS((EZ_Widget *ezterm, int clicks, int x, int y));
EXTERN  void scr_refresh MY_ANSIARGS((EZ_Widget *ezterm, int type));
EXTERN  void EZ_ITermReverseVideo MY_ANSIARGS((EZ_Widget *ezterm, int mode));
/***************************************************************************/
void EZ_TermStartCommand MY_ANSIARGS((EZ_Widget *ezterm, char **args));
void EZ_TermStopCommand MY_ANSIARGS((EZ_Widget *ezterm, int sig));
void EZ_TermSetExitHandler MY_ANSIARGS((EZ_Widget *ezterm, EZ_CallBack cb,  void *data));

void tt_resize MY_ANSIARGS((EZ_Widget *ezterm));
void tt_write MY_ANSIARGS((EZ_Widget* ezterm, char *buf, int nbytes));
void EZ_ITermTimerCb MY_ANSIARGS((EZ_Timer *timer, void *pdata));
void EZ_ITermEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));

void tt_printf  MY_VARARGS_D(void *, the_args);

void xterm_seq MY_ANSIARGS((EZ_Widget *ezterm, int op, char *str));
void set_width MY_ANSIARGS((EZ_Widget *ezterm, int width));
void EZ_iterm_resize_window MY_ANSIARGS((EZ_Widget *ezterm));
/***************************************************************************/
static void tt_winsize MY_ANSIARGS((EZ_Widget *ezterm, int fd));
static EZ_Widget *findEmulator MY_ANSIARGS((pid_t pid));
static void Child_signal MY_ANSIARGS((int sig));
static int get_pty  MY_ANSIARGS((EZ_Widget *ezterm));
static int get_tty MY_ANSIARGS((EZ_Widget *ezterm));
static void get_ttymode MY_ANSIARGS((EZ_Widget *ezterm, ttymode_t *tio));
static int run_command MY_ANSIARGS((EZ_Widget *ezterm, char *argv[]));
static void tt_winsize MY_ANSIARGS((EZ_Widget *ezterm, int fd));
static void lookup_key MY_ANSIARGS((EZ_Widget *ezterm, XEvent * ev));
static void cmd_fd_reader MY_ANSIARGS((void *input, void *data, int fd, int mask));
static void process_escape_seq  MY_ANSIARGS((EZ_Widget *ezterm));
static void process_csi_seq MY_ANSIARGS((EZ_Widget *ezterm));
static void process_xterm_seq MY_ANSIARGS(( EZ_Widget *ezterm));
static void process_terminal_mode MY_ANSIARGS((EZ_Widget *ezterm, int mode, int priv,
                                               unsigned int nargs, int arg[]));
static void process_sgr_mode MY_ANSIARGS((EZ_Widget *ezterm, unsigned int nargs, int arg[]));
static unsigned char cmd_getc MY_ANSIARGS((EZ_Widget *ezterm));
/***************************************************************************/

/* use the fastest baud-rate */
#ifdef B38400
#define BAUDRATE	B38400
#else
#ifdef B19200
#define BAUDRATE	B19200
#else
#define BAUDRATE	B9600
#endif
#endif

/* Disable special character functions */
#ifndef  VDISABLE
#ifdef _POSIX_VDISABLE
#define VDISABLE	_POSIX_VDISABLE
#else
#define VDISABLE	255
#endif
#endif

/*
 * system default characters if defined and reasonable
 */
#ifndef CINTR
#define CINTR		'\003'	/* ^C */
#endif
#ifndef CQUIT
#define CQUIT		'\034'	/* ^\ */
#endif
#ifndef CERASE
#define CERASE		'\010'	/* ^H */
#endif
#ifndef CKILL
#define CKILL		'\025'	/* ^U */
#endif
#ifndef CEOF
#define CEOF		'\004'	/* ^D */
#endif
#ifndef CSTART
#define CSTART		'\021'	/* ^Q */
#endif
#ifndef CSTOP
#define CSTOP		'\023'	/* ^S */
#endif
#ifndef CSUSP
#define CSUSP		'\032'	/* ^Z */
#endif
#ifndef CDSUSP
#define CDSUSP		'\031'	/* ^Y */
#endif
#ifndef CRPRNT
#define CRPRNT		'\022'	/* ^R */
#endif
#ifndef CFLUSH
#define CFLUSH		'\017'	/* ^O */
#endif
#ifndef CWERASE
#define CWERASE		'\027'	/* ^W */
#endif
#ifndef CLNEXT
#define CLNEXT		'\026'	/* ^V */
#endif

#ifndef VDISCRD
#ifdef VDISCARD
#define VDISCRD	VDISCARD
#endif
#endif

#ifndef VWERSE
#ifdef VWERASE
#define VWERSE	VWERASE
#endif
#endif

#define KBUFSZ		8	/* size of keyboard mapping buffer */
#define STRING_MAX	512	/* max string size for process_xterm_seq() */
#define ESC_ARGS	32	/* max # of args for esc sequences */


/* a large REFRESH_PERIOD causes problems with `cat' */
#define REFRESH_PERIOD	1

#ifndef REFRESH_PERIOD
#define REFRESH_PERIOD	10
#endif

/* time factor to slow down a `jumpy' mouse */
#define MOUSE_THRESHOLD	50
#define CONSOLE		"/dev/console"	/* console device */

/*
 * key-strings: if only these keys were standardized <sigh>
 */
#ifdef LINUX_KEYS
#define KS_HOME	"\033[1~"	/* Home == Find */
#define KS_END	"\033[4~"	/* End == Select */
#else
#define KS_HOME	"\033[7~"	/* Home */
#define KS_END	"\033[8~"	/* End */
#endif

/* and this one too! */
#ifdef NO_DELETE_KEY
#undef KS_DELETE		/* use X server definition */
#else
#ifndef KS_DELETE
#define KS_DELETE	"\033[3~"	/* Delete = Execute */
#endif
#endif

/*
 * ESC-Z processing:
 *
 * By stealing a sequence to which other xterms respond, and sending the
 * same number of characters, but having a distinguishable sequence,
 * we can avoid having a timeout (when not under an xiterm) for every login
 * shell to auto-set its DISPLAY.
 *
 * This particular sequence is even explicitly stated as obsolete since
 * about 1985, so only very old software is likely to be confused, a
 * confusion which can likely be remedied through termcap or TERM. Frankly,
 * I doubt anyone will even notice.  We provide a #ifdef just in case they
 * don't care about auto-display setting.  Just in case the ancient
 * software in question is broken enough to be case insensitive to the 'c'
 * character in the answerback string, we make the distinguishing
 * characteristic be capitalization of that character. The length of the
 * two strings should be the same so that identical read(2) calls may be
 * used.
 */
#define VT100_ANS	"\033[?1;2c"	/* vt100 answerback */
#ifndef ESCZ_ANSWER
#define ESCZ_ANSWER	VT100_ANS	/* obsolete ANSI ESC[c */
#endif


/* DEC private modes */
#define PrivMode_132		(1LU<<0)
#define PrivMode_132OK		(1LU<<1)
#define PrivMode_rVideo		(1LU<<2)
#define PrivMode_relOrigin	(1LU<<3)
#define PrivMode_Screen		(1LU<<4)
#define PrivMode_Autowrap	(1LU<<5)
#define PrivMode_aplCUR		(1LU<<6)
#define PrivMode_aplKP		(1LU<<7)
#define PrivMode_BackSpace	(1LU<<8)
#define PrivMode_ShiftKeys	(1LU<<9)
#define PrivMode_VisibleCursor	(1LU<<10)
#define PrivMode_MouseX10	(1LU<<11)
#define PrivMode_MouseX11	(1LU<<12)

#define PrivMode_mouse_report	(PrivMode_MouseX10|PrivMode_MouseX11)
#define PrivMode(ezterm, test,bit) do {\
  if(test) {EZ_ITermPrivateModes(ezterm) |= (bit);} \
   else {EZ_ITermPrivateModes(ezterm) &= ~(bit);}} while (0)
/****************************************************************************/

void EZ_TermStartCommand(widget, argv)
     EZ_Widget *widget; char *argv[];
{
  EZ_Widget *ezterm = NULL;
  if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)   ezterm = EZ_TermITerm(widget);
  else if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)  ezterm = widget;
  if(ezterm)
    {
      control_t *control = EZ_ITermControl(ezterm);
      int pid = EZ_ITermChildPid(ezterm);
      if(pid < 0 || kill(pid, 0) < 0)
        {
          scr_poweron(ezterm);
          control->cmdbuf_ptr = control->cmdbuf_endp = control->cmdbuf_base;
          control->outbufP = control->outbufB = control->outbuf;
          if((control->cmd_fd = run_command(ezterm, argv)) < 0)
            {
              fprintf(stderr, "couldn't execute %s", argv?argv[0]: "");
            }
        }
      else
        {
          fprintf(stderr, "Error: xterm is already running '%s'\n",control->args[0]);
        }
    }
}
/****************************************************************************/
void EZ_TermStopCommand(widget, sig)
     EZ_Widget *widget; int sig;
{
  EZ_Widget *ezterm = NULL;
  if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)   ezterm = EZ_TermITerm(widget);
  else if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)  ezterm = widget;
  if(ezterm)
    {
      if(EZ_ITermChildPid(ezterm) > 0)
        {
          kill(EZ_ITermChildPid(ezterm), sig);
        }
    }
}
void EZ_TermSetExitHandler(widget, handle, data)
     EZ_Widget *widget; EZ_CallBack handle; void *data;
{
  EZ_Widget *ezterm = NULL;
  if(EZ_WidgetType(widget) == EZ_WIDGET_TERM)   ezterm = EZ_TermITerm(widget);
  else if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM)  ezterm = widget;
  if(ezterm)
    {
      control_t *control = EZ_ITermControl(ezterm);
      control->exitHandle = handle;
      control->exitHandleData = data;
    }
}
/****************************************************************************/
void  EZ_InitITermCommand(wptr) EZ_Widget *wptr;
{
  control_t *control = EZ_ITermControl(wptr) = (control_t *)
    my_malloc(sizeof(control_t), _ITERM_DATA_);
  memset(control, 0, sizeof(control_t));
  control->ptydev = NULL;
  control->ttydev = NULL;
  control->cmd_fd = -1;
  control->tty_fd = -1;
  control->cmdbuf_base = (unsigned char *)my_malloc(8192 * sizeof(char), _ITERM_DATA_);

  EZ_ITermPrivateModes(wptr) = EZ_ITermSaveModes(wptr) =
    (PrivMode_Autowrap|PrivMode_aplKP|PrivMode_ShiftKeys|PrivMode_VisibleCursor);
}
void  EZ_DestroyITermCommand(wptr) EZ_Widget *wptr;
{
  control_t *control = EZ_ITermControl(wptr);
  if(control)
    {
      if(control->inputHandle) EZ_RemoveInput(control->inputHandle);
      if(control->outbuf) free(control->outbuf);
      if(control->cmdbuf_base) my_free(control->cmdbuf_base);
      if(control->cmd_fd != -1) close(control->cmd_fd);
      if(control->tty_fd != -1) close(control->tty_fd); 
      my_free((void *)control); 
    }
  EZ_ITermControl(wptr) = NULL;
}

/* substitute system functions */
#ifndef _POSIX_VERSION
#if defined (__svr4__)
static int getdtablesize()
{
  struct rlimit rlim;
  getrlimit (RLIMIT_NOFILE, &rlim);
  return rlim.rlim_cur;
}
#endif
#endif
/****************************************************************************
 *
 * return the emulator running process with pid PID
 */
static EZ_Widget *findEmulator(pid) pid_t pid;
     
{
  extern EZ_AllWidgetHTEntry EZ_AllWidgetsHashTable[];
  EZ_AllWidgetHTEntry  *tmp;  
  int i;
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_AllWidgetsHashTable[i].next;
      while(tmp)
        {
          EZ_Widget *widget = tmp->widget;
          if(EZ_WidgetType(widget) == EZ_WIDGET_ITERM) 
            {
              if(EZ_ITermChildPid(widget) == pid &&
                 EZ_WidgetIsDestroyed(widget) == 0)
                return(widget);
            }
          tmp = tmp->next;
        }
    }
  return(NULL);
}
/****************************************************************************
 *
 * Catch a SIGCHLD signal and exit if the direct child has died
 */
static RETSIGTYPE Child_signal(sig) int sig;
{
  int status;
  EZ_Widget *emu = NULL;
  
  if(sig == SIGCHLD)
    {
      pid_t childPid = waitpid(-1, &status, 0);
      control_t *control;
      emu = findEmulator(childPid);
      
      if(emu == NULL) {  signal(sig, Child_signal);   return; }
      control = EZ_ITermControl(emu);      
      if(status == 0)
        fprintf(stderr, "'%s' finished\n", control->args[0]);
      else if(WIFSIGNALED(status))
        fprintf(stderr, "'%s' finished with signal %d", control->args[0], WTERMSIG(status));
      else
        fprintf(stderr, "'%s' finished with code %d",  control->args[0], WEXITSTATUS(status));

      EZ_ITermChildPid(emu) = -1;
      /* clean up */
      EZ_RemoveInput(control->inputHandle); control->inputHandle = NULL;
      close(control->cmd_fd); control->cmd_fd = -1;
      close(control->tty_fd); control->tty_fd = -1;
      /* invoke exit callback */
      if(control->exitHandle)
        (control->exitHandle)(emu,control->exitHandleData);
    }
}

/* Acquire a pseudo-teletype from the system. 
 *
 * On failure, returns -1.
 * On success, returns the file descriptor.
 *
 * If successful, ttydev and ptydev point to the names of the
 * master and slave parts
 */
static int get_pty(ezterm) EZ_Widget *ezterm;
{
  control_t *control = EZ_ITermControl(ezterm);
  int pty_fd = -1;

#if defined (__sgi)
  control->ptydev = control->ttydev = _getpty (&pty_fd, O_RDWR | O_NDELAY, 0622, 0);
  if(control->ptydev == NULL)
    goto Failed;
#elif defined (__svr4__)
  {
    extern char *ptsname();

    /* open the STREAMS, clone device /dev/ptmx (master pty) */
    if ((pty_fd = open("/dev/ptmx", O_RDWR)) < 0)
      {
	goto Failed;
      }
    else
      {
	grantpt(pty_fd);		/* change slave permissions */
	unlockpt(pty_fd);		/* unlock slave */
	control->ptydev = control->ttydev = 
          ptsname(pty_fd); /* get slave's name */
      }
  }
#elif defined (_AIX)
  if((pty_fd = open ("/dev/ptc", O_RDWR)) < 0)
    goto Failed;
  else
    control->ptydev = control->ttydev = ttyname(pty_fd);
#else
  int len;
  char *c1, *c2;
  strcpy(control->pty_name,"/dev/pty??");
  strcpy(control->tty_name,"/dev/tty??");
  len = strlen(control->tty_name);
  control->ptydev = control->pty_name;
  control->ttydev = control->tty_name;
#define	PTYCHAR1	"pqrstuvwxyz"
#define	PTYCHAR2	"0123456789abcdef"
  for(c1 = PTYCHAR1; *c1; c1++)
    {
      control->ptydev[len - 2] = control->ttydev[len - 2] = *c1;
      for(c2 = PTYCHAR2; *c2; c2++)
	{
	  control->ptydev[len - 1] = control->ttydev[len - 1] = *c2;
	  if((pty_fd = open(control->ptydev, O_RDWR)) >= 0)
	    {
	      if(access(control->ttydev, R_OK | W_OK) == 0)
		goto Found;
	      close(pty_fd);
	    }
	}
    }
  goto Failed;
#endif

Found:
  fcntl(pty_fd, F_SETFL, O_NDELAY);
  return(pty_fd);

Failed:
  fprintf(stderr, "can't open pseudo-tty\n");
  return(-1);
}

/* establish a controlling teletype for new session 
 *
 * On some systems this can be done with ioctl() but on others we
 * need to re-open the slave tty.
 */
static int get_tty(ezterm) EZ_Widget *ezterm;
{
  control_t *control = EZ_ITermControl(ezterm);
  pid_t pid;

  /*
   * setsid() [or setpgrp] must be before open of the terminal,
   * otherwise there is no controlling terminal (Solaris 2.4, HP-UX 9)
   */
#ifndef ultrix
#ifdef NO_SETSID
  pid = setpgrp(0, 0);
#else
  pid = setsid();
#endif
  if(pid < 0) fprintf(stderr, "setsid failed.\n");
#endif /* ultrix */
  if((control->tty_fd = open(control->ttydev, O_RDWR)) < 0)
    {
      fprintf(stderr, "can't open slave tty %s", control->ttydev);
      exit(1);
    }
#if defined (__svr4__)
  /*
   * Push STREAMS modules:
   *    ptem: pseudo-terminal hardware emulation module.
   *    ldterm: standard terminal line discipline.
   *    ttcompat: V7, 4BSD and XENIX STREAMS compatibility module.
   */
  ioctl(control->tty_fd, I_PUSH, "ptem");
  ioctl(control->tty_fd, I_PUSH, "ldterm");
  ioctl(control->tty_fd, I_PUSH, "ttcompat");
#else /* __svr4__ */
  {
    /* change ownership of tty to real uid and real group */
    unsigned int mode = 0622;
    gid_t gid = getgid();
#ifdef USE_GETGRNAME
    {
      struct group *gr = getgrnam("tty");
      if(gr)
	{
	  /* change ownership of tty to real uid, "tty" gid */
	  gid = gr->gr_gid;
	  mode = 0620;
	}
    }
#endif /* USE_GETGRNAME */
    fchown(control->tty_fd, getuid(), gid);	/* fail silently */
    fchmod(control->tty_fd, mode);
  }
#endif /* __svr4__ */

  /*
   * Close all file descriptors.  If only stdin/out/err are closed,
   * child processes remain alive upon deletion of the window.
   */
  {
    int i, fd = control->tty_fd, nfds;
#ifdef _POSIX_VERSION
    nfds = sysconf(_SC_OPEN_MAX);
#else
    nfds = getdtablesize();
#endif
    for(i = 0; i < nfds; i++) { if(i != fd) close (i); }
    /* Reopen stdin, stdout and stderr over the tty file descriptor */      
    dup(fd); /* stdin:  0 */
    dup(fd); /* stdout: 1 */
    dup(fd); /* stderr: 2 */
    if(fd > 2) close(fd);  
  }

#ifdef ultrix
  if((control->tty_fd = open ("/dev/tty", O_RDONLY)) >= 0)
    {
      ioctl(control->fd, TIOCNOTTY, 0);
      close(control->fd);
    }
  else
    {
      pid = setpgrp(0, 0);
      if(pid < 0) fprntf(stderr, "setpgrp failed\n");
    }
  /* no error, we could run with no tty to begin with */
#else /* ultrix */

#ifdef TIOCSCTTY
  ioctl(0, TIOCSCTTY, 0);
#endif

  /* set process group */
#if defined (_POSIX_VERSION) || defined (__svr4__)
  tcsetpgrp(0, pid);
#elif defined (TIOCSPGRP)
  ioctl(0, TIOCSPGRP, &pid);
#endif

  /* svr4 problems: reports no tty, no job control */
  /* # if !defined (__svr4__) && defined (TIOCSPGRP) */

  close(open(control->ttydev, O_RDWR, 0) );
  /* # endif */
#endif /* ultrix */
  return(control->tty_fd);
}
/**********************************************************/
static void get_ttymode(ezterm, tio)
     EZ_Widget *ezterm; ttymode_t *tio;
{
#ifdef HAVE_TERMIOS_H
  /*
   * standard System V termios interface
   */
  if (GET_TERMIOS(0, tio) < 0)
    {
      /* return error - use system defaults */
      tio->c_cc[VINTR] = CINTR;
      tio->c_cc[VQUIT] = CQUIT;
      tio->c_cc[VERASE] = CERASE;
      tio->c_cc[VKILL] = CKILL;
      tio->c_cc[VSTART] = CSTART;
      tio->c_cc[VSTOP] = CSTOP;
      tio->c_cc[VSUSP] = CSUSP;
#ifdef VDSUSP
      tio->c_cc[VDSUSP] = CDSUSP;
#endif
#ifdef VREPRINT
      tio->c_cc[VREPRINT] = CRPRNT;
#endif
#ifdef VDISCRD
      tio->c_cc[VDISCRD] = CFLUSH;
#endif
#ifdef VWERSE
      tio->c_cc[VWERSE] = CWERASE;
#endif
#ifdef VLNEXT
      tio->c_cc[VLNEXT] = CLNEXT;
#endif
    }

  tio->c_cc[VEOF] = CEOF;
  tio->c_cc[VEOL] = VDISABLE;
#ifdef VEOL2
  tio->c_cc[VEOL2] = VDISABLE;
#endif
#ifdef VSWTC
  tio->c_cc[VSWTC] = VDISABLE;
#endif
#ifdef VSWTCH
  tio->c_cc[VSWTCH] = VDISABLE;
#endif
#if VMIN != VEOF
  tio->c_cc[VMIN] = 1;
#endif
#if VTIME != VEOL
  tio->c_cc[VTIME] = 0;
#endif

  /* input modes */
  tio->c_iflag = (BRKINT | IGNPAR | ICRNL | IXON
#ifdef IMAXBEL
		  | IMAXBEL
#endif
    );

  /* output modes */
  tio->c_oflag = (OPOST | ONLCR);

  /* control modes */
  tio->c_cflag = (CS8 | CREAD);

  /* line discipline modes */
  tio->c_lflag = (ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK
#if defined (ECHOCTL) && defined (ECHOKE)
		  | ECHOCTL | ECHOKE
#endif
    );

  /*
   * guess an appropriate value for Backspace
   */
#ifdef DONT_GUESS_BACKSPACE
  PrivMode(ezterm, 1, PrivMode_BackSpace);	/* always ^H */
#else
  PrivMode(ezterm, (tio->c_cc[VERASE] == '\b'), PrivMode_BackSpace);
#endif /* DONT_GUESS_BACKSPACE */

#else /* HAVE_TERMIOS_H */

  /*
   * sgtty interface
   */

  /* get parameters -- gtty */
  if(ioctl(0, TIOCGETP, &(tio->sg)) < 0)
    {
      tio->sg.sg_erase = CERASE;	/* ^H */
      tio->sg.sg_kill = CKILL;	/* ^U */
    }
  tio->sg.sg_flags = (CRMOD | ECHO | EVENP | ODDP);

  /* get special characters */
  if(ioctl(0, TIOCGETC, &(tio->tc)) < 0)
    {
      tio->tc.t_intrc = CINTR;	/* ^C */
      tio->tc.t_quitc = CQUIT;	/* ^\ */
      tio->tc.t_startc = CSTART;	/* ^Q */
      tio->tc.t_stopc = CSTOP;	/* ^S */
      tio->tc.t_eofc = CEOF;	/* ^D */
      tio->tc.t_brkc = -1;
    }

  /* get local special chars */
  if(ioctl(0, TIOCGLTC, &(tio->lc)) < 0)
    {
      tio->lc.t_suspc = CSUSP;	/* ^Z */
      tio->lc.t_dsuspc = CDSUSP;	/* ^Y */
      tio->lc.t_rprntc = CRPRNT;	/* ^R */
      tio->lc.t_flushc = CFLUSH;	/* ^O */
      tio->lc.t_werasc = CWERASE;	/* ^W */
      tio->lc.t_lnextc = CLNEXT;	/* ^V */
    }

  /* get line discipline */
  ioctl(0, TIOCGETD, &(tio->line));
#ifdef NTTYDISC
  tio->line = NTTYDISC;
#endif /* NTTYDISC */
  tio->local = (LCRTBS | LCRTERA | LCTLECH | LPASS8 | LCRTKIL);

  /*
   * guess an appropriate value for Backspace
   */
#ifdef DONT_GUESS_BACKSPACE
  PrivMode(ezterm, 1, PrivMode_BackSpace);	/* always ^H */
#else
  PrivMode(ezterm, (tio->sg.sg_erase == '\b'), PrivMode_BackSpace);
#endif /* DONT_GUESS_BACKSPACE */

#endif /* HAVE_TERMIOS_H */
}


/***********************************************************************
 *
 * Run the command in a subprocess and return a file descriptor for the
 * master end of the pseudo-teletype pair with the command talking to
 * the slave.
 */
static int run_command(ezterm, argv) EZ_Widget *ezterm; char *argv[];
{
  control_t *control = EZ_ITermControl(ezterm);
  int cmd_pid, ptyfd, fd_x, flags, i = 0;
  ttymode_t ttymode;

  ptyfd = get_pty(ezterm);
  if(ptyfd < 0) return(-1);
  control->cmd_fd = ptyfd;
  control->inputHandle = 
    EZ_AddInput(control->cmd_fd, EZ_READABLE_MASK, cmd_fd_reader, ezterm);

  /*
   * get tty settings before fork()
   * and make a reasonable guess at the value for BackSpace
   */
  get_ttymode(ezterm, &ttymode);
  /* add Backspace value */
  EZ_ITermSaveModes(ezterm) |= 
    (EZ_ITermPrivateModes(ezterm) & PrivMode_BackSpace);
  
  signal(SIGCHLD, Child_signal);

  /* need to trap SIGURG for SVR4 (Unixware) rlogin */
  /* signal (SIGURG, SIG_DFL); */
  fd_x = ConnectionNumber(EZ_GetDisplay());
  flags = fcntl(fd_x, F_GETFD) | FD_CLOEXEC;
  fcntl(fd_x, F_SETFD, flags);

  flags = fcntl(ptyfd, F_GETFD) | FD_CLOEXEC;
  fcntl(ptyfd, F_SETFD, flags);

  /* remember command args */
  while(argv[i] != NULL)  {control->args[i] = argv[i];  ++i; }  control->args[i] = NULL;

  XSync(EZ_Display, False);
  cmd_pid = fork();
  EZ_ITermChildPid(ezterm) = cmd_pid;

  if(cmd_pid < 0)
    {
      fprintf(stderr, "can't fork\n");
      EZ_RemoveInput(control->inputHandle);
      control->inputHandle = NULL;
      return(-1);
    }
  if(cmd_pid == 0)		/* child */
    {
#ifdef HAVE_UNSETENV
      /* avoid passing old settings and confusing term size */
      unsetenv("LINES");
      unsetenv("COLUMNS");
      /* avoid passing termcap since terminfo should be okay */
      unsetenv("TERMCAP");
#endif /* HAVE_UNSETENV */
      /* establish a controlling teletype for the new session */
      get_tty(ezterm);

      /* initialize terminal attributes */
      SET_TTYMODE(0, &(ttymode));
      tt_winsize(ezterm, 0);		/* set window size */

      /* reset signals and spin off the command interpreter */
      signal(SIGINT,  SIG_DFL);
      signal(SIGQUIT, SIG_DFL);
      signal(SIGCHLD, SIG_DFL);
      signal(SIGSEGV, SIG_DFL);
      signal(SIGBUS,  SIG_DFL);
      signal(SIGABRT, SIG_DFL);
      signal(SIGFPE,  SIG_DFL);
      signal(SIGILL,  SIG_DFL);
      signal(SIGALRM, SIG_DFL);

      /* command interpreter path */
      if(argv != NULL)
	{
          EZ_SitForALittleBit(200000); /* !!!! */
	  execvp(argv[0], argv);
	  fprintf(stderr, "can't execute \"%s\"", argv[0]);
	}
    }
  return(ptyfd);
}

/***********************************************************************/
/*
 * Tell the teletype handler what size the window is.
 * Called after a window size change.
 */
static void tt_winsize(ezterm, fd) EZ_Widget *ezterm; int fd;
{
  struct winsize ws;

  if(fd < 0) return;

  ws.ws_col = (unsigned short) EZ_ITermNCol(ezterm);
  ws.ws_row = (unsigned short) EZ_ITermNRow(ezterm);
  ws.ws_xpixel = ws.ws_ypixel = 0;
  ioctl(fd, TIOCSWINSZ, &ws);
}

void tt_resize(ezterm) EZ_Widget *ezterm;
{
  control_t *control = EZ_ITermControl(ezterm);
  tt_winsize(ezterm, control->cmd_fd);
}
/***********************************************************************/

/* Convert the keypress event into a string */
static void lookup_key(EZ_Widget *ezterm, XEvent * ev)
{
  static int numlock_state = 0; /* global !! */
  XComposeStatus compose;
  unsigned char kbuf[32];
  int ctrl, meta, shft, len;
  KeySym keysym;
  int    nrow = EZ_ITermNRow(ezterm);
  int saveLines = EZ_ITermSaveLines(ezterm);

  /*
   * use Num_Lock to toggle Keypad on/off.  If Num_Lock is off, allow an
   * escape sequence to toggle the Keypad.
   *
   * Always permit `shift' to override the current setting
   */
  shft = (ev->xkey.state & ShiftMask);
  ctrl = (ev->xkey.state & ControlMask);
  meta = (ev->xkey.state & Mod1Mask);
  if(numlock_state || (ev->xkey.state & Mod5Mask))
    {
      numlock_state = (ev->xkey.state & Mod5Mask);	/* numlock toggle */
      PrivMode(ezterm, (!numlock_state), PrivMode_aplKP);
    }
  len = XLookupString (&ev->xkey, kbuf, sizeof (kbuf), &keysym, &compose);
  /*
   * have unmapped Latin[2-4] entries -> Latin1
   * good for installations  with correct fonts, but without XLOCAL
   */
  if(!len && (keysym >= 0x0100) && (keysym < 0x0400))
    {
      len = 1;
      kbuf[0] = (keysym & 0xFF);
    }

  if(shft)
    {
      /* Shift + F1 - F10 generates F11 - F20 */
      if(keysym >= XK_F1 && keysym <= XK_F10)
	{
	  keysym += (XK_F11 - XK_F1);
	  shft = 0;		/* turn off Shift */
	}
      else if(!ctrl && !meta && (EZ_ITermPrivateModes(ezterm) & PrivMode_ShiftKeys))
	{
	  switch (keysym)
	    {
	      /* normal XTerm key bindings */
	    case XK_Prior:	/* Shift+Prior = scroll back */
	      if(saveLines)
		{
		  scr_page(ezterm, UP, nrow * 4 / 5);
		  return;
		}
	      break;

	    case XK_Next:	/* Shift+Next = scroll forward */
	      if(saveLines)
		{
		  scr_page(ezterm, DN, nrow * 4 / 5);
		  return;
		}
	      break;
	    case XK_Insert:	/* Shift+Insert = paste mouse selection */
	      selection_request(ezterm, ev->xkey.time, ev->xkey.x, ev->xkey.y);
	      return;
	      break;
	    }
	}
    }
  
  switch(keysym)
    {
    case XK_Print:
      return;
      break;
    case XK_Mode_switch:
      break;
    }
  
  if(keysym >= 0xFF00 && keysym <= 0xFFFF)
    {
	switch (keysym)
	  {
	  case XK_BackSpace:
	    len = 1;
	    kbuf[0] = (((EZ_ITermPrivateModes(ezterm) & PrivMode_BackSpace) ?
			!(shft | ctrl) : (shft | ctrl)) ? '\b' : '\177');
	    break;

	  case XK_Tab:
            if(meta) { EZ_FocusNextWidget(ezterm, shft); len = 0;}
	    else if(shft)
	      {
		len = 3;
		strcpy(kbuf, "\033[Z");
	      }
	    break;

	  case XK_Home:
	    len = strlen(strcpy (kbuf, KS_HOME));
	    break;
	  case XK_Left:	/* "\033[D" */
	  case XK_Up:		/* "\033[A" */
	  case XK_Right:	/* "\033[C" */
	  case XK_Down:	/* "\033[B" */
	    len = 3;
	    strcpy (kbuf, "\033[@");
	    kbuf[2] = ("DACB"[keysym - XK_Left]);
	    if(EZ_ITermPrivateModes(ezterm) & PrivMode_aplCUR)
	      {
		kbuf[1] = 'O';
	      }
	    /* do Shift first */
	    else if (shft)
	      {
		kbuf[2] = ("dacb"[keysym - XK_Left]);
	      }
	    else if (ctrl)
	      {
		kbuf[1] = 'O';
		kbuf[2] = ("dacb"[keysym - XK_Left]);
	      }
	    break;
	  case XK_Prior:
	    len = 4;
	    strcpy(kbuf, "\033[5~");
	    break;
	  case XK_Next:
	    len = 4;
	    strcpy(kbuf, "\033[6~");
	    break;
	  case XK_End:
	    len = strlen(strcpy (kbuf, KS_END));
	    break;

	  case XK_Select:
	    len = 4;
	    strcpy(kbuf, "\033[4~");
	    break;
	  case XK_Execute:
	    len = 4;
	    strcpy(kbuf, "\033[3~");
	    break;
	  case XK_Insert:
	    len = 4;
	    strcpy(kbuf, "\033[2~");
	    break;

	  case XK_Menu:
	    len = 5;
	    strcpy(kbuf, "\033[29~");
	    break;
	  case XK_Find:
	    len = 4;
	    strcpy(kbuf, "\033[1~");
	    break;
	  case XK_Help:
	    len = 5;
	    strcpy (kbuf, "\033[28~");
	    break;

	  case XK_KP_Enter:
	    /* allow shift to send normal "\033OM" */
	    if ((EZ_ITermPrivateModes(ezterm) & PrivMode_aplKP) ? !shft : shft)
	      {
		len = 1;
		kbuf[0] = '\r';
	      }
	    else
	      {
		len = 3;
		strcpy (kbuf, "\033OM");
	      }
	    break;

	  case XK_KP_F1:	/* "\033OP" */
	  case XK_KP_F2:	/* "\033OQ" */
	  case XK_KP_F3:	/* "\033OR" */
	  case XK_KP_F4:	/* "\033OS" */
	    len = 3;
	    strcpy (kbuf, "\033OP");
	    kbuf[2] += (keysym - XK_KP_F1);
	    break;

	  case XK_KP_Multiply:	/* "\033Oj" : "*" */
	  case XK_KP_Add:	/* "\033Ok" : "+" */
	  case XK_KP_Separator:	/* "\033Ol" : "," */
	  case XK_KP_Subtract:	/* "\033Om" : "-" */
	  case XK_KP_Decimal:	/* "\033On" : "." */
	  case XK_KP_Divide:	/* "\033Oo" : "/" */
	  case XK_KP_0:	/* "\033Op" : "0" */
	  case XK_KP_1:	/* "\033Oq" : "1" */
	  case XK_KP_2:	/* "\033Or" : "2" */
	  case XK_KP_3:	/* "\033Os" : "3" */
	  case XK_KP_4:	/* "\033Ot" : "4" */
	  case XK_KP_5:	/* "\033Ou" : "5" */
	  case XK_KP_6:	/* "\033Ov" : "6" */
	  case XK_KP_7:	/* "\033Ow" : "7" */
	  case XK_KP_8:	/* "\033Ox" : "8" */
	  case XK_KP_9:	/* "\033Oy" : "9" */
	    /* allow shift to override 'numlock_off' */
	    if ((EZ_ITermPrivateModes(ezterm) & PrivMode_aplKP) == 0)
	      {
		/* if numlock = off, for non numerics do : */
		len = 1;
		kbuf[0] = ('*' + (keysym - XK_KP_Multiply));
	      }
	    break;

	  case XK_KP_Left:	/* "\033[D" */
	  case XK_KP_Up:	/* "\033[A" */
	  case XK_KP_Right:	/* "\033[C" */
	  case XK_KP_Down:	/* "\033[B" */
	    len = 3;
	    strcpy (kbuf, "\033[@");
	    kbuf[2] = ("DACB"[keysym - XK_KP_Left]);
	    if(EZ_ITermPrivateModes(ezterm) & PrivMode_aplCUR)
	      {
		kbuf[1] = 'O';
	      }
	    else if (ctrl)
	      {
		kbuf[1] = 'O';
		kbuf[2] = ("dacb"[keysym - XK_KP_Left]);
	      }
	    break;
	  case XK_KP_Prior:
	    len = 4;
	    strcpy (kbuf, "\033[5~");
	    break;
	  case XK_KP_Next:
	    len = 4;
	    strcpy (kbuf, "\033[6~");
	    break;
	  case XK_KP_End:
	    len = strlen (strcpy (kbuf, KS_END));
	    break;
	  case XK_KP_Insert:
	    len = 4;
	    strcpy (kbuf, "\033[2~");
	    break;
	  case XK_KP_Home:
	    len = strlen (strcpy (kbuf, KS_HOME));
	    break;
	  case XK_KP_Delete:
	    len = strlen (strcpy (kbuf, KS_DELETE));
	    break;

#define FKEY(n,fkey) len = 5;\
sprintf (kbuf,"\033[%02d~", (int)((n) + (keysym - fkey)))

	  case XK_F1:		/* "\033[11~" */
	  case XK_F2:		/* "\033[12~" */
	  case XK_F3:		/* "\033[13~" */
	  case XK_F4:		/* "\033[14~" */
	  case XK_F5:		/* "\033[15~" */
	    FKEY (11, XK_F1);
	    break;

	  case XK_F6:		/* "\033[17~" */
	  case XK_F7:		/* "\033[18~" */
	  case XK_F8:		/* "\033[19~" */
	  case XK_F9:		/* "\033[20~" */
	  case XK_F10:		/* "\033[21~" */
	    FKEY (17, XK_F6);
	    break;

	  case XK_F11:		/* "\033[23~" */
	  case XK_F12:		/* "\033[24~" */
	  case XK_F13:		/* "\033[25~" */
	  case XK_F14:		/* "\033[26~" */
	    FKEY (23, XK_F11);
	    break;

	  case XK_F15:		/* "\033[28~" */
	  case XK_F16:		/* "\033[29~" */
	    FKEY (28, XK_F15);
	    break;

	  case XK_F17:		/* "\033[31~" */
	  case XK_F18:		/* "\033[32~" */
	  case XK_F19:		/* "\033[33~" */
	  case XK_F20:		/* "\033[34~" */
	  case XK_F21:		/* "\033[35~" */
	  case XK_F22:		/* "\033[36~" */
	  case XK_F23:		/* "\033[37~" */
	  case XK_F24:		/* "\033[38~" */
	  case XK_F25:		/* "\033[39~" */
	  case XK_F26:		/* "\033[40~" */
	  case XK_F27:		/* "\033[41~" */
	  case XK_F28:		/* "\033[42~" */
	  case XK_F29:		/* "\033[43~" */
	  case XK_F30:		/* "\033[44~" */
	  case XK_F31:		/* "\033[45~" */
	  case XK_F32:		/* "\033[46~" */
	  case XK_F33:		/* "\033[47~" */
	  case XK_F34:		/* "\033[48~" */
	  case XK_F35:		/* "\033[49~" */
	    FKEY (31, XK_F17);
	    break;
#undef FKEY
#ifdef KS_DELETE
	  case XK_Delete:
	    len = strlen(strcpy (kbuf, KS_DELETE));
	    break;
#endif
	  }
    }
  else if(ctrl && keysym == XK_minus)
    {
      len = 1;
      kbuf[0] = '\037';		/* Ctrl-Minus generates ^_ (31) */
    }

  if(len <= 0)
    return;			/* not mapped */

  /*
   * these modifications only affect the static keybuffer
   * pass Shift/Control indicators for function keys ending with `~'
   *
   * eg,
   *   Prior = "ESC[5~"
   *   Shift+Prior = "ESC[5~"
   *   Ctrl+Prior = "ESC[5^"
   *   Ctrl+Shift+Prior = "ESC[5@"
   */
  if(kbuf[0] == '\033' && kbuf[1] == '[' && kbuf[len - 1] == '~')
    kbuf[len - 1] = (shft ? (ctrl ? '@' : '$') : (ctrl ? '^' : '~'));

  /* escape prefix */
  if(meta)
    {
      unsigned char ch = '\033';
      tt_write(ezterm, &ch, 1);
    }
  tt_write(ezterm, kbuf, len);
}
/*******************************************************************************/
/*
 * send buf to child process 
 */
#if defined(linux)
#ifdef PTY_BUF_SIZE /* From <linux/tty.h> */
#define MAX_PTY_WRITE PTY_BUF_SIZE
#endif
#endif
/* NOTE: _POSIX_MAX_INPUT is defined _through_ <limits.h> at least for
 * the following systems: HP-UX 10.20, AIX (no idea about the version),
 * OSF1/alpha 4.0, Linux (probably any Linux system).
 */
#ifndef MAX_PTY_WRITE
#ifdef _POSIX_VERSION
#ifdef _POSIX_MAX_INPUT
#define MAX_PTY_WRITE _POSIX_MAX_INPUT
#else
#define MAX_PTY_WRITE 255 /* POSIX minimum MAX_INPUT */
#endif
#endif
#endif

#ifndef MAX_PTY_WRITE
#define MAX_PTY_WRITE 128	/* 1/2 POSIX minimum MAX_INPUT */
#endif

void tt_write(ezterm, buf, nbytes) 
     EZ_Widget *ezterm; char *buf; int nbytes;
{
  int nwrite, nwrote, nspace= nbytes+2;
  control_t *control = EZ_ITermControl(ezterm);  

  if(control->outbuf == NULL) /* allocate outbuf */
    {
      control->outbuf = (unsigned char *)malloc((nspace)*sizeof(unsigned char));
      control->outbufEnd = control->outbuf + nspace;
      control->outbufB = control->outbufP = control->outbuf;
    }
  /* append the the current buffer */
  if(nbytes > 0)
    {
      int itmp = control->outbufEnd - control->outbufP;
      if(nspace > itmp)
        {
          int jtmp = control->outbufP - control->outbufB;
          if(control->outbufB != control->outbuf)
            {
              char *src=control->outbufB;
              char *dst=control->outbuf;
              int i=0;
              while(i<jtmp) {*dst++ = *src++; i++;}
              /* memmove(control->outbuf,control->outbufB, jtmp);*/
            }
          control->outbufB = control->outbuf;
          control->outbufP = control->outbuf + jtmp;
        }
      itmp = control->outbufEnd - control->outbufP;      
      if(nspace > itmp)
        {
          int jtmp = control->outbufP - control->outbufB;
          itmp = jtmp + nspace;
          if((control->outbuf = (unsigned char *)realloc( control->outbuf,
                                                     itmp * sizeof(unsigned char))) != NULL)
            {
              control->outbufB = control->outbuf;
              control->outbufP =  control->outbuf + jtmp;
              control->outbufEnd  = control->outbuf + itmp;
            }
          else {fprintf(stderr, "'send_string': Out of memory\n"); exit(2);}
        }
      memcpy(control->outbufP, buf, nbytes);
      control->outbufP += nbytes;
    }
  
  if((nwrite = control->outbufP - control->outbufB) > 0)
    {
      int trysize = nwrite;
      if(trysize > MAX_PTY_WRITE) trysize = MAX_PTY_WRITE;
      nwrote = write(control->cmd_fd, control->outbufB, trysize);
      if(nwrote < 0) nwrote = 0;
      control->outbufB += nwrote;
      if(control->outbufB >= control->outbufP)
        {
          if(control->outbufEnd - control->outbuf > 4096)
            control->outbuf = (unsigned char *) realloc(control->outbuf, 1024*sizeof(char));
          control->outbufB = control->outbufP = control->outbuf;
        }
    }
}
/*************************************************************************
 *
 * read from cmd_fd, called by a file handler. 
 */
static void cmd_fd_reader(input, data, fd, mask)
     void *input; void *data; int fd, mask;
{
  int       total, nread;
  EZ_Widget *ezterm = (EZ_Widget *)data;
  control_t *control = EZ_ITermControl(ezterm);
  
  /* something is in buffer */
  if(control->cmdbuf_ptr < control->cmdbuf_endp)
    {
      if(control->cmdbuf_ptr != control->cmdbuf_base)
	{
	  int i=0, itmp = control->cmdbuf_endp - control->cmdbuf_ptr;
          char *src= control->cmdbuf_ptr;
          char *dst= control->cmdbuf_base;
          while(i<itmp) { *dst++ = *src++; i++;}
	  /* memmove(control->cmdbuf_base, control->cmdbuf_ptr, itmp);*/
	  control->cmdbuf_ptr = control->cmdbuf_base;
	  control->cmdbuf_endp = control->cmdbuf_ptr + itmp;
	}
    }
  else /* buffer is empty */
    {
      control->cmdbuf_ptr = control->cmdbuf_base;
      control->cmdbuf_endp = control->cmdbuf_ptr;
    }
  total = 8192 - (control->cmdbuf_endp - control->cmdbuf_ptr);
  if(total > 0)
    {
      nread = read(control->cmd_fd, control->cmdbuf_endp, total);
      if(nread > 0) control->cmdbuf_endp += nread;
    }
}
/*******************************************************************************/
/*
 * Return the next input character
 */
unsigned char cmd_getc(EZ_Widget *ezterm)
{
  control_t *control = EZ_ITermControl(ezterm);  
  int    nrow = EZ_ITermNRow(ezterm);

  if(control->refresh_count > nrow)
    {
      control->refresh_count = 0;
      control->refreshed = 1;
      scr_refresh(ezterm, control->refresh_type);
    }
  
  if(control->cmdbuf_ptr >= control->cmdbuf_endp)
    {
      control->refresh_count = 0;
      if(control->refreshed == 0)
        {
          control->refreshed = 1;
          scr_refresh(ezterm, control->refresh_type);
        }
      return('\0');
    }
  else
    {
      unsigned char ch = *(control->cmdbuf_ptr);
      control->cmdbuf_ptr++;
      control->refreshed = 0;
      return(ch);
    }
}
/************************************************************************/
/*
 * Send printf() formatted output to the command.
 * Only use for small ammounts of data.
 */
void tt_printf  MY_VARARGS(void *, the_args)
     MY_VARARGS_DECL
{
  static unsigned char buf[256];
  const unsigned char *fmt;
  EZ_Widget *ezterm;
  va_list ap;

  ezterm = (EZ_Widget *)( MY_VARARGS_START(void *, the_args, ap));
  fmt = va_arg(ap, char *);
  vsprintf(buf, fmt, ap);
  va_end(ap);
  tt_write(ezterm, buf, strlen(buf));
}

/* process escape sequences */
static void process_escape_seq(ezterm) EZ_Widget *ezterm;
{
  unsigned char ch = cmd_getc(ezterm);

  switch(ch)
    {
      /* case 1:        do_tek_mode (); break; */
    case '#':
      if (cmd_getc(ezterm) == '8')
	scr_E(ezterm);
      break;
    case '(':
      scr_charset_set(ezterm, 0, cmd_getc(ezterm));
      break;
    case ')':
      scr_charset_set (ezterm, 1, cmd_getc(ezterm));
      break;
    case '*':
      scr_charset_set (ezterm, 2, cmd_getc(ezterm));
      break;
    case '+':
      scr_charset_set (ezterm, 3, cmd_getc(ezterm));
      break;
    case '7':
      scr_cursor(ezterm, SAVE);
      break;
    case '8':
      scr_cursor(ezterm, RESTORE);
      break;
    case '=':
    case '>':
      PrivMode(ezterm, (ch == '='), PrivMode_aplKP);
      break;
    case '@':
      (void) cmd_getc(ezterm);
      break;
    case 'D':
      scr_index(ezterm, UP);
      break;
    case 'E':
      scr_add_lines(ezterm, "\n\r", 1, 2);
      break;
    case 'H':
      scr_set_tab(ezterm, 1);
      break;
    case 'M':
      scr_index(ezterm, DN);
      break;
    case 'Z':
      tt_printf(ezterm, ESCZ_ANSWER);
      break;			/* steal obsolete ESC [ c */
    case '[':
      process_csi_seq(ezterm);
      break;
    case ']':
      process_xterm_seq(ezterm);
      break;
    case 'c':
      scr_poweron(ezterm);
      break;
    case 'n':
      scr_charset_choose(ezterm, 2);
      break;
    case 'o':
      scr_charset_choose (ezterm, 3);
      break;
    }
}

/* process CSI (code sequence introducer) sequences `ESC[' */
static void process_csi_seq(ezterm) EZ_Widget *ezterm;
{
  unsigned char ch, priv;
  unsigned int nargs;
  int arg[ESC_ARGS];

  nargs = 0;
  arg[0] = 0;
  arg[1] = 0;

  priv = 0;
  ch = cmd_getc(ezterm);
  if(ch >= '<' && ch <= '?')
    {
      priv = ch;
      ch = cmd_getc(ezterm);
    }

  /* read any numerical arguments */
  do
    {
      int n;
      for(n = 0; isdigit(ch); ch = cmd_getc(ezterm))
	n = n * 10 + (ch - '0');

      if(nargs < ESC_ARGS)
	arg[nargs++] = n;

      if(ch == '\b')
	{
	  scr_backspace(ezterm);
	}
      else if(ch == 033)
	{
	  process_escape_seq(ezterm);
	  return;
	}
      else if(ch < ' ')
	{
	  scr_add_lines(ezterm, &ch, 0, 1);
	  return;
	}
      if (ch < '@')
	ch = cmd_getc(ezterm);
    }
  while (ch >= ' ' && ch < '@');
  if (ch == 033)
    {
      process_escape_seq(ezterm);
      return;
    }
  else if (ch < ' ')
    return;
  switch (ch)
    {
    case 'A':
    case 'e':			/* up <n> */
      scr_gotorc(ezterm, (arg[0] ? -arg[0] : -1), 0, RELATIVE);
      break;
    case 'B':			/* down <n> */
      scr_gotorc(ezterm, (arg[0] ? +arg[0] : +1), 0, RELATIVE);
      break;
    case 'C':
    case 'a':			/* right <n> */
      scr_gotorc(ezterm, 0, (arg[0] ? +arg[0] : +1), RELATIVE);
      break;
    case 'D':			/* left <n> */
      scr_gotorc(ezterm, 0, (arg[0] ? -arg[0] : -1), RELATIVE);
      break;
    case 'E':			/* down <n> & to first column */
      scr_gotorc(ezterm, (arg[0] ? +arg[0] : +1), 0, R_RELATIVE);
      break;
    case 'F':			/* up <n> & to first column */
      scr_gotorc(ezterm, (arg[0] ? -arg[0] : -1), 0, R_RELATIVE);
      break;
    case 'G':
    case '`':			/* move to col <n> */
      scr_gotorc(ezterm, 0, (arg[0] ? +arg[0] : +1), R_RELATIVE);
      break;
    case 'd':			/* move to row <n> */
      scr_gotorc(ezterm, (arg[0] ? +arg[0] : +1), 0, C_RELATIVE);
      break;
    case 'H':
    case 'f':			/* position cursor */
      switch (nargs)
	{
	case 0:
	  scr_gotorc(ezterm, 0, 0, 0);
	  break;
	case 1:
	  scr_gotorc(ezterm, (arg[0] ? arg[0] - 1 : 0), 0, 0);
	  break;
	default:
	  scr_gotorc(ezterm, arg[0] - 1, arg[1] - 1, 0);
	  break;
	}
      break;
    case 'I':
      scr_tab(ezterm, arg[0] ? +arg[0] : +1);
      break;
    case 'Z':
      scr_tab(ezterm, arg[0] ? -arg[0] : -1);
      break;
    case 'J':
      scr_erase_screen(ezterm, arg[0]);
      break;
    case 'K':
      scr_erase_line(ezterm, arg[0]);
      break;
    case '@':
      scr_insdel_chars(ezterm, (arg[0] ? arg[0] : 1), INSERT);
      break;
    case 'L':
      scr_insdel_lines(ezterm, (arg[0] ? arg[0] : 1), INSERT);
      break;
    case 'M':
      scr_insdel_lines(ezterm, (arg[0] ? arg[0] : 1), DELETE);
      break;
    case 'X':
      scr_insdel_chars(ezterm, (arg[0] ? arg[0] : 1), ERASE);
      break;
    case 'P':
      scr_insdel_chars(ezterm, (arg[0] ? arg[0] : 1), DELETE);
      break;

    case 'c':
      tt_printf(ezterm, VT100_ANS);
      break;
    case 'm':
      process_sgr_mode(ezterm, nargs, arg);
      break;
    case 'n':			/* request for information */
      switch(arg[0])
	{
	case 5:
	  tt_printf(ezterm, "\033[0n");
	  break;		/* ready */
	case 6:
	  scr_report_position(ezterm);
	  break;
	case 8:
	  /* xterm_seq(XTerm_title, APL_NAME "-" VERSION);*/
	  break;
	}
      break;
    case 'r':			/* set top and bottom margins */
      if(priv != '?')
	{
	  if(nargs < 2 || arg[0] >= arg[1])
	    scr_scroll_region(ezterm, 0, 10000);
	  else
	    scr_scroll_region(ezterm, arg[0] - 1, arg[1] - 1);
	  break;
	}
      /* drop */
    case 's':
    case 't':
    case 'h':
    case 'l':
      process_terminal_mode(ezterm, ch, priv, nargs, arg);
      break;
    case 'g':
      switch(arg[0])
	{
	case 0:
	  scr_set_tab(ezterm, 0);
	  break;		/* delete tab */
	case 3:
	  scr_set_tab(ezterm, -1);
	  break;		/* clear all tabs */
	}
      break;
    case 'W':
      switch(arg[0])
	{
	case 0:
	  scr_set_tab(ezterm, 1);
	  break;		/* = ESC H */
	case 2:
	  scr_set_tab(ezterm, 0);
	  break;		/* = ESC [ 0 g */
	case 5:
	  scr_set_tab(ezterm, -1);
	  break;		/* = ESC [ 3 g */
	}
      break;
    }
}

/* process xterm text parameters sequences `ESC ] Ps ; Pt BEL' */
static void process_xterm_seq(ezterm) EZ_Widget *ezterm;
{
  unsigned char ch, string[STRING_MAX];
  int arg;

  ch = cmd_getc(ezterm);
  for(arg = 0; isdigit(ch); ch = cmd_getc(ezterm))
    arg = arg * 10 + (ch - '0');

  if(ch == ';')
    {
      int n = 0;
      while((ch = cmd_getc(ezterm)) != 007)
	{
	  if((n < sizeof (string) - 1))
	    {
	      /* silently translate whitespace to space char */
	      if(isspace (ch))
		ch = ' ';
	      if(ch >= ' ')
		string[n++] = ch;
	    }
	}
      string[n] = '\0';
      xterm_seq(ezterm, arg, string);
    }
}

/* process DEC private mode sequences `ESC [ ? Ps mode' */
/*
 * mode can only have the following values:
 *      'l' = low
 *      'h' = high
 *      's' = save
 *      'r' = restore
 *      't' = toggle
 * so no need for fancy checking
 */
static void process_terminal_mode(EZ_Widget *ezterm, int mode, int priv,
		       unsigned int nargs, int arg[])
{
  unsigned int i;
  int state;

  if(nargs == 0)
    return;

  /* make lo/hi boolean */
  switch(mode)
    {
    case 'l':
      mode = 0;
      break;
    case 'h':
      mode = 1;
      break;
    }

  switch(priv)
    {
    case 0:
      if(mode && mode != 1)
	return;			/* only do high/low */
      for(i = 0; i < nargs; i++)
	switch (arg[i])
	  {
	  case 4:
	    scr_insert_mode(ezterm, mode);
	    break;

	  case 36:
	    PrivMode(ezterm, mode, PrivMode_BackSpace);
	    break;
	    /* case 38:  TEK mode */
	  }
      break;

#define PrivCases(ezt, bit)	\
  if(mode == 't') state = !(EZ_ITermPrivateModes(ezt) & bit); else state = mode;\
  switch (state) {\
	     case 's': EZ_ITermSavedModes(ezt) |= (EZ_ITermPrivateModes(ezt) & bit); continue; break;\
             case 'r': state = (EZ_ITermSavedModes(ezt) & bit) ? 1 : 0;/*drop*/\
             default:  PrivMode(ezt, state, bit); }

    case '?':
      for(i = 0; i < nargs; i++)
	switch(arg[i])
	  {
	  case 1:		/* application cursor keys */
	    PrivCases(ezterm, PrivMode_aplCUR);
	    break;

	    /* case 2:   - reset charsets to USASCII */

	  case 3:		/* 80/132 */
	    PrivCases(ezterm, PrivMode_132);
	    if(EZ_ITermPrivateModes(ezterm) & PrivMode_132OK)
	      set_width(ezterm, state ? 132 : 80);
	    break;

	  case 5:		/* reverse video */
	    PrivCases(ezterm,PrivMode_rVideo);
            EZ_ITermReverseVideo(ezterm, state);
	    break;

	  case 6:		/* relative/absolute origins  */
	    PrivCases(ezterm, PrivMode_relOrigin);
	    scr_relative_origin(ezterm, state);
	    break;

	  case 7:		/* autowrap */
	    PrivCases(ezterm, PrivMode_Autowrap);
	    scr_autowrap(ezterm, state);
	    break;

	    /* case 8:   - auto repeat, can't do on a per window basis */

	  case 9:		/* X10 mouse reporting */
	    PrivCases(ezterm, PrivMode_MouseX10);
	    /* orthogonal */
	    if(EZ_ITermPrivateModes(ezterm) & PrivMode_MouseX10)
	      EZ_ITermPrivateModes(ezterm) &= ~(PrivMode_MouseX11);
	    break;

	  case 25:		/* visible/invisible cursor */
	    PrivCases(ezterm, PrivMode_VisibleCursor);
	    scr_cursor_visible(ezterm, state);
	    break;

	  case 35:
	    PrivCases(ezterm, PrivMode_ShiftKeys);
	    break;

	  case 36:
	    PrivCases(ezterm, PrivMode_BackSpace);
	    break;

	  case 40:		/* 80 <--> 132 mode */
	    PrivCases(ezterm, PrivMode_132OK);
	    break;

	  case 47:		/* secondary screen */
	    /*
	    PrivCases(ezterm,PrivMode_Screen);
	    scr_change_screen (state);
	    */
	    break;

	  case 66:		/* application key pad */
	    PrivCases(ezterm, PrivMode_aplKP);
	    break;

	  case 1000:		/* X11 mouse reporting */
	    PrivCases(ezterm, PrivMode_MouseX11);
	    /* orthogonal */
	    if(EZ_ITermPrivateModes(ezterm) & PrivMode_MouseX11)
	      EZ_ITermPrivateModes(ezterm) &= ~(PrivMode_MouseX10);
	    break;
	  }
#undef PrivCases
      break;
    }
}

/* process sgr sequences */
static void process_sgr_mode(EZ_Widget *ezterm, unsigned int nargs, int arg[])
{
  unsigned int i;

  if (nargs == 0)
    {
      scr_rendition(ezterm, 0, ~RS_None);
      return;
    }
  for(i = 0; i < nargs; i++)
    switch (arg[i])
      {
      case 0:
	scr_rendition(ezterm, 0, ~RS_None);
	break;
      case 1:
	scr_rendition(ezterm, 1, RS_Bold);
	break;
      case 4:
	scr_rendition(ezterm, 1, RS_Uline);
	break;
      case 5:
	scr_rendition(ezterm, 1, RS_Blink);
	break;
      case 7:
	scr_rendition(ezterm, 1, RS_RVid);
	break;
      case 22:
	scr_rendition(ezterm, 0, RS_Bold);
	break;
      case 24:
	scr_rendition(ezterm, 0, RS_Uline);
	break;
      case 25:
	scr_rendition(ezterm, 0, RS_Blink);
	break;
      case 27:
	scr_rendition(ezterm, 0, RS_RVid);
	break;

      case 30:
      case 31:			/* set fg color */
      case 32:
      case 33:
      case 34:
      case 35:
      case 36:
      case 37:
	scr_color(ezterm, minColor + (arg[i] - 30), RS_Bold);
	break;
      case 39:			/* default fg */
	scr_color(ezterm, restoreFG, RS_Bold);
	break;

      case 40:
      case 41:			/* set bg color */
      case 42:
      case 43:
      case 44:
      case 45:
      case 46:
      case 47:
	scr_color(ezterm, minColor + (arg[i] - 40), RS_Blink);
	break;
      case 49:			/* default bg */
	scr_color(ezterm, restoreBG, RS_Blink);
	break;
      }
}
/*********************************************************************************************/
void EZ_ITermTimerCb(timer, pdata) EZ_Timer *timer; void *pdata;
{
  EZ_Widget *ezterm = (EZ_Widget *)pdata;
  control_t *control;
  int ch;

  if(EZ_LookupWidgetFromAllHT(ezterm) == NULL)
    {
      EZ_CancelTimer(timer);
      return;
    }
  control = EZ_ITermControl(ezterm);  
  while( (ch = cmd_getc(ezterm)) != 0)
    {
      if(ch >= ' ' || ch == '\t' || ch == '\n' || ch == '\r')
	{
	  /* Read a text string from the input buffer */
	  int nlines = 0;
	  unsigned char *str;
	  int nrow = EZ_ITermNRow(ezterm);
	  /*
	   * point to the start of the string,
	   * decrement first since already did get_com_char ()
	   */
	  str = --(control->cmdbuf_ptr);
	  while(control->cmdbuf_ptr < control->cmdbuf_endp)
	    {
	      ch = *(control->cmdbuf_ptr);
	      if(ch >= ' ' || ch == '\t' || ch == '\n' || ch == '\r')
		{
		  control->cmdbuf_ptr++;
		  if(ch == '\n')
		    {
		      nlines++;
		      control->refresh_count++;

		      if(control->refresh_count > nrow)
			break;
		    }
		}
	      else
		/* unprintable */
		{
		  break;
		}
	    }
	  scr_add_lines(ezterm, str, nlines, (control->cmdbuf_ptr - str));
	}
      else
	{
	  switch (ch)
	    {
	    case 005:
	      tt_printf(ezterm, VT100_ANS);
	      break;		/* terminal Status */
	    case 007:
	      scr_bell(ezterm);
	      break;		/* bell */
	    case '\b':
	      scr_backspace(ezterm);
	      break;		/* backspace */
	    case 013:
	    case 014:
	      scr_index(ezterm,UP);
	      break;		/* vertical tab, form feed */
	    case 016:
	      scr_charset_choose(ezterm,1);
	      break;		/* shift out - acs */
	    case 017:
	      scr_charset_choose(ezterm,0);
	      break;		/* shift in - acs */
	    case 033:
	      process_escape_seq(ezterm);
	      break;
	    }
	}
    }
}
/*************************************************************************
 *
 */
void EZ_iterm_resize_window(ezterm) EZ_Widget *ezterm;
{
  int bwp = EZ_WidgetBorderWidth(ezterm)+EZ_WidgetPadB(ezterm);
  int padx = EZ_WidgetPadX(ezterm);
  int pady = EZ_WidgetPadY(ezterm);
  int width = EZ_WidgetWidth(ezterm) - ( (bwp+padx)<<1);
  int height= EZ_WidgetHeight(ezterm) - ( (bwp+pady)<<1);
  int ncols= width/EZ_ITermFWidth(ezterm);
  int nrows= height/EZ_ITermFHeight(ezterm);

  if(ncols != EZ_ITermNCol(ezterm) || nrows != EZ_ITermNRow(ezterm))
    {
      EZ_ITermNCol(ezterm) = ncols;
      EZ_ITermNRow(ezterm) = nrows;
      scr_reset(ezterm);
    }
  EZ_ITermXOff(ezterm) = bwp + padx;
  EZ_ITermYOff(ezterm) = bwp + pady;
  EZ_ITermXDiff(ezterm) = width - ncols * EZ_ITermFWidth(ezterm);
  EZ_ITermYDiff(ezterm) = height- nrows * EZ_ITermFHeight(ezterm);
}
/*************************************************************************
 *
 *  Event handling
 */
extern Time  EZ_LastButton1ClickTime;
#define MULTICLICK_TIME	 500

void EZ_ITermEventHandle(ezterm, ev)
     EZ_Widget *ezterm; XEvent *ev;
{
  static int clicks = 0;
#define clickOnce()	(clicks <= 1)
  static int bypass_keystate = 0;
  int reportmode;
  control_t *control = EZ_ITermControl(ezterm);  
  
  control->refreshed = 0;

  switch (ev->type)
    {
    case KeyPress:
      if(EZ_ITermFocus(ezterm)==0)
        {
          Window fw; int rt;
          EZ_Widget *tmp;
          XGetInputFocus(EZ_Display, &fw, &rt);
          tmp = EZ_LookupWidgetFromMappedHT(fw);
          if(tmp != ezterm) EZ_SetFocusTo(ezterm);
          EZ_ITermFocus(ezterm) = 1;
        }
      lookup_key(ezterm, ev);
      break;
    case MappingNotify:
      XRefreshKeyboardMapping(&(ev->xmapping));
      break;
    case ConfigureNotify:
      EZ_ITermDirty(ezterm) = 1;
      scr_expose(ezterm, 0, 0, EZ_WidgetWidth(ezterm),
                 EZ_WidgetHeight(ezterm));
      EZ_DrawWidget(ezterm);
      break;
    case SelectionClear:
      selection_clear(ezterm);
      break;

    case SelectionNotify:
      selection_paste(ezterm, ev->xselection.requestor, ev->xselection.property, True);
      break;

    case SelectionRequest:
      EZ_HandleSelectionRequest(ezterm, ev);
      break;

    case FocusOut:
      EZ_ITermFocus(ezterm)=0;
      break;
    case FocusIn:
      EZ_ITermFocus(ezterm)=1;
      break;

    case Expose:
      scr_expose(ezterm, 0, 0, EZ_WidgetWidth(ezterm),
                 EZ_WidgetHeight(ezterm));
      EZ_DrawWidget(ezterm);
      break;

    case VisibilityNotify:
      switch (ev->xvisibility.state)
	{
	case VisibilityUnobscured:
	  control->refresh_type = FAST_REFRESH;
	  break;
	case VisibilityPartiallyObscured:
	  control->refresh_type = SLOW_REFRESH;
	  break;
	default:
	  control->refresh_type = NO_REFRESH;
	  break;
	}
      break;

    case ButtonPress:
      {
	Window fw; int rt;
	EZ_Widget *tmp;
	XGetInputFocus(EZ_Display, &fw, &rt);
	tmp = EZ_LookupWidgetFromMappedHT(fw);
	if(tmp != ezterm) EZ_SetFocusTo(ezterm);
      }
      bypass_keystate = (ev->xbutton.state & (Mod1Mask | ShiftMask));
      reportmode = (bypass_keystate ?
		    0 : (EZ_ITermPrivateModes(ezterm) & PrivMode_mouse_report));
      if(reportmode)
        {
          if(reportmode & PrivMode_MouseX10)
            {
              /* no state info allowed */
              ev->xbutton.state = 0;
            }
          if(ev->xbutton.button == EZ_Btn1)
            {
              if(ev->xbutton.time - EZ_LastButton1ClickTime < MULTICLICK_TIME) clicks++;
              else  clicks = 1;
            }
          mouse_report(ezterm, &(ev->xbutton));
        }
      else
        {
          if(ev->xbutton.button == EZ_Btn1)
            {
              if(ev->xbutton.time -  EZ_LastButton1ClickTime  < MULTICLICK_TIME) clicks++;
              else clicks = 1;
              selection_click(ezterm, clicks, ev->xbutton.x, ev->xbutton.y);
            }
          else if(ev->xbutton.button == EZ_Btn3)
            {
              selection_extend(ezterm, ev->xbutton.x, ev->xbutton.y);
            }
        }
      EZ_LastButton1ClickTime = ev->xbutton.time;
      return;
      break;
    case ButtonRelease:
      reportmode = (bypass_keystate ?
		    0 : (EZ_ITermPrivateModes(ezterm) & PrivMode_mouse_report));
      if(reportmode)
        {
          switch(reportmode & PrivMode_mouse_report)
            {
            case PrivMode_MouseX10:
              break;
            case PrivMode_MouseX11:
              ev->xbutton.state = bypass_keystate;
              ev->xbutton.button = AnyButton;
              mouse_report(ezterm, &(ev->xbutton));
              break;
            }
          return;
        }
      /*
       * dumb hack to compensate for the failure of click-and-drag
       * when overriding mouse reporting
       */
      if((EZ_ITermPrivateModes(ezterm) & PrivMode_mouse_report) &&
          (bypass_keystate) && (ev->xbutton.button == EZ_Btn1) &&  (clickOnce() ))
        selection_extend(ezterm, ev->xbutton.x, ev->xbutton.y);

      if(ev->xbutton.button == EZ_Btn1  || ev->xbutton.button == EZ_Btn3) 
        selection_make(ezterm, ev->xbutton.time);
      else if(ev->xbutton.button == EZ_Btn2)
        selection_request(ezterm, ev->xbutton.time,
                          ev->xbutton.x, ev->xbutton.y);
      break;

    case MotionNotify:
      if ((ev->xbutton.state & (Button1Mask | Button3Mask)) && clickOnce ())
        {
          Window unused_root, unused_child;
          int unused_root_x, unused_root_y;
          unsigned int unused_mask;
              
          while (XCheckTypedWindowEvent(EZ_Display, EZ_WidgetWindow(ezterm),
                                        MotionNotify, ev));
          XQueryPointer (EZ_Display, EZ_WidgetWindow(ezterm),
                         &unused_root, &unused_child,
                         &unused_root_x, &unused_root_y,
                         &(ev->xbutton.x), &(ev->xbutton.y),
                         &unused_mask);
          selection_extend(ezterm, (ev->xbutton.x), (ev->xbutton.y));
        }
      break;
    }
}

/***********************************************************************************/
/*
 * XTerm escape sequences: ESC ] Ps;Pt BEL
 *       0 = change iconName/title
 *       1 = change iconName
 *       2 = change title
 *      46 = change logfile (not implemented)
 *      50 = change font
 *
 * xiterm extensions:
 *      20 = bg pixmap
 *      39 = change default fg color
 *      49 = change default bg color
 */
void xterm_seq (ezterm,  op, str)
     EZ_Widget *ezterm; int op; char *str;
{

  if(str)
    {
      switch (op)
        {
        case XTerm_name:
        case XTerm_iconName:
          break;
        case XTerm_title:
          break;
        case XTerm_Pixmap:
          break;
        case XTerm_restoreFG:
          break;
        case XTerm_restoreBG:
          break;
        case XTerm_font:
          break;
        }
    }
}
void set_width(ezterm, width)
     EZ_Widget *ezterm; int width;
{
  int ncol = EZ_ITermNCol(ezterm);
  if(width != ncol)
    {
      EZ_ITermNCol(ezterm) = width;
      EZ_ClearWidgetSizeComputedFlag(ezterm);      
      EZ_ReDisplayWidget(ezterm);
      XRaiseWindow(EZ_Display,EZ_WidgetWindow(ezterm));
    }
}

/***********************************************************************************/
#undef _EZ_ITERM_COMMAND_C_

