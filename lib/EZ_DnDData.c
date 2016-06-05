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
#define _EZ_DND_DATA_C_
#include "EZ_Widget.h"

void EZ_ItemAddDnDDataEncoder MY_ANSIARGS((EZ_Item *item, Atom type, unsigned int tag,
					   EZ_DnDEncoder encoder, void *edata,
					   EZ_CallBack callback, void *cdata));
void EZ_ItemDeleteDnDDataEncoder MY_ANSIARGS((EZ_Item *item, 
					      EZ_DnDEncoder encoder, void *edata));
void EZ_ItemDeleteAllDnDDataEncoders MY_ANSIARGS((EZ_Item *item));
void EZ_ItemAddDnDDataDecoder MY_ANSIARGS((EZ_Item *item, Atom type, unsigned int tag,
					   EZ_DnDDecoder decoder, void *edata,
					   EZ_CallBack callback, void *cdata));
void EZ_ItemDeleteDnDDataDecoder MY_ANSIARGS((EZ_Item *item,
					      EZ_DnDDecoder decoder, void *edata));
void EZ_ItemDeleteAllDnDDataDecoders MY_ANSIARGS((EZ_Item *item));


void EZ_WidgetAddDnDDataEncoder MY_ANSIARGS((EZ_Widget *widget, Atom type, unsigned int tag,
					     EZ_DnDEncoder encoder, void *edata,
					     EZ_CallBack callback, void *cdata));
void EZ_WidgetDeleteDnDDataEncoder MY_ANSIARGS((EZ_Widget *widget, 
						EZ_DnDEncoder encoder, void *edata));
void EZ_WidgetDeleteAllDnDDataEncoders MY_ANSIARGS((EZ_Widget *widget));
void EZ_WidgetAddDnDDataDecoder MY_ANSIARGS((EZ_Widget *widget,Atom type, unsigned int tag,
					     EZ_DnDDecoder encoder, void *edata,
					     EZ_CallBack callback, void *cdata));
void EZ_WidgetDeleteDnDDataDecoder MY_ANSIARGS((EZ_Widget *widget,
						EZ_DnDDecoder decoder, void *edata));
void EZ_WidgetDeleteAllDnDDataDecoders MY_ANSIARGS((EZ_Widget *widget));

void EZ_WidgetAddDnDDataEncoder00 MY_ANSIARGS((EZ_Widget *widget, Atom type, unsigned int tag,
					     EZ_DnDEncoder encoder, void *edata,
					     EZ_CallBack callback, void *cdata));
void EZ_WidgetDeleteDnDDataEncoder00 MY_ANSIARGS((EZ_Widget *widget, 
						EZ_DnDEncoder encoder, void *edata));
void EZ_WidgetDeleteAllDnDDataEncoders00 MY_ANSIARGS((EZ_Widget *widget));
void EZ_WidgetAddDnDDataDecoder00 MY_ANSIARGS((EZ_Widget *widget,Atom type, unsigned int tag,
					     EZ_DnDDecoder encoder, void *edata,
					     EZ_CallBack callback, void *cdata));
void EZ_WidgetDeleteDnDDataDecoder00 MY_ANSIARGS((EZ_Widget *widget,
						EZ_DnDDecoder decoder, void *edata));
void EZ_WidgetDeleteAllDnDDataDecoders00 MY_ANSIARGS((EZ_Widget *widget));

/**********************************************************************************************/
EZ_DnDDataEncoder *EZ_FindEncoderGivenTarget MY_ANSIARGS((EZ_DnDDataEncoder *list, Atom target));
EZ_DnDDataDecoder *EZ_FindDecoderGivenTarget MY_ANSIARGS((EZ_DnDDataDecoder *list, Atom target));
/**********************************************************************************************/
static void EZ_AddDnDDataEncoderWork MY_ANSIARGS((EZ_DnDDataEncoder **encoders, Atom type, 
						  unsigned int  tag,
						  EZ_DnDEncoder encoder, void *data,
						  EZ_CallBack callback,  void *cdata,
						  void *obj));
