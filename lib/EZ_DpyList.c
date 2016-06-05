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
 *   Display list
 */
#define _EZ_DPY_LIST_C_
#include   "EZ_Widget.h"

/**************************************************************
 *
 *  Functions implemented in this file:
 */
void          EZ_CallList MY_ANSIARGS((int list));
void          EZ_CallLists MY_ANSIARGS((int start, int range));
int           EZ_GenLists MY_ANSIARGS((int range));
void          EZ_DeleteList MY_ANSIARGS((int list));
void          EZ_DeleteLists  MY_ANSIARGS((int start, int range));
void          EZ_NewList  MY_ANSIARGS((int lnum, int mode));
void          EZ_EndList  MY_ANSIARGS((void));
int           EZ_IsList  MY_ANSIARGS((int lnum));

EZ_Token      *AllocateNTokens  MY_ANSIARGS((int num));

/**************************************************************/
static void   EZ_GenOneList MY_ANSIARGS((int num));
static void   EZ_ExecuteList MY_ANSIARGS((int num));
static void   EZ_DeleteListContents MY_ANSIARGS((EZ_DpyList *lstptr));
/**************************************************************/

int EZ_GenLists(range)
     int range;
{
  if(EZ_InsideBeginEnd != 0)
    {
      EZ_GLError("EZ_GenList","Operation not allowed inside Begin/End");
      return(0);
    }
  if(range <= 0) return (0);
  {
    int        i,j,start, size;
    EZ_DpyList *tmp = EZ_DisplayLists;

    size = range + 17;
    if(tmp == (EZ_DpyList *)NULL) /* must be the first time */
      {
	tmp = EZ_DisplayLists = (EZ_DpyList *) my_malloc( (unsigned) size
						      * sizeof(EZ_DpyList), _DPY_LIST_);
	if(!tmp) EZ_OutOfMemory("EZ_GenLists");
	for(i = 1; i < size; i++)
	  {
	    tmp[i].in_use  = 0;
	    tmp[i].blockid = 0;
	    tmp[i].tokens = (EZ_Token *)NULL;
	  }
	EZ_NumberOfDpyLists = size;
      }
    /*
     * try to find contigous empty slots
     */
    for(i = 1, j = 0, start = 0; i < EZ_NumberOfDpyLists; i++) /* 0 is not used */
      {
	if(tmp[i].in_use == 0 && tmp[i].blockid == 0)
	  {
	    if(start == 0) start = i;
	    j++;
	    if(j == range) /* we have got them all */
	      {
		for(i = start; i < range+start; i++)
		  tmp[i].blockid = start;
		return(start);
	      }
	  }
	else 
	  {
	    start = 0;
	    j = 0;
	  }
      }
    /*
     * cannot find enough free slots 
     */
    EZ_DisplayLists = (EZ_DpyList *)my_realloc(EZ_DisplayLists,
					     (size + EZ_NumberOfDpyLists) *
					     sizeof(EZ_DpyList), _DPY_LIST_);
    if(!EZ_DisplayLists) EZ_OutOfMemory("EZ_GenLists");
    start = EZ_NumberOfDpyLists;
    EZ_NumberOfDpyLists += size;
    tmp = EZ_DisplayLists +  start;

    for(i = 0; i < range; i++)
      {
	tmp[i].in_use = 0; 
	tmp[i].blockid = start;
	tmp[i].tokens = (EZ_Token *)NULL;
      }
    for( ; i < size; i++)
      {
	tmp[i].in_use = 0; 
	tmp[i].blockid = 0;
	tmp[i].tokens = (EZ_Token *)NULL;
      }
    return(start);
  }
}

/**************************************************************/
EZ_Token *AllocateNTokens(n)
     int n;
{
  EZ_Token *t;
  int      size;

  size = n+2;
  if(EZ_AvailableTokens  < size)
    {
      size = (size < 1024? 1024: size);
      t = (EZ_Token *)my_malloc(size * sizeof(EZ_Token), _DPY_TOKEN_);
      if(!t) EZ_OutOfMemory("AllocateNTokens");
      EZ_TokenPtr->token_type = TOKEN_SKIP;
      EZ_TokenPtr++;
      EZ_TokenPtr->ptr = (void *)t;
      EZ_TokenPtr = t + n;
      EZ_AvailableTokens = size - n;
    }
  else
    {
      t = EZ_TokenPtr;
      EZ_TokenPtr += n;
      EZ_AvailableTokens -= n;      
    }
  return(t);
}
/**************************************************************/
int EZ_IsList(list)
     int list;
{
  int k = list & 0xffff;
  return( k > 0 && k < EZ_NumberOfDpyLists);
}

