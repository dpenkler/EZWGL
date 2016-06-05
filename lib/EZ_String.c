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
/**************************************************************
 *
 *  Render strings.
 */
#define _EZ_STRING_C_

#include "EZ_Widget.h"
#include "EZ_VectFont.h"
#include "EZ_Render.h"

/**************************************************************
 *
 *  Functions implemented in this file:
 */

void         EZ_SetVectorFont MY_ANSIARGS((int fid));
void         EZ_SetBitmapFont  MY_ANSIARGS((char *fname));
void         EZ_CMove2f  MY_ANSIARGS((float x, float y)); 
void         EZ_CMove3f MY_ANSIARGS((float x, float y, float z)); 
void         EZ_CMove3v MY_ANSIARGS((float *xyz)); 
void         EZ_CMoveScreen2f  MY_ANSIARGS((float x, float y)); 
void         EZ_CMoveScreen3f MY_ANSIARGS((float x, float y, float z)); 
void         EZ_CMoveScreen3fv MY_ANSIARGS((float *xyz)); 
void         EZ_DrawString MY_ANSIARGS((int font, char *str)); 

/**************************************************************/

#define DRAW_STRING_WORLD  0
#define DRAW_STRING_SCREEN 1

static float  EZ_CurTxtPos[4];
static int    EZ_CurTxtPosScrn[3];
static int    draw_string_type = DRAW_STRING_WORLD;

/************************************************************
 *
 *  Local functions
 */
static void   EZ_DrawStringVFont MY_ANSIARGS((char *str));
static void   EZ_DrawStringBFont MY_ANSIARGS((char *str));
static void   EZ_DrawStringVFont_Work MY_ANSIARGS((char *str, int x, int y, int z));
static void   EZ_DrawStringBFont_Work MY_ANSIARGS((char *str, int x, int y, int z));

/**************************************************************
 *
 *  Render string
 */
void EZ_DrawString(font,str)
     int font;
     char *str;
{
  if(EZ_InCompilingMode != 0)
    {
      char *tmp;
      int  length;
      int  size = (64*sizeof(EZ_Token)/sizeof(char)) - 4;
      EZ_Token *t = AllocateNTokens(66);
      t->i = TOKEN_DRAW_STRING;
      t++;
      t->i = font;
      t++;  
      t->ptr = t+1;
      tmp = (char *)(t->ptr);
      length = strlen(str);
      if(length > size) length = size;
      (void) strncpy(tmp,str,length);
      tmp[length] = '\0';
    }
  if(EZ_InExecutionMode != 0)
    {
      if(font == EZ_VECTOR_FONT)
	EZ_DrawStringVFont(str);
      else if(font == EZ_BITMAP_FONT)
	EZ_DrawStringBFont(str);
    }
}

/**************************************************************
 *
 * set vector font
 */
void EZ_SetVectorFont(font)
     int font;
{
  if(font == 0)   EZ_CurrentFont = EZ_SMALL_FONT;
  else  EZ_CurrentFont = EZ_LARGE_FONT;
}
  
/**************************************************************
 *
 * set the current text position, in modeling space.
 */
void EZ_CMove2f(x,y) 
     float x,y;
{
  EZ_CMove3f(x,y,0.0);
}

void EZ_CMove3f(x,y,z)
     float x,y,z;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(4);
      t->i = TOKEN_CMOVE;
      t[1].f = x;
      t[2].f = y;
      t[3].f = z;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_CurTxtPos[0] = x;
      EZ_CurTxtPos[1] = y;
      EZ_CurTxtPos[2] = z;
      draw_string_type = DRAW_STRING_WORLD;
    }
}

void EZ_CMove3fv(x)
     float *x;
{
  EZ_CMove3f(x[0],x[1],x[2]);
}

/**************************************************************
 *
 * set the current text position, in normalized clipping space.
 * The normalized screen is [-1,1]x[-1,1]x[-1,1].
 */
void EZ_CMoveScreen3fv(x)
     float *x;
{
  EZ_CMoveScreen3f(x[0],x[1],x[2]);
}
void EZ_CMoveScreen2f(x,y)
     float x,y;
{
  EZ_CMoveScreen3f(x,y,0.0);
}

