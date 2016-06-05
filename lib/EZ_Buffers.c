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
/*******************************************************************
 *
 *   Functions manipulating the back and the front buffers.
 */

#define _EZ_BUFFERS_C_
/*******************************************************************/
#include "EZ_Widget.h"

/*******************************************************************
 *
 * Functions implemented in this file:
 */

void                 EZ_LogicOp  MY_ANSIARGS((int op));
void                 EZ_DrawBuffer MY_ANSIARGS((int buf));
void                 EZ_SwapBuffers MY_ANSIARGS((void));
void                 EZ_Clear MY_ANSIARGS((int flag));
void                 EZ_ClearColorf MY_ANSIARGS((float r, float g, float b, float a));
void                 EZ_ClearIndex MY_ANSIARGS((int idx));
void                 EZ_SetBackBuffer MY_ANSIARGS((int buf));
void                 EZ_AutoSelectBackBuffer MY_ANSIARGS((void));
void                 EZ_SelectFastBackBuffer MY_ANSIARGS((void));
void                 EZ_SwapBuffers_XImage MY_ANSIARGS((void));
void                 EZ_SwapBuffers_Pixmap MY_ANSIARGS((void));

/****************************************************************
 *
 *   Set the logical operator
 */
void  EZ_LogicOp(op)
     int op;
{
  int xop;

  if(EZ_DoubleBufferSet == EZ_XIMAGE &&
     EZ_FrontBufferSet == 0)
    {
      EZ_GLError("EZ_LogicOp", "operation not supported for XImage backbuffer");
      return;
    }
  switch(op)
    {
    case EZ_CLEAR:        xop = GXclear;        break;
    case EZ_AND:          xop = GXand;          break;      
    case EZ_AND_REVERSE:  xop = GXandReverse;   break;
    case EZ_COPY:         xop = GXcopy;         break;
    case EZ_AND_INVERTED: xop = GXandInverted;  break;
    case EZ_NOOP:         xop = GXnoop;         break;      
    case EZ_XOR:          xop = GXxor;          break;   
    case EZ_OR:           xop = GXor;           break;    
    case EZ_NOR:          xop = GXnor;          break;     
    case EZ_EQUIV:        xop = GXequiv;        break; 
    case EZ_INVERT:       xop = GXinvert;       break;
    case EZ_OR_REVERSE:   xop = GXorReverse;    break;
    case EZ_COPY_INVERTED:xop = GXcopyInverted; break;
    case EZ_OR_INVERTED:  xop = GXorInverted;   break;
    case EZ_NAND:         xop = GXnand;         break;
    case EZ_SET:          xop = GXset;          break; 
    default:
      EZ_GLError("EZ_LogicOP", "Invalid logic op");
      return;
      break;
    }
  XSetFunction(EZ_Display,EZ_MainGC,xop);
}
    
/****************************************************************
 *
 *   Set the back buffer.
 */
void  EZ_SetBackBuffer(buf)
     int buf;
{
  if(EZ_InExecutionMode != 0)
    {
      if(buf == EZ_PIXMAP)
	{
	  EZ_DoubleBufferSet = EZ_PIXMAP;
	  EZ_GLConfig();
	}
      else if(buf == EZ_XIMAGE)
	{
	  EZ_DoubleBufferSet = EZ_XIMAGE;
	  EZ_GLConfig();
	}
      else
	EZ_GLError("EZ_SetBackBuffer", "Invalid Buffer");
    }
  else
    EZ_GLError("EZ_SetBackBuffer", "Operation not allowed in compiling mode");
}

void EZ_AutoSelectBackBuffer()
{
  if(EZ_InExecutionMode != 0)
    {
      char *bf = getenv("EZ_BACK_BUFFER");
      if(bf)
	{
	  if(bf[0] == 'p' || bf[0] == 'P')
	    EZ_SetBackBuffer(EZ_PIXMAP);
	  else if(bf[0] == 'x' || bf[0] == 'X')
	    EZ_SetBackBuffer(EZ_XIMAGE); 
	  else  EZ_SelectFastBackBuffer();
	}
      else EZ_SelectFastBackBuffer();
    }
  else
    EZ_GLError("EZ_AutoSelectBackBuffer",
	       "Operation not allowed in compiling mode");    
}