static void EZ_DeleteDnDDataEncoderWork MY_ANSIARGS((EZ_DnDDataEncoder **encoders, 
						     EZ_DnDEncoder encoder, void *edata));
static void EZ_DeleteAllDnDDataEncoderWork MY_ANSIARGS((EZ_DnDDataEncoder **encoders));

static void EZ_AddDnDDataDecoderWork  MY_ANSIARGS((EZ_DnDDataDecoder **decoders, Atom type, 
						   unsigned int  tag,
						   EZ_DnDDecoder decoder, void *data,
						   EZ_CallBack callback, void *cdata,
						   void *obj));
static void EZ_DeleteDnDDataDecoderWork MY_ANSIARGS((EZ_DnDDataDecoder **decoders,
						     EZ_DnDDecoder decoder, void *edata));
static void EZ_DeleteAllDnDDataDecoderWork MY_ANSIARGS((EZ_DnDDataDecoder **decoders));

/**********************************************************************************************
 *** 
 ***          Encoders 
 ***
 **********************************************************************************************/

void EZ_ItemAddDnDDataEncoder(item,type,tag,encoder,edata,callback,cdata)
     EZ_Item *item;
     Atom  type;
     unsigned int   tag;
     void  *edata, *cdata;
     EZ_DnDEncoder encoder;
     EZ_CallBack   callback;
{
  if(item)
    {
      EZ_DnDDataEncoder **encoders = &(EZ_ItemDnDDataEncoders(item));
      EZ_AddDnDDataEncoderWork(encoders,type,tag, encoder,edata,callback, cdata, (void *)item);
    }
}
/**********************************************************************************************/
void EZ_ItemDeleteDnDDataEncoder(item, encoder, edata)
     EZ_Item *item;
     EZ_DnDEncoder encoder;
     void *edata;
{
  if(item)
    {
      EZ_DnDDataEncoder **encoders = &(EZ_ItemDnDDataEncoders(item));
      EZ_DeleteDnDDataEncoderWork(encoders, encoder, edata);
    }
}
/**********************************************************************************************/
void EZ_ItemDeleteAllDnDDataEncoders(item)
     EZ_Item *item;
{
  if(item)
    {
      EZ_DnDDataEncoder **encoders = &(EZ_ItemDnDDataEncoders(item));
      EZ_DeleteAllDnDDataEncoderWork(encoders);
    }
}
/**********************************************************************************************/

