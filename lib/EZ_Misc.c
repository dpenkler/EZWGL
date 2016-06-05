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
/********************************************************************
 *
 *   Misc. Functions.
 *  
 */
#define _EZ_MISC_C_

#include "EZ_Widget.h"

void *EZ_Malloc MY_ANSIARGS((int nbytes));
void *EZ_Realloc MY_ANSIARGS((void *ptr, int nbytes));
char *EZ_AllocCopyString    MY_ANSIARGS((char *str));
char *EZ_ReallocCopyString  MY_ANSIARGS((char *ptr, char *str));
void  EZ_Free MY_ANSIARGS((void *ptr));
void  EZ_SitForALittleBit   MY_ANSIARGS((unsigned long length));
void  EZ_OutOfMemory        MY_ANSIARGS((char *msg));

void EZ_GLError MY_ANSIARGS((char *func, char *msg));
void EZ_GLFatal MY_ANSIARGS((char *func, char *msg));

/*****************************************************************
 *
 *  Allocate space and copy str to it.
 */

char *EZ_AllocCopyString(str)
     char *str;
{
  char *tmp;

  if(str == (char *)NULL)
    {
      tmp = (char *)my_malloc(4 * sizeof(char),_ALLOC_COPY_STR_);
      if(!tmp) EZ_OutOfMemory("EZ_AllocCopyString");
      tmp[0] = '\0'; tmp[1]='\0';  tmp[2]='\0'; tmp[3]='\0';
      return(tmp);
    }
  tmp = (char *)my_malloc( strlen(str) + 2, _ALLOC_COPY_STR_);
  if(!tmp) EZ_OutOfMemory("EZ_AllocCopyString");
  (void)strcpy(tmp, str);
  return(tmp);
}
void *EZ_Malloc(nbytes)
     int nbytes;
{
  if(nbytes > 0)
    return((void *)my_malloc( nbytes, _EZ_MALLOC_ ));
  return(NULL);
}
void *EZ_Realloc(ptr, nbytes)
     void *ptr; int nbytes;
{
  if(ptr == NULL) return(my_malloc(nbytes, _EZ_MALLOC_));
  else return(my_realloc(ptr, nbytes, _EZ_MALLOC_));
}
/*****************************************************************/
char *EZ_ReallocCopyString(ptr,str)
     char *ptr, *str;
{
  char *tmp;

  if(ptr == (char *)NULL)
    return(EZ_AllocCopyString(str));
  else
    {
      int len = strlen(str) + 2;
      tmp = (char *)my_realloc(ptr, len, _ALLOC_COPY_STR_);
      if(!tmp) EZ_OutOfMemory("EZ_ReallocCopyString");
      (void)strcpy(tmp,str);
      return(tmp);
    }
}
/*****************************************************************/
void EZ_Free(ptr)
     void *ptr;
{
  if(ptr) my_free(ptr);
}
/*****************************************************************/


void  EZ_SitForALittleBit(length)
     unsigned long length;
{
  struct timeval tmout;
  tmout.tv_usec = length % 1000000;
  tmout.tv_sec  = length / 1000000;
  (void) select(0, NULL, NULL, NULL, &tmout);
}

void EZ_OutOfMemory(msg) char *msg;
{
  (void)fprintf(stderr,"Out of memory in: %s\n",msg);
  EZ_Shutdown();
  exit(EZ_EXIT_ERROR);
}
/*****************************************************************/
void  EZ_GLError(fname, mesg)
     char *fname, *mesg;
{
  (void)fprintf(stderr, "In %s(), %s\n", fname, mesg);
}
     
void  EZ_GLFatal(fname, mesg)
     char *fname, *mesg;
{
  EZ_GLError(fname, mesg);
  exit(EZ_EXIT_ERROR);
}
/*****************************************************************/
#undef _EZ_MISC_C_
