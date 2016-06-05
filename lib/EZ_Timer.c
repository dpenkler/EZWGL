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
 * Timers
 */
#define _EZ_TIMER_C_

#include "EZ_Widget.h"

/*******************************************************************
 *
 * Functions implemented in this file:
 */
void        EZ_InitializeTimers MY_ANSIARGS((void));
EZ_Timer    *EZ_CreateTimer MY_ANSIARGS((long sec, long usec,int repeat,
					 EZ_CallBack callback, void *pdata, int idata));
EZ_Timer    *EZ_ResetTimer MY_ANSIARGS((EZ_Timer *timer, long sec, long usec,int repeat,
					EZ_CallBack callback, void *pdata, int idata));

int         EZ_CheckRealTimers MY_ANSIARGS((long sec, long usec));
void        EZ_GetTimerState MY_ANSIARGS((EZ_Timer *timer, long *sec, long *usec,int *repeat));
void        EZ_CancelTimer MY_ANSIARGS((EZ_Timer *timer));
void        EZ_DestroyAllTimers  MY_ANSIARGS((void));
void        EZ_RestartTimer  MY_ANSIARGS((EZ_Timer *timer));
void        EZ_SetTimerPtrData  MY_ANSIARGS((EZ_Timer *timer, void *ptr));
void        EZ_SetTimerIntData  MY_ANSIARGS((EZ_Timer *timer,int data));
void        EZ_SetTimerClientData  MY_ANSIARGS((EZ_Timer *timer, void *ptr, int idata));      
void        EZ_SetTimerCallBack  MY_ANSIARGS((EZ_Timer *timer, EZ_CallBack callback, void *data));
void        *EZ_GetTimerPtrData      MY_ANSIARGS((EZ_Timer *timer));
int         EZ_GetTimerIntData MY_ANSIARGS((EZ_Timer *timer));
void        EZ_GetTimerClientData  MY_ANSIARGS((EZ_Timer *timer, void **ptr, int *idata));      
/*******************************************************************
 *
 * Local variables
 */
static EZ_Timer    RepeatingTimerList;
static EZ_Timer    UsualTimerList;
/*******************************************************************
 *
 * Local functions
 */
static EZ_Timer *find_the_timer  MY_ANSIARGS((EZ_Timer *timer));      
static void insert_timer  MY_ANSIARGS((EZ_Timer *timer, int repeat, long sec, long usec));      
/*******************************************************************
 *
 *  Initialize Timers
 */
void  EZ_InitializeTimers()
{
  EZ_Timer *tmp;

  tmp = &RepeatingTimerList;
  tmp->next = tmp;
  tmp->prev = tmp;

  tmp = &UsualTimerList;
  tmp->next = tmp;
  tmp->prev = tmp;
}
/*******************************************************************
 * 
 * destroy all timers. called only by EZ_Shutdown.
 */
void  EZ_DestroyAllTimers()
{
  EZ_Timer *head, *tmp, *chase;

  head = &RepeatingTimerList;
  tmp = head->next;
  while(tmp != head)
    {
      chase = tmp;
      tmp = tmp->next;
      (void)my_free( (char *)chase);  
    }
  head->next = head; head->prev = head; /* just incase */

  head = &UsualTimerList;
  tmp = head->next;
  while(tmp != head)
    {
      chase = tmp;
      tmp = tmp->next;
      (void)my_free( (char *)chase);  
    }
  head->next = head; head->prev = head; /* just incase */
}
/*******************************************************************
 *
 *  Create a Timer.
 */
extern void EZ_GetCurrentTime();