void EZ_WidgetAddDnDDataEncoder(widget,ttype,tag,encoder,edata,callback,cdata)
     EZ_Widget *widget;
     Atom  ttype;
     unsigned int   tag;
     void  *edata, *cdata;
     EZ_DnDEncoder encoder;
     EZ_CallBack   callback;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = widget;
      switch(type)
	{
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_TEXT:
	  twidget = EZ_TextIText(widget); 
	  break;
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_WORK_AREA:
	  twidget = EZ_WorkAreaIWorkArea(widget);
	  break;
	case EZ_WIDGET_OPTIONAL_ENTRY:
	  twidget = EZ_OptEntryEntry(widget);
	  break;
	default:
	  break;
	}
      {
	EZ_DnDDataEncoder **encoders = &(EZ_WidgetDnDDataEncoders(twidget));
	EZ_AddDnDDataEncoderWork(encoders,ttype,tag,encoder,edata, callback, cdata, (void *)twidget);
	
	if(EZ_WidgetWindow(twidget))
	  { 
	    char msg[40];
	    sprintf(msg, "EZ_DND_SWIN %lx ", (unsigned long)EZ_DummyWindow);
	    XChangeProperty(EZ_Display,
			    EZ_WidgetWindow(twidget),
			    EZ_DND_SWIN_ATOM,
			    XA_STRING, 8,
			    PropModeReplace,
			    (unsigned char *)msg,
			    32);
	  }
      }
    }
}
void EZ_WidgetAddDnDDataEncoder00(widget,ttype,tag,encoder,edata,callback,cdata)
     EZ_Widget *widget;
     Atom  ttype;
     unsigned int   tag;
     void  *edata, *cdata;
     EZ_DnDEncoder encoder;
     EZ_CallBack   callback;
{
  if(widget)
    {
      EZ_Widget *twidget = widget;
      EZ_DnDDataEncoder **encoders = &(EZ_WidgetDnDDataEncoders(twidget));
      EZ_AddDnDDataEncoderWork(encoders,ttype,tag,encoder,edata, callback, cdata, (void *)twidget);
	
      if(EZ_WidgetWindow(twidget))
        { 
          char msg[40];
          sprintf(msg, "EZ_DND_SWIN %lx ", (unsigned long)EZ_DummyWindow);
          XChangeProperty(EZ_Display,
                          EZ_WidgetWindow(twidget),
                          EZ_DND_SWIN_ATOM,
                          XA_STRING, 8,
                          PropModeReplace,
                          (unsigned char *)msg,
                          32);
        }
    }
}
/**********************************************************************************************/
void EZ_WidgetDeleteDnDDataEncoder(widget, encoder, edata)
     EZ_Widget *widget;
     EZ_DnDEncoder encoder;
     void *edata;

{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = widget;
      switch(type)
	{
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_TEXT:
	  twidget = EZ_TextIText(widget); 
	  break;
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_WORK_AREA:
	  twidget = EZ_WorkAreaIWorkArea(widget);
	  break;
	case EZ_WIDGET_OPTIONAL_ENTRY:
	  twidget = EZ_OptEntryEntry(widget);
	  break;
	default:
	  break;
	}
      {
	EZ_DnDDataEncoder **encoders = &(EZ_WidgetDnDDataEncoders(twidget));
	EZ_DeleteDnDDataEncoderWork(encoders, encoder, edata);
	if(EZ_WidgetDnDDataEncoders(twidget) == (EZ_DnDDataEncoder *)NULL)
	  {
	    if(EZ_WidgetWindow(twidget))
	      XDeleteProperty(EZ_Display, EZ_WidgetWindow(twidget), EZ_DND_SWIN_ATOM);
	  }
      }
    }
}
void EZ_WidgetDeleteDnDDataEncoder00(widget, encoder, edata)
     EZ_Widget *widget;
     EZ_DnDEncoder encoder;
     void *edata;

