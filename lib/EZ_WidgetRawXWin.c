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
 ***                                                           ***
 ***              Widget RawXWindow                            ***
 ***                                                           ***
 ***                                                           ***
 *****************************************************************/
#define _EZ_WIDGET_RAWXWIN_C_
#include "EZ_Widget.h"

typedef struct udfwHandle_ {
  int         utype;
  XrmQuark    iname, cname;
  EZ_Widget   *(*Create)     MY_ANSIARGS((EZ_Widget *parent, char *in, char *cn));
  void        (*Setup)       MY_ANSIARGS((EZ_Widget *widget));
  void        (*ComputeSize) MY_ANSIARGS(( EZ_Widget *widget, int *w, int *h));
  void        (*DrawWidget)  MY_ANSIARGS((EZ_Widget *widget));
  void        (*FreeData)    MY_ANSIARGS((EZ_Widget *widget));
  void        (*EventHandle) MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
  EZ_ResourceHandle *resourceHandle;
  struct udfwHandle_  *next;
} EZ_UDFWHandle;

/*********************************************************************/
static EZ_UDFWHandle *udfhandles;
#define UdfType(ptr)                 ((ptr)->utype)
#define UdfIName(ptr)                ((ptr)->iname)
#define UdfCName(ptr)                ((ptr)->cname)
#define UdfCreate(ptr)               ((ptr)->create)
#define UdfSetup(ptr)                ((ptr)->Setup)
#define UdfComputeSize(ptr)          ((ptr)->ComputeSize)
#define UdfDraw(ptr)                 ((ptr)->DrawWidget)
#define UdfFreeData(ptr)             ((ptr)->FreeData)
#define UdfEHandle(ptr)              ((ptr)->EventHandle)
/*********************************************************************
 * 
 *  Functions implemented in this file:
 */
EZ_Widget       *EZ_CreateRawXWindow MY_ANSIARGS((EZ_Widget *parent));
void             EZ_DrawWidgetRawXWin MY_ANSIARGS((EZ_Widget *widget));
void             EZ_ComputeWidgetRawXWinSize MY_ANSIARGS((EZ_Widget *widget, int *w, int *h));
void             EZ_FreeWidgetRawXWinData MY_ANSIARGS((EZ_Widget *widget));
void             EZ_RawXWinEventHandle MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
EZ_Widget        *EZ_CreateUDFWidget MY_ANSIARGS((int type, EZ_Widget *parent,
                                                  char *iname, char *cname));

void             EZ_SetupRawXWinWidget MY_ANSIARGS(( EZ_Widget *widget,
		    void (*ComputeSize) MY_ANSIARGS(( EZ_Widget *widget, int *w, int *h)),
                    void  (*DrawWidget)  MY_ANSIARGS((EZ_Widget *widget)),
                    void  (*FreeData)    MY_ANSIARGS((EZ_Widget *widget)),
                    void  (*EventHandle) MY_ANSIARGS((EZ_Widget *widget, XEvent *event)) ));

/*********************************************************************/
GC      EZ_GetWritableGC MY_ANSIARGS((void));
GC      EZ_GetXorGC MY_ANSIARGS((void));
GC      EZ_GetDnDXorGC MY_ANSIARGS((void));
GC      EZ_GetDnDDragGC MY_ANSIARGS((void));
GC      EZ_GetDashedLineGC MY_ANSIARGS((void));

int    EZ_RegisterPrimitiveWidget MY_ANSIARGS((char *iname, char *cname,
                    void  (*Setup)  MY_ANSIARGS((EZ_Widget *widget)),
		    void  (*ComputeSize) MY_ANSIARGS(( EZ_Widget *widget, int *w, int *h)),
                    void  (*DrawWidget)  MY_ANSIARGS((EZ_Widget *widget)),
                    void  (*FreeData)    MY_ANSIARGS((EZ_Widget *widget)),
                    void  (*EventHandle) MY_ANSIARGS((EZ_Widget *widget, XEvent *event)) ));

int   EZ_RegisterCompositeWidget MY_ANSIARGS((char *iname, char *cname,
                    EZ_Widget *(*Create)  MY_ANSIARGS((EZ_Widget *parent, char *in, char *cn))
                                                ));