/**************************************************************/
static void EZ_GenOneList(k)
     int k;
{
  int i, size;
  EZ_DpyList *tmp = EZ_DisplayLists;

  if(k >= EZ_NumberOfDpyLists)
    {
      size = k + 17;
      EZ_DisplayLists = (EZ_DpyList *)my_realloc(EZ_DisplayLists,
					       size * sizeof(EZ_DpyList), _DPY_LIST_);
      if(!EZ_DisplayLists) EZ_OutOfMemory("EZ_GenOneList");
      tmp = EZ_DisplayLists;
      for(i = EZ_NumberOfDpyLists; i < size; i++)
	{
	  tmp[i].in_use = 0;
	  tmp[i].blockid = 0;
	  tmp[i].tokens = (EZ_Token *)NULL;
	}
      EZ_NumberOfDpyLists = size;
    }
  tmp[k].in_use = 1;
  tmp[k].blockid = k;
}

/**************************************************************/

void EZ_NewList(listnum,mode)
     int listnum, mode;
{
  int list;

  if(EZ_InsideBeginEnd != 0)
    {
      EZ_GLError("EZ_NewList","Operation not allowed inside Begin/End");
      return;
    }
  if( EZ_InCompilingMode != 0)
    {
      EZ_GLError("EZ_NewList","Operation not allowed inside List Compile ");
      return;
    }
  if(listnum <= 0 || ( mode != EZ_COMPILE && mode != EZ_COMPILE_AND_EXECUTE))
    {
      EZ_GLError("EZ_NewList","Invalid list or mode");
      return;
    }
  list = listnum & 0xffff;
  EZ_GenOneList(list);

  EZ_InCompilingMode = 1;
  EZ_InExecutionMode = (mode == EZ_COMPILE? 0 : 1);
  EZ_CurrentList = EZ_DisplayLists + list;
  EZ_TokenPtr = EZ_CurrentTokenHead = (EZ_Token *)my_malloc(1024 * sizeof(EZ_Token),_DPY_TOKENS_);
  if(!EZ_TokenPtr) EZ_OutOfMemory("EZ_NewList");
  EZ_AvailableTokens = 1024;
}
/**************************************************************/
void EZ_EndList()
{
  EZ_Token *t;

  if(EZ_InCompilingMode == 0)
    {
      EZ_GLError("EZ_EndList", "Not in List Compiling mode");
      return;
    }

  t = AllocateNTokens(1);
  t->token_type = TOKEN_LAST_ONE;
  
  EZ_DeleteListContents(EZ_CurrentList);
  EZ_CurrentList->in_use  = 1;
  EZ_CurrentList->tokens  = EZ_CurrentTokenHead;
  EZ_InCompilingMode = 0;
  EZ_InExecutionMode = 1;
}

/**************************************************************/
void EZ_DeleteLists(start, range)
     int start, range;
{
  int i;

  if(start <= 0 || range <= 0)  return;

  for(i = start; i < start+range; i++)
    EZ_DeleteList(i);
}

void EZ_DeleteList(listnum)
     int listnum;
{
  int list = listnum & 0xffff;

  if(EZ_IsList(list))
    {
      EZ_DpyList *dl = EZ_DisplayLists + list;
      EZ_DeleteListContents(dl);
      dl->in_use = 0;
      dl->blockid = 0;
    }
}
/**************************************************************/
void EZ_CallLists(start, range)
     int start, range;
{
  int i;

  if(start <= 0 || range <= 0)  return;  

  for(i = start; i < start + range; i++)
    EZ_CallList(i);
}