{
  if(widget)
    {
      EZ_Widget *twidget = widget;
      EZ_DnDDataEncoder **encoders = &(EZ_WidgetDnDDataEncoders(twidget));
      EZ_DeleteDnDDataEncoderWork(encoders, encoder, edata);
      if(EZ_WidgetDnDDataEncoders(twidget) == (EZ_DnDDataEncoder *)NULL)
        {
          if(EZ_WidgetWindow(twidget))
            XDeleteProperty(EZ_Display, EZ_WidgetWindow(twidget), EZ_DND_SWIN_ATOM);
        }
    }
}
/**********************************************************************************************/
void EZ_WidgetDeleteAllDnDDataEncoders(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = widget;
      switch(type)
	{
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_TEXT:
	  twidget = EZ_TextIText(widget); 
	  break;
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_WORK_AREA:
	  twidget = EZ_WorkAreaIWorkArea(widget);
	  break;
	case EZ_WIDGET_OPTIONAL_ENTRY:
	  twidget = EZ_OptEntryEntry(widget);
	  break;
	default:
	  break;
	}
      {
	EZ_DnDDataEncoder **encoders = &(EZ_WidgetDnDDataEncoders(twidget));
	EZ_DeleteAllDnDDataEncoderWork(encoders);
	if(EZ_WidgetDnDDataEncoders(twidget) == (EZ_DnDDataEncoder *)NULL)
	  {
	    if(EZ_WidgetWindow(twidget))
	      XDeleteProperty(EZ_Display, EZ_WidgetWindow(twidget), EZ_DND_SWIN_ATOM);
	  }
      }
    }
}
void EZ_WidgetDeleteAllDnDDataEncoders00(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *twidget = widget;
      EZ_DnDDataEncoder **encoders = &(EZ_WidgetDnDDataEncoders(twidget));
      EZ_DeleteAllDnDDataEncoderWork(encoders);
      if(EZ_WidgetDnDDataEncoders(twidget) == (EZ_DnDDataEncoder *)NULL)
        {
          if(EZ_WidgetWindow(twidget))
            XDeleteProperty(EZ_Display, EZ_WidgetWindow(twidget), EZ_DND_SWIN_ATOM);
        }
    }
}
/**********************************************************************************************/
static void EZ_AddDnDDataEncoderWork(encoders,type,tag,encoder,edata, callback,cdata, object)
     EZ_DnDDataEncoder **encoders;
     Atom type;
     unsigned int  tag;
     void  *edata, *cdata;
     EZ_DnDEncoder encoder;
     EZ_CallBack   callback;
     void  *object;
{
  EZ_DnDDataEncoder *tmp = *encoders, *last = NULL;  

  if(encoder == NULL) return;

  while(tmp != NULL)
    {
      if(tmp->type == type && tmp->tag == tag) break;
      last = tmp;
      tmp = tmp->next;
    }
  if(tmp == NULL)
    {
      tmp = (EZ_DnDDataEncoder *)my_malloc(sizeof(EZ_DnDDataEncoder), _DND_DATA_ENCODER);
      if(last != NULL)
	{
	  tmp->next = last->next;
	  last->next = tmp;
	}
      else
	{
	  tmp->next = *encoders;
	  *encoders = tmp;
	}
    }
  tmp->type = type;
  tmp->tag = tag;
  tmp->object = object;
  tmp->data = edata;
  tmp->encoder = encoder;
  (tmp->callback).callback = callback;
  (tmp->callback).data = cdata;
}
/**********************************************************************************************/
static void EZ_DeleteDnDDataEncoderWork(encoders, encoder, edata)
     EZ_DnDDataEncoder **encoders;
     EZ_DnDEncoder encoder;
     void *edata;

{
  EZ_DnDDataEncoder *next, *tmp = *encoders;

  if(encoder == NULL) return;
  
  if(tmp)
    {
      if(tmp->encoder == encoder && tmp->data == edata)
	{
	  *encoders = tmp->next;
	  (void)my_free((char *)tmp);
	}
      else
	{
	  next = tmp->next;
	  while(next)
	    {
	      if(next->encoder == encoder && next->data == edata)
		{
		  tmp->next = next->next;
		  (void)my_free((char *)next);
		  break;
		}
	      tmp = next;
	      next = tmp->next;
	    }
	}
    }
}
/********************************************************************************************/      
static void   EZ_DeleteAllDnDDataEncoderWork(encoders)
     EZ_DnDDataEncoder **encoders;
{
  EZ_DnDDataEncoder *tmp = *encoders;
  
  *encoders = (EZ_DnDDataEncoder *)NULL;
  
  while(tmp)
    {
      EZ_DnDDataEncoder *next = tmp->next;
      (void)my_free((char *)tmp);
      tmp = next;
    }
}
/**********************************************************************************************
 *** 
 ***          Decoders 
 ***
 **********************************************************************************************/