void  EZ_RegisterResourcesAll MY_ANSIARGS((int type, int nresrc, EZ_ResourceSpec *spec, 
                    int  (*Configure)   MY_ANSIARGS((EZ_Widget *widget, int opt, EZ_Value *values))
                                        ));
                                                
void  EZ_RegisterResourcesWidget MY_ANSIARGS((EZ_Widget *widget, int nresrc, EZ_ResourceSpec *spec, 
                    int  (*Configure)   MY_ANSIARGS((EZ_Widget *widget, int opt, EZ_Value *values))
                                        ));

EZ_ResourceHandle *EZ_GetDefaultResourceHandle MY_ANSIARGS(( int type));

void               EZ_FreeResourceHandles  MY_ANSIARGS((void)); 
EZ_UnknownDataType *EZ_GetRawXWinPrivateData MY_ANSIARGS((EZ_Widget *widget));
EZ_UnknownDataType *EZ_GetUDFWidgetPrivateData MY_ANSIARGS((EZ_Widget *widget));

void   *EZ_SearchUDFWidgetHandle MY_ANSIARGS(( int type));
void   EZ_DeleteAllUDFWidgetHandles MY_ANSIARGS((void));
void   EZ_extractValueTypes MY_ANSIARGS((char *spec, unsigned char *out));
/*********************************************************************
 * 
 *  Local Variables.
 */
static EZ_WidgetHandle EZ_RawXWinHandle =
{ 
  EZ_ComputeWidgetRawXWinSize,
  EZ_DrawWidgetRawXWin,
  EZ_FreeWidgetRawXWinData,
  EZ_RawXWinEventHandle, 
};

/*********************************************************************/

EZ_Widget  *EZ_CreateRawXWindow(parent)
     EZ_Widget  *parent;     /* parent widget    */
{
  EZ_Widget  *tmp;
  
  tmp = EZ_CreateNewWidget(parent);
  /*--------------------------------------------------
   * Register the handling functions for label.
   *  has to be done after  EZ_CreateNewWiget.
   *-------------------------------------------------*/
  EZ_WidgetHandlingFunctions[EZ_WIDGET_RAW_XWINDOW] = &EZ_RawXWinHandle;

  EZ_SetWidgetTypeAndNames(tmp, EZ_WIDGET_RAW_XWINDOW);
  EZ_WidgetBorderStyle(tmp)  = EZ_BORDER_NONE;
  EZ_WidgetBorderWidth(tmp)  = 0;
  EZ_WidgetPadX(tmp) = EZ_WidgetPadY(tmp) = 0;
  EZ_WidgetSepX(tmp) = EZ_WidgetSepY(tmp) = 0;
  EZ_SetWidgetFocusableFlag(tmp);
  EZ_RawXWinPHandle(tmp) = NULL;
  memset(EZ_RawXWinPData(tmp), 0, sizeof(EZ_RawXWinPData(tmp)));
  memset(EZ_RawXWinCData(tmp), 0, sizeof(EZ_RawXWinCData(tmp)));

  return(tmp);
}

/*************************************************************************/
void EZ_SetupRawXWinWidget(wptr, csize, draw, freedata, ehandle)
     EZ_Widget *wptr;
     void  (*csize)  MY_ANSIARGS(( EZ_Widget *widget, int *w, int *h));
     void  (*draw)  MY_ANSIARGS((EZ_Widget *widget));
     void  (*freedata) MY_ANSIARGS((EZ_Widget *widget));
     void  (*ehandle)  MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
{
  if(wptr && EZ_WidgetType(wptr) == EZ_WIDGET_RAW_XWINDOW)
    {
      EZ_UDFWHandle *ptr = (EZ_UDFWHandle *) EZ_RawXWinPData(wptr);
      ptr->Create = NULL;
      ptr->Setup = NULL;
      ptr->iname = NULLQUARK;
      ptr->cname = NULLQUARK;
      ptr->ComputeSize = csize;
      ptr->DrawWidget = draw;
      ptr->FreeData = freedata;
      ptr->EventHandle = ehandle;
      ptr->next = NULL;
      EZ_RawXWinPHandle(wptr) = ptr;
    }
}
/*************************************************************************/
void EZ_FreeWidgetRawXWinData(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_UDFWHandle *ptr = EZ_RawXWinPHandle(widget);
      if(ptr && UdfFreeData(ptr)) UdfFreeData(ptr)(widget);
    }
}
 