void EZ_CallList(list)
     int list; 
{
  if(EZ_IsList(list) == 0) return;

  EZ_CallListDepth++;
  if(EZ_CallListDepth >= 64)
    {
      EZ_CallListDepth--;
      EZ_GLError("EZ_CallList","CallList depth exceeded");
      return;
    }
  if(EZ_InExecutionMode)	  
    {
      int compile = EZ_InCompilingMode;
      int execute = EZ_InExecutionMode;
      EZ_InExecutionMode = 1;   
      EZ_InCompilingMode = 0;
      EZ_ExecuteList(list);
      EZ_InCompilingMode = (char) compile;
      EZ_InExecutionMode = (char) execute;
    }
  if(EZ_InCompilingMode)
    {
      /*
       * we are currently compling a list
       */
      EZ_Token *t = AllocateNTokens(2);
      t->token_type = TOKEN_CALL_LIST;
      t++;
      t->i = list;
    }
  EZ_CallListDepth--;
}
/**************************************************************/
static void  EZ_ExecuteList(list)
     int list;
{
  EZ_Token *t = (EZ_DisplayLists[list].tokens);
  
  if(t == (EZ_Token *)NULL) return;

  while(t->token_type != TOKEN_LAST_ONE)
    {
      switch(t->token_type)
	{
	case TOKEN_BEGIN:
	  t++;
	  EZ_Begin(t->i);
	  t++;
	  break;
	case TOKEN_END:
	  t++;
	  EZ_End();
	  break;
	case TOKEN_VERTEX:
	  EZ_Vertex3f(t[1].f, t[2].f,t[3].f);
	  t += 4;
	  break;
	case  TOKEN_NORMAL:
	  EZ_Normal3f(t[1].f, t[2].f,t[3].f);
	  t += 4;
	  break;
	case TOKEN_COLOR:
	  EZ_Color4f(t[1].f, t[2].f,t[3].f,t[4].f);
	  t += 5;
	  break;
	case TOKEN_INDEX:
	  t++;
	  EZ_Index(t->i);
	  t++;
	  break;
	case TOKEN_SHADE_MODEL:
	  t++;
	  EZ_ShadeModel(t->i);
	  t++;
	  break;
	case TOKEN_POLYGON_MODE:
	  EZ_PolygonMode(t[1].i,t[2].i);
	  t += 3;
	  break;
	case TOKEN_COLOR_MATERIAL:
	  EZ_ColorMaterial(t[1].i, t[2].i);
	  t += 3;
	  break;
	case TOKEN_CULL_FACE:
	  t++;
	  EZ_CullFace(t->i);
	  t++;
	  break;
	case TOKEN_FRONT_FACE:
	  t++;
	  EZ_FrontFace(t->i);
	  t++;
	  break;
	case TOKEN_FOG:
	  {
	    float tmp[4];
	    tmp[0] = t[2].f;tmp[1] = t[3].f;tmp[2] = t[4].f;tmp[3] = t[5].f;
	    EZ_Fogfv(t[1].i,tmp);
	    t += 6;
	  }
	  break;
	case TOKEN_CLEAR_COLOR:
	  EZ_ClearColorf(t[1].f,t[2].f,t[3].f,t[4].f);
	  t += 5;
	  break;
	case TOKEN_CLEAR_INDEX:
	  t++;
	  EZ_ClearIndex(t->i);
	  t++;
	  break;
	case TOKEN_ENABLE:
	  t++;
	  EZ_Enable(t->i);
	  t++;
	  break;
	case  TOKEN_DISABLE:
	  t++;
	  EZ_Disable(t->i);
	  t++;
	  break;
	case TOKEN_CALL_LIST:
	  t++;
	  EZ_CallList(t->i);
	  t++;
	  break;
	case TOKEN_CLEAR:
	  t++;
	  EZ_Clear(t->i);
	  t++;
	  break;
	case TOKEN_MATERIAL:
	  {
	    float tmp[4];
	    tmp[0] = t[3].f;tmp[1] = t[4].f;tmp[2] = t[5].f;tmp[3] = t[6].f;  
	    EZ_Materialfv(t[1].i,t[2].i,tmp);
	    t += 7;
	  }
	  break;
	case TOKEN_LIGHT:
	  {
	    float tmp[4];
	    tmp[0] = t[3].f;tmp[1] = t[4].f;tmp[2] = t[5].f;tmp[3] = t[6].f;
	    EZ_Lightfv(t[1].i,t[2].i, tmp);
	    t += 7;
	  }
	  break;
	case TOKEN_LIGHT_MODEL:
	  {
	    float tmp[4];
	    tmp[0] = t[2].f;tmp[1] = t[3].f;tmp[2] = t[4].f;tmp[3] = t[5].f;
	    EZ_LightModelfv(t[1].i, tmp);
	    t += 6;
	  }
	  break;
	case TOKEN_MATRIX_MODE:
	  t++;
	  EZ_MatrixMode(t->i);
	  t++;
	  break;
	case TOKEN_MULT_MATRIX:
	  {
	    t++;
	    EZ_MultMatrix((void *)t);
	    t += 16;
	  }
	  break;
	case TOKEN_LOAD_MATRIX:
	  {
	    t++;
	    EZ_LoadMatrix((void *)t);
	    t+= 16;
	  }
	  break;
	case TOKEN_LOAD_IDENTITY:	  
	  t++;
	  EZ_LoadIdentity();
	  break;
	case TOKEN_PUSH_MATRIX:
	  EZ_PushMatrix();
	  t++;
	  break;
	case TOKEN_POP_MATRIX:
	  EZ_PopMatrix();
	  t++;
	  break;
	case TOKEN_SCALE:
	  EZ_Scale(t[1].f,t[2].f, t[3].f);
	  t += 4;
	  break;
	case TOKEN_TRANSLATE:
	  EZ_Translate(t[1].f,t[2].f, t[3].f);
	  t += 4;
	  break;

	case TOKEN_DRAW_BUFFER:
	  t++;
	  EZ_DrawBuffer(t->i);
	  t++;
	  break;
	case TOKEN_BIND_MATERIAL:
	  EZ_BindMaterial(t[1].i,t[2].ptr);
	  t += 3;
	  break;
	case TOKEN_BIND_LIGHT:
	  EZ_BindLightSrc(t[1].i,t[2].ptr);
	  t += 3;
	  break;
	case TOKEN_BIND_LIGHT_MODEL:
	  EZ_BindLightModel(t[1].ptr);
	  t += 2;
	  break;
	case TOKEN_TETRAHEDRON:
	  EZ_Tetrahedron(t[1].f,t[2].f,t[3].f,t[4].f);
	  t += 5;
	  break;
	case TOKEN_CUBE:
	  EZ_Cube(t[1].f,t[2].f,t[3].f,t[4].f);
	  t += 5;
	  break;
	case TOKEN_OCTAHEDRON:
	  EZ_Octahedron(t[1].f,t[2].f,t[3].f,t[4].f);
	  t += 5;
	  break;
	case TOKEN_DODECAHEDRON:
	  EZ_Dodecahedron(t[1].f,t[2].f,t[3].f,t[4].f);
	  t += 5;
	  break;
	case TOKEN_ICOSAHEDRON:
	  EZ_Icosahedron(t[1].f,t[2].f,t[3].f,t[4].f);
	  t += 5;
	  break;
	case TOKEN_SPHERE:
	  EZ_Sphere(t[1].i,t[2].i,t[3].f,t[4].f,t[5].f,t[6].f);
	  t += 7;
	  break;
	case TOKEN_TUBE:
	  EZ_Tube(t[1].ptr,t[2].i,t[3].i);
	  t += 4;
	  break;

	case TOKEN_CMOVE:
	  EZ_CMove3f(t[1].f,t[2].f,t[3].f);
	  t += 4;
	  break;
	case TOKEN_CMOVE_SCREEN:
	  EZ_CMoveScreen3f(t[1].f,t[2].f,t[3].f);
	  t += 4;
	  break;
	case TOKEN_DRAW_STRING:
	  EZ_DrawString(t[1].i, (char *)t[2].ptr);
	  t += 66;
	  break;
	case TOKEN_GET_IMAGE_XWINDOW:
	  EZ_GetGLImageXWindow(t[1].i, t[2].i, t[3].i, t[4].i, (EZ_GLImage **)t[5].ptr);
	  t += 7;
	  break;
	case TOKEN_GET_IMAGE_WINDOW:
	  EZ_GetGLImageWindow(t[1].i, t[2].i, t[3].i, t[4].i, (EZ_GLImage **)t[5].ptr);
	  t += 7;
	  break;
	case TOKEN_PUT_IMAGE:
	  EZ_PutGLImage((EZ_GLImage *)t[1].ptr, t[2].i, t[3].i, t[4].i, t[5].i,
			t[6].f, t[7].f, t[8].f);
	  t += 9;
	  break;
	case TOKEN_PUT_IMAGE_SCREEN:
	  EZ_PutGLImageScreen((EZ_GLImage *)t[1].ptr, t[2].i, t[3].i, t[4].i, t[5].i,
			      t[6].f, t[7].f, t[8].f);
	  t += 9;
	  break;
	case TOKEN_PUT_IMAGE_WINDOW:
	  EZ_PutGLImageWindow((EZ_GLImage *)t[1].ptr, t[2].i, t[3].i, t[4].i, t[5].i,
			      t[6].i, t[7].i);
	  t += 9;
	  break;
	case TOKEN_PUT_IMAGE_XWINDOW:
	  EZ_PutGLImageXWindow((EZ_GLImage *)t[1].ptr, t[2].i, t[3].i, t[4].i, t[5].i,
			      t[6].i, t[7].i);
	  t += 9;
	  break;
	case TOKEN_STYLE:
	  EZ_SetLineStyle(t[1].i, t[2].i);
	  t += 3;
	break;  
	case TOKEN_SKIP:
	  t++;
	  t = (EZ_Token *)t->ptr;
	  break;

	default:
	  t++;
	  EZ_GLError("EZ_CallList","Internal error");
	  break;
	}
    }
}