void EZ_ItemAddDnDDataDecoder(item,type,tag,decoder,ddata, callback, cdata)
     EZ_Item *item;
     Atom  type;
     unsigned int   tag;
     void *ddata, *cdata;
     EZ_DnDDecoder  decoder;
     EZ_CallBack callback;
{
  if(item)
    {
      EZ_DnDDataDecoder **decoders = &(EZ_ItemDnDDataDecoders(item));
      EZ_AddDnDDataDecoderWork(decoders,type,tag,decoder,ddata,callback,cdata, (void *)item);
    }
}
/**********************************************************************************************/
void EZ_ItemDeleteDnDDataDecoder(item, decoder, edata)
     EZ_Item *item;
     EZ_DnDDecoder decoder;
     void *edata;
{
  if(item)
    {
      EZ_DnDDataDecoder **decoders = &(EZ_ItemDnDDataDecoders(item));
      EZ_DeleteDnDDataDecoderWork(decoders, decoder, edata);
    }
}
/**********************************************************************************************/
void EZ_ItemDeleteAllDnDDataDecoders(item)
     EZ_Item *item;
{
  if(item)
    {
      EZ_DnDDataDecoder **decoders = &(EZ_ItemDnDDataDecoders(item));
      EZ_DeleteAllDnDDataDecoderWork(decoders);
    }
}
/**********************************************************************************************/

void EZ_WidgetAddDnDDataDecoder(widget,ttype,tag,decoder,ddata,callback,cdata)
     EZ_Widget *widget;
     Atom  ttype;
     unsigned int   tag;
     void  *ddata, *cdata;
     EZ_DnDDecoder  decoder;
     EZ_CallBack callback;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = widget;
      switch(type)
	{
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_TEXT:
	  twidget = EZ_TextIText(widget); 
	  break;
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_WORK_AREA:
	  twidget = EZ_WorkAreaIWorkArea(widget);
	  break;
	case EZ_WIDGET_OPTIONAL_ENTRY:
	  twidget = EZ_OptEntryEntry(widget);
	  break;
	default:
	  break;
	}
      {
	EZ_DnDDataDecoder **decoders = &(EZ_WidgetDnDDataDecoders(twidget));
	EZ_AddDnDDataDecoderWork(decoders,ttype,tag,decoder,ddata,callback,cdata, (void *)twidget);
	EZ_Insert2DnDWidgetList(twidget);	
	
	if(EZ_WidgetWindow(twidget))
	  { 
	    char msg[40];
	    sprintf(msg, "EZ_DND_TWIN %lx ", (unsigned long)EZ_DummyWindow);
	    XChangeProperty(EZ_Display,
			    EZ_WidgetWindow(twidget),
			    EZ_DND_TWIN_ATOM,
			    XA_STRING, 8,
			    PropModeReplace,
			    (unsigned char *)msg,
			    32);
	    EZ_Insert2DnDWindowList(twidget); 	    
	  }
      }	
    }
}
void EZ_WidgetAddDnDDataDecoder00(widget,ttype,tag,decoder,ddata,callback,cdata)
     EZ_Widget *widget;
     Atom  ttype;
     unsigned int   tag;
     void  *ddata, *cdata;
     EZ_DnDDecoder  decoder;
     EZ_CallBack callback;
{
  if(widget)
    {
      EZ_Widget *twidget = widget;
      EZ_DnDDataDecoder **decoders = &(EZ_WidgetDnDDataDecoders(twidget));
      EZ_AddDnDDataDecoderWork(decoders,ttype,tag,decoder,ddata,callback,cdata, (void *)twidget);
      EZ_Insert2DnDWidgetList(twidget);	
	
      if(EZ_WidgetWindow(twidget))
        { 
          char msg[40];
          sprintf(msg, "EZ_DND_TWIN %lx ", (unsigned long)EZ_DummyWindow);
          XChangeProperty(EZ_Display,
                          EZ_WidgetWindow(twidget),
                          EZ_DND_TWIN_ATOM,
                          XA_STRING, 8,
                          PropModeReplace,
                          (unsigned char *)msg,
                          32);
          EZ_Insert2DnDWindowList(twidget); 	    
        }
    }
}