/********************************************************************
 *
 *  Figure out the dimension of a label widget.
 */

void EZ_ComputeWidgetRawXWinSize(widget, w, h)
    EZ_Widget *widget;
    int             *w, *h;
{
  int ch= 4, cw=4;
  EZ_UDFWHandle *ptr = EZ_RawXWinPHandle(widget);
  if(ptr && UdfComputeSize(ptr) ) (UdfComputeSize(ptr))(widget, &cw, &ch);

  *w = cw;
  *h = ch;
  EZ_SetWidgetSizeComputedFlag(widget);	
}

/*******************************************************************
 *
 *  Draw does nothing
 */
void  EZ_DrawWidgetRawXWin(wptr)
     EZ_Widget *wptr;
{
  if(wptr)
    {
      EZ_UDFWHandle *ptr = EZ_RawXWinPHandle(wptr);
      if(ptr && UdfDraw(ptr)) UdfDraw(ptr)(wptr);
    }
}

/*******************************************************************
 *
 *  Handle no events;
 */
void EZ_RawXWinEventHandle(widget, event)
     EZ_Widget *widget;
     XEvent *event;
{
  if( EZ_GetWidgetDisabledFlag(widget) && event->type != Expose )  return;
  {
    EZ_UDFWHandle *ptr = EZ_RawXWinPHandle(widget);
    if(ptr && UdfEHandle(ptr)) UdfEHandle(ptr)(widget,event);
  }
}

