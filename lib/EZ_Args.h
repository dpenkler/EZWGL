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
/*****************************************************************/

/****** var args *****/

#if defined(__STDC__) || defined(HAVE_STDARG_H)
#include <stdarg.h>
#define MY_VARARGS(type, name) (type name, ...)
#define MY_VARARGS_D(type, name) (type name, ...)
#define MY_VARARGS_DECL    /*nothing*/
#define MY_VARARGS_START(type, name, list) (va_start(list, name), name)
#else
#include <varargs.h>
#define MY_VARARGS(type, name) (va_alist)
#define MY_VARARGS_D(type, name) ()
#define MY_VARARGS_DECL    va_dcl
#define MY_VARARGS_START(type,name,list)  (va_start(list), va_arg(list, type))

#endif


/****** ansi args ******/
#undef MY_ANSIARGS
#undef CONST
#if  defined (__cplusplus) || defined(USE_PROTOTYPE) || (defined(__STDC__) && !defined(NO_PROTOTYPE))
#define MY_ANSIARGS(x)   x
#define CONST const
#else
#define MY_ANSIARGS(x)   ()
#define CONST 
#endif

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif
/*****************************************************************/
