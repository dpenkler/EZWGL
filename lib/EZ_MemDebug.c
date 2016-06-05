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

/***********************************************************
 *
 *  trivial malloc debuging facility
 */
#if defined(EZ_MEM_DEBUG) || defined(EZ_DEBUG)

#include "EZ_Widget.h"

typedef struct mem_record_ {
  void *addr;
  int  size;
  int  info;
  struct mem_record_ *next;
} mem_record;

#define HASH_SIZE 541
static mem_record  malloc_records[HASH_SIZE];
#define MemDebugData(type,code,name)  name,
static char *malloc_caller_names[] = {
#include "MemDebug.h"
};
#undef MemDebugData
static int totalBytes = 0;
/******************************************************************************/
void initialize_mem_debug_data MY_ANSIARGS((void));
void EZ_DumpMemoryRecords      MY_ANSIARGS((void));
void *my_malloc                MY_ANSIARGS((int n, int info));
void *my_realloc               MY_ANSIARGS((void *ptr, int n, int info));
void my_free                   MY_ANSIARGS((void *ptr));

/******************************************************************************/
static void MDInsert MY_ANSIARGS((void *data, int size, int info));
static void MDRemove  MY_ANSIARGS((void *data));

/******************************************************************************/
void initialize_mem_debug_data()
{
  int i;
  for(i = 0; i < HASH_SIZE; i++)
    malloc_records[HASH_SIZE].next = NULL;
}
/******************************************************************************/
static void MDInsert(addr, size, info)
     void *addr; int size, info;
{
  if(addr)
    {
      int bucked = (unsigned long) addr % HASH_SIZE;
      mem_record *tmp = (mem_record *)malloc(sizeof(mem_record));
      totalBytes += size;
      tmp->addr = addr;
      tmp->size = size;
      tmp->info = info;
      tmp->next = malloc_records[bucked].next;
      malloc_records[bucked].next = tmp;
    }
}
static void MDRemove(addr)
     void *addr;
{
  if(addr)
    {
      int bucked = (unsigned long) addr % HASH_SIZE;  
      mem_record *chase, *tmp =  malloc_records +bucked;
      while(tmp)
	{
	  chase = tmp->next;
	  if(chase && chase->addr == addr)
	    {
	      tmp->next = chase->next;
	      totalBytes -= chase->size; 
	      (void)free((char *)chase);
	      break;
	    }
	  tmp = chase;
	}
    }
}

static void dump_mem_records(flag) int flag;
{
  int i;
  mem_record *tmp, *save;

  if(totalBytes == 0) return;

  printf("===== Allocated %d bytes ===== \n", totalBytes);
  for(i = 0; i < HASH_SIZE; i++)
    {
      tmp =  malloc_records[i].next;
      while(tmp)
	{
	  printf("%d bytes at %lx : %s [%s]\n", 
		  tmp->size, (unsigned long)tmp->addr, 
		  malloc_caller_names[tmp->info], (char *)tmp->addr);
	  save = tmp->next;
	  if(flag)  (void)free(tmp);
	  tmp = save;
	}
      if(flag) malloc_records[i].next = NULL;
    }
}


void EZ_DumpMemoryRecords()
{
  dump_mem_records(0);
}

void EZ_DumpAndClearMemoryRecords()
{
  dump_mem_records(1);
}


void *my_malloc(n, info) int n, info;
{
  char *tmp = malloc( (unsigned) n);
  MDInsert(tmp, n, info);
  /*  printf("Malloc:  %5d  [%s]\n", n, malloc_caller_names[info]);*/
  return((void *)tmp);
}

void *my_realloc(a,b, info)
     void *a; int b, info;
{
  void *tmp;
  MDRemove(a);
  tmp = realloc(a,b);
  MDInsert(tmp,b, info);
  /* printf("Realloc: %5d  [%s]\n", b, malloc_caller_names[info]);*/
  return( (void *)tmp);
}
void my_free(a)
     void *a;
{
  MDRemove(a);
  (void)free((char *)a);
}
#undef HASH_SIZE
#endif