EZ_Timer  *EZ_CreateTimer(sec,usec,repeat,callback, pdata, idata)
     EZ_CallBack callback;
     long sec, usec;
     void      *pdata;
     int       repeat, idata;
{
  EZ_Timer *tmp;
  struct timeval ctv, *tv;
  long tsec, tusec;

  tv = &ctv;
  EZ_GetCurrentTime(tv);
  if(sec == 0 && usec == 0) return(NULL);
  
  /* allocate and initialize timer */
  if((tmp = (EZ_Timer *)my_malloc(sizeof(EZ_Timer),_TIMER_)) == NULL)
    EZ_OutOfMemory("EZ_CreateTimer");
  tsec = tv->tv_sec + sec;
  tusec = tv->tv_usec + usec;
  while(tusec >= 1000000) 
    {
      tusec -= 1000000;
      tsec  += 1;
    }
  tmp->TheTime.tv_sec = tsec;
  tmp->TheTime.tv_usec =tusec;
  tmp->sec = sec;
  tmp->usec = usec;
  (tmp->callback).callback = callback;
  (tmp->callback).data = pdata;
  (tmp->callback).internal = idata;
  tmp->repeat = repeat;
  tmp->inAction = 0;

  insert_timer(tmp, repeat, tsec, tusec);
  return(tmp);
}

static void insert_timer(tmp, repeat, tsec, tusec)
     EZ_Timer *tmp; int repeat;
     long tsec, tusec;
{
  /* insert tmp into a sorted link list */
 
  if(repeat)  /* repeating timers, put it in the end */
    {
      EZ_Timer *head, *tail;

      head = &RepeatingTimerList;
      tail = head->prev;

      tmp->next = head;
      tmp->prev = tail;
      tail->next = tmp;
      head->prev = tmp;
    }
  else  /* non-repeating timers, insert it into a sorted list */
    {
      EZ_Timer *head, *front, *chase;

      head = &UsualTimerList; 
      chase = head->next;
      while(chase != head)
	{
	  long s = chase->TheTime.tv_sec;
	  long us= chase->TheTime.tv_usec;
	  
	  if(s > tsec || (s = tsec && us >= tusec)) break;
	  chase = chase->next;
	}
      /* insert at the front of chase */
      front = chase->prev;
      
      tmp->next = chase;
      tmp->prev = front;
      front->next = tmp;
      chase->prev = tmp;
    }
}
/**********************************************************************
 *  
 *  Reset a timer if exists. Otherwise create a new timer.
 */
EZ_Timer  *EZ_ResetTimer(timer, sec,usec,repeat,callback, pdata, idata)
     EZ_Timer *timer;
     EZ_CallBack callback;
     long sec, usec;
     void      *pdata;
     int       repeat, idata;     
{
  if(find_the_timer(timer) == (EZ_Timer *)NULL)
    {
      return(EZ_CreateTimer(sec,usec,repeat,callback, pdata, idata));
    }
  else
    {
      EZ_Timer *front, *back;      
      struct timeval ctv, *tv;
      long tsec, tusec;
  
      if(sec == 0 && usec == 0)
	{
	  EZ_CancelTimer(timer);
	  return(NULL);
	}
      tv = &ctv;
      EZ_GetCurrentTime(tv);
      tsec = tv->tv_sec + sec;
      tusec = tv->tv_usec + usec;
      while(tusec >= 1000000) 
	{
	  tusec -= 1000000;
	  tsec  += 1;
	}
      /* remove timer from the timer list */
      front = timer->prev;
      back = timer->next;
      front->next = back;
      back->prev = front;

      /* reset timer */
      timer->TheTime.tv_sec = tsec;
      timer->TheTime.tv_usec =tusec;
      timer->sec = sec;
      timer->usec = usec;
      (timer->callback).callback = callback;
      (timer->callback).data = pdata;
      (timer->callback).internal = idata;
      timer->repeat = repeat;

      insert_timer(timer, repeat, tsec, tusec);
      return(timer);
    }
}
/**********************************************************************
 *  
 *  Restart a timer.
 */