/****************************************************************************/
EZ_Widget *EZ_CreateUDFWidget(type, parent, iname, cname)
     int type; EZ_Widget *parent; char  *iname, *cname;
{
  EZ_Widget *widget = NULL;
  EZ_UDFWHandle *tmp = EZ_SearchUDFWidgetHandle(type);
  if(tmp)
    {
      if(tmp->Create)
        {
          char *nameI, *nameC;
          if(iname == NULL) nameI = XrmQuarkToString(tmp->iname);
          else nameI = iname;
          if(cname == NULL) nameC = XrmQuarkToString(tmp->cname);
          else nameC = cname;
          widget = (tmp->Create)(parent, nameI, nameC);
        }
      else 
        {
          widget = EZ_CreateRawXWindow(parent);
          EZ_RawXWinPHandle(widget) = tmp;
          if(iname != NULL) EZ_WidgetIName(widget) = EZ_StringToQuark(iname);
          else if(tmp->iname) EZ_WidgetIName(widget) = tmp->iname;
          if(cname != NULL) EZ_WidgetCName(widget) = EZ_StringToQuark(cname);
          else if(tmp->cname) EZ_WidgetCName(widget) = tmp->cname;
          if(UdfSetup(tmp)) UdfSetup(tmp)(widget);
        }
      if(widget && tmp->resourceHandle)
        EZ_WidgetResourceHandle(widget) = tmp->resourceHandle;
    }
  return(widget);
}
/****************************************************************************/
static EZ_UDFWHandle *createUDFWidgetHandle()
{
  EZ_UDFWHandle *udf = NULL;
  int type;

  if(udfhandles) { type = udfhandles->utype+1; } else type = 1024;
  if( (udf = (EZ_UDFWHandle *)my_malloc(sizeof(EZ_UDFWHandle), _MISC_DATA_)) )
    {
      memset(udf, 0, sizeof(EZ_UDFWHandle));
      udf->utype = type;
      udf->next = udfhandles;
      udfhandles = udf;
    }
  else EZ_OutOfMemory("findCreateUDFWidgetHandle");
  return(udf);
}
/****************************************************************************/
int EZ_RegisterPrimitiveWidget(iname, cname, setup, csize, draw, freedata, ehandle)
     char  *iname, *cname;
     void  (*setup) MY_ANSIARGS((EZ_Widget *widget));
     void  (*csize)  MY_ANSIARGS(( EZ_Widget *widget, int *w, int *h));
     void  (*draw)  MY_ANSIARGS((EZ_Widget *widget));
     void  (*freedata) MY_ANSIARGS((EZ_Widget *widget));
     void  (*ehandle)  MY_ANSIARGS((EZ_Widget *widget, XEvent *event));
{
  EZ_UDFWHandle *udf;
  if( (udf = createUDFWidgetHandle()))
    {
      udf->Create = NULL;
      udf->Setup = setup;
      udf->ComputeSize = csize;
      udf->DrawWidget = draw;
      udf->FreeData = freedata;
      udf->EventHandle = ehandle;
      if(iname) udf->iname = XrmStringToQuark(iname);
      else  udf->iname = EZ_WidgetDefaultQInstanceNames[0];
      if(cname) udf->cname = XrmStringToQuark(cname);
      else  udf->cname = EZ_WidgetDefaultQClassNames[0];
      return(udf->utype);
    }
  return(0);
}
int EZ_RegisterCompositeWidget(iname, cname, create)
     char *iname, *cname;
     EZ_Widget  *(*create) MY_ANSIARGS((EZ_Widget *parent, char *in, char *cn));
{
  EZ_UDFWHandle *udf;
  if( (udf = createUDFWidgetHandle()))
    {
      udf->Create = create;
      udf->Setup = NULL;
      udf->ComputeSize = NULL;
      udf->DrawWidget = NULL;
      udf->FreeData = NULL;
      udf->EventHandle = NULL;
      if(iname) udf->iname = XrmStringToQuark(iname);
      else  udf->iname = EZ_WidgetDefaultQInstanceNames[0];
      if(cname) udf->cname = XrmStringToQuark(cname);
      else  udf->cname = EZ_WidgetDefaultQClassNames[0];
      return(udf->utype);
    }
  return(0);
}
/************************************************************************/
void *EZ_SearchUDFWidgetHandle(type)
     int type;
{
  EZ_UDFWHandle **ptr = &udfhandles;
  while(*ptr != NULL)
    {
      if( (*ptr)->utype == type)
	{
	  return(*ptr);
	  break;
	}
      ptr = &((*ptr)->next);
    }
  return(NULL);
}
/************************************************************************/
void EZ_DeleteAllUDFWidgetHandles()
{
  EZ_UDFWHandle *tmp, *ptr = udfhandles;  
  while(ptr)
    {
      /* delete resource handles */
      EZ_ResourceHandle *s, *rhandle = ptr->resourceHandle;
      while(rhandle)
        {
          s = rhandle->next;
          if(rhandle->resources) my_free(rhandle->resources);
          my_free(rhandle);
          rhandle=s;
        }
      tmp = ptr->next;
      (void)my_free((char *)ptr);
      ptr = tmp;
    }
  udfhandles = NULL;
}
/**************************************************************************/
EZ_UnknownDataType *EZ_GetRawXWinPrivateData(widget) 
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_RAW_XWINDOW)
    return(EZ_RawXWinCData(widget));
  return(NULL);
}
EZ_UnknownDataType *EZ_GetUDFWidgetPrivateData(widget) 
     EZ_Widget *widget;
{
  if(widget && EZ_WidgetType(widget) == EZ_WIDGET_RAW_XWINDOW)
    return(EZ_RawXWinCData(widget));
  return(NULL);
}