void EZ_SelectFastBackBuffer()
{
  EZ_SetBackBuffer(EZ_XIMAGE); 
  if(EZ_HasSHM == 0)
    EZ_SetBackBuffer(EZ_PIXMAP);
}

/****************************************************************
 *
 *  set the current draw buffer.
 */
void EZ_DrawBuffer(buf)
     int buf;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(2);
      t->i = TOKEN_DRAW_BUFFER;
      t++;
      t->i = buf;
    }
  if(EZ_InExecutionMode != 0)
    {
      if(buf == EZ_FRONT)
	{
	  if(EZ_FrontBufferSet == 0)
	    {
	      EZ_FrontBufferSet = 1;
	      EZ_Drawable = EZ_MainWindow;
	      EZ_SelectRenderFunctions(EZ_CurrentGWindow);
	    }
	}
      else if(buf == EZ_BACK)
	{
	  if(EZ_FrontBufferSet != 0)
	    {
	      EZ_FrontBufferSet = 0;
	      if(EZ_DoubleBufferSet != 0)
		{
		  if(EZ_DoubleBufferSet == EZ_PIXMAP)
		    EZ_Drawable = EZ_Pixmap3;
		  else
		    EZ_Drawable = EZ_MainWindow;
		  EZ_SelectRenderFunctions(EZ_CurrentGWindow);
		}
	      else
		{
		  EZ_DoubleBufferSet = EZ_PIXMAP;
		  EZ_GLConfig();
		  EZ_Drawable = EZ_Pixmap3;
		}
	    }
	}
      else
	EZ_GLError("EZ_DrawBuffer", "Invalid buffer");
    }
}

/****************************************************************
 *
 * Copy the back buffer to the front buffer.
 */
void EZ_SwapBuffers()
{
  if(EZ_InExecutionMode != 0)
    { 
      if(EZ_FrontBufferSet == 0)
	EZ_CURRENT_SWAPBUFFER_FUNC();
    }
}  
/*****************************************************************
 *
 *  Clear.
 */
void EZ_ClearColorf(r,g,b,a)
     float r,g,b,a;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(5);
      t->i = TOKEN_CLEAR_COLOR;
      t[1].f = r;
      t[2].f = g;
      t[3].f = b;
      t[4].f = a;
    }
  if(EZ_InExecutionMode != 0)
    {
      int *iptr = EZ_BackRGBA;
      iptr[0] = (int)(r * 255.5);
      iptr[1] = (int)(g * 255.5);
      iptr[2] = (int)(b * 255.5);
      iptr[3] = (int)(a * 255.5);
    }
}

void EZ_ClearIndex(idx)
     int idx;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(2);
      t->i = TOKEN_CLEAR_INDEX;
      t[1].i = idx;
    }
  if(EZ_InExecutionMode != 0)
    EZ_BackRGBA[0] = idx;
}

void EZ_Clear(flag)
     int flag;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(1);
      t->i = TOKEN_CLEAR;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_CURRENT_CLEAR_FUNC(flag);
    }
}

/*******************************************************************
 *
 *  copy the back XImage to the display window.
 */

void EZ_SwapBuffers_XImage()
{
#if defined(HAVE_XSHM) && defined(USE_XSHM)
  if(EZ_HasSHM != 0)
    XShmPutImage(EZ_Display, EZ_MainWindow, EZ_MainGC, EZ_Image3, 0,0, 
		 0, 0, 
		 (unsigned int)EZ_WindowWidth,
		 (unsigned int)EZ_WindowHeight,False); 
  else
#endif
    XPutImage(EZ_Display, EZ_MainWindow, EZ_MainGC, EZ_Image3, 0,0, 
	      0, 0,
	      (unsigned int)EZ_WindowWidth,
	      (unsigned int)EZ_WindowHeight);
  XSync(EZ_Display, 0);
}

/*******************************************************************
 *
 *  copy the back Pixmap to the display window;
 */
void EZ_SwapBuffers_Pixmap()
{
  XCopyArea(EZ_Display,
	    EZ_Pixmap3,
	    EZ_MainWindow,
	    EZ_MainGC,
	    0, 0,
	    EZ_WindowWidth, EZ_WindowHeight,
	    0, 0);
  XSync(EZ_Display, 0);
}

/**********************************************************************************/
#undef _EZ_BUFFERS_C_