void EZ_RestartTimer(timer)
     EZ_Timer *timer;
{
  if(find_the_timer(timer) == (EZ_Timer *)NULL)
    return;
  else
    {
      EZ_Timer *front, *back;      
      struct timeval ctv, *tv;
      long tsec, tusec;
      tv = &ctv;
      EZ_GetCurrentTime(tv);
      tsec = tv->tv_sec + timer->sec;
      tusec = tv->tv_usec + timer->usec;
      while(tusec >= 1000000) 
	{
	  tusec -= 1000000;
	  tsec  += 1;
	}
      /* remove timer from the timer list */
      front = timer->prev;
      back = timer->next;
      front->next = back;
      back->prev = front;

      /* reset timer */
      timer->TheTime.tv_sec = tsec;
      timer->TheTime.tv_usec =tusec;

      insert_timer(timer, timer->repeat, tsec, tusec);
    }
}
/**************************************************************
 *
 * Algorithm:
 * for repeating timers, check the list and fire the first timer
 * that has been expired. Move it to the end of the list. The order
 * is in general incorrect! 
 *
 * nonrepeating timers are sorted. Just find the first expried 
 * timer and fire it.
 */
int  EZ_CheckRealTimers(ts, tus)
     long ts,tus;
{
  EZ_Timer *chase, *head, *tail, *front, *back;
  int find = 0;

  /* repeating timers */
  head = &RepeatingTimerList;
  chase = head->next;
  while(chase != head)
    {
      long s = chase->TheTime.tv_sec;
      long us= chase->TheTime.tv_usec;

      if(s == 0 && us == 0)
	{
	  /* this timer has been cancelled */
	  front = chase->prev;
	  back = chase->next;
	  front->next = back;
	  back->prev = front;
	  (void)my_free( (char *)chase);
	  chase = back;
	  continue;
	}
      else if(s < ts || (s==ts && us <= tus)) /* fire it */
	{
	  if(chase->inAction == 0)
	    {
	      /* reset timer */
	      s  = ts + chase->sec;
	      us = tus + chase->usec;
	      while(us >= 1000000) 
		{
		  us -= 1000000;
		  s  += 1;
		}
	      chase->TheTime.tv_sec = s;
	      chase->TheTime.tv_usec = us;
	      
	      find++;

	      /* Need to block this timer from being
	       * invoked from its callback. The
	       * callback may invoke event dispatchers.
	       */
	      chase->inAction = 1;
	      if(chase->repeat > 0) chase->repeat--; 	  
	      /* fire the timer */
	      {
		EZ_CallBackStruct *f = &(chase->callback);
		if(f->callback)  (f->callback)(chase, f->data);
	      }
	      chase->inAction = 0; /* unblock */

	      /* now reset. 1. remove it from the queue */
	      front = chase->prev;
	      back = chase->next;
	      front->next = back;
	      back->prev = front;
	  
	      if(chase->repeat == 0) (void)my_free((char *)chase);
	      else /* if(chase->repeat) */
		{
		  /* 2. move it to the end of the list */
		  tail = head->prev;
		  tail->next = chase;
		  head->prev = chase;
		  chase->prev = tail;
		  chase->next = head;
		}
	    }
	  break;
	}
      chase = chase->next;
    }
  /* non-repeating timers */
  head = &UsualTimerList;
  chase = head->next;
  if(chase != head)
    {
      long s = chase->TheTime.tv_sec;
      long us= chase->TheTime.tv_usec;

      if(s == 0 && us == 0)
	{
	  /* this timer has been cancelled */
	  front = chase->prev;
	  back = chase->next;
	  front->next = back;
	  back->prev = front;
	  (void)my_free( (char *)chase);
	}
    }
  chase = head->next;
  if(chase != head)
    {
      long s = chase->TheTime.tv_sec;
      long us= chase->TheTime.tv_usec;

      if(s < ts || (s==ts && us <= tus)) /* fire it */
	{
	  if(chase->inAction == 0)
	    {
	      /* block recursive invokation */
	      chase->inAction = 1;

	      find++;
	      /* fire it */
	      {
		EZ_CallBackStruct *f = &(chase->callback);
		if(f->callback)  (f->callback)(chase, f->data);
	      }
	      /* reset: remove it to from the timer list */
	      front = chase->prev;
	      back = chase->next;
	      front->next = back;
	      back->prev = front;

	      (void)my_free((char *)chase);
	    }
	}
    }
  return(find);
}
/**************************************************************/  
void  EZ_GetTimerState(timer, sec, usec, repeat)
     EZ_Timer *timer;
     long     *sec, *usec;
     int      *repeat;
{
  if(find_the_timer(timer))
    {
      *sec = timer->sec;
      *usec = timer->usec;
      *repeat = timer->repeat;
      return;
    }
  *sec = 0; *usec = 0; *repeat = 0;
}
/**************************************************************/  
static EZ_Timer *find_the_timer(timer)
     EZ_Timer *timer;
{
  EZ_Timer *head, *tmp;

  head = &RepeatingTimerList;
  tmp = head->next;
  while(tmp != head)
    {
      if(tmp == timer) return(tmp);
      tmp = tmp->next;
    }
  head = &UsualTimerList;  
  tmp = head->next;
  while(tmp != head)
    {
      if(tmp == timer) return(timer);
      tmp = tmp->next;
    }
  return(NULL);
}
/**************************************************************/  
void EZ_CancelTimer(timer)
     EZ_Timer *timer;
{
  if(find_the_timer(timer))  
    {
      /* mark the timer that it has been cancelled */
      timer->TheTime.tv_sec = 0;
      timer->TheTime.tv_usec = 0;
    }
}
/**************************************************************/  
void  EZ_SetTimerPtrData(timer, ptr)
     EZ_Timer *timer; void *ptr;
{
  if(find_the_timer(timer) == timer)
    (timer->callback).data = ptr;
}
void EZ_SetTimerIntData(timer, idata)
      EZ_Timer *timer; int idata;
{
  if(find_the_timer(timer) == timer)
    (timer->callback).internal = idata;
}    
void EZ_SetTimerCallBack(timer, callback, data)
     EZ_Timer *timer; EZ_CallBack callback; void *data;
{
  if(find_the_timer(timer) == timer)
    {
      (timer->callback).callback = callback;
      (timer->callback).data = data;
    }
}
void *EZ_GetTimerPtrData(timer)      
     EZ_Timer *timer;
{
  if(find_the_timer(timer) == timer)
    return((timer->callback).data);
  return(NULL);
}
int EZ_GetTimerIntData(timer)
     EZ_Timer *timer;
{
  if(find_the_timer(timer) == timer)
    return((timer->callback).internal);
  return(0);
}
void EZ_GetTimerClientData(timer, p, i)      
     EZ_Timer *timer;
     void **p; int *i;
{
  if(find_the_timer(timer) == timer)
    {
      if(p) *p = (timer->callback).data;
      if(i) *i = (timer->callback).internal;
    }
  else
    {
      if(p) *p = NULL;
      if(i) *i = 0;
    }
}
void EZ_SetTimerClientData(timer, p, i)      
     EZ_Timer *timer;
     void *p; int i;
{
  if(find_the_timer(timer) == timer)
    {
      (timer->callback).data = p;
      (timer->callback).internal = i;
    }
}
/**************************************************************/
#ifdef EZ_DEBUG
void EZ_DumpTimers()
{
  EZ_Timer *head, *tmp;

  head = &RepeatingTimerList;
  printf("=========Repeadint Timers==========\n");
  tmp = head->next;
  while(head != tmp)
    {
      printf("\t %lx [%ld  %ld] repeat=%d\n", (unsigned long)tmp, tmp->sec,tmp->usec,tmp->repeat);
      tmp = tmp->next;
    }
  head = &UsualTimerList;
  printf("========NonRepeadint Timers========\n");
  tmp = head->next;
  while(head != tmp)
    {
      printf("\t %lx [%ld  %ld] repeat=%d\n", (unsigned long)tmp, tmp->sec,tmp->usec,tmp->repeat);
      tmp = tmp->next;
    }
}
#endif
/**************************************************************/  

#undef _EZ_TIMER_C_
