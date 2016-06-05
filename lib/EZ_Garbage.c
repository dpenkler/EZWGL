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
/*****************************************************************
 *
 * If an item or widget is destroyed in one of its callbacks or
 * event handlers. The resource allocated for the object is not
 * immediately released. Instead, it is inserted in a garbage 
 * list and will be freed at a later time.
 *
 *****************************************************************/
#define _EZ_GARBAGE_C_

#include "EZ_Widget.h"

typedef struct EZ_GarbageEntry_ {
  int                     type;
  void                    *obj;
  struct EZ_GarbageEntry_ *next;
} EZ_GarbageEntry;

static EZ_GarbageEntry *garbageList = NULL;

/*****************************************************************
 *
 * public functions
 */
void EZ_InitGarbageList MY_ANSIARGS((void));
void EZ_InsertObjToGarbageList MY_ANSIARGS((int type, void *obk));
void EZ_ResetGarbageList MY_ANSIARGS((void));

/*****************************************************************
 *
 *  a timer calls the next routine once every second.
 */
/*ARGSUSED*/
static void collectGarbage(obj, data)
     void *obj; void *data;
{
  EZ_GarbageEntry *parent = NULL, *ptr = garbageList;

  if(ptr)
    {
      struct timeval tv1, tv2;

      gettimeofday(&tv1, NULL);
      while(ptr)
	{
	  int remove = 0;

	  if(ptr->type == EZ_GARBAGE_IS_WIDGET)
	    {
	      EZ_Widget *widget = (EZ_Widget *)ptr->obj;
	      if(EZ_WidgetBlocked(widget) <= 0)
		{
		  EZ_DestroyWidgetFinal(widget);
		  remove = 1;
		}
	    }
	  else if(ptr->type == EZ_GARBAGE_IS_ITEM)
	    {
	      EZ_Item *item = (EZ_Item *)ptr->obj;
	      if(EZ_ItemBlocked(item) <= 0)
		{
		  EZ_DestroyItemFinal(item);	      
		  remove = 1;
		}
	    }
	  else remove = 1;
	  if(remove)
	    {
	      EZ_GarbageEntry  *tmp = ptr;
	      if(parent) parent->next = ptr->next;
	      else garbageList = ptr->next;
	      ptr = ptr->next;
	      (void)my_free((void *)tmp);
	    }
	  else { parent = ptr; ptr = ptr->next;}

	  gettimeofday(&tv2, NULL);
	  if( ((tv2.tv_sec - tv1.tv_sec) << 20) +
	      tv2.tv_usec - tv1.tv_usec >  500000) break;
	}
    }
}
/*****************************************************************/
void EZ_InitGarbageList()
{
  garbageList = NULL;
  (void)EZ_CreateTimer(1, 0, -1, collectGarbage, NULL, 0);
}
void EZ_ResetGarbageList()
{
  garbageList = NULL;  
}

/*****************************************************************/
void EZ_InsertObjToGarbageList(type, obj)
     int type; void *obj;
{
  EZ_GarbageEntry  *tmp;
  tmp = (EZ_GarbageEntry *)my_malloc(sizeof(EZ_GarbageEntry),_GARBAGE_OBJ_);
  if(tmp == NULL)  EZ_OutOfMemory("EZ_InsertObjToGarbageList");
  tmp->type = type;
  tmp->obj = obj;
  tmp->next = garbageList;
  garbageList = tmp;
}
/*****************************************************************/
#ifdef EZ_DEBUG 
void dumpGarbageList()
{
  EZ_GarbageEntry  *ptr = garbageList;
  if(ptr)
    {
      printf("========== Widget/Item Garbage List ========\n");
      while(ptr)
	{
	  if(ptr->type == EZ_GARBAGE_IS_WIDGET)
	    printf("\t === [%s] %d ====\n",
		   EZ_WidgetDefaultClassNames[(int)EZ_WidgetType((EZ_Widget *)ptr->obj)],
		   EZ_WidgetBlocked((EZ_Widget *)ptr->obj));
	  else if(ptr->type == EZ_GARBAGE_IS_ITEM)
	    {
	      EZ_Item *item = (EZ_Item *)ptr->obj;
	      if(EZ_ItemType(item) == EZ_LABEL_ITEM)
		{
		  char *str; int len;
		  EZ_GetLabelItemStringInfo(item, &str, &len);
		  printf("\t === [%s] %d ====\n",str,
			 EZ_WidgetBlocked(item));
		}
	    }
	  else printf("===!!! unknown garbage type [%lx] !!!===\n", (unsigned long)ptr->obj);
	  
	  ptr = ptr->next;
	}
    }
}
#endif
/*****************************************************************/
#undef _EZ_GARBAGE_C_
