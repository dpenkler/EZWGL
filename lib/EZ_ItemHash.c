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
 *  HashTable for display items.  3/5/98
 *
 *****************************************************************/
#define _EZ_ITEM_HASH_C_

#include "EZ_Widget.h"

typedef struct itemHashTbEntry {
  EZ_Item                 *item;
  struct itemHashTbEntry  *next;
} EZ_ItemHTE;

static EZ_ItemHTE    EZ_ItemHashTable[EZ_WIDGET_HASH_TABLE_SIZE];

/*****************************************************************
 *
 * public functions
 */
void      EZ_InsertItemToHashTable MY_ANSIARGS((EZ_Item *item));
EZ_Item   *EZ_LookupItemFromHT MY_ANSIARGS((EZ_Item *item));
EZ_Item   *EZ_ItemExist MY_ANSIARGS((EZ_Item *item));
void      EZ_RemoveItemFromHT MY_ANSIARGS((EZ_Item *item));
void      EZ_DestroyAllItems  MY_ANSIARGS((void));
void      EZ_EditItemResources MY_ANSIARGS((XrmDatabase db));
void      EZ_SetItemDefaultFontAll MY_ANSIARGS((XFontStruct *old,
                                                XFontStruct *neww));
void      EZ_SetItemDefaultFgAll MY_ANSIARGS((unsigned long old, unsigned long newfg));
/*****************************************************************/
#define HashItemPtr(p_) ( (int) ((unsigned long)(p_)) % EZ_WIDGET_HASH_TABLE_SIZE)

/*****************************************************************/
void EZ_InsertItemToHashTable(item)
     EZ_Item *item;
{
  if(item)
    {
      EZ_ItemHTE   *tmp;
      int          hash_value = HashItemPtr(item);
      tmp = (EZ_ItemHTE *)my_malloc(sizeof(EZ_ItemHTE), _ITEM_HTABLE_);
      if(!tmp) EZ_OutOfMemory("EZ_InsertItemToHashTable");
      tmp->item = item;
      tmp->next = EZ_ItemHashTable[hash_value].next;
      EZ_ItemHashTable[hash_value].next = tmp;
    }
}
/*****************************************************************/
EZ_Item  *EZ_LookupItemFromHT(item)
     EZ_Item *item;
{
  EZ_ItemHTE   *tmp;
  int          hash_value;

  if(item)
    {
      hash_value = HashItemPtr(item);
      tmp = EZ_ItemHashTable[hash_value].next;
      while(tmp)
	{
	  if(tmp->item == item) return(item);
	  tmp = tmp->next;
	}
    }
  return( (EZ_Item *)NULL);
}

EZ_Item  *EZ_ItemExist(item)
     EZ_Item *item;
{
  return(EZ_LookupItemFromHT(item));
}
/*****************************************************************/
void  EZ_RemoveItemFromHT(item)
     EZ_Item *item;
{
  EZ_ItemHTE *tmp, *tmpa;
  int        hash_value;

  if(item)
    {
      hash_value = HashItemPtr(item);
      tmp = &(EZ_ItemHashTable[hash_value]);
      while(tmp && tmp->next)
	{
	  if(tmp->next->item == item)
	    {
	      tmpa = tmp->next;
	      tmp->next = tmp->next->next;
	      (void)my_free( (char *)(tmpa));
	      return;
	    }
	  tmp = tmp->next;
	}
    }
}
/*****************************************************************/
void EZ_DestroyAllItems()
{
  EZ_ItemHTE  *tmp;  
  EZ_Item     **all;
  int i, count = 0;
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_ItemHashTable[i].next;
      while(tmp) { count++; tmp=tmp->next;}
    }
  all = (EZ_Item **)malloc((count+1)*sizeof(EZ_Item *));
  for(count=0,i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_ItemHashTable[i].next;
      while(tmp) {all[count]=tmp->item; count++; tmp=tmp->next;}
    }
  for(i = 0; i < count; i++) EZ_DestroyItem(all[i]);
  (void)free(all);
}
/*****************************************************************/
void EZ_EditItemResources(db)
     XrmDatabase db;
{
  EZ_ItemHTE  *tmp;  
  int i;
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_ItemHashTable[i].next;
      while(tmp)
        {
          (void) EZ_GetAndUseItemResourcesWithDB(tmp->item, db);
          tmp = tmp->next;
        }
    }
}
/*****************************************************************/
void  EZ_SetItemDefaultFontAll(old_font_struct, font_struct)
     XFontStruct *font_struct, *old_font_struct;
{
  EZ_ItemHTE  *tmp;  
  int i;
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_ItemHashTable[i].next;
      while(tmp)
        {
          EZ_Item *item = tmp->item;
          if(EZ_ItemFont(item) == old_font_struct)
            {
              EZ_ItemFont(item) = font_struct;
              EZ_ItemConfigureSentienl(item,1);
            }
          tmp = tmp->next;
        }
    }
}
/*****************************************************************/
void EZ_SetItemDefaultFgAll(old, nn)
     unsigned long old, nn;
{
  EZ_ItemHTE  *tmp;  
  int i;
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_ItemHashTable[i].next;
      while(tmp)
        {
          EZ_Item *item = tmp->item;
          if(EZ_ItemForeground(item) == old)
            {
              EZ_ItemForeground(item) = nn;
              EZ_ItemConfigureSentienl(item,1);
            }
          tmp = tmp->next;
        }
    }
}
/*****************************************************************/
void  EZ_ListAllWidgetItems(widget_ret, cnt_ret)
     EZ_Widget **widget_ret; int *cnt_ret;
{
  EZ_ItemHTE  *tmp;  
  int i, total=0;
  for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
    {
      tmp = EZ_ItemHashTable[i].next;
      while(tmp)
        {
          EZ_Item *item = tmp->item;
          if(EZ_ItemType(item) == EZ_WIDGET_ITEM)
            {
              EZ_Widget *widget= EZ_WidgetItemWidget(item);
              if(widget && EZ_WidgetParent(widget)) 
                {
                  widget_ret[total++] = widget;
                  if(total >= 255) break;
                }
            }
          tmp = tmp->next;
        }
    }
  *cnt_ret = total;
}
/*****************************************************************/

#ifdef EZ_DEBUG
void EZ_DumpItemHashTable()
{
  printf("========Items Hash Tabble========\n");
  {
    register int i;
    EZ_ItemHTE  *tmp;

    for(i = 0; i < EZ_WIDGET_HASH_TABLE_SIZE; i++)
      {
	tmp = EZ_ItemHashTable[i].next;
	if(tmp)
	  {
	    printf("%d: ", i);
	    while(tmp)
	      {
		int type = EZ_ItemType(tmp->item);
		printf(" -->[%lx=>%d:%s", (unsigned long)tmp->item, type,
		       type == EZ_LABEL_ITEM? EZ_LabelItemString(tmp->item): "");
		tmp = tmp->next;
	      }
	    printf("\n");
	  }
      }
  }
}
#endif
/*****************************************************************/
#undef _EZ_ITEM_HASH_C_
