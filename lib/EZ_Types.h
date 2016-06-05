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
 */

/*****************************************************************************************/
typedef  void (*EZ_CallBack)      MY_ANSIARGS((void *object, void *data));
typedef  void (*EZ_Callback)      MY_ANSIARGS((void *object, void *data));
typedef  void (*EZ_InputCallback) MY_ANSIARGS((void *object, void *data, int fd, int mask));
typedef  void (*EZ_InputCallBack) MY_ANSIARGS((void *object, void *data, int fd, int mask));
typedef  void (*EZ_EventHandler)  MY_ANSIARGS((void *object, void *data, int etype, XEvent *event));

typedef  int  (*EZ_DnDEncoder)    MY_ANSIARGS((void *object, void *data, char **message,int *length, int *needFree));
typedef  int  (*EZ_DnDDecoder)    MY_ANSIARGS((void *object, void *data, char *message,int length));

typedef  void (*EZ_WorkAreaGeometryManager)  MY_ANSIARGS((void *GMData, EZ_Item **data, int nitems, 
							  int Rx,int Ry,int Rw, int Rh, int xsp, int ysp,
							  int *w_ret, int *h_ret));
/******************************************************************************************/
