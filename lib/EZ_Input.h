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
/*******************************************************************
 *   Types, constants need for EZ_AddInput
 */
#define EZ_ADD_INPUT
#ifdef EZ_ADD_INPUT
/*****************************************************************/

#ifndef howmany
#define howmany(x, y)	(((x)+((y)-1))/(y))
#endif

#ifndef roundup
#define roundup(x, y)	((((x)+((y)-1))/(y))*(y))
#endif

#ifndef NFDBITS
#define NFDBITS	        (8 * sizeof(fd_mask))
#endif

#ifndef FD_SETSIZE
#define FD_SETSIZE      1024
#endif

#define EZ_FDMSIZE     howmany(FD_SETSIZE, NFDBITS)
#define EZ_FDELT(d)    ((d)/(NFDBITS))
#define EZ_FDMASK(d)   (1UL << ((d) % NFDBITS))

/*****************************************************************/
#define EZ_NONE_MASK            0L
#define EZ_READABLE_MASK        (1L<<0)
#define EZ_WRITABLE_MASK        (1L<<1)
#define EZ_EXCEPTION_MASK       (1L<<2)
/*****************************************************************/

typedef struct _EZ_InputId_ {
  int                 fd;                   /* file descriptor  */
  int                 setMask;              /* mask, what to watch for */
  int                 mask;                 /* checked mask */
  EZ_InputCallback    callback;             /* only one callback */
  void                *data;
  int                 inAction;             /* flag */
  int                 deleted;
  struct _EZ_InputId_ *next;  
  struct _EZ_InputId_ *prev;  
} EZ_InputId;
/*****************************************************************/
#endif
