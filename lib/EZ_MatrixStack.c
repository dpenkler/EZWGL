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
/****************************************************************
 *
 *   Operations on the matrix stack.
 *
 */
#define _EZ_MATRIX_STACK_C_

#include "EZ_Widget.h"

/***************************************************************
 *
 *  Functions implemented in this file
 */
void        EZ_MatrixMode MY_ANSIARGS((int mode));
void        EZ_PushMatrix MY_ANSIARGS((void));
void        EZ_PopMatrix MY_ANSIARGS((void));
void        EZ_LoadMatrix MY_ANSIARGS((EZ_Matrix mtx));
void        EZ_MultMatrix MY_ANSIARGS((EZ_Matrix mtx));
void        EZ_LoadIdentity MY_ANSIARGS((void));
void        EZ_GetMatrix MY_ANSIARGS((EZ_Matrix mtx));
void        EZ_FreeMatrixStacks MY_ANSIARGS((void));
void        EZ_FreeMatrixStackFreeEntries MY_ANSIARGS((void));

/*****************************************************************
 *
 * Local variables
 */

static EZ_MatrixStack *MStackFree = (EZ_MatrixStack *)NULL;
  
/*****************************************************************
 *
 *  Set the current matrix mode
 */

void  EZ_MatrixMode(mode)
     int mode;
{
  if(mode != EZ_PROJECTION && mode != EZ_MODELVIEW && mode != EZ_TEXTURE)
    {
      EZ_GLError("EZ_MatrixMode", "Invalid Value");
      return;
    }
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(2);
      t->i = TOKEN_MATRIX_MODE;
      t++;
      t->i = mode;
    }
  if(EZ_InExecutionMode != 0)
    {  
      EZ_MatrixModeMM = mode;
    }
}
/*****************************************************************
 *
 * PushMatrix 
 */

void EZ_PushMatrix()
{
  EZ_MatrixStack *tmp, *top;

  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(1);
      t->i = TOKEN_PUSH_MATRIX;
    }
  if(EZ_InExecutionMode != 0)
    {
      if(MStackFree == (EZ_MatrixStack *)NULL) 
	{
	  tmp = (EZ_MatrixStack *)my_malloc(sizeof(EZ_MatrixStack), _MATRIX_STACK_ENTRY_);
	  if(!tmp) EZ_OutOfMemory("EZ_PushMatrix");
	}
      else
	{
	  tmp = MStackFree;
	  MStackFree = tmp->prev;
	}
      
      top = EZ_CurrentMStack;
      tmp->prev = top;
      EZ_CopyMatrix(tmp->m, top->m);
      if(EZ_MatrixModeMM == EZ_MODELVIEW) EZ_CopyMatrix(tmp->itm, top->itm);
      EZ_CurrentMStack = tmp;
    }
}
/************************************************************************
 *
 * PopMatrix
 *
 */
void EZ_PopMatrix()
{
  EZ_MatrixStack *tmp;

  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(1);
      t->i = TOKEN_POP_MATRIX;
    }
  if(EZ_InExecutionMode != 0)
    {
      tmp = EZ_CurrentMStack;
      EZ_CurrentMStack = EZ_CurrentMStack->prev;

      if(EZ_MatrixModeMM != EZ_TEXTURE)
	EZ_MatrixValid = 0;

      tmp->prev = MStackFree;
      MStackFree = tmp;
    }
}

/************************************************************************
 *
 * Load m to MStack.
 */
void EZ_LoadMatrix(m)
     EZ_Matrix  m;
{
  register int	i, j;
  EZ_Matrix tmp;

  if(EZ_InCompilingMode != 0)
    {
      float *fs,*ft;
      int   i;
      EZ_Token *t = AllocateNTokens(17);

      t->i = TOKEN_LOAD_MATRIX;
      t++;
      fs = (float *)m;
      ft = (float *)t;
      for(i = 0; i < 16; i++)
	*ft++ = *fs++;
    }
  if(EZ_InExecutionMode != 0)
    {
      for(i = 0; i < 4; i++)
	for(j=0; j < 4; j++)
	  (EZ_CurrentMStack->m)[i][j] = m[i][j];

      if(EZ_MatrixModeMM == EZ_MODELVIEW)
	{
	  EZ_InvertMatrix4_(tmp, m, 1);
	  for (i = 0; i < 4; i++) 
	    for(j = 0; j < 4; j++)
	      (EZ_CurrentMStack->itm)[i][j] = tmp[j][i];
	}
      if(EZ_MatrixModeMM != EZ_TEXTURE)
	EZ_MatrixValid = 0;
    }
}

/************************************************************************
 *
 *  multiply to the top of MStack,
 */
