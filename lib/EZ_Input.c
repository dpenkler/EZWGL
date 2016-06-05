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
#define EZ_ADD_INPUT
#define _EZ_INPUT_C_
/***********************************************************/
#include "EZ_Widget.h"
/***********************************************************/
void         EZ_InitializeInputIdList  MY_ANSIARGS((void));
void         EZ_DestroyInputIdList  MY_ANSIARGS((void));
void         *EZ_AddInput MY_ANSIARGS((int fd, int mask, 
				       EZ_InputCallBack callback,
				       void *data));
void         EZ_RemoveInput MY_ANSIARGS((void *id));
int          EZ_CheckAppInputs MY_ANSIARGS((unsigned long length));
/***********************************************************/
#ifdef EZ_ADD_INPUT
static EZ_InputId *findInputIdFromFd  MY_ANSIARGS((int fd));
/***********************************************************/
static EZ_InputId    AppInputIdListHead;
/***********************************************************/
void EZ_InitializeInputIdList()
{
  EZ_InputId *tmp = &AppInputIdListHead;
  tmp->prev = tmp;
  tmp->next = tmp;
}
void EZ_DestroyInputIdList()
{
  EZ_InputId *head = &AppInputIdListHead;
  EZ_InputId *tmp = head->next;
  while(tmp != head)
    {
      EZ_InputId *t = tmp->next;
      (void)my_free((char *)tmp);
      tmp = t;
    }
  head->next = head;
  head->prev = head;
}
/***********************************************************/
static EZ_InputId *findInputIdFromFd(fd)
     int fd;
{
  EZ_InputId *head = &AppInputIdListHead;
  EZ_InputId *tmp = head->next;

  while(tmp != head)
    {
      if(tmp->fd == fd) return(tmp);
      tmp = tmp->next;
    }
  return(NULL);
}

static EZ_InputId *findInputId(id)
     EZ_InputId *id;
{
  EZ_InputId *head = &AppInputIdListHead;
  EZ_InputId *tmp = head->next;

  while(tmp != head)
    {
      if(tmp == id) return(tmp);
      tmp = tmp->next;
    }
  return(NULL);
}

/***********************************************************/
void  *EZ_AddInput(fd, mask, callback, data)
     int fd;
     int mask;
     EZ_InputCallBack callback;
     void  *data;
{
  if(fd >= 0 && fd < FD_SETSIZE)
    {
      EZ_InputId *iid = findInputIdFromFd(fd);

      if(iid == NULL)
	{
	  EZ_InputId  *next, *head = &AppInputIdListHead;
	  
	  iid = my_malloc(sizeof(EZ_InputId), _INPUT_ID_);
	  if(iid == NULL)  EZ_OutOfMemory("EZ_AddInput");
	  next = head->next;
	  head->next = iid;
	  next->prev = iid;
	  iid->next = next;
	  iid->prev = head;
	  iid->inAction = 0;
	  iid->deleted = 0;
	}
      iid->fd = fd;
      iid->setMask = mask;
      iid->mask =  0;
      iid->callback = callback;
      iid->data = data;

      return((void *)iid);
    }
  return(NULL);
}
/***********************************************************/
void EZ_RemoveInput(ptr)
     void *ptr;
{
  EZ_InputId *id = (EZ_InputId *)ptr;
  if(id && id == findInputId(id))  id->deleted = 1;
}
/***********************************************************
 *
 *  Check whether or not any fd is ready. If there are
 *  any, invoke their callbacks. 
 *  Return value:
 *     -1 if there are no files to watch. otherwise,
 *        return the number of fds which are ready.   
 */

int  EZ_CheckAppInputs(length)
     unsigned long length;  /* time to wait */
{
  fd_mask     fd_masks[3 * EZ_FDMSIZE];
  EZ_InputId    *next, *tmpInputId, *head = &AppInputIdListHead;  
  int maxfd_1 = 0; 
  
  next = head->next;
  if(next != head)
    {
      memset(fd_masks, 0, (3* EZ_FDMSIZE)*sizeof(fd_mask));

      while(next != head)
	{
	  tmpInputId = next->next;

	  if(next->inAction == 0)
	    {
	      int fd = next->fd;
	      int setmask = next->setMask;
	      if(next->deleted == 1)  /* marked removed. Remove it now */
		{
		  EZ_InputId *front = next->prev;      
		  EZ_InputId *back = next->next;      
      
		  front->next = back;
		  back->prev = front;
		  (void) my_free( (char *)next);
		}
	      else
		{
		  int elt = EZ_FDELT(fd);    /* which elt in fd_masks */
		  int bit = EZ_FDMASK(fd);   /* which bit in **[elt]  */
	      
		  if(setmask & EZ_READABLE_MASK)
		    fd_masks[elt] |= bit;
		  if(setmask & EZ_WRITABLE_MASK)
		    fd_masks[elt+EZ_FDMSIZE] |= bit;
		  if(setmask & EZ_EXCEPTION_MASK)
		    fd_masks[elt + 2*EZ_FDMSIZE] |= bit;
		  
		  next->mask = 0;
		  if(maxfd_1 <= fd) maxfd_1 = fd + 1;
		}
	    }
	  next = tmpInputId;
	}
    }
  if(maxfd_1 > 0)  /* we have some fds, go wait */
    {
      int count;
      struct timeval tmout;
      tmout.tv_usec = length % 1000000;
      tmout.tv_sec  = length / 1000000;
      count = select(maxfd_1, (fd_set *) (&fd_masks[0]),
		     (fd_set *)(&fd_masks[EZ_FDMSIZE]),
		     (fd_set *)(&fd_masks[2*EZ_FDMSIZE]),
		     &tmout);
      if(count > 0)
	{
	  next = head->next;
	  while(next != head)
	    {
	      if(next->inAction == 0)
		{
		  int fd = next->fd;
		  int mask = 0;
		  int elt = EZ_FDELT(fd);    /* which elt in fd_masks */
		  int bit = EZ_FDMASK(fd);   /* which bit in **[elt]  */
		  
		  if(fd_masks[elt] & bit)
		    mask |= EZ_READABLE_MASK;
		  if(fd_masks[elt+EZ_FDMSIZE] & bit)
		    mask |= EZ_WRITABLE_MASK;
		  if(fd_masks[elt + 2*EZ_FDMSIZE] & bit)
		    mask |= EZ_EXCEPTION_MASK;		    

		  next->mask = (next->setMask) & mask;
		}
	      next = next->next;
	    }

	  /* invoke callbacks */
	  next = head->next;	  
	  while(next != head)
	    {
	      EZ_InputId *tmp = next->next;
	      if(next->inAction == 0)
		{
		  int mask = next->mask;
		  if(mask && next->callback)
		    {
		      next->inAction = 1;
		      next->mask = 0;
		      (next->callback)(next, next->data, next->fd, mask);
		      next->inAction = 0;
		    }
		}
	      next = tmp;
	    }
	}
      return(count);
    }
  return(-1);
}
#else
void  EZ_InitializeInputIdList() { }
void  EZ_DestroyInputIdList() { }
void  *EZ_AddInput(fd, mask, callback, data) 
     int fd, mask; EZ_InputCallBack callback; void *data;
{ return(NULL); }
void EZ_RemoveInput(id) void *id; { };
int EZ_CheckAppInputs(length) 
     unsigned long length;
{
  return(-1);
}
#endif
#undef _EZ_INPUT_C_  
/****************************************************************************************/	  
	  
	  
	    