/**************************************************************************/
GC    EZ_GetWritableGC() { return (EZ_WRITABLEGC);}
GC    EZ_GetXorGC() { return (EZ_XORGC);}
GC    EZ_GetDnDXorGC()  { return (EZ_DNDXORGC);}
GC    EZ_GetDnDDragGC()  { return (EZ_DRAGGC);}
GC    EZ_GetDashedLineGC()  { return (EZ_DASHEDLINEGC);}
/**************************************************************************/
unsigned char  *EZ_GetUDFResourceVTypes(rhandle, option) 
     EZ_ResourceHandle *rhandle; int option;
{
  if(rhandle)
    {
      UDFResource *resources = rhandle->resources;
      int i, n =rhandle->nresources;
      for(i = 0; i < n; i++)
        { if( resources[i].option == option) return(resources[i].vtypes); }
    }
  return(NULL);
}
/**************************************************************************/     
int  EZ_SetWidgetUDFAttributes(widget,option, values)
     EZ_Widget *widget;
     int option;
     EZ_Value *values;
{
  EZ_ResourceHandle *rhandle = EZ_WidgetResourceHandle(widget);
  int ii = 0;
  while(rhandle)
    {
      if(rhandle->configure)
        ii |=  (rhandle->configure)(widget, option, values);
      rhandle = rhandle->next;
    }
  return(ii);
}
/**************************************************************************/
void EZ_extractValueTypes(spec, out) char *spec; unsigned char *out;
{
  int i = 0, j = 0;
  char *ptr= spec, *qtr;
  while(ptr)
    {
      while(*ptr && (*ptr == ' ' || *ptr=='\t')) ptr++;
      if(*ptr)
        {
          if(!strncmp(ptr, "int",3)) out[++i] = EZ_INT;
          else if(!strncmp(ptr, "float",5))  out[++i] = EZ_FLOAT;
          else if(!strncmp(ptr, "ulong",5))  out[++i] = EZ_ULONG;
          else if(!strncmp(ptr, "string",6)) out[++i] = EZ_STRING;
          else if(!strncmp(ptr, "pointer",7)) out[++i] = EZ_POINTER;
          else if(!strncmp(ptr, "boolean",4)) out[++i] = EZ_BOOL;
          else if(!strncmp(ptr, "short",5)) out[++i] = EZ_SHORT;
          else if(!strncmp(ptr, "enum",4)) out[++i] = EZ_ENUM;
          else if(!strncmp(ptr, "file",4)) out[++i] = EZ_FILENAME;
          else if(!strncmp(ptr, "font",4)) out[++i] = EZ_FONTNAME;
          else if(!strncmp(ptr, "color",5)) out[++i] = EZ_COLOR;
          else if(!strncmp(ptr, "function",5)) out[++i] = EZ_FUNCTION;
          else if(!strncmp(ptr, "...",3)) { out[++i] = EZ_REPEAT;j++;}
          else
            {
              fprintf(stderr, "EZ_RegisterResources: unknown type spec '%s'\n", ptr);  
              out[++i] = EZ_INT; /* treate as int */
            }
        }
      if( (qtr = strchr(ptr, ',')) ) ptr = qtr+1;
      else ptr = NULL;
    }
  out[i+1] = 127;
  out[0] = i-j;
}
/**************************************************************************/
EZ_ResourceHandle *EZ_WResourceHandles[EZ_TOTAL_NUMBER_OF_WIDGETS];
EZ_ResourceHandle *EZ_ApplicationResourceHandle;
/**************************************************************************/
void EZ_FreeResourceHandles()
{
  int i;
  EZ_ResourceHandle *s, *tmp = EZ_ApplicationResourceHandle;
  while(tmp)
    {
      s = tmp->next;
      if(tmp->resources)
        my_free(tmp->resources);
      my_free(tmp);
      tmp = s;
    }
  EZ_ApplicationResourceHandle = NULL;
  for(i = 0; i < EZ_TOTAL_NUMBER_OF_WIDGETS; i++)
    {
      tmp = EZ_WResourceHandles[i];
      while(tmp)
        {
          s = tmp->next;
          if(tmp->resources) my_free(tmp->resources);
          my_free(tmp);
          tmp=s;
        }
      EZ_WResourceHandles[i] = NULL;
    }
}