void EZ_MultMatrix(m)
     EZ_Matrix m;
{
  if(EZ_InCompilingMode != 0)
    {
      float *fs,*ft;
      int   i;
      EZ_Token *t = AllocateNTokens(17);

      t->i = TOKEN_MULT_MATRIX;
      t++;
      fs = (float *)m;
      ft = (float *)t;
      for(i = 0; i < 16; i++)
	*ft++ = *fs++;
    }
  if(EZ_InExecutionMode != 0)
    {
      EZ_Matrix tmp;

      EZ_CopyMatrix(tmp, EZ_CurrentMStack->m);  
      EZ_MultiplyMatrix(EZ_CurrentMStack->m, m, tmp);
      if(EZ_MatrixModeMM == EZ_MODELVIEW)  
	{
	  EZ_Matrix im;
	  EZ_InvertMatrix4_(im, m, 1);
	  EZ_CopyMatrix(tmp, EZ_CurrentMStack->itm);  
	  EZ_MultiplyMatrix(EZ_CurrentMStack->itm, im, tmp);
	}
      if(EZ_MatrixModeMM != EZ_TEXTURE)
	EZ_MatrixValid = 0;  
    }
}
/************************************************************************
 *
 *  Load the identity matrix onto the current Matrix stack.
 */

void  EZ_LoadIdentity()
{
  if(EZ_InCompilingMode != 0)
    {
      EZ_Token *t = AllocateNTokens(1);

      t->i = TOKEN_LOAD_IDENTITY;
    }
  if(EZ_InExecutionMode != 0)
    {
      register int i, j;

      for(i = 0; i < 4; i++)
	for(j=0; j < 4; j++)
	  (EZ_CurrentMStack->m)[i][j] = 0.0;
      (EZ_CurrentMStack->m)[0][0] = (EZ_CurrentMStack->m)[1][1] = 
	(EZ_CurrentMStack->m)[2][2] = (EZ_CurrentMStack->m)[3][3] = 1.0;

      if(EZ_MatrixModeMM == EZ_MODELVIEW)
	{
	  for (i = 0; i < 4; i++) 
	    for(j = 0; j < 4; j++)
	      (EZ_CurrentMStack->itm)[i][j] = 0.0;
	  (EZ_CurrentMStack->itm)[0][0] = (EZ_CurrentMStack->itm)[1][1] = 
	    (EZ_CurrentMStack->itm)[2][2] = (EZ_CurrentMStack->itm)[3][3] = 1.0;
	}
      if(EZ_MatrixModeMM != EZ_TEXTURE)
	EZ_MatrixValid = 0;
    }
}

/*****************************************************************
 *
 *   GetMatrix 
 */

void EZ_GetMatrix(m)
     EZ_Matrix m;
{
  EZ_MatrixStack *top;

  top = EZ_CurrentMStack;
  EZ_CopyMatrix(m, top->m);
}

/*****************************************************************
 *
 *  Free the memory allocated for Matrix stacks.
 */

void EZ_FreeMatrixStacks()
{
  EZ_MatrixStack *tmp, *tmp1;

  /*
   * the projection stack.
   */
  tmp = (EZ_MStack)[EZ_PROJECTION];
  while(tmp != (EZ_MatrixStack *)NULL)
    {
      tmp1 = tmp->prev;
      (void)my_free((char*) tmp);
      tmp = tmp1;
    }  
  (EZ_MStack)[EZ_PROJECTION] = (EZ_MatrixStack *)NULL;
  /*
   * the modeview stack.
   */
  tmp = (EZ_MStack)[EZ_MODELVIEW];
  while(tmp != (EZ_MatrixStack *)NULL)
    {
      tmp1 = tmp->prev;
      (void)my_free((char*) tmp);
      tmp = tmp1;
    }  
  (EZ_MStack)[EZ_MODELVIEW] = (EZ_MatrixStack *)NULL;

  /*
   * the texture stack.
   */
  tmp = (EZ_MStack)[EZ_TEXTURE];
  while(tmp != (EZ_MatrixStack *)NULL)
    {
      tmp1 = tmp->prev;
      (void)my_free((char*) tmp);
      tmp = tmp1;
    }  
  (EZ_MStack)[EZ_TEXTURE] = (EZ_MatrixStack *)NULL;
}


void EZ_FreeMatrixStackFreeEntries()  
{
  /*
   * the free entries.
   */
  EZ_MatrixStack *tmp;

  while(MStackFree != (EZ_MatrixStack *)NULL)
    {
      tmp = MStackFree->prev;
      (void)my_free((char*) MStackFree);
      MStackFree = tmp;
    }
  MStackFree = (EZ_MatrixStack *)NULL;
}
/************************************************************************/


#undef _EZ_MATRIX_STACK_C_