/**********************************************************************************************/	
void EZ_WidgetDeleteDnDDataDecoder(widget,decoder, edata)
     EZ_Widget *widget;
     EZ_DnDDecoder decoder;
     void *edata;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = widget;
      switch(type)
	{
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_TEXT:
	  twidget = EZ_TextIText(widget); 
	  break;
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_WORK_AREA:
	  twidget = EZ_WorkAreaIWorkArea(widget);
	  break;
	case EZ_WIDGET_OPTIONAL_ENTRY:
	  twidget = EZ_OptEntryEntry(widget);
	  break;
	default:
	  break;
	}
      {
	EZ_DnDDataDecoder **decoders = &(EZ_WidgetDnDDataDecoders(twidget));
	EZ_DeleteDnDDataDecoderWork(decoders,decoder, edata);
	if(EZ_WidgetDnDDataDecoders(twidget) == (EZ_DnDDataDecoder *)NULL)
	  {
	    EZ_RemveWidgetFromDnDList(twidget);
	    if(EZ_WidgetWindow(twidget))
	      {
		EZ_RemoveFromDnDWindowList(twidget); /* 5-20-97 */
		XDeleteProperty(EZ_Display, EZ_WidgetWindow(twidget), EZ_DND_TWIN_ATOM);
	      }
	  }
      }
    }
}
void EZ_WidgetDeleteDnDDataDecoder00(widget,decoder, edata)
     EZ_Widget *widget;
     EZ_DnDDecoder decoder;
     void *edata;
{
  if(widget)
    {
      EZ_Widget *twidget = widget;
      EZ_DnDDataDecoder **decoders = &(EZ_WidgetDnDDataDecoders(twidget));
      EZ_DeleteDnDDataDecoderWork(decoders,decoder, edata);
      if(EZ_WidgetDnDDataDecoders(twidget) == (EZ_DnDDataDecoder *)NULL)
        {
          EZ_RemveWidgetFromDnDList(twidget);
          if(EZ_WidgetWindow(twidget))
            {
              EZ_RemoveFromDnDWindowList(twidget); /* 5-20-97 */
              XDeleteProperty(EZ_Display, EZ_WidgetWindow(twidget), EZ_DND_TWIN_ATOM);
            }
        }
    }
}
/**********************************************************************************************/
void EZ_WidgetDeleteAllDnDDataDecoders(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      int type = EZ_WidgetType(widget);
      EZ_Widget *twidget = widget;
      switch(type)
	{
	case EZ_WIDGET_LIST_BOX:
	  twidget = EZ_ListBoxIListBox(widget);
	  break;
	case EZ_WIDGET_TEXT:
	  twidget = EZ_TextIText(widget); 
	  break;
	case EZ_WIDGET_TREE:
	  twidget = EZ_TreeWITree(widget);
	  break;
	case EZ_WIDGET_FANCY_LIST_BOX:
	  twidget = EZ_FListBoxIFListBox(widget);
	  break;
	case EZ_WIDGET_WORK_AREA:
	  twidget = EZ_WorkAreaIWorkArea(widget);
	  break;
	case EZ_WIDGET_OPTIONAL_ENTRY:
	  twidget = EZ_OptEntryEntry(widget);
	  break;
	default:
	  break;
	}
      {
	EZ_DnDDataDecoder **decoders = &(EZ_WidgetDnDDataDecoders(twidget));
	EZ_DeleteAllDnDDataDecoderWork(decoders);
	EZ_RemveWidgetFromDnDList(twidget);
	if(EZ_WidgetWindow(twidget))
	  {
	    EZ_RemoveFromDnDWindowList(twidget); /* 5-20-97 */
	    XDeleteProperty(EZ_Display, EZ_WidgetWindow(twidget), EZ_DND_TWIN_ATOM);
	  }
      }
    }
}
void EZ_WidgetDeleteAllDnDDataDecoders00(widget)
     EZ_Widget *widget;
{
  if(widget)
    {
      EZ_Widget *twidget = widget;
      EZ_DnDDataDecoder **decoders = &(EZ_WidgetDnDDataDecoders(twidget));
      EZ_DeleteAllDnDDataDecoderWork(decoders);
      EZ_RemveWidgetFromDnDList(twidget);
      if(EZ_WidgetWindow(twidget))
        {
          EZ_RemoveFromDnDWindowList(twidget); /* 5-20-97 */
          XDeleteProperty(EZ_Display, EZ_WidgetWindow(twidget), EZ_DND_TWIN_ATOM);
        }
    }
}
/**********************************************************************************************/