/**************************************************************************/
EZ_ResourceHandle *EZ_GetDefaultResourceHandle(type) int type;
{
  EZ_ResourceHandle *ret=NULL, *ptr=NULL, *s, *tmp = NULL;
  if(type < EZ_TOTAL_NUMBER_OF_WIDGETS)
    tmp = EZ_WResourceHandles[type];
  else
    {
      EZ_UDFWHandle *whand = EZ_SearchUDFWidgetHandle(type);
      if(whand) tmp = whand->resourceHandle;
    }
  while(tmp)
    {
      s =  (EZ_ResourceHandle *) my_malloc( sizeof(EZ_ResourceHandle), _MISC_DATA_);
      if(ret == NULL) { ret = ptr = s;}
      else { ptr->next = s; ptr = s;}
      memcpy(s,tmp, sizeof(EZ_ResourceHandle));
      s->next = NULL;
      tmp = tmp->next;
    }
  return(ret);
}
/**************************************************************************/
void  EZ_RecordResourceSpec(tmp, nspec, spec, configure)
     EZ_ResourceHandle *tmp;
     int nspec;
     EZ_ResourceSpec *spec;
     int (*configure) MY_ANSIARGS((EZ_Widget *widget, int opt, EZ_Value *values));
{
  if(tmp)
    {
      int i,cnt=0;
      tmp->resources = (UDFResource *)my_malloc( nspec * sizeof(UDFResource),
                                                 _MISC_DATA_);
      memset(tmp->resources, 0, nspec * sizeof(UDFResource));
      for(i = 0; i < nspec; i++)
        {
          if(spec[i].option >= 1024)
            {
              char *cn = spec[i].cname;
              char *in = spec[i].iname;
              if(cn == NULL) cn = in;
              if(in == NULL) in = cn;
              if(cn && in)
                {
                  (tmp->resources)[cnt].cname = EZ_StringToQuark(cn);
                  (tmp->resources)[cnt].iname = EZ_StringToQuark(in);
                  EZ_extractValueTypes(spec[i].vtypes, (tmp->resources)[cnt].vtypes);
                  (tmp->resources)[cnt].option = spec[i].option;
                  cnt++;
                }
            }
          else
            fprintf(stderr, "RegisterResources: option=%d < 1024, ignored.\n", 
                    spec[i].option);
        }
      tmp->nresources = cnt;
      tmp->configure = configure;
    }
}
/**************************************************************************/
void  EZ_RegisterResourcesAll(type, nspec, spec, configure)
     int type, nspec;
     EZ_ResourceSpec *spec;
     int (*configure) MY_ANSIARGS((EZ_Widget *widget, int opt, EZ_Value *values));
{     
  EZ_ResourceHandle *ptr, *tmp;

  if(nspec <= 0) return;
  
  tmp = (EZ_ResourceHandle *) my_malloc( sizeof(EZ_ResourceHandle),_MISC_DATA_);
  memset(tmp, 0, sizeof(EZ_ResourceHandle));
  if(type < EZ_TOTAL_NUMBER_OF_WIDGETS)
    {
      ptr = EZ_WResourceHandles[type];
      if(ptr != NULL)
        {
          while(ptr->next != NULL) ptr = ptr->next;
          ptr->next = tmp;
        }
      else EZ_WResourceHandles[type] = tmp;
    }
  else
    {
      EZ_UDFWHandle *whand = EZ_SearchUDFWidgetHandle(type);

      if(whand)
        {
          ptr = whand->resourceHandle;
          if(ptr) 
            {
              while(ptr->next != NULL) ptr = ptr->next;
              ptr->next = tmp;
            }
          else whand->resourceHandle = tmp;
        }
      else { my_free(tmp); return;}
    }
  EZ_RecordResourceSpec(tmp, nspec, spec, configure); 
  tmp->mark = 1;
}
/**************************************************************************/
void  EZ_RegisterResourcesWidget(widget, nspec, spec, configure)
     EZ_Widget *widget;
     int nspec;
     EZ_ResourceSpec *spec;
     int (*configure) MY_ANSIARGS((EZ_Widget *widget, int opt, EZ_Value *values));
{     
  EZ_ResourceHandle *tmp = NULL;
  if(widget && nspec > 0)
    {
      EZ_ResourceHandle *old = EZ_WidgetResourceHandle(widget);
      tmp = (EZ_ResourceHandle *) my_malloc( sizeof(EZ_ResourceHandle), _MISC_DATA_);
      memset(tmp, 0, sizeof(EZ_ResourceHandle));
      if(old) { while(old->next != NULL) old = old->next; old->next = tmp;}
      else  EZ_WidgetResourceHandle(widget) = tmp;
    }
  EZ_RecordResourceSpec(tmp, nspec, spec, configure); 
}
/**************************************************************************/
#undef _EZ_WIDGET_RAWXWIN_C_