void EZ_CMoveScreen3f(x,y,z)
     float x,y,z;
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(4);
      t->i = TOKEN_CMOVE_SCREEN;
      t[1].f = x;
      t[2].f = y;
      t[3].f = z;
    }
  if(EZ_InExecutionMode != 0)
    {
      float tmpf[4];
  
      tmpf[0] = (x <-1.0 ? -1.0: x > 1.0? 1.0: x);
      tmpf[1] = (y <-1.0 ? -1.0: y > 1.0? 1.0: y);
      tmpf[2] = (z <-1.0 ? -1.0: z > 1.0? 1.0: z);
      tmpf[3] = 1.0;

      TRANSFORM_2_SCREEN(tmpf, EZ_CurTxtPosScrn[0],EZ_CurTxtPosScrn[1],EZ_CurTxtPosScrn[2]);
      draw_string_type = DRAW_STRING_SCREEN;
    }
}

/**************************************************************
 *
 *  Render string using vector font.
 */
static void EZ_DrawStringVFont(str)
     char  *str;
{
  if(draw_string_type == DRAW_STRING_WORLD)
    {
      float           hcoor[4];
      int             xx,yy,zz;

      /*
       * first, transform the current text position to
       * screen space.
       */
      TRANSFORM_VERTEX(hcoor,EZ_CurTxtPos);
      TRANSFORM_2_SCREEN(hcoor, xx, yy, zz);

      EZ_DrawStringVFont_Work(str, xx,yy, zz);
    }
  else
    EZ_DrawStringVFont_Work(str, EZ_CurTxtPosScrn[0],EZ_CurTxtPosScrn[1],EZ_CurTxtPosScrn[2]);
}

/**************************************************************
 *
 *  Render string. Do Text cliping.
 */

static void EZ_DrawStringVFont_Work(str, sx,sy,sz)
     char  *str;
     int   sx,sy, sz;
{
  register int    i, j, k;
  int             ox, oy, nx;
  int             length, chwidth, chheight;
  char            *chbgn, *chend;
  int             WindowMinCoorY = 0, WindowMinCoorX = 0;
  int             WindowMaxCoorX = EZ_WindowWidth - 1;
  int             WindowMaxCoorY = EZ_WindowHeight -1;
  
  chwidth =  EZ_TextWidth  << EZ_CurrentFont;
  chheight = EZ_TextHeight << EZ_CurrentFont;

  oy = sy;

  /*--------------------------------
   * clip in the vertical direction
   *-------------------------------*/
  if(oy > WindowMaxCoorY  || oy < WindowMinCoorY) return;

  if(oy <= WindowMinCoorY + chheight)
    oy  += (chheight + 1);

  if(oy > WindowMaxCoorY) return;

  /*----------------------------------
   * clip in the horizontal direction
   *---------------------------------*/
  length = strlen(str);
  chbgn = str; 

  ox = sx;
  if((ox >= WindowMaxCoorX)  ||
     (ox + length * chwidth <= WindowMinCoorX))  return;
  
  while(ox < WindowMinCoorX && length > 0)
    {
      chbgn++; 
      ox += chwidth;
      length--;
    }
  if(length == 0) return;
  /*----------------------------------
   * select the substring that is 
   * inside the viewport
   *---------------------------------*/  
  nx = ox + length * chwidth;
  while(nx > WindowMaxCoorX && length >= 0)
    {
      nx  -= chwidth;
      length--;
    }
  if(length == 0) return;
  chend = chbgn + length;
  
  /*----------------------
   * now render the text
   *---------------------*/
  {
    int (*npos)[3] = EZ_VBuf.screen;
    int (*rgba)[4] = EZ_VBuf.rgbaF;

    EZ_VBuf.rgba = EZ_VBuf.rgbaF;
    rgba[0][0] = EZ_CurrentColorI[0];
    rgba[0][1] = EZ_CurrentColorI[1];
    rgba[0][2] = EZ_CurrentColorI[2];

    rgba[1][0] = EZ_CurrentColorI[0];
    rgba[1][1] = EZ_CurrentColorI[1];
    rgba[1][2] = EZ_CurrentColorI[2];

    npos[0][0] = ox;
    npos[0][1] = oy;
    npos[0][2] = npos[1][2] = sz;
    
    
    for(str = chbgn; str != chend; str++)
      {
	k = (int) (*str);
	if(EZ_VectFontTable[k][0][0]) 
	  {
	    i = 0;
	    while( (j = EZ_VectFontTable[k][i][0]) )
	      {
		switch (j) 
		  {
		  case 3:
		    npos[0][0] += 
		      (EZ_CurrentFont +1)* EZ_VectFontTable[k][i][1];
		    npos[0][1] -= 
		      (EZ_CurrentFont +1)* EZ_VectFontTable[k][i][2];
		    break;
		  case 2:
		    npos[1][0] = npos[0][0] +
		      (EZ_CurrentFont +1)* EZ_VectFontTable[k][i][1];
		    npos[1][1] = npos[0][1] -
		      (EZ_CurrentFont +1)* EZ_VectFontTable[k][i][2];
		    EZ_CURRENT_LINE_FUNC(0, 1);
		    npos[0][0] = npos[1][0]; 
		    npos[0][1] = npos[1][1];
		    break;
		  default:
		    break;
		  }
		i++;
	      }
	  }
      }
  }
}