static void EZ_AddDnDDataDecoderWork(decoders,type,tag,decoder,ddata, callback, cdata, object)
     EZ_DnDDataDecoder **decoders;
     Atom type;
     unsigned int  tag;
     void  *ddata, *cdata;
     EZ_DnDDecoder  decoder;
     EZ_CallBack callback;
     void  *object;
{
  EZ_DnDDataDecoder *tmp = *decoders, *last = NULL;  

  if(decoder == NULL) return;

  while(tmp != NULL)
    {
      if(tmp->type == type && tmp->tag == tag) break;
      last = tmp;
      tmp = tmp->next;
    }
  if(tmp == NULL)
    {
      tmp = (EZ_DnDDataDecoder *)my_malloc(sizeof(EZ_DnDDataDecoder), _DND_DATA_ENCODER);
      if(last != NULL)
	{
	  tmp->next = last->next;
	  last->next = tmp;
	}
      else
	{
	  tmp->next = *decoders;
	  *decoders = tmp;
	}
    }
  tmp->type = type;
  tmp->tag =  tag;
  tmp->object = object;
  tmp->data = ddata;
  tmp->decoder = decoder;
  (tmp->callback).callback = callback;
  (tmp->callback).data = cdata;
}

static void EZ_DeleteDnDDataDecoderWork(decoders,decoder, edata)
     EZ_DnDDataDecoder **decoders;
     EZ_DnDDecoder decoder;     
     void *edata;
{
  EZ_DnDDataDecoder *next, *tmp = *decoders;

  if(decoder == NULL) return;  

  if(tmp)
    {
      if(tmp->decoder == decoder && tmp->data == edata)
	{
	  *decoders = tmp->next;
	  (void)my_free((char *)tmp);
	}
      else
	{
	  next = tmp->next;
	  while(next)
	    {
	      if(next->decoder == decoder && next->data == edata)
		{
		  tmp->next = next->next;
		  (void)my_free((char *)next);
		  break;
		}
	      tmp = next;
	      next = tmp->next;
	    }
	}
    }
}
/**********************************************************************************************/      

static void   EZ_DeleteAllDnDDataDecoderWork(decoders)
     EZ_DnDDataDecoder **decoders;
{
  EZ_DnDDataDecoder *tmp = *decoders;
  
  *decoders = (EZ_DnDDataDecoder *)NULL;
  
  while(tmp)
    {
      EZ_DnDDataDecoder *next = tmp->next;
      (void)my_free((char *)tmp);
      tmp = next;
    }
}
/**********************************************************************************************/
EZ_DnDDataEncoder *EZ_FindEncoderGivenTarget(list, target)
     EZ_DnDDataEncoder *list; Atom target;
{
  EZ_DnDDataEncoder *tmp = list;

  while(tmp != NULL)
    {
      if(tmp->type == target) return(tmp);
      tmp = tmp->next;
    }
  return(NULL);
}

EZ_DnDDataDecoder *EZ_FindDecoderGivenTarget(list, target)
     EZ_DnDDataDecoder *list; Atom target;
{
  EZ_DnDDataDecoder *tmp = list;
  while(tmp != NULL)
    {
      if(tmp->type == target) return(tmp);
      tmp = tmp->next;
    }
  return(NULL);
}
/**********************************************************************************************/
#undef _EZ_DND_DATA_C_