static void EZ_DeleteListContents(lptr)
     EZ_DpyList *lptr;
{
  EZ_Token *t, *current, *next;
  int      done = 0;
  if(lptr->tokens == (EZ_Token *)NULL)
    return;

  current = t = lptr->tokens;
  lptr->tokens = (EZ_Token *)NULL;
  while( done == 0)
    {
      switch(t->token_type)
	{
	case TOKEN_BEGIN:  	  t += 2; 	  break;
	case TOKEN_END: 	  t++;  	  break;
	case TOKEN_VERTEX:	  t += 4;	  break;
	case TOKEN_NORMAL:	  t += 4;	  break;
	case TOKEN_COLOR:	  t += 5;	  break;
	case TOKEN_INDEX:	  t += 2;	  break;
	case TOKEN_SHADE_MODEL:	  t += 2;	  break;
	case TOKEN_COLOR_MATERIAL: t += 3;	  break;
	case TOKEN_CULL_FACE:	  t += 2;	  break;
	case TOKEN_FRONT_FACE:	  t += 2;	  break;
	case TOKEN_FOG:	          t += 6;	  break;
	case TOKEN_CLEAR_COLOR:	  t += 5;	  break;
	case TOKEN_CLEAR_INDEX:	  t += 2;	  break;
	case TOKEN_ENABLE:	  t += 2;	  break;
	case TOKEN_DISABLE:	  t += 2;	  break;
	case TOKEN_CALL_LIST:	  t += 2;	  break;
	case TOKEN_CLEAR:	  t += 2;	  break;
	case TOKEN_MATERIAL:	  t += 7;	  break;
	case TOKEN_LIGHT:	  t += 7;	  break;
	case TOKEN_LIGHT_MODEL:	  t += 6;	  break;
	case TOKEN_MATRIX_MODE:	  t += 2;	  break;
	case TOKEN_MULT_MATRIX:	  t += 17;	  break;
	case TOKEN_PUSH_MATRIX:	  t++;       	  break;
	case TOKEN_POP_MATRIX:	  t++;  	  break;
	case TOKEN_LOAD_MATRIX:	  t+= 17;	  break;
	case TOKEN_LOAD_IDENTITY:  t++; 	  break;
	case TOKEN_SCALE:	  t += 4;	  break;
	case TOKEN_TRANSLATE:	  t += 4;	  break;
	case TOKEN_DRAW_BUFFER:	  t += 2;	  break;
	case TOKEN_BIND_MATERIAL:  t += 3;	  break;
	case TOKEN_BIND_LIGHT:	  t += 3;	  break;
	case TOKEN_BIND_LIGHT_MODEL:  t += 2;	  break;
	case TOKEN_TETRAHEDRON:	  t += 5;	  break;
	case TOKEN_CUBE:	  t += 5;         break;
	case TOKEN_OCTAHEDRON:	  t += 5;	  break;
	case TOKEN_DODECAHEDRON:  t += 5;         break;
	case TOKEN_ICOSAHEDRON:	  t += 5;	  break;
	case TOKEN_SPHERE:	  t += 7;         break;
	case TOKEN_TUBE:	  t += 4;	  break;
	case TOKEN_CMOVE:	  t += 4;	  break;
	case TOKEN_CMOVE_SCREEN:  t += 4;	  break;
	case TOKEN_POLYGON_MODE:  t += 3;	  break;	  
	case TOKEN_DRAW_STRING:	  t += 66;	  break;
	case TOKEN_GET_IMAGE_WINDOW:  t += 6;	  break;
	case TOKEN_GET_IMAGE_XWINDOW:  t += 6;	  break;
	case TOKEN_PUT_IMAGE:	  t += 9;	  break;
	case TOKEN_PUT_IMAGE_SCREEN:  t += 9;	  break;
	case TOKEN_PUT_IMAGE_WINDOW:  t += 9;	  break;
	case TOKEN_PUT_IMAGE_XWINDOW: t += 9;	  break;
	case TOKEN_STYLE:             t += 3;     break;  
	case TOKEN_SKIP:
	  t++;
	  next = (EZ_Token *)t->ptr;
	  (void)my_free((char *)current);
	  current = t = next;
	  break;

	case TOKEN_LAST_ONE:
	  (void)my_free((char *)current);
	  done = 1;
	  break;
	default:
	  t++;
	  break;
	}
    }
}
/*********************************************************************************/
#undef _EZ_DPY_LIST_C_