/********************************************************************
 *
 *  Set the current bitmap font.
 */
void EZ_SetBitmapFont(name)
     char *name;
{
  EZ_BitmapFont = EZ_GetFontFromName(name);
  XSetFont(EZ_Display,EZ_MainGC,EZ_BitmapFont->fid);
}
/********************************************************************
 *
 * Render string using bitmap font
 */
static void  EZ_DrawStringBFont(str)
     char *str;
{
  if(EZ_DoubleBufferSet == EZ_XIMAGE && EZ_FrontBufferSet == 0)
    EZ_DrawStringVFont(str);
  else
    {
      unsigned long pixel_value;

      if(EZ_ColorMode == EZ_CMAP_MODE)
	{
	  int ir = EZ_CurrentColorI[0] >> COLOR_SHIFT_BITS;
	  pixel_value = EZ_ColorArray[ir];
	}
      else
	{
	  int r = EZ_CurrentColorI[0] >> COLOR_SHIFT_BITS;
	  int g = EZ_CurrentColorI[1] >> COLOR_SHIFT_BITS;
	  int b = EZ_CurrentColorI[2] >> COLOR_SHIFT_BITS;
	  if(EZ_Depth <= 8)
	    {
	      EZ_DitherRGB(pixel_value,4,5, r,g,b);
	    }
	  else
	    {
	      pixel_value = RGB2PixelLookUp(r,g,b);
	    }
	}
      XSetForeground(EZ_Display,EZ_MainGC,pixel_value);

      if(draw_string_type == DRAW_STRING_WORLD)
	{
	  float           hcoor[4], *fptr;
	  int             xxx,yyy,zzz;
	  /*
	   * first, transform the current text position to
	   * screen space.
	   */
	  fptr =  EZ_CurTxtPos;
	  TRANSFORM_VERTEX(hcoor,fptr);
	  TRANSFORM_2_SCREEN(hcoor, xxx,yyy,zzz);

	  EZ_DrawStringBFont_Work(str, xxx,yyy,zzz);
	}
      else
	EZ_DrawStringBFont_Work(str, EZ_CurTxtPosScrn[0],EZ_CurTxtPosScrn[1],EZ_CurTxtPosScrn[2]);
    }
}


static void EZ_DrawStringBFont_Work(str, sx,sy,sz)
     char  *str;
     int   sx,sy,sz;
{
  int  descent,ascent, top, bottom, i;  
  int  nx, ox, oy, length,tlength;
  int  WindowMinCoorY = 1, WindowMinCoorX = 1;
  int  WindowMaxCoorX = EZ_WindowWidth - 1;
  int  WindowMaxCoorY = EZ_WindowHeight -1;
  char *chbgn;

  oy = sy;
  descent = EZ_BitmapFont->descent;
  bottom = oy + descent;
  ascent  = EZ_BitmapFont->ascent;
  top = oy - ascent;
  
  /*--------------------------------
   * clip in the vertical direction
   *-------------------------------*/
  if(bottom >= WindowMaxCoorY  || top <= WindowMinCoorY) return;

  /*----------------------------------
   * clip in the horizontal direction
   *---------------------------------*/
  length = strlen(str);
  tlength = XTextWidth(EZ_BitmapFont,str, length);
  
  ox = sx;
  if((ox >= WindowMaxCoorX) || (ox + tlength  <= WindowMinCoorX))  return;

  /*--------------------------------------
   * figure out the visible piece of str
   *-------------------------------------*/
  chbgn = str; 
  
  i = 1;
  while(ox <= WindowMinCoorX && length > 0)
    {
      chbgn++; 
      ox += XTextWidth(EZ_BitmapFont,str, i);
      i++;
      length--;
    }
  if(length == 0) return;

  str = chbgn;
  tlength = XTextWidth(EZ_BitmapFont,str,length);
  nx = ox + tlength;
  
  while(nx > WindowMaxCoorX && length > 0)
    {
      length--;
      if(length > 0)
	nx = ox + XTextWidth(EZ_BitmapFont,str,length);
    }
  if(length <= 0) return;

  XDrawString(EZ_Display,EZ_Drawable,EZ_MainGC, ox, oy, str, length);
}
/********************************************************************/
#undef _EZ_STRING_C_
